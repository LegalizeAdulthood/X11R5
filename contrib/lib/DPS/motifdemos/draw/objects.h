/* objects.h - Interface for object implementors
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

#ifndef	OBJECTS_H
#define	OBJECTS_H

#include <X11/Xlib.h>

/*	===== BEGIN	TYPES/CONSTANTS =====	*/
typedef int ObjectType;
/* Values of ObjectType...	*/
#define	LineType		0
#define	RectType		1
#define CircleType		2
#define	FilledRectType		3
#define	FilledCircleType	4
#define	TextType		5
#define	CountTypes		6

typedef int DrawMode;
/* Values of DrawMode...	*/
#define	InitialDraw		0
#define	IntermediateDraw	1
#define	FinalDraw		2
#define	ReDraw			3
/*	===== END	TYPES/CONSTANTS =====	*/


/*	===== BEGIN	TYPES =====	*/

/* This struct defines a single graphics object	*/
typedef struct _t_GenericObject {
  int objectType;
  float x1, y1, x2, y2;
  float red, green, blue;
  struct _t_GenericObject *next;
  struct _t_GenericObject *prev;
  XRectangle bbox;
  } GenericObject;

typedef struct _t_ObjTypeDesc {
  ObjectType type;
  char *label;
  void (*computeBBox)(
    /* DPSContext, GenericObject, MappingData, XRectangle* */);
  GenericObject *(*createObject)();
  void (*drawObject)();
  void (*objectSelected)();
  void (*objectDeselected)();
  char *objectData;
  } ObjTypeDesc;
  
/*	===== END	TYPES =====	*/

/*	===== BEGIN	EXTERN DECLS =====	*/
extern void InitStdObjects();
extern void InitTextObject();
extern ObjTypeDesc objDescriptors[CountTypes];
/*	===== END	EXTERN DECLS =====	*/


#endif	OBJECTS_H
