/* $Header: x11.h,v 2.2 91/09/11 15:52:53 sinyaw Exp $ */
/*      @(#)x11.h 1.4 90/04/05 SMI/MIT      */

/***********************************************************
Copyright 1990 by Sun Microsystems, Inc. and the X Consortium.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Sun Microsystems,
the X Consortium, and MIT not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.  

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT 
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/* ************************************************************ *
 *						     		*
 *  Type definitions and Connection State for the  X11 protocol *
 *						      		*
 *	James Peterson, 1988			      		*
 *	(c) Copyright MCC, 1988 		      		*
 *						      		*
 * ************************************************************ */


/* Some field contents are constants, not just types */

#define CONST1(n)  CARD8
#define CONST2(n)  CARD16
#define CONST4(n)  CARD32

/* Some field contents define the components of an expression */

#define DVALUE1(expression)  CARD8
#define DVALUE2(expression)  CARD16
#define DVALUE4(expression)  CARD32


/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Built-in Types */

#define BYTE 1		       /* 8-bit value */
#define INT8 2		       /* 8-bit signed integer */
#define INT16 3		       /* 16-bit signed integer */
#define INT32 4		       /* 32-bit signed integer */
#define CARD8 5		       /* 8-bit unsigned integer */
#define CARD16 6	       /* 16-bit unsigned integer */
#define CARD32 7	       /* 32-bit unsigned integer */
#define STRING8 8	       /* List of CARD8 */
#define STRING16 9	       /* List of CHAR2B */
#define TEXTITEM8 10	       /* STRING8 or Font shift */
#define TEXTITEM16 11	       /* STRING16 or Font shift */

#define WINDOW 12	       /* CARD32  plus 0 = None */
#define WINDOWD 13	       /* CARD32  plus 0 = PointerWindow, 1 =
			          InputFocus */
#define WINDOWNR 14	       /* CARD32  plus 0 = None, 1 = PointerRoot */

#define PIXMAP 15	       /* CARD32  plus 0 = None */
#define PIXMAPNPR 16	       /* CARD32  plus 0 = None, 1 = ParentRelative 
			       */
#define PIXMAPC 17	       /* CARD32  plus 0 = CopyFromParent */

#define CURSOR 18	       /* CARD32  plus 0 = None */

#define FONT 19		       /* CARD32  plus 0 = None */

#define GCONTEXT 20	       /* CARD32 */

#define COLORMAP 21	       /* CARD32 plus 0 = None */
#define COLORMAPC 22	       /* CARD32 plus 0 = CopyFromParent */

#define DRAWABLE 23	       /* CARD32 */
#define FONTABLE 24	       /* CARD32 */

#define ATOM 25		       /* CARD32 plus 0 = None */
#define ATOMT 26	       /* CARD32 plus 0 = AnyPropertyType */

#define VISUALID 27	       /* CARD32 plus 0 = None */
#define VISUALIDC 28	       /* CARD32 plus 0 = CopyFromParent */

#define TIMESTAMP 29	       /* CARD32 plus 0 as the current time */

#define RESOURCEID 30	       /* CARD32 plus 0 = AllTemporary */

#define KEYSYM 31	       /* CARD32 */
#define KEYCODE 32	       /* CARD8 */
#define KEYCODEA 33	       /* CARD8 plus 0 = AnyKey */

#define BUTTON 34	       /* CARD8 */
#define BUTTONA 35	       /* CARD8 plus 0 = AnyButton */

#define EVENTFORM 36	       /* event format */
#define CHAR8 37	       /* CARD8 interpreted as a character */
#define STR 38		       /* String of CHAR8 with preceding length */

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Defined types */

