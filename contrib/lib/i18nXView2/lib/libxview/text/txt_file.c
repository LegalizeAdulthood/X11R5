#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_file.c 70.2 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * File load/save/store utilities for text subwindows.
 */

#include <xview_private/primal.h>
#include <xview_private/ev_impl.h>
#include <xview_private/txt_impl.h>
#include <sys/dir.h>
#ifdef SVR4
#include <dirent.h>
#endif SVR4
#include <sys/stat.h>
#ifdef OW_I18N
#include <xview/xv_i18n.h>
#include <xview_private/xv_i18n_impl.h>
#include <sys/types.h>
#include <sys/file.h>
#endif
/*
 * Following two #undefs are because Ultrix sys/param.h doesn't check to see
 * if MIN and MAX are already defined
 */
#undef MIN
#undef MAX
#include <sys/param.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <errno.h>
#include <xview/expandname.h>


#define SET_BOOL_FLAG(flags, to_test, flag)			\
	if ((unsigned)(to_test) != 0) (flags) |= (flag);	\
	else (flags) &= ~(flag)
#ifndef OW_I18N
extern CHAR     *STRCAT();
extern CHAR    *STRNCAT();
#endif /* OW_I18N */

extern char    *getwd();
extern int      errno, sys_nerr;
extern char    *sys_errlist[];

#ifdef OW_I18N
char *dgettext();
#endif OW_I18N

pkg_private int textsw_change_directory();
pkg_private void textsw_display(), textsw_display_view_margins();
pkg_private void textsw_input_before();
pkg_private void textsw_init_undo(), textsw_replace_esh();
pkg_private Es_index textsw_input_after();
Pkg_private Es_status es_copy();

static unsigned tmtn_counter;
static
textsw_make_temp_name(in_here)
    CHAR            *in_here;
/* *in_here must be at least MAXNAMLEN characters long */
{
    /*
     * BUG ALERT!  Should be able to specify directory other than /tmp.
     * However, if that directory is not /tmp it should be a local (not NFS)
     * directory and will make finding files that need to be replayed after
     * crash harder - assuming we ever implement replay.
     */
    in_here[0] = '\0';

    (void) SPRINTF(in_here, "%s/Text%d.%d",
		   "/tmp", getpid(), tmtn_counter++);
}

#define ES_BACKUP_FAILED	ES_CLIENT_STATUS(0)
#define ES_BACKUP_OUT_OF_SPACE	ES_CLIENT_STATUS(1)
#define ES_CANNOT_GET_NAME	ES_CLIENT_STATUS(2)
#define ES_CANNOT_OPEN_OUTPUT	ES_CLIENT_STATUS(3)
#define ES_CANNOT_OVERWRITE	ES_CLIENT_STATUS(4)
#define ES_DID_NOT_CHECKPOINT	ES_CLIENT_STATUS(5)
#define ES_PIECE_FAIL		ES_CLIENT_STATUS(6)
#define ES_SHORT_READ		ES_CLIENT_STATUS(7)
#define ES_UNKNOWN_ERROR	ES_CLIENT_STATUS(8)
#define ES_USE_SAVE		ES_CLIENT_STATUS(9)

#define TXTSW_LRSS_MASK		0x00000007

pkg_private     Es_handle
textsw_create_ps(folio, original, scratch, status)
    Textsw_folio    folio;
    register Es_handle original, scratch;
    Es_status      *status;
{
    register Es_handle result;

    result =
	folio->es_create(folio->client_data, original, scratch);
    if (result) {
	*status = ES_SUCCESS;
    } else {
	es_destroy(original);
	es_destroy(scratch);
	*status = ES_PIECE_FAIL;
    }
    return (result);
}

pkg_private     Es_handle
textsw_create_file_ps(folio, name, scratch_name, status)
    Textsw_folio    folio;
    CHAR             *name, *scratch_name;
    Es_status      *status;
/* *scratch_name must be at least MAXNAMLEN characters long, and is modified */
{
    Pkg_private Es_handle es_file_create();
    register Es_handle original_esh, scratch_esh, piece_esh;

    original_esh = es_file_create(name, 0, status);
    if (!original_esh)
	return (ES_NULL);
    textsw_make_temp_name(scratch_name);
    scratch_esh = es_file_create(scratch_name,
				 ES_OPT_APPEND | ES_OPT_OVERWRITE | ES_OPT_BACKUPFILE,
				 status);
    if (!scratch_esh) {
	es_destroy(original_esh);
	return (ES_NULL);
    }
    (void) es_set(scratch_esh, ES_FILE_MODE, 0600, 0);
    piece_esh = textsw_create_ps(folio, original_esh, scratch_esh, status);
    return (piece_esh);
}

#define	TXTSW_LFI_CLEAR_SELECTIONS	0x1

pkg_private     Es_status
textsw_load_file_internal(
		     textsw, name, scratch_name, piece_esh, start_at, flags)
    register Textsw_folio textsw;
    CHAR            *name, *scratch_name;
    Es_handle      *piece_esh;
    Es_index        start_at;
    int             flags;
/* *scratch_name must be at least MAXNAMLEN characters long, and is modified */
{
    Es_status       status;

    textsw_take_down_caret(textsw);
    *piece_esh = textsw_create_file_ps(textsw, name,
				       scratch_name, &status);
    if (status == ES_SUCCESS) {
	if (flags & TXTSW_LFI_CLEAR_SELECTIONS) {
	    Textsw          abstract = VIEW_REP_TO_ABS(textsw->first_view);

	    textsw_set_selection(abstract, ES_INFINITY, ES_INFINITY,
				 EV_SEL_PRIMARY);
	    textsw_set_selection(abstract, ES_INFINITY, ES_INFINITY,
				 EV_SEL_SECONDARY);
	}
	textsw_replace_esh(textsw, *piece_esh);
	if (start_at != ES_CANNOT_SET) {
#ifdef OW_I18N	
	     char		*name_mb = wcstombsdup(name);
#endif	     
	    (void) ev_set(textsw->views->first_view,
			  EV_FOR_ALL_VIEWS,
			  EV_DISPLAY_LEVEL, EV_DISPLAY_NONE,
			  EV_DISPLAY_START, start_at,
			  EV_DISPLAY_LEVEL, EV_DISPLAY,
			  0);
	    textsw_notify(textsw->first_view,
#ifdef OW_I18N	    
			  TEXTSW_ACTION_LOADED_FILE, name_mb, 
			  TEXTSW_ACTION_LOADED_FILE_WCS, name, 
#else
			  TEXTSW_ACTION_LOADED_FILE, name, 
#endif			  
			  0);
	    textsw_update_scrollbars(textsw, TEXTSW_VIEW_NULL);
#ifdef OW_I18N
            if (name_mb)	    	    
	        free(name_mb);
#endif	    
	}
    }
    return (status);
}

pkg_private void
textsw_destroy_esh(folio, esh)
    register Textsw_folio folio;
    register Es_handle esh;
{
    Es_handle       original_esh, scratch_esh;

    if (folio->views->esh == esh)
	folio->views->esh = ES_NULL;
    if (original_esh = (Es_handle) es_get(esh, ES_PS_ORIGINAL)) {
	textsw_give_shelf_to_svc(folio);
	scratch_esh = (Es_handle) es_get(esh, ES_PS_SCRATCH);
	es_destroy(original_esh);
	if (scratch_esh)
	    es_destroy(scratch_esh);
    }
    es_destroy(esh);
}

pkg_private void
textsw_replace_esh(textsw, new_esh)
    register Textsw_folio textsw;
    Es_handle       new_esh;
/* Caller is repsonsible for actually repainting the views. */
{
    Es_handle       save_esh = textsw->views->esh;
    int             undo_count = textsw->undo_count;

    (void) ev_set(textsw->views->first_view,
		  EV_DISPLAY_LEVEL, EV_DISPLAY_NONE,
		  EV_CHAIN_ESH, new_esh,
		  0);
    textsw->state &= ~TXTSW_EDITED;
    textsw_destroy_esh(textsw, save_esh);
    /* Following two calls are inefficient textsw_re-init_undo. */
    textsw_init_undo(textsw, 0);
    textsw_init_undo(textsw, undo_count);
    textsw->state &= ~TXTSW_READ_ONLY_ESH;
    if (textsw->notify_level & TEXTSW_NOTIFY_SCROLL) {
	register Textsw_view_handle view;
	FORALL_TEXT_VIEWS(textsw, view) {
	    textsw_display_view_margins(view, RECT_NULL);
	}
    }
}

pkg_private     Es_handle
textsw_create_mem_ps(folio, original)
    Textsw_folio    folio;
    register Es_handle original;
{
    Pkg_private Es_handle es_mem_create();
    register Es_handle scratch;
    Es_status       status;
    Es_handle       ps_esh = ES_NULL;
#ifdef OW_I18N
    CHAR	    dummy[1];
    
    dummy[0] = NULL;
#endif    

    if (original == ES_NULL)
	goto Return;
#ifdef OW_I18N
    scratch = es_mem_create(folio->es_mem_maximum, dummy);
#else
    scratch = es_mem_create(folio->es_mem_maximum, "");
#endif	
    if (scratch == ES_NULL) {
	es_destroy(original);
	goto Return;
    }
    ps_esh = textsw_create_ps(folio, original, scratch, &status);
Return:
    return (ps_esh);
}

/* Returns 0 iff load succeeded (can do cd instead of load). */
pkg_private int
textsw_load_selection(folio, locx, locy, no_cd)
    register Textsw_folio folio;
    register int    locx, locy;
    int             no_cd;
{
    CHAR             filename[MAXNAMLEN];
    register int    result;

    if (textsw_get_selection_as_filename(
			   folio, filename, SIZEOF(filename), locx, locy)) {
	return (-10);
    }
    result = no_cd ? -2
	: textsw_change_directory(folio, filename, TRUE, locx, locy);
    if (result == -2) {
	result = textsw_load_file(VIEW_REP_TO_ABS(folio->first_view),
				  filename, TRUE, locx, locy);
	if (result == 0) {
	    (void) textsw_set_insert(folio, 0L);
	}
    }
    return (result);
}

