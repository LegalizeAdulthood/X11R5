#ifndef __Vlib
#define __Vlib

#define VmaxVP	32		/* max # of vertices in a polygon */

#define MAXCOLORS 8		/* max number of colors available */
				/* when double buffering */

#define MAXPCOLORS	256	/* maximum colors available when */
				/* using a pixmap */

#if (MAXCOLORS==4)
#define PLANES 2
#endif
#if (MAXCOLORS==8)
#define PLANES 3
#endif
#if (MAXCOLORS==16)
#define PLANES 4
#endif

#include <X11/Xlib.h>
#include <stdio.h>
#include <memory.h>

typedef struct _vcolor {
	char	*color_name;	/* text name */
	short	index;		/* index into pixel value table */
	XColor	xcolor;		/* representation of this color */
	struct _vcolor *next;
	} VColor;

typedef struct {
	double	x, y, z;	/* A point or vector in 3-space */
	} VPoint;

typedef struct {
	short	numVtces;	/* vertex count */
	VPoint	*vertex;	/* pointer to array of vertices */
	VColor	*color;		/* pointer to color descriptor */
	} VPolygon;

typedef struct {
	char	*name;		/* object name */
	int	numPolys;	/* polygon count */
	VPolygon **polygon;	/* pointer to array of polygon pointers */
	} VObject;

typedef struct {
	double	m[4][4];
	} VMatrix;

typedef struct {
	unsigned long flags;	/* viewport flags */
	VMatrix	  eyeSpace;	/* transforms from world to eyeSpace system */
	VPolygon  *clipPoly;	/* planes to clip viewed polygons */
	double	  units;	/* world units expressed in meters */
	double	  dist;		/* distance in units from eye to screen */
	double	  xres;		/* x screen resolution in dots per unit */
	double	  yres;		/* y screen resolution in dots per unit */
	int	  width;	/* width of window in dots */
	int	  height;	/* height of window in dots */
	VPoint	  Middl;	/* center of window */
	VPoint	  Scale;	/* scaling factor */
	Display	  *dpy;		/* Display associated with this viewport */
	int	  screen;	/* X screen number */
	Window	  win;
	Pixmap	  monoPixmap;	/* Pixmap used to buffer drawing */

	int	  colors;	/* color count */
	int	  set;		/* id of buffer currently in use */
	unsigned long mask;	/* current plane mask */
	unsigned long aMask, bMask;
	unsigned long *pixel;	/* current pixel drawing values */
	unsigned long aPixel[MAXPCOLORS];
	unsigned long bPixel[MAXCOLORS];
	XColor	  aColor[MAXCOLORS*MAXCOLORS];
	XColor	  bColor[MAXCOLORS*MAXCOLORS];
	Colormap  cmap;
	} Viewport;

#define VGetPolygonPixel(p)		(p->color->xcolor.pixel)
#define VLookupViewportColor(v,n)	(v->pixel[n])
#define VGetViewportMask(v) 		(v->mask)
#define VConstantColor(v,index)		(v->aPixel[index] | v->bPixel[index])

/*
 *  Globals
 */

VColor   *VColorList;		/* list of colors needed for drawing */
int	 usePixmaps;		/* set to one to use Pixmaps on color
					viewports */

/*
 *  V macros and function prototypes
 */

#define VDestroyPoints(a)	free((char *) a)
#define VDestroyPolygon(a)	{free((char *) a->vertex); free((char *) a);}

Viewport *VOpenViewport(); 	/* (Display *, .. ) */
void	 VResizeViewport();	/* (Viewport *, ... ) */
void	 VCloseViewport();	/* (Viewport *) */
VPolygon *VCreatePolygon();	/* (VPoint *, int, VColor *) */
VPolygon *VCopyPolygon();	/* (VPolygon *) */
VPolygon *VClipPolygon();	/* (VPolygon *, VPoint) */
VPoint	 *VCreatePoints();	/* (int) */
VColor	 *VAllocColor();	/* (char *) */
int	 VBindColors();		/* (Viewport *, char *) */
void	 VExposeBuffer();	/* (Viewport *, GC) */
VObject	 *VReadObject();	/* (FILE *) */
int	 VWriteObject();	/* (FILE *, VObject *) */
VMatrix	 *VRotate();		/* (VMatrix *, int) */
VMatrix  *VTranslate();		/* (VMatrix *, double, double, double) */
VMatrix  *VTranslatePoint();	/* (VMatrix *, VPoint) */
double	 VMatrixDeterminant();	/* (VMatrix *) */
VMatrix  *VMatrixInvert();	/* (VMatrix *, VMatrix *) */
void	 VTransform();		/* (VPoint *, VMatrix *, VPoint *) */
double	 VDotProd();		/* (Vpoint *, VPoint *) */
void	 VCrossProd();		/* (Vpoint *, VPoint *, VPoint *) */
char	 *Vmalloc();		/* (int) */
int	 VEyeToScreen();
int	 VWorldToScreen();
void	 VMatrixMult();		/* (VMatrix *, VMatrix *, VMatrix *) */
void	 VMatrixMultByRank();	/* (VMatrix *, VMatrix *, VMatrix *, int) */

/*
 *  Viewport flags (must be changed manually after VOpenViewport for now)
 */

#define	VPClip		1	/* polygons should be clipped before drawing */
#define VPPerspective	2	/* Z coordinate used for depth information */
#define VPMono		4	/* Monochrome environment */
#define VPPixmap	8	/* Use color Pixmap rather than double
					buffering */

/*
 *  VRotate options
 */

#define XRotation	1	/* rotate about X axis */
#define YRotation	2	/* rotate about Y axis */
#define ZRotation	3	/* rotate about Z axis */

#ifndef SYSV
extern char *strdup();
#endif

#endif
