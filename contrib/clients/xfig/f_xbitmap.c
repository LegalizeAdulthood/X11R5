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
#include "paintop.h"
#include "w_setup.h"

static int create_n_write_bitmap();

int
write_bitmap(file_name)
    char           *file_name;
{
    if (!ok_to_write(file_name, "EXPORT"))
	return (1);

    return (create_n_write_bitmap(file_name));	/* write the bitmap file */
}

static Boolean  havegcs = False;
static GC       sav_fill_gc[NUMFILLPATS];
static GC       sav_un_fill_gc[NUMFILLPATS];
static unsigned long save_fg_color;
static unsigned long save_bg_color;

static int
create_n_write_bitmap(filename)
    char           *filename;
{
    int             xmin, ymin, xmax, ymax;
    int             width, height;
    Window          sav_canvas;
    int             sav_objmask;
    Pixmap          largepm, bitmap;
    extern F_compound objects;
    int             i;
    GC              xgc;

    /* Assume that there is at least one object */
    compound_bound(&objects, &xmin, &ymin, &xmax, &ymax);

    if (appres.DEBUG) {
	elastic_box(xmin, ymin, xmax, ymax);
    }
    /* provide a small margin */
    if ((xmin -= 10) < 0)
	xmin = 0;
    if ((ymin -= 10) < 0)
	ymin = 0;
    if ((xmax += 10) > CANVAS_WD)
	xmax = CANVAS_WD;
    if ((ymax += 10) > CANVAS_HT)
	ymax = CANVAS_HT;

    width = xmax - xmin + 1;
    height = ymax - ymin + 1;

    /* choose foreground/background colors as 1 and 0 respectively */
    /* that way we can just copy the lowest plane to make the bitmap */

    XSetPlaneMask(tool_d, gccache[PAINT], (unsigned long) 1);
    XSetForeground(tool_d, gccache[PAINT], (unsigned long) 1);
    XSetBackground(tool_d, gccache[PAINT], (unsigned long) 0);
    XSetPlaneMask(tool_d, gccache[ERASE], (unsigned long) 1);
    XSetForeground(tool_d, gccache[ERASE], (unsigned long) 0);
    XSetBackground(tool_d, gccache[ERASE], (unsigned long) 0);
    save_fg_color = x_fg_color.pixel;	/* save current colors */
    save_bg_color = x_bg_color.pixel;
    x_fg_color.pixel = 1;	/* set fore=1, back=0 */
    x_bg_color.pixel = 0;
    if (!havegcs) {
	havegcs = True;
	for (i = 0; i < NUMFILLPATS; i++) {	/* save current fill gc's */
	    sav_fill_gc[i] = fill_gc[i];
	    sav_un_fill_gc[i] = un_fill_gc[i];
	}
	init_fill_gc();		/* make some with 0/1 for colors */
    } else
	for (i = 0; i < NUMFILLPATS; i++) {
	    xgc = sav_fill_gc[i];	/* swap our gc's with orig */
	    sav_fill_gc[i] = fill_gc[i];
	    fill_gc[i] = xgc;
	    xgc = sav_un_fill_gc[i];
	    sav_un_fill_gc[i] = un_fill_gc[i];
	    un_fill_gc[i] = xgc;
	}

    /* create pixmap from (0,0) to (xmax,ymax) */
    largepm = XCreatePixmap(tool_d, canvas_win, xmax + 1, ymax + 1,
			    DefaultDepthOfScreen(tool_s));
    /* clear it */
    XFillRectangle(tool_d, largepm, gccache[ERASE], xmin, ymin, width, height);
    sav_canvas = canvas_win;	/* save current canvas window id */
    canvas_win = largepm;	/* make the canvas our pixmap */
    sav_objmask = cur_objmask;	/* save the point marker */
    cur_objmask = M_NONE;
    redisplay_objects(&objects);/* draw the figure into the pixmap */
    XFlush(tool_d);
    canvas_win = sav_canvas;	/* go back to the real canvas */
    cur_objmask = sav_objmask;	/* restore point marker */
    bitmap = XCreatePixmap(tool_d, canvas_win, width, height,
			   DefaultDepthOfScreen(tool_s));
    /* now copy one plane of the pixmap to a bitmap of the correct size */
    XCopyPlane(tool_d, largepm, bitmap, gccache[PAINT],
	       xmin, ymin, width, height, 0, 0, 1);
    x_fg_color.pixel = save_fg_color;	/* put colors back to normal */
    x_bg_color.pixel = save_bg_color;
    XSetPlaneMask(tool_d, gccache[PAINT], (unsigned long) AllPlanes);
    XSetForeground(tool_d, gccache[PAINT], x_fg_color.pixel);
    XSetBackground(tool_d, gccache[PAINT], x_bg_color.pixel);
    XSetPlaneMask(tool_d, gccache[ERASE], (unsigned long) AllPlanes);
    XSetForeground(tool_d, gccache[ERASE], x_bg_color.pixel);
    XSetBackground(tool_d, gccache[ERASE], x_bg_color.pixel);
    for (i = 0; i < NUMFILLPATS; i++) {	/* swap back the fill gc's */
	xgc = sav_fill_gc[i];
	sav_fill_gc[i] = fill_gc[i];
	fill_gc[i] = xgc;
	xgc = sav_un_fill_gc[i];
	sav_un_fill_gc[i] = un_fill_gc[i];
	un_fill_gc[i] = xgc;
    }
    if (XWriteBitmapFile(tool_d, filename, bitmap, width, height, -1, -1)
	!= BitmapSuccess) {
	put_msg("Couldn't write bitmap file");
	XFreePixmap(tool_d, largepm);
	XFreePixmap(tool_d, bitmap);
	return (1);
    } else {
	put_msg("Bitmap written to \"%s\"", filename);
	XFreePixmap(tool_d, largepm);
	XFreePixmap(tool_d, bitmap);
	return (0);
    }
}
