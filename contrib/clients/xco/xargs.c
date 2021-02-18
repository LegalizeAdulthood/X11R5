/*
 * xargs.c	Routines to parse X11 arguments on command line and in
 *		resource manager database string.
 *
 * author	jtk, 2/89
 *
 * use		Call InitXOption for each option in your program.  One
 *		option must be "-display", since it is needed to get
 *		the Resource Manager database string from the server.
 *
 *		Call ParseXArgs to actually do the parsing.
 *
 * warning	These routines are very simple, and probably only useful
 *		in their current form for trivial arguments.
 */
 
/***********************************************************************
* Copyright 1990 by Purdue University, J. Tim Korb, and Richard Bingle.
* All rights reserved.  Some individual files may be covered by other 
* copyrights.
* 
* Redistribution and use in source and binary forms are permitted
* provided that this entire copyright notice is duplicated in all such
* copies, and that any documentation, announcements, and other
* materials related to such distribution and use acknowledge that the
* software was developed at Purdue University, W. Lafayette, IN.  
* Neither the name of the University nor the name of the authors may 
* be used to endorse or promote products derived from this material without 
* specific prior written permission.  
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
* MERCHANTIBILITY AND FITNESS FOR ANY PARTICULAR PURPOSE.
************************************************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xos.h>
#include <pwd.h>

extern char *malloc();
extern char *getenv();
char *GetHome();

#define copMax 100	/* maximum number of options */
#define cchMax 200	/* maximum length of various strings */

XrmOptionDescRec rgod[copMax];	/* list of options to look for */
int copMac = 0;		/* number of options to look for */
int iopDisplay = -1;	/* option index of "-display" */
int iopName = -1;	/* option index of "-name" */

static struct or {	/* option record */
    char *sbName;	/* name of option */
    char *sbClass;	/* class of option */
    char *sbDef; 	/* default value if option not given */
    char *sbHelp;	/* help string for option */
    char **psb;		/* pointer to variable to receive value */
    } rgor[copMax];


/*
 * InitXOption(sbOpt, psb, sbDef, sbHelp, sbRm, sbName, sbClass) -- set
 * up for command line and resource string parsing of arguments.
 *
 * sbOpt	option name (eg, "-display")
 * psb		place to store option after parsed (eg, &sbDisplay)
 * sbDef	default option value (eg, "")
 * sbHelp	message to display for help (eg, "display to use")
 * sbRm		resource manager command line database string (eg, "Display")
 * sbName	resource manager name of option (eg, "display")
 * sbClass	resource manager class of option (eg, "Display")
 */
InitXOption(sbOpt, psb, sbDef, sbHelp, sbRm, sbName, sbClass)
char *sbOpt, **psb, *sbDef, *sbHelp, *sbRm, *sbName, *sbClass;
{
    int i;

    i = copMac++;

    if (strcmp(sbOpt, "-display") == 0)
	iopDisplay = i;
    else if (strcmp(sbOpt, "-name") == 0)
	iopName = i;

    rgod[i].option = sbOpt;
    rgod[i].specifier = sbRm;
    if (strcmp(sbDef, "on") == 0) {
	rgod[i].argKind = XrmoptionNoArg;
	rgod[i].value = "off";
	}
    else if (strcmp(sbDef, "off") == 0) {
	rgod[i].argKind = XrmoptionNoArg;
	rgod[i].value = "on";
	}
    else {
	rgod[i].argKind = XrmoptionSepArg;
	rgod[i].value = (caddr_t) NULL;
	}

    rgor[i].sbName = sbName;
    rgor[i].sbClass = sbClass;
    rgor[i].sbDef = sbDef;
    rgor[i].sbHelp = sbHelp;
    rgor[i].psb = psb;

    *psb = sbDef;	/* just in case no one else assign's it a value */
}


/*
 * ParseXArgs(sbProgName, sbProgClass, pcsb, rgsb) -- parse the arguments
 * on the command line, looking for ones given by previous InitXOption
 * calls.  Return a pointer to the display.
 */
