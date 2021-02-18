#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)win_compat.c% 50.5 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Win_compat.c: SunView 1.X compatibility routines.
 */

#include <xview/pkg.h>
#include <xview/window.h>
#include <xview/win_input.h>
#include <xview/fullscreen.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

/* ARGSUSED */
void
win_getinputmask(window, im, nextwindownumber)
    Xv_object       window;
    Inputmask      *im;
    Xv_opaque      *nextwindownumber;
{
    *im = *((Inputmask *) xv_get(window, WIN_INPUT_MASK));
}

/* ARGSUSED */
win_setinputmask(window, im, im_flush, nextwindownumber)
    Xv_object       window;
    Xv_opaque       nextwindownumber;
    Inputmask      *im, *im_flush;
{

    if (xv_get(window, WIN_IS_IN_FULLSCREEN_MODE)) {
#ifdef OW_I18N
	fprintf(stderr,
	XV_I18N_MSG("xv_messages", " Attempting to set the input mask of a window in fullscreen mode!\n"));
#else
	fprintf(stderr,
		" Attempting to set the input mask of a window in fullscreen mode!\n");
#endif
	abort();
    }
    xv_set(window, WIN_INPUT_MASK, im, 0);
}


coord
win_getheight(window)
    Xv_object       window;
{

    return ((int) window_get(window, WIN_GET_HEIGHT));
}

coord
win_getwidth(window)
    Xv_object       window;
{

    return ((int) window_get(window, WIN_GET_WIDTH));
}
