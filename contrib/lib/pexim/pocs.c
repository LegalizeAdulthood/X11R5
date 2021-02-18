#ifdef SCCS
static char sccsid[]="@(#)pocs.c	1.18 Stardent 91/07/24";
#endif
/*
 *			Copyright (c) 1991 by
 *			Stardent Computer Inc.
 *			All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Stardent Computer not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. Stardent Computer
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 */

/* Fixed accoding to and verified against phigs.c_bind 5.1 13-MAR-91     */
/*  a long, long time ago in a galaxy far, far away, this was based upon */
/*      @(#)phigs.c_bind 4.5 90/12/08 SMI      */

/***********************************************************
Copyright 1989,1990 by Sun Microsystems, Inc. and the X Consortium.

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

/*
 * The following PEXOC's are not generated in this file.
 * #define PEXOCBfSurfaceColourIndex	 38
 * #define PEXOCExtFillArea		 91
 * They are not part of PHIGS!  All other OCs are here.
 */

#include <stdio.h>
#include "pexim.h"
#include "PEX.h"
#include "PEXproto.h"
#include "peximint.h"

/* not possible outside of source build environment - use Xlibint.h
 * #include "include/Xlibos.h" !* for Lock/UnlockDisplay *!
 */
#include "Xlibint.h"

#define NLEFT (pPEXIMCurrDpy->bufmax - pPEXIMCurrDpy->bufptr)
#define min(a,b) ((a)>(b)?(b):(a))

/* put back at 98 */
void
ptri_strip3_data(fflag, vflag, colour_model, nv, fdata, vdata)
Pint           	fflag;      	/* what data is specified per facet */
Pint           	vflag;          /* what data is specified per vertex */
Pint           	colour_model;   /* colour model */
Pint		nv;		/* number of vertices */
Pfacet_data_arr3  *fdata;     	/* facet data */
Pfacet_vdata_arr3 *vdata;       /* facet vertex data */
{
 pexTriangleStrip *ocptr;
 int size;
 int iColor, facetSize, vertexSize;
 char *vsrcptr, *fsrcptr;
 int num_vert, s1, s2, mustFlush;

 PEXIMDebug("ptri_strip3_data called  - PEXOCTriangleStrip \n");

 iColor = (colour_model==PINDIRECT)?1:0;
 /* From encoding spec 4+((p-2)*d +p*v)/4 */

 if (fflag != 0) fsrcptr = (char *)fdata->colrs;
 vsrcptr = (char *)vdata->points;
 num_vert = nv;

 LockDisplay(pPEXIMCurrDpy);

 while (num_vert > 0) {
   facetSize = (nv-2)*pPEXIMFacetSizes[iColor][fflag];
   vertexSize = nv*pPEXIMVertexSizes[iColor][vflag];
   size = facetSize + vertexSize;
   if ((size +sizeof(pexTriangleStrip) +sizeof(pexRenderOutputCommandsReq))>>2
       > pPEXIMCurrDpy->max_request_size) {
     /*
      * size = (nv-2)*per facet size + nv * per vertex size + header
      * below:  size avail / per vertex + per facet ( -2 for good measure )
      */
     nv = pPEXIMCurrDpy->max_request_size;
     nv = ((nv<<2) - sizeof(pexTriangleStrip)
	   - sizeof(pexRenderOutputCommandsReq))
       /
	 (pPEXIMFacetSizes[iColor][fflag]+pPEXIMVertexSizes[iColor][vflag])-2;
     if (num_vert - nv < 3) nv = num_vert - 3;
     num_vert -= (nv-2);
     facetSize = (nv-2)*pPEXIMFacetSizes[iColor][fflag];
     vertexSize = nv*pPEXIMVertexSizes[iColor][vflag];
     size = facetSize + vertexSize;
   } else {
     num_vert = 0;
   }
   
   /* MACOCDATA( PEXOCTriangleStrip,pexTriangleStrip,size,ocptr); */
   /* #define MACOCDATA(oc, structName, size, ocptr) { */
   {
     register pexRenderOutputCommandsReq *req =
       (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req; 
     if ( (req->reqType == pPEXIMOpCode ) 
	 && (req->opcode == PEX_RenderOutputCommands ) 
	 && ((pPEXIMCurrDpy->bufptr + (sizeof(pexTriangleStrip)+(size))) <= pPEXIMCurrDpy->bufmax)) { 
       ocptr = (pexTriangleStrip *)pPEXIMCurrDpy->bufptr; 
       req->length += (sizeof(pexTriangleStrip)+(size))>>2; 
       pPEXIMCurrDpy->bufptr += sizeof(pexTriangleStrip);
       req->numCommands++; 
     } else { 
       ocptr = (pexTriangleStrip *)GetROC(pPEXIMCurrDpy, sizeof(pexTriangleStrip));
       ((pexRenderOutputCommandsReq *)pPEXIMCurrDpy->last_req)->length += (size)>>2; 
     } 
     ocptr->head.elementType = PEXOCTriangleStrip; 
     ocptr->head.length = (sizeof(pexTriangleStrip)+(size))>>2;  
   }
   ocptr->colourType = colour_model;
   ocptr->facetAttribs = pPEXIMFacetAttrs[fflag];
   ocptr->vertexAttribs = pPEXIMVertexAttrs[vflag];
   ocptr->numVertices = nv;
   if (!iColor) {
     if (facetSize>0) {
       s1 = facetSize;
       while (facetSize > 0) {
	 s1 = min(facetSize,NLEFT);
	 if (s1) bcopy(fsrcptr, pPEXIMCurrDpy->bufptr, s1);
	 pPEXIMCurrDpy->bufptr += s1;
	 fsrcptr += s1;
	 if (NLEFT == 0) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	 }
	 facetSize -= s1;
       }
     };
     s1 = vertexSize;
     while (vertexSize > 0) {
       s1 = min(vertexSize,NLEFT);
       if (s1) bcopy(vsrcptr, pPEXIMCurrDpy->bufptr, s1);
       pPEXIMCurrDpy->bufptr += s1;
       vsrcptr += s1;
       if (NLEFT == 0) {
	 XFlush(pPEXIMCurrDpy);
	 mustFlush = 1;
       }
       vertexSize -= s1;
     }
   } else {
     /*
      * Yucky case of having to compress something.
      */
     if (facetSize>0) {
       if (pPEXIMFacetAttrs[fflag] & PEXGAColour) {
	 s2 = nv-2;
	 while (s2 > 0) {
	   s1 = min(s2,NLEFT/pPEXIMFacetSizes[iColor][fflag]);
	   if (s1) CopyDown(s1, (long *)fsrcptr, 
			    pPEXIMFacetSizes[iColor][fflag]+8, 
			    (long *)pPEXIMCurrDpy->bufptr,
			    pPEXIMFacetSizes[iColor][fflag]);
	   pPEXIMCurrDpy->bufptr += s1*pPEXIMFacetSizes[iColor][fflag];
	   fsrcptr += s1*(pPEXIMFacetSizes[iColor][fflag]+8);
	   if (NLEFT == 0) {
	     XFlush(pPEXIMCurrDpy);
	     mustFlush = 1;
	   }
	   s2 -= s1;
	 }
       } else {
	 s1 = facetSize;
	 while (facetSize > 0) {
	   s1 = min(facetSize,NLEFT);
	   if (s1) bcopy(fsrcptr, pPEXIMCurrDpy->bufptr, s1);
	   pPEXIMCurrDpy->bufptr += s1;
	   fsrcptr += s1;
	   if (NLEFT == 0) {
	     XFlush(pPEXIMCurrDpy);
	     mustFlush = 1;
	   }
	   facetSize -= s1;
	 }
       }
     }; /* done with Facet data */
     if (pPEXIMVertexAttrs[vflag] & PEXGAColour) {
       s2 = nv;
       while (s2 > 0) {
	 s1 = min(s2,NLEFT/pPEXIMVertexSizes[iColor][vflag]);
	 if (s1) CopyDown(s1, (long *)vsrcptr, 
			  pPEXIMVertexSizes[iColor][vflag]+8, 
			  (long *)pPEXIMCurrDpy->bufptr,
			  pPEXIMVertexSizes[iColor][vflag]);
	 pPEXIMCurrDpy->bufptr += s1*pPEXIMVertexSizes[iColor][vflag];
	 vsrcptr += s1*(pPEXIMVertexSizes[iColor][vflag]+8);
	 if (NLEFT == 0) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	 }
	 s2 -= s1;
       }
     } else {
       s1 = vertexSize;
       while (vertexSize > 0) {
	 s1 = min(vertexSize,NLEFT);
	 if (s1) bcopy(vsrcptr, pPEXIMCurrDpy->bufptr, s1);
	 pPEXIMCurrDpy->bufptr += s1;
	 vsrcptr += s1;
	 if (NLEFT == 0) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	 }
	 vertexSize -= s1;
       }
     }
   }
   if (mustFlush) XFlush(pPEXIMCurrDpy);
   if ( num_vert > 0 ) {
     vsrcptr -= 2*pPEXIMVertexSizes[iColor][vflag];
     nv = num_vert;
   }
 }   
 UnlockDisplay(pPEXIMCurrDpy);   
}
     
 /* file-poc001 -*/
