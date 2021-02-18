/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)FXlib.c	2.1 91/08/13 11:28:27 FUJITSU LIMITED. */
/*
****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Takashi Fujiwara	FUJITSU LIMITED
				fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya		FUJITSU LIMITED
	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_sccs_id[] = "@(#)FXlib.c	2.1 91/08/13 11:28:27 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <stdio.h>
#include "Htt.h"

#ifdef	USE_X11R5_LIB

int
FXLoadQueryBaseFont(display, current_preedit_font, base_font_name)
    Display        *display;
    FXimpFont      *current_preedit_font;
    char           *base_font_name;
{
    char		**miss_name_return ;
    char		**def_string_return ;
    int			  miss_count_return = 0 ;
    XFontSet		 *font_set ;
    XFontSetExtents	 *fset_extents = NULL ;
    XRectangle		 *font_rect ;

#ifdef	DEBUG
    (void)dbg_printf( "*** FXLoadQueryBaseFont Start ***\n" ) ;
#endif	/* DEBUG */
    font_set = (XFontSet *)XCreateFontSet( display, base_font_name,
		    &miss_name_return, &miss_count_return, &def_string_return ) ;
#ifdef	DEBUG
    (void)dbg_printf( "  miss_count : %d\n", miss_count_return ) ;
#endif	/* DEBUG */

    if ( ( miss_count_return ) || ( !font_set ) )
	return ( -1 ) ;

    current_preedit_font->font_set = font_set ;
    current_preedit_font->status = FF_FONT_LOAD;

    fset_extents = XExtentsOfFontSet( font_set ) ;
    font_rect = &fset_extents->max_logical_extent ;

    current_preedit_font->max_font_width  = font_rect->width ;
    current_preedit_font->max_font_height = font_rect->height ;
    current_preedit_font->max_font_ascent = 0 - font_rect->y ;
    current_preedit_font->max_font_descent = font_rect->height + font_rect->y ;
#ifdef	DEBUG
    (void)dbg_printf( "  max_font_width   : %d\n", font_rect->width ) ;
    (void)dbg_printf( "  max_font_height  : %d\n", font_rect->height ) ;
    (void)dbg_printf( "  max_font_ascent  : %d\n", 0 - font_rect->y ) ;
    (void)dbg_printf( "  max_font_descent : %d\n",
			font_rect->height + font_rect->y ) ;
    (void)dbg_printf( "*** FXLoadQueryBaseFont Norm return ***\n" ) ;
#endif	/* DEBUG */
    return (0);
}

int
FXFreeFont(display, current_preedit_font)
    Display        *display;
    FXimpFont      *current_preedit_font;
{
    if (!(current_preedit_font->status & FF_FONT_LOAD))
	return (0);

    XFreeFontSet( display, current_preedit_font->font_set ) ;
    return (0);
}

int
FXCreateGC(display, window, current_preedit_font,
	   foreground_pixel, background_pixel)
    Display        *display;
    Window          window;
    FXimpFont      *current_preedit_font;
    unsigned long   foreground_pixel, background_pixel;
{
    unsigned long   gcmask;
    XGCValues       gcvalues, reverse_gcvalues;

    gcmask = GCBackground | GCForeground ;
    gcvalues.foreground = foreground_pixel;
    gcvalues.background = background_pixel;
    reverse_gcvalues.foreground = background_pixel;
    reverse_gcvalues.background = foreground_pixel;

    current_preedit_font->normal_gc
	= XCreateGC(display, window, gcmask, &gcvalues);
    current_preedit_font->reverse_gc
	= XCreateGC(display, window, gcmask, &reverse_gcvalues);

    gcvalues.function = GXxor;
    gcvalues.foreground = background_pixel ^ foreground_pixel;
    gcvalues.plane_mask = background_pixel ^ foreground_pixel;
    gcmask = GCForeground | GCFunction | GCPlaneMask;
    current_preedit_font->invert_gc
	= XCreateGC(display, window, gcmask, &gcvalues);
    current_preedit_font->status |= FF_CREATE_GC;
    return (0);
}

