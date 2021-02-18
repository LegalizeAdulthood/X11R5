#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon_obj.c 50.8 90/11/04";
#endif
#endif

/***********************************************************************/
/* icon_obj.c                               */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pixrect/pixrect.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/win_input.h>
#include <xview_private/icon_impl.h>
#include <xview/screen.h>
#include <xview/wmgr.h>
#include <xview/notify.h>
#include <xview_private/draw_impl.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

/*
 * Public
 */
extern Icon     icon_create();	/* compatibility */
extern          icon_set();	/* compatibility */
extern Xv_opaque icon_get();	/* compatibility */
extern          icon_destroy();	/* compatibility */
extern Notify_value icon_input();


/*****************************************************************************/
/* icon_create                                                               */
/*****************************************************************************/

/*VARARGS*/
Icon
icon_create(va_alist)
va_dcl
{
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return (Icon) xv_create_avlist(NULL, ICON, avlist);
}

/*ARGSUSED*//*VARARGS*/
int
icon_init(parent, object, avlist)
    Xv_opaque       parent;
    Xv_opaque       object;
    Xv_opaque      *avlist;
{
    register Xv_icon_info *icon;
    Rect            recticon;

    ((Xv_icon *) (object))->private_data = (Xv_opaque) xv_alloc(Xv_icon_info);
    if (!(icon = ICON_PRIVATE(object))) {
	xv_error(object,
		 ERROR_LAYER, ERROR_SYSTEM,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages", "Can't allocate icon structure"),
#else
		 ERROR_STRING, "Can't allocate icon structure",
#endif	OW_I18N
		 ERROR_PKG, ICON,
		 0);
	return XV_ERROR;
    }
    icon->public_self = object;
    icon->ic_gfxrect.r_width = 64;
    icon->ic_gfxrect.r_height = 64;
    rect_construct(&recticon, 0, 0, 64, 64);

    xv_set(object,
	   XV_SHOW, FALSE,
	   WIN_CONSUME_EVENT, WIN_REPAINT,
	   WIN_NOTIFY_SAFE_EVENT_PROC, icon_input,
	   WIN_NOTIFY_IMMEDIATE_EVENT_PROC, icon_input,
	   WIN_RECT, &recticon,
	   0);
    return XV_OK;
}


/*****************************************************************************/
/* icon_destroy	                                                             */
/*****************************************************************************/
icon_destroy(icon_public)
    Icon            icon_public;
{
    xv_destroy(icon_public);
}


/*****************************************************************************/
/* icon_destroy_internal						     */
/*****************************************************************************/

int
icon_destroy_internal(icon_public, status)
    Icon            icon_public;
    Destroy_status  status;
{

    Xv_icon_info   *icon = ICON_PRIVATE(icon_public);

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF))
	return XV_OK;

    free((char *) icon);

    return XV_OK;
}

/*****************************************************************************/
/* icon_set                                                                  */
/*****************************************************************************/

/*VARARGS*/
int
icon_set(icon_public, va_alist)
    Icon            icon_public;
va_dcl
{
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         valist;

    va_start(valist);
    (void) attr_make((char **) avlist, ATTR_STANDARD_SIZE, valist);
    va_end(valist);
    return (int) xv_set_avlist(icon_public, avlist);
}

