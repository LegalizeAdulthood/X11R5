/*
 * $Id: do_xjpdrct.c,v 1.3 1991/09/16 21:36:40 ohm Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
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

#if defined(XJPLIB) && defined(XJPLIB_DIRECT)

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
#define	XJP_PORT_IN    0x9494
#endif	/* TCPCONN */

#ifdef	UNIXCONN
#include <sys/un.h>
#ifndef	XJP_UNIX_PATH
#define	XJP_UNIX_PATH	"/tmp/xwnmo.V2"
#endif	/* XJP_UNIX_PATH */
#endif	/* UNIXCONN */

#include "do_xjplib.h"

#define	XJP_UNIX_ACPT	3
#define XJP_INET_ACPT	4

struct	cmblk	{
    int		sd;
    int		use;
    Bool	byteOrder;
    int		xjp;
};

extern struct  cmblk	accept_blk[];
extern struct  cmblk	*cblk;
extern struct cmblk	*cur_cblk;

extern int	*all_socks;
extern int	*ready_socks;

extern int	max_client;
extern int	cur_sock;

#define	BINTSIZE	(sizeof(int)*8)
#define	sock_set(array,pos)	(array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define	sock_clr(array,pos)	(array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define	sock_tst(array,pos)	(array[pos/BINTSIZE] & (1<<(pos%BINTSIZE)))

extern int read(), write();

extern XJpClientRec *xjp_clients;
extern XJpClientRec *xjp_cur_client;
extern XJpInputRec *xjp_inputs;
extern XJpInputRec *xjp_cur_input;

static int need_write = 0;

int
XJp_get_xjp_port()
{
    return(XJP_PORT_IN);
}

static void
xjp_init_net_area()
{
#ifdef	UNIXCONN
    accept_blk[XJP_UNIX_ACPT].sd = -1;
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    accept_blk[XJP_INET_ACPT].sd = -1;
#endif	/* TCPCONN */
}

