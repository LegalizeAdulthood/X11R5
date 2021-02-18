/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	x.c
 *	sjx window x11 interface
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Mon Jun 27 21:59:16 JST 1988
 */
/*
 * $Header: x.c,v 1.2 91/09/30 21:17:46 root Exp $ SONY;
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include "common.h"
#include "key.h"
#include "sjx.h"
#include "xcommon.h"

#define max(x,y)	((x)>(y) ? (x) : (y))

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN	31
#endif /* MAXHOSTNAMELEN */
char		hostname[MAXHOSTNAMELEN];
char		*localhost = "localhost";

Draws		*cur_draws;
Draws		*def_draws;

extern int	vflag;

static int	main_mapped = 0;
static int	icon_mapped = 0;
static int	main_unmap = 0;
static int	icon_unmap = 0;
static int	wm_map = 0;
static int	keypress = 0;
static int	mono = 0;
static int	top_create = 1;

int		do_iconic = 0;		/* iconic state	*/
int		on_the_screen = 0;

unsigned long	black;
unsigned long	white;

#define DOWN_OFFSET	0

#define FONT_W		8
#define FONT_H		16
#define INIT_X		0
#define INIT_Y		FONT_H - 2;
#define LINE_S		2
#define LINE_H		(FONT_H+LINE_S)

#define MOVE_WIDTH	0
#define MOVE_FUNC	GXxor
#define MOVE_VALUE	0xfd

#define SUB_BD		1		/* subwindow border width	*/
#define I_BORDER	8		/* internal border width	*/
#define GUIDE_LEN	64		/* guide len length		*/
#define TEXT_LINE	3

#define TXT_X		I_BORDER
#define TXT_Y		I_BORDER
#define TXT_WIDTH	(FONT_W * GUIDE_LEN)
#define TXT_HEIGHT	(LINE_H * TEXT_LINE)

#define SUB_X		I_BORDER
#define SUB_Y		(TXT_HEIGHT + (I_BORDER + SUB_BD) * 2)
#define SUB_WIDTH	(FONT_W * GUIDE_LEN)
#define SUB_HEIGHT	FONT_H

#define TOP_X		0
#define TOP_Y		0
#define TOP_WIDTH	(SUB_WIDTH + ((SUB_BD + I_BORDER)*2))
#define TOP_HEIGHT	(TXT_HEIGHT + SUB_HEIGHT + (SUB_BD*4) + (I_BORDER*3))

#define FONT	"-*-fixed-*-r-normal--16-120-100-100-*-*-jisx0201.1976-0"
#define JFONT	"-*-fixed-*-r-normal--16-120-100-100-*-*-jisx0208.1983-0"
#define BWIDTH		1

static wininfo		top = {TOP_X, TOP_Y, TOP_WIDTH, TOP_HEIGHT};
static wininfo		sub = {SUB_X, SUB_Y, SUB_WIDTH, SUB_HEIGHT};
static wininfo		txt = {TXT_X, TXT_Y, TXT_WIDTH, TXT_HEIGHT};
static int		sub_bd		= SUB_BD;
static int		i_border	= I_BORDER;
static int		text_line	= TEXT_LINE;

static char		*fn;
static char		*jfn;
static char		*efn = 0;
static char		*fore_color;
static char		*back_color;
static char		*g_fore_color;
static char		*g_back_color;
static char		*bord_color;
static char		*geom;
static char		*t_geom;

int			org_x		= INIT_X;
int			org_y		= INIT_Y;
int			text_x		= INIT_X;
int			text_y		= INIT_Y;
int			f_width		= FONT_W;
int			f_height	= FONT_H;
int			l_space		= LINE_S;
int			l_height	= LINE_H;

error_handle (dpy, ev)
Display		*dpy;
XErrorEvent	*ev;
{
#define ERRLEN	80
	char	error[ERRLEN];

	XGetErrorText (dpy, ev->error_code, error, ERRLEN);
	if (vflag > 1) {
	fprintf (stderr, "sjx: X Error %s\n", error);
	fprintf (stderr, "             resourceid 0x%x\n", ev->resourceid);
	fprintf (stderr, "             serial     %d\n",   ev->serial);
	fprintf (stderr, "             major      %d\n",   ev->request_code);
	fprintf (stderr, "             minor      %d\n",   ev->minor_code);
	}
}

fatal_error (dpy)
Display		*dpy;
{
	fprintf (stderr, "sjx: X Fatal Error\n");
	exit (1);
}

