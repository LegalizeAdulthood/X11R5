/* @(#)Display.c	2.4 91/08/30 15:20:55 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

/*  Xfeoak : Input Server (kana-kanji covertion front end)	*/
/*         : Version 2.0					*/

#ifndef lint
static char sccs_id[] = "@(#)Display.c	2.4 91/08/30 15:20:55 FUJITSU LIMITED.";
#endif

#include "Xfeoak.h"
#include <X11/Xutil.h>

int
SetFocusClient()
    {
    FXimp_Client	*old_client;
#ifdef DEBUG
    dbg_printf(" <SetFocusClient> current_client=%x\n", current_client );
#endif DEBUG
    current_preedit_font = current_client->preedit_font;

    if( focus_in_client && (focus_in_client->style & XIMStatusNothing) ) {
	old_client = focus_in_client;
	}
    else {
	old_client = root_client;
	}
    if( current_client->henkan_mode == 1 || old_client != root_client)
    if( old_client->status_visible ) {
	old_client->status_visible = 0;
	if( current_client->status_window != old_client->status_window )
            XUnmapWindow( display, old_client->status_window);
	if( current_client->henkan_mode == 1 ) {
	    if( control_modifiers_flag ) 
		ChangeShiftMode( 0);
	    }
        }
    if( old_client && old_client->preedit_visible ) {
        old_client->preedit_visible = 0;
        XUnmapWindow( display, old_client->preedit_window);
        if( old_client->style & XIMPreeditPosition) {
            XUnmapWindow( display, old_client->preedit_window2);
	    }
	}

    if( current_client->henkan_mode == 1 ) {
        if( current_client->status_visible == 0 ) {
            if( control_modifiers_flag ) 
	        ChangeShiftMode( 1);
    	    XMapRaised( display, current_client->status_window);
	    current_client->status_visible = 1;
            }
	MapPreeditWindow();
        focus_in_client = current_client;
        DisplayStatus( current_client );
        }
    else 
    if( root_client->status_visible == 0 ) {
        XMapRaised( display, root_client->status_window); 
	root_client->status_visible = 1;
        focus_in_client = (FXimp_Client *)0;
        }

    return( 0);
    }

int
UnsetFocusClient()
    {
    if( focus_in_client == (FXimp_Client *)0 ) {
	return( 0);
	}
#ifdef DEBUG
    dbg_printf(" <UnsetFocusClient> current_client=%x\n", current_client );
#endif DEBUG
    if( current_client->status_visible != 0 ) {
        if( current_client != root_client ) {
	    if( current_client->status_window != root_client->status_window )
                XUnmapWindow( display, current_client->status_window);
	    current_client->status_visible = 0;
	    if( current_client == focus_in_client ) {
		if( current_client->henkan_mode == 1 ) {
		     if( control_modifiers_flag ) 
			ChangeShiftMode( 0);
		     }
                XMapRaised( display, root_client->status_window); 
	        root_client->status_visible = 1;
                focus_in_client = (FXimp_Client *)0;
		}
            }
        }
    UnmapPreeditWindow();
    if( focus_in_client == (FXimp_Client *)0 )
        DisplayStatus( root_client );
    return( 0);
    }

int
MapPreeditWindow()
    {
    if( current_client->henkan_mode == 0
     || ( current_client->preedit_visible != 0
       && current_client->prot_type == FPRT_XIM ) ) 
	return(0);

    if( focus_in_client && focus_in_client->preedit_visible ) {
        focus_in_client->preedit_visible = 0;
        XUnmapWindow( display, focus_in_client->preedit_window);
        if( focus_in_client->style & XIMPreeditPosition) {
            XUnmapWindow( display, focus_in_client->preedit_window2);
	    }
	}
	
    if( current_client->select_kanji_mod == 0 ) {
        if( current_client->style & XIMPreeditPosition) {
            current_client->preedit_visible = 1;
            XMapRaised( display, current_client->preedit_window);
	    if( current_client->used_preedit_window_no >= 2 )
                XMapRaised( display, current_client->preedit_window2);
#ifdef DEBUG
    	    dbg_printf("  <MapPreeditWindow> XIMPreeditPosition current_client=%x preedit_window=%x\n", current_client, current_client->preedit_window );
#endif DEBUG
	    }
        else {
            current_client->preedit_visible = 1;
            XMapRaised( display, current_client->preedit_window);
#ifdef DEBUG
    	    dbg_printf("  <MapPreeditWindow> etc. current_client=%x preedit_window=%x\n", current_client, current_client->preedit_window );
#endif DEBUG
	    }
	}
    return(0);
    }

