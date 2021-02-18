#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sel_agent.c 50.9 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#define DEBUG

#define USE_COMPOUND_TEXT

#include <xview/xview.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xview/server.h>
#include <xview/sel_svc.h>
#include <xview/sel_attrs.h>
#include <xview_private/sel_impl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/file.h>
#include <xview/sel_compat.h>
#include <X11/Xatom.h>
#include <stdio.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

/*
 * The following header file provides fd_set compatibility with SunOS for
 * Ultrix
 */
#include <xview_private/ultrix_cpt.h>

#ifdef OW_I18N
static void     	selection_agent_process_functions();
static Seln_result 	selection_agent_process_request();
static Seln_result      sel_get_integer();
static Seln_result      sel_get_ascii();
static Seln_result      sel_get_wcs();
static Seln_attribute 	convert_target_to_attr();
static Atom     	convert_attr_to_target();
static XSelectionEvent 	*ask_selection_owner();
static void     	selection_agent_do_text_content();
static void     	selection_agent_do_request();
static void     	selection_agent_do_multiple();
static void     	selection_agent_do_timestamp();
static void     	selection_agent_do_target();
static Atom     	get_property_atom();
static Seln_result 	agent_do_yield();
static void     	send_SelectionNotify();
static void     	block();
static int      	is_blocking_over();
static Seln_result 	seln_do_request_from_file();
static void     	selection_agent_do_function();
static Atom 		get_atom();
static Seln_attribute 	save_atom();
#else
static void     selection_agent_process_functions();
static Seln_result selection_agent_process_request();
static Seln_result get_ascii_content();
static Seln_attribute convert_target_to_attr();
static Atom     convert_attr_to_target();
static Seln_result get_seln_int_request();
static XSelectionEvent *ask_selection_owner();
static void     selection_agent_do_ascii_content();
static void     selection_agent_do_request();
static void     selection_agent_do_multiple();
static void     selection_agent_do_timestamp();
static void     selection_agent_do_target();
static Atom     get_property_atom();
static Seln_result agent_do_yield();
static void     send_SelectionNotify();
static void     block();
static int      is_blocking_over();
static Seln_result seln_do_request_from_file();
static void     selection_agent_do_function();
static Atom 	get_atom();
static Seln_attribute save_atom();
#endif

Xv_private Seln_result 	seln_convert_request_to_property();

/* called by seln_svc.c
 * to do selection
 * between SV1 and SV2
 */

Xv_private Time server_get_timestamp();
Xv_private void selection_agent_selectionrequest();
Xv_private void selection_agent_clear();
Pkg_private void seln_give_up_selection();
Pkg_private void selection_init_holder_info();
extern int seln_debug;


/*
 * Initialize the selection service
 */
Xv_private int
selection_init_agent(server, screen)
    Xv_Server       server;
    Xv_Screen       screen;
{
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_agent_info *agent;
    int             i;
    Xv_Window       root_window =
	(Xv_Window) xv_get(screen, (Attr_attribute)XV_ROOT);
    Xv_Window       window;

    agent = xv_alloc(Seln_agent_info);
    agent->agent_holder.access.client =
	(char *) selection_create(server,
				  selection_agent_process_functions,
				  selection_agent_process_request,
				  (char *) server);
    if (!agent->agent_holder.access.client) {
	complain("Selection service: unable to create service client");
	exit(1);
    }
    agent->agent_holder.state = SELN_NONE;
    agent->agent_holder.rank = SELN_UNKNOWN;
    agent->agent_holder.access.pid = getpid();
    agent->timeout = SELN_STD_TIMEOUT_SEC + 1;

    if ((window = xv_create(root_window, WINDOW, WIN_SELECTION_WINDOW,
			    XV_SHOW, FALSE, 0)) == NULL) {
	complain("Could not create window");
	exit(1);
    }
    agent->xid = (XID) xv_get(window, (Attr_attribute)XV_XID);

    for (i = 0; i < SELN_RANKS; i++) {
	bzero((char *) &agent->client_holder[i], sizeof(Seln_holder));
	agent->client_holder[i].state = SELN_NONE;
	agent->client_holder[i].rank = (Seln_rank) i;
    }
    agent->get_prop.offset = 0;
    agent->get_prop.property = None;

    if ((CLIPBOARD(agent) =
	 XInternAtom(display, "CLIPBOARD", FALSE)) == None) {
	complain("No CLIPBOARD atom");
	exit(1);
    }
    if ((CARET(agent) =
	 XInternAtom(display, "_SUN_SELN_CARET", FALSE)) == None) {
	complain("No _SUN_SELN_CARET atom");
	exit(1);
    }
    (void) xv_set(server, XV_KEY_DATA, SELN_AGENT_INFO, agent, NULL);
}

/*
 * Called by the library when a SV client has a seen a function key state
 * change (UP events only) and it thinks that the agent should be the
 * recipient of this event. The agent in turn will have to send a msg to
 * another selection agent, which in turn will intrepret the message and call
 * the actual recipient's function call back.
 */
static void
selection_agent_process_functions(server, buffer)
    Xv_Server       server;
    Seln_function_buffer *buffer;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Window	    win = agent->xid;
    Display        *dpy =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Atom            selection = seln_rank_to_selection(buffer->addressee_rank,
						  agent);
    unsigned char   data[sizeof(Seln_function) + sizeof(Seln_rank)];
    Time            time = (Time) server_get_timestamp(server);
    Atom            property = get_property_atom(dpy, agent);
    Atom	    func_atom = get_atom(dpy, win, 
					 DO_FUNCTION(&agent->targets));
    int             len = (sizeof(Seln_function) + sizeof(Seln_rank) + 3) /
    sizeof(int);

    /* Send function key and rank info to the owner of the selection */
    bcopy((char *) &buffer->function, 
	  (char *) data, 
	  sizeof(Seln_function));
    bcopy((char *) &buffer->addressee_rank, 
	  (char *) (data + sizeof(Seln_function)),
	  sizeof(Seln_rank));

    XChangeProperty(dpy, win, property, func_atom, 32, PropModeReplace,
		    data, len);

    /* No need to block, as nothing is expected back */
    (void) ask_selection_owner(dpy, agent, selection, func_atom, property,
								   time, FALSE);
}

/*
 * Reply proc for the agent. This is called by the selection library when the
 * agent holds a selection and some other client within the same process
 * requests that selection. When an agent holds the selection, the selection
 * is actually held by another process. This routine gets selection from X
 * server and converts them into xview style.
 */

