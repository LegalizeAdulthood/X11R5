/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * Project: XMON - An X protocol monitor
 *
 * File: main.c
 *
 * Description: Contains main() for xmond
 *
 */

#include <sys/types.h>	       /* needed by sys/socket.h and netinet/in.h */
#include <sys/uio.h>	       /* for struct iovec, used by socket.h */
#include <sys/socket.h>	       /* for AF_INET, SOCK_STREAM, ... */
#include <sys/ioctl.h>	       /* for FIONCLEX, FIONBIO, ... */
#ifdef SYSV
#include <sys/fcntl.h>
#ifndef FD_CLOEXEC
#define FD_CLOEXEC 1
#endif
#endif
#include <netinet/in.h>	       /* struct sockaddr_in */
#include <netdb.h>	       /* struct servent * and struct hostent * */
#include <errno.h>	       /* for EINTR, EADDRINUSE, ... */
#include <signal.h>

#include "common.h"

#include "linkl.h"
#include "xmond.h"
#include "select_args.h"
#include "commands.h"

#define BACKLOG			5
#define XBasePort		6000
#define NUM_REQUESTS		/* 128 */ 256
#define NUM_EVENTS		/* 40 */ 256

#define Strleneq(a,b) Strneq(a, b, strlen(b))

/* function prototypes: */
/* main.c: */
static void ScanArgs P((int argc , char **argv ));
static void SetUpConnectionSocket P((int iport ));
static void SetSignalHandling P((void ));
static void MainLoop P((void ));
static Bool SendBuffer P((int fd , Buffer *buffer ));
static short GetServerport P((void ));
static short GetScopePort P((void ));
static void Usage P((void ));
static void ReadStdin P((Pointer private_data ));
static void do_command P((char *ptr ));
static void NewConnection P((Pointer private_data ));
static void DataFromClient P((Pointer private_data ));
static void DataFromServer P((Pointer private_data ));
static Bool ReadAndProcessData P((Pointer private_data , FDDescriptor
*read_fdd , FDDescriptor *write_fdd , Bool is_server ));
static void SaveBytes P((Buffer *buffer , char *buf , long n ));
static void RemoveSavedBytes P((Buffer *buffer , int n ));
static int ConnectToClient P((int ConnectionSocket ));
static int ConnectToServer P((char *hostName ));
static char *OfficialName P((char *name ));
static void SignalURG P((void ));
static void SignalPIPE P((void ));
static void SignalINT P((void ));
static void SignalTERM P((void ));
static void SignalTSTP P((void ));
static void SignalCONT P((void ));

#if (mskcnt>4)
static Bool ANYSET P((long *src));
#endif

extern char *getenv();

/* end function prototypes */

Global Bool    ignore_bytes;
Global int     RequestVerbose = 0;	/* verbose level for requests */
Global int     EventVerbose = 0;	/* verbose level for events */
Global int     ReplyVerbose = 0;	/* verbose level for replies */
Global int     ErrorVerbose = 1;	/* verbose level for error */
Global Bool    VerboseRequest[NUM_REQUESTS];/* requests that are very
verbose */
Global Bool    VerboseEvent[NUM_EVENTS];    /* events that are very verbose */
Global Bool    BlockRequest[NUM_REQUESTS];  /* do not transmit these
requests */
Global Bool    BlockEvent[NUM_EVENTS];	    /* do not transmit these events */
Global Bool    MonitoringRequests = True;   /* monitor selected requests? */
Global Bool    MonitoringEvents = True;	    /* monitor selected events? */
Global Bool    BlockingRequests = True;	    /* block selected requests? */
Global Bool    BlockingEvents = True;	    /* block selected events? */
Global int     SelectedRequestVerbose = 3;  /* selected requests verboseness */
Global int     SelectedEventVerbose = 3;    /* selected errors verboseness */
Global char    ServerHostName[255];
Global char    *LocalHostName;
Global int     debuglevel = 0;
Global LinkList client_list;	    /* list of Client */
Global FDDescriptor *FDD;
Global long	ReadDescriptors[mskcnt];
Global long	WriteDescriptors[mskcnt];
Global short	HighestFD;
Global Bool	littleEndian;

char		    save_buf[2048];
int		    saved_bytes = 0;

static long	    ClientNumber = 0;
static int		ServerPort = 0;
static int		ListenForClientsPort = 1;
static Bool	ShowPacketSize = False;	    /* show size of net packets? */

Global void
main(argc, argv)
	int	argc;
	char  **argv;
{
    int i;

    ScanArgs(argc, argv);
    InitializeFD();
    InitializeX11();
    InitRequestCount();
    InitEventCount();
    InitErrorCount();
    (void)UsingFD(fileno(stdin), ReadStdin, (Pointer)fileno(stdin));
    SetUpConnectionSocket(GetScopePort());
    SetSignalHandling();
    for (i = 0; i < NUM_REQUESTS; i++)
	BlockRequest[i] = VerboseRequest[i] = False;
    for (i = 0; i < NUM_EVENTS; i++)
	BlockEvent[i] = VerboseEvent[i] = False;
    initList(&client_list);
    MainLoop();
}

