#ifdef SCCS
static char sccsid[]="@(#)pexim.c	1.14 Stardent 91/08/20";
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
 * pexim.c - the basics of PEXIM
 *
 * pinit()
 * popen_renderer()
 * pclose_renderer()
 * pbegin_rendering()
 * pend_rendering()
 * pbegin_struct()
 * pend_struct()
 * popen_struct()
 * pclose_struct()
 * pempty_struct()
 * pinq_state()
 * pset_struct_ws()
 */

#include <pexim.h>
#include "PEX.h"
#include "PEXproto.h"
#include "peximint.h"

/*************************************************************************
 * pPEXIMClear
 *
 * The PEX 5.0 protocol deos not provide for double buffering of the
 * PEX Renderer except in conjunction with the multi-buffering extension.
 * Since many compnaines have not implemented the multi-buffering extension
 * and automatically swap buffers on EndRendering - ignoring the 5.0 spec
 * we need to be able to decide whether we are clearing the buffer or not.
 */
int pPEXIMClear;

/*************************************************************************
 * pinit - init global data structures
 */
Pint pinit()
{
  pPEXIMRendererCount = 0;
  pPEXIMCurrWKID = PEXIM_NO_WKID;
  pPEXIMForeignServer = -1;  /* needs to be on a per workstation basis ! */
  pPEXIMMaxStruct = 0;
  pPEXIMStructAllocced = 0;
  pPEXIMBaseGSE = 0;
  pPEXIMBaseGDP = 0;
  pPEXIMBaseGDP3 = 0;
  if (getenv("PEXIM_CLEAR")) {
    pPEXIMClear = 1;
  } else {
    pPEXIMClear = 0;
  }
  
  return(POK);
}

/*************************************************************************
 * popen_renderer - "open" or create a Renderer
 *
 * In:
 *    wkid      - application provided handle for Renderer
 *    display   - Display pointer return from XOpenDisplay
 *    window    - Actually, a drawable to be rendered to.
 * Out:
 *    error     - 
 */
Pint popen_renderer( Pint wkid, Display *display, Drawable d)
{
  Pint status;

  short proto_major;
  short proto_minor;
  char	*vendor;
  long release_number;
  long subset_info;

  int 	 exists, opcode, event, err, i;

  XID items[1];
  unsigned long itemMask;
  long length;

  PEXIMRendererData *rdata;
  PEXIMRendererData **ttab;

  /*
   * we need to have one workstation ID 
  if (wkid == -1) return (PEXIMERRBadWKID);

/*
 * Check to see if this wkid is already in use.
 */
  for (i = 0; i < pPEXIMRendererCount; i++ )
    if ((pPEXIMRendererTable[i])->wkid == wkid) 
      return (PEXIMERRAlreadyAllocced);

  pPEXIMRendererCount++;
  if (pPEXIMRendererCount == 1) {
    pPEXIMRendererTable = (PEXIMRendererData **)malloc(pPEXIMRendererCount*sizeof(long *));
  } else {
    pPEXIMRendererTable = (PEXIMRendererData **)realloc(pPEXIMRendererTable,
							pPEXIMRendererCount*sizeof(long *));
  }
  if (!pPEXIMRendererTable) return (PEXIMERRAlloc);

/*
 * Check to make sure PEX is available, and has renderers available.
 */
  exists = XQueryExtension( display, PEX_NAME_STRING, &opcode, &event, &err );
  if (!exists) {  return (PEXIMERRNotPEXServer); }

  PEXGetExtensionInfo( display, PEX_PROTO_MAJOR, PEX_PROTO_MINOR, &proto_major,
    &proto_minor, &vendor, &release_number, &subset_info );
  if ((proto_major != PEX_PROTO_MAJOR) || (subset_info == PEXPhigsWksOnly))
    {  return (PEXIMERRBadPEXServer); }

  if (!strncmp(vendor,"Stardent",strlen("Stardent"))) {
    pPEXIMForeignServer = 0;
  } else {
    pPEXIMForeignServer = 1;
  }

/*
 * Allocate the data structures, get XIDs and allocate a color table at least.
 *
 */

  rdata = (PEXIMRendererData *)malloc(sizeof(PEXIMRendererData));
  if (!rdata) {  return (PEXIMERRAlloc); }

  pPEXIMRendererTable[pPEXIMRendererCount-1] = rdata;

  rdata->wkid = wkid;
  rdata->dpy  = display;
  rdata->w    = d;
  rdata->opcode = opcode;
  rdata->state  = PEXIM_IDLE;

  rdata->info = (PEXIMRendererShadow *)malloc(sizeof(PEXIMRendererShadow));
  if (!rdata->info) {  return (PEXIMERRAlloc); }
  INIT_RDR_SHADOW(rdata->info);

  rdata->info->colorTable = XAllocID(display);
  status =
    PEXCreateLookupTable(display, d, rdata->info->colorTable, PEXColourLUT);
  status--;
  if (status != POK) return (status);
  items[0] = rdata->info->colorTable;
  itemMask = PEXRDColourTable;
  length = 4;

  rdata->rdr = XAllocID(display);
  status = 
    PEXCreateRenderer( display, rdata->rdr, d, itemMask, length, items );
  status--;
  /*
   * make this the current renderer
   */

  pPEXIMCurrWKID = wkid;
  pPEXIMCurrWKIdx = pPEXIMRendererCount-1;
  pPEXIMCurrDpy = display;
  pPEXIMCurrRdr = rdata->rdr;
  pPEXIMCurrWin = d;
  pPEXIMOpCode = opcode;
  pPEXIMState = PEXIM_IDLE;

  return (status);
}

