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
 * File: xmond.h
 *
 */

#ifndef XMOND_H
#define XMOND_H

#include <stdio.h>

#include    "common.h"
#include "linkl.h"

#define debug(n,f) (void)((debuglevel & n) ? (fprintf f,fflush(stderr)) : 0)

typedef struct
{
    char		    *data;
    long		    num_Saved;
    long		    num_Needed;
    long		    BlockSize;
}
    Buffer;

/*
 * File descriptors are grouped into four classes:
 *   standard input: one FD for reading from standard input
 *   X port: one FD for listening for new client connections
 *   clients: FDs connected to clients
 *   servers: FDs connected to servers
 * When input is read from a file desccriptor, an InputHandler function
 * is called to deal with the data just read. Each FD class has its own
 * input handler.
 *
 * The InputHandler called and the meaning of the private_data field for
 * each FD class is:
 *
 *   FD class		InputHandler	    private_data
 *   --------		------------	    ------------
 *   standard input	ReadStdin	    FD of stdin
 *   X port		NewConnection	    FD of X port
 *   clients		DataFromClient	    pointer to Client struct
 *   servers		DataFromServer	    pointer to Server struct
 */

typedef struct
{
    VoidCallback	    InputHandler;
    Pointer		    private_data;
    int			    fd;
    Buffer		    inBuffer;
    Buffer		    outBuffer;
    IntCallback		    ByteProcessing;
    Bool		    littleEndian;
}
    FDDescriptor;

typedef struct
{
    FDDescriptor	    *fdd;
    LinkList		    server_list;	/* list of Server */
    long		    SequenceNumber;
    long		    ClientNumber;
}
    Client;

typedef struct
{
    FDDescriptor	    *fdd;
    Client		    *client;
    LinkList		    reply_list;		/* list of QueueEntry */
}
    Server;

