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
/*
  This program contains a part of the X library.  The communication part
  of the X library is modified and built into this program.
 */
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
static char sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) rkcConvert.c 1.9 91/05/28 16:33:01";
/* LINTLIBRARY */

#ifdef SHLIB
#include "idefine.h"
#endif
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>

#include "net.h"
#include "RK.h"
#include "IR.h"
#include "rkc.h"

extern int    open() ;				
extern int    read() ;				
extern int    write() ; 			
extern int    close() ; 			
extern FILE  *fopen() ; 			
extern int    fclose() ;			
extern char  *malloc() ;
extern char  *free() ;
extern char  *getenv() ;
extern int   strlen() ;
extern char *strcpy() ;
extern char *strncpy() ;
extern char *strtok() ; 			
extern char *strcat() ;
extern int   strcmp() ; 		
extern int   atoi() ;			

extern unsigned long  inet_addr() ;		
extern char *gethostname() ;			
extern int    socket() ;			
extern int    connect() ;			
extern int    getmsg() ;			
extern int    putmsg() ;			
/*
extern unsigned short htons() ; 		
extern unsigned long  htonl() ; 	
extern unsigned long  ntohl() ; 	
*/
extern int   errno ;

#ifndef IROHAHOSTFILE
#define IROHAHOSTFILE	    "/usr/lib/iroha/irohahost"
#endif

#define LOCALHOSTADDR	    "127.0.0.1"
#define RkcFree( p )	{ if( (p) ) (void)free( (char *)(p) ) ; }

#define ReqType0    0
#define ReqType1    1
#define ReqType2    2
#define ReqType3    3
#define ReqType4    4
#define ReqType5    5
#define ReqType6    6
#define ReqType7    7
#define ReqType8    8
#define ReqType9    9
#define ReqType10   10
#define ReqType11   11
#define ReqType12   12

#define ACK0 0
#define ACK1 1
#define ACK2 2

static int ServerFD ;
static int WriteServer() ;
static int WriteToServer() ;
static int ReadFromServer() ;
static IRAck	Acknowledge ;
static IRReq	IRreq ;

/*
 * サーバから返された第一候補列を、第一候補列バッファに格納する。
 *  したがって、bun->kanjiのポインタの指す位置は変化しない。
 */
static
int
rkc_StoreFirstKouho( cx )
RkcContext *cx ;
{
    Ack1 *ack = &Acknowledge.Anck1 ;
    register unsigned char *return_kouho, *wp ;
    register int i, save_len ;
    unsigned char *first_kouho = cx->Fkouho ;
    int length ;

    /* コピーすべきバッファの大きさを調べる */
    for( save_len = i = 0; i < cx->curbun; i++ ) {
	length = strlen( (char *)first_kouho )+1 ;
	first_kouho += length ;
	save_len += length ;
    }

    if( !(wp = return_kouho = (unsigned char *)malloc( save_len + ack->len )) )
	return( -1 ) ;

    for( first_kouho = cx->Fkouho, i = 0; i < cx->curbun; i++ ) {
	strcpy( wp, first_kouho ) ;
	length = strlen( (char *)first_kouho )+1 ;
	wp += length;
	first_kouho += length ;
    }
    for( first_kouho = ack->data; i < ack->stat; i++ ) {
	length = strlen( first_kouho ) + 1 ;
	strcpy( wp, first_kouho ) ;
	wp += length ;
	first_kouho += length ;
    }

    *(++wp) = '\0' ;
    RkcFree( (char *)cx->Fkouho ) ;
    cx->Fkouho = return_kouho ;
    return( ack->stat ) ;
}