static          Seln_result
selection_agent_process_request(attr, context, max_length)
    Seln_attribute  attr;
    register Seln_replier_data *context;
    int             max_length;
{
    Seln_result     result;
    Xv_Server       server = (Xv_Server) context->client_data;
    Display         *dpy =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Atom            target;
    Atom            selection;

    if (agent->client_holder[ord(context->rank)].state == SELN_FILE)
	return (seln_do_request_from_file(attr, context,
		  agent->held_file[ord(context->rank)], max_length, agent));

    target = convert_attr_to_target(dpy, agent, attr);
    selection = seln_rank_to_selection(context->rank, agent);
    switch (attr) {
      case SELN_REQ_BYTESIZE:
      case SELN_REQ_OBJECT_SIZE:
      case SELN_REQ_FIRST:
      case SELN_REQ_FIRST_UNIT:
      case SELN_REQ_LAST:
      case SELN_REQ_LAST_UNIT:
      case SELN_REQ_LEVEL:
      case SELN_REQ_FAKE_LEVEL:
      case SELN_REQ_SET_LEVEL:
      case SELN_REQ_IS_READONLY:
      case SELN_REQ_YIELD:
      case SELN_REQ_FUNC_KEY_STATE:
#ifdef OW_I18N
      case SELN_REQ_CHARSIZE:
	result = sel_get_integer(server, selection, context, target);
#else
	result = get_seln_int_request(server, selection,
				      context, target);
#endif
	break;

      case SELN_REQ_SELECTED_WINDOWS:
      case SELN_REQ_CONTENTS_ASCII:
      case SELN_REQ_CONTENTS_OBJECT:
      case SELN_REQ_CONTENTS_PIECES:
      case SELN_REQ_FILE_NAME:
#ifdef OW_I18N
	result = sel_get_ascii(server, selection, context, max_length, target);
#else
	/*
	 * Get the content of the selection from another process and stuff
	 * into context->response_pointer
	 */
	result = get_ascii_content(server, selection, context,
				   max_length, target);
#endif
	break;

#ifdef OW_I18N
      case SELN_REQ_CONTENTS_WCS:
	result = sel_get_wcs(server, selection, context, max_length, target);
	break;
#endif

      case SELN_REQ_COMMIT_PENDING_DELETE:
      case SELN_REQ_DELETE:
      case SELN_REQ_RESTORE:{
	    /* These require no return value */
	    Seln_agent_info *agent2 =
		(Seln_agent_info *) xv_get(server,
					   (Attr_attribute)XV_KEY_DATA,
					   SELN_AGENT_INFO);
	    Time            time = (Time) server_get_timestamp(server);
	    Atom            property = get_property_atom(dpy, agent2);

	    (void) ask_selection_owner(dpy, agent2, selection,
				       target, property, time, FALSE);
	    result = SELN_SUCCESS;
	    *context->response_pointer++ = (caddr_t) SELN_SUCCESS;
	    break;
	}

      case SELN_REQ_END_REQUEST:
	result = SELN_SUCCESS;
	break;

      default:
	result = SELN_UNRECOGNIZED;
	break;
    }
    return result;
}


/*
 * seln_svc_do_reply: respond to a request concerning a selection for which
 * selection service holds a file.
 */
static          Seln_result
seln_do_request_from_file(attr, context, fd, max_length, agent)
    Seln_attribute  attr;
    register Seln_replier_data *context;
    int             fd;
    int             max_length;
    Seln_agent_info *agent;
{
    struct stat     stat_buf;
    int             count, size;
    char           *destp;
    extern long     lseek();

    if (fstat(fd, &stat_buf) != 0) {
	perror(
#ifdef OW_I18N
	    XV_I18N_MSG("xv_messages", "Agent couldn't reply about a file")
#else
	    "Agent couldn't reply about a file"
#endif
	    );
	return SELN_FAILED;
    }
    if (context->context == 0) {
	if (lseek(fd, 0L, 0) == (long) -1) {
	    perror(
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "Agent couldn't reset to start of file")
#else
		"Agent couldn't reset to start of file"
#endif
		);
	    return SELN_FAILED;
	}
    }
    switch (attr) {
      case SELN_REQ_BYTESIZE:
	*context->response_pointer++ = (char *) stat_buf.st_size;
	return SELN_SUCCESS;
      case SELN_REQ_CONTENTS_ASCII:
	size = stat_buf.st_size - (int) context->context;
	if (size > max_length) {
	    count = read(fd, (char *) context->response_pointer, max_length);
	    if (count != max_length) {
		goto terminate_buffer;
	    }
	    context->response_pointer = (char **)
		((char *) context->response_pointer + count);
	    context->context += count;
	    return SELN_CONTINUED;
	} else {
	    count = read(fd, (char *) context->response_pointer, size);
    terminate_buffer:
	    destp = (char *) context->response_pointer;
	    destp += count;
	    while ((int) destp % 4 != 0) {
		*destp++ = '\0';
	    }
	    context->response_pointer = (char **) destp;
	    *context->response_pointer++ = (char *) 0;
	    return SELN_SUCCESS;
	}
      case SELN_REQ_YIELD:
	*context->response_pointer++ =
	    (char *) agent_do_yield(agent, context->rank);
	return SELN_SUCCESS;
      case SELN_REQ_END_REQUEST:
	return SELN_SUCCESS;
      default:
	return SELN_UNRECOGNIZED;
    }
}

/*
 * Yield the selection owned by the agent.
 */
static          Seln_result
agent_do_yield(agent, rank)
    Seln_agent_info *agent;
    Seln_rank       rank;
{
    if (agent == NULL)
	return (SELN_DIDNT_HAVE);
    if (ord(rank) < ord(SELN_PRIMARY) || ord(rank) > ord(SELN_SHELF) ||
	agent->client_holder[ord(rank)].state != SELN_FILE) {
	return (SELN_DIDNT_HAVE);
    } else {
	agent->client_holder[ord(rank)].state = SELN_NONE;
	(void) close(agent->held_file[ord(rank)]);
	agent->held_file[ord(rank)] = 0;
	return (SELN_SUCCESS);
    }
}

#ifdef OW_I18N
static Seln_result
sel_get_selection(server, display, agent, selection, target)
    Xv_Server           server;
    Display             *display;
    Seln_agent_info     *agent;
    Atom                selection;
    Atom                target;
{
    XSelectionEvent     *sel_event;
    Time                time = (Time)server_get_timestamp(server);
    Atom                property = get_property_atom(display, agent);

    sel_event = ask_selection_owner(display, agent, selection, target,
                        property, time, TRUE);
 
    if (sel_event->target != target)
        return(SELN_FAILED);
    else if ((agent->get_prop.property = sel_event->property) == None)
        return(SELN_UNRECOGNIZED);
    else
        return(SELN_SUCCESS);
}
 
 
static Seln_result
sel_get_integer(server, selection, context, target)
    Xv_Server       server;
    Atom            selection;
    Seln_replier_data *context;
    Atom            target;
{
    Seln_result         result;
    Seln_agent_info     *agent =
        (Seln_agent_info *)xv_get(server, XV_KEY_DATA, SELN_AGENT_INFO);
    Display             *display =  (Display *)xv_get(server, XV_DISPLAY);
    unsigned long       nitems_return, bytes_remaining;
    unsigned char       *buffer;
    Atom                return_type;
    int                 format;
    int                 value;

    if ((result = sel_get_selection(server, display, agent, selection,
                        target)) !=  SELN_SUCCESS)
        return(result);

    /* Get property and interpret the results */
    if (XGetWindowProperty(display, agent->xid, agent->get_prop.property,
                           0L, 2, TRUE, AnyPropertyType, &return_type,
                           &format, &nitems_return,
                           &bytes_remaining, &buffer) == Success) {

        if (return_type == None) {
            result = SELN_FAILED;
        } else {
            bcopy((char *) buffer, (char *) &value, sizeof(int));
            *context->response_pointer++ = (caddr_t) value;
        }
    }    
    return result;
}