Global void
CloseConnection(client)
    Client		    *client;
{
    Server		    *server;

    if (client->fdd->outBuffer.BlockSize > 0)
	Tfree(client->fdd->outBuffer.data);
    if (client->fdd->inBuffer.BlockSize > 0)
	Tfree(client->fdd->inBuffer.data);
    close(client->fdd->fd);
    NotUsingFD(client->fdd->fd);

    server = (Server *)(TopOfList(&client->server_list));
    if (server->fdd->outBuffer.BlockSize > 0)
	Tfree(server->fdd->outBuffer.data);
    if (server->fdd->inBuffer.BlockSize > 0)
	Tfree(server->fdd->inBuffer.data);
    close(server->fdd->fd);
    NotUsingFD(server->fdd->fd);
    freeList(&client->server_list);
    freeMatchingLeaf(&client_list, (Pointer *)client);
    debug
    (
	1,
	(
	    stderr, "CloseConnection: client = %d, server = %d\n",
	    client->fdd->fd, server->fdd->fd
	)
    );
}

static void
ScanArgs(argc, argv)
	int	argc;
	char  **argv;
{
    int i;

    ServerHostName[0] = '\0';
    /* Scan argument list */
    for (i = 1; i < argc; i++)
	{
	    if (Streq(argv[i], "-server"))
	    {
		/*  Generally of the form server_name:display_number.
		 *  These all mean port 0 on server blah:
		 *	"blah",
		 *	"blah:",
		 *	"blah:0"
		 *  This means port 0 on local host: ":0".
		 */
		if (++i < argc || argv[i] == NULL || argv[i][0] == '\0')
		{
		    char *index = strchr(argv[i], ':');
		    if (index == NULL)
			ServerPort = 0;
		    else
		    {
			ServerPort = atoi(index + 1);
			*index = '\0';
		    }
		    if (ServerPort < 0)
			ServerPort = 0;
		    if (index != argv[i])
			strcpy(ServerHostName, OfficialName(argv[i]));
		}
		else
			Usage();
		debug(1,(stderr, "ServerHostName=%s\n", ServerHostName));
	    }
	    else if (Streq(argv[i], "-port"))
	    {
		if (++i < argc)
			ListenForClientsPort = atoi(argv[i]);
		else
			Usage();
		if (ListenForClientsPort <= 0)
		    ListenForClientsPort = 0;
		debug(1,(stderr,
		    "ListenForClientsPort=%d\n",ListenForClientsPort));
	    }
	    else if (Streq(argv[i], "-debug"))
	    {
		/*
		    debug levels:
			2 - trace each procedure entry
			4 - I/O, connections
			8 - Scope internals
			16 - Message protocol
			32 - 64 - malloc
			128 - 256 - really low level
		*/
		if (++i < argc)
			debuglevel = atoi(argv[i]);
		else
			Usage();
		if (debuglevel == 0)
		    debuglevel = 255;
		debuglevel |= 1;
		debug(1,(stderr, "debuglevel = %d\n", debuglevel));
	    }
	    else if (Streq(argv[i], "-verbose"))
	    {
		if (++i < argc)
		    RequestVerbose = EventVerbose =
			ReplyVerbose = ErrorVerbose = atoi(argv[i]);
		else
		    Usage();
	    }
	    else
		Usage();
	}

    LocalHostName = (char *)malloc(255);
    (void) gethostname(LocalHostName, 255);
    if (ServerHostName[0] == '\0')
    {
	char *display_env_name;

	if ((display_env_name = getenv("DISPLAY")) == NULL)
	    (void) gethostname(ServerHostName, sizeof (ServerHostName));
	else
	{
	    char *index;

	    strcpy(ServerHostName, display_env_name);
	    index = strchr(ServerHostName, ':');
	    if (index != NULL)
	    {
		*index = '\0';
		ServerPort = atoi(index + 1);
		if (ServerPort < 0)
		    ServerPort = 0;
	    }
	    if (index == ServerHostName)
		(void) gethostname(ServerHostName, sizeof (ServerHostName));
	}
    }
    if (Streq(ServerHostName,LocalHostName) && ListenForClientsPort ==
ServerPort)
	{
	    fprintf
	    (
		stderr, "Can't have xmond on same port as server (%d)\n",
		ListenForClientsPort
	    );
	    Usage();
	}
}

/*
 * SetUpConnectionSocket:
 *
 * Create a socket for a service to listen for clients
 */