int
FXFreeGC(display, current_preedit_font)
    Display        *display;
    FXimpFont      *current_preedit_font;
{
    if (current_preedit_font->status & FF_CREATE_GC) {
#define M_FreeGC( gc ) if( gc ) { XFreeGC( display, gc ); gc = 0; }

	M_FreeGC(current_preedit_font->normal_gc);
	M_FreeGC(current_preedit_font->reverse_gc);
	M_FreeGC(current_preedit_font->invert_gc);
#undef  M_FreeGC
    }
    return (0);
}

int
FXChangeGC(display, window, current_preedit_font,
	   foreground_pixel, background_pixel)
    Display        *display;
    Window          window;
    FXimpFont      *current_preedit_font;
    unsigned long   foreground_pixel, background_pixel;
{
    unsigned long   gcmask;
    XGCValues       gcvalues, reverse_gcvalues;
    if ((current_preedit_font->status & FF_CREATE_GC) == (unsigned char) 0) {
	return FXCreateGC(display, window, current_preedit_font,
			  foreground_pixel, background_pixel);
    }
    gcmask = GCBackground | GCForeground ;
    gcvalues.foreground = foreground_pixel;
    gcvalues.background = background_pixel;
    reverse_gcvalues.foreground = background_pixel;
    reverse_gcvalues.background = foreground_pixel;

    XChangeGC(display, current_preedit_font->normal_gc, gcmask, &gcvalues);
    XChangeGC(display, current_preedit_font->reverse_gc, gcmask,
	      &reverse_gcvalues);

    gcvalues.function = GXxor;
    gcvalues.foreground = background_pixel ^ foreground_pixel;
    gcvalues.plane_mask = background_pixel ^ foreground_pixel;
    gcmask = GCForeground | GCFunction | GCPlaneMask;
    XChangeGC(display, current_preedit_font->invert_gc, gcmask, &gcvalues);
    return (0);
}

int
FXTextWidth(current_preedit_font, input_data, length)
    FXimpFont      *current_preedit_font;
    wchar_t        *input_data;
    int             length;
{
    int		width = 0 ;

    if ( length )
	width = XwcTextEscapement( current_preedit_font->font_set,
				input_data, length ) ;
#ifdef	DEBUG
    (void)dbg_printf( "*** FXTextWidth width : %d ***\n", width ) ;
#endif	/* DEBUG */
    return ( width ) ;
}

