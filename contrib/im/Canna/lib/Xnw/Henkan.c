/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of NEC
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  NEC Corporation makes no representations about the
 * suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/*

  This program is made by modifying Label.c.

 */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 * Henkan.c - Henkan widget
 *
 */

#define XtStrlen(s)		((s) ? strlen(s) : 0)

#include <stdio.h>
#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "XawInit.h"
#include "HenkanP.h"
#include <X11/kanji.h>

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

#define offset(field) XtOffset(HenkanWidget, field)
static XtResource resources[] = {
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(henkan.foreground), XtRString, "XtDefaultForeground"},
    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	offset(henkan.background), XtRString, "XtDefaultBackground"},
    {XtNfont,  XtCFont, XtRFontStruct, sizeof(XFontStruct *),
        offset(henkan.font),XtRString, XnwDefaultFont},
    {XtNkanjiFont,  XtCKanjiFont, XtRKanjiFontStruct, sizeof(XFontStruct *),
        offset(henkan.kanji_font),XtRString, XnwDefaultKanjiFont},
    {XtNkanaFont,  XtCKanaFont, XtRKanaFontStruct, sizeof(XFontStruct *),
        offset(henkan.kana_font),XtRString, XnwDefaultKanaFont},
    {XtNcharSpace, XtCCharSpace, XtRInt, sizeof(int),
	offset(henkan.char_space), XtRImmediate, (caddr_t)4},
};

static void Initialize();
static void Resize();
static void Realize();
static void Redisplay();
static Boolean SetValues();
static void ClassInitialize();
static void Destroy();
static XtGeometryResult QueryGeometry();

HenkanClassRec henkanClassRec = {
  {
/* core_class fields */	
#define superclass		(&simpleClassRec)
    /* superclass	  	*/	(WidgetClass) superclass,
    /* class_name	  	*/	"Henkan",
    /* widget_size	  	*/	sizeof(HenkanRec),
    /* class_initialize   	*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited       	*/	FALSE,
    /* initialize	  	*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize		  	*/	XtInheritRealize,
    /* actions		  	*/	NULL,
    /* num_actions	  	*/	0,
    /* resources	  	*/	resources,
    /* num_resources	  	*/	XtNumber(resources),
    /* xrm_class	  	*/	NULLQUARK,
    /* compress_motion	  	*/	TRUE,
    /* compress_exposure  	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest	  	*/	FALSE,
    /* destroy		  	*/	Destroy,
    /* resize		  	*/	Resize,
    /* expose		  	*/	Redisplay,
    /* set_values	  	*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus	 	*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private   	*/	NULL,
    /* tm_table		   	*/	NULL,
    /* query_geometry		*/	QueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* simple fields */
    /* change sensitive */	XtInheritChangeSensitive
  }
};
WidgetClass henkanWidgetClass = (WidgetClass)&henkanClassRec;
/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static void ClassInitialize()
{
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (caddr_t) XtOffset(Widget, core.screen),
	     sizeof(Screen *)}
    };
    XawInitializeWidgetSet();
    XtAddConverter( XtRString, XtRFontStruct,
                        XnwCvtStringToFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
    XtAddConverter( XtRString, XtRKanjiFontStruct,
                        XnwCvtStringToKanjiFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
    XtAddConverter( XtRString, XtRKanaFontStruct,
                        XnwCvtStringToKanaFontStruct,
                        screenConvertArg, XtNumber(screenConvertArg));
} /* ClassInitialize */

/*
 * Calculate width and height of displayed text in pixels
 */

static void SetTextWidthAndHeight(lw)
    HenkanWidget lw;
{
    register XFontStruct	*fs = lw->henkan.font;
    register XFontStruct        *jfs = lw->henkan.kanji_font;
    register XFontStruct        *kfs = lw->henkan.kana_font;
    Dimension                   height, kanji_height, kana_height;

    lw->henkan.max_ascent = XnwMax3(fs->max_bounds.ascent,
				    jfs->max_bounds.ascent,
				    kfs->max_bounds.ascent);
    lw->henkan.max_descent = XnwMax3(fs->max_bounds.descent,
				     jfs->max_bounds.descent,
				     kfs->max_bounds.descent);

    lw->henkan.char_width = XKanjiTextWidth(
        &lw->henkan.font, " ", 1, fs_jis_euc3_mode);
}

static void GetGCs(lw)
    HenkanWidget lw;
{
    XGCValues	values;

    values.foreground	= lw->henkan.foreground;
    values.background	= lw->core.background_pixel;
    values.font		= lw->henkan.font->fid;

    lw->henkan.normal_GC = XtGetGC(
	(Widget)lw,
	(unsigned) GCForeground | GCBackground | GCFont,
	&values);
    values.font = lw->henkan.kanji_font->fid;
    lw->henkan.kanji_normal_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);

    values.font = lw->henkan.kana_font->fid;
    lw->henkan.kana_normal_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);

    values.foreground   = lw->core.background_pixel;
    values.background   = lw->henkan.foreground;
    values.font         = lw->henkan.font->fid;

    lw->henkan.invert_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);
    values.font = lw->henkan.kanji_font->fid;
    lw->henkan.kanji_invert_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);

    values.font = lw->henkan.kana_font->fid;
    lw->henkan.kana_invert_GC = XtGetGC(
        (Widget)lw,
        (unsigned) GCForeground | GCBackground | GCFont,
        &values);
}

