#ifndef lint
static char *rcs_id = "$Id: jhlp.c,v 1.2 1991/09/16 21:33:38 ohm Exp $";
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
#include <stdio.h>
#include <signal.h>
#include "jllib.h"
#include "commonhd.h"
#include "sdefine.h"
#include "sheader.h"
#include "config.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <pwd.h>
#include <sys/time.h>
#ifdef UX386
#include <sys/kdef.h>
#endif

struct passwd *getpwuid();

#ifdef SYSVR2
#	include <setjmp.h>
/*	don't define bcopy!!!!		*/
#endif /* SYSVR2 */
/* jlib.hの中でsetjmp.hがincludeされている!! */

jmp_buf kk_env;

#ifdef SYSVR2
#	include <sys/param.h>
#endif /* SYSVR2 */

#ifdef HAVE_WAIT3
#	include <sys/wait.h>
#endif /* HAVE_WAIT3 */

#ifdef BSD42
#       include <sgtty.h>
#endif /* BSD42 */

#ifdef SYSVR2
#       include <curses.h>
#       include <term.h>
#endif /* SYSVR2 */
#ifdef SVR4
#include <sys/termio.h>
#endif

#if defined(uniosu)
#	include <sys/pty.h>
#endif	/* defined(uniosu) */

#define ERROR -1

#ifdef TIOCSSIZE
struct ttysize pty_rowcol;
#endif /* TIOCSSIZE */

int ttyfd;

char *tname;			/* terminal name */
char *cmdnm = "csh";		/* char *cmdnm = "csh"; */

int child_id;

extern int errno;
extern char *optarg;
extern int optind;

extern char *ttyname(), *malloc(), *getenv();

static void save_signals();
static void restore_signals();

extern	int
  through(),        eujis_to_iujis(), jis_to_iujis(), sjis_to_iujis(),
  iujis_to_eujis(), through(),        jis_to_eujis(), sjis_to_eujis(),
  iujis_to_jis8(),  eujis_to_jis8(),  through(),      sjis_to_jis8(),
  iujis_to_sjis(),  eujis_to_sjis(),  jis_to_sjis(),  through();

/* bit 2-3 are source, bit 0-1 are destination */
int (*code_trans[])() = {
    through,        iujis_to_eujis, iujis_to_jis8, iujis_to_sjis,
    eujis_to_iujis, through,        eujis_to_jis8, eujis_to_sjis,
    jis_to_iujis,   jis_to_eujis,   through,      jis_to_sjis,
    sjis_to_iujis,  sjis_to_eujis,  sjis_to_jis8,  through
};

static void do_end(), open_pty(), open_ttyp(), do_main(), exec_cmd(),
  parse_options(), setsize(), terminate_handler(), get_rubout(), usage(),
  change_size();

/** メイン */
void
main(argc, argv)
int argc;
char **argv;
{

    char *name;
    char *p;
    char nlspath[64];

    flow_control = FLOW_CONTROL;
    tty_c_flag   = TTY_KCODE;
    pty_c_flag   = PTY_KCODE;

    strcpy(username, getpwuid(getuid())->pw_name);
    if((name = getenv(WNN_USERNAME_ENV)) != NULL){
	strcpy(username, name);
    }
    if((name = getenv(WNN_JSERVER_ENV)) == NULL) {
	name = "";
    }
    strcpy(def_servername, name);
    strcpy(def_reverse_servername, name);
    parse_options(argc, argv);

    strcpy(nlspath, LIBDIR);
    strcat(nlspath, "/%L/%N");
    cd = msg_open("uum.msg", nlspath, lang_dir);

    if (!isatty(0)) {
	fprintf(stderr,"Input must be a tty.\n");
	exit(1);
    }

    if((p = getenv(WNN_COUNTDOWN_ENV)) == NULL) {
      setenv(WNN_COUNTDOWN_ENV,"0");
    } else if (atoi(p) <= 0) {
#ifndef	OMRON
	puteustring("BOOOOOOOOOOOM!!!\n",stdout);
#else	/* OMRON */
	puteustring(MSG_GET(4),stdout);
	/*
	puteustring("ｕｕｍからｕｕｍは起こせません。\n",stdout);
	*/
#endif	/* OMRON */
	exit(126);
    } else {
      sprintf(p,"%d",atoi(p) - 1);
      setenv(WNN_COUNTDOWN_ENV,p);
    }

    if((tname = getenv("TERM")) == NULL) {
	fprintf(stderr,"Sorry. Please set your terminal type.\r\n");
	exit(1);
    }


#ifdef BSD42
    if (saveutmp() < 0) {
	puts("Can't save utmp\n");
    }
#endif /* BSD42 */


    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }
    if (argc > 1) {
	cmdnm = *++argv;
    } else {
	if ((name = getenv("SHELL")) != NULL) {
	    cmdnm = name;
	}
	argv[0] = cmdnm;
	argv[1] = NULL;
    }

    save_signals();

#ifdef BSD42
    if (getTermData() == -1) {
	fprintf(stderr,"Sorry. Something is wrong with termcap, maybe.\r\n");
	exit(21);
    }
#endif /* BSD42 */
#ifdef SYSVR2
    if (openTermData() == -1) {
	fprintf(stderr,"Sorry. Something is wrong with terminfo, maybe.\r\n");
	exit(21);
    }
    maxlength = columns;
    crow = lines - conv_lines;
#endif /* SYSVR2 */
#ifdef BSD43
    setsize();
#endif /* BSD43 */

#ifdef BSD42
    set_TERMCAP();
