/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

/*
 * Author:	Doyle C. Davidson
 *		Intergraph Corporation
 *		One Madison Industrial Park
 *		Huntsville, Al.  35894-0001
 *
 * Modification history:
 *		11 May 91 - added SetValues and GetValues - Paul King
 *
 * My macros for using XtSetArg easily:
 * Usage:
 *
 *	blah()
 *	{
 *	DeclareArgs(2);
 *		...
 *		FirstArg(XmNx, 100);
 *		NextArg(XmNy, 80);
 *		button = XmCreatePushButton(parent, name, Args, ArgCount);
 *	}
 */

#include "assert.h"

#define ArgCount	_fooArgCount
#define Args		_fooArgList
#define ArgCountMax     _fooArgCountMax

#define DeclareArgs(n)  Arg Args[n]; int ArgCountMax = n; int ArgCount

#define DeclareStaticArgs(n)  static Arg Args[n]; static int ArgCountMax = n; static int ArgCount

#define FirstArg(name, val) \
	{ XtSetArg(Args[0], (name), (val)); ArgCount=1;}
#define NextArg(name, val) \
	{ assert(ArgCount < ArgCountMax); \
	  XtSetArg(Args[ArgCount], (name), (val)); ArgCount++;}
#define GetValues(n)	XtGetValues(n, Args, ArgCount)
#define SetValues(n)	XtSetValues(n, Args, ArgCount)
