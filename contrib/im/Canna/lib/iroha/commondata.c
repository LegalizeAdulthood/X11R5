/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/commondata.c,v 3.9 91/08/30 21:39:44 kon Exp $";

#include "iroha.h"
#include <iroha/mfdef.h>
     
/* �ǥե���ȤΥ��޻������Ѵ��ѤΥХåե� */

int defaultContext;
int defaultBushuContext;

/* ���޻������Ѵ����� */
/*
 * ���޻������Ѵ��ơ��֥�ϣ��Ĥ�����ɤ��Ǥ��礦��ʣ����ɬ�פʤΤ�
 * ����� RomeStruct �Υ��ФȤ�������Ƥ���ɬ�פ⤢��ޤ��礦��...��
 * �λ��Ϥ��λ��ǹͤ��ޤ��礦��
 */
     
struct RkRxDic *romajidic, *RkOpenRoma();

/*
 * �����̾��������Ƥ����ѿ�
 */

char *kanjidicname[MAX_DICS];
int  nkanjidics = 0;

char *userdicname[MAX_DICS];
int  nuserdics = 0;
char userdicstatus[MAX_DICS]; /* �ޥ���Ȥ���Ƥ��뤫�ɤ����ξ��� */

char *bushudicname[MAX_DICS];
int  nbushudics = 0;

char *localdicname[MAX_DICS];
int  nlocaldics = 0;

/*
 * ���޻������Ѵ��򤷤Ĥ������ɤ����Υե饰
 */

int forceKana = 0;

/*
 * ��������λ��˲����ܤθ��䤫��ɽ�����뤫�ɤ����Υե饰
 */

int kCount = 0;

/*
 * ����������˥�����Хå���ػߤ��뤫�ɤ����Υե饰
 */

int iListCB = 0;

/*
 * irohacheck ���ޥ�ɤˤ�äƻȤ��Ƥ��뤫�Ȥ���
 * irohacheck �ʤ��Ǥ� verbose ��ɽ���͡�
 */

int ckverbose = 0;

/*
 * ���顼�Υ�å�����������Ƥ����ѿ�
 */

char *necKanjiError;

/*
 * �ǥХ���å�������Ф����ɤ����Υե饰
 */

int iroha_debug = 0;
