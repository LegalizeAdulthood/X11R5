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

#include "X11/gl/fillpolyobj.h"

static void _DrawFilledPolygonObject(report,oinfo)
XExposeEvent *report;
GL_ObjectInfo *oinfo;
{
  GL_PolygonObjectInfo *pinfo;
  GL_FilledPolygonObjectInfo *finfo;
  GC workgc;
  XRectangle theRectangle;

  pinfo = (GL_PolygonObjectInfo *)GL_Search_DataList(oinfo->thelist,
				  _GL_VA_Init_PolygonObject);
  finfo = (GL_FilledPolygonObjectInfo *)GL_Search_DataList(oinfo->thelist,
				  _GL_VA_Init_FilledPolygonObject);

  if (OBJECT_INVERTED(oinfo))
    workgc = default_reverse_gc;
  else
    workgc = default_clip_gc;
  theRectangle.x = report->x;
  theRectangle.y = report->y;
  theRectangle.width = report->width;
  theRectangle.height = report->height;
  XSetClipRectangles(display,workgc,0,0,&theRectangle,1,YXBanded);
  XFillPolygon(display,oinfo->drawwin,workgc,pinfo->points,
	       pinfo->numpoints,finfo->shape,pinfo->mode);
  if (workgc==default_reverse_gc)
    XSetClipMask(display,workgc,None);
}

void _GL_VA_Init_FilledPolygonObject(list)
GL_DataList list;
{
  GL_FilledPolygonObjectInfo *finfo;

  _GL_VA_Init_PolygonObject(list);

  finfo = XMALLOC(GL_FilledPolygonObjectInfo);

  finfo->shape = Complex;

  GL_Add_To_DataList(list,_GL_VA_Init_FilledPolygonObject,finfo);
}

void _GL_VA_Handle_FilledPolygonObject(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_FilledPolygonObjectInfo *finfo;
  
  finfo = (GL_FilledPolygonObjectInfo *)GL_Search_DataList(list,
				        _GL_VA_Init_FilledPolygonObject);

  switch(modifier)
    {
    case GLFPO_PolygonShape :
      finfo->shape = va_arg(*pvar,int);
      break;
    default: _GL_VA_Handle_PolygonObject(list,modifier,pvar);
    }
}

void _GL_VA_Execute_FilledPolygonObject(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;
  GL_PolygonObjectInfo *pinfo;
  GL_FilledPolygonObjectInfo *finfo;

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,
					      _GL_VA_Init_Object);
  pinfo = (GL_PolygonObjectInfo *)GL_Search_DataList(list,
			         _GL_VA_Init_PolygonObject);
  finfo = (GL_FilledPolygonObjectInfo *)GL_Search_DataList(list,
				 _GL_VA_Init_FilledPolygonObject);
  
  pinfo->closed=0;

  _GL_VA_Execute_PolygonObject(list);

  oinfo->draw = _DrawFilledPolygonObject;
  GL_Add_To_DataList(oinfo->thelist,_GL_VA_Init_FilledPolygonObject,finfo);
}

  
