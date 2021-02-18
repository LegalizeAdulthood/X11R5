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
 * File: table11.c
 *
 * Description: Table initialization for X11 protocol
 *
 * To initialize for the X11 protocol, we need to create data structures
 * describing the data types used by X11.
 *
 * There are about 100-128 data types for X11.	This start with the simple
 * INT8, INT16, INT32 (byte, short, long), and the CARD8, CARD16, CARD32
 * (unsigned) and extend to records like RGB (a resource id, 3 color
 * values and a bitmask to select a subset of the 3 color values).  Each
 * data type has an assigned type index.  The type index identifies the
 * type (with a #define in x11.h) and is used to access an entry in an
 * array of type descriptors (TD).  Each type descriptor has the type
 * name, the kind of type, and a procedure to print an object of that
 * type.  The print procedure for a type <foo> is named Print<foo>.  The
 * kind of type is
 *
 * BUILTIN:	 one of the primitive types.
 *
 * ENUMERATED:	 value should be one of a small set of values.	This type
 * needs a list of allowed values (and their print names).
 *
 * SET:		 value is a bitmask of a small set of values.  Each value
 * is a one-bit mask (and its print name).
 *
 * RECORD:	 value is a record of fields of other types.
 *
 * The Type Descriptor array allows us to print a value if we know its
 * type (index) and the bytes in memory that are its value.
 *
 */

#include "common.h"

#include "xmond.h"
#include "x11.h"

/* function prototypes: */
/* table11.c: */
static TYPE DefineType P((short typeid , short class , char *name ,
IntCallback printproc ));
static void DefineEValue P((TYPE type , long value , char *name ));
static void DefineValues P((TYPE type , long value , int length , int
ctype , char *name ));
static void InitBuiltInTypes P((void ));
static void InitEnumeratedTypes P((void ));
static void InitSetTypes P((void ));
static void PrintCHAR2B P((unsigned char *buf ));
static int PrintPOINT P((unsigned char *buf ));
static int PrintRECTANGLE P((unsigned char *buf ));
static int PrintARC P((unsigned char *buf ));
static int PrintHOST P((unsigned char *buf ));
static int PrintTIMECOORD P((unsigned char *buf ));
static int PrintFONTPROP P((unsigned char *buf ));
static int PrintCHARINFO P((unsigned char *buf ));
static int PrintSEGMENT P((unsigned char *buf ));
static int PrintCOLORITEM P((unsigned char *buf ));
static int PrintRGB P((unsigned char *buf ));
static int PrintFORMAT P((unsigned char *buf ));
static int PrintSCREEN P((unsigned char *buf ));
static int PrintDEPTH P((unsigned char *buf ));
static int PrintVISUALTYPE P((unsigned char *buf ));
static void InitRecordTypes P((void ));
static void InitValuesTypes P((void ));

/* end function prototypes */

Global void
InitializeX11()
{
    InitBuiltInTypes();
    InitEnumeratedTypes();
    InitSetTypes();
    InitValuesTypes();
    InitRecordTypes();
}

static TYPE
DefineType(typeid, class, name, printproc)
    short		    typeid;
    short		    class;
    char		    *name;
    IntCallback		    printproc;
{
    TD[typeid].Name = name;
    TD[typeid].Type = class;
    TD[typeid].ValueList = NULL;
    TD[typeid].PrintProc = printproc;
    return(&TD[typeid]);
}

/*
 * define an Enumerated Value (or a Set Value)
 */
static void
DefineEValue(type, value, name)
	TYPE type;
	long	value;
	char   *name;
{
    struct ValueListEntry  *p;

    /* define the new value */
    p = (struct ValueListEntry *)malloc((long)(sizeof(struct ValueListEntry)));
    p->Name = name;
    p->Value = value;

    /* add an new value to the list. */
    if (type->ValueList == NULL || type->ValueList->Value > p->Value)
	{
	    p->Next = type->ValueList;
	    type->ValueList = p;
	}
    else
	{
	    /* keep the list sorted, smallest to largest */
	    struct ValueListEntry  *q = type->ValueList;
	    while (q->Next != NULL && q->Next->Value < p->Value)
		q = q->Next;
	    p->Next = q->Next;
	    q->Next = p;
	}
}

/*
 * a Values list is like an enumerated Value, but has a type and length in
 * addition to a value and name.  It is used to print a Values List
 *
 * A Values List is a bitmask (like a set), but if the bit is set on, then
 * we have an associated value.	 We need to know the length and type of
 * the associated value for each bit
 */
static void
DefineValues(type, value, length, ctype, name)
	TYPE type;
	long	value;
	char   *name;
{
    struct ValueListEntry  *p;

    p = (struct ValueListEntry *)malloc((long)(sizeof(struct ValueListEntry)));
    p->Name = name;
    p->Type = ctype;
    p->Length = length;
    p->Value = value;

    /* add an new value to the list. */
    if (type->ValueList == NULL || type->ValueList->Value > p->Value)
	{
	    p->Next = type->ValueList;
	    type->ValueList = p;
	}
    else
	{
	    /* keep the list sorted, smallest to largest  */
	    struct ValueListEntry  *q = type->ValueList;
	    while (q->Next != NULL && q->Next->Value < p->Value)
		q = q->Next;
	    p->Next = q->Next;
	    q->Next = p;
	}
}

static void
InitBuiltInTypes()
{
    (void) DefineType(INT8, BUILTIN, "INT8", PrintINT8);
    (void) DefineType(INT16, BUILTIN, "INT16", PrintINT16);
    (void) DefineType(INT32, BUILTIN, "INT32", PrintINT32);
    (void) DefineType(CARD8, BUILTIN, "CARD8", PrintCARD8);
    (void) DefineType(CARD16, BUILTIN, "CARD16", PrintCARD16);
    (void) DefineType(CARD32, BUILTIN, "CARD32", PrintCARD32);
    (void) DefineType(BYTE, BUILTIN, "BYTE", PrintBYTE);
    (void) DefineType(CHAR8, BUILTIN, "CHAR8", PrintCHAR8);
    (void) DefineType(STRING16, BUILTIN, "STRING16", PrintSTRING16);
    (void) DefineType(STR, BUILTIN, "STR", PrintSTR);
    (void) DefineType(WINDOW, BUILTIN, "WINDOW", PrintWINDOW);
    (void) DefineType(WINDOWD, BUILTIN, "WINDOWD", PrintWINDOWD);
    (void) DefineType(WINDOWNR, BUILTIN, "WINDOWNR", PrintWINDOWNR);
    (void) DefineType(PIXMAP, BUILTIN, "PIXMAP", PrintPIXMAP);
    (void) DefineType(PIXMAPNPR, BUILTIN, "PIXMAPNPR", PrintPIXMAPNPR);
    (void) DefineType(PIXMAPC, BUILTIN, "PIXMAPC", PrintPIXMAPC);
    (void) DefineType(CURSOR, BUILTIN, "CURSOR", PrintCURSOR);
    (void) DefineType(FONT, BUILTIN, "FONT", PrintFONT);
    (void) DefineType(GCONTEXT, BUILTIN, "GCONTEXT", PrintGCONTEXT);
    (void) DefineType(COLORMAP, BUILTIN, "COLORMAP", PrintCOLORMAP);
    (void) DefineType(COLORMAPC, BUILTIN, "COLORMAPC", PrintCOLORMAPC);
    (void) DefineType(DRAWABLE, BUILTIN, "DRAWABLE", PrintDRAWABLE);
    (void) DefineType(FONTABLE, BUILTIN, "FONTABLE", PrintFONTABLE);
    (void) DefineType(ATOM, BUILTIN, "ATOM", PrintATOM);
    (void) DefineType(ATOMT, BUILTIN, "ATOMT", PrintATOMT);
    (void) DefineType(VISUALID, BUILTIN, "VISUALID", PrintVISUALID);
    (void) DefineType(VISUALIDC, BUILTIN, "VISUALIDC", PrintVISUALIDC);
    (void) DefineType(TIMESTAMP, BUILTIN, "TIMESTAMP", PrintTIMESTAMP);
    (void) DefineType(RESOURCEID, BUILTIN, "RESOURCEID", PrintRESOURCEID);
    (void) DefineType(KEYSYM, BUILTIN, "KEYSYM", PrintKEYSYM);
    (void) DefineType(KEYCODE, BUILTIN, "KEYCODE", PrintKEYCODE);
    (void) DefineType(KEYCODEA, BUILTIN, "KEYCODEA", PrintKEYCODEA);
    (void) DefineType(BUTTON, BUILTIN, "BUTTON", PrintBUTTON);
    (void) DefineType(BUTTONA, BUILTIN, "BUTTONA", PrintBUTTONA);
    (void) DefineType(EVENTFORM, BUILTIN, "EVENTFORM", PrintEVENTFORM);
}

static void
InitEnumeratedTypes()
{
    TYPE p;

    p = DefineType(REQUEST, ENUMERATED, "REQUEST", PrintENUMERATED);
    DefineEValue(p, 1L, "CreateWindow");
    DefineEValue(p, 2L, "ChangeWindowAttributes");
    DefineEValue(p, 3L, "GetWindowAttributes");
    DefineEValue(p, 4L, "DestroyWindow");
    DefineEValue(p, 5L, "DestroySubwindows");
    DefineEValue(p, 6L, "ChangeSaveSet");
    DefineEValue(p, 7L, "ReparentWindow");
    DefineEValue(p, 8L, "MapWindow");
    DefineEValue(p, 9L, "MapSubwindows");
    DefineEValue(p, 10L, "UnmapWindow");
    DefineEValue(p, 11L, "UnmapSubwindows");
    DefineEValue(p, 12L, "ConfigureWindow");
    DefineEValue(p, 13L, "CirculateWindow");
    DefineEValue(p, 14L, "GetGeometry");
    DefineEValue(p, 15L, "QueryTree");
    DefineEValue(p, 16L, "InternAtom");
    DefineEValue(p, 17L, "GetAtomName");
    DefineEValue(p, 18L, "ChangeProperty");
    DefineEValue(p, 19L, "DeleteProperty");
    DefineEValue(p, 20L, "GetProperty");
    DefineEValue(p, 21L, "ListProperties");
    DefineEValue(p, 22L, "SetSelectionOwner");
    DefineEValue(p, 23L, "GetSelectionOwner");
    DefineEValue(p, 24L, "ConvertSelection");
    DefineEValue(p, 25L, "SendEvent");
    DefineEValue(p, 26L, "GrabPointer");
    DefineEValue(p, 27L, "UngrabPointer");
    DefineEValue(p, 28L, "GrabButton");
    DefineEValue(p, 29L, "UngrabButton");
    DefineEValue(p, 30L, "ChangeActivePointerGrab");
    DefineEValue(p, 31L, "GrabKeyboard");
    DefineEValue(p, 32L, "UngrabKeyboard");
    DefineEValue(p, 33L, "GrabKey");
    DefineEValue(p, 34L, "UngrabKey");
    DefineEValue(p, 35L, "AllowEvents");
    DefineEValue(p, 36L, "GrabServer");
    DefineEValue(p, 37L, "UngrabServer");
    DefineEValue(p, 38L, "QueryPointer");
    DefineEValue(p, 39L, "GetMotionEvents");
    DefineEValue(p, 40L, "TranslateCoordinates");
    DefineEValue(p, 41L, "WarpPointer");
    DefineEValue(p, 42L, "SetInputFocus");
    DefineEValue(p, 43L, "GetInputFocus");
    DefineEValue(p, 44L, "QueryKeymap");
    DefineEValue(p, 45L, "OpenFont");
    DefineEValue(p, 46L, "CloseFont");
    DefineEValue(p, 47L, "QueryFont");
    DefineEValue(p, 48L, "QueryTextExtents");
    DefineEValue(p, 49L, "ListFonts");
    DefineEValue(p, 50L, "ListFontsWithInfo");
    DefineEValue(p, 51L, "SetFontPath");
    DefineEValue(p, 52L, "GetFontPath");
    DefineEValue(p, 53L, "CreatePixmap");
    DefineEValue(p, 54L, "FreePixmap");
    DefineEValue(p, 55L, "CreateGC");
    DefineEValue(p, 56L, "ChangeGC");
    DefineEValue(p, 57L, "CopyGC");
    DefineEValue(p, 58L, "SetDashes");
    DefineEValue(p, 59L, "SetClipRectangles");
    DefineEValue(p, 60L, "FreeGC");
    DefineEValue(p, 61L, "ClearArea");
    DefineEValue(p, 62L, "CopyArea");
    DefineEValue(p, 63L, "CopyPlane");
    DefineEValue(p, 64L, "PolyPoint");
    DefineEValue(p, 65L, "PolyLine");
    DefineEValue(p, 66L, "PolySegment");
    DefineEValue(p, 67L, "PolyRectangle");
    DefineEValue(p, 68L, "PolyArc");
    DefineEValue(p, 69L, "FillPoly");
    DefineEValue(p, 70L, "PolyFillRectangle");
    DefineEValue(p, 71L, "PolyFillArc");
    DefineEValue(p, 72L, "PutImage");
    DefineEValue(p, 73L, "GetImage");
    DefineEValue(p, 74L, "PolyText8");
    DefineEValue(p, 75L, "PolyText16");
    DefineEValue(p, 76L, "ImageText8");
    DefineEValue(p, 77L, "ImageText16");
    DefineEValue(p, 78L, "CreateColormap");
    DefineEValue(p, 79L, "FreeColormap");
    DefineEValue(p, 80L, "CopyColormapAndFree");
    DefineEValue(p, 81L, "InstallColormap");
    DefineEValue(p, 82L, "UninstallColormap");
    DefineEValue(p, 83L, "ListInstalledColormaps");
    DefineEValue(p, 84L, "AllocColor");
    DefineEValue(p, 85L, "AllocNamedColor");
    DefineEValue(p, 86L, "AllocColorCells");
    DefineEValue(p, 87L, "AllocColorPlanes");
    DefineEValue(p, 88L, "FreeColors");
    DefineEValue(p, 89L, "StoreColors");
    DefineEValue(p, 90L, "StoreNamedColor");
    DefineEValue(p, 91L, "QueryColors");
    DefineEValue(p, 92L, "LookupColor");
    DefineEValue(p, 93L, "CreateCursor");
    DefineEValue(p, 94L, "CreateGlyphCursor");
    DefineEValue(p, 95L, "FreeCursor");
    DefineEValue(p, 96L, "RecolorCursor");
    DefineEValue(p, 97L, "QueryBestSize");
    DefineEValue(p, 98L, "QueryExtension");
    DefineEValue(p, 99L, "ListExtensions");
    DefineEValue(p, 100L, "ChangeKeyboardMapping");
    DefineEValue(p, 101L, "GetKeyboardMapping");
    DefineEValue(p, 102L, "ChangeKeyboardControl");
    DefineEValue(p, 103L, "GetKeyboardControl");
    DefineEValue(p, 104L, "Bell");
    DefineEValue(p, 105L, "ChangePointerControl");
    DefineEValue(p, 106L, "GetPointerControl");
    DefineEValue(p, 107L, "SetScreenSaver");
    DefineEValue(p, 108L, "GetScreenSaver");
    DefineEValue(p, 109L, "ChangeHosts");
    DefineEValue(p, 110L, "ListHosts");
    DefineEValue(p, 111L, "SetAccessControl");
    DefineEValue(p, 112L, "SetCloseDownMode");
    DefineEValue(p, 113L, "KillClient");
    DefineEValue(p, 114L, "RotateProperties");
    DefineEValue(p, 115L, "ForceScreenSaver");
    DefineEValue(p, 116L, "SetPointerMapping");
    DefineEValue(p, 117L, "GetPointerMapping");
    DefineEValue(p, 118L, "SetModifierMapping");
    DefineEValue(p, 119L, "GetModifierMapping");
    DefineEValue(p, 127L, "NoOperation");

    p = DefineType(REPLY, ENUMERATED, "REPLY", PrintENUMERATED);
    DefineEValue(p, 3L, "GetWindowAttributes");
    DefineEValue(p, 14L, "GetGeometry");
    DefineEValue(p, 15L, "QueryTree");
    DefineEValue(p, 16L, "InternAtom");
    DefineEValue(p, 17L, "GetAtomName");
    DefineEValue(p, 20L, "GetProperty");
    DefineEValue(p, 21L, "ListProperties");
    DefineEValue(p, 23L, "GetSelectionOwner");
    DefineEValue(p, 26L, "GrabPointer");
    DefineEValue(p, 31L, "GrabKeyboard");
    DefineEValue(p, 38L, "QueryPointer");
    DefineEValue(p, 39L, "GetMotionEvents");
    DefineEValue(p, 40L, "TranslateCoordinates");
    DefineEValue(p, 43L, "GetInputFocus");
    DefineEValue(p, 44L, "QueryKeymap");
    DefineEValue(p, 47L, "QueryFont");
    DefineEValue(p, 48L, "QueryTextExtents");
    DefineEValue(p, 49L, "ListFonts");
    DefineEValue(p, 50L, "ListFontsWithInfo");
    DefineEValue(p, 52L, "GetFontPath");
    DefineEValue(p, 73L, "GetImage");
    DefineEValue(p, 83L, "ListInstalledColormaps");
    DefineEValue(p, 84L, "AllocColor");
    DefineEValue(p, 85L, "AllocNamedColor");
    DefineEValue(p, 86L, "AllocColorCells");
    DefineEValue(p, 87L, "AllocColorPlanes");
    DefineEValue(p, 91L, "QueryColors");
    DefineEValue(p, 92L, "LookupColor");
    DefineEValue(p, 97L, "QueryBestSize");
    DefineEValue(p, 98L, "QueryExtension");
    DefineEValue(p, 99L, "ListExtensions");
    DefineEValue(p, 101L, "GetKeyboardMapping");
    DefineEValue(p, 103L, "GetKeyboardControl");
    DefineEValue(p, 106L, "GetPointerControl");
    DefineEValue(p, 108L, "GetScreenSaver");
    DefineEValue(p, 110L, "ListHosts");
    DefineEValue(p, 116L, "SetPointerMapping");
    DefineEValue(p, 117L, "GetPointerMapping");
    DefineEValue(p, 118L, "SetModifierMapping");
    DefineEValue(p, 119L, "GetModifierMapping");

    p = DefineType(ERROR, ENUMERATED, "ERROR", PrintENUMERATED);
    DefineEValue(p, 1L, "Request");
    DefineEValue(p, 2L, "Value");
    DefineEValue(p, 3L, "Window");
    DefineEValue(p, 4L, "Pixmap");
    DefineEValue(p, 5L, "Atom");
    DefineEValue(p, 6L, "Cursor");
    DefineEValue(p, 7L, "Font");
    DefineEValue(p, 8L, "Match");
    DefineEValue(p, 9L, "Drawable");
    DefineEValue(p, 10L, "Access");
    DefineEValue(p, 11L, "Alloc");
    DefineEValue(p, 12L, "Colormap");
    DefineEValue(p, 13L, "GContext");
    DefineEValue(p, 14L, "IDChoice");
    DefineEValue(p, 15L, "Name");
    DefineEValue(p, 16L, "Length");
    DefineEValue(p, 17L, "Implementation");

    p = DefineType(EVENT, ENUMERATED, "EVENT", PrintENUMERATED);
    DefineEValue(p, 2L, "KeyPress");
    DefineEValue(p, 3L, "KeyRelease");
    DefineEValue(p, 4L, "ButtonPress");
    DefineEValue(p, 5L, "ButtonRelease");
    DefineEValue(p, 6L, "MotionNotify");
    DefineEValue(p, 7L, "EnterNotify");
    DefineEValue(p, 8L, "LeaveNotify");
    DefineEValue(p, 9L, "FocusIn");
    DefineEValue(p, 10L, "FocusOut");
    DefineEValue(p, 11L, "KeymapNotify");
    DefineEValue(p, 12L, "Expose");
    DefineEValue(p, 13L, "GraphicsExposure");
    DefineEValue(p, 14L, "NoExposure");
    DefineEValue(p, 15L, "VisibilityNotify");
    DefineEValue(p, 16L, "CreateNotify");
    DefineEValue(p, 17L, "DestroyNotify");
    DefineEValue(p, 18L, "UnmapNotify");
    DefineEValue(p, 19L, "MapNotify");
    DefineEValue(p, 20L, "MapRequest");
    DefineEValue(p, 21L, "ReparentNotify");
    DefineEValue(p, 22L, "ConfigureNotify");
    DefineEValue(p, 23L, "ConfigureRequest");
    DefineEValue(p, 24L, "GravityNotify");
    DefineEValue(p, 25L, "ResizeRequest");
    DefineEValue(p, 26L, "CirculateNotify");
    DefineEValue(p, 27L, "CirculateRequest");
    DefineEValue(p, 28L, "PropertyNotify");
    DefineEValue(p, 29L, "SelectionClear");
    DefineEValue(p, 30L, "SelectionRequest");
    DefineEValue(p, 31L, "SelectionNotify");
    DefineEValue(p, 32L, "ColormapNotify");
    DefineEValue(p, 33L, "ClientMessage");
    DefineEValue(p, 34L, "MappingNotify");

    DefineEValue(p, 128L + 2L, "KeyPress");
    DefineEValue(p, 128L + 3L, "KeyRelease");
    DefineEValue(p, 128L + 4L, "ButtonPress");
    DefineEValue(p, 128L + 5L, "ButtonRelease");
    DefineEValue(p, 128L + 6L, "MotionNotify");
    DefineEValue(p, 128L + 7L, "EnterNotify");
    DefineEValue(p, 128L + 8L, "LeaveNotify");
    DefineEValue(p, 128L + 9L, "FocusIn");
    DefineEValue(p, 128L + 10L, "FocusOut");
    DefineEValue(p, 128L + 11L, "KeymapNotify");
    DefineEValue(p, 128L + 12L, "Expose");
    DefineEValue(p, 128L + 13L, "GraphicsExposure");
    DefineEValue(p, 128L + 14L, "NoExposure");
    DefineEValue(p, 128L + 15L, "VisibilityNotify");
    DefineEValue(p, 128L + 16L, "CreateNotify");
    DefineEValue(p, 128L + 17L, "DestroyNotify");
    DefineEValue(p, 128L + 18L, "UnmapNotify");
    DefineEValue(p, 128L + 19L, "MapNotify");
    DefineEValue(p, 128L + 20L, "MapRequest");
    DefineEValue(p, 128L + 21L, "ReparentNotify");
    DefineEValue(p, 128L + 22L, "ConfigureNotify");
    DefineEValue(p, 128L + 23L, "ConfigureRequest");
    DefineEValue(p, 128L + 24L, "GravityNotify");
    DefineEValue(p, 128L + 25L, "ResizeRequest");
    DefineEValue(p, 128L + 26L, "CirculateNotify");
    DefineEValue(p, 128L + 27L, "CirculateRequest");
    DefineEValue(p, 128L + 28L, "PropertyNotify");
    DefineEValue(p, 128L + 29L, "SelectionClear");
    DefineEValue(p, 128L + 30L, "SelectionRequest");
    DefineEValue(p, 128L + 31L, "SelectionNotify");
    DefineEValue(p, 128L + 32L, "ColormapNotify");
    DefineEValue(p, 128L + 33L, "ClientMessage");
    DefineEValue(p, 128L + 34L, "MappingNotify");

    p = DefineType(BITGRAVITY, ENUMERATED, "BITGRAVITY", PrintENUMERATED);
    DefineEValue(p, 0L, "Forget");
    DefineEValue(p, 1L, "NorthWest");
    DefineEValue(p, 2L, "North");
    DefineEValue(p, 3L, "NorthEast");
    DefineEValue(p, 4L, "West");
    DefineEValue(p, 5L, "Center");
    DefineEValue(p, 6L, "East");
    DefineEValue(p, 7L, "SouthWest");
    DefineEValue(p, 8L, "South");
    DefineEValue(p, 9L, "SouthEast");
    DefineEValue(p, 10L, "Static");

    p = DefineType(WINGRAVITY, ENUMERATED, "WINGRAVITY", PrintENUMERATED);
    DefineEValue(p, 0L, "Unmap");
    DefineEValue(p, 1L, "NorthWest");
    DefineEValue(p, 2L, "North");
    DefineEValue(p, 3L, "NorthEast");
    DefineEValue(p, 4L, "West");
    DefineEValue(p, 5L, "Center");
    DefineEValue(p, 6L, "East");
    DefineEValue(p, 7L, "SouthWest");
    DefineEValue(p, 8L, "South");
    DefineEValue(p, 9L, "SouthEast");
    DefineEValue(p, 10L, "Static");

    p = DefineType(BOOL, ENUMERATED, "BOOL", PrintENUMERATED);
    DefineEValue(p, 0L, "False");
    DefineEValue(p, 1L, "True");

    p = DefineType(HOSTFAMILY, ENUMERATED, "HOSTFAMILY", PrintENUMERATED);
    DefineEValue(p, 0L, "Internet");
    DefineEValue(p, 1L, "DECnet");
    DefineEValue(p, 2L, "Chaos");

    p = DefineType(PK_MODE, ENUMERATED, "PK_MODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Synchronous");
    DefineEValue(p, 1L, "Asynchronous");

    p = DefineType(NO_YES, ENUMERATED, "NO_YES", PrintENUMERATED);
    DefineEValue(p, 0L, "No");
    DefineEValue(p, 1L, "Yes");
    DefineEValue(p, 2L, "Default");

    p = DefineType(WINDOWCLASS, ENUMERATED, "WINDOWCLASS", PrintENUMERATED);
    DefineEValue(p, 0L, "CopyFromParent");
    DefineEValue(p, 1L, "InputOutput");
    DefineEValue(p, 2L, "InputOnly");

    p = DefineType(BACKSTORE, ENUMERATED, "BACKSTORE", PrintENUMERATED);
    DefineEValue(p, 0L, "NotUseful");
    DefineEValue(p, 1L, "WhenMapped");
    DefineEValue(p, 2L, "Always");

    p = DefineType(MAPSTATE, ENUMERATED, "MAPSTATE", PrintENUMERATED);
    DefineEValue(p, 0L, "Unmapped");
    DefineEValue(p, 1L, "Unviewable");
    DefineEValue(p, 2L, "Viewable");

    p = DefineType(STACKMODE, ENUMERATED, "STACKMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Above");
    DefineEValue(p, 1L, "Below");
    DefineEValue(p, 2L, "TopIf");
    DefineEValue(p, 3L, "BottomIf");
    DefineEValue(p, 4L, "Opposite");

    p = DefineType(CIRMODE, ENUMERATED, "CIRMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "RaiseLowest");
    DefineEValue(p, 1L, "LowerHighest");

    p = DefineType(CHANGEMODE, ENUMERATED, "CHANGEMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Replace");
    DefineEValue(p, 1L, "Prepend");
    DefineEValue(p, 2L, "Append");

    p = DefineType(GRABSTAT, ENUMERATED, "GRABSTAT", PrintENUMERATED);
    DefineEValue(p, 0L, "Success");
    DefineEValue(p, 1L, "AlreadyGrabbed");
    DefineEValue(p, 2L, "InvalidTime");
    DefineEValue(p, 3L, "NotViewable");
    DefineEValue(p, 4L, "Frozen");

    p = DefineType(EVENTMODE, ENUMERATED, "EVENTMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "AsyncPointer");
    DefineEValue(p, 1L, "SyncPointer");
    DefineEValue(p, 2L, "ReplayPointer");
    DefineEValue(p, 3L, "AsyncKeyboard");
    DefineEValue(p, 4L, "SyncKeyboard");
    DefineEValue(p, 5L, "ReplayKeyboard");
    DefineEValue(p, 6L, "AsyncBoth");
    DefineEValue(p, 7L, "SyncBoth");

    p = DefineType(FOCUSAGENT, ENUMERATED, "FOCUSAGENT", PrintENUMERATED);
    DefineEValue(p, 0L, "None");
    DefineEValue(p, 1L, "PointerRoot");
    DefineEValue(p, 2L, "Parent");

    p = DefineType(DIRECT, ENUMERATED, "DIRECT", PrintENUMERATED);
    DefineEValue(p, 0L, "LeftToRight");
    DefineEValue(p, 1L, "RightToLeft");

    p = DefineType(GCFUNC, ENUMERATED, "GCFUNC", PrintENUMERATED);
    DefineEValue(p, 0L, "Clear");
    DefineEValue(p, 1L, "And");
    DefineEValue(p, 2L, "AndReverse");
    DefineEValue(p, 3L, "Copy");
    DefineEValue(p, 4L, "AndInverted");
    DefineEValue(p, 5L, "Noop");
    DefineEValue(p, 6L, "Xor");
    DefineEValue(p, 7L, "Or");
    DefineEValue(p, 8L, "Nor");
    DefineEValue(p, 9L, "Equiv");
    DefineEValue(p, 10L, "Invert");
    DefineEValue(p, 11L, "OrReverse");
    DefineEValue(p, 12L, "CopyInverted");
    DefineEValue(p, 13L, "OrInverted");
    DefineEValue(p, 14L, "Nand");
    DefineEValue(p, 15L, "Set");

    p = DefineType(LINESTYLE, ENUMERATED, "LINESTYLE", PrintENUMERATED);
    DefineEValue(p, 0L, "Solid");
    DefineEValue(p, 1L, "OnOffDash");
    DefineEValue(p, 2L, "DoubleDash");

    p = DefineType(CAPSTYLE, ENUMERATED, "CAPSTYLE", PrintENUMERATED);
    DefineEValue(p, 0L, "NotLast");
    DefineEValue(p, 1L, "Butt");
    DefineEValue(p, 2L, "Round");
    DefineEValue(p, 3L, "Projecting");

    p = DefineType(JOINSTYLE, ENUMERATED, "JOINSTYLE", PrintENUMERATED);
    DefineEValue(p, 0L, "Miter");
    DefineEValue(p, 1L, "Round");
    DefineEValue(p, 2L, "Bevel");

    p = DefineType(FILLSTYLE, ENUMERATED, "FILLSTYLE", PrintENUMERATED);
    DefineEValue(p, 0L, "Solid");
    DefineEValue(p, 1L, "Tiled");
    DefineEValue(p, 2L, "Stippled");
    DefineEValue(p, 3L, "OpaqueStippled");

    p = DefineType(FILLRULE, ENUMERATED, "FILLRULE", PrintENUMERATED);
    DefineEValue(p, 0L, "EvenOdd");
    DefineEValue(p, 1L, "Winding");

    p = DefineType(SUBWINMODE, ENUMERATED, "SUBWINMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "ClipByChildren");
    DefineEValue(p, 1L, "IncludeInferiors");

    p = DefineType(ARCMODE, ENUMERATED, "ARCMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Chord");
    DefineEValue(p, 1L, "PieSlice");

    p = DefineType(RECTORDER, ENUMERATED, "RECTORDER", PrintENUMERATED);
    DefineEValue(p, 0L, "UnSorted");
    DefineEValue(p, 1L, "YSorted");
    DefineEValue(p, 2L, "YXSorted");
    DefineEValue(p, 3L, "YXBanded");

    p = DefineType(COORMODE, ENUMERATED, "COORMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Origin");
    DefineEValue(p, 1L, "Previous");

    p = DefineType(POLYSHAPE, ENUMERATED, "POLYSHAPE", PrintENUMERATED);
    DefineEValue(p, 0L, "Complex");
    DefineEValue(p, 1L, "Nonconvex");
    DefineEValue(p, 2L, "Convex");

    p = DefineType(IMAGEMODE, ENUMERATED, "IMAGEMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Bitmap");
    DefineEValue(p, 1L, "XYPixmap");
    DefineEValue(p, 2L, "ZPixmap");

    p = DefineType(ALLORNONE, ENUMERATED, "ALLORNONE", PrintENUMERATED);
    DefineEValue(p, 0L, "None");
    DefineEValue(p, 1L, "All");

    p = DefineType(OBJECTCLASS, ENUMERATED, "OBJECTCLASS", PrintENUMERATED);
    DefineEValue(p, 0L, "Cursor");
    DefineEValue(p, 1L, "Tile");
    DefineEValue(p, 2L, "Stipple");

    p = DefineType(OFF_ON, ENUMERATED, "OFF_ON", PrintENUMERATED);
    DefineEValue(p, 0L, "Off");
    DefineEValue(p, 1L, "On");
    DefineEValue(p, 2L, "Default");

    p = DefineType(INS_DEL, ENUMERATED, "INS_DEL", PrintENUMERATED);
    DefineEValue(p, 0L, "Insert");
    DefineEValue(p, 1L, "Delete");

    p = DefineType(DIS_EN, ENUMERATED, "DIS_EN", PrintENUMERATED);
    DefineEValue(p, 0L, "Disabled");
    DefineEValue(p, 1L, "Enabled");

    p = DefineType(CLOSEMODE, ENUMERATED, "CLOSEMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Destroy");
    DefineEValue(p, 1L, "RetainPermanent");
    DefineEValue(p, 2L, "RetainTemporary");

    p = DefineType(SAVEMODE, ENUMERATED, "SAVEMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Reset");
    DefineEValue(p, 1L, "Activate");

    p = DefineType(RSTATUS, ENUMERATED, "RSTATUS", PrintENUMERATED);
    DefineEValue(p, 0L, "Success");
    DefineEValue(p, 1L, "Busy");
    DefineEValue(p, 2L, "Failed");

    p = DefineType(MOTIONDETAIL, ENUMERATED, "MOTIONDETAIL", PrintENUMERATED);
    DefineEValue(p, 0L, "Normal");
    DefineEValue(p, 1L, "Hint");

    p = DefineType(ENTERDETAIL, ENUMERATED, "ENTERDETAIL", PrintENUMERATED);
    DefineEValue(p, 0L, "Ancestor");
    DefineEValue(p, 1L, "Virtual");
    DefineEValue(p, 2L, "Inferior");
    DefineEValue(p, 3L, "Nonlinear");
    DefineEValue(p, 4L, "NonlinearVirtual");
    DefineEValue(p, 5L, "Pointer");
    DefineEValue(p, 6L, "PointerRoot");
    DefineEValue(p, 7L, "None");

    p = DefineType(BUTTONMODE, ENUMERATED, "BUTTONMODE", PrintENUMERATED);
    DefineEValue(p, 0L, "Normal");
    DefineEValue(p, 1L, "Grab");
    DefineEValue(p, 2L, "Ungrab");
    DefineEValue(p, 3L, "WhileGrabbed");

    p = DefineType(VISIBLE, ENUMERATED, "VISIBLE", PrintENUMERATED);
    DefineEValue(p, 0L, "Unobscured");
    DefineEValue(p, 1L, "PartiallyObscured");
    DefineEValue(p, 2L, "FullyObscured");

    p = DefineType(CIRSTAT, ENUMERATED, "CIRSTAT", PrintENUMERATED);
    DefineEValue(p, 0L, "Top");
    DefineEValue(p, 1L, "Bottom");

    p = DefineType(PROPCHANGE, ENUMERATED, "PROPCHANGE", PrintENUMERATED);
    DefineEValue(p, 0L, "NewValue");
    DefineEValue(p, 1L, "Deleted");

    p = DefineType(CMAPCHANGE, ENUMERATED, "CMAPCHANGE", PrintENUMERATED);
    DefineEValue(p, 0L, "Uninstalled");
    DefineEValue(p, 1L, "Installed");

    p = DefineType(MAPOBJECT, ENUMERATED, "MAPOBJECT", PrintENUMERATED);
    DefineEValue(p, 0L, "Modifier");
    DefineEValue(p, 1L, "Keyboard");
    DefineEValue(p, 2L, "Pointer");

    p = DefineType(BYTEMODE, ENUMERATED, "BYTEMODE", PrintENUMERATED);
    DefineEValue(p, 0x42L, "MSB first");
    DefineEValue(p, 0x6CL, "LSB first");

    p = DefineType(BYTEORDER, ENUMERATED, "BYTEORDER", PrintENUMERATED);
    DefineEValue(p, 0L, "LSB first");
    DefineEValue(p, 1L, "MSB first");

    p = DefineType(COLORCLASS, ENUMERATED, "COLORCLASS", PrintENUMERATED);
    DefineEValue(p, 0L, "StaticGray");
    DefineEValue(p, 1L, "GrayScale");
    DefineEValue(p, 2L, "StaticColor");
    DefineEValue(p, 3L, "PseudoColor");
    DefineEValue(p, 4L, "TrueColor");
    DefineEValue(p, 5L, "DirectColor");
}

static void
InitSetTypes()
{
    TYPE p;

    p = DefineType(SETofEVENT, SET, "SETofEVENT", PrintSET);
    DefineEValue(p, 0x00000001L, "KeyPress");
    DefineEValue(p, 0x00000002L, "KeyRelease");
    DefineEValue(p, 0x00000004L, "ButtonPress");
    DefineEValue(p, 0x00000008L, "ButtonRelease");
    DefineEValue(p, 0x00000010L, "EnterWindow");
    DefineEValue(p, 0x00000020L, "LeaveWindow");
    DefineEValue(p, 0x00000040L, "PointerMotion");
    DefineEValue(p, 0x00000080L, "PointerMotionHint");
    DefineEValue(p, 0x00000100L, "Button1Motion");
    DefineEValue(p, 0x00000200L, "Button2Motion");
    DefineEValue(p, 0x00000400L, "Button3Motion");
    DefineEValue(p, 0x00000800L, "Button4Motion");
    DefineEValue(p, 0x00001000L, "Button5Motion");
    DefineEValue(p, 0x00002000L, "ButtonMotion");
    DefineEValue(p, 0x00004000L, "KeymapState");
    DefineEValue(p, 0x00008000L, "Exposure");
    DefineEValue(p, 0x00010000L, "VisibilityChange");
    DefineEValue(p, 0x00020000L, "StructureNotify");
    DefineEValue(p, 0x00040000L, "ResizeRedirect");
    DefineEValue(p, 0x00080000L, "SubstructureNotify");
    DefineEValue(p, 0x00100000L, "SubstructureRedirect");
    DefineEValue(p, 0x00200000L, "FocusChange");
    DefineEValue(p, 0x00400000L, "PropertyChange");
    DefineEValue(p, 0x00800000L, "ColormapChange");
    DefineEValue(p, 0x01000000L, "OwnerGrabButton");

    p = DefineType(SETofPOINTEREVENT, SET, "SETofPOINTEREVENT", PrintSET);
    DefineEValue(p, 0x00000004L, "ButtonPress");
    DefineEValue(p, 0x00000008L, "ButtonRelease");
    DefineEValue(p, 0x00000010L, "EnterWindow");
    DefineEValue(p, 0x00000020L, "LeaveWindow");
    DefineEValue(p, 0x00000040L, "PointerMotion");
    DefineEValue(p, 0x00000080L, "PointerMotionHint");
    DefineEValue(p, 0x00000100L, "Button1Motion");
    DefineEValue(p, 0x00000200L, "Button2Motion");
    DefineEValue(p, 0x00000400L, "Button3Motion");
    DefineEValue(p, 0x00000800L, "Button4Motion");
    DefineEValue(p, 0x00001000L, "Button5Motion");
    DefineEValue(p, 0x00002000L, "ButtonMotion");
    DefineEValue(p, 0x00004000L, "KeymapState");

    p = DefineType(SETofDEVICEEVENT, SET, "SETofDEVICEEVENT", PrintSET);
    DefineEValue(p, 0x00000001L, "KeyPress");
    DefineEValue(p, 0x00000002L, "KeyRelease");
    DefineEValue(p, 0x00000004L, "ButtonPress");
    DefineEValue(p, 0x00000008L, "ButtonRelease");
    DefineEValue(p, 0x00000040L, "PointerMotion");
    DefineEValue(p, 0x00000100L, "Button1Motion");
    DefineEValue(p, 0x00000200L, "Button2Motion");
    DefineEValue(p, 0x00000400L, "Button3Motion");
    DefineEValue(p, 0x00000800L, "Button4Motion");
    DefineEValue(p, 0x00001000L, "Button5Motion");
    DefineEValue(p, 0x00002000L, "ButtonMotion");

    p = DefineType(SETofKEYBUTMASK, SET, "SETofKEYBUTMASK", PrintSET);
    DefineEValue(p, 0x0001L, "Shift");
    DefineEValue(p, 0x0002L, "Lock");
    DefineEValue(p, 0x0004L, "Control");
    DefineEValue(p, 0x0008L, "Mod1");
    DefineEValue(p, 0x0010L, "Mod2");
    DefineEValue(p, 0x0020L, "Mod3");
    DefineEValue(p, 0x0040L, "Mod4");
    DefineEValue(p, 0x0080L, "Mod5");
    DefineEValue(p, 0x0100L, "Button1");
    DefineEValue(p, 0x0200L, "Button2");
    DefineEValue(p, 0x0400L, "Button3");
    DefineEValue(p, 0x0800L, "Button4");
    DefineEValue(p, 0x1000L, "Button5");

    p = DefineType(SETofKEYMASK, SET, "SETofKEYMASK", PrintSET);
    DefineEValue(p, 0x0001L, "Shift");
    DefineEValue(p, 0x0002L, "Lock");
    DefineEValue(p, 0x0004L, "Control");
    DefineEValue(p, 0x0008L, "Mod1");
    DefineEValue(p, 0x0010L, "Mod2");
    DefineEValue(p, 0x0020L, "Mod3");
    DefineEValue(p, 0x0040L, "Mod4");
    DefineEValue(p, 0x0080L, "Mod5");
    DefineEValue(p, 0x8000L, "AnyModifier");

    p = DefineType(COLORMASK, SET, "COLORMASK", PrintSET);
    DefineEValue(p, 0x01L, "do-red");
    DefineEValue(p, 0x02L, "do-green");
    DefineEValue(p, 0x04L, "do-blue");

    p = DefineType(SCREENFOCUS, SET, "SCREENFOCUS", PrintSET);
    DefineEValue(p, 0x01L, "focus");
    DefineEValue(p, 0x02L, "same-screen");
}

/*
 * Print Routines for builtin record types
 */

static void
PrintCHAR2B(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 1, CARD8, "byte1");
    PrintField(buf, 1, 1, CARD8, "byte2");
}

static int
PrintPOINT(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, INT16, "x");
    PrintField(buf, 2, 2, INT16, "y");
    return(4);
}

static int
PrintRECTANGLE(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, INT16, "x");
    PrintField(buf, 2, 2, INT16, "y");
    PrintField(buf, 4, 2, CARD16, "width");
    PrintField(buf, 6, 2, CARD16, "height");
    return(8);
}

static int
PrintARC(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, INT16, "x");
    PrintField(buf, 2, 2, INT16, "y");
    PrintField(buf, 4, 2, CARD16, "width");
    PrintField(buf, 6, 2, CARD16, "height");
    PrintField(buf, 8, 2, INT16, "angle1");
    PrintField(buf, 10, 2, INT16, "angle2");
    return(12);
}

