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
**
**	pl_oc_buff.c
**
**  Subsystem:
**
**	PEXlib
**
**  Version:
**
**	V1.0
**
**  Abstract:
**
**	This file contains the routines that manipulate PEX output command
**	buffers.
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
**
**
**  Modification History:
**	19-Aug-88  
**
**--
*/

/*
 *   Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXAllocateRetainedOCBuffer
 * E   PEXAllocateTransientOCBuffer
 * I   PEXDeallocateRetainedOCBuffer
 * I   PEXDeallocateTransientOCBuffer
 * I   PEXClearRetainedOCBuffer
 * I   PEXSetRetainedOCBufferType
 * I   PEXSetTransientOCBufferType
 * I   PEXSetRetainedOCBufferTarget
 * I   PEXSetTransientOCBufferTarget
 * E   PEXGetDefaultOCBufferType
 * I   PEXGetRetainedOCBufferFreeSpace
 * I   PEXGetTransientOCBufferFreeSpac
 * I   PEXGetMaxRetainedOCChunkSize
 * I   PEXGetMaxTransientOCChunkSize
 * I   PEXFlushRetainedOCBuffer
 * I   PEXFlushTransientOCBuffer
 * I   PEXSendRetainedOCBuffer
 * I   PEXNoop
 * I   allocNewChunk
 * I   PEXRetainedStartOC
 * I   PEXTransientStartOC
 * I   PEXRetainedStartLargeRequest
 * I   PEXTransientStartLargeRequest
 * I   PEXRetainedFinishOC
 * I   PEXTransientFinishOC
 * E   PEXGetWordsError
 * I   PEXTransientGetOCWords
 * E   PEXCopyBytesError
 * E   PEXInitOC
 * E   PEXCopyBytesToOC
 * E   PEXCopyWordsToOC
 * E   PEXGetOCWords		DEBUG only
 * E   PEXAddOC			DEBUG only
 * E   PEXAddListOC		DEBUG only
 * I   PEXDefaultStoreOCList
 * E   PEXDefaultTransientOCBuffer
 * E   PEXDefaultRetainedOCBuffer
 * E   PEXDefaultOCError
 * E   PEXSetOCOverflow
 * E   PEXGetOCOverflow
 * E   PEXClearOCOverflow
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include "PEX.h"
#include "PEXlib.h"
#include "PEXlibint.h"

#include "MPEX.h"
#include "MPEXproto.h"

#include "pl_ref_data.h"
#include "pl_oc_buff.h"


/*
 *  Internal Routine Declarations
 */
#ifdef DEBUG
# define PEXRecoverTransientOCRequest(_ocbuf)		\
{							\
    ((pexRequestHeader *)((_ocbuf)->header.basePtr))->reqLength -=  \
                ((_ocbuf)->header.ocWordsLeft);		\
    PEXFlushTransientOCBuffer( _ocbuf);			\
    UnlockDisplay((_ocbuf)->header.display);		\
    (*(_ocbuf)->header.OCError)(_ocbuf, 0);		\
}
#endif

#ifdef DEBUG
# define PEXRecoverRetainedOCRequest(_ocbuf)		\
{							\
    ((pexRequestHeader *)((_ocbuf)->header.basePtr))->reqLength -=  \
                ((_ocbuf)->header.ocWordsLeft);		\
    (*(_ocbuf)->header.OCError)(_ocbuf, 0);		\
}
#endif

INTERNAL void PEXDeallocateRetainedOCBuffer
    ARGS ((
	INPUT_OUTPUT pxlRetainedOCBuf	* ocbuf
	));

INTERNAL void PEXDeallocateTransientOCBuffer
    ARGS ((
	INPUT_OUTPUT pxlTransientOCBuf	* ocbuf
	));

INTERNAL void PEXClearRetainedOCBuffer
    ARGS ((
	INPUT_OUTPUT pxlRetainedOCBuf	* ocbuf
	));

INTERNAL void PEXSetRetainedOCBufferType
    ARGS ((
	INPUT Display		*display,
	INPUT pxlRetainedOCBuf	*ocbuf,
	INPUT int		type,
	INPUT XID		target
	));

INTERNAL void PEXSetTransientOCBufferType
    ARGS ((
	INPUT Display		*display,
	INPUT pxlTransientOCBuf	*ocbuf,
	INPUT int		type,
	INPUT XID		target
	));

INTERNAL void PEXSetRetainedOCBufferTarget
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf,
	INPUT XID		target
	));

INTERNAL void PEXSetTransientOCBufferTarget
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf,
	INPUT XID		target
	));

INTERNAL int PEXGetRetainedOCBufferFreeSpace
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL int PEXGetTransientOCBufferFreeSpac
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf
	));

INTERNAL int PEXGetMaxRetainedOCChunkSize
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL int PEXGetMaxTransientOCChunkSize
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf
	));

INTERNAL void PEXFlushRetainedOCBuffer
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL void PEXFlushTransientOCBuffer
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf
	));

INTERNAL void PEXSendRetainedOCBuffer
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL void PEXNoop
    ARGS ((
	INPUT pxlOCBuf	*ocbuf
	));

INTERNAL pxlOCChunk * allocNewChunk
    ARGS ((
	pxlRetainedOCBuf	*ocbuf,
	int	 		dataSize
	));

INTERNAL int PEXRetainedStartOC
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf,
	INPUT int		ocLength
	));

INTERNAL int PEXTransientStartOC
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf,
	INPUT int		ocLength
	));

INTERNAL int PEXRetainedStartLargeRequest
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL int PEXTransientStartLargeRequest
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf
	));

INTERNAL void PEXRetainedFinishOC
    ARGS ((
	INPUT pxlRetainedOCBuf	*ocbuf
	));

INTERNAL void PEXTransientFinishOC
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf
	));

INTERNAL unsigned int * PEXTransientGetOCWords
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf,
	INPUT int		nWords
	));

INTERNAL void PEXTransientCopyOCBytes
    ARGS ((
	INPUT pxlTransientOCBuf	*ocbuf,
	INPUT int		nBytes,
	INPUT char 		*buffer
	));

/*+
 * NAME:
 * 	PEXAllocateRetainedOCBuffer
 *
 * FORMAT:
 * 	`PEXAllocateRetainedOCBuffer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 * 	type		The type of oc buffer (pxlRenderImmediate or
 *			pxlAddToStructure)
 *
 *	target		The resource identifier of the renderer or structure
 *			used as a target for the output commands
 *
 *	errorFunction	Called if an error occurs when writing to the OC buffer 
 *
 *	initLength 	Hint as to how large oc requests should be (in words)
 *			If zero, then a default size will be used.
 *
 * RETURNS:
 *	A pointer to the allocated OC buffer,
 *	or a zero if the allocation failed.
 *
 * DESCRIPTION:
 *	This routine allocates a retained oc buffer and returns a pointer to
 *	it.  The output commands are stored in a separately allocated buffer
 *	and are sent to the server through the connection specified by
 *	'display' when the oc buffer is flushed or sent (via `PEXFlushOCBuffer" 
 *	or `PEXSendOCBuffer").  Retained oc buffers are useful for building oc 
 *      requests which will be sent to the server more than once. 
 *
 *	If `type' is `pxlRenderImmediate', requests that pass output commands 
 *	to a renderer are created.
 *	If `type' is `pxlAddToStructure', requests that pass output commands to 
 *	a structure are created.
 *	The `PEXlib.h' include file contains definitions of these constants.
 *
 *	`Target' defines the destination for the output commands.  
 *	If the buffer type is `pxlRenderImmediate', `target' is the resource 
 *	identifier of the renderer to which the output commands will be sent.
 *	If the buffer type is `pxlAddToStructure', `target' is the resource 
 *	identifier of the structure to which the output commands will be sent.
 *	If the resource identifier is unknown it may be set to a dummy value
 *	initially and then set later using `PEXSetOCBufferTarget".  The 
 *	target must be set to a valid resource identifier before the oc
 *	buffer is sent or flushed (via `PEXSendOCBuffer" or `PEXFlushOCBuffer")
 *
 *	The `errorFunction' specifies what action is to be taken when the 
 *	output command cannot be written to the buffer either because it is 
 *	too large or because an alloc error has occured. 
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXDeallocateRetainedOCBuffer", `PEXCreateRenderer", 
 *	`PEXCreateStructure"
 */
pxlOCBuf *
PEXAllocateRetainedOCBuffer (display, type, target, errorFunction, initLength)

INPUT Display		*display;
INPUT int		type;
INPUT XID		target;
INPUT void		(* errorFunction)();
INPUT unsigned int	initLength;

