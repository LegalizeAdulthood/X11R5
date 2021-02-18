/*
 * pixedit.c - Pixel color editor for X11
 * 
 * Author:	Mike Yang (mikey@sgi.com)
 *		Silicon Graphics, Inc.
 * Date:	Mon Aug 9 1988, fix for R3 Wed Mar 8 1989
 *		  fix for R4 Fri Jul 13 1990
 *		  update to R5 Tue May 28 1991
 *
 * Copyright (c) 1988, 1991 Mike Yang
 *
 */

#include <stdio.h>
#include <math.h>
#include <X11/Xos.h>
#include <X11/Xproto.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include "Cpick.h"
#include <X11/cursorfont.h>

XColor allocated, current;
Widget realtop, toplevel, cpick;
XColor mapv[MAXPIXELS];
Colormap cmap;
Boolean picking;
int maxpixels, numpixels, numfree, realfree;
unsigned long avail[MAXPIXELS], got[MAXPIXELS];
char *argv0;
Position choose_x = 200, choose_y = 200;

typedef unsigned long (*PixProc)();

struct vals {
  int ih, iw, x0, y0, rownum;
  PixProc pix;
} gval, nval;

Arg gargs[] = {
  {XtNborderColor, NULL},
  {XtNheight, 1},
  {XtNwidth, 1},
  {XtNfromHoriz, NULL},
  {XtNfromVert, NULL},
  {XtNhorizDistance, (XtArgVal) 0},
  {XtNvertDistance, (XtArgVal) 0},
};

Arg setargs[] = {
  {XtNallocated, (XtArgVal) &allocated},
};

Dimension getD(w, s)
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

Position getP(w, s)
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

saveCurrent()
{
  current.red = allocated.red;
  current.green = allocated.green;
  current.blue = allocated.blue;
  XStoreColor(XtDisplay(toplevel), cmap, &current);
}

restoreCmap()
{
  Display *dpy;
  int screen, each, each2, count;
  Boolean found;
  XColor copy[MAXPIXELS];

  dpy = XtDisplay(toplevel);
  screen = XDefaultScreen(XtDisplay(toplevel));

  count = 0;
  for (each=0; each<numpixels; each++) {
    found = FALSE;
    for (each2=0; (each2<=realfree) && !found; each2++) {
      if (avail[each2] == got[each])
	found = TRUE;
    }
    if (!found)
      copy[count++] = mapv[got[each]];
  }

  XStoreColors(dpy,cmap,copy,count);
  XInstallColormap(dpy,cmap);

  XStoreColors(dpy,cmap,copy,count);
  XInstallColormap(dpy,cmap);
}

