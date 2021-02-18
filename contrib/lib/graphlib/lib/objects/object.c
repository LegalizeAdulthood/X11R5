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

#include  "X11/gl/object.h"
#include <stdio.h>

void GL_GetObjectXY(theobject,x,y)
GL_DataList theobject;
int *x,*y;
{
}

GL_ObjectMgrInfo *GL_CreateObjectManager()
{
  GL_ObjectMgrInfo *work;

  work = (GL_ObjectMgrInfo *)xmalloc(sizeof(GL_ObjectMgrInfo));
  work->Objects = GL_Init_DataList();
  work->objcount = 1;
  return work;
}

void GL_ExposeEventObjectManager(report,list)
XExposeEvent *report;
GL_DataList list;
{
  GL_DataList work;
  GL_ObjectMgrInfo *data;
  GL_ObjectInfo *stuff;

  data = (GL_ObjectMgrInfo *)GL_Search_DataList(list,GL_CreateObjectManager);
  work = data->Objects;

  for(;work!=NULL;work=work->next)
    if (work->data!=NULL) {
      stuff = (GL_ObjectInfo *)work->data;
      if ((MAX(stuff->xmin,report->x)<=
	   MIN(stuff->xmin+stuff->width,report->x+report->width))
	  &&
	  (MAX(stuff->ymin,report->y)<=
	   MIN(stuff->ymin+stuff->height,report->y+report->height))
	  )
	/* The Rectangles intersect in a non null fashion */
	stuff->draw(report,stuff);
    }
}

void GL_InstallObject(oinfo,window)
GL_ObjectInfo *oinfo;
Window window;
{
  GL_ObjectMgrInfo *exposemgr;
  GL_DataList list;

  list = GL_GetDataList(window);

  if (GL_Key_In_DataList(list,GL_CreateObjectManager)) 
    exposemgr = (GL_ObjectMgrInfo *)GL_Search_DataList(list,
						       GL_CreateObjectManager);
  else {
    exposemgr = GL_CreateObjectManager();
    GL_Add_To_DataList(list,GL_CreateObjectManager,exposemgr);
    GL_AddFuncPtr(window,
		  Expose, GL_ExposeEventObjectManager,
		  GL_Done);
  }
  oinfo->drawwin = window;
  GL_Add_To_DataList(exposemgr->Objects,exposemgr->objcount,oinfo);
  exposemgr->objcount++;
}


/*
void DrawObject(theobject)
GL_DataList theobject;
{
  GL_ObjectInfo *oinfo;

  oinfo = GL_Search_DataList(theobject, _GL_VA_Init_Object);
  if (oinfo->draw==NULL) {
    fprintf(stderr,"Warning: Object had no draw function\n");
    return;
  }
  if (oinfo->drawwin==0) {
    fprintf(stderr,"Warning: Object not installed in window\n");
    return;
  }
  oinfo->draw(NULL);
}
*/

void _GL_VA_Init_Object(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;

  oinfo = (GL_ObjectInfo *)xmalloc(sizeof(GL_ObjectInfo));

  oinfo->thelist=GL_Init_DataList();
  oinfo->x=0;
  oinfo->y=0;
  oinfo->xmin=0;
  oinfo->width=0;
  oinfo->ymin=0;
  oinfo->height=0;
  oinfo->draw=NULL;
  oinfo->install=NULL;
  oinfo->invert=NULL;
  oinfo->drawwin=0;
  oinfo->flags=0;
  GL_Add_To_DataList(list,_GL_VA_Init_Object,oinfo);
}

void _GL_VA_Handle_Object(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_ObjectInfo *oinfo;
  
  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);

  switch(modifier) 
    {
    case GLO_X : oinfo->x = va_arg(*pvar,int);
      break;
    case GLO_Y : oinfo->y = va_arg(*pvar,int);
      break;
    case GLO_Window : oinfo->drawwin = va_arg(*pvar, Window);
      break;
    case GLO_Inverted: oinfo->flags |= GLO_MASK_INVERTED;
      break;
      default :
	fprintf(stderr,"An unknown modifier was passed to\n");
      fprintf(stderr,"_GL_VA_Handle_Object.  the modifier was %d\n",modifier);
      fprintf(stderr,"This signifies a bug.  So the codes gonna bug-out.\n");
      exit(-1);
    }
}

void _GL_VA_Execute_Object(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;
  
  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);

  if (oinfo->drawwin!=0)
    GL_InstallObject(oinfo,oinfo->drawwin);
}

void _GL_VA_Destroy_Object(list)
GL_DataList list;
{
}

GLPointer _GL_VA_Return_Object(list)
GL_DataList list;
{
  GL_ObjectInfo *oinfo;

  oinfo = (GL_ObjectInfo *)GL_Search_DataList(list,_GL_VA_Init_Object);
  return (GLPointer)oinfo;
}

void GL_InvertObject(oinfo,state)
GL_ObjectInfo *oinfo;
int state;
{
  if (((oinfo->flags&GLO_MASK_INVERTED)==0&&state==0)||
      ((oinfo->flags&GLO_MASK_INVERTED)!=0&&state!=0))
    return;
  if (state==0)
    oinfo->flags &= ~GLO_MASK_INVERTED;
  else
    oinfo->flags |= GLO_MASK_INVERTED;

  if (oinfo->invert!=NULL)
    oinfo->invert(oinfo);
}
		 
