/*
 * $Id: ddefine.h,v 1.2 1991/09/16 21:31:13 ohm Exp $
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
/************************
 * define of daemon
 ************************/

/*
	���!! ���!! ���!! ���!! ���!! ���!! ���!!
	<ctype.h> �� ���󥯥롼�ɤ���Ȥ��ϡ�
	���Υե���������˥��󥯥롼�ɤ��뤳��
*/

#define SIZE_JISHOTABLE 1000  
#define SIZE_JISHOHEAP 2000

#define	KANGO_HINSI_MX	256		/* ������ʻ���κ���� */
#define	KANGO_VECT_L (KANGO_HINSI_MX+8*4-1)/(8*4) /* ������³�٥�����Ĺ�� */
#define	FZKVECT_L	5		/* ��°��٥�����Ĺ�� */
#define	VECT_L	KANGO_VECT_L + FZKVECT_L /* ��³�٥�����Ĺ�� */
#ifdef	CHINESE
#define	KANGO_VECT_KOSUU	256	/* ������³�٥����κ���� */
#else
#define	KANGO_VECT_KOSUU	20	/* ������³�٥����κ���� */
#endif	/* CHINESE */

struct	fzkkouho {		/* used in reffering to fuzokugo jouhou 
				 fuzokugo jouhou is stored using this struct*/
	int	offset;
	int	vector[KANGO_VECT_L];	/* ���콪ü�٥��� */
};

/* macro C(i , j)  is useful for finding a pointer which points out 
   the first jishoentry whose yomi starts at i and endes at j
   which returns NULL if there is no entry which matches this
*/

#define	C(i,j)	(j > maxj[i] ? (struct jdata *)NULL : *(jmtp[i] + (j - i)))


/* #define	FZKIBNO	10 */
#define	FZKIBNO	5

/* ��°����Ϸ�� */
struct	ICHBNP {
	struct	ICHBNP	*next_p;
	struct	fzkkouho fzkib[FZKIBNO];	/* vec �β��Ϸ�� */
	struct	fzkkouho fzkib1[FZKIBNO];	/* vec1 �β��Ϸ�� */
};
/* �¤ϡ�fzkib1->offset �ϡ����ߤ����äƤ��� */

#define	SV_KOSUU	3

#define	UJIS_A		0xa3c1	/* �� */
#define UJIS_Z		0xa3da	/* �� */
#define	UJIS_a		0xa3e1	/* �� */
#define	UJIS_z		0xa3fa	/* �� */
#ifdef CHINESE
#define	UJIS_comma	0xa3ac	/* �� */
#define	S_PAREN		0xa1ae	/* �� */
#define	E_PAREN		0xa1bf	/* �� */
#else
#define	UJIS_comma	0xa1a4	/* �� */
#define	S_PAREN		0xa1c6	/* �� */
#define	E_PAREN		0xa1db	/* �� */
#endif /* CHINESE */
/*
#define	isdigit(x)	((unsigned)((x) - '0') <= ('9' - '0'))
#define	isalpha(x)	((unsigned)((x) - 'A') <= ('Z' - 'A') || \
			    (unsigned)((x) - 'a') <= ('z' - 'a'))
#define	isalnum(x)	(isalpha(x) || isdigit(x))
*/
#ifdef	isspace
#	undef	isspace	/* ctype.h �� isspace() �ϻȤ�ʤ� */
#endif
#define	isspace(x)	(((x) == ' ') || ((x) == 0x09))
#define	isjspace(x)	((x) == 0xa1a1)
#define	isparen_s(x)	(/* (x) == '\"' || */ (x) == '`' || \
/* ����� */		 (x) == '(' || (x) == '<' || \
			 (x) == '[' || (x) == '{')
#define	isparen_e(x)	((x) == '\"' || (x) == '\'' || \
/* �ĳ�� */		 (x) == ')' || (x) == '>' || \
			 (x) == ']' || (x) == '}')

#define	isjdigit(x)	((unsigned)((x) - S_NUM) <= (E_NUM - S_NUM))
#define	isjalpha(x)	((unsigned)((x) - UJIS_A) <= (UJIS_Z - UJIS_A) || \
			 (unsigned)((x) - UJIS_a) <= (UJIS_z - UJIS_a))
