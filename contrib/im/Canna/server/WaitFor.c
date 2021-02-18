static char sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) WaitFor.c 1.1 90/10/09 16:41:56";

/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */
/*
  This program contains a part of the X server.  The communication part
  of the X server is modified and built into this program.
*/
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* LINTLIBRARY */

#include <errno.h>
#include <stdio.h>

#include <sys/param.h>
#include <signal.h>
#include <sys/time.h>
#include "IR.h"

extern long AllSockets[] ;
extern long AllClients[] ;
extern long LastSelectMask[] ;
extern long WellKnownConnections ;

extern ClientPtr ConnectionTranslation[] ;

extern void CheckConnections();
extern void EstablishNewConnections();

extern int errno;

/*****************
 * WaitForSomething:
 *****************/

WaitForSomething(pClientsReady, nready, pNewClients, nnew)
ClientPtr *pClientsReady;
int *nready;
ClientPtr *pNewClients;
int *nnew;
{
    int i;
    long clientsReadable[mskcnt];
    long curclient;
    int selecterr;

    *nready = 0;
    *nnew = 0;
    CLEARBITS(clientsReadable);

    while (1)
    {
	COPYBITS(AllSockets, LastSelectMask);

	i = select (MAXSOCKS, LastSelectMask,
			(int *) NULL, (int *) NULL, (int *)NULL) ;
	selecterr = errno;

	if (i <= 0) /* An error or timeout occurred */
	{
	    if (i < 0)
		if (selecterr == EBADF)    /* Some client disconnected */
		{
		    CheckConnections () ;
		    if (! ANYSET (AllClients))
			return;
		}
		else if (selecterr != EINTR)
		    PrintMsg("WaitForSomething(): select: errno=%d\n", selecterr) ;
	}
	else
	{
	    /* selectを抜けたＦＤをセットする */
	    MASKANDSETBITS(clientsReadable, LastSelectMask, AllClients) ;

	    /* 接続要求があったか */
	    if (LastSelectMask[0] & WellKnownConnections)
		EstablishNewConnections(pNewClients, nnew) ;

	    /* 要求処理のためにループをぬける */
	    if (*nnew || (ANYSET (clientsReadable)) )
			break;
	}
    }

    if (ANYSET(clientsReadable))
    {
	for (i=0; i<mskcnt; i++)
	{
	    while (clientsReadable[i])
	    {
		curclient = ffs (clientsReadable[i]) - 1;
		pClientsReady[(*nready)++] =
			ConnectionTranslation[curclient + (32 * i)];
		clientsReadable[i] &= ~(1 << curclient);
	    }
	}	
    }
}