int
FXTextExtentsArea( current_preedit_font, preedit,
			input_data, length, ret_area )
    FXimpFont			*current_preedit_font;
    Ximp_PreeditPropRec		*preedit;
    wchar_t			*input_data;
    int				 length;
    FXimp_PreeditAreaInfo	*ret_area;
{
    int		 edit_x1, edit_y1, edit_x2, edit_y2;
    int		 width, height;
    int		 st_x, st_y, ed_x, ed_y;
    int		 area_no = 0, start_pos = 0;
    int		 j;

    edit_x1 = preedit->Area.x;
    edit_y1 = preedit->Area.y;
    edit_x2 = edit_x1 + preedit->Area.width;
    edit_y2 = edit_y1 + preedit->Area.height;

#ifdef	DEBUG
    (void)dbg_printf( "*** FXTextExtentsArea ***\n");
    (void)dbg_printf( "  edit_x1=%d,edit_y1=%d,edit_x2=%d,edit_y2=%d\n",
			edit_x1, edit_y1, edit_x2, edit_y2);
#endif	/* DEBUG */
    width  = current_preedit_font->max_font_width;
    height = current_preedit_font->max_font_height;

    st_x = preedit->SpotLocation.x;
    st_y = preedit->SpotLocation.y - current_preedit_font->max_font_ascent;

    if ( st_x < edit_x1 )
		st_x = edit_x1;
    if ( st_x > edit_x2 ) {
		st_x = edit_x2;
		st_y += height + preedit->LineSpacing;
    }

    if ( st_y < edit_y1 )
		st_y = edit_y1 ;
    if ( preedit->SpotLocation.y > edit_y2 )
		st_y = edit_y1;

    ed_x = st_x;
    ed_y = st_y + height;

    if ( !length ) {
	ret_area->area_start_pos = 0;
	ret_area->area_draw_length = 0;
	ret_area->preedit_area.x = st_x;
	ret_area->preedit_area.y = st_y;
	ret_area->preedit_area.width = width;
	ret_area->preedit_area.height = height;
	area_no++;
	return ( area_no ) ;
    }

    for ( j = 0; j < length; ) {
	width = FXTextWidth( current_preedit_font, input_data, 1 );
	if ( ( ed_x + width ) <= edit_x2 ) {
	    ret_area->area_draw_length++;
	    ed_x += width;
	    j++;
	    input_data++;
	} else {
	    if ( ret_area->area_draw_length ) {
		area_no++;
		ret_area->area_start_pos = start_pos;
		ret_area->preedit_area.x = st_x;
		ret_area->preedit_area.y = st_y;
		ret_area->preedit_area.width = ed_x - st_x;
		ret_area->preedit_area.height = height;
		ret_area++;
		if ( area_no >= 2 )
			return ( area_no );
		start_pos = j;
	    }
	    st_x = edit_x1;
	    st_y = ed_y + preedit->LineSpacing;
	    ed_x = st_x;
	    ed_y = st_y + height;
	    if ( ed_y > edit_y2 ) {
		st_y = edit_y1;
		ed_y = st_y + height;
	    }
	}
    }

    if ( ret_area->area_draw_length ) {
	area_no++;
	ret_area->area_start_pos = start_pos;
	ret_area->preedit_area.x = st_x;
	ret_area->preedit_area.y = st_y;
	ret_area->preedit_area.width = ed_x - st_x;
	ret_area->preedit_area.height = height;
    }

    return ( area_no );

}

void
FXDrawImageString(display, window, current_preedit_font,
		  reverse, x, y, buff, length)
    Display	*display;
    Window	 window;
    FXimpFont	*current_preedit_font;
    int		 reverse;
    int		 x, y;
    wchar_t	*buff;
    int		 length;
{
    GC		 gc ;
#ifdef	DEBUG
    (void)dbg_printf( "*** FXImageString Start ***\n" ) ;
    (void)dbg_printf( "  Draw length : %d\n", length ) ;
    (void)dbg_printf( "  Draw x : %d\n", x ) ;
    (void)dbg_printf( "  Draw y : %d\n", y ) ;
#endif	/* DEBUG */

    if ( !length )
	return ;

    if ( reverse ) {
	XRectangle	 overall_ink_return, overall_logical_return ;
	XRectangle	*ink =     &overall_ink_return,
			*logical = &overall_logical_return ;
	gc = current_preedit_font->reverse_gc ;
	(void)XwcTextExtents( current_preedit_font->font_set,
			buff, length, ink, logical ) ;
#ifdef	DEBUG
    (void)dbg_printf( " ** XwcTextExteents return **\n" ) ;
    (void)dbg_printf( "    width=%d, height=%d, x=%d, y=%d\n",
			ink->width, ink->height, ink->x, ink->y );
#endif	/* DEBUG */
	XFillRectangle( display, window, current_preedit_font->normal_gc,
		x, y + ink->y,
		x + ink->width, y + ( ink->height + ink->y ) ) ;
    } else {
	gc = current_preedit_font->normal_gc ;
    }

    XwcDrawString( display, window, current_preedit_font->font_set,
		gc, x, y, buff, length ) ;

    return ;
}

int 
FXDefaultError(display, event)
    Display        *display;
    XErrorEvent    *event;
{
#define	MSG_SIZE	64
    char            xerr_msg[MSG_SIZE];
    extern int      verbose_flag;
    if (verbose_flag) {
	XGetErrorText(display, event->error_code, xerr_msg, MSG_SIZE);
	fprintf(stderr, "%s: X Error: %s\n", command_name, xerr_msg);
	fprintf(stderr, "  req.: %d - %d, res.ID: 0x%x, error #%d - #%d\n",
		event->request_code, event->minor_code,
		event->resourceid, event->serial, display->request);
    }
}

