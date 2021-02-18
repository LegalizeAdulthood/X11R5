/*
 * $Id: rk_main.c,v 1.2 1991/09/16 21:34:46 ohm Exp $
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
			rk_main.c
						87.12. 3  �� ��

	���޻������Ѵ�������¾�����ϥ����ɤ��Ѵ�����ץ���ࡣ
	ASCII�����ɻ��Ѥ�����Ȥ��Ƥ��롣
	���Υե�����ϡ��Ѵ��Υᥤ��롼����
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef	MULTI
#include "rk_multi.h"
#endif	/*MULTI*/
#endif
#include "rext.h"

#ifdef	MULTI
#define	DISOUT ((cur_rk->flags & RK_NONISE) ? cur_rk->rk_output : cur_rk->disout)
 /* �ե饰���ͤ�romkan_init3()��������ꤵ�졢����RK_NONISE�ΥӥåȤ��ͤ�
    ɸ���0�����ä�DISOUT��ɸ���ͤ�disout�������rk_output���Ѥ���ȡ���������
    �ʽ���LTREOF�ˤ���ڽФ��ʤ���â�����������Ϥ������ɬ���������֤��褦��
    ���Ƥ�����˸¤ꡢEOLTTR����֤��ˡ�*/

#else	/* !MULTI */
#ifdef KDSP
#	ifdef MVUX
letter	displine[DSPLIN]; /* �ǥХå���  ɽ���ΰ��ʬ�򵭲� */
#	endif
#endif

/* 88/06/02 V3.1 */
static	letter	nil[1] = {EOLTTR};
static	letter	*curdis;
static	int	codein_len;
static	letter	*codeout, *remainkbf;
static	letter	ungetc_buf = EOLTTR; /* romkan_ungetc����ʸ�����򤷤Ƥ����Хåե� */
static	letter	unnext_buf = EOLTTR; /* romkan_unnext��		   ��		  */

static	letter	(*keyin_method)(); /* ��������ؿ��Υ��ɥ쥹 */
static	int	(*bytcnt_method)(); /* �Х��ȥ�����ȴؿ��Υ��ɥ쥹 */
static	int	(*kbytcnt_method)(); /* �������ϲ���ѥХ��ȥ�����ȴؿ��Υ��ɥ쥹 */
 
/* �⡼��ɽ̾�����Ϥ��Υѥ�̾�ˤ���¸ */ /* ������꤬�����ø�! */
static	char	prv_modfnm[REALFN] = "\0";

#define	DISOUT ((flags & RK_NONISE) ? rk_output : disout)
 /* �ե饰���ͤ�romkan_init3()��������ꤵ�졢����RK_NONISE�ΥӥåȤ��ͤ�
    ɸ���0�����ä�DISOUT��ɸ���ͤ�disout�������rk_output���Ѥ���ȡ���������
    �ʽ���LTREOF�ˤ���ڽФ��ʤ���â�����������Ϥ������ɬ���������֤��褦��
    ���Ƥ�����˸¤ꡢEOLTTR����֤��ˡ�*/

static /* V3.1 */
char	eofflg; /* romkan_next()����LTREOF���褿�Ȥ��ˡ�romkan_henkan()���1
		   �ˤ��롣������ͤ���0�ʤ��Ѵ��б�ɽ��(error)��̵�뤷�ơ�
		   �ܽ����Хåե����̤���ꥳ���ɤ⡢����Ū���Ѵ������롣����
		   LTREOF���褿�������̤ˤ�����Ϥ�Ф��褦�˻��ꤵ��Ƥ����
		   ���ν�����Ԥ���â�������ν����ϵޤ����館���Դ����ˡ�����
		   �������eofflg��2��*/

static	letter	evalbuf[2][2][OUTSIZ];
static	letter	delchr, delchr2 = EOLTTR, nisedl; /* DEL�Υ���饯������Ĥޤǻ��Ƥ�*/

/* 88/05/31 V3.1 */
static	letter	rk_input;  /* ���ϡ�3�Х��ȤޤǤ��ѹ��ʤ����б��� */
static	letter	disout[OUTSIZ];  /* �ܽ�������ν��ϤΥХåե� */
static	letter	rk_output[OUTSIZ]; /* ���������ν��ϡʺǽ����ϡˤΥХåե� */
static	letter	keybuf[KBFSIZ], urabuf[KBFSIZ];  /* �ܽ����Хåե��Ȥ��������ΰ� */
static	int	lastoutlen, lastkbflen;
static	int	hyonum;
#endif	/* MULTI */

static void romkan_delete(), mchevl(), add_at_eof(), free_for_all_area(),
  ltrevlcpy(), set_rubout(), maeato_henkan(), codeout_chg();

/* letter���֤��ؿ�������ڤ�����ʰ����ˤ�char�Τ�Τ⺮���äƤ���� */

static int	head_bytecount(), p_eq(), hen_ikisrc(), match(),
  henkan_ok(), romkan_restart();
static letter	mchedsrc();
letter	romkan_next(), romkan_unnext();
letter	romkan_getc(), romkan_ungetc(), *romkan_henkan();
letter	to_zenalpha(), to_zenhira(), to_zenkata();
 /* to_zenhira;kata �����������ʸ�����ޤȤ�ˤ��ʤ���*/

static letter	*ltrcpy(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	while((*lp1++ = *lp2++) != EOLTTR);
	return(org);
}

static letter	*ltrncpy(lp1, lp2, n) /* ������'EOLTTR'�򥻥å� */
letter	*lp1, *lp2;
int	n;	
{
	fast	letter	*org;

	org = lp1;
	for(; n; --n) if(EOLTTR == (*lp1++ = *lp2++)) return(org);
	*lp1 = EOLTTR;
	return(org);
}

 /** letter����������˰�ʸ���Ĥʤ��� */
static /* V3.1 */
letter	*ltr1cat(lp, l)
fast	letter	*lp, l;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp;
	totail(lp);
	*lp++ = l;
	*lp = EOLTTR;
	return(org);
}

 /** ʸ����������˰�ʸ���Ĥʤ��롣ltr1cat��char�� */
#ifdef OMRON_LIB
static
#endif
char	*chrcat(s, c)
fast	char	*s, c;	/* V3.1 */
{
	fast	char	*org;	/* V3.1 */

	org = s;
	strtail(s);
	*s++ = c;
	*s = '\0';
	return(org);
}

static letter	*ltrcat(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	totail(lp1);
	ltrcpy(lp1, lp2);
	return(org);
}

 /** letter����lp2�γ����ǤκǾ�̥ӥåȤ�Ω�ƤƤ��顢lp1�θ��ˤĤʤ��롣*/
static /* V3.1 */
letter	*bitup_ltrcat(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	org = lp1;
	totail(lp1);

	while((*lp1 = *lp2++) != EOLTTR) *lp1++ |= HG1BIT;
	 /** lp2 ���������ƤκǾ�̥ӥåȤ�Ω�Ƥ롣*lp2��NISEBP(rk_spclval.h
	     ��define)�ʤɤΤȤ��Ϻǽ餫��Ǿ�̤�Ω�äƤ뤫��褤 */

	return(org);
}

 /** ltrcat�������ȡ���̤�ʸ����������˥ݥ��󥿤��äƤ�����*/
#ifdef OMRON_LIB
static
#endif
letter	*ltrgrow(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	totail(lp1);
	while((*lp1 = *lp2++) != EOLTTR) lp1++;
	return(lp1);
}

int
ltrlen(lp)
fast	letter	*lp;	/* V3.1 */
{
	fast	letter	*org;	/* V3.1 */

	for(org = lp; *lp != EOLTTR; lp++);
	return(lp - org);
}

 /**	letter����κǸ��ʸ���ؤΥݥ��󥿤��֤���â����ʸ������Ф��Ƥ�
	���Τޤ��֤���*/
static
letter	*ltrend(lp)
fast	letter	*lp;	/* V3.1 */
{
	return((*lp != EOLTTR) ? (lp + ltrlen(lp) - 1) : lp);
}

 /**	ʸ����κǸ��ʸ���ؤΥݥ��󥿤��֤���ltrend()��char�ǡ�ltrend()��
	Ʊ�͡���ʸ����ΤȤ������㤬���롣*/
#ifdef OMRON_LIB
static
#endif
char	*strend(s)
fast	char	*s;	/* V3.1 */
{
	return(*s ? (s + strlen(s) - 1) : s);
}

int
ltrcmp(lp1, lp2)
fast	letter	*lp1, *lp2;	/* V3.1 */
{
	for(; *lp1 == *lp2; lp1++, lp2++) if(*lp1 == EOLTTR) return(0);
	return(*lp1 > *lp2 ? 1 : -1);
}

#ifdef nodef
int
ltrncmp(lp1, lp2, n)
letter	*lp1, *lp2;
int	n;	
{
	for(; n && *lp1 == *lp2; n--, lp1++, lp2++)
		if(*lp1 == EOLTTR) return(0);
	return(n == 0 ? 0 : (*lp1 > *lp2 ? 1 : -1));
}

