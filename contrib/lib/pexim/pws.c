#ifdef SCCS
static char sccsid[]="@(#)pws.c	1.5 Stardent 91/06/12";
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
/*************************************************************************
 * pws.c - the workstation 
 *
 * pset_light_src_rep(ws, index, rep)
 * pset_view_rep3( ws, index, rep)
 * pset_hlhsr_mode( ws, mode)
 * pset_colr_rep( ws, index, rep)
 * pset_dcue_rep( ws, index, rep)
 */

#include <pexim.h>
#include "PEX.h"
#include "PEXproto.h"
#include "peximint.h"

#define CHECK_WS(ws,rdata) { \
  int i; \
  rdata = (PEXIMRendererData *)0; \
    for (i = 0; i < pPEXIMRendererCount; i++ ) \
      if ((pPEXIMRendererTable[i])->wkid == (ws)) { \
	rdata = pPEXIMRendererTable[i];  } }

#define COPY_COLOR(pcolrSpec,gcolr) { \
		long *i; float *f; \
	  pexColourSpecifier *cs = (pexColourSpecifier *)&(pcolrSpec); \
          Pgcolr *colour = (Pgcolr *)&(gcolr); \
        switch (colour->type) {  \
        case PINDIRECT:  \
	  cs->colourType =  PEXIndexedColour; \
	i = (long *)(((char *)cs) + sizeof(pexColourSpecifier));  \
	*i = colour->val.ind;  \
          break;  \
        case PMODEL_RGB:  \
	cs->colourType = PEXRgbFloatColour; \
	f = (float *)(((char *)cs) + sizeof(pexColourSpecifier));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_CIELUV:  \
	cs->colourType = PEXCieFloatColour; \
	f = (float *)(((char *)cs) + sizeof(pexColourSpecifier));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_HSV:  \
	cs->colourType = PEXHsvFloatColour; \
	f = (float *)(((char *)cs) + sizeof(pexColourSpecifier));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_HLS:  \
	cs->colourType = PEXHlsFloatColour; \
	f = (float *)(((char *)cs) + sizeof(pexColourSpecifier));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break; \
        default:cs->colourType =  PEXIndexedColour;  }} 



/*************************************************************************
 * MakeTable - local routine to create a LUT when needed
 */
static int MakeTable(PEXIMRendererData *rdata, pexTableType table_type)
{
  XID tID;
  int status, length, itemMask;

  tID = XAllocID(rdata->dpy);
  status =
    PEXCreateLookupTable(rdata->dpy, rdata->w, tID, table_type);
  if (status != 1) return (status);
  
  switch (table_type) {
  case PEXTextFontLUT:
    rdata->info->textFontTable = tID;
    itemMask =  PEXRDTextFontTable;
    break;
  case PEXColourLUT:
    rdata->info->colorTable = tID;
    itemMask = PEXRDColourTable;
    break;
  case PEXViewLUT:
    rdata->info->viewTable = tID;
    itemMask =  PEXRDViewTable;
    break;
  case PEXLightLUT:
    rdata->info->lightTable = tID;
    itemMask =  PEXRDLightTable;
    break;
  case PEXDepthCueLUT:
    rdata->info->depthCueTable = tID;
    itemMask =  PEXRDDepthCueTable;
    break;
  }

  length = 4;
  status = 
    PEXChangeRenderer(rdata->dpy, rdata->rdr, itemMask, length, &tID);
  if (status != 1) return (status);

  return (0);
}

typedef struct {  /* make this big enough to hold the whole entry */
  pexLightEntry e;
  float         c[3];
} lentry;
/*************************************************************************
 * pset_light_src_rep - 
 */
