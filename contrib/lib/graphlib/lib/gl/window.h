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
#ifndef _GL_WINDOW_H
#define _GL_WINDOW_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include "X11/gl/eventmgr.h"
/* Window owns varargs entries 1100-1999 */
extern XFontStruct *default_font;
extern char *s_classname;
extern int screen;
extern Pixmap default_grey,default_foreground,default_background;
extern GC default_gc, default_clip_gc, default_reverse_gc, default_xor_gc;
/* The default reverse gc has the foreground, background switched
   to facilitate drawing reversed things. */
typedef struct _GL_WindowInfo {
  Window parent;
  int width,height,x,y,borderwidth;
  char primarywindow;
  XSetWindowAttributes CWattr;
  XSizeHints size_hints;
  unsigned long CWvaluemask;
  char *winname,*iconname;
  XClassHint class_hints;
  XWMHints wm_hints;
} GL_WindowInfo;

void GL_InitGraphics(
#if NeedFunctionPrototypes
int , /* argc */
char **, /* argv */
char *, /* classname */
VPTF /* helpfunction */
#endif
);
void _GL_VA_Init_Window();
void _GL_VA_Handle_Window();
void _GL_VA_Execute_Window();
void _GL_VA_Destroy_Window();
GLPointer _GL_VA_Return_Window();

#define GLW 1100
#define GL_WinWidth GLW
#define GL_WinHeight GLW+1
#define GL_WinParent GLW+2
#define GL_WinX GLW+3
#define GL_WinY GLW+4
#define GL_EventMask GLW+5
#define GL_OverrideRedirect GLW+6
#define GL_SaveUnder GLW+7
#define GL_WindowBorderWidth GLW+8
#define GL_WinName GLW+9
#define GL_BackgroundPixel GLW+10
#define GL_PrimaryWindow GLW+11

#define GL_Window _GL_VA_Init_Window,_GL_VA_Handle_Window, \
                  _GL_VA_Execute_Window,_GL_VA_Destroy_Window, \
                  _GL_VA_Return_Window
#define GL_CreateWindow (Window)GL_Create
void GL_Window_Help();

#endif

