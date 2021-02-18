/*
 * $Id: renbn_kai.c,v 1.3 1991/09/16 21:32:02 ohm Exp $
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
#include <stdio.h>  /* koreha debug ga sundare iranai  */
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "fzk.h"

#include "kaiseki.h"
extern struct DSD_DBN	*get_dsd_dbn();
extern struct DSD_SBN	*get_dsd_sbn();
static void cnt_bzd();

int
renbn_kai(yomi_sno, yomi_eno, beginvect,
#ifndef	NO_FZK
	    fzkchar,
#endif	/* NO_FZK */
	    endvect, endvect1, endvect2, kaidbno, kaisbno, dsd_dbn)
int	yomi_sno;	/* 解析文字列 start index */
int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ YOBI */
int	endvect2;	/* bunsetsu 終端ベクタ */
int	kaidbno;	/* 解析大文節数 */
int	kaisbno;	/* 解析小文節数 */
struct DSD_DBN **dsd_dbn;	/* 決定大文節情報エリア pointer */
{
    int	dbn_cnt;
    int	sbn_cnt;
    struct DSD_SBN *dsd_sbn;	/* 決定大文節情報エリア pointer */

    register UINT	dicidyno,	/* 決定した大文節の index */
		    buncnt = 0;	/* 決定した大文節数 */
    struct BZD	*rbzdptr;	/* 決定の対象となるノード top pointer */
    register struct BZD 	
	    *brbzdptr,	/* work pointer */
	    *dicide_bp = 0,	/* 決定した大文節の top pointer */
	    *dicide_np,	/* 決定した大文節への pointer */
	    *wkbzdptr,	/* work pointer		*/
	    *maxbzd;	/* 決定の対象となっている大文節の */
			    /* 最大の評価値を持つノードへの pointer */
    struct DSD_DBN	*dcdbn_set();
    int	rtmknode;
    extern	int	_status;

    dicidyno = yomi_sno - 1;
    rbzdptr = 0;
    rtmknode = 1;

    _status = 0;

    if(c_env->fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD;
	return(-1);
    }
    ft = (struct FT *)files[c_env->fzk_fid].area;
    if (chk_yomi_endvect(yomi_sno, yomi_eno, endvect, endvect1) < 0)
	return (-1);
    while(dicidyno + 1 < yomi_eno) {
	if(rbzdptr == 0) {
	    if (dbn_kai(dicidyno + 1, yomi_eno, beginvect,
#ifndef	NO_FZK
			fzkchar,
#endif	/* NO_FZK */
			endvect, endvect1, kaisbno, &rbzdptr) < 0) {
		error1("CONVERSION_ERROR.\n");
		init_work_areas();
		return(-1); /* ERROR */
	    }
	    if(rbzdptr == 0) {
		    if (set_kata_giji_bzd(dicidyno + 1, yomi_eno, endvect,
					  endvect1, &rbzdptr, buncnt) < 0)
			return (-1);
		    buncnt++;
		    rtmknode = 0;
		    if(dicide_bp == 0)
			dicide_bp = rbzdptr;
		    else
			dicide_np->lnk_son = rbzdptr;
		    break;
	    }
	}
	/* 1 文節決定可能になるまでノードを作る */
	if (kaidbno == 1) {
	/* 各文節の評価値の足し算(son_v)は、mknode でやるんだけど kaidbno が 1
	   つまり 1文節解析の場合は、そこを通らないからここでやろう */
	    for (brbzdptr = rbzdptr->lnk_br; brbzdptr != 0;
		 brbzdptr = brbzdptr->lnk_br)
		brbzdptr->son_v = brbzdptr->v_jc;
	} else {
	    while((int)(rbzdptr->kbcnt) < kaidbno && rtmknode > 0) {
		for(brbzdptr = rbzdptr; brbzdptr != 0;
		brbzdptr = brbzdptr->lnk_br) {
		    rtmknode = mknode(brbzdptr, yomi_eno, beginvect,
#ifndef	NO_FZK
				      fzkchar,
#endif	/* NO_FZK */
				      endvect2, kaisbno);
		    brbzdptr->son_v /= brbzdptr->kbcnt;
		    if(rtmknode == 0) {
			brbzdptr->son_v += brbzdptr->v_jc;
		    } else if(rtmknode == -1) {
			error1("CONVERSION_ERROR.\n");
			init_work_areas();	/* エラーでいいのかな */
			return(-1);	/* ERROR */
		    }
		}
	    }
	}

	/* 最大の評価を持つノードを見つける */
	for(brbzdptr = rbzdptr->lnk_br,maxbzd = rbzdptr; brbzdptr != 0;
		    brbzdptr = wkbzdptr) {
	    wkbzdptr = brbzdptr->lnk_br;
	    if(maxbzd->son_v <= brbzdptr->son_v) {
		clr_node(maxbzd);     /* garbage collection */
		maxbzd = brbzdptr;
	    } else
		clr_node(brbzdptr);
	}

	/* 1 文節決定 */
	buncnt++;		/* 決定文節 count up */
	dicidyno = maxbzd->j_c;	/* 決定した文節の top */
	/* kettei sita node no link */
	if(maxbzd->bend_m == yomi_sno) {
	    dicide_bp = dicide_np = maxbzd;
	} else {
	    dicide_np->lnk_son = maxbzd;
	    dicide_np = maxbzd;
	}
	rbzdptr = maxbzd->lnk_son;
    }

