/*
 * Copyright IBM Corporation 1987,1988,1989,1991
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliIO.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliIO.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliIO.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#include "X.h"
#define NEED_EVENTS
#define NEED_REPLIES
#include "Xproto.h"
#include "resource.h"
#include "servermd.h"
#include "window.h"
#include "font.h"
#include "miscstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "mistruct.h"
#include "mibstore.h"
#include "scrnintstr.h"

#include "OScompiler.h"
#include "pliScreen.h"
#include "OScursor.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "pliVideo.h"

#include "ibmTrace.h"

#include "xpliDevStr.h"

miBSFuncRec pliBSFuncRec = {
    ppcSaveAreas,
    ppcRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

/* External Variables */
extern ScreenInfo screenInfo ;
extern int pliDisplayTubeType ;
extern ScreenRec pliScreenRec ;
extern ibmPerScreenInfo pliScreenInfoStruct;
extern VisualRec pliVisuals[] ;
/* SKYWAY Globals */
extern caddr_t SKYWAY_OFFSET[6];

/* globals defined here */
ibmPerScreenInfo *pliIbmScreen;
unsigned long pliSegAddr;  /* used by pli code via newmacros.h */

/* locals defined here */
static int HardwareReady = 0 ;
static t_xpliDevStrP xpliDevStrP;
static ScreenPtr lclScreenP;

Bool
pliScreenInit( index, pScreen, argc, argv )
     register const int index ;
     register ScreenPtr const pScreen ;
     register int const argc ;		/* these two may NOT be changed */
     register char * const * const argv ;
{ static char plikextData[8];
  extern void pliCommonScreenInit();
	static int been_here = 0 ;


	TRACE( ( "pliScreenInit(index=%d,pScreen=0x%x,argc=%d,argv=0x%x)\n",
		index, pScreen, argc, argv ) ) ;

	if ( !been_here ) {
	  been_here = TRUE ;
	  if (plikext(1,plikextData)) return FALSE;
	  pliSegAddr = SKYWAY_OFFSET[0];
	  xpliDevStrP->dispRAM = SKYWAY_OFFSET[0] + 0xA0000;
	  xpliDevStrP->dispROM = 0;
	  xpliDevStrP->dispCTL = 0;;
	  xpliDevStrP->dispIO =  SKYWAY_OFFSET[0] + 0x3C0;
	  lclScreenP = pScreen;
	}
	if ( !HardwareReady )
		HardwareReady = pliScreenInitHW( index ) ;

	pliVisuals[0].class = GrayScale ;
	pliCommonScreenInit( pScreen, index, &pliScreenRec ) ;
	pliCursorInit( index ) ;

	return HardwareReady ;
}

void
pliCommonScreenInit( pScreen, index, pScrnProto )
     register ScreenPtr const pScreen ;
     register const int index ;
     ScreenPtr const pScrnProto ;
{
  extern void pliDefineDefaultColormapColors();
  register int i ;
  VisualPtr pVis ;
  ppcScrnPriv *pScrnPriv ;
  int savemyNum = pScreen->myNum;
  int saveWindowPrivateLen = pScreen->WindowPrivateLen;
  unsigned *saveWindowPrivateSizes = pScreen->WindowPrivateSizes;
  unsigned savetotalWindowSize = pScreen->totalWindowSize;
  int saveGCPrivateLen = pScreen->GCPrivateLen;
  unsigned *saveGCPrivateSizes = pScreen->GCPrivateSizes;
  unsigned savetotalGCSize = pScreen->totalGCSize;
  DevUnion *savedevPrivates = pScreen->devPrivates;
  
  TRACE( ( "pliCommonScreenInit(pScreen=0x%x,index=%d,pScrnProto=0x%x)\n",
	  pScreen, index, pScrnProto ) ) ;
  
  *pScreen = *pScrnProto ; /* Copy The Prototype Structure */
  
  pScreen->devPrivates = savedevPrivates;
  pScreen->myNum = savemyNum;
  pScreen->WindowPrivateLen = saveWindowPrivateLen;
  pScreen->WindowPrivateSizes = saveWindowPrivateSizes;
  pScreen->totalWindowSize = savetotalWindowSize;
  pScreen->GCPrivateLen = saveGCPrivateLen;
  pScreen->GCPrivateSizes = saveGCPrivateSizes;
  pScreen->totalGCSize = savetotalGCSize;
  
  if (!mfbAllocatePrivates(pScreen, (int*)NULL, (int*)NULL))
    return ;
  
  pScrnPriv = (ppcScrnPriv *) ( pScreen->devPrivate ) ;
  pScreen->myNum = index;
  pScrnPriv->devPrototypeGC->pScreen =
    pScrnPriv->pixmap.drawable.pScreen =
      pScreen ;
  
  /* Set up the visuals */
  pVis = pScrnProto->visuals ;
  
  { /* Set The Standard Colormap Stuff */
    ColormapPtr pColormap ;
    
    /* "dix" Colormap Stuff */
    CreateColormap( pScreen->defColormap =
		   (Colormap) FakeClientID( 0 ),
		   pScreen, pVis, &pColormap, AllocNone, 0 ) ;
    pColormap->pScreen = pScreen ;
    
    /* "pli" Colormap Stuff */
    pliDefineDefaultColormapColors( pVis, pColormap ) ;
    ppcInstallColormap( pColormap ) ;
    ibmAllocBlackAndWhitePixels( pColormap ) ;
  }
  miInitializeBackingStore (pScreen, &pliBSFuncRec);
  TRACE(("ppcCommonScreenInit returns\n"));
  return ;
}


