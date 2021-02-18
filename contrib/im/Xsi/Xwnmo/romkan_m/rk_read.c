/*
 * $Id: rk_read.c,v 1.2 1991/09/16 21:34:50 ohm Exp $
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
			rk_read.c
						87.12. 9  �� ��

	rk_main.c �����ʡ��Ѵ��б�ɽ���ɤ߹��ߤ�ô����
***********************************************************************/
/*	Version 3.1	88/06/15	H.HASHIMOTO
 */
#ifndef OMRON_LIB
#include "rk_header.h"
#include "rk_extvars.h"
#ifdef	MULTI
#include "rk_multi.h"
#endif	/*MULTI*/
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "rext.h"


/* 88/06/13 V3.1 */
#ifndef	MULTI
static	letter	*ltrbufbgn;	/* �������letter�ΥХåե�����Ƭ��ؤ� */
#endif	/*!MULTI*/
static	letter	*ltrbufptr;
static	letter	*term;
static	letter	*dummy;


static	dat	*datptr;	/* V3.1 */
static	letter	*memptr;		/* V3.1 */

static	letter	*hen_iki;		/* V3.1 */

/* ���顼��å������Τ���Τ�� */
#ifndef	MULTI
static	char	*tcurdir;	/* �б�ɽ�Υ������ѥ� */
static	char	*tcurfnm;	/* �б�ɽ�Υե�����̾ */
static	char	*tcurread;	/* �б�ɽ�θ��߹�buf�ؤΥݥ��� */
#endif	/*!MULTI*/

/* 88/06/02 V3.1 */
#define bit3(x, y, z) ((x) | ((y) << 1) | ((z) << 2))

/* �Ȥ߹��ߴؿ����������ݻ�����struct */
struct	funstr {
	char	*fnname;	/* �ؿ�̾ */
	char	appear;		/* ���ϥ������������ϥ����������Хåե��Ĥ���
				   �˽񤱤뤫�ɤ����Υե饰���ӥå� */
	char	argnum;		/* �����θĿ� */
	char	fntype;		/* ��̤Υ����� */
};

/* �ؿ����ɲäˤϤ����� mchevl(), evlis() �򤤤��� */
static	struct	funstr	func[] = {
/*			�����ο���-1�Ĥ�����̤���ѡ�
	  ̾		    �и�     ��  �����ס�0:ʸ���ؿ� 1:ʸ����ؿ�
	  ��		     ��	     ��  ��	 2:�ü�ؿ� 3:��ǽ 4:��� */
	"defvar",	bit3(1,0,0), -1, 4, /*  0 */
	"include",	bit3(1,0,0), -1, 4,
	"toupper",	bit3(1,1,1),  1, 0,
	"tolower",	bit3(1,1,1),  1, 0,
/*	"error",	bit3(0,1,0),  0, 3,*/
	"off",		bit3(0,1,0), -1, 2,
	"on",		bit3(0,1,0), -1, 2, /*  5 */
	"switch",	bit3(0,1,0), -1, 2,
	"toupdown",	bit3(1,1,1),  1, 0,
	"tozenalpha",	bit3(1,1,1),  1, 0,
	"tohira",	bit3(1,1,1),  1, 0,
	"tokata",	bit3(1,1,1),  1, 0, /* 10 */
	"tohankata",	bit3(1,1,1),  1, 1,
	"tozenhira",	bit3(1,1,1),  1, 0,
	"tozenkata",	bit3(1,1,1),  1, 0,
	"+",		bit3(1,1,1),  2, 0,
	"-",		bit3(1,1,1),  2, 0, /* 15 */
	"*",		bit3(1,1,1),  2, 0,
	"/",		bit3(1,1,1),  2, 0,
	"%",		bit3(1,1,1),  2, 0,
	"last=",	bit3(1,0,0),  1, 2,  /* ľ���Υޥå�������==������? */
	"if",		bit3(1,0,0), -1, 2, /* 20 */
	"unless",	bit3(1,0,0), -1, 2,
	"restart",	bit3(0,1,0),  0, 3,
	"delchr",	bit3(1,1,1),  0, 0,  /* delchr���֤����������ޥ�� */
	"alloff",	bit3(0,1,0),  0, 2,  /* ���⡼�ɤ򤤤äڤ��OFF */
	"allon",	bit3(0,1,0),  0, 2, /* 25 */
	"bitand",	bit3(1,1,1),  2, 0,
	"bitor",	bit3(1,1,1),  2, 0,
	"bitnot",	bit3(1,1,1),  1, 0,
	"!",		bit3(0,1,0),  0, 2,  /* ΢�Хåե�������Ū���ꥢ */
	"atEOF",	bit3(1,0,0),  0, 2, /* 30 */
	"todigit",	bit3(1,1,1),  2, 1,
	"dakuadd",	bit3(1,1,1),  1, 1, /* �������ղ� */
	"handakuadd",	bit3(1,1,1),  1, 1, /* Ⱦ�������ղ� */
	"value",	bit3(1,1,1),  1, 0,
	"error",	bit3(0,1,0),  0, 3, /* 35 */
	"defconst",	bit3(1,0,0), -1, 4,
	"setmode",	bit3(0,1,0), -1, 2,
	"mode+",	bit3(0,1,0), -1, 2,
	"mode-",	bit3(0,1,0), -1, 2,
	"mode=",	bit3(1,0,0), -1, 2, /* 40 */
	"mode!=",	bit3(1,0,0), -1, 2,
	"mode<",	bit3(1,0,0), -1, 2,
	"mode>",	bit3(1,0,0), -1, 2,
	"send",		bit3(0,1,0),  0, 1,  /* ��� */
	NULL
};
	/* last=��unless�� ���ϥ����ɤΰ��֤ˤ����֤�����ʸ����ؿ������ʤΤ�
			�ܽ���ɽ�ˤ����񤱤ʤ���*/
	/* +,-,*,/,bitand,bitor,bitnot�� 3�Х���ʬ�α黻��Ԥ���*/
	/* atEOF�� �ե����������Ǥ�����ư�����ꤹ�뤿��Τ�Ρ�â��������
		ư����Դ�����*/

static void readhyo(), rangeset(), vchk(), singleqscan(), doubleqscan(),
  listscan(), ERHOPN(), ERRLIN(), ERRHYO();
static int evalandcpy(), eval1cpy(), evlis(), hensrc_tourk(),
  serfun(), termsscan(), atermscan(), partscan(), blankpass(),
  modsrcL();
static letter onescan(), *rangekettei();

extern	letter	*ltrgrow(), letterpick(), *ltr1cut();
extern	char	*chrcat(), *strend();

#define IHENSU (1 << 24) /* ����ɽ���ξ�̣��Х��Ȥǡ��ѿ���ɽ�� */
#define IKANSU (2 << 24) /*	      ��	      �ؿ���ɽ�� */
#define	IMODNM (3 << 24) /*	      ��	      �⡼��̾��ɽ�� */

