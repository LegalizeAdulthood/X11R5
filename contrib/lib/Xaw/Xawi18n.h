/*
 * Copyright 1990, 1991 by OMRON Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of OMRON not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * OMRON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 */				

#ifndef _XAWI18N_H
#define _XAWI18N_H
#include <X11/Xlocale.h>
/* for Text Widget */
#include <X11/Xaw/MultiText.h>

#define XawAsciiSourceFreeString	XawMultiSourceFreeString
#define XawAsciiSave			XawMultiSave
#define XawAsciiSaveAsFile		XawMultiSaveAsFile
#define XawAsciiSourceChanged		XawMultiSourceChanged

#define XawAsciiFile			XawMultiFile
#define XawAsciiString			XawMultiString

#define asciiSrcObjectClass		multiSrcObjectClass
#define asciiStringWidgetClass		multiStringWidgetClass
#define asciiTextWidgetClass		multiTextWidgetClass
#define asciiSinkObjectClass		multiSinkObjectClass

#include <X11/Xaw/CommandLc.h>
#define commandWidgetClass		commandLocaleWidgetClass
#define CommandWidgetClass		CommandLocaleWidgetClass
#define CommandWidget			CommandLocaleWidget

#include <X11/Xaw/LabelLc.h>
#define labelWidgetClass		labelLocaleWidgetClass
#define LabelWidget			LabelLocaleWidget

#include <X11/Xaw/ListLc.h>
#define listWidgetClass			listLocaleWidgetClass
#define ListWidget			ListLocaleWidget
#define XawListChange			XawListLocaleChange
#define XawListUnhighlight		XawListLocaleUnhighlight
#define XawListHighlight		XawListLocaleHighlight
#define XawListReturnStruct		XawListLocaleReturnStruct
#define XawListShowCurrent		XawListLocaleShowCurrent

#include <X11/Xaw/SmeBSBLc.h>
#define smeBSBObjectClass		smeBSBLocaleObjectClass
#define smeBSBObject			smeBSBLocaleObject

#include <X11/Xaw/ToggleLc.h>
#define toggleWidgetClass		toggleLocaleWidgetClass

#include <X11/Xaw/MenuBtnLc.h>
#define menuButtonWidgetClass		menuButtonLocaleWidgetClass

#include <X11/Xaw/DialogLc.h>
#define dialogWidgetClass               dialogLocaleWidgetClass
#define XawDialogAddButton              XawDialogLocaleAddButton
#define XawDialogGetValueString         XawDialogLocaleGetValueString

#endif _XAWI18N_H
