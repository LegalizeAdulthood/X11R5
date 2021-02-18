/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/vmiio.c,v 1.20 1991/09/12 23:22:11 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include <sys/types.h>
/* #include <sys/file.h> */
#include <sys/stat.h>
/* #include <sys/time.h> */
#include <sys/ioctl.h>
#include <sys/errno.h>
#include <setjmp.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef SYSV
#include <termio.h>
#endif /* SYSV */

#define TELOPTS
#include <arpa/telnet.h>

#include "X.h"
#include "Xos.h"
#include "Xproto.h"
#include "misc.h"
#include "extnsionst.h"
#include "opaque.h"

#include "MVEX.h"
#include "MVEXproto.h"
#include "videostr.h"
#include "vmi.h"
#include "vdev.h"

extern int errno;
extern char *Xcalloc();

extern int connect();
extern struct hostent *gethostbyname();

#ifdef VMI_NEED_IO_UTILS

/********************************************
 * Utilities for MVEX ddx devices.       *
 ********************************************/
static jmp_buf vmi_time_out_env;
int
vmi_time_out()
{
    longjmp(vmi_time_out_env, 1);
}

static int FSend(vdev, fmt, x0, x1,x2,x3,x4,x5,x6,x7,x8,x9)
VDEV *vdev;
char *fmt;
{
    char buf[ VDEV_BUFSIZ ];
    int length, wrote;

    sprintf(buf, fmt, x0, x1,x2,x3,x4,x5,x6,x7,x8,x9);
    length = strlen(buf);
    wrote = write(vdev->fd, buf, length);
    if (wrote != length)
        return(-1);
    else
        return(0);
}

int vmiDevTelnet(vdev, path)
VDEV *vdev;
char *path;
{
    struct hostent *haddr;
    struct sockaddr_in sock;
    struct servent *sp;
    int s, err;
    /*
     * Obtain the service info for telnet.
     */
    sp = getservbyname("telnet", "tcp");
    if (sp == NULL)
        return(-1);
    /*
     * First, try interpreting the address as an internet standard "dot"
     * form.  If that fails, look up the address as if it were a host name.
     */
    sock.sin_addr.s_addr = inet_addr(path);
    if (sock.sin_addr.s_addr != -1) {
        sock.sin_family = AF_INET;
    } else {
        haddr = gethostbyname(path);
        if (haddr) {
            sock.sin_family = haddr->h_addrtype;
            bcopy(haddr->h_addr, (caddr_t)&sock.sin_addr,
                    haddr->h_length);
        } else
            return(-1);
    }
    sock.sin_port = sp->s_port;
    /*
     * Create a socket to fiddle with.
     */
    s = socket (AF_INET, SOCK_STREAM, 0);
    if (s < 0)
        return(-1);
    /*
     * Try to connect to the address in question...
     */
    if (err = connect(s, (struct sockaddr *)&sock, sizeof(sock)))
        return(-1);

    vdev->fd = s;
    vdev->protocol == VDEV_TELNET;
    vdev->SendOK = TRUE;
    vdev->ReceiveOK = TRUE;
    return (s);
}

void PrintOption(direction, fmt, option)
char *direction, *fmt;
int option;
{
}

/*
 * Telnet receiver states for fsm
 */
#define TS_DATA         0
#define TS_IAC          1
#define TS_WILL         2
#define TS_WONT         3
#define TS_DO           4
#define TS_DONT         5

char    doopt[] = { IAC, DO, '%', 'c', 0 };
char    dont[] = { IAC, DONT, '%', 'c', 0 };
char    will[] = { IAC, WILL, '%', 'c', 0 };
char    wont[] = { IAC, WONT, '%', 'c', 0 };

/*
 * Returns bytes received.
 */
