/* stdobjects.c - Manages several simple object types
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
#include "objects.h"
#include "drawsupprt.h"
#include "stdwraps.h"
#include <DPS/dpsXclient.h>

/*	===== BEGIN	TYPES/CONSTANTS =====		*/
#define	CountStdTypes	5
#define	BaseStdTypes	0
/*	===== END	TYPES/CONSTANTS =====		*/


/*	===== BEGIN	MODULE GLOBALS =====		*/
static void (*drawFunctions[CountStdTypes])();
/*	===== END	MODULE GLOBALS =====		*/

/*	===== BEGIN	PRIVATE PROCEDURES =====	*/
static void ComputeBBoxForStdObject(ctxt, obj, mappingData, bbox)
  DPSContext ctxt;
  GenericObject *obj;
  MappingData mappingData;
  XRectangle *bbox;
  {
  *bbox = MakeXBBox(
    &obj->x1, &obj->y1, &obj->x2, &obj->y2, mappingData);

  if (obj->objectType == CircleType
  || obj->objectType == FilledCircleType)
    {
    if (obj->x1 < obj->x2)
      bbox->x -= bbox->width;
    if (obj->y1 > obj->y2)
      bbox->y -= bbox->height;
    bbox->width *= 2;
    bbox->height *= 2;
    }
  }

void DrawStdObject(ctxt, obj, mode)
  DPSContext ctxt;
  GenericObject *obj;
  DrawMode mode;
  {
  (*(drawFunctions[obj->objectType - BaseStdTypes]))(
    ctxt, obj->x1, obj->y1, obj->x2, obj->y2);
  }

GenericObject *CreateStdObject(type, ctxt)
  ObjectType type;
  DPSContext ctxt;
  {
  GenericObject *obj = (GenericObject *)malloc(sizeof(GenericObject));

  obj->objectType = type;
  return(obj);
  }
/*	===== END	PRIVATE PROCEDURES =====	*/

/*	===== BEGIN	PUBLIC PROCEDURES =====	*/
void InitStdObjects(parent, ctxt)
  Widget parent;
  DPSContext ctxt;
  {
  register ObjTypeDesc *objs;
  
  drawFunctions[0] = PSWDrawLine;
  drawFunctions[1] = PSWDrawRect;
  drawFunctions[2] = PSWDrawCircle;
  drawFunctions[3] = PSWDrawFilledRect;
  drawFunctions[4] = PSWDrawFilledCircle;
  
  objs = objDescriptors;

  /* Fill in LineType Descriptor...		*/
  objs[LineType].type = LineType;
  objs[LineType].label = "line";
  objs[LineType].computeBBox = ComputeBBoxForStdObject;
  objs[LineType].createObject = CreateStdObject;
  objs[LineType].drawObject = DrawStdObject;
  objs[LineType].objectSelected = 0;
  objs[LineType].objectDeselected = 0;

  /* Fill in RectType Descriptor...		*/
  objs[RectType].type = RectType;
  objs[RectType].label = "rectangle";
  objs[RectType].computeBBox = ComputeBBoxForStdObject;
  objs[RectType].createObject = CreateStdObject;
  objs[RectType].drawObject = DrawStdObject;
  objs[RectType].objectSelected = 0;
  objs[RectType].objectDeselected = 0;

  /* Fill in CircleType Descriptor...		*/
  objs[CircleType].type = CircleType;
  objs[CircleType].label = "circle";
  objs[CircleType].computeBBox = ComputeBBoxForStdObject;
  objs[CircleType].createObject = CreateStdObject;
  objs[CircleType].drawObject = DrawStdObject;
  objs[CircleType].objectSelected = 0;
  objs[CircleType].objectDeselected = 0;

  /* Fill in FilledRectType Descriptor...	*/
  objs[FilledRectType].type = FilledRectType;
  objs[FilledRectType].label = "filledRectangle";
  objs[FilledRectType].computeBBox = ComputeBBoxForStdObject;
  objs[FilledRectType].createObject = CreateStdObject;
  objs[FilledRectType].drawObject = DrawStdObject;
  objs[FilledRectType].objectSelected = 0;
  objs[FilledRectType].objectDeselected = 0;

  /* Fill in FilledCircleType Descriptor...	*/
  objs[FilledCircleType].type = FilledCircleType;
  objs[FilledCircleType].label = "filledCircle";
  objs[FilledCircleType].computeBBox = ComputeBBoxForStdObject;
  objs[FilledCircleType].createObject = CreateStdObject;
  objs[FilledCircleType].drawObject = DrawStdObject;
  objs[FilledCircleType].objectSelected = 0;
  objs[FilledCircleType].objectDeselected = 0;

  }
/*	===== END	PUBLIC PROCEDURES =====	*/