#else

int
FXLoadQueryBaseFont(display, current_preedit_font, base_font_name)
    Display        *display;
    FXimpFont      *current_preedit_font;
    char           *base_font_name;
{
    XFontStruct    *ascii_kana_font_struct = 0, *kanji_font_struct = 0, *gaiji_font_struct = 0;
    char           *ascii_kana_font_name, *kanji_font_name, *gaiji_font_name;
    int             wfont_h, no = 0, ii;
    int             error = 0;
    char           *cfn, cc, *none, *none2, null = 0;
    XFontStruct    *font_struct, *font_struct_tbl[4];
    char           *cfnt[4], work[512];

    current_preedit_font->fixe_width = 1;
    if (!base_font_name || !*base_font_name)
	return (-2);

    cfn = work;
    cfnt[no++] = cfn;
    while ((cc = *base_font_name++) != NULL) {
	if (cc == ',') {
	    *cfn++ = NULL;
	    cfnt[no++] = cfn;
	} else
	    *cfn++ = cc;
    }
    *cfn = NULL;

    none = &null;
    none2 = &null;
    error = 0;
    for (ii = 0; ii < no; ii++) {
	font_struct = XLoadQueryFont(display, cfnt[ii]);
	if (!font_struct) {
	    ErrorMessage(30, cfnt[ii]);
	    continue;
	}
#ifdef DEBUG
	dbg_printf("XLoadQueryFont (%s)\n min_byte1=%d max_byte1=%d min_char_or_byte2=%d max_char_or_byte2=%d\n",
		   cfnt[ii], font_struct->min_byte1, font_struct->max_byte1,
	    font_struct->min_char_or_byte2, font_struct->max_char_or_byte2);
#endif				/* DEBUG */
	if (font_struct->min_byte1 == 0 || font_struct->max_byte1 == 0) {
	    if (font_struct->min_char_or_byte2 <= 0x20
		&& font_struct->max_char_or_byte2 == 0xDF) {
#ifdef DEBUG
		dbg_printf("  (ascii_kana_font_name)\n");
#endif				/* DEBUG */
		if (ascii_kana_font_struct) {
#ifdef DEBUG
		    dbg_printf("  reload ! ascii_kana_font_name\n");
#endif				/* DEBUG */
		    XFreeFont(display, ascii_kana_font_struct);
		}
		ascii_kana_font_struct = font_struct;
		ascii_kana_font_name = cfnt[ii];
	    } else {
#ifdef DEBUG
		dbg_printf("  not ! ascii_kana_font_name (%s)\n", cfnt[ii]);
#endif				/* DEBUG */
		none = cfnt[ii];
		XFreeFont(display, font_struct);
	    }
	} else {
	    if (font_struct->min_byte1 <= 0x20
		|| font_struct->max_byte1 >= 0x40) {
#ifdef DEBUG
		dbg_printf("  (kanji_font_name)\n");
#endif				/* DEBUG */
		if (kanji_font_struct) {
#ifdef DEBUG
		    dbg_printf("  reload ! kanji_font_name\n");
#endif				/* DEBUG */
		    XFreeFont(display, kanji_font_struct);
		}
		kanji_font_struct = font_struct;
		kanji_font_name = cfnt[ii];
	    } else {
#ifdef DEBUG
		dbg_printf("  gaiji_font_name (%s)\n", cfnt[ii]);
#endif				/* DEBUG */
		if (!gaiji_font_struct) {
#ifdef DEBUG
		    dbg_printf("  reload ! gaiji_font_name\n");
#endif				/* DEBUG */
		    XFreeFont(display, gaiji_font_struct);
		}
		gaiji_font_struct = font_struct;
		gaiji_font_name = cfnt[ii];
	    }
	}
    }

    if (!ascii_kana_font_struct) {
	ErrorMessage(32);
	error = 1;
    }
    if (!kanji_font_struct) {
	ErrorMessage(34);
	error = 1;
    }
    if (error) {
	if (ascii_kana_font_struct)
	    XFreeFont(display, ascii_kana_font_struct);
	if (kanji_font_struct)
	    XFreeFont(display, kanji_font_struct);
	if (gaiji_font_struct)
	    XFreeFont(display, gaiji_font_struct);
#ifdef DEBUG
	dbg_printf("  XLoadQueryFont Error EXIT !!\n");
#endif				/* DEBUG */
	return (-1);
    }
    if (ascii_kana_font_struct->per_char)
	current_preedit_font->fixe_width = 0;
    if (kanji_font_struct->per_char)
	current_preedit_font->fixe_width = 0;
    if (gaiji_font_struct && gaiji_font_struct->per_char)
	current_preedit_font->fixe_width = 0;

    current_preedit_font->ascii_kana_font_struct = ascii_kana_font_struct;
    current_preedit_font->status = FF_FONT_LOAD;
    current_preedit_font->kanji_font_struct = kanji_font_struct;
    current_preedit_font->gaiji_font_struct = gaiji_font_struct;
    current_preedit_font->ascii_kana_font_width
	= ascii_kana_font_struct->max_bounds.width;
    current_preedit_font->max_font_ascent
	= (ascii_kana_font_struct->ascent > kanji_font_struct->ascent) ?
	ascii_kana_font_struct->ascent : kanji_font_struct->ascent;
    current_preedit_font->max_font_descent
	= (ascii_kana_font_struct->descent > kanji_font_struct->descent) ?
	ascii_kana_font_struct->descent : kanji_font_struct->descent;
    current_preedit_font->max_font_height
	= ascii_kana_font_struct->ascent + ascii_kana_font_struct->descent;
    wfont_h = kanji_font_struct->ascent + kanji_font_struct->descent;
    if (current_preedit_font->max_font_height < wfont_h)
	current_preedit_font->max_font_height = wfont_h;
    current_preedit_font->kanji_font_width = kanji_font_struct->max_bounds.width;
    return (0);
}

