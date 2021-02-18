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

#include <stdio.h>
#include <setjmp.h>
#include <netdb.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "Const.h"
#include "sj3cmd.h"
#include "sj3err.h"
#include "sj3lib.h"
#include "sj3lowlib.h"

char	*getlogin();

#ifndef lint
static	char	rcsid_sony[] = "$Header: level1.c,v 1.1 91/09/04 23:06:19 root Locked $ SONY;";
#endif

int	sj3_error_number;
char	*sj3_socket_name	= SocketName;
char	*sj3_port_name	= PortName;
int	sj3_port_number	= PortNumber;

static	char	*af_unix_str = "unix";
static	u_char	putbuf[BUFSIZ];
static	u_char	getbuf[BUFSIZ];
static	int	putpos = 0;
static	int	getlen = 0;
static	int	getpos = 0;
static	jmp_buf	server_dead;
static	SJ3_CLIENT_ENV	*cliptr;
static	int	server_fd = -1;

#define	client_init(p)	{ \
		cliptr = (p); \
		if ((server_fd = cliptr -> fd) == -1) { \
			sj3_error_number = SJ3_NotOpened; \
			return ERROR; \
		} \
		if (setjmp(server_dead)) return ERROR; \
	}

static	server_broken()
{
	shutdown(server_fd, 2);
	close(server_fd);
	cliptr -> fd = server_fd = -1;
	sj3_error_number = SJ3_ServerDown;
	if (cliptr -> serv_dead_flg) longjmp(cliptr -> serv_dead, ERROR);
	longjmp(server_dead, ERROR);
}

static	put_flush()
{
	register int	i, j;
	register u_char	*p;

	for (i = putpos, p = putbuf ; i > 0 ; i -= j, p += j) {
		if ((j = write(server_fd, p, i)) <= 0) server_broken();
	}
	putpos = 0;
}
static	put_byte(c)
register int	c;
{
	putbuf[putpos++] = c;
	if (putpos >= sizeof(putbuf)) put_flush();
}
static	put_word(c)
register int	c;
{
	put_byte(c >> 8);
	put_byte(c & 0xff);
}
static	put_int(c)
register int	c;
{
	put_byte(c >> (8 * 3));
	put_byte(c >> (8 * 2));
	put_byte(c >> (8 * 1));
	put_byte(c);
}
static	put_cmd(cmd)
register int	cmd;
{
	putpos = getlen = 0;
	put_int(cmd);
}
static	u_char	*put_string(p)
register u_char	*p;
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
static	u_char	*put_ndata(p, n)
register u_char	*p;
register int	n;
{
	while (n-- > 0) put_byte(p ? *p++ : 0);
	return p;
}

static	get_byte()
{
	if (getpos >= getlen) {
		getpos = getlen = 0;
		getlen = read(server_fd, getbuf, sizeof(getbuf));
		if (getlen <= 0) server_broken();
	}
	return (getbuf[getpos++] & 0xff);
}
static	get_word()
{
	register int	i;

	i = get_byte();
	return ((i << 8) | get_byte());
}
static	get_int()
{
	register int	i0;
	register int	i1;
	register int	i2;

	i0 = get_byte();
	i1 = get_byte();
	i2 = get_byte();
	return ((i0 << (8*3)) | (i1 << (8*2)) | (i2 << (8*1)) | get_byte());
}
static	u_char	*get_string(p)
register u_char	*p;
{
	register int	c;

	do {
		*p++ = c = get_byte();
	} while (c);

	return p;
}
static	get_nstring(p, n)
register u_char	*p;
register int	n;
{
	register int	c;

	c = get_byte();
	while (c) {
		if (n > 1) { *p++ = c; n--; }
		c = get_byte();
	}
	if (n > 0) *p++ = 0;
	return n;
}
static	u_char	*get_ndata(p, n)
register u_char	*p;
register int	n;
{
	while (n-- > 0) *p++ = get_byte();
	return p;
}
static	skip_string()
{
	while (get_byte()) ;
}
static	skip_ndata(n)
register int	n;
{
	while (n-- > 0) get_byte();
}

