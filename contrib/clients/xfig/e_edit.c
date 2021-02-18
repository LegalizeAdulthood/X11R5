/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
 * Change function implemented by Frank Schmuck (schmuck@svax.cs.cornell.edu)
 * X version by Jon Tombs <jon@uk.ac.oxford.robots>
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
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_fonts.h"
#include "u_search.h"
#include "u_list.h"
#include "u_create.h"
#include "w_canvas.h"
#include "w_drawprim.h"
#include "w_icons.h"
#include "w_util.h"
#include "w_mousefun.h"

char           *panel_get_value();
Widget          make_popup_menu();

extern Pixmap   psfont_menu_bitmaps[];
extern Pixmap   latexfont_menu_bitmaps[];
extern void     Quit();
extern          fontpane_popup();

static void     new_generic_values();
static          generic_window();
static          font_image_panel();
static          float_panel();
static          float_label();
static          int_panel();
static          int_label();
static          str_panel();
static          xy_panel();
static          f_pos_panel();
static          get_f_pos();
static          points_panel();
static          get_points();
int             panel_set_value();
static XtCallbackProc done_button(), apply_button(), cancel_button();
static void     line_style_select();
static void     textjust_select();
static void     area_fill_select();
static void     flip_eps_select();
static void     hidden_text_select();
static void     rigid_text_select();
static void     special_text_select();
static Widget   popup, form;
static Widget   below, beside;

#define	NUM_IMAGES	15

static int      done_line();
static int      done_text();
static int      done_arc();
static int      done_ellipse();
static int      done_spline();

static Widget   shrink, expand;
static Widget   label;
static Widget   thickness_panel;
static Widget   color_panel;
static Widget   depth_panel;
static Widget   angle_panel;
static Widget   textjust_panel;
static Widget   hidden_text_panel;
static Widget   rigid_text_panel;
static Widget   special_text_panel;
static Widget   area_fill_panel;
static Widget   flip_eps_panel;
static Widget   style_val_label;
static Widget   area_fill_label;
static Widget   style_panel;
static Widget   style_val_panel;

#ifdef notdef
/* not used at present */
static Widget   for_arrow_panel;
static Widget   back_arrow_panel;

#endif
static Widget   text_panel;
static Widget   eps_name_panel;
static Widget   x1_panel, y1_panel;
static Widget   x2_panel, y2_panel;
static Widget   x3_panel, y3_panel;
static Widget   hw_ratio_panel;
static Widget   orig_hw_panel;
static Widget   rotn_panel;
static Widget   font_panel;
static Widget   cur_fontsize_panel;
static Widget   fill_flag_panel;
static Widget   radius;
static Widget   menu, hidden_text_menu, textjust_menu;
static Widget   special_text_menu, rigid_text_menu;
static Widget   but1;

DeclareStaticArgs(12);
static	char	buf[64];

#define	MAX_POINTS	100
static Widget   px_panel[MAX_POINTS];
static Widget   py_panel[MAX_POINTS];

static int      ellipse_flag;
static int      fill_flag;
static int      flip_eps_flag;
static int      (*done_proc) ();
static int      button_result;
static int      textjust;
static int      hidden_text_flag;
static int      special_text_flag;
static int      rigid_text_flag;
static int      new_ps_font, new_latex_font;
static int      new_psflag;
static int      changed;

#define	CANCEL		0
#define	DONE		1
#define	APPLY		2

static struct {
    int             thickness;
    int             color;
    int             depth;
    int             style;
    float           style_val;
    int             pen;
    int             area_fill;
    F_arrow        *for_arrow;
    F_arrow        *back_arrow;
}               generic_vals;

#define put_generic_vals(x) \
	generic_vals.thickness	= x->thickness; \
	generic_vals.color	= x->color; \
	generic_vals.depth	= x->depth; \
	generic_vals.style	= x->style; \
	generic_vals.style_val	= x->style_val; \
	generic_vals.pen	= x->pen; \
	generic_vals.area_fill	= x->area_fill

#define get_generic_vals(x) \
	new_generic_values(); \
	x->thickness	= generic_vals.thickness; \
	x->color	= generic_vals.color; \
	x->depth	= generic_vals.depth; \
	x->style	= generic_vals.style; \
	x->style_val	= generic_vals.style_val; \
	x->pen		= generic_vals.pen; \
	x->area_fill	= generic_vals.area_fill

#define put_generic_arrows(x) \
	generic_vals.for_arrow  = x->for_arrow; \
	generic_vals.back_arrow = x->back_arrow

#define get_generic_arrows(x) \
	x->for_arrow  = generic_vals.for_arrow; \
	x->back_arrow = generic_vals.back_arrow

int             edit_item();

edit_item_selected()
{
    set_mousefun("edit object", "", "");
    canvas_kbd_proc = null_proc;
    canvas_locmove_proc = null_proc;
    init_searchproc_left(edit_item);
    canvas_leftbut_proc = object_search_left;
    canvas_middlebut_proc = null_proc;
    canvas_rightbut_proc = null_proc;
    set_cursor(&pick9_cursor);
    reset_action_on();
}

edit_item(p, type, x, y)
    char           *p;
    int             type;
    int             x, y;
{
    changed = 0;
    switch (type) {
    case O_POLYLINE:
	make_window_line((F_line *) p);
	break;
    case O_TEXT:
	make_window_text((F_text *) p);
	break;
    case O_ELLIPSE:
	make_window_ellipse((F_ellipse *) p);
	break;
    case O_ARC:
	make_window_arc((F_arc *) p);
	break;
    case O_SPLINE:
	make_window_spline((F_spline *) p);
	break;
    case O_COMPOUND:
	put_msg("There is no support for EDITING a compound object");
	return;
	break;
    default:
	return;
    }

    XtPopup(popup, XtGrabExclusive);
}

static void
expand_eps(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    struct f_point  p1, p2;
    int             dx, dy, rotation;
    float           ratio;
    register float  orig_ratio = new_l->eps->hw_ratio;

    p1.x = atoi(panel_get_value(x1_panel));
    p1.y = atoi(panel_get_value(y1_panel));
    p2.x = atoi(panel_get_value(x2_panel));
    p2.y = atoi(panel_get_value(y2_panel));
    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    rotation = 0;
    if (dx < 0 && dy < 0)
	rotation = 180;
    else if (dx < 0 && dy >= 0)
	rotation = 270;
    else if (dy < 0 && dx >= 0)
	rotation = 90;
    if (dx == 0 || dy == 0 || orig_ratio == 0.0)
	return;
    if (((rotation == 0 || rotation == 180) && !flip_eps_flag) ||
	(rotation != 0 && rotation != 180 && flip_eps_flag)) {
	ratio = fabs((float) dy / (float) dx);
	if (ratio < orig_ratio)
	    p2.y = p1.y + signof(dy) * (int) (fabs((float)dx) * orig_ratio);
	else
	    p2.x = p1.x + signof(dx) * (int) (fabs((float)dy) / orig_ratio);
    } else {
	ratio = fabs((float) dx / (float) dy);
	if (ratio < orig_ratio)
	    p2.x = p1.x + signof(dx) * (int) (fabs((float)dy) * orig_ratio);
	else
	    p2.y = p1.y + signof(dy) * (int) (fabs((float)dx) / orig_ratio);
    }
    sprintf(buf, "%d", p2.x);
    panel_set_value(x2_panel, buf);
    sprintf(buf, "%d", p2.y);
    panel_set_value(y2_panel, buf);
    sprintf(buf, "%1.1f", orig_ratio);
    FirstArg(XtNlabel, buf);
    SetValues(hw_ratio_panel);
}

