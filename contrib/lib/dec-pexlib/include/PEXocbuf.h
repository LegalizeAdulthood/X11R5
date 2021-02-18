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


#ifndef PEXOCBUF_H
#define PEXOCBUF_H

/*
 *   Table of Contents (Routine names in order of appearance)
 */

/*
 * E   STORE_ELEMENT_INFO
 * E   PEXAllocateOCBuffer
 * E   PEXDeallocateOCBuffer
 * E   PEXSetOCBufferType
 * E   PEXSetOCBufferTarget
 * E   PEXGetOCBufferType
 * E   PEXGetOCBufferFreeSpace
 * E   PEXGetMaxOCChunkSize
 * E   PEXFlushOCBuffer
 * E   PEXSendOCBuffer
 * E   INIT_OC
 * E   PEXFinishOC
 * E   PEXStoreOCList
 * E   PEXGetOCWords    except DEBUG
 * E   PEXAddOC         except DEBUG
 * E   PEXAddListOC     except DEBUG
 */

extern int PEXDefaultTransientOCBuffer();
extern int PEXDefaultRetainedOCBuffer();

/*
 * typedefs for referencing fields in oc requests 
 */

typedef struct pexRequestHeader
{
    unsigned char       extOpcode;
    unsigned char       pexOpcode;
    unsigned short      reqLength;
} pexRequestHeader;

typedef struct pexOCRequestHeader
{
    unsigned char       extOpcode;
    unsigned char       pexOpcode;
    unsigned short      reqLength;
    unsigned short      fpFormat;
    unsigned short      pad;
    unsigned long       target;
    unsigned long       numCommands;
} pexOCRequestHeader;

typedef pexElementInfo pexOCListHeader;

typedef struct pexOCcListHeader
{
    pexElementInfo      head;
    unsigned short      length;
    unsigned short      pad;
} pexOCcListHeader;


#define STORE_ELEMENT_INFO(_reqPtr_,_ocType_,_ocLength_) \
{ \
    ((pexElementInfo *)(_reqPtr_))->elementType = (_ocType_); \
    ((pexElementInfo *)(_reqPtr_))->length = (_ocLength_); \
}


/*+
 * NAME:
 *      PEXAllocateOCBuffer
 *
 * FORMAT:
 *      pxlOCBuf *PEXAllocateOCBuffer( display, type, target, ocBufType,
 *                                             errorFunction, initLength)
 *      Display         *display;
 *      int             type;
 *      XID             target;
 *      void            (* errorFunction)();
 *      int             initLength;
 *
 * ARGUMENTS:
 *      display         A pointer to a display structure
 *                      returned by a successful `XOpenDisplay" call.
 *
 *      type            The type of oc buffer (pxlRenderImmediate / pxlAddToStructure)
 *
 *      target          The resource identifier of the renderer or structure
 *                      used as a target for the output commands
 *
 *      ocBufType	This is either `PEXDefaultRetainedOCBuffer" or 
 *			`PEXDefaultTransientOCBuffer" or NULL.  If it is NULL
 *			then `PEXDefaultRetainedOCBuffer" is used.
 *
 *      errorFunction   Called if an error occurs when writing to the OC buffer
 *
 *      initLength      Hint as to how large oc requests should be (in words)
 *
 *
 * RETURNS:
 *      A pointer to the allocated OC buffer or zero if the allocation failed.
 *
 * DESCRIPTION:
 *      This routine is maintained for backwards compatibility only.  
 *	`PEXAllocateRetainedOCBuffer" or `PEXAllocateTransientOCBuffer"
 *	should be called to allocate oc buffers. 
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 */
#define PEXAllocateOCBuffer(_dpy_,_typ_,_targ_,_bufTyp_,_errFn_,_initSiz_)\
( (_bufTyp_ == PEXDefaultTransientOCBuffer) ?\
      (PEXAllocateTransientOCBuffer(_dpy_,_typ_,_targ_,_errFn_,_initSiz_)) :\
      (PEXAllocateRetainedOCBuffer(_dpy_,_typ_,_targ_,_errFn_,_initSiz_)) )

/*+
 * NAME:
 *      PEXDeallocateOCBuffer
 *
 * FORMAT:
 *      void PEXDeallocateOCBuffer( ocbuf)
 *      pxlOCBuf 	*ocbuf;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to be deallocated.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      This routine frees all memory for the oc buffer
 *      specified by `ocbuf'.
 *
 *      NOTE: This is the only proper way to deallocate an OC buffer.
 *      Do not use the `PEXFree" command to deallocate OC buffers.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 */
