#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)panel_seln.c 50.9 91/01/03";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/sun.h>
#include <xview/sel_attrs.h>
#ifdef OW_I18N
#include <stdlib.h>
#include <widec.h>
#include <xview/win_input.h>
#include <xview/xv_i18n.h>
#include <xview_private/win_info.h>
#endif /* OW_I18N */

extern void     (*panel_seln_inform_proc) (),
                (*panel_seln_destroy_proc) ();
#ifdef OW_I18N
extern wchar_t		null_string_wc[];
#endif /* OW_I18N */

static void     panel_seln_destroy_info(),
                panel_seln_function(),
                panel_seln_get();

static void     panel_seln_report_event(), check_cache();


static Seln_result panel_seln_request();

/* Register with the service */
Pkg_private void
panel_seln_init(panel)
    register Panel_info *panel;
{
    /*
     * this is so we only try to contact the selection service once.
     */
    static          no_selection_service;	/* Defaults to FALSE */

    register Panel_selection *primary = panel_seln(panel, SELN_PRIMARY);
    register Panel_selection *secondary = panel_seln(panel, SELN_SECONDARY);
    register Panel_selection *caret = panel_seln(panel, SELN_CARET);
    register Panel_selection *shelf = panel_seln(panel, SELN_SHELF);

    if (no_selection_service)
	return;

    panel->seln_client =
	(Xv_opaque) seln_create(panel_seln_function, panel_seln_request,
		    (char *) panel);

    if (!panel->seln_client) {
	no_selection_service = TRUE;
	return;
    }
    panel_seln_destroy_proc = panel_seln_destroy_info;
    panel_seln_inform_proc = (void (*) ()) panel_seln_report_event;

    primary->rank = SELN_PRIMARY;
    primary->is_null = TRUE;
    primary->ip = (Item_info *) 0;

    secondary->rank = SELN_SECONDARY;
    secondary->is_null = TRUE;
    secondary->ip = (Item_info *) 0;

    caret->rank = SELN_CARET;
    caret->is_null = TRUE;
    caret->ip = (Item_info *) 0;

    shelf->rank = SELN_SHELF;
    shelf->is_null = TRUE;
    shelf->ip = (Item_info *) 0;
}


/* Inquire about the holder of a selection */
Pkg_private     Seln_holder
panel_seln_inquire(rank)
    Seln_rank       rank;
{
    Seln_holder     holder;

    /*
     * always ask the service, even if we have not setup contact before (i.e.
     * no text items). This could happen if some other item has
     * PANEL_ACCEPT_KEYSTROKE on. if (!panel->seln_client) holder.rank =
     * SELN_UNKNOWN; else
     */
    holder = seln_inquire(rank);
    return holder;
}


static void
panel_seln_report_event(panel, event)
    Panel_info     *panel;
    Event          *event;
{
    seln_report_event(panel->seln_client, event);

    if (!panel->seln_client)
	return;

    check_cache(panel, SELN_PRIMARY);
    check_cache(panel, SELN_SECONDARY);
    check_cache(panel, SELN_CARET);
}

static void
check_cache(panel, rank)
    register Panel_info *panel;
    register Seln_rank rank;
{
    Seln_holder     holder;

    if (panel_seln(panel, rank)->ip) {
	holder = seln_inquire(rank);
	if (!seln_holder_same_client(&holder, (char *) panel))
	    panel_seln_cancel(panel, rank);
    }
}



