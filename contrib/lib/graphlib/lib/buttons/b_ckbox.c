/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Jeffrey Hutzelman, Eric Anderson 
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

/*	GraphLib: An X Windows Programming Library
	Developed at CMU Fall 1990 - Spring 1991

	Eric Anderson (ea08@andrew.cmu.edu, eanders@sura.net)
	Jeffrey Hutzelman (jh4o@andrew.cmu.edu, jhutz@drycas.club.cc.cmu.edu)

	glb_ckbox.c
	Mac-Like checkboxes for GraphLib.  This button type acts like a Mac checkbox.
	The button may be toggled by clicking on the box or text.  Default width is
	expanded by 20 pixels to make room for the box.  The height of the window is forced
	to be at least 15 pixels.  GL_Bautocheck is set by default for automatic action;
	be sure to set it if you specify flags.  This button type is suitable for inclusion
	in radio groups.

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	03-17-91 jth	Created

*/

#include "X11/gl/buttons.h"
#include <malloc.h>

void _GL_CheckBox_0(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int o,p;

  o=(int)((info->height-15)/2.0);if (o<0) o=0;
  p=(int)((info->height-default_font->max_bounds.ascent-default_font->max_bounds.ascent)/2.0); if (p<0) p=0;
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o,0,o+14);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o,14,o);
  XDrawLine(display,info->winid,_GL_ButtonGC,14,o,14,o+15);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o+14,14,o+14);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,20,default_font->max_bounds.ascent+p,info->name,strlen(info->name));
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XFillRectangle(display,info->winid,_GL_ButtonGC,1,o+1,13,13);
  if (info->flags & GL_Bchecked)
    {
      XSetForeground(display,_GL_ButtonGC,info->fgc);
      XDrawLine(display,info->winid,_GL_ButtonGC,0,o,14,o+14);
      XDrawLine(display,info->winid,_GL_ButtonGC,0,o+14,14,o);
    }
}

void _GL_CheckBox_2(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int o,p;

  o=(int)((info->height-15)/2.0);if (o<0) o=0;
  p=(int)((info->height-default_font->max_bounds.ascent-default_font->max_bounds.ascent)/2.0); if (p<0) p=0;
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o,0,o+14);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o,14,o);
  XDrawLine(display,info->winid,_GL_ButtonGC,14,o,14,o+15);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,o+14,14,o+14);
  XDrawLine(display,info->winid,_GL_ButtonGC,1,o+1,1,o+13);
  XDrawLine(display,info->winid,_GL_ButtonGC,1,o+1,13,o+1);
  XDrawLine(display,info->winid,_GL_ButtonGC,13,o+1,13,o+14);
  XDrawLine(display,info->winid,_GL_ButtonGC,1,o+13,13,o+13);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,20,default_font->max_bounds.ascent+p,
    info->name,strlen(info->name));
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XFillRectangle(display,info->winid,_GL_ButtonGC,2,o+2,11,11);
}

void _GL_VA_InitB_CheckBox(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;

  _GL_VA_Init_Button(list);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  btninfo->draw[0]=_GL_CheckBox_0;
  btninfo->draw[1]=_GL_CheckBox_0;
  btninfo->draw[2]=_GL_CheckBox_2;
  btninfo->draw[3]=_GL_CheckBox_0;
  btninfo->flags=GL_Bautocheck;
}

void _GL_VA_ExecB_CheckBox(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);

  if (!wininfo->winname)
    {
      wininfo->winname=malloc(strlen("Button")+1);
      strcpy(wininfo->winname,"Button");
    }

  if (wininfo->width<0)
      wininfo->width=XTextWidth(default_font,wininfo->winname,strlen(wininfo->winname))+30;

  if (wininfo->height<0)
      wininfo->height=default_font->max_bounds.ascent+default_font->max_bounds.descent+2;
  if (wininfo->height<15) wininfo->height=15;

  _GL_VA_Execute_Button(list);
}