#define BITGRAVITY	 40 
#define WINGRAVITY	 41 
#define BOOL		 42 
#define HOSTFAMILY	 43 
#define PK_MODE		 44 
#define NO_YES		 45 
#define WINDOWCLASS	 46 
#define BACKSTORE	 47 
#define MAPSTATE	 48 
#define STACKMODE	 49 
#define CIRMODE	 	 50 
#define CHANGEMODE	 51 
#define GRABSTAT	 52 
#define EVENTMODE	 53 
#define FOCUSAGENT	 54 
#define DIRECT		 55 
#define GCFUNC		 56 
#define LINESTYLE	 57 
#define CAPSTYLE	 58 
#define JOINSTYLE	 59 
#define FILLSTYLE	 60 
#define FILLRULE	 61 
#define SUBWINMODE	 62 
#define ARCMODE	 	 63 
#define RECTORDER	 64 
#define COORMODE	 65 
#define POLYSHAPE	 66 
#define IMAGEMODE	 67 
#define ALLORNONE	 68 
#define OBJECTCLASS	 69 
#define OFF_ON		 70 
#define INS_DEL	 	 71 
#define DIS_EN		 72 
#define CLOSEMODE	 73 
#define SAVEMODE	 74 
#define RSTATUS	 	 75 
#define MOTIONDETAIL	 76 
#define ENTERDETAIL	 77 
#define BUTTONMODE	 78 
#define SCREENFOCUS	 79 
#define VISIBLE	 	 80 
#define CIRSTAT	 	 81 
#define PROPCHANGE	 82 
#define CMAPCHANGE	 83 
#define MAPOBJECT	 84 
#define SETofEVENT	 85 
#define SETofPOINTEREVENT	 86 
#define SETofDEVICEEVENT	 87 
#define SETofKEYBUTMASK	 	 88 
#define SETofKEYMASK		 89 
#define WINDOW_BITMASK		 90 
#define CONFIGURE_BITMASK	 91 
#define GC_BITMASK		 92 
#define KEYBOARD_BITMASK	 93 
#define COLORMASK		 94 
#define CHAR2B		 95 
#define POINT		 96 
#define RECTANGLE	 97 
#define ARC		 98 
#define HOST		 99 
#define TIMECOORD	100 
#define FONTPROP	101 
#define CHARINFO	102 
#define SEGMENT		103 
#define COLORITEM	104 
#define RGB		105 
#define BYTEMODE	110
#define BYTEORDER	111
#define COLORCLASS	112
#define FORMAT		113
#define SCREEN		114
#define DEPTH		115
#define VISUALTYPE	116

#define REQUEST		117
#define REPLY		118
#define ERROR		119
#define EVENT		120

#define MaxTypes 128

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* declaration of the existance of print routines for the basic types */

PrintINT8();
PrintINT16();
PrintINT32();
PrintCARD8();
PrintCARD16();
PrintCARD32();
PrintBYTE();
PrintCHAR8();
PrintSTRING16();
PrintTEXTITEM8();
PrintTEXTITEM16();
PrintSTR();
PrintWINDOW();
PrintWINDOWD();
PrintWINDOWNR();
PrintPIXMAP();
PrintPIXMAPNPR();
PrintPIXMAPC();
PrintCURSOR();
PrintFONT();
PrintGCONTEXT();
PrintCOLORMAP();
PrintCOLORMAPC();
PrintDRAWABLE();
PrintFONTABLE();
PrintATOM();
PrintATOMT();
PrintVISUALID();
PrintVISUALIDC();
PrintTIMESTAMP();
PrintRESOURCEID();
PrintKEYSYM();
PrintKEYCODE();
PrintKEYCODEA();
PrintBUTTON();
PrintBUTTONA();
PrintEVENTFORM();
PrintENUMERATED();
PrintSET();

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/*  Type Definition Table

    Each item in the X11 Protocol has a type.  There are about 120
    different types.  We need to be able to print each item in a 
    format and interpretation which is appropriate for the type of
    that item.  To do so, we build a table describing each type.
    Each type has a name, possibly a list of named values and a
    procedure which knows how to print that type.
*/

/* Types of Types */

#define BUILTIN    1
#define ENUMERATED 2
#define SET        3
#define RECORD     5


/* Enumerated and Set types need a list of Named Values */

struct ValueListEntry
{
    struct ValueListEntry  *Next;
    char   *Name;
    short   Type;
    short   Length;
    long    Value;
};

struct TypeDef
{
    char   *Name;
    short   Type /* BUILTIN, ENUMERATED, SET, or RECORD */ ;
    struct ValueListEntry  *ValueList;
    int     (*PrintProc)();
};

typedef struct TypeDef *TYPE;

struct TypeDef  TD[MaxTypes];

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* Reply Buffer: Pseudo-buffer used to provide the opcode for the
                 request to which this is a reply: Set by DecodeReply
		 and used in the PrintField of the Reply procedure */
unsigned char    RBf[2];


/* Sequence Buffer: Pseudo-buffer used to provide the sequence number for a
                 request: Set by DecodeReply and used in a PrintField of 
		 the Request procedure */
unsigned char    SBf[4];


#define PRINTSERVER 5	       /* indent output as if it comes from server */
#define PRINTCLIENT 1	       /* indent output as if it comes from client */

