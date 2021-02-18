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

/*	glb_opensink.c
	This module provides sink-in Open Look-like buttons for GraphLib.  These
	buttons are Mac-Like in shape, with an additional thickness in the
	lower-right portion.  When they are selected, they appear to "sink into"
	the screen, which is accomplished by manipulating which part of the
	border is bold.

	Project Manager: Eric Anderson (ea08@andrew.cmu.edu)

	Revision History:
	Date     Who	Modification
	-------- ---	------------------------------
	09-22-90 jth	Created

*/

#include <stdio.h>

#include "X11/gl/buttons.h"

void _GL_OpenSink_0(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int w=info->width, h=info->height;

  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,0,11,11,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,0,11,11,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,h-12,11,11,180*64,90*64);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,6,0,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,0,w-6,0);
  XSetLineAttributes(display,_GL_ButtonGC,2,LineSolid,CapButt,JoinMiter);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-1,4,w-1,h-3);
  XDrawLine(display,info->winid,_GL_ButtonGC,4,h-1,w-3,h-1);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-9,h-9,7,7,270*64,90*64);
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XSetLineAttributes(display,_GL_ButtonGC,1,LineSolid,CapButt,JoinMiter);
  XDrawLine(display,info->winid,_GL_ButtonGC,1,4,1,h-4);
  XDrawLine(display,info->winid,_GL_ButtonGC,4,1,w-4,1);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-3,4,w-3,h-4);
  XDrawLine(display,info->winid,_GL_ButtonGC,4,h-3,w-4,h-3);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,1,7,7,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-10,1,7,7,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,h-10,7,7,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-10,h-10,7,7,270*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,1,7,7,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-10,1,7,7,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,h-10,7,7,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-10,h-10,7,7,270*64,90*64);
  XFillRectangle(display,info->winid,_GL_ButtonGC,1,4,w-4,h-8);
  XFillRectangle(display,info->winid,_GL_ButtonGC,4,1,w-8,h-4);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,6,default_font->max_bounds.
	      ascent+1,info->name,strlen(info->name));
}

void _GL_OpenSink_2(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int w=info->width, h=info->height;

  if (report->type != Expose) XClearWindow(display,info->winid);
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,1,11,11,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,h-12,11,11,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-12,h-12,11,11,270*64,90*64);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-1,6,w-1,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,h-1,w-6,h-1);
/*XSetLineAttributes(display,_GL_ButtonGC,2,LineSolid,CapButt,JoinMiter); */
  XDrawArc(display,info->winid,_GL_ButtonGC,1,1,11,11,90*64,90*64);
  XDrawLine(display,info->winid,_GL_ButtonGC,1,6,1,h-6);
  XDrawLine(display,info->winid,_GL_ButtonGC,6,1,w-6,1);
  XSetForeground(display,_GL_ButtonGC,info->bgc);
/*XSetLineAttributes(display,_GL_ButtonGC,1,LineSolid,CapButt,JoinMiter); */
  XDrawLine(display,info->winid,_GL_ButtonGC,2,5,2,h-3);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,2,w-3,2);
  XDrawLine(display,info->winid,_GL_ButtonGC,w-2,5,w-2,h-3);
  XDrawLine(display,info->winid,_GL_ButtonGC,5,h-2,w-3,h-2);
  XDrawArc(display,info->winid,_GL_ButtonGC,2,2,7,7,90*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-9,2,7,7,0,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,2,h-9,7,7,180*64,90*64);
  XDrawArc(display,info->winid,_GL_ButtonGC,w-9,h-9,7,7,270*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,2,2,7,7,90*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-9,2,7,7,0,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,2,h-9,7,7,180*64,90*64);
  XFillArc(display,info->winid,_GL_ButtonGC,w-9,h-9,7,7,270*64,90*64);
  XFillRectangle(display,info->winid,_GL_ButtonGC,2,5,w-4,h-8);
  XFillRectangle(display,info->winid,_GL_ButtonGC,5,2,w-8,h-4);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,7,default_font->max_bounds.
	      ascent+2,info->name,strlen(info->name));
}

void _GL_VA_InitB_OpenSink(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;
  GL_WindowInfo *wininfo;
  
  _GL_VA_Init_Button(list);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  btninfo->draw[0]=_GL_OpenSink_0;
  btninfo->draw[1]=_GL_OpenSink_0;
  btninfo->draw[2]=_GL_OpenSink_2;
  btninfo->draw[3]=_GL_OpenSink_0;
}

void _GL_VA_ExecB_OpenSink(list)
GL_DataList list;
{
  GL_WindowInfo *wininfo;
  
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);

  if (!wininfo->winname)
    {
      wininfo->winname=(char *)malloc(strlen("Button")+1);
      strcpy(wininfo->winname,"Button");
    }

  if (wininfo->width<0)
      wininfo->width=XTextWidth(default_font,wininfo->winname,strlen(wininfo->winname))+12;

  if (wininfo->height<0)
      wininfo->height=default_font->max_bounds.ascent+default_font->max_bounds.descent+5;

  _GL_VA_Execute_Button(list);
}
