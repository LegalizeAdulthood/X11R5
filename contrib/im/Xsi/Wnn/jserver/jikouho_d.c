/*
 * $Id: jikouho_d.c,v 1.2 1991/09/16 21:31:44 ohm Exp $
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
 *	大文節次候補取りだし			*
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

#ifdef	CONVERT_from_TOP
static void sbn_set(), sons_set();
#endif	/* CONVERT_from_TOP */

static void dsd_hyouka(), cnt_sbn(), cnt_dbn();

static	int	yomi_sno_tmp;

/*
 * jkt_get_dai : 次候補 all 取りだし (大文節)
 */
int
#ifdef	NO_FZK
jkt_get_dai(yomi_sno, yomi_eno, beginvect, endvect, endvect1, nmax, dsd_dbn)
#else
jkt_get_dai(yomi_sno, yomi_eno, beginvect, fzkchar, endvect, endvect1, nmax, dsd_dbn)
#endif	/* NO_FZK */
int	yomi_sno;
int	yomi_eno;
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
int	nmax;		/* 1大文節中の最大小文節数 */
struct	DSD_DBN	**dsd_dbn;
{
    register int	i;
    register int	cnt;
    struct JKT_DBN	*rjkt_dbn = 0;
    register struct	DSD_DBN	*dbn;
    int			divid;
    extern int		cmp_dsd_dbn();

    if(c_env->fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD;
	return(-1);
    }
    ft = (struct FT *)files[c_env->fzk_fid].area;
    if ((cnt = jkt_dbn(yomi_sno, yomi_eno, beginvect, 
#ifndef	NO_FZK
			fzkchar,
#endif	/* NO_FZK */
			endvect, endvect1, nmax, &rjkt_dbn)) < 0) {
	init_work_areas();
	return (-1);
    }
    if ((cnt = dbn_set(dsd_dbn, rjkt_dbn)) < 0) {
	init_work_areas();
	return (-1);
    }
    for (dbn = *dsd_dbn, i = cnt; i > 0; i--, dbn++) {
	dsd_hyouka(dbn);
	if ((divid = get_jkt_status(dbn->sbn->kangovect, beginvect,
#ifndef	NO_FZK
				    fzkchar,
#endif	/* NO_FZK */
				    &(dbn->sbn->status))) != 0)
	    dbn->v_jc = DIVID_HYOUKA(dbn->v_jc, divid);
    }
    /* SORT */
    qsort((char *) *dsd_dbn, cnt, sizeof(struct DSD_DBN), cmp_dsd_dbn);
    return (cnt);
}

int
cmp_dsd_dbn(dbn1, dbn2)
struct  DSD_DBN	*dbn1;
struct  DSD_DBN	*dbn2;
{
    return (dbn2->v_jc - dbn1->v_jc);
}

static void
dsd_hyouka(dbn)
struct	DSD_DBN *dbn;
{
    struct	DSD_SBN	*sbn;
    int	len;		/* 大文節長 */
    int	son_v = 0;
    register int	j_c;
    register struct	DSD_SBN	*s_p;
    int	i;

    sbn = dbn->sbn;
    j_c = sbn->bun_jc;
    len = j_c - dbn->bun_m + 1;

    for (i = 0, s_p = sbn; i < dbn->sbncnt ; s_p++, i++) {
	son_v += s_p->v_jc;
    }

    dbn->v_jc = hyoka_dbn(son_v, dbn->sbncnt, len);
}
/*******************/

int
#ifdef	NO_FZK
jkt_dbn(yomi_sno, yomi_eno, beginvect, endvect, endvect1, nmax, rjkt_dbn)
#else
jkt_dbn(yomi_sno, yomi_eno, beginvect, fzkchar, endvect, endvect1, nmax, rjkt_dbn)
#endif	/* NO_FZK */
int	yomi_sno;
int	yomi_eno;
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
register int	nmax;	/* 1大文節中の最大小文節数 */
struct	JKT_DBN	**rjkt_dbn;	/* 次候補解析結果 */
{
    register struct	JKT_SBN	**sb_que_head;	/* 小文節解析結果 */
    int			tmp;		/* 次候補ベクター数 */
    struct		JKT_SBN	*db_tmp;
    register struct	JKT_SBN	*sb_one;
    struct		JKT_SBN	*sbn_tmp;
    register struct	JKT_SBN	**sb_que_newcomer;
    struct		JKT_DBN	*jktdbn = 0;
    extern	struct	JKT_SBN	*jkt_que_reorder();
    int			dbncnt = 0;