static Seln_result
sel_get_ascii(server, selection, replier_data, length, target)
    Xv_Server           server;
    Atom                selection;
    Seln_replier_data   *replier_data;
    int                 length; /* buffer size */
    Atom                target;
{
    Seln_result         result;
    Seln_agent_info     *agent =
        (Seln_agent_info *)xv_get(server, XV_KEY_DATA, SELN_AGENT_INFO);
    Display             *display =  (Display *)xv_get(server, XV_DISPLAY);
    unsigned long       nitems_return, bytes_remaining;
    unsigned char       *buffer;
    Atom                return_type;
    int                 format;
    int                 value;
    char                *temp = (char *) replier_data->response_pointer;
    int			buf_length, max_length;
 

    /*
     * This function may be called several times for a particular selection
     * as part of a selection_query(). The first time this function is called
     * for a selection request, ask selection holder to write the property
     * on the server and copy the entire property over to the client.
     */
    if (!agent->get_prop.offset) {
       if ((result = sel_get_selection(server, display, agent, 
			selection, target)) != SELN_SUCCESS)
		return(result);

	/* find the length of the property in bytes */
	if (XGetWindowProperty(display, agent->xid, agent->get_prop.property,
		0, 0, False, AnyPropertyType, &return_type, &format,
		&nitems_return, &bytes_remaining, &buffer) != Success) 
	    return(SELN_FAILED);
	XFree(buffer);

	/*
	 * bytes_remaining is the length of property in bytes.
	 * Read the entire property over to the client side.
	 */
	if (XGetWindowProperty(display, agent->xid, agent->get_prop.property, 
		0, (bytes_remaining + 3)/4, True,  AnyPropertyType, 
		&return_type, &format, &nitems_return, &bytes_remaining,
		&buffer) != Success) 
	    return(SELN_FAILED);

#ifdef DEBUG
 	if (bytes_remaining != 0)
	    xv_error(NULL,
		ERROR_STRING, "Property not deleted on server during selection",
		0);
#endif DEBUG

	/* save buffer */
	replier_data->context = (char *)buffer;
    }  

    buf_length = strlen(replier_data->context + agent->get_prop.offset);
    max_length = ((length - 1)/4) * 4;

    bcopy(replier_data->context + agent->get_prop.offset, temp,
		MIN(buf_length, max_length));
    temp += MIN(buf_length, max_length);
    while ((unsigned) temp % sizeof(*replier_data->response_pointer))
	*temp++ = '\0';
    replier_data->response_pointer = (char **) temp;
    *replier_data->response_pointer++ = 0;

    if (buf_length <= max_length) {
	agent->get_prop.offset = 0;
	XFree(replier_data->context);
	 result = SELN_SUCCESS;
    } else {
	agent->get_prop.offset += max_length;
	result = SELN_CONTINUED;
    }

    return(result);
}

static Seln_result
sel_get_wcs(server, selection, replier_data, length, target)
    Xv_Server           server;
    Atom                selection;
    Seln_replier_data   *replier_data;
    int                 length; /* buffer size */
    Atom                target;
{
    Seln_result         result;
    Seln_agent_info     *agent =
        (Seln_agent_info *)xv_get(server, XV_KEY_DATA, SELN_AGENT_INFO);
    Display             *display =  (Display *)xv_get(server, XV_DISPLAY);
    unsigned long       nitems_return, bytes_remaining;
    unsigned char       *buffer;
    Atom                return_type;
    int                 format;
    int                 value;
    CHAR		*temp = (CHAR *) replier_data->response_pointer;
    int			buf_length, max_length, num_wchars;
 

    /*
     * This function may be called several times for a particular selection
     * as part of a selection_query(). The first time this function is called
     * for a selection request, ask selection holder to write the property
     * on the server and copy the entire property over to the client.
     */
    if (!agent->get_prop.offset) {
       /* selection transfer is through compound text */
       target = convert_attr_to_target(display, agent, SELN_REQ_CONTENTS_CT);

       if ((result = sel_get_selection(server, display, agent, 
			selection, target)) != SELN_SUCCESS)
		return(result);

	/* find the length of the property in bytes */
	if (XGetWindowProperty(display, agent->xid, agent->get_prop.property,
		0, 0, False, AnyPropertyType, &return_type, &format,
		&nitems_return, &bytes_remaining, &buffer) != Success) 
	    return(SELN_FAILED);
	XFree(buffer);

	/*
	 * bytes_remaining is the length of property in bytes.
	 * Read the entire property over to the client side.
	 */
	if (XGetWindowProperty(display, agent->xid, agent->get_prop.property, 
		0, (bytes_remaining + 3)/4, True,  AnyPropertyType, 
		&return_type, &format, &nitems_return, &bytes_remaining,
		&buffer) != Success) 
	    return(SELN_FAILED);

#ifdef DEBUG
 	if (bytes_remaining != 0)
	    xv_error(NULL,
		ERROR_STRING, "Property not deleted on server during selection",
		0);
#endif DEBUG

#ifdef USE_COMPOUND_TEXT
	replier_data->context = (char *)ctstowcsdup(buffer);
	XFree(buffer);
#else USE_COMPOUND_TEXT
	replier_data->context = (char *)buffer;
#endif USE_COMPOUND_TEXT
    }  

    /* 
     * String lengths and offsets refer to wide character lengths 
     * and offsets and not byte lengths and byte offsets.
     */
    buf_length = 
	STRLEN(((CHAR *)replier_data->context) + agent->get_prop.offset);
    max_length = (length/sizeof(CHAR) - 1)/4 * 4;
    if (buf_length <= max_length) {
	STRCPY(temp, ((CHAR *)replier_data->context) + 
		    agent->get_prop.offset);
	temp += buf_length;
    } else {
        STRNCPY(temp, ((CHAR *)replier_data->context) + 
	    agent->get_prop.offset, max_length);
	temp += max_length;
	/* Null teminate the wide char string */
	*temp++ = '\0';
	*temp++ = '\0';
    }

    while ((unsigned) temp % sizeof(*replier_data->response_pointer))
	*temp++ = '\0';

    replier_data->response_pointer = (char **) temp;
    *replier_data->response_pointer++ = 0;

    if (buf_length <= max_length) {
	agent->get_prop.offset = 0;
	free(replier_data->context);
	result = SELN_SUCCESS;
    } else {
	agent->get_prop.offset += max_length;
	result = SELN_CONTINUED;
    }
    return(result);
}
 

#else 
static          Seln_result
get_seln_int_request(server, selection, context, target)
    Xv_Server       server;
    Atom            selection;
    Seln_replier_data *context;
    Atom            target;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_result     result;
    Time            time = (Time) server_get_timestamp(server);
    XSelectionEvent *selection_event;
    unsigned long   nitems_return, bytes_remaining;
    unsigned char  *buffer;
    Atom            return_type;
    int             format;
    int             value;
    Atom            property = get_property_atom(display, agent);

    result = SELN_SUCCESS;

    selection_event = ask_selection_owner(display, agent, selection, target,
					  property, time, TRUE);
    if (selection_event->target != target)
	return (SELN_FAILED);
    /* Unrecognized request */
    if ((agent->get_prop.property = selection_event->property) == None)
	return (SELN_UNRECOGNIZED);

    /* Get property and interpert the results */
    if (XGetWindowProperty(display, agent->xid, agent->get_prop.property,
			   0L, 2, TRUE, AnyPropertyType, &return_type, 
			   &format, &nitems_return,
			   &bytes_remaining, &buffer) == Success) {

	if (return_type == None) {
	    result = SELN_FAILED;
	} else {
	    bcopy((char *) buffer, (char *) &value, sizeof(int));
	    *context->response_pointer++ = (caddr_t) value;
	}
    }
    return result;
}