int
UnmapPreeditWindow()
    {
    if( current_client->preedit_visible == 0 )
	return(0);

#ifdef DEBUG
    dbg_printf("  <UnmapPreeditWindow> current_client=%x preedit_window=%x\n", current_client, current_client->preedit_window );
#endif DEBUG
    current_client->preedit_visible = 0;
    XUnmapWindow( display, current_client->preedit_window);
    if(current_client->style & XIMPreeditPosition) {
        XUnmapWindow( display, current_client->preedit_window2);
	}
    return(0);
    }

int
Redisplay()
    {
    if(current_client->select_kanji_mod == 0) {
#ifdef DEBUG2
    	dbg_printf(" <Redisplay> current_client=%x\n", current_client );
#endif DEBUG
        DisplayOutputBuff(0);
	DisplayCursor();
	}
    return(0);
    }

int
DisplayOutputBuff()
    {
    int	lx, ly, width;
    int	length, reverseLength;
    Ximp_PreeditPropRec	*current_preedit;
#ifdef DEBUG2
    dbg_printf(" <DisplayOutputBuff> current_client=%x\n", current_client );
#endif DEBUG
    if( current_client->style & XIMPreeditPosition) {
	ResizePreeditArea();
	DisplayOutputBuffInPosition();
	return( 0 );
	}
    else
    if(current_client->style & XIMPreeditNothing) 
	current_preedit = root_client->preedit;
    else
	current_preedit = root_client->preedit;

    XClearWindow( display, current_client->preedit_window);
    length = current_client->reverse_colums;
    lx = 0;	
    ly = (current_preedit->Area.height - current_preedit_font->max_font_height + 1) / 2;
    ly += current_preedit_font->max_font_ascent;
    if( length > 0 ) {	
        lx = FXDrawImageString( display, current_client->preedit_window,
		current_preedit_font, 0, lx, ly,
		&(current_client->output_buff[0]), length);
	}
    length = current_client->last_colums - current_client->reverse_colums;
    if( length > 0 ) {	
	lx = FXTextWidth( current_preedit_font, current_client->output_buff,
			  current_client->reverse_colums); 
	FXDrawImageString( display, current_client->preedit_window,
		current_preedit_font, 1, lx, ly,
		&(current_client->output_buff[current_client->reverse_colums]),
		length);
	}
    return( 0 );
    }

int
ResizePreeditArea()
    {
    int		lx;
    Ximp_PreeditPropRec	*current_preedit;
    XRectangle	preeddit_area[2], *current_area;
    short	preedit_line_length[2];
    int		area_no, line_no;
    Window	preedit_window[2];

    if( current_client->check_preedit_area_size ) {
        Ximp_PreeditPropRec preedit;
        current_preedit = current_client->preedit;
        preedit.Area.width  = 0;
        preedit.Area.height = 0;
        CheckPreeditWidowSize( &preedit );
        if( ( preedit.Area.width  != current_preedit->Area.width )
         || ( preedit.Area.height != current_preedit->Area.height ) ) {
    	    current_preedit->Area.width  = preedit.Area.width;
	    current_preedit->Area.height = preedit.Area.height;
	    current_client->input_flag = FINF_MODY;
	    }
	}

    if((current_client->style & XIMPreeditPosition)
    && (current_client->input_flag != FINF_NONE) ) {
        area_no = FXTextExtentsArea( current_preedit_font,
				     current_client->preedit,
				     current_client->output_buff,
				     current_client->last_colums,
				     preeddit_area,
				     preedit_line_length, &line_no);
	if( area_no == 0 ) {
	    return( -1 );
	    }

        preedit_window[0] = current_client->preedit_window;
        preedit_window[1] = current_client->preedit_window2;

	for( lx = 0; lx < area_no; lx++ ) {
	    if( lx > current_client->used_preedit_window_no 
	    || (preeddit_area[lx].x      != current_client->preedit_area[lx].x)
	    || (preeddit_area[lx].y      != current_client->preedit_area[lx].y)
	    || (preeddit_area[lx].width  != current_client->preedit_area[lx].width)
	    || (preeddit_area[lx].height != current_client->preedit_area[lx].height)) {
	        XMoveResizeWindow( display, preedit_window[lx],
			preeddit_area[lx].x,     preeddit_area[lx].y,
			preeddit_area[lx].width, preeddit_area[lx].height);
		current_client->preedit_area[lx] = preeddit_area[lx];
		}
            if( current_client->henkan_mode == 1 ) {
		current_client->preedit_visible = 1;
	        XMapRaised( display, preedit_window[lx] );
		}
	    else {
	        XRaiseWindow( display, preedit_window[lx] );
		}
	    }

	for( lx = area_no; lx < 2; lx++ ) {
	    XUnmapWindow( display, preedit_window[lx] );
	    bzero( &preeddit_area[lx], sizeof(preeddit_area[0]) );
	    }
	 
        current_client->input_flag = FINF_NONE;
	XFlush( display );
	current_client->used_preedit_window_no = area_no;
	current_client->preedit_line_no = line_no;
	for( lx = 0; lx < 2; lx++ ) {
	    current_client->preedit_line_length[lx] = preedit_line_length[lx];
	    }
	}
    return(0);
    }