/* 88/06/14 V3.1 */
/******************************************************************************/
#ifndef OMRON
letter	*ltr_index(lp, l)
letter	l, *lp;
{
	for(; *lp != EOLTTR; lp++) if(*lp == l) return(lp);
	return(NULL);
}
#endif
/******************************************************************************/
#endif

static /* V3.1 */
letter	*ltr_rindex(lp, l)
fast	letter	l, *lp;	/* V3.1 */
{
	fast	letter	*lp2;	/* V3.1 */

	for(lp += ltrlen(lp2 = lp); lp2 != lp; ) if(*--lp == l) return(lp);
	return(NULL);
}

 /** ʸ�����������ʸ���򥫥åȤ��롣*/
/*static V3.1 */
letter	*ltr1cut(lp)
fast	letter	*lp;	/* V3.1 */
{
	fast	int	l;	/* V3.1 */

	if(0 == (l = ltrlen(lp))) BUGreport(0); /* Illegal POP */
	*(lp + --l) = EOLTTR;
	return(lp);
}

 /** letter�ΰ�ʸ����Ĺ������ʸ������Ѵ����� */
static /* V3.1 */
letter	*ltr_to_ltrseq(lp, l)
letter	*lp, l;
{
	*lp++ = l;
	*lp-- = EOLTTR;
	return(lp);
}

 /** char���󤫤�letter���ļ��Ф���â��'\0'��EOLTTR���Ѥ��� */
#ifdef OMRON_LIB
static
#endif
letter	letterpick(lbfpptr)
uns_chr	**lbfpptr;
{
	letter	l = 0;
	fast	int	i;	/* V3.1 */

	for(i = (*(cur_rk_table->bytcnt_method))(*lbfpptr); i; i--)
		l = (l << 8) + *(*lbfpptr)++;
	return(l == 0 ? EOLTTR : l);
}

 /**	���Ϥ��줿�����ɤ�ʸ��ñ�̤ˤޤȤ�롣â��EOF��LTREOF��rk_spclval.h
	�ˤ�����ˤ��Ѥ��롣*/
letter	romkan_next()
{
	letter	in;
	int	i, n;
	uns_chr	c;

	  /* unnext����Ƥ�����ϡ��������Ф���*/
	if(cur_rk->unnext_buf != EOLTTR)
		return(in = cur_rk->unnext_buf, cur_rk->unnext_buf = EOLTTR, in);

	if( (letter)EOF == (in = (*(cur_rk->keyin_method))() )) return(LTREOF);
	c = (in &= 0xff);
	n = (*(cur_rk->kbytcnt_method))(&c);
	for(i = 1; i < n; i++) in = (in << 8) + ((*(cur_rk->keyin_method))() & 0xff);
	return(in);
}

 /*	letter��ʸ�� l ��char��ʸ�� c ����ӡ�����ɥޡ���Ʊ�Ρ�letter���
	����ɥޡ�����EOLTTR��char��Τ�'\0'�ˤ���פȤߤʤ���*/
#define	ltrchreq(l, c) ((l) == (c) && (c) != 0 || (l) == EOLTTR && (c) == 0)

 /** letter��ʸ�����char��ʸ�������� â���羮��Ӥ����ʤ��Ȥ��Ƥ�� */
#ifdef OMRON_LIB
static
#endif
int
ltrstrcmp(l, s)
fast	 letter	*l;	/* V3.1 */
fast	 char	*s;	/* V3.1 */
{
	for(; ltrchreq(*l, *s); l++, s++) if(*s == 0) return(0);
	return((*l < (uns_chr)*s || *l == EOLTTR)? -1 : 1);
}

 /** �Ǹ�˥ޥå�����ʸ�� �Ĥޤ�urabuf�κǸ塣urabuf��������EOLTTR���֤� */
static /* V3.1 */
letter	lastmch()
{
	return(*(ltrend(cur_rk->urabuf)));
}

/* end of 'letter' functions */


 /** �Ѵ��Хåե��Υ��ꥢ */
 /* ��ա� romkan_clear()��Ƥֻ��ϡ�ɬ���������ˡ�cur_rk�򥻥åȤ��뤳�� */
void	romkan_clear()
{
#ifdef KDSP
#	ifdef MVUX
	*displine = EOLTTR;
#	endif
#endif
	cur_rk->ungetc_buf = *(cur_rk->keybuf) = *(cur_rk->urabuf) = *(cur_rk->disout) = *(cur_rk->rk_output) = EOLTTR;
	cur_rk->curdis = DISOUT;
	cur_rk->lastoutlen = cur_rk->lastkbflen = 0;
}

 /**	WnnV4.0�ޥ�����饤������Ѥ�Romkan��¤�Τ��������� **/
Romkan *
rk_buf_init()
{
	register Romkan	*p;
	register int i;

	if ((p = (Romkan *)malloc(sizeof(Romkan))) == NULL) {
		return(NULL);
	}
	malloc_for_henmatch(&(p->rk_henmatch),
			    cur_rk_table->rk_hensuudef.count);
	malloc_for_modesw(&(p->rk_modesw),
			  cur_rk_table->rk_modesw.max);
	p->rk_modesw.max = cur_rk_table->rk_modesw.max;
	p->rk_modesw.count = cur_rk_table->rk_modesw.count;
	for (i = 0; i < cur_rk_table->rk_modesw.max; i++) {
		p->rk_modesw.point[i] = cur_rk_table->rk_modesw.point[i];
	}
	malloc_for_usehyo(&(p->rk_usehyo), cur_rk_table->rk_usehyo.size);
	p->rk_usehyo.size = cur_rk_table->rk_usehyo.size;
	for (i = 0; i < (cur_rk_table->rk_usehyo.size + 1); i++) {
		p->rk_usehyo.usemaehyo[i] = cur_rk_table->rk_usehyo.usemaehyo[i];
		p->rk_usehyo.usehyo[i] = cur_rk_table->rk_usehyo.usehyo[i];
		p->rk_usehyo.useatohyo[i] = cur_rk_table->rk_usehyo.useatohyo[i];
	}
	p->rk_errstat = NULL;
	p->flags = 0;
	p->nil[0] = EOLTTR;
	p->ungetc_buf = EOLTTR;
	p->unnext_buf = EOLTTR;
	p->delchr2 = EOLTTR;
	p->ebf_sw = NULL;
	p->oneletter[0] = p->oneletter[1] = EOLTTR;
	p->keybuf[0] = p->urabuf[0] = p->disout[0] = p->rk_output[0] = EOLTTR;
	p->rk_table = (RomkanTable *)NULL;
	return(p);
}

RomkanTable *
rk_table_buf_init()
{
	RomkanTable	*p;
	if ((p = (RomkanTable *)malloc(sizeof(RomkanTable))) == NULL) {
		return(NULL);
	}
	p->flags = 0;
	p->prv_modfnm[0] = NULL;
	return(p);
}


void
rk_close(p)
Romkan	*p;
{
	/*free_for_all_area();*/	/* V3.1 */
	free_for_modesw(&(p->rk_modesw));
	free_for_henmatch(&(p->rk_henmatch));
	free_for_usehyo(&(p->rk_usehyo));
	free(p);
}

 /**	ɽ���ɤ߹�����Ѵ��ν������򤹤롣��part 3��*/
 /* WnnV4.0�ޥ�����饤����Ȥξ��
	romkan_init3�ϡ�Romkan��¤�ΤؤΥݥ��󥿤��֤��ޤ���
	���θ塢romkan�򤹤���ϡ������Х��ѿ�cur_rk��
	���Υݥ��󥿤�����ʤ���Фʤ�ʤ��� 
	â����romkan_init3()�ϡ������ǰ��Ū��cur_rk�򥻥åȤ���
	return������˸����ͤ��ᤷ�ޤ��� */

/* 88/06/13 V3.1 */
 /** �Ѵ�ɽ�Υ��ꥢ */
static /* V3.1 */
void
romkan_reset()
{
	if (cur_rk_table->rk_modenaibu.org != NULL) {
		cur_rk_table->rk_modenaibu.org[0] = 0;	/* Terminator */
	}
	choosehyo();
	romkan_clear();
}


RomkanTable *
romkan_table_init(p, modhyo, bytcntfn, flags_)
RomkanTable *p;
char	*modhyo;	/* �⡼�����ɽ��̾���Ϥ��Υѥ�̾ */
int	(*bytcntfn)();	/* �Х��ȥ�����ȴؿ� */
int	flags_;
{
	int	head_bytecount(); /* �ǥե���ȤΥХ��ȥ�����ȴؿ� */
	int	errcod; /* ���ν�1�Τ� */
	RomkanTable	*tmp_cur_rk_table;
	RomkanTable	*rk_table_buf;

	tmp_cur_rk_table = cur_rk_table;
	if (p == NULL) {
		if ((rk_table_buf = (RomkanTable *)rk_table_buf_init()) == NULL) {
			return(NULL);
		}
	} else {
		rk_table_buf = p;
	}
	cur_rk_table = rk_table_buf;
	cur_rk_table->bytcnt_method = (bytcntfn == NULL ? head_bytecount : bytcntfn);
	/* 88/05/30/ V3.1 */
	if (modhyo == NULL)
		modhyo = "";
	strcpy(cur_rk_table->prv_modfnm, modhyo);	/* V3.1 */
	if((errcod = setjmp(cur_rk_table->env0)) != 0){
		/* 88/06/13 V3.1 */
		free_for_all_area();	/* V3.1 */
		fprintf(stderr,"romkan_init failed.");
	} else {
		readdata(modhyo);	/* 88/06/08 V3.1 */
	}
	cur_rk_table = tmp_cur_rk_table;
	if (!errcod) {
		return(rk_table_buf);
	} else {
		free(rk_table_buf);
		return(NULL);
	}
}


 /**	ɽ���ɤ߹�����Ѵ��ν������򤹤롣��part 3��*/

