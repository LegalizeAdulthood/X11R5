/* 
 * FontSB.c
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

#include <ctype.h>
#include <stdio.h>
#if NeXT
#include <stdlib.h>
#endif /* NeXT */
#include <math.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/PushB.h>
#include <Xm/PanedW.h>
#include <Xm/PushBG.h>
#include <Xm/SeparatoG.h>
#include <Xm/TextF.h>
#include <Xm/RowColumn.h>
#include <Xm/DrawingA.h>
#include <DPS/dpsXclient.h>
#include "dpsXcommonI.h"
#include "DPS/dpsXcommon.h"
#include "DPS/dpsXshare.h"
#include <DPS/PSres.h>
#include "DPS/FontSBP.h"
#include "FSBwraps.h"
#include "FontSBI.h"
#include "DPS/FontSample.h"

/* Turn a string into a compound string */
#define CS(str) XmStringCreate(str, XmSTRING_DEFAULT_CHARSET)
static XmString CSempty;

/* Create a canonical representation of a string, and as a side effect
   make sure the string is in permanent storage.  This implementation may
   not work under all Xlibs */

#define Canonical(str) XrmQuarkToString(XrmStringToQuark(str))

static float defaultSizeList[] = {
#ifndef DEFAULT_SIZE_LIST
    8, 10, 12, 14, 16, 18, 24, 36, 48, 72
#else
	DEFAULT_SIZE_LIST
#endif /* DEFAULT_SIZE_LIST */
};

#ifndef DEFAULT_SIZE_LIST_COUNT
#define DEFAULT_SIZE_LIST_COUNT 10
#endif /* DEFAULT_SIZE_LIST_COUNT */

#ifndef DEFAULT_SIZE
static float default_size = 12.0;
#else
static float default_size = DEFAULT_SIZE
#endif /* DEFAULT_SIZE */

#ifndef DEFAULT_RESOURCE_PATH
#define DEFAULT_RESOURCE_PATH NULL
#endif /* DEFAULT_RESOURCE_PATH */

#ifndef DEFAULT_MAX_PENDING
#define DEFAULT_MAX_PENDING 10
#endif /* DEFAULT_MAX_PENDING */

#define Offset(field) XtOffsetOf(FontSelectionBoxRec, fsb.field)

static XtResource resources[] = {
    {XtNcontext, XtCContext, XtRDPSContext, sizeof(DPSContext),
	Offset(context), XtRDPSContext, (XtPointer) NULL},
    {XtNpreviewString, XtCPreviewString, XtRString, sizeof(String),
	Offset(preview_string), XtRString, (XtPointer) NULL},
    {XtNsizes, XtCSizes, XtRFloatList, sizeof(float*),
	Offset(sizes), XtRImmediate, (XtPointer) defaultSizeList},
    {XtNsizeCount, XtCSizeCount, XtRInt, sizeof(int),
	Offset(size_count), XtRImmediate, (XtPointer) DEFAULT_SIZE_LIST_COUNT},
    {XtNdefaultResourcePath, XtCDefaultResourcePath, XtRString, sizeof(String),
	Offset(default_resource_path), XtRImmediate,
	(XtPointer) DEFAULT_RESOURCE_PATH},
    {XtNresourcePathOverride, XtCResourcePathOverride,
	XtRString, sizeof(String),
	Offset(resource_path_override), XtRString, (XtPointer) NULL},
    {XtNuseFontName, XtCUseFontName, XtRBoolean, sizeof(Boolean),
	Offset(use_font_name), XtRImmediate, (XtPointer) True},
    {XtNfontName, XtCFontName, XtRString, sizeof(String),
	Offset(font_name), XtRString, (XtPointer) NULL},
    {XtNfontFamily, XtCFontFamily, XtRString, sizeof(String),
	Offset(font_family), XtRString, (XtPointer) NULL},
    {XtNfontFace, XtCFontFace, XtRString, sizeof(String),
	Offset(font_face), XtRString, (XtPointer) NULL},
    {XtNfontSize, XtCFontSize, XtRFloat, sizeof(String),
	Offset(font_size), XtRFloat, (XtPointer) &default_size},
    {XtNfontNameMultiple, XtCFontNameMultiple, XtRBoolean, sizeof(Boolean),
	Offset(font_name_multiple), XtRImmediate, (XtPointer) False},
    {XtNfontFamilyMultiple, XtCFontFamilyMultiple, XtRBoolean, sizeof(Boolean),
	Offset(font_family_multiple), XtRImmediate, (XtPointer) False},
    {XtNfontFaceMultiple, XtCFontFaceMultiple, XtRBoolean, sizeof(Boolean),
	Offset(font_face_multiple), XtRImmediate, (XtPointer) False},
    {XtNfontSizeMultiple, XtCFontSizeMultiple, XtRBoolean, sizeof(Boolean),
	Offset(font_size_multiple), XtRImmediate, (XtPointer) False},
    {XtNgetServerFonts, XtCGetServerFonts, XtRBoolean, sizeof(Boolean),
	Offset(get_server_fonts), XtRImmediate, (XtPointer) True},
    {XtNgetAFM, XtCGetAFM, XtRBoolean, sizeof(Boolean),
	Offset(get_afm), XtRImmediate, (XtPointer) False},
    {XtNautoPreview, XtCAutoPreview, XtRBoolean, sizeof(Boolean),
	Offset(auto_preview), XtRImmediate, (XtPointer) True},
    {XtNpreviewOnChange, XtCPreviewOnChange, XtRBoolean, sizeof(Boolean),
	Offset(preview_on_change), XtRImmediate, (XtPointer) True},
    {XtNundefUnusedFonts, XtCUndefUnusedFonts, XtRBoolean, sizeof(Boolean),
	Offset(undef_unused_fonts), XtRImmediate, (XtPointer) True},
    {XtNmaxPendingDeletes, XtCMaxPendingDeletes, XtRCardinal, sizeof(Cardinal),
	Offset(max_pending_deletes), XtRImmediate,
	(XtPointer) DEFAULT_MAX_PENDING},
    {XtNmakeFontsShared, XtCMakeFontsShared, XtRBoolean, sizeof(Boolean),
	Offset(make_fonts_shared), XtRImmediate, (XtPointer) True},
    {XtNshowSampler, XtCShowSampler, XtRBoolean, sizeof(Boolean),
	Offset(show_sampler), XtRImmediate, (XtPointer) False},
    {XtNshowSamplerButton, XtCShowSamplerButton, XtRBoolean, sizeof(Boolean),
	Offset(show_sampler_button), XtRImmediate, (XtPointer) True},
    {XtNokCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(ok_callback), XtRCallback, (XtPointer) NULL},
    {XtNapplyCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(apply_callback), XtRCallback, (XtPointer) NULL},
    {XtNresetCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(reset_callback), XtRCallback, (XtPointer) NULL},
    {XtNcancelCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(cancel_callback), XtRCallback, (XtPointer) NULL},
    {XtNvalidateCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(validate_callback), XtRCallback, (XtPointer) NULL},
    {XtNfaceSelectCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
	Offset(face_select_callback), XtRCallback, (XtPointer) NULL},
    {XtNcreateSamplerCallback, XtCCallback, XtRCallback,
	sizeof(XtCallbackList), Offset(create_sampler_callback),
	XtRCallback, (XtPointer) NULL},
    {XtNpaneChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(pane_child), XtRWidget, (XtPointer) NULL},
    {XtNpreviewChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(preview_child), XtRWidget, (XtPointer) NULL},
    {XtNpanelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(panel_child), XtRWidget, (XtPointer) NULL},
    {XtNfamilyLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(family_label_child), XtRWidget, (XtPointer) NULL},
    {XtNfamilyMultipleLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(family_multiple_label_child), XtRWidget, (XtPointer) NULL},
    {XtNfamilyScrolledListChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(family_scrolled_list_child), XtRWidget, (XtPointer) NULL},
    {XtNfaceLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(face_label_child), XtRWidget, (XtPointer) NULL},
    {XtNfaceMultipleLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(face_multiple_label_child), XtRWidget, (XtPointer) NULL},
    {XtNfaceScrolledListChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(face_scrolled_list_child), XtRWidget, (XtPointer) NULL},
    {XtNsizeLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(size_label_child), XtRWidget, (XtPointer) NULL},
    {XtNsizeTextFieldChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(size_text_field_child), XtRWidget, (XtPointer) NULL},
    {XtNsizeOptionMenuChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(size_option_menu_child), XtRWidget, (XtPointer) NULL},
    {XtNsizeMultipleLabelChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(size_multiple_label_child), XtRWidget, (XtPointer) NULL},
    {XtNpreviewButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(preview_button_child), XtRWidget, (XtPointer) NULL},
    {XtNsamplerButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(sampler_button_child), XtRWidget, (XtPointer) NULL},
    {XtNseparatorChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(separator_child), XtRWidget, (XtPointer) NULL},
    {XtNokButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(ok_button_child), XtRWidget, (XtPointer) NULL},
    {XtNapplyButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(apply_button_child), XtRWidget, (XtPointer) NULL},
    {XtNresetButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(reset_button_child), XtRWidget, (XtPointer) NULL},
    {XtNcancelButtonChild, XtCReadOnly, XtRWidget, sizeof(Widget),
	Offset(cancel_button_child), XtRWidget, (XtPointer) NULL}
};

/* Forward declarations */

static void ClassInitialize(), ClassPartInitialize(), Initialize(), Destroy(),
	ChangeManaged(), Resize(),
	SetFontName(), SetFontFamilyFace(), SetFontSize(), RefreshFontList(),
	GetFamilyList(), GetFaceList(), UndefUnusedFonts(),
	FontNameToFamilyFace(), FontFamilyFaceToName(),
	GetTextDimensions();
static Boolean SetValues(), DownloadFontName(), MatchFontFace();
static XtGeometryResult GeometryManager();
static String FindAFM(), FindFontFile();

FontSelectionBoxClassRec fontSelectionBoxClassRec = {
    /* Core class part */
  {
    /* superclass	     */	(WidgetClass) &xmManagerClassRec,
    /* class_name	     */ "FontSelectionBox",
    /* widget_size	     */ sizeof(FontSelectionBoxRec),
    /* class_initialize      */ ClassInitialize,
    /* class_part_initialize */ ClassPartInitialize,
    /* class_inited          */	FALSE,
    /* initialize	     */	Initialize,
    /* initialize_hook       */	NULL,
    /* realize		     */	XtInheritRealize,
    /* actions		     */	NULL,
    /* num_actions	     */	0,
    /* resources	     */	resources,
    /* num_resources	     */	XtNumber(resources),
    /* xrm_class	     */	NULLQUARK,
    /* compress_motion	     */	TRUE,
    /* compress_exposure     */	XtExposeCompressMultiple,
    /* compress_enterleave   */	TRUE,
    /* visible_interest	     */	FALSE,
    /* destroy		     */	Destroy,
    /* resize		     */	Resize,
    /* expose		     */	NULL,
    /* set_values	     */	SetValues,
    /* set_values_hook       */	NULL,			
    /* set_values_almost     */	XtInheritSetValuesAlmost,  
    /* get_values_hook       */	NULL,			
    /* accept_focus	     */	NULL,
    /* version		     */	XtVersion,
    /* callback offsets      */	NULL,
    /* tm_table              */	NULL,
    /* query_geometry	     */	XtInheritQueryGeometry,
    /* display_accelerator   */	NULL,
    /* extension	     */	NULL,
  },
   /* Composite class part */
  {
    /* geometry_manager	     */	GeometryManager,
    /* change_managed	     */	ChangeManaged,
    /* insert_child	     */	XtInheritInsertChild,
    /* delete_child	     */	XtInheritDeleteChild,
    /* extension	     */	NULL,
  },
   /* Constraint class part */
  {
    /* resources	     */ NULL,
    /* num_resources	     */ 0,
    /* constraint_size	     */ 0,
    /* initialize	     */ NULL,
    /* destroy		     */ NULL,
    /* set_values	     */ NULL,
    /* extension	     */ NULL,
  },
   /* Manager class part */
  {
    /* translations	     */ XtInheritTranslations,
    /* syn_resources	     */ NULL,
    /* num_syn_resources     */ 0,
    /* syn_constraint_resources */ NULL,
    /* num_syn_constraint_resources */ 0,
    /* parent_process	     */ XmInheritParentProcess,
    /* extension	     */ NULL,
  },
   /* FontSelectionBox class part */
  {
    /* set_font_name	     */	SetFontName,
    /* set_font_family_face  */	SetFontFamilyFace,
    /* set_font_size	     */	SetFontSize,
    /* refresh_font_list     */ RefreshFontList,
    /* get_family_list	     */	GetFamilyList,
    /* get_face_list	     */	GetFaceList,
    /* undef_unused_fonts    */ UndefUnusedFonts,
    /* download_font_name    */ DownloadFontName,
    /* match_font_face	     */ MatchFontFace,
    /* font_name_to_family_face */ FontNameToFamilyFace,
    /* font_family_face_to_name */ FontFamilyFaceToName,
    /* find_afm		     */ FindAFM,
    /* find_font_file	     */ FindFontFile,
    /* get_text_dimensions   */ GetTextDimensions,			
    /* extension	     */	NULL,
  }
};

