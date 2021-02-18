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
 *
 */

static char sccs_id[]="@(#) 102.1 NEC UNIX( PC-UX/EWS-UX ) irohastat.c 1.2 90/10/16 11:50:32";

#include    <stdio.h>
#include    <time.h>
#include    <sys/types.h>
#include    "net.h"
#include    <errno.h>

#include    "IR.h"

#define PROTO	1
#define ALL	2
#define TOTAL	3

#define ATFORMAT "%-10s%5s%5s%5s%16s%9s%9s  %-s\n"
#define ADFORMAT "%-10s%5d%5d%5d%16s%9s%9s  %-s\n"

#define BTFORMAT "%7s %7s %7s %11s %11s\n"
#define BDFORMAT "%7d %7d %7d %11s %11s\n\n"

static int ServerFD ;
static int ServerVersion ;

static IRReq IRreq ;
static long cur_time ;	

static int
*TotalReqCount, OldTotalReqCount[ OLD_MAXREQUESTNO ] ;
ProtoCount,
ListSize,
ContextNum ;

static char
*ProtoList, *ContextFlag ;

main(argc, argv)
int argc ;
char **argv ;
{
    char		irohahostname[ 256 ] ;
    register Req0	*req ;
    ClientRec		client ;		
    ClientStatRec	clientstat ;		
    int 		ConectNum, i, ResevInt ;
    char		*hostname = (char *)NULL, *Version, *Buffer ;
    int 		argflag = 0, flag = 0 ;

    irohahostname[0] = '\0';
    for( i = 1; i < argc; i++ ) {
	if( !strcmp( argv[ i ], "-is" ) || !strcmp( argv[ i ], "-irohaserver" ) ) {
	    strcpy( irohahostname, argv[i + 1] ) ;
	    i++ ;
	} else if( !strcmp( argv[ i ], "-p" ))	
	    argflag = PROTO ;
	else if( !strcmp( argv[ i ], "-a" ))
	    argflag = ALL ;
	else if( !strcmp( argv[ i ], "-t" ))
	    argflag = TOTAL ;
	else if( !strcmp( argv[ i ], "-v" ))
	    flag = 1 ;
	else
	    usage() ;
    }
	
    if( (ServerFD = rkc_Connect_Iroha_Server( irohahostname )) < 0 ) {
	fprintf(stderr,"Can't Connected %s\n", irohahostname ) ;
	exit( 2 ) ;
    }
    printf("Connected to %s\n", irohahostname ) ;

    if( (ServerVersion = rkc_initialize( "1.1:bin" )) < 0 ) {
	if( ServerVersion == RETURN_VERSION_ERROR_STAT ){
	    ServerVersion = 0 ;
	} else {
	    fprintf(stderr,"Error Disconnected %s\n", irohahostname ) ;
	    exit( 2 ) ;
	}
    } else {
	rkc_finalize() ;
    }

    /*	パケット組み立て */
    req = &IRreq.Request0 ;

    ServerFD = rkc_Connect_Iroha_Server( irohahostname ) ;

    if( ServerVersion ) {
	req->Type = htonl( IR_SER_STAT2 ) ;
    } else
	req->Type = htonl( IR_SER_STAT ) ;

    if( WriteToServer( IRreq.Buffer, sizeof( Req0 )) == -1 ) {
	perror("WriteToServer") ;
	exit( 1 ) ;
    }

    /* サーババージョンを取得する */
    ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
    ResevInt = ntohl( ResevInt ) ;
    if( Version = (char *)malloc( ResevInt ) ) {
	ReadServer( Version, ResevInt ) ;
	printf("Iroha Server (Ver. %s)\n", Version ) ;
	if( strcmp( Version, "1.0" ) )
	    ServerVersion = 1 ;
	else
	    ServerVersion = 0 ;
	free( Version ) ;
    } else {
	printf("Iroha Server (Ver. XX)\n" ) ;
    }

    if( ServerVersion ) {
	/* サーバのカレント時間 */
	ReadServer( (char *)&cur_time, sizeof( int ) ) ;

	/* プロトコル数 */
	ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
	ProtoCount = ntohl( ResevInt ) ;
	
	/* プロトコル名リスト長 */
	ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
	ListSize = ntohl( ResevInt ) ;
	
	/* プロトコル名リスト取得 */
	ProtoList = (char *)malloc( ListSize ) ;
	ReadServer( ProtoList, ListSize ) ;
	
	TotalReqCount = (int *)calloc( ProtoCount, sizeof( int ) ) ;
	ReadServer( (char *)TotalReqCount, ProtoCount*sizeof( int ) ) ;
	
	for( i = 0; i < ProtoCount; i++ )
	    DATATOLEN( (char *)&TotalReqCount[ i ], TotalReqCount[ i ] ) ;

	ReadServer( (char *)&ConectNum, sizeof( int ) ) ;
    } else {	
	ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
	ResevInt = ntohl( ResevInt ) ;

	ReadServer( (char *)OldTotalReqCount, ResevInt ) ;
	
	ReadServer( (char *)&ConectNum, sizeof( int ) ) ;
	
	ReadServer( (char *)&cur_time, sizeof( int ) ) ;
    }
    ConectNum = ntohl( ConectNum ) ;
    cur_time = ntohl( cur_time ) ;

    if( ConectNum )
	printf("Total connecting clients %d\n" , ConectNum ) ;
    else {
	printf("No clients\n") ;
    }

    if( argflag == TOTAL ) {
	DispProto( (ClientPtr)NULL ) ;
	putchar( '\n' ) ;
	exit( 0 ) ;
    } else if( !ConectNum )
	exit( 0 ) ;

    if( !flag && (!argflag || (argflag == ALL)) ) {
	printf( ATFORMAT,
   "USER_NAME", "ID", "NO", "U_CX", "C_TIME", "U_TIME", "I_TIME", "HOST_NAME" ) ;
    } else {
	printf("\n") ;
    }
    if( ServerVersion ) {
	ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
	ContextNum = ntohl( ResevInt ) ;
	ContextFlag = (char *)malloc( ContextNum ) ;
    } else {
	ContextFlag = (char *)malloc( 100 ) ;
    }
    for( i = 0; i < ConectNum ; i++ ) {
	if( ServerVersion ) {
	    ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
	    ResevInt = ntohl( ResevInt ) ;

	    Buffer = (char *)malloc( ResevInt ) ;
	
	    ReadServer( (char *)Buffer, ResevInt ) ;
	
	    bzero( &client, sizeof( ClientRec ) ) ;
	    bzero( ContextFlag, ContextNum ) ;
	    CreateData( Buffer, &client ) ;
	} else {
	    bzero( &clientstat, sizeof( ClientStatRec ) ) ;
	    ReadServer( (char *)&clientstat, sizeof( ClientStatRec ) ) ;
	    ConvertData( &clientstat, &client ) ;
	}

	switch( argflag ) {
	    case PROTO :
		printf("%s\n", client.username ) ;
		DispProto( &client ) ;
		putchar( '\n' ) ;
		break ;
	    case ALL :
		DispInfo( &client, flag ) ;
		DispProto( &client ) ;
		putchar( '\n' ) ;
		break ;
	    default :
		DispInfo( &client, flag ) ;
		break ;
	}
	if( ServerVersion ) {
	    free( client.username ) ;
	    free( client.hostname ) ;
	    free( Buffer ) ;
	}
    }
}

