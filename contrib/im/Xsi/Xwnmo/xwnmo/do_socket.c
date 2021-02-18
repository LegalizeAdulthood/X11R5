/*
 * $Id: do_socket.c,v 1.5 1991/09/16 21:36:39 ohm Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				

#include <stdio.h>
#include <X11/Xlib.h>
#ifdef	X11R4
#define	NEED_EVENTS	1
#include <X11/Xlibos.h>
#else	/* X11R4 */
#include "Xlibnet.h"
#endif	/* X11R4 */
#include <X11/Xos.h>
#if defined(TCPCONN) || defined(UNIXCONN)
#include <sys/socket.h>
#endif /* defined(TCPCONN) || defined(UNIXCONN) */
#ifdef	TCPCONN
#define	XIM_PORT_IN    0x9495
#endif	/* TCPCONN */

#ifdef	UNIXCONN
#include <sys/un.h>
#ifndef	XIM_UNIX_PATH
#define	XIM_UNIX_PATH	"/tmp/.X11-unix/XIM"
#endif	/* XIM_UNIX_PATH */
#endif	/* UNIXCONN */

#include "sdefine.h"
#include "xim.h"
#include "proto.h"
#include "ext.h"

#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
#define	MAX_ACCEPT	5
#else
#define	MAX_ACCEPT	3
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */

#define	UNIX_ACPT	0
#define INET_ACPT	1
#define SERVER_ACPT	2

char my_byteOrder;

struct	cmblk	{
    int		sd;
    int		use;
    Bool	byteOrder;
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
    int		xjp;
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
};

struct  cmblk	accept_blk[MAX_ACCEPT];
struct  cmblk	*cblk;
struct  cmblk	*cur_cblk = NULL;

int	*all_socks;
int	*ready_socks;
static int	*dummy1_socks, *dummy2_socks;
static int	sel_width;

static int	nofile;
int	max_client;
int	cur_sock;
static int	rest_length = 0;

#define SEND_BUF_SZ	128
#define	RECV_BUF_SZ	128

static char	send_buf[SEND_BUF_SZ];
static int	sp = 0;
static char	recv_buf[RECV_BUF_SZ];
static int	rp = 0;
static int	rc = 0;

#define	BINTSIZE	(sizeof(int)*8)
#define	sock_set(array,pos)	(array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define	sock_clr(array,pos)	(array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define	sock_tst(array,pos)	(array[pos/BINTSIZE] & (1<<(pos%BINTSIZE)))

extern int read(), write();

#define	_Read(fd, data, size)	read((fd), (data), (size))
#define	_Write(fd, data, size)	write((fd), (data), (size))

static void
_Error()
{
    close_socket(cur_sock);
}

static int
_Writen(num)
register int num;
{
    register int i, ret;

    for (i = 0; i< num;) {
	ret = _Write(cur_sock, &send_buf[i], num - i);
	if (ret <= 0) {
	    _Error();
	    return(-1);
	}
	i += ret;
    }
    return(0);
}

int
_Send_Flush()
{
    if (sp == 0) {
	return(0);
    }
    if (_Writen(sp) == -1) {
	sp = 0;
	return(-1);
    }
    sp = 0;
    return(0);
}

int
_WriteToClient(p, num)
register char *p;
register int num;
{
    if (num <= 0) return(0);
    for (;;) {
	if ((sp + num) > SEND_BUF_SZ) {
	    bcopy(p, &send_buf[sp], SEND_BUF_SZ - sp);
	    if (_Writen(SEND_BUF_SZ) == -1) {
		sp = 0;
		return(-1);
	    }
	    num -= (SEND_BUF_SZ - sp);
	    p += (SEND_BUF_SZ - sp);
	    sp = 0;
	} else {
	    bcopy(p, &send_buf[sp], num);
	    sp += num;
	    return(0);
	}
    }
}

