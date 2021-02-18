/* @(#)Mode.c	2.2 91/07/05 17:00:30 FUJITSU LIMITED. */
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
static char sccs_id[] = "@(#)Mode.c	2.2 91/07/05 17:00:30 FUJITSU LIMITED.";
#endif

#include <stdio.h>
#include "Xfeoak.h"
#include "TransTbl.h"
typedef unsigned char   Uchar;
typedef unsigned short  Ushort;

int
Fj_InitialMode()
    {
    current_client->romaji_mode = 1;
    current_client->siftmode = FS_HIRAGANA;
    return(0);
    }

int
Fj_ChangeMode(event,keysym,string,length,key_type)
    XEvent      *event;
    KeySym      keysym;
    char	*string;
    int		length;
    int		key_type;
    {
    int		changeMode = 0;
    switch( key_type ) {
    case FK_HIRAGANA:
	if( current_client->siftmode != FS_HIRAGANA ) {
	    current_client->siftmode = FS_HIRAGANA;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    else
		current_client->logical_kanashift = 1;
	    changeMode = 1;
	    }
	break;

    case FK_KATAKANA:
	if( current_client->siftmode != FS_KATAKANA ) {
	    current_client->siftmode = FS_KATAKANA;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    else
		current_client->logical_kanashift = 1;
	    changeMode = 1;
	    }
	break;

    case FK_UPPER_ASCII:
	if( current_client->siftmode != FS_UPPER_ASCII ) {
	    current_client->siftmode = FS_UPPER_ASCII;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    else {
		current_client->logical_kanashift = 0;
		current_client->logical_capslock = 1;
		}
	    changeMode = 1;
	    }
	break;

    case FK_LOWER_ASCII:
	if( current_client->siftmode != FS_LOWER_ASCII ) {
	    current_client->siftmode = FS_LOWER_ASCII;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    else {
		current_client->logical_kanashift = 0;
		current_client->logical_capslock = 0;
		}
	    changeMode = 1;
	    }
	break;

    case FK_ROMA:
	    {
  	    current_client->romaji_mode = current_client->romaji_mode ? 0: 1;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    changeMode = 1;
	    }
	break;

    case FK_ROMA_ON:
  	if( current_client->romaji_mode == 0 ) {
  	    current_client->romaji_mode = 1;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    changeMode = 1;
	    }
	break;

    case FK_ROMA_OFF:
  	if( current_client->romaji_mode != 0 ) {
  	    current_client->romaji_mode = 0;
            if( control_modifiers_flag ) ChangeShiftMode(2);
	    changeMode = 1;
	    }
	break;

    case FK_HANKAKU:
	    {
            DisplayCursor();
  	    current_client->hankakumode = current_client->hankakumode ? 0: 1;
            DisplayCursor();
	    changeMode = 1;
	    }
	break;

    case FK_NIHONGO:
	    {
	    ChangeHenkanMode( (current_client->henkan_mode == 0)? 1: 0 );
	    changeMode = 1;
	    }
	break;

    case FK_NIHONGO_ON:
  	if( current_client->henkan_mode == 0 ) {
	    ChangeHenkanMode(1);
	    changeMode = 1;
	    }
	break;

    case FK_NIHONGO_OFF:
  	if( current_client->henkan_mode != 0 ) {
	    ChangeHenkanMode(0);
	    changeMode = 1;
	    }
	break;

    case FK_KANA_ON:
	if( current_client->physical_kanashift == 0 ) {
	    if( !control_modifiers_flag ) {
		current_client->physical_kanashift = 1;
		}
	    changeMode = 1;
	    }
	break;

    case FK_KANA_OFF:
	if( current_client->physical_kanashift != 0 ) {
	    if( !control_modifiers_flag ) { 
		current_client->physical_kanashift = 0;
		}
	    changeMode = 1;
	    }
	break;
       }

    if( changeMode ) {
    	DisplayStatus( current_client );
	}
    return(0);
    }

Static int
ChangeHenkanMode( mode )
    int 	mode;
    {
    if( mode == 0 ) {	/* off */
        current_client->henkan_mode = 0;
     	if( control_modifiers_flag )  ChangeShiftMode(0);
	UnsetFocusClient();
	}
    else {		/* on */
        current_client->henkan_mode = 1;
     	if( control_modifiers_flag )  ChangeShiftMode(1);
	SetFocusClient();
	}
    return(0);
    }

#define MAX_STATUS_MSG  6+1+2+3+8

int
GetMaxStatusMessageLength( work_client )
    FXimp_Client *work_client;
    {
    return( MAX_STATUS_MSG );
    }

