/*
 * $Id: rk_vars.c,v 1.2 1991/09/16 21:34:53 ohm Exp $
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
			rk_vars.c
						87.12. 3  改 正

	プログラム内で二つ以上のファイルにまたがって使う変数を
	まとめて定義している。
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
#ifndef	MULTI
#ifndef OMRON_LIB
#include "rk_header.h"
#endif

char	rk_errstat = 0; /* (error)によってエラーが引き起こされた時1（但し
			   (eofflg)が立っていたため末尾までの強制出力処理が
			   起こった場合は2）になる。romkan_henkan()実行ごとに
			   更新される。これはユーザにも開放。*/

#ifdef OMRON_LIB
static
#endif
int	flags = 0;
 /* 以下のフラグのOR。
	RK_CHMOUT モードチェンジを知らせるかのフラグ 
	RK_KEYACK キーインに対し必ず何か返すかのフラグ 
	RK_DSPNIL romkan_disp(off)modeの返値のデフォルトが空文字列であるかの
		  フラグ。立たなければデフォルトはNULL。互換性のためromkan_
		  init2まではこれが立つ。
	RK_NONISE 偽コードを出さないようにするかのフラグ
	RK_REDRAW Wnnで使う特殊なフラグ。これが立っている場合、romkan_henkan
		  の結果としてdisoutにnisedl（偽物のdel）以後が特殊コードのみ
		  で終わる文字列が返ってきたら、その後ろにREDRAWをつなぐ。Wnn
		  で変換行のredrawをするために必要な措置。
	RK_SIMPLD deleteの動作を単純にするかのフラグ
	RK_VERBOS verboseで起こすかのフラグ
 */

#ifdef OMRON_LIB
static
#endif
jmp_buf	env0;

#ifdef OMRON_LIB
static
#endif
char	*dspmod[2][2] = {NULL, NULL, NULL, NULL};
	 /* 現在及び一つ前の、romkanをそれぞれon・offしている時のモード表示
	    文字列へのポインタ。romkan_dispmode()で返る値はdspmod[0][0]。*/

 
/* 88/06/07 V3.1 */
struct	modetable {
	int	max;
	int	count;
	char	**point;
	int	size;
	char	*org;
	char	*next;
};

#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_defmode;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_dspmode;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_taiouhyo;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_path;


 /* モードの状態。各々1か0の値をとる */
struct	modeswtable {
	int	max;
	int	count;
	int	*point;
};
#ifdef OMRON_LIB
static
#endif
struct	modeswtable	rk_modesw;

 /* モード定義表の読み込みのためのバッファ */
struct	modebuftable {
	int	size;
	char	*org;
};
#ifdef OMRON_LIB
static
#endif
struct	modebuftable	rk_modebuf;

 /* モード定義表の内部形式のデータ */
struct	modenaibutable {
	int	size;
	int	*org;
	int	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	modenaibutable	rk_modenaibu;

 /* 前・本・後処理表のどれが選択されているかのデータ */
struct	usehyotable {
	int	size;
	int	*usemaehyo;
	int	*usehyo;
	int	*useatohyo;
};
#ifdef OMRON_LIB
static
#endif
struct	usehyotable	rk_usehyo;

struct	dat /* 入力・出力・バッファ残り部へのポインタ */
{
	letter	*code[3];
};

 /* 一つの対応表の、変換データ及び各変数の変域の、格納場所へのポインタ。*/
struct	hyo {
	int		hyoshu; 	/* 表の種別(前・本・後処理表) */
	struct	dat	*data;		/* 変換データの開始番地 */
	letter		**hensudef;	/* 変域表の開始番地 */
	int		size;		/* 格納場所のサイズ */ /* V3.1 */
	letter		*memory;	/* 格納場所へのポインタ */ /* V3.1 */
};

struct	hyotable {
	int		size;
	struct	hyo	*point;
};
#ifdef OMRON_LIB
static
#endif
struct	hyotable	rk_hyo;

struct	hyobuftable {
	int	size;
	char	*org;
	char	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	hyobuftable	rk_hyobuf;

struct	henikitable {
	int	size;
	letter	*org;
	letter	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	henikitable	rk_heniki;

struct  hensuset {
	unsigned	regdflg : 1;	/* 既登録の変数を表す */
	unsigned	curlinflg : 1;	/* 現在行に既出の変数を表す */
	letter		*name;
	letter		*range;
};

struct	hensuutable {
	int	max;
	int	count;
	struct	hensuset *point;
	int	size;
	letter	*org;
	letter	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	hensuutable  rk_hensuu;

struct	hensuudeftable {
	int	max;
	int	count;
	letter	**point;
};
#ifdef OMRON_LIB
static
#endif
struct	hensuudeftable	rk_hensuudef;

/* 現在マッチされている変数の番号と、マッチした文字の組を保持。*/
struct	matchpair {
	int	hennum;
	letter	ltrmch;
};
/* 変数のマッチ状況をためておく */
struct	matchtable {
	int	size;
	struct	matchpair *point;
};
#ifdef OMRON_LIB
static
#endif
struct	matchtable	rk_henmatch;
#else	/*!MULTI*/
#include "rk_header.h"
letter  *lptr;
#endif	/*!MULTI*/
