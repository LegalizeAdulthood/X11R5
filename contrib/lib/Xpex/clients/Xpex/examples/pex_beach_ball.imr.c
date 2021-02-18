/* $Header: pex_beach_ball.imr.c,v 1.5 91/09/11 16:05:26 sinyaw Exp $ */

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

/* beach_ball.Xpex.c */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"


/* PEX Resource Ids */
XpexPhigsWks    wks1;
XpexStructure   sphere;
XpexStructure   locations;
XpexStructure   root;

#define FILLAREA3D   
#define MAX_SPHERES         100

#define BLACK          0
#define WHITE          1
#define RED            2
#define GREEN          3
#define BLUE           4
#define YELLOW         5

#define WC_MIN        -5.0
#define WC_MAX         5.0

#define TIME_INC       1.0
#define VELOCITY       0.2

#if defined(SYSTYPE_SYSV) || defined(SVR4) 
#define RANDOM_FUNCTION rand
#define MAX_RAND 32767
#else
#define RANDOM_FUNCTION random
#define MAX_RAND 2147483647
#endif

#ifndef MIN
#define MIN( a, b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX( a, b)    (((a) > (b)) ? (a) : (b))
#endif

/* Sphere velocity and location data. */
typedef struct {
    XpexVector3D  velocity;
    XpexMatrix    position;
} Sphere_data;

static XpexMatrix identity = { 
	1.0, 0.0, 0.0, 0.0,
   	0.0, 1.0, 0.0, 0.0,
   	0.0, 0.0, 1.0, 0.0,
   	0.0, 0.0, 0.0, 1.0 
};

static Sphere_data    sphere_data[MAX_SPHERES];
static int            sphere_count = 0;
static int				num_spheres = 1;

static Display *dpy;
static Drawable drawable;
static char *dpy_name = NULL;
static XpexRenderer rdr;
static XpexRendererAttributes rdr_attrs;

static void
random_velocity(v)
XpexVector3D *v;
{
    extern int  RANDOM_FUNCTION();

    int         Xr, Yr;
    float   Xv, Yv, Zv, XYv, V2;

    Xv = Yv = Zv = 0.0;
    V2 = VELOCITY * VELOCITY;

    Xr = RANDOM_FUNCTION();
    Xv = ((float)Xr/(float)MAX_RAND) * (VELOCITY/sqrt(3.0));
    if ( Xr % 2 )
    Xv = - Xv;

    Yr = RANDOM_FUNCTION();
    Yv = ((float)Yr/(float)MAX_RAND) * sqrt(V2 - Xv * Xv);
    if ( Yr % 2 )
    Yv = - Yv;

    XYv = sqrt(Xv * Xv + Yv * Yv);
    if ( XYv < VELOCITY ) {
    Zv = sqrt(V2 - XYv * XYv);
    if ( RANDOM_FUNCTION() % 2 )
        Zv = - Zv;
    }

    v->x = Xv;
    v->y = Yv;
    v->z = Zv;
}

static void
add_sphere()
{
    register Sphere_data *data;

    if (sphere_count < MAX_SPHERES) {
    	data = &sphere_data[sphere_count++];
    	memcpy(data->position, identity, sizeof(XpexMatrix));
    	random_velocity(&data->velocity);
    }
}

DrawSphere()
{
	extern int facet_sphere();

	/* Radius of spheres. */
	static double         radius = 1.0;

	XpexListOfCoord3D *facets;
	int num_facets, num_lat = 6, num_long = 8;
	register int i;

	num_facets = facet_sphere(radius, num_lat, num_long, &facets);
	/* (void) fprintf(stderr, "numfacets = %d\n", num_facets); */

	for (i = 0; i < num_facets; i++) {
		XpexSetSurfaceColorIndex(dpy, (i % 6) + 2);
#ifdef FILLAREA3D
		XpexFillArea(dpy, Xpex_UnknownShape, True,
		facets[i].points, facets[i].num_points);
#else /* !FILLAREA3D */
		XpexFillAreaSet(dpy, Xpex_UnknownShape,
		True, Xpex_UnknownContour, &facets[i], 1);
#endif /* FILLAREA3D */
	}
	free((char *)facets);
}

#define NEW_RI( _v, _dt, _r ) \
    { \
    (_r) += (_v) * (_dt); \
    if ( (_r) >= WC_MAX ) { \
         (_r) = 2.0 * WC_MAX - (_r); \
         (_v) = -(_v); \
    } else if ( (_r) <= WC_MIN ) { \
         (_r) = 2.0 * WC_MIN - (_r); \
         (_v) = -(_v); \
    } \
    }

static void
MoveSpheres()
{
	register Sphere_data *data;
	register int i;

	for (i = 0; i < sphere_count; i++) {
		data = &sphere_data[i];
		NEW_RI(data->velocity.x, TIME_INC, data->position[0][3])
		NEW_RI(data->velocity.y, TIME_INC, data->position[1][3])
		NEW_RI(data->velocity.z, TIME_INC, data->position[2][3])
		XpexSetLocalTransform(dpy, Xpex_Replace, data->position);
		DrawSphere();
	}
}