DispInfo( client, flag )
register ClientPtr client ;
int flag ;
{
    static char *week[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" } ;
    char	ctime[ 15 ], utime[ 10 ], itime[ 10 ] ;
    char	name[ 10 ], host[ 15 ] ;
    char	*ascdate ;
    int 	i, id, user_no, u_cx ;
    long	cdate, udate, idate ;
    struct tm	*tt, *tt_buf ;

    id = client->id ;
    user_no = client->usr_no ;
    udate = client->used_time ;

    /* コネクト時間の整形 */
    cdate = client->connect_date ;
    tt = localtime( &cdate ) ;
    if( flag ) {
	ascdate = asctime( tt ) ;
    } else {
	sprintf( ctime,"%s %d", week[ tt->tm_wday ], tt->tm_mday ) ;
	if( tt->tm_hour > 12 )
	  sprintf( ctime,"%s %2d:%02dpm", ctime, tt->tm_hour - 12, tt->tm_min ) ;
	else if( tt->tm_hour == 12 )
	  sprintf( ctime,"%s 12:%02dpm", ctime, tt->tm_min ) ;
	else if( tt->tm_hour == 0 )
	  sprintf( ctime,"%s 12:%02dam", ctime, tt->tm_min ) ;
	else
	  sprintf( ctime,"%s %2d:%02dam", ctime, tt->tm_hour, tt->tm_min ) ;
	
	sprintf( utime,"%02d:%02d:%02d", udate/3600, (udate%3600)/60
						    , (udate%3600)%60 ) ;
    }

    if (udate < 3600) {
	if (udate < 60) 
	    sprintf( utime, "      %2d", udate);
	else
	    sprintf( utime, "   %2d'%02d", udate/60, udate%60 );
    } else {
      sprintf( utime,"%02d:%02d'%02d",
		udate/3600, (udate%3600)/60, (udate%3600)%60 ) ;
    }

    idate = cur_time - client->idle_date ;
    /* １分以内は表示しない */
    if( idate < 60 )
	strcpy( itime, "" ) ;
    else if ( idate < 3600)
	sprintf (itime, "   %2d", idate / 60);
    else
	sprintf( itime,"%2d:%02d", idate/3600, (idate%3600)/60 ) ;
				
    for( i = 0, u_cx = 0; i < ContextNum ; i++ )
	if( ContextFlag[ i ] )
	    u_cx ++ ;

    if( flag ) {
	printf( "USER_NAME    : %s\n", client->username ) ;
	printf( "HOST_NAME    : %s\n", client->hostname ) ;
	printf( "CONNECT_DATE : %s", ascdate ) ;
	printf( BTFORMAT,
	"USER_ID", "USER_NO", "USED_CX", "USED_TIME","IDLE_TIME" ) ;
	printf( BDFORMAT, id, user_no, u_cx, utime, itime ) ;
    } else {
	bzero( name, 10 ) ;
	bzero( host, 15 ) ;
	strncpy( name, client->username, 9 ) ;
	strncpy( host, client->hostname, 14 ) ;
	
	printf( ADFORMAT, name, id,
			user_no, u_cx, ctime, utime, itime, host ) ;
    }	
}

DispProto( client )
register ClientPtr client ;
{
    static char *OldProtoList[] = {
    "IR_INIT", "IR_FIN", "IR_CRE_CON", "IR_DUP_CON", "IR_CLO_CON",
    "IR_DIC_LIST", "IR_CRE_DIC", "IR_DEF_DIC", "IR_UNDEF_DIC", "IR_DIC_PATH",
    "IR_DIR_LIST", "IR_MNT_DIC", "IR_UMNT_DIC", "IR_RMNT_DIC", "IR_MNT_LIST",
    "IR_CONVERT", "IR_CONV_END", "IR_KAN_LST", "IR_RESIZE", "IR_STO_YOMI",
    "IR_GET_LEX", "IR_GET_STA"
    } ;
    register int i ;
    char *protoname ;
    int  *dispdata ;

    if( ServerVersion ) {
	protoname = ProtoList ;
	dispdata = TotalReqCount ;
    } else {
	ProtoCount = OLD_MAXREQUESTNO - 2 ;
    }

    for( i = 0; i < ProtoCount; i++ ) {
	if( ServerVersion ) {
	    printf("%12s:%6d", protoname, dispdata[ i ] ) ;
	    protoname += ( strlen( protoname ) + 1 ) ;
	} else {
	    printf("%12s:%6d", OldProtoList[ i ], ntohl( OldTotalReqCount[ i+1 ] ) ) ;
	}
	if( !((i+1)%4) ) putchar('\n') ;
    }
    putchar('\n') ;			
}

int
WriteToServer( Buffer, size )
char *Buffer ;
int size ;
{
    register int todo;
    register int write_stat;
    register char *bufindex;
    int cnt ;

    errno = 0 ;
    bufindex = Buffer ;
    todo = size ;
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
	    perror( "write faild" ) ;
	    return( -1 ) ;
	}
    }
}


