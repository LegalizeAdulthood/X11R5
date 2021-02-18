/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * Project: XMON - An X protocol monitor
 *
 * File: decode11.c
 *
 * Description: Decoding and switching routines for the X11 protocol
 *
 * There are 4 types of things in X11: requests, replies, errors, and
 * events.
 *
 * Each of them has a format defined by a small integer that defines the
 * type of the thing.
 *
 * Requests have an opcode in the first byte.
 *
 * Events have a code in the first byte.
 *
 * Errors have a code in the second byte (the first byte is 0)
 *
 * Replies have a sequence number in bytes 2 and 3.  The sequence number
 * should be used to identify the request that was sent, and from that
 * request we can determine the type of the reply.
 *
 */

#include "common.h"

#include "xmond.h"
#include "linkl.h"
#include "x11.h"

#define MAX_REQUEST	    127
#define MAX_EXT_REQUEST	    255
#define MAX_EVENT	    34
#define MAX_ERROR	    17

/*
 * We need to keep the sequence number for a request to match it with an
 * expected reply.  The sequence number is associated only with the
 * particular connection that we have. We would expect these replies to be
 * handled as a FIFO queue.
*/

typedef struct
{
    long    SequenceNumber;
    short   Request;
}
    QueueEntry;

/* function prototypes: */
/* decode11.c: */
static void SequencedReplyExpected P((Client *client , short RequestType ));
static short CheckReplyTable P((Server *server , short SequenceNumber ,
Bool checkZero ));
static void ReplyExpected P((Client *client , short Request ));

/* end function prototypes */

extern Bool		    ignore_bytes;

extern int		    RequestVerbose;
extern int		    EventVerbose;
extern int		    ReplyVerbose;
extern int		    ErrorVerbose;
extern int		    CurrentVerbose;

extern Bool		    VerboseRequest[];
extern Bool		    VerboseEvent[];
extern Bool		    BlockRequest[];
extern Bool		    BlockEvent[];
extern Bool		    MonitoringRequests;
extern Bool		    MonitoringEvents;
extern Bool		    BlockingRequests;
extern Bool		    BlockingEvents;
extern int		    SelectedRequestVerbose;
extern int		    SelectedEventVerbose;

static int		    Lastfd;
static long		    LastSequenceNumber;
static short		    LastReplyType;
static int		    RequestCount[MAX_EXT_REQUEST + 1];
static int		    EventCount[MAX_EVENT + 1];
static int		    ErrorCount[MAX_ERROR + 1];
static Bool		    CountRequests;
static Bool		    CountEvents;
static Bool		    CountErrors;


Global void
InitRequestCount()
{
    ClearRequestCount();
    CountRequests = True;
}

Global void
ClearRequestCount()
{
    int i;

    for (i = 0; i <= MAX_EXT_REQUEST; i++)
	RequestCount[i] = 0;
}

Global void
StartRequestCount()
{
    CountRequests = True;
}

Global void
EndRequestCount()
{
    CountRequests = False;
}

Global void
PrintRequestCounts()
{
    int i;
    Bool found;

    found = False;
    for (i = MAX_REQUEST + 1; i <= MAX_EXT_REQUEST; i++)
	if (RequestCount[i] != 0)
	{
	    if (!found)
		fprintf(stdout, "extended requests received:\n");
	    found = True;
	    fprintf(stdout, "%3d  %d\n", i, RequestCount[i]);
	}
    if (found)
	fprintf(stdout, "\n");
    found = False;
    for (i = 1; i <= MAX_REQUEST; i++)
	if ((i <= 119 || i == 127) && RequestCount[i] != 0)
	{
	    if (!found)
		fprintf(stdout, "requests received:\ncode  count  name \n");
	    found = True;
	    fprintf(stdout, "%3d   %3d    ", i, RequestCount[i]);
	    PrintENUMERATED(&i, sizeof(i), TD[REQUEST].ValueList);
	    fprintf(stdout, "\n");
	}
    if (!found)
	fprintf(stdout, "no requests received\n");
}

Global void
PrintZeroRequestCounts()
{
    int i;

    fprintf(stdout, "requests never received:\ncode  name\n");
    for (i = 1; i <= MAX_REQUEST; i++)
	if ((i <= 119 || i == 127) && RequestCount[i] == 0)
	{
	    fprintf(stdout, "%3d   ", i);
	    PrintENUMERATED(&i, sizeof(i), TD[REQUEST].ValueList);
	    fprintf(stdout, "\n");
	}
}