Romkan *
romkan_init3
(delchr_, nisedl_, delchr2_, keyinfn, bytcntfn, kbytcntfn, restartf,
 flags_)
letter	delchr_;	/* DEL�Ȥ��ƻȤ������� */
letter	nisedl_;	/* ��DEL�Ȥ��ƻȤ������� */
letter	delchr2_;	/* DEL�Ȥ��ƻȤ������ɤ���Ĥ����礽�Τ⤦���� */
letter	(*keyinfn)();	/* ��������ؿ� */
int	(*bytcntfn)();	/* �Х��ȥ�����ȴؿ� */
int	(*kbytcntfn)();	/* ��������β���ѥХ��ȥ�����ȴؿ���NULL����λ���
			   bytcntfn��Ʊ����Τ��Ȥ��� */
char	restartf;
 /*	romkan_restart()�⤫��ƤФ줿����Ω�ĥե饰�����줬Ω�Ĥȡ��Хåե���
	���ꥢ�����˺Ѥޤ������λ��ϥ��顼���л��Υ�å������⾯���ۤʤ� */
int	flags_;
 /* �ʲ��Υե饰�����Ѳġ�������rk_spclval.h�ˤ������
	RK_CHMOUT:�⡼�ɥ����󥸤��Τ餻�륳���ɤ��֤�����
	RK_KEYACK:����������Ф�ɬ���������֤���
	RK_NONISE:�������ɤ�Ф��ʤ��褦�ˤ��뤫
	RK_REDRAW:Wnn���ü�ե饰��redraw�ѤΥե饰��Ф����ɤ�����
	RK_SIMPLD:delete��ư���ñ��ˤ��뤫
 */
{
	Romkan	*tmp_cur_rk;
	Romkan	*rk_buf;

	tmp_cur_rk = cur_rk;

	if(setjmp(cur_rk_table->env0) != 0){
		return(NULL);
	}

	if ((rk_buf = (Romkan *)rk_buf_init()) == NULL) {
		return(NULL);
	}
	cur_rk = rk_buf;
	 /* �������ɤ�Ф����������romkan_clear�ǻȤ��Τǡ��ե饰�������
	    ��ˤ�äƤ����ʤ��Ȥ����ʤ���*/
	cur_rk->flags = flags_;
	cur_rk->curdis = DISOUT;


	cur_rk->keyin_method = keyinfn;
	cur_rk->bytcnt_method = (bytcntfn == NULL ? head_bytecount : bytcntfn);
	cur_rk->kbytcnt_method = (kbytcntfn == NULL ? cur_rk->bytcnt_method : kbytcntfn);

	cur_rk->delchr = delchr_;
	cur_rk->nisedl = nisedl_;
	cur_rk->delchr2 = delchr2_;
 /**	�¤�DEL�Υ���饯������Ĥޤǻ��Ƥ롣����ܡ�delchr2�ϡ�̤�����
	�Ȥ����ͤ�EOLTTR�ˤ�̵�뤵��롣����ʳ��ϡ�delchr2�����Ϥ�����
	delchr�����Ϥ��줿�Τ�Ʊ�������񤤤򤹤롣*/

	choosehyo();
	cur_rk = tmp_cur_rk;
	return(rk_buf);
}

#define ifflg(a, b) ((a) ? (b) : 0)

 /**	ɽ���ɤ߹�����Ѵ��ν������򤹤롣��part 2: �������Ϥ��Ф�ɬ������
	�֤��褦�ˤ��뤫�������Хåե��򥯥ꥢ���뤫�ɤ����ʤɤ����ġ�*/

Romkan *
romkan_init2
(delchr_, chmoutf, keyinfn, bytcntfn, keyackf, restartf, nonisecodf)
char	chmoutf, keyackf, restartf, nonisecodf;
letter	delchr_;
letter	(*keyinfn)();
int	(*bytcntfn)(); /* �����ξܺ٤�romkan_init3�򻲾� */
{
	return(romkan_init3(delchr_, toNISE(delchr_), EOLTTR,
			    keyinfn, bytcntfn, (int (*)())NULL,
			    restartf,
			    ifflg(chmoutf, RK_CHMOUT) |
			    ifflg(keyackf, RK_KEYACK) |
			    ifflg(nonisecodf, RK_NONISE)));
}

 /**	ɽ���ɤ߹�����Ѵ��ν������򤹤롣��part 1��*/
Romkan *
romkan_init(delchr_, chmoutf, keyinfn, bytcntfn)
char	chmoutf;
letter	delchr_;
letter	(*keyinfn)();
int	(*bytcntfn)(); /* �����ξܺ٤�romkan_init3�򻲾� */
{
	return(romkan_init2(delchr_, chmoutf, keyinfn, bytcntfn,
			    0, 0, 0));
}


 /**	�Х��ȥ�����ȴؿ��Υǥե���ȡ�s�λؤ��Ƥ��������äƤ���Τ�
	���Х��ȥ����ɤ����֤���
	romkan_next����ա�������Ǥϡ��Х��ȥ�����ȴؿ����ͤ�
	�����ΰ�ʸ���ܤˤΤ߰�¸����Ȳ��ꤷ�Ƥ��롣*/
static /* V3.1 */
int
head_bytecount(s)
uns_chr	*s;
{
#ifdef IKIS
	return((*s <= 0xa0 || *s == 0xff)? 1 : 2);
#else
	return((*s <= 0xa0 && *s != HNKAK1 || *s == 0xff)? 1 : 2);
#endif
}

 /** �Ѵ����줿ʸ����缡�֤� */
letter	romkan_getc()
{
	fast	letter	l;	/* V3.1 */

	  /* ungetc����Ƥ�����ϡ��������Ф���*/
	if(cur_rk->ungetc_buf != EOLTTR)
		return(l = cur_rk->ungetc_buf, cur_rk->ungetc_buf = EOLTTR, l);

	while(*(cur_rk->curdis) == EOLTTR){
		 /* romkan_next()���ͤ�LTREOF�λ��⡢���Τޤ�������ɤ���*/
		cur_rk->curdis = romkan_henkan(romkan_next());

		 /* keyackflg����0�ʤ顢�������󤬤����ɬ�������֤�������
		    ���ᡢ�֤��٤���Τ��ʤ�����EOLTTR���֤����Ȥˤ��롣*/
		if(cur_rk->flags & RK_KEYACK) break;
	}

	if(EOLTTR != (l = *(cur_rk->curdis))) cur_rk->curdis++;
	return(l);
	 /* ��ʪ��ʸ���ʤ顢HG1BIT��Ω�äƤ��롣*/
	 /* ���̤ʥ�����(�㤨�е���BEEP�Ȥ���NISEBP)���֤��Ȥ������롣������
	    rk_spclval.h���������Ƥ��롣*/
}

 /**	romkan_getc�β��̴ؿ��Ȥ��ơ����Ϥ��ʸ��������ä�
	�Ѵ���̤�ʸ�������Ϥ��롣*/
