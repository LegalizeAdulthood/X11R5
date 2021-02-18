/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *snptgerr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/snap2/guardian/cmd/RCS/snptgerr.c,v 1.6 1991/09/12 17:19:22 bobg Exp $";

#include <pfiou.h>
#include <stdio.h>
#include <snap.h>
#include <gasp.h>
#include <snptguaru.h>

#ifndef TRUE
#define TRUE (0==0)
#define FALSE (0==1)
#endif

extern jmp_buf guarcatch;

void rl_error(fmt,
	 a1,a2,a3,a4,a6,a7,a8,a9,a10,
	 b1,b2,b3,b4,b6,b7,b8,b9,b10,
	 c1,c2,c3,c4,c6,c7,c8,c9,c10)
char *fmt;
long a1,a2,a3,a4,a6,a7,a8,a9,a10;
long b1,b2,b3,b4,b6,b7,b8,b9,b10;
long c1,c2,c3,c4,c6,c7,c8,c9,c10;
{
  fflush(stdout);
  fprintf(stderr,"snptguar io error:");

  fprintf(stderr,fmt,
	 a1,a2,a3,a4,a6,a7,a8,a9,a10,
	 b1,b2,b3,b4,b6,b7,b8,b9,b10,
	 c1,c2,c3,c4,c6,c7,c8,c9,c10);
  if(errno!=0)
     fprintf(stderr,"\nlast errno=%d '%s'\n",errno,errno_to_text(errno));
  fflush(stderr);
  set_gerror(GASP_AUTH_FORMAT,"snptguar");
  longjmp(SETJM guarcatch,1);
}

void rl_parerr(parblk,
	       fmt,
	 a1,a2,a3,a4,a6,a7,a8,a9,a10,
	 b1,b2,b3,b4,b6,b7,b8,b9,b10,
	 c1,c2,c3,c4,c6,c7,c8,c9,c10)
char *fmt;
PFM_pt parblk;
long a1,a2,a3,a4,a6,a7,a8,a9,a10;
long b1,b2,b3,b4,b6,b7,b8,b9,b10;
long c1,c2,c3,c4,c6,c7,c8,c9,c10;
{
  rl_error(fmt,
	 a1,a2,a3,a4,a6,a7,a8,a9,a10,
	 b1,b2,b3,b4,b6,b7,b8,b9,b10,
	 c1,c2,c3,c4,c6,c7,c8,c9,c10);
}