    yomi_sno_tmp = yomi_sno;

    sb_que_head = &db_tmp;
    *sb_que_head = NULL;

    if((tmp = zen_sbn(yomi_sno, yomi_eno, endvect, endvect1, sb_que_head, 1,
			(struct JKT_SBN *)0)) <= 0)
	return (tmp);
    while (*sb_que_head != NULL) {
	sb_one = *sb_que_head;
	*sb_que_head = sb_one->lnk_br;
	if(nmax > sb_one->kbcnt && yomi_eno > sb_one->j_c+1) {
	    sb_que_newcomer = &sbn_tmp;
	    *sb_que_newcomer = NULL;
	    tmp = zen_sbn(sb_one->j_c+1, yomi_eno, sb_one->kangovect, WNN_VECT_NO,
			  sb_que_newcomer, sb_one->kbcnt + 1, sb_one);
	    if (tmp < 0) {
		return (-1);	/* ERROR */
	    } else if (tmp > 0) {
		sb_one->reference += tmp;
		if (*sb_que_head != 0)
		    *sb_que_head = jkt_que_reorder(*sb_que_head, *sb_que_newcomer);
		else
		    *sb_que_head = *sb_que_newcomer;
	    } else {
		freejktsbn(sb_one);
	    }
	} else if (yomi_eno == sb_one->j_c+1) {
	    if (set_jkt_dai(rjkt_dbn, &jktdbn, sb_one) < 0)
		return (-1);
	    dbncnt++;
	}
    }
    return (dbncnt);
}

/* 前(beginvect,fzkchar)と接続できるか調べ、
	接続できるとき		1
	大文節の先頭の時	0
	接続できないとき	-1
   を返す
	get_status 参照
*/

int
#ifdef	NO_FZK
get_jkt_status(kangovect, beginvect, status)
#else
get_jkt_status(kangovect, beginvect, fzkchar, status)
#endif	/* NO_FZK */
int kangovect;
int beginvect;
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
short *status;
{
    extern int _status;
    _status = 0;
    return(get_status(kangovect, beginvect,
#ifndef	NO_FZK
		      fzkchar,
#endif	/* NO_FZK */
		      status));
}
/*
int
get_dsd_status(kangovect, beginvect, fzkchar, status)
int	kangovect;
int	beginvect;
w_char	*fzkchar;
int	*status;
{
    if (zentan_able(kangovect, beginvect, fzkchar) == YES) {
	if (beginvect == WNN_ALL_HINSI) {
	    if (kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
		*status = WNN_SENTOU;
	    } else {
		*status = WNN_NOT_CONNECT;
		return (-1);
	    }
	} else if (beginvect == WNN_BUN_SENTOU) {
		*status = WNN_SENTOU;
	} else {
	    if ((fzkchar == NULL || *fzkchar == NULL) && beginvect == sentou_no)
		*status = WNN_SENTOU;
	    else
		*status = WNN_CONNECT;
		return (1);
	}
    } else if (kan_ckvt(sentou_no, kangovect) == WNN_CONNECT_BK) {
	    sb_one->status = WNN_SENTOU;
    } else {
	    *status = WNN_NOT_CONNECT;
	    return (-1);
    }
    return(0);
}
*/

/* 小文節の並び替え 長さとベクタでソートする */
/* que の先頭を返す */
struct	JKT_SBN	 *
jkt_que_reorder(que, new)
register struct	JKT_SBN	*que, *new;
{
	struct		JKT_SBN	*que_sv;
	register struct	JKT_SBN	*tmp;
	register struct	JKT_SBN	*next;

	if (new == 0)
		return (que);
	if (jkt_sbjunjo(que, new) < 0) {
			que_sv = new;
	} else
		que_sv = que;

	while(new != NULL) {
		next = new->lnk_br;
		if (jkt_sbjunjo(que, new) < 0) {
		/* NEW が前 */
			tmp = que;
			que = new;
			que->lnk_br = tmp;
			new = next;
			continue;
		}
		/* QUE が前 */
		while((jkt_sbjunjo(que, new) > 0) && que->lnk_br != NULL)
			que = (que->lnk_br);
		tmp = que->lnk_br;
		/* QUE の後にNEW */
		que->lnk_br = new;
		new->lnk_br = tmp;
		new = next;
	}
	return (que_sv);
}

