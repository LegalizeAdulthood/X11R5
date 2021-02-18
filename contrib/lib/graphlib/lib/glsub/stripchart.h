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
#ifndef _GL_STRIPCHART_H
#define _GL_STRIPCHART_H

#include "X11/gl/window.h"
/* stripchart owns varargs entries 2000-2999 */
typedef long (*LPTF)();

void GL_ResetStripChart();

typedef struct _GL_StripChartInfo {
  VPTF exposefunc,drawfunc,changescalemaxfunc;
  LPTF getfunc;
  GC gc;
  int winheight,winwidth;
  int realmax,curstat,drawpos;
  int minscale,maxscale,deltascale,multiplier,curscale;
  int interval_sec,interval_usec;
  long *stats;
} GL_StripChartInfo;

void _GL_VA_Init_StripChart();
void _GL_VA_Handle_StripChart();
void _GL_VA_Execute_StripChart();

#define GL_StripChart _GL_VA_Init_StripChart,_GL_VA_Handle_StripChart,\
                      _GL_VA_Execute_StripChart,_GL_VA_Destroy_Window,\
                      _GL_VA_Return_Window
#define GLSC 2000

#define GLSC_DrawFunc GLSC
#define GLSC_GetFunc GLSC+1
#define GLSC_CurScale GLSC+2
#define GLSC_MinScale GLSC+3
#define GLSC_MaxScale GLSC+4
#define GLSC_DeltaScale GLSC+5
#define GLSC_Interval_Sec GLSC+6
#define GLSC_Interval_USec GLSC+7
#define GLSC_ChangeScaleMaxFunc GLSC+8
#define GLSC_Multiplier GLSC+9

void GL_StripChart_Help();
#endif /* ndef _GL_STRIPCHART_H */
