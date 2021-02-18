static char sccs_id[]="%Z% NEC UNIX( PC-UX/EWS-UX ) %M% %R%.%L% %E% %U%";

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
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <errno.h>

#ifdef pcux
#include <sys/fcntl.h>
#else
#include <fcntl.h>
#endif
#include <signal.h>
#include "IR.h"

#ifndef LOCKDIR
#define LOCKDIR     "/usr/spool/iroha/lock"
#endif

#ifndef DICHOME
#define DICHOME     "/usr/lib/iroha/dic"
#endif

#ifndef ERRDIR
#define ERRDIR      "/usr/spool/iroha"
#endif

#ifndef LOCKFILE
#define LOCKFILE    ".IROHALOCK"
#endif

#define ERRFILE     "IROHA"
#define ERRFILE2    "msgs"
#define ERRSIZE     64
#define MOUNT_FILE  "premountdics"
#define ACCESS_FILE "/etc/hosts.iroha"

extern CheckConnections() ;

void
FatalError() ;

#ifdef DEBUG
#define LOGFILE "/tmp/iroha.log"
static FILE *ServerLogFp ;
static FILE *Fp ;
static int DebugMode = 0 ;
static int LogLevel = 0 ;
#endif

int PortNumberPlus = 0 ;
static char *LockFile[ 64 ] ;
static char *Name[64] ;
static FILE *lockfp ;
unsigned char *PreMountTabl[ MAX_CX ] ;
static unsigned char *MyName ;

ACLPtr ACLHead = (ACLPtr)NULL ;

static Reset() ;
static void parQUIT();
void MountDictionary() ;

char anotherServerFormat[] = "\
ERROR:\n\
\tAnother 'irohaserver' is detected.\n\
\tIf 'irohaserver' is not running, lock-file may remain accidentally.\n\
\tSo, after making sure that 'irohaserver' is not running,\n\
\tPlease execute following command.\n\n\
\trm %s\n";