/* que と new の順序
	1: que が前
	0: que の後に new
	-1:new が前
XXXXX	-2:同一順位 */
int
jkt_sbjunjo(que, new)
struct	JKT_SBN	*que, *new;
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
		return (0); /* return (-2); */
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


/* 文節の先頭になれれば、大文節の候補をセットする */
int
set_jkt_dai(rjkt_dbn, jktdbn, sbn)
struct		JKT_DBN		**rjkt_dbn;
register struct	JKT_DBN		**jktdbn;
register struct	JKT_SBN	*sbn;
{
    struct	JKT_DBN	*getjktdbn();

    if (*jktdbn != 0) {
	if (((*jktdbn)->lnk_br = getjktdbn()) == 0)
	    return(-1);
	(*jktdbn) = (*jktdbn)->lnk_br;
    } else {
	if ((*rjkt_dbn = *jktdbn = getjktdbn()) == 0)
		return(-1);
    }
    (*jktdbn)->j_c = sbn->j_c;
    (*jktdbn)->sbn_cnt = sbn->kbcnt;
    (*jktdbn)->lnk_br = 0;
    (*jktdbn)->sbn = sbn;
    (*jktdbn)->bend_m = yomi_sno_tmp;
    return (0);
}

int
zen_sbn(yomi_sno, yomi_eno, endvect, endvect1, tjktsbn, bnst_num, parent)
int		yomi_sno;
int		yomi_eno;
int		endvect;	/* 終端ベクタ */
int		endvect1;	/* 終端ベクタ */
struct JKT_SBN **tjktsbn;	/* 小文節解析結果 */
int		bnst_num;	/* これまでに解析した小文節数 */
struct JKT_SBN *parent;	/* 親の幹語ノード */
{
    register int	fzkcnt, ii, jktcnt;
    register int	i,		/* 幹語の終わりのインデックス */
			j;		/* 文節始めのインデックス */
    struct		JKT_SBN		*jktsbn;
    struct		JKT_SBN		*jktsbn_top = 0;
    struct		ICHBNP		*ichbnpbp;
    int			index_tmp;
    struct		JKT_SBN		*gijisbn_top;
    struct		fzkkouho	*fzkptr;
    int			get_giji_flg = -1; /* 擬似文節を作れたか */
    int			cnt;
    int			*vector1;

    struct	JKT_SBN		*get_kata_kouho();
    struct	JKT_SBN		*get_hira_kouho();
    struct	SYO_BNSETSU	*giji_sbn;
    struct	SYO_BNSETSU	*getsbnsp();

    if (yomi_sno == yomi_eno)
	return(NOTHING);

    if (fzk_ckvt(endvect) == NO && fzk_ckvt(endvect1) == NO)
	return (NOTHING);
	fzkcnt = fzk_kai(&bun[yomi_sno], &bun[yomi_eno], endvect, endvect1, &ichbnpbp);
	if (fzkcnt <= 0)
	    return (fzkcnt);	/* ERROR */
	for (ii = 0; ii < fzkcnt; ii++)
	    getfzkoh(ichbnpbp, ii)->offset += yomi_sno;


    for (ii = 0; ii < fzkcnt; ii++) {
	i = getfzkoh(ichbnpbp, ii)->offset;
	if (jmtp[i] == (struct jdata **) UN_KNOWN)  /* もう引いた? */
	    jmt_set(i);	/* 辞書引き */
    }

    j = j_max(ichbnpbp, fzkcnt);
    j = (j >= yomi_eno) ? yomi_eno - 1 : j;

