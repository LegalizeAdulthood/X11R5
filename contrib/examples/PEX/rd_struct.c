/* $XConsortium: rd_struct.c,v 5.2 91/02/16 09:32:39 rws Exp $ */

/***********************************************************
Copyright 1989, 1990, 1991 by Sun Microsystems, Inc. and the X Consortium.

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
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define PRINT(msg)    (void) fprintf(stderr,msg)
#define PRINTXY(X,Y) \
    (void) fprintf(stderr,"(%g,%g)",(X),(Y))
#define PRINTXYZ(X,Y,Z) \
    (void) fprintf(stderr,"(%g,%g,%g)",(X),(Y),(Z))
#define PRINTSTR(string) \
    (void) fprintf(stderr,"\"%s\"",string)
#define PRINTNAME(name) \
    (void) fprintf(stderr,"%s",name)
#define PRINTINT(number) \
    (void) fprintf(stderr,"%d",number)
#define PRINTFLOAT(number) \
    (void) fprintf(stderr,"%g",number)
#define PRINTINTPAIR(X,Y) \
    (void) fprintf(stderr,"{%d,%d}",(X),(Y))
#define OPEN_BRACKET    (void) fprintf(stderr,"{")
#define CLOSE_BRACKET    (void) fprintf(stderr,"}")
#define COMMA (void) fprintf(stderr,",")
#define ADD_COMMA_IF_NOT_LAST(i,limit) \
	if (i < limit) (void) fprintf(stderr, ",") 

typedef void Print_status;
typedef Print_status (*Print_func)();

void read_struct();

static Print_status print_nothing();
static Print_status print_pointlst3();
static Print_status print_pointlst();
static Print_status print_text3();
static Print_status print_tx();
static Print_status print_atx3();
static Print_status print_atx();
static Print_status print_point();
static Print_status print_fas3();
static Print_status print_fas();
static Print_status print_ca3();
static Print_status print_ca();
static Print_status print_idata();
static Print_status print_fdata();
static Print_status print_txprec();
static Print_status print_vector();
static Print_status print_txpath();
static Print_status print_txalign();
static Print_status print_interstyle();
static Print_status print_edgef();
static Print_status print_linetype();
static Print_status print_markertype();
static Print_status print_pat();
static Print_status print_annotationstyle();
static Print_status print_intlst();
static Print_status print_asf();
static Print_status print_mtran3();
static Print_status print_mtran();
static Print_status print_matrix3();
static Print_status print_matrix();
static Print_status print_mclip3();
static Print_status print_mclip();
static Print_status print_clip();
static Print_status print_data();
static Print_status print_hlhsrid();
static Print_status print_colour();

static Print_func print_func[] = {
    print_nothing,
    print_pointlst3,
    print_pointlst,
    print_pointlst3,
    print_pointlst,
    print_text3,
    print_tx,
    print_atx3,
    print_atx,
    print_pointlst3,
    print_pointlst,
    print_fas3,
    print_fas,
    print_ca3,
    print_ca,
    print_nothing, /* pgpd3 */
    print_nothing, /* pgpd */
    print_idata, /* pset_line_ind */
    print_idata, /* pset_marker_ind */
    print_idata, /* pset_text_ind */
    print_idata, /* pset_int_ind */
    print_idata, /* pset_edge_ind */
    print_linetype, /* pset_linetype */
    print_fdata,  /* pset_linewidth */
    print_idata, /* pset_line_colr_ind */
    print_markertype, /* pset_marker_type */
    print_fdata, /* pset_marker_size */
    print_idata, /* pset_marker_colr_ind */
    print_idata, /* pset_text_font */
    print_txprec, /* pset_text_prec */
    print_fdata, /* psetcharexp */
    print_fdata, /* psetcharspacing */
    print_idata, /* pset_text_colr_ind */
    print_fdata, /* pset_char_ht */
    print_vector, /* pset_char_up_vec */
    print_txpath, /* pset_text_path */
    print_txalign, /* pset_text_align */
    print_fdata, /* pset_anno_char_ht */
    print_vector,
    print_txpath,
    print_txalign,
    print_annotationstyle, /* setannotationstyle */
    print_interstyle,
    print_idata,
    print_idata,
    print_edgef,
    print_linetype,
    print_fdata,
    print_idata,
    print_point,
    print_pat,
    print_point,
    print_intlst, /* padd_names_set */
    print_intlst, /* premove_names_set */
    print_asf, 
    print_hlhsrid, /* pset_hlhsr_id */
    print_mtran3,
    print_mtran,
    print_matrix3,
    print_matrix,
    print_mclip3,
    print_mclip,
    print_clip,
    print_nothing,
    print_idata, /* pset_view_ind */
    print_idata, /* plabel */
    print_idata, /* pexec_struct */
    print_data, /* pappl_data */
    print_nothing, /* pgse */
    print_idata, /* pset_pick_id */
    print_nothing, /* ppolylineset3data */
    print_nothing, /* pfill_area3data */
    print_nothing, /* pfill_area_set3data */
    print_nothing, /* ptri3data */
    print_nothing, /* pquad3data */
    print_nothing, /* ppolyhedron3data */
    print_nothing, /* pnunibspcurv */
    print_nothing, /* pnunibspsurf ? */
    print_nothing, /* ext. cell array3 */
    print_colour, /* psettextcolour */
    print_colour, /* psetmarkercolour */
    print_colour, /* psetedgecolour */
    print_colour, /* psetlinecolour */
    print_nothing, /* setcurvapprox */
    print_nothing, /* setlineshadmethod */
    print_colour, /* psetintcolour */
    print_colour, /* psetbackintcolour */
    print_interstyle, /* setbackintstyle */
    print_idata, /* setbackintstyleind */
    print_nothing, /* setareaprop */
    print_nothing, /* setbackareaprop */
    print_nothing, /* setintshadmethod */
    print_nothing, /* setbackintshadmethod */
    print_nothing, /* setintreflecteq */
    print_nothing, /* setbackintreflecteq */
    print_nothing, /* PELEM_SURF_APPROX_CRIT */
    print_nothing, /* PEL_TRIM_CURVE_APPROX_CRITERIA */
    print_nothing, /* setfacedistgmode */
    print_nothing, /* setfacecullmode */
    print_nothing, /* setlightsrcstate */
    print_idata, /* setdcueind */
    print_nothing, /* PEL_COLR_MAPPING_IND */
    print_nothing, /* setcolourmodel */
    print_nothing
};

