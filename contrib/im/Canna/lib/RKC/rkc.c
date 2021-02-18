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

#ifndef lint
static char sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) rkc.c 1.7 91/02/05 14:33:03";
static char rcs_id[] = "$Header: /work/nk.R3.1/lib/RKC/RCS/rkc.c,v 3.4 91/08/29 10:28:28 kubota Exp $";
#endif

/* LINTLIBRARY */

#include    <stdio.h>
#include    <sys/types.h>
#include    <errno.h>
#include    <pwd.h>
#include    <signal.h>
	
#include    "IRproto.h" 
#include    "RK.h"
#include    "RKintern.h"
#include    "rkc.h"

/* CX:	����ƥ����ȥơ��֥�
 *	ɬ�פʥ쥳���ɤ�����malloc�Ǻ������롣
 *	^^^^^^^^^^^^^^^^^^
 */
#define MAX_CX	100   /* �����RK.h���������٤���Τ����龭��̵���ʤ� */

static RkcContext *CX[MAX_CX] ;

#define RkcFree( p )	{ if( (p) ) (void)free( (char *)(p) ) ; }

#define BUSY	1

static short
rkc_call_flag = 0x00   ; /* RkInitialize���ƤФ��RkFinalize���ƤФ��ޤ� */
			 /* BUSY�ӥåȤ�Ω�äƤ���			   */

static short ServerVersion = 0 ;

static char ConnectIrohaServerName[ MAX_HOSTNAME ] ;
char *RkGetServerName() ;

/*
 * ���饤����ȡ�����ƥ����ȥ桼�ƥ���ƥ��ؿ�
 */


/*
 * ����������ƥ����Ȥ�������롣
 */
static
RkcContext  *
newCC()
{
    register RkcContext *cx ;
    register int i ;

    for( i = 0; i < MAX_CX; i++) {
	if( !CX[ i ] ) {
	    cx = (RkcContext *)malloc( sizeof( RkcContext ) ) ;
	    if( cx ) {
		cx->client = i ;
		cx->bun = (RkcBun *)NULL ;
		cx->Fkouho = (unsigned char *)NULL ;
		cx->curbun = cx->maxbun = cx->bgnflag = 0 ;
		CX[ i ] = cx ;
	    }
	    return( cx ) ;
	}
    }
    return( (RkcContext *)NULL ) ;
}

/*
 * ���ꤵ�줿ʸ�ᤫ��ǽ�ʸ��ޤǤ���Ƭ����ޤ��ϡ���������ΰ���������
 */
static
void
freeBUN(cx, from)
register RkcContext *cx ;
register int	    from ;
{
    register RkcBun *bun ;

    for( ; from < cx->maxbun; from++ ) {
	bun = &cx->bun[ from ] ;
	if( bun->flags == NUMBER_KOUHO ) {
	    /* �����䤷�����äƤ��ʤ�ʸ��ϡ��ºݤˤ�malloc�����櫓�ǤϤʤ�
	     * �����Ф������Τ��줿��ʸ��������������ؤΥݥ��󥿤�
	     * ���ꤷ�Ƥ������������ե꡼���ʤ���
	     */
	    RkcFree( (char *)bun->kanji ) ;
	    bun->kanji = (unsigned char *)NULL ;
	    bun->curcand = bun->maxcand = 0 ;
	    bun->flags = NOTHING_KOUHO ;
	}
    }
}

/*
 * ���ꤵ�줿����ƥ����Ȥ�������롣
 */
static
void
freeCC( clientcx )
int clientcx ;
{
    register RkcContext     *cx ;

    if( (0 <= clientcx) && (clientcx < MAX_CX) ) {
	cx = CX[ clientcx ] ;
	if( cx->bun ) {
	    freeBUN( cx, 0 ) ;
	    RkcFree( (char *)cx->bun ) ;
	    cx->bun = (RkcBun *)NULL ;
	}
	RkcFree( (char *)cx->Fkouho ) ;
	cx->Fkouho = (unsigned char *)NULL ;
	cx->curbun = cx->maxbun = 0 ;
	cx->bgnflag = 0 ;
	RkcFree( (char *)cx ) ;
	cx = (RkcContext *)NULL ;
	CX[ clientcx ] = (RkcContext *)NULL ;
    }
}

/*
 * ����ƥ������ֹ���б���������ƥ����Ȥ�������롣
 * (���饤�����¦)
 */
