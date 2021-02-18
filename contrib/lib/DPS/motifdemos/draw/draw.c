/* draw.c - The draw application
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

#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/Form.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/Scale.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/Text.h>
#include <DPS/XDPS.h>
#include <DPS/XDPSlib.h>
#include <DPS/dpsXclient.h>

#include "draw.h"
#include "drawsupprt.h"
#include "objects.h"
#include "drawwraps.h"
#include "textmgr.h"
#include "colormgr.h"
#include <X11/Xlib.h>

/*      ===== BEGIN     MODULE GLOBALS =====    */
XmStringCharSet charSet = "ISOLatin1";
/*      ===== END       MODULE GLOBALS =====    */



/*	===== BEGIN	INLINE PROCEDURES =====	*/

#define	UpdateWindow(data, bbox)			\
	{						\
	DPSWaitContext((data)->dpsCtxt);			\
	UpdateDrawable(					\
	  (data), (bbox), (data)->canvasData.stagingPixmap,	\
	  XtWindow((data)->canvasData.canvas));		\
	}

#define	UpdateBackup(data, bbox)			\
	{						\
	DPSWaitContext((data)->dpsCtxt);			\
	UpdateDrawable(					\
	  (data), (bbox), (data)->canvasData.stagingPixmap,	\
	  (data)->canvasData.backupPixmap);		\
	}

#define	UpdateStagingArea(data, bbox)			\
	UpdateDrawable(					\
	  (data), (bbox), (data)->canvasData.backupPixmap,	\
	  (data)->canvasData.stagingPixmap);

#define	UpdateFullBackup(data)					\
	{							\
	DPSWaitContext((data)->dpsCtxt);				\
	UpdateDrawable(						\
	  (data), nullBBox, (data)->canvasData.stagingPixmap,	\
	  (data)->canvasData.backupPixmap);			\
	}

#define	UpdateFullWindow(data)					\
	{							\
	DPSWaitContext((data)->dpsCtxt);				\
	UpdateDrawable(						\
	  (data), nullBBox, (data)->canvasData.stagingPixmap,	\
	  XtWindow((data)->canvasData.canvas));			\
	}
/*	===== END	INLINE PROCEDURES =====	*/

/*	===== BEGIN	GLOBAL VARIABLES =====	*/
ObjTypeDesc objDescriptors[CountTypes];
/*	===== END	GLOBAL VARIABLES =====	*/

/*	===== BEGIN	LOCAL VARIABLES =====	*/
static XRectangle nullBBox = {0, 0, 0, 0};
#if	BROKENWM
  /* The following is a GROSS HACK to get around the fact that	*/
  /* the WM doesn't want to pass the mouse down event to the	*/
  /* drawing area widget when that event is also used to focus	*/
  /* in on the window. Various "Pass thru" resources have no	*/
  /* effect on the problem.					*/
static int mouseDownAlready;
#endif	BROKENWM
/*	===== END	LOCAL VARIABLES =====	*/


/*	===== BEGIN	PRIVATE ROUTINES =====	*/
/*	----- BEGIN	UPDATE ROUTINES -----	*/
static void UpdateDrawable(data, bbox, from, to)
  DrawData *data;
  XRectangle bbox;
  Drawable from, to;
  {
  if (bbox.width == 0)	/* Null BBox, copy everything		*/
    {
    bbox.width = data->canvasData.width;
    bbox.height = data->canvasData.height;
    }
  if (bbox.x < 0)
    {
    bbox.width += bbox.x;
    bbox.x = 0;
    }
  if (bbox.y < 0)
    {
    bbox.height += bbox.y;
    bbox.y = 0;
    }
  XCopyArea(
    XtDisplay(data->canvasData.canvas), from,
    to, data->dpsGC,
    bbox.x, bbox.y, bbox.width, bbox.height, bbox.x, bbox.y);
  XFlush(XtDisplay(data->canvasData.canvas));
  }

