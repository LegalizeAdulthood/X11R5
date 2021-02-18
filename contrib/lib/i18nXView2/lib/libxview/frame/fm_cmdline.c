#if defined(sccs) && !defined(lint)
static char     sccsid[] = "@(#)fm_cmdline.c 50.4 90/11/04";
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <ctype.h>
#include <pixrect/pixrect.h>
#include <xview/cms.h>
#include <xview/rect.h>
#include <xview/screen.h>
#include <xview/server.h>
#include <xview/frame.h>
#include <xview/pkg.h>
#include <xview/attrol.h>
#include <xview/icon.h>
#include <xview/defaults.h>
#include <xview/win_screen.h>
#include <xview_private/draw_impl.h>
#include <xview_private/fm_impl.h>
#include <X11/Xutil.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

extern Pixrect *icon_load_mpr();
static int frame_parse_color();

/*
 * Convert command line frame defaults into attributes. Apply the attributes
 * to the frame with xv_set_avlist().
 */
Pkg_private int
frame_set_cmdline_options(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Frame_class_info *frame = FRAME_PRIVATE(frame_public);
    Xv_opaque       defaults_array[ATTR_STANDARD_SIZE];
    register Attr_avlist defaults = defaults_array;
    struct xv_singlecolor foreground_color, background_color;
    int             status;
    int             rgb[3];
    char           *string;
    char           *defaults_string[1];
    int            default_width = -1, default_height = -1;
    int		   value;

    /* No string malloc's done yet */
    defaults_string[0] = NULL;

    /* Parse frame command line options */
    if (defaults_exists("window.header", "Window.Header")) {
	*defaults++ = (Xv_opaque) FRAME_LABEL;
	string = defaults_get_string("window.header", "Window.Header", "");
	defaults_string[0] = malloc(strlen(string) + 1);
	strcpy(defaults_string[0], string);
	*defaults++ = (Xv_opaque) defaults_string[0];
    }
    if (defaults_exists("window.iconic", "Window.Iconic")) {
	*defaults++ = (Xv_opaque) FRAME_CLOSED;
	*defaults++ = (Xv_opaque)
	    defaults_get_boolean("window.iconic", "Window.Iconic", FALSE);
    }
    if (defaults_exists("window.color.foreground", "Window.Color.Foreground")) {
	string = defaults_get_string("window.color.foreground",
				     "Window.Color.Foreground", "0 0 0");
	if (frame_parse_color(frame_public, string, &foreground_color)) {
	    *defaults++ = (Xv_opaque) FRAME_FOREGROUND_COLOR;
	    *defaults++ = (Xv_opaque) &foreground_color;
	}
    }

    if (defaults_exists("window.color.background", "Window.Color.Background")) {
	string = defaults_get_string("window.color.background",
				     "Window.Color.Background", "0 0 0");
	if (frame_parse_color(frame_public, string, &background_color)) {
	    *defaults++ = (Xv_opaque) FRAME_BACKGROUND_COLOR;
	    *defaults++ = (Xv_opaque) &background_color;
	}
    }

    /* Set the size if the user asked for it */
    if (frame->geometry_flags & WidthValue) {
	*defaults++ = (Xv_opaque) XV_WIDTH;
	*defaults++ = (Xv_opaque) frame->user_rect.r_width;
    }
    
    if (frame->geometry_flags & HeightValue) {
	*defaults++ = (Xv_opaque) XV_HEIGHT;
	*defaults++ = (Xv_opaque) frame->user_rect.r_height;
    }
    
    /* Set the position if the user asked for it */
    if (frame->geometry_flags & (XValue | YValue)) {
	int x = frame->user_rect.r_left;
	int y = frame->user_rect.r_top;
	int width, height;
	int screen;
	Xv_Drawable_info *info;
	
	DRAWABLE_INFO_MACRO(frame_public, info);
	screen = (int) xv_get(xv_screen(info), SCREEN_NUMBER, 0);
	
	if (frame->geometry_flags & XNegative) {
	    width = (frame->geometry_flags & WidthValue) ? 
	                frame->user_rect.r_width : (int) xv_get(frame_public, XV_WIDTH, 0);
	    x += DisplayWidth(xv_display(info), screen) - width - (2 * FRAME_BORDER_WIDTH);
	}
	if (frame->geometry_flags & YNegative) {
	    height = (frame->geometry_flags & HeightValue) ?
	               frame->user_rect.r_height : (int) xv_get(frame_public, XV_HEIGHT, 0);
	    y += DisplayHeight(xv_display(info), screen) - height - (2 * FRAME_BORDER_WIDTH);
	}
	*defaults++ = (Xv_opaque) XV_X;
	*defaults++ = (Xv_opaque) x;
	*defaults++ = (Xv_opaque) XV_Y;
	*defaults++ = (Xv_opaque) y;
    }

    /* NULL terminate the defaults list */
    *defaults = 0;
    
    /* Do a frame set if there are frame attrs */
    if (defaults_array[0])
	status = (int) xv_set_avlist(frame_public, defaults_array);
    else
	status = XV_OK;

    /* Free any malloc'ed strings */
    if (defaults_string[0])
	free(defaults_string[0]);

    return (status);
}

