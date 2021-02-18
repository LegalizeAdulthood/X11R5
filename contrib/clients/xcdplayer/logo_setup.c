/*
 * Copyright (C) 1990 Regents of the University of California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of the University of
 * California not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  the University of California makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 */

# include <X11/Intrinsic.h>
# include <X11/StringDefs.h>
# include <X11/Xaw/Form.h>
# include <X11/Xaw/Label.h>
# include <X11/Xaw/Toggle.h>
# include <X11/Xaw/Scrollbar.h>

# include <math.h>
# include <stdio.h>

# include "cdrom_globs.h"
# include "cdrom_sun.h"

# include "logo.xbm"

# define MAXVOL		0xff

static Widget	track_button_widget;
static Widget	timer_label_widget;
static Widget	volume_scroll_widget;

static int	vol;

void
logo_setup(parent_widget)
	Widget		parent_widget;
{
	extern void	cb_track_button();
	extern void	leds_label_setup();
	void		volume_jump_proc();
	void		volume_scroll_proc();
	Widget		logo_form_widget;
	Widget		logo_label_widget;
	Pixmap		logo_label_pixmap;
	Arg		args[1];

	logo_form_widget = XtCreateManagedWidget("logoForm", formWidgetClass,
						 parent_widget,
						 (ArgList) NULL, 0);

	logo_label_widget = XtCreateManagedWidget("logoLabel",
						  labelWidgetClass,
						  logo_form_widget,
						  (ArgList) NULL, 0);

	logo_label_pixmap = XCreateBitmapFromData(XtDisplay(logo_label_widget),
						  rootwin(logo_label_widget),
						  logo_bits,
						  logo_width, logo_height);

	XtSetArg(args[0], XtNbitmap, logo_label_pixmap);
	XtSetValues(logo_label_widget, args, 1);

	leds_label_setup(logo_form_widget);

	track_button_widget = XtCreateManagedWidget("trackButton",
						    toggleWidgetClass,
						    logo_form_widget,
						    (ArgList) NULL, 0);

	XtAddCallback(track_button_widget, XtNcallback, cb_track_button, 0);

	track_button_update();

	if (display_timer == True)
		timer_label_widget = XtCreateManagedWidget("timerLabel",
							   labelWidgetClass,
							   logo_form_widget,
							   (ArgList) NULL, 0);

	timer_label_update();

	volume_scroll_widget = XtCreateManagedWidget("volumeScrollbar",
						     scrollbarWidgetClass,
						     logo_form_widget,
						     (ArgList) NULL, 0);

	XtAddCallback(volume_scroll_widget, XtNjumpProc, volume_jump_proc, 0);
	XtAddCallback(volume_scroll_widget, XtNscrollProc,
		      volume_scroll_proc, 0);

	vol = MAXVOL;
	XawScrollbarSetThumb(volume_scroll_widget, (float) 1.0, (float) -1.0);
}

/*ARGSUSED*/
static void
cb_track_button(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	char		track_buf[40];
	Arg		args[1];
	Boolean		state;

	XtSetArg(args[0], XtNstate, &state);
	XtGetValues(widget, args, 1);

	if (state == True)
		sprintf(track_buf, "%d\n", cdi.cdi_maxtrack);
	else
		sprintf(track_buf, "%d\n", cdi.cdi_curtrack);

	XtSetArg(args[0], XtNlabel, track_buf);
	XtSetValues(widget, args, 1);
}

void
track_button_update() {
	char		track_buf[40];
	Arg		args[1];

	sprintf(track_buf, "%d\n", cdi.cdi_curtrack);

	XtSetArg(args[0], XtNlabel, track_buf);
	XtSetValues(track_button_widget, args, 1);
}

void
timer_label_update() {
	char		timer_buf[40];
	Arg		args[1];

	if (display_timer == False)
		return;

	sprintf(timer_buf, "%02u:%02u\n", cdi.cdi_dur / 60, cdi.cdi_dur % 60);

	XtSetArg(args[0], XtNlabel, timer_buf);
	XtSetValues(timer_label_widget, args, 1);
}

static void
volume_jump_proc(scroll_widget, client_data, percent)
	Widget		scroll_widget;
	XtPointer	client_data;
	XtPointer	percent;
{
	vol = (*(float *) percent) * MAXVOL;

	vol = (vol * volpcent) + volbase;

	if (vol > MAXVOL)
		vol = MAXVOL;
	if (vol <= 0)
		vol = 1;

	debug_printf(1, "volume=%u\n", (unsigned int) vol);

	cdrom_volume(vol, vol);
}

static void
volume_scroll_proc(scroll_widget, client_data, position)
	Widget		scroll_widget;
	XtPointer	client_data;
	XtPointer	position;
{
	Arg		args[1];
	Dimension	length;
	float		top;
	double		abspos;
	double		percent;
	double		vdelta;

	if ((abspos = (int) position) < 0)
		abspos = -abspos;

	XtSetArg(args[0], XtNlength, &length);
	XtGetValues(scroll_widget, args, 1);

	if (length <= 0)
		length = 1;

	percent = abspos / (float) length;

	if ((vdelta = (vol * percent)) < 1.0)
		vdelta = 1.0;

	if ((int) position < 0)
		vol += vdelta;
	else
		vol -= vdelta;

# if defined(notdef)
	vol *= volbase;
# endif /* defined(notdef) */

	if (vol > MAXVOL)
		vol = MAXVOL;
	if (vol <= 0)
		vol = 1;

	cdrom_volume(vol, vol);

	top = (double) vol / (double) MAXVOL;

	XawScrollbarSetThumb(volume_scroll_widget, (float) top, (float) -1.0);
}
