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
#include "X11/StringDefs.h"

char fallbackImage[] = " \
	/doimage {gsave -49 -32.5 translate \
	0 setgray 0 0 98 65 rectfill \
	/Helvetica 18 selectfont \
	1 1 0 setrgbcolor 10 40 moveto (Image ) show \
	1 0 1 setrgbcolor (not) show \
	0 1 1 setrgbcolor 10 20 moveto (installed) show \
	grestore} bind def \
";

/*	===== BEGIN	PRIVATE VARIABLES =====	*/
static XtResource resources[] = {
  {"imageFile", "ImageFile", XtRString, sizeof(char *),
   XtOffset(AppData *, imageFileName), XtRString, "flowers90.ps"},
  {"labelFont", XtCFont, XtRString, sizeof(char *),
   XtOffset(AppData *, font), XtRString, XtDefaultFont},
  {"fallbackImage", "FallbackImage", XtRString, sizeof(char *),
   XtOffset(AppData *, fallbackImage), XtRString, fallbackImage}
  };

float circle_x1, circle_x2;
float x, y, width, height;
DPSContext ctxt;
DPSContext txtCtxt;
int screen;
boolean debug = { FALSE };
GC gc;
long int gc_ID;
boolean context_init = FALSE;
/*	===== END	PRIVATE VARIABLES =====	*/

/*	===== BEGIN	PUBLIC VARIABLES =====	*/
AppData appData;
boolean nobuf = { FALSE };
Display *dpy;
RGBColor square_fore, circle_fore, text_fore, image_fore;

int image_back_val, text_back_val, circle_back_val, square_back_val;

int square_rotate_val, square_scale_val, circle_scale_val,
           circle_rotate_val, text_scale_val, text_rotate_val,
	   image_scale_val, image_rotate_val;

Widget toplevel, square, circle, text, image;

Widget square_fore_red, square_fore_green, square_fore_blue,
	      square_back, square_scale, square_rotate,
              circle_fore_red, circle_fore_green, circle_fore_blue,
	      circle_back, circle_scale, circle_rotate,
              text_fore_red, text_fore_green, text_fore_blue,
	      text_back, text_scale, text_rotate,
              image_fore_red, image_fore_green, image_fore_blue,
	      image_back, image_scale, image_rotate;

Window square_win, circle_win, text_win, image_win;

Pixmap square_pix, circle_pix, text_pix, image_pix;
/*	===== END	PUBLIC VARIABLES =====	*/



void main(argc, argv)
     int argc;
     char *argv[];
{
  Arg wargs[10];
  int n;
  Widget bb;
  char *fileName;

  toplevel = XtInitialize(argv[0], "Wonderland", NULL, 0, &argc, argv);

  if(argc == 2) {
    if(strcmp(argv[1], "-debug") == 0) {
      debug = TRUE;
    }
    else {
      printf("Usage: wonderland [-display xx:0] [-sync] [-debug]\n");
      exit(1);
    }
  }

  XtGetApplicationResources(
    toplevel, &appData, resources, XtNumber(resources), NULL, 0);

  dpy = XtDisplay(toplevel);
  screen = DefaultScreen(dpy);

  gc = XCreateGC(dpy, RootWindow(dpy, screen), 0, NULL);
  gc_ID = XGContextFromGC(gc);

  n = 0;
  XtSetArg(wargs[n], XmNwidth, BASE_WIDTH); n++;
  XtSetArg(wargs[n], XmNheight, BASE_HEIGHT); n++;
  bb = XtCreateManagedWidget("bb", xmBulletinBoardWidgetClass,
			     toplevel, wargs, n);

  set_up_colors();
  set_up_drawing_wins(bb);
  set_up_separators(bb);
  set_up_labels(bb);
  set_up_toggle_button(bb);
  set_up_slider_panels(bb);

  XtRealizeWidget(toplevel);

  XtMainLoop();
}