static char *oc_names[] = {
    "Nil",
    "Polyline 3",
    "Polyline",
    "PolyMarker 3",
    "PolyMarker",
    "Text 3",
    "Text",
    "ANNOTATION_TEXT3",
    "ANNOTATION_TEXT",
    "Fill Area 3",
    "Fill Area",
    "Fill Area Set 3",
    "Fill Area Set",
    "Cell Array 3",
    "Cell Array",
    "Generalized Drawing Primitive 3",
    "Generalized Drawing Primitive",
    "Polyline Index",
    "Polymarker Index",
    "Text Index",
    "Interior Index",
    "Edge Index",
    "LINE_TYPE",
    "LINE_WIDTH",
    "LINE_COLOUR_INDEX",
    "MARKER_TYPE",
    "MARKER_SIZE",
    "MARKER_COLOR_INDEX",
    "TEXT_FONT",
    "TEXT_PREC",
    "CHAR_EXP",
    "CHAR_SPACE",
    "TEXT_COLOUR_INDEX",
    "CHAR_HEIGHT",
    "CHAR_UP_VECTOR",
    "TEXT_PATH",
    "TEXT_ALIGN",
    "ANNO_TEXT_CHAR_HEIGHT",
    "ANNO_TEXT_CHAR_UP_VECTOR",
    "Annotation Text Path",
    "Annotation Text Alignment",
    "Annotation Style",
    "INTERIOR_STYLE",
    "Interior Style Index",
    "Interior Style Colour Index",
    "Edge Flag",
    "Edge Type",
    "Edge Width Scale Factor",
    "Edge Colour Index",
    "Pattern Size",
    "Pattern Reference Point and Vectors",
    "Pattern Reference Point",
    "Add Names To Set",
    "Remove Names From Set",
    "Individual Asf",
    "Hlhsr Identifier",
    "Local Modelling Transformation 3",
    "Local Modelling Transformation",
    "Global Modelling Transformation 3",
    "Global Modelling Transformation",
    "Modelling Clipping Volume 3",
    "Modelling Clipping Volume",
    "Modelling Clipping Indicator",
    "Restore Modelling Clipping Volume",
    "View Index",
    "Execute Structure",
    "Label",
    "Application Data",
    "Generalize Structure Element",
    "Pick ID",
    "Polyline Set 3 With Data",
    "Fill Area 3 With Data",
    "Fill Area Set With Data",
    "Triangle Strip 3 With Data",
    "Quad Mesh 3 With Data",
    "Polyhedron 3 With Data",
    "NURB Curve",
    "NURB Surface",
    "Extended Cell Array 3",
    "Text Colour",
    "MARKER_COLOUR",
    "Edge Colour",
    "Polyline Colour",
    "Curve Approximation Criteria",
    "Polyline Shading Method",
    "Interior Colour",
    "Back Interior Colour",
    "Back Interior Style",
    "Back Interior Style Index",
    "Area Properties",
    "Back Area Properties",
    "Interior Shading Method",
    "Back Interior Shading Method",
    "Interior Reflectance Equation",
    "Back Interior Reflectance Equation",
    "Surface Approximation Criteria",
    "Trim Curve Approximation Criteria",
    "Face Distinguishing Mode",
    "Face Culling Mode",
    "Light Source State",
    "DEPTHCUE_INDEX",
    "Colour Mapping Index",
    "Rendering Colour Model"
};