#define ENDOF_NestFileArea ((FILE *)~(NULL))

int tmp[64];

/* 88/06/01/ V3.1 */
 /** ���ʬ�Υݥ��󥿤�ʤ�롣 */
static char * /* V3.1 */
readln_ustrtoltr(buf, ltrbuf)
char	*buf;
fast	letter	*ltrbuf;
{
	fast	int	l;

	if (*buf == '\0')
		return(NULL);
	while ((l = letterpick(&buf), is_space(l)))
		;
	if ((*ltrbuf++ = l) == EOLTTR)
		return(buf);
	while((*ltrbuf++ = letterpick(&buf)) != EOLTTR);
	return(cur_rk_table->tcurread = buf);
}

/* 88/06/01/ V3.1 */
/** ���б�ɽ���ɤ߹��ߡ��б�ɽ���ɤߤʤ���Կ�������å����롣�б�ɽ�ˤ�
    ����ʸ���ʳ��Υ���ȥ���ʸ�������ǤϺ�����ʤ���ΤȤ��������äƤ�
    ��������å����롣*/
static int	/* V3.1 */
readbuf(fp, s)
fast	FILE	*fp;
fast	char	*s;
{
	fast	int	c, sv = '\0';
	fast	int	cnt = 0; 		/* ���б�ɽ�ιԿ� */

	cur_rk_table->tcurread = s;
	while ((c = getc(fp)) != EOF){
		if (is_cntrl(c) && !isspace(c)) {
			fclose(fp);
			sprintf(cur_rk_table->tcurread, "\\%03o", c);
			ERRLIN(21);
		} 
		if (c == '\n') {
			if (sv == '\0') continue;
			c = '\0';	/* '\n' --> '\0' */
			cnt++;
			cur_rk_table->tcurread = s + 1;
		}
		*s++ = sv = c;
	}
	if (*(s-1) != '\0') {
		cnt++;
		*s++ = '\0'; /* �ƥ����Ƚ�ü�β��Ե����ǡ�Υե�������Ф���ͭ�� */
	}
	*s = '\0';
	return(cnt);
}

 /** ɽ�������ɤ߹��� */
/* 88/06/03/ V3.1 */
#ifdef OMRON_LIB
static
#endif
void
readdata(filename)
char	*filename;	/* �⡼�����ɽ��̾���Ϥ��Υѥ�̾�����äƤ��� */
{
	fast	int	i;	/* V3.1 */
	fast	int	n;	/* V3.1 */

	/* 88/06/09 V3.1 */
	malloc_for_modetable_struct(&(cur_rk_table->rk_path),
				RK_PATH_MAX_DEF, RK_PATH_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_taiouhyo),
				RK_TAIOUHYO_MAX_DEF, RK_TAIOUHYO_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_defmode),
				RK_DEFMODE_MAX_DEF, RK_DEFMODE_MEM_DEF);
	malloc_for_modetable_struct(&(cur_rk_table->rk_dspmode),
				RK_DSPMODE_MAX_DEF, RK_DSPMODE_MEM_DEF);
	malloc_for_modesw(&(cur_rk_table->rk_modesw), RK_DEFMODE_MAX_DEF);

	/* �⡼�����ɽ���ɤ߹��� */
	if ((n = readmode(filename)) != 0) {
		 /** �ΰ�γ��� */
		malloc_for_hyo(&(cur_rk_table->rk_hyo), n);
		malloc_for_usehyo(&(cur_rk_table->rk_usehyo), n);
		malloc_for_hensuudef(&(cur_rk_table->rk_hensuudef), RK_HENSUUDEF_MAX_DEF);

		malloc_for_hensuu(&(cur_rk_table->rk_hensuu),
					RK_HENSUU_MAX_DEF, RK_HENSUU_MEM_DEF);

		malloc_for_hyobuf(&(cur_rk_table->rk_hyobuf), RK_HYO_SIZE_DEF);
		malloc_for_heniki(&(cur_rk_table->rk_heniki), (RK_HYO_SIZE_DEF / 2) + 1);

		/* ���줾����б�ɽ���ɤ߹��� */
		for(i = 0; i < n; i++){
			readhyo(i);
		}
		for(i = 0; i < n; i++){
			cur_rk_table->rk_hyo.point[i].hensudef =
				&cur_rk_table->rk_hensuudef.point[tmp[i]];
		}
	
		 /** �ΰ�β��� */
		free_for_heniki(&(cur_rk_table->rk_heniki));
		free_for_hyobuf(&(cur_rk_table->rk_hyobuf));

		free_for_hensuu(&(cur_rk_table->rk_hensuu));
	}

	/* 88/06/07 V3.1 */
	free_for_modetable_struct(&(cur_rk_table->rk_defmode));
	free_for_modetable_struct(&(cur_rk_table->rk_taiouhyo));
	free_for_modetable_struct(&(cur_rk_table->rk_path));

	/* 88/06/09 V3.1 */
	/*
	if ((n = cur_rk_table->rk_hensuudef.count) != 0)
		malloc_for_henmatch(&(cur_rk_table->rk_henmatch), n);

	choosehyo();
	*/
}

/* 88/06/02 V3.1 */
 /**	���ꤵ�줿�ʥ������ѥ��򸫤�ɬ�פΤʤ��˥ե�����̾���Ф��Ƥ���0��
	�֤������ߤΤȤ��� / ./ ../ �Τɤ줫�ǻϤޤ��ΤȤ��Ƥ��뤬�ʺ�Ԥ�
	���ǡˡ�Ŭ�����Ѥ��Ƥ褤��strchr(s,'/')!=NULL �Ȥ��������ɤ����� */
static
int
fixednamep(s)
char	*s;
{
	return(!strncmp("/",s,1)|| !strncmp("./",s,2)|| !strncmp("../",s,3));
}

 /**  name��Ϳ����줿̾���Ѵ��б�ɽ�򥪡��ץ󤹤롣����̾�ȸ��ʤ���ʤ�
      ̾����fixednamep()���ȡˤ��Ф��Ƥϡ��������ѥ��򸫤롣*/
