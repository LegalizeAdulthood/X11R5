/*
 * $Id: do_henkan1.c,v 1.2 1991/09/16 21:31:29 ohm Exp $
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
#include <stdio.h>
#include <ctype.h>

#include "demcom.h"
#include "commonhd.h"
#include "de_header.h"
#include "ddefine.h"
#include "jdata.h"
#include "fzk.h"

#include "kaiseki.h"
#ifdef SYSVR2
#include <fcntl.h>
#endif

#ifndef NULL
#define NULL 0
#endif

#define FZK_L 256
#ifdef	putwchar
#undef	putwchar
#endif

static int bun_pter_;

static void
henkan_setup()
{
    register int index;
    register w_char c;

    wnn_loadhinsi(hinsi_file_name);
#ifdef	CONVERT_from_TOP
    index = 1;
#else
    index = maxchg-1;
#endif
    while((c = get2_cur()) != 0){
#ifdef	DEBUG
	putwchar(c);
#endif
	if (make_buns_par(c,index) == -1){
	    break;
	}
#ifdef	CONVERT_from_TOP
	index ++;
#else
	index--;
#endif
    }
#ifdef	CONVERT_from_TOP
    bun_pter_ = index ;
#else
    bun_pter_ = index + 1;
#endif
    init_jmt(0);
#ifdef	DEBUG
    wsputs("\n");
#endif
}

static void
get_fzk_vec(hinsi,fzk,vec,vec1)
register int *hinsi;
register int *vec;
register int *vec1;
register w_char *fzk;
{
    *hinsi=get4_cur();
    getws_cur(fzk);
    *vec=get4_cur();
    *vec1=get4_cur();
}

#ifdef CHINESE
static void
do_question(daip, zenp)
int daip, zenp;
{
    int b_suu_;

    b_suu_ = jishobiki_b(1,bun_pter_,0);
    if (b_suu_ < 0){
	error_ret();    /* ERROR */
	return;
    }
    if (zenp == WNN_BUN)
	b_suu_ = (b_suu_ == 0) ? 0:1;
    if(daip == WNN_DAI)
	ret_daiB(1, bun_pter_, b_suu_, zenp);
    else
	ret_B(1, bun_pter_, b_suu_, zenp);
    putc_purge();
}
#endif

/* RENBUN */
void
do_kanren()
{
    int end,eid;
    int vec;
    int vec1;
    int vec2;
    w_char fzk[FZK_L];
    struct DSD_DBN *d_list;
    register int b_suu_;
    int hinsi;

    eid = get4_cur();
    c_env=env[eid];
    end = maxchg - 1;
    henkan_setup();
    get_fzk_vec(&hinsi,fzk,&vec,&vec1);
    vec2 = get4_cur();

#ifdef CHINESE
    if (is_bwnn_rev_dict()) {
	do_question(WNN_DAI, WNN_BUN);
	return;
    }
#endif /* CHINESE */

    b_suu_ = renbn_kai(
#ifdef	CONVERT_from_TOP
		 1, bun_pter_,
#else
		 bun_pter_, end + 1,
#endif
		 hinsi,
#ifndef	NO_FZK
		 fzk,
#endif
		 vec, vec1, vec2, c_env->nbun, c_env->nshobun, &d_list);
    if (b_suu_ < 0){
	error_ret();	/* ERROR */
	return;
    }
    ret_dai(d_list, b_suu_);
    putc_purge();
#ifdef	DEBUG
    print_dlist(d_list, b_suu_);
#endif
}

/* TANBUN (dai)*/
void
do_kantan_dai()
{
    int end, eid;
    int vec;
    int vec1;
    w_char fzk[FZK_L];
    struct DSD_DBN *d_list;
    register int b_suu_;
    int hinsi;

    eid = get4_cur();
    c_env=env[eid];
    end = maxchg - 1;
    henkan_setup();
    get_fzk_vec(&hinsi,fzk,&vec,&vec1);

#ifdef CHINESE
    if (is_bwnn_rev_dict()) {
	do_question(WNN_DAI, WNN_BUN);
	return;
    }
#endif /* CHINESE */
    b_suu_ = tan_dai(
#ifdef	CONVERT_from_TOP
		     1, bun_pter_,
#else
		     bun_pter_, end + 1,
#endif
		     hinsi,
#ifndef	NO_FZK
		     fzk,
#endif
		     vec, vec1, c_env->nshobun, &d_list);
    if (b_suu_ < 0){
	error_ret();	/* ERROR */
	return;
    }
    ret_dai(d_list, b_suu_);
    putc_purge();
#ifdef	DEBUG
    print_dlist(d_list, b_suu_);
#endif
}

