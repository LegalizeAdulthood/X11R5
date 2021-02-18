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

#include "wonderland.h"
#include "winit.h"



static XmFontList m_font;


static int max(val1, val2)
     int val1, val2;
{
  return (val1 > val2) ? val1 : val2;
}

static int min(val1, val2)
     int val1, val2;
{
  return (val1 < val2) ? val1 : val2;
}

void set_up_colors()
{
  square_fore = (RGBColor)XtMalloc(sizeof(struct colorstruct));
  circle_fore = (RGBColor)XtMalloc(sizeof(struct colorstruct));
  text_fore = (RGBColor)XtMalloc(sizeof(struct colorstruct));
  image_fore = (RGBColor)XtMalloc(sizeof(struct colorstruct));

  square_fore->red = FORE_RED;
  square_fore->green = FORE_GREEN;
  square_fore->blue = FORE_BLUE;
  
  square_back_val = BACK_GRAY;

  circle_fore->red = FORE_RED;
  circle_fore->green = FORE_GREEN;
  circle_fore->blue = FORE_BLUE;
  
  circle_back_val = BACK_GRAY;

  text_fore->red = FORE_RED;
  text_fore->green = FORE_GREEN;
  text_fore->blue = FORE_BLUE;
  
  text_back_val = BACK_GRAY;

  image_fore->red = FORE_RED;
  image_fore->green = FORE_GREEN;
  image_fore->blue = FORE_BLUE;
  
  image_back_val = BACK_GRAY;

  square_scale_val = SCALE;
  square_rotate_val = ROTATE;
  circle_scale_val = SCALE;
  circle_rotate_val = ROTATE;
  text_scale_val = SCALE;
  text_rotate_val = ROTATE;
  image_scale_val = SCALE;
  image_rotate_val = ROTATE;
}

void set_up_drawing_wins(bb)
Widget bb;
{
  Arg wargs[10];
  int n;

  n = 0;
  XtSetArg(wargs[n], XmNwidth, DRAW_WIDTH); n++;
  XtSetArg(wargs[n], XmNheight, DRAW_HEIGHT); n++;
  XtSetArg(wargs[n], XmNx, SQUARE_X); n++;
  XtSetArg(wargs[n], XmNy, DRAW_Y); n++;
  square = XtCreateManagedWidget("square", xmDrawingAreaWidgetClass, 
				 bb, wargs, n);

  XtAddCallback(square, XmNexposeCallback, init_square, NULL);

  n = 0;
  XtSetArg(wargs[n], XmNwidth, DRAW_WIDTH); n++;
  XtSetArg(wargs[n], XmNheight, DRAW_HEIGHT); n++;
  XtSetArg(wargs[n], XmNx, CIRCLE_X); n++;
  XtSetArg(wargs[n], XmNy, DRAW_Y); n++;
  circle = XtCreateManagedWidget("circle", xmDrawingAreaWidgetClass, 
				 bb, wargs, n);

  XtAddCallback(circle, XmNexposeCallback, init_circle, NULL);

  n = 0;
  XtSetArg(wargs[n], XmNwidth, DRAW_WIDTH); n++;
  XtSetArg(wargs[n], XmNheight, DRAW_HEIGHT); n++;
  XtSetArg(wargs[n], XmNx, TEXT_X); n++;
  XtSetArg(wargs[n], XmNy, DRAW_Y); n++;
  text = XtCreateManagedWidget("text", xmDrawingAreaWidgetClass, 
				 bb, wargs, n);

  XtAddCallback(text, XmNexposeCallback, init_text, NULL);

  n = 0;
  XtSetArg(wargs[n], XmNwidth, DRAW_WIDTH); n++;
  XtSetArg(wargs[n], XmNheight, DRAW_HEIGHT); n++;
  XtSetArg(wargs[n], XmNx, IMAGE_X); n++;
  XtSetArg(wargs[n], XmNy, DRAW_Y); n++;
  image = XtCreateManagedWidget("image", xmDrawingAreaWidgetClass, 
				 bb, wargs, n);

  XtAddCallback(image, XmNexposeCallback, init_image, NULL);
}

