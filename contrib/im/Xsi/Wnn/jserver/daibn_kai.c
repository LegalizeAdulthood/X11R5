/*
 * $Id: daibn_kai.c,v 1.3 1991/09/16 21:31:11 ohm Exp $
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
/************************************************
 *	大文節解析				*
 ************************************************/
#include <stdio.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "kaiseki.h"
#include "fzk.h"
#include "wnn_malloc.h"

extern	struct	BZD *getbzdsp();

static	int	yomi_sno_tmp;
int		_status = 0;
static	int	edagari_hyouka;

int
#ifdef NO_FZK
dbn_kai(yomi_sno, yomi_eno, beginvect, endvect, endvect1, nmax, rbzd)
#else
dbn_kai(yomi_sno, yomi_eno, beginvect, fzkchar, endvect, endvect1, nmax, rbzd)
#endif /* NO_FZK */
int		yomi_sno;	/* 被解析文字開始位置 */
register int	yomi_eno;	/* 被解析文字終了位置（の次）*/
int		beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef NO_FZK
w_char		*fzkchar;	/* 前端の付属語文字列 */
#endif /* NO_FZK */
int		endvect;	/* 終端ベクトル */
int		endvect1;	/* 終端ベクトル */
register int	nmax;
struct	BZD	**rbzd;
 /* struct BZD *p; p = NULL; として &p を使う。ここに大文節候補の
    リストが入る */
{
     /* 大文節候補リストのカレントポインタ */
    int			tmp;
    struct		SYO_BNSETSU	*db_set;
    register struct	SYO_BNSETSU	**sb_que_head;
    struct		BZD		*bzd = 0;
    int			dbncnt = 0;
    int			setflg;
    register struct	SYO_BNSETSU	*sb_one;
    struct		SYO_BNSETSU	*sb_set;
    register struct	SYO_BNSETSU	**sb_que_newcomer;
    extern struct	SYO_BNSETSU	*que_reorder();
    int			divid;
    int			edagari_hyouka_sbn;

    edagari_hyouka = _MININT;
    edagari_hyouka_sbn = _MININT;
    yomi_sno_tmp = yomi_sno;

    sb_que_head = &db_set;
    *sb_que_head = NULL;
    if((tmp = sbn_kai(yomi_sno, yomi_eno, endvect, endvect1, sb_que_head, 1,
			(struct SYO_BNSETSU *)0)) <= 0)
	return(tmp);
    for (sb_one = *sb_que_head; sb_one != 0; sb_one = sb_one->lnk_br)
	sb_one->son_v = sum_hyouka(sb_one);

    while (*sb_que_head != NULL) {
	sb_one = *sb_que_head;
	*sb_que_head = sb_one->lnk_br;
	sb_one->son_v = sum_hyouka(sb_one);
	setflg = 0;
	if (yomi_eno <= sb_one->j_c+1) {
#ifdef NO_FZK
	    divid = get_status(sb_one->kangovect, beginvect, &sb_one->status);
#else
	    divid = get_status(sb_one->kangovect, beginvect, fzkchar, &sb_one->status);
#endif /* NO_FZK */
	    if (beginvect != WNN_ALL_HINSI && bzd != 0 &&
		bzd->j_c == sb_one->j_c &&
		(sb_one->status == WNN_SENTOU && bzd->sbn->status != WNN_SENTOU ||
		 sb_one->status == WNN_CONNECT && bzd->sbn->status != WNN_CONNECT))
		bzd->v_jc = 0;
	    if ((tmp = set_daibnsetu(rbzd, &bzd, sb_one, &setflg, divid)) < 0)
		return (tmp);
	    dbncnt += tmp;
	    if (setflg == 0)
		clr_sbn_node(sb_one);
	} else {
		if (kan_ckvt(sentou_no, sb_one->kangovect) == WNN_CONNECT_BK) {
		    divid = 0;
		    sb_one->status = WNN_SENTOU;
		} else {
/*		    divid = 2; */
		    divid = -1;
		    sb_one->status = WNN_GIJI;
		}
		if ((tmp = set_daibnsetu(rbzd, &bzd, sb_one, &setflg, divid)) < 0)
		    return (tmp);
		dbncnt += tmp;

	    if (nmax > sb_one->kbcnt) {
/****************************/
	    if (_DIVID(edagari_hyouka_sbn, 3) < sb_one->son_v) {
		if(edagari_hyouka_sbn < sb_one->son_v) {
		    edagari_hyouka_sbn = sb_one->son_v;
		}
/****************************/
		sb_que_newcomer = &sb_set;
		*sb_que_newcomer = NULL;
		if ((tmp = sbn_kai(sb_one->j_c+1, yomi_eno, sb_one->kangovect,
				WNN_VECT_NO, sb_que_newcomer, sb_one->kbcnt +1,
				sb_one)) < 0)
		    return (tmp);	/* ERROR */
/****************************/
	    } else {
		tmp = 0;
	    }
/****************************/
		if (tmp > 0) {
		    sb_one->reference += tmp;
		    for (sb_one = *sb_que_newcomer; sb_one != 0;
			     sb_one = sb_one->lnk_br)
			sb_one->son_v = sum_hyouka(sb_one);
		    if (*sb_que_head != 0)
			*sb_que_head = que_reorder(*sb_que_head, *sb_que_newcomer);
		    else
			*sb_que_head = *sb_que_newcomer;
		} else {
		    if (setflg == 0)
			clr_sbn_node(sb_one);
		}
	    } else {
		if (setflg == 0)
		    clr_sbn_node(sb_one);
	    }
	}
    }
/**********/
{
    struct BZD *bzd_sv;
    bzd = *rbzd;
    while (dbncnt > 0 && bzd->v_jc < _DIVID(edagari_hyouka, 2)) {
	*rbzd = bzd->lnk_br;
	bzd->lnk_br = 0;
	clr_node(bzd);
	bzd = *rbzd;
        dbncnt--;
    }
    for (; bzd != 0 && bzd->lnk_br != 0; bzd = bzd->lnk_br) {
	if (bzd->lnk_br->v_jc < _DIVID(edagari_hyouka, 2)) {
	     bzd_sv = bzd->lnk_br->lnk_br;
	     bzd->lnk_br->lnk_br = 0;
	     clr_node(bzd->lnk_br);
	     bzd->lnk_br = bzd_sv;
	     dbncnt--;
	}
    }
}
/**********/
    return (dbncnt);
}

