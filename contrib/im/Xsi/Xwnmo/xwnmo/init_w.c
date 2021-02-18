/*
 * $Id: init_w.c,v 1.12 1991/09/25 16:07:10 proj Exp $
 */
/*
 * Copyright 1989, 1990, 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				
/*	Version 4.0
 */
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include "sdefine.h"
#include "xim.h"
#include "Xlocaleint.h"
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "proto.h"
#include "sheader.h"
#include "ext.h"

Display		*dpy = 0;
int		screen;
Window		root_window;
Atom		xim_id;
XPoint		button;

Atom	wm_id = 0;	/* Window Manager */
Atom	wm_id1 = 0;	/* Window Manager */

Cursor	cursor1, cursor2, cursor3;

XSetWindowAttributes	attributes;
XGCValues	xgcv;
XSizeHints	hints;
XClassHint	class_hints;
XWMHints	wm_hints;
#ifndef	X11R3
XTextProperty	winName, iconName;
#else	/* X11R3 */
char	*winName, *iconName;
#endif	/* X11R3 */
XEvent 		event;

wchar_t	dol_wchar_t;
wchar_t	cursor_wchar_t;

static struct _resource {
    char *servername_res;
    char *username_res;
    char *ximrc_file_res;
    char *cvt_fun_file_res;
    char *cvt_meta_file_res;
    Pixel fore_color_res;
    Pixel back_color_res;
    Pixel border_color_res;
    char *display_name_res;
    char *geometry_res;
    char *icon_geometry_res;
    int border_width_res;
#ifdef	USING_XJUTIL
    char *xjutil_res;
#endif	/* USING_XJUTIL */
    Boolean iconic_start_res;
    char *lang_name_res;
    char *font_set_res;
} resource;

#define	DEFAULT_WIDTH	1280
#define	DEFAULT_BORDERWIDTH	4
#define	DEFAULT_X	0
#define	DEFAULT_Y	0

static int defaultBorderWidth = DEFAULT_BORDERWIDTH;
static defaultFALSE = FALSE;
#define offset(field)	XtOffset(struct _resource *, field)

static XtResource app_resources[] = {
    {"serverName", "ServerName", XtRString, sizeof (char *),
	offset(servername_res), XtRString, NULL},
    {"userName", "UserName", XtRString, sizeof (char *),
	offset(username_res), XtRString, NULL},
    {"ximrcName", "XimrcName", XtRString, sizeof (char *),
	offset(ximrc_file_res), XtRString, NULL},
    {"cvtfunName", "CvtfunName", XtRString, sizeof (char *),
	offset(cvt_fun_file_res), XtRString, NULL},
    {"cvtmetaName", "CvtmetaName", XtRString, sizeof (char *),
	offset(cvt_meta_file_res), XtRString, NULL},
    {XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
	offset(fore_color_res), XtRString, "XtDefaultForeground"},
    {XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	offset(back_color_res), XtRString, "XtDefaultBackground"},
    {XtNborderColor, XtCBorderColor, XtRPixel, sizeof(Pixel),
	offset(border_color_res), XtRString, "XtDefaultForeground"},
    {"display", "Display", XtRString, sizeof (char *),
	offset(display_name_res), XtRString, NULL},
#ifdef	USING_XJUTIL
    {"xjutilName", "XjutilName", XtRString, sizeof (char *),
	offset(xjutil_res), XtRString, NULL},
#endif	/* USING_XJUTIL */
    {XtNiconic, XtCIconic, XtRBoolean, sizeof (Boolean),
	offset(iconic_start_res), XtRBoolean, (caddr_t) &defaultFALSE},
    {XtNgeometry, XtCGeometry, XtRString, sizeof (char *),
	offset(geometry_res), XtRString, (caddr_t) NULL},
    {"iconGeometry", "IconGeometry", XtRString, sizeof (char *),
	offset(icon_geometry_res), XtRString, (caddr_t) NULL},
    {XtNborderWidth, XtCBorderWidth, XtRInt, sizeof (int),
	offset(border_width_res), XtRInt, (caddr_t) &defaultBorderWidth},
    {"langName", "LangName", XtRString, sizeof (char *),
	offset(lang_name_res), XtRString, DEFAULT_LANG},
    {"fontSet", "FontSet", XtRString, sizeof (char *),
	offset(font_set_res), XtRString,  NULL},
};
#undef	offset

/*
static char *fallback_resources[] = {
};
*/

static XrmOptionDescRec	optionDescList[] = {
{ "-D",		"*serverName",	XrmoptionSepArg,	(caddr_t) NULL},
{ "-n",		"*userName",	XrmoptionSepArg,	(caddr_t) NULL},
{ "-F",		"*cvtfunName",	XrmoptionSepArg,	(caddr_t) NULL},
{ "-M",		"*cvtmetaName",	XrmoptionSepArg,	(caddr_t) NULL},
#ifdef	USING_XJUTIL
{ "-ju",	"*xjutilName",	XrmoptionSepArg,	(caddr_t) NULL},
#endif	/* USING_XJUTIL */
{ "-lc",	"*langName",	XrmoptionSepArg,	(caddr_t) NULL},
{ "-fs",	"*fontSet",	XrmoptionSepArg,	(caddr_t) NULL},
{ "#",		".iconGeometry",XrmoptionStickyArg,	(caddr_t) NULL},
};

