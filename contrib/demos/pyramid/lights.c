/* $XConsortium: lights.c,v 5.1 91/02/16 09:32:52 rws Exp $ */

/***********************************************************
Copyright (c) 1989-1991 by Sun Microsystems, Inc. and the X Consortium.

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
 *	Pyramid - PHIGS PLUS demo program
 *
 *	Greg Schechter  ---  August, 1989
 *
 */

#include "pyramid.h"

#define MAXLIGHTS	    6
#define LNUM_OFFSET	    1

Pint_list             	activate, deactivate;
int                 	activate_light[6];
Plight_src_bundle	lights[MAXLIGHTS];

Pmatrix3	    identity = {
    1.0,0.0,0.0,0.0,
    0.0,1.0,0.0,0.0,
    0.0,0.0,1.0,0.0,
    0.0,0.0,0.0,1.0
};

Pint	 defaultLightType[MAXLIGHTS] = { 
    PLIGHT_POSITIONAL,
    PLIGHT_POSITIONAL,
    PLIGHT_POSITIONAL,
    PLIGHT_POSITIONAL,
    PLIGHT_POSITIONAL,
    PLIGHT_POSITIONAL,
};

int     light_onoff[MAXLIGHTS] = { 1, 1, 1, 0, 0, 0 };

Pgcolr	*defaultLightColor[MAXLIGHTS] = { 
    &red, &green, &blue, &cyan, &magenta, &yellow
};

Ppoint3	 defaultLightPosition[MAXLIGHTS] = {
    0.0, 0.5, 0.5,
    1.0, 0.5, 0.5,
    0.5, 0.0, 0.5,
    0.5, 1.0, 0.5,
    0.5, 0.5, 0.0,
    0.5, 0.5, 1.0
};

Pvec3 defaultLightDirection[MAXLIGHTS] = {
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0
};

Pfloat	 defaultAttenCoef1[MAXLIGHTS] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
Pfloat	 defaultAttenCoef2[MAXLIGHTS] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
Pfloat	 defaultSpecExp[MAXLIGHTS] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
Pfloat	 defaultSpreadAngle[MAXLIGHTS] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };


void 
set_prop_proc()
{
    Prefl_props areaprops;
    Pgcolr color;

    color.type = PMODEL_RGB;

    areaprops.ambient_coef = 
	(Pfloat) (int) panel_get_value(ambcoef_item) / 100.0;
    areaprops.diffuse_coef = 
	(Pfloat) (int) panel_get_value(diffcoef_item) / 100.0;
    areaprops.specular_coef = 
	(Pfloat) (int) panel_get_value(speccoef_item) / 100.0;
    areaprops.specular_exp = 
	(Pfloat) (int) panel_get_value(specexp_item) / 100.0;
    areaprops.transpar_coef = 
	(Pfloat) (int) panel_get_value(transpar_item) / 100.0;
    color.val.general.x = 
	(Pfloat) (int) panel_get_value(speccolr_item) / 100.0;
    color.val.general.y = 
	(Pfloat) (int) panel_get_value(speccolg_item) / 100.0;
    color.val.general.z = 
	(Pfloat) (int) panel_get_value(speccolb_item) / 100.0;

    areaprops.specular_colr = color;

    color.val.general.x = (Pfloat)(int) panel_get_value(diffcolr_item) / 100.0;
    color.val.general.y = (Pfloat)(int) panel_get_value(diffcolg_item) / 100.0;
    color.val.general.z = (Pfloat)(int) panel_get_value(diffcolb_item) / 100.0;

    pset_edit_mode(PEDIT_REPLACE);
    popen_struct(MAIN_PYRAMID);
	pset_elem_ptr(0);
	pset_elem_ptr_label(ADD_PROPS_HERE);
	poffset_elem_ptr(1);
	pset_refl_props(&areaprops);
	
	pset_elem_ptr(0);
	pset_elem_ptr_label(ADD_INT_COLOR_HERE);
	poffset_elem_ptr(1);
	pset_int_colr(&color);
    pclose_struct();
    pset_edit_mode(PEDIT_INSERT);
    
    pupd_ws(WSID, PUPD_PERFORM);
}