pkg_private CHAR  *
textsw_full_pathname(name)
    register CHAR   *name;
{
    CHAR             pathname[MAXPATHLEN];
#ifdef OW_I18N
    char             pathname_mb[MAXPATHLEN];
    CHAR	     dummy[2];
#endif    
    register CHAR   *full_pathname;

    if (name == 0)
	return (name);
    if (*name == '/') {
	if ((full_pathname = MALLOC((unsigned) (1 + STRLEN(name)))) == 0)
	    return (0);
	(void) STRCPY (full_pathname, name);
	return (full_pathname);
    }

#ifdef OW_I18N
    if (getwd(pathname_mb) == 0)
	return (0);
    mbstowcs(pathname, pathname_mb, MAXPATHLEN-1);
#else
    if (getwd(pathname) == 0)
	return (0);
#endif	
    
    if ((full_pathname =
	 MALLOC((unsigned) (2 + STRLEN(pathname) + STRLEN(name)))) == 0)
	return (0);
    (void) STRCPY (full_pathname, pathname);
#ifdef OW_I18N 
    dummy[0] = '/';
    dummy[1] = NULL;
    (void) STRCAT (full_pathname, dummy);
#else  
    (void) STRCAT (full_pathname, "/");
#endif    
    (void) STRCAT (full_pathname, name);
    return (full_pathname);
}

/* ARGSUSED */
pkg_private int
textsw_format_load_error(msg, status, filename, scratch_name)
    CHAR            *msg;
    Es_status       status;
    CHAR            *filename;
    CHAR            *scratch_name;	/* Currently unused */
{
    CHAR            *full_pathname;
#ifdef OW_I18N
    CHAR	    temp[30];	
#endif    

    switch (status) {
      case ES_PIECE_FAIL:
#ifdef OW_I18N  
        (void) SPRINTF(msg, "%s", XV_I18N_MSG("xv_messages", "Cannot create piece stream."));
#else        
	(void) sprintf(msg, "Cannot create piece stream.");
#endif	
	break;
      case ES_SUCCESS:
	/* Caller is being lazy! */
	break;
      default:
	full_pathname = textsw_full_pathname(filename);
#ifdef OW_I18N
        (void) SPRINTF(msg, "%s", XV_I18N_MSG("xv_messages", "Cannot load; "));
        mbstowcs(temp, XV_I18N_MSG("xv_messages", "file"), 29);
        es_file_append_error(msg, temp, status);
#else        
	(void) sprintf(msg, "Cannot load; ");
	es_file_append_error(msg, "file", status);
#endif	
	es_file_append_error(msg, full_pathname, status);
	free(full_pathname);
	break;
    }
}

pkg_private int
textsw_format_load_error_quietly(msg, status, filename, scratch_name)
    CHAR            *msg;
    Es_status       status;
    CHAR            *filename;
    CHAR            *scratch_name;	/* Currently unused */
{
    CHAR            *full_pathname;

    switch (status) {
      case ES_PIECE_FAIL:
#ifdef OW_I18N
        (void) SPRINTF(msg, "%s", XV_I18N_MSG("xv_messages", 
		"INTERNAL ERROR: Cannot create piece stream."));
#else              
	(void) sprintf(msg,
		       "INTERNAL ERROR: Cannot create piece stream.");
#endif		       
	break;
      case ES_SUCCESS:
	/* Caller is being lazy! */
	break;
      default:
	full_pathname = textsw_full_pathname(filename);
#ifdef OW_I18N
        (void) SPRINTF(msg, "%s", XV_I18N_MSG("xv_messages", "Unable to load file:"));
#else	
	(void) sprintf(msg, "Unable to load file:");
#endif
	es_file_append_error(msg, full_pathname, status);
	free(full_pathname);
	break;
    }
}

/* Returns 0 iff load succeeded. */
extern int
textsw_load_file(abstract, filename, reset_views, locx, locy)
    Textsw          abstract;
    CHAR            *filename;
    int             reset_views;
    int             locx, locy;
{
    CHAR             notice_msg_buf[MAXNAMLEN + 100];
    CHAR             scratch_name[MAXNAMLEN];
    int             result;
    Es_status       status;
    Es_handle       new_esh;
    Es_index        start_at;
    Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    Textsw_folio    textsw = FOLIO_FOR_VIEW(view);

    start_at = (reset_views) ? 0 : ES_CANNOT_SET;
    status = textsw_load_file_internal(
			 textsw, filename, scratch_name, &new_esh, start_at,
				       TXTSW_LFI_CLEAR_SELECTIONS);
    if (status == ES_SUCCESS) {
#ifdef OW_I18N	
	char		*filename_mb = wcstombsdup(filename);
#endif	         
	if (start_at == ES_CANNOT_SET)
	    textsw_notify((Textsw_view_handle) textsw,	/* Cast for lint */
#ifdef OW_I18N	    
			  TEXTSW_ACTION_LOADED_FILE, filename_mb, 
			  TEXTSW_ACTION_LOADED_FILE_WCS, filename, 0);
	if (filename_mb)	    	    
	    free(filename_mb);			   
#else
			  TEXTSW_ACTION_LOADED_FILE, filename, 0);
#endif			  	    
    } else {
	textsw_format_load_error_quietly(
			    notice_msg_buf, status, filename, scratch_name);
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
#ifdef OW_I18N			       
			       NOTICE_MESSAGE_STRINGS_WCS,
#else
			       NOTICE_MESSAGE_STRINGS,
#endif			       
			       notice_msg_buf,
			       0,
			       0);
    }

    return (status);
}

/* Returns 0 iff load succeeded. */
extern int
textsw_load_file_quietly(abstract, filename, feedback, reset_views, locx, locy)
    Textsw          abstract;
    CHAR            *filename, *feedback;
    int             reset_views;
    int             locx, locy;
{
    CHAR             scratch_name[MAXNAMLEN];
    Es_status       status;
    Es_handle       new_esh;
    Es_index        start_at;
    Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    Textsw_folio    textsw = FOLIO_FOR_VIEW(view);

    start_at = (reset_views) ? 0 : ES_CANNOT_SET;
    status = textsw_load_file_internal(
			 textsw, filename, scratch_name, &new_esh, start_at,
				       TXTSW_LFI_CLEAR_SELECTIONS);
    if (status == ES_SUCCESS) {
        char		*filename_mb = wcstombsdup(filename);    
	if (start_at == ES_CANNOT_SET)
	    textsw_notify((Textsw_view_handle) textsw,	/* Cast for lint */
#ifdef OW_I18N	    
			  TEXTSW_ACTION_LOADED_FILE, filename_mb, 
			  TEXTSW_ACTION_LOADED_FILE_WCS, filename, 0);
            if (filename_mb)	    	    
	        free(filename_mb);			  
#else
			  TEXTSW_ACTION_LOADED_FILE, filename, 0);
#endif		    
    } else {
	textsw_format_load_error_quietly(
				  feedback, status, filename, scratch_name);
    }
    return (status);
}

#define RELOAD		1
#define NO_RELOAD	0
static          Es_status
textsw_save_store_common(folio, output_name, reload)
    register Textsw_folio folio;
    CHAR            *output_name;
    int             reload;
{
    CHAR             scratch_name[MAXNAMLEN];
    Es_handle       new_esh;
    register Es_handle output;
    Es_status       result;
    Es_index        length;
    Pkg_private Es_handle es_file_create();

    output = es_file_create(output_name, ES_OPT_APPEND, &result);
    if (!output)
	/* BUG ALERT!  For now, don't look at result. */
	return (ES_CANNOT_OPEN_OUTPUT);
    length = es_get_length(folio->views->esh);
    result = es_copy(folio->views->esh, output, TRUE);
    if (result == ES_SUCCESS) {
	es_destroy(output);
	if (folio->checkpoint_name) {
	    if (unlink(folio->checkpoint_name) == -1) {
		perror(
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "removing checkpoint file:")
#else
		"removing checkpoint file:"
#endif
		);
	    }
	    free(folio->checkpoint_name);
	    folio->checkpoint_name = NULL;
	}
	if (reload) {
	    result = textsw_load_file_internal(
				 folio, output_name, scratch_name, &new_esh,
					       ES_CANNOT_SET, 0);
	    if ((result == ES_SUCCESS) &&
		(length != es_get_length(new_esh))) {
		/* Added a newline - repaint to fix line tables */
		textsw_display(VIEW_REP_TO_ABS(folio->first_view));
	    }
	}
    }
    if (folio->menu && folio->sub_menu_table)
        xv_set(folio->sub_menu_table[(int) TXTSW_FILE_SUB_MENU], MENU_DEFAULT, 1, 0 );
	
    return (result);
}

extern          Es_status
textsw_process_save_error(abstract, error_buf, status, locx, locy)
    Textsw          abstract;
    CHAR            *error_buf;
    Es_status       status;
    int             locx, locy;
{
    char            *msg1, *msg2;
    char             msg[200];
    int             result;
#ifdef OW_I18N    
    char	    *error_buf_mb;
#endif    
    switch (status) {
      case ES_BACKUP_FAILED:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Save Current File. ");
	msg2 = XV_I18N_MSG("xv_messages", "Cannot back-up file:");
#else
	msg1 = "Unable to Save Current File. ";
	msg2 = "Cannot back-up file:";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_BACKUP_OUT_OF_SPACE:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Save Current File. ");
	msg2 = XV_I18N_MSG("xv_messages", "No space for back-up file:");
#else
	msg1 = "Unable to Save Current File. ";
	msg2 = "No space for back-up file:";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_CANNOT_OPEN_OUTPUT:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Save Current File. ");
	msg2 = XV_I18N_MSG("xv_messages", "Cannot re-write file:");
#else
	msg1 = "Unable to Save Current File. ";
	msg2 = "Cannot re-write file:";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_CANNOT_GET_NAME:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Save Current File. ");
	msg2 = XV_I18N_MSG("xv_messages", "INTERNAL ERROR: Forgot the name of the file.");
#else
	msg1 = "Unable to Save Current File. ";
	msg2 = "INTERNAL ERROR: Forgot the name of the file.";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_UNKNOWN_ERROR:	/* Fall through */
      default:
	goto InternalError;
    }
InternalError:
    msg[0] = '\0';
#ifdef OW_I18N
    msg1 = XV_I18N_MSG("xv_messages", "Unable to Save Current File. ");
    msg2 = XV_I18N_MSG("xv_messages", "An INTERNAL ERROR has occurred.");
#else
    msg1 = "Unable to Save Current File. ";
    msg2 = "An INTERNAL ERROR has occurred.";
#endif
    strcat (msg, msg1);
    strcat (msg, msg2);
PostError:
#ifdef OW_I18N
    error_buf_mb = wcstombsdup(error_buf);
#endif
    result = notice_prompt(
			   xv_get(abstract, WIN_FRAME),
			   (Event *) 0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
			   NOTICE_MESSAGE_STRINGS,
			   msg1,
			   msg2,
#ifdef OW_I18N
			   error_buf_mb,
#else
			   error_buf,
#endif
                           0,
                           0);
#ifdef OW_I18N
    free(error_buf_mb);
#endif			   
    return (ES_UNKNOWN_ERROR);
}