Global void
InitEventCount()
{
    ClearEventCount();
    CountEvents = True;
}

Global void
ClearEventCount()
{
    int i;

    for (i = 0; i <= MAX_EVENT; i++)
	EventCount[i] = 0;
}

Global void
StartEventCount()
{
    CountEvents = True;
}

Global void
EndEventCount()
{
    CountEvents = False;
}

Global void
PrintEventCounts()
{
    int i;
    Bool found;

    found = False;
    for (i = 2; i <= MAX_EVENT; i++)
	if (EventCount[i] != 0)
	{
	    if (!found)
		fprintf(stdout, "events received:\ncode  count name:\n");
	    found = True;
	    fprintf(stdout, "%3d   %3d    ", i, EventCount[i]);
	    PrintENUMERATED(&i, sizeof(i), TD[EVENT].ValueList);
	    fprintf(stdout, "\n");
	}
    if (!found)
	fprintf(stdout, "no events received\n");
}

Global void
PrintZeroEventCounts()
{
    int i;

    fprintf(stdout, "events never received:\ncode  name\n");
    for (i = 2; i <= MAX_EVENT; i++)
	if (EventCount[i] == 0)
	{
	    fprintf(stdout, "%3d   ", i);
	    PrintENUMERATED(&i, sizeof(i), TD[EVENT].ValueList);
	    fprintf(stdout, "\n");
	}
}

Global void
InitErrorCount()
{
    ClearErrorCount();
    CountErrors = True;
}

Global void
ClearErrorCount()
{
    int i;

    for (i = 0; i <= MAX_ERROR; i++)
	ErrorCount[i] = 0;
}

Global void
StartErrorCount()
{
    CountErrors = True;
}

Global void
EndErrorCount()
{
    CountErrors = False;
}

Global void
PrintErrorCounts()
{
    int i;
    Bool found;

    found = False;
    for (i = 1; i <= MAX_ERROR; i++)
	if (ErrorCount[i] != 0)
	{
	    if (!found)
		fprintf(stdout, "errors received:\ncode  count  name \n");
	    found = True;
	    fprintf(stdout, "%3d   %3d    ", i, ErrorCount[i]);
	    PrintENUMERATED(&i, sizeof(i), TD[ERROR].ValueList);
	    fprintf(stdout, "\n");
	}
    if (!found)
	fprintf(stdout, "no errors received\n");
}

Global void
PrintZeroErrorCounts()
{
    int i;

    fprintf(stdout, "errors never received:\ncode  name\n");
    for (i = 1; i <= MAX_ERROR; i++)
	if (ErrorCount[i] == 0)
	{
	    fprintf(stdout, "%3d   ", i);
	    PrintENUMERATED(&i, sizeof(i), TD[ERROR].ValueList);
	    fprintf(stdout, "\n");
	}
}

