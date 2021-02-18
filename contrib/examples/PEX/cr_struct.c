/* $XConsortium: cr_struct.c,v 5.1 91/02/16 09:32:38 rws Exp $ */

/***********************************************************
Copyright (c) 1989,1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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

#include <stdio.h>
#include <phigs/phigs.h>
#include "structdefs.h"
#include "test_ss.h"

static Ppoint3 xyzpoints[2];
static Ppoint xypoints[2];
static Ppoint_list3 pointlst3;
static Ppoint_list pointlst;
static Pparal pgram;
static Prect rect;
static Ppat_rep	colour_array;
static Pmatrix3 xform3 = {
    {1.0, 0.0, 0.0, 0.0},
    {0.0, 1.0, 0.0, 0.0},
    {0.0, 0.0, 1.0, 0.0},
    {0.0, 0.0, 0.0, 1.0}
};
static Pmatrix xform = {
    {1.0, 0.0, 0.0 },
    {0.0, 1.0, 0.0 },
    {0.0, 0.0, 1.0 }
};

void create_struct();

typedef void Oc_status;
typedef Oc_status (*Oc_func)();

static void nothing();
static void polyline3();
static void polyline();
static void polymarker3();
static void polymarker();
static void text3();
static void text();
static void annotationtextrelative3();
static void annotationtextrelative();
static void fillarea3();
static void fillarea();
static void fillareaset3();
static void fillareaset();
static void cellarray3();
static void cellarray();
static void setlineind();
static void setmarkerind();
static void settextind();
static void setintind();
static void setedgeind();
static void setlinetype();
static void setlinewidth();
static void setlinecolourind();
static void setmarkertype();
static void setmarkersize();
static void setmarkercolourind();
static void settextfont();
static void settextprec();
static void setcharexpan();
static void setcharspace();
static void settextcolourind();
static void setcharheight();
static void setcharup();
static void settextpath();
static void settextalign();
static void setannotationcharheight();
static void setannotationcharup();
static void setannotationpath();
static void setannotationalign();
static void setannotationstyle();
static void setintstyle();
static void setbackintstyle();
static void setintstyleind();
static void setintcolourind();
static void setedgeflag();
static void setedgetype();
static void setedgewidth();
static void setedgecolourind();
static void setpatsize();
static void setpatrefptvectors();
static void setpatrefpt();
static void addnameset();
static void removenameset();
static void setindivasf();
static void sethlhsrid();
static void setlocaltran3();
static void setlocaltran();
static void setglobaltran3();
static void setglobaltran();
static void setmodelclipvolume3();
static void setmodelclipvolume();
static void setmodelclipindicator();
static void restoremodelclipvolume();
static void setviewind();
static void executestruct();
static void label();
static void applicationdata();
static void setpickid();
static void settextcolour();
static void setmarkercolour();
static void setedgecolour();
static void setlinecolour();
static void setintcolour();
static void setbackintcolour();
static void setdcueind();
static void setbackintstyleind();
static void setareaprop();
static void setbackareaprop();
static void setlineshadmethod();
static void setcurvapprox();
static void setintshadmethod();
static void setbackintshadmethod();
static void setintreflecteq();
static void setbackintreflecteq();
static void setsurfapprox(); 
static void settrimcurvapprox(); 
static void polylineset3data(); 
static void fillarea3data(); 
static void fillareaset3data(); 
static void tri3data(); 
static void quad3data(); 
static void polyhedron3data(); 
static void nunibspcurv(); 
static void nunibspsurf(); 
static void setfacedistgmode(); 
static void setfacecullmode(); 
static void setlightsrcstate(); 
static void setcolourmodel(); 

static Oc_func oc_func[] = {
    nothing,
    polyline3,
    polyline,
    polymarker3,
    polymarker,
    text3,
    text,
    annotationtextrelative3,
    annotationtextrelative,
    fillarea3,
    fillarea,
    fillareaset3,
    fillareaset,
    cellarray3,
    cellarray,
    nothing, /* gpd3 */
    nothing, /* gpd */
    setlineind,
    setmarkerind,
    settextind,
    setintind,
    setedgeind,
    setlinetype,
    setlinewidth,
    setlinecolourind,
    setmarkertype,
    setmarkersize,
    setmarkercolourind,
    settextfont,
    settextprec,
    setcharexpan,
    setcharspace,
    settextcolourind,
    setcharheight,
    setcharup,
    settextpath,
    settextalign,
    setannotationcharheight,
    setannotationcharup,
    setannotationpath,
    setannotationalign,
    setannotationstyle,
    setintstyle,
    setintstyleind,
    setintcolourind,
    setedgeflag,
    setedgetype,
    setedgewidth,
    setedgecolourind,
    setpatsize,
    setpatrefptvectors,
    setpatrefpt,
    addnameset,
    removenameset,
    setindivasf,
    sethlhsrid,
    setlocaltran3,
    setlocaltran,
    setglobaltran3,
    setglobaltran,
    setmodelclipvolume3,
    setmodelclipvolume,
    setmodelclipindicator,
    restoremodelclipvolume,
    setviewind,
    executestruct,
    label,
    applicationdata,
    nothing,					/* gse */
    setpickid,
    polylineset3data,
    fillarea3data,
    fillareaset3data, /* pfill_area_set3data */
    tri3data, /* ptri3data */
    quad3data, /* pquad3data */
    polyhedron3data, /* ppolyhedron3data */
    nunibspcurv, /* pnunibspcurv */
    nunibspsurf, /* pnunibspsurf */
    nothing, /* ext. cell array3 */
    settextcolour, /* psettextcolour */
    setmarkercolour, /* psetmarkercolour */
    setedgecolour, /* psetedgecolour */
    setlinecolour, /* psetlinecolour */
    setcurvapprox, /* psetcurvapprox */
    setlineshadmethod, /* psetlineshadmethod */
    nothing, /* psetintcolour */
    nothing, /* psetbackintcolour */
    setbackintstyle, /* psetbackintstyle */
    setbackintstyleind, /* psetbackintstyleind */
    setareaprop, /* psetareaprop */
    setbackareaprop, /* psetbackareaprop */
    setintshadmethod, /* psetintshadmethod */
    setbackintshadmethod, /* psetbackintshadmethod */
    setintreflecteq, /* psetintreflecteq */
    setbackintreflecteq, /* psetbackintreflecteq */
    setsurfapprox, /* psetsurfapprox */
    settrimcurvapprox, /* psettrimcurvapprox */
    setfacedistgmode, /* psetfacedistgmode */
    setfacecullmode, /* psetfacecullmode */
    setlightsrcstate, /* psetlightsrcstate */
    setdcueind, /* psetdcueind */
    nothing, /* PEL_COLR_MAPPING_IND */
    setcolourmodel, /* pset_colr_model */
    nothing
};