/* Acquire the selection and update state */
Pkg_private void
panel_seln_acquire(panel, ip, rank, is_null)
    register Panel_info *panel;
    Item_info      *ip;
    Seln_rank       rank;
    int             is_null;
{
    register Panel_selection *selection;

    if (!panel->seln_client)
	return;

    switch (rank) {
      case SELN_PRIMARY:
      case SELN_SECONDARY:
      case SELN_CARET:
	selection = panel_seln(panel, rank);
	/*
	 * if we already own the selection, don't ask the service for it.
	 */
	if (ip && selection->ip == ip)
	    break;
	/* otherwise fall through ... */

      default:
	rank = seln_acquire(panel->seln_client, rank);
	switch (rank) {
	  case SELN_PRIMARY:
	  case SELN_SECONDARY:
	  case SELN_CARET:
	  case SELN_SHELF:
	    selection = panel_seln(panel, rank);
	    break;

	  default:
	    return;
	}
	break;
    }

#ifndef ecd_panel
    /*
     * if we already have the selection & it's not null, don't do anything.
     */
    if (ip && selection->ip == ip && !selection->is_null)
	return;
#endif	/* ecd_panel */

    /* if there was an old selection, de-hilite it */
    if (selection->ip)
#ifdef ecd_panel
	panel_seln_dehilite(selection->ip, rank);
#else
	panel_seln_dehilite(selection);
#endif	/* ecd_panel */

    /* record the selection & hilite it if it's not null */
    selection->ip = ip;
    selection->is_null = is_null;
    if (!is_null)
	panel_seln_hilite(selection);
}

/*
 * Clear out the current selection.
 */
Pkg_private void
panel_seln_cancel(panel, rank)
    Panel_info     *panel;
    Seln_rank       rank;
{
    Panel_selection *selection = panel_seln(panel, rank);

    if (!panel->seln_client || !selection->ip)
	return;

    /* de-hilite the selection */
#ifdef ecd_panel
    panel_seln_dehilite(selection->ip, rank);
#else
    panel_seln_dehilite(selection);
#endif	/* ecd_panel */
    selection->ip = (Item_info *) 0;
    (void) seln_done(panel->seln_client, rank);
}

#ifndef ecd_panel
/* de-hilite selection */
void
panel_seln_dehilite(selection)
    Panel_selection *selection;
{
    if (selection->is_null)
	return;

    panel_seln_hilite(selection);
    selection->is_null = TRUE;
}

#endif	/* ecd_panel */


/* Destroy myself as a selection client */
static void
panel_seln_destroy_info(panel)
    register Panel_info *panel;
{
    if (!panel->seln_client)
	return;

    /*
     * cancel PRIMARY and SECONDARY to get rid of possible highlighting
     */
    panel_seln_cancel(panel, SELN_PRIMARY);
    panel_seln_cancel(panel, SELN_SECONDARY);
    if (panel->shelf) {
	free(panel->shelf);
	panel->shelf = (char *) 0;
    }
    seln_destroy(panel->seln_client);
}

/* Callback routines */

