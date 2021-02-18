/*
 * $Id: rk_multi.h,v 1.2 1991/09/16 21:34:49 ohm Exp $
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
			rk_extvars.h
						87.11.12  �� ��

	��İʾ�Υե�����ˤޤ����äƻȤ����ѿ���extern�����
	���Τ�rk_vars.c�������
***********************************************************************/
/*	Version 3.1	88/06/13	H.HASHIMOTO
 */
#ifdef max
#undef	max
#endif

typedef struct	_modetable {
	int	max;
	int	count;
	char	**point;
	int	size;
	char	*org;
	char	*next;
} modetable;

typedef struct	_modeswtable {
	int	max;
	int	count;
	int	*point;
} modeswtable;

typedef struct	_modenaibutable {
	int	size;
	int	*org;
	int	*next;
} modenaibutable ;

typedef struct	_modebuftable {
	int	size;
	char	*org;
} modebuftable;

typedef struct	_dat {
	letter	*code[3];
} dat;

typedef struct	_hyo {
	int		hyoshu;
	dat	*data;
	letter		**hensudef;
	int		size;
	letter		*memory;
} hyo;

typedef struct	_hyotable {
	int		size;
	hyo	*point;
} hyotable;

typedef struct	_usehyotable {
	int	size;
	int	*usemaehyo;
	int	*usehyo;
	int	*useatohyo;
} usehyotable;

typedef struct	_hyobuftable {
	int	size;
	char	*org;
	char	*next;
} hyobuftable;

typedef struct	_henikitable {
	int	size;
	letter	*org;
	letter	*next;
} henikitable;

typedef struct  _hensuset {
	unsigned	regdflg : 1;	/* ����Ͽ���ѿ���ɽ�� */
	unsigned	curlinflg : 1;	/* ���߹Ԥ˴��Ф��ѿ���ɽ�� */
	letter		*name;
	letter		*range;
} hensuset;

typedef struct	_hensuutable {
	int	max;
	int	count;
	hensuset *point;
	int	size;
	letter	*org;
	letter	*next;
} hensuutable;

typedef struct	_hensuudeftable {
	int	max;
	int	count;
	letter	**point;
} hensuudeftable;

typedef struct	_matchpair {
	int	hennum;
	letter	ltrmch;
} matchpair;

typedef struct	_matchtable {
	int	size;
	matchpair *point;
} matchtable;


typedef struct _RomkanTable {
int	flags;
jmp_buf env0;
letter	*ltrbufbgn;	/* �������letter�ΥХåե�����Ƭ��ؤ� */

FILE	**base;

FILE	*modefile;	/*  fp */

/* 88/06/03 V3.1 */
/* ���顼�����Τ���Τ�� */
char	*hcurread;
char	*mcurdir;	/* �⡼��ɽ�Υ������ѥ� */
char	*mcurfnm;	/* �⡼��ɽ�Υե�����̾ */
char	*mcurread;	/* �⡼��ɽ�θ��߹�buf�ؤΥݥ��� */

char	*tcurdir;	/* �б�ɽ�Υ������ѥ� */
char	*tcurfnm;	/* �б�ɽ�Υե�����̾ */
char	*tcurread;	/* �б�ɽ�θ��߹�buf�ؤΥݥ��� */

char	modefilename[REALFN + 5];
char	pathname[REALFN];

modetable	rk_defmode;
modetable	rk_dspmode;
modetable	rk_taiouhyo;
modetable	rk_path;

modeswtable	rk_modesw;

modenaibutable	rk_modenaibu;

modebuftable	rk_modebuf;

hyotable	rk_hyo;

usehyotable	rk_usehyo;

hyobuftable	rk_hyobuf;

henikitable	rk_heniki;

hensuutable	rk_hensuu;

hensuudeftable	rk_hensuudef;

#ifdef KDSP
#	ifdef MVUX
letter	displine[DSPLIN]; /* �ǥХå���  ɽ���ΰ��ʬ�򵭲� */
#	endif
#endif
 
int	(*bytcnt_method)(); /* �Х��ȥ�����ȴؿ��Υ��ɥ쥹 */

/* �⡼��ɽ̾�����Ϥ��Υѥ�̾�ˤ���¸ */ /* ������꤬�����ø�! */
char	prv_modfnm[REALFN];
} RomkanTable;

typedef struct _Romkan {
char	rk_errstat;

int	flags;
jmp_buf	env0;

char	*dspmod[2][2];

/* 88/06/02 V3.1 */
letter	nil[1];
letter	*curdis;
int	codein_len;
letter	*codeout, *remainkbf;
letter	ungetc_buf; /* romkan_ungetc����ʸ�����򤷤Ƥ����Хåե� */
letter	unnext_buf; /* romkan_unnext��		   ��		  */

letter	(*keyin_method)(); /* ��������ؿ��Υ��ɥ쥹 */
int	(*bytcnt_method)(); /* �Х��ȥ�����ȴؿ��Υ��ɥ쥹 */
int	(*kbytcnt_method)(); /* �������ϲ���ѥХ��ȥ�����ȴؿ��Υ��ɥ쥹 */
 
char	eofflg; /* romkan_next()����LTREOF���褿�Ȥ��ˡ�romkan_henkan()���1
		   �ˤ��롣������ͤ���0�ʤ��Ѵ��б�ɽ��(error)��̵�뤷�ơ�
		   �ܽ����Хåե����̤���ꥳ���ɤ⡢����Ū���Ѵ������롣����
		   LTREOF���褿�������̤ˤ�����Ϥ�Ф��褦�˻��ꤵ��Ƥ����
		   ���ν�����Ԥ���â�������ν����ϵޤ����館���Դ����ˡ�����
		   �������eofflg��2��*/

letter	evalbuf[2][2][OUTSIZ];
letter	delchr, delchr2, nisedl; /* DEL�Υ���饯������Ĥޤǻ��Ƥ�*/

/* 88/05/31 V3.1 */
letter	rk_input;  /* ���ϡ�3�Х��ȤޤǤ��ѹ��ʤ����б��� */
letter	disout[OUTSIZ];  /* �ܽ�������ν��ϤΥХåե� */
letter	rk_output[OUTSIZ]; /* ���������ν��ϡʺǽ����ϡˤΥХåե� */
letter	keybuf[KBFSIZ], urabuf[KBFSIZ];  /* �ܽ����Хåե��Ȥ��������ΰ� */
int	lastoutlen, lastkbflen;
int	hyonum;

char 	ebf_sw;
letter	oneletter[2];

modeswtable	rk_modesw;

matchtable	rk_henmatch;

usehyotable	rk_usehyo;

RomkanTable *rk_table;
} Romkan;

extern Romkan	*cur_rk;
extern RomkanTable	*cur_rk_table;
extern Romkan	*romkan_init3();

