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
 * PRPQ 5799-PFF (C) COPYRIGHT IBM CORPORATION 1987
 * LICENSED MATERIALS - PROPERTY OF IBM
 * REFER TO COPYRIGHT INSTRUCTIONS FORM NUMBER G120-2083
 */
/* $Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliProcs.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */
/* $Source: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliProcs.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $ */

#ifndef lint
/*
static char *rcsid = "$Header: /usr/pli/x11r4.ps2/mit/extensions/server/xpli/pliProcs.h,v 0.0 03/12/90 15:28:51 jrhyne Exp $" ;
*/
#endif


#if !defined(NO_FUNCTION_PROTOTYPES)
/* Cursor Stuff */
/* pliCurs.o */
extern int pliCheckCursor(
	const int x,
	const int y,
	const int lx,
	const int ly ) ;

extern void pliReplaceCursor(
	void ) ;

extern void pliRemoveCursor(
	void ) ;

extern int pliCursorSemaphore ;

#if !defined(CURSOR_ONLY)
extern void pliShowCursor(
	const int x_loc,
	const int y_loc ) ;

extern void pliCursorInit(
	int index ) ;

extern void pliColorCursor(
	unsigned long int fg,
	unsigned long int bg ) ;

extern void pliRecolorCursor(
	ColormapPtr cmap ) ;

extern int pliDisplayCursor(
	const ScreenPtr pScr,
	const CursorPtr pCurs ) ;

/* pliHWCntl.o */
extern void save_pli_state(
	struct pli_video_hardware_state *VS ) ;

extern void restore_pli_state(
	struct pli_video_hardware_state const *VS ) ;

extern void pliSetColor(
	unsigned long int color,
	unsigned short int r,
	unsigned short int g,
	unsigned short int b ) ;

extern void pli_set_graphics_mode(
	struct pli_video_hardware_state *VS ) ;

extern Bool pliScreenClose(
	const int index,
	const ScreenPtr pScreen ) ;

extern Bool pliScreenInit(
	const int index,
	const ScreenPtr pScreen,
	const int argc,
	char * const * const argv ) ;

/* pliOSD.o */
extern int pliCheckDisplay(
	int fd ) ;

extern int pliProbe(
	void ) ;

extern int pliScreenInitHW(
	int index ) ;

extern void pliCloseHW(
	int index ) ;

/* pliSuspScr.o */
extern void pliSuspendScreenAndSave(
	void ) ;

extern void pliRestoreScreenAndActivate(
	void ) ;

/*
 * Everything Below is "ppc" entry points
 */
/* pliGC.o */
extern Mask pliChangeGCtype(
	GC *pGC,
	ppcPrivGCPtr devPriv ) ;

extern Mask pliChangeWindowGC(
	GC *pGC,
	Mask changes ) ;

/* pliBitBlt.o */
extern void pliBitBlt(
	const int alu,
	const int readplanes,
	const int writeplanes,
	int x0,
	int y0,
	int x1,
	int y1,
	int w,
	int h ) ;

/* pliGlyph.o */
extern void pliDrawGlyph(
	const unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	const int alu,
	const unsigned long int planes ) ;

/* pliImages.o */
extern void pliDrawColorImage(
	int x,
	int y,
	int w,
	int h,
	const unsigned char *data,
	int stride,
	const int alu,
	const unsigned int planes ) ;

extern void pliReadColorImage(
	int x,
	int y,
	int lx,
	int ly,
	unsigned char *data,
	int stride ) ;

/* pliLine.o */
extern void pliHorzLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int w ) ;

extern void pliVertLine(
	unsigned long int color,
	int alu,
	unsigned long int mask,
	int x,
	int y,
	int h ) ;

extern void pliBresLine(
	unsigned long int color,
	int alu,
	unsigned long int planes,
	int signdx,
	int signdy,
	int axis,
	int x,
	int y,
	int et,
	int e1,
	int e2,
	int len ) ;

/* pliStipple.o */
extern void pliDrawMonoImage(
	unsigned char *data,
	int x,
	int y,
	int w,
	int h,
	unsigned long int fg,
	int alu,
	unsigned long int planes ) ;

extern void pliFillStipple(
	const PixmapPtr pStipple,
	unsigned long int fg,
	const int alu,
	unsigned long int planes,
	int x,
	int y,
	int w,
	int h,
	const int xSrc,
	const int ySrc ) ;

/* pliSolid.o */
extern void pliFillSolid(
	unsigned long int color,
	const int alu,
	unsigned long int planes,
	int x0,
	const int y0,
	int lx,
	const int ly ) ;
#endif /* CURSOR_ONLY */
#else /* defined(NO_FUNCTION_PROTOTYPES) Brain-Damaged */
/* Cursor Stuff */
/* pliCurs.o */
extern int pliCheckCursor() ;
extern void pliReplaceCursor() ;
extern int pliCursorSemaphore ;
#if !defined(CURSOR_ONLY)
extern void pliRemoveCursor() ;
extern void pliShowCursor() ;
extern void pliCursorInit() ;
extern void pliColorCursor() ;
extern void pliRecolorCursor() ;
extern int pliDisplayCursor() ;

extern void save_pli_state() ;
extern void restore_pli_state() ;
extern void pliSetColor() ;
extern void pli_save_dac() ;
extern void pli_restore_dac() ;
extern void pli_set_graphics_mode() ;
extern Bool pliScreenClose() ;
extern Bool pliScreenInit() ;
/* pliOSD.o */
extern int pliCheckDisplay() ;
extern int pliProbe() ;
extern int pliScreenInitHW() ;
extern void pliCloseHW() ;
extern void pliSuspendScreenAndSave() ;
extern void pliRestoreScreenAndActivate() ;
/*
 * Everything Below is "ppc" entry points
 */
/* pliGC.o */
extern Mask pliChangeGCtype() ;
extern Mask pliChangeWindowGC() ;
/* pliBitBlt.o */
extern void pliBitBlt() ;
/* pliGlyph.o */
extern void pliDrawGlyph() ;
/* pliImages.o */
extern void pliDrawColorImage() ;
extern void pliReadColorImage() ;
/* pliLine.o */
extern void pliHorzLine() ;
extern void pliVertLine() ;
extern void pliBresLine() ;
/* pliStipple.o */
extern void pliDrawMonoImage() ;
extern void pliFillStipple() ;
/* pliSolid.o */
extern void pliFillSolid() ;

#endif /* CURSOR_ONLY */
#endif
