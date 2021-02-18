#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon.c 70.3 91/07/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * icon.c - Display icon.
 */

#include <stdio.h>
/* #include <pixrect/pixrect_hs.h> */
#include <xview/frame.h>
#include <xview/xview.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/pixwin.h>
#include <xview/font.h>
#include <xview_private/icon_impl.h>
#include <xview_private/svrim_impl.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N


unsigned long  GetWorkSpacePixel();


Xv_private void
icon_display(icon_public, x, y)
    Icon            icon_public;
    register int    x, y;
{
    extern Pixrect             *frame_bkgrd;
    register Xv_icon_info      *icon = ICON_PRIVATE(icon_public);
    register Xv_Window         pixwin = icon_public;
    unsigned long              wrk_space_pixel;
    register Xv_Drawable_info  *info;
    register Display           *display;
    register XID               xid;
    
    DRAWABLE_INFO_MACRO( pixwin, info );
    display = xv_display( info );
    xid = (XID) xv_xid(info);

    if (icon->ic_flags & ICON_BKGRDGRY || icon->ic_flags & ICON_BKGRDPAT) {
	/* Cover the icon's rect with pattern */
	(void) xv_replrop(pixwin, x, y,
			icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		   PIX_SRC, (icon->ic_flags & ICON_BKGRDGRY) ? frame_bkgrd :
			  icon->ic_background, 0, 0);
    } else if (icon->ic_flags & ICON_BKGRDCLR ||
	       icon->ic_flags & ICON_BKGRDSET) {
	/* Cover the icon's rect with solid. */
	(void) pw_writebackground(pixwin, x, y,
			icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		      (icon->ic_flags & ICON_BKGRDCLR) ? PIX_CLR : PIX_SET);
    }

    if ( (icon->ic_flags & ICON_BKGDTRANS) || icon->ic_mask ) 	
        wrk_space_pixel = (unsigned long) GetWorkSpacePixel(pixwin);

    if ( icon->ic_mask )  {   /* we have a icon mask to use */
        FillRect( pixwin, wrk_space_pixel,
		 icon->ic_gfxrect.r_left, icon->ic_gfxrect.r_top,
		 icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height);	
	DrawNonRectIcon( display, xid, icon, info, x, y, wrk_space_pixel );
    }
    else   {
	if (icon->ic_mpr ) {
	    if ( icon->ic_flags & ICON_BKGDTRANS ) 	
	        DrawTransparentIcon( icon, pixwin, x, y, wrk_space_pixel );
	    else
	        (void) xv_rop(pixwin,
		      icon->ic_gfxrect.r_left + x, icon->ic_gfxrect.r_top + y,
		      icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		      PIX_SRC, icon->ic_mpr, 0, 0);
	}
    }
#ifdef OW_I18N
    if (icon->ic_text_wcs && (icon->ic_text_wcs[0] != '\0')) 
#else
    if (icon->ic_text && (icon->ic_text[0] != '\0')) 
#endif
	icon_draw_label( icon, pixwin, info, x, y, wrk_space_pixel );
    icon->ic_flags |= ICON_PAINTED;
}