static void
xjp_init_net_un()
{
    int so;
    struct sockaddr_un  saddr_un;
    extern int unlink(), socket(), bind(), shutdown(), listen();

    unlink(XJP_UNIX_PATH);
    saddr_un.sun_family = AF_UNIX;
    strcpy(saddr_un.sun_path, XJP_UNIX_PATH);
  
    if((so = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
	return;
    }
    if(bind(so, &saddr_un, strlen(saddr_un.sun_path) + 2) < 0) {
	shutdown(so, 2);
	return;
    }
    if(listen(so, 5) < 0) {
	shutdown(so, 2);
	return;
    }
    accept_blk[XJP_UNIX_ACPT].sd = so;
    sock_set(all_socks, so);
    return;
}

static void
xjp_init_net_in()
{
    int so;
    struct servent	*sp;
    struct sockaddr_in  saddr_in;
    unsigned short port;
    int on = 1;
    int i, ok = 0;
    extern int setsockopt();

    if ((sp = getservbyname("xwnmo","tcp")) == NULL) {
	port = XJP_PORT_IN;
    } else {
	port = ntohs(sp->s_port);
    }
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_port = htons(port);
    saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
  
    if((so = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
	return;
    }
    setsockopt(so, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
    if(bind(so, &saddr_in, sizeof(struct sockaddr_in)) < 0) {
	shutdown(so);
	return;
    }
    if(listen(so, 5) < 0) {
	shutdown(so);
	return;
    }
    accept_blk[XJP_INET_ACPT].sd = so;
    sock_set(all_socks, so);
    return;
}

void
XJp_init_net()
{
    xjp_init_net_area();
#ifdef	UNIXCONN
    xjp_init_net_un();
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    xjp_init_net_in();
#endif	/* TCPCONN */
}

void
XJp_close_net()
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
    if (accept_blk[XJP_UNIX_ACPT].sd != -1) {
	ioctl(accept_blk[XJP_UNIX_ACPT].sd, FIONBIO, &trueFlag);
	for (;;) {
	    addrlen = sizeof(addr_un);
	    if (accept(accept_blk[XJP_UNIX_ACPT].sd, &addr_un, &addrlen) < 0)
		break;
	}
	shutdown(accept_blk[XJP_UNIX_ACPT].sd, 2);
	close(accept_blk[XJP_UNIX_ACPT].sd);
    }
#endif	/* UNIXCONN */

#ifdef	TCPCONN
    if (accept_blk[XJP_INET_ACPT].sd != -1) {
	ioctl(accept_blk[XJP_INET_ACPT].sd, FIONBIO, &trueFlag);
	for (;;) {
	    addrlen = sizeof(addr_in);
	    if (accept(accept_blk[XJP_INET_ACPT].sd, &addr_in, &addrlen) < 0) break;
	}
	shutdown(accept_blk[XJP_INET_ACPT].sd, 2);
	close(accept_blk[XJP_INET_ACPT].sd);
    }
#endif	/* TCPCONN */
}

void
XJp_return_sock(state, detail, open, keysym, string)
short state, detail;
int open;
KeySym keysym;
char *string;
{
    unsigned short tmp_short;
    unsigned long tmp_long;

    tmp_short = (unsigned short)htons(state);
    if (_WriteToClient(&tmp_short, sizeof(short)) == -1) return;
    tmp_short = (unsigned short)htons(detail);
    if (_WriteToClient(&tmp_short, sizeof(short)) == -1) return;

    if (open == 0 && state == (short)0) {
	tmp_long = (unsigned long)htonl(keysym);
	if (_WriteToClient(tmp_long, sizeof(KeySym)) == -1) return;
	if (detail > (short)0) {
	    if (_WriteToClient(string, (int)detail) == -1) return;
	}
	need_write = 0;
    }
    _Send_Flush();
}

static int
xjp_new_cl_sock(fd)
int fd;
{
    register XJpClientRec *xjp;
    register XIMClientRec *xc;
    short detail;
    char buf[32];
    ximICValuesReq ic_req;
    ximICAttributesReq pre_req, st_req;
    XCharStruct cs;

    if ((_ReadFromClient(buf, 2) == -1) ||
	(_ReadFromClient(buf, (int)buf[1]) == -1)) {
	return(-1);
    }
    if ((xjp = (XJpClientRec *)Malloc(sizeof(XJpClientRec))) == NULL) {
	XJp_return_sock((short)-1, (short)7, 1, (KeySym)0, NULL);
	return(-1);
    }
    xjp->dispmode = XJP_ROOT;
    xjp->w = 0;
    XJp_xjp_to_xim(xjp, &ic_req, &pre_req, &st_req, &cs);
    if ((xc = create_client(&ic_req, &pre_req, &st_req, NULL, NULL,
			    DEFAULT_LANG, NULL, NULL, NULL, &detail)) == NULL){
	Free((char*)xjp);
	XJp_return_sock((short)-1, (short)7, 1, (KeySym)0, NULL);
	return(-1);
    }
    xc->xjp = 1;
    xjp->xim_client = xc;
    xjp->direct_fd = fd;
    xjp->next = xjp_clients;
    xjp->ref_count = 0;
    xjp_clients = xjp;
    XJp_return_sock((short)0, (short)0, 1, (KeySym)0, NULL);
    return(0);
}

static int
xjp_new_client_acpt(fd)
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
    if (accept_blk[XJP_UNIX_ACPT].sd != -1) {
	if (fd == accept_blk[XJP_UNIX_ACPT].sd) {
	    for (i = 0 ; i < max_client ; i++) {
		if (cblk[i].use == 0) {
		    addrlen = sizeof(addr_un);
		    if((cblk[i].sd = accept(fd, &addr_un, &addrlen)) < 0) {
			return(-1);
		    }
		    cur_cblk = &cblk[i];
		    cur_cblk->use = 1;
		    cur_cblk->xjp = 1;
		    cur_cblk->byteOrder = False;
		    cur_sock = cur_cblk->sd;
		    if (xjp_new_cl_sock(cur_sock) == 0) {
			sock_set(all_socks, cur_sock);
		    }
		    return(0);
		}
	    }
	}
    }
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    if (fd == accept_blk[XJP_INET_ACPT].sd) {
	for (i = 0 ; i < max_client ; i++) {
	    if (cblk[i].use == 0) {
		addrlen = sizeof(addr_in);
		if((cblk[i].sd = accept(fd, &addr_in, &addrlen)) < 0) {
		    return(-1);
		}
		cur_cblk = &cblk[i];
		cur_cblk->use = 1;
		cur_cblk->xjp = 1;
		cur_cblk->byteOrder = False;
		cur_sock = cur_cblk->sd;
		if (xjp_new_cl_sock(cur_sock) == 0) {
		    sock_set(all_socks, cur_sock);
		}
		break;
	    }
	}
    }
#endif	/* TCPCONN */
    return(0);
}

int
XJp_wait_for_socket()
{
#ifdef	UNIXCONN
    if (accept_blk[XJP_UNIX_ACPT].sd != -1) {
	if (sock_tst(ready_socks, accept_blk[XJP_UNIX_ACPT].sd)) {
	    sock_clr(ready_socks, accept_blk[XJP_UNIX_ACPT].sd);
	    xjp_new_client_acpt(accept_blk[XJP_UNIX_ACPT].sd);
	    return(XJP_DIRECT_TYPE);
	}
    }
#endif	/* UNIXCONN */
#ifdef	TCPCONN
    if (accept_blk[XJP_INET_ACPT].sd != -1) {
	if (sock_tst(ready_socks, accept_blk[XJP_INET_ACPT].sd)) {
	    sock_clr(ready_socks, accept_blk[XJP_INET_ACPT].sd);
	    xjp_new_client_acpt(accept_blk[XJP_INET_ACPT].sd);
	    return(XJP_DIRECT_TYPE);
	}
    }
#endif	/* TCPCONN */
    return(0);
}

void
XJp_destroy_for_sock(fd)
int fd;
{
    XJpClientRec *p, **prev_p;
    XJpInputRec *i, **prev_i;

    for (prev_p = &xjp_clients; p = *prev_p; prev_p = &p->next) {
	if (p->direct_fd != -1 && p->direct_fd == fd) {
	    destroy_client(p->xim_client);
	    for (prev_i = &xjp_inputs; i = *prev_i;) {
		if (i->pclient == p) {
		    XSelectInput(dpy, i->w, NoEventMask);
		    *prev_i = i->next;
		    XFree((char *)i);
		} else {
		    prev_i = &i->next;
		}
	    }
	    XSelectInput(dpy, p->w, NoEventMask);
	    *prev_p = p->next;
	    XFree((char *)p);
	    return;
	}
    }
}

static void
xjp_buffer_to_xevent(buf, ev)
char *buf;
XEvent *ev;
{
    ev->xkey.display = dpy;
    ev->xkey.type = (int)buf[0];
    ev->xkey.state = (unsigned int)buf[1];
    ev->xkey.serial = (unsigned long)ntohs(*(short *)(buf + 2));
    ev->xkey.time = (Time)ntohl(*(Time *)(buf + 4));
    ev->xkey.root = (Window)ntohl(*(Window *)(buf + 8));
    ev->xkey.window = (Window)ntohl(*(Window *)(buf + 12));
    ev->xkey.subwindow = (Window)ntohl(*(Window *)(buf + 16));
    ev->xkey.x_root = (int)ntohs(*(short *)(buf + 20));
    ev->xkey.y_root = (int)ntohs(*(short *)(buf + 22));
    ev->xkey.x = (int)ntohs(*(short *)(buf + 24));
    ev->xkey.y = (int)ntohs(*(short *)(buf + 26));
    ev->xkey.keycode = (unsigned int)ntohs(*(short *)(buf + 28));
    ev->xkey.same_screen = (Bool)buf[30];
}

static int
xjp_get_event(ev)
XEvent *ev;
{
    char readbuf[64];
    register XJpClientRec *p;
    register XJpInputRec *i;
    int len;

    if (_ReadFromClient(readbuf, 32) == -1) {
	return(-1);
    }
    xjp_buffer_to_xevent(readbuf, ev);
    for (p = xjp_clients; p; p = p->next) {
	if (p->direct_fd != -1 && p->direct_fd == cur_sock) {
	    if (p->w == (Window)0) {
		p->w = ev->xkey.window;
		XSelectInput(dpy, p->w, StructureNotifyMask);
		if ((i = (XJpInputRec *)Malloc(sizeof(XJpInputRec))) == NULL) {
		    return(-1);
		}
		i->w = ev->xkey.window;
		i->pclient = p;
		i->save_event = 0;
		i->next = xjp_inputs;
		xjp_inputs = i;
	    }
	    return(0);
	}
    }
    return(-1);
}

void
XJp_Direct_Dispatch()
{
    XEvent ev;
    int buff[32];
    int ret, i;

    if (xjp_get_event(&ev) == -1) return;
    ret = key_input(buff, &ev);
    for (i = 0; i < ret; i++) {
	in_put(buff[i]);
    }
    if (need_write) {
	XJp_return_sock((short)0, (short)0, 0, (KeySym)0, NULL);
    }
    return;
}

static KeySym save_sock_keysym = (KeySym)0;
static unsigned char save_sock_str[32];
static unsigned int save_sock_str_len = 0;

void
XJp_save_sockbuf(len, str, keysym)
int len;
unsigned char *str;
KeySym keysym;
{
    if (xjp_cur_client && xjp_cur_client->direct_fd != -1) {
	need_write = 1;
	save_sock_keysym = keysym;
	if (len > 0) {
	    bcopy(str, save_sock_str, len);
	    save_sock_str_len = len;
	}
    }
}

void
XJp_direct_send_cl_key()
{
    XJp_return_sock((short)0, (short)save_sock_str_len, 0, save_sock_keysym,
		    save_sock_str);
}

#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
