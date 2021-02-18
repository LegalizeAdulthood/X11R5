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
# include <X11/Xaw/Toggle.h>

# include "cdrom_globs.h"
# include "cdrom_sun.h"

# include "play.xbm"
# include "pause.xbm"
# include "stop.xbm"
# include "rewind.xbm"
# include "ff.xbm"
# include "eject.xbm"

# include "quit.xbm"
# include "cycle.xbm"
# include "shuffle.xbm"

extern Widget	play_button_setup();
extern Widget	stop_button_setup();

extern void	pause_button_setup();
extern void	rew_button_setup();
extern void	ff_button_setup();
extern void	eject_button_setup();
extern void	quit_button_setup();
extern void	cycle_button_setup();
extern void	shuffle_button_setup();

static Widget	pause_button_widget;
static Widget	shuffle_button_widget;

void
button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		buttons_form_widget;
	Widget		play_button_widget;
	Widget		stop_button_widget;
	Arg		args[1];

	buttons_form_widget = XtCreateManagedWidget("buttonsForm",
						    formWidgetClass,
						    parent_widget,
						    (ArgList) NULL, 0);

	play_button_widget = play_button_setup(buttons_form_widget);

	pause_button_setup(buttons_form_widget);


	stop_button_widget = stop_button_setup(buttons_form_widget);

	rew_button_setup(buttons_form_widget);

	ff_button_setup(buttons_form_widget);

	eject_button_setup(buttons_form_widget);

	quit_button_setup(buttons_form_widget);

	cycle_button_setup(buttons_form_widget);

	shuffle_button_setup(buttons_form_widget);

	/* set the initial state of the buttons */
	switch (cdrom_status()) {
		case CDROM_PLAYING:
		XtSetArg(args[0], XtNstate, True);
		XtSetValues(play_button_widget, args, 1);
		cdi.cdi_state = CDROM_STATE_PLAY;
		cdrom_timer_on();
		break;

		case CDROM_PAUSED:
		XtSetArg(args[0], XtNstate, True);
		XtSetValues(play_button_widget, args, 1);
		XtSetArg(args[0], XtNstate, True);
		XtSetValues(pause_button_widget, args, 1);
		cdi.cdi_state = CDROM_STATE_PLAY | CDROM_STATE_PAUSE;
		break;

		default:
		XtSetArg(args[0], XtNstate, True);
		XtSetValues(stop_button_widget, args, 1);
		cdi.cdi_state = CDROM_STATE_EJECTED;
		break;
	}
}

static Widget
play_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		play_button_widget;
	Pixmap		play_button_pixmap;
	Arg		args[1];

	play_button_widget = XtCreateManagedWidget("playButton",
						   toggleWidgetClass,
						   parent_widget,
						   (ArgList) NULL, 0);

	play_button_pixmap = XCreateBitmapFromData(XtDisplay(play_button_widget),
						   rootwin(play_button_widget),
						   play_bits,
						   play_width, play_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) play_button_pixmap);
	XtSetValues(play_button_widget, args, 1);

	XtAddCallback(play_button_widget, XtNcallback, cb_cdrom_play, 0);

	return(play_button_widget);
}

static void
pause_button_setup(parent_widget)
	Widget		parent_widget;
{
	Pixmap		pause_button_pixmap;
	Arg		args[1];

	pause_button_widget = XtCreateManagedWidget("pauseButton",
						    toggleWidgetClass,
						    parent_widget,
						    (ArgList) NULL, 0);

	pause_button_pixmap = XCreateBitmapFromData(XtDisplay(pause_button_widget),
						    rootwin(pause_button_widget),
						    pause_bits,
						    pause_width, pause_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) pause_button_pixmap);
	XtSetValues(pause_button_widget, args, 1);

	XtAddCallback(pause_button_widget, XtNcallback, cb_cdrom_pause, 0);
}

static Widget
stop_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		stop_button_widget;
	Pixmap		stop_button_pixmap;
	Arg		args[1];

	stop_button_widget = XtCreateManagedWidget("stopButton",
						   toggleWidgetClass,
						   parent_widget,
						   (ArgList) NULL, 0);

	stop_button_pixmap = XCreateBitmapFromData(XtDisplay(stop_button_widget),
						   rootwin(stop_button_widget),
						   stop_bits,
						   stop_width, stop_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) stop_button_pixmap);
	XtSetValues(stop_button_widget, args, 1);

	XtAddCallback(stop_button_widget, XtNcallback, cb_cdrom_stop, 0);

	return(stop_button_widget);
}