/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* 
  In general, we are called with a buffer of bytes and are supposed to
  try to make sense of these bytes according to the X11 protocol.  There
  are two different types of communication: requests from the client to
  the server and replies/errors/events from the server to the client.
  We must interpret these two differently.

  Also, we must consider that the bytes on the communication socket may
  be sent asynchronously in any amount.  This means that we must be prepared
  to read in and save some bytes until we get enough to do something with
  them.  For example, suppose that we get a buffer from a client.  We would
  expect it to be a request.  The request may be 24 bytes long.  We may find,
  however, that only 16 bytes have actually arrived -- the other 8 are stuck
  in a buffer somewhere.  We must be prepared to simply hold the 16 bytes we
  have until more bytes arrive.

  In general, we do two things: we wait for some number of bytes, and
  then we interpret this set of bytes.  To interpret this data we use 
  a modified state machine.  We keep two pieces of information:

  (1) the number of bytes that we need
  (2) what to do with those bytes.

  This last piece of information is the "state" of the interpretation.
  We keep the state as a pointer to the procedure that is to be executed.


  CLIENTS:

  The data going from the client to the x11 server consists of a
  set-up message followed by an infinite stream of variable length
  requests.  

  Our overall flow is then:

  (a) Wait for 12 bytes.
  (b) Interpret these first 12 bytes of the set-up message to get the
      length of the rest of the message.
  (c) Wait for the rest of the set-up message.
  (d) Interpret and print the set-up message.
  
  *** end of set-up phase -- start normal request loop ***

  (e) Wait for 4 bytes.
  (f) Interpret these 4 bytes to get the length of the rest of the command.
  (g) Wait for the rest of the command.
  (h) Interpret and print the command.
  (i) Go back to step (e).

  SERVERS:

  Again, we have a set-up reply followed by an infinite stream of variable
  length replies/errors/events.

  Our overall flow is then:

  (a) Wait for 8 bytes.
  (b) Interpret the 8 bytes to get the length of the rest of the set-up reply.
  (c) Wait for the rest of the set-up reply.
  (d) Interpret and print the set-up reply.

  *** end of set-up phase -- start normal reply/error/event loop ***

  We have the following properties of X11 replies, errors, and events:

  Replies:  32 bytes plus a variable amount.  Byte 0 is 1.
            Bytes 2-3 are a sequence number; bytes 4-7 are length (n).  The
	    complete length of the reply is 32 + 4 * n.

  Errors:   32 bytes.  Byte 0 is 0.
            Byte 1 is an error code; bytes 2-3 are a sequence number.
	    Bytes 8-9 are a major opcode; byte 10 is a minor opcode.

  Events:   32 bytes.  Byte 0 is 2, 3, 4, ....

  Looking at this we have two choices:  wait for one byte and then separately
  wait for replies, errors, and events, or wait for 32 bytes, then separately
  process each type.  We may have to wait for more, in the event of a reply.
  This latter seems more effective.  It appears reply/error/event formats
  were selected to allow waiting for 32 bytes, and it will allow short packets
  which are only 32 bytes long, to be processed completely in one step.
  
  Thus, For normal reply/error/event processing we have 

  (e) Wait for 32 bytes.
  (f) Interpret these 32 bytes.  If possible, go back to step (e).
  (g) If the packet is a reply with bytes 4-7 non-zero, wait for the
      remainder of the the reply.
  (h) Interpret and print the longer reply.  Go back to step (e).
  

  The similarity in approach to how both the client and server are handled
  suggests we can use the same control structure to drive the interpretation
  of both types of communication client->server and server->client.  
  Accordingly, we package up the relevant variables in a ConnState
  record.  The ConnState record contains the buffer of saved bytes (if any),
  the size and length of this buffer, the number of bytes we are waiting for
  and what to do when we get them.  A separate ConnState record is kept
  for the client and server.

  In addition, we may have several different client or server connections.
  Thus we need an array of all the necessary state for each client or server.
  A client/server is identified with a file descriptor (fd), so we use the
  fd to identify the client/server and use it as an index into an array of
  state variables.
*/

struct ConnState
{
    unsigned char   *SavedBytes;
    long    SizeofSavedBytes;
    long    NumberofSavedBytes;

    long    NumberofBytesNeeded;
    long    (*ByteProcessing)();

    long    SequenceNumber;
};

struct ConnState    CS[StaticMaxFD];




/* ************************************************************ */
/*								*/
/*								*/
/* ************************************************************ */

/* declaraction of the types of some common functions */

extern unsigned long    ILong();
extern unsigned short   IShort();
extern unsigned short   IChar2B();
extern unsigned short   IByte();
extern Boolean          IBool();

extern PrintString8();
extern PrintString16();
extern PrintListSTR();

extern long    PrintList();
extern long    pad();
