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
#include <stdio.h>

void GL_VA_handle_geometry(resourcename,cmdlinename,resource,destptr)
char *resourcename,*cmdlinename,*resource;
GL_WindowInfo *destptr;
{
  int retval;

  retval = XParseGeometry(resource,&(destptr->x),&(destptr->y),
			  &(destptr->width),&(destptr->height));
  if (retval&XNegative) 
    destptr->x=DisplayWidth(display,screen)+destptr->x-destptr->width-
      2*destptr->borderwidth;
  if (retval&YNegative)
    destptr->y=DisplayHeight(display,screen)+destptr->y-destptr->height-
      2*destptr->borderwidth;
  if (retval&XValue)
    destptr->size_hints.flags|=USPosition;
  if (retval&WidthValue)
    destptr->size_hints.flags|=USSize;
}
