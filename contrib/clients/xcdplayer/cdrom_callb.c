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
# include <stdio.h>

# include "debug.h"
# include "cdrom_globs.h"
# include "cdrom_sun.h"

extern void		cdrom_timer_on();
extern void		cdrom_timer_off();
extern void		cdrom_atend();
extern void		leds_update();

extern int		cdrom_play();

static XtIntervalId	ivid			= -1;
static unsigned int	timer_mod		= 1000 / TIMER_PERIOD;
static unsigned int	timer_fsecs;

/*
 * cb_cdrom_play checks to see if we're playing; if so do nothing.
 * if not playing turns on playing.  if paused then return.
 * otherwise start cd playing.
 */
/*ARGSUSED*/
void
cb_cdrom_play(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_play: error from cdrom_open\n");
		return;
	}

	/* toggle button weirdness */
	if (cdi.cdi_state & CDROM_STATE_PLAY) {
		debug_printf(1, "cb_cdrom_play: already playing track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	cdi.cdi_state |= CDROM_STATE_PLAY;

	if (cdi.cdi_state & CDROM_STATE_PAUSE) {
		debug_printf(1, "cb_cdrom_play: paused on track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	timer_fsecs = 0;
	cdi.cdi_dur = 0;
	timer_label_update();

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE)
		cdi.cdi_curtrack = shuffle_next_track();

	if (cdrom_play() != -1)
		cdi.cdi_state &= ~CDROM_STATE_EJECTED;
}

/*
 * cb_cdrom_pause toggles pausing on or off.
 */
/*ARGSUSED*/
void
cb_cdrom_pause(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_pause: error from cdrom_open\n");
		return;
	}

	if (cdi.cdi_state & CDROM_STATE_PAUSE) {
		cdi.cdi_state &= ~CDROM_STATE_PAUSE;

		debug_printf(1, "cb_cdrom_pause: resuming track %d\n",
			    cdi.cdi_curtrack);

		/*
		 * if we use ff or rewind after a pause we can't
		 * just resume but have to move to the track.
		 */
		if (cdi.cdi_curtrack == cdrom_get_curtrack()) {
			if (cdrom_resume() != -1)
				cdi.cdi_state &= ~CDROM_STATE_EJECTED;

			cdrom_timer_on();

			return;
		}

		if (cdrom_play() != -1)
			cdi.cdi_state &= ~CDROM_STATE_EJECTED;

		return;
	}

	cdi.cdi_state |= CDROM_STATE_PAUSE;

	debug_printf(1, "cb_cdrom_pause: pausing on track %d\n",
		    cdi.cdi_curtrack);

	if (cdrom_pause() != -1)
		cdi.cdi_state &= ~CDROM_STATE_EJECTED;

	cdrom_timer_off();
}

/*
 * cb_cdrom_stop checks to see if we're playing; if not then
 * do nothing.  sets the current track to the first audio track.
 * turns off play, pause, stops the cd, and closes it so that the
 * disc can be ejected with the eject button on the drive.
 */
/*ARGSUSED*/
void
cb_cdrom_stop(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_stop: error from cdrom_open\n");
		return;
	}

	cdrom_timer_off();
	leds_stop();

	cdi.cdi_curtrack = cdi.cdi_mintrack;

	track_button_update();
	pause_button_reset();

	cdi.cdi_state &= ~(CDROM_STATE_PAUSE | CDROM_STATE_EJECTED);

	/* toggle button weirdness */
	if ((cdi.cdi_state & CDROM_STATE_PLAY) == 0) {
		debug_printf(1, "cb_cdrom_stop: already stopped\n");
		return;
	}

	cdi.cdi_state &= ~CDROM_STATE_PLAY;

	debug_printf(1, "cb_cdrom_stop: stopping on track %d\n",
		    cdi.cdi_curtrack);

	cdrom_stop();

	cdrom_close();

	cdi.cdi_dur = 0;
	timer_label_update();
}

/*
 * cb_cdrom_rewind decrments the current track.  if paused or stopped
 * then return.  otherwise start playing the (new) current track.
 */
