/*		@(#)peximint.h	1.9 Stardent 91/07/19	*/
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
 * peximint.h - PEXIM internal header file.
 *
 * typedefs - always begin PEXIM...
 * globals  - always begin pPEXIM...
 * functions prototypes for utilities.
 */

/*
 * Renderer shadow structure - remove what PEXIM does not care about.
 */
typedef struct {
  XID		viewTable;            
  XID		colorTable;           
  XID		depthCueTable;       
  XID		lightTable;           
  XID		colorApproxTable;     
  XID		textFontTable;        
  XID		highlightInclusion;   
  XID		highlightExclusion;   
  XID		invisibilityInclusion;
  XID		invisibilityExclusion;
  short 	HLHSRMode;	/* zbuffering ?			*/
  Plimit3 	NPCSubvolume;	/* part of 0..1 cube we want	*/
  Plimit3 	viewport;	/* where in window NPC space is */
  CARD8         useDrawableVP;	/* use all drawable for NPC	*/
} PEXIMRendererShadow;

#define INIT_RDR_SHADOW(rs) { rs->viewTable = (XID)0; \
			      rs->colorTable = (XID)0; \
			      rs->depthCueTable = (XID)0; \
			      rs->lightTable = (XID)0; \
			      rs->colorApproxTable = (XID)0; \
			      rs->textFontTable = (XID)0; \
				rs->highlightInclusion = (XID)0; \
				rs->highlightExclusion = (XID)0; \
				rs->invisibilityInclusion = (XID)0; \
				rs->invisibilityExclusion = (XID)0; \
				rs->HLHSRMode = 1; \
				rs->NPCSubvolume.x_min = 0.0; \
			      rs->NPCSubvolume.x_max = 1.0; \
			      rs->NPCSubvolume.y_min = 0.0; \
			      rs->NPCSubvolume.y_max = 1.0; \
			      rs->NPCSubvolume.z_min = 0.0; \
			      rs->NPCSubvolume.z_max = 1.0; \
			      rs->viewport.x_max = 1.0; \
			      rs->viewport.y_min = 0.0; \
			      rs->viewport.y_max = 1.0; \
			      rs->viewport.z_min = 0.0; \
			      rs->viewport.z_max = 1.0; \
			      rs->useDrawableVP = 1; }

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


#define PAD4(a) (((a)+3)&(~3))

#define MACOC(oc, structName, ocptr) { \
	register pexRenderOutputCommandsReq *req = \
	  (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req; \
        if ( (req->reqType == pPEXIMOpCode ) \
           && ((req->opcode == PEX_RenderOutputCommands ) \
	       || (req->opcode == PEX_StoreElements )) \
           && ((pPEXIMCurrDpy->bufptr + sizeof(structName)) <= pPEXIMCurrDpy->bufmax)) { \
	   ocptr = (structName *)pPEXIMCurrDpy->bufptr; \
           req->length += sizeof(structName)>>2; \
	   pPEXIMCurrDpy->bufptr += sizeof(structName); \
	   req->numCommands++; \
	} else { \
	   ocptr = (structName *)GetROC(pPEXIMCurrDpy, sizeof(structName)); \
	} \
	ocptr->head.elementType = oc; \
	ocptr->head.length = (sizeof(structName))>>2;  }

#define MACOCDATA(oc, structName, size, ocptr) { \
	register pexRenderOutputCommandsReq *req = \
	  (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req; \
        if ( (req->reqType == pPEXIMOpCode ) \
           && ((req->opcode == PEX_RenderOutputCommands ) \
	       || (req->opcode == PEX_StoreElements )) \
           && ((pPEXIMCurrDpy->bufptr + (sizeof(structName)+(size))) <= pPEXIMCurrDpy->bufmax)) { \
	   ocptr = (structName *)pPEXIMCurrDpy->bufptr; \
           req->length += (sizeof(structName)+(size))>>2; \
	   pPEXIMCurrDpy->bufptr += (sizeof(structName)+(size)); \
	   req->numCommands++; \
	} else { \
	   ocptr = (structName *)GetROC(pPEXIMCurrDpy, sizeof(structName)+size); \
	} \
	ocptr->head.elementType = oc; \
	ocptr->head.length = (sizeof(structName)+(size))>>2;  }

