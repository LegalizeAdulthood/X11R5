#ifdef SCCS
static char sccsid[]="@(#)pat.c	1.7 Stardent 91/08/14";
#endif
/*
 *			Copyright (c) 1991 by
 *			Stardent Computer Inc.
 *			All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Stardent Computer not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. Stardent Computer
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */
/*
	This file is under sccs control at Stardent in:
	@uranus/sccs1.p/cmd/grafix/X11/extensions/lib/PEX/pexim/s.pat.c
*/
/*
 *  Annotext.c - have some fun. - now for PEXIM
 */
#include <X11/Xlib.h>
#include <phigs/pexim.h>

#define RADICAL3 1.732
#define RAD3INV (1.0/RADICAL3)
#define RADIAN(x) ((x)*(3.1415927 / 180.0))

static int AmbientLightIndex = 1;
static int DirectionalLightIndex = 2;

Pview_map3 view_map = {
    { -2.0, 2.0, -2.0, 2.0},
    { 0.0, 1.0, 0.0, 1.0, 0.0, 1.0},
    PTYPE_PERSPECT,
    { 0., 0., 1000.0},
    0.0,
    -5.0,
    5.0
};

/*************************************************************************
 * InitView
 */
int
InitView(long wkid, long view)
{
  Ppoint3 vrp;
  Pvec3 vpn;
  Pvec3 vup;
  
  Pview_rep3 vrep;                           /*  view structure */
  
  Pint err;

  vrp.x = 0.0; vrp.y = 0.0; vrp.z = 0.0;   	  /*  origin */
  /* view plane normal */
  vpn.delta_x = 1.0; vpn.delta_y = 1.0; vpn.delta_z = 1.0;
  /*  view up vector */
  vup.delta_x = 0.0; vup.delta_y = 1.0; vup.delta_z = 0.0;
  
  peval_view_ori_matrix3( &vrp, &vpn, &vup, 
			 &err, vrep.ori_matrix);
  
  if (err != 0) { (void) printf( "view orientation error "); return (0); }
  
  peval_view_map_matrix3( &view_map, &err, vrep.map_matrix);
  
  if (err != 0) { (void) printf( "view mapping error "); return (0); }
  
  vrep.clip_limit =  view_map.vp;
  vrep.xy_clip = PIND_CLIP;
  vrep.back_clip = PIND_CLIP;
  vrep.front_clip = PIND_CLIP;
  pset_view_rep3( wkid, view, &vrep);

  return (1);
}

/*************************************************************************
 * BuildStrux
 */