#define NOCHECK    0
#define CHECK	   1
static
RkcContext *
getCC( clientcx, type )
int	clientcx, type ;
{
    register RkcContext     *cx = (RkcContext *)NULL ;
    register RkcBun	    *bun ;

    if( (0 <= clientcx) && (clientcx < MAX_CX) ) {
	if( cx = CX[ clientcx ] )
	    if( (type == CHECK) && (cx->bgnflag != BUSY) )    /* �Ѵ���λ�,maxbun�Ϻ��㣱�Ǥ��� */
		cx = (RkcContext *)NULL ;
    }
    return( cx ) ;
}

static char *
FindLogname()
{
    char *username = NULL ;

    if ( (username = (char *)getenv( "LOGNAME" )) == NULL ) {	
	if( (username = (char *)getenv( "USER" )) == NULL ) {
	    if( (username = (char *)getlogin()) == NULL ) {
		struct passwd *pass = getpwuid( (int)getuid() ) ;
		if( pass )
		    username = pass->pw_name ;
	    }
	}
    }
    return( username ) ;
}

int
RkInitialize( hostname ) /* �Ȥꤢ��������ξ��ϡ���������̵�뤹�� */
char *hostname ;
{
    register int    i, server ;
    register RkcContext *cx ;
    char *username, *data ;

    if( rkc_call_flag == BUSY )
	return ( 0 ) ;	

    bzero( ConnectIrohaServerName, MAX_HOSTNAME ) ;
    if( hostname ) {
	if( hostname[0] == '/' || (strlen( hostname ) == 0) )
	    ConnectIrohaServerName[0] = '\0' ;
	else
	    strncpy( ConnectIrohaServerName, hostname, MAX_HOSTNAME ) ;
    } else {
	ConnectIrohaServerName[0] = '\0' ;
    }	

    if( rkc_Connect_Iroha_Server( ConnectIrohaServerName ) < 0 ) {
	errno = EPIPE ;
	return( -1 ) ;
    }

    username = FindLogname() ;

    if( !(data = (char *)malloc( strlen(username) + strlen(VERSION)+2 ) ) )
	    return( -1 ) ;

    strcpy( data, VERSION ) ;
    strcat( data, ":" ) ;
    strcat( data, username ) ;

    /* ����ƥ����Ȥ��������� */
    for( i=0; i < MAX_CX; i++)
	CX[ i ] = (RkcContext *)NULL ;

    /* ����ƥ����Ȥ�������� */
    if( (cx = newCC()) == (RkcContext *)NULL ) {
        free(data);
	return( -1 ) ;
    }

    /* �����Ф˽�������׵ᤷ�������ФΥ���ƥ����Ȥ�������� */
    if( (server = rkc_initialize( data )) < 0 ) {
	/* ���˥���ƥ����Ȥ���ݤ��Ƥ���Τǡ������������� */
	if( server == RETURN_VERSION_ERROR_STAT ){
	    if( rkc_Connect_Iroha_Server( ConnectIrohaServerName ) < 0 ) {
		freeCC( cx->client ) ;
		free(data);
		errno = EPIPE ;
		return( -1 ) ;
	    }
	    strcpy( data, "1.0:" ) ;	/* Version 1.0 �б� */
	    strcat( data, username ) ;
	    if( (server = rkc_initialize( data )) < 0 ) {
		freeCC( cx->client ) ;
		free(data);
		return( -1 ) ;
	    }
	} else {
	    freeCC( cx->client ) ;
	    free(data);
	    return( -1 ) ;
	}
    }
    free(data);

    /* �����Ф��������������ƥ������ֹ������� */
    ServerVersion = (unsigned)(server & 0xffff0000) >> (unsigned)0x10 ;
    cx->server = server & 0x0000ffff ;
    rkc_call_flag = BUSY ;
    return( cx->client ) ;
}

void
RkFinalize()
{
    register int i ;

    if( rkc_call_flag != BUSY )
	return	;		   /* �꥿�����ͤ� ? */

    /* ������ƥ����Ȥ��������
     *	    �Ѵ���Υ���ƥ����ȤϤɤ�����Τ� ?
     */
    for( i = 0; i < MAX_CX; i++ ){
	if( CX[ i ] ) {
	    freeCC( i ) ;
	}
    }	

    rkc_finalize() ;

    ServerVersion = 0 ;
    rkc_call_flag = 0 ;
}

