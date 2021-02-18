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

#include <stdio.h>
#include <X11/gl/eventmgr.h>
#include <X11/gl/buttons.h>
#include <X11/gl/menus.h>
#include <X11/gl/miscutils.h>

Window win,qdialog, menuwin, testbtn;
GC gc;

void QuitExposeProc()
{
  XDrawString(display,qdialog,gc,10,10+default_font->max_bounds.ascent,
    "Do you really want to quit?",27);
}

void QuitProc()
{
  XCloseDisplay(display);
  exit(0);
}

void NoQuitProc()
{
  XUnmapWindow(display,qdialog);
}

void QuitRequest()
{
  XMapSubwindows(display,qdialog);
  XMapRaised(display,qdialog);
}

void MainMenuProc(report)
XEvent *report;
{
  GL_HandleMenu(report,menuwin);
}

main(argc,argv)
int argc;
char **argv;
{
  unsigned long texcolor, wincolor;

  GL_InitGraphics(argc,argv,"GLTest",GL_Window_Help);
  texcolor=GL_GetColor("Blue","Black");
  win=(Window)GL_Create(GL_Window,
    GL_WinName, "GraphLib Tester",
    GL_WinWidth, 300,
    GL_WinHeight, 120,
    ButtonPress, MainMenuProc,
    GL_Done);
  GL_SetWinColor(win,"LightGrey",default_grey);
  testbtn=(Window)GL_Create(GL_AthenaBtn,
    GL_WinParent, win,
    GL_WinName, "Test",
    GL_WinX, 5,
    GL_WinY, 5,
    GL_Done);
  menuwin = (Window)GL_Create(GL_Menu,
    GL_MenuShadow, 3,
    GL_ComplexMenuItem,
      GL_MItemName, "Menu Tester",
      GL_MItemTitle,
      GL_MItemSeparator,
      GL_Done,
    GL_MenuItem, "Dummy Item", NULL,			      
    GL_MenuItem, "Quit", QuitRequest,
    GL_MenuWindow, testbtn,
    GL_Done);
  GL_AttachButtonMenu(menuwin, testbtn);
  qdialog=(Window)GL_Create(GL_Window,
    Expose, QuitExposeProc,
    GL_WinName, "Quit Confirmation",
    GL_WinX, (DisplayWidth(display,screen) - 214) / 2,
    GL_WinY, (DisplayHeight(display,screen) - 74) / 2,
    GL_WinWidth, 210,
    GL_WinHeight, 70,
    GL_OverrideRedirect, 1,
    GL_WindowBorderWidth, 2,
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
  GL_GetGC(win,&gc,GL_Done);
  GL_MainLoop(win);
}