int
_ReadFromClient(p, num)
register char *p;
register int num;
{
    register char *x = p;

    if (num <= 0) return(0);
    for (;;) {
	if (num > rc) {
	    if (rc > 0) {
		bcopy(&recv_buf[rp], x, rc);
		x += rc;
		num -= rc;
		rc = 0;
	    }
	    rc = _Read(cur_sock, recv_buf, RECV_BUF_SZ);
	    if (rc <= 0) {
		_Error();
		rp = 0;
		rc = 0;
		return(-1);
	    }
	    rp = 0;
	} else {
	    bcopy(&recv_buf[rp], x, num);
	    rc -= num;
	    rp += num;
	    return(0);
	}
    }
}

static void
init_net_area()
{
    int sel_w;
    int sel_bwidth;

    nofile = NOFILE;
    sel_w = (nofile - 1)/BINTSIZE + 1;
    all_socks = (int *)Calloc(sel_w, sizeof(int));
    ready_socks = (int *)Malloc(sel_w * sizeof(int));
    dummy1_socks = (int *)Malloc(sel_w * sizeof(int));
    dummy2_socks = (int *)Malloc(sel_w * sizeof(int));
    sel_width = sel_w * sizeof(int);
    sel_bwidth = sel_width * 8;

    max_client = NOFILE - MAX_ACCEPT;
    cblk = (struct cmblk *)Malloc(max_client * sizeof(struct cmblk));
    bzero((char *)cblk, max_client * sizeof(struct cmblk));
    bzero((char *)accept_blk, MAX_ACCEPT * sizeof(struct cmblk));
#ifdef	UNIXCONN
    accept_blk[UNIX_ACPT].sd = -1;
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    accept_blk[INET_ACPT].sd = -1;
#endif	/* TCPCONN */
}

