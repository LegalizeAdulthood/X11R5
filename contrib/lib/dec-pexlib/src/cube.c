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
 *	cube.c
 * 
 *  Subsystem:
 *
 *  Version: 
 *	V1
 *
 *  Abstract:
 *	    Test program.
 *  Keywords:
 *
 *  Environment:
 *	VMS, User mode
 *
 *  Creation Date: 
 *
 *  Modification History:
 *
 */


#include <stdio.h>
#include <math.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "PEXlib.h"
#include "PEXproto.h"
#include "MPEXlib.h"
 
float v1x[12] =
       {
       -1,   -1,    1, 
       -1,    1,    1, 
       -1,    1,   -1, 
       -1,   -1,   -1, 
 	};

float v2x[12] =
       {
        1,   -1,    1, 
        1,   -1,   -1, 
        1,    1,   -1, 
        1,    1,    1, 
 	};

float v3x[12] =
       {
       -1,   -1,    1, 
        1,   -1,    1, 
        1,    1,    1, 
       -1,    1,    1, 
 	};

float v4x[12] =
       {
       -1,   -1,    -1,
       -1,    1,    -1,
        1,    1,    -1,
        1,   -1,    -1,
 	};

float v5x[12] =
       {
        1,    -1,    -1,
        1,    -1,    1, 
       -1,    -1,    1, 
       -1,    -1,    -1,
 	};

float v6x[12] =
       {
       -1,    1,    -1, 
       -1,    1,    1, 
        1,    1,    1, 
        1,    1,    -1, 
 	};

#define DEGRAD(_a) ((_a)*3.1415926/180.0)
#define	TEXT_OFFSET	.3
#define	TEXT_EPS	.05


#define	INIT_NEAR	5.0
#define	INIT_FAR	-5.0

#define	TITLE	"PEX Rotating Cube Test"

extern Colormap CreatePseudoColormap();


