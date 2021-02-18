/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *tildes_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/tildes.c,v 2.7 1991/09/12 15:50:08 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/tildes.c,v 2.7 1991/09/12 15:50:08 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/shr/RCS/tildes.c,v $
*/
#include <stdio.h>
#include <pwd.h>
#include <andyenv.h>
#include <util.h>
#ifdef WHITEPAGES_ENV
#include <wp.h>
#endif /* #ifdef WHITEPAGES_ENV */

extern char *malloc();

/* The purpose of this routine is simply to minimize the number of times
	we call the getpwnam routine, because that routine seems to be
	both very slow AND seems to encourage fragmentation of memory
	by mallocing huge areas of memory for the password file.

   The basic idea is very simple:  We keep a cache of every name we've
	looked up before, and we don't go to the password file if we can
	get it from the cache.  Currently the cache is searched linearly;
	so far I have not known anyone to use more than 5 or 6 different
	tilde references in a session.  The real problem is that the
	same ones get made over and over, especially by the bboard daemon.
*/

static struct udircache {
    char *user, *ucell, *udir;
    struct udircache *next;
} *Udc = NULL;

char *
FindUserDir(user, cellname)
char *user, *cellname;
{
    struct udircache *u;
    struct passwd *pw;

    for (u = Udc; u; u = u->next) {
	if (strcmp(u->user, user) == 0 && ULstrcmp(cellname, u->ucell) == 0) break;
    }
    if (u) {
	return(u->udir);
    }

    pw=getcpwnam(user, cellname);
    if (!pw) {
	return((char *) NULL);
    }
    u = (struct udircache *) malloc(sizeof(struct udircache));
    if (!u) return((char *) -1);
    u->user = NewString(user);
    u->udir = NewString(pw->pw_dir);
    if (!u->user || !u->udir) {
	if (u->user != NULL) free(u->user);
	if (u->udir != NULL) free(u->udir);
	free(u);
	return((char *) -1);
    }
    u->ucell = NewString(cellname);
    if (u->ucell == NULL) {
	free(u->user); free(u->udir); free(u);
	return((char *) -1);
    }
    u->next = Udc;
    Udc = u;
    return(u->udir);
}