int
RkCloseContext(cxnum)
int cxnum ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !cx || (rkc_call_flag != BUSY) )
	return( -1 ) ;

    if( rkc_close_context( cx ) == -1 )
	return( -1 ) ;

    freeCC( cxnum ) ;
    return( 0 );
}

int
RkCreateContext()
{
    register int    server ;
    register RkcContext *cx ;

    if( rkc_call_flag != BUSY )
	return( -1 ) ;

    /* ����ƥ����Ȥ�������� */
    if( (cx = newCC()) == (RkcContext *)NULL )
	return( -1 ) ;

    if( (server = rkc_create_context()) == -1 ) {
	/* ���˥���ƥ����Ȥ���ݤ��Ƥ���Τǡ������������� */
	freeCC( cx->client ) ;
	return( -1 ) ;
    }

    cx->server = server ;
    return( cx->client ) ;
}

int
RkDuplicateContext( src_cx )
int src_cx ;
{
    register RkcContext *cx_dest, *cx_src = getCC( src_cx, NOCHECK ) ;
    register int	dest_cx ;

    if( !cx_src || (rkc_call_flag != BUSY) )
	return( -1 ) ;

    /* ����ƥ����Ȥ�������� */
    if( (cx_dest = newCC()) == (RkcContext *)NULL )
	return( -1 ) ;

    if( (dest_cx = rkc_duplicate_context( cx_src )) == -1 ) {
	/* ���˥���ƥ����Ȥ���ݤ��Ƥ���Τǡ������������� */
	freeCC( cx_dest->client ) ;
	return( -1 ) ;
    }

    cx_dest->server = dest_cx ;
    return( cx_dest->client ) ;
}

int
RkGetDicList(cxnum, dicnames, max)
int cxnum ;
char *dicnames ;
int max ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !cx )
	return( -1 ) ;

    if( !max  ) 
	return( 0 ) ;
    else if( !dicnames ) {
	char buffer[BUFSIZE] ;

	return( rkc_dictionary_list( cx, buffer, BUFSIZE ) ) ;
    }
    return( rkc_dictionary_list( cx, dicnames, max) ) ;
}

int
RkDefineDic( cxnum, dicname, wordrec )	/* ñ����Ͽ */
int cxnum ;
unsigned char *dicname ;
unsigned char *wordrec ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !wordrec || !cx )
	return( -1 ) ;

    return( rkc_define_dic( cx, dicname, wordrec) ) ;
}

int
RkDeleteDic( cxnum, dicname, wordrec )	  /* ñ���� */
int cxnum ;
unsigned char *dicname ;
unsigned char *wordrec ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !wordrec || !cx )
	return( -1 ) ;

    return( rkc_delete_dic( cx, dicname, wordrec) ) ;
}

int
RkMountDic(cxnum, dicname, mode)
int cxnum, mode ;
unsigned char *dicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !cx )
	return( -1 ) ;

    return( rkc_mount_dictionary( cx, dicname, mode )) ;
}

int
RkRemountDic(cxnum, dicname, where)
int cxnum, where  ;
unsigned char *dicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !cx )
	return( -1 ) ;

    return( rkc_remount_dictionary( cx, dicname, where ) ) ;
}

int
RkUnmountDic(cxnum, dicname)
int cxnum ;
unsigned char *dicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !cx )
	return( -1 ) ;

    return( rkc_umount_dictionary( cx, dicname )) ;
}

int
RkGetMountList(cxnum, dicnames_return, max)
int cxnum, max ;
unsigned char *dicnames_return ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !cx )
	return( -1 ) ;

    if( !dicnames_return ) {
	char buffer[BUFSIZE] ;

	return( rkc_mount_list( cx, buffer, BUFSIZE ) ) ;
    } else if( !max )
	return( 0 ) ;

    return(rkc_mount_list( cx, dicnames_return, max )) ;
}

int
RkSetDicPath( cxnum, path ) /* �������ѥ������� */
int cxnum ;
char *path ;
{
    /*
     * �����Ф�Ω���夬��Ȥ������ꤹ��Τ�RKC�Ǥϲ��⤻�����֤�
     */
    return( 0 ) ;
}

int
RkGetDirList( cxnum, ddname, maxddname )   /* ����ꥹ�Ȥ���� */
int cxnum ;
unsigned char *ddname ;
int maxddname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !ddname || !cx || !maxddname )
	return( -1 ) ;

    return( rkc_get_dir_list( cx, ddname, maxddname ) ) ;
}

