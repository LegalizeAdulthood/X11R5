/* $Header: dpy_enum_types.c,v 1.1 91/09/18 13:50:02 sinyaw Exp $ */

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
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "Xpexlib.h"

void
dpy_enum_types(dpy)
	Display *dpy;
{
	static char *et_msg_str[27] = {
		"MarkerType(s)",
		"AtextStyle(s)",
		"InteriorStyle(s)",
		"HatchStyle(s)",
		"LineType(s)",
		"SurfaceEdgeTypes",
		"PickDeviceTypes", 
		"PolylineInterpMethod(s)",
		"CurveApproxMethod(s)",
		"ReflectionModel(s)",
		"SurfaceInterpMethod(s)",
		"SurfaceApproxMethod(s)",
		"ModelClipOperator(s)",
		"LightType(s)",
		"ColorType(s)",
		"FloatFormat(s)",
		"HlhsrMode(s)",
		"PromptEchoType(s)",
		"DisplayUpdateMode(s)",
		"ColorApproxType(s)",
		"ColorApproxModel(s)",
		"GDP(s)",
		"GDP3(s)",
		"GSE(s)",
		"TrimCurveApproxMethod(s)",
		"RenderingColourModel(s)",
		"ParaSurfCharacteristic(s)"
	};
	static short enum_types[27] = {
		XpexET_MarkerType,
		XpexET_ATextStyle,
		XpexET_InteriorStyle,
		XpexET_HatchStyle,
		XpexET_LineType,
		XpexET_SurfaceEdgeType,
		XpexET_PickDeviceType,
		XpexET_PolylineInterpMethod,
		XpexET_CurveApproxMethod,
		XpexET_ReflectionModel,
		XpexET_SurfaceInterpMethod,
		XpexET_SurfaceApproxMethod,
		XpexET_ModelClipOperator,
		XpexET_LightType,
		XpexET_ColorType,
		XpexET_FloatFormat,
		XpexET_HlhsrMode,
		XpexET_PromptEchoType,
		XpexET_DisplayUpdateMode,
		XpexET_ColorApproxType,
		XpexET_ColorApproxModel,
		XpexET_GDP,
		XpexET_GDP3,
		XpexET_GSE,
		XpexET_TrimCurveApproxMethod,
		XpexET_RenderingColourModel,
		XpexET_ParaSurfCharacteristics
	};
	XpexBitmask returnFormat = 0x0;
	XpexListOfEnumTypeDesc *info_list;
	int actual_count;
	register long i, j;

	int screen = DefaultScreen(dpy);
	Drawable drawableExample = RootWindow(dpy,screen);

	returnFormat = 0x3;
	XpexGetEnumTypeInfo(dpy, drawableExample, 
	enum_types, 27, returnFormat, &info_list, &actual_count);

	(void) fprintf(stderr, "Enumerated Type Information\n\n");
	(void) fprintf(stderr, "\t%d Enumerated Type(s)\n\n", actual_count);

	for (j = 0; j < actual_count; j++) {
		(void) fprintf(stderr, "\tNumber of Supported %s: %d\n\n",
			et_msg_str[j], info_list[j].count);
		for (i = 0; i < info_list[j].count; i++) {
			(void) fprintf(stderr, "\t\t%s(%d)\n",
				info_list[j].desc[i].name, info_list[j].desc[i].index);
		}
		(void) fprintf(stderr, "\n");
	}
}
