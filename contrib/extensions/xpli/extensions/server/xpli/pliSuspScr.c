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

/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSuspScr.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSuspScr.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliSuspScr.c,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
#endif

#include "X.h"
#include "windowstr.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "cursor.h"
#include "gcstruct.h"

#include "OScompiler.h"

#include "pliScreen.h"

#include "ppcGCstr.h"
#include "ppcScrn.h"

#include "pliVideo.h"
#include "pliSave.h"

#define CURSOR_ONLY
#include "pliProcs.h"

#if defined(IBM_OS_HAS_HFT) && !defined(IBM_OS_HAS_X_QUEUE)
#define HFT_HACKS "Oh no. The hack is here!"
#endif

#if !defined(HFT_HACKS)
extern WindowPtr ObscureEntireScreen() ;
#else
extern void ibmSaveScreenData() ;
extern void ibmRestoreScreenData() ;
extern void ppcSaveAreas() ;
extern void ppcRestoreAreas() ;
#endif

/* ************************************************************************** */

/* Data from pliScrInfo.c */
extern ibmPerScreenInfo pliScreenInfoStruct ;

/* Data from pliData.c */
extern ppcScrnPriv pliScrnPriv ;

/* Procedures from pliOSD.c */
extern int pliScreenInitHW() ;
extern int pliCloseHW() ;

/* Procedures from pliCurs.c */
extern void pliRemoveCursor() ;
extern void pliShowCursor() ;

/* Global Available For Anyone */
int pliDisabled = 0 ; /* But only used in pliOSD.c ! */

/* ************************************************************************** */

/* Private Saved State Variables */
static int CursorWasActive = 0 ;
static ppcScrnPriv savedpliScrnPriv ;
#if !defined(HFT_HACKS)
static WindowPtr coveringWindow = (WindowPtr) 0 ;
#endif

static int
DoNaught()
{
return 0 ;
}

#if !defined(HFT_HACKS)
void
pliSuspendScreenAndSave( index )
int index ;
#else /* erik's hft hack */
void
pliSuspendScreenAndSave( pScreen, saveBits )
ScreenPtr pScreen ;
int saveBits ;
#endif
{
if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_ACTIVE ) {
	/* Now Disable any further screen writes
	 * by preventing call though the screen-priv
	 */
	savedpliScrnPriv = pliScrnPriv ;
	CursorWasActive = !pliCursorSemaphore
			 && pliCheckCursor( 0, 0, MAX_ROW, MAX_COLUMN ) ;
	pliCursorSemaphore++ ;

	/* Fields in the ibmPerScreenInfo Structure */
	pliScreenInfoStruct.ibm_HideCursor = (void (*)()) DoNaught ;
	pliScreenInfoStruct.ibm_CursorShow = (void (*)()) DoNaught ;

#if !defined(HFT_HACKS)
	/* Obscure The Screen */
	coveringWindow = ObscureEntireScreen( pliScreenInfoStruct.ibm_Screen ) ;
#else /* erik's hft hack */
	if ( saveBits )
		ibmSaveScreenData( pScreen, ppcSaveAreas ) ;
#endif

	/* Fields in the ppcScrnPriv Structure */
	pliScrnPriv.blit = (void (*)()) DoNaught ;
	pliScrnPriv.solidFill = (void (*)()) DoNaught ;
	pliScrnPriv.tileFill = (void (*)()) DoNaught ;
	pliScrnPriv.stipFill = (void (*)()) DoNaught ;
	pliScrnPriv.opstipFill = (void (*)()) DoNaught ;
	pliScrnPriv.imageFill = (void (*)()) DoNaught ;
	pliScrnPriv.imageRead = (void (*)()) DoNaught ;
	pliScrnPriv.lineBres = (void (*)()) DoNaught ;
	pliScrnPriv.lineHorz = (void (*)()) DoNaught ;
	pliScrnPriv.lineVert = (void (*)()) DoNaught ;
	pliScrnPriv.setColor = (void (*)()) DoNaught ;
	pliScrnPriv.RecolorCursor = (void (*)()) DoNaught ;
	pliScrnPriv.monoFill = (void (*)()) DoNaught ;
	pliScrnPriv.glyphFill = (void (*)()) DoNaught ;
	pliScrnPriv.planeRead = (unsigned long int *(*)()) DoNaught ;
	pliScrnPriv.replicateArea = (void (*)()) DoNaught ;
	pliScrnPriv.DestroyGCPriv = (void (*)()) DoNaught ;
	pliScrnPriv.CheckCursor = (int (*)()) DoNaught ;
	pliScrnPriv.ReplaceCursor = (void (*)()) DoNaught ;

	pliDisabled = PhysicalPliAccessDisabled ;
	pliScreenInfoStruct.ibm_ScreenState = SCREEN_INACTIVE ;
}
else if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_INACTIVE ) {
	ErrorF( "pliSuspendScreenAndSave: pli State Is Already Inactive!" ) ;
}
else if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_UNINITIALIZED ) {
	ErrorF( "pliSuspendScreenAndSave: pli Still uninitialized!" ) ;
}