#define CREATE_ELEM(eltype) oc_func[eltype]()
#define VALID_ELEM_TYPE(eltype) \
    ((eltype > PELEM_NIL) && (eltype < PELEM_NUM_EL_TYPES))

void
create_struct(structlst, elemlst, type)
Pint_list *structlst;
Pint_list *elemlst;
Pint type;
{
    register long i, j;
    register Pint el_type;

    for (i = 0; i < structlst->num_ints; i++) {
	popen_struct(structlst->ints[i]);
	switch (type) {
	case NO_ELEMENTS:
	    break;
	case ALL_ELEMENTS:
	    for (el_type = PELEM_POLYLINE3; 
	    el_type < PELEM_NUM_EL_TYPES; el_type++) {
		CREATE_ELEM(el_type);
	    }
	    break;
	case RANGE_OF_ELEMENTS:
	    if (elemlst->num_ints != 2) {
		pclose_struct();
		(void) fprintf(stderr,
		"# of elements must equal to 2 !\n");
		return;
	    }

	    if (!(VALID_ELEM_TYPE(elemlst->ints[0]) &&
		VALID_ELEM_TYPE(elemlst->ints[1]))) {
		(void) fprintf(stderr,
		"Specified range is invalid !\n");
	    }

	    if (elemlst->ints[0] < elemlst->ints[1]) {
		for (j = elemlst->ints[0];
		j <= elemlst->ints[1]; j++) {
		    (void) fprintf(stderr,"%s\n",oc_names[j]);
		    CREATE_ELEM(j);
		}
	    } else if (elemlst->ints[0] > elemlst->ints[1]) {
		for (j = elemlst->ints[0]; 
		j >= elemlst->ints[1]; j--) {
		    CREATE_ELEM(j);
		}
	    } else if (elemlst->ints[0] == elemlst->ints[1]) {
		CREATE_ELEM(elemlst->ints[0]);
	    } else {
		fprintf(stderr,
		"Error detected in create_struct();\n");
	    }
	    break;
	case SPECIFIED_ELEMENTS:
	    for (j = 0; j < elemlst->num_ints; j++) {
		if (VALID_ELEM_TYPE(elemlst->ints[j])) {
		    (void) fprintf(stderr,
		    "%s\n",oc_names[elemlst->ints[j]]);
		    CREATE_ELEM(elemlst->ints[j]);
		}
	    }
	    break;
	default:
	    break;

	} /* end-switch-case */

	pclose_struct();

    } /* end-for */
}

