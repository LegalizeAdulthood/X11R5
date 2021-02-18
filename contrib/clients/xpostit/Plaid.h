/*
 * Copyright 1991 by David A. Curry
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation.  The
 * author makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied warranty.
 */
/*
 * $Header: /home/orchestra/davy/progs/xpostit/RCS/Plaid.h,v 1.4 91/09/06 18:29:07 davy Exp $
 *
 * Plaid.h - public definitions for the plaid widget.
 *
 * Based on the Template widget from the X11R4 distribution.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	Plaid.h,v $
 * Revision 1.4  91/09/06  18:29:07  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:15:23  davy
 * Changed my address, added callbacks.
 * 
 * Revision 1.2  90/06/14  11:18:27  davy
 * Ported to X11 Release 4.
 * 
 * Revision 1.1  90/06/13  09:48:42  davy
 * Initial revision
 * 
 */
#include <X11/copyright.h>

#ifndef _Plaid_h
#define _Plaid_h

/*
 * Resources:
 *
 * Name		     Class		RepType		Default Value
 * ----		     -----		-------		-------------
 * background	     Background		Pixel		XtDefaultBackground
 * border	     BorderColor	Pixel		XtDefaultForeground
 * borderWidth	     BorderWidth	Dimension	1
 * callback	     Callback		Callback	NULL
 * destroyCallback   Callback		Pointer		NULL
 * foreground	     Foreground		Pixel		XtDefaultForeground
 * height	     Height		Dimension	0
 * lowerCallback     Callback		Pointer		NULL
 * raiseCallback     Callback		Pointer		NULL
 * mappedWhenManaged MappedWhenManaged	Boolean		True
 * sensitive	     Sensitive		Boolean		True
 * width	     Width		Dimension	0
 * x		     Position		Position	0
 * y		     Position		Position	0
 */

/*
 * Declare specific PlaidWidget class and instance datatypes.
 */
typedef struct _PlaidClassRec*		PlaidWidgetClass;
typedef struct _PlaidRec*		PlaidWidget;

/*
 * Declare the class constant.
 */
extern WidgetClass			plaidWidgetClass;

/*
 * Declare a couple of new resources.
 */
#define XtNlowerCallback		"lowerCallback"
#define XtNraiseCallback		"raiseCallback"
#define XtNhideCallback			"hideCallback"
#define XtNshowCallback			"showCallback"
#define XtNquitCallback			"quitCallback"

#endif /* _Plaid_h */