static void RefreshObjects(data)
  DrawData *data;
  {
  GenericObject *obj;
  DPSContext ctxt = data->dpsCtxt;
  
  DPSgsave(ctxt);
  for (obj = data->objects; obj != NULL; obj = obj->next)
    {
    DPSsetrgbcolor(ctxt, obj->red, obj->green, obj->blue);
    (*objDescriptors[obj->objectType].drawObject)(ctxt, obj, ReDraw);
    }
  DPSgrestore(ctxt);
  UpdateFullBackup(data);
  }
/*	----- END	UPDATE ROUTINES -----	*/

/*	----- BEGIN	GRID ROUTINES -----	*/
static void RefreshGrid(data)
  DrawData *data;
  {
  float ux, uy;
  
  ToUserSpace(
    &ux, &uy, &data->canvasData.mappingData, (int)(data->canvasData.width), 0);
  PSWDrawGrid(
    data->dpsCtxt, data->canvasData.mappingData.grid.gridSize, ux, uy, 0.0);
  }

static void GridChange(w, data, callData)
  Widget w;
  DrawData *data;
  XmToggleButtonCallbackStruct *callData;
  {
  DPSerasepage(data->dpsCtxt);
  if (callData->set) RefreshGrid(data);
  RefreshObjects(data);
  UpdateFullWindow(data);
  UpdateFullBackup(data);
  data->canvasData.mappingData.grid.gridOn = callData->set;
  }

static void ChangeGridSize(w, data, callData)
  Widget w;
  DrawData *data;
  XmScaleCallbackStruct *callData;
  {
  data->canvasData.mappingData.grid.gridSize = callData->value;
  data->canvasData.mappingData.grid.halfGrid = callData->value/2;
  if (data->canvasData.mappingData.grid.gridOn == 1)
    {
    DPSerasepage(data->dpsCtxt);
    RefreshGrid(data);
    RefreshObjects(data);
    UpdateFullWindow(data);
    UpdateFullBackup(data);
    }
  }

static void CreateGridControls(parent, data)
  Widget parent;
  DrawData *data;
  {
  Arg wargs[5];
  Widget controls, w, rb;
  int value;
  int i;

  controls = XtCreateManagedWidget(
    "gridControls", xmRowColumnWidgetClass, parent, NULL, 0);

  w = XtCreateManagedWidget(
    "gridScale", xmScaleWidgetClass, controls, (Arg *) NULL, 0);
  XtAddCallback(w, XmNvalueChangedCallback, ChangeGridSize, data);
  XtAddCallback(w, XmNdragCallback, ChangeGridSize, data);
  
  i = 0;
  XtSetArg(wargs[i], XmNvalue, &value);		i++;
  XtGetValues(w, wargs, i);

  w = XtCreateManagedWidget(
    "grid", xmToggleButtonWidgetClass, controls, NULL, 0);
  XtAddCallback(w, XmNvalueChangedCallback, GridChange, data);

  data->canvasData.mappingData.grid.gridOn = 0;
  data->canvasData.mappingData.grid.gridSize = value;
  data->canvasData.mappingData.grid.halfGrid = value/2;
  }
/*	----- END	GRID ROUTINES -----	*/

/*	----- BEGIN	CANVAS ROUTINES -----	*/
static void DoneWithMark(data)
  DrawData *data;
  {
  GenericObject *obj = data->curObj;
  
  if (data->objects == NULL)
    {
    data->objects = data->lastObj = obj;
    obj->next = obj->prev = NULL;
    }
  else
    {
    data->lastObj->next = obj;
    obj->prev = data->lastObj;
    obj->next = NULL;
    data->lastObj = obj;
    }
  }

static void RefreshCanvas(w, data, clientData)
  Widget w;
  DrawData *data;
  void *clientData;
  {
  UpdateFullWindow(data);
  }

