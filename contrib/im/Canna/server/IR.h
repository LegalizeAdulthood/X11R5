/* sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) IR.h 1.8 91/03/07 17:05:27"; */

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

#include    <stdio.h>

#include    "RK.h"
#include    "RKintern.h"
#include    "IRproto.h"

#ifdef MAX_CONTEXT
#define MAX_CX	MAX_CONTEXT
#else
#define MAX_CONTEXT  100
#endif
#define OLD_MAX_CX  100

#define FALSE 0
#define TRUE 1

#ifdef DEBUG
#define ir_debug( irohadebug )	 irohadebug
#else
#define ir_debug( irohadebug )	
#endif

#define IR_SERVICE_NAME	    "iroha"
#define IR_DEFAULT_PORT	    5680
#define IR_UNIX_DIR	    "/tmp/.iroha_unix"
#define IR_UNIX_PATH	    "/tmp/.iroha_unix/IROHA"
#define NAME_LENGTH	    15
#define HOST_NAME	    15
			
#define DATE_LENGH	    29
#define GETDATE 	    1
#define CONNECT 	    2
#define SETTIME 	    3
#define GETTIME 	    4


/* ユーザのクライアント管理テーブル */
typedef struct _Client {
    int 	id ;			     /* ソケット番号 */
    int 	usr_no ;		     /* ユーザ管理�� */
    int 	version ;		     /* protocol miner version */
    long	used_time ;		     /* ユーザ消費時間 */
    long	idle_date ;		     /* アイドル時間 */
    long	connect_date ;		     /* コネクトした時間 */
    char	*username ;		     /* ユーザ名  */
    char	*hostname ;		     /* ホスト名  */
    int 	pcount[ MAXREQUESTNO ] ;     /* プロトコルカウント */
    char	context_flag[ MAX_CX ] ;     /* コンテクスト管理フラグ */
} ClientRec ;			

typedef struct _UserTabl {
    char *username ;
    int count ;
} UsrTabl ;

typedef struct _ClientStat {
    int 	id ;			     /* ソケット番号 */
    int 	usr_no ;		     /* ユーザ管理番号 */
    long	used_time ;		     /* ユーザ消費時間 */
    long	idle_date ;		     /* アイドル時間 */
    long	connect_date ;		     /* コネクトした時間 */
    int 	pcount[ OLD_MAXREQUESTNO ] ; /* プロトコルカウント */
    char	username[ NAME_LENGTH+1] ;   /* ユーザ名  */
    char	hostname[ HOST_NAME ] ;      /* ホスト名  */
    char	context_flag[ OLD_MAX_CX ] ;	 /* コンテクスト管理フラグ */
} ClientStatRec ;		

typedef struct _AccessControlList {
    struct _AccessControlList  *prev ;
    struct _AccessControlList  *next ;
    unsigned char *hostname ;
    unsigned char *usernames ;
    int  usercnt ;
} ACLRec ;

typedef struct _Client *ClientPtr ;
typedef struct _ClientStat *ClientStatPtr ;
typedef struct _UserTabl *UserTblPtr ;
typedef struct _AccessControlList *ACLPtr ;

#define LOCAL_BUFSIZE		2048
#define BUFSIZE 		4096
/*
#define BUFWATERMARK		8192	
#define MAXBUFSIZE (1 << 18)
*/
#ifndef BCOPY
#include    <memory.h>
#define bcopy( src, dst, size )   memcpy( (char *)(dst), (char *)(src), (size) )
#define bzero( buf, size )	  memset( (char *)(buf), 0x00, (size) )
#endif /* BCOPY */

#ifndef MIN
#define MIN( n, m )	( ((n) > (m)) ? (m) : (n) )
#endif
#define LENTODATA( len, data ) { \
				long work ; \
				work = htonl( (len) ) ; \
				bcopy( (char *)&work, (char *)(data), sizeof( long ) ) ; \
				}

#define DATATOLEN( data, len ) { \
				long work ; \
				bcopy( (char *)(data), (char *)&work, sizeof( long ) ) ; \
				len = ntohl( (work) ) ; \
				}

