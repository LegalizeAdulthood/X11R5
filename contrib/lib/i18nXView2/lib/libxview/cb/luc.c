#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)luc.c 70.6 91/09/28";
#endif
#endif

/*
 *      (c) Copyright 1990 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */
#define	DEBUG
#define	DO_NEGOTIATE
#define	GET_EVENT

#include <stdio.h>
#include <sys/types.h>
#include <widec.h>
#include <stdlib.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>

#include <xview/xview.h>
#include <xview/frame.h>
#include <xview/panel.h>
#include <xview/xv_xrect.h>
#include <xview/font.h>
#include <xview/cms.h>
#include <X11/XSunExt.h>
#include <xview_private/lookupchoice.h>
#include <xview_private/xv_i18n_impl.h>

static   void	luc_mouse_handler(); /* mouse selection handler */
void	ignore_mouse_handler();  /* ignore mouse selection handler */
static	 Bool	luc_start_is_called = FALSE;
#ifdef  GET_EVENT
static    XKeyEvent           e ;
#endif

/*
 *	 This is a LookupChoice callback package.
 */

void
lookup_choices_start(ic, client_data, callback_data)
    XIC				ic;
    XPointer			client_data;
    XIMLookupStartCallbackStruct	*callback_data;
{

/* Caution!;
 *    callback_data->WhoIsMaster must be XIMIsMaster at the 2nd call
 *    Also, if callback_data->WhoIsMaster was XIMIsMaster at 1st call 
 *    XIM canot call lookup_choices_start in the same session ,because 
 *    in this case CB has to obey XIM , there remains no room
 *    for further negotiation.
 */

    register LucInfo    *info;                /* LUC data            */ 
    
#ifdef	DO_NEGOTIATE
    /* check to see if this is the 2nd call-- negotiateion only. */
/*
    if( callback_data->CBPreference->choice_per_window != 0 ){
*/  if( luc_start_is_called ) {
	info = (LucInfo *)LUC_UDATA_INFO(client_data);
	luc_do_negotiate(info, callback_data, 2);
/*
	return XIMCB_Success;
*/
	return;
    }
#endif

    if( !luc_info_create( client_data ) ) {
	xv_error( NULL,
		ERROR_STRING,
		XV_I18N_MSG("xv_messages", "Lookupchoice: out of memory"),
		0 );
/*
	return XIMCB_FatalError ;
*/
	return;
    }
    info = (LucInfo *)LUC_UDATA_INFO(client_data);
    info->ic = ic;

/*
    info->dpy = (Display *)(((XKeyEvent *)callback_data->event)->display);
*/
    info->dpy = (Display *)XDisplayOfIM( (XIM)XIMOfIC(ic) );

#ifdef	DO_NEGOTIATE
    info->start = callback_data;
#else
    info->start = (XIMLookupStartCallbackStruct *)
                        malloc( sizeof(XIMLookupStartCallbackStruct) );
    info->start->WhoIsMaster = CBIsMaster;
#endif

    luc_do_negotiate(info, callback_data, 1);

#ifdef	GET_EVENT
    luc_get_preedit_handle(info, callback_data);
    /* create xview objects */
    info->frame = (Frame)xv_create( info->p_frame , FRAME_CMD ,
		XV_SHOW, 	              FALSE,
		OPENWIN_NO_MARGIN,	      TRUE,
		FRAME_SHOW_IMSTATUS,          FALSE,
		FRAME_SHOW_HEADER,	      FALSE,
		FRAME_SHOW_RESIZE_CORNER,     FALSE,
		FRAME_CLOSED,                 FALSE,
		WIN_USE_IM, 		      FALSE,
		FRAME_CMD_POINTER_WARP,       FALSE,
		NULL );
#else
    info->frame = (Frame)xv_create( NULL,     FRAME,	
                XV_SHOW,                      FALSE,
                FRAME_SHOW_IMSTATUS,          FALSE,
                FRAME_SHOW_HEADER,            FALSE,
                FRAME_SHOW_RESIZE_CORNER,     FALSE,
                FRAME_CLOSED,                 FALSE,
                NULL );
#endif

    if( !info->frame )
        goto err_return;

    /* get font from the frame */
    info->font = (Xv_Font)xv_get(info->frame, XV_FONT);

#ifdef  GET_EVENT
    info->panel = (Panel)xv_get( info->frame , FRAME_CMD_PANEL,NULL );
#else
    info->panel = (Panel)xv_create( info->frame, PANEL,
		WIN_USE_IM,		FALSE,
		NULL );
#endif

    if( !info->panel )
	    goto err_return;
    xv_set( info->panel , WIN_FONT , info->font , NULL );

    /*  Disable mouse event in lookup choice region.
     *  mouse selection of the kana -> kanji conversion
     *  will be supported in the future
     *  At that time replace PANEL_EVENT_PROC with PANEL_NOTIFY_PROC,
     *  and replace ignore_mouse_handler with luc_mouse_handler.
     *  So capture events here with ignore_mouse_handler
     */

    info->panel_item = (Panel_item)xv_create(info->panel,PANEL_CHOICE,
		PANEL_CHOOSE_ONE,       TRUE,
		PANEL_EVENT_PROC,	ignore_mouse_handler,
		XV_KEY_DATA,            LUC_KEY,        info,
		NULL );
    if( !info->panel_item )
	    goto err_return;
    
    /* set up some initial values */
    info->state = LUC_START_CALLED;
    info->first = 0;    /* index of the 1st candidate */

    /* get window information */
    get_luc_win_info(info, callback_data);
    info->WhoIsMaster = callback_data->WhoIsMaster;

/*
    return XIMCB_Success;
*/
    luc_start_is_called = TRUE;
#ifdef  GET_EVENT
    e = *(callback_data->event);
#endif
    return;


err_return:     /* xv-object creation failure */
    xv_error( NULL,
	ERROR_STRING,
	XV_I18N_MSG("xv_messages", "Lookupchoice: failed to create xv_object"),
	0 );
/*
    return XIMCB_FatalError;
*/
    return;

}