/*
 * StoreFirstKouho
 *	������ʸ�ᤫ��ǽ�ʸ��ޤǤ���Ƭ������ᡢ��Ǽ����
 */
static void
StoreFirstKouho( cx, bun_max )
register RkcContext *cx ;
int bun_max ;
{
    /* �����ϡ��褯�ͤ��ʤ��ȹ�¤�ޤ��Ѥ�����̣��̵���ʤ뤷
     *	 ��Ǥ��餤�ܤ����������Τ�ʤ�
     */
    register	int		i ;
    register	RkcBun		*bun ;
    register	unsigned char	*kouhobuf ;

    /* ������ʸ�ᤫ��ǽ�ʸ��ޤǤθ����������� */	
    freeBUN( cx, cx->curbun ) ; 

    /* �����ˤϡ�rkc_*(rkcConvert.c)���������󤬳�Ǽ����Ƥ��� */
    kouhobuf = cx->Fkouho ;

    /* ����ʸ�ᤫ��ǽ�ʸ��ޤǤ�������Υݥ��󥿤����ꤹ�� */
    for( i = 0; i < bun_max; i++ ) {
	bun = &cx->bun[ i ] ;
	/* ������ʸ��ޤǤ�ʸ��Ǹ����������˼������Ƥ���ʸ��ϡ�
	 * �ݥ��󥿤κ�����Ϥ��ʤ���
	 */
	if( bun->flags != NUMBER_KOUHO ) {
	    bun->kanji = kouhobuf ;
	    bun->curcand = 0 ;			/*  ʸ��0ʸ��1ʸ��2ʸ��3ʸ��4@@ */
	    bun->maxcand = 1 ;			/*  ��	 ��   ��   ��	��	*/
	    bun->flags = FIRST_KOUHO ;		/*	bun->kaji		*/
	}
	kouhobuf += strlen( (char *)kouhobuf)+1 ;
    }
    cx->maxbun = bun_max ;
}

/*
 *    Ϣʸ���Ѵ�����
 */
int	
RkBgnBun(cxnum, yomi, maxyomi, kouhomode)
int cxnum, maxyomi, kouhomode ; 
unsigned char *yomi ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;
    int 		nbun ;

    if( (maxyomi <= 0) || !cx )
	return( -1 ) ;

    /* RkBgnBun�����ϡ�BUSY�ե饰��Ω�äƤ�������äƤϤ����ʤ� */
    if( cx->bgnflag == BUSY )
	return( -1 ) ;

    /* maxyomi��ʸ��������������Ȥ��Ƥ��� */
    /* ����ʾ��ʸ�����¸�ߤ��ʤ��Ϥ� */
    if( !(cx->bun = (RkcBun *)calloc( maxyomi, sizeof(RkcBun) )) )
	return( -1 ) ;

    cx->curbun = cx->maxbun = 0 ;
    nbun = rkc_convert( cx, yomi, maxyomi, kouhomode ) ;
    if( nbun <= 0 ) {				
	RkcFree( (char *)cx->bun ) ;
	cx->bun = (RkcBun *)NULL ;
	return( -1 ) ;
    }
    StoreFirstKouho( cx, nbun ) ;
    cx->bgnflag = BUSY ;
    return( nbun ) ;
}

int
RkEndBun( cxnum, mode )
int cxnum, mode ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    int ret ;

    if( cx ) {
	/*
	 * rkc_convert_end�ǳؽ����٤�����򥵡��Ф��Τ餻��
	 */
	if( (ret = rkc_convert_end( cx, mode )) >= 0 ) {
	    freeBUN( cx, 0 ) ;
	    RkcFree( (char *)cx->bun ) ;
	    RkcFree( (char *)cx->Fkouho ) ;
	    cx->bun = (RkcBun *)NULL ;
	    cx->Fkouho = (unsigned char *)NULL ;
	    cx->curbun = cx->maxbun = 0 ;
	    cx->bgnflag = 0 ;
	}
	return( ret ) ;
    }

    return( 0 ) ;
}

/* LoadKouho
 *	ɬ�פ˱�������������ɤ߽Ф�
 */
