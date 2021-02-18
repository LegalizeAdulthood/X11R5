/*
 * $Id: error.c,v 1.3 1991/09/16 21:31:31 ohm Exp $
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
#include <time.h>
#include <errno.h>
#include <signal.h>
#include "commonhd.h"
#include "de_header.h"
#include <pwd.h>

static void my_error(), exit_hand();
void error1();

void
error_exit1(x , y1 , y2 , y3 , y4 , y5)
char *x;
int y1 , y2 , y3 , y4 , y5;
{
  char buf[512];

  strcpy(buf , "Fatal error. Exiting...\n");
  strcat(buf , x);
  error1(buf , y1, y2 , y3 , y4 , y5);
  exit_hand();
}
  
void
error_exit(x)
char *x;
{
  char buf[512];

  strcpy(buf , "Fatal error. Exiting...\n");
  strcat(buf , x);
  my_error(buf);
  exit_hand();
}


static void
my_error(x)
register char *x;
{
  long obakenoQ;
/*  struct passed *getpwiuid();  */

  if(!noisy)return;
  obakenoQ = time((long *)0);
  if(c_c){
#ifdef SYSVR2
      fprintf(stderr , "client %s :	%s	%s	 errno =%d\n\n" ,
/*	      getpwuid(c_c->uid)->pw_name, */
	      c_c->user_name,  /* V3.0 */
	      asctime(localtime(&obakenoQ)),
	      x , errno);
#endif
#ifdef BSD42
      fprintf(stderr , "client %s :	%s	%s	 \n\n" ,
/*	      getpwuid(c_c->uid)->pw_name,  */
	      c_c->user_name,  /* V3.0 */
	      asctime(localtime(&obakenoQ)),
	      x );
#endif
  }else{
#ifdef SYSVR2
      fprintf(stderr , "	%s	%s	 errno =%d\n\n" ,
	      asctime(localtime(&obakenoQ)),
	      x , errno);

#endif
#ifdef BSD42
      fprintf(stderr , "	%s	%s	 \n\n" ,
	      asctime(localtime(&obakenoQ)),
	      x );
#endif
  }
  fflush(stderr);
}

void
error1(x , y1 , y2 , y3 , y4 , y5)
register char *x;
int y1 , y2 , y3 , y4 , y5;
{
  char buf[512];

  strcpy(buf , "Jserver:");
  strcat(buf , x);
  sprintf(buf , x , y1 , y2 , y3 , y4 , y5);
  my_error(buf);
}
  

void
signal_hand(x)
int x;
{
  error1("signal catched signal_no = %d" , x);
#ifdef SYSVR2
  signal(x , signal_hand);
#endif
}

void
terminate_hand()
{
  demon_fin();
  exit(0);
}

static void
exit_hand()
{
  demon_fin();
  exit(250);
}

void
out(x , y1 , y2 , y3 , y4 , y5, y6, y7, y8, y9, y10, y11, y12)
char *x;
int y1 , y2 , y3 , y4 , y5,  y6, y7, y8, y9, y10, y11, y12;
{
  if(!noisy)return;
  fprintf(stderr, x, y1 , y2 , y3 , y4 , y5,  y6, y7, y8, y9, y10, y11, y12);
  fflush(stderr);
}



#ifdef	DEBUG
/*
	debug print
*/

#ifdef	putwchar
#undef	putwchar
#endif
void
putwchar(x)
unsigned short x;
{
    if(!noisy)return;
    putc( x >> 8, stderr);
    putc( x, stderr);
    /*
    putchar( x >> 8);
    putchar( x );
    */
	fflush(stdout);
}

void
wsputs(buf)
short *buf;
{
    if(!noisy)return;

    for(; ; ) {
	if (*buf == 0)
	    return;
	putwchar(*buf++);
    }
}
#endif	/* DEBUG */
