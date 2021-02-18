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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliScrInfo.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliScrInfo.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliScrInfo.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif


#include "X.h"
#include "misc.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "cursorstr.h"

#include "pliScreen.h"
#include "pliVideo.h"

extern	Bool	pliScreenInit();
extern	Bool	pliProbe();
extern	void	pliRemoveCursor();
extern	void	pliShowCursor();
extern	void	pliSuspendScreenAndSave() ;
extern	void	pliRestoreScreenAndActivate() ;
extern	PixmapFormatRec  pliFormats[];

ibmPerScreenInfo pliScreenInfoStruct = {
	{ 0, 0, 639, 479 },	/* BoxRec	 ibm_ScreenBounds; */
	1,			/* int		 ibm_NumFormats; */
	pliFormats,		/* PixmapFormatRec	*ibm_ScreenFormats; */
	pliScreenInit,		/* Bool		(*ibm_InitFunc)(); */
	pliProbe,		/* int		(*ibm_ProbeFunc)(); */
	pliRemoveCursor,	/* void		(*ibm_HideCursor)(); */
	"-pli",			/* char		 *ibm_ScreenFlag; */
	"",			/* char		 *ibm_ScreenDevice; */
	"",			/* char		 *ibm_ScreenPointer; */
	pliSuspendScreenAndSave,/* void		(*ibm_SaveFunc)(); */
	pliRestoreScreenAndActivate,/* void	(*ibm_RestoreFunc)(); */
	(ScreenPtr) 0,		/* ScreenPtr	  ibm_Screen */
	-1,			/* int		  ibm_ScreenFD */
	0,			/* int		  ibm_Wanted */
	0,			/* short	  ibm_CursorHotX */
	0,			/* short	  ibm_CursorHotY */
	pliShowCursor,		/* void		 (*ibm_CursorShow)() */
	(CursorPtr) 0,		/* CursorPtr	  ibm_CurrentCursor */
	SCREEN_UNINITIALIZED	/* int		  ibm_ScreenState */
} ;
