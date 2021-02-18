/*	@(#)icon_impl.h 50.4 90/10/16 SMI	*/

/*****************************************************************************/
/*                                 icon_impl.h                               */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/*****************************************************************************/

#ifndef _xview_icon_impl_h_already_included
#define _xview_icon_impl_h_already_included

#ifdef OW_I18N
#include <xview/xv_i18n.h>
#endif
#include <xview/icon.h>
#include <xview/pkg.h>
#include <xview/attrol.h>

#ifdef OW_I18N
#include <X11/Xresource.h>
#endif

/*****************************************************************************/
/* icon struct                                                               */
/*****************************************************************************/

typedef struct {
	Icon		public_self;	/* Back pointer */
/*
	short	 	ic_width, ic_height;	
*/
	struct pixrect *ic_background;	/* background pattern (mem pixrect) */
	Rect		ic_gfxrect;	/* where the graphic goes */
	struct pixrect *ic_mpr;		/* the graphic (a memory pixrect) */
	Rect		ic_textrect;	/* where text goes */
#ifdef OW_I18N
	wchar_t        *ic_text_wcs;	/* primary text data */
#endif
	char	       *ic_text;	/* the text */
	int		ic_flags;
	Xv_opaque	frame;		/* frame Icon is assoc w/ */
	Server_image    ic_mask;        /* graphic mask (pixmap) */
} Xv_icon_info;

/* flag values */
#define	ICON_BKGRDPAT	 0x02		/* use ic_background to prepare image*/
#define	ICON_BKGRDGRY	 0x04		/* use std gray to prepare image*/
#define	ICON_BKGRDCLR	 0x08		/* clear to prepare image*/
#define	ICON_BKGRDSET	 0x10		/* set to prepare image*/
#define ICON_PAINTED	 0x20		/* icon window has been painted */
#define ICON_BKGDTRANS   0x40            /* transparent window */
#define ICON_TRANSLABEL  0x80            /* transparent labels */
#define	ICON_FIRSTPRIV	 0x0100		/* start of private flags range */
#define	ICON_LASTPRIV	 0x8000		/* end of private flags range */

/*****************************************************************************/
/* typedefs                                                                  */
/*****************************************************************************/

typedef Xv_icon_info *icon_handle;

/*	Other Macros 	*/
#define ICON_PRIVATE(icon) \
	XV_PRIVATE(Xv_icon_info, Xv_icon, icon)
#define ICON_PUBLIC(icon)	XV_PUBLIC(icon)

/* from icon_object.c */
Pkg_private int 	icon_init();
Pkg_private Xv_opaque icon_set_internal();
Pkg_private Xv_opaque icon_get_internal();
Pkg_private int          icon_destroy_internal();


#endif _xview_icon_impl_h_already_included

