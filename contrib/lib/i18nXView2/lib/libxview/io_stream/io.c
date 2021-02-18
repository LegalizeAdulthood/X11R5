#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)io.c 50.4 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/xv_error.h>
#include <xview_private/io_stream.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

/* GENERIC FUNCTIONS THAT APPLY TO BOTH INPUT AND OUTPUT */

void
stream_close(stream)
    STREAM         *stream;
{
    switch (stream->stream_type) {
      case Input:{
	    struct input_ops_vector *ops = stream->ops.input_ops;
	    (*(ops->close)) (stream);
	    goto out;
	}

      case Output:{
	    struct output_ops_vector *ops = stream->ops.output_ops;
	    (*(ops->close)) (stream);
	    goto out;
	}

      default:
	xv_error(stream,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
#ifdef	OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages" , "invalid stream type"),
#else
		 ERROR_STRING, "invalid stream type",
#endif
		 0);
    }
out:free((char *) stream);	/* client should have freed the client data */
}

struct posrec
stream_get_pos(stream)
    STREAM         *stream;
{
    switch (stream->stream_type) {
      case Input:{
	    struct input_ops_vector *ops = stream->ops.input_ops;
	    return ((*ops->get_pos) (stream));
	}
      case Output:{
	    struct output_ops_vector *ops = stream->ops.output_ops;
	    return ((*ops->get_pos) (stream));
	}
      default:{
	  struct posrec null_posrec;
	  null_posrec.lineno = -1;
	  null_posrec.charpos = -1;
	  xv_error((Xv_object)stream,
		   ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
#ifdef	OW_I18N
		   ERROR_STRING, XV_I18N_MSG("xv_messages" , "invalid stream type"), NULL);
#else
		   ERROR_STRING, "invalid stream type", NULL);
#endif
	  return (null_posrec);
        }
    }
}
