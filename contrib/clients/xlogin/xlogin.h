/*
 * xlogin - X login manager
 *
 * $Id: xlogin.h,v 1.2 1991/10/04 17:06:24 stumpf Exp stumpf $
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
 *		<stumpf@informatik.tu-muenchen.de>
 *
 * Filename: xlogin.h
 * Date:     xxx 24 June 1991  15:38:08
 * Creator:  Markus Stumpf <stumpf@informatik.tu-muenchen.de>
 *
 * $Source: /usr/wiss/stumpf/R4/local/clients/xlogin/RCS/xlogin.h,v $
 *
 */

#include <X11/Xos.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xmu/CharSet.h>
#include <X11/Xmu/Drawing.h>

#define NNeedFunctionPrototypes TRUE

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#if NeedFunctionPrototypes
#define _P(x) x
#else
#define _P(x) ()
#endif				/* NeedFunctionPrototypes */

#include "patchlevel.h"

#define XtNbackgroundBitmapFile "backgroundBitmapFile"
#define XtNdebug		"debug"
#define XtNdisplay		"display"
#define XtNhaveClassDefaults	"haveClassDefaults"
#define XtNlines		"lines"
#define XtNlogout		"logout"
#define XtNnoLastloginLabel	"noLastloginLabel"
#define XtNnologinLabel		"nologinLabel"
#define XtNshowMOTD		"showMOTD"
#define XtNshowVersion		"showVersion"
#define XtNsysFiles		"sysFiles"
#define XtNtimeoutAction	"timeoutAction"
#define XtNuserFiles		"userFiles"
#define XtNversion		"version"
#define XtNversionOnly		"versionOnly"


#define XtCBackgroundBitmapFile "BackgroundBitmapFile"
#define XtCDebug		"Debug"
#define XtCDisplay		"Display"
#define XtCFileList		"FileList"
#define XtCHaveClassDefaults	"HaveClassDefaults"
#define XtCLines		"Lines"
#define XtCLogout		"Logout"
#define XtCShowMOTD		"ShowMOTD"
#define XtCShowVersion		"ShowVersion"
#define XtCTimeoutAction	"TimeoutAction"
#define XtCVersion		"Version"
#define XtCVersionOnly		"VersionOnly"

#define XtNchangedLabel		"changedLabel"
#define XtNunchangedLabel	"unchangedLabel"

#define	XLOGIN_NAME	"xlogin"
#define	XLOGIN_CLASS	"Xlogin"

/*
 * BUFSIZ is defined in most stdio.h
 */
#ifndef BUFSIZ
#define BUFSIZ		1024
#endif				/* BUFSIZ */

#define PATH_DELIMITER		"/"

#define STRCPY(a, b)	strncpy(a, b, sizeof(a))
#define STRNCAT(a, b)	strncat(a, b, BUFSIZ-strlen(a))

typedef struct _motdList {
    char           *filename;
    Boolean         showit;
    Widget          label, motd;
    Cardinal        lines;
    struct _motdList *next;
}               MotdList, *MotdListPtr;

typedef struct _motdSortTree {
    Widget          wid;
    Cardinal        lines;
    struct _motdSortTree *left, *right;
} MotdSortTree, *MotdSortTreePtr;

typedef struct {
    String          bitmap_file;
    int             font_height;
    int             lines_motds;
    long            ll_time;
    MotdListPtr     listAnchor;
    int             max_lines;
    char           *motdFileString;
    Boolean         motd_always;
    int             num_motds;
    Boolean         changed_use_path;
    Boolean         unchanged_use_path;
    int             wid_width;
    String          changed_label;
    String          unchanged_label;
}               MotdWidgetClassRec;

typedef struct {
    String          background_bitmap_file;
    String          version;
    Pixel	    foreground;
    Boolean         have_class_defaults;
    String          nologin_label;
    String          no_lastlogin_label;
    Boolean         show_version;
    char           *show_MOTD;
    String          user_files;
}               xlogin_resourceRec, *xlogin_resourcePtr;

typedef struct {
    Boolean         debug;
    String          display;
    int             lines;
    Boolean         logout;
    String          sys_files;
    Boolean         timeout_action;
    Boolean         version_only;
}               xlogin_preresourceRec;
