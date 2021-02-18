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
#ifndef _GL_POLYGONOBJ_H
#define _GL_POLYGONOBJ_H

/* Not really a polygon, actually a list of points for XDrawLine, 
   but hey who cares, it essentially the same thing, 
   and this works well for subclassing. */
#include "X11/gl/object.h"

typedef struct _GL_PolygonObjectInfo {
  int numpoints,mode,closed;
  XPoint *points;
} GL_PolygonObjectInfo;

void _GL_VA_Init_PolygonObject();
void _GL_VA_Handle_PolygonObject();
void _GL_VA_Execute_PolygonObject();

#define GL_PolygonObject _GL_VA_Init_PolygonObject, _GL_VA_Handle_PolygonObject, \
                         _GL_VA_Execute_PolygonObject, _GL_VA_Destroy_Object, \
                         _GL_VA_Return_Object

#define GLPO 2000

#define GLPO_NumPoints GLPO
#define GLPO_Closed GLPO+1
#define GLPO_CoordModePrevious GLPO+2
#define GLPO_Points GLPO+3

/* Must specify Number of points before listing points.
   Code does check for this though. */

#endif
