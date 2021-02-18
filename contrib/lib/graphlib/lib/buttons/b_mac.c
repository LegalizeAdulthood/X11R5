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

	glb_mac.c
	Mac-like button subclass.  Object type GL_OBJ_MacButton.
	Emulates Macintosh RoundRect buttons.  If the GL_Bchecked
	flag is set, emulates a bold button (as if default).  [Not implemented yet]

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	03-18-91 jth	Created from old Mac draw code

*/

#include "X11/gl/buttons.h"

void _GL_MacButton_0(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int w=info->width;
  int h=info->height;

  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,0,11,11,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,0,11,11,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,h-12,11,11,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,h-12,11,11,270*64,90*64);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,5,0,h-5);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,0,w-5,0);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-1,5,w-1,h-5);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,h-1,w-5,h-1);
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,1,9,9,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-11,1,9,9,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,h-11,9,9,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-11,h-11,9,9,270*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,1,9,9,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-11,1,9,9,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,h-11,9,9,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-11,h-11,9,9,270*64,90*64);
/*  XDrawArc(display,info->winid,_GL_ButtonGC,1,1,11,11,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-13,1,11,11,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,h-13,11,11,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-13,h-13,11,11,270*64,90*64); */
/*  XFillArc(display,info->winid,_GL_ButtonGC,1,1,11,11,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-13,1,11,11,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,h-13,11,11,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-13,h-13,11,11,270*64,90*64);*/
/*  XDrawLine(display,info->winid,_GL_ButtonGC,1,6,1,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,1,w-6,1);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-2,6,w-2,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,h-2,w-6,h-2); */
  XFillRectangle(display,info->winid,_GL_ButtonGC,1,6,w-2,h-12);
  XFillRectangle(display,info->winid,_GL_ButtonGC,6,1,w-12,h-2);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,5,default_font->max_bounds.
	      ascent+2,info->name,strlen(info->name));
}

void _GL_MacButton_2(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int w=info->width;
  int h=info->height;

  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,0,11,11,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,0,11,11,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,h-12,11,11,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,h-12,11,11,270*64,90*64);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,5,0,h-5);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,0,w-5,0);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-1,5,w-1,h-5);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,h-1,w-5,h-1);
  XFillArc(display,info->winid,_GL_ButtonGC,0,0,11,11,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-12,0,11,11,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,0,h-12,11,11,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-12,h-12,11,11,270*64,90*64);
/*  XDrawArc(display,info->winid,_GL_ButtonGC,1,1,9,9,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-13,1,9,9,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,h-13,9,9,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-13,h-13,9,9,270*64,90*64); */
/*  XFillArc(display,info->winid,_GL_ButtonGC,1,1,9,9,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-13,1,9,9,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,h-13,9,9,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-13,h-13,9,9,270*64,90*64);*/
/*  XDrawLine(display,info->winid,_GL_ButtonGC,1,6,1,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,1,w-6,1);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-2,6,w-2,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,h-2,w-6,h-2); */
  XFillRectangle(display,info->winid,_GL_ButtonGC,1,6,w-2,h-12);
  XFillRectangle(display,info->winid,_GL_ButtonGC,6,1,w-12,h-2);
  XSetForeground(display,_GL_ButtonGC,(info->txc==info->fgc)?info->bgc:info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,5,default_font->max_bounds.
	    ascent+2,info->name,strlen(info->name));
}

void _GL_VA_InitB_Mac(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;

  _GL_VA_Init_Button(list);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  btninfo->draw[0]=_GL_MacButton_0;
  btninfo->draw[1]=_GL_MacButton_0;
  btninfo->draw[2]=_GL_MacButton_2;
  btninfo->draw[3]=_GL_MacButton_0;
}

void _GL_VA_ExecB_Mac(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);

  if (wininfo->height<0)
      wininfo->height=default_font->max_bounds.ascent+default_font->max_bounds.descent+4;

  _GL_VA_Execute_Button(list);
}