static void CanvasResize(w, data, callData)
  Widget w;
  DrawData *data;
  caddr_t callData;
  {
  Display *dpy;
  Arg wargs[2];
  Dimension width, height;
  CanvasData *cData = &data->canvasData;

  if (!XtIsRealized(w)) return;

  XtSetArg(wargs[0], XtNheight, &height);
  XtSetArg(wargs[1], XtNwidth, &width);
  XtGetValues(w, wargs, 2);
  
  dpy = XtDisplay(cData->canvas);
  cData->width = width;
  cData->height = height;
  XFreePixmap(dpy, cData->backupPixmap);
  XFreePixmap(dpy, cData->stagingPixmap);
  cData->backupPixmap = XCreatePixmap(
    dpy, DefaultRootWindow(dpy), (unsigned int)width, (unsigned int)height,
    DefaultDepthOfScreen(DefaultScreenOfDisplay(dpy)));
  cData->stagingPixmap = XCreatePixmap(
    dpy, DefaultRootWindow(dpy), (unsigned int)width, (unsigned int)height,
    DefaultDepthOfScreen(DefaultScreenOfDisplay(dpy)));
  PSWSetXGCDrawable(
    data->dpsCtxt, data->dpsGC->gid, cData->stagingPixmap,
    0, (int)height);
  cData->mappingData.yOffset = (int)height;
  DPSerasepage(data->dpsCtxt); UpdateFullBackup(data);
  if (data->canvasData.mappingData.grid.gridOn) RefreshGrid(data);
  RefreshObjects(data);
  }

static void InitCanvas(data)
  DrawData *data;
  {
  Arg wargs[2];
  DPSContext ctxt;
  CanvasData *cData = &data->canvasData;
  Widget canvas = cData->canvas;
  Display *dpy = XtDisplay(canvas);

  XGrabButton(
    XtDisplay(canvas), AnyButton, AnyModifier, XtWindow(canvas), TRUE,
    ButtonPressMask | ButtonMotionMask | ButtonReleaseMask,
    GrabModeAsync, GrabModeAsync, XtWindow(canvas),
    XCreateFontCursor(XtDisplay(canvas), XC_crosshair));

  XtSetArg(wargs[0], XtNheight, &cData->height);
  XtSetArg(wargs[1], XtNwidth, &cData->width);
  XtGetValues(canvas, wargs, 2);

  XSetForeground(dpy, data->dpsGC, BlackPixel (dpy, DefaultScreen (dpy)));
  XSetBackground(dpy, data->dpsGC, WhitePixel (dpy, DefaultScreen (dpy)));
  
  cData->stagingPixmap = XCreatePixmap(
    dpy, DefaultRootWindow(dpy),
    (unsigned int)(cData->width), (unsigned int)(cData->height),
    DefaultDepthOfScreen(DefaultScreenOfDisplay(dpy)));

  cData->backupPixmap = XCreatePixmap(
    dpy, DefaultRootWindow(dpy),
    (unsigned int)(cData->width), (unsigned int)(cData->height),
    DefaultDepthOfScreen(DefaultScreenOfDisplay(dpy)));

  PSWSetXGCDrawable(
    data->dpsCtxt, data->dpsGC->gid, cData->stagingPixmap,
    0, (int)(cData->height));

  PSWInitTransform(data->dpsCtxt);

  PSWGetTransform(
    data->dpsCtxt, cData->mappingData.ctm, cData->mappingData.itm,
    &cData->mappingData.xOffset, &cData->mappingData.yOffset);
  }

static void MouseDown(w, data, event)
  Widget w;
  DrawData *data;
  XEvent *event;
  {
  ObjTypeDesc *curObjDesc;
  GenericObject *obj;
  XRectangle bbox;
  float r, g, b;
  
#if	BROKENWM
  /* The following is a GROSS HACK to get around the fact that	*/
  /* the WM doesn't want to pass the mouse down event to the	*/
  /* drawing area widget when that event is also used to focus	*/
  /* in on the window. Various "Pass thru" resources have no	*/
  /* effect on the problem.					*/
  mouseDownAlready = 1;
#endif	BROKENWM

  if ( (curObjDesc = data->curObjDesc) == NULL) return;
  data->curObj = obj = (*curObjDesc->createObject)(
    curObjDesc->type, data->dpsCtxt);

  GetColors(data->colorPopup, &r, &g, &b);
  DPSsetrgbcolor(
    data->dpsCtxt, (obj->red = r), (obj->green = g), (obj->blue = b));
  
  ToUserSpace(
    &obj->x1, &obj->y1,
    &(data->canvasData.mappingData), event->xbutton.x, event->xbutton.y);

  obj->x2 = obj->x1;
  obj->y2 = obj->y1;

  (*(curObjDesc->computeBBox))(
    data->dpsCtxt, obj, &(data->canvasData.mappingData), &bbox);
  obj->bbox = bbox;

  UpdateStagingArea(data, bbox);

  (*(curObjDesc->drawObject))(data->dpsCtxt, obj, InitialDraw);

  UpdateWindow(data, bbox);
  }