letter	*romkan_henkan(mae_in)
letter	mae_in;
{
	letter	mae_out[2], *p;

	/* if(*(cur_rk->curdis) != EOLTTR){p = cur_rk->curdis; cur_rk->curdis = cur_rk->nil; return(p);} */
	/* ������Ǥ�mae_in��̵�뤵��� */
	cur_rk->curdis = cur_rk->nil;

	  /* ����������ؤ����ϤϾ�ˤ�����ʸ�� */
	mae_out[0] = mae_out[1] = EOLTTR;

	cur_rk->eofflg = cur_rk->rk_errstat = 0;
	*(cur_rk->rk_output) = *(cur_rk->disout) = EOLTTR;

	maeato_henkan(mae_in, mae_out, cur_rk->rk_usehyo.usemaehyo);
	cur_rk->rk_input = *mae_out;

	if(cur_rk->rk_input == LTREOF){
		 /* LTREOF���褿��硢(error)��̵�뤷���ܽ����Хåե���������
		    �����Ѵ����롣���Τ����eofflg��1�ˤ��롣���θ塢��̤�
		    �����ˡ�LTREOF���褿�������̥����ɡʻ��ꤵ��Ƥ���Сˤȡ�
		    LTREOF��Ĥʤ���*/
		cur_rk->eofflg = 1;
		match();

		cur_rk->eofflg = 2;
		add_at_eof(); /* codeout�ˡ�LTREOF���褿���Ф������ɤ����롣*/
		ltr1cat(cur_rk->codeout, LTREOF);

		ltrcat(cur_rk->disout, cur_rk->codeout);
		ltrcat(cur_rk->rk_output, cur_rk->codeout);
	} else if(cur_rk->rk_input == EOLTTR){
		 /* EOLTTR���褿���⡢���Ʊ�ͤν�����Ԥ�����LTREOF��
		    �Ĥʤ��ʤ����ʤ��������romkan_getc()��Ƥ�Ǥ������
		    ������ʤ���romkan_next()��EOLTTR���֤��ʤ�����ˡ�*/
		cur_rk->eofflg = 1;
		match();
	} else if(cur_rk->rk_input == cur_rk->delchr
		|| cur_rk->rk_input == cur_rk->delchr2){
	    /* delchr2��̤����ʤ餽���ͤ�EOLTTR�ʤΤ�rk_input���������ʤ���*/
		romkan_delete(cur_rk->rk_input);	/* V3.1 */
	} else {
		ltr1cat(cur_rk->keybuf, cur_rk->rk_input);
		ltr1cat(cur_rk->disout, toNISE(cur_rk->rk_input));
		match();
	}

	if(!(cur_rk->flags & RK_CHMOUT)){
	   /* chmoutflg��0�λ��ϡ�CHMSIG��Ф��ʤ���*/
		for(p = DISOUT; ; p++){
			while(*p == CHMSIG) ltrcpy(p, p + 1);
			if(*p == EOLTTR) break;
		}
	}

	if((cur_rk->flags & RK_REDRAW)
	&& NULL != (p = ltr_rindex(cur_rk->disout, cur_rk->nisedl))){
		for(p++; *p != EOLTTR || (ltr1cat(cur_rk->disout, REDRAW), 0); p++)
			if(!isSPCL(*p)) break;
	}
	 /* wnn��redraw�Υե饰��Ω�äƤ��ơ�disout��nisedl�ʸ��ü쥳���ɤΤ�
	    �ǽ���äƤ����顢REDRAW��Ф��ơ�Wnn���Ѵ��Ԥ�redraw�򤵤��롣*/

	return(DISOUT);
}


/******************************************************************************/
 /* �ǥХå��Ѵؿ� */
#ifdef	KDSP
#	ifdef	MVUX

pridbg2(a, b, c)
char	*a, *c;
letter	*b;
{
	printf("%s", a); dump_fordbg(b); printf("%s", c);
}

ltr_displen(l)
letter	l;
{
	while(l >= 256) l >>= 8;

	if(l == 0) return(0);
	if(l < ' ') return(0);
	if(168 <= l && l <= 170) return(1);
	return(l > 160 ? 2 : 1);
}

dump_fordbg(lp)
letter	*lp;
{
	while(*lp != EOLTTR) printf("%x/", *lp++);
}

print_fordbg(lp)
letter	*lp;
{
	while(*lp != EOLTTR) putletter(*lp++ & ~HG1BIT);
}

print_ltrseq(lp)
letter	*lp;
{
	while(*lp != EOLTTR) print_ltr(*lp++ & ~HG1BIT);
}

print_ltr(l)
letter	l;
{
	letter	*disptail;
	int	i;

	if(!isSPCL(l)) l &= ~HG1BIT;

	if(l == CHMSIG)
#ifdef CHMDSP
		printf("...mode=%s\n",romkan_dispmode())
#endif
		;
	else
#define BEEPCH	'\007'
#define NEWLIN	'\n'
	if(l == BEEPCH || l == NISEBP) putchar(BEEPCH);
	else
	if(l == NEWLIN){
		*(cur_rk->displine) = *(cur_rk->keybuf) = *(cur_rk->urabuf) = EOLTTR;
		cur_rk->lastoutlen = cur_rk->lastkbflen = 0;
		putchar(l);
	}
	else
	if(l == cur_rk->delchr || l == cur_rk->nisedl){
		if(*(cur_rk->displine) == EOLTTR) putchar(BEEPCH);
		 else {
			disptail = ltrend(cur_rk->displine);
			for(i = ltr_displen(*disptail); i; i--)
				printf("\031 \031");
			*disptail = EOLTTR;
		}
	}
	else {
		ltr1cat(cur_rk->displine, l);
		putletter(l);
	}
}

#	endif /* of #ifdef MVUX */
#endif
/* �ǥХå��Ѵؿ������ */


/* 88/06/10 V3.1 */
 /** DEL�����Ϥ��줿�Ȥ��ν����򤹤� */
static /* V3.1 */
void
romkan_delete(input_del)
letter	input_del;
{
	if(ltrlen(cur_rk->keybuf) > ((cur_rk->flags & RK_SIMPLD) ? 0 : cur_rk->lastkbflen)){
		ltr1cut(cur_rk->keybuf);
		set_rubout(cur_rk->disout, 1, cur_rk->nisedl);
	} else {
		if(*(cur_rk->urabuf) != EOLTTR && !(cur_rk->flags & RK_SIMPLD)){
			ltr1cut(ltrcpy(cur_rk->keybuf, cur_rk->urabuf));
			*(cur_rk->urabuf) = EOLTTR;

			set_rubout(cur_rk->rk_output, cur_rk->lastoutlen, input_del);

			set_rubout(cur_rk->disout, cur_rk->lastkbflen, cur_rk->nisedl);
			bitup_ltrcat(ltrcat(cur_rk->disout, cur_rk->rk_output), cur_rk->keybuf);

			cur_rk->lastkbflen = cur_rk->lastoutlen = 0;
		} else {
			set_rubout(cur_rk->disout, 1, input_del);
			set_rubout(cur_rk->rk_output, 1, input_del);
		}
	}
}

 /**	letter���� lp1 �� lp2 �Υޥå����ߤ롣���ͤϡ�lp1��lp2��Ƭ����
	���פλ� -1��lp1�ޤ��Ϥ���Ƭ����lp2�����פΤȤ��ϰ���Ĺ��lp2��
	��ʸ����λ���ޤࡣ���ξ�����ͤ�0�ˡ�����ʳ��� -2��
	lp2¦�˼����ޤޤ�Ƥ�����ɾ���򤹤롣lp1¦�ˤϼ���ޤ�ǤϤ��� */
static /* V3.1 */
int
prefixp(lp1, lp2)
letter	*lp1, *lp2;
{
	/* 1�ԥޥå������뤴�Ȥˡ��ޤ�match����򥯥ꥢ���Ƥ��顣
	   �Ĥޤꡢrk_henmatch.point[0].ltrmch = EOLTTR; �Ȥ��Ƥ�����*/

	fast	 int	mch_len = 0, d_len;	/* V3.1 */

	for(;;){
		if(*lp2 == EOLTTR) return(mch_len);
		if(*lp1 == EOLTTR) return(-1);
		if((d_len = p_eq(&lp2, &lp1)) < 0) return(d_len);
		mch_len += d_len;
	}
}

 /** num���ܤ��ѿ������ˤ���ʸ���ȥޥå����Ƥ���Ȳ��ꤷ�ơ�����ʸ�����֤� */
static /* V3.1 */
letter	mchedsrc(num)
fast	int	num;	/* V3.1 */
{
	fast	matchpair	*pairptr;	/* V3.1 */

	if (cur_rk->rk_henmatch.size <= 0) return(0);
	for(pairptr = cur_rk->rk_henmatch.point; pairptr -> ltrmch != EOLTTR; pairptr++){
		if(num == pairptr -> hennum)
			return(pairptr -> ltrmch);
	}
	return(BUGreport(8), 0);
}

 /**	num���ܤ��ѿ���ʸ�� l �ȥޥå����뤫Ĵ�٤롣�����ѿ���unbound���ä�
	���� l ��bind���롣�ޥå�������(bind�λ���ޤ�)1�����ʤ���0���֤� */
static /* V3.1 */
int
mchsrc(num, l)
fast	int	num;	/* V3.1 */
fast	letter	l;	/* V3.1 */
{
	fast	matchpair	*pairptr;	/* V3.1 */

	if(hen_ikisrc(num, l) == 0) return(0);
	if (cur_rk->rk_henmatch.size <= 0) return(0);
	for(pairptr = cur_rk->rk_henmatch.point; pairptr -> ltrmch != EOLTTR; pairptr++){
		if(num == pairptr -> hennum)
			return(pairptr -> ltrmch == l);
	}

	pairptr -> ltrmch = l;
	pairptr -> hennum = num;
	(++pairptr) -> ltrmch = EOLTTR;
	return(1);
}

 /**	l1p�����ñ�̤��ä�ɾ������ʸ��l2�Ȱ��פ��뤫�ɤ������֤���ɾ������
	��̤���ʸ���ˤʤ�ʤ��ä��顢�������פ��ʤ���*/
