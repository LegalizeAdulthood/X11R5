/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from  @(#)KeyPress.c	2.1 91/08/13 11:28:31 FUJITSU LIMITED. */
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
static char     derived_from_sccs_id[] = "@(#)KeyPress.c	2.1 91/08/13 11:28:31 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <ctype.h>
#include <X11/Xlib.h>
#define XK_KATAKANA
#include <X11/keysym.h>
#include <X11/Xutil.h>

#include "Htt.h"
#include "HttKanjiTbl.h"
#include "HttTransTbl.h"
extern short    xjp_flag;

int
InitialTranslationTable()
{
    if (keybord_type == FKEY_G_JIS)
	transmode = &default_jis_mode;
    else if (keybord_type == FKEY_G_OYA)
	transmode = &default_oak_mode;
    else
	transmode = &default_non_mode;
    return (0);
}

int 
CurrentKeyPress(event)
    XEvent         *event;
{
    char            string[128];
    KeySym          keysym;
    int             i, count;
    static XComposeStatus statusmode = {0, 0};
    int             event_type;
    int             f_mach;
    FtTransTable   *current_trans_table;

    bzero(string, 128);
    count = XmuLookupKana(event, string, 128, &keysym, &statusmode);
    event_type = event->type;

    if (current_client->henkan_mode == 1) {
	if (focus_in_client != current_client) {
	    SetFocusClient();
	}
	switch (current_client->siftmode) {
	case FS_HIRAGANA:
	    current_trans_table = transmode->hiragana;
	    break;
	case FS_KATAKANA:
	    current_trans_table = transmode->katakana;
	    break;
	case FS_UPPER_ASCII:
	    current_trans_table = transmode->bigascii;
	    break;
	case FS_LOWER_ASCII:
	    current_trans_table = transmode->smallascii;
	    break;
	}

	f_mach = TranslationProc(current_trans_table, event, keysym, string, count);
	if (f_mach == 0
	    && event_type == KeyPress) {
	    if (current_client->select_kanji_mod)
		Beep();
	    else
		SendKeyEvent(event);
	}
    } else {
	if (transmode->henkanoff) {
	    current_trans_table = transmode->henkanoff;
	    f_mach = TranslationProc(current_trans_table, event, keysym, string, count);
	}
	if (event_type == KeyPress)
#ifdef XJP
	    if (current_client->prot_type != FPRT_XJP
		|| IsMode(keysym, event->xkey.state) == 0)
#endif	/* XJP */
		SendKeyEvent(event);
    }

    if (event_type == KeyPress
	&& IsMode(keysym, event->xkey.state)) {
	if (current_client->prot_type == FPRT_XIM) {
	    Fj_ChangeMode(event, keysym, string, count, FK_NIHONGO);
	    if (current_client->henkan_mode == 0)
		XimpCnversionEnd(current_client);
	}
#ifdef XJP
	else if (current_client->prot_type == FPRT_XJP) {
	    if (xjp_flag && keysym == XK_space)
		Fj_ChangeMode(event, keysym, string, count, FK_NIHONGO);
	    else
		SendKeyEvent(event);
	}
#endif	/* XJP */
    }
}

int
TranslationProc(current_trans_table, event, keysym, string, count)
    FtTransTable   *current_trans_table;
    XEvent         *event;
    char           *string;
    KeySym          keysym;
    int             count;
{
    unsigned int    event_state;
    int             event_type;
    unsigned int    f_state;
    KeySym          f_keysym;
    int             jj, f_mach = 0;

    event_state = event->xkey.state & (ControlMask | ShiftMask);
    event_type = event->type;

    for (; current_trans_table != 0; current_trans_table = current_trans_table->next_item) {
	f_state = current_trans_table->state;
	if ((f_state & FtKeyUp) != 0) {
	    if (event_type == KeyRelease) {
		f_state &= ~FtKeyUp;
	    } else
		continue;
	} else if (event_type != KeyPress) {
	    continue;
	}
	if (f_state == AnyModifier
	    || event_state == f_state) {
	    f_keysym = current_trans_table->keysym;
	    if (f_keysym == FT_KANA) {
		if ((keysym < XK_kana_fullstop)
		    || (keysym > XK_semivoicedsound))
		    continue;
	    } else if (f_keysym == FT_ASCII) {
		if ((keysym < XK_space)
		    || (keysym > XK_asciitilde))
		    continue;
	    } else if (keysym != f_keysym) {
		continue;
	    }
	    (*current_trans_table->proc) (event, keysym, string, count, current_trans_table->arg);
	    f_mach = 1;
	}
    }
    return (f_mach);
}

SendKeyEvent(event)
    XEvent         *event;
{
    if (current_client != root_client) {
	if (current_client->prot_type == FPRT_XIM)
	    XimpSendKeyEvent(event);
#ifdef XJP
	else
	    XjpSendKeyEvent(event);
#endif	/* XJP */
    }
}

int
SendBuffByLength(window, input_data, input_length)
    Window          window;
    ushort         *input_data;
    int             input_length;
{
    if (current_client->prot_type == FPRT_XIM)
	return XimpSendBuffByLength(current_client->focus_window,
				    input_data, input_length);
#ifdef XJP
    else
	return XjpSendBuffByLength(current_client->focus_window,
				   input_data, input_length);
#endif	/* XJP */
}

int
Fj_PutAscii(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    int             num;
    if (current_client->select_kanji_mod) {
	num = (int) *string - (int) '0';
	fj_ExecuteSelectKanji(num);
    } else if ((!current_client->romaji_mode)
	       && (current_client->logical_kanashift)) {
	AsciiToKanaInJISkeybord(string, length);
	if (event->xkey.state == 0 && keysym == XK_space)
	    *string = ' ';
	fj_PutKana(string, length);
    } else {
	if ((event->xkey.state == (LockMask | ShiftMask))
	    || (current_client->logical_capslock != 0)) {
	    int             ii;
	    char           *cc;
	    for (ii = length, cc = string; ii > 0; ii--, cc++) {
		if (isupper(*cc))
		    *cc = tolower(*cc);
		else if (islower(*cc))
		    *cc = toupper(*cc);
	    }
	}
	fj_PutAscii(string, length);
    }
}

int
Fj_PutKana(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	if ((keybord_type != FKEY_G_OYA)
	    && ((current_client->siftmode & FS_KANA) != 0)) {
	    int             num = -1;
	    switch ((int) *string) {
	    case 0xc7:
		num = 1;
		break;
	    case 0xcc:
		num = 2;
		break;
	    case 0xb1:
		num = 3;
		break;
	    case 0xb3:
		num = 4;
		break;
	    case 0xb4:
		num = 5;
		break;
	    case 0xb5:
		num = 6;
		break;
	    case 0xd4:
		num = 7;
		break;
	    case 0xd5:
		num = 8;
		break;
	    case 0xd6:
		num = 9;
		break;
	    case 0xdc:
		num = 0;
		break;
	    }
	    fj_ExecuteSelectKanji(num);
	} else
	    Beep();
    } else {
	fj_PutKana(string, length);
    }
}