int TelnetReceive(vdev, received, data, length)
VDEV *vdev;
char *received, *data;
int length;
{
    char *dataStart = data;
    register int c;
    static int state = TS_DATA;
    while (length > 0) {
        c = *received++ & 0377, length--;
        switch (state) {
        case TS_DATA:
            if (c == IAC) {
                state = TS_IAC;
                continue;
            }
            /*
             * Deaf, dumb and blind.  We *assume* that if a carriage
             * return is sent, it is accompanied by a newline.
             * So drop the carriage return.
             */
            if (c != '\r')
                *data++ = c;
            continue;
        case TS_IAC:
            switch (c) {

            case IAC:
                *data++ = c;
                state = TS_DATA;
                continue;
            case WILL:
                state = TS_WILL;
                continue;

            case WONT:
                state = TS_WONT;
                continue;

            case DO:
                state = TS_DO;
                continue;
            case DONT:
                state = TS_DONT;
                continue;
            case DM:
                /* in telnet: ioctl(fileno(stdout), TIOCFLUSH, 0); */
                break;

            case NOP:
            case GA:
                break;
            default:
                break;
            }
            state = TS_DATA;
            continue;
        case TS_WILL:
            PrintOption("RCVD", will, c);
            FSend(vdev, dont, c);
            PrintOption("SENT", dont, c);
            state = TS_DATA;
            continue;

        case TS_WONT:
            PrintOption("RCVD", wont, c);
            FSend(vdev, dont, c);
            PrintOption("SENT", dont, c);
            state = TS_DATA;
            continue;


        case TS_DO:
            PrintOption("RCVD", doopt, c);
            FSend(vdev, wont, c);
            PrintOption("SENT", wont, c);
            state = TS_DATA;
            continue;

        case TS_DONT:
            PrintOption("RCVD", dont, c);
            FSend(vdev, wont, c);
            PrintOption("SENT", wont, c);
            state = TS_DATA;
            continue;
        }
    }
    return (data - dataStart);
}

int vmiDevRlogin(vdev, path)
VDEV *vdev;
char *path;
{
    struct passwd *pwd;
    struct servent *sp;
    int sock;

    if (strcmp(path, "none") == 0)
        return (-1);

    pwd = getpwuid(getuid());
    if (pwd == 0) {
            /* fprintf(stderr, "Who are you?\n"); */
            return(-1);
    }

    sp = getservbyname("login", "tcp");
    if (sp == 0) {
            /* fprintf(stderr, "rlogin: login/tcp: unknown service\n"); */
            return(-1);

    }

/***********************************
    signal(SIGPIPE, ConnectionGone);
    signal(SIGINT, Shutdown);
    signal(SIGQUIT, Shutdown);
***********************************/

    sock = rcmd(&path, sp->s_port, pwd->pw_name, pwd->pw_name, "", 0);
    if (sock < 0) {
          /*  FatalError("Cannot connect to %s\n", Abekas); */
            return(-1);
    }
    vdev->fd = sock;
    vdev->protocol == VDEV_RLOGIN;
    vdev->SendOK = TRUE;
    vdev->ReceiveOK = TRUE;
    return(vdev->fd);
}