/*
 *   Do negotiation with XIM.
 */
luc_do_negotiate(info, callback_data, count)
    register LucInfo    	*info;
    XIMLookupStartCallbackStruct        *callback_data;
    int				count;   /* how many times has this
					  * function been called   
					  */
{

#ifdef	DO_NEGOTIATE
    if( callback_data->WhoIsMaster == XIMIsMaster ) {
	info->WhoOwnsLabel = callback_data->XIMPreference->WhoOwnsLabel;
	if( count != 1 )
        	get_luc_defaults(info, callback_data);
	info->max_can = callback_data->XIMPreference->choice_per_window;
	info->rows = callback_data->XIMPreference->nrows;
	info->columns = callback_data->XIMPreference->ncolumns;
	info->dir = callback_data->XIMPreference->DrawUpDirection;
    }
    else {
	info->WhoOwnsLabel = CBOwnsLabel;
        get_luc_defaults(info, callback_data);
	callback_data->CBPreference->choice_per_window = info->max_can;
	callback_data->CBPreference->nrows = info->rows;
	callback_data->CBPreference->ncolumns = info->columns;
	callback_data->CBPreference->DrawUpDirection = info->dir;
	callback_data->CBPreference->WhoOwnsLabel = CBOwnsLabel;
	callback_data->WhoIsMaster = CBIsMaster;
    }
#else
    info->WhoOwnsLabel = CBOwnsLabel;
    get_luc_defaults(info, callback_data);
#endif
}

/*
 *	 This function allocates appropriate memory for LucInfo and
 *	connects it to client_data so that every function in the package can
 *	reference LucInfo.
 */
luc_info_create( client_data )
XPointer	client_data;
{
    LucInfo	*data;
    Xv_opaque	*dum;

    dum = (Xv_opaque *)client_data;
    data = (LucInfo *)malloc( sizeof( LucInfo ) );
    if( !data )
	return 0;

    *dum = (Xv_opaque)data;

    return 1;
}

/*       
 *      This function gets handlers of pre-edit windows.( Frame and
 *      the associated object ) to enable Luc frame a command-frame,
 *      and also, to make keyboard grab possible.
 */
luc_get_preedit_handle(info, callback_data)
    register  LucInfo    	*info;
    XIMLookupStartCallbackStruct	*callback_data;
{
    XID		xid;
    XKeyEvent   *event = (XKeyEvent *)callback_data->event;
    int         is_subframe;

    xid = event->window;

    info->p_obj = (Xv_object)win_data(info->dpy, xid);
    info->p_frame = (Frame)xv_get(info->p_obj, WIN_FRAME);
    is_subframe = (int) xv_get(xv_get(info->p_frame, WIN_OWNER),
    			XV_IS_SUBTYPE_OF, FRAME_CLASS);
    if (is_subframe) 
	info->p_frame = (Frame)xv_get(info->p_frame, WIN_OWNER);
}

/*
 *       This function retrieves user-preference setings from
 *       resource database.
 */
get_luc_defaults(info, callback_data)
    register   LucInfo		*info;	
    XIMLookupStartCallbackStruct	*callback_data;
{
    XrmValue	value;
    XrmDatabase	db;
    char	str_type[20];

#ifndef  GET_IC_VAL_IMPLEMENTED
    char	db_file[256];
    char	*home;
    extern	char *getenv();

    if (home = getenv("HOME"))
	(void)strcpy(db_file,home);
    (void)strcat(db_file,"/.Xdefaults");
    db = XrmGetFileDatabase( db_file );
#else
    (void)XGetICValue( info->ic , XimNResourceDatabase , &db , 0 );
#endif

   /*** label mode --- default is alphabetic ***/
    if( info->WhoOwnsLabel == CBOwnsLabel ) {
        if( XrmGetResource(db,"luc.keykind","Luc.Keykind",str_type,&value)){
	    if( !strcmp( value.addr , "Numeric" ) )
		info->key_kind = LUC_NUMERIC;
	    else if( !strcmp( value.addr , "AlphabeticLower" ) )
                info->key_kind = LUC_ALPHABETIC_LOWER;
            else if( !strcmp( value.addr , "AlphabeticUpper" ) )
                info->key_kind = LUC_ALPHABETIC_UPPER;
	    else
		info->key_kind = LUC_ALPHABETIC;
        }
        else
	    info->key_kind = LUC_ALPHABETIC;
    }

   /*** offset of wondow location(x)  --- default = 20 **/
    if( XrmGetResource(db,"luc.xoffset","Luc.Xoffset",str_type,&value))
	info->off_x = atoi( value.addr );
    else 
    	info->off_x = 0;

   /*** offset of wondow location(y)  --- default = 20 **/
    if( XrmGetResource(db,"luc.yoffset","Luc.Yoffset",str_type,&value))
	info->off_y = atoi( value.addr );
    else 
    	info->off_y = 0;

   /***  switch to do keyboard grab when popup is displayed ***/
    if( XrmGetResource(db,"luc.keygrab","Luc.Keygrab",str_type,&value)) {
    	if( !strcmp( value.addr , "False" ) )
		info->keygrab = FALSE;
        else
		info->keygrab = TRUE;
    }
    else
	info->keygrab = TRUE;

    /*
     *  return in case of XIM-preference , where those data downwards
     *  are going to be overwritten by XIMPreference data
     */
#ifdef	DO_NEGOTIATE
    if( callback_data->WhoIsMaster == XIMIsMaster )
	return;
#endif


   /*** Display Direction --- default is horizontal***/
    if( XrmGetResource(db,"luc.ddir","Luc.Ddir",str_type,&value)){
	if( !strcmp( value.addr , "Vertical" ) )
		info->dir = DrawUpVertically;
	else
		info->dir = DrawUpHorizontally;
    }
    else 
	info->dir = DrawUpHorizontally;
    	
   /*** MAX candidate --- default is 26 ***/
    if( XrmGetResource(db,"luc.max","Luc.Max",str_type,&value))
	info->max_can = atoi( value.addr );
    else 
    	info->max_can = 26; 

   /*** number of candidates per line --- default is 6 ***/
    if( XrmGetResource(db,"luc.nrows","Luc.Nrows",str_type,&value))
	info->rows = atoi( value.addr );
    else 
    	info->rows = 6;

   /*** number of candidates per column --- default is 5 ***/
    if( XrmGetResource(db,"luc.ncolumns","Luc.Ncolumns",str_type,&value))
	info->columns = atoi( value.addr );
    else 
    	info->columns = 6;
}

