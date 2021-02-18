/* $Header: Xpexutil.h,v 2.2 91/09/11 16:05:58 sinyaw Exp $ */
/*
 * Xpexutil.h
 */
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

#ifndef XPEXUTIL_H_INCLUDED
#define XPEXUTIL_H_INCLUDED

#define Xpex_InvalidWindow  151
#define Xpex_InvalidViewport  152
#define Xpex_ProjViewportOutOfNpcRange  155
#define Xpex_ProjViewportIsZero  158
#define Xpex_ViewPlaneNormalIsZero  159
#define Xpex_ViewUpVectorIsZero  160
#define Xpex_ViewUpVectorAndViewPlaneNormalAreParallel  161
#define Xpex_PrpIsBwtnTheFrontAndBackPlanes  162
#define Xpex_PrpCantBePositionedOnTheViewPlane  163
#define Xpex_BackPlaneIsInFrontOfFrontPlane  164
#define Xpex_DegenerateFillArea  505

#define XpexCreateLineBundleLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_LineBundleLUT)

#define XpexCreateMarkerBundleLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_MarkerBundleLUT)

#define XpexCreateTextBundleLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_TextBundleLUT)

#define XpexCreateInteriorBundleLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_InteriorBundleLUT)

#define XpexCreateEdgeBundleLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_EdgeBundleLUT)

#define XpexCreatePatternLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_PatternLUT)

#define XpexCreateTextFontLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_TextFontLUT)

#define XpexCreateColorLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_ColorLUT)

#define XpexCreateLightLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_LightLUT)

#define XpexCreateDepthCueLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_DepthCueLUT)

#define XpexCreateColorApproxLUT(_dpy,_drawable) \
XpexCreateLookupTable(_dpy,_drawable,Xpex_ColorApproxLUT)

typedef enum {
	Xpex_PARALLEL,
	Xpex_PERSPECTIVE
} XpexProjectionType;

#endif /* XPEXUTIL_H_INCLUDED */