int
get_application_resources(argc, argv)
int argc;
char **argv;
{
	Widget	toplevel;
	XtAppContext app;

	toplevel = XtAppInitialize(&app, "Xwnmo", optionDescList,
				   XtNumber(optionDescList),
				   &argc, (String*)argv,
				   (String*)NULL, (ArgList)NULL, (Cardinal)0);
	XtGetApplicationResources(toplevel, (XtPointer) &resource,
				  app_resources,
				  XtNumber(app_resources), NULL, 0 );

    root_def_servername = root_def_reverse_servername = resource.servername_res;
    root_username = resource.username_res;
    ximrc_file = resource.ximrc_file_res;
    cvt_fun_file = resource.cvt_fun_file_res;
    cvt_meta_file = resource.cvt_meta_file_res;
    display_name = resource.display_name_res;
#ifdef	USING_XJUTIL
    xjutil_name = resource.xjutil_res;
#endif	/* USING_XJUTIL */
    def_lang = resource.lang_name_res;
    return(argc);
}

static int
xerror_handler(d, ev)
Display	*d;
register XErrorEvent *ev;
{
    return(0);
}

static int
xioerror_handler(d)
Display	d;
{
    perror("xwnmo");
    epilogue();
    shutdown(dpy->fd, 2);
    close(dpy->fd);
    exit(2);
    return(0);
}

#define	hname_size	128
static void
write_ximid(port)
short port;
{
    char name[hname_size];
    unsigned char buffer[hname_size * 2];
    register unsigned char *p;
    unsigned short pnumber;
    long major_version;
    long minor_version;
    extern int gethostname();

    if (!(xim_id = XInternAtom(dpy, XIM_INPUTMETHOD, True))) {
	xim_id = XInternAtom(dpy, XIM_INPUTMETHOD, False);
    }
    major_version = htonl(XIM_MAJOR_VERSION);
    minor_version = htonl(XIM_MINOR_VERSION);
    pnumber = (unsigned short)htons(port);
    gethostname(name, hname_size);
    p = buffer;
    bcopy(name, p, hname_size);
    bcopy(&pnumber, (p += hname_size), sizeof(unsigned short));
    bcopy(&major_version, (p += sizeof(unsigned short)), sizeof(long));
    bcopy(&minor_version, (p += sizeof(long)), sizeof(long));
    *(p += sizeof(long)) = '\0';

    XChangeProperty(dpy, root_window, xim_id, XA_STRING, 8,
          PropModeReplace, buffer, (p - buffer));

    XSetSelectionOwner(dpy, xim_id, xim->ximclient->w,  0L);
}

static void
clear_xl(x)
register XIMLangRec *x;
{
    x->linefeed[0] = x->linefeed[1] = x->linefeed[2] = 0;
    x->vst = 0;
    x->max_l1 = x->max_l2 = x->max_l3 = 0;
    x->note[0] = x->note[1] = 0;
    x->currentcol0 = x->currentcol = 0;
    x->max_pos0 = x->max_pos = 0;
    x->max_cur = 0;
    x->del_x = 0;
    x->cursor_flag = 0;
    x->mark_flag = 0;
    x->u_line_flag = x->r_flag = x->b_flag = x->visible = 0;
    x->visible_line = 0;
    x->m_cur_flag = 0;
}

static XFontSet
get_default_font_set(lang)
register char *lang;
{
    register XIMLangDataBase *db = NULL;

    for (db = language_db; db->lang != NULL; db++) {
	if (!strcmp(db->lang, lang)) {
	    return((XFontSet)create_font_set(db->fontset_name));
	}
    }
    return((XFontSet)NULL);
}

