/* $Header: pex_beach_ball.c,v 2.9 91/09/11 16:05:01 sinyaw Exp $ */

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

static Display *dpy;
static Drawable drawable;
static char *displayName = NULL;
static int immediateMode = 0;
static int phigsWorkstation = 0;
static XpexWksResources wks_res;

static int counter;

int duration;
int synchronize = 1;

/* PEX Resource Ids */
XpexPhigsWks    wks1;
XpexStructure   sphere;
XpexStructure   locations;
XpexStructure   root;

#define FILLAREA3D   
#define MAX_SPHERES         100

/* Labels */
#define INTERIOR_STYLE      1
#define EDGE_FLAG           2

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
int				num_spheres = 1;

/* Radius of spheres. */
static double         radius = 1.0;

void
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

    	XpexSetEditingMode(dpy, locations, XpexStructure_Insert);

		XpexStoreOutputCmds(dpy, locations);
    		XpexSetLocalTransform(dpy,Xpex_Replace, data->position);
    		XpexExecuteStructure(dpy, sphere);
    }
}

static void
set_view_rep()
{
	XpexViewEntry view_entry;
    XpexViewRep view_rep;

	InitViewEntry(&view_entry);

	view_rep.index = 1;
	view_rep.view = view_entry;

    XpexSetViewRep(dpy, wks1, &view_rep);
}

static void
build_sphere()
{
    extern int facet_sphere();
    
    XpexListOfCoord3D *facets;
    int            num_facets, num_lat = 6, num_long = 8;
    register int   i;

    num_facets = facet_sphere(radius, num_lat, num_long, &facets);
    (void) fprintf(stderr, "numfacets = %d \n", num_facets);
    
	XpexStoreOutputCmds(dpy, sphere);

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
    free(facets); 
}

static void
build_css(edge_flag, int_style)
	int	edge_flag;
	int	int_style;
{
    XpexMatrix mat, mat2, newmat;

    XpexRotateY(3.14/2, mat2);
    XpexRotateX(3.14/2, mat);
    XpexComposeMatrix(mat, mat2, newmat);

    XpexSetEditingMode(dpy, root, XpexStructure_Insert);

	XpexStoreOutputCmds(dpy, root);
    	XpexSetViewIndex(dpy, 1);
    	XpexSetHlhsrId(dpy, 1); 
    	XpexSetSurfaceColorIndex(dpy, YELLOW);
    	XpexSetSurfaceEdgeColorIndex(dpy, GREEN);
    	XpexLabel(dpy, EDGE_FLAG);
    	XpexSetSurfaceEdgeFlag(dpy, edge_flag);
    	XpexLabel(dpy, INTERIOR_STYLE);
    	XpexSetInteriorStyle(dpy, int_style);
    	XpexExecuteStructure(dpy, locations);
		XpexSetLocalTransform(dpy, Xpex_Replace, newmat); 

    build_sphere();
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
move_spheres()
{
    register Sphere_data     *data;
    register int             i;    
             XpexElementPos  elem_pos;

    XpexSetEditingMode(dpy, locations, XpexStructure_Replace);

    elem_pos.whence = Xpex_Beginning;
    elem_pos.offset = 1;
    XpexSetElementPointer(dpy, locations, &elem_pos);

    for (i = 0; i < sphere_count; i++) {
    	data = &sphere_data[i];
		NEW_RI(data->velocity.x, TIME_INC, data->position[0][3])
		NEW_RI(data->velocity.y, TIME_INC, data->position[1][3])
		NEW_RI(data->velocity.z, TIME_INC, data->position[2][3])
		XpexStoreOutputCmds(dpy, locations);
			XpexSetLocalTransform(dpy, Xpex_Replace, data->position);
		elem_pos.whence = Xpex_Current;
		elem_pos.offset = 2;
        XpexSetElementPointer(dpy, locations, &elem_pos);
    } /* end-for */
}

static void
InitWks()
{
	static void create_phigs_wks();

    int  hidden_surf = XpexHlhsr_Off;

/*
 *   ... open a workstation
 */

    create_phigs_wks();

    XpexSetupColormap(dpy, drawable, wks_res.color_approx_table); 

    XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeNone);
    XpexSetHlhsrMode(dpy, wks1, hidden_surf);
    set_view_rep();

	(void) fprintf(stderr, "wks1: %u, root: %u\n", wks1, root);
    XpexPostStructure(dpy, wks1, root, 1.0);
    XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeEach);
}

static void
Initialize()
{
	InitX();
	InitPEX();
	InitStructures();
	SetWindowLabel("pex_beach_ball - PHIGS Workstation");
	SetIconLabel("pex_beach_ball");
	drawable = CreateWindow(dpy);
	XSynchronize(dpy, synchronize); 
	InitWks();
}

InitX()
{
	dpy = XOpenDisplay(displayName);

	if (!dpy) {
		(void) fprintf(stderr, "Connection Failed\n");
		exit(-1);
	}
}

