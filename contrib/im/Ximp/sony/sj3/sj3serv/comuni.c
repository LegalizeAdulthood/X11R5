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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include "sj_kcnv.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>

int	client_num = 0;
Client	client[MaxClientNum];
int	cur_client;

int	maxfds = 0;
fd_set	fds_org = { 0 };
jmp_buf	client_dead;
int	client_fd;

extern	char	*socket_name;
extern	char	*port_name;
extern	int	port_number;
extern	int	max_client;
extern	int	errno;

static	int	fd_inet = -1;
static	int	fd_unix = -1;

static	char	putbuf[BUFSIZ];
static	char	getbuf[BUFSIZ];
static	int	putpos = 0;
static	int	buflen = 0;
static	int	getpos = 0;

socket_init()
{
	client_num = 0;
	FD_ZERO(&fds_org);
	maxfds = 0;
}

static	void	set_fd(fd)
int	fd;
{
	FD_SET(fd, &fds_org);
	if (fd >= maxfds) maxfds = fd + 1;
}

static	void	clr_fd(fd)
int	fd;
{
	FD_CLR(fd, &fds_org);
	if (maxfds == fd + 1) {
		while (--fd >= 0) {
			if (FD_ISSET(fd, &fds_org)) break;
		}
		maxfds = fd + 1;
	}
}

static	void	open_af_unix()
{
	struct sockaddr_un	sosun;

	unlink(socket_name);

	bzero((char *)&sosun, sizeof(sosun));
	sosun.sun_family = AF_UNIX;
	strcpy(sosun.sun_path, socket_name);

	if ((fd_unix = socket(AF_UNIX, SOCK_STREAM, 0)) == ERROR) {
		unlink(socket_name);
		fprintf(stderr, "Can't create AF_UNIX socket\n");
		exit(1);
	}

	if (bind(fd_unix, &sosun, strlen(sosun.sun_path) + 2) == ERROR) {
		if (errno == EADDRINUSE)
			fprintf(stderr, "Port '%s' is already in use\n",
				socket_name);
		else
			fprintf(stderr, "Can't bind AF_UNIX socket\n");
		shutdown(fd_unix, 2);
		close(fd_unix);
		unlink(socket_name);
		exit(1);
	}

	if (listen(fd_unix, SOMAXCONN) == ERROR) {
		shutdown(fd_unix, 2);
		unlink(socket_name);
		close(fd_unix);
		fprintf(stderr, "Can't listen AF_UNIX socket\n");
		exit(1);
	}

	set_fd(fd_unix);
}

