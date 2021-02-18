/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/profile.c,v 2.11 1991/09/12 17:28:27 bobg Exp $ */
/* $ACIS:profile.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/profile.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/profile.c,v 2.11 1991/09/12 17:28:27 bobg Exp $";
#endif /* lint */

#include <fdplumb.h>
#include <stdio.h>
#include <andrewos.h>		/* sys/types.h */
#include <sys/stat.h>
#include <sys/param.h>	/* For MAXPATHLEN */
#include <pwd.h>
#include <ctype.h>
#include <util.h>

#define DEFAULTPROFILES "~/preferences:~/.preferences:~/.Xdefaults"
#define GLOBALPROFILE AndrewDir("/lib/global.prf")

static struct configurelist *profileHead = NULL;
static struct configurelist *GloprofileHead = NULL;

static  int inited = 0;  /* Used to be static local to openprofile -- nsb */
static char *profileFileName = NULL;
static char *firstProfileFileName = NULL;

#define openglobalprofile() ((struct configurelist *) ReadConfigureFile(GLOBALPROFILE))

static struct configurelist *openprofile (filename, defaultname)
    char *filename;
    char *defaultname;
{
    char *pl=(char *) getenv(filename);
    char *home=(char *) gethome(NULL);
    char *sep;
    int homelen=(home==NULL ? 0 : strlen(home));
    struct configurelist *cl;
    char tmpFileName[MAXPATHLEN];

    if(pl==NULL || *pl=='\0')
        pl=defaultname;

    do{
	char *name;
	int namelen;

	sep=(char *)index(pl,':');

	name = pl;

	if(sep!=NULL){
	    namelen = sep - pl;
	    pl=sep+1;
	}
	else
	    namelen=strlen(name);

	if(*name=='~' && (name++,namelen--,homelen>0)){
	    strcpy(tmpFileName,home);
	    strncat(tmpFileName,name, namelen);
	    tmpFileName[namelen + homelen] = '\0';
	}
	else {
	    strncpy(tmpFileName,name, namelen);
	    tmpFileName[namelen] = '\0';
	}

	if (firstProfileFileName == NULL)  {
	    firstProfileFileName = (char *) malloc(strlen(tmpFileName) + 1);
	    strcpy(firstProfileFileName, tmpFileName);
	}

	if ((cl = (struct configurelist *) ReadConfigureFile(tmpFileName)) != NULL)  {
	    if (profileFileName != NULL)  {
		free(profileFileName);
	    }
	    profileFileName = (char *) malloc(strlen(tmpFileName) + 1);
	    strcpy(profileFileName, tmpFileName);
	    return cl;
	}

    } while(sep!=NULL);

    return NULL;

}

char *GetProfileFileName()
{
    if (! inited)  {
	profileHead = openprofile("PROFILES", DEFAULTPROFILES);
	GloprofileHead = openprofile("GLOBALPROFILES", GLOBALPROFILE);
	inited = 1;
    }

    return profileFileName;
}

char *GetFirstProfileFileName()
{
    if (! inited)  {
	profileHead = openprofile("PROFILES", DEFAULTPROFILES);
	GloprofileHead = openprofile("GLOBALPROFILES", GLOBALPROFILE);
	inited = 1;
    }

    return firstProfileFileName;
}

refreshprofile() {  /* Force rereading */
    if (profileHead != NULL)  {
	FreeConfigureList(profileHead);
	profileHead = NULL;
    }
    if (profileFileName != NULL)  {
	free(profileFileName);
	profileFileName = NULL;
    }
    inited = 0;
}

char *getprofile (var)
char *var; {
    char *retval;
    if (! inited)  {
	profileHead = openprofile("PROFILES", DEFAULTPROFILES);
	GloprofileHead = openprofile("GLOBALPROFILES", GLOBALPROFILE);
	inited = 1;
    }
#ifdef GLOBALPREFERENCE
/* check for exact match in users profile */
    if((retval = (char *) GetConfig(profileHead, var, 0)) != NULL)
	return retval;
/* check for exact match in global profile */
    if((retval = (char *) GetConfig(GloprofileHead, var, 0)) != NULL)
	return retval;
#endif
/* check for unexact match in users profile */
    if((retval = (char *) GetConfig(profileHead, var, 1)) != NULL)
	return retval;
/* check for unexact match in global profile */
    return (char *) GetConfig(GloprofileHead, var, 1) ;
}

getprofileswitch (var, DefaultValue)
char   *var; {
    char   *val;
    register    len;
    static struct keys {
	char   *name;
	int     value;
    }                   keys[] = {
	                    "true", 1,
	                    "false", 0,
	                    "on", 1,
	                    "off", 0,
	                    "yes", 1,
	                    "no", 0,
	                    "1", 1,
	                    "0", 0,
	                    0, 0
    };
    register struct keys   *p;
    if (var && (val = getprofile (var))) {
	len = strlen (val);
	for (p = keys; p -> name; p++)
	    if (p -> name[0] == val[0] && strncmp (p -> name, val, len) == 0)  {
		return p -> value;
	    }
    }
    return DefaultValue;
}

getprofileint (var, DefaultValue)
char   *var; {
    register char  *val;
    register    n = 0;
    register    neg = 0;

    if (var == 0 || (val = getprofile(var)) == 0)  {
	return DefaultValue;
    }
    while (*val) {
	if (*val == '-')
	    neg = ~neg;
	else
	    if (*val != ' ' && *val != '\t')
		if ('0' <= *val && *val <= '9')
		    n = n * 10 + *val - '0';
		else  {
		    return DefaultValue;
		}
	val++;
    }
    return neg ? -n : n;
}

profileentryexists(var, usedefault)
    char *var;
    int usedefault;
{

    if (! inited)  {
	profileHead = openprofile("PROFILES", DEFAULTPROFILES);
	GloprofileHead = openglobalprofile();
	inited = 1;
    }

    return (var != NULL && ( (GetConfig(profileHead, var, usedefault) != NULL) || 
			     (GetConfig(GloprofileHead, var, usedefault) != NULL)));
}
