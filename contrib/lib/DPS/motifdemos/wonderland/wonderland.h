/*
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
#include <X11/Xos.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/Scale.h>
#include <DPS/psops.h>
#include <DPS/dpsXclient.h>
#include "wwraps.h"

typedef int boolean;

typedef struct colorstruct {
  	int red;
	int green;
	int blue;
} * RGBColor;

typedef struct {
  char *imageFileName;
  char *font;
  char *fallbackImage;
} AppData;

extern AppData appData;
extern boolean nobuf;
extern Display *dpy;

extern RGBColor square_fore,
  	circle_fore,
  	text_fore,
	image_fore;

extern int   	image_back_val,
  	text_back_val,
  	circle_back_val,
  	square_back_val;

extern int square_rotate_val,
  	square_scale_val,
	circle_scale_val,
	circle_rotate_val,
	text_scale_val,
  	text_rotate_val,
  	image_scale_val,
  	image_rotate_val;

extern Widget       square,
       circle,
       text,
       image;

extern Widget  square_fore_red,
  	square_fore_green,
	square_fore_blue,
  	square_back,
	square_scale,
	square_rotate,

	circle_fore_red,
  	circle_fore_green,
	circle_fore_blue,
  	circle_back,
	circle_scale,
	circle_rotate,

	text_fore_red,
  	text_fore_green,
	text_fore_blue,
  	text_back,
	text_scale,
	text_rotate,

	image_fore_red,
  	image_fore_green,
	image_fore_blue,
  	image_back,
	image_scale,
	image_rotate;
extern void init_square();
extern void init_circle();
extern void init_text();
extern void init_image();

#define BASE_WIDTH	900
#define BASE_HEIGHT	500
#define DRAW_WIDTH	200
#define DRAW_HEIGHT   	200

#define SCALE	100
#define ROTATE  0