static
int
LoadKouho( cx )
register RkcContext	 *cx ;
{
    register RkcBun	*bun = &cx->bun[ cx->curbun ] ;
    int 		ret ;

    if( bun->flags == FIRST_KOUHO ) {
	/*	����������ɤ߽Ф���
	 *	�ɤ߽Ф��˼��Ԥ����顢��Ƭ���䤷���ʤ��դ�򤹤�
	 */
	if( (ret = rkc_get_kanji_list( cx )) >= 0 ){
	    /* �ɤߤ������� */
	    bun->curcand = 0 ;
	    bun->maxcand = ret ;
	} else if( errno == EPIPE )
	    return( -1 ) ;

	bun->flags = NUMBER_KOUHO ;
    }
    return( 0 ) ;
}

RkXfer(cxnum, knum)
int cxnum, knum ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    register RkcBun	 *bun ;

    if( cx ) {
	bun = &cx->bun[cx->curbun];
	if( LoadKouho( cx ) < 0 )
	    return( -1 ) ;
	if ( 0 <= knum && knum < bun->maxcand ) 
	    bun->curcand = knum;
	return( bun->curcand );
    }
    return( 0 );
}

int
RkNfer(cxnum)
int cxnum ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    register RkcBun	 *bun ;

    if( cx ) {
	bun = &cx->bun[ cx->curbun ];	
	if( LoadKouho( cx ) < 0 )
	    return( -1 ) ;
	bun->curcand = bun->maxcand ; /* �ɤߤϡ��Ǹ�ˤ��� */
	return( bun->curcand ) ;
    }
    return( 0 );
}

int
RkNext(cxnum)
int cxnum ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    register RkcBun	 *bun ;

    if( cx ) {
	bun = &cx->bun[ cx->curbun ];	
	if( LoadKouho( cx ) < 0 )
	    return( -1 ) ;
	if ( ++bun->curcand > bun->maxcand-1 )
	    bun->curcand = 0;
	return( bun->curcand ) ;
    }
    return( 0 ) ;
}

int
RkPrev(cxnum)
int cxnum ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    register RkcBun	 *bun ;

    if( cx ) {
	bun = &cx->bun[ cx->curbun ];
	if( LoadKouho( cx ) < 0 )
	    return( -1 ) ;
	if ( --bun->curcand < 0 )
	    bun->curcand = bun->maxcand-1 ; /* �����ꥸ�� */
	return( bun->curcand ) ;
    }
    return( 0 );
}

static
unsigned char	*
SeekKouho( bun, to )
register RkcBun     *bun ;
register int	    to ;
{
    register int    i ;
    unsigned char   *src_yomi ;

    src_yomi = bun->kanji ;
    for( i = 0; i < to; i++ )
	src_yomi += strlen( (char *)src_yomi ) + 1 ;

    return( src_yomi ) ;
}

int
RkGetKanji( cxnum, kanji, maxkanji )
int cxnum, maxkanji ;
unsigned char *kanji ;
{
    RkcContext		    *cx = getCC( cxnum, CHECK ) ;
    RkcBun		    *bun ;
    register unsigned char  *src_kouho ;

    if( cx ){
	bun = &cx->bun[ cx->curbun ] ;
			     /* �ɤߤ����ʤ������ɤߤ��֤� */
	src_kouho = SeekKouho( bun, (bun->maxcand ? bun->curcand : 1) ) ;
	if( strlen((char *)src_kouho ) > maxkanji )
	    return( 0 ) ;
	if( kanji )
	    strcpy( kanji, src_kouho ) ;
	return( strlen( (char *)src_kouho ) ) ;
    }
    return( -1 ) ;
}

int
RkGetKanjiList(cxnum, kouho, max)
int cxnum, max ;
unsigned char *kouho ;
{
    RkcContext	*cx = getCC( cxnum, CHECK ) ;
    RkcBun	*bun ;
    register unsigned char  *dest_kouho, *src_kouho ;
    register int i, len ;
    int total ;

    if( cx ) {
	bun = &cx->bun[ cx->curbun ];
	if( LoadKouho( cx ) < 0 )
	    return( -1 ) ;
	if( !max )
	    return( 0 ) ;
	if( !kouho )
	    return( bun->maxcand ? bun->maxcand : 1 ) ;
	/* ����򥳥ԡ����� */
	src_kouho = bun->kanji ;
	dest_kouho = kouho ;
	for( total = strlen((char *)src_kouho)+1, i = 0;
		  (i < bun->maxcand) && (total < max) ; i++, total += len ) {
	    strcpy( dest_kouho, src_kouho ) ;
	    len = strlen( (char *)src_kouho ) + 1 ;
	    src_kouho += len ;
	    dest_kouho += len ;
	}
	*(dest_kouho++) = '\0' ;
	*(dest_kouho) = '\0' ;
	return( i ) ;
    }
    return( -1 ) ;
}


