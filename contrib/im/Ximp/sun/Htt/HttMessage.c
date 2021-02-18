/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)Message.c	2.1 91/08/13 11:28:36 FUJITSU LIMITED. */
/*
****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Takashi Fujiwara	FUJITSU LIMITED
				fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya		FUJITSU LIMITED
	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_from_sccs_id[] = "@(#)Message.c	2.1 91/08/13 11:28:36 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <stdio.h>

#include "Htt.h"

typedef struct _message {
    int             no;
    char           *kanji_data;
}               Message;

#include "HttMessage.h"

Static char    *
GetMessage(no)
    int             no;
{
    static char     buff[64];
    int             ii;

    for (ii = 0; ii < max_message_no; ii++) {
	if (message[ii].no == no) {
	    return (message[ii].kanji_data);
	}
    }
    sprintf(buff, "Invalid message no. (%d)", no);
    return (buff);
}

void
ErrorMessage(no, a1, a2, a3, a4, a5)
    int             no;
    unsigned long   a1, a2, a3, a4, a5;
{
    char           *form;
    form = GetMessage(no);
    fprintf(stderr, "%s error : ", command_name);
    fprintf(stderr, form, a1, a2, a3, a4, a5);
    fprintf(stderr, "\n");
    fflush(stderr);
}

void
WarningMessage(no, a1, a2, a3, a4, a5)
    int             no;
    unsigned long   a1, a2, a3, a4, a5;
{
    char           *form;
    form = GetMessage(no);
    fprintf(stderr, "%s warning : ", command_name);
    fprintf(stderr, form, a1, a2, a3, a4, a5);
    fprintf(stderr, "\n");
    fflush(stderr);
}