Global void
DecodeRequest(client, buf, n)
    Client		    *client;
    unsigned char	    *buf;
    long		    n;
{
    int			    fd = client->fdd->fd;
    short		    Request = IByte (&buf[0]);

    if
    (
	BlockingRequests
	&&
	1 <= Request && Request <= 127
	&&
	BlockRequest[Request]
    )
	ignore_bytes = True;

    if (!ignore_bytes) /* We don't send this request, so don't increment SN */
	client->SequenceNumber++;
    bcopy ((char *)&(client->SequenceNumber), (char *)SBf, sizeof(long));
    SetIndentLevel(PRINTCLIENT);

    if
    (
	MonitoringRequests
	&&
	1 <= Request && Request <= 127
	&&
	VerboseRequest[Request]
    )
	CurrentVerbose = SelectedRequestVerbose;
    else
	CurrentVerbose = RequestVerbose;

    if (CurrentVerbose > 3)
	DumpItem("Request", fd, buf, n);
    if (CountRequests)
    {
	if (Request < 0 || Request > MAX_EXT_REQUEST)
	    fprintf(stdout, "####### Illegal request opcode %d\n", Request);
	else
	    RequestCount[Request]++;
    }
    if (Request <= 0 || 127 < Request)
	fprintf
	(
	    stdout, "Extended request opcode: %d, SequenceNumber: %d\n",
	    Request, client->SequenceNumber
	);
    else
	switch (Request)
	{
	case 1:
	    CreateWindow(buf);
	    break;
	case 2:
	    ChangeWindowAttributes(buf);
	    break;
	case 3:
	    GetWindowAttributes(buf);
	    ReplyExpected(client, Request);
	    break;
	case 4:
	    DestroyWindow(buf);
	    break;
	case 5:
	    DestroySubwindows(buf);
	    break;
	case 6:
	    ChangeSaveSet(buf);
	    break;
	case 7:
	    ReparentWindow(buf);
	    break;
	case 8:
	    MapWindow(buf);
	    break;
	case 9:
	    MapSubwindows(buf);
	    break;
	case 10:
	    UnmapWindow(buf);
	    break;
	case 11:
	    UnmapSubwindows(buf);
	    break;
	case 12:
	    ConfigureWindow(buf);
	    break;
	case 13:
	    CirculateWindow(buf);
	    break;
	case 14:
	    GetGeometry(buf);
	    ReplyExpected(client, Request);
	    break;
	case 15:
	    QueryTree(buf);
	    ReplyExpected(client, Request);
	    break;
	case 16:
	    InternAtom(buf);
	    ReplyExpected(client, Request);
	    break;
	case 17:
	    GetAtomName(buf);
	    ReplyExpected(client, Request);
	    break;
	case 18:
	    ChangeProperty(buf);
	    break;
	case 19:
	    DeleteProperty(buf);
	    break;
	case 20:
	    GetProperty(buf);
	    ReplyExpected(client, Request);
	    break;
	case 21:
	    ListProperties(buf);
	    ReplyExpected(client, Request);
	    break;
	case 22:
	    SetSelectionOwner(buf);
	    break;
	case 23:
	    GetSelectionOwner(buf);
	    ReplyExpected(client, Request);
	    break;
	case 24:
	    ConvertSelection(buf);
	    break;
	case 25:
	    SendEvent(buf);
	    break;
	case 26:
	    GrabPointer(buf);
	    ReplyExpected(client, Request);
	    break;
	case 27:
	    UngrabPointer(buf);
	    break;
	case 28:
	    GrabButton(buf);
	    break;
	case 29:
	    UngrabButton(buf);
	    break;
	case 30:
	    ChangeActivePointerGrab(buf);
	    break;
	case 31:
	    GrabKeyboard(buf);
	    ReplyExpected(client, Request);
	    break;
	case 32:
	    UngrabKeyboard(buf);
	    break;
	case 33:
	    GrabKey(buf);
	    break;
	case 34:
	    UngrabKey(buf);
	    break;
	case 35:
	    AllowEvents(buf);
	    break;
	case 36:
	    GrabServer(buf);
	    break;
	case 37:
	    UngrabServer(buf);
	    break;
	case 38:
	    QueryPointer(buf);
	    ReplyExpected(client, Request);
	    break;
	case 39:
	    GetMotionEvents(buf);
	    ReplyExpected(client, Request);
	    break;
	case 40:
	    TranslateCoordinates(buf);
	    ReplyExpected(client, Request);
	    break;
	case 41:
	    WarpPointer(buf);
	    break;
	case 42:
	    SetInputFocus(buf);
	    break;
	case 43:
	    GetInputFocus(buf);
	    ReplyExpected(client, Request);
	    break;
	case 44:
	    QueryKeymap(buf);
	    ReplyExpected(client, Request);
	    break;
	case 45:
	    OpenFont(buf);
	    break;
	case 46:
	    CloseFont(buf);
	    break;
	case 47:
	    QueryFont(buf);
	    ReplyExpected(client, Request);
	    break;
	case 48:
	    QueryTextExtents(buf);
	    ReplyExpected(client, Request);
	    break;
	case 49:
	    ListFonts(buf);
	    ReplyExpected(client, Request);
	    break;
	case 50:
	    ListFontsWithInfo(buf);
	    ReplyExpected(client, Request);
	    break;
	case 51:
	    SetFontPath(buf);
	    break;
	case 52:
	    GetFontPath(buf);
	    ReplyExpected(client, Request);
	    break;
	case 53:
	    CreatePixmap(buf);
	    break;
	case 54:
	    FreePixmap(buf);
	    break;
	case 55:
	    CreateGC(buf);
	    break;
	case 56:
	    ChangeGC(buf);
	    break;
	case 57:
	    CopyGC(buf);
	    break;
	case 58:
	    SetDashes(buf);
	    break;
	case 59:
	    SetClipRectangles(buf);
	    break;
	case 60:
	    FreeGC(buf);
	    break;
	case 61:
	    ClearArea(buf);
	    break;
	case 62:
	    CopyArea(buf);
	    break;
	case 63:
	    CopyPlane(buf);
	    break;
	case 64:
	    PolyPoint(buf);
	    break;
	case 65:
	    PolyLine(buf);
	    break;
	case 66:
	    PolySegment(buf);
	    break;
	case 67:
	    PolyRectangle(buf);
	    break;
	case 68:
	    PolyArc(buf);
	    break;
	case 69:
	    FillPoly(buf);
	    break;
	case 70:
	    PolyFillRectangle(buf);
	    break;
	case 71:
	    PolyFillArc(buf);
	    break;
	case 72:
	    PutImage(buf);
	    break;
	case 73:
	    GetImage(buf);
	    ReplyExpected(client, Request);
	    break;
	case 74:
	    PolyText8(buf);
	    break;
	case 75:
	    PolyText16(buf);
	    break;
	case 76:
	    ImageText8(buf);
	    break;
	case 77:
	    ImageText16(buf);
	    break;
	case 78:
	    CreateColormap(buf);
	    break;
	case 79:
	    FreeColormap(buf);
	    break;
	case 80:
	    CopyColormapAndFree(buf);
	    break;
	case 81:
	    InstallColormap(buf);
	    break;
	case 82:
	    UninstallColormap(buf);
	    break;
	case 83:
	    ListInstalledColormaps(buf);
	    ReplyExpected(client, Request);
	    break;
	case 84:
	    AllocColor(buf);
	    ReplyExpected(client, Request);
	    break;
	case 85:
	    AllocNamedColor(buf);
	    ReplyExpected(client, Request);
	    break;
	case 86:
	    AllocColorCells(buf);
	    ReplyExpected(client, Request);
	    break;
	case 87:
	    AllocColorPlanes(buf);
	    ReplyExpected(client, Request);
	    break;
	case 88:
	    FreeColors(buf);
	    break;
	case 89:
	    StoreColors(buf);
	    break;
	case 90:
	    StoreNamedColor(buf);
	    break;
	case 91:
	    QueryColors(buf);
	    ReplyExpected(client, Request);
	    break;
	case 92:
	    LookupColor(buf);
	    ReplyExpected(client, Request);
	    break;
	case 93:
	    CreateCursor(buf);
	    break;
	case 94:
	    CreateGlyphCursor(buf);
	    break;
	case 95:
	    FreeCursor(buf);
	    break;
	case 96:
	    RecolorCursor(buf);
	    break;
	case 97:
	    QueryBestSize(buf);
	    ReplyExpected(client, Request);
	    break;
	case 98:
	    QueryExtension(buf);
	    ReplyExpected(client, Request);
	    break;
	case 99:
	    ListExtensions(buf);
	    ReplyExpected(client, Request);
	    break;
	case 100:
	    ChangeKeyboardMapping(buf);
	    break;
	case 101:
	    GetKeyboardMapping(buf);
	    ReplyExpected(client, Request);
	    break;
	case 102:
	    ChangeKeyboardControl(buf);
	    break;
	case 103:
	    GetKeyboardControl(buf);
	    ReplyExpected(client, Request);
	    break;
	case 104:
	    Bell(buf);
	    break;
	case 105:
	    ChangePointerControl(buf);
	    break;
	case 106:
	    GetPointerControl(buf);
	    ReplyExpected(client, Request);
	    break;
	case 107:
	    SetScreenSaver(buf);
	    break;
	case 108:
	    GetScreenSaver(buf);
	    ReplyExpected(client, Request);
	    break;
	case 109:
	    ChangeHosts(buf);
	    break;
	case 110:
	    ListHosts(buf);
	    ReplyExpected(client, Request);
	    break;
	case 111:
	    SetAccessControl(buf);
	    break;
	case 112:
	    SetCloseDownMode(buf);
	    break;
	case 113:
	    KillClient(buf);
	    break;
	case 114:
	    RotateProperties(buf);
	    break;
	case 115:
	    ForceScreenSaver(buf);
	    break;
	case 116:
	    SetPointerMapping(buf);
	    ReplyExpected(client, Request);
	    break;
	case 117:
	    GetPointerMapping(buf);
	    ReplyExpected(client, Request);
	    break;
	case 118:
	    SetModifierMapping(buf);
	    ReplyExpected(client, Request);
	    break;
	case 119:
	    GetModifierMapping(buf);
	    ReplyExpected(client, Request);
	    break;
	case 127:
	    NoOperation(buf);
	    break;
	default:
	    fprintf(stdout, "####### Illegal request opcode (%d)\n", Request);
	    break;
	}
}