/*
 * Convert command line icon defaults into attributes. Apply the attributes
 * to the frame's icon with xv_set_avlist().
 */
Pkg_private int
frame_set_icon_cmdline_options(frame_public, avlist)
    Frame           frame_public;
    Frame_attribute avlist[];
{
    Xv_opaque       defaults_array[ATTR_STANDARD_SIZE];
    char            errors[100], *string;
    int             status;
    Xv_object       screen, server;
    Pixrect        *image;
    register Attr_avlist defaults = defaults_array;
    char           *defaults_string = NULL;

    if (defaults_exists("icon.font.name", "Icon.Font.Name")) {
	*defaults++ = (Xv_opaque) ICON_FONT;
	string = defaults_get_string("icon.font.name",
				     "Icon.Font.Name", NULL);
	screen = xv_get(frame_public, XV_SCREEN);
	server = xv_get(screen, SCREEN_SERVER);
	*defaults++ = xv_get(server, SERVER_FONT_WITH_NAME, string);
    }
    if (defaults_exists("icon.pixmap", "Icon.Pixmap")) {
	string = defaults_get_string("icon.pixmap", "Icon.Pixmap", NULL);
	image = icon_load_mpr(string, errors);
	if (image) {
	    *defaults++ = (Xv_opaque) ICON_IMAGE;
	    *defaults++ = (Xv_opaque) image;
	} else {
	    /* BUG ALERT!  Show contents of errors to user? */
	}
    }
    if (defaults_exists("icon.footer", "Icon.Footer")) {
	string = defaults_get_string("icon.footer", "Icon.Footer", NULL);
	defaults_string = malloc(strlen(string) + 1); 
	strcpy(defaults_string, string);
	*defaults++ = (Xv_opaque) XV_LABEL;
	*defaults++ = (Xv_opaque) defaults_string;
    }
    if (defaults_exists("icon.x", "Icon.X")) {
	*defaults++ = (Xv_opaque) XV_X;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("icon.x", "Icon.X", 0);
    }
    if (defaults_exists("icon.y", "Icon.Y")) {
	*defaults++ = (Xv_opaque) XV_Y;
	*defaults++ = (Xv_opaque)
	    defaults_get_integer("icon.y", "Icon.Y", 0);
    }
    /* null terminate attr list */
    *defaults = NULL;

    /* Do a frame set if there are frame attrs */
    if (defaults_array[0])
	status = (int) xv_set_avlist(xv_get(frame_public, FRAME_ICON),
				     defaults_array);
    else
	status = XV_OK;

#ifdef RECLAIM_STRINGS
    /* Free any malloc'ed strings */
    if (defaults_string)
	free(defaults_string);
#endif RECLAIM_STRINGS

    return (status);
}

static int
frame_parse_color(frame, colorname, color)
Frame frame;
char *colorname;
struct xv_singlecolor *color;
{
    Xv_Drawable_info  *info;
    XColor             xcolor;
    int                red, green, blue;
    char 	       error_message[50];
    
    /* no color to parse! */
    if (!colorname) return(FALSE);
    
    /* Is it a X color specification?  */
    DRAWABLE_INFO_MACRO(frame, info);
    if (XParseColor(xv_display(info), 
		    (Colormap) xv_get(xv_screen(info), SCREEN_DEFAULT_COLORMAP),
		    colorname, &xcolor)) {
	/* BUG ALERT: because the xv_singlecolor has
	 * only unsigned characters, we can't support the full X Color 
	 * unsigned short values, so we must shift them right to get
	 * a decent value.  This should be fixed when we are able to 
	 * break binary compatibility (XView 2.0?) and change
	 * xv_singlecolor to use unsigned shorts.
	 */
	color->red =   (u_char) (xcolor.red >> 8);
	color->green = (u_char) (xcolor.green >> 8);
	color->blue =  (u_char) (xcolor.blue >> 8);
    } 

    /* Is it a sunview style color specification */
    else if (sscanf(colorname, "%d %d %d", &red, &green, &blue) == 3) {
	color->red =   (u_char) red;
	color->green = (u_char) green;
	color->blue =  (u_char) blue;
    }

    /* You got me. I give up, what kind of color is this? */
    else {
#ifdef OW_I18N
	sprintf(error_message,
		XV_I18N_MSG("xv_messages", "Unknown color: \"%.30s\""),
		 colorname);
#else
	sprintf(error_message, "Unknown color: \"%.30s\"", colorname);
#endif
	(void) xv_error(frame, 
			ERROR_STRING, error_message,
			ERROR_PKG,    FRAME,
			0);
	return(FALSE);
    }
    return(TRUE);
}