static /* V3.1 */
int
l_eq(l1p, l2)
letter	*l1p, l2;
{
	letter	evlrsl[RSLMAX];

	switch(SHUBET(*l1p)){
		case 0:	return(*l1p == l2);
		case 1:	return(mchsrc((int)LWRMSK(*l1p), l2));
		case 2:
			mchevl(&l1p, evlrsl);
			return(evlrsl[0] == l2 && evlrsl[1] == EOLTTR);
		default:
			return(BUGreport(1), 0);
	}
}

 /**	prefixp��ǻ���  â�������ν���ϵա����ʤ�������ޤޤ줦��Τ�l1pp¦
	�Τߡ�l1pp�����ñ�̤֤��ä�ɾ��������Τ�l2pp�Υޥå����ߤ롣����
	��l2pp�ޤ��Ϥ���Ƭ���ȥޥå�����а���Ĺ���֤���l1pp��ɾ����̤���ʸ��
	��λ���ޤࡣ���ξ�����ͤ�0�ˡ��դ�l1pp��ɾ����̤�Ƭ����l2pp���ޥ�
	���������� -1���֤����ޥå������Ԥ��������ͤ� -2��*/
static /* V3.1 */
int
p_eq(l1pp, l2pp)
fast	 letter	**l1pp, **l2pp;	/* V3.1 */
{
	int	num;
	letter	evlrsl[RSLMAX], *rslptr;
	fast	 int	retval = -2;	/* V3.1 */

	 /*  l2pp¦�ˤϼ���ޤޤʤ�Ȧ */
	if(!is_HON(**l2pp)){
	    /*	if(is_HON(**l1pp)) retval = p_eq(l2pp, l1pp); else  */
		BUGreport(9);
	} else {
		switch(SHUBET(**l1pp)){
			case 0:	/* ʸ��Ʊ�� */
				retval = (*(*l1pp)++ == *(*l2pp)++ ? 1 : -2);
				break;
			case 1:	/* �ѿ���ʸ�� */
				num = LWRMSK(*(*l1pp)++);
				retval = (mchsrc(num, *(*l2pp)++) ? 1 : -2);
				break;
			case 2:	/* ����ʸ�� */
				mchevl(l1pp, rslptr = evlrsl);
				for(retval = 0; *rslptr != EOLTTR; retval++){
					if(**l2pp == EOLTTR){
						retval = -1; break;
					} else if(*rslptr++ != *(*l2pp)++){
						retval = -2; break;
					}
				}
				break;
			default:
				BUGreport(2);
		}
	}

	return(retval);
}

 /** l1p�����ñ��ɾ������l2p������� */
#ifdef	MULTI


#define XY2INT(X, Y) (((X) << 24) | (Y)) /* ���줬���󤷤�Y��ɾ�����ʤ����Ȥ�
	��¸���ƽ񤤤Ƥ���ս꤬����  ��ա� */

 /** l1p�����ñ��ɾ������l2p������� */