static int
PrintHOST(buf)
	unsigned char *buf;
{
    short   n;
    PrintField(buf, 0, 1, HOSTFAMILY, "family");
    PrintField(buf, 2, 2, DVALUE2(n), "length of address");
    n = IShort(&buf[2]);
    PrintList(&buf[4], (long)n, BYTE, "address");
    return(pad((long)(4 + n)));
}

static int
PrintTIMECOORD(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 4, TIMESTAMP, "time");
    PrintField(buf, 4, 2, CARD16, "x");
    PrintField(buf, 6, 2, CARD16, "y");
    return(8);
}

static int
PrintFONTPROP(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 4, ATOM, "name");
    PrintField(buf, 4, 4, INT32, "value");
    return(8);
}

static int
PrintCHARINFO(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, INT16, "left-side-bearing");
    PrintField(buf, 2, 2, INT16, "right-side-bearing");
    PrintField(buf, 4, 2, INT16, "character-width");
    PrintField(buf, 6, 2, INT16, "ascent");
    PrintField(buf, 8, 2, INT16, "descent");
    PrintField(buf, 10, 2, CARD16, "attributes");
    return(12);
}

static int
PrintSEGMENT(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, INT16, "x1");
    PrintField(buf, 2, 2, INT16, "y1");
    PrintField(buf, 4, 2, INT16, "x2");
    PrintField(buf, 6, 2, INT16, "y2");
    return(8);
}

