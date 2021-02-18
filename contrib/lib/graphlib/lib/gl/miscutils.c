/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */


#include "X11/gl/window.h"
#include "X11/gl/miscutils.h"
static void _GL_SendExpose(win)
Window win;
{
  XEvent sending;
  Window root;
  int x,y;
  unsigned w,h,bw,d;
  XGetGeometry(display,win,&root,&x,&y,&w,&h,&bw,&d);
  sending.type=Expose;
  sending.xexpose.display=display;
  sending.xexpose.window=win;
  sending.xexpose.x=0;
  sending.xexpose.y=0;
  sending.xexpose.width=w;
  sending.xexpose.height=h;
  sending.xexpose.count=0;
  XSendEvent(display,win,0,0,&sending);
}

void GL_ExposeChildren(win)
Window win;
{
  Window root,parent,*children;
  unsigned int n_children,x;
  XQueryTree(display,win,&root,&parent,&children,&n_children);
  for (x=0;x<n_children;x++)
    {
      _GL_SendExpose(children[x]);
      GL_ExposeChildren(children[x]);
    }
  XFree(children);
}

unsigned long GL_GetColor(colorname,mononame)
char *colorname, *mononame;
{
  XColor scrcol,exactcol;
  
  if (!XAllocNamedColor(display,DefaultColormap(display,screen),
    (DefaultDepth(display,screen)<2)?mononame:colorname,&scrcol,&exactcol))
    return BlackPixel(display,screen);
  else
    return scrcol.pixel;
}

void GL_SetWinColor(win,colorname,monopix)
Window win;
char *colorname;
Pixmap monopix;
{
  XColor scrcol,exactcol;

  if ((DefaultDepth(display,screen)>1)&&
      XAllocNamedColor(display,DefaultColormap(display,screen),
		       colorname,&scrcol,&exactcol)) {
    XSetWindowBackground(display,win,scrcol.pixel);
  } else {
    XSetWindowBackgroundPixmap(display,win,monopix);
  }
}

void GL_SetGCColor(gc,colorname,monopix)
GC gc;
char *colorname;
Pixmap monopix;
{
  XColor scrcol,exactcol;

  if ((DefaultDepth(display,screen)>1)&&
      XAllocNamedColor(display,DefaultColormap(display,screen),
		       colorname,&scrcol,&exactcol)) {
      XSetForeground(display,gc,scrcol.pixel);
    } else {
      XSetForeground(display,gc,default_foreground);
      XSetBackground(display,gc,default_background);
      XSetStipple(display,gc,monopix);
      XSetTSOrigin(display,gc,0,0);
      XSetFillStyle(display,gc,FillStippled);
    }
}