static void
shrink_eps(w, ev)
    Widget          w;
    XButtonEvent   *ev;
{
    struct f_point  p1, p2;
    int             dx, dy, rotation;
    float           ratio;
    register float  orig_ratio = new_l->eps->hw_ratio;

    p1.x = atoi(panel_get_value(x1_panel));
    p1.y = atoi(panel_get_value(y1_panel));
    p2.x = atoi(panel_get_value(x2_panel));
    p2.y = atoi(panel_get_value(y2_panel));
    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    rotation = 0;
    if (dx < 0 && dy < 0)
	rotation = 180;
    else if (dx < 0 && dy >= 0)
	rotation = 270;
    else if (dy < 0 && dx >= 0)
	rotation = 90;
    if (dx == 0 || dy == 0 || orig_ratio == 0.0)
	return;
    if (((rotation == 0 || rotation == 180) && !flip_eps_flag) ||
	(rotation != 0 && rotation != 180 && flip_eps_flag)) {
	ratio = fabs((float) dy / (float) dx);
	if (ratio > orig_ratio)
	    p2.y = p1.y + signof(dy) * (int) (fabs((float)dx) * orig_ratio);
	else
	    p2.x = p1.x + signof(dx) * (int) (fabs((float)dy) / orig_ratio);
    } else {
	ratio = fabs((float) dx / (float) dy);
	if (ratio > orig_ratio)
	    p2.x = p1.x + signof(dx) * (int) (fabs((float)dy) * orig_ratio);
	else
	    p2.y = p1.y + signof(dy) * (int) (fabs((float)dx) / orig_ratio);
    }
    sprintf(buf, "%d", p2.x);
    panel_set_value(x2_panel, buf);
    sprintf(buf, "%d", p2.y);
    panel_set_value(y2_panel, buf);
    sprintf(buf, "%1.1f", orig_ratio);
    FirstArg(XtNlabel, buf);
    SetValues(hw_ratio_panel);
}

static char    *flip_eps_items[] = {"Normal            ",
				    "Flipped about diag"};

make_window_line(l)
    F_line         *l;
{
    struct f_point  p1, p2;
    int             dx, dy, rotation;
    float           ratio;

    set_temp_cursor(&panel_cursor);
    mask_toggle_linemarker(l);
    new_l = copy_line(l);
    new_l->next = l;
    put_generic_vals(new_l);
    switch (new_l->type) {
    case T_POLYLINE:
	put_generic_arrows(new_l);
	generic_window("POLYLINE", "Polyline", &line_ic, done_line, 1, 1);
	points_panel(new_l->points, 0);
	break;
    case T_POLYGON:
	put_generic_arrows(new_l);
	generic_window("POLYLINE", "Polygon", &polygon_ic, done_line, 1, 1);
	points_panel(new_l->points, 1);
	break;
    case T_BOX:
	generic_window("POLYLINE", "Box", &box_ic, done_line, 1, 0);
	p1 = *new_l->points;
	p2 = *new_l->points->next->next;
	xy_panel(p1.x, p1.y, "Top Left:", &x1_panel, &y1_panel);
	xy_panel(p2.x, p2.y, "Bottom Right:", &x2_panel, &y2_panel);
	break;
    case T_ARC_BOX:
	generic_window("POLYLINE", "ArcBox", &arc_box_ic, done_line, 1, 0);
	p1 = *new_l->points;
	p2 = *new_l->points->next->next;
	xy_panel(p1.x, p1.y, "Top Left:", &x1_panel, &y1_panel);
	xy_panel(p2.x, p2.y, "Bottom Right:", &x2_panel, &y2_panel);
	int_panel(new_l->radius, "Radius =", &radius);
	break;
    case T_EPS_BOX:
	generic_window("POLYLINE", "EPS", &epsbitmap_ic, done_line, 0, 0);
	int_panel(new_l->color, "Color =", &color_panel);
	int_panel(new_l->depth, "Depth =", &depth_panel);
	if (!strcmp(new_l->eps->file, EMPTY_EPS))
	    new_l->eps->file[0] = '\0';
	str_panel(new_l->eps->file, "EPS bitmap filename =", &eps_name_panel);
	p1 = *new_l->points;
	p2 = *new_l->points->next->next;
	xy_panel(p1.x, p1.y, "Eps origin:", &x1_panel, &y1_panel);
	xy_panel(p2.x, p2.y, "Eps opposite corner:", &x2_panel, &y2_panel);

	/* make popup flipped menu */
	FirstArg(XtNfromVert, below);
	NextArg(XtNborderWidth, 0);
	beside = XtCreateManagedWidget("Orientation =", labelWidgetClass,
				       form, Args, ArgCount);
	FirstArg(XtNfromVert, below);
	NextArg(XtNfromHoriz, beside);
	flip_eps_flag = new_l->eps->flipped;
	flip_eps_panel = XtCreateManagedWidget(
		  flip_eps_items[flip_eps_flag ? 1 : 0], menuButtonWidgetClass,
						form, Args, ArgCount);
	below = flip_eps_panel;
	menu = make_popup_menu(flip_eps_items, XtNumber(flip_eps_items),
			       flip_eps_panel, flip_eps_select);

	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	rotation = 0;
	if (dx < 0 && dy < 0)
            rotation = 180;
	else if (dx < 0 && dy >= 0)
            rotation = 270;
	else if (dy < 0 && dx >= 0)
            rotation = 90;
	if (dx == 0 || dy == 0)
	    ratio = 0.0;
	else if (((rotation == 0 || rotation == 180) && !flip_eps_flag) ||
		 (rotation != 0 && rotation != 180 && flip_eps_flag))
	    ratio = fabs((float) dy / (float) dx);
	else
	    ratio = fabs((float) dx / (float) dy);

	int_label(rotation, "Rotation =      ", &rotn_panel);
	float_label(ratio, "Curr h/w Ratio =", &hw_ratio_panel);
	float_label(new_l->eps->hw_ratio, "Orig h/w Ratio =", &orig_hw_panel);
	below = orig_hw_panel;
	FirstArg(XtNfromVert, below);
	NextArg(XtNborderWidth, 0);
	beside = XtCreateManagedWidget("Change h/w ratio:", labelWidgetClass,
				       form, Args, ArgCount);
	FirstArg(XtNfromVert, below);
	NextArg(XtNsensitive, new_l->eps->hw_ratio ? True : False);
	NextArg(XtNfromHoriz, beside);
	shrink = XtCreateManagedWidget("Shrink to orig", commandWidgetClass,
				       form, Args, ArgCount);
	XtAddEventHandler(shrink, ButtonReleaseMask, (Boolean) 0,
                          shrink_eps, (XtPointer) NULL);
	beside = shrink;

	ArgCount--;
	NextArg(XtNfromHoriz, beside);
	expand = XtCreateManagedWidget("Expand to orig", commandWidgetClass,
				       form, Args, ArgCount);
	XtAddEventHandler(expand, ButtonReleaseMask, (Boolean) 0,
                          expand_eps, (XtPointer) NULL);
	break;
    }
}

