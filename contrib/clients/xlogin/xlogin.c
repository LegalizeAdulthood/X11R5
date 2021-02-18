/*
 * xlogin - X login manager
 *
 * $Id: xlogin.c,v 2.9 1991/10/04 17:06:04 stumpf Exp stumpf $
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
 * Filename: xlogin.c
 * Creator:  Markus Stumpf <Markus.Stumpf@Informatik.TU-Muenchen.DE>
 *
 * $Source: /usr/wiss/stumpf/R4/local/clients/xlogin/RCS/xlogin.c,v $
 *
 */

#include "xlogin.h"

extern int      do_login;
extern int      have_display;
extern int      info_eproclim;
extern int      info_eusers;
extern int      info_lastlog;
extern char     ll_label[BUFSIZ];
extern char     version[BUFSIZ];
extern char     Myname[BUFSIZ];
extern char     MyVersion[BUFSIZ];

/*
 * external functions
 */
extern void AddMotdInfo _P((Widget, Widget *, MotdListPtr));
extern void RelayoutMotdList _P((void));
extern Boolean MakeValidLabel _P((String));

/*
 * forward definitions
 */
void InitXDisplay _P((int *, char **));
void DoXDisplay _P((void));
void ExitMotd   _P((Widget, XtPointer, XtPointer));
static void motdTimeOver _P((XtPointer, XtIntervalId *));


XtAppContext    app_ctxt;
String          display;
Widget          toplevel;

static char    *fallback_resources[] =
{
    "*motdLabel*changedLabel:	%P",
    "*motdLabel*unchangedLabel:	%P unchanged",
    "*motdLabel.vertDistance:	10",
    "*Label.borderWidth:	1",
    ".haveClassDefaults:	False",
    "*version:			0.0",
    NULL
};

XrmOptionDescRec options[] =
{
    {"-showMOTD", XtNshowMOTD, XrmoptionSepArg, NULL},
    {"-showVersion", XtNshowVersion, XrmoptionNoArg, "True"},
    {"-userFiles", XtNuserFiles, XrmoptionSepArg, NULL}
};

typedef struct {
    String          unchanged_label;
    String          changed_label;
}               xlogin_label_subresourceRec, *xlogin_label_subresourcePtr;

typedef struct {
    XFontStruct    *font;
}               xlogin_text_subresourceRec, *xlogin_text_subresourcePtr;

xlogin_label_subresourceRec app_label_subresources;
xlogin_text_subresourceRec app_text_subresources;
xlogin_resourceRec app_resources;
xlogin_preresourceRec pre_app_resources;

MotdWidgetClassRec mwc;

#define MOffset(a)	XtOffset(xlogin_label_subresourcePtr, a)
XtResource      label_subresources[] =
{
    {XtNchangedLabel, XtCLabel, XtRString, sizeof(String),
     MOffset(changed_label), XtRString, ""},
    {XtNunchangedLabel, XtCLabel, XtRString, sizeof(String),
     MOffset(unchanged_label), XtRString, ""}
};
#undef MOffset

#define MOffset(a)	XtOffset(xlogin_text_subresourcePtr, a)
XtResource      text_subresources[] =
{
    {XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     MOffset(font), XtRString,
     "-misc-fixed-medium-r-normal--*-120-*-*-c-*-iso8859-1"}
};
#undef MOffset

#define MOffset(a)	XtOffset(xlogin_resourcePtr, a)
XtResource      resources[] =
{
    {XtNbackgroundBitmapFile, XtCBackgroundBitmapFile, XtRString,
     sizeof(String),
     MOffset(background_bitmap_file), XtRString, "DEFAULT"},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
     MOffset(foreground), XtRString, (XtPointer) "XtDefaultForeground"},
    {XtNhaveClassDefaults, XtCHaveClassDefaults, XtRBoolean,
     sizeof(Boolean),
     MOffset(have_class_defaults), XtRString, "false"},
    {XtNnoLastloginLabel, XtCLabel, XtRString, sizeof(String),
     MOffset(no_lastlogin_label), XtRString,
     "No information on time of last login found"},
    {XtNnologinLabel, XtCLabel, XtRString, sizeof(String),
     MOffset(nologin_label), XtRString, "No login permitted"},
    {XtNshowMOTD, XtCShowMOTD, XtRString, sizeof(String),
     MOffset(show_MOTD), XtRString, "IfChanged"},
    {XtNshowVersion, XtCShowVersion, XtRBoolean, sizeof(Boolean),
     MOffset(show_version), XtRString, "false"},
    {XtNuserFiles, XtCFileList, XtRString, sizeof(String),
     MOffset(user_files), XtRString, ""},
    {XtNversion, XtCVersion, XtRString, sizeof(String),
     MOffset(version), XtRImmediate, NULL}
};
#undef MOffset