static void
create_ichi() 
{
    register Ichiran *ichi = xim->ichi;
    register XIMLangRec *xl = xim->ximclient->xl[0];
    register unsigned int width, height, bwidth;
    unsigned long fg, bg;
    int i;

    alloc_for_save_buf();
    ichi->max_button = MAX_BUTTON;

    width = xim->ximclient->client_area.width;
    height = xim->ximclient->client_area.height;
    bwidth = resource.border_width_res;
    fg = xim->ximclient->pe.fg;
    bg = xim->ximclient->pe.bg;

    ichi->w = XCreateSimpleWindow(dpy, root_window, 0, 0,
				  width, height, bwidth, fg, bg);
    ichi->w0 = XCreateSimpleWindow(dpy, ichi->w, 0, FontWidth(xl) * 4,
				   width, height, 1, fg, bg);
    ichi->w1 = XCreateSimpleWindow(dpy, ichi->w0, 0, FontWidth(xl) * 4,
				   width, height, 0, fg, bg);
    ichi->nyuu_w = XCreateSimpleWindow(dpy, ichi->w, 0, 0,
				       FontWidth(xl) * 5, FontHeight(xl),
				       1, fg, bg);
			
    XDefineCursor(dpy, ichi->w, cursor3);
    XSelectInput(dpy, ichi->w, KeyPressMask|StructureNotifyMask);
    XSelectInput(dpy, ichi->w1, ExposureMask);
    XSelectInput(dpy, ichi->nyuu_w, ExposureMask);

    ichi->title = create_box(ichi->w, 0, 0, FontWidth(xl), FontHeight(xl),
			     1, fg, bg, 0, '\0');
    ichi->subtitle = create_box(ichi->w, 0, 0, FontWidth(xl), FontHeight(xl),
				1, fg, bg, 0, '\0');
    ichi->title->redraw_cb = set_j_c;
    ichi->subtitle->redraw_cb = set_j_c;
    for (i = 0; i < ichi->max_button; i++) {
	ichi->button[i] = create_box(ichi->w, 0, 0, FontWidth(xl),
				     FontHeight(xl), 2, fg, bg, cursor2, 1);
	ichi->button[i]->redraw_cb = set_j_c;
    }
    ichi->button[0]->do_ret = ichi->button[1]->do_ret = True;
    ichi->button[0]->sel_ret = -2;
    ichi->button[1]->sel_ret = 1;
    ichi->button[2]->cb = xw_mouse_select;
    ichi->button[3]->cb = next_ichi;
    ichi->button[4]->cb = back_ichi;
    map_box(ichi->title);
    map_box(ichi->button[CANCEL_W]);

#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"List";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"List";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "List";
    iconName = "List";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = SouthWestGravity;
    /*
    hints.win_gravity = CenterGravity;
    */
#endif	/* X11R3 */
    class_hints.res_name = "list";
    class_hints.res_class = "List";
    wm_hints.input = True;
    wm_hints.flags = InputHint;

#ifndef	X11R3
    XSetWMProperties(dpy, ichi->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, ichi->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, ichi->w, &wm_hints);
    XSetClassHint(dpy, ichi->w, &class_hints);
#endif	/* X11R3 */

    XMapWindow(dpy, ichi->w1);
    XFlush(dpy);
}

static void
create_inspect()
{
    Inspect *ins = xim->inspect;
    XIMLangRec *xl = xim->ximclient->xl[0];
    unsigned int width, height, bwidth;
    unsigned long fg, bg;
    int i;

    ins->max_button = MAX_BUTTON;

    width = xim->ximclient->client_area.width;
    height = xim->ximclient->client_area.height;
    bwidth = resource.border_width_res;
    fg = xim->ximclient->pe.fg;
    bg = xim->ximclient->pe.bg;

    ins->w = XCreateSimpleWindow(dpy, root_window, 0, 0,
				 width, height, bwidth, fg, bg);
    ins->w1 = XCreateSimpleWindow(dpy, ins->w, 0, FontHeight(xl) * 2,
				  width, height, 1, fg, bg);
    XDefineCursor(dpy, ins->w, cursor3);
    XSelectInput(dpy, ins->w, KeyPressMask|StructureNotifyMask);
    XSelectInput(dpy, ins->w1, ExposureMask);

    ins->title = create_box(ins->w, 0, 0, FontWidth(xl), FontHeight(xl), 1,
			    fg, bg, 0, '\0');
    ins->title->redraw_cb = set_j_c;
    for (i = 0; i < ins->max_button; i++) {
	ins->button[i] = create_box(ins->w, 0, 0, FontWidth(xl),
				    FontHeight(xl) , 2, fg, bg, cursor2, 1);
	ins->button[i]->redraw_cb = set_j_c;
    }
    ins->button[0]->do_ret = ins->button[1]->do_ret
			   = ins->button[2]->do_ret = True;
    ins->button[0]->sel_ret = -2;
    ins->button[1]->sel_ret = 0;
    ins->button[2]->sel_ret = 1;
    ins->button[3]->cb = xw_next_inspect;
    ins->button[4]->cb = xw_back_inspect;
    ins->button[3]->cb_data = ins->button[4]->cb_data = NULL;

#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"Inspect";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"Inspect";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "Inspect";
    iconName = "Inspect";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = SouthWestGravity;
#endif	/* X11R3 */
    class_hints.res_name = "inspect";
    class_hints.res_class = "Inspect";
    wm_hints.input = True;
    wm_hints.flags = InputHint;
#ifndef	X11R3
    XSetWMProperties(dpy, ins->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, ins->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, ins->w, &wm_hints);
    XSetClassHint(dpy, ins->w, &class_hints);
#endif	/* X11R3 */
    XMapSubwindows(dpy, ins->w);
    XFlush(dpy);
}

