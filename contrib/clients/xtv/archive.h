/*
 * Copyright (c) 1991 Old Dominion University.
 * Copyright (c) 1991 University of North Carolina at Chapel Hill.
 * All rights reserved.
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Old Dominion University and the University of North
 * Carolina at Chapel Hill make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#ifndef ARCHIVE__H
#define ARCHIVE__H

extern void ChangeGCFunc();
extern void ChangeWindowAttributesFunc();
extern void CirculateWindowFunc();
extern void CloseFontFunc();
extern void ConfigureWindowFunc();
extern void CopyColormapAndFreeFunc();
extern void CopyGCFunc();
extern void CreateColormapFunc();
extern void CreateCursorFunc();
extern void CreateGCFunc();
extern void CreateGlyphCursorFunc();
extern void CreatePixmapFunc();
extern void CreateWindowFunc();
extern void DestroySubwindowsFunc();
extern void DestroyWindowFunc();
extern void DrawingFunc();
extern void FreeColormapFunc();
extern void FreeCursorFunc();
extern void FreeGCFunc();
extern void FreePixmapFunc();
extern void GrabFunc();
extern void InstallColormapFunc();
extern void MapSubwindowsFunc();
extern void MapWindowFunc();
extern void OpenFontFunc();
extern void PropertyFunc();
extern void RecolorCursorFunc();
extern void SaveAllFunc();
extern void SetClipRectanglesFunc();
extern void SetDashesFunc();
extern void SetFontPathFunc();
extern void SimpleColormapFunc();
extern void UninstallColormapFunc();
extern void UnmapSubwindowsFunc();
extern void UnmapWindowFunc();
extern void ReparentWindowFunc();

void SwapCreateWindow();
void SwapChangeWindowAttributes();
void SwapConfigureWindow();
void SwapCreateGC();
void SwapChangeGC();

typedef struct _OffsetEntry {
	int Offset;
	int size;
} OffsetEntry;

typedef struct _ArchiveTableEntry {
	char *Request;
	void (*Func)();
	OffsetEntry OffsetList[15];
	void (*SwapFunc)();
} ArchiveTableEntry;

/*
	ArchiveTable:
		This table contains the pointers to the functions to call
		whenever a new request comes from the client
		The first entry is for debugging and ease of reference.

*/