static int
PrintCOLORITEM(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 4, CARD32, "pixel");
    PrintField(buf, 4, 2, CARD16, "red");
    PrintField(buf, 6, 2, CARD16, "green");
    PrintField(buf, 8, 2, CARD16, "blue");
    PrintField(buf, 10, 1, COLORMASK, "component selector");
    return(12);
}

static int
PrintRGB(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 2, CARD16, "red");
    PrintField(buf, 2, 2, CARD16, "green");
    PrintField(buf, 4, 2, CARD16, "blue");
    return(8);
}

static int
PrintFORMAT(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 1, CARD8, "depth");
    PrintField(buf, 1, 1, CARD8, "bits-per-pixel");
    PrintField(buf, 2, 1, CARD8, "scanline-pad");
    return(8);
}

static int
PrintSCREEN(buf)
	unsigned char *buf;
{
    short   n /* number of elements in List of DEPTH */ ;
    long    m /* length (in bytes) of List of DEPTH */ ;

    PrintField(buf, 0, 4, WINDOW, "root");
    PrintField(buf, 4, 4, COLORMAP, "default-colormap");
    PrintField(buf, 8, 4, CARD32, "white-pixel");
    PrintField(buf, 12, 4, CARD32, "black-pixel");
    PrintField(buf, 16, 4, SETofEVENT, "current-input-masks");
    PrintField(buf, 20, 2, CARD16, "width-in-pixels");
    PrintField(buf, 22, 2, CARD16, "height-in-pixels");
    PrintField(buf, 24, 2, CARD16, "width-in-millimeters");
    PrintField(buf, 26, 2, CARD16, "height-in-millimeters");
    PrintField(buf, 28, 2, CARD16, "min-installed-maps");
    PrintField(buf, 30, 2, CARD16, "max-installed-maps");
    PrintField(buf, 32, 4, VISUALID, "root-visual");
    PrintField(buf, 36, 1, BACKSTORE, "backing-stores");
    PrintField(buf, 37, 1, BOOL, "save-unders");
    PrintField(buf, 38, 1, CARD8, "root-depth");
    PrintField(buf, 39, 1, CARD8, "number of allowed-depths");
    n = IByte(&buf[39]);
    m = PrintList(&buf[40], (long)n, DEPTH, "allowed-depths");
    return(40 + m);
}