static
void
mchevl(l1pp, l2p)
letter	**l1pp, *l2p;
{
	letter	*l1p, tmpevl[RSLMAX];

	l1p = *l1pp;

	switch(SHUBET(*l1p)){
		case 0: *l2p++ = *l1p++; break;
		case 1: *l2p++ = mchedsrc((int)LWRMSK(*l1p++)); break;
		case 2:	 /* toupper, tolower, error, �� */
			switch(LWRMSK(*l1p++)){
				case 2: mchevl(&l1p, tmpevl);
					*l2p++ = to_upper(*tmpevl);
					break;
				case 3: mchevl(&l1p, tmpevl);
					*l2p++ = to_lower(*tmpevl);
					break;
				case 4:
					*l2p++ = CHMSIG;
				        *l2p++ = *l1p++;
					*l2p++ = 0; break; /* EOL�ǤϤʤ� */
				case 5:
					*l2p++ = CHMSIG;
				        *l2p++ = *l1p++;
					*l2p++ = 1; break;
				case 6:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 2; break;
				case 7:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_updown(*tmpevl);
					break;
				case 8:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenalpha(*tmpevl);
					break;
				case 9:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_hira(*tmpevl);
					break;
				case 10:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_kata(*tmpevl);
					break;
				case 11:
					mchevl(&l1p, tmpevl);
					to_hankata(*tmpevl, &l2p);
					break; /* �ü� */
				case 12:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenhira(*tmpevl);
					break;
				case 13:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenkata(*tmpevl);
					break;
				case 14:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p += *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 15:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p -= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 16:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p *= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 17:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p /= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 18:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p %= *tmpevl;
					LWRCUT(*l2p++);
					break;
				 /* 19��21��30�ϡ������������ȶ�ʸ����
				    �����Ǥʤ���UNUSDC��ʸ����Ȥ����֤���*/
				case 19:
					mchevl(&l1p, tmpevl);
					if(lastmch() != *tmpevl)
						*l2p++ = UNUSDC;
					break;
				case 20:
					if(! (cur_rk->rk_modesw.point)[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 21:
					if(cur_rk->rk_modesw.point[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 22:
					*l2p++ = REASIG;
					break;
				case 23:
					*l2p++ = cur_rk->delchr;
					break;
				case 24:
					*l2p++ = CHMSIG;
					*l2p++ = 0; /* ����ǡ�all�פ�ɽ�� */
					*l2p++ = 0; break;
				case 25:
					*l2p++ = CHMSIG;
					*l2p++ = 0;
					*l2p++ = 1; break;
				case 26:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p &= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 27:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p |= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 28:
					mchevl(&l1p, tmpevl);
					*l2p = ~(*tmpevl);
					LWRCUT(*l2p++);
					break;
				case 29:
					*l2p++ = URBFCL;
					break;
				case 30:
					if(cur_rk->eofflg != 2 || *(cur_rk->keybuf) != EOLTTR)
						*l2p++ = UNUSDC;
					break;
				case 31:
					{
						letter	code, basenum;

						mchevl(&l1p, tmpevl);
						code = *tmpevl;
						mchevl(&l1p, tmpevl);
						if((basenum = *tmpevl) <= 1 ||
						   BASEMX < basenum)
							basenum = 10;
						to_digit(code, basenum, &l2p);
					}
					break;
				case 32:
					mchevl(&l1p, tmpevl);
					dakuadd(*tmpevl, &l2p);
					break; /* �ü� */
				case 33:
					mchevl(&l1p, tmpevl);
					handakuadd(*tmpevl, &l2p);
					break; /* �ü� */
				case 34:
					mchevl(&l1p, tmpevl);
					*l2p++ = ltov(*tmpevl);
					break;
				case 35:
					*l2p++ = ERRCOD;
					break;
			     /*	case 36: omitted */
				case 37:
					*l2p++ = CHMSIG;
/*
					*l2p++ = LWRMSK(*l1p++);
*/
				        *l2p++ = *l1p++;
					*l2p++ = *l1p++; break;
				case 38:
					*l2p++ = CHMSIG;
					*l2p++ = XY2INT(2, LWRMSK(*l1p++));
					*l2p++ = *l1p++; break;
				case 39:
					*l2p++ = CHMSIG;
					*l2p++ = XY2INT(3, LWRMSK(*l1p++));
					*l2p++ = *l1p++; break;
				case 40:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] !=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 41:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] ==
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 42:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] >=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 43:
					{
						letter	modnum;

						modnum = LWRMSK(*l1p++);
						if(cur_rk->rk_modesw.point[modnum] <=
						   *l1p++)
							*l2p++ = UNUSDC;
						break;
					}
				case 44:
					mchevl(&l1p, tmpevl);
					*l2p++ = SENDCH;
					*l2p++ = *tmpevl;
					break;
				default:/* case 0�ڤӾ嵭�ʳ� */;
					BUGreport(7);
			}
	}

	*l2p = EOLTTR;
	*l1pp = l1p;
}
#else	/*!MULTI*/
static /* V3.1 */
mchevl(l1pp, l2p)
letter	**l1pp, *l2p;
{
	letter	*l1p, tmpevl[RSLMAX];

	l1p = *l1pp;

	switch(SHUBET(*l1p)){
		case 0: *l2p++ = *l1p++; break;
		case 1:	*l2p++ = mchedsrc((int)LWRMSK(*l1p++)); break;
		case 2:  /* toupper, tolower, error, �� */
			switch(LWRMSK(*l1p++)){
				case 1:	mchevl(&l1p, tmpevl);
					*l2p++ = to_upper(*tmpevl);
					break;
				case 2:	mchevl(&l1p, tmpevl);
					*l2p++ = to_lower(*tmpevl);
					break;
				case 3: *l2p++ = ERRCOD;
					break;
				case 4:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 0; break; /* EOL�ǤϤʤ� */
				case 5:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 1; break;
				case 6:
					*l2p++ = CHMSIG;
					*l2p++ = *l1p++;
					*l2p++ = 2; break;
				case 7:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_updown(*tmpevl);
					break;
				case 8:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenalpha(*tmpevl);
					break;
				case 9:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_hira(*tmpevl);
					break;
				case 10:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_kata(*tmpevl);
					break;
				case 11:
					mchevl(&l1p, tmpevl);
					to_hankata(*tmpevl, &l2p);
					break; /* �ü� */
				case 12:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenhira(*tmpevl);
					break;
				case 13:
					mchevl(&l1p, tmpevl);
					*l2p++ = to_zenkata(*tmpevl);
					break;
				case 14:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p += *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 15:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p -= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 16:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p *= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 17:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					if(! *tmpevl) *l2p = LTRHUG;
					 else *l2p /= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 18:
					mchevl(&l1p, tmpevl);
					*l2p++ = ltov(*tmpevl);
					break;
				 /* 19��21��30�ϡ������������ȶ�ʸ����
				    �����Ǥʤ���UNUSDC��ʸ����Ȥ����֤���*/
				case 19:
					mchevl(&l1p, tmpevl);
					if(lastmch() != *tmpevl)
						*l2p++ = UNUSDC;
					break;
				case 20:
					if(!(cur_rk->rk_modesw.point)[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 21:
					if(cur_rk->rk_modesw.point[LWRMSK(*l1p++)])
						*l2p++ = UNUSDC;
					break;
				case 22:
					*l2p++ = REASIG;
					break;
				case 23:
					*l2p++ = cur_rk->delchr;
					break;
				case 24:
					*l2p++ = CHMSIG;
					*l2p++ = 0; /* ����ǡ�all�פ�ɽ�� */
					*l2p++ = 0; break;
				case 25:
					*l2p++ = CHMSIG;
					*l2p++ = 0;
					*l2p++ = 1; break;
				case 26:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p &= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 27:
					mchevl(&l1p, tmpevl);
					*l2p = *tmpevl;
					mchevl(&l1p, tmpevl);
					*l2p |= *tmpevl;
					LWRCUT(*l2p++);
					break;
				case 28:
					mchevl(&l1p, tmpevl);
					*l2p = ~(*tmpevl);
					LWRCUT(*l2p++);
					break;
				case 29:
					*l2p++ = URBFCL;
					break;
				case 30:
					if(cur_rk->eofflg != 2 || *(cur_rk->keybuf) != EOLTTR)
						*l2p++ = UNUSDC;
					break;
				case 31:
					{
						letter	code, basenum;

						mchevl(&l1p, tmpevl);
						code = *tmpevl;
						mchevl(&l1p, tmpevl);
						if((basenum = *tmpevl) <= 1 ||
						   BASEMX < basenum)
							basenum = 10;
						to_digit(code, basenum, &l2p);
					}
					break;
				case 32:
					mchevl(&l1p, tmpevl);
					dakuadd(*tmpevl, &l2p);
					break; /* �ü� */
				case 33:
					mchevl(&l1p, tmpevl);
					handakuadd(*tmpevl, &l2p);
					break; /* �ü� */
				default:/* case 0�ڤӾ嵭�ʹ� */;
					BUGreport(7);
			}
	}

	*l2p = EOLTTR;
	*l1pp = l1p;
}
#endif	/*!MULTI*/

 /** num���ܤ��ѿ����Ѱ褬ʸ�� l ��ޤफ�ɤ������֤� */
static /* V3.1 */
int
hen_ikisrc(num, l)
int	num;
letter	l;
{
	fast	letter	*defptr;	/* V3.1 */

	defptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].hensudef[num];	/* V3.1 */
	if(*defptr == VARRNG){
		for(defptr++; *defptr != EOLTTR; )
			if(*defptr++ <= l && l <= *defptr++) return(1);
		return(0);
	}

	for(; *defptr != EOLTTR; defptr++){
		if(l == *defptr) return(1);
	}
	return(0);
}

 /**	�Ѵ��Υᥤ��롼�����ܽ�����Ԥ������Ĥ��Ǥ˸�������äƤ��롣
	���ʤߤ��������ϡ�romkan_getc�β��̴ؿ�romkan_henkan����ǡ�
	���δؿ���Ƥ����ˤ�äƤ��롣
	���δؿ��ϡ�romkan_next�����ʸ������٤˸ƤФ�롣�ƤӽФ��줿ľ���
	output�϶�ʸ����disout�ˤ����ϥ����ɰ�ʸ�������äƤ��롣
	���δؿ���������ʸ�����󤬡�romkan_henkan���Ϥꡢ
	romkan_getc�ϡ������ʸ�����Ȥ�ʬ�򤷤��֤���
	(error)�ǥ��顼�������������줿����0���֤������ｪλ����1���֤� */
static /* V3.1 */
int
match()
{
	int	henkanflg = 0, okcode = 0, chm_exist;
	letter	*q;	/* V3.1 */
	letter	urabufcreate[KBFSIZ], orgkeybuf[KBFSIZ];
	letter	*urabufjunbi, *outcutptr, *dis_end;
	/* 88/03/01/ V3.1 */
	letter	nokoribuf[KBFSIZ];

	if(*(cur_rk->keybuf) == EOLTTR){
		*(cur_rk->urabuf) = EOLTTR;
		return(1);
	}

	ltrcpy(urabufjunbi = orgkeybuf, cur_rk->keybuf);
	outcutptr = cur_rk->rk_output;

	while((okcode = henkan_ok()) > 0){
		henkanflg = 1;

		codeout_chg();
		ltrcat(cur_rk->rk_output, cur_rk->codeout);
		/* 88/03/01/ V3.1 */
		/*
		p = keybuf + codein_len;

		ltrcat(ltrcpy(keybuf, remainkbf), p);
		*/
		ltrcpy(nokoribuf, cur_rk->keybuf + cur_rk->codein_len);
		ltrcat(ltrcpy(cur_rk->keybuf, cur_rk->remainkbf), nokoribuf);

		if(okcode == 2){
			ltrcpy(urabufjunbi = urabufcreate, cur_rk->keybuf);
			totail(outcutptr);
		}
	}

	if(okcode == 0){
		ltr1cut(ltrcpy(cur_rk->keybuf, orgkeybuf));
		ltr_to_ltrseq(cur_rk->disout, NISEBP);
		*(cur_rk->rk_output) = EOLTTR;
		return(0);
	}
	if(henkanflg){
		ltrcpy(cur_rk->urabuf, urabufjunbi);

		set_rubout(cur_rk->disout, ltrlen(orgkeybuf) - 1, cur_rk->nisedl);

		dis_end = cur_rk->disout;
		totail(dis_end);
		ltrcpy(dis_end, cur_rk->rk_output);

		 /* �⡼�ɥ����󥸤�ľ�����Τ餻�뤿�� CHMSIG�����
		    ��romkan_chmack(1)���äƤʤ��ȡ����Ȥǰ�ʸ�����Ĥ�
		    ʬ�򤹤�����ǡ�CHMSIG�򥫥åȡˡ�
		    â����rk_output����ϡ�CHMSIG��ȴ����
		    �ޤ���CHMSIG�������ˣ��󤷤����Ϥ��ʤ�
		    �ʣ���ʾ夢�äƤ⡢����ˤޤȤ�ơ��������֤��ˡ�*/
		for(chm_exist = 0, q = cur_rk->rk_output; ; q++){
			while(*q == CHMSIG){
				chm_exist = 1;
				if(ltrcpy(q, q + 1) < outcutptr) outcutptr--;
			}
			if(*q == EOLTTR) break;
		}
		if(chm_exist){
		 /* CHMSIG�򣱤ĤˤޤȤ᤿��Τ�dis_end�ˤĤʤ�ľ����
		    ����ifʸ�򥫥åȤ���С�CHMSIG�ΤȤ�ޤȤ�Ϥ��ʤ� */
			ltr1cat(ltrcpy(dis_end, cur_rk->rk_output), CHMSIG);
		}

		bitup_ltrcat(cur_rk->disout, cur_rk->keybuf);
		cur_rk->lastoutlen = ltrlen(outcutptr);
		cur_rk->lastkbflen = ltrlen(cur_rk->keybuf);
	}
	return(1);
}

 /**	LTREOF�����ä��Ȥ��ˡ������Ф��褦�˻��ꤵ��Ƥ��뤫Ĵ�٤ơ�
	codeout�򤽤η�̤�ʸ����ʻ��꤬�ʤ��ä����������ˤ˥ݥ���Ȥ��롣
	Ķ�ޤ����館�ǡ��ü쥳�������ϰ���̵�뤹�롣*/
static /* V3.1 */
void
add_at_eof()
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i;			/* V3.1 */
	fast	int	hyoseq;			/* V3.1 */
	letter	*p;
	letter	evlrsl[RSLMAX];

	/* 88/06/14 V3.1 */
	for (hyoseq = 0;
	    cur_rk_table->rk_hyo.point != NULL && (cur_rk->hyonum = cur_rk->rk_usehyo.usehyo[hyoseq]) != -1;
								hyoseq++) {
		for (i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
				    (p = datptr[i].code[0]) != NULL; i++) {
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			while(*p != EOLTTR){
				switch(SHUBET(*p)){
					case 0:	/* ʸ�� */
					case 1:	/* �ѿ� */
					 /* ����餬������ϡ�NULL�Ȥ�
					    �ޥå������ʤ���*/
						goto Pass;
					case 2:	/* �� */
						mchevl(&p, evlrsl);
						if(*evlrsl != EOLTTR)
							goto Pass;
					 /* ���ϥ��������ˡ�ɾ�������
					    ��ʸ����ˤʤ��Τ���¾�ˤϤʤ�
					    ���Ȥ�����*/
				}
			}

			ltrevlcpy(cur_rk->codeout = p = cur_rk->evalbuf[0][0],
				  datptr[i] . code[1]);
			while(*p != EOLTTR){
				if(isSPCL(*p)) ltrcpy(p, p + 1);
				 else p++;
			}
			codeout_chg();
			return;

Pass:			;
		}
	}
	cur_rk->codeout = cur_rk->nil;
}

/* 88/06/02 V3.1 */
 /**	num���ܤΥ⡼�ɤ�����󥸤����Ѵ�ɽ������ľ�������� mod ���ͤ�0�ʤ�
	�⡼�ɤ�off��1�ʤ�on��2�ʤ��ڤ��ؤ��롣��mod���͡�0��1�ˤ��֤���*/
static
int
chgmod(num, mod)
int	num, mod;
{
	fast	int	oldmod;	/* V3.1 */

	oldmod = cur_rk->rk_modesw.point[num];
	cur_rk->rk_modesw.point[num] = (mod != 2 ? mod : !oldmod);
	choosehyo();
	return(oldmod);
}

 /** ���⡼�ɤ�mod��1�ʤ�on��0�ʤ�off���ڤ괹���롣2�ʤ�switch���롣*/
static
void
allchgmod(mod)
int	mod;
{
	fast	int	i;	/* V3.1 */

	/* 88/06/07 V3.1 */
	for(i = 0; i < cur_rk->rk_modesw.count; i++)
		cur_rk->rk_modesw.point[i] = (mod != 2 ? mod : !(cur_rk->rk_modesw.point[i]));
	choosehyo();
}

 /**	���ޥå����ߤ롣���ͤϡ��ޥå����Ƴ��ꤷ�����1�ʥ⡼�ɥ����󥸤�
	�����äƤ������2�ˡ��ޥå�������̤����λ�-1���ޥå����ʤ��ä���0��
	�¹���ϡ��ѿ� l �ˡ�����ޤǤ˰��פ���Ĺ���κǹ⵭Ͽ������Ƥ��ꡢ
	���Ĺ�����פ����Τ����Ĥ��뤴�Ȥˡ�����򹹿����롣l���ͤϡ��ޥå�
	���Ƥ��Ƥ�0�ˤʤ뤳�Ȥ⤢�롣p_eq() �ڤ� prefixp() �����ʸ�򻲾ȡ�*/
static /* V3.1 */
int
henkan_ok()
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i, k;			/* V3.1 */
	fast	int	l, j, hyoseq;		/* V3.1 */
	char	urabuf_clrf; /* �⡼�ɥ����󥸤ʤɤǡ�urabuf�򥯥ꥢ����ɬ�פ�
				���������Ϥ��줬Ω�������η�̡�henkan_ok()
				��1���֤��٤��Ȥ����2���֤�������򸫤ơ�
				match()��urabuf�ʤɤ�Ĵ���򤹤롣*/
	fast	letter	*p;	/* V3.1 */
#ifndef	MULTI
	static	char	ebf_sw = 0;	/* V3.1 */
	static	letter	oneletter[2] = {EOLTTR, EOLTTR};	/* V3.1 */
#endif	/*!MULTI*/

	if(*(cur_rk->keybuf) == EOLTTR) return(-1);

	/* 88/06/14 V3.1 */
	for (l = -1, hyoseq = 0;
			cur_rk_table->rk_hyo.point != NULL && cur_rk->rk_usehyo.usehyo != NULL &&
			(cur_rk->hyonum = cur_rk->rk_usehyo.usehyo[hyoseq]) != -1; hyoseq++) {
		for (i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
					(p = datptr[i].code[0]) != NULL; i++) {
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			switch(k = prefixp(cur_rk->keybuf, p)){
				case -2: break;
				case -1:
					if(cur_rk->eofflg != 0) break;
				 /* eofflg��Ω�äƤ����顢̤����β�ǽ����
				    �ΤƤ롣*/

					return(-1);
				default:
					if(k > l){
						cur_rk->ebf_sw = !cur_rk->ebf_sw;
						for(j = 1; j <= 2; j++){
							ltrevlcpy(
							 cur_rk->evalbuf[cur_rk->ebf_sw][j-1],
							 datptr[i] . code[j]);
						}

						l = k;
					}
			}
		}
	}

	if(l >= 0){
		cur_rk->codein_len = l;
		cur_rk->codeout = cur_rk->evalbuf[cur_rk->ebf_sw][0];
		cur_rk->remainkbf = cur_rk->evalbuf[cur_rk->ebf_sw][1];

		for(urabuf_clrf = 0, p = cur_rk->codeout; *p != EOLTTR; p++){
			switch(*p){
				case CHMSIG:
					 /* codeout��1,2�Х����ܤ�
					    �⡼���ֹ�ȥ����å������äƤ� */
					if(*(p + 1) == 0)
						allchgmod((int)*(p + 2));
					 else chgmod((int)LWRMSK(*(p + 1)),
						     (int)*(p + 2));
					ltrcpy(p + 1, p + 3);
					 /* CHMSIG�����Ĥ���1,2�Х�����cut */

					urabuf_clrf = 1;
					break;
				case URBFCL:
				 /* urabuf�Υ��ꥢ������Ū�˻��ꤹ�� */
					urabuf_clrf = 1;
					ltrcpy(p, p + 1);
					p--; /* ��ʸ�����ˤ���뤿�ᡢ
						forʸ�� p++ ���껦���ơ�
						Ʊ��������⤦���ٸ��롣*/
					break;
			}
		}

		if(*(cur_rk->codeout) == ERRCOD){
			if(cur_rk->eofflg == 0){
				cur_rk->rk_errstat = 1;
				return(0);
			}

			 /* (error)�Ǥ��äơ�������eofflg��Ω�äƤ��顢keybuf
			    �������ޤǡ����Τޤ޽Ф���*/
			cur_rk->codein_len = ltrlen(cur_rk->keybuf);
			cur_rk->codeout = ltrcpy(cur_rk->evalbuf[cur_rk->ebf_sw][0], cur_rk->keybuf);
			cur_rk->remainkbf = cur_rk->nil;
			cur_rk->rk_errstat = 2;

			return(1);
		}
	   /* (error)��ñ�ȤǤ����񤱤ʤ��Τǡ����顼���ФϤ���ǽ�ʬ��*/

		if(*(cur_rk->codeout) == REASIG){
			*(cur_rk->codeout) = (romkan_restart() != 0 ? EOLTTR : CHMSIG);
			 /* ��read�ǥ���ä���⡼�ɥ����󥸤����ΤϤ��ʤ� */

			urabuf_clrf = 1;
		}
	   /* ɽ�κ�read��â�������줬�����ä����Ȥ򳰤��Τ餻��Τ�CHMSIG��
	      ���Υ����ɤ��Τ�Τϳ��ؽФʤ���(restart)�ϡ�(error)Ʊ�͡�
	      ñ�ȤǤ����񤱤ʤ��Τǡ����ФϤ���ǽ�ʬ��*/

		return(urabuf_clrf ? 2 : 1);
	}

	 /* ɽ�˸���Ƥ��ʤ������ɤϤ��Τޤ��֤� */

	cur_rk->codein_len = 1;
	*(cur_rk->codeout = cur_rk->oneletter) = *(cur_rk->keybuf);
	cur_rk->remainkbf = cur_rk->nil;
	return(1);
}

