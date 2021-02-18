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


/*	glb_radio.c
	This module provides Macintosh-like radio buttons for GraphLib.  These
	are 8-state buttons; they can be either active or inactive.
	
	Project Manager: Eric Anderson (ea08@andrew.cmu.edu)

	Revision History:
	Date     Who	Modification
	-------- ---	------------------------------
	09-24-90 jth	Created
	11-01-90 jth    Added Radio Group support.  Not working yet.

*/

#include "X11/gl/buttons.h"
#include "rad_bitmaps"

Pixmap _GL_RadMap_0a,_GL_RadMap_0i,_GL_RadMap_2,_GL_RadMask;
int _GL_RadButton_Flag=0;

void _GL_RadioBtn_Init()
{
  _GL_RadMap_0a=XCreateBitmapFromData(display,RootWindow(display,screen),
    rad_a_bits,rad_a_width,rad_a_height);
  _GL_RadMap_0i=XCreateBitmapFromData(display,RootWindow(display,screen),
    rad_i_bits,rad_i_width,rad_i_height);
  _GL_RadMap_2=XCreateBitmapFromData(display,RootWindow(display,screen),
    rad_2_bits,rad_2_width,rad_2_height);
  _GL_RadMask=XCreateBitmapFromData(display,RootWindow(display,screen),
    rad_mask_bits,rad_mask_width,rad_mask_height);
  _GL_RadButton_Flag=1;
}


void _GL_RadioBtn_0(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int o,p;

  o=(int)((info->height-14)/2.0);if (o<0) o=0;
  p=(int)((info->height-default_font->max_bounds.ascent-default_font->max_bounds.ascent)/2.0); if (p<0) p=0;
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XSetBackground(display,_GL_ButtonGC,info->bgc);
  if (info->flags & GL_Bchecked)
    XSetStipple(display,_GL_ButtonGC,_GL_RadMap_0a);
  else
    XSetStipple(display,_GL_ButtonGC,_GL_RadMap_0i);
  XSetFillStyle(display,_GL_ButtonGC,FillOpaqueStippled);
  XSetClipMask(display,_GL_ButtonGC,_GL_RadMask);
  XSetClipOrigin(display,_GL_ButtonGC,0,o);
  XSetTSOrigin(display,_GL_ButtonGC,0,o);
  XFillRectangle(display,info->winid,_GL_ButtonGC,0,o,14,14);
  XSetClipMask(display,_GL_ButtonGC,None);
  XSetFillStyle(display,_GL_ButtonGC,FillSolid);
#if 0
  XSetLineAttributes(display,_GL_ButtonGC,1,LineSolid,CapButt,JoinMiter);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,o,13,13,0,360*64);
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XDrawArc(display,info->winid,_GL_ButtonGC,1,o+1,11,11,0,360*64);
  XFillArc(display,info->winid,_GL_ButtonGC,1,o+1,11,11,0,360*64);
  if (info->flags & GL_Bchecked)
    {
      XSetForeground(display,_GL_ButtonGC,info->fgc);
      XDrawArc(display,info->winid,_GL_ButtonGC,3,o+3,7,7,0,360*64);
      XFillArc(display,info->winid,_GL_ButtonGC,3,o+3,7,7,0,360*64);
    }
#endif
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,20,default_font->max_bounds.ascent+p,
	      info->name,strlen(info->name));
}

void _GL_RadioBtn_2(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_ButtonInfo *info=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  int o,p;

  o=(int)((info->height-14)/2.0);if (o<0) o=0;
  p=(int)((info->height-default_font->max_bounds.ascent-default_font->max_bounds.ascent)/2.0); if (p<0) p=0;
  XSetForeground(display,_GL_ButtonGC,info->fgc);
  XSetBackground(display,_GL_ButtonGC,info->bgc);
  XSetStipple(display,_GL_ButtonGC,_GL_RadMap_2);
  XSetFillStyle(display,_GL_ButtonGC,FillOpaqueStippled);
  XSetClipMask(display,_GL_ButtonGC,_GL_RadMask);
  XFillRectangle(display,info->winid,_GL_ButtonGC,0,o,14,14);
  XSetClipMask(display,_GL_ButtonGC,None);
  XSetFillStyle(display,_GL_ButtonGC,FillSolid);
#if 0
  XSetLineAttributes(display,_GL_ButtonGC,2,LineSolid,CapButt,JoinMiter);
  XDrawArc(display,info->winid,_GL_ButtonGC,0,o,13,13,0,360*64);
  XSetForeground(display,_GL_ButtonGC,info->bgc);
  XSetLineAttributes(display,_GL_ButtonGC,1,LineSolid,CapButt,JoinMiter);
  XDrawArc(display,info->winid,_GL_ButtonGC,2,o+2,9,9,0,360*64);
  XFillArc(display,info->winid,_GL_ButtonGC,2,o+2,9,9,0,360*64);
#endif
  XSetForeground(display,_GL_ButtonGC,info->txc);
  XDrawString(display,info->winid,_GL_ButtonGC,20,default_font->max_bounds.ascent+p,
	      info->name,strlen(info->name));
}


void _GL_VA_InitB_RadioBtn(list)
GL_DataList list;
{
  GL_ButtonInfo *btninfo;

  if (!_GL_RadButton_Flag) _GL_RadioBtn_Init();
  _GL_VA_Init_Button(list);
  btninfo=(GL_ButtonInfo *)GL_Search_DataList(list,_GL_VA_Init_Button);
  btninfo->draw[0]=_GL_RadioBtn_0;
  btninfo->draw[1]=_GL_RadioBtn_0;
  btninfo->draw[2]=_GL_RadioBtn_2;
  btninfo->draw[3]=_GL_RadioBtn_0;
  btninfo->flags=GL_Bautocheck;
}

void _GL_VA_ExecB_RadioBtn(list)
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
      wininfo->width=XTextWidth(default_font,wininfo->winname,strlen(wininfo->winname))+30;

  if (wininfo->height<0)
      wininfo->height=default_font->max_bounds.ascent+default_font->max_bounds.descent+2;
  if (wininfo->height<14) wininfo->height=14;

  _GL_VA_Execute_Button(list);
}