XtCallbackRec   callbacks[] =
{
    {ExitMotd, NULL},
    {NULL, NULL}};

Arg             label_args[] =
{
    {XtNjustify, (XtArgVal) XtJustifyCenter},
    {XtNborderWidth, (XtArgVal) 1},
    {XtNwidth, (XtArgVal) 0},
    {XtNfromVert, (XtArgVal) NULL},
    {XtNlabel, (XtArgVal) NULL}
};

Arg             text_args[] =
{
    {XtNstring, (XtArgVal) NULL},
    {XtNfromVert, (XtArgVal) NULL},
    {XtNwidth, (XtArgVal) 0},
    {XtNheight, (XtArgVal) 0},
    {XtNscrollHorizontal, (XtArgVal) XawtextScrollWhenNeeded},
    {XtNscrollVertical, (XtArgVal) XawtextScrollWhenNeeded},
    {XtNeditType, (XtArgVal) XawtextRead},
    {XtNtype, (XtArgVal) XawAsciiFile},
    {XtNresizable, (XtArgVal) True},
};


/*
 * This routine is called before exit; it closes the display
 * and frees all resources
 */
void
CloseXDisplay()
{
    Debug("Closing X Display");
    XtDestroyWidget(toplevel);
    XtDestroyApplicationContext(app_ctxt);
}


/*
 * This is the routine that is called on press of the
 * endbutton; exits program with status 0, if login permitted,
 * with status 1 otherwise
 */
#if NeedFunctionPrototypes
static void
ExitMotd(
	    Widget widget,	/* UNUSED */
	    XtPointer client_d,	/* UNUSED */
	    XtPointer call_d)	/* UNUSED */
#else
static void
ExitMotd(widget, client_d, call_d)
    Widget          widget;	/* UNUSED */
    XtPointer       client_d;	/* UNUSED */
    XtPointer       call_d;	/* UNUSED */
#endif				/* NeedFunctionPrototypes */
{
    CloseXDisplay();
    Debug("ExitMotd(): status=%d ", !do_login);
    exit(!do_login);
}



/*
 * This is the routine that's called when the timeout has
 * expired.
 */
#if NeedFunctionPrototypes
static void
motdTimeOver(
		XtPointer c_data,	/* UNUSED */
		XtIntervalId * id)	/* UNUSED */
#else
static void
motdTimeOver(c_data, id)
    XtPointer       c_data;	/* UNUSED */
    XtIntervalId   *id;		/* UNUSED */
#endif				/* NeedFunctionPrototypes */
{
    Debug("Timeout has expired");
    do_login = !pre_app_resources.timeout_action;
    ExitMotd((Widget) NULL, (XtPointer) NULL, (XtPointer) NULL);
}

/*
 * this procedure initializes the display and parses
 * the command line options
 */
#if NeedFunctionPrototypes
void
InitXDisplay(
		int *argc,
		char *argv[])
#else
void
InitXDisplay(argc, argv)
    int            *argc;
    char           *argv[];
#endif				/* NeedFunctionPrototypes */
{
    Display        *mDisplay;

    Debug("InitXDisplay()");
    Debug("InitXDisplay(): argc=%d", *argc);
    XtToolkitInitialize();
    app_ctxt = XtCreateApplicationContext();
    XtAppSetFallbackResources(app_ctxt, fallback_resources);
    mDisplay = XtOpenDisplay(app_ctxt, display, XLOGIN_NAME, XLOGIN_CLASS,
			     options, XtNumber(options), argc, argv);
    if (mDisplay == (Display *) NULL) {
	fprintf(stderr, "%s: WARNING! Cannot open display \"%s\" \n",
		Myname, display);
	XtDestroyApplicationContext(app_ctxt);
	have_display = FALSE;
	return;
    }
    Debug("    mDisplay=0x%x", mDisplay);
    toplevel = XtVaAppCreateShell(XLOGIN_NAME, XLOGIN_CLASS,
				  applicationShellWidgetClass, mDisplay,
				  XtNallowShellResize, True,
				  NULL, NULL);
    XtGetApplicationResources(toplevel, &app_resources, resources,
			      XtNumber(resources), NULL, 0);
    Debug("    Dumping APPLresources ...");
    Debug("        backgroundBitmapFile=%s",
	  app_resources.background_bitmap_file);
    Debug("        version=%s", app_resources.version);
    Debug("        showMOTD=%s", app_resources.show_MOTD);
    Debug("        showVersion=%d", app_resources.show_version);
    Debug("        userFiles=%s", app_resources.user_files);
    Debug("        haveClassDefaults=%d", app_resources.have_class_defaults);
    Debug("        foreground=%d", app_resources.foreground);

    mwc.bitmap_file = app_resources.background_bitmap_file;
}


