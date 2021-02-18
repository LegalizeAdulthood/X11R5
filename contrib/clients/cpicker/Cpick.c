/*****************************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Cpick.c - Color picker widget for X11 Toolkit
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989,
 *		  improvements Mon Jul 24 1989
 *		  fix for R4 Fri Jul 13 1990
 *		  update to R5 Tue May 28 1991
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#include <stdio.h>
#include <math.h>
#include <X11/Xos.h>
#include <X11/cursorfont.h>
#include <X11/IntrinsicP.h>
#include <X11/Xresource.h>
#include <X11/CompositeP.h>
#include <X11/StringDefs.h>
#include <X11/Composite.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/DialogP.h>	/* I'm a bad boy... */
#include "Slide.h"
#include "CpickP.h"
#include "color.h"

#include <X11/Shell.h>

/* #define BUBBLE_MATCH	/* Uses a visible bubble sort when matching colors */

#define NULLSTR ""
#define VDIST 5
#define HDIST 5
#define LDIST 20
#define LWIDTH 50
#define BHDIST 15
#define BVDIST 5
#define BWIDTH 120
#define BHEIGHT 55
#define SHEIGHT 10
#define SWIDTH 100
#define SBACKGD "white"
#define HORIGIN 35
#define CDIST 10
#define HHDIST 15
#define HVDIST (BVDIST-5)
#define NVDIST 10
#define PVDIST 5
#define CVDIST 5
#define MVDIST 5
#define NHEIGHT 77	/* ((int) (1.4*BHEIGHT)) */
#define NWIDTH (BWIDTH+HHDIST+LWIDTH)
#define CCURSOR XC_star

#define DEFAULTWIDTH 400
#define DEFAULTHEIGHT 230

#define INCREMENT 10
#define WIDENEAR 8192
#define NARROWNEAR 768
#define MAXIMUM 65536
#define MINIMUM 0
#define MBASE 256
#define RATIO (MAXIMUM/MBASE)

#define RGBFILE "/usr/lib/X11/rgb.txt"

#define FONT "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-iso8859-1"

#define Offset(field) XtOffset(CpickWidget, field)

static Dimension defNearpixels = 64;

static XtResource resources[] = {
  {XtNokProc, XtCCallback, XtRCallback, sizeof(XtPointer),
     Offset(cpick.okProc), XtRCallback, NULL},
  {XtNselectProc, XtCCallback, XtRCallback, sizeof(XtPointer),
     Offset(cpick.selectProc), XtRCallback, NULL},
  {XtNchangeProc, XtCCallback, XtRCallback, sizeof(XtPointer),
     Offset(cpick.changeProc), XtRCallback, NULL},
  {XtNrestoreProc, XtCCallback, XtRCallback, sizeof(XtPointer),
     Offset(cpick.restoreProc), XtRCallback, NULL},
  {XtNallocated, XtCAllocated, XtRXColor, sizeof(XtPointer),
     Offset(cpick.allocated), XtRXColor, NULL},
  {XtNcmap, XtCCmap, XtRColormap, sizeof(Colormap),
     Offset(cpick.cmap), XtRColormap, NULL},
  {XtNselectLabel, XtCLabel, XtRString, sizeof(String),
     Offset(cpick.selectlabel), XtRString, "select"},
  {XtNcancelLabel, XtCLabel, XtRString, sizeof(String),
     Offset(cpick.cancellabel), XtRString, "cancel"},
  {XtNrestoreLabel, XtCLabel, XtRString, sizeof(String),
     Offset(cpick.restorelabel), XtRString, "restore"},
  {XtNokLabel, XtCLabel, XtRString, sizeof(String),
     Offset(cpick.oklabel), XtRString, "ok"},
  {XtNnearPixels, XtCDimension, XtRDimension, sizeof(Dimension),
     Offset(cpick.nearpixels), XtRDimension, (XtPointer) &defNearpixels},
  {XtNuseColors, XtCUsecolors, XtRBoolean, sizeof(Boolean),
     Offset(cpick.usecolors), XtRBoolean, (XtPointer) TRUE},
};

static void doreturn();
static void ClassInitialize();
static void Initialize();
static void Realize();
static void Resize();
static void Redisplay();
static Boolean SetValues();
static XtGeometryResult GeometryManager();

static int changelabel();
static int doNew();
static int doExpose();
static int changePalette();
static int undomatch();
static int matchPalette();

static void Notify(), Destroy();

static XtActionsRec actionTable[] = {
  {"doreturn", doreturn},
  {NULL, NULL},
};

CpickClassRec cpickClassRec = {
  {
    (WidgetClass) &compositeClassRec,	/* superclass		  */	
    "Cpick",				/* class_name		  */
    sizeof(CpickRec),			/* size			  */
    NULL,				/* class_initialize	  */
    NULL,				/* class_part_initialize  */
    FALSE,				/* class_inited		  */
    Initialize,				/* initialize		  */
    NULL,				/* initialize_hook	  */
    Realize,				/* realize		  */
    actionTable,			/* actions		  */
    XtNumber(actionTable),		/* num_actions		  */
    resources,				/* resources		  */
    XtNumber(resources),		/* resource_count	  */
    NULLQUARK,				/* xrm_class		  */
    TRUE,				/* compress_motion	  */
    TRUE,				/* compress_exposure	  */
    TRUE,				/* compress_enterleave    */
    FALSE,				/* visible_interest	  */
    Destroy,				/* destroy		  */
    Resize,				/* resize		  */
    XtInheritExpose,			/* expose		  */
    SetValues,				/* set_values		  */
    NULL,				/* set_values_hook	  */
    XtInheritSetValuesAlmost,		/* set_values_almost	  */
    NULL,				/* get_values_hook	  */
    NULL,				/* accept_focus		  */
    XtVersion,				/* version		  */
    NULL,				/* callback_private	  */
    NULL,				/* tm_table		  */
    NULL,				/* query_geometry	  */
    XtInheritDisplayAccelerator,	/* display_accelerator	  */
    NULL				/* extension		  */
  },{
/* composite_class fields */
    /* geometry_handler   */    GeometryManager,
    /* change_managed     */    XtInheritChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
    /* extension	   */   NULL
  },{
/* Cpick class fields */
    /* empty		  */	0,
  }
};

static void Destroy(w)
     Widget w;
{

}

WidgetClass cpickWidgetClass = (WidgetClass)&cpickClassRec;

static String argv0 = "CPick";

typedef unsigned long (*PixProc)();

struct vals {
  int ih, iw, x0, y0, rownum;
  PixProc pix;
} gval, nval;