#define	isjalnum(x)	(isjalpha(x) || isjdigit(x))
#define	isjhira(x)	((unsigned)((x) - S_HIRA) <= (E_HIRA - S_HIRA))
#define	isjkata(x)	((unsigned)((x) - S_KATA) <= (E_KATA - S_KATA))
#define	isj1bytekana(x)	((unsigned)((x) - S_HANKATA) <= (E_HANKATA - S_HANKATA))
# define S_KANJI	0xb0a1	/* 16�� */
# define E_KANJI	0xfefe	/* 94�� */
#define	isjkanji(x)	((unsigned)((x) - S_KANJI) <= (E_KANJI - S_KANJI))
#ifdef	CHINESE
#define	isjparen(x)	((unsigned)((x) - S_PAREN) <= (E_PAREN - S_PAREN) || \
			 (x)==0xa3a8 || (x)==0xa3a9 ||  \
			 (x)==0xa3bc || (x)==0xa3be || \
			 (x)==0xa3db || (x)==0xa3dd || \
			 (x)==0xa3fb || (x)==0xa3fd )
			/*  for (  )  <  >  [  ]  { } */
#define	isjparen_s(x)	(((unsigned)((x) - S_PAREN) <= (E_PAREN - S_PAREN)) && \
			     (((x) & 0x01) == 0) || \
			 (x)==0xa3a8 || (x)==0xa3bc || \
			 (x)==0xa3db || (x)==0xa3fb )

#define	isjparen_e(x)	(((unsigned)((x) - S_PAREN) <= (E_PAREN - S_PAREN)) && \
			     (((x) & 0x01) == 1) || \
			 (x)==0xa3a9 || (x)==0xa3be || \
			 (x)==0xa3dd || (x)==0xa3fd )
#else	/* JAPANESE */
#define	isjparen(x)	((unsigned)((x) - S_PAREN) <= (E_PAREN - S_PAREN))
#define	isjparen_s(x)	(isjparen(x) && (((x) & 0x01) == 0))
#define	isjparen_e(x)	(isjparen(x) && ((x) & 0x01))
#endif	/* CHINESE */

#define	UJIS_DKT	0xa1ab		/* �� */
#define	UJIS_HDK	0xa1ac		/* �� */

#define	BAR_CODE(x)	((x) == BAR)
#define	TOKU(x)	(((x) == UJIS_DKT) || ((x) == UJIS_HDK))

#define	TOKUSYU(x)	(TOKU(x) || isjkata(x) || isj1bytekana(x))
	/* this resembles macro before */

#define	kuten(x)	(((x)==KUTEN_NUM)? True:False)
#define	touten(x)	(((x)==TOUTEN_NUM)? True:False)

/* �������ü��³�٥��ȥ�ι�¤ */
struct	kangovect	{
	unsigned int	vector[VECT_L];		/* ����٥��ȥ� */
};

struct	SYO_BNSETSU {
	struct	SYO_BNSETSU	*lnk_br;	/* ¾�ξ�ʸ��ؤΥݥ��� */
	struct	SYO_BNSETSU	*parent;	/* ������ξ�ʸ��ؤΥݥ��� */
	short	j_c;			/* ���� top index */
	short	i_jc;			/* ���� end index */
	short	bend_m;			/* ��ʸ�� end char index */
	int	v_jc;			/* ����ʸ���ɾ���� */
	int	son_v;			/* ���ξ�ʸ��ޤǤ�ɾ���ͤι�� */
	struct	jdata	*jentptr;	/* ����ǡ����ؤΥݥ��� */
	int	t_jc;			/* �ʻ졢���پ���Υ���ǥå��� */
	int	kangovect;		/* ��³�٥��ȥ�ơ��֥�Υ���ǥå��� */
	int	hinsi_fk;		/* ������ʻ�No.(ʣ���ʻ��ޤ�) */
	short	kbcnt;			/* ��ʸ����ǲ���ʸ���ܤ� */
	short	reference;		/* ���ξ�ʸ��Υ�ե���󥹿� */
	short	status;			/* ����ʸ�����³�Ǥ��뤫 */
	short	status_bkwd;		/* ���ʸ��Ȥ���³�Ǥ��뤫 */
};

struct	BZD	{
	struct	BZD	*lnk_br;	/* ¾����ʸ����� */
	struct	BZD	*lnk_son;	/* ������ʸ��ؤ� pointer */
	short	j_c;			/* ���� top index */
	short	bend_m;			/* ������ʸ��� end char index */
	int	v_jc;			/* ������ʸ���ɾ���� */
	int	son_v;			/* ������ʸ���ɾ���� */
	struct	SYO_BNSETSU *sbn;	/* ��Ƭ�ξ�ʸ�� */
	short	sbn_cnt;		/* ������ʸ��������뾮ʸ��ο� */
	short	kbcnt;			/* ������ʸ������˲�ʸ����Ϥ�����  */
};

