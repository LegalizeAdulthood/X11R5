/*
 * $Id: bnsetu_kai.c,v 1.2 1991/09/16 21:31:10 ohm Exp $
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
 *	��ʸ�����				*
 ************************************************/
#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "jdata.h"

#include "fzk.h"
#include "kaiseki.h"

int
sbn_kai(yomi_sno, yomi_eno, endvect, endvect1, tsbnptr, bnst_num, parent)
int		yomi_sno;
int		yomi_eno;
int		endvect;	/* ��ü�٥��� */
int		endvect1;	/* ��ü�٥��� */
struct SYO_BNSETSU **tsbnptr;	/* ��ʸ����Ϸ�� */
int		bnst_num;	/* ����ޤǤ˲��Ϥ�����ʸ��� */
struct SYO_BNSETSU *parent;	/* �Ƥδ���Ρ��� */
{
    int	fzkcnt, hyokaval, hyokaval0 = 0, hinsidt, sbncnt;
    unsigned short	*hinsi_buf;
	     int	hinsidt_fk;	/* ʣ���ʻ�Υ����� */
    register int	i,		/* ����ν����Υ���ǥå��� */
			j,		/* ʸ��Ϥ�Υ���ǥå��� */
			t,
			ii;
    struct		jdata		*jentptr;
    register struct	SYO_BNSETSU	*sbnptr = NULL;
    struct		SYO_BNSETSU	*sbnptr_top = 0;
    struct		ICHBNP		*ichbnpbp;
    int			k, ll;
    struct		SYO_BNSETSU	*kanptr;
    struct		SYO_BNSETSU	*kanptr_tmp = NULL;
    struct		SYO_BNSETSU	*kanptr_before;
    struct		SYO_BNSETSU	*giji_sbn;
    int			get_giji_flg = -1; /* ����ʸ����줿�� */

    enum	FLG {
		set,
		noset
    } setflg;
    enum	FLG	kanflg;	/* Ʊ���٥������ʻ줬���ä����ݤ� */
    enum	GFLG {
		get,
		noget
    } getflg;
    struct	SYO_BNSETSU	*getsbnsp();
    int		kangovect;
    int		connect_flg;

    if ((yomi_sno == yomi_eno) ||
        (fzk_ckvt(endvect) == NO && fzk_ckvt(endvect1) == NO))
	return (NOTHING);

    if ((fzkcnt = fzk_kai(
		&bun[yomi_sno], &bun[yomi_eno], endvect, endvect1, &ichbnpbp)) <= 0)
	return (fzkcnt);	/* ERROR */
    for (ii = 0; ii < fzkcnt; ii++) 
	getfzkoh(ichbnpbp, ii)->offset += yomi_sno;

    for (ii = 0; ii < fzkcnt; ii++) {
	i = getfzkoh(ichbnpbp, ii)->offset;
	if (jmtp[i] == (struct jdata **) UN_KNOWN) { /* �⤦������? */
	    if(jmt_set(i) == -1){	/* ������� */
		return (-1);
	    }
	}
    }

    j = j_max(ichbnpbp, fzkcnt);
    j = (j >= yomi_eno) ? yomi_eno - 1 : j;

    setflg = noset;
    if ((giji_sbn = getsbnsp()) == NO)
	return (-1);	/* ERROR */
    giji_sbn->kbcnt = bnst_num;
    giji_sbn->bend_m = yomi_sno;
    giji_sbn->parent = parent;

    /* ����ʸ�����˼��Ф��ޤ�(�⤷�����) */
    if (getgiji(yomi_sno, yomi_eno, giji_sbn, ichbnpbp, fzkcnt, bnst_num) >= 0) {
	sbncnt = 1;
	getflg = get;
        get_giji_flg = giji_sbn->j_c;
	if (giji_sbn->j_c > j) {
	    sbnptr_top = giji_sbn;
	    get_giji_flg = -1;
	    kanptr_tmp = giji_sbn->lnk_br;
	}
    } else {
	sbnptr = giji_sbn;
        sbncnt = 0;
	getflg = noget;
    }
 