void
pset_light_src_rep(ws, index, rep)
Pint	        	ws;	/* workstation identifier */
Pint	        	index;	/* light table index */
Plight_src_bundle	*rep;	/* light source representation */	
{
  pexLightEntry *entry;
  PEXIMRendererData *rdata;
  lentry theEntry;
  int size, status;

  CHECK_WS(ws,rdata);
  if (!rdata) PEXIMError();

  if (!rdata->info->lightTable) {
    if (MakeTable(rdata,PEXLightLUT)) PEXIMError();
  }

  entry = (pexLightEntry *)&theEntry;

  switch (rep->type) {
  case PLIGHT_AMBIENT:
    entry->lightType = PEXLightAmbient;
    COPY_COLOR(entry->lightColour,rep->rec.ambient.colr);
    break;
  case PLIGHT_DIRECTIONAL:
    entry->lightType = PEXLightWcsVector;
    entry->direction.x = rep->rec.directional.dir.delta_x;
    entry->direction.y = rep->rec.directional.dir.delta_y;
    entry->direction.z = rep->rec.directional.dir.delta_z;
    COPY_COLOR(entry->lightColour,rep->rec.directional.colr);
    break;
  case PLIGHT_POSITIONAL:
    entry->lightType = PEXLightWcsPoint;
    entry->point.x = rep->rec.positional.pos.x;
    entry->point.y = rep->rec.positional.pos.y;
    entry->point.z = rep->rec.positional.pos.z;
    entry->attenuation1 = rep->rec.positional.coef[0];
    entry->attenuation2 = rep->rec.positional.coef[1];
    COPY_COLOR(entry->lightColour,rep->rec.positional.colr);
    break;
  case PLIGHT_SPOT:
    entry->lightType = PEXLightWcsSpot;
    entry->direction.x = rep->rec.spot.dir.delta_x;
    entry->direction.y = rep->rec.spot.dir.delta_y;
    entry->direction.z = rep->rec.spot.dir.delta_z;
    entry->point.x = rep->rec.spot.pos.x;
    entry->point.y = rep->rec.spot.pos.y;
    entry->point.z = rep->rec.spot.pos.z;
    entry->concentration = rep->rec.spot.exp;
    entry->spreadAngle = rep->rec.spot.angle;
    entry->attenuation1 = rep->rec.spot.coef[0];
    entry->attenuation2 = rep->rec.spot.coef[1];
    COPY_COLOR(entry->lightColour,rep->rec.spot.colr);
    break;
  default: break;
  }

  size = sizeof(pexLightEntry)+pPEXIMColorSizes[entry->lightColour.colourType];
  status = PEXSetTableEntries(rdata->dpy, rdata->info->lightTable, index, 1,
			      size, entry);
  if (status != 1) PEXIMError();
}

/*************************************************************************
 * pset_view_rep3 - 
 */
void
pset_view_rep3( ws, index, rep)
    Pint		ws;	/* workstation id	*/
    Pint		index;	/* view index	*/
    Pview_rep3	*rep;	/* view representation	*/
{
  pexViewEntry *entry, theEntry;
  PEXIMRendererData *rdata;
  int status;

  CHECK_WS(ws,rdata);
  if (!rdata) PEXIMError();

  if (!rdata->info->viewTable) {
    if (MakeTable(rdata,PEXViewLUT)) PEXIMError();
  }

  entry = (pexViewEntry *)&theEntry;

  entry->clipFlags = ((rep->xy_clip==PIND_CLIP)?0x1:0x0) |
    			((rep->back_clip==PIND_CLIP)?0x2:0x0) | 
    			((rep->front_clip==PIND_CLIP)?0x4:0x0);
  entry->clipLimits.minval.x = rep->clip_limit.x_min;
  entry->clipLimits.maxval.x = rep->clip_limit.x_max;
  entry->clipLimits.minval.y = rep->clip_limit.y_min;
  entry->clipLimits.maxval.y = rep->clip_limit.y_max;
  entry->clipLimits.minval.z = rep->clip_limit.z_min;
  entry->clipLimits.maxval.z = rep->clip_limit.z_max;
  bcopy((char *)rep->ori_matrix,(char *)entry->orientation,sizeof(pexMatrix));
  bcopy((char *)rep->map_matrix,(char *)entry->mapping,sizeof(pexMatrix));

  status = PEXSetTableEntries(rdata->dpy, rdata->info->viewTable, index, 1,
			      sizeof(pexViewEntry), entry);
  if (status != 1) PEXIMError();
}