#endif /* BSD42 */

    open_pty();
    open_ttyp();
    exec_cmd(argv);

    /*
      close(0);
      close(1);
      close(2);
      if((ttyfd = open("/dev/tty", O_RDWR | O_NDELAY, 0)) != 0) {
      err("Can't open /dev/tty.");
      }
      dup(0);
      dup(0);
     */

    ttyfd = 0;
    get_rubout();

    switch (init_uum()) {	/* initialize of kana-kanji henkan */
    case -1:
	terminate_handler();
	break;
    case -2:
	epilogue();
	do_end();
	break;
    }

    fcntl(ttyfd, F_SETFL, O_NDELAY);

    if (j_term_init() == ERROR) {
	err("term initialize fault.");
    }

    if (!jl_isconnect(bun_data_)) {
      if (*servername == 0){
	  printf("%s\r\n", wnn_perror());
      } else {
	  printf("jserver(at %s):%s\r\n", servername, wnn_perror());
      }
      flush();
    }

    puteustring(MSG_GET(1),
	/*
	"\rｕｕｍ(かな漢字変換フロントエンドプロセッサ)\r\n",
	*/
	stdout);
    initial_message_out();  /* print message if exists. */

#if defined(uniosu)
    if (setjmp(kk_env)) {
	disconnect_jserver();
	ioctl_off();
	connect_jserver(0);
    }
#endif	/* defined(uniosu) */

    do_main();
}

/*
  each options handling functions
 */

static int do_h_opt()
{
    henkan_off_flag = 1;
    defined_by_option |= OPT_WAKING_UP_MODE;
    return 0;
}

static int do_H_opt()
{
    henkan_off_flag = 0;
    defined_by_option |= OPT_WAKING_UP_MODE;
    return 0;
}

static int do_u_opt()
{
    pty_c_flag = J_EUJIS;
    return 0;
}

static int do_j_opt()
{
    pty_c_flag  = J_JIS;
    return 0;
}

static int do_s_opt()
{
    pty_c_flag = J_SJIS;
    return 0;
}

static int do_U_opt()
{
    tty_c_flag = J_EUJIS;
    return 0;
}

static int do_J_opt()
{
    tty_c_flag = J_JIS;
    return 0;
}

static int do_S_opt()
{
    tty_c_flag = J_SJIS;
    return 0;
}

static int do_P_opt()
{
    sleep(20);
    return 0;
}

static int do_x_opt()
{
    flow_control = 0;
    defined_by_option |= OPT_FLOW_CTRL;
    return 0;
}

static int do_X_opt()
{
    flow_control = 1;
    defined_by_option |= OPT_FLOW_CTRL;
    return 0;
}

