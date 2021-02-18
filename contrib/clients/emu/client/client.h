#ifndef _CLIENT_H
#define _CLIENT_H

/* $Header: /usr3/emu/client/RCS/client.h,v 1.1 90/11/20 17:36:37 jkh Exp Locker: jkh $ */

/*
 * This file is part of the PCS emu program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Common includes, types, routines, etc.
 *
 * Author: Jordan K. Hubbard
 * Date: October 18th, 1990.
 * Description: Some general stuff for the client.
 *
 * Revision History:
 *
 * $Log:	client.h,v $
 * Revision 1.1  90/11/20  17:36:37  jkh
 * Initial revision
 * 
 *
 */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "Term.h"
#include "TermCanvas.h"
#include "xt_ops.h"

#ifdef MOTIF
#include <X11/Xm/ScrollBar.h>
#else /* ATHENA */
#include <X11/Xaw/Scrollbar.h>
#endif /* MOTIF */

/*
 * We redefine these here just to keep a consistent coding style between
 * our widget code and client code. We can't just #include common.h from
 * the widgets, since that would break spec (and we really don't want to
 * enforce our coding standard on the world by putting them in Term.h).
 */
#define Export
#define Import extern
#define Local static
#ifdef __GNUC__
#define Inline inline
#else
#define Inline
#endif

Import Dimension HeightOf(), WidthOf();
Import Widget Term, SBar, MBar, Canvas;
Import Widget ScrollbarCreate();
Import Widget XpEmuInitializeMenus();
Import XtGeometryResult do_layout();
Import void ScrollbarAdjust();

#endif /* _CLIENT_H */