struct	JKT_SONE {
	struct	JKT_SONE *lnk_br;	/* ¾�ξ�ʸ��ؤΥݥ��� */
	short	i_jc;			/* ���� end index */
	short	v_jc;			/* ����ʸ���ɾ���� */
	struct	jdata	*jentptr;	/* ����ǡ����ؤΥݥ��� */
	int	t_jc;			/* �ʻ졢���پ���Υ���ǥå��� */
	int	hinsi_fk;		/* ������ʻ�No.(ʣ���ʻ��ޤ�) */
	short	status_bkwd;		/* ���ʸ��Ȥ���³�Ǥ��뤫 */
};

struct	JKT_SBN {
	struct	JKT_SBN	*lnk_br;	/* ¾�ξ�ʸ��ؤΥݥ��� */
	struct	JKT_SBN	*parent;	/* ������ξ�ʸ��ؤΥݥ��� */
	short	j_c;			/* ���� top index */
	short	bend_m;			/* ��ʸ�� end char index */
	struct	JKT_SONE *sbn;		/* ¾�ξ�ʸ��ؤΥݥ��� */
	int	kangovect;		/* ��³�٥��ȥ�ơ��֥�Υ���ǥå��� */
	short	kbcnt;			/* ��ʸ����ǲ���ʸ���ܤ�  */
	short	reference;		/* ���ξ�ʸ��Υ�ե���󥹿� */
	short	status;			/* ����ʸ�����³�Ǥ��뤫 */
};

struct	JKT_DBN {
	struct	JKT_DBN	*lnk_br;	/* ¾�ξ�ʸ��ؤΥݥ��� */
	short	j_c;			/* ���� top index */
	short	bend_m;			/* ��ʸ�� end char index */
	struct	JKT_SBN *sbn;		/* ¾�ξ�ʸ��ؤΥݥ��� */
	int	v_jc;			/* ����ɾ����ɾ���� */
	short	sbn_cnt;		/* ������ʸ��������뾮ʸ��ο� */
};

struct	DSD_SBN {
	short	bun_m;		/* ����ʸ��� end char index */
	short	bun_jc;		/* ����ʸ��� top char index */
	short	i_jc;		/* ����ʸ��μ�Ω�� end char index */
	struct	jdata *jentptr;	/* ����ʸ��μ��� entry pointer */
	int 	t_jc;		/* ���伫Ω����ʻ� & ���� index */ 
	int	hinsi;		/* �ʻ� */
	int	kangovect;	/* ��³�٥��ȥ�ơ��֥�Υ���ǥå��� */
	int	v_jc;		/* ��ʸ���ɾ���� */
	short	status;		/* ����ʸ�����³�Ǥ��뤫 */
	short	status_bkwd;	/* ���ʸ��Ȥ���³�Ǥ��뤫 */
};

struct	DSD_DBN {
	short	bun_m;		/* ����ʸ��� end char index */
	short	bun_jc;		/* ����ʸ��� top char index */
	struct	DSD_SBN	*sbn;	/* ��ʸ����Ϸ�̤ؤΥݥ��� */
	short	sbncnt;		/* ��ʸ��� (������ξ��ϡ��������)
				   DSD_SBN�ϡ�*sbn ���� sbncnt �������� */
	int	v_jc;		/* ��ʸ���ɾ���� */
};

#ifdef	nodef	/* move to jslib.h */
/* ����ʸ���ľ�������򤷤����� */
#define HIRAGANA	-1	/* �Ҥ餬�� *//* �ɤߤΤޤ� */
#define KATAKANA	-11	/* �������� */
/* ���� */
#define	NUM_HAN		-2	/* Ⱦ�ѿ��� *//* �ɤߤΤޤ� */
#define	NUM_ZEN		-12	/* ���ѿ��� *//* ������ */
#define	NUM_KAN		-13	/* ������ *//* ���� */
#define	NUM_KANSUUJI	-15	/* ������ *//* ɴ�󽽻� */
#define	NUM_KANOLD	-16	/* ������ *//* ��ɴ������ */
#define	NUM_HANCAN	-17	/* Ⱦ�ѿ��� *//* 1,234 */
#define	NUM_ZENCAN	-18	/* ���ѿ��� *//* ���������� */
/* �ѿ� */
#define	ALP_HAN		-4	/* Ⱦ�� *//* �ɤߤΤޤ� */
#define	ALP_ZEN		-30	/* ���� */
/* ���� */
#define	KIG_HAN		-5	/* Ⱦ�� *//* �ɤߤΤޤ� */
#define	KIG_JIS		-40	/* ����(JIS) */
#define	KIG_ASC		-41	/* ����(ASC) */
#endif	

#define	YES		1
#define	NO		0
#define	UN_KNOWN	-1

#define	NOTHING		0