    cnt_bzd(dicide_bp, &dbn_cnt, &sbn_cnt);
    if ((*dsd_dbn = get_dsd_dbn(dbn_cnt)) <= (struct DSD_DBN *)0) {
	init_work_areas();
	return (-1);
    }
    if ((dsd_sbn = get_dsd_sbn(sbn_cnt)) <= (struct DSD_SBN *)0) {
	init_work_areas();
	return (-1);
    }

    dcdbn_set(*dsd_dbn, &dsd_sbn, dicide_bp);
    return(buncnt);
}

/************************************************************/
/*	単文節解析(大文節) routine			    */
/************************************************************/
int
tan_dai(yomi_sno, yomi_eno, beginvect,
#ifndef	NO_FZK
	    fzkchar,
#endif	/* NO_FZK */
	    endvect, endvect1, kaisbno, dsd_dbn)
int	yomi_sno;	/* 解析文字列 start index */
int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
int	kaisbno;		/* 解析小文節数 */
struct	DSD_DBN	**dsd_dbn;	/* 決定大文節情報エリア pointer */
{
    int	dbn_cnt;
    int	sbn_cnt;
    struct	DSD_SBN *dsd_sbn;	/* 決定大文節情報エリア pointer */
    struct	BZD	*rbzdptr;	/* 決定の対象となるノードの
				       トップポインタ */
    register struct BZD	*brbzdptr, *wkbzdptr;
    struct BZD		*maxbzd;	/* 決定の対象となっている文節の
			       最大の評価値を持つノードへのポインタ */
    extern struct	DSD_DBN	*dcdbn_set();
    extern int		_status;

    if(c_env->fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD;
	return(-1);
    }
    ft = (struct FT *)files[c_env->fzk_fid].area;

    rbzdptr = 0;
    _status = 0;
    if (chk_yomi_endvect(yomi_sno, yomi_eno, endvect, endvect1) < 0)
	return (-1);
    if (dbn_kai(yomi_sno,yomi_eno, beginvect,
#ifndef	NO_FZK
		fzkchar,
#endif	/* NO_FZK */
		endvect, endvect1, kaisbno, &rbzdptr) < 0) {
	init_work_areas();
	return(-1);	/* ERROR */
    }

    /* 最大の評価値を持つノードを見つける */
    for(brbzdptr = rbzdptr, maxbzd = 0; brbzdptr != 0; brbzdptr = wkbzdptr) {
	wkbzdptr = brbzdptr->lnk_br;
	if(brbzdptr->j_c == yomi_eno - 1) {	/* 文節長さ */
	    if(maxbzd == 0) {
		maxbzd = brbzdptr;
	    } else if (maxbzd->v_jc < brbzdptr->v_jc) {
		freebzd(maxbzd);
		maxbzd = brbzdptr;
	    } else {
		freebzd(brbzdptr);
	    }
	} else {
	    clr_node(brbzdptr);
	}
    }

