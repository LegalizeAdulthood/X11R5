/*
 * Copyright IBM Corporation 1987,1988,1989
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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcCmap.c,v 9.4 89/06/08 20:21:10 jeff Exp $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcCmap.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcCmap.c,v 9.4 89/06/08 20:21:10 jeff Exp $" ;
#endif

/*
 *  Colormap routines for IBM ppc section.
 *  T. Paquin 7/87
 */
#define NEED_EVENTS

#include "X.h"
#include "Xproto.h"
#include "resource.h"
#include "misc.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "colormapst.h"
#include "colormap.h"
#include "pixmapstr.h"

#include "OScompiler.h"
#include "ppc.h"
#include "ibmTrace.h"

Bool
ppcCreateColormapNoop( pmap )
	ColormapPtr pmap;
	{
	return TRUE;
	}
void
ppcStoreColors( pmap, ndef, pdefs )
register ColormapPtr pmap ;
register int ndef ;
register xColorItem *pdefs ;
{
/* check to see if talking to current colormap, IF SO THEN INSTALL */
   register void (*fnp)() ;
   register int i ;
   int this_is_installed ;
   ppcScrnPrivPtr ppcPriv = (ppcScrnPriv *) (pmap->pScreen->devPrivate) ;

   this_is_installed = ( pmap == ppcPriv->InstalledColormap ) ;

   for ( fnp = ppcPriv->setColor ; ndef-- ; pdefs++ ) {
	/* first, resolve colors into possible colors */
	(*pmap->pScreen->ResolveColor)(	&(pdefs->red),
					&(pdefs->green),
					&(pdefs->blue),
					pmap->pVisual ) ;
	if ( ( i = pdefs->pixel ) > pmap->pVisual->ColormapEntries )
		ErrorF("ppcStoreColors: Bad pixel number\n") ;
	else {
		/* remember that the index to red is really the entry number */
		pmap->red[i].co.local.red   = pdefs->red ;
		pmap->red[i].co.local.green = pdefs->green ;
		pmap->red[i].co.local.blue  = pdefs->blue ;
		if ( this_is_installed )
    			(*fnp)( i, pdefs->red, pdefs->green, pdefs->blue,
				pmap->pVisual ) ;
	}
    }

    return ;
}

int
ppcListInstalledColormaps( pScreen, pCmapList )
register ScreenPtr pScreen ;
register Colormap *pCmapList ;
{
   	if ( ( (ppcScrnPriv *) pScreen->devPrivate)->InstalledColormap ) {
		*pCmapList = ((ppcScrnPriv *) 
				(pScreen->devPrivate))->InstalledColormap->mid ;
		return 1 ;
	}
	else {
		*pCmapList = 0 ;
		return 0 ;
	}
	/*NOTREACHED*/
}

void
ppcInstallColormap( pColormap ) 
register ColormapPtr pColormap ;
{
   register int i ;
   register void (*fnp)() ;
   ppcScrnPrivPtr devPriv ;
   ScreenPtr pScreen = pColormap->pScreen ;

TRACE( ( "ppcInstallColormap(pColormap=x%x)", pColormap ) ) ;

   if ( !pColormap ) /* Re-Install of NULL map ?? */
	return ;

   devPriv = (ppcScrnPriv *) pScreen->devPrivate ;
   fnp = devPriv->setColor ;
   for ( i = pColormap->pVisual->ColormapEntries ; i-- ;  )
	(*fnp)( i,
		pColormap->red[i].co.local.red,
		pColormap->red[i].co.local.green,
		pColormap->red[i].co.local.blue,
		pColormap->pVisual ) ;

   if ( devPriv->InstalledColormap != pColormap ) {
	if ( devPriv->InstalledColormap ) /* Remember it may not exist */
	    WalkTree( pScreen, TellLostMap,
		      &(devPriv->InstalledColormap->mid) ) ;
	devPriv->InstalledColormap = pColormap ;
	WalkTree( pScreen, TellGainedMap, &(pColormap->mid) ) ;

	/* RecolorCursor --- * Added 9/28/87  T Paquin
	 *
	 * I know there is a RecolorCursor in the Screen structure, but
	 * It requires a pCurs pointer, which we do not have here.  Therefore,
	 * a ppcScreenPrivate function will be called with a pScreen parameter.
	 * At worst, the private function will return.  At second worst,
	 * it will have remembered the pCurs, and will call miRecolorCursor.
	 * At best, it will have remembered the cursor's FG and BG rgb values
	 * (which it has if it has saved the pCurs pointer)
	 * and will cause the cursor's colors to match the best available in
	 * the colormap we just finished installing.
	 * miRecolorCursor is a dog and calls code you had already in
	 * DisplayCursor (as well as Realize/Unrealize) anyway.
	 */
	(* devPriv->RecolorCursor)( pColormap ) ;
    }

   return ;
}

void
ppcUninstallColormap( pColormap ) 
register ColormapPtr pColormap ;
{
   register ColormapPtr pCmap ;

   TRACE( ( "ppcUninstallColormap(pColormap=x%x)\n", pColormap ) ) ;

   pCmap = (ColormapPtr) LookupIDByType( pColormap->pScreen->defColormap,
				   RT_COLORMAP ) ;
   if ( pCmap != pColormap ) /* never uninstall the default map */
	(* pColormap->pScreen->InstallColormap)( pCmap ) ;

   return ;
}

/* WARNING WARNING WARNING
 *
 * THIS IS ONLY HERE FOR USE WHILE A SCREEN IS UNAVAILABLE.  We don't
 * want the case where someone has stolen the screen but my alive (and
 * nicely-not-drawing) server wants to do an alloc color on an installed
 * colormap.
 */

void
ppcUninstallDefaultColormap( pScreen )
    register ScreenPtr pScreen ;
{
ColormapPtr pCmap ;

	pCmap = (ColormapPtr) LookupIDByType( pScreen->defColormap, 
				        RT_COLORMAP ) ;

   	((ppcScrnPriv *) pScreen->devPrivate)->InstalledColormap = 0 ;

	WalkTree( pScreen, TellLostMap, &(pCmap->mid) ) ;
	return ;
}