String colors[] = {"red", "green", "blue",
		     "gold", "khaki", "wheat",
		     "cyan", "magenta", "yellow"};

static Arg sargs[] = {
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfillColor, NULL},
  {XtNbackground, (XtArgVal) NULL},
  {XtNheight, (XtArgVal) SHEIGHT},
  {XtNwidth, (XtArgVal) SWIDTH},
  {XtNhorizDistance, HORIGIN},
  {XtNvertDistance, VDIST},
  {XtNorientation, (XtArgVal) XtorientHorizontal},
};

static Arg largs[] = {
  {XtNlabel, (XtArgVal) NULLSTR},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNhorizDistance, HDIST},
  {XtNvertDistance, VDIST},
  {XtNborderWidth, 0},
  {XtNwidth, LWIDTH},
  {XtNjustify, (XtArgVal) XtJustifyLeft},
};

static Arg targs[] = {
  {XtNlabel, (XtArgVal) NULLSTR},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNhorizDistance, LDIST},
  {XtNvertDistance, VDIST},
  {XtNborderWidth, 0},
};

static Arg bargs[] = {
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNhorizDistance, BHDIST},
  {XtNvertDistance, BVDIST},
  {XtNwidth, BWIDTH},
  {XtNheight, BHEIGHT},
  {XtNborderWidth, 2},
};

static Arg hargs[] = {
  {XtNlabel, (XtArgVal) NULLSTR},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNhorizDistance, HHDIST},
  {XtNvertDistance, HVDIST},
  {XtNborderWidth, 1},
  {XtNjustify, (XtArgVal) XtJustifyCenter},
  {XtNwidth, LWIDTH},
};

static Arg cargs[] = {
  {XtNlabel, (XtArgVal) NULLSTR},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNhorizDistance, BHDIST},
  {XtNvertDistance, CVDIST},
  {XtNfont, NULL},
  {XtNborderWidth, 1},
};

static Arg nargs[] = {
  {XtNborderWidth, (XtArgVal) 0},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNheight, NHEIGHT},
  {XtNwidth, NWIDTH},
  {XtNhorizDistance, BHDIST},
  {XtNvertDistance, NVDIST},
};

static Arg mlargs[] = {
  {XtNlabel, (XtArgVal) NULLSTR },
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNlabel, (XtArgVal) NULLSTR},
  {XtNhorizDistance, BHDIST},
  {XtNvertDistance, MVDIST},
  {XtNborderWidth, 1},
  {XtNjustify, (XtArgVal) XtJustifyCenter},
  {XtNwidth, NWIDTH},
};

static Arg dargs[] = {
  {XtNvalue, (XtArgVal) ""},
  {XtNheight, 1},
  {XtNwidth, 1},
  {XtNlabel, (XtArgVal) "RGB hex value?"},
};

static Arg pargs[] = {
  {XtNlabel, (XtArgVal) NULLSTR },
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNlabel, (XtArgVal) " range "},
  {XtNhorizDistance, HHDIST},
  {XtNvertDistance, PVDIST},
  {XtNborderWidth, 1},
  {XtNjustify, (XtArgVal) XtJustifyCenter},
  {XtNwidth, LWIDTH},
};

static Arg p2args[] = {
  {XtNlabel, (XtArgVal) NULLSTR },
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNfont, NULL},
  {XtNlabel, (XtArgVal) " match "},
  {XtNhorizDistance, HHDIST},
  {XtNvertDistance, PVDIST},
  {XtNborderWidth, 1},
  {XtNjustify, (XtArgVal) XtJustifyCenter},
  {XtNwidth, LWIDTH},
};

static Arg plabelargs[] = {
  {XtNlabel, NULL},
};

static Arg labelargs2[] = {
  {XtNlabel, NULL},
  {XtNwidth, LWIDTH},
};

static Arg labelargs1[] = {
  {XtNlabel, NULL},
};

char STRING[] = "rgbhsvcmy";

static Position getP(w, s)
Widget w;
String s;
{
  Position result;
  Arg args[1];
  int v;

  XtSetArg(args[0],s,(XtArgVal) &result);
  XtGetValues(w, args, XtNumber(args));
  v = result;

  return(v);
}

static Dimension getD(w, s)
Widget w;
String s;
{
  Dimension result;
  Arg args[1];
  int v;

  XtSetArg(args[0],s,(XtArgVal) &result);
  XtGetValues(w, args, XtNumber(args));
  v = result;

  return(v);
}

static XtGeometryResult GeometryManager(w, request, reply)
    Widget w;
    XtWidgetGeometry *request;
    XtWidgetGeometry *reply;    /* RETURN */
{
  return XtGeometryYes;
}

static void Realize( gw, valueMask, attributes )
   Widget gw;
   XtValueMask *valueMask;
   XSetWindowAttributes *attributes;
{
  CpickWidget cw = (CpickWidget) gw;

  XtCreateWindow( gw, InputOutput, (Visual *)CopyFromParent,
		 *valueMask, attributes );
  XtRealizeWidget(cw->cpick.tlevel);

  XtMakeResizeRequest(gw, getD(cw->cpick.tlevel, XtNwidth),
		      getD(cw->cpick.tlevel, XtNheight),
		      NULL, NULL);
}

static void Resize( gw )
   Widget gw;
{
  CpickWidget cw = (CpickWidget) gw;

  XClearWindow( XtDisplay(gw), XtWindow(gw) );
  XtResizeWidget(cw->cpick.tlevel, cw->core.width,
		 cw->core.height, (Dimension) 1);
  XClearWindow(XtDisplay(cw->cpick.nlevel), XtWindow(cw->cpick.nlevel) );
  doExpose(cw);
}


static Boolean SetValues( current, request, desired )
   Widget current,		/* what I am */
          request,		/* what he wants me to be */
          desired;		/* what I will become */
{
    CpickWidget sw = (CpickWidget) desired;
    Boolean redraw = TRUE; /* be stupid for now */

    sw->cpick.oldvalue = *(sw->cpick.allocated);
    doNew(sw);
    return(redraw);
}

static updateBox(cw)
CpickWidget cw;
{
  XStoreColor(XtDisplay(cw),
	      cw->cpick.cmap,
	      cw->cpick.allocated);
}

void CenterWidget(parent, child)
Widget parent, child;
{
  Position x, y;

  x = (getD(parent, XtNwidth) - getD(child, XtNwidth)) / 2;
  y = (getD(parent, XtNheight) - getD(child, XtNheight)) / 2;
  if (x < 0) x = 0;
  XtMoveWidget(child, x, y);
}

