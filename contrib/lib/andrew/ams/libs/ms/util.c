/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/util.c,v 2.14 1991/09/12 15:49:41 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/util.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/util.c,v 2.14 1991/09/12 15:49:41 bobg Exp $ ";
#endif /* lint */

#include <ms.h>
#include <andrewos.h> /* sys/file.h */
#include <mailconf.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

extern char *getprofile(), *index(), *StripWhiteEnds();
extern char home[], MyMailDomain[];

extern Boolean DidInit;
extern char *SearchPath;

/* The following should have home prepended to it whenever used */
char MAILSEARCHPATHTEMPLATE[]="/.MESSAGES";

static char OTHERSEARCHPATHTEMPLATE[]="$LOCAL:$EXTERNAL"; /* strictly historical */

static char DEFAULTMARKER[] = "$default";
static char MAILMARKER[] = "$mail";
static char LOCALMARKER[] = "$local";
static char EXTERNALMARKER[] = "$external";
static char OTHERMARKER[] = "$other";
static char OFFICIALMARKER[] = "$official";
#define MARKERINIT '$'
/* MARKERINIT is the first char in the xxxMARKER values */

InitializeSearchPaths() {
    char   *s, *t, ErrorText[256], MailRoot[1+MAXPATHLEN];
    int whichpath, NeedMailDir;

    DidInit = TRUE;

    NeedMailDir = FALSE;
    strcpy(MailRoot, home);
    strcat(MailRoot, MAILSEARCHPATHTEMPLATE);
    SearchPath = getprofile("mspath");
    if (SearchPath == NULL) {
	SearchPath = malloc(1+strlen(DEFAULTSEARCHPATHTEMPLATE));
	if (SearchPath == NULL) {
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	}
	strcpy(SearchPath, DEFAULTSEARCHPATHTEMPLATE);
    } else {
	t = malloc(1+strlen(SearchPath));
	if (!t) {
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	}
	strcpy(t, SearchPath);
	SearchPath = t;
    }
    while ((s = index(SearchPath, MARKERINIT)) != NULL) {
	if (!lc2strncmp(DEFAULTMARKER, s, sizeof(DEFAULTMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + strlen(DEFAULTSEARCHPATHTEMPLATE) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(DEFAULTMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s", SearchPath, DEFAULTSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    continue;
	}
	if (!lc2strncmp(MAILMARKER, s, sizeof(MAILMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + strlen(MAILSEARCHPATHTEMPLATE) + strlen(home) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(MAILMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s%s", SearchPath, home, MAILSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    NeedMailDir = 1;
	    continue;
	}
	if (!lc2strncmp(OTHERMARKER, s, sizeof(OTHERMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + sizeof(OTHERSEARCHPATHTEMPLATE) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(OTHERMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s", SearchPath, OTHERSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    continue;
	}
	if (!lc2strncmp(LOCALMARKER, s, sizeof(LOCALMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + strlen(LOCALSEARCHPATHTEMPLATE) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(LOCALMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s", SearchPath, LOCALSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    continue;
	}
	if (!lc2strncmp(EXTERNALMARKER, s, sizeof(EXTERNALMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + strlen(EXTERNALSEARCHPATHTEMPLATE) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(EXTERNALMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s", SearchPath, EXTERNALSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    continue;
	}
	if (!lc2strncmp(OFFICIALMARKER, s, sizeof(OFFICIALMARKER)-1)) {
	    t = malloc(strlen(SearchPath) + strlen(OFFICIALSEARCHPATHTEMPLATE) + 10);
	    if (t == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DISAMB);
	    }
	    *s = '\0';		/* Now SearchPath points to first part only */
	    s += (sizeof(OFFICIALMARKER)-1);
				/* Now s points to last part only */
	    sprintf(t, "%s%s%s", SearchPath, OFFICIALSEARCHPATHTEMPLATE, s);
	    free(SearchPath);
	    SearchPath = t;
	    continue;
	}
	t = index(s, ':');
	if (t)
	    *t = '\0';
	sprintf(ErrorText, "The mspath variable %s is unknown", s);
	NonfatalBizarreError(ErrorText);
	AMS_RETURN_ERRCODE(EMSNOSUCHVAR, EIN_MSPATHCHECK, EVIA_INITSEARCHPATHS);
    }

    debug(16,("Search path is %s\n", SearchPath));
    MS_NumDirsInSearchPath = 0;
    for (s = SearchPath; *s != '\0' && MS_NumDirsInSearchPath < MAXPATHELTS;) {
	SearchPathElements[MS_NumDirsInSearchPath].HasMailbox = SearchPathElements[MS_NumDirsInSearchPath].HasValidated = 0;
	SearchPathElements[MS_NumDirsInSearchPath].Path = SearchPathElements[MS_NumDirsInSearchPath].Cell = NULL;
	SearchPathElements[MS_NumDirsInSearchPath].label = NULL;
	for (t = s; *t && *t != ':'; ++t) {
	    ;
	}
	if (*t) {
	    *t++ = '\0';
	}
	s = StripWhiteEnds(s);
	if (!*s) {
	    /* empty path element */
	    NonfatalBizarreError("Your mspath has an empty element in it which I am ignoring.");
	    s = t;
	    continue;
	}
	if (*s == '[') {
	    char *prefix = ++s;
	    char *futurestuff;

	    s = index(s, ']');
	    if (!s) {
		NonfatalBizarreError("Square brackets in mspath preference do not match.");
		AMS_RETURN_ERRCODE(EMSSYNTAX, EIN_MSPATHCHECK, EVIA_INITSEARCHPATHS);
	    }
	    *s++ = '\0';
	    if (*prefix == '*') {
		++prefix;
		SearchPathElements[MS_NumDirsInSearchPath].HasMailbox = 1;
	    }
	    futurestuff = index(prefix, ':');
	    if (futurestuff) *futurestuff = NULL; /* Future expansion can annotate the names, following a colon, with some kinds of options */
	    if (prefix && *prefix) {
		SearchPathElements[MS_NumDirsInSearchPath].label = malloc(1+strlen(prefix));
		if (SearchPathElements[MS_NumDirsInSearchPath].label == NULL) {
		    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_INITSEARCHPATHS);
		}
		strcpy(SearchPathElements[MS_NumDirsInSearchPath].label, prefix);
	    }
	    s = StripWhiteEnds(s);
	}
	mserrcode = ResolveTildes(s, &SearchPathElements[MS_NumDirsInSearchPath].Path, MyMailDomain);
	for (whichpath=0; whichpath < MS_NumDirsInSearchPath; ++whichpath) {
	    if (strcmp(SearchPathElements[MS_NumDirsInSearchPath].Path, SearchPathElements[whichpath].Path) == 0) {
		/* duplicate path element */
		char ErrorText[100+MAXPATHLEN];
		sprintf(ErrorText, "Ignoring duplicate mspath element: %s.", s);
		NonfatalBizarreError(ErrorText);
		s = t;
		continue;
	    }
	}
	if (mserrcode) {
	    sprintf(ErrorText, "The mspath element %s is unknown (%d)", s, AMS_ERRNO);
	    NonfatalBizarreError(ErrorText);
	    AMS_RETURN_ERRCODE(ENOENT, EIN_MSPATHCHECK, EVIA_INITSEARCHPATHS);
	}
	CheckLabel(MS_NumDirsInSearchPath);
	SearchPathElements[MS_NumDirsInSearchPath].HasValidated = 0;
	if (strcmp(MailRoot, SearchPathElements[MS_NumDirsInSearchPath].Path) == 0) {
	    NeedMailDir = 1;
	}
	s = t;
	++MS_NumDirsInSearchPath;
    }
    if (NeedMailDir != 0) {
	if (EnsureMailDirExists()) {
	    return(mserrcode);
	}
    }
    return(0);
}

static int CheckLabel(i)
int i;
{
    if (!SearchPathElements[i].label) {
	char FBuf[1+MAXPATHLEN];
	if (!strcmp(SearchPathElements[i].Path, EXTERNALSEARCHPATHTEMPLATE)) {
	    SearchPathElements[i].label = "external";
	    return 0;
	}
	if (!strcmp(SearchPathElements[i].Path, OFFICIALSEARCHPATHTEMPLATE)) {
	    SearchPathElements[i].label = "official";
	    return 0;
	}
	if (!strcmp(SearchPathElements[i].Path, LOCALSEARCHPATHTEMPLATE)) {
	    SearchPathElements[i].label = "local";
	    return 0;
	}
	sprintf(FBuf, "%s%s", home, MAILSEARCHPATHTEMPLATE);
	if (!strcmp(SearchPathElements[i].Path, FBuf)) {
	    SearchPathElements[i].label = "mail";
	    return 0;
	}
    }
    return 0;
}

OKRoot(Name)
char *Name;
{/* Check (mspath) root Name.  Return TRUE if all was OK, or FALSE, after having printed a message, if there was a problem. */
    char ErrorText[100+MAXPATHLEN];
    int Authenticated, saveerr;

    if (access(Name, R_OK)) {
	saveerr = errno;
	mserrcode = MS_CheckAuthentication(&Authenticated);
	if (vdown(saveerr) || vdown(AMS_ERRNO)) {
	    sprintf(ErrorText, "AFS/network error checking %s; continuing...", ap_Shorten(Name));
	} else if (!Authenticated) {
	    sprintf(ErrorText, "You are apparently unauthenticated and cannot access mspath element %s.", ap_Shorten(Name));
	} else if (saveerr == ENOENT) {
	    sprintf(ErrorText, "The mspath element %s does not exist.", ap_Shorten(Name));
	} else if (saveerr == EACCES) {
	    sprintf(ErrorText, "The mspath element %s is protected so that you can not read it.", ap_Shorten(Name));
	} else {
	    sprintf(ErrorText, "Unexpected error %d checking path entry %s; hoping for the best...", saveerr, ap_Shorten(Name));
	}
	NonfatalBizarreError(ErrorText);
	return (FALSE);
    } else {
	return (TRUE);
    }
}

ValidateSearchPath(i)
int i;
{
    if (i<0 || i>= MS_NumDirsInSearchPath) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_DISAMB);
    }
    SearchPathElements[i].HasValidated = OKRoot(SearchPathElements[i].Path);
    return(0);
}