/* ARGSUSED */
static          Es_status
textsw_save_internal(folio, error_buf, locx, locy)
    register Textsw_folio folio;
    CHAR            *error_buf;
    int             locx, locy;	/* Currently unused */
{
    Pkg_private Es_handle es_file_make_backup();
    CHAR             original_name[MAXNAMLEN], *name;
#ifdef OW_I18N
#define		MSG_LENGTH		100
    register CHAR   msg[MSG_LENGTH];
#else    
    register char   *msg;
#endif    
    Es_handle       backup, original = ES_NULL;
    int             status;
    Es_status       es_status;

    /*
     * Save overwrites the contents of the original stream, which makes the
     * call to textsw_give_shelf_to_svc in textsw_destroy_esh (reached via
     * textsw_save_store_common calling textsw_replace_esh) perform bad
     * operations on the pieces that are the shelf. To get around this
     * problem, we first save the shelf explicitly.
     */
    textsw_give_shelf_to_svc(folio);
    if (textsw_file_name(folio, &name) != 0)
	return (ES_CANNOT_GET_NAME);
    (void) STRCPY (original_name, name);
    original = (Es_handle) es_get(folio->views->esh,
				  ES_PS_ORIGINAL);
    if (!original) {
#ifdef OW_I18N
	mbstowcs(msg, XV_I18N_MSG("xv_messages", "es_ps_original"), MSG_LENGTH - 1);
#else    
	msg = "es_ps_original";
#endif	
	goto Return_Error_Status;
    }
#ifdef OW_I18N
    if ((backup = es_file_make_backup(original, "%ws%%", &es_status))
	== ES_NULL) {
#else    
    if ((backup = es_file_make_backup(original, "%s%%", &es_status))
	== ES_NULL) {
#endif	
	return (((es_status == ES_CHECK_ERRNO) && (errno == ENOSPC))
		? ES_BACKUP_OUT_OF_SPACE
		: ES_BACKUP_FAILED);
    }
    (void) es_set(folio->views->esh,
		  ES_STATUS_PTR, &es_status,
		  ES_PS_ORIGINAL, backup,
		  0);
    if (es_status != ES_SUCCESS) {
	int             result;

	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(folio),
			       (Event *) 0,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages", "\
Unable to Save Current File.\n\
Was the file edited with another editor?."),
#else
			       "Unable to Save Current File.",
			       "Was the file edited with another editor?.",
#endif
			       0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages", "Continue"),
#else
			       "Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,
			       0);
	if (result == NOTICE_FAILED) {
	    (void) es_destroy(backup);
	    status = (int) es_status;
#ifdef OW_I18N
    	    msg[0] = NULL;
	    mbstowcs(msg, XV_I18N_MSG("xv_messages", "ps_replace_original"), MSG_LENGTH - 1);
#else    
	    msg = "ps_replace_original";
#endif		    
	    goto Return_Error_Status;
	}
	goto Dont_Return_Error_Status;
    }
    switch (status =
	    textsw_save_store_common(folio, original_name, RELOAD)) {
      case ES_SUCCESS: {
#ifdef OW_I18N	      
        char		*original_name_mb  = wcstombsdup(original_name);
#endif	                 
	(void) es_destroy(original);
	textsw_notify(folio->first_view,
#ifdef OW_I18N	    
			  TEXTSW_ACTION_LOADED_FILE, original_name_mb, 
			  TEXTSW_ACTION_LOADED_FILE_WCS, original_name, 0);
        if (original_name_mb)	    	    
	    free(original_name_mb);			  
#else
			  TEXTSW_ACTION_LOADED_FILE, original_name, 0);
#endif			  	    
	
		          
	return (ES_SUCCESS);
	}
      case ES_CANNOT_OPEN_OUTPUT:
	if (errno == EACCES)
	    goto Return_Error;
#ifdef OW_I18N
    	msg[0] = NULL;
	mbstowcs(msg, XV_I18N_MSG("xv_messages", "es_file_create"), MSG_LENGTH - 1);
#else    
	msg = "es_file_create";
#endif		    	    
	goto Return_Error_Status;
      default:
#ifdef OW_I18N
    	msg[0] = NULL;
	mbstowcs(msg, XV_I18N_MSG("xv_messages", "textsw_save_store_common"), MSG_LENGTH - 1);
#else    
	msg = "textsw_save_store_common";
#endif		    	    
     	break;
    }
Return_Error_Status:
#ifdef OW_I18N
    (void) SPRINTF(error_buf, XV_I18N_MSG("xv_messages", "  %ws; status = 0x%x"),
				msg, status);
#else   
    (void) sprintf(error_buf, "  %s; status = 0x%x", msg, status);
#endif		    	    

Dont_Return_Error_Status:
    status = ES_UNKNOWN_ERROR;
Return_Error:
    if (original)
	(void) es_set(folio->views->esh,
		      ES_STATUS_PTR, &es_status,
		      ES_PS_ORIGINAL, original,
		      0);
    return (status);
#ifdef OW_I18N
#undef	MSG_LENGTH
#endif    
}

extern unsigned
textsw_save(abstract, locx, locy)
    Textsw          abstract;
    int             locx, locy;
{
    CHAR             error_buf[MAXNAMLEN + 100];
    Es_status       status;
    Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);

    error_buf[0] = '\0';
    status = textsw_save_internal(FOLIO_FOR_VIEW(view), error_buf,
				  locx, locy);
    if (status != ES_SUCCESS)
	status = textsw_process_save_error(
				   abstract, error_buf, status, locx, locy);
    return ((unsigned) status);
}

static          Es_status
textsw_get_from_fd(view, fd, print_error_msg)
    register Textsw_view_handle view;
    int             fd;
    int             print_error_msg;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             record;
    Es_index        old_insert_pos, old_length;
    register long   count;
    char             buf[2096];    
    Es_status       result = ES_SUCCESS;
    int             status;
#ifdef OW_I18N
    CHAR             buf_ws[2096];
    int		     wc_count, temp;    
    int		     new_pos, old_pos;    
#endif

    textsw_flush_caches(view, TFC_PD_SEL);	/* Changes length! */
    textsw_input_before(view, &old_insert_pos, &old_length);
    textsw_take_down_caret(folio);
#ifdef OW_I18N
    new_pos = old_pos = 0;
#endif    
    for (;;) {
	count = read(fd, buf, sizeof(buf) - 1);
	if (count == 0)
	    break;
	if (count < 0) {
	    return (ES_UNKNOWN_ERROR);
	}
	buf[count] = '\0';
#ifdef OW_I18N
	temp = count;
	
	wc_count = my_mbstowcs(buf_ws, buf, &temp);
	if (temp != count) {
	   /* re-read the incomplete mb character */
	    new_pos = lseek(fd, temp - count, L_INCR);
	    if (new_pos == old_pos) {
	       /* Invalid char, so advance to next byte */
	        old_pos = lseek(fd, 1L, L_INCR);   
	    } else
	        old_pos = new_pos;
	}
	status = ev_input_partial(FOLIO_FOR_VIEW(view)->views, buf_ws, wc_count);
#else	
	status = ev_input_partial(FOLIO_FOR_VIEW(view)->views, buf, count);
#endif
	if (status) {
	    if (print_error_msg)
		(void) textsw_esh_failed_msg(view, 
#ifdef OW_I18N
			XV_I18N_MSG("xv_messages", "Insertion failed - ")
#else
			"Insertion failed - "
#endif
		);
	    result = (Es_status) es_get(folio->views->esh, ES_STATUS);
	    break;
	}
    }
    record = (TXTSW_DO_AGAIN(folio) &&
	      ((folio->func_state & TXTSW_FUNC_AGAIN) == 0));
    (void) textsw_input_after(view, old_insert_pos, old_length, record);
    return (result);
}

pkg_private int
textsw_cd(textsw, locx, locy)
    Textsw_folio    textsw;
    int             locx, locy;
{
    CHAR             buf[MAXNAMLEN];

    if (0 == textsw_get_selection_as_filename(
				    textsw, buf, SIZEOF(buf), locx, locy)) {
	(void) textsw_change_directory(textsw, buf, FALSE, locx, locy);
    }
    return;
}

pkg_private     Textsw_status
textsw_get_from_file(view, filename, print_error_msg)
    Textsw_view_handle view;
    CHAR            *filename;
    int             print_error_msg;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             fd;
    Es_status       status;
    Textsw_status   result = TEXTSW_STATUS_CANNOT_INSERT_FROM_FILE;
    CHAR             buf[MAXNAMLEN];
#ifdef OW_I18N
    char             *buf_mb;