WidgetClass fontSelectionBoxWidgetClass =
	(WidgetClass) &fontSelectionBoxClassRec;

/* ARGSUSED */

static Boolean CvtStringToFloatList(dpy, args, num_args, from, to, data)
    Display *dpy;
    XrmValuePtr args;
    Cardinal *num_args;
    XrmValuePtr from, to;
    XtPointer *data;
{
    register int i, count = 1;
    register char *ch, *start = from->addr;
    static float *list;
    char save;

    if (*num_args != 0) {	/* Check for correct number */
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
	       "cvtStringToFloatList", "wrongParameters",
	       "XtToolkitError",
	       "String to integer list conversion needs no extra arguments",
	       (String *) NULL, (Cardinal *) NULL);
    }

    if (to->addr != NULL && to->size < sizeof(int *)) {
	to->size = sizeof(int *);
	return FALSE;
    }
    if (start == NULL || *start == '\0') list = NULL;
    else {
	for (ch = start; *ch != '\0'; ch++) {    /* Count floats */
	    if (!isdigit(*ch) && *ch != '.' && *ch != ',') {
		XtDisplayStringConversionWarning(dpy, from->addr, "FloatList");
		return FALSE;
	    }
	    if (*ch == ',') count++;
	}
	list = (float *) XtCalloc(count+1, sizeof(float));

	for (i = 0; i < count; i++) {
	    for (ch = start; *ch != ',' && *ch != '\0'; ch++) {}
	    save = *ch;
	    *ch = '\0';
	    list[i] = atof(start);
	    *ch = save;
	    start = ch + 1;
	}
    }
    if (to->addr == NULL) to->addr = (caddr_t) &list;
    else *(float **) to->addr = list;
    to->size = sizeof(int *);
    return TRUE;
}

/* ARGSUSED */

static void FloatListDestructor(app, to, converter_data, args, num_args)
    XtAppContext app;
    XrmValuePtr to;
    XtPointer converter_data;
    XrmValuePtr args;
    Cardinal *num_args;
{
    float *list = (float *) to->addr;

    if (list == NULL) return;
    XtFree((XtPointer) list);
}

static void ClassInitialize()
{
    /* Register a converter for string to int list */

    XtSetTypeConverter(XtRString, XtRFloatList,
	    CvtStringToFloatList, (XtConvertArgList) NULL, 0,
	    XtCacheAll | XtCacheRefCount, FloatListDestructor);

    CSempty = CS("");
}

static void ClassPartInitialize(widget_class)
    WidgetClass widget_class;
{
    register FontSelectionBoxWidgetClass wc =
	    (FontSelectionBoxWidgetClass) widget_class;
    FontSelectionBoxWidgetClass super =
	    (FontSelectionBoxWidgetClass) wc->core_class.superclass;

    if (wc->fsb_class.set_font_name == InheritSetFontName) {
	wc->fsb_class.set_font_name = super->fsb_class.set_font_name;
    }
    if (wc->fsb_class.set_font_family_face == InheritSetFontFamilyFace) {
	wc->fsb_class.set_font_family_face =
		super->fsb_class.set_font_family_face;
    }
    if (wc->fsb_class.set_font_size == InheritSetFontSize) {
	wc->fsb_class.set_font_size = super->fsb_class.set_font_size;
    }
    if (wc->fsb_class.refresh_font_list == InheritRefreshFontList) {
	wc->fsb_class.refresh_font_list = super->fsb_class.refresh_font_list;
    }
    if (wc->fsb_class.get_family_list == InheritGetFamilyList) {
	wc->fsb_class.get_family_list = super->fsb_class.get_family_list;
    }
    if (wc->fsb_class.get_face_list == InheritGetFaceList) {
	wc->fsb_class.get_face_list = super->fsb_class.get_face_list;
    }
    if (wc->fsb_class.undef_unused_fonts == InheritUndefUnusedFonts) {
	wc->fsb_class.undef_unused_fonts = super->fsb_class.undef_unused_fonts;
    }
    if (wc->fsb_class.download_font_name == InheritDownloadFontName) {
	wc->fsb_class.download_font_name = super->fsb_class.download_font_name;
    }
    if (wc->fsb_class.match_font_face == InheritMatchFontFace) {
	wc->fsb_class.match_font_face = super->fsb_class.match_font_face;
    }
    if (wc->fsb_class.font_name_to_family_face ==
						InheritFontNameToFamilyFace) {
	wc->fsb_class.font_name_to_family_face =
		super->fsb_class.font_name_to_family_face;
    }
    if (wc->fsb_class.font_family_face_to_name ==
						InheritFontFamilyFaceToName) {
	wc->fsb_class.font_family_face_to_name =
		super->fsb_class.font_family_face_to_name;
    }
    if (wc->fsb_class.find_afm == InheritFindAFM) {
	wc->fsb_class.find_afm = super->fsb_class.find_afm;
    }
    if (wc->fsb_class.find_font_file == InheritFindFontFile) {
	wc->fsb_class.find_font_file = super->fsb_class.find_font_file;
    }
    if (wc->fsb_class.get_text_dimensions == InheritGetTextDimensions) {
	wc->fsb_class.get_text_dimensions =
		super->fsb_class.get_text_dimensions;
    }
}

static String bugFamilies[] = {
    "Berkeley", "CaslonFiveForty", "CaslonThree", "GaramondThree",
    "Music", "TimesTen", NULL
};

static String fixedFamilies[] = {
    "ITC Berkeley Oldstyle", "Caslon 540", "Caslon 3", "Garamond 3",
    "Sonata", "Times 10", NULL
};

static String missingFoundries[] = {
    "Berthold ", "ITC ", "Linotype ", NULL
};

static int missingFoundryLen[] = {
    9, 4, 9, 0
};

/* I wish we didn't have to do this! */

static void MungeFontNames(name, family, fullname, weight,
			   familyReturn, fullnameReturn, faceReturn)
    String name, family, fullname, weight;
    String *familyReturn, *fullnameReturn, *faceReturn;
{
    register char *src, *dst, prev;
    char buf[256];
    int digits = 0;
    int i, diff;
    static Bool inited = FALSE;
    static String FetteFrakturDfr, LinotextDfr;

    /* Don't make bugFamilies canonical; we'd have to make the initial
       family canonical to do anything with it and there's no point in that */

    if (!inited) {
	for (i = 0; fixedFamilies[i] != NULL; i++) {
	    fixedFamilies[i] = Canonical(fixedFamilies[i]);
	}
	FetteFrakturDfr = Canonical("FetteFraktur-Dfr");
	LinotextDfr = Canonical("Linotext-Dfr");
	inited = TRUE;
    }

    /* Copy the fullname into buf, enforcing one space between words.
       Eliminate leading digits and spaces, ignore asterisks, if the
       full name ends with 5 digits strip them, and replace periods that
       aren't followed by a space with a space.  If leading digits are
       followed by " pt " skip that too. */

    dst = buf;
    prev = ' ';
    src = fullname; 
    while (isdigit(*src)) src++;
    while (*src == ' ' || *src == '\t') src++;
    if (strncmp(src, "pt ", 3) == 0) src += 3;
    else if (strncmp(src, "pt. ", 4) == 0) src += 4;

    while (*src != '\0') {
	if (*src == '*') {
	    src++;
	    continue;
	}

	if (*src == '.') {
	    if (*(src+1) != ' ') {
		prev = *dst++ = ' ';
	    } else prev = *dst++ = '.';
	    src++;
	    continue;
	}

	if (isdigit(*src)) digits++;
	else digits = 0;

	if (isupper(*src)) {
	    if (prev != ' ' && (islower(*(src+1)) || islower(prev))) {
		*dst++ = ' ';
		prev = *dst++ = *src++;
	    } else prev = *dst++ = *src++;

	} else if (*src == ' ' || *src == '\t') {
	    if (prev == ' ') {
		src++;
		continue;
	    }
	    prev = *dst++ = ' ';
	    src++;

	} else prev = *dst++ = *src++;
    }

    if (digits == 5) {
	dst -= 5;
    }
    if (dst > buf && *(dst-1) == ' ') dst--;

    *dst = '\0';

    /* Stupid Fette Fraktur */

    if (name == FetteFrakturDfr) {
	strcat(buf, " Black Dfr");
    } else if (name == LinotextDfr) {
	strcat(buf, " Dfr");
    }

    if (strncmp(fullname, "pt ", 3) == 0) {
	src = buf + 2;
	while (*++src != '\0') *(src-3) = *src;
	*(src-3) = '\0';
    }
    *fullnameReturn = XtNewString(buf);

    /* From here on fullname should not be used */

    /* Done with the full name; now onto the family */

    for (i = 0; bugFamilies[i] != NULL; i++) {
	diff = strcmp(family, bugFamilies[i]);
	if (diff < 0) break;
	if (diff == 0) {
	    *familyReturn = fixedFamilies[i];
	    goto FAMILY_DONE;
	}
    }

    /* Copy the family into buf, enforcing one space between words */

    dst = buf;
    prev = ' ';
    src = family; 

    while (*src != '\0') {
	if (isupper(*src)) {
	    if (prev != ' ' && (islower(*(src+1)) || islower(prev))) {
		*dst++ = ' ';
		prev = *dst++ = *src++;
	    } else prev = *dst++ = *src++;

	} else if (*src == ' ' || *src == '\t') {
	    if (prev == ' ') {
		src++;
		continue;
	    }
	    prev = *dst++ = ' ';
	    src++;

	} else prev = *dst++ = *src++;
    }

    if (dst > buf && *(dst-1) == ' ') dst--;
    *dst = '\0';

    /* Compensate for fonts with foundries in the full name but not the
       family name by adding to the family name */
 
    for (i = 0; missingFoundries[i] != NULL; i++) {
	diff = strncmp(*fullnameReturn, missingFoundries[i],
		       missingFoundryLen[i]);
	if (diff > 0) continue;
	if (diff == 0 && strncmp(buf, missingFoundries[i],
		       missingFoundryLen[i] != 0)) {
	    while (dst >= buf) {
		*(dst+missingFoundryLen[i]) = *dst;
		dst--;
	    }
	    strncpy(buf, missingFoundries[i], missingFoundryLen[i]);
	}
	break;
    }

    /* From here on dst no longer points to the end of the buffer */

    /* Stupid Helvetica Rounded! */

    if (strncmp(*fullnameReturn, "Helvetica Rounded ", 18) == 0) {
	strcat(buf, " Rounded");
    }

    *familyReturn = Canonical(buf);

    /* From here on family should not be used */

FAMILY_DONE:

    /* Now to find the face in all this */

    src = *fullnameReturn;
    dst = *familyReturn;
    while (*dst == *src && *dst != '\0') {
        src++;
        dst++;
    }
    if (*src == ' ') src++;

    if (*src != '\0') *faceReturn = Canonical(src);
    else if (*weight != '\0') {
	if (islower(weight[0])) weight[0] = toupper(weight[0]);
	*faceReturn = Canonical(weight);
    } else *faceReturn = Canonical("Medium");
}

static String strip[] = {
    "Adobe ", "Bauer ", "Berthold ", "ITC ", "Linotype ",
    "New ", "Simoncini ", "Stempel ", NULL};

static int striplen[] = {6, 6, 9, 4, 9, 4, 10, 8, 0};

#define STEMPELINDEX 7

static Boolean CreateSortKey(family, key)
    register String family, key;
{
    char newkey[256];
    int len = strlen(family);
    register int i, diff;

    if (family[len-2] == 'P' && family[len-1] == 'i') {
	key[0] = 'P';
	key[1] = 'i';
	key[2] = ' ';
	strcpy(key+3, family);
	key[len] = '\0';
	return TRUE;
    }

    for (i = 0; strip[i] != NULL; i++) {
	diff = strncmp(family, strip[i], striplen[i]);
	if (diff < 0) break;
	if (diff == 0) {
	    if (i == STEMPELINDEX) {
		if (strcmp(family, "Stempel Schneidler") == 0) break;
	    }
	    strcpy(key, family + striplen[i]);
	    key[len - striplen[i]] = ' ';
	    strcpy(key + len - striplen[i] + 1, strip[i]);
	    key[len] = '\0';
	    if (CreateSortKey(key, newkey)) strcpy(key, newkey);
	    return TRUE;
	}
    }
    strcpy(key, family);
    return FALSE;
}