void
pset_marker_type(markertype)
Pint	markertype;	/* markertype	*/
{PEXIMDebug("pset_marker_type called  - PEXOCMarkerType \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCMarkerType,pexMarkerType,markerType,markertype);
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc002 -*/
void
pset_marker_size(size)
Pfloat	size;	/* markersize scale factor	*/
{PEXIMDebug("pset_marker_size called  - PEXOCMarkerScale \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCMarkerScale,pexMarkerScale,scale,size); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc003 -*/
void
pset_marker_colr_ind(colour)
Pint	colour;	/* polymarker colour index	*/
{PEXIMDebug("pset_marker_colr_ind called  - PEXOCMarkerColourIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCMarkerColourIndex,pexMarkerColourIndex,index,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc004 -*/
void
pset_marker_colr(colour)
Pgcolr	*colour;	/* polymarker colour */
{PEXIMDebug("pset_marker_colr called  - PEXOCMarkerColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCMarkerColour,pexMarkerColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc005 -*/
void
pset_marker_ind(index)
Pint	index;	/* polymarker index	*/
{PEXIMDebug("pset_marker_ind called  - PEXOCMarkerBundleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCMarkerBundleIndex,pexMarkerBundleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc006 -*/
void
pset_text_font(font)
Pint	font;	/* text font	*/
{PEXIMDebug("pset_text_font called  - PEXOCTextFontIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCTextFontIndex,pexTextFontIndex,index,font); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc007 -*/
void
pset_text_prec(precision)
Ptext_prec	precision;	/* text precision	*/
{PEXIMDebug("pset_text_prec called  - PEXOCTextPrecision \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCTextPrecision,pexTextPrecision,precision,precision); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc008 -*/
void
pset_char_expan(exp_factor)
Pfloat	exp_factor;	/* character expansion factor	*/
{PEXIMDebug("pset_char_expan called  - PEXOCCharExpansion \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCCharExpansion,pexCharExpansion,expansion,exp_factor); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc009 -*/
void
pset_char_space(spacing)
Pfloat	spacing;	/* character spacing	*/
{PEXIMDebug("pset_char_space called  - PEXOCCharSpacing \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCCharSpacing,pexCharSpacing,spacing,spacing); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc010 -*/
void
pset_text_colr_ind(colour)
Pint	colour;	/* text colour index	*/
{PEXIMDebug("pset_text_colr_ind called  - PEXOCTextColourIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCTextColourIndex,pexTextColourIndex,index,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc011 -*/
void
pset_text_colr(colour)
Pgcolr	*colour;	/* text colour */
{PEXIMDebug("pset_text_colr called  - PEXOCTextColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCTextColour,pexTextColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc012 -*/
void
pset_char_ht(height)
Pfloat	height;	/* character height	*/
{PEXIMDebug("pset_char_ht called  - PEXOCCharHeight \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCCharHeight,pexCharHeight,height,height); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc013 -*/
void
pset_char_up_vec(up_vect)
Pvec	*up_vect;	/* character up vector	*/
{
 pexCharUpVector *ocptr;
 PEXIMDebug("pset_char_up_vec called  - PEXOCCharUpVector \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCCharUpVector,pexCharUpVector,ocptr);
 ocptr->up.x = up_vect->delta_x;
 ocptr->up.y = up_vect->delta_y;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc014 -*/
void
pset_text_path(path)
Ptext_path	path;	/* text path	*/
{PEXIMDebug("pset_text_path called  - PEXOCTextPath \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCTextPath,pexTextPath,path,path); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc015 -*/
void
pset_text_align(text_align)
Ptext_align	*text_align;	/* text alignment	*/
{pexTextAlignment *ocptr;
 PEXIMDebug("pset_text_align called  - PEXOCTextAlignment \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCTextAlignment,pexTextAlignment,ocptr);
 ocptr->alignment.vertical = text_align->vert;
 ocptr->alignment.horizontal = text_align->hor;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc016 -*/
void
pset_anno_char_ht(height)
Pfloat	height;	/* character height	*/
{PEXIMDebug("pset_anno_char_ht called  - PEXOCAtextHeight \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCAtextHeight,pexAtextHeight,height,height); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc017 -*/
void
pset_anno_char_up_vec(up_vect)
Pvec	*up_vect;	/* character up vector	*/
{pexAtextUpVector *ocptr;
 PEXIMDebug("pset_anno_char_up_vec called  - PEXOCAtextUpVector \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCAtextUpVector,pexAtextUpVector,ocptr);
 ocptr->up.x = up_vect->delta_x;
 ocptr->up.y = up_vect->delta_y;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc018 -*/
void
pset_anno_path(path)
Ptext_path	path;	/* text path	*/
{PEXIMDebug("pset_anno_path called  - PEXOCAtextPath \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCAtextPath,pexAtextPath,path,path); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc019 -*/
void
pset_anno_align(text_align)
Ptext_align	*text_align;	/* text alignment	*/
{pexAtextAlignment *ocptr;
 PEXIMDebug("pset_anno_align called  - PEXOCAtextAlignment \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCAtextAlignment,pexAtextAlignment,ocptr);
 ocptr->alignment.vertical = text_align->vert;
 ocptr->alignment.horizontal = text_align->hor;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc020 -*/
void
pset_anno_style(style)
Pint	style;	/* annotation style	*/
{PEXIMDebug("pset_anno_style called  - PEXOCAtextStyle \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCAtextStyle,pexAtextStyle,style,style); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc021 -*/
void
pset_text_ind(index)
Pint	index;	/* text index	*/
{PEXIMDebug("pset_text_ind called  - PEXOCTextBundleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCTextBundleIndex,pexTextBundleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc022 -*/
void
pset_linetype(linetype)
Pint	linetype;	/* linetype	*/
{PEXIMDebug("pset_linetype called  - PEXOCLineType \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCLineType,pexLineType,lineType,linetype); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc023 -*/
void
pset_linewidth(width)
Pfloat	width;	/* linewidth scale factor	*/
{PEXIMDebug("pset_linewidth called  - PEXOCLineWidth \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCLineWidth,pexLineWidth,width,width); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc024 -*/
void
pset_line_colr_ind(colour)
Pint	colour;	/* polyline colour index	*/
{PEXIMDebug("pset_line_colr_ind called  - PEXOCLineColourIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCLineColourIndex,pexLineColourIndex,index,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc025 -*/
void
pset_line_colr(colour)
Pgcolr	*colour;	/* polyline colour */
{PEXIMDebug("pset_line_colr called  - PEXOCLineColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCLineColour,pexLineColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc026 -*/
void
pset_curve_approx( type, value )
    Pint            type;
    Pfloat          value;
{pexCurveApproximation *ocptr;
 PEXIMDebug("pset_curve_approx called  - PEXOCCurveApproximation \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCCurveApproximation,pexCurveApproximation,ocptr);
 ocptr->approx.approxMethod = type;
 ocptr->approx.tolerance = value;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc027 -*/
void
pset_line_shad_meth( method )
   Pint		method;	/* shading method */
{PEXIMDebug("pset_line_shad_meth called  - PEXOCPolylineInterp \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCPolylineInterp,pexPolylineInterp,polylineInterp,method); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc028 -*/
void
pset_line_ind( index)
    Pint	index;	/* polyline index	*/
{PEXIMDebug("pset_line_ind called  - PEXOCLineBundleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCLineBundleIndex,pexLineBundleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc029 -*/
void
pset_int_style(style)
Pint_style	style;	/* interior style	*/
{PEXIMDebug("pset_int_style called  - PEXOCInteriorStyle \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCInteriorStyle,pexInteriorStyle,interiorStyle,style+1); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc030 -*/
void
pset_int_style_ind(index)
Pint	index;	/* interior style index	*/
{PEXIMDebug("pset_int_style_ind called  - PEXOCInteriorStyleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCInteriorStyleIndex,pexInteriorStyleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc031 -*/
void
pset_int_colr_ind(index)
Pint	index;	/* interior colour index	*/
{PEXIMDebug("pset_int_colr_ind called  - PEXOCSurfaceColourIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceColourIndex,pexSurfaceColourIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc032 -*/
void
pset_int_colr(colour)
Pgcolr	*colour;	/* interior colour */
{PEXIMDebug("pset_int_colr called  - PEXOCSurfaceColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCSurfaceColour,pexSurfaceColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc033 -*/
void
pset_refl_props(properties)
Prefl_props	*properties;	/* surface area properties */
{pexSurfaceReflAttr *ocptr;
 int size;

 size = (properties->specular_colr.type==PINDIRECT)?4:12;
 PEXIMDebug("pset_refl_props called  - PEXOCSurfaceReflAttr \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCSurfaceReflAttr,pexSurfaceReflAttr,size,ocptr);
 ocptr->reflectionAttr.ambient =  properties->ambient_coef;
 ocptr->reflectionAttr.diffuse =  properties->diffuse_coef;
 ocptr->reflectionAttr.specular =  properties->specular_coef;
 ocptr->reflectionAttr.specularConc =  properties->specular_exp;
 ocptr->reflectionAttr.transmission =  0.0; /* life s..., look for px_set... */
 COPY_COLOR(ocptr->reflectionAttr.specularColour,properties->specular_colr);

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc034 -*/
void
pset_refl_eqn(equation)
Pint	equation;	/* reflectance equation */
{PEXIMDebug("pset_refl_eqn called  - PEXOCSurfaceReflModel \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceReflModel,pexSurfaceReflModel,reflectionModel,equation); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc035 -*/
void
pset_int_shad_meth( method )
    Pint	method;		/* shading method */
{PEXIMDebug("pset_int_shad_meth called  - PEXOCSurfaceInterp \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceInterp,pexSurfaceInterp,surfaceInterp,method); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc036 -*/
void
pset_back_int_style(style)
Pint_style	style;	/* back interior style	*/
{PEXIMDebug("pset_back_int_style called  - PEXOCBfInteriorStyle \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCBfInteriorStyle,pexBfInteriorStyle,interiorStyle,style+1); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc037 -*/
void
pset_back_int_style_ind(index)
Pint	index;	/* back interior sytle index	*/
{PEXIMDebug("pset_back_int_style_ind called  - PEXOCBfInteriorStyleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCBfInteriorStyleIndex,pexBfInteriorStyleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc039 -*/
void
pset_back_int_colr(colour)
Pgcolr	*colour;	/* back interior colour */
{PEXIMDebug("pset_back_int_colr called  - PEXOCBfSurfaceColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCBfSurfaceColour,pexBfSurfaceColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc040 -*/
void
pset_back_refl_props(properties)
Prefl_props	*properties;	/* back surface area properties */
{pexBfSurfaceReflAttr *ocptr;
 int size;

 size = (properties->specular_colr.type==PINDIRECT)?4:12;
 PEXIMDebug("pset_back_refl_props called  - PEXOCBfSurfaceReflAttr \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCBfSurfaceReflAttr,pexBfSurfaceReflAttr,size,ocptr);
 ocptr->reflectionAttr.ambient =  properties->ambient_coef;
 ocptr->reflectionAttr.diffuse =  properties->diffuse_coef;
 ocptr->reflectionAttr.specular =  properties->specular_coef;
 ocptr->reflectionAttr.specularConc =  properties->specular_exp;
 ocptr->reflectionAttr.transmission =  0.0; /* life s..., look for px_set... */
 COPY_COLOR(ocptr->reflectionAttr.specularColour,properties->specular_colr);

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc041 -*/
void
pset_back_refl_eqn(equation)
Pint	equation;	/* back reflectance equation */
{PEXIMDebug("pset_back_refl_eqn called  - PEXOCBfSurfaceReflModel \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCBfSurfaceReflModel,pexBfSurfaceReflModel,reflectionModel,equation); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc042 -*/
void
pset_back_int_shad_meth( method )
    Pint	method;		/* back shading method */
{PEXIMDebug("pset_back_int_shad_meth called  - PEXOCBfSurfaceInterp \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCBfSurfaceInterp,pexBfSurfaceInterp,surfaceInterp,method); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc043 -*/
void
pset_surf_approx( type, uvalue, vvalue )
    Pint	type;
    Pfloat	uvalue;
    Pfloat	vvalue;
{pexSurfaceApproximation *ocptr;
 PEXIMDebug("pset_surf_approx called  - PEXOCSurfaceApproximation \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCSurfaceApproximation,pexSurfaceApproximation,ocptr);
 ocptr->approx.approxMethod = type;
 ocptr->approx.uTolerance = uvalue;
 ocptr->approx.vTolerance = vvalue;

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc044 -*/
void
pset_face_cull_mode(mode)
Pcull_mode	mode;	/* culling mode */
{PEXIMDebug("pset_face_cull_mode called  - PEXOCCullingMode \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCCullingMode,pexCullingMode,cullMode,mode); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc045 -*/
void
pset_face_disting_mode(mode)
Pdisting_mode	mode;	/* distinguishing mode */
{PEXIMDebug("pset_face_disting_mode called  - PEXOCDistinguishFlag \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCDistinguishFlag,pexDistinguishFlag,distinguish,mode); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc046 -*/
void
pset_pat_size(size_x, size_y)
Pfloat	size_x;
Pfloat	size_y;
{pexPatternSize *ocptr;
 PEXIMDebug("pset_pat_size called  - PEXOCPatternSize \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCPatternSize,pexPatternSize,ocptr);
 ocptr->size.x = size_x;
 ocptr->size.x = size_y;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc047 -*/
void
pset_pat_ref_point(ref_pt)
Ppoint	*ref_pt;	/* pattern reference point	*/
{pexPatternRefPt *ocptr;
 PEXIMDebug("pset_pat_ref_point called  - PEXOCPatternRefPt \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCPatternRefPt,pexPatternRefPt,ocptr);
 ocptr->point.x = ref_pt->x;
 ocptr->point.x = ref_pt->y;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc048 -*/
void
pset_pat_ref_point_vecs(ref_pt, pat_ref_vec)
Ppoint3	*ref_pt;	/* pattern reference point	*/
Pvec3	pat_ref_vec[2];/* direction vector 1; X axis of pattern	*/
{pexPatternAttr *ocptr;
 PEXIMDebug("pset_pat_ref_point_vecs called  - PEXOCPatternAttr \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCPatternAttr,pexPatternAttr,ocptr);
 ocptr->refPt.x = ref_pt->x;
 ocptr->refPt.y = ref_pt->y;
 ocptr->refPt.z = ref_pt->z;
 ocptr->vector1.x = pat_ref_vec[0].delta_x;
 ocptr->vector1.y = pat_ref_vec[0].delta_y;
 ocptr->vector1.z = pat_ref_vec[0].delta_z;
 ocptr->vector2.x = pat_ref_vec[1].delta_x;
 ocptr->vector2.y = pat_ref_vec[1].delta_y;
 ocptr->vector2.z = pat_ref_vec[1].delta_z;

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc049 -*/
void
pset_int_ind(index)
Pint	index;	/* interior index	*/
{PEXIMDebug("pset_int_ind called  - PEXOCInteriorBundleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCInteriorBundleIndex,pexInteriorBundleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc050 -*/
void
pset_edge_flag(edge_flag)
Pedge_flag	edge_flag;	/* edge flag	*/
{PEXIMDebug("pset_edge_flag called  - PEXOCSurfaceEdgeFlag \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceEdgeFlag,pexSurfaceEdgeFlag,onoff,edge_flag); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc051 -*/
void
pset_edgetype(edgetype)
Pint	edgetype;	/* edgetype	*/
{PEXIMDebug("pset_edgetype called  - PEXOCSurfaceEdgeType \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceEdgeType,pexSurfaceEdgeType,edgeType,edgetype); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc052 -*/
void
pset_edgewidth(scale)
Pfloat	scale;	/* edgewidth scale factor	*/
{PEXIMDebug("pset_edgewidth called  - PEXOCSurfaceEdgeWidth \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceEdgeWidth,pexSurfaceEdgeWidth,width,scale); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc053 -*/
void
pset_edge_colr_ind(index)
Pint	index;	/* edge colour index	*/
{PEXIMDebug("pset_edge_colr_ind called  - PEXOCSurfaceEdgeColourIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCSurfaceEdgeColourIndex,pexSurfaceEdgeColourIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc054 -*/
void
pset_edge_colr(colour)
Pgcolr	*colour;	/* edge colour */
{PEXIMDebug("pset_edge_colr called  - PEXOCSurfaceEdgeColour \n");
 LockDisplay(pPEXIMCurrDpy);
 MACCOLOC( PEXOCSurfaceEdgeColour,pexSurfaceEdgeColour,colour); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc055 -*/
void
pset_edge_ind(index)
Pint	index;	/* edge index	*/
{PEXIMDebug("pset_edge_ind called  - PEXOCEdgeBundleIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCEdgeBundleIndex,pexEdgeBundleIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc056 -*/
void
pset_indiv_asf( att_id, att_source)
    Paspect	att_id;		/* attribute identifier	*/
    Pasf	att_source;	/* attribute source	*/
{pexSetAsfValues *ocptr;
 PEXIMDebug("pset_indiv_asf called  - PEXOCSetAsfValues \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCSetAsfValues,pexSetAsfValues,ocptr);
 ocptr->attribute = att_id;
 ocptr->source = att_source;
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc057 -*/
void
pset_local_tran3(xform, compose_type)
Pmatrix3	xform;	/* transformation matrix	*/
Pcompose_type	compose_type;	/* composition type	*/
{pexLocalTransform *ocptr;
 PEXIMDebug("pset_local_tran3 called  - PEXOCLocalTransform \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCLocalTransform,pexLocalTransform,ocptr);
 ocptr->compType = compose_type;
 bcopy((char *)xform, (char *)ocptr->matrix, sizeof(pexMatrix));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc058 -*/
void
pset_local_tran(xform, compose_type)
Pmatrix	xform;	/* transformation matrix	*/
Pcompose_type	compose_type;	/* composition type	*/
{pexLocalTransform2D *ocptr;
 PEXIMDebug("pset_local_tran called  - PEXOCLocalTransform2D \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCLocalTransform2D,pexLocalTransform2D,ocptr);
 ocptr->compType = compose_type;
 bcopy((char *)xform, (char *)ocptr->matrix3X3, sizeof(pexMatrix3X3));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc059 -*/
void
pset_global_tran3(xform)
Pmatrix3	xform;	/* transformation matrix	*/
{pexGlobalTransform *ocptr;
 PEXIMDebug("pset_global_tran3 called  - PEXOCGlobalTransform \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCGlobalTransform,pexGlobalTransform,ocptr);
 bcopy((char *)xform, (char *)ocptr->matrix, sizeof(pexMatrix));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc060 -*/
void
pset_global_tran(xform)
Pmatrix	xform;	/* transformation matrix	*/
{pexGlobalTransform2D *ocptr;
 PEXIMDebug("pset_global_tran called  - PEXOCGlobalTransform2D \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCGlobalTransform2D,pexGlobalTransform2D,ocptr);
 bcopy((char *)xform, (char *)ocptr->matrix3X3, sizeof(pexMatrix3X3));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc061 -*/
void
pset_model_clip_ind( ind)
    Pclip_ind	ind;	/* clipping indicator	*/
{PEXIMDebug("pset_model_clip_ind called  - PEXOCModelClip \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCModelClip,pexModelClip,onoff,ind); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc062 -*/
void
pset_model_clip_vol3( op, half_spaces)
    Pint		op;			/* operator	*/
    Phalf_space_list3	*half_spaces;	/* list of half spaces	*/
{
 pexModelClipVolume *ocptr;
 int size;
 PEXIMDebug("pset_model_clip_vol3 called  - PEXOCModelClipVolume \n");
 LockDisplay(pPEXIMCurrDpy);
 size = half_spaces->num_half_spaces*2*sizeof(pexCoord3D);
 MACOCDATA( PEXOCModelClipVolume,pexModelClipVolume,size,ocptr);
 ocptr->modelClipOperator = op;
 ocptr->numHalfSpaces = half_spaces->num_half_spaces;
 bcopy((char *)half_spaces->half_spaces, (char *)(ocptr+1), size);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc063 -*/
void
pset_model_clip_vol( op, half_spaces)
    Pint		op;			/* operator	*/
    Phalf_space_list	*half_spaces;	/* list of half spaces	*/
{
 pexModelClipVolume2D *ocptr;
 int size;
 PEXIMDebug("pset_model_clip_vol called  - PEXOCModelClipVolume2D \n");
 LockDisplay(pPEXIMCurrDpy);
 size = half_spaces->num_half_spaces*2*sizeof(Ppoint);
 MACOCDATA( PEXOCModelClipVolume2D,pexModelClipVolume2D,size,ocptr);
 ocptr->modelClipOperator = op;
 ocptr->numHalfSpaces = half_spaces->num_half_spaces;
 bcopy((char *)half_spaces->half_spaces, (char *)(ocptr+1), size);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc064 -*/
void
prestore_model_clip_vol()
{pexRestoreModelClip *ocptr;
 PEXIMDebug("prestore_model_clip_vol called  - PEXOCRestoreModelClip \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOC( PEXOCRestoreModelClip,pexRestoreModelClip,ocptr);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc065 -*/
void
pset_view_ind(index)
Pint	index;	/* view index	*/
{PEXIMDebug("pset_view_ind called  - PEXOCViewIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCViewIndex,pexViewIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc066 -*/
void
pset_light_src_state(activation, deactivation)
Pint_list	 *activation;	/* activation list */
Pint_list	 *deactivation;	/* deactivation list */
{
 pexLightState *ocptr;
 int size;
 CARD16 *l;

 PEXIMDebug("pset_light_src_state called  - PEXOCLightState \n");
 LockDisplay(pPEXIMCurrDpy);
 size = PAD4(activation->num_ints*sizeof(CARD16))
   + PAD4(deactivation->num_ints*sizeof(CARD16));
 MACOCDATA( PEXOCLightState,pexLightState,size,ocptr);
 ocptr->numEnable = activation->num_ints;
 ocptr->numDisable = deactivation->num_ints;
 l = (CARD16 *)(ocptr+1);
 if (activation->num_ints > 0) {
   CopyPintToCARD16((Pint *)activation->ints, (CARD16 *)(l),
	 activation->num_ints);
   l = (CARD16 *)(((char *)l)+PAD4(activation->num_ints*sizeof(CARD16)));
 }
 if (deactivation->num_ints > 0) {
   CopyPintToCARD16((Pint *)deactivation->ints, (CARD16 *)(l),
	 deactivation->num_ints);
 }
 UnlockDisplay(pPEXIMCurrDpy); 
 }
 /* file-poc067 -*/
void
pset_dcue_ind( index)
    Pint	index;	/* depth cue index	*/
{PEXIMDebug("pset_dcue_ind called  - PEXOCDepthCueIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCDepthCueIndex,pexDepthCueIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }   
 /* file-poc068 -*/
void
pset_pick_id( pick_id)
    Pint	pick_id;	/* pick identifier	*/
{PEXIMDebug("pset_pick_id called  - PEXOCPickId \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCPickId,pexPickId,pickId,pick_id); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc069 -*/
void
pset_hlhsr_id(id)
Pint	id;	/* HLHSR identifier	*/
{PEXIMDebug("pset_hlhsr_id called  - PEXOCHlhsrIdentifier \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCHlhsrIdentifier,pexHlhsrIdentifier,hlhsrID,id); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc070 -*/
void
pset_colr_map_ind( index )
    Pint	index;	/* colour mapping index	*/
{PEXIMDebug("pset_colr_map_ind called  - PEXOCColourApproxIndex \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCColourApproxIndex,pexColourApproxIndex,index,index); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc071 -*/
void
pset_rendering_colr_model( colour_model )
    Pint	colour_model;	/* colour model	*/
{PEXIMDebug("pset_rendering_colr_model called  - PEXOCRenderingColourModel \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCRenderingColourModel,pexRenderingColourModel,model,colour_model); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc072 -*/
void
pset_para_surf_characs( psc_type, data )
    Pint		psc_type;	/* type	*/
    Ppara_surf_characs	*data;		/* data record */
{
 pexParaSurfCharacteristics *ocptr;
 int size;
 pexPSC_IsoparametricCurves *iso;
 pexPSC_LevelCurves	    *level;
 short		            *none;

 PEXIMDebug("pset_para_surf_characs called  - PEXOCParaSurfCharacteristics \n");
 switch(psc_type) {
 case PEXPSCNone:
 case PEXPSCImpDep:
   psc_type = PEXPSCNone;
   size = sizeof(long);
   break;
 case PEXPSCIsoCurves:
   size = sizeof(long)+sizeof(pexPSC_IsoparametricCurves);
   break;
 case PEXPSCMcLevelCurves:
 case PEXPSCWcLevelCurves: /* same data structures, different coord space */
   size = sizeof(long)+sizeof(pexPSC_LevelCurves)
     + sizeof(float)*data->psc_4.params.num_floats;
   break;
 default:
   break;
 }

 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCParaSurfCharacteristics,pexParaSurfCharacteristics,
	   size,ocptr);
 
 switch(psc_type) {
 case PEXPSCNone:
 case PEXPSCImpDep:
   none = (short *)(ocptr + 1);
   none[0] = PEXPSCNone;
   none[1] = 0;
   break;
 case PEXPSCIsoCurves:
   none = (short *)(ocptr + 1);
   none[0] = PEXPSCNone;
   none[1] = sizeof(pexPSC_IsoparametricCurves);
   iso = (pexPSC_IsoparametricCurves *)(none+2);
   iso->placementType = data->psc_3.placement;
   iso->numUcurves = data->psc_3.u_count;
   iso->numVcurves = data->psc_3.v_count;
   break;
   break;
 case PEXPSCMcLevelCurves:
 case PEXPSCWcLevelCurves: /* same data structures, different coord space */
   none = (short *)(ocptr + 1);
   none[0] = PEXPSCNone;
   none[1] = size - sizeof(long);
   level = (pexPSC_LevelCurves *)(none+2);
   level->origin.x = data->psc_4.origin.x;
   level->origin.y = data->psc_4.origin.y;
   level->origin.z = data->psc_4.origin.z;
   level->direction.x = data->psc_4.direction.delta_x;
   level->direction.y = data->psc_4.direction.delta_y;
   level->direction.z = data->psc_4.direction.delta_z;
   level->numberIntersections = data->psc_4.params.num_floats;
   bcopy((char *)data->psc_4.params.floats, (char *)(level+1),
	 sizeof(float)*level->numberIntersections);
   break;
 default:
   break;
 }
 UnlockDisplay(pPEXIMCurrDpy); 
}
 /* file-poc073 -*/
void
padd_names_set(set)
Pint_list	*set;	/* set of names to be added	*/
{
 pexAddToNameSet *ocptr;
 int size;

 PEXIMDebug("padd_names_set called  - PEXOCAddToNameSet \n");
 LockDisplay(pPEXIMCurrDpy);
 size = PAD4(set->num_ints*sizeof(CARD16));
 MACLSTOC( PEXOCAddToNameSet,pexAddToNameSet,set->num_ints, sizeof(CARD32),
	  ocptr); 
 bcopy((char *)set->ints, (char *)(ocptr+1), set->num_ints*sizeof(CARD32));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc074 -*/
void
premove_names_set(set)
Pint_list	*set;	/* set of names to be removed	*/
{
 pexRemoveFromNameSet *ocptr;

 PEXIMDebug("premove_names_set called  - PEXOCRemoveFromNameSet \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCRemoveFromNameSet,pexRemoveFromNameSet,set->num_ints, sizeof(CARD32),
	  ocptr); 
 bcopy((char *)set->ints, (char *)(ocptr+1), set->num_ints*sizeof(CARD32));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc075 -*/
void
pexec_struct(struct_id)
Pint	struct_id;	/* structure identifier	*/
{
  int i;
  XID sid;

  PEXIMDebug("pexec_struct called  - PEXOCExecuteStructure \n");
  /*
   * O.K. Optimize programmer time here for two reasons.
   * 1) I have no idea of how many structures will be needed in an immediate
   *    mode package.
   * 2) This code get thrown away at PEX 6.0
   */
  sid = 0;
  for (i = 0; i < pPEXIMMaxStruct; i++ ) {
    if (pPEXIMStructTable[i].structID == struct_id) {
      sid = pPEXIMStructTable[i].resourceID;
      break;
    }
  }

  if ( !sid ) return;

 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCExecuteStructure,pexExecuteStructure,id,sid); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc076 -*/
void
plabel(label_id)
Pint	label_id;	/* label identifier	*/
{PEXIMDebug("plabel called  - PEXOCLabel \n");
 LockDisplay(pPEXIMCurrDpy);
 MACSOC( PEXOCLabel,pexLabel,label,label_id); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc077 -*/
void
pappl_data(data)
Pdata	*data;	/* application data	*/
{
 pexApplicationData *ocptr;

 PEXIMDebug("pappl_data called  - PEXOCApplicationData \n");
 LockDisplay(pPEXIMCurrDpy);

 MACOCDATA( PEXOCApplicationData,pexApplicationData,PAD4(data->size),ocptr); 
 ocptr->numElements = data->size;
 bcopy((char *)data->data, (char *)(ocptr+1), data->size);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc078 -*/
void
pgse(id, gse)
Pint	id;	/* gse identifier	*/
Pgse_data	*gse;	/* gse data record	*/
{
 pexGse *ocptr;
 int i;

 PEXIMDebug("pgse called  - PEXOCGse \n");

 if(id > 0) return;

 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCGse,pexGse,PAD4(gse->unsupp.size),ocptr);
   if (pPEXIMForeignServer != 0) {
     if (pPEXIMForeignServer < 0) {  /* need to inquire list of avail. */
       pPEXIMForeignServer = 0;
       pPEXIMBaseGSE = 0;
       pPEXIMBaseGDP = 0;
       pPEXIMBaseGDP3 = 0;
     } else {
       return;
     }
   }

 ocptr->id = id;
 ocptr->numElements = gse->unsupp.size;
 bcopy((char *)gse->unsupp.data, (char *)(ocptr+1), gse->unsupp.size);
 UnlockDisplay(pPEXIMCurrDpy); 
 }
 /* file-poc079 -*/
void
ppolymarker3(point_list)
Ppoint_list3	*point_list;
{
 pexMarker *ocptr;
 int c,s;
 PEXIMDebug("ppolymarker3 called  - PEXOCMarker \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCMarker,pexMarker,point_list->num_points, 
	  sizeof(pexCoord3D), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1), point_list->num_points*sizeof(pexCoord3D));

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc080 -*/
void
ppolymarker(point_list)
Ppoint_list	*point_list;
{
 pexMarker2D *ocptr;
 int c,s;
 PEXIMDebug("ppolymarker called  - PEXOCMarker2D \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCMarker2D,pexMarker2D,point_list->num_points, 
	  sizeof(Ppoint), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1), point_list->num_points*sizeof(Ppoint));

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc081 -*/
void
ptext3(text_pt, dir, text)
Ppoint3		*text_pt;	/* text point	*/
Pvec3   	dir[2];		/* directon vectors	*/
char		*text;		/* text string	*/
{
 pexText *ocptr;
 pexMonoEncoding *mec;
 int size,s;
 PEXIMDebug("ptext3 called  - PEXOCText \n");
 s = strlen(text);
 size = sizeof(pexMonoEncoding) + PAD4(s);
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCText,pexText,size,ocptr);
 ocptr->origin.x = text_pt->x;
 ocptr->origin.y = text_pt->y;
 ocptr->origin.z = text_pt->z;
 ocptr->vector1.x = dir[0].delta_x;
 ocptr->vector1.y = dir[0].delta_y;
 ocptr->vector1.z = dir[0].delta_z;
 ocptr->vector2.x = dir[1].delta_x;
 ocptr->vector2.y = dir[1].delta_y;
 ocptr->vector2.z = dir[1].delta_z;
 ocptr->numEncodings = 1;
 mec = (pexMonoEncoding *)(ocptr+1);
 mec->characterSet = 0;
 mec->characterSetWidth = 0;;
 mec->encodingState = 0;
 mec->numChars = s;
 bcopy(text,(char *)(((char *)mec)+sizeof(pexMonoEncoding)),s);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc082 -*/
void
ptext(text_pt, text)
Ppoint	*text_pt;	/* text point	*/
char	*text;		/* text string	*/
{
 pexText2D *ocptr;
 pexMonoEncoding *mec;
 int size,s;
 PEXIMDebug("ptext called  - PEXOCText2D \n");
 s = strlen(text);
 size = sizeof(pexMonoEncoding) + PAD4(s);
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCText2D,pexText2D,size,ocptr);
 ocptr->origin.x = text_pt->x;
 ocptr->origin.y = text_pt->y;
 ocptr->numEncodings = 1;
 mec = (pexMonoEncoding *)(ocptr+1);
 mec->characterSet = 0;
 mec->characterSetWidth = 0;;
 mec->encodingState = 0;
 mec->numChars = s;
 bcopy(text,(char *)(((char *)mec)+sizeof(pexMonoEncoding)),s);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc083 -*/
void
panno_text_rel3( ref_pt, anno_offset, text )
    Ppoint3	*ref_pt;	/* reference point	*/
    Pvec3	*anno_offset;	/* annotation offset	*/
    char	*text;		/* annotation text string	*/
{
 pexAnnotationText *ocptr;
 pexMonoEncoding *mec;
 int size,s;
 PEXIMDebug("panno_text_rel3 called  - PEXOCAnnotationText \n");
 s = strlen(text);
 size = sizeof(pexMonoEncoding) + PAD4(s);
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCAnnotationText,pexAnnotationText,size,ocptr);
 ocptr->origin.x = ref_pt->x;
 ocptr->origin.y = ref_pt->y;
 ocptr->origin.z = ref_pt->z;
 ocptr->offset.x = anno_offset->delta_x;
 ocptr->offset.y = anno_offset->delta_y;
 ocptr->offset.z = anno_offset->delta_z;
 ocptr->numEncodings = 1;
 mec = (pexMonoEncoding *)(ocptr+1);
 mec->characterSet = 0;
 mec->characterSetWidth = 0;;
 mec->encodingState = 0;
 mec->numChars = s;
 bcopy(text,(char *)(((char *)mec)+sizeof(pexMonoEncoding)),s);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc084 -*/
void
panno_text_rel( ref_pt, anno_offset, text )
    Ppoint	*ref_pt;	/* reference point	*/
    Pvec	*anno_offset;	/* annotation offset	*/
    char	*text;		/* annotation text string	*/
{
 pexAnnotationText2D *ocptr;
 pexMonoEncoding *mec;
 int size,s;
 PEXIMDebug("panno_text_rel called  - PEXOCAnnotationText2D \n");
 LockDisplay(pPEXIMCurrDpy);
 s = strlen(text);
 size = sizeof(pexMonoEncoding) + PAD4(s);
 MACOCDATA( PEXOCAnnotationText2D,pexAnnotationText2D,size,ocptr);
 ocptr->origin.x = ref_pt->x;
 ocptr->origin.y = ref_pt->y;
 ocptr->offset.x = anno_offset->delta_x;
 ocptr->offset.y = anno_offset->delta_y;
 ocptr->numEncodings = 1;
 mec = (pexMonoEncoding *)(ocptr+1);
 mec->characterSet = 0;
 mec->characterSetWidth = 0;;
 mec->encodingState = 0;
 mec->numChars = s;
 bcopy(text,(char *)(((char *)mec)+sizeof(pexMonoEncoding)),s);
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc085 -*/
void
ppolyline3(point_list)
Ppoint_list3	*point_list;	/* list of points */
{
 pexPolyline *ocptr;

 PEXIMDebug("ppolyline3 called  - PEXOCPolyline \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCPolyline,pexPolyline,point_list->num_points, 
	  sizeof(pexCoord3D), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1),
       point_list->num_points*sizeof(pexCoord3D));

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc086 -*/
void
ppolyline(point_list)
Ppoint_list	*point_list;
{
 pexPolyline2D *ocptr;

 PEXIMDebug("ppolyline called  - PEXOCPolyline2D \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCPolyline2D,pexPolyline2D,point_list->num_points, 
	  sizeof(Ppoint), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1),
       point_list->num_points*sizeof(Ppoint));

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc087 -*/
void
ppolyline_set3_data(vflag, colour_model, npl, vdata)
Pint	        	vflag;		/* data per vertex flag */
Pint	        	colour_model;	/* colour model */
Pint	        	npl;		/* number of polylines in the set */
Pline_vdata_list3	*vdata;		/* pointer to line vertex data list */
{
 pexPolylineSet *ocptr;
 int i, size, ts, ts2, maxSize, iColor, vtype;
 Pline_vdata_list3 *v;
 char *op;
 long *n;
 int mustFlush = 0, breakup, s1, s2, data_size;

		/* pointer to line vertex data list */
 Pline_vdata_list3	*save_vdata, *next_vdata;
 Pint			save_npl = 0, npl_left;

 char *srcptr;

 PEXIMDebug("ppolyline_set3_data called  - PEXOCPolylineSet \n");

 if (vflag==PVERT_COORD) {vtype=0;}
 else if (vflag==PVERT_COORD_COLOUR) {vtype=1;}
 else return;

 iColor = (colour_model==PINDIRECT)?1:0; /* indexed color? headaches */
 ts = pPEXIMVertexSizes[iColor][vflag];
 for (i=0, data_size = 0, v = vdata; i<npl; i++, v++) 
   data_size += (v->num_vertices*ts + sizeof(long));

 next_vdata = vdata;
 npl_left = npl;

 LockDisplay(pPEXIMCurrDpy);

 while (data_size > 0) {
   
   if ((data_size + sizeof(pexPolylineSet) + sizeof(pexRenderOutputCommandsReq))>>2 >
       pPEXIMCurrDpy->max_request_size) {
     if (save_npl == 0) {
       save_vdata = vdata;
       save_npl = npl;
       
       maxSize = pPEXIMCurrDpy->max_request_size
	 - ((sizeof(long) + sizeof(pexPolylineSet) + 
	     sizeof(pexRenderOutputCommandsReq)) >> 2);
       ts2 = ts>>2;
       
       /* make sure all of the individual polylines will fit */
       for (i=0, v = vdata; i<npl; i++, v++) 
	 if (v->num_vertices*ts2 > maxSize) {
	   PEXIMError();
	   return;
	 }
       maxSize = maxSize << 2;
     }
     vdata = next_vdata;
     /* O.K. we can ship it, gather up the first bunch to ship */
     for (i=0, size = 0, v = vdata; i<npl_left; i++, v++) {
       if ((size + (ts2 = v->num_vertices*ts + sizeof(long))) > maxSize) {
	 npl = i;
	 next_vdata = v;
	 npl_left -= npl;
	 data_size -= size;
	 break;
       } else {
	 size += ts2;
       }
     }
   } else {
     /* last pass, or only pass */
     size = data_size;
     data_size = 0;
     vdata = next_vdata;
     npl = npl_left;
   }
   
   /* MACOCDATA( PEXOCPolylineSet,pexPolylineSet,size,ocptr); */
   /* #define MACOCDATA(oc, structName, size, ocptr) { */
   {
     register pexRenderOutputCommandsReq *req =
       (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req; 
     if ( (req->reqType == pPEXIMOpCode ) 
	 && (req->opcode == PEX_RenderOutputCommands ) 
	 && ((pPEXIMCurrDpy->bufptr + (sizeof(pexPolylineSet)+(size))) <= pPEXIMCurrDpy->bufmax)) { 
       ocptr = (pexPolylineSet *)pPEXIMCurrDpy->bufptr; 
       req->length += (sizeof(pexPolylineSet)+(size))>>2; 
       pPEXIMCurrDpy->bufptr += sizeof(pexPolylineSet);
       req->numCommands++; 
     } else { 
       ocptr = (pexPolylineSet *)GetROC(pPEXIMCurrDpy, sizeof(pexPolylineSet));
       ((pexRenderOutputCommandsReq *)pPEXIMCurrDpy->last_req)->length += (size)>>2; 
     } 
     ocptr->head.elementType = PEXOCPolylineSet; 
     ocptr->head.length = (sizeof(pexPolylineSet)+(size))>>2;  
   }
   
   ocptr->colourType = colour_model;
   ocptr->vertexAttribs = pPEXIMVertexAttrs[vflag];
   ocptr->numLists = npl;
   n = (long *)(ocptr+1);
   if (!vtype) {
     for (i=0,v=vdata; i<npl; i++,v++) {
       if (NLEFT < sizeof(long)) {
	 XFlush(pPEXIMCurrDpy);
	 mustFlush = 1;
	 n = (long *)pPEXIMCurrDpy->bufptr;
       }
       *n = v->num_vertices;
       pPEXIMCurrDpy->bufptr += sizeof(long);
       s2 = v->num_vertices*sizeof(pexCoord3D);
       srcptr = (char *)v->vertex_data.points; 
       while (s2 > 0) {
	 s1 = min(s2,NLEFT);
	 if (s1) bcopy(srcptr, pPEXIMCurrDpy->bufptr, s1);
	 srcptr += s1;
	 pPEXIMCurrDpy->bufptr += s1;
	 if (NLEFT == 0) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	 }
	 s2 -= s1;
       }
       n = (long *)pPEXIMCurrDpy->bufptr;
     }
   } else {
     /* we have colors with the vertices, how bad is it ? */
     if (!iColor) {
       /* not too bad, vertices are floating point */
       for (i=0,v=vdata; i<npl; i++,v++) {
	 if (NLEFT < sizeof(long)) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	   n = (long *)pPEXIMCurrDpy->bufptr;
	 }
	 *n = v->num_vertices;
	 pPEXIMCurrDpy->bufptr += sizeof(long);
	 s2 = v->num_vertices*ts;
	 srcptr = (char *)v->vertex_data.ptcolrs;
	 while (s2 > 0) {
	   s1 = min(s2,NLEFT);
	   if (s1) bcopy(srcptr, pPEXIMCurrDpy->bufptr, s1);
	   srcptr += s1;
	   pPEXIMCurrDpy->bufptr += s1;
	   if (NLEFT == 0) {
	     XFlush(pPEXIMCurrDpy);
	     mustFlush = 1;
	   }
	   s2 -= s1;
	 }
	 n = (long *)pPEXIMCurrDpy->bufptr;
       }
     } else {
       for (i=0,v=vdata; i<npl; i++,v++) {
	 if (NLEFT < sizeof(long)) {
	   XFlush(pPEXIMCurrDpy);
	   mustFlush = 1;
	   n = (long *)pPEXIMCurrDpy->bufptr;
	 }
	 *n = v->num_vertices;
	 pPEXIMCurrDpy->bufptr += sizeof(long);
	 s2 = v->num_vertices;
	 srcptr = (char *)v->vertex_data.ptcolrs;
	 while (s2 > 0) {
	   /* s1 is vertex count in this loop */
	   s1 = min(s2,NLEFT/ts);
	   if (s1) CopyDown(s1, (long *)srcptr, ts+8, 
			    (long *)pPEXIMCurrDpy->bufptr, ts);
	   srcptr += s1*(ts+8);
	   pPEXIMCurrDpy->bufptr += s1*ts;
	   if (NLEFT < ts) {
	     XFlush(pPEXIMCurrDpy);
	     mustFlush = 1;
	   }
	   s2 -= s1;
	 }
	 n = (long *)pPEXIMCurrDpy->bufptr;
       }
     }
   }
   if (mustFlush) XFlush(pPEXIMCurrDpy);
 }
 UnlockDisplay(pPEXIMCurrDpy); 
}
 /* file-poc088 -*/
void
pnuni_bsp_curv( order, knots, rationality, cpoints, pmin, pmax )
    Pint		order;		/* spline order */
    Pfloat_list		*knots;		/* list of knots */
    Prational		rationality;
    Ppoint_list34	*cpoints;	/* list of 3D or 4D  control points */
    Pfloat		pmin, pmax;	/* parameter range */
{
 pexNurbCurve *ocptr;
 int size;

 PEXIMDebug("pnuni_bsp_curv called  - PEXOCNurbCurve \n");
 LockDisplay(pPEXIMCurrDpy);
 size = knots->num_floats*sizeof(float)
   + cpoints->num_points*((rationality)?sizeof(pexCoord4D):sizeof(pexCoord3D));
 MACOCDATA( PEXOCNurbCurve,pexNurbCurve,size,ocptr);
 ocptr->curveOrder = order;
 ocptr->coordType = rationality;
 ocptr->tmin = pmin;
 ocptr->tmax = pmax;
 ocptr->numKnots = knots->num_floats;
 ocptr->numPoints = cpoints->num_points;
 bcopy((char *)knots->floats, (char *)(ocptr+1), 
       knots->num_floats*sizeof(float));
 bcopy((char *)cpoints->points.point3d,
       (((char *)(ocptr+1)) + knots->num_floats*sizeof(float)), 
       cpoints->num_points*((rationality)?sizeof(pexCoord4D):sizeof(pexCoord3D)));
 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc089 -*/
void
pfill_area3(point_list)
Ppoint_list3	*point_list;
{
 pexFillArea *ocptr;

 PEXIMDebug("pfill_area3 called  - PEXOCFillArea \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCFillArea,pexFillArea,point_list->num_points, 
	  sizeof(pexCoord3D), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1),
       point_list->num_points*sizeof(pexCoord3D));

 UnlockDisplay(pPEXIMCurrDpy); }

 /* file-poc090 -*/
void
pfill_area(point_list)
Ppoint_list	*point_list;
{
 pexFillArea2D *ocptr;

 PEXIMDebug("pfill_area called  - PEXOCFillArea2D \n");
 LockDisplay(pPEXIMCurrDpy);
 MACLSTOC( PEXOCFillArea2D,pexFillArea2D,point_list->num_points, 
	  sizeof(Ppoint), ocptr);
 bcopy((char *)point_list->points, 
       (char *)(ocptr+1),
       point_list->num_points*sizeof(Ppoint));

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc092 -*/
void
pfill_area_set3(num_sets, sets)
Pint		num_sets;	/* number of sets of points     */
Ppoint_list3	*sets;		/* array of sets        */
{
 pexFillAreaSet *ocptr;
 int i, size;
 Ppoint_list3 *p;
 char *op;

 PEXIMDebug("pfill_area_set3 called  - PEXOCFillAreaSet \n");
 LockDisplay(pPEXIMCurrDpy);

 for (i=0, size = 0, p = sets; i<num_sets; i++, p++) 
   size += p->num_points*sizeof(pexCoord3D);
 MACOCDATA( PEXOCFillAreaSet,pexFillAreaSet,size,ocptr);
 op = (char *)(ocptr+1);
 for (i=0,p=sets; i<num_sets; i++,p++) {
   bcopy((char *)p->points, op, p->num_points*sizeof(pexCoord3D));
   op += p->num_points*sizeof(pexCoord3D);
 
 UnlockDisplay(pPEXIMCurrDpy); }
}
 /* file-poc093 -*/
void
pfill_area_set(num_sets, sets)
Pint		num_sets;	/* number of sets of points     */
Ppoint_list	*sets;		/* array of sets        */
{
 pexFillAreaSet2D *ocptr;
 int i, size;
 Ppoint_list *p;
 char *op;

 PEXIMDebug("pfill_area_set called  - PEXOCFillAreaSet2D \n");
 LockDisplay(pPEXIMCurrDpy);
 for (i=0, size = 0, p = sets; i<num_sets; i++, p++) 
   size += p->num_points*sizeof(pexCoord3D);
 MACOCDATA( PEXOCFillAreaSet2D,pexFillAreaSet2D,size,ocptr);
 op = (char *)(ocptr+1);
 for (i=0,p=sets; i<num_sets; i++,p++) {
   bcopy((char *)p->points, op, p->num_points*sizeof(pexCoord2D));
   op += p->num_points*sizeof(pexCoord2D);
 
 UnlockDisplay(pPEXIMCurrDpy); }
}
 /* file-poc094 -*/
void
pfill_area_set3_data(fflag, eflag, vflag, colour_model, fdata, nfa, edata, vdata)
Pint           	fflag;      	/* what data is specified per facet */
Pint           	eflag;          /* what data is specified per edge */
Pint           	vflag;          /* what data is specified per vertex */
Pint           	colour_model;   /* colour model */
Pfacet_data3     *fdata;     	/* facet data */
Pint		nfa;		/* number of fill areas in the set */
Pedge_data_list	*edata;		/* edge data list */
Pfacet_vdata_list3	*vdata;         /* facet vertex data list */
{
 pexExtFillAreaSet *ocptr;
 int size;
 PEXIMDebug("pfill_area_set3_data called  - PEXOCExtFillAreaSet \n");
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCExtFillAreaSet,pexExtFillAreaSet,size,ocptr); 
 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc095 -*/
 /* file-poc096 -*/
void
pquad_mesh3_data(fflag, vflag, colour_model, dim, fdata, vdata)
Pint           	fflag;      	/* what data is specified per facet */
Pint           	vflag;          /* what data is specified per vertex */
Pint           	colour_model;   /* colour model */
Pint_size	*dim;		/* dimension of vertices */
Pfacet_data_arr3  *fdata;     	/* facet data */
Pfacet_vdata_arr3	*vdata;         /* facet vertex data */
{
 pexQuadrilateralMesh *ocptr;
 int size, nf, nv;
 int iColor, facetSize, vertexSize;

 PEXIMDebug("pquad_mesh3_data called  - PEXOCQuadrilateralMesh \n");
 LockDisplay(pPEXIMCurrDpy);

 iColor = (colour_model==PINDIRECT)?1:0;
 /* From encoding spec 4+((p-2)*d +p*v)/4 */
 facetSize = (dim->size_x-1)*(dim->size_y-1)*pPEXIMFacetSizes[iColor][fflag];
 vertexSize = dim->size_x*dim->size_y*pPEXIMVertexSizes[iColor][vflag];
 size = facetSize + vertexSize;

 MACOCDATA( PEXOCQuadrilateralMesh,pexQuadrilateralMesh,size,ocptr);
 ocptr->colourType = colour_model;
 ocptr->mPts = dim->size_x;
 ocptr->nPts = dim->size_y;
 ocptr->facetAttribs = pPEXIMFacetAttrs[fflag];
 ocptr->vertexAttribs = pPEXIMVertexAttrs[vflag];
 ocptr->shape = PEXComplex; /* don't bother now */

 if (!iColor) {
   if (facetSize>0) {
     bcopy((char *)fdata->colrs, (char *)(ocptr+1), facetSize);
   };
   bcopy((char *)vdata->points, (((char *)(ocptr+1))+facetSize), vertexSize);
 } else {
   /*
    * Yucky case of having to compress something.
    */
   if (facetSize>0) {
     if (ocptr->facetAttribs & PEXGAColour) {
       CopyDown((dim->size_x-1)*(dim->size_y-1),
		(long *)fdata->colrs, pPEXIMFacetSizes[iColor][fflag]+8, 
		(long *)(ocptr+1), pPEXIMFacetSizes[iColor][fflag]);
     } else {
       bcopy((char *)fdata->colrs, (char *)(ocptr+1), facetSize);
     }
   }; /* done with Facet data */
   if (ocptr->vertexAttribs & PEXGAColour) {
     CopyDown(dim->size_x*dim->size_y,
	      (long *)vdata->points, pPEXIMVertexSizes[iColor][vflag]+8, 
	      (long *)(((char *)(ocptr+1))+facetSize),
	      pPEXIMVertexSizes[iColor][vflag]);
   } else {
     bcopy((char *)vdata->points, 
	   (char *)(((char *)(ocptr+1))+facetSize),
	   vertexSize);
   }
 } 
 UnlockDisplay(pPEXIMCurrDpy);   
}
 /* file-poc097 -*/
void
pset_of_fill_area_set3_data( fflag, eflag, vflag, colour_model, num_sets,
	fdata, edata, vlist, vdata )
    Pint		fflag;		/* data per facet flag */
    Pint		eflag;		/* data per edge flag */
    Pint		vflag;		/* data per vertex flag */
    Pint		colour_model;	/* colour model */
    Pint		num_sets;	/* number of fill area sets */
    Pfacet_data_arr3	*fdata;     	/* facet data */
    Pedge_data_list_list *edata;	/* array of L(L(E)) edge data */
    Pint_list_list	*vlist;		/* array of L(L(I)) vertex indices */
    Pfacet_vdata_list3	*vdata;         /* facet vertex data */
{
 pexSOFAS *ocptr;
 int i, j, size, isOne;
 int iColor, facetSize, vertexSize, edgeSize, conSize;
 Pint_list *l;
 Pedge_data_list *e;
 CARD16 *c16;
 char *b;
 Pedge_data_list_list *ell;
 Pint_list_list	*vll;
 int conCount;

 PEXIMDebug("pset_of_fill_area_set3_data called  - PEXOCSOFAS \n");

 iColor = (colour_model==PINDIRECT)?1:0;
 /* From encoding spec fc*d + vc*v + ec*e +p1 +2fc + 2cc + 2ec +p2 */
 facetSize = num_sets*pPEXIMFacetSizes[iColor][fflag];
 vertexSize = vdata->num_vertices*pPEXIMVertexSizes[iColor][vflag];
 if ( eflag == PEDGE_VISIBILITY ) {
   for ( j = 0, edgeSize = 0, ell = edata; j < num_sets; j++,ell++) {
     for (i = 0, e =ell->edgelist; i <ell->num_lists; i++,e++){
       edgeSize += e->num_edges;
     }
   }
 } else edgeSize = 0;
 isOne = 1;
 conCount = 0;
 for (j=0, conSize = 0, vll = vlist; j < num_sets; j++, vll++ ) {
   if (vll->num_lists != 1) isOne = 0;
   conCount += vll->num_lists;
   for (i = 0, l = vll->lists; i < vll->num_lists; i++, l++) {
     conSize += l->num_ints*sizeof(CARD16);
   }
 }
 size = facetSize + vertexSize + PAD4(edgeSize) + conSize;

 if ((edgeSize != 0) && (conSize != 2*edgeSize)) PEXIMDebug("SOFA check!\n");

 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCSOFAS,pexSOFAS,size,ocptr);

 ocptr->colourType = colour_model;
 ocptr->FAS_Attributes = pPEXIMFacetAttrs[fflag];
 ocptr->vertexAttributes = pPEXIMVertexAttrs[vflag];
 ocptr->edgeAttributes = (eflag==PEDGE_VISIBILITY)?1:0;
 ocptr->shape = PEXComplex; /* don't bother now */
 ocptr->contourHint = PEXUnknownShape;
 ocptr->contourCountsFlag = isOne;
 ocptr->numFAS = num_sets;
 ocptr->numVertices = vdata->num_vertices;
 ocptr->numEdges = edgeSize;
 ocptr->numContours = conCount;

 if (!iColor) {
   if (facetSize>0) {
     bcopy((char *)fdata->colrs, (char *)(ocptr+1), facetSize);
   };
   bcopy((char *)vdata->vertex_data.points, (((char *)(ocptr+1))+facetSize), vertexSize);
 } else {
   /*
    * Yucky case of having to compress something.
    */
   if (facetSize>0) {
     if (ocptr->FAS_Attributes & PEXGAColour) {
       CopyDown(num_sets,
		(long *)fdata->colrs, pPEXIMFacetSizes[iColor][fflag]+8, 
		(long *)(ocptr+1), pPEXIMFacetSizes[iColor][fflag]);
     } else {
       bcopy((char *)fdata->colrs, (char *)(ocptr+1), facetSize);
     }
   }; /* done with Facet data */
   if (ocptr->vertexAttributes & PEXGAColour) {
     CopyDown(vdata->num_vertices,
	      (long *)vdata->vertex_data.points, pPEXIMVertexSizes[iColor][vflag]+8, 
	      (long *)(((char *)(ocptr+1))+facetSize),
	      pPEXIMVertexSizes[iColor][vflag]);
   } else {
     bcopy((char *)vdata->vertex_data.points, 
	   (char *)(((char *)(ocptr+1))+facetSize),
	   vertexSize);
   }
 }   
 /* alright, we copied the facets and and the vertices. Next is the edge
  * data and then the connectivity data
  */
 b = ((char *)(ocptr+1)) + facetSize + vertexSize;
 if (edgeSize>0)
   for ( j = 0, ell = edata; j < num_sets; j++, ell++) {
     for (i = 0, e =ell->edgelist; i <ell->num_lists; i++,e++){
       bcopy(e->edgedata.edges, b, e->num_edges);
       b += e->num_edges;
     }
   }
 c16 = (CARD16 *)PAD4((int)b);
 for (j=0, vll = vlist; j < num_sets; j++, vll++ ) {
   for (i = 0, l = vll->lists; i < vll->num_lists; i++, l++) {
     *c16++ = l->num_ints;
     CopyPintToCARD16( l->ints, c16, l->num_ints);
     c16 += l->num_ints; /* risky, counting on compiler to add 2*num_ints */
   }
 }
 /* CLEAR! */
 UnlockDisplay(pPEXIMCurrDpy); 
}
 /* file-poc098 -*/
void
pnuni_bsp_surf(Pint uorder, Pint vorder, Pfloat_list *uknots,
               Pfloat_list *vknots, Prational rationality, Ppoint_grid34 *grid,
               Pint nloops, Ptrimcurve_list *tloops)
{
 pexNurbSurface *ocptr;
 int i, j, size;
 int knotSize, ctrlSize, trimSize;
 Ptrimcurve_list *tl;
 Ptrimcurve *tc;
 char *b;
 pexTrimCurve *ptc;

 PEXIMDebug("pnuni_bsp_surf called  - PEXOCNurbSurface \n");
 knotSize = (uknots->num_floats+vknots->num_floats)*sizeof(float);
 ctrlSize = grid->num_points.u_dim*grid->num_points.v_dim
   * ((rationality)?sizeof(pexCoord4D):sizeof(pexCoord3D));
	      
 for (j=0, trimSize=0, tl = tloops; j < nloops; j++, tl++) {
   for (i = 0, tc=tl->curves; i < tl->num_curves; i++, tc++){
     trimSize += sizeof(pexTrimCurve)
	       + (tc->knots.num_floats)*sizeof(float)
	       + (tc->cpts.num_points)
		 * ((tc->rationality)?sizeof(pexCoord3D):sizeof(pexCoord2D));
   }
 }
 size = knotSize + ctrlSize + trimSize;
 LockDisplay(pPEXIMCurrDpy);
 MACOCDATA( PEXOCNurbSurface,pexNurbSurface,PAD4(size),ocptr);
 ocptr->type = rationality;
 ocptr->uOrder = uorder;
 ocptr->vOrder = vorder;
 ocptr->numUknots = uknots->num_floats;
 ocptr->numVknots = vknots->num_floats;
 ocptr->mPts = grid->num_points.u_dim;
 ocptr->nPts = grid->num_points.v_dim;
 ocptr->numLists = nloops;
 bcopy((char *)uknots->floats, (char *)(ocptr-1), 
       uknots->num_floats*sizeof(pexCoord3D));
 b = ((char *)(ocptr-1)+uknots->num_floats*sizeof(pexCoord3D));
 bcopy((char *)vknots->floats, b, vknots->num_floats*sizeof(pexCoord3D));
 b += vknots->num_floats*sizeof(pexCoord3D);
 bcopy((char *)grid->points.point3d, b, ctrlSize );
 b += ctrlSize;
 ptc = (pexTrimCurve *)b;
 for (j=0, tl = tloops; j < nloops; j++, tl++) {
   for (i = 0, tc=tl->curves; i < tl->num_curves; i++, tc++){
   ptc->visibility = tc->visible;
   ptc->order = tc->order;
   ptc->type = tc->rationality;
   ptc->approxMethod = tc->approx_type;
   ptc->tolerance = tc->approx_val;
   ptc->tMin = tc->tmin;
   ptc->tMax = tc->tmax;
   ptc->numKnots = tc->knots.num_floats;
   ptc->numCoord = tc->cpts.num_points;
   bcopy((char *)tc->knots.floats, (char *)(ptc+1), 
	 tc->knots.num_floats*sizeof(float));
   b = ((char *)(ptc+1)) + tc->knots.num_floats*sizeof(float);
   ctrlSize = tc->cpts.num_points
	      * ((tc->rationality)?sizeof(pexCoord3D):sizeof(pexCoord2D));
   bcopy((char *)tc->cpts.points.point2d, b, ctrlSize);
   ptc = (pexTrimCurve *)(b + ctrlSize);
   }
 }
 UnlockDisplay(pPEXIMCurrDpy); 
}
 /* file-poc099 -*/
void
pcell_array3(parallelogram, colr_array)
Pparal		*parallelogram;	/* cell parallelogram: [0]=P; [1]=Q; [2]=R */
Ppat_rep	*colr_array;	/* colour array */
{
 pexCellArray *ocptr;
 int i, size;

 PEXIMDebug("pcell_array3 called  - PEXOCCellArray \n");
 LockDisplay(pPEXIMCurrDpy);
 size = colr_array->dims.size_x * colr_array->dims.size_y * sizeof(CARD16);
 MACOCDATA( PEXOCCellArray,pexCellArray,PAD4(size),ocptr);
 bcopy((char *)parallelogram, (char *)&(ocptr->point1),3*sizeof(pexCoord3D)); 
 ocptr->dx = colr_array->dims.size_x;
 ocptr->dy = colr_array->dims.size_y;
 CopyPintToCARD16(colr_array->colr_array, (CARD16 *)(ocptr+1), size );

 UnlockDisplay(pPEXIMCurrDpy); }
 /* file-poc100 -*/
void
pcell_array(rectangle, colr_array)
Prect		*rectangle;	/* cell rectangle	*/
Ppat_rep	*colr_array;	/* colour array */
{
 pexCellArray2D *ocptr;
 int size;

 PEXIMDebug("pcell_array called  - PEXOCCellArray2D \n");
 LockDisplay(pPEXIMCurrDpy);
 size = colr_array->dims.size_x * colr_array->dims.size_y * sizeof(CARD16);
 MACOCDATA( PEXOCCellArray2D,pexCellArray2D,PAD4(size),ocptr);
 bcopy((char *)rectangle, (char *)&(ocptr->point1), 2*sizeof(pexCoord2D));
 ocptr->dx = colr_array->dims.size_x;
 ocptr->dy = colr_array->dims.size_y;
 CopyPintToCARD16(colr_array->colr_array, (CARD16 *)(ocptr+1), size );

 UnlockDisplay(pPEXIMCurrDpy); }

 /* file-poc101 -*/
void
pcell_array3_plus(paral, dim, colour_model, colrs)
Pparal		*paral;	/* cell parallelogram: [0]=P; [1]=Q; [2]=R */
Pint_size    *dim;                   /* dimension of cell: DX and DY */
Pint    colour_model;           /* colour model */
Pcoval  *colrs;                 /* colour array   */
{
 pexExtCellArray *ocptr;
 int size, iColor;

 PEXIMDebug("pcell_array3_plus called  - PEXOCExtCellArray \n");
 LockDisplay(pPEXIMCurrDpy);

 size =  (dim->size_x*dim->size_y)
	* ((colour_model==PINDIRECT)?sizeof(long):sizeof(pexCoord3D));
 MACOCDATA( PEXOCExtCellArray,pexExtCellArray,size,ocptr);
 ocptr->colourType = colour_model;
 bcopy((char *)paral, (char *)&(ocptr->point1), 3*sizeof(pexCoord3D));
 ocptr->dx = dim->size_x;
 ocptr->dy = dim->size_y;
 if (colour_model==PINDIRECT) {
   CopyDown(dim->size_x*dim->size_y, (long *)colrs, sizeof(Pcoval), 
	    (long *)(ocptr+1), sizeof(long));
 } else {
   bcopy((char *)colrs, (char *)(ocptr+1), size );
 }
 UnlockDisplay(pPEXIMCurrDpy); 
}

 /* file-poc102 -*/
void
pgdp3(point_list, gdp3_id, gdp_data)
Ppoint_list3	*point_list;
Pint		gdp3_id;	/* gdp function identifier	*/
Pgdp_data3	*gdp_data;	/* data record pointer	*/
{
 pexGdp *ocptr;
 int size;
 char *d;

 PEXIMDebug("pgdp3 called  - PEXOCGdp \n");
 LockDisplay(pPEXIMCurrDpy);
 if ( gdp3_id > 0 ) return;
 size = point_list->num_points*sizeof(pexCoord3D)
        + PAD4(gdp_data->unsupp.size);
 MACOCDATA( PEXOCGdp,pexGdp,size,ocptr);
 if (pPEXIMForeignServer != 0) {
   if (pPEXIMForeignServer < 0) {  /* need to inquire list of avail. */
     pPEXIMForeignServer = 0;
     pPEXIMBaseGSE = 0;
     pPEXIMBaseGDP = 0;
     pPEXIMBaseGDP3 = 0;
   } else {
     return;
   }
 }
 ocptr->gdpId = gdp3_id;
 ocptr->numPoints = point_list->num_points;
 ocptr->numBytes = gdp_data->unsupp.size;
 bcopy((char *)point_list->points, (char *)(ocptr+1), 
       point_list->num_points*sizeof(pexCoord3D));
 d = (char *)(ocptr+1)  + point_list->num_points*sizeof(pexCoord3D);
 bcopy((char *)gdp_data->unsupp.data, d, gdp_data->unsupp.size);

 UnlockDisplay(pPEXIMCurrDpy); 
}
 /* file-poc103 -*/
void
pgdp(point_list, gdp_id, gdp_data)
Ppoint_list	*point_list;	/* list of points */
Pint	gdp_id;		/* gdp function identifier	*/
Pgdp_data	*gdp_data;	/* data record pointer	*/
{
 pexGdp2D *ocptr;
 int size;
 char *d;

 PEXIMDebug("pgdp called  - PEXOCGdp2D \n");
 LockDisplay(pPEXIMCurrDpy);
 size = point_list->num_points*sizeof(pexCoord2D)
	 + PAD4(gdp_data->unsupp.size);
 MACOCDATA( PEXOCGdp2D,pexGdp2D,size,ocptr);
 if (pPEXIMForeignServer != 0) {
   if (pPEXIMForeignServer < 0) {  /* need to inquire list of avail. */
     pPEXIMForeignServer = 0;
     pPEXIMBaseGSE = 0;
     pPEXIMBaseGDP = 0;
     pPEXIMBaseGDP3 = 0;
   } else {
     return;
   }
 }
 ocptr->gdpId = gdp_id;
 ocptr->numPoints = point_list->num_points;
 ocptr->numBytes = gdp_data->unsupp.size;
 bcopy((char *)point_list->points, (char *)(ocptr+1), 
       point_list->num_points*sizeof(pexCoord2D));
 d = (char *)(ocptr+1)  + point_list->num_points*sizeof(pexCoord2D);
 bcopy((char *)gdp_data->unsupp.data, d, gdp_data->unsupp.size);

 UnlockDisplay(pPEXIMCurrDpy); 
}