static Pint	lightStructs[MAXLIGHTS] = { 400,401,402,403,404,405 };

static void
setAmbientLight()
{
    Pcolr_rep		ambientColor;
    Pint		err;
    Plight_src_bundle	rep;
    int			i;

    ambientColor.rgb.red = 
    ambientColor.rgb.green = 
    ambientColor.rgb.blue = 0.0;
    
    for (i = 0; i < MAXLIGHTS; i++) {

	if (!light_onoff[i])
	    continue;

	pinq_light_src_rep (WSID, i + LNUM_OFFSET, PINQ_REALIZED, &err, &rep);
	switch (rep.type) {
	    case PLIGHT_AMBIENT	    :
		ambientColor.rgb.red += rep.rec.ambient.colr.val.general.x;
		ambientColor.rgb.green += rep.rec.ambient.colr.val.general.y;
		ambientColor.rgb.blue += rep.rec.ambient.colr.val.general.z;
		if (ambientColor.rgb.red > 1.0) ambientColor.rgb.red = 1.0;
		if (ambientColor.rgb.green > 1.0) ambientColor.rgb.green = 1.0;
		if (ambientColor.rgb.blue > 1.0) ambientColor.rgb.blue = 1.0;
		break;	
	}
    }

    /** Set the background color of the workstation to the sum of all of the
     ** ambient light sources **/
    pset_colr_rep(WSID2, 0, &ambientColor);

}

static void
makeLightStruct(lnum)
int lnum;
{
    Pint		s = lightStructs[lnum];
    static Pint		ns_int[1];
    static Pint_list	ns = { 1, ns_int };
    Pint		err;
    Plight_src_bundle	rep;
    static Ppoint3	transformPt = { 0.0,0.0,0.0 };
    Pvec3		shift;
    static Pvec3	posScale = { 0.08, 0.08, 0.08 };
    static Pvec3	dirScale = { 1.0, 1.0, 1.0 };
    Pmatrix3		matrix; 
    Ppoint3		linePts[2];
    static Ppoint_list3	colored_line;
    double		twiceLen, x, y, z, xrad, yrad, zrad;
    double		acos(), sqrt();
    
    pempty_struct(s);
    popen_struct(s);
    
	ns_int[0] = s;
	padd_names_set(&ns);

	pinq_light_src_rep(WSID, lnum + LNUM_OFFSET, PINQ_REALIZED, &err, &rep);
	switch (rep.type) {
	    case PLIGHT_AMBIENT	    :
		setAmbientLight();
		break;
	    case PLIGHT_DIRECTIONAL :
		x = (double)rep.rec.directional.dir.delta_x;
		y = (double)rep.rec.directional.dir.delta_y;
		z = (double)rep.rec.directional.dir.delta_z;
		twiceLen = 2 * sqrt(x*x + y*y + z*z);
		x /= twiceLen;
		y /= twiceLen;
		z /= twiceLen;

		pset_line_colr(&rep.rec.directional.colr);
		pset_linewidth(4.0);
		linePts[0].x = linePts[0].y = linePts[0].z = 0.0;
		linePts[1].x = x;
		linePts[1].y = y;
		linePts[1].z = z;
		colored_line.num_points = 2;
		colored_line.points = linePts;
		ppolyline3( &colored_line );
		
		pset_linewidth(1.0);
		linePts[0].x = linePts[1].x = x;
		linePts[0].y = linePts[1].y = y;
		linePts[0].z = 0.0; linePts[1].z = z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );

		linePts[0].x = linePts[1].x = x;
		linePts[0].y = 0.0; linePts[1].y = y;
		linePts[0].z = linePts[1].z = z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );

		linePts[0].x = 0.0; linePts[1].x = x;
		linePts[0].y = linePts[1].y = y;
		linePts[0].z = linePts[1].z = z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );

		break;

	    case PLIGHT_POSITIONAL  :
		pset_int_colr(&rep.rec.positional.colr);
		shift.delta_x = rep.rec.positional.pos.x;
		shift.delta_y = rep.rec.positional.pos.y;
		shift.delta_z = rep.rec.positional.pos.z;
		pbuild_tran_matrix3(&transformPt, &shift, 0.0, 0.0, 0.0, &posScale, 
		    &err, matrix);
		pset_local_tran3(matrix, PTYPE_REPLACE);
		pexec_struct(POS_LIGHT_STRUCT);
		pset_line_colr(&rep.rec.positional.colr);

		pset_local_tran3(identity, PTYPE_REPLACE);
		linePts[0].x = linePts[1].x = rep.rec.positional.pos.x;
		linePts[0].y = linePts[1].y = rep.rec.positional.pos.y;
		linePts[0].z = 0.0; linePts[1].z = rep.rec.positional.pos.z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );

		linePts[0].x = linePts[1].x = rep.rec.positional.pos.x;
		linePts[0].y = 0.0; linePts[1].y = rep.rec.positional.pos.y;
		linePts[0].z = linePts[1].z = rep.rec.positional.pos.z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );
		linePts[0].x = 0.0; linePts[1].x = rep.rec.positional.pos.x;
		linePts[0].y = linePts[1].y = rep.rec.positional.pos.y;
		linePts[0].z = linePts[1].z = rep.rec.positional.pos.z;
                colored_line.num_points = 2;
                colored_line.points = linePts;
                ppolyline3( &colored_line );

		break;
	    case PLIGHT_SPOT	    :
		printf("Spot light - index %d\n", lnum);
		break;	
	}
	
	premove_names_set(&ns);
	
    pclose_struct();	
}