/*
 * this procedure does all the display stuff:
 *   defining, creating and starting widgets
 */
#if NeedFunctionPrototypes
void
DoXDisplay(
	      void)
#else
void
DoXDisplay()
#endif				/* NeedFunctionPrototypes */
{
    Widget          form, queue = (Widget) NULL;
    Arg             args[20];
    Dimension       formwidth, formheight, fnt_width;
    Position        dpywidth, dpyheight;
    int             timeout_add = 0, dhx, dhy;
    MotdListPtr     item;
    Pixmap          bgPmap, bgBmap;
    unsigned int    bm_h, bm_w;
    Pixel           fgPixel, bgPixel;
    String          labelstr;

    labelstr = (String) NULL;

    Debug("DoXDisplay(): entered ...");

    form =
	XtCreateManagedWidget("form",
			      formWidgetClass,
			      toplevel,
			      NULL, NULL);

    if (strcmp(mwc.bitmap_file, "DEFAULT") == 0) {
	if (XDefaultDepthOfScreen(XtScreen(form)) == 1)
	    mwc.bitmap_file = "gray";
	else
	    mwc.bitmap_file = "None";
    }

    if (strcmp(mwc.bitmap_file, "None") != 0) {
	bgBmap = XmuLocateBitmapFile(XtScreen(form),
				  mwc.bitmap_file, NULL, 0, &bm_w, &bm_h,
				     &dhx, &dhy);
	if (bgBmap == (Pixmap) NULL) {
	    fprintf(stderr, "%s: WARNING! Bitmap-file for %s%s%s \n",
		    Myname, "background \"", mwc.bitmap_file,
		    "\" not found.");
	}
	else {
	    fgPixel = app_resources.foreground;
	    XtVaGetValues(form,
			  XtNbackground, &bgPixel,
			  NULL, NULL);
	    Debug("fgColor=%d, bgColor=%d ", fgPixel, bgPixel);

	    bgPmap = XmuCreatePixmapFromBitmap(XtDisplay(form),
				      DefaultRootWindow(XtDisplay(form)),
					       bgBmap, bm_w, bm_h,
				   XDefaultDepthOfScreen(XtScreen(form)),
					       fgPixel, bgPixel);
	    XFreePixmap(XtDisplay(form), bgBmap);

	    XtVaSetValues(form,
			  XtNbackgroundPixmap, bgPmap,
			  NULL, NULL);
	}
    }
    app_text_subresources.font = (XFontStruct *) NULL;
    XtGetSubresources(form, (XtPointer) & app_text_subresources,
		      "font", "Text", text_subresources,
		      XtNumber(text_subresources), (ArgList) NULL, 0);
    Debug("    Subresource font=0x%x", app_text_subresources.font);

    /* find the max width and height of the font */
    fnt_width = app_text_subresources.font->max_bounds.width;
    mwc.font_height = app_text_subresources.font->ascent +
	app_text_subresources.font->descent;
    Debug("    Fontinfo: width=%d, height=%d", fnt_width, mwc.font_height);

    /*
     * plan for 80 columns plus 30 pixels for scrollbar (if needed)
     * and match the number of lines in motd
     * allow for default border width of 1 at both edges
     * allow 1 line for sometimes wrong font infos
     */
    mwc.wid_width = fnt_width * 80 + 30;
    label_args[2].value = (XtArgVal) mwc.wid_width;
    text_args[2].value = (XtArgVal) mwc.wid_width;

#define WARN1	"WARNING! Version in ClassDefaults \"%s\" \
not equal to version of program \"%s\"\n         \
Please notify System-Administrator"

#define WARN2	"WARNING! No ClassDefaults found for class \"%s\"\n         \
Please notify System-Aministrator"

#define WARN3	"WARNING! No resource \"version\" found in \
ClassDefaults file for class \"%s\"\n         \
Please notify System-Administrator"

    if (app_resources.version == (char *) NULL) {
	Debug("    No resource \"version\" found in ClassDefaults file!");
	labelstr = (char *) malloc(strlen(WARN3) + strlen(XLOGIN_CLASS));
	if (labelstr != (char *) NULL) {
	    sprintf(labelstr, WARN3, XLOGIN_CLASS);
	}
	else {
	    Debug("    main() - noClassDefaults: malloc() failed");
	    fprintf(stderr, "%s: %s \"%s\"\n", Myname,
		    "WARNING! No resource \'version\' found for class",
		    XLOGIN_CLASS);
	}
    }
    else if (strcmp(app_resources.version, "0.0") == 0)
	if (app_resources.have_class_defaults == True) {
	    Debug("    No resource \"version\" found in ClassDefaults file!");
	    labelstr = (char *) malloc(strlen(WARN3) + strlen(XLOGIN_CLASS));
	    if (labelstr != (char *) NULL) {
		sprintf(labelstr, WARN3, XLOGIN_CLASS);
	    }
	    else {
		Debug("    main() - noClassDefaults: malloc() failed");
		fprintf(stderr, "%s: %s \"%s\"\n", Myname,
		      "WARNING! No resource \'version\' found for class",
			XLOGIN_CLASS);
	    }
	}
	else {

	    Debug("    No ClassDefaults found! Issue warning");
	    labelstr = (char *) malloc(strlen(WARN2) + strlen(XLOGIN_CLASS));
	    if (labelstr != (char *) NULL) {
		sprintf(labelstr, WARN2, XLOGIN_CLASS);
	    }
	    else {
		Debug("    main() - noClassDefaults: malloc() failed");
		fprintf(stderr, "%s: %s \"%s\"\n", Myname,
			"WARNING! No ClassDefaults found for class",
			XLOGIN_CLASS);
	    }
	}
    else if (strcmp(app_resources.version, MyVersion) != 0) {
	labelstr = (char *) malloc(strlen(WARN1) + strlen(MyVersion)
				   + strlen(app_resources.version));
	if (labelstr != (char *) NULL) {
	    sprintf(labelstr, WARN1, app_resources.version, MyVersion);
	}
	else {
	}
    }
    if (labelstr != (char *) NULL) {
	label_args[3].value = (XtArgVal) queue;
	label_args[4].value = (XtArgVal) labelstr;
	queue =
	    XtCreateManagedWidget("noClassDefaults",
				  labelWidgetClass,
				  form,
				  label_args, XtNumber(label_args));
    }


    if (app_resources.show_version) {
	Position        lwidth;

	label_args[2].value = (XtArgVal) 0;
	label_args[3].value = (XtArgVal) queue;
	label_args[4].value = (XtArgVal) version;
	queue =
	    XtCreateManagedWidget("version",
				  labelWidgetClass,
				  form,
				  label_args, XtNumber(label_args));
	XtVaGetValues(queue,
		      XtNwidth, &lwidth,
		      NULL, NULL);

	Debug("    lwidth=%d - Re-positioning to %d", lwidth,
	      (mwc.wid_width - lwidth) / 2);
	XtVaSetValues(queue,
		      XtNhorizDistance, (mwc.wid_width - lwidth) / 2,
		      NULL, NULL);
	label_args[2].value = (XtArgVal) mwc.wid_width;
    }
    label_args[3].value = (XtArgVal) queue;
#ifdef LASTLOGIN
    if (info_lastlog) {
	label_args[4].value = (XtArgVal) ll_label;
    }
    else
#endif				/* LASTLOGIN */
    if (do_login) {
	label_args[4].value = (XtArgVal) app_resources.no_lastlogin_label;
    }
    else {
	label_args[4].value = (XtArgVal) ll_label;
    }
    label_args[2].value = (XtArgVal) mwc.wid_width;
    queue =
	XtCreateManagedWidget("loginInfo",
			      labelWidgetClass,
			      form,
			      label_args, XtNumber(label_args));

    XtGetSubresources(form, (XtPointer) & app_label_subresources,
		      "motdLabel", "Label", label_subresources,
		      XtNumber(label_subresources), (ArgList) NULL, 0);
    Debug("  Subres cLabel=\"%s\"", app_label_subresources.changed_label);
    mwc.changed_label = app_label_subresources.changed_label;
    mwc.unchanged_label = app_label_subresources.unchanged_label;

    if (mwc.changed_label != NULL) {
	mwc.changed_use_path =
	    MakeValidLabel(mwc.changed_label);
	Debug("   changed to=\"%s\"", mwc.changed_label);
	Debug("   use_pathname=%d ", mwc.changed_use_path);
    }

    Debug("  Subres uLabel=\"%s\"", mwc.unchanged_label);
    if (mwc.unchanged_label != NULL) {
	mwc.unchanged_use_path =
	    MakeValidLabel(mwc.unchanged_label);
	Debug("   changed to=\"%s\"", mwc.unchanged_label);
	Debug("   use_pathname=%d ", mwc.unchanged_use_path);
    }

    item = mwc.listAnchor;
    while (item != (MotdListPtr) NULL) {
	AddMotdInfo(form, &queue, item);
	item = item->next;
    }

#ifdef QUOTA
    if (info_eusers || info_eproclim) {
	if (info_eproclim) {
	    label_args[5].value = (XtArgVal)
		"You have too many processes running.";
	}
	else {
	    label_args[5].value = (XtArgVal)
		"Too many users already logged in. Try again later";
	}
	label_args[4].value = (XtArgVal) queue;
	queue =
	    XtCreateManagedWidget("quotaInfo",
				  labelWidgetClass,
				  form,
				  (ArgList) label_args, 6);
    }

#endif				/* QUOTA */

    queue =
	XtVaCreateManagedWidget("exitButton",
				commandWidgetClass,
				form,
				XtNcallback, callbacks,
				XtNwidth, mwc.wid_width,
				XtNfromVert, queue,
				NULL, NULL);

    Debug("    Installing Accelerators for form widget");
    XtInstallAccelerators(form, queue);

    /*
     * Get the size of the screen.
     * Want the window to be centered.
     */
    dpywidth = XDisplayWidth(XtDisplay(toplevel),
			     DefaultScreen(XtDisplay(toplevel)));
    dpyheight = XDisplayHeight(XtDisplay(toplevel),
			       DefaultScreen(XtDisplay(toplevel)));

    /*
     * Don't map, but realize it, to get the size of the window
     */
    XtSetMappedWhenManaged(toplevel, False);
    XtRealizeWidget(toplevel);

    while (!XtIsRealized(form))
	formwidth = 2;

    /*
     * Get the size of the window
     */
    XtVaGetValues(form,
		  XtNwidth, &formwidth,
		  XtNheight, &formheight,
		  NULL, NULL);

    /*
     * Place the window centered on the screen.
     * allow 5 pixels distance to top and bottom
     */

    Debug("    DisplayHeight=%d, FormHeight=%d ", dpyheight,
	  formheight);
    if ((dpyheight < formheight) && (mwc.lines_motds != 0)) {
	int             dpy_offest = 5;

	Debug("    New layout for motd ...");

	dpyheight -= 2 * dpy_offest;

	mwc.max_lines = mwc.lines_motds -
	    ((formheight - dpyheight) / mwc.font_height);
	Debug("    mwc.max_lines=%d ", mwc.max_lines);

	XawFormDoLayout(form, True);
	RelayoutMotdList();

	XtSetArg(args[0], XtNheight, dpyheight);
	XtVaSetValues(form,
		      XtNheight, dpyheight,
		      NULL, NULL);

	XawFormDoLayout(form, True);

	XtMoveWidget(toplevel, (Position) (dpywidth - formwidth) / 2,
		     (Position) dpy_offest);
    }
    else {
	XtMoveWidget(toplevel, (Position) (dpywidth - formwidth) / 2,
		     (Position) (dpyheight - formheight) / 2);
    }

    /*
     * Now map the window to get it visible
     */
    XtSetMappedWhenManaged(toplevel, True);
    XtMapWidget(toplevel);

    /*
     * Warp the pointer into the button-click window
     */
    XWarpPointer(XtDisplay(toplevel), (Window) NULL, XtWindow(queue),
		 0, 0, 0, 0, mwc.wid_width / 2, 3);

    /*
     * add timeout, so user get's kicked off, if he's not
     * behind the the display
     */
    if ((mwc.num_motds > 0) || pre_app_resources.timeout_action) {
	timeout_add = mwc.lines_motds * 1.2;
	Debug("    timeout addition=%d", timeout_add);
	XtAppAddTimeOut(app_ctxt,
			(unsigned long) (timeout_add + 30) * 1000,
			motdTimeOver, (XtPointer) NULL);
	Debug("    TimeOut added.");
    }
    Debug("DoXDisplay(): starting XtAppMainLoop()");
    XtAppMainLoop(app_ctxt);
}