/* 88/06/09 V3.1 */
static FILE * /* V3.1 */
trytoopen(n, size)
int	n;	/* �б�ɽ���ֹ� */
int	*size;
{
	fast	char	*name;	/* V3.1 */
	/*fast	char	**src;*/	/* V3.1 */
	fast	int	i;	/* V3.1 */
	/*fast	int	cnt;*/	/* V3.1 */

	char	taio_fnm[REALFN];
	struct	stat	statbuf;	/* V3.1 */
	FILE	*fp;	/* V3.1 */


	/* 88/06/10 V3.1 */
	if (fixednamep(cur_rk_table->tcurfnm = name = cur_rk_table->rk_taiouhyo.point[n])) {
		/* 88/06/09 V3.1 */
		cur_rk_table->tcurdir = "";	/* ���顼������ */	/* V3.1 */
		if (stat(name, &statbuf) != 0) ERHOPN(1);
		if ((fp = fopen(name, "r")) == NULL) ERHOPN(2);
		/*
		if (cur_rk->flags & RK_VERBOS) {
			fprintf(stderr, "romkan: using Taio-hyo %s ...\r\n",
				name);
		}
		*/
		*size = statbuf.st_size;
		return(fp);
	} else {
		/* 88/06/09 V3.1 */
		for (i = 0; i < cur_rk_table->rk_path.count; i++){
			strcpy(taio_fnm, (cur_rk_table->tcurdir = cur_rk_table->rk_path.point[i]));
			strcat(taio_fnm, name);

			if (stat(taio_fnm, &statbuf) != 0 || 
			    (fp = fopen(taio_fnm, "r")) == NULL) continue;
			 /* Now Taio-hyo found */
			 /*
			if (cur_rk->flags & RK_VERBOS) {
				fprintf(stderr,
					"romkan: using Taio-hyo %s ...\r\n",
					taio_fnm);
			}
			*/
			*size = statbuf.st_size;
			return(fp);
		}
		/* Taio-hyo not found */
#ifdef	nodef
		if (cur_rk->flags & RK_VERBOS) {
			fast	char	*p, *q;		/* V3.1 */

			fprintf(stderr, "no %s in", name);
			for (i = 0; i < cur_rk_table->rk_path.count; i++) {	/* V3.1 */
				fputc(' ', stderr);
				p = q = cur_rk_table->rk_path.point[i];	/* V3.1 */
				if(*q == KUGIRI && *(q + 1) == '\0') q++;
				 else q = strend(q);
				for (; p < q; p++)		/* V3.1 */
					fputc(*p, stderr);
			}
			fprintf(stderr, ".\n");
		}
#endif
		cur_rk_table->tcurdir = "";	/* V3.1 */
		ERHOPN(1);	/* V3.1 */
	}
	return(NULL);
}

/* 88/06/02 V3.1 */
/******************************************************************************/
#ifndef OMRON
 /**	linbuf�����äƤ���unsigned char�����letter����ˤ���ltrbuf������롣
	����ɥޡ�����EOLTTR�ˤʤ롣flg����0�λ��ϡ���Ƭ�ζ���ʸ�������Ф���*/
static /* V3.1 */
void
ustrtoltr(linbuf, ltrbuf, flg)
uns_chr	*linbuf;
int	flg;
fast	letter	*ltrbuf;	/* V3.1 */
{
	fast	letter	l;	/* V3.1 */

	if(flg){
		while(l = letterpick(&linbuf), is_eolsp(l)){
			if(l == EOLTTR){
				*ltrbuf = EOLTTR;
				return;
			}
		}
		*ltrbuf++ = l;
	}
	while((*ltrbuf++ = letterpick(&linbuf)) != EOLTTR);
}
#endif
/******************************************************************************/

 /** letter��ʸ����˥���С��� */
void
ltr1tostr(l, sptr)
char	**sptr;
letter	l;
{
	int	i;

	for(i = 0; i < 3 && 0 == (l & (0xff000000)); i++) l <<= 8;
	for(; i < 4; i++){
		*(*sptr)++ = (char)(l >> 24);
		l <<= 8;
	}
}

 /** �ѿ��Ρָ��߹Դ��Хե饰�פ����ƥ��ꥢ */
static /* V3.1 */
void
hen_useflgclr(hensu, n)
fast	hensuset *hensu;	/* V3.1 */
fast	int	n;			/* V3.1 */
{
	fast	int	i;	/* V3.1 */
	
	for (i = 0; i < n; i++)
		(hensu++) -> curlinflg = 0; /* ���߹Ԥ�̤�� */
}

 /** �б�ɽ����ɤ߹��� */
