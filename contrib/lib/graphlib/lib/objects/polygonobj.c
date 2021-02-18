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
#include "X11/gl/polygonobj.h"

static void _DrawPolygonObject(report,oinfo)
XExposeEvent *report;
GL_ObjectInfo *oinfo;
{
  GL_PolygonObjectInfo *pinfo;
  GC workgc;
  XRectangle theRectangle;

  pinfo = (GL_PolygonObjectInfo *)GL_Search_DataList(oinfo->thelist,
				  _GL_VA_Init_PolygonObject);
  
  if (OBJECT_INVERTED(oinfo))
    workgc = default_reverse_gc;
  else
    workgc = default_clip_gc;
  theRectangle.x = report->x;
  theRectangle.y = report->y;
  theRectangle.width = report->width;
  theRectangle.height = report->height;
  XSetClipRectangles(display,workgc,0,0,&theRectangle,1,YXBanded);
  XDrawLines(display,oinfo->drawwin,workgc,pinfo->points,pinfo->numpoints,
	     pinfo->mode);
  if (workgc==default_reverse_gc)
    XSetClipMask(display,workgc,None);
}

void _GL_VA_Init_PolygonObject(list)
GL_DataList list;
{
  GL_PolygonObjectInfo *pinfo;

  _GL_VA_Init_Object(list);
  pinfo = XMALLOC(GL_PolygonObjectInfo);

  pinfo->numpoints= 0;
  pinfo->mode= CoordModeOrigin;
  pinfo->closed= 0;
  pinfo->points= NULL;

  GL_Add_To_DataList(list,_GL_VA_Init_PolygonObject,pinfo);
}

void _GL_VA_Handle_PolygonObject(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_PolygonObjectInfo *pinfo;
  int loop;

  pinfo = (GL_PolygonObjectInfo *)GL_Search_DataList(list,
						     _GL_VA_Init_PolygonObject);
  
  switch(modifier)
    {
    case GLPO_NumPoints: pinfo->numpoints = va_arg(*pvar,int);
      if (pinfo->numpoints<=1) 
	fprintf(stderr,"Warning %d points is meaningless.\n",pinfo->numpoints);
      else {
	if (pinfo->points)
	  free(pinfo->points);
	pinfo->points=(XPoint *)xmalloc(sizeof(XPoint)*pinfo->numpoints);
      }
      break;
    case GLPO_Closed: pinfo->closed = 1;
      break;
    case GLPO_CoordModePrevious: pinfo->mode = CoordModePrevious;
      break;
    case GLPO_Points : 
      if (pinfo->points==NULL) {
	fprintf(stderr,"Number of points not specified before attempting\n");
	fprintf(stderr,"To enter points in polygon object.\n");
	exit(-1);
      }
      for(loop=0;loop<pinfo->numpoints;loop++) {
	pinfo->points[loop].x = (short)va_arg(*pvar,int);
	pinfo->points[loop].y = (short)va_arg(*pvar,int);
      }
      break;
      default : _GL_VA_Handle_Object(list,modifier,pvar);
    }
}

void _GL_VA_Execute_PolygonObject(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;
  GL_PolygonObjectInfo *pinfo;
  XPoint *workcopy;

  _GL_VA_Execute_Object(list);

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,
					      _GL_VA_Init_Object);
  pinfo = (GL_PolygonObjectInfo *)GL_Search_DataList(list,
			         _GL_VA_Init_PolygonObject);

  oinfo->draw = _DrawPolygonObject;

  if (pinfo->closed&&pinfo->points!=NULL)
    if (pinfo->points[0].x!=pinfo->points[pinfo->numpoints-1].x ||
	pinfo->points[0].y!=pinfo->points[pinfo->numpoints-1].y) {
      workcopy = (XPoint *)xmalloc(sizeof(XPoint)*(pinfo->numpoints+1));
      memcpy(workcopy,pinfo->points,sizeof(XPoint)*pinfo->numpoints);
      workcopy[pinfo->numpoints]=workcopy[0];
      free(pinfo->points);
      pinfo->points=workcopy;
      pinfo->numpoints++;
    }
  
  GL_Add_To_DataList(oinfo->thelist,_GL_VA_Init_PolygonObject,pinfo);
}