void
BecomeDaemon ( argc, argv )
int argc ;
char *argv[] ;	
{
    unsigned char *ddname = (unsigned char *)NULL ;
    unsigned char buf[ MAXDATA ] ;
    char    errfile[ ERRSIZE ] ;
    int     parent, parentid, i ;
    int     lockfd, errfd, context ;

    strcpy( Name, argv[ 0 ] ) ;

    for( i = 1; i < argc; i++ ) {
	if( argv[i][0] == '/' ) {
	    ddname = (unsigned char *)malloc( strlen( argv[ i ] ) + 1 ) ;
	    if( ddname )
		strcpy( ddname, argv[ i ] ) ;
	}

	if( !strcmp( argv[i], "-p") )
	    PortNumberPlus = atoi( argv[ i+1 ] ) ;
    }

    if( !ddname ) {
	ddname = (unsigned char *)malloc( strlen( DICHOME ) + 1 ) ;
	if( !ddname )
	    FatalError("irohaserver:Initialize failed\n") ;
	strcpy( ddname, DICHOME ) ;
    }

/* 同一マシン上で複数のサーバが起動しないようにロックファイルを作る */
    if ( mkdir( LOCKDIR, 0777 ) == -1 &&
	errno != EEXIST ) {
	(void)fprintf(stderr, "Can't open ");
	(void)fprintf(stderr, LOCKDIR) ;
	(void)fprintf(stderr, ", error No. %d\n", errno);
    }
    if( PortNumberPlus )
	sprintf( LockFile, "%s/%s:%d", LOCKDIR, LOCKFILE, PortNumberPlus ) ;
    else
	sprintf( LockFile, "%s/%s", LOCKDIR, LOCKFILE ) ;
    if( (lockfd = open( LockFile, O_CREAT|O_RDWR|O_EXCL, 00644 )) < 0){
	if( errno == EEXIST ) {
	    fprintf(stderr, anotherServerFormat, LockFile);
	} else {
	    fprintf(stderr,"%s:Can't create lock file(%s)\n", Name, LockFile) ;
	}
	exit( 1 );
    }
    close( lockfd ) ;

    if( (lockfp = fopen( LockFile, "w" )) == (FILE *)NULL ) {
	exit( 2 ) ;
    }

#ifdef DEBUG
    DebugMode = 0 ;
    ServerLogFp = stderr ;
		
    for( i = 1; i < argc; i++ ) {
	if( !strcmp( argv[ i ], "-d" )) {
	    DebugMode = 1 ;
	    LogLevel = 5 ;
	}
	
	if( !strcmp( argv[ i ], "-l" ) ) {
	    /* ログファイル作成 */
	    if( (Fp = fopen( LOGFILE, "w" ) ) != NULL ){
		LogLevel = atoi( argv[ i+1 ] ) ;
		if( LogLevel <= 0 )
		    LogLevel = 1 ;
		ServerLogFp = Fp ;
	    } else {
		perror("Can't Create Log File!!\n");
	    }
	}
    }

#endif /* DEBUG */

    getserver_version() ;

   ir_debug( Dmsg(5, "辞書ホームディレクトリィ = %s\n", ddname ); )

    if( context = RkInitialize( (char *)ddname ) < 0 )
	FatalError("irohaserver:Initialize failed\n") ;
    free( (char *)ddname ) ;
    RkCloseContext( context ) ;

    if( gethostname( buf, MAXDATA ) == 0 ) {
	if( MyName = (unsigned char *)malloc( strlen( buf )+1) )
	    strcpy( MyName, buf ) ;
    }

   ir_debug( Dmsg(5, "My name is %s\n", MyName ); )

#ifdef DEBUG
    if( DebugMode ) {
	signal(SIGPIPE,  SIG_IGN) ;
	bzero( PreMountTabl, MAX_CX*sizeof(int) ) ;
	MountDictionary( (unsigned char *)NULL ) ;
	CreateAccessControlList() ;

	return ; /* デーモンにならない */
    }
#endif
    /*
     * FORK a CHILD
     */

    parentid = getpid() ;

    /* 標準エラー出力をエラーファイルに切り替えて、標準入出力をクローズする */
    bzero( errfile, ERRSIZE ) ;
    sprintf( errfile,"%s/%s%d%s", ERRDIR, ERRFILE, PortNumberPlus, ERRFILE2 ) ;

    if( ( errfd = open( errfile, O_CREAT | O_RDWR | O_TRUNC, 00644 ) ) < 0 ) {
	(void)fprintf(stderr, "Warning: %s: %s open faild\n", Name, errfile );
	(void)perror("");
    } else {
	if( dup2( errfd, fileno( stderr ) ) < 0 ) {
	    (void)fprintf(stderr, "Warning: %s: %s dup2 faild\n", Name, errfile );
	    (void)perror("") ;
	    close( fileno( stderr ) ) ;
	}
    }
    close( fileno( stdin ) ) ;
    close( fileno( stdout ) ) ;
    close( errfd ) ;

    bzero( PreMountTabl, MAX_CX*sizeof( int ) ) ;
    MountDictionary( (unsigned char *)NULL ) ;

    CreateAccessControlList() ;

    signal(SIGQUIT, parQUIT);

    if ((parent = fork()) == -1) {
	PrintMsg( "Fork faild\n" );
	exit( 1 ) ;
    }
    if ( parent ) {

	pause() ;
	exit( 0 ) ;
	/* wait( (int *)0 ) ;	*/
    }

    fprintf( lockfp ,"%d\n", getpid() ) ;
    fflush( lockfp ) ;
    fclose( lockfp ) ;

    /*
     * 子プロセス
     */
    (void)setpgrp();

    /*
     * シグナル処理
     */
    signal(SIGHUP,   SIG_IGN);
    signal(SIGINT,   SIG_IGN);
    signal(SIGQUIT,  SIG_IGN);
    signal(SIGALRM,  SIG_IGN);
    signal(SIGTRAP,  Reset);
    signal(SIGILL,   Reset);
    signal(SIGPIPE,  SIG_IGN) ;
#ifdef pcux
    signal(SIGABRT,  Reset);
#endif
    signal(SIGTERM,  Reset);
    signal(SIGEMT,   Reset);
    signal(SIGBUS,   Reset);
    signal(SIGSEGV,  Reset);

    umask( 002 ) ;

    kill( parentid, SIGQUIT )  ;
}

static void
CloseServer()
{
    RkFinalize() ;
    AllCloseDownClients() ;
    RemoveLockFile() ;
}

void
FatalError(f)
    char *f;
{
    fprintf(stderr,"%s\n", f);
    CloseServer() ;
    exit(2);
    /*NOTREACHED*/
}

