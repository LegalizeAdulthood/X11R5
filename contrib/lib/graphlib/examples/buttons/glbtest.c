/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Jeffrey Hutzelman and Eric Anderson
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
#include <X11/gl/eventmgr.h>
#include <X11/gl/window.h>
#ifndef NO_BUTTONS
#include <X11/gl/buttons.h>
#endif
#include <X11/gl/miscutils.h>

#ifndef NO_BUTTONS
Window qdialog;
#endif
Window win;
GC gc;

void GoofyExposeProc(report)
XEvent *report;
{
  XDrawLine(display,report->xexpose.window,gc,0,0,10,10);
}

#ifndef NO_BUTTONS
void QuitExposeProc()
{
  XDrawString(display,qdialog,gc,10,10+default_font->max_bounds.ascent,
    "Do you really want to quit?",27);
}

void NoQuitProc()
{
  XUnmapWindow(display,qdialog);
}

void QuitRequest()
{
  XMapSubwindows(display,qdialog);
  XMapWindow(display,qdialog);
}
#endif

void QuitProc()
{
  XCloseDisplay(display);
  exit(0);
}

main(argc,argv)
int argc;
char **argv;
{
  unsigned long texcolor, wincolor;
#ifndef NO_BUTTONS
  GL_RadioGroup *rgroup;
#endif

  GL_InitGraphics(argc,argv,"GLTest",GL_Window_Help);
  texcolor=GL_GetColor("Blue","Black");
  win=(Window)GL_Create(GL_Window,
    Expose, GoofyExposeProc,
#ifdef NO_BUTTONS
	ButtonPress, QuitProc,
#endif
    GL_WinName, "GraphLib Tester",
    GL_WinWidth, 300,
    GL_WinHeight, 120,
	GL_PrimaryWindow,
    GL_Done);
  GL_SetWinColor(win,"LightGrey",default_grey);
#ifndef NO_BUTTONS
  GL_Create(GL_AthenaBtn,
    GL_BtnAction, QuitRequest,
    GL_WinParent, win,
    GL_WinName, "Quit",
    GL_WinX, 10,
    GL_WinY, 10,
    GL_BtnTX, GL_GetColor("Red","Black"),
    GL_BtnFG, GL_GetColor("Red","Black"),
    GL_Done);
  GL_Create(GL_CheckBox,
    GL_WinParent, win,
    GL_WinName, "CheckBox",
    GL_WinX, 10,
    GL_WinY, 50,
    GL_BtnTX, texcolor,
    GL_Done);
  GL_Create(GL_MacButton,
    GL_WinParent, win,
    GL_WinName, "Mac",
    GL_WinX, 10,
    GL_WinY, 90,
    GL_BtnTX, texcolor,
    GL_Done);
  GL_Create(GL_NeXTButton,
    GL_WinParent, win,
    GL_WinName, "NeXT",
    GL_WinX, 110,
    GL_WinY, 10,
    GL_BtnTX, texcolor,
    GL_BtnBG, GL_GetColor("LightGrey","White"),
    GL_Done);
  GL_Create(GL_OpenButton,
    GL_WinParent, win,
    GL_WinName, "OpenLook",
    GL_WinX, 110,
    GL_WinY, 50,
    GL_BtnTX, texcolor,
    GL_Done);
  GL_Create(GL_OpenSinkButton,
    GL_WinParent, win,
    GL_WinName, "OpenSink",
    GL_WinX, 110,
    GL_WinY, 90,
    GL_BtnTX, texcolor,
    GL_Done);
  rgroup=GL_CreateRadGrp(3,1,0L);
  GL_Create(GL_RadioButton,
    GL_WinParent, win,
    GL_WinName, "Button 1",
    GL_WinX, 210,
    GL_WinY, 10,
    GL_BtnTX, texcolor,
    GL_BtnRadGrp, rgroup,
    GL_Done);
  GL_Create(GL_RadioButton,
    GL_WinParent, win,
    GL_WinName, "Button 2",
    GL_WinX, 210,
    GL_WinY, 50,
    GL_BtnTX, texcolor,
    GL_BtnRadGrp, rgroup,
    GL_Done);
  GL_Create(GL_RadioButton,
    GL_WinParent, win,
    GL_WinName, "Button 3",
    GL_WinX, 210,
    GL_WinY, 90,
    GL_BtnTX, texcolor,
    GL_BtnRadGrp, rgroup,
    GL_Done);
  qdialog=(Window)GL_Create(GL_Window,
    Expose, QuitExposeProc,
    GL_WinName, "Quit Confirmation",
    GL_WinWidth, 210,
    GL_WinHeight, 70,
    GL_Done);
  GL_Create(GL_AthenaBtn,
    GL_BtnAction, QuitProc,
    GL_WinParent, qdialog,
    GL_WinName, "Quit",
    GL_WinX, 30,
    GL_WinY, 40,
    GL_Done);
  GL_Create(GL_AthenaBtn,
    GL_BtnAction, NoQuitProc,
    GL_WinParent, qdialog,
    GL_WinName, "Cancel",
    GL_WinX, 120,
    GL_WinY, 40,
    GL_Done);
#endif
  GL_GetGC(win,&gc,GL_Done);
  GL_MainLoop(win);
}
