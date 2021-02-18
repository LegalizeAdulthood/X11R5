/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *      file for terms of the license.
 */

#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/seln.h>

#define DD_FAILED  -1

int
xv_decode_drop(ev, buffer, bsize)
    Event 	*ev;
    char	*buffer;
    unsigned int bsize;
{
    unsigned long	 nitems, bytes_after; 
    XClientMessageEvent *cM;
    Seln_holder	 	 seln_holder;
    Atom	         actual_type;
    int	                 actual_format,
			 data_length,
			 return_value,
			 NoDelete = False;
    char		*data;


    if ((event_action(ev) != ACTION_DRAG_COPY) &&
	(event_action(ev) != ACTION_DRAG_MOVE) &&
	(event_action(ev) != ACTION_DRAG_LOAD))
	return (DD_FAILED);
				       	/* Dig out the ClientMessage event. */
    cM = (XClientMessageEvent *)event_xevent(ev);

				        /* Find out who owns the drag. */
    seln_holder = seln_inquire(SELN_PRIMARY);
    if (seln_holder.state == SELN_NONE)
        return(DD_FAILED);
    			             	/* Use client set property. */
    if ((cM->data.l[4]) && (XGetWindowProperty(cM->display, cM->data.l[3],
					cM->data.l[4], 0L,(long)((bsize + 3)/4),
					True, AnyPropertyType, &actual_type,
					&actual_format, &nitems, &bytes_after,
			                (unsigned char **)&data) == Success)) {
	    data_length = strlen(data);
	    return_value = data_length + bytes_after;
	    if (data_length >= bsize) {
		data_length = bsize-1;
		NoDelete = True;
	    }
	    bcopy(data, buffer, data_length);
	    buffer[data_length] = '\0';
	    XFree(data);
    } else {
	Seln_request	*seln_buffer;
					/* Ask for the ascii contents of the */
					/* selection.			     */
	seln_buffer = seln_ask(&seln_holder,
				     SELN_REQ_CONTENTS_ASCII, 0,
				     0);

	if (seln_buffer->status == SELN_FAILED)
	    return(DD_FAILED);

	if (*((Seln_attribute *) seln_buffer->data) != SELN_REQ_CONTENTS_ASCII)
	    return(DD_FAILED);

	data = (char *) seln_buffer->data;
	data += sizeof(SELN_REQ_CONTENTS_ASCII);

	return_value = data_length = strlen(data);
	if (data_length >= bsize) {
		data_length = bsize-1;
		NoDelete = True;
	}
	bcopy(data, buffer, data_length);
	buffer[data_length] = '\0';
    }
					/* If this is a move, then ask */
					/* the owner to delete the selection. */
    if ((event_action(ev) == ACTION_DRAG_MOVE) && (!NoDelete))
	seln_ask(&seln_holder, SELN_REQ_DELETE, 0, 0);

				        /* Insure the buffer ends with a NULL.*/
    return(return_value);
}
