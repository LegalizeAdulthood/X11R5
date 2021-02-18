/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)TransTbl.c	2.1 91/08/13 11:28:41 FUJITSU LIMITED. */
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
static char     derived_from_sccs_id[] = "@(#)TransTbl.c	2.1 91/08/13 11:28:41 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include "Htt.h"
#include "HttTransTbl.h"

#ifndef NULL
#define NULL	0
#endif	/* NULL */


/*
 * next_item tabl entry		state		keysym proc
 * ter
 */

FtTransTable    def_common_func3[] = {
    {&def_common_func3[1],
	NULL, XK_BackSpace,
    Fj_CursorMoveBackward, NULL},
    {&def_common_func3[2],
	NULL, XK_Left,
    Fj_CursorMoveBackward, NULL},
    {&def_common_func3[3],
	NULL, XK_Right,
    Fj_CursorMoveFoward, NULL},
    {&def_common_func3[4],
	NULL, XK_Up,
    Fj_CursorMoveTop, NULL},
    {&def_common_func3[5],
	NULL, XK_Down,
    Fj_CursorMoveBottom, NULL},
    {NULL,
	 /* TAB */ NULL, XK_Tab,
    Fj_CursorMoveBottom, NULL},
};

FtTransTable    def_common_func2[] = {
    {&def_common_func2[1],
	NULL, XK_Kanji,
    Fj_Henkan, NULL},
    {&def_common_func2[2],
	NULL, XK_Muhenkan,
    Fj_Muhenkan, NULL},
    {&def_common_func2[3],
	ControlMask, XK_Kanji,
    Fj_SelectKanji, NULL},
    {&def_common_func2[4],
	ControlMask, XK_Muhenkan,
    Fj_Maekouho, NULL},
    {&def_common_func2[5],
	NULL, XK_Cancel,
    Fj_HenkanCancel, NULL},
    {&def_common_func2[6],
	 /* ESC */ NULL, XK_Escape,
    Fj_HenkanCancel, NULL},
    {&def_common_func2[7],
	 /* DEL */ NULL, XK_Delete,
    Fj_DeleteChar, NULL},
    {&def_common_func2[8],
	NULL, XK_F28,
    Fj_DeleteChar, NULL},
    {&def_common_func2[9],
	NULL, XK_Insert,
    Fj_InsertChar, NULL},
    {&def_common_func2[10],
	ControlMask, XK_Insert,
    Fj_InsertChar, NULL},
    {&def_common_func2[11],
	NULL, XK_Linefeed,
    Fj_HenkanExecuteLinefeed, NULL},
    {&def_common_func2[12],
	NULL, XK_Execute,
    Fj_HenkanExecute, NULL},
    {&def_common_func2[13],
	ControlMask, XK_space,
    Fj_HenkanExecute, NULL},
    {def_common_func3,
	NULL, XK_Henkan_Mode,
    Fj_HenkanExecute, NULL},
};

FtTransTable    def_common_func[] = {
    {&def_common_func[1],
	NULL, FT_ASCII,
    Fj_PutAscii, NULL},
    {&def_common_func[2],
	ShiftMask, FT_ASCII,
    Fj_PutAscii, 1},
    {&def_common_func[3],
	NULL, FT_KANA,
    Fj_PutKana, NULL},
    {def_common_func2,
	ShiftMask, FT_KANA,
    Fj_PutKana, 1},
};