get_luc_win_info(info, callback_data)
    register LucInfo    *info;
    XIMLookupStartCallbackStruct        *callback_data;
{
    XWindowAttributes   w_attr;
    Window              xwin_preedit;
#ifdef  GET_EVENT
    XKeyEvent           *event = (XKeyEvent *)callback_data->event;
#endif
    
 /** overall window geometry **/
#ifdef  GET_EVENT
    xwin_preedit = event->window;
    if( !XGetWindowAttributes(info->dpy, xwin_preedit, &w_attr) )
        return -1;
    info->sc_x = DisplayWidth(info->dpy, DefaultScreen(info->dpy));
    info->sc_y = DisplayHeight(info->dpy, DefaultScreen(info->dpy));
    info->win_x = event->x + w_attr.x + info->off_x;
    info->win_y = event->y + w_attr.y + info->off_y;
    if(info->win_x < info->sc_x/10 ) info->win_x = info->sc_x/10 ;
    if(info->win_x > (info->sc_x*2)/10) info->win_x = (info->sc_x*2)/10 ;
    if(info->win_y < info->sc_y/10 ) info->win_y = info->sc_y/10 ;
    if(info->win_y > (info->sc_y*9)/10) info->win_y = (info->sc_y*9)/10 ;
#else
    info->sc_x = DisplayWidth(info->dpy, DefaultScreen(info->dpy));
    info->sc_y = DisplayHeight(info->dpy, DefaultScreen(info->dpy));
    info->win_x = 100;
    info->win_y = 100;
#endif   

}


void
lookup_choices_draw(ic, client_data, callback_data)
    XIC				ic;
    XPointer			client_data;
    XIMLookupDrawCallbackStruct		*callback_data;
{
    register LucInfo    *info;
    Window		xwin;


    if (!luc_start_is_called)
        return;
        
    info = LUC_UDATA_INFO(client_data);
    info->draw = callback_data;

    if( info->state  == LUC_DRAW_CALLED ) {
	xv_set( info->panel , XV_SHOW , FALSE , NULL );
        xv_destroy(info->panel_item);
          info->panel_item = (Panel_item)xv_create(info->panel,PANEL_CHOICE,
                        PANEL_CHOOSE_ONE,       TRUE,
                        PANEL_EVENT_PROC,       ignore_mouse_handler,
                        XV_KEY_DATA,            LUC_KEY,        info,
                        NULL );
    }

    if( get_luc_draw_info(info) < 0 )
/*
	return XIMCB_FatalError;
*/
	return;
    luc_draw_candidates(info);          /* draw candidates     */

    window_fit( info->panel );
/*
    window_fit( info->frame );
*/

    if( info->state  == LUC_DRAW_CALLED ) {
        xv_set(info->panel, XV_SHOW, TRUE, NULL);
        window_fit( info->frame );
    }
    else {
        window_fit( info->frame );
	xv_set( info->frame,
               XV_X,           info->win_x,
               XV_Y,           info->win_y,
		XV_SHOW,        TRUE,
		NULL );
    }

    xv_set( info->panel_item , PANEL_VALUE , 0 , 0 );
    info->current = 1;   /* current choice nember */
    info->state = LUC_DRAW_CALLED;

    if( info->keygrab ) {
    	xwin = (Window)xv_get( info->p_obj , XV_XID );
    	XGrabKeyboard(info->dpy,xwin,True,GrabModeAsync,GrabModeAsync,CurrentTime);
    }


/*
    return XIMCB_Success;
*/
    return;

}

/*
 *	 This function sets geometry of LUC window.
 */