{
/* AUTHOR: M.P. Womack */
    pxlRetainedOCBuf	*ocbuf;

    /* 
     * allocate the new oc buffer 
     */
    ocbuf = (pxlRetainedOCBuf *) PEXAllocBuf( sizeof(pxlRetainedOCBuf));
    if (ocbuf == NULL) return ((pxlOCBuf *)ocbuf);

    ocbuf->header.retainedOCBuffer = 1;

    /* 
     * initialize the header of the oc buffer
     */

#define DEFAULT_CHUNK_SIZE 4098
    /* initLength can't be bigger than the maximum transport buffer size */
    ocbuf->header.initLengthRequested = (initLength > 0) ? 
		initLength : DEFAULT_CHUNK_SIZE;
    ocbuf->header.initLength = MIN( ocbuf->header.initLengthRequested, 
		display->max_request_size);

    ocbuf->header.display = display;
    ocbuf->header.extOpcode = 
		pexExtCodes[ConnectionNumber(display)]->major_opcode;
    ocbuf->header.pexOpcode = type;
    ocbuf->header.target = target;

    /* the maximum amount of data which can be sent per large request packet */
    ocbuf->header.lrBufferLength = PEXGetMaxRetainedOCChunkSize(ocbuf) -
		LENOF( mpexLargeRequestReq);

    /* no large requests or ocs in progress */
    ocbuf->header.basePtr = NULL;
    ocbuf->header.ocWordsLeft = ocbuf->header.lrSequenceNum  = 0;

    ocbuf->header.OCError = 
		(errorFunction == NULL) ? PEXDefaultOCError : errorFunction;
    ocbuf->header.FreeOCBuf = PEXDeallocateRetainedOCBuffer;
    ocbuf->header.ClearOCBuf = PEXClearRetainedOCBuffer;
    ocbuf->header.SetOCBufType = PEXSetRetainedOCBufferType;
    ocbuf->header.SetOCBufTarget = PEXSetRetainedOCBufferTarget;
    ocbuf->header.GetOCBufType = PEXGetDefaultOCBufferType;
    ocbuf->header.GetOCBufFreeSpace = PEXGetRetainedOCBufferFreeSpace;
    ocbuf->header.GetMaxOCChunkSize = PEXGetMaxRetainedOCChunkSize;
    ocbuf->header.FlushOCBuf = PEXFlushRetainedOCBuffer;
    ocbuf->header.SendOCBuf = PEXSendRetainedOCBuffer;
    ocbuf->header.StartOC = PEXRetainedStartOC;
    ocbuf->header.StartLargeRequest = PEXRetainedStartLargeRequest;
    ocbuf->header.FinishOC = PEXRetainedFinishOC;
    ocbuf->header.StoreOCList = PEXDefaultStoreOCList;

    /*
     * When using retained oc buffers ocs will never need to be split across 
     * buffer boundaries so falling into one of these routines is an error.
     */
    ocbuf->header.GetOCWords = PEXGetWordsError;
    ocbuf->header.CopyOCBytes = PEXCopyBytesError;

    ocbuf->chunkList = ocbuf->currentChunk = NULL;

    return ((pxlOCBuf *)ocbuf);
}

/*+
 * NAME:
 * 	PEXAllocateTransientOCBuffer
 *
 * FORMAT:
 * 	`PEXAllocateTransientOCBuffer`
 *
 * ARGUMENTS:
 *	display		A pointer to a display structure
 *			returned by a successful `XOpenDisplay" call.
 *
 * 	type		The type of oc buffer (pxlRenderImmediate or
 *			pxlAddToStructure)
 *
 *	target		The resource identifier of the renderer or structure
 *			used as a target for the output commands
 *
 *	errorFunction	Called if an error occurs when writing to the OC buffer 
 *
 *	initLength 	Hint as to how large oc requests should be (in words)
 *			If zero then oc requests will be flushed after each
 *			oc.  If non-zero then oc requests may grow as large
 *			as the transport buffer size.
 *
 * RETURNS:
 *	A pointer to the allocated OC buffer,
 *	or a zero if the allocation failed.
 *
 * DESCRIPTION:
 *	This routine allocates a transient oc buffer and returns a pointer
 *	to it.  The output commands are stored directly into the connection
 *      queue specified by `display'. The request which contains the output
 *	commands will be available to the server immediately.
 *	Transient oc buffers yield better performance than retained oc 
 *	buffers since they do not copy the output commands to a separate buffer.
 *
 *	If `type' is `pxlRenderImmediate', requests that pass output commands 
 *	to a renderer are created.
 *	If `type' is `pxlAddToStructure', requests that pass output commands to 
 *	a structure are created.
 *	The `PEXlib.h' include file contains definitions of these constants.
 *
 *	`Target' defines the destination for the output commands.  
 *	If the buffer type is `pxlRenderImmediate', `target' is the resource 
 *	identifier of the renderer to which the output commands will be sent.
 *	If the buffer type is `pxlAddToStructure', `target' is the resource 
 *	identifier of the structure to which the output commands will be sent.
 *	If the resource identifier is unknown it may be set to a dummy value
 *	initially and then set later using `PEXSetOCBufferTarget".  The
 *	target must be set to a valid resource identifier before any ocs are
 *	stored in the oc buffer. 
 *
 *	The `errorFunction' specifies what action is to be taken when the 
 *	output command cannot be written to the buffer either because it is 
 *	too large or because an alloc error has occured. 
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXDeallocateRetainedOCBuffer", `PEXCreateRenderer", 
 *	`PEXCreateStructure"
 */

pxlOCBuf *
PEXAllocateTransientOCBuffer (display, type, target, errorFunction, initLength)

INPUT Display		*display;
INPUT int		type;
INPUT XID		target;
INPUT void		(* errorFunction)();
INPUT int		initLength;

{
/* AUTHOR: M.P. Womack */
    pxlTransientOCBuf	*ocbuf;

    /*   
     * allocate the new oc buffer 
     */
    ocbuf = (pxlTransientOCBuf *) PEXAllocBuf( sizeof(pxlTransientOCBuf));
    if (ocbuf == NULL) return ((pxlOCBuf *)ocbuf);

    ocbuf->header.retainedOCBuffer = 0;

    /* 
     * initialize the header of the oc buffer 
     */
    ocbuf->header.display = display;

    /* can't be bigger than the maximum transport buffer size */
    ocbuf->header.initLengthRequested = initLength;
    ocbuf->header.initLength = MIN( ocbuf->header.initLengthRequested, 
		display->max_request_size);

    ocbuf->header.extOpcode = 
		pexExtCodes[ConnectionNumber(display)]->major_opcode;
    ocbuf->header.pexOpcode = type;
    ocbuf->header.target = target;

    /* the maximum amount of data which can be sent per large request packet */
    ocbuf->header.lrBufferLength = PEXGetMaxTransientOCChunkSize(ocbuf) -
		LENOF( mpexLargeRequestReq);

    /* no large requests or ocs in progress */
    ocbuf->header.basePtr = NULL;
    ocbuf->header.ocWordsLeft = ocbuf->header.lrSequenceNum = 0;

    ocbuf->header.OCError = 
		(errorFunction == NULL) ? PEXDefaultOCError : errorFunction;
    ocbuf->header.FreeOCBuf = PEXDeallocateTransientOCBuffer;
    ocbuf->header.ClearOCBuf = PEXFlushTransientOCBuffer;
    ocbuf->header.SetOCBufType = PEXSetTransientOCBufferType;
    ocbuf->header.SetOCBufTarget = PEXSetTransientOCBufferTarget;
    ocbuf->header.GetOCBufType = PEXGetDefaultOCBufferType;
    ocbuf->header.GetOCBufFreeSpace = PEXGetTransientOCBufferFreeSpac;
    ocbuf->header.GetMaxOCChunkSize = PEXGetMaxTransientOCChunkSize;
    ocbuf->header.FlushOCBuf = PEXNoop;
    ocbuf->header.SendOCBuf = PEXNoop;
    ocbuf->header.StartOC = PEXTransientStartOC;
    ocbuf->header.StartLargeRequest = PEXTransientStartLargeRequest;
    ocbuf->header.FinishOC = PEXTransientFinishOC;
    ocbuf->header.StoreOCList = PEXDefaultStoreOCList;

    /*
     * When using retained oc buffers the ocs will never need to be split 
     * across buffer boundaries so falling into this is an error.
     */

    ocbuf->header.GetOCWords = PEXTransientGetOCWords;
    ocbuf->header.CopyOCBytes = PEXTransientCopyOCBytes;

    ocbuf->reqNumber = -1;

    return ((pxlOCBuf *)ocbuf);
}

/*+
 * NAME:
 *    PEXDeallocateRetainedOCBuffer
 * 
 * FORMAT:
 * 	void PEXDeallocateRetainedOCBuffer( ocbuf)
 *	pxlRetainedOCBuf 	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be deallocated.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine frees all memory for the retained oc buffer
 *	specified by `ocbuf'.
 *
 * ERRORS:
 *	None
 */

