/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gethome.c,v 2.6 1991/09/12 17:26:04 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gethome.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <util.h>

#define NULL 0

extern char *getenv();

extern int errno;

static char home[100]="";

char *gethome(name)
char *name;
{
    struct passwd *pw=NULL;
    char *h=NULL;

    if (name==NULL) {
	if(home[0]!='\0')
	    return home;

	h=getenv("HOME");

	/* Do not trust env vbl HOME if it is "/" because we
	    may be spawned by the guardian -- nsb */
	if(h==NULL || strcmp(h,"/")==0) {
	    pw = getpwuid(getuid());
	    if (pw != NULL) h = pw->pw_dir;
	}
	if (h != NULL) strncpy(home, h, sizeof(home));
    } else {
	errno = 0;
	pw=getpwnam(name);
	if (pw != NULL) h = pw->pw_dir;
    }

    return h;
}
#ifdef TESTINGONLYTESTING
main()
{
    char Buf[500], *Ques, *Ans;

    for (;;) {
	printf("home dir of: "); fflush(stdout);
	if (gets(Buf) == NULL) exit(0);
	Ques = Buf[0] == '\0' ? NULL : Buf;
	Ans = gethome(Ques);
	printf("gethome(\"%s\") returns ``%s''.\n",
	       Ques == NULL ? "NULL" : Ques,
	       Ans == NULL ? "NULL" : Ans);
    }
}
#endif /* TESTINGONLYTESTING */
