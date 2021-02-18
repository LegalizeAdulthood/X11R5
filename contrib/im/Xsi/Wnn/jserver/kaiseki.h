/*
 * $Id: kaiseki.h,v 1.2 1991/09/16 21:31:51 ohm Exp $
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
/*********************
 *  header of daemon 
**********************/
#define	WNN_BUN		0
#define	WNN_ZENKOUHO	1
#define	WNN_SHO		0
#define	WNN_DAI		1


#define	_MININT	0x80000000
#define	HIRA_HYOUKA -2000;	/* ��������Ф��λ���ʿ��̾��ɾ���� */
#define	KATA_HYOUKA -1000;	/* ��������Ф��λ����Ҳ�̾��ɾ���� */

#define	_DIVID(val, div)	(((int)(val) > (int)0) ? ((val) / (div)) : ((val) * (div)))

extern struct fzkkouho *getfzkoh_body();
extern struct fzkkouho *getfzkoh1_body();

#define	getfzkoh(ichbnp_p,no) \
	(((int)(no) < (int)FZKIBNO) ? &((ichbnp_p)->fzkib[no]) : getfzkoh_body(ichbnp_p,no))

#define	getfzkoh1(ichbnp_p,no) \
	(((int)(no) < (int)FZKIBNO) ? &((ichbnp_p)->fzkib1[no]) : getfzkoh1_body(ichbnp_p,no))

#define kan_ck_vector(h, v) \
    (((((int)*((v) + ((h) / (sizeof(int)<<3))) >> (int)((int)(h) % (int)((int)sizeof(int)<<(int)3))) & 1) == 1) ? \
	WNN_CONNECT_BK : WNN_NOT_CONNECT_BK)

extern	struct	FT	*ft;		/* �����Ȥ���°��ơ��֥� */

/* ���ϥ��ꥢ�δ����ѤΥꥹ�� */
struct	free_list {
	struct	free_list *lnk;
	int	num;
};

#define	FIRST_BZD_KOSUU	200		/* �ǽ�˼��Ф���ʸ����ϥ��ꥢ */
#define	BZD_KOSUU	20 		/* ­��󤫤ä����դ��ä����礭�� */

#define FIRST_SBN_KOSUU 200		/* �ǽ�˼��Ф���ʸ����ϥ��ꥢ */
#define SBN_KOSUU	20		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_ICHBN_KOSUU	20	/* �ǽ�˼��Ф���°����ϥ��ꥢ */
#define	ICHBN_KOSUU	4 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKDBN_KOSUU	50	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKDBN_KOSUU	10 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKSBN_KOSUU	50	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKSBN_KOSUU	10 		/* ­��󤫤ä����դ��ä����礭�� */

#define	FIRST_JKSONE_KOSUU	300	/* �ǽ�˼��Ф���ʸ�ἡ���䥨�ꥢ */
#define	JKSONE_KOSUU	20 		/* ­��󤫤ä����դ��ä����礭�� */

/************************************************
 *	����ʸ�����ɾ���롼����		*
 ************************************************/
/*
#define	HYOKAVAL(jiritugo_len, bnst_len) \
		hyoka1(0x80, 0, (jiritugo_len), (bnst_len), 0)
*/
/*
#define	HYOKAVAL(hindo, jiritugo_len, bnst_len) \
		hyoka1(0, hindo, (jiritugo_len), (bnst_len), 0)
*/
#define	HYOKAVAL(hindo, jiritugo_len, bnst_len) \
  (c_env->hindoval * (hindo) \
   + c_env->jirival * (jiritugo_len) \
   + c_env->lenval * (bnst_len) )

/************************************************
 *	��ʸ�����ɾ���롼����			*
 ************************************************/
#define hyoka1(HINDO, HINDO_IN, LENGTH, BUNLNGTH, PRIO) \
  (c_env->hindoval * (((HINDO) & 0x7f) + ((HINDO_IN) & 0x7f)) \
   + c_env->jirival * (LENGTH) \
   + c_env->lenval * (BUNLNGTH) \
   + c_env->jishoval * (PRIO)\
   + c_env->flagval * ((int)((HINDO) & 0x80) >> (int)7))

/************************************************
 *	��ʸ�����ɾ���롼����			*
 ************************************************/
/* ��ʸ���ɾ����
	sbn_val * ��ʸ���ɾ���ͤ��� / ��ʸ���
	   + sbn_cnt_val * ��ʸ���
	   + dbn_len_val * ��ʸ��Ĺ
*/
#define hyoka_dbn(SON_V, SBNCNT, DAILEN) \
	((c_env->sbn_val * (SON_V)) / (SBNCNT) \
	 + c_env->sbn_cnt_val * (SBNCNT) \
	 + c_env->dbn_len_val * (DAILEN))

/* ��ʸ���ɾ���ͤ�����ʸ�����³�Ǥ��뤫�ݤ����Ѥ��롣
   divid :  1	����ʸ�����³�Ǥ���	hyouka * 2
	    0	��ʸ�����Ƭ		hyouka
	   -1	����ʸ�����³�Ǥ��ʤ�	hyouka / 2
*/
#define	DIVID_HYOUKA(hyouka, divid) \
	(((divid) == 0) ? (hyouka) : \
		(((divid)) > 0 ? ((hyouka) << 1) : ((hyouka) >> 1)))

/*********************
 * kaiseki work area
**********************/
extern	w_char	*bun;	/* D */

extern	w_char giji_eisuu[];	/* �����ֱѿ��פ���� */

extern	int	maxchg;
extern	int	initjmt;	/* I think initjmt is the length of jmt_ */

extern	int	*maxj;	/* maxj is counts to entries in jmt_ */

/* jmt_ptr is used in jmt0.c only.
   but it must be changed by clients.
   jishobiki does not use it. this is sent to jishobiki by arguments
   */

/* j_e_p is used to hold the current point to which jmtw_ is used */
extern	struct	jdata	*j_e_p;
extern	struct	jdata	**jmt_;
extern	struct	jdata	*jmtw_;
extern	struct	jdata	**jmt_end;
extern	struct	jdata	*jmtw_end;
extern	struct	jdata	**jmt_ptr;
extern	struct	jdata	***jmtp;

/********************************
 *	������Ω����ʻ�	*
 ********************************/
#ifdef	nodef
extern	int	sentou_no;
extern	int	suuji_no;
extern	int	katakanago_no;
extern	int	eisuu_no;
extern	int	kigou_no;
extern	int	toji_kakko_no;
extern	int	fuzokugo_no;
extern	int	kai_kakko_no;
extern	int	giji_no;
#endif	/* nodef */

#define	sentou_no	0
#define	suuji_no	1
#define	katakanago_no	2
#define	eisuu_no	3
#define	kigou_no	4
#define	toji_kakko_no	5
#define	fuzokugo_no	6
#define	kai_kakko_no	7
#define	giji_no		8
