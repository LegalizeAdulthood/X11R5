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
#ifndef _GL_SYSDEF_H
#define _GL_SYSDEF_H
#define STUPID char
typedef STUPID * GLPointer;
/* This should be large enough to hold a long, and any pointer.
   It REALLY should be a void * but most people don't support that. */
/* If your system doesn't like void *'s, make it a char. */
/* I think this is stupid to have to worry about.  therefore, the name. */
/* this really should be a void. */

#ifndef NeedFunctionPrototypes
#define NeedFunctionPrototypes 0
#endif
/* comment this all out if your compiler can't deal with function prototypes */
/* or make it 0. */
/* #define NeedVarArgsPrototype
   /* Remove if you compiler can deal with (aa,bb,cc,...) 
      function prototypes. */

#ifdef sun
#ifdef mc68000
#define Sun3System
#endif
#endif

#endif /*ndef*/


