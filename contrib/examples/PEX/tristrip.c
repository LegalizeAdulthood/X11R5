/* $XConsortium: tristrip.c,v 5.1 91/02/16 09:32:34 rws Exp $ */

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

/* a simple example of a triangle strip */

#include "phigs/phigs.h"

main()
{

        static Ppoint3 tri_data[] = {0.1, 0.3, 0.0, 
                                    0.2, 0.7, 0.0,
                                    0.3, 0.3, 0.0,
                                    0.4, 0.7, 0.0,
                                    0.5, 0.3, 0.0,
                                    0.6, 0.7, 0.0,
                                    0.7, 0.3, 0.0,
                                    0.8, 0.7, 0.0,
                                    0.9, 0.3, 0.0 };
        Pint            fflag = PFACET_NONE; /* what data is specified per facet */
        Pint            vflag = PVERT_COORD; /* what data is specified per vertex */
        Pint            colour_model = PINDIRECT;       /* colour model */
        Pint            nv = 9;                         /* number of vertices */
        Pfacet_vdata_arr3 vdata;              /* facet vertex data */
        

	vdata.points = tri_data;
        popen_phigs((char *)NULL,PDEF_MEM_SIZE);
        popen_ws(1,(char *)NULL,phigs_ws_type_x_tool);
        popen_struct(1);
        ppost_struct(1,1,1.0);
        ptri_strip3_data(fflag, vflag, colour_model, nv,
	    (Pfacet_vdata_arr3 *)NULL, &vdata);
        printf("a hollow triangle strip. press return...");
        getchar();
}