    if(maxbzd == 0) {
	if (set_kata_giji_bzd(yomi_sno, yomi_eno, endvect, endvect1, &maxbzd, 0) < 0)
	    return (-1);
    }
    cnt_bzd(maxbzd, &dbn_cnt, &sbn_cnt);
    if ((*dsd_dbn = get_dsd_dbn(dbn_cnt)) <= (struct DSD_DBN *)0) {
	init_work_areas();
	return (-1);
    }
    if ((dsd_sbn = get_dsd_sbn(sbn_cnt)) <= (struct DSD_SBN *)0) {
	init_work_areas();
	return (-1);
    }

    dcdbn_set(*dsd_dbn, &dsd_sbn, maxbzd);
    return(1);
}
/************************************************************/
/*	単文節解析(小文節) routine			    */
/************************************************************/
int
tan_syo(yomi_sno, yomi_eno, beginvect,
#ifndef	NO_FZK
	    fzkchar,
#endif	/* NO_FZK */
	    endvect, endvect1, dsd_sbn)
int	yomi_sno;	/* 解析文字列 start index */
int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
int	beginvect;	/* 前端ベクタ(-1:文節先頭、-2:なんでも)品詞No.*/
#ifndef	NO_FZK
w_char	*fzkchar;
#endif	/* NO_FZK */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
struct	DSD_SBN	**dsd_sbn;	/* 決定小文節情報エリア pointer */
{
    int			sbn_cnt;
    struct SYO_BNSETSU	*rsbnptr;	/* 決定の対象となるノードのトップポインタ */
    register struct	SYO_BNSETSU	*brsbnptr, *wksbnptr;
    struct SYO_BNSETSU	*maxsbn;	/* 決定の対象となっている文節の
				       最大の評価値を持つノードへのポインタ */
    extern struct	DSD_SBN *dcdsbn_set();
    int			divid;
    extern int		_status;

    if(c_env->fzk_fid == -1){
	wnn_errorno = WNN_FZK_FILE_NO_LOAD;
	return(-1);
    }
    ft = (struct FT *)files[c_env->fzk_fid].area;

    rsbnptr = 0;
    _status = 0;
    if (chk_yomi_endvect(yomi_sno, yomi_eno, endvect, endvect1) < 0)
	return (-1);
    if (sbn_kai(yomi_sno, yomi_eno, endvect, endvect1, &rsbnptr,1,0) < 0) {
	init_work_areas();
	return(-1);	/* ERROR */
    }

    /* 最大の評価値を持つノードを見つける */
    for(brsbnptr = rsbnptr, maxsbn = 0;brsbnptr != 0;brsbnptr = wksbnptr) {
	wksbnptr = brsbnptr->lnk_br;
	if(brsbnptr->j_c == yomi_eno - 1) {	/* 文節長さ */
	    if(maxsbn == 0) {
		maxsbn = brsbnptr;
		divid = get_status(brsbnptr->kangovect, beginvect,
#ifndef	NO_FZK
				   fzkchar,
#endif	/* NO_FZK */
				   &(brsbnptr->status));
		brsbnptr->v_jc = DIVID_HYOUKA(brsbnptr->v_jc, divid);
	    } else {
		divid = get_status(brsbnptr->kangovect, beginvect, 
#ifndef	NO_FZK
				   fzkchar,
#endif	/* NO_FZK */
				   &(brsbnptr->status));
		brsbnptr->v_jc = DIVID_HYOUKA(brsbnptr->v_jc, divid);
		if (beginvect != WNN_ALL_HINSI &&
		    (brsbnptr->status == WNN_SENTOU && maxsbn->status != WNN_SENTOU ||
		     brsbnptr->status == WNN_CONNECT && maxsbn->status != WNN_CONNECT)) {
		    freesbn(maxsbn);
		    maxsbn = brsbnptr;
		} else if (maxsbn->v_jc < brsbnptr->v_jc) {
		    freesbn(maxsbn);
		    maxsbn = brsbnptr;
		} else {
		    freesbn(brsbnptr);
		}
	    }
	} else {
	    freesbn(brsbnptr);
	}
    }

    if(maxsbn == 0) {
	if (set_kata_giji_sbn(yomi_sno, yomi_eno, endvect, endvect1, &maxsbn) < 0)
	    return (-1);
    }
    if (maxsbn->jentptr == 0 && maxsbn->status == WNN_NOT_CONNECT)
	maxsbn->status = WNN_GIJI;
    sbn_cnt = cnt_syo(maxsbn);
    if ((*dsd_sbn = get_dsd_sbn(sbn_cnt)) <= (struct DSD_SBN *)0) {
	init_work_areas();
	return (-1);
    }
    dcdsbn_set(*dsd_sbn, maxsbn);
    freesbn(maxsbn);
    return(1);
}

