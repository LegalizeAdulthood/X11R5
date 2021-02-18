/* 
 * FontSB.h
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

#ifndef _FontSelectionBox_h
#define _FontSelectionBox_h

/* New resouce names and classes */

#define XtNcontext "context"
#define XtCContext "Context"
#define XtNpreviewString "previewString"
#define XtCPreviewString "PreviewString"
#define XtNsizes "sizes"
#define XtCSizes "Sizes"
#define XtNsizeCount "sizeCount"
#define XtCSizeCount "SizeCount"
#define XtNdefaultResourcePath "defaultResourcePath"
#define XtCDefaultResourcePath "DefaultResourcePath"
#define XtNresourcePathOverride "resourcePathOverride"
#define XtCResourcePathOverride "ResourcePathOverride"
#define XtNuseFontName "useFontName"
#define XtCUseFontName "UseFontName"
#define XtNfontName "fontName"
#define XtCFontName "FontName"
#define XtNfontFamily "fontFamily"
#define XtCFontFamily "FontFamily"
#define XtNfontFace "fontFace"
#define XtCFontFace "FontFace"
#define XtNfontSize "fontSize"
#define XtCFontSize "FontSize"
#define XtNfontNameMultiple "fontNameMultiple"
#define XtCFontNameMultiple "FontNameMultiple"
#define XtNfontFamilyMultiple "fontFamilyMultiple"
#define XtCFontFamilyMultiple "FontFamilyMultiple"
#define XtNfontFaceMultiple "fontFaceMultiple"
#define XtCFontFaceMultiple "FontFaceMultiple"
#define XtNfontSizeMultiple "fontSizeMultiple"
#define XtCFontSizeMultiple "FontSizeMultiple"
#define XtNgetServerFonts "getServerFonts"
#define XtCGetServerFonts "GetServerFonts"
#define XtNgetAFM "getAFM"
#define XtCGetAFM "GetAFM"
#define XtNautoPreview "autoPreview"
#define XtCAutoPreview "AutoPreview"
#define XtNpreviewOnChange "previewOnChange"
#define XtCPreviewOnChange "PreviewOnChange"
#define XtNundefUnusedFonts "undefUnusedFonts"
#define XtCUndefUnusedFonts "UndefUnusedFonts"
#define XtNmaxPendingDeletes "maxPendingDeletes"
#define XtCMaxPendingDeletes "MaxPendingDeletes"
#define XtNmakeFontsShared "makeFontsShared"
#define XtCMakeFontsShared "MakeFontsShared"
#define XtNshowSampler "showSampler"
#define XtCShowSampler "ShowSampler"
#define XtNshowSamplerButton "showSamplerButton"
#define XtCShowSamplerButton "ShowSamplerButton"
#define XtNokCallback "okCallback"
#define XtNapplyCallback "applyCallback"
#define XtNresetCallback "resetCallback"
#define XtNcancelCallback "cancelCallback"
#define XtNvalidateCallback "validateCallback"
#define XtNfaceSelectCallback "faceSelectCallback"
#define XtNcreateSamplerCallback "createSamplerCallback"

/* Read-only resources for all the subwidgets */

#define XtNpaneChild "paneChild"
#define XtNpreviewChild "previewChild"
#define XtNpanelChild "panelChild"
#define XtNfamilyLabelChild "familyLabelChild"
#define XtNfamilyMultipleLabelChild "familyMultipleLabelChild"
#define XtNfamilyScrolledListChild "familyScrolledListChild"
#define XtNfaceLabelChild "faceLabelChild"
#define XtNfaceMultipleLabelChild "faceMultipleLabelChild"
#define XtNfaceScrolledListChild "faceScrolledListChild"
#define XtNsizeLabelChild "sizeLabelChild"
#define XtNsizeTextFieldChild "sizeTextFieldChild"
#define XtNsizeOptionMenuChild "sizeOptionMenuChild"
#define XtNsizeMultipleLabelChild "sizeMultipleLabelChild"
#define XtNsamplerButtonChild "samplerButtonChild"
#define XtNpreviewButtonChild "previewButtonChild"
#define XtNseparatorChild "separatorChild"
#define XtNokButtonChild "okButtonChild"
#define XtNapplyButtonChild "applyButtonChild"
#define XtNresetButtonChild "resetButtonChild"
#define XtNcancelButtonChild "cancelButtonChile"