static          Seln_result
get_ascii_content(server, selection, context, length, target)
    Xv_Server       server;
    Atom            selection;
    register Seln_replier_data *context;
    int             length;	/* buffer size */
    Atom            target;
{
    XSelectionEvent *selection_event;
    Time            time;
    Atom            return_type;
    int             format;
    unsigned long   nitems_return, bytes_remaining;
    Display        *display;
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    unsigned char  *buffer;
    char           *temp = (char *) context->response_pointer;
    Seln_result     result;
    Atom            property;

    result = SELN_SUCCESS;

    display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    /* Read Property only the first time */
    if (!agent->get_prop.offset) {
	time = (Time) server_get_timestamp(server);
	property = get_property_atom(display, agent);
	selection_event = ask_selection_owner(display, agent, selection,
					      target, property, time, TRUE);
	if (selection_event->target != target)
	    return (SELN_FAILED);
	if ((agent->get_prop.property = selection_event->property) == None)
	    return (SELN_UNRECOGNIZED);
    }
    if (agent->get_prop.property == None)
	return (SELN_UNRECOGNIZED);

    /* For now deal with small data */
    if (XGetWindowProperty(display, agent->xid, agent->get_prop.property,
			   agent->get_prop.offset, (length - 1) / 4, FALSE,
			   AnyPropertyType, &return_type, &format, 
			   &nitems_return, &bytes_remaining, &buffer)
	== Success) {
	
	if (return_type == None) {
	    result = SELN_FAILED;
	} else {
	    /* Check to see if everything was kosher */
	    unsigned int    num_bytes = nitems_return * format / 8;
	    bcopy((char *) buffer, temp, (int) num_bytes);
	    temp += num_bytes;
	    while ((unsigned) temp % sizeof(*context->response_pointer))
		*temp++ = '\0';
	    context->response_pointer = (char **) temp;
	    *context->response_pointer++ = 0;
	    if (bytes_remaining) {
		agent->get_prop.offset += num_bytes / sizeof(long);
		result = SELN_CONTINUED;
	    } else {
		agent->get_prop.offset = 0;
		agent->get_prop.property = None;
		XDeleteProperty(display, agent->xid, return_type);
	    }
	}
    }
    return result;
}
#endif

/*
 * Tell the X server that the agent has the selection specified by the rank.
 */

Pkg_private     Seln_result
selection_agent_acquire(server, asked)
    Xv_Server       server;
    Seln_rank       asked;
{
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);

#ifdef X11_TIME
    Time            time = server_get_timestamp(server);
#else
    Time            time = CurrentTime;
#endif
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Atom            selection = seln_rank_to_selection(asked, agent);

    if (selection != None) {
	agent->seln_acquired_time[(int) asked] = time;
	XSetSelectionOwner(display, selection, agent->xid, time);
	if (XGetSelectionOwner(display, selection) == agent->xid)
	    return (SELN_SUCCESS);
    } else {
	complain("UNKNOWN selection cannot be acquired");
	return (SELN_FAILED);
    }
    return (SELN_FAILED);
}


/*
 * Find out if any other process has the specified selection. In that case,
 * make the agent the holder of the selection.
 */

Pkg_private void
selection_agent_get_holder(server, which, holder)
    Xv_Server       server;
    Seln_rank       which;
    Seln_holder    *holder;
{
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server,
				   (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Window          owner = None;
    Atom            selection = seln_rank_to_selection(which, agent);


    if (selection == None) {
	complain("The selection is not implemented");
    } else
	owner = XGetSelectionOwner(display, selection);

    /* Somebody in this process already owns the selection */
    if (owner == agent->xid) {
	return;
    }
    if (owner != None) {
	agent->agent_holder.rank = which;
	agent->agent_holder.state = SELN_EXISTS;
	*holder = agent->agent_holder;
    }
}

/*
 * Received by the agent if somebody else has acquired the selection or agent
 * gives up the owneship of the selection. [called from func xevent_to_event
 * in win/win_input.c]
 */
Xv_private void
selection_agent_clear(server, clear_event)
    Xv_Server       server;
    XSelectionClearEvent *clear_event;
{
    Seln_rank       rank;
    Seln_holder     holder;
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);

    rank = selection_to_rank(clear_event->selection, agent);
    holder = selection_inquire(server, rank);

    /*
     * ask the current holder to yield. The current holder should not be the
     * agent. Update the holder info.
     */
    /*
     * if (!selection_equal_agent(server, holder)) {
     */
    (void) selection_ask(server, &holder, SELN_REQ_YIELD, rank, 0);
    selection_init_holder_info(server, rank);
    /*
     * }
     */
}


/*
 * Some other process has made a request to the agent to give it the contents
 * of the selection the agent owns. The agent, in turn, gets the selection
 * from one of the clients within the process. [called from func
 * xevent_to_event in win/win_input.c] [data tansfer from the agent to X/SV
 * client] According to the ICCCM, if the property is None send the reply by
 * using the target atom as the property name. Here, we get a new property
 * name using get_property_atom().
 */
/* OWNER */
Xv_private void
selection_agent_selectionrequest(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Display        *dpy = (Display *) xv_get(server,
					     (Attr_attribute)XV_DISPLAY);
    Window	    win = agent->xid;
    Seln_rank       rank = selection_to_rank(req_event->selection, agent);


#ifdef X11_TIME
    /*
     * if the time selection was acquired is greater than the time on the
     * request event then deny request.  This means that request occured
     * before this process acquired ownership
     */
    if (req_event->time < agent->seln_acquired_time[ord(rank)]) {
	send_SelectionNotify(dpy, req_event->requestor,
			     req_event->selection, req_event->target,
			     None, req_event->time);
    }
#endif
    /* DEAL with different target atom types */
    if (req_event->target == get_atom(dpy, win, DO_FUNCTION(&agent->targets))) {
	selection_agent_do_function(server, req_event);
    } else if ((req_event->target == get_atom(dpy, win,
						   STRING(&agent->targets))) ||
	       (req_event->target == get_atom(dpy, win,
					  CONTENTS_PIECES(&agent->targets))) ||
	       (req_event->target == get_atom(dpy, win,
						FILE_NAME(&agent->targets))) ||
	       (req_event->target == get_atom(dpy, win,
					  CONTENTS_OBJECT(&agent->targets))) ||
	       (req_event->target == get_atom(dpy, win,
					  SELECTED_WINDOWS(&agent->targets)))) {
#ifdef OW_I18N
	selection_agent_do_text_content(server, req_event, FALSE);
#else
	selection_agent_do_ascii_content(server, req_event);
#endif
#ifdef OW_I18N
    } else if (req_event->target == get_atom(dpy, win,
					    COMPOUND_TEXT(&agent->targets))) {
	selection_agent_do_text_content(server, req_event, TRUE);
#endif
    } else if (req_event->target == get_atom(dpy, win,
						  TIMESTAMP(&agent->targets))) {
	selection_agent_do_timestamp(server, req_event, rank);
    } else if (req_event->target == get_atom (dpy, win,
						   MULTIPLE(&agent->targets))) {
	selection_agent_do_multiple(server, req_event);
    } else if (req_event->target == get_atom(dpy, win,
						    TARGETS(&agent->targets))) {
	selection_agent_do_target(server, req_event);
    } else
	selection_agent_do_request(server, req_event);
}


#ifdef OW_I18N
/* 
 * This function receives the selection data from the holder and converts 
 * it to compound text before writing the property on the server. If the 
 * selection data is being handed to this function in chunks, it accumulates 
 * the complete wide char text before the conversion to compound text.
 */