RenderStrux( long wkid, long view, Pmatrix3 bldmat )
{
  Ppoint3 p[7], *pp;
  Ppoint3 origin;
  Pvec3   dir[2];
  Ppoint_list3 plist;

/* These are from the "underground buttons" at the past couple of SIGGRAPHS */

  char   *safePEX = "safe PEX";           /* SIGGRAPH 89 - Boston */
  char   *hotPEX = "hot steamy PEX";      /* SIGGRAPH 90 - Dallas */
  char   *winPEX = "silicon PEX or bust"; /* SIGGRAPH 91 - Las Vegas */
  char   *starPEX = "A new dimensions for X"; /* PEX-IC slogan */

  Ptext_align align;
  Pvec up_vect;

  Ppoint origin2;

  Pvec3 offset;

  Ppoint offset2;

  Pfacet_vdata_arr3 vData;
  Pptco3 verts[8], *ptco;

  Pint_list ActivateList;
  Pint_list DeactivateList;
  Prefl_props AreaProperties;

  pbegin_rendering(wkid);
/*
 * where's posting to views?
 */
  pset_view_ind(view);
  pset_local_tran3(bldmat, PTYPE_REPLACE);


#ifdef NEEDLIGHTS
  ActivateList.num_ints = 2;
  ActivateList.ints = (Pint *) malloc(ActivateList.num_ints * sizeof(Pint));
  ActivateList.ints[0] = 1;
  ActivateList.ints[1] = 2;
  DeactivateList.num_ints = 0;
  DeactivateList.ints = NULL;
  pset_light_src_state(&ActivateList, &DeactivateList);
  free(ActivateList.ints);

  /* set the area properties */
  AreaProperties.ambient_coef = 0.3;	/* ambient coefficient */
  AreaProperties.diffuse_coef = 0.3;	/* diffuse coefficient */
  AreaProperties.specular_coef = 0.7;	/* specular coefficient */
  /* set specular color to white */
  AreaProperties.specular_colr.type = PMODEL_RGB;	/* specular color model */
  AreaProperties.specular_colr.val.general.x = 1.0;
  AreaProperties.specular_colr.val.general.y = 1.0;
  AreaProperties.specular_colr.val.general.z = 1.0;
  AreaProperties.specular_exp = 50.0;	/* specular exponent */
  AreaProperties.transpar_coef = 0.0;	/* transparency coefficient */

  pset_refl_props(&AreaProperties);
#endif
/*
 * just so we see something...
 */
  pset_line_colr_ind(2);
  plist.points = p;
  pp = (Ppoint3 *)p;
  pp->x = -0.05; pp->y = -0.05; pp->z = -0.05; pp++;
  pp->x = 2.05; pp->y = -0.05; pp->z = -0.05; pp++;
  pp->x = 1.0; pp->y = RADICAL3+0.5; pp->z = -0.05; pp++;
  pp->x = -0.05; pp->y = -0.05; pp->z = -0.05; pp++;
  plist.num_points = 4;
  ppolyline3( &plist );

/*
 * make a tetrahedron (0,0,0), (2,0,0), (1,V3,0), (1,.5,V3)
 *                    1-white  2-magenta 3-red    4-green
 *   1 - 3 - 1
 *    \ / \ / \
 *     2 - 4 - 2
 */

  pset_int_style(PSTYLE_SOLID);
  pset_refl_eqn(PREFL_AMB_DIFF_SPEC);
  pset_int_shad_meth(PSD_COLOUR);

  ptco = verts;

  ptco->point.x = 0; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 2; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 1; ptco->point.y = RADICAL3; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0; 
  ptco++;

  ptco->point.x = 1; ptco->point.y = 0.5; ptco->point.z = RADICAL3;
  ptco->colr.direct.rgb.red = 0;
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 0;  
  ptco++;

  ptco->point.x = 0; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 2; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  vData.ptcolrs = verts;
  ptri_strip3_data(PFACET_NONE, PVERT_COORD_COLOUR, PMODEL_RGB, 6, 
		  (Pfacet_data_arr3 *)0, &vData);

  pset_text_colr_ind(3);

  pset_char_ht((double)0.2);
  pset_char_expan((double)0.75);
  pset_char_space((double)-0.1);

  origin.x = 0.1; origin.y = -0.03; origin.z = 0.0;
  dir[0].delta_x = 1.0; dir[0].delta_y = 0.0; dir[0].delta_z = 0.0;
  dir[1].delta_x = 0.0; dir[1].delta_y = 0.5; dir[1].delta_z = RADICAL3;
  ptext3( &origin, dir, safePEX );

  origin.x = 2.1; origin.y = 0.0; origin.z = 0.0;
  dir[0].delta_x = -1.0; dir[0].delta_y = RADICAL3; dir[0].delta_z = 0.0;
  dir[1].delta_x = -0.5; dir[1].delta_y = (1-RADICAL3)/2; dir[1].delta_z = RADICAL3;
  ptext3( &origin, dir, hotPEX );

  origin.x = 1.0; origin.y = RADICAL3+0.1; origin.z = 0.0;
  dir[0].delta_x = -1.0; dir[0].delta_y = -RADICAL3; dir[0].delta_z = 0.0;
  dir[1].delta_x = 0.5; dir[1].delta_y = (RADICAL3-1)/2; dir[1].delta_z = RADICAL3;
  ptext3( &origin, dir, winPEX );

  origin.x = 0.0; origin.y = 0.0; origin.z = -0.1;
  dir[0].delta_x = RADICAL3; dir[0].delta_y = 0.5; dir[0].delta_z = 0.0;
  dir[1].delta_x = -0.5; dir[1].delta_y = RADICAL3; dir[1].delta_z = 0.0;
  ptext3( &origin, dir, starPEX );


/* Annotation text!
 ************************************************************************
 * make something like page 51
 */  

  pset_text_colr_ind(1);

  pset_anno_char_ht((double)0.02);
  origin.x = 0.0; origin.y = 0.0; origin.z = 0.0;
  offset.delta_x = 0.0;  offset.delta_y = 0.0;  offset.delta_z = 0.0;

  panno_text_rel3( &origin, &offset, safePEX );

  origin.x = 2.0; origin.y = 0.0; origin.z = 0.0;
  offset.delta_x = 0.1;  offset.delta_y = 0.0;  offset.delta_z = 0.0;
  panno_text_rel3( &origin, &offset, hotPEX );

  pset_anno_style(PANNO_STYLE_LEAD_LINE);
  origin.x = 1.0; origin.y = RADICAL3; origin.z = 0.0;
  offset.delta_x = 0.0;  offset.delta_y = 0.2;  offset.delta_z = 0.0;
  panno_text_rel3( &origin, &offset, winPEX );

  origin.x = 1.0; origin.y = 0.5; origin.z = RADICAL3;
  offset.delta_x = 0.0;  offset.delta_y = 0.0;  offset.delta_z = 0.2;
  panno_text_rel3( &origin, &offset, starPEX );

  AddPolyLineSet();

/*
 * try zero area triangle, 
 */

  ptco = verts;

  ptco->point.x = -0.3; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = -0.3; ptco->point.y = 0.5; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 0; ptco->point.y = 1; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0; 
  ptco++;

  ptco->point.x = 0; ptco->point.y = 1; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0;  
  ptco++;

  ptco->point.x = 0.2; ptco->point.y = 1.4; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 0.2; ptco->point.y = 1.4; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = -0.3; ptco->point.y = 1.2; ptco->point.z = 0.2;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = -0.3; ptco->point.y = 1.0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 0;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  vData.ptcolrs = verts;
  ptri_strip3_data(PFACET_NONE, PVERT_COORD_COLOUR, PMODEL_RGB, 8,
		  (Pfacet_data_arr3 *)0, &vData);

  pend_rendering(wkid,1);

}