void
create_yes_no(xc)
XIMClientRec *xc;
{
    XIMLangRec *xl = xc->xl[0];
    YesOrNo *yes_no = xc->yes_no;
    register unsigned int width, height, bwidth;
    unsigned long fg, bg;

    width = 1;
    height = 1;
    bwidth = resource.border_width_res;
    fg = xc->pe.fg;
    bg = xc->pe.bg;

    yes_no->w = XCreateSimpleWindow(dpy, root_window, 0, 0,
				    width, height, bwidth, fg, bg);
    XDefineCursor(dpy, yes_no->w, cursor3);
    XSelectInput(dpy, yes_no->w, (StructureNotifyMask | KeyPressMask));
    yes_no->title = create_box(yes_no->w, 0, 0, width, height, 0, fg, bg,
			       0, '\0');
    yes_no->title->redraw_cb = cur_cl_change4;
    yes_no->title->redraw_cb_data = (int *)xc;
    map_box(yes_no->title);
    yes_no->button[0] = create_box(yes_no->w, (FontWidth(xl) * 2),
				   (FontHeight(xl) * 2 + FontHeight(xl)/2),
				   (FontWidth(xl) * 4 + IN_BORDER * 2),
				   (FontHeight(xl) + IN_BORDER * 2), bwidth,
				   fg, bg, cursor2, 1);
    yes_no->button[1] = create_box(yes_no->w, (FontWidth(xl) * 10),
				   (FontHeight(xl) * 2 + FontHeight(xl)/2),
				   (FontWidth(xl) * 6 + IN_BORDER * 2),
				   (FontHeight(xl) + IN_BORDER * 2), bwidth,
				   fg, bg, cursor2, 1);
    yes_no->button[0]->redraw_cb = cur_cl_change3;
    yes_no->button[1]->redraw_cb = cur_cl_change3;
    yes_no->button[0]->redraw_cb_data = (int *)xc;
    yes_no->button[1]->redraw_cb_data = (int *)xc;
    yes_no->button[0]->sel_ret = 1;
    yes_no->button[1]->sel_ret = 0;
    yes_no->button[0]->cb = yes_no->button[1]->cb = cur_cl_change3;
    yes_no->button[0]->cb_data = yes_no->button[1]->cb_data = (int *)xc;
    yes_no->button[0]->do_ret = yes_no->button[1]->do_ret = True;

#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"YesNo";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"YesNo";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "YesNo";
    iconName = "YesNo";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );

    hints.x = 0;
    hints.y = 0;
    hints.width = hints.min_width = hints.max_width = width;
    hints.height = hints.min_height = hints.max_height = height;
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = width;
    hints.base_height = height;
    hints.win_gravity = SouthWestGravity;
#endif	/* X11R3 */
    class_hints.res_name = "yesNo";
    class_hints.res_class = "YesNo";
    wm_hints.input = True;
    wm_hints.flags = InputHint;
#ifndef	X11R3
    XSetWMProperties(dpy, yes_no->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, yes_no->w, winName, iconName, None, NULL, 0,
			   &hints);
    XSetWMHints(dpy, yes_no->w, &wm_hints);
    XSetClassHint(dpy, yes_no->w, &class_hints);
#endif	/* X11R3 */
    XFlush(dpy);
}

void
read_wm_id()
{
    if ((wm_id = XInternAtom(dpy, "WM_PROTOCOLS", True))) {
	wm_id1 = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
	XChangeProperty(dpy, xim->ximclient->w, wm_id, XA_INTEGER,
			32, PropModeReplace, (unsigned char*)&wm_id1,
			sizeof(Atom));
	XChangeProperty(dpy, xim->ichi->w,      wm_id, XA_INTEGER,
			32, PropModeReplace, (unsigned char*)&wm_id1,
			sizeof(Atom));
	XChangeProperty(dpy, xim->inspect->w,   wm_id, XA_INTEGER,
			32, PropModeReplace, (unsigned char*)&wm_id1,
			sizeof(Atom));
	/*
	XChangeProperty(dpy, xim->yes_no->w,    wm_id, XA_INTEGER,
			32, PropModeReplace, (unsigned char*)&wm_id1,
			sizeof(Atom));
	*/
    } else {
	wm_id = wm_id1 = 0;
    }
}