static
void
free_for_all_area()
{
	fast	int	i;

	free_for_modetable_struct(&(cur_rk_table->rk_defmode));
	free_for_modetable_struct(&(cur_rk_table->rk_taiouhyo));
	free_for_modetable_struct(&(cur_rk_table->rk_path));
	free_for_modebuf(&(cur_rk_table->rk_modebuf));
	free_for_hyobuf(&(cur_rk_table->rk_hyobuf));
	free_for_heniki(&(cur_rk_table->rk_heniki));
	free_for_hensuu(&(cur_rk_table->rk_hensuu));

	free_for_modetable_struct(&(cur_rk_table->rk_dspmode));
	/* free_for_modesw(&(cur_rk_table->rk_modesw)); */
	free_for_modenaibu(&(cur_rk_table->rk_modenaibu));
	for (i = 0; i < cur_rk_table->rk_hyo.size; i++) {
		free_for_hyo_area(&(cur_rk_table->rk_hyo), i);
	}
	free_for_hyo(&(cur_rk_table->rk_hyo));
	/* free_for_usehyo(&(cur_rk_table->rk_usehyo)); */
	free_for_hensuudef(&(cur_rk_table->rk_hensuudef));
	/* free_for_henmatch(&(cur_rk_table->rk_henmatch)); */
}

/* 88/06/10 V3.1 */
 /* romkan_restart��ǻȤ��ޥ��� */
