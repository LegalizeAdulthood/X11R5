/* 
 * xcolors - display all X color names and colors
 * Time-stamp: <91/10/04 13:49:11 gildea>
 *
 * Copyright (c) 1989,1991 by Stephen Gildea
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided 
 * that the above copyright notice appears in all copies and that both that 
 * copyright notice and this permission notice appear in supporting 
 * documentation.  The authors make no representations about the 
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Author:  Stephen Gildea <gildea@expo.lcs.mit.edu>
 *          Orignal program by Paul Vixie.
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>		/* for sqrt() */

#ifndef RGB_TXT
#define RGB_TXT "/usr/lib/X11/rgb.txt"
#endif

String fallback_resources[] = {
    "*Label.Font: 5x7",
    "*panes.Width: 985",
    "*Text*Height: 50",
    "*viewport.allowVert: on",
    "*sample*String: \
Button 1 sets the text color, button 2 sets the background.\\n\
Press q to quit.  You can also type new text here.",
    "*Label*translations: #override \
	<Btn1Up>: set-foreground()\\n\
	<Btn2Up>: set-background()",
    "*Box*baseTranslations: #override \
	<Key>q: quit()",
    NULL};

struct OptionVals {
    char *start_color;
    char *rgb_file;
    char *near_color;
    int   near_distance;	/* radius, 0 to sqrt(3)*256 */
} opts;

XtResource resources[] = {
    { "startColor", "StartColor", XtRString, sizeof (char *), 
      XtOffsetOf(struct OptionVals, start_color), XtRString, NULL },
    { "rgbFile", "RgbFile", XtRString, sizeof (char *), 
      XtOffsetOf(struct OptionVals, rgb_file), XtRString, RGB_TXT },
    { "nearColor", "NearColor", XtRString, sizeof (char *), 
      XtOffsetOf(struct OptionVals, near_color), XtRString, NULL },
    { "nearDistance", "NearDistance", XtRInt, sizeof (int), 
      XtOffsetOf(struct OptionVals, near_distance),XtRImmediate,(XtPointer)64},
};

XrmOptionDescRec cmd_options[] = {
    {"-start", ".startColor", XrmoptionSepArg, NULL},
    {"-rgbfile", ".rgbFile", XrmoptionSepArg, NULL},
    {"-near", ".nearColor", XrmoptionSepArg, NULL},
    {"-distance", ".nearDistance", XrmoptionSepArg, NULL},
};

String wm_trans =
    "<ClientMessage>WM_PROTOCOLS: quit()\n";

char *program_name;
int n_colors_displayed = 0;	/* number of colors we are showing */
Atom wm_delete_window;
Atom wm_protocols;

Widget sample;			/* the sample text */

void usage()
{
    static char *option_help[] = {
"    -rgbfile filename           rgb.txt color database file to use",
"    -start colorname            first color to show",
"    -near colorname             only show colors similar to this one",
"    -distance int               how far in 0-255 RGB space is similar",
"",
NULL};
    char **cpp;

    fprintf (stderr, "usage: %s [-options ...]\n",
	     program_name);
    fprintf (stderr, " where options include:\n");
    for (cpp = option_help; *cpp; cpp++) {
	fprintf (stderr, "%s\n", *cpp);
    }
}

void
do_set_color(w, colorname, foreground_flag)
    Widget w;
    String colorname;		/* if null, get from w */
    Boolean foreground_flag;
{
    Arg arg[2];
    int n;
    XrmValue namein, pixelout;
    Pixel color;

    if (colorname) {
	namein.addr = colorname;
	namein.size = strlen(colorname) + 1;
	pixelout.size = 0;	/* so we can check for success */
	XtConvert(w, XtRString, &namein, XtRPixel, &pixelout);
	if (pixelout.size == 0) {
	    fprintf(stderr, "%s: no such color as \"%s\" for %s\n",
		    program_name, colorname,
		    foreground_flag ? XtNforeground : XtNbackground);
	    return;
	}
	color = *(Pixel*)(pixelout.addr);
    } else {
	n = 0;
	XtSetArg(arg[n], XtNborderColor, &color); n++;
	XtGetValues(w, arg, n);
    }

    /* set the value in the text sample */
    XtSetArg(arg[0], foreground_flag ? XtNforeground : XtNbackground, color);
    XtSetValues(sample, arg, 1);
}

