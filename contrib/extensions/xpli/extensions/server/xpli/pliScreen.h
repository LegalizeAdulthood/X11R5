/*
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/* This file must match the ibmScreen.h file in ddx/ibm/common.
   It exists to support the ppc functions which need access to
   this data where the screen in not in the ibmScreens table and
   is not accessible through the usual "drive the cursor off the 
   screen" machinery in ddx/ibm/AIX/aixWrap.c. Only the access
   macros have been changed - to change the layout of the structure
   will surely cause things to die */

#ifndef PLISCREEN_H
#define PLISCREEN_H

	/*
	 * IBM specific, per-screen information
	 */

typedef struct IBMPERSCRINFO {
	BoxRec		 ibm_ScreenBounds;

	int		 ibm_NumFormats;
	PixmapFormatRec	*ibm_ScreenFormats;

	Bool		(*ibm_InitFunc)();
	int		(*ibm_ProbeFunc)(); /* returns file descriptor */
	void		(*ibm_HideCursor)();

	char		 *ibm_ScreenFlag;
	char		 *ibm_ScreenDevice;
	char		 *ibm_ScreenPointer;
	void		(*ibm_SaveFunc)();
	void		(*ibm_RestoreFunc)();

	ScreenPtr	  ibm_Screen;

	int		  ibm_ScreenFD;

	int		  ibm_Wanted;

	short		  ibm_CursorHotX;
	short		  ibm_CursorHotY;
	void		(*ibm_CursorShow)();
	CursorPtr	  ibm_CurrentCursor;
	int		  ibm_ScreenState ;
	unsigned	  ibm_DeviceID;
} ibmPerScreenInfo;

#define SCREEN_UNINITIALIZED	0
#define	SCREEN_INACTIVE		1
#define	SCREEN_ACTIVE		2

#define	ibmScreenBounds(n)	(&pliIbmScreen->ibm_ScreenBounds)
#define	ibmScreenMinX(n)	(ibmScreenBounds(n)->x1)
#define	ibmScreenMinY(n)	(ibmScreenBounds(n)->y1)
#define	ibmScreenMaxX(n)	(ibmScreenBounds(n)->x2)
#define	ibmScreenMaxY(n)	(ibmScreenBounds(n)->y2)
#define	ibmNumFormats(n)	(pliIbmScreen->ibm_NumFormats)
#define	ibmScreenFormats(n)	(pliIbmScreen->ibm_ScreenFormats)
#define	ibmScreenInit(n)	(pliIbmScreen->ibm_InitFunc)
#define	ibmHideCursor(n)	(pliIbmScreen->ibm_HideCursor)
#define	ibmScreenFlag(n)	(pliIbmScreen->ibm_ScreenFlag)
#define	ibmScreenDevice(n)	(pliIbmScreen->ibm_ScreenDevice)
#define	ibmScreenPointer(n)	(pliIbmScreen->ibm_ScreenPointer)
#define	ibmScreenFD(n)		(pliIbmScreen->ibm_ScreenFD)
#define	ibmCursorHotX(n)	(pliIbmScreen->ibm_CursorHotX)
#define	ibmCursorHotY(n)	(pliIbmScreen->ibm_CursorHotY)
#define	ibmCursorShow(n)	(pliIbmScreen->ibm_CursorShow)
#define	ibmCurrentCursor(n)	(pliIbmScreen->ibm_CurrentCursor)
#define	ibmScreen(n)		(pliIbmScreen->ibm_Screen)
#define	ibmScreenState(n)	(pliIbmScreen->ibm_ScreenState)
#define	ibmSetScreenState(n,s)	(pliIbmScreen->ibm_ScreenState=(s))
#define	ibmDeviceID(n)		(pliIbmScreen->ibm_DeviceID)

/* Macro Calls For Dynamically changing the screen set */
#define	ibmSaveScreenInfo(n)	(* pliIbmScreen->ibm_SaveFunc)(n)
#define	ibmRestoreScreenInfo(n)	(* pliIbmScreen->ibm_RestoreFunc)(n)

extern ibmPerScreenInfo *pliIbmScreen;
extern	int		 ibmSaveScreen();
/* extern	ibmPerScreenInfo *ibmPossibleScreens[]; */
/* extern	ibmPerScreenInfo *ibmScreens[MAXSCREENS]; */
extern	int		 ibmNumScreens;
extern	int		 ibmXWrapScreen;
extern	int		 ibmYWrapScreen;
extern	int		 ibmCurrentScreen;
extern	int		 ibmUseHardware;
extern  char		 *ibmWhitePixelText;
extern  char		 *ibmBlackPixelText;
extern  Bool		 ibmDontZap;

#endif /* PLISCREEN_H */
