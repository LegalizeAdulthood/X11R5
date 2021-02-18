/*
 * FontSample.h
 *
 * Copyright (C) 1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#ifndef _FontSampler_h
#define _FontSampler_h

/* New resouce names and classes */

#define XtNdismissCallback "dismissCallback"
#define XtNsizes "sizes"
#define XtCSizes "Sizes"
#define XtNsizeCount "sizeCount"
#define XtCSizeCount "SizeCount"
#define XtNfontSelectionBox "fontSelectionBox"
#define XtCFontSelectionBox "FontSelectionBox"
#define XtNminimumWidth "minimumWidth"
#define XtCMinimumWidth "MinimumWidth"
#define XtNminimumHeight "minimumHeight"
#define XtCMinimumHeight "MinimumHeight"
#define XtNnoRoomMessage "noRoomMessage"
#define XtNnoFontMessage "noFontMessage"
#define XtNnoSelectedFontMessage "noSelectedFontMessage"
#define XtNnoSelectedFamilyMessage "noSelectedFamilyMessage"
#define XtNnoFamilyFontMessage "noFamilyFontMessage"
#define XtNnoMatchMessage "noMatchMessage"
#define XtCMessage "Message"

/* Read-only resources for all the subwidgets */

#define XtNpanelChild "panelChild"
#define XtNareaChild "areaChild"
#define XtNtextChild "textChild"
#define XtNfontLabelChild "fontLabelChild"
#define XtNscrolledWindowChild "scrolledWindowChild"
#define XtNdisplayButtonChild "displayButtonChild"
#define XtNdismissButtonChild "dismissButtonChild"
#define XtNstopButtonChild "stopButtonChild"
#define XtNclearButtonChild "clearButtonChild"
#define XtNradioFrameChild "radioFrameChild"
#define XtNradioBoxChild "radioBoxChild"
#define XtNallToggleChild "allToggleChild"
#define XtNselectedToggleChild "selectedToggleChild"
#define XtNselectedFamilyToggleChild "selectedFamilyToggleChild"
#define XtNfilterToggleChild "filterToggleChild"
#define XtNfilterTextChild "filterTextChild"
#define XtNfilterBoxChild "filterBoxChild"
#define XtNfilterFrameChild "filterFrameChild"
#define XtNsizeOptionMenuChild "sizeOptionMenuChild"
#define XtNsizeTextFieldChild "sizeTextFieldChild"
#define XtNsizeLabelChild "sizeLabelChild"

/* Class record constants */

extern WidgetClass fontSamplerWidgetClass;

typedef struct _FontSamplerRec *FontSamplerWidget;

#ifdef _NO_PROTO

extern void FSBCancelSampler();

#else /* _NO_PROTO */

extern void FSBCancelSampler(Widget w);

#endif /* _NO_PROTO */

#endif /* _FontSampler_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