static void
create_xim_window(xc)
XIMClientRec *xc;
{
    int icon_x, icon_y;
    unsigned int icon_width, icon_height;
    int icon_mask = 0;

    if (resource.icon_geometry_res) {
	icon_mask = XParseGeometry(resource.icon_geometry_res,
				   &icon_x, &icon_y, &icon_width, &icon_height);
    }

    xc->w = XCreateSimpleWindow(dpy, root_window, ClientX(xc), ClientY(xc),
				ClientWidth(xc), ClientHeight(xc),
				resource.border_width_res,
				xc->pe.fg, xc->pe.bg);
    XSetWindowBorder(dpy, xc->w, resource.border_color_res);
    XDefineCursor(dpy, xc->w, cursor3);

#ifndef	X11R3
    winName.encoding = XA_STRING;
    winName.format = 8;
    winName.value = (unsigned char *)"Xwnmo";
    winName.nitems = strlen((char*)winName.value);
    iconName.encoding = XA_STRING;
    iconName.format = 8;
    iconName.value = (unsigned char *)"Xwnmo";
    iconName.nitems = strlen((char*)winName.value);
#else	/* X11R3 */
    winName = "Xwnmo";
    iconName = "Xwnmo";
#endif	/* X11R3 */

    hints.flags = (USSize | USPosition | PMinSize | PMaxSize | PResizeInc
#ifndef	X11R3
		   | PBaseSize | PWinGravity
#endif	/* X11R3 */
		  );
    hints.x = ClientX(xc);
    hints.y = ClientY(xc);
    hints.width = hints.min_width = hints.max_width = ClientWidth(xc);
    hints.height = hints.min_height = hints.max_height =  ClientHeight(xc);
    hints.width_inc = hints.height_inc = 0;
#ifndef	X11R3
    hints.base_width = ClientWidth(xc);
    hints.base_height = ClientHeight(xc);
    hints.win_gravity = SouthWestGravity;
#endif	/* X11R3 */
    class_hints.res_name = "xwnmo";
    class_hints.res_class = "Xwnmo";
    wm_hints.input = False;
    if (resource.iconic_start_res) {
	wm_hints.initial_state = IconicState;
    } else {
	wm_hints.initial_state = NormalState;
    }
    wm_hints.flags = InputHint | StateHint;
    if ((icon_mask & XValue) && (icon_mask & YValue)) {
	wm_hints.icon_x = icon_x;
	wm_hints.icon_y = icon_y;
	wm_hints.flags |= IconPositionHint;
    }
#ifndef	X11R3
    XSetWMProperties(dpy, xc->w, &winName, &iconName, NULL, 0,
		     &hints, &wm_hints, &class_hints);
#else	/* X11R3 */
    XSetStandardProperties(dpy, xc->w, winName, iconName, None, NULL, 0,
			   &hints, &wm_hints, &class_hints);
    XSetWMHints(dpy, xc->w, &wm_hints);
    XSetClassHint(dpy, xc->w, &class_hints);
#endif	/* X11R3 */

    XClearWindow(dpy, xc->w);
    XSelectInput(dpy, xc->w, (ButtonPressMask | ButtonReleaseMask |
			      ResizeRedirectMask | StructureNotifyMask));
}