/*VARARGS*/
Xv_opaque
icon_set_internal(icon_public, avlist)
    Icon            icon_public;
    register Attr_avlist avlist;
{
    register Xv_icon_info *icon = ICON_PRIVATE(icon_public);
    register Xv_opaque arg1;
    register short  repaint = FALSE;
    register short  label_changed = FALSE;
    
    for (; *avlist; avlist = attr_next(avlist)) {
	arg1 = avlist[1];
	switch (*avlist) {
	  case ICON_WIDTH:
	    icon->ic_gfxrect.r_width = (short) arg1;
	    repaint = TRUE;
	    break;

	  case ICON_HEIGHT:
	    icon->ic_gfxrect.r_height = (short) arg1;
	    repaint = TRUE;
	    break;

	  case ICON_IMAGE:
	    icon->ic_mpr = (struct pixrect *) arg1;
	    repaint = TRUE;
	    break;

	  case ICON_IMAGE_RECT:
	    if (arg1) {
		icon->ic_gfxrect = *(Rect *) arg1;
		repaint = TRUE;
	    }
	    break;

	  case ICON_LABEL_RECT:
	    if (arg1) {
		icon->ic_textrect = *(Rect *) arg1;
		repaint = TRUE;
	    }
	    break;

	  case XV_LABEL:
	    /* Consume attribute so that generic handler not also invoked. */
	    *avlist = (Xv_opaque) ATTR_NOP(*avlist);
#ifdef OW_I18N
	    icon->ic_flags &= (~ICON_TRANSLABEL);  /* set the flag to 0 */
set_xv_label:
	    if ( icon->ic_text_wcs )
		free ( icon->ic_text_wcs );
	    if ( icon->ic_text ) {
	        free( icon->ic_text );
		icon->ic_text = NULL;
	    }
	    if ( (char *) arg1 )
		icon->ic_text_wcs = mbstowcsdup((char *)arg1);
	    label_changed = TRUE;
	    repaint = TRUE;
#else OW_I18N
	    if ( icon->ic_text )
	        free( icon->ic_text );
	    if ( (char *) arg1 ) {
		icon->ic_text = (char *) calloc( 1, strlen((char *) arg1) + 1 );
		strcpy( icon->ic_text, (char *) arg1 );
	    }
	    label_changed = TRUE;
	    repaint = TRUE;
	    icon->ic_flags &= (~ICON_TRANSLABEL);  /* set the flag to 0 */
#endif OW_I18N
	    break;

#ifdef OW_I18N
	  case XV_LABEL_WCS:
	    /* Consume attribute so that generic handler not also invoked. */
	    *avlist = (Xv_opaque) ATTR_NOP(*avlist);
	    icon->ic_flags &= (~ICON_TRANSLABEL);  /* set the flag to 0 */
set_xv_label_wcs:
	    if ( icon->ic_text_wcs )
		free ( icon->ic_text_wcs );
	    if ( icon->ic_text ) {
	        free( icon->ic_text );
		icon->ic_text = NULL;
	    }
	    if ( (wchar_t *) arg1 )
		icon->ic_text_wcs = wsdup((wchar_t *)arg1);
	    label_changed = TRUE;
	    repaint = TRUE;
	    break;
#endif OW_I18N

	  case XV_OWNER:{

		/*
		 * Consume attribute so that generic handler not also
		 * invoked.
		 */
		*avlist = (Xv_opaque) ATTR_NOP(*avlist);
		icon->frame = arg1;
		break;
	    }

	  case WIN_CMS_CHANGE:
	    repaint = TRUE;
	    break;

	  case ICON_TRANSPARENT:
	    if (arg1) 
		icon->ic_flags |= ICON_BKGDTRANS;
	    else 
		icon->ic_flags &= (~ICON_BKGDTRANS);
	    repaint = TRUE;
	    break;

	  case ICON_MASK_IMAGE:
	    icon->ic_mask = (Server_image) arg1;
	    repaint = TRUE;
	    break;

	  case ICON_TRANSPARENT_LABEL:
#ifdef OW_I18N
	    icon->ic_flags |= ICON_TRANSLABEL;
	    goto set_xv_label;
#else OW_I18N
	    if ( icon->ic_text )
	        free( icon->ic_text );
	    if ( (char *) arg1 ) {
		icon->ic_text = (char *) calloc( 1, strlen((char *) arg1) + 1 );
		strcpy( icon->ic_text, (char *) arg1 );
	    }
	    label_changed = TRUE;
	    icon->ic_flags |= ICON_TRANSLABEL;
	    repaint = TRUE;
	    break;
#endif OW_I18N

#ifdef OW_I18N
	  case ICON_TRANSPARENT_LABEL_WCS:
	    icon->ic_flags |= ICON_TRANSLABEL;
	    goto set_xv_label_wcs;
#endif OW_I18N

	  default:
	    if (xv_check_bad_attr(ICON, *avlist) == XV_OK) {
		return *avlist;
	    }
	    break;
	}
    }

    /*
     * tell the window manager the new icon name, this provides a fall-back
     * for window managers whose ideas about icons differ widely from those
     * of the client.
     */
    if (label_changed && icon->frame) {
	Xv_Drawable_info *info;
	DRAWABLE_INFO_MACRO(icon->frame, info);
	XSetIconName(xv_display(info), xv_xid(info), icon->ic_text);
    }
    
    if (icon->ic_mask || (icon->ic_flags & ICON_BKGDTRANS ) ) 
	icon_set_wrk_space_color( icon_public );

    if (repaint && icon->ic_flags & ICON_PAINTED)
	icon_display(icon_public, 0, 0);

    return (Xv_opaque) XV_OK;
}