#endif    


    if (!TXTSW_IS_READ_ONLY(folio) && (STRLEN(filename) > 0)) {
        buf[0] = NULL;
	STRCPY (buf, filename);
#ifdef OW_I18N	
	if (textsw_expand_filename(folio, buf, MAXNAMLEN, -1, -1)
#else
	if (textsw_expand_filename(folio, buf, -1, -1)
#endif	
	    == 0) {
#ifdef OW_I18N	 
	    buf_mb = wcstombsdup(buf);
	    if ((fd = open(buf_mb, 0)) >= 0) {
	        free(buf_mb);
#else   
	    if ((fd = open(buf, 0)) >= 0) {
#endif	    
		textsw_take_down_caret(folio);
		textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
				       (caddr_t) TEXTSW_INFINITY - 1);
		status = textsw_get_from_fd(view, fd, print_error_msg);
		textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
				       (caddr_t) TEXTSW_INFINITY - 1);
		textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);
		(void) close(fd);
		if (status == ES_SUCCESS)
		    result = TEXTSW_STATUS_OKAY;
		else if (TEXTSW_OUT_OF_MEMORY(folio, status))
		    result = TEXTSW_STATUS_OUT_OF_MEMORY;
		textsw_invert_caret(folio);
	
	    }
	}
    }
    return (result);
}


pkg_private int
textsw_file_stuff(view, locx, locy)
    Textsw_view_handle view;
    int             locx, locy;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             fd;
    CHAR             buf[MAXNAMLEN], msg[MAXNAMLEN + 100], *sys_msg;   
    CHAR             notice_msg1[MAXNAMLEN + 100];
    CHAR            *notice_msg2 = NULL;
    Es_status       status;
    int             cannot_open = 0;
    int             result;

    if (0 == textsw_get_selection_as_filename(
				     folio, buf, SIZEOF(buf), locx, locy)) {
	if ((fd = open(buf, 0)) < 0) {
	    cannot_open = (fd == -1);
	    goto InternalError;
	};
	errno = 0;
	textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
			       (caddr_t) TEXTSW_INFINITY - 1);
	status = textsw_get_from_fd(view, fd, TRUE);
	textsw_checkpoint_undo(VIEW_REP_TO_ABS(view),
			       (caddr_t) TEXTSW_INFINITY - 1);
	textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);
	(void) close(fd);
	if (status != ES_SUCCESS && status != ES_SHORT_WRITE)
	    goto InternalError;
    }
    return;
InternalError:
    if (cannot_open) {
	CHAR            *full_pathname;

	full_pathname = textsw_full_pathname(buf);
#ifdef OW_I18N
	(void) SPRINTF(msg, "'%ws': ", full_pathname);
	(void) SPRINTF(notice_msg1, "'%ws'", full_pathname);
	notice_msg2 = mbstowcsdup("  ");
#else	
	(void) sprintf(msg, "'%s': ", full_pathname);
	(void) sprintf(notice_msg1, "'%s'", full_pathname);
	notice_msg2 = "  ";
#endif	
	free(full_pathname);
    } else {
#ifdef OW_I18N
	(void) SPRINTF(msg, "%s", 
			XV_I18N_MSG("xv_messages", "Unable to Include File.  An INTERNAL ERROR has occurred.: "));
	(void) SPRINTF(notice_msg1, "%s", 
			XV_I18N_MSG("xv_messages", "Unable to Include File."));
	notice_msg2 = mbstowcsdup(XV_I18N_MSG("xv_messages", "An INTERNAL ERROR has occurred."));
#else
	(void) sprintf(msg, "%s", "Unable to Include File.  An INTERNAL ERROR has occurred.: ");
	(void) sprintf(notice_msg1, "%s", "Unable to Include File.");
	notice_msg2 = "An INTERNAL ERROR has occurred.";
#endif    
    }
#ifdef OW_I18N    
    sys_msg = (errno > 0 && errno < sys_nerr) ? mbstowcsdup(sys_errlist[errno]) : NULL;
#else
    sys_msg = (errno > 0 && errno < sys_nerr) ? sys_errlist[errno] : NULL;
#endif
    
    result = notice_prompt(
			   FRAME_FROM_FOLIO_OR_VIEW(view),
			   (Event *) 0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
			   NOTICE_MESSAGE_STRINGS,
			   (STRLEN(sys_msg)) ? sys_msg : notice_msg1,
			   (STRLEN(sys_msg)) ? notice_msg1 : notice_msg2,
			   (STRLEN(sys_msg)) ? notice_msg2 : 0,
			   0,
			   0);
#ifdef OW_I18N
    if (sys_msg)
        free(sys_msg);
    if (notice_msg2)
        free(notice_msg2);   
#endif			   
}
pkg_private     Textsw_status
textsw_file_stuff_from_str(view, buf, locx, locy)
    Textsw_view_handle view;
    CHAR            *buf;
    int             locx, locy;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(view);
    int             fd;
    CHAR             msg[MAXNAMLEN + 100], *sys_msg;
    CHAR             notice_msg1[MAXNAMLEN + 100];
    CHAR            *notice_msg2 = NULL;
    Es_status       status;
    int             cannot_open = 0;
    int             result;
#ifdef OW_I18N
    CHAR	    buf_mb[MAXNAMLEN];
#endif    

#ifdef OW_I18N
    wcstombs(buf_mb, buf, MAXNAMLEN);
    if ((fd = open(buf_mb, 0)) < 0) {
#else
    if ((fd = open(buf, 0)) < 0) {
#endif    
	cannot_open = (fd == -1);
	goto InternalError;
    }
    errno = 0;
    textsw_checkpoint_undo(VIEW_REP_TO_ABS(view), (caddr_t) TEXTSW_INFINITY - 1);
    status = textsw_get_from_fd(view, fd, TRUE);
    textsw_checkpoint_undo(VIEW_REP_TO_ABS(view), (caddr_t) TEXTSW_INFINITY - 1);
    textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);
    (void) close(fd);
    if (status != ES_SUCCESS && status != ES_SHORT_WRITE)
	goto InternalError;
    return (Textsw_status) status;
InternalError:
    if (cannot_open) {
	CHAR            *full_pathname;

	full_pathname = textsw_full_pathname(buf);
#ifdef OW_I18N
	(void) SPRINTF(msg, "'%ws': ", full_pathname);
	(void) SPRINTF(notice_msg1, "'%ws'", full_pathname);
	notice_msg2 = mbstowcsdup("  ");
#else	
	(void) sprintf(msg, "'%s': ", full_pathname);
	(void) sprintf(notice_msg1, "'%s'", full_pathname);
	notice_msg2 = "  ";
#endif	
	free(full_pathname);
    } else {
#ifdef OW_I18N
	(void) SPRINTF(msg, "%s", 
		XV_I18N_MSG("xv_messages", "Unable to Include File.  An INTERNAL ERROR has occurred.: "));
	(void) SPRINTF(notice_msg1, "%s", 
		XV_I18N_MSG("xv_messages", "Unable to Include File."));
	notice_msg2 = mbstowcsdup(XV_I18N_MSG("xv_messages",
			"An INTERNAL ERROR has occurred."));
#else
	(void) sprintf(msg, "%s", "Unable to Include File.  An INTERNAL ERROR has occurred.: ");
	(void) sprintf(notice_msg1, "%s", "Unable to Include File.");
	notice_msg2 = "An INTERNAL ERROR has occurred.";
#endif
    }
#ifdef OW_I18N
    sys_msg = (errno > 0 && errno < sys_nerr) ? mbstowcsdup(sys_errlist[errno]) : NULL;
#else
    sys_msg = (errno > 0 && errno < sys_nerr) ? sys_errlist[errno] : NULL;
#endif
    result = notice_prompt(
			   FRAME_FROM_FOLIO_OR_VIEW(view),
			   (Event *) 0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
#ifdef OW_I18N			   
			   NOTICE_MESSAGE_STRINGS_WCS,
#else			   
			   NOTICE_MESSAGE_STRINGS,
#endif			   
			   (STRLEN(sys_msg)) ? sys_msg : notice_msg1,
			   (STRLEN(sys_msg)) ? notice_msg1 : notice_msg2,
			   (STRLEN(sys_msg)) ? notice_msg2 : 0,
			   0,
			   0);
#ifdef OW_I18N
    if (sys_msg)
        free(sys_msg);
    if (notice_msg2)
        free(notice_msg2);   
#endif			   
    return (Textsw_status) status;
}

pkg_private     Es_status
textsw_store_init(textsw, filename)
    Textsw_folio    textsw;
    CHAR            *filename;
{
    struct stat     stat_buf;
#ifdef OW_I18N  
    char	    *filename_mb = wcstombsdup(filename);
      
    if (stat(filename_mb, &stat_buf) == 0) {
#else
    if (stat(filename, &stat_buf) == 0) {
#endif    
	Es_handle       original = (Es_handle)
	es_get(textsw->views->esh, ES_PS_ORIGINAL);
	if AN_ERROR
	    (original == ES_NULL) {
	    return (ES_CANNOT_GET_NAME);
	    }
	switch ((Es_enum) es_get(original, ES_TYPE)) {
	  case ES_TYPE_FILE:
	    if (es_file_copy_status(original, filename) != 0)
		return (ES_USE_SAVE);
	    /* else fall through */
	  default:
	    if ((stat_buf.st_size > 0) &&
		(textsw->state & TXTSW_CONFIRM_OVERWRITE))
		return (ES_CANNOT_OVERWRITE);
	    break;
	}
    } else if (errno != ENOENT) {
	return (ES_CANNOT_OPEN_OUTPUT);
    }
#ifdef OW_I18N  
         free(filename_mb);
#endif	
    
    return (ES_SUCCESS);
}

/* ARGSUSED */
pkg_private     Es_status
textsw_process_store_error(textsw, filename, status, locx, locy)
    Textsw_folio    textsw;
    CHAR            *filename;	/* Currently unused */
    Es_status       status;
    int             locx, locy;
{
    char            *msg1, *msg2;
    char             msg[200];
    int             result;

    switch (status) {
      case ES_SUCCESS:
	LINT_IGNORE(ASSUME(0));
	return (ES_UNKNOWN_ERROR);
      case ES_CANNOT_GET_NAME:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Store as New File. ");
	msg2 = XV_I18N_MSG("xv_messages",
		"INTERNAL ERROR: Forgot the name of the file.");
#else
	msg1 = "Unable to Store as New File. ";
	msg2 = "INTERNAL ERROR: Forgot the name of the file.";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_CANNOT_OPEN_OUTPUT:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Store as New File. ");
	msg2 = XV_I18N_MSG("xv_messages", "Problems accessing specified file.");
#else
	msg1 = "Unable to Store as New File. ";
	msg2 = "Problems accessing specified file.";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_CANNOT_OVERWRITE:
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Confirm"),
#else
				"Confirm",
#endif
			       NOTICE_BUTTON_NO, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Cancel"),
#else
				"Cancel",
#endif
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages",
				"Please confirm Store as New File:"),
#else
			       "Please confirm Store as New File:",
#endif
			       filename,
			       "  ",
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages",
				"That file exists and has data in it."),
#else
			       "That file exists and has data in it.",
#endif
			       0,
			       0);
	return ((result == NOTICE_YES) ? ES_SUCCESS : ES_UNKNOWN_ERROR);
      case ES_FLUSH_FAILED:
      case ES_FSYNC_FAILED:
      case ES_SHORT_WRITE:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Store as New File. ");
	msg2 = XV_I18N_MSG("xv_messages", "File system full.");