Xv_private      Seln_result
seln_convert_request_to_ct_property(buffer)
    Seln_request   *buffer;
{
    Xv_Server       server = (Xv_Server) buffer->requester.context;
    Seln_agent_info *agent =
        (Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
                                   SELN_AGENT_INFO);
    Seln_agent_context *context = &agent->req_context;
    Attr_avlist         reply;
    int                 length;
 
    if (context->event_sent)
	return(SELN_OVER);

    if (context->first_time) {
	if (context->buffer != NULL) {
	    free(context->buffer);
	}

        context->first_time = FALSE;
        reply = (Attr_avlist) buffer->data;
        if (reply[0] != (Attr_attribute) SELN_REQ_CHARSIZE) {
            return(SELN_FAILED);
	}

        /* skip past attr */
        reply++;

        /* remember the total number of bytes */
        context->chars_remaining = (int) *reply++;
	context->offset = 0;

	/* skip past attr */
 	reply++;

 	/* 
	 * If the selection is being handed back in chunks, start 
	 * accumlating the selection until the entire selection is 
	 * received.
	 */
	if (context->chars_remaining > STRLEN((CHAR *)reply)) {
	    context->offset = 0;
	    context->buffer = (CHAR *)
		calloc(context->chars_remaining + 1, sizeof(CHAR));
	    if (context->buffer == NULL) {
		xv_error(NULL,
		    ERROR_STRING, 
			XV_I18N_MSG("xv_messages", "selection: malloc() failed"),
		    0);
		return(SELN_FAILED);
	    }
	}
    } else {
        reply = (Attr_avlist) buffer->data;
        /*
         * If the owner of the selection didn't recognise the request abort
         * the transfer
         */
        if (reply[0] == (Attr_attribute) SELN_REQ_UNKNOWN) {
            reply[1] = (Attr_attribute) SELN_UNRECOGNIZED;
            send_SelectionNotify(context->display, context->requestor,
                                 context->selection, context->target,
                                 None, context->timestamp);
            context->event_sent = TRUE;
            return(SELN_OVER);
        }
    }

    length = STRLEN((CHAR *) reply);
    context->chars_remaining -= length; 

    if (context->chars_remaining == 0) {
#ifdef USE_COMPOUND_TEXT
	char	*sel_ct;
#endif USE_COMPOUND_TEXT

	if (context->buffer != NULL) {
	    bcopy((char *)reply, (char *)(context->buffer + context->offset),
	    		length * sizeof(CHAR));

#ifdef USE_COMPOUND_TEXT
	    /* Convert to compound text and store */
	    sel_ct = wcstoctsdup((CHAR *)context->buffer);
	    XChangeProperty(context->display, context->requestor,
		context->property, context->target, 16, PropModeReplace,
		(unsigned char *)sel_ct, strlen(sel_ct) + 1);
	    free(sel_ct);
#else USE_COMPOUND_TEXT
	    XChangeProperty(context->display, context->requestor, 
		context->property, context->target, 16, PropModeReplace,
		(unsigned char *)context->buffer, STRLEN(context->buffer)+1);
#endif USE_COMPOUND_TEXT

	    free(context->buffer);
	    context->buffer = NULL;
	    context->offset = 0;
	} else {
#ifdef USE_COMPOUND_TEXT
	    /* Convert to compound text and store */
	    sel_ct = wcstoctsdup((CHAR *)reply);
	    XChangeProperty(context->display, context->requestor,
		context->property, context->target, 16, PropModeReplace,
		(unsigned char *)sel_ct, strlen(sel_ct) + 1);
	    free(sel_ct);
#else USE_COMPOUND_TEXT
            XChangeProperty(context->display, context->requestor, 
		context->property, context->target, 16, PropModeReplace,
			(unsigned char *) reply, length+1);
#endif USE_COMPOUND_TEXT
	}
	send_SelectionNotify(context->display, context->requestor,
		context->selection, context->target,
		context->property, context->timestamp);
	context->event_sent = TRUE;
	return(SELN_SUCCESS);
    } else {
       /* start accumulating selection chunks */
       bcopy((char *)reply, (char *)(context->buffer + context->offset),
		length * sizeof(CHAR));
       context->offset += length;
       return(SELN_CONTINUED);
    }
}

#endif OW_I18N

/*
 * convert a request response to a property. This deals with small data
 * transfer only for now.
 */
Xv_private      Seln_result
seln_convert_request_to_property(buffer)
    Seln_request   *buffer;
{
    Xv_Server       server = (Xv_Server) buffer->requester.context;
    Seln_agent_info *agent =
        (Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
                                   SELN_AGENT_INFO);
    Seln_agent_context *context = &agent->req_context;
    Attr_avlist     	reply;
    int             	mode, len;


    /*
     * Returns SELN_OVER if the event was already sent This can happen only
     * if seln_convert_request_to_property is called after the entire
     * selection is written as property. This result is used by
     * seln_local_request to end request.
     */
    if (context->event_sent)
	return (SELN_OVER);

    if (context->first_time) {
	context->first_time = FALSE;
	reply = (Attr_avlist) buffer->data;
	if (reply[0] != (Attr_attribute) SELN_REQ_BYTESIZE) {
	    complain("Selection service: unrecognized request");
	    return SELN_FAILED;
	}
	/* skip past attr */
	reply++;
#ifdef OW_I18N
	/* remember the total number of bytes */
	context->chars_remaining = (int) *reply++;
#else
	/* remember the total number of bytes */
	context->bytes_remaining = (int) *reply++;
#endif
	if ((Seln_attribute) reply[0] == SELN_REQ_SELECTED_WINDOWS)
	    context->format = 32;
	else
	    context->format = 8;
	/* skip past the attr */
	reply++;
	mode = PropModeReplace;
    } else {
	reply = (Attr_avlist) buffer->data;
	/*
	 * If the owner of the selection didn't recognise the request abort
	 * the transfer
	 */
	if (reply[0] == (Attr_attribute) SELN_REQ_UNKNOWN) {
	    reply[1] = (Attr_attribute) SELN_UNRECOGNIZED;
	    send_SelectionNotify(context->display, context->requestor,
				 context->selection, context->target,
				 None, context->timestamp);
	    context->event_sent = TRUE;
	    return (SELN_OVER);
	}
	mode = PropModeAppend;
    }

    len = strlen((char *) reply);

    /*
     * now copy the reply data from the buffer to the property For the time
     * being type of the property is the same as the as the target.
     */
    XChangeProperty(context->display, context->requestor, context->property,
		    context->target, context->format, mode, 
		    (unsigned char *) reply, len);

#ifdef OW_I18N
    context->chars_remaining -= len;
#else
    context->bytes_remaining -= len;
#endif

    /* tell the requester if we are done */
#ifdef OW_I18N
    if (context->chars_remaining == 0) {
#else
    if (context->bytes_remaining == 0) {
#endif
	send_SelectionNotify(context->display, context->requestor,
			     context->selection, context->target,
			     context->property, context->timestamp);
	context->event_sent = TRUE;
    }
    return SELN_SUCCESS;
}

/*
 * someone is asking when we got the selection.  Put this into the property
 * data field and send a selectionnotify telling the enquirer that the value
 * is available in the property.
 */
/* OWNER */

static void
selection_agent_do_timestamp(server, req_event, rank)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
    Seln_rank       rank;
{
    Seln_agent_info    *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA, 
				   SELN_AGENT_INFO);
    Display            *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Atom            	return_type;
    int             	format;
    unsigned long   	nitems_return, bytes_remaining;
    int             	length = 100;
    Time             	time;
    unsigned char      *data;

    if (req_event->property == None)
	goto Done;

    if (XGetWindowProperty(display, req_event->requestor, req_event->property,
			   0L, length, TRUE, AnyPropertyType, &return_type,
			   &format, &nitems_return, &bytes_remaining,
			   &data) == Success) {
	if (return_type == None)
	    goto Done;
    }
    /* ICCCM : do a getselectionowner here? */
    /* time this selection was acquired */
    time = agent->seln_acquired_time[ord(rank)];
    XChangeProperty(display, req_event->requestor,
		    req_event->property, req_event->target,
		    32, PropModeReplace, (unsigned char *) &time, 1);

Done:
    send_SelectionNotify(display, req_event->requestor, req_event->selection,
		   req_event->target, req_event->property, req_event->time);
}

