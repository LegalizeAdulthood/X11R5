#include "display.h"
#include "image.h"
#include "X.h"

#ifdef PRE_R5_ICCCM
/*
  Compatibility routines for pre X11R5 ICCCM.
*/
XrmDatabase XrmGetDatabase(display)
Display 
  *display;
{
  return(display->db);
}
#endif

#ifdef PRE_R4_ICCCM
/*
  Compatibility routines for pre X11R4 ICCCM.
*/
XClassHint *XAllocClassHint()
{
  return((XClassHint *) malloc(sizeof(XClassHint)));
}

XIconSize *XAllocIconSize()
{
  return((XIconSize *) malloc(sizeof(XIconSize)));
}

XSizeHints *XAllocSizeHints()
{
  return((XSizeHints *) malloc(sizeof(XSizeHints)));
}

Status XReconfigureWMWindow(display,window,screen_number,value_mask,values)
Display
  *display;

Window
  window;

int
  screen_number;

unsigned int
  value_mask;

XWindowChanges
  *values;
{
  return(XConfigureWindow(display,window,value_mask,values));
}

XStandardColormap *XAllocStandardColormap()
{
  return((XStandardColormap *) malloc(sizeof(XStandardColormap)));
}

XWMHints *XAllocWMHints()
{
  return((XWMHints *) malloc(sizeof(XWMHints)));
}

Status XGetRGBColormaps(display,window,colormap,count,property)
Display
  *display;

Window
  window;

XStandardColormap
  **colormap;

int
  *count;

Atom
  property;
{
  *count=1;
  return(XGetStandardColormap(display,window,colormap,property));
}

Status XGetWMName(display,window,text_property)
Display
  *display;

Window
  window;

XTextProperty
  *text_property;
{
  char
    *window_name;

  if (XFetchName(display,window,&window_name) == 0)
    return(False);
  text_property->value=(unsigned char *) window_name;
  text_property->encoding=XA_STRING;
  text_property->format=8;
  text_property->nitems=strlen(window_name);
  return(True);
}

char *XResourceManagerString(display)
Display 
  *display;
{
  return display->xdefaults;
}

void XrmDestroyDatabase(database)
XrmDatabase
  database;
{
}

void XSetWMProperties(display,window,window_name,icon_name,argv,argc,
  size_hints,manager_hints,class_hint)
Display
  *display;

Window
  window;

XTextProperty
  *window_name,
  *icon_name;

char
  **argv;

int
  argc;

XSizeHints
  *size_hints;

XWMHints *manager_hints;

XClassHint
  *class_hint;
{
  XSetStandardProperties(display,window,window_name->value,icon_name,None,
    argv,argc,size_hints);
  XSetWMHints(display,window,manager_hints);
  XSetClassHint(display,window,class_hint);
}

Status XSetWMProtocols(display,window,protocols,count)
Display
  *display;

Window
  window;

Atom
  *protocols;

int
  count;
{
  Atom
    protocols_property;

  protocols_property=XInternAtom(display,"WM_PROTOCOLS",False);
  XChangeProperty(display,window,protocols_property,XA_ATOM,32,PropModeReplace,
    (unsigned char *) protocols, count);
  return(True);
}

VisualID XVisualIDFromVisual(visual)
Visual
  *visual;
{
  return(visual->visualid);
}

Status XWithdrawWindow(display,window,screen)
Display
  *display;

Window
  window;

int
  screen;
{
  return(XUnmapWindow(display,window));
}

int XWMGeometry(display,screen,user_geometry,default_geometry,border_width,
  size_hints,x,y,width,height,gravity)
Display
  *display;

int
  screen;

char
  *user_geometry,
  *default_geometry;

unsigned int
  border_width;

XSizeHints
  *size_hints;

int
  *x,
  *y,
  *width,
  *height,
  *gravity;
{
  int
    status;

  status=XGeometry(display,screen,user_geometry,default_geometry,border_width,
    0,0,0,0,x,y,width,height);
  *gravity=NorthWestGravity;
  return(status);
}
#endif
