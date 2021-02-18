/* textmgr.c - Implements the Font parameters Popup
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

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/Text.h>
#include <DPS/FontSB.h>
#include <malloc.h>
#include "textmgr.h"
#include "fontwraps.h"
#include "draw.h"

#define	PopdownButton		0
  /* We don't want a Popdown Button in the panel	*/

/*	===== BEGIN	CONSTANTS/INLINES =====	*/
#define DefaultFontSize		24
static char *defaultFont = "Helvetica";
static char defaultFontLength = 11;
/*	===== END	CONSTANTS/INLINES =====	*/


/*	===== BEGIN	TYPES =====		*/
typedef struct {
  Widget textField, fontScale;
  char *fontName;
  int sizeOfFontNameString;
  } FontData;
/*	===== END	TYPES =====		*/


/*	===== BEGIN	MODULE GLOBALS =====	*/
static XmString selectedItem;
/* static */ XmString *fontList = NULL;
/* static */ int countFontsInList;
/*	===== END	MODULE GLOBALS =====	*/


/*	===== BEGIN	PUBLIC ROUTINES =====	*/
void GetTextParams(data, font, text, size)
  DrawData *data;
  char **font;
  char **text;
  int *size;
{
  int fs;

  *font = XtNewString(data->currentFont);
  *text = (char *)XmTextGetString(data->textField);
  
  *size = data->currentSize;
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
    DrawData *data = (DrawData *) clientData;

    (void)strcpy(data->currentFont, cb->name);
    data->currentSize = cb->size;
}

Widget CreateFontPanel(parent, data)
    Widget parent;
    DrawData *data;
{
    Widget shell, panel;

    shell = XtCreatePopupShell("Fonts", transientShellWidgetClass,
			       parent, (ArgList) NULL, 0);

    panel = XtCreateManagedWidget("fontPanel", fontSelectionBoxWidgetClass,
				  shell, (ArgList) NULL, 0);
    XtAddCallback(panel, XtNvalidateCallback, CheckFont, (XtPointer) NULL);
    XtAddCallback(panel, XtNokCallback, SetFont, (XtPointer) data);
    XtAddCallback(panel, XtNapplyCallback, SetFont, (XtPointer) data);

    FSBSetFontName(panel, defaultFont, False);
    FSBSetFontSize(panel, (double) DefaultFontSize , False);
    data->currentFont = XtMalloc(256);
    strcpy(data->currentFont, defaultFont);
    data->currentSize = DefaultFontSize;

    return panel;
}


/*	===== END	PUBLIC PROCEDURES =====	*/
