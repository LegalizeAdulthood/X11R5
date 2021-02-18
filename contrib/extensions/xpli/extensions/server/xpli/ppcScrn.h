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
		Copyright IBM Corporation 1988

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

/* $Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcScrn.h,v 1.3 89/06/21 16:36:51 jeff Exp Locker: jeff $ */
/* $Source: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcScrn.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidppc = "$Header: /andrew/X11/r3src/r3plus/server/ddx/ibm/ppc/RCS/ppcScrn.h,v 1.3 89/06/21 16:36:51 jeff Exp Locker: jeff $" ;
#endif

#include "ppcCache.h"

typedef struct {
	PixmapRec	pixmap ; /* this is here for mfb, it expects
				  * devPrivate to point to a pixmap.
				  */
	/* These next 7 field are for support of
	 * common Screen Init & Validate GC
	 */
	void		(* devHardwareInitBB)() ; /* Call Before struct copy */
	void		(* devHardwareInitAB)() ; /* Call After struct copy */
	GCPtr		devPrototypeGC ;
	ppcPrivGC 	*devPrototypeGCPriv ;
	Mask		(* changeGCtype)() ;
	Mask		(* changeWindowGC)() ;
	/* Here Down Is the "old" ppc */
	ColormapPtr	InstalledColormap ;
	void		(* blit)() ;
	void		(* solidFill)() ;
	void		(* tileFill)() ;
	void		(* stipFill)() ;
	void		(* opstipFill)() ;
	void		(* imageFill)() ;
	void		(* imageRead)() ;
	void		(* lineBres)() ;
	void		(* lineHorz)() ;
	void		(* lineVert)() ;
	void		(* setColor)() ;
	void		(* RecolorCursor)() ;
	void		(* monoFill)() ;		/* 32 bit padded */
	void		(* glyphFill)() ;	/* GLYPHPADBYTES*8 bit padded */
	unsigned long *((* planeRead)()) ;	/* for XYformat getImage() */
	void		(* replicateArea)() ; /*Accelerator for Tile & Stipple*/
	void		(* DestroyGCPriv)() ; /* destroy devPriv in ppcPrivGC */
	/* High Level Software Cursor Support !! */
	int		* CursorSemaphore ;
	int			(* CheckCursor)() ;
	void		(* ReplaceCursor)() ;
	ppcCacheInfoPtr	cacheInfo;
} ppcScrnPriv, *ppcScrnPrivPtr ;
