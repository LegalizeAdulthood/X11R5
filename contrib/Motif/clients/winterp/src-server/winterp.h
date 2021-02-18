/* -*-C-*-
********************************************************************************
*
* File:         winterp.h
* RCS:          $Header: winterp.h,v 1.9 91/04/22 00:25:33 mayer Exp $
* Description:  Miscellaneous macros, #defines, etc
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 14:09:08 1988
* Modified:     Fri Oct  4 03:22:45 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/

#define WINTERP_VERSION_INT 1
#define WINTERP_REVISION_INT 13

/*----------------------------------------------------------------------------
 * Kludges to sweeten Xt's ugly argument processing for widget creation.
 *---------------------------------------------------------------------------*/
extern Arg _args[];		/* global in winterp.c */
extern int _num_args;		/* global in winterp.c */

#define ARGLIST_RESET() \
  _num_args = 0

#define ARGLIST_ADD(name, value) \
  XtSetArg(_args[_num_args], (name), (value)); _num_args++

#define ARGLIST_SET_LAST_ELT(name, value) \
  XtSetArg(_args[(_num_args - 1)], (name), (value))

#define ARGLIST() \
  _args, _num_args


/* Setup cpp symbols for conditional compilation of Motif 1.0 vs Motif 1.1 */

#if (XmVersion >= 1001)		/* XmVersion from <Xm/Xm.h> */
#define WINTERP_MOTIF_11
#endif

#ifndef WINTERP_MOTIF_11	/* Motif 1.0 backwards compatibility */
typedef unsigned char XtEnum;
#endif

#ifndef WINTERP_MOTIF_11	/* Motif 1.0 backwards compatibility */
typedef char* XtPointer;
#endif

/*
 * Motif 1.1.1 has some changes over version 1.1 that are important enough to
 * require #ifdefs in the code. Gratuitous, consistency-violating alterations
 * to the API were made in fixing the 1.1 bugs. Because Motif 1.1 and Motif 1.1.1
 * cannot be identified through any sort of documented means, I had to resort
 * to a hack, in the truest sense of the word, in order to work around this
 * charming detail. The hack to determine which version of Motif 1.1 you're
 * running: 1.1.1 defines the new symbol XmDYNAMIC_DEFAULT_TAB_GROUP, and
 * 1.1 doesn't... The presence of that symbol tells WINTERP that we're using
 * 1.1.1 and defines WINTERP_MOTIF_111. This is used in a few places in
 * w_resources.c, for example.
 */
#ifdef WINTERP_MOTIF_11
#ifdef XmDYNAMIC_DEFAULT_TAB_GROUP
#define WINTERP_MOTIF_111
#endif				/* XmDYNAMIC_DEFAULT_TAB_GROUP */
#endif				/* WINTERP_MOTIF_11 */

/*
 * Motif 1.1.3 defines two new symbols, XmCR_CREATE and XmCR_PROTOCOLS, use
 * these to tell us we're running 1.1.3...
 */
#ifdef WINTERP_MOTIF_11
#ifdef XmCR_CREATE
#define WINTERP_MOTIF_113
#endif /* XmCR_CREATE */
#endif /* WINTERP_MOTIF_11 */
