/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/errormon.c,v 2.22 1991/09/12 16:06:34 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/errormon.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/errormon.c,v 2.22 1991/09/12 16:06:34 bobg Exp $";
#endif /* lint */

/* 
 ***************************************************************
 * Routines for monitoring messages on console service
 * and (on some machines) /dev/console.
 * Some routines swiped from ttyscript.
 ****************************************************************
 */

#include <class.h>
#include <im.ih>
#include <conclass.ih>
#include <environ.ih>
#include <console.h>
#include <sitevars.h>
#include <ctype.h>
#include <andrewos.h> /* sys/types.h sys/time.h sys/file.h */
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/* #include <sys/tty.h> */
#include <fcntl.h>
#include <util.h>

#if SY_AIX221
#include <sys/devinfo.h>
#include <sys/pty.h>
#include <sys/tty.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/termio.h>
#endif /* #if SY_AIX221 */

#ifdef ibm032
/* 
 * Should do the following but include files are wrong
 * #include </sys/cacons/screen_conf.h>
 * So instead we do the following hack:
 */
#define TIOCSCONS _IO(t, 104)
#endif /* ibm032 */

extern int ForceErrorMonitoring, InhibitErrorMonitoring;

extern CheckErrorsIn();  
extern CheckConsoleSocket();

int ConsoleSocket;
struct sockaddr_in ConsoleAddr;
int SubChannel;
char ptyname[30];
char ThisHostName[256]="";
u_long ThisHostAddr;

#define MAXERRORRESTARTS 2