/*************************************************************************
 * pbegin_rendering - issue the PEXBeginRendering Command.
 */
Pint pbegin_rendering(Pint wkid)
{
  Pint status;
  int i, found;

  if ( wkid != pPEXIMCurrWKID ) {
    /*
     * Check to see if this wkid is already in use.
     */
    for (i = 0; i < pPEXIMRendererCount; i++ )
      if ((pPEXIMRendererTable[i])->wkid == wkid) {
	pPEXIMCurrWKID = wkid;
	pPEXIMCurrWKIdx = i;
	pPEXIMCurrDpy = (pPEXIMRendererTable[i])->dpy;
	pPEXIMCurrRdr = (pPEXIMRendererTable[i])->rdr;
	pPEXIMCurrWin = (pPEXIMRendererTable[i])->w;
	pPEXIMOpCode = (pPEXIMRendererTable[i])->opcode;
	pPEXIMState = (pPEXIMRendererTable[i])->state;
	found = 1;
	break;
      }
    if (!found) return (PEXIMERRBadWKID);
  }

  if (pPEXIMState != PEXIM_IDLE) { 
    return (PEXIMERRNotIdle);
  }

  if (pPEXIMClear && pPEXIMForeignServer) 
    XClearWindow(pPEXIMCurrDpy,pPEXIMCurrWin);
  
  status = PEXBeginRendering(pPEXIMCurrDpy, pPEXIMCurrRdr, pPEXIMCurrWin );
  pPEXIMState = PEXIM_RENDERING;
  status--;

  return (status);
}

/*************************************************************************
 * pend_rendering
 */
