/*
 * dpsXcmu.c -- Simple color management/allocation utility
 *
 * Copyright (C) 1988-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */  

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

/* Defines for standard colormap routines */

#define PrivSort qsort
#ifndef NULL
#define NULL 0
#endif /* NULL */

#define DPS_DEFAULT_GRAYS	8
#define DPS_DEFAULT_RGBS_8P	4

Atom XA_GRAY_DEFAULT_MAP;
Atom XA_GRAY_BEST_MAP;

static Status contiguous();
static int compare();
static void ColorValuesFromMask();
static int XSetUpStandardColormap();

/*
-==========
 PUBLIC
-==========
*/


static void
GetGrayRamp(dpy, screen, root, grayramp)
    Display *dpy;
    Screen *screen;
    Window root;
    XStandardColormap *grayramp;
{ 
    int i, status;
    Atom grayProps[3];
    Visual vis, *pVis = DefaultVisualOfScreen(screen);
    
    if (pVis->class == StaticGray)
	/* can't rely on DPS compatible static ramp, so do mono */
	goto mono_convention;
    
    /* try all three gray properties */
    grayProps[0] = XA_RGB_GRAY_MAP;
    grayProps[1] = XA_GRAY_DEFAULT_MAP;
    grayProps[2] = XA_GRAY_BEST_MAP;
    for (i = 0; i < 3; i++)
	{
	status = XGetStandardColormap(dpy, root, grayramp, grayProps[i]);
	if (status) return;
	}

    /* fall through loop means no default property, so try to define one */
    
    status = XSetUpStandardColormap(
      dpy,
      screen,				    
      root,
      (pVis->class == GrayScale) ? XA_RGB_GRAY_MAP : XA_GRAY_DEFAULT_MAP,
      grayramp,
      &vis);

    if (!status)
        {
mono_convention:
        /* server recognizes a colorcube with red_mult=0 and a grayramp with
           red_max=1, red_mult=1, base_pixel=0
	   to mean a 2 gray ramp with BlackPixel being the lowest
	   intensity gray and WhitePixel being the highest
	   intensity gray */
        grayramp->colormap = DefaultColormapOfScreen(screen);
	grayramp->red_max = 1;
	grayramp->red_mult = 1;
	grayramp->base_pixel = 0;
        }
}

void
XDPSGetDefaultColorMaps(dpy, screen, drawable, colorcube, grayramp)
    Display *dpy;
    Screen *screen;
    Drawable drawable;
    XStandardColormap *colorcube, *grayramp;
{
    int status;
    Visual vis;
    Window root;
    
    XA_GRAY_DEFAULT_MAP = XInternAtom(dpy, "DEFAULT_GRAY", False);
    XA_GRAY_BEST_MAP = XInternAtom(dpy, "BEST_GRAY", False);

    /* If there is a screen specified, use it; otherwise use the drawable */

    if (screen == NULL) {
	if (drawable == None || ScreenCount(dpy) == 1) {
	    root = DefaultRootWindow(dpy);
	    screen = DefaultScreenOfDisplay(dpy);
	} else {
	    /* Have to get the root for this drawable */
	    int x, y;
	    int i;
	    unsigned int width, height, bwidth, depth;
	    if (!XGetGeometry(dpy, drawable, &root, &x, &y, &width, &height,
			      &bwidth, &depth)) root = DefaultRootWindow(dpy);
	    for (i = 0;
		 i < ScreenCount(dpy) && root != RootWindow(dpy, i);
		 i++) {}
	    screen = ScreenOfDisplay(dpy, i);
	}
    } else root = RootWindowOfScreen(screen);

    if (colorcube != NULL)
        {
        /* try to get default colorcube */
        
        status = XSetUpStandardColormap(
	  dpy,
	  screen,
	  root,
	  XA_RGB_DEFAULT_MAP,
	  colorcube,
	  &vis);

        if (!status ||
            (colorcube->red_max == 1
	     && colorcube->red_mult == 1
	     && !colorcube->green_max && !colorcube->green_mult
	     && !colorcube->blue_max && !colorcube->blue_mult))
            {
            /* if we couldn't get a default colorcube, or it
               came back as monochrome, ignore the cube and use
               the grayramp only */
            colorcube->red_mult = 0;
            }
        }
    
    if (grayramp != NULL)
        /* get a default grayramp */
	GetGrayRamp(dpy, screen, root, grayramp);
}

/* Put standard colormap properties on the root window.  It tries to
   make the best color allocation, but if it can't, it falls back
   to monochrome.  If the routine returns 0, some unrecoverable error
   occurred. */