#else
	msg1 = "Unable to Store as New File. ";
	msg2 = "File system full.";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_USE_SAVE:
	msg[0] = '\0';
#ifdef OW_I18N
	msg1 = XV_I18N_MSG("xv_messages", "Unable to Store as New File. ");
	msg2 = XV_I18N_MSG("xv_messages", "Use Save Current File instead.");
#else
	msg1 = "Unable to Store as New File. ";
	msg2 = "Use Save Current File instead.";
#endif
	strcat (msg, msg1);
	strcat (msg, msg2);
	goto PostError;
      case ES_UNKNOWN_ERROR:	/* Fall through */
      default:
	goto InternalError;
    }
InternalError:
    msg[0] = '\0';
#ifdef OW_I18N
    msg1 = XV_I18N_MSG("xv_messages", "Unable to Store as New File. ");
    msg2 = XV_I18N_MSG("xv_messages", "An INTERNAL ERROR has occurred.");
#else
    msg1 = "Unable to Store as New File. ";
    msg2 = "An INTERNAL ERROR has occurred.";
#endif
    strcat (msg, msg1);
    strcat (msg, msg2);
PostError:
    result = notice_prompt(
			   FRAME_FROM_FOLIO_OR_VIEW(textsw),
			   (Event *) 0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
			   NOTICE_MESSAGE_STRINGS,
			   msg1,
			   msg2,
			   0,
			   0);
    return (ES_UNKNOWN_ERROR);
}


#ifdef OW_I18N
extern unsigned
textsw_store_file(abstract, filename_mb, locx, locy)
    Textsw          abstract;
    char            *filename_mb;
    int             locx, locy;
#else
extern unsigned
textsw_store_file(abstract, filename, locx, locy)
    Textsw          abstract;
    char            *filename;
    int             locx, locy;
#endif
{
    register Es_status status;
    Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Textsw_folio textsw = FOLIO_FOR_VIEW(view);
#ifdef OW_I18N    
    CHAR	    *filename = mbstowcsdup(filename_mb);
#endif    
    
    status = textsw_store_init(textsw, filename);
    switch (status) {
      case ES_SUCCESS:
	break;
      case ES_USE_SAVE:
	return (textsw_save(abstract, locx, locy));
	/* Fall through */
      default:
	status = textsw_process_store_error(
				      textsw, filename, status, locx, locy);
	break;
    }
    if (status == ES_SUCCESS) {
	status = textsw_save_store_common(textsw, filename,
				  (textsw->state & TXTSW_STORE_CHANGES_FILE)
					  ? TRUE : FALSE);
	if (status == ES_SUCCESS) {
	    if (textsw->state & TXTSW_STORE_CHANGES_FILE)
		textsw_notify(textsw->first_view,
#ifdef OW_I18N		
			      TEXTSW_ACTION_LOADED_FILE, filename_mb, 
			      TEXTSW_ACTION_LOADED_FILE_WCS, filename, 
#else
			      TEXTSW_ACTION_LOADED_FILE, filename, 
#endif			      
			      0);
	} else {
	    status = textsw_process_store_error(
				      textsw, filename, status, locx, locy);
	}
    }
#ifdef OW_I18N		   
    if (filename)
        free(filename);
#endif        
    return ((unsigned) status);
}

#ifdef OW_I18N
extern unsigned
textsw_store_file_wcs(abstract, filename, locx, locy)
    Textsw          abstract;
    CHAR            *filename;
    int             locx, locy;
{
    register Es_status status;
    Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Textsw_folio textsw = FOLIO_FOR_VIEW(view);
    

    status = textsw_store_init(textsw, filename);
    switch (status) {
      case ES_SUCCESS:
	break;
      case ES_USE_SAVE:
	return (textsw_save(abstract, locx, locy));
	/* Fall through */
      default:
	status = textsw_process_store_error(
				      textsw, filename, status, locx, locy);
	break;
    }
    if (status == ES_SUCCESS) {
	status = textsw_save_store_common(textsw, filename,
				  (textsw->state & TXTSW_STORE_CHANGES_FILE)
					  ? TRUE : FALSE);
	if (status == ES_SUCCESS) {
	    if (textsw->state & TXTSW_STORE_CHANGES_FILE) {
	        char		*filename_mb = wcstombsdup(filename);
		textsw_notify(textsw->first_view,
			      TEXTSW_ACTION_LOADED_FILE, filename_mb, 
			      TEXTSW_ACTION_LOADED_FILE_WCS, filename, 0);
		if (filename_mb)
		    free(filename_mb);	      
            }	      
	} else {
	    status = textsw_process_store_error(
				      textsw, filename, status, locx, locy);
	}
    }
    return ((unsigned) status);
}

#endif

pkg_private     Es_status
textsw_store_to_selection(textsw, locx, locy)
    Textsw_folio    textsw;
    int             locx, locy;
{
    CHAR             filename[MAXNAMLEN];

    if (textsw_get_selection_as_filename(
			    textsw, filename, SIZEOF(filename), locx, locy))
	return (ES_CANNOT_GET_NAME);
    return ((Es_status) textsw_store_file(VIEW_REP_TO_ABS(textsw->first_view),
					  filename, locx, locy));
}

/* ARGSUSED */
extern void
textsw_reset_2(abstract, locx, locy, preserve_memory, cmd_is_undo_all_edit)
    Textsw          abstract;
    int             locx, locy;	/* Currently ignored */
    int             preserve_memory;
    int             cmd_is_undo_all_edit;	/* This is for doing an "Undo
						 * All edit" */
{
#ifndef SVR4
    pkg_private Es_status textsw_checkpoint_internal();
#else SVR4
    static Es_status textsw_checkpoint_internal();
#endif SVR4
    Pkg_private Es_handle es_mem_create();
    Es_handle       piece_esh, old_original_esh, new_original_esh;
    CHAR            *name, save_name[MAXNAMLEN], scratch_name[MAXNAMLEN], *temp_name;
    int             status;
    Textsw_folio    folio = FOLIO_FOR_VIEW(VIEW_ABS_TO_REP(abstract));
    register int    old_count = folio->again_count;
    int             old_memory_length = 0;
    int             wrap_around_size = (int) es_get(
				  folio->views->esh, ES_PS_SCRATCH_MAX_LEN);
    short           is_readonly = TXTSW_IS_READ_ONLY(folio);	/* jcb */

    if (preserve_memory) {
	/* Get info about original esh before possibly invalidating it. */
	old_original_esh = (Es_handle) es_get(
					 folio->views->esh, ES_PS_ORIGINAL);
	if ((Es_enum) es_get(old_original_esh, ES_TYPE) ==
	    ES_TYPE_MEMORY) {
	    old_memory_length = es_get_length(old_original_esh);
	}
    }
    if (textsw_has_been_modified(abstract) &&
	(status = textsw_file_name(folio, &name)) == 0) {
	/* Have edited a file, so reset to the file, not memory. */
	/* First take a checkpoint, so recovery is possible. */
	if (folio->checkpoint_frequency > 0) {
	    if (textsw_checkpoint_internal(folio) == ES_SUCCESS) {
		folio->checkpoint_number++;
	    }
	}
	/* This is for cmdsw log file */
	/* When empty document load up the original empty tmp file */
	/* instead of the one we did a store in */
	temp_name = cmd_is_undo_all_edit ? NULL :
#ifdef OW_I18N
	    folio->temp_filename_wc;
#else	
	    (char *) window_get(abstract, TEXTSW_TEMP_FILENAME);
#endif

	if (temp_name)
	    (void) STRCPY (save_name, temp_name);
	else
	    (void) STRCPY (save_name, name);

	status = textsw_load_file_internal(folio, save_name, scratch_name,
				&piece_esh, 0L, TXTSW_LFI_CLEAR_SELECTIONS);
	/*
	 * It would be nice to preserve the old positioning of the views, but
	 * all of the material in a view may be either new or significantly
	 * rearranged. One possiblity is to get the piece_stream to find the
	 * nearest original stream position and use that if we add yet
	 * another hack into ps_impl.c!
	 */
	if (status == ES_SUCCESS)
	    goto Return;
	/* BUG ALERT: a few diagnostics might be appreciated. */
    }
    if (old_memory_length > 0) {
	/*
	 * We are resetting from memory to memory, and the old memory had
	 * preloaded contents that we should preserve.
	 */
	new_original_esh =
	    es_mem_create(old_memory_length + 1, "");
	if (new_original_esh) {
	    es_copy(old_original_esh, new_original_esh, FALSE);
	}
    } else {
	new_original_esh = es_mem_create(0, "");
    }
    piece_esh = textsw_create_mem_ps(folio, new_original_esh);
    if (piece_esh != ES_NULL) {
	textsw_set_selection(abstract, ES_INFINITY, ES_INFINITY,
			     EV_SEL_PRIMARY);
	textsw_set_selection(abstract, ES_INFINITY, ES_INFINITY,
			     EV_SEL_SECONDARY);
	textsw_replace_esh(folio, piece_esh);
	(void) ev_set(folio->views->first_view,
		      EV_FOR_ALL_VIEWS,
		      EV_DISPLAY_LEVEL, EV_DISPLAY_NONE,
		      EV_DISPLAY_START, 0,
		      EV_DISPLAY_LEVEL, EV_DISPLAY,
		      0);

	textsw_update_scrollbars(folio, TEXTSW_VIEW_NULL);

	textsw_notify(folio->first_view, TEXTSW_ACTION_USING_MEMORY, 0);
    }
Return:
    (void) textsw_set_insert(folio, 0L);
    textsw_init_again(folio, 0);
    textsw_init_again(folio, old_count);	/* restore number of again
						 * level */
    (void) es_set(folio->views->esh,
		  ES_PS_SCRATCH_MAX_LEN, wrap_around_size,
		  0);
    if (folio->menu && folio->sub_menu_table)
        xv_set(folio->sub_menu_table[(int) TXTSW_FILE_SUB_MENU], MENU_DEFAULT, 1, 0 );

    if (is_readonly)		/* jcb -- reset if readonly */
	SET_BOOL_FLAG(folio->state, TRUE, TXTSW_READ_ONLY_ESH);
}