static	open_unix()
{
	int	fd;
	struct	sockaddr_un	sosun;

	sosun.sun_family      = AF_UNIX;
	strcpy(sosun.sun_path, sj3_socket_name);

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == ERROR) {
		sj3_error_number = SJ3_OpenSocket;
		return ERROR;
	}

	if (connect(fd, &sosun, strlen(sosun.sun_path)+sizeof(sosun.sun_family))
			== ERROR) {
		sj3_error_number = SJ3_ConnectSocket;
		return ERROR;
	}

	return fd;
}
static	open_inet(host)
char	*host;
{
	struct	hostent	*hp;
	struct	servent	*sp;
	int	port;
	struct	sockaddr_in	sin;
	int	fd;

	if (!(hp = gethostbyname(host))) {
		sj3_error_number = SJ3_GetHostByName;
		return ERROR;
	}

	if (sp = getservbyname(sj3_port_name, "tcp"))
		port = ntohs(sp -> s_port);
	else
		port = sj3_port_number;

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family      = AF_INET;
	sin.sin_port        = htons(port);
	bcopy(hp -> h_addr, &sin.sin_addr, hp -> h_length);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
		sj3_error_number = SJ3_OpenSocket;
		return ERROR;
	}

	if (connect(fd, &sin, sizeof(sin)) == ERROR) {
		sj3_error_number = SJ3_ConnectSocket;
		return ERROR;
	}

	return fd;
}

int	sj3_make_connection(client, serv, user, prog)
SJ3_CLIENT_ENV	*client;
char		*serv;
char		*user;
char		*prog;
{
	char	host[MAXHOSTNAMELEN];
	int	tmp;

	client -> fd = -1;

	if (!serv || *serv == '\0' || !strcmp(serv, af_unix_str)) {
		if ((server_fd = open_unix()) == ERROR) return ERROR;
		strcpy(host, af_unix_str);
	}
	else {
		if ((server_fd = open_inet(serv)) == ERROR) return ERROR;
		gethostname(host, sizeof(host));
	}

	client -> fd = server_fd;
	client_init(client);

	put_cmd(SJ3_CONNECT);
	put_int(SJ3SERV_VERSION_NO);
	put_string(host);
	put_string(user);
	put_string(prog);
	put_flush();

	if (tmp = get_int()) {
		sj3_erase_connection(client);
		sj3_error_number = tmp;
		return ERROR;
	}

	sj3_error_number = 0;

	put_cmd(SJ3_STDYSIZE);
	put_flush();

	if (tmp = get_int()) {
		sj3_erase_connection(client);
		sj3_error_number = tmp;
		return ERROR;
	}

	cliptr -> stdy_size = get_int();
	cliptr -> fd        = server_fd;

	return 0;
}
int	sj3_erase_connection(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_DISCONNECT);
	put_flush();

	sj3_error_number = get_int();
	close(server_fd);
	cliptr -> fd = -1;
	return sj3_error_number ? ERROR : 0;
}

long	sj3_open_dictionary(client, dictname, password)
SJ3_CLIENT_ENV	*client;
char	*dictname;
char	*password;
{
	client_init(client);

	put_cmd(SJ3_OPENDICT);
	put_string(dictname);
	put_string(password);
	put_flush();

	if (sj3_error_number = get_int()) return 0;
	return get_int();
}
int	sj3_close_dictionary(client, dicid)
SJ3_CLIENT_ENV	*client;
long	dicid;
{
	client_init(client);

	put_cmd(SJ3_CLOSEDICT);
	put_int(dicid);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_open_study_file(client, stdyname, password)
SJ3_CLIENT_ENV	*client;
char	*stdyname;
char	*password;
{
	client_init(client);

	put_cmd(SJ3_OPENSTDY);
	put_string(stdyname);
	put_string(password);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}
int	sj3_close_study_file(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_CLOSESTDY);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_get_id_size(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_STDYSIZE);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return (cliptr -> stdy_size = get_int());
}