static /* V3.1 */
void
readhyo(n)
fast	int	n;	/* V3.1 */
{
	fast	int	i, j;	/* V3.1 */
	fast	letter	*lp;	/* V3.1 */
	fast	int	m;	/* V3.1 */
	fast	hensuset *hensuptr;		/* V3.1 */
	letter	ltrbuf[LINSIZ];				/* V3.1 */
	letter	termbuf[TRMSIZ], dummybuf[TRMSIZ];	/* V3.1 */
	int	hyosw, rsltyp[3];

	FILE	*fp;			/* V3.1 */
	int	size, lines;		/* V3.1 */

	/* 88/06/11 V3.1 */
	/* �б�ɽ�Υ����ץ� */
	fp = trytoopen(n, &size);
	check_and_realloc_for_hyobuf(&(cur_rk_table->rk_hyobuf), size);
	lines = readbuf(fp, cur_rk_table->rk_hyobuf.next = cur_rk_table->rk_hyobuf.org);
	fclose(fp);

	/* �Ѵ��ǡ������ΰ�γ����Ƚ�������ڤӡ��б�ɽ�μ��̤���� */
	malloc_for_hyo_area(&(cur_rk_table->rk_hyo), n, lines, size, &datptr, &memptr);
	hyosw = cur_rk_table->rk_hyo.point[n].hyoshu = filnamchk(cur_rk_table->rk_taiouhyo.point[n]);
	/*
	cur_rk_table->rk_hyo.point[n].hensudef = &cur_rk_table->rk_hensuudef.point[cur_rk_table->rk_hensuudef.count];
	*/
	tmp[n] = cur_rk_table->rk_hensuudef.count;

	check_and_realloc_for_heniki(&(cur_rk_table->rk_heniki), (size / 2) + 1);
	*(hen_iki = cur_rk_table->rk_heniki.next = cur_rk_table->rk_heniki.org) = EOLTTR;

	/* 88/06/10 V3.1 */
	cur_rk_table->rk_hensuu.count = 0;
	*(cur_rk_table->rk_hensuu.next = cur_rk_table->rk_hensuu.org) = EOLTTR;

	cur_rk_table->ltrbufbgn = ltrbuf;
	dummy = dummybuf;

	/* 88/06/11/ V3.1 */
	cur_rk_table->rk_hyobuf.next = cur_rk_table->rk_hyobuf.org;
	while (cur_rk_table->rk_hyobuf.next =
	    readln_ustrtoltr(cur_rk_table->rk_hyobuf.next, (ltrbufptr = ltrbuf))) {
		datptr->code[0] = NULL;				/* V3.1 */
		/*hen_useflgclr(cur_rk_table->rk_hensuu.org, cur_rk_table->rk_hensuu.count);	 V3.1 */
		hen_useflgclr(cur_rk_table->rk_hensuu.point, cur_rk_table->rk_hensuu.count);	 /* V3.1 */

		for(m = 0; termsscan(&ltrbufptr, term = termbuf, 1); m++){
			 /* m�ϡ������ܤι��ܤ򸫤Ƥ��뤫��ɽ�� */
			if(*term == ';') break; /* ���� */
			if(m == 3) ERRLIN(15);
			if(m != 0 && rsltyp[0] == 4) ERRLIN(12);

			datptr -> code[m] = memptr;
			if((rsltyp[m] = evalandcpy(&term, m)) == 4){
				if(m) ERRLIN(14);
			 /* ����Ϻǽ�ι��ܤˤ������ʤ���
			    funstr[]��appear�ե饰�Ǥ�����å����Ƥ��뤬
			    ����Τ���˰�������ˤ�����å�������Ƥ�����*/
			} else {
			 /* ����λ��ˤ�����ɽ���ؤΥݥ��󥿤Ͽʤ�ʤ� */
				totail(memptr);
				memptr++;
			}
		}

		if(m != 0 && rsltyp[0] != 4){
			static	letter	nil[1] = {EOLTTR};	/* V3.1 */
			for( /* m=? */ ; m < 3; m++){
				datptr -> code[m] = nil;
				rsltyp[m] = -1; /* doesn't exist */
			}
			datptr++;

 /* rsltyp: 0=ʸ���� 1=ʸ����Ϣ 2=�ǡ���Ϣ 3=��ǽ�� 4=����� -1=¸�ߤ��� */
			switch(hyosw){
		   /* ����������ϡ�ɽ�����Ƥ����¤����롣����򸡺� */
				case 1:
					if(!(rsltyp[0] == 0 &&
					     rsltyp[1] == 0 && 
					     rsltyp[2] == -1
					    ))
						ERRLIN(17);
					break;
				case 2:
					if(rsltyp[1] == 3 && rsltyp[2] != -1)
						ERRLIN(19);
					break;
				case 3:
					if(!(rsltyp[0] == 0 &&
					     (rsltyp[1] == 0 ||
					      rsltyp[1] == 1) &&
					     rsltyp[2] == -1
					    ))
						ERRLIN(18);
					break;
				default:
					BUGreport(10);
			}

		}
	}

	(datptr++) -> code[0] = NULL;

		/* �ѿ���Ͽ�����ǤޤȤ�Ƥ��� */
	for (lp = cur_rk_table->rk_heniki.org; lp < hen_iki; )
		*memptr++ = *lp++;

	for (i = 0, hensuptr = cur_rk_table->rk_hensuu.point; i < cur_rk_table->rk_hensuu.count;
							i++, hensuptr++) {
		if (hensuptr->regdflg == 0) ERRHYO(1);	/* V3.1 */

		check_and_realloc_for_hensuudef(&(cur_rk_table->rk_hensuudef),
				j = cur_rk_table->rk_hensuudef.count, RK_HENSUUDEF_MAX_LOT);
		cur_rk_table->rk_hensuudef.point[j] = memptr - (lp - hensuptr -> range);
	}
}

 /**	�Ѵ��б�ɽ�ι��ܰ�Ĥ򡢲�ᤷ�ơ����������Υǡ������ꥢ�˥��ԡ����롣
	�֤��ͤϡ���ᤷ�����ܤ�ʸ����ʤ�0������ʳ���ʸ����Ϣ�ʤ�1������ʳ�
	�ǥǡ���Ϣ�ʤ�2����ǽ��ʤ�3�������ʤ�4������ʳ��ϥ��顼��*/
static /* V3.1 */
int
evalandcpy(socp, m)
fast	letter	**socp;	/* ���ܤؤΥݥ��󥿡ʤؤΥݥ��󥿡�*/	/* V3.1 */
int	m;   /* �б�ɽ�β����ܤι��ܤ򸫤Ƥ��뤫��ɽ�������ϥ���������
		���Ƥ���Ȥ���0�����ϥ��������ʤ�1���Хåե��Ĥ����ʤ�2 */
{
#define	TYPMAX	5

	char	exist[TYPMAX];	/* V3.1 */
	fast	char	total;	/* V3.1 */
	 /* exist�ϡ����줾�쥿����0��4�ι�νи��Υե饰��eval1cpy()���ȡˡ�
	    total�����ΤȤ��Ƥνи��ե饰���ɤ���⡢1=̤�� 2=���� ����¾��
	    ���ϲ�2�ӥå�0 */
	fast	int	type;	/* V3.1 */

#define	TIME_0(flag) ((flag) == 1)
#define	TIME_1(flag) ((flag) == 2)
#define	TIME_n(flag) (((flag) & 3) == 0)

	total = 1;
	for(type = 0; type < TYPMAX; type++) exist[type] = 1;

	while(!is_eolsp(**socp)){
		if(!(0 <= (type = eval1cpy(socp, m, 0)) && type < TYPMAX))
			BUGreport(3);
		exist[type] <<= 1;
		total <<= 1;
	}

	if(TIME_0(total))
		BUGreport(13); /* ���ܤ����ʤ�evalandcpy�ϼ¹Ԥ��ʤ�Ȧ */
	if(!TIME_0(exist[3]) || !TIME_0(exist[4])){
		if(TIME_n(total)) ERRLIN(9);
		return(type); /* 3����4��type���ͤ��ݻ����Ƥ���Ȧ */
	}
	if(TIME_1(total) && TIME_1(exist[0])) return(0);
	return(!TIME_0(exist[2]) ? 2 : 1);
}

 /**	�б�ɽ�ι��ܤ���ι��Ĥ��ᤷ�����������Υǡ������ꥢ�˥��ԡ�����
	����Υ����ס�ʸ����=0 ʸ�����=1 �ü�ؿ���=2 ��ǽ��=3 �����=4�ˤ�
	�֤���flg����0�ʤ顢�Ƶ�Ū�˸ƤФ줿���Ȥ��̣�������ξ�硢
	���߹Ԥ�̤�Ф��ѿ��򸡽Ф����饨�顼���ޤ���m����0�ΤȤ������ϥ�������
	�ʳ��ν�򸫤Ƥ�����ˤ⡢���߹Ԥ�̤�Ф��ѿ��򸡽Ф����饨�顼��*/
static /* V3.1 */
int
eval1cpy(socp, m, flg)
letter	**socp; /* flg�ʳ��ΰ����ΰ�̣��evalandcpy��Ʊ�� */
int	m, flg;
{
	letter	t1buf[TRMSIZ], *t1bufp;

	t1bufp = t1buf;

	*memptr = EOLTTR;
	switch(partscan(socp, t1bufp)){
		case 1:	/* ñʸ�� */
			memptr = ltrgrow(memptr, t1bufp);
			*memptr = EOLTTR;
			return(0);
		case 2: /* ����ʸ�� */
			t1bufp++;
			*memptr++ = onescan(&t1bufp, dummy);
			*memptr = EOLTTR;
			return(0);
		case 3: /* ����ʸ���� */
			t1bufp++;
			while(*t1bufp != '"'){
				*memptr++ = onescan(&t1bufp, dummy);
			}
			*memptr = EOLTTR;
			return(1);
		case 0: /* �ꥹ�� */
			return(evlis(m, &t1bufp, flg));
			 /* evlis��� *memptr = EOLTTR; �򤷤Ƥ��롣*/
		default:
			BUGreport(4);
			 /*NOTREACHED*/
	}
	return(-1);
}