#define taihi(X, Y) {(X) = (Y);}
#define clear(X, N) {memset((char *)(&(X)), 0, (N));}
#define recov(X, Y) taihi((Y), (X))

 /**	ɽ��ưŪ���ɤ߹��ߤ򤹤롣���ߤ�����ɽ�����������򤷡�����Ʊ��
	�ǥ��쥯�ȥ�ʤˡ����ߤΤȤ������ˤ���ɽ���ɤ߹��ࡣ�⤷��
	�ɤ߹�����˥��顼�򸡽Ф���С���Ȥ������ǡ��������褷��0���֤���*/
static /* V3.1 */
int
romkan_restart()
{
	fast	int	i/*, j*/;	/* V3.1 */

	modenaibutable	rk_modenaibu_sv;
	modeswtable	rk_modesw_sv;
	modetable	rk_dspmode_sv;
	hyotable	rk_hyo_sv;
	/* usehyotable	rk_usehyo_sv; */
	hensuudeftable	rk_hensuudef_sv;
	/* matchtable	rk_henmatch_sv; */

	/* char	*dspmod_sv[2][2]; */

	/* ���� */
	taihi(rk_modenaibu_sv,		cur_rk_table->rk_modenaibu);
	taihi(rk_modesw_sv,		cur_rk_table->rk_modesw);
	taihi(rk_dspmode_sv,		cur_rk_table->rk_dspmode);
	taihi(rk_hyo_sv,		cur_rk_table->rk_hyo);
	/* taihi(rk_usehyo_sv,		cur_rk_table->rk_usehyo); */
	taihi(rk_hensuudef_sv,		cur_rk_table->rk_hensuudef);
	/* taihi(rk_henmatch_sv,		cur_rk_table->rk_henmatch);

	for(i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)  dspmod_sv[i][j] = cur_rk_table->dspmod[i][j];
	*/

	/* �õ� */
	clear(cur_rk_table->rk_modenaibu,		sizeof(modenaibutable));
	clear(cur_rk_table->rk_modesw,		sizeof(modeswtable));
	clear(cur_rk_table->rk_dspmode,		sizeof(modetable));
	clear(cur_rk_table->rk_hyo,			sizeof(hyotable));
	/* clear(cur_rk_table->rk_usehyo,		sizeof(usehyotable)); */
	clear(cur_rk_table->rk_hensuudef,		sizeof(hensuudeftable));
	/* clear(cur_rk_table->rk_henmatch,		sizeof(matchtable)); */

	if (NULL != romkan_table_init(cur_rk_table, cur_rk_table->prv_modfnm, cur_rk_table->flags)) {
		/* ���ｪλ */
		/* 88/06/10 V3.1 */
		/* �����ΰ�κ�� */
		free_for_modenaibu(&rk_modenaibu_sv);
		free_for_modesw(&rk_modesw_sv);
		free_for_modetable_struct(&rk_dspmode_sv);
		for (i = 0; i < rk_hyo_sv.size; i++) {
			free_for_hyo_area(&rk_hyo_sv, i);
		}
		free_for_hyo(&rk_hyo_sv);
		/* free_for_usehyo(&rk_usehyo_sv); */
		free_for_hensuudef(&rk_hensuudef_sv);
		/* free_for_henmatch(&rk_henmatch_sv); */

		return(0);
	}

	/* �۾ｪλ */
	/* 88/06/10 V3.1 */
	/* �����ΰ�κ�� */
	free_for_modenaibu(&(cur_rk_table->rk_modenaibu));
	free_for_modesw(&(cur_rk_table->rk_modesw));
	free_for_modetable_struct(&(cur_rk_table->rk_dspmode));
	for (i = 0; i < cur_rk_table->rk_hyo.size; i++) {
		free_for_hyo_area(&(cur_rk_table->rk_hyo), i);
	}
	free_for_hyo(&(cur_rk_table->rk_hyo));
	/* free_for_usehyo(&(cur_rk_table->rk_usehyo)); */
	free_for_hensuudef(&(cur_rk_table->rk_hensuudef));
	/* free_for_henmatch(&(cur_rk_table->rk_henmatch)); */

	/* ���� */
	recov(rk_modenaibu_sv,		cur_rk_table->rk_modenaibu);
	recov(rk_modesw_sv,		cur_rk_table->rk_modesw);
	recov(rk_dspmode_sv,		cur_rk_table->rk_dspmode);
	recov(rk_hyo_sv,		cur_rk_table->rk_hyo);
	/* recov(rk_usehyo_sv,		cur_rk_table->rk_usehyo); */
	recov(rk_hensuudef_sv,		cur_rk_table->rk_hensuudef);
	/* recov(rk_henmatch_sv,		cur_rk_table->rk_henmatch);

	for(i = 0; i < 2; i++)
		for(j = 0; j < 2; j++)  cur_rk_table->dspmod[i][j] = dspmod_sv[i][j];
	*/
	return(1);
}

 /** lp2����ɾ����������ʸ�����lp1�˥��ԡ� */
static /* V3.1 */
void
ltrevlcpy(lp1, lp2)
letter	*lp1, *lp2;
{
	while(*lp2 != EOLTTR){
		mchevl(&lp2, lp1);
		totail(lp1);
	}
	*lp1 = EOLTTR;
}

static /* V3.1 */
void
set_rubout(lp, n, del)
 /** lp�� ��del��n�Ĥ���򥻥åȤ��롣������ del �� 'delchr'��'nisedl' */
fast	letter	*lp, del;	/* V3.1 */
fast	int	n;		/* V3.1 */
{
	for(; n; n--) *lp++ = del;
	*lp = EOLTTR;
}

 /** ���줬�¹Ԥ��줿��Х���â���¹ԤϤ��Τޤ�³�� */
#ifdef OMRON_LIB
static
#endif
void
BUGreport(n)
int	n;
{
	fprintf(stderr, "\r\nromkan-Bug%d!!\r\n", n);
}

 /** ������(mae_in��mae_out)���ϸ����(ato_in��ato_out)��Ԥ���*/
static /* V3.1 */
void
maeato_henkan(in, outp, m_a_hyo)
letter	in;		/* ���Ϥΰ�ʸ�� */
letter	*outp;		/* ���ϤϤ��������� */
int	*m_a_hyo;	/* �ɤ����������ɽ�����򤵤�Ƥ��뤫�ξ��� */
{
	fast	dat	*datptr;	/* V3.1 */
	fast	int	i, hyoseq;		/* V3.1 */
	fast	letter	*curdat;		/* V3.1 */

	if(isSPCL(in)){
	 /* LTREOF��CHMSIG���褦��Τǡ��ü쥳���ɤϤ��Τޤ��֤��褦��
	    �ٹ����Ƥ����ʤ��Ȥ����ʤ���*/
		ltr_to_ltrseq(outp, in);
		return;
	}

	/* 88/06/14 V3.1 */
	for (hyoseq = 0;
	    m_a_hyo != NULL && (cur_rk->hyonum = m_a_hyo[hyoseq]) != -1; hyoseq++) {
		for(i = 0, datptr = cur_rk_table->rk_hyo.point[cur_rk->hyonum].data;
		    NULL != (curdat = datptr[i] . code[0]); i++){
			if (cur_rk->rk_henmatch.size > 0) {
			cur_rk->rk_henmatch.point[0].ltrmch = EOLTTR;
			}
			if(!l_eq(curdat, in)) continue;

			ltrevlcpy(outp, datptr[i] . code[1]);
			return;
		}
	}
	ltr_to_ltrseq(outp, in);
}

 /** ����� */
static /* V3.1 */
void
codeout_chg()
{
	fast	letter	*saishu_outp;	/* V3.1 */
	letter	saishu_out[OUTSIZ];	/* V3.1 */

	*(saishu_outp = saishu_out) = EOLTTR;

	for(; *(cur_rk->codeout) != EOLTTR; cur_rk->codeout++){
		maeato_henkan(*(cur_rk->codeout), saishu_outp, cur_rk->rk_usehyo.useatohyo);
		totail(saishu_outp);
	}

	cur_rk->codeout = saishu_out;
}

 /** ��ʸ���ץå��塦�Хå� */
letter	romkan_ungetc(l)
letter	l;
{
	return(cur_rk->ungetc_buf = l);
}

 /** romkan_next���Ф���ʸ���ץå��塦�Хå� */
letter	romkan_unnext(l)
letter	l;
{
	return(cur_rk->unnext_buf = l);
}

/* 88/06/02 V3.1 */
/******************************************************************************/
#ifndef OMRON
 /**	delete�Ȥ��ƻȤ�����饯��������ʵ�delete��ˡ������¹Ը��
	romkan_clear��¹Ԥ��Ƥ����ʤ��Ⱥ���Τ�Ȥˤʤ�ޤ������ѻ�ͽ���*/
void
romkan_setdel(delchr_, nisedl_)
letter	delchr_, nisedl_;
{
	cur_rk->delchr = delchr_;
	cur_rk->nisedl = nisedl_;
}
#endif
/******************************************************************************/