/**********************************************/
/* 決定した文節の情報をセットする	      */
/**********************************************/
struct DSD_DBN *dcdbn_set(dsd_dbn, dsd_sbn, bzd)
register struct DSD_DBN *dsd_dbn;
register struct DSD_SBN **dsd_sbn;
register struct BZD     *bzd;
{
    register struct	DSD_DBN	*nextp;
    extern struct DSD_SBN *dcdsbn_set();
#ifdef	CONVERT_from_TOP
    register struct DSD_DBN *dsd_dbn_head;
    register struct BZD     *s_bzd;     /* Buffer son's bzd	*/
    dsd_dbn_head = dsd_dbn;
    while (bzd != 0) {
	nextp = dsd_dbn++;
    	nextp->bun_m = bzd->bend_m;
    	nextp->bun_jc = bzd->j_c;		
    	nextp->sbncnt = bzd->sbn_cnt;		
    	nextp->v_jc = bzd->v_jc;		
	
    	if (bzd->sbn->status == 0)
		bzd->sbn->status = WNN_SENTOU;

	nextp->sbn = *dsd_sbn;
	*dsd_sbn = dcdsbn_set(*dsd_sbn,bzd->sbn);
   
	s_bzd=bzd->lnk_son;
    	freebzd(bzd);
        bzd=s_bzd;
    }
    return(dsd_dbn_head); 
#else	/* CONVERT_from_TOP */
    if(bzd == 0)
      return(dsd_dbn);
    nextp = dcdbn_set(dsd_dbn, dsd_sbn, bzd->lnk_son);
    nextp->bun_m = bzd->bend_m;
    nextp->bun_jc = bzd->j_c;		
    nextp->sbncnt = bzd->sbn_cnt;		
    nextp->v_jc = bzd->v_jc;		

    if (bzd->sbn->status == 0)
	bzd->sbn->status = WNN_SENTOU;

    nextp->sbn = *dsd_sbn;
    *dsd_sbn = dcdsbn_set(*dsd_sbn,bzd->sbn);
    freebzd(bzd);
    return(++nextp);
#endif	/* CONVERT_from_TOP */
}

struct DSD_SBN *
dcdsbn_set(dsd_sbn, sbn)
register struct DSD_SBN *dsd_sbn;
register struct SYO_BNSETSU  *sbn;
{
    if ( sbn==0)
	return (dsd_sbn);
#ifdef	CONVERT_from_TOP
    dsd_sbn = dcdsbn_set(dsd_sbn,sbn->parent);
#endif	/* CONVERT_from_TOP */
    dsd_sbn->bun_m = sbn->bend_m;
    dsd_sbn->bun_jc = sbn->j_c;		
    dsd_sbn->i_jc   = sbn->i_jc;
    dsd_sbn->jentptr = sbn->jentptr;		
    dsd_sbn->t_jc = sbn->t_jc;		
    dsd_sbn->hinsi = sbn->hinsi_fk;		
    dsd_sbn->kangovect = sbn->kangovect;		
    dsd_sbn->v_jc = sbn->v_jc;		
    dsd_sbn->status = sbn->status;		
    dsd_sbn->status_bkwd = sbn->status_bkwd;		
    dsd_sbn++;
#ifndef	CONVERT_from_TOP
    dsd_sbn = dcdsbn_set(dsd_sbn,sbn->parent);
#endif	/* CONVERT_from_TOP */
    return(dsd_sbn);
}

/* 1 大文節中の小文節の数 */
int
cnt_syo(sbn)
register struct	SYO_BNSETSU *sbn;
{
    register int	cnt;
    cnt = 0;
    while(sbn) {
	cnt++;
	sbn = sbn->parent;
    }
    return (cnt);
}