int
DisplayOutputBuffInPosition()
	{
	int	ip, line, lx, ly, width, height;
	int	display_length, area_length, draw_length, current_colums;
	Window	preedit_window[3];

        preedit_window[0] = current_client->preedit_window;
        preedit_window[1] = current_client->preedit_window2;
	height = current_preedit_font->max_font_height
	       + current_client->preedit->LineSpacing;

	for( ip = 0; ip < current_client->used_preedit_window_no; ip++ ) {
	    XClearWindow( display, preedit_window[ip] );
	    }

	lx = 0;	
	ly = current_preedit_font->max_font_ascent;
	ip = 0;
	line = 0;
	area_length = current_client->preedit_line_length[ ip ];

	display_length = current_client->reverse_colums;
	current_colums  = 0;
	for( ; ( display_length > 0) && (ip < current_client->used_preedit_window_no); ) {
	    if( display_length > area_length) 
	        draw_length = area_length;
	    else
	        draw_length = display_length;

	    lx = FXDrawImageString( display, preedit_window[ip],
	    		current_preedit_font, 0, lx, ly,
	    		&(current_client->output_buff[current_colums]),
			draw_length);
	    current_colums += draw_length;
	    display_length -= draw_length;
	    area_length -= draw_length;
	    if( area_length <= 0 ) {
	        lx = 0;
	        ip++;
	        if( ip < 2 )
	       	    area_length = current_client->preedit_line_length[ ip ];
		}
	    }

	display_length = current_client->last_colums
		       - current_client->reverse_colums;
	for( ; ( display_length > 0) && (ip < current_client->used_preedit_window_no); ) {
	    if( display_length > area_length) 
	        draw_length = area_length;
	    else
	        draw_length = display_length;

	    lx = FXDrawImageString( display, preedit_window[ip],
	    		current_preedit_font, 1, lx, ly,
			&(current_client->output_buff[current_colums]),
			draw_length);
	    current_colums += draw_length;
	    display_length -= draw_length;
	    area_length -= draw_length;
	    if( area_length <= 0 ) {
	        lx = 0;
	        ip++;
	        if( ip < 2 )
	       	    area_length = current_client->preedit_line_length[ ip ];
		}
	    }
	}

int
DisplayStatus( work_client )
    FXimp_Client *work_client;
    {
    int		xx, yy, ww, length;
    FXimpFont	*current_status_font;
    Ximp_StatusPropRec	*current_status;
    FXimp_Client	*current_client;
    ushort	status_message[21];
    extern int	default_border_width;

    current_client = work_client;
    if(( focus_in_client == (FXimp_Client *)0 )
    || (current_client->select_kanji_mod == 0) ) {
	current_status_font = current_client->status_font;
	if( current_client->style & XIMStatusArea ) {
	    current_status = current_client->status;
	    }
	else{
	    current_status = root_client->status;
	    }
	XClearWindow( display, current_client->status_window);
        length = GetStatusMessage( work_client, status_message );
        ww = FXTextWidth( current_status_font, status_message, length );
        xx = current_status->Area.width - ww - default_border_width;
        yy = (current_status->Area.height - current_status_font->max_font_height + 1) / 2;
        yy += current_status_font->max_font_ascent;
        FXDrawImageString( display, current_client->status_window,
			   current_status_font, 0, xx, yy,
			   status_message, length );
	}
    else {
	DisplaySelectKanji();
	DisplaySelectCursor(current_client->select_kanji_no);
	}
    return(0);
    }

