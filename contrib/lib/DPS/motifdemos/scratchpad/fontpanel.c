/* fontpanel.c
 *
 * Copyright (C) 1990-1991 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
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

/* C headers */
#include <math.h>

/* Xt toolkit headers */
#include <X11/Xos.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <DPS/FontSB.h>

#include "list.h"	/* for bbox struct */

#define MAXLINESIZE 256

static char currentfont[256] = "Helvetica";
static float currentsize = 36;
static bboxStruct currentbbox;

#define SetCurrentFontInfo(name, size, bbox)                                  \
      PSWfontbbox(name, size, &(bbox.llx), &(bbox.lly),                       \
            &(bbox.urx), &(bbox.ury));                                        \
      SetCursorSize(bbox.urx-bbox.llx, bbox.ury-bbox.lly);

InitFonts()
{
    SetCurrentFontInfo(currentfont, currentsize, currentbbox);
    updateSelectedFont(currentfont, currentsize, &currentbbox);
}

void CurrentFontInfo (name, size, bbox)
    char *name;
    float *size;
    bboxStruct *bbox;
{
    strcpy(name, currentfont);
    *size = currentsize;

    bbox->llx = currentbbox.llx;
    bbox->lly = currentbbox.lly;
    bbox->urx = currentbbox.urx;
    bbox->ury = currentbbox.ury;
}

void CheckFont(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FSBValidateCallbackRec *cb = (FSBValidateCallbackRec *) callData;

    if (cb->family_selection != FSBOne && cb->face_selection != FSBOne &&
	cb->size_selection != FSBOne && cb->name_selection != FSBOne) {
	cb->doit = FALSE;
    }
}

void SetFont(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FSBCallbackRec *cb = (FSBCallbackRec *) callData;

    (void)strcpy(currentfont, cb->name);
    currentsize = cb->size;

    SetCurrentFontInfo(currentfont, currentsize, currentbbox);
    updateSelectedFont(currentfont, currentsize, &currentbbox);
}