main(argc,argv)
int	argc;
char	*argv[];
{

	Display			*disp;		/* X disp */
	Window			window;		/* target window */
	Pixmap			pixmap;		/* for double buffering */
	Colormap		colormap;

	pxlInfo			*pexinfo;	/* PEX initialization info */
	pxlCoord3D		from,to,up;	/* viewing vectors */
	pxlLookupTable		viewLut;	/* view look up table */
	pxlViewEntry		view;		/* view data */
	pexRenderer		rd;		/* renderer */
	pxlRendererAttributes	attribs;	/* renderer attributes */
	pxlOCBuf		*oc;		/* output command buffer */
	pxlRgbFloatColour 	surfaceColour;
	pxlRgbFloatColour 	BFsurfaceColor;
	int			mask;
	pxlLocalTransform3DData	matrix;
	pexMatrix		temp;
	pexMatrix		temp2;
	float			rotation;
	pxlLookupTable		lightLut;
	pxlLightEntry		light;
	float			height = 4.0;
	float			fovy = DEGRAD(3.0);
	float			distance = 90.0;
	float			aspect = 1.0;
	float			near = INIT_NEAR;
	float			far  = INIT_FAR;
	
	mpxlRendererAttributes	newAttbs;

	int			clear_i = 0;
	int			clip_draw = 0;
	int			clip_clear = 0;
	float			ang;
	int			doubleBuffer = 1;	/* for double buffering*/
	
	from.x = from.y = to.x = to.y = to.z = up.x = up.z = 0.0;
	from.z = 90.0;
	up.y = 1.0;

/* open up X and PEX */

	disp = XOpenDisplay(0);
	if (disp == NULL)
	{
	    fprintf (stderr, "Could not open display \n");
	    exit (0);
	}
	if (PEXInitialize(disp, &pexinfo) )
	{
	    fprintf (stderr, "Could not get pex on display \n");
	    exit (0);	    
	}
	
/* create a window and such */

	window = XCreateSimpleWindow(disp, RootWindow(disp,DefaultScreen(disp)),
			100,100,500,500,1,
			WhitePixel(disp,DefaultScreen(disp)),
       			BlackPixel(disp,DefaultScreen(disp)));
	pixmap = XCreatePixmap(disp, window, 500,500, 
		DefaultDepth(disp, DefaultScreen(disp)));

	/* clear pixmap - clear image won't clear entire pixmap */
	XFillRectangle(disp, pixmap, DefaultGC(disp,DefaultScreen(disp)),
					0, 0, 500,500);

	XMapWindow(disp, window);
	
/* create PEX viewing stuff */
 
/* view 1 */
	viewLut = PEXCreateLookupTable(disp, window, PEXViewLUT);
	view.clipFlags = (PEXClipXY | PEXClipBack | PEXClipFront);
	view.clipLimits.minval.x = 0.0;
	view.clipLimits.minval.y = 0.0;
	view.clipLimits.minval.z = 0.0;
	view.clipLimits.maxval.x = 1.0;
	view.clipLimits.maxval.y = 1.0;
	view.clipLimits.maxval.z = 1.0;
	PEXPerspProjMatrix(fovy, distance, aspect,
			near, far, view.mapping);
	PEXLookatViewMatrix(&from,&to,&up,view.orientation);
	PEXSetTableEntries(disp, viewLut, PEXViewLUT, 1, 1, &view);

/* create the light stuff */

	lightLut = PEXCreateLookupTable(disp,window,PEXLightLUT);

	light.lightType = PEXLightAmbient;
	light.lightColour.colourType = PEXRgbFloatColour;
	light.lightColour.colour.format.rgbFloat.red = 1.0;
	light.lightColour.colour.format.rgbFloat.green = 1.0;
	light.lightColour.colour.format.rgbFloat.blue = 1.0;
	PEXSetTableEntries(disp,lightLut,PEXLightLUT,1,1,&light);

/* create the PEX renderer */
	
	attribs.viewTable = viewLut;
	attribs.lightTable = lightLut;
	attribs.hlhsrMode = PEXHlhsrZBuffer;
	mask = PEXRDLightTable|PEXRDHlhsrMode|PEXRDViewTable;

	rd = PEXCreateRenderer(disp, window, mask, &attribs);
	colormap = CreatePseudoColormap(disp,window,rd);

/* set some MPEX attributes */
 
    {
	XColor		bg_color;
	unsigned long	black;
	
	bg_color.pixel = BlackPixel(disp,DefaultScreen(disp));
	bg_color.red = 0;
	bg_color.green = 0;
	bg_color.blue = 32768;
	bg_color.flags = DoRed | DoGreen | DoBlue;
	XAllocColor (disp, colormap, &bg_color);

	newAttbs.backgroundPixel = bg_color.pixel;
	newAttbs.clearI = clear_i;

    }
	newAttbs.clearZ = 1;

	MPEXChangeNewRenderer(disp,rd,MPEXNRAClearI|MPEXNRAClearZ|MPEXNRABackgroundPixel
		|MPEXNRAClearI,&newAttbs);

/* build an oc buffer */

	oc = (pxlOCBuf *)PEXAllocateOCBuffer(disp, pxlRenderImmediate, rd,
   	     PEXDefaultRetainedOCBuffer, PEXDefaultOCError, 2048);

/* rotating loop */

	matrix.composition = PEXReplace;
	for (ang=0;;ang+=5)
	{

/* Begin PEXRendering */

	    if (!doubleBuffer)
	    {
		XClearWindow(disp,window);
		PEXBeginRendering(disp, window, rd);
	    }
	    else
	    {
		XFillRectangle(disp, pixmap, 
			DefaultGC(disp,DefaultScreen(disp)), 0, 0,
			500,500);
		PEXBeginRendering(disp, pixmap, rd);
	    }


/* set the view index and polyline color */
 
	   PEXSetViewIndex(oc, 1);
	   PEXSetColourApproxIndex(oc,1);
	   PEXSetSurfaceInterpMethod(oc,PEXSurfaceInterpColour);
	   PEXSetBFSurfaceInterpMethod(oc,PEXSurfaceInterpColour);
	   PEXSetInteriorStyle(oc,PEXInteriorStyleSolid);
	   PEXSetFacetDistinguishFlag(oc,False);

	    rotation = DEGRAD(ang);
	    PEXRotationMatrix(pxlYAxis, rotation, matrix.matrix);
	    PEXRotationMatrix(pxlXAxis, 2*rotation, temp);
	    PEXRotationMatrix(pxlZAxis, rotation, temp2);
	    PEXMultiplyMatrices(temp,temp2,temp);
	    PEXMultiplyMatrices(matrix.matrix,temp,matrix.matrix);

	/* set the local transformation */

	    PEXSetLocalTransform(oc, &matrix);

	/* draw  */

		    surfaceColour.red = 1.0;
		    surfaceColour.green = 0.0;
		    surfaceColour.blue = 0.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v1x, 4);

		    surfaceColour.red = 0.0;
		    surfaceColour.green = 1.0;
		    surfaceColour.blue = 0.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v2x, 4);


		    surfaceColour.red = 0.0;
		    surfaceColour.green = 0.0;
		    surfaceColour.blue = 1.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v3x, 4);

		    surfaceColour.red = 1.0;
		    surfaceColour.green = 1.0;
		    surfaceColour.blue = 0.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v4x, 4);

		    surfaceColour.red = 0.0;
		    surfaceColour.green = 1.0;
		    surfaceColour.blue = 1.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v5x, 4);

		    surfaceColour.red = 0.0;
		    surfaceColour.green = 0.5;
		    surfaceColour.blue = 1.0;

		    BFsurfaceColor.red = 6.0;
		    BFsurfaceColor.green = 0.0;
		    BFsurfaceColor.blue = 0.0;
		    PEXSetSurfaceColour(oc, &surfaceColour);
		    PEXSetBFSurfaceColour(oc, &BFsurfaceColor);
		    PEXFillArea(oc, PEXConvex, 1, v6x, 4);