static
get_new_line_values()
{
    struct f_point  p1, p2, *p;
    char           *s;
    int             dx, dy, rotation;
    float           ratio;

    switch (new_l->type) {
    case T_POLYLINE:
    case T_POLYGON:
	get_generic_vals(new_l);
	get_generic_arrows(new_l);
	get_points(new_l->points, (new_l->type == T_POLYGON));
	return;
    case T_ARC_BOX:
	new_l->radius = atoi(panel_get_value(radius));
    case T_BOX:
	get_generic_vals(new_l);
	p1.x = atoi(panel_get_value(x1_panel));
	p1.y = atoi(panel_get_value(y1_panel));
	p2.x = atoi(panel_get_value(x2_panel));
	p2.y = atoi(panel_get_value(y2_panel));
	break;
    case T_EPS_BOX:
	new_l->color = atoi(panel_get_value(color_panel));
	new_l->depth = atoi(panel_get_value(depth_panel));
	p1.x = atoi(panel_get_value(x1_panel));
	p1.y = atoi(panel_get_value(y1_panel));
	p2.x = atoi(panel_get_value(x2_panel));
	p2.y = atoi(panel_get_value(y2_panel));

	dx = p2.x - p1.x;
	dy = p2.y - p1.y;
	rotation = 0;
	if (dx < 0 && dy < 0)
            rotation = 180;
	else if (dx < 0 && dy >= 0)
            rotation = 270;
	else if (dy < 0 && dx >= 0)
            rotation = 90;
	if (dx == 0 || dy == 0)
	    ratio = 0.0;
	else if (((rotation == 0 || rotation == 180) && !flip_eps_flag) ||
		 (rotation != 0 && rotation != 180 && flip_eps_flag))
	    ratio = fabs((float) dy / (float) dx);
	else
	    ratio = fabs((float) dx / (float) dy);

	sprintf(buf, "%d", rotation);
	FirstArg(XtNlabel, buf);
	SetValues(rotn_panel);
	new_l->eps->flipped = flip_eps_flag;
	sprintf(buf, "%1.1f", ratio);
	FirstArg(XtNlabel, buf);
	SetValues(hw_ratio_panel);
	s = panel_get_value(eps_name_panel);
	if (s[0] == '\0')
	    s = EMPTY_EPS;
	if (strcmp(s, new_l->eps->file)) {
	    if (new_l->eps->bitmap) {
		free((char *) new_l->eps->bitmap);
		new_l->eps->bitmap = NULL;
	    }
	    strcpy(new_l->eps->file, s);
	    new_l->eps->hw_ratio = 0.0;
	    read_epsf(new_l->eps);
	}
	sprintf(buf, "%1.1f", new_l->eps->hw_ratio);
	FirstArg(XtNlabel, buf);
	SetValues(orig_hw_panel);
	FirstArg(XtNsensitive, new_l->eps->hw_ratio ? True : False);
	SetValues(shrink);
	SetValues(expand);
	break;
    }
    p = new_l->points;
    p->x = p1.x;
    p->y = p1.y;
    p = p->next;
    p->x = p2.x;
    p->y = p1.y;
    p = p->next;
    p->x = p2.x;
    p->y = p2.y;
    p = p->next;
    p->x = p1.x;
    p->y = p2.y;
    p = p->next;
    p->x = p1.x;
    p->y = p1.y;
}

