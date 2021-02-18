/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 *  File Name:
 *	pst_colormap.c
 * 
 *  Subsystem:
 *
 *  Version: 
 *	V1
 *
 *  Abstract:
 *	This will create a gamma corrected colormap.  The main entry point
 *	of CreateColormap will switch off the visual type.  Also, one can
 *	call either CreatePsuedoColormap (8 plane devices) or
 *	CreateDirectColormap (24 plane devices) directly.
 *
 *  Keywords:
 *
 *  Environment:
 *
 *
 *  Creation Date:
 *		1988	
 *
 *  Modification History:
 *
 */


/*
 * Table of Contents  (Routine names in order of appearance)
 *
 * E CreateColormap
 * E CreatePseudoColormap
 * E CreateTrueColormap
 */


/*
 *  Include Files
 */
#include <math.h>
#include "X.h"
#include "Xlib.h"
#include "Xutil.h"
#include "Xatom.h"
#include "PEX.h"
#include "PEXprotost.h"
#include "PEXlib.h"


/*
 *  Local defines and macros
 */

#define	DEFAULT_GAMMA			2.3
#define	PSEUDO_COLORMAP_ATOM_NAME	"Pete'sGammaCorrectedPseudoColormap"
#define	TRUE_COLORMAP_ATOM_NAME		"Pete'sGammaCorrectedTrueColormap"
#define	DIRECT_COLORMAP_ATOM_NAME	"Pete'sGammaCorrectedTrueColormap"

#define	MAX_BREAKUP	4

static Colormap	CreatePseudoColormap();
static Colormap CreateTrueColormap();

typedef struct {
	Colormap	cmap;
	int		basePixel;
} ColormapProperty;


/*+
 *  NAME: 
 *	CreateColormap
 *
 *  FORMAT:
 *	none
 *
 *  ARGUMENTS:
 *	display		- pointer to the display
 *	window		- target window
 *	rdr		- renderer
 *	pc		- pc associated with renderer (if 0, one will
 *			  be created and associated)
 *	gamma		- gamma value (for direct color, you specify
 *			  gamma values for each channel (if 0,
 *			  the value DEFAULT_GAMMA will be used) 
 *
 *  RETURNS:
 *	colormap
 *
 *  EXAMPLES:
 *	none
 *
 *  DESCRIPTION:
 *	Uses visual type to create a colormap
 *
 *  SEE ALSO:
 *      {@references@}
 *
 *
 *  CAVEATS:
 *	{@bugs-and-things-not-handled@}
 */
Colormap CreateColormap(display,window,rdr,pc,gamma)
Display		*display;
Window		window;
pexRenderer	rdr;
pxlPipelineContext	pc;
float		gamma;
{
    Colormap		return_colormap;
    XWindowAttributes	attributes;

    return_colormap = 0;
    
    XGetWindowAttributes(display,window,&attributes);

    switch (attributes.visual->class)
    {


    case PseudoColor:
	return_colormap = CreatePseudoColormap (display, window,
		&attributes, rdr, pc, gamma);
	break;

    case TrueColor:
	return_colormap = CreateTrueColormap (display, window,
		&attributes, rdr, pc, gamma);
	break;


    }

    return (return_colormap);
}

/*+
 *  NAME: 
 *	CreatePseudoColormap
 *
 *  FORMAT:
 *	none
 *
 *  ARGUMENTS:
 *	display		- pointer to the display
 *	window		- target window
 *	attrs		- window attributes
 *	rdr		- renderer
 *	pc		- pc associated with renderer (if 0, one will
 *			  be created and associated)
 *	gamma		- gamma value (for direct color, you specify
 *			  gamma values for each channel (if 0,
 *			  the value DEFAULT_GAMMA will be used) 
 *
 *  RETURNS:
 *	colormap
 *
 *  EXAMPLES:
 *	none
 *
 *  DESCRIPTION:
 *	Creates pseudo colormap
 *
 *  SEE ALSO:
 *      {@references@}
 *
 *
 *  CAVEATS:
 *	{@bugs-and-things-not-handled@}
 */