    jktcnt = 0;

/* 疑似文節を先に取り出します(もしあれば) */
#ifndef	NO_KANA
/* ひらがな候補 */
    for (ii = 0; ii < fzkcnt; ii++) { /* 付属語 */
	fzkptr = getfzkoh(ichbnpbp, ii);
	index_tmp = fzkptr->offset;
	if ((kan_ck_vector(giji_no, fzkptr->vector) ||
	    (bnst_num != 1 ||
	     kan_ck_vector(giji_no, getfzkoh1(ichbnpbp, ii)->vector))) &&
		(index_tmp != yomi_eno - 1 + 1) && !(touten(bun[index_tmp])) &&
		!(kuten(bun[index_tmp]))) {
	    if (kan_ck_vector(giji_no, fzkptr->vector) == WNN_NOT_CONNECT_BK &&
		(bnst_num != 1 ||
		 kan_ck_vector(giji_no, getfzkoh1(ichbnpbp, ii)->vector) == WNN_NOT_CONNECT_BK))
		     break;
	    if ((jktsbn = get_hira_kouho(&jktsbn_top, index_tmp, yomi_eno)) < (struct JKT_SBN *)0)
		return (-1);
		
	    if (jktsbn > (struct JKT_SBN *)0) {
		jktsbn->parent = parent;	
		jktsbn->bend_m = yomi_sno;	
		jktsbn->kbcnt = bnst_num;
		jktcnt++;
	    }
	    break;	/* 1個あればいい */
	}
    }
/* カタカナ候補 */
    if (j < yomi_eno - 1) {
	for (ii = 0; ii < fzkcnt; ii++) { /* 付属語 */
	    fzkptr = getfzkoh(ichbnpbp, ii);
	    if (bnst_num == 1)
		vector1 = getfzkoh1(ichbnpbp, ii)->vector;
	    else
		vector1 = fzkptr->vector;
	    index_tmp = fzkptr->offset;
	    if ((jktsbn = get_kata_kouho(&jktsbn_top, yomi_sno, yomi_eno,
		fzkptr->offset, fzkptr->vector, vector1, ii)) < (struct JKT_SBN *)0)
	    
		return (-1);     
	    if (jktsbn > (struct JKT_SBN *)0) {
		jktsbn->parent = parent;	
		jktsbn->bend_m = yomi_sno;	
		jktsbn->kbcnt = bnst_num;
		jktcnt++;
	    }
	} /* 付属語 */
    }
#endif	/* NO_KANA */
    if ((giji_sbn = getsbnsp()) == NO)
	return (-1);
    gijisbn_top = 0;
    giji_sbn->bend_m = yomi_sno;
    if (getgiji(yomi_sno, yomi_eno, giji_sbn, ichbnpbp, fzkcnt, bnst_num) >= 0) {
	if (get_zen_giji(giji_sbn, &gijisbn_top) < 0)
	    return (-1);
	gijisbn_top->parent = parent;
	gijisbn_top->kbcnt = bnst_num;
	get_giji_flg = giji_sbn->j_c;
	jktcnt++;
    }
    freesbn(giji_sbn);

    if (gijisbn_top != 0) {
	if (get_giji_flg > j) {
	    gijisbn_top->lnk_br = jktsbn_top;
	    jktsbn_top = gijisbn_top;
	    get_giji_flg = -1;
	}
    }

    for (; j >= yomi_sno; j--) {
    /* 文節の長さ */
    if (get_giji_flg == j) {
	gijisbn_top->lnk_br = jktsbn_top;	
	jktsbn_top = gijisbn_top;
	get_giji_flg = -1;
    }
    if ((cnt = jkt_sbn_one(yomi_sno, j + 1, &jktsbn_top, bnst_num, parent,
				ichbnpbp, fzkcnt)) < 0)
	return (-1);
    jktcnt += cnt;

    }
    *tjktsbn = jktsbn_top;	/* 短いものから並べる */
    freeibsp(ichbnpbp);
    return (jktcnt);
}

struct DSD_DBN	*
get_dsd_dbn(cnt)
register int	cnt;
{
    static struct DSD_DBN	*m_dbn = 0;
    static int	md_cnt = 0;

    if (md_cnt < cnt) {
	/* どんどんふくれる Jserver */
	if (m_dbn)
	    free(m_dbn);
	m_dbn = (struct DSD_DBN *) malloc(cnt * sizeof(struct DSD_DBN));
	if (m_dbn == NULL) {
	    /* 大きくなって天まで届け! */
		wnn_errorno = WNN_JKTAREA_FULL;
		error1("malloc err in dbn_set (at daibnsetsu jikouho).\n");
		md_cnt = 0;
		return ((struct DSD_DBN *) -1);
	} else
	    md_cnt = cnt;
    }
    return (m_dbn);
}

