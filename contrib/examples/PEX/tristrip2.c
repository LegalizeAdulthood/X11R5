/* $XConsortium: tristrip2.c,v 5.1 91/02/16 09:32:36 rws Exp $ */

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

#ifdef BWEE
The following program draws some triangle strips with reflectange equation
setting = PREFL_AMBIENT.  It is expected that the triangle strips be rendered
with the current interior colour since the current light source state is empty
This program originated from a bug report created by the InsPEX test group.
#endif


#include "phigs/phigs.h"

main(argc,argv)
        int argc; char **argv;
{

static Ppoint3 pts1[] = {0.3,0.6,0.0, 0.5,0.5,0.5, 0.7,0.6,0.0, 0.7,0.4,0.0};
static Ppoint3 pts2[] = {0.3,0.6,0.0, 0.5,0.5,0.5, 0.3,0.4,0.0, 0.7,0.4,0.0};

Pfacet_vdata_arr3 vdata1;
Pfacet_vdata_arr3 vdata2;

static Ppoint3 farea[]= {0.2,0.9,0.0, 0.1,0.7,0.0, 0.0,0.8,0.0};
Ppoint_list3 fset;
 
vdata1.points = pts1;
vdata2.points = pts2;
fset.num_points = 3;
fset.points = farea;

popen_phigs(NULL,0);
popen_ws(1,NULL,phigs_ws_type_x_tool);
popen_struct(1);

pset_int_style(PSTYLE_SOLID);
pset_int_colr_ind(3);
pset_int_shad_meth(PSD_NONE);

pset_refl_eqn(PREFL_AMBIENT);

if (argc == 1) {
	ptri_strip3_data(PFACET_NONE,PVERT_COORD,PMODEL_RGB,4,NULL,&vdata1);
	ptri_strip3_data(PFACET_NONE,PVERT_COORD,PMODEL_RGB,4,NULL,&vdata2);
} else {
        pfill_area_set3(1,&fset); 
}

ppost_struct(1,1,1.0);

if (argc == 1) {
	printf("a triangle strip. press return...");
} else {
	printf("a fill area set with the same attributes. press return...");
}
getchar();

}


