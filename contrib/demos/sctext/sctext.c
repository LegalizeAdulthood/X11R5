/*
 * Simple scaled text demo
 */

/*
 * Copyright 1991 David Lemke and Network Computing Devices
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Network Computing Devices not be 
 * used in advertising or publicity pertaining to distribution of the 
 * software without specific, written prior permission.  Network Computing 
 * Devices makes no representations about the suitability of this software 
 * for any purpose.  It is provided "as is" without express or implied 
 * warranty.
 *
 * NETWORK COMPUTING DEVICES DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL NETWORK COMPUTING DEVICES BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE 
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  	
 *		Dave Lemke
 *		lemke@ncd.com
 *
 *		Network Computing Devices, Inc
 *		350 North Bernardo Ave
 *		Mountain View, CA 94043
 *
 *	%W%	%E%
 *
 */

#include	<stdio.h>
#include	<sys/types.h>
#include	<string.h>
#include	<ctype.h>
#include	<X11/Xos.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/cursorfont.h>

#define strdup(s)   (strcpy (malloc (strlen(s) + 1), s))

#define	MAX_VALUE	65535

#define	max(a, b)	(((a) > (b)) ? (a) : (b))
#define	abs(a)		(((a) > 0) ? (a) : -(a))

#define	FONT	"*-charter-medium-r-*"
char       *pattern = FONT;
char      **font_list;
int         num_fonts;
char       *scale_name;
int         showfname = 0;
int         disname = 0;
Bool        keep_aspect = False;

char       *fontname;

Display    *dpy;
Window      win;
int         screen;
int         depth;
GC          fontgc;
GC          labelgc;
Atom        kill_atom,
            protocol_atom;
Cursor      watch_cursor;

/* relative size of strings */
int         basewidth,
            baseheight;
int         basexloc,
            baseyloc;
int         basescale = 12;

int         labely;

int         full_scaling;

Font        fid;

/* these all get adjusted when the window gets resized */
int         width = 300;
int         height = 100;
int         xloc = 30;
int         yloc = 90;

#define	MAX_ROWS	30
#define	MAX_COLS	100

#define	DEFAULT_STRING	"Turbo Powered Beast"

char        display_text[MAX_ROWS][MAX_COLS];
int         num_lines = 1;

char       *scale_res();
char       *scaled_font();

extern char *getenv();

static void
usage(cmd)
    char       *cmd;
{
    (void) fprintf(stderr,
		   "usage: %s [-display display] [-aspect] [-file text_file] [-string string] [-fn font_pattern]\n", cmd);
    exit(1);
}

static void
read_string(filename)
    char       *filename;
{
    FILE       *fp;
    int         i = 0;

    if ((fp = fopen(filename, "r")) == NULL) {

	fprintf(stderr, "can't open file \"%s\"\n");
	return;
    }
    while ((i < MAX_ROWS) && (fgets(display_text[i], MAX_COLS, fp) != NULL)) {
	display_text[i][strlen(display_text[i])] = '\0';	/* nuke CR */
	i++;
    }

    num_lines = i;
    (void) fclose(fp);
}

