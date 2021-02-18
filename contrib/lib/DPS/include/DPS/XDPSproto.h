/*
   XDPSproto.h -- protocol requests constants and alignment values,
		  analgous to Xproto.h
  		 
Copyright 1988, 1989 Adobe Systems Incorporated.
Portions Copyright 1988, 1989 Digital Equipment Corporation.

Permission to use, copy, modify, and distribute this software and
its documentation for any purpose and without fee is hereby granted,
provided that the above copyright notices appear in all copies and
that both those copyright notices and this permission notice appear
in supporting documentation, and that the names of Adobe Systems 
Incorporated and Digital Equipment Corporation not be used in 
advertising or publicity pertaining to distribution of the software 
without specific, written prior permission.  Adobe Systems Incorporated
and Digital Equipment Corporation make no representations about the 
suitability of this software for any purpose.  It is provided "as is" 
without express or implied warranty.

ADOBE SYSTEMS INCORPORATED AND DIGITAL EQUIPMENT CORPORATION DISCLAIM
ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ADOBE
SYSTEMS INCORPORATED AND DIGITAL EQUIPMENT CORPORATION BE LIABLE FOR
ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN 
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT 
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Author: Adobe Systems Incorporated and Digital Equipment Corporation
  
*/

#ifndef _XDPSproto_h
#define _XDPSproto_h

/* NOTE: The Digital UWS 2.2 DECWindows/DPS server uses "DPSExtension"
         for the name of the extension.  The name registered with
         the X Consortium is "Adobe-DPS-Extension".  Your client
         should query both names. */

#define DPSNAME			"Adobe-DPS-Extension"
#define DECDPSNAME		"DPSExtension"

#define DPSPROTOCOLVERSION	8

#define CONTEXTXID		CARD32 
#define SPACEXID		CARD32 

#define BYTESPEROUTPUTEVENT	24

/*
 * Requests:
 */

#define X_PSInit		1
#define X_PSCreateContext	2
#define X_PSCreateSpace		3
#define X_PSGiveInput		4
#define X_PSGetStatus		5
#define X_PSDestroySpace	6
#define X_PSReset		7
#define X_PSNotifyContext	8
#define X_PSCreateContextFromID	9
#define X_PSXIDFromContext	10
#define X_PSContextFromXID	11
#define X_PSSetStatusMask	12

#define PSLASTREQUEST		X_PSSetStatusMask

typedef struct _PSInit {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSInit */
    CARD16 length;		/* length of request */
    CARD32 libraryversion;	/* version of protocol implementation */
} xPSInitReq;
#define sz_xPSInitReq	8

typedef struct _PSCreateContext {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSCreateContext */
    CARD16 length;		/* length of request */
    CONTEXTXID cxid;		/* XID for context */
    SPACEXID sxid;		/* XID for Space to use for new context */
    Drawable drawable;		/* Drawable for new context */
    GContext gc;		/* GC for new context */
    INT16 x;			/* x, initial origin of context's device */
    INT16 y;			/* y, initial origin of context's device */
    CARD32 eventmask;		/* events that context is interested in */
    Colormap cmap;		/* colormap for this context */
    CARD32 redmax;		/* color cube */
    CARD32 redmult;
    CARD32 greenmax;
    CARD32 greenmult;
    CARD32 bluemax;
    CARD32 bluemult;
    CARD32 colorbase;
    CARD32 graymax;		/* gray ramp */
    CARD32 graymult;
    CARD32 graybase;
    CARD32 actual;              /* actual colors, if any */
} xPSCreateContextReq;
#define sz_xPSCreateContextReq	76

typedef struct _PSCreateSpace {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSCreateSpace */
    CARD16 length;
    SPACEXID sxid;		/* Space to be created */
} xPSCreateSpaceReq;
#define sz_xPSCreateSpaceReq	8

typedef struct _PSGiveInput {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSGiveInput */
    CARD16 length;		/* length of request */
    CARD16 nunits;              /* number of bytes in following string */
    CARD16 pad;
    CONTEXTXID cxid;		/* Context for input. */
} xPSGiveInputReq;
#define sz_xPSGiveInputReq	12