return ;
}

#if !defined(HFT_HACKS)
void
pliRestoreScreenAndActivate( index )
int index ;
#else /* erik's hft hack */
void
pliRestoreScreenAndActivate( pScreen, restoreBits )
ScreenPtr pScreen ;
int restoreBits ;
#endif
{
ColormapPtr currentColormap ;

if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_INACTIVE ) {

	/* Re-enable Cursor Fields in the ibmPerScreenInfo Structure */
	pliScreenInfoStruct.ibm_HideCursor = pliRemoveCursor ;
	pliScreenInfoStruct.ibm_CursorShow = pliShowCursor ;

	/* Now Re-Enable any screen writes by restoring the screen-priv */
	currentColormap = pliScrnPriv.InstalledColormap ;
	pliScrnPriv = savedpliScrnPriv ;

	if ( !( pliDisabled &= ~ ( PhysicalPliAccessDisabled
				 | VirtualPliHardwareOpenPending ) ) ) {
#if !defined(HFT_HACKS)
		pliScreenInitHW( index ) ; /* Magic Hardware Initialization */
		pliScreenInfoStruct.ibm_ScreenState = SCREEN_ACTIVE ;
		/* Now We Are Free to Draw Again !! */
		FreeResource( coveringWindow->drawable.id, RT_NONE ) ;
		coveringWindow = (WindowPtr) 0 ;
#else /* erik's hft hack */
		/* Magic Hardware Initialization */
		pliScreenInitHW( pScreen->myNum ) ;
		pliScreenInfoStruct.ibm_ScreenState = SCREEN_ACTIVE ;
		/* Now We Are Free to Draw Again !! */
		if ( restoreBits )
			ibmRestoreScreenData( pScreen, ppcRestoreAreas ) ;
#endif
		/* Now Reinstall the current Colormap !! */
		ppcInstallColormap( currentColormap ) ;

		if ( !--pliCursorSemaphore && CursorWasActive )
			pliReplaceCursor() ;
	}
	else { /* Close Request is pending */
		/* pliDisabled == VirtualPliHardwareClosePending */
		pliDisabled = 0 ;
		/* Magic Hardware Shutdown Code */
#if !defined(HFT_HACKS)
		pliCloseHW( index ) ;
#else /* erik's hft hack */
		pliCloseHW( pScreen->myNum ) ;
#endif
	}
}
else if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_ACTIVE ) {
	ErrorF( "pliRestoreScreenAndActivate: pli State Is Still Active!\n" ) ;
	return ;
}
else if ( pliScreenInfoStruct.ibm_ScreenState == SCREEN_UNINITIALIZED ) {
	ErrorF( "pliRestoreScreenAndActivate: pli Still uninitialized!" ) ;
}

return ;
}
