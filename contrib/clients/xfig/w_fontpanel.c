/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 * Added by Brian V. Smith
 *
 * "Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty."
 *
 */

#include "fig.h"
#include "resources.h"
#include "u_fonts.h"		/* printer font names */
#include "w_setup.h"
#include "w_util.h"

/********************  global variables  ***************************/

extern char    *psfont_menu_bits[];
extern char    *latexfont_menu_bits[];
extern Pixmap   psfont_menu_bitmaps[];
extern Pixmap   latexfont_menu_bitmaps[];
extern struct _fstruct ps_fontinfo[];	/* font names */
extern struct _fstruct latex_fontinfo[];	/* font names */

/* LOCAL VARIABLES */

static int     *font_ps_sel;	/* ptr to store selected ps font in */
static int     *font_latex_sel;	/* ptr to store selected latex font */
static int     *flag_sel;	/* pointer to store ps/latex flag */
static TOOL     font_widget;	/* widget adr to store font image in */
static int      (*font_setimage) ();

/********************  local variables  ***************************/

static MENUITEM ps_fontmenu_items[NUM_PS_FONTS];
static MENUITEM latex_fontmenu_items[NUM_LATEX_FONTS];

static void     fontpane_select();
static void     fontpane_cancel();
static void     fontpane_swap();

static XtCallbackRec pane_callbacks[] =
{
    {fontpane_select, NULL},
    {NULL, NULL},
};

static TOOL     ps_fontpanes, ps_buttons;
static TOOL     latex_fontpanes, latex_buttons;
static TOOL     ps_fontpane[NUM_PS_FONTS];
static TOOL     latex_fontpane[NUM_LATEX_FONTS];

init_fontmenu(tool)
    TOOL            tool;
{
    TOOL            tmp_but;
    register int    i;
    register MENUITEM *mi;
    XtTranslations  pane_actions;

    DeclareArgs(8);

    FirstArg(XtNborderWidth, POPUP_BW);
    NextArg(XtNmappedWhenManaged, False);

    ps_fontmenu = XtCreatePopupShell("xfig: ps font menu",
				     transientShellWidgetClass, tool,
				     Args, ArgCount);
    latex_fontmenu = XtCreatePopupShell("xfig: latex font menu",
					transientShellWidgetClass, tool,
					Args, ArgCount);

    FirstArg(XtNvSpace, -INTERNAL_BW);
    NextArg(XtNhSpace, 0);

    ps_fontpanes = XtCreateManagedWidget("menu", boxWidgetClass,
					 ps_fontmenu, Args, ArgCount);
    latex_fontpanes = XtCreateManagedWidget("menu", boxWidgetClass,
					    latex_fontmenu, Args, ArgCount);

    for (i = 0; i < NUM_PS_FONTS; i++) {
	ps_fontmenu_items[i].type = MENU_IMAGESTRING;	/* put the fontnames in
							 * menu */
	ps_fontmenu_items[i].label = ps_fontinfo[i].name;
	ps_fontmenu_items[i].info = (caddr_t) i;	/* index for font # */
    }

    for (i = 0; i < NUM_LATEX_FONTS; i++) {
	latex_fontmenu_items[i].type = MENU_IMAGESTRING;	/* put the fontnames in
								 * menu */
	latex_fontmenu_items[i].label = latex_fontinfo[i].name;
	latex_fontmenu_items[i].info = (caddr_t) i;	/* index for font # */
    }

    FirstArg(XtNwidth, PS_FONTPANE_WD);
    NextArg(XtNdefaultDistance, INTERNAL_BW);
    NextArg(XtNborderWidth, 0);
    ps_buttons = XtCreateManagedWidget("buttons", formWidgetClass,
				       ps_fontpanes, Args, ArgCount);
    latex_buttons = XtCreateManagedWidget("buttons", formWidgetClass,
					  latex_fontpanes, Args, ArgCount);

    i = (int) ((PS_FONTPANE_WD - INTERNAL_BW) / 3);
    FirstArg(XtNwidth, i);
    NextArg(XtNborderWidth, 0);
    tmp_but = XtCreateManagedWidget("cancel", commandWidgetClass,
				    ps_buttons, Args, ArgCount);
    XtAddEventHandler(tmp_but, ButtonReleaseMask, (Boolean) 0,
		      fontpane_cancel, (XtPointer) NULL);

    FirstArg(XtNfromHoriz, tmp_but);
    NextArg(XtNwidth, PS_FONTPANE_WD - INTERNAL_BW - i);
    NextArg(XtNborderWidth, 0);
    tmp_but = XtCreateManagedWidget("use latex fonts", commandWidgetClass,
				    ps_buttons, Args, ArgCount);
    XtAddEventHandler(tmp_but, ButtonReleaseMask, (Boolean) 0,
		      fontpane_swap, (XtPointer) NULL);

    FirstArg(XtNwidth, i);
    NextArg(XtNborderWidth, 0);
    tmp_but = XtCreateManagedWidget("cancel", commandWidgetClass,
				    latex_buttons, Args, ArgCount);
    XtAddEventHandler(tmp_but, ButtonReleaseMask, (Boolean) 0,
		      fontpane_cancel, (XtPointer) NULL);

    FirstArg(XtNfromHoriz, tmp_but);
    NextArg(XtNwidth, PS_FONTPANE_WD - INTERNAL_BW - i);
    NextArg(XtNborderWidth, 0);
    tmp_but = XtCreateManagedWidget("use postscript fonts", commandWidgetClass,
				    latex_buttons, Args, ArgCount);
    XtAddEventHandler(tmp_but, ButtonReleaseMask, (Boolean) 0,
		      fontpane_swap, (XtPointer) NULL);

    pane_actions = XtParseTranslationTable("<EnterWindow>:set()\n\
		<Btn1Up>:notify()unset()\n");

    FirstArg(XtNwidth, PS_FONTPANE_WD);
    NextArg(XtNheight, PS_FONTPANE_HT);
    NextArg(XtNcallback, pane_callbacks);
    NextArg(XtNbitmap, NULL);
    NextArg(XtNinternalWidth, 0);	/* space between pixmap and edge */
    NextArg(XtNinternalHeight, 0);
    NextArg(XtNborderWidth, INTERNAL_BW);
    NextArg(XtNresize, False);	/* don't allow resize */

    for (i = 0; i < NUM_PS_FONTS; ++i) {
	mi = &ps_fontmenu_items[i];
	pane_callbacks[0].closure = (caddr_t) mi;
	ps_fontpane[i] = XtCreateManagedWidget("pane", commandWidgetClass,
					       ps_fontpanes, Args, ArgCount);
	XtOverrideTranslations(ps_fontpane[i], pane_actions);
    }

    for (i = 0; i < NUM_LATEX_FONTS; ++i) {
	mi = &latex_fontmenu_items[i];
	pane_callbacks[0].closure = (caddr_t) mi;
	latex_fontpane[i] = XtCreateManagedWidget("pane", commandWidgetClass,
					   latex_fontpanes, Args, ArgCount);
	XtOverrideTranslations(latex_fontpane[i], pane_actions);
    }

    return (1);
}

