#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_deaf.c 50.5 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * xv_deaf.c
 *
 * int
 * xv_deaf(window, on)
 * Xv_Window	window;
 * Bool		on;
 * 
 * If 'on' is True, makes 'window' and all it's children deaf i.e.
 * ignores these events. See libxvin/win/win_input.c
 * KeyPress
 * KeyRelease
 * ButtonPress
 * ButtonRelease
 * MotionNotify
 *
 * Returns XV_OK if success, XV_ERROR otherwise.
 *
 */
#include <stdio.h>
#include <xview/window.h>
#include <xview/cursor.h>
#include <xview_private/windowimpl.h>
#include <xview_private/draw_impl.h>
#include <X11/Xlib.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

/*
 * xv_deaf(parent, on)
 * makes the object 'parent' and it's descendants deaf if
 * 'on' is True. Otherwise, it restores their original input
 * state.
 */
xv_deaf(parent, on)
Xv_Window	parent;
Bool		on;
{
    Display			*display;
    register Xv_Drawable_info	*info;

    /*
     * Get display from object
     */
    DRAWABLE_INFO_MACRO(parent, info);
    display = xv_display(info);

    if (makeDeaf(display, parent, on) != XV_OK)  {
#ifdef OW_I18N
        xv_error(parent,
                ERROR_STRING, XV_I18N_MSG("xv_messages", 
		
                "xv_deaf:attempt to make windows deaf/hear failed"),
                0);
#else
        xv_error(parent,
                ERROR_STRING, 
                "xv_deaf:attempt to make windows deaf/hear failed", 
                0);
#endif
        return(XV_ERROR);
    }

    return(XV_OK);
}

/*
 * makeDeaf(display, topLevel, on)
 * Makes the object 'topLevel' and it's children deaf/hear, depending
 * on the value of 'on'.
 */
makeDeaf(display, topLevel, on)
Display		*display;
Xv_Window	topLevel;
Bool		on;
{
    register Xv_Drawable_info	*info;
    XWindowAttributes		win_attr;
    Window_info			*win = WIN_PRIVATE(topLevel);
    unsigned long		newIm;
    Drawable			parentXid;

    /*
     * Get Xid of window
     */
    DRAWABLE_INFO_MACRO(topLevel, info);
    parentXid = xv_xid(info);

    /*
     * Return if the window is already in the state we want to set 
     * it to
     */
    if (on)  {
	if (WIN_IS_DEAF(win))  {
	    return(XV_OK);
	}
    }
    else  {
	if (!WIN_IS_DEAF(win))  {
	    return(XV_OK);
	}
    }

    WIN_SET_DEAF(win, on);
    setDeafCursor(topLevel, on);

    /*
     * Do the same for ALL children/descendants.
     */
    if (makeChildrenDeaf(display, topLevel, parentXid, on) != XV_OK)  {
	/*
	 * If unsuccessful, restore all windows that were made deaf
	 */
#ifdef OW_I18N
        xv_error(topLevel,
            ERROR_STRING, XV_I18N_MSG("xv_messages", 
	    
	    "Attempt to make children windows deaf/hear failed"),
            0);
#else
        xv_error(topLevel,
            ERROR_STRING, 
		"Attempt to make children windows deaf/hear failed", 
            0);
#endif OW_I18N

        return(XV_ERROR);
    }

    return(XV_OK);
}

/*
 * makeChildrenDeaf(display, topLevel, topLevelXid, on)
 * Makes the children of object 'topLevel' deaf/hear, depending
 * on the value if 'on'.
 */
makeChildrenDeaf(display, topLevel, topLevelXid, on)
Display		*display;
Xv_Window	topLevel;
Window		topLevelXid;
Bool		on;
{
    Window			root, 
				parent, 
				*children;
    unsigned int		numchildren = 0;
    int				i;

    /*
     * Query for all children of parent window
     */
    if (!(XQueryTree(display, topLevelXid, &root, &parent, &children, &numchildren)))  {
#ifdef OW_I18N
        xv_error(topLevel,
            ERROR_STRING, XV_I18N_MSG("xv_messages", 
	    "Attempt to query the children windows failed"),
            0);
#else
        xv_error(topLevel,
            ERROR_STRING, 
		"Attempt to query the children windows failed", 
            0);
#endif OW_I18N

	return(XV_ERROR);
    }

    if (numchildren == 0)  {
	return(XV_OK);
    }

    /*
     * For each child:
     */
    for (i=0; i<numchildren; ++i)  {
	Xv_Window	win_obj;

	if (win_obj = win_data(display, children[i]))  {
	    Window_info		*win_private = WIN_PRIVATE(win_obj);
	    
            /*
             * Return if the window is already in the state we want to set 
             * it to
             */
            if (on)  {
	        if (!WIN_IS_DEAF(win_private))  {
	            WIN_SET_DEAF(win_private, on);
                    setDeafCursor(win_obj, on);
	        }
            }
            else  {
	        if (WIN_IS_DEAF(win_private))  {
	            WIN_SET_DEAF(win_private, on);
                    setDeafCursor(win_obj, on);
	        }
            }
	}

    }

    /*
     * For each child found, make it's children deaf
     */
    for (i=0; i<numchildren; ++i)  {
        if (makeChildrenDeaf(display, topLevel, children[i], on) != XV_OK)  {
            XFree(children);
	    return(XV_ERROR);
	}
    }

    XFree(children);

    return(XV_OK);
}

setDeafCursor(window, deaf)
Xv_window	window;
Bool		deaf;
{
    Window_info		*win_private = WIN_PRIVATE(window);
    static Xv_cursor	deafPointer = NULL;

    if (!deafPointer) {
        Xv_object   screen, server;
        screen = xv_get(window, XV_SCREEN);
        server = xv_get(screen, SCREEN_SERVER);
        /*
         * Check if busy pointer has been created on server
         * already
         */
        deafPointer = xv_get(server, XV_KEY_DATA, CURSOR_BUSY_PTR);
        /*
         * If it hasn't, create and store it on server as key data
         */
        if (!deafPointer) {
            deafPointer = xv_create(screen, CURSOR, CURSOR_SRC_CHAR, 
                                OLC_BUSY_PTR, 
                                CURSOR_MASK_CHAR, 0,
                                0);
            xv_set(server, XV_KEY_DATA, CURSOR_BUSY_PTR, deafPointer, 0);
        }
    }

    if (deaf)  {
        win_private->normal_cursor = win_private->cursor;
        xv_set(window, WIN_CURSOR, deafPointer, NULL);
    }
    else  {
	if (win_private->normal_cursor)  {
            xv_set(window, WIN_CURSOR, win_private->normal_cursor, NULL);
	}
    }
}
