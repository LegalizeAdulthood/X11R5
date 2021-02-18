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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/commondata.c,v 3.9 91/08/30 21:39:44 kon Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>
     
/* デフォルトのローマ字かな変換用のバッファ */

int defaultContext;
int defaultBushuContext;

/* ローマ字かな変換辞書 */
/*
 * ローマ字かな変換テーブルは１個あれば良いでしょう。複数個必要なので
 * あれば RomeStruct のメンバとして入れておく必要もありましょうが...そ
 * の時はその時で考えましょう。
 */
     
struct RkRxDic *romajidic, *RkOpenRoma();

/*
 * 辞書の名前を入れておく変数
 */

char *kanjidicname[MAX_DICS];
int  nkanjidics = 0;

char *userdicname[MAX_DICS];
int  nuserdics = 0;
char userdicstatus[MAX_DICS]; /* マウントされているかどうかの情報 */

char *bushudicname[MAX_DICS];
int  nbushudics = 0;

char *localdicname[MAX_DICS];
int  nlocaldics = 0;

/*
 * ローマ字かな変換をしつくすかどうかのフラグ
 */

int forceKana = 0;

/*
 * 候補一覧の時に何番目の候補かを表示するかどうかのフラグ
 */

int kCount = 0;

/*
 * 候補一覧時にコールバックを禁止するかどうかのフラグ
 */

int iListCB = 0;

/*
 * irohacheck コマンドによって使われているかとか、
 * irohacheck ないでの verbose を表す値。
 */

int ckverbose = 0;

/*
 * エラーのメッセージを入れておく変数
 */

char *necKanjiError;

/*
 * デバグメッセージを出すかどうかのフラグ
 */

int iroha_debug = 0;
