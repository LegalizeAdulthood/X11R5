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
 *
 *	8/16 bit String Manipulations.
 *
 *      "@(#)kanji.h	2.3    88/10/03 10:25:34"
 *      "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/kanji.h,v 1.21 91/05/21 18:03:55 kon Exp $"
 */

#ifndef _X_KANJI_H_
#define _X_KANJI_H_

#ifdef XN_BC /* 旧Ｘ(EWS-UX/V X Window R7.1)完全ソース互換とする時定義 */
#define XLookupKanjiString XoldLookupKanjiString
#endif /* XN_BC */

#include <iroha/jrkanji.h>

/* 以下は入力関係で <iroha/jrkanji.h> で定義されないもの */

typedef struct {
    unsigned char *echoStr;    /* local echo string */
    int length;		        /* length of echo string */
    int revPos;                 /* reverse position  */
    int revLen;                 /* reverse length    */
    unsigned long info;		/* その他の情報 */
    unsigned char *mode;	/* モード情報 */
    struct {
      unsigned char *line;
      int           length;
      int           revPos;
      int           revLen;
    } gline;			/* 一覧表示のための情報 */
} XKanjiStatus;

typedef struct {
  int  val;
  unsigned char *buffer;
  int  bytes_buffer;
  XKanjiStatus *ks;
} XKanjiStatusWithValue;


/* 描画関係の部分 */

typedef enum {single_byte_code, double_byte_code} code_len;

typedef struct {
  code_len nbyte;
  int (*func)();
} fs_mode_rec, *fs_mode;

typedef struct {
    char *chars;		/* pointer to string */
    int nchars;			/* number of characters */
    int delta;			/* delta between strings */
    Font *fids;		        /* font to print it in, None don't change */
    XFontStruct **fonts;        /* font structs. */
    fs_mode mode;
} XKanjiTextItem;


extern int XKanjiTextWidth();
extern int XKanjiTextExtents();
extern int XQueryKanjiTextExtents();
extern int XDrawKanjiText();
extern int XDrawKanjiString();
extern int XDrawImageKanjiString();

extern fs_mode_rec fs_euc_mode[], fs_simple_jis_mode[],
  fs_jis_mode[], fs_jis_euc_mode[],
  fs_euc3_mode[], fs_jis_euc3_mode[],
  fs_nec_euc_mode[];

/* fs_euc_mode -- EUC set
   Index == 0 means the string belongs to  ASCII (or JIS-ROMAN) set.
   Index == 1 means the string belongs to 2 Byte Code set.
   */

/* fs_euc_mode -- EUC set
   Index == 0 means the string belongs to  ISO8859-1 set.
   Index == 1 means the string belongs to 2 Byte Code set.
   Index == 2 means the string belongs to SS2 set. (Katakana)
   */

/* fs_jis_euc3_mode -- JIS and EUC set.
   Index == 0 : ISO8859-1 set.
   Index == 1 : KANJI Code set.  (Allow both JIS & EUC)
   Index == 2 : KATAKANA Code set. (JIS-X0201 7bit Katakana)
   */

/* fs_simple_jis_mode -- Simple JIS set
   Index == 0 : ASCII (or JIS-ROMAN) and KANA (JIS-8bit) set.
   (^[(B, ^[(J, ^[(H and ^[(I)
   Index == 1 : KANJI Code set. (^[$B, ^[$@)
   */

/* fs_jis_mode -- JIS set
   Index == 0 : ASCII set.		(^[(B)
   Index == 1 : UK set.		(^[(A)
   Index == 2 : JIS-ROMAN set.		(^[(J)
   Index == 3 : JIS-KANA set.		(^[(I)
   Index == 4 : obsolete JIS-ROMAN set.(^[(H)
   Index == 5 : JIS-KANJI set.		(^[$B)
   index == 6 : obsolete JIS-KANJI set.(^[$@)
   */

/* fs_jis_euc_mode -- JIS and EUC set.
   Index == 0 : ASCII (or JIS-ROMAN) and KANA (JIS-8bit) set.
   Index == 1 : KANJI Code set.  (Allow both JIS & EUC)
   */

#endif /* _X_KANJI_H_ */
