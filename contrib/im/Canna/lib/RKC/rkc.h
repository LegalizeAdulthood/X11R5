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

/* 文節情報レコード
 *
 */

#define MAX_HOSTNAME	256

typedef struct _RkcBun {
    unsigned char   *kanji  ;	/* 第一候補または全候補列 */
    short	    curcand ;	/* カレント漢字候補番号 */
    short	    maxcand ;	/* 漢字候補総数 */
    short	    flags   ;	/* フラグ */
#define NOTHING_KOUHO	 0x00
#define FIRST_KOUHO	0x01	   /* kanjiは先頭候補のみ */
#define NUMBER_KOUHO	0x02	   /* kanjiは候補一覧のポインタ */
} RkcBun ;			   /* この場合、curcandは0,maxcandは1 */


/*
 *  クライアントコンテクストレコ−ド
 *
 */
typedef struct _RkcContext {
    short	    server ;  /* サ−バ・コンテクスト番号 */
    short	    client ;  /* クライアント・コンテクスト番号 */
    RkcBun	    *bun   ;  /* 文節情報レコード配列へのポインタ */
    unsigned char   *Fkouho ; /* 第一候補列へのポインタ */
    short	    curbun ;  /* カレント文節番号 */
    short	    maxbun ;  /* 文節総数 */
    short	    bgnflag ; /* RkBgnBunのフラグ */
} RkcContext ;