void init_context(win)
Window win;
{
  unsigned int size;
  FILE *image_file;
  char *imagebuf;
  char *filename;

  ctxt = XDPSCreateSimpleContext(dpy, win, gc, 0, DRAW_HEIGHT, 
				 DPSDefaultTextBackstop, DPSDefaultErrorProc,
				 NULL);
  if(ctxt == NULL) {
    fprintf(stderr, "Error attempting to create DPS context");
    exit(1);
  }
  
  DPSSetContext(ctxt);
  
  if(debug) {
    txtCtxt = DPSCreateTextContext(DPSDefaultTextBackstop, 
				   DPSDefaultErrorProc);
    DPSChainContext(ctxt, txtCtxt);
  }

  image_file = fopen(appData.imageFileName, "r");
  if(image_file == NULL) {
      filename = XtResolvePathname(XtDisplay(toplevel), "images", NULL, ".ps",
				   NULL, (Substitution) NULL, 0,
				   (XtFilePredicate) NULL);
      if (filename == NULL) {
	  filename = XtResolvePathname(XtDisplay(toplevel), NULL,
				       appData.imageFileName, NULL,
				       NULL, (Substitution) NULL, 0,
				       (XtFilePredicate) NULL);
      }
      if (filename != NULL) {
	  image_file = fopen(filename, "r");
	  XtFree(filename);
      }
  }

  if (image_file != NULL) {
      imagebuf = (char *) XtMalloc(8000);

      while(feof(image_file) == 0) {
	  size = fread(imagebuf, 1, 8000, image_file);
	  DPSWritePostScript(ctxt, imagebuf, size);
      }
      fclose(image_file);
      XtFree((XtPointer) imagebuf);
  } else {
      DPSWritePostScript(ctxt, appData.fallbackImage, strlen(fallbackImage));
  }

  PSitransform( (float) DRAW_WIDTH,
	       (float) -DRAW_HEIGHT,
	       &width, &height);
  PSWsetWH(width, height, (float)width/2, (float)height/2);
}  