static void
nothing()
{
}

static void
polyline3()
{
    xyzpoints[0].x = 10.0;
    xyzpoints[0].y = 20.1;
    xyzpoints[0].z = 30.2;
    xyzpoints[1].x = 100.3;
    xyzpoints[1].y = 200.4;
    xyzpoints[1].z = 300.5;
    pointlst3.num_points = 2;
    pointlst3.points = xyzpoints;
    ppolyline3(&pointlst3);
}

static void
polyline()
{
    xypoints[0].x = 0.0;
    xypoints[0].y = 0.0;
    xypoints[1].x = 1.0;
    xypoints[1].y = 1.0;
    pointlst.num_points = 2;
    pointlst.points = xypoints;
    ppolyline(&pointlst);
}

static void
polymarker3()
{
    ppolymarker3(&pointlst3);
}

static void
polymarker()
{
    ppolymarker(&pointlst);
}

static void
text3()
{
    ptext3(&xyzpoints[0],xyzpoints,"ptext3");
}

static void
text()
{
    ptext(&xypoints[0],"ptext");
}

static void
annotationtextrelative3()
{
    panno_text_rel3(&xyzpoints[0],&xyzpoints[1],
    "panno_text_rel3");
}

static void
annotationtextrelative()
{
    panno_text_rel(&xypoints[0],&xypoints[1],
    "panno_text_rel");
}

static void
fillarea3()
{
    pfill_area3(&pointlst3);
}

static void
fillarea()
{
    pfill_area(&pointlst);
}

static void
fillareaset3()
{
    pointlst3.num_points = 2;
    pointlst3.points = xyzpoints;
    pfill_area_set3(1,&pointlst3);
}

static void
fillareaset()
{
    pointlst.num_points = 2;
    pointlst.points = xypoints;
    pfill_area_set(1,&pointlst);
}

static void
cellarray3()
{
    pgram.p.x = 0.0, pgram.p.y = 0.0, pgram.p.z = 0.0;
    pgram.q.x = 1.0, pgram.q.y = 1.0, pgram.q.z = 1.0;
    pgram.r.x = 2.0, pgram.r.y = 2.0, pgram.r.z = 2.0;
    colour_array.dims.size_x = 2, colour_array.dims.size_y = 2;
    colour_array.colr_array = (Pint *) malloc((colour_array.dims.size_x
				    * colour_array.dims.size_y) * sizeof(Pint));
    colour_array.colr_array[0] = 1, colour_array.colr_array[1] = 2;
    colour_array.colr_array[2] = 3, colour_array.colr_array[3] = 4;
    pcell_array3(&pgram,&colour_array);
    free(colour_array.colr_array);
}