icon_draw_label( icon, pixwin, info, x, y, wrk_space_pixel )
register Xv_icon_info      *icon;
register Xv_Window         pixwin;
register int             x, y;
register Xv_Drawable_info  *info;
unsigned long              wrk_space_pixel;
{	
    PIXFONT        *font = (PIXFONT *) xv_get(pixwin, WIN_FONT);
    int            left, top, line_leading = xv_get(font, FONT_DEFAULT_CHAR_HEIGHT);
#ifdef OW_I18N
    XFontSet       font_set;
    Display        *dpy;
    XRectangle	   overall_ink_extents, overall_logical_extents;
#else
    XFontStruct    *x_font_info;
#endif
    XCharStruct    overall_return;
    struct rect    textrect;
	
    if (rect_isnull(&icon->ic_textrect)) 
        /* Set text rect to accomodate 1 line at bottom. */
        rect_construct(&icon->ic_textrect,
		       0, icon->ic_gfxrect.r_height - line_leading,
		       icon->ic_gfxrect.r_width, line_leading);

    if ( (icon->ic_flags & ICON_BKGDTRANS) || icon->ic_mask ) {
	if ( !( icon->ic_flags & ICON_TRANSLABEL) )  /*check for transparent label*/
            FillRect( pixwin, wrk_space_pixel,
		 icon->ic_textrect.r_left + x, icon->ic_textrect.r_top + y - 3,
		 icon->ic_textrect.r_width, icon->ic_textrect.r_height + 3);	
    }
    else
        /* Blank out area onto which text will go. */
        (void) xv_rop(pixwin,
		      icon->ic_textrect.r_left + x, icon->ic_textrect.r_top + y-3,
		      icon->ic_textrect.r_width, icon->ic_textrect.r_height+3,
		      PIX_CLR, (Pixrect *)NULL, 0, 0 );

    /* Format text into textrect */
    textrect = icon->ic_textrect;
    textrect.r_left += x;
    textrect.r_top += y;

#ifdef OW_I18N
    font_set = (XFontSet) xv_get(font, FONT_SET_ID);
    dpy = xv_display( info );
    XwcTextExtents(font_set, icon->ic_text_wcs, wslen(icon->ic_text_wcs),
    	&overall_ink_extents, &overall_logical_extents);
    left = (icon->ic_gfxrect.r_width - overall_ink_extents.width)/2;

    if (left < 0)
	left = 0;
    top = textrect.r_top - overall_logical_extents.y - 3;

#else OW_I18N
    x_font_info = (XFontStruct *) xv_get(font, FONT_INFO );

    (void) XTextExtents( x_font_info, icon->ic_text, strlen( icon->ic_text ),
			&direction, &ascent, &descent, &overall_return );

    left = (icon->ic_gfxrect.r_width - overall_return.width)/2;
    if (left < 0)  
	left = 0;

    top = textrect.r_top + x_font_info->ascent -3;
#endif

    if ( (icon->ic_flags & ICON_BKGDTRANS) || icon->ic_mask )
#ifdef OW_I18N
        DrawString(pixwin,xv_fg(info),wrk_space_pixel,left,top,font_set,icon->ic_text_wcs);
#else
        DrawString(pixwin,xv_fg(info),wrk_space_pixel,left,top,font,icon->ic_text);
#endif
    else
#ifdef OW_I18N
    {
	GC		gc;
	Drawable	d;

	gc = xv_find_proper_gc(dpy, info, PW_TEXT);
	d = xv_xid( info );
	xv_set_gc_op(dpy, info, gc, PIX_SRC,
		PIX_OPCOLOR(PIX_SRC) ? XV_USE_OP_FG : XV_USE_CMS_FG,
		XV_DEFAULT_FG_BG);
        (void) XwcDrawString(dpy, d, font_set, gc,
		left, top, icon->ic_text_wcs, wslen(icon->ic_text_wcs));
    }
#else
        (void) xv_text(pixwin, left, top, PIX_SRC, font, icon->ic_text);
#endif
    
}
    

#ifdef OW_I18N
DrawString( win, frg_pixel, bkg_pixel, x, y, font_set, str )
#else
DrawString( win, frg_pixel, bkg_pixel, x, y, pixfont, str )
#endif
register Xv_Window  win;
unsigned long       frg_pixel, bkg_pixel;
register int        x, y;
#ifdef OW_I18N
XFontSet            font_set;
wchar_t            *str;
#else
Xv_opaque           pixfont;
char                *str;
#endif
{
    register Xv_Drawable_info  *info, *src_info;
    Display  *display;
#ifdef OW_I18N
    XID      xid;
#else
    XID      xid, font;
#endif
    GC       gc;
    XGCValues  val;
    unsigned long  val_mask;
    
    DRAWABLE_INFO_MACRO( win, info );
    display = xv_display( info );
    xid = (XID) xv_xid(info);
#ifndef OW_I18N
    font = (XID) xv_get( pixfont, XV_XID );
#endif

    gc = xv_find_proper_gc( display, info, PW_TEXT );
    val.function = GXcopy;
    val.foreground = frg_pixel;
    val.background = bkg_pixel;
    val.clip_mask = None;
    val_mask = GCBackground | GCForeground | GCClipMask;
    XChangeGC(display, gc, val_mask, &val );
#ifndef OW_I18N
    XSetFont(display, gc, font );
#endif

#ifdef OW_I18N
    XwcDrawString( display, xid, font_set, gc, x, y, str, wslen(str) );
#else
    XDrawString( display, xid, gc, x, y, str, strlen(str) );
#endif
}