static
done_line()
{
#ifdef FASTSERVER
    int             xmin, ymin, xmax, ymax;
#endif

    old_l = new_l->next;
    switch (button_result) {
    case APPLY:
	changed = 1;
	draw_line(new_l, ERASE);
	get_new_line_values();
	draw_line(new_l, PAINT);
	break;
    case DONE:
#ifndef FASTSERVER
	draw_line(new_l, ERASE);
#endif
	get_new_line_values();
	new_l->next = NULL;
	change_line(old_l, new_l);
#ifdef FASTSERVER
	line_bound(new_l, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	mask_toggle_linemarker(new_l);
#else
	draw_line(new_l, PAINT);
#endif
	mask_toggle_linemarker(new_l);
	reset_cursor();
	break;
    case CANCEL:
	if (changed) {
	    draw_line(new_l, ERASE);
#ifdef FASTSERVER
	    line_bound(old_l, &xmin, &ymin, &xmax, &ymax);
	    redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	    mask_toggle_linemarker(old_l);
#else
	    draw_line(old_l, PAINT);
#endif
	}
	new_l->next = NULL;
	free_line(&new_l);
	mask_toggle_linemarker(old_l);
	reset_cursor();
	break;
    }

}

make_window_text(t)
    F_text         *t;
{
    static char    *textjust_items[] = {
    "Left Justified ", "Centered       ", "Right Justified"};
    static char    *hidden_text_items[] = {
    "Normal", "Hidden"};
    static char    *rigid_text_items[] = {
    "Normal", "Rigid "};
    static char    *special_text_items[] = {
    "Normal ", "Special"};

    set_temp_cursor(&panel_cursor);
    toggle_textmarker(t);
    new_t = copy_text(t);
    new_t->next = t;

    textjust = new_t->type;	/* get current justification */
    hidden_text_flag = hidden_text(new_t) ? 1 : 0;
    new_psflag = psfont_text(new_t) ? 1 : 0;
    rigid_text_flag = rigid_text(new_t) ? 1 : 0;
    special_text_flag = special_text(new_t) ? 1 : 0;
    new_ps_font = cur_ps_font;
    new_latex_font = cur_latex_font;
    if (new_psflag)
	new_ps_font = new_t->font;	/* get current font */
    else
	new_latex_font = new_t->font;	/* get current font */
    generic_window("TEXT", "", &text_ic, done_text, 0, 0);

    int_panel(new_t->size, "Size =", &cur_fontsize_panel);
    int_panel(new_t->color, "Color =", &color_panel);
    int_panel(new_t->depth, "Depth =", &depth_panel);
    int_panel(round(180 / M_PI * new_t->angle), "Angle =", &angle_panel);

    /* make text justification menu */

    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget("Justification =", labelWidgetClass,
				   form, Args, ArgCount);

    FirstArg(XtNfromVert, below);
    NextArg(XtNfromHoriz, beside);
    textjust_panel = XtCreateManagedWidget(
			    textjust_items[textjust], menuButtonWidgetClass,
					   form, Args, ArgCount);
    below = textjust_panel;
    textjust_menu = make_popup_menu(textjust_items, XtNumber(textjust_items),
				    textjust_panel, textjust_select);

    /* make hidden text menu */

    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget("Hidden Flag =", labelWidgetClass,
				   form, Args, ArgCount);

    FirstArg(XtNfromVert, below);
    NextArg(XtNfromHoriz, beside);
    hidden_text_panel = XtCreateManagedWidget(
		 hidden_text_items[hidden_text_flag], menuButtonWidgetClass,
					      form, Args, ArgCount);
    below = hidden_text_panel;
    hidden_text_menu = make_popup_menu(hidden_text_items,
				       XtNumber(hidden_text_items),
				     hidden_text_panel, hidden_text_select);

    /* make rigid text menu */

    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget("Rigid Flag =", labelWidgetClass,
				   form, Args, ArgCount);

    FirstArg(XtNfromVert, below);
    NextArg(XtNfromHoriz, beside);
    rigid_text_panel = XtCreateManagedWidget(
		   rigid_text_items[rigid_text_flag], menuButtonWidgetClass,
					     form, Args, ArgCount);
    below = rigid_text_panel;
    rigid_text_menu = make_popup_menu(rigid_text_items,
				      XtNumber(rigid_text_items),
				      rigid_text_panel, rigid_text_select);

    /* make special text menu */

    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget("Special Flag =", labelWidgetClass,
				   form, Args, ArgCount);

    FirstArg(XtNfromVert, below);
    NextArg(XtNfromHoriz, beside);
    special_text_panel = XtCreateManagedWidget(
				      special_text_items[special_text_flag],
			       menuButtonWidgetClass, form, Args, ArgCount);
    below = special_text_panel;
    special_text_menu = make_popup_menu(special_text_items,
					XtNumber(special_text_items),
				   special_text_panel, special_text_select);

    xy_panel(new_t->base_x, new_t->base_y, "Origin:", &x1_panel, &y1_panel);
    font_image_panel(new_psflag ? psfont_menu_bitmaps[new_t->font] :
		 latexfont_menu_bitmaps[new_t->font], "Font:", &font_panel);
    str_panel(new_t->cstring, "Text =", &text_panel);
}

static
get_new_text_values()
{
    char           *s;
    PR_SIZE         size;

    new_t->type = textjust;
    new_t->flags =
	(rigid_text_flag ? RIGID_TEXT : 0)
	| (special_text_flag ? SPECIAL_TEXT : 0)
	| (hidden_text_flag ? HIDDEN_TEXT : 0)
	| (new_psflag ? PSFONT_TEXT : 0);
    if (psfont_text(new_t))
	new_t->font = new_ps_font;
    else
	new_t->font = new_latex_font;
    new_t->size = atoi(panel_get_value(cur_fontsize_panel));
    if (new_t->size < 1) {
	new_t->size = 1;
	panel_set_value(cur_fontsize_panel, "1");
    }
    new_t->color = atoi(panel_get_value(color_panel));
    new_t->depth = atoi(panel_get_value(depth_panel));
    new_t->angle = M_PI / 180 * atoi(panel_get_value(angle_panel));
    new_t->base_x = atoi(panel_get_value(x1_panel));
    new_t->base_y = atoi(panel_get_value(y1_panel));
    if (new_t->cstring)
	cfree(new_t->cstring);
    s = panel_get_value(text_panel);
    new_t->cstring = new_string(strlen(s) + 1);
    strcpy(new_t->cstring, s);
    size = pf_textwidth(new_t->font, psfont_text(new_t), new_t->size,
			strlen(s), s);
    new_t->height = size.y;
    new_t->length = size.x;
}

static
done_text()
{
    old_t = new_t->next;
    switch (button_result) {
    case APPLY:
	draw_text(new_t, ERASE);
	changed = 1;
	get_new_text_values();
	draw_text(new_t, PAINT);
	break;
    case DONE:
	draw_text(new_t, ERASE);
	get_new_text_values();
	new_t->next = NULL;
	change_text(old_t, new_t);
	draw_text(new_t, PAINT);
	toggle_textmarker(new_t);
	reset_cursor();
	break;
    case CANCEL:
	if (changed) {
	    draw_text(new_t, ERASE);
	    draw_text(old_t, PAINT);
	}
	new_t->next = NULL;
	free_text(&new_t);
	toggle_textmarker(old_t);
	new_t = old_t;
	reset_cursor();
	break;
    }
}

make_window_ellipse(e)
    F_ellipse      *e;
{
    char           *s1, *s2;
    PIXRECT         image;

    set_temp_cursor(&panel_cursor);
    toggle_ellipsemarker(e);
    new_e = copy_ellipse(e);
    new_e->next = e;
    switch (new_e->type) {
    case T_ELLIPSE_BY_RAD:
	s1 = "ELLIPSE";
	s2 = "specified by radius";
	ellipse_flag = 1;
	image = &ellrad_ic;
	break;
    case T_ELLIPSE_BY_DIA:
	s1 = "ELLIPSE";
	s2 = "specified by diameter";
	ellipse_flag = 1;
	image = &elldia_ic;
	break;
    case T_CIRCLE_BY_RAD:
	s1 = "CIRCLE";
	s2 = "specified by radius";
	ellipse_flag = 0;
	image = &cirrad_ic;
	break;
    case T_CIRCLE_BY_DIA:
	s1 = "CIRCLE";
	s2 = "specified by diameter";
	ellipse_flag = 0;
	image = &cirdia_ic;
	break;
    }
    put_generic_vals(new_e);
    generic_window(s1, s2, image, done_ellipse, 1, 0);
    int_panel(round(180 / M_PI * new_e->angle), "Angle =", &angle_panel);

    if (ellipse_flag) {
	f_pos_panel(&new_e->center, "Center:",
		    &x1_panel, &y1_panel);
	f_pos_panel(&new_e->radiuses, "Radiuses:",
		    &x2_panel, &y2_panel);
    } else {
	f_pos_panel(&new_e->center, "Center:",
		    &x1_panel, &y1_panel);
	int_panel(new_e->radiuses.x, "Radius =",
		  &x2_panel);
    }
}

static
get_new_ellipse_values()
{
    F_pos           old_center;
    F_pos           old_radiuses;

#define	adjust_ref(s,f) \
		s.f = new_e->center.f + \
		((s.f - old_center.f)*new_e->radiuses.f)/old_radiuses.f

    get_generic_vals(new_e);
    new_e->angle = M_PI / 180 * atoi(panel_get_value(angle_panel));
    old_center = new_e->center;
    old_radiuses = new_e->radiuses;
    get_f_pos(&new_e->center, x1_panel, y1_panel);
    if (ellipse_flag)
	get_f_pos(&new_e->radiuses, x2_panel, y2_panel);

    else
	new_e->radiuses.x = new_e->radiuses.y =
	    atoi(panel_get_value(x2_panel));

    adjust_ref(new_e->start, x);
    adjust_ref(new_e->start, y);
    adjust_ref(new_e->end, x);
    adjust_ref(new_e->end, y);
}

static
done_ellipse()
{
    old_e = new_e->next;
    switch (button_result) {
    case APPLY:
	draw_ellipse(new_e, ERASE);
	changed = 1;
	get_new_ellipse_values();
	draw_ellipse(new_e, PAINT);
	break;
    case DONE:
	draw_ellipse(new_e, ERASE);
	get_new_ellipse_values();
	new_e->next = NULL;
	change_ellipse(old_e, new_e);
	draw_ellipse(new_e, PAINT);
	toggle_ellipsemarker(new_e);
	reset_cursor();
	break;
    case CANCEL:
	if (changed) {
	    draw_ellipse(new_e, ERASE);
	    draw_ellipse(old_e, PAINT);
	}
	new_e->next = NULL;
	free_ellipse(&new_e);
	toggle_ellipsemarker(old_e);
	reset_cursor();
	break;
    }

}

make_window_arc(a)
    F_arc          *a;
{
    set_temp_cursor(&panel_cursor);
    toggle_arcmarker(a);
    new_a = copy_arc(a);
    new_a->next = a;
    put_generic_vals(new_a);
    put_generic_arrows(new_a);
    generic_window("ARC", "Specified by 3 points", &arc_ic, done_arc, 1, 1);
    f_pos_panel(&new_a->point[0], "p1:", &x1_panel, &y1_panel);
    f_pos_panel(&new_a->point[1], "p2:", &x2_panel, &y2_panel);
    f_pos_panel(&new_a->point[2], "p3:", &x3_panel, &y3_panel);
}

static
get_new_arc_values()
{
    F_pos           p0, p1, p2;
    float           cx, cy;

    get_generic_vals(new_a);
    get_generic_arrows(new_a);
    get_f_pos(&p0, x1_panel, y1_panel);
    get_f_pos(&p1, x2_panel, y2_panel);
    get_f_pos(&p2, x3_panel, y3_panel);
    if (compute_arccenter(p0, p1, p2, &cx, &cy)) {
	new_a->point[0] = p0;
	new_a->point[1] = p1;
	new_a->point[2] = p2;
	new_a->center.x = cx;
	new_a->center.y = cy;
	new_a->direction = compute_direction(p0, p1, p2);
    } else
	put_msg("Invalid ARC points!");
}

static
done_arc()
{
    old_a = new_a->next;
    switch (button_result) {
    case APPLY:
	draw_arc(new_a, ERASE);
	changed = 1;
	get_new_arc_values();
	draw_arc(new_a, PAINT);
	break;
    case DONE:
	draw_arc(new_a, ERASE);
	get_new_arc_values();
	new_a->next = NULL;
	change_arc(old_a, new_a);
	draw_arc(new_a, PAINT);
	toggle_arcmarker(new_a);
	reset_cursor();
	break;
    case CANCEL:
	if (changed) {
	    draw_arc(new_a, ERASE);
	    draw_arc(old_a, PAINT);
	}
	new_a->next = NULL;
	free_arc(&new_a);
	toggle_arcmarker(old_a);
	reset_cursor();
	break;
    }

}

make_window_spline(s)
    F_spline       *s;
{
    set_temp_cursor(&panel_cursor);
    toggle_splinemarker(s);
    new_s = copy_spline(s);
    new_s->next = s;
    put_generic_vals(new_s);
    put_generic_arrows(new_s);
    switch (new_s->type) {
    case T_OPEN_NORMAL:
	generic_window("SPLINE", "Normal open", &spl_ic,
		       done_spline, 1, 1);
	points_panel(new_s->points, 0);
	break;
    case T_CLOSED_NORMAL:
	generic_window("SPLINE", "Normal closed", &c_spl_ic,
		       done_spline, 1, 1);
	points_panel(new_s->points, 1);
	break;
    case T_OPEN_INTERP:
	generic_window("SPLINE", "Interpolated open", &intspl_ic,
		       done_spline, 1, 1);
	points_panel(new_s->points, 0);
	break;
    case T_CLOSED_INTERP:
	generic_window("SPLINE", "Interpolated closed", &c_intspl_ic,
		       done_spline, 1, 1);
	points_panel(new_s->points, 1);
	break;
    }
}

static
done_spline()
{
    old_s = new_s->next;
    switch (button_result) {
    case APPLY:
	draw_spline(new_s, ERASE);
	changed = 1;
	get_generic_vals(new_s);
	get_generic_arrows(new_s);
	get_points(new_s->points, closed_spline(new_s));
	if (int_spline(new_s))
	    remake_control_points(new_s);
	draw_spline(new_s, PAINT);
	break;
    case DONE:
	draw_spline(new_s, ERASE);
	get_generic_vals(new_s);
	get_generic_arrows(new_s);
	get_points(new_s->points, closed_spline(new_s));
	if (int_spline(new_s))
	    remake_control_points(new_s);
	new_s->next = NULL;
	change_spline(old_s, new_s);
	draw_spline(new_s, PAINT);
	toggle_splinemarker(new_s);
	reset_cursor();
	break;
    case CANCEL:
	if (changed) {
	    draw_spline(new_s, ERASE);
	    draw_spline(old_s, PAINT);
	}
	new_s->next = NULL;
	free_spline(&new_s);
	toggle_splinemarker(old_s);
	reset_cursor();
	break;
    }
}

static void
new_generic_values()
{
    int             fill;
    char           *val;

    generic_vals.thickness = atoi(panel_get_value(thickness_panel));
    generic_vals.color = atoi(panel_get_value(color_panel));
    generic_vals.depth = atoi(panel_get_value(depth_panel));
    /* include dash length in panel, too */
    generic_vals.style_val = (float) atof(panel_get_value(style_val_panel));
    if (fill_flag) {
	val = panel_get_value(area_fill_panel);
	if (*val >= ' ' && *val <= '9') {
	    if ((fill = atoi(val)) > 100)
		fill = 100;
	    generic_vals.area_fill = (fill / (100 / (NUMFILLPATS - 1))) + 1;
	}
	fill = (generic_vals.area_fill - 1) * (100 / (NUMFILLPATS - 1));
	sprintf(buf, "%d", fill);
	panel_set_value(area_fill_panel, buf);
    } else
	generic_vals.area_fill = 0;
}

#ifdef notdef
/* not used at present */
static
new_arrow_values()
{
    generic_vals.for_arrow = panel_get_value(for_arrow_panel);
    generic_vals.back_arrow = panel_get_value(back_arrow_panel);
}
#endif

static          XtCallbackProc
done_button(panel_local, item, event)
    Widget          panel_local;
    Widget         *item;
    int            *event;
{
    button_result = DONE;
    done_proc();
    Quit(NULL, NULL, NULL);
}

static          XtCallbackProc
apply_button(panel_local, item, event)
    Widget          panel_local;
    Widget         *item;
    int            *event;
{
    button_result = APPLY;
    done_proc();
}

static          XtCallbackProc
cancel_button(panel_local, item, event)
    Widget          panel_local;
    Widget         *item;
    int            *event;
{
    button_result = CANCEL;
    done_proc();
    Quit(NULL, NULL, NULL);
}

/*
 * the following pix_table entries are guaranteed to be initialized to 0 by
 * the compiler
 */

static struct {
    PIXRECT         image;
    Pixmap          image_pm;
}               pix_table[NUM_IMAGES];

static
generic_window(object_type, sub_type, icon, d_proc, generics, arrows)
    char           *object_type, *sub_type;
    PIXRECT         icon;
    int             (*d_proc) ();
int             generics, arrows;

{
    Position        x_val, y_val;
    Dimension       width, height;
    Dimension       label_height, image_height;
    int             button_distance;
    int             i, fill, dist;
    Widget          image;
    Pixmap          image_pm;

#ifdef OPENWIN_BUG
    /* to cater for OpenWindows bug - see below */
    Pixel           fg, bg;

#endif

    static char    *linestyle_items[] = {
    "Solid Line ", "Dashed Line", "Dotted Line"};
    static char    *area_fill_items[] = {
    "No fill", "Filled "};

    FirstArg(XtNwidth, &width);
    NextArg(XtNheight, &height);
    GetValues(tool);
    XtTranslateCoords(tool, (Position) (width / 2), (Position) (height / 5),
		      &x_val, &y_val);

    FirstArg(XtNx, x_val);
    NextArg(XtNy, y_val);

    popup = XtCreatePopupShell("xfig: edit panel",
			       transientShellWidgetClass, tool,
			       Args, ArgCount);

    form = XtCreateManagedWidget("form", formWidgetClass, popup, NULL, 0);

    done_proc = d_proc;

    sprintf(buf, "%s:%s", object_type, sub_type);
    FirstArg(XtNborderWidth, 0);
    label = XtCreateManagedWidget(buf, labelWidgetClass, form, Args, ArgCount);

    FirstArg(XtNfromHoriz, label);
    NextArg(XtNbottom, XtChainTop);
    NextArg(XtNleft, XtChainRight);
    NextArg(XtNright, XtChainRight);
    image = XtCreateManagedWidget("image", labelWidgetClass, form,
				  Args, ArgCount);

    /* put in the image */
    /* search to see if that pixmap has already been created */
    image_pm = NULL;
    for (i = 0; i < NUM_IMAGES; i++) {
	if (pix_table[i].image == 0)
	    break;
	if (pix_table[i].image == icon) {
	    image_pm = pix_table[i].image_pm;
	    break;
	}
    }

    /* doesn't already exist, create a pixmap from the data (ala panel.c) */
    /* OpenWindows bug doesn't handle a 1-plane bitmap on a n-plane display */
    if (!image_pm) {
#ifdef OPENWIN_BUG
	/* get the foreground/background of the widget */
	FirstArg(XtNforeground, &fg);
	NextArg(XtNbackground, &bg);
	GetValues(image);

	image_pm = XCreatePixmapFromBitmapData(tool_d, canvas_win,
				      icon->data, icon->width, icon->height,
				     fg, bg, XDefaultDepthOfScreen(tool_s));
#else
	image_pm = XCreateBitmapFromData(tool_d, canvas_win,
				     icon->data, icon->width, icon->height);
#endif
	pix_table[i].image_pm = image_pm;
	pix_table[i].image = icon;
    }
    FirstArg(XtNbitmap, image_pm);
    SetValues(image);

    /* get height of label widget and distance between widgets */
    FirstArg(XtNheight, &label_height);
    NextArg(XtNvertDistance, &button_distance);
    GetValues(label);
    /* do the same for the image widget */
    FirstArg(XtNheight, &image_height);
    GetValues(image);

    if (image_height > label_height)
	dist = image_height - label_height + button_distance;
    else
	dist = button_distance;
    FirstArg(XtNfromVert, label);
    NextArg(XtNvertDistance, dist);
    but1 = XtCreateManagedWidget("done", commandWidgetClass, form, Args, ArgCount);
    XtAddCallback(but1, XtNcallback, done_button, NULL);

    below = but1;
    FirstArg(XtNfromHoriz, but1);
    NextArg(XtNfromVert, label);
    NextArg(XtNvertDistance, dist);
    but1 = XtCreateManagedWidget("apply", commandWidgetClass, form, Args, ArgCount);
    XtAddCallback(but1, XtNcallback, apply_button, NULL);

    FirstArg(XtNfromHoriz, but1);
    NextArg(XtNfromVert, label);
    NextArg(XtNvertDistance, dist);
    but1 = XtCreateManagedWidget("cancel", commandWidgetClass, form, Args, ArgCount);
    XtAddCallback(but1, XtNcallback, cancel_button, NULL);

    FirstArg(XtNborderWidth, 0);
    NextArg(XtNfromVert, below);
    below = XtCreateManagedWidget(" ", labelWidgetClass, form, Args, ArgCount);

    if (generics) {
	int_panel(generic_vals.thickness, "Width =", &thickness_panel);
	int_panel(generic_vals.color, "Color =", &color_panel);
	int_panel(generic_vals.depth, "Depth =", &depth_panel);

	if (generic_vals.area_fill == 0) {
	    fill = 0;
	    fill_flag = False;
	} else {
	    fill = (generic_vals.area_fill - 1) * (100 / (NUMFILLPATS - 1));
	    fill_flag = True;
	}

	/* make popup fill area menu */
	FirstArg(XtNfromVert, below);
	NextArg(XtNborderWidth, 0);
	beside = XtCreateManagedWidget("Area fill =", labelWidgetClass,
				       form, Args, ArgCount);
	FirstArg(XtNfromVert, below);
	NextArg(XtNfromHoriz, beside);
	fill_flag_panel = XtCreateManagedWidget(
		  area_fill_items[fill_flag ? 1 : 0], menuButtonWidgetClass,
						form, Args, ArgCount);
	below = fill_flag_panel;
	menu = make_popup_menu(area_fill_items, XtNumber(area_fill_items),
			       fill_flag_panel, area_fill_select);

	int_panel(fill, "Fill density % =", &area_fill_panel);
	area_fill_label = beside;	/* save pointer to fill label */
	FirstArg(XtNsensitive, fill_flag ? True : False);
	SetValues(area_fill_panel);
	NextArg(XtNhorizDistance, 30);
	SetValues(area_fill_label);
	/* if fill is off, blank out fill % value */
	if (!fill_flag)
	    panel_clear_value(area_fill_panel);

	/* make popup line style menu */
	FirstArg(XtNfromVert, below);
	NextArg(XtNborderWidth, 0);
	beside = XtCreateManagedWidget("Line style =", labelWidgetClass,
				       form, Args, ArgCount);
	FirstArg(XtNfromVert, below);
	NextArg(XtNfromHoriz, beside);
	style_panel = XtCreateManagedWidget(
		 linestyle_items[generic_vals.style], menuButtonWidgetClass,
					    form, Args, ArgCount);
	below = style_panel;
	menu = make_popup_menu(linestyle_items, XtNumber(linestyle_items),
			       style_panel, line_style_select);

	/* new field for style_val */
	float_panel(generic_vals.style_val, "Dash length/Dot gap =",
		    &style_val_panel);
	/* save pointer to dash/dot gap label panel */
	style_val_label = beside;
	FirstArg(XtNhorizDistance, 30);
	SetValues(style_val_label);
	if (generic_vals.style == SOLID_LINE) {
	    FirstArg(XtNsensitive, False);
	    SetValues(style_val_panel);
	    SetValues(style_val_label);
	    /* and clear any value from the dash length panel */
	    panel_clear_value(style_val_panel);
	}
    }
}

/* make a button panel with the image 'pixmap' in it */
/* for the font selection */

void            f_menu_popup();

static XtCallbackRec f_sel_callback[] =
{
    {f_menu_popup, NULL},
    {NULL, NULL},
};

set_font_image(widget)
    TOOL            widget;
{
    FirstArg(XtNbitmap, new_psflag ?
	     psfont_menu_bitmaps[new_ps_font] :
	     latexfont_menu_bitmaps[new_latex_font]);
    SetValues(widget);
}

static
font_image_panel(pixmap, label, pi_x)
    Pixmap          pixmap;
    char           *label;
    Widget         *pi_x;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    below = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);

    FirstArg(XtNfromVert, below);
    NextArg(XtNvertDistance, 2);
    NextArg(XtNbitmap, pixmap);
    NextArg(XtNcallback, f_sel_callback);
    *pi_x = XtCreateManagedWidget(label, commandWidgetClass, form, Args, ArgCount);
    below = *pi_x;
}

