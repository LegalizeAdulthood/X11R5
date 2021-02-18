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

# define ran(min, max)  ((random() % ((max) - (min))) + (min))

static unsigned char	*random_tracks;

void
shuffle_setup() {
	extern char	*malloc();
	extern long	time();
	extern long	random();
	unsigned long	seed, now;
	char		state[128];
	int		try;
	int		i, j;

	if (cdi.cdi_maxtrack == cdi.cdi_mintrack) {
		cdi.cdi_currand = -1;
		return;
	}

	cdi.cdi_ntracks = (cdi.cdi_maxtrack - cdi.cdi_mintrack) + 1;

	cdi.cdi_currand = 0;

	if (random_tracks != NULL) {
		free(random_tracks);
		random_tracks = NULL;
	}

	if ((random_tracks = (unsigned char *) malloc(cdi.cdi_ntracks)) == NULL) {
		perror("malloc");
		exit(1);
	}

	now = time((long *) 0);
	seed = now & 0xfff;

	initstate(seed, state, sizeof(state));

	/*
	 * set up the random_tracks array
	 */
	for (i = 0; i < cdi.cdi_ntracks; i++) {
		for (;;) {
			try = ran(cdi.cdi_mintrack, cdi.cdi_maxtrack+1);

			if (i == 0)
				break;

			for (j = 0; j < i; j++) {
				if (random_tracks[j] == try)
					goto again;
			}

			/* not a repeat */
			break;

			again:;
		}

		random_tracks[i] = try;
	}

	if (debug == True) {
		debug_printf(1, "shuffle_setup: ");
		for (i = 0; i < cdi.cdi_ntracks; i++)
			debug_printf(1, "%d ", random_tracks[i]);
		debug_printf(1, "\n");
	}
}

unsigned char
shuffle_next_track() {
	if (cdi.cdi_currand == -1)
		return(cdi.cdi_mintrack);

	if (cdi.cdi_currand == cdi.cdi_ntracks) {
		fprintf(stderr, "shuffle_get_track: ran off end\n");
		return(cdi.cdi_mintrack);
	}

	return(random_tracks[cdi.cdi_currand++]);
}

unsigned char
shuffle_prev_track() {
	if (cdi.cdi_currand == -1)
		return(cdi.cdi_mintrack);

	cdi.cdi_currand -= 2;

	if (cdi.cdi_currand < 0)
		cdi.cdi_currand = 0;

	return(random_tracks[cdi.cdi_currand++]);
}