static int
PrintDEPTH(buf)
	unsigned char *buf;
{
    short   n /* number of elements in List of VISUALTYPE */ ;
    short   m /* length (in bytes) of List of VISUALTYPE */ ;

    PrintField(buf, 0, 1, CARD8, "depth");
    PrintField(buf, 2, 2, DVALUE2(n), "number of visuals");
    n = IShort(&buf[2]);
    m = PrintList(&buf[8], (long)n, VISUALTYPE, "visuals");
    return(8 + m);
}

static int
PrintVISUALTYPE(buf)
	unsigned char *buf;
{
    PrintField(buf, 0, 4, VISUALID, "visual-id");
    PrintField(buf, 4, 1, COLORCLASS, "class");
    PrintField(buf, 5, 1, CARD8, "bits-per-rgb-value");
    PrintField(buf, 6, 2, CARD16, "colormap-entries");
    PrintField(buf, 8, 4, CARD32, "red-mask");
    PrintField(buf, 12, 4, CARD32, "green-mask");
    PrintField(buf, 16, 4, CARD32, "blue-mask");
    return(24);
}

static void
InitRecordTypes()
{
    (void) DefineType(CHAR2B, RECORD, "CHAR2B", PrintCHAR2B);
    (void) DefineType(POINT, RECORD, "POINT", PrintPOINT);
    (void) DefineType(RECTANGLE, RECORD, "RECTANGLE", PrintRECTANGLE);
    (void) DefineType(ARC, RECORD, "ARC", PrintARC);
    (void) DefineType(HOST, RECORD, "HOST", PrintHOST);
    (void) DefineType(TIMECOORD, RECORD, "TIMECOORD", PrintTIMECOORD);
    (void) DefineType(FONTPROP, RECORD, "FONTPROP", PrintFONTPROP);
    (void) DefineType(CHARINFO, RECORD, "CHARINFO", PrintCHARINFO);
    (void) DefineType(SEGMENT, RECORD, "SEGMENT", PrintSEGMENT);
    (void) DefineType(COLORITEM, RECORD, "COLORITEM", PrintCOLORITEM);
    (void) DefineType(RGB, RECORD, "RGB", PrintRGB);
    (void) DefineType(FORMAT, RECORD, "FORMAT", PrintFORMAT);
    (void) DefineType(SCREEN, RECORD, "SCREEN", PrintSCREEN);
    (void) DefineType(DEPTH, RECORD, "DEPTH", PrintDEPTH);
    (void) DefineType(VISUALTYPE, RECORD, "VISUALTYPE", PrintVISUALTYPE);
}