int
RkGoTo(cxnum, bnum)
int cxnum, bnum ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;

    if( cx ){
	if ( 0 <= bnum && bnum < cx->maxbun )
	     cx->curbun = bnum;
	return(cx->curbun);
    }
    return( 0 );
}

int
RkLeft(cxnum)
int cxnum;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;

    if( cx ){
	if ( --cx->curbun < 0 )
	     cx->curbun = cx->maxbun-1 ;
	return( cx->curbun );
    }
    return( 0 );
}

int
RkRight(cxnum)
int cxnum;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;

    if( cx ){
	if ( ++cx->curbun > cx->maxbun-1 )
	     cx->curbun = 0;
	return( cx->curbun );
    }
    return( 0 );
}

#define ENLARGE     -1
#define SHORTEN     -2
#define MIN_YOMI     2

static int
RKReSize( cxnum, len )
int cxnum, len ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    int ret ;		/* ��ʸ��� */

    if( cx ) {	
	/* ʸ��Ĺ���ѹ��Ǥ��뤫�ɤ��������å����� */
	register RkcBun *bun = &cx->bun[ cx->curbun ] ;

	/* ������ʸ�᤬�����������ɤ߹���Ǥ�����ν��� */
	if( bun->flags == NUMBER_KOUHO ) {
	     /* ʸ��̤�ǥ������ɤߤ�Ĺ����MIN_YOMI�ξ��ϲ��⤻���� */
	     /* ������ʸ������֤� */
	    int yomilen = strlen( (char *)SeekKouho( bun, (bun->maxcand ?
						       bun->maxcand : 1) ) ) ;

	    if( (len == SHORTEN) || (len == MIN_YOMI) ) {
		if( yomilen == MIN_YOMI )
		    return( cx->maxbun ) ;
	    } else {
		int curbun_save = cx->curbun ;
		int yomi_zan ;

		for( yomi_zan = 0; cx->curbun < cx->maxbun; cx->curbun++ ) {
		    RkStat stat ;
		
		    if( rkc_get_stat( cx, &stat ) < 0 )
			return( -1 ) ;
		    yomi_zan += stat.ylen ;
		}
		cx->curbun = curbun_save ;
		if( ((len == ENLARGE) && (yomilen+2 > yomi_zan))
						      || (yomi_zan < len) )
		    return( cx->maxbun ) ;
	    }
	}
	/* ��ʸ�ᤫ��ǽ�ʸ��ޤǳ�Ǽ����� */	
	if( (ret = rkc_resize( cx, len )) <= 0 ){
	    return( -1 ) ;
	}
	StoreFirstKouho( cx, ret ) ;
	return( ret ) ;
    }
    return( 0 ) ;
}

int
RkResize(cxnum, len)
int cxnum, len ;
{
    if( len <= 0 ) {
	register RkcContext  *cx = getCC( cxnum, CHECK ) ;

	if( cx )
	    return( cx->maxbun ) ;
	else
	    return( 0 ) ;
    }

    return( RKReSize( cxnum, len ) ) ;
}

int
RkEnlarge( cxnum )  /* ʸ�῭�Ф� */
int cxnum ;
{
    return( RKReSize( cxnum, ENLARGE  ) ) ;
}

int
RkShorten(cxnum)    /* ʸ��̤� */
int cxnum ;
{
    return( RKReSize( cxnum, SHORTEN ) ) ;
}

int
RkStoreYomi(cxnum, yomi, max)
int cxnum, max ;
char *yomi ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    int ret ;

    ret = 0 ;
    if( cx && yomi ) {
	if( (ret =  rkc_store_yomi( cx, yomi, max)) < 0 ) {
	    return( -1 ) ;
	}
	StoreFirstKouho( cx, ret ) ;
    }
    return( ret ) ;
}

