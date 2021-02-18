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

/*
 *  A few definitions to make XcuButton.c easier to read.
 */

  /* Yes, this is gross, but the code will be easier to read.
     Trust me.  */
#define ComWx                            cbw->core.x
#define ComWy                            cbw->core.y
#define ComWdepth                        cbw->core.depth
#define ComWwidth                        cbw->core.width
#define ComWheight                       cbw->core.height
#define ComWborder_width                 cbw->core.border_width
#define ComWbackground                   cbw->core.background_pixel
#define ComWsensitive                    cbw->core.sensitive 
#define ComWforeground                   cbw->label.foreground
#define ComWfont                         cbw->label.font
#define ComWlabel                        cbw->label.label
#define ComWlocations                    cbw->label.locations
#define ComWlen                          cbw->label.len
#define ComWn_lines                      cbw->label.n_lines
#define ComWinternalWidth                cbw->label.internal_width
#define ComWinternalHeight               cbw->label.internal_height
#define ComWlabelX                       cbw->label.x
#define ComWlabelY                       cbw->label.y
#define ComWnormalGC                     cbw->button.normal_GC
#define ComWgrayGC                       cbw->button.gray_GC
#define ComWcallbackList                 cbw->button.callback_list
#define ComWcallback                     cbw->button.callback
#define ComWclosure                      cbw->button.closure
#define ComWhighlightGC                  cbw->button.highlight_GC
#define ComWinverseGC                    cbw->button.inverse_GC
#define ComWinverseTextGC                cbw->button.inverse_text_GC
#define ComWset                          cbw->button.set
#define ComWdisplaySet                   cbw->button.display_set
#define ComWdisplayHighlighted           cbw->button.display_highlighted
#define ComWhighlighted                  cbw->simple.highlighted
#define ComWhighlightChange              cbw->simple.highlight_change
#define ComWnotExpose                    cbw->button.not_expose

#define XtBField(cbw,field)  cbw->button.field
#define XtLField(cbw,field)   cbw->label.field
#define XtCField(cbw,field)   cbw->core.field


static void Initialize();
static void Redisplay();
static Boolean SetValues();
static void Notify();
static void Highlight();
static void Unhighlight();
static void Destroy();
