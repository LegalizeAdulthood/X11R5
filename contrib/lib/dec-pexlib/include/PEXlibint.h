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

/******************************************************************************/
/*	30-Apr-91   AFV	    Changing ColourSizeFromType to include pad word  */
/*			    for indexed color.				     */
/******************************************************************************/


#ifndef PEXLIBINT_H
#define PEXLIBINT_H

#include <X11/Xlibint.h>
#include "PEXproto.h"

/* 
 * These are used by PEXFree to know which routine needs to be called
 */

    extern void _PEXFreeInfo();
    extern void _PEXFreeEnumTypeDesc();
    extern void _PEXFreeTableEntries();
    extern void _PEXFreeList();
    extern void _PEXFreeStructurePath();
    extern void _PEXFreeRdrAttr();
    extern void _PEXFreePCAttr();
    extern void _PEXFreePtr();
    extern void _PEXFreeFontInfo();
    extern void _PEXFreeFontName();

/* 
 * Macros for padding bytes to be on long word boudaries and for 
 * converting between word sizes and bytes 
 */
/* How many bytes are needed to get a longword? */
#define Pad(size) (3 - (((size)+3) & 0x3))

#define NUMWORDS(_size_) ( ((unsigned int)((_size_) + 3)) >> 2 )

#define NUMBYTES(_len_) ( ((unsigned int)(_len_)) << 2 )

#define LENOF(_ctype_) (sizeof(_ctype_)>>2)

/* 
 *  Macros for dealing with the transport buffer 
 */
#define MAX_REQUEST_SIZE ( (1<<16) - 1 )

#define XBufferFlushed(_display_)                       \
        ( (_display_)->buffer == (_display_)->bufptr )

#define MaxXBufferLength(_display_)                     \
        (((_display_)->bufmax - (_display_)->buffer)>>2)

#define WordsLeftInXBuffer(_display_)                   \
        ( ((_display_)->bufmax - (_display_)->bufptr)>>2 )

/* 
 * Set up a packet to be sent to the X server 
 * If the buffer is full, flush it.  Then initialize the next command 
 * to be put in the buffer 
 */
#define PEXGetReq(name, req)\
    if ((display->bufptr + sizeof(pex/**/name/**/Req)) > display->bufmax)\
        _XFlush(display);\
    req = (pex/**/name/**/Req *)(display->last_req = display->bufptr);\
    req->reqType = pexExtCodes[ConnectionNumber(display)]->major_opcode;\
    req->opcode = PEX_/**/name;\
    req->length = (sizeof(pex/**/name/**/Req))>>2;\
    display->bufptr += sizeof(pex/**/name/**/Req);\
    display->request++

/* 
 * PEXGetReqExtra is the same as GetReq, but allocates "n" additional
 * bytes after the request. "n" must be a multiple of 4!  
 */
#define PEXGetReqExtra(name, n, req)\
    if ((display->bufptr + sizeof(pex/**/name/**/Req) + n + Pad(n)) >\
        display->bufmax)\
        _XFlush(display);\
    req = (pex/**/name/**/Req *)(display->last_req = display->bufptr);\
    req->reqType = pexExtCodes[ConnectionNumber(display)]->major_opcode;\
    req->opcode = PEX_/**/name;\
    req->length = (sizeof(pex/**/name/**/Req) + n + Pad(n))>>2;\
    display->bufptr += sizeof(pex/**/name/**/Req) + n + Pad(n);\
    display->request++

/*
 * PEXGetOCReq is similiar to PEXGetReq except that it does not update
 * display->bufptr .. this is used when writing ocs into the transport
 * buffer.  display->bufptr cannot be updated until the oc has been written
 * since it may need to be split up when it is sent (this happens when
 * the server's max_request_size is larger than the actual transport buffer)
 */
#define PEXGetOCReq(_display_,_nBytes_,_reqPtr_)        \
    {                                                   \
        if ((_display_)->bufptr + (_nBytes_) > (_display_)->bufmax)\
            _XFlush(_display_);                         \
        (_reqPtr_) = (unsigned int *)                   \
                ((_display_)->last_req = (_display_)->bufptr);\
        (_display_)->request++;                         \
    }

