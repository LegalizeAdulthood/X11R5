#ifndef lint
static char *rcs_id = "$Id: de.c,v 1.2 1991/09/16 21:31:14 ohm Exp $";
#endif /* lint */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
/*
	Jserver		(Nihongo Demon)
*/
#include <stdio.h>
#include <signal.h>
#include "commonhd.h"
#include "config.h"
#include "jd_sock.h"
#include "demcom.h"
#include "wnn_malloc.h"

#include <ctype.h>
#include <errno.h>
extern int errno;		/* Pure BSD */

#ifdef BSD42
#include <sys/file.h>
#else
#ifndef UX386
#include <fcntl.h>
#endif
#endif
#include <sys/ioctl.h>

#ifdef SYSVR2
#include <sys/param.h>
#ifndef SIGCHLD
#define	SIGCHLD	SIGCLD
#endif
#endif

#include "de_header.h"
#ifdef UX386
#include <sys/termio.h>
#undef	AF_UNIX
#endif

#include "msg.h"

#ifdef BSD42
#define NOFILE getdtablesize() 
#endif

#define	QUIET	1

#define	NOT_QUIET	DEBUG | !QUIET

#define	min(x,y)	( (x)<(y) ? (x) : (y) )

/*	Accept Socket	*/
#define	MAX_ACCEPTS	2
#define	UNIX_ACPT	0
#define	INET_ACPT	1

extern	void	do_command();

jmp_buf	client_dead;

#ifdef	AF_UNIX
static struct sockaddr_un saddr_un;	/**	ソケット	**/
#endif	/* AF_UNIX */
static struct sockaddr_in saddr_in;	/**	ソケット	**/

#ifdef	AF_UNIX
static	int	sock_d_un;		/**	ソケットのfd	**/
#endif	/* AF_UNIX */
static	int	sock_d_in;		/**	ソケットのfd	**/

static	int	serverNO = 0;

struct	cmblk	{
		int	sd;	/** ソケットのfd	**/
	};
#define	COMS_BLOCK	struct	cmblk

static	COMS_BLOCK	*cblk;
		/** クライアントごとのソケットを管理するテーブル **/

static	COMS_BLOCK	accept_blk[MAX_ACCEPTS];	/*accept sock blocks */


/*	Communication Buffers		*/
static	char	snd_buf[R_BUF_SIZ];	/** 送信バッファ **/
static	int	sbp;			/** 送信バッファポインタ **/

static	int	rbc;			/** 受信バッファcounter **/
static	char	rcv_buf[S_BUF_SIZ];	/** 受信バッファ **/

/*	Client Table	*/
int	clientp;	/** cblkの有効なデータの最後を差している **/

int	cur_clp;	/** 現在のクライアントの番号 **/

static	int	*all_socks;	/** ビットパターン
				 which jserver to select を保持 **/
static	int	*ready_socks;	/** データのきているソケットの
					ビットパターンを保持 **/
static	int	*dummy1_socks,
		*dummy2_socks;

static	int	no_of_ready_socks;
static	int	sel_bwidth,	/** bit width of all_socks **/
		sel_width;	/** byte width of all_socks **/

static	int	nofile;		/** No. of files **/

#define	BINTSIZE	(sizeof(int)*8)
#define	sock_set(array,pos)	(array[pos/BINTSIZE] |= (1<<(pos%BINTSIZE)))
#define	sock_clr(array,pos)	(array[pos/BINTSIZE] &= ~(1<<(pos%BINTSIZE)))
#define	sock_tst(array,pos)	(array[pos/BINTSIZE] &  (1<<(pos%BINTSIZE)))

struct msg_cat *wnn_msg_cat;
struct msg_cat *js_msg_cat;

extern void terminate_hand();
static void demon_main(), sel_all(), new_client(),
  demon_init(), socket_init(), socket_init_in(), xerror(), get_options();
