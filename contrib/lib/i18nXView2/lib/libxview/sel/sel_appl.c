#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sel_appl.c 50.6 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/sel_impl.h>
#include <xview/attr.h>
#include <xview/rect.h>
#include <xview/server.h>
#include <xview/sel_compat.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

static void     seln_init_request_buffer();
extern char    *malloc();

/*
 * Generic request to another holder
 */
/*VARARGS1*/
Xv_public Seln_request *
seln_ask(holder, va_alist)
    Seln_holder    *holder;
va_dcl
{
    Xv_opaque       avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    va_list         args;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);
    return (selection_ask(xv_default_server, holder, ATTR_LIST, avlist, 0));
}

/*VARARGS2*/ /*ARGSUSED*/
Xv_public Seln_request *
selection_ask(server, holder, va_alist)
    Xv_Server       server;
    Seln_holder    *holder;
va_dcl
{
    static Seln_request *buffer;
    va_list         valist;

    if (buffer == (Seln_request *) NULL) {
	buffer = (Seln_request *)
	    malloc((unsigned) (sizeof(Seln_request)));
	if (buffer == (Seln_request *) NULL) {
	    (void) fprintf(stderr,
#ifdef OW_I18N
		       XV_I18N_MSG("xv_messages", "Couldn't malloc request buffer (no swap space?)\n")
#else
		       "Couldn't malloc request buffer (no swap space?)\n"
#endif
		       );
	    return &seln_null_request;
	}
    }
    if (holder->state == SELN_NONE) {
	return &seln_null_request;
    }
    va_start(valist);
    if (attr_make((Attr_avlist) buffer->data,
		  sizeof(buffer->data) / sizeof(char *),
		  valist) == (Attr_avlist) NULL) {
	complain("Selection request too big -- not sent");
	va_end(valist);
	return &seln_null_request;
    }
    va_end(valist);
    seln_init_request_buffer(buffer, holder);
    if (selection_request(server, holder, buffer) == SELN_SUCCESS) {
	return buffer;
    } else {
	return &seln_null_request;
    }
}


/*VARARGS2*/
Xv_public void
seln_init_request(buffer, holder, va_alist)
    Seln_request   *buffer;
    Seln_holder    *holder;
va_dcl
{
    Xv_opaque       avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    va_list         args;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);
    selection_init_request(xv_default_server, buffer, holder, 
			   ATTR_LIST, avlist, 0);
}

/*VARARGS2*/ /*ARGSUSED*/
Xv_public void
selection_init_request(server, buffer, holder, va_alist)
    Xv_Server	    server;
    Seln_request   *buffer;
    Seln_holder    *holder;
va_dcl
{

    va_list         valist;

    va_start(valist);
    if (attr_make((Attr_avlist) buffer->data,
		  sizeof(buffer->data) / sizeof(char *),
		  valist) == (Attr_avlist) NULL) {
	complain("Selection request too big -- not sent");
	va_end(valist);
	return;
    }
    va_end(valist);
    seln_init_request_buffer(buffer, holder);
}

/*VARARGS3*/
Xv_public       Seln_result
seln_query(holder, reader, context, va_alist)
    Seln_holder    *holder;
Seln_result(*reader) ();
    char           *context;
va_dcl
{
    Xv_opaque       avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    va_list         args;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);
    return (selection_query(xv_default_server, holder, reader, context,
			    ATTR_LIST, avlist, 0));
}



/*VARARGS4*/ /*ARGSUSED*/
Xv_public       Seln_result
selection_query(server, holder, reader, context, va_alist)
    Xv_Server       server;
    Seln_holder    *holder;
    Seln_result	  (*reader) ();
    char           *context;
va_dcl
{
    static Seln_request *buffer;
    va_list         valist;

    if (buffer == (Seln_request *) NULL) {
	buffer = (Seln_request *)
	    malloc((unsigned) (sizeof(Seln_request)));
	if (buffer == (Seln_request *) NULL) {
	    (void) fprintf(stderr,
#ifdef OW_I18N
		       XV_I18N_MSG("xv_messages", "Couldn't malloc request buffer (no swap space?)\n")
#else
		       "Couldn't malloc request buffer (no swap space?)\n"
#endif
		       );
	    return SELN_FAILED;
	}
    }
    if (holder->state == SELN_NONE) {
	return SELN_FAILED;
    }
    va_start(valist);
    if (attr_make((Attr_avlist) buffer->data,
		  sizeof(buffer->data) / sizeof(char *),
		  valist) == (Attr_avlist) NULL) {
	complain("Selection request too big -- not sent");
	va_end(valist);
	return SELN_FAILED;
    }
    va_end(valist)
	seln_init_request_buffer(buffer, holder);
    buffer->requester.consume = reader;
    buffer->requester.context = context;
    return selection_request(server, holder, buffer);
}

static void
seln_init_request_buffer(buffer, holder)
    Seln_request   *buffer;
    Seln_holder    *holder;
{
    buffer->buf_size = attr_count((Attr_avlist) buffer->data) *
	sizeof(char *);
    buffer->rank = holder->rank;
    buffer->addressee = holder->access.client;
    buffer->replier = 0;
    buffer->requester.consume = 0;
    buffer->requester.context = 0;
}