static char *linetypes[] = {
    "",
    "PLINE_SOLID",
    "PLINE_DASH",
    "PLINE_DOT",
    "PLINE_DOT_DASH"
};

static char *markertypes[] = {
    "",
    "PMARKER_DOT",
    "PMARKER_PLUS",
    "PMARKER_ASTERISK",
    "PMARKER_CIRCLE",
    "PMARKER_CROSS"
};

static char *txprec[] = {
    "PPREC_STRING",
    "PPREC_CHAR",
    "PPREC_STROKE"
};

static char *txpath[] = {
    "PPATH_RIGHT",
    "PPATH_LEFT",
    "PPATH_UP",
    "PPATH_DOWN"
};

static char *txhor[] = {
    "PHOR_NORM",
    "PHOR_LEFT",
    "PHOR_CTR",
    "PHOR_RIGHT"
};

static char *txver[] = {
    "PVERT_NORM",
    "PVERT_TOP",
    "PVERT_CAP",
    "PVERT_HALF",
    "PVERT_BASE",
    "PVERT_BOTTOM"
};

static char *edgeflags[] = {
    "PEDGE_OFF",
    "PEDGE_ON"
};

static char *interstyle[] = {
    "PSTYLE_HOLLOW",
    "PSTYLE_SOLID",
    "PSTYLE_PAT",
    "PSTYLE_HATCH",
    "PSTYLE_EMPTY",
    "PGENERAL"
};

static char *annotationstyle[] = {
    "",
    "PANNO_STYLE_UNCONNECTED",
    "PANNO_STYLE_LEAD_LINE"
};

static char *attrid [] = {
    "PASPECT_LINETYPE",
    "PASPECT_LINEWIDTH",
    "PASPECT_LINE_COLR_IND",
    "PASPECT_MARKERTYPE",
    "PASPECT_MARKERSIZE",
    "PASPECT_MARKER_COLR_IND",
    "PASPECT_TEXT_FONT",
    "PASPECT_TEXT_PREC",
    "PASPECT_CHAR_EXPAN",
    "PASPECT_CHAR_SPACE",
    "PASPECT_TEXT_COLR_IND",
    "PASPECT_INT_STYLE",
    "PASPECT_INT_STYLE_IND",
    "PASPECT_INT_COLR_IND",
    "PASPECT_EDGE_FLAG",
    "PASPECT_EDGETYPE",
    "PASPECT_EDGEWIDTH",
    "PASPECT_EDGE_COLR_IND",
    "PASPECT_CURVE_APPROX_CRIT",
    "PASPECT_SURF_APPROX_CRIT",
    "PTRIMMING_CURVE_APPROXIMATION_CRITERIA",
    "PASPECT_LINE_SHAD_METH",
    "PASPECT_REFL_PROPS",
    "PASPECT_INT_REFL_EQN",
    "PASPECT_INT_SHAD_METH",
    "PASPECT_BACK_INT_STYLE",
    "PASPECT_BACK_INT_STYLE_IND",
    "PASPECT_BACK_INT_COLR",
    "PASPECT_BACK_REFL_PROPS",
    "PASPECT_BACK_INT_REFL_EQN",
    "PASPECT_BACK_INT_SHAD_METH"
};