int
DisplayCursor()
    {
    int 	width, hight;
    int 	cx, cy;
    Window	preedit_window;
    Ximp_PreeditPropRec	*current_preedit;

    if( focus_in_client != current_client )
	return( 0 );

    if(current_client->style & XIMPreeditPosition) {
        int 	colums, length, lno, wno;
	current_preedit = current_client->preedit;

	length = current_client->current_colums;
	colums = current_client->preedit_line_length[0];
	if( length >= colums ) {
	    length -= colums;
	    preedit_window = current_client->preedit_window2;
	    }
	else {
	    preedit_window = current_client->preedit_window;
	    }

	colums = current_client->current_colums - length;
        cx = FXTextWidth( current_preedit_font,
			 &(current_client->output_buff[colums]), length);
	cy = 0;
	}
    else {
	current_preedit = root_client->preedit;
        cx = FXTextWidth( current_preedit_font, current_client->output_buff,
			  current_client->current_colums);
        cy = (current_preedit->Area.height - current_preedit_font->max_font_height + 1) / 2;
	preedit_window = current_client->preedit_window;
	}
    hight = current_preedit_font->max_font_height;

    if(current_client->current_colums == current_client->last_colums ) {
         width = (current_client->hankakumode == 0) ?
		  current_preedit_font->kanji_font_width :
		  current_preedit_font->ascii_kana_font_width;
    } else
	 width = (current_client->output_buff[current_client->current_colums] > 0xff) ?
		  current_preedit_font->kanji_font_width :
		  current_preedit_font->ascii_kana_font_width;
    width-=2;
    hight-=2;
    cx++;
    cy++;
    XFillRectangle( display, preedit_window, current_preedit_font->invert_gc,
			cx, cy, width, hight);
    return(0);
    }

int
CreatePreeditWindow()
    {
    Ximp_PreeditPropRec	*reference_preedit, *current_preedit;
    int		rc;
    Window	status_window;

    if( (current_client->window_type & FC_SAME_ROOT_PREEDIT)
			  	    == FC_SAME_ROOT_PREEDIT ) {
	*current_client->preedit_font    = *root_client->preedit_font;
	current_client->preedit_font->status = 0;
	current_client->preedit_fontname = root_client->preedit_fontname;
        current_client->preedit_window   = root_client->preedit_window;
#ifdef DEBUG
	dbg_printf("CreatePreeditWindow : copy root preedit_window=%x\n",
			current_client->preedit_window );
#endif DEBUG
	return( 0 );
	}

    if(( current_client->style & XIMPreeditCallbacks )
    || ( current_client->style & XIMPreeditNone )) {
	return( 0 );
	}

    current_preedit_font  = current_client->preedit_font;
    if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_FONT) ) {
        rc = FXLoadQueryBaseFont( display, current_preedit_font,
			current_client->preedit_fontname );
        if( rc ) {
	    current_client->window_type |= FC_SAME_ROOT_PREEDIT_FONT;
	    *current_preedit_font = *root_client->preedit_font;
	    current_preedit_font->status = 0;
	    }
	}
    else {
        *current_preedit_font = *root_client->preedit_font;
        current_preedit_font->status = 0;
	}

    current_preedit  = current_client->preedit;
    if( current_preedit->Foreground == current_preedit->Background ) {
	current_preedit->Foreground = root_client->preedit->Foreground;
	current_preedit->Background = root_client->preedit->Background;
	}

    if( current_client->style & XIMPreeditNothing ) {
	reference_preedit = root_client->preedit;
	status_window = root_client->window;
	}
    else {
	reference_preedit = current_preedit;
	status_window = current_client->window;
	}

    if( current_client->style & (XIMPreeditNothing | XIMPreeditArea) ) {
        if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_WINDOW) ) {
            current_client->preedit_window =
	        XCreateSimpleWindow( display, status_window,
		  reference_preedit->Area.x,     reference_preedit->Area.y,
		  reference_preedit->Area.width, reference_preedit->Area.height,
		  0, current_preedit->Foreground, current_preedit->Background);
	    XStoreName( display, current_client->preedit_window, "XIMPreedit" );
	    ChangeWindowAttributes( display, current_client->preedit_window, current_preedit );
	    }
	else
	    current_client->preedit_window = root_client->preedit_window;
	}

    if( current_client->style & XIMPreeditPosition ) {
	current_client->input_flag = FINF_MODY;
	CreatePreeditPositionWindow( current_client, current_preedit_font );
	}

    if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_GC) ) {
        FXCreateGC( display, current_client->preedit_window, current_preedit_font,
		current_preedit->Foreground, current_preedit->Background );
	}
    XSelectInput( display, current_client->preedit_window, ExposureMask);
