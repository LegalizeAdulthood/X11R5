#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

static char *id = "V library animation test -- Riley Rainey";

#define STRING	"Double Buffered Animation"
#define BORDER	1
#define FONT	"fixed"
#define	ARG_FONT		"font"
#define	ARG_BORDER_COLOR	"borderColor"
#define	ARG_BACKGROUND		"background"
#define ARG_BORDER		"borderWidth"
#define	ARG_GEOMETRY		"geometry"
#define DEFAULT_BACKGROUND	"#29350B"
#define DEFAULT_BORDER		"black"

#define SW_BORDER	1
#define SW_BG		3
#define SW_GEOM		4

struct {
	char	*sw;
	int	value;
	}	switches[] = {
	"-bw", SW_BORDER,
	"-bg", SW_BG,
	"-geometry", SW_GEOM,
	NULL, 0}, *swp;

char *background = NULL, *filename;
int  borderWidth = 1;
int  mono;


/*
 * This structure forms the WM_HINTS property of the window,
 * letting the window manager know how to handle this window.
 * See Section 9.1 of the Xlib manual.
 */
XWMHints	xwmh = {
    (InputHint|StateHint),	/* flags */
    False,			/* input */
    NormalState,		/* initial_state */
    0,				/* icon pixmap */
    0,				/* icon window */
    0, 0,			/* icon location */
    0,				/* icon mask */
    0,				/* Window group */
};

Display		*dpy;		/* X server connection */
Window		win;		/* Window ID */
GC		gc;		/* GC to draw with */
XGCValues	gcv;
XSizeHints	xsh;		/* Size hints for window manager */
Colormap	cmap;

static unsigned long	planemask [6];	/* plane masks from XAllocColorCells */
static unsigned long   pixels [16];	/* pixel values from XAllocColorCells */
GC		curGC;