void demon_fin(), del_client(), put2_cur(), putc_cur(); 
static int get_client(), rcv_1_client(), socket_accept(), socket_accept_in();
int get2_cur();

/* No arguments are used. Only options. */
void
main(argc, argv)
int argc;
char **argv;
{
	extern void signal_hand();
	extern void _exit();
	int tmpttyfd;

	char nlspath[64];

	strcpy(nlspath, LIBDIR);
	strcat(nlspath, "/%L/%N");
	js_msg_cat = msg_open(MESSAGE_FILE, nlspath, LANG_NAME);
	wnn_msg_cat = msg_open("libwnn.msg", nlspath, LANG_NAME);
	if(wnn_msg_cat == NULL){
	    fprintf(stderr, "libwnn: Cannot open message file for libwnn.a\n");
	}

	setuid(geteuid());
/* check whether another jserver already exists. */
	get_options(argc, argv);
#ifdef	CHINESE
	printf("\nChinese Multi Client Server (%s)\n", SER_VERSION);
#else
	printf("\nNihongo Multi Client Server (%s)\n", SER_VERSION);
#endif	/* CHINESE */
#ifndef NOTFORK
	if (fork()) {
	    signal(SIGCHLD , _exit);
	    signal(SIGHUP , SIG_IGN);
	    signal(SIGINT , SIG_IGN);
	    signal(SIGQUIT, SIG_IGN);
#ifdef	SIGTSTP
	    signal(SIGTSTP, SIG_IGN);
#endif
	    signal(SIGTERM, _exit);
	    pause();
	}
#endif /* !NOTFORK */

	signal(SIGHUP , signal_hand);
	signal(SIGINT , signal_hand);
	signal(SIGQUIT, signal_hand);
	signal(SIGTERM, terminate_hand);
#ifndef NOTFORK
#ifdef	SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif	/* SIGTSTP */
#endif /* !NOTFORK */
	read_default();
	demon_init();

	env_init();
	file_init();
	dic_init();
	get_kaiseki_area(LENGTHCONV + 1);	/* 変換可能文字数 */
        init_work_areas();
	init_jmt(0);

	read_default_files();

#ifndef NOTFORK
	/* End of initialization, kill parent */
	kill(getppid(), SIGTERM);
	fclose(stdin);
	fclose(stdout);
	if(!noisy){
	    fclose(stderr);
	}

# if !defined(TIOCNOTTY) && (defined(__hp9000s300) || defined(SVR4))
#  define TIOCNOTTY	_IO('t', 113)
# endif /* hpux */
	if ((tmpttyfd = open("/dev/tty", O_RDWR)) >= 0) {
		ioctl(tmpttyfd, TIOCNOTTY, 0);
		close(tmpttyfd);
	}
#endif /* !NOTFORK */

	demon_main();

 demon_fin();
}

static void
demon_main()
{
 for(;;){
   sel_all();
   new_client();
   for(;;){
#ifdef DEBUG
	error1("main loop: ready_socks = %02X\n", ready_socks);
#endif
	if(get_client() == -1) break;
	c_c = &client[cur_clp];
/*	error1("cur_clp = %d\n", cur_clp);  Moved into do_command() */
	rbc = 0; sbp = 0;
/*	if(rcv_1_client(cur_clp) == 0){ del_client(); continue; }	*/
	if(setjmp(client_dead)){
		 del_client();
		 continue;
	}
	do_command(c_c);
   }
 }
}

/*
	allocs area for selecting socketts
*/
void
socket_disc_init()
{
 int sel_w; /* long word(==int) width of all_socks */

	nofile = NOFILE;
	sel_w = (nofile-1)/BINTSIZE + 1;
	all_socks   = (int *)calloc( sel_w,(sizeof(int)));
	ready_socks = (int *)malloc( sel_w*(sizeof(int)));
	dummy1_socks= (int *)malloc( sel_w*(sizeof(int)));
	dummy2_socks= (int *)malloc( sel_w*(sizeof(int)));
	sel_width = sel_w*sizeof(int);	/* byte width */
	sel_bwidth = sel_width*8;	/* bit width */
}

