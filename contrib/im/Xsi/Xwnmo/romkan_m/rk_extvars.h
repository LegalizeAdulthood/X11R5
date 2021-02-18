/*
 * $Id: rk_extvars.h,v 1.2 1991/09/16 21:34:41 ohm Exp $
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
			rk_extvars.h
						87.11.12  訂 補

	二つ以上のファイルにまたがって使われる変数のextern宣言。
	本体はrk_vars.cで定義。
***********************************************************************/
/*	Version 3.1	88/06/13	H.HASHIMOTO
 */
#ifndef	MULTI
extern	char	rk_errstat;

extern	int	flags;
extern	jmp_buf	env0;

extern	char	*dspmod[][2];

struct	modetable {
	int	max;
	int	count;
	char	**point;
	int	size;
	char	*org;
	char	*next;
};

extern	struct	modetable	rk_defmode;
extern	struct	modetable	rk_dspmode;
extern	struct	modetable	rk_taiouhyo;
extern	struct	modetable	rk_path;

struct	modeswtable {
	int	max;
	int	count;
	int	*point;
};
extern	struct	modeswtable	rk_modesw;

struct	modenaibutable {
	int	size;
	int	*org;
	int	*next;
};
extern	struct	modenaibutable	rk_modenaibu;

struct	modebuftable {
	int	size;
	char	*org;
};
extern	struct	modebuftable	rk_modebuf;

struct	dat {
	letter	*code[3];
};

struct	hyo {
	int		hyoshu;
	struct	dat	*data;
	letter		**hensudef;
	int		size;
	letter		*memory;
};

struct	hyotable {
	int		size;
	struct	hyo	*point;
};
extern	struct	hyotable	rk_hyo;

struct	usehyotable {
	int	size;
	int	*usemaehyo;
	int	*usehyo;
	int	*useatohyo;
};
extern	struct	usehyotable	rk_usehyo;

struct	hyobuftable {
	int	size;
	char	*org;
	char	*next;
};
extern	struct	hyobuftable	rk_hyobuf;

struct	henikitable {
	int	size;
	letter	*org;
	letter	*next;
};
extern	struct	henikitable	rk_heniki;

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
extern	struct	hensuutable	rk_hensuu;

struct	hensuudeftable {
	int	max;
	int	count;
	letter	**point;
};
extern	struct hensuudeftable	rk_hensuudef;

struct	matchpair {
	int	hennum;
	letter	ltrmch;
};
struct	matchtable {
	int	size;
	struct	matchpair *point;
};
extern struct	matchtable	rk_henmatch;
#endif	/* MULTI */

extern	letter *lptr;