/* A function key has gone up -- do something. */
static void
panel_seln_function(panel, buffer)
    register Panel_info *panel;
    register Seln_function_buffer *buffer;
{
    Seln_holder    *holder;
    Panel_selection *selection;

#ifdef OW_I18N
    char	   tmp_buffer[BUFFERSIZE];
    wchar_t	   save_char_wc;
    char           *selection_string;
    wchar_t	   *selection_string_wc;
    wchar_t	   *tmp_str_wc;
#else
    char           *selection_string;
#endif /* OW_I18N */

#ifdef ecd_panel
    int             selection_length;
    char            save_char;
#endif	/* ecd_panel */
    register u_char *cp;
    u_char         *char_buf;
    Event           event;

    if (!panel->kbd_focus_item)
	return;

    switch (seln_figure_response(buffer, &holder)) {
      case SELN_IGNORE:
	break;

      case SELN_REQUEST:
	panel_seln_get(panel, holder, buffer->addressee_rank);
	break;

      case SELN_SHELVE:
	if (panel->shelf)
	    free(panel->shelf);
#ifdef ecd_panel
#ifdef OW_I18N
	selection = panel_seln(panel, buffer->addressee_rank);
	if (selection->is_null || !selection->ip) {
	    selection_string_wc = null_string_wc;
	    selection_length = 0;
	} else
	    panel_get_text_selection(selection->ip, 
				     &(wchar_t *)selection_string_wc,
				     &selection_length, selection->rank);
	if (selection_string_wc) {
	    save_char_wc = *(selection_string_wc + selection_length);
	    *(selection_string_wc + selection_length) = 0;
	    tmp_str_wc = (wchar_t *) 
		panel_strsave_wc((wchar_t *)selection_string_wc);
	    panel->shelf = (char *) wcstombsdup (tmp_str_wc);
	    *(selection_string_wc + selection_length) = save_char_wc;
	} else
	    panel->shelf = (char *) panel_strsave((u_char *)"");
	panel_seln_acquire(panel, (Item_info *) 0, SELN_SHELF, TRUE);
	free ((char *) tmp_str_wc);
#else
	selection = panel_seln(panel, buffer->addressee_rank);
	if (selection->is_null || !selection->ip) {
	    selection_string = "";
	    selection_length = 0;
	} else
	    panel_get_text_selection(selection->ip, &(u_char *)selection_string,
				     &selection_length, selection->rank);
	if (selection_string) {
	    save_char = *(selection_string + selection_length);
	    *(selection_string + selection_length) = 0;
	    panel->shelf = (char *) panel_strsave((u_char *)selection_string);
	    *(selection_string + selection_length) = save_char;
	} else
	    panel->shelf = (char *) panel_strsave((u_char *)"");
	panel_seln_acquire(panel, (Item_info *) 0, SELN_SHELF, TRUE);
#endif /* OW_I18N */
#else
#ifdef notdef
	panel->shelf = panel_strsave(panel_get_value(panel->kbd_focus_item));
#else
	/*
	 * shelve the requested selection, not the caret selection.
	 */
	selection = panel_seln(panel, buffer->addressee_rank);
	if (selection->is_null || !selection->ip)
	    selection_string = "";
	else
	    selection_string = panel_get_value(ITEM_PUBLIC(selection->ip));
	panel->shelf = panel_strsave(selection_string ? selection_string : "");
#endif	/* notdef */
	panel_seln_acquire(panel, (Item_info *) 0, SELN_SHELF, TRUE);
#endif	/* ecd_panel */
	break;

      case SELN_FIND:
	(void) seln_ask(holder,
			SELN_REQ_COMMIT_PENDING_DELETE,
			SELN_REQ_YIELD, 0,
			0);
	break;

      case SELN_DELETE:{

	    if (panel->shelf)
		free(panel->shelf);
	    selection = panel_seln(panel, buffer->addressee_rank);
#ifdef OW_I18N
	    if (selection->is_null || !selection->ip) {
		selection_string_wc = null_string_wc;
		selection_length = 0;
	    } else
		panel_get_text_selection(selection->ip,
					 &(wchar_t *)selection_string_wc,
					 &selection_length, selection->rank);
	    if (selection_string_wc) {
		save_char_wc = *(selection_string_wc + selection_length);
		*(selection_string_wc + selection_length) = 0;
		tmp_str_wc = (wchar_t *) 
			panel_strsave_wc((wchar_t *)selection_string_wc);
		panel->shelf = (char *) wcstombsdup (tmp_str_wc);
		*(selection_string_wc + selection_length) = save_char_wc;
	    } else
		tmp_str_wc = (wchar_t *) panel_strsave_wc((wchar_t *)null_string_wc);
		panel->shelf = wcstombsdup(tmp_str_wc);
		free((char *) tmp_str_wc);
	    panel_seln_acquire(panel, (Item_info *) 0,
			       SELN_SHELF, TRUE);

	    if (!selection_string_wc || selection->is_null || !selection->ip)
		break;

	    /* CUT: Delete the selected characters */
	    panel_seln_delete(selection);

	    if (buffer->addressee_rank == SELN_SECONDARY) {	/* "Quick Move"
								 * operation */
		/* PASTE: Copy the shelf to the caret */
		event_init(&event);
		event_set_down(&event);
		char_buf = cp = (u_char *) malloc(strlen(panel->shelf) + 1);
		strcpy(cp, panel->shelf);

		/* Chop up panel->shelf if it's greater than 256
		 * characters, put valid multibyte characters into the
		 * variable buffer, then stuff buffer into
		 * ie_string.  Do this until the entire
		 * panel->shelf has been stuffed into ie_string
		 */

		while (*cp)
		{
		    register unsigned char	*p;
		    wchar_t			w;
		    register int		i, j, nbytes;

		    for (p=cp, i=BUFFERSIZE, nbytes=0; i > 0 && *p;)
		    {
			if (( j = mbtowc(&w, p, MB_CUR_MAX)) < 0)
			    break;
			p += j;
			i -= j;
			nbytes += j;

		    }
		    bcopy (cp, tmp_buffer, nbytes);
		    tmp_buffer[nbytes] = NULL;
		    cp = p;
		    event_set_id(&event, tmp_buffer[0]);
		    event_set_string(&event, tmp_buffer);
		    panel_handle_event(ITEM_PUBLIC(panel->kbd_focus_item),
					   &event);
		}
		free(char_buf);
	    }
#else
	    if (selection->is_null || !selection->ip) {
		selection_string = "";
		selection_length = 0;
	    } else
		panel_get_text_selection(selection->ip,
					 &(u_char *)selection_string,
					 &selection_length, selection->rank);
	    if (selection_string) {
		save_char = *(selection_string + selection_length);
		*(selection_string + selection_length) = 0;
		panel->shelf =
		    (char *) panel_strsave((u_char *)selection_string);
		*(selection_string + selection_length) = save_char;
	    } else
		panel->shelf = (char *) panel_strsave((u_char *)"");
	    panel_seln_acquire(panel, (Item_info *) 0,
			       SELN_SHELF, TRUE);

	    if (!selection_string || selection->is_null || !selection->ip)
		break;

	    /* CUT: Delete the selected characters */
	    panel_seln_delete(selection);

	    if (buffer->addressee_rank == SELN_SECONDARY) {	/* "Quick Move"
								 * operation */
		/* PASTE: Copy the shelf to the caret */
		event_init(&event);
		event_set_down(&event);
		char_buf = cp = (u_char *) malloc(strlen(panel->shelf) + 1);
		strcpy(cp, panel->shelf);
		while (*cp) {
		    event_id(&event) = (short) *cp++;
		    panel_handle_event(ITEM_PUBLIC(panel->kbd_focus_item),
				       &event);
		}
		free(char_buf);
	    }
#endif /* OW_I18N */
	    break;
	}

      default:
	/* ignore anything else */
	break;
    }
}