static void
cellarray()
{
    rect.p.x = 0.0, rect.p.y = 0.0;
    rect.q.x = 1.0, rect.q.y = 1.0;
    colour_array.dims.size_x = 2, colour_array.dims.size_y = 2;
    colour_array.colr_array = (Pint *) malloc((colour_array.dims.size_x
				    * colour_array.dims.size_y) * sizeof(Pint));
    colour_array.colr_array[0] = 1, colour_array.colr_array[1] = 2;
    colour_array.colr_array[2] = 3, colour_array.colr_array[3] = 4;
    pcell_array3(&rect,&colour_array);
    free(colour_array.colr_array);
}

static void
setlineind()
{
    pset_line_ind(7);
}

static void
setmarkerind()
{
    pset_marker_ind(8);
}

static void
settextind()
{
    pset_text_ind(9);
}

static void
setintind()
{
    pset_int_ind(10);
}

static void
setedgeind()
{
    pset_edge_ind(11);
}

static void
setlinetype()
{
    pset_linetype(PLINE_DASH);
}

static void
setlinewidth()
{
    pset_linewidth(1.0);
}

static void
setlinecolourind()
{
    pset_line_colr_ind(1);
}

static void
setmarkertype()
{
    pset_marker_type(PMARKER_CROSS);
}

static void
setmarkersize()
{
    pset_marker_size(2.0);
}

static void
setmarkercolourind()
{
    pset_marker_colr_ind(2);
}

static void
settextfont()
{
    pset_text_font(1);
}

static void
settextprec()
{
    pset_text_prec(PPREC_STRING);
}

static void
setcharexpan()
{
    pset_char_expan(2.0);
}

static void
setcharspace()
{
    pset_char_space(0.4);
}

static void
settextcolourind()
{
    pset_text_colr_ind(1);
}

static void
setcharheight()
{
    pset_char_ht(0.05);
}

static void
setcharup()
{
    Pvec up_vector;
    up_vector.delta_x = 1.0;
    up_vector.delta_y = 1.0;
    pset_char_up_vec(&up_vector);
}

static void
settextpath()
{
    pset_text_path(PPATH_UP);
}

static void
settextalign()
{
    Ptext_align text_align;
    text_align.hor = PHOR_NORM;
    text_align.vert = PVERT_NORM;
    pset_text_align(&text_align);
}

static void
setannotationcharheight()
{
    pset_anno_char_ht(0.02);
}

static void
setannotationcharup()
{
    Pvec up_vect;
    up_vect.delta_x = 0.0;
    up_vect.delta_y = 0.2;
    pset_anno_char_up_vec(&up_vect);
}

static void
setannotationpath()
{
    pset_anno_path(PPATH_DOWN);
}

static void
setannotationalign()
{
    Ptext_align text_align;
    text_align.hor = PHOR_NORM;
    text_align.vert = PVERT_NORM;
    pset_anno_align(&text_align);
}

static void
setannotationstyle()
{
    pset_anno_style(PANNO_STYLE_UNCONNECTED);
}

static void
setintstyle()
{
    pset_int_style(PSTYLE_SOLID);
}

static void  
setbackintstyle()
{
    pset_back_int_style(PSTYLE_SOLID);
}

static void
setintstyleind()
{
    pset_int_style_ind(1);
}

static void
setintcolourind()
{
    pset_int_colr_ind(1);
}

static void
setedgeflag()
{
    pset_edge_flag(PEDGE_OFF);
}
static void
setedgetype()
{
    pset_edgetype(PLINE_SOLID);
}
static void
setedgewidth()
{
    pset_edgewidth(0.1);
}
static void
setedgecolourind()
{
    pset_edge_colr_ind(0);
}
static void
setpatsize()
{
    Ppoint size;
    size.x = 1.0, size.y = 1.0;
    pset_pat_size(size.x, size.y);
}

