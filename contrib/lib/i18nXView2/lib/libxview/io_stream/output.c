#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)output.c 50.4 90/11/04";
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

#ifdef	OW_I18N
#define Get_Output_Ops \
	struct output_ops_vector *ops;\
	if (out->stream_type != Output) \
	    xv_error(NULL, \
		ERROR_SEVERITY, ERROR_NON_RECOVERABLE, \
		ERROR_STRING, XV_I18N_MSG("xv_messages","output stream not of type output"), 0); \
	ops = out->ops.output_ops
#else
#define Get_Output_Ops \
	struct output_ops_vector *ops;\
	if (out->stream_type != Output) \
	    xv_error(NULL, \
		ERROR_SEVERITY, ERROR_NON_RECOVERABLE, \
		ERROR_STRING, "output stream not of type output", 0); \
	ops = out->ops.output_ops
#endif


static void stream_putstring();

/* GENERIC OUTPUT FUNCTIONS */

int
stream_putc(c, out)
    char            c;
    STREAM         *out;
{
    Get_Output_Ops;
    return ((*ops->str_putc) (c, out));
}

void
stream_puts(s, out)
    char           *s;
    STREAM         *out;
{
    stream_putstring(s, out, True);
}

void
stream_fputs(s, out)
    char           *s;
    STREAM         *out;
{
    stream_putstring(s, out, False);
}

static void
stream_putstring(s, out, include_newline)
    char           *s;
    STREAM         *out;
    Bool            include_newline;
{
    int             i;

    Get_Output_Ops;
    if (ops->str_fputs != NULL)
	(*ops->str_fputs) (s, out);
    else
	for (i = 0; s[i] != '\0'; i++)
	    (*ops->str_putc) (s[i], out);

    if (include_newline)
	(*ops->str_putc) ('\n', out);

}


void
stream_flush(out)
    STREAM         *out;
{
    void            (*fn) ();

    Get_Output_Ops;
    fn = ops->flush;
    if (fn != NULL)
	(*fn) (out);
}