#ifdef DEBUG
    dbg_printf("CreatePreeditWindow : preedit_window=%x status_window=%x\n",
			current_client->preedit_window, status_window );
#endif DEBUG
    return(0);
    }

int
ChangePreeditWindow(preedit)
    Ximp_PreeditPropRec *preedit;
    {
    Ximp_PreeditPropRec	*current_preedit;
    int			rc;
    Window		status_window;

    if( (current_client->window_type & FC_SAME_ROOT_PREEDIT)
			  	    == FC_SAME_ROOT_PREEDIT ) {
        return( 0 );
        }

    if(( current_client->style & XIMPreeditCallbacks )
    || ( current_client->style & XIMPreeditNone )) {
	return( 0 );
	}

    current_preedit_font = current_client->preedit_font;
    current_preedit  = current_client->preedit;
    if( current_preedit->Foreground == current_preedit->Background ) {
	current_preedit->Foreground = root_client->preedit->Foreground;
	current_preedit->Background = root_client->preedit->Background;
	}

    if( current_client->style & XIMPreeditArea ) {
	if(( preedit->Area.x      != current_preedit->Area.x )
	|| ( preedit->Area.y      != current_preedit->Area.y )
	|| ( preedit->Area.width  != current_preedit->Area.width )
	|| ( preedit->Area.height != current_preedit->Area.height ) ) {
	    XMoveResizeWindow( display, current_client->preedit_window,
		preedit->Area.x,     preedit->Area.y,
		preedit->Area.width, preedit->Area.height);
	    current_preedit->Area = preedit->Area;
	    }
	}
    else
    if( current_client->style & XIMPreeditPosition ) {
        CheckPreeditWidowSize( preedit );
	if(( preedit->Area.x         != current_preedit->Area.x )
	|| ( preedit->Area.y         != current_preedit->Area.y )
	|| ( preedit->Area.width     != current_preedit->Area.width )
	|| ( preedit->Area.height    != current_preedit->Area.height )
	|| ( preedit->SpotLocation.x != current_preedit->SpotLocation.x )
	|| ( preedit->SpotLocation.y != current_preedit->SpotLocation.y )
	|| ( preedit->LineSpacing    != current_preedit->LineSpacing    )) {
	    if( preedit->SpotLocation.x != 0 || preedit->SpotLocation.y != 0 )
	        current_preedit->SpotLocation = preedit->SpotLocation;
	    current_preedit->LineSpacing  = preedit->LineSpacing;
	    current_preedit->Area         = preedit->Area;
	    current_client->input_flag = FINF_MODY;
	    ResizePreeditArea();
	    }
	}

    if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_GC) )
    if( current_client->style &
	(XIMPreeditNothing | XIMPreeditArea | XIMPreeditPosition) ) {
	if(( preedit->Background  != current_preedit->Background )
	|| ( preedit->Foreground  != current_preedit->Foreground )) {
	    current_preedit->Background = preedit->Background;
	    current_preedit->Foreground = preedit->Foreground;
	    XSetWindowBackground( display, current_client->preedit_window,
					preedit->Background );
    	    FXChangeGC( display, current_client->preedit_window,
			current_preedit_font, current_preedit->Foreground,
			current_preedit->Background );
	    }
	current_preedit->Colormap = preedit->Colormap;
	current_preedit->Bg_Pixmap = preedit->Bg_Pixmap;
	current_preedit->Cursor = preedit->Cursor;
	current_preedit->AreaNeeded = preedit->AreaNeeded;
	ChangeWindowAttributes( display, current_client->preedit_window,
				current_preedit );
	}
    XClearArea( display, current_client->preedit_window, 0, 0, 0, 0, True );
    return(0);
    }