doSelect(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     float position;
{
  Window chosen, ignorew, last;
  int ignore;
  unsigned int ignoreu;
  XEvent ev;
  int x, y, each;
  unsigned long newpix;

  printf("Click on a pixel to edit its colormap value.\n");
  picking = TRUE;
  XGrabPointer(XtDisplay(toplevel),
	       XRootWindowOfScreen(XDefaultScreenOfDisplay(XtDisplay(toplevel))),
	       FALSE,
	       ButtonPressMask,
	       GrabModeAsync,
	       GrabModeSync,
	       None,
	       XCreateFontCursor(XtDisplay(toplevel), XC_crosshair),
	       CurrentTime);

  XMaskEvent(XtDisplay(toplevel), ButtonPressMask, &ev);
  if (ev.type != ButtonPress) {
    printf("Impossible.\n");
    exit(1);
  }
  last = XDefaultRootWindow(XtDisplay(toplevel));
  chosen = last;
  while (chosen != None) {
    XQueryPointer(XtDisplay(toplevel), last,  &ignorew, &chosen,
		  &ignore, &ignore, &ignore, &ignore, &ignoreu);
    if (chosen != None)
      last = chosen;
  }
  XUngrabPointer(XtDisplay(toplevel), CurrentTime);
  choose_x = ev.xbutton.x;
  choose_y = ev.xbutton.y;
  if (!cpick)
    createCpick(last);
  if (picking) {
    x = ((XButtonEvent *) &ev)->x_root;
    y = ((XButtonEvent *) &ev)->y_root;
    newpix = XGetPixel(XGetImage(XtDisplay(toplevel), 
					XRootWindowOfScreen(XDefaultScreenOfDisplay(XtDisplay(toplevel))),
					x, y, 1, 1,
					512-1 /* yuck dependency */, ZPixmap),
			      0, 0);
    for (each=0; each<=realfree; each++) {
      if (newpix == avail[each]) {
	fprintf(stderr,
		"That's one of the widget's colormap cells.  Pick another.\n");
	XBell(XtDisplay(cmd), 0);
	doSelect(cmd, cdata, position);
	return;
      }
    }
    current.pixel = newpix;
    XQueryColor(XtDisplay(toplevel), cmap, &current);
    allocated.red = current.red;
    allocated.green = current.green;
    allocated.blue = current.blue;
    XSync(XtDisplay(toplevel), 0);
  }
  if (picking) {
    /* generates error if can't allocate */
    XStoreColor(XtDisplay(toplevel), cmap, &current);
    XSync(XtDisplay(toplevel), 0);
    if (picking) {
      XtSetValues(cpick, setargs, XtNumber(setargs));
      picking = FALSE;
    } else {
      doSelect(cmd, cdata, position);
    }
  } else {
    doSelect(cmd, cdata, position);
  }
}

doOk(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     float position;
{
  restoreCmap();
  exit(0);
}

doChange(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     float position;
{
  saveCurrent();
}

doRestore(cmd, cdata, position)
     Widget cmd;
     XtPointer cdata;
     float position;
{
  restoreCmap();
  XQueryColor(XtDisplay(toplevel), cmap, &current);
  allocated.red = current.red;
  allocated.green = current.green;
  allocated.blue = current.blue;
  XtSetValues(cpick, setargs, XtNumber(setargs));
}

int createCmap(num, wcmap, visual)
int num;
Colormap wcmap;
Visual *visual;
{
  Display *dpy;
  int screen, each, real;
  XSetWindowAttributes xswa;

  real = num+1;

  dpy = XtDisplay(toplevel);
  screen = XDefaultScreen(XtDisplay(toplevel));

  for (each=0; each<maxpixels; each++) {
    mapv[each].pixel = each;
    mapv[each].flags = (DoRed | DoGreen | DoBlue);
  }
  XQueryColors(dpy, wcmap, mapv, numpixels);

  while ((real > 1) &&
	 (!XAllocColorCells(dpy, wcmap, FALSE, NULL, 0, avail,
			    real+1))) {
    real--;
  }

  if (real <= 1) {
    printf("Sorry, can't allocate enuf colormap cells.\n");
    exit(1);
  }

  cmap = XCreateColormap(dpy, XRootWindow(dpy,screen),
			 visual, AllocNone);

  XAllocColorCells(dpy, cmap, FALSE, NULL, 0, got, numpixels);
  XStoreColors(dpy,cmap,mapv,numpixels);
  XFreeColors(dpy, cmap, avail, real, 0);
  allocated = mapv[avail[real]];

  XInstallColormap(dpy,cmap);

  return real-1;
}

errorHandler(dpy, event)
Display *dpy;
XErrorEvent *event;
{
  char buf[256];

  if ((event->error_code == BadAccess) ||
      (event->request_code == X_QueryColors)) {
    printf("Can't allocate that colormap cell.  Pick another.\n");
    picking = FALSE;
  } else {
    XGetErrorText(dpy, event->error_code, buf, 256);
    printf("X protocol error detected by server: %s\n", buf);
    printf("  Failed request major op code %d\n", (int) event->request_code);
    exit(1);
  }
}

createCpick(win)
Window win;
{
  static Arg args[] = {
    {XtNcmap, NULL},
    {XtNuseColors, TRUE},
    {XtNnearPixels, NULL},
    {XtNallocated, (XtArgVal) &allocated},
    {XtNfromHoriz, NULL},
    {XtNfromVert, NULL},
    {XtNhorizDistance, (XtArgVal) 0},
    {XtNvertDistance, (XtArgVal) 0},
    {XtNokLabel, (XtArgVal) "quit"},
    {XtNborderWidth, (XtArgVal) 0},
  };
  
  static Arg cargs[] = {
    {XtNcolormap, NULL},
  };
  
  XVisualInfo *vip, viproto, *bestvip;
  XWindowAttributes xwa;
  int nvi, i;
  Dimension cw, ch;
  Position x, y;

  XGetWindowAttributes(XtDisplay(toplevel), win, &xwa);
  viproto.visual = xwa.visual;
  viproto.class = PseudoColor;
  vip = XGetVisualInfo(XtDisplay(toplevel), VisualClassMask, &viproto, &nvi);
  if (!nvi || !xwa.colormap) {
    fprintf(stderr, "Sorry, that window doesn't have a colormap to edit (no PseudoColor visual).\n");
    XBell(XtDisplay(toplevel), 0);
    picking = FALSE;
  } else {
/* First try to find a visual that matches the selected window's visual */
    bestvip = (XVisualInfo *) NULL;
    for (i=0; i<nvi; i++) {
      if (xwa.visual == vip[i].visual) {
	bestvip = vip+i;
      }
    }
/* Otherwise, find the pseudocolor visual with the most colormap entries,
   but within our hardcoded limit */
    if (!bestvip || bestvip->colormap_size > MAXPIXELS) {
      bestvip = vip;
      for (i=1; i<nvi; i++) {
	if (bestvip->colormap_size < vip[i].colormap_size &&
            vip[i].colormap_size <= MAXPIXELS) {
	  bestvip = vip+i;
	}
      }
    }
    numpixels = bestvip->colormap_size;
    maxpixels = 1;
    while (maxpixels < numpixels) {
      maxpixels = maxpixels*2;
    }
    
    if (numpixels < 11) {
      printf("Sorry, you don't have enough colormap cells.\n");
      exit(1);
    } else if (numpixels < 58) {
      args[1].value = (XtArgVal) FALSE;
      args[2].value = (XtArgVal) 4;
      numfree = COLORLESSPIXELS+4;
    } else if (numpixels < 136) {
      args[1].value = (XtArgVal) TRUE;
      args[2].value = (XtArgVal) 25;
      numfree = NECESSARYPIXELS+25;
    } else {
      args[1].value = (XtArgVal) TRUE;
      args[2].value = (XtArgVal) 64;
      numfree = NECESSARYPIXELS+64;
    }
    
    realfree = createCmap(numfree, xwa.colormap, bestvip->visual);
    XFree((char *) vip);
    
    args[0].value = (XtArgVal) cmap;
    if (realfree != numfree)
      if (args[1].value == (XtArgVal) TRUE)
	args[2].value = (XtArgVal) realfree-NECESSARYPIXELS;
      else
	args[2].value = (XtArgVal) realfree-COLORLESSPIXELS;
    
    cpick = XtCreateManagedWidget(argv0, cpickWidgetClass,
				  toplevel, args, XtNumber(args));
    XtAddCallback(cpick, XtNselectProc, (XtCallbackProc) doSelect, NULL);
    XtAddCallback(cpick, XtNokProc, (XtCallbackProc) doOk, NULL);
    XtAddCallback(cpick, XtNchangeProc, (XtCallbackProc) doChange, NULL);
    XtAddCallback(cpick, XtNrestoreProc, (XtCallbackProc) doRestore, NULL);
    
    cw = getD(cpick, XtNwidth);
    ch = getD(cpick, XtNheight);
    XGetWindowAttributes(XtDisplay(cpick),
			 XDefaultRootWindow(XtDisplay(cpick)), &xwa);
    x = choose_x-(Position) (cw/2);
    y = choose_y-(Position) (ch/2);
    if (x < 0) {
      x = 0;
    } else if (x > xwa.width-cw) {
      x = xwa.width-cw;
    }
    if (y < 0) {
      y = 0;
    } else if (y > xwa.height-ch) {
      y = xwa.height-ch;
    }
    
    XtMoveWidget(realtop, x, y);
    XtRealizeWidget(realtop);

    XSetWindowColormap(XtDisplay(realtop), XtWindow(realtop), cmap);
    XSetWindowColormap(XtDisplay(toplevel), XtWindow(toplevel), cmap);
    XSetWindowColormap(XtDisplay(cpick), XtWindow(cpick), cmap);
  }
}

main(argc, argv)
    int argc;
    char **argv;
{
  int each;
  
  argv0 = argv[0];
  allocated.flags = current.flags = DoRed | DoGreen | DoBlue;
  
  realtop = XtInitialize(argv[0], "Pixedit", NULL, 0, &argc, argv);
  toplevel = XtCreateManagedWidget(argv[0], formWidgetClass,
				   realtop, NULL, 0);
  
  if (XDisplayPlanes(XtDisplay(toplevel),
		     XDefaultScreen(XtDisplay(toplevel))) == 1) {
    printf("Sorry, you need a color display.\n");
    exit(1);
  }

  XSetErrorHandler(errorHandler);
  
  doSelect(cpick, NULL, NULL);

  XtMainLoop();
}
