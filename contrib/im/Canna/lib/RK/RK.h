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

/* @(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RK.h,v 1.4 91/05/21 18:29:06 mako Exp $ */
#ifndef		_RK_h
#define		_RK_h

typedef	struct {
   int		ylen;		/* yomigana no nagasa (in byte) */ 
   int		klen;		/* kanji no nagasa (in byte) */
   int		rownum;		/* row number */
   int		colnum;		/* column number */
   int		dicnum;		/* dic number */
}		RkLex;

typedef	struct {
   int		bunnum;		/* bunsetsu bangou */
   int		candnum;	/* kouho bangou */
   int		maxcand;  	/* sou kouho suu */
   int		diccand;	/* jisho ni aru kouho suu */
   int		ylen;		/* yomigana no nagasa (in byte) */ 
   int		klen;		/* kanji no nagasa (in byte) */
   int		tlen;		/* tango no kosuu */
}		RkStat;
/* romaji/kanakanji henkan code */
#define	RK_XFERBITS	4	/* bit-field width */
#define	RK_XFERMASK	((1<<RK_XFERBITS)-1)
#define	RK_NFER		0	/* muhenkan */
#define	RK_XFER		1	/* hiragana henkan */
#define	RK_HFER		2	/* hankaku henkan */
#define	RK_KFER		3	/* katakana henkan */
#define	RK_ZFER		4	/* zenkaku  henkan */

/* kanakanji henkan */

/* romaji hennkan code */
#define	RK_FLUSH	0x8000	/* flush */
#define	RK_SOKON	0x4000	/* sokuon shori */

/* kanakanji henkan */

/* context */
int		RkInitialize();
void		RkFinalize();
int		RkCreateContext(), RkCloseContext(), RkDuplicateContext();
/* dictionary search path */
int		RkSetDicPath();
int		RkGetDirList();
int		RkGetDicList();
/* mount control */
int		RkMountDic(), RkUnmountDic(), RkRemountDic();
int		RkGetMountList();

/* special effects */
int		RkDefineDic();
int		RkDeleteDic();

/* henkan */
int		RkBgnBun();
int		RkEndBun();

int		RkGoTo(), RkLeft(), RkRight();
int		RkXfer(), RkNfer(), RkNext(), RkPrev();
int		RkResize(), RkEnlarge(), RkShorten();
int		RkStoreYomi();

/* queries */
int		RkGetStat();
int		RkGetYomi();
int		RkGetKanji();
int		RkGetKanjiList();
int		RkGetLex();

struct RkKxGram	*RkOpenGram();
void		RkCloseGram();
int		RkSetGramConj();
unsigned char	*RkGetGramName();
int		RkGetGramNum();
unsigned char	*RkUparseWrec();
unsigned char	*RkParseWrec();

/* romaji conversion */
struct RkRxDic 	*RkOpenRoma();
void		RkCloseRoma();
int		RkMapRoma();
int		RkCvtRoma();
/* code conversion */
int		RkCvtHira(), RkCvtKana(), RkCvtHan(), RkCvtZen(), RkCvtEuc();


#endif	/* _RK_h */
/* don't add stuff after this line */