static void MouseMove(w, data, event)
  Widget w;
  DrawData *data;
  XEvent *event;
  {
  ObjTypeDesc *curObjDesc;
  GenericObject *obj;
  XRectangle bbox, mergedBBox;
  XEvent nextEvent;
  int gotOne;

#if	BROKENWM
  /* The following is a GROSS HACK to get around the fact that	*/
  /* the WM doesn't want to pass the mouse down event to the	*/
  /* drawing area widget when that event is also used to focus	*/
  /* in on the window. Various "Pass thru" resources have no	*/
  /* effect on the problem.					*/
  if (!mouseDownAlready)
    {
    MouseDown(w, data, event);
    return;
    }
#endif	BROKENWM

  gotOne = 0;
  while (XtPending())
    {
    XtPeekEvent(&nextEvent);
    if (nextEvent.type == NoExpose || nextEvent.type == MotionNotify)
      {XtNextEvent(&nextEvent); gotOne = 1;}
    else break;
    }
  if (gotOne && nextEvent.type == MotionNotify) event = &nextEvent;

  if ( (curObjDesc = data->curObjDesc) == NULL) return;
  obj = data->curObj;

  ToUserSpace(
    &obj->x2, &obj->y2,
    &(data->canvasData.mappingData), event->xbutton.x, event->xbutton.y);

  (*(curObjDesc->computeBBox))(
    data->dpsCtxt, obj, &(data->canvasData.mappingData), &bbox);
  mergedBBox = MergeBounds(obj->bbox, bbox);
  obj->bbox = bbox;

  UpdateStagingArea(data, mergedBBox); 

  (*(curObjDesc->drawObject))(data->dpsCtxt, obj, IntermediateDraw);

  UpdateWindow(data, mergedBBox);
  }

static void MouseUp(w, data, event)
  Widget w;
  DrawData *data;
  XEvent *event;
  {
  GenericObject *obj;
  ObjTypeDesc *curObjDesc;
  XRectangle bbox, mergedBBox;

  if ( (curObjDesc = data->curObjDesc) == NULL) return;
  obj = data->curObj;

  ToUserSpace(
    &obj->x2, &obj->y2,
    &(data->canvasData.mappingData), event->xbutton.x, event->xbutton.y);

  (*(curObjDesc->computeBBox))(
    data->dpsCtxt, obj, &(data->canvasData.mappingData), &bbox);
  mergedBBox = MergeBounds(obj->bbox, bbox);
  obj->bbox = bbox;

  UpdateStagingArea(data, mergedBBox);

  (*(curObjDesc->drawObject))(data->dpsCtxt, obj, FinalDraw);

  UpdateWindow(data, mergedBBox);
  UpdateBackup(data, mergedBBox);
  
  DoneWithMark(data);
#if	BROKENWM
  /* The following is a GROSS HACK to get around the fact that	*/
  /* the WM doesn't want to pass the mouse down event to the	*/
  /* drawing area widget when that event is also used to focus	*/
  /* in on the window. Various "Pass thru" resources have no	*/
  /* effect on the problem.					*/
  mouseDownAlready = 0;
#endif	BROKENWM
  }