get_luc_draw_info(info)
    register LucInfo	*info;
{
    int			win_width,win_height;
    XWindowAttributes	w_attr;			
    Window		xwin_preedit;
    int			max_col,max_lin;
#ifdef	DO_NEGOTIATE
    register XIMLookupDrawCallbackStruct  *draw = info->draw;
#else
    XIMChoices	*draw = (XIMChoices  *)(info->draw);
#endif
    int			char_width,char_height;
    XFontSet		font_set;
    XFontSetExtents	*font_set_extents;
#ifdef	GET_EVENT
    XKeyEvent		*event = (XKeyEvent *)info->start->event;
#endif
    int			last_candidate;

/** number of candidates to be drawn **/
    if( info->WhoIsMaster == XIMIsMaster ){
#ifdef	DO_NEGOTIATE
    	info->first = draw->index_of_first_candidate;
	last_candidate = draw->index_of_last_candidate;
#endif
    }
    else {
	/*
	 *   This setting is done in lookup_choices_start() for the 1st time,
	 *   and in lookup_choices_redraw() for the 2nd call and afterwards.
	 */
	 last_candidate = draw->n_choices - 1;
    }
    info->num = last_candidate - info->first + 1;
    if( info->num > info->max_can ) {
	info->num = info->max_can;
	last_candidate = info->first + info->num - 1;
    }

/** character information **/
    font_set = (XFontSet)xv_get(info->font, FONT_SET_ID);
    font_set_extents = XExtentsOfFontSet(font_set);
    char_width = font_set_extents->max_ink_extent.width;
    char_height = font_set_extents->max_ink_extent.height;    
    
    info->char_ratio = char_width / xv_get(info->font,FONT_DEFAULT_CHAR_WIDTH);

/** cell-size etc **/
    info->x0 =  10;	/* might be a constant value */
    info->y0 =  20;	/* might be a constant value */

    if( info->WhoOwnsLabel == CBOwnsLabel )
       info->dx = (LUC_MAX_LABEL_LEN + 1 )
			* xv_get(info->font,FONT_DEFAULT_CHAR_WIDTH)
			+ draw->max_len * char_width;
	/* LUC_MAX_LABEL_LEN + 1 ( including delimiter ) */
    else {
#ifdef  DO_NEGOTIATE
	register int    max_label_len = 0;
        register int	lab;

	for( lab = draw->index_of_first_candidate; 
		lab <= last_candidate ; lab++ ) {
		if( max_label_len < draw->choices->label->length )
			max_label_len = draw->choices->label->length;
	}
	info->dx = max_label_len * xv_get(info->font,FONT_DEFAULT_CHAR_WIDTH)
			+ draw->max_len * char_width;
#endif
    }

    info->dy = char_height + 12;   /* !! hack */

/** max number of cells **/
    max_col = (info->sc_x - info->x0 * 2)/ info->dx;
    max_lin = (info->sc_y - info->y0 * 2)/ info->dy;

/** number of lines and columns **/
    if( info->dir == DrawUpVertically ) { /* use specified nrows first*/
	if( info->rows > max_lin )
	    info->rows = max_lin;
	info->columns = (info->num - 1)/info->rows + 1;
	if( info->columns > max_col ) {
	    info->rows = max_lin;
	    info->columns = (info->num - 1)/info->rows + 1;
	    if( info->columns > max_col ) {
		info->columns = max_col;
	    	info->num = max_lin * max_col;
		last_candidate = info->first + info->num - 1;
		/*
		 *  BUG!! To determine cols & rows, we used info->num and
		 *  candidate, which affect the max-len
		 *  of the candidates. Here, we reduce those numbers considering
		 *  ths restriction of screen size. So it may happen that,
		 *  this reduction (in turn) reduces the required area 
		 *  per candidate. As a result, we might be able to display
		 *  more candidates if we make a feedback.But this seems to be
		 *  an exceptional case.
		 */
	    }
	}
    }
    else if( info->dir == DrawUpHorizontally ) { /* use specified ncolumns */
	if( info->columns > max_col )
	    info->columns = max_col;
    	info->rows = (info->num - 1)/info->columns + 1;
	if( info->rows > max_lin ) {
	    info->columns = max_col;
	    info->rows = (info->num -1 )/info->columns + 1;
	    if( info->rows > max_lin ) {
		info->rows = max_lin;
	    	info->num = max_lin * max_col;
		last_candidate = info->first + info->num -1;
	    }
        }
    }

/** feedback to window geometry **/
    win_width = info->columns * info->dx + info->x0 * 2;
    win_height = info->rows * info->dy + info->y0 * 2;
    if( win_width + info->win_x > info->sc_x ) 
		info->win_x = info->sc_x - win_width;
    if( win_height + info->win_y > info->sc_y ) 
		info->win_y = info->sc_y - win_height;

    return 0;

}

/*    
 *      This function puts candidates on the panel.
 */
