/* $Header: dpy_imp_dep.c,v 1.1 91/09/18 13:50:04 sinyaw Exp $ */

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
#include "Xpexutil.h"

static Drawable drawableExample;
static int screen;

void
dpy_imp_dep(dpy)
	Display *dpy;
{
	static char *idc_msg_str[34] = {
		NULL,
		"\tDitheringSupported: %d\n",
		"\tMaxEdgeWidth: %d\n",
		"\tMaxLineWidth: %d\n",
		"\tMaxMarkerSize: %d\n",
		"\tMaxModelClipPlanes: %d\n",
		"\tMaxNamesetNames: %d\n",
		"\tMaxNonAmbientLights: %d\n",
		"\tMaxNURBOrder: %d\n",
		"\tMaxTrimCurveOrder: %d\n",
		"\tMinEdgeWidth: %d\n",
		"\tMinLineWidth: %d\n",
		"\tMinMarkerSize: %d\n",
		"\tNominalEdgeWidth: %d\n",
		"\tNominalLineWidth: %d\n",
		"\tNominalMarkerSize: %d\n",
		"\tNumSupportedEdgeWidths: %d\n",
		"\tNumSupportedLineWidths: %d\n",
		"\tNumSupportedMarkerSizes: %d\n",
		"\tBestColorApproxValues: %d\n",
		"\tTransparencySupported: %d\n",
		"\tDoubleBufferSupported: %d\n",
		"\tChromaticityRedU: %.3f\n",
		"\tChromaticityRedV: %.3f\n",
		"\tLuminanceRed: %.3f\n",
		"\tChromaticityGreenU: %.3f\n",
		"\tChromaticityGreenU: %.3f\n",
		"\tLuminanceGreen: %.3f\n",
		"\tChromacityBlueU: %.3f\n",
		"\tChromacityBlueV: %.3f\n",
		"\tLuminanceBlue: %.3f\n",
		"\tChromacityWhiteU: %.3f\n",
		"\tChromacityWhiteV: %.3f\n",
		"\tLuminanceWhite: %.3f\n",
	};
	XpexAllImpDepConstants idc;

	screen = DefaultScreen(dpy);
	drawableExample = RootWindow(dpy,screen);

	XpexGetAllImpDepConstants(dpy, drawableExample, &idc);

	(void) fprintf(stderr, 
	"\nImplementation-Dependent Constants\n\n");

	(void) fprintf(stderr, idc_msg_str[XpexID_NominalLineWidth],
	idc.nominalLineWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_NumSupportedLineWidths],
	idc.numSupportedLineWidths);

	(void) fprintf(stderr, idc_msg_str[XpexID_MinLineWidth],
	idc.minLineWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxLineWidth],
	idc.maxLineWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_NominalEdgeWidth],
	idc.nominalEdgeWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_NumSupportedEdgeWidths],
	idc.numSupportedEdgeWidths);

	(void) fprintf(stderr, idc_msg_str[XpexID_MinEdgeWidth],
	idc.minEdgeWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxEdgeWidth],
	idc.maxEdgeWidth);

	(void) fprintf(stderr, idc_msg_str[XpexID_NominalMarkerSize],
	idc.nominalMarkerSize);

	(void) fprintf(stderr, idc_msg_str[XpexID_NumSupportedMarkerSizes],
	idc.numSupportedMarkerSizes);

	(void) fprintf(stderr, idc_msg_str[XpexID_MinMarkerSize],
	idc.minMarkerSize);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxMarkerSize],
	idc.maxMarkerSize);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityRedU],
	idc.chromaticityRedU);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityRedV],
	idc.chromaticityRedV);

	(void) fprintf(stderr, idc_msg_str[XpexID_LuminanceRed],
	idc.luminanceRed);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityGreenU],
	idc.chromaticityGreenU);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityGreenV],
	idc.chromaticityGreenV);

	(void) fprintf(stderr, idc_msg_str[XpexID_LuminanceGreen],
	idc.luminanceGreen);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityBlueU],
	idc.chromaticityBlueU);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityBlueV],
	idc.chromaticityBlueV);

	(void) fprintf(stderr, idc_msg_str[XpexID_LuminanceBlue],
	idc.luminanceBlue);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityWhiteU],
	idc.chromaticityWhiteU);

	(void) fprintf(stderr, idc_msg_str[XpexID_ChromaticityWhiteV],
	idc.chromaticityWhiteV);

	(void) fprintf(stderr, idc_msg_str[XpexID_LuminanceWhite],
	idc.luminanceWhite);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxNameSetNames],
	idc.maxNamesetNames);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxModelClipPlanes],
	idc.maxModelClipPlanes);

	(void) fprintf(stderr, idc_msg_str[XpexID_TransparencySupported],
	idc.transparencySupported);

	(void) fprintf(stderr, idc_msg_str[XpexID_DitheringSupported],
	idc.ditheringSupported);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxNonAmbientLights],
	idc.maxNonAmbientLights);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxNURBOrder],
	idc.maxNurbOrder);

	(void) fprintf(stderr, idc_msg_str[XpexID_MaxTrimCurveOrder],
	idc.maxTrimCurveOrder);

	(void) fprintf(stderr, idc_msg_str[XpexID_BestColourApproximation],
	idc.bestColorApproxValues);

	(void) fprintf(stderr, idc_msg_str[XpexID_DoubleBufferingSupported],
	idc.doubleBufferingSupported);
}
