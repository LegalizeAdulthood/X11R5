/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


#ifndef MPEXOC_H
#define MPEXOC_H

#include <X11/Xlib.h>

#include "PEX.h"
#include "PEXprotostr.h"
#include "MPEX.h"

/* C programmers may choose to use these macros to create output commands
 * that set attributes instead of the subroutine calls in order to save the
 * overhead of a subroutine call for each output command. */


/* MPEX_SetHighlightIndex
 *
 *  pexOCBuf		*ocbuf;
 *  pexTableIndex	*index;
 */
#define MPEX_SetHighlightIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), MPEXOCHighlightIndex, sizeof(pexTableIndex), (index))

/* MPEX_SetEchoIndex
 *
 *  pexOCBuf		*ocbuf;
 *  pexTableIndex	*index;
 */
#define MPEX_SetEchoIndex(ocbuf, index) \
    PEXAddOC ((ocbuf), MPEXOCEchoIndex, sizeof(pexTableIndex), (index))

/* MPEX_Noop
 *
 *  pexOCBuf		*ocbuf;
 */
#define MPEX_Noop(ocbuf) \
    PEXAddOC ((ocbuf), MPEXOCNoop, 0, NULL)


#endif /* MPEXOC_H */
