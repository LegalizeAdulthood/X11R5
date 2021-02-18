/*
 * $Id: jikouho.c,v 1.2 1991/09/16 21:31:42 ohm Exp $
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
 *	小文節次候補取りだし			*
 ************************************************/
#include	<stdio.h>
#include	<ctype.h>
#include	"commonhd.h"
#include	"ddefine.h"
#include	"de_header.h"
#include	"jdata.h"

#include	"fzk.h"
#include	"kaiseki.h"
#include "wnn_malloc.h"

extern struct	JKT_SBN		*getjktsbn();
extern struct	JKT_SBN		*find_jktsbn();
extern struct	JKT_SONE	*getjktsone();
extern struct	JKT_SBN		*get_kata_kouho();

int
jkt_sbn(yomi_sno, yomi_eno, endvect, endvect1, tjktsbn, bnst_num, parent)
int	yomi_sno;
int	yomi_eno;
int	endvect;		/* 終端ベクタ */
int	endvect1;		/* 終端ベクタ */
struct	JKT_SBN **tjktsbn;	/* 小文節解析結果 */
int	bnst_num;
struct	JKT_SBN *parent;	/* 親の幹語ノード */
{
    register int	fzkcnt, ii, jktcnt;
    register int	i;
    struct		JKT_SBN		*jktsbn;
    struct		JKT_SBN		*jktsbn_top;
    struct		ICHBNP		*ichbnpbp;
    int		cnt;
    int		gijiflag = 0;

    struct	SYO_BNSETSU	*getsbnsp();
    struct	SYO_BNSETSU	*giji_sbn;
    struct	JKT_SBN		*get_hira_kouho();

    if (yomi_sno == yomi_eno)
	return(NOTHING);

    jktcnt = 0;
    if (fzk_ckvt(endvect) == NO && fzk_ckvt(endvect1) == NO)
	return (NOTHING);
    fzkcnt = fzk_kai(&bun[yomi_sno], &bun[yomi_eno], endvect, endvect1, &ichbnpbp);
    if (fzkcnt <= 0) 
	return (fzkcnt);	/* ERROR */
    for (ii = 0; ii < fzkcnt; ii++)
	getfzkoh(ichbnpbp, ii)->offset += yomi_sno;

    jktsbn_top = 0;

    for (ii = 0; ii < fzkcnt; ii++) {	/* 辞書引き */
	i = getfzkoh(ichbnpbp, ii)->offset;
	if (jmtp[i] == (struct jdata **) UN_KNOWN) { /* もう引いた? */
	    jmt_set(i);
	}
    }

    if ((giji_sbn = getsbnsp()) == NO)
	return (-1);       /* ERROR */

    giji_sbn->bend_m = yomi_sno;
    if (getgiji(yomi_sno, yomi_eno, giji_sbn, ichbnpbp, fzkcnt, bnst_num) >= 0) {
	if ((giji_sbn->j_c == yomi_eno - 1) &&
	    (giji_sbn->hinsi_fk != katakanago_no &&
	     giji_sbn->hinsi_fk != giji_no &&
	     giji_sbn->hinsi_fk != fuzokugo_no)) {
		if (get_zen_giji(giji_sbn, &jktsbn) < 0)
		    return (-1);
		jktsbn->parent = parent;
		jktsbn->kbcnt = bnst_num;
		jktsbn_top = jktsbn;
		gijiflag = 1;
		jktcnt = 1;
	}
    }
    freesbn(giji_sbn);

    if ((cnt = jkt_sbn_one(yomi_sno, yomi_eno, &jktsbn_top, bnst_num, parent,
			   ichbnpbp, fzkcnt)) < 0)
	return (-1);
    jktcnt += cnt;

#ifndef	NO_KANA
    if (gijiflag == 0) { /* ひらがな候補 */
	if ((jktsbn = get_hira_kouho(&jktsbn_top, yomi_sno, yomi_eno))
	     < (struct JKT_SBN *)0)
	    return (-1);
	if (jktsbn > (struct JKT_SBN *)0) {
	    jktsbn->parent = parent;	
	    jktsbn->bend_m = yomi_sno;	
	    jktsbn->kbcnt = bnst_num;
	    jktcnt++;
	}
    }
#endif	/* NO_KANA */