INTERNAL void
PEXDeallocateRetainedOCBuffer (ocbuf)
INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */

    /* free all the data chunks in the oc buffer */
    PEXClearRetainedOCBuffer (ocbuf);

    /* free the header portion of the oc buffer */
    PEXFreeBuf ((char *)ocbuf);

    return;
}

/*+
 * NAME:
 *    PEXDeallocateTransientOCBuffer
 *
 * FORMAT:
 * 	void PEXDeallocateTransientOCBuffer( ocbuf)
 *	pxlOCBuf 	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be deallocated.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine frees all memory for the transient oc buffer
 *	specified by `ocbuf'.
 *
 * ERRORS:
 *	None
 */

INTERNAL void
PEXDeallocateTransientOCBuffer (ocbuf)
INPUT pxlTransientOCBuf		*ocbuf;

{
/* AUTHOR: M.P. Womack */

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverTransientOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* free the header portion of the oc buffer */
    PEXFreeBuf ((char *)ocbuf);

    return;
}

/*+
 * NAME:
 *    PEXClearRetainedOCBuffer
 *
 * FORMAT:
 * 	void PEXClearRetainedOCBuffer( ocbuf)
 *	pxlRetainedOCBuf 	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be cleared 
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine frees all memory for the retained oc buffer
 *	specified by `ocbuf'.
 *
 * ERRORS:
 *	None
 */

INTERNAL void
PEXClearRetainedOCBuffer (ocbuf)
INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */
    pxlOCChunk	*pchunk;
    pxlOCChunk	*pnext;

    /* get the pointer to the first chunk */
    pchunk = ocbuf->chunkList;

    /* free all the data chunks in the oc buffer */
    while (pchunk)
    {
	pnext = pchunk->pnext;
	PEXFreeBuf ((char *)pchunk);
	pchunk = pnext;
    }

   /*
    * clear oc buffer -- no ocs or large requests in progress
    */
    ocbuf->header.basePtr = NULL;
    ocbuf->header.ocWordsLeft = 0;
    ocbuf->chunkList = ocbuf->currentChunk = NULL;

    return;
}

/*+
 * NAME:
 *    PEXSetRetainedOCBufferType
 *
 * FORMAT:
 *      void PEXSetRetainedOCBufferType (display, ocbuf, type, target)
 *      Display         *display;
 *      pxlRetainedOCBuf        *ocbuf;
 *      int             type;
 *      XID             target;
 *
 * ARGUMENTS:
 *      display         A pointer to a display structure
 *                      returned by a successful `XOpenDisplay" call.
 *
 *      ocbuf           The OC buffer whose attributes are
 *                      to be modified.
 *
 *      type            The type of OC buffer.
 *
 *      target          The resource identifier of the renderer or structure
 *                      used as a target for the output commands
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Change the display, target and type associated with the oc buffer.
 *      This routine will flush the current contents of `ocbuf'.
 *
 * ERRORS:
 *      None
 */

INTERNAL void
PEXSetRetainedOCBufferType (display, ocbuf, type, target)

INPUT Display		*display;
INPUT pxlRetainedOCBuf	*ocbuf;
INPUT int		type;
INPUT XID		target;

{
/* AUTHOR: M.P. Womack */

    PEXFlushRetainedOCBuffer( ocbuf);

    ocbuf->header.display = display;
    ocbuf->header.initLength = MIN( ocbuf->header.initLengthRequested, 
		display->max_request_size);
    ocbuf->header.target = target;
    ocbuf->header.pexOpcode = type;

    /* the maximum amount of data which can be sent per large request packet */
    ocbuf->header.lrBufferLength = PEXGetMaxRetainedOCChunkSize(ocbuf) -
		LENOF( mpexLargeRequestReq);
    return;
}

/*+
 * NAME:
 *    PEXSetTransientOCBufferType
 *
 * FORMAT:
 *      void PEXSetTransientOCBufferType (display, ocbuf, type, target)
 *      Display         *display;
 *      pxlTransientOCBuf        *ocbuf;
 *      int             type;
 *      XID             target;
 *
 * ARGUMENTS:
 *      display         A pointer to a display structure
 *                      returned by a successful `XOpenDisplay" call.
 *
 *      ocbuf           The OC buffer whose attributes are
 *                      to be modified.
 *
 *      type            The type of OC buffer.
 *
 *      target          The resource identifier of the renderer or structure
 *                      used as a target for the output commands
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Change the display, target and type associated with the oc buffer.
 *      This routine will flush the current contents of `ocbuf'.
 *
 * ERRORS:
 *      None
 */

INTERNAL void
PEXSetTransientOCBufferType (display, ocbuf, type, target)
INPUT Display		*display;
INPUT pxlTransientOCBuf	*ocbuf;
INPUT int		type;
INPUT XID		target;

{
/* AUTHOR: M.P. Womack */

    PEXFlushTransientOCBuffer( ocbuf);

    ocbuf->header.display = display;
    ocbuf->header.initLength = MIN( ocbuf->header.initLengthRequested, 
		display->max_request_size);
    ocbuf->header.target = target;
    ocbuf->header.pexOpcode = type;

    /* the maximum amount of data which can be sent per large request packet */
    ocbuf->header.lrBufferLength = PEXGetMaxTransientOCChunkSize(ocbuf) -
		LENOF( mpexLargeRequestReq);

    /*
     * on a non shared memory display it may be necessary to split
     * ocs across x transport buffer boundaries (when the oc is larger than
     * the x transport buffer by smaller than the max_request_size)..
     */

    return;
}

/*+
 * NAME:
 *    PEXSetRetainedOCBufferTarget
 *
 * FORMAT:
 *      void PEXSetRetainedOCBufferTarget (ocbuf, target)
 *      pxlOCBuf        *ocbuf;
 *      XID             target;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer whose attributes are
 *                      to be modified.
 *
 *      target          The resource identifier of the renderer or structure
 *                      used as a target for the output commands
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Change the target associated with the retained oc buffer.  This
 *      routine will not flush the current contents of the oc buffer.
 *	Instead it will modify the target of all the requests queued up
 *	in the oc buffer.
 *
 * ERRORS:
 *      None
 */

INTERNAL void
PEXSetRetainedOCBufferTarget (ocbuf, target)

INPUT pxlRetainedOCBuf	*ocbuf;
INPUT XID		target;

{
/* AUTHOR: M.P. Womack */
    pxlOCChunk		*pchunk;
    pexOCRequestHeader 	*pReq;

    pchunk = ocbuf->chunkList;
    while (pchunk)
    {
	pReq = (pexOCRequestHeader *)&pchunk[1];
	if ( pReq->pexOpcode == MPEX_LargeRequest )
	{
	    pReq = (pexOCRequestHeader *) ( ((char *)pReq) + 
			sizeof( mpexLargeRequestReq) );
	}
	pReq->target = target;
	pchunk = pchunk->pnext;
    }

    ocbuf->header.target = target;

    return;
}

/*+
 * NAME:
 *    PEXSetTransientOCBufferTarget
 *
 * FORMAT:
 *      void PEXSetTransientOCBufferTarget (ocbuf, target)
 *      pxlOCBuf        *ocbuf;
 *      XID             target;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer whose attributes are
 *                      to be modified.
 *
 *      target          The resource identifier of the renderer or structure
 *                      used as a target for the output commands
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Change the target associated with the transient oc buffer.
 *      This routine will flush the current contents of `ocbuf'.
 *	Therefore this will not affect ocs which have already been sent 
 * 	to the oc buffer.
 *
 * ERRORS:
 *      None
 */

INTERNAL void
PEXSetTransientOCBufferTarget (ocbuf, target)

INPUT pxlTransientOCBuf	*ocbuf;
INPUT XID		target;

{
/* AUTHOR: M.P. Womack */

    PEXFlushTransientOCBuffer( ocbuf);

    ocbuf->header.target = target;

    return;
}

/*+
 * NAME:
 *    PEXGetDefaultOCBufferType
 *
 * FORMAT:
 * 	`PEXGetDefaultOCBufferType`
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 *	displayReturn 	Returns a pointer to the display structure used
 *			when the OC buffer is flushed or sent.
 *
 *	typeReturn	Returns the type of the OC buffer.
 *
 *	targetReturn 	Returns the resource identifier of the OC buffer target
 *			renderer or structure
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine returns attributes of `ocbuf' that can be altered.
 *
 * ERRORS:
 *	None
 */

void
PEXGetDefaultOCBufferType (ocbuf, displayReturn, typeReturn, targetReturn)

INPUT pxlOCBuf		*ocbuf;
OUTPUT Display		**displayReturn;
OUTPUT int		*typeReturn;
OUTPUT XID		*targetReturn;

{
/* AUTHOR: M.P. Womack */
    *displayReturn = ocbuf->display;
    *typeReturn = ocbuf->pexOpcode;
    *targetReturn = ocbuf->target;

    return;
}