rkc_Connect_Iroha_Server( hostname )
char *hostname ;
{
    struct sockaddr_in inaddr;	    /* INET socket address. */
    unsigned long hostinetaddr;     /* result of inet_addr of arpa addr */
#ifdef UNIXCONN
    struct sockaddr_un unaddr;	    /* UNIX socket address. */
#endif /* UNIXCONN */
    struct sockaddr *addr;	    /* address to connect to */
    struct hostent *host_ptr ;
    struct servent *sp ;
    char irohahostname[ MAX_HOSTNAME ], *hostpt ; /* pointer to server host name */
    FILE *HostFp ;
    int addrlen ;		    /* length of address */
    int num ;
    char *number ;

    bzero( irohahostname, MAX_HOSTNAME ) ;
    if( hostname[0] == '\0' ) {
	if ( (hostpt = getenv( "IROHAHOST" )) != (char *)NULL ) {
	    strncpy( irohahostname, hostpt, MAX_HOSTNAME ) ;
	}
	else if( (HostFp = fopen( IROHAHOSTFILE, "r" )) != (FILE *)NULL ) {
	    if ( (hostpt=fgets( irohahostname, MAX_HOSTNAME, HostFp)) == (char *)NULL ) {
		strcpy( irohahostname, "unix" ) ;
	    } else {
		/* irohahostnameには、改行文字が最後に入っているから */
		strcpy( irohahostname, hostpt ) ;
		irohahostname[ strlen( hostpt )-1 ] = '\0' ;
	    }
	    fclose( HostFp ) ;
	} else {
	    strcpy( irohahostname, "unix" ) ;
	}
    } else {
	strncpy( irohahostname, hostname, MAX_HOSTNAME ) ;
    }

    strtok( irohahostname, ":" ) ;
    number = (char *)strtok( NULL, ":" ) ;
    num = number ? atoi( number ) : 0 ;
    strcpy( hostname, irohahostname ) ;
    if( num )
	sprintf( hostname,"%s:%d", hostname, num ) ;

#ifdef UNIXCONN	
    if ( (strcmp("unix", irohahostname) == 0) ||
	(strcmp("localhost", irohahostname) == 0) ||
	(strcmp(LOCALHOSTADDR, irohahostname) == 0)) {
	/* いろはサーバと、ＵＮＩＸドメインで接続 */
	unaddr.sun_family = AF_UNIX;
	if( num )
	    sprintf( unaddr.sun_path,"%s:%d", IR_UNIX_PATH, num ) ;
	else
	    strcpy( unaddr.sun_path, IR_UNIX_PATH ) ;
	addr = (struct sockaddr *) &unaddr;
	addrlen = strlen( (char *)unaddr.sun_path ) + 2;
	/*
	 * Open the network connection.
	 */
	if ((ServerFD = socket((int) addr->sa_family, SOCK_STREAM, 0)) >= 0){
	    if (connect(ServerFD, addr, addrlen) == -1) 
		 (void)close( ServerFD ) ;
	}
    } else { 
#endif /* UNIXCONN */
	/* インターネットドメインで接続する。 */
	if ( (host_ptr = gethostbyname( irohahostname ) )
	                             == (struct hostent *)NULL) {
	    hostinetaddr = inet_addr( irohahostname );
	    if (hostinetaddr == -1) {
		/* No such host! */
		errno = EINVAL;
		return( -1 );
	    } else {
		host_ptr = gethostbyaddr( (char *)&hostinetaddr,
					sizeof( hostinetaddr ), AF_INET ) ;
		if( !host_ptr ) {
		    errno = EINVAL;
		    return( -1 ) ;
		}
	    }
	} else { 
	    /* Check the address type for an internet host. */
	    if (host_ptr->h_addrtype != AF_INET) {
		/* Not an Internet host! */
		errno = EPROTOTYPE;
		return( -1 );
	    }
	}
	/*
	 * Open the network connection.
	 */
	
	if ((ServerFD = socket( AF_INET, SOCK_STREAM, 0)) < 0){
#ifdef COMMENT
	    perror("socket");
	    printf("errno=%d\n", errno);
#endif	
	    return(-1);	/* errno set by system call. */
	}
	/* make sure to turn off TCP coalescence */
	errno = 0;
	
	/* /etc/servicesからポート番号を取得する(NULL もみとめる) */ 
	sp = getservbyname(IR_SERVICE_NAME, "tcp");
	
	/*
	  snnoblock( ServerFD, 1 ) ;
	  */

	/* Set up the socket data. */
	inaddr.sin_family = host_ptr->h_addrtype;
	inaddr.sin_port = (sp ? sp->s_port : IR_DEFAULT_PORT) + htons( num ) ;
	bcopy( host_ptr->h_addr, &inaddr.sin_addr, sizeof(inaddr.sin_addr));
	addrlen = sizeof(struct sockaddr_in) ;
	errno = 0 ; 
	if (connect(ServerFD, &inaddr, addrlen) < 0) {
#ifdef COMMENT
	    perror("connect");
	    printf("errno=%d\n", errno);
#endif
	    (void) close (ServerFD);
	    return( -1 );	      /* errno set by system call. */
	}
#ifdef UNIXCONN
    }
#endif    
    return( ServerFD ) ;
}

