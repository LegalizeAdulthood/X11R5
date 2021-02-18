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
#ifndef _GL_LABEL_H
#define _GL_LABEL_H

#include "X11/gl/window.h"
#include "X11/gl/object.h"
/* label owns varargs entries 2000-2999 */

typedef struct _GL_LabelInfo {
  char *data;
  int leftbuffer;
  XFontStruct *font;
  GLPointer textobject;
} GL_LabelInfo;

void _GL_VA_Init_Label();
void _GL_VA_Handle_Label();
void _GL_VA_Execute_Label();
void GL_Label_Help();
extern void GL_Label_Help();

#define GL_Label _GL_VA_Init_Label, _GL_VA_Handle_Label, \
                 _GL_VA_Execute_Label, _GL_VA_Destroy_Window, \
                 _GL_VA_Return_Window

#define GLL 2000

#define GLL_InitialLabel GLL
#define GLL_LeftBuffer GLL+1
#define GLL_Font GLL+2


#endif

