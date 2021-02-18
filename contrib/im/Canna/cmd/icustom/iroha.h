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
 *
 */

/*
 * @(#) 102.1 $Header: /work/nk.R3.1/cmd/icustom/RCS/iroha.h,v 1.4 91/05/21 18:20:26 hata Exp $
 * $Revision: 1.4 $
 * $Author: hata $
 */

#include <stdio.h>

#define MAX_DICS 16

extern char *kanjidicname[];
extern int  nkanjidics;

extern char *userdicname[];
extern int  nuserdics;
extern char userdicstatus[];

extern char *bushudicname[];
extern int nbushudics;

extern char *localdicname[];
extern int nlocaldics;

/*
 * エラーのメッセージを入れておく変数
 */

extern char *necKanjiError;

/*
 * デバグ文を表示するかどうかのフラグ
 */

extern iroha_debug;

/*
 * 16進コード入力を一覧行に表示するかどうかを調べる条件。
 */

#define hexGLine(plen) (d->ncolumns >= plen + 4)

/*
 * キーシーケンスを発生するようなキー
 */

#define IrohaFunctionKey(key) \
  ((0x80 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x8b) || \
   (0x90 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x9b) || \
   (0xe0 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0xff) )

/*
 * デバグメッセージ出力用のマクロ
 */

#ifdef DEBUG
#define debug_message(fmt, x, y, z)	dbg_msg(fmt, x, y, z)
#else /* !DEBUG */
#define debug_message(fmt, x, y, z)
#endif /* !DEBUG */