/* ARGSUSED */
static void Initialize(request, new)
 Widget request, new;
{
    HenkanWidget lw = (HenkanWidget) new;

    GetGCs(lw);

    SetTextWidthAndHeight(lw);

    lw->henkan.kanji_status.echoStr = NULL;
    lw->henkan.kanji_status.length = 0;
    lw->henkan.kanji_status.revPos = 0;
    lw->henkan.kanji_status.revLen = 0;
    lw->henkan.kanji_status.info = 0;
    lw->henkan.kanji_status.mode = NULL;
    lw->henkan.kanji_status.gline.line = NULL;
    lw->henkan.kanji_status.gline.length = 0;
    lw->henkan.kanji_status.gline.revPos = 0;
    lw->henkan.kanji_status.gline.revLen = 0;
    if (lw->core.width == 0)
        lw->core.width = lw->henkan.char_width * 10 + lw->henkan.char_space * 2;
    if (lw->core.height == 0)
        lw->core.height = (lw->henkan.max_ascent + lw->henkan.max_descent)
				+ lw->henkan.char_space * 2;

    (*XtClass(new)->core_class.resize) ((Widget)lw);

} /* Initialize */

/*
 * Repaint the widget window
 */

static void DrawKanjiText(w, x, y, str, length, revpos, revlen)
    Widget w;
    int x, y;
    unsigned char *str;
    int length;
    int revpos;
    int revlen;
{
    HenkanWidget lw = (HenkanWidget) w;

    if(revpos) {
        XDrawImageKanjiString( XtDisplay(w), XtWindow(w),
                        &(lw->henkan.normal_GC), &(lw->henkan.font),
                        x, y, str, revpos, fs_jis_euc3_mode);
        x += XKanjiTextWidth(&(lw->henkan.font), str, revpos,
                                                fs_jis_euc3_mode);
    }
    XDrawImageKanjiString( XtDisplay(w), XtWindow(w),
                        &(lw->henkan.invert_GC), &(lw->henkan.font),
                        x, y, str + revpos, revlen, fs_jis_euc3_mode);
    x += XKanjiTextWidth(&(lw->henkan.font), str + revpos, revlen,
                                                fs_jis_euc3_mode);
    if(revpos + revlen < length) {
        XDrawImageKanjiString( XtDisplay(w), XtWindow(w),
                        &(lw->henkan.normal_GC), &(lw->henkan.font),
                        x, y, str + revpos + revlen,
                        length - revpos - revlen, fs_jis_euc3_mode);
    }
}

static MitsuiDebug2(hw)
HenkanWidget hw;
{
    XKanjiStatus *ks = &(hw->henkan.kanji_status);
    printf("XKanjiStatus\n");
    printf("\techoStr=%s\n", ks->echoStr);
    printf("\tlength=%d\n", ks->length);
    printf("\trevPos=%d\n", ks->revPos);
    printf("\trevLen=%d\n", ks->revLen);
    printf("\tinfo=%d\n", ks->info);
    printf("\tmode=%s\n", ks->mode);
    printf("\tgline\n");
    printf("\t\tline=%s\n", ks->gline.line);
    printf("\t\tlength=%d\n", ks->gline.length);
    printf("\t\trevPos=%d\n", ks->gline.revPos);
    printf("\t\trevLen=%d\n", ks->gline.revLen);
}

