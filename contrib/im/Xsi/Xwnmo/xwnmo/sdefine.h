/*
 * $Id: sdefine.h,v 1.4 1991/09/16 21:37:18 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*************************
 * define of standard i/o
 *************************/

#ifndef	_SDEFINE_H_
#define	_SDEFINE_H_

#ifndef	MAXNAMLEN
#define	MAXNAMLEN	62
#endif	
#ifndef	MAXPATHLEN
#define	MAXPATHLEN	255
#endif	

#ifndef	MAX_CHARSET
#define	MAX_CHARSET	4
#endif
#ifndef	MAX_ESCAPELEN
#define	MAX_ESCAPELEN	4
#endif
#ifndef	MAX_ESCSLIST
#define	MAX_ESCSLIST	4
#endif

#ifndef	SS2
#define	SS2	0x8e
#endif
#ifndef	SS3
#define	SS3	0x8f
#endif
#ifndef	XK_Eisuu_Lock
#define	XK_Eisuu_Lock	0xFF2E
#endif

#define	PENDING_WCHAR	0x7e7e7e7e

#define MAXCHG 80		/* maximum character can be converted	*/

#define	MAX_PORT_NUMBER		8
#ifdef	CALLBACKS
#define	MAX_SUPPORT_STYLE	7
#else	/* CALLBACKS */
#define	MAX_SUPPORT_STYLE	3
#endif	/* CALLBACKS */

#define	MAX_ICHI_LINES	40
#define	MAX_ICHI_COLUMNS	200
#define	MAX_PAGE_LINES	10

#define	SPACING		4

#define	MAX_BUTTON	5

#define	MAX_LANGS	8	/* Max number of languages */
#define	MAXFONTNMLEN	128	/* Max length of font name */
#define	IN_BORDER	2
#define	MAX_FUNC_STACK	32

#define TBL_CNT 11		/* key_table no kazu */

#define TBL_SIZE 256

#define ESCAPE_CHAR(c)	(((c) < ' ') || ((c) == 127))
#define NORMAL_CHAR(c)	(((c) >= ' ') && ((c) < 127))
#define NOT_NORMAL_CHAR(c)	(((c) >= 128) && ((c) < 256))
#define ONEBYTE_CHAR(c)		(!((c) & ~0xff)) /* added by Nide */

#define KANJI_CHAR(c)		(((c) >= 256 ) && ((c) & 0xff00) != 0x8e00)

#define zenkaku(x)(KANJI_CHAR(x) || ESCAPE_CHAR(x))
#define hankaku(x) (!zenkaku(x))
#define ZENKAKU_HYOUJI(x)     zenkaku(x)
        /* CHANGE AFTERWARD TO BE ABLE TO TREAT hakaku katakana. */

#define MAX_VAL(a , b) (((int)(a) > (int)(b))?(a) : (b))
#define MIN_VAL(a , b) (((int)(a) <= (int)(b))?(a) : (b))

#define numeric(x)	(((x >= S_NUM)&&(x <= E_NUM))? True : NIL)
#define ISKUTENCODE(x)	((x) == 0xa1a3)

#define	XOR(a , b)	((a&(~b)) | ((~a)&b))
#define	malloc	Salloc

#ifdef	XJUTIL
#define	MAX_ICHIRAN_KOSU 36
#define	JISHO_PRIO_DEFAULT	5

struct jisho_ {
    char name[1024];
    char hname[1024];
    int dict_no;
    int prio;
    int rdonly;
};
#endif	/* XJUTIL */

struct kansuu{			/* kansuu_hyo no entry */
    char *kansuumei;
    char *comment;
    int romkan_flag;
    int  (*func[TBL_CNT])();
};

#define MHL0 5

#define MAX_HISTORY 10

#ifdef	XJUTIL
#define	UNDER_LINE_MODE		(0x02 | 0x08 | 0x20)
#endif	/* XJUTIL */

#define OPT_RKFILE		0x01
#define OPT_WNNKEY		0x02
#define OPT_WAKING_UP_MODE	0x04

#define rkfile_defined_by_option	(defined_by_option & OPT_RKFILE)
#define uumkey_defined_by_option	(defined_by_option & OPT_WNNKEY)

/*
  GETOPT string & ALL OPTIONS string for configuration.
  see each config.h for detail.
  NOTE: WHEN YOU MODIFY THESE, YOU ALSO MODIFY do_opt[] ARRAY AND
  ALL config.h!!!!
 */

#define	LANG_JP		"ja_JP.ujis"
#define	LANG_CN		"zh_CN.ugb"
#define	LANG_KR		"ko_KR.euc"
#define	LANG_TW		"zh_TW.big5"
#define	LANG_DE		"de_DE.ct"
#define	LANG_WR		"ja_JP.ujis;zh_CN.ugb;ko_KR.euc;lt_LN.bit8"
#define	DEFAULT_LANG	LANG_JP

#define	XIM_INPUTMETHOD	"_XWNMO"
#define	XIM_JUTIL	"_XIM_JUTIL"
#define	XIM_RCENV	"XIMRC"
#define	USR_XIMRC	"/.ximrc"
#define	XIMRCFILE	"/ximrc"
#define CVTFUNFILE	"/cvt_fun_tbl"
#define CVTMETAFILE	"/cvt_meta_tbl"
#define	XIMMSGFILE	"xim.msg"
#define	DEF_LANG_FOR_XIMMSGFILE		"C"

#define	CWNN_PINYIN	0
#define	CWNN_ZHUYIN	1

#define KENSAKU		0
#define NYUURYOKU	1
#define SENTAKU		2
#define JIKOUHO		3
#define KUTEN_IN	4
#define JIS_IN		5