int
FXFreeFont(display, current_preedit_font)
    Display        *display;
    FXimpFont      *current_preedit_font;
{
    if (!(current_preedit_font->status & FF_FONT_LOAD))
	return (0);

    if (current_preedit_font->ascii_kana_font_struct) {
	XFreeFont(display, current_preedit_font->ascii_kana_font_struct);
	current_preedit_font->ascii_kana_font_struct = 0;
    }
    if (current_preedit_font->kanji_font_struct) {
	XFreeFont(display, current_preedit_font->kanji_font_struct);
	current_preedit_font->kanji_font_struct = 0;
    }
    if (current_preedit_font->gaiji_font_struct) {
	XFreeFont(display, current_preedit_font->gaiji_font_struct);
	current_preedit_font->gaiji_font_struct = 0;
    }
    return (0);
}

int
FXCreateGC(display, window, current_preedit_font,
	   foreground_pixel, background_pixel)
    Display        *display;
    Window          window;
    FXimpFont      *current_preedit_font;
    unsigned long   foreground_pixel, background_pixel;
{
    unsigned long   gcmask;
    XGCValues       gcvalues, reverse_gcvalues;

    gcmask = GCBackground | GCForeground | GCFont;
    gcvalues.foreground = foreground_pixel;
    gcvalues.background = background_pixel;
    reverse_gcvalues.foreground = background_pixel;
    reverse_gcvalues.background = foreground_pixel;

    gcvalues.font = current_preedit_font->ascii_kana_font_struct->fid;
    reverse_gcvalues.font = current_preedit_font->ascii_kana_font_struct->fid;
    current_preedit_font->ascii_kana_gc
	= XCreateGC(display, window, gcmask, &gcvalues);
    current_preedit_font->ascii_kana_reverse_gc
	= XCreateGC(display, window, gcmask, &reverse_gcvalues);

    gcvalues.font = current_preedit_font->kanji_font_struct->fid;
    reverse_gcvalues.font = current_preedit_font->kanji_font_struct->fid;
    current_preedit_font->kanji_gc
	= XCreateGC(display, window, gcmask, &gcvalues);
    current_preedit_font->kanji_reverse_gc
	= XCreateGC(display, window, gcmask, &reverse_gcvalues);

    if (current_preedit_font->gaiji_font_struct) {
	gcvalues.font = current_preedit_font->gaiji_font_struct->fid;
	reverse_gcvalues.font = current_preedit_font->gaiji_font_struct->fid;
	current_preedit_font->gaiji_gc
	    = XCreateGC(display, window, gcmask, &gcvalues);
	current_preedit_font->gaiji_reverse_gc
	    = XCreateGC(display, window, gcmask, &reverse_gcvalues);
    }
    gcvalues.function = GXxor;
    gcvalues.foreground = background_pixel ^ foreground_pixel;
    gcvalues.plane_mask = background_pixel ^ foreground_pixel;
    gcmask = GCForeground | GCFunction | GCPlaneMask;
    current_preedit_font->invert_gc
	= XCreateGC(display, window, gcmask, &gcvalues);
    current_preedit_font->status |= FF_CREATE_GC;
    return (0);
}