/* ARGSUSED */
static void Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    HenkanWidget lw = (HenkanWidget) w;
    int x, y;
    int len;
    Boolean cleared = False;

/*
MitsuiDebug2(lw);
*/
    if(!XtIsRealized(w))
	return;
    if(lw->henkan.kanji_status.mode)
	len = strlen(lw->henkan.kanji_status.mode);
    else 
	len = 0;
    x = lw->henkan.char_space;
    y = lw->henkan.char_space;
    XClearArea(XtDisplay(w), XtWindow(w), x, y,
			lw->henkan.char_width * 10 + lw->henkan.char_space,
			lw->henkan.max_ascent + lw->henkan.max_descent, False);
    if(len > 0) {
    	y += lw->henkan.max_ascent;
    	XDrawImageKanjiString( XtDisplay(w), XtWindow(w),
			&(lw->henkan.normal_GC), &(lw->henkan.font), x, y,
			lw->henkan.kanji_status.mode, len, fs_jis_euc3_mode);
    }
    x = lw->henkan.char_space * 2 + lw->henkan.char_width * 10;
    y = lw->henkan.char_space;
    XClearArea(XtDisplay(w), XtWindow(w), x, y, (int)lw->core.width,
			lw->henkan.max_ascent + lw->henkan.max_descent, False);
    if(lw->henkan.kanji_status.gline.length > 0) {
    	y += lw->henkan.max_ascent;
    	DrawKanjiText(w, x, y,
			lw->henkan.kanji_status.gline.line,
			lw->henkan.kanji_status.gline.length,
			lw->henkan.kanji_status.gline.revPos,
			lw->henkan.kanji_status.gline.revLen);
    }
}

static void Realize(w, valueMask, attributes)
    register Widget w;
    Mask *valueMask;
    XSetWindowAttributes *attributes;
{
    Pixmap border_pixmap;

    if (!XtIsSensitive(w)) {
        /* change border to gray; have to remember the old one,
         * so XtDestroyWidget deletes the proper one */
        if (((SimpleWidget)w)->simple.insensitive_border == NULL)
            ((SimpleWidget)w)->simple.insensitive_border =
                XmuCreateStippledPixmap(XtScreen(w),
                                        w->core.border_pixel,
                                        w->core.background_pixel,
                                        w->core.depth);
        border_pixmap = w->core.border_pixmap;
        attributes->border_pixmap =
          w->core.border_pixmap = ((SimpleWidget)w)->simple.insensitive_border;

        *valueMask |= CWBorderPixmap;
        *valueMask &= ~CWBorderPixel;
    }

    if ((attributes->cursor = ((SimpleWidget)w)->simple.cursor) != None)
        *valueMask |= CWCursor;

    XtCreateWindow( w, (unsigned int)InputOutput, (Visual *)CopyFromParent,
                    *valueMask, attributes );

    if (!XtIsSensitive(w))
        w->core.border_pixmap = border_pixmap;

/*
    XKanjiControl(XtDisplay(w), XtWindow(w), KC_INITIALIZE, NULL);
*/
}

static void Resize(w)
    Widget w;
{
    HenkanWidget hw = (HenkanWidget)w;
    Dimension width;

    if(XtIsRealized(w))
       	(*(XtClass(w))->core_class.expose)(w, NULL, NULL);
/*
    width = hw->core.width / hw->henkan.char_width;
    XKanjiControl(XtDisplay(w), XtWindow(w), KC_SETWIDTH, (caddr_t)width);
*/
}

/*
 * Set specified arguments into widget
 */

/* ARGSUSED */
static Boolean SetValues(current, request, new)
    Widget current, request, new;
{
    HenkanWidget curlw = (HenkanWidget) current;
    HenkanWidget reqlw = (HenkanWidget) request;
    HenkanWidget newlw = (HenkanWidget) new;
    Boolean was_resized = False;
    Boolean redisplay = False;

    if (was_resized
	|| (curlw->henkan.font != newlw->henkan.font)
        || (curlw->henkan.kanji_font != newlw->henkan.kanji_font)
        || (curlw->henkan.kana_font != newlw->henkan.kana_font)) {

	SetTextWidthAndHeight(newlw);
	was_resized = True;

	}

    if (curlw->henkan.foreground != newlw->henkan.foreground
        || curlw->henkan.font->fid != newlw->henkan.font->fid
        || curlw->henkan.kanji_font->fid != newlw->henkan.kanji_font->fid
        || curlw->henkan.kana_font->fid != newlw->henkan.kana_font->fid) {

	XtReleaseGC(new, curlw->henkan.normal_GC);
        XtReleaseGC(new, curlw->henkan.kanji_normal_GC);
        XtReleaseGC(new, curlw->henkan.kana_normal_GC);
	XtReleaseGC(new, curlw->henkan.invert_GC);
        XtReleaseGC(new, curlw->henkan.kanji_invert_GC);
        XtReleaseGC(new, curlw->henkan.kana_invert_GC);
	GetGCs(newlw);
	redisplay = True;
    }

    return was_resized || redisplay;
}