/* ARGSUSED */
void
set_foreground(w, event, params, num_params)
    Widget w;
    XEvent *event;		
    String *params;	
    Cardinal *num_params;
{
    do_set_color(w, *num_params ? params[0] : NULL, TRUE);
}

/* ARGSUSED */
void
set_background(w, event, params, num_params)
    Widget w;
    XEvent *event;		
    String *params;	
    Cardinal *num_params;
{
    do_set_color(w, *num_params ? params[0] : NULL, FALSE);
}

/* ARGSUSED */
void
quit_action(w, event, params, num_params)
    Widget w;
    XEvent *event;		
    String *params;	
    Cardinal *num_params;
{
    if(event->type == ClientMessage && event->xclient.type == wm_protocols
       && event->xclient.data.l[0] != wm_delete_window)
	return;

    exit(0);
}

XtActionsRec actionTable[] = {
    {"quit", quit_action},
    {"set-foreground", set_foreground},
    {"set-background", set_background},
};


main(argc, argv)
     int argc;
     char *argv[];
{
    XtAppContext app_context;
    Widget toplevel, panes, viewport, colors;
    Arg arg[10];
    int n;
    Status stat;
    XColor near_rgb, junk_rgb;

    toplevel = XtAppInitialize(&app_context, "Xcolors",
			       cmd_options, XtNumber(cmd_options), &argc, argv,
			       fallback_resources, NULL, 0);

    XtGetApplicationResources (toplevel, (XtPointer) &opts, resources,
			       XtNumber(resources), NULL, 0);

    program_name = argv[0];
    argc--; argv++;

    if (argc != 0) {
	usage();
	exit(3);
    }

    if (opts.near_color) {
	/* User asked to only see colors near this in RGB space.
	   Get the color from the server and scale it for comparison with
	   rgb.txt values */
	stat = XLookupColor(XtDisplay(toplevel),
			    DefaultColormapOfScreen(XtScreen(toplevel)),
			    opts.near_color, &near_rgb, &junk_rgb);
	if (!stat) {
	    fprintf(stderr, "%s: color \"%s\" is not known to the X server\n",
		program_name, opts.near_color);
	    exit(1);
	}
	near_rgb.red >>= 8;
	near_rgb.green >>= 8;
	near_rgb.blue >>= 8;
	near_rgb.flags = DoRed|DoBlue|DoGreen;
    } else
	near_rgb.flags = 0;

    XtAppAddActions(app_context, actionTable, XtNumber(actionTable));

    n = 0;
    panes = XtCreateManagedWidget("panes", panedWidgetClass,
				  toplevel, arg, n);
    
    n = 0;
    XtSetArg(arg[n], XtNeditType, XawtextEdit); n++;
    sample = XtCreateManagedWidget("sample", asciiTextWidgetClass,
				   panes, arg, n);
    
    n = 0;
    viewport = XtCreateManagedWidget("viewport", viewportWidgetClass,
				     panes, arg, n);
    
    n = 0;
    colors = XtCreateManagedWidget("colors", boxWidgetClass,
				   viewport, arg, n);
    
    colordemo(colors, opts.start_color, near_rgb, opts.near_distance);

    if (n_colors_displayed == 0) {
	fprintf(stderr, "%s: no colors to display\n", program_name);
	exit(5);
    }
    
    XtSetMappedWhenManaged(toplevel, FALSE);
    XtRealizeWidget(toplevel);

    /* do WM_DELETE_WINDOW before map */
    XtOverrideTranslations(toplevel, XtParseTranslationTable(wm_trans));
    wm_protocols = XInternAtom(XtDisplay(toplevel), "WM_PROTOCOLS", False);
    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				   False);
    XSetWMProtocols(XtDisplay(toplevel), XtWindow(toplevel),
		    &wm_delete_window, 1);
    
    XtMapWidget(toplevel);
    XtAppMainLoop(app_context);
    /* NOTREACHED */
}