int
FXFreeGC(display, current_preedit_font)
    Display        *display;
    FXimpFont      *current_preedit_font;
{
    if (current_preedit_font->status & FF_CREATE_GC) {
#define M_FreeGC( gc ) if( gc ) { XFreeGC( display, gc ); gc = 0; }

	M_FreeGC(current_preedit_font->ascii_kana_gc);
	M_FreeGC(current_preedit_font->kanji_gc);
	M_FreeGC(current_preedit_font->gaiji_gc);
	M_FreeGC(current_preedit_font->ascii_kana_reverse_gc);
	M_FreeGC(current_preedit_font->kanji_reverse_gc);
	M_FreeGC(current_preedit_font->gaiji_reverse_gc);
	M_FreeGC(current_preedit_font->invert_gc);
#undef  M_FreeGC
    }
    return (0);
}

int
FXChangeGC(display, window, current_preedit_font,
	   foreground_pixel, background_pixel)
    Display        *display;
    Window          window;
    FXimpFont      *current_preedit_font;
    unsigned long   foreground_pixel, background_pixel;
{
    unsigned long   gcmask;
    XGCValues       gcvalues, reverse_gcvalues;
    if ((current_preedit_font->status & FF_CREATE_GC) == (unsigned char) 0) {
	return FXCreateGC(display, window, current_preedit_font,
			  foreground_pixel, background_pixel);
    }
    gcmask = GCBackground | GCForeground | GCFont;
    gcvalues.foreground = foreground_pixel;
    gcvalues.background = background_pixel;
    reverse_gcvalues.foreground = background_pixel;
    reverse_gcvalues.background = foreground_pixel;

    gcvalues.font = current_preedit_font->ascii_kana_font_struct->fid;
    reverse_gcvalues.font = current_preedit_font->ascii_kana_font_struct->fid;
    XChangeGC(display, current_preedit_font->ascii_kana_gc, gcmask, &gcvalues);
    XChangeGC(display, current_preedit_font->ascii_kana_reverse_gc, gcmask,
	      &reverse_gcvalues);
    gcvalues.font = current_preedit_font->kanji_font_struct->fid;
    reverse_gcvalues.font = current_preedit_font->kanji_font_struct->fid;
    XChangeGC(display, current_preedit_font->kanji_gc, gcmask, &gcvalues);
    XChangeGC(display, current_preedit_font->kanji_reverse_gc, gcmask,
	      &reverse_gcvalues);

    if (current_preedit_font->gaiji_font_struct) {
	gcvalues.font = current_preedit_font->gaiji_font_struct->fid;
	reverse_gcvalues.font = current_preedit_font->gaiji_font_struct->fid;
	if (current_preedit_font->gaiji_gc) {
	    XChangeGC(display, current_preedit_font->gaiji_gc, gcmask, &gcvalues);
	    XChangeGC(display, current_preedit_font->gaiji_reverse_gc, gcmask,
		      &reverse_gcvalues);
	} else {
	    current_preedit_font->gaiji_gc
		= XCreateGC(display, window, gcmask, &gcvalues);
	    current_preedit_font->gaiji_reverse_gc
		= XCreateGC(display, window, gcmask, &reverse_gcvalues);
	}
    } else {
	if (current_preedit_font->gaiji_gc) {
	    XFreeGC(display, current_preedit_font->gaiji_gc);
	    XFreeGC(display, current_preedit_font->gaiji_reverse_gc);
	    current_preedit_font->gaiji_gc = 0;
	    current_preedit_font->gaiji_reverse_gc = 0;
	}
    }

    gcvalues.function = GXxor;
    gcvalues.foreground = background_pixel ^ foreground_pixel;
    gcvalues.plane_mask = background_pixel ^ foreground_pixel;
    gcmask = GCForeground | GCFunction | GCPlaneMask;
    XChangeGC(display, current_preedit_font->invert_gc, gcmask, &gcvalues);
    return (0);
}