static char *asf[] =  {
    "PASF_BUNDLED",
    "PASF_INDIV"
};

static char *hlhsrid[] = {
    "PHIGS_HLHSR_ID_NONE",
    "PHIGS_HLHSR_ID_ZBUFF"
};

static char *clip[] = {
    "PIND_CLIP",
    "PIND_NO_CLIP"
};

static char *operator[] = {
    "PMC_REPLACE",
    "PMC_INTERSECT"
};

static char *comptype[] = {
    "PTYPE_PRECONCAT",
    "PTYPE_POSTCONCAT",
    "PTYPE_REPLACE"
};

static Print_status
print_nothing(el_data)
register Pelem_data *el_data;
{
}

static Print_status
print_idata(el_data)
register Pelem_data *el_data;
{
    PRINTINT(el_data->int_data);
}

static Print_status
print_fdata(el_data)
register Pelem_data *el_data;
{
    PRINTFLOAT(el_data->float_data);
}

static Print_status
print_intlst(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint limit = el_data->names.num_ints;

    OPEN_BRACKET;
	PRINTINT(limit);
	COMMA;
	OPEN_BRACKET;
	for (i = 0; i < limit; i++) {
	    PRINTINT(el_data->names.ints[i]);
	    ADD_COMMA_IF_NOT_LAST(i,limit - 1); 
	}
	CLOSE_BRACKET;
    CLOSE_BRACKET;
}

static Print_status
print_pointlst3(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint limit = el_data->point_list3.num_points;

    for (i = 0; i < el_data->point_list3.num_points; i++) {
	PRINTXYZ(el_data->point_list3.points[i].x,
	el_data->point_list3.points[i].y,el_data->point_list3.points[i].z); 
	ADD_COMMA_IF_NOT_LAST(i,limit - 1); 
    }
}

static Print_status
print_pointlst(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint limit = el_data->point_list.num_points;

    for (i = 0; i < el_data->point_list.num_points; i++) {
	PRINTXY(el_data->point_list.points[i].x,el_data->point_list.points[i].y); 
	ADD_COMMA_IF_NOT_LAST(i,limit - 1); 
    }
}

static Print_status
print_text3(el_data)
register Pelem_data *el_data;
{
    PRINTXYZ(el_data->text3.pos.x,el_data->text3.pos.y,
    el_data->text3.pos.z);

    OPEN_BRACKET;

    PRINTXYZ(el_data->text3.dir[0].delta_x,el_data->text3.dir[0].delta_y,
    el_data->text3.dir[0].delta_z);

    COMMA;

    PRINTXYZ(el_data->text3.dir[1].delta_x,el_data->text3.dir[1].delta_y,
    el_data->text3.dir[1].delta_z);

    CLOSE_BRACKET;

    PRINTSTR(el_data->text3.char_string);
}

static Print_status
print_tx(el_data)
register Pelem_data *el_data;
{
    PRINTXY(el_data->text.pos.x,el_data->text.pos.y);

    COMMA;

    PRINTSTR(el_data->text.char_string);
}

static Print_status
print_atx3(el_data)
register Pelem_data *el_data;
{
    PRINTXYZ(el_data->anno_text_rel3.ref_point.x,el_data->anno_text_rel3.ref_point.y,
    el_data->anno_text_rel3.ref_point.z);

    COMMA;

    PRINTXYZ(el_data->anno_text_rel3.offset.delta_x,
		el_data->anno_text_rel3.offset.delta_y,
		el_data->anno_text_rel3.offset.delta_z);

    COMMA;

    PRINTSTR(el_data->anno_text_rel3.char_string);
}

static Print_status
print_atx(el_data)
register Pelem_data *el_data;
{
    PRINTXY(el_data->anno_text_rel.ref_point.x,el_data->anno_text_rel.ref_point.y);

    COMMA;

    PRINTXY(el_data->anno_text_rel.offset.delta_x,el_data->anno_text_rel.offset.delta_y);

    COMMA;

    PRINTSTR(el_data->anno_text_rel.char_string);
}