int vmiDevOpenTty(vdev, tty, baudrate, flags)
VDEV *vdev;
char *tty;
int baudrate, flags;
{
    int ttyflag;
    int len = strlen(tty);
    char *ttydev;
#ifdef SYSV
    struct termio ttyold, ttynew;
#else
    struct sgttyb	tty_params;
#endif /* SYSV */
    int speed;
    struct itimerval value, ovalue, junkvalue;
    int (*prev_signal_fn)();

    if((speed = vmittyMakebaud(baudrate)) < 0 )
        return(-1);
    if((ttydev = Xcalloc(len + 1)) == NULL)
    {
        return(-1);
    }
    strcpy(ttydev, tty);
    vdev->dev_path = ttydev;

        /*
         * Disable timer and set signal handler for SIGALRM to "time_out".
         */
        value.it_interval.tv_sec = 0;
        value.it_interval.tv_usec = 0;
        value.it_value.tv_sec = 0;
        value.it_value.tv_usec = 0;
        if (setitimer(ITIMER_REAL, &value, &ovalue))
            assert(FALSE);
        prev_signal_fn = (int (*)())signal (SIGALRM, vmi_time_out);
        if ((int) prev_signal_fn == -1)
            assert(FALSE);
        /*
         * "time_out" will longjmp to here if the call to "open(2)" times-out
         */
        if (setjmp(vmi_time_out_env)) {
            (void) signal (SIGALRM, prev_signal_fn);
            if (setitimer(ITIMER_REAL, &ovalue, &junkvalue))
                assert(FALSE);
            return(-1);
        }
        value.it_value.tv_sec = vdev->timeout_sec;
        value.it_value.tv_usec = vdev->timeout_usec;
        if (setitimer(ITIMER_REAL, &value, &junkvalue))
            assert(FALSE);

#ifdef UTEKV
    vdev->fd = open( tty, O_RDWR, 0);
#else
    vdev->fd = open( tty, O_NDELAY | O_RDWR | O_EXCL, 0777 );
#endif /* UTEKV */

    /*
     * If we get here, the open has succeeded without timing out.
     * Re-establish the old signal handler and reset the old time value
     * in the timer.
     */
    (void) signal (SIGALRM, prev_signal_fn);
    if (setitimer(ITIMER_REAL, &ovalue, &junkvalue))
        assert(FALSE);

    if (vdev->fd < 0)
    {
        vdev->fd = 0;
        return(-1);
    }
#ifdef SYSV
    ioctl(vdev->fd, TCGETA, &ttyold);
    ioctl(vdev->fd, TCGETA, &ttynew);
    if(flags) {
        ttynew.c_lflag &= ~( ICANON | ECHO);
        if(flags & VMI7BIT)
            ttyflag = CS7;
        else
            ttyflag = CS8;
        if(flags & VMIODDPARITY)
            ttyflag |= (PARODD | PARENB);
        if(flags & VMIEVENPARITY)
            ttyflag |= PARENB;
        ttynew.c_cflag = (unsigned short) (speed | CLOCAL | CREAD) |
                         (unsigned short)ttyflag;
        ttynew.c_cc[4] = 1; /* Set to return after single char entered */
    }
    else /* default RAW, 8 bit, no parity */
    {
        ttynew.c_lflag &= ~( ICANON | ECHO);
        ttynew.c_cflag = (unsigned short) (speed | CS8 | CLOCAL | CREAD);
        ttynew.c_cc[4] = 1; /* Set to return after single char entered */
    }
    ioctl(vdev->fd, TCSETAW, &ttynew);
#else
    if (gtty(vdev->fd, &tty_params) == -1)
    {
	fprintf(stderr,"vmiDevOpenTty: gtty failed\n");
	return(-1);
    }
    if( !flags ) 
	ttyflag = RAW|TANDEM; /* default: no processing (no parity) */
    else {
	ttyflag = CBREAK;	/* semi-cooked */
        if(flags & VMIODDPARITY)
            ttyflag |= ODDP;
        if(flags & VMIEVENPARITY)
            ttyflag |= EVENP;
    }
    tty_params.sg_ospeed = speed;	/* the system setting */
    tty_params.sg_ispeed = speed;
    tty_params.sg_flags = ttyflag;

    if (stty(vdev->fd,&tty_params) == -1)
    {
	perror("vmiDevOpenTty: stty");
	return(-1);
    }
#endif
    
    vdev->protocol == VDEV_TTY;
    vdev->SendOK = TRUE;
    vdev->ReceiveOK = TRUE;
    return(vdev->fd);
}

void vmiDevClose(fd)
int fd;
{
    close(fd);
}

void vmiDevSend(vdev, msg)
VDEV *vdev;
char *msg;
{
    int len = strlen(msg);
    int wrote;
    struct timeval timeout;
    int nFound;
    u_long writeFds;

    /*****************
    if (!vdev->SendOK)
        return;
     *****************/

    timeout.tv_sec = vdev->timeout_sec;
    timeout.tv_usec = vdev->timeout_usec;
    writeFds = 1<<(vdev->fd);
    nFound = select(vdev->fd+1, NULL, &writeFds, NULL, &timeout);
    if (nFound <= 0) {
        /*********************
        vdev->SendOK = FALSE;
         ********************/
        if (errno == EINTR) {
            if (vdev->Verbose)
                printf("select on write interrupted\n");
            return;
        }
        FatalError("Cannot send message to %s after %d seconds: \"%s\"\n",
            vdev->fd, vdev->timeout_sec, msg);
    }
    if ((wrote = write(vdev->fd, msg, len)) != len) {
        vdev->SendOK = FALSE;
        if (errno == EINTR) {
            if (vdev->Verbose)
                printf("write interrupted\n");
            return;
        }
        FatalError("Only able to send %d out of %d bytes to %s: \"%s\"\n",
            wrote, len, msg);
    }
}

/*
 * Flush the input queue.
 */
int vmiDevFlushInput(vdev)
VDEV *vdev;
{
    int     i;
    int     num;
    char    c;

    if(ioctl(vdev->fd, FIONREAD, &num) == -1)
        return (-1);
    for(i=num; i>0; i--) {
        if(read(vdev->fd,&c,1) == -1)
            return(-1);
    }
    return (num);
}

/*
 * Read up to num_bytes, buff is the pointer to the input buffer.
 * If fill is TRUE, return only if buffer has num_bytes or timeout occurrs
 * Return either number of bytes read or -1 for timeout or error.
 */