int	sj3_lock_server(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_LOCK);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}
int	sj3_unlock_server(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_UNLOCK);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_ikkatu_henkan(client, src, dst, dstsiz)
SJ3_CLIENT_ENV	*client;
u_char	*src;
register u_char	*dst;
register int	dstsiz;
{
	register int	c;
	u_char	*top;
	int	result;
	int	len;
	int	len1;
	int	stysiz;

	client_init(client);

	put_cmd(SJ3_PH2KNJ);
	put_string(src);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	result = get_int();

	result = 0;
	stysiz = cliptr -> stdy_size;
	len1 =  1 + stysiz + 1 + 1;
	while (c = get_byte()) {

		top = dst;
		if (dstsiz < len1) goto error1;

		*dst++ = len = c;
		dst = get_ndata(dst, stysiz);
		dstsiz -= (stysiz + 1);

		while (c = get_byte()) {
			if (dstsiz-- < 3) goto error2;
			*dst++ = c;
		}
		*dst++ = 0;
		dstsiz--;
		result += len;
	}

	*dst = 0;
	return result;

error1:
	do {
		skip_ndata(stysiz);
error2:
		while (get_byte()) ;
	} while (get_byte());

	*top = 0;
	return result;
}

int	sj3_bunsetu_henkan(client, yomi, len, kanji)
SJ3_CLIENT_ENV	*client;
u_char	*yomi;
int	len;
u_char	*kanji;
{
	int	result;

	client_init(client);

	put_cmd(SJ3_CL2KNJ);
	put_int(len);
	put_ndata(yomi, len);
	put_byte(0);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	result = get_int();
	get_string( get_ndata(kanji, cliptr -> stdy_size) );
	return result;
}
int	sj3_bunsetu_jikouho(client, kanji, mode)
SJ3_CLIENT_ENV	*client;
u_char	*kanji;
int	mode;
{
	int	result;

	client_init(client);

	put_cmd(SJ3_NEXTCL);
	put_int(mode);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	result = get_int();
	get_string( get_ndata(kanji, cliptr -> stdy_size) );
	return result;
}
int	sj3_bunsetu_maekouho(client, kanji, mode)
SJ3_CLIENT_ENV	*client;
u_char	*kanji;
int	mode;
{
	int	result;

	client_init(client);

	put_cmd(SJ3_PREVCL);
	put_int(mode);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	result = get_int();
	get_string( get_ndata(kanji, cliptr -> stdy_size) );
	return result;
}

int	sj3_bunsetu_kouhosuu(client, yomi, len)
SJ3_CLIENT_ENV	*client;
u_char	*yomi;
int	len;
{
	client_init(client);

	put_cmd(SJ3_CL2KNJ_CNT);
	put_int(len);
	put_ndata(yomi, len);
	put_byte(0);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	return get_int();
}
int	sj3_bunsetu_zenkouho(client, yomi, len, douon)
SJ3_CLIENT_ENV	*client;
u_char	*yomi;
int	len;
SJ3_DOUON	*douon;
{
	int	cnt = 0;

	client_init(client);

	put_cmd(SJ3_CL2KNJ_ALL);
	put_int(len);
	put_ndata(yomi, len);
	put_byte(0);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	while (get_int()) {
                get_ndata(&(douon -> dcid), cliptr -> stdy_size);
		get_string(douon -> ddata);
		douon -> dlen = strlen(douon -> ddata);
		douon++;
		cnt++;
	}

	return cnt;
}

int	sj3_tango_gakusyuu(client, stdy)
SJ3_CLIENT_ENV	*client;
SJ3_STUDYREC	*stdy;
{
	client_init(client);

	put_cmd(SJ3_STUDY);
	put_ndata(stdy, cliptr -> stdy_size);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}