/* OWNER */
static void
selection_agent_do_multiple(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Atom            return_type;
    int             format;
    unsigned long   nitems_return, bytes_remaining;
    int             length = 100;
    unsigned char  *data;

    if (req_event->property == None)
	goto Done;

    if (XGetWindowProperty(display, req_event->requestor, req_event->property,
			   0L, length, TRUE, AnyPropertyType, &return_type,
			   &format, &nitems_return, &bytes_remaining,
			   &data) == Success) {
	if (return_type == None)
	    goto Done;
    }
Done:
    (void) fprintf(stderr, 
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "MULTIPLE target atom not implemented\n")
#else
		"MULTIPLE target atom not implemented\n"
#endif
		);
    return;
}


/* OWNER */
static void
selection_agent_do_target(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Atom            property;
    Atom            targets[NUM_OF_TARGETS];


    if (req_event->property == None)
	property = get_property_atom(display, agent);
    else
	property = req_event->property;

    bcopy((char *) &agent->targets, (char *) targets,
	  sizeof(Atom) * NUM_OF_TARGETS);

    /* Write the result on a property */
    /*
     * Questionable: NUM_OF_TARGETS is not the length if sizeof(Atom) is not
     * 32
     */
    XChangeProperty(display, req_event->requestor,
		    property, req_event->target,
		    32, PropModeReplace, (unsigned char *) targets,
		    NUM_OF_TARGETS);
    send_SelectionNotify(display, req_event->requestor, req_event->selection,
			 req_event->target, property, req_event->time);

}

/*
 * Call function call back of the client that holds the selection
 */
/* OWNER */
static void
selection_agent_do_function(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Seln_function_buffer buffer;
    Seln_holder     recipient_holder;
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    int             length = sizeof(Seln_function) + sizeof(Seln_rank) + 3;
    Atom            return_type;
    int             format;
    unsigned long   nitems_return, bytes_remaining;
    unsigned char  *data;


    /*
     * Execute function proc of the client that is the holder of this rank.
     * Get the information about rank and function key from property.
     */
    if (XGetWindowProperty(display, req_event->requestor, req_event->property,
			   0L, length / sizeof(int), TRUE, AnyPropertyType,
		    &return_type, &format, &nitems_return, &bytes_remaining,
			   &data) == Success) {
	if (return_type == None) {
	    complain("Error while trying GetProperty");
	    return;
	} else {
	    bcopy((char *) data, 
		  (char *) &buffer.function,
		  sizeof(Seln_function));
	    bcopy((char *) (data + sizeof(Seln_function)),
		  (char *) &buffer.addressee_rank,
		  sizeof(Seln_rank));
	}
    }
    recipient_holder = selection_inquire(server, buffer.addressee_rank);

    /*
     * Fill buffer holder info.
     */
    buffer.caret = selection_inquire(server, SELN_CARET);
    buffer.primary = selection_inquire(server, SELN_PRIMARY);
    buffer.secondary = selection_inquire(server, SELN_SECONDARY);
    buffer.shelf = selection_inquire(server, SELN_SHELF);
    /*
     * call the client's function call back routine
     */
    if (!selection_equal_agent(server, recipient_holder)) {
	Seln_client_node *client = (Seln_client_node *)
	recipient_holder.access.client;
	client->ops.do_function(client->client_data, &buffer);
    } else {
	complain("This process does not hold the selection");
    }
}


Pkg_private void
seln_give_up_selection(server, rank)
    Xv_Server       server;
    Seln_rank       rank;
{
    Display        *display =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Atom            selection = seln_rank_to_selection(rank, agent);

    if (selection != None) {
	if ((XID) XGetSelectionOwner(display, selection) == agent->xid)
	    /*
	     * the time field is set to agent->seln_acquired_time[(int) rank]
	     * - the time current owner got ownership of the selection
	     */
	    XSetSelectionOwner(display, selection, None,
			       agent->seln_acquired_time[(int) rank]);
	selection_init_holder_info(server, rank);
    }
}

/* OWNER */
static void
selection_agent_do_request(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA, 
				   SELN_AGENT_INFO);
    Display        *dpy =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_attribute  attr= convert_target_to_attr(dpy, agent, req_event->target);
    Seln_rank       rank;
    Seln_holder     holder;
    Atom            property;
    Seln_request   *request;
    Attr_avlist     reply;

    if (req_event->property == None)
	property = get_property_atom(dpy, agent);
    else
	property = req_event->property;

    rank = selection_to_rank(req_event->selection, agent);
    holder = selection_inquire(server, rank);
    switch (attr) {

      case SELN_REQ_YIELD:{
	    request = selection_ask(server, &holder, SELN_REQ_YIELD, rank, 0);
	    if (request->status == SELN_FAILED) {
		property = None;
	    } else {
		seln_give_up_selection(server,
			    selection_to_rank(req_event->selection, agent));
	    }
	    break;
	}

      case SELN_REQ_COMMIT_PENDING_DELETE:
      case SELN_REQ_DELETE:
      case SELN_REQ_RESTORE:
	/* No need to return value for these */
	(void) selection_ask(server, &holder, attr, 0, 0);
	return;

      case SELN_REQ_UNKNOWN:
	/* Let the X Clients know that target is not supported */
	property = None;
	goto Done;

      default:
	request = selection_ask(server, &holder, attr, 0, 0);
	break;
    }
    if (request->status != SELN_SUCCESS) {
	property = None;
	goto Done;
    }
    reply = (Xv_opaque *) request->data;
    if (reply[0] == (Xv_opaque) attr) {
    } else if (reply[0] == (Xv_opaque) SELN_REQ_UNKNOWN) {
	reply[1] = (Xv_opaque) SELN_UNRECOGNIZED;
	/* Property None indicates target not recognized */
	property = None;
	goto Done;
    } else
	complain("Garbage returned");
    reply++;

    /* Write the result on a property */
    XChangeProperty(dpy, req_event->requestor,
		    property, req_event->target,
		    32, PropModeReplace, (unsigned char *) reply, 1);

Done:
    send_SelectionNotify(dpy, req_event->requestor, req_event->selection,
			 req_event->target, property, req_event->time);
}

static void
send_SelectionNotify(display, requestor, selection, target, property, time)
    Display        *display;
    Window          requestor;
    Atom            selection, target, property;
    Time            time;
{
    XSelectionEvent xevent;

    xevent.property = property;
    xevent.type = SelectionNotify;
    xevent.display = display;
    xevent.requestor = requestor;
    xevent.selection = selection;
    xevent.target = target;
    xevent.time = time;
    (void) XSendEvent(display, requestor, 0, 0, &xevent);
    XFlush(display);
}