static Print_status
print_fas3(el_data)
register Pelem_data *el_data;
{
    register long i,j;

    PRINTINT(el_data->fill_area_set3.num_point_lists);

    COMMA;

    for (i = 0; i < el_data->fill_area_set3.num_point_lists; i++) {
	OPEN_BRACKET;
	PRINTINT(el_data->fill_area_set3.point_lists[i].num_points); 
	COMMA;
	for (j = 0; j < el_data->fill_area_set3.point_lists[i].num_points; j++) {
	    PRINTXYZ(el_data->fill_area_set3.point_lists[i].points[j].x,
		el_data->fill_area_set3.point_lists[i].points[j].y,
		el_data->fill_area_set3.point_lists[i].points[j].z);
	    ADD_COMMA_IF_NOT_LAST(j,el_data->fill_area_set3.point_lists[i].num_points - 1);
	}
	CLOSE_BRACKET;
    }
}

static Print_status
print_fas(el_data)
register Pelem_data *el_data;
{
    register long i,j;

    PRINTINT(el_data->fill_area_set.num_point_lists);

    COMMA;

    for (i = 0; i < el_data->fill_area_set.num_point_lists; i++) {
	OPEN_BRACKET;
	PRINTINT(el_data->fill_area_set.point_lists[i].num_points); 
	COMMA;
	for (j = 0; j < el_data->fill_area_set.point_lists[i].num_points; j++) {
	    PRINTXY(el_data->fill_area_set.point_lists[i].points[j].x,
		el_data->fill_area_set.point_lists[i].points[j].y);
	    ADD_COMMA_IF_NOT_LAST(j,el_data->fill_area_set.point_lists[i].num_points - 1);
	}
	CLOSE_BRACKET;
    }
}

static Print_status
print_ca3(el_data)
register Pelem_data *el_data;
{
    register long i;
    Pint x_dim = el_data->cell_array3.dim.size_x;
    Pint y_dim = el_data->cell_array3.dim.size_y;
    Pint num_indices = x_dim * y_dim;

    OPEN_BRACKET;
    PRINTXYZ(el_data->cell_array3.paral.p.x,
	 el_data->cell_array3.paral.p.y,
	 el_data->cell_array3.paral.p.z);
    ADD_COMMA_IF_NOT_LAST(0,2);
    PRINTXYZ(el_data->cell_array3.paral.q.x,
	 el_data->cell_array3.paral.q.y,
	 el_data->cell_array3.paral.q.z);
    ADD_COMMA_IF_NOT_LAST(1,2);
    PRINTXYZ(el_data->cell_array3.paral.r.x,
	 el_data->cell_array3.paral.r.y,
	 el_data->cell_array3.paral.r.z);
    ADD_COMMA_IF_NOT_LAST(2,2);
    CLOSE_BRACKET;
    COMMA;
    PRINTINTPAIR(x_dim,y_dim);
    COMMA;
    
    OPEN_BRACKET;
    for (i = 0; i < num_indices; i++) {
	PRINTINT(el_data->cell_array3.colr[i]);
	ADD_COMMA_IF_NOT_LAST(i,num_indices - 1);
    }
    CLOSE_BRACKET;
}

static Print_status
print_ca(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint x_dim = el_data->cell_array.dim.size_x;
    register Pint y_dim = el_data->cell_array.dim.size_y;
    register Pint num_indices = x_dim * y_dim;

    OPEN_BRACKET;
	PRINTXY(el_data->cell_array.rect.p.x,
	    el_data->cell_array.rect.p.y);
	COMMA;
	PRINTXY(el_data->cell_array.rect.q.x,
	    el_data->cell_array.rect.q.y);
    CLOSE_BRACKET;
    COMMA;
    PRINTINTPAIR(x_dim,y_dim);
    COMMA;
    
    OPEN_BRACKET;
    for (i = 0; i < num_indices; i++) {
	PRINTINT(el_data->cell_array.colr[i]);
	ADD_COMMA_IF_NOT_LAST(i,num_indices - 1);
    }
    CLOSE_BRACKET;
}

static Print_status
print_linetype(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(linetypes[el_data->int_data]);
}

static Print_status
print_markertype(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(markertypes[el_data->int_data]);
}

static Print_status
print_txprec(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(txprec[el_data->int_data]);
}

static Print_status
print_vector(el_data)
register Pelem_data *el_data;
{
    PRINTXY(el_data->char_up_vec.delta_x,el_data->char_up_vec.delta_y);
}

static Print_status
print_txpath(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(txpath[el_data->int_data]);
}