/*+
 * NAME:
 *    PEXGetRetainedOCBufferFreeSpace
 *
 * FORMAT:
 *	int PEXGetRetainedOCBufferFreeSpace (ocbuf)
 *	pxlOCBuf		*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 * RETURNS:
 *	See description.
 *
 * DESCRIPTION:
 *	This routine returns the free space (in words) remaining in the output
 *	command buffer pointed to by `ocbuf'.
 *
 * ERRORS:
 *	None
 */

INTERNAL int
PEXGetRetainedOCBufferFreeSpace (ocbuf)
INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */
    pexRequestHeader *currentReq = (pexRequestHeader *)ocbuf->header.basePtr;

    if (currentReq == NULL) return( ocbuf->header.initLength);

    return( ocbuf->currentChunk->size - currentReq->reqLength);
}

/*+
 * NAME:
 *    PEXGetTransientOCBufferFreeSpac
 *
 * FORMAT:
 *	int PEXGetTransientOCBufferFreeSpac (ocbuf)
 *	pxlOCBuf		*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 * RETURNS:
 *	See description.
 *
 * DESCRIPTION:
 *	This routine returns the free space (in words) remaining in the output
 *	command buffer pointed to by `ocbuf'.
 *
 * ERRORS:
 *	None
 */

INTERNAL int
PEXGetTransientOCBufferFreeSpac (ocbuf)
INPUT pxlTransientOCBuf		*ocbuf;

{
/* AUTHOR: M.P. Womack */
    return( WordsLeftInXBuffer( ocbuf->header.display) );
}

/*+
 * NAME:
 *    PEXGetMaxRetainedOCChunkSize
 *
 * FORMAT:
 *	int PEXGetMaxRetainedOCChunkSize (ocbuf)
 *	pxlRetainedOCBuf		*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 * RETURNS:
 *	See description.
 *
 * DESCRIPTION:
 *	This routine returns the maximum number of words which can be
 *      grabbed via PEXGetOCWords.
 *
 * ERRORS:
 *	None
 */

INTERNAL int
PEXGetMaxRetainedOCChunkSize (ocbuf)
INPUT pxlRetainedOCBuf		*ocbuf;

{
/* AUTHOR: M.P. Womack */
    return( ocbuf->header.display->max_request_size );
}

/*+
 * NAME:
 *    PEXGetMaxTransientOCChunkSize
 *
 * FORMAT:
 *	int PEXGetMaxTransientOCChunkSize (ocbuf)
 *	pxlTransientOCBuf		*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 * RETURNS:
 *	See description.
 *
 * DESCRIPTION:
 *	This routine returns the maximum number of words which can be
 *      grabbed via PEXGetOCWords.
 *
 * ERRORS:
 *	None
 */

INTERNAL int
PEXGetMaxTransientOCChunkSize (ocbuf)
INPUT pxlTransientOCBuf		*ocbuf;

{
/* AUTHOR: M.P. Womack */
    return( MaxXBufferLength( ocbuf->header.display) );
}

/*+
 * NAME:
 *    PEXFlushRetainedOCBuffer
 *
 * FORMAT:
 *	void PEXFlushRetainedOCBuffer( ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be flushed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *    	This routine sends the current contents of the OC buffer to the
 *	Xlib connection queue and frees all the requests which were queued 
 *	up in the oc buffer.  Thus the oc buffer contains no output commands 
 *	after this routine is called.
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXFlushRetainedOCBuffer( ocbuf)
INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */

   /*
    * Send the oc requests to the server 
    */
    PEXSendRetainedOCBuffer( ocbuf);

   /*
    * Delete the ocs from the oc buffer 
    */
    PEXClearRetainedOCBuffer (ocbuf);

    return;
}

/*+
 * NAME:
 *    PEXFlushTransientOCBuffer
 *
 * FORMAT:
 *	void PEXFlushTransientOCBuffer( ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be flushed.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *      Clear the oc buffer so that subsequent oc commands will generate a 
 *	new `PEXRenderOutputCommands" or `PEXStoreElements" request.	
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXFlushTransientOCBuffer( ocbuf)
INPUT pxlTransientOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */
    Display	*display = ocbuf->header.display;

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverTransientOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

   /*
    * Sync end of last oc request 
    */
    PEXSyncHandle();

   /*
    * clear oc buffer -- no ocs or large requests in progress
    */
    ocbuf->header.basePtr = NULL;
    ocbuf->header.ocWordsLeft = 0;
    ocbuf->reqNumber = -1;

    return;
}

/*+
 * NAME:
 *    PEXSendRetainedOCBuffer
 *
 * FORMAT:
 *	void PEXSendRetainedOCBuffer (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *    	This routine sends the current contents of the OC buffer to the
 *	Xlib connection queue.  The oc buffer is not emptied and can be
 *	sent again.
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXSendRetainedOCBuffer( ocbuf)
INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR: M.P. Womack */
    pxlOCChunk		*pchunk = ocbuf->chunkList;
    Display		*display = ocbuf->header.display;

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverRetainedOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif


    /* lock around the critical section, for multithreading */
    LockDisplay (display);

    while (pchunk)
    {
	pexRequestHeader *nextReq = (pexRequestHeader *)&pchunk[1];

	if (nextReq->reqLength > LENOF (pexOCRequestHeader) )
	{
	    display->request++;
	    _SendToX( display, (char *)nextReq, NUMBYTES(nextReq->reqLength));
	}

	pchunk = pchunk->pnext;
    }

    /* done, so unlock and synchronize if necessary */
    UnlockDisplay (display);
    PEXSyncHandle ();

    return;
}

/*+
 * NAME:
 *    PEXNoop
 *
 * FORMAT:
 *	void PEXNoop (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *      Noop.	
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXNoop( ocbuf)
INPUT pxlOCBuf	*ocbuf;

{
    return;
}

/*+
 * NAME:
 *    allocNewChunk
 *
 * FORMAT:
 *	pxlOCChunk *allocNewChunk (ocbuf, dataSize)
 *	pxlOCBuf	*ocbuf;
 *	int		dataSize;
 *
 * ARGUMENTS:
 *	ocbuf		The retained OC buffer to allocate a chunk for.
 *	dataSize	The size of the chunk to allocate (in words). 
 *
 * RETURNS:
 *   	A pointer to the new chunk or NULL.	
 *
 * DESCRIPTION:
 *	Allocate a new chunk for the retained oc buffer and add it to the
 *	linked list of chunks.   Calls the error routine associated with 
 *	the oc buffer if the chunk cannot be allocated.
 *
 * ERRORS:
 * 	None	
 */

INTERNAL pxlOCChunk *allocNewChunk(ocbuf, dataSize)
pxlRetainedOCBuf	*ocbuf;
int	 		dataSize;
{
/* AUTHOR:  M.P. Womack */
    pxlOCChunk *newChunk;

    newChunk = 
	(pxlOCChunk *)PEXAllocBuf( sizeof(pxlOCChunk) + NUMBYTES(dataSize));
    if (newChunk)
    { 
       (newChunk)->size = dataSize;
       (newChunk)->pprev = ocbuf->currentChunk;
       (newChunk)->pnext = NULL;
       if (ocbuf->chunkList == NULL)
	   ocbuf->chunkList = newChunk;
       else
	   ocbuf->currentChunk->pnext = newChunk;
       ocbuf->currentChunk = newChunk;
       return( newChunk);
    }
    else
    {
	(*ocbuf->header.OCError)(ocbuf, sizeof(pxlOCChunk)+ NUMBYTES(dataSize));
        return( NULL);
    }

}

/*+
 * NAME:
 *    PEXRetainedStartOC
 *
 * FORMAT:
 *	int PEXRetainedStartOC (ocbuf, ocLength)
 *	pxlOCBuf	*ocbuf;
 *	int		ocLength;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *	ocLength	The length of oc in words
 *
 * RETURNS:
 *  	Returns Success if the oc was successfully started
 *
 * DESCRIPTION:
 *      If there is a `PEXRenderOutputCommands" or `PEXStoreElements" request
 *	in progress for this oc buffer and there room in the request for this 
 *	oc then update the oc request header (reqLength and numCommands) to 
 *	include the new oc.  Otherwise allocate a new chunk and start a new 
 *	request to store this oc.
 *
 * ERRORS:
 * 	None	
 */

INTERNAL int 
PEXRetainedStartOC (ocbuf, ocLength)

INPUT pxlRetainedOCBuf	*ocbuf;
INPUT int		ocLength;