/* 1 大文節の数 */
static void
cnt_bzd(bzd, dbn_cnt, sbn_cnt)
register struct	BZD *bzd;
register int	*dbn_cnt;
register int	*sbn_cnt;
{
    *dbn_cnt = 0;
    *sbn_cnt = 0;
    while(bzd) {
	*sbn_cnt += cnt_syo(bzd->sbn);
	(*dbn_cnt)++;
	bzd = bzd->lnk_son;
    }
}

int
chk_yomi_endvect(yomi_sno, yomi_eno, endvect, endvect1)
register int	yomi_sno;	/* 解析文字列 start index */
register int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
register int	endvect;	/* 終端ベクタ */
register int	endvect1;	/* 終端ベクタ */
{
    if (yomi_sno == yomi_eno || fzk_ckvt(endvect) == NO && fzk_ckvt(endvect1) == NO) {
	wnn_errorno = WNN_NO_KOUHO;
	error1("cannot make Tanbunsetu kouho.\n");
	return (-1);
    }
    return (0);
}

int
set_kata_giji_sbn(yomi_sno, yomi_eno, endvect, endvect1, sbn)
int	yomi_sno;	/* 解析文字列 start index */
int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
struct SYO_BNSETSU	**sbn;
{
    struct ICHBNP	*ichbnpbp;		/* ICHBNP のセーブ */	
    int	fzkcnt;
    register int	tempi;
    int connect_flg;
    extern	struct SYO_BNSETSU	*getsbnsp();

    if (chk_yomi_endvect(yomi_sno, yomi_eno, endvect, endvect1) < 0)
	return (-1);

    fzkcnt = fzk_kai(&bun[yomi_sno], &bun[yomi_eno], endvect, endvect1, &ichbnpbp);
    if (fzkcnt <= 0) {
	error1("Cannot make Tanbunsetu kouho in tan_syo.\n");
	init_work_areas();
	return (-1);	/* ERROR */
    }
    for(tempi = fzkcnt - 1; (int)tempi >= (int)0  ; tempi--){
       if (kan_ck_vector(giji_no, getfzkoh(ichbnpbp, tempi)->vector) == WNN_CONNECT_BK) {
	   connect_flg = YES;
	   break;
       }
    }
    if(tempi < 0) {
	for(tempi = fzkcnt - 1; (int)tempi >= (int)0  ; tempi--){
	   if (kan_ck_vector(giji_no, getfzkoh1(ichbnpbp, tempi)->vector) == WNN_CONNECT_BK) {
		 break;
	   }
	}
	if(tempi < 0) {
	    freeibsp(ichbnpbp);
	    wnn_errorno = WNN_NO_KOUHO;
	    error1("Cannot make Tanbunsetu kouho in tan_syo.\n");
	    return (-1);
	}
    }

    if ((*sbn = getsbnsp()) == 0)
	return (-1);

    (*sbn)->j_c = yomi_eno - 1;
    (*sbn)->i_jc = getfzkoh(ichbnpbp, tempi)->offset + yomi_sno;
    (*sbn)->bend_m = yomi_sno;
    (*sbn)->v_jc = 0;
    (*sbn)->jentptr = 0;
    (*sbn)->t_jc = WNN_KATAKANA;
    (*sbn)->kangovect = ft->kango_hinsi_area[giji_no];
    (*sbn)->hinsi_fk = giji_no;
    (*sbn)->status = WNN_GIJI;
    (*sbn)->status_bkwd = connect_flg;
    freeibsp(ichbnpbp);
    return(1);
}

int
set_kata_giji_bzd(yomi_sno, yomi_eno, endvect, endvect1, bzd, buncnt)
int	yomi_sno;	/* 解析文字列 start index */
int	yomi_eno;	/* 解析文字列 end index (end char no tugi) */
int	endvect;	/* 終端ベクタ */
int	endvect1;	/* 終端ベクタ */
struct	BZD	**bzd;
int	buncnt;
{
    extern	struct	BZD	*getbzdsp();

    if ((*bzd = getbzdsp()) == 0)
	return (-1);
    if (set_kata_giji_sbn(yomi_sno, yomi_eno, endvect, endvect1, &(*bzd)->sbn) < 0)
	return (-1);
    (*bzd)->j_c = yomi_eno - 1;
    (*bzd)->bend_m = yomi_sno;
    (*bzd)->v_jc = 0;
    (*bzd)->sbn_cnt = 1;
    (*bzd)->kbcnt = buncnt + 1;
    return(1);
}
