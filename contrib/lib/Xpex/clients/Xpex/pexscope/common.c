/* $Header: common.c,v 2.2 91/09/11 15:52:05 sinyaw Exp $ */
#ifndef lint
static char     sccsid[] = "@(#)common.c 1.4 90/04/05 MIT/SMI";
#endif

/***********************************************************
Copyright 1990 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* ************************************************************ *\
 *								*
 *    Common support routines for sockets			*
 *								*
 *       James L. Peterson	 				*
 *	(c) Copyright MCC, 1987                                 *
 * 				  				*
 * 				  				*
 \* *********************************************************** */

#include "scope.h"
#if defined(SYSV) || defined(SYSTYPE_SYSV) || defined(SVR4)
#include <sys/filio.h>
#endif /* SYSV */

/* ********************************************** */
/*						  */
/*       Debugging support routines               */
/*						  */
/* ********************************************** */

enterprocedure(s)
     char   *s;
{
  debug(2,(stderr, "-> %s\n", s));
}

warn(s)
     char   *s;
{
  fprintf(stderr, "####### %s\n", s);
}

panic(s)
     char   *s;
{
  fprintf(stderr, "%s\n", s);
  exit(1);
}

/* ********************************************** */
/*						  */
/*  Debugging forms of memory management          */
/*						  */
/* ********************************************** */

extern char *malloc();

char   *Malloc (n)
     long    n;
{
  char   *p;
  p = (char *)malloc((unsigned int)n);
  debug(64,(stderr, "%x = malloc(%d)\n", p, n));
  if (p == NULL)
    panic("no more malloc space");
  return(p);
}

Free(p)
     char   *p;
{
  debug(64,(stderr, "%x = free\n", p));
  free(p);
}



/* ************************************************************ */
/*								*/
/*    Signal Handling support					*/
/*								*/
/* ************************************************************ */

#include <signal.h>

SignalURG()
{
  debug(1,(stderr, "==> SIGURG received\n"));
}

SignalPIPE()
{
  debug(1,(stderr, "==> SIGPIPE received\n"));
}

SignalINT()
{
  debug(1,(stderr, "==> SIGINT received\n"));
  exit(1);
}

SignalQUIT()
{
  debug(1,(stderr, "==> SIGQUIT received\n"));
  exit(1);
}

SignalTERM()
{
  debug(1,(stderr, "==> SIGTERM received\n"));
  exit(1);
}

SignalTSTP()
{
  debug(1,(stderr, "==> SIGTSTP received\n"));
}

SignalCONT()
{
  debug(1,(stderr, "==> SIGCONT received\n"));
}

SignalUSR1()
{
  extern char ScopeEnabled;

  debug(1,(stderr, "==> SIGCONT received\n"));
  ScopeEnabled = ! ScopeEnabled;
}

SetSignalHandling()
{
  extern char HandleSIGUSR1;

  enterprocedure("SetSignalHandling");
  (void)signal(SIGURG, SignalURG);
  (void)signal(SIGPIPE, SignalPIPE);
  (void)signal(SIGINT, SignalINT);
  (void)signal(SIGQUIT, SignalQUIT);
  (void)signal(SIGTERM, SignalTERM);
  (void)signal(SIGTSTP, SignalTSTP);
  (void)signal(SIGCONT, SignalCONT);
  if (HandleSIGUSR1)
    (void)signal(SIGUSR1, SignalUSR1);
}



/* ************************************************************ */
/*								*/
/*   Create a socket for a service to listen for clients        */
/*								*/
/* ************************************************************ */

#include <sys/types.h>	       /* needed by sys/socket.h and netinet/in.h */
#include <sys/uio.h>	       /* for struct iovec, used by socket.h */
#include <sys/socket.h>	       /* for AF_INET, SOCK_STREAM, ... */
#include <sys/ioctl.h>	       /* for FIONCLEX, FIONBIO, ... */
#include <netinet/in.h>	       /* struct sockaddr_in */
#include <netdb.h>	       /* struct servent * and struct hostent *  */

static int  ON = 1 /* used in ioctl */ ;
#define	BACKLOG	5

/* for use in the UsingFD call -- defined later */
extern int  NewConnection ();


SetUpConnectionSocket(port)
     int     port;
{
  FD ConnectionSocket;
  struct sockaddr_in  sin;
#ifndef	SO_DONTLINGER
  struct linger linger;
#endif	SO_DONTLINGER

  enterprocedure("SetUpConnectionSocket");

  /* create the connection socket and set its parameters of use */
  ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (ConnectionSocket < 0)
    {
      perror("socket");
      exit(-1);
    }
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_REUSEADDR,   (char *)NULL, 0);
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_USELOOPBACK, (char *)NULL, 0);
#ifdef	SO_DONTLINGER
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_DONTLINGER,  (char *)NULL, 0);
#else	SO_DONTLINGER
  linger.l_onoff = 0;
  linger.l_linger = 0;
  (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_LINGER, (char
*)&linger, sizeof linger);
#endif	SO_DONTLINGER

  /* define the name and port to be used with the connection socket */
  bzero((char *)&sin, sizeof(sin));
  sin.sin_family = AF_INET;

  /* the address of the socket is composed of two parts: the host machine and
     the port number.  We need the host machine address for the current host
   */
  {
    /* define the host part of the address */
    char    MyHostName[256];
    struct hostent *hp;

    (void) gethostname(MyHostName, sizeof(MyHostName));
    ScopeHost = (char *) Malloc((long)strlen(MyHostName));
    (void)strcpy(ScopeHost, MyHostName);
    hp = gethostbyname(MyHostName);
    if (hp == NULL)
      panic("No address for our host");
    bcopy((char *)hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
  }
    /* new code -- INADDR_ANY should be better than using the name of the
       host machine.  The host machine may have several different network
       addresses.  INADDR_ANY should work with all of them at once. */
  sin.sin_addr.s_addr = INADDR_ANY;

  sin.sin_port = port;
  ScopePort = port;

  /* bind the name and port number to the connection socket */
  if (bind(ConnectionSocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      perror("bind");
      exit(-1);
    }

  debug(4,(stderr, "Socket is FD %d for %s,%d\n",
	   ConnectionSocket, ScopeHost, ScopePort));

  /* now activate the named connection socket to get messages */
  if (listen(ConnectionSocket, BACKLOG) < 0)
    {
      perror("listen");
      exit(-1);
    };

  /* a few more parameter settings */
  (void)ioctl(ConnectionSocket, FIOCLEX, 0);
  (void)ioctl(ConnectionSocket, FIONBIO, &ON);

  debug(4,(stderr, "Listening on FD %d\n", ConnectionSocket));
  UsingFD(ConnectionSocket, NewConnection);
}