struct DSD_SBN	*
get_dsd_sbn(cnt)
register int	cnt;
{
    static struct DSD_SBN	*m_sbn = 0;
    static int	ms_cnt = 0;

    if (ms_cnt < cnt) {
	if (m_sbn)
	    free(m_sbn);
	m_sbn = (struct DSD_SBN *) malloc(cnt * sizeof(struct DSD_SBN));
	if (m_sbn == NULL) {
	    wnn_errorno = WNN_JKTAREA_FULL;
	    error1("malloc err in dbn_set (at daibnsetsu jikouho).\n");
	    ms_cnt = 0;
	    return ((struct DSD_SBN *) -1);
	} else
	    ms_cnt = cnt;
    }
    return (m_sbn);
}

int
dbn_set(dsd_dbn, dbn)
struct DSD_DBN **dsd_dbn;
register struct JKT_DBN	*dbn;
{
    register struct JKT_SBN	*sbn;
    register struct JKT_SONE	*sone;
    int	i, j, son, par;
    struct JKT_DBN	*next_dbn;

    int	cnt;
    int	dbn_cnt;
    int	sbn_cnt;
    struct DSD_DBN *d_dbn;
    struct DSD_SBN *d_sbn;
    register struct DSD_SBN *p_sbn;
    struct DSD_SBN *sv_sbn;
    extern struct DSD_DBN	*get_dsd_dbn();
    extern struct DSD_SBN	*get_dsd_sbn();

    cnt_dbn(dbn, &cnt, &sbn_cnt);

    if ((d_dbn = *dsd_dbn = get_dsd_dbn(cnt)) <= (struct DSD_DBN *)0)
	return (-1);
    if ((sv_sbn = d_sbn = get_dsd_sbn(sbn_cnt)) <= (struct DSD_SBN *)0)
	return (-1);

    while (dbn) {
	sbn = dbn->sbn;
	cnt_sbn(sbn, &dbn_cnt, &sbn_cnt);
	p_sbn = d_sbn = sv_sbn;
	sv_sbn += sbn_cnt;
	for (i = 0; i < dbn_cnt; i++) {
	    d_dbn->bun_m = dbn->bend_m;
	    d_dbn->bun_jc = dbn->j_c;		
	    d_dbn->sbncnt = dbn->sbn_cnt;		
	    d_dbn->sbn = p_sbn;
	    p_sbn += dbn->sbn_cnt;
	    d_dbn++;
	}
#ifdef	CONVERT_from_TOP
	sbn_set(dbn, &d_sbn, sone, dbn->sbn);

    	next_dbn = dbn->lnk_br;
    	freejktdbn(dbn);
    	dbn = next_dbn;
#else	/* CONVERT_from_TOP */
	for (sbn = dbn->sbn; sbn != 0; sbn = sbn->parent, d_sbn++) {
	    son = cnt_son(dbn->sbn, sbn);
	    par = cnt_par(sbn);
	    p_sbn = d_sbn;
	    for (j = 0; j < son; j ++) {
		for (sone = sbn->sbn; sone != 0; sone = sone->lnk_br) {
		    for (i = 0; i < par; i++) {
			p_sbn->bun_m = sbn->bend_m;
			p_sbn->bun_jc = sbn->j_c;		
			p_sbn->i_jc   = sone->i_jc;
			p_sbn->jentptr = sone->jentptr;
			p_sbn->t_jc = sone->t_jc;
			p_sbn->hinsi = sone->hinsi_fk;
			p_sbn->kangovect = sbn->kangovect;
			p_sbn->v_jc = sone->v_jc;
			p_sbn->status_bkwd = sone->status_bkwd;
			if (sbn->status == WNN_NOT_CONNECT && sone->jentptr == 0)
			    p_sbn->status = WNN_GIJI;
			else
			    p_sbn->status = sbn->status;
			p_sbn += dbn->sbn_cnt;
		    }
		}
	    }
	}
	next_dbn = dbn->lnk_br;
	freejktdbn(dbn);
	dbn = next_dbn;
#endif	/* CONVERT_from_TOP */
    }
    return (cnt);
}

