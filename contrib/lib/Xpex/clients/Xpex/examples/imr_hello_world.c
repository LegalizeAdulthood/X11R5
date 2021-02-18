/* $Header: imr_hello_world.c,v 1.10 91/09/11 17:39:34 sinyaw Exp $ */

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
#include <math.h>
#include "Xpexlib.h"
#include "Xpexutil.h"
#include "window.h"
#include "pex_hello_world.h"

static Display *dpy;
static Drawable drawable;
static XpexPC pc;
static XpexRenderer rdr;
static XpexPCValues pc_values;
static XpexRendererAttributes attrs;

extern int counter;
extern int duration;

static void
DrawImage(counter)
    int *counter;
{
	register float i;

	static XpexMatrix matrix_a = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};
	static XpexMatrix matrix_b = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 },
		{ 0.0, 0.0, 0.0, 1.0 }
	};
	XpexMatrix composed_matrix;

	static XpexCoord3D origin = { 0.0, 0.0, 0.0 };
	static XpexVector3D vector1 = { 1.0, 0.0, 0.0 };
	static XpexVector3D vector2 = { 0.0, 1.0, 0.0 };
	static XpexString message = "Sony PEX Library";

	/* target output commands to this renderer */
	XpexRenderOutputCmds(dpy, rdr); 

	for (i = 0.; i < 360.; i += 1., (*counter)++) {
		XClearWindow(dpy,drawable);
		XpexBeginRendering(dpy, rdr, drawable);
			XpexRotateX(DEG_TO_RAD(i*7), matrix_a);
			XpexRotateZ(DEG_TO_RAD(i*5), matrix_b);
			XpexComposeMatrix(matrix_a, matrix_b, composed_matrix);
			XpexSetLocalTransform(dpy, Xpex_Replace, composed_matrix);
			XpexText(dpy, &origin, &vector1, &vector2,
			message, strlen(message));
			XpexEndRendering(dpy, rdr, False);
	}
}

void static 
stop_proc(counter)
    caddr_t counter;
{
    (void) printf("%d frame(s) posted\n", *(int *) counter);
    XCloseDisplay(dpy);
    exit(0);
}

static void
SetViewTableEntry()
{
	extern void InitViewEntry();

	XpexViewEntry view_entry;

	InitViewEntry(&view_entry);

	XpexSetTableEntries(dpy, attrs.view_table, 
	Xpex_ViewLUT, 1, 1, (char *)&view_entry, sizeof(XpexViewEntry));
}

static void
InitPEXresources()
{
	XpexBitmask attrMask = 
		XpexRD_ColorApproxTable | 
		XpexRD_ViewTable |
		XpexRD_PipelineContext | 
		XpexRD_ColorTable;

	XpexPCBitmask pcValueMask;

	pcValueMask[0] = 
		XpexPC_TextColor | 
		XpexPC_CharHeight | 
		XpexPC_TextAlignment;

	pcValueMask[1] = XpexPC_ViewIndex;

	pcValueMask[2] = 0x0;

	pc_values.view_index = 1;
	pc_values.text_color.color_type = Xpex_IndexedColor;
	pc_values.text_color.value.index = MAGENTA;
	pc_values.char_height = 1.0;
	pc_values.text_alignment.vertical = XpexValign_Half;
	pc_values.text_alignment.horizontal = XpexHalign_Center;

	attrs.pc_id = XpexCreatePC(dpy, pcValueMask, &pc_values);

	attrs.color_table = 
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorLUT);

	attrs.view_table = 
	XpexCreateLookupTable(dpy, drawable, Xpex_ViewLUT);

	attrs.color_approx_table = 
	XpexCreateLookupTable(dpy, drawable, Xpex_ColorApproxLUT);

	rdr = XpexCreateRenderer(dpy, drawable, attrMask, &attrs);

	XpexSetupColormap(dpy, drawable, attrs.color_approx_table);

	SetViewTableEntry();
}

static void
InitXresources()
{
	SetWindowLabel("pex_hello_world - immediate mode");
	drawable = CreateWindow(dpy);
}

static void
Initialize()
{
	InitXresources();
	InitPEXresources();
}

imr_hello_world(display)
	Display *display;
{
    static void stop_proc();

	dpy = display;

	Initialize();

	if (duration > 0) {
	    start_clock(duration, stop_proc, &counter);
    }
    for (counter = 0;;) {
		DrawImage(&counter);
	}
    /* not reached */
}