int
ChangePreeditFont()
    {
    char	*work_ascii_kana_font_name, *work_kanji_font_name,
		*work_gaiji_font_name;
    Ximp_PreeditPropRec	*reference_preedit, *current_preedit;
    int		rc;
    Window	status_window;

    if( current_client->window_type & ( FC_SAME_ROOT_PREEDIT_GC
				      | FC_SAME_ROOT_PREEDIT_FONT) ) {
        return( 0 );
        }
    if(( current_client->style & XIMPreeditCallbacks )
    || ( current_client->style & XIMPreeditNone )) {
	return( 0 );
	}

    current_preedit_font = current_client->preedit_font;
    FXFreeFont( display, current_preedit_font );
    rc = FXLoadQueryBaseFont( display, current_preedit_font,
			current_client->preedit_fontname );
    if( rc ) {
	*current_preedit_font = *root_client->preedit_font;
	current_preedit_font->status = 0;
	}
    current_preedit = current_client->preedit;
    FXChangeGC( display, current_client->preedit_window, current_preedit_font,
		current_preedit->Foreground, current_preedit->Background );
    XClearArea( display, current_client->preedit_window, 0, 0, 0, 0, True );
    return( 0 );
    }


int
CreateStatusWindow()
    {
    Ximp_StatusPropRec	*root_status, *current_status;
    FXimpFont	*current_status_font;
    Window	parent;
    int		rc;

    if( current_client->style & XIMStatusNothing ) {
	/* Deiconified Root */
	XWMHints    wm_hints;
	bzero( &wm_hints, sizeof(XWMHints));
	wm_hints.initial_state = NormalState;
	wm_hints.flags = StateHint;
	XSetWMHints( display, root_client->window, &wm_hints);
	XMapRaised( display, root_client->window);
	}

    if( (current_client->window_type & FC_SAME_ROOT_STATUS)
			  	    == FC_SAME_ROOT_STATUS ) {
	*current_client->status_font    = *root_client->status_font;
	current_client->status_font->status = 0;
	current_client->status_fontname = root_client->status_fontname;
        current_client->status_window   = root_client->status_window;
#ifdef DEBUG
	dbg_printf("CreateStatusWindow : copy root status_window=%x\n",
			current_client->status_window );
#endif DEBUG
	return( 0 );
	}

    if(( current_client->style & XIMStatusCallbacks )
    || ( current_client->style & XIMStatusNone )) {
	return( 0 );
	}

    current_status_font = current_client->status_font;
    current_status = current_client->status;
    root_status = root_client->status;
    if( current_status->Foreground == current_status->Background ) {
	current_status->Foreground = root_client->status->Foreground;
	current_status->Background = root_client->status->Background;
	}

    if( !(current_client->window_type & FC_SAME_ROOT_STATUS_FONT) ) {
        rc = FXLoadQueryBaseFont( display, current_status_font,
			current_client->status_fontname );
        if( rc != 0 ) {
	    current_client->window_type |= FC_SAME_ROOT_STATUS_FONT;
	    *current_status_font = *root_client->status_font;
	    current_status_font->status = 0;
	    }
	}
    else {
	*current_status_font = *root_client->status_font;
	current_status_font->status = 0;
	}

    if( current_client->style & XIMStatusNothing ) {
        if( !(current_client->window_type & FC_SAME_ROOT_STATUS_WINDOW) ) {
            current_client->status_window =
	      XCreateSimpleWindow( display, root_client->window,
		root_status->Area.x,	 root_status->Area.y,
		root_status->Area.width, root_status->Area.height,
		0, current_status->Foreground, current_status->Background);
	    XStoreName( display, current_client->status_window, "XIMStatusNothing" );
            ChangeWindowAttributes( display, current_client->status_window,
				current_status );
            FXCreateGC( display, current_client->status_window, current_status_font,
		current_status->Foreground, current_status->Background );
#ifdef DEBUG
	    dbg_printf("CreateStatusWindow : XIMStatusNothing status_window=%x parent\n",
			current_client->status_window, root_client->window );
#endif DEBUG
	    }
	else {
	    current_client->status_window = root_client->status_window;
#ifdef DEBUG
	    dbg_printf("CreateStatusWindow : copy root status_window=%x\n",
			current_client->status_window );
#endif DEBUG
	    }
	}
    else
    if( current_client->style & XIMStatusArea ) {
	if( current_client->ext_ui_statuswindow && current_status->window ) {
	    parent = current_status->window;
	    }
	else {
	    parent = current_client->window;
	    }
        current_client->status_window =
	    XCreateSimpleWindow( display, parent,
		current_status->Area.x,	current_status->Area.y,
		current_status->Area.width, current_status->Area.height,
		0, current_status->Foreground, current_status->Background);
	XStoreName( display, current_client->status_window, "XIMStatusArea" );
        ChangeWindowAttributes( display, current_client->status_window,
				current_status );
        FXCreateGC( display, current_client->status_window, current_status_font,
		current_status->Foreground, current_status->Background );
#ifdef DEBUG
    	dbg_printf("CreateStatusWindow : status_window=%x parent\n",
			current_client->status_window, parent );
#endif DEBUG
	}

    XSelectInput( display, current_client->status_window, ExposureMask);
    return( 0 );
    }

