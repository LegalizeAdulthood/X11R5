/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	lcstring.c-- copies a source string to a dest buff, converting all uppercase to lower.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/lcstring.c,v 1.2 1991/09/12 17:27:35 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/lcstring.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/lcstring.c,v 1.2 1991/09/12 17:27:35 bobg Exp $";
#endif /* lint */

#include <ctype.h>

#define DOWNCASE(x) (isascii(x) && isalpha(x) && isupper(x) ? (tolower(x)) : (x))
/* 
 * Just like strncpy but shift-case in transit and forces null termination.  
 * Copied 8/24/89 from afs/util/casestrcpy.c to allow omission of 
 * lib/afs/util.a; NOTE: on the sun4c version of AFS we need 
 * lib/afs/util.a so this function is going into its very own module.
 * (4/4/91)
*/

char *lcstring (d, s, n)
  char *d;				/* dest string */
  char *s;				/* source string */
  int   n;				/* max transfer size */
{   char *original_d = d;
    char  c;

    if ((s == 0) || (d == 0)) return 0;	/* just to be safe */
    while (n) {
	c = *s++;
	c = DOWNCASE(c);
	*d++ = c;
	if (c == 0) break;		/* quit after transferring null */
	if (--n == 0) *(d-1) = 0;	/* make sure null terminated */
    }
    return original_d;
}

#undef DOWNCASE

#ifdef TESTINGONLYTESTING
#include <stdio.h>
main(argc,argv)
int argc;
char *argv[];
{
char *str1, str2;
int len;

  switch(argc) {
  case 3:
    len = argv[3];
    str1 = strcpy(str1,(char*)malloc(strlen(argv[1]) + len + 1));
    str2 = strcpy(str2,(char*)malloc(strlen(argv[2]) + 1));
    printf("str1 %s\nstr2 %s\n", str1, str2);
    result = lcstring(str1, str2, len);
    break;
    fprintf(stderr, "usage: lcstring s1 s2 [n].\n");
    exit(1);
  }
  printf("new str1: %s", str1);
}
#endif /* TESTINGONLYTESTING */
