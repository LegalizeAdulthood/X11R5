/***********************************************************
Copyright 1989 by Digital Equipment Corporation, Maynard, Massachusetts,
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

/* Definitions for Digital Pick/Echo/Z PEX extension likely to be used by
applications */

#ifndef MPEXPROTOSTR_H
#define MPEXPROTOSTR_H

/* Typedefs for lookup tables */

typedef struct
{
    pexEnumTypeIndex	method B16;
    CARD16		pad B16;
    pexColourSpecifier	colour;
} mpexHighlightEntry;

typedef struct
{
    pexEnumTypeIndex	method B16;
    CARD16		pad B16;
    pexColourSpecifier	colour;
} mpexEchoEntry;

typedef struct
{
    Pixmap		pixmapId B32;
    INT16		xOffset B16;
    INT16		yOffset B16;
} mpexPixmapEntry;

#endif /* MPEXPROTOSTR_H */