/* TANBUN (sho)*/
void
do_kantan_sho()
{
    int end, eid;
    int vec;
    int vec1;
    w_char fzk[FZK_L];
    struct DSD_SBN *d_list1;
    register int b_suu_;
    int hinsi;

    eid = get4_cur();
    c_env=env[eid];
    end = maxchg - 1;
    henkan_setup();
    get_fzk_vec(&hinsi,fzk,&vec,&vec1);

#ifdef CHINESE
    if (is_bwnn_rev_dict()) {
	do_question(WNN_SHO, WNN_BUN);
	return;
    }
#endif /* CHINESE */
    b_suu_ = tan_syo(
#ifdef	CONVERT_from_TOP
		 1, bun_pter_,
#else
		 bun_pter_, end + 1,
#endif
		 hinsi,
#ifndef	NO_FZK
		 fzk,
#endif
		 vec, vec1, &d_list1);
    if (b_suu_ < 0){
	error_ret();	/* ERROR */
	return;
    }
    ret_sho(d_list1, b_suu_);
    putc_purge();
#ifdef	DEBUG
    print_dlist1(d_list1, b_suu_);
#endif
}

/* JIKOUHO (dai)*/
void
do_kanzen_dai()
{
    int end, eid;
    int vec;
    int vec1;
    w_char fzk[FZK_L];
    struct DSD_DBN *d_list;
    register int b_suu_;
    int hinsi;

    eid = get4_cur();
    c_env=env[eid];
    end = maxchg - 1;
    henkan_setup();
    get_fzk_vec(&hinsi,fzk,&vec,&vec1);

#ifdef CHINESE
    if (is_bwnn_rev_dict()) {
	do_question(WNN_DAI, WNN_ZENKOUHO);
	return;
    }
#endif /* CHINESE */
    b_suu_ = jkt_get_dai(
#ifdef	CONVERT_from_TOP
			 1,bun_pter_,
#else
			 bun_pter_, end + 1,
#endif
			 hinsi,
#ifndef	NO_FZK
			 fzk,
#endif
			 vec, vec1, c_env->nshobun, &d_list);
    if (b_suu_ < 0){
	error_ret();	/* ERROR */
	return;
    }
    ret_dai(d_list, b_suu_);
    putc_purge();
#ifdef	DEBUG
    print_dlist(d_list, b_suu_);
#endif
}

/* JIKOUHO (sho)*/
void
do_kanzen_sho()
{
    int end, eid;
    int vec;
    int vec1;
    w_char fzk[FZK_L];
    struct DSD_SBN *d_list1;
    register int b_suu_;
    int hinsi;

    eid = get4_cur();
    c_env=env[eid];
    end = maxchg - 1;
    henkan_setup();
    get_fzk_vec(&hinsi,fzk,&vec,&vec1);

#ifdef CHINESE
    if (is_bwnn_rev_dict()) {
	do_question(WNN_SHO, WNN_ZENKOUHO);
	return;
    }
#endif /* CHINESE */
    b_suu_ = jkt_get_syo(
#ifdef	CONVERT_from_TOP
		     1, bun_pter_,
#else
		     bun_pter_, end + 1,
#endif
		     hinsi,
#ifndef	NO_FZK
		     fzk,
#endif
		     vec, vec1, &d_list1);
    if (b_suu_ < 0){
	error_ret();	/* ERROR */
	return;
    }
#ifdef	DEBUG
    print_dlist1(d_list1, b_suu_);
#endif
    ret_sho(d_list1, b_suu_);
    putc_purge();
}

int
make_buns_par (c, at_index)	/* make bun and bun	*/
register w_char c;
register int at_index;
{
	if ((at_index >= maxchg) || (at_index <= 0)){
	    wnn_errorno = WNN_LONG_MOJIRETSU;
	    return(-1);
	}
	bun[at_index] = c;

	jmtp[at_index] = (struct jdata **) -1;	/* 辞書をまだ引いていない */
	return(0);
}

/* get kanji and put it in  kouho*/
void
Get_knj(jentptr, u, kouho, oy, oyl) 
int u;
register struct jdata *jentptr;
w_char *kouho, *oy;
int oyl;
{
    UCHAR *kptr;
    register struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    if(jentptr->which == D_YOMI){
	Get_knj1(kptr, oy, oyl, jentptr->kanji2 + u, kouho, NULL, NULL);
    }else{
	Get_knj1(kptr, oy, oyl, jentptr->kanji2 + u, NULL, kouho, NULL);
    }
}

#ifdef	no_def
void
get_yomi(jentptr, u, kouho)/* get yomi and put it in  kouho*/
int u;
register struct jdata *jentptr;
w_char *kouho;
{
    UCHAR *kptr;
    register struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    Get_knj2(kptr, jentptr->kanji2 + u, NULL, kouho, NULL);
}
#endif

/* get comment and put it in  kouho*/
void
get_knj_com(jentptr, u, kouho, com)
int u;
register struct jdata *jentptr;
w_char *kouho;		/* kouho must be ended with NULL.	*/
w_char *com;
{
    register UCHAR *kptr;
    struct JT	*jtl;

    jtl = (struct JT *)(files[dic_table[jentptr->jishono].body].area);
    kptr = jtl->kanji + jentptr->kanji1;
    Get_knj2(kptr, jentptr->kanji2 + u, kouho, NULL,  com);
}
