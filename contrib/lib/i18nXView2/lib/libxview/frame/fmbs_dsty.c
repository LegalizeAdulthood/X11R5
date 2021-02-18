#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fmbs_dsty.c 1.12 90/02/26";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview_private/frame_base.h>

static void     frame_base_free();

/* Destroy the frame struct */
Pkg_private int
frame_base_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_base_info *frame = FRAME_BASE_PRIVATE(frame_public);

    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	frame_base_free(frame);
    }
    return XV_OK;
}

/*
 * free the frame struct and all its resources.
 */
static void
frame_base_free(frame)
    Frame_base_info *frame;
{
    /* Free frame struct */
    free((char *) frame);
}
