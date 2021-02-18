/*	@(#)scrn_impl.h 50.3 90/10/16 SMI	*/

/****************************************************************************/
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/****************************************************************************/

#ifndef _screen_impl_h_already_included
#define _screen_impl_h_already_included

#include <xview/pkg.h>
#include <xview/screen.h>
#include <xview_private/scrn_vis.h>
#include <xview/xv_xrect.h>
#include <xview/window.h>
#include <xview/cms.h>

#ifdef OW_I18N
#include <X11/Xresource.h>
#endif

typedef struct scrn_cache_window {
    Xv_window			window;
    short			busy;
    short			borders;
    int				visual_class;
    struct scrn_cache_window	*next;
} Xv_cached_window;

typedef struct {
    Xv_Screen		public_self;	/* back pointer */
    int			number;		/* screen number on server */
    Xv_opaque		server;		/* always a Server */
    Xv_opaque		root_window;	/* always a Window */
    Screen_visual	*first_visual;	/* list of visual/depth */
    Xv_xrectlist        clip_xrects;    /* clipping rectangle list */
    Xv_cached_window    *cached_windows;
    short               retain_windows; /* retain leaf windows for perf ? */
    Visual              *static_visual;
    Visual              *dynamic_visual;
    Cms			default_cms;
    Xv_opaque		colormap_list;
} Screen_info;

#define	SCREEN_PRIVATE(screen)	\
	XV_PRIVATE(Screen_info, Xv_screen_struct, screen)
#define	SCREEN_PUBLIC(screen)	XV_PUBLIC(screen)

/* screen_get.c */
Pkg_private Xv_opaque	screen_get_attr();

/* screen.c */
Pkg_private Xv_opaque	screen_set_avlist();

/* screen_layout.c */
Pkg_private int			screen_layout();

#endif
