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
#ifndef _GL_OBJECT_H
#define _GL_OBJECT_H

#include "X11/gl/window.h"

typedef struct _GL_ObjectInfo {
  Window drawwin;
  int x, y;
  int xmin, ymin, width, height;
  VPTF draw,install,invert;
  long flags;
  GL_DataList thelist;
} GL_ObjectInfo;

typedef struct _GL_ObjectMgrInfo {
  GL_DataList Objects;
  long objcount;
} GL_ObjectMgrInfo;

/* Draw Function takes expose event, or null which means draw everything. */
/* Install Function takes object,window. */
void _GL_VA_Init_Object();
void _GL_VA_Handle_Object();
void _GL_VA_Execute_Object();
void _GL_VA_Destroy_Object();
GLPointer _GL_VA_Return_Object();

#define GL_Object  _GL_VA_Init_Object, _GL_VA_Handle_Object, \
                   _GL_VA_Execute_Object, _GL_VA_Destroy_Object, \
                   _GL_VA_Return_Object

#define GLO 1000

#define GLO_X GLO
#define GLO_Y GLO+1
#define GLO_Window GLO+2
#define GLO_Inverted GLO+3

void GL_InvertObject(/* object,invert setting */);
void GL_InstallObject(/* object ,window */);

#define GLO_MASK_INVERTED (1<<0)
#define OBJECT_INVERTED(oinfo) (oinfo->flags&GLO_MASK_INVERTED)

#endif /*ndef...*/