#define CHECKCXT( num ) ( ( num >= 0) && ( num < MAX_CX ) && ( client->context_flag[ (num) ]) )
#define SETCXT( num ) ( client->context_flag[ (num) ] = 1 )
#define OFFCXT( num ) ( client->context_flag[ (num) ] = 0 )

#ifdef pcux
#include     <sys/kdef.h>
#if (NOFILES <= 128)
#define MAXSOCKS (NOFILES - 1)
#else
#define MAXSOCKS 128
#endif
#else /* pcux */

#include    <sys/param.h>
#if (NOFILE <= 128) /* 128 is value of MAXCLIENTS in dix layer */
#define MAXSOCKS (NOFILE - 1)
#else
#define MAXSOCKS 128
#endif
#endif /* pcux */

#define mskcnt ((MAXSOCKS + 31) / 32)	/* size of bit array */

#if (mskcnt==1)
#define BITMASK(i) (1 << (i))
#define MASKIDX(i) 0
#endif
#if (mskcnt>1)
#define BITMASK(i) (1 << ((i) & 31))
#define MASKIDX(i) ((i) >> 5)
#endif

#define MASKWORD(buf, i) buf[MASKIDX(i)]
#define BITSET(buf, i) MASKWORD(buf, i) |= BITMASK(i)
#define BITCLEAR(buf, i) MASKWORD(buf, i) &= ~BITMASK(i)
#define GETBIT(buf, i) (MASKWORD(buf, i) & BITMASK(i))

#if (mskcnt==1)
#define COPYBITS(src, dst) dst[0] = src[0]
#define CLEARBITS(buf) buf[0] = 0
#define MASKANDSETBITS(dst, b1, b2) dst[0] = (b1[0] & b2[0])
#define ORBITS(dst, b1, b2) dst[0] = (b1[0] | b2[0])
#define UNSETBITS(dst, b1) (dst[0] &= ~b1[0])
#define ANYSET(src) (src[0])
#endif
#if (mskcnt==2)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]
#define ANYSET(src) (src[0] || src[1])
#endif
#if (mskcnt==3)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]);\
		      dst[2] = (b1[2] & b2[2])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]);\
		      dst[2] = (b1[2] | b2[2])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]; \
		      dst[2] &= ~b1[2]
#define ANYSET(src) (src[0] || src[1] || src[2])
#endif
#if (mskcnt==4)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];\
		      dst[3] = src[3]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]);\
		      dst[2] = (b1[2] & b2[2]);\
		      dst[3] = (b1[3] & b2[3])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]);\
		      dst[2] = (b1[2] | b2[2]);\
		      dst[3] = (b1[3] | b2[3])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]; \
		      dst[2] &= ~b1[2]; \
		      dst[3] &= ~b1[3]
#define ANYSET(src) (src[0] || src[1] || src[2] || src[3])
#endif

#if (mskcnt>4)
#define COPYBITS(src, dst) bcopy((caddr_t) src, (caddr_t) dst,\
				 mskcnt*sizeof(long))
#define CLEARBITS(buf) bzero((caddr_t) buf, mskcnt*sizeof(long))
#define MASKANDSETBITS(dst, b1, b2)  \
		      { int cri;			\
			for (cri=0; cri<mskcnt; cri++)	\
			  dst[cri] = (b1[cri] & b2[cri]) }
#define ORBITS(dst, b1, b2)  \
		      { int cri;			\
		      for (cri=0; cri<mskcnt; cri++)	\
			  dst[cri] = (b1[cri] | b2[cri]) }
#define UNSETBITS(dst, b1) \
		      { int cri;			\
		      for (cri=0; cri<mskcnt; cri++)	\
			  dst[cri] &= ~b1[cri];  }
/*
 * If mskcnt>4, then ANYSET is a routine defined in WaitFor.c.
 *
 * #define ANYSET(src) (src[0] || src[1] || src[2] || src[3] || src[4] ...)
 */
#endif


