/* $Header: XpexColormap.c,v 2.2 91/09/11 16:06:18 sinyaw Exp $ */

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

/* XpexColormap.c */

#include <X11/Xmd.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "Xpexlibint.h"
#include "Xpexlib.h"

/* define max number of colors and number of planes for color cube */
/* use a 2/3/2 (planes) cube (or  4/8/4 in terms of # of colors) if
 * possible. This puts in 128 colors. However, Sun has a "feature"
 * that if the last entry in the colormap is the same as the first
 * entry, it auto-magically reverses the last entry. If this cube
 * is allocated to go in the end of the colormap (which has always
 * happened for me), then that causes the last color (white) to be
 * reversed (to black).  What a nice feature :). Therefore, when using
 * Sun's, mudge this to do a 2/2/2 cube so it won't be as likely to
 * end up at the end of the colormap.
 */

#ifdef sun

#define REDS_128		4
#define GREENS_128		4
#define BLUES_128		4

#define RED_PLANES		2
#define GREEN_PLANES	2
#define BLUE_PLANES	2

#else

#define REDS_128		4
#define GREENS_128		8
#define BLUES_128		4

#define RED_PLANES		2
#define GREEN_PLANES	3
#define BLUE_PLANES	2

#endif

#define X_MAX_COLOR_VALUE	65535
#define Malloc( _size )	malloc((unsigned)(_size))