void set_up_slider_panels(bb)
Widget bb;
{
  init_square_sliders(bb);
  init_circle_sliders(bb);
  init_text_sliders(bb);
  init_image_sliders(bb);
}

void init_square_sliders(bb)
Widget bb;
{
  Arg wargs[12];
  int n;
  Widget square_form;

  XtSetArg(wargs[0], XmNx, SQUARE_X);
  XtSetArg(wargs[1], XmNy, FORM_Y);
  XtSetArg(wargs[2], XmNwidth, SLIDER_WIDTH);
  XtSetArg(wargs[3], XmNheight, 200);
  square_form = XtCreateManagedWidget("square_form", xmFormWidgetClass,
				      bb, wargs, 4);

  XtSetArg(wargs[0], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[1], XmNprocessingDirection, XmMAX_ON_RIGHT);
  XtSetArg(wargs[2], XmNminimum, 0);
  XtSetArg(wargs[3], XmNmaximum, 1000);

  XtSetArg(wargs[4], XmNx, SQUARE_X);
  XtSetArg(wargs[5], XmNy, FORE_RED_Y);
  XtSetArg(wargs[6], XmNshowValue, True);

  XtSetArg(wargs[7], XmNdecimalPoints, 3);
  XtSetArg(wargs[8], XmNvalue, square_fore->red);
  XtSetArg(wargs[9], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(wargs[10], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(wargs[11], XmNfontList, m_font);

  square_fore_red = XtCreateManagedWidget("square_fore_red", 
					  xmScaleWidgetClass,
					  square_form, wargs, 12);
  XtAddCallback(square_fore_red, XmNdragCallback, 
		square_fore_red_moved, NULL);
  XtAddCallback(square_fore_red, XmNvalueChangedCallback, 
		square_fore_red_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_GREEN_Y);
  XtSetArg(wargs[8], XmNvalue, square_fore->green);
  square_fore_green = XtCreateManagedWidget("square_fore_green", 
					    xmScaleWidgetClass,
					    square_form, wargs, 12);
  XtAddCallback(square_fore_green, XmNdragCallback, 
		square_fore_green_moved, NULL);
  XtAddCallback(square_fore_green, XmNvalueChangedCallback, 
		square_fore_green_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_BLUE_Y);
  XtSetArg(wargs[8], XmNvalue, square_fore->blue);
  square_fore_blue = XtCreateManagedWidget("square_fore_blue", 
					   xmScaleWidgetClass,
					   square_form, wargs, 12);
  XtAddCallback(square_fore_blue, XmNdragCallback, 
		square_fore_blue_moved, NULL);
  XtAddCallback(square_fore_blue, XmNvalueChangedCallback, 
		square_fore_blue_moved, NULL);

  XtSetArg(wargs[5], XmNy, BACK_Y);
  XtSetArg(wargs[8], XmNvalue, square_back_val);
  square_back = XtCreateManagedWidget("square_back", 
				      xmScaleWidgetClass,
				      square_form, wargs, 12);
  XtAddCallback(square_back, XmNdragCallback, 
		square_back_moved, NULL);
  XtAddCallback(square_back, XmNvalueChangedCallback, 
		square_back_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 250);
  XtSetArg(wargs[5], XmNy, SCALE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 2);
  XtSetArg(wargs[8], XmNvalue, square_scale_val);
  square_scale = XtCreateManagedWidget("square_scale", 
				       xmScaleWidgetClass,
				       square_form, wargs, 12);
  XtAddCallback(square_scale, XmNdragCallback, 
		square_scale_moved, NULL);
  XtAddCallback(square_scale, XmNvalueChangedCallback, 
		square_scale_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 360);
  XtSetArg(wargs[5], XmNy, ROTATE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 0);
  XtSetArg(wargs[8], XmNvalue, square_rotate_val);
  square_rotate = XtCreateManagedWidget("square_rotate", 
					xmScaleWidgetClass,
					square_form, wargs, 12);
  XtAddCallback(square_rotate, XmNdragCallback, 
		square_rotate_moved, NULL);
  XtAddCallback(square_rotate, XmNvalueChangedCallback, 
		square_rotate_moved, NULL);
}

void init_circle_sliders(bb)
Widget bb;
{
  Arg wargs[12];
  int n;
  Widget circle_form;

  XtSetArg(wargs[0], XmNx, CIRCLE_X);
  XtSetArg(wargs[1], XmNy, FORM_Y);
  XtSetArg(wargs[2], XmNwidth, SLIDER_WIDTH);
  XtSetArg(wargs[3], XmNheight, 200);
  circle_form = XtCreateManagedWidget("circle_form", xmFormWidgetClass,
				      bb, wargs, 4);

  XtSetArg(wargs[0], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[1], XmNprocessingDirection, XmMAX_ON_RIGHT);
  XtSetArg(wargs[2], XmNminimum, 0);
  XtSetArg(wargs[3], XmNmaximum, 1000);

  XtSetArg(wargs[4], XmNx, CIRCLE_X);
  XtSetArg(wargs[5], XmNy, FORE_RED_Y);
  XtSetArg(wargs[6], XmNshowValue, True);

  XtSetArg(wargs[7], XmNdecimalPoints, 3);
  XtSetArg(wargs[8], XmNvalue, circle_fore->red);
  XtSetArg(wargs[9], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(wargs[10], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(wargs[11], XmNfontList, m_font);

  circle_fore_red = XtCreateManagedWidget("circle_fore_red", 
					  xmScaleWidgetClass,
					  circle_form, wargs, 12);
  XtAddCallback(circle_fore_red, XmNdragCallback, 
		circle_fore_red_moved, NULL);
  XtAddCallback(circle_fore_red, XmNvalueChangedCallback, 
		circle_fore_red_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_GREEN_Y);
  XtSetArg(wargs[8], XmNvalue, circle_fore->green);
  circle_fore_green = XtCreateManagedWidget("circle_fore_green", 
					    xmScaleWidgetClass,
					    circle_form, wargs, 12);
  XtAddCallback(circle_fore_green, XmNdragCallback, 
		circle_fore_green_moved, NULL);
  XtAddCallback(circle_fore_green, XmNvalueChangedCallback, 
		circle_fore_green_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_BLUE_Y);
  XtSetArg(wargs[8], XmNvalue, circle_fore->blue);
  circle_fore_blue = XtCreateManagedWidget("circle_fore_blue", 
					   xmScaleWidgetClass,
					   circle_form, wargs, 12);
  XtAddCallback(circle_fore_blue, XmNdragCallback, 
		circle_fore_blue_moved, NULL);
  XtAddCallback(circle_fore_blue, XmNvalueChangedCallback, 
		circle_fore_blue_moved, NULL);

  XtSetArg(wargs[5], XmNy, BACK_Y);
  XtSetArg(wargs[8], XmNvalue, circle_back_val);
  circle_back = XtCreateManagedWidget("circle_back", 
				      xmScaleWidgetClass,
				      circle_form, wargs, 12);
  XtAddCallback(circle_back, XmNdragCallback, 
		circle_back_moved, NULL);
  XtAddCallback(circle_back, XmNvalueChangedCallback, 
		circle_back_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 250);
  XtSetArg(wargs[5], XmNy, SCALE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 2);
  XtSetArg(wargs[8], XmNvalue, circle_scale_val);
  circle_scale = XtCreateManagedWidget("circle_scale", 
				       xmScaleWidgetClass,
				       circle_form, wargs, 12);
  XtAddCallback(circle_scale, XmNdragCallback, 
		circle_scale_moved, NULL);
  XtAddCallback(circle_scale, XmNvalueChangedCallback, 
		circle_scale_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 360);
  XtSetArg(wargs[5], XmNy, ROTATE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 0);
  XtSetArg(wargs[8], XmNvalue, circle_rotate_val);
  circle_rotate = XtCreateManagedWidget("circle_rotate", 
					xmScaleWidgetClass,
					circle_form, wargs, 12);
  XtAddCallback(circle_rotate, XmNdragCallback, 
		circle_rotate_moved, NULL);
  XtAddCallback(circle_rotate, XmNvalueChangedCallback, 
		circle_rotate_moved, NULL);
}

void init_text_sliders(bb)
Widget bb;
{
  Arg wargs[12];
  int n;
  Widget text_form;

  XtSetArg(wargs[0], XmNx, TEXT_X);
  XtSetArg(wargs[1], XmNy, FORM_Y);
  XtSetArg(wargs[2], XmNwidth, SLIDER_WIDTH);
  XtSetArg(wargs[3], XmNheight, 200);
  text_form = XtCreateManagedWidget("text_form", xmFormWidgetClass,
				      bb, wargs, 4);

  XtSetArg(wargs[0], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[1], XmNprocessingDirection, XmMAX_ON_RIGHT);
  XtSetArg(wargs[2], XmNminimum, 0);
  XtSetArg(wargs[3], XmNmaximum, 1000);

  XtSetArg(wargs[4], XmNx, TEXT_X);
  XtSetArg(wargs[5], XmNy, FORE_RED_Y);
  XtSetArg(wargs[6], XmNshowValue, True);

  XtSetArg(wargs[7], XmNdecimalPoints, 3);
  XtSetArg(wargs[8], XmNvalue, text_fore->red);
  XtSetArg(wargs[9], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(wargs[10], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(wargs[11], XmNfontList, m_font);

  text_fore_red = XtCreateManagedWidget("text_fore_red", 
					  xmScaleWidgetClass,
					  text_form, wargs, 12);
  XtAddCallback(text_fore_red, XmNdragCallback, 
		text_fore_red_moved, NULL);
  XtAddCallback(text_fore_red, XmNvalueChangedCallback, 
		text_fore_red_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_GREEN_Y);
  XtSetArg(wargs[8], XmNvalue, text_fore->green);
  text_fore_green = XtCreateManagedWidget("text_fore_green", 
					    xmScaleWidgetClass,
					    text_form, wargs, 12);
  XtAddCallback(text_fore_green, XmNdragCallback, 
		text_fore_green_moved, NULL);
  XtAddCallback(text_fore_green, XmNvalueChangedCallback, 
		text_fore_green_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_BLUE_Y);
  XtSetArg(wargs[8], XmNvalue, text_fore->blue);
  text_fore_blue = XtCreateManagedWidget("text_fore_blue", 
					   xmScaleWidgetClass,
					   text_form, wargs, 12);
  XtAddCallback(text_fore_blue, XmNdragCallback, 
		text_fore_blue_moved, NULL);
  XtAddCallback(text_fore_blue, XmNvalueChangedCallback, 
		text_fore_blue_moved, NULL);

  XtSetArg(wargs[5], XmNy, BACK_Y);
  XtSetArg(wargs[8], XmNvalue, text_back_val);
  text_back = XtCreateManagedWidget("text_back", 
				      xmScaleWidgetClass,
				      text_form, wargs, 12);
  XtAddCallback(text_back, XmNdragCallback, 
		text_back_moved, NULL);
  XtAddCallback(text_back, XmNvalueChangedCallback, 
		text_back_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 250);
  XtSetArg(wargs[5], XmNy, SCALE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 2);
  XtSetArg(wargs[8], XmNvalue, text_scale_val);
  text_scale = XtCreateManagedWidget("text_scale", 
				       xmScaleWidgetClass,
				       text_form, wargs, 12);
  XtAddCallback(text_scale, XmNdragCallback, 
		text_scale_moved, NULL);
  XtAddCallback(text_scale, XmNvalueChangedCallback, 
		text_scale_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 360);
  XtSetArg(wargs[5], XmNy, ROTATE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 0);
  XtSetArg(wargs[8], XmNvalue, text_rotate_val);
  text_rotate = XtCreateManagedWidget("text_rotate", 
					xmScaleWidgetClass,
					text_form, wargs, 12);
  XtAddCallback(text_rotate, XmNdragCallback, 
		text_rotate_moved, NULL);
  XtAddCallback(text_rotate, XmNvalueChangedCallback, 
		text_rotate_moved, NULL);
}

void init_image_sliders(bb)
Widget bb;
{
  Arg wargs[12];
  int n;
  Widget image_form;

  XtSetArg(wargs[0], XmNx, IMAGE_X);
  XtSetArg(wargs[1], XmNy, FORM_Y);
  XtSetArg(wargs[2], XmNwidth, SLIDER_WIDTH);
  XtSetArg(wargs[3], XmNheight, 200);
  image_form = XtCreateManagedWidget("image_form", xmFormWidgetClass,
				      bb, wargs, 4);

  XtSetArg(wargs[0], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[1], XmNprocessingDirection, XmMAX_ON_RIGHT);
  XtSetArg(wargs[2], XmNminimum, 0);
  XtSetArg(wargs[3], XmNmaximum, 1000);

  XtSetArg(wargs[4], XmNx, IMAGE_X);
  XtSetArg(wargs[5], XmNy, FORE_RED_Y);
  XtSetArg(wargs[6], XmNshowValue, True);

  XtSetArg(wargs[7], XmNdecimalPoints, 3);
  XtSetArg(wargs[8], XmNvalue, image_fore->red);
  XtSetArg(wargs[9], XmNleftAttachment, XmATTACH_FORM);
  XtSetArg(wargs[10], XmNrightAttachment, XmATTACH_FORM);
  XtSetArg(wargs[11], XmNfontList, m_font);

  image_fore_red = XtCreateManagedWidget("image_fore_red", 
					  xmScaleWidgetClass,
					  image_form, wargs, 12);
  XtAddCallback(image_fore_red, XmNdragCallback, 
		image_fore_red_moved, NULL);
  XtAddCallback(image_fore_red, XmNvalueChangedCallback, 
		image_fore_red_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_GREEN_Y);
  XtSetArg(wargs[8], XmNvalue, image_fore->green);
  image_fore_green = XtCreateManagedWidget("image_fore_green", 
					    xmScaleWidgetClass,
					    image_form, wargs, 12);
  XtAddCallback(image_fore_green, XmNdragCallback, 
		image_fore_green_moved, NULL);
  XtAddCallback(image_fore_green, XmNvalueChangedCallback, 
		image_fore_green_moved, NULL);

  XtSetArg(wargs[5], XmNy, FORE_BLUE_Y);
  XtSetArg(wargs[8], XmNvalue, image_fore->blue);
  image_fore_blue = XtCreateManagedWidget("image_fore_blue", 
					   xmScaleWidgetClass,
					   image_form, wargs, 12);
  XtAddCallback(image_fore_blue, XmNdragCallback, 
		image_fore_blue_moved, NULL);
  XtAddCallback(image_fore_blue, XmNvalueChangedCallback, 
		image_fore_blue_moved, NULL);

  XtSetArg(wargs[5], XmNy, BACK_Y);
  XtSetArg(wargs[8], XmNvalue, image_back_val);
  image_back = XtCreateManagedWidget("image_back", 
				      xmScaleWidgetClass,
				      image_form, wargs, 12);
  XtAddCallback(image_back, XmNdragCallback, 
		image_back_moved, NULL);
  XtAddCallback(image_back, XmNvalueChangedCallback, 
		image_back_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, IMAGE_SCALE_MAX);
  XtSetArg(wargs[5], XmNy, SCALE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 2);
  XtSetArg(wargs[8], XmNvalue, image_scale_val);
  image_scale = XtCreateManagedWidget("image_scale", 
				       xmScaleWidgetClass,
				       image_form, wargs, 12);
  XtAddCallback(image_scale, XmNdragCallback, 
		image_scale_moved, NULL);
  XtAddCallback(image_scale, XmNvalueChangedCallback, 
		image_scale_moved, NULL);

  XtSetArg(wargs[3], XmNmaximum, 360);
  XtSetArg(wargs[5], XmNy, ROTATE_Y);
  XtSetArg(wargs[7], XmNdecimalPoints, 0);
  XtSetArg(wargs[8], XmNvalue, image_rotate_val);
  image_rotate = XtCreateManagedWidget("image_rotate", 
					xmScaleWidgetClass,
					image_form, wargs, 12);
  XtAddCallback(image_rotate, XmNdragCallback, 
		image_rotate_moved, NULL);
  XtAddCallback(image_rotate, XmNvalueChangedCallback, 
		image_rotate_moved, NULL);
}

void set_up_labels(bb)
     Widget bb;
{
  Arg wargs[10];
  Arg getarg[1];
  XFontStruct *new_font;
  short int strLen, fgLen;
  Widget mylabel;
  Widget fglabel;

  new_font = XLoadQueryFont(dpy, appData.font);
  if(new_font == NULL) {
    new_font = XLoadQueryFont(dpy, "fixed");
    if(new_font == NULL) {
      fprintf(stderr, "No fonts avaliable\n");
      exit(1);
    }
  }

  m_font = XmFontListCreate(new_font, XmSTRING_DEFAULT_CHARSET);

  if(m_font == NULL) {
    fprintf(stderr, "Couldn't create motif font list\n");
    exit(1);
  }

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, 0);
  XtSetArg(wargs[2], XmNfontList, m_font);
  mylabel = XtCreateManagedWidget("BoxView", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(SQUARE_X + 100 - strLen/2, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  mylabel = XtCreateManagedWidget("BezierCurve", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(CIRCLE_X + 100 - strLen/2, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  mylabel = XtCreateManagedWidget("TextView", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(TEXT_X + 100 - strLen/2, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  mylabel = XtCreateManagedWidget("ColorImage", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(IMAGE_X + 100 - strLen/2, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, FORE_GREEN_Y+FORM_Y+5);
  fglabel = XtCreateManagedWidget("Foreground\nColor", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &fgLen);
  XtGetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, FORE_RED_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("R", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(LABELS_RIGHT - strLen, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, max(LABELS_RIGHT - strLen, 0));
  XtSetArg(wargs[1], XmNy, FORE_GREEN_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("G", xmLabelWidgetClass,
			bb, wargs, 3);

  XtSetArg(wargs[1], XmNy, FORE_BLUE_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("B", xmLabelWidgetClass,
			bb, wargs, 3);

  XtSetArg(getarg[0], XmNx, max((LABELS_RIGHT - strLen)/2 - fgLen/2, 0));
  XtSetValues(fglabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, BACK_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("Background", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(LABELS_RIGHT - strLen, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, SCALE_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("Size", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(LABELS_RIGHT - strLen, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, ROTATE_Y+FORM_Y+12);
  mylabel = XtCreateManagedWidget("Angle", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(LABELS_RIGHT - strLen, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, DRAW_Y+205);
  mylabel = XtCreateManagedWidget("Color Filters", xmLabelWidgetClass,
			bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, max(IMAGE_X + 100 - strLen/2, 0));
  XtSetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, 0);
  XtSetArg(wargs[1], XmNy, SEP_BOTTOM_Y + FORM_Y + 45);
  mylabel = XtCreateManagedWidget("Photo Courtesy of discimagery", 
				  xmLabelWidgetClass, bb, wargs, 3);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);
  XtSetArg(getarg[0], XmNx, min(IMAGE_X + DRAW_WIDTH/2 - strLen/2,
				IMAGE_X + DRAW_WIDTH - strLen));
  XtSetValues(mylabel, getarg, 1);
}

void set_up_separators(bb)
     Widget bb;
{
  Arg wargs[10];

  XtSetArg(wargs[0], XmNx, SQUARE_X-5);
  XtSetArg(wargs[1], XmNy, DRAW_Y-10);
  XtSetArg(wargs[2], XmNorientation, XmVERTICAL);
  XtSetArg(wargs[3], XmNheight, SEP_BOTTOM_Y + FORM_Y - DRAW_Y + 47);
  XtCreateManagedWidget("squaresep1", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, SQUARE_X+205);
  XtCreateManagedWidget("squaresep2", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, SQUARE_X-5);
  XtSetArg(wargs[2], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[3], XmNwidth, 75);
  XtCreateManagedWidget("squaresep3", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, SQUARE_X+130);
  XtCreateManagedWidget("squaresep5", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, SQUARE_X-5);
  XtSetArg(wargs[1], XmNy, SEP_BOTTOM_Y+FORM_Y+37);
  XtSetArg(wargs[3], XmNwidth, 210);
  XtCreateManagedWidget("squaresep4", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, CIRCLE_X-5);
  XtSetArg(wargs[1], XmNy, DRAW_Y-10);
  XtSetArg(wargs[2], XmNorientation, XmVERTICAL);
  XtSetArg(wargs[3], XmNheight, SEP_BOTTOM_Y + FORM_Y - DRAW_Y + 47);
  XtCreateManagedWidget("circlesep1", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, CIRCLE_X+205);
  XtCreateManagedWidget("circlesep2", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, CIRCLE_X-5);
  XtSetArg(wargs[2], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[3], XmNwidth, 62);
  XtCreateManagedWidget("circlesep3", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, CIRCLE_X+143);
  XtCreateManagedWidget("circlesep5", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, CIRCLE_X-5);
  XtSetArg(wargs[1], XmNy, SEP_BOTTOM_Y+FORM_Y+37);
  XtSetArg(wargs[3], XmNwidth, 210);
  XtCreateManagedWidget("circlesep4", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, TEXT_X-5);
  XtSetArg(wargs[1], XmNy, DRAW_Y-10);
  XtSetArg(wargs[2], XmNorientation, XmVERTICAL);
  XtSetArg(wargs[3], XmNheight, SEP_BOTTOM_Y + FORM_Y - DRAW_Y + 47);
  XtCreateManagedWidget("textsep1", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, TEXT_X+205);
  XtCreateManagedWidget("textsep2", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, TEXT_X-5);
  XtSetArg(wargs[2], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[3], XmNwidth, 73);
  XtCreateManagedWidget("textsep3", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, TEXT_X+132);
  XtCreateManagedWidget("textsep5", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, TEXT_X-5);
  XtSetArg(wargs[1], XmNy, SEP_BOTTOM_Y+FORM_Y+37);
  XtSetArg(wargs[3], XmNwidth, 210);
  XtCreateManagedWidget("textsep4", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, IMAGE_X-5);
  XtSetArg(wargs[1], XmNy, DRAW_Y-10);
  XtSetArg(wargs[2], XmNorientation, XmVERTICAL);
  XtSetArg(wargs[3], XmNheight, SEP_BOTTOM_Y + FORM_Y - DRAW_Y + 47);
  XtCreateManagedWidget("imagesep1", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, IMAGE_X+205);
  XtCreateManagedWidget("imagesep2", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, IMAGE_X-5);
  XtSetArg(wargs[2], XmNorientation, XmHORIZONTAL);
  XtSetArg(wargs[3], XmNwidth, 67);
  XtCreateManagedWidget("imagesep3", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, IMAGE_X+138);
  XtCreateManagedWidget("imagesep5", xmSeparatorWidgetClass,
			bb, wargs, 4);

  XtSetArg(wargs[0], XmNx, IMAGE_X-5);
  XtSetArg(wargs[1], XmNy, SEP_BOTTOM_Y+FORM_Y+37);
  XtSetArg(wargs[3], XmNwidth, 210);
  XtCreateManagedWidget("imagesep4", xmSeparatorWidgetClass,
			bb, wargs, 4);

}


void set_up_toggle_button(bb)
     Widget bb;
{
  Arg wargs[5];
  Arg getarg[1];
  short int strLen;
  Widget mylabel;
  Widget togglebutton;


  XtSetArg(wargs[0], XmNx, TOGGLE_X);
  XtSetArg(wargs[1], XmNy, TOGGLE_Y);
  mylabel = XtCreateManagedWidget("Buffering\nON/OFF:", xmLabelWidgetClass,
				  bb, wargs, 2);
  XtSetArg(getarg[0], XmNwidth, &strLen);
  XtGetValues(mylabel, getarg, 1);

  XtSetArg(wargs[0], XmNx, TOGGLE_X+strLen+5);
  XtSetArg(wargs[1], XmNy, TOGGLE_Y+14);
  XtSetArg(wargs[2], XmNset, True);
  togglebutton = XtCreateManagedWidget(" ", xmToggleButtonWidgetClass,
				       bb, wargs, 3);
  XtAddCallback(togglebutton, XmNvalueChangedCallback, on_off, NULL);
}
