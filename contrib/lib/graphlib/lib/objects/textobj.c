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

#include "X11/gl/textobj.h"
#include <stdio.h>

static void recalc_string_size(tinfo,oinfo)
GL_TextObjectInfo *tinfo;
GL_ObjectInfo *oinfo;
{
  XCharStruct overall_return;
  int direction_return,font_ascent_return,font_descent_return;
  int x1,y1,x2,y2;

  XTextExtents(default_font,tinfo->data,tinfo->strlength,&direction_return,
	       &font_ascent_return,&font_descent_return,
	       &overall_return);
  tinfo->ascent=font_ascent_return;
  tinfo->descent=font_descent_return;
  oinfo->width=XTextWidth(default_font,tinfo->data,tinfo->strlength);
  oinfo->height=font_ascent_return+font_descent_return;
  x1 = oinfo->x;
  y1 = oinfo->y;
  x2 = oinfo->x+oinfo->width;
  y2 = oinfo->y+oinfo->height;
  oinfo->xmin=x1;
  oinfo->ymin=y1;
  oinfo->width=x2-x1+1;
  oinfo->height=y2-y1+1;
}

static void _DrawTextObject(report,oinfo)
XExposeEvent *report;
GL_ObjectInfo *oinfo;
{
  GL_TextObjectInfo *tinfo;

  tinfo = (GL_TextObjectInfo *)GL_Search_DataList(oinfo->thelist,
						  _GL_VA_Init_TextObject);

  if (tinfo->data!=NULL)
    XDrawString(display,oinfo->drawwin,
		(oinfo->flags&GLO_MASK_INVERTED)?default_reverse_gc:default_gc,
		oinfo->x,oinfo->y+tinfo->ascent,tinfo->data,tinfo->strlength);
}

static void change_to_null(tinfo,oinfo)
GL_TextObjectInfo *tinfo;
GL_ObjectInfo *oinfo;
{
  tinfo->data=NULL;
  tinfo->ascent=0;
  tinfo->descent=0;
  tinfo->strlength=0;
  XClearArea(display,oinfo->drawwin,oinfo->xmin,oinfo->ymin,
	     oinfo->width,oinfo->height,True);
  oinfo->xmin=oinfo->x;
  oinfo->width=0;
  oinfo->ymin=oinfo->y;
  oinfo->height=0;
} 

static void change_string(tinfo,oinfo,to)
GL_TextObjectInfo *tinfo;
GL_ObjectInfo *oinfo;
char *to;
{
  int oldwidth=oinfo->width;
  int charspassed=0,tostartwidth=0;
  char *loop1,*loop2=to;

  if (tinfo->data!=NULL) {
    for (loop1=tinfo->data;
	 *loop1==*loop2&&*loop1!='\0';
	 loop1++,loop2++,charspassed++)
      ;
    if (loop1!=tinfo->data) 
      tostartwidth = XTextWidth(default_font,tinfo->data,charspassed);
  }
  tinfo->strlength=strlen(to);
  tinfo->data=(char *)xmalloc(tinfo->strlength+1);
  strcpy(tinfo->data,to);
  recalc_string_size(tinfo,oinfo);
  XDrawImageString(display,oinfo->drawwin,default_gc,oinfo->x+tostartwidth,
	      tinfo->ascent+oinfo->y,loop2,tinfo->strlength-charspassed);
  if (oinfo->width<oldwidth)
    XClearArea(display,oinfo->drawwin,oinfo->width+oinfo->x-1,oinfo->y,
	       oldwidth-oinfo->width+1,
	       oinfo->height,False);
}

void Change_TextObjectString(oinfo,to)
GL_ObjectInfo *oinfo;
char *to;
{
  GL_TextObjectInfo *tinfo;

  tinfo = (GL_TextObjectInfo *)GL_Search_DataList(oinfo->thelist,
						  _GL_VA_Init_TextObject);
  if (tinfo->data!=NULL)
    free(tinfo->data);
  if (to==NULL)
    change_to_null(tinfo,oinfo);
  else 
    change_string(tinfo,oinfo,to);
}

void _GL_VA_Init_TextObject(list)
GL_DataList list;
{
  GL_TextObjectInfo *tinfo;

  _GL_VA_Init_Object(list);
  tinfo = (GL_TextObjectInfo *)xmalloc(sizeof(GL_TextObjectInfo));
  tinfo->data=NULL;
  tinfo->strlength=0;
  tinfo->ascent=0;
  tinfo->descent=0;
  GL_Add_To_DataList(list,_GL_VA_Init_TextObject,tinfo);
}

void _GL_VA_Handle_TextObject(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_TextObjectInfo *tinfo;
  char *dummy;

  tinfo = (GL_TextObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_TextObject);

  switch(modifier)
    {
    case GLTO_String : if(tinfo->data) free(tinfo->data);
      dummy = va_arg(*pvar,char *);
      if (dummy!=NULL) {
	tinfo->strlength = strlen(dummy);
	tinfo->data = xmalloc(tinfo->strlength+1);
	strcpy(tinfo->data,dummy);
      }
      break;
    default: 
      _GL_VA_Handle_Object(list,modifier,pvar);
    }
}

void _GL_VA_Execute_TextObject(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;
  GL_TextObjectInfo *tinfo;

  _GL_VA_Execute_Object(list);

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);
  tinfo = (GL_TextObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_TextObject);
  
  oinfo->draw = _DrawTextObject;
  if (tinfo->data!=NULL)
    recalc_string_size(tinfo,oinfo);
  else {
    oinfo->xmin=0;
    oinfo->ymin=0;
    oinfo->width=0;
    oinfo->height=0;
  }
  GL_Add_To_DataList(oinfo->thelist,_GL_VA_Init_TextObject,tinfo);
}
