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
 *  A few definitions to make XcuEntry.c easier to read.
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
#define ComWforeground                   cbw->label.foreground
#define ComWfont                         cbw->label.font
#define ComWlabel                        cbw->label.label
#define ComWjustify                      cbw->label.justify
#define ComWinternalWidth                cbw->label.internal_width
#define ComWinternalHeight               cbw->label.internal_height
#define ComWlabelX                       cbw->label.label_x
#define ComWlabelY                       cbw->label.label_y
#define ComWlabelWidth                   cbw->label.label_width
#define ComWlabelHeight                  cbw->label.label_height
#define ComWlabelLen                     cbw->label.label_len
#define ComWgrayPixmap                   cbw->label.gray_pixmap
#define ComWsensitive                    cbw->core.sensitive 
#define ComWnormalGC                     cbw->command.normal_GC
#define ComWgrayGC                       cbw->command.gray_GC
#define ComWcallbackList                 cbw->command.callback_list
#define ComWcallback                     cbw->command.callback
#define ComWclosure                      cbw->command.closure
#define ComWhighlightGC                  cbw->command.highlight_GC
#define ComWinverseGC                    cbw->command.inverse_GC
#define ComWinverseTextGC                cbw->command.inverse_text_GC
#define ComWhighlightThickness           cbw->command.highlight_thickness
#define ComWset                          cbw->command.set
#define ComWdisplaySet                   cbw->command.display_set
#define ComWdisplayHighlighted           cbw->command.display_highlighted
#define ComWhighlighted                  cbw->simple.highlighted
#define ComWexposing	                 cbw->simple.exposing
#define ComWnotExpose	                 cbw->command.not_expose
#define ComWhighlightChange              cbw->simple.highlight_change

#define XtCBField(cbw,field)  cbw->command.field
#define XtLField(cbw,field)   cbw->label.field
#define XtCField(cbw,field)   cbw->core.field

static void Initialize();
static void Redisplay();
static Boolean SetValues();
static void Destroy();