/*
 * Respond to a request about my selections.
 */
static          Seln_result
panel_seln_request(attr, context, max_length)
    Seln_attribute  attr;
    register Seln_replier_data *context;
    int             max_length;
{
    register Panel_info *panel = (Panel_info *) context->client_data;
    register Panel_selection *selection;
#ifdef ecd_panel
    char           *selection_string = (char *) 0;
    int             selection_length;
#ifdef OW_I18N
    wchar_t	*selection_string_wc;
    wchar_t	*tmp_str_wc;
    char	*tmp_str;
#endif /* OW_I18N */
#else
    char           *selection_string = (char *) 0;
#endif	/* ecd_panel */
    Seln_result     result;

    selection_string_wc = null_string_wc;
    switch (context->rank) {
      case SELN_PRIMARY:
      case SELN_SECONDARY:
	selection = panel_seln(panel, context->rank);
	if (selection->ip)
#ifdef ecd_panel
#ifdef OW_I18N
	{
	    if (!selection->is_null)
		panel_get_text_selection(selection->ip,
		     &(wchar_t *)selection_string_wc, &selection_length,
		     selection->rank);
	}
#else
	{
	    if (!selection->is_null)
		panel_get_text_selection(selection->ip,
		     &(u_char *)selection_string, &selection_length,
		     selection->rank);
	}
#endif /* OW_I18N */
#else
	    selection_string = selection->is_null ?
		"" : panel_get_value(ITEM_PUBLIC(selection->ip));
#endif	/* ecd_panel */
	break;

      case SELN_SHELF:
#ifdef OW_I18N
	selection_string_wc = mbstowcsdup(panel->shelf);
	selection_length = (selection_string_wc ? wslen(selection_string_wc) : 0);
#else
	selection_string = panel->shelf;
#endif /* OW_I18N */
#if defined (ecd_panel) && !defined (OW_I18N)
	selection_length = (selection_string ? strlen(selection_string) : 0);
#endif	/* ecd_panel */
	break;

      default:
	break;
    }

    switch (attr) {
      case SELN_REQ_BYTESIZE:

#ifdef OW_I18N
	if (!selection_string_wc)
	    return SELN_DIDNT_HAVE;
#else
	if (!selection_string)
	    return SELN_DIDNT_HAVE;
#endif /* OW_I18N */


#ifdef ecd_panel
#ifdef OW_I18N
	tmp_str_wc = wsdup(selection_string_wc);
	if (tmp_str_wc) {
	    tmp_str_wc[selection_length] = *null_string_wc;
	    *context->response_pointer++ = (caddr_t)
		strlen ((char *)wcstombsdup(tmp_str_wc));
	}
	else
	    return SELN_FAILED;
#else
	*context->response_pointer++ = (caddr_t) selection_length;
#endif /* OW_I18N */
#else
	*context->response_pointer++ = (caddr_t) strlen(selection_string);
#endif	/* ecd_panel */

	return SELN_SUCCESS;

#ifdef OW_I18N
      case SELN_REQ_CHARSIZE:
	*context->response_pointer++ = (caddr_t) selection_length;
	return SELN_SUCCESS;

      case SELN_REQ_CONTENTS_ASCII:{
	    char           *temp = (char *) context->response_pointer;
	    int             count;

	    if (!selection_string_wc)
		return SELN_DIDNT_HAVE;

	    tmp_str = wcstombsdup(selection_string_wc);
	    count = selection_length;
	    if (count <= max_length) {
		bcopy(tmp_str, temp, count);
		temp += count;
		while ((unsigned) temp % sizeof(*context->response_pointer))
		    *temp++ = '\0';
		context->response_pointer = (char **) temp;
		*context->response_pointer++ = 0;
		free ((char *) tmp_str);
		return SELN_SUCCESS;
	    }
	    return SELN_FAILED;
	}

      case SELN_REQ_CONTENTS_WCS: {
	    char           *temp = (char *) context->response_pointer;
	    int             count;

	    if (!selection_string_wc)
		return SELN_DIDNT_HAVE;

	    count = selection_length;
	    if (count <= max_length) {
		wsncpy((wchar_t *)temp, selection_string_wc, count);
		temp += count * sizeof(wchar_t);
		while ((unsigned) temp % sizeof(*context->response_pointer))
		    *temp++ = '\0';
		context->response_pointer = (char **) temp;
		*context->response_pointer++ = 0;
		return SELN_SUCCESS;
	    }
	    return SELN_FAILED;
            break;
	}
#else
      case SELN_REQ_CONTENTS_ASCII:{
	    char           *temp = (char *) context->response_pointer;
	    int             count;

	    if (!selection_string)
		return SELN_DIDNT_HAVE;

#ifdef ecd_panel
	    count = selection_length;
#else
	    count = strlen(selection_string);
#endif	/* ecd_panel */
	    if (count <= max_length) {
		bcopy(selection_string, temp, count);
		temp += count;
		while ((unsigned) temp % sizeof(*context->response_pointer))
		    *temp++ = '\0';
		context->response_pointer = (char **) temp;
		*context->response_pointer++ = 0;
		return SELN_SUCCESS;
	    }
	    return SELN_FAILED;
	}
#endif /* OW_I18N */

      case SELN_REQ_YIELD:
	result = SELN_FAILED;
	switch (context->rank) {
	  case SELN_SHELF:
	    if (panel->shelf) {
		result = SELN_SUCCESS;
		free(panel->shelf);
		panel->shelf = 0;
		(void) seln_done(panel->seln_client, SELN_SHELF);
	    }
	    break;

	  default:
	    if (panel_seln(panel, context->rank)->ip) {
		panel_seln_cancel(panel, (Seln_rank) context->rank);
		result = SELN_SUCCESS;
	    }
	    break;
	}
	*context->response_pointer++ = (caddr_t) result;
	return result;

#ifdef OW_I18N
      case SELN_REQ_END_REQUEST:
	result = SELN_FAILED;
	break;
#endif /* OW_I18N */

      default:
	return SELN_UNRECOGNIZED;
    }
}