/* 前(beginvect,fzkchar)と接続できるか調べ、
	接続できるとき		1
	大文節の先頭の時	0
	接続できないとき	-1
   を返す
	get_jkt_status 参照
*/
int
#ifdef NO_FZK
get_status(kangovect, beginvect, status)
#else
get_status(kangovect, beginvect, fzkchar, status)
#endif /* NO_FZK */
register int	kangovect;
int	beginvect;
#ifndef NO_FZK
w_char	*fzkchar;
#endif /* NO_FZK */
register short *status;
{
#ifdef NO_FZK
    if (zentan_able(kangovect, beginvect) == YES) {
#else
    if (zentan_able(kangovect, beginvect, fzkchar) == YES) {
#endif /* NO_FZK */
	_status = 2;
	if (beginvect == WNN_ALL_HINSI) {
	    if (kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
		*status = WNN_SENTOU;
/*
	    } else if (jentptr == 0) {
		*status = WNN_GIJI;
		return (-1);
*/
	    } else {
		*status = WNN_NOT_CONNECT;
		return (-1);
	    }
	} else if (beginvect == WNN_BUN_SENTOU) {
		    *status = WNN_SENTOU;
	} else {
		if(
#ifndef	NO_FZK
		    (fzkchar == NULL || *fzkchar == NULL) &&
#endif	/* NO_FZK */
		    beginvect == sentou_no) {
			*status = WNN_SENTOU;
		} else {
			*status = WNN_CONNECT;
			return (1);
		}
	}
    } else if (_status < 2 && kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
	_status = 1;
	*status = WNN_SENTOU;
    } else {
/*
	if (jentptr == 0)
	    *status = WNN_GIJI;
	else
*/
	    *status = WNN_NOT_CONNECT;
	return (-1);
    }
    return (0);
}

/* 文節先頭になれるか */
/* 前端ベクタのチェック */
int
#ifdef NO_FZK
zentan_able(v, hinsi)
#else
zentan_able(v, hinsi, fzkchar)
#endif /* NO_FZK */
int	 v;
register int	hinsi;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef NO_FZK
w_char		*fzkchar;
#endif  /* NO_FZK */
{
#ifndef NO_FZK
    register int	ll;
    unsigned short	*buf;
    struct	ICHBNP	*ichbnpbp;
    w_char		*fzk_buf;
    int			fzkcnt;
    int			fzklen;
    w_char		*rev_fzk();
#endif /* NO_FZK */

    if (hinsi == WNN_ALL_HINSI) {
	return (YES);
    } else if (hinsi == WNN_BUN_SENTOU)  {
	return (kan_ckvt(sentou_no, v));
    }
#ifndef NO_FZK
    else {
	if (fzkchar == NULL || *fzkchar == NULL) {
	    if ((ll = wnn_get_fukugou_component_body(hinsi, &buf)) == 0) {
		    error1("wnn_get_fukugou_component:error in zentan_able.");
		return (-1);
	    }
	    for ( ; ll > 0; ll--) {
		if (kan_ckvt(*buf, v) == WNN_CONNECT_BK)
		    return (YES);
		buf++;
	    }
	} else {
	    fzklen = Strlen(fzkchar);
	    fzk_buf = rev_fzk(fzkchar, fzklen);
	    if (fzk_buf == 0)
		    return(NO);	/* ええかげん */

	    fzkcnt = fzk_kai(fzk_buf, fzk_buf + fzklen, v, WNN_VECT_NO, &ichbnpbp);
	    if ((fzkcnt <= 0) || (getfzkoh(ichbnpbp, fzkcnt-1)->offset != fzklen)) {
		    freeibsp(ichbnpbp);
		    return(NO);
	    }
	    for (ll = wnn_get_fukugou_component_body(hinsi, &buf); ll > 0; ll--) {
		if (kan_ck_vector(*buf, getfzkoh(ichbnpbp, fzkcnt - 1)->vector) == WNN_CONNECT_BK) {
		    freeibsp(ichbnpbp);
		    return (YES);
		}
		buf++;
	    }
	    freeibsp(ichbnpbp);
	}
    }
#endif /* NO_FZK */
    return(NO);
}

#ifndef NO_FZK
w_char *
rev_fzk(fzkchar, len)
register w_char	*fzkchar;
int			len;
{
    static	w_char	*fzk = 0;
    static	int	fzk_len = 0;

    if (fzk_len < len) {
	if(fzk != 0)
	    free(fzk);
	if ((fzk = (w_char *)malloc((len+1)*sizeof(w_char))) == 0) {
	    wnn_errorno = WNN_MALLOC_ERR;
	    fzk_len = 0;
	    return (fzk);
	}
	fzk_len = len;
    }

    (void)Sreverse(fzk, fzkchar);
    return (fzk);
}
#endif /* NO_FZK */

/* 小文節の並び替え 長さとベクタでソートする */
/* que の先頭を返す */
/* 長さもベクタも同じなら評価値の高い方だけにする */
struct	SYO_BNSETSU *
que_reorder(que, new)
register struct	SYO_BNSETSU	*que, *new;
{
    struct		SYO_BNSETSU	*que_sv;
    register struct	SYO_BNSETSU	*q;
    register struct	SYO_BNSETSU	*tmp;
    register struct	SYO_BNSETSU	*next;
    int	flg;

    if (new == 0)
	return(que);
    if ((flg = sbjunjo(que, new)) < 0) {
	if (flg == -2) {
	    if (cmp_hyouka(new, que) > 0) {
		tmp = que->lnk_br;
		next = new->lnk_br;
		clr_sbn_node(que);
		que = new;
		que->lnk_br = tmp;
	    } else {
		next = new->lnk_br;
		clr_sbn_node(new);
	    }
	    que_sv = que;
	    new = next;
	} else
	    que_sv = new;
    } else
	que_sv = que;

    while(new != NULL){
	next = new->lnk_br;
	if ((flg = sbjunjo(que, new)) < 0) {
	    if (flg == -2) {
		if (cmp_hyouka(new, que) > 0) {
		    for (q = que_sv; q->lnk_br != que; q = q->lnk_br);
		    tmp = que->lnk_br;
		    clr_sbn_node(que);
		    que = q->lnk_br = new;
		    que->lnk_br = tmp;
		} else {
		    clr_sbn_node(new);
		}
	    } else {
		tmp = que;
		que = new;
		que->lnk_br = tmp;
	    }
	    new = next;
	    continue;
	}
	while(((flg = sbjunjo(que, new)) > 0) && (que)->lnk_br != NULL)
	    que = (que->lnk_br);
	tmp = que->lnk_br;
	if (flg == -2) {
	    if (cmp_hyouka(new, que) > 0) {
		for (q = que_sv; q->lnk_br != que; q = q->lnk_br);
		clr_sbn_node(que);
		que = q->lnk_br = new;
		    que->lnk_br = tmp;
	    } else {
		clr_sbn_node(new);
	    }
	} else {
	    que->lnk_br = new;
	    new->lnk_br = tmp;
	}
	new = next;
    }
    return (que_sv);
}

/* que と new の順序
	1: que が前
	0: que の後に new
	-1:new が前
	-2:同一順位 */
int
sbjunjo(que, new)
register struct	SYO_BNSETSU	*que, *new;
{
    if (new == 0)
	return (1);
    if (que->j_c > new->j_c)
	return (-1);
    if (que->j_c < new->j_c) {
	if (que->lnk_br == 0)
	    return (0);
	if (que->lnk_br->j_c > new->j_c)
	    return (0);
	if (que->lnk_br->j_c < new->j_c)
	    return (1);
	if (que->lnk_br->kangovect > new->kangovect)
	    return (0);
	return (1);
    }
    if (que->kangovect == new->kangovect)
	return (-2);
    if (que->kangovect > new->kangovect)
	return (-1);
    if (que->lnk_br == 0)
	return (0);
    if (que->lnk_br->j_c > new->j_c)
	return (0);
    if (que->lnk_br->kangovect > new->kangovect)
	return (0);
    return (1);
}


/* 文節の先頭になれれば、大文節の候補をセットする
   bzd に小文節を追加したなら 1 追加しなければ 0 を返す。 */
int
set_daibnsetu(rbzd, bzd, sbn, setflg, divid)
struct		BZD		**rbzd;
register struct	BZD		**bzd;
register struct	SYO_BNSETSU	*sbn;
int				*setflg;
int				divid;
{
    register int	ret;
    int hyouka;

    hyouka = DIVID_HYOUKA(ave_hyouka(sbn), divid);
    if(hyouka > edagari_hyouka)
	edagari_hyouka = hyouka;
    else if(hyouka < _DIVID(edagari_hyouka, 2))
	return (0);

    if (*bzd != 0) {
	if ((*bzd)->j_c == sbn->j_c) {
	    /* 同じ長さ */
	    if ((*bzd)->v_jc >= hyouka)
		return (0);
	    clr_sbn_node((*bzd)->sbn);
	    ret = 0;
	} else {
	    if (((*bzd)->lnk_br = getbzdsp()) == 0)
		return(-1);
	    (*bzd) = (*bzd)->lnk_br;
		ret = 1;
	}
    } else {
	if ((*rbzd = *bzd = getbzdsp()) == 0)
	    return(-1);
	ret = 1;
    }
    (*bzd)->v_jc = hyouka;
    (*bzd)->j_c = sbn->j_c;
    (*bzd)->sbn_cnt = sbn->kbcnt;
    (*bzd)->lnk_br = 0;
    (*bzd)->lnk_son = 0;
    (*bzd)->son_v = 0;
    (*bzd)->sbn = sbn;
    (*bzd)->kbcnt = 1;
    sbn->reference++;
    (*bzd)->bend_m = yomi_sno_tmp;
    *setflg = 1;
    return (ret);
}

/* 大文節の評価関数 とりあえず */

/* sbn に含まれる小文節の評価値の合計 */
int
sum_hyouka(sbn)
register struct	SYO_BNSETSU	*sbn;
{
    return(sbn->v_jc + (sbn->parent ? sbn->parent->son_v : 0));
}

int
ave_hyouka(sbn)
register struct	SYO_BNSETSU	*sbn;
{
    register int	len;		/* 大文節長 */

    len = sbn->j_c - yomi_sno_tmp + 1;
    return (hyoka_dbn(sbn->son_v, sbn->kbcnt, len));
}

/* 大文節の評価値
	長さが同じで、前端ベクタも同じ2つの大文節のどちらを
	選ぶか決めるために使う
	小文節のリスト sbn1 の方が高いと正の値を返す
 */
int
cmp_hyouka(sbn1, sbn2)
register struct	SYO_BNSETSU	*sbn1;
register struct	SYO_BNSETSU	*sbn2;
{
    /* 取りあえず大文節の評価値は、小文節の評価値の平均と考えている */
    return (ave_hyouka(sbn1) - ave_hyouka(sbn2));
}