FillRect( win, bkg_pixel, x, y, w, h )
register Xv_Window win;
unsigned long      bkg_pixel;
register int  x, y, w, h;
{
    register Xv_Drawable_info  *info;
    Display  *display;
    XID      xid;
    GC       gc;
    XGCValues  val;
    unsigned long  val_mask;
    
    DRAWABLE_INFO_MACRO( win, info );
    display = xv_display( info );
    xid = (XID) xv_xid(info);

    gc = xv_find_proper_gc( display, info, PW_ROP );
    val.function = GXcopy;
    val.foreground = bkg_pixel;
    val.fill_style = FillSolid;
    val.clip_mask = 0;
    val_mask = GCClipMask | GCFillStyle | GCForeground | GCFunction;
    XChangeGC(display, gc, val_mask, &val );
    XFillRectangle( display, xid, gc, x, y, w, h );
}

    
DrawTransparentIcon( icon, pixwin, x, y, bkg_color )
register Xv_icon_info    *icon;
register Xv_Window       pixwin;
register int             x, y;
unsigned long            bkg_color;
{
    register Xv_Drawable_info  *info, *src_info;
    Display  *display;
    XID      xid;
    GC       gc;
    XGCValues  val;
    unsigned long  val_mask;
    
    DRAWABLE_INFO_MACRO( pixwin, info );
    display = xv_display( info );
    xid = (XID) xv_xid(info);
    
	
    DRAWABLE_INFO_MACRO( (Xv_opaque) icon->ic_mpr, src_info );
    gc = xv_find_proper_gc( display, info, PW_ROP );
    val.function = GXcopy;
    val.plane_mask = xv_plane_mask(info);
    val.background = bkg_color;
    val.foreground = xv_fg(info);
    val.stipple = xv_xid(src_info);
    val.fill_style = FillOpaqueStippled;
    val.ts_x_origin = 0;
    val.ts_y_origin = 0;	
    val_mask = GCForeground | GCBackground | GCFunction | 
               GCPlaneMask | GCFillStyle | GCTileStipXOrigin | 
               GCTileStipYOrigin | GCStipple;
	
    XChangeGC(display, gc, val_mask, &val );
    XFillRectangle( display, xid, gc, icon->ic_gfxrect.r_left + x,
		   icon->ic_gfxrect.r_top + y,
		   icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height );
}