int
Fj_Henkan(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_NEXT);
    } else {
	fj_Henkan();
    }
}

int
Fj_Muhenkan(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_PREV);
    } else {
	fj_Muhenkan();
    }
}

int
Fj_Maekouho(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_PREV);
    } else {
	fj_Maekouho();
    }
}

int
Fj_HenkanCancel(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_CancelSelectKanji();
    } else {
	fj_HenkanCancel();
    }
}

int
Fj_HenkanExecute(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_ExecuteSelectKanji(current_client->select_kanji_no);
    } else {
	fj_exec();
    }
}

int
Fj_HenkanExecuteLinefeed(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_NEXT);
    } else {
	if (current_client->convert_count != 0) {
	    fj_exec();
	    SendKeyEvent(event);
	} else if (current_client->last_colums == 0) {
	    SendKeyEvent(event);
	} else
	    Beep();
    }
}

int
Fj_CursorMoveTop(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_PREV);
    } else {
	if (current_client->convert_count != 0
	 && current_client->current_colums == current_client->last_colums) {
	    fj_exec();
	    SendKeyEvent(event);
	} else if (current_client->last_colums == 0) {
	    SendKeyEvent(event);
	} else
	    fj_CursorMove(KS_TOP);
    }
}

int
Fj_CursorMoveBottom(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_NEXT);
    } else {
	if (current_client->convert_count != 0
	 && current_client->current_colums == current_client->last_colums) {
	    fj_exec();
	    SendKeyEvent(event);
	} else if (current_client->last_colums == 0) {
	    SendKeyEvent(event);
	} else
	    fj_CursorMove(KS_BOTTOM);
    }
}

int
Fj_CursorMoveFoward(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_FOWARD);
    } else {
	if (current_client->convert_count != 0
	 && current_client->current_colums == current_client->last_colums) {
	    fj_exec();
	    SendKeyEvent(event);
	} else if (current_client->last_colums == 0) {
	    SendKeyEvent(event);
	} else
	    fj_CursorMove(KS_FOWARD);
    }
}

int
Fj_CursorMoveBackward(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	fj_MoveCursorSelectKanji(KS_BACKWARD);
    } else {
	if (current_client->convert_count != 0
	 && current_client->current_colums == current_client->last_colums) {
	    fj_exec();
	    SendKeyEvent(event);
	} else if (current_client->last_colums == 0) {
	    SendKeyEvent(event);
	} else
	    fj_CursorMove(KS_BACKWARD);
    }
}

int
Fj_DeleteChar(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	Beep();
    } else {
	fj_DeleteChar();
    }
}

int
Fj_InsertChar(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	Beep();
    } else {
	fj_InsertChar();
    }
}

int
Fj_SelectKanji(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    if (current_client->select_kanji_mod) {
	Beep();
    } else {
	fj_SelectKanji(arg);
    }
}

int
Fj_SendEvent(event, keysym, string, length, arg)
    XEvent         *event;
    KeySym          keysym;
    char           *string;
    int             length;
    FtArg           arg;
{
    SendKeyEvent(event);
}