{
/* AUTHOR:  M.P. Womack */
    pexOCRequestHeader *currentReq = 
		(pexOCRequestHeader *)ocbuf->header.basePtr;

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverRetainedOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * If there is a request in progress and this oc will fit in the
     * current request then just add this oc to the end of the current
     * request.  Otherwise allocate a new chunk and start off a new
     * oc request.
     */
    if ( currentReq == NULL || 
         (ocLength + currentReq->reqLength > ocbuf->currentChunk->size) )
    { 
	pxlOCChunk 	*newChunk;
	int	 	dataSize;

	dataSize = MAX( ocbuf->header.initLength, 
		 ocLength + LENOF( pexOCRequestHeader));
	newChunk = allocNewChunk( ocbuf, dataSize);
	if (newChunk == NULL) return( !Success);
	ocbuf->header.basePtr = ocbuf->header.curPtr =
		(unsigned int *)&newChunk[1];	
        ocbuf->header.bufMax = ocbuf->header.basePtr + dataSize;
	STORE_OC_REQUEST_HEADER( &ocbuf->header, ocbuf->header.curPtr,
		ocLength, 1/*# cmds*/);
    }
    else
    {
	currentReq->reqLength += ocLength;
	currentReq->numCommands++;
    }

    return( Success);
}

/*+
 * NAME:
 *    PEXTransientStartOC
 *
 * FORMAT:
 *	int PEXTransientStartOC (ocbuf, ocLength)
 *	pxlOCBuf	*ocbuf;
 *	int		ocLength;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *	ocLength	The length of oc in words
 *
 * RETURNS:
 *  	Zero if the oc was successfully started, non-zero otherwise.
 *
 * DESCRIPTION:
 *      If there is a `PEXRenderOutputCommands" or `PEXStoreElements" request
 *	in progress for this oc buffer and there room in the xlib connection 
 *	queue for this oc then update the oc request header (reqLength and 
 *	numCommands) to include the new oc.  Otherwise start a new request to 
 *	store this oc.
 *
 *	Note that starting a new request may cause the xlib connection
 *	queue to be flushed.  This routine locks the display before writing
 *	any data for the oc into the xlib connection queue.  The display will 
 *	be unlocked when `PEXTransientFinishOC" is called.  Since the display
 *	is locked while writing the oc into the x transport buffer we do not
 *	need to keep the bufptr associated with the display up to date.. it
 *	is updated when the oc is completed. 
 *
 * ERRORS:
 * 	None	
 */

INTERNAL int 
PEXTransientStartOC (ocbuf, ocLength)

INPUT pxlTransientOCBuf	*ocbuf;
INPUT int		ocLength;

{
/* AUTHOR:  M.P. Womack */
    pexOCRequestHeader *currentReq;
    Display *display = ocbuf->header.display;

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverTransientOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * If initLength is zero then we are in sync mode. 
     */ 
    if ( ocbuf->header.initLength == 0 ) PEXFlushTransientOCBuffer( ocbuf);

    /*
     * Get pointer to current request
     */
    currentReq = (XBufferFlushed( display)) ? 
			(NULL) : 
			((pexOCRequestHeader *)ocbuf->header.basePtr);

   /*
    * If the last request in the xlib connection queue is an oc request for
    * this oc buffer and there is room in the connection queue then just add 
    * this oc to the end of the last request.  Otherwise start a new request.
    */
    LockDisplay( display);
    if ( currentReq == NULL || 
	 ocbuf->reqNumber != display->request ||
	 (NUMWORDS(sizeof(pexOCRequestHeader)) + ocLength > WordsLeftInXBuffer(display)) )
    { 
	PEXGetOCReq( display, 
		sizeof( pexOCRequestHeader) + NUMBYTES( ocLength), 
		ocbuf->header.basePtr);
	ocbuf->header.curPtr = ocbuf->header.basePtr;
  	ocbuf->header.bufMax = (unsigned int *) ocbuf->header.display->bufmax;
	ocbuf->reqNumber = display->request;
	STORE_OC_REQUEST_HEADER( &ocbuf->header, ocbuf->header.curPtr, 
		ocLength, 1/*# cmds*/);
    }
    else
    {
	currentReq->reqLength += ocLength;
	currentReq->numCommands++;
    }

    return( Success);
}

/*+
 * NAME:
 *    PEXRetainedLargeRequest
 *
 * FORMAT:
 *	int PEXRetainedStartLargeRequest (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *
 * RETURNS:
 *   	The number of words left in the large request packet	
 *
 * DESCRIPTION:
 *	Allocate a new chunk for the large request and initialize the header
 *	of the large request.  Since it is not known at first what the size 
 *	of the large request will be the length of the large request must 
 *	be updated before it is sent. 
 *
 * ERRORS:
 * 	None	
 */

INTERNAL int 
PEXRetainedStartLargeRequest (ocbuf)

INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR:  M.P. Womack */
    pxlOCChunk 	*newChunk;

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverRetainedOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * number of next large request packet to be sent
     */
    ocbuf->header.lrSequenceNum++;

    /* 
     * Update the request length for the current large request before 
     * starting a new one.
     */
    if (ocbuf->header.lrSequenceNum > 1) 
    {
 	((pexRequestHeader *)(ocbuf->header.basePtr))->reqLength =  
		(int) (ocbuf->header.curPtr - ocbuf->header.basePtr);
    }

    /* 
     * now start the large request 
     */
    newChunk = allocNewChunk( ocbuf,  ocbuf->header.display->max_request_size);
    if (newChunk == NULL) 
    {
        ocbuf->header.lrSequenceNum = 0;
	return( 0);
    }
    ocbuf->header.basePtr = ocbuf->header.curPtr = 
		(unsigned int *) &newChunk[1];	
    ocbuf->header.bufMax = 
		ocbuf->header.basePtr + ocbuf->header.display->max_request_size;

    STORE_LARGE_REQUEST_HEADER( &ocbuf->header, ocbuf->header.curPtr, 
		ocbuf->header.lrSequenceNum,
		ocbuf->header.lrTotalNum, ocbuf->header.lrTotalLength);

    return( ocbuf->header.lrBufferLength);
}

/*+
 * NAME:
 *    PEXTransientStartLargeRequest
 *
 * FORMAT:
 *	int PEXTransientStartLargeRequest (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *
 * RETURNS:
 *   	The number of words left in the large request packet	
 *
 * DESCRIPTION:
 *	Allocate space for the large request in the xlib connection queue and 
 *	initialize the header of the large request.  If this is the first
 *	large request in the sequence then lock the display.  The display 
 *	will be unlocked when all data for the current output command has 
 *	been received and `PEXFinishOC" is called. 
 *
 *	Since it is not known at first what the size of the large request 
 *	will be the length of the large request cannot be determined until 
 *	it is ready to be sent. 
 *
 *	Since the display is locked while writing the oc into the x transport 
 *	buffer we do not need to keep the bufptr associated with the display 
 *	up to date.. however it must be updated before starting a new large 
 *	request (since the large request will cause the x transport buffer 
 *	to flush)
 *
 * ERRORS:
 * 	None	
 */

INTERNAL int 
PEXTransientStartLargeRequest (ocbuf)

INPUT pxlTransientOCBuf	*ocbuf;

{
/* AUTHOR:  M.P. Womack */

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverTransientOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * number of next large request packet to be sent
     */
    ocbuf->header.lrSequenceNum++;

    /*
     * If this is the first large request in the sequence then lock the
     * display until all the data for the oc has been received.  Otherwise
     * update the request length for the current large request before
     * starting the next one.  Also need to update the display's bufptr
     * since it is not updated while writing the oc into the transport 
     * buffer (only curPtr is updated between InitOC and FinishOC) 
     */ 
    if (ocbuf->header.lrSequenceNum == 1) 
    {
	LockDisplay( ocbuf->header.display);
    }
    else
    {
        ocbuf->header.display->bufptr = (char *) ocbuf->header.curPtr;
 	((pexRequestHeader *)(ocbuf->header.basePtr))->reqLength = 
			(int) (ocbuf->header.curPtr - ocbuf->header.basePtr);
    }

    /* 
     * now start the large request 
     */
    PEXGetOCReq(ocbuf->header.display, 
	    	NUMBYTES( ocbuf->header.lrBufferLength) + 
			sizeof( mpexLargeRequestReq), 
	    	ocbuf->header.basePtr);
    ocbuf->header.curPtr = ocbuf->header.basePtr;
    ocbuf->header.bufMax = (unsigned int *) ocbuf->header.display->bufmax;
    STORE_LARGE_REQUEST_HEADER( &ocbuf->header, ocbuf->header.curPtr, 
		ocbuf->header.lrSequenceNum,
		ocbuf->header.lrTotalNum, ocbuf->header.lrTotalLength);

    return( ocbuf->header.lrBufferLength);
}

