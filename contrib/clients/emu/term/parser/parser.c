#ifndef lint
static char *rcsid = "$Header: /usr3/Src/emu/term/parser/RCS/parser.c,v 1.7 91/09/30 21:58:11 me Exp Locker: jkh $";
#endif

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
 * These are the main entry points for the parser section.
 *
 * Author: Jordan K. Hubbard
 * Date: October 7th, 1990.
 * Description: General parser init code and switch table point for
 * all hard-coded parsers.
 *
 * Revision History:
 *
 * $Log:	parser.c,v $
 * Revision 1.7  91/09/30  21:58:11  me
 * Parser reorganization.
 * 
 * Revision 1.6  91/09/30  13:26:49  jkh
 * Whatever Jordan did
 * 
 * Revision 1.5  91/07/26  17:02:01  jkh
 * #ifdef'd the debug statement.
 * 
 * Revision 1.4  91/01/28  10:56:30  jkh
 * Changed EIO handling for sun. I did have it wrong. Double sigh.
 * 
 * Revision 1.3  91/01/28  10:42:57  jkh
 * Added EIO as "reasonable" error for Suns who seem to like to generate
 * that one when reading from a non-blocking fd that has no output ready
 * yet. I hope I don't come to regret this..
 * 
 * 
 * Revision 1.2  90/11/20  17:46:27  jkh
 * Alpha.
 * 
 * Revision 1.1  90/11/13  04:02:46  jkh
 * Initial revision
 * 
 *
 */

#include "TermP.h"

Import void parseDumb();
Import void parseGeneric();
Import void parseHEmu();

Local ParserTable parseTab[] = {
     {  "emu",		parseHEmu,	NULL	},
     {	"dumb",		parseDumb,	NULL	},
     {	"tty",		parseDumb,	NULL	},
     {	NULL,		parseGeneric,	NULL	},
};

/* Used to both add and delete callback entries */
Local void
modify_table_callbacks(w, idx, fptr)
TermWidget w;
int idx;
void (*fptr)();
{
     if (parseTab[idx].in_parser)
	  (*fptr)(w, XpNinParser, parseTab[idx].in_parser, w->term.master);
     if (parseTab[idx].out_parser)
	  (*fptr)(w, XpNoutParser, parseTab[idx].out_parser, w->term.master);

     /*
      * If we're referring to the generic input parser, make sure we
      * initialize it first.
      */
     if (parseTab[idx].in_parser == parseGeneric) {
	  Import void parse_init();

	  parse_init(w);
     }
}

/*
 * Look for and install a parser for the terminal type specified in the
 * term widget.
 */
Export Boolean
parser_install(w)
TermWidget w;
{
     int i, added_hard = 0;
     String ops, term = w->term.term_type;

     for (i = 0; parseTab[i].name; i++) {
	  if (!strcmp(term, parseTab[i].name)) {
	       modify_table_callbacks(w, i, XtAddCallback);
	       added_hard = 1;
	       break;
	  }
     }

     if (!added_hard) {
	  /* Add default if we fell off the end and it's never been added */
	  if (XtHasCallbacks((Widget)w, XpNinParser) == XtCallbackHasNone)
	       modify_table_callbacks(w, i, XtAddCallback);
     
#ifndef NO_GENERIC_PARSER
	  if (ops = get_sub_resource_string(w, term, "ops", "Ops")) {
	       ops = backslash_convert(ops);
	       parse_add(w, ops);
	  }
	  else 
	       return FALSE;
#endif
     }

     /* Get ROPS either way */
     if ((ops = get_sub_resource_string(w, term, "rops", "Rops"))) {
	  ops = backslash_convert(ops);
	  rparse_add(w, ops);
     }
     return TRUE;
}

Export void
parser_remove(w, term)
TermWidget w;
String term;
{
     int i;

     for (i = 0; parseTab[i].name; i++) {
	  if (!strcmp(term, parseTab[i].name)) {
	       modify_table_callbacks(w, i, XtRemoveCallback);
	       break;
	  }
     }

     /* Remove default if we fell off the end */
     if (!parseTab[i].name)
	  modify_table_callbacks(w, i, XtRemoveCallback);
}

/* The input parser */
Export void
iparse(w, fdp, id)
TermWidget w;
int *fdp;
XtInputId id;
{
     Import int errno;
     Import int read();
     int nread, fd = *fdp;

     if (fd < 0)
	  fatal("parse called with negative file descriptor (%d).", fd);
     do {
	  char *inbuf = ((char *)(w->term.buf + w->term.chars_to_parse));

	  nread = read(fd, inbuf, w->term.read_size);
     } while ((nread == -1) && ((errno == EAGAIN) || (errno == EINTR)));
     if (nread == -1) {
	  warn("read error on fd %d in parse, errno = %d.", fd, errno);
	  return;
     }
     w->term.chars_to_parse += nread;
#ifdef DEBUG
     debug("iparse: read %d bytes", nread);
#endif

     /* Call everyone on what we read */
     XtCallCallbackList((Widget)w, w->term.in_parse, w->term.buf);
}

/* Rarely used, but where we dispatch output parsing callbacks */
Export void
oparse(w, fdp, id)
TermWidget w;
int *fdp;
XtInputId id;
{
     XtCallCallbackList((Widget)w, w->term.out_parse, *fdp);
}