/* come here when user presses font image button */

void
f_menu_popup()
{
    fontpane_popup(&new_ps_font, &new_latex_font, &new_psflag,
		   set_font_image, font_panel);
}

/*
 * make a popup menu with "nent" button entries (labels) that call "callback"
 * when pressed
 */

Widget
make_popup_menu(entries, nent, parent, callback)
    char           *entries[];
Cardinal        nent;
Widget          parent;
XtCallbackProc  callback;

{
    Widget          pop_menu, entry;
    int             i;

    pop_menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, parent,
				  NULL, ZERO);

    for (i = 0; i < nent; i++) {
	entry = XtCreateManagedWidget(entries[i], smeBSBObjectClass, pop_menu,
				      NULL, ZERO);
	XtAddCallback(entry, XtNcallback, callback, (XtPointer) i);
    }
    return pop_menu;
}

static
int_panel(x, label, pi_x)
    int             x;
    char           *label;
    Widget         *pi_x;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);

    sprintf(buf, "%d", x);
    ArgCount = 1;
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_x = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);
    below = *pi_x;
}

static
float_panel(x, label, pi_x)
    float           x;
    char           *label;
    Widget         *pi_x;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form,
				   Args, ArgCount);
    sprintf(buf, "%1.1f", x);
    FirstArg(XtNfromVert, below);
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_x = XtCreateManagedWidget(label, asciiTextWidgetClass, form,
				  Args, ArgCount);
    below = *pi_x;
}

