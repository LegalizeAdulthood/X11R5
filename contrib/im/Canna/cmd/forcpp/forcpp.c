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
static char rcsid[]="@(#) 102.1 $Header: /imports/wink/work/nk.R3.1/cmd/forcpp/RCS/forcpp.c,v 1.7 91/09/13 15:29:05 kubota Exp $";
#endif
/*
 * forcpp.c	/lib/cpp を通す時に8ビットコードを保護する
 *	forcpp -7 < [in-file-name] > [out-file-name]
 *		-7	前処理
 *		-8	後処理
 */
#include	<stdio.h>
#include	<signal.h>
#include	<ctype.h>

char	*hd	= "0123456789abcdef";

/* #define	ESC	'@'*/
#define	ESC 033	
e2j()
{
    unsigned	c;
    int		kin = 0;

    while ( (c = getchar()) != EOF ) {
	if ( c & 0x80 ) {
	    if ( !kin ) {
		putchar(ESC);
		kin = 1;
	    };
	    putchar(hd[c>>4]);
	    putchar(hd[c&15]);
	}
	else {
	    if ( kin ) {
		putchar(ESC);
		kin = 0;
	    };
	    putchar(c);
	};
    };
}

j2e()
{
    unsigned	c;
    int		kin = 0;

    while ( (c = getchar()) != EOF ) {
	if ( c == ESC ) {
	    kin = 1 - kin;
	}
	else  {
	    if ( kin ) {
		char	s[3];

		s[0] = c;
		s[1] = getchar();
		s[2] = 0;
		sscanf(s, "%x", &c);
		

	    }
	    putchar(c);
	};
    };
}

catch(sig)
int sig;
{
  fprintf(stderr, "Dictionary format error.\n");
  exit(1);
}

main(n, args)
int	n;
char	*args[];
{

  signal(SIGSEGV, catch);
  signal(SIGBUS, catch);

  if( n == 1 ) {		/* コマンド名だけの時 */
      fprintf(stderr,
	      "usage: forcpp -7 < [file],\n       forcpp -8 < [file]\n");
      exit( -1 );
  }

  if( !strcmp(args[1], "-7"))
      e2j();
  else if( !strcmp(args[1], "-8"))
      j2e();
  else
      fprintf(stderr,
	      "usage: forcpp -7 < [file],\n       forcpp -8 < [file]\n");

  exit(0);
}
