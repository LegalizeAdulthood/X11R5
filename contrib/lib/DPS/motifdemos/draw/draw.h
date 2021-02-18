/* draw.h - Private definitions for the draw application
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

#ifndef	DRAW_H
#define	DRAW_H

#include "objects.h"
#include "drawsupprt.h"
#include <DPS/dpsclient.h>

/*	===== BEGIN	CONSTANTS/TYPES =====	*/

#define	InitRed		0
#define	InitGreen	0
#define	InitBlue	0

/* This struct contains the DPS related data	*/
/* that the application must keep track of.	*/
typedef struct {
  Widget canvas;
  Pixmap backupPixmap, stagingPixmap;
  Dimension width, height;
  MappingDataRec mappingData;
  } CanvasData;

/* This is the struct that maintains all data	*/
/* that is of interest to the application.	*/
typedef struct {
  GC dpsGC;
  DPSContext dpsCtxt;
  GenericObject *curObj;
  ObjTypeDesc *curObjDesc;
  XRectangle (*computeBBox)();
  GridData grid;
  Widget colorPopup;
  Widget fontPanel;
  Widget textField;
  char *currentFont;
  float currentSize;
  CanvasData canvasData;
  GenericObject *objects;
  GenericObject *lastObj;
  } DrawData;
/*	===== END	CONSTANTS/TYPES =====	*/

#endif	DRAW_H
