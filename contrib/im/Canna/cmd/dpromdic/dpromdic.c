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
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/dpromdic/RCS/dpromdic.c,v 1.6 91/08/07 10:40:06 y-morika Exp $";
#endif

# include	<stdio.h>
# include	"RK.h"
# include	"RKintern.h"

main(argc, argv)
int	argc;
char	**argv;
{
  char *filename;
  struct RkRxDic *rdic;
  unsigned char *s;
  int i, mode;

  mode = 0x0004; /* 読み取り可能か？ */

  if(argc != 2){
    fprintf(stderr, "usage: dpromdic <file name>\n");
    exit(1);
    /* NOTREACHED */
  }

  ++argv;
  filename = *argv;
  
  if(access(filename, mode) == -1) {
    fprintf(stderr, "cannot open %s\n", filename);
    exit(1);
    /* NOTREACHED */
  }

  if(rdic = RkOpenRoma(filename)) {
    for(i=0; i<rdic->nr_nkey; i++) {
      s = rdic->nr_keyaddr[i];
      /* ローマ字 */
      for(s = rdic->nr_keyaddr[i]; *s; s++) {
	switch(*s) {
	  case ' '    :
	  case '#'    :
	  case '\\'   : printf("\\");
          default     : printf("%c", *s);
                        break;
	}
      }
      printf("\t");
      s++;
      /* かな */
      for(; *s; s++) {
	switch(*s) {
	  case ' '    :
	  case '#'    :
	  case '\\'   : printf("\\");
          default     : printf("%c", *s);
    	                break;
	}
      }
      printf("\n");
    }
    RkCloseRoma(rdic);
    exit(0);
  } else {
    fprintf(stderr, "<%s> isn't rom-kana-table.\n", filename);
    exit(1);
    /* NOTREACHED */
  }
}
