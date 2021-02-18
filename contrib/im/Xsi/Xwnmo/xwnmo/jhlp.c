#ifndef	lint
static char rcs_id[] = "$Id: jhlp.c,v 1.8 1991/09/30 02:05:23 proj Exp $";
#endif	/* lint */
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
#include <signal.h>
#include <pwd.h>
#include "commonhd.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xim.h"
#include "sheader.h"
#include "config.h"
#include "ext.h"

#ifdef	BSD42
#include <sgtty.h>
#endif	/* BSD42 */
#ifdef	SYSVR2
#include <termio.h>
#endif	/* SYSVR2 */
extern char *optarg;
extern int optind;

extern char *getenv();
extern void exit();

static void save_signals();

static struct _options {
    char *opt;
    char *desc;
} options[] = {
{ "-D servername",		"Wnn server to contact" },
{ "-lc localename",		"language name" },
{ "-n username",		"user name" },
{ "-k uumkeyname",		"uumkey file" },
{ "-r rkfilename",		"romkan file" },
{ "-F cvtfunname",		"convert function file" },
{ "-M cvtmetaname",		"convert meta file" },
{ "-fs fontnamelist",		"font name list" },
{ "-geometry geom",		"size (in pixels) and position" },
{ "-fg color",			"foreground color" },
{ "-bg color",			"background color" },
{ "-bd color",			"border color" },
{ "-bw number",			"border width in pixels" },
{ "-display displayname",	"X server to contact" },
{ "-iconic",			"start iconic" },
{ "#geom",			"icon window geometry" },
{ "-help",			"print out this message" },
{ "-h",				"wake up in convertion on mode" },
{ "-H",				"wake up in convertion off mode" },
#ifdef	USING_XJUTIL
{ "-ju",			"xjutil (dictionary utility manager) name" },
#endif	/* USING_XJUTIL */
{ NULL, NULL}
};

static int
message_open(lang)
register char *lang;
{
    char nlspath[MAXPATHLEN];

    strcpy(nlspath, LIBDIR);
    strcat(nlspath, "/%L/%N");

    cd = msg_open(XIMMSGFILE, nlspath, lang);
    if (cd->msg_bd == NULL) {
	print_out1("I can't open message_file \"%s\", use default message.",
		  cd->nlspath);
    }
    return(0);
}

static  void
usage(bad_option)
char *bad_option;
{
    struct _options *opt;
    int col;

    fprintf (stderr, "%s:  bad command line option \"%s\"\r\n\n",
	     prgname, bad_option);

    fprintf (stderr, "usage:  %s", prgname);
    col = 8 + strlen(prgname);
    for (opt = options; opt->opt; opt++) {
	int len = 3 + strlen(opt->opt);
	if (col + len > 79) {
	    fprintf (stderr, "\r\n   ");
	    col = 3;
	}
	fprintf (stderr, " [%s]", opt->opt);
	col += len;
    }

    fprintf (stderr, "\r\n\nType %s -help for a full description.\r\n\n",
	     prgname);
    exit(1);
}

static void
help()
{
    struct _options *opt;

    fprintf (stderr, "usage:\n        %s [-options ...]\n\n",
	     prgname);
    fprintf (stderr, "where options include:\n");
    for (opt = options; opt->opt; opt++) {
	fprintf (stderr, "    %-28s %s\n", opt->opt, opt->desc);
    }
    fprintf (stderr, "\n");
    exit(0);
}

/*
 * Parse options
 */
static void
parse_options(argc, argv)
int	argc;
char	**argv;
{
    int i;

    for (i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-h")) {
	    root_henkan_off_def = 1;
	    defined_by_option |= OPT_WAKING_UP_MODE;
	} else if (!strcmp(argv[i], "-H")) {
	    root_henkan_off_def = 0;
	    defined_by_option |= OPT_WAKING_UP_MODE;
	} else if (!strcmp(argv[i], "-k")) {
	    if ((++i >= argc) || (argv[i][0] == '-')) usage(argv[i]);
	    root_uumkeyname = alloc_and_copy(argv[i]);
	    if (uumkeyname == NULL || *uumkeyname == '\0') usage(argv[i]);
	    defined_by_option |= OPT_WNNKEY;
	} else if (!strcmp(argv[i], "-r")) {
	    if ((++i >= argc) || (argv[i][0] == '-')) usage(argv[i]);
	    root_rkfilename = alloc_and_copy(argv[i]);
	    if (rkfilename == NULL || *rkfilename == '\0') usage(argv[i]);
	    defined_by_option |= OPT_RKFILE;
	} else if (!strcmp(argv[i], "-help")) {
	    help();
	} else {
	    usage(argv[i]);
	}
    }
}


/* get rubout_code */
#define UNDEF_STTY -1

#ifdef BSD42
static void
get_rubout()
{
    struct sgttyb savetmio;
    int fd;
    extern int open(), ioctl(), close();

    if (ioctl(0, TIOCGETP, &savetmio) < 0) {
	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
	    rubout_code = RUBOUT;		/* BackSpase */
	    return;
	}
	if (ioctl(fd, TIOCGETP, &savetmio) < 0) {
	    close(fd);
	    return;
	}
    }
    if (savetmio.sg_erase == UNDEF_STTY) {
	rubout_code = RUBOUT;
    } else {
	rubout_code = savetmio.sg_erase;
    }
    close(fd);
    return;
}
#endif /* BSD42 */