ArchiveTableEntry	ArchiveTable[128] = {
{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"CreateWindow",		CreateWindowFunc,
 {{2,2},{4,4},{8,4},{12,2},{14,2},{16,2},{18,2},{20,2},{22,2},{24,4},{28,4},{0,0}},				SwapCreateWindow},

{"ChangeWindowAttributes",	ChangeWindowAttributesFunc,
 {{2,2},{4,4},{8,4},{0,0}},	SwapChangeWindowAttributes},

{"GetWindowAttributes",		NULL,
 {{0,0}},			NULL},

{"DestroyWindow",		DestroyWindowFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"DestroySubwindows",		DestroySubwindowsFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"ChangeSaveSet",		NULL,
 {{0,0}},			NULL},

{"ReparentWindow",		ReparentWindowFunc,
 {{4,4},{8,4},{12,2},{14,2},{0,0}},	NULL},

{"MapWindow",			MapWindowFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"MapSubwindows",		MapSubwindowsFunc,
 {{4,4},{0,0}},			NULL},

{"UnmapWindow",			UnmapWindowFunc,
 {{4,4},{0,0}},			NULL},

{"UnmapSubwindows",		UnmapSubwindowsFunc,
 {{4,4},{0,0}},			NULL},

{"ConfigureWindow",		ConfigureWindowFunc,
 {{4,4},{8,2},{0,0}},		SwapConfigureWindow},

{"CirculateWindow",		CirculateWindowFunc,
 {{4,4},{0,0}},			NULL},

{"GetGeometry",			NULL,
 {{0,0}},			NULL},

{"QueryTree",			NULL,
 {{0,0}},			NULL},

{"InternAtom",			NULL,
 {{2,2},{4,2},{0,0}},		NULL},

{"GetAtomName",			NULL,
 {{0,0}},			NULL},

{"ChangeProperty",		PropertyFunc,
 {{4,4},{0,0}},			NULL},

{"DeleteProperty",		PropertyFunc,
 {{4,4},{0,0}},			NULL},

{"GetProperty",			NULL,
 {{0,0}},			NULL},

{"ListProperties",		NULL,
 {{0,0}},			NULL},

{"SetSelectionOwner",		SaveAllFunc,
 {{0,0}},			NULL},

{"GetSelectionOwner",		NULL,
 {{0,0}},			NULL},

{"ConvertSelection",		NULL,
 {{0,0}},			NULL},

{"SendEvent",			NULL,
 {{0,0}},			NULL},

{"GrabPointer",			NULL,
 {{0,0}},			NULL},

{"UngrabPointer",		NULL,
 {{0,0}},			NULL},

{"GrabButton",			GrabFunc,
 {{4,4},{16,4},{0,0}},		NULL},

{"UngrabButton",		GrabFunc,
 {{4,4},{0,0}},			NULL},

{"ChangeActivePointerGrab",	NULL,
 {{0,0}},			NULL},

{"GrabKeyboard",		NULL,
 {{0,0}},			NULL},

{"UngrabKeyboard",		NULL,
 {{0,0}},			NULL},

{"GrabKey",			GrabFunc,
 {{4,4},{0,0}},			NULL},

{"UngrabKey",			GrabFunc,
 {{4,4},{0,0}},			NULL},

{"AllowEvents",			NULL,
 {{0,0}},			NULL},

{"GrabServer",			NULL,
 {{0,0}},			NULL},

{"UngrabServer",		NULL,
 {{0,0}},			NULL},

{"QueryPointer",		NULL,
 {{0,0}},			NULL},

{"GetMotionEvents",		NULL,
 {{0,0}},			NULL},

{"TranslateCoordinates",	NULL,
 {{0,0}},			NULL},

{"WarpPointer",			NULL,
 {{0,0}},			NULL},

{"SetInputFocus",		SaveAllFunc,
 {{0,0}},			NULL},

{"GetInputFocus",		NULL,
 {{0,0}},			NULL},

{"QueryKeymap",			NULL,
 {{0,0}},			NULL},

{"OpenFont",			OpenFontFunc,
 {{4,4},{0,0}},			NULL},

{"CloseFont",			CloseFontFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"QueryFont",			NULL,
 {{0,0}},			NULL},

{"QueryTextExtents",		NULL,
 {{0,0}},			NULL},

{"ListFonts",			NULL,
 {{0,0}},			NULL},

{"ListFontsWithInfo",		NULL,
 {{0,0}},			NULL},

{"SetFontPath",			SetFontPathFunc,
 {{0,0}},			NULL},

{"GetFontPath",			NULL,
 {{0,0}},			NULL},

{"CreatePixmap",		CreatePixmapFunc,
 {{2,2},{4,4},{8,4},{12,2},{14,2},{0,0}},	NULL},

{"FreePixmap",			FreePixmapFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"CreateGC",			CreateGCFunc,
 {{2,2},{4,4},{8,4},{12,4},{0,0}},	SwapCreateGC},

{"ChangeGC",			ChangeGCFunc,
 {{2,2},{4,4},{8,4},{0,0}},	SwapChangeGC},

{"CopyGC",			CopyGCFunc,
 {{4,4},{8,4},{12,4},{0,0}},	NULL},

{"SetDashes",			SetDashesFunc,
 {{4,4},{0,0}},			NULL},

{"SetClipRectangles",		SetClipRectanglesFunc,
 {{4,4},{0,0}},			NULL},

{"FreeGC",			FreeGCFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"ClearArea",			NULL,
 {{0,0}},			NULL},

{"CopyArea",			DrawingFunc,
 {{8,4},{0,0}},			NULL},

{"CopyPlane",			DrawingFunc,
 {{8,4},{0,0}},			NULL},

{"PolyPoint",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyLine",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolySegment",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyRectangle",		DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyArc",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"FillPoly",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyFillRectangle",		DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyFillArc",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PutImage",			DrawingFunc,
 {{2,2},{4,4},{8,4},{12,2},{14,2},{16,2},{18,2},{0,0}},	NULL},

{"GetImage",			NULL,
 {{0,0}},			NULL},

{"PolyText8",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"PolyText16",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"ImageText8",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"ImageText16",			DrawingFunc,
 {{4,4},{0,0}},			NULL},

{"CreateColormap",		CreateColormapFunc,
 {{4,4},{8,4},{0,0}},		NULL},

{"FreeColormap",		FreeColormapFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"CopyColormapAndFree",		CopyColormapAndFreeFunc,
 {{4,4},{8,4},{0,0}},		NULL},

{"InstallColormap",		InstallColormapFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"UninstallColormap",		UninstallColormapFunc,
 {{4,4},{0,0}},			NULL},

{"ListInstalledColormaps",	NULL,
 {{0,0}},			NULL},

{"AllocColor",			SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"AllocNamedColor",		SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"AllocColorCells",		SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"AllocColorPlanes",		SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"FreeColors",			SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"StoreColors",			SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"StoreNamedColor",		SimpleColormapFunc,
 {{4,4},{0,0}},			NULL},

{"QueryColors",			NULL,
 {{0,0}},			NULL},

{"LookupColor",			NULL,
 {{0,0}},			NULL},

{"CreateCursor",		CreateCursorFunc,
 {{4,4},{8,4},{12,4},{0,0}},	NULL},

{"CreateGlyphCursor",		CreateGlyphCursorFunc,
 {{4,4},{8,4},{12,4},{0,0}},	NULL},

{"FreeCursor",			FreeCursorFunc,
 {{2,2},{4,4},{0,0}},		NULL},

{"RecolorCursor",		RecolorCursorFunc,
 {{4,4},{0,0}},			NULL},

{"QueryBestSize",		NULL,
 {{0,0}},			NULL},

{"QueryExtension",		NULL,
 {{0,0}},			NULL},

{"ListExtensions",		NULL,
 {{0,0}},			NULL},

{"ChangeKeyboardMapping",	SaveAllFunc,
 {{0,0}},			NULL},

{"GetKeyboardMapping",		NULL,
 {{0,0}},			NULL},

{"ChangeKeyboardControl",	SaveAllFunc,
 {{0,0}},			NULL},

{"GetKeyboardControl",		NULL,
 {{0,0}},			NULL},

{"Bell",			NULL,
 {{0,0}},			NULL},

{"ChangePointerControl",	SaveAllFunc,
 {{0,0}},			NULL},

{"GetPointerControl",		NULL,
 {{0,0}},			NULL},

{"SetScreenSaver",		SaveAllFunc,
 {{0,0}},			NULL},

{"GetScreenSaver",		NULL,
 {{0,0}},			NULL},

{"ChangeHosts",			SaveAllFunc,
 {{0,0}},			NULL},

{"ListHosts",			NULL,
 {{0,0}},			NULL},

{"SetAccessControl",		SaveAllFunc,
 {{0,0}},			NULL},

{"SetCloseDownMode",		SaveAllFunc,
 {{0,0}},			NULL},

{"KillClient",			SaveAllFunc,
 {{0,0}},			NULL},

{"RotateProperties",		PropertyFunc,
 {{4,4},{0,0}},			NULL},

{"ForceScreenSaver",		SaveAllFunc,
 {{0,0}},			NULL},

{"SetPointerMapping",		SaveAllFunc,
 {{0,0}},			NULL},

{"GetPointerMapping",		NULL,
 {{0,0}},			NULL},

{"SetModifierMapping",		SaveAllFunc,
 {{0,0}},			NULL},

{"GetModifierMapping",		NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NOT_DEFINED",			NULL,
 {{0,0}},			NULL},

{"NoOperation",			NULL,
 {{0,0}},			NULL}
};

#endif /* ARCHIVE__H */