static void AddFontRecord(fsb, serverNum, name, family,
			  fullname, weight, resident)
    FontSelectionBoxWidget fsb;
    int serverNum;
    String name, family, fullname, weight;
    Boolean resident;
{
    FontFamilyRec *ff;
    FontRec *f;
    String familyReturn, fullnameReturn, faceReturn;
    char key[256];

    name = Canonical(name);

    /* First see if it's there already */

    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	for (f = ff->fonts; f != NULL; f = f->next) {
	    if (f->font_name == name) {
		if (!f->resident && resident) f->resident = TRUE;
		return;
	    }
	}
    }

    /* We believe that names gotten from PS resource files have been
       pre-munged, so no need to do it again */

    if (resident) {
	/* Have to get the info from the server */
	_DPSFGetFontInfo(fsb->fsb.context, serverNum, family, fullname,
			 weight);

	/* Deal with fonts that don't have useful information */

	if (family[0] == '\0') {
	    if (fullname[0] == '\0') {
		strcpy(family, name);
		strcpy(fullname, name);
	    } else strcpy(family, fullname);
	} else if (fullname[0] == '\0') strcpy(fullname, family);

	MungeFontNames(name, family, fullname, weight,
		       &familyReturn, &fullnameReturn, &faceReturn);
    } else {
	familyReturn = Canonical(family);
	fullnameReturn = XtNewString(fullname);
	faceReturn = Canonical(weight);
    }

    /* We didn't get an exact match, go for family match */

    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	if (ff->family_name == familyReturn) break;
    }

    if (ff == NULL) {
	ff = (FontFamilyRec *) XtMalloc(sizeof(FontFamilyRec));
	ff->next = fsb->fsb.known_families;
	ff->family_name = familyReturn;;
	ff->CS_family_name = CS(familyReturn);
	ff->fonts = NULL;
	ff->font_count = 0;
	(void) CreateSortKey(familyReturn, key);
	ff->sort_key = XtNewString(key);
	fsb->fsb.known_families = ff;
	fsb->fsb.family_count++;
    }

    f = (FontRec *) XtMalloc(sizeof(FontRec));
    f->next = ff->fonts;
    f->font_name = name;
    f->full_name = fullnameReturn;
    f->resident = resident;
    f->temp_resident = FALSE;
    f->pending_delete_next = NULL;
    f->face_name = faceReturn;
    f->CS_face_name = CS(f->face_name);
    ff->fonts = f;
    ff->font_count++;
}

static void SortFontNames(ff)
    FontFamilyRec *ff;
{
    FontRec *f, *highest, **prev, **highestPrev;
    FontRec *newFontList = NULL;

    while (ff->fonts != NULL) {
	prev = highestPrev = &ff->fonts;
	highest = ff->fonts;

	for (f = ff->fonts->next; f != NULL; f = f->next) {
	    prev = &(*prev)->next;
	    if (strcmp(f->face_name, highest->face_name) > 0) {
		highest = f;
		highestPrev = prev;
	    }
	}

	*highestPrev = highest->next;
	highest->next = newFontList;
	newFontList = highest;
    }
    ff->fonts = newFontList;
}

static void SortFontFamilies(fsb)
    FontSelectionBoxWidget fsb;
{
    FontFamilyRec *ff, *highest, **prev, **highestPrev;
    FontFamilyRec *newFamilyList = NULL;

    while (fsb->fsb.known_families != NULL) {
	prev = highestPrev = &fsb->fsb.known_families;
	highest = fsb->fsb.known_families;

	for (ff = fsb->fsb.known_families->next; ff != NULL; ff = ff->next) {
	    prev = &(*prev)->next;
	    if (strcmp(ff->sort_key, highest->sort_key) > 0) {
		highest = ff;
		highestPrev = prev;
	    }
	}

	*highestPrev = highest->next;
	highest->next = newFamilyList;
	newFamilyList = highest;
	SortFontNames(highest);
	XtFree(highest->sort_key);
	highest->sort_key = NULL;
    }
    fsb->fsb.known_families = newFamilyList;
}

static void AddFamily(fsb, family, fonts, weight, fullname, name)
    FontSelectionBoxWidget fsb;
    char *family, *fonts, *weight, *fullname, *name;
{
    int j;
    char *ch;

    ch = fonts;
    while (*ch != '\0') {
	j = 0;
	while (1) {
            if (*ch == '\\' && (*(ch+1) == '\\' || *(ch+1) == ',')) {
		ch++;
		weight[j++] = *ch++;
	    } else if (*ch == '\0' || *ch == ',') {
		weight[j] = '\0';
		break;
	    } else weight[j++] = *ch++;
	}
	if (*ch == ',') {
	    j = 0;
	    ch++;
	    while (1) {
		if (*ch == '\\' && (*(ch+1) == '\\' || *(ch+1) == ',')) {
		    ch++;
		    name[j++] = *ch++;
		} else if (*ch == '\0' || *ch == ',') {
		    name[j] = '\0';
		    break;
	        } else name[j++] = *ch++;
	    }
	    strcpy(fullname, family);
	    strcat(fullname, " ");
	    strcat(fullname, weight);
	    AddFontRecord(fsb, 0, name, family, fullname, weight, FALSE);
	    if (*ch == ',') ch++;
	}
    }
}

static void GetFontNames(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    char name[256], family[256], fullname[256], weight[256];
    char *buffer, *ch, *start;
    int fontCount, totalLength;
    char **loadableFamilies, **loadableFamilyFonts;
    
    fsb->fsb.family_count = 0;

    fontCount = ListPSResourceFiles(fsb->fsb.resource_path_override,
				    fsb->fsb.default_resource_path,
				    PSResFontFamily, NULL,
				    &loadableFamilies, &loadableFamilyFonts);
    for (i = 0; i < fontCount; i++) {
        AddFamily(fsb, loadableFamilies[i], loadableFamilyFonts[i],
		  weight, fullname, name);
    }

    XtFree((XtPointer) loadableFamilies);
    XtFree((XtPointer) loadableFamilyFonts);
    FreePSResourceStorage(False);

    if (fsb->fsb.get_server_fonts) {
	_DPSFEnumFonts(fsb->fsb.context, &fontCount, &totalLength);

	buffer = XtMalloc(totalLength);
	_DPSFGetAllFontNames(fsb->fsb.context, fontCount, totalLength, buffer);
	ch = start = buffer;
	for (i = 0; i < fontCount; i++) {
	    while (*ch != ' ') ch++;
	    *ch = '\0';
	    AddFontRecord(fsb, i, start, family, fullname, weight, TRUE);
	    start = ch+1;
	}
	XtFree(buffer);
    }

    SortFontFamilies(fsb);
}

static void ManageFamilyMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];

    XtManageChild(fsb->fsb.family_multiple_label_child);

    i = 0;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.family_multiple_label_child);i++;
    XtSetValues(XtParent(fsb->fsb.family_scrolled_list_child), args, i);
}

static void ManageFaceMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];

    XtManageChild(fsb->fsb.face_multiple_label_child);

    i = 0;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.face_multiple_label_child);i++;
    XtSetValues(XtParent(fsb->fsb.face_scrolled_list_child), args, i);
}

static void ManageSizeMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    XtManageChild(fsb->fsb.size_multiple_label_child);
}

static void UnmanageFamilyMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];

    i = 0;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.family_label_child);	i++;
    XtSetValues(XtParent(fsb->fsb.family_scrolled_list_child), args, i);

    XtUnmanageChild(fsb->fsb.family_multiple_label_child);
}

static void UnmanageFaceMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];

    i = 0;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.face_label_child);		i++;
    XtSetValues(XtParent(fsb->fsb.face_scrolled_list_child), args, i);

    XtUnmanageChild(fsb->fsb.face_multiple_label_child);
}

static void UnmanageSizeMultiple(fsb)
    FontSelectionBoxWidget fsb;
{
    XtUnmanageChild(fsb->fsb.size_multiple_label_child);
}

/* Callbacks for subwidgets */

static Boolean DownloadFont(fsb, name, ctxt, make_shared)
    FontSelectionBoxWidget fsb;
    String name;
    DPSContext ctxt;
    Boolean make_shared;
{
    int count;
    char **names, **files;
    FILE *f;
#define BUFLEN 256
    char buf[BUFLEN];
#define RESETLEN 11

    /* Assume context is correct */

    count = ListPSResourceFiles(fsb->fsb.resource_path_override,
				fsb->fsb.default_resource_path,
				PSResFontOutline, name,
				&names, &files);
    if (count == 0) return FALSE;

    f = fopen(files[0], "r");
    if (f == NULL) return FALSE;

    DPSPrintf(ctxt, "\ncurrentshared %s setshared\n",
	      (make_shared ? "true" : "false"));
    while (fgets(buf, BUFLEN, f) != NULL) {
	DPSWritePostScript(ctxt, buf, strlen(buf));
    }
    DPSWritePostScript(ctxt, "\nsetshared\n", RESETLEN);
    fclose (f);
    free(names);
    free(files);
    return TRUE;

#undef RESETLEN
#undef SETLEN
}

static void UndefSomeUnusedFonts(fsb, all)
    FontSelectionBoxWidget fsb;
    Boolean all;
{
    FontRec *f, *nextf, **start;
    int i;

    if (!all && fsb->fsb.pending_delete_count < fsb->fsb.max_pending_deletes) {
	return;
    }

    if (all) start = &fsb->fsb.pending_delete_font;
    else {
	/* Skip to the end of the ones we're keeping */
	f = fsb->fsb.pending_delete_font;
	for (i = 1; f != NULL && i < fsb->fsb.max_pending_deletes; i++) {
	    f = f->pending_delete_next;
	}
	if (f == NULL) return;
	start = &f->pending_delete_next;
    }

    for (f = *start; f != NULL; f = nextf) {
	nextf = f->pending_delete_next;
	if (f == fsb->fsb.currently_previewed) {
	    start = &f->pending_delete_next;
	    continue;
	}
	*start = nextf;
	if (!f->resident) _DPSFUndefineFont(fsb->fsb.context, f->font_name);
	f->temp_resident = FALSE;
	fsb->fsb.pending_delete_count--;	    
	f->pending_delete_next = NULL;
    }
}

static void UndefUnusedFonts(fsb)
    FontSelectionBoxWidget fsb;
{
    UndefSomeUnusedFonts(fsb, TRUE);
}

void _FSBDownloadFontIfNecessary(f, fsb)
    FontRec *f;
    FontSelectionBoxWidget fsb;
{
    Boolean shared;

    if (!f->resident && !f->temp_resident) {
	shared = fsb->fsb.make_fonts_shared && !fsb->fsb.undef_unused_fonts;
	(void) DownloadFont(fsb, f->font_name, fsb->fsb.context, shared);
	if (shared) f->resident = TRUE;
	else f->temp_resident = TRUE;

	if (f->pending_delete_next == NULL && fsb->fsb.undef_unused_fonts) {
	    f->pending_delete_next = fsb->fsb.pending_delete_font;
	    fsb->fsb.pending_delete_font = f;
	    fsb->fsb.pending_delete_count++;
	    UndefSomeUnusedFonts(fsb, FALSE);
	}
    }
}

static void DoPreview(fsb, override)
    FontSelectionBoxWidget fsb;
    Boolean override;
{
    int i;
    Arg args[20];
    int *selectList, selectCount;
    float size;
    FontFamilyRec *ff = fsb->fsb.known_families;
    FontRec *f;
    char *chSize;
    Dimension height;
    Cardinal depth;

    if (!XtIsRealized(fsb)) return;

    i = 0;
    XtSetArg(args[i], XmNheight, &height);				i++;
    XtSetArg(args[i], XmNdepth, &depth);				i++;
    XtGetValues(fsb->fsb.preview_child, args, i);

    if (fsb->fsb.gstate == 0) {
	XDPSSetContextParameters(fsb->fsb.context, XtScreen(fsb), depth,
				 XtWindow(fsb->fsb.preview_child), height,
				 (XDPSStandardColormap *) NULL,
				 (XDPSStandardColormap *) NULL,
				 XDPSContextScreenDepth | XDPSContextDrawable |
				 XDPSContextRGBMap | XDPSContextGrayMap);
	XDPSCaptureContextGState(fsb->fsb.context, &fsb->fsb.gstate);
    } else XDPSSetContextGState(fsb->fsb.context, fsb->fsb.gstate);

    _DPSFClearWindow(fsb->fsb.context);

    if (override) {
	f = fsb->fsb.currently_previewed;
	size = fsb->fsb.currently_previewed_size;
    }

    if (!override || f == NULL || size == 0.0) {
	if (!XmListGetSelectedPos(fsb->fsb.family_scrolled_list_child,
				  &selectList, &selectCount)) return;
	if (selectCount == 0 ||
	    *selectList < 1 || *selectList > fsb->fsb.family_count) return;

	for (i = 1; i < *selectList; i++) ff = ff->next;
    
	XtFree((XtPointer) selectList);

	if (!XmListGetSelectedPos(fsb->fsb.face_scrolled_list_child,
				  &selectList, &selectCount)) return;
	if (selectCount == 0 ||
	    *selectList < 1 || *selectList > ff->font_count) return;
    
	f = ff->fonts;
	for (i = 1; i < *selectList; i++) f = f->next;
    
	XtFree((XtPointer) selectList);

	i = 0;
	XtSetArg(args[i], XmNvalue, &chSize);				i++;
	XtGetValues(fsb->fsb.size_text_field_child, args, i);

	if (chSize == NULL || *chSize == '\0') return;
	size = atof(chSize);
    }

    if (size <= 0.0) return;

    fsb->fsb.currently_previewed = f;
    fsb->fsb.currently_previewed_size = size;

    _FSBDownloadFontIfNecessary(f, fsb);

    if (fsb->fsb.preview_string == NULL) {
	_DPSFPreviewString(fsb->fsb.context, f->font_name, size,
			   f->full_name, height);
    } else _DPSFPreviewString(fsb->fsb.context, f->font_name, size,
			      fsb->fsb.preview_string, height);
}

