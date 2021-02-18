/*
 * $Id: rk_header.h,v 1.2 1991/09/16 21:34:43 ohm Exp $
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
			rk_header.h
						87.11.17  �� ��

	rk_main.c rk_read.c rk_modread.c rk_bltinfn.c�ζ��̥إå���
	��Ǽ�äƤ���������礭���ʤɤ������
***********************************************************************/
/*	Version 3.1	88/06/14	H.HASHIMOTO
 */
/*	make����ɬ�פ�define

	BSD42		BSD�ˤ�strings.h����ѡ�string.h��Ȥ��������ס�
	SYSVR2		System V�ˤ����������¤��줿toupper��tolower�����
			�ʤʤ��Ƥ�ư����
	MVUX		ECLIPSE MV�Ǥα�ž����define  IKIS����ưdefine�����

	RKMODPATH="ʸ����"
			����ʸ�����⡼�����ɽ�Υ������ѥ��δĶ��ѿ���
			̾���ˤ���
	WNNDEFAULT	��@LIBDIR�פ�ɸ������ɽ�Τ���ǥ��쥯�ȥ��ɽ����
			�褦�ˤ���	
	IKIS		Ⱦ�Ѳ�̾�Σ��Х����ܤ�0xA8�ʥǥե���Ȥ�0x8E�ˤˤ���

	����¾ �ǥХå�����ɬ�פ˱����� KDSP��CHMDSP��define
*/

#ifdef MVUX
#	define IKIS
#endif

#include <stdio.h>
#ifdef BSD42
#	include <strings.h>
#	define	strchr	index
#	define	strrchr	rindex
#else
#	include <string.h>
#endif
#include "rk_macros.h"

#define fast register	/* V3.1 */

#define ESCCHR	'\033'
#define	BASEMX	(26 + 10)

#define	LINSIZ	512 /* �б�ɽ�ΰ�Ԥκ��祵���� */ /* V3.1 1000 --> 512 */
#define	TRMSIZ	256 /* �б�ɽ�ΰ���ܤκ��祵������
			�⡼��̾�κ�Ĺ�Ȥ��Ƥ�ȤäƤ� */ /* V3.1 500 --> 256 */
#define	KBFSIZ	100 /* �ܽ����Хåե��Υ����� */ /* V3.1 100 --> 128 */
#ifdef KDSP
#define	DSPLIN	256 /* �ǥХå��� */
#endif
#define	OUTSIZ	200 /* ���ϥХåե��Υ����� */ /* V3.1 200 --> 256 */
#define	RSLMAX	10 /* �ؿ����ͤȤ����֤�ʸ����κ�Ĺ */

	/* rk_modread.c�ǻȤ�define */

#define	REALFN	256 /* ɽ�Υե�͡���κ���Ĺ */	/* V3.1 200 --> 256 */

#define	MDT1LN	256 /* �⡼������ꥹ��1�Ĥκ���Ĺ */	/* V3.1 200 -->256 */

#define	ARGMAX	2 /* ���Ƚ�Ǵؿ��ΰ����κ���Ŀ� */

 /* �ǥ��쥯�ȥ�̾�ζ��ڤ��define��UNIX�ѡˡ�UNIX�ʳ��δĶ��ǻȤ��ˤ�
    ����ȡ�fixednamep()���ѹ���ɬ�פ������readmode()��getenv�ط��������ˡ�*/
#define	KUGIRI '/'

/* 88/06/07 V3.1 */
#define RK_DEFMODE_MAX_DEF		16	/*  */
#define RK_DEFMODE_MAX_LOT		16	/*  */
#define RK_DEFMODE_MEM_DEF		128	/*  */
#define RK_DEFMODE_MEM_LOT		128	/*  */

#define RK_DSPMODE_MAX_DEF		16	/*  */
#define RK_DSPMODE_MAX_LOT		16	/*  */
#define RK_DSPMODE_MEM_DEF		128	/*  */
#define RK_DSPMODE_MEM_LOT		128	/*  */

#define RK_PATH_MAX_DEF			16	/*  */
#define RK_PATH_MAX_LOT			16	/*  */
#define RK_PATH_MEM_DEF			256	/*  */
#define RK_PATH_MEM_LOT			256	/*  */

#define RK_TAIOUHYO_MAX_DEF		16	/*  */
#define RK_TAIOUHYO_MAX_LOT		16	/*  */
#define RK_TAIOUHYO_MEM_DEF		128	/*  */
#define RK_TAIOUHYO_MEM_LOT		128	/*  */

#define RK_HENSUU_MAX_DEF		32	/*  */
#define RK_HENSUU_MAX_LOT		32	/*  */
#define RK_HENSUU_MEM_DEF		256	/*  */
#define RK_HENSUU_MEM_LOT		256	/*  */
#define RK_HENSUUDEF_MAX_DEF		64	/*  */
#define RK_HENSUUDEF_MAX_LOT		64	/*  */

#define RK_HYO_SIZE_DEF			4096	/*  */

 /* ���顼������ */
#ifndef		_WNN_SETJMP
#	define	_WNN_SETJMP
#include <setjmp.h>
#endif		/* _WNN_SETJMP */
