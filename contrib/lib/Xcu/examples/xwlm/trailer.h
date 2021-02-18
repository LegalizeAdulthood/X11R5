XtRealizeWidget (top) ;

XcuWlmSample (wlm_id) ;

for (;;)
    {
    XcuWlmRequest ( wlm_id ) ;
    fprintf (stderr, "x value is now %d\n", x) ;
    }
}

static void
exit_routine (w, client_data, call_data)
    Widget w ;
    caddr_t client_data ;
    caddr_t call_data ;
{
fprintf (stderr, "X value on exit is %d\n", x) ;
exit (0) ;
}


#include <X11/Xcu/WlmP.h>
#include <X11/Xcu/Wlm.h>
#include <X11/Xcu/Bmgr.h>
#include <X11/Xcu/Button.h>
#include <X11/Xcu/Command.h>
#include <X11/Xaw/Command.h>
#include <X11/Xcu/Tbl.h>
#include <X11/Xcu/Deck.h>

void
make_tag_class_list (ww)
    XcuWlmWidget ww ;
{
TAG_CLASS_ENTRY(ww, "XcuWlm", xcuWlmWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuTbl", xcuTblWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuDeck", xcuDeckWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuBmgr", xcuBmgrWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuButton", xcuButtonWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCommand", xcuCommandWidgetClass) ;
TAG_CLASS_ENTRY(ww, "Command", commandWidgetClass) ;
return ;
}

/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