/**	全てのソケットについて待つ	**/
static void
sel_all()
{
	bcopy(all_socks,ready_socks,sel_width);
	bzero(dummy1_socks,sel_width);
	bzero(dummy2_socks,sel_width);

    top:
	if((no_of_ready_socks
		=select(nofile,ready_socks,dummy1_socks,dummy2_socks,0))==-1){
	        if(errno == EINTR) goto top;
		xerror("select error");
	}
#ifdef DEBUG
	error1("select OK, ready_socks[0]=%02X, n-r-s=%x\n",
			ready_socks[0], no_of_ready_socks);
#endif
}

/**	ready_socksから今やる一人を取り出して返す(cur_clpにもセットする)
	誰も居ない時は-1
	スケジューリングはラウンドロビン	**/
static int
get_client()
{
	register int i;

	if(no_of_ready_socks==0) return -1;	/* no client waits service*/

	for (i = cur_clp; ; ) {
		if(no_of_ready_socks == 0) return -1;
		i++;
		if (i >= clientp) i = 0;
		if(sock_tst(ready_socks,cblk[i].sd)){
			sock_clr(ready_socks,cblk[i].sd);
			no_of_ready_socks--;
			return cur_clp = i;
		}
	}
}

/**	新しいクライアントが居るか否かを調べる
	居た場合はcblkに登録する	**/
static void
new_client()	/* NewClient */
{
    register int sd;
    register int full, i;
    FILE *f[3];
    char gomi[1024];
#ifdef	AF_UNIX
    if((serverNO == 0) && (sock_tst(ready_socks,accept_blk[UNIX_ACPT].sd))){
	sock_clr(ready_socks,accept_blk[UNIX_ACPT].sd);
	no_of_ready_socks--;
	sd = socket_accept();
    }else
#endif
    if(sock_tst(ready_socks,accept_blk[INET_ACPT].sd)){
	sock_clr(ready_socks,accept_blk[INET_ACPT].sd);
	no_of_ready_socks--;
	sd = socket_accept_in();
    }else{
	return;
    }
    error1("new Client: sd = %d\n", sd);
    /* reserve 2 fd */
    for(full=i=0;i<2;i++){
	if(NULL==(f[i]= fopen("/dev/null","r"))){
	    full=1;
	}
    }
    for(i=0;i<2;i++){
	if(NULL != f[i]) fclose(f[i]);
    }

    if(full || sd >= nofile || clientp >= max_client) {
	fprintf(stderr,"Jserver: no more client\n");
	read(sd,gomi,1024);
	shutdown(sd, 2);
	close(sd);
	return;
    }

    cblk[clientp].sd = sd;
    sock_set(all_socks,sd);
    clientp++;
}

/**	クライアントをcblkから削除する	**/
/* delete Client (please call from JS_CLOSE) */
void
del_client()
{
	disconnect_all_env_of_client();
	sock_clr(all_socks,cblk[cur_clp].sd);
	close(cblk[cur_clp].sd);
	cblk[cur_clp] = cblk[clientp - 1];
	client[cur_clp] = client[clientp - 1];
	clientp--;
	error1(stderr,"Delete Client: cur_clp = %d\n", cur_clp);
}


/**	サーバをイニシャライズする	**/
static void
demon_init()	/* initialize Demon */
{
	signal(SIGHUP,SIG_IGN);
	signal(SIGINT,SIG_IGN);
	signal(SIGQUIT,SIG_IGN);


	if((cblk =(COMS_BLOCK *)malloc(max_client * sizeof(COMS_BLOCK)))
	   == NULL){
	    perror("Malloc for cblk");
	    exit(1);
	}
	if((client =(CLIENT *)malloc(max_client * sizeof(CLIENT)))
	   == NULL){
	    perror("Malloc for client");
	    exit(1);
	}
#ifdef SRAND48
	srand48(time((long *)0));
#else
	srand((int)time((long *)0));
#endif
	clientp = 0;	/* V3.0 */
	cur_clp = 0;	/* V3.0 */
	socket_disc_init();
	socket_init_in();
#ifdef	AF_UNIX
	socket_init();
#endif	/* AF_UNIX */
}