#define bitchk(x, n) ((x) & (1 << (n)))

#define get_ltr(lp) (*(lp)++)
#define unget_ltr(l, lp) (*--(lp) = (l))

 /** global�ʥݥ��󥿤���ؤ���Ƥ���letter�󤫤��ʸ����äƤ��롣*/
letter	get1ltr()
{
	return(get_ltr(lptr));
}

letter	unget1ltr(l)
letter	l;
{
	return(unget_ltr(l, lptr));
}

int int_get1ltr() {return((int)(get1ltr()));}
int int_unget1ltr(c) letter c; {return((int)(unget1ltr((letter) c)));}
 /** �������Ȥ��ξ�ʤ����ʤ��ؿ��η���cast���Ǥ��ʤ��Ρ�
    ��(int ()) get1ltr�פȽ񤭤����� */

 /**	include�ե�����̾��letter���string�˼��Ф���Ȥ�letter��ν����
	�ޤ�³���롣flg & 01����0�ʤ顢'/'�Ǥ⽪λ��*/
letter	getfrom_dblq(socp, destp, flg)
letter	**socp;
char	**destp;
int	flg;
{
	letter	l;

	while(**socp != EOLTTR && !(flg & 01 && **socp == KUGIRI)){
		if (**socp == '\\') *(*destp)++ = '\\';
		l = onescan(socp, dummy);
		ltr1tostr(l, destp);
	}
	*(*destp)++ = '\0';
	return(**socp);
}

int
getfrom_lptr(sptr, flg)
char	**sptr;
int	flg;
{
	return((int)getfrom_dblq(&lptr, sptr, flg));
}

 /**	�ꥹ�Ȥ��ᤷ������ɽ���ˤ��롣���ͤϡ����Υꥹ�ȤΥ����פ�ɽ������
	ʸ���ѿ�������ʸ���ؿ���:0 ʸ����ؿ���:1 �ü�ؿ���:2
	��ǽ��:3 �����:4 */
static /* V3.1 */
int
evlis(m, socp, flg)
letter	**socp; /* �����ΰ�̣��eval1cpy�򻲾� */
int	m, flg;
{
	fast	int	fnnum, hennum, i;	/* V3.1 */
	letter	t1buf[TRMSIZ];

	(*socp)++; /* '('�򥹥��å� */
	atermscan(socp, t1buf, 3);

	fnnum = serfun(t1buf);
	if(fnnum != -1 && !bitchk(func[fnnum] . appear, m)) ERRLIN(14);
	 /* m���ͤˤ�äơ�����ƤϤ����ʤ���ؤνи����ɤ������Ƥ��롣*/

	switch(fnnum){ /* default�ν�ʳ��� func[fnnum].argnum��ȤäƤʤ� */
		case -1: /* �ѿ� */
			vchk(t1buf);
			atermscan(socp, dummy, 2); /* �����ERR */
			hennum = hensrc_tourk(t1buf, ((m==0 && !flg)? 0 : 1));
			*memptr++ = hennum | IHENSU;
			break;

		case 0: /* fn No.0 defvar */
			atermscan(socp, t1buf, 3);
			if(*t1buf == '('){
				letter	*soc2, t1buf2[TRMSIZ], t1buf3[TRMSIZ];
				letter	*common_hen;

				atermscan(socp, t1buf3, 3);

				soc2 = t1buf + 1; /* skip '(' */

				atermscan(&soc2, t1buf2, 3);
				vchk(t1buf2);
				if(-1 != serfun(t1buf2)) ERRLIN(11);
				hennum = hensrc_tourk(t1buf2, -1);
				common_hen = rangekettei(hennum, t1buf3);

				while(atermscan(&soc2, t1buf2, 0)){
					vchk(t1buf2);
					if(-1 != serfun(t1buf2)) ERRLIN(11);
					hennum = hensrc_tourk(t1buf2, -1);
					rangeset(hennum, common_hen);
				}
			} else {
				vchk(t1buf);
				if(-1 != serfun(t1buf)) ERRLIN(11);
				hennum = hensrc_tourk(t1buf, -1);
				  /* defvar�ν�ʣ���򤱤뤿�� */
				atermscan(socp, t1buf, 3);
				rangekettei(hennum, t1buf);
			}
			atermscan(socp, dummy, 2);
			break;

		case 36: /* fn No.36 defconst */

			atermscan(socp, t1buf, 3);
			vchk(t1buf);
			if(-1 != serfun(t1buf)) ERRLIN(11);

			hennum = hensrc_tourk(t1buf, 6);
			  /* defvar��defconst���ѿ�̾��ʣ���򤱤뤿�� */

			rangeset(hennum, hen_iki);

			blankpass(socp, 1);
			if(*(*socp)++ != '\'') ERRLIN(8);
			*hen_iki++ = onescan(socp, dummy);
			(*socp)++; /*��'�פ��Ĥ��Ƥ��뤳�Ȥθ����ϺѤ�Ǥ���*/
			*hen_iki++ = EOLTTR; /* needed? */
			*hen_iki = EOLTTR;

			atermscan(socp, dummy, 2);
			break;

		case 1: /* fn No.1 include */
			{
				char	fnmtmparea[REALFN], *s, *dirnamptr;
				int	dummyc = 0, err;

				blankpass(socp, 1);
				if(3 != partscan(socp, t1buf)) ERRLIN(22);
				atermscan(socp, dummy, 2);
				 /* ;ʬ�ˤ����ERR */

				ltr1cut(lptr = t1buf + 1);
				*(s = fnmtmparea) = '\0';
				err = readfnm(int_get1ltr, int_unget1ltr,
					      getfrom_lptr, &s, &dummyc);

				if(err){
					cur_rk_table->hcurread = s;
					switch(err){
						case 1:
						case 3: ERRLIN(25);
						case 2: ERRLIN(26);
						case 4: ERRLIN(27);
					}
				}
				de_bcksla(fnmtmparea, fnmtmparea);

				if(*++(cur_rk_table->base) == ENDOF_NestFileArea){
					(cur_rk_table->base)--;
					ERRLIN(23);
				}
				*(cur_rk_table->base)= trytoopen(fnmtmparea, &dirnamptr,&err);
				if(err){
					switch(err){
						case 1:
						case 3:
						case 2:
						case 4: (cur_rk_table->base)--;
							ERRLIN(24);
					}
				}
			}
			break;

		 /* �⡼��̾��Ĥ�����˼���� */
		case 4: /* fn No.4��6 off,on,switch */
		case 5:
		case 6:
		case 20: /* fn No.20,21 if,unless */
		case 21:
			*memptr++ = fnnum | IKANSU;
			atermscan(socp, t1buf, 3);
			*memptr++ = modsrcL(t1buf) | IMODNM;
			break;

		 /* �⡼��̾��ʸ�� ��Ĥ��Ĥ�����˼���� */
		case 37: /* fn No.37��43 setmode�ʤ� */
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
/*
		case 43:
			{
				int	err, n;
				modetyp	stat;

				*memptr++ = fnnum | IKANSU;
				atermscan(socp, t1buf, 3);
				*memptr++ = (n = modsrcL(t1buf))| IMODNM;
				atermscan(socp, t1buf, 3);
				err = chkL_get_int(t1buf, &stat,
							 modesw[n] . moderng);
				if(err != 0) ERRLIN(29);
				*memptr++ = stat;
				atermscan(socp, t1buf, 2);
				break;
			}
*/

		 /* ���̡ʰ�������ʤ���Τ�ޤ�� */
		default: /* toupper,tolower�� */
			*memptr++ = fnnum | IKANSU;
			*memptr = EOLTTR;
			for(i = 0; i < func[fnnum] . argnum; i++){
				blankpass(socp, 1);
				if(eval1cpy(socp, m, 1) != 0) ERRLIN(13);
			}
			atermscan(socp, dummy, 2); /* ;ʬ�ˤ����ERR */
			break;
	}
	*memptr = EOLTTR;
	return(fnnum == -1 ? 0 : func[fnnum] . fntype);
}

 /** ʸ������Ρ�\�פ�ȴ�� */