int
ReadServer( Buffer, size )
char *Buffer ;
int size ;
{
    register long bytes_read;

    if (size == 0) return;
    errno = 0;
    while ((bytes_read = read(ServerFD, Buffer, (int)size)) != size) {	
	if (bytes_read > 0) {
	    size -= bytes_read;
	    Buffer += bytes_read;
	} else if (errno == EWOULDBLOCK) { /* pc98 */
	    continue ;
	} else {
	    perror("ReadToServer") ;
	    exit( 1 ) ;
	}
    }
    return( 0 ) ;
}

CreateData( readbuf, who )
char *readbuf ;
ClientPtr   who ;
{
    char *receivep = readbuf ;
    int len, j ;

    DATATOLEN( receivep, who->id ) ; receivep += sizeof( int ) ;
    DATATOLEN( receivep, who->usr_no ) ; receivep += sizeof( int ) ;
    DATATOLEN( receivep, who->used_time ) ; receivep += sizeof( int ) ;
    DATATOLEN( receivep, who->idle_date ) ; receivep += sizeof( int ) ;
    DATATOLEN( receivep, who->connect_date ) ; receivep += sizeof( int ) ;

    for( j = 0; j < ProtoCount; j++ ) {
	DATATOLEN( receivep, TotalReqCount[ j ] ) ;
	receivep += sizeof( int ) ;
    }

    DATATOLEN( receivep, len ) ; receivep += sizeof( int ) ;
    if( who->username = (char *)malloc( len ) )
	bcopy( receivep, who->username, len ) ; 
    receivep += len ;

    DATATOLEN( receivep, len ) ; receivep += sizeof( int ) ;
    if( who->hostname = (char *)malloc( len ) )
	bcopy( receivep, who->hostname, len ) ; 
    receivep += len ;

    if( ContextFlag )
	bcopy( receivep, ContextFlag, ContextNum ) ;
}

ConvertData( clientstat, client )
ClientStatPtr	clientstat ;
ClientPtr	client ;
{
    register int i ;

    client->id = ntohl( clientstat->id ) ;		
    client->usr_no = ntohl( clientstat->usr_no ) ;	
    client->used_time = ntohl( clientstat->used_time ) ;	
    client->idle_date = ntohl( clientstat->idle_date ) ;	
    client->connect_date = ntohl( clientstat->connect_date ) ;	
    client->username = clientstat->username ;
    client->hostname = clientstat->hostname ;
    for( i = 0; i < 0x18; i++ )
	OldTotalReqCount[ i ] = clientstat->pcount[ i ] ;
    bcopy( client->context_flag, ContextFlag, 100 ) ;
}


usage()
{
    fprintf( stderr, "usage: irohastat [-is | -irohaserver hostname] [-a|-v]\n" ) ;
    fprintf( stderr, "                 [-is | -irohaserver hostname] [-t]\n" ) ;

    fflush( stderr ) ;
    exit( 0 ) ;
}
