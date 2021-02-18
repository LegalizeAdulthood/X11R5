/*
 * $Id: fzk.h,v 1.2 1991/09/16 21:31:34 ohm Exp $
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
/*
 * fzk.h
 */

#define	YOMI_L	8		/* 付属語の読みの長さの最大値 */

#define STRK_L	128		/* 付属語解析のための作業領域の大きさ */
				/* これが、付属語文字列の長さの最大値 */

/* 付属語ベクトルの構造 */
struct	fzkvect	{
	int	no;			/* 付属語ベクトルの番号 */
	int	vector[VECT_L];		/* 付属語ベクトル */
};

/* 付属語エントリーの構造 */
struct	fzkentry {
	w_char	yomi[YOMI_L];		/* 付属語の読みの文字列 */
	int	yomi_su;		/* 付属語の読みの文字列の長さ */
	int	kosu;			/* 付属語ベクトルの個数 */
	struct	fzkvect	*pter;		/* 付属語ベクトルへのポインタ */
	struct	fzkentry *link;		/* 次の付属語エントリーへのポインタ */
};

/* 付属語の検索結果の情報の構造 */
struct	fzkken {
	struct	fzkentry *ent_ptr;	/* 次の付属語エントリーへのポインタ */
	int	vector[VECT_L];		/* 付属語ベクトル */
};

/* 付属語解析のための作業領域の構造 */
struct	fzkwork {
	int	vector[VECT_L];		/* 付属語ベクトル */
};


/*
	structure of FZK TABLE
 */
struct FT {
#ifndef NO_FZK
	struct	fzkvect		*vect_area;
#endif
	struct	kangovect	*kango_vect_area;
#ifndef NO_FZK
	struct	fzkentry	*tablefuzokugo;
#endif
	int			*kango_hinsi_area;
	int	fzkvect_l;			/* 接続ベクタの長さ * 32 bit */
	int	kango_vect_l;			/* 幹語ベクタの長さ * 32 bit */
#ifndef NO_FZK
	int	fzklength;			/* 付属語数 */
#endif
};
