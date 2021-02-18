/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sjxproto.c
 *	sjx protocol
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Thu Jun 23 11:31:31 JST 1988
 */
/*
 * $Header: sjxproto.c,v 1.1 91/09/04 23:08:31 root Locked $ SONY;
 */

#include <sys/errno.h>
#include "sj2.h"
#include "sjx.h"
#include "key.h"
#include "xcommon.h"
#include "Xlc.h"
#include "Xlcdef.h"
#include <X11/Xatom.h>

#ifdef PROTO1
#include "SJXproto1.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "select.h"
#endif /* PROTO1 */

extern int	vflag;

extern char	hostname[];

char		*conversion = JAPANESE_CONVERSION;

static char	*localdisp = "unix:0";
static int	local_display = 0;

int		inchar;
int		keylen = 0;
int		through_status = 0;

u_char		keybuff[BUFFLENGTH];
KeySym		keysym;
int		inclient;
int		input;
Atom		selection;
#ifdef PROTO2
Window		inwindow;
#endif /* PROTO2 */

static Time	time;
static int	storebytes = 0;
static char	storebuff[BUFFLENGTH * 4];

#ifdef PROTO1
#define BIND_RETRY	1000

static struct sockaddr_in	server;
static struct servent		*sp, spp;
static SELECT_FD		ready_clients;
static SELECT_FD		clients_code;
static int			sock = SOCK_INIT;
static int			flushed_client = 0;
static int			read_client = 0;

extern int			convert_status;
#endif /* PROTO1 */

/*
 *	Common Protocol Routines
 */

StoreBuff (buff, len)
char	*buff;
int	len;
{
	bcopy (buff, &storebuff[storebytes], len);
	storebytes += len;
}

flush_to_client ()
{
	send_to_client (input, storebuff, storebytes);
	storebytes = 0;
#ifdef PROTO1
	flushed_client = 1;
#endif /* PROTO1 */
}

proto_out ()
{
#ifdef PROTO1
	master_out (keybuff, keylen);
	keylen = 0;
#endif /* PROTO1 */
}

next_keybuff ()
{
	register int	i;

	for (i = 0 ; i < keylen ; i ++)
		keybuff[i] = keybuff[i + 1];
	keylen --;
	keysym = 0;
	return (*keybuff);
}

proc_proto ()
{
#ifdef PROTO1
	if (flushed_client) {
		flushed_client = 0;
		read_client = 0;
	} else
#endif /* PROTO1 */
	if (storebytes) {
		master_flush ();
#ifdef PROTO1
		flushed_client = 0;
		read_client = 0;
#endif /* PROTO1 */
	}
#ifdef PROTO1
	else if (read_client) {
		flush_dummy ();
		flushed_client = 0;
		read_client = 0;
	}
#endif /* PROTO1 */
}

send_to_client (input, storebuff, storebytes)
int	input;
char	*storebuff;
int	storebytes;
{
#ifdef PROTO1
	if (input == INPUT_SOCKET)
		send_to_socket (inclient, storebuff, storebytes);
#endif /* PROTO1 */
#ifdef PROTO2
	if (input == INPUT_WINDOW)
		send_to_window (inwindow, storebuff, storebytes);
#endif /* PROTO2 */
}

selection_init ()
{
#ifdef PROTO2
	prop_init ();
#endif /* PROTO2 */

        /* become the selection owner	*/
	selection = XInternAtom (dpy, conversion, False);
        XSetSelectionOwner (dpy, selection, subwin, time);

#ifdef PROTO3
	xim_init ();
#endif /* PROTO3 */
}

/*
 *	SJX Protocol Version 1 Routines
 */