#define PEXDeallocateOCBuffer(_ocbuf_) (*(_ocbuf_)->FreeOCBuf)(_ocbuf_)


/*+
 * NAME:
 *      PEXClearOCBuffer
 *
 * FORMAT:
 *      void PEXClearOCBuffer( ocbuf)
 *      pxlOCBuf 	*ocbuf;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to be emptied 
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *      This routine empties all ocs in the oc buffer.  If the oc
 *	buffer is retained then all the ocs currently stored in the buffer
 *	will be deleted.  If the oc buffer is transient then the next
 *	oc will start a new request in the transport buffer. 
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 */
#define PEXClearOCBuffer(_ocbuf_) (*(_ocbuf_)->ClearOCBuf)(_ocbuf_)


/*+
 * NAME:
 *      PEXSetOCBufferType
 *
 * FORMAT:
 *      void PEXSetOCBufferType (display, ocbuf, type, target)
 *      Display         *display;
 *      pxlOCBuf        *ocbuf;
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
 *
 *      If `type' is `pxlRenderImmediate', requests that pass output commands to
 *      a renderer are created.
 *      If `type' is `pxlAddToStructure', requests that pass output commands to
 *      a structure are created.
 *      The `PEXlib.h' include file contains definitions of these constants.
 *
 *      `Target' defines the destination for the output commands.
 *      If the buffer type is `pxlRenderImmediate', `target' is the resource
 *      identifier of the renderer to which the output commands will be sent.
 *      If the buffer type is `pxlAddToStructure', `target' is the resource
 *      identifier of the structure to which the output commands will be sent.
 *
 *      This routine will flush the current contents of `ocbuf'.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *      `PEXGetOCBufferType",  `PEXCreateRenderer", `PEXCreateStructure"
 *
 */
#define PEXSetOCBufferType(_display_, _ocbuf_, _type_, _target_) \
	(*(_ocbuf_)->SetOCBufType)(_display_,_ocbuf_,_type_,_target_)

/*+
 * NAME:
 *      PEXSetOCBufferTarget
 *
 * FORMAT:
 *      void PEXSetOCBufferTarget (ocbuf, target)
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
 *      Change the target associated with the oc buffer.
 *
 *      `Target' defines the destination for the output commands.
 *      If the buffer type is `pxlRenderImmediate', `target' is the resource
 *      identifier of the renderer to which the output commands will be sent.
 *      If the buffer type is `pxlAddToStructure', `target' is the resource
 *      identifier of the structure to which the output commands will be sent.
 *
 *      For retained oc buffers this routine will modify the target 
 *      associated with the output commands currently stored in the oc
 *      buffer.  For transient oc buffers this routine will modify the
 *	target of subsequent output commands.  This routine will `not` 
 *	flush the current contents of `ocbuf'.
 *
 * ERRORS:
 *      None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *      `PEXGetOCBufferType",  `PEXCreateRenderer", `PEXCreateStructure"
 *
 */
#define PEXSetOCBufferTarget(_ocbuf_, _target_) \
	(*(_ocbuf_)->SetOCBufTarget)(_ocbuf_,_target_)

/*+
 * NAME:
 * 	PEXGetOCBufferType
 *
 * FORMAT:
 * 	void PEXGetOCBufferType (ocbuf, displayReturn, typeReturn, targetReturn)
 *	pxlOCBuf	*ocbuf;
 *	Display		**displayReturn;
 *	int		*typeReturn;
 *	XID		*targetReturn;
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
 *	`DisplayReturn' returns a pointer to information
 *	about the connection over which the output commands are sent.
 *
 *	`typeReturn' returns the OC buffer type.
 *
 *	`targetReturn' returns the resource identifier of the target resource
 *	for the output commands in `ocbuf'.   The resource identifier is
 *	either a renderer or a structure resource identifier, depending on
 *	the OC buffer type.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *      `PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *	`PEXSetOCBufferType",  `PEXCreateRenderer", `PEXCreateStructure"
 *
 */
#define PEXGetOCBufferType(_ocbuf_,_displayReturn_,_typeReturn_,_targetReturn_)\
  (*(_ocbuf_)->GetOCBufType)(_ocbuf_,_displayReturn_,_typeReturn_,_targetReturn_)

/*+
 * NAME:
 * 	PEXGetOCBufferFreeSpace
 *
 * FORMAT:
 *	int PEXGetOCBufferFreeSpace (ocbuf)
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
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 *
 */
#define PEXGetOCBufferFreeSpace(_ocbuf_) (*(_ocbuf_)->GetOCBufFreeSpace)(_ocbuf_)