#define LOWINTEN 0x8000
#define HI_INTEN 0xFFFF

void
pliDefineDefaultColormapColors( pVisual, pCmap )
     register VisualPtr pVisual ;
     register ColormapPtr pCmap ;
{
  register int i, j ;
  
  /* Read The Note about classes in <X11/X.h> */
  if ( pVisual->class & 1 ) /* Dynamic classes have odd values */
    if ( pVisual->class == DirectColor ) /* Can't Handle Yet */
      ibmInfoMsg(
		 "pliDefineDefaultColormapColors: No defaults for DirectColor Visual\n" ) ;
    else /* Must be PseudoColor or GrayScale */
      switch ( i = pVisual->ColormapEntries ) {
      case 2: /* Monochrome */
	pCmap->red[0].co.local.red   = 0 ;
	pCmap->red[0].co.local.green = 0 ;
	pCmap->red[0].co.local.blue  = 0 ;
	pCmap->red[1].co.local.red   = HI_INTEN ;
	pCmap->red[1].co.local.green = HI_INTEN ;
	pCmap->red[1].co.local.blue  = HI_INTEN ;
	break ;
      case 8: /* Gray scale with inking inversion for PLI */
	j = HI_INTEN;
	while ( --i > 0 ) 
	  {
	    pCmap->red[i].co.local.red   = 
	      pCmap->red[i].co.local.green = 
		pCmap->red[i].co.local.blue  = j ;
	    j -= 1<<10;
	  }
	pCmap->red[0].co.local.red   = 
	  pCmap->red[0].co.local.green = 
	    pCmap->red[0].co.local.blue  = 0 ;
	break ;
      default:
	ErrorF(
	       "pliDefineDefaultColormapColors: Bad num of map entries\n" ) ;
	/* But no big deal ; who cares? */
	break ;
      } /* end switch */
  
  return ;
}

/*ARGSUSED*/
Bool
pliScreenClose( index, pScreen )
register const int index ;
register ScreenPtr const pScreen ;
{
TRACE( ( "pliScreenClose(index=%d,pScreen=0x%x)\n", index, pScreen ) ) ;
ppcCommonScreenClose( pScreen, index, &pliScreenRec ) ;
if ( HardwareReady ) {
  plikext(0);
  xpliDevStrP->dispRAM = 0;
  xpliDevStrP->dispROM = 0;
  xpliDevStrP->dispCTL = 0;
  xpliDevStrP->dispIO  = 0;
  pliCloseHW( index ) ;
  HardwareReady = 0 ;
}

return 1 ;
}

int
InitVGA(IVxpliDevStrP, argc, argv)
     t_xpliDevStrP IVxpliDevStrP; int argc; char *argv[];
{ int k, j, i; PixmapFormatRec *pliFormatP;
  /* save the xpliDevStrP */
  xpliDevStrP = IVxpliDevStrP;
  /* copy the ibmPerScreenInfo struct for pli */
  pliIbmScreen = (ibmPerScreenInfo *) xalloc(sizeof(ibmPerScreenInfo));
  *pliIbmScreen = pliScreenInfoStruct;
  k = 0;
  for (j=0, pliFormatP=pliScreenInfoStruct.ibm_ScreenFormats; 
       j<pliScreenInfoStruct.ibm_NumFormats; j++, pliFormatP++) {
    for (i=0; i<screenInfo.numPixmapFormats; i++) {
      if ((screenInfo.formats[i].depth == pliFormatP->depth) &&
	  (screenInfo.formats[i].bitsPerPixel == pliFormatP->bitsPerPixel) &&
	  (screenInfo.formats[i].scanlinePad == pliFormatP->scanlinePad))
	break; }
    if (i >= screenInfo.numPixmapFormats) {
      k++;
      i = screenInfo.numPixmapFormats;
      screenInfo.formats[i].depth = pliFormatP->depth;
      screenInfo.formats[i].bitsPerPixel = pliFormatP->bitsPerPixel;
      screenInfo.formats[i].scanlinePad == pliFormatP->scanlinePad;
      screenInfo.numPixmapFormats++; } }
  /* now we can call AddScreen, NB xpliSBScrnInit called, sets up screenP */
  if (AddScreen(pliScreenInit, argc, argv) < 0) {
    /* don't need the formats if cannot add the screen */
    screenInfo.numPixmapFormats -= k;
    return -1; }
  /* remember the screen we added for xpli */
  xpliDevStrP->screenP = lclScreenP;
  return 0;
}  