static Print_status
print_txalign(el_data)
register Pelem_data *el_data;
{
    OPEN_BRACKET;
    PRINTNAME(txhor[el_data->text_align.hor]);
    COMMA;
    PRINTNAME(txver[el_data->text_align.vert]);
    CLOSE_BRACKET;
}

static Print_status
print_interstyle(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(interstyle[el_data->int_style]);
}

static Print_status
print_annotationstyle(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(annotationstyle[el_data->int_data]);
}

static Print_status
print_edgef(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(edgeflags[el_data->int_data]);
}

static Print_status
print_point(el_data)
register Pelem_data *el_data;
{
    PRINTXY(el_data->point.x,el_data->point.y);    
}

static Print_status
print_pat(el_data)
register Pelem_data *el_data;
{
    OPEN_BRACKET;
	PRINTXYZ(el_data->pat_ref_point_vecs.ref_point.x,
	     el_data->pat_ref_point_vecs.ref_point.y,
	     el_data->pat_ref_point_vecs.ref_point.z);
    CLOSE_BRACKET;
    COMMA;
	PRINTXYZ(el_data->pat_ref_point_vecs.ref_vec[0].delta_x,
	     el_data->pat_ref_point_vecs.ref_vec[0].delta_y,
	     el_data->pat_ref_point_vecs.ref_vec[0].delta_z);
    COMMA;
	PRINTXYZ(el_data->pat_ref_point_vecs.ref_vec[1].delta_x,
	     el_data->pat_ref_point_vecs.ref_vec[1].delta_y,
	     el_data->pat_ref_point_vecs.ref_vec[1].delta_z);
}

static Print_status
print_asf(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(attrid[el_data->asf.id]);
    COMMA;
    PRINTNAME(asf[el_data->asf.source]);
}

static Print_status
print_hlhsrid(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(hlhsrid[el_data->int_data]);
}

static Print_status
print_mtran3(el_data)
register Pelem_data *el_data;
{
    register long i, j;

    PRINTNAME(comptype[el_data->local_tran3.comp]);
    COMMA;

    OPEN_BRACKET;
    for (i = 0; i <= 3; i++) {
	OPEN_BRACKET;
	for (j = 0; j <= 3; j++) {
	    PRINTFLOAT(el_data->local_tran3.tran[j][i]);
	    ADD_COMMA_IF_NOT_LAST(j,3);  
	}
	CLOSE_BRACKET;
	ADD_COMMA_IF_NOT_LAST(i,3);  
    }
    CLOSE_BRACKET;
}

static Print_status
print_mtran(el_data)
register Pelem_data *el_data;
{
    register long i, j;

    PRINTNAME(comptype[el_data->local_tran.comp]);
    COMMA;

    OPEN_BRACKET;
    for (i = 0; i <= 2; i++) {
	OPEN_BRACKET;
	for (j = 0; j <= 2; j++) {
	    PRINTFLOAT(el_data->local_tran.tran[j][i]);
	    ADD_COMMA_IF_NOT_LAST(j,2);
	}
	CLOSE_BRACKET;
	ADD_COMMA_IF_NOT_LAST(i,2);
    }
    CLOSE_BRACKET;
}

static Print_status
print_matrix3(el_data)
register Pelem_data *el_data;
{
    register long i,j;

    for (j = 0; j <= 3; j++) {
	OPEN_BRACKET;
	for (i = 0; i <= 3; i++) {
	    PRINTFLOAT(el_data->global_tran3[j][i]);
	    ADD_COMMA_IF_NOT_LAST(i,3);
	}
	CLOSE_BRACKET;
	ADD_COMMA_IF_NOT_LAST(j,3);
    }
}

static Print_status
print_matrix(el_data)
register Pelem_data *el_data;
{
    register long i,j;

    for (j = 0; j <= 2; j++) {
	OPEN_BRACKET;
	for (i = 0; i <= 2; i++) {
	    PRINTFLOAT(el_data->global_tran[j][i]);
	    ADD_COMMA_IF_NOT_LAST(i,2);
	}
	CLOSE_BRACKET;
	ADD_COMMA_IF_NOT_LAST(j,2);
    }
}

