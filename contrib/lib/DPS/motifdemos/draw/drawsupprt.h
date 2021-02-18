/* drawsupport.h - Interface to draw support routines
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

#ifndef	DRAWSUPPORT_H
#define	DRAWSUPPORT_H

#include <X11/Xlib.h>

/*	===== BEGIN	TYPES/CONSTANTS =====	*/
/* The following lines define constants to use	*/
/* as indices into DPS matrices.		*/
#define	MatrixA		0
#define	MatrixB		1
#define	MatrixC		2
#define	MatrixD		3
#define	MatrixTX	4
#define	MatrixTY	5

typedef struct {
  float gridSize;
  float halfGrid;
  int gridOn;
  } GridData;

/* This struct contains elements that allow one	*/
/* to map between user and window space.	*/
typedef struct {
  float ctm[6], itm[6];
  int xOffset, yOffset;
  GridData grid;
  } MappingDataRec, *MappingData;
/*	===== END	TYPES/CONSTANTS =====	*/


/*	===== BEGIN	PROC DECLS =====	*/
extern void ToUserSpace(/*
  float *resultUX, *resultUY;
  MappingData mappingData;
  int x, y; /*);

extern void ToWindowSpace(/*
  int *resultX, *resultY;
  MappingData mappingData;
  float *ux, *uy; */);

extern XRectangle MakeXBBox(/*
  float *llx, *lly, *urx, *ury;
  MappingData mappingData; */);

extern XRectangle MergeBounds(/* XRectangle b1, b2; */);

/*	===== END	PROC DECLS =====	*/

#endif	DRAWSUPPORT_H
