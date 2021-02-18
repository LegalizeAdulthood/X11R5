/* $Header: /usr/src/local/emu/misc/RCS/msgs.c,v 1.7 91/07/26 17:06:43 jkh Exp $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Error and debugging message routines.
 *
 * Author: Jordan K. Hubbard
 * Date: April 24th, 1990.
 * Description: Miscellaneous error and debugging message routines.
 *		I don't use varargs() since I end up passing the results
 *		to the toolkit routines, and vsprintf() is unfortunately
 *		not available a lot of the time.  Sometimes I hate Unix.
 *
 * Revision History:
 *
 * $Log:	msgs.c,v $
 * Revision 1.7  91/07/26  17:06:43  jkh
 * Allowed debug routine to be used without DEBUG.  Changed declarations for TAGS compatability.
 * 
 * Revision 1.6  90/11/20  17:34:19  jkh
 * Alpha.
 * 
 * Revision 1.5  90/10/19  00:23:35  jkh
 * Messages more clear now.
 * 
 * Revision 1.4  90/07/26  02:44:52  jkh
 * Added new copyright.
 * 
 * Revision 1.1  90/04/27  12:09:06  me
 * Initial revision
 * 
 *
 */

#include "common.h"

/* print an error and toss our cookies */
Export void
fatal(fmt, p1, p2, p3, p4, p5, p6)
String fmt;
Generic p1, p2, p3, p4, p5, p6;
{
     Import void XtError();
     char errmsg[1024];

     sprintf(errmsg, "Emu: ");
     sprintf(&errmsg[strlen(errmsg)], fmt, p1, p2, p3, p4, p5, p6);
     strcat(errmsg, "\n");
     XtError(errmsg);
}

/* print a warning message */
Export void
warn(fmt, p1, p2, p3, p4, p5, p6)
String fmt;
Generic p1, p2, p3, p4, p5, p6;
{
     Import void XtWarning();
     char warning[1024];

     sprintf(warning, "Emu: ");
     sprintf(&warning[strlen(warning)], fmt, p1, p2, p3, p4, p5, p6);
     strcat(warning, "\n");
     XtWarning(warning);
}

/* print a general information message */
Export void
pmessage(fmt, p1, p2, p3, p4, p5, p6)
String fmt;
Generic p1, p2, p3, p4, p5, p6;
{
     fprintf(stderr, "Emu: ");
     fprintf(stderr, fmt, p1, p2, p3, p4, p5, p6);
     fputc('\n', stderr);
     fflush(stderr);
}

Export void
debug(fmt, p1, p2, p3, p4, p5, p6)
String fmt;
Generic p1, p2, p3, p4, p5, p6;
{
     FILE *dout;

     /* Open and close each time so that en-masse fd closes won't affect us */
     if ((dout = fopen(DEBUGFILE, "a+")) == NULL)
	  fatal("Can't open debug file %s!", DEBUGFILE);
     fprintf(dout, fmt, p1, p2, p3, p4, p5, p6);
     fputc('\n', dout);
     fclose(dout);
}

/* Return the type name for a type id */
Export String
reg_type_name(n)
int n;
{
     switch (n) {
     case CB_INT_TYPE:
	  return "Int";

     case CB_STR_TYPE:
	  return "String";

     case CB_CHAR_TYPE:
	  return "Char";

     case CB_ANY_TYPE:
	  return "Any";
     }
     return "???";
}