Usage ()
{
	printf ("Sony Kana-Kanji Conversion Server on X Window System\n");
	printf ("%s (X11/SJ3)\n", SJX_VERSION);
	printf ("Copyright (c) 1988 1989 1990 1991 Sony Corporation\n");
	printf ("        -fn        font name\n");
	printf ("        -jfn       japanese font name\n");
	printf ("        -efn       extra char font name\n");
	printf ("        -ecode     extra char start code\n");
	printf ("        -fg        foreground color\n");
	printf ("        -bg        background color\n");
	printf ("        -bd        border color\n");
	printf ("        -bw        border width\n");
	printf ("        -iconic    iconic state\n");
	printf ("        -help      show this\n");
	printf ("        -H host    server host name\n");
	exit (1);
}

InitArgv (argc, argv)
int		argc;
char		**argv;
{
	extern int	Direct;

	argv ++; argc --;
	while (argc --) {
		if (strcmp ("-help", *argv) == 0) {
			Usage ();
		}
		else if (strcmp ("-d", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			display = *argv;
		}
		else if (strcmp ("-display", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			display = *argv;
		}
		else if (strcmp ("-fn", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			fn = *argv;
		}
		else if (strcmp ("-jfn", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			jfn = *argv;
		}
		else if (strcmp ("-efn", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			efn = *argv;
		}
		else if (strcmp ("-ecode", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			if (set_ecode (*argv)) Usage;
		}
		else if (strcmp ("-fg", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			fore_color = *argv;
		}
		else if (strcmp ("-bg", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			back_color = *argv;
		}
		else if (strcmp ("-gfg", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			g_fore_color = *argv;
		}
		else if (strcmp ("-gbg", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			g_back_color = *argv;
		}
		else if (strcmp ("-bd", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			bord_color = *argv;
		}
		else if (strcmp ("-bw", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			bwidth = atoi (*argv);
		}
		else if (strcmp ("-geometry", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			geom = *argv;
		}
		else if (strcmp ("-g", *argv) == 0) {
			argv ++; argc --; if (argc < 0) Usage ();
			geom = *argv;
		}
		else if (strcmp ("-iconic", *argv) == 0) {
			do_iconic = 1;
		}
		else if (strcmp ("-mono", *argv) == 0) {
			mono ++;
		}
		else if (strcmp ("-H", *argv) == 0) {
			argv++; argc--; if (argc < 0) Usage ();
			setsjserv (*argv);
		}
		else {
			Usage ();
		}
		argv ++;
	}
}

InitDefault (dpy, argc, argv)
Display		*dpy;
int		argc;
char		**argv;
{
	char		*XGetDefault ();
	Colormap	cmap;
	XColor		color;
	char		*file, *tmp;

	if (!fn)
		fn = XGetDefault (dpy, argv[0], "font");
	if (!fn)
		fn = FONT;
	if (!jfn)
		jfn = XGetDefault (dpy, argv[0], "japaneseFont");
	if (!jfn)
		jfn = JFONT;
	if (!efn)
		efn = XGetDefault (dpy, argv[0], "extraFont");

	/* alloc color entry	*/

	cmap = DefaultColormap (dpy, screen);

	if (!mono && (fore_color ||
	    (fore_color = XGetDefault (dpy, argv[0], "foreground")))) {
		if (XParseColor (dpy, cmap, fore_color, &color) &&
		    XAllocColor (dpy, cmap, &color)) {
			fore = color.pixel;
		} else {
			fore = black;
		}
	} else {
		fore = black;
	}
	if (!mono && (back_color ||
	    (back_color = XGetDefault (dpy, argv[0], "background")))) {
		if (XParseColor (dpy, cmap, back_color, &color) &&
		    XAllocColor (dpy, cmap, &color)) {
			back = color.pixel;
		} else {
			back = white;
		}
	} else {
		back = white;
	}
	if (!mono && (g_fore_color ||
	    (g_fore_color = XGetDefault (dpy, argv[0], "guideFore")))) {
		if (XParseColor (dpy, cmap, g_fore_color, &color) &&
		    XAllocColor (dpy, cmap, &color)) {
			g_fore = color.pixel;
		} else {
			g_fore = fore;
		}
	} else {
		g_fore = fore;
	}
	if (!mono && (g_back_color ||
	    (g_back_color = XGetDefault (dpy, argv[0], "guideBack")))) {
		if (XParseColor (dpy, cmap, g_back_color, &color) &&
		    XAllocColor (dpy, cmap, &color)) {
			g_back = color.pixel;
		} else {
			g_back = back;
		}
	} else {
		g_back = back;
	}
	if (!mono && (bord_color ||
	    (bord_color = XGetDefault (dpy, argv[0], "borderColor")))) {
		if (XParseColor (dpy, cmap, bord_color, &color) &&
		    XAllocColor (dpy, cmap, &color)) {
			bord = color.pixel;
		} else {
			bord = black;
		}
	} else {
		bord = black;
	}

	/* set border width */
	if (bwidth < 0 &&
	    (((tmp = XGetDefault (dpy, argv[0], "borderWidth")) == (char *)0)
	    || ((bwidth = atoi(tmp)) < 0))) {
		bwidth = BWIDTH;
	}
}

set_font_info (gc)
GC	gc;
{
	XFontStruct	*fs;

	fs = XQueryFont (dpy, XGContextFromGC (gc));
	f_width = fs->max_bounds.width / 2;
	f_height = fs->ascent + fs->descent;
	l_height = f_height + l_space;
	text_y = org_y = fs->ascent;
	if (vflag > 3) {
		fprintf (stderr, "XFontStruct 0x%x\n", fs);
		fprintf (stderr, "fs->max_bounds.width %d\n",
			fs->max_bounds.width);
		fprintf (stderr, "fs->ascent %d\n",
			fs->ascent);
		fprintf (stderr, "fs->descent %d\n",
			fs->descent);
		fprintf (stderr, "font height %d\n", f_height);
	}
}

guide_length ()
{
	extern char	*Mode_Zhira, *Gmode;

	return (strlen (Mode_Zhira) + strlen (Gmode));
}

resize_wininfo (create_top)
int	create_top;
{
	sub.width = f_width * guide_length ();
	sub.height = f_height;
	txt.width = sub.width;
	txt.height = l_height * text_line;
	top.width = sub.width + i_border * 2 + sub_bd * 2;
	top.height = sub.height + txt.height + i_border * 3 + sub_bd * 4;
	if (create_top) {
		top.x = (DisplayWidth (dpy, screen) - top.width) / 2;
		top.y = DisplayHeight (dpy, screen)
			- top.height - DOWN_OFFSET - bwidth;
		sub.x = i_border;
		sub.y = txt.height + i_border * 2 + sub_bd * 2;
		txt.x = i_border;
		txt.y = i_border;
	}
	else {
		sub.x = (DisplayWidth (dpy, screen) - sub.width) / 2;
		sub.y = DisplayHeight (dpy, screen)
			- sub.height - DOWN_OFFSET - sub_bd;
		txt.x = sub.x;
		txt.y = sub.y - txt.height - i_border - sub_bd * 2;
	}
}

set_resize_hint (win_info, xsh, geom)
wininfo		*win_info;
XSizeHints	*xsh;
char		*geom;
{
	int		result;

	xsh->x = win_info->x;
	xsh->y = win_info->y;
	xsh->width = win_info->width;
	xsh->height = win_info->height;
	xsh->flags = (USSize | USPosition);

	result = NoValue;
	if (geom) {
		result = XParseGeometry (
				geom, &xsh->x, &xsh->y,
				&xsh->width, &xsh->height
			);
		if ((result & WidthValue) && (result & HeightValue))
			xsh->flags |= USSize;
		if ((result & XValue) && (result & YValue))
			xsh->flags |= USPosition;

		if (result & XValue) {
			win_info->x = xsh->x;
			xsh->flags |= USPosition;
		}
		if (result & YValue) {
			win_info->y = xsh->y;
			xsh->flags |= USPosition;
		}
		if (result & WidthValue) {
			win_info->width = xsh->width;
			xsh->flags |= USSize;
		}
		if (result & HeightValue) {
			win_info->height = xsh->height;
			xsh->flags |= USSize;
		}
	}
}

set_host_name (dpy, win, name)
Display	*dpy;
Window	win;
char	*name;
{
	XChangeProperty (
		dpy, win, XA_WM_CLIENT_MACHINE, XA_STRING, 8,
		PropModeReplace, (char *) name, strlen (name)
	);
}

Font		font, jfont;
Font		efont;

X_init (argc, argv)
int	argc;
char	**argv;
{
	Cursor		cursor;
	int		x, y, w, h;
	int		y_off;
	char		*dsp;
	GC		CreateGC (), CreateMoveGC ();
	unsigned long	win_mask;
	Window		parent;
	XClassHint	class_hints;
	extern char	*prog_name;

	/*
	 *	Initialize 1
	 *	Initialize 2
	 */
	display = XDisplayName (display);
	gethostname (hostname, MAXHOSTNAMELEN);
	if (*hostname == '\0')
		strcpy (hostname, localhost);
#ifdef PROTO1
	if (CheckRemote (display)) {
		if (vflag > 3)
			fprintf (stderr, "(remote display %s)\n", display);
	}
#endif /* PROTO1 */
	if ((dpy = XOpenDisplay (display)) == (Display *)0) {
		fprintf (stderr, "openning display");
		fprintf (stderr, " (%s)\n", display);
		exit (1);
	}
	screen = DefaultScreen (dpy);
	depth = DefaultDepth (dpy, screen);
	black = BlackPixel (dpy, screen);
	white = WhitePixel (dpy, screen);
	cursor = XCreateFontCursor (dpy, XC_top_left_arrow);
	root = DefaultRootWindow (dpy);
	root0 = RootWindow (dpy, 0);	/* screen 0 */
	visual = DefaultVisual (dpy, screen);

	/*
	 *	Initialize 3
	 */
	InitDefault (dpy, argc, argv);

	/*
	 *	Initialize 4
	 *	Create and Set Top Window
	 */

	win_mask = 0;
	win_mask |= CWCursor;
	win_mask |= CWBorderPixel;
	win_mask |= CWBackPixel;
	attributes.cursor = cursor;
	attributes.border_pixel = bord;
	attributes.background_pixel = back;
	attributes.bit_gravity = NorthWestGravity;
	win_mask |= CWBitGravity;

	resize_wininfo (top_create);
	x = top.x;
	y = top.y;
	w = top.width;
	h = top.height;
	topwin = XCreateWindow (
			dpy, root,
			x, y, w, h, bwidth,
			CopyFromParent, InputOutput, CopyFromParent,
			win_mask, &attributes
		);
	if (top_create)
		parent = topwin;
	else
		parent = root;

	/*
	 *	Create GC
	 */
	{
		char	**fn_name;
		int	fn_count;

		fn_name = XListFonts (dpy, fn, 1, &fn_count);
		XFreeFontNames (fn_name);
		if (fn_count <= 0) {
			if (vflag > 3)
				fprintf (stderr, "can't open font %s\n", fn);
			exit (1);
		}
	}
	font  = XLoadFont (dpy, fn);
	{
		char	**fn_name;
		int	fn_count;

		fn_name = XListFonts (dpy, jfn, 1, &fn_count);
		XFreeFontNames (fn_name);
		if (fn_count <= 0) {
			if (fn_count <= 0) {
				fprintf (stderr, "can't open font %s\n", jfn);
				exit (1);
			}
		}
	}
	jfont = XLoadFont (dpy, jfn);
	if (efn)
		efont = XLoadFont (dpy, efn);
	else
		efont = jfont;

	if (vflag > 3) {
		fprintf (stderr, "font  %s\n", fn);
		fprintf (stderr, "mfont %s\n", jfn);
	}
	gcTFore = CreateGC (fore, back, font);
	gcTBack = CreateGC (back, fore, font);
	gcTJFore = CreateGC (fore, back, jfont);
	gcTJBack = CreateGC (back, fore, jfont);
	gcCFore = CreateGC (fore, back, font);
	gcCBack = CreateGC (back, fore, font);
	gcCJFore = CreateGC (fore, back, jfont);
	gcCJBack = CreateGC (back, fore, jfont);
	gcGFore = CreateGC (g_fore, g_back, font);
	gcGBack = CreateGC (g_back, g_fore, font);
	gcGJFore = CreateGC (g_fore, g_back, jfont);
	gcGJBack = CreateGC (g_back, g_fore, jfont);
	gcTEFore = CreateGC (fore, back, efont);
	gcTEBack = CreateGC (back, fore, efont);
	gcCEFore = CreateGC (fore, back, efont);
	gcCEBack = CreateGC (back, fore, efont);
	gcGEFore = CreateGC (g_fore, g_back, efont);
	gcGEBack = CreateGC (g_back, g_fore, efont);

	gcMove = CreateMoveGC ();

	/*
	 *	Set Font Information
	 */
	set_font_info (gcTJFore);

	/*
	 *	Resize Window Information
	 *	Reset the top, sub and txt, wininfo structure
	 */
	resize_wininfo (top_create);
	set_resize_hint (&top, &shint, geom);
	ConfigureWindow (topwin, &top);

	/*
	 *	Create GuideLine Subwindow
	 */
	x = sub.x;
	y = sub.y;
	w = sub.width;
	h = sub.height;
	attributes.background_pixel = g_back;
	subwin = XCreateWindow (
			dpy, parent, x, y, w, h, sub_bd,
			CopyFromParent, InputOutput, CopyFromParent,
			win_mask, &attributes
		);

	/*
	 *	Create TextScreen Subwindow
	 */
	x = txt.x;
	y = txt.y;
	w = txt.width;
	h = txt.height;
	attributes.background_pixel = back;
	txtwin = XCreateWindow (
			dpy, parent, x, y, w, h, sub_bd,
			CopyFromParent, InputOutput, CopyFromParent,
			win_mask, &attributes
		);

	/*
	 *	Initial window is a text screen window,
	 *	and set the default text window.
	 */
	win = def_txtwin = txtwin;

	/*
	 *	Set Resize Hints
	 *	Set WM Hints
	 *	Set Normal Hints
	 *	Set Class Hints
	 */
	class_hints.res_name = prog_name;
	class_hints.res_class = prog_name;
	if (top_create) {
		MakeIconPixmap (topwin, do_iconic);
		/*
		set_resize_hint (&top, &shint, geom);
		*/
		XSetNormalHints (dpy, topwin, &shint);
		XSetStandardProperties (
			dpy, topwin, prog_name, prog_name,
			NULL, argv, argc, &shint
		);
		XSetClassHint (dpy, topwin, &class_hints);
		set_host_name (dpy, topwin, hostname);
	}
	else {
		MakeIconPixmap (subwin, do_iconic);

		set_resize_hint (&sub, &shint, geom);
		XSetNormalHints (dpy, subwin, &shint);
		XSetStandardProperties (
			dpy, subwin, prog_name, prog_name,
			NULL, argv, argc, &shint
		);
		XSetClassHint (dpy, subwin, &class_hints);
		set_host_name (dpy, subwin, hostname);

		set_resize_hint (&txt, &shint, t_geom);
		XSetNormalHints (dpy, txtwin, &shint);
		XSetStandardProperties (
			dpy, txtwin, prog_name, prog_name,
			NULL, argv, argc, &shint
		);
		XSetClassHint (dpy, txtwin, &class_hints);
		set_host_name (dpy, txtwin, hostname);
	}

	/*
	 *	Set Event Selection
	 */
#define B_MASK (PropertyChangeMask | ButtonPressMask | ButtonReleaseMask)
#define M_MASK (B_MASK | StructureNotifyMask)
#define C_MASK (StructureNotifyMask | ExposureMask)
#define S_MASK (B_MASK | ExposureMask)

	XSelectInput (dpy, topwin, M_MASK);
	XSelectInput (dpy, subwin, S_MASK);
	XSelectInput (dpy, txtwin, S_MASK);

	/*
	 *	Initialize for selection
	 */
	selection_init ();

	/*
	 *	Initialize Draws
	 */
	InitDraws ();

	/*
	 *	Map Windows
	 */
	if (top_create) {
		XMapSubwindows (dpy, topwin);
		XMapWindow (dpy, topwin);
	}
	else {
		XMapWindow (dpy, subwin);
	}

	/*
	 *	Set Error Handler
	 */
	XSetErrorHandler (error_handle);
	XSetIOErrorHandler (fatal_error);
}

Xdispatch ()
{
	extern int	convert_status;
	extern int	codevalue;
	register int	code_event;

	wm_map = 0;
	code_event = 0;
	keypress = 0;
	while (XPending (dpy) != 0) {
		XNextEvent (dpy, &event);
		switch (event.type) {
			case SelectionClear:
				proc_sclear (&event);
				break;
#ifdef PROTO2
			case ClientMessage:
				proc_clientmsg (&event);
#ifdef sony_news
				keypress ++;
#else /* sony_news */
				if (codevalue == KEY_CONV)
					keypress ++;
#endif /* sony_news */
				break;
			case KeyPress:
				proc_key (&event);
				keypress ++;
				break;
			case DestroyNotify:
				proc_destroy (&event);
				break;
			case PropertyNotify:
				proc_prop (&event);
				if (codevalue == KEY_CONV)
					keypress ++;
				break;
#endif /* PROTO2 */
			case Expose:
				proc_expose (&event);
				break;
			case ButtonPress:
				proc_button (&event);
				break;
			case ButtonRelease:
				proc_button (&event);
				break;
			case MotionNotify:
				proc_motion (&event);
				break;
			case MapNotify:
				proc_map (&event);
				break;
			case UnmapNotify:
				proc_unmap (&event);
				break;
			case ConfigureNotify:
				proc_conf (&event);
				break;
			case CirculateNotify:
			case NoExpose:
				break;
			default:
				break;
		}

		XFlush (dpy);
#ifdef PROTO2
		if (keypress)
			break;
#endif /* PROTO2 */
	}

	if (keypress) {
		return (1);
	}

	return (0);
}

proc_sclear (ev)
XAnyEvent	*ev;
{
	printf ("selection clear event occured.\n\r");
	done2 ();
}

proc_conf (ev)
XConfigureEvent	*ev;
{
	Window	window;

	if (ev->send_event)
		return;
	window = ev->window;
	if (window == subwin) {
		ResetGuideWindow ();
		ConfigureGuideWindow ();
	}
	else if (window == txtwin) {
		ResetTextWindow ();
		ConfigureTextWindow ();
	}
	else if (window == topwin) {
		ResetSubWindows ();
		ConfigureGuideWindow ();
		ConfigureTextWindow ();
	}
}

proc_map (ev)
XMapEvent	*ev;
{
	Window	window;

	window = ev->window;
	if (window == subwin) {
		ResetGuideWindow ();
		ConfigureGuideWindow ();
	}
	else if (window == txtwin) {
		ResetTextWindow ();
		ConfigureTextWindow ();
	}
}

proc_unmap (ev)
XUnmapEvent	*ev;
{
	Window	window;
	Bool	conf;

	window = ev->window;
	conf = ev->from_configure;
	if (!conf) {
		if (window == txtwin) {
			if (main_unmap)
				main_unmap = 0;
		}
	}
}

ResetGuideWindow ()
{
	XWindowAttributes	attr;

	XGetWindowAttributes (dpy, subwin, &attr);
	sub.x = attr.x;
	sub.y = attr.y;
	sub.width = attr.width;
	sub.height = attr.height;
	ResetGuideDrawingSet ();
}

ResetTextWindow ()
{
	XWindowAttributes	attr;

	XGetWindowAttributes (dpy, txtwin, &attr);
	txt.x = attr.x;
	txt.y = attr.y;
	txt.width = attr.width;
	txt.height = attr.height;
	ResetTextDrawingSet ();
}

ResetSubWindows ()
{
	XWindowAttributes	attr;
	int			width, height;

	XGetWindowAttributes (dpy, topwin, &attr);
	width = top.width = attr.width;
	height = top.height = attr.height;
	ResetSub (width, height);
	ResetTxt (width, height);
}

ResetSub (w, h)
int	w, h;
{
	sub.x = i_border;
	sub.y = h - (i_border + sub_bd * 2 + f_height);
	sub.width = w - (i_border + sub_bd) * 2;
	sub.height = f_height;
	SetGuideDrawing (def_draws->g_draw);
}

ResetTxt (w, h)
int	w, h;
{
	txt.x = i_border;
	txt.y = i_border;
	txt.width = w - (i_border + sub_bd) * 2;
	txt.height = h - (i_border * 3 + sub_bd * 4 + sub.height);
	SetTextDrawing (def_draws->t_draw);
}

ConfigureGuideWindow ()
{
	ConfigureWindow (subwin, &sub);
}

ConfigureTextWindow ()
{
	ConfigureWindow (txtwin, &txt);
}

ConfigureWindow (w, i)
Window	w;
wininfo	*i;
{
	XMoveResizeWindow (dpy, w, i->x, i->y, i->width, i->height);
}

proc_expose (ev)
XExposeEvent	*ev;
{
	Window	window;
	int	x, y;
	int	width, height;
	int	count;

	window = ev->window;
	count = ev->count;
	x = ev->x;
	y = ev->y;
	width = ev->width;
	height = ev->height;

	if (count == 0) {
		if (window == subwin) {
			ResetGuideDrawingSet ();
			disp_mode ();
		}
		else if (window == txtwin) {
			ChangeDefaultDS ();
			DrawScreen ();
		}
		else
			DrawInplace (window);
	}
}

proc_button (ev)
XButtonEvent	*ev;
{
	Window	window;

	window = ev->window;
	if (ev->type == ButtonRelease) {
		if (window == topwin || window == txtwin || window == subwin)
			RaiseWindow ();
	}
}

proc_motion (ev)
XMotionEvent	*ev;
{
	Window	window;

	window = ev->window;
}

DrawString (s, len)
u_char	*s;
int	len;
{
	DrawStrings (s, len);
}

static int	push_x, push_y;

GotoSubwin ()
{
	cur_draws->c_draw = cur_draws->g_draw;
	cur_draws->c_draw->text_x = 0;
	cur_draws->c_draw->text_y = cur_draws->c_draw->origin_y;
	SetLineCol (cur_draws->c_draw);
}

GotoTxtwin ()
{
	cur_draws->c_draw = cur_draws->t_draw;
	SetLineCol (cur_draws->c_draw);
}

static Draws	*push_draws;

FreePushDraws (d)
Draws	*d;
{
	if (push_draws == d)
		push_draws = 0;
}

PushDraws (d)
Draws	*d;
{
	push_draws = d;
}

PopGotoTxtwin ()
{
	if (push_draws)
		push_draws->c_draw = push_draws->t_draw;
}

SetDraws (d)
Draws	*d;
{
	PushDraws (cur_draws);
	cur_draws = d;
	SetLineCol (d->c_draw);
}

DefaultDrawingSet ()
{
	cur_draws = def_draws;
}

ResetTextDrawingSet ()
{
	cur_draws = def_draws;
	cur_draws->c_draw = cur_draws->t_draw;
	SetLineCol (cur_draws->c_draw);
}

ResetGuideDrawingSet ()
{
	cur_draws = def_draws;
	cur_draws->c_draw = cur_draws->g_draw;
	SetLineCol (cur_draws->c_draw);
}

static DrawingSet	*d_stak;

PushTextDrawing ()
{
	d_stak = cur_draws->c_draw;
	cur_draws->c_draw = cur_draws->t_draw;
}

PopDrawing ()
{
	cur_draws->c_draw = d_stak;
}

SetLineCol (d)
DrawingSet	*d;
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->line = d->line;
	cv->column = d->column;
}

InitDraws ()
{
	register DrawingSet	*p;
	unsigned long		value;

	if ((def_draws = (Draws *) malloc (sizeof(Draws))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	if ((p = (DrawingSet *) malloc (sizeof(DrawingSet))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	SetGuideDrawing (p);
	def_draws->g_draw = p;
	if ((p = (DrawingSet *) malloc (sizeof(DrawingSet))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	SetTextDrawing (p);
	def_draws->t_draw = p;
	def_draws->c_draw = p;
	cur_draws = def_draws;
	if ((t_gcs = (GCs *) malloc (sizeof(GCs))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	if ((g_gcs = (GCs *) malloc (sizeof(GCs))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	if ((c_gcs = (GCs *) malloc (sizeof(GCs))) == NULL) {
		fprintf (stderr, "sjx: can't alloc memory\n");
		exit (1);
	}
	t_gcs->e_fore_gc = gcTEFore;
	t_gcs->e_back_gc = gcTEBack;
	g_gcs->e_fore_gc = gcGEFore;
	g_gcs->e_back_gc = gcGEBack;
	c_gcs->e_fore_gc = gcCEFore;
	c_gcs->e_back_gc = gcCEBack;
	t_gcs->fore_gc = gcTFore;
	t_gcs->back_gc = gcTBack;
	t_gcs->j_fore_gc = gcTJFore;
	t_gcs->j_back_gc = gcTJBack;
	def_draws->t_draw->gcs = t_gcs;
	g_gcs->fore_gc = gcGFore;
	g_gcs->back_gc = gcGBack;
	g_gcs->j_fore_gc = gcGJFore;
	g_gcs->j_back_gc = gcGJBack;
	def_draws->g_draw->gcs = g_gcs;
	c_gcs->fore_gc = gcCFore;
	c_gcs->back_gc = gcCBack;
	c_gcs->j_fore_gc = gcCJFore;
	c_gcs->j_back_gc = gcCJBack;
}

SetGuideDrawing (d)
register DrawingSet	*d;
{
	d->ex_window = 0;
	d->ex_w = 0;
	d->window = subwin;
	d->origin_x = org_x;
	d->origin_y = org_y;
	d->width = sub.width;
	d->height = sub.height;
	if (d->height < l_height)
		d->height = l_height;
	d->text_x = text_x;
	d->text_y = text_y;
	d->reverse = 0;
	d->under = 0;
	d->line_height = f_height;
	d->font_width = f_width;
	d->x = sub.x;
	d->y = sub.y;
	d->x1 = d->x;
	d->y1 = d->y;
	d->x2 = d->x1 + d->width;
	d->y2 = d->y1 + d->height;
	d->line = d->height / d->line_height;
	d->column = d->width / d->font_width;
	d->mapsw = 0;
	d->unmap = 0;
	d->auto_replace = 0;
}

SetTextDrawing (d)
register DrawingSet	*d;
{
	d->ex_window = 0;
	d->ex_w = 0;
	d->window = txtwin;
	d->origin_x = org_x;
	d->origin_y = org_y;
	d->width = txt.width;
	d->height = txt.height;
	d->text_x = text_x;
	d->text_y = text_y;
	d->reverse = 0;
	d->under = 0;
	d->line_height = l_height;
	d->font_width = f_width;
	d->x = txt.x;
	d->y = txt.y;
	d->x1 = d->x;
	d->y1 = d->y;
	d->x2 = d->x1 + d->width;
	d->y2 = d->y1 + d->height;
	d->line = d->height / d->line_height;
	d->column = d->width / d->font_width;
	d->mapsw = 0;
	d->unmap = 0;
	d->auto_replace = 0;
}

GotoXY (x, y)
int	x, y;
{
	cur_draws->c_draw->text_x = x;
	cur_draws->c_draw->text_y = y;
}

UnmapWindow ()
{
	if (main_mapped) {
		main_mapped = 0;
		main_unmap ++;
	}
#ifdef PROTO2
	All_Unmap ();
#endif /* PROTO2 */
}

MapWindow ()
{
	if (!main_mapped) {
		main_mapped ++;
	}
	RaiseWindow ();
}

RaiseWindow ()
{
	if (top_create) {
		XRaiseWindow (dpy, topwin);
	}
	else {
		XRaiseWindow (dpy, txtwin);
		XRaiseWindow (dpy, subwin);
	}
}

SJ_warnning (s)
char	*s;
{
	printf ("%s", s);
	fflush (stdout);
}

SJ_print (s)
char	*s;
{
	DrawString (s, strlen (s));
	Flush ();
}

SJ_write (s, len)
char	*s;
int	len;
{
	StoreBuff (s, len);
}

SJ_through (s, len, ev)
char		*s;
int		len;
XKeyEvent	*ev;
{
	extern int	input;
#ifdef PROTO1
	extern int	through_status;
#endif /* PROTO1 */
#ifdef PROTO2
	extern Window	inwindow;
#endif /* PROTO2 */

#ifdef PROTO1
	if (input == INPUT_SOCKET) {
		through_status = 1;
		StoreBuff (s, len);
	}
#endif /* PROTO1 */
#ifdef PROTO2
	if (input == INPUT_WINDOW) {
		if (!IsCflag ())
			exec_kettei ();
		flush_to_client ();
		send_key_event (inwindow, ev);
	}
#endif /* PROTO2 */
}

GC
CreateMoveGC ()
{
	XGCValues	gcv;
	unsigned long	mask;

	gcv.line_width = MOVE_WIDTH;
	gcv.function = MOVE_FUNC;
	gcv.foreground = MOVE_VALUE;
	gcv.subwindow_mode = IncludeInferiors;
	mask = GCLineWidth | GCFunction | GCForeground | GCSubwindowMode;
	return (XCreateGC (dpy, root, mask, &gcv));
}

GC
CreateGC (fore, back, font)
unsigned long	fore, back;
Font		font;
{
	XGCValues	gcv;
	unsigned long	mask;

	mask  = GCForeground | GCBackground | GCFont;
	gcv.foreground = fore;
	gcv.background = back;
	gcv.font = font;
	return (XCreateGC (dpy, topwin, mask, &gcv));
}

ChangeGCs (gcs, fore, back, font, jfont, efont)
register GCs		*gcs;
register unsigned long	fore, back;
register Font		font, jfont;
Font			efont;
{
	ChangeGC (gcs->fore_gc, fore, back, font);
	ChangeGC (gcs->back_gc, back, fore, font);
	ChangeGC (gcs->j_fore_gc, fore, back, jfont);
	ChangeGC (gcs->j_back_gc, back, fore, jfont);
	if (efont) {
		ChangeGC (gcs->e_fore_gc, fore, back, efont);
		ChangeGC (gcs->e_back_gc, back, fore, efont);
	}
}

ChangeGC (gc, fore, back, font)
GC		gc;
unsigned long	fore, back;
Font		font;
{
	XGCValues	gcv;
	unsigned long	mask;

	if (font == 0)
		return;
	mask  = GCForeground | GCBackground | GCFont;
	gcv.foreground = fore;
	gcv.background = back;
	gcv.font = font;
	XChangeGC (dpy, gc, mask, &gcv);
}

Window
CreateTextWindow (win, ba, bo, bw, x, y, w, h)
Window		win;
unsigned long	ba, bo;
int		bw;
int		x, y, w, h;
{
	Window			text;
	XSetWindowAttributes	attributes;
	unsigned long		mask;

	/*
	 *	Create TextScreen Subwindow
	 */
	mask = 0;
	attributes.background_pixel = ba;
	mask |= CWBackPixel;
	attributes.border_pixel = bo;
	mask |= CWBorderPixel;
	attributes.bit_gravity = NorthWestGravity;
	mask |= CWBitGravity;

	text = XCreateWindow (
			dpy, win, x, y, w, h, bw,
			CopyFromParent, CopyFromParent, CopyFromParent,
			mask, &attributes
		);

	XSelectInput (dpy, text, C_MASK);

	return (text);
}