void
de_bcksla(s, r)
char	*s, *r;
{
	for(; *s; *r++ = *s++) if(*s == '\\') s++;
	*r = '\0';
}

 /**	defvar�����������shiki������ˤ��ᤷ�ơ������ѿ����Ѱ����ꤹ�롣
	�ѿ���κǸ�˥���ɥޡ�������������롣���ͤϡ��Ѱ���Ǽ����
	��ؤΥݥ��󥿡�*/
static /* V3.1 */
letter	*rangekettei(num, shiki)
letter	*shiki;
int	num; /* �Ѱ����ꤷ�ĤĤ����ѿ��������ֹ� */
{
	letter	hyoukabuf[TRMSIZ];

	rangeset(num, hen_iki);
	*hen_iki = EOLTTR;

	if(*shiki++ != '(') ERRLIN(8);
	atermscan(&shiki, hyoukabuf, 1);

	if(!ltrstrcmp(hyoukabuf, "all")){
		*hen_iki++ = VARRNG;
		*hen_iki++ = 0;
		*hen_iki++ = LTRHUG; /* �Ѱ����ʸ�� */
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
		atermscan(&shiki, dummy, 2); /* ;ʬ�ˤ����ERR */
	} else 
	if(!ltrstrcmp(hyoukabuf, "between")){
		int	i;

		*hen_iki++ = VARRNG;
		while(blankpass(&shiki, 1), *shiki != ')'){
			for(i = 1; i <= 2; i++){
				switch(*shiki){
					case '\'':
						shiki++;
						*hen_iki++ = onescan(&shiki,
								     dummy);
						shiki++;
						break;
					case ')':
					case '"':
					case '(': ERRLIN(8);
					default:
						*hen_iki++ = *shiki++;
				}
				if(i < 2){
					if(!is_eolsp(*shiki)) ERRLIN(8);
					blankpass(&shiki, 1);
				}
			}
		}
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
	} else
	if(!ltrstrcmp(hyoukabuf, "list")){
		while(blankpass(&shiki, 1), *shiki != ')'){
			switch(*shiki){
				case '"':
				case '(': ERRLIN(8);
				case '\'':
					shiki++;
					*hen_iki++ = onescan(&shiki, dummy);
					shiki++; /* �����ˡ�'�פ��Ĥ��Ƥ��뤫
					 �ɤ����θ����Ϥ⤦�Ѥ�Ǥ��롣*/
					break;
				default:
					*hen_iki++ = *shiki++;
			}
			if(!is_eolsp(*shiki)) ERRLIN(8);
		}
		*hen_iki++ = EOLTTR;
		*hen_iki = EOLTTR;
	} else {
		ERRLIN(8); /* ����Ϥ���¾�ι�ʸ�����ͽ��Ǥ��ä� */
	}

	return(cur_rk_table->rk_hensuu.point[num].range);	/* V3.1 */
}

 /**	num���ܤ��ѿ����Ѱ��ؤ��٤��ݥ��󥿤λؤ������ꤷ�������ѿ���
	����Ͽ���֤ˤ��롣*/
static /* V3.1 */
void
rangeset(num, range)
letter	*range;	/* �Ѱ��������Υݥ��� */
int	num;
{
	cur_rk_table->rk_hensuu.point[num].range = range;	/* V3.1 */
	cur_rk_table->rk_hensuu.point[num].regdflg = 1;	/* V3.1 */
}


 /**	name�ǻ��ꤵ�줿̾���ѿ���õ�����ʤ������Ͽ���ѿ�̾�κǸ��
	����ɥޡ�������������롣
	flg��1�λ�������̾���ѿ������߹Ԥ�̤�Фʤ饨�顼���ޤ���flg��-1�λ���
	����̾���ѿ���������ʤ饨�顼��defvar�ν�ʣ�����å��ѡˡ�*/
static /* V3.1 */
int
hensrc_tourk(name, flg)
letter	*name;
int	flg;
{
	fast	int	n;			/* V3.1 */
	fast	hensuset *hensuptr;	/* V3.1 */

	/* 88/06/06 V3.1 */
	for (n = 0; n < cur_rk_table->rk_hensuu.count; n++){
		if(ltrcmp(cur_rk_table->rk_hensuu.point[n].name, name)) continue;
		 /* found */
		if(flg == -1 && cur_rk_table->rk_hensuu.point[n].regdflg != 0) ERRLIN(10);
		if(flg == 1 && cur_rk_table->rk_hensuu.point[n].curlinflg == 0) ERRLIN(5);
		cur_rk_table->rk_hensuu.point[n].curlinflg = 1;
		return(n);
	}

	 /* �������褿�Ȥ������Ȥϡ���Ф��ѿ��ȸ������ȡ�������flg��1�ʤ�
	    ���顼��*/
	if(flg == 1) ERRLIN(5);

	check_and_realloc_for_hensuu(&cur_rk_table->rk_hensuu, n, name,
					RK_HENSUU_MAX_LOT, RK_HENSUU_MEM_LOT);

	hensuptr = &cur_rk_table->rk_hensuu.point[n];
	hensuptr -> name =  cur_rk_table->rk_hensuu.next;
	hensuptr -> curlinflg = 1;
	hensuptr -> regdflg = 0; /* ��Ф�����range��def��̤����Ȧ */
	cur_rk_table->rk_hensuu.next = ltrgrow(hensuptr->name, name);
	*(++(cur_rk_table->rk_hensuu.next)) = EOLTTR;
	return(n);
}

 /** �Ȥ߹��ߴؿ�����ǽ̾���Ф��ƤϤ����ֹ�򡢤����Ǥʤ���Τʤ�-1���֤� */
