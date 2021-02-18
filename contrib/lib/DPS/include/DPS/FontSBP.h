/* 
 * FontSBP.h
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

#ifndef _FontSelectionBoxP_H
#define _FontSelectionBoxP_H

#include <DPS/FontSB.h>

/* Typedefs used in private fields */

typedef struct _FontRec {
    char *font_name;
    char *face_name;
    XmString CS_face_name;
    char *full_name;
    Boolean resident;
    Boolean temp_resident;
    struct _FontRec *pending_delete_next;
    struct _FontRec *next;
} FontRec;

typedef struct _FontFamilyRec {
    char *family_name;
    char *sort_key;
    XmString CS_family_name;
    FontRec *fonts;
    int font_count;
    struct _FontFamilyRec *next;
} FontFamilyRec;

typedef struct {
    DPSContext context;
    String preview_string;
    float *sizes;
    int size_count;
    String default_resource_path;
    String resource_path_override;
    String font_name;
    String font_family;
    String font_face;
    float font_size;
    Boolean use_font_name;
    Boolean font_name_multiple;
    Boolean font_family_multiple;
    Boolean font_face_multiple;
    Boolean font_size_multiple;
    Boolean get_server_fonts;
    Boolean get_afm;
    Boolean auto_preview;
    Boolean preview_on_change;
    Boolean undef_unused_fonts;
    Boolean make_fonts_shared;
    Boolean show_sampler;
    Boolean show_sampler_button;
    Cardinal max_pending_deletes;
    XtCallbackList ok_callback;
    XtCallbackList apply_callback;
    XtCallbackList reset_callback;
    XtCallbackList cancel_callback;
    XtCallbackList validate_callback;
    XtCallbackList face_select_callback;
    XtCallbackList create_sampler_callback;
    Widget pane_child;
    Widget preview_child;
    Widget panel_child;
    Widget family_label_child;
    Widget family_multiple_label_child;
    Widget family_scrolled_list_child;
    Widget face_label_child;
    Widget face_multiple_label_child;
    Widget face_scrolled_list_child;
    Widget size_label_child;
    Widget size_text_field_child;
    Widget size_option_menu_child;
    Widget size_multiple_label_child;
    Widget sampler_button_child;
    Widget preview_button_child;
    Widget separator_child;
    Widget ok_button_child;
    Widget apply_button_child;
    Widget reset_button_child;
    Widget cancel_button_child;

 /* Private fields */

    DPSGState gstate;
    Widget other_size;
    Widget size_menu;
    Widget sampler;
    FontFamilyRec *known_families;
    int family_count;
    Boolean preview_fixed;
    Boolean current_family_multiple, current_face_multiple;
    Boolean current_size_multiple;
    FontRec *pending_delete_font;
    int pending_delete_count;
    FontRec *currently_previewed;
    FontRec *currently_selected_face;
    FontFamilyRec *currently_selected_family;
    float currently_previewed_size;
    float currently_selected_size;
} FontSelectionBoxPart;

typedef struct _FontSelectionBoxRec {
    CorePart			core;
    CompositePart		composite;
    ConstraintPart		constraint;
    XmManagerPart		manager;
    FontSelectionBoxPart	fsb;
} FontSelectionBoxRec;

#ifdef _NO_PROTO

typedef void (*FSBSetFontNameProc) ();
typedef void (*FSBSetFontFamilyFaceProc) ();
typedef void (*FSBSetFontSizeProc) ();
typedef void (*FSBRefreshFontListProc) ();
typedef void (*FSBGetFamilyListProc) ();
typedef void (*FSBGetFaceListProc) ();
typedef void (*FSBUndefUnusedFontsProc) ();
typedef Boolean (*FSBDownloadFontNameProc) ();
typedef Boolean (*FSBMatchFontFaceProc) ();
typedef void (*FSBFontNameToFamilyFaceProc) ();
typedef void (*FSBFontFamilyFaceToNameProc) ();
typedef String (*FSBFindAFMProc) ();
typedef String (*FSBFindFontFileProc) ();
typedef void (*FSBGetTextDimensionsProc) ();

#else /* _NO_PROTO */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Note use of Bool and double in prototypes:  this makes the library
   work correctly when linked with no-prototype compiled objects */

typedef void (*FSBSetFontNameProc) (Widget w,
				    String font_name,
				    Bool font_name_multiple);

