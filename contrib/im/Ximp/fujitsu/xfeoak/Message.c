/* @(#)Message.c	2.2 91/07/05 16:59:44 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

/*  Xfeoak : Input Server (kana-kanji covertion front end)	*/
/*         : Version 2.0					*/

#ifndef lint
static char sccs_id[] = "@(#)Message.c	2.2 91/07/05 16:59:44 FUJITSU LIMITED.";
#endif

#include <stdio.h>

#include "Xfeoak.h"

typedef struct _message {
    int		no;
    char	*kanji_data;
    } Message;

#include "Message.h"

Static char *
GetMessage(no)
int	no;
{
    static char buff[64];
    int ii;

    for(ii=0; ii<max_message_no; ii++) {
	if( message[ii].no == no ) {
	    return( message[ii].kanji_data );
	}
    }
    sprintf(buff, "Invalid message no. (%d)", no );
    return( buff );
}

void
ErrorMessage( no, a1,a2,a3,a4,a5)
    int	    no;
    unsigned long   a1,a2,a3,a4,a5;
    {
    char	*form;
    form = GetMessage( no );
    fprintf( stderr,"%s error : ", command_name );
    fprintf( stderr, form, a1,a2,a3,a4,a5);
    fprintf( stderr,"\n");
    fflush( stderr);
    }

void
WarningMessage(no,a1,a2,a3,a4,a5)
    int	    no;
    unsigned long   a1,a2,a3,a4,a5;
    {
    char	*form;
    form = GetMessage( no );
    fprintf( stderr,"%s warning : ", command_name );
    fprintf( stderr, form, a1,a2,a3,a4,a5);
    fprintf( stderr,"\n");
    fflush( stderr);
    }