/**	サーバを終わる	**/
void
demon_fin()
{
    int trueFlag = 1;
    int fd;
#ifdef	AF_UNIX
    struct sockaddr_un addr_un;
#endif	/* AF_UNIX */
    struct sockaddr_in addr_in;
    int addrlen;

    /*
      accept all pending connection from new clients,
      avoiding kernel hangup.
     */

#ifdef	AF_UNIX
    if (serverNO == 0){
#if defined(FIONBIO)
	ioctl(sock_d_un, FIONBIO, &trueFlag);
#endif
	for (;;) {
	    addrlen = sizeof(addr_un);
	    if (accept(sock_d_un, &addr_un, &addrlen) < 0) break;
	    /* EWOULDBLOCK EXPECTED, but we don't check */
	}
	shutdown(sock_d_un, 2);
	close(sock_d_un);
    }
#endif	/* AF_UNIX */

#if defined(FIONBIO)
    ioctl(sock_d_in, FIONBIO, &trueFlag);
#endif
    for (;;) {
	addrlen = sizeof(addr_in);
	if (accept(sock_d_in, &addr_in, &addrlen) < 0) break;
	/* EWOULDBLOCK EXPECTED, but we don't check */
    }
    shutdown(sock_d_in, 2);
    close(sock_d_in);

    for (fd = nofile; fd >= 0; fd--) {
	shutdown(fd, 2);
	close(fd);
    }
}

/*------*/

/**	**/
void
gets_cur(cp)
register char *cp;
{
    while((*cp++ = getc_cur()) != '\0');
}

/**	**/
void
getws_cur(cp)
register w_char *cp;
{
    while((*cp++ = get2_cur()) != 0);
}

/**	カレント・クライアントから2バイト取る	**/
int
get2_cur()
{
    register int	x;
    x = getc_cur();
    return (x << 8) | getc_cur();
}

/**	カレント・クライアントから4バイト取る	**/
int
get4_cur()
{
    register int	x1, x2, x3;
    x1 = getc_cur();
    x2 = getc_cur();
    x3 = getc_cur();
    return (x1 << (8 * 3)) | (x2 << (8 * 2)) | (x3 << (8 * 1)) | getc_cur();
}

/**	カレント・クライアントから1バイト取る	**/
int
getc_cur()
{
    static int rbp;
    if(rbc<=0){
	rbc=rcv_1_client(cur_clp);
	rbp = 0;
    }
    rbc--;
    return rcv_buf[rbp++] & 0xFF;
}

/**	クライアントから1パケット取る	**/
static int
rcv_1_client(clp)
int clp;	/**	clp=クライアント番号	**/
{
    register int cc;
    cc = read(cblk[clp].sd, rcv_buf, S_BUF_SIZ);
#ifdef DEBUG
    error1("rcv: clp = %d, sd = %d, cc = %d\n", clp, cblk[clp].sd, cc);
    dmp(rcv_buf,cc);
#endif
    if(cc<=0){ /* client dead */ longjmp(client_dead,666); }
    return cc;
}

/**	クライアントへ1パケット送る	**/
static void
snd_1_client(clp,n)
int clp, n;	/**	clp=クライアント番号, n= send n Bytes	**/
{
    register int cc,x;
#ifdef	DEBUG
    error1("snd: clp = %d, sd = %d\n", clp, cblk[clp].sd);
    dmp(snd_buf,n);
#endif
    for(cc=0;cc<n;){
	x = write(cblk[clp].sd, &snd_buf[cc], n-cc);
	if(x<=0){ /* client dead */ longjmp(client_dead,666); }
	cc+=x;
    }
}