    for (; j >= yomi_sno; j--) {	/* ʸ���Ĺ�� */
	if ((setflg == noset) && (get_giji_flg == j)) {
	    giji_sbn->lnk_br = sbnptr_top;	
	    sbnptr_top = giji_sbn;
	    setflg = set;
	    kanptr_tmp = giji_sbn->lnk_br;
	}
	for (ii = 0; (ii < fzkcnt) &&
		     ((i = (getfzkoh(ichbnpbp, ii)->offset)) <= j);
		     ii++) {		/* ��°�� */
	    jentptr = (struct jdata *) C(i, j);
	    for (; jentptr; jentptr = jentptr->jptr) { /* ���� */
		for (t = 0; t < (jentptr->kosuu); t++) { /* ���� */
		    if(((jentptr->hindo[t]&0x7f) == 0x7f) ||
		       (jentptr->hindo_in && (jentptr->hindo_in[t]&0x7f) == 0x7f))
			continue; /*�����ȥ����Ȥ��줿���񥨥�ȥ� */
#ifdef	CONVERT_with_SiSheng
		    hyokaval = hyoka1(jentptr->hindo[t],
			(jentptr->hindo_in == 0 ? 0 : jentptr->hindo_in[t]),
			(jentptr->sisheng == 0 ? 0 : diff_sisheng(
			    jentptr->sisheng[t], jentptr->sisheng_int)),
			j - i/2 - yomi_sno/2 + 1,
			dic_table[jentptr->jishono].nice);
#else
		    hyokaval = hyoka1(jentptr->hindo[t],
			(jentptr->hindo_in == 0 ? 0 : jentptr->hindo_in[t]),
			j - i + 1, j - yomi_sno + 1,
			dic_table[jentptr->jishono].nice);
#endif	/* CONVERT_with_SiSheng */
		    if (bnst_num != 1) {
			if (parent != 0 && parent->status_bkwd == NO)
			    /* ����ʸ�᤬ endvect ����³�Ǥ��ʤ���Τϡ�
			       ɾ���ͤ򲼤��� */
			    hyokaval0 = _DIVID(hyokaval, 2);
			else
			    hyokaval0 = hyokaval;
		    }
		    hinsidt_fk = hinsidt = jentptr->hinsi[t];
		    if ((ll = wnn_get_fukugou_component_body(hinsidt_fk, &hinsi_buf)) == 0) {
			error1("wnn_get_fukugou_component:erroe in bunsetukai.");
			return (-1);
		    }
		    for ( ; ll > 0; ll--) {
		    hinsidt = *hinsi_buf;
		    hinsi_buf++;
		    /* ����������ñ���ʻ�����ͤ���Ф��� */
			connect_flg = kan_ck_vector(hinsidt,
					(getfzkoh(ichbnpbp, ii)->vector));
			if (
			    (connect_flg == WNN_CONNECT_BK ||
			     bnst_num == 1 &&
			     kan_ck_vector(hinsidt,
					 (getfzkoh1(ichbnpbp, ii)->vector)) ==
					    WNN_CONNECT_BK)) {
			/* ��³�Ǥ��� */
			    if (bnst_num == 1)
				/* endvect ����³�Ǥ��ʤ���Τ�ɾ���ͤ򲼤��� */
				hyokaval0 = (connect_flg != WNN_NOT_CONNECT_BK) ?
						hyokaval: _DIVID(hyokaval, 2);
			    kangovect = ft->kango_hinsi_area[hinsidt];
			    if (getflg == get) {
				getflg = noget;
				if(0 == (sbnptr = getsbnsp()))
				    return (-1);	/* ERROR */
				sbnptr->kbcnt = bnst_num;
				sbnptr->bend_m = yomi_sno;
				sbnptr->parent = parent;
			    }
			    sbnptr->j_c = j;

			    if (setflg == noset) { /* ����Ĺ����ʸ��Ͻ��� */
			        sbnptr->lnk_br = sbnptr_top;	
			        sbnptr_top = sbnptr;
				setflg = set;
				sbnptr->v_jc = hyokaval0;
				sbnptr->t_jc = t;
				sbnptr->jentptr = jentptr;
				sbnptr->hinsi_fk = hinsidt_fk;
				sbnptr->kangovect = kangovect;
				sbnptr->i_jc = i;
				/* yoku wakaran 9/8
				if (endvect1 == WNN_VECT_NO)
				    sbnptr->status_bkwd = WNN_NOT_CONNECT_BK;
				else
				*/
				    sbnptr->status_bkwd = connect_flg;
				kanptr_tmp = sbnptr->lnk_br;
				getflg = get;
				sbncnt++;
			    } else {
				kanflg = noset;
				for (kanptr_before = kanptr = sbnptr_top;
				     kanptr != kanptr_tmp;
				     kanptr_before = kanptr,
				     kanptr = kanptr->lnk_br) {
				    if (kanptr->kangovect == kangovect) {
				    /* Ʊ���٥������ʻ줬���ä�  */
					if (hyokaval0 > kanptr->v_jc) {
					/* ���ä�������ɾ���ͤ��⤤ */
					    kanptr->v_jc = hyokaval0;
					    kanptr->t_jc = t;
					    kanptr->jentptr = jentptr;
					    kanptr->hinsi_fk = hinsidt_fk;
					    kanptr->i_jc = i;
					    /* yoku wakaran 9/8
					    if (endvect1 == WNN_VECT_NO)
						kanptr->status_bkwd = WNN_NOT_CONNECT_BK;
					    else
					    */
						kanptr->status_bkwd = connect_flg;
					}
					kanflg = set;
					break;
				    } else if (kanptr->kangovect > kangovect) {
				    /* Ʊ���٥������ʻ줬�ʤ��ä� */
					break;
				    }
				} /* SORT & INSERT SYO_BN */

				if (kanflg == noset) {
				/* Ʊ���٥������ʻ줬�ʤ��ä� */
				    if (kanptr == sbnptr_top) {
					sbnptr->lnk_br = kanptr;
					sbnptr_top = sbnptr;
				    } else {
					sbnptr->lnk_br = kanptr;
					kanptr_before->lnk_br = sbnptr;
				    }
				    sbnptr->v_jc = hyokaval0;
				    sbnptr->t_jc = t;
				    sbnptr->jentptr = jentptr;
				    sbnptr->hinsi_fk = hinsidt_fk;
				    sbnptr->kangovect = kangovect;
				    sbnptr->i_jc = i;
				    /* yoku wakaran 9/8
				    if (endvect1 == WNN_VECT_NO)
					sbnptr->status_bkwd = WNN_NOT_CONNECT_BK;
				    else
				    */
					sbnptr->status_bkwd = connect_flg;
				    sbncnt++;
				    getflg = get;
			        }
			    }
			} /* ��³�Ǥ��� */
		    } /* ñ���ʻ� */
		} /* ���� */
	    } /* ���� */
	} /* ��°�� */
	setflg = noset;	/* ����Ĺ���δ���Ϥޤ�set����̵�� */
    } /* ʸ���Ĺ�� */
    if (sbncnt == NOTHING) { /* ���줬�ʤ��ä� */
	for (k = fzkcnt - 1; k >= 0; k--) {
	    if (kan_ck_vector(katakanago_no, getfzkoh(ichbnpbp, k)->vector) ==
			WNN_CONNECT_BK) {
		k = getgiji(yomi_sno, yomi_eno, sbnptr, ichbnpbp, k + 1, bnst_num);
		break;
	    }
	}
	if (k < 0) {
	    if (bnst_num == 1) {
		/* �������Ǥ⵿��ʸ��������� */
		getgiji_f(yomi_sno, sbnptr);
		sbncnt = 1;
		sbnptr_top = sbnptr;
#ifdef	nodef
		wnn_errorno = WNN_NO_KOUHO;
		error1("Cannot get bunsetsu kouho in bunsetu-kaiseki.\n");
#endif	/* nodef */
	    } else {
		freesbn(sbnptr);
		sbncnt = 0;
		sbnptr_top = 0;
	    }
	} else {
	    sbncnt = 1;
	    sbnptr_top = sbnptr;
	}
    } else if (getflg == noget) {
	freesbn(sbnptr);
    }
    *tsbnptr = sbnptr_top;	/* û����Τ����¤٤� */
    freeibsp(ichbnpbp);
    return (sbncnt);
}

