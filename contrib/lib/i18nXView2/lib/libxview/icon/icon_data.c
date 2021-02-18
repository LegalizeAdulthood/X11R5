#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon_data.c 1.12 90/04/10";
#endif
#endif
/*****************************************************************************/
/*
 * icon_object.c
 */
/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
/*****************************************************************************/

#include <stdio.h>

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED
/* <pixrect/pixrect_hs.h> without frame buffer variable include files */
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <rasterfile.h>
#include <pixrect/pr_io.h>
#endif	pixrect_hs_DEFINED

#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview_private/icon_impl.h>

Xv_pkg          xv_icon_pkg = {
    "Icon",			/* seal -> package name */
    (Attr_pkg) ATTR_PKG_ICON,	/* icon attr */
    sizeof(Xv_icon),		/* size of the icon data struct */
    &xv_window_pkg,		/* pointer to parent */
    icon_init,			/* init routine for icon */
    icon_set_internal,		/* set routine */
    icon_get_internal,		/* get routine */
    icon_destroy_internal,	/* destroy routine */
    NULL
};