static void
InitValuesTypes()
{
    TYPE p;

    p = DefineType(WINDOW_BITMASK, SET, "WINDOW_BITMASK", PrintSET);

    DefineValues(p, 0x00000001L, 4, PIXMAPNPR, "background-pixmap");
    DefineValues(p, 0x00000002L, 4, CARD32, "background-pixel");
    DefineValues(p, 0x00000004L, 4, PIXMAPC, "border-pixmap");
    DefineValues(p, 0x00000008L, 4, CARD32, "border-pixel");
    DefineValues(p, 0x00000010L, 1, BITGRAVITY, "bit-gravity");
    DefineValues(p, 0x00000020L, 1, WINGRAVITY, "win-gravity");
    DefineValues(p, 0x00000040L, 1, BACKSTORE, "backing-store");
    DefineValues(p, 0x00000080L, 4, CARD32, "backing-planes");
    DefineValues(p, 0x00000100L, 4, CARD32, "backing-pixel");
    DefineValues(p, 0x00000200L, 1, BOOL, "override-redirect");
    DefineValues(p, 0x00000400L, 1, BOOL, "save-under");
    DefineValues(p, 0x00000800L, 4, SETofEVENT, "event-mask");
    DefineValues(p, 0x00001000L, 4, SETofDEVICEEVENT, "do-not-propagate-mask");
    DefineValues(p, 0x00002000L, 4, COLORMAPC, "colormap");
    DefineValues(p, 0x00004000L, 4, CURSOR, "cursor");

    p = DefineType(CONFIGURE_BITMASK, SET, "CONFIGURE_BITMASK", PrintSET);
    DefineValues(p, 0x0001L, 2, INT16, "x");
    DefineValues(p, 0x0002L, 2, INT16, "y");
    DefineValues(p, 0x0004L, 2, CARD16, "width");
    DefineValues(p, 0x0008L, 2, CARD16, "height");
    DefineValues(p, 0x0010L, 2, CARD16, "border-width");
    DefineValues(p, 0x0020L, 4, WINDOW, "sibling");
    DefineValues(p, 0x0040L, 1, STACKMODE, "stack-mode");

    p = DefineType(GC_BITMASK, SET, "GC_BITMASK", PrintSET);
    DefineValues(p, 0x00000001L, 1, GCFUNC, "function");
    DefineValues(p, 0x00000002L, 4, CARD32, "plane-mask");
    DefineValues(p, 0x00000004L, 4, CARD32, "foreground");
    DefineValues(p, 0x00000008L, 4, CARD32, "background");
    DefineValues(p, 0x00000010L, 2, CARD16, "line-width");
    DefineValues(p, 0x00000020L, 1, LINESTYLE, "line-style");
    DefineValues(p, 0x00000040L, 1, CAPSTYLE, "cap-style");
    DefineValues(p, 0x00000080L, 1, JOINSTYLE, "join-style");
    DefineValues(p, 0x00000100L, 1, FILLSTYLE, "fill-style");
    DefineValues(p, 0x00000200L, 1, FILLRULE, "fill-rule");
    DefineValues(p, 0x00000400L, 4, PIXMAP, "tile");
    DefineValues(p, 0x00000800L, 4, PIXMAP, "stipple");
    DefineValues(p, 0x00001000L, 2, INT16, "tile-stipple-x-origin");
    DefineValues(p, 0x00002000L, 2, INT16, "tile-stipple-y-origin");
    DefineValues(p, 0x00004000L, 4, FONT, "font");
    DefineValues(p, 0x00008000L, 1, SUBWINMODE, "subwindow-mode");
    DefineValues(p, 0x00010000L, 1, BOOL, "graphics-exposures");
    DefineValues(p, 0x00020000L, 2, INT16, "clip-x-origin");
    DefineValues(p, 0x00040000L, 2, INT16, "clip-y-origin");
    DefineValues(p, 0x00080000L, 4, PIXMAP, "clip-mask");
    DefineValues(p, 0x00100000L, 2, CARD16, "dash-offset");
    DefineValues(p, 0x00200000L, 1, CARD8, "dashes");
    DefineValues(p, 0x00400000L, 1, ARCMODE, "arc-mode");

    p = DefineType(KEYBOARD_BITMASK, SET, "KEYBOARD_BITMASK", PrintSET);
    DefineValues(p, 0x0001L, 1, INT8, "key-click-percent");
    DefineValues(p, 0x0002L, 1, INT8, "bell-percent");
    DefineValues(p, 0x0004L, 2, INT16, "bell-pitch");
    DefineValues(p, 0x0008L, 2, INT16, "bell-duration");
    DefineValues(p, 0x0010L, 1, CARD8, "led");
    DefineValues(p, 0x0020L, 1, OFF_ON, "led-mode");
    DefineValues(p, 0x0040L, 1, KEYCODE, "key");
    DefineValues(p, 0x0080L, 1, OFF_ON, "auto-repeat-mode");
}