static void
setpatrefptvectors()
{
    Ppoint3 ref_pt;
    Pvec3 *dxy[2];
    ref_pt.x = 0.0, ref_pt.y = 1.0, ref_pt.z = 0.0;
    if (!(dxy[0] = (Pvec3 *)malloc(sizeof(Pvec3)*2))) {
	fprintf (stderr, "Malloc Failure in setpatrefptvectors\n");
	exit (1);
    }
    dxy[0]->delta_x = 1.0, dxy[0]->delta_y = 1.0, dxy[0]->delta_z = 1.0;
    dxy[1] = (Pvec3 *) (dxy[0] + sizeof(Pvec3));
    dxy[1]->delta_x = 1.0, dxy[1]->delta_y = 1.0, dxy[1]->delta_z = 1.0;
    pset_pat_ref_point_vecs(&ref_pt,dxy);
}

static void
setpatrefpt()
{
    Ppoint ref_pt;
    ref_pt.x = 0.0, ref_pt.y = 1.0;
    pset_pat_ref_point(&ref_pt);
}

static void
addnameset()
{
    Pint_list list;
    Pint some_name = 1;
    list.num_ints = 1; 
    list.ints = &some_name;
    padd_names_set(&list);
}

static void
removenameset()
{
    Pint_list list;
    Pint some_name = 1;
    list.num_ints = 1; 
    list.ints = &some_name;
    premove_names_set(&list);
}

static void
setindivasf()
{
    pset_indiv_asf(PASPECT_LINETYPE,PASF_INDIV);
}

static void
sethlhsrid()
{
    pset_hlhsr_id(PHIGS_HLHSR_MODE_NONE);
}

static void
setlocaltran3()
{
    pset_local_tran3(xform3,PTYPE_PRECONCAT);
}

static void
setlocaltran()
{
    pset_local_tran(xform,PTYPE_PRECONCAT);
}

static void
setglobaltran3()
{
    pset_global_tran3(xform3);
}

static void
setglobaltran()
{
    pset_global_tran(xform);
}

static void
setmodelclipvolume3()
{
    Phalf_space_list3 half_spacelst3;
    Phalf_space3 half_space3;
    half_space3.point.x = 1.0, 
    half_space3.point.y = 1.0,
    half_space3.point.z = 1.0;
    half_space3.norm.delta_x = .1;
    half_space3.norm.delta_y = .1;
    half_space3.norm.delta_z = .1;
    half_spacelst3.num_half_spaces = 1;
    half_spacelst3.half_spaces = &half_space3;
    pset_model_clip_vol3(PMC_REPLACE,&half_spacelst3);
}

static void
setmodelclipvolume()
{
    Phalf_space_list half_spacelst;
    Phalf_space half_space;
    half_space.point.x = 1.0, 
    half_space.point.y = 1.0,
    half_space.norm.delta_x = .1;
    half_space.norm.delta_y = .1;
    half_spacelst.num_half_spaces = 1;
    half_spacelst.half_spaces = &half_space;
    pset_model_clip_vol(PMC_REPLACE,&half_spacelst);
}

static void
setmodelclipindicator()
{
    pset_model_clip_ind(PIND_NO_CLIP);
}

static void
restoremodelclipvolume()
{
    prestore_model_clip_vol();
}

static void
setviewind()
{
    pset_view_ind(1);
}

static void
executestruct()
{
    pexec_struct(2);
}

static void
label()
{
    plabel(1);
}

static void
applicationdata()
{
    Pdata data;
    char some_data = '*';
    data.size = 1;
    data.data = &some_data;
    pappl_data(&data);
}

static void
setpickid()
{
    pset_pick_id(8);
}

static void
settextcolour()
{    
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 127.;
    colour.val.general.y = 255.;
    colour.val.general.z = 127.;

    pset_text_colr(&colour);
}

static void
setmarkercolour()
{
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 0.;
    colour.val.general.y = 255.;
    colour.val.general.z = 0.;

    pset_marker_colr(&colour);
}

static void
setedgecolour()
{
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 255.;
    colour.val.general.y = 127.;
    colour.val.general.z = 255.;

    pset_edge_colr(&colour);
}

static void
setlinecolour()
{    
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 255.;
    colour.val.general.y = 127.;
    colour.val.general.z = 0.;

    pset_line_colr(&colour);
}

