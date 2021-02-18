/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	ulstrcmp.c--compare strings ignoring alphabetic case.
*/

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstrcmp.c,v 2.9 1991/09/12 17:29:59 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstrcmp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/ulstrcmp.c,v 2.9 1991/09/12 17:29:59 bobg Exp $";
#endif /* lint */

#include <ctype.h>


int ULstrcmp(s1, s2)
register char *s1, *s2;
{
    /* case INSENSITIVE:  Compare strings:  s1>s2: >0  s1==s2: 0  s1<s2: <0
	  */
    register char c1,c2;

    for(;;) {
	c1 = *s1++; if (c1 <= 'Z') if (c1 >= 'A') c1 += 040;
	c2 = *s2++; if (c2 <= 'Z') if (c2 >= 'A') c2 += 040;
	if (c1 != c2) break;
	if (c1 == '\0') return(0);
    }
    return(c1 - c2);
}

#define DOWNCASE(x) (isascii(x) && isalpha(x) && isupper(x) ? (tolower(x)) : (x) )

int ULstrncmp(s1,s2,n)
char *s1, *s2;
int n;
{
  /* case INSENSITIVE:  Compare strings, up to n chars:  
     s1>s2: >0  s1==s2: 0  s1<s2: <0
   */

  register int i;
  register int result = 0;

  for(i = 0;(s1[i] || s2[i]) && i<n && !result;++i){
    result = DOWNCASE(s1[i]) - DOWNCASE(s2[i]);
  }
  return(result);
}

#undef DOWNCASE

#ifdef TESTINGONLYTESTING
#include <stdio.h>
main(argc,argv)
int argc;
char *argv[];
{
  int result = 0;

  switch(argc) {
  case 3:
    result = ULstrcmp(argv[1], argv[2]);
    break;
  case 4:
    result = ULstrncmp(argv[1], argv[2], atoi(argv[3]));
    break;
  default:
    fprintf(stderr, "usage: ulstrcmp s1 s2 [n].\n");
    exit(1);
  }
  printf("'%s' %s '%s'.\n",
	 argv[1], (result == 0)?"==":(result<0)?"<":">", argv[2]);
}
#endif /* TESTINGONLYTESTING */