    *tjktsbn = jktsbn_top;
    freeibsp(ichbnpbp);
    return (jktcnt);
}

int
jkt_sbn_one(yomi_sno, yomi_eno, jktsbn_top, bnst_num, parent, ichbnpbp, fzkcnt)
int	yomi_sno;
int	yomi_eno;
struct	JKT_SBN	**jktsbn_top;
int	bnst_num;		/* これまでに解析した小文節数 */
struct	JKT_SBN *parent;	/* 親の幹語ノード */
struct	ICHBNP	*ichbnpbp;
register int	fzkcnt;
{
    register int	ii, hyokaval, hyokaval0, hinsidt, jktcnt;
	     int	hinsidt_fk;	/* 複合品詞のセーブ */
    unsigned short	*hinsi_buf;
    struct	jdata	*jentptr;
    register int	i,		/* 幹語の終わりのインデックス */
			t;
    struct		JKT_SBN		*jktsbn;
    register struct	JKT_SONE	*jktsone;
    int		ll;
    struct		fzkkouho	*fzkptr;

    int			connect_flg;

    jktcnt = 0;

    for (ii = 0; ii < fzkcnt; ii++) { /* 付属語 */
	fzkptr = getfzkoh(ichbnpbp, ii);
	i = fzkptr->offset;
	jentptr = (i > yomi_eno - 1) ?
		(struct jdata *) NULL : (struct jdata *) C(i, yomi_eno - 1);
	for (; jentptr; jentptr = jentptr->jptr) { /* 辞書 */
	    for (t = 0; t < (jentptr->kosuu); t++) { /* 幹語 */
#ifdef	CONVERT_with_SiSheng
		hyokaval = hyoka1(jentptr->hindo[t],
			(jentptr->hindo_in == 0 ? 0 : jentptr->hindo_in[t]),
			diff_sisheng(jentptr->sisheng[t],jentptr->sisheng_int),
			yomi_eno - yomi_sno/2 - i/2,
			dic_table[jentptr->jishono].nice);
#else
		hyokaval = hyoka1(jentptr->hindo[t],
			(jentptr->hindo_in == 0 ? 0 : jentptr->hindo_in[t]),
			yomi_eno - i, yomi_eno - yomi_sno,
			dic_table[jentptr->jishono].nice);
#endif	/* CONVERT_with_SiSheng */
		    hinsidt_fk = hinsidt = jentptr->hinsi[t];
		    if ((ll = wnn_get_fukugou_component_body(hinsidt_fk, &hinsi_buf)) == 0) {
			error1("wnn_get_fukugou_component:erroe in jkt_sbn_one.\n");
			return (-1);
		    }
		    for ( ; ll > 0; ll--) {
			hinsidt = *hinsi_buf;
			hinsi_buf++;
			/* ここから後は単純品詞だけ考えればいい */
			connect_flg = kan_ck_vector(hinsidt, fzkptr->vector);
			if (
			    !(jentptr->hindo_in && (jentptr->hindo_in[t]&0x7f) == 0x7f) &&
			    ((jentptr->hindo[t]&0x7f) != 0x7f) &&
			    /*コメントアウトされた辞書エントリ */
			    (connect_flg != NO ||
			     bnst_num == 1 && kan_ck_vector(hinsidt,
				    getfzkoh1(ichbnpbp, ii)->vector) == WNN_CONNECT_BK)) {

			    if (bnst_num == 1)
				/* endvect に接続できないものは評価値を下げる */
				hyokaval0 = (connect_flg != NO) ?
					hyokaval: _DIVID(hyokaval, 2);
			    else if (parent != 0) {
				/* 前の文節が endvect に接続できないものは、
				   評価値を下げる */
				struct JKT_SONE *tmp;
				hyokaval0 = _DIVID(hyokaval, 2);
				for (tmp = parent->sbn; tmp != 0;
				       tmp = tmp->lnk_br) {
				    if (tmp->status_bkwd == YES) {
					/* 実は接続できた */
					hyokaval0 = hyokaval;
					break;
				    }
				}
			    } else
				hyokaval0 = hyokaval;

			    if ((jktsone = getjktsone()) == NO)
				    return (-1);	/* ERROR */
			    jktsone->i_jc = i;
			    jktsone->v_jc = hyokaval0;
			    jktsone->jentptr = jentptr;
			    jktsone->t_jc = t;
			    jktsone->hinsi_fk = hinsidt_fk;
			    jktsone->status_bkwd = connect_flg;
			    if ((jktsbn =
				find_jktsbn(jktsbn_top, jktsone, hinsidt, yomi_eno - 1)) < (struct JKT_SBN *)0)
				    return (-1);
			    if (jktsbn > (struct JKT_SBN *)0) {
				    jktsbn->parent = parent;	
				    jktsbn->bend_m = yomi_sno;	
				    jktsbn->kbcnt = bnst_num;
				    jktcnt++;
			    }
			} /* 接続できる */
		    } /* 単純品詞 */
		} /* 幹語 */
	    } /* 辞書 */
#ifndef	NO_KANA
	    /* カタカナ候補 */
	    if ((jktsbn = get_kata_kouho(jktsbn_top, yomi_sno, yomi_eno, i,
		fzkptr->vector, getfzkoh1(ichbnpbp, ii)->vector, ii)) < (struct JKT_SBN *)0)
		    return (-1);
	    if (jktsbn > (struct JKT_SBN *)0) {
		    jktsbn->parent = parent;	
		    jktsbn->bend_m = yomi_sno;	
		    jktsbn->kbcnt = bnst_num;
		    jktcnt++;
	    }
#endif	/* NO_KANA */
	} /* 付属語 */
    return (jktcnt);
}