static void Erase(w, data, callData)
  Widget w;
  DrawData *data;
  char *callData;
  {
  GenericObject *obj;

  DPSerasepage(data->dpsCtxt);
  if (data->canvasData.mappingData.grid.gridOn) RefreshGrid(data);
  DPSWaitContext(data->dpsCtxt);
  UpdateFullWindow(data);
  UpdateFullBackup(data);

  obj = data->objects;
  while (obj != NULL)
    {
    GenericObject *next = obj->next;
    if (obj->objectType == TextType) 
      {
      /*
      Do something with all of those font name
      and text strings. How are they to be freed?
      */
      }
    free((char *)obj);
    obj = next;
    }
  data->objects = data->lastObj = NULL;
  }

static void CreateCanvas(parent, data)
  Widget parent;
  DrawData *data;
  {
  Widget canvas;
  
  canvas = XtCreateManagedWidget(
    "canvas", xmDrawingAreaWidgetClass, parent, NULL, 0);
  XtAddCallback(canvas, XmNexposeCallback, RefreshCanvas, data);
  XtAddCallback(canvas, XmNresizeCallback, CanvasResize, data);
  
  XtAddEventHandler(canvas, ButtonPressMask, FALSE, MouseDown, data);
  XtAddEventHandler(canvas, ButtonMotionMask, FALSE, MouseMove, data);
  XtAddEventHandler(canvas, ButtonReleaseMask, FALSE, MouseUp, data);
  data->canvasData.canvas = canvas;
  }
/*	----- END	CANVAS ROUTINES -----	*/

/*	----- BEGIN	COMMAND ROUTINES -----	*/
void ObjectSelected(w, data, callData)
  Widget w;
  DrawData *data;
  XmToggleButtonCallbackStruct *callData;
  {
  Arg wargs[1];
  ObjTypeDesc *objDesc;

  XtSetArg(wargs[0], XmNuserData, &objDesc);
  XtGetValues(w, wargs, 1);

  if (callData->set)
    {
    data->curObjDesc = objDesc;
    if (data->curObjDesc != NULL && data->curObjDesc->objectSelected)
      (data->curObjDesc->objectSelected)();
    }
  else if (data->curObjDesc->objectDeselected)
    (data->curObjDesc->objectDeselected)();
  }

Widget CreateDrawingCommands(parent, data)
  Widget parent;
  DrawData *data;
  {
  int i;
  Arg wargs[1];
  ObjTypeDesc *cur;
  Widget w, commands;

  /* Here is the major kludge we do to initialize...	*/
  InitStdObjects(parent, data->dpsCtxt);
  InitTextObject(parent, data->dpsCtxt, data);

  XtSetArg(wargs[0], XmNentryClass, xmToggleButtonGadgetClass);
  commands = XmCreateRadioBox(parent, "commands", wargs, 1);
  XtManageChild(commands);

  for (i = CountTypes - 1, cur = &objDescriptors[i]; i >= 0; i--, cur--)
    {
    XtSetArg(wargs[0], XmNuserData, cur);
    w = XmCreateToggleButtonGadget(commands, cur->label, wargs, 1);
    XtManageChild(w);
    XtAddCallback(w, XmNvalueChangedCallback, ObjectSelected, data);
    }

  return(commands);
  }
/*	----- END	COMMAND ROUTINES -----	*/

static void Quit(w, clientData, callData)
  Widget w;
  caddr_t clientData;
  caddr_t callData;
  {
  XCloseDisplay(XtDisplay(w));
  exit(0);
  }

static void PopupColorEditor(w, popup, callData)
  Widget w;
  Widget popup;
  caddr_t callData;
  {
  XtManageChild(popup);
  XRaiseWindow(XtDisplay(popup), XtWindow(XtParent(popup)));
  PokeColorEditor(popup);
  }

static void PopupFontPanel(w, popup, callData)
  Widget w;
  Widget popup;
  caddr_t callData;
  {
  XtPopup(XtParent(popup), XtGrabNone);
  XRaiseWindow(XtDisplay(popup), XtWindow(XtParent(popup)));
  }
/*	===== END	PRIVATE ROUTINES =====	*/


String fallback_resources[] = {
  "*XmScale.Orientation:			HORIZONTAL",
  "*XmScale.ProcessingDirection:		MAX_ON_RIGHT",
  "*framework.width:				600",
  "*framework.height:				512",
  NULL
  };