static void Destroy(w)
    Widget w;
{
    HenkanWidget lw = (HenkanWidget)w;

    if(lw->henkan.kanji_status.echoStr)
    	XtFree( lw->henkan.kanji_status.echoStr );
    if(lw->henkan.kanji_status.gline.line)
    	XtFree( lw->henkan.kanji_status.gline.line );

/*
    XKanjiControl(XtDisplay(w), XtWindow(w), KC_FINALIZE, NULL);
*/
    XtReleaseGC( w, lw->henkan.normal_GC );
    XtReleaseGC( w, lw->henkan.kanji_normal_GC);
    XtReleaseGC( w, lw->henkan.kana_normal_GC);
    XtReleaseGC( w, lw->henkan.invert_GC);
    XtReleaseGC( w, lw->henkan.kanji_invert_GC);
    XtReleaseGC( w, lw->henkan.kana_invert_GC);
}


static XtGeometryResult QueryGeometry(w, intended, preferred)
    Widget w;
    XtWidgetGeometry *intended, *preferred;
{
    register HenkanWidget lw = (HenkanWidget)w;

    preferred->request_mode = CWWidth | CWHeight;
    preferred->width = lw->henkan.char_width * 10 + lw->henkan.char_space * 2;
    preferred->height = (lw->henkan.max_ascent + lw->henkan.max_descent)
                                + lw->henkan.char_space * 2;
    if (  ((intended->request_mode & (CWWidth | CWHeight))
	   	== (CWWidth | CWHeight)) &&
	  intended->width == preferred->width &&
	  intended->height == preferred->height)
	return XtGeometryYes;
    else if (preferred->width == w->core.width &&
	     preferred->height == w->core.height)
	return XtGeometryNo;
    else
	return XtGeometryAlmost;
}

void XnwSetHenkanStatus(w, new_status)
Widget w;
XKanjiStatus *new_status;
{
    HenkanWidget hw = (HenkanWidget)w;
    XKanjiStatus *current_status = &(hw->henkan.kanji_status);
    Boolean redisplay = False;

    current_status->info = new_status->info;
    if(current_status->info & KanjiModeInfo) {
  	int len = strlen(new_status->mode);
	if(current_status->mode)
            XtFree(current_status->mode);
        current_status->mode =
		(unsigned char *)XtMalloc(len + 1);
        strncpy(current_status->mode, new_status->mode,
						strlen(new_status->mode));
	current_status->mode[len] = '\0';
	redisplay = True;
    }
    if(current_status->info & KanjiGLineInfo) {
        if(current_status->gline.line)
            XtFree(current_status->gline.line);
        current_status->gline.line =
		(unsigned char *)XtMalloc(new_status->gline.length + 1);
        strncpy(current_status->gline.line, new_status->gline.line,
                                                new_status->gline.length);
        current_status->gline.line[new_status->gline.length] = '\0';
        current_status->gline.length = new_status->gline.length;
        current_status->gline.revPos = new_status->gline.revPos;
        current_status->gline.revLen = new_status->gline.revLen;
        redisplay = True;
    }
    if(redisplay)
	(*hw->core.widget_class->core_class.expose)(w, NULL, NULL);
}

void XnwClearHenkanStatus(w)
Widget w;
{
    HenkanWidget hw = (HenkanWidget)w;
    XKanjiStatus *current_status = &(hw->henkan.kanji_status);

    if(current_status->gline.line)
        XtFree(current_status->gline.line);
    current_status->gline.line = NULL;
    current_status->gline.length = 0;
    current_status->gline.revPos = 0;
    current_status->gline.revLen = 0;
    (*hw->core.widget_class->core_class.expose)(w, NULL, NULL);
}
