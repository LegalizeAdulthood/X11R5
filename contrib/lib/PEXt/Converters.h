/*
			  C o n v e r t e r s . h

    $Header: Converters.h,v 1.0 91/10/04 17:00:57 rthomson Exp $

    Rich Thomson
    Design Systems Division
    Evans & Sutherland Computer Corporation

	Copyright (C) 1990, 1991, Evans & Sutherland Computer Corporation
*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			   Salt Lake City, Utah

			    All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its 
    documentation for any purpose and without fee is hereby granted, 
    provided that the above copyright notice appear in all copies and that
    both that copyright notice and this permission notice appear in 
    supporting documentation, and that the names of Evans & Sutherland not be
    used in advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.  

    EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef _PEXt_Converters_h
#define _PEXt_Converters_h

#define PEXtToolkitError "PEXtToolkitError"

					/* wrong parameter errors */
#define WrongParameters(routine, message) \
  XtAppWarningMsg(XtDisplayToApplicationContext(dpy), "wrongParameters", \
		  routine, PEXtToolkitError, message, \
		  (String *) NULL, (Cardinal *) NULL)

					/* conversion errors */
#define ConversionError(routine, message) \
  XtAppWarningMsg(XtDisplayToApplicationContext(dpy), "conversionError", \
		  routine, PEXtToolkitError, message, \
		  (String *) NULL, (Cardinal *) NULL)

					/* convenience macro */
#define DONE(var, type) \
  if (toVal->addr) \
    { \
      if (toVal->size < sizeof(type)) \
	{ \
	  toVal->size = sizeof(type); \
	  return False; \
	} \
      else \
	*((type *) toVal->addr) = var; \
    } \
  else \
    toVal->addr = (caddr_t) &var; \
  toVal->size = sizeof(type); \
  return True;

#define DONESTR(str) \
  if (toVal->addr && toVal->size < sizeof(String)) \
    { \
      toVal->size = sizeof(String); \
      return False; \
    } \
  else \
    toVal->addr = (caddr_t) str; \
  toVal->size = sizeof(String); \
  return True;

#endif /* _PEXt_Converters_h */
