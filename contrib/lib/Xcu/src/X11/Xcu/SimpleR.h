/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

/*
 * The ifndef is intentionally not here!!!!
 * This include file must be included more than once!!!
 */

/* Simple */
  {XtNcursor, XtCCursor, XtRCursor, sizeof(Cursor),
     offset(cursor), XtRCursor, (caddr_t)&defaultCursor},
  {XtNinsensitiveBorder, XtCInsensitive, XtRPixmap, sizeof(Pixmap),
     offset(insensitive_border), XtRPixmap, (caddr_t)&defaultPixmap},
/* Facet */
    {XtNdimFacetColor, XtCDimFacetColor, XtRPixel, 
        sizeof(Pixel), offset(dim_facet_color),
        XtRString,(caddr_t)"Black"},
    {XtNbrightFacetColor, XtCBrightFacetColor, XtRPixel, 
        sizeof(Pixel), offset(bright_facet_color),
        XtRString,(caddr_t)"White"},
    {XtNhighlightColor, XtCHighlightColor, XtRPixel, 
        sizeof(Pixel), offset(highlight_color),
        XtRString,(caddr_t)"White"},
    {XtNhighlightWidth, XtCHighlightWidth, XtRDimension, sizeof(Dimension),
	offset(highlight_width), XtRDimension, (caddr_t)&defHighlightWidth},
    {XtNfacetWidth, XtCFacetWidth, XtRDimension, sizeof(Dimension),
	offset(facet_width), XtRDimension, (caddr_t)&defFacetWidth},
    {XtNcursorForeground, XtCCursorForeground, XtRPixel, sizeof(Pixel),
	offset(cursor_foreground), XtRPixel, (caddr_t)&def_bp},
    {XtNcursorBackground, XtCCursorBackground, XtRPixel, sizeof(Pixel),
	offset(cursor_background), XtRPixel, (caddr_t)&def_wp},
/* Shadow */
    {XtNshadow, XtCShadow, XtRBoolean, sizeof(Boolean),
	offset(shadow), XtRBoolean, (caddr_t)&def_false},
    {XtNsunDirection, XtCSunDirection, XtRSunDirection, sizeof(XcuSunType), 
	offset(sun_direction), XtRSunDirection, (caddr_t)&def_sun_direction},
    {XtNfirstExpose, XtCCallback, XtRCallback, sizeof(caddr_t), 
	offset(firstExposeCallbacks), XtRCallback, (caddr_t)NULL},
    {XtNresize, XtCCallback, XtRCallback, sizeof(caddr_t), 
	offset(resizeCallbacks), XtRCallback, (caddr_t)NULL}