/* setup some things if we're doing normal text */

	    {
		pxlColour	textColour;
		pxlCoord3D	xaxis,yaxis,origin;

		PEXSetCharHeight(oc,.3);
		  textColour.format.rgbFloat.red = 1.0;
		  textColour.format.rgbFloat.blue = 1.0;
		  textColour.format.rgbFloat.green = 1.0;

		PEXSetTextColour(oc,&textColour);

		xaxis.x = 1.0;
		xaxis.y = 0.0;
		xaxis.z = 0.0;
		yaxis.x = 0.0;
		yaxis.y = 1.0;
		yaxis.z = 0.0;
		origin.x = -TEXT_OFFSET - .2;
		origin.y = -TEXT_OFFSET;
		origin.z = 1.0 + TEXT_EPS;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Front",5); 

		xaxis.x = -1.0;
		xaxis.y = 0.0;
		xaxis.z = 0.0;
		yaxis.x = 0.0;
		yaxis.y = 1.0;
		yaxis.z = 0.0;
		origin.x = TEXT_OFFSET;
		origin.y = -TEXT_OFFSET;
		origin.z = -1.0 - TEXT_EPS;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Back",4); 

		xaxis.x = 0.0;
		xaxis.y = 0.0;
		xaxis.z = 1.0;
		yaxis.x = 0.0;
		yaxis.y = 1.0;
		yaxis.z = 0.0;
		origin.x = -1.0 - TEXT_EPS;
		origin.y = -TEXT_OFFSET;
		origin.z = -TEXT_OFFSET;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Left",4); 

		xaxis.x = 0.0;
		xaxis.y = 0.0;
		xaxis.z = -1.0;
		yaxis.x = 0.0;
		yaxis.y = 1.0;
		yaxis.z = 0.0;
		origin.x = 1.0 + TEXT_EPS;
		origin.y = -TEXT_OFFSET;
		origin.z = TEXT_OFFSET;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Right",5); 


		xaxis.x = 1.0;
		xaxis.y = 0.0;
		xaxis.z = 0.0;
		yaxis.x = 0.0;
		yaxis.y = 0.0;
		yaxis.z = -1.0;
		origin.x = -TEXT_OFFSET;
		origin.y = 1.0 + TEXT_EPS;
		origin.z = TEXT_OFFSET;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Top",3); 

		xaxis.x = -1.0;
		xaxis.y = 0.0;
		xaxis.z = 0.0;
		yaxis.x = 0.0;
		yaxis.y = 0.0;
		yaxis.z = 1.0;
		origin.x = TEXT_OFFSET + .2;
		origin.y = -1.0 - TEXT_EPS;
		origin.z = -TEXT_OFFSET;
	        PEXText (oc, &origin, &xaxis, &yaxis,"Bottom",6); 
	    }

	    /* send the buffer down to PEX and draw on the pixmap */ 

	     PEXFlushOCBuffer(oc);

	    /* copy the pixmap to the window */

	   PEXEndRendering(disp, rd, 1);

	   if (doubleBuffer)
		XCopyArea(disp, pixmap, window, 
			DefaultGC(disp, DefaultScreen(disp)), 0, 0, 
			500,500, 0,0);
	   XSync(disp,False);

	}

}



static Colormap CreatePseudoColormap(display,window,rdr)
Display		*display;
Window		window;
pexRenderer	rdr;
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
	pxlPipelineContext	pc=0;
	float			gamma = 2.3;

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
		printf("Cannot alocate %n color cells - we die\n",colorCount);
		exit(1);
	    }

/* we have the color cells, now look for the set of contiguous cells
   because X may return back some cells which are left over from
   other planes, but the planes are continugous */

	    {
		int	minPixel[4],maxPixel[4],startIndex[4];
		int	lastPixel,lastMin,largest,index;

		lastMin = 0;
		lastPixel = minPixel[lastMin] = pixels[0];
		startIndex[lastMin] = 0;
		for (i=1;i<colorCount;i++)
		{
		    if (pixels[i] != ++lastPixel)
		    {
			if (lastMin >= 4)
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

	    XStoreColors(display, cmap,color_def,i);
	    XFlush(display);

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
