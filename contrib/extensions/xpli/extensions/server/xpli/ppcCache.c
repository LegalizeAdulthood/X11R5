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
/*
 * ppcCache.c - generic font cache manager
 */

#include "X.h"
#include "scrnintstr.h"
#include "ppcCache.h"
#include "pixmapstr.h"
#include "ppc.h"

void
ppcCacheInit(pScreen)
	ScreenPtr pScreen;
	{
	int i;
	ppcScrnPrivPtr devPriv;
	ppcCacheInfoPtr cacheInfo;
	static int inited = 0;

	if ( inited )
		return;

	inited = 1;

	devPriv = (ppcScrnPrivPtr) pScreen->devPrivate;
	cacheInfo = devPriv->cacheInfo;

	for( i = 0; i < NUM_BAND_HEIGHTS; i++ )
		cacheInfo->bandsByHeight[i] = (ppcBandInfoPtr)NULL;

	for( i = 0; i < cacheInfo->numPlanes; i++ )
		{
		(cacheInfo->planes[i]).bandListHead = NULL;
		(cacheInfo->planes[i]).bandListTail = NULL;
		(cacheInfo->planes[i]).nextY = 0;
		}

	}


/*
 * newBand - create a new band and return a pointer to it
 */
static ppcBandInfoPtr
newBand(cacheInfo, height)
	ppcCacheInfoPtr cacheInfo;
	int height;
	{
	ppcBandInfoPtr band;
	int bandlist;
	int i;

	if ( ( band = (ppcBandInfoPtr)Xalloc(sizeof(ppcBandInfo)) ) == NULL )
		{
		return((ppcBandInfoPtr)NULL);
		}

	band->h = BAND_HEIGHT(height);

	/*
	 * look through the planes for one with room for this band
	 */
	for( i = 0; i < cacheInfo->numPlanes; i++ )
		{
		register ppcPlaneInfoPtr planeInfo;
		planeInfo = &(cacheInfo->planes[i]);
		if ( ( band->h + planeInfo->nextY ) <= 
				planeInfo->h )
			{

			/*
			 * init rest of band's fields
			 */
			band->y = planeInfo->nextY;
			planeInfo->nextY += band->h;
			band->plane = i;
			band->nextX = 0;
			band->w = planeInfo->w;
			band->bmList = (ppcBMInfoPtr)NULL;
			band->bmListTail = (ppcBMInfoPtr)NULL;
			band->nextBand = 
				band->nextBandByHeight = (ppcBandInfoPtr)NULL;
			/*
			 * insert band on plane's list
			 */
			if ( planeInfo->bandListTail == NULL )
				{
				planeInfo->bandListTail = 
						planeInfo->bandListHead = band;
				band->prevBand = (ppcBandInfoPtr)NULL;
				}
			else
				{
				planeInfo->bandListTail->nextBand = band;
				band->prevBand = planeInfo->bandListTail;
				planeInfo->bandListTail = band;
				}

			/*
			 * insert onto the appropriate by-height band list
			 */
			bandlist = BAND_LIST(band->h);

			if ( cacheInfo->bandsByHeightTail[bandlist] == NULL )
				{
				cacheInfo->bandsByHeightTail[bandlist] =
					cacheInfo->bandsByHeight[bandlist] =
					band;
				band->prevBandByHeight = (ppcBandInfoPtr)NULL;
				}
			else
				{
				(cacheInfo->bandsByHeightTail[bandlist])->nextBandByHeight = band;
				band->prevBandByHeight = 
					cacheInfo->bandsByHeightTail[bandlist];
				cacheInfo->bandsByHeightTail[bandlist] = band;
				}

			return(band);
			}
		}

	/*
	 * If we get this far we didn't find any space for the band
	 */
	Xfree(band);
	return((ppcBandInfoPtr)NULL);

	}

/*
 * allocate space for a new bitmap in the cache
 */
ppcBMInfoPtr
newBM(cacheInfo, w, h)
	ppcCacheInfoPtr cacheInfo;
	int w, h;
	{
	ppcBandInfoPtr b;
	ppcBMInfoPtr nbm;

	/*
	 * look through all bands of the correct height
	 */
	for( b = cacheInfo->bandsByHeight[BAND_LIST(h)]; b != NULL; 
					b = b->nextBandByHeight )
		{
		if ( ( b->w - b->nextX ) >= w )
			{

			if ( (nbm = (ppcBMInfoPtr)Xalloc(sizeof(ppcBMInfo))) 
							== (ppcBMInfoPtr)NULL)
				{
				return((ppcBMInfoPtr)NULL);
				}

			/* init nbm */
			nbm->w = w;
			nbm->x = b->nextX;
			nbm->h = h;
			nbm->y = b->y;
			nbm->used = 1;

			/* update band */
			b->nextX += w;

			/* now link nbm into used list */
			if ( b->bmListTail == NULL )
				{
				b->bmList = nbm;
				b->bmListTail = nbm;
				nbm->next = nbm->prev = NULL;
				}
			else 
				{
				nbm->next = NULL;
				nbm->prev = b->bmListTail;
				b->bmListTail->next = nbm;
				b->bmListTail = nbm;
				}
			return(nbm);
			}
		}
	/* didn't find enough space in any band, lets make a new one */
	if ( ( b = newBand(cacheInfo, h) ) == NULL )
		return((ppcBMInfoPtr)NULL);

	if ( (nbm = (ppcBMInfoPtr)Xalloc(sizeof(ppcBMInfo))) == 
							(ppcBMInfoPtr)NULL)
		{
		return((ppcBMInfoPtr)NULL);
		}

	/* this code cheats because it knows that the new band is empty */

	/* init nbm */
	nbm->w = w;
	nbm->x = b->nextX;
	nbm->h = h;
	nbm->y = b->y;
	nbm->used = 1;

	/* update band */
	b->nextX += w;

	/* link into band */
	b->bmList = nbm;
	b->bmListTail = nbm;
	nbm->next = nbm->prev = NULL;

	return(nbm);
	
	}