/*+
 * NAME:
 *    PEXRetainedFinishOC
 *
 * FORMAT:
 *	void PEXRetainedFinishOC (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *
 * RETURNS:
 *   	None	
 *
 * DESCRIPTION:
 *      If a large request was generated then clear the large request flag
 *	to indicate that no large request is in progress and clear the
 *	ocbuf request pointer so a new request will be started.
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXRetainedFinishOC (ocbuf)

INPUT pxlRetainedOCBuf	*ocbuf;

{
/* AUTHOR:  M.P. Womack */

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverRetainedOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * If a large request was generated then clear large request flag and set
     * basePtr to NULL so a new request will be started
     */
    if  ( ocbuf->header.lrSequenceNum ) 
    {
        /*
	 * Update the length of the last large request packet
	 */
 	((pexRequestHeader *)(ocbuf->header.basePtr))->reqLength =  
		(int) (ocbuf->header.curPtr - ocbuf->header.basePtr);

        /* 
         * Make sure the next oc is put in a new chunk 
         */
        ocbuf->header.lrSequenceNum = 0;
        ocbuf->header.basePtr = NULL;
    }
}

/*+
 * NAME:
 *    PEXTransientFinishOC
 *
 * FORMAT:
 *	void PEXTransientFinishOC (ocbuf)
 *	pxlOCBuf	*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *
 * RETURNS:
 *  	None	
 *
 * DESCRIPTION:
 *	Unlock the display.  If a large request was generated then clear the 
 *	large request flag to indicate that no large request is in progress
 *	and flush the oc buffer (so a new request will be started).
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXTransientFinishOC (ocbuf)

INPUT pxlTransientOCBuf	*ocbuf;

{
/* AUTHOR:  M.P. Womack */

#ifdef DEBUG
    if (ocbuf->header.ocWordsLeft)
    {
        PEXRecoverTransientOCRequest(ocbuf);
#     ifdef PRINT_ERRORS
        printf("FATAL error: Missing PEXFinishOCBuffer\n");
#     endif
    }
#endif

    /* 
     * Only curPtr is bumped by PEXCopyWords, PEXCopyBytes and
     * and PEXGetOCWords so need to update the bufptr in the display 
     */
    ocbuf->header.display->bufptr = (char *) ocbuf->header.curPtr;

    if ( ocbuf->header.lrSequenceNum )
    {
        /*
	 * Update the length of the last large request packet
	 */
	((pexRequestHeader *)(ocbuf->header.basePtr))->reqLength = 
		(int) (ocbuf->header.curPtr - ocbuf->header.basePtr);

        UnlockDisplay(ocbuf->header.display);

        /* 
         * flush the oc buffer so the next oc will start a new request
         */
	PEXFlushTransientOCBuffer( ocbuf);
	ocbuf->header.lrSequenceNum = 0;
    }

    else
         UnlockDisplay(ocbuf->header.display);

}

/*+
 * NAME:
 *    PEXGetWordsError
 *
 * FORMAT:
 *	`PEXGetWordsError`
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *      nWords		The number of words that caused overflow
 *
 * RETURNS:
 *   	None	
 *
 * DESCRIPTION:
 *	This routine should never be called .. if it is then that means
 *	there is an implementation error. 
 *
 * ERRORS:
 * 	None	
 */

unsigned int *
PEXGetWordsError (ocbuf, nWords)

INPUT pxlOCBuf	*ocbuf;
INPUT int	nWords;

{
/* AUTHOR:  M.P. Womack */
#ifdef PRINT_ERRORS
    printf("FATAL error PEXGetOCWords: ocbuf %X overflowed\n", ocbuf);
#endif
    return(NULL);
}

/*+
 * NAME:
 *    PEXTransientGetOCWords
 *
 * FORMAT:
 *	unsigned int *PEXTransientGetOCWords (ocbuf, nWords)
 *	pxlOCBuf	*ocbuf;
 *	int		nWords;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *      nWords		The number of words requested.
 *
 * RETURNS:
 *   	None	
 *
 * DESCRIPTION:
 *      Called when PEXGetOCWords overflows.  This happens when using
 * 	transient oc buffers without shared memory transport.  The transport
 *	buffer may be smaller than max_request_size requiring the oc to 
 *	be split up.  This routine just updates the bufptr associated with
 *	the display (since it is not kept up to date while writing the oc)
 *	and then flushes the x transport buffer. 
 *
 * ERRORS:
 * 	None	
 */

INTERNAL unsigned int *
PEXTransientGetOCWords (ocbuf, nWords)

INPUT pxlTransientOCBuf	*ocbuf;
INPUT int		nWords;

{
/* AUTHOR:  M.P. Womack */
    if ( nWords > MaxXBufferLength(ocbuf->header.display) ) return (NULL);

    /* 
     * Need to update bufptr since only curPtr is updated between InitOC and
     * FinishOC calls 
     */
    ocbuf->header.display->bufptr = (char *) ocbuf->header.curPtr;

    /*
     * Now flush the X transport buffer so we can get space to write nWords
     */
    _XFlush(ocbuf->header.display);
    ocbuf->header.curPtr = (unsigned int *) ocbuf->header.display->bufptr;

    return( ocbuf->header.curPtr);
}

/*+
 * NAME:
 *    PEXCopyBytesError
 *
 * FORMAT:
 *	`PEXCopyBytesError`
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *      nBytes		The number of bytes that caused overflow
 *      buffer		The buffer to copy
 *
 * RETURNS:
 *   	None	
 *
 * DESCRIPTION:
 *	This routine should never be called .. if it is then that means
 *	there is an implementation error. 
 *
 * ERRORS:
 * 	None	
 */

void
PEXCopyBytesError (ocbuf, nBytes, buffer)

INPUT pxlOCBuf	*ocbuf;
INPUT int	nBytes;
INPUT char 	*buffer;

{
/* AUTHOR:  M.P. Womack */
#ifdef PRINT_ERRORS
    printf("FATAL error in PEXCopyOCBytes: ocbuf %X overflowed\n", ocbuf);
#endif
    return;
}

/*+
 * NAME:
 *    PEXTransientCopyOCBytes
 *
 * FORMAT:
 *	void PEXTransientCopyOCBytes (ocbuf, nBytes, buffer)
 *	pxlOCBuf	*ocbuf;
 *	int		nBytes;
 *	char		*buffer;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be sent.
 *      nBytes		The number of bytes to send to X
 *      buffer		Buffer to send to X
 *
 * RETURNS:
 *   	None	
 *
 * DESCRIPTION:
 *      Called when PEXCopyOCWords or PEXCopyOCBytes overflows.  This happens 
 *	when using transient oc buffers without shared memory transport.  The 
 *	transport buffer may be smaller than max_request_size requiring the 
 *	oc to be split up.  This routine just updates the bufptr associated 
 *	with the display (since it is not kept up to date while writing the oc)
 *	and then calls _XSend to do the buffering. 
 *
 * ERRORS:
 *
 * ERRORS:
 * 	None	
 */

INTERNAL void
PEXTransientCopyOCBytes (ocbuf, nBytes, buffer)

INPUT pxlTransientOCBuf	*ocbuf;
INPUT int		nBytes;
INPUT char 		*buffer;

{
/* AUTHOR:  M.P. Womack */
    /* 
     * Need to update bufptr since only curPtr is updated when writing 
     * directly to the X transport buffer between InitOC and FinishOC calls 
     */
    ocbuf->header.display->bufptr = (char *) ocbuf->header.curPtr;

    /*
     * Now send the buffer to X .. XSend will take care of splitting the
     * buffer into chunks small enough to fit in the transport buffer 
     * The display's bufptr will point to the next word to be written in the
     * transport buffer  
     */
    _XSend( ocbuf->header.display, buffer, nBytes);
    ocbuf->header.curPtr = (unsigned int *) ocbuf->header.display->bufptr;

    return;
}

/*+
 * NAME:
 *	PEXInitOC 
 *
 * FORMAT:
 *      `PEXInitOC`
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to store the oc in
 *      ocType          The type of oc (PEXMarker,PEXColour,...)
 *      ocHeaderLength  The number of words in the oc header structure 
 *      ocDataLength    The number of words of data for the oc
 *	pReq		Contains pointer to oc header
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Start an oc. `PEXCopyBytesToOC" or `PEXCopyWordsToOC" may be called
 *      between `PEXInitOC" and `PEXFinishOC" pairs to copy oc data.   The
 *      oc data may be written directly into the oc request by calling
 *      `PEXGetOCWords" to get a pointer to a chunk of words in the oc.
 *      `PEXFinishOC" must be called after all the data has been sent for
 *      the oc.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXGetOCWords",`PEXCopyWordsToOC",`PEXCopyBytesToOC"
 *      `PEXFinishOC",`PEXSendOCBuffer",`PEXFlushOCBuffer"
 *
 */

void
PEXInitOC(ocbuf, ocType, ocHeaderLength, ocDataLength, pReq)

INPUT pxlOCBuf        *ocbuf;
INPUT int             ocType;
INPUT int	      ocHeaderLength;
INPUT int             ocDataLength;
INPUT char	      **pReq;

