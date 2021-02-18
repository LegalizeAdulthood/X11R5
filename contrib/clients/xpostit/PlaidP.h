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
 * $Header: /home/orchestra/davy/progs/xpostit/RCS/PlaidP.h,v 1.4 91/09/06 18:29:13 davy Exp $
 *
 * PlaidP.h - private definitions for the plaid widget.
 *
 * Based on the Template widget from the X11R4 distribution.
 *
 * David A. Curry
 * Purdue University
 * Engineering Computer Network
 * West Lafayette, IN 47907
 * davy@ecn.purdue.edu
 *
 * $Log:	PlaidP.h,v $
 * Revision 1.4  91/09/06  18:29:13  davy
 * Added copyright/permission notice for submission to MIT R5 contrib.
 * 
 * Revision 1.3  91/09/06  17:15:34  davy
 * Changed my address, added callbacks.
 * 
 * Revision 1.2  90/06/14  11:18:37  davy
 * Ported to X11 Release 4.
 * 
 * Revision 1.1  90/06/13  09:48:45  davy
 * Initial revision
 * 
 */
#include <X11/copyright.h>

#ifndef _PlaidP_h
#define _PlaidP_h

#include <X11/CoreP.h>
#include "Plaid.h"

/*
 * The plaid part of the class record is not used.
 */
typedef struct {
	int		empty;
} PlaidClassPart;

/*
 * Declare the class record for the widget.
 */
typedef struct _PlaidClassRec {
	CoreClassPart	core_class;
	PlaidClassPart	plaid_class;
} PlaidClassRec;

/*
 * Declare the plaid class record type.
 */
extern PlaidClassRec	plaidClassRec;

/*
 * Resources specific to the plaid widget.
 */
typedef struct {
	Pixel		foreground;
	XtCallbackList	lower_callback;
	XtCallbackList	raise_callback;
	XtCallbackList	hide_callback;
	XtCallbackList	show_callback;
	XtCallbackList	quit_callback;
} PlaidPart;

/*
 * Declare the widget type.
 */
typedef struct _PlaidRec {
	CorePart	core;
	PlaidPart	plaid;
} PlaidRec;

#endif /* _PlaidP_h */
