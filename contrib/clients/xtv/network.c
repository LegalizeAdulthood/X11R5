/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

/*
	network.c
	Nwtwork-related routines

*/

#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/time.h>
#include "memory.h"
#include "network.h"

#define	DEBUGConnectTo		0&DEBUGON
#define	DEBUGSendData		0&DEBUGON
#define DEBUGTimedSendData      0&DEBUGON
#define	DEBUGSendDataError	0&DEBUGON
#define	DEBUGReadData		0&DEBUGON
#define	DEBUGSendByte		0&DEBUGON



extern char *sys_errlist[];	/* Grrr! */



/*
 *	ConnectTo()
 *
 *	Open an INET socket and connect to the host name using the port passed in.
 *	If HostName is NULL, get the current host name and use it.
 *
 *	RETURN VALUE:	File descriptor for socket
 *			ERROR if there were any problems
 */
Socket ConnectTo( HostName, Port )
char   *HostName;
int   Port;
{
	int			NewSocket;
	struct sockaddr_in	HostAddr;
	char			local_name[HOST_SIZE];
	struct hostent		*HostInfo;

#	if DEBUGConnectTo
	ErrorMessage( "ConnectTo( %s, %d )\n", HostName, Port );
#	endif

	/* Get a file descriptor from the system */

	if ( (NewSocket=socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
		return ERROR;
	}

	/* Call the name server and get the full poop on our host */

	if  ( (HostInfo = gethostbyname(HostName)) == NULL ) {
		HostAddr.sin_addr.s_addr = inet_addr(HostName);
		if ((HostInfo = gethostbyaddr(&HostAddr.sin_addr.s_addr,
		    sizeof(HostAddr.sin_addr.s_addr),AF_INET))== NULL) {
			ErrorMessage( "ConnectTo(): Can't find host \"%s\"\n", HostName );
			return ERROR;
		}
	}
	bcopy ( HostInfo->h_addr, &(HostAddr.sin_addr), HostInfo->h_length);

	/* Build the host address and take a stab at connecting to it */

#	if DEBUGConnectTo
	ErrorMessage( "ConnectTo(): Trying to connect to %s:%d\n", HostInfo->h_name, Port );
#	endif
	HostAddr.sin_family = AF_INET;
	HostAddr.sin_port = Port;
	if (connect( NewSocket, &HostAddr, sizeof(HostAddr) )<0) {
		close( NewSocket );
		ErrorMessage( "Unable to connect to %s: %s\n", HostName, sys_errlist[errno] );
		return( ERROR );
	}

#	if DEBUGConnectTo
	ErrorMessage( "ConnectTo(): Connected\n" );
#	endif
	return NewSocket;
}



/*
   OpenListener

   Create a non-blocking socket on "port" and set it to listen for up to
   "listeners" listeners.  Return a file descriptor or ERROR is a problem
   crops up.
*/
int OpenListener( port, listeners, name )
int   port;
int   listeners;
struct sockaddr_in *name;
{
	int         NewSocket;
	struct   sockaddr   from;
	int         length;



	/* Try and create a new socket */
	if ((NewSocket=socket( AF_INET, SOCK_STREAM, 0)) < 0) {
		return( ERROR );
	}

	/* Prepare the usual information and try to bind it */

	name->sin_family = AF_INET;
	name->sin_addr.s_addr = INADDR_ANY;
	name->sin_port = port;
	if ( bind( NewSocket, name, sizeof(*name) ) ) {
		close( NewSocket );
		return( ERROR );
	}


	/* SocketInfo(NewSocket); */

	if (listen( NewSocket, listeners )) {
		return( ERROR );
	}
	return(NewSocket);
}


/*
 *   BYTE *ReadData
 *
 *      Read "size" bytes from the socket described by the file
 *   descriptor "source".  Any problems will cause a termination.
 *   Block if "block" is TRUE.
 *
 *   Blocking is "true"... That is, only the exact amount of data
 *   requested for reception will cause a successful read.  Blocking
 *   is achieved by testing data with MSG_PEEK.
 *   
 *
 *   RETURN VALUE:   Pointer to data read,
 */
BYTE *ReadData( source, size, block )
int source;
int size;
BOOL block;
{
	BYTE   *buffer, *buffer_p;
	int   recv_size;

	if (block == DONTBLOCK)
		fcntl( source, F_SETFL, FNDELAY );

	buffer = (BYTE *) GetMem( size );
	recv_size = 0;
	buffer_p = buffer;

	/* If we're in non-blocking mode, do a peek */

	if (block == DONTBLOCK) {
		if ( recv(source, buffer_p, size, MSG_PEEK) != size) {
			free( buffer);
			return( NULL );
		}
		for ( ; ( size > 0) ; ) {
			recv_size = recv (source, buffer_p, size, 0);
			if (recv_size > 0) {
				size -= recv_size;
				buffer_p += recv_size;
			}
		}
	}
	else {
		for ( ; ( size > 0) ; ) {
#       if DEBUGReadData
	fprintf( stderr, "ReadData(): going to recv.......\n");
#       endif
			if ( (recv_size = recv (source, buffer_p, size, 0)) < 0){
				if (errno != EWOULDBLOCK) {
#       if DEBUGReadData

					perror ("perror: Receiving Data:\n");
#       endif
}

			}
			if (recv_size > 0) {
				size -= recv_size;
				buffer_p += recv_size;
			}
                        if (recv_size == 0) {
#       if DEBUGReadData
	fprintf( stderr, "ReadData(): going to recv.......\n");
#       endif
			return(NULL);
			}
#       if DEBUGReadData
			fprintf( stderr, "ReadData(): got %d bytes from %d\n", recv_size, source);
#       endif
		}
	}

	return( buffer );
}

static  int     pid;
static  jmp_buf env;

/*
 *   TimedSendData;
 *
 *   Send a "size" bytes of data pointed to by "buffer" over the
 *   socket whose file descriptor is "dest" within a limited time.
 *
 *   RETURN VALUE:   None.
 */
TimedSendData( dest, buffer, size )
int dest;
BYTE *buffer;
int size;
{
        int sec = 20, onalarm();
        union wait *status;

        signal(SIGALRM, onalarm);
        alarm(sec);
        if (setjmp(env) == 0) {

                int sent;
                BYTE *buffer_p;

                buffer_p = buffer;
                sent = 0;

#       if DEBUGTimedSendData
        fprintf( stderr, "TimedSendData(): sending %d bytes to %d\n", size, dest);
#       endif


                for (;size > 0;) {
                        sent = send( dest, buffer_p, size, 0);
                        if (sent >= 0) {
                                size -= sent;
                                buffer_p += sent;
                        }
                        else
                                if (errno != EWOULDBLOCK) {
                                perror ("perror: SendData:");
#       if DEBUGTimedSendData
                                fprintf( stderr, "TimedSendData(): ERROR, sending %d bytes to %d: %s\n",
                                    size, dest, sys_errlist[errno]);
#       endif
                                        signal(SIGALRM, SIG_IGN);
                                        return(ERROR);
                                }
                }
#       if DEBUGTimedSendData
                fprintf( stderr, "TimedSendData():send finished normally\n");
#       endif
                signal(SIGALRM, SIG_IGN);
        /*      return(0);*/
        }
        else {
#       if DEBUGTimedSendData
        fprintf( stderr, "TimedSendData():participant not responded and will be
dropped\n");
#       endif
                return(TIMEOUT);
        }
}

onalarm()
{
#       if DEBUGTimedSendData
        fprintf( stderr, "TimedSendData():a SIG_ALRM is caught!\n");
#       endif
        longjmp(env, 1);
}



/*
 *   SendData;
 *
 *   Send a "size" bytes of data pointed to by "buffer" over the
 *   socket whose file descriptor is "dest".
 *
 *   RETURN VALUE:   None.
 */
SendData( dest, buffer, size )
int dest;
BYTE *buffer;
int size;
{
	int sent;
	BYTE *buffer_p;

	buffer_p = buffer;
	sent = 0;

#       if DEBUGSendData
	fprintf( stderr, "SendData(): sending %d bytes to %d\n", size, dest );
#       endif


	for (;size > 0;) {
		sent = send( dest, buffer_p, size, 0);
		if (sent >= 0) {
			size -= sent;
			buffer_p += sent;
		}
		else 
			if (errno != EWOULDBLOCK) {
#       if DEBUGSendDataError
				perror ("perror: SendData:");
				fprintf( stderr, "SendData(): ERROR, sending %d bytes to %d: %s\n", 
				    size, dest, sys_errlist[errno]);
#       endif
				return (ERROR);
			}
	}

}


/*
 *   void SendByte
 *
 *   Send a single byte over the network.
 *
 *   RETURN VALUE:   (void)
 */
SendByte( Dest, WhatByte )
int Dest;
BYTE WhatByte;
{
int ReturnCode;
 ReturnCode = SendData( Dest, &WhatByte, 1 );
# if DEBUGSendByte
	fprintf(stderr,"SendByte(): ReturnCode is %s\n", 
		(ReturnCode==ERROR)? "ERROR":"OK"); 
# endif

 return(ReturnCode);
}


/*
 *   BYTE GetByte
 *
 *   Get a single byte from source
 *
 *   RETURN VALUE:   Byte received
 *         NothingAvail otherwise
 */
BYTE GetByte( Source )
int Source;
{
	BYTE   *SingleByte;
	BYTE   TheByte;

	if ((SingleByte=ReadData(Source,1,BLOCK))==NULL) return (ERROR);
	else{
	TheByte = *SingleByte;
	free( SingleByte );
	return( TheByte );
	}
}


/*
 *   BYTE CheckByte
 *
 *   Get a single byte from source
 *
 *   RETURN VALUE:   Byte received
 *         NothingAvail otherwise
 */
BYTE CheckByte( Source )
int Source;
{
	BYTE   *SingleByte;
	BYTE   TheByte;

	if ((SingleByte=ReadData(Source,1,DONTBLOCK)) != NULL) {
		TheByte = *SingleByte;
		free( SingleByte );
		return( TheByte );
	} else {
		return( NothingAvail );
	}
}


void RemoteInfo(sd)
int sd;
{
	struct sockaddr_in from;
	int fromlen;
	struct  hostent *HostInfo, *gethostbyaddr();

	fromlen = sizeof(from);
	if (getpeername(sd,&from,&fromlen)<0){
		perror("could't get peername\n");
		exit(1);
	}
	printf("Connected to:");
	printf("%s:%d", inet_ntoa(from.sin_addr),
	    ntohs(from.sin_port));
	if ((HostInfo = gethostbyaddr(&from.sin_addr.s_addr,
	    sizeof(from.sin_addr.s_addr),AF_INET)) == NULL)
		fprintf(stderr, " RemoteInfo(): Can't find host %s\n", inet_ntoa(from.sin_addr));
	else
		printf("(%s)\n", HostInfo->h_name);
}
SocketInfo(sd)
int sd;
{
	struct sockaddr_in name;
	int length;
	length = sizeof(name);
	if (getsockname( sd, &name, &length)) {
		return( ERROR );
	}

	printf(" Port: %d\n", ntohs(name.sin_port));

}