Display *ParseXArgs(sbProgName, sbProgClass, pcsb, rgsb)
char *sbProgName, *sbProgClass;
int *pcsb;
char *rgsb[];
{
    Display *pdp;
    char *sbResMgr;
    char *sbHome;
    XrmDatabase db, dbCmd = NULL;
    int i;

    if (iopDisplay == -1)
	fprintf(stderr, "%s: No display option listed!\n", rgsb[0]), exit(1);

    if (iopName != -1) {  /* if -name given, override program name */
	for (i = 1; i < *pcsb; i++)
	    if (strcmp(rgsb[i], "-name") == 0) {
		if (i+1 >= *pcsb)
		    fprintf(stderr, "%s: -name requires name argument\n", 
			rgsb[0]), exit(1);
		rgsb[0] = sbProgName = rgsb[i+1];
		}
	}

    XrmInitialize();
    XrmPutLineResource(&dbCmd, "*Display:");
    XrmParseCommand(&dbCmd, rgod, copMac, sbProgName, pcsb, rgsb);

    FindValue(dbCmd, iopDisplay, sbProgName, sbProgClass);
    pdp = XOpenDisplay(*rgor[iopDisplay].psb);
    if (pdp == NULL)
	fprintf(stderr, "%s: Could not open display\n", rgsb[0]), exit(1);

    sbResMgr = XResourceManagerString(pdp);
    if (sbResMgr != NULL) {
    	db = XrmGetStringDatabase(sbResMgr);
    }
    else {
	sbHome = GetHome();
	strcat(sbHome, "/.Xdefaults");
	db = XrmGetFileDatabase(sbHome);
    }

    XrmMergeDatabases(dbCmd, &db);
    for (i = 0; i < copMac; i++)
	FindValue(db, i, sbProgName, sbProgClass);

    return pdp;
}


/*
 * NewString(sb) -- utility routine to malloc and initialize a string.
 */
char *NewString(sb)
char *sb;
{
    char *sbNew = (char *) malloc((unsigned) strlen(sb)+1);
    if (sbNew == NULL)
	fprintf(stderr, "Could not malloc\n"), exit(1);
    strcpy(sbNew, sb);
    return sbNew;
}


/*
 * FindValue(db, i, sbProgName, sbProgClass) -- lookup option i in the
 * database (typically the merger of the command line and resource manager).
 */
FindValue(db, i, sbProgName, sbProgClass)
XrmDatabase db;
int i;
char *sbProgName, *sbProgClass;
{
    char sbNameKey[cchMax], sbClassKey[cchMax];
    XrmValue rv;
    char *sbType;
    char sb[cchMax];

    strcpy(sbNameKey, sbProgName); strcat(sbNameKey, ".");
    strcat(sbNameKey, rgor[i].sbName);
    strcpy(sbClassKey, sbProgClass); strcat(sbClassKey, ".");
    strcat(sbClassKey, rgor[i].sbClass);
    if (XrmGetResource(db, sbNameKey, sbClassKey, &sbType, &rv)) {
	if (strcmp(sbType, "String") != 0 || rv.size >= cchMax)
	    fprintf(stderr, "bad resource type or size\n"), exit(1);
	strncpy(sb, rv.addr, (int) rv.size);
	sb[rv.size] = '\0';
	*rgor[i].psb = NewString(sb);
	}
}


/*
 * PrintOptions() -- generates a help message if the argument parsing
 * fails.  
 */
PrintOptions()
{
    int i;
    struct or *por;

    fprintf(stderr, "Valid options are ([default]):\n");
    for (i = 0; i < copMac; i++) {
	por = &rgor[i];
	fprintf(stderr, "\t%-20s %s [%s]\n",
		rgod[i].option, por->sbHelp, por->sbDef);
	}
}

/*
 * GetHome() -- returns a string containing the user's home directory (plus
 * some extra space)
 */
char *GetHome()
{
    char *sb;
    char *sbTmp;
    int iUid;
    struct passwd *ppw;

/* first see if they have a HOME environment variable, if so, we're done */

    sb = malloc(cchMax);
    sbTmp = getenv("HOME");
    if (sbTmp) {
	strcpy(sb, sbTmp);
	return(sb);
    }

/* if not, get the user name from the USER environment variable (if it's */
/* there) or get the uid.  Either way, get the passwd struct and get the */
/* home directory from there.                                            */

    sbTmp = getenv("USER");
    if (sbTmp) {
	ppw = getpwnam(sbTmp);
    }
    else {
	iUid = getuid();
        ppw = getpwuid(iUid);
    }
    if (ppw)
	strcpy(sb, ppw->pw_dir);
    else
	strcpy(sb, "");

    return(sb);
}