/**	**/
void
puts_cur(p)
char *p;
{
    register int c;
    while(c= *p++) putc_cur(c);
    putc_cur(0);
}

/**	**/
void
putws_cur(p)
w_char *p;
{
    register int c;
    while(c= *p++) put2_cur(c);
    put2_cur(0);
}

/**	**/
void
putnws_cur(p, n)
register w_char *p;
register int n;
{
    register int c;
    for(;n > 0 ; n--){
	if((c= *p++) == 0)break;
	put2_cur(c);
    }
    put2_cur(0);
}

/**	カレント・クライアントへ2バイト送る	**/
void
put2_cur(c)
register int c;
{
	putc_cur( c >> (8 * 1));
	putc_cur(c);
}

/**	カレント・クライアントへ4バイト送る	**/
void
put4_cur(c)
register int c;
{
	putc_cur(c >> (8 * 3));
	putc_cur(c >> (8 * 2));
	putc_cur(c >> (8 * 1));
	putc_cur(c);
}

/**	カレント・クライアントへ1バイト送る	**/
void
putc_cur(c)
register int c;
{
	snd_buf[sbp++] = c;
	if (sbp >= R_BUF_SIZ) {
		snd_1_client(cur_clp, R_BUF_SIZ);
		sbp = 0;
	}
}

/**	カレント・クライアントの送信バッファをフラッシュする	**/
void
putc_purge()
{
	if (sbp != 0) {
		snd_1_client(cur_clp,sbp);
		sbp = 0;
	}
}

/*-----*/

/**	ソケットのイニシャライズ	**/
#ifdef	AF_UNIX
static void
socket_init()
{
    if (serverNO == 0){
	saddr_un.sun_family = AF_UNIX;
	unlink(sockname);
	strcpy(saddr_un.sun_path, sockname);
	if ((sock_d_un = socket(AF_UNIX, SOCK_STREAM, 0)) == ERROR) {
		xerror("Can't create socket.\n");
	}
	if (bind(sock_d_un, &saddr_un, strlen(saddr_un.sun_path) + 2)
	    == ERROR) {
		shutdown(sock_d_un, 2);
		xerror("Can't bind socket.\n");
	}
	if (listen(sock_d_un, 5) == ERROR) {
		shutdown(sock_d_un, 2);
		xerror("Can't listen socket.\n");
	}
	signal(SIGPIPE, SIG_IGN);
#ifdef DEBUG
	error1("sock_d_un = %d\n", sock_d_un);
#endif
	accept_blk[UNIX_ACPT].sd = sock_d_un;
	sock_set(all_socks,sock_d_un);
    }
}
#endif	/* AF_UNIX */

