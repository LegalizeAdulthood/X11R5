#ifndef	lint
static char rcs_id[] = "$Id: jhlp.c,v 1.3 1991/09/16 21:35:18 ohm Exp $";
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
#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include "commonhd.h"
#include "sdefine.h"
#include <X11/Xos.h>
#include "xjutil.h"
#include "sxheader.h"
#include "config.h"
#include "xext.h"

struct passwd *getpwuid();

#ifdef	BSD42
#include <sgtty.h>
#endif	/* BSD42 */
#ifdef SYSVR2
#include <termio.h>
#endif /* SYSVR2 */

extern char *optarg;
extern int optind;

extern char *getenv();

#define UNDEF_STTY -1

#ifdef BSD42
static void
get_rubout()
{
	struct sgttyb savetmio;
	int	fd;
	extern int open(), ioctl(), close();

	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
		rubout_code = RUBOUT;		/* BackSpase */
		return;
	}
	if (ioctl(fd, TIOCGETP, &savetmio) < 0) {
		close(fd);
		return;
	}
	if (savetmio.sg_erase == UNDEF_STTY) {
		rubout_code = RUBOUT;
	} else {
		rubout_code = savetmio.sg_erase;
	}
	close(fd);
}
#endif /* BSD42 */

#ifdef SYSVR2
static void
get_rubout()
{
	struct termio tmio;
	int	fd;

	if ((fd = open("/dev/console",O_WRONLY)) == -1) {
		rubout_code = RUBOUT;		/* BackSpase */
		return;
	}
	if (ioctl(fd, TCGETA, &tmio) < 0) {
		rubout_code = RUBOUT;		/* BackSpase */
		close(fd);
		return;
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

static void
do_end()
{
    xw_end();
    disconnect_server();
    exit(0);
}

void
terminate_handler()
{
    do_end();
}

static void
do_main()
{
    int buf[2];

    for (;;) {
      xw_read(buf);
    }
}

static int
keyin0()
{
    static int buf[BUFSIZ];
    static int *bufend = buf;
    static int *bufstart = buf;
    int  n;

    if (bufstart < bufend) {
        return (*bufstart++);
    }
    for (;;) {
        if ((n = xw_read(buf)) > 0) {
            bufstart = buf;
            bufend = buf + n;
            return (*bufstart++);
        }
    }
}

static int conv_keyin()
{
    int keyin0();

    return(keyin0());
}

int keyin()
{
    return(conv_keyin());
}


/*
  signal settings
 */

static void
save_signals()
{
    signal(SIGPIPE, SIG_IGN);
    signal(SIGHUP,  terminate_handler);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTERM, terminate_handler);
    signal(SIGCHLD, SIG_IGN);
}

void
main(argc, argv)
int argc;
char **argv;
{
    extern char xjutil_name[];
    extern int counter;
    prgname = argv[0];

#ifdef  DEBUG
    fprintf(stderr, "%s, %s, %s, %d\n", argv[0], argv[1], argv[2], argv[3][0]);
#endif  /* DEBUG */
    display_name = (char *)alloc_and_copy(argv[1]);
    strcpy(xjutil_name, argv[2]);
    counter = (int)argv[3][0];

    save_signals();

    get_rubout();


    maxchg = MAXCHG;
    maxlength = MAXCHG * 2;
    if(create_xjutil() == -1) {
	return_error();
	exit(-1);
    }
    if (init_xcvtkey() == -1) {
	return_error();
	exit(-1);
    }

    switch (init_wnn()) {
    case -1:
	terminate_handler();
	break;
    case -2:
	disconnect_server();
	do_end();
	break;
    }
    connect_server();

    do_main();
}

