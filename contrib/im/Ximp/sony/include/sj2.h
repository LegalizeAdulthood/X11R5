/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sj2.h
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 */
/*
 * $Header: sj2.h,v 1.1 91/04/29 17:57:45 masaki Exp $ SONY;
 */

#include "sjparam.h"

/* input mode flag		*/

#define MODE_ZHIRA	1		/* zenkaku hiragana mode	*/
#define MODE_ZKATA	2		/* zenkaku katakana mode	*/
#define MODE_HKATA	3		/* hankaku hiragana mode	*/
#define MODE_ZALPHA	4		/* zenkaku alphabet mode	*/
#define MODE_HALPHA	5		/* hankaku alphabet mode	*/
#define MODE_SCODE	6		/* shift jis code mode		*/
#define MODE_ECODE	7		/* euc code mode		*/
#define MODE_JCODE	8		/* jis code mode		*/
#define MODE_KCODE	9		/* kuten code mode		*/

/* terminal		*/

#define OTHER_KB	0
#define NEWS_KB		1

#define MINCOL	72

/* standard i/o		*/

#define STDIN	0
#define STDOUT	1

/* control characters	*/

#define BS	'\010'
#define ESC	'\033'
#define DEL	'\177'

/* henkan buffering mode	*/

#define UNBUFF	0
#define BUFF	1

/* true end false		*/

#define TRUE	0
#define FALSE	-1

/* word entry			*/

struct	wordent {
		char word_str[MAXWORD];
	};

/*
extern char	*CM,*US,*UE,*SO,*SE,*CE,*CS,*RP,*AP,*TS,*BEL,*FS,*DS,*I2;
extern char	*SC,*RC,*SR,*DO;
extern void	outc(),erroutc();
*/
extern int status_line;

#define aprintf	printf