typedef void (*FSBSetFontFamilyFaceProc) (Widget w,
					  String font_family,
					  String font_face,
					  Bool font_family_multiple,
					  Bool font_face_multiple);

typedef void (*FSBSetFontSizeProc) (Widget w,
				    double font_size,
				    Bool font_size_multiple);

typedef void (*FSBRefreshFontListProc) (Widget w);

typedef void (*FSBGetFamilyListProc) (Widget w, int *count_return,
				      String **family_return);

typedef void (*FSBGetFaceListProc) (Widget w, String family, int *count_return,
				    String **face_return,
				    String **font_return);

typedef void (*FSBUndefUnusedFontsProc) (Widget w);

typedef Boolean (*FSBDownloadFontNameProc) (Widget w, String font_name);

typedef Boolean (*FSBMatchFontFaceProc) (Widget w, String old_face,
					 String new_family, String *new_face);

typedef void (*FSBFontNameToFamilyFaceProc) (Widget w, String font_name,
					     String *family, String *face);

typedef void (*FSBFontFamilyFaceToNameProc) (Widget w, String family,
					     String face, String *font_name);

typedef String (*FSBFindAFMProc) (Widget w, String font_name);

typedef String (*FSBFindFontFileProc) (Widget w, String font_name);

typedef void (*FSBGetTextDimensionsProc) (Widget w, String text, String font,
					  double size, double x, double y,
					  float *dx, float *dy,
					  float *left, float *right,
					  float *top, float *bottom);


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif /* _NO_PROTO */

#define InheritSetFontName ((FSBSetFontNameProc) _XtInherit)
#define InheritSetFontFamilyFace ((FSBSetFontFamilyFaceProc) _XtInherit)
#define InheritSetFontSize ((FSBSetFontSizeProc) _XtInherit)
#define InheritRefreshFontList ((FSBRefreshFontListProc) _XtInherit)
#define InheritGetFamilyList ((FSBGetFamilyListProc) _XtInherit)
#define InheritGetFaceList ((FSBGetFaceListProc) _XtInherit)
#define InheritUndefUnusedFonts ((FSBUndefUnusedFontsProc) _XtInherit)
#define InheritDownloadFontName ((FSBDownloadFontNameProc) _XtInherit)
#define InheritMatchFontFace ((FSBMatchFontFaceProc) _XtInherit)
#define InheritFontNameToFamilyFace ((FSBFontNameToFamilyFaceProc) _XtInherit)
#define InheritFontFamilyFaceToName ((FSBFontFamilyFaceToNameProc) _XtInherit)
#define InheritFindAFM ((FSBFindAFMProc) _XtInherit)
#define InheritFindFontFile ((FSBFindFontFileProc) _XtInherit)
#define InheritGetTextDimensions ((FSBGetTextDimensionsProc) _XtInherit)

typedef struct {
    FSBSetFontNameProc		set_font_name;
    FSBSetFontFamilyFaceProc	set_font_family_face;
    FSBSetFontSizeProc		set_font_size;
    FSBRefreshFontListProc	refresh_font_list;
    FSBGetFamilyListProc	get_family_list;
    FSBGetFaceListProc		get_face_list;
    FSBUndefUnusedFontsProc	undef_unused_fonts;
    FSBDownloadFontNameProc	download_font_name;
    FSBMatchFontFaceProc	match_font_face;
    FSBFontNameToFamilyFaceProc font_name_to_family_face;
    FSBFontFamilyFaceToNameProc font_family_face_to_name;
    FSBFindAFMProc		find_afm;
    FSBFindFontFileProc		find_font_file;
    FSBGetTextDimensionsProc	get_text_dimensions;
    XtPointer			extension;
} FontSelectionBoxClassPart;

typedef struct _FontSelectionBoxClassRec {
    CoreClassPart		core_class;
    CompositeClassPart		composite_class;
    ConstraintClassPart		constraint_class;
    XmManagerClassPart		manager_class;
    FontSelectionBoxClassPart	fsb_class;
} FontSelectionBoxClassRec, *FontSelectionBoxWidgetClass;

extern FontSelectionBoxClassRec fontSelectionBoxClassRec;

#endif /* _FontSelectionBoxP_H */
/* DON'T ADD ANYTHING AFTER THIS #endif */