static /* V3.1 */
int
serfun(lp)
fast	letter	*lp; /* �ؿ�����ǽ̾�⤷�����ѿ�̾ */	/* V3.1 */
{
	fast	int	i;	/* V3.1 */

	for(i = 0; func[i] . fnname != NULL; i++){
		if(! ltrstrcmp(lp, func[i] . fnname)) return(i);
	}
	return(-1);
}

 /** �ѿ���̾���Υ����å� ���������ȥ��顼 */
static /* V3.1 */
void
vchk(lp)
fast	letter	*lp;	/* V3.1 */
{
	if(is_digit(*lp)) ERRLIN(3);
	for(; *lp != EOLTTR; lp++){
	  /*	if(is_lower(*lp)) *lp = to_upper(*lp);  */
		if(!is_alnum(*lp) && *lp != '_') ERRLIN(3);
	}
}

 /**	����ܤ���Ф������Ф������������1���֤���flg����0�λ��ϡ�')'��
	���Ĥ���ȥ��顼��';'�Ϥ�������ǰ���ܰ�����*/
static /* V3.1 */
int
termsscan(socp, dest, flg)
fast	letter	**socp, *dest;	/* V3.1 */
	 /* socp�λؤ��Ƥ���ݥ��󥿤��ؤ��Ƥ���꤫����Ф���dest������롣
	    ���θ塢socp���ؤ��Ƥ���ݥ��󥿤�ʤ�롣���Υե������ **scan()
	    �Ȥ����ؿ������Ƥ����ʤäƤ롣*/
int	flg;
{
	fast	letter	*bgn;	/* V3.1 */

	bgn = dest;

	if(blankpass(socp, 0) == 0){
		if(flg && **socp == ';'){
			*dest++ = *(*socp)++;
		} else
		while(!is_eolsp(**socp)){
			if(**socp == ')'){
				if(flg) ERRLIN(0);
				break;
			} else {
				partscan(socp, dest);
				totail(dest);
			}
		}
	}

	*dest = EOLTTR;
	return(bgn != dest);
}

 /**	�ꥹ�Ȱ�Ĥ���ñ���ΰ�ޤȤޤ����Ф�������������1���֤���
	flg��1�ΤȤ���')'�����Ĥ���ȥ��顼��
	flg��2�ΤȤ������Ф������������饨�顼��
	flg��3�ΤȤ������Ф��˼��Ԥ����饨�顼��*/
static /* V3.1 */
int
atermscan(socp, dest, flg)
fast	letter	**socp, *dest;	/* V3.1 */
int	flg;
{
	fast	letter	*bgn;	/* V3.1 */
	fast	int	found;	/* V3.1 */

	bgn = dest;

	if(blankpass(socp, 0) == 0){
		if(**socp == '('){
			listscan(socp, dest);
			totail(dest);
		} else {
			while(!is_eolsp(**socp) && **socp != '('){
				if(**socp == ')'){
					if(flg == 1) ERRLIN(0);
					break;
				} else {
					partscan(socp, dest);
					totail(dest);
				}
			}
		}
	}

	*dest = EOLTTR;

	found = (bgn != dest);
	if(!found && flg == 3 || found && flg == 2) ERRLIN(7);
	return(found);
}

 /**	���Ĥ���Ф������Ф�����Τ��ꥹ�Ȥʤ����ͤ�0��ñʸ���ʤ�1��
	����ʸ���ʤ�2������ʸ����ʤ�3��*/
static /* V3.1 */
int
partscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	switch(**socp){
		case '(':
			listscan(socp, dest);
			return(0);
		case '\'':
			singleqscan(socp, dest);
			return(2);
		case '"':
			doubleqscan(socp, dest);
			return(3);
		default:
			*dest++ = *(*socp)++;
			*dest = EOLTTR;
			return(1);
	}
}

 /** ���󥰥륯������ɽ����Ĥ���Ф���*/
static /* V3.1 */
void
singleqscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	*dest++ = *(*socp)++;
	onescan(socp, dest);
	totail(dest);
	if((*dest++ = *(*socp)++) != '\'') ERRLIN(1);

	*dest = EOLTTR;
}

 /** ���֥륯������ɽ����Ĥ���Ф���*/
static /* V3.1 */
void
doubleqscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	*dest++ = *(*socp)++;
	while(**socp != '"'){
		if(**socp == EOLTTR) ERRLIN(1);
		onescan(socp, dest);
		totail(dest);
	}
	*dest++ = *(*socp)++;

	*dest = EOLTTR;
}

 /**	8��10��16�ʥ������ѥ���饯����ºݤΥ����ɤ�ľ�������ϤΥ����å���
	��������ʸ���ȿ����ʳ������Ϥ��Ф��Ƥ�ñ��0���֤���*/
#ifdef OMRON_LIB
static
#endif
int
ltov(l)
fast	letter	l;	/* V3.1 */
{
	if(is_upper(l)) return(l - 'A' + 10);
	if(is_lower(l)) return(l - 'a' + 10);
	if(is_digit(l)) return(l - '0'); else return(0);
}

 /** ltov�ε� */
#ifdef OMRON_LIB
static
#endif
letter	vtol(l)
fast	letter	l;	/* V3.1 */
{
	if(BASEMX <= l) return('*');
	return(l + (l < 10 ? '0' : 'A' - 10));
}

 /**	���󥰥롦���֥륯�����Ȥ���Ǥΰ�ʸ�����Ф���
	��^�פˤ�륳��ȥ��륳����ɽ������\�פˤ��8��10��16��ɽ���ˤ�
	�б�����\��ɽ���ϡ���\��o����d����x�Ͽ������¤ӡ�;�ϡפǤ��롣*/
