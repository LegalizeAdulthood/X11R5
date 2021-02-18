/* $XConsortium: sofa.c,v 5.1 91/02/16 09:32:29 rws Exp $ */

/***********************************************************
Copyright (c) 1991 by Sun Microsystems, Inc. and the X Consortium.

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

#include "phigs/phigs.h"

main()
{

    static Ppoint3 sofa_data[] = {0.2, 0.2, 0.0, 	/* 0 */
                                  0.4, 0.2, 0.0,	/* 1 */
                                  0.6, 0.2, 0.0,	/* 2 */
                                  0.8, 0.2, 0.0,	/* 3 */
                                  0.2, 0.4, 0.0,	/* 4 */
                                  0.4, 0.4, 0.0,	/* 5 */
                                  0.6, 0.4, 0.0,	/* 6 */
                                  0.8, 0.4, 0.0,	/* 7 */
                                  0.2, 0.6, 0.0,	/* 8 */
                                  0.4, 0.6, 0.0,	/* 9 */
                                  0.6, 0.6, 0.0,	/*10 */
                                  0.8, 0.6, 0.0,	/*11 */
                                  0.2, 0.8, 0.0,	/*12 */
                                  0.4, 0.8, 0.0,	/*13 */
                                  0.6, 0.8, 0.0,	/*14 */
                                  0.8, 0.8, 0.0 };	/*15 */
    Pint            fflag = PFACET_NONE; /* data specified per facet */
    Pint            eflag = PEDGE_NONE; /* data specified per edge */
    Pint            vflag = PVERT_COORD; /* data specified per vertex */
    Pint            colour_model = PINDIRECT; /* colour model */
    Pint            num_sets = 5;             /* number of fill area sets */
    Pfacet_data_arr3     *fdata;         /* facet data */
    Pedge_data_list_list *edata;         /* array of L(L(E)) edge data */
    Pint_list_list       vlistlist[5];  /* array of L(L(I)) vertex indices */
    Pint_list		 vlist[5];		
    static Pint		 v0[]= { 0,  1,  5,  4 };
    static Pint		 v1[]= { 2,  3,  7,  6 };
    static Pint		 v2[]= { 5,  6, 10,  9 };
    static Pint		 v3[]= { 8,  9, 13, 12 };
    static Pint		 v4[]= {10, 11, 15, 14 };
    Pfacet_vdata_list3   vdata;         /* facet vertex data */

    static Pview_rep viewrep = {
		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{1.0,0.0,0.0},
		{0.0,1.0,0.0},
		{0.0,0.0,1.0},

		{0.75,1.0,0.75,1.0},
		PIND_CLIP
	};
        

    Ppoint3 p[5];
    Pvec3 xlv;
    Pmatrix3 mx;
    Pvec3 scale;
    Pvec3 shift;
    Pint err;
    Pfloat xang, yang, zang;

    popen_phigs((char *)NULL,PDEF_MEM_SIZE);
    popen_ws(1,(char *)NULL,phigs_ws_type_x_tool);
    pset_view_rep(1,1,&viewrep);
    popen_struct(1);

    fdata = NULL;
    edata = NULL;


    /*
     * first quadrant - hollow
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = -0.25; shift.delta_y = -0.25; shift.delta_z = 0;
    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);
    pset_local_tran3(mx, PTYPE_REPLACE);

    pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				 num_sets,
				 fdata, edata, vlistlist, &vdata );

    /*
     * second quadrant - solid
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = 0.25; shift.delta_y = -0.25; shift.delta_z = 0;
    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);
    pset_local_tran3(mx, PTYPE_REPLACE);

    pset_int_colr_ind(2);
    pset_int_style(PSTYLE_SOLID);
    pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				 num_sets,
				 fdata, edata, vlistlist, &vdata );

    /*
     * third quadrant - edges
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = -0.25; shift.delta_y = 0.25; shift.delta_z = 0;
    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);
    pset_local_tran3(mx, PTYPE_REPLACE);

    pset_edgetype(PSTYLE_SOLID);
    pset_edge_colr_ind(3);
    pset_edgewidth(2.0);
    pset_edge_flag(PEDGE_ON);
    pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				 num_sets,
				 fdata, edata, vlistlist, &vdata );

    /*
     * fourth quadrant - clipped
     */
    vlistlist[0].num_lists = 1; vlistlist[0].lists = &vlist[0];
    vlist[0].num_ints = 4;   vlist[0].ints = v0;
    vlistlist[1].num_lists = 1; vlistlist[1].lists = &vlist[1];
    vlist[1].num_ints = 4;   vlist[1].ints = v1;
    vlistlist[2].num_lists = 1; vlistlist[2].lists = &vlist[2];
    vlist[2].num_ints = 4;   vlist[2].ints = v2;
    vlistlist[3].num_lists = 1; vlistlist[3].lists = &vlist[3];
    vlist[3].num_ints = 4;   vlist[3].ints = v3;
    vlistlist[4].num_lists = 1; vlistlist[4].lists = &vlist[4];
    vlist[4].num_ints = 4;   vlist[4].ints = v4;

    vdata.num_vertices = 16; vdata.vertex_data.points = sofa_data;

    xlv.delta_x = 0.5; xlv.delta_y = 0.5; xlv.delta_z = 0.0;
    scale.delta_x = 0.5; scale.delta_y = 0.5; scale.delta_z = 1.0;         
    xang = 0.0; yang = 0.0; zang = 0.0; 
    shift.delta_x = 0.25; shift.delta_y = 0.25; shift.delta_z = 0;
    pbuild_tran_matrix3( &xlv, &shift, xang, yang, zang,
			 &scale, &err, mx);
    pset_local_tran3(mx, PTYPE_REPLACE);

    pset_view_rep(1,1,&viewrep);
    pset_view_ind(1);
    pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, 
				 num_sets,
				 fdata, edata, vlistlist, &vdata );

    ppost_struct(1,1,1.0);
    printf("should now have a SOFAS...");
    getchar();

}
