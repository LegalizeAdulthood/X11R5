#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)mem_ops.c 50.5 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <xview/xv_error.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

extern int      xv_mem_destroy();
static int      xv_pr_error();

/*
 * "Pixrect" operations vector
 */
struct pixrectops mem_ops = {
    xv_pr_error,		/* mem_rop, */
    xv_pr_error,		/* mem_stencil, */
    xv_pr_error,		/* mem_batchrop, */
    0,
    xv_mem_destroy,
    xv_pr_error,		/* mem_get, */
    xv_pr_error,		/* mem_put, */
    xv_pr_error,		/* mem_vector, */
    (Pixrect * (*) ()) xv_pr_error,	/* mem_region, */
    xv_pr_error,		/* mem_putcolormap, */
    xv_pr_error,		/* mem_getcolormap, */
    xv_pr_error,		/* mem_putattributes, */
    xv_pr_error,		/* mem_getattributes */
};


static int
xv_pr_error(pr)
    Pixrect        *pr;
{
    xv_error(pr,
	     ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
	     ERROR_STRING, 
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages", "Unsupported pixrect operation attempted"),
#else
		 "Unsupported pixrect operation attempted", 
#endif
	     0);
    /* doesn't return */
}