static int 
_XpexGetTrueColors(dpy, drawable, attrs, ncolors, xcolors, pcae)
	Display *dpy;
	Drawable drawable;
	XWindowAttributes *attrs;
	int *ncolors;		/* RETURN */
	XColor **xcolors;	/* RETURN */
	pexColourApproxEntry *pcae;		/* RETURN */
{
	Drawable pex_map_window;
	XStandardColormap std;
	Atom best_map;
	Atom pex_map = 0;
	register int i;
	register XColor *xcolor;

	if ((attrs->visual->class == StaticGray) ||
	(attrs->visual->class == GrayScale)) {
		best_map = XA_RGB_GRAY_MAP;
	} else {
		best_map = XA_RGB_BEST_MAP;
	}

	/* look for predefined maps first */
	/* pex_map = ws->type->desc_tbl.xwin_dt.colormap_property_atom; */
#ifdef IGNORE
	if (XGetStandardColormap(dpy, DefaultRootWindow(dpy),  &std,
	pex_map) && (attrs->colormap == std.colormap)) {
		/* colors are already in the colormap */
		*ncolors = 0;
		goto set_color_approx_table;
	} else 
#endif /* IGNORE */
	
	if (XGetStandardColormap(dpy, DefaultRootWindow(dpy), &std,
	best_map) && (attrs->colormap == std.colormap)) {
		/* colors are already in the colormap */
		*ncolors = 0;
		goto set_color_approx_table;
	} else if (XGetStandardColormap(dpy, DefaultRootWindow(dpy), &std,
	XA_RGB_DEFAULT_MAP) && (attrs->colormap == std.colormap)) {
		/* colors are already in the colormap */
		*ncolors = 0;
		goto set_color_approx_table;
	}

	/* no predefined maps found, so make our own color cube */
	std.colormap = attrs->colormap;

	/* define color entries */
	switch (attrs->visual->class){
	case StaticGray:
	case StaticColor:
	case TrueColor:
	case DirectColor:
		return (False);
		break;
#ifdef PEX_SI_API_GRAY_VISUAL
/* PEX-SI doesn't support GrayScale visuals.
 * Here's untested code in case you want to try it
 */
	case GrayScale:
	/* set up gray scale, r=g=b */
	{
		unsigned int nplanes;
		unsigned long *plane_masks;

		*ncolors = REDS_128 * GREENS_128 * BLUES_128;
		nplanes = RED_PLANES + BLUE_PLANES + GREEN_PLANES;
		if (!(*xcolors = (XColor *)Malloc(nplanes * 
		sizeof(unsigned long) + *ncolors * sizeof(XColor)))) { 
			return (False);
		}

		plane_masks = (unsigned long *)(*xcolors + *ncolors);

		if (!XAllocColorCells( dpy, attrs->colormap, True,
		plane_masks, nplanes, &std.base_pixel, 1 )) {
			if (!(std.colormap = XCreateColormap( dpy,
			drawable, attrs->visual, AllocNone))) {
				goto fail;
			} else {
				if (!XAllocColorCells( dpy, std.colormap, True,
				plane_masks, nplanes, &std.base_pixel, 1 )) {
					XFreeColormap(dpy, std.colormap);
					goto fail;
				} else {
					XSetWindowColormap(dpy, drawable, std.colormap);
					attrs->colormap = std.colormap;
				}
			}
		}
/* find low order plane mask. it may always be in [0],
 * but I'm not sure, so look for it
 */
		for (i=0, std.red_mult = plane_masks[0]; i<nplanes; i++)
			if (plane_masks[i] < std.red_mult)
				std.red_mult = plane_masks[i];

/* setting up a gray ramp into a PEX ColourSpace doesn't
 * really make sense, so try something else if you can
 */
		std.green_mult = std.red_mult * REDS_128;
		std.blue_mult = std.green_mult * GREENS_128;

		std.red_max = REDS_128 - 1;
		std.green_max = GREENS_128 - 1;
		std.blue_max = BLUES_128 - 1;

/* use nplanes as temp value */
		nplanes = X_MAX_COLOR_VALUE / (*ncolors - 1);
		for (xcolor = *xcolors, i = 0; i < *ncolors; i++, xcolor++) {
			xcolor->flags = DoRed;
			xcolor->pixel = std.base_pixel + i;
			xcolor->red = i * nplanes;
			xcolor->green = xcolor->blue = xcolor->red;
		}
		break;
	}
#endif /* PEX_SI_API_GRAY_VISUAL */
	case PseudoColor:
	/* set up pseudo color cube */
	{
		unsigned int nplanes;
		unsigned long *plane_masks;
		register int j, k;

		*ncolors = REDS_128 * GREENS_128 * BLUES_128;
		nplanes = RED_PLANES + GREEN_PLANES + BLUE_PLANES;
		if (!(*xcolors = (XColor *)Malloc(nplanes * 
		sizeof(unsigned long) + *ncolors * sizeof(XColor)))) {
			return (False);
		}
		plane_masks = (unsigned long *)(*xcolors + *ncolors);
		
		if (!XAllocColorCells( dpy, attrs->colormap, True,
		plane_masks, nplanes, &std.base_pixel, 1 )) {
			if (!(std.colormap = XCreateColormap(dpy,
			drawable, attrs->visual, AllocNone ))) { 
				goto fail; 
			} else { 
				if (!XAllocColorCells( dpy, std.colormap, True, 
				plane_masks, nplanes, &std.base_pixel, 1)) { 
					XFreeColormap( dpy, std.colormap ); 
					goto fail; 
				} else {
					XSetWindowColormap(dpy, drawable, std.colormap); 
					attrs->colormap = std.colormap; 
				}
			} 
		}

/* find low order plane mask. it may always be in [0],
 * but I'm not sure, so look for it
 */
		for (i=0, std.red_mult = plane_masks[0]; i<nplanes; i++) {
			if (plane_masks[i] < std.red_mult) {
				std.red_mult = plane_masks[i];
			}
		}

		std.green_mult = std.red_mult * REDS_128;
		std.blue_mult = std.green_mult * GREENS_128;

		std.red_max = REDS_128 - 1;
		std.green_max = GREENS_128 - 1;
		std.blue_max = BLUES_128 - 1;

		xcolor = *xcolors;
		for (i = 0; i <= std.red_max; i++) { /* red loop */
			for (j = 0; j <= std.green_max; j++) { /* green loop */
				for (k = 0; k <= std.blue_max; k++) { /* blues */
					xcolor->flags = DoRed | DoGreen | DoBlue;
					xcolor->pixel = std.base_pixel +
						i * std.red_mult + 
						j * std.green_mult +
						k * std.blue_mult; 
					xcolor->red = i * X_MAX_COLOR_VALUE / std.red_max;
					xcolor->green = j * X_MAX_COLOR_VALUE / std.green_max; 
					xcolor->blue = k * X_MAX_COLOR_VALUE / std.blue_max; 
					xcolor++;
				}
			}
		}
		break;
	}
#ifdef PEX_SI_API_DIRECT_VISUAL
/* PEX-SI doesn't support DirectColor visuals.
 * Here's untested code in case you want to try it
 */
case DirectColor:
/* set up direct color cube */
{
	unsigned long red_mask, green_mask, blue_mask;
	register unsigned long red_shift, green_shift, blue_shift;

	*ncolors = WS_MAX_3(REDS_128, GREENS_128, BLUES_128);
	if (!(*xcolors = (XColor *)Malloc(*ncolors * sizeof(XColor))))
		return (False);

	if (!XAllocColorPlanes(dpy, attrs->colormap, True,
	&std.base_pixel, 1, RED_PLANES, GREEN_PLANES, BLUE_PLANES
	&red_mask, &green_mask, &blue_mask)) {
		if (!(std.colormap = XCreateColormap(dpy, drawable, 
		attrs->visual, AllocNone))) { 
			goto fail;
		} else {
			if (!XAllocColorPlanes(dpy, std.colormap, True,
			&std.base_pixel, 1, RED_PLANES, GREEN_PLANES, BLUE_PLANES
			&red_mask, &green_mask, &blue_mask)) {
				XFreeColormap(dpy, std.colormap); 
				goto fail;
			} else {
				XSetWindowColormap(dpy, drawable, std.colormap);
				attrs->colormap = std.colormap; 
			} 
		}
	}
	/* find low order bit position of the planes */
	for (red_shift = 0; !(red_mask & (1<<red_shift)); red_shift++);
	for (green_shift = 0; !(green_mask & (1<<green_shift)); green_shift++);
	for (blue_shift = 0; !(blue_mask & (1<<blue_shift)); blue_shift++);

	std.red_mult = 1 << red_shift;
	std.green_mult = 1 << green_shift;
	std.blue_mult = 1 << blue_shift;

	std.red_max = REDS_128 - 1;
	std.green_max = GREENS_128 - 1;
	std.blue_max = BLUES_128 - 1;

	for (xcolor = *xcolors, i = 0; i < *ncolors; i++, xcolor++) {
		xcolor->flags = DoRed | DoGreen | DoBlue;
		xcolor->pixel = (i << red_shift) |
			(i << green_shift) |
			(i << blue_shift) | std.base_pixel;
		if (i < REDS_128)
			xcolor->red = i * X_MAX_COLOR_VALUE / std.red_max;
		if (i < GREENS_128)
			xcolor->green = i * X_MAX_COLOR_VALUE / std.green_max;
		if (i < BLUES_128)
			xcolor->blue = i * X_MAX_COLOR_VALUE / std.blue_max;
		}
		break;
	}
#endif
	default:
		break;
	}

set_color_approx_table:
		pcae->approxType = PEXColourSpace;
		pcae->approxModel = PEXColourApproxRGB;
		pcae->max1 = std.red_max;
		pcae->max2 = std.green_max;
		pcae->max3 = std.blue_max;
		pcae->dither = True;
		pcae->mult1 = std.red_mult;
		pcae->mult2 = std.green_mult;
		pcae->mult3 = std.blue_mult;
		pcae->basePixel = std.base_pixel;
		pcae->weight1 = 0.0; /* unused for ColourSpace */
		pcae->weight2 = 0.0; /* unused for ColourSpace */
		pcae->weight3 = 0.0; /* unused for ColourSpace */
		return (True);
	fail:
		free((char *)*xcolors);
		return (False);
}

Status
XpexSetupColormap(dpy, drawable, color_approx_lut_id)
	Display *dpy;
	Drawable drawable;
	XpexLookupTable color_approx_lut_id;
{
	XWindowAttributes		attrs;
	XColor		*xcolors;
	pexColourApproxEntry	cae;
	int 					ncolors;

	XGetWindowAttributes(dpy, drawable, &attrs);

	if (!_XpexGetTrueColors(dpy, drawable, &attrs, 
	&ncolors, &xcolors, &cae)) {
		return (False);
	}

	XpexSetTableEntries(dpy, color_approx_lut_id, Xpex_ColorApproxLUT, 
	(XpexTableIndex) 0, 1, (char *) &cae, sizeof(cae));

	if (ncolors) {
		XStoreColors(dpy, attrs.colormap, xcolors, ncolors);
		free((char *)xcolors);
	}

	return (True);
}
