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
 *
 */

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/irohacheck/RCS/main.c,v 1.2 91/08/30 21:37:55 kon Exp $";

#include <stdio.h>
#include <string.h>
#include <iroha/jrkanji.h>

int IROHA_verbose = 0;
  
main(argc, argv,envp)
int argc;
char *argv[], *envp[];
{
  char **warn;

  if (argc > 1 && !strcmp(argv[1], "-v")) {
    IROHA_verbose = 1;
  }
  else if (argc > 1) {
    fprintf(stderr, "usage: %s [-v]\n", argv[0]);
    exit (1);
  }
  jrKanjiControl(0, KC_SETVERBOSE,
		 IROHA_verbose ? CANNA_FULL_VERBOSE : CANNA_HALF_VERBOSE);
  if (jrKanjiControl(0, KC_INITIALIZE, &warn) != -1) {
    if (IROHA_verbose) {
      char *p, *RkGetServerName();

      p = RkGetServerName();
      printf("サーバ \"%s\" に接続します。\n", p);
    }
    if (warn) {
      char **p;

      for (p = warn; *p ; p++) {
	printf("%s\n", *p);
      }
    }
    jrKanjiControl(0, KC_FINALIZE, 0);
  }
}