{
/* AUTHOR:  M.P. Womack */
    int ocLength = ocHeaderLength + ocDataLength;

    *pReq = (char *) NULL;
    /*
     * If the oc is larger that the maximum allowed size then generate an error 
     */
    if ( OCIsTooLarge( ocLength) )
    {
        (*ocbuf->OCError)(ocbuf, ocLength);
    }

    /*
     * If the oc is larger than the x transport buffer then start a large req
     */
    else if ( IsLargeRequest( ocbuf->display, ocLength) )
    {
        ocbuf->lrTotalLength = ocLength + LENOF( pexOCRequestHeader);
        ocbuf->lrBufferLeft = ocDataLength;
	ocbuf->lrTotalNum = 0; /* bogus field, not used */ 

        if ( (ocbuf->ocWordsLeft = (*ocbuf->StartLargeRequest)( ocbuf)) != 0 )
	{
            STORE_OC_REQUEST_HEADER( ocbuf, ocbuf->curPtr, ocLength, 1);
            STORE_ELEMENT_INFO( ocbuf->curPtr, ocType, ocLength);
	    ocbuf->ocWordsLeft -= LENOF( pexOCRequestHeader) + ocHeaderLength;
    	    *pReq = (char *) ocbuf->curPtr;
    	    ocbuf->curPtr += ocHeaderLength;
	}
    }

    /*
     * Otherwise just add the oc to a ROC or StoreElements request
     */
    else
    {
        if ( (*ocbuf->StartOC)( ocbuf, ocLength) == Success )
  	{
            ocbuf->ocWordsLeft = ocDataLength;
            STORE_ELEMENT_INFO( ocbuf->curPtr, ocType, ocLength);
    	    *pReq = (char *) ocbuf->curPtr;
    	    ocbuf->curPtr += ocHeaderLength;
	}
    }

}

/*+
 * NAME:
 *      PEXCopyBytesToOC
 *
 * FORMAT:
 *      `PEXCopyBytesToOC`
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to copy data to.
 *      nBytes          The number of bytes to copy
 *      OCData          A pointer to the data to be copied
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Copies 'nBytes' of data to output command which is currently in
 *      progress.  'PEXInitOC" must be called prior to this.  The registered
 *      error function will be called if a memory alloc error occurs or
 *      if an attempt is made to write more words than were reserved
 *      when `PEXInitOC" was called.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXInitOC", `PEXFinishOC", `PEXCopyWordsToOC", `PEXGetOCWords"
 */

void
PEXCopyBytesToOC(ocbuf, nBytes, OCData) 

INPUT pxlOCBuf        *ocbuf;
INPUT int		nBytes;
INPUT char		*OCData;

{
/* AUTHOR:  M.P. Womack */
    if ( ocbuf->ocWordsLeft >= NUMWORDS(nBytes) )
    {
 	if ( NUMWORDS(nBytes) <= (ocbuf->bufMax - ocbuf->curPtr) )
	{
	    COPY_AREA( OCData, ocbuf->curPtr, nBytes);
	    ocbuf->curPtr += NUMWORDS(nBytes);
  	}
    	else
	    (*ocbuf->CopyOCBytes)(ocbuf, nBytes, OCData);
        ocbuf->ocWordsLeft -= NUMWORDS(nBytes);
    }
    else if ( ocbuf->lrSequenceNum && 
		(ocbuf->lrBufferLeft -= NUMWORDS(nBytes)) >= 0 )
    {
	int size;
	int nBytesLeft = nBytes; 
	char *dataPtr = (char *)OCData;
    	
	while ( nBytesLeft ) 
	{
	    if ( ocbuf->ocWordsLeft > 0 || 
	      (ocbuf->ocWordsLeft = (*ocbuf->StartLargeRequest)( ocbuf)) > 0 )
	    { 
		size = MIN(nBytesLeft, NUMBYTES(ocbuf->ocWordsLeft) ); 
		COPY_AREA( dataPtr, ocbuf->curPtr, size);
		dataPtr += size;
		nBytesLeft -= size;
		ocbuf->curPtr += NUMWORDS(size);
        	ocbuf->ocWordsLeft -= NUMWORDS(size);
	    }
	    else 	
		break;
	} 
    }
}

/*+
 * NAME:
 *      PEXCopyWordsToOC
 *
 * FORMAT:
 *      `PEXCopyWordsToOC`
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to copy data to.
 *      nWords          The number of words to copy
 *      OCData          A pointer to the data to be copied
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      Copies 'nWords' of data to output command which is currently in
 *      progress.  'PEXInitOC" must be called prior to this.  The registered
 *      error function will be called if a memory alloc error occurs or
 *      if an attempt is made to write more words than were reserved
 *      when `PEXInitOC" was called.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXInitOC", `PEXFinishOC"
 */

void
PEXCopyWordsToOC(ocbuf, nWords, OCData)

INPUT pxlOCBuf        *ocbuf;
INPUT int		nWords;
INPUT char		*OCData;

{
/* AUTHOR:  M.P. Womack */
    if ( ocbuf->ocWordsLeft >= nWords )
    {
 	if ( nWords <= (ocbuf->bufMax - ocbuf->curPtr) )
	{
	    COPY_AREA( OCData, ocbuf->curPtr, NUMBYTES( nWords));
	    ocbuf->curPtr += nWords;
	}
    	else
	    (*ocbuf->CopyOCBytes)(ocbuf, NUMBYTES( nWords), OCData);
        ocbuf->ocWordsLeft -= nWords;
    }
    else if ( ocbuf->lrSequenceNum && 
		(ocbuf->lrBufferLeft -= nWords) >= 0 )
    {
	int size;
	int nWordsLeft = nWords; 
	int *dataPtr = (int *)OCData;
    	
	while ( nWordsLeft ) 
	{
	    if ( ocbuf->ocWordsLeft > 0 || 
	      (ocbuf->ocWordsLeft = (*ocbuf->StartLargeRequest)( ocbuf)) > 0 )
	    { 
		size = MIN(nWordsLeft, ocbuf->ocWordsLeft ); 
		COPY_AREA( dataPtr, ocbuf->curPtr, NUMBYTES( size));
		dataPtr += size;
		nWordsLeft -= size;
		ocbuf->curPtr += size;
        	ocbuf->ocWordsLeft -= size;
	    }
	    else 	
		break;
	} 
    }
}

#ifdef DEBUG

/*
 * PEXGetOCWords
 *
 *      Return a pointer to 'nWords' of data located in the output command
 *      which is currently in progress.  The pointer is stored in `pBuf`.
 *      'PEXInitOC" must be called prior to this.  The registered
 *      error function will be called if a memory alloc error occurs;
 *      if an attempt is made to grab more words than were reserved
 *      when `PEXInitOC" was called; or if an attempt is made to grab
 *      more words than the maximum connection buffer.
 */

unsigned int *
PEXGetOCWords(ocbuf, nWords)
pxlOCBuf 	*ocbuf;
int	 	nWords;
{
/* AUTHOR:  M.P. Womack */
    unsigned int *pBuf;

    if ( (ocbuf->ocWordsLeft -= nWords) >= 0 )
    {
 	if ( nWords <= (ocbuf->bufMax - ocbuf->curPtr) )
	    pBuf = (unsigned int *) ocbuf->curPtr;
  	else
	    pBuf = (*ocbuf->GetOCWords)(ocbuf, nWords);
	ocbuf->curPtr += nWords;
    }
    else if ( ocbuf->lrSequenceNum && 
     (ocbuf->lrBufferLeft -= nWords) >= 0 &&
     (ocbuf->ocWordsLeft = (*ocbuf->StartLargeRequest)( ocbuf) - nWords) >= 0 )
    {
	pBuf = (unsigned int *) ocbuf->curPtr;
	ocbuf->curPtr += nWords;
    }
    else
	pBuf = (unsigned int *) NULL;

    return( pBuf);
}

#endif /* DEBUG */

#ifdef DEBUG

/*
 * PEXAddOC
 *
 *      This routine adds an output command to `ocbuf'.  The `ocType'
 *      argument indicates the type of output command being passed.
 *      The `PEX.h' include file contains possible values for `octype',
 *      for example, `PEXOCMarkerType' and  `PEXOCNurbCurve'.
 *
 *      `Data' points to the actual data for the output command.
 *
 *      `Size' specifies the length of `OCData' (in bytes).
 *
 *      Client programs should call the individual output
 *      command functions or use the output command macros
 *      defined in `PEXoc.h' or `MPEXoc.h' instead of calling this routine
 *      directly.
 */