/* ARGSUSED */

static void PreviewText(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb = 
	    (FontSelectionBoxWidget) XtParent(XtParent(widget));
    XmAnyCallbackStruct *cb = (XmAnyCallbackStruct *) callData;

    if (!fsb->fsb.preview_fixed) {
	XSetWindowAttributes att;
	att.bit_gravity = ForgetGravity;
	XChangeWindowAttributes(XtDisplay(fsb),
				XtWindow(fsb->fsb.preview_child),
				CWBitGravity, &att);
	fsb->fsb.preview_fixed = TRUE;
    }

    if (cb != NULL && cb->event->type == Expose &&
	cb->event->xexpose.count != 0) return;

    DoPreview(fsb, TRUE);
}

/* ARGSUSED */

static void PreviewCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb = 
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    DoPreview(fsb, FALSE);
}

/* ARGSUSED */

static void DismissCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) clientData;

    fsb->fsb.show_sampler = FALSE;
}

static void ShowSampler(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];
    Widget s;

    if (fsb->fsb.sampler == NULL) {
	FSBCreateSamplerCallbackRec cs;

	cs.sampler_shell = NULL;

	XtCallCallbackList((Widget) fsb, fsb->fsb.create_sampler_callback,
			   (XtPointer) &cs);

	if (cs.sampler_shell == NULL || cs.sampler == NULL) {
	    fsb->fsb.sampler =
		    XtCreatePopupShell("samplerShell",
				       transientShellWidgetClass,	
				       (Widget) fsb, (ArgList) NULL, 0);
	    i = 0;
	    XtSetArg(args[i], XtNfontSelectionBox, fsb);		i++;
	    s = XtCreateManagedWidget("sampler", fontSamplerWidgetClass,
				      fsb->fsb.sampler, args, i);
	    XtAddCallback(s, XtNdismissCallback,
			  DismissCallback, (XtPointer) fsb);
	} else {
	    fsb->fsb.sampler = cs.sampler_shell;
	    XtAddCallback(cs.sampler, XtNdismissCallback,
			  DismissCallback, (XtPointer) fsb);
	}
    }
    XtPopup(fsb->fsb.sampler, XtGrabNone);
    XRaiseWindow(XtDisplay(fsb->fsb.sampler), XtWindow(fsb->fsb.sampler));
    fsb->fsb.show_sampler = TRUE;
}

/* ARGSUSED */

static void ShowSamplerCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb = 
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    ShowSampler(fsb);
}
/* ARGSUSED */

static void PreviewDoubleClick(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb = 
	    (FontSelectionBoxWidget)
		    XtParent(XtParent(XtParent(XtParent(widget))));

    DoPreview(fsb, FALSE);
}

/* ARGSUSED */

static void ResizePreview(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    int i;
    Arg args[20];
    Dimension height;
    Cardinal depth;
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(widget));

    if (!XtIsRealized(widget) || fsb->fsb.gstate == 0) return;

    i = 0;
    XtSetArg(args[i], XmNheight, &height);				i++;
    XtSetArg(args[i], XmNdepth, &depth);				i++;
    XtGetValues(widget, args, i);

    XDPSSetContextGState(fsb->fsb.context, fsb->fsb.gstate);

    XDPSSetContextParameters(fsb->fsb.context, XtScreen(widget), depth,
			     XtWindow(widget), height,
			     (XDPSStandardColormap *) NULL,
			     (XDPSStandardColormap *) NULL,
			     XDPSContextScreenDepth | XDPSContextDrawable);

    _DPSFReclip(fsb->fsb.context);

    XDPSUpdateContextGState(fsb->fsb.context, fsb->fsb.gstate);
}

static String FindAFM(w, name)
    Widget w;
    String name;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    int count;
    char **names, **files;
    char *ret;

    if (name == NULL) return NULL;

    count = ListPSResourceFiles(fsb->fsb.resource_path_override,
				fsb->fsb.default_resource_path,
				PSResFontAFM,
				name,
				&names, &files);
    
    if (count == 0) return NULL;

    ret = files[0];
    free(names);
    free(files);
    return ret;
}

static String FindFontFile(w, name)
    Widget w;
    String name;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    int count;
    char **names, **files;
    char *ret;

    if (name == NULL) return NULL;

    count = ListPSResourceFiles(fsb->fsb.resource_path_override,
				fsb->fsb.default_resource_path,
				PSResFontOutline,
				name,
				&names, &files);
    
    if (count == 0) return NULL;

    ret = files[0];
    free(names);
    free(files);
    return ret;
}

static Boolean Verify(fsb, cb, afm)
    FontSelectionBoxWidget fsb;
    FSBValidateCallbackRec *cb;
    String afm;
{
    int i;
    Arg args[20];
    char *chSize;

    if (fsb->fsb.current_family_multiple) {
	cb->family = NULL;
	cb->family_selection = FSBMultiple;
    } else if (fsb->fsb.currently_selected_family == NULL) {
	cb->family = NULL;
	cb->family_selection = FSBNone;
    } else {
	cb->family = fsb->fsb.currently_selected_family->family_name;
	cb->family_selection = FSBOne;
    }

    if (fsb->fsb.current_face_multiple) {
	cb->face = NULL;
	cb->face_selection = FSBMultiple;
    } else if (fsb->fsb.currently_selected_face == NULL) {
	cb->face = NULL;
	cb->face_selection = FSBNone;
    } else {
	cb->face = fsb->fsb.currently_selected_face->face_name;
	cb->face_selection = FSBOne;
    }
	
    if (cb->family_selection == FSBMultiple ||
	cb->face_selection == FSBMultiple) {
	cb->name = NULL;
	cb->name_selection = FSBMultiple;
    } else if (fsb->fsb.currently_selected_face == NULL) {
	cb->name = NULL;
	cb->name_selection = FSBNone;
    } else {
	cb->name = fsb->fsb.currently_selected_face->font_name;
	cb->name_selection = FSBOne;
    }

    if (fsb->fsb.current_size_multiple) {
	cb->size = 0.0;
	cb->size_selection = FSBMultiple;
    } else {
	i = 0;
	XtSetArg(args[i], XmNvalue, &chSize);				i++;
	XtGetValues(fsb->fsb.size_text_field_child, args, i);

	if (chSize == NULL || *chSize == '\0') {
	    cb->size = 0.0;
	    cb->size_selection = FSBNone;
	} else {
	    cb->size = atof(chSize);
	    cb->size_selection = FSBOne;
	}
    }

    cb->afm_filename = afm;
    cb->afm_present = (afm != NULL);
    cb->doit = TRUE;

    XtCallCallbackList((Widget) fsb, fsb->fsb.validate_callback, cb);
    return cb->doit;
}

static Boolean VerifyAndCallback(fsb, reason, callback)
    FontSelectionBoxWidget fsb;
    FSBCallbackReason reason;
    XtCallbackList callback;
{
    String afm = NULL;
    FSBValidateCallbackRec cb;
    FontRec *fsave, *face;

    if (fsb->fsb.get_afm) {
	if (fsb->fsb.currently_selected_face == NULL) afm = NULL;
	else afm = FindAFM((Widget) fsb,
			   fsb->fsb.currently_selected_face->font_name);
    }

    DoPreview(fsb, FALSE);

    cb.reason = reason;
    if (!Verify(fsb, &cb, afm)) return FALSE;

    fsb->fsb.font_family_multiple = fsb->fsb.current_family_multiple;
    if (!fsb->fsb.font_family_multiple &&
	fsb->fsb.currently_selected_family != NULL) {
	fsb->fsb.font_family =
		fsb->fsb.currently_selected_family->family_name;
    } else fsb->fsb.font_family = NULL;

    fsb->fsb.font_face_multiple = fsb->fsb.current_face_multiple;
    if (!fsb->fsb.font_face_multiple &&
	fsb->fsb.currently_selected_face != NULL) {
	fsb->fsb.font_face = fsb->fsb.currently_selected_face->face_name;
    } else fsb->fsb.font_face = NULL;

    fsb->fsb.font_name_multiple =
	    fsb->fsb.font_family_multiple || fsb->fsb.font_face_multiple;
    if (!fsb->fsb.font_name_multiple &&
	fsb->fsb.currently_selected_face != NULL) {
	fsb->fsb.font_name = fsb->fsb.currently_selected_face->font_name;
    } else fsb->fsb.font_name = NULL;

    fsb->fsb.font_size_multiple = fsb->fsb.current_size_multiple;
    if (!fsb->fsb.font_size_multiple) {
	fsb->fsb.font_size = cb.size;
    }

    if (fsb->fsb.undef_unused_fonts) {
	fsave = fsb->fsb.currently_previewed;
	if (fsb->fsb.make_fonts_shared) {
	    fsb->fsb.currently_previewed = NULL;
	}
	UndefUnusedFonts(fsb);
	fsb->fsb.currently_previewed = fsave;
	face = fsb->fsb.currently_selected_face;
	if (face != NULL && !face->resident) {
	    face->resident = TRUE;
	    if (fsb->fsb.make_fonts_shared) {
		(void) DownloadFont(fsb, face->font_name,
				    fsb->fsb.context, TRUE);
		/* If making it shared, be sure to synchronize with
		   the caller who might be using a different context */
		DPSWaitContext(fsb->fsb.context);
	    }
	}
    }

    XtCallCallbackList((Widget) fsb, callback, &cb);
    return TRUE;
}

/* ARGSUSED */

static void OKCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    if (!VerifyAndCallback(fsb, FSBOK, fsb->fsb.ok_callback)) return;
    if (XtIsShell(XtParent(fsb))) XtPopdown(XtParent(fsb));
    if (fsb->fsb.show_sampler) XtPopdown(fsb->fsb.sampler);
}

/* ARGSUSED */

static void ApplyCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    (void) VerifyAndCallback(fsb, FSBApply, fsb->fsb.apply_callback);
}

static void ResetFSB(fsb, reason)
    FontSelectionBoxWidget fsb;
    FSBCallbackReason reason;
{
    FSBCallbackRec cb;
    static void SetUpCurrentSelections();

    fsb->fsb.currently_previewed = NULL;
    fsb->fsb.currently_previewed_size = fsb->fsb.currently_selected_size = 0.0;
    SetUpCurrentSelections(fsb);
    if (fsb->fsb.undef_unused_fonts) UndefUnusedFonts(fsb);

    cb.reason = reason;
    if (fsb->fsb.font_family_multiple) {
	cb.family = NULL;
	cb.family_selection = FSBMultiple;
    } else if (fsb->fsb.font_family == NULL) {
	cb.family = NULL;
	cb.family_selection = FSBNone;
    } else {
	cb.family = fsb->fsb.font_family;
	cb.family_selection = FSBOne;
    }

    if (fsb->fsb.font_face_multiple) {
	cb.face = NULL;
	cb.face_selection = FSBMultiple;
    } else if (fsb->fsb.font_face == NULL) {
	cb.face = NULL;
	cb.face_selection = FSBNone;
    } else {
	cb.face = fsb->fsb.font_face;
	cb.face_selection = FSBOne;
    }
	
    if (cb.family_selection == FSBMultiple ||
	cb.face_selection == FSBMultiple) {
	cb.name = NULL;
	cb.name_selection = FSBMultiple;
    } else if (fsb->fsb.font_face == NULL) {
	cb.name = NULL;
	cb.name_selection = FSBNone;
    } else {
	cb.name = fsb->fsb.font_name;
	cb.name_selection = FSBOne;
    }

    if (fsb->fsb.font_size_multiple) {
	cb.size = 0.0;
	cb.size_selection = FSBMultiple;
    } else {
	cb.size = fsb->fsb.font_size;
	cb.size_selection = FSBOne;
    }

    cb.afm_filename = NULL;
    cb.afm_present = FALSE;

    if (reason == FSBReset) {
	XtCallCallbackList((Widget) fsb, fsb->fsb.reset_callback, &cb);
    } else XtCallCallbackList((Widget) fsb, fsb->fsb.cancel_callback, &cb);
}

/* ARGSUSED */

static void ResetCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    ResetFSB(fsb, FSBReset);
}

/* ARGSUSED */