#ifdef OW_I18N
static void
selection_agent_do_text_content(server, req_event, compound_text)
    Xv_Server       	    	server;
    XSelectionRequestEvent  	*req_event;
    int				compound_text;		
{
    Seln_agent_info 	*agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Display        	*dpy =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_rank       	rank;
    Seln_holder     	holder;
    Seln_attribute  	attr =
	convert_target_to_attr(dpy, agent, req_event->target);

    rank = selection_to_rank(req_event->selection, agent);
    holder = selection_inquire(server, rank);

    agent->req_context.first_time = TRUE;
    agent->req_context.requestor = req_event->requestor;
    if (req_event->property != None)
	agent->req_context.property = req_event->property;
    else
	agent->req_context.property = get_property_atom(dpy, agent);
    agent->req_context.selection = req_event->selection;
    agent->req_context.target = req_event->target;
    agent->req_context.display = dpy;
    agent->req_context.timestamp = req_event->time;
    agent->req_context.event_sent = FALSE;

    if (compound_text == TRUE) {
	(void)selection_query(server, &holder,
		 seln_convert_request_to_ct_property, server,
		 SELN_REQ_CHARSIZE, 0, SELN_REQ_CONTENTS_WCS, 0, 0);
    } else {
	(void)selection_query(server, &holder,
		 seln_convert_request_to_property, server,
		 SELN_REQ_BYTESIZE, 0, attr, 0, 0);
    }
}

#else
/* OWNER */
static void
selection_agent_do_ascii_content(server, req_event)
    Xv_Server       server;
    XSelectionRequestEvent *req_event;
{
    Seln_agent_info *agent =
	(Seln_agent_info *) xv_get(server, (Attr_attribute)XV_KEY_DATA,
				   SELN_AGENT_INFO);
    Display        *dpy =
	(Display *) xv_get(server, (Attr_attribute)XV_DISPLAY);
    Seln_rank       rank;
    Seln_holder     holder;
    Seln_attribute  attr =convert_target_to_attr(dpy, agent, req_event->target);

    rank = selection_to_rank(req_event->selection, agent);
    holder = selection_inquire(server, rank);

    agent->req_context.first_time = TRUE;
    agent->req_context.requestor = req_event->requestor;
    if (req_event->property != None)
	agent->req_context.property = req_event->property;
    else
	agent->req_context.property = get_property_atom(dpy, agent);
    agent->req_context.selection = req_event->selection;
    agent->req_context.target = req_event->target;
    agent->req_context.display = dpy;
    agent->req_context.timestamp = req_event->time;
    agent->req_context.event_sent = FALSE;

    (void) selection_query(server, &holder,
			   seln_convert_request_to_property, (char *) server,
			   SELN_REQ_BYTESIZE, 0, attr, 0, 0);
}
#endif


static          XSelectionEvent
*
ask_selection_owner(display, agent, selection, target, property, time, do_block)
    Display        *display;
    Seln_agent_info *agent;
    Atom            selection;
    Atom            target;
    Atom            property;
    Time            time;
    int             do_block;
{
    XEvent          xevent;

    if (property == None) {	/* not allowed */
	complain("cannot specify propertyfield to be None in XConvertSelection");
    }
    XConvertSelection(display, selection, target, property, agent->xid, time);
    /* Block until SelectionNotify is seen */
    if (do_block) {
	block(display, &xevent, agent->timeout);
    }
    return ((XSelectionEvent *) & xevent);
}


/*
 * Predicate function for XCheckIfEvent
 * 
 */

/*ARGSUSED*/
static int
is_blocking_over(display, xevent, args)
    Display        *display;
    XEvent         *xevent;
    char	   *args;
{
    if ((xevent->type & 0177) == SelectionNotify) {
	return (TRUE);
    }
    return (FALSE);
}


/*
 * block returns only under two circumstances: 1) SelectionNotify is seen. 2)
 * Timeout (The owner of the selection is not responding)
 */
static void
block(display, xevent, seconds)
    Display        *display;
    XEvent         *xevent;
    int             seconds;
{
    struct timeval  timeout;
    int             blocking = TRUE;

    timeout.tv_sec = seconds;
    timeout.tv_usec = 2 * SELN_STD_TIMEOUT_USEC;
    XFlush(display);
    while (blocking) {
	timeout.tv_sec -= 1;
	if (waitforReadableTimeout(display, &timeout) == 0) {
	    ((XSelectionEvent *) xevent)->property = None;
	    if (seln_debug)
	        complain("Selection Timed out");
	    blocking = FALSE;
	} else {
	    blocking = !XCheckIfEvent(display, xevent, is_blocking_over, 
				      (char *) NULL);
	}
    }
}

/* performance: global cache of getdtablesize() */
extern int      dtablesize_cache;
#define GETDTABLESIZE() \
        (dtablesize_cache?dtablesize_cache:(dtablesize_cache=getdtablesize()))


/*
 * Wait for either input or timeout
 */
static int
waitforReadableTimeout(display, timeout)
    Display        *display;
    struct timeval *timeout;
{
    fd_set          select_ibits;
    int             result;
    int             max_fds = GETDTABLESIZE();

    FD_ZERO(&select_ibits);
    do {
	FD_SET(display->fd, &select_ibits);
#ifndef SVR4
	result = select(max_fds, &select_ibits,
			(fd_set *) NULL, (fd_set *) NULL, timeout);
#else SVR4
	result = select(display->fd + 1, &select_ibits, NULL, NULL, timeout);
#endif SVR4
	if (result == -1 && errno != EINTR)
	{
	    complain("Select call returned error");
	    perror(
#ifdef OW_I18N
		XV_I18N_MSG("xv_messages", "Select")
#else
		"Select"
#endif
		);
	}
    } while (result < 0);
    return result;
}