typedef struct _PSGetStatus {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSGetStatus */
    CARD16 length;
    CONTEXTXID cxid;
    CARD32 notifyIfChange;	/* unused */
} xPSGetStatusReq;
#define sz_xPSGetStatusReq	12

typedef struct _PSDestroySpace {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSDestroySpace */
    CARD16 length;
    SPACEXID sxid;
} xPSDestroySpaceReq;
#define sz_xPSDestroySpaceReq	8


typedef struct _PSReset {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSReset */
    CARD16 length;
    CONTEXTXID cxid;
} xPSResetReq;
#define sz_xPSResetReq	8

typedef struct _PSNotifyContext {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSNotifyContext */
    CARD16 length;
    CONTEXTXID cxid;
    CARD8 notifyType;		/* see XDPS.h */
    CARD8 pad1;
    CARD8 pad2;
    CARD8 pad3;
} xPSNotifyContextReq;
#define sz_xPSNotifyContextReq	12


typedef struct _PSCreateContextFromID {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSCreateContextFromID */
    CARD16 length;
    CARD32 cpsid;		/* the PostScript context ID */
    CONTEXTXID cxid;		/* resource ID to associate with cpsid */
} xPSCreateContextFromIDReq;
#define sz_xPSCreateContextFromIDReq	12


typedef struct _PSXIDFromContext {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSXIDFromContext */
    CARD16 length;
    CARD32 cpsid;
} xPSXIDFromContextReq;
#define sz_xPSXIDFromContextReq	8

typedef struct _PSContextFromXID {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSContextFromXID */
    CARD16 length;
    CONTEXTXID cxid;
} xPSContextFromXIDReq;
#define sz_xPSContextFromXIDReq	8

typedef struct _PSSetStatusMask {
    CARD8 reqType;		/* always DPSReqCode */
    CARD8 dpsReqType;		/* always X_PSContextFromXID */
    CARD16 length;
    CONTEXTXID cxid;
    CARD32 enableMask;
    CARD32 disableMask;
    CARD32 nextMask;
} xPSSetStatusMaskReq;
#define sz_xPSSetStatusMaskReq	20


/*
 * Replies:
 */

typedef struct {	
    BYTE type;              /* X_Reply */
    BYTE pad;
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    CARD32 cpsid B32;
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    } xPSCreateContextReply;

typedef struct {
    BYTE type;              /* X_Reply */
    BYTE status;            /* see XDPS.h */
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    CARD32 pad0 B32;
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    } xPSGetStatusReply;

typedef struct {	
    BYTE type;              /* X_Reply */
    BYTE pad;
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    SPACEXID sxid B32;
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    } xPSCreateContextFromIDReply;

typedef struct {	
    BYTE type;              /* X_Reply */
    BYTE pad;
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    CONTEXTXID cxid B32;
    SPACEXID sxid B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    } xPSXIDFromContextReply;

typedef struct {	
    BYTE type;              /* X_Reply */
    BYTE pad;
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    CARD32 cpsid B32;
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    CARD32 pad4 B32;
    CARD32 pad5 B32;
    } xPSContextFromXIDReply;

typedef struct {	
    BYTE type;              /* X_Reply */
    BYTE pad;
    CARD16 sequenceNumber B16;  /* of last request received by server */
    CARD32 length B32;      /* 4 byte quantities beyond size of GenericReply */
    CARD32 serverversion B32;	/* version of protocol implementation */
    CARD32 preferredNumberFormat B32; /* that the server wants to use */
    CARD32 floatingNameLength B32; /* length of name of native floating fmt */
    CARD32 pad1 B32;
    CARD32 pad2 B32;
    CARD32 pad3 B32;
    } xPSInitReply;		/* Followed by name of native floating fmt. */



/*
 * Events:
 */

typedef struct {
    BYTE type;
    CARD8 length;
    CARD16 sequenceNumber;
    CONTEXTXID cxid;
    CARD8 data[BYTESPEROUTPUTEVENT];
} PSOutputEvent;


typedef struct  {
    BYTE type;
    BYTE status;		/* see XDPS.h */
    CARD16 sequenceNumber;
    CONTEXTXID cxid;
    CARD32 pad1, pad2, pad3, pad4, pad5, pad6;
} PSStatusEvent;

#endif /* _XDPSproto_h */