int
GetStatusMessage( work_client, status_message )
    FXimp_Client *work_client;
    Ushort *status_message;
    {
    Uchar *nf_msg, *sm_msg, *sf_msg;

    static Uchar space4[]  = {0xa1,0xa1, 0xa1,0xa1, 0xa1,0xa1, 0xa1,0xa1, 0x0};
#define		space3		&(space4[2])
#define		space1		&(space4[6])
    static Uchar nihongo[]  = {0xc6,0xfc, 0xcb,0xdc, 0xb8,0xec, 0x0};
    static Uchar han[]      = {0xc8,0xbe, 0x0};
    static Uchar hiragana[] = {0xa4,0xd2, 0xa4,0xe9, 0xa4,0xac, 0xa4,0xca, 0x0};
    static Uchar katakana[] = {0xa5,0xab, 0xa5,0xbf, 0xa5,0xab, 0xa5,0xca, 0x0};
    static Uchar eioomozi[] = {0xb1,0xd1, 0xc2,0xe7, 0xca,0xb8, 0xbb,0xfa, 0x0};
    static Uchar eikomozi[] = {0xb1,0xd1, 0xbe,0xae, 0xca,0xb8, 0xbb,0xfa, 0x0};
    static Uchar R_hira[]   = {0xa3,0xd2, 0xa4,0xab, 0xa4,0xca, 0xa1,0xa1, 0x0};
    static Uchar R_kana[]   = {0xa3,0xd2, 0xa5,0xab, 0xa5,0xca, 0xa1,0xa1, 0x0};
    static Uchar off_msg[]  = {0xb1,0xd1, 0xbf,0xf4, 0xca,0xb8, 0xbb,0xfa, 0x0};

    static Uchar *nf_msgtbl[2] = { space3, nihongo };
    static Uchar *sm_msgtbl[2] = { space1, han    };
    static Uchar *sf_msgtbl[4] = { hiragana, katakana, eioomozi, eikomozi };
    static Uchar *sf_msgtb2[4] = { R_hira, R_kana, eioomozi, eikomozi };
    char	work_msg[42];
    int		siftmode;

    switch(work_client->siftmode) {
    case FS_HIRAGANA:	siftmode = 0; break;
    case FS_KATAKANA:	siftmode = 1; break;
    case FS_UPPER_ASCII:siftmode = 2; break;
    case FS_LOWER_ASCII:siftmode = 3; break;
    defult: siftmode = 0;
        }

     /* JIS keybord_type map */
    if(( ( focus_in_client == work_client            ) ||
         ( work_client->style & XIMStatusArea ) )
    && (work_client->henkan_mode != 0)) {	

        if (work_client->romaji_mode) {
            sf_msg = sf_msgtb2[siftmode];
	    if( control_modifiers_flag == 0 && work_client->physical_kanashift != 0 ) {
		if( siftmode > 1 ) {
		    if( work_client->hankakumode )
			siftmode = 1;
		    else
			siftmode = 0;
		    }
                sf_msg = sf_msgtbl[siftmode];
	        }
	    }
	else
            sf_msg = sf_msgtbl[siftmode];
        nf_msg = nf_msgtbl[1];
        sm_msg = sm_msgtbl[work_client->hankakumode];
	}
    else {
#ifdef FUJITSU_SERVER_EXTENSION
	if( control_modifiers_flag ) {
	    siftmode = GetKeyMapMode();
	    }
	else {
#endif /*FUJITSU_SERVER_EXTENSION*/
            if (work_client->physical_kanashift)	
		siftmode = 1;
	    else
		siftmode = 3;
#ifdef FUJITSU_SERVER_EXTENSION
	    }
#endif /*FUJITSU_SERVER_EXTENSION*/
        sf_msg = sf_msgtbl[siftmode];
        nf_msg = nf_msgtbl[0];
        sm_msg = sm_msgtbl[0];
	if( control_modifiers_flag == 0 && focus_in_client != work_client )	/* FocusOut ? */
	    sf_msg = off_msg;
	}

    sprintf( work_msg, "%s %s   %s", nf_msg, sm_msg, sf_msg );
#ifdef DEBUG2
    dbg_printf(" GetStatusMessage (%s) focus_in_client=%x work_client=%x \n",
			work_msg, focus_in_client, work_client );
#endif DEBUG
    return( AsciiToUshort( status_message, work_msg ) );
    }

Static int
AsciiToUshort(out, in)
Ushort	*out;
Uchar	*in;
{
    int		work;
    Ushort	*top_of_out = out;
    while( work = *in++ )
	*out++ = (Ushort)( (work < 0x80) ? work: ((work << 8) | *in++ ) );
    *out = (Ushort)0;
    return( (int)(out - top_of_out) );
}