/* カタカナ候補 */
struct JKT_SBN	*
get_kata_kouho(jktsbn_top, yomi_sno, yomi_eno, i_jc, vector, vector1, hyouka)
struct	JKT_SBN **jktsbn_top;
int	yomi_sno,
	yomi_eno,
	i_jc;
int	*vector;
int	*vector1;
int	hyouka;
{
    struct	JKT_SONE	*jktsone;
    extern	struct JKT_SONE	*getjktsone();
    extern	struct JKT_SBN	*find_jktsbn();
    int		connect_flg;

    if (!isjhira(bun[i_jc])) 
	return ((struct JKT_SBN *) 0);

    if (i_jc != yomi_eno && !(touten(bun[i_jc])) && !(kuten(bun[i_jc]))) {
	if (kan_ck_vector(giji_no, vector) == WNN_CONNECT_BK) {
	    connect_flg = WNN_CONNECT_BK;
	} else if (kan_ck_vector(giji_no, vector1) == WNN_NOT_CONNECT_BK) {
	    return ((struct JKT_SBN *) 0);
	} else {
	    connect_flg = WNN_NOT_CONNECT_BK;
	}
	if ((jktsone = getjktsone()) == 0)
	    return ((struct JKT_SBN *) 0);
	jktsone->i_jc = i_jc;
	jktsone->jentptr = 0;
	jktsone->t_jc = WNN_KATAKANA; /* カタカナ */
	jktsone->v_jc = (yomi_eno - i_jc ) * KATA_HYOUKA;
	jktsone->hinsi_fk = giji_no;
	jktsone->status_bkwd = connect_flg;
	return (find_jktsbn(jktsbn_top, jktsone, giji_no, yomi_eno - 1));
    } else {
	return ((struct JKT_SBN *) 0);
    }
}