InitPEX()
{
	if (!XpexInitialize(dpy)) {
		(void) fprintf(stderr,"PEX not supported\n");
		exit(-1);
	}
	XpexSetFloatingPointFormat(dpy, Xpex_Ieee_754_32);
}

InitStructures()
{
    int  edge_flag = Xpex_On;
    int  int_style = XpexInteriorStyle_Solid;

    sphere = XpexCreateStructure(dpy);
    locations = XpexCreateStructure(dpy);
    root = XpexCreateStructure(dpy);

    build_css(edge_flag, int_style);

	while (num_spheres--) {
		add_sphere();
	}
}

static void
DrawImage(counter)
	int *counter;
{
	register int i;
    clock_reset();

    for (i = 0;;i++,(*counter)++) {
    	XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeNone);
        move_spheres();
        XpexSetDisplayUpdateMode(dpy, wks1, Xpex_VisualizeEach);
    }
}

static void
create_phigs_wks()
{
	wks_res.line_bundle =
	XpexCreateLineBundleLUT(dpy, drawable);

	wks_res.marker_bundle =
	XpexCreateMarkerBundleLUT(dpy,drawable);

	wks_res.text_bundle =
	XpexCreateTextBundleLUT(dpy,drawable);

	wks_res.interior_bundle =
	XpexCreateInteriorBundleLUT(dpy,drawable);

	wks_res.edge_bundle =
	XpexCreateEdgeBundleLUT(dpy,drawable);

	wks_res.pattern_table =
	XpexCreatePatternLUT(dpy,drawable);

	wks_res.text_font_table =
	XpexCreateTextFontLUT(dpy,drawable);

	wks_res.color_table =
	XpexCreateColorLUT(dpy,drawable);

	wks_res.light_table =
	XpexCreateLightLUT(dpy,drawable);

	wks_res.depth_cue_table =
	XpexCreateDepthCueLUT(dpy,drawable);

	wks_res.color_approx_table =
	XpexCreateColorApproxLUT(dpy,drawable);

	wks_res.highlight_incl = XpexCreateNameSet(dpy);
	wks_res.highlight_excl = XpexCreateNameSet(dpy);
	wks_res.invis_incl = XpexCreateNameSet(dpy);
	wks_res.invis_excl = XpexCreateNameSet(dpy);

	wks1 = XpexCreatePhigsWks(dpy,
	drawable, &wks_res, Xpex_SingleBuffered);
}

static XpexString cmd_line_options[] = {
	"[-display display_name]",
	"\n\t[-phigsWorkstation] or [-immediateMode]",
	"\n\t[-asynchronous]",
	"\n\t[-spheres num_spheres]",
	"\n\t[-time duration_in_seconds]",
	"\n",
	NULL
};

usage(name)
	char *name;
{
	int i;

	(void) printf("usage: %s ",name);

	for (i = 0; cmd_line_options[i] != NULL; i++) {
		(void) printf("%s", cmd_line_options[i]);
	}
}

static void
handle_args(argc, argv)
	int argc;
	char *argv[];
{
	register int i;
	static void stop_proc();

	if (argc < 2) {
		usage(argv[0]);
		exit(-1);
	}

	for (i = 1; i < argc;  i++) {
		if (!strcmp(argv[i],"-asynchronous")) {
			synchronize = 0;
		} else if (!strcmp(argv[i],"-spheres")) {
			num_spheres = atoi(argv[++i]);
		} else if (!strcmp(argv[i],"-display")) {
			displayName = argv[++i];
		} else if (!strcmp(argv[i],"-immediateMode")) {
			immediateMode = 1;
		} else if (!strcmp(argv[i],"-phigsWorkstation")) {
			phigsWorkstation = 1;
		} else if (!strcmp(argv[i],"-time")) {
			duration = atoi(argv[++i]);
		} else {
			(void) printf( 
			"%s: bad command line option \"%s\"\n\n",
			argv[0],argv[i]);

			usage(argv[0]);
			exit(-1);
		}
	}

	if (!num_spheres) {
		(void) printf("%s: can't display 0 beach_balls\n",
		argv[0], num_spheres);
		exit(-1);
	}
	if (duration > 0) {
		start_clock(duration, stop_proc, &counter);
	}
}

static void 
stop_proc(counter)
	caddr_t counter;
{
	(void) printf("%d frame(s) posted\n", *(int *) counter);
	XCloseDisplay(dpy);
	exit(0);
}

wks_beach_ball()
{
	Initialize();

	for (counter = 0;;) {
		DrawImage(&counter);
	}
}

pex_beach_ball()
{
	if (phigsWorkstation) {
		wks_beach_ball();
	}
	if (immediateMode) {
		InitX();
		InitPEX();
		imr_beach_ball(dpy);
	}
}

int
main(argc, argv)
	int argc;
	char *argv[];
{
	handle_args(argc, argv);

	pex_beach_ball();
}