#ifdef DEBUG
Dmsg( Pri, f, s0, s1, s2, s3, s4, s5, s6, s7, s8 )
int Pri ;
unsigned char *f;
unsigned char *s0, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8 ;
{
    if ( LogLevel >= Pri ) {
	fprintf(ServerLogFp , f, s0, s1, s2, s3, s4, s5, s6, s7, s8 );
	fflush( ServerLogFp ) ;
    }
}
#endif

PrintMsg( f, s0, s1, s2, s3, s4, s5, s6, s7, s8 )
unsigned char *f;
unsigned char *s0, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8 ;
{
    long    Time ;
    char    *date ;

    Time = time( (long *)0 ) ;
    date = (char *)ctime( &Time ) ;
    date[24] = '\0' ;
    fprintf( stderr, "%s :", date ) ;
    fprintf( stderr, f, s0, s1, s2, s3, s4, s5, s6, s7, s8 );
    fflush( stderr ) ;
}

static
RemoveLockFile()
{
    if( unlink( LockFile ) < 0 )				
	PrintMsg( "Remove lockfile faild\n" ) ;
}

static
Reset(sig)
int	sig;
{
    if( sig == SIGTERM ) {
	PrintMsg( "Irohaserver Terminated\n" ) ;
	CloseServer() ;
    } else {
	PrintMsg( "Caught a signal(%d)\n", sig ) ;
	RemoveLockFile() ;
    }
    exit(2);
}

static void
parQUIT(sig)
int    sig;
{
    exit( 0 ) ;
  /* 何もしない */
}

/*
 * クライアントのマウント時間を短縮するためにあらかじめ
 * 指定された辞書をマウントしておく。
 */
void
MountDictionary( ddname )
char *ddname ;
{
    register int    i,	context, index = 0 ;
    unsigned char   buf[ BUFSIZE ], *wp ;
    FILE *fp ;

    sprintf(buf, "%s/%s", DICHOME, MOUNT_FILE);
    if( (fp = fopen( buf, "r" )) == (FILE *)NULL ) {
	PrintMsg( "Can't open %s\n", buf ) ;
	return ;
    }

    while( fgets( (char *)buf, BUFSIZE, fp ) != (char *)NULL ) {
	buf[ strlen( buf )-1 ] = '\0' ;
	wp = buf ;
	if( !strtok( wp, ":" ) )
	    continue ;
	if( ddname ) {
	    if( strcmp( ddname, (char *)wp ) )	
		continue ;
	    for( i = 0; i < MAX_CX; i++ ) { /* 使用しているコンテクストを探す */
		if( !strcmp( PreMountTabl[ i ], ddname ) ){
		    context = i ;
		    break ;
		}
	    }
	    if( i == MAX_CX ) {
		fclose( fp ) ;
		return ;
	    }
	} else {
	   /* 辞書ホームディレクトリにあるディレクトリをDDPATHに設定する。*/
	    if( (context = RkCreateContext()) < 0 ) {
		PrintMsg("Can't create context\n" ) ;
		fclose( fp ) ;
		return ;
	    }
	    if( RkSetDicPath( context, wp ) < 0 ) {
		PrintMsg("Can't set dictionary path\n" ) ;
		fclose( fp ) ;
		return ;
	    }
	    if( PreMountTabl[ index ] = (unsigned char *)malloc( strlen( wp )+1 ) ) {
		strcpy( PreMountTabl[ index ], wp ) ;
		index++ ;
	       ir_debug( Dmsg(5,"dicpath=%s\n", wp ); )
	       ir_debug( Dmsg(5,"Mount Dic File is"); )
	    }
	}
	while( (wp = (unsigned char *)strtok( (char *)NULL, "," ))
						!= (unsigned char *)NULL ) {
	    if( ddname ) {
		if( RkUnmountDic( context, wp ) < 0 )
		    PrintMsg( "Can't unmount [%s] dictionary\n", wp ) ;
	    }
	    if( RkMountDic( context, wp ) < 0 ) 
		PrintMsg( "Can't mount [%s] dictionary\n", wp ) ;
	   ir_debug( Dmsg(5,"[ %s ]", wp ); )
	}
       ir_debug( Dmsg(5,"\n" ); )
    }
    fclose( fp ) ;
}

static
ACLCheckHostName( currentptr )
ACLPtr	currentptr ;
{
    unsigned char *hostname = currentptr->hostname ;
    ACLPtr  wp ;
    int     i ;

    for( wp = ACLHead; wp != (ACLPtr)NULL; wp = wp->next ) {
	if( !strcmp( wp->hostname, hostname ) ) {
	    free( (char *)currentptr ) ;
	    return( -1 ) ;
	}
    }
    return( 0 ) ;
}

