/* $XConsortium: cell_array.c,v 5.1 91/02/16 09:32:16 rws Exp $ */

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

#define ROWS   5 
#define COLUMNS 7 

main(argc,argv)
        int argc; char  ** argv;
{
        Pint            err;
	Ppat_rep	color_grid; 
 
        static Pparal 		outline =
					{0.1, 0.1, 0.1,
					 0.7, 0.2, 0.1,
					 0.1, 0.5, 0.1};  

	color_grid.dims.size_x = COLUMNS; 
	color_grid.dims.size_y = ROWS; 

        color_grid.colr_array = (Pint *)malloc(ROWS*COLUMNS*sizeof(Pint));
 
        popen_phigs(NULL,0);
        popen_ws(1,NULL,phigs_ws_type_x_tool);
        popen_struct(1);
        ppost_struct(1,1,1.0);
        pcell_array3(&outline,&color_grid);
        printf("cell array!");
        getchar();
}