static void CancelCallback(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));

    ResetFSB(fsb, FSBCancel);
    if (XtIsShell(XtParent(fsb))) XtPopdown(XtParent(fsb));
    if (fsb->fsb.show_sampler) XtPopdown(fsb->fsb.sampler);
}

/* ARGSUSED */

static void SizeSelect(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    int i;
    Arg args[20];
    String value;
    Widget option;
    FontSelectionBoxWidget fsb = 
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));
    char *ch;

    i = 0;
    XtSetArg(args[i], XmNvalue, &value);				i++;
    XtGetValues(widget, args, i);

    if (value == NULL) option = fsb->fsb.other_size;
    else {
	if (value[0] != '\0' && fsb->fsb.current_size_multiple) {
	    fsb->fsb.current_size_multiple = FALSE;
	    UnmanageSizeMultiple(fsb);
	}
	for (ch = value; *ch != '\0'; ch++) if (*ch == '.') *ch = '-';

	option = XtNameToWidget(fsb->fsb.size_menu, value);
	if (option == NULL) option = fsb->fsb.other_size;
    }

    i = 0;
    XtSetArg(args[i], XmNmenuHistory, option);				i++;
    XtSetValues(fsb->fsb.size_option_menu_child, args, i);

    if (value != NULL && value[0] != '\0') {
	fsb->fsb.currently_selected_size = atof(value);
    } else fsb->fsb.currently_selected_size = 0.0;

    fsb->fsb.current_size_multiple = FALSE;
}

/* There's a problem; sometimes the change has already been made in the field,
   and sometimes it hasn't.  The times when it has seem to correspond to
   making changes with the size option menu, so we use this disgusting
   global flag to notice when this happens.  */

static Boolean changingSize = False;

/* ARGSUSED */

static void TextVerify(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    int i;
    Arg args[20];
    XmTextVerifyPtr v = (XmTextVerifyPtr) callData;
    char ch, *cp;
    int decimalPoints = 0;
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget) XtParent(XtParent(XtParent(widget)));
    
    if (changingSize) return;	/* We know what we're doing; allow it */

    /* Should probably look at format field, but seems to contain garbage */

    if (v->text->length == 0) return;

    if (v->text->length == 1) {
	ch = v->text->ptr[0];
	if (ch == 'p' || ch == 'P') {
	    XtCallCallbacks(fsb->fsb.preview_button_child,
			    XmNactivateCallback, NULL);
	    v->doit = False;
	    return;
	}
    }
	
    for (i = 0; i < v->text->length; i++) {
	ch = v->text->ptr[i];
	if (ch == '.') decimalPoints++;
	else if (!isdigit(ch)) {
	    v->doit = False;
	    return;
	}
    }

    if (decimalPoints > 1) {
	v->doit = False;
	return;
    }

    i = 0;
    XtSetArg(args[i], XmNvalue, &cp);					i++;
    XtGetValues(widget, args, i);

    for (/**/; *cp != '\0'; cp++) {
	if (*cp == '.') decimalPoints++;
    }

    if (decimalPoints > 1) v->doit = False;
}

/* ARGSUSED */

static void SetSize(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    int i;
    Arg args[20];
    char buf[20];
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget)
	    XtParent(XtParent(XtParent(XtParent(XtParent(widget)))));
    float *size = (float *) clientData;

    if (fsb->fsb.current_size_multiple) {
	fsb->fsb.current_size_multiple = FALSE;
	UnmanageSizeMultiple(fsb);
    }

    (void) sprintf(buf, "%g", *size);

    changingSize = True;
    i = 0;
    XtSetArg(args[i], XmNvalue, buf);				i++;
    XtSetValues(fsb->fsb.size_text_field_child, args, i);
    changingSize = False;

    if (fsb->fsb.auto_preview) DoPreview(fsb, FALSE);
}

static void SetUpFaceList(fsb, ff)
    FontSelectionBoxWidget fsb;
    FontFamilyRec *ff;
{
    int i;
    Arg args[20];
    FontRec *f;
    XmString *CSfaces;

    CSfaces = (XmString *) XtCalloc(ff->font_count, sizeof(XmString));

    for (i = 0, f = ff->fonts; i < ff->font_count; i++, f = f->next) {
	CSfaces[i] = f->CS_face_name;
    }

    i = 0;
    XtSetArg(args[i], XmNitemCount, ff->font_count);			i++;
    XtSetArg(args[i], XmNitems, CSfaces);				i++;
    XtSetValues(fsb->fsb.face_scrolled_list_child, args, i);
    XtFree((XtPointer) CSfaces);
}

static String categories[][6] = {
    {"Regular", "Roman", "Medium", "Book", "Light", NULL},
    {"Italic", "Slanted", "Oblique", NULL},
    {"Demi", "Semibold", "Heavy", "Bold", NULL},
    {NULL},
};

#define NORMALINDEX 0][0
#define ITALICINDEX 1][0
#define BOLDINDEX 2][3
#define DEMIINDEX 2][0
#define LIGHTINDEX 0][4
#define BOOKINDEX 0][3

static String extraNormalFaces[] = {"Demi", "Semibold", NULL};

static int MatchFaceName(rec, gaveUp)
    FSBFaceSelectCallbackRec *rec;
    Boolean *gaveUp;
{
    int i, j, k, face;
#define PIECEMAX 10
    String pieces[PIECEMAX];
    int numPieces;
    int pass;
    char *ch, *start, *compare;
    char save;
    static Boolean categoriesInited = FALSE;
    static char *canonicalBold, *canonicalLight, *canonicalBook;

    *gaveUp = FALSE;

    if (!categoriesInited) {
	for (i = 0; categories[i][0] != NULL; i++) {
	    for (j = 0; categories[i][j] != NULL; j++) {
		categories[i][j] = Canonical(categories[i][j]);
	    }
	}
	for (i = 0; extraNormalFaces[i] != NULL; i++) {
	    extraNormalFaces[i] = Canonical(extraNormalFaces[i]);
	}
	canonicalBold = categories[BOLDINDEX];
	canonicalLight = categories[LIGHTINDEX];
	canonicalBook = categories[BOOKINDEX];
	categoriesInited = TRUE;
    }

    if (rec->current_face == NULL || rec->current_face[0] == '\0') {
	goto GIVE_UP;
    }

    /* First check for an exact match */

    for (i = 0; i < rec->num_available_faces; i++) {
	if (rec->available_faces[i] == rec->current_face) return i;
    }

    /* Try some category matching.  We make two passes; in the first pass
       we remove "Bold" from the "Demi" family and "Light" and "Book" from
       the "Regular" family; in the second pass we include them.  We ignore
       leading digits in the face name.  */

    categories[BOLDINDEX] = categories[LIGHTINDEX] =
	    categories[BOOKINDEX] = NULL;

    i = 0;
    ch = rec->current_face;
    while (*ch == ' ' || isdigit(*ch)) ch++;
    start = ch;

    while (1) {
	while (*ch != ' ' && *ch != '\0') ch++;
	save = *ch;
	*ch = '\0';
	compare = Canonical(start);
	for (j = 0; categories[j][0] != NULL; j++) {
	    for (k = 0; categories[j][k] != NULL; k++) {
		if (compare == categories[j][k]) {
		    pieces[i++] = categories[j][0];
		    goto FOUND_PIECE;
		}
	    }
	}
	pieces[i++] = compare;	/* A unique piece */
FOUND_PIECE:
	*ch = save;
	while (*ch == ' ') ch++;
	if (*ch == '\0') break;
	if (i >= PIECEMAX) goto GIVE_UP;
	start = ch;
    }
    numPieces = i;
    if (numPieces == 0) goto GIVE_UP;

    /* Special case starting with the italic category */

    if (pieces[0] == categories[ITALICINDEX] && numPieces < PIECEMAX-1) {
	for (i = numPieces; i > 0; i--) pieces[i] = pieces[i-1];
	pieces[0] = categories[NORMALINDEX];
	numPieces++;
    }

    for (pass = 0; pass < 2; pass++) {
	if (pass == 1) {
	    categories[BOLDINDEX] = canonicalBold;
	    categories[LIGHTINDEX] = canonicalLight;
	    categories[BOOKINDEX] = canonicalBook;
	    for (i = 0; i < numPieces; i++) {
		if (pieces[i] == canonicalBold) {
		    pieces[i] = categories[DEMIINDEX];
		} else if (pieces[i] == canonicalLight) {
		    pieces[i] = categories[NORMALINDEX];
		}  else if (pieces[i] == canonicalBook) {
		    pieces[i] = categories[NORMALINDEX];
		}
	    }
	}

	/* Now match against each face */

	for (face = 0; face < rec->num_available_faces; face++) {
	    i = 0;
	    ch = rec->available_faces[face];
	    while (*ch == ' ' || isdigit(*ch)) ch++;
	    start = ch;

	    while (1) {
		while (*ch != ' ' && *ch != '\0') ch++;
		save = *ch;
		*ch = '\0';
		compare = Canonical(start);
		for (j = 0; categories[j][0] != NULL; j++) {
		    for (k = 0; categories[j][k] != NULL; k++) {
			if (compare == categories[j][k]) {
			    compare = categories[j][0];
			    goto MATCH;
			}
		    }
		}
    MATCH:
		/* Special case matching the italic category again */

		if (i == 0 && compare == categories[ITALICINDEX] &&
		    pieces[0] == categories[NORMALINDEX] &&
		    pieces[1] == categories[ITALICINDEX]) i = 1;

		if (pieces[i] != compare) {
		    *ch = save;
		    goto NEXT_FACE;
		} else i++;

		*ch = save;
		while (*ch == ' ') ch++;
		if (*ch == '\0') break;
		if (i >= numPieces) goto NEXT_FACE;
		start = ch;
	    }
	    if (i == numPieces) return face;	/* Found a match! */
    NEXT_FACE:		
	    ;
	}
    }

    /* Couldn't find a match.  Look for a "normal face".  Make sure "Light"
       and "Book" are installed. Again, ignore leading spaces.  */
GIVE_UP:
    *gaveUp = TRUE;
    categories[LIGHTINDEX] = canonicalLight;
    categories[BOOKINDEX] = canonicalBook;

    for (i = 0; categories[0][i] != NULL; i++) {
	for (face = 0; face < rec->num_available_faces; face++) {
	    compare = rec->available_faces[face];
	    while (*compare == ' ' || isdigit(*compare)) compare++;
	    if (compare != rec->available_faces[face]) {
		compare = Canonical(compare);
	    }
	    if (categories[0][i] == compare) return face;
	}
    }

    for (i = 0; extraNormalFaces[i] != NULL; i++) {
	for (face = 0; face < rec->num_available_faces; face++) {
	    compare = rec->available_faces[face];
	    while (*compare == ' ' || isdigit(*compare)) compare++;
	    if (compare != rec->available_faces[face]) {
		compare = Canonical(compare);
	    }
	    if (extraNormalFaces[i] == compare) return face;
	}
    }
    
    /* Oh, well.  Use the first one */
    return 0;
}

/* This makes sure the selected item is visible */

static void ListSelectPos(w, pos, notify)
    Widget w;
    int pos;
    Boolean notify;
{
    Arg args[10];
    int i;
    int topPos, items, visible;

    XmListSelectPos(w, pos, notify);
    
    i = 0;
    XtSetArg(args[i], XmNtopItemPosition, &topPos);			i++;
    XtSetArg(args[i], XmNvisibleItemCount, &visible);			i++;
    XtSetArg(args[i], XmNitemCount, &items);				i++;
    XtGetValues(w, args, i);

    if (pos >= topPos && pos < topPos + visible) return;
    topPos = pos - (visible-1)/2;
    if (topPos + visible > items) topPos = items - visible + 1;
    if (topPos < 1) topPos = 1;

    i = 0;
    XtSetArg(args[i], XmNtopItemPosition, topPos);			i++;
    XtSetValues(w, args, i);
}

static void GetInitialFace(fsb, ff)
    FontSelectionBoxWidget fsb;
    FontFamilyRec *ff;
{
    FSBFaceSelectCallbackRec rec;
    String *faces;
    int i;
    FontRec *f;
    Boolean junk;

    faces = (String *) XtMalloc(ff->font_count * sizeof(String));
    i = 0;
    for (f = ff->fonts; f != NULL; f = f->next) faces[i++] = f->face_name;

    rec.available_faces = faces;
    rec.num_available_faces = ff->font_count;

    if (fsb->fsb.currently_selected_face != NULL) {
	rec.current_face = fsb->fsb.currently_selected_face->face_name;
    } else rec.current_face = fsb->fsb.font_face;

    rec.new_face = NULL;

    XtCallCallbackList((Widget) fsb, fsb->fsb.face_select_callback, &rec);
    if (rec.new_face != NULL) {
	for (i = 0; i < ff->font_count; i++) {
	    if (rec.new_face == faces[i]) break;
	}
    }
    if (rec.new_face == NULL || i >= ff->font_count) {
	i = MatchFaceName(&rec, &junk);
    }
    XtFree((XtPointer) faces);

    ListSelectPos(fsb->fsb.face_scrolled_list_child, i+1, FALSE);
    for (f = ff->fonts; i != 0; f = f->next) i--;
    fsb->fsb.currently_selected_face = f;
}