static
float_label(x, label, pi_x)
    float           x;
    char           *label;
    Widget         *pi_x;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form,
				   Args, ArgCount);
    sprintf(buf, "%1.1f", x);
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, buf);
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNwidth, 40);
    NextArg(XtNborderWidth, 0);
    *pi_x = XtCreateManagedWidget(label, labelWidgetClass, form,
				  Args, ArgCount);
    below = *pi_x;
}

static
int_label(x, label, pi_x)
    int             x;
    char           *label;
    Widget         *pi_x;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form,
				   Args, ArgCount);
    sprintf(buf, "%d", x);
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, buf);
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNwidth, 40);
    NextArg(XtNborderWidth, 0);
    *pi_x = XtCreateManagedWidget(label, labelWidgetClass, form,
				  Args, ArgCount);
    below = *pi_x;
}

/* don't allow newlines in text until we handle multiple line texts */

String          text_translations =
"<Key>Return: no-op(RingBell)\n\
	Ctrl<Key>J: no-op(RingBell)\n\
	Ctrl<Key>M: no-op(RingBell)\n";

static
str_panel(string, label, pi_x)
    char           *string;
    char           *label;
    Widget         *pi_x;
{
    int             width, nlines, i;
    Dimension       pwidth;
    PIX_FONT        temp_font;

    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);

    /* get the font and width of above label widget */
    FirstArg(XtNfont, &temp_font);
    NextArg(XtNwidth, &pwidth);
    GetValues(beside);
    /* make panel as wide as image pane above less the label widget's width */
    /* but at least 200 pixels wide */
    width = max2(PS_FONTPANE_WD - pwidth + 2, 200);

    /* count number of lines in this text string */
    nlines = 1;			/* number of lines in string */
    for (i = 0; i < strlen(string); i++) {
	if (string[i] == '\n') {
	    nlines++;
	}
    }
    if (nlines > 6)		/* limit to displaying 6 lines and show
				 * scrollbars */
	nlines = 6;
    if (nlines == 1)		/* if only one line, allow space for
				 * scrollbar */
	nlines = 2;
    FirstArg(XtNfromVert, below);
    NextArg(XtNstring, string);
    NextArg(XtNinsertPosition, strlen(string));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "edit");
    NextArg(XtNwidth, width);
    NextArg(XtNheight, char_height(temp_font) * nlines + 4);
    NextArg(XtNscrollHorizontal, XawtextScrollWhenNeeded);
    NextArg(XtNscrollVertical, XawtextScrollWhenNeeded);
    *pi_x = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);

    /* make Newline do nothing for now */
    XtOverrideTranslations(*pi_x, XtParseTranslationTable(text_translations));

    below = *pi_x;
}

