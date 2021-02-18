/*
 * $Id: commonhd.h,v 1.4 1991/09/18 06:30:56 proj Exp $
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
/****************
 * Common header 
 ****************/
#include <stdio.h>
#define	JSERVER_VERSION	0x4100	/* minor version */
#define	_SERVER_VERSION	"4.10"

#include "wnnerror.h"

#ifndef JS
typedef  unsigned int  UINT;
typedef  unsigned char UCHAR;
#ifndef	w_char
# define w_char unsigned short
#endif	/* w_char */
#endif /*JS */

#if	defined(CHINESE) || defined(TAIWANESE)
#define	CONVERT_from_TOP
#define	CONVERT_by_STROKE	/* 筆形(Bi Xing) */
#define	CONVERT_with_SiSheng	/* 四声(Si Sheng) */
#define NO_FZK			/* 付属語は、ない */
#define NO_KANA			/* ひらがな(読みと同じ候補)は、ない */
#endif

#ifdef luna
#ifdef uniosu
# ifndef	SYSVR2
#  define	SYSVR2
# endif
# ifndef	TERMINFO
#  define	TERMINFO
# endif
#else	/* if defined(MACH) || defined(uniosb) */
# ifndef	BSD42
#  define	BSD42
# endif
# ifndef	BSD43
#  define	BSD43
# endif
# ifndef	TERMCAP
#  define	TERMCAP
# endif
#endif
#else	/* defined(luna) */
#ifdef sun
# ifndef	BSD42
#  define	BSD42
# endif
# ifndef	TERMCAP
#  define	TERMCAP
# endif
#else	/* sun else */
#ifdef SVR4
# ifndef	SYSVR2
#  define	SYSVR2
# endif
# ifndef	TERMINFO
#  define	TERMINFO
# endif
#else
# ifndef	BSD43
#  define	BSD43
# endif
# ifndef	BSD42
#  define	BSD42
# endif
# ifndef	TERMCAP
#  define	TERMCAP
# endif
#endif
#endif
#endif

#if defined(SYSVR2) || defined(sun)
# define SRAND48
#endif
#if defined(SVR4)
#ifndef F_OK
#define F_OK	0
#endif
#ifndef R_OK
#define R_OK	4
#endif
#endif

#define MAXBUNSETSU	80
#define LIMITBUNSETSU   400
#define MAXJIKOUHO	400

#define J_IUJIS 0
#define J_EUJIS 1
#define J_JIS   2
#define J_SJIS  3

#ifndef	True
#define	True	1
#endif
#ifndef	False
#define	False	0
#endif

#define KANJI(x)	((x) & 0x80)


#define Ctrl(X)		((X) & 0x1f)

#define NEWLINE		Ctrl('J')
#define CR		Ctrl('M')
#define ESC		'\033'

#ifdef luna
#define RUBOUT		0x08	/* BS */
#else
#define RUBOUT		'\177'
#endif
#define SPACE		' '


#define BAR		0xA1BC	/* ー	*/
#define KUTEN_NUM	0xA1A3	/* 。	*/
#define TOUTEN_NUM	0xA1A2	/* 、	*/
#define S_NUM		0xA3B0	/* ０	*/
#define E_NUM		0xA3B9	/* ９	*/
#define S_HIRA		0xA4A1	/* ぁ	*/
#define E_HIRA		0xA4F3	/* ん	*/
#define S_KATA		0xA5A1	/* ァ	*/
#define E_KATA		0xA5F6	/* ヶ	*/
#define S_HANKATA	0x00A1	/* ｡	*/
#define E_HANKATA	0x00DF	/* ﾟ    */

#define HIRAP(X) ((X) >= S_HIRA && (X) <= E_HIRA)
#define KATAP(X) (((X) >= S_KATA && (X) <= E_KATA) || ((X) == BAR))
#define ASCIIP(X) ((X) < 0x7f)
#define KANJIP(X) (!(HIRAP(X) || KATAP(X) || ASCIIP(X)))

#define YOMICHAR(X) ((HIRAP(X)) || \
		     ('0'<=(X)&&'9'>=(X)) || \
		     ('A'<=(X)&&'Z'>=(X)) || \
		     ('a'<=(X)&&'z'>=(X)) || \
		     (BAR == X) \
		    )
#define HIRA_OF(X) ((KATAP(X) && !(BAR == (X)))? ((X) & ~0x0100) : (X))

#ifdef	CONVERT_by_STROKE
# define Q_MARK		'?'
#endif	/* CONVERT_by_STROKE */

#define LENGTHYOMI      256	/* jisho ni touroku suru yomi no nagasa */
#define LENGTHKANJI     256	/* jisho ni touroku suru kanji no nagasa */
#define LENGTHBUNSETSU  264    /* 文節の最大長 */
#define LENGTHCONV      512	/* 変換可能最大文字数 */

#define JISHOKOSUU     20	

#define DIC_RDONLY 1   /* 辞書がリード・オンリーである。*/


/* 複数のファイルにまたがって用いられているバッファサイズの定義 */
#define EXPAND_PATH_LENGTH 256 /* expand_expr()が用いるバッファのサイズ */


#define WNN_FILE_STRING "Ｗｎｎのファイル"

#define WNN_FILE_STRING_LEN 16


#define F_STAMP_NUM 64
#define FILE_ALREADY_READ -2
#define FILE_NOT_READ -3

/*	file ID	*/
/*
  Local Variables:
  kanji-flag: t
  End:
*/