/* ARGSUSED */

static void FamilySelect(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    XmListCallbackStruct *listCB = (XmListCallbackStruct *) callData;
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget)
		    XtParent(XtParent(XtParent(XtParent(widget))));
    FontFamilyRec *ff = fsb->fsb.known_families;
    int i;

    if (fsb->fsb.current_family_multiple) {
	fsb->fsb.current_family_multiple = FALSE;
	UnmanageFamilyMultiple(fsb);
    }

    /* List uses 1-based addressing!! */
    for (i = 1; i < listCB->item_position; i++) ff = ff->next;

    fsb->fsb.currently_selected_family = ff;

    SetUpFaceList(fsb, ff);
    if (!fsb->fsb.current_face_multiple) GetInitialFace(fsb, ff);
    if (fsb->fsb.auto_preview) DoPreview(fsb, FALSE);
}

/* ARGSUSED */

static void FaceSelect(widget, clientData, callData)
    Widget widget;
    XtPointer clientData, callData;
{
    XmListCallbackStruct *listCB = (XmListCallbackStruct *) callData;
    FontSelectionBoxWidget fsb =
	    (FontSelectionBoxWidget)
		    XtParent(XtParent(XtParent(XtParent(widget))));
    FontRec *f = fsb->fsb.currently_selected_family->fonts;
    int i;

    if (fsb->fsb.current_face_multiple) {
	fsb->fsb.current_face_multiple = FALSE;
	UnmanageFaceMultiple(fsb);
    }

    /* List uses 1-based addressing!! */
    for (i = 1; i < listCB->item_position; i++) f = f->next;

    fsb->fsb.currently_selected_face = f;

    if (fsb->fsb.auto_preview) DoPreview(fsb, FALSE);
}

static void CreateSizeMenu(fsb, destroyOldChildren)
    FontSelectionBoxWidget fsb;
    Boolean destroyOldChildren;
{
    Arg args[20];
    int i, j;
    Widget *sizes;
    char buf[20];
    Widget *children;
    Cardinal num_children;
    XmString csName;
    char *ch;

    if (destroyOldChildren) {
	i = 0;
	XtSetArg(args[i], XtNchildren, &children);			i++;
	XtSetArg(args[i], XtNnumChildren, &num_children);		i++;
	XtGetValues(fsb->fsb.size_menu, args, i);
	
	/* Don't destroy first child ("other") */
	for (j = 1; j < num_children; j++) XtDestroyWidget(children[j]);

	sizes = (Widget *) XtMalloc((fsb->fsb.size_count+1) * sizeof(Widget));
	sizes[0] = children[0];
    } else {
	i = 0;
	sizes = (Widget *) XtMalloc((fsb->fsb.size_count+1) * sizeof(Widget));
	fsb->fsb.other_size = sizes[0] =
		XtCreateManagedWidget("other", xmPushButtonGadgetClass,
				  fsb->fsb.size_menu, args, i);
    }

    for (j = 0; j < fsb->fsb.size_count; j++) {
	(void) sprintf(buf, "%g", fsb->fsb.sizes[j]);
	csName = CS(buf);
	for (ch = buf; *ch != '\0'; ch++) if (*ch == '.') *ch = '-';
	i = 0;
	XtSetArg(args[i], XmNlabelString, csName);			i++;
	sizes[j+1] =
		XmCreatePushButtonGadget(fsb->fsb.size_menu, buf, args, i);
	XmStringFree(csName);
	XtAddCallback(sizes[j+1], XmNactivateCallback,
		      SetSize, (XtPointer) (fsb->fsb.sizes+j));
    }
    XtManageChildren(sizes, j+1);
    XtFree((char *) sizes);
}

static void CreateChildren(fsb)
    FontSelectionBoxWidget fsb;
{
    Arg args[20];
    int i;
    Widget form;

    i = 0;
    fsb->fsb.pane_child =
	    XtCreateManagedWidget("pane", xmPanedWindowWidgetClass,
				  (Widget) fsb, args, i);

    i = 0;
    fsb->fsb.preview_child =
	    XtCreateManagedWidget("preview", xmDrawingAreaWidgetClass,
				  fsb->fsb.pane_child, args, i);
    XtAddCallback(fsb->fsb.preview_child, XmNexposeCallback,
		  PreviewText, (XtPointer) NULL);
    XtAddCallback(fsb->fsb.preview_child, XmNresizeCallback,
		  ResizePreview, (XtPointer) NULL);

    i = 0;	
    form = XtCreateManagedWidget("panel", xmFormWidgetClass,
				 fsb->fsb.pane_child, args, i);
    fsb->fsb.panel_child = form;

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.ok_button_child =
	    XtCreateManagedWidget("okButton", xmPushButtonWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.ok_button_child, XmNactivateCallback,
		  OKCallback, NULL);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget,fsb->fsb.ok_button_child );		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.apply_button_child =
	    XtCreateManagedWidget("applyButton", xmPushButtonWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.apply_button_child, XmNactivateCallback,
		  ApplyCallback, NULL);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget,fsb->fsb.apply_button_child );	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.reset_button_child =
	    XtCreateManagedWidget("resetButton", xmPushButtonWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.reset_button_child, XmNactivateCallback,
		  ResetCallback, NULL);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget,fsb->fsb.reset_button_child );	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.cancel_button_child =
	    XtCreateManagedWidget("cancelButton", xmPushButtonWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.cancel_button_child, XmNactivateCallback,
		  CancelCallback, NULL);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.ok_button_child);	i++;
    fsb->fsb.separator_child =
	    XtCreateManagedWidget("separator", xmSeparatorGadgetClass,
				  form, args, i);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.separator_child);	i++;
    fsb->fsb.size_label_child =
	    XtCreateManagedWidget("sizeLabel", xmLabelWidgetClass,
				  form, args, i);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget, fsb->fsb.size_label_child);	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.size_label_child);	i++;
    fsb->fsb.size_text_field_child =
	    XtCreateManagedWidget("sizeTextField", xmTextFieldWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.size_text_field_child, XmNvalueChangedCallback,
		  SizeSelect, (XtPointer) NULL);
    XtAddCallback(fsb->fsb.size_text_field_child, XmNmodifyVerifyCallback,
		  TextVerify, (XtPointer) NULL);

    i = 0;
    fsb->fsb.size_menu = XmCreatePulldownMenu(form, "sizeMenu", args, i);

    CreateSizeMenu(fsb, FALSE);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget, fsb->fsb.size_text_field_child);	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.size_label_child);	i++;
    XtSetArg(args[i], XmNsubMenuId, fsb->fsb.size_menu);		i++;
    fsb->fsb.size_option_menu_child =
	    XmCreateOptionMenu(form, "sizeOptionMenu", args, i);
    XtManageChild(fsb->fsb.size_option_menu_child);

    i = 0;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNleftWidget, fsb->fsb.size_option_menu_child);	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.separator_child);	i++;
    fsb->fsb.size_multiple_label_child =
	    XtCreateWidget("sizeMultipleLabel", xmLabelWidgetClass,
			   form, args, i);

    i = 0;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.size_label_child);	i++;
    fsb->fsb.preview_button_child =
	    XtCreateManagedWidget("previewButton", xmPushButtonWidgetClass,
				  form, args, i);
    XtAddCallback(fsb->fsb.preview_button_child, XmNactivateCallback,
		  PreviewCallback, (XtPointer) NULL);

    i = 0;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNrightWidget, fsb->fsb.preview_button_child);	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET);	i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.preview_button_child);	i++;
    fsb->fsb.sampler_button_child =
	    XtCreateWidget("samplerButton", xmPushButtonWidgetClass,
				  form, args, i);
    if (fsb->fsb.show_sampler_button) {
	XtManageChild(fsb->fsb.sampler_button_child);
    }
    XtAddCallback(fsb->fsb.sampler_button_child, XmNactivateCallback,
		  ShowSamplerCallback, (XtPointer) NULL);

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNrightPosition, 50);				i++;
    fsb->fsb.family_label_child =
	    XtCreateManagedWidget("familyLabel", xmLabelGadgetClass,
				  form, args, i);

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_FORM);			i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNleftPosition, 50);				i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.face_label_child =
	    XtCreateManagedWidget("faceLabel", xmLabelGadgetClass,
				  form, args, i);

    /* The next two must be widgets in order to be reversed in color */

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.family_label_child);	i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNrightPosition, 50);				i++;
    fsb->fsb.family_multiple_label_child =
	    XtCreateWidget("familyMultipleLabel", xmLabelWidgetClass,
			   form, args, i);

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.face_label_child);		i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNleftPosition, 50);				i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM);		i++;
    fsb->fsb.face_multiple_label_child =
	    XtCreateWidget("faceMultipleLabel", xmLabelWidgetClass,
			   form, args, i);

    i = 0; 
    XtSetArg(args[i], XmNitemCount, 1);					i++;
    XtSetArg(args[i], XmNitems, &CSempty);				i++;
    fsb->fsb.family_scrolled_list_child =
	    XmCreateScrolledList(form, "familyScrolledList", args, i);
    XtAddCallback(fsb->fsb.family_scrolled_list_child,
		  XmNbrowseSelectionCallback, FamilySelect, NULL);
    XtAddCallback(fsb->fsb.family_scrolled_list_child,
		  XmNdefaultActionCallback, PreviewDoubleClick, NULL);

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.family_label_child);	i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.size_text_field_child);	i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_FORM);		i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNrightPosition, 50);				i++;
    XtSetValues(XtParent(fsb->fsb.family_scrolled_list_child), args, i);
    XtManageChild(fsb->fsb.family_scrolled_list_child);

    i = 0;
    XtSetArg(args[i], XmNitemCount, 1);					i++;
    XtSetArg(args[i], XmNitems, &CSempty);				i++;
    fsb->fsb.face_scrolled_list_child =
	    XmCreateScrolledList(form, "faceScrolledList", args, i);
    XtAddCallback(fsb->fsb.face_scrolled_list_child,
		  XmNbrowseSelectionCallback, FaceSelect, NULL);
    XtAddCallback(fsb->fsb.face_scrolled_list_child,
		  XmNdefaultActionCallback, PreviewDoubleClick, NULL);

    i = 0;
    XtSetArg(args[i], XmNtopAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNtopWidget, fsb->fsb.face_label_child);		i++;
    XtSetArg(args[i], XmNbottomAttachment, XmATTACH_WIDGET);		i++;
    XtSetArg(args[i], XmNbottomWidget, fsb->fsb.size_text_field_child);	i++;
    XtSetArg(args[i], XmNleftAttachment, XmATTACH_POSITION);		i++;
    XtSetArg(args[i], XmNleftPosition, 50);				i++;
    XtSetArg(args[i], XmNrightAttachment, XmATTACH_FORM);		i++;
    XtSetValues(XtParent(fsb->fsb.face_scrolled_list_child), args, i);
    XtManageChild(fsb->fsb.face_scrolled_list_child);

    i = 0;
    XtSetArg(args[i], XmNdefaultButton, fsb->fsb.ok_button_child);	i++;
    XtSetValues(form, args, i);
}

static void DisplayFontFamilies(fsb)
    FontSelectionBoxWidget fsb;
{
    Arg args[20];
    int i;
    FontFamilyRec *ff;
    XmString *CSlist, *str;
    
    CSlist = (XmString *) XtMalloc(fsb->fsb.family_count * sizeof(XmString));
    str = CSlist;
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	*str++ = ff->CS_family_name;
    }
    
    i = 0;
    XtSetArg(args[i], XmNitemCount, fsb->fsb.family_count);		i++;
    XtSetArg(args[i], XmNitems, CSlist);				i++;
    XtSetValues(fsb->fsb.family_scrolled_list_child, args, i);

    XtFree((char *) CSlist);
}

