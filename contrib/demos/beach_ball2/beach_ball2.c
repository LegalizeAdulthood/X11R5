#ifndef lint
static char     sccsid[] = "@(#)beach_ball.c 3.1 90/05/29 Copyr 1989,1990  SMI/MIT";
#endif
/***********************************************************
Copyright 1989,1990 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Copyright (c) 1989,1990 by M.I.T. and Sun Microsystems, Inc.
 */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <phigs/phigs.h>

#ifdef USE_X_DRAWABLE
extern void init_window();
extern Display *appl_display;
extern Window appl_window;
#endif /*USE_X_DRAWABLE*/


#define	WS_ID 			1
#define MAX_SPHERES  100
#define SPHERE         1
#define LOCATIONS      2
#define ROOT           3

#define LINTSTYLE      1
#define LEDGEFLAG      2

#define BLACK          0
#define WHITE          1
#define RED            2
#define GREEN          3
#define BLUE           4
#define YELLOW         5
#define CYAN           6
#define MAGENTA        7

#define WC_MIN        -5.0
#define WC_MAX         5.0

#define TIME_INC       1.0
#define VELOCITY       0.2

#define MAX_RAND    2147483647
#define PFILLAREASET3

#ifndef MIN
#define MIN( a, b)    (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX( a, b)    (((a) > (b)) ? (a) : (b))
#endif

/* Sphere velocity and location data. */
typedef struct {
    Pvec3          velocity;
    Pmatrix3          position;
} Sphere_data;

static Pmatrix3      identity = { 1.0, 0.0, 0.0, 0.0,
                                  0.0, 1.0, 0.0, 0.0,
                                  0.0, 0.0, 1.0, 0.0,
                                  0.0, 0.0, 0.0, 1.0 };

static Ppoint3 boundary1[5] = {
	{ 5.0,  5.0, -5.0},
	{ 5.0, -5.0, -5.0},
	{-5.0, -5.0, -5.0},
	{-5.0,  5.0, -5.0},
	{ 5.0,  5.0, -5.0}
};
static Ppoint3 boundary2[5] = {
	{ 5.0,  5.0,  5.0},
	{ 5.0, -5.0,  5.0},
	{-5.0, -5.0,  5.0},
	{-5.0,  5.0,  5.0},
	{ 5.0,  5.0,  5.0}
};
static Ppoint3 boundary3[2] = {
	{ 5.0,  5.0, -5.0},
	{ 5.0,  5.0,  5.0}
};
static Ppoint3 boundary4[2] = {
	{ 5.0, -5.0, -5.0},
	{ 5.0, -5.0,  5.0}
};
static Ppoint3 boundary5[2] = {
	{-5.0, -5.0, -5.0},
	{-5.0, -5.0,  5.0}
};
static Ppoint3 boundary6[2] = {
	{-5.0,  5.0, -5.0},
	{-5.0,  5.0,  5.0}
};
	
static Sphere_data    sphere_data[MAX_SPHERES];
static int            sphere_count = 0;
static int				num_spheres = 1;

Pint	colors_sw = 0,
	shading_sw = 0,
	solid_sw = 0,
	edges_sw = 0;

/* Radius of spheres. */
static double         radius = 1.0;

/* Viewing parameters */
static Pview_rep3      rep;
static Pview_map3  map;
static Ppoint3        vrp = { 0.0, 0.0, 0.0};
static Pvec3       vup = { 0.0, 0.0, 1.0};
static Pvec3       vpn = { .680, -1.0, .260};

static void
random_velocity(v)
Pvec3 *v;
{
    extern int  random();

    int         Xr, Yr;
    Pfloat      Xv, Yv, Zv, XYv, V2;

    Xv = Yv = Zv = 0.0;
    V2 = VELOCITY * VELOCITY;

    Xr = random();
    Xv = ((float)Xr/(float)MAX_RAND) * (VELOCITY/sqrt(3.0));
    if ( Xr % 2 )
    Xv = - Xv;

    Yr = random();
    Yv = ((float)Yr/(float)MAX_RAND) * sqrt(V2 - Xv * Xv);
    if ( Yr % 2 )
    Yv = - Yv;

    XYv = sqrt(Xv * Xv + Yv * Yv);
    if ( XYv < VELOCITY ) {
    Zv = sqrt(V2 - XYv * XYv);
    if ( random() % 2 )
        Zv = - Zv;
    }

    v->delta_x = Xv;
    v->delta_y = Yv;
    v->delta_z = Zv;
}