int
RkGetYomi(cxnum, yomi, maxyomi)
int	cxnum, maxyomi ;
unsigned char	*yomi;
{
    register RkcContext *cx = getCC( cxnum, CHECK ) ;
    register RkcBun	*bun ;
    unsigned char	*src_yomi ;

    if( cx ){
	bun = &cx->bun[ cx->curbun ] ;		
	if( ServerVersion == 0 ) {  /* Ver 1.0 �Ǥϡ���ꤢ��������������äƤ��� */
	    if( LoadKouho( cx ) < 0 )
		return( -1 ) ;
	}
	if( bun->flags == NUMBER_KOUHO ) {
	    src_yomi = SeekKouho( bun, (bun->maxcand ? bun->maxcand : 1) ) ;
	} else {
	    if( rkc_get_yomi( cx, &src_yomi ) < 0 )
		return( -1 ) ;
	}

	if( strlen((char *)src_yomi) > maxyomi )
	    return( 0 ) ;

	if( yomi )
	    strcpy( yomi, src_yomi ) ;
	return( strlen( (char *)src_yomi ) ) ;
    }
    return( -1 ) ;
}

int
RkGetLex(cxnum, lex, maxlex)
int cxnum, maxlex ;
RkLex *lex ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    int ret = -1 ;

    if( cx ) {
	RkLex  buf[1024] ;

	if( !maxlex )
	    return( 0 ) ;
	if( lex )
	    ret = rkc_get_lex( cx, maxlex, lex) ;
	else
	    ret = rkc_get_lex( cx, 1024, buf) ; 
    }
    return( ret ) ;
}

int
RkGetStat(cxnum, stat)
int cxnum ;
RkStat *stat ;
{
    register RkcContext  *cx = getCC( cxnum, CHECK ) ;
    int ret ;

    if( cx && stat ){
	ret = rkc_get_stat( cx, stat ) ;
	if( ServerVersion == 0 ) {
	    int p[ 7 ] ;
	    register int tmp1, tmp2 ;
	    int i ;

	    bcopy( stat, p, sizeof( RkStat ) ) ;
	    tmp1 = p[ 5 ];
	    tmp2 = p[ 6 ] ;
	    for( i = 4; i > 1; i-- )
		p[ i + 2 ] = p[ i ] ;
	    p[ 2 ] = tmp1 ;
	    p[ 3 ] = tmp2 ;
	    bcopy( p, stat, sizeof( RkStat ) ) ;
	}
    } else {
	ret = -1 ;
    }

    return( ret ) ;
}

char *
RkGetServerName()
{
    return( ConnectIrohaServerName ) ;
}


#ifdef EXTENSION
static
CheckRemoteToolProtoVersion()
{
    if( ServerVersion < 2 )	/* protocol version 1.2 */
	return( -1 ) ;
    else
	return( 0 ) ;
}

int
RkListDic( cxnum, dirname, dicnames_return, size )
int cxnum, size ;
unsigned char *dirname, *dicnames_return ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !cx || CheckRemoteToolProtoVersion() )
	return( -1 ) ;

    if( !dirname )
	if( !(dirname = (unsigned char *)FindLogname()) )
	    return( -1 ) ;

    if( !dicnames_return ) {
	unsigned char buffer[ MAXDATA ] ;

	return( rkc_list_dictionary( cx, dirname, buffer, MAXDATA ) ) ;
    } else if( !size )
	return( 0 ) ;

    return( rkc_list_dictionary( cx, dirname, dicnames_return, size ) ) ;
}

RkCreateDic( cxnum, dicname, mode )
int cxnum, mode ;
unsigned char *dicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || CheckRemoteToolProtoVersion() )
	return( -1 ) ;

    return( rkc_create_dictionary( cx, dicname, mode ) ) ;
}

RkRemoveDic( cxnum, dicname )
int cxnum ;
unsigned char *dicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || CheckRemoteToolProtoVersion() )
	return( -1 ) ;

    return( rkc_destroy_dictionary( cx, dicname ) ) ;
}

RkRenameDic( cxnum, dicname, newdicname, mode )
int cxnum, mode ;
unsigned char *dicname, *newdicname ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dicname || !newdicname || CheckRemoteToolProtoVersion() )
	return( -1 ) ;

    return( rkc_rename_dictionary( cx, dicname, newdicname, mode ) ) ;
}

RkGetWordTextDic( cxnum, dirname, dicname, info, infolen )
int cxnum, infolen ;
unsigned char *dirname, *dicname, *info ;
{
    register RkcContext *cx = getCC( cxnum, NOCHECK ) ;

    if( !dirname || CheckRemoteToolProtoVersion() )
	return( -1 ) ;

    return( rkc_get_text_dictionary( cx, dirname, dicname, info, infolen ) ) ;	
				
}

#endif /* EXTENSION */