int
FXTextWidth(current_preedit_font, input_data, length)
    FXimpFont      *current_preedit_font;
    ushort         *input_data;
    int             length;
{
    int             j, width = 0;
    for (j = length; j > 0; j--, input_data++) {
	width += (*input_data > 0xff) ?
	    current_preedit_font->kanji_font_width :
	    current_preedit_font->ascii_kana_font_width;
	/** !! Not suported valiable width fonts !! */
    }
    return (width);
}

int
FXTextExtentsArea(current_preedit_font, preedit, input_data, length,
		  ret_area, ret_len, ret_lineno)
    FXimpFont      *current_preedit_font;
    Ximp_PreeditPropRec *preedit;
    ushort         *input_data;
    int             length;
    XRectangle     *ret_area;
    short          *ret_len;
    int            *ret_lineno;
{
    int             j, x2, y2;
    int             sy, over = 0;
    int             area_no = 0;
    int             x, y, width, height;
    int             max_x2;
    int             edit_x1, edit_y1, edit_x2, edit_y2;
    ushort          save, *savep;

    savep = &input_data[length];
    save = *savep;
    *savep = 0xA1A1;

    edit_x1 = preedit->Area.x;
    edit_y1 = preedit->Area.y;
    edit_x2 = edit_x1 + preedit->Area.width;
    edit_y2 = edit_y1 + preedit->Area.height;

    width = 0;
    max_x2 = 0;
    height = current_preedit_font->max_font_height;
    x = preedit->SpotLocation.x;
    y = preedit->SpotLocation.y - current_preedit_font->max_font_ascent;
    if (x < edit_x1)
	x = edit_x1;
    if (x > edit_x2) {
	x = edit_x1;
	y = y + height + preedit->LineSpacing;
    }
    if (y < edit_y1)
	y = edit_y1;
    if (preedit->SpotLocation.y > edit_y2)
	y = edit_y1;
    sy = y;
    x2 = x;
    y2 = y + height;

    ret_len[0] = 0;
    ret_len[1] = 0;
    *ret_lineno = 0;

    for (j = length + 1; j > 0; j--, input_data++) {
	width = FXTextWidth(current_preedit_font, input_data, 1);
	if ((x2 + width) <= edit_x2) {
	    x2 = x2 + width;
	} else {		/* over	width		 */
	    if (*ret_len) {	/* kanji_data exist ?	 */
		(*ret_lineno)++;/* count line no.	 */
		area_no++;
		ret_area->x = x;
		ret_area->y = y;
		ret_area->width = x2 - x;
		ret_area->height = y2 - y;
		ret_area++;	/* next_item ret_area (max 2) */
		if (area_no >= 2) {
		    *savep = save;
		    return (area_no);	/* area over !!		 */
		}
		ret_len++;
	    }
	    x = edit_x1;
	    y = y2 + preedit->LineSpacing;
	    x2 = x + width;
	    y2 = y2 + height + preedit->LineSpacing;
	    if (y2 > edit_y2) {
		y = edit_y1;
		y2 = y + height;
	    }
	}
	(*ret_len)++;		/* count String length	 */
    }

    if (*ret_len) {		/* kanji_data exist ?	 */
	(*ret_lineno)++;	/* count line no.	 */
	area_no++;
	ret_area->x = x;
	ret_area->y = y;
	ret_area->width = x2 - x;
	ret_area->height = y2 - y;
    }
    *savep = save;
    return (area_no);
}