showLights()
{
    int i, j;
    static  Pint    inclSet_ints[6];
    static  Pint_list inclSet = { 0, inclSet_ints };
    static  Pint_list exclSet = { 0, NULL };

    pset_edit_mode(PEDIT_INSERT);    
    punpost_struct(WSID2, LIGHT_STRUCT);
    pempty_struct(LIGHT_STRUCT);
    popen_struct(LIGHT_STRUCT);
	pset_hlhsr_id(PHIGS_HLHSR_ID_ZBUFF);
	pset_view_ind(VIEW_1);
	pset_int_style(PSTYLE_SOLID);
	for (i = 0; i < MAXLIGHTS; i++)
	    pexec_struct(lightStructs[i]);
    pclose_struct();
    
    for (i = 0; i < MAXLIGHTS; i++)
	makeLightStruct(i);

    setAmbientLight();
    
    for (i = 0, j = 0; i < MAXLIGHTS; i++) {
	if (!light_onoff[i])
	    inclSet_ints[j++] = lightStructs[i];
    }
    inclSet.num_ints = j;
    
    pset_invis_filter(WSID2, &inclSet, &exclSet);

    ppost_struct(WSID2, LIGHT_STRUCT, 0.0);
    pupd_ws(WSID2, PUPD_PERFORM);

}

set_light(lightrep,color,pos,dir,ac1,ac2,spexp,spang)
Plight_src_bundle *lightrep;
Pgcolr          color;
Ppoint3         pos;
Pvec3        dir;
Pfloat          ac1, ac2, spexp, spang;
{
        switch(lightrep->type) {
        case PLIGHT_AMBIENT:
            lightrep->rec.ambient.colr = color;
            break;
        case PLIGHT_DIRECTIONAL:
            lightrep->rec.directional.colr = color;
            lightrep->rec.directional.dir = dir;
            break;
        case PLIGHT_POSITIONAL:
            lightrep->rec.positional.colr = color;
            lightrep->rec.positional.pos = pos;
            lightrep->rec.positional.coef[0] = ac1;
            lightrep->rec.positional.coef[1] = ac2;
            break;
        case PLIGHT_SPOT:
            lightrep->rec.spot.colr = color;
            lightrep->rec.spot.pos = pos;
            lightrep->rec.spot.dir = dir;
            lightrep->rec.spot.exp = spexp;
            lightrep->rec.spot.coef[0] = ac1;
            lightrep->rec.spot.coef[1] = ac2;
            lightrep->rec.spot.angle = spang;
            break;
        default:
            break;
        }
}