unsigned long
GetWorkSpacePixel( win )
Xv_opaque win;
{
    int   scrn_num;
    XID   xid;
    Display  *display;
    XWindowAttributes  win_attr;
    char    *wrk_spc_color, *defaults_get_string();
    Xv_Screen  screen;
    unsigned long   wrk_space_pixel;
    Colormap   cmap;
    XColor     rgb;
    
    display = (Display *) xv_get( win, XV_DISPLAY );
    screen = (Xv_Screen) xv_get( win, XV_SCREEN );
    scrn_num = (int) xv_get( screen, SCREEN_NUMBER );
    xid = (XID) xv_get( win, XV_XID );
    cmap = DefaultColormap( display, scrn_num );


    wrk_spc_color = (char *) defaults_get_string("openWindows.workspaceColor",
					"OpenWindows.WorkspaceColor", "#cccccc" );
    
    if ( DisplayPlanes( display, scrn_num ) == 1 ) {
        wrk_space_pixel = WhitePixel( display, scrn_num );
    }
    else  {
	if (!XParseColor( display, cmap, wrk_spc_color, &rgb ) ) {
	    char msg[100];
	    
#ifdef	OW_I18N
	    sprintf( msg,XV_I18N_MSG("xv_messages", "GetWorkSpacePixel: color name %s not in database"),
		    wrk_spc_color );
#else
	    sprintf( msg,"GetWorkSpacePixel: color name %s not in database",
		    wrk_spc_color );
#endif	OW_I18N
	    xv_error( NULL, ERROR_SEVERITY, ERROR_RECOVERABLE,
		     ERROR_STRING, msg,
		     ERROR_PKG, ICON,
		     0 );
	}
	if ( !XAllocColor( display, cmap, &rgb ) )  {
	    xv_error( NULL, ERROR_SEVERITY, ERROR_RECOVERABLE,
#ifdef	OW_I18N
		     ERROR_STRING,XV_I18N_MSG("xv_messages","GetWorkSpacePixel:All color cells are allocated"),
#else
		     ERROR_STRING,"GetWorkSpacePixel:All color cells are allocated",
#endif	OW_I18N
		     ERROR_PKG, ICON,
		     0 );	
	}	    
	wrk_space_pixel = rgb.pixel;
    }
    return wrk_space_pixel;
}

DrawNonRectIcon( display, xid, icon, info, x, y, bkg_color )
Display                  *display;
XID                      xid;
register Xv_icon_info    *icon;
register Xv_Drawable_info  *info;
register int             x, y;
unsigned long            bkg_color;
{
    register Xv_Drawable_info  *src_info, *mask_info;
    GC       gc;
    XGCValues  val;
    unsigned long  val_mask;

    DRAWABLE_INFO_MACRO( (Xv_opaque) icon->ic_mask, mask_info );
    gc = xv_find_proper_gc( display, info, PW_ROP );

    val.function = GXcopy;
    val.plane_mask = xv_plane_mask(info);
    if ( info->is_bitmap ) {
	val.background = WhitePixel( display, DefaultScreen( display) );
	val.foreground = BlackPixel( display, DefaultScreen( display) );
    }
    else  {
	val.background = xv_bg(info);
	val.foreground = xv_fg(info);
    }
    val.fill_style = FillOpaqueStippled;
    val.ts_x_origin = 0;
    val.ts_y_origin = 0;	
    val_mask = GCForeground | GCBackground | GCFunction | 
               GCPlaneMask | GCFillStyle | GCTileStipXOrigin | 
               GCTileStipYOrigin;
    XChangeGC(display, gc, val_mask, &val );

    if (PR_NOT_MPR(((Pixrect *) icon->ic_mpr)))  {
	DRAWABLE_INFO_MACRO( (Xv_opaque) icon->ic_mpr, src_info );
	val.clip_mask = xv_xid(mask_info);
	val.stipple = xv_xid(src_info);
	val_mask = GCStipple | GCClipMask;
	XChangeGC(display, gc, val_mask, &val );

	if ( xv_rop_internal( display, xid, gc, icon->ic_gfxrect.r_left + x,
			     icon->ic_gfxrect.r_top + y,
			     icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
			     (Xv_opaque) icon->ic_mpr, 0, 0, info ) == XV_ERROR) {
#ifdef	OW_I18N
	    xv_error( NULL, ERROR_STRING, XV_I18N_MSG("xv_messages","xv_rop: xv_rop_internal failed"), 0 );
#else
	    xv_error( NULL, ERROR_STRING, "xv_rop: xv_rop_internal failed", 0 );
#endif	OW_I18N
	}
    }
    else {
	if (xv_rop_mpr_internal( display, xid, gc, icon->ic_gfxrect.r_left + x,
			     icon->ic_gfxrect.r_top + y,
			     icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
			     icon->ic_mpr, 0, 0, info, TRUE) == XV_ERROR)
	return(XV_ERROR);
    }
    XSync( display, FALSE );
}