extern void
textsw_reset(abstract, locx, locy)
    Textsw          abstract;
    int             locx, locy;
{
    textsw_reset_2(abstract, locx, locy, FALSE, FALSE);
}

pkg_private int
textsw_filename_is_all_blanks(filename)
    CHAR            *filename;
{
    int             i = 0;

    while ((filename[i] == ' ') || (filename[i] == '\t') || (filename[i] == '\n'))
	i++;
    return (filename[i] == '\0');
}


#ifndef notdef
/* Returns 0 iff a selection exists and it is matched by exactly one name. */
/* ARGSUSED */
pkg_private int
textsw_expand_filename_quietly(textsw, buf, err_buf, locx, locy)
    Textsw_folio    textsw;
    CHAR            *buf, *err_buf;
    int             locx, locy;
{
    struct namelist *nl;
    CHAR            *msg, *msg_extra = 0;
    
    nl = xv_expand_name(buf);
    if ((buf[0] == '\0') || (nl == NONAMES)) {
#ifdef OW_I18N
	msg = mbstowcsdup(XV_I18N_MSG("xv_messages", "Unrecognized file name.  Unable to match specified pattern."));
#else
	msg = "Unrecognized file name.  Unable to match specified pattern.";
#endif
	msg_extra = buf;
	goto PostError;
    }
    if (textsw_filename_is_all_blanks(buf)) {
#ifdef OW_I18N
	msg = mbstowcsdup(XV_I18N_MSG("xv_messages", "Unrecognized file name.  Filename contains only blank or tab characters."));
#else
	msg = "Unrecognized file name.  Filename contains only blank or tab characters.";
#endif
	goto PostError;
    }
    /*
     * Below here we have dynamically allocated memory pointed at by nl that
     * we must make sure gets deallocated.
     */
    if (nl->count == 0) {
#ifdef OW_I18N
	msg = mbstowcsdup(XV_I18N_MSG("xv_messages", "Unrecognized file name.  No files match specified pattern."));
#else
	msg = "Unrecognized file name.  No files match specified pattern.";
#endif
	msg_extra = buf;
    } else if (nl->count > 1) {
#ifdef OW_I18N
	msg = mbstowcsdup(XV_I18N_MSG("xv_messages", "Unrecognized file name.  Too many files match specified pattern"));
#else
	msg = "Unrecognized file name.  Too many files match specified pattern";
#endif
	msg_extra = buf;
	goto PostError;
    } else
	(void) STRCPY (buf, nl->names[0]);
    free_namelist(nl);
    if (msg_extra != 0)
	goto PostError;
    return (0);
PostError:
    STRCAT (err_buf, msg);	/* STRCAT (err_buf, msg_extra); */
    free(msg);
    return (1);
}

#endif

/* Returns 0 iff a selection exists and it is matched by exactly one name. */

#ifdef OW_I18N
pkg_private int
textsw_expand_filename(textsw, buf, buf_len, locx, locy)
    Textsw_folio    textsw;    
    CHAR            *buf;
    int		    buf_len;  
    int             locx, locy;
#else
pkg_private int
textsw_expand_filename(textsw, buf, locx, locy)
    Textsw_folio    textsw;    
    CHAR            *buf;  
    int             locx, locy;
#endif    
{
    struct namelist *nl;
    int             result;
#ifdef OW_I18N
    char	    buf_mb[MAXPATHLEN];
#else
    char	    msg;    
#endif
    

#ifdef OW_I18N
    wcstombs(buf_mb, buf, MAXPATHLEN);
    nl = xv_expand_name(buf_mb);
    mbstowcs(buf, buf_mb, buf_len);
#else
    nl = xv_expand_name(buf);
#endif
    
    if ((buf[0] == '\0') || (nl == NONAMES)) {
#ifndef OW_I18N
	msg = "Unrecognized file name.  Unable to expand specified pattern: ";
#endif
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages", "\
Unrecognized file name.\n\
Unable to expand specified pattern:"),
			       buf_mb,
#else
			       "Unrecognized file name.",
			       "Unable to expand specified pattern:",
			       buf,
#endif
			       
			       0,
			       0);
	return (1);
    }
    if (textsw_filename_is_all_blanks(buf)) {
#ifndef OW_I18N
	msg = "Unrecognized file name.  Filname contains only blank or tab characters.  Please use a valid file name.";
#endif
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, "Continue",
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "\
Unrecognized file name.\n\
File name contains only blank or tab characters.\n\
Please use a valid file name."),
#else
			       "Unrecognized file name.",
			 "File name contains only blank or tab characters.",
			       "Please use a valid file name.",
#endif
			       0,
			       0);
	return (1);
    }
    /*
     * Below here we have dynamically allocated memory pointed at by nl that
     * we must make sure gets deallocated.
     */
    if (nl->count == 0) {
#ifndef OW_I18N
	msg = "Unrecognized file name.  No files match specified pattern: ";
#endif
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages", "\
Unrecognized file name.\n\
No files match specified pattern:"),
			       buf_mb,
#else			       
			       "Unrecognized file name.",
			       "No files match specified pattern:",
			       buf,
#endif			       
			       0,
			       0);
	return (1);
    } else if (nl->count > 1) {
#ifndef OW_I18N
	msg = "Unrecognized file name.  Too many files match specified pattern: ";
#endif
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(textsw),
			       (Event *) 0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			       NOTICE_TRIGGER, ACTION_STOP,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			       XV_I18N_MSG("xv_messages", "\
Unrecognized file name.\n\
Too many files match specified pattern:"),
			       buf_mb,
#else
			       "Unrecognized file name.",
			       "Too many files match specified pattern:",
			       buf,
#endif
			       
			       0,
			       0);
	return (1);
    } else
#ifdef OW_I18N    
	(void) mbstowcs (buf, nl->names[0], buf_len);
#else
	(void) strcpy (buf, nl->names[0]);

#endif	
    free_namelist(nl);
    return (0);
}


/* Returns 0 iff there is a selection, and it is matched by exactly one name. */
pkg_private int
textsw_get_selection_as_filename(textsw, buf, sizeof_buf, locx, locy)
    Textsw_folio    textsw;
    CHAR            *buf;
    int             sizeof_buf, locx, locy;
{
    CHAR            *msg;
    int             result;

    if (!textsw_get_selection_as_string(textsw, EV_SEL_PRIMARY,
					buf, sizeof_buf)) {
#ifndef OW_I18N
	msg = "After removing this message, please select a file name and choose this menu option again.";
#endif
	goto PostError;
    }
#ifdef OW_I18N    
    return (textsw_expand_filename(textsw, buf, sizeof_buf, locx, locy));
#else
    return (textsw_expand_filename(textsw, buf, locx, locy));
#endif    
PostError:
    result = notice_prompt(
			   FRAME_FROM_FOLIO_OR_VIEW(textsw),
			   (Event *) 0,
			   NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
	      XV_I18N_MSG("xv_messages", "Please select a filename and choose this menu option again."),
#else
	      "Please select a filename and choose this menu option again.",
#endif
			   0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
			   0);
    return (1);
}

pkg_private int
textsw_possibly_edited_now_notify(folio)
    Textsw_folio    folio;
{
    CHAR            *name;

    if (textsw_has_been_modified(VIEW_REP_TO_ABS(folio->first_view))) {
	/*
	 * WARNING: client may textsw_reset() during notification, hence edit
	 * flag's state is only known BEFORE the notification and must be set
	 * before the notification.
	 */

	folio->state |= TXTSW_EDITED;

	if (textsw_file_name(folio, &name) == 0) {
#ifdef OW_I18N
	    char		*name_mb = wcstombsdup(name);
#endif	
	    if (folio->menu && folio->sub_menu_table)
		xv_set(folio->sub_menu_table[(int) TXTSW_FILE_SUB_MENU], MENU_DEFAULT, 2, 0 );
#ifdef OW_I18N
	
	    textsw_notify(folio->first_view, TEXTSW_ACTION_EDITED_FILE, name_mb,
	    		        TEXTSW_ACTION_EDITED_FILE_WCS, name, 0);
	    if (name_mb)	    	    
	        free(name_mb);			   
#else
	    textsw_notify(folio->first_view, TEXTSW_ACTION_EDITED_FILE, name, 0);
#endif			    

	} else {
	    textsw_notify(folio->first_view,
			  TEXTSW_ACTION_EDITED_MEMORY, 0);
	    if (folio->menu && folio->sub_menu_table)
		xv_set(folio->sub_menu_table[(int) TXTSW_FILE_SUB_MENU], MENU_DEFAULT, 3, 0 );	
	}

    }
}