initialize_light_params(lnum,color,pos,dir,ac1,ac2,spexp,spang)
int		 lnum;
Pgcolr          *color;
Ppoint3         *pos;
Pvec3        *dir;
Pfloat          *ac1, *ac2, *spexp, *spang;
{
        *color = *defaultLightColor[lnum];
        *pos   = defaultLightPosition[lnum];
        *dir   = defaultLightDirection[lnum];
        *ac1   = defaultAttenCoef1[lnum];
        *ac2   = defaultAttenCoef2[lnum];
        *spexp = defaultSpecExp[lnum];
        *spang = defaultSpreadAngle[lnum];
}


void
light_num_proc(item,value,event)
Panel_item item; unsigned int value; Event *event;
{
        load_light_items((int) value);
}

initialize_lights()
{
        int i;
        Pgcolr color;
        Ppoint3 pos;
        Pvec3 dir;
        Pfloat ac1, ac2, spexp, spang;
	static firstTime = 1;


	if (firstTime) {
	    firstTime = 0;
	    for (i = 0; i < MAXLIGHTS; i++) {
		initialize_light_params(i, &color, &pos, &dir, &ac1, &ac2,
		    &spexp, &spang);
		lights[i].type = defaultLightType[i];
		set_light(&lights[i], color, pos, dir, ac1, ac2, spexp, spang);
		load_light_items(i);
		pset_light_src_rep(WSID, i + LNUM_OFFSET, &lights[i]);
	    }
	    load_light_items(0);
	}
        activate_lights();
}

load_light_items(lnum)
int lnum;
{
    Pgcolr color;
    Pvec3 dir;
    Ppoint3 pos;
    Pfloat ac1;
    Pfloat ac2;
    Pfloat spexp;
    Pfloat spang;
    char itemstr[80];

    switch(lights[lnum].type) {
    case PLIGHT_AMBIENT:
	color = lights[lnum].rec.ambient.colr;
	panel_set_value(ltype_item,0);
	break;
    case PLIGHT_DIRECTIONAL:
	color = lights[lnum].rec.directional.colr;
	dir = lights[lnum].rec.directional.dir;
	panel_set_value(ltype_item,1);
	break;
    case PLIGHT_POSITIONAL:
	color = lights[lnum].rec.positional.colr;
	pos = lights[lnum].rec.positional.pos;
	ac1 = lights[lnum].rec.positional.coef[0];
	ac2 = lights[lnum].rec.positional.coef[1];
	panel_set_value(ltype_item,2);
	break;
    case PLIGHT_SPOT:
	color = lights[lnum].rec.spot.colr;
	dir = lights[lnum].rec.spot.dir;
	pos = lights[lnum].rec.spot.pos;
	ac1 = lights[lnum].rec.spot.coef[0];
	ac2 = lights[lnum].rec.spot.coef[1];
	spexp = lights[lnum].rec.spot.exp;
	spang = lights[lnum].rec.spot.angle;
	panel_set_value(ltype_item,3);
	break;
    default:
	break;
    }

    panel_set_value(lonoff_item, light_onoff[lnum]);
    /* show only the proper items for the light type */
    light_type_proc(0);

    panel_set_value(ldirx_item, (int) dir.delta_x);
    panel_set_value(ldiry_item, (int) dir.delta_y);
    panel_set_value(ldirz_item, (int) dir.delta_z);

    sprintf(itemstr,"%g %g %g",pos.x,pos.y,pos.z);
    panel_set_value(lpos_item, itemstr);

    panel_set_value(lcolr_item, (int) (color.val.general.x * 100.0));
    panel_set_value(lcolg_item, (int) (color.val.general.y * 100.0));
    panel_set_value(lcolb_item, (int) (color.val.general.z * 100.0));

    sprintf(itemstr,"%g %g",ac1,ac2);
    panel_set_value(latten_item, itemstr);

    sprintf(itemstr,"%g",spexp);
    panel_set_value(lexp_item, itemstr);

    sprintf(itemstr,"%g",spang);
    panel_set_value(lspread_item, itemstr);
}

