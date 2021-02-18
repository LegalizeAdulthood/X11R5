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

static char sccs_id[]="@(#) 102.1 NEC UNIX( PC-UX/EWS-UX ) ishost.c 1.1 90/10/27 14:22:03";

#include    <stdio.h>
#include    <time.h>
#include    <errno.h>
#include    <sys/types.h>

#include    "IR.h"
#include    "net.h"

static IRReq IRreq ;
static int ServerFD ;

main(argc, argv)
int argc ;
char **argv ;
{
    register Req0	*req ;
    char		*hostname = (char *)NULL ;
    char		irohahostname[ 256 ] ;
    int 		i, ResevInt ;
    int 		ServerVersion ; 

    irohahostname[0] = '\0';
    for( i = 1; i < argc; i++ ) {
	if( !strcmp( argv[ i ], "-is" ) || !strcmp( argv[ i ], "-irohaserver" ) ) {
	    strcpy( irohahostname, argv[i + 1] ) ;
	    i++ ;
	} else
	    usage() ;
    }

    if( (ServerFD = rkc_Connect_Iroha_Server( irohahostname )) < 0 ) {
	fprintf(stderr,"Can't Connected %s\n", irohahostname ) ;
	exit( 2 ) ;
    }
    printf("Connected to %s\n", irohahostname ) ;

    /*	パケット組み立て */
    req = &IRreq.Request0 ;
    req->Type = htonl( IR_HOST_CTL ) ;
    WriteToServer( IRreq.Buffer, sizeof( Req0 )) ;

    DispControlList() ;
}

DispControlList()
{
    char    ResevBuf[ BUFSIZE ], *wp ;
    int     ResevInt, UserNum, HostNum, NameLen ;
    int     i, j ;

    wp = ResevBuf ;
    ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
    HostNum = ntohl( ResevInt ) ;
    ReadServer( (char *)&ResevInt, sizeof( int ) ) ;
    ResevInt = ntohl( ResevInt ) ;
    ReadServer( ResevBuf, ResevInt ) ;
    printf("access control enabled\n" ) ;
    for( i = 0; i < HostNum; i++ ) {
	DATATOLEN( wp, NameLen )	; wp += sizeof( int ) ;
	printf("HOST NAME:%s\n", wp )	; wp += NameLen ;
	DATATOLEN( wp, UserNum )	; wp += sizeof( int ) ;
	if( UserNum )
	    printf("USER NAME:" ) ;
	else
	    printf("ALL USER" ) ;
	for( j = 0; j < UserNum; j++ ) {
	    DATATOLEN( wp, NameLen ) ;
	    wp += sizeof( int ) ;
	    printf("%s ", wp ) ;
	    wp += NameLen ;
	}
	printf("\n\n") ;
    }
    rkc_finalize() ;
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
	    exit( 2 ) ;
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

usage()
{
    fprintf( stderr, "usage: ishost [-is | -irohaserver hostname]\n" ) ;
    fflush( stderr ) ;
    exit( 0 ) ;
}