static Colormap CreatePseudoColormap(display,window,attrs,rdr,pc,gamma)
Display		*display;
Window		window;
XWindowAttributes   * attrs;
pexRenderer	rdr;
pxlPipelineContext	pc;
float		gamma;
{

	Window			root;
	Atom			cmapAtom=0;
	unsigned long		pixels[256];
	unsigned long		planes[8];
	int			r, g, b;
	int			i, status;
	double			rinc, ginc, binc;
	XColor			color_def[256];
	double			igamma, value, num, inc;
	double			pow();
	int			two_to_16th = 1 << 16;
	unsigned short		red_gamma[256], green_gamma[256];
	unsigned short		blue_gamma[256];
	int			planeMasks[24];
	int			colorCount;
	Colormap		cmap = 0;
	pxlColourApproxEntry	approx;
	int			maxIntensity;
	pxlLookupTable		lut;
	pxlRendererAttributes	rdrAttribs;
	pxlPCAttributes 	pcAttribs;
	unsigned int		masks[3];

/* do some setup */

	root = RootWindow(display,DefaultScreen(display)),

/* get the maximum intensity */

	lut = PEXCreateLookupTable(display, window, PEXColourApproxLUT);
	maxIntensity = CellsOfScreen(DefaultScreenOfDisplay(display)) - 1;

/* setup a 6,6,6 colormap */

	approx.approxType = PEXColourSpace;
	approx.approxModel = PEXRdrColourModelRGB;
	approx.max1 = 5;
	approx.max2 = 5;
	approx.max3 = 5;
	approx.mult1 = 36;
	approx.mult2 = 6;
	approx.mult3 = 1;
	approx.basePixel = 3;

/* get the colormap */

	{
	    colorCount = (approx.max1+1) * (approx.max2+1) * (approx.max3+1);

#ifdef	NO_WM
	    cmap = XCreateColormap(display, root,
		DefaultVisual(display,DefaultScreen(display)),AllocAll);
	    for (i = 0; i <= maxIntensity; i++)
		color_def[i].pixel = i;
	    XQueryColors(display, XDefaultColormap(display,
		DefaultScreen(display)), color_def, maxIntensity + 1);

#else

/* if we're using the default colormap => allocate more than enough
   cells in order to insure a set of continguous cells (not just
   color planes).  */

	    colorCount += 8;
	    cmap = XDefaultColormap(display,DefaultScreen(display));
	    status = XAllocColorCells(display,cmap,1,planeMasks,0,pixels,
		colorCount);
	    if (!status) 
	    {

/* can't allocate the cells, look and see if someone has already 
   created the colormap for us */

		cmapAtom = XInternAtom(display,PSEUDO_COLORMAP_ATOM_NAME,1);
		if (cmapAtom)
		{
		    Atom		actual_type;
		    int			actual_format;	
		    unsigned long	num;
		    ColormapProperty	*prop;
		    unsigned long	bytes_after;
		    
		    XGetWindowProperty(display,root,cmapAtom,0,2,0,XA_COLORMAP,
			&actual_type,&actual_format,&num,&bytes_after,&prop);
		    if (actual_type != XA_COLORMAP || num != 2)
		    {
			printf ("Bad colormap property\n");
			exit (1);
		    }
		    cmap = prop->cmap;
		    approx.basePixel = prop->basePixel;
		    XFree(prop);
		    goto HaveColormap;
		}
		else
		{
		    printf("colormap allocation failed\n");
		    exit(1);
		}
	    }

/* we have the color cells, now look for the set of contiguous cells
   because X may return back some cells which are left over from
   other planes, but the planes are continugous */

	    {
		int	minPixel[MAX_BREAKUP],maxPixel[MAX_BREAKUP],startIndex[MAX_BREAKUP];
		int	lastPixel,lastMin,largest,index;

		lastMin = 0;
		lastPixel = minPixel[lastMin] = pixels[0];
		startIndex[lastMin] = 0;
		for (i=1;i<colorCount;i++)
		{
		    if (pixels[i] != ++lastPixel)
		    {
			if (lastMin >= MAX_BREAKUP)
			{
			     printf("pst_colormap - colormap too fragmented\n");
			     exit(0);
			}
			maxPixel[lastMin] = pixels[i-1];
			lastPixel = minPixel[++lastMin] = pixels[i];
			startIndex[lastMin] = i;
		    }
		}
		maxPixel[lastMin] = pixels[colorCount-1];
		largest = maxPixel[0] - minPixel[0];
		index = 0;
		for (i=1;i<=lastMin;i++)
		{
		    if (maxPixel[i] - minPixel[i] > largest)
		    {
			largest = maxPixel[i] - minPixel[i];
			index = i;
		    }
		}

		colorCount = largest;
		for (i=0;i<=largest;i++)
		{
		    pixels[i] = pixels[startIndex[index]+i];
		}
	    }

/* set the "real" values for the color approximation.  This isn't really
   "right" as the range of values we get might not be the actual ones
   we wanted, so we would have to take into account a new colorCount. */
		
	    approx.basePixel = pixels[0];

/* we have to bump up colorCount because colorCount was assigned as
   the maximum value, thus, entires 0-colorCount must be accounted 
   for */

	    colorCount++;

#endif	NO_WM

/* compute a gamma-corrected color list */

	    rinc = maxIntensity / (double)(approx.max1 + 1);
	    ginc = maxIntensity / (double)(approx.max2 + 1);
	    binc = maxIntensity / (double)(approx.max3 + 1);
	    if (gamma == 0.0)
		gamma = DEFAULT_GAMMA;
	    igamma = 1.0/gamma;
	    inc = (double) two_to_16th / (double) maxIntensity;
	    num = inc / 2.0;
	    for (i = 0; i <= maxIntensity; i++)
    	    {
		value = num / (double) (two_to_16th - 1);
		red_gamma[i] = green_gamma[i] = blue_gamma[i] =
			(unsigned short) (pow(value, igamma) * two_to_16th);
		num += inc;
	    }

	    i = 0;
	    {
		for (r = 0; r <= approx.max1; r++)
		    for (g = 0; g <= approx.max2; g++)
			for (b = 0; b <= approx.max3; b++)
			{
			    color_def[i].red = red_gamma[(int)((double)r * rinc)];
			    color_def[i].green = green_gamma[(int)((double)g * ginc)];
			    color_def[i].blue = blue_gamma[(int)((double)b * binc)];
			    color_def[i].flags = DoRed | DoGreen | DoBlue;
			    color_def[i].pixel = pixels[i];
			    i++;
			}
	    }

/* store the colors */

#ifdef	NO_WM
	    XStoreColors(display, cmap,color_def,maxIntensity+1);
	    XInstallColormap(display,cmap);
#else
	    XStoreColors(display, cmap,color_def,i);
#endif	NO_WM
	    XFlush(display);

/* store the atom */

	    {
		ColormapProperty	prop;
		cmapAtom = XInternAtom(display,PSEUDO_COLORMAP_ATOM_NAME,0);
		prop.cmap = cmap;
		prop.basePixel = approx.basePixel;
		XChangeProperty(display,root,cmapAtom,XA_COLORMAP,32,
			PropModeReplace,&prop,2);
	    }
	}

HaveColormap:

/* set the window to this colormap */

	XSetWindowColormap(display,window,cmap);

/* update the pc to the new color approximation */

	masks[0] = masks[1] =  masks[2] = 0;
	pcAttribs.colourApproxIndex = 1;
	PEX_SetPCAttrMaskBit(masks,PEXPCColourApproxIndex);
	if (!pc)
	{
	    pc = PEXCreatePipelineContext(display, masks, &pcAttribs);
	    rdrAttribs.pipelineContext = pc;
	    PEXChangeRenderer(display, rdr, PEXRDPipelineContext, &rdrAttribs);
	}
	else
	    PEXChangePipelineContext(display,pc,masks,&pcAttribs);
	rdrAttribs.colourApproxTable = lut;
	PEXChangeRenderer(display, rdr, PEXRDColourApproxTable, &rdrAttribs);
	PEXSetTableEntries(display,lut,PEXColourApproxLUT,1,1,&approx);

	return(cmap);

}

