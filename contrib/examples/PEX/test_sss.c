/* $XConsortium: test_sss.c,v 5.2 91/02/16 09:32:36 rws Exp $ */

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

#include "structdefs.h"

extern void create_struct();
extern void read_struct();

/*
 *    Create and read back the structure
 */
static void
create_and_read_structs()
{
    static Pint elem_id[] = {
	PELEM_POLYLINE3,
	PELEM_POLYLINE,
	PELEM_POLYMARKER3,
	PELEM_POLYMARKER,
	PELEM_TEXT3,
	PELEM_TEXT,
	PELEM_ANNO_TEXT_REL3, 
	PELEM_ANNO_TEXT_REL, 
	PELEM_FILL_AREA3,
	PELEM_FILL_AREA,
	PELEM_FILL_AREA_SET3, 
	PELEM_FILL_AREA_SET,
	PELEM_CELL_ARRAY3,
	PELEM_CELL_ARRAY,
	PELEM_LINE_IND,
	PELEM_MARKER_IND,
	PELEM_TEXT_IND,
	PELEM_INT_IND,
	PELEM_EDGE_IND,
	PELEM_LINETYPE,
	PELEM_LINEWIDTH,
	PELEM_LINE_COLR_IND,
	PELEM_MARKER_TYPE,
	PELEM_MARKER_SIZE,
	PELEM_MARKER_COLR_IND,
	PELEM_TEXT_FONT,
	PELEM_TEXT_PREC,
	PELEM_CHAR_EXPAN,
	PELEM_CHAR_SPACE,
	PELEM_TEXT_COLR_IND,
	PELEM_CHAR_HT,
	PELEM_CHAR_UP_VEC,
	PELEM_TEXT_PATH,
	PELEM_TEXT_ALIGN,  
	PELEM_ANNO_CHAR_HT,
	PELEM_ANNO_CHAR_UP_VEC,
	PELEM_ANNO_PATH,
	PELEM_ANNO_ALIGN, 
	PELEM_ANNO_STYLE,
	PELEM_INT_STYLE,
	PELEM_INT_STYLE_IND,
	PELEM_INT_COLR_IND,
	PELEM_EDGE_FLAG,
	PELEM_EDGETYPE,
	PELEM_EDGEWIDTH,
	PELEM_EDGE_COLR_IND,
	PELEM_PAT_SIZE,
	PELEM_PAT_REF_POINT_VECS, 
	PELEM_PAT_REF_POINT,
	PELEM_ADD_NAMES_SET, 
	PELEM_REMOVE_NAMES_SET, 
	PELEM_INDIV_ASF,
	PELEM_HLHSR_ID,
	PELEM_LOCAL_MODEL_TRAN3, 
	PELEM_LOCAL_MODEL_TRAN, 
	PELEM_GLOBAL_MODEL_TRAN3, 
	PELEM_GLOBAL_MODEL_TRAN, 
	PELEM_MODEL_CLIP_VOL3,  
	PELEM_MODEL_CLIP_VOL, 
	PELEM_MODEL_CLIP_IND, 
	PELEM_RESTORE_MODEL_CLIP_VOL, 
	PELEM_VIEW_IND,
	PELEM_EXEC_STRUCT,
	PELEM_LABEL,
	PELEM_APPL_DATA,
	PELEM_PICK_ID 
    };

    Pint struct_id;
    Pint_list structlist;
    Pint_list elemlist;
    Pint type;

    struct_id = 1;
    structlist.num_ints = 1;
    structlist.ints = &struct_id;

    type = SPECIFIED_ELEMENTS;
    elemlist.num_ints = 66;
    elemlist.ints = &elem_id[0];
    create_struct(&structlist, &elemlist, type);
    free(elemlist.ints);
/*
 *  read the contents of the structure and show the contents 
 *  by printing them out on the display.
 */
    read_struct(&structlist);
}

static void
init_phigs()
{
    popen_phigs((char *)NULL, PDEF_MEM_SIZE);
}

static void
init()
{
    init_phigs();
}

main()
{
    init();
    create_and_read_structs();
}
