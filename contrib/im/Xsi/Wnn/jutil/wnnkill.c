#ifndef lint
static char *rcs_id = "$Id: wnnkill.c,v 1.2 1991/09/16 21:32:32 ohm Exp $";
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
#include "jslib.h"
#include "commonhd.h"
#include "config.h"

static void out();

WNN_JSERVER_ID	*js;
struct wnn_ret_buf rb = {0, NULL};
char *serv;
int ocode = TTY_KCODE;

void
main(argc, argv)
int argc;
char **argv;
{
    extern char *getenv();
    int c;
    char *tmp;
    int x;
    static char lang[64] = { 0 };
    extern int optind;
    extern char *optarg;
    char *p;

    if ((p = getenv("LANG")) != NULL) {
	strcpy(lang, p);
	   lang[5] = '\0';
    } else {
	lang[0] = '\0';
    }

    if (*lang == '\0')
#ifdef CHINESE
	strcpy(lang, WNN_DEFAULT_LANG);
#else
	strcpy(lang, WNN_DEFAULT_LANG);
#endif

    while ((c = getopt(argc,argv,"USJL:")) != EOF) {
	switch(c){
	case 'U':
	    ocode = J_EUJIS;
	    break;
	case 'J':
	    ocode = J_JIS;
	    break;
	case 'S':
	    ocode = J_SJIS;
	    break;
	case 'L':
	    strcpy(lang, optarg);
	    break;
	default:
	    break;
	}
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }

    if(argc > 1){
	serv = argv[1];
    }else if(tmp = getenv(WNN_JSERVER_ENV)){
	serv = tmp;
    }else{
	serv = "";
    }

    if((js=js_open_lang(serv, lang, WNN_TIMEOUT)) == NULL){
	out("%s:", serv);
	out("%s\n",wnn_perror_lang(lang)); 
/*	fprintf(stderr, "Can't connect to jserver.\n"); */
	exit(255);
    }
    if((x = js_kill(js)) > 0){
	out("%d Users Exists.\n", x);
	if (strcmp(lang, "zh_CN"))
	    out("Jserver Not Killed.\n");
	else
	    out("Cserver Not Killed.\n");
	exit(1);
    }else if(x == 0){
	if (strcmp(lang, "zh_CN"))
	    out("Jserver Terminated\n");
	else
	    out("Cserver Terminated\n");
	exit(0);
    }else{
	if (strcmp(lang, "zh_CN"))
	    out("Jserver Terminated\n");
	else
	    out("Cserver Terminated\n");
	exit(2);
    }
    exit (0);
}
	
extern	int eujis_to_jis8(), eujis_to_sjis();

static void
out(a1, a2, a3, a4, a5, a6, a7, a8)
char *a1, *a2, *a3, *a4, *a5, *a6, *a7, *a8;
{
    int len;
    char buf[1024];
    char jbuf[1024];
    sprintf(buf, a1, a2, a3, a4, a5, a6, a7, a8);

    len = strlen(buf);
    switch(ocode){
    case J_EUJIS:
	strncpy(jbuf, buf, len + 1);
	break;
    case J_JIS:
	eujis_to_jis8(jbuf, buf, len + 1);
	break;
    case J_SJIS:
	eujis_to_sjis(jbuf, buf, len + 1);
	break;
    }
    fprintf(stderr, "%s", jbuf);
}