static
xy_panel(x, y, label, pi_x, pi_y)
    int             x, y;
    char           *label;
    Widget         *pi_x, *pi_y;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNlabel, label);
    NextArg(XtNborderWidth, 0);
    below = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);
    FirstArg(XtNfromVert, below);
    NextArg(XtNhorizDistance, 30);
    NextArg(XtNlabel, "X =");
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);

    sprintf(buf, "%d", x);
    ArgCount = 1;
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_x = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);

    ArgCount = 1;
    NextArg(XtNlabel, "Y =");
    NextArg(XtNborderWidth, 0);
    NextArg(XtNfromHoriz, *pi_x);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);

    sprintf(buf, "%d", y);
    ArgCount = 1;
    NextArg(XtNstring, buf);
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_y = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);

    below = *pi_x;
}

static
f_pos_panel(fp, label, pi_x, pi_y)
    F_pos          *fp;
    char           *label;
    Widget         *pi_x, *pi_y;
{
    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    below = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);
    FirstArg(XtNfromVert, below);
    NextArg(XtNhorizDistance, 30);
    NextArg(XtNlabel, "X =");
    NextArg(XtNborderWidth, 0);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);
    sprintf(buf, "%d", fp->x);
    ArgCount = 1;
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_x = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);
    ArgCount = 1;
    NextArg(XtNlabel, "Y =");
    NextArg(XtNborderWidth, 0);
    NextArg(XtNfromHoriz, *pi_x);
    beside = XtCreateManagedWidget(label, labelWidgetClass, form, Args, ArgCount);
    sprintf(buf, "%d", fp->y);
    ArgCount = 1;
    NextArg(XtNstring, buf);
    NextArg(XtNinsertPosition, strlen(buf));
    NextArg(XtNfromHoriz, beside);
    NextArg(XtNeditType, "append");
    NextArg(XtNwidth, 40);
    *pi_y = XtCreateManagedWidget(label, asciiTextWidgetClass, form, Args, ArgCount);
    below = *pi_x;
}

static
get_f_pos(fp, pi_x, pi_y)
    F_pos          *fp;
    Widget          pi_x, pi_y;
{
    fp->x = (atoi(panel_get_value(pi_x)));
    fp->y = (atoi(panel_get_value(pi_y)));
}

