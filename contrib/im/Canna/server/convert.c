static char sccs_id[]="@(#) NEC UNIX( PC-UX/EWS-UX ) convert.c 1.10 90/11/29 18:07:15";
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/server/RCS/convert.c,v 1.13 91/07/22 20:03:35 kon Exp $";

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

/* LINTLIBRARY */

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>

#include "net.h"
#include "IR.h"

#define DDPATH	"iroha"

#define ACK0 0
#define ACK1 1
#define ACK2 2
#define ACK3 3
#define CHECK_ACK_BUF_SIZE	(ACK_BUFSIZE + sizeof( int )*2 )

static IRReq	Request ;
static IRAck	Acknowledge ;
static unsigned char
local_buffer[ LOCAL_BUFSIZE ],
local_buffer2[ LOCAL_BUFSIZE ] ;

unsigned int
TotalRequestTypeCount[ MAXREQUESTNO ] ;
static int iroha_server_hi = 0 ;
static int iroha_server_lo = 0 ;

getserver_version()
{
    char version[ 32 ], *buf ;

    strcpy( version, VERSION ) ;
    if( version[0] ) {
	if( !(buf = (char *)strtok(version, ".")) ) {
	    return ;
	}
	iroha_server_hi = atoi( buf ) ;
	if( !(buf = (char *)strtok((char *)NULL, ".")) ) {
	    return ;
	}
	iroha_server_lo = atoi( buf ) ;
    }
}

static
GetFirstKouho( cxnum, start, end, ack )
int cxnum, start, end ;
Ack1 *ack ;
{
    unsigned char *src = local_buffer2 ;
    register unsigned char *dst = ack->data ;
    register int i, len ;

   ir_debug( Dmsg(5,"最優先候補リスト\n" ); )
    for( i = start; i < end; i++){
	len = RkGetKanji( cxnum, src, LOCAL_BUFSIZE  ) + 1 ;
	if( dst + len + sizeof( long ) - ack->data  > CHECK_ACK_BUF_SIZE ) {
	    PrintMsg( "Acknowledge buffer(fast kouho) over flow!!\n" ) ;
	    break ;
	}
	LENTODATA( len, dst ) ;
	dst += sizeof( long ) ;
	strcpy( dst, src );
       ir_debug( Dmsg(5,"%d:[%s]", i, dst ); )
	dst += len ;
	RkRight( cxnum ) ;
    }
   ir_debug( Dmsg(5,"\n" ); )
    ack->len = htonl( dst - ack->data ) ;
    ack->stat = htonl( i ) ;
    RkGoTo( cxnum, start ) ;/* 先頭文節をカレント文節に戻しておく */
    return( dst - ack->data ) ;
}

static
MakeList( src, ack, cnt )
unsigned char *src ;
Ack1 *ack ;
register int cnt ;
{
    unsigned char *dst = ack->data ;
    register int i, len ;

    for( i = 0; i < cnt; i++ ){
	len = strlen( (char *)src ) + 1 ;
	if( dst + len + sizeof( long ) - ack->data  > CHECK_ACK_BUF_SIZE ) {
	    PrintMsg( "Acknowledge buffer(fast kouho) over flow!!\n" ) ;
	    break ;
	}
	LENTODATA( len, dst ) ;
	dst += sizeof( long ) ;
	strcpy( dst, src ) ;
       ir_debug( Dmsg(5,"%d:[%s] ", i, dst ); )
	dst += len ;
	src += len ;
    }
   ir_debug( Dmsg(5,"\n" ); )
    return( dst - ack->data ) ;
}

ir_error(client)
ClientPtr client ;
{
    unsigned char *buf = local_buffer ;
    int size ;
	
    while( (size = read( client->id, buf, LOCAL_BUFSIZE )) == LOCAL_BUFSIZE ) {
#ifdef DEBUG
	Dmsg(5,"ごみデータ\n")	;
	DebugDump( 5, buf, size );
#endif
    }
    return( -1 ) ;
}

static int
CheckVersion( req, client )
Req2 *req ;
ClientPtr client ;
{
    unsigned char *data = req->name, *logname ;
    int clienthi, clientlo ;
    char *buf ;

    if( !(buf = (char *)strtok( (char *)data, "." )) )
	return( -1 ) ;
    clienthi = atoi( buf ) ;

    if( !(buf = (char *)strtok((char *)NULL, ":")) )
	return( -1 ) ;
    clientlo = atoi( buf ) ;

    if( !(logname = (unsigned char *)strtok( (char *)NULL, ":" )) )
	return( -1 ) ;

    strcpy( req->name, logname ) ;

   ir_debug( Dmsg( 5,"UserName:[%s]\n", req->name ); )
   ir_debug( Dmsg( 5,"client:hi[%d],lo[%d]\n", clienthi, clientlo ); )
   ir_debug( Dmsg( 5,"server:hi[%d],lo[%d]\n", iroha_server_hi, iroha_server_lo ); )
    if( iroha_server_hi != clienthi )
	return( RETURN_VERSION_ERROR_STAT ) ;

    client->version = clientlo ;
    return( ( clientlo ? iroha_server_lo : 0 ) ) ;
}

static int
SetDicHome( client, cxnum )
ClientPtr client ;
int cxnum ;
{
    unsigned char dichome[ 256 ] ;

    if( cxnum < 0 || cxnum > MAX_CX )
	return( -1 ) ;

    if( client->username && ( strlen( client->username ) > 0 ) )
	sprintf( dichome, "%s:%s", client->username, DDPATH ) ;
    else
	strcpy( dichome, DDPATH ) ;

   ir_debug( Dmsg(5,"辞書ホームディレクトリィ：%s\n", dichome ); )
    if( RkSetDicPath( cxnum, dichome ) == -1 ) {
	RkCloseContext( cxnum ) ;
	return( -1 ) ;
    }
    SETCXT( cxnum ) ;
    return( 1 ) ;
}