void
PEXAddOC (ocbuf, ocType, size, OCData)
 pxlOCBuf        *ocbuf;
 unsigned int    ocType;
 int             size;
 char            *OCData;
{
/* AUTHOR:  M.P. Womack */
    int 	ocLength = (size + Pad(size) + sizeof(pexElementInfo))>>2;

    if ( (*ocbuf->StartOC)( ocbuf, ocLength) == Success )
    {
        STORE_ELEMENT_INFO( ocbuf->curPtr, ocType, ocLength);
        ocbuf->curPtr += LENOF( pexElementInfo);
        if (size > 0) COPY_AREA (OCData, ocbuf->curPtr, size);
        ocbuf->curPtr += NUMWORDS( size);
        (*ocbuf->FinishOC)( ocbuf);
    }
    else
        (*ocbuf->OCError)(ocbuf, ocLength);
}

#endif /* DEBUG */

#ifdef DEBUG

/*
 * PEXAddListOC
 *
 *      This routine adds an output command of type `ocType' to the oc
 *      buffer.  The `PEX.h' include file contains possible values of `ocType'.
 *
 *      The last part of the command is a list of `count' items.  Each item is
 *      `elementSize' bytes long.  The list is padded if necessary.  If
 *      `countNeeded`  is True the list created in `ocbuf' is preceded by a
 *      count.  `Data' points to the data for the list.
 */

void
PEXAddListOC (ocbuf, ocType, countNeeded, count, elementSize, elementList)
INPUT pxlOCBuf          *ocbuf;
INPUT unsigned int      ocType;
INPUT int               countNeeded;
INPUT int               count;
INPUT int               elementSize;
INPUT char              *elementList;

{
/* AUTHOR:  M.P. Womack */
    int         ocListSize;
    char	*pReq;

    /*
     * ocListSize is the length of the list data in bytes
     */
    ocListSize = count * elementSize;

    /*
     * PEXInitOC will generate a large request if necessary
     */
    PEXInitOC( ocbuf, ocType, 
		(countNeeded) ? LENOF( pexOCcListHeader) : 
				LENOF( pexOCListHeader), 
		NUMWORDS( ocListSize), &pReq);
    if (pReq == NULL) return;

    /*
     * If required add the count field to the oc header
     */
    if (countNeeded)
        ((pexOCcListHeader *)pReq)->length = count;

    /*
     * now copy the data to the buffer
     */
    PEXCopyBytesToOC( ocbuf, ocListSize, elementList);

    PEXFinishOC( ocbuf);
}

#endif /* DEBUG */

/*+
 * NAME:
 *    PEXDefaultStoreOCList
 *
 * FORMAT:
 *	void PEXDefaultStoreOCList (ocbuf, numElements, ocList)
 *	pxlOCBuf		*ocbuf;
 *	int			numElements;
 *	pxlElementInfo		*ocList;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to store the ocs in  
 *	numElements     The number of ocs to store
 *	ocList		The list of ocs to store
 *
 * RETURNS:
 *  	None	
 *
 * DESCRIPTION:
 *	Store a list of ocs into the oc buffer.  The list is in the
 *	same format as returned by PEXFetchElList. 
 *
 * ERRORS:
 * 	None	
 */

void
PEXDefaultStoreOCList (ocbuf, numElements, ocList)

INPUT pxlOCBuf		*ocbuf;
INPUT int 		numElements;
INPUT pxlElementInfo 	*ocList;

{
/* AUTHOR:  M.P. Womack */
    int			i;
    int			ocDataLength;
    pxlElementInfo	* pData;
    char		* pReq;

    for (i = numElements, pData = ocList;  i > 0; i--)
    {
	ocDataLength = ((int) pData->length) - LENOF(pxlElementInfo);
	PEXInitOC( ocbuf, pData->elementType, LENOF(pxlElementInfo), 
		ocDataLength, &pReq);
        if (pReq == NULL) break; 
	PEXCopyWordsToOC( ocbuf, ocDataLength, (char *) &pData[1]);
	PEXFinishOC( ocbuf);

        pData = (pxlElementInfo *) (((int *)pData) + ((int) pData->length));
    }
}


/*+
 * NAME:
 *    PEXDefaultTransientOCBuffer
 *
 * FORMAT:
 *      `PEXDefaultTransientOCBuffer`
 *
 * ARGUMENTS:
 *      None 
 *
 * RETURNS:
 *      None 
 *
 * DESCRIPTION:
 *      This routine is a dummy routine to support PEXAllocateOCBuffer.
 *	PEXAllocateOCBuffer should not be used it is only kept for backwards 
 *	compatability.
 *
 * ERRORS:
 *	None
 */

int
PEXDefaultTransientOCBuffer ()

{
    return;
}

/*+
 * NAME:
 *    PEXDefaultRetainedOCBuffer
 *
 * FORMAT:
 *      `PEXDefaultRetainedOCBuffer`
 *
 * ARGUMENTS:
 *      None 
 *
 * RETURNS:
 *      None 
 *
 * DESCRIPTION:
 *      This routine is a dummy routine to support PEXAllocateOCBuffer.
 *	PEXAllocateOCBuffer should not be used it is only kept for backwards 
 *	compatability.
 *
 * ERRORS:
 *	None
 */

int
PEXDefaultRetainedOCBuffer ()

{
    return;
}

/*+
 * NAME:
 *      PEXDefaultOCError
 *
 * FORMAT:
 *      `PEXDefaultOCError`
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer for which there was an overflow error.
 *
 *  	value		The size (in bytes) of the data that would not fit in
 *			`ocbuf'.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      This routine is the default function provided for when output command
 *	buffers overflow and the grow function cannot provide enough room
 *	for the data.
 *	This routine should not be called directly by an
 *	application program.  It should be passed as the `errorFunction'
 *	argument to `PEXAllocateOCBuffer".
 *	See the introduction to this chapter for more information.
 *
 *	This function saves `value'.  (For the default grow functions
 *	`PEXDefaultTransientOCBuffer" and `PEXDefaultRetainedOCBuffer",
 *	`value' is the number of bytes of data the application program
 *	attempted to put in OC buffer.)
 *	Query `value' by calling
 *	`PEXGetOCOverflow".  Reset `value' by calling `PEXClearOCOverflow".
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *      `PEXAllocateOCBuffer", `PEXFlushOCBuffer",  `PEXSetOCOverflow",
 *	`PEXGetOCOverflow",   `PEXClearOCOverflow"
 *
 */

void
PEXDefaultOCError (ocbuf, value)

INPUT pxlOCBuf	*ocbuf;
INPUT int	value;

{
/* AUTHOR:  Sally C. Barry */
    pexOCError = value;

#ifdef PRINT_ERRORS
    printf("ocbuf: %X needed %d\n", ocbuf, value);
#endif

    return;
}

/*+
 * NAME:
 *      PEXSetOCOverflow
 *
 * FORMAT:
 *      `PEXSetOCOverflow`
 *
 * ARGUMENTS:
 *	value		The output command grow error value to be
 *			saved.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *  	This routine saves `value'.  `Value' can be inquired by calling
 *	`PEXGetOCOverflow" and reset by calling `PEXClearOCOverflow".  This
 *	function is used by application programs that write their own output
 *	command buffer grow functions.  For more information, see the
 *	introduction to this chapter.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *      `PEXAllocateOCBuffer", `PEXGetOCOverflow", `PEXClearOCOverflow"
 *
 */

void
PEXSetOCOverflow (value)

INPUT int	value;

{
/* AUTHOR:  Sally C. Barry */
    pexOCError = value;

    return;
}

/*+
 * NAME:
 *      PEXGetOCOverflow
 *
 * FORMAT:
 *      `PEXGetOCOverflow`
 *
 *
 * RETURNS:
 *      The integer value previously saved by an OC buffer
 *	grow function.
 *
 * DESCRIPTION:
 *	This routine queries the current value saved by an OC buffer
 *	grow function which could not expand an OC buffer.
 *	The value can be reset by calling `PEXClearOCOverflow".
 *	Typically, this value is the size (in bytes) of the data the
 *	application program attempted to add to the OC buffer but
 *	would not fit.
 *	For more information, see the introduction to this
 *	chapter.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *      `PEXAllocateOCBuffer", `PEXSetOCOverflow", `PEXClearOCOverflow"
 *
 */

int
PEXGetOCOverflow ()

{
/* AUTHOR:  Sally C. Barry */
    return (pexOCError);
}

/*+
 * NAME:
 *      PEXClearOCOverflow
 *
 * FORMAT:
 *      `PEXClearOCOverflow`
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	This routine clears the OC buffer overflow error value.
 *	This value is
 *	set by OC buffer "error" functions, which are called when
 *	grow functions cannot expand an OC buffer.
 *	See the introduction to this chapter for more information.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *      `PEXAllocateOCBuffer", 	`PEXSetOCOverflow", `PEXGetOCOverflow"
 *
 */

void
PEXClearOCOverflow ()

{
/* AUTHOR:  Sally C. Barry */
    pexOCError = 0;

    return;
}