/*+
 * NAME:
 * 	PEXGetMaxOCChunkSize
 *
 * FORMAT:
 *	int PEXGetMaxOCChunkSize (ocbuf)
 *	pxlOCBuf		*ocbuf;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to be queried.
 *
 * RETURNS:
 *	See description.
 *
 * DESCRIPTION:
 *      This routine returns the maximum number of words which can be
 *      reserved via PEXGetOCWords.  Note that the size may change if the
 *	display is changed (ie: if `PEXSetOCBufferType" is called)
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 *	`PEXGetOCWords", `PEXSetOCBufferType"
 *
 */
#define PEXGetMaxOCChunkSize(_ocbuf_) (*(_ocbuf_)->GetMaxOCChunkSize)(_ocbuf_)

/*+
 * NAME:
 *      PEXFlushOCBuffer
 *
 * FORMAT:
 *      void PEXFlushOCBuffer( ocbuf)
 *      pxlOCBuf        *ocbuf;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to be flushed.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	If the oc buffer is a retained buffer then `PEXRenderOutputCommands"
 *	or `PEXStoreElements" requests will be copied from the oc buffer to 
 *	the Xlib connection queue.  The oc buffer will then be emptied, 
 *	meaning that the ocs in the buffer will be deleted.  This call has 
 *	no meaning for transient oc buffers. Note that transient oc buffers 
 *	are built directly into the Xlib connection queue and therefore do 
 *	not require a `PEXFlushOCBuffer" or `PEXSendOCBuffer" in order to be 
 *	sent.  They are automatically flushed when an intervening X/PEX 
 *	request is received, when the Xlib connection queue becomes full, or
 *      when the Xlib connection queue is flushed.
 *
 *      The Xlib queue can be flushed by calling the Xlib routine `XFlush".
 *      `Ocbuf' contains no output commands after `PEXFlushOCBuffer" is
 *      called.
 *
 * ERRORS:
 *      `PEXRenderOutputCommands" or `PEXStoreElements" requests may
 *      cause the following errors to occur. 
 *
 *      Renderer                Specified renderer resource identifier is
 *                              invalid.
 *
 *      Structure               Specified structure resource identifier is
 *                              invalid.
 *
 *      FloatingPointFormat     Device does not support the specified floating
 *                              point format.
 *
 *      OutputCommand           A value in an output command is illegal,
 *                              out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *      `PEXSendOCBuffer"
 *
 */
#define PEXFlushOCBuffer(_ocbuf_) (*(_ocbuf_)->FlushOCBuf)(_ocbuf_)

/*+
 * NAME:
 *      PEXSendOCBuffer
 *
 * FORMAT:
 *	void PEXSendOCBuffer (ocbuf)
 *	pxlOCBuf         *ocbuf;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to be sent.
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	If the oc buffer is a retained buffer then `PEXRenderOutputCommands"
 *	or `PEXStoreElements" requests will be copied from the oc buffer to 
 *	the Xlib connection queue.  The oc buffer is not emptied and can be 
 *	sent again.  This call has no meaning for transient oc buffers. 
 *
 * ERRORS:
 *      `PEXRenderOutputCommands" or `PEXStoreElements" requests may
 *      cause the following errors to occur. 
 *
 *      Renderer                Specified renderer resource identifier is
 *                              invalid.
 *
 *      Structure               Specified structure resource identifier is
 *                              invalid.
 *
 *      FloatingPointFormat     Device does not support the specified floating
 *                              point format.
 *
 *      OutputCommand           A value in an output command is illegal,
 *                              out of range, or otherwise inappropriate.
 *
 *                              out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *      `PEXFlushOCBuffer"
 *
 */
#define PEXSendOCBuffer(_ocbuf_) (*(_ocbuf_)->SendOCBuf)(_ocbuf_)

/*+
 * NAME:
 *      INIT_OC
 *
 * DESCRIPTION:
 *	Macro to start an oc. Calls PEXInitOC. 
 */
#define INIT_OC(_ocbuf_,_ocType_,_ocDataLength_,_pReq_) \
    PEXInitOC(_ocbuf_,OC/**/_ocType_,LENOF( pex/**/_ocType_),_ocDataLength_,\
	&(_pReq_))