/*******************************************/
/*	max value in maxj[] search routine */
/*******************************************/
int
j_max(fzkbnp, fzkcnt)
register struct	ICHBNP	*fzkbnp;
int		fzkcnt;		/* max number of fzkkouho table */
{
    register int	i, mxj = -1, k, ofst;

    for (i = 0; i < fzkcnt; i++) {
	if ((ofst = getfzkoh(fzkbnp, i)->offset) < maxchg)
	    if ((k = maxj[ofst]) > mxj)
		mxj = k;
    }
    return (mxj);
}

/**********************************************/
/*	��������γ���			      */
/**********************************************/
int
getgiji(yomi_sno, yomi_eno, sbnptr, fzkbnp, fzkcnt, bnst_num)
int 			yomi_sno;
int 			yomi_eno;
struct	SYO_BNSETSU	*sbnptr;
struct	ICHBNP		*fzkbnp;
int			fzkcnt;
int			bnst_num;
{
	register int	j_end;
	int	j_end_sv;
	int	*vector;
#ifndef	CHINESE
	int	*vector1;
#endif
	int	giji_hinsi;
	int	giji_hindo;
	int	len = 0;
	w_char	c = (w_char)0;

	j_end = getfzkoh(fzkbnp, fzkcnt - 1)->offset;
	vector = getfzkoh(fzkbnp, fzkcnt - 1)->vector;
#ifndef	CHINESE
	vector1 = getfzkoh1(fzkbnp, fzkcnt - 1)->vector;
#endif

	if (j_end >= yomi_eno) {	/* ��°������ξ�� */
	    giji_hinsi = fuzokugo_no;
	    giji_hindo = c_env->fuzokogo_val;
	    len = 0;
	    goto	_Only_Fuzokugo;
	}
/*#ifdef	not_use*/
	if (isjhira(bun[j_end]))	/* �Ҥ餬�ʤʤ�Х��Х� */
		return (-1);
/*#endif*/
	for (j_end_sv = j_end; j_end < yomi_eno; j_end++) {
	    /* ���ڡ����ϡ��������ε���ʸ��ˤ��äĤ��� */
	    if (!(isspace(c = bun[j_end])) && !(isjspace(c))) {
		if (TOKUSYU(c) || check_bar_katakana(j_end, yomi_eno) ||
		    isjhira(c) || isjkanji(c)) { /* �Ҳ�̾ �Ҥ餬�� ���� */
		    j_end = j_end_sv;
		}
		break;
	    }
	}

	if (j_end >= yomi_eno) {	/* ���ڡ��������ξ�� */
	    giji_hinsi = fuzokugo_no;
	    giji_hindo = c_env->fuzokogo_val;
	    len = 0;
	    goto	_Only_Fuzokugo;
	}

/* ʬ�� */
	if (isascii(c = bun[j_end]) && isdigit(c) || isjdigit(c)) { /* ���� */
	    giji_hinsi = suuji_no;
	    giji_hindo = c_env->suuji_val;
	} else if (isascii(c) && isparen_e(c) || isjparen_e(c)) { /* �ĳ�� */
	    giji_hinsi = toji_kakko_no;
	    giji_hindo = c_env->toji_kakko_val;
	} else if (TOKUSYU(c) || check_bar_katakana(j_end, yomi_eno)) { /* �Ҳ�̾ */
	    giji_hinsi = katakanago_no;
	    giji_hindo = c_env->kana_val;
	} else if (is_g_eisuu(c)) { /* ��������(����ե��٥å�, ����) */
	    giji_hinsi = eisuu_no;
	    giji_hindo = c_env->eisuu_val;
	} else if (isascii(c) && isparen_s(c) || isjparen_s(c)) { /* ����� */
	    giji_hinsi = kai_kakko_no;
	    giji_hindo = c_env->kaikakko_val;
#ifdef	nodef
	} else if (isjhira(c)) { /* �Ҥ餬�� */
	    giji_hinsi = giji_no;
	    giji_hindo = 0;
#endif
	} else { /* ���桢���� */
	    giji_hinsi = kigou_no;
	    giji_hindo = c_env->kigou_val;
	}

/* ��³��ǽ�� */
	if (sbnptr->status_bkwd = kan_ck_vector(giji_hinsi, vector) == WNN_CONNECT_BK)
	    goto _Can_connect;
#ifdef	nodef
	/* ��������³�Ǥ��ʤ���,�������ˤ���³�Ǥ��� */
	if (giji_hinsi == suuji_no && kan_ck_vector(eisuu_no, vector) == WNN_CONNECT_BK) {
	    giji_hinsi = eisuu_no;
	    giji_hindo = c_env->eisuu_val;
	    goto _Can_connect;
	}
#endif	/* nodef */
	if (bnst_num != 1)
		return (-1);
#ifndef	CHINESE
	if (kan_ck_vector(giji_hinsi, vector1) == WNN_NOT_CONNECT_BK) {
	    if (giji_hinsi != suuji_no ||
		kan_ck_vector(eisuu_no, vector1) == WNN_NOT_CONNECT_BK)
		return (-1);
	    else {
		giji_hinsi = eisuu_no;
		giji_hindo = c_env->eisuu_val;
	    }
	}
#endif	/* CHINESE */

_Can_connect:

/* Ĺ����Ĵ�٤� */
	if (giji_hinsi == suuji_no) {
	    for ( ; (j_end + len < yomi_eno) &&
		    (isascii(bun[j_end + len]) && isdigit(bun[j_end + len]) ||
		     isjdigit(bun[j_end + len])); len++)
		;
#ifndef	CHINESE
	    if ((j_end + len < yomi_eno) && is_g_eisuu(bun[j_end + len])) {
	    /* �ѿ� */
		for (len++;
		     (j_end + len < yomi_eno) && is_g_eisuu(bun[j_end + len]);
		     len++)
			;
		if (kan_ck_vector(eisuu_no, vector) == WNN_NOT_CONNECT_BK &&
		    (bnst_num != 1 || kan_ck_vector(eisuu_no, vector1)))
			return (-1);
		giji_hinsi = eisuu_no;
#ifdef	nodef
	    /* Ⱦ�ѿ��� */
	    } else {
		if (kan_ck_vector(suuji_no, vector) == WNN_NOT_CONNECT_BK)
		    return (-1);
#endif	/* nodef */
	    }
#endif	/* CHINESE */
	} else if (giji_hinsi == eisuu_no) {
	    for ( ; ((j_end + len) < yomi_eno) && is_g_eisuu(bun[j_end + len]);
		 len++)
		    ;
	} else if (giji_hinsi == toji_kakko_no) { /* �ĳ�� */
	    len = 1;
	} else if (giji_hinsi == kigou_no) { /* ���� */
	    len = 1;
	} else if (giji_hinsi == kai_kakko_no) { /* ����� */
	    len = 1;
	} else if (giji_hinsi == giji_no) {
	    len = 1;
	} else if (giji_hinsi == katakanago_no) {
	    /* �������� Ⱦ�ѥ������� ���� Ⱦ����*/
	    for (len = 1; (j_end + len < yomi_eno) &&
		(TOKUSYU(bun[j_end + len]) || 
		 check_bar_katakana(j_end + len, yomi_eno)); len++)
		    ;
	} else {
	    if (j_end >= yomi_eno)
		    len = 0;
	    else
		    len = 1;
	}

	/* ���ڡ����ϡ����Τ��Ȥε���ʸ��ˤ��äĤ��� */
	if ((giji_hinsi == eisuu_no) || (giji_hinsi == suuji_no)) {
	    for ( ; ((j_end + len) < yomi_eno) &&
		    (isspace(bun[j_end + len]) || (isjspace(c))); len++)
			    ;
	}

_Only_Fuzokugo:
/* ɾ���ͤη��� */
	sbnptr->v_jc = HYOKAVAL(giji_hindo, len, len + j_end - yomi_sno);
	sbnptr->i_jc = j_end;
	if (giji_hinsi == katakanago_no) {
	    sbnptr->t_jc = WNN_KATAKANA;
	} else if (isascii(c)) {
	    if (giji_hinsi == suuji_no)
		sbnptr->t_jc = c_env->giji.number;
	    else if (giji_hinsi == eisuu_no) {
		sbnptr->t_jc = c_env->giji.eisuu;
	    } else if (giji_hinsi == kigou_no ||
		       giji_hinsi == toji_kakko_no ||
		       giji_hinsi == kai_kakko_no) {
		sbnptr->t_jc = c_env->giji.kigou;
	    } else {
		sbnptr->t_jc = -1;
	    }
	    sbnptr->v_jc += 2; /* ���������ɾ���ͤ�夲�뤿�� */
	} else {
	    sbnptr->t_jc = -1;
	}
	sbnptr->hinsi_fk = giji_hinsi;
	sbnptr->jentptr = 0;
	sbnptr->kangovect = ft->kango_hinsi_area[giji_hinsi];
	sbnptr->j_c = j_end + len - 1;
	return (len);
}

