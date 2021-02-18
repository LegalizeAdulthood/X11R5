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

	glb_next.c
	NeXT-like buttons for GraphLib.  These buttons emulate the ordinary push
	buttons available on the NeXT.

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	03/19/91 jth	Created

*/

#include "X11/gl/buttons.h"

void _GL_NeXTBtn_0(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);

  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XFillRectangle(display,info->winid,_GL_ButtonGC,0,0,info->width,info->height);
  XSetForeground(display,_GL_ButtonGC,WhitePixel(display,screen));
  XDrawLine(display,info->winid,_GL_ButtonGC,0,0,0,info->height-1);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,0,info->width-1,0);
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawLine(display,info->winid,_GL_ButtonGC,info->width-1,0,info->width-1,info->height-1);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,info->height-1,info->width-1,info->height-1);
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,5,default_font->max_bounds.ascent,
	      info->name,strlen(info->name));
}

void _GL_NeXTBtn_2(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);

  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XFillRectangle(display,info->winid,_GL_ButtonGC,0,0,info->width,info->height);
  XSetForeground(display,_GL_ButtonGC,WhitePixel(display,screen));
  XDrawLine(display,info->winid,_GL_ButtonGC,info->width-1,0,info->width-1,info->height-1);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,info->height-1,info->width-1,info->height-1);
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,0,0,info->height-1);
  XDrawLine(display,info->winid,_GL_ButtonGC,0,0,info->width-1,0); 
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,6,default_font->max_bounds.ascent+1,
	      info->name,strlen(info->name));
}

void _GL_VA_InitB_NeXTBtn(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;

  _GL_VA_Init_Button(list);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  btninfo->draw[0]=_GL_NeXTBtn_0;
  btninfo->draw[1]=_GL_NeXTBtn_0;
  btninfo->draw[2]=_GL_NeXTBtn_2;
  btninfo->draw[3]=_GL_NeXTBtn_0;
}