/*****************************************************************************/
/* icon_get                                                                  */
/*****************************************************************************/

Xv_opaque
icon_get(icon_public, attr)
    register Icon   icon_public;
    Icon_attribute  attr;
{
    return xv_get(icon_public, attr);
}


/*ARGSUSED*/
Xv_opaque
icon_get_internal(icon_public, status, attr, args)
    Icon            icon_public;
    int            *status;
    Attr_attribute  attr;
    va_list         args;
{
    Xv_icon_info   *icon = ICON_PRIVATE(icon_public);

    switch (attr) {
      case ICON_WIDTH:
	return (Xv_opaque) icon->ic_gfxrect.r_width;

      case ICON_HEIGHT:
	return (Xv_opaque) icon->ic_gfxrect.r_height;

      case ICON_IMAGE:
	return (Xv_opaque) icon->ic_mpr;

      case ICON_IMAGE_RECT:
	return (Xv_opaque) & (icon->ic_gfxrect);

      case ICON_LABEL_RECT:
	return (Xv_opaque) & (icon->ic_textrect);

      case XV_LABEL:
#ifdef OW_I18N
get_xv_label:
	if ( icon->ic_text == NULL && icon->ic_text_wcs != NULL )
	    icon->ic_text = wcstombsdup( icon->ic_text_wcs );
#endif
	return (Xv_opaque) icon->ic_text;

#ifdef OW_I18N
      case XV_LABEL_WCS:
	return (Xv_opaque) icon->ic_text_wcs;
#endif

      case XV_OWNER:
	return (Xv_opaque) icon->frame;

      case ICON_TRANSPARENT_LABEL:
#ifdef OW_I18N
	goto get_xv_label;
#else
	return (Xv_opaque) icon->ic_text;
#endif

#ifdef OW_I18N
      case ICON_TRANSPARENT_LABEL_WCS:
	return (Xv_opaque) icon->ic_text_wcs;
#endif

      case ICON_TRANSPARENT:
	return (Xv_opaque) (icon->ic_flags & ICON_BKGDTRANS);

      case ICON_MASK_IMAGE:
	return (Xv_opaque) icon->ic_mask;

      default:
	if (xv_check_bad_attr(ICON, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	return (Xv_opaque) NULL;
    }
    /* NOTREACHED */
}


icon_set_wrk_space_color( icon_public )
Icon            icon_public;
{	
    Display    *display;
    Xv_Screen  screen;
    int        scrn_num;
    XID        xid;
    Colormap   cmap;
    register Xv_Drawable_info  *info;
    char       *wrk_space_color;
    unsigned long   wrk_space_pixel;
    XColor     rgb;

    DRAWABLE_INFO_MACRO( icon_public, info );
    display = xv_display( info );
    xid = (XID) xv_xid(info);
    screen = xv_screen( info );
    scrn_num = (int) xv_get( screen, SCREEN_NUMBER );
	    
    cmap = DefaultColormap( display, scrn_num );
    wrk_space_color = (char *) defaults_get_string(
				   "openWindows.workspaceColor",
				   "OpenWindows.WorkspaceColor", "#cccccc" );
    
    if ( DisplayPlanes( display, scrn_num ) == 1 ) {
	wrk_space_pixel = WhitePixel( display, scrn_num );
    }
    else  {
	if (!XParseColor( display, cmap, wrk_space_color, &rgb ) ) {
	    char msg[100];
		
#ifdef	OW_I18N
	    sprintf(msg,XV_I18N_MSG("xv_messages", "icon_obj:color name %s not in database"),
		    wrk_space_color );
#else
	    sprintf(msg,"icon_obj:color name %s not in database",
		    wrk_space_color );
#endif	OW_I18N
	    xv_error( NULL, ERROR_SEVERITY, ERROR_RECOVERABLE,
		     ERROR_STRING, msg,
		     ERROR_PKG, ICON,
		     0 );
	}
	if ( !XAllocColor( display, cmap, &rgb ) )  {
	    xv_error( NULL, ERROR_SEVERITY, ERROR_RECOVERABLE,
#ifdef	OW_I18N
		     ERROR_STRING, XV_I18N_MSG("xv_messages", "icon_obj: All color cells are allocated"),
#else
		     ERROR_STRING, "icon_obj: All color cells are allocated",
#endif	OW_I18N
		     ERROR_PKG, ICON,
		     0 );	
	}	    
	wrk_space_pixel = rgb.pixel;
    }
    XSetWindowBackground( display, xid, wrk_space_pixel );
}