static void SetUpCurrentFontFromName(fsb)
    FontSelectionBoxWidget fsb;
{
    FontFamilyRec *ff;
    FontRec *f;
    int i, j;

    fsb->fsb.currently_selected_face = NULL;
    fsb->fsb.currently_selected_family = NULL;

    if (fsb->fsb.font_name_multiple || fsb->fsb.font_name == NULL) {
	fsb->fsb.font_name = NULL;
	fsb->fsb.font_family = NULL;
	fsb->fsb.font_face = NULL;
	if (fsb->fsb.font_name_multiple) {
	    fsb->fsb.current_family_multiple = TRUE;
	    fsb->fsb.current_face_multiple = TRUE;
	    ManageFamilyMultiple(fsb);
	    ManageFaceMultiple(fsb);
	}
	XmListDeselectAllItems(fsb->fsb.family_scrolled_list_child);
	XmListDeselectAllItems(fsb->fsb.face_scrolled_list_child);
	XmListDeleteAllItems(fsb->fsb.face_scrolled_list_child);
	XmListAddItem(fsb->fsb.face_scrolled_list_child, CSempty, 1);
	return;
    }

    if (!fsb->fsb.font_name_multiple) {
	fsb->fsb.current_family_multiple = FALSE;
	fsb->fsb.current_face_multiple = FALSE;
	UnmanageFamilyMultiple(fsb);
	UnmanageFaceMultiple(fsb);
    }

    fsb->fsb.font_name = Canonical(fsb->fsb.font_name);
    i = 1;
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	j = 1;
	for (f = ff->fonts; f != NULL; f = f->next) {
	    if (f->font_name == fsb->fsb.font_name) {
		fsb->fsb.font_family = ff->family_name;
		fsb->fsb.font_face = f->face_name;
		SetUpFaceList(fsb, ff);
		ListSelectPos(fsb->fsb.family_scrolled_list_child, i, FALSE);
		ListSelectPos(fsb->fsb.face_scrolled_list_child, j, FALSE);
		fsb->fsb.currently_selected_face = f;
		fsb->fsb.currently_selected_family = ff;
		return;
	    }
	    j++;
	}
	i++;
    }
 
   /* Didn't find it! */
    fsb->fsb.font_name = NULL;
    fsb->fsb.font_family = NULL;
    fsb->fsb.font_face = NULL;
    XmListDeselectAllItems(fsb->fsb.family_scrolled_list_child);
    XmListDeselectAllItems(fsb->fsb.face_scrolled_list_child);
    XmListDeleteAllItems(fsb->fsb.face_scrolled_list_child);
    XmListAddItem(fsb->fsb.face_scrolled_list_child, CSempty, 1);
}

static void SetUpCurrentFontFromFamilyFace(fsb)
    FontSelectionBoxWidget fsb;
{
    FontFamilyRec *ff;
    FontRec *f;
    int i;

    fsb->fsb.currently_selected_face = NULL;
    fsb->fsb.currently_selected_family = NULL;

    if (fsb->fsb.font_family_multiple) {
	fsb->fsb.font_family = NULL;
	fsb->fsb.current_family_multiple = TRUE;
	ManageFamilyMultiple(fsb);
    } else {
	fsb->fsb.current_family_multiple = FALSE;
	UnmanageFamilyMultiple(fsb);
    }

    if (fsb->fsb.font_face_multiple) {
	fsb->fsb.font_face = NULL;
	fsb->fsb.current_face_multiple = TRUE;
	ManageFaceMultiple(fsb);
    } else {
	fsb->fsb.current_face_multiple = FALSE;
	UnmanageFaceMultiple(fsb);
    }

    if (fsb->fsb.font_family != NULL) {
	fsb->fsb.font_family = Canonical(fsb->fsb.font_family);
	i = 1;
	for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	    if (fsb->fsb.font_family == ff->family_name) {
		ListSelectPos(fsb->fsb.family_scrolled_list_child, i, FALSE);
		fsb->fsb.currently_selected_family = ff;
		SetUpFaceList(fsb, ff);
		break;
	    }
	    i++;
	}
	if (ff == NULL) fsb->fsb.font_family = NULL;
    }   
	
    if (fsb->fsb.font_family == NULL) {
	fsb->fsb.font_face = NULL;
	XmListDeselectAllItems(fsb->fsb.family_scrolled_list_child);
	XmListDeselectAllItems(fsb->fsb.face_scrolled_list_child);
	XmListDeleteAllItems(fsb->fsb.face_scrolled_list_child);
	XmListAddItem(fsb->fsb.face_scrolled_list_child, CSempty, 1);
	return;
    }

    if (fsb->fsb.font_face != NULL) {
	fsb->fsb.font_face = Canonical(fsb->fsb.font_face);

	i = 1;
	for (f = ff->fonts; f != NULL; f = f->next) {
	    if (fsb->fsb.font_face == f->face_name) {
		ListSelectPos(fsb->fsb.face_scrolled_list_child, i, FALSE);
		fsb->fsb.currently_selected_face = f;
		break;
	    }
	    i++;
	}
	if (f == NULL) fsb->fsb.font_face = NULL;
    } else {
	f = NULL;
	XmListDeselectAllItems(fsb->fsb.face_scrolled_list_child);
    }

    if (f == NULL && !fsb->fsb.font_face_multiple) GetInitialFace(fsb, ff);
}

static void SetUpCurrentFont(fsb)
    FontSelectionBoxWidget fsb;
{
    if (fsb->fsb.use_font_name) SetUpCurrentFontFromName(fsb);
    else SetUpCurrentFontFromFamilyFace(fsb);
}

static void SetUpCurrentSize(fsb)
    FontSelectionBoxWidget fsb;
{
    int i;
    Arg args[20];
    char buf[20];

    if (fsb->fsb.font_size_multiple) {
	i = 0;
	XtSetArg(args[i], XmNvalue, "");				i++;
	XtSetValues(fsb->fsb.size_text_field_child, args, i);
	fsb->fsb.current_size_multiple = TRUE;
	ManageSizeMultiple(fsb);
	return;
    } else UnmanageSizeMultiple(fsb);
    
    if (fsb->fsb.currently_selected_size == 0.0) {
	sprintf(buf, "%g", fsb->fsb.font_size);
    } else sprintf(buf, "%g", fsb->fsb.currently_selected_size);

    changingSize = True;
    i = 0;
    XtSetArg(args[i], XmNvalue, buf);					i++;
    XtSetValues(fsb->fsb.size_text_field_child, args, i);
    changingSize = False;
}

static void SetUpCurrentSelections(fsb)
    FontSelectionBoxWidget fsb;
{
    SetUpCurrentFont(fsb);
    SetUpCurrentSize(fsb);
    if (fsb->fsb.preview_on_change) DoPreview(fsb, FALSE);
}

/* ARGSUSED */

static void Initialize(request, new, args, num_args)
    Widget request, new;
    ArgList args;
    Cardinal *num_args;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) new;
    Bool inited;

    /* Verify size list */

    if (fsb->fsb.size_count > 0 && fsb->fsb.sizes == NULL) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"initializeFontBox", "sizeMismatch",
			"FontSelectionBoxError",
			"Size count specified but no sizes present",
			(String *) NULL, (Cardinal *) NULL);
	fsb->fsb.size_count = 0;
    }

    if (fsb->fsb.size_count < 0) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"initializeFontBox", "negativeSize",
			"FontSelectionBoxError",
			"Size count should not be negative",
			(String *) NULL, (Cardinal *) NULL);
	fsb->fsb.size_count = 0;
    }

    if (fsb->fsb.max_pending_deletes <= 0) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"initializeFontBox", "nonPositivePendingDelete",
			"FontSelectionBoxError",
			"Pending delete max must be positive",
			(String *) NULL, (Cardinal *) NULL);
	fsb->fsb.max_pending_deletes = 1;
    }

    /* Copy strings.  SetUpCurrentSelection will copy the font strings */

    if (fsb->fsb.preview_string != NULL) {
	fsb->fsb.preview_string = XtNewString(fsb->fsb.preview_string);
    }
    if (fsb->fsb.default_resource_path != NULL) {
	fsb->fsb.default_resource_path =
		XtNewString(fsb->fsb.default_resource_path);
    }
    if (fsb->fsb.resource_path_override != NULL) {
	fsb->fsb.resource_path_override =
		XtNewString(fsb->fsb.resource_path_override);
    }

    /* Get the context */

    if (fsb->fsb.context == NULL) {
	fsb->fsb.context = XDPSGetSharedContext(XtDisplay(fsb));
    }

    if (_XDPSTestComponentInitialized(fsb->fsb.context,
				      dps_init_bit_fsb, &inited) ==
	dps_status_unregistered_context) {
	XDPSRegisterContext(fsb->fsb.context, False);
    }

    if (!inited) {
	(void) _XDPSSetComponentInitialized(fsb->fsb.context,
					    dps_init_bit_fsb);
	_DPSFDefineFontEnumFunctions(fsb->fsb.context);
    }

    /* Initialize non-resource fields */

    fsb->fsb.gstate = 0;
    fsb->fsb.sampler = NULL;
    fsb->fsb.known_families = NULL;
    fsb->fsb.family_count = 0;
    fsb->fsb.currently_previewed = NULL;
    fsb->fsb.currently_selected_face = NULL;
    fsb->fsb.currently_selected_family = NULL;
    fsb->fsb.currently_previewed_size = 0.0;
    fsb->fsb.currently_selected_size = 0.0;
    fsb->fsb.pending_delete_count = 0;
    fsb->fsb.pending_delete_font = NULL;
    fsb->fsb.preview_fixed = FALSE;
    fsb->fsb.current_family_multiple = FALSE;
    fsb->fsb.current_face_multiple = FALSE;
    fsb->fsb.current_size_multiple = FALSE;

    GetFontNames(fsb);
    CreateChildren(fsb);

    DisplayFontFamilies(fsb);
    SetUpCurrentSelections(fsb);
    if (fsb->fsb.show_sampler) ShowSampler(fsb);
}

static void FreeFontLists(fsb)
    FontSelectionBoxWidget fsb;
{
    FontFamilyRec *ff, *next_ff;
    FontRec *f, *next_f;

    /* font_name, face_name, and family_name are canonical strings and so
       should not be freed */

    for (ff = fsb->fsb.known_families; ff != NULL; ff = next_ff) {
	for (f = ff->fonts; f != NULL; f = next_f) {
	    XmStringFree(f->CS_face_name);
	    XtFree(f->full_name);
	    next_f = f->next;
	    XtFree((char *) f);
	}
	XmStringFree(ff->CS_family_name);
	next_ff = ff->next;
	XtFree((char *) ff);
    }
}

static void Destroy(widget)
    Widget widget;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) widget;

    /* Lots of stuff to destroy! */

    if (fsb->fsb.gstate != 0) XDPSFreeContextGState(fsb->fsb.context,
						    fsb->fsb.gstate);
    if (fsb->fsb.preview_string != NULL) XtFree(fsb->fsb.preview_string);
    if (fsb->fsb.default_resource_path != NULL) {
	XtFree(fsb->fsb.default_resource_path);
    }
    if (fsb->fsb.resource_path_override != NULL) {
	XtFree(fsb->fsb.resource_path_override);
    }

    FreeFontLists(fsb);
}

static void Resize(widget)
    Widget widget;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) widget;

    XtResizeWidget(fsb->fsb.pane_child, fsb->core.width, fsb->core.height, 0);
}

/* ARGSUSED */