#define MACSOC(oc, structName, pfield, value) { \
        structName *ocptr; \
	register pexRenderOutputCommandsReq *req = \
	  (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req; \
        if ( (req->reqType == pPEXIMOpCode ) \
           && ((req->opcode == PEX_RenderOutputCommands ) \
	       || (req->opcode == PEX_StoreElements )) \
           && ((pPEXIMCurrDpy->bufptr + sizeof(structName)) <= pPEXIMCurrDpy->bufmax)) { \
	   ocptr = (structName *)pPEXIMCurrDpy->bufptr; \
           req->length += sizeof(structName)>>2; \
	   pPEXIMCurrDpy->bufptr += sizeof(structName); \
	   req->numCommands++; \
	} else { \
	   ocptr = (structName *)GetROC(pPEXIMCurrDpy, sizeof(structName)); \
	} \
	ocptr->head.elementType = oc; \
	ocptr->head.length = (sizeof(structName))>>2; \
	ocptr->pfield = value; }

#define MACCOLOC(oc,structName,colour) { \
	structName *ocptr; \
	int size;  \
	long *i; float *f; \
	register pexRenderOutputCommandsReq *req =  \
	  (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req;  \
        if (colour->type < PINDIRECT || colour->type > PMODEL_HLS ) return;  \
        size = sizeof(structName) +pPEXIMColorSizes[colour->type];  \
        if ( (req->reqType == pPEXIMOpCode )  \
           && ((req->opcode == PEX_RenderOutputCommands ) \
	       || (req->opcode == PEX_StoreElements )) \
           && ((pPEXIMCurrDpy->bufptr + size) <= pPEXIMCurrDpy->bufmax)) {  \
	   ocptr = (structName *)pPEXIMCurrDpy->bufptr;  \
           req->length += size>>2;  \
	   pPEXIMCurrDpy->bufptr += size;  \
	   req->numCommands++;  \
	} else {  \
	   ocptr = (structName *)GetROC(pPEXIMCurrDpy, size);  \
	}  \
	ocptr->head.elementType = oc;  \
	ocptr->head.length = size>>2;  \
        switch (colour->type) {  \
        case PINDIRECT:  \
	ocptr->colourSpec.colourType =  PEXIndexedColour; \
	i = (long *)(((char *)ocptr) + sizeof(structName));  \
	*i = colour->val.ind;  \
          break;  \
        case PMODEL_RGB:  \
	ocptr->colourSpec.colourType = PEXRgbFloatColour; \
	f = (float *)(((char *)ocptr) + sizeof(structName));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_CIELUV:  \
	ocptr->colourSpec.colourType = PEXCieFloatColour; \
	f = (float *)(((char *)ocptr) + sizeof(structName));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_HSV:  \
	ocptr->colourSpec.colourType = PEXHsvFloatColour; \
	f = (float *)(((char *)ocptr) + sizeof(structName));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
        case PMODEL_HLS:  \
	ocptr->colourSpec.colourType = PEXHlsFloatColour; \
	f = (float *)(((char *)ocptr) + sizeof(structName));  \
	*f++ = colour->val.general.x; *f++ = colour->val.general.y;  \
	 *f = colour->val.general.z;  \
          break;  \
	default:; /* we already checked this! */  \
        }  \
  } 

