#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_destroy.c 1.31 90/04/27";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_destroy.c
 * 
 * Description:
 * 
 * cleanups scrollbar as it is destroyed
 * 
 */

/*
 * Include files:
 */
#include <xview_private/sb_impl.h>
#include <xview/screen.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Pkg_private int scrollbar_destroy_internal();

/******************************************************************/

Pkg_private int
scrollbar_destroy_internal(scroll_public, status)
    Scrollbar       scroll_public;
    Destroy_status  status;
{
	Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(scroll_public);
	
	if ((status == DESTROY_CLEANUP) || (status == DESTROY_PROCESS_DEATH)) {
		/* clean up menu here */
		xv_destroy(sb->menu);
		
		if (status == DESTROY_CLEANUP)
		  free((char *) sb);
	}
	return XV_OK;
}


	
