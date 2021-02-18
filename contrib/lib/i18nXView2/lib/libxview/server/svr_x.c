#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)svr_x.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/pkg.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef _XV_DEBUG
#include <xview_private/xv_debug.h>
#endif
#include <xview/win_event.h>
#include <X11/Xlib.h>
#include <xview/sel_svc.h>
#include <xview/server.h>
#include <xview_private/svr_impl.h>
#include <X11/keysym.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

extern Display *XOpenDisplay();

#ifndef SVR4
/*
 * The following table describes the default key mappings for special 
 * XView keys. The first default key mapping is attempted. If this 
 * fails, then the machine we're on probably doesn't
 * have sufficient function keys, so we try the second mapping, and
 * so on. Right now, the rest of XView only supports the default mapping
 * so we set NUM_KEYSYM_SETS to 1 and only supply one set of keys to use.
 * In the future, this will go away and we'll provide a more elegant
 * and flexible way for the user to map function keys to XView functions.
 */

#define NUM_KEYSYM_SETS	1
static KeySym	default_fkey_keysyms[NUM_KEYSYM_SETS][SELN_FN_NUM] = {
		{			/* default keysyms */
			XK_F13,
			XK_F16,
			XK_F18,
			XK_F19,
			XK_F20
		}
};

#define MAX_RETRIES	10	/* max number of mapping retries */

#else SVR4
KeyCode         selnfn_to_keycode[SELN_FN_NUM] = {32, 58, 80, 102, 104};
KeySym          selnfn_to_keysym[SELN_FN_NUM] = {XK_F13, XK_F16, XK_F18, XK_F19, XK_F20};
#endif SVR4


static int
my_sync(display)
    Display        *display;
{
    XSync(display, 0);
}

Pkg_private     Xv_opaque
server_init_x(server_name)
    char           *server_name;
{
    register Display *display;

    if (!(display = XOpenDisplay(server_name)))
    	return ((Xv_opaque) NULL);

    if (defaults_get_boolean("window.synchronous", "Window.Synchronous", FALSE)
		 			        && !XSynchronize(display, TRUE))
	(void) XSetAfterFunction(display, my_sync);

    return ((Xv_opaque) display);
}

#ifndef SVR4

/*
 * keycode_in_map(map, keycode)
 *
 * returns the associated modifier index if the specified keycode is in the 
 * given modifier map. Otherwise, returns -1.
 */

static int
keycode_in_map(map, keycode)
	XModifierKeymap *map;
	KeyCode keycode;
{
	register int i, max;

	if (!keycode) return(0);

	max = 8 * map->max_keypermod;
	for (i = 0; i < max; i++) {
		if (map->modifiermap[i] == keycode) {
			return (i / map->max_keypermod);
		}
	}
	return -1;
}

static int
find_free_row(map)
	XModifierKeymap *map;
{
	int row, offset, base;

	/*
	 * Find the first unused row in the modifier map.
	 * An unused row will have all zeros in it.
	 */
	for (row = Mod1MapIndex; row <= Mod5MapIndex; row++) {
		base = row * map->max_keypermod;
		for (offset = 0; (offset < map->max_keypermod)  &&
			(map->modifiermap[base + offset] == 0); 
			offset++);
		if (offset == map->max_keypermod) {
			return(row);
		}
	}
	return(-1);
}
#endif SVR4

/*
 * server_init_modifier(display)
 *
 * 1) Designates the meta keys as a modifier keys.
 * 2) Inserts all the keys in the array default_fkey_keysyms[] into
 * 	the server's modifier map (all under the same modifier; any
 *	of the modifiers Mod2-Mod5 may be used). This function then
 *	sets SELN_FNMASK to be the appropriate mask for whatever modifier
 *	the keys were designated as.
 */