static /* V3.1 */
letter	onescan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	fast	letter	l, realcode;	/* V3.1 */
	fast	int	digflg;		/* V3.1 */

	switch(realcode = *dest++ = *(*socp)++){
		case '^':
			if(!(' ' <= (l = *(*socp)++) && l < '\177'))ERRLIN(2);
			realcode = ((*dest++ = l) == '?' ? '\177' : l & 0x1f);
			break;
		case '\\':
			digflg = 0;
			switch(**socp){
				case 'n':
					*dest++ = *(*socp)++; realcode = '\n';
					break;
				case 't':
					*dest++ = *(*socp)++; realcode = '\t';
					break;
				case 'b':
					*dest++ = *(*socp)++; realcode = '\b';
					break;
				case 'r':
					*dest++ = *(*socp)++; realcode = '\r';
					break;
				case 'f':
					*dest++ = *(*socp)++; realcode = '\f';
					break;
				case 'e':
				case 'E':
					*dest++ = *(*socp)++; realcode=ESCCHR;
					break;
				case 'o':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_octal(**socp);){
						digflg = 1;
						realcode <<= 3;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				case 'x':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_xdigit(**socp);){
						digflg = 1;
						realcode <<= 4;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				case 'd':
					*dest++ = *(*socp)++;
					for(realcode = 0; is_digit(**socp);){
						digflg = 1;
						realcode *= 10;
						realcode += ltov
						       (*dest++ = *(*socp)++);
					}
					if(!digflg) ERRLIN(2);
					if(**socp== ';') *dest++ = *(*socp)++;
					break;
				default:
					if(is_octal(**socp)){
						for(realcode = 0;
						    is_octal(**socp);){
							realcode <<= 3;
							realcode += ltov(
							*dest++ = *(*socp)++);
						}
						if(**socp == ';')
							*dest++ = *(*socp)++;
					} else {
						realcode= *dest++= *(*socp)++;
					}
			}
			break;
		default: ;
	}

	*dest = EOLTTR;
	return(realcode);
}
	
 /**	letter�������Ƭ�ˤ������򥹥��åפ��롣
	������ã�����顢flg��0�ΤȤ���1���֤��������Ǥʤ��ȥ��顼��*/
static /* V3.1 */
int
blankpass(pptr, flg)
fast	letter	**pptr;	/* V3.1 */
	 /* letter����Υݥ��󥿤ؤΥݥ��󥿡����줬�ؤ��Ƥ����Τ�ʤ�� */
int	flg;
{
	while(is_eolsp(**pptr)){
		if(EOLTTR == **pptr){
			if(flg) ERRLIN(4);
			return(1);
		}
		(*pptr)++;
	}
	return(0);
}

 /** �ꥹ�Ȱ�ļ��Ф� */
static /* V3.1 */
void
listscan(socp, dest)
fast	letter	**socp, *dest;	/* V3.1 */
{
	fast	int	eofreach;	/* V3.1 */

	*dest++ = *(*socp)++; /* = '(' */
	*dest++ = ' ';
	
	while(eofreach = blankpass(socp, 0), **socp != ')'){
		if(eofreach){
			/* 88/06/02/ V3.1 */
			if((cur_rk_table->rk_hyobuf.next = readln_ustrtoltr(cur_rk_table->rk_hyobuf.next,
					(*socp = cur_rk_table->ltrbufbgn))) == NULL)
				ERRLIN(20);
			 /* list����ǹԤ��ڤ�Ƥ����硢����ɲ��ɤ߹��ߤ�
			    ���롣uns_chr�ѤΥХåե��⡢letter�ѤΤ�Τ⡢
			    ������ʪ����Ƭ��������Ѥ��Ƥ���Τ���ա��ޤ���
			    ���顼����������硢 ���顼�ΰ��֤ˤ�����餺��
			    ���߹ԤȤ���ɽ�������Τϡ��Ǹ���ɤޤ줿�ԤΤ�*/
		} else {
			termsscan(socp, dest, 0);
			totail(dest);
			*dest++ = ' ';
		}
	}
	*dest++ = *(*socp)++; /* = ')' */
	*dest = EOLTTR;

}

 /** lp�ǻ��ꤵ�줿�⡼��̾��õ�������Ĥ���ʤ��ȥ��顼 */
static /* V3.1 */
int
modsrcL(lp)
letter	*lp;
{
	fast	int	n;	/* V3.1 */

	/* 88/06/07 V3.1 */
	for(n = 0; n < cur_rk_table->rk_defmode.count; n++)
		if(!ltrstrcmp(lp, cur_rk_table->rk_defmode.point[n])) return(n);

	ERRLIN(16);
	 /*NOTREACHED*/
	return(0);
}

 /**	������ϡ�ɽ�ɤ߹��ߤǥ���ä����˷ٹ𤹤�롼����n�ϥ��顼�����ɡ�
	ermes�Ȥ��������Ѱդ���Ƥʤ���Τϡ����νꥨ�顼��å�������
	��Ĥ����ʤ���Ρ����ξ�票�顼�����ɤ⺣�ν�0�Τߡ�
	�ޤ����������ơ�romkan_init�Υ��顼�����ɤȤ��ơ�longjmp��ͳ��
	1���֤���*/


static /* V3.1 */
void
ERHOPN(n) /* �Ѵ��б�ɽ�������ץ�Ǥ��ʤ� */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Unprintable error",
		"Taio-hyo doesn't exist",
		"Can't open Taio-hyo",
	};

	if(n >= numberof(ermes)) n = 0;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n", cur_rk_table->tcurdir, cur_rk_table->tcurfnm);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}


static	/* V3.1 */
void
ERRLIN(n) /* �Ѵ��б�ɽ�Υ��顼 */
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"')' mismatch",
		"Incomplete single-quote or double-quote expression",
		"Illegal ^,\\o,\\x or \\d expression",
		"Illegal variable name",
		"Incomplete line",
 /*  5 */	"Evaluation of unbound variable",
		"Unprintable error",
		"Too many or too few contents of list",
		"Illegal defvar",
		"Faculity or declaration joined other item(s)",
 /* 10 */	"Duplicate defvar",
		"Variable name conflicts with Function name",
		"A line contains both declaration and other output item(s)",
		"Argument isn't a letter",
		"Function, faculity or declaration in illegal place",
 /* 15 */	"More than 3 items",
		"Undefined mode",
		"Against the restriction of pre-transform table",
		"Against the restriction of after-transform table",
		"Item comes after faculity",
 /* 20 */	"Incomplete list",
 		"Illegal character",
	};

	if(n >= numberof(ermes)) n = 6;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n%s\r\n",
		cur_rk_table->tcurdir, cur_rk_table->tcurfnm, cur_rk_table->tcurread);
	fprintf(stderr, "\tError No.%d: %s.\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}

/* 88/06/10 V3.1 */
static /* V3.1 */
void
ERRHYO(n) /* ERRLIN��Ʊ�͡��б�ɽ�Υ��顼��������Ԥ����θ��Ǥʤ�
             ���Τ򸫤ʤ��Ȥ狼��ʤ���ꡣ��̤����ѿ��νи���*/
fast	unsigned int	n;	/* V3.1 */
{
	static	char	*ermes[] = {
 /*  0 */	"Unprintable error",
		"Undefined variable was found",
	};

	if(n >= numberof(ermes)) n = 0;

	fprintf(stderr, "\r\nromkan: Taio-hyo %s%s ---\r\n", cur_rk_table->tcurdir, cur_rk_table->tcurfnm);
	fprintf(stderr, "Error No.%d: %s\r\n", n, ermes[n]);
	longjmp(cur_rk_table->env0, 1);
}
