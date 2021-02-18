/*
 * psw.h
 *
 * Copyright (C) 1988 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#ifndef	PSW_H
#define	PSW_H

extern char *currentPSWName; /* valid between DEFINEPS and ENDPS */

/* C types */

#define T_BOOLEAN	   101
#define T_FLOAT		   102
#define T_DOUBLE	   103
#define T_CHAR		   104
#define T_UCHAR		   105
#define T_INT		   106
#define T_UINT		   107
#define T_LONGINT	   108
#define T_SHORTINT	   109
#define T_ULONGINT	   110
#define T_USHORTINT	   111
#define T_USEROBJECT   112
#define T_NUMSTR       113
#define T_FLOATNUMSTR  114
#define T_LONGNUMSTR   115
#define T_SHORTNUMSTR  116


/* PostScript types */

#define T_STRING       91
#define T_HEXSTRING    92
#define T_NAME         93
#define T_LITNAME      94
#define T_ARRAY        95
#define T_PROC         96
#define T_CONTEXT      97
#define T_SUBSCRIPTED  98

/* Other PostScript types:

   T_FLOAT is used for real
   T_INT is used for integer
   T_BOOLEAN is used for boolean
   T_USEROBJECT is used for userobjects

*/

#endif /* PSW_H */