Global void
DecodeReply(server, buf, n)
    Server		    *server;
    unsigned char	    *buf;
    long		    n;
{
    int			    fd = server->fdd->fd;
    short		    SequenceNumber = IShort (&buf[2]);
    short		    Request;

    Request = CheckReplyTable(server, SequenceNumber, True);
    if
    (
	MonitoringRequests
	&&
	1 <= Request && Request <= 127
	&&
	VerboseRequest[Request]
    )
	CurrentVerbose = SelectedRequestVerbose;
    else
	CurrentVerbose = ReplyVerbose;
    if (Request == 0)
	return;
    if (CurrentVerbose <= 0)
	return;
    SetIndentLevel(PRINTSERVER);
    RBf[0] = Request /* for the PrintField in the Reply procedure */ ;
    if (CurrentVerbose > 3)
	DumpItem("Reply", fd, buf, n);
    if (Request <= 0 || 127 < Request)
	fprintf(stdout, "####### Extended reply opcode %d\n", Request);
    else
	switch (Request)
	{
	case 3:
	    GetWindowAttributesReply(buf);
	    break;
	case 14:
	    GetGeometryReply(buf);
	    break;
	case 15:
	    QueryTreeReply(buf);
	    break;
	case 16:
	    InternAtomReply(buf);
	    break;
	case 17:
	    GetAtomNameReply(buf);
	    break;
	case 20:
	    GetPropertyReply(buf);
	    break;
	case 21:
	    ListPropertiesReply(buf);
	    break;
	case 23:
	    GetSelectionOwnerReply(buf);
	    break;
	case 26:
	    GrabPointerReply(buf);
	    break;
	case 31:
	    GrabKeyboardReply(buf);
	    break;
	case 38:
	    QueryPointerReply(buf);
	    break;
	case 39:
	    GetMotionEventsReply(buf);
	    break;
	case 40:
	    TranslateCoordinatesReply(buf);
	    break;
	case 43:
	    GetInputFocusReply(buf);
	    break;
	case 44:
	    QueryKeymapReply(buf);
	    break;
	case 47:
	    QueryFontReply(buf);
	    break;
	case 48:
	    QueryTextExtentsReply(buf);
	    break;
	case 49:
	    ListFontsReply(buf);
	    break;
	case 50:
	    ListFontsWithInfoReply(buf);
	    break;
	case 52:
	    GetFontPathReply(buf);
	    break;
	case 73:
	    GetImageReply(buf);
	    break;
	case 83:
	    ListInstalledColormapsReply(buf);
	    break;
	case 84:
	    AllocColorReply(buf);
	    break;
	case 85:
	    AllocNamedColorReply(buf);
	    break;
	case 86:
	    AllocColorCellsReply(buf);
	    break;
	case 87:
	    AllocColorPlanesReply(buf);
	    break;
	case 91:
	    QueryColorsReply(buf);
	    break;
	case 92:
	    LookupColorReply(buf);
	    break;
	case 97:
	    QueryBestSizeReply(buf);
	    break;
	case 98:
	    QueryExtensionReply(buf);
	    break;
	case 99:
	    ListExtensionsReply(buf);
	    break;
	case 101:
	    GetKeyboardMappingReply(buf);
	    break;
	case 103:
	    GetKeyboardControlReply(buf);
	    break;
	case 106:
	    GetPointerControlReply(buf);
	    break;
	case 108:
	    GetScreenSaverReply(buf);
	    break;
	case 110:
	    ListHostsReply(buf);
	    break;
	case 116:
	    SetPointerMappingReply(buf);
	    break;
	case 117:
	    GetPointerMappingReply(buf);
	    break;
	case 118:
	    SetModifierMappingReply(buf);
	    break;
	case 119:
	    GetModifierMappingReply(buf);
	    break;
	default:
	    fprintf(stdout, "####### Unimplemented reply opcode %d\n",Request);
	    break;
	}
}

