/*
 *	acm : an aerial combat simulator for X
 *
 *	Written by Riley Rainey,  riley@mips.com
 *
 *	Permission to use, copy, modify and distribute (without charge) this
 *	software, documentation, images, etc. is granted, provided that this 
 *	comment and the author's name is retained.
 *
 */
 
#include "pm.h"
#include <stdio.h>
#include <X11/Xutil.h>
#ifndef hpux
#include <X11/Xos.h>
#endif
#include <X11/cursorfont.h>

#ifdef sgi
#undef index
#endif

extern void resizePlayerWindow ();
extern void newExplosion ();

#define BORDER	1

#if defined(dec) || defined(Dec)
#define FONT			"courier_18"
#else
#if defined(SunOSPlatform) || defined(Sun) || defined(sun)
#define FONT			"9x15"
#else
#define FONT			"*courier-bold-r-normal--17*"
#endif
#endif

#define	ARG_FONT		"font"
#define ARG_RADAR_FONT		"radarFont"
#define	ARG_BORDER_COLOR	"borderColor"
#define ARG_BORDER		"borderWidth"
#define	ARG_GEOMETRY		"geometry"
#define DEFAULT_BACKGROUND	"#93bde4"	/* my version of sky blue */
#define DEFAULT_BORDER		"black"
#define DEFAULT_RADAR_FONT	"fixed"

#define SW_BORDER	1
#define SW_BACKGROUND	2
#define SW_HUDFONT	3
#define SW_GEOM		4
#define SW_RADARFONT	5
#define SW_TEAM		6
#define SW_OBSERVE	7
#define SW_DEFAULT_VISUAL 8

struct {
	char	*sw;
	int	value;
	}	swt[] = {
	"-bw", SW_BORDER,
	"-skycolor", SW_BACKGROUND,
	"-hudfont",  SW_HUDFONT,
	"-radarfont",SW_RADARFONT,
	"-geometry", SW_GEOM,
	"-team",     SW_TEAM,
	"-chase",    SW_OBSERVE,
	"-dv",	     SW_DEFAULT_VISUAL,
	NULL, 0}, *swp;

void	recoverAcmArgv (args, argc, argv)
char	*args;
int	*argc;
char	*argv[]; {

	char *s;

	argv[0] = ACM;
	argv[1] = args;

	if (*args == '\0') {
		*argc = 1;
		argv[1] = (char *) NULL;
		return;
	}
	else
		*argc = 2;

	for (s=args; *s;) {
		if (*s == '|') {
			*s = '\0';
			argv[(*argc)++] = ++s;
		}
		else
			++s;
	}

	argv[*argc] = (char *) NULL;
}

Visual *get_pseudo_visual (display, screen)
Display *display;
long	screen; {

	XVisualInfo	vTemplate;
	XVisualInfo	*visualList;
	int		i, visualsMatched;

/*
 *  Get all Visuals with a depth of 8 on this screen
 */

	vTemplate.screen = screen;
	vTemplate.depth = 8;
	visualList = XGetVisualInfo (display, VisualScreenMask |
		VisualDepthMask, &vTemplate, &visualsMatched);

	if (visualsMatched == 0)
		return NULL;

/*
 *  Return the first PseudoColor Visual found.
 */

	for (i=0; i<visualsMatched; ++i) {
		if (visualList[i].visual->class == PseudoColor)
			return visualList[i].visual;
	}

	return NULL;
}

