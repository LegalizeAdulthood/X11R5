/*
 * flip -- mess up the screen
 */
/*
 * Copyright 1990 David Lemke and Network Computing Devices
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
 *		@(#)flip.c	1.3	90/03/22
 *
 */

#include	<X11/Xlib.h>
#include	<stdio.h>

#ifdef DEBUG
#define assert(ex)	{if (!(ex)){(void)fprintf(stderr,"Assertion \"ex\" failed: file \"%s\", line %d\n", __FILE__, __LINE__);abort();}}
#else
#define assert(ex)
#endif
#define	rnd(x)	(random() % (x))

/* parameters for random flips */
#define	CHUNK_WIDTH	100
#define	CHUNK_HEIGHT	100

#define	MIN_CHUNK_WIDTH		5
#define	MIN_CHUNK_HEIGHT	5

/* number of times to do random flipping */
#define	NUM_TIMES	300
#define	MIN_TIMES	20

#define	GLOAT_TIME	3	/* time in seconds to show the results if
				 * in fast mode
				 */

Display	*dpy;
Window	win;
Pixmap	pmap;
GC	copygc;
int	screen;
Bool	do_vert = False;	/* flip top-to-bottom */
Bool	do_fast = False;	/* show results all at once */
Bool	do_random = False;	/* do small random pieces instead of 
					full screen */
Bool	do_planes = False;	/* do a single plane at a time instead of all */

int	do_half = 1;		/* divisor for how much to flip.
				 * 1 leaves things the way they started
				 * 2 leaves them flipped
				 */

usage()
{
	fprintf(stderr, "Usage: flip [-planes] [-vert] [-fast | -random] [-times] [-display <displayname>]\n");
	exit(1);
}

