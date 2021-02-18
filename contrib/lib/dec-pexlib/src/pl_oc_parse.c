/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
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


/*
**++
**  File Name:
**     pl_parseoc.c
**
**  Subsystem:
**     pexlib
**
**  Version: 
**     0.1
**
**  Abstract:
**     oc buffer debug functions
**
**  Keywords:
**     pexlib
**
**  Environment:
**     pexlib
**
**
**  Creation Date:
**     18-Jan-91
**
**  Modification History:
**--
*/

/*
 *  Table of Contents: (Routine names in order of appearance)
 */

/*
 * E	PEXDumpOCBufHeader
 * E	PEXGetOCListsFromOCBuf
 */

/*
 *  Include Files
 */

#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>

#include "pex_features.h"
#include "pex_macros.h"
#include "PEXlib.h"
#include "PEXproto.h"

#include "pl_oc_buff.h"

#ifdef MPEX
# include "MPEX.h"
# include "MPEXproto.h"
#endif

/*
 *  Internal Definitions (macro, struct, and typedef definitions)
 */


/*
 *  Internal Data Definitions
 */


/*
 *  Internal Routine Declarations
 */


/*+
 * NAME:
 *     PEXDumpOCBufHeader
 *
 * FORMAT:
 *	`PEXDumpOCBufHeader`
 *
 * ARGUMENTS:
 *	oc		oc buffer to dump
 *
 *	output_file	file to which we should dump the info
 *
 * RETURNS:
 *    None
 *
 * DESCRIPTION:
 *	This routine when processed dumps internal OC Buffer information
 *	to the file specified by `output_file'.
 *
 *  ERRORS:
 *
 *  SEE ALSO:
 *
 */

void
PEXDumpOCBufHeader (oc, output_file)

INPUT pxlOCBuf		* oc;
INPUT_OUTPUT FILE	* output_file;

{
    pxlRetainedOCBuf	* roc;
    pxlTransientOCBuf * toc;
    pxlOCChunk		*pchunk;
    int			i;

    /* tell the user about the header of the OC buffer */

    fprintf (output_file, "The initLength is %d\n", oc->initLength);
    fprintf (output_file, "The extension opcode is %d\n", oc->extOpcode);
    if (oc->pexOpcode == pxlRenderImmediate)
    {
	fprintf (output_file, "The PEX opcode is pxlRenderImmediate (RenderOutputCommands)\n");
	fprintf (output_file, "The target renderer resource is 0x%x\n", oc->target);
    }
    else if (oc->pexOpcode == pxlAddToStructure)
    {
	fprintf (output_file, "The PEX opcode is pxlAddToStructure (StoreElements)\n");
	fprintf (output_file, "The target structure resource is 0x%x\n", oc->target);
    }
    else
    {
	fprintf (output_file, "The PEX opcode is INVALID %d\n", oc->pexOpcode);
	fprintf (output_file, "The target is 0x%x\n", oc->target);
    }

    if ( ((ADDRESS) oc->OCError) == (ADDRESS) PEXDefaultOCError)
	{fprintf (output_file, "The error function is PEXDefaultOCError\n");}
    else
	{fprintf (output_file, "The error function is user-defined at 0x%X\n");}

    fprintf (output_file, "oc words left = %d, lr sequence # = %d, lr buffer left = %d\n",
	oc->ocWordsLeft, oc->lrSequenceNum, oc->lrBufferLeft);
    fprintf (output_file, "lr buffer size = %d, lr total packets = %d, lr total length = %d\n",
	oc->lrBufferLeft, oc->lrTotalNum, oc->lrTotalLength);

    /* make sure that this is a retained oc buffer */
    if ( ! oc->retainedOCBuffer)
    {
        fprintf (output_file, "This is a transient oc buffer\n");
	toc = (pxlTransientOCBuf *) oc;

	fprintf (output_file, "    request number %d\n",
		toc->reqNumber);
    }
    else
    {
    
	roc = (pxlRetainedOCBuf *) oc;

	/* tell the user about the chunks */

	pchunk = roc->chunkList;

	i = 1;
	while (pchunk != 0)
	{
	    pexOCRequestHeader *pReq = (pexOCRequestHeader *)&pchunk[1];
	    pexElementInfo *p = (pexElementInfo *)&pReq[1];

	    fprintf (output_file, "Chunk %d size is %d\n", i, pchunk->size);
	    fprintf (output_file, "The request length is %d\n", pReq->reqLength);
	    fprintf (output_file, "The number of commands is %d\n", pReq->numCommands);
	    if (roc->currentChunk == pchunk)
	    {
		fprintf (output_file, "This is the current chunk\n");
	    }
	    pchunk = pchunk->pnext;
	    i++;
	}
    }

    return;
}

/*+
 * NAME:
 *     PEXGetOCListsFromOCBuf
 *
 * FORMAT:
 *	`PEXGetOCListsFromOCBuf`
 *
 * ARGUMENTS:
 *	oc		oc buffer from which to get the OC lists
 *
 *	routine		routine to call with each oc list
 *
 *	arg		parameter to pass to routine
 *
 * RETURNS:
 *     None
 *
 * DESCRIPTION:
 *	This routine when processed goes through the lists of OC's in the
 *	OCBuffer and calls the given routine for each list of output commands.
 *
 * ERRORS:
 *
 * SEE ALSO:
 *
 */

void
PEXGetOCListsFromOCBuf (oc, routine, arg)

INPUT pxlOCBuf		* oc;
INPUT void		(*routine) ();
INPUT unsigned long	arg;

{
    pxlOCChunk		*pchunk;
    int			i;

    /* make sure that this is a retained oc buffer */
    if ( ! oc->retainedOCBuffer)
    {
	return;
    }

    /* tell the user about the header of the OC buffer */

    pchunk = ((pxlRetainedOCBuf *) oc)->chunkList;

    while (pchunk != 0)
    {
	pexOCRequestHeader *pReq = (pexOCRequestHeader *)&pchunk[1];
        pexElementInfo *p = (pexElementInfo *)&pReq[1];

	(*routine) (arg, p, pReq->numCommands);

	pchunk = pchunk->pnext;
    }

    return;
}