/*
 * If this is our tetrahedron and Bob wants a line from each we need to
 * figure the center....
 * make a tetrahedron (0,0,0), (2,0,0), (1,V3,0), (1,RAD3INV,V3)
 *                    1-white  2-magenta 3-red    4-green
 *   1 - 3 - 1
 *    \ / \ / \
 *     2 - 4 - 2
 *
 * center = (1, RAD3INV, RAD3INV)
 *
 */

AddPolyLineSet()
{
  Pptco3 verts[12], *ptco;
  Pline_vdata_list3 lists[6], *plslist;

  ptco = verts;
  plslist = lists;

  plslist->num_vertices = 2;
  plslist->vertex_data.ptcolrs = ptco;

  ptco->point.x = 0; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 1;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = -1.0; ptco->point.y = -RAD3INV; ptco->point.z = -RAD3INV;
  ptco->colr.direct.rgb.red = 0;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0;  
  ptco++;

  plslist++;
  plslist->num_vertices = 2;
  plslist->vertex_data.ptcolrs = ptco;

  ptco->point.x = 2; ptco->point.y = 0; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 1;  
  ptco++;

  ptco->point.x = 3; ptco->point.y = -RAD3INV; ptco->point.z = -RAD3INV;
  ptco->colr.direct.rgb.red = 0;
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0;  
  ptco++;

  plslist++;
  plslist->num_vertices = 2;
  plslist->vertex_data.ptcolrs = ptco;

  ptco->point.x = 1; ptco->point.y = RADICAL3; ptco->point.z = 0;
  ptco->colr.direct.rgb.red = 1;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0; 
  ptco++;

  /* this is not right, but... */
  ptco->point.x = 1; ptco->point.y = RADICAL3+1.0; ptco->point.z = -RAD3INV;
  ptco->colr.direct.rgb.red = 0;  
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0; 
  ptco++;

  plslist++;
  plslist->num_vertices = 2;
  plslist->vertex_data.ptcolrs = ptco;

  ptco->point.x = 1; ptco->point.y = RAD3INV; ptco->point.z = RADICAL3;
  ptco->colr.direct.rgb.red = 0.7;
  ptco->colr.direct.rgb.green = 0.5;
  ptco->colr.direct.rgb.blue = 0.8;  
  ptco++;

  ptco->point.x = 1; ptco->point.y = RAD3INV; ptco->point.z = RADICAL3+1.0;
  ptco->colr.direct.rgb.red = 0;
  ptco->colr.direct.rgb.green = 0;
  ptco->colr.direct.rgb.blue = 0;  
  ptco++;

  ppolyline_set3_data(PVERT_COORD_COLOUR, PMODEL_RGB, 4, lists);
}

/*
 * from MainPobj.c
 */

