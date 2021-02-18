/*
 * $Id: rk_header.h,v 1.2 1991/09/16 21:34:43 ohm Exp $
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
/***********************************************************************
			rk_header.h
						87.11.17  訂 補

	rk_main.c rk_read.c rk_modread.c rk_bltinfn.cの共通ヘッダ。
	中で取っている配列の大きさなどを定義。
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
/*	make時に必要なdefine

	BSD42		BSDにてstrings.hを使用（string.hを使う場合は不要）
	SYSVR2		System Vにて定義域の制限されたtoupper・tolowerを使用
			（なくても動く）
	MVUX		ECLIPSE MVでの運転時にdefine  IKISが自動defineされる

	RKMODPATH="文字列"
			その文字列をモード定義表のサーチパスの環境変数の
			名前にする
	WNNDEFAULT	「@LIBDIR」で標準設定表のあるディレクトリを表せる
			ようにする	
	IKIS		半角仮名の１バイト目を0xA8（デフォルトは0x8E）にする

	この他 デバッグ時は必要に応じて KDSP、CHMDSPをdefine
*/

#ifdef MVUX
#	define IKIS
#endif

#include <stdio.h>
#ifdef BSD42
#	include <strings.h>
#	define	strchr	index
#	define	strrchr	rindex
#else
#	include <string.h>
#endif
#include "rk_macros.h"

#define fast register	/* V3.1 */

#define ESCCHR	'\033'
#define	BASEMX	(26 + 10)

#define	LINSIZ	512 /* 対応表の一行の最大サイズ */ /* V3.1 1000 --> 512 */
#define	TRMSIZ	256 /* 対応表の一項目の最大サイズ・
			モード名の最長としても使ってる */ /* V3.1 500 --> 256 */
#define	KBFSIZ	100 /* 本処理バッファのサイズ */ /* V3.1 100 --> 128 */
#ifdef KDSP
#define	DSPLIN	256 /* デバッグ用 */
#endif
#define	OUTSIZ	200 /* 出力バッファのサイズ */ /* V3.1 200 --> 256 */
#define	RSLMAX	10 /* 関数の値として返る文字列の最長 */

	/* rk_modread.cで使うdefine */

#define	REALFN	256 /* 表のフルネームの最大長 */	/* V3.1 200 --> 256 */

#define	MDT1LN	256 /* モード設定リスト1個の最大長 */	/* V3.1 200 -->256 */

#define	ARGMAX	2 /* 条件判断関数の引数の最大個数 */

 /* ディレクトリ名の区切りのdefine（UNIX用）。UNIX以外の環境で使うには
    これと、fixednamep()も変更の必要がある（readmode()のgetenv関係も勿論）。*/
#define	KUGIRI '/'

/* 88/06/07 V3.1 */
#define RK_DEFMODE_MAX_DEF		16	/*  */
#define RK_DEFMODE_MAX_LOT		16	/*  */
#define RK_DEFMODE_MEM_DEF		128	/*  */
#define RK_DEFMODE_MEM_LOT		128	/*  */

#define RK_DSPMODE_MAX_DEF		16	/*  */
#define RK_DSPMODE_MAX_LOT		16	/*  */
#define RK_DSPMODE_MEM_DEF		128	/*  */
#define RK_DSPMODE_MEM_LOT		128	/*  */

#define RK_PATH_MAX_DEF			16	/*  */
#define RK_PATH_MAX_LOT			16	/*  */
#define RK_PATH_MEM_DEF			256	/*  */
#define RK_PATH_MEM_LOT			256	/*  */

#define RK_TAIOUHYO_MAX_DEF		16	/*  */
#define RK_TAIOUHYO_MAX_LOT		16	/*  */
#define RK_TAIOUHYO_MEM_DEF		128	/*  */
#define RK_TAIOUHYO_MEM_LOT		128	/*  */

#define RK_HENSUU_MAX_DEF		32	/*  */
#define RK_HENSUU_MAX_LOT		32	/*  */
#define RK_HENSUU_MEM_DEF		256	/*  */
#define RK_HENSUU_MEM_LOT		256	/*  */
#define RK_HENSUUDEF_MAX_DEF		64	/*  */
#define RK_HENSUUDEF_MAX_LOT		64	/*  */

#define RK_HYO_SIZE_DEF			4096	/*  */

 /* エラー処理用 */
#ifndef		_WNN_SETJMP
#	define	_WNN_SETJMP
#include <setjmp.h>
#endif		/* _WNN_SETJMP */