static void
rew_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		rew_button_widget;
	Pixmap		rew_button_pixmap;
	Arg		args[1];

	rew_button_widget = XtCreateManagedWidget("rewButton",
						  toggleWidgetClass,
						  parent_widget,
						  (ArgList) NULL, 0);

	rew_button_pixmap = XCreateBitmapFromData(XtDisplay(rew_button_widget),
						  rootwin(rew_button_widget),
						  rewind_bits,
						  rewind_width, rewind_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) rew_button_pixmap);
	XtSetValues(rew_button_widget, args, 1);

	XtAddCallback(rew_button_widget, XtNcallback, cb_cdrom_rewind, 0);
}

static void
ff_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		ff_button_widget;
	Pixmap		ff_button_pixmap;
	Arg		args[1];

	ff_button_widget = XtCreateManagedWidget("ffButton",
						 toggleWidgetClass,
						 parent_widget,
						 (ArgList) NULL, 0);

	ff_button_pixmap = XCreateBitmapFromData(XtDisplay(ff_button_widget),
						 rootwin(ff_button_widget),
						 ff_bits,
						 ff_width, ff_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) ff_button_pixmap);
	XtSetValues(ff_button_widget, args, 1);

	XtAddCallback(ff_button_widget, XtNcallback, cb_cdrom_ff, 0);
}

static void
eject_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		eject_button_widget;
	Pixmap		eject_button_pixmap;
	Arg		args[1];

	eject_button_widget = XtCreateManagedWidget("ejectButton",
						    toggleWidgetClass,
						    parent_widget,
						    (ArgList) NULL, 0);

	eject_button_pixmap = XCreateBitmapFromData(XtDisplay(eject_button_widget),
						    rootwin(eject_button_widget),
						    eject_bits,
						    eject_width, eject_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) eject_button_pixmap);
	XtSetValues(eject_button_widget, args, 1);

	XtAddCallback(eject_button_widget, XtNcallback, cb_cdrom_eject, 0);
}

static void
quit_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		quit_button_widget;
	Pixmap		quit_button_pixmap;
	Arg		args[1];

	quit_button_widget = XtCreateManagedWidget("quitButton",
						    toggleWidgetClass,
						    parent_widget,
						    (ArgList) NULL, 0);

	quit_button_pixmap = XCreateBitmapFromData(XtDisplay(quit_button_widget),
						    rootwin(quit_button_widget),
						    quit_bits,
						    quit_width, quit_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) quit_button_pixmap);
	XtSetValues(quit_button_widget, args, 1);

	XtAddCallback(quit_button_widget, XtNcallback, cb_cdrom_quit, 0);
}

static void
cycle_button_setup(parent_widget)
	Widget		parent_widget;
{
	Widget		cycle_button_widget;
	Pixmap		cycle_button_pixmap;
	Arg		args[1];

	cycle_button_widget = XtCreateManagedWidget("cycleButton",
						    toggleWidgetClass,
						    parent_widget,
						    (ArgList) NULL, 0);

	cycle_button_pixmap = XCreateBitmapFromData(XtDisplay(cycle_button_widget),
						    rootwin(cycle_button_widget),
						    cycle_bits,
						    cycle_width, cycle_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) cycle_button_pixmap);
	XtSetValues(cycle_button_widget, args, 1);

	XtAddCallback(cycle_button_widget, XtNcallback, cb_cdrom_cycle, 0);
}

static void
shuffle_button_setup(parent_widget)
	Widget		parent_widget;
{
	Pixmap		shuffle_button_pixmap;
	Arg		args[1];

	shuffle_button_widget = XtCreateManagedWidget("shuffleButton",
						      toggleWidgetClass,
						      parent_widget,
						      (ArgList) NULL, 0);

	shuffle_button_pixmap = XCreateBitmapFromData(XtDisplay(shuffle_button_widget),
						      rootwin(shuffle_button_widget),
						      shuffle_bits,
						      shuffle_width, shuffle_height);

	XtSetArg(args[0], XtNbitmap, (XtArgVal) shuffle_button_pixmap);
	XtSetValues(shuffle_button_widget, args, 1);

	XtAddCallback(shuffle_button_widget, XtNcallback, cb_cdrom_shuffle, 0);
}

void
pause_button_reset() {
	Arg		args[1];

	XtSetArg(args[0], XtNstate, False);
	XtSetValues(pause_button_widget, args, 1);
}

void
shuffle_button_set() {
	Arg		args[1];

	XtSetArg(args[0], XtNstate, True);
	XtSetValues(shuffle_button_widget, args, 1);
}

void
shuffle_button_reset() {
	Arg		args[1];

	XtSetArg(args[0], XtNstate, False);
	XtSetValues(shuffle_button_widget, args, 1);
}
