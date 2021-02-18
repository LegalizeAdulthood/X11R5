/*
 * FIG : Facility for Interactive Generation of figures
 * Copyright (c) 1985 by Supoj Sutanthavibul
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

/**************** IMPORTS ****************/

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "paintop.h"
#include "u_draw.h"
#include "u_elastic.h"
#include "u_list.h"
#include "u_undo.h"

/*************** EXPORTS *****************/

/*
 * Object_tails *usually* points to the last object in each linked list in
 * objects.  The exceptions occur when multiple objects are added to a figure
 * (e.g. file read, break compound, undo delete region).  In these cases,
 * the added objects are appended to the object lists (and saved_objects is
 * set up to point to the new objects) but object_tails is not changed.
 * This speeds up a subsequent undo operation which need only set
 * all the "next" fields of objects pointed to by object_tails to NULL.
 */

F_compound      saved_objects = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};
F_compound      object_tails = {NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/*************** LOCAL *****************/

static int      last_object;
static int      last_action = F_NULL;
static F_pos    last_position, new_position;
static int      last_arcpointnum;
static F_point *last_prev_point, *last_selected_point, *last_next_point;

void
undo()
{
    switch (last_action) {
	case F_ADD:
	undo_add();
	break;
    case F_DELETE:
	undo_delete();
	break;
    case F_MOVE:
	undo_move();
	break;
    case F_CHANGE:
	undo_change();
	break;
    case F_GLUE:
	undo_glue();
	break;
    case F_BREAK:
	undo_break();
	break;
    case F_LOAD:
	undo_load();
	break;
    case F_SCALE:
	undo_scale();
	break;
    case F_ADD_POINT:
	undo_addpoint();
	break;
    case F_DELETE_POINT:
	undo_deletepoint();
	break;
    case F_ADD_ARROW_HEAD:
	undo_add_arrowhead();
	break;
    case F_DELETE_ARROW_HEAD:
	undo_delete_arrowhead();
	break;
    case F_CONVERT:
	undo_convert();
	break;
    default:
	put_msg("Nothing to UNDO");
	return;
    }
    put_msg("Undo complete");
}

undo_addpoint()
{
    if (last_object == O_POLYLINE)
	linepoint_deleting(saved_objects.lines, last_prev_point,
			   last_selected_point);
    else
	splinepoint_deleting(saved_objects.splines, last_prev_point,
			     last_selected_point);
}

undo_deletepoint()
{
    last_action = F_NULL;	/* to avoid doing a clean-up during adding */

    if (last_object == O_POLYLINE)
	linepoint_adding(saved_objects.lines, last_prev_point,
			 last_selected_point, last_next_point);
    else
	splinepoint_adding(saved_objects.splines, last_prev_point,
			   last_selected_point, last_next_point);
    last_next_point = NULL;
}

undo_break()
{
    cut_objects(&objects, &object_tails);
    list_add_compound(&objects.compounds, saved_objects.compounds);
    last_action = F_GLUE;
    toggle_markers_in_compound(saved_objects.compounds);
    mask_toggle_compoundmarker(saved_objects.compounds);
}

undo_glue()
{
    list_delete_compound(&objects.compounds, saved_objects.compounds);
    tail(&objects, &object_tails);
    append_objects(&objects, saved_objects.compounds, &object_tails);
    last_action = F_BREAK;
    mask_toggle_compoundmarker(saved_objects.compounds);
    toggle_markers_in_compound(saved_objects.compounds);
}

undo_convert()
{
    switch (last_object) {
    case O_POLYLINE:
	spline_2_line(saved_objects.splines);
	break;
    case O_SPLINE:
	line_2_spline(saved_objects.lines);
	break;
    }
}

undo_add_arrowhead()
{
    switch (last_object) {
    case O_POLYLINE:
	delete_linearrow(saved_objects.lines,
			 last_prev_point, last_selected_point);
	break;
    case O_SPLINE:
	delete_splinearrow(saved_objects.splines,
			   last_prev_point, last_selected_point);
	break;
    case O_ARC:
	delete_arcarrow(saved_objects.arcs, last_arcpointnum);
	break;
    default:
	return;
    }
    last_action = F_DELETE_ARROW_HEAD;
}

undo_delete_arrowhead()
{
    switch (last_object) {
    case O_POLYLINE:
	add_linearrow(saved_objects.lines,
		      last_prev_point, last_selected_point);
	break;
    case O_SPLINE:
	add_splinearrow(saved_objects.splines,
			last_prev_point, last_selected_point);
	break;
    case O_ARC:
	add_arcarrow(saved_objects.arcs, last_arcpointnum);
	break;
    default:
	return;
    }
    last_action = F_ADD_ARROW_HEAD;
}

/*
 * saved_objects.xxxx contains a pointer to the original object,
 * saved_objects.xxxx->next points to the changed object.
 */

undo_change()
{
    last_action = F_NULL;	/* to avoid a clean-up during "unchange" */
    switch (last_object) {
    case O_POLYLINE:
	new_l = saved_objects.lines;	/* the original */
	old_l = saved_objects.lines->next;	/* the changed object */
	mask_toggle_linemarker(old_l);
	draw_line(old_l, ERASE);
	change_line(old_l, new_l);
	draw_line(new_l, PAINT);
	mask_toggle_linemarker(new_l);
	break;
    case O_ELLIPSE:
	new_e = saved_objects.ellipses;
	old_e = saved_objects.ellipses->next;
	mask_toggle_ellipsemarker(old_e);
	draw_ellipse(old_e, ERASE);
	change_ellipse(old_e, new_e);
	draw_ellipse(new_e, PAINT);
	mask_toggle_ellipsemarker(new_e);
	break;
    case O_TEXT:
	new_t = saved_objects.texts;
	old_t = saved_objects.texts->next;
	mask_toggle_textmarker(old_t);
	draw_text(old_t, ERASE);
	change_text(old_t, new_t);
	draw_text(new_t, PAINT);
	mask_toggle_textmarker(new_t);
	break;
    case O_SPLINE:
	new_s = saved_objects.splines;
	old_s = saved_objects.splines->next;
	mask_toggle_splinemarker(old_s);
	draw_spline(old_s, ERASE);
	change_spline(old_s, new_s);
	draw_spline(new_s, PAINT);
	mask_toggle_splinemarker(new_s);
	break;
    case O_ARC:
	new_a = saved_objects.arcs;
	old_a = saved_objects.arcs->next;
	mask_toggle_arcmarker(old_a);
	draw_arc(old_a, ERASE);
	change_arc(old_a, new_a);
	draw_arc(new_a, PAINT);
	mask_toggle_arcmarker(new_a);
	break;
    case O_COMPOUND:
	new_c = saved_objects.compounds;
	old_c = saved_objects.compounds->next;
	mask_toggle_compoundmarker(old_c);
	draw_compoundelements(old_c, ERASE);
	change_compound(old_c, new_c);
	draw_compoundelements(new_c, PAINT);
	mask_toggle_compoundmarker(new_c);
	break;
    }
}

/*
 * When a single object is created, it is appended to the appropriate list
 * in objects.  It is also placed in the appropriate list in saved_objects.
 *
 * When a number of objects are created (usually by reading them in from
 * a file or undoing a remove-all action), they are appended to the lists in
 * objects and also saved in saved_objects.  The pointers in object_tails
 * will be set to point to the last members of the lists in objects prior to
 * the appending.
 *
 * Note: The read operation will set the pointers in object_tails while the
 * remove-all operation will zero pointers in objects.
 */

undo_add()
{
    int             xmin, ymin, xmax, ymax;

    switch (last_object) {
    case O_POLYLINE:
	list_delete_line(&objects.lines, saved_objects.lines);
	mask_toggle_linemarker(saved_objects.lines);
#ifdef FASTSERVER
	line_bound(saved_objects.lines, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_line(saved_objects.lines, ERASE);
#endif
	break;
    case O_ELLIPSE:
	list_delete_ellipse(&objects.ellipses, saved_objects.ellipses);
	mask_toggle_ellipsemarker(saved_objects.ellipses);
#ifdef FASTSERVER
	ellipse_bound(saved_objects.ellipses, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_ellipse(saved_objects.ellipses, ERASE);
#endif
	break;
    case O_TEXT:
	list_delete_text(&objects.texts, saved_objects.texts);
	mask_toggle_textmarker(saved_objects.texts);
#ifdef FASTSERVER
	text_bound(saved_objects.texts, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_text(saved_objects.texts, ERASE);
#endif
	break;
    case O_SPLINE:
	list_delete_spline(&objects.splines, saved_objects.splines);
	mask_toggle_splinemarker(saved_objects.splines);
#ifdef FASTSERVER
	spline_bound(saved_objects.splines, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_spline(saved_objects.splines, ERASE);
#endif
	break;
    case O_ARC:
	list_delete_arc(&objects.arcs, saved_objects.arcs);
	mask_toggle_arcmarker(saved_objects.arcs);
#ifdef FASTSERVER
	arc_bound(saved_objects.arcs, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_arc(saved_objects.arcs, ERASE);
#endif
	break;
    case O_COMPOUND:
	list_delete_compound(&objects.compounds, saved_objects.compounds);
	mask_toggle_compoundmarker(saved_objects.compounds);
#ifdef FASTSERVER
	compound_bound(saved_objects.compounds, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
#else
	draw_compoundelements(saved_objects.compounds, ERASE);
#endif
	break;
    case O_ALL_OBJECT:
	cut_objects(&objects, &object_tails);
	compound_bound(&saved_objects, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	break;
    }
    last_action = F_DELETE;
}

undo_delete()
{
    int             xmin, ymin, xmax, ymax;

    switch (last_object) {
    case O_POLYLINE:
	draw_line(saved_objects.lines, PAINT);
	mask_toggle_linemarker(saved_objects.lines);
	list_add_line(&objects.lines, saved_objects.lines);
	break;
    case O_ELLIPSE:
	draw_ellipse(saved_objects.ellipses, PAINT);
	mask_toggle_ellipsemarker(saved_objects.ellipses);
	list_add_ellipse(&objects.ellipses, saved_objects.ellipses);
	break;
    case O_TEXT:
	draw_text(saved_objects.texts, PAINT);
	mask_toggle_textmarker(saved_objects.texts);
	list_add_text(&objects.texts, saved_objects.texts);
	break;
    case O_SPLINE:
	draw_spline(saved_objects.splines, PAINT);
	mask_toggle_splinemarker(saved_objects.splines);
	list_add_spline(&objects.splines, saved_objects.splines);
	break;
    case O_ARC:
	draw_arc(saved_objects.arcs, PAINT);
	mask_toggle_arcmarker(saved_objects.arcs);
	list_add_arc(&objects.arcs, saved_objects.arcs);
	break;
    case O_COMPOUND:
	draw_compoundelements(saved_objects.compounds, PAINT);
	mask_toggle_compoundmarker(saved_objects.compounds);
	list_add_compound(&objects.compounds, saved_objects.compounds);
	break;
    case O_ALL_OBJECT:
	compound_bound(&saved_objects, &xmin, &ymin, &xmax, &ymax);
	append_objects(&objects, &saved_objects, &object_tails);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
    }
    last_action = F_ADD;
}

undo_move()
{
    int             dx, dy;

#ifdef FASTSERVER
    int             xmin, ymin, xmax, ymax;

#endif

    dx = last_position.x - new_position.x;
    dy = last_position.y - new_position.y;
    switch (last_object) {
    case O_POLYLINE:
	mask_toggle_linemarker(saved_objects.lines);
#ifdef FASTSERVER
	list_delete_line(&objects.lines, saved_objects.lines);
	line_bound(saved_objects.lines, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_line(&objects.lines, saved_objects.lines);
#else
	draw_line(saved_objects.lines, ERASE);
#endif
	translate_line(saved_objects.lines, dx, dy);
	draw_line(saved_objects.lines, PAINT);
	mask_toggle_linemarker(saved_objects.lines);
	break;
    case O_ELLIPSE:
	mask_toggle_ellipsemarker(saved_objects.ellipses);
#ifdef FASTSERVER
	list_delete_ellipse(&objects.ellipses, saved_objects.ellipses);
	ellipse_bound(saved_objects.ellipses, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_ellipse(&objects.ellipses, saved_objects.ellipses);
#else
	draw_ellipse(saved_objects.ellipses, ERASE);
#endif
	translate_ellipse(saved_objects.ellipses, dx, dy);
	draw_ellipse(saved_objects.ellipses, PAINT);
	mask_toggle_ellipsemarker(saved_objects.ellipses);
	break;
    case O_TEXT:
	mask_toggle_textmarker(saved_objects.texts);
#ifdef FASTSERVER
	list_delete_text(&objects.texts, saved_objects.texts);
	text_bound(saved_objects.texts, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_text(&objects.texts, saved_objects.texts);
#else
	draw_text(saved_objects.texts, ERASE);
#endif
	translate_text(saved_objects.texts, dx, dy);
	draw_text(saved_objects.texts, PAINT);
	mask_toggle_textmarker(saved_objects.texts);
	break;
    case O_SPLINE:
	mask_toggle_splinemarker(saved_objects.splines);
#ifdef FASTSERVER
	list_delete_spline(&objects.splines, saved_objects.splines);
	spline_bound(saved_objects.splines, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_spline(&objects.splines, saved_objects.splines);
#else
	draw_spline(saved_objects.splines, ERASE);
#endif
	translate_spline(saved_objects.splines, dx, dy);
	draw_spline(saved_objects.splines, PAINT);
	mask_toggle_splinemarker(saved_objects.splines);
	break;
    case O_ARC:
	mask_toggle_arcmarker(saved_objects.arcs);
#ifdef FASTSERVER
	list_delete_arc(&objects.arcs, saved_objects.arcs);
	arc_bound(saved_objects.arcs, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_arc(&objects.arcs, saved_objects.arcs);
#else
	draw_arc(saved_objects.arcs, ERASE);
#endif
	translate_arc(saved_objects.arcs, dx, dy);
	draw_arc(saved_objects.arcs, PAINT);
	mask_toggle_arcmarker(saved_objects.arcs);
	break;
    case O_COMPOUND:
	mask_toggle_compoundmarker(saved_objects.compounds);
#ifdef FASTSERVER
	list_delete_compound(&objects.compounds, saved_objects.compounds);
	compound_bound(saved_objects.compounds, &xmin, &ymin, &xmax, &ymax);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	list_add_compound(&objects.compounds, saved_objects.compounds);
#else
	draw_compoundelements(saved_objects.compounds, ERASE);
#endif
	translate_compound(saved_objects.compounds, dx, dy);
	draw_compoundelements(saved_objects.compounds, PAINT);
	mask_toggle_compoundmarker(saved_objects.compounds);
	break;
    }
    swap_newp_lastp();
}

undo_load()
{
    F_compound      temp;
    char            ctemp[128];

    temp = objects;
    objects = saved_objects;
    saved_objects = temp;
    strcpy(ctemp, cur_filename);
    strcpy(cur_filename, save_filename);
    strcpy(save_filename, ctemp);
    redisplay_canvas();
    set_modifiedflag();
    last_action = F_LOAD;
}

undo_scale()
{
    float           scalex, scaley;

    mask_toggle_compoundmarker(saved_objects.compounds);
    draw_compoundelements(saved_objects.compounds, ERASE);
    scalex = ((float) (last_position.x - fix_x)) / (new_position.x - fix_x);
    scaley = ((float) (last_position.y - fix_y)) / (new_position.y - fix_y);
    scale_compound(saved_objects.compounds, scalex, scaley, fix_x, fix_y);
    draw_compoundelements(saved_objects.compounds, PAINT);
    mask_toggle_compoundmarker(saved_objects.compounds);
    swap_newp_lastp();
}

swap_newp_lastp()
{
    int             t;		/* swap new_position and last_position  */

    t = new_position.x;
    new_position.x = last_position.x;
    last_position.x = t;
    t = new_position.y;
    new_position.y = last_position.y;
    last_position.y = t;
}

/*
 * Clean_up should be called before committing a user's request. Clean_up
 * will attempt to free all the allocated memories which resulted from
 * delete/remove action.  It will set the last_action to F_NULL.  Thus this
 * routine should be before set_action_object(). if they are to be called in
 * the same routine.
 */
clean_up()
{
    if (last_action == F_CHANGE) {
	switch (last_object) {
	case O_ARC:
	    saved_objects.arcs->next = NULL;
	    free_arc(&saved_objects.arcs);
	    break;
	case O_COMPOUND:
	    saved_objects.compounds->next = NULL;
	    free_compound(&saved_objects.compounds);
	    break;
	case O_ELLIPSE:
	    saved_objects.ellipses->next = NULL;
	    free_ellipse(&saved_objects.ellipses);
	    break;
	case O_POLYLINE:
	    saved_objects.lines->next = NULL;
	    free_line(&saved_objects.lines);
	    break;
	case O_SPLINE:
	    saved_objects.splines->next = NULL;
	    free_spline(&saved_objects.splines);
	    break;
	case O_TEXT:
	    saved_objects.texts->next = NULL;
	    free_text(&saved_objects.texts);
	    break;
	}
    } else if (last_action == F_DELETE) {
	switch (last_object) {
	case O_ARC:
	    free_arc(&saved_objects.arcs);
	    break;
	case O_COMPOUND:
	    free_compound(&saved_objects.compounds);
	    break;
	case O_ELLIPSE:
	    free_ellipse(&saved_objects.ellipses);
	    break;
	case O_POLYLINE:
	    free_line(&saved_objects.lines);
	    break;
	case O_SPLINE:
	    free_spline(&saved_objects.splines);
	    break;
	case O_TEXT:
	    free_text(&saved_objects.texts);
	    break;
	case O_ALL_OBJECT:
	    free_arc(&saved_objects.arcs);
	    free_compound(&saved_objects.compounds);
	    free_ellipse(&saved_objects.ellipses);
	    free_line(&saved_objects.lines);
	    free_spline(&saved_objects.splines);
	    free_text(&saved_objects.texts);
	    break;
	}
    } else if (last_action == F_DELETE_POINT) {
	free((char *) last_selected_point);
	last_prev_point = NULL;
	last_selected_point = NULL;
	last_next_point = NULL;
    } else if (last_action == F_ADD_POINT) {
	last_prev_point = NULL;
	last_selected_point = NULL;
    } else if (last_action == F_LOAD) {
	free_arc(&saved_objects.arcs);
	free_compound(&saved_objects.compounds);
	free_ellipse(&saved_objects.ellipses);
	free_line(&saved_objects.lines);
	free_spline(&saved_objects.splines);
	free_text(&saved_objects.texts);
    } else if (last_action == F_GLUE) {
	saved_objects.compounds = NULL;
    } else if (last_action == F_BREAK) {
	free((char *) saved_objects.compounds);
	saved_objects.compounds = NULL;
    } else if (last_action == F_ADD) {
	saved_objects.arcs = NULL;
	saved_objects.compounds = NULL;
	saved_objects.ellipses = NULL;
	saved_objects.lines = NULL;
	saved_objects.splines = NULL;
	saved_objects.texts = NULL;
    } else if (last_action == F_CONVERT) {
	saved_objects.splines = NULL;
	saved_objects.lines = NULL;
    } else if (last_action == F_ADD_ARROW_HEAD ||
	       last_action == F_DELETE_ARROW_HEAD) {
	saved_objects.splines = NULL;
	saved_objects.lines = NULL;
	saved_objects.arcs = NULL;
	last_prev_point = NULL;
	last_selected_point = NULL;
    }
    last_action = F_NULL;
}

set_latestarc(arc)
    F_arc          *arc;
{
    saved_objects.arcs = arc;
}

set_latestobjects(objects)
    F_compound     *objects;
{
    saved_objects = *objects;
}

set_latestcompound(compound)
    F_compound     *compound;
{
    saved_objects.compounds = compound;
}

set_latestellipse(ellipse)
    F_ellipse      *ellipse;
{
    saved_objects.ellipses = ellipse;
}

set_latestline(line)
    F_line         *line;
{
    saved_objects.lines = line;
}

set_latestspline(spline)
    F_spline       *spline;
{
    saved_objects.splines = spline;
}

set_latesttext(text)
    F_text         *text;
{
    saved_objects.texts = text;
}

set_last_prevpoint(prev_point)
    F_point        *prev_point;
{
    last_prev_point = prev_point;
}

set_last_selectedpoint(selected_point)
    F_point        *selected_point;
{
    last_selected_point = selected_point;
}

set_last_nextpoint(next_point)
    F_point        *next_point;
{
    last_next_point = next_point;
}

set_last_arcpointnum(num)
    int             num;
{
    last_arcpointnum = num;
}

set_lastposition(x, y)
    int             x, y;
{
    last_position.x = x;
    last_position.y = y;
}

set_newposition(x, y)
    int             x, y;
{
    new_position.x = x;
    new_position.y = y;
}

set_action(action)
    int             action;
{
    last_action = action;
}

set_action_object(action, object)
    int             action, object;
{
    last_action = action;
    last_object = object;
}