struct JKT_SBN	*
get_hira_kouho(jktsbn_top, yomi_sno, yomi_eno)
struct	JKT_SBN **jktsbn_top;
int	yomi_sno,
	yomi_eno;
{
    struct	JKT_SONE	*jktsone;
    extern	struct JKT_SONE	*getjktsone();
    extern	struct JKT_SBN	*find_jktsbn();

    if ((jktsone = getjktsone()) == 0)
	    return ((struct JKT_SBN *)-1);
    jktsone->i_jc = yomi_sno;
    jktsone->jentptr = 0;
    jktsone->t_jc = WNN_HIRAGANA;	/* ひらがな候補 */
    jktsone->hinsi_fk = giji_no;
    jktsone->v_jc = (yomi_eno - yomi_sno) * HIRA_HYOUKA;
    jktsone->status_bkwd = WNN_NOT_CONNECT_BK;
    return (find_jktsbn(jktsbn_top, jktsone, giji_no, yomi_eno - 1));
}

struct JKT_SBN *
find_jktsbn(jktsbn, jktsone, hinsi, j_c)
struct	JKT_SBN		**jktsbn;
struct	JKT_SONE	*jktsone;
int			hinsi;
int			j_c;
{
    register struct	JKT_SBN		*sbn;
    struct		JKT_SBN		*tsbn;
    register struct	JKT_SONE	*sone;
    int	v_jc;
    int	kangovect;

    kangovect = ft->kango_hinsi_area[hinsi];
    sbn = *jktsbn;
    if (sbn == 0 || sbn->j_c != j_c || sbn->kangovect > kangovect) {
	if ((sbn = getjktsbn()) == NO)
	    return ((struct JKT_SBN *)-1);
	sbn->lnk_br = *jktsbn;	
	*jktsbn = sbn;
	sbn->j_c = j_c;
	sbn->sbn = jktsone;
	sbn->kangovect = kangovect;
	return(sbn);
    }
    while (sbn != 0 && sbn->j_c == j_c) {
	if (sbn->kangovect == kangovect) {
	    /* 評価値で SORT */
	    v_jc = jktsone->v_jc;
	    sone = sbn->sbn;
	    if (sone == 0 || sone->v_jc < v_jc) {
		sbn->sbn = jktsone;
		jktsone->lnk_br = sone;
		return (0);
	    }
	    while(sone) {
		if (sone->lnk_br == 0 || sone->lnk_br->v_jc < v_jc) {
		    jktsone->lnk_br = sone->lnk_br;
		    sone->lnk_br = jktsone;
		    return (0);
		}
		sone = sone->lnk_br;
	    }
	} else if (sbn->lnk_br == 0 || sbn->lnk_br->j_c != j_c ||
		   sbn->lnk_br->kangovect > kangovect) {
	    if ((tsbn = getjktsbn()) == NO)
		return ((struct JKT_SBN *)-1);
	    tsbn->lnk_br = sbn->lnk_br;
	    sbn->lnk_br = tsbn;
	    tsbn->sbn = jktsone;
	    tsbn->kangovect = kangovect;
	    tsbn->j_c = j_c;
	    return(tsbn);
	}
	sbn = sbn->lnk_br;
    }
    return(NULL);
}

/**************************************************
 * jkt_get_syo : 次候補 all 取りだし 小文節
 **************************************************/
int
#ifdef	NO_FZK
jkt_get_syo(yomi_sno, yomi_eno, beginvect, endvect, endvect1, dsd_sbn)
#else
jkt_get_syo(yomi_sno, yomi_eno, beginvect, fzkchar, endvect, endvect1, dsd_sbn)
#endif	/* NO_FZK */
int	yomi_sno;
int	yomi_eno;
int	beginvect;		/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;		/* 終端ベクタ */
int	endvect1;		/* 終端ベクタ */
struct	DSD_SBN	**dsd_sbn;
{
    struct	DSD_SBN	*d_sbn;
    struct	JKT_SBN *tjktsbn = 0;	/* 小文節解析結果 */
    register int	cnt = 0;	/* 次候補数 */
    register struct	JKT_SBN		*jktsbn;
    struct	JKT_SBN			*next_sbn;
    register struct	JKT_SONE	*sone_p;
    int	sbn_cnt;
    extern	struct DSD_SBN	*get_dsd_sbn();
    extern	int		cmp_dsd_sbn();
    int			divid;

    if(c_env->fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD;
	return(-1);
    }