FXDrawImageString(display, window, current_preedit_font,
		  reverse, x, y, buff, length)
    Display        *display;
    Window          window;
    FXimpFont      *current_preedit_font;
    int             reverse;
    int             x, y;
    ushort         *buff;
    int             length;
{
    GC              gc, ascii_kana_gc, kanji_gc, gaiji_gc;
    int             sv_x = x;
    unsigned int    c2, cmask, ctype;
    ushort         *buff_end;
    unsigned char  *outp, *out_end;
#define outbufsize 256
    unsigned char   outbuf[outbufsize];

    if (!reverse) {
	ascii_kana_gc = current_preedit_font->ascii_kana_gc;
	kanji_gc = current_preedit_font->kanji_gc;
	gaiji_gc = current_preedit_font->gaiji_gc;
    } else {
	ascii_kana_gc = current_preedit_font->ascii_kana_reverse_gc;
	kanji_gc = current_preedit_font->kanji_reverse_gc;
	gaiji_gc = current_preedit_font->gaiji_reverse_gc;
    }

    outp = outbuf;
    out_end = outbuf + outbufsize - 2;
    buff_end = buff + length;

    while (buff < buff_end) {
	ctype = *buff & 0x8080;
	switch (ctype) {
	case 0x0000:
	    gc = ascii_kana_gc;
	    cmask = 0x007f;
	    break;
	case 0x0080:
	    gc = ascii_kana_gc;
	    cmask = 0x00ff;
	    break;
	case 0x8080:
	    gc = kanji_gc;
	    cmask = 0x7f7f;
	    break;
	case 0x8000:
	    gc = gaiji_gc;
	    cmask = 0x7f7f;
	    break;
	}

	if (!gc) {
	    while ((buff < buff_end)
		   && ((*buff & 0x8080) == ctype)) {
		buff++;
	    }
	} else
	    while ((buff < buff_end)
		   && (outp < out_end)
		   && (((c2 = *buff) & 0x8080) == ctype)) {
		if (cmask & 0x7f00) {
		    *outp++ = (c2 & cmask) >> 8;
		    *outp++ = c2 & 0x007f;
		} else {
		    *outp++ = c2 & cmask;
		}
		buff++;
	    }

	length = outp - outbuf;
	if (length > 0) {
	    if (cmask & 0x7f00) {
		length = length / 2;
		XDrawImageString16(display, window, gc, x, y,
				   (XChar2b *) outbuf, length);
		x += current_preedit_font->kanji_font_width * length;
	    } else {
		XDrawImageString(display, window, gc, x, y,
				 (char *) outbuf, length);
		x += current_preedit_font->ascii_kana_font_width * length;
	    }
	    outp = outbuf;
	}
    }
    return (x - sv_x);
}

int 
FXDefaultError(display, event)
    Display        *display;
    XErrorEvent    *event;
{
#define	MSG_SIZE	64
    char            xerr_msg[MSG_SIZE];
    extern int      verbose_flag;
    if (verbose_flag) {
	XGetErrorText(display, event->error_code, xerr_msg, MSG_SIZE);
	fprintf(stderr, "%s: X Error: %s\n", command_name, xerr_msg);
	fprintf(stderr, "  req.: %d - %d, res.ID: 0x%x, error #%d - #%d\n",
		event->request_code, event->minor_code,
		event->resourceid, event->serial, display->request);
    }
}

#endif	/* USE_X11R5_LIB */