int
create_xim(lc_name, lang)
char *lc_name, *lang;
{
    register XIMClientRec *xc;
    register XIMLangRec *xl;
    register YesOrNo *yes_no;
    register LangUumrcRec *lu;
    register Ichiran *ichi;
    register Inspect *ins;
    register int i, num;
    int x, y;
    unsigned int width, height;
    int mask = 0;
    Atom ret;
    char displaybuf[128];
    short port;
    int lang_len = 0;
    char *p;
/*
    XLocale xlc;
*/
    int size_of_xim;
    int size_of_ximclient;
    int size_of_yes_no;
    int size_of_ichiran;
    int size_of_inspect;
    int all_size;
    XIMNestLangList lc_list = NULL, lc;
    char *def_font;

    extern char *getenv();

#ifdef	X_WCHAR
    dol_wchar_t = 0x20202024;
    cursor_wchar_t = 0x20202020;
#else	/* X_WCHAR */
    mbtowc(&dol_wchar_t, "$", 1);
    mbtowc(&cursor_wchar_t, " ", 1);
#endif	/* X_WCHAR */

    if((dpy = XOpenDisplay(display_name)) == NULL)
    {
	print_out1("I could not open Display : %s.", display_name);
	return(-1);
    }
    screen = DefaultScreen(dpy);
    root_window = RootWindow(dpy, screen);
    XSetErrorHandler(xerror_handler);
    XSetIOErrorHandler(xioerror_handler);

    if (ret = XInternAtom(dpy, XIM_INPUTMETHOD, True)) {
	if (XGetSelectionOwner(dpy, ret)) {
	    print_out("I am already running.");
	    return(-1);
	}
    }

    if (DisplayString(dpy)) {
	strcpy(displaybuf, DisplayString(dpy));
    } else if (getenv("DISPLAY")) {
	strcpy(displaybuf, getenv("DISPLAY"));
    } else {
	print_out("I could not open socket.");
	return(-1);
    }
    if ((port = init_net(dpy->fd, displaybuf)) == (short)-1) {
	print_out("I could not open socket.");
	return(-1);
    }

    size_of_xim = sizeof(XInputManager);
    size_of_ximclient = sizeof(XIMClientRec);
    size_of_yes_no = sizeof(YesOrNo);
    size_of_ichiran = sizeof(Ichiran);
    size_of_inspect = sizeof(Inspect);
    all_size = size_of_xim + size_of_ximclient
	     + size_of_yes_no + size_of_ichiran + size_of_inspect;
    if ((p = (char *)Malloc(all_size)) == NULL) {
	malloc_error("allocation of the initial area");
	return(-1);
    }
    xim = (XInputManager *)p;
    p += size_of_xim;
    xim->ximclient = xc = (XIMClientRec *)p;
    p += size_of_ximclient;
    yes_no= (YesOrNo *)p;
    p += size_of_yes_no;
    ichi = (Ichiran *)p;
    p += size_of_ichiran;
    ins = (Inspect *)p;
    p += size_of_inspect;

    xim->ichi = ichi;
    xim->inspect = ins;
    xim->j_c = NULL;
    xim->sel_ret = -1;
    xim->client_count = 1;
    xim->supported_style[0] = SUPPORT_STYLE_1;
    xim->supported_style[1] = SUPPORT_STYLE_2;
    xim->supported_style[2] = SUPPORT_STYLE_3;
#ifdef	CALLBACKS
    xim->supported_style[3] = SUPPORT_STYLE_4;
    xim->supported_style[4] = SUPPORT_STYLE_5;
    xim->supported_style[5] = SUPPORT_STYLE_6;
    xim->supported_style[6] = SUPPORT_STYLE_7;
#endif	/* CALLBACKS */

/*
    xlc = _XFallBackConvert();
    xl->xlc = _XlcDupLocale(xlc);
*/
    for (lu = languumrc_list; lu != NULL; lu = lu->next) {
/*
	if (!strcmp(lu->lang_name, lang)) {
	    xl->lang_uumrc = lu;
	}
*/
	lang_len += (strlen(lu->lang_name) + 1);
    }
    if (lang_len) {
	xim->supported_language = Malloc(lang_len);
	*xim->supported_language = '\0';
	for (lu = languumrc_list; lu != NULL; lu = lu->next, i++) {
	    strcat(xim->supported_language, lu->lang_name);
	    strcat(xim->supported_language, ",");
	}
	*(xim->supported_language + (lang_len - 1)) = '\0';
    } else {
	xim->supported_language = NULL;
    }
	
    cursor1 = XCreateFontCursor(dpy, 52);
    cursor2 = XCreateFontCursor(dpy, 60);
    cursor3 = XCreateFontCursor(dpy, 24);

    xc->cur_xl = NULL;
    xc->next = NULL;
    xc->using_language = alloc_and_copy(lang);
    xc->user_name = alloc_and_copy(root_username);
    xc->yes_no = yes_no;
    xc->fd = -1;
    xc->input_style = (XIMPreeditArea | XIMStatusArea);
    xc->ch_lc_flg = 0;
    xc->w = (Window)0;
    default_xc_set(xc);

    xc->pe.fg = xc->st.fg = resource.fore_color_res;
    xc->pe.bg = xc->st.bg = resource.back_color_res;

    /*      Create GC   */
    xgcv.foreground = xc->pe.fg;
    xgcv.background = xc->pe.bg;
    xc->pe.gc = xc->st.gc
      = XCreateGC(dpy, root_window, (GCForeground | GCBackground ),&xgcv);
    /*      Create reverse GC   */
    xgcv.foreground = xc->pe.bg;
    xgcv.background = xc->pe.fg;
    xc->pe.reversegc = xc->st.reversegc
      = XCreateGC(dpy, root_window, (GCForeground | GCBackground ),&xgcv);
    /*	Create invert GC */
    xgcv.foreground = xc->pe.fg;
    xgcv.function = GXinvert;
    xgcv.plane_mask = XOR(xc->pe.fg, xc->pe.bg);
    xc->pe.invertgc = XCreateGC(dpy, root_window,
				(GCForeground | GCFunction | GCPlaneMask),
				&xgcv);

    if (resource.border_width_res == 0) resource.border_width_res = 4;

    if (resource.geometry_res) {
	mask = XParseGeometry(resource.geometry_res, &x, &y, &width, &height);
    }
    if (mask & WidthValue) {
	ClientWidth(xc) = width;
    } else {
	ClientWidth(xc) = DisplayWidth(dpy, screen)
			      - (resource.border_width_res*2);
    }

    lc_list = get_langlist(lc_name);
    if (lc_list == NULL || have_world(lc_list) == True) {
	if (lc_list)
	    lc_list->next = get_langlist(LANG_WR);
	else
	    lc_list = get_langlist(LANG_WR);
    }
    for (num = 0, lc = lc_list; lc != NULL; lc = lc->next) num++;
    if ((xc->xl = (XIMLangRec**)Malloc(sizeof(XIMLangRec*) * num)) == NULL) {
	return(-1);
    }
    for (i = 0, lc = lc_list; lc != NULL; lc = lc->next) {
	if (!strcmp(lc->lc_name, "wr_WR.ct")) {
	    xc->world_on = 1;
	    xc->have_world = 1;
	    continue;
	}
	if((xl = xc->xl[i]
	       = (XIMLangRec *)Calloc(1, sizeof(XIMLangRec))) == NULL) {
	    return(-1);
	}
	xc->cur_xl = xl;
	xl->lang = alloc_and_copy(lc->lang);
	xl->lc_list = NULL;
	add_locale_to_xl(xl, lc);
	xl->w_c = NULL;
#ifdef	USING_XJUTIL
	xl->xjutil_fs_id = -1;
#endif	/* USING_XJUTIL */
	/* Create Default Windows 	*/
	if (resource.font_set_res) {
	    load_font_set(xl, resource.font_set_res, resource.font_set_res);
	}
	if (xl->pe_fs == NULL) {
	    if ((def_font = get_default_font_name(lc->lang)) == NULL) {
		return(-1);
	    }
	    if (load_font_set(xl, def_font, def_font) == -1) {
		return(-1);
	    }
	}
	if (i == 0) {
	    ClientHeight(xc) = FontHeight(xl) + FontHeight(xl)/2;

	    xc->columns = (int)ClientWidth(xc)/(int)FontWidth(xl);
	    xc->max_columns = xc->columns - MHL0;

	    PreeditWidth(xc) = xc->max_columns * FontWidth(xl);
	    PreeditHeight(xc) = FontHeight(xl);
	    PreeditX(xc) = FontWidth(xl) * (MHL0 + 1);
	    PreeditY(xc) = (int)(ClientHeight(xc) - FontHeight(xl))/ 2;

	    StatusWidth(xc) = FontWidth(xl) * MHL0;
	    StatusHeight(xc) = FontHeight(xl);
	    StatusX(xc) = 0;
	    StatusY(xc) = PreeditY(xc);
	    if (mask & XValue) {
		ClientX(xc) = x;
	    } else {
		ClientX(xc) = 0;
	    }
	    if (mask & YValue) {
		ClientY(xc) = y;
	    } else {
		ClientY(xc) = DisplayHeight(dpy, screen) - ClientHeight(xc)
				  - (resource.border_width_res * 2);
	    }
	    create_xim_window(xc);
	    xc->yes_no = (YesOrNo *)Malloc(sizeof(YesOrNo));
	    create_yes_no(xc);
	}
	cur_p = cur_x = xc;
	if (new_client(xc, xl, True) == -1) {
	    return(-1);
	}
	XMapWindow(dpy, xc->w); 
	XMapWindow(dpy, xl->ws); 
	XFlush(dpy);
	i++;
    }
    xc->lang_num = i;
    xc->cur_xl = xc->xl[0];
    cur_p = cur_x = xim->ximclient;
    cur_lang = xim->ximclient->cur_xl->lang;
    c_c = cur_p->cur_xl->w_c;
    cur_rk = c_c->rk;
    cur_rk_table = cur_rk->rk_table;

    ximclient_list = xc;
    ximclient_list->next = NULL;

    create_ichi();
    create_inspect();
    if (henkan_off_flag == 0) {
	disp_mode();
    } else {
	display_henkan_off_mode();
    }

    write_ximid(port);
    read_wm_id();
#ifdef	XJPLIB
    XJp_init();
#endif	/* XJPLIB */
    return(0);
}

