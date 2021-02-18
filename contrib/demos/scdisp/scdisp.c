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
 *	@(#)scdisp.c	1.1	91/05/08
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

#define	MAX_VALUE	65535

#define	max(a, b)	(((a) > (b)) ? (a) : (b))
#define	abs(a)		(((a) > 0) ? (a) : -(a))

#define	FONT	"*-charter-medium-r-*"
char       *pattern = FONT;
char       *scale_name;
char      **font_list;
int         full_scaling = 0;

char       *fontname;

Display    *dpy;
Window      win;
int         screen;
int         x_res = 75,
            y_res = 75;
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

static int  fontsizes[] = {
    6,
    8,
    10,
    12,
    14,
    16,
    18,
    20,
    24,
    28,
    32,
    36,
    40,
    44,
    48,
    52,
    56,
    60,
    64,
    70,
    76,
    82
};

#define	MAX_FONTS	(sizeof(fontsizes)/sizeof(int))

static Font fonts[MAX_FONTS];
int         num_fonts = 6;	/* start with 6 sizes */

/* these all get adjusted when the window gets resized */
int         width = 300;
int         height = 100;

#define	DEFAULT_STRING	"Turbo Powered Beast"

char       *string = DEFAULT_STRING;

extern char *getenv();
static char *scale_res();

static void
usage(cmd)
    char       *cmd;
{
    (void) fprintf(stderr,
		   "usage: %s [-display display] [-string string] [-fn font_pattern]\n", cmd);
    exit(1);
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

    cmd = strrchr(argv[0], '/');
    if (cmd)
	cmd++;
    else
	cmd = argv[0];

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[i], "-d", 2) == 0) {
	    if (argv[++i])
		display = argv[i];
	    else
		usage(cmd);
	} else if (strncmp(argv[i], "-st", 3) == 0) {
	    if (argv[i + 1]) {
		string = argv[++i];
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

    if ((dpy = XOpenDisplay(display)) == NULL) {
	fprintf(stderr, "Can\'t open %s\n",
		(display ? display : getenv("DISPLAY")));
	exit(1);
    }
    screen = DefaultScreen(dpy);

    x_res = DisplayWidth(dpy, screen) / DisplayWidthMM(dpy, screen) * 25.4;
    y_res = DisplayHeight(dpy, screen) / DisplayHeightMM(dpy, screen) * 25.4;

    fontgc = XCreateGC(dpy, RootWindow(dpy, screen), NULL, 0);
    XSetForeground(dpy, fontgc, BlackPixel(dpy, screen));

    watch_cursor = XCreateFontCursor(dpy, XC_watch);
    init_fonts();

    vmask = CWEventMask | CWBackPixel;
    xswat.event_mask = ExposureMask | StructureNotifyMask;
    xswat.background_pixel = WhitePixel(dpy, screen);

    win = XCreateWindow(dpy, RootWindow(dpy, screen),
			100, 100, width, height, 0, CopyFromParent,
			CopyFromParent, CopyFromParent, vmask, &xswat);

    xsh.flags = PPosition | PSize | PMinSize;
    xsh.x = 100;
    xsh.y = 100;
    xsh.width = width;
    xsh.height = height;
    xsh.width = basewidth;
    xsh.height = baseheight + baseyloc;
    XSetStandardProperties(dpy, win, "Scaled Text Demo", "Text Scaler", None,
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

static void
load_fonts(name)
    char       *name;
{
    int         i;
    XFontStruct *pfont;
    char       *basename,
               *fname;
    int         dir,
                ascent,
                descent,
                w,
                h;
    XCharStruct overall;

    /* get initial font, and use its metrics to guess at the rest */
    /*
     * this does assume that such scaling is rational, but it seems to work OK
     */
    basename = scale_res(fontsizes[0], x_res, y_res);
    pfont = XLoadQueryFont(dpy, basename);
    free(basename);
    if (!pfont) {
	fprintf(stderr, "couldn't load font \#%s\"\n", basename);
	exit(-1);
    }
    XTextExtents(pfont, string, strlen(string),
		 &dir, &ascent, &descent, &overall);
    w = overall.width;
    h = pfont->ascent + pfont->descent;

    basexloc = XTextWidth(pfont, "A", 1);
    baseyloc = h + 5;
    /* large enough for string plus point size */
    basewidth = w + 2 * basexloc + XTextWidth(pfont, "88 ", 3);
    baseheight = h;

    fonts[0] = pfont->fid;
    /* XXX -- note the full preload -- this may be too slow to accept */
    for (i = 1; i < MAX_FONTS; i++) {
	fname = scale_res(fontsizes[i], x_res, y_res);
	fonts[i] = XLoadFont(dpy, fname);
	free(fname);
    }

    /* try to estimate the window size */
    /*
     * make it wide enough to hold the largets size, and long enough to hold
     * all the initial heights
     */
    width = basewidth * ((double) fontsizes[num_fonts] / (double) fontsizes[0]);
    height = baseheight;
    for (i = 1; i < num_fonts; i++)
	height += baseheight * ((double) fontsizes[i] / (double) fontsizes[0]);
}


/*
 * find all the fonts we have that match the pattern.
 * flag things if we've got a scalable font
 *
 * calculate the base scales and locations
 */
init_fonts()
{
    int         i;
    int         w = 0,
                h = 0;
    char       *fname;
    int         f;
    XFontStruct *pfont;
    int         nf;

    font_list = XListFonts(dpy, pattern, 50, &nf);
    if (nf == 0) {
	fprintf(stderr, "Sorry, can't find any fonts matching %s\n", pattern);
	exit(1);
    }
    if ((f = found_scalable(nf)) != -1) {
	printf("Please wait: loading fonts.\n");
	full_scaling = 1;
	scale_name = font_list[f];
	load_fonts(fname);
	XSync(dpy, 0);
	printf("All fonts loaded.\n");
    } else {

#ifdef notyet
	full_scaling = 0;
	fname = scaled_font(basescale, nf);
	pfont = XLoadQueryFont(dpy, fname);
	if (pfont == (XFontStruct *) 0) {
	    fprintf(stderr, "can't get font\n");
	    exit(1);
	}
#endif

	fprintf(stderr, "no scalable fonts match \"%s\"\n", pattern);
    }
    XFreeFontNames(font_list);
}

/*
 * returns the font (as an index into the list) if it finds a scalable
 * version
 */
found_scalable(nf)
    int         nf;
{
    int         i,
                j;
    char       *d;
    int         scale;

    for (i = 0; i < nf; i++) {
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
    for (i = 0; i < 8; i++) {
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
static char *
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

#define strdup(s)   (strcpy (malloc (strlen (s) + 1), s))

/*
 * returns the font name the server knows closest to the
 * given scale
 */
char       *
scaled_font(scale, nf)
    int         scale;
    int         nf;
{
    int         i;
    int         cvalue = 10000;
    int         closest;
    int         newscale;

    for (i = 0; i < nf; i++) {
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
    int         vis_height;
    int         i;

    width = ev->width;
    height = ev->height;

    vis_height = baseheight;
    num_fonts = 1;
    for (i = 1; i < MAX_FONTS; i++) {
	vis_height += baseheight *
	    ((double) fontsizes[i] / (double) fontsizes[0]);
	/* XXX this often gets one more than it should, but i'm sick of trying
	 * to get this perfect
	 */
	num_fonts++;	
	if (vis_height > height) {
	    break;
	}
    }
}

/*
 * deal with exposure.
 */
redraw()
{
    int         i;
    int         y;
    char        str[512];

    /* clear out window */
    XClearWindow(dpy, win);

    y = baseyloc;
    for (i = 0; i < num_fonts; i++) {
	XSetFont(dpy, fontgc, fonts[i]);
	sprintf(str, "%d %s", fontsizes[i], string);
	XDrawString(dpy, win, fontgc, basexloc, y,
		    str, strlen(str));
	y += (double) baseheight *
	            (double) fontsizes[i] / (double) fontsizes[0];
    }
}