static Print_status
print_mclip3(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint limit = el_data->model_clip3.half_spaces.num_half_spaces;

    PRINTNAME(operator[el_data->model_clip3.op]);
    COMMA;
    OPEN_BRACKET;
    PRINTINT(limit);
    COMMA;
    for (i = 0; i < limit; i++) {
	OPEN_BRACKET;
	PRINTXYZ(el_data->model_clip3.half_spaces.half_spaces[i].point.x,
	el_data->model_clip3.half_spaces.half_spaces[i].point.y,
	el_data->model_clip3.half_spaces.half_spaces[i].point.z);
	COMMA;
	PRINTXYZ(el_data->model_clip3.half_spaces.half_spaces[i].norm.delta_x,
	el_data->model_clip3.half_spaces.half_spaces[i].norm.delta_y,
	el_data->model_clip3.half_spaces.half_spaces[i].norm.delta_z);
	CLOSE_BRACKET;    
	ADD_COMMA_IF_NOT_LAST(i,limit);
    }
    CLOSE_BRACKET;
}

static Print_status
print_mclip(el_data)
register Pelem_data *el_data;
{
    register long i;
    register Pint limit = el_data->model_clip.half_spaces.num_half_spaces;

    PRINTNAME(operator[el_data->model_clip.op]);
    COMMA;
    OPEN_BRACKET;
    PRINTINT(limit);
    COMMA;
    for (i = 0; i < limit; i++) {
	OPEN_BRACKET;
	PRINTXY(el_data->model_clip.half_spaces.half_spaces[i].point.x,
	el_data->model_clip.half_spaces.half_spaces[i].point.y);
	COMMA;
	PRINTXY(el_data->model_clip.half_spaces.half_spaces[i].norm.delta_x,
	el_data->model_clip.half_spaces.half_spaces[i].norm.delta_y);
	CLOSE_BRACKET;    
	ADD_COMMA_IF_NOT_LAST(i,limit);
    }
    CLOSE_BRACKET;
}

static Print_status
print_clip(el_data)
register Pelem_data *el_data;
{
    PRINTNAME(clip[el_data->clip_ind]);
}

static Print_status
print_data(el_data)
register Pelem_data *el_data;
{
    register long i;

    OPEN_BRACKET;
	PRINTINT(el_data->appl_data.size);
	COMMA;
	OPEN_BRACKET;
	for (i = 0; i < el_data->appl_data.size; i++) {
	    fprintf(stderr,"%c",el_data->appl_data.data[i]);    
	    ADD_COMMA_IF_NOT_LAST(i,el_data->appl_data.size - 1);
	}
	CLOSE_BRACKET;
    CLOSE_BRACKET;
}


static Print_status
print_colour(el_data)
register Pelem_data *el_data;
{
    switch (el_data->colr.type) {
    case PINDIRECT:
	fprintf(stderr,	"{PINDIRECT,{%d}}", el_data->colr.val.ind);
	break;
    case PMODEL_RGB:
	fprintf(stderr,"{PMODEL_RGB,{%g, %g, %g}}", 
	el_data->colr.val.general.x,
	el_data->colr.val.general.y, 
	el_data->colr.val.general.z);
	break;
    case PMODEL_CIELUV:
    case PMODEL_HSV:
    case PMODEL_HLS:
    default:
	break;
    }
}


/*
 *    read the structure 
 */
void
read_struct(structlist)
register Pint_list *structlist;
{
    register long i;

    Pint elem_num;
    Pint error_ind;
    Pelem_type elem_type;
    Pint size;
    Pelem_data *elem_data;
    Pstore store;

    for (i = 0; i < structlist->num_ints; i++) {
	for (elem_num = 1;; elem_num++) {
	    pinq_elem_type_size(structlist->ints[i], elem_num,
	    &error_ind, &elem_type, &size);
	    if (error_ind) {
		(void) fprintf(stderr,
		"%d element(s) in structure %d\n",
		elem_num - 1, structlist->ints[i]);
		return;
	    }

	    pcreate_store (&error_ind, &store);
	    if (error_ind) { 
	      fprintf (stderr, "ERROR in creating storage for inquiry of structure elements\n");
	      exit (1);
	    }
	    pinq_elem_content(structlist->ints[i], elem_num, store,
		      &error_ind, &elem_data);

	    if (error_ind) break;

	    (void) fprintf(stderr, "%s(",oc_names[elem_type]);
	    print_func[elem_type](elem_data);
	    (void) fprintf(stderr,");\n");
	}
    }
}