void
xw_end()
{
    extern int shutdown(), close();

    close_net();
    shutdown(dpy->fd, 2);
    close(dpy->fd);
}

int
create_preedit(xc, xl)
XIMClientRec *xc;
XIMLangRec *xl;
{
    register char *p;
    int total_size;
    int	width, height, border_width;
    Window parent;
    unsigned long mask = 0L;

    if (IsPreeditNothing(xc)) return(0);

    if ((p = Malloc((total_size = (sizeof(wchar) + sizeof(unsigned char))
		    * (maxchg * 2 + 1)))) == NULL) {
	malloc_error("allocation of client's area");
	return(-1);
    }
    bzero(p, total_size);
    xl->buf = (wchar *)p;
    p += sizeof(wchar) * (maxchg * 2 + 1);
    xl->att = (unsigned char *)p;
    clear_xl(xl);

    border_width = 0;
    xl->del_x = 0;
    xl->vst = 0;
    xl->note[0] = 0;
    xl->note[1] = 0;
    xc->maxcolumns = maxchg * 2;

    if (IsPreeditArea(xc)) {
	xc->pe.area_needed.width = FontWidth(xl) * 2;
	xc->pe.area_needed.height = FontHeight(xl);
	if (PreeditWidth(xc) <= 0) PreeditWidth(xc) = 1;
	if (PreeditHeight(xc) <= 0) PreeditHeight(xc) = 1;
	height = FontHeight(xl);
		
	mask = CWBorderPixel;
	attributes.border_pixel = xc->pe.fg;
	if (xc->pe.bg_pixmap) {
	    mask |= CWBackPixmap;
	    attributes.background_pixmap = xc->pe.bg_pixmap;
	} else {
	    mask |= CWBackPixel;
	    attributes.background_pixel = xc->pe.bg;
	}
	if (xc->pe.colormap) {
	    mask |= CWColormap;
	    attributes.colormap = xc->pe.colormap;
	}
	xl->wp[0] = XCreateWindow(dpy, xc->w, PreeditX(xc), PreeditY(xc),
				  PreeditWidth(xc), PreeditHeight(xc),
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);

	xl->wp[1] = xl->wp[2] = 0;

	xl->wn[2] = XCreateWindow(dpy, xl->wp[0],
				  PreeditWidth(xc) - FontWidth(xl), 0,
				  FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);

	mask |= CWEventMask;
	attributes.event_mask = ExposureMask;
	xl->wn[0] = XCreateWindow(dpy, xl->wp[0], 0, 0,
				  FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->wn[1] = XCreateWindow(dpy, xl->wp[0],
				  PreeditWidth(xc) - FontWidth(xl), 0,
				  FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->w[0] = XCreateWindow(dpy, xl->wp[0], 0, 0,
				 FontWidth(xl) * (xc->maxcolumns + 1), height,
				 border_width, CopyFromParent, InputOutput,
				 CopyFromParent, mask, &attributes);
	xl->w[1] = xl->w[2] = 0;

	if (!xl->wp[0] || !xl->w[0] || !xl->wn[0] || !xl->wn[1] || !xl->wn[2]){
		return(-1);
	}

	return(0);
    }else if (IsPreeditPosition(xc)){
	
	width = FontWidth(xl) * (xc->maxcolumns + 1);
	height = FontHeight(xl);
	if (xc->focus_window) {
	    parent = xc->focus_window;
	} else {
	    parent = xc->w;
	}
	if (PreeditWidth(xc) <= 0) PreeditWidth(xc) = 1;

	mask = CWBorderPixel;
	attributes.border_pixel = xc->pe.fg;
	if (xc->pe.bg_pixmap) {
	    mask |= CWBackPixmap;
	    attributes.background_pixmap = xc->pe.bg_pixmap;
	} else {
	    mask |= CWBackPixel;
	    attributes.background_pixel = xc->pe.bg;
	}
	if (xc->pe.colormap) {
	    mask |= CWColormap;
	    attributes.colormap = xc->pe.colormap;
	}

	xl->wp[0] = XCreateWindow(dpy, parent, 0, 0, PreeditWidth(xc), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->wp[1] = XCreateWindow(dpy, parent, 0, 0, PreeditWidth(xc), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->wp[2] = XCreateWindow(dpy, parent, 0, 0, PreeditWidth(xc), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->wn[2] = XCreateWindow(dpy, parent, 0, 0, FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	mask |= CWEventMask;
	attributes.event_mask = ExposureMask;
	xl->wn[0] = XCreateWindow(dpy, parent, 0, 0, FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	xl->wn[1] = XCreateWindow(dpy, parent, 0, 0, FontWidth(xl), height,
				  border_width, CopyFromParent, InputOutput,
				  CopyFromParent, mask, &attributes);
	mask |= CWBitGravity;
	attributes.bit_gravity = WestGravity;
	xl->w[0] = XCreateWindow(dpy, xl->wp[0], 0, 0, width, height,
				 border_width, CopyFromParent, InputOutput,
				 CopyFromParent, mask, &attributes);
	xl->w[1] = XCreateWindow(dpy, xl->wp[1], 0, 0, width, height,
				 border_width, CopyFromParent, InputOutput,
				 CopyFromParent, mask, &attributes);
	xl->w[2] = XCreateWindow(dpy, xl->wp[2], 0, 0, width, height,
				 border_width, CopyFromParent, InputOutput,
				 CopyFromParent, mask, &attributes);

	if (!xl->wp[0] || !xl->wp[1] || !xl->wp[2] || !xl->w[0] || !xl->w[1] ||
	    !xl->w[2] || !xl->wn[0] || !xl->wn[1] || !xl->wn[2]) {
	    return(-1);
	}

	xc->c0 = xc->c1 = xc->c2 = xc->maxcolumns;
	XMapWindow(dpy, xl->w[0]);
	XMapWindow(dpy, xl->w[1]);
	XMapWindow(dpy, xl->w[2]);
	XFlush(dpy);
	return(0);
#ifdef	CALLBACKS
    } else if (IsStatusCallbacks(xc)) {
        return(0);
#endif	/* CALLBACKS */
    } else {
	return(-1);
    }
}

int
create_status(xc, xl)
XIMClientRec *xc;
XIMLangRec *xl;
{
    register char *p;
    int total_size;
    unsigned long mask = 0L;

    if (IsStatusNothing(xc)) return(0);

    if ((p = Malloc((total_size = (sizeof(wchar) + sizeof(unsigned char))
		    * (MHL0 * 2 + 1)))) == NULL) {
	malloc_error("allocation of client's area");
	return(-1);
    }
    bzero(p, total_size);
    xl->buf0 = (wchar *)p;
    p += sizeof(wchar) * (MHL0 * 2 + 1);
    xl->att0 = (unsigned char *)p;

    if (IsStatusArea(xc)){
	xc->st.area_needed.width = StatusFontWidth(xl) * MHL0;
	xc->st.area_needed.height = StatusFontHeight(xl);
	if (StatusWidth(xc) <= 0) StatusWidth(xc) = 1;
	if (StatusHeight(xc) <= 0) StatusHeight(xc) = 1;

	mask = (CWBorderPixel | CWEventMask | CWCursor);
	attributes.border_pixel = xc->pe.fg;
	attributes.event_mask = (ButtonPressMask | ButtonReleaseMask |
				 EnterWindowMask | LeaveWindowMask |
				 ExposureMask);
	attributes.cursor = cursor2;
	if (xc->st.bg_pixmap) {
	    mask |= CWBackPixmap;
	    attributes.background_pixmap = xc->st.bg_pixmap;
	} else {
	    mask |= CWBackPixel;
	    attributes.background_pixel = xc->st.bg;
	}
	if (xc->st.colormap) {
	    mask |= CWColormap;
	    attributes.colormap = xc->st.colormap;
	}
	if ((xl->ws = XCreateWindow(dpy, xc->w, StatusX(xc), StatusY(xc),
				    StatusWidth(xc), StatusHeight(xc),
				    0, CopyFromParent, InputOutput,
				    CopyFromParent, mask, &attributes)) == 0) {
	    return(-1);
	}
	return(0);
#ifdef	CALLBACKS
    } else if (IsStatusCallbacks(xc)) {
        return(0);
#endif	/* CALLBACKS */
    } else {
	return(-1);
    }
}