ir_initialize(client)
ClientPtr client ;
{
    extern CheckAccessControlList() ;
    extern char *ProtoName[] ;
    Req2 *req = &Request.Request2 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, i, ret ;

    ack->stat = htonl( -1 ) ;
    /* プロトコルバージョンのチェック */
    if( (ret = CheckVersion( req, client )) < 0 ) {
	if( WriteToClient( client, ACK0 ) < 0 ) {
	    return( -1 ) ;
	}
	CloseDownClient( client ) ;
	return( 0 ) ;
    }

    if( client->username = (char *)malloc( strlen( req->name ) + 1 ) )
	strcpy( client->username, req->name ) ;
    else
	client->username = (char *)NULL ;

    for( i = 0; i < MAX_CX; i++ )
	OFFCXT( i ) ;

    if( ConnectionSetup( client ) > 0) {
	if( CheckAccessControlList( (strpbrk(client->hostname, "(UNIX)")
				? "unix" : client->hostname),
						  client->username ) < 0 ) {	
	    if( WriteToClient( client, ACK0 ) < 0 )
		return( -1 ) ;

	    PrintMsg("[%s](%s) Can't find access control list\n", client->username, ProtoName[ req->Type -1 ] ) ;
	    CloseDownClient( client ) ;
	    return( 0 ) ;
	}
	cxnum = RkCreateContext() ;
	if( cxnum >= 0 ) {
	    if( SetDicHome( client, cxnum ) > 0 )
		ack->stat = htonl( (ret << 0x10) | cxnum ) ;
	}
	
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_finalize(client)
register ClientPtr client ;
{
    Ack0 *ack = &Acknowledge.Anck0 ;

    ack->stat = htonl( 0 ) ;

    if( WriteToClient( client, ACK0 ) < 0 ) {
	return( -1 ) ;
    }

    /* close処理＆後始末（コンテクストの開放等） */
    CloseDownClient( client ) ;
    return( 0 ) ;
}

ir_create_context(client)
ClientPtr client ;
{
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum ;

    cxnum = RkCreateContext() ;
    if( SetDicHome( client, cxnum ) > 0 ) {
	ack->stat = htonl( cxnum ) ;
    } else {	
	extern char *ProtoName[] ;
	Req0 *req0 = &Request.Request0 ;

	ack->stat =  htonl( -1 ) ;
	PrintMsg("[%s](%s) Can't set dictionary home\n", client->username, ProtoName[ req0->Type -1 ] ) ;
    }
    return( WriteToClient( client, ACK0 ) ) ;
}

ir_duplicate_context(client)
ClientPtr client ;
{
    Req1 *req = &Request.Request1 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum ;

    if( CHECKCXT( cxnum = req->context ) ) {
	ack->stat = htonl(cxnum = RkDuplicateContext( cxnum )) ;
	if( cxnum >= 0 && cxnum < MAX_CX )
	    SETCXT( cxnum ) ;
    } else {
	extern char *ProtoName[] ;

	ack->stat = htonl( -1 ) ;
	PrintMsg("[%s](%s) Context Err[%d]\n", client->username, cxnum, ProtoName[ req->Type - 1 ] ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_close_context(client)
ClientPtr client ;
{
    Req1 *req = &Request.Request1 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum ;

    if( CHECKCXT( cxnum = req->context ) ) {
	ack->stat = htonl( RkCloseContext(cxnum) ) ;
	OFFCXT( cxnum ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_dictionary_list(client)
ClientPtr client ;
{
    Req3 *req = &Request.Request3 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    unsigned char *dicnames = local_buffer ;
    int cxnum, size = 0 ;
    int ret, max ;

    if( CHECKCXT( cxnum = req->context ) ) {
	max = MIN( req->number, LOCAL_BUFSIZE ) ;
	ret = (int)RkGetDicList( cxnum, dicnames, max ) ;
	
	ack->stat = htonl( ret ) ;
	if( ret < 0 ) {
	    ack->len = htonl( 0 ) ;
	} else {
	   ir_debug( Dmsg(5,"辞書リスト\n" ); )
	    ack->len = htonl( size = MakeList( dicnames, ack, ret ) ) ;
	}
    } else {
	extern char *ProtoName[] ;

	ack->stat = htonl( -1 ) ;
	PrintMsg("[%s](%s) Context Err[%d]\n", client->username, cxnum, ProtoName[ req->Type - 1 ] ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_get_yomi( client )
ClientPtr client ;
{
    Req5 *req = &Request.Request5 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    unsigned char *yomi = local_buffer ;
    int ret, cxnum ;
    int size = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	int bunsetuno = req->number ;
	int maxyomi = MIN( req->datalen, LOCAL_BUFSIZE ) ;

       ir_debug( Dmsg(5,"maxyomi [ %d ]\n", maxyomi ); )
	if( RkGoTo(cxnum, bunsetuno) == bunsetuno ) {
	    ret = RkGetYomi( cxnum, yomi, maxyomi ) ;
	    ack->stat = htonl( ret ) ;		
	    if( ret < 0 )
		ack->len = htonl( 0 ) ;
	    else {
		int datalen = strlen( yomi ) + 1 ;

		LENTODATA( datalen, ack->data ) ;
		strcpy( ack->data + sizeof( int ), yomi ) ;
		ack->len = htonl( size = sizeof( int ) + datalen )  ;
	    }
	} else {
	    extern char *ProtoName[] ;

	    PrintMsg("[%s](%s) bunsetu move failed\n", client->username, ProtoName[ req->Type - 1 ] ) ;
	    ack->stat = htonl( ret ) ;
	    ack->len = htonl( 0 ) ;
       }
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_set_dic_path( client )
ClientPtr client ;
{
    return( 0 ) ;
}

ir_define_dic(client)
ClientPtr client ;
{
    Req7 *req = &Request.Request7 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    unsigned char *dicname, *data ;
    int cxnum, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
	dicname = req->dicname ;
	data = req->datainfo ;
       ir_debug( Dmsg(5,"辞書名=%s\n", dicname ); )
       ir_debug( Dmsg(5,"登録するデータ[%s]\n", data );)
	ret = RkDefineDic( cxnum, dicname, data ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( -1 ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_delete_dic(client)
ClientPtr client ;
{
    Req7 *req = &Request.Request7 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    unsigned char *dicname, *data ;
    int cxnum, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
	dicname = req->dicname ;
	data = req->datainfo ;
       ir_debug( Dmsg(5,"辞書名=%s\n", dicname ); )
       ir_debug( Dmsg(5,"削除するデータ[%s]\n", data ); )
	ret = RkDeleteDic( cxnum, dicname, data ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( -1 ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_get_dir_list(client)
ClientPtr client ;
{
    Ack1 *ack = &Acknowledge.Anck1 ;
    Req3 *req = &Request.Request3 ;
    unsigned char *dicnames = local_buffer ;
    int cxnum, ret, max ;
    int size = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	max = MIN( req->number, LOCAL_BUFSIZE ) ;
	
	ret = RkGetDirList( cxnum, dicnames, max) ;
	ack->stat = htonl( ret ) ;
	if( ret < 0 ) {
	    ack->len = htonl( 0 ) ;
	} else {
	   ir_debug( Dmsg(5,"辞書リスト\n" ); )
	    ack->len = htonl( size = MakeList( dicnames, ack, ret ) ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }	
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_mount_dictionary(client)
ClientPtr client ;
{
    Ack0 *ack = &Acknowledge.Anck0 ;
    Req8 *req = &Request.Request8 ;
    unsigned char *dicname ;
    int cxnum, mode, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
	mode = req->mode ;
       ir_debug( Dmsg(5,"dicname = %s\n", req->data ); )
	dicname = req->data ;
	ret = RkMountDic( cxnum, dicname, mode ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( -1 ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_umount_dictionary(client)
ClientPtr client ;
{
    Req8 *req = &Request.Request8 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
       ir_debug( Dmsg(5,"dicname = %s\n", req->data ); )
	ret = RkUnmountDic( cxnum, req->data ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( ret ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}

ir_rmount_dictionary(client)
ClientPtr client ;
{
    Req9 *req = &Request.Request9  ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, where ;	
    int ret = -1 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	where = req->number ;
       ir_debug( Dmsg(5,"dicname = %s\n", req->data ); )
	ret = RkRemountDic( cxnum, req->data, where ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
    }
    ack->stat = htonl( ret ) ;
    return( WriteToClient( client, ACK0 ) ) ;
}

ir_mount_list( client )
ClientPtr client ;
{
    Ack1 *ack = &Acknowledge.Anck1 ;
    Req3 *req = &Request.Request3 ;
    unsigned char *dicnames = local_buffer ;
    int cxnum, ret, size = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	ret = RkGetMountList( cxnum, dicnames, MIN( req->number, LOCAL_BUFSIZE ) ) ;
	ack->stat = htonl( ret ) ;
	if( ret < 0 ) {
	    ack->stat = htonl( 0 ) ;
	} else {
	   ir_debug( Dmsg(5,"辞書リスト\n" ); )
	    ack->len = htonl( size = MakeList( dicnames, ack, ret ) ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }	

    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_convert( client )
ClientPtr client ;
{
    Ack1 *ack = &Acknowledge.Anck1 ;
    Req8 *req = &Request.Request8 ;
    int cxnum, yomilen, ret, mode ;
    int size = 0 ;
    unsigned char *data ;

    if( CHECKCXT( cxnum = req->context ) ) {
	mode = req->mode ;
	yomilen = req->datalen ;
	data = req->data ;
	if( yomilen + 1 <= CHECK_ACK_BUF_SIZE )
	    data[ yomilen + 1 ] = '\0' ;
       ir_debug( Dmsg(5,"読み = %s\n",data ); )
	ret = RkBgnBun( cxnum, data, yomilen, mode ) ;
	if( ret != -1 ) {
	    /* 最優先候補リストを取得する */
	    size = GetFirstKouho( cxnum, 0, ret, ack ) ;
	} else {
	    extern char *ProtoName[] ;

	    PrintMsg( "[%s](%s) kana-kanji convert failed\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	    ack->stat = htonl( -1 ) ;
	    ack->len = 0 ;
	    ack->data[0] = '\0' ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = 0 ;
	ack->data[0] = (unsigned char)NULL ;
    }	
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_convert_end( client )
ClientPtr client ;
{
    Req4 *req = &Request.Request4 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, len, i, mode, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
	len = req->number ;
	if( len ) {
	    mode = 1 ;
	    if( RkGoTo( cxnum, 0 ) != 0 ) {	
		extern char *ProtoName[] ;

		PrintMsg("[%s](%s) ir_convert_end: RkGoTo failed\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	    }
	   ir_debug( Dmsg( 5,"学習させる候補\n" ); )
	    /* カレント候補を先頭に移動クライアントが選んだ候補を */	
	    /* ＲＫに知らせる */		
	    for( i = 0; i < len; i++ ){ 
		if( req->kouho[ i ] != RkXfer( cxnum, req->kouho [ i ] ) ) {
		    extern char *ProtoName[] ;

		    PrintMsg("[%s](%s) ir_convert_end: RkXfer failed\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
		}
	       ir_debug( DebugDispKanji( cxnum, i ); )	
		if( RkRight( cxnum ) == 0 && i != (len - 1) ) { 	
		    extern char *ProtoName[] ;

		    PrintMsg("[%s](%s) ir_convert_end: RkRight failed\n", client->username, ProtoName[ req->Type - 1 ]	 ) ;
		}
	    }
	   ir_debug( Dmsg( 5,"\n" ); )
	} else {
	    mode = 0 ;
	}
	ret = RkEndBun( cxnum, mode ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
    }
    return( WriteToClient( client, ACK0 ) ) ;
}

ir_get_kanjilist( client )
ClientPtr client ;
{
    Req5 *req = &Request.Request5 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    unsigned char *kouho = local_buffer ;
    unsigned char *yomi, *data ;
    int ret = -1, size = 0, cxnum, len	;
    int bunsetuno, maxkanji ;

    if( CHECKCXT( cxnum = req->context ) ) {
	bunsetuno = req->number ;
	maxkanji = MIN( req->datalen, LOCAL_BUFSIZE ) ; 
       ir_debug( Dmsg(5,"maxkanji [ %d ]\n", maxkanji ); )
	if( RkGoTo(cxnum, bunsetuno) == bunsetuno ) {
	    ret = RkGetKanjiList( cxnum, kouho, maxkanji ) ;
	    data = ack->data ;
	    if( ret ) {
	       ir_debug( Dmsg(5,"候補リスト\n" ); )
		data += MakeList( kouho, ack, ret ) ;
	    } else {
		LENTODATA( 1, data ) ;
		data += sizeof( int ) ;
		*data = '\0' ; data++ ;
	    }
	    yomi = data + sizeof( int ) ;	 /* 読みを最後につける*/
	    RkGetYomi( cxnum, yomi, LOCAL_BUFSIZE - (yomi - ack->data) ) ;
	
	    len = strlen( (char *)yomi ) + 1 ;
	    LENTODATA( len, data ) ;
	    data += (sizeof( long ) + len ) ;
	
	    size = data - ack->data ;		       /* データ長 */
	    ack->stat = htonl( ret ) ;		       /* 候補数 */
	    ack->len = htonl( size ) ;	
	} else {
	    extern char *ProtoName[] ;

	    PrintMsg("[%s](%s) bunsetu move failed\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	    ack->stat = htonl( ret ) ;
	    ack->len = htonl( 0 ) ;
       }
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( ret ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_resize(client)
ClientPtr client ;
{
#define ENLARGE -1
#define SHORTEN -2
    Req5 *req = &Request.Request5 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    int ret, cxnum, yomilen, bunsetu ;
    int size = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	bunsetu = req->number ;
	yomilen = req->datalen ;

	RkGoTo(cxnum, bunsetu) ;
       ir_debug( Dmsg(5,"yomilen = %d\n",yomilen ); )
       ir_debug( Dmsg(5,"bunsetu = %d\n",bunsetu ); )
	switch( yomilen ) {
	    case ENLARGE :
		ret = RkEnlarge( cxnum ) ;
		break ;
	    case SHORTEN :
		ret = RkShorten( cxnum ) ;
		break ;
	    default :
		ret = RkResize( cxnum, yomilen ) ;
		break ;
	    }
	/* 最優先候補リストを取得する */
	size = GetFirstKouho( cxnum, bunsetu, ret, ack ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( 0 ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_store_yomi( client )
ClientPtr client ;
{
    Req9 *req = &Request.Request9 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    int cxnum, bunsetu, len, ret ;
    int size = 0 ;
    unsigned char *data ;

    if( CHECKCXT( cxnum = req->context ) ) {
	bunsetu = req->number ;
	len = req->datalen ;

	RkGoTo( cxnum, bunsetu ) ;

	data = req->data ;
	data[ len + 1 ] = '\0' ;
       ir_debug( Dmsg(5,"読み = %s\n",data ); )
	ret = RkStoreYomi( cxnum, data, len ) ;
	if( ret >= 0 ){
	    size = GetFirstKouho( cxnum, bunsetu, ret, ack ) ;
	} else {
	    extern char *ProtoName[] ;

	    PrintMsg("[%s](%s) RkStoreYomi faild\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	    ack->stat = htonl( -1 ) ;
	    ack->len = htonl( 0 ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK1, size ) ) ;
}

ir_query_extension( client )
ClientPtr client ;
{
    extern char *ExtensionName[][2] ;
    Req12 *req = &Request.Request12 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int i = 0 ;
    int status = -1 ;

    while( strlen( ExtensionName[ i ][ 0 ] ) ) {
	if( !strcmp( ExtensionName[ i ][ 0 ], req->data ) ) {
	    status = atoi( ExtensionName[ i ][ 1 ] ) ;
	    break ;
	}
	i++ ;
    }
    ack->stat = htonl( status ) ;

    return( WriteToClient( client, ACK0 ) ) ;
}

#ifdef EXTENSION
ir_list_dictionary( client )
ClientPtr client ;
{
    Req9 *req = &Request.Request9 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    unsigned char *dicnames = local_buffer ;
    unsigned char *dirname ;
    int cxnum, size, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
	size = MIN( req->number, LOCAL_BUFSIZE ) ;
	dirname = req->data ;
	ret = RkListDic( cxnum, dirname, dicnames, size ) ;
	ack->stat = htonl( ret ) ;
	if( ret < 0 ) {
	    ack->stat = htonl( 0 ) ;
	} else {
	   ir_debug( Dmsg(5,"辞書リスト\n" ); )
	    ack->len = htonl( size = MakeList( dicnames, ack, ret ) ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s@%s](%s) Context Err\n", client->username, client->hostname, ProtoName[ req->Type - 1 ]	) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }	

    return( WriteToClient( client, ACK1, size ) ) ;
}


ir_create_dictionary( client )
ClientPtr client ;
{
    Req8 *req = &Request.Request8 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
       ir_debug( Dmsg(5,"dicname = %s\n", req->data ); )
	ret = RkCreateDic( cxnum, req->data, req->mode ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( ret ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}


ir_remove_dictionary( client )
ClientPtr client ;
{
    Req8 *req = &Request.Request8 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum, ret ;

    if( CHECKCXT( cxnum = req->context ) ) {
       ir_debug( Dmsg(5,"dicname = %s\n", req->data ); )
	ret = RkRemoveDic( cxnum, req->data ) ;
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
	ack->stat = htonl( ret ) ;
    }

    return( WriteToClient( client, ACK0 ) ) ;
}


ir_rename_dictionary( client )
ClientPtr client ;
{
    Req10 *req = &Request.Request10 ;
    Ack0 *ack = &Acknowledge.Anck0 ;
    int cxnum ; 
    int ret = -1 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	ret = RkRenameDic( cxnum, req->dicname, req->textdicname, req->mode ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
    }
    ack->stat = htonl( ret ) ;
    return( WriteToClient( client, ACK0 ) ) ;
}


ir_get_word_text_dic( client )
ClientPtr client ;
{
    Req10 *req = &Request.Request10 ;
    Ack1 *ack = &Acknowledge.Anck1 ;
    unsigned char *infobuf = local_buffer ;
    unsigned char *dicname, *dirname ;
    int cxnum, infosize, ret, cnt = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	dirname = ( req->diclen ? req->dicname : (unsigned char *)NULL ) ;
	infosize = MIN( req->mode, MAXDATA ) ;
	dicname = req->textdicname ;
	ret = RkGetWordTextDic( cxnum, dirname, dicname, infobuf, infosize ) ;
	ack->stat = htonl( ret ) ;
	if( ret > 0 ) {
	    unsigned char *wp = ack->data ;

	    cnt = ret + 1 ;
	    LENTODATA( cnt, wp ) ; wp += sizeof( int ) ;
	    bcopy( infobuf, wp, cnt ) ;
	    ack->len = htonl( cnt + sizeof( int ) ) ;
	} else {
	    ack->len = htonl( 0 ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]   ) ;
    }
    return( WriteToClient( client, ACK1, cnt+sizeof( int ) ) ) ;
}
#endif /* EXTENSION */

ir_get_stat( client )
ClientPtr client ;
{
    Req5 *req = &Request.Request5 ;
    Ack2 *ack = &Acknowledge.Anck2 ;
    int cxnum, kouho, bunsetu, ret ;
    int size = 0 ;
    RkStat stat ;

    if( CHECKCXT( cxnum = req->context ) ) {
	bunsetu = req->number ;
	kouho = req->datalen ;

	RkGoTo( cxnum, bunsetu ) ;
	RkXfer( cxnum, kouho ) ;

	ret = RkGetStat( cxnum, &stat ) ;

	if( ret < 0 ) {
	    ack->len = htonl( 0 ) ;
	} else {
	    register int i ;
	    int *dst = ack->info ;
	    int *src = (int *)&stat ;

	    size = sizeof( RkStat ) ;
	    for( i = 0; i < size/sizeof(int); i++ ) {
		LENTODATA( *src++, dst++ ) ;
	    }
	    if( !client->version ) {	      /* client version が ０ のは */
		register int *p = ack->info ;	
		register int tmp1 = p[ 2 ];
		register int tmp2 = p[ 3 ] ;
		int i ;

		for( i = 2; i < 5; i++ )
		    p[ i ] = p[ i + 2 ] ;
		p[ 5 ] = tmp1 ;
		p[ 6 ] = tmp2 ;
	    }
	    ack->len = htonl( size ) ;
	}
	ack->stat = htonl( ret ) ;
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }
    return( WriteToClient( client, ACK2, size ) ) ;
}

ir_get_lex( client )
ClientPtr client ;
{
    Req11 *req = &Request.Request11 ;
    Ack2 *ack = &Acknowledge.Anck2 ;
    RkLex *lex = (RkLex *)local_buffer ;
    int     cxnum ;
    int     size = 0 ;

    if( CHECKCXT( cxnum = req->context ) ) {
	int tangosu ;

	RkGoTo( cxnum, req->number ) ;
	RkXfer( cxnum, req->kouho ) ;

	tangosu = RkGetLex( cxnum, lex, MIN( req->max, LOCAL_BUFSIZE/sizeof( RkLex ) )	) ;

	ack->stat = htonl( tangosu ) ;
	if( tangosu < 0 ) {
	    ack->len = htonl( 0 ) ;
	} else {
	    register int i ;
	    int *dst = ack->info ;
	    int *src = (int *)lex ;

	    size = sizeof( RkLex )*tangosu ;
	    for( i = 0; i < size; i++ ){
		LENTODATA( *src++, dst++ ) ;
	    }
	    ack->len = htonl( size ) ;
	}
    } else {
	extern char *ProtoName[] ;

	PrintMsg( "[%s](%s) Context Err\n", client->username, ProtoName[ req->Type - 1 ]  ) ;
	ack->stat = htonl( -1 ) ;
	ack->len = htonl( 0 ) ;
    }

    return( WriteToClient( client, ACK2, size ) ) ;
}

#ifdef DEBUG
static void
DispDebug( client )
ClientPtr client ;
{
    extern char *ClientStat() ;
    char    return_date[DATE_LENGH] ;
    long    wtime = client->used_time ;
    char    buf[10] ;

    ClientStat( client, GETDATE, 0, 0, return_date ) ;
    Dmsg(5,"ユーザ名         :%s\n", client->username ) ;
    Dmsg(5,"コネクトした時間 :%s\n", return_date ) ;
    Dmsg(5,"ホスト名         :%s\n", client->hostname ) ;
    sprintf( buf,"%02d:%02d:%02d", wtime/3600, (wtime%3600)/60, (wtime%3600)%60 ) ;
    Dmsg(5,"ユーザ消費時間   :%s\n\n", buf ) ;
}	
#endif

static int
ConnectClientCount( client, buf )
ClientPtr   client ;
ClientRec   *buf[] ;
{
    extern ClientPtr	    ConnectionTranslation[] ;
    register ClientPtr	    who ;
    int 		    i, count ;

    bzero( (char *)buf, sizeof( ClientPtr )*MAXSOCKS ) ;
    for( i = 0, count = 0; i < MAXSOCKS; i++ ) {
	if( ((who = ConnectionTranslation[ i ]) != (ClientPtr)NULL)
						&& ( who != client ) ) {
	    *buf = who ;
	    buf ++ ;
	    count ++ ;
	}
    }
    return( count ) ;
}

static int
SetServerProtoVersion( buf )
unsigned char *buf ;
{
    int SendSize = strlen( VERSION ) + 1 ;

    LENTODATA( SendSize, buf ) ; buf += sizeof( int ) ;
    /* サーバのバージョンをセットする */
    bcopy( VERSION, buf, SendSize ) ;
    return( SendSize + sizeof( int ) ) ;
}

ir_server_stat2( client )
ClientPtr client ;
{
    extern char *ProtoName[] ;
    unsigned char *sendp = Acknowledge.SendAckBuffer ;
    unsigned char *savep ;
    register ClientPtr	    who ;
    register ClientStatPtr  Wp ;
    ClientPtr	OutPut[ MAXSOCKS ] ;
    int     RequestCount[ MAXREQUESTNO ] ;
    int     i, j, count, len ;

    /* プロトコルバージョンセット */
    sendp += SetServerProtoVersion( sendp ) ;

    /* 現在時刻セット */
    LENTODATA( time( (long *)0 ), sendp ) ; sendp += sizeof( long ) ;

    /* プロトコル数セット */
    LENTODATA( REALREQUEST, sendp ) ; sendp += sizeof( long ) ;

    /* プロトコル名リスト作成 */
    savep = sendp ; sendp += sizeof( int ) ;
    for( i = 1; i < MAXREQUESTNO; i++ ) {
	RequestCount[ i - 1 ] = htonl( TotalRequestTypeCount[ i ] ) ;
	strcpy( sendp, ProtoName[ i - 1 ] ) ;
	sendp += (strlen( ProtoName[ i - 1 ] ) + 1 ) ;
    }

    /* プロトコル名リスト長セット */
    LENTODATA( sendp - ( savep+sizeof( int ) ), savep ) ;

    /* プロトコル使用頻度セット */
    bcopy( RequestCount, sendp, REALREQUEST*sizeof( int ) ) ;
    sendp += ( REALREQUEST*sizeof( int ) ) ;

    /* 接続しているクライアント数セット */
    count = ConnectClientCount( client, OutPut ) ;
    LENTODATA( count, sendp ) ; sendp += sizeof( int ) ;

    /* コンテクスト数をセット */
    LENTODATA( MAX_CX, sendp ) ; sendp += sizeof( int ) ;

    if( WriteToClient( client, ACK3, sendp - Acknowledge.SendAckBuffer ) < 0 )
	return( -1 ) ;

    /* 各クライアント情報をセット */
    for( i = 0; i < count; i ++ ) {
	savep = sendp = Acknowledge.SendAckBuffer ;
	who = OutPut[ i ] ;
	sendp += sizeof( int ) ;

	LENTODATA( who->id, sendp ) ; sendp += sizeof( int ) ;
	LENTODATA( who->usr_no, sendp ) ; sendp += sizeof( int ) ;
	LENTODATA( who->used_time, sendp ) ; sendp += sizeof( int ) ;
	LENTODATA( who->idle_date, sendp ) ; sendp += sizeof( int ) ;
	LENTODATA( who->connect_date, sendp ) ; sendp += sizeof( int ) ;

	/* プロトコル使用頻度情報セット */
	for( j = 1; j < MAXREQUESTNO; j++ ) {
	    LENTODATA( who->pcount[ j ], sendp ) ;
	    sendp += sizeof( int ) ;
	}

	/* ユーザ名セット */
	len = strlen( who->username ) + 1 ;
	LENTODATA( len, sendp ) ; sendp += sizeof( int ) ;
	bcopy( who->username, sendp, len ) ; sendp += len ;

	/* ホスト名セット */
	len = strlen( who->hostname ) + 1 ;
	LENTODATA( len, sendp ) ; sendp += sizeof( int ) ;
	bcopy( who->hostname, sendp, len ) ; sendp += len ;

	/* コンテクスト管理フラグセット */
	bcopy( who->context_flag, sendp, MAX_CX ) ; sendp += MAX_CX ;

	/* クライアント管理情報リスト長セット */
	LENTODATA( sendp - (savep + sizeof( int )), savep ) ;

	if( WriteToClient( client, ACK3, sendp - savep ) < 0 )
	    return( -1 ) ;

	who ++ ;
    }

    CloseDownClient( client ) ;

    return( 1 ) ;
}

ir_server_stat( client )
ClientPtr client ;
{
    unsigned char *sendp = Acknowledge.SendAckBuffer ;
    register ClientPtr	    who ;
    register ClientStatPtr  Wp ;
    ClientPtr		    OutPut[ MAXSOCKS ] ;
    ClientStatRec	    Sstat[ MAXSOCKS ] ;
    int 		    i, j, count, InfoSize, SendSize ;
    char		    Version[ 128 ] ;
    int 		    RequestCount[ OLD_MAXREQUESTNO ] ;

    count = ConnectClientCount( client, OutPut ) ;

    InfoSize = sizeof( ClientStatRec )*count ;

    Wp = Sstat ;
    for( i = 0 ; i < count; i++ ) {
	who = OutPut[ i ] ;
       ir_debug( DispDebug( who ); )
	Wp->id = htonl( who->id ) ;		
	Wp->usr_no = htonl( who->usr_no ) ;	
	Wp->used_time = htonl( who->used_time ) ;	
	Wp->idle_date = htonl( who->idle_date ) ;
	Wp->connect_date = htonl( who->connect_date ) ; 
	for( j = 0; j < OLD_MAXREQUESTNO; j++ )
	    Wp->pcount[ j ] = htonl( who->pcount[ j ] ) ;
	strncpy( Wp->username, who->username, 10 ) ;
	strncpy( Wp->hostname, who->hostname, 15 ) ;
	bcopy( who->context_flag, Wp->context_flag, MAX_CX ) ;
	Wp ++ ;
    }	

    /* サーバのバージョンを通知する */
    sendp += SetServerProtoVersion( sendp ) ;

    /* サーバの立ち上がってからの総プロトコル */
    for( i = 0; i < OLD_MAXREQUESTNO; i++ )
	RequestCount[ i ] = htonl( TotalRequestTypeCount[ i ] ) ;

    SendSize = sizeof( int )*OLD_MAXREQUESTNO ;
    LENTODATA( SendSize, sendp ) ; sendp += sizeof( int ) ;
    bcopy( RequestCount, sendp, SendSize ) ; sendp += SendSize ;

    /* 情報を送るクライアント数を通知する */
    LENTODATA( count, sendp ) ; sendp += sizeof( int ) ;

    /* サーバの現在の時刻を通知する */
    LENTODATA( time( (long *)0 ), sendp ) ; sendp += sizeof( long ) ;

    /* 実際にクライアント情報を通知する */
    bcopy( Sstat, sendp, InfoSize ) ; sendp += InfoSize ;

    if( WriteToClient( client, ACK3, sendp - Acknowledge.SendAckBuffer ) < 0 )
	return( -1 ) ;

    CloseDownClient( client ) ;
    return( 1 ) ;
}

ir_host_ctl( client )
ClientPtr client ;
{
    extern NumberAccessControlList() ;
    extern ACLPtr ACLHead ;
    unsigned char *sendp = Acknowledge.SendAckBuffer ;
    unsigned char *savep = Acknowledge.SendAckBuffer + sizeof( int ) ;
    unsigned char *namep ;
    ACLPtr wp ;
    int cnt, i ;

    LENTODATA( NumberAccessControlList(), sendp ) ;
    sendp += (sizeof( int )*2) ;

    for( wp = ACLHead; wp != (ACLPtr)NULL; wp = wp->next ) {
	cnt = strlen( wp->hostname ) + 1 ;
	LENTODATA( cnt, sendp ) ;
	sendp += sizeof( long ) ;
	strcpy( sendp, wp->hostname ) ;
	sendp += cnt ;
	LENTODATA( wp->usercnt, sendp ) ;
	sendp += sizeof( long ) ;
	for( i = 0, namep = wp->usernames; i < wp->usercnt; i++ ) {
	    cnt = strlen( namep ) + 1 ;
	    LENTODATA( cnt, sendp ) ;
	    sendp += sizeof( long ) ;
	    strcpy( sendp, namep ) ;
	    sendp += cnt ;
	    namep += cnt ;
	}
    }
    LENTODATA( sendp - ( savep+sizeof( int ) ), savep ) ;
    if( WriteToClient( client, ACK3, sendp - Acknowledge.SendAckBuffer ) < 0 )
	return( -1 ) ;

    CloseDownClient( client ) ;
    return( 1 ) ;
}

static
WriteToClient( client, type, size )
ClientPtr client ;
int type, size ;
{
    int send_size ;

    switch( type ) {
	case ACK0 :
	    send_size = 4 ;
	    break ;
	case ACK1 :
	case ACK2 :
	    send_size = size + 8 ;
	    break ;
	case ACK3 :
	    send_size = size ;
	    break ;
	default :
	    return( -1 ) ;
    }
    return( WriteClient( client->id, Acknowledge.SendAckBuffer, send_size ) ) ;
}


static
WriteClient( ClientFD, buf, size )
int ClientFD ;
unsigned char *buf ;
int size ;
{
    register int write_stat ;
    register unsigned char *bufindex = buf ;
    register int todo = size ;

   ir_debug( Dmsg( 10, "WriteClient:" ) ; )
   ir_debug( DebugDump( 10, buf, size ); )

    while ( size > 0 ) {
	errno = 0;
	write_stat = write( ClientFD, bufindex, todo ) ;
	if (write_stat >= 0) {
	    size -= write_stat;
	    todo = size;
	    bufindex += write_stat;
	    continue ;
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
	    PrintMsg( "Write Error[ %d ]\n", errno ) ;
	    return( -1 ) ;
	}
    }

    return( 0 ) ;
}

/*
 * もともとio.cに入れいていたものをここから下に置く
 */

#define READ_HEADER_SIZE    sizeof( int )
#define READ_SIZE	    2048
#define SIZE4	4   /* sizeof( int ) */
#define SIZE8	8
#define SIZE12	12
#define SIZE16	16
#define SIZE20	20
#define TRY_COUNT   10

static unsigned char
ReadRequestBuffer[ READ_SIZE ] ;	/* デフォルトバッファ */

static int  ReadSize = READ_SIZE ;     /* バッファサイズ */

static unsigned char
*readbufptr = ReadRequestBuffer ;	/* デフォルトバッファ */

int (* CallFunc)() ;

ReadRequestFromClient(who, status )
ClientPtr who;
int *status;	      /* read at least n from client */
{
    int (* ReqCallFunc)() ;
#ifdef DEBUG
    extern char *DebugProc[][2] ;
#endif
    extern int (* Vecter[][2])() ;
#ifdef EXTENSION
    extern int (* ExtensionVecter[][2])() ;
#endif /* EXTENSION */
    unsigned char *bufptr = ReadRequestBuffer ;
    register Req0 *req0 = &Request.Request0 ;
    int client = who->id, n = 0  ;
    int     readsize ;		    /* 一回に読み込んだサイズ */
    int     bufsize = ReadSize ;   /* バッファサイズ */
    int     bufcnt = 0 ;	    /* 読み込んだ累積サイズ */
    int     empty_count = 0 ;

    if( readbufptr != ReadRequestBuffer ) {	   /* 前回バッファを大きく */
       ir_debug( Dmsg( 8,"free readbufptr.\n" ); ) /* した場合は，デフォルト*/
	free( readbufptr ) ;			   /* に戻す */
	readbufptr = ReadRequestBuffer ;
	ReadSize = READ_SIZE ;
    }

    while( empty_count < TRY_COUNT ) {
	if ( (readsize = read( client, bufptr, bufsize )) < 0) {
	    extern char *ProtoName[] ;
	    Req0 *req0 = &Request.Request0 ;

	    if (who->username) {
		PrintMsg( "[%s] ", who->username  ) ;
	    }
	    PrintMsg( "Read request failed\n" ) ;
	    ir_debug( Dmsg( 5, "ReadRequestFromClient: Read request failed\n"); )
	    *status = -1 ;
	    return( 0 ) ;
	} else if ( readsize == 0 ) {	
	    empty_count ++ ;
	    continue ;
	}
       ir_debug( Dmsg( 10, "ReadRequest:" ) ; )
       ir_debug( DebugDump( 10, bufptr, readsize ); )
	bufcnt += readsize ;
	if( (n == 0) && (bufcnt >= 4) ) 
	    DATATOLEN( bufptr, req0->Type ) ;
	
	if (!(who->username)) {
	    ir_debug( Dmsg(5,"Client: <@%s> [%d]\n",
			   who->hostname, req0->Type ) ; );
	} else {
	    ir_debug( Dmsg(5,"Client: <%s@%s> [%d]\n",
			   who->username, who->hostname, req0->Type ) ; );
	}
	
	if( (0 > req0->Type) || 
	   ( (req0->Type > REALREQUEST) && (req0->Type < EXTBASEPROTONO) ) ||
		(req0->Type > (MAXEXTREQUESTNO+EXTBASEPROTONO)) ) {
	    extern char *ProtoName[] ;

	    if (who->username) {
		PrintMsg( "[%s] ", who->username  ) ;
	    }
	    PrintMsg( "Request error[ %d ]\n", req0->Type ) ;
	    *status = -1 ;
	    return( 0 ) ;
	}
	
	/* プロトコルのタイプ毎にデータを呼んでくる関数を呼ぶ */
#ifdef EXTENSION
	if( req0->Type >= EXTBASEPROTONO ) {
	    int request = req0->Type - EXTBASEPROTONO ;
	    ReqCallFunc = ExtensionVecter[ request ][ 1 ] ;
	    CallFunc = ExtensionVecter[ request ][ 0 ] ;
	} else	
#endif /* EXTENSION */
	{
	   ir_debug( Dmsg( 8,"Now Call %s\n", DebugProc[ req0->Type ][ 1 ] ); )
	    ReqCallFunc = Vecter[ req0->Type ][ 1 ] ;
	    CallFunc = Vecter[ req0->Type ][ 0 ] ;
	}
	if( (n = (* ReqCallFunc)( readbufptr, bufcnt ))  < 0 ) {
	    if (who->username) {
		PrintMsg( "[%s] ", who->username  ) ;
	    }
	    PrintMsg( "Read Data failed\n") ;
	    *status = -1 ;
	    return( 0 ) ;	
	} else if( n == 0 ) {
	    break ;
	} else {
	    bufsize -= readsize ;
	    /* 読み込むべきデータがバッファサイズより */
	    /* 多い場合には，アロケートし直す */	
	    if( n > bufsize ) { 
		unsigned char *local_bufptr ;

		if( local_bufptr = (unsigned char *)malloc( bufcnt + n ) ) {
		   ir_debug( Dmsg( 8,"malloc size is %d.\n", bufcnt + n ); )
		    bcopy( readbufptr, local_bufptr, bufcnt ) ;
		    if( readbufptr != ReadRequestBuffer )
			free( readbufptr ) ;
		    readbufptr = local_bufptr ;
		    bufptr = local_bufptr + bufcnt ;
		    ReadSize = bufcnt + n ;
		    bufsize = n ;
		} else {
		    *status = -1 ;
		    if (who->username) {
			PrintMsg( "[%s] ", who->username  ) ;
		    }
		    PrintMsg( "Read Buffer allocate failed\n") ;
		    return( 0 ) ;	
		}
	    } else {
		bufptr += readsize ;
	    }
	    continue ;
	}
    }

    if( empty_count >= TRY_COUNT ) {
	*status = -1 ;
	if (who->username) {
	    PrintMsg( "[%s] ", who->username ) ;
	}
	PrintMsg( "Read Over Time!!\n") ;
	return( 0 ) ;	
    }

    /* プロトコルの種類毎に統計を取る */
    if( req0->Type < MAXREQUESTNO ) {
	who->pcount[ req0->Type ] ++ ;
	TotalRequestTypeCount[ req0->Type ] ++ ;
    }

    ClientStat( who, SETTIME, req0->Type ) ;

    *status = 1 ;
    return( req0->Type ) ;
}

ProcReq0( buf, size )
unsigned char *buf ;
int size ;
{
    return( 0 ) ;
}

ProcReq1( buf, size )
unsigned char *buf ;
int size ;
{
    register Req1 *req = &Request.Request1  ;

    if( size < SIZE8 )
	return( SIZE8 - size ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
    return( 0 ) ;
}

ProcReq2( buf, size )
unsigned char *buf ;
int size ;
{
    register Req2 *req = &Request.Request2 ;
    int needsize ;

   ir_debug( Dmsg(10,"ProcReq start!!\n" ); )
    if( (needsize = SIZE8 - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->namelen )  ;
   ir_debug( Dmsg(10,"req->namelen =%d\n", req->namelen ); )

    if( (needsize = SIZE8 + req->namelen - size) > 0 )
	return( needsize ) ;

    if( req->namelen > 0 ){
	req->name = buf + SIZE8 ;
    }
   ir_debug( Dmsg(10,"req->namelen =%d\n", req->namelen ); )
   ir_debug( Dmsg(10,"req->name =%s\n", req->name ); )
    return( 0 ) ;
}

ProcReq3( buf, size )
unsigned char *buf ;
int size ;
{
    register Req3 *req = &Request.Request3 ;
    int needsize ;

    if( (needsize = SIZE12 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->number ) ;
   ir_debug( Dmsg(10,"req->contest =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->number =%d\n", req->number ); )
    return( 0 ) ;
}

ProcReq4( buf, size )
unsigned char *buf ;
int size ;
{
    register Req4 *req = &Request.Request4 ;
    register int i ;
    int needsize ;

    if( (needsize = SIZE12 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->number ) ;

   ir_debug( Dmsg(5,"req->number=%d\n", req->number ); )

    if( (needsize = SIZE12 + (req->number)*SIZE4 - size ) > 0 )
	return( needsize ) ;

    buf += SIZE12 ;
    req->kouho = (int *)buf ;
    for( i = 0; i < req->number; i++, buf+= SIZE4 )
	DATATOLEN( buf, req->kouho[ i ] ) ;

   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->number =%d\n", req->number ); )
#ifdef DEBUG
    for( i = 0; i < req->number; i++ )
       Dmsg(10,"req->kouho =%d\n", req->kouho[ i ] ) ;
#endif
    return( 0 ) ;
}

ProcReq5( buf, size )
unsigned char *buf ;
int size ;
{
    register Req5 *req = &Request.Request5 ;
    int needsize ;

    if( (needsize = SIZE16 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->number ) ;
    DATATOLEN( buf + SIZE12, req->datalen ) ;
   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->number =%d\n", req->number ); )
   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
    return( 0 ) ;
}

ProcReq6( buf, size )
unsigned char *buf ;
int size ;
{
    register Req6 *req = &Request.Request6 ;
    int needsize ;

    if( (needsize = SIZE16 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->mode ) ;
    DATATOLEN( buf + SIZE12, req->datalen )  ;

    if( (needsize = SIZE16 + req->datalen - size ) > 0 )
	return( needsize ) ;

    req->data = buf + SIZE16 ;
   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->mode =%d\n", req->mode ); )
   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
   ir_debug( Dmsg(10,"req->data =%s\n", req->data ); )

    return( 0 ) ;
}

ProcReq7( buf, size )
unsigned char *buf ;
int size ;
{
    register Req7 *req = &Request.Request7 ;
    int needsize ;

    if( (needsize = SIZE12 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->diclen ) ;

    if( (needsize = SIZE12 + req->diclen - size ) > 0 )
	return( needsize ) ;

    req->dicname = buf + SIZE12 ;

    if( (needsize = SIZE12 + req->diclen - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE12 + req->diclen, req->datalen ) ;

    if( (needsize = SIZE16 + req->diclen + req->datalen - size ) > 0 )
	return( needsize ) ;

    req->datainfo = buf + SIZE16 + req->diclen ;
   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->diclen =%d\n", req->diclen ); )
   ir_debug( Dmsg(10,"req->dicname =%s\n", req->dicname ); )
   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
   ir_debug( Dmsg(10,"req->datainfo =%s\n", req->datainfo ); )
    return( 0 ) ;
}

ProcReq8( buf, size )
unsigned char *buf ;
int size ;
{
    register Req8 *req = &Request.Request8 ;
    int needsize ;

    if( (needsize = SIZE12 - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->datalen ) ;

    if( (needsize = SIZE12 + req->datalen - size ) > 0 )
	return( needsize ) ;

    req->data = buf + SIZE12 ;

    if( (needsize = SIZE16 + req->datalen - size ) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE12 + req->datalen, req->mode ) ;

   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
   ir_debug( Dmsg(10,"req->data =%s\n", req->data ); )
   ir_debug( Dmsg(10,"req->mode =%d\n", req->mode ); )
    return( 0 ) ;
}

ProcReq9( buf, size )
unsigned char *buf ;
int size ;
{
    register Req9 *req = &Request.Request9 ;
    int needsize ;

    if( (needsize = SIZE16 - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->number ) ;
    DATATOLEN( buf + SIZE12, req->datalen ) ;

    if( (needsize = SIZE16 + req->datalen - size) > 0 )
	return( needsize ) ;

    req->data = buf + SIZE16 ;

   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->number =%d\n", req->number ); )
   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
   ir_debug( Dmsg(10,"req->data =%s\n", req->data ); )
    return( 0 ) ;
}

ProcReq10( buf, size )
unsigned char *buf ;
int size ;
{
    register Req10 *req = &Request.Request10 ;
    int needsize ;

    if( (needsize = SIZE12 - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->diclen ) ;

    if( (needsize = SIZE12 + req->diclen - size) > 0 )
	return( needsize ) ;

    req->dicname = buf + SIZE12 ;

    if( (needsize = SIZE16 + req->diclen - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE12 + req->diclen, req->textdiclen ) ;

    if( (needsize = SIZE16 + req->diclen + req->textdiclen - size) > 0 )
	return( needsize ) ;

    if( req->textdiclen )
	req->textdicname = buf + SIZE16 + req->diclen ;
    else
	req->textdicname = (unsigned char *)NULL ;

    if( (needsize = SIZE20 + req->diclen + req->textdiclen - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE16 + req->diclen + req->textdiclen, req->mode ) ;

   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->diclen =%d\n", req->diclen ); )
   ir_debug( Dmsg(10,"req->dicname =%s\n", req->dicname ); )
   ir_debug( Dmsg(10,"req->textdiclen =%d\n", req->textdiclen ); )
   ir_debug( Dmsg(10,"req->textdicname =%s\n", req->textdicname ); )
   ir_debug( Dmsg(10,"req->mode =%d\n", req->mode ); )

    return( 0 ) ;
}

ProcReq11( buf, size )
unsigned char *buf ;
int size ;
{
    register Req11 *req = &Request.Request11 ;
    int needsize ;

    if( (needsize = SIZE20 - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->context ) ;
    DATATOLEN( buf + SIZE8, req->number ) ;
    DATATOLEN( buf + SIZE12, req->kouho ) ;
    DATATOLEN( buf + SIZE16, req->max ) ;
   ir_debug( Dmsg(10,"req->context =%d\n", req->context ); )
   ir_debug( Dmsg(10,"req->number =%d\n", req->number ); )
   ir_debug( Dmsg(10,"req->kouho =%d\n", req->kouho ); )
   ir_debug( Dmsg(10,"req->max =%d\n", req->max ); )
    return( 0 ) ;
}

ProcReq12( buf, size )
unsigned char *buf ;
int size ;
{
    register Req12 *req = &Request.Request12 ;
    int needsize ;

    if( (needsize = SIZE8 - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE4, req->datalen ) ;

    if( (needsize = SIZE8 + req->datalen - size) > 0 )
	return( needsize ) ;

    if( req->datalen > 0 ){
	req->data = buf + SIZE8 ;
    }

    if( (needsize = SIZE8 + req->datalen - size) > 0 )
	return( needsize ) ;

    DATATOLEN( buf + SIZE8 + req->datalen, req->number ) ;

   ir_debug( Dmsg(10,"req->datalen =%d\n", req->datalen ); )
   ir_debug( Dmsg(10,"req->data =%s\n", req->data ); )
   ir_debug( Dmsg(10,"req->number =%s\n", req->number ); )
    return( 0 ) ;
}

#ifdef DEBUG
DebugDump( level, buf, size )
int level, size ;
unsigned char *buf ;
{
    unsigned char buf1[80] ;
    unsigned char buf2[17] ;
    unsigned char c ;
    int     i, j;
    int     count = 0 ;

    Dmsg( level, " SIZE = %d\n", size ) ;
    Dmsg( level, " COUNT  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f      0123456789abcdef\n" ) ;
    for (i = 0; i < size; i += 16) {
	    bzero( buf1, 50 ) ;
	    bzero( buf2, 17 ) ;
	    for (j = 0; j < 16; j++) {
		    if( i+j >= size ) {
			strcat( buf1, "   " ) ;
			strcat( buf2, " " ) ;
		    } else {
			sprintf( buf1,"%s%02x ", buf1, (c = buf[i + j]) & 0xFF);
			if( (c & 0xff) >= ' ' && (c & 0xff) < 0x7f )
			    sprintf( buf2,"%s%c", buf2, c ) ;
			else
			    sprintf( buf2,"%s.", buf2 ) ;
		    }
	    }
	    Dmsg( level," %05x %s     %s\n", count++,  buf1, buf2 );
    }

}

DebugDispKanji( cxnum, num )
int cxnum, num ;
{
    unsigned char buf[1024] ;

    RkGetKanji( cxnum, buf, 1024 ) ;
    Dmsg( 5,"%d:[%s] ", num, buf ) ;
}
#endif

#ifdef PROTO
RkListDic( cxnum, dirname, mbuf, size )
int cxnum, size ;
unsigned char *dirname, *mbuf ;
{
    if( RkSetDicPath( cxnum, dirname ) < 0 )
	return( -1 ) ;

    return( RkGetDicList( cxnum, mbuf, size ) ) ;
}

RkCreateDic( cxnum, dicname, mode )
int cxnum, mode ;
unsigned char *dicname ;
{
    return( 0 ) ;
}

RkRemoveDic( cxnum, dicname )
int cxnum ;
unsigned char *dicname ;
{
    return( 0 ) ;
}

RkRenameDic( cxnum, dicname, newdicname, mode )
int cxnum, mode ;
unsigned char *dicname, *newdicname ;
{
    return( 0 ) ;
}

RkGetWordTextDic( cxnum, dirname, dicname, info, infolen )
int cxnum, infolen ;
unsigned char *dirname, *dicname, *info ;
{
   ir_debug( Dmsg( 5,"RkGetWordTextDic( %d, %s, %s, info, infolen )\n", cxnum, dirname, dicname ) );
    strncpy( info, "てすと #T35 テスト", infolen ) ;

    return( strlen( info ) ) ;
}

#endif /* PROTO */