/*+
 * NAME:
 *      PEXFinishOC
 *
 * FORMAT:
 *	void PEXFinishOC (ocbuf)
 *	pxlOCBuf        *ocbuf;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to which the oc was sent 
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	Called after all the data for the oc has been sent.  
 *	`PEXCopyBytesToOC", `PEXCopyWordsToOC" or `PEXGetOCWords" may be 
 *	called between `PEXInitOC" and `PEXFinishOC" pairs.  Note that
 *	calling `PEXFinishOC" without first calling `PEXInitOC" successfully
 *	can have severe consequences since pexlib does not check for this.
 *
 * ERRORS:
 * 	None	
 *
 * SEE ALSO:
 *      `PEXGetOCWords",`PEXCopyWordsToOC",`PEXCopyBytesToOC"
 *      `PEXInitOC",`PEXSendOCBuffer",`PEXFlushOCBuffer"
 */
#define PEXFinishOC(_ocbuf_) (*(_ocbuf_)->FinishOC)( _ocbuf_)

/*+
 * NAME:
 *      PEXStoreOCList
 *
 * FORMAT:
 *	void PEXStoreOCList (ocbuf, numElements, ocList)
 *	pxlOCBuf        *ocbuf;
 *      int		numElements;
 *      pxlElementInfo  *ocList;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to store the ocs in 
 *	numElements	The number of ocs to store
 *	ocList		list of ocs to store
 *
 * RETURNS:
 *      None
 *
 * DESCRIPTION:
 *	Copies the list of output commands to the specified oc buffer. 
 *	The registered error function will be called if a memory alloc 
 *	error occurs.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *      `PEXSendOCBuffer",`PEXFlushOCBuffer" 
 */
#define PEXStoreOCList(_ocbuf_,_numElements_,_ocList_) 			\
	(*(_ocbuf_)->StoreOCList)( _ocbuf_,_numElements_,_ocList_)

#ifndef DEBUG
/*+
 * NAME:
 *      PEXGetOCWords
 *
 * FORMAT:
 *	unsigned int *PEXGetOCWords (ocbuf, nWords)
 *	pxlOCBuf        *ocbuf;
 *	int		nWords;
 *
 * ARGUMENTS:
 *      ocbuf           The OC buffer to allocate words from.
 *	nWords		The number of words to reserve
 *
 * RETURNS:
 *      Pointer to 'nWords' of data 
 *
 * DESCRIPTION:
 *	Return a pointer to 'nWords' of data located in the output command 
 *	which is currently in progress.  'PEXInitOC" must be called prior 
 *	to this.   NULL will be returned if an attempt is made to grab more 
 *	words than were reserved when `PEXInitOC" was called or if an attempt 
 *	is made to grab more words than specified in 'PEXGetMaxOCChunkSize". 
 *
 *      The code below is implemented as a macro since it will be called
 *	often and it is critical that it performs well.  The basic 
 *	algorithm is 
 *
 *    	    if ( (A && B) || C )
 *        	return ( (curPtr += nWords) - nWords);
 *    	    else
 *	  	return NULL;
 *
 * where A =
 * 	(the words requested dont overflow the request which is being built)
 *
 * B = 
 *  	(there is room in the current Xlib buffer) OR 
 * 	(we can get room in the current Xlib buffer)
 *
 * C =
 *  	(the current request is a large request) AND 
 *  	(the words requested dont overflow the large request) AND 
 *  	(a new large request packet was successfully started )
 *	 
 *
 * ERRORS:
 * 	None	
 *
 * SEE ALSO:
 *      `PEXInitOC", `PEXFinishOC"
 *
 */
#define RoomInThisRequest(_oc_,_nWords_)			\
 ( ((_oc_)->ocWordsLeft -= (_nWords_)) >= 0 )

#define RoomInXBuffer(_oc_,_nWords_) 				\
 ( ((_nWords_) <= ((_oc_)->bufMax - (_oc_)->curPtr)) ||		\
   ((*(_oc_)->GetOCWords)((_oc_), (_nWords_))) )

#define NewLargeRequestPacket(_oc_,_nWords_)			\
 ( ((_oc_)->lrSequenceNum) &&  					\
   (((_oc_)->lrBufferLeft -= (_nWords_)) >= 0) && 		\
   (((_oc_)->ocWordsLeft= (*(_oc_)->StartLargeRequest)(_oc_)-(_nWords_)) >= 0) )

#define PEXGetOCWords(_oc_,_nWords_) 				\
 ( (( RoomInThisRequest(_oc_,_nWords_) && RoomInXBuffer(_oc_,_nWords_) ) || \
    NewLargeRequestPacket(_oc_,_nWords_)) ?			\
	 (((_oc_)->curPtr += (_nWords_)) - (_nWords_)) 		\
  : 								\
         (NULL) )
#endif /* DEBUG not defined */

