/*
 * xlogin - X login manager
 *
 * $Id: resources.c,v 1.2 1991/10/04 17:05:58 stumpf Exp stumpf $
 *
 * Copyright 1989, 1990, 1991 Technische Universitaet Muenchen (TUM), Germany
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of TUM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  TUM makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 *
 * Author:  Markus Stumpf, Technische Universitaet Muenchen
 *		<Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * Filename: resources.c
 * Date:     xxx 25 Jun 1991  11:39:04
 * Creator:  Markus Stumpf <Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * $Source: /usr/wiss/stumpf/R4/local/clients/xlogin/RCS/resources.c,v $
 *
 */


#include "xlogin.h"

extern void     Debug();

extern MotdWidgetClassRec mwc;
extern char     Myname[];
extern char     Motd[];
extern xlogin_preresourceRec pre_app_resources;

XrmOptionDescRec pre_options[] =
{
    {"-debug", XtNdebug, XrmoptionNoArg, "True"},
    {"-display", XtNdisplay, XrmoptionSepArg, ""},
    {"-lines", XtNlines, XrmoptionSepArg, NULL},
    {"-logout", XtNlogout, XrmoptionNoArg, "True"},
    {"-sysFiles", XtNsysFiles, XrmoptionSepArg, Motd},
    {"-timeoutAction", XtNtimeoutAction, XrmoptionSepArg, "logout"},
    {"-version", XtNversionOnly, XrmoptionNoArg, "True"}
};

void LoadPreResources _P((int *, char[] *));


/* ----------------------------------------------------------------
 * Function Name: LoadPreResources
 * Description:
 * Parameters:
 *	- argc
 *	- argv
 * Returns:
 *	None
 */
#if NeedFunctionPrototypes
void
LoadPreResources(
    int            *argc,
    char           *argv[])
#else
void
LoadPreResources(argc, argv)
    int            *argc;
    char           *argv[];
#endif				/* NeedFunctionPrototypes */
{
    char           *str_type;
    XrmValue        xrm_value;
    XrmDatabase     PreResDB;
    char            res_n[BUFSIZ];
    char            res_c[BUFSIZ];

    XrmInitialize();
    /*
     * Need an empty database; that's a good trick
     */
    PreResDB = XrmGetStringDatabase("");

    XrmParseCommand(&PreResDB, pre_options, XtNumber(pre_options),
		    XLOGIN_NAME, argc, argv);

#define MKRESNC(n, c) \
		STRCPY(n, XLOGIN_NAME); STRNCAT(n, "."); \
		STRCPY(c, XLOGIN_CLASS); STRNCAT(c, ".")

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNdebug);
    STRNCAT(res_c, XtCDebug);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.debug =
	    (XmuCompareISOLatin1(xrm_value.addr, "true") == 0);
    }
    else
	pre_app_resources.debug = False;

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNdisplay);
    STRNCAT(res_c, XtCDisplay);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.display = XtNewString(xrm_value.addr);
    }
    else
	pre_app_resources.display = (String) NULL;

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNlines);
    STRNCAT(res_c, XtCLines);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	fprintf(stderr, "%s: WARNING! Resource/option \"lines\" is obsolete \n",
		Myname);
    }

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNlogout);
    STRNCAT(res_c, XtCLogout);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.logout =
	    (XmuCompareISOLatin1(xrm_value.addr, "true") == 0);
    }
    else
	pre_app_resources.logout = False;

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNsysFiles);
    STRNCAT(res_c, XtCFileList);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.sys_files = XtNewString(xrm_value.addr);
    }
    else
	pre_app_resources.sys_files = Motd;

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNtimeoutAction);
    STRNCAT(res_c, XtCTimeoutAction);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.timeout_action =
	    (XmuCompareISOLatin1(xrm_value.addr, "logout") == 0);
	/* set to True equiv. to logout */
    }
    else
	pre_app_resources.timeout_action = True;

    MKRESNC(res_n, res_c);
    STRNCAT(res_n, XtNversionOnly);
    STRNCAT(res_c, XtCVersionOnly);
    if (XrmGetResource(PreResDB, res_n, res_c, &str_type, &xrm_value)) {
	pre_app_resources.version_only =
	    (XmuCompareISOLatin1(xrm_value.addr, "true") == 0);
    }
    else
	pre_app_resources.version_only = False;


    XrmDestroyDatabase(PreResDB);
    Debug("    Dumping PREresources ....");
    Debug("        display=%s", pre_app_resources.display);
    Debug("        lines=%d", pre_app_resources.lines);
    Debug("        logout=%d", pre_app_resources.logout);
    Debug("        sysFiles=%s", pre_app_resources.sys_files);
    Debug("        timeOutAction=%d", pre_app_resources.timeout_action);
    Debug("        versionOnly=%d", pre_app_resources.version_only);
    Debug("LoadPreResources(): leaving ...");
}
