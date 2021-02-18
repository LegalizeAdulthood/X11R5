/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	gtvpwnam.c
	getvpwnam(vuid) is like getpwnam(nam), but for Vice IDs.
\* ************************************************************ */
static char RCSid[]="$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/gtvpwnam.c,v 2.6 1991/09/12 17:27:14 bobg Exp $";

#include <andyenv.h>
#include <andrewos.h> /* syslog.h */
#include <stdio.h>
#include <errno.h>
#include <util.h>
#ifdef WHITEPAGES_ENV
#include <pwd.h>
#include <wp.h>
#endif /* WHITEPAGES_ENV */
#include <svcconf.h>
#define NULL 0
extern int errno;

#ifdef WHITEPAGES_ENV
struct passwd *getvpwnam(vnam)
char *vnam;
{/* Return a struct passwd for vuid, a Vice pw_nam */
    wp_ErrorCode Res;
    wp_PrimeKey KVal;
    struct passwd *RV;
    extern struct passwd *_pw_getvpwkey();

    CheckServiceConfiguration();
    if (AMS_UseWP) {
	if (_pw_OpenWP() == 0) {
		errno = ENXIO;
		return NULL;
	}
	Res = wp_GetUIDOnly(vnam, &KVal);
	if (Res != wperr_NoError) {_pw_CloseWP(); errno = ETIMEDOUT; return NULL;}
	RV = _pw_getvpwkey(KVal);
	free(KVal);
	_pw_CloseWP();
	return RV;
    } else {
	return(getpwnam(vnam));
    }
}
#endif /* WHITEPAGES_ENV */