#define REV_FLAG	(1<<0)
#define BOLD_FLAG	(1<<1)
#define UNDER_FLAG	(1<<2)

#define	MessageOnly	0
#define	YesMessage	1
#define	YesNoMessage	2

#ifndef	XJUTIL
#define	CANCEL_W	0
#define	DELETE_W	1
#define	EXEC_W		1
#define	USE_W		2
#define	MOUSE_W		2
#define	NEXT_W		3
#define	BACK_W		4

#define	XEVENT_TYPE	1
#define	REQUEST_TYPE	2
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
#define	XJP_DIRECT_TYPE	3
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */

#define IsPreeditArea(xc)	(xc->input_style & XIMPreeditArea)
#define IsPreeditCallbacks(xc)	(xc->input_style & XIMPreeditCallbacks)
#define IsPreeditPosition(xc)	(xc->input_style & XIMPreeditPosition)
#define	IsPreeditNothing(xc)	(xc->input_style & XIMPreeditNothing)
#define	IsPreeditNone(xc)	(xc->input_style & XIMPreeditNone)
#define IsStatusArea(xc)	(xc->input_style & XIMStatusArea)
#define IsStatusCallbacks(xc)	(xc->input_style & XIMStatusCallbacks)
#define	IsStatusNothing(xc)	(xc->input_style & XIMStatusNothing)
#define	IsStatusNone(xc)	(xc->input_style & XIMStatusNone)

#define SUPPORT_STYLE_1		(XIMPreeditArea | XIMStatusArea)
#define SUPPORT_STYLE_2		(XIMPreeditPosition | XIMStatusArea)
#define SUPPORT_STYLE_3		(XIMPreeditNothing | XIMStatusNothing)
#ifdef	CALLBACKS
#define SUPPORT_STYLE_4		(XIMPreeditCallbacks | XIMStatusArea)
#define SUPPORT_STYLE_5		(XIMPreeditArea | XIMStatusCallbacks)
#define SUPPORT_STYLE_6		(XIMPreeditPosition | XIMStatusCallbacks)
#define SUPPORT_STYLE_7		(XIMPreeditCallbacks | XIMStatusCallbacks)
#endif  /* CALLBACKS */

/* Change mask */
#define ChClientWindow		(1 << 0)
#define ChFocusWindow		(1 << 1)
#define ChPreeditLocation	(1 << 2)
#define ChPreeditSize		(1 << 3)
#define ChPreColormap		(1 << 4)
#define ChPreStdColormap	(1 << 5)
#define ChPreForeground		(1 << 6)
#define ChPreBackground		(1 << 7)
#define ChPreBackgroundPixmap	(1 << 8)
#define ChPreFontSet		(1 << 9)
#define ChPreLineSpace		(1 << 10)
#define ChPreCursor		(1 << 11)
#define ChStatusLocation	(1 << 12)
#define ChStatusSize		(1 << 13)
#define ChStColormap		(1 << 14)
#define ChStStdColormap		(1 << 15)
#define ChStForeground		(1 << 16)
#define ChStBackground		(1 << 17)
#define ChStBackgroundPixmap	(1 << 18)
#define ChStFontSet		(1 << 19)
#define ChStLineSpace		(1 << 20)
#define ChStCursor		(1 << 21)

#define	ChPreeditAttributes	(ChPreeditLocation | ChPreeditSize |\
				 ChPreColormap | ChPreStdColormap | \
				 ChPreForeground | ChPreBackground | \
				 ChPreBackgroundPixmap | ChPreFontSet | \
				 ChPreLineSpace | ChPreCursor)
#define	ChStatusAttributes	(ChStatusLocation | ChStatusSize |\
				 ChStColormap | ChStStdColormap | \
				 ChStForeground | ChStBackground | \
				 ChStBackgroundPixmap | ChStFontSet | \
				 ChStLineSpace | ChStCursor)

#define	BadIC			1
#define	BadStyle		2
#define	BadClientWindow		3
#define	BadFocusWindow		4
#define	BadLanguage		5
#define	BadSpotLocation		6
#define	BadFontSet		7
#define	AllocError		8
#define	BadSomething		9
#endif	/* !XJUTIL */

#if defined(USING_XJUTIL) || defined(XJUTIL)
#define	XJUTIL_NAME		"xjutil"

#define	XJUTIL_START_PRO	"_XJUTIL_START_PRO_"
#define	XJUTIL_ENV_PRO		"_XJUTIL_ENV_PRO"

#define	DIC_ADD		1
#define	DIC_KENSAKU	2
#define	DIC_ICHIRAN	3
#define	DIC_PARAM	4
#define	DIC_INFO	5
#define	DIC_FZK		6
#define	DIC_TOUROKU	7

#define	DIC_START	1000
#define	DIC_START_ER	1001
#define	DIC_FONTSET_ER	1002

#endif	/* USING_XJUTIL */

#ifdef	XJUTIL
#define	MAX_KEY_BUTTON	3
#define	MAX_ICHI_BUTTON	4
#define	MAX_JU_BUTTON	2

#define	TEXT_WIDTH	60

#define	CANCEL_W	0
#define	EXEC_W		1
#define	NEXT_W		2
#define	BACK_W		3

#define	ICHIRAN_MODE	1
#define	JUTIL_MODE	2

#define	MESSAGE1	0
#define	MESSAGE2	1
#define	MESSAGE3	2
#define	MESSAGE4	3

#define	CANCEL		1
#define	KANJI_IN_START	2
#define	KANJI_IN_END	3
#define	YOMI_IN		4
#define	HINSI_IN	5
#define	JISHO_IN	6
#define	EXECUTE		7
#define	TOUROKU_GO	8
#endif	/* XJUTIL */
#endif	/* _SDEFINE_H_ */