static void
Initialize()
{
	InitX();
	InitPEX();
	InitXresources();
	InitPEXresources();

	while (num_spheres--) {
		add_sphere();
	}
}

InitX()
{
	dpy = XOpenDisplay(NULL);

	if (!dpy) {
		(void) fprintf(stderr, "Connection Failed\n");
		exit(-1);
	}
	XSynchronize(dpy, 1); 
}

InitPEX()
{
	if (!XpexInitialize(dpy)) {
		(void) fprintf(stderr,"PEX not supported\n");
		exit(-1);
	}
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32);
}

InitXresources()
{
	SetWindowLabel("pex_beach_ball - immediate mode");
	SetIconLabel("pex_beach_ball");
	drawable = CreateWindow(dpy);
}

InitPEXresources()
{
	rdr_attrs.color_approx_table = 
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorApproxLUT);

	rdr_attrs.view_table =
	XpexCreateLookupTable(dpy, drawable, Xpex_ViewLUT);

	rdr_attrs.color_table =
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorLUT);

	rdr = XpexCreateRenderer(dpy,
		drawable, 
		XpexRD_ColorApproxTable | 
		XpexRD_ViewTable | 
		XpexRD_ColorTable,
		&rdr_attrs);

	XpexSetupColormap(dpy,
		drawable,
		rdr_attrs.color_approx_table);

	InitView();
}

InitView()
{
	static XpexCoord3D   vrp = { 0.0, 0.0, 0.0 };
	static XpexVector3D  vup = { 0.0, 0.0, 1.0 };
	static XpexVector3D  vpn = { .680, -1.0, .260 };

	XpexViewEntry  view;
	XpexViewMap3D  viewmap;

    viewmap.proj_type = Xpex_PARALLEL;
    viewmap.proj_ref_point.z = 6.0 * WC_MAX;
    viewmap.window.x_min = WC_MIN; 
	viewmap.window.x_max = WC_MAX;
    viewmap.window.y_min = WC_MIN; 
	viewmap.window.y_max = WC_MAX;
    viewmap.front_plane = 2.0 * WC_MAX;
    viewmap.back_plane =  2.0 * WC_MIN;
    viewmap.view_plane =  0.4 * viewmap.proj_ref_point.z;
    viewmap.proj_ref_point.x = 
	(viewmap.window.x_min + viewmap.window.x_max) / 2.0;
    viewmap.proj_ref_point.y = 
	(viewmap.window.y_min + viewmap.window.y_max) / 2.0;
    viewmap.viewport.x_min = 0.0; 
	viewmap.viewport.x_max = 1.0;
    viewmap.viewport.y_min = 0.0; 
	viewmap.viewport.y_max = 1.0;
    viewmap.viewport.z_min = 0.0; 
	viewmap.viewport.z_max = 1.0;

    if (XpexEvaluateViewMappingMatrix(&viewmap, view.mapping)) {
		(void) fprintf(stderr, "Error from eval mapping\n");
	}

    if (XpexEvaluateViewOrientationMatrix(&vrp, &vpn, &vup, 
	view.orientation)) {
        (void) fprintf(stderr, "Error from eval orientation\n");
	}

    view.clipFlags = Xpex_ClipXY | Xpex_ClipBack | Xpex_ClipFront;
    view.clipLimits.minval.x = viewmap.viewport.x_min;
    view.clipLimits.minval.y = viewmap.viewport.y_min;
    view.clipLimits.minval.z = viewmap.viewport.z_min;
    view.clipLimits.maxval.x = viewmap.viewport.x_max;
    view.clipLimits.maxval.y = viewmap.viewport.y_max;
    view.clipLimits.maxval.z = viewmap.viewport.z_max;

	XpexSetTableEntries(dpy, rdr_attrs.view_table,
	Xpex_ViewLUT, 1, 1, (char *) &view, sizeof(XpexViewEntry));
}

static void
DrawImage()
{
    clock_reset();
	XpexRenderOutputCmds(dpy, rdr);
    for (;;) {
		XClearWindow(dpy, drawable);
		XpexBeginRendering(dpy, rdr, drawable);
		XpexSetViewIndex(dpy, 1);
		XpexSetHlhsrId(dpy, 1);
		XpexSetSurfaceColorIndex(dpy, YELLOW);
		XpexSetSurfaceEdgeColorIndex(dpy, GREEN);
		XpexSetInteriorStyle(dpy, XpexInteriorStyle_Solid); 
        MoveSpheres();
		XpexEndRendering(dpy, rdr, False);
    }
}

static void
HandleArgs(argc, argv)
	int argc;
	char *argv[];
{
	if (argc > 1) 
		num_spheres = atoi(argv[1]);

	if (!num_spheres) {
		(void) printf("%s: can't display 0 beach_balls\n",
		argv[0], num_spheres);
		exit(1);
	}
}

main( argc, argv)
	int argc;
	char *argv[];
{
	HandleArgs(argc, argv);

	Initialize();

	DrawImage();
}