Pint pend_rendering(Pint wkid, Bool flush)
{
  Pint status;
  int i, found;

  if ( wkid != pPEXIMCurrWKID ) {
    /*
     * Check to see if this wkid is already in use.
     */
    for (i = 0; i < pPEXIMRendererCount; i++ )
      if ((pPEXIMRendererTable[i])->wkid == wkid) {
	pPEXIMCurrWKID = wkid;
	pPEXIMCurrWKIdx = i;
	pPEXIMCurrDpy = (pPEXIMRendererTable[i])->dpy;
	pPEXIMCurrRdr = (pPEXIMRendererTable[i])->rdr;
	pPEXIMCurrWin = (pPEXIMRendererTable[i])->w;
	pPEXIMOpCode = (pPEXIMRendererTable[i])->opcode;
	pPEXIMState = (pPEXIMRendererTable[i])->state;
	found = 1;
	break;
      }
    if (!found) return (PEXIMERRBadWKID);
  }

  if (pPEXIMState != PEXIM_RENDERING) { 
    return (PEXIMERRNotRendering);
  }

  status = PEXEndRendering(pPEXIMCurrDpy, pPEXIMCurrRdr, flush);
  status--;

  (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_IDLE;
  pPEXIMState = PEXIM_IDLE;

  return (status);
}

/*************************************************************************
 * pclose_renderer
 */
Pint pclose_renderer(Pint wkid)
{
  Pint status;
  int i, j, found;

  for (i = 0; i < pPEXIMRendererCount; i++ )
    if ((pPEXIMRendererTable[i])->wkid == wkid) {
      found = 1;
      break;
    }
  if (!found) return (PEXIMERRBadWKID);

/* as defined in plib/include/phigspex.h #define PEXFreeRenderer(_dpy, _xid) */

  PEXResourceIdNoReplyFunc(PEX_FreeRenderer, 
			   (pPEXIMRendererTable[i])->dpy,
			   (pPEXIMRendererTable[i])->rdr );

  /*
   * remove this one from the list of pointers
   */
  pPEXIMRendererCount--;
  for (j = i; j < pPEXIMRendererCount; j++ ) 
    pPEXIMRendererTable[j] = pPEXIMRendererTable[j+1];

  pPEXIMRendererTable = (PEXIMRendererData **)realloc(pPEXIMRendererTable,
				           pPEXIMRendererCount*sizeof(long *));

  if (wkid == pPEXIMCurrWKID) {
    pPEXIMCurrWKID = PEXIM_NO_WKID;
  }
}

/*************************************************************************
 * pbegin_struct() - push the attribute stack
 */
Pint pbegin_struct(Pint s_id)
{
  Pint status;

  if (!pPEXIMCurrWKID) return(PEXIMERRNotRendering);

  status = PEXBeginStructure(pPEXIMCurrDpy, pPEXIMCurrRdr, s_id);
  status--;

  return (status);
}

/*************************************************************************
 * pend_struct() - pop the attribute stack
 */
Pint pend_struct()
{
  Pint status;

  if (!pPEXIMCurrWKID) return(PEXIMERRBadWKID);


/* as defined in plib/include/phigspex.h #define PEXEndStructure(_dpy, _xid) */

  status =  PEXResourceIdNoReplyFunc(PEX_EndStructure, pPEXIMCurrDpy,
				     pPEXIMCurrRdr);
  status--;

  return (status);
}

/*************************************************************************
 * popen_struct() - open a server side structure.
 */
void 
popen_struct(Pint struct_id)
{
  Pint status;
  int i, found;
  
  if (pPEXIMCurrWKID == PEXIM_NO_WKID) { 
    /* return (PEXIMERRNoCurrentWKID); */ PEXIMError();
				  }

  if (pPEXIMState != PEXIM_IDLE) { 
    /* return (PEXIMERRNotIdle); */ PEXIMError();
				  }

  /*
   * O.K. Optimize programmer time here for two reasons.
   * 1) I have no idea of how many structures will be needed in an immediate
   *    mode package.
   * 2) This code get thrown away at PEX 6.0
   */
  found = 0;
  for (i = 0; i < pPEXIMMaxStruct; i++ ) {
    if (pPEXIMStructTable[i].structID == struct_id) {
      found = 1;
      (pPEXIMRendererTable[pPEXIMCurrWKIdx])->structIndex = i;
      break;
    }
  }

  if ( !found ) {
    if (pPEXIMMaxStruct >= pPEXIMStructAllocced) {
      if (pPEXIMMaxStruct > pPEXIMStructAllocced) PEXIMError();
      pPEXIMStructAllocced += 42; /* grow the table by a reasonable size */
      pPEXIMStructTable = (PEXIMStructTableEntry *)realloc(pPEXIMStructTable,
	pPEXIMStructAllocced*sizeof(PEXIMStructTableEntry));
      if (!pPEXIMStructTable) /* return (PEXIMERRAlloc); */ PEXIMError();
    }
    /*
     * use the next table entry, allocate an X resource ID (client side)
     * and put that and the structure ID into the table.
     * Then make it the "current structure" and create the PEX resource.
     */
    pPEXIMStructTable[pPEXIMMaxStruct].resourceID = XAllocID(pPEXIMCurrDpy);
    pPEXIMStructTable[pPEXIMMaxStruct].structID = struct_id;

    (pPEXIMRendererTable[pPEXIMCurrWKIdx])->structIndex = pPEXIMMaxStruct;
    pPEXIMMaxStruct++;

    status = PEXResourceIdNoReplyFunc(PEX_CreateStructure,
	     pPEXIMCurrDpy, pPEXIMStructTable[
			(pPEXIMRendererTable[pPEXIMCurrWKIdx])->structIndex
					      ].resourceID );
    status--;
    if (status != POK) PEXIMError();
  }

  XFlush(pPEXIMCurrDpy);

  (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_STRUCTURE;
  pPEXIMState = PEXIM_STRUCTURE;
}

/*************************************************************************
 * pclose_struct()
 */
void pclose_struct()
{
  if (pPEXIMCurrWKID == PEXIM_NO_WKID) { 
    /* return (PEXIMERRNoCurrentWKID); */ PEXIMError();
				  }

  if (pPEXIMState != PEXIM_STRUCTURE) { 
    /* return (PEXIMERRNotIdle); */ PEXIMError();
				  }

  (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_IDLE;
  pPEXIMState = PEXIM_IDLE;
}
  
/*************************************************************************
 *  pset_struct_ws - set the workstation at which structure will be stored.
 *
 */
Pint pset_struct_ws(Pint wkid)
{
  int i, found = 0;

  if ( wkid != pPEXIMCurrWKID ) {
    /*
     * Check to see if this wkid is already in use.
     */
    for (i = 0; i < pPEXIMRendererCount; i++ )
      if ((pPEXIMRendererTable[i])->wkid == wkid) {
	pPEXIMCurrWKIdx = i;
	pPEXIMCurrWKID = wkid;
	pPEXIMCurrDpy = (pPEXIMRendererTable[i])->dpy;
	pPEXIMCurrRdr = (pPEXIMRendererTable[i])->rdr;
	pPEXIMCurrWin = (pPEXIMRendererTable[i])->w;
	pPEXIMOpCode = (pPEXIMRendererTable[i])->opcode;
	pPEXIMState = (pPEXIMRendererTable[i])->state;
	found = 1;
	break;
      }
    if (!found) return (PEXIMERRBadWKID);
  }

  if (pPEXIMState != PEXIM_IDLE) { 
    return (PEXIMERRNotIdle);
  }

  return (POK);
}

/*************************************************************************
 * Pint pinq_renderer_state(Pint *curr_wkid, Pint *state, Pint *curr_struct);
 */
Pint pinq_renderer_state(Pint *curr_wkid, Pint *state, Pint *curr_struct)
{
  *state = pPEXIMState;
  *curr_wkid = pPEXIMCurrWKID;
  if ((pPEXIMCurrWKID != PEXIM_NO_WKID) && (pPEXIMState == PEXIM_STRUCTURE))
    *curr_struct = 
      pPEXIMStructTable[(pPEXIMRendererTable[pPEXIMCurrWKIdx])->structIndex].structID;
  return (POK);
}

/*************************************************************************
 * ppause_rendering - pause this rendering work on another
 */
Pint ppause_rendering(Pint wkid)
{
  if (pPEXIMCurrWKID != wkid) return(PEXIMERRBadWKID);

  if (pPEXIMState != PEXIM_RENDERING) return(PEXIMERRNotRendering);

  (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_PAUSED;
  pPEXIMCurrWKID = PEXIM_NO_WKID;
/*
 * also, since the OC routines do not check for the correct resource ID...
 */
  XFlush(pPEXIMCurrDpy);

  return (POK);
}

/*************************************************************************
 * ppause_rendering - pause this rendering work on another
 *
 * Error if
 *  current wkid is not idle or paused.
 *  wkid to resume is not paused.
 */
Pint presume_rendering(Pint wkid)
{
  int i, found;

  if (pPEXIMCurrWKID == wkid) {
    /*
     * resuming the current wkid. Should be easy.
     */
    if (pPEXIMState != PEXIM_PAUSED) return (PEXIMERRNotPaused);

    (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_RENDERING;
    pPEXIMState = PEXIM_RENDERING;

    return (POK);
  } else {
    /*
     * Make sure the current workstation is idel or paused
     */
    if (pPEXIMCurrWKID != PEXIM_NO_WKID) {
      if ((pPEXIMState == PEXIM_RENDERING)||(pPEXIMState == PEXIM_STRUCTURE)){
	return (PEXIMERRNotIdle);
      }
    }
    /*
     * find specified wkdi and if paused make it current.
     */
    for (i = 0; i < pPEXIMRendererCount; i++ )
      if ((pPEXIMRendererTable[i])->wkid == wkid) {
	found = 1;
	if ((pPEXIMRendererTable[i])->state == PEXIM_PAUSED) {
	  pPEXIMCurrWKIdx = i;
	  pPEXIMCurrWKID = wkid;
	  pPEXIMCurrDpy = (pPEXIMRendererTable[i])->dpy;
	  pPEXIMCurrRdr = (pPEXIMRendererTable[i])->rdr;
	  pPEXIMCurrWin = (pPEXIMRendererTable[i])->w;
	  pPEXIMOpCode = (pPEXIMRendererTable[i])->opcode;
	  (pPEXIMRendererTable[pPEXIMCurrWKIdx])->state = PEXIM_RENDERING;
	  pPEXIMState = PEXIM_RENDERING;
	} else {
	  return (PEXIMERRNotPaused);
	}
	break;
      } /* end of for loop */
    if (!found) return (PEXIMERRBadWKID);
  }
  return (POK);
}