static int do_k_opt()
{
    strcpy(uumkey_name_in_uumrc, optarg);
    if (*uumkey_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_WNNKEY;
    return 0;
}

static int do_c_opt()
{
    strcpy(convkey_name_in_uumrc, optarg);
    if (*convkey_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_CONVKEY;
    return 0;
}

static int do_r_opt()
{
    strcpy(rkfile_name_in_uumrc, optarg);
    if (*rkfile_name_in_uumrc == '\0') {
	return -1;
    }
    defined_by_option |= OPT_RKFILE;
    return 0;
}

static int do_l_opt()
{
    conv_lines = atoi(optarg);
    return 0;
}

static int do_D_opt()
{
    strcpy(def_servername, optarg);
    strcpy(def_reverse_servername, optarg);
    if (*def_servername == '\0') {
	return -1;
    }
    return 0;
}

static int do_n_opt()
{
    strcpy(username, optarg);
    if (*username == '\0') {
	return -1;
    }
    return 0;
}

static int do_v_opt()
{
    defined_by_option |= OPT_VERBOSE;
    return 0;
}

static int do_L_opt()
{
    strcpy(lang_dir, optarg);
    if (*lang_dir == '\0') {
	return -1;
    }
    defined_by_option |= OPT_LANGDIR;
    return 0;
}

static int (*do_opt[])() = {
    do_h_opt,			/* 'h' : waking_up_in_henkan_mode */
    do_H_opt,			/* 'H' : waking_up_no_henkan_mode */
    do_u_opt,			/* 'u' : set pty's kanji code to UJIS */
    do_j_opt,			/* 'j' : set pty's kanji code to JIS */
    do_s_opt,			/* 's' : set pty's kanji code to SJIS */
    do_U_opt,			/* 'U' : set tty's kanji code to UJIS */
    do_J_opt,			/* 'J' : set tty's kanji code to JIS */
    do_S_opt,			/* 'S' : set tty's kanji code to SJIS */
    do_P_opt,			/* 'P' : sleep 20 seconds (for debug) */
    do_x_opt,			/* 'x' : disable tty's flow control */
    do_X_opt,			/* 'X' : enable tty's flow control */
    do_k_opt,			/* 'k' : specify uumkey file */
    do_c_opt,			/* 'c' : specify convert_key file */
    do_r_opt,			/* 'r' : specify romkan mode file */
    do_l_opt,			/* 'l' : specify # of lines used for henkan */
    do_D_opt,			/* 'D' : specify hostname of jserver */
    do_n_opt,			/* 'n' : specify username for jserver */
    do_v_opt,			/* 'v' : verbose */
    do_L_opt			/* 'L' : specify language */
  };

static void
parse_options(argc, argv)
int argc;
char **argv;
{
  register int c;
  register char *ostr = OPTIONS;
  register char *p;
#ifndef	SYSVR2
  extern char *index();
#else
  extern char *strchr();
#endif
  
  while ((c = getopt(argc, argv, GETOPTSTR)) != EOF) {
#ifndef	SYSVR2
    if ((p = index(ostr, c)) == NULL || (*do_opt[p - ostr])() < 0) {
#else
    if ((p = strchr(ostr, c)) == NULL || (*do_opt[p - ostr])() < 0) {
#endif
      usage(ostr);
    }
  }
}

/** tty に対する ioctl のセット */

#define UNDEF_STTY -1

#ifdef BSD42
struct sgttyb savetmio;
/*
  struct sgttyb ttyb =
  { B9600, B9600, 0x7f, 0x15, EVENP | ODDP | ECHO | CRMOD };
  int local_mode = LCRTBS | LCRTERA | LCRTKIL | LCTLECH | LPENDIN | LDECCTQ;
*/

/* added later */
struct tchars tcharsv;
struct ltchars ltcharsv;
struct sgttyb ttyb;
int local_mode;

static void
get_rubout()
{
    if(savetmio.sg_erase == UNDEF_STTY){
      rubout_code = RUBOUT;
    }else{
      rubout_code = savetmio.sg_erase;
    }
}    

int
j_term_init()
{
    struct sgttyb buf;

    buf = savetmio;
    buf.sg_flags |= RAW;
    buf.sg_flags &= ~ECHO;
    ioctl(ttyfd, TIOCSETP, &buf);

    return 0;
}
#endif /* BSD42 */

#ifdef SYSVR2
struct termio savetmio;
struct termio terms = {
    IGNBRK | ICRNL | IXON,	/* c_iflag */
    ONLCR | OPOST,		/* c_oflag */
    B9600 | CS8 | CSTOPB | CREAD | CLOCAL, /* c_cflag */
    ISIG | ICANON | ECHO | ECHOE | ECHOK, /* c_lflag */
    0,				/* c_line */
{ 0x3, 0x1c, 0x8, 0x15, 0x4, 0, 0, 0 } /* c_cc */
};

#if defined(uniosu)
struct jtermio savejtmio;
struct jtermio savejtmiottyp;
struct auxtermio saveauxterm ;
struct auxtermio auxterm = {
  0,				/* -tostop */
{ 0x1a, 0, 0, 0, 0, 0, 0, 0 }	/* c_cc2 */
};
#endif	/* defined(uniosu) */

static void
get_rubout()
{
    if(savetmio.c_cc[VERASE] == UNDEF_STTY){
      rubout_code = RUBOUT;
    }else{
      rubout_code = savetmio.c_cc[VERASE];
    }
}

int
j_term_init()
{
    struct termio buf1;
#if defined(uniosu)
    struct jtermio buf2;
#endif	/* defined(uniosu) */

    buf1 = savetmio;
    buf1.c_iflag &= ~(ISTRIP | INLCR | IGNCR | ICRNL | IUCLC | IXON); 
    if (flow_control) {
	buf1.c_iflag |= IXON;
    }
    buf1.c_lflag &= ~(ECHONL | ECHOK | ECHOE | ECHO | XCASE | ICANON | ISIG);
    buf1.c_oflag = OPOST; 
    buf1.c_cc[VINTR] = CDEL;
    buf1.c_cc[VQUIT] = CDEL;
    buf1.c_cc[VERASE] =CDEL;
    buf1.c_cc[VKILL] = CDEL;
    buf1.c_cc[VEOF] = 1;	/* cf. ICANON */
    buf1.c_cc[VEOL] = 0;
    if(ioctl(ttyfd, TCSETA, &buf1) == -1){
	fprintf(stderr, "error in ioctl TCSETA.\n");
	exit(1);
    }

#if defined(uniosu)
    if(ioctl(ttyfd, JTERMGET, &buf2) < 0) {
	fprintf(stderr, "error in ioctl JTERMGET in j_term_init.\n");
	exit(1);
    }
    savejtmio = buf2;

    buf2.j_flg = CONVTOEXT | WNN_EXIST;
    buf2.j_level = jterm;
    switch(jcode_set) {
    case 0:
	buf2.j_ecode = JIS;
	break;
    case 1:
	buf2.j_ecode = SJIS;
	break;
    case 2:
	buf2.j_ecode = UJIS;
	break;
    default:
	fprintf(stderr, "uum: kanji code set not supported in terminfo\n");
	exit(1);
    }
    if(jis_kanji_in) {
	strcpy(buf2.j_jst, jis_kanji_in);
	buf2.j_jstl = strlen(jis_kanji_in);
    }
    if(jis_kanji_out){
	strcpy(buf2.j_jend, jis_kanji_out);
	buf2.j_jendl = strlen(jis_kanji_out);
    }
    if(jgaiji_start_address) { 
	*(short *)buf2.j_gcsa = jgaiji_start_address;
    }
    if(jgaiji_disp) {
	strcpy(buf2.j_gdsp, jgaiji_disp);
	buf2.j_gdspl = strlen(jgaiji_disp);
    }

    if(ioctl(ttyfd, JTERMSET, &buf2) < 0) {
	fprintf(stderr, "error in ioctl JTERMSET.\n");
	exit(1);
    }
#endif	/* defined(uniosu) */

    return 0;
}
#endif /* SYSVR2 */

/** signal SIGCHLD を受けた後の処理をする。*/
void
chld_handler()
{
    extern void chld_handler();
#ifdef HAVE_WAIT3
    union wait status;
    int pid;

    if ((pid = wait3(&status, WNOHANG | WUNTRACED, NULL)) == child_id) {
	if (WIFSTOPPED(status)) {
#ifdef SIGCONT
	    kill(pid, SIGCONT);
#ifndef SYSVR2
	    pid = getpgrp(pid);
	    killpg(pid, SIGCONT);
#else
#if defined(uniosu)
	    pid = ngetpgrp(pid);
	    kill(pid, SIGCONT);
#endif
#endif
#endif
	} else {
	    signal(SIGCHLD, SIG_IGN);
	    printf(MSG_GET(3));
	    /*
	    printf("\r\nｕｕｍを終わります。\r\n");
	    */
	    epilogue();
	    do_end();
	}
    }
#else /* ! HAVE_WAIT3 */
    if(wait(0) == child_id) {
	signal(SIGCHLD, SIG_IGN);
	printf(MSG_GET(3));
	/*
	printf("\r\nｕｕｍを終わります。\r\n");
	*/
	epilogue();
	do_end();
    }
#endif /* HAVE_WAIT3 */

    re_signal(SIGCHLD, chld_handler);
}

/** signal SIGTERM を受けた時の処理をする。*/
static 
void
terminate_handler()
{
    signal(SIGCHLD, SIG_IGN);
    epilogue_no_close();
    do_end();
}

#ifdef	SIGWINCH
void
resize_handler()
{
    change_size();
}
#endif	/* SIGWINCH */

/** メインループ */

int sel_ptn;
int pty_bit;
int tty_bit;
int ptyfd;

static void
do_main()
{
    unsigned char *buf;
    int ml;

    if ((buf = (unsigned char *)malloc(maxchg * 4)) == NULL) {
	printf(MSG_GET(2));
	printf(MSG_GET(3));
	/*
	printf("malloc に失敗しました。ｕｕｍを終わります。\r\n");
	*/
	epilogue();
	do_end();
    }

    pty_bit = 1 << ptyfd;
    tty_bit = 1 << ttyfd;
    sel_ptn = pty_bit | tty_bit;

    if (henkan_off_flag == 0) {
	disp_mode();
    }

    for (;;) {

      ml = kk();
      
/*
      if (!((ml < 2) && ((return_buf[0] <= ' ') || (return_buf[0] == 0x7f)))) {
*/
	make_history(return_buf, ml);
/*
      }
*/
      ml = (*code_trans[(J_IUJIS << 2) | pty_c_flag])
	(buf, return_buf, sizeof (w_char) * ml);
      if (ml > 0)
	write(ptyfd, buf, ml);
    }
  }

/** convert_key nomi okonau key-in function */
int conv_keyin()
{
    int keyin0();

    return keyin1(keyin0);
}

/** キー入力関数 1 */
int keyin()
{
    return(conv_keyin());
}

/*  an old one.
int keyin()
{
    unsigned char c;

    if(henkan_off_flag){
	henkan_off();
	henkan_off_flag = 0;
    }
    for(; (c = conv_keyin()) == kk_on; ) {
	if (quote_flag) {
	    quote_flag = 0;
	    break;
	}
	henkan_off();
    }
    return c;
}
*/

#if !defined(uniosu)
/** キー入力関数 2 */
int keyin0()
{
    static unsigned char buf[BUFSIZ];
    static unsigned char outbuf[BUFSIZ];
    static unsigned char *bufend = buf;
    static unsigned char *bufstart = buf;
    int n;
    int rfds;
    int i, j, mask;
    unsigned char *p;
    extern int henkan_off_flag;
#ifdef BSD42
    struct timeval time_out;
#else
    int	time_out;
#endif
    int	sel_ret;

    if (bufstart < bufend) {
        return *bufstart++;
    }
    for (;;) {
        if((n = read(ttyfd, buf, BUFSIZ)) > 0) {
	    if (henkan_off_flag == 1 && tty_c_flag == pty_c_flag)
		i = through(outbuf, buf, n);
	    else
		i = (*code_trans[(tty_c_flag << 2) | J_EUJIS])(outbuf, buf, n);
	    if (i <= 0) continue;
	    bufstart = outbuf;
	    bufend = outbuf + i;
            return *bufstart++;
        }

#ifdef BSD42
	time_out.tv_sec = 2;
	time_out.tv_usec = 0;
#else	
	time_out = 2;	/* 2 sec 間待つのだゾ! */
#endif
	for(rfds = sel_ptn;
	    (sel_ret = select(20, &rfds, 0, 0, &time_out)) < 0 && errno == EINTR;
	    rfds = sel_ptn)
	  ;
        if (sel_ret == 0) {
	    if ((tty_c_flag == J_JIS) && ((i = jis_flush(outbuf)) > 0)) {
		/* 溜まっているＥＳＣを吐き出す */
		bufstart = outbuf;
		bufend = outbuf + i;
		return *bufstart++;
	    }
	    /*
	    return (-1);
	    */
	    continue;
	}

        if (rfds & pty_bit) {
            if ((n = read(ptyfd, buf, BUFSIZ)) <= 0) {
                epilogue();
                do_end();
            }
	    i = (*code_trans[(pty_c_flag << 2) | tty_c_flag])(outbuf, buf, n);
	    if (i <= 0) continue;
	    p = outbuf;
	    push_cursor();
	    kk_restore_cursor();
	    while ((j = write(ttyfd, p, i)) < i) {
		if (j >= 0) {
		    p += j;
		    i -= j;
		}
		mask = 1 << ttyfd;
		select(32, 0, &mask, 0, 0);
	    }
	    pop_cursor();
        }
        if (rfds & tty_bit) {
            if ((n = read(ttyfd, buf, BUFSIZ)) > 0) {
		if (henkan_off_flag == 1 && tty_c_flag == pty_c_flag)
		    i = through(outbuf, buf, n);
		else
		    i = (*code_trans[(tty_c_flag << 2) | J_EUJIS])(outbuf, buf, n);
		if (i <= 0) continue;
		bufstart = outbuf;
		bufend = outbuf + i;
                return *bufstart++;
#ifdef nodef		
            }else{  /* Consider it as EOF */
		epilogue();
		do_end();
#endif   /* It seems that select does not return EOF when  Non-brock
	  What should I do?*/
	    }
        }
    }
}
#endif	/* defined(uniosu) */

#if defined(uniosu)
/** キー入力関数 2 */
int keyin0()
{
    static unsigned char buf[BUFSIZ];
    static unsigned char outbuf[BUFSIZ];
    static unsigned char *bufend = buf;
    static unsigned char *bufstart = buf;
    int code,size;
    int n,un;
    int rfds;
    int key_code;
    int i;
    unsigned char *p;
    extern int henkan_off_flag;
#ifdef BSD42
    struct timeval time_out;
#else
    int	time_out;
#endif
    int	sel_ret;

    if (bufstart < bufend) {
        return *bufstart++;
    }
    for (;;) {
        if ((n = read(ttyfd, buf, BUFSIZ)) > 0) {
	    if (henkan_off_flag == 1 && tty_c_flag == pty_c_flag)
		i = through(outbuf, buf, n);
	    else
		i = (*code_trans[(tty_c_flag << 2) | J_EUJIS])(outbuf, buf, n);
	    if (i <= 0) continue;
            bufstart = outbuf;
            bufend = outbuf + i;
            return *bufstart++;
        }

#ifdef BSD42
	time_out.tv_sec = 2;
	time_out.tv_usec = 0;
#else
	time_out = 2;	/* 2 sec 間待つのだゾ! */
#endif
	for(rfds = sel_ptn;
	    (sel_ret = select(20, &rfds, 0, 0, &time_out)) < 0 && errno == EINTR;
	    rfds = sel_ptn)
	  ;

        if (sel_ret == 0) {
	    if ((tty_c_flag == J_JIS) && ((i = jis_flush(outbuf)) > 0)) {
		/* 溜まっているＥＳＣを吐き出す */
		bufstart = outbuf;
		bufend = outbuf + i;
		return *bufstart++;
	    }
	    return (-1);
	}
        if (rfds & pty_bit) {
            if ((n = read(ptyfd, buf, BUFSIZ)) <= 0) {
                epilogue();
                do_end();
            }
	    if (*buf == 0) { /* sequence of data */
		i=(*code_trans[(pty_c_flag<<2)|tty_c_flag])(outbuf, buf+1, n-1);
		if (i <= 0) continue;
		push_cursor();
		kk_restore_cursor();
		write(ttyfd, outbuf , i);
		pop_cursor();
	    } else if (*buf == PIOCPKT_IOCTL) {
		arrange_ioctl(1);
	    }
        }
        if (rfds & tty_bit) {
            if ((n = read(ttyfd, buf, BUFSIZ)) > 0) {
		if (henkan_off_flag == 1 && tty_c_flag == pty_c_flag)
		    i = through(outbuf, buf, n);
		else
		    i = (*code_trans[(tty_c_flag << 2) | J_EUJIS])(outbuf, buf, n);
		if (i <= 0) continue;
		bufstart = outbuf;
		bufend = outbuf + i;
                return *bufstart++;
            }
        }
    }
}
#endif	/* defined(uniosu) */

#if defined(uniosu)
/** pty から ioctl がかかった時の処理 */
int arrange_ioctl(jflg)
int jflg; /* jtermio の j_flg の変換フラグがオフの時 0 オンの時 1 */ 
{
    struct jtermio jbuf1;
    struct termio frombuf;
    struct termio tobuf;
    int i;

    ioctl(ptyfd, TCGETA, &frombuf);
    ioctl(ttyfd, TCGETA, &tobuf);

    if ((i = (frombuf.c_iflag & IXON)) != (tobuf.c_iflag & IXON)) {
	if (i == 0) {
	    tobuf.c_iflag &= ~IXON;
	} else {
	    tobuf.c_iflag |= IXON;
	}
    }
    if ((i = (frombuf.c_iflag & IXOFF)) != (tobuf.c_iflag & IXOFF)) {
	if (i == 0) {
	    tobuf.c_iflag &= ~IXOFF;
	} else {
	    tobuf.c_iflag |= IXOFF;
	}
    }
    if ((i = (frombuf.c_oflag & OPOST)) != (tobuf.c_oflag & OPOST)) {
	if (i == 0) {
	    tobuf.c_oflag &= ~OPOST;
	} else {
	    tobuf.c_oflag |= OPOST;
	}
    }
    tobuf.c_cflag = (tobuf.c_cflag & ~CBAUD) | (frombuf.c_cflag & CBAUD);

    ioctl(ttyfd, TCSETA, &tobuf); /* set again */

    ioctl(ptyfd, JTERMGET, &jbuf1); /* about Japanease */

    if ((jflg) && ((jbuf1.j_flg & KANJIINPUT) == 0)) {
    	jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT); /* kanji henkan flg off */
	ioctl(ttyfd, JTERMSET, &jbuf1);
	kk_restore_cursor();
	reset_cursor();
	longjmp(kk_env,1);
    }
    if ((!jflg) && ((jbuf1.j_flg & KANJIINPUT) != 0)) {
    	jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT); /* kanji henkan flg off */
    	ioctl(ttyfd, JTERMSET, &jbuf1);
	return(1);
    }
    jbuf1.j_flg &= ~(KANJIINPUT | CONVTOINT); /* kanji henkan flg off */
    ioctl(ttyfd, JTERMSET, &jbuf1);
    return(0);
}
#endif	/* defined(uniosu) */

/** 子プロセスを起こす。*/

int ttypfd;

static void
exec_cmd(argv)
char **argv;
{
    int i, pid;
#ifdef SYSVR2
	setpgrp();
	close(open(ttyname(ttypfd), O_WRONLY, 0));
#endif /* SYSVR2 */

    if (!(child_id = fork())) {
#ifdef BSD42
	/* set notty */
	(void) ioctl(ttypfd,TIOCNOTTY,0);
	/* set tty process group */
	pid = getpid();
	ioctl(ttypfd, TIOCSPGRP, &pid);
	setpgrp(0, 0);
	close(open(ttyname(ttypfd), O_WRONLY, 0));
	setpgrp(0, pid);
#endif /* BSD42 */
/*
#ifdef SYSVR2
	setpgrp();
	close(open(ttyname(ttypfd), O_WRONLY, 0));
#endif 
*/
	if (setutmp(ttypfd) == ERROR) {
	    puts("Can't set utmp.");
	} 

	setgid(getgid());
	setuid(getuid());
	close(0);
	close(1);
	close(2);
	if (dup(ttypfd) != 0 || dup(ttypfd) != 1 || dup(ttypfd) != 2) {
	    err("redirection fault.");
	}
#ifdef UX386
	for (i = NOFILES - 1; i > 2; i--) {
#else
	for (i = getdtablesize() - 1; i > 2; i--) {
#endif
	    close(i);
	}

	restore_signals();

#ifdef SIGTSTP
	signal(SIGTSTP, SIG_IGN);
#endif
#ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
#endif
#ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
#endif

/*	if (setutmp(0) == ERROR) {
	    err("Can't set utmp.");
	} 
*/
	execvp(cmdnm, argv);
	err("exec fault.");
    }
}

/** 環境変数のセット */
void
setenv(var, value)
char *var;
char *value;
{
    extern char **environ;
    char *malloc();
    char **newenv;
    register int i, j;

    j = strlen(var);
    for (i = 0; environ[i] != NULL; i++) {
	if (strncmp(var, environ[i], j) == 0 && environ[i][j] == '=') {
	    break;
	}
    }
    if (environ[i] == NULL) {
	if ((newenv = (char **)malloc((sizeof (char *))*(i + 2))) == NULL) {
	    puts("Can't allocate environ.");
	    exit(1);
	}
	for (j = 0; j < i + 1; j++) {
	    newenv[j] = environ[j];
	}
	newenv[i + 1] = NULL;
	environ = newenv;
    }
    if ((environ[i] = malloc(strlen(var) + strlen(value) + 2)) == NULL) {
	puts("Can't allocate environ area.");
	exit(1);
    }
    strcpy(environ[i], var);
    strcat(environ[i], "=");
    strcat(environ[i], value);
}

/** ttyp のオープン */

int ptyno;
char *ptynm = "/dev/pty";
char *ttypnm = "/dev/tty";

#ifdef	BSD43	/* FOR PASS8 */
static int local_mode_sv;
#endif

static void ptyname();

static void
open_ttyp()
{
    char nmbuf[20];
    int word;
    int pid;
#ifdef SYSVR2
    struct termio buf1;
#endif
#if defined(uniosu)
    struct jtermio buf2;
#endif

    ptyname(nmbuf, ttypnm, ptyno);
    if ((ttypfd = open(nmbuf, O_RDWR, 0)) == ERROR) {
	err("Can't open ttyp.");
    }
    chown(nmbuf, getuid(), getgid());
    chmod(nmbuf, 0622);
#ifdef BSD42
    ioctl(ttyfd, TIOCGETC, &tcharsv);
    ioctl(ttyfd, TIOCGLTC, &ltcharsv);
    ioctl(ttyfd, TIOCGETP, &ttyb);
    savetmio = ttyb;
#ifdef BSD43
    ioctl(ttyfd, TIOCLGET, &local_mode_sv);
    local_mode = local_mode_sv | LPASS8;	/* set PASS8 */
    ioctl(ttyfd, TIOCLSET, &local_mode);
#else /* BSD42 */
    ioctl(ttyfd, TIOCLGET, &local_mode);
#endif /* BSD43 */

    ioctl(ttypfd, TIOCSETC, &tcharsv);
    ioctl(ttypfd, TIOCSLTC, &ltcharsv);
    ioctl(ttypfd, TIOCSETP, &ttyb);
    ioctl(ttypfd, TIOCLSET, &local_mode);
#ifdef TIOCSSIZE
    pty_rowcol.ts_lines = crow; /* instead of lines */
    pty_rowcol.ts_cols = maxlength; /* instead of columns */
    ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
#endif /* TIOCSSIZE */
    pid = getpid();
    ioctl(ttypfd, TIOCSPGRP, &pid);
    if (pty_c_flag == J_JIS) {
	word = LCTLECH;
	ioctl(ttypfd, TIOCLBIC, &word);
    }
    {
	int fd;

	/* Above code dosen't suit on SunOS 4.1.X */
	/* Modefied by sho */

	if ((fd = open("/dev/tty",O_WRONLY)) >= 0 ) {
	    /* set notty */
	    (void) ioctl(fd,TIOCNOTTY,0);
	    close(fd);
	}
    }
    setpgrp(0, 0);
    close(open(ttyname(ttypfd), O_WRONLY, 0));
    setpgrp(0, pid);
#endif /* BSD42 */
#ifdef SYSVR2   
    if (ioctl(ttyfd, TCGETA, &buf1) == -1) {
	buf1 = terms;
    }
    savetmio = buf1;
    if (ioctl(ttypfd, TCSETA, &buf1) < 0) {
	fprintf(stderr, "error in ioctl TCSETA.\n");
	exit(1);
    }
#if defined(uniosu)
    if (ioctl(ttyfd, JTERMGET, &buf2) < 0) {
	fprintf(stderr, "uum: error in ioctl JTERMGET in open_ttyp.\n");
	exit(1);
    }
    savejtmiottyp = buf2;
    buf2.j_flg = CONVTOEXT | KANJIINPUT; /* kanji input & output ok */
    buf2.j_level = jterm;
    switch (jcode_set) {
    case 0:
	buf2.j_ecode = JIS;
	break;
    case 1:
	buf2.j_ecode = SJIS;
	break;
    case 2:
	buf2.j_ecode = UJIS;
	break;
    default:
	fprintf(stderr , "uum: kanji code set not supported in terminfo.\n");
	exit(1);
    }

    if (jis_kanji_in) {
	strcpy(buf2.j_jst, jis_kanji_in);
	buf2.j_jstl = strlen(jis_kanji_in);
    }
    if (jis_kanji_out) {
	strcpy(buf2.j_jend, jis_kanji_out);
	buf2.j_jendl = strlen(jis_kanji_out);
    }
    if (jgaiji_start_address) { 
	*(short *)buf2.j_gcsa = jgaiji_start_address;
    }
    if (jgaiji_disp) {
	strcpy(buf2.j_gdsp, jgaiji_disp);
	buf2.j_gdspl = strlen(jgaiji_disp);
    }

    if (ioctl(ttypfd, JTERMSET, &buf2) < 0) {
	fprintf(stderr, "error in ioctl JTERMSET.\n");
	exit(1);
    }

    if (ioctl(ttypfd, TIOCSETAUX, &auxterm) < 0) {
	fprintf(stderr, "error in ioctl TIOCSETAUX.\n");
	exit(1);
    }

/*
    pid = getpid();
*/
#endif	/* defined(uniosu) */
/*
    setpgrp();
    close(open(ttyname(ttypfd), O_WRONLY, 0));
*/
#endif /* SYSVR2 */
}

/** pty のオープン */
static void
open_pty()
{
    char nmbuf[20];

    for (ptyno = 0; ptyno < 32; ptyno++) {
        ptyname(nmbuf, ptynm, ptyno);
        if ((ptyfd = open(nmbuf, O_RDWR, 0)) != ERROR) {
#if defined(uniosu)
	    if(ioctl(ptyfd, PIOCPKT, 1) < 0) { /* packet mode on */
		fprintf(stderr, "error in ioctl PIOCPKT.\n");
		exit(1);
	    }
#endif
            return;
        }
    }
    err("Can't get pty.");
}

/** エラーだよ。さようなら。 */
void
err(s)
char *s;
{
    puts(s);
    fclose(stdout);
    fclose(stderr);
    fclose(stdin);
    do_end();
}

/** 立つ鳥後を濁さず 終わりの処理 */
static void
do_end()
{
    char nmbuf[20];

    static int do_end_flg = 0;
    if (do_end_flg == 1)
	return;
    do_end_flg = 1;

    signal(SIGCHLD, SIG_DFL);
    fcntl(ttyfd, F_SETFL, 0);

#ifdef BSD42
    ioctl(ttyfd, TIOCSETP, &savetmio);
#endif /* BSD42 */
#ifdef BSD43
    ioctl(ttyfd, TIOCLSET, &local_mode_sv);
#endif /* BSD43 */

#ifdef SYSVR2
    if(ioctl(ttyfd, TCSETA, &savetmio) < 0) {
	fprintf(stderr, "error in ioctl TCSETA.\n");
	exit(1);
    }
#endif /* SYSVR2 */

#if defined(uniosu)
    if (ioctl(ttyfd, JTERMSET,  &savejtmio) < 0) {
	fprintf(stderr, "error in ioctl JTERMSET.\n");
	exit(1);
    }
#endif	/* defined(uniosu) */

    ptyname(nmbuf, ptynm, ptyno);
    if (chown(nmbuf, 0, 0) == ERROR) {
	perror("uum");
    }
    if (chmod(nmbuf, 0666) == ERROR) {
	perror("uum");
    }

    ptyname(nmbuf, ttypnm, ptyno);
    if (chown(nmbuf, 0, 0) == ERROR) {
	perror("uum");
    }
    if (chmod(nmbuf, 0666) == ERROR) {
	perror("uum");
    }

    if (resetutmp(ttypfd) == ERROR) {
	printf("Can't reset utmp.");
    }
#ifdef TIOCSSIZE
    pty_rowcol.ts_lines = 0;
    pty_rowcol.ts_cols = 0;
    ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
#endif /* TIOCSSIZE */
    close(ttyfd);
    close(ptyfd);

    chdir("/tmp"); /* to avoid making too many mon.out files */

    
#ifdef BSD42
    killpg(child_id,SIGHUP);
/*    killpg(getpgrp(),SIGHUP);  */
#endif
#ifdef SYSVR2
/*    kill(0, SIGHUP); */
    kill(-child_id,SIGHUP);
#endif /* SYSVR2 */

    exit(0);
}

#if defined(uniosu)
/** 仮名漢字変換を ioctl でオフした時の keyin に代わる関数 */
int ioctl_off()
{
    static unsigned char buf[BUFSIZ];
    int n;
    int rfds;

    kk_restore_cursor();
    clr_line_all();
    display_henkan_off_mode();

    for (;;) {
        if ((n = read(ttyfd , buf , BUFSIZ)) > 0) {
	    write(ptyfd, buf, n);
        }
        rfds = sel_ptn;
        select(20, &rfds, 0, 0, NULL);
        if (rfds & pty_bit) {
            if ((n = read(ptyfd, buf, BUFSIZ)) <= 0) {
                epilogue();
                do_end();
            }
	    if (*buf == 0) {	/* sequence of data */
		write(ttyfd, buf + 1, n - 1);
	    } else if (*buf == PIOCPKT_IOCTL) {
		if (arrange_ioctl(0) > 0) {
		    return;
		}
	    }
        }
        if (rfds & tty_bit) {
            if ((n = read(ttyfd, buf, BUFSIZ)) > 0) {
		write(ptyfd, buf, n);
            }
        }
    }
}
#endif	/* defined(uniosu) */


static void
ptyname(b, pty, no)
char *b, *pty;
int no;
{
    sprintf(b, "%s%1c%1x", pty, 'p' + (no >> 4), no & 0x0f);
}

int
through(x, y, z)
char *x, *y;
int z;
{
    bcopy(y, x, z);
    return z;
}

static void
usage(optstr)
char *optstr;
{
  printf("usage: uum %s\n", optstr);
/*
#ifdef SYSVR2
  printf("usage: uum [-hHjJuUsSxXk:c:l:r:D:n:]\n");
#endif

#ifdef BSD42
  printf("usage: uum [-hHjJuUsSkc:l:r:D:n:]\n");
#endif
*/
  exit(0);
}

/*
  save/restore signal settings
 */

#if defined(sun) || defined(ultrix)
 typedef void (*intfnptr)();
#else
 typedef int (*intfnptr)(); 
#endif
intfnptr sigpipe, sighup, sigint, sigquit, sigterm, sigtstp,
  sigttin, sigttou, sigchld;
#ifdef SIGWINCH
intfnptr sigwinch;
#endif /* SIGWINCH */

static void save_signals()
{
    sigpipe = signal(SIGPIPE, SIG_IGN);
/*     sighup  = signal(SIGHUP,  SIG_IGN); /88/12/27*/
    sighup  = signal(SIGHUP,  terminate_handler);
    sigint  = signal(SIGINT,  SIG_IGN);
    sigquit = signal(SIGQUIT, SIG_IGN);
    sigterm = signal(SIGTERM, terminate_handler);
    sigchld = signal(SIGCHLD, chld_handler);
#ifdef SIGWINCH
    sigwinch = signal(SIGWINCH, resize_handler);
#endif /* SIGWINCH */
#ifdef SIGTSTP
    sigtstp = signal(SIGTSTP, SIG_IGN);
    sigttin = signal(SIGTTIN, SIG_IGN);
    sigttou = signal(SIGTTOU, SIG_IGN);
#endif /* SIGTSTP */
}

static void restore_signals()
{
    signal(SIGPIPE, sigpipe);
    signal(SIGHUP,  sighup);
    signal(SIGINT,  sigint);
    signal(SIGQUIT, sigquit);
    signal(SIGTERM, sigterm);
    signal(SIGCHLD, sigchld);
#ifdef SIGWINCH
    signal(SIGWINCH, sigwinch);
#endif /* SIGWINCH */
#ifdef SIGTSTP
    signal(SIGTSTP, sigtstp);
    signal(SIGTTIN, sigttin);
    signal(SIGTTOU, sigttou);
#endif /* SIGTSTP */
}

#ifdef BSD43
static void
setsize()
{
	register int i;
	struct winsize win;
	extern Term_LineWidth,Term_RowWidth,maxlength,crow;

	if (ioctl(ttyfd, TIOCGWINSZ, &win) < 0) {
		/* Default set at getTermData() */
		return;
	} else {
		if ((i = win.ws_row) != 0) {
			crow = Term_RowWidth = i - conv_lines;
		}
		if ((i = win.ws_col) != 0) {
			maxlength = Term_LineWidth = i;
		}
	}
}
#endif /* BSD43 */

#ifdef	SIGWINCH
static void
change_size()
{
	register int i;
	struct winsize win;
	extern Term_LineWidth,Term_RowWidth,maxlength,crow;

	if (ioctl(ttyfd, TIOCGWINSZ, &win) < 0) {
		/* Default set at getTermData() */
		return;
	} else {
	    int grp;

	        throw_cur_raw(0,crow);
	        clr_line();

		if ((i = win.ws_row) != 0) {
			crow = Term_RowWidth = i - conv_lines;
		}
		if ((i = win.ws_col) != 0) {
			maxlength = Term_LineWidth = i;
		}
#ifdef	TIOCSSIZE
    		pty_rowcol.ts_lines = crow; /* instead of lines */
    		pty_rowcol.ts_cols = maxlength; /* instead of columns */
    		ioctl(ttypfd, TIOCSSIZE, &pty_rowcol);
#endif	/* TIOCSSIZE */
		ioctl(ptyfd, TIOCGPGRP, &grp);
		killpg(grp, SIGWINCH);
/*		killpg(getpgrp(typfd), SIGWINCH); */

		set_scroll_region(0, crow - 1);
		if (henkan_off_flag) {
			kk_restore_cursor();
			throw_cur_raw(0 ,0);
			kk_save_cursor();
			display_henkan_off_mode();
			set_screen_vars_default();
			t_print_l();
			kk_restore_cursor();
		} else {
			kk_restore_cursor();
			throw_cur_raw(0 ,0);
			kk_save_cursor();
			disp_mode();
			set_screen_vars_default();
			t_print_l();
		}
	}
}
#endif	/* SIGWINCH */