FtTransTable    def_common_mod[] = {
    {&def_common_mod[1],
	NULL, XK_Zenkaku_Hankaku,
    Fj_ChangeMode, FK_HANKAKU},
    {&def_common_mod[2],
	ShiftMask, XK_Romaji,
    Fj_ChangeMode, FK_ROMA},
    {&def_common_mod[3],
	NULL, XK_Kanji,
    Fj_ChangeMode, FK_HIRAGANA},
    {def_common_func,
	NULL, XK_Muhenkan,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_non_henkanoff_mode[] = {
    {&def_non_henkanoff_mode[1],
	NULL, FT_ASCII,
    Fj_ChangeMode, FK_KANA_OFF},
    {&def_non_henkanoff_mode[2],
	ShiftMask, FT_ASCII,
    Fj_ChangeMode, FK_KANA_OFF},
    {&def_non_henkanoff_mode[3],
	FtKeyUp, XK_Kana_Lock,
    Fj_ChangeMode, FK_KANA_OFF},
    {&def_non_henkanoff_mode[4],
	NULL, XK_Kana_Lock,
    Fj_ChangeMode, FK_KANA_ON},
    {&def_non_henkanoff_mode[5],
	NULL, FT_KANA,
    Fj_ChangeMode, FK_KANA_ON},
    {NULL,
	ShiftMask, FT_KANA,
    Fj_ChangeMode, FK_KANA_ON},
};

FtTransTable    def_non_common_mod[] = {
    {&def_non_common_mod[1],
	NULL, XK_F2,
    Fj_ChangeMode, FK_HANKAKU},
    {&def_non_common_mod[2],
	NULL, XK_F3,
    Fj_ChangeMode, FK_ROMA},
    {&def_non_common_mod[3],
	NULL, FT_ASCII,
    Fj_ChangeMode, FK_KANA_OFF},
    {&def_non_common_mod[4],
	ShiftMask, FT_ASCII,
    Fj_ChangeMode, FK_KANA_OFF},
    {&def_non_common_mod[5],
	NULL, FT_KANA,
    Fj_ChangeMode, FK_KANA_ON},
    {&def_non_common_mod[6],
	ShiftMask, FT_KANA,
    Fj_ChangeMode, FK_KANA_ON},

    {&def_non_common_mod[7],
	ControlMask, XK_h,
    Fj_Henkan, NULL},
    {&def_non_common_mod[8],
	ControlMask, XK_m,
    Fj_Muhenkan, NULL},
    {&def_non_common_mod[9],
	ControlMask, XK_w,
    Fj_SelectKanji, NULL},
    {&def_non_common_mod[10],
	ControlMask, XK_p,
    Fj_Maekouho, NULL},
    {&def_non_common_mod[11],
	ControlMask, XK_u,
    Fj_HenkanCancel, NULL},
    {&def_non_common_mod[12],
	ControlMask, XK_d,
    Fj_DeleteChar, NULL},
    {&def_non_common_mod[13],
	ControlMask, XK_i,
    Fj_InsertChar, NULL},
    {&def_non_common_mod[14],
	NULL, XK_Return,
    Fj_HenkanExecuteLinefeed, NULL},
    {&def_non_common_mod[15],
	ControlMask, XK_l,
    Fj_HenkanExecute, NULL},

    {&def_non_common_mod[16],
	ControlMask, XK_b,
    Fj_CursorMoveBackward, NULL},
    {&def_non_common_mod[17],
	ControlMask, XK_f,
    Fj_CursorMoveFoward, NULL},
    {&def_non_common_mod[18],
	ControlMask, XK_a,
    Fj_CursorMoveTop, NULL},
    {def_common_mod,
	ControlMask, XK_e,
    Fj_CursorMoveBottom, NULL},
};

FtTransTable    def_non_hira[] = {
    {&def_non_hira[1],
	NULL, XK_F4,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_non_common_mod,
	NULL, XK_F1,
    Fj_ChangeMode, FK_KATAKANA},
};

FtTransTable    def_non_kana[] = {
    {&def_non_kana[1],
	NULL, XK_F4,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_non_common_mod,
	NULL, XK_F1,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_non_upper[] = {
    {&def_non_upper[1],
	NULL, XK_F4,
    Fj_ChangeMode, FK_LOWER_ASCII},
    {def_non_common_mod,
	NULL, XK_F1,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_non_lower[] = {
    {&def_non_lower[1],
	NULL, XK_F4,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_non_common_mod,
	NULL, XK_F1,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_jis_hira[] = {
    {&def_jis_hira[1],
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_KATAKANA},
};

FtTransTable    def_jis_kana[] = {
    {&def_jis_kana[1],
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_jis_upper[] = {
    {&def_jis_upper[1],
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_LOWER_ASCII},
    {&def_jis_upper[2],
	ControlMask | ShiftMask, XK_Eisu_toggle,
    Fj_ChangeMode, FK_LOWER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_jis_lower[] = {
    {&def_jis_lower[1],
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {&def_jis_lower[2],
	ControlMask | ShiftMask, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_HIRAGANA},
};

FtTransTable    def_oak_hira[] = {
    {&def_oak_hira[1],
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_KATAKANA},
};

FtTransTable    def_oak_kana[] = {
    {def_common_mod,
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
};

FtTransTable    def_oak_upper[] = {
    {&def_oak_upper[1],
	ControlMask, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_LOWER_ASCII},
    {def_common_mod,
	NULL, XK_Hiragana_Katakana,
    Fj_ChangeMode, FK_LOWER_ASCII},
};

FtTransTable    def_oak_lower[] = {
    {&def_oak_lower[1],
	ControlMask, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
    {def_common_mod,
	NULL, XK_Eisu_toggle,
    Fj_ChangeMode, FK_UPPER_ASCII},
};

FtTransMode     default_non_mode =
{def_non_hira, def_non_kana, def_non_upper, def_non_lower,
def_non_henkanoff_mode};

FtTransMode     default_jis_mode =	/* JIS */
{def_jis_hira, def_jis_kana, def_jis_upper, def_jis_lower, NULL};

FtTransMode     default_oak_mode =	/* oyaubi	 */
{def_oak_hira, def_oak_kana, def_oak_upper, def_oak_lower, NULL};

FtTransMode    *transmode = &default_non_mode;
