/* sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) IRproto.h 1.5 90/11/05 23:06:58"; */

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

#define VERSION    "1.2"
#define RETURN_VERSION_ERROR_STAT   -2

#define BUFSIZE 	4096
#define MAXDATA 	1024
#define OWNER		"bin"
#define EXIT		999

typedef struct _Req0{
    int Type ;
} Req0 ;

typedef struct _Req1{
    int Type ;
    int context ;
} Req1 ;

typedef struct _Req2{		
    int Type ;
    int namelen ;
    unsigned char *name ;
} Req2 ;

typedef struct _Req3{
    int Type ;
    int context ;
    int number ;
} Req3 ;

typedef struct _Req4{
    int Type ;
    int context ;
    int number ;
    int *kouho ;
} Req4 ;

typedef struct _Req5{
    int Type ;
    int context ;
    int number ;
    int datalen ;
} Req5 ;

typedef struct _Req6{
    int Type ;
    int context ;
    int mode ;
    int datalen ;
    unsigned char *data ;
} Req6 ;

typedef struct _Req7{
    int Type ;
    int context ;
    int diclen ;
    unsigned char *dicname ;
    int datalen ;
    unsigned char *datainfo ;
} Req7 ;

typedef struct _Req8{
    int Type ;
    int context ;
    int datalen ;
    unsigned char *data ;
    int mode ;
 } Req8 ;

typedef struct _Req9{
    int Type ;
    int context ;
    int number ;
    int datalen ;
    unsigned char *data ;
} Req9 ;

typedef struct _Req10{
    int Type ;
    int context ;
    int diclen ;
    unsigned char *dicname ;
    int textdiclen ;
    unsigned char *textdicname ;
    int mode ;
} Req10 ;

typedef struct _Req11{
    int Type ;
    int context ;
    int number ;
    int kouho ;
    int max ;
} Req11 ;

typedef struct _Req12{
    int Type ;
    int datalen ;
    unsigned char *data ;
    int number ;
} Req12 ;


/* 現在のところReq10が最大だから */			
/* プロトコルを追加したら、ちゃんとチェックするように */
#define     REQ_UNIONSIZE   sizeof( Req10 )
			
typedef union _IRReq {
    Req0 Request0 ;
    Req1 Request1 ;
    Req2 Request2 ;
    Req3 Request3 ;
    Req4 Request4 ;
    Req5 Request5 ;
    Req6 Request6 ;
    Req7 Request7 ;
    Req8 Request8 ;
    Req9 Request9 ;
    Req10 Request10;
    Req11 Request11;
    Req12 Request12;

    unsigned char Buffer[ REQ_UNIONSIZE ] ;
} IRReq ;

#define     ACK_BUFSIZE   2048

typedef struct _Ack0 {
    int stat ;
} Ack0 ;

typedef struct _Ack1 {
    int stat ;
    int len ;
    unsigned char data[ ACK_BUFSIZE ] ;
} Ack1 ;

typedef struct _Ack2 {
    int stat ;
    int len ;
    int info[ ACK_BUFSIZE/sizeof( int ) ] ;
} Ack2 ;

typedef union _IRAck {
    Ack0 Anck0 ;
    Ack1 Anck1 ;
    Ack2 Anck2 ;

    unsigned char SendAckBuffer[ ACK_BUFSIZE + sizeof( int )*2 ] ;
} IRAck ;	

/* いろはサーバプロトコル */
#define OLD_MAXREQUESTNO    0x18

#define MAXREQUESTNO	   0x1b
#define REALREQUEST	   0x1a

#define IR_INIT 	   0x01
#define IR_FIN		   0x02
#define IR_CRE_CON	   0x03
#define IR_DUP_CON	   0x04
#define IR_CLO_CON	   0x05
#define IR_DIC_LIST	   0x06
#define IR_GET_YOMI	   0x07
#define IR_DEF_DIC	   0x08
#define IR_UNDEF_DIC	   0x09
#define IR_DIC_PATH	   0x0a
#define IR_DIR_LIST	   0x0b
#define IR_MNT_DIC	   0x0c
#define IR_UMNT_DIC	   0x0d
#define IR_RMNT_DIC	   0x0e
#define IR_MNT_LIST	   0x0f
#define IR_CONVERT	   0x10
#define IR_CONV_END	   0x11
#define IR_KAN_LST	   0x12
#define IR_RESIZE	   0x13
#define IR_STO_YOMI	   0x14
#define IR_GET_LEX	   0x15
#define IR_GET_STA	   0x16
#define IR_SER_STAT	   0x17
#define IR_SER_STAT2	   0x18
#define IR_HOST_CTL	   0x19
#define IR_QUREY_EXT	   0x1a

#ifdef EXTENSION
#define EXTBASEPROTONO	   0x00010000
#define MAXEXTREQUESTNO    0x04

#define REMOTE_DIC_UTIL    "remote_dictionary_maintenance"
#define IR_LIST_DIC	   0x00
#define IR_CREAT_DIC	   0x01
#define IR_DEST_DIC	   0x02
#define IR_RENAME_DIC	   0x03
#define IR_GET_WORD_DIC    0x04
#else
#define EXTBASEPROTONO	   0x00000000
#define MAXEXTREQUESTNO    0x00
#endif /* EXTENSION */