luc_draw_candidates( info )
    register        LucInfo     *info;
{
    int			count_minus_1 = 0;
    int			pad_len,pad,max_len;
#ifdef	DO_NEGOTIATE
    register XIMChoiceObject	
			*choice  = info->draw->choices;
#else
    XIMText		*choice  = ((XIMChoices  *)(info->draw))->choices.value;
#endif
    wchar_t		buf[256];
    static wchar_t	blank[2]= {(wchar_t)' ' , (wchar_t)'\0' };
    wchar_t		wchar_buf[256];
    char		*format1 = "% d:";
    char		*format2 = "  %c:";
    char		char_num[4];
    int			number,offset,u_or_l;

    xv_set( info->panel_item, XV_X, 			info->x0 , 
			      XV_Y, 			info->y0 ,
			      PANEL_CHOICE_NROWS,	info->rows,
			      PANEL_CHOICE_NCOLS,	info->columns,
			      NULL );

#ifdef	DO_NEGOTIATE
    max_len = info->draw->max_len;
#else
    max_len = ((XIMChoices  *)(info->draw))->max_len;
#endif
    choice += info->first ;

    if( info->dir == DrawUpVertically )
	xv_set( info->panel_item , PANEL_LAYOUT , PANEL_VERTICAL ,NULL);
    else
	xv_set( info->panel_item , PANEL_LAYOUT , PANEL_HORIZONTAL ,NULL);

    if( info->WhoOwnsLabel == CBOwnsLabel && info->key_kind == LUC_NUMERIC ){
	/*  setup character format for labels( numeric case ) */
	sprintf( char_num, "%1d" ,LUC_MAX_LABEL_LEN);
	format1[1] = char_num[0];
    }
	
    while( count_minus_1 < info->num ) {
    	*buf = 0;

	/** make label first **/
       	if( info->WhoOwnsLabel == XIMOwnsLabel ) {
#ifdef  DO_NEGOTIATE
		if( choice->label->encoding_is_wchar )
			wscpy(buf, choice->label->string.wide_char);
		else {
			mbstowcs(buf, choice->label->string.multi_byte ,choice->label->length);
			buf[choice->label->length] = NULL;
		}
#endif
	}
	else if( info->key_kind == LUC_NUMERIC ) {
	        wsprintf(buf, format1, count_minus_1 + 1);
	}
	else if( info->key_kind == LUC_ALPHABETIC ) {
		number = count_minus_1 / 52;
		offset = count_minus_1 % 26 ;
		u_or_l = ( count_minus_1 / 26 ) / 2;

		if( number != 0 ) {
			sprintf( char_num, "2d", number );
			bcopy( char_num, format2, 2);
		}
		
		if( u_or_l == 0 )
			wsprintf( buf, format2, offset + 'a' );
		else
			wsprintf( buf, format2, offset + 'A' );
	} else if( info->key_kind == LUC_ALPHABETIC_UPPER ) {
                number = 0;
                offset = count_minus_1 % 26;

                if( number != 0 ) {
                        sprintf( char_num, "2d", number );
                        bcopy( char_num, format2, 2);
                }
                wsprintf( buf, format2, offset + 'A' );
        }else if( info->key_kind == LUC_ALPHABETIC_LOWER ) {
                number = 0;
                offset = count_minus_1 % 26;

                if( number != 0 ) {
                        sprintf( char_num, "2d", number );
                        bcopy( char_num, format2, 2);
                }
                wsprintf( buf, format2, offset + 'a' );
        }

	/** now , append the candidate string **/
#ifdef	DO_NEGOTIATE
	if( choice->value->encoding_is_wchar )
  		wscat(buf, choice->value->string.wide_char);
	else {
 		mbstowcs(wchar_buf, choice->value->string.multi_byte, choice->value->length);
 		wchar_buf[choice->value->length] = NULL;
		wscat(buf, wchar_buf);
	}
/*
	pad_len = ( max_len - choice->value->length ) * info->char_ratio;
	for( pad = 0 ; pad < pad_len ; pad++ )
		wscat(buf, blank);
*/
	xv_set(info->panel_item,
		PANEL_CHOICE_STRING_WCS, count_minus_1, buf,
		NULL);
	count_minus_1++;
    	choice ++;
#else
	if( choice->encoding_is_wchar )
                wscat(buf, choice->string.wide_char);
        else {
                mbstowcs(wchar_buf, choice->string.multi_byte, choice->length);
                wchar_buf[choice->length] = NULL;
                wscat(buf, wchar_buf);
        }
/*
        pad_len = ( max_len - choice->length ) * info->char_ratio;
        for( pad = 0 ; pad < pad_len ; pad++ )
                wscat(buf, blank);
*/
        xv_set(info->panel_item,
                PANEL_CHOICE_STRING_WCS, count_minus_1, buf,
                NULL);
        count_minus_1++;
        choice ++;
#endif

    }

    if( info->draw->n_choices > info->num ) {
    	xv_set(info->panel_item,
                PANEL_CHOICE_STRING, count_minus_1++ ,
	 	XV_I18N_MSG("xv_messages","^N: NEXT"),
                NULL);
    	xv_set(info->panel_item,
                PANEL_CHOICE_STRING, count_minus_1++ , 
		XV_I18N_MSG("xv_messages","^P: PREVIOUS"),
                NULL);
    }
}