/*************************************************************************
 *
 */
void
pset_hlhsr_mode( ws, mode)
    Pint	ws;	/* workstation id	*/
    Pint	mode;	/* HLHSR mode */
{
  PEXIMRendererData *rdata;
  int status, length, itemMask;
  short t[2];

  CHECK_WS(ws,rdata);
  if (!rdata) PEXIMError();

  switch (mode) {
  case PHIGS_HLHSR_MODE_NONE: t[0] = PEXHlhsrOff;  break;
  case PHIGS_HLHSR_MODE_ZBUFF: t[0] = PEXHlhsrZBuffer; break;
  case PHIGS_HLHSR_MODE_PAINTERS: t[0] = PEXHlhsrPainters; break;
  case PHIGS_HLHSR_MODE_SCANLINE: t[0] = PEXHlhsrScanline; break;
  case PHIGS_HLHSR_MODE_LINE_ONLY: t[0] = PEXHlhsrHiddenLineOnly; break;
  default:t[0] = PEXHlhsrZBuffer;
  }

  itemMask = PEXRDHlhsrMode;
  length = 4;
  status = 
    PEXChangeRenderer(rdata->dpy, rdata->rdr, itemMask, length, t);
  if (status != 1) PEXIMError();
}

typedef struct {
  pexColourSpecifier cs;
  float c[3];
} centry;

void
pset_colr_rep( ws, index, rep)
    Pint	ws;	/* workstation identifier	*/
    Pint	index;	/* colour bundle index	*/
    Pcolr_rep	*rep;	/* colour representation pointer	*/
{  
  pexColourSpecifier *entry;
  PEXIMRendererData *rdata;
  centry theEntry;
  int size, status;

  CHECK_WS(ws,rdata);
  if (!rdata) PEXIMError();

  if (!rdata->info->colorTable) {
    if (MakeTable(rdata,PEXColourLUT)) PEXIMError();
  }

  theEntry.cs.colourType = PEXRgbFloatColour;
  theEntry.c[0] = rep->rgb.red;
  theEntry.c[1] = rep->rgb.green;
  theEntry.c[2] = rep->rgb.blue;

  status = PEXSetTableEntries(rdata->dpy, rdata->info->colorTable, index, 1,
			      sizeof(centry), &theEntry);
  if (status != 1) PEXIMError();
}


typedef struct {
 pexDepthCueEntry entry;
 float c[3];
} dentry;

/*************************************************************************
 * pset_dcue_rep -
 */
void
pset_dcue_rep( ws, index, rep)
    Pint	ws;	/* workstation identifier	*/
    Pint	index;	/* depth cue bundle index	*/
    Pdcue_bundle *rep;	/* depth cue representation pointer	*/
{
  pexDepthCueEntry *entry;
  PEXIMRendererData *rdata;
  dentry theEntry;
  int size, status;

  CHECK_WS(ws,rdata);
  if (!rdata) PEXIMError();

  if (!rdata->info->depthCueTable) {
    if (MakeTable(rdata,PEXDepthCueLUT)) PEXIMError();
  }

  entry = (pexDepthCueEntry *)&theEntry;

  entry->mode = rep->mode;
  entry->frontPlane = rep->ref_planes[0];
  entry->backPlane = rep->ref_planes[1];
  entry->frontScaling = rep->scaling[0];
  entry->backScaling = rep->scaling[1];
  COPY_COLOR(entry->depthCueColour,rep->colr);

  size = sizeof(pexDepthCueEntry) 
         + pPEXIMColorSizes[entry->depthCueColour.colourType];
  status = PEXSetTableEntries(rdata->dpy, rdata->info->depthCueTable, index, 1,
			      size, entry);
  if (status != 1) PEXIMError();
 }





