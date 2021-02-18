/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *restilde_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/restilde.c,v 2.8 1991/09/12 15:47:57 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/restilde.c,v 2.8 1991/09/12 15:47:57 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/restilde.c,v $
*/
#include <util.h>
#include <ms.h>

#define MAXUSERNAME 100		/* Biggest length of a user name */

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char home[], *StripWhiteEnds(), *FindUserDir();
extern char MyMailDomain[];

/* If you make any changes to this routine, check out TildeResolve in
MessagesBE.c as well. */

/* This routine takes a path name and resolves leading ~ references, e.g.
	~/foo and ~nsb/bar, returning a freshly malloced version */

ResolveTildes(old, new, domain)
char   *old,
      **new, *domain;
{
    char   *t, user[2*MAXUSERNAME], *udir;
    struct CellAuth *ca;
    int Res;

    debug(1,("ResolveTildes %s\n", old));
    old = StripWhiteEnds(old);
    if (*old != '~') {
	*new = NewString(old);
	if (*new == NULL) {
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	}
    } else {
	if (*++old == '/') {
	    if (domain == NULL || *domain == '\0' || ULstrcmp(domain, MyMailDomain) == 0) {
		udir = home;
	    } else {
		ca = NULL;
		Res = FindCell(domain, &ca);
		if (ca == NULL) {
		    if (Res < 0) Res = ETIMEDOUT;
		    else Res = EMSUNAUTH;
		    AMS_RETURN_ERRCODE(Res, EIN_DISAMB, EVIA_DISAMB);
		}
		FillInCell(ca);
		if (ca->homeDir == NULL) AMS_RETURN_ERRCODE(EMSNOHOME, EIN_DISAMB, EVIA_DISAMB);
		udir = ca->homeDir;
	    }
	    *new = malloc(strlen(old) + strlen(udir) + 2);
	    if (*new == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    sprintf(*new, "%s/%s", udir, ++old);
	} else {
	    for (t = user; *old && *old != '/'; ++old, ++t) {
		*t = *old;
	    }
	    *t = '\0';
	    if (*old)
		++old;
	    if (domain == NULL || *domain == '\0') domain = MyMailDomain;
#ifdef WHITEPAGES_ENV
	    cpw_error = 0;
#endif /* WHITEPAGES_ENV */
	    udir = FindUserDir(user, domain);
	    if (udir == (char *) - 1) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_GETPWNAM, EVIA_DISAMB);
	    }
	    if (udir == NULL) {
#ifdef WHITEPAGES_ENV
		AMS_RETURN_ERRCODE(ConvertWpErrToMSErr(cpw_error, EMSNOSUCHUSER, TRUE), EIN_GETPWNAM, EVIA_DISAMB);
#else /* WHITEPAGES_ENV */
		AMS_RETURN_ERRCODE(EMSNOSUCHUSER, EIN_GETPWNAM, EVIA_DISAMB);
#endif /* WHITEPAGES_ENV */
	    }
	    *new = malloc(strlen(old) + strlen(udir) + 2);
	    if (*new == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    sprintf(*new, "%s/%s", udir, old);
	}
    }
    return(0);

}