#ifdef PROTO1
proto1_dispatch ()
{
	SELECT_FD	ifds;
	struct timeval	t;
	register int	msgsock, rval;
	register int	i;
	static int	count = BIND_RETRY;

	if (sock == SOCK_INIT && count > 0) {
		sock = SJX_init ();
		count --;
	}
	if (sock == SOCK_INIT)
		return (0);
	for (;;) {
		set_smask (sock, &ready_clients, &ifds);
		set_stime (&t);

		if ((i = select (MAXSOCKET, &ifds, 0, 0, &t)) < 0) {
			return (0);
		}

		if (XPending (dpy) != 0) {
			if (ProcX ())
				return (1);
			else
				return (0);
		}

		if (i == 0)
			return (0);
		if (FD_ISSET (ConnectionNumber (dpy), &ifds)) {
			if (ProcX ())
				return (1);
			else
				return (0);
		}
		if (FD_ISSET (sock, &ifds)) {
			CreateNewClient (sock);
		}
		if (is_client (&ready_clients, &ifds)) {
			ProcClient (&ifds);
			return (1);
		}
		else if (!FD_ISSET (sock, &ifds)) {
			return (0);
		}
	}
}

set_smask (sock, ready, ifds)
int		sock;
SELECT_FD	*ready, *ifds;
{
	bcopy (ready, ifds, sizeof(SELECT_FD));
	FD_SET (sock, ifds);
	FD_SET (ConnectionNumber (dpy), ifds);
}

set_stime (t)
struct timeval	*t;
{
	t -> tv_sec = 0;
	t -> tv_usec = 100000;
}

is_client (ready, ifds)
SELECT_FD	*ready, *ifds;
{
#ifdef LONG_FD
	if ((ready->fds_bits[0] & ifds->fds_bits[0])
	 || (ready->fds_bits[1] & ifds->fds_bits[1]))
#else /* LONG_FD */
	if (*ready & *ifds)
#endif /* LONG_FD */
		return (1);
	else
		return (0);
}

fd_ffs (ready, ifds)
SELECT_FD	*ready, *ifds;
{
#ifdef LONG_FD
	register fd_mask	i;

	if (i = ifds->fds_bits[0] & ready->fds_bits[0])
		return (ffs (i));
	else if (i = ifds->fds_bits[1] & ready->fds_bits[1])
		return (ffs (i) + (1 << 5));
	else
		return (0);
#else /* LONG_FD */
	return (ffs (*ifds & *ready));
#endif /* LONG_FD */
}

fd_client (ifds)
SELECT_FD	*ifds;
{
	return (fd_ffs (&ready_clients, ifds) - 1);
}

CreateNewClient (sock)
int	sock;
{
	register int	client;

	client = accept (sock, 0, 0);
	if (client == -1) {
		perror ("accept");
	} else {
		FD_SET (client, &ready_clients);
		AllocResource (client);
	}
}

CloseClient (client)
int	client;
{
	FD_CLR (client, &ready_clients);
	FreeResource (client);
	shutdown (client, 2);
	close (client);
}

ProcClient (ifds)
SELECT_FD	*ifds;
{
	register int	client;

	client	= fd_client (ifds);
	ChangeResource (client);
	receive (client);
}

receive (client)
int	client;
{
	register int	len;
	u_char		pnum;

	len = read (client, &pnum, sizeof pnum);
	if (len <= 0) {
		CloseClient (client);
		inchar	= '\0';
		return (len);
	}
	switch (pnum) {
		case SJX_CONFIGURE_REQUEST:
			proc_client_configure (client);
			break;
		case SJX_MAP_REQUEST:
			proc_client_map (client);
			break;
		case SJX_UNMAP_REQUEST:
			proc_client_unmap (client);
			break;
		case SJX_SEND_KEYEVENT:
			proc_client_send (client);
			read_client = client;
			break;
		case SJX_CODE_REQUEST:
			proc_client_code (client);
			break;
		default:
			fprintf (stderr,
				"unknown protocol number 0x%x\n",
				pnum);
			break;
	}

	return (len);
}

flush_dummy ()
{
	char	*dummy = " ";
	send_to_client (INPUT_SOCKET, dummy, 0);
}

static int	next_off = 0;
static int	now_status = 0;

