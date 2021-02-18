/* $Header: XpexVisual.c,v 2.2 91/09/11 16:06:20 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

/* XpexVisual.c */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

/*  Ideally, what we want to be able to do is take these steps (before a window  *  is created) :
 *
 *  Find the most appropriate visual for our needs
 *  See if there is a StandardColormap property defined
 *      on that visual.
 *  Set up the colormap on that visual, or use an existing one.
 *
 *  The problem, though, is that X doesn't provide a mechanism by
 *  which StandardColormap properties can be queried on an individual
 *  visual class if a window on that visual doesn't exist.  Therefore,
 *  we have to go heavily heuristic, and the heuristics used here are
 *  certainly open to tweaking.
 */

int
XpexFindBestVisual(dpy, bestVisual, colorMap)
	Display *dpy;
	Visual *bestVisual;
	Colormap *colorMap;
{
	XVisualInfo *available_visuals, *the_best, template;
	XStandardColormap    std;
	int          nvisuals, i;
	/* Atom         pex_map = 0; */
	Atom         best_map;
	Visual      *default_visual;

	default_visual = DefaultVisual(dpy, DefaultScreen(dpy));
	*colorMap = DefaultColormap(dpy, DefaultScreen(dpy));

	/* use default visual if it has a standard colormap defined
	 * regardless of the visual type
	 * try for BEST std colormap first, then DEFAULT
				   */
	if ((default_visual->class == StaticGray) ||
	   (default_visual->class == GrayScale)) {
		best_map = XA_RGB_GRAY_MAP;
	} else {
		best_map = XA_RGB_BEST_MAP;
	}
	/* look to see if there is an application defined std colormap
	 * if there is, use it's colormap, but still find the best visual
	 */
#ifdef IGNORE	
	if (XGetStandardColormap(dpy, DefaultRootWindow(dpy),  &std,
	pex_map) && (std.colormap)) {
		*colorMap = std.colormap;
		goto get_best_visual;
	}
#endif /* IGNORE */

	if (XGetStandardColormap(dpy, DefaultRootWindow(dpy), &std,
	best_map) && (std.colormap)) {
		/* map already exists */
		*bestVisual = *default_visual;
		*colorMap = std.colormap;
		return (True);
	}

	if (XGetStandardColormap(dpy, DefaultRootWindow(dpy),
	&std, XA_RGB_DEFAULT_MAP) && (std.colormap)) {
		/* map already exists */
		*bestVisual = *default_visual;
		*colorMap = std.colormap;
		return (True);
	}

get_best_visual:
/* no standard colormap so pick best visual
 *
 *  Best Visual is that with the highest numbered class.
 *  This relies on the fact that the class values are ordered:
 *    DirectColor > TrueColor > PseudoColor > StaticColor
 *
 *  However, only choose a higher numbered class if its got at
 *  least as many cmap entries as what's currently best
 *  Note: colour mapping is only supported on PseudoColor visuals
 */
	template.screen = DefaultScreen(dpy);
	available_visuals = XGetVisualInfo(dpy, VisualScreenMask,
	&template, &nvisuals);

	the_best = &available_visuals[0];
	for (i = 1; i < nvisuals; i++) {
		if ((available_visuals[i].class > the_best->class) &&
		(available_visuals[i].colormap_size >= the_best->colormap_size)) {
			the_best = &available_visuals[i]; 
		}
	}

	*bestVisual = *the_best->visual;
	XFree((caddr_t) available_visuals);

	return (True);
}
