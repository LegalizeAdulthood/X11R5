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
#include "X11/gl/label.h"
#include "X11/gl/textobj.h"

void GL_Label_Help()
{
  GL_Window_Help();
}

void Change_Label(window,to)
{
  GL_DataList list;
  GL_LabelInfo *linfo;

  list = GL_GetDataList(window);
  linfo = (GL_LabelInfo *)GL_Search_DataList(list,
					     _GL_VA_Init_Label);

  Change_TextObjectString(linfo->textobject,to);
}

void _GL_VA_Init_Label(list)
GL_DataList list;
{
  GL_LabelInfo *linfo;

  _GL_VA_Init_Window(list);
  
  linfo = (GL_LabelInfo *)xmalloc(sizeof(GL_LabelInfo));
  linfo->data = NULL;
  linfo->leftbuffer = 2;
  GL_Add_To_DataList(list,_GL_VA_Init_Label,linfo);
}

void _GL_VA_Handle_Label(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  GL_LabelInfo *linfo;
  char *newdata;

  linfo = (GL_LabelInfo *)GL_Search_DataList(list,_GL_VA_Init_Label);
  
  switch (modifier) {
  case GLL_InitialLabel : 
    if (linfo->data) free(linfo->data);
    newdata = va_arg(*pvar,char *);
    linfo->data = (char *)xmalloc(strlen(newdata)+1);
    strcpy(linfo->data,newdata);
    break;
  case GLL_LeftBuffer :
    linfo->leftbuffer = va_arg(*pvar,int);
    break;
  case GLL_Font :
    linfo->font = va_arg(*pvar,XFontStruct *);
    break;
  default : _GL_VA_Handle_Window(list,modifier,pvar);
  }
}

void _GL_VA_Execute_Label(list)
GL_DataList list;
{
  GL_LabelInfo *linfo;
  GL_EventMgrInfo *eventmgrinfo;
  GLPointer textobject;

  _GL_VA_Execute_Window(list);

  eventmgrinfo = (GL_EventMgrInfo *)GL_Search_DataList(list,
						       _GL_VA_Init_EventMgr);
  linfo = (GL_LabelInfo *)GL_Search_DataList(list,_GL_VA_Init_Label);
  
  textobject = GL_Create(GL_TextObject,
			 GLTO_String, linfo->data,
			 GLO_X, linfo->leftbuffer,
			 GL_Done);
  GL_InstallObject(textobject,eventmgrinfo->winid);
  linfo->textobject = textobject;
  GL_AddWindowAssocInfo(eventmgrinfo->winid,_GL_VA_Init_Label,
			(GLPointer)linfo);
}
