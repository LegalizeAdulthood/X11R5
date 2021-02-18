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

#include "X11/gl/lineobj.h"

static void _DrawLineObject(report,oinfo)
XExposeEvent *report;
GL_ObjectInfo *oinfo;
{
  GL_LineObjectInfo *linfo;
  GC workgc;
  XRectangle theRectangle;

  linfo = (GL_LineObjectInfo *)GL_Search_DataList(oinfo->thelist,
						  _GL_VA_Init_LineObject);

  if (OBJECT_INVERTED(oinfo)) 
    workgc = default_reverse_gc;
   else
    workgc = default_clip_gc;
  theRectangle.x = report->x;
  theRectangle.y = report->y;
  theRectangle.width = report->width;
  theRectangle.height = report->height;
  XSetClipRectangles(display,workgc,0,0,&theRectangle,1,YXBanded);
  XDrawLine(display,oinfo->drawwin,workgc,linfo->x1,linfo->y1,linfo->x2,
	    linfo->y2);
  if (workgc==default_reverse_gc)
    XSetClipMask(display,workgc,None);
}

void _GL_VA_Init_LineObject(list)
GL_DataList list;
{
  GL_LineObjectInfo *linfo;

  _GL_VA_Init_Object(list);
  linfo = XMALLOC(GL_LineObjectInfo);

  linfo->x1=0;
  linfo->y1=0;
  linfo->x2=0;
  linfo->y2=0;
  linfo->haveline = 0;
  
  GL_Add_To_DataList(list,_GL_VA_Init_LineObject,linfo);
}

void _GL_VA_Handle_LineObject(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_LineObjectInfo *linfo;
  
  linfo = (GL_LineObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_LineObject);

  switch(modifier) 
    {
    case GLLO_LinePos : 
      linfo->x1 = va_arg(*pvar,int);
      linfo->y1 = va_arg(*pvar,int);
      linfo->x2 = va_arg(*pvar,int);
      linfo->y2 = va_arg(*pvar,int);
      linfo->haveline = 1;
      break;
      default : _GL_VA_Handle_Object(list,modifier,pvar);
    }
}

void _GL_VA_Execute_LineObject(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;
  GL_LineObjectInfo *linfo;

  _GL_VA_Execute_Object(list);

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);
  linfo = (GL_LineObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_LineObject);
  
  oinfo->draw = _DrawLineObject;

#define ABS(x) (x<0?-(x):x)
  if (linfo->haveline) {
    oinfo->xmin = MIN(linfo->x1,linfo->x2);
    oinfo->ymin = MIN(linfo->y1,linfo->y2);
    oinfo->width = ABS(linfo->x2-linfo->x1)+1;
    oinfo->height = ABS(linfo->y2-linfo->y1)+1;
    oinfo->x = (linfo->x1+linfo->x2)/2;
    oinfo->y = (linfo->y1+linfo->y2)/2;
  }

  GL_Add_To_DataList(oinfo->thelist,_GL_VA_Init_LineObject,linfo);
}

