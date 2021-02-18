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
#include "object.h"
#include "paintop.h"
#include "u_create.h"
#include "u_elastic.h"
#include "w_canvas.h"
#include "mode.h"
#include <stdio.h>

extern char    *malloc();

read_epsf(eps)
    F_eps          *eps;
{
    int             nbitmap;
    int             bitmapz;
    char           *cp;
    unsigned char  *mp;
    int             n;
    int             flag;
    char            buf[300];
    int             llx, lly, urx, ury;
    FILE           *epsf;
    register unsigned char *last;

    epsf = fopen(eps->file, "r");
    if (epsf == NULL) {
	put_msg("Cannot open file: %s", eps->file);
	return 0;
    }
    while (fgets(buf, 300, epsf) != NULL) {
	lower(buf);
	if (!strncmp(buf, "%%boundingbox", 13)) {
	    if (sscanf(buf, "%%%%boundingbox: %d %d %d %d",
		       &llx, &lly, &urx, &ury) < 4) {
		put_msg("Bad EPS bitmap file: %s", eps->file);
		fclose(epsf);
		return 0;
	    }
	    break;
	}
    }

    eps->hw_ratio = (float) (ury - lly) / (float) (urx - llx);

    eps->bitmap = NULL;
    eps->bit_size.x = 0;
    eps->bit_size.y = 0;

    eps->pixmap = 0;
    eps->pix_flipped = 0;
    eps->pix_rotation = 0;
    eps->pix_width = 0;
    eps->pix_height = 0;

    if (ury - lly <= 0 || urx - llx <= 0) {
	put_msg("Bad values in EPS bitmap bounding box");
    }
    bitmapz = 0;

    /* look for a preview bitmap */
    while (fgets(buf, 300, epsf) != NULL) {
	lower(buf);
	if (!strncmp(buf, "%%beginpreview", 14)) {
	    sscanf(buf, "%%%%beginpreview: %d %d %d",
		   &eps->bit_size.x, &eps->bit_size.y, &bitmapz);
	    break;
	}
    }

    if (eps->bit_size.x > 0 && eps->bit_size.y > 0 && bitmapz == 1) {
	nbitmap = (eps->bit_size.x + 7) / 8 * eps->bit_size.y;
	eps->bitmap = (unsigned char *) malloc(nbitmap);
	if (eps->bitmap == NULL)
	    fprintf(stderr, "could not allocate %d bytes of memory\n", nbitmap);
    }
    /* read for a preview bitmap */
    if (eps->bitmap != NULL) {
	mp = eps->bitmap;
	bzero(mp, nbitmap);	/* init bitmap to zero */
	last = eps->bitmap + nbitmap;
	flag = True;
	while (fgets(buf, 300, epsf) != NULL && mp < last) {
	    lower(buf);
	    if (!strncmp(buf, "%%endpreview", 12) ||
		!strncmp(buf, "%%endimage", 10))
		break;
	    cp = buf;
	    if (*cp != '%')
		break;
	    cp++;
	    while (*cp != NULL) {
		if (isxdigit(*cp)) {
		    n = hex(*cp);
		    if (flag) {
			flag = False;
			*mp = n << 4;
		    } else {
			flag = True;
			*mp = *mp + n;
			mp++;
			if (mp >= last)
			    break;
		    }
		}
		cp++;
	    }
	}
    }
    put_msg("EPS bitmap file read OK");
    fclose(epsf);
    return 1;
}

int
hex(c)
    char            c;
{
    if (isdigit(c))
	return (c - 48);
    else
	return (c - 87);
}

lower(buf)
    char           *buf;
{
    while (*buf) {
	if (isupper(*buf))
	    *buf = (char) tolower(*buf);
	buf++;
    }
}