#ifdef	CONVERT_from_TOP
static void
sbn_set(dbn, pr_d_sbn, sone, sbn)
register struct JKT_DBN		*dbn;
register struct DSD_SBN 	**pr_d_sbn;
register struct JKT_SONE	*sone;
register struct JKT_SBN		*sbn;
{
    int	son, par;
    register struct DSD_SBN *p_sbn;
    if (sbn != 0){
	sbn_set(dbn, pr_d_sbn, sone, sbn->parent);

	son = cnt_son(dbn->sbn, sbn);
	par = cnt_par(sbn);
	p_sbn = *pr_d_sbn;
	sons_set(dbn, sone, sbn, p_sbn, son, par);	    
    
	(*pr_d_sbn)++;
    }
}

static void
sons_set(dbn,sone, sbn, p_sbn, son, par)
register struct JKT_DBN		*dbn;
register struct JKT_SONE	*sone;
register struct JKT_SBN		*sbn;
register struct DSD_SBN *p_sbn;
register int  son, par;
{	    
    register int i, j;

    for (j = 0; j < son; j ++) {
	for (sone = sbn->sbn; sone != 0; sone = sone->lnk_br) {
	    for (i = 0; i < par; i++) {
		p_sbn->bun_m = sbn->bend_m;
		p_sbn->bun_jc = sbn->j_c;		
		p_sbn->i_jc   = sone->i_jc;
		p_sbn->jentptr = sone->jentptr;
		p_sbn->t_jc = sone->t_jc;
		p_sbn->hinsi = sone->hinsi_fk;
		p_sbn->kangovect = sbn->kangovect;
		p_sbn->v_jc = sone->v_jc;
		p_sbn->status_bkwd = sone->status_bkwd;
		if (sbn->status == WNN_NOT_CONNECT && sone->jentptr == 0)
		    p_sbn->status = WNN_GIJI;
		else
		    p_sbn->status = sbn->status;
		p_sbn += dbn->sbn_cnt;
	    }
	}
    }
}
#endif	/* CONVERT_from_TOP */

/* 小文節の次候補の数 */
int
cnt_sone(sone)
register struct	JKT_SONE *sone;
{
    register int	i = 0;
    while(sone) {
	i++;
	sone = sone->lnk_br;
    }
    return (i);
}

/* その文節以後の総文節数 */
int
cnt_par(sbn)
register struct	JKT_SBN *sbn;
{
    register int	cnt;
    cnt = 1;
    while(sbn->parent) {
	sbn = sbn->parent;
	cnt *= cnt_sone(sbn->sbn);
    }
    return (cnt);
}

/* その文節以前の総文節数 */
int
cnt_son(son, sbn)
register struct	JKT_SBN *son;
register struct	JKT_SBN *sbn;
{
    register int	cnt;
    cnt = 1;
    while(son != sbn) {
	cnt *= cnt_sone(son->sbn);
	son = son->parent;
    }
    return (cnt);
}

/* 1 大文節中の小文節の数 */
static void
cnt_sbn(sbn, d_cnt, s_cnt)
register struct	JKT_SBN *sbn;
register int	*d_cnt;
register int	*s_cnt;
{
    *s_cnt = 0;
    *d_cnt = 1;
    while(sbn) {
	*d_cnt *= cnt_sone(sbn->sbn);
	(*s_cnt)++;
	sbn = sbn->parent;
    }
    *s_cnt *= *d_cnt;
}

/* 1 大文節の数 */
static void
cnt_dbn(dbn, dbn_cnt, sbn_cnt)
register struct	JKT_DBN *dbn;
register int	*dbn_cnt;
register int	*sbn_cnt;
{
    int	dbn_cnt_tmp;
    int	sbn_cnt_tmp;

    *dbn_cnt = 0;
    *sbn_cnt = 0;
    while(dbn) {
	cnt_sbn(dbn->sbn, &dbn_cnt_tmp, &sbn_cnt_tmp);
	*dbn_cnt +=dbn_cnt_tmp;
	*sbn_cnt +=sbn_cnt_tmp;
	dbn = dbn->lnk_br;
    }
}