#ifndef DEBUG
/*+
 * NAME:
 * 	PEXAddOC
 *
 * FORMAT:
 *	void PEXAddOC (ocbuf, ocType, size, data)
 *	pxlOCBuf	*ocbuf;
 *	unsigned int	ocType;
 *	int		size;
 *	char		*data;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to which the output command
 *			is to be added.
 *
 *	ocType		The type of output command to be added
 *			to the OC buffer.
 *
 *	size		The size (in bytes) of the output command.
 *
 *	data		A pointer to the data for the output command.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine adds an output command to `ocbuf'.  The `ocType'
 *	argument indicates the type of output command being passed.
 *	The `PEX.h' include file contains possible values for `octype',
 *	for example, `PEXOCMarkerType' and  `PEXOCNurbCurve'.
 *
 *	`Data' points to the actual data for the output command.
 *
 *	`Size' specifies the length of `data' (in bytes).
 *
 *	Client programs should call the individual output
 *	command functions or use the output command macros
 *	defined in `PEXoc.h' or `MPEXoc.h' instead of calling this routine
 *	directly.
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer"
 *
 */

#define PEXAddOC(_ocbuf_,_ocType_,_size_,_OCData_)		\
{								\
    int _ocLength = ((_size_) + Pad(_size_) + sizeof(pexElementInfo))>>2;\
    if ( (*(_ocbuf_)->StartOC)((_ocbuf_),_ocLength) == Success )\
    {								\
        STORE_ELEMENT_INFO((_ocbuf_)->curPtr, _ocType_, _ocLength);\
        (_ocbuf_)->curPtr += LENOF(pexElementInfo);		\
        if (_size_ > 0) COPY_AREA(_OCData_, (_ocbuf_)->curPtr, _size_);\
        (_ocbuf_)->curPtr += NUMWORDS(_size_);			\
        (*(_ocbuf_)->FinishOC)((_ocbuf_));			\
   }								\
    else							\
        (*(_ocbuf_)->OCError)((_ocbuf_), _ocLength);		\
}
#endif /* DEBUG not defined */

#ifndef DEBUG
/*+ 
 * NAME:
 *	PEXAddListOC
 *
 * FORMAT:
 *	void PEXAddListOC (ocbuf, ocType, countNeeded, count, elementSize, 
 *			   elementList)
 *	pxlOCBuf	*ocbuf;
 *	unsigned int	ocType;
 *	int		countNeeded;
 *	int		count;
 *	int		elementSize;
 *	char		*elementList;
 *
 * ARGUMENTS:
 *	ocbuf		The OC buffer to which the output command is to be
 *			added.
 *
 *	ocType		The type of output command to be added to the OC 
 *			buffer.
 *
 *	countNeeded     True if the list to be added requires a count to
 *			precede it; False otherwise 
 *
 *	count		The number of items in the list to be placed
 *			in the output command.
 *
 *	elementSize	The size (in bytes) of each item in the list.
 *
 *	elementList	A pointer to the data for the list.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine adds an output command of type `ocType' to the oc
 *	buffer.  The `PEX.h' include file contains possible values of `ocType'.
 *
 *	The last part of the command is a list of `count' items.  Each item is
 *	`elementSize' bytes long.  The list is padded if necessary.  If 
 *	`countNeeded`  is True the list created in `ocbuf' is preceded by a 
 *	count.  `elementList' points to the data for the list.  
 *
 * ERRORS:
 *	Renderer		Specified renderer resource identifier is
 *				invalid.
 *
 *	Structure		Specified structure resource identifier is
 *				invalid.
 *
 *	FloatingPointFormat	Device does not support the specified floating
 *				point format.
 *
 *	OutputCommand		A value in an output command is illegal,
 *				out of range, or otherwise inappropriate.
 *
 * SEE ALSO:
 *	`PEXAllocateRetainedOCBuffer", `PEXAllocateTransientOCBuffer",
 *	`PEXAddOC"
 *
 */
#define PEXAddListOC(_ocbuf_,_ocType_,_countNeeded_,_count_,_elSize_,_elList_)\
{							\
    int         _ocListSize = (_count_) * (_elSize_);	\
    char        *_pReq;					\
    PEXInitOC(_ocbuf_,_ocType_,				\
                (_countNeeded_) ? LENOF( pexOCcListHeader) :\
                                LENOF( pexOCListHeader),\
                NUMWORDS( _ocListSize), &_pReq);	\
    if (_pReq == NULL) return;				\
    if (_countNeeded_) ((pexOCcListHeader *)_pReq)->length = (_count_);\
    PEXCopyBytesToOC(_ocbuf_, _ocListSize, _elList_);	\
    PEXFinishOC(_ocbuf_);				\
}

#endif /* DEBUG not defined */

#endif /* PEXOCBUF_H */