int
ChangeStatusWindow( status )
    Ximp_StatusPropRec	*status;
    {
    Ximp_StatusPropRec	*current_status;
    FXimpFont	*current_status_font;
    Window	parent;
    int		rc;

    if( (current_client->window_type & FC_SAME_ROOT_STATUS)
			  	    == FC_SAME_ROOT_STATUS ) {
        return( 0 );
        }
    if(( current_client->style & XIMStatusCallbacks )
    || ( current_client->style & XIMStatusNone )) {
	return( 0 );
	}

    current_status_font = current_client->status_font;
    current_status = current_client->status;
    if( current_status->Foreground == current_status->Background ) {
	current_status->Foreground = root_client->status->Foreground;
	current_status->Background = root_client->status->Background;
	}

    if( current_client->style & XIMStatusArea ) {
	if( current_client->ext_ui_statuswindow
	&& ( status->window || status->window != current_status->window ) ) {
	    current_status->window = status->window;
	    if( status->window ) {
	        parent = status->window;
	        current_status->Area.x = status->Area.x;
	        current_status->Area.y = status->Area.y;
		}
	    else {
	        parent = current_client->window;
		}
	    XReparentWindow( display, current_client->status_window,
		parent, current_status->Area.x, current_status->Area.y );
	    }

	if(( status->Area.x      != current_status->Area.x)
	|| ( status->Area.y      != current_status->Area.y)
	|| ( status->Area.width  != current_status->Area.width)
	|| ( status->Area.height != current_status->Area.height)) {
	    current_status->Area.x      = status->Area.x;
	    current_status->Area.y      = status->Area.y;
	    current_status->Area.width  = status->Area.width;
	    current_status->Area.height = status->Area.height;
	    XMoveResizeWindow( display, current_client->status_window,
		status->Area.x,	    status->Area.y,
		status->Area.width, status->Area.height );
	    }
	}

    if( !(current_client->window_type & FC_SAME_ROOT_STATUS_GC) )
    if( current_client->style & ( XIMStatusNothing | XIMStatusArea )) {
	if(( status->Foreground != current_status->Foreground )
	|| ( status->Background != current_status->Background )) {
	    current_status->Foreground = status->Foreground;
	    current_status->Background = status->Background;
	    XSetWindowBackground( display, current_client->status_window, status->Background );
    	    FXCreateGC( display, current_client->status_window, current_status_font,
		current_status->Foreground, current_status->Background );
	    }
	current_status->Colormap = status->Colormap;
	current_status->Bg_Pixmap = status->Bg_Pixmap;
	current_status->LineSpacing = status->LineSpacing;
	current_status->Cursor = status->Cursor;
        ChangeWindowAttributes( display, current_client->status_window, current_status );
	}
    XClearArea( display, current_client->status_window, 0, 0, 0, 0, True );
    return( 0 );
    }

int
ChangeStatusFont()
    {
    FXimpFont	*current_status_font;
    Ximp_StatusPropRec	*current_status;
    int		rc;

    if( current_client->window_type & ( FC_SAME_ROOT_STATUS_GC
				      | FC_SAME_ROOT_STATUS_FONT) ) {
        return( 0 );
        }
    if(( current_client->style & XIMStatusCallbacks )
    || ( current_client->style & XIMStatusNone )) {
	return( 0 );
	}

    current_status_font = current_client->status_font;
    current_status = current_client->status;
    FXFreeFont( display, current_status_font );
    rc = FXLoadQueryBaseFont( display, current_status_font,
			current_client->status_fontname );
    if( rc != 0 ) {
	*current_status_font = *root_client->status_font;
	current_status_font->status = 0;
	}
    FXChangeGC( display, current_client->status_window, current_status_font,
	current_status->Foreground, current_status->Background );
    XClearArea( display, current_client->status_window, 0, 0, 0, 0, True );
    return( 0 );
    }