Global void
DecodeError(server, buf, n)
    Server		    *server;
    unsigned char	    *buf;
    long		    n;
{
    int			    fd = server->fdd->fd;
    short		    Error = IByte (&buf[1]);

    (void)CheckReplyTable (server, (short)IShort(&buf[2]), False);
    if (CountErrors)
    {
	if (Error < 0 || Error > MAX_ERROR)
	    fprintf(stdout, "####### Illegal error opcode %d\n", Error);
	else
	    ErrorCount[Error]++;
    }
    CurrentVerbose = ErrorVerbose;
    if (CurrentVerbose <= 0)
	return;
    SetIndentLevel(PRINTSERVER);
    if (CurrentVerbose > 3)
	DumpItem("Error", fd, buf, n);
    if (Error < 1 || Error > 17)
	fprintf(stdout, "####### Extended Error opcode %d\n", Error);
    else
	switch (Error)
	{
	case 1:
	    RequestError(buf);
	    break;
	case 2:
	    ValueError(buf);
	    break;
	case 3:
	    WindowError(buf);
	    break;
	case 4:
	    PixmapError(buf);
	    break;
	case 5:
	    AtomError(buf);
	    break;
	case 6:
	    CursorError(buf);
	    break;
	case 7:
	    FontError(buf);
	    break;
	case 8:
	    MatchError(buf);
	    break;
	case 9:
	    DrawableError(buf);
	    break;
	case 10:
	    AccessError(buf);
	    break;
	case 11:
	    AllocError(buf);
	    break;
	case 12:
	    ColormapError(buf);
	    break;
	case 13:
	    GContextError(buf);
	    break;
	case 14:
	    IDChoiceError(buf);
	    break;
	case 15:
	    NameError(buf);
	    break;
	case 16:
	    LengthError(buf);
	    break;
	case 17:
	    ImplementationError(buf);
	    break;
	default:
	    fprintf(stdout, "####### Unimplemented Error opcode %d\n", Error);
	    break;
	}
}