/*ARGSUSED*/
void
cb_cdrom_rewind(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_rewind: error from cdrom_open\n");
		return;
	}

	cdi.cdi_state &= ~CDROM_STATE_EJECTED;

	cdrom_timer_off();

	if (cdi.cdi_curtrack > cdi.cdi_mintrack)
		cdi.cdi_curtrack--;

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE)
		cdi.cdi_curtrack = shuffle_prev_track();

	track_button_update();

	if (cdi.cdi_state & CDROM_STATE_PAUSE) {
		debug_printf(1, "cb_cdrom_rewind: paused on track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	if ((cdi.cdi_state & CDROM_STATE_PLAY) == 0) {
		debug_printf(1, "cb_cdrom_rewind: stopped on track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	debug_printf(1, "cb_cdrom_rewind: playing track %d\n",
		     cdi.cdi_curtrack);

	timer_fsecs = 0;
	cdi.cdi_dur = 0;
	timer_label_update();

	/* restart playing if not paused and currently playing */
	(void) cdrom_play();
}

/*
 * cb_cdrom_ff incrments the current track.  if paused or stopped
 * then return.  otherwise start playing the (new) current track.
 */
/*ARGSUSED*/
void
cb_cdrom_ff(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_ff: error from cdrom_open\n");
		return;
	}

	cdi.cdi_state &= ~CDROM_STATE_EJECTED;

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE) {
		if (cdi.cdi_currand == cdi.cdi_ntracks) {
			debug_printf(1, "cb_cdrom_ff: at last track\n");
			return;
		}
	}
	else {
		if (cdi.cdi_curtrack >= cdi.cdi_maxtrack) {
			debug_printf(1, "cb_cdrom_ff: at last track\n");
			return;
		}
	}

	cdrom_timer_off();

	timer_fsecs = 0;
	cdi.cdi_dur = 0;
	timer_label_update();

	cdi.cdi_curtrack++;
	track_button_update();

	if (cdi.cdi_state & CDROM_STATE_PAUSE) {
		debug_printf(1, "cb_cdrom_ff: paused on track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	if ((cdi.cdi_state & CDROM_STATE_PLAY) == 0) {
		debug_printf(1, "cb_cdrom_ff: stopped on track %d\n",
			    cdi.cdi_curtrack);
		return;
	}

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE)
		cdi.cdi_curtrack = shuffle_next_track();

	debug_printf(1, "cb_cdrom_ff: playing track %d\n",
		    cdi.cdi_curtrack);

	/* restart playing if not paused and currently playing */
	(void) cdrom_play();
}

/*ARGSUSED*/
void
cb_cdrom_eject(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdrom_open() == -1) {
		debug_printf(1, "cb_cdrom_eject: error from cdrom_open\n");
		return;
	}

	/* toggle button weirdness */
	if (cdi.cdi_state & CDROM_STATE_EJECTED) {
		debug_printf(1, "cb_cdrom_eject: already ejected\n");
		return;
	}

	cdrom_timer_off();
	cdrom_stop();
	leds_stop();

	debug_printf(1, "cb_cdrom_eject: ejecting on track %d\n",
		    cdi.cdi_curtrack);

	cdi.cdi_curtrack = 0;

	track_button_update();
	pause_button_reset();

	cdi.cdi_state &= ~(CDROM_STATE_PAUSE | CDROM_STATE_PLAY);
	cdi.cdi_state |= CDROM_STATE_EJECTED;

	cdrom_eject();
	cdrom_close();
}

/*ARGSUSED*/
void
cb_cdrom_cycle(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	if (cdi.cdi_state & CDROM_STATE_CYCLE) {
		debug_printf(1, "cb_cdrom_cycle: cycle off\n");
		cdi.cdi_state &= ~CDROM_STATE_CYCLE;

		return;
	}

	debug_printf(1, "cb_cdrom_cycle: cycle on\n");
	cdi.cdi_state |= CDROM_STATE_CYCLE;
}