    ft = (struct FT *)files[c_env->fzk_fid].area;

    if (jkt_sbn(yomi_sno, yomi_eno, endvect, endvect1, &tjktsbn, 1, 
			  (struct JKT_SBN *)0) < 0) {
	init_work_areas();
	return (-1);
    }

    sbn_cnt = cnt_jkt_sbn(tjktsbn);
    if ((*dsd_sbn = d_sbn = get_dsd_sbn(sbn_cnt)) <= (struct DSD_SBN *)0) {
	init_work_areas();
	return (-1);
    }
    for (jktsbn = tjktsbn; jktsbn != 0; ) {
	next_sbn = jktsbn->lnk_br;
	for (sone_p = jktsbn->sbn; sone_p != 0; sone_p = sone_p->lnk_br) {
	    d_sbn->bun_m = jktsbn->bend_m;
	    d_sbn->bun_jc = jktsbn->j_c;
	    d_sbn->i_jc = sone_p->i_jc;
	    d_sbn->jentptr = sone_p->jentptr;
	    d_sbn->t_jc = sone_p->t_jc;
	    d_sbn->hinsi = sone_p->hinsi_fk;
	    d_sbn->kangovect = jktsbn->kangovect;
	    d_sbn->status_bkwd = sone_p->status_bkwd;
	    divid = get_jkt_status(jktsbn->kangovect, beginvect,
#ifndef	NO_FZK
				   fzkchar,
#endif	/* NO_FZK */
				   &(jktsbn->status));
	    d_sbn->v_jc = DIVID_HYOUKA(sone_p->v_jc, divid);
	    if (sone_p->jentptr == 0 && jktsbn->status == WNN_NOT_CONNECT)
		d_sbn->status = WNN_GIJI;
	    else
		d_sbn->status = jktsbn->status;
	    cnt++;
	    d_sbn++;
	}
	freejktsbn(jktsbn);
	jktsbn = next_sbn;
    }
    qsort((char *) *dsd_sbn, cnt, sizeof(struct DSD_SBN), cmp_dsd_sbn);
    return (cnt);
}

int
cmp_dsd_sbn(sbn1, sbn2)
struct	DSD_SBN	*sbn1;
struct	DSD_SBN	*sbn2;
{
    return (sbn2->v_jc - sbn1->v_jc);
}

int
cnt_jkt_sbn(sbn)
register struct JKT_SBN *sbn;
{
    register int    cnt;
    cnt = 0;
    while(sbn) {
	cnt += cnt_sone(sbn->sbn);
	sbn = sbn->lnk_br;
    }
    return (cnt);
}

int
get_zen_giji(giji_sbn, jktsbn)
struct SYO_BNSETSU *giji_sbn;
struct	JKT_SBN	**jktsbn;
{
    int	hinsi;
    hinsi = giji_sbn->hinsi_fk;

    if ((*jktsbn = getjktsbn()) == NULL)
	return (-1);
    if (((*jktsbn)->sbn = getjktsone()) == NULL)
	return (-1);

    (*jktsbn)->j_c = giji_sbn->j_c;
    (*jktsbn)->bend_m = giji_sbn->bend_m;
    (*jktsbn)->kangovect = giji_sbn->kangovect;
    (*jktsbn)->sbn->i_jc = giji_sbn->i_jc;
    (*jktsbn)->sbn->v_jc = giji_sbn->v_jc;
    (*jktsbn)->sbn->jentptr = 0;
    (*jktsbn)->sbn->t_jc = giji_sbn->t_jc;
    (*jktsbn)->sbn->hinsi_fk = giji_sbn->hinsi_fk;
    (*jktsbn)->sbn->status_bkwd = giji_sbn->status_bkwd;

    if ((*jktsbn)->sbn->hinsi_fk == fuzokugo_no)
	return(1);
    if (isascii(bun[(*jktsbn)->sbn->i_jc])) {
	if (hinsi == suuji_no)
	    return (get_suuji_kouho(*jktsbn));
	else if (hinsi == eisuu_no)
	    return (get_eisuu_kouho(*jktsbn));
	else
	    return (get_kigou_kouho(*jktsbn));
    } else {
	return (1);
    }
}