void
light_onoff_proc(item,value,event)
Panel_item item; 
unsigned int value; 
Event *event;
{
    light_onoff[(int) panel_get_value(lnum_item)] = (int) value;
    set_light_proc();
}

void
make_lights_effective()
{
    pupd_ws(WSID, PUPD_PERFORM);
}

void
set_light_proc(item, val, event)
Panel_item  item;
int	    val;
Event	   *event;
{
    int lnum;
    Pgcolr color;
    Ppoint3 pos;
    Pvec3 dir;
    Pfloat ac1, ac2, spexp, spang;

    color.type = PMODEL_RGB;
    color.val.general.x = (Pfloat) (int) panel_get_value(lcolr_item) / 100.0;
    color.val.general.y = (Pfloat) (int) panel_get_value(lcolg_item) / 100.0;
    color.val.general.z = (Pfloat) (int) panel_get_value(lcolb_item) / 100.0;
    dir.delta_x = (Pfloat) (int) panel_get_value(ldirx_item);
    dir.delta_y = (Pfloat) (int) panel_get_value(ldiry_item);
    dir.delta_z = (Pfloat) (int) panel_get_value(ldirz_item);
    sscanf((char *) panel_get_value(lpos_item), "%f %f %f", 
	    &pos.x, &pos.y, &pos.z);
    sscanf((char *) panel_get_value(latten_item), "%f %f", 
	    &ac1, &ac2);
    sscanf((char *) panel_get_value(lexp_item), "%f", &spexp);
    sscanf((char *) panel_get_value(lspread_item), "%f", &spang);

    if (item == lcolr_item)
	color.val.general.x = (Pfloat)(val) / 100.0;
    else if (item == lcolg_item)
	color.val.general.y = (Pfloat)(val) / 100.0;
    else if (item == lcolb_item)
	color.val.general.z = (Pfloat)(val) / 100.0;
    else if (item == ldirx_item)
	dir.delta_x = val;
    else if (item == ldiry_item)
	dir.delta_y = val;
    else if (item == ldirz_item)
	dir.delta_z = val;

    lnum = (int) panel_get_value(lnum_item);
    switch((int) panel_get_value(ltype_item)) {
    case 0:
	lights[lnum].type = PLIGHT_AMBIENT;
	lights[lnum].rec.ambient.colr = color;
	break;
    case 1:
	lights[lnum].type = PLIGHT_DIRECTIONAL;
	lights[lnum].rec.directional.colr = color;
	lights[lnum].rec.directional.dir = dir;
	break;
    case 2:
	lights[lnum].type = PLIGHT_POSITIONAL;
	lights[lnum].rec.positional.colr = color;
	lights[lnum].rec.positional.pos = pos;
	lights[lnum].rec.positional.coef[0] = ac1;
	lights[lnum].rec.positional.coef[1] = ac2;
	break;
    case 3:
	lights[lnum].type = PLIGHT_SPOT;
	lights[lnum].rec.spot.colr = color;
	lights[lnum].rec.spot.dir = dir;
	lights[lnum].rec.spot.pos = pos;
	lights[lnum].rec.spot.coef[0] = ac1;
	lights[lnum].rec.spot.coef[1] = ac2;
	lights[lnum].rec.spot.exp = spexp;
	lights[lnum].rec.spot.angle = spang;
	break;
    default:
	break;
    }
    

    pset_light_src_rep(WSID, lnum + LNUM_OFFSET, &lights[lnum]);
    activate_lights();
    makeLightStruct(lnum);
    pupd_ws(WSID2, PUPD_PERFORM);
}


