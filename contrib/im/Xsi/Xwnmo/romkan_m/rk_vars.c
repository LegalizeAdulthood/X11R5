/*
 * $Id: rk_vars.c,v 1.2 1991/09/16 21:34:53 ohm Exp $
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
			rk_vars.c
						87.12. 3  �� ��

	�ץ���������İʾ�Υե�����ˤޤ����äƻȤ��ѿ���
	�ޤȤ��������Ƥ��롣
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
#ifndef	MULTI
#ifndef OMRON_LIB
#include "rk_header.h"
#endif

char	rk_errstat = 0; /* (error)�ˤ�äƥ��顼�������������줿��1��â��
			   (eofflg)��Ω�äƤ������������ޤǤζ������Ͻ�����
			   �����ä�����2�ˤˤʤ롣romkan_henkan()�¹Ԥ��Ȥ�
			   ��������롣����ϥ桼���ˤⳫ����*/

#ifdef OMRON_LIB
static
#endif
int	flags = 0;
 /* �ʲ��Υե饰��OR��
	RK_CHMOUT �⡼�ɥ����󥸤��Τ餻�뤫�Υե饰 
	RK_KEYACK ����������Ф�ɬ�������֤����Υե饰 
	RK_DSPNIL romkan_disp(off)mode�����ͤΥǥե���Ȥ���ʸ����Ǥ��뤫��
		  �ե饰��Ω���ʤ���Хǥե���Ȥ�NULL���ߴ����Τ���romkan_
		  init2�ޤǤϤ��줬Ω�ġ�
	RK_NONISE �������ɤ�Ф��ʤ��褦�ˤ��뤫�Υե饰
	RK_REDRAW Wnn�ǻȤ��ü�ʥե饰�����줬Ω�äƤ����硢romkan_henkan
		  �η�̤Ȥ���disout��nisedl�ʵ�ʪ��del�˰ʸ夬�ü쥳���ɤΤ�
		  �ǽ����ʸ�����֤äƤ����顢���θ���REDRAW��Ĥʤ���Wnn
		  ���Ѵ��Ԥ�redraw�򤹤뤿���ɬ�פ����֡�
	RK_SIMPLD delete��ư���ñ��ˤ��뤫�Υե饰
	RK_VERBOS verbose�ǵ��������Υե饰
 */

#ifdef OMRON_LIB
static
#endif
jmp_buf	env0;

#ifdef OMRON_LIB
static
#endif
char	*dspmod[2][2] = {NULL, NULL, NULL, NULL};
	 /* ���ߵڤӰ�����Ρ�romkan�򤽤줾��on��off���Ƥ�����Υ⡼��ɽ��
	    ʸ����ؤΥݥ��󥿡�romkan_dispmode()���֤��ͤ�dspmod[0][0]��*/

 
/* 88/06/07 V3.1 */
struct	modetable {
	int	max;
	int	count;
	char	**point;
	int	size;
	char	*org;
	char	*next;
};

#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_defmode;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_dspmode;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_taiouhyo;
#ifdef OMRON_LIB
static
#endif
struct	modetable	rk_path;


 /* �⡼�ɤξ��֡��ơ�1��0���ͤ�Ȥ� */
struct	modeswtable {
	int	max;
	int	count;
	int	*point;
};
#ifdef OMRON_LIB
static
#endif
struct	modeswtable	rk_modesw;

 /* �⡼�����ɽ���ɤ߹��ߤΤ���ΥХåե� */
struct	modebuftable {
	int	size;
	char	*org;
};
#ifdef OMRON_LIB
static
#endif
struct	modebuftable	rk_modebuf;

 /* �⡼�����ɽ�����������Υǡ��� */
struct	modenaibutable {
	int	size;
	int	*org;
	int	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	modenaibutable	rk_modenaibu;

 /* �����ܡ������ɽ�Τɤ줬���򤵤�Ƥ��뤫�Υǡ��� */
struct	usehyotable {
	int	size;
	int	*usemaehyo;
	int	*usehyo;
	int	*useatohyo;
};
#ifdef OMRON_LIB
static
#endif
struct	usehyotable	rk_usehyo;

struct	dat /* ���ϡ����ϡ��Хåե��Ĥ����ؤΥݥ��� */
{
	letter	*code[3];
};

 /* ��Ĥ��б�ɽ�Ρ��Ѵ��ǡ����ڤӳ��ѿ����Ѱ�Ρ���Ǽ���ؤΥݥ��󥿡�*/
struct	hyo {
	int		hyoshu; 	/* ɽ�μ���(�����ܡ������ɽ) */
	struct	dat	*data;		/* �Ѵ��ǡ����γ������� */
	letter		**hensudef;	/* �Ѱ�ɽ�γ������� */
	int		size;		/* ��Ǽ���Υ����� */ /* V3.1 */
	letter		*memory;	/* ��Ǽ���ؤΥݥ��� */ /* V3.1 */
};

struct	hyotable {
	int		size;
	struct	hyo	*point;
};
#ifdef OMRON_LIB
static
#endif
struct	hyotable	rk_hyo;

struct	hyobuftable {
	int	size;
	char	*org;
	char	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	hyobuftable	rk_hyobuf;

struct	henikitable {
	int	size;
	letter	*org;
	letter	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	henikitable	rk_heniki;

struct  hensuset {
	unsigned	regdflg : 1;	/* ����Ͽ���ѿ���ɽ�� */
	unsigned	curlinflg : 1;	/* ���߹Ԥ˴��Ф��ѿ���ɽ�� */
	letter		*name;
	letter		*range;
};

struct	hensuutable {
	int	max;
	int	count;
	struct	hensuset *point;
	int	size;
	letter	*org;
	letter	*next;
};
#ifdef OMRON_LIB
static
#endif
struct	hensuutable  rk_hensuu;

struct	hensuudeftable {
	int	max;
	int	count;
	letter	**point;
};
#ifdef OMRON_LIB
static
#endif
struct	hensuudeftable	rk_hensuudef;

/* ���ߥޥå�����Ƥ����ѿ����ֹ�ȡ��ޥå�����ʸ�����Ȥ��ݻ���*/
struct	matchpair {
	int	hennum;
	letter	ltrmch;
};
/* �ѿ��Υޥå������򤿤�Ƥ��� */
struct	matchtable {
	int	size;
	struct	matchpair *point;
};
#ifdef OMRON_LIB
static
#endif
struct	matchtable	rk_henmatch;
#else	/*!MULTI*/
#include "rk_header.h"
letter  *lptr;
#endif	/*!MULTI*/
