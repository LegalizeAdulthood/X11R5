/* $Header: dpy_sc_info.c,v 1.1 91/09/18 13:50:06 sinyaw Exp $ */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include <stdio.h>
#include "Xpexlib.h"
#include "PEX_names.h"

dpy_sc_info(dpy) 
	Display *dpy;
{
	XpexSC sc_id;
	XpexBitmask valueMask;
	XpexSCValues values, valuesRet;

	valueMask = 0x0;

	sc_id = XpexCreateSC(dpy, valueMask, &values);

	XpexGetSC(dpy, sc_id, XpexSC_Position, &valuesRet);

	(void) fprintf(stderr, 
		"%s: (%.1f, %.1f, %.1f)\n",
		NameOfSCAttribute(XpexSC_Position),
		valuesRet.search_pos.x, 
		valuesRet.search_pos.y, 
		valuesRet.search_pos.z);

	XpexGetSC(dpy, sc_id, XpexSC_Distance, &valuesRet);

	(void) fprintf(stderr, "%s: %.1f\n",
		NameOfSCAttribute(XpexSC_Distance),
		valuesRet.search_dist);

	XpexGetSC(dpy, sc_id, XpexSC_Ceiling, &valuesRet);

	(void) fprintf(stderr, "%s: %d\n",
		NameOfSCAttribute(XpexSC_Ceiling),
		valuesRet.search_ceiling);

	XpexGetSC(dpy, sc_id, XpexSC_ModelClipFlag, &valuesRet);

	(void) fprintf(stderr, "%s: %s(%d)\n",
		NameOfSCAttribute(XpexSC_ModelClipFlag),
		valuesRet.model_clip_flag ? "True" : "False",
		valuesRet.model_clip_flag);

	XpexGetSC(dpy, sc_id, XpexSC_StartPath, &valuesRet);

	(void) fprintf(stderr, "%s: %d\n",
		NameOfSCAttribute(XpexSC_StartPath),
		valuesRet.start_path.num_refs);

	XpexGetSC(dpy, sc_id, XpexSC_NormalList, &valuesRet);

	(void) fprintf(stderr, "%s: %d\n",
		NameOfSCAttribute(XpexSC_NormalList),
		valuesRet.normal_list.num_pairs);

	XpexGetSC(dpy, sc_id, XpexSC_InvertedList, &valuesRet);

	(void) fprintf(stderr, "%s: %d\n",
		NameOfSCAttribute(XpexSC_InvertedList),
		valuesRet.inverted_list.num_pairs);

	XpexFreeSC(dpy, sc_id);
}
