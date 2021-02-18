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

#include "X11/gl/invertrect.h"
#include <stdio.h>

void Intersect_Rectangles(x1,y1,width1,height1,
			  x2,y2,width2,height2,
			  x,y,width,height)
int x1,y1,width1,height1,x2,y2,width2,height2;
int *x,*y,*width,*height;
{
  register int xa,xb,ya,yb;

  xa = MAX(x1,x2);
  xb = MIN(x1+width1,x2+width2);
  ya = MAX(y1,y2);
  yb = MIN(y1+height1,y2+height2);

  *x = xa;
  *y = ya;
  *width = xb-xa;
  *height = yb-ya;
}

static void _DrawInvertObject(report,oinfo)
XExposeEvent *report;
GL_ObjectInfo *oinfo;
{
  int x,y,width,height;

  if ((oinfo->flags&GLO_MASK_INVERTED)==0)
    return;
		       
  Intersect_Rectangles(report->x,report->y,report->width,report->height,
		       oinfo->xmin,oinfo->ymin,
		       oinfo->width,oinfo->height,
		       &x,&y,&width,&height);
  XFillRectangle(display,oinfo->drawwin,default_gc,x,y,width,height);
}

static void _InvertFunc(oinfo)
GL_ObjectInfo *oinfo;
{
  XFillRectangle(display,oinfo->drawwin,default_xor_gc,
		 oinfo->xmin,oinfo->ymin,oinfo->width,oinfo->height);
}

void _GL_VA_Handle_InvertibleRectangle(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_ObjectInfo *oinfo;

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);
  switch (modifier) 
    {
    case GLIR_InvertRect : 
      oinfo->x = va_arg(*pvar,int);
      oinfo->y = va_arg(*pvar,int);
      oinfo->width = va_arg(*pvar,int);
      oinfo->height = va_arg(*pvar,int);
      oinfo->xmin = oinfo->x;
      oinfo->ymin = oinfo->y;
      break;
      default : 
	_GL_VA_Handle_Object(list,modifier,pvar);
    }
}

void _GL_VA_Execute_InvertibleRectangle(list,modifier,pvar)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);

  oinfo->draw = _DrawInvertObject;
  oinfo->invert = _InvertFunc;
}

