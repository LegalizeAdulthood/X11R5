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



/* Module comment:  Data definitions for PEXlib */

EXTERNAL_DEF XExtCodes	*pexExtCodes[128] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

#ifdef VAX
EXTERNAL_DEF int	pexFpFormat		= PEXDEC_F_Floating;
#else
EXTERNAL_DEF int	pexFpFormat		= PEXIeee_754_32;
#endif /* VAX */

EXTERNAL_DEF int	pexColourTypeHidden	= PEXRgbFloatColour;
EXTERNAL_DEF int	pexOCError		= 0;

EXTERNAL_DEF char	*pexErrorList[(PEXMaxError + 1)] = {
/* ColourType */
    "PEXColourTypeError, specified colour type not supported",
/* RendererState */
    "PEXRendererStateError, renderer not in proper state for operation",
/* FloatingPointFormat */
    "PEXFloatingPointFormatError, specified fp format not supported",
/* Label */
    "PEXLabelError, specified label not in structure",
/* LookupTable */
    "LookupTableError, specified lookup table ID invalid",
/* NameSet */
    "PEXNameSetError, specified nameset ID invalid",
/* Path */
    "PEXPathError, illegal or invalid values in path",
/* PEXFont */
    "PEXFontError, specified font ID invalid",
/* PhigsWKS */
    "PhigsWKSError, specified PHIGS workstation ID invalid", 
/* PickMeasure */
    "PEXPickMeasureError, specified pick measure ID invalid",
/* PipelineContext */
    "PEXPipelineContextError, specified pipeline context ID invalid",
/* Renderer */
    "PEXRendererError, specified renderer ID invalid",
/* SearchContext */
    "PEXSearchContextError, specified search context ID invalid",
/* Structure */
    "PEXStructureError, specified structure ID invalid",
/* OutputCommand */
    "PEXOutputCommandError, illegal value in output commands",
};