static void
setintcolour()
{    
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 0.;
    colour.val.general.y = 127.;
    colour.val.general.z = 255.;
    pset_int_colr(&colour);
}

static void
setbackintcolour()
{
    Pgcolr colour;

    colour.type = PMODEL_RGB;
    colour.val.general.x = 0.;
    colour.val.general.y = 1.;
    colour.val.general.z = 255.;

    pset_back_int_colr(&colour);
}

static void
setbackintstyleind()
{
    pset_back_int_style_ind(21);
}

static void
setdcueind()
{
    pset_dcue_ind(8);
}

static void
setareaprop()
{
    Prefl_props properties;

    properties.ambient_coef = 1.0;
    properties.diffuse_coef = 1.0;
    properties.specular_coef = 1.0;
    properties.specular_colr.type = PINDIRECT;
    properties.specular_colr.val.ind = 88;
    properties.specular_exp = 1.0;
    properties.transpar_coef = 1.0;
    pset_refl_props(&properties);
}

static void
setbackareaprop()
{
    Prefl_props properties;

    properties.ambient_coef = 1.0;
    properties.diffuse_coef = 1.0;
    properties.specular_coef = 1.0;
    properties.specular_colr.type = PINDIRECT;
    properties.specular_colr.val.ind = 88;
    properties.specular_exp = 1.0;
    properties.transpar_coef = 1.0;

    pset_back_refl_props(&properties);
}

static void
setcurvapprox()
{
    Pint type = PCURV_CONSTANT_PARAMETRIC_BETWEEN_KNOTS;
    Pfloat value = 1.0;

    pset_curve_approx(type, value);
}

static void
setlineshadmethod()
{
    pset_line_shad_meth(PSD_NONE);
}

static void
setintshadmethod()
{
    pset_int_shad_meth(PSD_NONE);
}

static void
setbackintshadmethod()
{
    pset_back_int_shad_meth(PSD_NONE);
}

static void
setintreflecteq()
{
    pset_refl_eqn(PREFL_NONE);
}

static void
setbackintreflecteq()
{
    pset_back_refl_eqn(PREFL_NONE);
}

static void
setsurfapprox()
{
    Pint type = PSURF_WS_DEP;
    Pfloat u_value = 0.0;
    Pfloat v_value = 0.0;

    pset_surf_approx(type, u_value, v_value);
}

static void 
settrimcurvapprox()
{
    Pint type = PCURV_WS_DEP;
    Pfloat value = 1.0;
/***
****  GONE ! ! ! 
****
    pset_trimcurve_approx(type,value);
****
****
***/
}
static void 
polylineset3data()
{
    Pline_vdata_list3 vdata;

    static Ppoint3 points[2] = {
	{0.,0.,0.},
	{1.,1.,1.}
    };

    vdata.num_vertices = 2;
    vdata.vertex_data.points = &points[0];

    ppolyline_set3_data(PVERT_COORD,PMODEL_RGB,1,&vdata); 
}

static void 
fillarea3data()
{
    Pfacet_data3 fdata;
    Pfacet_vdata_arr3 vdata;

    static Ppoint3 points[4] = {
	{0.,0.,0.},
	{0.,1.,0.},
	{1.,1.,0.},
	{1.,0.,0.}
    };

    fdata.colr.direct.rgb.red = 127.;
    fdata.colr.direct.rgb.green = 127.;
    fdata.colr.direct.rgb.blue = 127.;

    vdata.points = &points[0];

    pfill_area_set3_data(PFACET_NONE, PVERT_COORD, PMODEL_RGB, &fdata, 4, &vdata); 
}

static void 
fillareaset3data()
{
    Pfacet_data3 fdata;
    Pedge_data_list edata;
    Pfacet_vdata_list3 vdata;

    static Ppoint3 points[4] = {
	{0.,0.,0.},
	{0.,1.,0.},
	{1.,1.,0.},
	{1.,0.,0.}
    };

    fdata.colr.direct.rgb.red = 127.;
    fdata.colr.direct.rgb.green = 128.;
    fdata.colr.direct.rgb.blue = 129.;

    edata.num_edges = 0;

    vdata.num_vertices = 4;
    vdata.vertex_data.points = &points[0];

    pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD, PMODEL_RGB,
    &fdata, 1, &edata, &vdata); 
}