static int
XSetUpStandardColormap(dpy, screen, root, mapProp, pMap, pVis)
  Display *dpy;
  Screen *screen;
  Drawable root;
  Atom mapProp;
  XStandardColormap *pMap;		/* RETURN */
  Visual *pVis;				/* RETURN */
{
    register unsigned long i, n;
    Visual *dfltVis;
    Colormap dfltCmap;
    XWindowAttributes xwa;
    Window w;
    Display *newDpy;
    int ok, retainable;
    int doGray = 0;
    XStandardColormap *new;
    unsigned long red, grn, blu;
    int redMult, greenMult, blueMult;
    int npixels, first, remain, count, retryGrays;
    unsigned long *pixels = NULL;
    XColor color;
  
    dfltVis = DefaultVisualOfScreen(screen);
    dfltCmap = DefaultColormapOfScreen(screen);
    w = RootWindowOfScreen(screen);
      
    /* If window's Visual is incompatible with root, fall back to mono */
    if (dfltVis == (Visual *)NULL) return(0);
    *pVis = *dfltVis;
    if (root != None)
        {
        ok = XGetWindowAttributes(dpy, root, &xwa);
	if (!ok
	    || xwa.visual->class != dfltVis->class
	    || xwa.visual->bits_per_rgb != dfltVis->bits_per_rgb
	    || (dfltVis->class == StaticGray && dfltVis->bits_per_rgb == 1))
	    {
mono:       /* Fall back to monochrome */
	    pMap->colormap = dfltCmap;
	    pMap->red_max = 1;
            pMap->red_mult = 1;
            pMap->blue_max =
            pMap->blue_mult = 0;
            pMap->green_max =
            pMap->green_mult = 0;
	    pMap->base_pixel = 0;
	    return(1);
	    }
        }
  
    /* Have someone registered the property already? */
    ok = XGetStandardColormap(dpy, w, pMap, mapProp);
    if (ok) return(ok);
    
    /* No property, so we define it */
    
    /* Choose number of reds, greens, blues or grays */
    if (mapProp == XA_RGB_GRAY_MAP)
	{
rgb_gray:	
	n = pVis->map_entries;
	grn = 0;
	blu = 0;
	redMult = greenMult = blueMult = 1;

	if (pVis->class == StaticGray)
	    goto mono;
	else if (pVis->class == GrayScale)
	    {
	    if (n < DPS_DEFAULT_GRAYS)
	        {red = n; retryGrays = (n < 3) ? 2: 3;}
	    else if (n > 2 * DPS_DEFAULT_GRAYS)
	        {
	        red = (2 * DPS_DEFAULT_GRAYS) - 1;
	        retryGrays = DPS_DEFAULT_GRAYS - 1;
	        }
	    else if (n > DPS_DEFAULT_GRAYS)
	        {red = DPS_DEFAULT_GRAYS - 1; retryGrays = 3;}
	    else
	        {red = n/2 - 1; retryGrays = 2;}
	    /* pre-qualify allocation */
	    i = red + 1;
	    pixels = (unsigned long *) calloc(i, sizeof(unsigned long));
	    if (pixels)
	        {
	        XGrabServer(dpy);
	        ok = XAllocColorCells(dpy, dfltCmap, 1, NULL, 0, pixels, i);
	        if (!ok)
	            {
	            /* we asked for too many, so try something less */
	            red = retryGrays;
	            }
	        else
	            XFreeColors(dpy, dfltCmap, pixels, i, 0);
	        free(pixels);
	        XUngrabServer(dpy);
	        XFlush(dpy);
	        pixels = NULL;
	        }
	    }
	else if (pVis->class == PseudoColor
	         || pVis->class == DirectColor
	         || pVis->class == TrueColor)
	    {
	    XStandardColormap cube;
	    unsigned long t = DPS_DEFAULT_GRAYS;
	    int diagonalOK, rampOK;
	    
	    /* First see if we can use diagonal of existing colorcube */
	    
	    ok = XGetStandardColormap(dpy, w, &cube, XA_RGB_DEFAULT_MAP);
	    diagonalOK = (ok && cube.red_max == cube.green_max
		&& cube.red_max == cube.blue_max);
		
	    /* Next see if ok to use a default gray ramp */

	    pixels = (unsigned long *) calloc(t, sizeof(unsigned long));
	    if (pixels)
	        {
	        XGrabServer(dpy);
	        ok = XAllocColorCells(dpy, dfltCmap, 1, NULL, 0, pixels, t);
	        if (ok)
	            {
	            XFreeColors(dpy, dfltCmap, pixels, t, 0);
	            rampOK = 1;
	            }
	        else rampOK = 0;
	        free(pixels);
	        XUngrabServer(dpy);
	        XFlush(dpy);
	        pixels = NULL;
	        }
	    else rampOK = 0;

	    
	    if (diagonalOK && cube.red_max >= DPS_DEFAULT_GRAYS - 1)
	        /* 1st choice: large diagonal */
	        goto use_diagonal;
	    else if (rampOK)
	        /* 2nd choice: default ramp */
	        red = DPS_DEFAULT_GRAYS - 1;
	    else if (diagonalOK)
		{
		/* 3rd choice: any diagonal */
use_diagonal:
		pMap->colormap = dfltCmap;
		pMap->red_max = cube.red_max;
		t = pMap->red_max + 1;
		pMap->red_mult = (t * t) + t + 1;
		pMap->base_pixel = cube.base_pixel;
		XSetStandardColormap(dpy, w, pMap, mapProp);
		return(1);
		}
	    else 
	        {
	        /* 4th choice: small ramp */
	        /* if this doesn't fit either, we'll fall back to mono */
	        red = 3;
	        }
	    }
	else goto mono;
	doGray = 1;
	}
    else if (mapProp == XA_GRAY_DEFAULT_MAP)
	{
	/* hack: treat like RGB_GRAY_MAP */
	goto rgb_gray;
	}
    else if (mapProp == XA_GRAY_BEST_MAP)
	{
	/* hack: treat like RGB_GRAY_MAP */
	goto rgb_gray;
	}
    else /* assume XA_RGB_DEFAULT_MAP */
	{
	switch (pVis->class)
	    {
	    case TrueColor:
		ColorValuesFromMask(pVis->red_mask, &red, &redMult);
		ColorValuesFromMask(pVis->green_mask, &grn, &greenMult);
		ColorValuesFromMask(pVis->blue_mask, &blu, &blueMult);
		break;
	    case PseudoColor:
		if (pVis->map_entries > 500000)
		    /* assume 24 plane or greater */
		    {red = 255; grn = 255; blu = 255;}
		else if (pVis->map_entries > 4000)
		    /* assume 12 plane */
		    {red = 12; grn = 12; blu = 12;}
		else if (pVis->map_entries > 16)
		    /* assume 8 plane */
		    {
		    red = DPS_DEFAULT_RGBS_8P - 1;
		    grn = DPS_DEFAULT_RGBS_8P - 1;
		    blu = DPS_DEFAULT_RGBS_8P - 1;
		    }
		else if (pVis->map_entries <= 16 && pVis->map_entries > 4)
		    /* assume 4 plane */
		    {red = 1; grn = 1; blu = 1;}
		else 
		  /* unknown */ 
		  goto mono;
		redMult = (grn + 1) * (blu + 1);
		greenMult = blu + 1;
		blueMult = 1;
		break;
	    case DirectColor:
	    case StaticGray:
	    case StaticColor:
		/* default to monochrome */
		goto mono;
	    default: goto mono;
	    }
	}
	
    /* Create standard colormap */
    new = (XStandardColormap *) calloc(1, sizeof(XStandardColormap));
    if (new == NULL) return(0);
      
    /* Try to open new connection so property can be retained */
    if ((newDpy = XOpenDisplay(XDisplayString(dpy))) == NULL)
	{
	newDpy = dpy;
	retainable = 0;
	}
    else retainable = 1;
    
    new->colormap = dfltCmap;
    new->red_max = red;
    new->green_max = grn;
    new->blue_max = blu;
    new->red_mult = redMult;
    new->green_mult = greenMult;
    new->blue_mult = blueMult;
    new->base_pixel = 0; /* temporary, may change */
    
    count = (red + 1) * (blu + 1) * (grn + 1);

    if (pVis->class != PseudoColor && pVis->class != GrayScale) 
	{
	XGrabServer(newDpy);
	goto skip_cmap_alloc;
	}

    pixels = (unsigned long *) calloc(pVis->map_entries, sizeof(unsigned long));
    if (pixels == NULL) 
	{
	free(new);
	if (retainable) XCloseDisplay(newDpy);
	return(0);
	}
    
    /* First try to allocate the entire colormap */
    XGrabServer(newDpy);
    npixels = pVis->map_entries;
    ok = XAllocColorCells(newDpy, dfltCmap, 1, NULL, 0, pixels, npixels);
    if (!ok)
	{
	int total = count;
	int top, mid;
	int success = 0;
	/* allocate all available cells, using binary backoff */
	top = pVis->map_entries - 1;
	while (total <= top)
	    {
	    mid = total + ((top - total + 1) / 2);
	    ok = XAllocColorCells(newDpy, dfltCmap, 1, NULL, 0, pixels, mid);
	    if (ok)
		{
		if (mid == top)
		    {
		    success = 1;
		    break;
		    }
		else
		    {
		    XFreeColors(newDpy, dfltCmap, pixels, mid, 0);
		    total = mid;
		    }
		}
	    else
		top = mid - 1;
	    }
	/* check to see if we fell through loop */
	if (!success)
	    {
	    npixels = 0;
	    goto fallback;
	    }
	npixels = mid;
	}

    /* Avoid pessimal case by testing to see if already sorted */
    for (i = 0; i < npixels-1; ++i)
	if (pixels[i] != pixels[i+1]-1) break;
    
    if (i < npixels-1)
	PrivSort((char *)pixels, npixels, sizeof(unsigned long), compare);
    
    if (!contiguous(pixels, npixels, count, &first, &remain))
	{
fallback:
	/* can't find enough contiguous cells, fall back */
	if (npixels > 0)
	    XFreeColors(newDpy, dfltCmap, pixels, npixels, 0);
	if (pixels != NULL) free((char *) pixels);
	if (new != NULL) free((char *) new);
	XUngrabServer(newDpy);
	if (retainable) XCloseDisplay(newDpy);
	goto mono;
	}

    new->base_pixel = pixels[first];
    color.flags = DoRed | DoGreen | DoBlue;
    
    /* Define colors */
    for (n = pixels[first], i = 0; i < count; ++i, ++n)
	{
	unsigned long pix = n;
	XColor request;
	
	if (doGray)
	    {
	    color.pixel = n;
	    color.red = (unsigned short)((i * 65535) / (red + grn + blu));
	    color.green = color.red;
	    color.blue = color.red;
	    }
	else
	    {
	    color.pixel = n;
	    color.red = (unsigned short)
	      (((i / new->red_mult) * 65535) / red);
	    color.green = (unsigned short)
	      ((((i / new->green_mult) % (grn + 1)) * 65535) / grn);
	    color.blue = (unsigned short)
	      (((i % new->green_mult) * 65535) / blu);
	    }
	request = color;
	
	/* Free RW, Alloc RO, if fails, try RW */
	XFreeColors(newDpy, dfltCmap, &pix, 1, 0);
	ok = XAllocColor(newDpy, dfltCmap, &request);
	/* If the pixel we get back isn't the request one, probably RO
	   White or Black, so shove it in RW so our cube is correct.
	   If alloc fails, try RW. */
	if (!ok || request.pixel != color.pixel)
	    {
	    pix = n;
	    /*
	    XFreeColors(newDpy, dfltCmap, &pix, 1, 0);
	    */
	    ok = XAllocColorCells(newDpy, dfltCmap, 0, NULL, 0, &pix, 1);
	    if (!ok) goto fallback;
	    if (pix != color.pixel)
	        {
	        XFreeColors(newDpy, dfltCmap, &pix, 1, 0);
	        goto fallback;
	        }
	    request = color;
	    XStoreColor(newDpy, dfltCmap, &request);
	    }
        }
    
    /* Free unused cells, if any */
    if (first) XFreeColors(newDpy, dfltCmap, pixels, first, 0);
    if (remain) XFreeColors(newDpy, dfltCmap, &(pixels[first+count]), remain, 0);

skip_cmap_alloc:
    /* Attach property */
    XSetStandardColormap(newDpy, w, new, mapProp);
    *pMap = *new;
    
    if (retainable) XSetCloseDownMode(newDpy, RetainPermanent);
    ok = 1;
    
    if (pixels != NULL) free((char *) pixels);
    if (new != NULL) free((char *) new);
    XUngrabServer(newDpy);
    XCloseDisplay(newDpy);
    return(ok);
}


