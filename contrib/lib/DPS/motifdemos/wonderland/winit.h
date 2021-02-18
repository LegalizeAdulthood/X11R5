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
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/Form.h>
#include <Xm/ToggleB.h>
#include <DPS/psops.h>
#include <DPS/dpsXclient.h>
#include "wwraps.h"

extern void set_up_colors();
extern void set_up_drawing_wins();
extern void set_up_separators();
extern void set_up_toggle_button();
extern void set_up_labels();
extern void set_up_slider_panels();
extern void init_square_sliders();
extern void init_circle_sliders();
extern void init_text_sliders();
extern void init_image_sliders();

extern void init_square();
extern void init_circle();
extern void init_text();
extern void init_image();
extern void on_off();

extern void square_fore_red_moved();
extern void square_fore_green_moved();
extern void square_fore_blue_moved();
extern void square_back_moved();
extern void square_scale_moved();
extern void square_rotate_moved();

extern void circle_fore_red_moved();
extern void circle_fore_green_moved();
extern void circle_fore_blue_moved();
extern void circle_back_moved();
extern void circle_scale_moved();
extern void circle_rotate_moved();

extern void text_fore_red_moved();
extern void text_fore_green_moved();
extern void text_fore_blue_moved();
extern void text_back_moved();
extern void text_scale_moved();
extern void text_rotate_moved();

extern void image_fore_red_moved();
extern void image_fore_green_moved();
extern void image_fore_blue_moved();
extern void image_back_moved();
extern void image_scale_moved();
extern void image_rotate_moved();

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

#define DRAW_Y		30
#define SQUARE_X	120
#define CIRCLE_X	340
#define TEXT_X		560
#define IMAGE_X		780

#define FORM_Y			250
#define FORE_RED_Y		0
#define FORE_GREEN_Y		30
#define FORE_BLUE_Y		60
#define BACK_Y			100
#define SCALE_Y			130
#define ROTATE_Y		160
#define SLIDER_WIDTH		200
#define SEP_BOTTOM_Y		200

#define LABELS_RIGHT		110

#define FORE_RED	1000
#define FORE_GREEN	1000
#define FORE_BLUE	1000
#define BACK_GRAY	500

#define SCALE	100
#define ROTATE  0

#define IMAGE_SCALE_MAX 400
#define TOGGLE_X	820
#define TOGGLE_Y 	450