#ifdef	UNIXCONN
static int
init_net_un()
{
    int so;
    struct sockaddr_un  saddr_un;
    extern int unlink(), socket(), bind(), shutdown(), listen();

    unlink(XIM_UNIX_PATH);
    saddr_un.sun_family = AF_UNIX;
    strcpy(saddr_un.sun_path, XIM_UNIX_PATH);
  
    if((so = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	return(-1);
    }
    if(bind(so, &saddr_un, strlen(saddr_un.sun_path) + 2) < 0) {
	shutdown(so, 2);
	return(-1);
    }
    if(listen(so, 5) < 0) {
	shutdown(so, 2);
	return(-1);
    }
    accept_blk[UNIX_ACPT].sd = so;
    sock_set(all_socks, so);
    return(0);
}
#endif	/* UNIXCONN */

#ifdef	TCPCONN
static short
init_net_in()
{
    int so;
    struct servent	*sp;
    struct sockaddr_in  saddr_in;
    unsigned short port;
    int on = 1;
    int i, ok = 0;
    extern int setsockopt();

    if ((sp = getservbyname("xim","tcp")) == NULL) {
	port = XIM_PORT_IN;
    } else {
	port = ntohs(sp->s_port);
    }
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_port = htons(port);
    saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  
    for (i = 0; i < MAX_PORT_NUMBER; i++) {
	if((so = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	    return((short)-1);
	}
	setsockopt(so, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	if(bind(so, &saddr_in, sizeof(struct sockaddr_in)) == 0) {
	    ok = 1;
	    break;
	}
	shutdown(so);
	saddr_in.sin_port = htons(++port);
    }
    if (ok == 0) {
	return((short)-1);
    }
    if(listen(so, 5) < 0) {
	shutdown(so);
	return((short)-1);
    }
    accept_blk[INET_ACPT].sd = so;
    sock_set(all_socks, so);
    return((short)port);
}
#endif	/* TCPCONN */

short
init_net(fd, displayname)
int fd;
char *displayname;
{
    short port = -1;
    int indian = 1;

    if (*(char *) &indian) {
	my_byteOrder = 'l';
    } else {
	my_byteOrder = 'B';
    }
    init_net_area();
#ifdef	UNIXCONN
    if (!strncmp(displayname, "unix:", 5) || !strncmp(displayname, ":", 1)) {
	if (init_net_un() == -1) {
	    return(port);
	}
    }
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    if ((port = init_net_in()) == (short)-1) {
	return(port);
    }
    accept_blk[SERVER_ACPT].sd = fd;
    sock_set(all_socks, fd);
#endif	/* TCPCONN */
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
    XJp_init_net();
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
    return(port);
}

void
close_net()
{
    int i;
    int	trueFlag = 1;
#ifdef	UNIXCONN
    struct sockaddr_un addr_un;
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    struct sockaddr_in addr_in;
#endif	/* TCPCONN */
#if defined(UNIXCONN) || defined(TCPCONN)
    int	addrlen;
    extern int accept(), close();
#endif

#ifdef	UNIXCONN
    if (accept_blk[UNIX_ACPT].sd != -1) {
	ioctl(accept_blk[UNIX_ACPT].sd, FIONBIO, &trueFlag);
	for (;;) {
	    addrlen = sizeof(addr_un);
	    if (accept(accept_blk[UNIX_ACPT].sd, &addr_un, &addrlen) < 0) break;
	}
	shutdown(accept_blk[UNIX_ACPT].sd, 2);
	close(accept_blk[UNIX_ACPT].sd);
    }
#endif	/* UNIXCONN */

#ifdef	TCPCONN
    if (accept_blk[INET_ACPT].sd != -1) {
	ioctl(accept_blk[INET_ACPT].sd, FIONBIO, &trueFlag);
	for (;;) {
	    addrlen = sizeof(addr_in);
	    if (accept(accept_blk[INET_ACPT].sd, &addr_in, &addrlen) < 0) break;
	}
	shutdown(accept_blk[INET_ACPT].sd, 2);
	close(accept_blk[INET_ACPT].sd);
    }
#endif	/* TCPCONN */
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
    XJp_close_net();
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */

    for (i = 0; i < max_client; i++) {
	if (cblk[i].use) {
	    shutdown(cblk[i].sd, 2);
	    close(cblk[i].sd);
	}
    }
}

#if defined(UNIXCONN) || defined(TCPCONN)
static void
new_cl_sock(b)
struct cmblk *b;
{
    char displaybuf[256];
    ximConnClient client;
    ximNormalReply reply;

    if (_ReadFromClient(&client, sz_ximConnClient) == -1) return;
    if (_ReadFromClient(displaybuf, client.length) == -1) return;
    if (my_byteOrder != client.byteOrder) {
	b->byteOrder = True;
    } else {
	b->byteOrder = False;
    }
    reply.state = 0;
    if (_WriteToClient(&reply, sz_ximNormalReply) == -1) return;
    if (_Send_Flush() == -1) return;
}
#endif

static int
new_client_acpt(fd)
int fd;
{
#ifdef	UNIXCONN
    struct sockaddr_un addr_un;
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    struct sockaddr_in addr_in;
#endif	/* TCPCONN */
#if defined(UNIXCONN) || defined(TCPCONN)
    register int i;
    int	addrlen;
#endif

#ifdef	UNIXCONN
    if (accept_blk[UNIX_ACPT].sd != -1) {
	if (fd == accept_blk[UNIX_ACPT].sd) {
	    for (i = 0 ; i < max_client ; i++) {
		if (cblk[i].use == 0) {
		    addrlen = sizeof(addr_un);
		    if((cblk[i].sd = accept(fd, &addr_un, &addrlen)) < 0) {
			return(-1);
		    }
		    cur_cblk = &cblk[i];
		    cur_cblk->use = 1;
		    cur_sock = cur_cblk->sd;
		    sock_set(all_socks, cur_sock);
		    new_cl_sock(cur_cblk);
		    return(0);
		}
	    }
	}
    }
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    if (fd == accept_blk[INET_ACPT].sd) {
	for (i = 0 ; i < max_client ; i++) {
	    if (cblk[i].use == 0) {
		addrlen = sizeof(addr_in);
		if((cblk[i].sd = accept(fd, &addr_in, &addrlen)) < 0) {
		    return(-1);
		}
		cur_cblk = &cblk[i];
		cur_cblk->use = 1;
		cur_sock = cur_cblk->sd;
		sock_set(all_socks, cur_sock);
		new_cl_sock(cur_cblk);
		break;
	    }
	}
    }
#endif	/* TCPCONN */
    return(0);
}

int
wait_for_socket()
{
    register int i;
    int n, ret;
    extern int select();

    bcopy(all_socks, ready_socks, sel_width);
    bzero(dummy1_socks, sel_width);
    bzero(dummy2_socks, sel_width);

    n = select(nofile, ready_socks, dummy1_socks, dummy2_socks, 
	       (struct timeval *)NULL);
#ifdef	UNIXCONN
    if (accept_blk[UNIX_ACPT].sd != -1) {
	if (sock_tst(ready_socks, accept_blk[UNIX_ACPT].sd)) {
	    sock_clr(ready_socks, accept_blk[UNIX_ACPT].sd);
	    new_client_acpt(accept_blk[UNIX_ACPT].sd);
	    return(0);
	}
    }
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    if (accept_blk[INET_ACPT].sd != -1) {
	if (sock_tst(ready_socks, accept_blk[INET_ACPT].sd)) {
	    sock_clr(ready_socks, accept_blk[INET_ACPT].sd);
	    new_client_acpt(accept_blk[INET_ACPT].sd);
	    return(0);
	}
    }
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
    if (ret = XJp_wait_for_socket()) return(ret);
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
#endif	/* TCPCONN */
    if (sock_tst(ready_socks, accept_blk[SERVER_ACPT].sd)) {
	sock_clr(ready_socks, accept_blk[SERVER_ACPT].sd);
	cur_sock = accept_blk[SERVER_ACPT].sd;
	return(XEVENT_TYPE);
    } else {
	if (sock_tst(ready_socks, cur_sock)) {
	    sock_clr(ready_socks, cur_sock);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
	    if (cur_cblk && cur_cblk->xjp) return(XJP_DIRECT_TYPE);
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
	    return(REQUEST_TYPE);
	}
	for (i = 0; i < max_client; i++) {
	    if (cblk[i].use && sock_tst(ready_socks, cblk[i].sd)) {
		sock_clr(ready_socks, cblk[i].sd);
		cur_cblk = &cblk[i];
		cur_sock = cur_cblk->sd;
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
		if (cur_cblk && cur_cblk->xjp) return(XJP_DIRECT_TYPE);
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
		return(REQUEST_TYPE);
	    }
	}
	return(0);
    }
}

void
close_socket(fd)
int	fd;
{
    int	i;
    for (i = 0 ; i < max_client ; i++) {
	if (fd == cblk[i].sd && cblk[i].use == 1) {
	    cblk[i].use = 0;
	    shutdown(cblk[i].sd, 2);
	    sock_clr(all_socks, cblk[i].sd);
	    close(cblk[i].sd);
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
	    if (cblk[i].xjp)
		XJp_destroy_for_sock(cblk[i].sd);
	    else
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
	    destroy_for_sock(cblk[i].sd);
	    return;
	}
    }
}

int
get_cur_sock()
{
    return(cur_sock);
}

int
get_rest_len()
{
    return(rest_length);
}

int
read_requestheader()
{
    ximRequestHeader request_header;

    if (_ReadFromClient(&request_header, sz_ximRequestHeader) == -1) {
	return(-1);
    } else {
	if (need_byteswap() == True)
	    request_header.length = byteswap_s(request_header.length);
	rest_length = request_header.length - sz_ximRequestHeader;
	return(request_header.reqType);
    }
}

int
read_strings(p, p_len, r_len)
char **p;
int *p_len, r_len;
{
    if (r_len > 0) {
	if (*p_len <= r_len) *p = Realloc(*p, (*p_len = (r_len + 1)));
	if (_ReadFromClient(*p, r_len) == 1) return(-1);
	p[0][r_len] = '\0';
    } else {
	if (*p_len > 0) **p = '\0';
    }
    return(0);
}

Bool
need_byteswap()
{
    return(cur_cblk->byteOrder);
}