#ifdef	DO_NEGOTIATE
void
#else
XIMText	*
#endif
lookup_choices_process(ic,client_data,callback_data)
    XIC                     ic;
    XPointer                client_data;
    XIMLookupProcessCallbackStruct  *callback_data;
{
    static char                 keybuf[LUC_MAX_LABEL_LEN+1];
    static int			count = 1;
    register LucInfo            *info;
    LucKeyType			type;
    int				len;
#ifdef	GET_EVENT
    XKeyEvent			*event = (XKeyEvent *)callback_data->event;
#else
    KeySym			*key = (KeySym *)callback_data;
    char			*tmp_str;
#endif
    char			buf[LUC_LOOKUPSTRING_BUFLEN];
    KeySym			keysym;
    XComposeStatus 		status;
    int				find;
    int				i;
    
    
    if (!luc_start_is_called)
        return;

    info = LUC_UDATA_INFO(client_data);

    /* initialize keybuffer */
    if( count == 1 ) {
	for( i = 0 ; i <= LUC_MAX_LABEL_LEN ; i++ )
		keybuf[i] = '\0';
    }
	
    /*
     * categorise key event     1) number key;
     *			 	2) upper alphabetic keys 
     *				3) lower alphabetic keys
     *			 	4) return key
     *			 	5) space key
     *			 	6) control function
     *			 	7) arrow key
     */
#ifdef	GET_EVENT
    len = XLookupString(event, buf, LUC_LOOKUPSTRING_BUFLEN, &keysym, &status);
    if( event->state & ControlMask )
	type = LUC_KEY_CONTROL;
#else
    keysym = *key;
    if( keysym == XK_n || keysym == XK_p )  /* just for test */
	type = LUC_KEY_CONTROL;
#endif
    else if( (keysym >= XK_0) && (keysym <= XK_9) )
	type = LUC_KEY_NUM;
    else if( (keysym >= XK_A) && (keysym <= XK_Z) )
	type = LUC_KEY_UPPER;
    else if( (keysym >= XK_a) && (keysym <= XK_z) )
	type = LUC_KEY_LOWER;
    else if( keysym == XK_Return )
	type = LUC_KEY_RETURN;
    else if( keysym == XK_space )
	type = LUC_KEY_SPACE;
    else if( (keysym == XK_Left)  || (keysym == XK_Up) ||
	     (keysym == XK_Right) || (keysym == XK_Down) )
	type = LUC_KEY_ARROW;
    else
	type = LUC_KEY_ILLEGAL;

/* process key input */
    switch( type ) {
	case	LUC_KEY_NUM:
	case	LUC_KEY_UPPER:
	case	LUC_KEY_LOWER:
#ifndef	GET_EVENT
	    tmp_str = XKeysymToString( keysym );
	    buf[0] = *tmp_str;
#endif
	    keybuf[count-1] = buf[0];
	    find = luc_match_label(info, type, keybuf, count, 1);
	    if( find == XIM_UNDETERMINED ) {
		if( count >= LUC_MAX_LABEL_LEN ){
			count = 1;
/*
			return XIMCB_FatalError;
*/
			return;
		}
		count++;
	    }
	    else
		count = 1;
	    break;
	case	LUC_KEY_RETURN:
	    find = info->current;
	    count = 1;
	    break;
	case	LUC_KEY_SPACE:
	    /* suppress XIM_UNDETERMINED */
	    find = luc_match_label(info, type, keybuf, count-1, 0);
	    count = 1;
	    break;
	case	LUC_KEY_CONTROL:
	    lookup_choices_redraw(info, keysym);
	    find = XIM_UNDETERMINED;
	    count = 1;
	    break;
	case	LUC_KEY_ARROW:
	    luc_arrow_handle(info, keysym);
	    find = XIM_UNDETERMINED;
	    count = 1;
	    break;
	default:
	    find = XIM_UNKNOWN_KEYSYM;
	    count = 1;
	    break;
    }

    if( find == 0 ) {
	callback_data->index_of_choice_selected = XIM_UNKNOWN_KEYSYM;
	return;
    }

#ifdef  DO_NEGOTIATE
    if( find != XIM_UNDETERMINED && find != XIM_UNKNOWN_KEYSYM )
        callback_data->index_of_choice_selected = find - 1 + info->first;
    else
	callback_data->index_of_choice_selected = find;
/* if selection complete , free keyboard grab 
    if( find != XIM_UNDETERMINED && find != XIM_UNKNOWN_KEYSYM  && info->keygrab){
        XUngrabKeyboard(info->dpy, CurrentTime);
    } */

/*
    return XIMCB_Success;
*/
    return;

#else
    if( find == XIM_UNDETERMINED )
        return  (XIMText *)XIM_UNDETERMINED;
    if( find == XIM_UNKNOWN_KEYSYM )
        return  (XIMText *)XIM_UNKNOWN_KEYSYM;
    else {
/*	if( info->keygrab )
        	XUngrabKeyboard(info->dpy, CurrentTime); */
	return ((XIMChoices *)info->draw)->choices.value + find - 1 + 
			info->first;
    }
#endif	

}