colordemo(parent, startcolor, nearcolor, maxdist)
     Widget parent;
     char *startcolor;
     XColor nearcolor;		/* scaled 0-255 */
     int maxdist;
{
    int r, g, b, prev_r, prev_g, prev_b;
    char colorname[50], save_colorname[50];
    FILE *rgb;
    Bool do_color();
    double ddist = maxdist;
    
    rgb = fopen(opts.rgb_file, "r");
    if (rgb == NULL) {
	perror(opts.rgb_file);
	exit(2);
    }
    
    prev_r = prev_g = prev_b = -1;
    save_colorname[0] = '\0';
    while (4 == fscanf(rgb, "%d %d %d %[^\n]\n", &r, &g, &b, colorname)) {
	if (startcolor)
	  if (l_strcasecmp(colorname, startcolor))
	    continue;		/* haven't reached starting point yet */
	  else
	    startcolor = (char *)NULL;
	if (r != prev_r  ||  g != prev_g  ||  b != prev_b) {
	    if (nearcolor.flags) {
		double ourdist =
		    sqrt((double)((nearcolor.red-r)*(nearcolor.red-r)
				  + (nearcolor.green-g)*(nearcolor.green-g)
				  + (nearcolor.blue-b)*(nearcolor.blue-b)));
		if (ourdist > ddist)
		    continue;
	    }
	    if (save_colorname[0] != '\0') /* skip first time through */
	      if (!do_color(parent, save_colorname))
		return;
	    prev_r = r;
	    prev_g = g;
	    prev_b = b;
	}
	strcpy(save_colorname, colorname);
    }
    if (save_colorname[0] != '\0')
      (void)do_color(parent, save_colorname);

    if (startcolor) {		/* never found starting color in the file */
	fprintf(stderr, "%s: \"%s\" not found in %s\n",
		program_name, startcolor, opts.rgb_file);
    }
    
}


/*
 * does one color.
 * Returns a success code.
 */
Bool
do_color(parent, colorname)
     Widget parent;
     char *colorname;
{
    Arg		arg[10];
    int 	n;
    XrmValue	namein, pixelout;
    
    /* convert colorname to a Pixel (a colormap index) */
    namein.addr = colorname;
    namein.size = strlen(colorname) + 1;
    pixelout.size = 0;	/* so we can check for success */
    XtConvert(parent, XtRString, &namein, XtRPixel, &pixelout);
    if (pixelout.size == 0) {
	fprintf(stderr, "%s: Not enough room in color map.\n", program_name);
	fprintf(stderr, "To see colors after this, ");
	fprintf(stderr, "use the -start option to name a starting color.\n");
	return FALSE;
    }
    
    /* create a widget to display the color */
    n = 0;
    XtSetArg(arg[n], XtNborderWidth, 10); n++;
    XtSetArg(arg[n], XtNborderColor, *(Pixel*)(pixelout.addr)); n++;
    XtCreateManagedWidget(colorname, labelWidgetClass, parent, arg, n);
    n_colors_displayed++;
    return TRUE;
}


/*
 * Not all systems have strcasecmp, so we provide our own.
 * Similar to strcmp, but ignores case.
 * Always returns 1 if different.
 */
int
l_strcasecmp(s1, s2)
     char *s1, *s2;
{
    for ( ; *s1 && *s2 ; s1++, s2++)
      if ((isupper(*s1) ? tolower(*s1) : *s1) !=
	  (isupper(*s2) ? tolower(*s2) : *s2))
	return 1;
    if (*s1 || *s2)
      return 1;
    return 0;
}