static          Seln_attribute
convert_target_to_attr(dpy, agent, target)
    Display 	    *dpy;
    Seln_agent_info *agent;
    Atom            target;
{
    Seln_attribute attr = SELN_REQ_UNKNOWN;
    Window	   	   win = agent->xid;

			       /* If we already have the attr associated with */
			       /* this atom hashed away somewhere, return it. */
    if (XFindContext(dpy, win, (XContext) target, &attr) != XCNOENT)
	return attr;
			       /* Else go the opposite direction. Since we    */
			       /* have the atom, get the name assoc. with it. */
			       /* Save that info away so we don't have to     */
			       /* XInternAtom it later.                       */
    else {
	char *atomName = XGetAtomName(dpy, target);

	if (!strcmp(atomName, "STRING"))
	    attr = save_atom(target, dpy, win, STRING(&agent->targets));
#ifdef OW_I18N
	else if (!strcmp(atomName, "COMPOUND_TEXT"))
            attr = save_atom(target, dpy, win, COMPOUND_TEXT(&agent->targets));
	else if (!strcmp(atomName, "LENGTH_CHARS"))
	    attr = save_atom(target, dpy, win, LENGTH_CHARS(&agent->targets));
#endif
	else if (!strcmp(atomName, "LENGTH"))
	    attr = save_atom(target, dpy, win, LENGTH(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_OBJECT_SIZE"))
	    attr = save_atom(target, dpy, win, OBJECT_SIZE(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_CONTENTS_PIECES"))
	    attr = save_atom(target, dpy, win, 
			     CONTENTS_PIECES(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_FIRST"))
	    attr = save_atom(target, dpy, win, FIRST(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_FIRST_UNIT"))
	    attr = save_atom(target, dpy, win, FIRST_UNIT(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_LAST"))
	    attr = save_atom(target, dpy, win, LAST(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_LAST_UNIT"))
	    attr = save_atom(target, dpy, win, LAST_UNIT(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_LEVEL"))
	    attr = save_atom(target, dpy, win, LEVEL(&agent->targets));
	else if (!strcmp(atomName, "FILE_NAME"))
	    attr = save_atom(target, dpy, win, FILE_NAME(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_COMMIT_PENDING_DELETE"))
	    attr = save_atom(target, dpy, win,
					COMMIT_PENDING_DELETE(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_DELETE"))
	    attr = save_atom(target, dpy, win, DELETE(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_RESTORE"))
	    attr = save_atom(target, dpy, win, RESTORE(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_YIELD"))
	    attr = save_atom(target, dpy, win, YIELD(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_FAKE_LEVEL"))
	    attr = save_atom(target, dpy, win, FAKE_LEVEL(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_SET_LEVEL"))
	    attr = save_atom(target, dpy, win, SET_LEVEL(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_END_REQUEST"))
	    attr = save_atom(target, dpy, win, END_REQUEST(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_IS_READONLY"))
	    attr = save_atom(target, dpy, win, IS_READONLY(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_FUNC_KEY_STATE"))
	    attr = save_atom(target, dpy, win, FUNC_KEY_STATE(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_SELECTED_WINDOWS"))
	    attr = save_atom(target, dpy,win,SELECTED_WINDOWS(&agent->targets));
	else if (!strcmp(atomName, "_SUN_SELN_CONTENTS_OBJECT"))
	    attr = save_atom(target, dpy, win, CONTENTS_OBJECT(&agent->targets));

	(void) XFree(atomName);
    	return attr;
    }
}

/*ARGSUSED*/
static Seln_attribute
save_atom(target, dpy, win, atom_name, attr, atom)
Atom		target;
Display	       *dpy;
Window		win;
char	       *atom_name;
Seln_attribute	attr;
Atom           *atom;
{
	*atom = target;
	(void) XSaveContext(dpy, win, (XContext) target, (caddr_t) attr);
	return attr;
}

static          Atom
convert_attr_to_target(dpy, agent, attr)
    Display         *dpy;
    Seln_agent_info *agent;
    Seln_attribute  attr;
{
    if (attr == SELN_REQ_BYTESIZE)
	return get_atom(dpy, agent->xid, LENGTH(&agent->targets));
    if (attr == SELN_REQ_OBJECT_SIZE)
	return get_atom(dpy, agent->xid, OBJECT_SIZE(&agent->targets));
    if (attr == SELN_REQ_CONTENTS_ASCII)
	return get_atom(dpy, agent->xid, STRING(&agent->targets));
#ifdef OW_I18N
    if (attr == SELN_REQ_CONTENTS_CT)
        return get_atom(dpy, agent->xid, COMPOUND_TEXT(&agent->targets));
    if (attr == SELN_REQ_CHARSIZE)
	return get_atom(dpy, agent->xid, LENGTH_CHARS(&agent->targets));
#endif
    if (attr == SELN_REQ_CONTENTS_PIECES)
	return get_atom(dpy, agent->xid, CONTENTS_PIECES(&agent->targets));
    if (attr == SELN_REQ_FIRST)
	return get_atom(dpy, agent->xid, FIRST(&agent->targets));
    if (attr == SELN_REQ_FIRST_UNIT)
	return get_atom(dpy, agent->xid, FIRST_UNIT(&agent->targets));
    if (attr == SELN_REQ_LAST)
	return get_atom(dpy, agent->xid, LAST(&agent->targets));
    if (attr == SELN_REQ_LAST_UNIT)
	return get_atom(dpy, agent->xid, LAST_UNIT(&agent->targets));
    if (attr == SELN_REQ_LEVEL)
	return get_atom(dpy, agent->xid, LEVEL(&agent->targets));
    if (attr == SELN_REQ_FILE_NAME)
	return get_atom(dpy, agent->xid, FILE_NAME(&agent->targets));
    if (attr == SELN_REQ_COMMIT_PENDING_DELETE)
	return get_atom(dpy, agent->xid, COMMIT_PENDING_DELETE(&agent->targets));
    if (attr == SELN_REQ_DELETE)
	return get_atom(dpy, agent->xid, DELETE(&agent->targets));
    if (attr == SELN_REQ_RESTORE)
	return get_atom(dpy, agent->xid, RESTORE(&agent->targets));
    if (attr == SELN_REQ_YIELD)
	return get_atom(dpy, agent->xid, YIELD(&agent->targets));
    if (attr == SELN_REQ_FAKE_LEVEL)
	return get_atom(dpy, agent->xid, FAKE_LEVEL(&agent->targets));
    if (attr == SELN_REQ_SET_LEVEL)
	return get_atom(dpy, agent->xid, SET_LEVEL(&agent->targets));
    if (attr == SELN_REQ_END_REQUEST)
	return get_atom(dpy, agent->xid, END_REQUEST(&agent->targets));
    if (attr == SELN_REQ_IS_READONLY)
	return get_atom(dpy, agent->xid, IS_READONLY(&agent->targets));
    if (attr == SELN_REQ_FUNC_KEY_STATE)
	return get_atom(dpy, agent->xid, FUNC_KEY_STATE(&agent->targets));
    if (attr == SELN_REQ_SELECTED_WINDOWS)
	return get_atom(dpy, agent->xid, SELECTED_WINDOWS(&agent->targets));
    if (attr == SELN_REQ_CONTENTS_OBJECT)
	return get_atom(dpy, agent->xid, CONTENTS_OBJECT(&agent->targets));
    return (None);
}

/*
 * Creates property atoms on the fly (upto a maximum of SELN_PROPERTY)
 */
static          Atom
get_property_atom(display, agent)
    Display        *display;
    Seln_agent_info *agent;
{
    static int      count;
    static int      donot_create;
    Atom            property;

    if (!donot_create) {
	char            tmp[80];
	char            temp1[5];
	(void) strcpy(tmp, "SELECTION_PROPERTY");
	(void) sprintf(temp1, "%d", count);
	(void) strcat(tmp, temp1);
	if ((agent->property[count] = XInternAtom(display, tmp, FALSE))
	    == None) {
	    complain("Couldn't find property atom");
	    exit(1);
	}
    }
    property = agent->property[count];
    if (++count >= SELN_PROPERTY) {
	donot_create = True;
	count = 0;
    }
    return (property);
}

static Atom
get_atom(dpy, win, prop_name, attr, current_value)
Display        *dpy;
Window 		win;
char           *prop_name;
Seln_attribute	attr;
register Atom  *current_value;
{
        			    /* If we already have the atom, return it.*/
        if (*current_value != None)
                return (*current_value);

				    /* else, lets get it's value.             */
        if ((*current_value = XInternAtom(dpy, prop_name, 0)) == None) 
                xv_error(NULL,
                        ERROR_STRING,
#ifdef OW_I18N
		    XV_I18N_MSG("xv_messages", "Couldn't get selection atom, selections may not work. (Selection package)"),
#else
		    "Couldn't get selection atom, selections may not work. (Selection package)",
#endif
			0);
				    /* Assoc. the atom value with an attr.    */
				    /* This is used in convert_target_to_attr */
				    /* to convert an atom back to an attr.    */
	(void) XSaveContext(dpy, win, (XContext) *current_value,
			    (caddr_t) attr);
        
        return *current_value;
}
