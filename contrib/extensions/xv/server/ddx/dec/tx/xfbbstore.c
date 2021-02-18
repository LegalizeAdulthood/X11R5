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
/*
 * xfbbstore.c --
 *	Functions required by the backing-store implementation in MI.
 *
 * Route the request to cfb or cfb32 based on depth.
 */

#include    "X.h"
#include    "regionstr.h"
#include    "scrnintstr.h"
#include    "pixmapstr.h"
#include    "windowstr.h"

void
xfbSaveAreas(pPixmap, prgnSave, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnSave; 	/* Region to save (pixmap-relative) */
    int	    	  	xorg;	    	/* X origin of region */
    int	    	  	yorg;	    	/* Y origin of region */
{
    if (pPixmap->drawable.depth == 8) {
	cfbSaveAreas(pPixmap, prgnSave, xorg, yorg);
    } else {
	cfb32SaveAreas(pPixmap, prgnSave, xorg, yorg);
    }
}

void
xfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg)
    PixmapPtr	  	pPixmap;  	/* Backing pixmap */
    RegionPtr	  	prgnRestore; 	/* Region to restore (screen-relative)*/
    int	    	  	xorg;	    	/* X origin of window */
    int	    	  	yorg;	    	/* Y origin of window */
{
    if (pPixmap->drawable.depth == 8) {
	cfbRestoreAreas(pPixmap, prgnRestore, xorg, yorg);
    } else {
	cfb32RestoreAreas(pPixmap, prgnRestore, xorg, yorg);
    }
}
