/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#ifndef lint
static char rcs[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/delwords/RCS/delwords.c,v 1.1 91/06/05 10:21:17 kon Exp $";
#endif
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"

#define TRUE 1
#define FALSE 0

#ifdef nec_ews_svr2
#define SIGVAL int
#else
#ifdef nec_ews_svr4
#define SIGVAL void
#else
#define SIGVAL void
#endif /* nec_ews_svr4 */
#endif /* nec_ews_svr2 */

extern	RkDeleteLine();
extern void Message();

char	init[RECSZ], Progname[RECSZ];
char	*r_dic;
int	cx_num, is_display, mode;

static int	Delwords(), ParseFile();
static SIGVAL	RefreshAll(), StopAll();

void
usage()
{
    (void) fprintf(stderr, "Usage: %s [options]  remote-dic\n",
		  Progname);
    (void) fprintf(stderr, " options include:\n");
    (void) fprintf(stderr, "\t{-is | -irohaserver} iroha-server\n");
    (void) fprintf(stderr, "\t-s\n");
    (void) fprintf(stderr, "\t-l local-file\n");
    (void) fflush(stderr);
    exit(ERR_VALUE);
}

static SIGVAL
StopAll()
{
#ifdef DEBUG
    Message("StopAll: Caught signal, Quit delwords and do nothing");
#endif
    (void) signal(SIGINT,  SIG_IGN);
    (void) signal(SIGQUIT, SIG_IGN);
    (void) signal(SIGTERM, SIG_IGN);
    /*
     * Shutting down and close connection with server.
     */
    (void) RkFinalize();
#ifdef DEBUG
    Message("do nothing to dictionary.");
#endif
    Message("\ndelwordsを中断します。");
    exit(ERR_VALUE);
}

static SIGVAL
RefreshAll()
{
#ifdef DEBUG
    Message("RefreshAll: Caught signal, Shutting down delwords.");
#endif
    (void) signal(SIGINT,  SIG_IGN);
    (void) signal(SIGQUIT, SIG_IGN);
    (void) signal(SIGTERM, SIG_IGN);

    /*
     * Shutting down and close old connection with server.
     */
    (void) RkUnmountDic(cx_num, r_dic);
    (void) RkFinalize();

    /*
     * Restart new connection with server and delete dic.
     */
    cx_num = RkInitialize(init);
    if (cx_num != 0) {
    } else {
	(void) RkFinalize();
    }
    Message("delwordsを中断します。");
    exit(ERR_VALUE);
}

static int
ParseFile(fp)
  FILE	*fp;
{
    char line[RECSZ];
    int ret = -1;

    if (is_display != TRUE) {
	(void) fprintf(stderr, "\n");
    }
    while (fgets((char *)line, sizeof(line), fp)) {
	if (RkDeleteLine(cx_num, r_dic, line) == -1) {
	    Message("write error \"%s\"", line);
	    continue;
	}
	if ((is_display == TRUE) &&
	    ((++ret & 0x0f) == 0)) {
	    (void) fputs(".", stderr);
	}
    }
    RkUnmountDic(cx_num, r_dic);
    return (0);
}

static int
Delwords(fp)
  FILE	*fp;
{
    if (RkMountDic(cx_num, r_dic, 0)) {
	(void) Message("辞書 \"%s\" をマウントできませんでした。", r_dic);
	return (-1);
    }
    return (ParseFile(fp));
}

main (argc, argv)
  int argc;
  char *argv [];
{
    FILE	*fp;
    char	*l_file = NULL;

    mode = Rk_MWD;
    is_display = FALSE;

    (void) strcpy(Progname, *argv);
	(void) strcpy(init,"/usr/lib/iroha/dic");
    while(--argc) {
	argv++;	
	if (strcmp(*argv, "-is") == 0 ||
	    strcmp(*argv, "-irohaserver") == 0) {
	    if (--argc) {
		argv++;
		(void) strcpy(init, *argv);
		continue;
	    }
	} else if (strcmp(*argv, "-s") == 0) {
	    if (mode == Rk_MWD) {
		mode = Rk_SWD;
		continue;
	    }
	} else if (strcmp(*argv, "-l") == 0) {
	    if (--argc) {
		argv++;
		if (!l_file) {
		    l_file = *argv;
		    continue;
		}
	    }
	} else if (strcmp(*argv, "-h") == 0) {
		(void)usage();
	} else if (!r_dic) {
	    if ((unsigned char *)strchr(*argv, '-' )) {
		Message("辞書名に\"-\"は使用できません。");
		exit(ERR_VALUE);
	    }
	    if (strlen(*argv) >= RECSZ) {
		Message("辞書名\"%s\"が長すぎます。", *argv);
		exit(ERR_VALUE);
	    }
	    r_dic = *argv;
	    continue;
	}
	usage();
    }
    if (!r_dic)
	usage();
    if (!l_file) {
	fp = stdin;
	if (isatty(fileno(stdin)) == 0) {
	    is_display = TRUE;
	}
    } else {
	is_display = TRUE;
	fp = (FILE *)fopen(l_file, "r");
	if (!fp) {
	    Message("%s: cannot open \"%s\"", Progname, l_file);
	    exit(ERR_VALUE);
	}
    }

    /*
     * main routin.
     */
    if (cx_num = RkInitialize(init)) {
	(void) fprintf (stderr, "かな漢字変換サーバ");
	if (init[0] != '/') {
	    (void)fprintf(stderr, "\"%s\"", init);
	}
	(void)fprintf(stderr, "と接続できませんでした。\n");
	exit (ERR_VALUE);
    }
    if (Delwords(fp) == -1) {
#ifdef DEBUG
	Message("辞書\"%s\"でdelwordsできませんでした。", r_dic);
#endif
	if (!l_file)
	    (void) fclose(fp);
	(void) RkFinalize();
	exit(ERR_VALUE);
    }
    (void) RkFinalize();
    Message("\n辞書\"%s\"でdelwordsしました。", r_dic);
    exit (0);
}
