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
#ifndef _GL_RESMGR_H
#define _GL_RESMGR_H

#include "X11/gl/stddef.h"
#define GL_RES_BOOLEAN 1
#define GL_RES_LONG 2
#define GL_RES_STRING 3
#define GL_RES_FUNCTION 4

#define GL_RES_MAXVAL 5 /* 1+Maximum value */

#define NO_CMDLINE_ARG "\17\127 NOCMDLINEARG"
#define NO_RESOURCE "\17\127 NORESOURCE"

void _GL_VA_resmgr_nullproc();
void _GL_VA_init_resmgr();
void _GL_VA_handle_resmgr();

#define GL_Resources _GL_VA_init_resmgr,_GL_VA_handle_resmgr,  \
                     _GL_VA_resmgr_nullproc,_GL_VA_resmgr_nullproc,\
                     _GL_VA_resmgr_nullproc
#endif