CreateAccessControlList()
{
    unsigned char   buf[ BUFSIZE ] ;
    unsigned char   *wp, *p ;
    ACLPtr  current = (ACLPtr)NULL ;
    ACLPtr  prev = (ACLPtr)NULL ;
    int     i, c ;
    FILE    *fp ;

    if( (fp = fopen( ACCESS_FILE, "r" )) == (FILE *)NULL )
	return( -1 ) ;

    if( !ACLHead )
	FreeAccessControlList() ;

    while( fgets( (char *)buf, BUFSIZE, fp ) != (char *)NULL ) {
	buf[ strlen( buf )-1 ] = '\0' ;
	wp = buf ;
	if( !strtok( wp, ":" ) )
	    continue ;

	if( !(current = (ACLPtr)malloc( sizeof( ACLRec ) )) ) {
	    PrintMsg("Can't create access control list!!" ) ;	
	    fclose( fp ) ;
	    FreeAccessControlList() ;
	    return( -1 ) ;
	}

	bzero( current, sizeof( ACLRec ) ) ;

	current->hostname = (unsigned char *)malloc( strlen(wp) + 1 ) ;
	if( current->hostname ) {
	    strcpy(current->hostname, wp);
	    if( !strcmp( current->hostname, MyName ) ) {
		free( current->hostname ) ;
		current->hostname = (unsigned char *)malloc( strlen("unix")+1 ) ;
		strcpy( current->hostname, "unix" ) ;
	    }
	}

	if( ACLCheckHostName( current ) < 0 )
	    continue ;

	wp += ( strlen( wp )+1 );
	
	if( strlen( wp ) ) {
	    if(current->usernames = (unsigned char *)malloc(strlen(wp) + 1)) {
	        strcpy(current->usernames, wp);
		for( p = current->usernames; *p != '\0'; p++ ) {
		    if( *p == ',' ) {
			*p = '\0' ;
			current->usercnt ++ ;
		    }
		}
		current->usercnt ++ ;
	    }
	}
	if( ACLHead ) {
	    current->prev = prev ;
	    prev->next = current ;
	} else {
	    ACLHead = current ;
	    current->prev = (ACLPtr)NULL ;
	}
	current->next = (ACLPtr)NULL ;
	prev = current ;
    }
    if( current )
	current->next = (ACLPtr)NULL ;

    fclose( fp ) ;
}

FreeAccessControlList() 
{
    ACLPtr  wp, tailp ;
    int     i, c ;

    if( !(wp = ACLHead) )
	return ;

    for( ; wp != (ACLPtr)NULL; wp = wp->next ) {
	    if( wp->hostname )
		free( wp->hostname ) ;
	    if( wp->usernames )
		free( wp->usernames ) ;
	    tailp = wp ;
    }

    for( wp = tailp; wp != (ACLPtr)NULL; wp = wp->prev ) {
	if( wp->next )
	    free( wp->next ) ;
    }
    ACLHead = (ACLPtr)NULL ;
}

CheckAccessControlList( hostname, username )
unsigned char *hostname ;
unsigned char *username ;
{
    unsigned char *hostp = hostname;
    unsigned char *userp ;
    ACLPtr  wp ;
    int     i, c ;


    if( !ACLHead )
	return( 0 ) ;

   ir_debug( Dmsg(5, "My name is %s\n", MyName ); )
   ir_debug( Dmsg(5, "Check name is %s\n", hostname ); )

    if( !strcmp( MyName, hostname ) )
	hostp = (unsigned char *)"unix" ;

    for( wp = ACLHead; wp != (ACLPtr)NULL; wp = wp->next ) {
	if( !strcmp( wp->hostname, hostp ) ) {
	    if( wp->usernames ) {
		for( i = 0, userp = wp->usernames; i < wp->usercnt; i++ ) {
		    if( !strcmp( userp, username ) )
			return( 0 ) ;
		    userp += (strlen( userp )+1 ) ;
		}
		return( -1 ) ;
	    } else
		return( 0 ) ;
	}
    }
    return( -1 ) ;
}

NumberAccessControlList()
{
    ACLPtr  wp ;
    int     i ;

    for( wp = ACLHead, i = 0; wp != (ACLPtr)NULL; wp = wp->next, i++ )
			;
    return( i ) ;
}
