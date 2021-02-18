/* colormgr.c - The color chooser dialog box
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

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <DPS/XDPS.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>
#include "colormgr.h"
#include "colorwraps.h"


/*	===== BEGIN	TYPE DECLARATIONS =====	*/
typedef struct {
  Widget colorSampler, redScale, greenScale, blueScale;
  float red, green, blue;
  void (*setColorProc)();
  caddr_t clientData;
  DPSContext ctxt;
  } ColorData;
/*	===== END	TYPE DECLARATIONS =====	*/


/*      ===== BEGIN     MODULE GLOBALS =====    */
static XmStringCharSet charSet = "ISOLatin1";
/*      ===== END       MODULE GLOBALS =====    */

/*	===== BEGIN	PRIVATE ROUTINES =====	*/
static void Popdown(w, clientData, callData)
  Widget w;
  caddr_t clientData;
  XmAnyCallbackStruct *callData;
  {
  XtUnmanageChild((Widget)clientData);
  }

static void UpdateColorSampler(data)
  ColorData *data;
  {
  PSWUpdateColorSampler(data->ctxt, data->red, data->green, data->blue);
  DPSWaitContext(data->ctxt);
  }

static void Refresh(w, data, clientData)
  Widget w;
  ColorData *data;
  void *clientData;
  {
  UpdateColorSampler(data);
  }

static void ColorChanged(w, data, callData)
  Widget w;
  ColorData *data;
  XmScaleCallbackStruct *callData;
  {
  if (w == data->redScale) data->red = (float)callData->value/100;
  else if (w == data->greenScale) data->green = (float)callData->value/100;
  else if (w == data->blueScale) data->blue = (float)callData->value/100;

  UpdateColorSampler(data);
  }

static void UpdateColors(w, data, callData)
  Widget w;
  ColorData *data;
  char *callData;
  {
  if (data->setColorProc)
    (*(data->setColorProc))(
      data->clientData, data->red, data->green, data->blue);
  }
/*	===== END	PRIVATE ROUTINES =====	*/


/*	===== BEGIN	PUBLIC ROUTINES =====	*/
Widget CreateColorEditor(
  parent, setColorProc, clientData, initRed, initGreen, initBlue)
  Widget parent;
  void (*setColorProc)();
  caddr_t clientData;
  float initRed, initGreen, initBlue;
  {
  Arg wargs[5];
  Widget colorPanel, popup, w;
  ColorData *data = (ColorData *)malloc(sizeof(ColorData));

  data->setColorProc = setColorProc;
  data->clientData = clientData;

  XtSetArg(wargs[0], XmNautoUnmanage, FALSE);
  XtSetArg(wargs[1], XmNuserData, data);
  popup = XmCreateFormDialog(parent, "ColorSelection", wargs, 2);

  colorPanel = XtCreateManagedWidget(
    "colorPanel", xmRowColumnWidgetClass, popup, NULL, 0);

  w = XtCreateManagedWidget(
    "red", xmScaleWidgetClass, colorPanel, (Arg *) NULL, 0);
  XtAddCallback(w, XmNvalueChangedCallback, ColorChanged, data);
  XtAddCallback(w, XmNdragCallback, ColorChanged, data);
  data->redScale = w;

  w = XtCreateManagedWidget(
    "green", xmScaleWidgetClass, colorPanel, (Arg *) NULL, 0);
  XtAddCallback(w, XmNvalueChangedCallback, ColorChanged, data);
  XtAddCallback(w, XmNdragCallback, ColorChanged, data);
  data->greenScale = w;

  w = XtCreateManagedWidget(
    "blue", xmScaleWidgetClass, colorPanel, (Arg *) NULL, 0);
  XtAddCallback(w, XmNvalueChangedCallback, ColorChanged, data);
  XtAddCallback(w, XmNdragCallback, ColorChanged, data);
  data->blueScale = w;

  data->colorSampler = XtCreateManagedWidget(
    "colorSampler", xmDrawingAreaWidgetClass, colorPanel, NULL, 0);
  XtAddCallback(data->colorSampler, XmNexposeCallback, Refresh, data);

  w = XtCreateManagedWidget(
    "done", xmPushButtonWidgetClass, colorPanel, NULL, 0);
  XtAddCallback(w, XmNactivateCallback, UpdateColors, data);
  XtAddCallback(w, XmNactivateCallback, Popdown, popup);

  data->red = initRed;
  data->green = initGreen;
  data->blue = initBlue;

  return(popup);
  }

void InitColorEditor(popup, ctxt)
  Widget popup;
  DPSContext ctxt;
  {
  Arg wargs[1];
  ColorData *data;

  XtSetArg(wargs[0], XmNuserData, &data);
  XtGetValues(popup, wargs, 1);
  
  data->ctxt = ctxt;

  if (data->setColorProc)
    (*(data->setColorProc))(
      data->clientData, data->red, data->green, data->blue);
  }

void PokeColorEditor(popup)
  Widget popup;
  {
  int h;
  Arg wargs[1];
  ColorData *data;

  XtSetArg(wargs[0], XmNuserData, &data);
  XtGetValues(popup, wargs, 1);

  XFlush(XtDisplay(data->colorSampler));

  XtSetArg(wargs[0], XtNheight, &h);
  XtGetValues(data->colorSampler, wargs, 1);

  PSWCreateColorMgrGState(data->ctxt, XtWindow(data->colorSampler), h);
  }

void GetColors(popup, r, g, b)
  Widget popup;
  float *r, *g, *b;
  {
  Arg wargs[1];
  ColorData *data;

  XtSetArg(wargs[0], XmNuserData, &data);
  XtGetValues(popup, wargs, 1);

  *r = data->red;
  *g = data->green;
  *b = data->blue;
  }
