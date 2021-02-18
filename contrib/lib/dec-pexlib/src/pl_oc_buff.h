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


#ifndef PLOCBUFF_H
#define PLOCBUFF_H

/* 
 * more typedefs for OC buffers
 */

typedef struct pxlOCChunk
{
    int                 size;
    struct pxlOCChunk   *pnext;
    struct pxlOCChunk   *pprev;
} pxlOCChunk;

typedef struct pxlTransientOCBuf
{
    pxlOCBuf		header;

    /* serial number of oc request currently in progress */
    int 		reqNumber;
} pxlTransientOCBuf;

typedef struct pxlRetainedOCBuf
{
    pxlOCBuf		header;

    /* Linked list containing requests. Each chunk is a separate request */
    pxlOCChunk		*chunkList;
    pxlOCChunk		*currentChunk;
} pxlRetainedOCBuf;

/*
 * macros used for implementing oc buffer interface
 */
#define MAX_OC_SIZE ( MAX_REQUEST_SIZE - (sizeof( pexOCRequestHeader)>>2) - \
		      (sizeof( mpexLargeRequestReq)>>2) )
#define OCIsTooLarge(_ocLength_) (_ocLength_ > MAX_OC_SIZE) 

#define IsLargeRequest(_display_,_ocLength_) \
  (_ocLength_ + LENOF( pexOCRequestHeader) > (_display_)->max_request_size)

#define STORE_OC_REQUEST_HEADER(_ocbuf_,_reqPtr_,_ocLength_,_numOC_) \
{							\
     ((pexOCRequestHeader *)(_reqPtr_))->extOpcode = (_ocbuf_)->extOpcode; \
     ((pexOCRequestHeader *)(_reqPtr_))->pexOpcode =  (_ocbuf_)->pexOpcode; \
     ((pexOCRequestHeader *)(_reqPtr_))->reqLength =	\
		(LENOF( pexOCRequestHeader) + (_ocLength_)); \
     ((pexOCRequestHeader *)(_reqPtr_))->fpFormat = pexFpFormat; \
     ((pexOCRequestHeader *)(_reqPtr_))->target = (_ocbuf_)->target; \
     ((pexOCRequestHeader *)(_reqPtr_))->numCommands = (_numOC_);  \
     (_reqPtr_) += LENOF( pexOCRequestHeader);		\
}

#define STORE_LARGE_REQUEST_HEADER(_ocbuf_,_reqPtr_,_seqNum_,_totNum_,_totLen_)\
{							\
     ((mpexLargeRequestReq *)(_reqPtr_))->reqType = (_ocbuf_)->extOpcode; \
     ((mpexLargeRequestReq *)(_reqPtr_))->opcode = MPEX_LargeRequest; \
     ((mpexLargeRequestReq *)(_reqPtr_))->length = LENOF( mpexLargeRequestReq);\
     ((mpexLargeRequestReq *)(_reqPtr_))->sequenceNum = (_seqNum_); \
     ((mpexLargeRequestReq *)(_reqPtr_))->totalNum = (_totNum_); \
     ((mpexLargeRequestReq *)(_reqPtr_))->totalLength = (_totLen_); \
     (_reqPtr_) += LENOF( mpexLargeRequestReq);		\
}

#endif /* PLOCBUFF_H */