activate_lights()
{
    Pint_list activ, deactiv;
    Pint activ_list[MAXLIGHTS];
    Pint deactiv_list[MAXLIGHTS];
    int i;

    activ.num_ints = 0;
    activ.ints = activ_list;
    deactiv.num_ints = 0;
    deactiv.ints = deactiv_list;
    /* get current on and off lights */
    for (i = 0; i < MAXLIGHTS; i++) {
	if (light_onoff[i] == 0) {
	    deactiv_list[deactiv.num_ints] = i + LNUM_OFFSET;
	    deactiv.num_ints++;
	}
	else {
	    activ_list[activ.num_ints] = i + LNUM_OFFSET;
	    activ.num_ints++;
	}
    }
    pset_edit_mode(PEDIT_REPLACE);
    popen_struct(MAIN_PYRAMID);
	pset_elem_ptr(0);
	pset_elem_ptr_label(ADD_LIGHTS_HERE);
	poffset_elem_ptr(1);
	pset_light_src_state(&activ,&deactiv);
    pclose_struct();
    showLights();
}


createPosLightStructure()
{
    /** The positional light will be represented by a cube colored the color
     ** of the light.  Create the cube via a quad mesh **/
    static Pint_size	quadDim[] = { 5, 4 }; 
    Pfacet_data_arr3 facetData;
    Pfacet_vdata_arr3 vertData; 
    
    static Ppoint3	quad_points[] = {
	/** Bottom **/
	-0.5,-0.5,0.5,
	0.5,-0.5,0.5,
	0.5,-0.5,0.5,		/* results in empty facet */
	0.5,-0.5,0.5,		/* results in empty facet */
	0.5,-0.5,0.5,		/* results in empty facet */
	
	/** front side **/
	-0.5,-0.5,-0.5,
	0.5,-0.5,-0.5,

	/** continue around to right side **/
	0.5,-0.5,0.5,

	/** continue around to back side **/
	-0.5,-0.5,0.5,

	/** continue around to left side **/
	-0.5,-0.5,-0.5,

	/** do the same except for y == 0.5 **/
	-0.5,0.5,-0.5,
	0.5,0.5,-0.5,
	0.5,0.5,0.5,
	-0.5,0.5,0.5,
	-0.5,0.5,-0.5,
	
	/** Top **/
	-0.5,0.5,0.5,
	0.5,0.5,0.5,
	0.5,0.5,0.5,		/* results in empty facet */
	0.5,0.5,0.5,		/* results in empty facet */
	0.5,0.5,0.5		/* results in empty facet */
    };
    
    vertData.points = quad_points;
    
    popen_struct(POS_LIGHT_STRUCT);
	pquad_mesh3_data(PFACET_NONE, PVERT_COORD, PMODEL_RGB, quadDim, 
			&facetData, &vertData);
    pclose_struct();
}

createDirLightStructure()
{

    Pint	    err;
    Pmatrix3	    transform;
    static Ppoint3  transformPt = { 0.0, 0.0, 0.0 }; 
    static Pvec3 shift	= { 0.0, 1.0, 0.0 }; 
    static Pvec3 scale	= { 0.02, 0.02, 0.02 }; 

    static Ppoint3  pts[] = {
	0.0,0.0,0.0,
	0.0,1.0,0.0
    };
    Ppoint_list3 colored_line;

    popen_struct(DIR_LIGHT_STRUCT);
	/** The directional light will be a line connected to a small pyramid,
	 ** which takes the place of an arrow.  Initially it will be oriented
	 ** as (0,0,0)->(0,1,0) **/
	pset_linewidth(4.0);
        colored_line.num_points = 2;
        colored_line.points = pts;
        ppolyline3( &colored_line );

	pbuild_tran_matrix3(&transformPt, &shift, 0.0, 0.0, 0.0, &scale, &err,
	    transform);
	pset_local_tran3(transform, PTYPE_POSTCONCAT);	    
	pexec_struct(SINGLE_PYRAMID);
    pclose_struct();
}
    
createSpotLightStructure()
{
    popen_struct(SPOT_LIGHT_STRUCT);
    pclose_struct();
}

initLightStuff()
{
    createPosLightStructure();
    createSpotLightStructure();
}