main(argc, argv)
int	argc;
char	**argv;
{
char	*display = NULL;
unsigned long	vmask, gcmask;
XSetWindowAttributes	xswat;
XGCValues	gcvals;
int	i;
int	times = -1;
Drawable	drawable;
int	x, y, w, h;

	/* process args */
	for (i = 1; i < argc; i++)	{
		if (strncmp(argv[i], "-dis", 4) == 0)	{
			if (argv[i+1])
				display = argv[++i];
			else
				usage();
		} else if (strncmp(argv[i], "-t", 2) == 0)	{
			if (argv[i+1])
				times = atoi(argv[++i]);
			else
				usage();
		} else if (strncmp(argv[i], "-p", 2) == 0)	{
			do_planes = True;
		} else if (strncmp(argv[i], "-f", 2) == 0)	{
			do_fast = True;
			do_half = 2;
		} else if (strncmp(argv[i], "-r", 2) == 0)	{
			do_random = True;
			if (times < 0)
				times = rnd(NUM_TIMES) + MIN_TIMES;
		} else if (strncmp(argv[i], "-v", 2) == 0)	{
			do_vert = True;
		}	else
			usage();
	}

	if ((dpy = XOpenDisplay(display)) == NULL)	{
		fprintf(stderr, "can't open display\n");
		exit(0);
	}

	screen = DefaultScreen(dpy);

	if (!do_random)	{
		xswat.override_redirect = True;
		xswat.do_not_propagate_mask = KeyPressMask | KeyReleaseMask |
			ButtonPressMask | ButtonReleaseMask;
		vmask = CWOverrideRedirect | CWDontPropagate;
		win = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0, 
			DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			0, CopyFromParent, CopyFromParent, CopyFromParent,
			vmask, &xswat);
	}

	if (do_fast)	{
		pmap = XCreatePixmap(dpy, RootWindow(dpy, screen),
			DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			DisplayPlanes(dpy, screen));
	}

	gcvals.graphics_exposures = False;
	/* copyplane gc wants to leave the data alone */
	gcvals.foreground = 1;
	gcvals.background = 0;
	gcvals.subwindow_mode = IncludeInferiors;
	gcmask = GCForeground | GCBackground | GCGraphicsExposures 
				| GCSubwindowMode;
	copygc = XCreateGC(dpy, RootWindow(dpy, screen), gcmask, &gcvals);

	if (do_fast)	{	/* copy the screen contents */
		XCopyArea(dpy, RootWindow(dpy, screen), pmap, copygc,
			0, 0,
			DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			0, 0);
	} else if (!do_random)	{
		XMapRaised(dpy, win);
	}

	/* make sure everything's done */
	XSync(dpy, 0);

	/* choose what to mess up */
	if (do_fast)
		drawable = pmap;
	else if (do_random)
		drawable = RootWindow(dpy, screen);
	else
		drawable = win;

	srandom(getpid());
	if (do_random)	{
		for (i = 0; i < times; i++)	{
			if (do_planes)
				XSetPlaneMask(dpy, copygc, 
				    (1 << rnd(DisplayPlanes(dpy, screen))));
			x = rnd(DisplayWidth(dpy, screen));
			y = rnd(DisplayHeight(dpy, screen));
			w = rnd(CHUNK_WIDTH) + MIN_CHUNK_WIDTH;
			h = rnd(CHUNK_HEIGHT) + MIN_CHUNK_HEIGHT;

			/* shift over if going off screen */
			if ((x + w) >= DisplayWidth(dpy, screen))	{
				x -= (w + 1);	/* dont get the last line */
			}
			if ((y + h) >= DisplayHeight(dpy, screen))	{
				y -= (h + 1);
			}

			if (rnd(2))
				flip_vert(drawable, x, y, w, h);
			else
				flip_horz(drawable, x, y, w, h);
			XSync(dpy, 0);
		}
		exit (0);
	} else	{
		if (do_planes)
			XSetPlaneMask(dpy, copygc, 
			    (1 << rnd(DisplayPlanes(dpy, screen))));
		if (do_vert)
			flip_vert(drawable, 0, 0, DisplayWidth(dpy, screen) - 1,
				DisplayHeight(dpy, screen) - 1);
		else
			flip_horz(drawable, 0, 0, DisplayWidth(dpy, screen) - 1,
				DisplayHeight(dpy, screen) - 1);
	}

	/* show the dirty work */
	if (do_fast)	{
		XMapWindow(dpy, win);
		XSync(dpy, 0);	/* be sure its mapped before filling it */
		XCopyArea(dpy, pmap, win, copygc,
			0, 0,
			DisplayWidth(dpy, screen), DisplayHeight(dpy, screen),
			0, 0);
	}

	XSync(dpy, 0);

	/* gloat a bit */
	if (do_fast)
		sleep(GLOAT_TIME);

	exit(0);
}

flip_horz(d, x, y, w, h)
Drawable	d;
int	x, y, w, h;
{
Pixmap	temp;
int	i;
int	width = 1;

	assert(h > 0);
	temp = XCreatePixmap(dpy, RootWindow(dpy, screen), 
		width, h, DisplayPlanes(dpy, screen));

	for (i = x; i <= (x + w)/do_half; i+= width)	{
		XCopyArea(dpy, d, temp, copygc, 
			i, y, 
			width, h,
			0, 0);
		XCopyArea(dpy, d, d, copygc, 
			(w + x) - (i - x), y, 
			width, h,
			i, y);
		XCopyArea(dpy, temp, d, copygc, 
			0, 0, 
			width, h,
			(w + x) - (i - x), y);
	}
	XFreePixmap(dpy, temp);
}

flip_vert(d, x, y, w, h)
Drawable	d;
int	x, y, w, h;
{
Pixmap	temp;
int	i;
int	height = 1;

	assert(w > 0);
	temp = XCreatePixmap(dpy, RootWindow(dpy, screen), 
		w, height, DisplayPlanes(dpy, screen));

	for (i = y; i <= (y + h)/do_half; i+= height)	{
		XCopyArea(dpy, d, temp, copygc, 
			x, i,
			w, height,
			0, 0);
		XCopyArea(dpy, d, d, copygc, 
			x, (y + h) - (i - y),
			w, height,
			x, i);
		XCopyArea(dpy, temp, d, copygc, 
			0, 0, 
			w, height,
			x, (y + h) - (i - y));
	}
	XFreePixmap(dpy, temp);
}