static void
add_sphere()
{
    register Sphere_data *data;

    if (sphere_count < MAX_SPHERES) {
    data = &sphere_data[sphere_count++];
    memcpy(data->position, identity, sizeof(Pmatrix3));
    random_velocity(&data->velocity );

    pset_edit_mode(PEDIT_INSERT);
    popen_struct(LOCATIONS);
        pset_local_tran3(data->position, PTYPE_REPLACE);
        pexec_struct(SPHERE);
    pclose_struct();
    pset_edit_mode(PEDIT_REPLACE);
    }
}

static void
init_view_mapping()
{
    map.proj_type = PTYPE_PERSPECT;
    map.proj_ref_point.z = 6.0 * WC_MAX;
    map.win.x_min = WC_MIN; map.win.x_max = WC_MAX;
    map.win.y_min = WC_MIN; map.win.y_max = WC_MAX;
    map.front_plane = 2.0 * WC_MAX;
    map.back_plane =  2.0 * WC_MIN;
    map.view_plane =  0.4 * map.proj_ref_point.z;
    map.proj_ref_point.x = (map.win.x_min + map.win.x_max) / 2.0;
    map.proj_ref_point.y = (map.win.y_min + map.win.y_max) / 2.0;
    map.proj_vp.x_min = 0.0; map.proj_vp.x_max = 1.0;
    map.proj_vp.y_min = 0.0; map.proj_vp.y_max = 1.0;
    map.proj_vp.z_min = 0.0; map.proj_vp.z_max = 1.0;
}

static void
eval_view_rep(rep)
Pview_rep3  *rep;
{
    Pint err;

    peval_view_map_matrix3( &map, &err, rep->map_matrix);

    if (err) 
    	(void) fprintf(stderr, "Error from eval mapping %d\n", err);

    peval_view_ori_matrix3( &vrp, &vpn, &vup, &err, rep->ori_matrix);

    if (err) 
    	(void) fprintf(stderr, "Error from eval orientation %d\n", err);
    
}

static void
set_view_rep()
{
    eval_view_rep(&rep);
    rep.clip_limit = map.proj_vp;
    rep.xy_clip = rep.back_clip = rep.front_clip = PIND_CLIP;
    pset_view_rep3(1, 1, &rep);
}

static void
build_sphere()
{
    Ppoint_list3 *facets;
    int            num_facets, num_lat = 6, num_long = 8;
    register int   i, j;
    Plight_src_bundle       lrep;                   /* light source */
    Pint_list       act, deact;                     /*  setting     */
    Prefl_props     aprop;                          /* area property*/
    Pint		shade_meth;

    popen_struct( SPHERE);
	if(shading_sw)
	{
        	/* LOAD LIGHT SOURCE REPS */

        	/* light 1 */

        	lrep.type = PLIGHT_AMBIENT;
        	lrep.rec.ambient.colr.type = PINDIRECT;
        	lrep.rec.ambient.colr.val.ind /* WARNING: may be index */= WHITE;
        	pset_light_src_rep(WS_ID, 1, &lrep);

        	/* light 2 */

        	lrep.type = PLIGHT_POSITIONAL;
        	lrep.rec.positional.colr.type = PINDIRECT;
        	lrep.rec.positional.colr.val.ind /* WARNING: may be index */= WHITE;
        	lrep.rec.positional.pos.x = 0.0;
        	lrep.rec.positional.pos.y = 1.0;
        	lrep.rec.positional.pos.z = 3.0;
        	lrep.rec.positional.coef[0] = 0.4;
        	lrep.rec.positional.coef[1] = 0.2;
        	pset_light_src_rep(WS_ID, 2, &lrep);

        	act.num_ints = 2;
        	deact.num_ints = 0;
        	act.ints = (Pint *) malloc(5*sizeof(Pint));
        	deact.ints = (Pint *) malloc(5*sizeof(Pint));

        	act.ints[0] = 1;
        	act.ints[1] = 2;

                aprop.ambient_coef = 0.3;
                aprop.diffuse_coef = 0.8;
                aprop.specular_coef = 0.7;
                aprop.specular_colr.type = PINDIRECT;
                aprop.specular_colr.val.ind /* WARNING: may be index */= WHITE;
                aprop.specular_exp = 4.0;

                pset_refl_props(&aprop);                 /* 6 - area prop. */

fprintf(stderr,"enter shading method : \n");
fprintf(stderr,"  1 = no shading   2 = flat shading\n");
fprintf(stderr,"  3 = dot product  4 - normal (phong)   :");
fscanf(stdin, "%d", &shade_meth);
getchar();
                pset_int_shad_meth(shade_meth);      /* 7 - shad methd */

                /*pset_refl_eqn(1);*/             /* 8 - refl eq */

                pset_light_src_state(&act, &deact);     /* 9 - lgt conf */

	}
	my_build_sphere(colors_sw);
    pclose_struct();
    free(facets);
}

