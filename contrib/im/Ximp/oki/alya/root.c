/*
 * Copyright 1991 Oki Technosystems Laboratory, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Oki Technosystems Laboratory not
 * be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Oki Technosystems Laboratory makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Oki TECHNOSYSTEMS LABORATORY DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL OKI TECHNOSYSTEMS LABORATORY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Yasuhiro Kawai,	Oki Technosystems Laboratory, Inc.
 *				kawai@otsl.oki.co.jp
 */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include "Ximp.h"

#include "Preedit.h"

static Widget preedit, status, client_info;

void
RootInitialize(w)
    Widget w;
{
    Arg args[10];
    int ac;

    ac = 0;
    XtSetArg(args[ac], XtNwidth, 320); ac++;
    XtSetArg(args[ac], XtNheight, 18); ac++;
    preedit = XtCreateManagedWidget("rootPreedit", preeditWidgetClass,
				    w, args, ac);
    ac = 0;
    XtSetArg(args[ac], XtNwidth, 320); ac++;
    XtSetArg(args[ac], XtNheight, 18); ac++;
    status = XtCreateManagedWidget("rootStatus", preeditWidgetClass,
				   w, args, ac);
}

void
RootPreeditDraw()
{
}

void
RootStatusDraw()
{
}

void
RootPreeditCaret()
{
}