RotateObject(long wkid)
{
  Ppoint3 pt;
  Pvec3 shift;
  Pvec3 scale;
  Pfloat  x_ang;
  Pfloat  y_ang;
  Pfloat  z_ang;
  Pint    degree,
          err;
  Pmatrix3 bldmat;

  pt.x = 0.0;
  pt.y = 0.0;
  pt.z = 0.0;
  shift.delta_x = 0;
  shift.delta_y = 0;
  shift.delta_z = 0;
  scale.delta_x = 1.0;
  scale.delta_y = 1.0;
  scale.delta_z = 1.0;

    x_ang = 0;
    y_ang = 0;
    z_ang = 0;
    pbuild_tran_matrix3(&pt, &shift, x_ang, y_ang, z_ang, &scale, &err, bldmat);
    RenderStrux( wkid, 1, bldmat );

  for (degree = 0; degree < 360; degree += 4) {
    x_ang = RADIAN(degree);
    y_ang = 0;
    z_ang = 0;
    pbuild_tran_matrix3(&pt, &shift, x_ang, y_ang, z_ang, &scale, &err, bldmat);
    RenderStrux( wkid, 1, bldmat );


  }
  for (degree = 0; degree < 360; degree += 4) {
    x_ang = 0;
    y_ang = RADIAN(degree);
    z_ang = 0;
    pbuild_tran_matrix3(&pt, &shift, x_ang, y_ang, z_ang, &scale, &err, bldmat);
    RenderStrux( wkid, 1, bldmat );
  }
  for (degree = 0; degree < 360; degree += 4) {
    x_ang = 0;
    y_ang = 0;
    z_ang = RADIAN(degree);
    pbuild_tran_matrix3(&pt, &shift, x_ang, y_ang, z_ang, &scale, &err, bldmat);
    RenderStrux( wkid, 1, bldmat );

  }

}

/*************************************************************************
 * from MainPobj.c
 */
int 
NormalizeVector(v)
Pvec3 *v;
{
  register float len;

#define SQR(x) ((x)*(x))

  len = sqrt(SQR(v->delta_x) + SQR(v->delta_y) + SQR(v->delta_z));
  if (len == 0.0)
    return (0);
  len = 1.0 / len;
  v->delta_x *= len;
  v->delta_y *= len;
  v->delta_z *= len;
  return (1);
}

/*************************************************************************
 * InitLights - do I really want to do this?
 */
InitLights(long wkid)
{
  Plight_src_bundle Light;
 
  /* Set the ambient light */
  Light.type = PLIGHT_AMBIENT;
  Light.rec.ambient.colr.type = PMODEL_RGB;
  Light.rec.ambient.colr.val.general.x = 1.0;	/* white */
  Light.rec.ambient.colr.val.general.y = 1.0;
  Light.rec.ambient.colr.val.general.z = 1.0;
  pset_light_src_rep(wkid, AmbientLightIndex, &Light);

  /* Set the directional light */
  Light.type = PLIGHT_DIRECTIONAL;
  Light.rec.directional.colr.type = PMODEL_RGB;
  Light.rec.directional.colr.val.general.x = 1.0;	/* white */
  Light.rec.directional.colr.val.general.y = 1.0;
  Light.rec.directional.colr.val.general.z = 1.0;
  /*
   * Note: the light source direction vector points from the light to the
   * object.
   */
  Light.rec.directional.dir.delta_x =  1.0;   /* in front, to the left,above */
  Light.rec.directional.dir.delta_y = -1.0;
  Light.rec.directional.dir.delta_z = -1.0;
  NormalizeVector(&Light.rec.directional.dir);
  pset_light_src_rep(wkid, DirectionalLightIndex, &Light);
}


/*************************************************************************
 * main
 */

main(int argc, char *argv[])
{
  int i;
  Display *display;
  Window w;
  char *displayString = (char *)0;
  int x = 100, y = 100, width = 500, height = 400;
  int err, error;
  int forever = 0;

  long wkid = 1;
  long struxid = 1;
  for ( i = 1; i<argc; i++ ) {
    if ((strncmp(argv[i],"-geometry",strlen(argv[i]))) == 0) {
      printf("found -geom\n");
    } else if ((strncmp(argv[i],"-display",strlen(argv[i]))) == 0) {
      if (++i > argc) { printf("not enough args"); exit(1); }
      displayString = argv[i];
    } else if ((strncmp(argv[i],"-forever",strlen(argv[i]))) == 0) {
	forever = 1;
    }
  }

  display = XOpenDisplay(displayString);
  if (!display) { printf(" cannot open display %s\n",displayString); exit(1);}

  w = XCreateSimpleWindow( display, DefaultRootWindow(display), x, y,
                           width, height, 5, 
                           BlackPixel(display,DefaultScreen(display)),
                           WhitePixel(display,DefaultScreen(display)));

  XMapWindow(display,w);
  XFlush(display);

  error = pinit();

  error = popen_renderer(wkid, display, w);
  if (error != POK) {printf("popen_renderer, error = %d\n",error); exit(1);}
  XSync(display, 1);

  pset_hlhsr_mode(wkid, PHIGS_HLHSR_MODE_ZBUFF); /* not default anymore */

  InitView(wkid,1);
#ifdef NEEDLIGHTS
  InitLights(wkid);
#endif

  RotateObject(wkid);
  XSync(display, 1);
  while (forever) {
    RotateObject(wkid);
    XSync(display, 1);
  }
  getchar();

}