send_to_socket (client, buff, len)
int	client, len;
char	*buff;
{
	char		buff1[BUFSIZ];
	char		buff2[BUFSIZ];
	char		*b;
	register char	*p;
	char		status;
	int		dlen;
	int		wlen;
	extern KeySym	keysym;

	if (FD_ISSET (client, &ready_clients)) {
		if (through_status) {
			status = 0;
			through_status = 0;
		}
		else if (convert_status) {
			now_status = 1;
			next_off = 0;
			status = 1;
		}
		else if (next_off) {
			now_status = 0;
			next_off = 0;
			status = 0;
		}
		else if (now_status) {
			now_status = 0;
			next_off = 1;
			status = 1;
		}
		else {
			now_status = 0;
			next_off = 0;
			status = 0;
		}

		if (is_euc_client (client)) {
			buff[len] = '\0';
			sjistoeuc (buff, buff2);
			b = buff2;
			dlen = strlen (buff2);
		}
		else {
			b = buff;
			dlen = len;
		}

		if (status)
			keysym = 0;

		p = buff1;
		wlen = 0;

		bcopy (&status, p, sizeof(status));
		p += sizeof(status);
		wlen += sizeof(status);

		bcopy (&keysym, p, sizeof(keysym));
		p += sizeof(keysym);
		wlen += sizeof(keysym);

		bcopy (&dlen, p, sizeof(dlen));
		p += sizeof(dlen);
		wlen += sizeof(dlen);

		if (dlen > 0) {
			bcopy (b, p, dlen);
			wlen += dlen;
		}

		if (write (client, buff1, wlen) < 0) {
			perror ("write socket");
			exit (1);
		}
	}
}

proc_client_code (client)
register int	client;
{
	int	len;
	int	codenum;

	len = read (client, &codenum, sizeof(codenum));
	if (len <= 0) {
		return (0);
	}
	if (FD_ISSET (client, &ready_clients)) {
		if (codenum == SJX_CODE_SJIS) {
			set_code_sjis (client);
		}
		else if (codenum == SJX_CODE_EUC) {
			set_code_euc (client);
		}
		else {
			;
		}
	}
	return (1);
}

set_code_sjis (client)
register int	client;
{
	FD_CLR (client, &clients_code);
}

set_code_euc (client)
register int	client;
{
	FD_SET (client, &clients_code);
}

is_euc_client (client)
register int	client;
{
	return (FD_ISSET (client, &clients_code));
}

