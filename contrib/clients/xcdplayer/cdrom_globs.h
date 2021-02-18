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

/* shorthand */
# define rootwin(x)	RootWindow(XtDisplay(x), XtWindow(x))

/*
 * number of milliseconds to sleep between
 * checking to see what the current track
 * is.
 */
# define TIMER_PERIOD		((unsigned long) 250)

extern Widget		top_setup();
extern Widget		main_setup();

extern void		cb_cdrom_play();
extern void		cb_cdrom_pause();
extern void		cb_cdrom_stop();
extern void		cb_cdrom_rewind();
extern void		cb_cdrom_ff();
extern void		cb_cdrom_eject();
extern void		cb_cdrom_cycle();
extern void		cb_cdrom_quit();
extern void		cb_cdrom_shuffle();

extern void		cdrom_timer_on();
extern void		logo_setup();
extern void		button_setup();
extern void		top_start();
extern void		track_button_update();
extern void		timer_label_update();
extern void		pause_button_reset();
extern void		shuffle_button_set();
extern void		shuffle_button_reset();
extern void		leds_stop();
extern void		shuffle_setup();

extern int		cdrom_open();
extern int		cdrom_init();

extern unsigned char	shuffle_next_track();
extern unsigned char	shuffle_prev_track();

extern Boolean		display_timer;

extern XtAppContext	appc;

extern char		*file;

extern float		volbase;
extern float		volpcent;