extern int
textsw_has_been_modified(abstract)
    Textsw          abstract;
{
    Textsw_folio    folio = FOLIO_FOR_VIEW(VIEW_ABS_TO_REP(abstract));

    if (folio->state & TXTSW_INITIALIZED) {
	return ((int) es_get(folio->views->esh, ES_HAS_EDITS));
    }
    return (0);
}

pkg_private int
textsw_file_name(textsw, name)
    Textsw_folio    textsw;
    CHAR           **name;
/* Returns 0 iff editing a file and name could be successfully acquired. */
{
    Es_handle       original;

    original = (Es_handle)
	es_get(textsw->views->esh, ES_PS_ORIGINAL);
    if (original == 0)
	return (1);
    if ((Es_enum) es_get(original, ES_TYPE) != ES_TYPE_FILE)
	return (2);
    if ((*name = (CHAR *) es_get(original, ES_NAME)) == NULL)
	return (3);
    if (name[0] == '\0')
	return (4);
    return (0);
}

extern int
textsw_append_file_name(abstract, name)
    Textsw          abstract;
    char            *name;
/* Returns 0 iff editing a file and name could be successfully acquired. */
{
    Textsw_folio    textsw = FOLIO_FOR_VIEW(VIEW_ABS_TO_REP(abstract));
    CHAR            *internal_name;
    int             result;

    result = textsw_file_name(textsw, &internal_name);
    if (result == 0) {
#ifdef OW_I18N  
        char	    *temp;
        temp = wcstombsdup(internal_name);
        (void) strcat (name, temp);
        if (temp)
            free(temp);       	
#else  
	(void) strcat (name, internal_name);
#endif	
    }
    
    return (result);
}

#ifdef OW_I18N
extern int
textsw_append_file_name_wcs(abstract, name)
    Textsw          abstract;
    CHAR            *name;
/* Returns 0 iff editing a file and name could be successfully acquired. */
{
    Textsw_folio    textsw = FOLIO_FOR_VIEW(VIEW_ABS_TO_REP(abstract));
    CHAR            *internal_name;
    int             result;

    result = textsw_file_name(textsw, &internal_name);
    if (result == 0)
	(void) STRCAT (name, internal_name);
    return (result);
}

#endif

/* ARGSUSED */
pkg_private void
textsw_post_error(folio_or_view, locx, locy, msg1, msg2)
    Textsw_opaque   folio_or_view;
    int             locx, locy;	/* Unused */
    CHAR            *msg1, *msg2;
{
    CHAR             buf[MAXNAMLEN + 1000];
    int             size_to_use = SIZEOF(buf);

    buf[0] = '\0';
    (void) STRNCAT (buf, msg1, size_to_use);
    if (msg2) {
	int             len = STRLEN(buf);
	if (len < size_to_use) {
	    (void) STRNCAT (buf, msg2, size_to_use - len);
	}
    }
    (void) notice_prompt(
			 FRAME_FROM_FOLIO_OR_VIEW(folio_or_view),
			 (Event *) 0,
			 NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			 NOTICE_TRIGGER, ACTION_STOP,
#ifdef OW_I18N
			 NOTICE_MESSAGE_STRINGS_WCS,
#else			 
			 NOTICE_MESSAGE_STRINGS,
#endif			 
			 buf,
			 0,
			 0);
}


/*
 * ===================================================================
 * 
 * Misc. file system manipulation utilities
 * 
 * ===================================================================
 */

/* Returns 0 iff change directory succeeded. */
#ifdef OW_I18N
Pkg_private int
textsw_change_directory(textsw, filename_wc, might_not_be_dir, locx, locy)
    Textsw_folio    textsw;
    CHAR            *filename_wc;
#else
Pkg_private int
textsw_change_directory(textsw, filename, might_not_be_dir, locx, locy)
    Textsw_folio    textsw;
    char            *filename;
#endif   
    int             might_not_be_dir;
    int             locx, locy;
{
    char            *sys_msg;
    char            *full_pathname;
    CHAR	    *full_pathname_wc;
#ifndef OW_I18N    
    CHAR	    *full_pathname_wc;
    char             msg[MAXNAMLEN + 100];
#endif
    char             notice_msg[MAXNAMLEN + 100];
    struct stat     stat_buf;
    int             result = 0;
    int             notice_result;
#ifdef OW_I18N
    char            *filename = wcstombsdup(filename_wc);   
#endif       

    errno = 0;
    if (stat(filename, &stat_buf) < 0) {
	result = -1;
	goto Error;
    }
    if ((stat_buf.st_mode & S_IFMT) != S_IFDIR) {
	if (might_not_be_dir)
	    return (-2);
    }
    if (chdir(filename) < 0) {
	result = errno;
	goto Error;
    }
    textsw_notify((Textsw_view_handle) textsw,	/* Cast is for lint */
		  TEXTSW_ACTION_CHANGED_DIRECTORY, filename, 
#ifdef OW_I18N
		  TEXTSW_ACTION_CHANGED_DIRECTORY_WCS, filename_wc, 
#endif		  
		  0);
#ifdef OW_I18N
    if (filename)
        free(filename);
#endif		  
		  
    return (result);

Error:
#ifdef OW_I18N
    full_pathname_wc = textsw_full_pathname(filename_wc);
    full_pathname = wcstombsdup(full_pathname_wc);
#else
    full_pathname = textsw_full_pathname(filename);
#endif 

#ifdef OW_I18N
    (void) sprintf(notice_msg, "%s:",
		   (might_not_be_dir ? XV_I18N_MSG("xv_messages", "Unable to access file") : 
			XV_I18N_MSG("xv_messages", "Unable to cd to directory")));
#else
    (void) sprintf(msg, "Unable to %s '%s': ",
		   (might_not_be_dir ? "access file" : "cd to directory"),
		   full_pathname);
    (void) sprintf(notice_msg, "Unable to %s:",
		   (might_not_be_dir ? "access file" : "cd to directory"));
#endif
    sys_msg = (errno > 0 && errno < sys_nerr) ? sys_errlist[errno] : NULL;
    notice_result = notice_prompt(
				  FRAME_FROM_FOLIO_OR_VIEW(textsw),
				  (Event *) 0,
				  NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
				  NOTICE_TRIGGER, ACTION_STOP,
				  NOTICE_MESSAGE_STRINGS,
				  notice_msg,
				  full_pathname,
				  sys_msg,
				  0,
				  0);
    free(full_pathname);
#ifdef SUNDAE
    free(full_pathname_wc);
#endif    
    return (result);
}
#ifdef OW_I18N
/* Returns 0 iff change directory succeeded. */
pkg_private int
textsw_change_directory_quietly(textsw, filename_wc, err_msgs, might_not_be_dir, locx, locy)
    Textsw_folio    textsw;
    CHAR            *filename_wc, *err_msgs;
#else
/* Returns 0 iff change directory succeeded. */
pkg_private int
textsw_change_directory_quietly(textsw, filename, err_msgs, might_not_be_dir, locx, locy)
    Textsw_folio    textsw;
    CHAR            *filename, *err_msgs;
#endif   
    int             might_not_be_dir;
    int             locx, locy;
{
    char            *sys_msg;
#ifdef OW_I18N
    CHAR            *sys_msg_wc;
#endif
    CHAR            *full_pathname;
    struct stat     stat_buf;
    int             result = 0;
#ifdef OW_I18N
    char            *filename = wcstombsdup(filename_wc);   
#endif       
    

    errno = 0;
    if (stat(filename, &stat_buf) < 0) {
	result = -1;
	goto Error;
    }
    if ((stat_buf.st_mode & S_IFMT) != S_IFDIR) {
	if (might_not_be_dir)
	    return (-2);
    }
    if (chdir(filename) < 0) {
	result = errno;
	goto Error;
    }
    textsw_notify((Textsw_view_handle) textsw,	/* Cast is for lint */
		  TEXTSW_ACTION_CHANGED_DIRECTORY, filename, 
#ifdef OW_I18N
		  TEXTSW_ACTION_CHANGED_DIRECTORY_WCS, filename_wc, 
#endif		  
		  0);
#ifdef OW_I18N
    if (filename)
        free(filename);
#endif		  
    return (result);

Error:
#ifdef OW_I18N
    full_pathname = textsw_full_pathname(filename_wc);
#else
    full_pathname = textsw_full_pathname(filename);
#endif 
#ifdef OW_I18N
    (void) SPRINTF(err_msgs, "%s:",
		   (might_not_be_dir ? XV_I18N_MSG("xv_messages", "Unable to access file") : 
			XV_I18N_MSG("xv_messages", "Unable to cd to directory")));
#else
    (void) sprintf(err_msgs, "Cannot %s '%s': ",
		   (might_not_be_dir ? "access file" : "cd to directory"),
		   full_pathname);
#endif
    free(full_pathname);
    sys_msg = (errno > 0 && errno < sys_nerr) ? sys_errlist[errno] : NULL;
#ifdef OW_I18N
    if (sys_msg) {
	sys_msg_wc = mbstowcsdup(sys_msg);
	STRCAT (err_msgs, sys_msg);
	free(sys_msg_wc);
    }
#else
    if (sys_msg)
	STRCAT (err_msgs, sys_msg);
#endif
    return (result);
}

#ifndef SVR4
pkg_private     Es_status
#else SVR4
static     Es_status
#endif SVR4
textsw_checkpoint_internal(folio)
    Textsw_folio    folio;
{
    Pkg_private Es_handle es_file_create();
    Es_handle       cp_file;
    Es_status       result;

    if (!folio->checkpoint_name) {
	CHAR            *name;
	if (textsw_file_name(folio, &name) != 0)
	    return (ES_CANNOT_GET_NAME);
	if ((folio->checkpoint_name = (CHAR *) MALLOC(MAXNAMLEN)) == 0)
	    return (ES_CANNOT_GET_NAME);
	(void) SPRINTF(folio->checkpoint_name, "%ws%%%%", name);
    }
    cp_file = es_file_create(folio->checkpoint_name,
			     ES_OPT_APPEND, &result);
    if (!cp_file) {
	/* BUG ALERT!  For now, don't look at result. */
	return (ES_CANNOT_OPEN_OUTPUT);
    }
    result = es_copy(folio->views->esh, cp_file, TRUE);
    es_destroy(cp_file);

    return (result);
}