#ifdef SYSVR2
static void
get_rubout()
{
    struct termio tmio;
    int fd;
    extern int close();

    if (ioctl(0, TCGETA, &tmio) < 0) {
	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
	    rubout_code = RUBOUT;		/* BackSpase */
	    return;
	}
	if (ioctl(fd, TCGETA, &tmio) < 0) {
	    rubout_code = RUBOUT;		/* BackSpase */
	    close(fd);
	    return;
	}
    }
    if (tmio.c_cc[VERASE] == UNDEF_STTY) {
	rubout_code = RUBOUT;
    } else {
	rubout_code = tmio.c_cc[VERASE];
    }
    close(fd);
    return;
}
#endif /* SYSVR2 */

void
do_end()
{
    xw_end();
    epilogue();
#ifdef	USING_XJUTIL
    kill_all_xjutil();
#endif	/* USING_XJUTIL */
    exit(0);
}

/** Handler of SIGTERM */
static void
terminate_handler()
{
    do_end();
}

/** Mail roop */

void
in_put(c)
int c;
{
    register int ml;

    do {
	ml = kk(c);
	if (ml >= 0) {
	   make_history(return_buf, ml);
	   xw_write(return_buf, ml);
	}
    } while (if_unget_buf());
}

static void
do_main()
{
    int type;

    for (;;) {
	X_flush();
	type = wait_for_socket();
	switch(type) {
	 case XEVENT_TYPE:
	    XEventDispatch();
	    break;
	 case REQUEST_TYPE:
	    RequestDispatch();
	    break;
#if defined(XJPLIB) && defined(XJPLIB_DIRECT)
	 case XJP_DIRECT_TYPE:
	    XJp_Direct_Dispatch();
#endif	/* defined(XJPLIB) && defined(XJPLIB_DIRECT) */
	 default:
	    break;
	}
    }
}

/*
  signal settings
 */

static void
 save_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP,  SIG_IGN);
#ifndef	NOTFORK
    signal(SIGINT,  SIG_IGN);
#endif	/* NOTFORK */
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, terminate_handler);
    signal(SIGCHLD, SIG_IGN);
}

void
main(argc, argv)
int argc;
char **argv;
{
    char *name;
    char *lc_name = NULL, *p;
    char lc_name_buf[16], lang[16];
    register int i;
    register XIMLangDataBase *db = NULL;
    extern struct passwd * getpwuid();

    prgname = argv[0];

    /*
     * get application resources
     */
    argc = get_application_resources(argc, argv);
    /*
     * parse option
     */
    parse_options(argc, argv);

    /*
     * Get user name and server name
     */
    if (root_username == NULL) {
	if((name = getenv(WNN_USERNAME_ENV)) != NULL){
	    root_username = alloc_and_copy(name);
	} else {
	    root_username = alloc_and_copy(getpwuid(getuid())->pw_name);
	}
    }
    if (root_def_servername == NULL) {
	if((name = getenv(WNN_JSERVER_ENV)) != NULL) {
	    root_def_servername = root_def_reverse_servername
				= alloc_and_copy(name);
	} else {
	    root_def_servername = root_def_reverse_servername = NULL;
	}
    }

    if (def_lang) {
	for (db = language_db; db->lang != NULL; db++) {
	    if (!strcmp(db->lang, def_lang)) {
		lc_name = setlocale(LC_ALL, db->lc_name);
		break;
	    }
	}
    }
    if (lc_name == NULL) {
	lc_name = setlocale(LC_ALL, "");
	if (lc_name == NULL || !strcmp(lc_name, "C")) {
	    if (!strcmp(lc_name, "C")) {
		fprintf (stderr, "%s : The current locale is C.", prgname);
		fprintf (stderr, " I try to set locale to defualt \"%s\"\n",
			 DEFAULT_LANG);
	    }
	    lc_name = setlocale(LC_ALL, DEFAULT_LANG);
	    if (lc_name == NULL) {
		fprintf (stderr, "%s : Can not set locale by setlocale()\n",
			 prgname);
		exit(1);
	    }
	    if (!strcmp(lc_name, "C")) {
		fprintf (stderr,
		"%s : Sorry, can not run under the C locale environment\n",
		prgname);
		exit(1);
	    }
	}
    }
    for (i = 0, p = lc_name; (*p != '@') && *p; i++, p++) {
	lc_name_buf[i] = *p;
    }
    lc_name_buf[i] = '\0';
    for (i = 0, p = lc_name_buf; (*p != '.') && *p; i++, p++) {
	lang[i] = *p;
    }
    lang[i] = '\0';
    cur_lang = lang;

    save_signals();

    get_rubout();

#ifndef NOTFORK
    if (fork()) {
	signal(SIGTERM, exit);
	signal(SIGCHLD, exit);
	pause();
    }
#endif	/* NOTFORK */

    close(fileno(stdin));
    close(fileno(stdout));
    /*
     * read ximrc file
     */
    if (read_ximrc() == -1) exit(1);

    /*
     * Open message mechanism
     */
    if (message_open(lang) == -1) exit(1);

    if (read_default_rk() == -1) exit(1);

    if (init_xcvtkey() == -1) exit(1);

    /*
     * Creation of X window environment
     */
    if(create_xim(lc_name_buf, lang) == -1) exit(1);

    if (c_c->use_server && !jl_isconnect(bun_data_)) {
	if (servername && *servername){
	    print_msg_getc(" I can not connect server(at %s)",
			   servername, NULL, NULL);
	} else {
	    print_msg_getc(" I can not connect server", NULL, NULL, NULL);
	}
    }

#ifndef NOTFORK
    /* End of initialization, kill parent */
    kill(getppid(), SIGTERM);
#endif /* !NOTFORK */

    clear_c_b();

    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;
    visual_status();
    if (henkan_off_flag == 0) {
	disp_mode();
    } else {
	henkan_off();
    }
    do_main();
}