struct JKT_SONE *
cp_sone(d_sone, s_sone)
register struct JKT_SONE *d_sone, *s_sone;
{
    if ((d_sone->lnk_br = getjktsone()) == NO)
	return ((struct JKT_SONE *)-1);
    d_sone = d_sone->lnk_br;
    d_sone->i_jc = s_sone->i_jc;
    d_sone->v_jc = s_sone->v_jc - 10; /* 第1候補より評価値を下げる */
    d_sone->jentptr = 0;
    d_sone->hinsi_fk = s_sone->hinsi_fk;
    d_sone->status_bkwd = s_sone->status_bkwd;
    return (d_sone);
}

/* 半角数字 */
int
get_suuji_kouho(jktsbn)
register struct	JKT_SBN	*jktsbn;
{
    register struct JKT_SONE *jktsone;
    int	hinsi;
    int	len; 

    hinsi = jktsbn->sbn->t_jc;
    len = jktsbn->j_c - jktsbn->sbn->i_jc;
    if (isspace(bun[jktsbn->j_c]) || isjspace(bun[jktsbn->j_c]))
	len--;

    if ((jktsone = cp_sone(jktsbn->sbn, jktsbn->sbn)) < (struct JKT_SONE *)0)
	return (-1);

    if (hinsi != WNN_NUM_HAN) {
	jktsone->t_jc = WNN_NUM_HAN;	/* 半角数字 */
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    if (hinsi != WNN_NUM_ZEN) {
	jktsone->t_jc = WNN_NUM_ZEN;	/* １２３ */
	if (len < 0 && hinsi == WNN_NUM_KAN)
	    return (3);
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    if (hinsi != WNN_NUM_KAN) {
	jktsone->t_jc = WNN_NUM_KAN;	/* 一二三 */
	if (len < 0)
	    return (3);
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    if (hinsi != WNN_NUM_KANSUUJI) {
	jktsone->t_jc = WNN_NUM_KANSUUJI;	/* 百二十三 */
	if (len < 3 && hinsi == WNN_NUM_KANOLD)
	    return (5);
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    if (hinsi != WNN_NUM_KANOLD) {
	jktsone->t_jc = WNN_NUM_KANOLD;	/* 壱百弐拾参 */
	jktsone->v_jc -= 2;	/* 評価値をちょっと下げる */
	if (len < 3)
	    return (5);
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    if (hinsi != WNN_NUM_ZENCAN) {
	jktsone->t_jc = WNN_NUM_ZENCAN;	/* １，２３４ */
	if (hinsi == WNN_NUM_HANCAN)
	    return (7);
	if ((jktsone = cp_sone(jktsone, jktsbn->sbn)) < (struct JKT_SONE *)0)
	    return (-1);
    }
    jktsone->t_jc = WNN_NUM_HANCAN;	/* 1,234 */
    return (7);
}

int
get_eisuu_kouho(jktsbn)
struct	JKT_SBN	*jktsbn;
{
    struct JKT_SONE	*jktsone;
    if ((jktsone = cp_sone(jktsbn->sbn, jktsbn->sbn)) < (struct JKT_SONE *)0)
	return (-1);

    if (jktsbn->sbn->t_jc == WNN_ALP_ZEN)
	jktsone->t_jc = WNN_ALP_HAN;
    else
	jktsone->t_jc = WNN_ALP_ZEN;
    return (2);
}

int
get_kigou_kouho(jktsbn)
struct	JKT_SBN	*jktsbn;
{
    struct JKT_SONE	*jktsone;
    if ((jktsone = cp_sone(jktsbn->sbn, jktsbn->sbn)) < (struct JKT_SONE *)0)
	return (-1);

    if (jktsbn->sbn->t_jc == WNN_KIG_JIS)
	jktsone->t_jc = WNN_KIG_HAN;
    else
	jktsone->t_jc = WNN_KIG_JIS;
    return (2);
}