static	void	open_af_inet()
{
	struct sockaddr_in	sin;
	struct servent	*sp;
	u_short	port;
	int	true = 1;

	if (sp = getservbyname(port_name, "tcp"))
		port = ntohs(sp -> s_port);
	else
		port = port_number;

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family      = AF_INET;
	sin.sin_port        = htons(port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((fd_inet = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		fprintf(stderr, "Can't create AF_INET socket\n");
		exit(1);
	}

	setsockopt(fd_inet, SOL_SOCKET, SO_REUSEADDR, &true, sizeof(true));

	if (bind(fd_inet, &sin, sizeof(sin)) == ERROR) {
		if (errno == EADDRINUSE) {
			if (sp)
				fprintf(stderr,
					"Port '%s(%d)' is already in use\n",
					port_name, port);
			else
				fprintf(stderr,
					"Port %d is already in use\n",
					port);
		}
		else
			fprintf(stderr, "Can't bind AF_INET socket\n");
		shutdown(fd_inet, 2);
		close(fd_inet);
		exit(1);
	}

	if (listen(fd_inet, SOMAXCONN) == ERROR) {
		shutdown(fd_inet, 2);
		close(fd_inet);
		fprintf(stderr, "Can't listen AF_INET socket\n");
		exit(1);
	}

	set_fd(fd_inet);
}
open_socket()
{
	open_af_inet();
	open_af_unix();
}

static	int	connect_af_unix()
{
	struct sockaddr_un	sosun;
	int	i = sizeof(sosun);
	int	fd;

	if ((fd = accept(fd_unix, &sosun, &i)) == ERROR)
		warning_out("Can't accept AF_UNIX socket");
	return fd;
}

static	int	connect_af_inet()
{
	struct sockaddr_in	sin;
	int	i = sizeof(sin);
	int	fd;

	if ((fd = accept(fd_inet, &sin, &i)) == ERROR)
		warning_out("Can't accept AF_INET socket");
	return fd;
}

static	void	close_af_unix()
{
	struct sockaddr_un	sosun;
	int	true = 1;
	int	i;

	ioctl(fd_unix, FIONBIO, &true);
	for ( ; ; ) {
		i = sizeof(sosun);
		if (accept(fd_unix, &sosun, &i) < 0) break;
	}
	shutdown(fd_unix, 2);
	close(fd_unix);
	unlink(socket_name);
	clr_fd(fd_unix);
}

static	void	close_af_inet()
{
	struct sockaddr_in	sin;
	int	true = 1;
	int	i;

	ioctl(fd_inet, FIONBIO, &true);
	for ( ; ; ) {
		i = sizeof(sin);
		if (accept(fd_inet, &sin, &i) < 0) break;
	}
	shutdown(fd_inet, 2);
	close(fd_inet);
	clr_fd(fd_inet);
}
close_socket()
{
	close_af_unix();
	close_af_inet();
}

static	void	connect_client(readfds)
fd_set	*readfds;
{
	int	fd;

	if (FD_ISSET(fd_inet, readfds))
		fd = connect_af_inet();
	else if (FD_ISSET(fd_unix, readfds))
		fd = connect_af_unix();
	else
		return;
	if (fd == ERROR) return;


	if (client_num >= MaxClientNum || client_num >= max_client) {
		warning_out("No more client");
	}

	else {
		debug_out(1, "client create(%d)\r\n", fd);
		logging_out("connect to client on %d", fd);

		bzero(&client[client_num], sizeof(client[0]));
		client[client_num].fd = fd;
		client_num++;

		set_fd(fd);

		return;
	}

	shutdown(fd, 2);
	close(fd);
}

static	void	disconnect_client(num)
int	num;
{
	WorkArea *wp;
	StdyFile *sp;
	int	fd;

	if (sp = client[num].stdy) closestdy(sp);
	if (wp = client[num].work) free_workarea(wp);

	debug_out(1, "client dead(%d)\r\n", client[num].fd);
	logging_out("disconnect from client on %d", client[num].fd);
	shutdown(fd = client[num].fd, 2);
	close(fd);
	clr_fd(fd);

	client_num--;
	client[num] = client[client_num];
}

void	communicate()
{
	signal(SIGPIPE, SIG_IGN);

	for ( ; ; ) {
		fd_set	readfds;

		readfds = fds_org;
		while (select(maxfds,&readfds,NULL,NULL,NULL) == ERROR) {
			if (errno != EINTR) error_out("select");
			errno = 0;
		}

		for (cur_client = 0 ; cur_client < client_num ; ) {

			if (setjmp(client_dead)) {
				disconnect_client(cur_client);
				continue;
			}
			if (FD_ISSET(client[cur_client].fd, &readfds)) {
				client_fd = client[cur_client].fd;
				putpos = buflen = getpos = 0;
				execute_cmd();
			}
			cur_client++;
		}

		connect_client(&readfds);
	}
}

void	put_flush()
{
	register int	len;
	register int	i;
	register char	*p;

	for (len = putpos, p = putbuf ; len > 0 ; ) {
		if ((i = write(client_fd, p, len)) == ERROR) {
			longjmp(client_dead, -1);
		}
		len -= i;
		p += i;
	}
	putpos = 0;
}
void	put_byte(c)
register int	c;
{
	putbuf[putpos++] = c;
	if (putpos >= sizeof(putbuf)) put_flush();
}
void	put_work(c)
register int	c;
{
	put_byte(c >> 8);
	put_byte(c & 0xff);
}
void	put_int(c)
register int	c;
{
	put_byte(c >> (8 * 3));
	put_byte(c >> (8 * 2));
	put_byte(c >> (8 * 1));
	put_byte(c);
}
Uchar	*put_string(p)
register Uchar	*p;
{
	if (p) {
		do {
			put_byte(*p);
		} while (*p++);
	}
	else
		put_byte(0);
	return p;
}
Uchar	*put_ndata(p, n)
register Uchar	*p;
register int	n;
{
	while (n-- > 0) put_byte(p ? *p++ : 0);
	return p;
}

void	get_buf()
{
	register int	i;

	for (;;) {
		if ((i = read(client_fd, getbuf, sizeof(getbuf))) > 0) break;
		if (i == ERROR) {
			if (errno == EINTR) continue;
			if (errno == EWOULDBLOCK) continue;
		}
		longjmp(client_dead, -1);
	}
	buflen = i;
	getpos = 0;
}
int	get_byte()
{
	if (getpos >= buflen) get_buf();
	return (getbuf[getpos++] & 0xff);
}
int	get_word()
{
	register int	i;

	i = get_byte();
	return ((i << 8) | get_byte());
}
int	get_int()
{
	register int	i0;
	register int	i1;
	register int	i2;

	i0 = get_byte();
	i1 = get_byte();
	i2 = get_byte();
	return((i0 << (8*3)) | (i1 << (8*2)) | (i2 << (8*1)) | get_byte());
}
int	get_nstring(p, n)
register Uchar	*p;
register int	n;
{
	register int	c;

	do {
		c = get_byte();
		if (n-- > 0) *p++ = c;
	} while (c);

	return (n < 0) ? ERROR : 0;
}
Uchar	*get_ndata(p, n)
register Uchar	*p;
register int	n;
{
	while (n-- > 0) *p++ = get_byte();
	return p;
}