/* Selection utilities */

/*
 * Get the selection from holder and put it in the text item that owns the
 * selection rank.
 */
static void
panel_seln_get(panel, holder, rank)
    Panel_info     *panel;
    Seln_holder    *holder;
    Seln_rank       rank;
{
    Seln_request   *buffer;
    register Attr_avlist avlist;
#ifdef ecd_panel
    register u_char *cp;
#else
    register char  *cp;
#endif	/* ecd_panel */
    Event           event;
    int             num_chars;
    Panel_selection *selection = panel_seln(panel, rank);
    Item_info      *ip = selection->ip;
#ifdef OW_I18N
    int		    num_bytes;
    char	    tmp_buffer[BUFFERSIZE];
    wchar_t	    *wcp;
#endif /* OW_I18N */

    if (!panel->seln_client)
	return;

#ifdef OW_I18N
    /*
     * if the request is too large, drop it on the floor.
     * also check for Kanji client, if so then request
     * for wide char content, otherwise just ask for
     * regular ascii content.
     */
    buffer = seln_ask(holder, SELN_REQ_CHARSIZE, NULL,
			    NULL);

    if (buffer->status == SELN_FAILED)
       return;

    avlist = (Attr_avlist) buffer->data;
 
    if ((Seln_attribute) * avlist++ != SELN_REQ_CHARSIZE) {
	if (holder->rank == SELN_SECONDARY)
	    buffer = seln_ask(holder,
			      SELN_REQ_BYTESIZE, NULL,
			      SELN_REQ_CONTENTS_ASCII, NULL,
			      SELN_REQ_YIELD, NULL,
			      NULL);
	else
	    buffer = seln_ask(holder,
			      SELN_REQ_BYTESIZE, NULL,
			      SELN_REQ_CONTENTS_ASCII, NULL,
			      SELN_REQ_COMMIT_PENDING_DELETE, NULL,
			      NULL);

	if (buffer->status == SELN_FAILED)
           return;
 
	avlist = (Attr_avlist) buffer->data;
     
	if ((Seln_attribute) * avlist++ != SELN_REQ_BYTESIZE)
	   return; 

	num_bytes = (int) *avlist++;

	if ((Seln_attribute) * avlist++ != SELN_REQ_CONTENTS_ASCII)
	    return;
#ifdef ecd_panel
	cp = (u_char *) avlist;
#else
	cp = (char *) avlist;
#endif /* ecd_panel */

	cp[num_bytes] = '\0';
    }
    else {
	if (holder->rank == SELN_SECONDARY)
	    buffer = seln_ask(holder,
			      SELN_REQ_CHARSIZE, NULL,
			      SELN_REQ_CONTENTS_WCS, NULL,
			      SELN_REQ_YIELD, NULL,
			      NULL);
	else
	    buffer = seln_ask(holder,
			      SELN_REQ_CHARSIZE, NULL,
			      SELN_REQ_CONTENTS_WCS, NULL,
			      SELN_REQ_COMMIT_PENDING_DELETE, NULL,
			      NULL);

	if (buffer->status == SELN_FAILED)
           return;
 
	avlist = (Attr_avlist) buffer->data;

	if ((Seln_attribute) *avlist++ != SELN_REQ_CHARSIZE)
	    return;

	num_chars = (int) *avlist++;

	if ((Seln_attribute) * avlist++ != SELN_REQ_CONTENTS_WCS)
	    return;

	wcp = (wchar_t *) avlist;
	wcp[num_chars] = *null_string_wc;

#ifdef ecd_panel
	cp = (u_char *) wcstombsdup(wcp);
#else
	cp = (char *) wcstombsdup(wcp);
#endif /* ecd_panel */

    }
#else
    /*
     * if the request is too large, drop it on the floor.
     */
    if (holder->rank == SELN_SECONDARY)
	buffer = seln_ask(holder,
			  SELN_REQ_BYTESIZE, 0,
			  SELN_REQ_CONTENTS_ASCII, 0,
			  SELN_REQ_YIELD, 0,
			  0);
    else
	buffer = seln_ask(holder,
			  SELN_REQ_BYTESIZE, 0,
			  SELN_REQ_CONTENTS_ASCII, 0,
			  SELN_REQ_COMMIT_PENDING_DELETE,
			  0);
    if (buffer->status == SELN_FAILED)
       return;

    avlist = (Attr_avlist) buffer->data;
 
    if ((Seln_attribute) * avlist++ != SELN_REQ_BYTESIZE)
       return; 
    num_chars = (int) *avlist++;
    if (buffer->status == SELN_FAILED)
	return;

    avlist = (Attr_avlist) buffer->data;

    if ((Seln_attribute) * avlist++ != SELN_REQ_CONTENTS_ASCII)
	return;
#ifdef ecd_panel
    cp = (u_char *) avlist;
#else
    cp = (char *) avlist;
#endif /* ecd_panel */

    cp[num_chars] = '\0';
#endif /* OW_I18N */

#ifdef ecd_panel
    panel_seln_dehilite(ip, rank);
#else
    panel_seln_dehilite(selection);
#endif	/* ecd_panel */


#ifdef OW_I18N

    /* Chop up content of cp if it's greater than 256
     * characters, put valid multibyte characters into the
     * variable buffer, then stuff buffer into
     * ie_string.  Do this until the entire
     * panel->shelf has been stuffed into ie_string
     * Need to stuff a character into ie_code so that
     * panel event handling routines will work.
     * Control characters have to be handled separately
     * in ie_code alone.
     */
     {
	  register u_char		*p;
	  wchar_t		w;
	  register int		i, j, nbytes;

	  p = cp;
	  while (*cp)
	  {
	      /* initialize the event */
	      event_init(&event);
	      event_set_down(&event);


	      /*  Check for control characters, because
	       *  they need to be sent through the event loop
	       *  individually
	       */

	      if (iscntrl(*p))
	      {
		  event_set_id(&event, *p);
		  p++;
		  cp++;
		  panel_handle_event(ITEM_PUBLIC(ip), &event);
	      }
	      else
	      {

	      /* Send all printable characters in the ie_string field,
	       * Should I check for not control character, or check
	       * for printable characters?? Try not control first.
	       */

	          for (i=BUFFERSIZE, nbytes=0; 
			i > 0 && *p && !iscntrl(*p); )
		  {
			if (( j = mbtowc(&w, p, MB_CUR_MAX)) < 0)
			    break;
			p += j;
			i -= j;
			nbytes += j;
		  }
		  bcopy(cp, tmp_buffer, nbytes);
		  tmp_buffer[nbytes] = NULL;
		  cp = p;
		  event_set_id(&event, tmp_buffer[0]);
		  event_set_string(&event, tmp_buffer);
		  panel_handle_event(ITEM_PUBLIC(ip), &event);

	      }
	  }
      }

#else
    /* initialize the event */
    event_init(&event);
    event_set_down(&event);

    while (*cp) {
	event_id(&event) = (short) *cp++;
	(void) panel_handle_event(ITEM_PUBLIC(ip), &event);
    }
#endif /* OW_I18N */
}