/*
 * If the number of edits since the last checkpoint has exceeded the
 * checkpoint frequency, take a checkpoint. Return ES_SUCCESS if we do the
 * checkpoint.
 */
pkg_private     Es_status
textsw_checkpoint(folio)
    Textsw_folio    folio;
{
    Textsw_view_handle view = VIEW_FROM_FOLIO_OR_VIEW(folio);
    int             edit_number = (int)
    ev_get((Ev_handle) view->e_view,
	   EV_CHAIN_EDIT_NUMBER);
    Es_status       result = ES_DID_NOT_CHECKPOINT;

    if (folio->state & TXTSW_IN_NOTIFY_PROC ||
	folio->checkpoint_frequency <= 0)
	return (result);
    if ((edit_number / folio->checkpoint_frequency)
	> folio->checkpoint_number) {
	/* do the checkpoint */
	result = textsw_checkpoint_internal(folio);
	if (result == ES_SUCCESS) {
	    folio->checkpoint_number++;
	}
    }
    return (result);
}

Xv_private int
textsw_empty_document(abstract, ie)      /* returns XV_OK or XV_ERROR */
    Textsw          abstract;
    Event          *ie;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Textsw_folio textsw = FOLIO_FOR_VIEW(view);
    int             has_been_modified =
    textsw_has_been_modified(abstract);
    int             number_of_resets = 0;
    int             is_cmdtool =
    (textsw->state & TXTSW_NO_RESET_TO_SCRATCH);
    int             result;
    int             loc_x = (ie) ? ie->ie_locx : 0;
    int             loc_y = (ie) ? ie->ie_locy : 0;

    if (has_been_modified) {
	result = notice_prompt(
			       FRAME_FROM_FOLIO_OR_VIEW(view),
			       (Event *) 0,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			   XV_I18N_MSG("xv_messages", "\
The text has been edited.\n\
Clear Log will discard these edits. Please confirm."),
#else
			       "The text has been edited.",
		      "Clear Log will discard these edits. Please confirm.",
#endif
			       0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Confirm, discard edits"),
#else
				"Confirm, discard edits",
#endif
			       NOTICE_BUTTON_NO, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Cancel"),
#else
				"Cancel",
#endif
			       0);
	if (result == NOTICE_NO)
	    return XV_ERROR;
	textsw_reset(abstract, loc_x, loc_y);
	number_of_resets++;
    }
    if (is_cmdtool) {
	if ((has_been_modified) && (number_of_resets == 0))
	    if (number_of_resets == 0)
		textsw_reset(abstract, loc_x, loc_y);
    } else {
	textsw_reset(abstract, loc_x, loc_y);
    }
    return XV_OK;
}

pkg_private void
textsw_post_need_selection(abstract, ie)
    Textsw          abstract;
    Event          *ie;		/* ignored, check for NIL if ever used */
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    int             result;

    result = notice_prompt(
			   FRAME_FROM_FOLIO_OR_VIEW(view),
			   (Event *) 0,
			   NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
	XV_I18N_MSG("xv_messages",
		"Please select a file name and choose this option again."),
#else
		  "Please select a file name and choose this option again.",
#endif
			   0,
			   NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			   NOTICE_TRIGGER, ACTION_STOP,
			   0);

}

pkg_private void
textsw_load_file_as_menu(abstract, ie)
    Textsw          abstract;
    Event          *ie;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Textsw_folio textsw = FOLIO_FOR_VIEW(view);
    int             result, no_cd;
    int             loc_x = (ie) ? ie->ie_locx : 0;
    int             loc_y = (ie) ? ie->ie_locy : 0;

    if (textsw_has_been_modified(abstract)) {
	result = notice_prompt(
			       xv_get(abstract, WIN_FRAME),
			       (Event *) 0,
			       NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "\
The text has been edited.\n\
Load File will discard these edits. Please confirm."),
#else
			       "The text has been edited.",
		      "Load File will discard these edits. Please confirm.",
#endif
			       0,
			       NOTICE_BUTTON_YES, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Confirm, discard edits"),
#else
				"Confirm, discard edits",
#endif
			       NOTICE_BUTTON_NO, 
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Cancel"),
#else
				"Cancel",
#endif
			       0);
	if (result == NOTICE_NO)
	    return;
    }
    no_cd = ((textsw->state & TXTSW_NO_CD) == 0);
    if (textsw_load_selection(textsw,
			      loc_x, loc_y, no_cd) == 0) {
    }
}

/*
 * return 0 if textsw was readonly, 1 if should allow insert, accelerator not
 * translated 2 shouldn't insert, accelerator translated
 */
pkg_private int
textsw_handle_esc_accelerators(abstract, ie)
    Textsw          abstract;
    Event          *ie;
{
    register Textsw_view_handle view = VIEW_ABS_TO_REP(abstract);
    register Textsw_folio textsw = FOLIO_FOR_VIEW(view);
    CHAR             dummy[1024];

    if (event_shift_is_down(ie)) {	/* select & include current line */
	unsigned        span_to_beginning_flags =
	(EI_SPAN_LINE | EI_SPAN_LEFT_ONLY);
	unsigned        span_line_flags =
	(EI_SPAN_LINE | EI_SPAN_RIGHT_ONLY);
	Ev_chain        chain = textsw->views;
	Es_index        first = 0;
	Es_index        last_plus_one, insert_pos;
	extern int      FILE_STUFF_POPUP_KEY;
	Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
					       FILE_STUFF_POPUP_KEY);

	textsw_flush_caches(view, TFC_STD);
	insert_pos = EV_GET_INSERT(chain);
	if (insert_pos != 0) {
	    /* first go left from insertion pt */
	    (void) ev_span(chain, insert_pos,
			   &first, &last_plus_one, span_to_beginning_flags);
	}
	/* then go right to end of line from line beginning */
	(void) ev_span(chain, first,
		       &first, &last_plus_one, span_line_flags);
	/* check for no selection because at beginning of new line */
	if ((first == last_plus_one) && (insert_pos != 0)) {
	    insert_pos--;
	    first = 0;
	    last_plus_one = 0;
	    if (insert_pos != 0) {
		/* first go left from insertion pt */
		(void) ev_span(chain, insert_pos,
			   &first, &last_plus_one, span_to_beginning_flags);
	    }
	    /* then go right to end of line from line beginning */
	    (void) ev_span(chain, first,
			   &first, &last_plus_one, span_line_flags);
	}
	textsw_set_insert(textsw, last_plus_one);
	textsw_set_selection(abstract,
		first, last_plus_one, (EV_SEL_PRIMARY | EV_SEL_PD_PRIMARY));
	/*
	 * if (textsw_get_selection_as_filename(textsw, file_str,
	 * SIZEOF(file_str), first, last_plus_one) == 0)
	 */
	if (popup) {
	    (void) textsw_set_dir_str((int) TEXTSW_MENU_FILE_STUFF);
	    (void) textsw_get_and_set_selection(popup, view,
					      (int) TEXTSW_MENU_FILE_STUFF);
	} else {
	    (void) textsw_create_popup_frame(view, (int) TEXTSW_MENU_FILE_STUFF);
	}

    } else if (0 == textsw_file_name(textsw, dummy)) {
	if (TXTSW_IS_READ_ONLY(textsw))
	    return (0);
	return (1);

    } else {			/* select and load-file current line */
	unsigned        span_to_beginning_flags =
	(EI_SPAN_LINE | EI_SPAN_LEFT_ONLY);
	unsigned        span_line_flags =
	(EI_SPAN_LINE | EI_SPAN_RIGHT_ONLY);
	Ev_chain        chain = textsw->views;
	Es_index        first = 0;
	Es_index        last_plus_one, insert_pos;
	extern int      LOAD_FILE_POPUP_KEY;
	Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
					       LOAD_FILE_POPUP_KEY);

	textsw_flush_caches(view, TFC_STD);
	insert_pos = EV_GET_INSERT(chain);
	if (insert_pos != 0) {
	    /* first go left from insertion pt */
	    (void) ev_span(chain, insert_pos,
			   &first, &last_plus_one, span_to_beginning_flags);
	}
	/* then go right to end of line from line beginning */
	(void) ev_span(chain, first,
		       &first, &last_plus_one, span_line_flags);
	if ((first == last_plus_one) && (insert_pos != 0)) {
	    insert_pos--;
	    first = 0;
	    last_plus_one = 0;
	    if (insert_pos != 0) {
		/* first go left from insertion pt */
		(void) ev_span(chain, insert_pos,
			   &first, &last_plus_one, span_to_beginning_flags);
	    }
	    /* then go right to end of line from line beginning */
	    (void) ev_span(chain, first,
			   &first, &last_plus_one, span_line_flags);
	}
	textsw_set_insert(textsw, last_plus_one);
	textsw_set_selection(abstract, first, last_plus_one, EV_SEL_PRIMARY);

	if (popup) {
	    (void) textsw_set_dir_str((int) TEXTSW_MENU_LOAD);
	    (void) textsw_get_and_set_selection(popup, view,
						(int) TEXTSW_MENU_LOAD);
	} else {
	    (void) textsw_create_popup_frame(view, (int) TEXTSW_MENU_LOAD);
	}
	/*
	 * textsw->state &= ~TXTSW_READ_ONLY_ESH;
	 */
    }
    return (2);
}

#if defined(DEBUG) && !defined(lint)
static char    *header = "fd      dev: #, type    inode\n";
static void
debug_dump_fds(stream)
    FILE           *stream;
{
    register int    fd;
    struct stat     s;

    if (stream == 0)
	stream = stderr;
    fprintf(stream, header);
    for (fd = 0; fd < 32; fd++) {
	if (fstat(fd, &s) != -1) {
	    fprintf(stream, "%2d\t%6d\t%4d\t%5d\n",
		    fd, s.st_dev, s.st_rdev, s.st_ino);
	}
    }
}

#endif
