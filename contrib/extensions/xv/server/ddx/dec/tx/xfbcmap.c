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

#include "X.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "resource.h"

#ifdef	STATIC_COLOR

static ColormapPtr InstalledMaps[MAXSCREENS];

int
xfbListInstalledColormaps(pScreen, pmaps)
    ScreenPtr	pScreen;
    Colormap	*pmaps;
{
    /* By the time we are processing requests, we can guarantee that there
     * is always a colormap installed */
    *pmaps = InstalledMaps[pScreen->myNum]->mid;
    return (1);
}


void
xfbInstallColormap(pmap)
    ColormapPtr	pmap;
{
    int index = pmap->pScreen->myNum;
    ColormapPtr oldpmap = InstalledMaps[index];

    if(pmap != oldpmap)
    {
	/* Uninstall pInstalledMap. No hardware changes required, just
	 * notify all interested parties. */
	if(oldpmap != (ColormapPtr)None)
	    WalkTree(pmap->pScreen, TellLostMap, (char *)&oldpmap->mid);
	/* Install pmap */
	InstalledMaps[index] = pmap;
	WalkTree(pmap->pScreen, TellGainedMap, (char *)&pmap->mid);

    }
}

void
xfbUninstallColormap(pmap)
    ColormapPtr	pmap;
{
    int index = pmap->pScreen->myNum;
    ColormapPtr curpmap = InstalledMaps[index];

    if(pmap == curpmap)
    {
	if (pmap->mid != pmap->pScreen->defColormap)
	{
	    curpmap = (ColormapPtr) LookupIDByType(pmap->pScreen->defColormap,
						   RT_COLORMAP);
	    (*pmap->pScreen->InstallColormap)(curpmap);
	}
    }
}

#endif

void
xfbResolveColor(pred, pgreen, pblue, pVisual)
    unsigned short	*pred, *pgreen, *pblue;
    register VisualPtr	pVisual;
{
    int shift = 16 - pVisual->bitsPerRGBValue;
    unsigned lim = (1 << pVisual->bitsPerRGBValue) - 1;

    if ((pVisual->class == PseudoColor) || (pVisual->class == DirectColor))
    {
	/* rescale to rgb bits */
	*pred = ((*pred >> shift) * 65535) / lim;
	*pgreen = ((*pgreen >> shift) * 65535) / lim;
	*pblue = ((*pblue >> shift) * 65535) / lim;
    }
    else if (pVisual->class == GrayScale)
    {
	/* rescale to gray then rgb bits */
	*pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	*pblue = *pgreen = *pred = ((*pred >> shift) * 65535) / lim;
    }
    else if (pVisual->class == StaticGray)
    {
	unsigned limg = pVisual->ColormapEntries - 1;
	/* rescale to gray then [0..limg] then [0..65535] then rgb bits */
	*pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	*pred = ((((*pred * (limg + 1))) >> 16) * 65535) / limg;
	*pblue = *pgreen = *pred = ((*pred >> shift) * 65535) / lim;
    }
    else
    {
	unsigned limr, limg, limb;

	limr = pVisual->redMask >> pVisual->offsetRed;
	limg = pVisual->greenMask >> pVisual->offsetGreen;
	limb = pVisual->blueMask >> pVisual->offsetBlue;
	/* rescale to [0..limN] then [0..65535] then rgb bits */
	*pred = ((((((*pred * (limr + 1)) >> 16) *
		    65535) / limr) >> shift) * 65535) / lim;
	*pgreen = ((((((*pgreen * (limg + 1)) >> 16) *
		      65535) / limg) >> shift) * 65535) / lim;
	*pblue = ((((((*pblue * (limb + 1)) >> 16) *
		     65535) / limb) >> shift) * 65535) / lim;
    }
}

Bool
xfbInitializeColormap(pmap)
    register ColormapPtr	pmap;
{
    register unsigned i;
    register VisualPtr pVisual;
    unsigned lim, maxent, shift;

    pVisual = pmap->pVisual;
    lim = (1 << pVisual->bitsPerRGBValue) - 1;
    shift = 16 - pVisual->bitsPerRGBValue;
    maxent = pVisual->ColormapEntries - 1;
    if (pVisual->class == TrueColor)
    {
	unsigned limr, limg, limb;

	limr = pVisual->redMask >> pVisual->offsetRed;
	limg = pVisual->greenMask >> pVisual->offsetGreen;
	limb = pVisual->blueMask >> pVisual->offsetBlue;
	for(i = 0; i <= maxent; i++)
	{
	    /* rescale to [0..65535] then rgb bits */
	    pmap->red[i].co.local.red =
		((((i * 65535) / limr) >> shift) * 65535) / lim;
	    pmap->green[i].co.local.green =
		((((i * 65535) / limg) >> shift) * 65535) / lim;
	    pmap->blue[i].co.local.blue =
		((((i * 65535) / limb) >> shift) * 65535) / lim;
	}
    }
    else if (pVisual->class == StaticColor)
    {
	unsigned limr, limg, limb;

	limr = pVisual->redMask >> pVisual->offsetRed;
	limg = pVisual->greenMask >> pVisual->offsetGreen;
	limb = pVisual->blueMask >> pVisual->offsetBlue;
	for(i = 0; i <= maxent; i++)
	{
	    /* rescale to [0..65535] then rgb bits */
	    pmap->red[i].co.local.red =
		((((((i & pVisual->redMask) >> pVisual->offsetRed)
		    * 65535) / limr) >> shift) * 65535) / lim;
	    pmap->red[i].co.local.green =
		((((((i & pVisual->greenMask) >> pVisual->offsetGreen)
		    * 65535) / limg) >> shift) * 65535) / lim;
	    pmap->red[i].co.local.blue =
		((((((i & pVisual->blueMask) >> pVisual->offsetBlue)
		    * 65535) / limb) >> shift) * 65535) / lim;
	}
    }
    else if (pVisual->class == StaticGray)
    {
	for(i = 0; i <= maxent; i++)
	{
	    /* rescale to [0..65535] then rgb bits */
	    pmap->red[i].co.local.red = ((((i * 65535) / maxent) >> shift)
					 * 65535) / lim;
	    pmap->red[i].co.local.green = pmap->red[i].co.local.red;
	    pmap->red[i].co.local.blue = pmap->red[i].co.local.red;
	}
    }
    return TRUE;
}

Bool
xfbCreateDefColormap(pScreen)
    ScreenPtr pScreen;
{
    unsigned short	zero = 0, ones = ~0;
    VisualPtr	pVisual;
    ColormapPtr	cmap;
    
    for (pVisual = pScreen->visuals;
	 pVisual->vid != pScreen->rootVisual;
	 pVisual++)
	;

    if (CreateColormap(pScreen->defColormap, pScreen, pVisual, &cmap,
		       (pVisual->class & DynamicClass) ? AllocNone : AllocAll,
		       0)
	!= Success)
	return FALSE;
    if ((AllocColor(cmap, &ones, &ones, &ones, &(pScreen->whitePixel), 0) !=
       	   Success) ||
    	(AllocColor(cmap, &zero, &zero, &zero, &(pScreen->blackPixel), 0) !=
       	   Success))
	return FALSE;
    (*pScreen->InstallColormap)(cmap);
    return TRUE;
}