Pkg_private void
server_init_modifier(server)
	Server_info	*server;
{
	Display	*display = (Display *)server->xdisplay;
#ifndef SVR4
	XModifierKeymap *map;
	int             i, meta_modifier, func_modifier, updated = False;
	int		keysym_set, result, retry_count;

	for (keysym_set = 0; keysym_set < NUM_KEYSYM_SETS; keysym_set++) {
		if (!(map = XGetModifierMapping(display))) {
			return;
		}
				         /* See if META is already installed. */
		if ((meta_modifier = keycode_in_map(map,
				 XKeysymToKeycode(display, XK_Meta_L))) == -1) {
					          /* Find a free row for META */
			if ((meta_modifier = find_free_row(map)) != -1) {
				updated = True;
				        /* Insert the meta keys as modifiers. */
				map = XInsertModifiermapEntry(map,
						XKeysymToKeycode(display,
							    (KeySym) XK_Meta_L),
							         meta_modifier);
				map = XInsertModifiermapEntry(map,
						XKeysymToKeycode(display,
							    (KeySym) XK_Meta_R),
								 meta_modifier);
			}
		}

		if (meta_modifier == -1 || meta_modifier == 0)
			server->meta_modmask = 0;
		else
			server->meta_modmask = 1<<meta_modifier;

					       /* See if function keys in map */
		if ((func_modifier= keycode_in_map(map,XKeysymToKeycode(display,
				 default_fkey_keysyms[keysym_set][0]))) == -1) {
					                  /* Find a free row. */
			if ((func_modifier = find_free_row(map)) != -1) {
			    for (i = 0; i < SELN_FN_NUM; i++) {
				updated = True;
				map = XInsertModifiermapEntry(map,
					XKeysymToKeycode(display,
					   default_fkey_keysyms[keysym_set][i]),
								 func_modifier);
			    }
			    SELN_FNMASK = 1 << func_modifier;
			}
		} else
  			SELN_FNMASK = 1 << func_modifier;

		if (func_modifier == -1 || func_modifier == 0)/* no free rows */
			SELN_FNMASK = 0;

		/*
		 * Attempt to install the modifier mapping.
		 * If successful, exit this function. If not, try another 
		 * set of keysyms.
		 */
		if (updated) {
		    for (retry_count = 0;
			((result = XSetModifierMapping(display, map)) 
				== MappingBusy && retry_count < MAX_RETRIES);
			retry_count++) {
				sleep(1);/* if busy, wait 1 sec and retry */
		    }
  	  	    if (result == Success) {
    			XFreeModifiermap(map);
			return;
		    }
		} else {
    		    XFreeModifiermap(map);
		    return;
		}
	}

	/* all our attempts failed */
	xv_error(NULL,
		 ERROR_STRING,
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages", "Problems setting default modifier mapping"),
#else
		 "Problems setting default modifier mapping",
#endif
		 ERROR_PKG, SERVER,
		 0);

    	XFreeModifiermap(map);
#else SVR4
    XModifierKeymap *map = XGetModifierMapping(display);
    register int    i, j, row;
    XModifierKeymap *newmap;
    int             index;
    int             t;
		     
    for (i = 0; i < SELN_FN_NUM; i++)
	selnfn_to_keycode[i] = XKeysymToKeycode (display, selnfn_to_keysym[i]);
				  
	/* If the existing map is not long enough, strech it */
	if (map->max_keypermod < SELN_FN_NUM) {
	    int             lastrow, newrow;
		
	    newmap = XNewModifiermap(SELN_FN_NUM);
	    newrow = row = 0;
	    lastrow = map->max_keypermod * 8;
	    while (row < lastrow) {
		for (i = 0; i < map->max_keypermod; i++)
		    newmap->modifiermap[newrow + i] = map->modifiermap[row + i];
		for (; i < newmap->max_keypermod; i++)
		    newmap->modifiermap[newrow + i] = 0;
		row += map->max_keypermod;
		newrow += newmap->max_keypermod;
	}
	XFreeModifiermap(map);
	map = newmap;
    }
    map = XInsertModifiermapEntry(map,
			    XKeysymToKeycode(display, XK_Meta_L), Mod1MapIndex);
    map = XInsertModifiermapEntry(map,
			    XKeysymToKeycode(display, XK_Meta_R), Mod1MapIndex);
    /*
     * BUG ALERT: We might need to combine this XSetModifierMapping() call
     * with the one below
     */
     if (XSetModifierMapping(display, map) != Success)
         xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		  XV_ERROR_STRING,
#ifdef OW_I18N
		  XV_I18N_MSG("xv_messages", "Error from call to XSetModifierMapping"),
#else
		  "Error from call to XSetModifierMapping",
#endif
		  XV_ERROR_PKG, ATTR_PKG_SERVER,
		  0);

     /* Find an index not already used */
     for (i = Mod1MapIndex; i <= Mod5MapIndex; i++) {
	row = i * map->max_keypermod;
	for (j = 0; j < map->max_keypermod; j++) {
	     /* If seln_function modifier already exists */
	     if (map->modifiermap[row + j] == selnfn_to_keycode[j]) {
		 index = i;
		 goto Done;
	     } else if (map->modifiermap[row + j] != 0)
{
	         break;
}
	}
	if (j >= map->max_keypermod)
{
	break;
}
	}

	if (i <= Mod5MapIndex) { index = i;
	    row = i * map->max_keypermod;
#ifndef vax
	/* 
	 * BUG ALERT:  This needs to be rewritten so that it can be portable
	 * to non-Sun systems.
	 */
	for (i = 0; i < SELN_FN_NUM; i++)
	    map->modifiermap[row + i] = selnfn_to_keycode[i];
#endif                          /* vax */
	if (t=XSetModifierMapping(display, map) != Success)
{
	    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		     XV_ERROR_STRING,
#ifdef OW_I18N
		     XV_I18N_MSG("xv_messages", "Error from call to XSetModifierMapping"),
#else
		     "Error from call to XSetModifierMapping",
#endif
		     XV_ERROR_PKG, ATTR_PKG_SERVER,
		     0);
}
    } else
	xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages", "No unused modifier index found"),
#else
		 "No unused modifier index found",
#endif
		 XV_ERROR_PKG, ATTR_PKG_SERVER,
		 0);

Done:
    SELN_FNMASK = 1 << index;
    XFreeModifiermap(map);

#endif SVR4
}


Xv_private void
server_set_seln_function_pending(server_public, flag)
    Xv_Server       server_public;
    int             flag;
{
    Server_info    *server = SERVER_PRIVATE(server_public);
    server->seln_function_pending = flag ? TRUE : FALSE;
}

Xv_private int
server_get_seln_function_pending(server_public)
    Xv_Server       server_public;
{
    return (SERVER_PRIVATE(server_public)->seln_function_pending);
}