luc_match_label(info, type, keybuf, count, flag)
    register LucInfo        *info;
    LucKeyType              type;
    char		    *keybuf;
    int			    count;
    int			    flag;  /* 0; suppress XIM_UNDETERMINED */
{
    register int		find;

    if( count <= 0 )   /* space key cannot be the first input */
	return  XIM_UNKNOWN_KEYSYM;


    /*
     *  It is redundant that CB compares labels where as labels were
     *  created in XIM. The following if block supports this case,
     *  but it is very slow.
     */
    if( info->WhoOwnsLabel == XIMOwnsLabel ) {
        char	    		   labelbuf[LUC_MAX_LABEL_LEN];
        register char		   *lab;
        register XIMChoiceObject   *choice = info->draw->choices + info->first;
        int			    match = 0;
        int	    		    find_mark;

	for( find = 1 ; find <= info->num ;  find++ ) {
	    if( count > choice->label->length )
		    continue;
	    if( choice->label->encoding_is_wchar ){
		lab = labelbuf;
	        if( count != wcstombs(lab, choice->label->string.wide_char,
				LUC_MAX_LABEL_LEN + 1 ) )
			/*
			 *  only ascii characters can be used in labels
			 */
			return 0;  /* Fatal error (wrong char in labels)*/
	    }
	    else {
		lab = choice->label->string.multi_byte;
	    }
	    if( !strncmp(lab, keybuf, count) ) {
	        find_mark = find;
	        match ++;
	        if( flag == 0 ){    
		    /* 
		     *	There must be a label that matches exactly. If not,
		     *  XIM_UNKNOWN_KEYSYM is returned.
		     */
	            if( !strcmp(lab, keybuf) ) 
			return find;
		    else
			match = 0;
		}
	    }
	    choice++;
	}
	if( match == 1 )    /* Just one candidate matched */
		return  find_mark;
	else if( match == 0 )
		return  XIM_UNKNOWN_KEYSYM;
	else
		return  XIM_UNDETERMINED;
    }

    /*
     *  Labels are created in CB itself and the labels are numeric.
     */
    if( info->key_kind == LUC_NUMERIC ) {
	if( type != LUC_KEY_NUM ){    /* Only numeric keys are allowed */
	    return  XIM_UNKNOWN_KEYSYM;
	}
	else {                        /* numeric keys */
	    find = luc_get_number(keybuf, count);
	    if( find > info->num  ||  find < 0 )
		return  XIM_UNKNOWN_KEYSYM;  /* out of range */
	    if( flag != 0  &&  find * 10 <= info->num )
		return  XIM_UNDETERMINED;
	    return  find;
	}
    }

    /*
     *  Labels are created in CB itself and the labels are alphabetic.
     *  Note that in this case , flag is always 1, because our way(like JLE)
     *  of making alphabetic labels doesnot need (flag = 1).
     */
    if( info->key_kind == LUC_ALPHABETIC ){
	int	phase = 0;
	char	key = keybuf[count-1];

	if( type == LUC_KEY_LOWER ) {
	    phase  = luc_get_number( keybuf , count-1 );
	    find = key - 'a' + phase * 52 + 1;
	    if( find > info->num  ||  find < 0 )
		return  XIM_UNKNOWN_KEYSYM;
	    else
		return  find ;
	}
	else if( type == LUC_KEY_UPPER ) {
	    phase  = luc_get_number( keybuf , count-1 );
	    find = key - 'A' + phase * 52 + 27; /* 27 = 26 + 1 */
	    if( find > info->num  ||  find < 0 )
		return  XIM_UNKNOWN_KEYSYM;
	    else
		return  find;
	}
	else if( type == LUC_KEY_NUM ) {
	    if( luc_get_number( keybuf , count ) > (info->num-1)/52  )
		return  XIM_UNKNOWN_KEYSYM;    
	    else
		return  XIM_UNDETERMINED;
	}
	else {
	    return  XIM_UNKNOWN_KEYSYM;    /* This is a roof on the roof!! */
	}
    } else if( info->key_kind == LUC_ALPHABETIC_UPPER ){
        int     phase = 0 ;
        char    key = keybuf[count-1];
 
        if( type == LUC_KEY_UPPER ) {
            phase  = luc_get_number( keybuf , count-1 );
            find = key - 'A' + phase * 26 + 1;
            if( find > info->num  ||  find < 0 )
                return  XIM_UNKNOWN_KEYSYM;
            else
                return  find;
        }
        else if( type == LUC_KEY_NUM ) {
            if( luc_get_number( keybuf , count ) > (info->num-1)/52  )
                return  XIM_UNKNOWN_KEYSYM;
            else
                return  XIM_UNDETERMINED;
        }
        else {
            return  XIM_UNKNOWN_KEYSYM;    /* This is a roof on the roof!! */
        }     
    } else if( info->key_kind == LUC_ALPHABETIC_LOWER ){
        int     phase = 0 ;
        char    key = keybuf[count-1];
 
        if( type == LUC_KEY_LOWER ) {
            phase  = luc_get_number( keybuf , count-1 );
            find = key - 'a' + phase * 26 + 1;
            if( find > info->num  ||  find < 0 )
                return  XIM_UNKNOWN_KEYSYM;
            else
                return  find;
        }
        else if( type == LUC_KEY_NUM ) {
            if( luc_get_number( keybuf , count ) > (info->num-1)/52  )
                return  XIM_UNKNOWN_KEYSYM;
            else
                return  XIM_UNDETERMINED;
        }
        else {
            return  XIM_UNKNOWN_KEYSYM;    /* This is a roof on the roof!! */
        }     
    }
}

/*  
 *   This function converts from string made of numbers to an integer
 */
luc_get_number(keybuf, count)
    char	*keybuf;
    int		count;
{
    register int	n;
    register int	result = 0;

    for( n = 0 ; n < count ; n++ )
	result = 10 * result + (keybuf[n] - '0');

    return  result;
}


/*
 *   This function is a handler of arrow keys. When user inputs arrow keys,
 *   the only thing that lookup_choices_process() does is change the current
 *   candidate.
 */
luc_arrow_handle( info , keysym )
register LucInfo	*info;
KeySym			keysym;
{
    int		deltax,deltay;

    if( info->dir == DrawUpHorizontally ) {
	deltax = 1;
	deltay = info->columns;
    }
    else {
	deltax = info->rows;
	deltay = 1;
    }
	
    switch( keysym ) {
      case XK_Left:
	if( info->current > deltax )
		info->current -= deltax;
	else
		return;
	break;
      case XK_Right:
	if( info->current + deltax <= info->num )
		info->current += deltax;
	else
		return;
	break;
      case XK_Up:
	if( info->current > deltay )
		info->current -= deltay;
	else
		return;
	break;
      case XK_Down:
	if( info->current + deltay <= info->num )
		info->current += deltay;
	else
		return;
	break;
    }

    xv_set( info->panel_item , PANEL_VALUE, info->current-1 , NULL );

}


/*  !!Caution.
 *  This function assumes that WhoIsMaster is CBIsMaster
 *
 *  This function is used to redraw lookup choice window in case that
 *  user specifies next ( or previous ) sets of candidates to be drawn.
 */

