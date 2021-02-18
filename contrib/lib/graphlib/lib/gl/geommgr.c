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


#include <stdio.h>
#include "X11/gl/window.h"

struct where {
  Window awin;
  int x,y;
};

Window GL_BuildSurroundWindow(win_name,va_alist)
char *win_name;
va_dcl
{
  va_list pvar;
  Window winid;
  int width=0,height=0,tmp,count=0;
  XWindowAttributes win_attr;
  struct where *windows;

  va_start(pvar);
  for(winid = va_arg(pvar,Window);winid!=NULL;winid=va_arg(pvar,Window)) {
    count++;
  }
  windows = (struct where *)malloc(count*sizeof(struct where));
  va_start(pvar);
  count=0;
  for(winid = va_arg(pvar,Window);winid!=NULL;winid=va_arg(pvar,Window)) {
    if (!XGetWindowAttributes(display,winid,&win_attr)) {
      fprintf(stderr,"Well, gee, that window really doesn't feel good today.\n");
      fprintf(stderr,"Actually,it feels really bad.\n");
      fprintf(stderr,"Window id %d has been reported as bad by the server\n",winid);
      fprintf(stderr,"I feel pretty bad too.\n");
      fprintf(stderr,"I think I will go to bed, so I guess we will just have\n");
      fprintf(stderr,"to shut down this program.\nBye!\n");
      exit(-1);
    }
    tmp = win_attr.x + win_attr.width+win_attr.border_width*2;
    windows[count].x = win_attr.x;
    windows[count].y = win_attr.y;
    windows[count].awin = winid;
    count++;
    if (tmp>width)
      width=tmp;
    tmp = win_attr.y + win_attr.height+win_attr.border_width*2;
    if (tmp>height)
      height=tmp;
  }
  winid = GL_CreateWindow(GL_Window,
			  GL_WinHeight, height,
			  GL_WinWidth, width,
			  GL_WinName, win_name,
			  GL_PrimaryWindow,
			  GL_Done);
  for(tmp=0;tmp<count;tmp++) {
    XReparentWindow(display,windows[tmp].awin,winid,windows[tmp].x,windows[tmp].y);
  }
  return winid;
}