SJX_init ()
{
	extern int	errno;
	extern int	vflag;
	extern int	screen;
	register int	soc;
	int		optval;
	int		optlen = sizeof(optval);
	int 		optnam;
	int		r;
	int		port_limit;

	if ((sp = getservbyname (SJX_SERVICE, SJX_PROTO)) == NULL) {
		spp.s_port = SJX_TCP_PORT;
		sp = &spp;
	}
	if (!local_display) {
		port_limit = 0;
		++ sp->s_port;
		while (anyserver (sp)) {
			if (vflag > 1)
				fprintf (stderr,
				"existed server address %d\n", sp->s_port);
			if (++ port_limit >= SJX_PORT_LIMIT) {
				fprintf (stderr,
					"all address in use (end %d)\n",
					sp->s_port);
				done2 ();
			}
			++ sp->s_port;
		}
		if (vflag > 1)
			fprintf (stderr, "address %d\n", sp->s_port);
	}
	if ((soc = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		return (SOCK_INIT);
	}

	bzero ((char *)&server, sizeof (server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl (INADDR_ANY);
	server.sin_port = htons (sp->s_port);

	r = bind (soc, &server, sizeof(server));
	if (r) {
		if (errno == EADDRINUSE) {
			getsockopt (
				soc, SOL_SOCKET, optval,
				(char *)&optval, &optlen
			);
			optnam = SO_REUSEADDR;
			setsockopt (
				soc, SOL_SOCKET, optnam,
				(char *)&optnam, sizeof(optnam)
			);
			r = bind (soc, &server, sizeof(server));
			setsockopt (
				soc, SOL_SOCKET, optval,
				(char *)&optval, optlen
			);
		}
		if (r) {
			shutdown (soc, 2);
			close (soc);
			return (SOCK_INIT);
		}
	}

	FD_ZERO (&ready_clients);
	FD_ZERO (&clients_code);

	listen (soc, 5);
	export (sp->s_port);
	return (soc);
}

anyserver (sp)
struct servent		*sp;
{
	struct sockaddr_in	serv;
	struct hostent		*hp;
	int			so;
	int			ack;

	bzero (&serv, sizeof(serv));
	serv.sin_family = AF_INET;
	serv.sin_port = htons (sp->s_port);
	if ((hp = gethostbyname (hostname)) == NULL)
		return (0);
	bcopy (hp->h_addr, &serv.sin_addr, hp->h_length);
	if ((so = socket (AF_INET, SOCK_STREAM, 0)) < 0)
		return (0);
	if (connect (so, &serv, sizeof(serv)) < 0) {
		shutdown (so, 2);
		close (so);
		return (0);
	}
	shutdown (so, 2);
	close (so);
	return (1);
}

static	Atom	sjx_host, sjx_port;

export (port)
int	port;
{
	sjx_host = XInternAtom (dpy, SJX_MACHINE, False);
	sjx_port = XInternAtom (dpy, SJX_PORT, False);
	XChangeProperty (
		dpy, root0, sjx_host, XA_STRING, 8,
		PropModeReplace, (char *) hostname, strlen (hostname)
	);
	XChangeProperty (
		dpy, root0, sjx_port, XA_INTEGER, 32,
		PropModeReplace, (char *) &port, 1
	);
}

delete_export ()
{
	XDeleteProperty (dpy, root0, sjx_host);
	XDeleteProperty (dpy, root0, sjx_port);
	XFlush (dpy);
}

SJX_end ()
{
	int	client;

	while ((client = fd_client (&ready_clients)) >= 0)
		CloseClient (client);
	if (sock != SOCK_INIT) {
		shutdown (sock, 2);
		close (sock);
	}
	delete_export ();
}

proc_client_send (client)
register int	client;
{
	register int	len;
	KeySym		ks;
	XKeyEvent	ev;
	XComposeStatus	xcs;

	len = read (client, &ev, sizeof ev);
	if (len <= 0)
		return (0);
	ev.display = dpy;
	ev.window = win;
	*keybuff = '\0';
	keylen = XLookupKanaString (&ev, keybuff, BUFFLENGTH, &ks, &xcs);
	sjx_eval_key (&ev, keybuff, keylen, &ks, &xcs);
	inchar = *keybuff;
	inclient = client;
	keysym = ks;
	input = INPUT_SOCKET;

	ChangeDefaultDS ();
	return (1);
}

proc_client_configure (client)
register int	client;
{
	int		len;
	wininfo		i;

	len = read (client, &i, sizeof i);
	if (len <= 0)
		return (0);
	XMoveResizeWindow (dpy, subwin, i.x, i.y, i.width, i.height);
	XFlush (dpy);
	return (1);
}

proc_client_map (client)
int	client;
{
	MapWindow ();
}

proc_client_unmap (client)
int	client;
{
	UnmapWindow ();
}

CheckRemote (disp)
register char	*disp;
{
	register char	*p, *q;
	int		check_next;
	char		displayname[MAXHOSTNAMELEN];

	if (disp != (char *)NULL && *disp != '\0' && *disp != ':') {
		p = localdisp;
		q = disp;
		check_next = 1;
		while (check_next) {
			if (*p == '\0') {
				check_next = 0;
				break;
			}
			else if (*(q ++) != *(p ++)) {
				break;
			}
		}
		strcpy (displayname, hostname);
		strcat (displayname, ":0");
		p = displayname;
		q = disp;
		while (check_next) {
			if (*p == '\0')
				break;
			else if (*(q ++) != *(p ++))
				return (1);		/* remote */
		}
	}
	local_display = 1;
	return (0);					/* local */
}

AllocResource (client)
int	client;
{
	set_code_sjis (client);
}

FreeResource (client)
int	client;
{
	set_code_sjis (client);
}

ChangeResource (client)
int	client;
{
	DefaultDrawingSet ();
}

#endif /* PROTO1 */

ProcX ()
{
	if (XPending (dpy) == 0) {
		fprintf (stderr, "sjx: destroyed X Server or sjx\n");
		done2 ();
	}
	return (Xdispatch ());
}
