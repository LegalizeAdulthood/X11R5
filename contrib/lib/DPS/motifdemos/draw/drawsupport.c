/* drawsupport.c - Support Routines for the draw application
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

#include "drawsupprt.h"
#include <X11/Xlib.h>

/*	===== BEGIN	INLINE ROUTINES =====	*/
#define	SnapToGrid(grid, x, y, xs, ys)	\
	xs = ((int)((x+grid.halfGrid)/grid.gridSize)) * grid.gridSize;	\
	ys = ((int)((y+grid.halfGrid)/grid.gridSize)) * grid.gridSize;
/*	===== END	INLINE ROUTINES =====	*/

/*	===== BEGIN	PRIVATE ROUTINES =====	*/
static void CheckPoints(x, y, x2, y2)
  int *x, *y, *x2, *y2;
  {
  if (*x2 < *x) { int tmp = *x; *x = *x2; *x2 = tmp;}
  if (*y2 < *y) { int tmp = *y; *y = *y2; *y2 = tmp;}
  }
/*	===== END	PRIVATE ROUTINES =====	*/

/*	===== BEGIN	PUBLIC ROUTINES =====	*/
XRectangle MergeBounds(b1, b2)
  XRectangle b1, b2;
  {
  int x1, y1, x2, y2;
  XRectangle merged;
 
  x1 = (b1.x < b2.x) ? b1.x : b2.x;
  x2 = (b1.x+b1.width > b2.x+b2.width) ? b1.x+b1.width : b2.x+b2.width;

  y1 = (b1.y < b2.y) ? b1.y : b2.y;
  y2 = (b1.y+b1.height > b2.y+b2.height) ? b1.y+b1.height : b2.y+b2.height;
  
  merged.x = x1; merged.y = y1;
  merged.width = x2 - x1 + 1;
  merged.height = y2 - y1 + 1;

  return(merged);
  }

void ToUserSpace(resultUX, resultUY, mappingData, x, y)
  float *resultUX, *resultUY;
  MappingData mappingData;
  int x, y;
  {
  x -= mappingData->xOffset;
  y -= mappingData->yOffset;
  
  *resultUX = mappingData->itm[MatrixA] * x
            + mappingData->itm[MatrixC] * y
            + mappingData->itm[MatrixTX]; 
  *resultUY = mappingData->itm[MatrixB] * x
            + mappingData->itm[MatrixD] * y
            + mappingData->itm[MatrixTY]; 

  if (mappingData->grid.gridOn)
    {
    SnapToGrid(mappingData->grid, *resultUX, *resultUY, *resultUX, *resultUY);
    }
  }

void ToWindowSpace(resultX, resultY, mappingData, ux, uy)
  int *resultX, *resultY;
  MappingData mappingData;
  float *ux, *uy;
  {
  *resultX = mappingData->ctm[MatrixA] * *ux
           + mappingData->ctm[MatrixC] * *uy
           + mappingData->ctm[MatrixTX] + mappingData->xOffset; 
  *resultY = mappingData->ctm[MatrixB] * *ux
           + mappingData->ctm[MatrixD] * *uy
           + mappingData->ctm[MatrixTY] + mappingData->yOffset; 
  }

XRectangle MakeXBBox(llx, lly, urx, ury, mappingData)
  float *llx, *lly, *urx, *ury;
  MappingData mappingData;
  {
  int x, y, x1, y1;
  XRectangle rect;

  ToWindowSpace(&x, &y, mappingData, llx, lly);
  ToWindowSpace(&x1, &y1, mappingData, urx, ury);
  CheckPoints(&x, &y, &x1, &y1);

  rect.x = x - 1;
  rect.y = y - 1;
  rect.width = x1 - x + 3;	/* == (x1 - x + 1) + 2 fudge	*/
  rect.height = y1 - y + 3;	/* == (y1 - y + 1) + 2 fudge	*/
  
  return(rect);
  }
/*	===== END	PUBLIC ROUTINES =====	*/
