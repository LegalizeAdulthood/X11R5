/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	sys.c		
	Print name that describes current machine and operating
	system.
*/

#include <andrewos.h>

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/sys/RCS/sys.c,v 2.10 1991/09/12 17:22:28 bobg Exp $";


#define NULL (char *)0

main (argc,argv)
int argc;
char **argv;
{
  char retval[1000], *s;

  strcpy(retval, SYS_NAME);

  while (*++argv!=NULL)
    if (**argv=='-' && (*argv)[1]=='c' && (*argv)[2]=='\0') {
      if ((s=index(retval, '_')) != NULL)
	*s = '\0';
      break;
    }
  printf("%s\n", retval);
  exit(0); /* Don't return bogoid status... -zs */
}

