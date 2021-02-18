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

#include "fig.h"
#include "resources.h"
#include "mode.h"
#include "object.h"
#include "u_undo.h"
#include "w_setup.h"

extern int      num_object;

int
load_file(file)
    char           *file;
{
    int             s;
    F_compound      c;

    c.arcs = NULL;
    c.compounds = NULL;
    c.ellipses = NULL;
    c.lines = NULL;
    c.splines = NULL;
    c.texts = NULL;
    c.next = NULL;
    set_temp_cursor(&wait_cursor);
    s = read_fig(file, &c);
    if (s == 0) {		/* Successful read */
	clean_up();
	(void) strcpy(save_filename, cur_filename);
	(void) strcpy(cur_filename, file);
	saved_objects = objects;
	objects = c;
	redisplay_canvas();
	put_msg("Current figure \"%s\" (%d objects)", file, num_object);
	set_action(F_LOAD);
	reset_cursor();
	reset_modifiedflag();
	return (0);
    } else if (s == ENOENT) {
	clean_up();
	saved_objects = objects;
	objects = c;
	redisplay_canvas();
	put_msg("Current figure \"%s\" (new file)", file);
	(void) strcpy(save_filename, cur_filename);
	(void) strcpy(cur_filename, file);
	set_action(F_LOAD);
	reset_cursor();
	reset_modifiedflag();
	return (0);
    }
    read_fail_message(file, s);
    reset_modifiedflag();
    reset_cursor();
    return (1);
}

int
merge_file(file)
    char           *file;
{
    F_compound      c;
    int             s;

    c.arcs = NULL;
    c.compounds = NULL;
    c.ellipses = NULL;
    c.lines = NULL;
    c.splines = NULL;
    c.texts = NULL;
    c.next = NULL;
    set_temp_cursor(&wait_cursor);

    s = read_fig(file, &c);
    if (s == 0) {		/* Successful read */
	int             xmin, ymin, xmax, ymax;

	compound_bound(&c, &xmin, &ymin, &xmax, &ymax);
	clean_up();
	saved_objects = c;
	tail(&objects, &object_tails);
	append_objects(&objects, &saved_objects, &object_tails);
	redisplay_zoomed_region(xmin, ymin, xmax, ymax);
	put_msg("%d object(s) read from \"%s\"", num_object, file);
	set_action_object(F_ADD, O_ALL_OBJECT);
	reset_cursor();
	set_modifiedflag();
	return (0);
    }
    read_fail_message(file, s);
    reset_cursor();
    return (1);
}