rkc_initialize( username )
char *username ;
{
    Req2 *req = &IRreq.Request2 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int len = strlen( (char *)username ) + 1 ;

    /*	パケット組み立て */
    req->Type = htonl( IR_INIT ) ;
    req->namelen = htonl( len ) ;
    req->name = (unsigned char *)username ;

    if( WriteToServer( ReqType2, len ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    if( ack->stat < 0 )
	close( ServerFD ) ;

    return( ack->stat );
}

static
Fin_Create( request )
int request ;
{
    Req0 *req = &IRreq.Request0 ;
    Ack0 *ack = &Acknowledge.Anck0 ;

    req->Type = htonl( request ) ;

    if( WriteToServer( ReqType0, sizeof( Req0 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    if( request == IR_FIN )
	(void)close( ServerFD ) ;

    return( ack->stat );
}

rkc_finalize()
{
    return( Fin_Create( IR_FIN ) ) ;
}

rkc_create_context()
{
    return( Fin_Create( IR_CRE_CON ) ) ;
}

static
Dup_Close_CX( cx_num, request )
int cx_num, request ;
{
    Req1 *req = &IRreq.Request1 ;

    req->Type = htonl( request ) ;
    req->context = htonl( cx_num ) ;

    if( WriteToServer( ReqType1, sizeof( Req1 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    return( Acknowledge.Anck0.stat );
}

rkc_duplicate_context( cx )
register RkcContext *cx ;
{
    return( Dup_Close_CX( cx->server, IR_DUP_CON ) ) ;
}

rkc_close_context( cx )
register RkcContext *cx ;
{
    return( Dup_Close_CX( cx->server, IR_CLO_CON ) ) ;
}

static
Dic_Dir_List( context, data, max, request )
int context, max, request ;
unsigned char *data ;
{
    Req3 *req = &IRreq.Request3 ;
    unsigned char *wp, *datap ;
    int ret, len, i ;

    req->Type = htonl( request ) ;
    req->context = htonl( context ) ;
    req->number = htonl( max ) ;

    if( WriteToServer( ReqType3, sizeof( Req3 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    ret = Acknowledge.Anck1.stat ;
    wp = Acknowledge.Anck1.data ;

    datap = data ;
    for( i = 0; i < ret; i++){
	len = strlen( wp ) + 1 ;
	strcpy( datap, wp ) ;
	datap += len ;
	wp += len ;
    }
    *(datap++) = '\0' ;
    *(datap) = '\0' ;

    return( ret );
}

rkc_dictionary_list( cx, dicnames, max)
register RkcContext *cx ;
unsigned char *dicnames ;
int max ;
{
    return( Dic_Dir_List( cx->server, dicnames, max, IR_DIC_LIST ) ) ;
}

static
Define_Delete_dic( cx, dicname, wordrec, request )
register RkcContext *cx ;
unsigned char *dicname, *wordrec ;
{
    Req7 *req = &IRreq.Request7 ;
    int diclen, datalen ;

    req->Type = htonl( request ) ;
    req->context = htonl( cx->server ) ;
    diclen = strlen( (char *)dicname ) + 1 ;
    req->diclen = htonl( diclen ) ;
    req->dicname = dicname ;
    datalen = strlen( (char *)wordrec ) + 1 ;
    req->datalen = htonl( datalen ) ;
    req->datainfo = wordrec ;

    if( WriteToServer( ReqType7, diclen, datalen ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    return( Acknowledge.Anck0.stat );
}

rkc_define_dic( cx, dicname, wordrec)
register RkcContext *cx ;
unsigned char *dicname, *wordrec ;
{
    return ( Define_Delete_dic( cx, dicname, wordrec, IR_DEF_DIC ) ) ;
}

rkc_delete_dic( cx,  dicname, wordrec)
register RkcContext *cx ;
unsigned char *dicname, *wordrec ;
{
    return ( Define_Delete_dic( cx, dicname, wordrec, IR_UNDEF_DIC ) ) ;
}

static
Call_Request8( context, data, mode, request )
int context, mode, request ;
unsigned char *data ;
{
    Req8 *req = &IRreq.Request8 ;
    int datalen = strlen( (char *)data ) + 1 ;

    req->Type = htonl( request ) ;
    req->context = htonl( context ) ;
    req->datalen = htonl( datalen ) ;
    req->data = data ;
    req->mode = htonl( mode ) ;

    if( WriteToServer( ReqType8, datalen ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    return( Acknowledge.Anck0.stat );
}

rkc_mount_dictionary( cx, dicname, mode )
register RkcContext *cx ;
char *dicname ;
int  mode ;
{
    return( Call_Request8( cx->server, dicname, mode, IR_MNT_DIC ) ) ;
}

rkc_umount_dictionary( cx, dicname )
register RkcContext *cx ;
char *dicname ;
{
    return( Call_Request8( cx->server, dicname, 0, IR_UMNT_DIC ) ) ;
}


rkc_remount_dictionary( cx, dicname, where )
register RkcContext *cx ;
unsigned char *dicname ;
int where ;
{
    Req9 *req = &IRreq.Request9 ;
    int datalen ;

    req->Type = htonl( IR_RMNT_DIC ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( where ) ;
    datalen = strlen( (char *)dicname ) + 1 ;
    req->datalen = htonl( datalen ) ;
    req->data = dicname ;

    if( WriteToServer( ReqType9, strlen( dicname ) + 1 ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    return( Acknowledge.Anck0.stat );
}

rkc_mount_list( cx, data, max)
RkcContext *cx ;
unsigned char *data ;
int max ;
{
    return( Dic_Dir_List( cx->server, data, max, IR_MNT_LIST ) ) ;
}

rkc_get_dir_list( cx, ddname, maxddname )
RkcContext *cx ;
unsigned char *ddname ;
int maxddname ;
{
    return( Dic_Dir_List( cx->server, ddname, maxddname, IR_DIR_LIST ) ) ;
}

rkc_convert( cx, yomi, length, mode )
RkcContext *cx ;
int length ,mode;
unsigned char *yomi ;
{
    Req8 *req = &IRreq.Request8 ;
    int datalen = MIN( length, strlen( yomi ) + 1 ) ;
    int ret ;

    req->Type = htonl( IR_CONVERT ) ;
    req->context = htonl( cx->server ) ;
    req->datalen = htonl( datalen ) ;
    req->data = yomi ;
    req->mode = htonl( mode ) ;

    if( WriteToServer( ReqType8, datalen ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    if( (ret = rkc_StoreFirstKouho( cx )) < 0 ){
	rkc_convert_end( cx->server, 0 ) ; /* サーバ側もRkBigBunを終了 */
	return( -1 ) ;			   /* させる		       */
    }
    return( ret ) ;
}

rkc_convert_end( cx, mode )
RkcContext *cx ;
int mode ;
{
#define GAKUSHUU    1
    register Req4 *req = &IRreq.Request4 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    register RkcBun *bun ;
    int i ;

    req->Type = htonl( IR_CONV_END ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( (mode == GAKUSHUU ? cx->maxbun : 0) ) ;

    if( mode == GAKUSHUU ) {
	if( !(req->kouho = (int *)malloc( sizeof( int )*(cx->maxbun) )) )
	    return( -1 ) ;
					
	for( i = 0; i < cx->maxbun; i++ ) {
	    bun = &cx->bun[ i ] ;
	    req->kouho[ i ] = htonl( bun->curcand ) ;
	}
    }

    if( WriteToServer( ReqType4, (mode == GAKUSHUU ? cx->maxbun : 0) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK0 ) < 0 )
	return( -1 ) ;

    return( ack->stat );
}

rkc_get_yomi( cx, yomip )
register RkcContext *cx ;
unsigned char **yomip ;
{		
    Req5 *req = &IRreq.Request5 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    RkcBun *bun = &cx->bun[ cx->curbun ] ;

    req->Type = htonl( IR_GET_YOMI ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;
    req->datalen = htonl( BUFSIZE ) ;

    if( WriteToServer( ReqType5, sizeof( Req5 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    *yomip = ack->data ;
    return( ack->stat ) ;
}

rkc_get_kanji_list( cx )
register RkcContext *cx ;
{		
    Req5 *req = &IRreq.Request5 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    RkcBun *bun = &cx->bun[ cx->curbun ] ;
    unsigned char *kouho_list ;

    req->Type = htonl( IR_KAN_LST ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;
    req->datalen = htonl( BUFSIZE ) ;

    if( WriteToServer( ReqType5, sizeof( Req5 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    if( !(kouho_list = (unsigned char *)malloc( ack->len )) )
	return( -1 ) ;

    bcopy( ack->data, kouho_list, ack->len ) ;
    bun->kanji = kouho_list ;

    return( ack->stat ) ;
}

rkc_resize( cx, yomi_length )
register RkcContext *cx ;
int yomi_length ;
{
    Req5 *req = &IRreq.Request5 ;

    req->Type = htonl( IR_RESIZE ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;     /* カレント文節番号 */
    req->datalen = htonl( yomi_length ) ;

    if( WriteToServer( ReqType5, sizeof( Req5 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    return( rkc_StoreFirstKouho( cx ) ) ;
}

rkc_store_yomi( cx, yomi, max )
register RkcContext *cx ;
unsigned char *yomi ;
int max ;
{
    Req9 *req = &IRreq.Request9 ;
    int datalen = MIN( strlen( yomi ) + 1, max ) ;

    req->Type = htonl( IR_STO_YOMI ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;
    req->datalen = htonl( datalen ) ;
    req->data = yomi ;

    if( WriteToServer( ReqType9, datalen ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    return( rkc_StoreFirstKouho( cx ) ) ;
}

static
int RemoteDicUtilBaseProtoNumber = 0 ;

static
Query_Extension()
{
    if( !RemoteDicUtilBaseProtoNumber ){
	Req12 *req = &IRreq.Request12 ;
	Ack0 *ack = &Acknowledge.Anck0 ;
	int datalen = strlen( REMOTE_DIC_UTIL ) + 1 ;

	req->Type = htonl( IR_QUREY_EXT ) ;
	req->datalen = htonl( datalen ) ;
	req->data = (unsigned char *)REMOTE_DIC_UTIL ;
	req->number = htonl( MAXEXTREQUESTNO + 1 ) ;

	if( WriteToServer( ReqType12, datalen ) < 0 )
	    return( -1 ) ;
	if( ReadFromServer( ACK0 ) < 0 )
	    return( -1 ) ;
	return( RemoteDicUtilBaseProtoNumber = ack->stat ) ;
    } else {
	return( RemoteDicUtilBaseProtoNumber ) ;
    }
}

#ifdef EXTENSION
rkc_list_dictionary( cx, dirname, dicnames_return, size )
register RkcContext *cx ;
unsigned char *dirname, *dicnames_return ;
int size ;
{
    Req9 *req = &IRreq.Request9 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    int datalen = MIN( strlen( dirname ) + 1, size ) ;
    int extension_base = Query_Extension() ;
    unsigned char *datap, *wp ;
    int ret, i ;

    if( extension_base < 0 )
	return( -1 ) ;

    req->Type = htonl( IR_LIST_DIC + extension_base ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( size ) ;
    req->datalen = htonl( datalen ) ;
    req->data = dirname ;

    if( WriteToServer( ReqType9, datalen ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK1 ) < 0 )
	return( -1 ) ;

    ret = Acknowledge.Anck1.stat ;
    wp = Acknowledge.Anck1.data ;

    datap = dicnames_return ;
    for( i = 0; i < ret; i++){
	register int len ;

	len = strlen( wp ) + 1 ;
	strcpy( datap, wp ) ;
	datap += len ;
	wp += len ;
    }
    *(datap++) = '\0' ;
    *(datap) = '\0' ;

    return( ret );
}

rkc_create_dictionary( cx, dicname, mode )
register RkcContext *cx ;
unsigned char *dicname ;
int mode ;
{
    int extension_base = Query_Extension() ;

    if( extension_base < 0 )
	return( -1 ) ;
    return( Call_Request8( cx->server, dicname, mode,
					IR_CREAT_DIC + extension_base ) ) ;
}

rkc_destroy_dictionary( cx, dicname )
register RkcContext *cx ;
unsigned char *dicname ;
{
    int extension_base = Query_Extension() ;

    if( extension_base < 0 )
	return( -1 ) ;
    return( Call_Request8( cx->server, dicname, 0,
					IR_DEST_DIC + extension_base ) ) ;
}

static
Call_Request10( context, data1, data2, mode, request, ack )
int context, mode, request, ack ;
unsigned char *data1, *data2 ;
{
    Req10 *req = &IRreq.Request10 ;
    int data1len =  strlen( data1 ) + 1 ;
    int data2len = 0 ;

    if( data2 )
	data2len = strlen( data2 ) + 1 ;

    req->Type = htonl( request ) ;
    req->context = htonl( context ) ;
    req->diclen = htonl( data1len ) ;
    req->textdiclen = htonl( data2len ) ;
    req->mode = htonl( mode ) ;
    req->dicname = data1 ;
    req->textdicname = data2 ;

    if( WriteToServer( ReqType10, data1len, data2len ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ack ) < 0 )
	return( -1 ) ;

    return( Acknowledge.Anck0.stat ) ;
}

rkc_rename_dictionary( cx, dicname, newdicname, mode )
register RkcContext *cx ;
unsigned char *dicname, *newdicname ;
int mode ;
{
    int extension_base = Query_Extension() ;

    if( extension_base < 0 )
	return( -1 ) ;
    return( Call_Request10( cx->server, dicname, newdicname, mode,
				IR_RENAME_DIC + extension_base, ACK0 ) ) ;
}

rkc_get_text_dictionary( cx, dirname, dicname, info, infolen )	
register RkcContext *cx ;
unsigned char *dirname, *dicname, *info ;
int infolen ;
{
    int extension_base = Query_Extension() ;
    int ret ;

    if( extension_base < 0 )
	return( -1 ) ;

    ret = Call_Request10( cx->server, dirname, dicname, infolen,
			     IR_GET_WORD_DIC + extension_base, ACK1 ) ;
    if( ret > 0 )
	strncpy( info, Acknowledge.Anck1.data, infolen ) ;

    return( ret ) ;
}

			
#endif /* EXTENSION */

rkc_get_stat( cx, stat )
register RkcContext *cx ;
RkStat *stat ;
{
    Req5 *req = &IRreq.Request5 ;
    Ack2 *ack = &Acknowledge.Anck2 ;
    RkcBun *bun = &cx->bun[ cx->curbun ] ;

    req->Type = htonl( IR_GET_STA ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;
    req->datalen = htonl( bun->curcand ) ;

    if( WriteToServer( ReqType5, sizeof( Req5 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK2 ) < 0 )
	return( -1 ) ;

    bcopy( ack->info, stat, sizeof( RkStat ) ) ;

    return( ack->stat );
}

rkc_get_lex( cx, max, info_return )
register RkcContext *cx ;
int max ;
RkLex *info_return ;
{
    RkcBun *bun = &cx->bun[ cx->curbun ] ;
    Ack2 *ack = &Acknowledge.Anck2 ;
    Req11 *req ;

    req = &IRreq.Request11 ;
    req->Type = htonl( IR_GET_LEX ) ;
    req->context = htonl( cx->server ) ;
    req->number = htonl( cx->curbun ) ;
    req->kouho = htonl( bun->curcand ) ;
    req->max = htonl( max ) ;

    if( WriteToServer( ReqType11, sizeof( Req11 ) ) < 0 )
	return( -1 ) ;

    if( ReadFromServer( ACK2 ) < 0 )
	return( -1 ) ;

    bcopy( ack->info, info_return, sizeof( RkLex )*(ack->stat) ) ;

    return( ack->stat );
}

#define SEND_BUF_SIZE	2048

static
unsigned char SendBuffer[ SEND_BUF_SIZE ] ;

static unsigned char *
Malloc( size )
int size ;
{
    if( size > SEND_BUF_SIZE ) {
	return( (unsigned char *)malloc( size ) ) ;
    } else {
	return( SendBuffer ) ;
    }
}

static
Free( ptr )
unsigned char *ptr ;
{
    if( (ptr == SendBuffer) || (ptr == IRreq.Buffer) )
	return ;
    free( ptr ) ;
}

static int
WriteToServer( type, size1, size2 )
int type, size1, size2 ;
{
    unsigned char *bufptr ;
    int  len, *info, i, write_size, result ;

    switch( type ) {
	case ReqType0 : 
	case ReqType1 : 
	case ReqType3 : 
	case ReqType5 : 
	case ReqType11 :
		write_size = size1 ;
		bufptr = IRreq.Buffer ;
		break ;
	case ReqType2 : 
	    {
		Req2 *req = &IRreq.Request2 ;
		
		write_size = size1 + sizeof( int )*2 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*2 ) ;
		bcopy( req->name, bufptr + sizeof( int )*2, size1 ) ;
		break ;
	    }
	case ReqType4 : 
	     {
		Req4 *req = &IRreq.Request4 ;

		write_size = sizeof( int )*(size1 + 3 ) ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*3 ) ;
		bcopy( req->kouho, bufptr + sizeof( int )*3,
						    sizeof( int )*size1 ) ;
		if( size1 )
		    free( (char *)req->kouho ) ;
		break ;
	     }
	case ReqType7 : 
	    {
		Req7 *req = &IRreq.Request7 ;
		
		write_size = size1 + size2 + sizeof( int )*4 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*3 ) ;
		bcopy( req->dicname, bufptr + sizeof( int )*3, size1 ) ;
		bcopy( &req->datalen,
			  bufptr + sizeof( int )*3 + size1,sizeof( int ) ) ;
		bcopy( req->datainfo,
				bufptr + sizeof( int )*4 + size1, size2 ) ;
		break ;
	    }
	case ReqType8 : 
	    {
		Req8 *req = &IRreq.Request8 ;
		
		write_size = size1 + sizeof( int )*4 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*3 ) ;
		bcopy( req->data, bufptr + sizeof( int )*3, size1 ) ;
		bcopy( &req->mode,
			  bufptr + sizeof( int )*3 + size1, sizeof( int ) ) ;
		break ;
	    }
	case ReqType9 : 
	    {
		Req9 *req = &IRreq.Request9 ;
		
		write_size = size1 + sizeof( int )*4 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*4 ) ;
		bcopy( req->data, bufptr + sizeof( int )*4, size1 ) ;
		break ;
	    }
	case ReqType10 :
	    {
		Req10 *req = &IRreq.Request10 ;
		
		write_size = size1 + size2 + sizeof( int )*5 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*3 ) ;
		if( req->diclen )
		    bcopy( req->dicname, bufptr + sizeof( int )*3, size1 ) ;
		bcopy( &req->textdiclen,
			  bufptr + sizeof( int )*3 + size1,sizeof( int ) ) ;
		bcopy( req->textdicname,
				bufptr + sizeof( int )*4 + size1, size2 ) ;
		bcopy( &req->mode,
		   bufptr + sizeof( int )*4 + size1 + size2,sizeof( int ) ) ;
		break ;
	    }
	case ReqType12 :
	    {
		Req12 *req = &IRreq.Request12 ;
		
		write_size = size1 + sizeof( int )*3 ;
		if( !(bufptr = Malloc( write_size )) ) {
		    return( -1 ) ;
		}
		bcopy( req, bufptr, sizeof( int )*2 ) ;
		bcopy( req->data, bufptr + sizeof( int )*2, size1 ) ;
		bcopy( &req->number, bufptr + sizeof( int )*2 + size1,
							   sizeof( int ) ) ;
		break ;
	    }
	case ReqType6 : 
	default :
	    return( -1 ) ;
    }
    result = WriteServer( bufptr, write_size ) ;
    Free( bufptr ) ;
    return( result ) ;
}

static int
WriteServer( Buffer, size )
unsigned char *Buffer ;
int size ;
{
    register int todo;
    register int write_stat;
    register unsigned char *bufindex;
    static void (*Sig)(), (*signal())();
    static void DoSomething();

    errno = 0 ;
    bufindex = Buffer ;
    todo = size ;
    Sig = signal(SIGPIPE, DoSomething);
    while (size) {
	errno = 0;
	write_stat = write(ServerFD, bufindex, (int) todo);
	if (write_stat >= 0) {
	    size -= write_stat;
	    todo = size;
	    bufindex += write_stat;
	} else if (errno == EWOULDBLOCK) {   /* pc98 */
	    continue ;
#ifdef EMSGSIZE
	} else if (errno == EMSGSIZE) {
	    if (todo > 1)
		todo >>= 1;
	    else
		continue ;
#endif
	} else {
	    /* errno set by write system call. */
	    errno = EPIPE ;
	    close( ServerFD ) ;
	    signal(SIGPIPE, Sig);
	    return( -1 ) ;
	}
    }
    return( 0 ) ;
}

#define READ_BUF_SIZE	    (ACK_BUFSIZE + sizeof( int )*2 )
#define TRY_COUNT	    10

static int
ReadFromServer( type )
int type ;
{
    unsigned char *bufptr = Acknowledge.SendAckBuffer ;
    int readlen, len, i ;
    int bufsize = READ_BUF_SIZE ;
    int bufcnt = 0 ;
    int empty_count = 0 ;

    errno = 0 ;
    while( empty_count < TRY_COUNT ) {
	if ( (readlen = read( ServerFD, bufptr, bufsize )) < 0 ) {
	    empty_count = TRY_COUNT ;
	    break ;
	} else if ( readlen == 0 ) {
	    empty_count ++ ;
	    continue ;
	}

	bufcnt += readlen ;
	bufptr += readlen ;
	bufsize -= readlen ;
	switch( type ) {
	    case ACK0 : 
	    {
		Ack0 *ack = &Acknowledge.Anck0 ;

		if( bufcnt < sizeof( int ) )
		    continue ;

		ack->stat = ntohl( ack->stat ) ;
		return( 0 ) ;
	    }
	    case ACK1 : 
	    {
		Ack1 *ack = &Acknowledge.Anck1 ;
		unsigned char *src, *dst ;
		long work ;
		int cnt ;

		if( bufcnt < sizeof( int )*2 )
		    continue ;

		len = ntohl( ack->len ) ;
		if( bufcnt < (len + sizeof( int )*2) )
		    continue ;

		ack->stat = ntohl( ack->stat ) ;
		ack->len = len ;
		src = dst = ack->data ;
		for( i = 0; i < len; i += (work + sizeof( int )) ) {
		    bcopy( src, &work, sizeof( long ) ) ;
		    work = ntohl( work ) ;
		    src += sizeof( long ) ;
		    bcopy( src , dst, work ) ;
		    src += work ;
		    dst += work ;
		}
		return( 0 ) ;
	    }
	    case ACK2 :
	    {
		Ack2 *ack = &Acknowledge.Anck2 ;
		int i, *info ;
		long work ;

		if( bufcnt < sizeof( int )*2 )
		    continue ;
	
		len = ntohl( ack->len ) ;
		if( bufcnt < (len + sizeof( int )*2) )
		    continue ;
	
		ack->stat = ntohl( ack->stat ) ;
		ack->len = len ;
		info = ack->info ;
		for( i = 0; i < len; i += sizeof( int ), info++ ) {
		    bcopy( info, &work, sizeof( long ) ) ;
		    work = ntohl( work ) ;
		    bcopy( &work, info, sizeof( long ) ) ;
		}
		return( 0 ) ;
	    }
	    default :
		return( -1 ) ;
	}
    }
    errno = EPIPE;
    close( ServerFD ) ;
    return( -1 ) ;
}

static void DoSomething(sig)
{
#ifdef DEBUG
    fprintf(stderr, "irohaserver not responding.\n");
#endif
    errno = EPIPE ;
}