/*ARGSUSED*/
void
cb_cdrom_shuffle(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	/*if (cdi.cdi_state & CDROM_STATE_PLAY)
		XBell(XtDisplay(widget), 100);*/

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE) {
		/*if (cdi.cdi_state & CDROM_STATE_PLAY) {
			shuffle_button_set();
			return;
		}*/

		debug_printf(1, "cb_cdrom_shuffle: shuffle off\n");
		cdi.cdi_state &= ~CDROM_STATE_SHUFFLE;

		return;
	}

	if (cdi.cdi_state & CDROM_STATE_PLAY) {
		XBell(XtDisplay(widget), 100);
		shuffle_button_reset();
		return;
	}

	debug_printf(1, "cb_cdrom_shuffle: shuffle on\n");
	cdi.cdi_state |= CDROM_STATE_SHUFFLE;
	shuffle_setup();
}

/*ARGSUSED*/
void
cb_cdrom_quit(widget, client_data, call_data)
	Widget		widget;
	XtPointer	client_data;
	XtPointer	call_data;
{
	debug_printf(1, "cb_cdrom_quit: bye\n");

	cdrom_timer_off();
	cdrom_close();

	exit(0);
}

/*
 * we ignore the relmsf entirely and simply poll the cd-rom drive
 * every TIMER_PERIOD milliseconds to see where it is and if it's
 * on a new track, update the label widget.
 */
void
cdrom_timer_on() {
	static void	update_track();

	if (cdi.cdi_state & CDROM_STATE_PLAY)
		ivid = XtAppAddTimeOut(appc, TIMER_PERIOD, update_track, NULL);
}

/*
 * cdrom_timer_off turns off the X timeout.
 */
static void
cdrom_timer_off() {
	if (ivid != -1) {
		XtRemoveTimeOut(ivid);
		ivid = -1;
	}
}

/*
 * update_track is called when the timeout fires;
 * it updates curtrack and calls the label update
 * routine.
 */
/*ARGSUSED*/
static void
update_track(data, id)
	XtPointer	*data;
	XtIntervalId	*id;
{
	unsigned int	curtrack;

	if ((curtrack = cdrom_get_curtrack()) != cdi.cdi_curtrack) {
		if (curtrack == 0) {
			cdrom_atend();

			return;
		}

		timer_fsecs = 0;
		cdi.cdi_dur = 0;
		timer_label_update();

		cdi.cdi_curtrack = curtrack;
		track_button_update();
	}

	leds_update();

	if ((timer_fsecs++ % timer_mod) == 0) {
		timer_label_update();
		cdi.cdi_dur++;
	}

	ivid = XtAppAddTimeOut(appc, TIMER_PERIOD, update_track, NULL);
}

/*
 * called by update_track when the cd has hit
 * the end of the track or the disc.
 */
static void
cdrom_atend() {
	debug_printf(1, "cdrom_atend: at end\n");

	cdrom_timer_off();
	leds_stop();

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE) {

		if (cdi.cdi_currand == cdi.cdi_ntracks) {
			if ((cdi.cdi_state & CDROM_STATE_CYCLE) == 0) {
				debug_printf(1, "cdrom_atend: shuffle done\n");

				return;
			}

			debug_printf(1, "cdrom_atend: shuffle cycling\n");
			shuffle_setup();
		}

		cdi.cdi_curtrack = shuffle_next_track();
	}
	else if (cdi.cdi_curtrack < cdi.cdi_maxtrack) {
		debug_printf(1, "cdrom_atend: continuing\n");
		cdi.cdi_curtrack++;
	}
	else if (cdi.cdi_state & CDROM_STATE_CYCLE) {
		debug_printf(1, "cdrom_atend: cycling\n");
		cdi.cdi_curtrack = cdi.cdi_mintrack;
	}
	else {
		debug_printf(1, "cdrom_atend: all done\n");
		return;
	}

	timer_fsecs = 0;
	cdi.cdi_dur = 0;
	timer_label_update();

	(void) cdrom_play();
}

static int
cdrom_play() {
	int	ret;

	debug_printf(1, "cdrom_play: starting track %d\n", (int) cdi.cdi_curtrack);

	track_button_update();

	if (cdi.cdi_state & CDROM_STATE_SHUFFLE)
		ret = cdrom_play_track(cdi.cdi_curtrack, cdi.cdi_curtrack);
	else
		ret = cdrom_play_track(cdi.cdi_curtrack, cdi.cdi_maxtrack);

	cdrom_timer_on();

	return(ret);
}
