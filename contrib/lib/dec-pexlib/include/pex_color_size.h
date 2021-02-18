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

#ifndef	PEX_COLOR_SIZE_H
#define	PEX_COLOR_SIZE_H

/* this has been defined statically so that we don't use external globals */

static int     colorSize [(PEXMaxColour + 1)] =
{
    sizeof(pexIndexedColour),
    sizeof(pexRgbFloatColour),
    sizeof(pexHsvColour),
    sizeof(pexHlsColour),
    sizeof(pexCieColour),
    sizeof(pexRgb8Colour),
    sizeof(pexRgb16Colour)
};

static int	colorSizeAdjust[(PEXMaxColour + 1)] =
{
    sizeof(pexIndexedColour) - sizeof (pexColour),
    sizeof(pexRgbFloatColour) - sizeof (pexColour),
    sizeof(pexHsvColour) - sizeof (pexColour),
    sizeof(pexHlsColour) - sizeof (pexColour),
    sizeof(pexCieColour) - sizeof (pexColour),
    sizeof(pexRgb8Colour) - sizeof (pexColour),
    sizeof(pexRgb16Colour) - sizeof (pexColour)
};

#endif	PEX_COLOR_SIZE_H