/* create the bitmaps for the font menu */

setup_fontmenu()
{
    register int    i;

    DeclareArgs(2);

    Pixel           bg, fg;

    /* get the foreground/background of the widget */
    FirstArg(XtNforeground, &fg);
    NextArg(XtNbackground, &bg);
    GetValues(ps_fontpane[0]);

    /* Create the bitmaps */

    for (i = 0; i < NUM_PS_FONTS; i++)
	psfont_menu_bitmaps[i] = XCreatePixmapFromBitmapData(tool_d,
				   XtWindow(ind_panel), psfont_menu_bits[i],
				     PS_FONTPANE_WD, PS_FONTPANE_HT, fg, bg,
					     XDefaultDepthOfScreen(tool_s));

    for (i = 0; i < NUM_LATEX_FONTS; i++)
	latexfont_menu_bitmaps[i] = XCreatePixmapFromBitmapData(tool_d,
				XtWindow(ind_panel), latexfont_menu_bits[i],
			       LATEX_FONTPANE_WD, LATEX_FONTPANE_HT, fg, bg,
					     XDefaultDepthOfScreen(tool_s));

    /* Store the bitmaps in the menu panes */
    for (i = 0; i < NUM_PS_FONTS; i++) {
	FirstArg(XtNbitmap, psfont_menu_bitmaps[i]);
	SetValues(ps_fontpane[i]);
    }
    for (i = 0; i < NUM_LATEX_FONTS; i++) {
	FirstArg(XtNbitmap, latexfont_menu_bitmaps[i]);
	SetValues(latex_fontpane[i]);
    }

    FirstArg(XtNbackground, BlackPixelOfScreen(tool_s));
    SetValues(ps_buttons);
    SetValues(latex_buttons);

    XtRealizeWidget(ps_fontmenu);
    XtRealizeWidget(latex_fontmenu);
    /* at this point the windows are realized but not drawn */
    XDefineCursor(tool_d, XtWindow(ps_fontpanes),
		  (Cursor) arrow_cursor.bitmap);
    XDefineCursor(tool_d, XtWindow(latex_fontpanes),
		  (Cursor) arrow_cursor.bitmap);
}

void
fontpane_popup(psfont_adr, latexfont_adr, psflag_adr, showfont_fn, show_widget)
    int            *psfont_adr, *latexfont_adr, *psflag_adr;
    int             (*showfont_fn) ();
Widget          show_widget;

{
    DeclareArgs(2);
    Position        xposn, yposn, dummy;

    font_ps_sel = psfont_adr;
    font_latex_sel = latexfont_adr;
    flag_sel = psflag_adr;
    font_setimage = showfont_fn;
    font_widget = show_widget;
    XtTranslateCoords(show_widget, (Position) 0, (Position) 0, &xposn, &dummy);
    XtTranslateCoords(tool, (Position) 0, (Position) 0, &dummy, &yposn);
    FirstArg(XtNx, xposn);
    NextArg(XtNy, yposn - 20);	/* up a little bit from top of tool */
    SetValues(ps_fontmenu);
    SetValues(latex_fontmenu);
    XtPopup(*flag_sel ? ps_fontmenu : latex_fontmenu, XtGrabNonexclusive);
}

static void
fontpane_select(widget, mi)
    TOOL            widget;
    MENUITEM       *mi;
{
    char           *font_name = mi->label;

    if (*flag_sel)
	*font_ps_sel = (int) mi->info;	/* set ps font to one selected */
    else
	*font_latex_sel = (int) mi->info;	/* set latex font to one
						 * selected */
    put_msg("Font: %s", font_name);
    /* put image of font in indicator window */
    (*font_setimage) (font_widget);
    XtPopdown(*flag_sel ? ps_fontmenu : latex_fontmenu);
}

static void
fontpane_cancel()
{
    XtPopdown(*flag_sel ? ps_fontmenu : latex_fontmenu);
}

static void
fontpane_swap()
{
    XtPopdown(*flag_sel ? ps_fontmenu : latex_fontmenu);
    *flag_sel = 1 - *flag_sel;
    /* put image of font in indicator window */
    (*font_setimage) (font_widget);
    XtPopup(*flag_sel ? ps_fontmenu : latex_fontmenu, XtGrabNonexclusive);
}
