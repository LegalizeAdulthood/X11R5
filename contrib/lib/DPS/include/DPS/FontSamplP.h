/* 
 * FontSamplP.h
 *
 * Copyright (C) 1991 by Adobe Systems Incorporated.
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

#ifndef _FontSamplerP_H
#define _FontSamplerP_H

#include <DPS/FontSample.h>

typedef struct _DisplayedFontRec {
    Position l, r, t, b;
    FontRec *font;
    struct _DisplayedFontRec *next;
} DisplayedFontRec;

typedef struct {
    FontSamplerWidget sampler;
    Boolean inited, any_shown;
    Dimension column_width;
    Position x, y;
    Dimension height, width;
    Dimension window_height;
    int depth;
    float size;
    FontFamilyRec *current_family;
    FontRec *current_font;
    DisplayedFontRec *shown_fonts;
} DisplayRecord;

typedef struct {
    float *sizes;
    int size_count;
    FontSelectionBoxWidget fsb;
    XtCallbackList dismiss_callback;
    Dimension minimum_width;
    Dimension minimum_height;
    XmString no_room_message;
    XmString no_font_message;
    XmString no_selected_font_message;
    XmString no_selected_family_message;
    XmString no_family_font_message;
    XmString no_match_message;
    Widget panel_child;
    Widget area_child;
    Widget text_child;
    Widget font_label_child;
    Widget scrolled_window_child;
    Widget display_button_child;
    Widget dismiss_button_child;
    Widget stop_button_child;
    Widget clear_button_child;
    Widget radio_frame_child;
    Widget radio_box_child;
    Widget all_toggle_child;
    Widget selected_toggle_child;
    Widget selected_family_toggle_child;
    Widget filter_toggle_child;
    Widget filter_text_child;
    Widget filter_box_child;
    Widget filter_frame_child;
    Widget size_option_menu_child;
    Widget size_text_field_child;
    Widget size_label_child;

 /* Private fields */

    Widget size_menu;
    Widget other_size;
    Widget *filter_widgets;
    Widget clip_widget;
    Boolean displaying;
    Pixmap pixmap;
    XtWorkProcId current_display_proc;
    DisplayRecord *current_display_info;
    DPSGState gstate;
    DPSGState pixmap_gstate;
    GC gc;
    char *filter_text;
    Boolean *filter_flags;
    Boolean filters_changed;
    float ctm[6];
    float invctm[6];
} FontSamplerPart;

typedef struct _FontSamplerRec {
    CorePart			core;
    CompositePart		composite;
    ConstraintPart		constraint;
    XmManagerPart		manager;
    FontSamplerPart		sampler;
} FontSamplerRec;

#ifdef _NO_PROTO

typedef void (*FSBCancelProc) ();

#else /* _NO_PROTO */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

typedef void (*FSBCancelProc) (Widget w);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif /* _NO_PROTO */

#define InheritCancel ((FSBCancelProc) _XtInherit)

typedef struct {
    FSBCancelProc		cancel;
    XtPointer			extension;
} FontSamplerClassPart;

typedef struct _FontSamplerClassRec {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    FontSamplerClassPart	sampler_class;
} FontSamplerClassRec, *FontSamplerWidgetClass;

extern FontSamplerClassRec fontSamplerClassRec;

#endif /* _FontSamplerP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
