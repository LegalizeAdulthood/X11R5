/*      @(#)omi_impl.h 50.4 90/10/16 SMI      */

/***********************************************************************/
/*	                omi_impl.h			      	       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef _xview_image_impl_h_already_included
#define _xview_image_impl_h_already_included

#ifndef pixrect_DEFINED
#ifndef _TYPES_
#include <sys/types.h>
#endif _TYPES_
#include <pixrect/pixrect.h>
#endif pixrect_DEFINED
#include <xview/font.h>
#include <xview/svrimage.h>

#ifdef	OW_I18N
#include <xview/xv_i18n.h>
#endif

#ifndef TRUE
#define	TRUE	1
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef NULL
#define NULL	0
#endif


/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

struct image {

    int			*ops; /* Should be ops vector or unique id */
    Font		 font;
    Font		 bold_font;	/* bold version of "font" */
#ifdef	OW_I18N
    char		*string_mbs;
#endif
    CHAR		*string;
    Server_image	 svr_im;
    struct pr_pos	 left_pos;	/* position of left image
					 * (usually pushpin) */
    short		 left_margin;
    short		 right_margin;
    short		 margin;
	
/* Flags */
    unsigned		 inactive:1;
    unsigned		 invert:1;
    unsigned		 title:1;
    unsigned		 free_image:1;
    unsigned		 free_string:1;
    unsigned		 free_svr_im:1;

/* Auxiliary fields */
    short		 width;
    short		 height;
	struct pr_pos	button_pos;		/* menu button position */
	struct pr_size	button_size;	/* menu button size */
};


#define image_vector(x1,y1,x2,y2) \
	(xv_vector(window,x1,y1,x2,y2,PIX_SET,0))
#endif _xview_image_impl_h_already_included