main(argc, argv)
    int         argc;
    char      **argv;
{
    char       *display = NULL;
    register int i;
    char       *cmd;
    XSetWindowAttributes xswat;
    XSizeHints  xsh;
    int         vmask;
    XFontStruct *labelfont;
    char       *filename = NULL;

    cmd = strrchr(argv[0], '/');
    if (cmd)
	cmd++;
    else
	cmd = argv[0];

    strcpy(display_text[0], DEFAULT_STRING);

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[i], "-d", 2) == 0) {
	    if (argv[++i])
		display = argv[i];
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-a", 2) == 0) {
	    keep_aspect = True;
	} else if (strncmp(argv[i], "-sh", 3) == 0) {
	    showfname = 1;
	} else if (strncmp(argv[i], "-st", 3) == 0) {
	    if (argv[i + 1]) {
		strcpy(display_text[0], argv[++i]);
	    } else
		usage(cmd);
	} else if (strncmp(argv[i], "-fi", 3) == 0) {
	    if (argv[i + 1]) {
		filename = argv[++i];
	    } else
		usage(cmd);
	} else if (strncmp(argv[i], "-fn", 3) == 0) {
	    if (argv[++i])
		pattern = argv[i];
	    else
		usage(cmd);
	} else
	    usage(cmd);
    }

    if (filename)
	read_string(filename);

    if ((dpy = XOpenDisplay(display)) == NULL) {
	fprintf(stderr, "Can\'t open %s\n",
		(display ? display : getenv("DISPLAY")));
	exit(1);
    }
    screen = DefaultScreen(dpy);

    fontgc = XCreateGC(dpy, RootWindow(dpy, screen), NULL, 0);
    XSetForeground(dpy, fontgc, BlackPixel(dpy, screen));

    labelgc = XCreateGC(dpy, RootWindow(dpy, screen), NULL, 0);
    XSetForeground(dpy, labelgc, BlackPixel(dpy, screen));

    labelfont = XLoadQueryFont(dpy, "fixed");
    if (labelfont) {
	XSetFont(dpy, labelgc, labelfont->fid);
	labely = labelfont->ascent + labelfont->descent;
    }
    XFreeFont(dpy, labelfont);

    watch_cursor = XCreateFontCursor(dpy, XC_watch);
    init_fonts();

    vmask = CWEventMask | CWBackPixel;
    xswat.event_mask = ExposureMask | StructureNotifyMask;
    xswat.background_pixel = WhitePixel(dpy, screen);

    win = XCreateWindow(dpy, RootWindow(dpy, screen),
			100, 100, width, height, 0, CopyFromParent,
			CopyFromParent, CopyFromParent, vmask, &xswat);

    xsh.flags = PPosition | PSize;
    if (keep_aspect)
	xsh.flags |= PAspect;
    xsh.x = 100;
    xsh.y = 100;
    xsh.width = width;
    xsh.height = height;
    xsh.min_aspect.x = xsh.max_aspect.x = basewidth;
    xsh.min_aspect.y = xsh.max_aspect.y = baseheight;
    XSetStandardProperties(dpy, win, "Text Sampler", "Sampler", None,
			   argv, argc, &xsh);

    protocol_atom = XInternAtom(dpy, "WM_PROTOCOLS", False);
    kill_atom = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &kill_atom, 1);

    XMapWindow(dpy, win);
    while (1) {
	XEvent      ev;

	XNextEvent(dpy, &ev);

	if (ev.type == Expose && ev.xexpose.count == 0) {
	    /* get rid of any in the queue */
	    while (XCheckTypedEvent(dpy, Expose, &ev));
	    redraw();
	} else if (ev.type == ConfigureNotify) {
	    rescale((XConfigureEvent *) & ev);
	} else if (ev.type == ClientMessage) {
	    XClientMessageEvent *cmev = (XClientMessageEvent *) & ev;

	    if (cmev->message_type == protocol_atom &&
		    cmev->data.l[0] == kill_atom)
		exit(0);
	}
    }
}


/*
 * find all the fonts we have that match the pattern.
 * flag things if we've got a scalable font
 *
 * calculate the base scales and locations
 */
init_fonts()
{
    int         dir,
                ascent,
                descent;
    int         i;
    XCharStruct overall;
    int         w = 0,
                h = 0;
    char       *fname;
    int         f;
    XFontStruct *pfont;

    font_list = XListFonts(dpy, pattern, 50, &num_fonts);
    if (num_fonts == 0) {
	fprintf(stderr, "Sorry, can't find any fonts matching %s\n", pattern);
	exit(1);
    }
    if ((f = found_scalable()) != -1) {
	full_scaling = 1;
	scale_name = font_list[f];
	fname = scale_res(basescale, 75, 75);
	pfont = XLoadQueryFont(dpy, fname);
	if (pfont == (XFontStruct *) 0) {
	    fprintf(stderr, "can't get font\n");
	    exit(1);
	}
    } else {
	full_scaling = 0;
	fname = scaled_font(basescale);
	pfont = XLoadQueryFont(dpy, fname);
	if (pfont == (XFontStruct *) 0) {
	    fprintf(stderr, "can't get font\n");
	    exit(1);
	}
    }


    for (i = 0; i < num_lines; i++) {
	XTextExtents(pfont, display_text[i], strlen(display_text[i]),
		     &dir, &ascent, &descent, &overall);
	w = max(w, overall.width);
	h = max(h, overall.ascent + overall.descent);
    }

    basexloc = XTextWidth(pfont, "A", 1);
    baseyloc = h;

    basewidth = w + 2 * basexloc;
    baseheight = h * (num_lines + 1);

    width = basewidth;
    height = baseheight;

    xloc = basexloc;
    yloc = baseyloc;

    /* dump the extent info, and reload as an ID */
    XFreeFont(dpy, pfont);
    fid = XLoadFont(dpy, fname);
    free(fname);

    XSetFont(dpy, fontgc, fid);
}

