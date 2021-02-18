/*	@(#)svr_impl.h 70.2 91/07/25 SMI	*/

/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */

#ifndef _server_impl_h_already_included
#define _server_impl_h_already_included

#include <sys/types.h>
#include <xview_private/xv_list.h>
#include <xview/server.h>
#include <xview/screen.h>
#include <xview/sel_svc.h>
#include <xview_private/sel_impl.h>
#include <xview_private/svr_atom.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#include <X11/Xresource.h>
#endif

/* maximum # of screens per server (arbitrary) */
#define	MAX_SCREENS		10
#define BITS_PER_BYTE		8

typedef struct {
    Xv_sl_link		next;
    Xv_Server		public_self;	 /* Back pointer */
    Xv_Screen		screens[MAX_SCREENS];
    Xv_opaque		xdisplay;
    unsigned int	*xv_map;
    unsigned char	*sem_map;
    unsigned char	*ascii_map;
    Xv_opaque		xtime;           /* Last time stamp */
    Xv_opaque		atom[MAX_NUM_ATOMS];
    int			seln_function_pending;
    unsigned		journalling;
    short		in_fullscreen;
    Xv_opaque		top_level_win;
    XID			atom_context;
    short		nbuttons;        /* Number of physical mouse buttons */
    unsigned int	but_two_mod;     /* But 1 + this modifier == but 2 */
    unsigned int	but_three_mod;   /* But 1 + this modifier == but 3 */
					 /* Above only valid if nbuttons < 3 */
    void		(*extensionProc)(); 

#ifdef OW_I18N
    XrmDatabase		db;             
    XIM   	        xim;
    char                *basiclocale;
    char                *displaylang;
    char                *inputlang;
    char                *numeric;
    char                *timeformat;
    char		*localedir;
#endif

    char		*display_name;
				         /* Represents the modifier slot the  */
					 /* META keys are in, Mod1MapIndex -> */
					 /* Mod5MapIndex.		      */
    int			meta_modmask;
} Server_info;

#define	SERVER_PRIVATE(server)	XV_PRIVATE(Server_info, Xv_server_struct, server)
#define	SERVER_PUBLIC(server)	XV_PUBLIC(server)

#define         SELN_FN_NUM     5
int             SELN_FNMASK;

Pkg_private Xv_opaque	server_init_x();

/* server_get.c */
Pkg_private int		server_init();
Pkg_private int		server_destroy();

/* server_get.c */
Pkg_private Xv_opaque	server_get_attr();

/* server_set.c */
Pkg_private Xv_opaque	server_set_avlist();

#endif