/* function prototypes: */
/* decode11.c: */
Global void InitRequestCount P((void ));
Global void ClearRequestCount P((void ));
Global void StartRequestCount P((void ));
Global void EndRequestCount P((void ));
Global void PrintRequestCounts P((void ));
Global void PrintZeroRequestCounts P((void ));
Global void InitEventCount P((void ));
Global void ClearEventCount P((void ));
Global void StartEventCount P((void ));
Global void EndEventCount P((void ));
Global void PrintEventCounts P((void ));
Global void PrintZeroEventCounts P((void ));
Global void InitErrorCount P((void ));
Global void ClearErrorCount P((void ));
Global void StartErrorCount P((void ));
Global void EndErrorCount P((void ));
Global void PrintErrorCounts P((void ));
Global void PrintZeroErrorCounts P((void ));
Global void DecodeRequest P((Client *client , unsigned char *buf , long n ));
Global void DecodeReply P((Server *server , unsigned char *buf , long n ));
Global void DecodeError P((Server *server , unsigned char *buf , long n ));
Global void DecodeEvent P((Server *server , unsigned char *buf , long n ,
Bool real_event ));
Global void KeepLastReplyExpected P((void ));
/* print11.c: */
Global void PrintSetUpMessage P((unsigned char *buf ));
Global void PrintSetUpReply P((unsigned char *buf ));
Global void RequestError P((unsigned char *buf ));
Global void ValueError P((unsigned char *buf ));
Global void WindowError P((unsigned char *buf ));
Global void PixmapError P((unsigned char *buf ));
Global void AtomError P((unsigned char *buf ));
Global void CursorError P((unsigned char *buf ));
Global void FontError P((unsigned char *buf ));
Global void MatchError P((unsigned char *buf ));
Global void DrawableError P((unsigned char *buf ));
Global void AccessError P((unsigned char *buf ));
Global void AllocError P((unsigned char *buf ));
Global void ColormapError P((unsigned char *buf ));
Global void GContextError P((unsigned char *buf ));
Global void IDChoiceError P((unsigned char *buf ));
Global void NameError P((unsigned char *buf ));
Global void LengthError P((unsigned char *buf ));
Global void ImplementationError P((unsigned char *buf ));
Global void KeyPressEvent P((unsigned char *buf ));
Global void KeyReleaseEvent P((unsigned char *buf ));
Global void ButtonPressEvent P((unsigned char *buf ));
Global void ButtonReleaseEvent P((unsigned char *buf ));
Global void MotionNotifyEvent P((unsigned char *buf ));
Global void EnterNotifyEvent P((unsigned char *buf ));
Global void LeaveNotifyEvent P((unsigned char *buf ));
Global void FocusInEvent P((unsigned char *buf ));
Global void FocusOutEvent P((unsigned char *buf ));
Global void KeymapNotifyEvent P((unsigned char *buf ));
Global void ExposeEvent P((unsigned char *buf ));
Global void GraphicsExposureEvent P((unsigned char *buf ));
Global void NoExposureEvent P((unsigned char *buf ));
Global void VisibilityNotifyEvent P((unsigned char *buf ));
Global void CreateNotifyEvent P((unsigned char *buf ));
Global void DestroyNotifyEvent P((unsigned char *buf ));
Global void UnmapNotifyEvent P((unsigned char *buf ));
Global void MapNotifyEvent P((unsigned char *buf ));
Global void MapRequestEvent P((unsigned char *buf ));
Global void ReparentNotifyEvent P((unsigned char *buf ));
Global void ConfigureNotifyEvent P((unsigned char *buf ));
Global void ConfigureRequestEvent P((unsigned char *buf ));
Global void GravityNotifyEvent P((unsigned char *buf ));
Global void ResizeRequestEvent P((unsigned char *buf ));
Global void CirculateNotifyEvent P((unsigned char *buf ));
Global void CirculateRequestEvent P((unsigned char *buf ));
Global void PropertyNotifyEvent P((unsigned char *buf ));
Global void SelectionClearEvent P((unsigned char *buf ));
Global void SelectionRequestEvent P((unsigned char *buf ));
Global void SelectionNotifyEvent P((unsigned char *buf ));
Global void ColormapNotifyEvent P((unsigned char *buf ));
Global void ClientMessageEvent P((unsigned char *buf ));
Global void MappingNotifyEvent P((unsigned char *buf ));
Global void CreateWindow P((unsigned char *buf ));
Global void ChangeWindowAttributes P((unsigned char *buf ));
Global void GetWindowAttributes P((unsigned char *buf ));
Global void GetWindowAttributesReply P((unsigned char *buf ));
Global void DestroyWindow P((unsigned char *buf ));
Global void DestroySubwindows P((unsigned char *buf ));
Global void ChangeSaveSet P((unsigned char *buf ));
Global void ReparentWindow P((unsigned char *buf ));
Global void MapWindow P((unsigned char *buf ));
Global void MapSubwindows P((unsigned char *buf ));
Global void UnmapWindow P((unsigned char *buf ));
Global void UnmapSubwindows P((unsigned char *buf ));
Global void ConfigureWindow P((unsigned char *buf ));
Global void CirculateWindow P((unsigned char *buf ));
Global void GetGeometry P((unsigned char *buf ));
Global void GetGeometryReply P((unsigned char *buf ));
Global void QueryTree P((unsigned char *buf ));
Global void QueryTreeReply P((unsigned char *buf ));
Global void InternAtom P((unsigned char *buf ));
Global void InternAtomReply P((unsigned char *buf ));
Global void GetAtomName P((unsigned char *buf ));
Global void GetAtomNameReply P((unsigned char *buf ));
Global void ChangeProperty P((unsigned char *buf ));
Global void DeleteProperty P((unsigned char *buf ));
Global void GetProperty P((unsigned char *buf ));
Global void GetPropertyReply P((unsigned char *buf ));
Global void ListProperties P((unsigned char *buf ));
Global void ListPropertiesReply P((unsigned char *buf ));
Global void SetSelectionOwner P((unsigned char *buf ));
Global void GetSelectionOwner P((unsigned char *buf ));
Global void GetSelectionOwnerReply P((unsigned char *buf ));
Global void ConvertSelection P((unsigned char *buf ));
Global void SendEvent P((unsigned char *buf ));
Global void GrabPointer P((unsigned char *buf ));
Global void GrabPointerReply P((unsigned char *buf ));
Global void UngrabPointer P((unsigned char *buf ));
Global void GrabButton P((unsigned char *buf ));
Global void UngrabButton P((unsigned char *buf ));
Global void ChangeActivePointerGrab P((unsigned char *buf ));
Global void GrabKeyboard P((unsigned char *buf ));
Global void GrabKeyboardReply P((unsigned char *buf ));
Global void UngrabKeyboard P((unsigned char *buf ));
Global void GrabKey P((unsigned char *buf ));
Global void UngrabKey P((unsigned char *buf ));
Global void AllowEvents P((unsigned char *buf ));
Global void GrabServer P((unsigned char *buf ));
Global void UngrabServer P((unsigned char *buf ));
Global void QueryPointer P((unsigned char *buf ));
Global void QueryPointerReply P((unsigned char *buf ));
Global void GetMotionEvents P((unsigned char *buf ));
Global void GetMotionEventsReply P((unsigned char *buf ));
Global void TranslateCoordinates P((unsigned char *buf ));
Global void TranslateCoordinatesReply P((unsigned char *buf ));
Global void WarpPointer P((unsigned char *buf ));
Global void SetInputFocus P((unsigned char *buf ));
Global void GetInputFocus P((unsigned char *buf ));
Global void GetInputFocusReply P((unsigned char *buf ));
Global void QueryKeymap P((unsigned char *buf ));
Global void QueryKeymapReply P((unsigned char *buf ));
Global void OpenFont P((unsigned char *buf ));
Global void CloseFont P((unsigned char *buf ));
Global void QueryFont P((unsigned char *buf ));
Global void QueryFontReply P((unsigned char *buf ));
Global void QueryTextExtents P((unsigned char *buf ));
Global void QueryTextExtentsReply P((unsigned char *buf ));
Global void ListFonts P((unsigned char *buf ));
Global void ListFontsReply P((unsigned char *buf ));
Global void ListFontsWithInfo P((unsigned char *buf ));
Global void ListFontsWithInfoReply P((unsigned char *buf ));
Global void ListFontsWithInfoReply1 P((unsigned char *buf ));
Global void ListFontsWithInfoReply2 P((unsigned char *buf ));
Global void SetFontPath P((unsigned char *buf ));
Global void GetFontPath P((unsigned char *buf ));
Global void GetFontPathReply P((unsigned char *buf ));
Global void CreatePixmap P((unsigned char *buf ));
Global void FreePixmap P((unsigned char *buf ));
Global void CreateGC P((unsigned char *buf ));
Global void ChangeGC P((unsigned char *buf ));
Global void CopyGC P((unsigned char *buf ));
Global void SetDashes P((unsigned char *buf ));
Global void SetClipRectangles P((unsigned char *buf ));
Global void FreeGC P((unsigned char *buf ));
Global void ClearArea P((unsigned char *buf ));
Global void CopyArea P((unsigned char *buf ));
Global void CopyPlane P((unsigned char *buf ));
Global void PolyPoint P((unsigned char *buf ));
Global void PolyLine P((unsigned char *buf ));
Global void PolySegment P((unsigned char *buf ));
Global void PolyRectangle P((unsigned char *buf ));
Global void PolyArc P((unsigned char *buf ));
Global void FillPoly P((unsigned char *buf ));
Global void PolyFillRectangle P((unsigned char *buf ));
Global void PolyFillArc P((unsigned char *buf ));
Global void PutImage P((unsigned char *buf ));
Global void GetImage P((unsigned char *buf ));
Global void GetImageReply P((unsigned char *buf ));
Global void PolyText8 P((unsigned char *buf ));
Global void PolyText16 P((unsigned char *buf ));
Global void ImageText8 P((unsigned char *buf ));
Global void ImageText16 P((unsigned char *buf ));
Global void CreateColormap P((unsigned char *buf ));
Global void FreeColormap P((unsigned char *buf ));
Global void CopyColormapAndFree P((unsigned char *buf ));
Global void InstallColormap P((unsigned char *buf ));
Global void UninstallColormap P((unsigned char *buf ));
Global void ListInstalledColormaps P((unsigned char *buf ));
Global void ListInstalledColormapsReply P((unsigned char *buf ));
Global void AllocColor P((unsigned char *buf ));
Global void AllocColorReply P((unsigned char *buf ));
Global void AllocNamedColor P((unsigned char *buf ));
Global void AllocNamedColorReply P((unsigned char *buf ));
Global void AllocColorCells P((unsigned char *buf ));
Global void AllocColorCellsReply P((unsigned char *buf ));
Global void AllocColorPlanes P((unsigned char *buf ));
Global void AllocColorPlanesReply P((unsigned char *buf ));
Global void FreeColors P((unsigned char *buf ));
Global void StoreColors P((unsigned char *buf ));
Global void StoreNamedColor P((unsigned char *buf ));
Global void QueryColors P((unsigned char *buf ));
Global void QueryColorsReply P((unsigned char *buf ));
Global void LookupColor P((unsigned char *buf ));
Global void LookupColorReply P((unsigned char *buf ));
Global void CreateCursor P((unsigned char *buf ));
Global void CreateGlyphCursor P((unsigned char *buf ));
Global void FreeCursor P((unsigned char *buf ));
Global void RecolorCursor P((unsigned char *buf ));
Global void QueryBestSize P((unsigned char *buf ));
Global void QueryBestSizeReply P((unsigned char *buf ));
Global void QueryExtension P((unsigned char *buf ));
Global void QueryExtensionReply P((unsigned char *buf ));
Global void ListExtensions P((unsigned char *buf ));
Global void ListExtensionsReply P((unsigned char *buf ));
Global void ChangeKeyboardMapping P((unsigned char *buf ));
Global void GetKeyboardMapping P((unsigned char *buf ));
Global void GetKeyboardMappingReply P((unsigned char *buf ));
Global void ChangeKeyboardControl P((unsigned char *buf ));
Global void GetKeyboardControl P((unsigned char *buf ));
Global void GetKeyboardControlReply P((unsigned char *buf ));
Global void Bell P((unsigned char *buf ));
Global void ChangePointerControl P((unsigned char *buf ));
Global void GetPointerControl P((unsigned char *buf ));
Global void GetPointerControlReply P((unsigned char *buf ));
Global void SetScreenSaver P((unsigned char *buf ));
Global void GetScreenSaver P((unsigned char *buf ));
Global void GetScreenSaverReply P((unsigned char *buf ));
Global void ChangeHosts P((unsigned char *buf ));
Global void ListHosts P((unsigned char *buf ));
Global void ListHostsReply P((unsigned char *buf ));
Global void SetAccessControl P((unsigned char *buf ));
Global void SetCloseDownMode P((unsigned char *buf ));
Global void KillClient P((unsigned char *buf ));
Global void RotateProperties P((unsigned char *buf ));
Global void ForceScreenSaver P((unsigned char *buf ));
Global void SetPointerMapping P((unsigned char *buf ));
Global void SetPointerMappingReply P((unsigned char *buf ));
Global void GetPointerMapping P((unsigned char *buf ));
Global void GetPointerMappingReply P((unsigned char *buf ));
Global void SetModifierMapping P((unsigned char *buf ));
Global void SetModifierMappingReply P((unsigned char *buf ));
Global void GetModifierMapping P((unsigned char *buf ));
Global void GetModifierMappingReply P((unsigned char *buf ));
Global void NoOperation P((unsigned char *buf ));
/* server.c: */
Global void PrintTime P((void ));
Global int pad P((long n ));
Global unsigned long ILong P((unsigned char buf []));
Global unsigned short IShort P((unsigned char buf []));
Global unsigned short IByte P((unsigned char buf []));
Global Bool IBool P((unsigned char buf []));
Global void StartClientConnection P((Client *client ));
Global void StartServerConnection P((Server *server ));
/* fd.c: */
Global void InitializeFD P((void ));
Global FDDescriptor *UsingFD P((int fd , VoidCallback Handler , Pointer
private_data ));
Global void NotUsingFD P((int fd ));
Global void EOFonFD P((int fd ));
/* main.c: */
Global void CloseConnection P((Client *client ));
Global void enterprocedure P((char *s ));
Global void panic P((char *s ));
/* prtype.c: */
Global void SetIndentLevel P((short which ));
Global void ModifyIndentLevel P((short amount ));
Global void DumpItem P((char *name , int fd , unsigned char *buf , long n ));
Global void PrintINT8 P((unsigned char *buf ));
Global void PrintINT16 P((unsigned char *buf ));
Global void PrintINT32 P((unsigned char *buf ));
Global void PrintCARD8 P((unsigned char *buf ));
Global void PrintCARD16 P((unsigned char *buf ));
Global int PrintCARD32 P((unsigned char *buf ));
Global int PrintBYTE P((unsigned char *buf ));
Global int PrintCHAR8 P((unsigned char *buf ));
Global int PrintSTRING16 P((unsigned char *buf ));
Global int PrintSTR P((unsigned char *buf ));
Global int PrintWINDOW P((unsigned char *buf ));
Global void PrintWINDOWD P((unsigned char *buf ));
Global void PrintWINDOWNR P((unsigned char *buf ));
Global void PrintPIXMAP P((unsigned char *buf ));
Global void PrintPIXMAPNPR P((unsigned char *buf ));
Global void PrintPIXMAPC P((unsigned char *buf ));
Global void PrintCURSOR P((unsigned char *buf ));
Global void PrintFONT P((unsigned char *buf ));
Global void PrintGCONTEXT P((unsigned char *buf ));
Global int PrintCOLORMAP P((unsigned char *buf ));
Global void PrintCOLORMAPC P((unsigned char *buf ));
Global void PrintDRAWABLE P((unsigned char *buf ));
Global void PrintFONTABLE P((unsigned char *buf ));
Global int PrintATOM P((unsigned char *buf ));
Global void PrintATOMT P((unsigned char *buf ));
Global void PrintVISUALID P((unsigned char *buf ));
Global void PrintVISUALIDC P((unsigned char *buf ));
Global void PrintTIMESTAMP P((unsigned char *buf ));
Global void PrintRESOURCEID P((unsigned char *buf ));
Global int PrintKEYSYM P((unsigned char *buf ));
Global int PrintKEYCODE P((unsigned char *buf ));
Global void PrintKEYCODEA P((unsigned char *buf ));
Global void PrintBUTTON P((unsigned char *buf ));
Global void PrintBUTTONA P((unsigned char *buf ));
Global void PrintEVENTFORM P((unsigned char *buf ));
Global void PrintENUMERATED P((unsigned char *buf , short length ,
struct ValueListEntry *ValueList ));
Global void PrintSET P((unsigned char *buf , short length , struct
ValueListEntry *ValueList ));
Global void PrintField P((unsigned char *buf , short start , short
length , short FieldType , char *name ));
Global int PrintList P((unsigned char *buf , long number , short
ListType , char *name ));
Global void PrintListSTR P((unsigned char *buf , long number , char *name ));
Global int PrintBytes P((unsigned char buf [], long number , char *name ));
Global int PrintString8 P((unsigned char buf [], short number , char *name ));
Global int PrintString16 P((unsigned char buf [], short number , char
*name ));
Global void PrintValues P((unsigned char *control , short clength ,
short ctype , unsigned char *values , char *name ));
Global void PrintTextList8 P((unsigned char *buf , short length , char
*name ));
Global void PrintTextList16 P((unsigned char *buf , short length , char
*name ));
Global void DumpHexBuffer P((unsigned char *buf , long n ));
/* table11.c: */
Global void InitializeX11 P((void ));

/* end function prototypes */

#endif	/* XMOND_H */
