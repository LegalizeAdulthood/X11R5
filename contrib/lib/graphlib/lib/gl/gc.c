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
#include "X11/gl/gc.h"

void GL_GetGC(win,gc,va_alist)
Window win;
GC *gc;
va_dcl
{
  va_list pvar;
  unsigned long valuemask = 0;
  XGCValues values;
  unsigned int line_width = 1;  /* use fastest width=1 line */
  int line_style = LineSolid;
  int cap_style = CapButt;      /* for drawing nice, square boxes */
  int join_style = JoinMiter;
  int dash_offset = 0;          /* If they do pick dashes, leave a
                                   sane value for them */
  static char dash_list[] = {6, 6};
  int list_length = 2;

  checkinit();

  /* Create default Graphics Context */
  *gc = XCreateGC(display, win, valuemask, &values);
  /* specify font */
  XSetFont(display, *gc, default_font->fid);

  /* specify black foreground since default may be white on white */
 XSetForeground(display, *gc, default_foreground);
  XSetBackground(display, *gc, default_background);

  /* set line attributes */
  XSetLineAttributes(display, *gc, line_width, line_style, cap_style, join_style)
;

  /* set dashes to be line_width in length */
  XSetDashes(display, *gc, dash_offset, dash_list, list_length);
}