main(argc, argv)
  int argc;
  char *argv[];
  {
  Arg wargs[5];
  Display *dpy;
  XtAppContext app;
  Widget topLevel, canvas, framework, command, w, popup;
  DrawData data;

  data.curObjDesc = NULL;
  data.objects = data.lastObj = NULL;

  topLevel = XtAppInitialize(&app, "Draw", NULL, 0, &argc, argv,
			     fallback_resources, (ArgList)NULL, 0);
  framework = XtCreateManagedWidget(
    "framework", xmFormWidgetClass, topLevel, NULL, 0);

  /* Create column to hold commands... */
  XtSetArg(wargs[0], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(wargs[1], XmNbottomAttachment, XmATTACH_FORM);
  XtSetArg(wargs[2], XmNleftAttachment, XmATTACH_FORM);
  command = XtCreateManagedWidget(
    "command", xmRowColumnWidgetClass, framework, wargs, 3);

  w = XtCreateManagedWidget(
    "quit", xmPushButtonWidgetClass, command, NULL, 0);
  XtAddCallback(w, XmNactivateCallback, Quit, NULL);

  w = XtCreateManagedWidget(
    "erase", xmPushButtonWidgetClass, command, NULL, 0);
  XtAddCallback(w, XmNactivateCallback, Erase, &data);

  w = XtCreateManagedWidget(
    "color", xmPushButtonWidgetClass, command, NULL, 0);

  data.colorPopup = CreateColorEditor(
    command, NULL, &data, (float)InitRed, (float)InitGreen, (float)InitBlue);
  XtAddCallback(w, XmNactivateCallback, PopupColorEditor, data.colorPopup);

  w = XtCreateManagedWidget(
    "fonts", xmPushButtonWidgetClass, command, NULL, 0);
  data.fontPanel = CreateFontPanel(w, &data);
  XtAddCallback(w, XmNactivateCallback, PopupFontPanel, data.fontPanel);

  dpy = XtDisplay(topLevel);
  data.dpsGC = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)), 0, NULL);
  data.dpsCtxt = XDPSCreateSimpleContext(
    dpy, 0, 0, 0, 0, DPSDefaultTextBackstop, DPSDefaultErrorProc, NULL);
  if (data.dpsCtxt == NULL) exit(-1);
  DPSSetContext(data.dpsCtxt);

#ifdef	JPDEBUG
  {
  DPSContext textCtxt = 
    DPSCreateTextContext(DPSDefaultTextBackstop, DPSDefaultErrorProc);
  DPSChainContext(data.dpsCtxt, textCtxt);
  }
#endif

  w = XtCreateManagedWidget("textString", xmLabelWidgetClass,
			    command, NULL, 0);
  data.textField = XtCreateManagedWidget("textField", xmTextWidgetClass,
					 command, NULL, 0);
  CreateDrawingCommands(command, &data);
  CreateGridControls(command, &data);
  CreateCanvas(framework, &data);
  XtSetArg(wargs[0], XmNtopAttachment, XmATTACH_FORM);
  XtSetArg(wargs[1], XmNbottomAttachment, XmATTACH_FORM);
  XtSetArg(wargs[2], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(wargs[3], XmNleftAttachment, XmATTACH_WIDGET);
  XtSetArg(wargs[4], XmNleftWidget, command);
  XtSetValues(data.canvasData.canvas, wargs, 5);

  XtRealizeWidget(topLevel);

  /* Handle post-Realize initialization	*/
  InitCanvas(&data);
  InitColorEditor(data.colorPopup, data.dpsCtxt);
  PSWLoadProcs(data.dpsCtxt);
  DPSerasepage(data.dpsCtxt); UpdateFullBackup(&data);

#if	BROKENWM
  /* The following is a GROSS HACK to get around the fact that	*/
  /* the WM doesn't want to pass the mouse down event to the	*/
  /* drawing area widget when that event is also used to focus	*/
  /* in on the window. Various "Pass thru" resources have no	*/
  /* effect on the problem.					*/
  mouseDownAlready = 0;
#endif	BROKENWM

  XtAppMainLoop(app);
  }