/*	Inet V3.0 */
static void
socket_init_in()
{
    int on = 1;
    int port; struct servent *sp;
#if !defined(SO_DONTLINGER) && defined(SO_LINGER)
    struct linger linger;
#endif

    if ((sp = getservbyname(SERVERNAME,"tcp")) == NULL) {
 	port =  htons(WNN_PORT_IN + serverNO);
    } else {
#ifdef UX386
	int	hs;
	hs = ntohs(sp->s_port) + serverNO;
	port = htons(hs);
#else
	port = htons(ntohs(sp->s_port) + serverNO); 
#endif
    }
#if DEBUG
    error1("port=%x\n",port);
#endif
    saddr_in.sin_family = AF_INET;
    saddr_in.sin_port = port;
    saddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    if ((sock_d_in = socket(AF_INET,SOCK_STREAM, 0)) == ERROR) {
	xerror("can't create inet-socket\n");
    }
    setsockopt(sock_d_in, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
#ifdef SO_DONTLINGER
    setsockopt(sock_d_in, SOL_SOCKET, SO_DONTLINGER, (char *)0, 0);
#else
# ifdef SO_LINGER
    linger.l_onoff = 0;
    linger.l_linger = 0;
    setsockopt(sock_d_in, SOL_SOCKET, SO_LINGER, (char *)&linger, sizeof linger);
# endif	/* SO_LINGER */
#endif	/* SO_DONTLINGER */

    if (bind(sock_d_in,&saddr_in,sizeof(saddr_in)) == ERROR) {
	shutdown(sock_d_in, 2);
	xerror("can't bind inet-socket\n");
    }
    if (listen(sock_d_in,5) == ERROR) {
	shutdown(sock_d_in, 2);
	xerror("can't listen inet-socket\n");
    }
#if DEBUG
    error1("sock_d_in = %d\n", sock_d_in);
#endif
    accept_blk[INET_ACPT].sd = sock_d_in;
    sock_set(all_socks,sock_d_in);
}


/**	accept new client socket	**/
#ifdef	AF_UNIX
static int
socket_accept()
{
    struct sockaddr_un addr;
    int addrlen;

    addrlen = sizeof(addr);
    return accept(sock_d_un, &addr, &addrlen);
}
#endif	/* AF_UNIX */

static int
socket_accept_in()
{
    struct sockaddr_in addr;
    int addrlen;

    addrlen = sizeof(addr);
    return accept(sock_d_in, &addr, &addrlen);
}

static void
xerror(s)
register char *s;
{
    fprintf(stderr,"Jserver: %s\n", s);
    exit(1);
}

#if DEBUG
static void
dmp(p,n)
char *p;
{
	int	i, j;

	for (i = 0; i < n; i += 16) {
		for (j = 0; j < 16; j++) {
			fprintf(stderr,"%02x ", p[i + j] & 0xFF);
		}
		fprintf(stderr,"n=%d\n",n);
	}
}

int
niide(x)		/* おまけだよ。niide(3);とコールしてね */
int x;
{
	switch (x) {
	case 0:
		return 1;
	case 1:
		fprintf(stderr,"ゲッターロボは");
		break;
	case 2:
		fprintf(stderr,"ゴレンジャーは");
		break;
	case 3:
		fprintf(stderr,"ロードランナーシリーズは");
		break;
	default:
		return x * niide(x - 1);
	}
	fprintf(stderr,"良い番組だ by 新出。\n");
	return x * niide(x - 1);
}
#endif

static void
get_options(argc, argv)
int argc;
char **argv;
{
    int c;
    extern char *optarg;

    strcpy(jserverrcfile, LIBDIR);	/* usr/local/lib/wnn */
    strcat(jserverrcfile, SERVER_INIT_FILE);	/* ja_JP/jserverrc */
    while ((c = getopt(argc, argv, "f:s:h:N:")) != EOF) {
	switch(c){
	case 'f':
	    strcpy(jserverrcfile, optarg);
	    break;
	case 's':
	    noisy = 1;
	    if(strcmp("-", optarg) != 0){
		if(freopen(optarg, "w", stderr) == NULL){
		    printf("Error in opening scriptfile %s.\n" , optarg);
		    exit(1);
		}
	    }
	    error1("script started\n");
	    break;
	case 'h':
	    hinsi_file_name = optarg;
	    break;
	case 'N':
	    serverNO = atoi(optarg);
	    break;
	default:
	    printf(
"usage: jserver [-F <fuzokugo file> -f <initialize-file> -s <script-file(\"-\" for stderr)> -h <hinsi_file>]\n");
	  exit(1);
	}
    }
}



/*
*/
void
js_who()
{
    register int i,j;

    put4_cur(clientp); 
    for(i=0;i<clientp;i++){
	put4_cur(cblk[i].sd);
	puts_cur( client[i].user_name );
	puts_cur( client[i].host_name );
	for(j=0;j<WNN_MAX_ENV_OF_A_CLIENT;j++){
	    put4_cur((client[i].env)[j]);
	}

    }
    putc_purge();
}

void
js_kill()
{
    if(clientp == 1){
	put4_cur(0);
	putc_purge();
	terminate_hand();
    }else{
	put4_cur(clientp - 1);
	putc_purge();
    }
}