#define MACLSTOC(oc,structName,count,perElementSize,ocptr) { \
	int size = sizeof(structName) + count*perElementSize;  \
	register pexRenderOutputCommandsReq *req =  \
	  (pexRenderOutputCommandsReq *) pPEXIMCurrDpy->last_req;  \
        if ( (req->reqType == pPEXIMOpCode )  \
           && ((req->opcode == PEX_RenderOutputCommands ) \
	       || (req->opcode == PEX_StoreElements )) \
           && ((pPEXIMCurrDpy->bufptr + size) <= pPEXIMCurrDpy->bufmax)) {  \
	   ocptr = (structName *)pPEXIMCurrDpy->bufptr;  \
           req->length += size>>2;  \
	   pPEXIMCurrDpy->bufptr += size;  \
	   req->numCommands++;  \
	} else {  \
	   ocptr = (structName *)GetROC(pPEXIMCurrDpy, size);  \
	}  \
	ocptr->head.elementType = oc;  \
	ocptr->head.length = size>>2; }


/*
 * To fit into the PHIGS mentality, the application's Renderer Identifier
 * is given to PEXIM.  We must then store this in a table.  In practice,
 * it is assumed that the application will not be switching Renderers too
 * often.
 *
 * Anyway, 
 */
typedef struct {
  long     wkid;
  Display *dpy;
  Drawable w;
  XID      rdr;
  PEXIMRendererShadow *info;
  CARD8    opcode;
  CARD8    state;
  short    structIndex;
} PEXIMRendererData;

/*
 * These are PEXIM global variables.
 * 
 */
#ifdef PUTILS_C
#define PGLOBAL 
#else
#define PGLOBAL extern
#endif

PGLOBAL long     pPEXIMCurrWKID;
PGLOBAL int      pPEXIMCurrWKIdx;/* only valid when CurrWKID != NO_WKID */
PGLOBAL Display *pPEXIMCurrDpy;  /* only valid when CurrWKID != NO_WKID */
PGLOBAL CARD8    pPEXIMOpCode;   /* only valid when CurrWKID != NO_WKID */
PGLOBAL XID      pPEXIMCurrRdr;  /* only valid when CurrWKID != NO_WKID */
PGLOBAL Window   pPEXIMCurrWin;  /* only valid when CurrWKID != NO_WKID */
PGLOBAL int      pPEXIMState;    /* only valid when CurrWKID != NO_WKID */

PGLOBAL int                pPEXIMRendererCount;
PGLOBAL PEXIMRendererData **pPEXIMRendererTable;

PGLOBAL int pPEXIMDebug
#ifdef PUTILS_C
 = 0
#endif
;

#define PEXIMDebug(s) {if (pPEXIMDebug) printf(s);}

/*
 * These are sort of gestating. The right way to do this is to inquire
 * the list from foeign servers and use the higehst/lowest as an offset
 * although this could potentially leads to bugs, so probably the best
 * way is to use a pair of arrays for the each of the GDPs, GSEs and
 * for each GDP/GSE look to see if it has been used before and then 
 */
PGLOBAL int pPEXIMForeignServer;
PGLOBAL int pPEXIMBaseGSE;
PGLOBAL int pPEXIMBaseGDP;
PGLOBAL int pPEXIMBaseGDP3;

/*
 * This is really being indexed by PINDIRECT, PRGB, PCIE, PHVS, PHLS
 * from the gcolr - see phigs.h
 */
PGLOBAL int pPEXIMColorSizes[]
#ifdef PUTILS_C
 = {
        sizeof(pexIndexedColour),	 /* PEXIndexedColour	0 */
        sizeof(pexRgbFloatColour),	 /* PEXRgbFloatColour	1 */
        sizeof(pexCieColour),		 /* PEXCieFloatColour	2 */
        sizeof(pexHsvColour),		 /* PEXHsvFloatColour	3 */
        sizeof(pexHlsColour),		 /* PEXHlsFloatColour	4 */
      }
#endif
;

