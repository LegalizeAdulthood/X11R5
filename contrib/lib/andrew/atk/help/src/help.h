/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.h,v 1.41 1991/09/12 19:41:10 bobg Exp $ */
/* $ACIS$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/help/src/RCS/help.h,v 1.41 1991/09/12 19:41:10 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*---------------------------------------------------------------------------*/
/*	MODULE: help.h							     */
/*		Contains error messages and static structures used within    */
/*		the help object.					     */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*			     INTERNAL CONSTANTS				     */
/*---------------------------------------------------------------------------*/

static char *overview_ext_array[] = OVERVIEW_EXTS;
static char *program_ext_array[] = PROGRAM_EXTS;

/*
 * error messages
 */

static char *err_generic = "Unspecified error.";
static char *err_unexpected = "Unexpected failure to get help on '%s'.";
static char *err_server = "Sorry; a file server is down.";
static char *err_missing = "Sorry; help file '%s' appears to be missing.";
static char *err_read = "Sorry; could not read help file.";
static char *err_sorry = "Sorry; no help available on '%s'.";
static char *err_index = "Sorry; index %s not found.  Cannot find entries.";
static char *err_no_more = "No more help entries\n";
static char *err_file = "Help file %s not found.\n";
static char *err_no_sel = "Sorry; you must first select a help topic.";
static char *err_sel_too_long = "Selected region (topic) too long.";
static char *err_no_new_view = "Can't make new window.";
static char *err_view = "Can't create a new %s view.";
static char *err_dobj = "Can't create a new %s dataobject.";
static char *err_nontext = "Can't display non-text %s dataobject.";
static char *err_readonly = "Buffer is read-only.";
static char *err_noproc = "Can't find procedure.";
static char *err_openhelpdir = "Cannot get help files.";
static char *err_terminal =
    "The help entry you selected runs a window-manager program to show\n\
documentation.  This cannot be done on a terminal.";

/*
 * informational messages
 */
static char *msg_print_queue = "Queueing files for printing; please wait...";
static char *msg_queue_done = "Your help file has been queued for printing.";
static char *msg_expanding = "Expanding program list; please wait...";
static char *msg_comment = "Preparing sending window; it should appear soon.";
static char *msg_new_win = "Preparing new window; it should appear soon.";
static char *msg_converting = "Processing troff file; please wait...";
static char *msg_dir_prompt = "Enter a directory name to add to the searchpath: ";
static char *msg_filter_type_prompt = "What kind of filtering?";
static char *msg_filter_prompt = "Enter the filter string: ";
static char *msg_print_prompt = "Print the file %s? [yes, no]? ";
static char *msg_term_prompt =
    "See the next help file (%s) [yes, no, quit]? ";
static char *msg_ask_prompt =
    "Enter a keyword about which you want more help: ";