static char hexDigit(v)
int v;
{
  if (v < 10)
    return '0'+v;
  else
    return 'a'+(v-10);
}

static int unhexChar(ch)
char ch;
{
  if (ch >= '0' && ch <= '9')
    return (ch-'0');
  else if (ch >= 'a' && ch <= 'f')
    return (ch-'a'+10);
  else if (ch >= 'A' && ch <= 'F')
    return (ch-'A'+10);
  else
    return -1;
}

static Boolean hexChars(s)
char *s;
{
  int each;

  for (each=0; each<strlen(s); each++) {
    if (unhexChar(s[each]) == -1)
      return(FALSE);
  }
  return(TRUE);
}

static createHex(r, g, b, s)
int r, g, b;
char s[];
{
  sprintf(s, "#%c%c%c%c%c%c",
	  hexDigit(256*r/MBASE/16),
	  hexDigit(256*r/MBASE % 16),
	  hexDigit(256*g/MBASE/16),
	  hexDigit(256*g/MBASE % 16),
	  hexDigit(256*b/MBASE/16),
	  hexDigit(256*b/MBASE % 16));
}

static changeRGB(cw, which)
CpickWidget cw;
int which;
{
  RGB rgb;
  HSV hsv;
  CMY cmy;
  char str[13];
  static Arg labelargs1[] = {
    {XtNlabel, NULL},
  };

  if (cw->cpick.matched) {
    labelargs1[0].value = (XtArgVal) " ";
    XtSetValues(cw->cpick.mlabel, labelargs1, XtNumber(labelargs1));
  }
  if (which < H) {
    rgb.r = cw->cpick.values[R]*RATIO;
    rgb.g = cw->cpick.values[G]*RATIO;
    rgb.b = cw->cpick.values[B]*RATIO;
    hsv = RGBToHSV(rgb);
    cw->cpick.values[H] = (int) (hsv.h*(MBASE-1));
    cw->cpick.values[S] = (int) (hsv.s*(MBASE-1));
    cw->cpick.values[V] = (int) (hsv.v*(MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[H],cw->cpick.values[H]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[S],cw->cpick.values[S]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[V],cw->cpick.values[V]/(float) (MBASE-1));
    changelabel(cw, H);
    changelabel(cw, S);
    changelabel(cw, V);
    cmy = RGBToCMY(rgb);
    cw->cpick.values[C] = cmy.c*RATIO/MAXIMUM;
    cw->cpick.values[M] = cmy.m*RATIO/MAXIMUM;
    cw->cpick.values[Y] = cmy.y*RATIO/MAXIMUM;
    XtBargraphSetValue(cw->cpick.slides[C],cw->cpick.values[C]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[M],cw->cpick.values[M]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[Y],cw->cpick.values[Y]/(float) (MBASE-1));
    changelabel(cw, C);
    changelabel(cw, M);
    changelabel(cw, Y);
  } else if (which < C) {
    hsv.h = cw->cpick.values[H]/(float) (MBASE-1);
    hsv.s = cw->cpick.values[S]/(float) (MBASE-1);
    hsv.v = cw->cpick.values[V]/(float) (MBASE-1);
    rgb = HSVToRGB(hsv);
    cw->cpick.values[R] = rgb.r*RATIO/MAXIMUM;
    cw->cpick.values[G] = rgb.g*RATIO/MAXIMUM;
    cw->cpick.values[B] = rgb.b*RATIO/MAXIMUM;
    XtBargraphSetValue(cw->cpick.slides[R],cw->cpick.values[R]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[G],cw->cpick.values[G]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[B],cw->cpick.values[B]/(float) (MBASE-1));
    changelabel(cw, R);
    changelabel(cw, G);
    changelabel(cw, B);
    cmy = RGBToCMY(rgb);
    cw->cpick.values[C] = cmy.c*RATIO/MAXIMUM;
    cw->cpick.values[M] = cmy.m*RATIO/MAXIMUM;
    cw->cpick.values[Y] = cmy.y*RATIO/MAXIMUM;
    XtBargraphSetValue(cw->cpick.slides[C],cw->cpick.values[C]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[M],cw->cpick.values[M]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[Y],cw->cpick.values[Y]/(float) (MBASE-1));
    changelabel(cw, C);
    changelabel(cw, M);
    changelabel(cw, Y);
  } else {
    cmy.c = cw->cpick.values[C]*RATIO;
    cmy.m = cw->cpick.values[M]*RATIO;
    cmy.y = cw->cpick.values[Y]*RATIO;
    rgb = CMYToRGB(cmy);
    cw->cpick.values[R] = rgb.r*RATIO/MAXIMUM;
    cw->cpick.values[G] = rgb.g*RATIO/MAXIMUM;
    cw->cpick.values[B] = rgb.b*RATIO/MAXIMUM;
    XtBargraphSetValue(cw->cpick.slides[R],cw->cpick.values[R]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[G],cw->cpick.values[G]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[B],cw->cpick.values[B]/(float) (MBASE-1));
    changelabel(cw, R);
    changelabel(cw, G);
    changelabel(cw, B);
    hsv = RGBToHSV(rgb);
    cw->cpick.values[H] = (int) (hsv.h*(MBASE-1));
    cw->cpick.values[S] = (int) (hsv.s*(MBASE-1));
    cw->cpick.values[V] = (int) (hsv.v*(MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[H],cw->cpick.values[H]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[S],cw->cpick.values[S]/(float) (MBASE-1));
    XtBargraphSetValue(cw->cpick.slides[V],cw->cpick.values[V]/(float) (MBASE-1));
    changelabel(cw, H);
    changelabel(cw, S);
    changelabel(cw, V);
  }
  createHex(cw->cpick.values[R], cw->cpick.values[G], cw->cpick.values[B], str);
  labelargs2[0].value = (XtArgVal) str;
/*
  labelargs2[1].value = (XtArgVal) getD(cw->cpick.box, XtNwidth);
*/
  XtSetValues(cw->cpick.hex, labelargs2, XtNumber(labelargs2));
  XtManageChild(cw->cpick.hex);
  cw->cpick.allocated->red = cw->cpick.values[R]*RATIO;
  cw->cpick.allocated->green = cw->cpick.values[G]*RATIO;
  cw->cpick.allocated->blue = cw->cpick.values[B]*RATIO;
  changePalette(cw, FALSE);
  if (XtIsRealized((Widget) cw) && cw->cpick.changeProc) {
    XtCallCallbacks((Widget) cw, XtNchangeProc,
		    (XtPointer) (cw->cpick.allocated));
  }
}

static update(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     XtPointer position;
{
  int which;
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  which = (int) cdata;
  cw->cpick.values[which] = (int)((* (float *) position)*(MBASE-1));
  changelabel(cw, which);

  changeRGB(cw, which);
  updateBox(cw);
}

static newRGB(cw, r, g, b)
CpickWidget cw;
unsigned short r, g, b;
{
  cw->cpick.values[R] = r/RATIO;
  cw->cpick.values[G] = g/RATIO;
  cw->cpick.values[B] = b/RATIO;
  changelabel(cw, R);
  changelabel(cw, G);
  changelabel(cw, B);
  XtBargraphSetValue(cw->cpick.slides[R], r/(float) MAXIMUM);
  XtBargraphSetValue(cw->cpick.slides[G], g/(float) MAXIMUM);
  XtBargraphSetValue(cw->cpick.slides[B], b/(float) MAXIMUM);
  changeRGB(cw, R);
  updateBox(cw);
}
			      

static scroll(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     XtPointer position;
{
  int which;
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  which = (int) cdata;
  if ((int) position < 0) {
    cw->cpick.values[which] = cw->cpick.values[which]+cw->cpick.inc;
    if (cw->cpick.values[which] >= MBASE)
      cw->cpick.values[which] = MBASE-1;
  } else {
    cw->cpick.values[which] = cw->cpick.values[which]-cw->cpick.inc;
    if (cw->cpick.values[which] < MINIMUM)
      cw->cpick.values[which] = MINIMUM;
  }
  XtBargraphSetValue(cw->cpick.slides[which], cw->cpick.values[which]/(float) (MBASE-1));
  changelabel(cw, which);

  if (cw->cpick.box != NULL) {
    changeRGB(cw, which);
    updateBox(cw);
  }

}

static changelabel(cw, which)
CpickWidget cw;
int which;
{
  char str[10];

  if ((which < H) || (which > V)) {
    sprintf(str, "%-5d", cw->cpick.values[which]);
  } else {
    sprintf(str, "%-5.4f", cw->cpick.values[which]/(float) (MBASE-1));
  }
  labelargs1[0].value = (XtArgVal) str;
  XtSetValues(cw->cpick.labels[which], labelargs1, XtNumber(labelargs1));
}

static createSlides(cw, argv0)
CpickWidget cw;
String argv0;
{
  int each;
  Widget temp;
  XColor xc, ignore;
  XFontStruct *fs;

  fs = XLoadQueryFont(XtDisplay(cw), FONT);
  if (fs == NULL) {
    fs = XLoadQueryFont(XtDisplay(cw), "fixed");
  }
  pargs[3].value = p2args[3].value = mlargs[3].value = hargs[3].value =
    cargs[5].value = targs[3].value = largs[3].value =
      (XtArgVal) fs;

  largs[0].value = (XtArgVal) NULLSTR;
  largs[1].value = NULL;
  cw->cpick.toplabel = XtCreateManagedWidget(argv0, labelWidgetClass, cw->cpick.tlevel,
				   largs, XtNumber(largs));
  sargs[1].value = (XtArgVal) cw->cpick.toplabel;
  largs[1].value = (XtArgVal) cw->cpick.toplabel;
  largs[2].value = (XtArgVal) cw->cpick.toplabel;
  XAllocNamedColor(XtDisplay(cw), cw->cpick.cmap, SBACKGD, &xc, &ignore);
  sargs[3].value = (XtArgVal) xc.pixel;
  targs[0].value = (XtArgVal) malloc(2);
  ((char *) targs[0].value)[1] = '\0';

  for (each=0; each<NUM; each++) {
    if ((each % 3 == 0) && (each != 0)) {
      temp = XtCreateManagedWidget(argv0, labelWidgetClass, cw->cpick.tlevel,
				    largs, XtNumber(largs));
      largs[2].value = (XtArgVal) temp;
      sargs[1].value = (XtArgVal) temp;
    }

    if (cw->cpick.usecolors && ((each < H) || (each > V))) {
      XAllocNamedColor(XtDisplay(cw),
		       cw->cpick.cmap,
		       colors[each], &xc, &ignore);
      XtSetArg(sargs[2], XtNfillColor, xc.pixel);
    } else {
      unsigned int depth = 1;

      XtSetArg(sargs[2], XtNfillPixmap,
	       XmuCreateStippledPixmap(XtScreen(cw),
				       cw->core.border_pixel,
				       cw->core.background_pixel,
				       cw->core.depth));
    }

    cw->cpick.slides[each] = XtCreateManagedWidget(argv0, slideWidgetClass, cw->cpick.tlevel,
					 sargs, XtNumber(sargs));
    XtAddCallback(cw->cpick.slides[each], XtNjumpProc,
		  (XtCallbackProc) update, (XtPointer) each);
    XtAddCallback(cw->cpick.slides[each], XtNscrollProc,
		  (XtCallbackProc) scroll, (XtPointer) each);
    largs[1].value = (XtArgVal) cw->cpick.slides[each];
    cw->cpick.labels[each] = XtCreateManagedWidget(argv0, labelWidgetClass, cw->cpick.tlevel,
					 largs, XtNumber(largs));
    targs[2].value = largs[2].value;
    ((char *) targs[0].value)[0] = STRING[each];
    temp = XtCreateManagedWidget(argv0, labelWidgetClass, cw->cpick.tlevel,
				 targs, XtNumber(targs));
    sargs[1].value = (XtArgVal) cw->cpick.slides[each];
    largs[2].value = (XtArgVal) cw->cpick.slides[each];
  }
}

static void dodialogok(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;
  Widget dw;
  char *response;

  dw = (Widget) cdata;
  cw = (CpickWidget) XtParent(dw);
  response = XawDialogGetValueString(dw);
  if (response[0] == '#') {
    response++;
  }
  if ((strlen(response) != 6 && strlen(response) != 3)
      || !hexChars(response)) {
    fprintf(stderr, "Illegal hex string.\n");
  } else if (strlen(response) == 6) {
    cw->cpick.allocated->red =
      (unhexChar(response[0])*16+unhexChar(response[1]))*RATIO;
    cw->cpick.allocated->green =
      (unhexChar(response[2])*16+unhexChar(response[3]))*RATIO;
    cw->cpick.allocated->blue =
      (unhexChar(response[4])*16+unhexChar(response[5]))*RATIO;
    cw->cpick.keep = FALSE;
    doNew(cw);
  } else if (strlen(response) == 3) {
    cw->cpick.allocated->red = unhexChar(response[0])*16*RATIO;
    cw->cpick.allocated->green = unhexChar(response[1])*16*RATIO;
    cw->cpick.allocated->blue = unhexChar(response[2])*16*RATIO;
    cw->cpick.keep = FALSE;
    doNew(cw);
  }
  XtUnmanageChild(dw);
  XtDestroyWidget(dw);
}

static void doreturn(w,e,s,c)
Widget w;
XEvent *e;
String *s;
Cardinal *c;
{
  dodialogok(w, (XtPointer) XtParent(w), (XtPointer) NULL);
}
  
static dohex(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;
  DialogWidget dw;
  char str[256];

  static String defaultTranslations =
    "<Key>Return:doreturn()";

  cw = (CpickWidget) cdata;

  createHex(cw->cpick.values[R], cw->cpick.values[G], cw->cpick.values[B],
	    str);

  dw = (DialogWidget) XtCreateWidget(argv0, dialogWidgetClass,
				     (Widget) cw, dargs, XtNumber(dargs));
  XawDialogAddButton((Widget) dw, "OK", dodialogok, (XtPointer) dw);
  XtRealizeWidget((Widget) dw);
  XtOverrideTranslations(dw->dialog.valueW,
			 XtParseTranslationTable(defaultTranslations));
  CenterWidget(cw, dw);
  XtManageChild((Widget) dw);
  XSetInputFocus(XtDisplay(dw), XtWindow(dw->dialog.valueW),
		 RevertToPointerRoot, CurrentTime);
}

static createBox(cw, argv0)
CpickWidget cw;
String argv0;
{
  bargs[0].value = (XtArgVal) cw->cpick.labels[H];
  bargs[1].value = (XtArgVal) cw->cpick.toplabel;
  cw->cpick.box = XtCreateManagedWidget(argv0, boxWidgetClass,
			      cw->cpick.tlevel, bargs, XtNumber(bargs));
  hargs[1].value = (XtArgVal) cw->cpick.box;
  hargs[2].value = (XtArgVal) cw->cpick.toplabel;
  cw->cpick.hex = XtCreateManagedWidget(argv0, commandWidgetClass,
			      cw->cpick.tlevel, hargs, XtNumber(hargs));
  XtAddCallback(cw->cpick.hex, XtNcallback, (XtCallbackProc) dohex,
		(XtPointer) cw);
/*
  cw->cpick.hex = XtCreateManagedWidget(argv0, labelWidgetClass,
			      cw->cpick.tlevel, hargs, XtNumber(hargs));
*/
}

static doNew(cw)
CpickWidget cw;
{
  static Arg args[] = {
    {XtNbackground, NULL},
  };

  newRGB(cw,
	 cw->cpick.allocated->red,
	 cw->cpick.allocated->green,
	 cw->cpick.allocated->blue);
  if (cw->cpick.box) {
    XtSetArg(args[0], XtNbackground,
	     (XtArgVal) cw->cpick.allocated->pixel);
    XtSetValues(cw->cpick.box, args, XtNumber(args));
  }
}

static nButtonHandler(w, val, event)
Widget w;
struct vals *val;
XButtonEvent *event;
{
  int newpix, v;
  static Arg args[] = {
    {XtNbackground, NULL},
  };
  static Arg labelargs1[] = {
    {XtNlabel, NULL},
  };
  XColor xc;
  CpickWidget cw = (CpickWidget) XtParent(XtParent(w));

  v = (event->x-val->x0)/val->iw + ((event->y-val->y0)/val->ih)*val->rownum;
  newpix = val->pix(cw, v);
  if (newpix != MAXIMUM) {
    xc.pixel = newpix;
    XQueryColor(XtDisplay(cw), cw->cpick.cmap, &xc);
    cw->cpick.allocated->red = xc.red;
    cw->cpick.allocated->green = xc.green;
    cw->cpick.allocated->blue = xc.blue;
    cw->cpick.keep = TRUE;
    doNew(cw);
    if (cw->cpick.matched) {
      labelargs1[0].value = (XtArgVal) cw->cpick.mnames[v];
      XtSetValues(cw->cpick.mlabel, labelargs1, XtNumber(labelargs1));
    }
  }
}

static unsigned long nPixel(cw, each)
CpickWidget cw;
int each;
{
  if (each < cw->cpick.nearpixels)
    return cw->cpick.nearcells[each].pixel;
  else
    return MAXIMUM;
}

static createGrid(wid, num, x0, y0, h, w, proc, val, pix)
Widget wid;
int num;
Position x0, y0;
Dimension h, w;
XtEventHandler proc;
struct vals *val;
PixProc pix;
{
  int each, x, y, hs, ws;
  double hsize, wsize;
  GC gc;
  XGCValues gcv;
  unsigned long black;

  val->pix = pix;

  hsize = sqrt((double) num*VNEARRATIO/HNEARRATIO);
  wsize = ceil(hsize*HNEARRATIO/VNEARRATIO);
  hsize = ceil(hsize);

  val->ih = (int) ((h-1)/hsize);
  val->iw = (int) ((w-1)/wsize);
  val->rownum = (int) wsize;
  hs = (int) hsize*val->ih+1;
  ws = (int) wsize*val->iw+1;

/*
  XtMoveWidget(wid, (Position) x0, (Position) y0);
  XtResizeWidget(wid, w, h, (Dimension) 1);
*/

  val->y0 = (h-hs)/2;
  val->x0 = (w-ws)/2;

  gcv.function = GXcopy;
  gcv.fill_style = FillSolid;
  black = XBlackPixel(XtDisplay(wid),
		      XDefaultScreen(XtDisplay(wid)));
  gcv.background = black;
  gc = XCreateGC(XtDisplay(wid), XtWindow(wid),
		 GCBackground | GCFunction | GCFillStyle, &gcv);

  XSync(XtDisplay(wid),0);
  for (each=0; each<num; each++) {
    x = (each % val->rownum)*val->iw;
    y = (each/val->rownum)*val->ih;
    XSetForeground(XtDisplay(wid), gc, pix(XtParent(XtParent(wid)), each));
    XFillRectangle(XtDisplay(wid), XtWindow(wid), gc, val->x0+x, val->y0+y,
		   val->iw, val->ih);
    XSetForeground(XtDisplay(wid), gc, black);
    XDrawRectangle(XtDisplay(wid), XtWindow(wid), gc, val->x0+x, val->y0+y,
		   val->iw, val->ih);
  }

  XtAddEventHandler(wid, ButtonPressMask, FALSE, (XtEventHandler) proc,
		    (XtPointer) val);
  XFreeGC(XtDisplay(wid), gc);
}

static doExpose(cw)
CpickWidget cw;
{
  createGrid(cw->cpick.nlevel, (int) cw->cpick.nearpixels,
	     getP(cw->cpick.nlevel, XtNx),
	     getP(cw->cpick.nlevel, XtNy),
	     getD(cw->cpick.nlevel, XtNheight),
	     getD(cw->cpick.nlevel, XtNwidth),
	     nButtonHandler, &nval, nPixel);
  if (!cw->cpick.matched) {
    if XtWindow(cw->cpick.mlabel)
      XtUnmapWidget(cw->cpick.mlabel);
  }
}

static Bool
ExposePending(display, event, arg)
Display *display;
XEvent *event;
Widget arg;
{
  return((event->type == Expose) &&
	 (((XExposeEvent *)event)->window == XtWindow(arg)));
}

static exposeHandler(w, client_data, event)
Widget w;
XtPointer client_data;
XEvent *event;
{
  XEvent ev;
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(w));

  XFlush(XtDisplay(w));
  while (XCheckIfEvent(XtDisplay(w), &ev, ExposePending, (char *) w)) {
    /* eat it */
  }
  
  doExpose(cw);
}

static doselect(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  undomatch(cw);
  if (cw->cpick.selectProc) {
    XtCallCallbacks((Widget) cw, XtNselectProc,
		    (XtPointer)(cw->cpick.allocated));
  }
}

static docancel(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  *(cw->cpick.allocated) = cw->cpick.oldvalue;
  doNew(cw);
}

static dorestore(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  if (cw->cpick.restoreProc) {
    XtCallCallbacks((Widget) cw, XtNrestoreProc,
		    (XtPointer)(cw->cpick.allocated));
  }
  changePalette(cw, TRUE);
}

static dook(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  if (cw->cpick.okProc) {
    XtCallCallbacks((Widget) cw, XtNokProc,
		    (XtPointer)(cw->cpick.allocated));
  }
}

static createCommands(cw)
CpickWidget cw;
{
  Widget last;

  last = cw->cpick.labels[H];

  cargs[1].value = (XtArgVal) last;
  cargs[2].value = (XtArgVal) cw->cpick.nlevel;
  if (cw->cpick.selectlabel) {
    cargs[0].value = (XtArgVal) cw->cpick.selectlabel;
    last = cw->cpick.select0 = XtCreateManagedWidget(argv0, commandWidgetClass,
	 		      cw->cpick.tlevel, cargs, XtNumber(cargs));
    XtAddCallback(cw->cpick.select0, XtNcallback,
		  (XtCallbackProc) doselect, NULL);
  }
  cargs[1].value = (XtArgVal) last;
  cargs[3].value = CDIST;
  if (cw->cpick.cancellabel) {
    cargs[0].value = (XtArgVal) cw->cpick.cancellabel;
    last = cw->cpick.cancel = XtCreateManagedWidget(argv0, commandWidgetClass,
				 cw->cpick.tlevel, cargs, XtNumber(cargs));
    XtAddCallback(cw->cpick.cancel, XtNcallback,
		  (XtCallbackProc) docancel, NULL);
  }
  cargs[1].value = (XtArgVal) last;
  if (cw->cpick.restorelabel) {
    cargs[0].value = (XtArgVal) cw->cpick.restorelabel;
    last = cw->cpick.restore = XtCreateManagedWidget(argv0, commandWidgetClass,
				  cw->cpick.tlevel, cargs, XtNumber(cargs));
    XtAddCallback(cw->cpick.restore, XtNcallback,
		  (XtCallbackProc) dorestore, NULL);
  }
  cargs[1].value = (XtArgVal) last;
  if (cw->cpick.oklabel) {
    cargs[0].value = (XtArgVal) cw->cpick.oklabel;
    last = cw->cpick.ok = XtCreateManagedWidget(argv0, commandWidgetClass,
			     cw->cpick.tlevel, cargs, XtNumber(cargs));
    XtAddCallback(cw->cpick.ok, XtNcallback,
		  (XtCallbackProc) dook, NULL);
  }
  mlargs[1].value = (XtArgVal) cw->cpick.labels[H];
  mlargs[2].value = (XtArgVal) cw->cpick.select0;
  cw->cpick.mlabel = XtCreateManagedWidget(argv0, labelWidgetClass,
				   cw->cpick.tlevel, mlargs, XtNumber(mlargs));
  cw->cpick.matched = FALSE;
}

static initPalette(cw)
CpickWidget cw;
{
  unsigned long cells[MAXPIXELS];
  int each;

  if (!XAllocColorCells(XtDisplay(cw), cw->cpick.cmap, FALSE, NULL, 0,
			cells, (int) cw->cpick.nearpixels)) {
    printf("Can't allocate %d cells for palette.\n", (int) cw->cpick.nearpixels);
    exit(1);
  }
  for (each=0; each<(int) cw->cpick.nearpixels; each++) {
    cw->cpick.nearcells[each].pixel = cells[each];
    cw->cpick.nearcells[each].flags = DoRed | DoGreen | DoBlue;
  }
  changePalette(cw, TRUE);
}

static changePalette(cw, new)
CpickWidget cw;
Boolean new;
{
  int each, each2, r, g, b, nr, ng, nb, eachr, eachg, eachb;
  int delta, halfr, halfg, halfb, near;
  double hueinc;
  RGB rgb;
  HSV hsv;
  int mods[MAXPIXELS][3], nummods;

  if (new) {
    undomatch(cw);
  }
  if (cw->cpick.keep) {
    cw->cpick.keep = FALSE;
  } else {
    if (cw->cpick.wide == NARROW) {
      halfr = halfg = halfb = (int) cw->cpick.nearpixels/2;
      r = cw->cpick.allocated->red;
      g = cw->cpick.allocated->green;
      b = cw->cpick.allocated->blue;

      for (each=0; each<(int) cw->cpick.nearpixels/2; each++) {
	if (r-halfr*NARROWNEAR < 0)
	  halfr--;
	if (r+((int) cw->cpick.nearpixels-1-halfr)*NARROWNEAR >= MAXIMUM)
	  halfr++;
	if (g-halfg*NARROWNEAR < 0)
	  halfg--;
	if (g+((int) cw->cpick.nearpixels-1-halfg)*NARROWNEAR >= MAXIMUM)
	  halfg++;
	if (b-halfb*NARROWNEAR < 0)
	  halfb--;
	if (b+((int) cw->cpick.nearpixels-1-halfb)*NARROWNEAR >= MAXIMUM)
	  halfb++;
      }

      halfb = halfg = halfr = (halfr+halfg+halfb)/3;
      for (each=0; each<(int) cw->cpick.nearpixels; each++) {
	nr = r+(each-halfr)*NARROWNEAR;
	ng = g+(each-halfg)*NARROWNEAR;
	nb =  b+(each-halfb)*NARROWNEAR;
	if (nr < 0)
	  nr = 0;
	if (ng < 0)
	  ng = 0;
	if (nb < 0)
	  nb = 0;
	if (nr >= MAXIMUM)
	  nr = MAXIMUM-1;
	if (ng >= MAXIMUM)
	  ng = MAXIMUM-1;
	if (nb >= MAXIMUM)
	  nb = MAXIMUM-1;
	cw->cpick.nearcells[each].red = nr;
	cw->cpick.nearcells[each].green = ng;
	cw->cpick.nearcells[each].blue = nb;
      }
      XStoreColors(XtDisplay(cw), cw->cpick.cmap, cw->cpick.nearcells,
		   (int) cw->cpick.nearpixels);
    } else if (cw->cpick.wide == WIDE) {
      near = WIDENEAR;
      delta = (int) exp(log((double) cw->cpick.nearpixels)/3.0);
      halfr = halfg = halfb = delta/2;

      r = cw->cpick.allocated->red;
      g = cw->cpick.allocated->green;
      b = cw->cpick.allocated->blue;

      for (each=0; each<delta/2; each++) {
	if (r-halfr*near < 0)
	  halfr--;
	if (r+(delta-1-halfr)*near >= MAXIMUM)
	  halfr++;
	if (g-halfg*near < 0)
	  halfg--;
	if (g+(delta-1-halfg)*near >= MAXIMUM)
	  halfg++;
	if (b-halfb*near < 0)
	  halfb--;
	if (b+(delta-1-halfb)*near >= MAXIMUM)
	  halfb++;
      }

      nummods = 0;
      for (eachb=0; eachb<delta; eachb++) {
	for (eachg=0; eachg<delta; eachg++) {
	  for (eachr=0; eachr<(int) cw->cpick.nearpixels/(delta*delta); eachr++) {
	    mods[nummods][R] = eachr-halfr;
	    mods[nummods][G] = eachg-halfg;
	    mods[nummods][B] = eachb-halfb;
	    nummods++;
	  }
	}
      }
      for (each=0; each<nummods-1; each++) {
	for (each2=each+1; each2<nummods; each2++) {
	  if (mods[each][R]+mods[each][G]+mods[each][B] >
	      mods[each2][R]+mods[each2][G]+mods[each2][B]) {
	    eachr = mods[each][R];
	    eachg = mods[each][G];
	    eachb = mods[each][B];
	    mods[each][R] = mods[each2][R];
	    mods[each][G] = mods[each2][G];
	    mods[each][B] = mods[each2][B];
	    mods[each2][R] = eachr;
	    mods[each2][G] = eachg;
	    mods[each2][B] = eachb;
	  }
	}
      }

      for (each=0; each<nummods; each++) {
	cw->cpick.nearcells[each].red = r+mods[each][R]*near;
	cw->cpick.nearcells[each].green = g+mods[each][G]*near;
	cw->cpick.nearcells[each].blue = b+mods[each][B]*near;
      }
      XStoreColors(XtDisplay(cw), cw->cpick.cmap, cw->cpick.nearcells,
		   (int) cw->cpick.nearpixels);
    } else if (new) {
      hueinc = 1.0/cw->cpick.nearpixels;
      hsv.s = hsv.v = 1.0;
      hsv.h = 0.0;
      for (each=0; each<(int) cw->cpick.nearpixels; each++) {
	rgb = HSVToRGB(hsv);
	cw->cpick.nearcells[each].red = rgb.r;
	cw->cpick.nearcells[each].green = rgb.g;
	cw->cpick.nearcells[each].blue = rgb.b;
	hsv.h = hsv.h + hueinc;
      }
      XStoreColors(XtDisplay(cw), cw->cpick.cmap, cw->cpick.nearcells,
		   (int) cw->cpick.nearpixels);
    }
  }
}

static dopalette(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));

  cw->cpick.wide = (cw->cpick.wide+1) % (RANGE+1);
  if (cw->cpick.wide == WIDE) {
    plabelargs[0].value = (XtArgVal) "wide";
  } else if (cw->cpick.wide == RANGE) {
    plabelargs[0].value = (XtArgVal) "range";
  } else {
    plabelargs[0].value = (XtArgVal) "narrow";
  }
  XtSetValues(cw->cpick.palette, plabelargs, XtNumber(plabelargs));
  changePalette(cw, TRUE);
}

static domatch(cmd, cdata, data)
     Widget cmd;
     XtPointer cdata;
     XtPointer data;
{
  CpickWidget cw;

  cw = (CpickWidget) XtParent(XtParent(cmd));
  cw->cpick.matched = TRUE;
  XtMapWidget(cw->cpick.mlabel);
  matchPalette(cw);
}

static undomatch(cw)
CpickWidget cw;
{
  if (cw->cpick.matched) {
    cw->cpick.matched = FALSE;
    if XtWindow(cw->cpick.mlabel)
      XtUnmapWidget(cw->cpick.mlabel);
  }
}

static matchPalette(cw)
CpickWidget cw;
{
  int each, each2, count, r, g, b, nr, ng, nb, cr, cg, cb, d;
  FILE *fd;
  char name[MAXNAME], *sname;
  static Arg labelargs1[] = {
    {XtNlabel, NULL},
  };

  for (each=0; each<MAXPIXELS; each++) {
    strcpy(cw->cpick.mnames[each], NULLSTR);
    cw->cpick.mdist[each] = MBASE*MBASE*3;
  }
  cr = cw->cpick.allocated->red/RATIO;
  cg = cw->cpick.allocated->green/RATIO;
  cb = cw->cpick.allocated->blue/RATIO;
  fd = fopen(RGBFILE, "r");
  if (fd == NULL || feof(fd)) {
    undomatch(cw);
  } else {
    count = 0;
    r = g = b = -1;
    while (!feof(fd)) {
      fscanf(fd, "%d %d %d", &nr, &ng, &nb);
      fgets(name, MAXNAME, fd);
      name[strlen(name)-1] = '\0';
      sname = name;
      while (sname[0] == ' ' || sname[0] == '\t') {
	sname++;
      }
      if (nr != r || ng != g || nb != b) {
	r = nr;
	g = ng;
	b = nb;
	d = (nr-cr)*(nr-cr)+(ng-cg)*(ng-cg)+(nb-cb)*(nb-cb);
	for (each = 0; each<count; each++) {
	  if (d < cw->cpick.mdist[each]) {
	    for (each2 = cw->cpick.nearpixels-1; each2>= each; each2--) {
	      cw->cpick.nearcells[each2+1].red =
		cw->cpick.nearcells[each2].red;
	      cw->cpick.nearcells[each2+1].green =
		cw->cpick.nearcells[each2].green;
	      cw->cpick.nearcells[each2+1].blue =
		cw->cpick.nearcells[each2].blue;
	      cw->cpick.mdist[each2+1] = cw->cpick.mdist[each2];
	      strcpy(cw->cpick.mnames[each2+1], cw->cpick.mnames[each2]);
	    }
	    cw->cpick.nearcells[each].red = nr*RATIO;
	    cw->cpick.nearcells[each].green = ng*RATIO;
	    cw->cpick.nearcells[each].blue = nb*RATIO;
	    cw->cpick.mdist[each] = d;
	    strcpy(cw->cpick.mnames[each], sname);
	    if (count < cw->cpick.nearpixels) {
	      count++;
	    }
#ifdef BUBBLE_MATCH
	    XStoreColors(XtDisplay(cw), cw->cpick.cmap,
			 &(cw->cpick.nearcells[each]),
			 (int) (cw->cpick.nearpixels-each));
#endif
	    break;
	  }
	}
	if (count < cw->cpick.nearpixels) {
	  for (each2 = cw->cpick.nearpixels-1; each2>= count; each2--) {
	    cw->cpick.nearcells[each2+1].red =
	      cw->cpick.nearcells[each2].red;
	    cw->cpick.nearcells[each2+1].green =
	      cw->cpick.nearcells[each2].green;
	    cw->cpick.nearcells[each2+1].blue =
	      cw->cpick.nearcells[each2].blue;
	    cw->cpick.mdist[each2+1] = cw->cpick.mdist[each2];
	    strcpy(cw->cpick.mnames[each2+1], cw->cpick.mnames[each2]);
	  }
	  cw->cpick.nearcells[count].red = nr*RATIO;
	  cw->cpick.nearcells[count].green = ng*RATIO;
	  cw->cpick.nearcells[count].blue = nb*RATIO;
	  cw->cpick.mdist[count] = d;
	  strcpy(cw->cpick.mnames[count], sname);
#ifdef BUBBLE_MATCH
	  XStoreColors(XtDisplay(cw), cw->cpick.cmap,
		       &(cw->cpick.nearcells[count]),
		       (int) (cw->cpick.nearpixels-count));
#endif
	  count++;
	}	  
      }
    }
    if (count != 0 &&
	cw->cpick.nearcells[0].red == cw->cpick.allocated->red &&
	cw->cpick.nearcells[0].green == cw->cpick.allocated->green &&
	cw->cpick.nearcells[0].blue == cw->cpick.allocated->blue) {
      labelargs1[0].value = (XtArgVal) cw->cpick.mnames[0];
      XtSetValues(cw->cpick.mlabel, labelargs1, XtNumber(labelargs1));
    } else {
      labelargs1[0].value = (XtArgVal) " ";
      XtSetValues(cw->cpick.mlabel, labelargs1, XtNumber(labelargs1));
    }
    fclose(fd);
#ifndef BUBBLE_MATCH
    XStoreColors(XtDisplay(cw), cw->cpick.cmap,
		 cw->cpick.nearcells,
		 (int) cw->cpick.nearpixels);
#endif
  }
}

static createPalette(cw)
CpickWidget cw;
{
/*
  nargs[0].value = (XtArgVal) XWhitePixel(XtDisplay(cw),
					  XDefaultScreen(XtDisplay(cw)));
*/
  nargs[1].value = (XtArgVal) cw->cpick.labels[H];
  nargs[2].value = (XtArgVal) cw->cpick.box;
  cw->cpick.nlevel = XtCreateManagedWidget(argv0, formWidgetClass,
				 cw->cpick.tlevel, nargs, XtNumber(nargs));

  XtAddEventHandler(cw->cpick.nlevel, ExposureMask, TRUE,
		    (XtEventHandler) exposeHandler, NULL);

  pargs[1].value = p2args[1].value = (XtArgVal) cw->cpick.box;
  pargs[2].value = (XtArgVal) cw->cpick.hex;
  cw->cpick.wide = RANGE;
  cw->cpick.keep = FALSE;
  cw->cpick.matched = FALSE;
  cw->cpick.palette = XtCreateManagedWidget(argv0, commandWidgetClass,
					    cw->cpick.tlevel, pargs,
					    XtNumber(pargs));
  XtAddCallback(cw->cpick.palette, XtNcallback,
		(XtCallbackProc) dopalette, NULL);
  p2args[2].value = (XtArgVal) cw->cpick.palette;
  cw->cpick.match = XtCreateManagedWidget(argv0, commandWidgetClass,
					  cw->cpick.tlevel, p2args,
					  XtNumber(p2args));
  XtAddCallback(cw->cpick.match, XtNcallback,
		(XtCallbackProc) domatch, NULL);
}

static void Initialize( request, new )
   Widget request;		/* what the client asked for */
   Widget new;			/* what we're going to give him */
{
  static Arg args[] = {
    {XtNbackground, NULL},
  };
  static Arg fargs[] = {
    {XtNborderWidth, (XtArgVal) 0},
  };

  int each, depth;

  CpickWidget cw = (CpickWidget) new;

  cw->cpick.tlevel = XtCreateManagedWidget(argv0, formWidgetClass, (Widget) cw,
					   fargs, XtNumber(fargs));

  depth = XDisplayPlanes(XtDisplay(cw),
			 XDefaultScreen(XtDisplay(cw)));
  cw->cpick.inc = INCREMENT;
  if (!cw->cpick.inc)
    cw->cpick.inc = 1;

  cw->cpick.box = NULL;
  createSlides(cw, argv0);

  createBox(cw, argv0);

  createPalette(cw);

  createCommands(cw);

  initPalette(cw);

  if (cw->cpick.allocated == NULL) {
    printf("Must set XtNallocated resource.\n");
    exit(1);
    doselect(cw->cpick.select0, NULL, (XtPointer) 0);
  } else {
    doNew(cw);
  }

  if (cw->core.width == 0)
    cw->core.width = DEFAULTWIDTH;
  if (cw->core.height == 0)
    cw->core.height = DEFAULTHEIGHT;
}
