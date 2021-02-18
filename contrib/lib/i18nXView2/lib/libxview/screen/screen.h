/*      @(#)screen.h 20.24 90/03/06 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_screen_DEFINED
#define xview_screen_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/generic.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

#ifndef XV_ATTRIBUTES_ONLY

/*
 * PUBLIC #defines 
 */

#define	SCREEN				&xv_screen_pkg

/*
 * PRIVATE #defines 
 */

#define SCREEN_TYPE			ATTR_PKG_SCREEN

#endif ~XV_ATTRIBUTES_ONLY

#define SCREEN_ATTR(type, ordinal)      ATTR(ATTR_PKG_SCREEN, type, ordinal)

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

#ifndef XV_ATTRIBUTES_ONLY

typedef	Xv_opaque 	Xv_Screen;
typedef	Xv_opaque 	Xv_screen;

#endif ~XV_ATTRIBUTES_ONLY

typedef enum {
	/*
	 * Public attributes 
	 */
	SCREEN_NUMBER	  		= SCREEN_ATTR(ATTR_INT,		10),
	SCREEN_SERVER	  		= SCREEN_ATTR(ATTR_OPAQUE,	15),
	/*
	 * Private attributes 
	 */
	SCREEN_STATIC_VISUAL    	= SCREEN_ATTR(ATTR_OPAQUE,	20),
        SCREEN_DYNAMIC_VISUAL   	= SCREEN_ATTR(ATTR_OPAQUE,     	25),
        SCREEN_DEFAULT_CMS      	= SCREEN_ATTR(ATTR_OPAQUE,     	30),
        SCREEN_DEFAULT_COLORMAP      	= SCREEN_ATTR(ATTR_OPAQUE,     	35),
        SCREEN_RETAIN_WINDOWS   	= SCREEN_ATTR(ATTR_BOOLEAN,    	40),
	SCREEN_COLORMAP_LIST		= SCREEN_ATTR(ATTR_OPAQUE,	45),
	SCREEN_BG1_PIXMAP		= SCREEN_ATTR(ATTR_OPAQUE,	50),
	SCREEN_BG2_PIXMAP		= SCREEN_ATTR(ATTR_OPAQUE,	55),
	SCREEN_BG3_PIXMAP		= SCREEN_ATTR(ATTR_OPAQUE,	60),
	SCREEN_GINFO			= SCREEN_ATTR(ATTR_OPAQUE,	65),
} Screen_attr;

#ifndef XV_ATTRIBUTES_ONLY

typedef struct {
    Xv_generic_struct	parent;
    Xv_opaque		private_data;
} Xv_screen_struct;

#endif ~XV_ATTRIBUTES_ONLY

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC Variables 
 */

extern Xv_Screen	xv_default_screen;

/*
 * PRIVATE Variables 
 */

extern Xv_pkg	xv_screen_pkg;

#endif ~xview_screen_DEFINED