/*
-==========
 PRIVATE
-==========
*/

static Status contiguous(pixels, npixels, ncolors, first, rem)
    unsigned long	pixels[];	/* specifies allocated pixels */
    int			npixels;	/* specifies count of alloc'd pixels */
    int			ncolors;	/* specifies needed sequence length */
    int			*first;		/* returns first index of sequence */
    int			*rem;		/* returns first index after sequence,
					 * or 0, if none follow */
{
    register int i = 1;		/* walking index into the pixel array */
    register int count = 1;	/* length of sequence discovered so far */

    *rem = npixels - 1;
    *first = 0;
    while (count < ncolors && ncolors - count <= *rem)
    {
	if (pixels[i-1] + 1 == pixels[i])
	    count++;
	else {
	    count = 1;
	    *first = i;
	}
	i++;
	(*rem)--;
    }
    if (count != ncolors)
	return 0;
    return 1;
}


static int compare(e1, e2)
    register unsigned long	*e1, *e2;
{
    if (*e1 < *e2)	return -1;
    if (*e1 > *e2)	return 1;
    return 0;
}


static void
ColorValuesFromMask(mask, maxColor, mult)
  unsigned long	mask;
  unsigned long	*maxColor;
  int		*mult;
  {
  *mult = 1;
  while ((mask & 1) == 0) {
    *mult <<= 1;
    mask >>= 1;
    }
  *maxColor = mask;
  }