static void
SetUpConnectionSocket(iport)
int			iport;
{
    int			  ON = 1;	  /* used in ioctl */
    int			   ConnectionSocket;
    struct sockaddr_in	  sin;

    enterprocedure("SetUpConnectionSocket");

    /* create the connection socket and set its parameters of use */
    ConnectionSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ConnectionSocket < 0)
	{
	    perror("socket");
	    exit(-1);
	}
    (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_REUSEADDR, (char
*)NULL, 0);
    (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_USELOOPBACK,
(char*)NULL,0);
#ifdef SO_DONTLINGER
    (void)setsockopt(ConnectionSocket, SOL_SOCKET, SO_DONTLINGER,
(char*)NULL, 0);
#endif

    /* define the name and port to be used with the connection socket */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;

    /* the address of the socket is composed of two parts: the host machine and
	the port number.  We need the host machine address for the current host
    */
    {
	/* define the host part of the address */
	struct hostent *hp;

#ifdef OLD_CODE
	hp = gethostbyname(LocalHostName);
	if (hp == NULL)
	    panic("No address for our host"); /* and exit */
	bcopy((char *)hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
#endif
    }
	/* new code -- INADDR_ANY should be better than using the name of the
	    host machine.  The host machine may have several different network
	    addresses.	INADDR_ANY should work with all of them at once. */
    sin.sin_addr.s_addr = INADDR_ANY;

    sin.sin_port = htons (iport);

    /* bind the name and port number to the connection socket */
    if (bind(ConnectionSocket, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
	    perror("bind");
	    exit(-1);
	}

    debug(4,(stderr, "Socket is FD %d for %s,%d\n",
		    ConnectionSocket, LocalHostName, iport));

    /* now activate the named connection socket to get messages */
    if (listen(ConnectionSocket, BACKLOG) < 0)
	{
	    perror("listen");
	    exit(-1);
	};

    /* a few more parameter settings */
#ifdef SYSV
    fcntl(ConnectionSocket, F_SETFD, FD_CLOEXEC);
#else
    ioctl(ConnectionSocket, FIOCLEX, 0);
#endif
#ifndef SVR4
    ioctl(ConnectionSocket, FIONBIO, &ON);
#endif

    debug(4,(stderr, "Listening on FD %d\n", ConnectionSocket));
    (void)UsingFD(ConnectionSocket, NewConnection, (Pointer)ConnectionSocket);
}

/*
 * Signal Handling support
 */
static void
SetSignalHandling()
{
    enterprocedure("SetSignalHandling");
    signal(SIGURG, SignalURG);
    signal(SIGPIPE, SignalPIPE);
    signal(SIGINT, SignalINT);
    signal(SIGTERM, SignalTERM);
    signal(SIGTSTP, SignalTSTP);
    signal(SIGCONT, SignalCONT);
}

/*
 * MainLoop:
 *
 * Wait for input from any source and Process.
 */
static void
MainLoop()
{
    long		    rfds[mskcnt];
    long		    wfds[mskcnt];
    long		    xfds[mskcnt];
    short		    nfds;
    short		    fd;
    Client		    *client;
    Server		    *server;

    while (True)
    {
	COPYBITS(ReadDescriptors, rfds);
	COPYBITS(WriteDescriptors, wfds);
	ORBITS(xfds, ReadDescriptors, WriteDescriptors);
	nfds = select
	(
	    HighestFD + 1, (fd_set *)rfds, (fd_set *)wfds, (fd_set *)xfds, NULL
	);
	if (nfds <= 0)
	{
	    if (errno != EINTR)
		if (errno == EBADF)
		    EOFonFD(HighestFD);
		else
		    panic("Select returns error");
	}
	else
	{
	    for (fd = 0; ANYSET(rfds) && fd <= HighestFD; fd++)
	    {
		if (GETBIT(rfds, fd))
		{
		    BITCLEAR(rfds, fd);
		    (FDD[fd].InputHandler)(FDD[fd].private_data);
		}
	    }
	    for (fd = 0; ANYSET(wfds) && fd <= HighestFD; fd++)
	    {
		if (GETBIT(wfds, fd))
		{
		    BITCLEAR(wfds, fd);
		    if (!SendBuffer(fd, &FDD[fd].outBuffer))
			panic("help: which connection do we shut down (TODO?");
		}
	    }
	    for (fd = 0; ANYSET(xfds) && fd <= HighestFD; fd++)
	    {
		if (GETBIT(xfds, fd))
		{
		    BITCLEAR(xfds, fd);
		    if (fd == fileno(stdin))
			NotUsingFD(fd);
		    else
			fprintf(stderr, "error in select: fd is %d\n", fd);
		}
	    }
	}
	client_list.current = client_list.top;
	while (client_list.current != (LinkLeaf *)(&client_list))
	{
	    client = (Client *)CurrentContentsOfList(&client_list);
	    server = (Server *)(TopOfList(&client->server_list));
	    if
	    (
		SendBuffer(client->fdd->fd, &client->fdd->outBuffer)
		&&
		SendBuffer(server->fdd->fd, &server->fdd->outBuffer)
	    )
		client_list.current = client_list.current->next;
	    else
		CloseConnection(client);
	}
    }
}

static Bool
SendBuffer(fd, buffer)
    int			    fd;
    Buffer		    *buffer;
{
    char		    *buf = buffer->data;
    int			    BytesToWrite = buffer->num_Saved;
    int			    num_written;
    int			    total_num_written = 0;
    Bool		    ok = True;

    BITCLEAR(WriteDescriptors, fd);
    if (BytesToWrite > 0)
    {
	do
	{
	    num_written = write (fd, buf, BytesToWrite);
	    if (num_written > 0)
	    {
		BytesToWrite -= num_written;
		buf += num_written;
		total_num_written += num_written;
	    }
	}
	while (BytesToWrite > 0 && num_written > 0);

	if (BytesToWrite > 0)
	{
	    if (errno == EWOULDBLOCK || errno == EINTR)
	    {
		debug(4,(stderr, "write is blocked: buffering output\n"));
		RemoveSavedBytes(buffer, total_num_written);
		BITSET(WriteDescriptors, fd);
	    }
	    else
	    {
		perror("Error on write to Client/Server");
		ok = False;
	    }
	}
	else
	    buffer->num_Saved = 0;
    }
    return ok;
}

static short
GetServerport ()
{
    short     port;

    enterprocedure("GetServerport");

    port = XBasePort + ServerPort;
    debug(4,(stderr, "Server service is on port %d\n", port));
    return(port);
}

static short
GetScopePort ()
{
    short     port;

    enterprocedure("GetScopePort");

    port = XBasePort + ListenForClientsPort;
    debug(4,(stderr, "xmond service is on port %d\n", port));
    return(port);
}

static void
Usage()
{
    fprintf(stderr, "Usage: xmond\n");
    fprintf(stderr, "              [-server <server_name:port>]\n");
    fprintf(stderr, "              [-port <listen_port>]\n");
    fprintf(stderr, "              [-debug <debug_level>]\n");
    fprintf(stderr, "              [-verbose <all_monitor_level>]\n");
    exit(1);
}

static void
ReadStdin(private_data)
    Pointer		private_data;
{
    int			fd = (int)private_data;
    char		buf[4096];
    char		*ptr;
    char		*end_ptr;
    int			n;
    int			endn;

    enterprocedure("ReadStdin");
    n = read(fd, buf, 2048);
    if (n < 0)
    {
	fprintf(stdout, "Error reading stdin\n");
	return;
    }
    if (n + saved_bytes == 0)
	return;
    if (saved_bytes > 0)
    {
	bcopy(buf, buf + saved_bytes, n);
	bcopy(save_buf, buf, saved_bytes);
	n += saved_bytes;
	saved_bytes = 0;
    }
    ptr = buf;
    while(n > 0)
    {
	while (n > 0 && (*ptr == ' ' || *ptr == '\t'))
	{
	    ptr++;
	    n--;
	}
	if (n <= 0)
	    break;
	end_ptr = ptr;
	endn = n;
	while(endn > 0 && *end_ptr != '\n')
	{
	    end_ptr++;
	    endn--;
	}
	if (endn <= 0)
	{
	    bcopy(ptr, save_buf, n);
	    saved_bytes = n;
	    break;
	}
	*end_ptr = '\0';
	do_command(ptr);
	ptr = end_ptr + 1;
	n = endn - 1;
    }
}

static void
do_command(ptr)
    char		*ptr;
{
	if (Strleneq(ptr, REQUEST_VERBOSE_STR))
	    RequestVerbose = atoi(ptr + strlen(REQUEST_VERBOSE_STR));
	else if (Strleneq(ptr, EVENT_VERBOSE_STR))
	    EventVerbose = atoi(ptr + strlen(EVENT_VERBOSE_STR));
	else if (Strleneq(ptr, REPLY_VERBOSE_STR))
	    ReplyVerbose = atoi(ptr + strlen(REPLY_VERBOSE_STR));
	else if (Strleneq(ptr, ERROR_VERBOSE_STR))
	    ErrorVerbose = atoi(ptr + strlen(ERROR_VERBOSE_STR));
	else if (Strleneq(ptr, MONITOR_REQUEST_ON_STR))
	    VerboseRequest[atoi(ptr + strlen(MONITOR_REQUEST_ON_STR))] = True;
	else if (Strleneq(ptr, MONITOR_REQUEST_OFF_STR))
	    VerboseRequest[atoi(ptr + strlen(MONITOR_REQUEST_OFF_STR))] =False;
	else if (Strleneq(ptr, MONITOR_EVENT_ON_STR))
	    VerboseEvent[atoi(ptr + strlen(MONITOR_EVENT_ON_STR))] = True;
	else if (Strleneq(ptr, MONITOR_EVENT_OFF_STR))
	    VerboseEvent[atoi(ptr + strlen(MONITOR_EVENT_OFF_STR))] = False;
	else if (Strleneq(ptr, REQUEST_MONITORING_ON_STR))
	    MonitoringRequests = True;
	else if (Strleneq(ptr, REQUEST_MONITORING_OFF_STR))
	    MonitoringRequests = False;
	else if (Strleneq(ptr, EVENT_MONITORING_ON_STR))
	    MonitoringEvents = True;
	else if (Strleneq(ptr, EVENT_MONITORING_OFF_STR))
	    MonitoringEvents = False;
	else if (Strleneq(ptr, REQUEST_BLOCKING_ON_STR))
	    BlockingRequests = True;
	else if (Strleneq(ptr, REQUEST_BLOCKING_OFF_STR))
	    BlockingRequests = False;
	else if (Strleneq(ptr, EVENT_BLOCKING_ON_STR))
	    BlockingEvents = True;
	else if (Strleneq(ptr, EVENT_BLOCKING_OFF_STR))
	    BlockingEvents = False;
	else if (Strleneq(ptr, SHOW_PACKET_SIZE_ON_STR))
	    ShowPacketSize = True;
	else if (Strleneq(ptr, SHOW_PACKET_SIZE_OFF_STR))
	    ShowPacketSize = False;
	else if (Strleneq(ptr, SEND_EVENT))
	    test_send_event();
	else if (Strleneq(ptr, SELECTED_REQUEST_VERBOSE_STR))
	    SelectedRequestVerbose =
		atoi(ptr +strlen(SELECTED_REQUEST_VERBOSE_STR));
	else if (Strleneq(ptr, SELECTED_EVENT_VERBOSE_STR))
	    SelectedEventVerbose =
		atoi(ptr + strlen(SELECTED_EVENT_VERBOSE_STR));
	else if (Strleneq(ptr, BLOCK_REQUEST_ON_STR))
	    BlockRequest[atoi(ptr + strlen(BLOCK_REQUEST_ON_STR))] = True;
	else if (Strleneq(ptr, BLOCK_REQUEST_OFF_STR))
	    BlockRequest[atoi(ptr + strlen(BLOCK_REQUEST_OFF_STR))] = False;
	else if (Strleneq(ptr, BLOCK_EVENT_ON_STR))
	    BlockEvent[atoi(ptr + strlen(BLOCK_EVENT_ON_STR))] = True;
	else if (Strleneq(ptr, BLOCK_EVENT_OFF_STR))
	    BlockEvent[atoi(ptr + strlen(BLOCK_EVENT_OFF_STR))] = False;
	else if (Strleneq(ptr, START_REQUEST_COUNT_STR))
	    StartRequestCount();
	else if (Strleneq(ptr, STOP_REQUEST_COUNT_STR))
	    EndRequestCount();
	else if (Strleneq(ptr, CLEAR_REQUEST_COUNT_STR))
	    ClearRequestCount();
	else if (Strleneq(ptr, PRINT_REQUEST_COUNT_STR))
	    PrintRequestCounts();
	else if (Strleneq(ptr, PRINT_ZERO_REQUEST_COUNT_STR))
	    PrintZeroRequestCounts();
	else if (Strleneq(ptr, START_EVENT_COUNT_STR))
	    StartEventCount();
	else if (Strleneq(ptr, STOP_EVENT_COUNT_STR))
	    EndEventCount();
	else if (Strleneq(ptr, CLEAR_EVENT_COUNT_STR))
	    ClearEventCount();
	else if (Strleneq(ptr, PRINT_EVENT_COUNT_STR))
	    PrintEventCounts();
	else if (Strleneq(ptr, PRINT_ZERO_EVENT_COUNT_STR))
	    PrintZeroEventCounts();
	else if (Strleneq(ptr, START_ERROR_COUNT_STR))
	    StartErrorCount();
	else if (Strleneq(ptr, STOP_ERROR_COUNT_STR))
	    EndErrorCount();
	else if (Strleneq(ptr, CLEAR_ERROR_COUNT_STR))
	    ClearErrorCount();
	else if (Strleneq(ptr, PRINT_ERROR_COUNT_STR))
	    PrintErrorCounts();
	else if (Strleneq(ptr, PRINT_ZERO_ERROR_COUNT_STR))
	    PrintZeroErrorCounts();
	else if (Strleneq(ptr, QUIT_STR))
	    exit(0);
	else if (Strleneq(ptr, HELP_STR))
	{
	    fprintf(stdout, "%s <verbose_level>\n", REQUEST_VERBOSE_STR);
	    fprintf(stdout, "%s <verbose_level>\n", EVENT_VERBOSE_STR);
	    fprintf(stdout, "%s <verbose_level>\n", REPLY_VERBOSE_STR);
	    fprintf(stdout, "%s <verbose_level>\n", ERROR_VERBOSE_STR);
	    fprintf(stdout, "%s <verbose_request_number>\n",
						MONITOR_REQUEST_ON_STR);
	    fprintf(stdout, "%s <verbose_request_number>\n",
						MONITOR_REQUEST_OFF_STR);
	    fprintf(stdout, "%s <verbose_event_number>\n",
						MONITOR_EVENT_ON_STR);
	    fprintf(stdout, "%s <verbose_event_number>\n",
						MONITOR_EVENT_OFF_STR);
	    fprintf(stdout, "%s <selected_request_verboseness>\n",
						SELECTED_REQUEST_VERBOSE_STR);
	    fprintf(stdout, "%s <selected_event_verboseness>\n",
						SELECTED_EVENT_VERBOSE_STR);
	    fprintf(stdout,"%s <block_request_number>\n",BLOCK_REQUEST_ON_STR);
	   fprintf(stdout,"%s <block_request_number>\n",BLOCK_REQUEST_OFF_STR);
	    fprintf(stdout, "%s <block_event_number>\n", BLOCK_EVENT_ON_STR);
	    fprintf(stdout, "%s <block_event_number>\n", BLOCK_EVENT_OFF_STR);

	    fprintf(stdout, "%s\n", START_REQUEST_COUNT_STR);
	    fprintf(stdout, "%s\n", STOP_REQUEST_COUNT_STR);
	    fprintf(stdout, "%s\n", CLEAR_REQUEST_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_REQUEST_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_ZERO_REQUEST_COUNT_STR);
	    fprintf(stdout, "%s\n", START_EVENT_COUNT_STR);
	    fprintf(stdout, "%s\n", STOP_EVENT_COUNT_STR);
	    fprintf(stdout, "%s\n", CLEAR_EVENT_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_EVENT_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_ZERO_EVENT_COUNT_STR);
	    fprintf(stdout, "%s\n", START_ERROR_COUNT_STR);
	    fprintf(stdout, "%s\n", STOP_ERROR_COUNT_STR);
	    fprintf(stdout, "%s\n", CLEAR_ERROR_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_ERROR_COUNT_STR);
	    fprintf(stdout, "%s\n", PRINT_ZERO_ERROR_COUNT_STR);
	    fprintf(stdout, "%s\n", QUIT_STR);
	    fprintf(stdout, "%s\n", HELP_STR);
	}
	else
	    fprintf
	    (
		stdout, "illegal command: %s\n\"help\" to get help\n", ptr
	    );
}

/*
 * NewConnection:
 *
 * Create New Connection to a client program and to Server.
 */
static void
NewConnection(private_data)
    Pointer		private_data;
{
    int			XPort = (int)private_data;
    Client		*client;
    Server		*server;
    int			fd;

    client = Tmalloc(Client);
    appendToList(&client_list, (Pointer)client);
    fd = ConnectToClient(XPort);
    client->fdd = UsingFD(fd, DataFromClient, (Pointer)client);
    client->fdd->fd = fd;
    StartClientConnection(client);

    initList(&client->server_list);
    server = Tmalloc(Server);
    appendToList(&client->server_list, (Pointer)server);
    fd = ConnectToServer(ServerHostName);
    server->fdd = UsingFD(fd, DataFromServer, (Pointer)server);
    server->fdd->fd = fd;
    server->client = client;
    StartServerConnection(server);

    ClientNumber += 1;
    client->ClientNumber = ClientNumber;
    if (ErrorVerbose > 1)
	fprintf(stdout, "Opening client connection: %d\n", ClientNumber);
}

static void
DataFromClient(private_data)
    Pointer		    private_data;
{
    Client		    *client = (Client *)private_data;
    Server		    *server;

    server = (Server *)(TopOfList(&client->server_list));
    if (!ReadAndProcessData(private_data, client->fdd, server->fdd, False))
	CloseConnection(client);
}

static void
DataFromServer(private_data)
    Pointer		    private_data;
{
    Server		    *server = (Server *)private_data;
    Client		    *client = server->client;

    if (!ReadAndProcessData(private_data, server->fdd, client->fdd, True))
	CloseConnection(client);
}

/*
 * ReadAndProcessData:
 *
 * Read as much as we can and then loop as long as we have enough
 * bytes to do anything.
 *
 * In each cycle check if we have enough bytes (saved or in the newly read
 * buffer) to do something.  If so, we want the bytes to be grouped
 * together into one contiguous block of bytes.	 We have three cases:
 *
 * (1) num_Saved == 0; so all needed bytes are in the read buffer.
 *
 * (2) num_Saved >= num_Needed; in this case all needed
 * bytes are in the save buffer and we will not need to copy any extra
 * bytes from the read buffer into the save buffer.
 *
 * (3) 0 < num_Saved < num_Needed; so some bytes are in
 * the save buffer and others are in the read buffer.  In this case we
 * need to copy some of the bytes from the read buffer to the save buffer
 * to get as many bytes as we need, then use these bytes.  First determine
 * the number of bytes we need to transfer; then transfer them and remove
 * them from the read buffer.  (There may be additional requests in the
 * read buffer - we'll deal with them next cycle.)
 *
 * At this stage, we have a pointer to a contiguous block of
 * num_Needed bytes that we should process.  The type of
 * processing depends upon the state we are in, given in the
 * ByteProcessing field of the FDDescriptor structure pointed to by
 * read_fdd.  The processing routine returns the number of bytes that it
 * actually used.
 *
 * The number of bytes that were actually used is normally (but not
 * always) the number of bytes needed.	Discard the bytes that were
 * actually used, not the bytes that were needed.  The number of used
 * bytes must be less than or equal to the number of needed bytes.  If
 * there were no saved bytes, then the bytes that were used must have been
 * in the read buffer so just modify the buffer pointer.  Otherwise call
 * RemoveSavedBytes.
 *
 * After leaving the loop, if there are still some bytes left in the read
 * buffer append the newly read bytes to the save buffer.
 *
 * Return False if we reached end-of-file on read.
 */

static Bool
ReadAndProcessData(private_data, read_fdd, write_fdd, is_server)
    Pointer		    private_data;
    FDDescriptor	    *read_fdd;
    FDDescriptor	    *write_fdd;
    Bool		    is_server;
{
    Buffer		    *inbuffer = &read_fdd->inBuffer;
    Buffer		    *outbuffer = &write_fdd->outBuffer;
    int			    fd = read_fdd->fd;
    char		    read_buf_block[16384];
    char		    *read_buf = read_buf_block;
    int			    num_read;
    char		    *process_buf;
    int			    NumberofUsedBytes;
    int			    BytesToSave;

    num_read = read(fd, read_buf, 16384);
    if (ShowPacketSize)
    {
	if (is_server)
	    fprintf(stdout, "\t\t\t\t\t");
	else
	    fprintf(stdout, "Client %d --> ", read_fdd->fd);
	fprintf(stdout, "%4d byte%s", num_read, (num_read == 1) ? "" : "s");
	if (is_server)
	    fprintf(stdout, "<-- Server %d\n", read_fdd->fd);
	else
	    fprintf(stdout, "\n");
    }
    if (num_read < 0)
    {
	perror("read error");
	if (errno == EWOULDBLOCK)
	    panic("ReadAndProcessData: read would block (not implemented)");
	if (errno == EINTR)
	    panic("ReadAndProcessData: read interrupted (not implemented)");
    }
    if (num_read == 0)
    {
	if (ErrorVerbose > 1)
	    fprintf(stdout, "EOF\n");
	return False;
    }

    littleEndian = read_fdd->littleEndian;
    while (inbuffer->num_Saved + num_read >= inbuffer->num_Needed)
    {
	if (inbuffer->num_Saved == 0)
	    process_buf = read_buf;
	else
	{
	    if (inbuffer->num_Saved < inbuffer->num_Needed)
	    {
		BytesToSave = inbuffer->num_Needed - inbuffer->num_Saved;
		SaveBytes(inbuffer, read_buf, BytesToSave);
		read_buf += BytesToSave;
		num_read -= BytesToSave;
	    }
	    process_buf = inbuffer->data;
	}
	ignore_bytes = False;
	NumberofUsedBytes = (*read_fdd->ByteProcessing)
	(
	    private_data, process_buf, inbuffer->num_Needed
	);
	if (NumberofUsedBytes > 0)
	{
	    if (!ignore_bytes)
		SaveBytes(outbuffer, process_buf, NumberofUsedBytes);
	    if (inbuffer->num_Saved > 0)
		RemoveSavedBytes(inbuffer, NumberofUsedBytes);
	    else
	    {
		read_buf += NumberofUsedBytes;
		num_read -= NumberofUsedBytes;
	    }
	}
    }
    if (num_read > 0)
	SaveBytes(inbuffer, read_buf, num_read);
    return True;
}

/*
 * We will need to save bytes until we get a complete request to
 * interpret.  The following procedures provide this ability.
 */

static void
SaveBytes(buffer, buf, n)
    Buffer		    *buffer;
    char		    *buf;
    long		    n;
{
    long		    new_size;
    char		    *new_buf;

    /* check if there is enough space to hold the bytes we want */
    if (buffer->num_Saved + n > buffer->BlockSize)
	{
	    /* not enough room so far; malloc more space and copy */
	    new_size = (buffer->num_Saved + n + 1);
	    new_buf = (char *)malloc(new_size);
	    bcopy(buffer->data, new_buf, buffer->BlockSize);
	    if (buffer->BlockSize > 0)
		Tfree(buffer->data);
	    buffer->data = new_buf;
	    buffer->BlockSize = new_size;
	}

    /* now copy the new bytes onto the end of the old bytes */
    bcopy(buf, (buffer->data + buffer->num_Saved), n);
    buffer->num_Saved += n;
}

static void
RemoveSavedBytes(buffer, n)
    Buffer		    *buffer;
    int			    n;
{
    /* check if all bytes are being removed -- easiest case */
    if (buffer->num_Saved <= n)
	buffer->num_Saved = 0;
    else if (n != 0)
    {
	/* not all bytes are being removed -- shift the remaining ones down  */
	register char  *p = buffer->data;
	register char  *q = buffer->data + n;
	register int   i = buffer->num_Saved - n;
	while (i-- > 0)
	    *p++ = *q++;
	buffer->num_Saved -= n;
    }
}

static int
ConnectToClient(ConnectionSocket)
	int ConnectionSocket;
{
    int			  ON = 1;	  /* used in ioctl */
    int ClientFD;
    struct sockaddr_in	from;
    int	   len = sizeof (from);

    ClientFD = accept(ConnectionSocket, (struct sockaddr *)&from, &len);
    debug(4,(stderr, "Connect To Client: FD %d\n", ClientFD));
    if (ClientFD < 0 && errno == EWOULDBLOCK)
	{
	    debug(4,(stderr, "Almost blocked accepting FD %d\n", ClientFD));
	    panic("Can't connect to Client");
	}
    if (ClientFD < 0)
	{
	    debug(4,(stderr, "ConnectToClient: error %d\n", errno));
	    panic("Can't connect to Client");
	}

#ifdef SYSV
    fcntl(ClientFD, F_SETFD, FD_CLOEXEC);
#else
    ioctl(ClientFD, FIOCLEX, 0);
#endif
#ifndef SVR4
    ioctl(ClientFD, FIONBIO, &ON);
#endif
    return(ClientFD);
}

static int
ConnectToServer(hostName)
    char *hostName;
{
    int ServerFD;
    struct sockaddr_in	sin;
    struct hostent *hp;
    unsigned long hostinetaddr;

    enterprocedure("ConnectToServer");

    /* establish a socket to the name server for this host */
    bzero((char *)&sin, sizeof(sin));
    ServerFD = socket(AF_INET, SOCK_STREAM, 0);
    if (ServerFD < 0)
	{
	    perror("socket() to Server failed");
	    debug(1,(stderr, "socket failed\n"));
	    panic("Can't open connection to Server");
	}
    (void) setsockopt(ServerFD, SOL_SOCKET, SO_REUSEADDR,  (char *) NULL, 0);
    (void) setsockopt(ServerFD, SOL_SOCKET, SO_USELOOPBACK,(char *) NULL, 0);
#ifdef SO_DONTLINGER
    (void) setsockopt(ServerFD, SOL_SOCKET, SO_DONTLINGER, (char *) NULL, 0);
#endif

    debug(4,(stderr, "try to connect on %s\n", hostName));

    if (isdigit(hostName[0]))
    {
	hostinetaddr = inet_addr(hostName);
	if (hostinetaddr == -1)
	    printf("ConnectToServer: hostinetaddr is -1\n");
    }
    else
	hostinetaddr = -1;
    if (hostinetaddr == -1)
    {
	if ((hp = gethostbyname(hostName)) == 0)
	{
	    perror("gethostbyname failed");
	    debug(1,(stderr, "gethostbyname failed for %s\n", hostName));
	    panic("Can't open connection to Server");
	}
	if (hp->h_addrtype != AF_INET)
	{
	    perror("gethostbyname failed (not INET)");
	    debug(1,(stderr, "gethostbyname failed for %s\n", hostName));
	    panic("Can't open connection to Server");
	}
	sin.sin_family = hp->h_addrtype;
	bcopy((char *)hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    }
    else
    {
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = hostinetaddr;
    }

    sin.sin_port = htons (GetServerport());

    /* ******************************************************** */
    /* try to connect to Server */

    if (connect(ServerFD, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
	    debug(4,(stderr, "connect returns errno of %d\n", errno));
	    if (errno != 0)
		perror("connect");
	    switch (errno)
		{
		case ECONNREFUSED:
		    /* experience says this is because there is no Server
			to connect to */
		    close(ServerFD);
		    debug(1,(stderr, "No Server\n"));
		    panic("Can't open connection to Server (ECONNREFUSED)");
		default:
		    close(ServerFD);
		    fprintf(stderr, "errno = %d: ", errno);
		    panic("Can't open connection to Server");
		}
	}

    debug(4,(stderr, "Connect To Server: FD %d\n", ServerFD));
    return(ServerFD);
}

static char*
OfficialName(name)
char *name;
{
    struct hostent *HostEntry;

    HostEntry = gethostbyname(name);
    if (HostEntry == NULL)
	return(name);
    else
	return(HostEntry->h_name);
}

static void
SignalURG()
{
    debug(1,(stderr, "==> SIGURG received\n"));
}

static void
SignalPIPE()
{
    debug(1,(stderr, "==> SIGPIPE received\n"));
}

static void
SignalINT()
{
    debug(1,(stderr, "==> SIGINT received\n"));
    (void) fflush(stdout);
    exit(1);
}

static void
SignalTERM()
{
    debug(1,(stderr, "==> SIGTERM received\n"));
    (void) fflush(stdout);
    exit(1);
}

static void
SignalTSTP()
{
    debug(1,(stderr, "==> SIGTSTP received\n"));
}

static void
SignalCONT()
{
    debug(1,(stderr, "==> SIGCONT received\n"));
}

Global void
enterprocedure(s)
	char   *s;
{
    debug(2,(stderr, "-> %s\n", s));
}

Global void
panic(s)
	char   *s;
{
    fprintf(stderr, "%s\n", s);
    exit(1);
}

#if (mskcnt>4)
static Bool
ANYSET(src)
long *src;
{
    int cri;

    for (cri = 0; cri < mskcnt; cri++)
	if (src[cri])
	    return 1;
    return 0;
}
#endif

/*
 * This was to be the beginning of a client-side X event record/playback
 * program. If you finish it let me know.
 *    - gregm@otc.otca.oz.au
 */
test_send_event()
{
    char		    buf[32];
    Client		    *client;

    client_list.current = client_list.top;
    client = (Client *)CurrentContentsOfList(&client_list);

    buf[0] = 2;
    buf[1] = 61;
    ShortToBuf(client->SequenceNumber, &buf[2]);
    LongToBuf(0, &buf[4]);
    LongToBuf(0x0008006d, &buf[8]);
    LongToBuf(0x01100003, &buf[12]);
    LongToBuf(0, &buf[16]);
    ShortToBuf(399, &buf[20]);
    ShortToBuf(333, &buf[22]);
    ShortToBuf(78, &buf[24]);
    ShortToBuf(12, &buf[26]);
    ShortToBuf(0, &buf[28]);
    buf[30] = 1;
    buf[31] = 0;
    write (client->fdd->fd, buf, 32);
}