static void 
tri3data()
{ 
    static Ppoint3 points[3] = {
	{0.,0.,0.},
	{0.,1.,0.},
	{1.,1.,0.}
    };

    Pcoval colour;
    Pfacet_data_arr3 fdata;    
    Pfacet_vdata_arr3 vdata;    

    colour.direct.rgb.red = 127.;
    colour.direct.rgb.green = 128.;
    colour.direct.rgb.blue = 129.;
    fdata.colrs = &colour;
    vdata.points = &points[0];

    ptri_strip3_data(PFACET_NONE,PVERT_COORD,PMODEL_RGB,3,&fdata,&vdata);
}

static void 
quad3data()
{
    static Ppoint3 points[4] = {
	{0.,0.,0.},
	{0.,1.,0.},
	{1.,1.,0.},
	{1.,0.,0.}
    };

    static Pint_size dim = {2, 2};
    Pcoval colour;
    Pfacet_data_arr3 fdata;
    Pfacet_vdata_arr3 vdata;

    colour.direct.rgb.red = 127.;
    colour.direct.rgb.green = 128.;
    colour.direct.rgb.blue = 129.;
    fdata.colrs = &colour;

    vdata.points = &points[0];

    pquad_mesh3_data(PFACET_NONE, PVERT_COORD, PMODEL_RGB,
    &dim, &fdata, &vdata); 
}

static void 
polyhedron3data()
{
#ifdef NOT_READY
    static Ppoint3 points[4] = {
	{0.,0.,0.},
	{0.,1.,0.},
	{1.,1.,0.},
	{1.,0.,0.}
    };

    Pcoval colour;
    Pfacet_data_arr3 fdata;
    Pedge_data_list edata;
    Pint_list vlist;
    Pfacet_vdata_list3 vdata;

    colour.direct.rgb.red = 127.;
    colour.direct.rgb.green = 128.;
    colour.direct.rgb.blue = 129.;
    fdata.colr = &colour;

    edata.num_ints = 0;

    vlist.num_ints = 4;
    vlist.ints = (Pint *) malloc(4 * sizeof(Pint));
    vlist.ints[0] = 1;
    vlist.ints[1] = 2;
    vlist.ints[2] = 3;
    vlist.ints[3] = 4;

    vdata.num_ints = 4;
    vdata.facetdata.points = &points[0];

    ppolyhedron3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD, 1,
    &fdata, &edata, &vlist, &vdata); 
#endif /* NOT_READY */
}

static void 
nunibspcurv()
{
#ifdef NOT_READY
    Pint order;
    Pfloat_list knots;
    Prational rationality;
    Ppoint_list34 ctlpoints;
    Pfloat min, max;

    pnuni_bsp_curv(order, &knots, rationality, &ctlpoints, min, max); 
#endif /* NOT_READY */
}

static void 
nunibspsurf()
{
#ifdef NOT_READY
    Pint uorder;
    Pint vorder;
    Pfloat_list uknots;
    Pfloat_list vknots;
    Ppoint_grid34 grid;
    Ppcs_limit range;
    Pint num_tloops;
    Ptrimcurve_list tloops;

    pnuni_bsp_surf(uorder, vorder, uknots, vknots,
    grid, range, num_tloops, tloop); 
#endif /* NOT_READY */
}

static void 
setfacedistgmode()
{
    Pdisting_mode mode = PDISTING_NO;

    pset_face_disting_mode(mode); 
}

static void
setfacecullmode()
{
    Pcull_mode mode =  PCULL_NONE;

    pset_face_cull_mode(mode); 
}


static void
setlightsrcstate()
{
    Pint_list activation;
    Pint_list deactivation;

    activation.num_ints = 0;
    deactivation.num_ints = 0;

    pset_light_src_state(&activation, &deactivation); 
}

static void
setcolourmodel() 
{ 
}