void init_square(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmDrawingAreaCallbackStruct *call_data;
{
  static boolean first = TRUE;

  if(first == TRUE) {
    first = FALSE;
    square_win = XtWindow(square);
    XSetWindowBackground(dpy, square_win, WhitePixel(dpy, screen));
    if(context_init == TRUE) {
      square_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
				 DRAW_WIDTH, DRAW_HEIGHT, 
				 DefaultDepth(dpy, screen));
      PSWsetSquare((float)square_fore->red / 1000, 
		   (float)square_fore->green / 1000,
		   (float)square_fore->blue / 1000,
		   (float)square_back_val / 1000,
		   (float)square_scale_val / 100,
		   (float)square_rotate_val,
		   gc_ID, square_pix, (long int)0, (long int)DRAW_HEIGHT);
    }
  }

  if(context_init == FALSE) {
    context_init = TRUE;
    init_context(square_win);
    square_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
			       DRAW_WIDTH, DRAW_HEIGHT, 
			       DefaultDepth(dpy, screen));
    PSWsetSquare((float)square_fore->red / 1000, 
		 (float)square_fore->green / 1000,
		 (float)square_fore->blue / 1000,
		 (float)square_back_val / 1000,
		 (float)square_scale_val / 100,
		 (float)square_rotate_val,
		 gc_ID, square_pix, (long int)0, (long int)DRAW_HEIGHT);
  }

  PSWDrawSquare();
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void init_circle(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmDrawingAreaCallbackStruct *call_data;
{

  static boolean first = TRUE;

  if(first == TRUE) {
    circle_x1 = ((float)circle_rotate_val /360.0) * (-100.0) + 90.0;
    circle_x2 = ((float)circle_rotate_val /360.0) * 130.0 - 100.0;

    first = FALSE;
    circle_win = XtWindow(circle);
    XSetWindowBackground(dpy, circle_win, WhitePixel(dpy, screen));
    if(context_init == TRUE) {
      circle_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
				 DRAW_WIDTH, DRAW_HEIGHT, 
				 DefaultDepth(dpy, screen));
      PSWsetCircle((float)circle_fore->red / 1000, 
		   (float)circle_fore->green / 1000,
		   (float)circle_fore->blue / 1000,
		   (float)circle_back_val / 1000,
		   (float)circle_scale_val / 100,
		   (float)circle_rotate_val,
		   circle_x1, circle_x2,
		   gc_ID, circle_pix, (long int)0, (long int)DRAW_HEIGHT);
    }
  }

  if(context_init == FALSE) {
    context_init = TRUE;
    init_context(circle_win);
    circle_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
			       DRAW_WIDTH, DRAW_HEIGHT, 
			       DefaultDepth(dpy, screen));
    PSWsetCircle((float)circle_fore->red / 1000, 
		 (float)circle_fore->green / 1000,
		 (float)circle_fore->blue / 1000,
		 (float)circle_back_val / 1000,
		 (float)circle_scale_val / 100,
		 (float)circle_rotate_val,
		 circle_x1, circle_x2,
		 gc_ID, circle_pix, (long int)0, (long int)DRAW_HEIGHT);
  }

  PSWDrawCircle();
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void init_text(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmDrawingAreaCallbackStruct *call_data;
{
  static boolean first = TRUE;

  if(first == TRUE) {
    first = FALSE;
    text_win = XtWindow(text);
    XSetWindowBackground(dpy, text_win, WhitePixel(dpy, screen));
    if(context_init == TRUE) {
      text_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
			       DRAW_WIDTH, DRAW_HEIGHT, 
			       DefaultDepth(dpy, screen));
      PSWsetText((float)text_fore->red / 1000, 
		 (float)text_fore->green / 1000,
		 (float)text_fore->blue / 1000,
		 (float)text_back_val / 1000,
		 (float)text_scale_val / 100,
		 (float)text_rotate_val,
		 gc_ID, text_pix, (long int)0, (long int)DRAW_HEIGHT);
    }
  }

  if(context_init == FALSE) {
    context_init = TRUE;
    init_context(text_win);
    text_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
			     DRAW_WIDTH, DRAW_HEIGHT, 
			     DefaultDepth(dpy, screen));
    PSWsetText((float)text_fore->red / 1000, 
	       (float)text_fore->green / 1000,
	       (float)text_fore->blue / 1000,
	       (float)text_back_val / 1000,
	       (float)text_scale_val / 100,
	       (float)text_rotate_val,
	       gc_ID, text_pix, (long int)0, (long int)DRAW_HEIGHT);
  }

  PSWDrawText();
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void init_image(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmDrawingAreaCallbackStruct *call_data;
{
  static boolean first = TRUE;

  if(first == TRUE) {
    first = FALSE;
    image_win = XtWindow(image);
    XSetWindowBackground(dpy, image_win, WhitePixel(dpy, screen));
    if(context_init == TRUE) {
      image_pix = XCreatePixmap(dpy, RootWindow(dpy, screen), 
				DRAW_WIDTH, DRAW_HEIGHT, 
				DefaultDepth(dpy, screen));
      PSWsetImage((float)image_fore->red / 1000, 
		  (float)image_fore->green / 1000,
		  (float)image_fore->blue / 1000,
		  (float)image_back_val / 1000,
		  (float)image_scale_val / 100,
		  (float)image_rotate_val,
		  gc_ID, image_pix, (long int)0, (long int)DRAW_HEIGHT);
    }
  }

  if(context_init == FALSE) {
    context_init = TRUE;
    init_context(image_win);
    image_pix = XCreatePixmap(dpy, RootWindow(dpy, screen),
			      DRAW_WIDTH, DRAW_HEIGHT, 
			      DefaultDepth(dpy, screen));
    PSWsetImage((float)image_fore->red / 1000, 
		(float)image_fore->green / 1000,
		(float)image_fore->blue / 1000,
		(float)image_back_val / 1000,
		(float)image_scale_val / 100,
		(float)image_rotate_val,
		gc_ID, image_pix, (long int)0, (long int)DRAW_HEIGHT);
  }

  PSWDrawImage();
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}


/*
 *  Callbacks
 */

void square_fore_red_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWsquare_fore_red((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void square_fore_green_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWsquare_fore_green((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void square_fore_blue_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWsquare_fore_blue((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void square_back_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWsquare_back((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void square_scale_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  square_scale_val = call_data->value;
  PSWsquare_distort((float)call_data->value / 100,
		    (float)square_rotate_val);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void square_rotate_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  square_rotate_val = call_data->value;
  PSWsquare_distort((float)square_scale_val / 100,
		    (float)call_data->value);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, square_pix, square_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void circle_fore_red_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWcircle_fore_red((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void circle_fore_green_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWcircle_fore_green((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void circle_fore_blue_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWcircle_fore_blue((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void circle_back_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWcircle_back((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void circle_scale_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  if(call_data->value == 0) {
    call_data->value = 1;
  }
  circle_scale_val = call_data->value;
  PSWcircle_distort((float)call_data->value / 100,
		    (float)circle_rotate_val,
		    circle_x1, circle_x2);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void circle_rotate_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  circle_rotate_val = call_data->value;
  circle_x1 = ((float)circle_rotate_val /360.0) * (-100.0) + 90.0;
  circle_x2 = ((float)circle_rotate_val /360.0) * 130.0 - 100.0;
  PSWcircle_distort((float)circle_scale_val / 100,
		    (float)call_data->value,
		    circle_x1, circle_x2);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, circle_pix, circle_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void text_fore_red_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWtext_fore_red((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void text_fore_green_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWtext_fore_green((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void text_fore_blue_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWtext_fore_blue((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void text_back_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWtext_back((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void text_scale_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  if(call_data->value == 0) {
    call_data->value = 1;
  }
  text_scale_val = call_data->value;
  PSWtext_distort((float)call_data->value / 100,
		  (float)text_rotate_val);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}

void text_rotate_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  text_rotate_val = call_data->value;
  PSWtext_distort((float)text_scale_val / 100,
		  (float)call_data->value);
  DPSWaitContext(ctxt);
  XCopyArea(dpy, text_pix, text_win, gc, 0, 0, 
	    DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
}


void image_fore_red_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWimage_fore_red((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}

void image_fore_green_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWimage_fore_green((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}

void image_fore_blue_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWimage_fore_blue((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}


void image_back_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  PSWimage_back((float)call_data->value / 1000);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}

void image_scale_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  static float adjust;

  if(call_data->value == 0) {
    call_data->value = 1;
  }
  image_scale_val = call_data->value;
  PSWimage_distort((float)call_data->value / 100,
		   (float)image_rotate_val);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}

void image_rotate_moved(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmScaleCallbackStruct *call_data;
{
  image_rotate_val = call_data->value;
  PSWimage_distort((float)image_scale_val / 100,
		   (float)call_data->value);
  DPSWaitContext(ctxt);
  if(nobuf == FALSE) {
    XCopyArea(dpy, image_pix, image_win, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
  else {
    XCopyArea(dpy, image_win, image_pix, gc, 0, 0, 
	      DRAW_WIDTH, DRAW_HEIGHT, 0, 0);
  }
}

void on_off(w, client_data, call_data)
     Widget w;
     caddr_t client_data;
     XmToggleButtonCallbackStruct *call_data;
{
  if(call_data->set == TRUE) {
    nobuf = FALSE;
    PSWsetgcdrawable(gc_ID, (long int)0, (long int)DRAW_HEIGHT, image_pix);
  }
  else {
    nobuf = TRUE;
    PSWsetgcdrawable(gc_ID, (long int)0, (long int)DRAW_HEIGHT, image_win);
  }
}