static void
build_css(edge_flag, int_style)
Pedge_flag  edge_flag;
Pint_style  int_style;
{
    Ppoint3     axes_origin, axes_length;
    Pint        axes_color[3];
    Pint        err;
    Pmatrix3    mat, mat2, newmat;
    Ppoint_list3 plist3;

    axes_origin.x = axes_origin.y = axes_origin.z = 0;
    axes_length.x = axes_length.y = axes_length.z = 1.5;
    axes_color[0] = RED; axes_color[1] = GREEN; axes_color[2] = BLUE;

    popen_struct(ROOT);
    pset_view_ind(1);
    pset_hlhsr_id(PHIGS_HLHSR_ID_ON);
    pset_int_colr_ind(YELLOW);
    pset_edge_colr_ind(GREEN);

    /* Draw a 3D boundary */
    pset_line_colr_ind(WHITE);
    plist3.num_points = 5;
    plist3.points = boundary1;
    /*WARNING: parameters changed */ ppolyline3(&plist3);
    plist3.num_points = 5;
    plist3.points = boundary2;
    /*WARNING: parameters changed */ ppolyline3(&plist3);
    plist3.num_points = 2;
    plist3.points = boundary3;
    /*WARNING: parameters changed */ ppolyline3(&plist3);
    plist3.num_points = 2;
    plist3.points = boundary4;
    /*WARNING: parameters changed */ ppolyline3(&plist3);
    plist3.num_points = 2;
    plist3.points = boundary5;
    /*WARNING: parameters changed */ ppolyline3(&plist3);
    plist3.num_points = 2;
    plist3.points = boundary6;
    /*WARNING: parameters changed */ ppolyline3(&plist3);

    plabel(LEDGEFLAG);
    if( edges_sw )
    	pset_edge_flag(PEDGE_ON);
    else
    	pset_edge_flag(PEDGE_OFF);
    plabel(LINTSTYLE);
    if( solid_sw || shading_sw )
    	pset_int_style(PSTYLE_SOLID);
    else
    	pset_int_style(PSTYLE_HOLLOW);        /* 1 - int. style */
    pexec_struct(LOCATIONS);

    protate_y(3.14/2, &err, mat2);
    protate_x(3.14/2, &err, mat);
    pcompose_matrix3(mat, mat2, &err, newmat);
    pset_local_tran3(newmat,PTYPE_REPLACE);
    pclose_struct();
    build_sphere();
}

#define NEW_RI( _v, _dt, _r ) \
    { \
    (_r) += (_v) * (_dt); \
    if ( (_r) >= (WC_MAX-1) ) { \
         (_r) = 2.0 * (WC_MAX-1) - (_r); \
         (_v) = -(_v); \
    } else if ( (_r) <= (WC_MIN+1) ) { \
         (_r) = 2.0 * (WC_MIN+1) - (_r); \
         (_v) = -(_v); \
    } \
    }