int vmiDevReceive(vdev, buf, num_bytes, fill)
VDEV *vdev;
char *buf;
int  num_bytes;
int  fill;
{
    char *pSTR;
    struct timeval timeout;
    int nFound;
    int ret;
    int nRead = 0;
    int gotIt = FALSE;
    unsigned long readFds;
    char data[ VDEV_BUFSIZ ]; /* Temporary data holder for telnet protocol */

    /*********************
    if (!vdev->ReceiveOK)
        return;
     *********************/
    while (! gotIt) {
        timeout.tv_sec = vdev->timeout_sec;
        timeout.tv_usec = vdev->timeout_usec;
        readFds = 1<<vdev->fd;
        nFound = select(vdev->fd+1, &readFds, NULL, NULL, &timeout);
        if (nFound < 0 || (nFound == 0 && ! nRead)) {
            if (errno == EINTR) /* timeout */
                return(-1);
            else {
               /***********
                vdev->ReceiveOK = FALSE;
                FatalError("Select failure\n");
                ************/
		return(-1);
            }
        } 
        if ((ret = read(vdev->fd, &buf[nRead], num_bytes - nRead)) < 0) {
            return(-1);
            /*************************
            vdev->ReceiveOK = FALSE;
            FatalError("Read failure from %s\n", Tty);
            **************************/
        }
        if (ret == 0) {
            /***************************
            vdev->ReceiveOK = FALSE;
            if (vdev->Verbose)
                printf("got EOF\n");
            break;
            ***************************/
            return(-1);
        }
        if(vdev->protocol == VDEV_TELNET)
        {
            /*
             * Strip out Telnet Protocol data.
             */
            ret = TelnetReceive(vdev, &buf[nRead], data, ret);
            strncpy(data, &buf[nRead], ret);
        }
        nRead += ret;
        if(fill) {
            if(nRead == num_bytes)
                gotIt = TRUE;
        }
        else
            gotIt = TRUE;
    }
    return(0);	/* no error */
}


void vmiDevSendChar(vdev, c)
VDEV *vdev;
unsigned char c;
{
    char buf[2];

    buf[0] = c;
    buf[1] = '\0';	/* make a message */

    vmiDevSend(vdev, buf);
}

/*
 * Convert integer baud rate to system value
 */
#ifdef SYSV
#define SPEED_19_2	B19200
#define SPEED_38_4	B38400
#else
#define SPEED_19_2	EXTA
#define SPEED_38_4	EXTA
#endif
static int vmittyMakebaud(baudrate)
int baudrate;
{
	switch( baudrate ) {
		case 0:
			return (B0);
			break;
		case 50:
			return (B50);
			break;
		case 75:
			return (B75);
			break;
		case 110:
			return (B110);
			break;
		case 134:
			return (B134);
			break;
		case 150:
			return (B150);
			break;
		case 200:
			return (B200);
			break;
		case 300:
			return (B300);
			break;
		case 600:
			return (B600);
			break;
		case 1200:
			return (B1200);
			break;
		case 1800:
			return (B1800);
			break;
		case 2400:
			return (B2400);
			break;
		case 4800:
			return (B4800);
			break;
		case 9600:
			return (B9600);
			break;
		case 19200:
			return (SPEED_19_2);
			break;
		case 38400:
			return (SPEED_38_4);
			break;
		default:
			return (-1);
			break;		
		}
	/*NOTREACHED*/
}

void
vmiPause( interval )
    int	interval;
{
    struct timeval to;

    to.tv_sec = 0;
    to.tv_usec = interval * 1000;
    select(0,0,0,0,&to);
}


void vmiBlockCurrentRequest(client)
    ClientPtr client;
{
    --client->sequence;
    ResetCurrentRequest(client);
    IgnoreClient(client);
}

void vmiUnblockCurrentRequest(client)
    ClientPtr client;
{
    AttendClient(client);
}

/*
 *   FiOpenForRead, FiRead, FiClose,
 */

FID
vmiOpenForRead( lenn, name)
    int		lenn;
    char 	*name;
{
    FID		fid;
    char	*unixfname = name;

    if ( name[lenn-1] != '\0')
    {
	unixfname = (char *) ALLOCATE_LOCAL( lenn+1);
	strncpy( unixfname, name, lenn);
	unixfname[lenn] = '\0';
    }
    else
	unixfname = name;

    fid = (FID)fopen( unixfname, "r");

    if(unixfname != name)
        DEALLOCATE_LOCAL(unixfname);
    return fid;
}


/*
 * returns 0 on error
 */
int
vmiRead( buf, itemsize, nitems, fid)
    char	*buf;
    unsigned	itemsize;
    unsigned	nitems;
    FID		fid;
{
    return fread( buf, itemsize, nitems, (FILE *)fid);
}

int
vmiClose(fid)
    FID		fid;
{
    return fclose( (FILE *)fid);
}

#endif /* VMI_NEED_IO_UTILS */