int	sj3_bunsetu_gakusyuu(client, yomi1, yomi2, stdy)
SJ3_CLIENT_ENV	*client;
u_char	*yomi1;
u_char	*yomi2;
SJ3_STUDYREC	*stdy;
{
	client_init(client);

	put_cmd(SJ3_CLSTUDY);
	put_string(yomi1);
	put_string(yomi2);
	put_ndata(stdy, cliptr -> stdy_size);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_tango_touroku(client, dicid, yomi, kanji, code)
SJ3_CLIENT_ENV	*client;
long	dicid;
u_char	*yomi;
u_char	*kanji;
int	code;
{
	client_init(client);

	put_cmd(SJ3_ADDDICT);
	put_int(dicid);
	put_string(yomi);
	put_string(kanji);
	put_int(code);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}
int	sj3_tango_sakujo(client, dicid, yomi, kanji, code)
SJ3_CLIENT_ENV	*client;
long	dicid;
u_char	*yomi;
u_char	*kanji;
int	code;
{
	client_init(client);

	put_cmd(SJ3_DELDICT);
	put_int(dicid);
	put_string(yomi);
	put_string(kanji);
	put_int(code);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_tango_syutoku(client, dicid, buf)
SJ3_CLIENT_ENV	*client;
int	dicid;
u_char	*buf;
{
	register u_char	*p;

	client_init(client);

	put_cmd(SJ3_GETDICT);
	put_int(dicid);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	p = get_string(buf);
	p = get_string(p);
	*p = get_int();
	return 0;
}
int	sj3_tango_jikouho(client, dicid, buf)
SJ3_CLIENT_ENV	*client;
int	dicid;
u_char	*buf;
{
	register u_char	*p;

	client_init(client);

	put_cmd(SJ3_NEXTDICT);
	put_int(dicid);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	p = get_string(buf);
	p = get_string(p);
	*p = get_int();
	return 0;
}
int	sj3_tango_maekouho(client, dicid, buf)
SJ3_CLIENT_ENV	*client;
int	dicid;
u_char	*buf;
{
	register u_char	*p;

	client_init(client);

	put_cmd(SJ3_PREVDICT);
	put_int(dicid);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;

	p = get_string(buf);
	p = get_string(p);
	*p = get_int();
	return 0;
}

int	sj3_make_dict_file(client, path, idxlen, seglen, segnum)
SJ3_CLIENT_ENV	*client;
char	*path;
int	idxlen;
int	seglen;
int	segnum;
{
	client_init(client);

	put_cmd(SJ3_MAKEDICT);
	put_string(path);
	put_int(idxlen);
	put_int(seglen);
	put_int(segnum);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_make_study_file(client, path, stynum, clstep, cllen)
SJ3_CLIENT_ENV	*client;
char	*path;
int	stynum;
int	clstep;
int	cllen;
{
	client_init(client);

	put_cmd(SJ3_MAKESTDY);
	put_string(path);
	put_int(stynum);
	put_int(clstep);
	put_int(cllen);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_make_directory(client, path)
SJ3_CLIENT_ENV	*client;
char	*path;
{
	client_init(client);

	put_cmd(SJ3_MAKEDIR);
	put_string(path);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_access(client, path, mode)
SJ3_CLIENT_ENV	*client;
char	*path;
int	mode;
{
	client_init(client);

	put_cmd(SJ3_ACCESS);
	put_string(path);
	put_int(mode);
	put_flush();

	sj3_error_number = 0;
	return get_int();
}

int	sj3_who(client, ret, num)
SJ3_CLIENT_ENV	*client;
SJ3_WHO_STRUCT	*ret;
int	num;
{
	int	i, j;

	client_init(client);

	put_cmd(SJ3_WHO);
	put_flush();

	if ((i = get_int()) < 0) {
		sj3_error_number = SJ3_InternalError;
		return -1;
	}

	sj3_error_number = 0;
	for (j = 0 ; j < i ; j++) {
		if (j < num) {
			ret -> fd = get_int();
			get_nstring(ret -> hostname, SJ3_NAME_LENGTH);
			get_nstring(ret -> username, SJ3_NAME_LENGTH);
			get_nstring(ret -> progname, SJ3_NAME_LENGTH);
			ret++;
		}
		else {
			get_int();
			skip_string();
			skip_string();
			skip_string();
		}
	}

	return (i < num) ? i : num;
}

int	sj3_quit(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_QUIT);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}
int	sj3_kill(client)
SJ3_CLIENT_ENV	*client;
{
	client_init(client);

	put_cmd(SJ3_KILL);
	put_flush();

	if (sj3_error_number = get_int()) return ERROR;
	return 0;
}

int	sj3_version(client, dst, dstsiz)
SJ3_CLIENT_ENV	*client;
char	*dst;
int	dstsiz;
{
	int	c;

	client_init(client);

	put_cmd(SJ3_VERSION);
	put_flush();

	sj3_error_number = get_int();
	c = get_byte();
	while (c) {
		while (c) {
			if (dstsiz > 2) { *dst++ = c; dstsiz--; }
			c = get_byte();
		}
		if (dstsiz > 1) { *dst++ = 0; dstsiz--; }
		c = get_byte();
	}
	if (dstsiz > 0) { *dst++ = 0; dstsiz--; }
	return 0;
}