static void
move_spheres()
{
    register Sphere_data  *data;
    register int          i;    
    Pint        err;
    Pmatrix3    mat, mat2, mat3, newmat;
    static Pfloat thetax = 0.0, thetay = 0.0, thetaz = 0.0;

    popen_struct(LOCATIONS);
    pset_elem_ptr(1);
    for (i = 0; i < sphere_count; i++) {
    	data = &sphere_data[i];
    	NEW_RI(data->velocity.delta_x, TIME_INC, data->position[0][3])
    	NEW_RI(data->velocity.delta_y, TIME_INC, data->position[1][3])
    	NEW_RI(data->velocity.delta_z, TIME_INC, data->position[2][3])
	switch(i) {
	    case 0:
            	protate_x(thetax, &err, mat);
                pcompose_matrix3(data->position, mat, &err, newmat);
		thetax +=.4;
		break;
	    case 1:
            	protate_y(thetay, &err, mat);
                pcompose_matrix3(data->position, mat, &err, newmat);
		thetay +=.4;
		break;
	    case 2:
            	protate_x(thetax, &err, mat);
            	protate_y(thetay, &err, mat2);
                pcompose_matrix3(mat, mat2, &err, mat3);
                pcompose_matrix3(data->position, mat3, &err, newmat);
		break;
	    case 3:
            	protate_x(thetax, &err, mat);
            	protate_y(thetay, &err, mat2);
                pcompose_matrix3(mat2, mat, &err, mat3);
                pcompose_matrix3(data->position, mat3, &err, newmat);
		break;
	}
    	pset_local_tran3(newmat, PTYPE_REPLACE);
    	/* pset_local_tran3(data->position, PTYPE_REPLACE); */
    	poffset_elem_ptr(2);
    }
    pclose_struct();
}

static void
init_phigs()
{
	register long i;
/*    int num_spheres = 5; */
#ifdef USE_X_DRAWABLE
	Pconnid_x_drawable conn;
#endif /*USE_X_DRAWABLE*/

    Pedge_flag  edge_flag = PEDGE_OFF;
    Pint_style  int_style = PSTYLE_SOLID; 
    Pint        hidden_surf = PHIGS_HLHSR_MODE_ZBUFF;

    popen_phigs( (char *)NULL, PDEF_MEM_SIZE);

#ifdef USE_X_DRAWABLE
	conn.display = appl_display;
	conn.drawable_id = appl_window;

	popen_ws(WS_ID, (Pconnid)(&conn),phigs_ws_type_x_drawable);
#else /* !USE_X_DRAWABLE */
	popen_ws(WS_ID, (Pconnid)NULL, phigs_ws_type_x_tool);
#endif /*USE_X_DRAWABLE*/

    init_view_mapping();
    build_css( edge_flag, int_style);
    pset_disp_upd_st(1, PDEFER_WAIT, PMODE_NIVE);
    pset_hlhsr_mode(WS_ID, hidden_surf);
    set_view_rep();

    for (i = 0; i < num_spheres; i++) 
    	add_sphere();

    ppost_struct(WS_ID, ROOT, 1.0);
    pset_disp_upd_st(1, PDEFER_ASAP, PMODE_NIVE);
    pset_edit_mode(PEDIT_REPLACE);
}

static void
init()
{
#ifdef USE_X_DRAWABLE	
	init_window();
#endif /*USE_X_DRAWABLE*/
	init_phigs();
}
static void
draw_image()
{
    clock_reset();
    for (;;) {
        pset_disp_upd_st(WS_ID, PDEFER_WAIT, PMODE_NIVE);
        move_spheres();
        pset_disp_upd_st(WS_ID, PDEFER_ASAP, PMODE_NIVE);
    }
}

static void
handle_args(argc, argv)
int argc;
char *argv[];
{
        while (*++argv)
        {
                if ( !strcmp(*argv, "-colors") ) {
                        colors_sw = TRUE;
                }
                else if ( !strcmp(*argv, "-solid") ) {
                        solid_sw = TRUE;
                }
                else if ( !strcmp(*argv, "-shading") ) {
                        shading_sw = TRUE;
                }
                else if ( !strcmp(*argv, "-edges") ) {
                        edges_sw = TRUE;
                }
                else if ( !strcmp(*argv, "-num") ) {
		    if(*++argv)
			num_spheres = atoi(*argv);
		}
	}

	if (!num_spheres) {
		(void) printf("%s: can't display 0 beach_balls\n",
		argv[0], num_spheres);
		exit(1);
	}
}

main(argc,argv)
int argc;
char *argv[];
{
	handle_args(argc, argv);
	init();
	draw_image();
}