int
is_g_eisuu(c)
register w_char c;
{
    register int i;

    if (isascii(c) && isalnum(c) || isjalnum(c))
	return(YES);
    else {
	for (i = 0; i < 20; i++) {
	    if (giji_eisuu[i] == 0xffff)
		return (NO);
	    else if (giji_eisuu[i] == c)
		return(YES);
	}
    }
    return(NO);
}


/* �������Ǥ⵿��ʸ��������� */
int
getgiji_f(yomi_sno, sbnptr)
register int 			yomi_sno;
register struct	SYO_BNSETSU	*sbnptr;
{
	sbnptr->v_jc = HYOKAVAL(0, 1, 1);
	sbnptr->i_jc = yomi_sno;
	sbnptr->t_jc = -1;
	sbnptr->hinsi_fk = giji_no;
	sbnptr->jentptr = 0;
	sbnptr->kangovect = ft->kango_hinsi_area[kigou_no];
	sbnptr->j_c = yomi_sno;

	return (1);
}

/* �������ʤ�? Ĺ����������ϡ��������ʤǤʤ��ȥ��� */
int
check_bar_katakana(i, yomi_eno)
register int i;
register int yomi_eno;
{
	register int j;
	for (j = i; j < yomi_eno; j++) {
		if (isjkata(bun[j]))
			return (YES);
		if (!(BAR_CODE(bun[j])))
			return (NO);
	}
	return (NO);
}

/*
 * kan_ckvt	hinsi ���ʻ줬vector����³�Ǥ��뤫
 */

int
kan_ckvt(hinsi, vector)
register unsigned short hinsi;	/* �ʻ�No. */
register int vector;	/* ��°����ü�٥��� */
{
	register int *v;
	v = (int *) ((struct kangovect *)ft->kango_vect_area + vector);
	return (kan_ck_vector(hinsi, v));
	/*
	return (kan_ck_vector(hinsi,
			((struct kangovect *) ft->kango_vect_area + vector)));
	*/
}

#ifdef	change_macro
int
kan_ck_vector(hinsi, vector)
unsigned short hinsi;	/* �ʻ�No. */
int vector[];	/* ��°����ü�٥��� */
{
	register int wvect;

	wvect = vector[hinsi / (sizeof (int) << 3)];/* << 3 == * 8  */
	wvect >>= (hinsi % (sizeof (int) << 3));
	if ((wvect & 0x00000001) == 1)
		return (WNN_CONNECT_BK);
	else
		return (WNN_NOT_CONNECT_BK);
}
#endif	/* change_macro */