PGLOBAL int pPEXIMFacetSizes[2][4]
#ifdef PUTILS_C
 = {
	0, 				 /* NONE - colour model = rgb */
	sizeof(pexRgbFloatColour),	 /*  PFACET_COLOUR     floats */
	sizeof(pexVector3D),      	 /*  PFACET_NORMAL            */
				 	 /*  PFACET_COLOUR_NORMAL     */
	sizeof(pexRgbFloatColour)+sizeof(pexVector3D),
	0, 				 /* NONE - colour model = ind.*/
	sizeof(pexIndexedColour),	 /*  PFACET_COLOUR    integer */
	sizeof(pexVector3D),		 /*  PFACET_NORMAL            */
					 /*  PFACET_COLOUR_NORMAL     */
        sizeof(pexIndexedColour)+sizeof(pexVector3D)
	}
#endif
;

PGLOBAL int pPEXIMVertexSizes[2][4]
#ifdef PUTILS_C
 = {
	sizeof(pexCoord3D),			/* PVERT_COORD  rgb float   */
 sizeof(pexCoord3D)+sizeof(pexRgbFloatColour),	/* PVERT_COORD_COLOUR       */
 sizeof(pexCoord3D)+sizeof(pexVector3D),	/* PVERT_COORD_NORMAL       */
						/* PVERT_COORD_COLOUR_NORMAL*/
 sizeof(pexCoord3D)+sizeof(pexRgbFloatColour)+sizeof(pexVector3D),
	sizeof(pexCoord3D),			/* PVERT_COORD  rgb float   */
 sizeof(pexCoord3D)+sizeof(pexIndexedColour),	/* PVERT_COORD_COLOUR       */
 sizeof(pexCoord3D)+sizeof(pexVector3D),	/* PVERT_COORD_NORMAL       */
						/* PVERT_COORD_COLOUR_NORMAL*/
 sizeof(pexCoord3D)+sizeof(pexIndexedColour)+sizeof(pexVector3D)
      }
#endif
;

PGLOBAL int pPEXIMFacetAttrs[]
#ifdef PUTILS_C
 = {
	0, 				 /* NONE -                    */
	PEXGAColour,			 /*  PFACET_COLOUR            */
	PEXGANormal,			 /*  PFACET_NORMAL            */
	PEXGAColour|PEXGANormal	 	 /*  PFACET_COLOUR_NORMAL     */
	}
#endif
;

PGLOBAL int pPEXIMVertexAttrs[]
#ifdef PUTILS_C
 = {
	0, 				 /*  PVERT_COORD                   */
	PEXGAColour,			 /*  PVERT_COORD_COLOUR            */
	PEXGANormal,			 /*  PVERT_COORD_NORMAL            */
	PEXGAColour|PEXGANormal	 	 /*  PVERT_COORD_COLOUR_NORMAL     */
	}
#endif
;

/*************************************************************************
 * PEXIM State global
 *
 * these are mutually incompatable states. 
 * That is to say one acnnot have both an open structure and be rendering
 * at the same time.
 * moved to pexim.h
 * define PEXIM_IDLE 0
 * define PEXIM_RENDERING 1
 * define PEXIM_S_OPEN 2
 */

typedef struct {
  Pint structID;
  Pint wkid;
  XID  resourceID;
  int  otherWKID; /* integer to another entry */
} PEXIMStructTableEntry;
PGLOBAL int pPEXIMMaxStruct;
PGLOBAL PEXIMStructTableEntry *pPEXIMStructTable;
PGLOBAL int pPEXIMStructAllocced;

/*
 * function prototypes for utility routines. in putils.c
 */
char *GetROC(Display *dpy, int size);
void CopyDown(int count, long *fromAddr, int fromStep,long *toAddr,int toStep);
void CopyMerge2(int count, long *coords, long *other, long *toAddr);
void CopyMerge3(int count, long *coords, long *other, long *self, long *toAddr);
void CopyPintToCARD16( Pint *fromAddr, CARD16 *toAddr, int count );

/*
 * other business
 */
#if defined(SVR4_0) || defined(SVR4) || defined(hpux)
#include <string.h>
#define bcopy(b1,b2,len) memmove(b2, b1, len)
#define bzero(b,len) memset(b, 0, len)
#define bcmp(b1,b2,len) memcmp(b1, b2, len)
#endif