/*
 * Put data in the X transport buffer if it fits.  If not then call
 * _XSend.  _XSend will flush the transport buffer and write the data
 * to the wire.
 */
#define _SendToX(_display, _data, _size)                \
{                                                       \
    if ( (_display)->bufptr + (_size) <= (_display)->bufmax ) \
    {                                                   \
        COPY_AREA((_data), (_display)->bufptr, (int)(_size));\
        (_display)->bufptr += ((_size) + 3) & ~3;       \
    }                                                   \
    else                                                \
        _XSend(_display, _data, _size);                 \
}

/*
 * Need to use MPEX prefix for MPEX requests
 */
#ifdef MPEX
#define MPEXGetReq(name, req)\
    if ((display->bufptr + sizeof(mpex/**/name/**/Req)) > display->bufmax)\
        _XFlush(display);\
    req = (mpex/**/name/**/Req *)(display->last_req = display->bufptr);\
    req->reqType = pexExtCodes[ConnectionNumber(display)]->major_opcode;\
    req->opcode = MPEX_/**/name;\
    req->length = (sizeof(mpex/**/name/**/Req))>>2;\
    display->bufptr += sizeof(mpex/**/name/**/Req);\
    display->request++

#define MPEXGetReqExtra(name, n, req)\
    if ((display->bufptr + sizeof(mpex/**/name/**/Req) + n + Pad(n)) >\
        display->bufmax)\
        _XFlush(display);\
    req = (mpex/**/name/**/Req *)(display->last_req = display->bufptr);\
    req->reqType = pexExtCodes[ConnectionNumber(display)]->major_opcode;\
    req->opcode = MPEX_/**/name;\
    req->length = (sizeof(mpex/**/name/**/Req) + n + Pad(n))>>2;\
    display->bufptr += sizeof(mpex/**/name/**/Req) + n + Pad(n);\
    display->request++
#endif

/* 
 * See if XSynchronize has been called.  If so, send request right away 
 */
#define PEXSyncHandle()\
    if (display->synchandler) (*display->synchandler)(display)

/* 
 * Count the number of ones in a longword.  See TLE::VAXC note 493.4 
 */
#define CountOnes(mask, countReturn) \
    countReturn = ((mask) - (((mask)>>1)&0x77777777) \
	- (((mask)>>2)&0x33333333) - (((mask)>>3)&0x11111111)); \
    countReturn = ((((countReturn)+((countReturn)>>4)) & 0x0F0F0F0F) % 255)

/* 
 * A PEXHeader is at the beginning of all storage allocated for the 
 * client by PEXlib 
 */
typedef struct PEXHeader
    {
    void       (*free) (); /* The routine to call to free the storage */
    int        size;       /* size of the storage */
    int        type;       /* type of storage */
    } PEXHeader;


/* 
 * Memory allocation routines 
 */
#define PEXAllocBuf(size)          malloc(size)
#define PEXFreeBuf(ptr)            free(ptr)
#define PEXReallocBuf(ptr, size)   realloc(ptr, size)


/* 
 * dst is a pxlColourSpecifier, src is a pxlColour 
 */
#define InitializeColourSpecifier(dst, src)\
    (dst).colourType = pexColourTypeHidden; \
    COPY_SMALL_AREA ((src), &((dst).colour), \
	ColourSizeFromType (pexColourTypeHidden));


/* 
 * Returns the size of the colour in bytes by looking at the colour type.  
 * Note that the size of an indexed colour is NOW pre-padded to a 
 * word boundary 
 */
#define ColourSizeFromType(type)\
    ((type) == PEXIndexedColour ? (sizeof(pexTableIndex) * 2) :\
    ((type) == PEXRgb8Colour ? sizeof(pexRgb8Colour) :\
    ((type) == PEXRgb16Colour ? sizeof(pexRgb16Colour) : \
				sizeof(pexRgbFloatColour))))


/* 
 * How big, relative to the largest colour specifier, is the colour? */
/* The users of this macro must subtract this value from the sizeof value */

#define AdjustSizeFromType(type)\
    ( sizeof (pexColour) - ColourSizeFromType (type) )

#endif /* PEXLIBINT_H */
