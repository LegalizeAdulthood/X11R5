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
/***********************************************************
		Copyright IBM Corporation 1987,1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

			    All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* $Header: ppcIO.c,v 1.2 89/11/29 18:01:46 keith Exp $ */
/* $Source: /xsrc/mit/server/ddx/ibm/ppc/RCS/ppcIO.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ppcIO.c,v 1.2 89/11/29 18:01:46 keith Exp $" ;
#endif

#include "X.h"
#include "Xproto.h"
#include "resource.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "window.h"
#include "font.h"
#include "miscstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "cursorstr.h"
#include "pixmapstr.h"
#include "mistruct.h"
#include "mibstore.h"

#include "OScompiler.h"
#include "pliScreen.h"
#include "OScursor.h"

#include "ppc.h"
#include "ppcProcs.h"

#include "ibmTrace.h"

miBSFuncRec ppcBSFuncRec = {
    ppcSaveAreas,
    ppcRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

static void
ppcDoNaught()
{
return ;
}

void
ppcCommonScreenInit( pScreen, index, pScrnProto )
register ScreenPtr const pScreen ;
register const int index ;
ScreenPtr const pScrnProto ;
{
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

	TRACE( ( "ppcCommonScreenInit(pScreen=0x%x,index=%d,pScrnProto=0x%x)\n",
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
	ibmScreen( pScreen->myNum = index ) =
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

		/* "ppc" Colormap Stuff */
		ibmDefineDefaultColormapColors( pVis, pColormap ) ;
		ppcInstallColormap( pColormap ) ;
		ibmAllocBlackAndWhitePixels( pColormap ) ;
	}
	miInitializeBackingStore (pScreen, &ppcBSFuncRec);
	TRACE(("ppcCommonScreenInit returns\n"));
	return ;
}

/*ARGSUSED*/
void
ppcCommonScreenClose( pScreen, index, pScrnProto )
register ScreenPtr const pScreen ;
register const int index ;
ScreenPtr const pScrnProto ;
{
/*
	FreeColormap( pScreen->defColormap ) ;
*/
	pScreen->defColormap = 0 ;
	( (ppcScrnPriv *) pScreen->devPrivate )->InstalledColormap =
		(ColormapPtr) 0 ;
	return ;
}
