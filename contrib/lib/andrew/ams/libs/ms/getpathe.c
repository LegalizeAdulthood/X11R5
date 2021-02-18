/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getpathe_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getpathe.c,v 2.5 1991/09/12 15:44:15 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getpathe.c,v 2.5 1991/09/12 15:44:15 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getpathe.c,v $
*/
#include <ms.h>
#include <mailconf.h>

extern char home[];
extern Boolean DidInit;

MS_GetSearchPathEntry(which, buf, lim)
int     which,
        lim;
char   *buf;
{
    int     i;

    if (!DidInit) {		/* First time-only -- initialization
				   section */
	if ((i = InitializeSearchPaths()) != 0)
	    return(i);
    }
    switch(which) {
	case AMS_MAILPATH:
	    /* Wants *mail* directory */
	    strncpy(buf, home, lim);
	    strncat(buf, MAILSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_OFFICIALPATH:
	    strncpy(buf, OFFICIALSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_LOCALPATH:
	    strncpy(buf, LOCALSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_EXTERNALPATH:
	    strncpy(buf, EXTERNALSEARCHPATHTEMPLATE, lim);
	    break;
	default:
	    if (which < 0 || which >= MS_NumDirsInSearchPath) {
		AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETSEARCHPATHENTRY);
	    }
	    if (strlen(SearchPathElements[which].Path) > lim) {
		AMS_RETURN_ERRCODE(ERANGE, EIN_PARAMCHECK, EVIA_GETSEARCHPATHENTRY);
	    }
	    strncpy(buf, SearchPathElements[which].Path, lim);
	    break;
    }
    return(0);
}