static
points_panel(p, closed)
    struct f_point *p;
    int             closed;
{
    char            buf[32];
    char            bufxy[32];
    int             i;

    FirstArg(XtNfromVert, below);
    NextArg(XtNborderWidth, 0);
    below = XtCreateManagedWidget("Points:", labelWidgetClass, form,
				  Args, ArgCount);
    for (i = 0; p != NULL; i++) {
	if (i >= MAX_POINTS)
	    break;
	FirstArg(XtNfromVert, below);
	NextArg(XtNhorizDistance, 30);
	NextArg(XtNborderWidth, 0);
	sprintf(buf, "X%d =", i);
	beside = XtCreateManagedWidget(buf, labelWidgetClass, form,
				       Args, ArgCount);
	sprintf(bufxy, "%d", p->x);
	ArgCount = 1;
	NextArg(XtNstring, bufxy);
	NextArg(XtNinsertPosition, strlen(bufxy));
	NextArg(XtNfromHoriz, beside);
	NextArg(XtNeditType, "append");
	NextArg(XtNwidth, 40);
	px_panel[i] = XtCreateManagedWidget("xy", asciiTextWidgetClass,
					    form, Args, ArgCount);

	sprintf(buf, "Y%d =", i);
	ArgCount = 1;
	NextArg(XtNfromHoriz, px_panel[i]);
	NextArg(XtNborderWidth, 0);
	beside = XtCreateManagedWidget(buf, labelWidgetClass,
				       form, Args, ArgCount);

	sprintf(bufxy, "%d", p->y);
	ArgCount = 1;
	NextArg(XtNstring, bufxy);
	NextArg(XtNinsertPosition, strlen(bufxy));
	NextArg(XtNfromHoriz, beside);
	NextArg(XtNeditType, "append");
	NextArg(XtNwidth, 40);

	py_panel[i] = XtCreateManagedWidget("xy", asciiTextWidgetClass,
					    form, Args, ArgCount);
	below = px_panel[i];

	p = p->next;
	if (closed && (p == NULL || p->next == NULL))
	    break;
    }
}

static
get_points(p, closed)
    struct f_point *p;
{
    struct f_point *q;
    int             i;

    for (q = p, i = 0; q != NULL; i++) {
	if (i >= MAX_POINTS)
	    break;
	q->x = (atoi(panel_get_value(px_panel[i])));
	q->y = (atoi(panel_get_value(py_panel[i])));
	q = q->next;
	if (closed) {
	    if (q == NULL)
		break;
	    else if (q->next == NULL) {
		q->x = p->x;
		q->y = p->y;
		break;
	    }
	}
    }
}

void
Quit(widget, client_data, call_data)
    Widget          widget;
    XtPointer       client_data, call_data;
{
    XtDestroyWidget(popup);
}

char           *
panel_get_value(widg)
    Widget          widg;
{
    char           *val;

    FirstArg(XtNstring, &val);
    GetValues(widg);
    return val;

}

panel_clear_value(widg)
    Widget          widg;
{
    FirstArg(XtNstring, " ");
    NextArg(XtNinsertPosition, 0);
    SetValues(widg);
}

int
panel_set_value(widg, val)
    Widget          widg;
    char           *val;
{
    FirstArg(XtNstring, val);
    /* I don't know why this doesn't work? */
    /* NextArg(XtNinsertPosition, strlen(val)); */
    SetValues(widg);
    XawTextSetInsertionPoint(widg, strlen(val));
}

static void
line_style_select(w, new_style, garbage)
    Widget          w;
    XtPointer       new_style, garbage;
{
    FirstArg(XtNlabel, XtName(w));
    SetValues(style_panel);

    generic_vals.style = (int) new_style;

    switch (generic_vals.style) {
    case SOLID_LINE:
	panel_clear_value(style_val_panel);
	FirstArg(XtNsensitive, False);
	break;
    case DASH_LINE:
	/*
	 * if style_val contains no useful value, set it to the default
	 * dashlength, scaled by the line thickness
	 */
	if (generic_vals.style_val < 0.1)
	    generic_vals.style_val = cur_dashlength * (cur_linewidth + 1) / 2;
	sprintf(buf, "%1.1f", generic_vals.style_val);
	panel_set_value(style_val_panel, buf);
	FirstArg(XtNsensitive, True);
	break;
    case DOTTED_LINE:
	if (generic_vals.style_val < 0.1)
	    generic_vals.style_val = cur_dotgap * (cur_linewidth + 1) / 2;
	sprintf(buf, "%1.1f", generic_vals.style_val);
	panel_set_value(style_val_panel, buf);
	FirstArg(XtNsensitive, True);
	break;
    }
    /* make both the label and value panels sensitive or insensitive */
    SetValues(style_val_panel);
    SetValues(style_val_label);
}

static void
hidden_text_select(w, new_hidden_text, garbage)
    Widget          w;
    XtPointer       new_hidden_text, garbage;
{
    FirstArg(XtNlabel, XtName(w));
    SetValues(hidden_text_panel);
    hidden_text_flag = (int) new_hidden_text;
}

static void
rigid_text_select(w, new_rigid_text, garbage)
    Widget          w;
    XtPointer       new_rigid_text, garbage;
{
    FirstArg(XtNlabel, XtName(w));
    SetValues(rigid_text_panel);
    rigid_text_flag = (int) new_rigid_text;
}

static void
special_text_select(w, new_special_text, garbage)
    Widget          w;
    XtPointer       new_special_text, garbage;
{
    FirstArg(XtNlabel, XtName(w));
    SetValues(special_text_panel);
    special_text_flag = (int) new_special_text;
}

static void
textjust_select(w, new_textjust, garbage)
    Widget          w;
    XtPointer       new_textjust, garbage;
{
    FirstArg(XtNlabel, XtName(w));
    SetValues(textjust_panel);
    textjust = (int) new_textjust;
}

static void
flip_eps_select(w, new_flipflag, garbage)
    Widget          w;
    XtPointer       new_flipflag, garbage;
{
    struct f_point  p1, p2;
    int             dx, dy, rotation;
    float           ratio;

    FirstArg(XtNlabel, XtName(w));
    SetValues(flip_eps_panel);
    flip_eps_flag = (int) new_flipflag;
    p1.x = atoi(panel_get_value(x1_panel));
    p1.y = atoi(panel_get_value(y1_panel));
    p2.x = atoi(panel_get_value(x2_panel));
    p2.y = atoi(panel_get_value(y2_panel));
    dx = p2.x - p1.x;
    dy = p2.y - p1.y;
    rotation = 0;
    if (dx < 0 && dy < 0)
	rotation = 180;
    else if (dx < 0 && dy >= 0)
	rotation = 270;
    else if (dy < 0 && dx >= 0)
	rotation = 90;
    if (dx == 0 || dy == 0)
	ratio = 0.0;
    else if (((rotation == 0 || rotation == 180) && !flip_eps_flag) ||
	(rotation != 0 && rotation != 180 && flip_eps_flag))
	ratio = fabs((float) dy / (float) dx);
    else
	ratio = fabs((float) dx / (float) dy);
    sprintf(buf, "%1.1f", ratio);
    FirstArg(XtNlabel, buf);
    SetValues(hw_ratio_panel);
}

static void
area_fill_select(w, new_fillflag, garbage)
    Widget          w;
    XtPointer       new_fillflag, garbage;
{
    int             fill;

    FirstArg(XtNlabel, XtName(w));
    SetValues(fill_flag_panel);
    fill_flag = (int) new_fillflag;

    if (fill_flag) {
	fill = (generic_vals.area_fill - 1) * (100 / (NUMFILLPATS - 1));
	if (fill < 0)
	    fill = 0;
	/* if no fill, blank out fill density value */
	sprintf(buf, "%d", fill);
	panel_set_value(area_fill_panel, buf);
    } else {
	panel_clear_value(area_fill_panel);
    }

    /* make fill% panel sensitive or insensitive depending on fill flag */
    FirstArg(XtNsensitive, fill_flag ? True : False);
    SetValues(area_fill_panel);
    SetValues(area_fill_label);
}
