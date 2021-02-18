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

/* sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) rkc.h 1.2 90/10/15 20:32:06"; */
#ident	"@(#) NEC/V(386) R3.0B rkc.h 5.9 90/03/26 21:04:36"

#include <memory.h>
#ifndef bzero
#define bzero(buf, size) memset((char *)(buf), 0x00, (size))
#endif
#ifndef bcopy
#define bcopy(src, dst, size) memcpy((char *)(dst), (char *)(src), (size))
#endif

/* ʸ�����쥳����
 *
 */

#define MAX_HOSTNAME	256

typedef struct _RkcBun {
    unsigned char   *kanji  ;	/* ������ޤ����������� */
    short	    curcand ;	/* �����ȴ��������ֹ� */
    short	    maxcand ;	/* ����������� */
    short	    flags   ;	/* �ե饰 */
#define NOTHING_KOUHO	 0x00
#define FIRST_KOUHO	0x01	   /* kanji����Ƭ����Τ� */
#define NUMBER_KOUHO	0x02	   /* kanji�ϸ�������Υݥ��� */
} RkcBun ;			   /* ���ξ�硢curcand��0,maxcand��1 */


/*
 *  ���饤����ȥ���ƥ����ȥ쥳�ݥ�
 *
 */
typedef struct _RkcContext {
    short	    server ;  /* ���ݥС�����ƥ������ֹ� */
    short	    client ;  /* ���饤����ȡ�����ƥ������ֹ� */
    RkcBun	    *bun   ;  /* ʸ�����쥳��������ؤΥݥ��� */
    unsigned char   *Fkouho ; /* ��������ؤΥݥ��� */
    short	    curbun ;  /* ������ʸ���ֹ� */
    short	    maxbun ;  /* ʸ����� */
    short	    bgnflag ; /* RkBgnBun�Υե饰 */
} RkcContext ;