main(argc,argv)
    int argc;
    char **argv;
{
    char       *fontName;	/* Name of font for string */
    XFontStruct *fontstruct;	/* Font descriptor */
    unsigned long ftw, fth, pad;/* Font size parameters */
    unsigned long bg, bd;	/* Pixel values */
    unsigned long bw;		/* Border width */
    char       *tempstr;	/* Temporary string */
    XColor      color;		/* Temporary color */
    char       *geomSpec = NULL;/* Window geometry string */
    XSetWindowAttributes xswa;	/* Temporary Set Window Attribute struct */
    char	**c;

/*
 *  Parse command line
 */

    for (c= &argv[1]; *c != (char *) NULL; ++c)
	if (**c == '-') {
	    for (swp= &switches[0]; swp->value != 0; ++swp)
		if (strcmp (swp->sw, *c) == 0) {
			switch (swp->value) {
			case SW_GEOM: 
				geomSpec = *(++c);
				break;
			case SW_BG:
				background = *(++c);
				break;
			case SW_BORDER:
				borderWidth = atoi (*(++c));
				break;
			}
		        break;
		}
	    if (swp->value == 0) {
		fprintf (stderr, "%s: invalid switch %s", argv[0], *c);
		exit (1);
	    }
	}
	else
	    filename = *c;

    /*
     * Open the display using the $DISPLAY environment variable to locate
     * the X server.  See Section 2.1.
     */
    if ((dpy = XOpenDisplay(NULL)) == NULL) {
	fprintf(stderr, "%s: can't open %s\n", argv[0], XDisplayName(NULL));
	exit(1);
    }

    mono = (DisplayPlanes(dpy, 0) < 2) ? 1 : 0;

    /*
     * Load the font to use.  See Sections 10.2 & 6.5.1
     */
    if ((fontName = XGetDefault(dpy, argv[0], ARG_FONT)) == NULL) {
	fontName = FONT;
    }
    if ((fontstruct = XLoadQueryFont(dpy, fontName)) == NULL) {
	fprintf(stderr, "%s: display %s doesn't know font %s\n",
		argv[0], DisplayString(dpy), fontName);
	exit(1);
    }
    fth = fontstruct->max_bounds.ascent + fontstruct->max_bounds.descent;
    ftw = fontstruct->max_bounds.width;

    /*
     * Select colors for the border,  the window background,  and the
     * foreground.  We use the default colormap to allocate the colors in.
     * See Sections 2.2.1, 5.1.2, & 10.4.
     */

    cmap = DefaultColormap(dpy, DefaultScreen(dpy));

    if ((tempstr = XGetDefault(dpy, argv[0], ARG_BORDER_COLOR)) == NULL)
	tempstr = DEFAULT_BORDER;
    if (XParseColor(dpy, cmap, tempstr, &color) == 0) {
	fprintf (stderr, "Can't get border color %s\n", tempstr);
	exit (1);
    }
	bd = color.pixel;

/*
 *  Allocate Color Planes and one extra cell for the border color.
 */

    if (mono == 0)
    if (XAllocColorCells (dpy, cmap, False, planemask, 0, pixels, 1) == 0) {
	fprintf (stderr, "Cannot allocate color cells\n");
	exit (1);
    }
    bg = WhitePixel (dpy, 0);

/*
 *  Store border color
 */

    if (mono)
	bd = BlackPixel (dpy, 0);
    else {
    	bd = color.pixel = pixels [0];
    	XStoreColor (dpy, cmap, &color);
    }

/*
 *  Background Color (pixels [0] defines this).
 */

    if (background == NULL)
	background = XGetDefault (dpy, argv[0], ARG_BACKGROUND);
    if (background == NULL)
	background = DEFAULT_BACKGROUND;

    /*
     * Set the border width of the window,  and the gap between the text
     * and the edge of the window, "pad".
     */
    pad = BORDER;
    if ((tempstr = XGetDefault(dpy, argv[0], ARG_BORDER)) == NULL)
	bw = 1;
    else
	bw = atoi(tempstr);

    /*
     * Deal with providing the window with an initial position & size.
     * Fill out the XSizeHints struct to inform the window manager. See
     * Sections 9.1.6 & 10.3.
     */
    if (geomSpec == NULL)
        geomSpec = XGetDefault(dpy, argv[0], ARG_GEOMETRY);

    if (geomSpec == NULL) {
	/*
	 * The defaults database doesn't contain a specification of the
	 * initial size & position - fit the window to the text and locate
	 * it in the center of the screen.
	 */
	xsh.flags = (PPosition | PSize);
	xsh.height = 800;
	xsh.width = 1000;
	xsh.x = (DisplayWidth(dpy, DefaultScreen(dpy)) - xsh.width) / 2;
	xsh.y = (DisplayHeight(dpy, DefaultScreen(dpy)) - xsh.height) / 2;
    }
    else {
	int         bitmask;

	bzero(&xsh, sizeof(xsh));
	bitmask = XGeometry(dpy, DefaultScreen(dpy), geomSpec, geomSpec,
			    bw, ftw, fth, pad, pad, &(xsh.x), &(xsh.y),
			    &(xsh.width), &(xsh.height));
	if (bitmask & (XValue | YValue)) {
	    xsh.flags |= USPosition;
	}
	if (bitmask & (WidthValue | HeightValue)) {
	    xsh.flags |= USSize;
	}
    }

    /*
     * Create the Window with the information in the XSizeHints, the
     * border width,  and the border & background pixels. See Section 3.3.
     */
    win = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy),
			      xsh.x, xsh.y, xsh.width, xsh.height,
			      bw, bd, bg);

    /*
     * Set the standard properties for the window managers. See Section
     * 9.1.
     */
    XSetStandardProperties(dpy, win, STRING, STRING, None, argv, argc, &xsh);
    XSetWMHints(dpy, win, &xwmh);

    /*
     * Ensure that the window's colormap field points to the default
     * colormap,  so that the window manager knows the correct colormap to
     * use for the window.  See Section 3.2.9. Also,  set the window's Bit
     * Gravity to reduce Expose events.
     */
    xswa.colormap = DefaultColormap(dpy, DefaultScreen(dpy));
    xswa.bit_gravity = NorthWestGravity;
    XChangeWindowAttributes(dpy, win, (CWColormap | CWBitGravity), &xswa);

    /*
     * Create the GC for drawing the picture.
     */
    gcv.font = fontstruct->fid;
    curGC = XCreateGC(dpy, win, GCFont, &gcv);

    /*
     * Specify the event types we're interested in - only Exposures.  See
     * Sections 8.5 & 8.4.5.1
     */
    XSelectInput(dpy, win, ExposureMask);

    /*
     * Map the window to make it visible.  See Section 3.5.
     */
    XMapWindow(dpy, win);

    app(background);

    exit(1);
}