Global void
DecodeEvent(server, buf, n, real_event)
    Server		    *server;
    unsigned char	    *buf;
    long		    n;
    Bool		    real_event;
{
    short		    Event = IByte (&buf[0]) & 0x7f;

    if (real_event)
    {
	if (CountEvents)
	{
	    if (Event < 0 || Event >= MAX_EVENT)
		fprintf(stdout, "####### Illegal event opcode %d\n", Event);
	    else
		EventCount[Event]++;
	}

	if
	(
	    BlockingEvents
	    &&
	    2 <= Event && Event <= 34
	    &&
	    BlockEvent[Event]
	)
	    ignore_bytes = True;

	if
	(
	    MonitoringEvents
	    &&
	    2 <= Event && Event <= 34
	    &&
	    VerboseEvent[Event]
	)
	    CurrentVerbose = SelectedEventVerbose;
	else
	    CurrentVerbose = EventVerbose;
	if (CurrentVerbose <= 0)
	    return;
	SetIndentLevel(PRINTSERVER);
	if (CurrentVerbose > 3)
	    DumpItem("Event", server->fdd->fd, buf, n);
    }
    /*
    fprintf(stdout, "Client: %d\n", client->ClientNumber);
    */
    if (Event < 2 || Event > 34)
	fprintf(stdout, "####### Extended Event opcode %d\n", Event);
    else
	switch (Event)
	{
	case 2:
	    KeyPressEvent(buf);
	    break;
	case 3:
	    KeyReleaseEvent(buf);
	    break;
	case 4:
	    ButtonPressEvent(buf);
	    break;
	case 5:
	    ButtonReleaseEvent(buf);
	    break;
	case 6:
	    MotionNotifyEvent(buf);
	    break;
	case 7:
	    EnterNotifyEvent(buf);
	    break;
	case 8:
	    LeaveNotifyEvent(buf);
	    break;
	case 9:
	    FocusInEvent(buf);
	    break;
	case 10:
	    FocusOutEvent(buf);
	    break;
	case 11:
	    KeymapNotifyEvent(buf);
	    break;
	case 12:
	    ExposeEvent(buf);
	    break;
	case 13:
	    GraphicsExposureEvent(buf);
	    break;
	case 14:
	    NoExposureEvent(buf);
	    break;
	case 15:
	    VisibilityNotifyEvent(buf);
	    break;
	case 16:
	    CreateNotifyEvent(buf);
	    break;
	case 17:
	    DestroyNotifyEvent(buf);
	    break;
	case 18:
	    UnmapNotifyEvent(buf);
	    break;
	case 19:
	    MapNotifyEvent(buf);
	    break;
	case 20:
	    MapRequestEvent(buf);
	    break;
	case 21:
	    ReparentNotifyEvent(buf);
	    break;
	case 22:
	    ConfigureNotifyEvent(buf);
	    break;
	case 23:
	    ConfigureRequestEvent(buf);
	    break;
	case 24:
	    GravityNotifyEvent(buf);
	    break;
	case 25:
	    ResizeRequestEvent(buf);
	    break;
	case 26:
	    CirculateNotifyEvent(buf);
	    break;
	case 27:
	    CirculateRequestEvent(buf);
	    break;
	case 28:
	    PropertyNotifyEvent(buf);
	    break;
	case 29:
	    SelectionClearEvent(buf);
	    break;
	case 30:
	    SelectionRequestEvent(buf);
	    break;
	case 31:
	    SelectionNotifyEvent(buf);
	    break;
	case 32:
	    ColormapNotifyEvent(buf);
	    break;
	case 33:
	    ClientMessageEvent(buf);
	    break;
	case 34:
	    MappingNotifyEvent(buf);
	    break;
	default:
	    fprintf(stdout, "####### Unimplemented Event opcode %d\n", Event);
	    break;
	}
}