/* New resource types */

#define XtRFloatList "FloatList"
#define XtRDPSContext "DPSContext"

/* Class record constants */

extern WidgetClass fontSelectionBoxWidgetClass;

typedef struct _FontSelectionBoxRec *FontSelectionBoxWidget;

typedef enum {FSBNone, FSBOne, FSBMultiple} FSBSelectionType;
typedef enum {FSBOK, FSBApply, FSBReset, FSBCancel} FSBCallbackReason;

typedef struct {
    FSBCallbackReason reason;
    String family;
    String face;
    float size;
    String name;
    String afm_filename;
    FSBSelectionType family_selection;
    FSBSelectionType face_selection;
    FSBSelectionType size_selection;
    FSBSelectionType name_selection;
    Boolean afm_present;
} FSBCallbackRec;

typedef struct {
    FSBCallbackReason reason;
    String family;
    String face;
    float size;
    String name;
    String afm_filename;
    FSBSelectionType family_selection;
    FSBSelectionType face_selection;
    FSBSelectionType size_selection;
    FSBSelectionType name_selection;
    Boolean afm_present;
    Boolean doit;
} FSBValidateCallbackRec;

typedef struct {
    String *available_faces;
    int num_available_faces;
    String current_face;
    String new_face;
} FSBFaceSelectCallbackRec;

typedef struct {
    Widget sampler;
    Widget sampler_shell;
} FSBCreateSamplerCallbackRec;

/* Convenience routine */

#ifdef _NO_PROTO

extern void FSBSetFontName();
extern void FSBSetFontFamilyFace();
extern void FSBSetFontSize();
extern void FSBRefreshFontList();
extern void FSBGetFamilyList();
extern void FSBGetFaceList();
extern void FSBUndefineUnusedFonts();
extern Boolean FSBDownloadFontName();
extern Boolean FSBMatchFontFace();
extern void FSBFontNameToFamilyFace();
extern void FSBFontFamilyFaceToName();
extern String FSBFindAFM();
extern String FSBFindFontFile();
extern void FSBGetTextDimensions();

#else /* _NO_PROTO */

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/* Note use of Bool and double in prototypes:  this makes the library
   work correctly when linked with no-prototype compiled objects */

extern void FSBSetFontName(Widget w,
			   String font_name,
			   Bool font_name_multiple);

extern void FSBSetFontFamilyFace(Widget w,
				 String font_family, String font_face,
				 Bool font_family_multiple,
				 Bool font_face_multiple);

extern void FSBSetFontSize(Widget w, double font_size,
			   Bool font_size_multiple);

extern void FSBRefreshFontList(Widget w);

extern void FSBGetFamilyList(Widget w, int *count_return,
			     String **family_return);

extern void FSBGetFaceList(Widget w, String family, int *count_return,
		      String **face_return, String **font_return);

extern void FSBUndefineUnusedFonts(Widget w);

extern Boolean FSBDownloadFontName(Widget w, String font_name);

extern Boolean FSBMatchFontFace(Widget w, String old_face, String new_family,
				String *new_face);

extern void FSBFontNameToFamilyFace(Widget w, String font_name,
				    String *family, String *face);

extern void FSBFontFamilyFaceToName(Widget w, String family, String face,
				    String *font_name);

extern String FSBFindAFM(Widget w, String font_name);

extern String FSBFindFontFile(Widget w, String font_name);

extern void FSBGetTextDimensions(Widget w, String text, String font,
				 double size, double x, double y,
				 float *dx, float *dy,
				 float *left, float *right,
				 float *top, float *bottom);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif
#endif /* _NO_PROTO */

#endif /* _FontSelectionBox_h */
/* DON'T ADD ANYTHING AFTER THIS #endif */
