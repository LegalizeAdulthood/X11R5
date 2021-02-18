#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)pw_read.c 50.5 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * pw_read.c: Implements the read function of pixwin interface.
 */

#include <xview_private/pw_impl.h>
#include <pixrect/memvar.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

Xv_public int
xv_read(pr, x, y, width, height, op, window, sx, sy)
    Pixrect        *pr;
    int             op, x, y, width, height;
    Xv_opaque       window;
    int             sx, sy;
{
    if (PR_IS_MPR(pr)) {
	Xv_Drawable_info *info;

	DRAWABLE_INFO_MACRO(window, info);
	xv_read_internal(pr, x, y, width, height, op, xv_display(info),
			 xv_xid(info), sx, sy);
    } else if (PR_IS_SERVER_IMAGE(pr)) {
	xv_rop(pr, x, y, width, height, op, window, sx, sy);
    } else {
	xv_error(NULL,
		 ERROR_STRING,
#ifdef OW_I18N
		     XV_I18N_MSG("xv_messages", "xv_read: attempting to read into an invalid object"),
#else
		     "xv_read: attempting to read into an invalid object",
#endif
		 0);
    }
}

Xv_public int
xv_read_internal(pr, x, y, width, height, op, display, d, sx, sy)
    Pixrect        *pr;
    int             op, x, y, width, height;
    Display        *display;
    Drawable        d;
    int             sx, sy;
{
    register XImage *image;
    struct mpr_data image_mpr_data;
    struct pixrect  image_mpr;

    image = XGetImage(display, d, sx, sy, width, height,
		      AllPlanes, pr->pr_depth == 1 ? XYPixmap : ZPixmap);
    if (image->depth > pr->pr_depth) {
	xv_error(pr,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_STRING,
#ifdef OW_I18N
		     XV_I18N_MSG("xv_messages", "xv_read_internal(): image depth > pixrect depth"),
#else
		     "xv_read_internal(): image depth > pixrect depth",
#endif
		 0);
    }
    /* Fabricate a memory pixrect from the Xlib image */
    image_mpr.pr_ops = &mem_ops;
    image_mpr.pr_depth = image->depth;
    image_mpr.pr_height = image->height;
    image_mpr.pr_width = image->width;
    image_mpr.pr_data = (caddr_t) (&image_mpr_data);
    image_mpr_data.md_linebytes = image->bytes_per_line;
    image_mpr_data.md_image = (short *) image->data;
    image_mpr_data.md_offset.x = 0;
    image_mpr_data.md_offset.y = 0;
    image_mpr_data.md_primary = 0;
    image_mpr_data.md_flags = 0;
    /* Move the image from the memory pixrect into the destination pr */
    pr_rop(pr, x, y, width, height, op, &image_mpr, 0, 0);
    image->f.destroy_image(image);
}