#ifdef NOT_YET
/*
 * KeepLastReplyExpected:
 *
 * another reply is expected for the same reply as we just had.	 This is
 * only used with ListFontsWithInfo
 */
Global void
KeepLastReplyExpected()
{
    SequencedReplyExpected(Lastfd, LastSequenceNumber, LastReplyType);
}
#endif

/*
 * SequencedReplyExpected:
 *
 * A reply is expected to the type of request given for the fd associated
 * with this one
 */
static void
SequencedReplyExpected(client, RequestType)
    Client		    *client;
    short		    RequestType;
{
    Server		    *server;
    long		    SequenceNumber = client->SequenceNumber;
    QueueEntry		    *p;

    if (ignore_bytes) /* We don't send request, so we won't get a reply */
	return;
    /* create a new queue entry */
    p = Tmalloc(QueueEntry);
    p->SequenceNumber = SequenceNumber;
    p->Request = RequestType;

    server = (Server *)(TopOfList(&client->server_list));
    appendToList(&server->reply_list, (Pointer)p);
}

/*
 * CheckReplyTable:
 *
 * search for the type of request that is associated with a reply to the
 * given sequence number for this fd
 */

static short
CheckReplyTable (server, SequenceNumber, checkZero)
    Server		    *server;
    short		    SequenceNumber;
    Bool		    checkZero;
{
    int			    fd = server->fdd->fd;
    QueueEntry		    *p;

    ForAllInList(&server->reply_list)
    {
	p = (QueueEntry *)CurrentContentsOfList(&server->reply_list);
	if (SequenceNumber == ((short)(0xFFFF & p->SequenceNumber)))
	{
	    /* save the Request type */
	    Lastfd = fd;
	    LastSequenceNumber = p->SequenceNumber;
	    LastReplyType = p->Request;
	    /* pull the queue entry out of the queue for this fd */
	    freeCurrent(&server->reply_list);
	    return(LastReplyType);
	}
    }

    /* not expecting a reply for that sequence number */
    if (checkZero)
    {
	fprintf(stdout, "Unexpected reply, SequenceNumber: %d",SequenceNumber);
	if (ListIsEmpty(&server->reply_list))
	    fprintf(stdout, "No expected replies\n");
	else
	{
	    fprintf(stdout, "Expected replies are:\n");
	    ForAllInList(&server->reply_list)
	    {
		p = (QueueEntry *)CurrentContentsOfList(&server->reply_list);
		fprintf
		(
		    stdout, "Reply on fd %d for sequence %d is type %d\n",
		    fd, p->SequenceNumber, p->Request
		);
	    }
	    fprintf(stdout, "End of expected replies\n");
	}
    }
    return(0);
}


/*
 * ReplyExpected:
 *
 * A reply is expected to the type of request given for the sequence
 * number associated with this fd
 */
static void
ReplyExpected(client, Request)
    Client		    *client;
    short		    Request;
{
    SequencedReplyExpected(client, Request);
}