/*
 * returns the font (as an index into the list) if it finds a scalable
 * version
 */
found_scalable()
{
    int         i,
                j;
    char       *d;
    int         scale;

    for (i = 0; i < num_fonts; i++) {
	d = font_list[i];
	for (j = 0; j < 7; j++) {
	    d = strchr(d, '-');
	    if (d == NULL)
		goto next;
	    d++;		/* skip over '-' */
	}
	scale = atoi(d);
	if (scale == 0)
	    return i;
next:
	;
    }
    return -1;
}

/*
 * returns the scale for the given font name
 */
return_scale(fname)
    char       *fname;
{
    int         i;
    char       *d;

    d = fname;
    for (i = 0; i < 7; i++) {
	d = strchr(d, '-');
	if (d == NULL)
	    return -1;
	d++;			/* skip over '-' */
    }
    return atoi(d);
}

/*
 * builds a font name with the given scale and resolution out of scale_name
 */
char       *
scale_res(scale, xres, yres)
    int         scale,
                xres,
                yres;
{
    char       *newname;
    char        foo[10];
    int         dashes = 0;
    char       *c,
               *n,
               *f;

    c = scale_name;
    n = newname = (char *) malloc(strlen(scale_name) * 2 + 1);
    while (*c != '\0') {
	*n = *c;
	if (*c == '-') {
	    n++;
	    c++;		/* skip dash */
	    dashes++;
	    if (dashes == 8) {
		sprintf(foo, "%1d", scale * 10);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    } else if (dashes == 9) {
		sprintf(foo, "%1d", xres);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    } else if (dashes == 10) {
		sprintf(foo, "%1d", yres);
		f = foo;
		while (isdigit(*c))
		    c++;
		while (*f != '\0')
		    *n++ = *f++;
	    }
	} else {
	    n++;
	    c++;
	}
    }
    *n = '\0';
    return newname;
}

/*
 * returns the font name the server knows closest to the
 * given scale
 */
char       *
scaled_font(scale)
    int         scale;
{
    int         i;
    int         cvalue = 10000;
    int         closest;
    int         newscale;

    for (i = 0; i < num_fonts; i++) {
	newscale = return_scale(font_list[i]);
	if (abs(scale - newscale) < cvalue) {
	    closest = i;
	    cvalue = abs(scale - newscale);
	}
    }
    return strdup(font_list[closest]);
}

/*
 * handle a configure notify event.  rescale the font for the
 * new window size, and adjust the text location
 */
rescale(ev)
    XConfigureEvent *ev;
{
    char       *fname;
    static      init = 0;
    int         scale,
                xres,
                yres;

    if (init && ev->width == width && ev->height == height)
	return;

    init = 1;
    width = ev->width;
    height = ev->height;

    xres = (int) ((float) width / (float) basewidth *
		  (float) basescale * 72.0);
    yres = (int) ((float) height / (float) baseheight *
		  (float) basescale * 72.0);
    xloc = basexloc * xres / (72 * basescale);
    yloc = baseyloc * yres / (72 * basescale);

    /* figure out whether to try to display font name */
    if ((height > 3 * labely) && showfname)
	disname = 1;
    else
	disname = 0;

    if (fontname)
	free(fontname);

    if (full_scaling) {
	scale = 1;
	/* normalize to avoid overflowing things */
	if (xres > 1000 && yres > 1000) {
	    xres /= 10;
	    yres /= 10;
	    scale = 10;
	}
	fname = scale_res(scale, xres, yres);
    } else {
	scale = (int) ((float) height / (float) baseheight *
		       (float) basescale);
	fname = scaled_font(scale);
    }

    fontname = fname;

    XUnloadFont(dpy, fid);
    XDefineCursor(dpy, win, watch_cursor);
    fid = XLoadFont(dpy, fname);
    XUndefineCursor(dpy, win);
    XSetFont(dpy, fontgc, fid);
}

/*
 * deal with exposure.
 */
redraw()
{
    int         i;

    /* clear out window */
    XClearWindow(dpy, win);

    for (i = 0; i < num_lines; i++) {
	XDrawString(dpy, win, fontgc, xloc, yloc * (i + 1),
		    display_text[i], strlen(display_text[i]));
    }

    if (disname)
	XDrawString(dpy, win, labelgc, 10, labely, fontname,
		    strlen(fontname));
}