RestartErrorMonitoring(self, msg)
    struct consoleClass *self;
    char *msg;
{
    char ErrTxt[256];
    static int ErrorRestarts = 0;

    mydbg(("entering: RestartErrorMonitoring\n"));
    if (++ErrorRestarts > MAXERRORRESTARTS) {
        sprintf(ErrTxt, "console: Error monitoring terminated: %s", msg);
        ReportInternalError(self, ErrTxt);
	ErrorsIn = NULL;
        return;
    } else {
        sprintf(ErrTxt,"console: An error message may have been lost: %s", msg);
        ReportInternalError(self, ErrTxt);
        fclose(ErrorsIn);
        close(SubChannel);
        InitErrorMonitoring(self, FALSE);
    }
}
InitErrorMonitoring(self,FirstTime)
    struct consoleClass *self;
    boolean FirstTime;
{
    int tempfd;
    int PtyNumber = 0;
    int flags;
    char ptyname[64];
    
    mydbg(("entering: InitErrorMonitoring\n"));
    if (InhibitErrorMonitoring || environ_Get("WMHOST") && !ForceErrorMonitoring) {
        ReportInternalError(self, "console: Not monitoring errors on remote machine");
        return(-1);
    }
    if (FirstTime && OpenConsoleSocket(self)) return(-1);
#ifdef hpux /* **JG added 89-01-14 */
    ReportInternalError(self, "Warning: can't intercept \"/dev/console\" on HP-UX");
#else /* hpux */
    SubChannel = open (_SITE_DEV_TTY, 2);
    if (SubChannel >= 0) {
        ioctl (SubChannel, TIOCNOTTY, 0);
        close (SubChannel);
    }

    if (! GetPtyandName(&SubChannel, &tempfd, ptyname, sizeof(ptyname)))  {
        sprintf(ErrTxt, "console: Incomplete error monitoring: Can't open pty %s %s", ptyname, sys_errlist[errno]);
        ReportInternalError(self, ErrTxt);
	return(-1);
    }
#ifdef	TIOCCONS
    {
	int ON = 1;
	if (ioctl (tempfd, TIOCCONS, &ON) < 0) {
            ReportInternalError(self, "console: Incomplete error monitoring: ioctl (TIOCSCONS) failed");
	    return(-1);
	}
    }
#else /* TIOCCONS */
#if (defined(vax) || defined(MIPSEL))
      if ((SubChannel = open(_SITE_INTERCEPT, O_RDONLY)) < 0) {
	  if ((SubChannel = open("/dev/smscreen", O_RDONLY)) < 0){
	      sprintf(ErrTxt,"console: Incomplete error monitoring (open '%s' (and '/dev/smscreen') failed)", _SITE_INTERCEPT);
	      ReportInternalError(self, ErrTxt);
	      return(-1);
	  }
      }
#else /* vax || MIPSEL */
      {
#if SY_AIX221
	  int ON = REMOTE;
	  if (ioctl (SubChannel, PTYSETM, &ON) < 0) {
	      ReportInternalError(self, "console: Incomplete error monitoring: ioctl() failed on PTYSETM");
	      return(-1);
	  }
	  switch (fork()) {
	      case -1:
		  ReportInternalError(self, "console: Incomplete error monitoring: fork of AIX console logger failed.");
		  return(-1);
	      case 0:
		  /* child */
		  break;
	      default:
		  /* parent */
		  {
		  int con;
		  int msgqid, msgsize;
		  char iobuf[100];
		  struct tlog tl;
		  struct {
		      mtyp_t	mtype;		/* message type */
		      char	mtext[4096];	/* message text */
		  } msg;

		  dup2(tempfd, 1);
		  dup2(tempfd, 2);
		  setvbuf(stdout, iobuf, _IOLBF, sizeof(iobuf));
		  setbuf(stderr, NULL);
		  if ((con = open("/dev/console", O_RDWR)) < 0) {
		      perror("console: Incomplete error monitoring: open /dev/console failed.");
		      exit(1);
		  }
		  /* Set logging to our message queue. */
		  if ((msgqid = msgget(IPC_PRIVATE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
		      perror("console: Incomplete error monitoring:  no msg queue.");
		      exit(1);
		  }
		  tl.tl_flags = TLOG_ON|TLOG_QID;
		  tl.tl_msgqid = msgqid;
		  if (ioctl(con, TCLOG, &tl) < 0) {
		      perror("console:  Incomplete error monitoring: msg queueing failed.");
		      exit(1);
		  }
		  while (1) {
		      if ((msgsize = msgrcv(msgqid, &msg, sizeof(msg.mtext), 0, MSG_NOERROR)) < 0) {
			  perror("console:  Incomplete error monitoring: msg receive error.");
			  exit(1);
		      }
		      msg.mtext[msgsize] = '\0';
		      printf("%s", msg.mtext);
		  }
		  }
	  }
#else /* #if SY_AIX221 */
	  int ON = 1;
	  if (ioctl (SubChannel, TIOCREMOTE, &ON) < 0) {
	      ReportInternalError(self, "console: Incomplete error monitoring: ioctl (TIOCREMOTE) failed on TIOCREMOTE");
	      return(-1);
	  }
#endif /* #if SY_AIX221 */
      }
#endif /* vax */
#endif /* TIOCCONS */
#if SY_AIX31
	/* redirect console to the pty. */
	{
	  char buf[256];
	  sprintf(buf, "/etc/swcons %s > /dev/null", ptyname);
	  system(buf);
        }
#endif
	ErrorsIn = fdopen(SubChannel, "r");
	flags = fcntl(fileno(ErrorsIn), F_GETFL, 0);
	fcntl(fileno(ErrorsIn), F_SETFL, flags | FNDELAY);
    im_AddFileHandler(ErrorsIn, CheckErrorsIn, self, 2);
    return(0);
#endif /* hpux */
}

maptolower(str)
char str[];
{
    int i;
    for (i = 0; i < strlen(str); i++){
	if (isupper(str[i])){
	    str[i] = tolower(str[i]);
	}
    }
}
    

extern struct hostent *gethostbyname();
extern int make_socket();

OpenConsoleSocket(self)
    struct consoleClass *self;
{
    struct hostent *hent;
    int flags;

    errno = 0;
    mydbg(("entering: OpenConsoleSocket\n"));
    if (gethostname(ThisHostName, 256) < 0){
	if (self){/* I'm not sure if self is ever valid at this stage? */
	    ReportInternalError(self, "console:<OpenConsoleSocket> gethostname failed");
	}
	else{
	    arrgh(("console:<OpenConsoleSocket> gethostname failed"));
	}
	return;
    }
    maptolower(ThisHostName);
    hent = gethostbyname(ThisHostName);
    if (hent == 0) {
	if (self){/* I'm not sure if self is ever valid at this stage? */
	    ReportInternalError(self, "console:<OpenConsoleSocket> gethostbyname failed");
	}
	else{
	    arrgh(("console:<OpenConsoleSocket> gethostbyname failed"));
	}
	return;
    }
    strcpy(ThisHostName, hent->h_name);
    bcopy(hent->h_addr, &ThisHostAddr, sizeof(ThisHostAddr));

    if ((ConsoleSocket = make_socket(self, _SITE_CONSOLE_SOCKET)) < 0){
	if (self){/* I'm not sure if self is ever valid at this stage? */
	    ReportInternalError(self, "console:<OpenConsoleSocket> Incomplete error monitoring: bind to ConsoleSocket failed");
	}
	else{
	    arrgh(("console:<OpenConsoleSocket> Incomplete error monitoring: bind to ConsoleSocket failed"));
	}
	return;/* should I be setting some global flag ? */
    }
    flags = fcntl(ConsoleSocket, F_GETFL, 0);
    if (fcntl(ConsoleSocket, F_SETFL, flags | FNDELAY) < 0){
	if (self){
	    ReportInternalError(self, "console:<OpenConsoleSocket> fcntl on ConsoleSocket failed");
	}
	else {
	    arrgh(("console:<OpenConsoleSocket> fcntl on ConsoleSocket failed"));
	}
    }
    ConsoleIn = fdopen(ConsoleSocket, "r");
    if (ConsoleIn == NULL){
	if (self){/* I'm not sure if self is ever valid at this stage? */
	    ReportInternalError(self, "console:<OpenConsoleSocket> fdopen of ConsoleIn failed");
	}
	else{
	    arrgh(("console:<OpenConsoleSocket> fdopen of ConsoleIn failed"));
	}
	return;
    }
    im_AddFileHandler(ConsoleIn, CheckConsoleSocket, self, 3);
    return(0);
}

ReportInternalError(self, string)
    struct consoleClass *self;
    char *string;
{
    mydbg(("entering: ReportInternalError\n"));
    AnotherError(self,string, TRUE);
}