/*+
 *  NAME: 
 *	CreateTrueColormap
 *
 *  FORMAT:
 *	none
 *
 *  ARGUMENTS:
 *	display		- pointer to the display
 *	window		- target window
 *	attrs		- window attributes
 *	rdr		- renderer
 *	pc		- pc associated with renderer (if 0, one will
 *			  be created and associated)
 *	gamma		- gamma value (for direct color, you specify
 *			  gamma values for each channel (if 0,
 *			  the value DEFAULT_GAMMA will be used) 
 *
 *  RETURNS:
 *	colormap
 *
 *  EXAMPLES:
 *	none
 *
 *  DESCRIPTION:
 *	Creates true color colormap
 *
 *  SEE ALSO:
 *      {@references@}
 *
 *
 *  CAVEATS:
 *	{@bugs-and-things-not-handled@}
 */
static Colormap CreateTrueColormap(display,window,attrs,rdr,pc,rgamma,ggamma,bgamma)
Display		*display;
Window		window;
XWindowAttributes   * attrs;
pexRenderer	rdr;
pxlPipelineContext	pc;
float		rgamma;
float		ggamma;
float		bgamma;
{
	Window			root;
	Atom			cmapAtom;
	unsigned long		pixels[256];
	unsigned long		planes[8];
	int			r, g, b;
	int			i, status;
	XColor			color_def[256];
	double			value, num, inc;
	double			inv_rgamma,inv_ggamma,inv_bgamma;
	double			pow();
	int			two_to_16th = 1 << 16;
	int			planeMasks[24];
	int			colorCount;
	Colormap		cmap = 0;
	pxlColourApproxEntry	approx;
	int			maxIntensity;
	pxlLookupTable		lut;
	pxlRendererAttributes	rdrAttribs;
	pxlPCAttributes 	pcAttribs;
	unsigned int		masks[3];
	Display			* retaining_display;
	Window			retained_root;
	
/* do some setup */

	root = attrs->root;

/* get the maximum intensity */

	lut = PEXCreateLookupTable(display, window, PEXColourApproxLUT);
	maxIntensity = 255;

/* setup a 8,8,8 colormap */

	approx.approxType = PEXColourSpace;
	approx.approxModel = PEXRdrColourModelRGB;
	approx.max1 = 255;
	approx.max2 = 255;
	approx.max3 = 255;
	approx.mult1 = 1;
	approx.mult2 = 256;
	approx.mult3 = 1<<16;
	approx.basePixel = 0;

/* see if we have to create the colormap */
	                                        
	cmapAtom = XInternAtom(display,DIRECT_COLORMAP_ATOM_NAME,1);
	if (cmapAtom)
	{
	    Atom		actual_type;
	    int			actual_format;	
	    unsigned long	num;
	    unsigned long	bytes_after;
	    int			*data;
	    XGetWindowProperty (display, root, cmapAtom, 0, 1, 0, XA_COLORMAP,
		    &actual_type, &actual_format, &num, &bytes_after,
		    &data);
	    if (actual_type != XA_COLORMAP || num != 1)
	    {
		printf ("Bad colormap property\n");
		exit (1);
	    }
	    cmap = *data;
	    XFree(data);
#ifdef DEBUG
	    printf ("colormap found is 0x%8x\n", cmap);
#endif
	}

/* if we don't have a colormap => create it */

	if (!cmap)
	{

	    colorCount = (approx.max1+1) * (approx.max2+1) * (approx.max3+1);
	    retaining_display = XOpenDisplay (XDisplayString(display));
	    if (retaining_display == 0)
	    {
		printf ("Couldn't reopen display\n");
		exit (1);
	    }
	    XSetCloseDownMode (retaining_display, RetainPermanent);
	    retained_root = RootWindow (retaining_display,
		    DefaultScreen (retaining_display));
	    cmap = XCreateColormap (retaining_display, retained_root,
		    DefaultVisual (retaining_display,
			DefaultScreen(retaining_display)),
		    AllocNone);
#ifdef DEBUG
	    XSync (retaining_display, False);
	    printf ("colormap created is 0x%8x\n", cmap);
#endif


/* store the atom */

	    cmapAtom = XInternAtom(display,DIRECT_COLORMAP_ATOM_NAME,0);
	    XChangeProperty (retaining_display, retained_root,
		    cmapAtom, XA_COLORMAP, 32, PropModeReplace, &cmap, 1);

	    XSync (retaining_display, False);
	    XCloseDisplay (retaining_display);
	}


/* set the window to this colormap */

	XSetWindowColormap (display,window,cmap);
	XInstallColormap (display,cmap);

/* update the pc to the new color approximation */

	masks[0] = masks[1] = masks[2] = 0;
	pcAttribs.colourApproxIndex = 1;
	PEX_SetPCAttrMaskBit(masks,PEXPCColourApproxIndex);

	if (!pc)
	{
	    pc = PEXCreatePipelineContext(display, masks, &pcAttribs);
	    rdrAttribs.pipelineContext = pc;
	    PEXChangeRenderer(display, rdr, PEXRDPipelineContext, &rdrAttribs);
	}
	else
	    PEXChangePipelineContext(display,pc,masks,&pcAttribs);

	rdrAttribs.colourApproxTable = lut;
	PEXChangeRenderer(display, rdr, PEXRDColourApproxTable, &rdrAttribs);
	PEXSetTableEntries(display,lut,PEXColourApproxLUT,1,1,&approx);

	return(cmap);

}