static Boolean SetValues(old, req, new, args, num_args)
    Widget old, req, new;
    ArgList args;
    Cardinal *num_args;
{
    FontSelectionBoxWidget oldfsb = (FontSelectionBoxWidget) old;
    FontSelectionBoxWidget newfsb = (FontSelectionBoxWidget) new;
    Boolean refreshLists = FALSE, setSelection = FALSE, do_preview = FALSE;
    Bool inited;

#define NE(field) newfsb->fsb.field != oldfsb->fsb.field
#define DONT_CHANGE(field) \
    if (NE(field)) newfsb->fsb.field = oldfsb->fsb.field;

    DONT_CHANGE(pane_child);
    DONT_CHANGE(preview_child);
    DONT_CHANGE(panel_child);
    DONT_CHANGE(family_label_child);
    DONT_CHANGE(family_multiple_label_child);
    DONT_CHANGE(family_scrolled_list_child);
    DONT_CHANGE(face_label_child);
    DONT_CHANGE(face_multiple_label_child);
    DONT_CHANGE(face_scrolled_list_child);
    DONT_CHANGE(size_label_child);
    DONT_CHANGE(size_text_field_child);
    DONT_CHANGE(size_option_menu_child);
    DONT_CHANGE(preview_button_child);
    DONT_CHANGE(sampler_button_child);
    DONT_CHANGE(separator_child);
    DONT_CHANGE(ok_button_child);
    DONT_CHANGE(apply_button_child);
    DONT_CHANGE(reset_button_child);
    DONT_CHANGE(cancel_button_child);
#undef DONT_CHANGE

    if (newfsb->fsb.size_count > 0 && newfsb->fsb.sizes == NULL) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"setValuesFontBox", "sizeMismatch",
			"FontSelectionBoxError",
			"Size count specified but no sizes present",
			(String *) NULL, (Cardinal *) NULL);
	newfsb->fsb.size_count = 0;
    }

    if (newfsb->fsb.size_count < 0) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"setValuesFontBox", "negativeSize",
			"FontSelectionBoxError",
			"Size count should not be negative",
			(String *) NULL, (Cardinal *) NULL);
	newfsb->fsb.size_count = 0;
    }

    if (newfsb->fsb.max_pending_deletes <= 0) {
	XtAppWarningMsg(XtWidgetToApplicationContext(new),
			"setValuesFontBox", "nonPositivePendingDelete",
			"FontSelectionBoxError",
			"Pending delete max must be positive",
			(String *) NULL, (Cardinal *) NULL);
	newfsb->fsb.max_pending_deletes = 1;
    }

    if (NE(preview_string)) {
	XtFree(oldfsb->fsb.preview_string);
	newfsb->fsb.preview_string = XtNewString(newfsb->fsb.preview_string);
	do_preview = TRUE;
    }

    if (NE(default_resource_path)) {
	XtFree(oldfsb->fsb.default_resource_path);
	newfsb->fsb.default_resource_path =
		XtNewString(newfsb->fsb.default_resource_path);
	refreshLists = TRUE;
    }

    if (NE(resource_path_override)) {
	XtFree(oldfsb->fsb.resource_path_override);
	newfsb->fsb.resource_path_override =
		XtNewString(newfsb->fsb.resource_path_override);
	refreshLists = TRUE;
    }

    if (newfsb->fsb.undef_unused_fonts) UndefSomeUnusedFonts(newfsb, FALSE);

    if (NE(context)) {
	if (newfsb->fsb.context == NULL) {
	    newfsb->fsb.context = XDPSGetSharedContext(XtDisplay(newfsb));
	} 
	if (_XDPSTestComponentInitialized(newfsb->fsb.context,
					  dps_init_bit_share, &inited) ==
	    dps_status_unregistered_context) {
	    XDPSRegisterContext(newfsb->fsb.context, False);
	}
	if (!inited) {
	    (void) _XDPSSetComponentInitialized(newfsb->fsb.context,
						dps_init_bit_fsb);
	    _DPSFDefineFontEnumFunctions(newfsb->fsb.context);
	}
    }	

    if (refreshLists) {
	FreeFontLists(newfsb);
	GetFontNames(newfsb);
	DisplayFontFamilies(newfsb);
	setSelection = TRUE;
    }

    if (NE(sizes)) {
	CreateSizeMenu(newfsb, TRUE);
	setSelection = TRUE;
    }

    if (NE(show_sampler)) {
	if (newfsb->fsb.show_sampler) ShowSampler(newfsb);
	else XtPopdown(newfsb->fsb.sampler);
    }

    if (NE(show_sampler_button)) {
	if (newfsb->fsb.show_sampler_button) {
	    XtManageChild(newfsb->fsb.sampler_button_child);
	} else XtUnmanageChild(newfsb->fsb.sampler_button_child);
    }

    if (NE(font_size)) newfsb->fsb.currently_selected_size = 0.0;

    if (NE(use_font_name) || NE(font_name) || NE(font_family) ||
	NE(font_face) || NE(font_size) || NE(font_name_multiple) ||
	NE(font_family_multiple) || NE(font_face_multiple) ||
	NE(font_size_multiple)) setSelection = TRUE;

    if (setSelection) SetUpCurrentSelections(newfsb);
    else if (do_preview && newfsb->fsb.preview_on_change) {
	DoPreview(newfsb, FALSE);
    }

    return FALSE;
#undef NE
}

/* ARGSUSED */

static XtGeometryResult GeometryManager(w, desired, allowed)
    Widget w;
    XtWidgetGeometry *desired, *allowed;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) XtParent(w);
    XtWidgetGeometry request;
    XtGeometryResult result;

#define WANTS(flag) (desired->request_mode & flag)

    if (WANTS(XtCWQueryOnly)) return XtGeometryYes;

    if (WANTS(CWWidth)) w->core.width = desired->width;
    if (WANTS(CWHeight)) w->core.height = desired->height;
    if (WANTS(CWX)) w->core.x = desired->x;
    if (WANTS(CWY)) w->core.y = desired->y;
    if (WANTS(CWBorderWidth)) {
	w->core.border_width = desired->border_width;
    }

    if (request.width != fsb->core.width ||
	    request.height != fsb->core.height) {
	request.request_mode = CWWidth | CWHeight;
	do {
	    result = XtMakeGeometryRequest((Widget) fsb,
		    &request, &request);
	} while (result == XtGeometryAlmost);
    } 

    return XtGeometryYes;
#undef WANTS
}

static void ChangeManaged(w)
    Widget w;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;

    w->core.width = fsb->composite.children[0]->core.width;
    w->core.height = fsb->composite.children[0]->core.height;
}

static void SetFontName(w, name, name_multiple)
    Widget w;
    String name;
    Bool name_multiple;
{
    int i;
    Arg args[20];

    i = 0;
    XtSetArg(args[i], XtNfontName, name);				i++;
    XtSetArg(args[i], XtNuseFontName, TRUE);				i++;
    XtSetArg(args[i], XtNfontNameMultiple, name_multiple);		i++;
    XtSetValues(w, args, i);
}

void FSBSetFontName(w, name, name_multiple)
    Widget w;
    String name;
    Bool name_multiple;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass) XtClass(w))->fsb_class.set_font_name)
	    (w, name, name_multiple);
}

static void SetFontFamilyFace(w, family, face, family_multiple, face_multiple)
    Widget w;
    String family, face;
    Bool family_multiple, face_multiple;
{
    int i;
    Arg args[20];

    i = 0;
    XtSetArg(args[i], XtNfontFamily, family);				i++;
    XtSetArg(args[i], XtNfontFace, face);				i++;
    XtSetArg(args[i], XtNuseFontName, FALSE);				i++;
    XtSetArg(args[i], XtNfontFamilyMultiple, family_multiple);		i++;
    XtSetArg(args[i], XtNfontFaceMultiple, face_multiple);		i++;
    XtSetValues(w, args, i);
}

void FSBSetFontFamilyFace(w, family, face, family_multiple, face_multiple)
    Widget w;
    String family, face;
    Bool family_multiple, face_multiple;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.set_font_family_face)
	    (w, family, face, family_multiple, face_multiple);
}

static void SetFontSize(w, size, size_multiple)
    Widget w;
    double size;
    Bool size_multiple;
{
    int i;
    Arg args[20];
    union {
	int i;
	float f;
    } kludge;

    kludge.f = size;

    i = 0;
    XtSetArg(args[i], XtNfontSize, kludge.i);				i++;
    XtSetArg(args[i], XtNfontSizeMultiple, size_multiple);		i++;
    XtSetValues(w, args, i);
}

void FSBSetFontSize(w, size, size_multiple)
    Widget w;
    double size;
    Bool size_multiple;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass) XtClass(w))->fsb_class.set_font_size)
	    (w, size, size_multiple);
}

static void RefreshFontList(w)
    Widget w;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;

    FreeFontLists(fsb);
    GetFontNames(fsb);
    DisplayFontFamilies(fsb);
    SetUpCurrentSelections(fsb);
}

void FSBRefreshFontList(w)
    Widget w;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.refresh_font_list) (w);
}

static void GetFamilyList(w, count, list)
    Widget w;
    int *count;
    String **list;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    String *buf;
    FontFamilyRec *ff;

    *count = fsb->fsb.family_count;
    *list = buf = (String *) XtMalloc(*count * sizeof(String));
    
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	*buf++ = ff->family_name;
    }
}

void FSBGetFamilyList(w, count, list)
    Widget w;
    int *count;
    String **list;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.get_family_list) (w, count, list);
}

static void GetFaceList(w, family, count, face_list, font_list)
    Widget w;
    String family;
    int *count;
    String **face_list, **font_list;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    String *buf1, *buf2;
    FontFamilyRec *ff;
    FontRec *f;
    
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	if (ff->family_name == family) break;
    }

    *count = ff->font_count;
    *face_list = buf1 = (String *) XtMalloc(*count * sizeof(String));
    *font_list = buf2 = (String *) XtMalloc(*count * sizeof(String));

    for (f = ff->fonts; f != NULL; f = f->next) {
	*buf1++ = f->face_name;
	*buf2++ = f->font_name;
    }
}

void FSBGetFaceList(w, family, count, face_list, font_list)
    Widget w;
    String family;
    int *count;
    String **face_list, **font_list;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.get_face_list) (w, family, count,
					      face_list, font_list);
}

void FSBUndefineUnusedFonts(w)
    Widget w;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.undef_unused_fonts) (w);
}

static Boolean DownloadFontName(w, name)
    Widget w;
    String name;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    FontFamilyRec *ff;
    FontRec *f;

    name = Canonical(name);
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	for (f = ff->fonts; f != NULL; f = f->next) {
	    if (f->font_name == name) {
		if (f->resident) return TRUE;
		else {
		    if (fsb->fsb.make_fonts_shared) f->resident = TRUE;
		    return DownloadFont(fsb, name, fsb->fsb.context,
					fsb->fsb.make_fonts_shared);
		}
	    }
	}
    }
 
    return DownloadFont(fsb, name, fsb->fsb.context,
			fsb->fsb.make_fonts_shared);
}

Boolean FSBDownloadFontName(w, name)
    Widget w;
    String name;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    if (name == NULL) return FALSE;
    return (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.download_font_name) (w, name);
}

static Boolean MatchFontFace(w, old_face, new_family, new_face)
    Widget w;
    String old_face, new_family;
    String *new_face;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    FSBFaceSelectCallbackRec rec;
    String *faces;
    int i;
    FontFamilyRec *ff;
    FontRec *f;
    Boolean retVal;

    new_family = Canonical(new_family);
    old_face = Canonical(old_face);
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	if (ff->family_name == new_family) break;
    }
    if (ff == NULL) {
	*new_face = NULL;
	return FALSE;
    }

    faces = (String *) XtMalloc(ff->font_count * sizeof(String));
    i = 0;
    for (f = ff->fonts; f != NULL; f = f->next) faces[i++] = f->face_name;

    rec.available_faces = faces;
    rec.num_available_faces = ff->font_count;
    rec.current_face = old_face;
    rec.new_face = NULL;

    i = MatchFaceName(&rec, &retVal);
    *new_face = faces[i];
    XtFree((XtPointer) faces);
    return !retVal;
}

Boolean FSBMatchFontFace(w, old_face, new_family, new_face)
    Widget w;
    String old_face, new_family;
    String *new_face;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    return (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.match_font_face) (w, old_face,
						new_family, new_face);
}

static void FontNameToFamilyFace(w, font_name, family, face)
    Widget w;
    String font_name;
    String *family, *face;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    FontFamilyRec *ff;
    FontRec *f;

    font_name = Canonical(font_name);
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	for (f = ff->fonts; f != NULL; f = f->next) {
	    if (f->font_name == font_name) {
		*family = ff->family_name;
		*face = f->face_name;
		return;
	    }
	}
    }

    *family = NULL;
    *face = NULL;
}

void FSBFontNameToFamilyFace(w, font_name, family, face)
    Widget w;
    String font_name;
    String *family, *face;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.font_name_to_family_face) (w, font_name,
							 family, face);
}

static void FontFamilyFaceToName(w, family, face, font_name)
    Widget w;
    String family, face;
    String *font_name;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;
    FontFamilyRec *ff;
    FontRec *f;

    family = Canonical(family);
    for (ff = fsb->fsb.known_families; ff != NULL; ff = ff->next) {
	if (ff->family_name == family) break;
    }
    if (ff == NULL) {
	*font_name = NULL;
	return;
    }

    face = Canonical(face);
    for (f = ff->fonts; f != NULL; f = f->next) {
	if (f->face_name == face) {
	    *font_name = f->font_name;
	    return;
	}
    }

    *font_name = NULL;
}

void FSBFontFamilyFaceToName(w, family, face, font_name)
    Widget w;
    String family, face;
    String *font_name;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass)
       XtClass(w))->fsb_class.font_family_face_to_name) (w, family, face,
							 font_name);
}

String FSBFindAFM(w, font_name)
    Widget w;
    String font_name;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    return (*((FontSelectionBoxWidgetClass) XtClass(w))->
	    fsb_class.find_afm) (w, font_name);
}

String FSBFindFontFile(w, font_name)
    Widget w;
    String font_name;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    return (*((FontSelectionBoxWidgetClass) XtClass(w))->
	    fsb_class.find_font_file) (w, font_name);
}

static void GetTextDimensions(w, text, font, size, x, y, dx, dy,
			      left, right, top, bottom)
    Widget w;
    String text, font;
    double size, x, y;
    float *dx, *dy, *left, *right, *top, *bottom;
{
    FontSelectionBoxWidget fsb = (FontSelectionBoxWidget) w;

    _DPSFGetTextDimensions(fsb->fsb.context, text, font, size, x, y,
			   dx, dy, left, right, top, bottom);
}

void FSBGetTextDimensions(w, text, font, size, x, y, dx, dy,
			  left, right, top, bottom)
    Widget w;
    String text, font;
    double size, x, y;
    float *dx, *dy, *left, *right, *top, *bottom;
{
    XtCheckSubclass(w, fontSelectionBoxWidgetClass, NULL);

    (*((FontSelectionBoxWidgetClass) XtClass(w))->
	fsb_class.get_text_dimensions) (w, text, font, size, x, y,
					dx, dy, left, right, top, bottom);
}