int	newPlayer(s, display, logname, switches)
int	s;
char	*display;
char	*logname;
char	*switches; {

    char       *fontName;	/* Name of font for string */
    XSizeHints	xsh;		/* Size hints for window manager */
    Colormap	cmap;
    GC		curGC;
    XGCValues	gcv;
    unsigned long pad;		/* Font size parameters */
    unsigned long bd;		/* Pixel values */
    unsigned long bw;		/* Border width */
    char       *tempstr;	/* Temporary string */
    XColor      color;		/* Temporary color */
    char       *geomSpec;	/* Window geometry string */
    XWMHints	xwmh;		/* Window manager hints */
    Cursor	cursor;
    char	**c;
    char	err[64];
    static char	*background = NULL;
    int		borderWidth = -1;
    int		player;
    viewer	*u;
    craft	*cf;
    int		argc;
    char	*argv[32];
    int		screen;
    char	*hudfont = NULL, *radarfont = NULL;
    int		team = 1;
    char	*plane;		/* name of plane type */
    int		observer = -1;
    int		width, height;	/* dimensions of main window */
    double	scale;
    int		depth, win_attr_mask;
    Visual	*theVisual;
    XSetWindowAttributes window_attributes;
    int		useDefaultVisual = 0;

    recoverAcmArgv (switches, &argc, argv);

    geomSpec = NULL;
    u = (viewer *) malloc (sizeof(viewer));

/*
 *  Parse command line
 */

    for (c = &argv[1]; *c != (char *) NULL; ++c)
	if (**c == '-') {
	    for (swp = &swt[0]; swp->value != 0; ++swp)
		if (strcmp (swp->sw, *c) == 0) {

			switch (swp->value) {

			case SW_GEOM: 
				geomSpec = *(++c);
				break;

			case SW_BORDER:
				borderWidth = atoi (*(++c));
				break;

			case SW_BACKGROUND:
				background = *(++c);
				break;

			case SW_HUDFONT:
				hudfont = *(++c);
				break;

			case SW_RADARFONT:
				radarfont = *(++c);
				break;

			case SW_TEAM:
				team = atoi (*(++c));
				break;

			case SW_OBSERVE:
				observer = atoi (*(++c));
				break;

			case SW_DEFAULT_VISUAL:
				useDefaultVisual = 1;
				break;
			}
		        break;
		}
	    if (swp->value == 0) {
		free ((char *) u);
		sprintf (err, "%s: invalid switch %s", ACM, *c);
		write (s, err, strlen(err));
		return -1;
	    }
	}

    if (team == 1)
	plane = "f-16c";
    else
	plane = "mig-23";

    if ((player = newPlane (plane)) < 0) {
	sprintf (err, "Sorry, acm is popular -- no room for \
any more players at this moment.\n");
	write (s, err, strlen(err));
	return -1;
    }

/*
 *  assign a (kludged) team location.
 */

    if (team == 2) {
	ptbl[player].Sg.x = -0.6 * NM + 110.0;
	ptbl[player].Sg.y = 49.0 * NM;
    }
    else
	team = 1;

    if ((u->dpy = XOpenDisplay(display)) == (Display *) NULL) {
	free ((char *) u);
	ptbl[player].type = CT_FREE;
	sprintf(err, "%s: can't open %s\n", ACM, display);
	write (s, err, strlen(err));
	return -1;
    }
    screen = DefaultScreen (u->dpy);

    if (hudfont)
	 fontName = hudfont;
    else if ((fontName = XGetDefault(u->dpy, ACM, ARG_FONT)) == NULL) {
	fontName = FONT;
    }
    if ((u->font = XLoadQueryFont(u->dpy, fontName)) == NULL) {
	XCloseDisplay (u->dpy);
	free ((char *) u);
	ptbl[player].type = CT_FREE;
	sprintf(err, "%s: display %s doesn't know font %s\n",
		ACM, display, fontName);
	write (s, err, strlen(err));
	return -1;
    }
    u->fth = u->font->max_bounds.ascent + u->font->max_bounds.descent;
    u->ftw = u->font->max_bounds.width;

    if (radarfont)
	 fontName = radarfont;
    else if ((fontName = XGetDefault(u->dpy, ACM, ARG_RADAR_FONT)) == NULL) {
	fontName = DEFAULT_RADAR_FONT;
    }
    if ((u->rfont = XLoadQueryFont(u->dpy, fontName)) == NULL) {
	XCloseDisplay (u->dpy);
	free ((char *) u);
	ptbl[player].type = CT_FREE;
	sprintf(err, "%s: display %s doesn't know font %s\n",
		ACM, display, fontName);
	write (s, err, strlen(err));
	return -1;
    }
    u->rfth = u->rfont->max_bounds.ascent + u->rfont->max_bounds.descent;
    u->rftw = u->rfont->max_bounds.width;

/*
 *  If the player has specified that the want the default Visual, simply
 *  give 'em that along with the default Colormap.
 */

    if (useDefaultVisual) {

	theVisual = DefaultVisual (u->dpy, screen);
	cmap = DefaultColormap (u->dpy, screen);
	depth = DisplayPlanes (u->dpy, screen);

    }

/*
 *  Look for a visual with a depth of eight; if we can't find one,
 *  fall back to monochrome mode.
 */

    else {
	if ((theVisual = get_pseudo_visual (u->dpy, screen)) == NULL) {
		theVisual = DefaultVisual (u->dpy, screen);
		cmap = DefaultColormap (u->dpy, screen);
		depth = 1;
	}
	else {
		cmap = XCreateColormap (u->dpy, RootWindow(u->dpy, screen),
			theVisual, AllocNone);
		depth = 8;
	}
    }

/*
 * Select colors for the border, the window background, and the
 * foreground.  We use the default colormap to allocate the colors in.
 */

    if (background == NULL)
	background = DEFAULT_BACKGROUND;

    if ((tempstr = XGetDefault(u->dpy, ACM, ARG_BORDER_COLOR)) == NULL)
	tempstr = DEFAULT_BORDER;
    if (XParseColor(u->dpy, cmap, tempstr, &color) == 0) {
	XCloseDisplay (u->dpy);
	free ((char *) u);
	ptbl[player].type = CT_FREE;
	sprintf (err, "Can't get border color %s\n", tempstr);
	write (s, err, strlen(err));
	return -1;
    }

    if (depth == 1)
	bd = BlackPixel (u->dpy, screen);
    else {
        if (XAllocColor(u->dpy, cmap, &color) == 0) {
	    XCloseDisplay (u->dpy);
	    free ((char *) u);
	    ptbl[player].type = CT_FREE;
	    sprintf (err, "Cannot allocate color cells\n");
	    write (s, err, strlen(err));
	    return -1;
	}
	bd = color.pixel;
    }

/*
 * Set the border width of the window, and the gap between the text
 * and the edge of the window, "pad".
 */

    pad = BORDER;
    if (borderWidth >= 0)
	bw = borderWidth;
    else if ((tempstr = XGetDefault(u->dpy, ACM, ARG_BORDER)) == NULL)
	bw = 1;
    else
	bw = atoi(tempstr);

/*
 * Deal with providing the window with an initial position & size.
 * Fill out the XSizeHints struct to inform the window manager.
 */

    if (geomSpec == NULL)
        geomSpec = XGetDefault(u->dpy, ACM, ARG_GEOMETRY);

/*
 * If the defaults database doesn't contain a specification of the
 * initial size & position - fit the window to the text and locate
 * it in the center of the screen.
 */

    if (geomSpec == NULL) {
	xsh.flags = PPosition | PSize;
	xsh.height = FS_WINDOW_HEIGHT;
	xsh.width =  FS_WINDOW_WIDTH;
	xsh.x = (DisplayWidth(u->dpy, screen) - xsh.width) / 2;
	xsh.y = (DisplayHeight(u->dpy, screen) - xsh.height) / 2;
    }
    else {
	int         bitmask;

	bzero((char *) &xsh, sizeof(xsh));
	bitmask = XGeometry(u->dpy, screen, geomSpec, geomSpec,
			    bw, u->ftw, u->fth, pad, pad, &(xsh.x), &(xsh.y),
			    &(xsh.width), &(xsh.height));
	if (bitmask & (XValue | YValue)) {
	    xsh.flags |= USPosition;
	}
	if (bitmask & (WidthValue | HeightValue)) {
	    xsh.flags |= USSize;
	}
    }

    width = xsh.width;
    height = xsh.height;

/*
 * Create the Window with the information in the XSizeHints, the
 * border width,  and the border & background pixels.
 */

    win_attr_mask = CWColormap | CWBitGravity;
    window_attributes.colormap = cmap;
    window_attributes.bit_gravity = NorthWestGravity;

    u->win = XCreateWindow (u->dpy, RootWindow (u->dpy, screen),
		xsh.x, xsh.y, xsh.width, xsh.height, bw, depth,
		InputOutput, theVisual, win_attr_mask, &window_attributes);

    scale = (double) xsh.width / (double) FS_WINDOW_WIDTH;

/*
 * Create a pixmap of the engine RPM gauge and flap indicators.
 */

    u->eng = XCreateBitmapFromData (u->dpy, u->win, eng_bits, eng_width,
	eng_height);
    u->flap[0] = XCreateBitmapFromData (u->dpy, u->win, flaps0_bits,
	flaps0_width, flaps0_height);
    u->flap[1] = XCreateBitmapFromData (u->dpy, u->win, flaps1_bits,
	flaps1_width, flaps1_height);
    u->flap[2] = XCreateBitmapFromData (u->dpy, u->win, flaps2_bits,
	flaps2_width, flaps2_height);
    u->flap[3] = XCreateBitmapFromData (u->dpy, u->win, flaps3_bits,
	flaps3_width, flaps3_height);
    u->handle[0] = XCreateBitmapFromData (u->dpy, u->win, handleUp_bits,
	handleUp_width, handleUp_height);
    u->handle[1] = XCreateBitmapFromData (u->dpy, u->win, handleDn_bits,
	handleDn_width, handleDn_height);
    u->gearLight[0] = XCreateBitmapFromData (u->dpy, u->win, gearUp_bits,
	gearUp_width, gearUp_height);
    u->gearLight[1] = XCreateBitmapFromData (u->dpy, u->win, gearTran_bits,
	gearTran_width, gearTran_height);
    u->gearLight[2] = XCreateBitmapFromData (u->dpy, u->win, gearDown_bits,
	gearDown_width, gearDown_height);

/*
 * Set the standard properties and hints for the window managers.
 */

    XSetStandardProperties(u->dpy, u->win, ACM, ACM, None, argv, argc, &xsh);
    xwmh.flags = InputHint | StateHint;
    xwmh.input = True;
    xwmh.initial_state = NormalState;
    XSetWMHints(u->dpy, u->win, &xwmh);
    cursor = XCreateFontCursor (u->dpy, XC_tcross);
    XDefineCursor (u->dpy, u->win, cursor);

    xsh.width = RADAR_WINDOW_WIDTH * scale;
    xsh.height = RADAR_WINDOW_HEIGHT * scale;
    xsh.x = RADAR_X * scale;
    xsh.y = RADAR_Y * scale;
    u->rwin = XCreateWindow (u->dpy, u->win, xsh.x, xsh.y, xsh.width,
	xsh.height, 0, CopyFromParent, CopyFromParent, CopyFromParent,
	0, NULL);

/*
 * Create the GC for drawing the picture.
 */

    gcv.font = u->font->fid;
    gcv.graphics_exposures = False;
    curGC = XCreateGC(u->dpy, u->win, GCFont | GCGraphicsExposures, &gcv);
    XSelectInput(u->dpy, u->win, KeyPressMask | ButtonPressMask |
	StructureNotifyMask | ButtonReleaseMask | ExposureMask);

/*
 *  Fill-in the viewer structure
 */

    cf = &ptbl[player];
    cf->team = team;
    cf->vl = u;
    strncpy (cf->name, logname, sizeof (cf->name));
    strncpy (cf->display, display, sizeof (cf->display));
    u->next = (viewer *) NULL;
    u->gc = curGC;
 
    resizePlayerWindow (c, u, width, height, 1);

#ifdef USE_PIXMAP_ANIMATION
    if (depth != 1)
	usePixmaps = 1;
#endif

    u->v = VOpenViewport (u->dpy, screen, u->win, cmap,
	12.0*25.4/1000.0, 0.5, scale * 0.70, u->width, u->height);

    if (VBindColors (u->v, background) < 0) {
	XCloseDisplay (u->dpy);
	free ((char *) u);
	ptbl[player].type = CT_FREE;
	sprintf (err, "Error in binding colors.\n");
	write (s, err, strlen(err));
	return -1;
    }
									       
/*
 * Map the window to make it visible.
 */

    XMapWindow(u->dpy, u->win);
    XMapWindow(u->dpy, u->rwin);
    if (depth != 1) {
	register int curPixel;

    	VExposeBuffer (u->v, u->gc);
	curPixel = *(u->v->pixel);
	XSetForeground (u->dpy, u->gc, curPixel);
	XFillRectangle (u->dpy, u->win, u->gc, 0, 0, u->width, u->height);
    }

    whitePixel = whiteColor->index;
    blackPixel = blackColor->index;
    HUDPixel = HUDColor->index;

    if (depth == 1)
	XSetWindowBackground (u->dpy, u->win, BlackPixel(u->dpy, screen));
    else
	XSetWindowBackground (u->dpy, u->win, VConstantColor(u->v, blackPixel));

    return 0;

}

int killPlayer (c)
craft	*c; {

	viewer	*v, *vn;
	int	i;

/*
 *  Erase our radar emissions
 */

	for (i=0; i<MAXPLAYERS; ++i)
		ptbl[i].rval[c->index] = 0.0;

/*
 *  Replace the plane with an explosion.
 */

	newExplosion (&(c->Sg), 100.0, 15.0, 4.0);

/*
 *  Free HUD string storage
 */

	if (c->leftHUD[0] != (char *) NULL)
		for (i=0; i<3; ++i) {
			free (c->leftHUD[i]);
			free (c->rightHUD[i]);
		}

/*
 *  Close viewers' display
 */

	for (v=c->vl; v != (viewer *) NULL;) {
		XCloseDisplay (v->dpy);
		vn = (viewer *) v->next;
		free ((char *) v);
		v = vn;
	}

	if (c->flags && FL_RECORD)
		-- recordCount;

	c->type = CT_FREE;
	return 0;
}