lookup_choices_redraw(info, keysym)
    register LucInfo    *info;
    KeySym		keysym;
{
    int		n_set;
    Window	xwin;
    XEvent      event;
    Bool        check_unmap();
    Bool        check_map();
    XID         frame_xid;
#ifdef	DO_NEGOTIATE
    register XIMLookupDrawCallbackStruct  *draw = info->draw;
#else
    XIMChoices	*draw = (XIMChoices  *)(info->draw);
#endif

    switch (keysym) {
	case XK_p:
	    info->first -= info->max_can;
	    if ( info->first < 0 ) {
		n_set = (draw->n_choices - 1) / info->max_can + 1;
		info->first = (n_set - 1) * info->max_can;
	    }
	    break;
	case XK_n:
	    info->first += info->num;
	    if( info->first >= draw->n_choices ) {
		info->first = 0;
	    }
	    break;
	default :
	    return;
    }

/*    if( info->keygrab )
    	XUngrabKeyboard(info->dpy, CurrentTime); */
    xv_set( info->frame , XV_SHOW , FALSE , NULL );

/*  Because of the asynchronous nature of X, requesting an
 *  XV_SHOW, FALSE, of the frame then immediately request
 *  an XV_SHOW, TRUE, of the frame causes the caching
 *  mechanism to go out of sync.  Therefore, after
 *  we do XV_SHOW, FALSE, we sit and wait until we
 *  see the unmap event.
 */
    frame_xid = xv_get(info->frame, XV_XID);
    XSync(info->dpy,0);
    XPeekIfEvent(info->dpy, &event, check_unmap, (char *) frame_xid);

    if( get_luc_draw_info(info) < 0 )
/*
	return XIMCB_FatalError;
*/
	return;
    xv_destroy( info->panel_item );

    /*  Disable mouse event in lookup choice region.
     *  mouse selection of the kana -> kanji conversion
     *  will be supported in the future
     *  At that time replace PANEL_EVENT_PROC with PANEL_NOTIFY_PROC,
     *  and replace ignore_mouse_handler with luc_mouse_handler.
     *  So capture events here with ignore_mouse_handler
     */

    info->panel_item = (Panel_item)xv_create(info->panel,PANEL_CHOICE,
                PANEL_CHOOSE_ONE,       TRUE,
		PANEL_EVENT_PROC,	ignore_mouse_handler,
                XV_KEY_DATA,            LUC_KEY,        info,
                NULL );

    luc_draw_candidates(info);          /* draw candidates     */

    window_fit( info->panel );
    window_fit( info->frame );

    xv_set( info->frame,
               XV_X,           info->win_x,
               XV_Y,           info->win_y,

		XV_SHOW,        TRUE,
		NULL );

/*  Because of the asynchronous nature of X, requesting an
 *  XV_SHOW, FALSE, of the frame then immediately request
 *  an XV_SHOW, TRUE, of the frame causes the caching
 *  mechanism to go out of sync.  Therefore, we sit and wait
 *  until we see the map event, then do XV_SHOW, TRUE of
 *  the frame.
 */
    XSync(info->dpy,0);
    XPeekIfEvent(info->dpy, &event, check_map, (char *)frame_xid);

    xv_set( info->panel_item , PANEL_VALUE , 0 , 0 );
    info->current = 1;   /* current choice nember */
    info->state = LUC_DRAW_CALLED;

/*    if( info->keygrab ) {
    	xwin = (Window)xv_get( info->p_obj , XV_XID );
    	XGrabKeyboard(info->dpy,xwin,True,GrabModeAsync,GrabModeAsync,CurrentTime);
    } */

}
    

void
lookup_choices_done(ic, client_data, callback_data)
    XIC				ic;
    XPointer			client_data;
    XIMLookupStartCallbackStruct	*callback_data;
{
    register        LucInfo     *info;


    if (!luc_start_is_called)
	return;

    info = LUC_UDATA_INFO(client_data);
    
    if (info->keygrab ) {
    	XUngrabKeyboard(info->dpy, CurrentTime);
    }

    info->state = LUC_DONE_CALLED;  /* dust */
    xv_destroy( info->frame );
    
    free( info );
    luc_start_is_called = FALSE;
/*
    XWarpPointer(e.display, 0, e.window, 0,0,0,0,e.x, e.y);
*/
}


static void
luc_mouse_handler(item,value,event)
Panel_item	item;
int		value;
Event		*event;
{
    register LucInfo	*info;

    info = (LucInfo *)xv_get(item,XV_KEY_DATA,LUC_KEY);

    info->current = info->first + value + 1;
#ifdef	DEBUG
fprintf( stderr , "Mouse selection was %d\n" , info->current );
#endif

}

void ignore_mouse_handler(item, event)

Panel_item	item;
Event		*event;
{
    /*  Only pass on non-mouse events to
     *  default event handler
     */

    if ( (event_action(event) != ACTION_SELECT) &&
       (event_action(event) != ACTION_ADJUST) &&
       (event_action(event) != ACTION_MENU) &&
       (event_action(event) != LOC_DRAG) &&
       (event_action(event) != PANEL_EVENT_CANCEL) &&
       (event_action(event) != PANEL_EVENT_DRAG_IN) )
       panel_default_handle_event(item, event);
}

Bool check_map(display, event, xid)
        Display *display;
        XEvent  *event;
        char    *xid;
{
        if ((event->type == MapNotify) && (((XMapEvent *) event)->window == (XID)xid))
                return True;
        else
                return False;
}

Bool check_unmap(display, event, xid)
        Display *display;
        XEvent  *event;
        char    *xid;
{
        if ((event->type == UnmapNotify) && (((XUnmapEvent *) event)->window == (XID)xid))
                return True;
        else
                return False;
}