int
ChangeWindowAttributes( display, window, preedit )
    Display	*display;
    Window	window;
    Ximp_PreeditPropRec	*preedit;
    {
    unsigned long valuemask;
    XSetWindowAttributes  attributes;

    valuemask = 0;
    bzero( &attributes, sizeof(attributes) );
    if( preedit->Bg_Pixmap ) {
	attributes.background_pixmap = preedit->Bg_Pixmap;
	valuemask |= CWBackPixmap;
	}
    if( preedit->Cursor ) {
	attributes.cursor = preedit->Cursor;
	valuemask |= CWCursor;
	}
    if( preedit->Colormap ) {
	attributes.colormap = preedit->Colormap;
	valuemask |= CWColormap;
	}
    if( valuemask ) {
	XChangeWindowAttributes( display, window, valuemask, &attributes );
	}
    return( 0 );
    }

int
CreatePreeditPositionWindow( current_client, current_preedit_font )
    FXimp_Client	*current_client;
    FXimpFont		*current_preedit_font;
    {
    Ximp_PreeditPropRec	*current_preedit;
    Window		focus_window;
    unsigned long	fgpixel, bgpixel;
    long		xx, yy, ww, hh;

    current_preedit = current_client->preedit;
    focus_window = current_client->focus_window;

    xx = current_preedit->SpotLocation.x;
    yy = current_preedit->SpotLocation.y - current_preedit_font->max_font_ascent;
    ww = current_preedit_font->kanji_font_width;
    hh = current_preedit_font->max_font_height;

    fgpixel = current_preedit->Foreground;
    bgpixel = current_preedit->Background;

    current_client->preedit_window = XCreateSimpleWindow( display, focus_window,
			xx, yy, ww, hh, 0, fgpixel, bgpixel);

    current_client->preedit_window2 = XCreateSimpleWindow( display, focus_window,
			xx, yy, ww, hh, 0, fgpixel, bgpixel);

    XStoreName( display, current_client->preedit_window, "XIMPreeditPosition" );
    XStoreName( display, current_client->preedit_window2, "XIMPreeditPosition2" );
    ChangeWindowAttributes( display, current_client->preedit_window,  current_preedit );
    ChangeWindowAttributes( display, current_client->preedit_window2, current_preedit );
    CheckPreeditWidowSize( current_preedit );
    return( 0 );
    }

int
CheckPreeditWidowSize( preedit )
    Ximp_PreeditPropRec *preedit;
    {
    Window	root_return;
    int		x_return, y_return;
    unsigned int  width_return, height_return,
		border_width_return, depth_return;
    if( preedit->Area.width == 0
    &&  preedit->Area.height == 0 ) {
        current_client->check_preedit_area_size = 1;
	if( XGetGeometry( display, (Drawable)current_client->focus_window,
		&root_return, &x_return, &y_return, &width_return, &height_return,
		&border_width_return, &depth_return ) ) {
	    preedit->Area.width  = width_return;
	    preedit->Area.height = height_return;
	    return(0);
	    }
	return(-1);
	}
    return(0);
    }

int
DestroyPreeditWindow( flag)
    int	flag;
    {
    if( (current_client->window_type & FC_SAME_ROOT_PREEDIT)
			  	    == FC_SAME_ROOT_PREEDIT ) {
        return( 0 );
        }

    current_preedit_font = current_client->preedit_font;
    if( current_client->style & XIMPreeditNothing ) {
    	if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_WINDOW) )
	    XDestroyWindow( display, current_client->preedit_window );
	}

    if( flag == 0 )
    if( current_client->style & XIMPreeditPosition ) {
	XDestroyWindow( display, current_client->preedit_window );
	XDestroyWindow( display, current_client->preedit_window2 );
	}

    if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_GC) )
        FXFreeGC( display, current_preedit_font );
    if( !(current_client->window_type & FC_SAME_ROOT_PREEDIT_FONT) )
        FXFreeFont( display, current_preedit_font );
    return( 0 );
    }

int
DestroyStatusWindow( flag)
    int	flag;
    {
    if( (current_client->window_type & FC_SAME_ROOT_STATUS)
			  	    == FC_SAME_ROOT_STATUS ) {
        return( 0 );
        }
    if( !(current_client->window_type & FC_SAME_ROOT_STATUS_FONT) )
        FXFreeFont( display, current_client->status_font );

    if( current_client->style & (XIMStatusNothing | XIMStatusArea) ) {
        if( flag == 0 )
    	    if( !(current_client->window_type & FC_SAME_ROOT_STATUS_WINDOW) )
	        XDestroyWindow( display, current_client->status_window );
	}
    if( !(current_client->window_type & FC_SAME_ROOT_STATUS_GC) )
        FXFreeGC( display, current_client->status_font );
    return( 0 );
    }
