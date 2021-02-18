#ifndef lint
static char *rcs_id = "$Id: wnntouch.c,v 1.2 1991/09/16 21:32:34 ohm Exp $";
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
  ����Υإå���񤭴����ơ������ inode �ȹ�碌��ץ���ࡣ
  */

#include <stdio.h>
#include "jslib.h"
#include "commonhd.h"

static void usage();

char *com_name;
struct wnn_file_head fh;

static void
parse_options(argc, argv)
int argc;
char **argv;
{
    int c;
    extern int optind;
    extern char *optarg;

    while ((c = getopt(argc,argv,"")) != EOF) {
    }
    if (optind) {
	optind--;
	argc -= optind;
	argv += optind;
    }
    if(argc < 2){
	usage();
    }
}

static void
usage()
{
  fprintf(stderr , "Usage: %s Wnn_file_name* \n",com_name);
  exit(1);
}

void
main(argc, argv)
int argc;
char **argv;
{
    FILE *ifpter;
    int k;

    com_name = argv[0];
    parse_options(argc, argv);

    for(k = 1 ; k < argc ; k++){
	if((ifpter = fopen(argv[k] , "r")) == NULL){
	    fprintf(stderr , "Can't open the input file %s.\n" , argv[k]);
	    perror("");
	    exit(1);
	}
	if(input_file_header(ifpter, &fh) == -1){
	    fprintf(stderr, "%s %s: It's not a Wnn File.\n", com_name, argv[k]);
	    exit(1);
	}
	if(check_inode(ifpter, &fh) == -1){
	    if(change_file_uniq(&fh, argv[k]) == -1){
		fprintf(stderr, "%s %s: Can't change file_uniq.\n", com_name, argv[k]);
		perror("");
		exit(1);
	    }
	}
	fclose(ifpter);
    }
    exit(0);
}
