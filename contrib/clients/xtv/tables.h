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

/*
	Definition of X11 Packet Table

	Each entry in the packet table takes the following format:

		\* NNN *\ { "PacketName",	{ 1, 2, 3, 4, 5, 6, 7, 8, 9,10}, func }

		NNN		The element number of the array and also the X11 Major Opcode.  This is
				not actually part of the entry (it's commented), it's just there to help
				lost sheep.

		PacketName	The standard MIT name for the packet (used for debugging)

		{ 1..10 }	The positions of XIDs in the packet divided by 4, as is the usual format
				for things in X. If the number is negative, the XID in question may have
				the value NONE (0), in which case it does not need to be translated.  Be
				sure all entries are flush-left in the array and that all unused elements
				have zeros in them.  These will be multiplied by four once XTV starts.
				This avoids having to do extra multiplications while translating the
				packet, which takes up enough time as it is.  If there are no simple
				fixed translations to be done, make sure the first element is a zero.

		func		A pointer to a function to process the packet, NULL if none needed.


	A sample entry for the GetGeometry packet (Opcode #14)

		014	{ "GetGeometry",	SimpleReplace,	{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, NULL }


*/

	typedef struct _RequestTableEntry {
		char	PacketName[30];		/* MIT Standard Packet Name */
		int	OffsetToID[10];		/* Table of offsets to simple XIDs */
		void	(*ProcessFunc)();	/* Function to handle any special processing */
	} RequestTableEntry;

	/* xyzzy - Multiply XID positions by FOUR when the program starts up */
	RequestTableEntry	RequestTable[128] = {

	/*000*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*001*/	{ "CreateWindow",		{ 1, 2, -6, 0 },	XlateCreateWindow	},
	/*002*/	{ "ChangeWindowAttributes",	{ 1, 0 },		XlateChangeWindowAttr	},
	/*003*/	{ "GetWindowAttributes",	{ 1, 0 },		NULL		},
	/*004*/	{ "DestroyWindow",		{ 1, 0 },		NULL			},
	/*005*/	{ "DestroySubwindows",		{ 1, 0 },		NULL			},
	/*006*/	{ "ChangeSaveSet",		{ 1, 0 },		NULL			},
	/*007*/	{ "ReparentWindow",		{ 1, 2, 0 },		NULL			},
	/*008*/	{ "MapWindow",			{ 1, 0 },		NULL			},
	/*009*/	{ "MapSubwindows",		{ 1, 0 },		NULL			},
	/*010*/	{ "UnmapWindow",		{ 1, 0 },		NULL			},
	/*011*/	{ "UnmapSubwindows",		{ 1, 0 },		NULL			},
	/*012*/	{ "ConfigureWindow",		{ 1, 0 },		XlateConfigWindow	},
	/*013*/	{ "CirculateWindow",		{ 1, 0 },		NULL			},
	/*014*/	{ "GetGeometry",		{ 1, 0 },		NULL		},
	/*015*/	{ "QueryTree",			{ 1, 2, 3, 0 },		NULL		},

	/*016*/	{ "InternAtom",			{ 0 },			NULL			},
	/*017*/	{ "GetAtomName",		{ 1, 0 },		NULL		},
	/*018*/	{ "ChangeProperty",		{ 1, 2, 3, 0 },		NULL			},
	/*019*/	{ "DeleteProperty",		{ 1, 2, 0 },		NULL			},
	/*020*/	{ "GetProperty",		{ 1, 2, -3, 0 },	NULL			},
	/*021*/	{ "ListProperties",		{ 1, 0 },		NULL		},
	/*022*/	{ "SetSelectionOwner",		{ 1, 2, 0 },		NULL			},
	/*023*/	{ "GetSelectionOwner",		{ 1, 0 },		NULL		},
	/*024*/	{ "ConvertSelection",		{ 1, 2, 3, -4, 0 },	NULL			},

	/*025*/	{ "SendEvent",			{ -1, 0 },		NULL		},
	/*026*/	{ "GrabPointer",		{ 1, -4, -4, 0 },	NULL		},
	/*027*/	{ "UngrabPointer",		{ 0 },			NULL		},
	/*028*/	{ "GrabButton",			{ 1, -3, -4, 0 },	NULL		},
	/*029*/	{ "UngrabButton",		{ 1, 0 },		NULL		},
	/*030*/	{ "ChangeActivePointerGrab",	{ -1, 0 },		NULL		},
	/*031*/	{ "GrabKeyboard",		{ 1, 0 },		NULL		},
	/*032*/	{ "UngrabKeyboard",		{ 0 },			NULL		},
	/*033*/	{ "GrabKey",			{ 1, 0 },		NULL		},
	/*034*/	{ "UngrabKey",			{ 1, 0 },		NULL		},
	/*035*/	{ "AllowEvents",		{ 0 },			NULL			},
	/*036*/	{ "GrabServer",			{ 0 },			NULL		},
	/*037*/	{ "UngrabServer",		{ 0 },			NULL		},
	/*038*/	{ "QueryPointer",		{ 1, 0 },		NULL		},
	/*039*/	{ "GetMotionEvents",		{ 1, 0 },		NULL		},
	/*040*/	{ "TranslateCoordinates",	{ 1, 2, 0 },		NULL		},
	/*041*/	{ "WarpPointer",		{ -1, -2, 0 },		NULL		},
	/*042*/	{ "SetInputFocus",		{ -1, 0 },		NULL		},
	/*043*/	{ "GetInputFocus",		{ 0 },			NULL		},
	/*044*/	{ "QueryKeymap",		{ 0 },			NULL		},
	/*045*/	{ "OpenFont",			{ 1, 0 },		NULL			},

	/*046*/	{ "CloseFont",			{ 1, 0 },		NULL			},
	/*047*/	{ "QueryFont",			{ 1, 0 },		NULL		},
	/*048*/	{ "QueryTextExtents",		{ 1, 0 },		NULL		},
	/*049*/	{ "ListFonts",			{ 0 },			NULL		},
	/*050*/	{ "ListFontsWithInfo",		{ 0 },			NULL		},
	/*051*/	{ "SetFontPath",		{ 0 },			NULL			},
	/*052*/	{ "GetFontPath",		{ 0 },			NULL		},

	/*053*/	{ "CreatePixmap",		{ 1, 2, 0 },		XlateCreatePixmap	},
	/*054*/	{ "FreePixmap",			{ 1, 0 },		NULL			},
	/*055*/	{ "CreateGC",			{ 1, 2, 0 },		XlateCreateGC		},
	/*056*/	{ "CrangeGC",			{ 1, 0 },		XlateChangeGC		},
	/*057*/	{ "CopyGC",			{ 1, 2, 0 },		NULL			},
	/*058*/	{ "SetDashes",			{ 1, 0 },		NULL			},
	/*059*/	{ "SetClipRectangles",		{ 1, 0 },		NULL			},
	/*060*/	{ "FreeGC",			{ 1, 0 },		NULL			},

	/*061*/	{ "ClearArea",			{ 1, 0 },		NULL			},
	/*062*/	{ "CopyArea",			{ 1, 2, 3, 0 },		NULL			},
	/*063*/	{ "CopyPlane",			{ 1, 2, 3, 0},		NULL			},

	/*064*/	{ "PolyPoint",			{ 1, 2, 0 },		NULL			},
	/*065*/	{ "PolyLine",			{ 1, 2, 0 },		NULL			},
	/*066*/	{ "PolySegment",		{ 1, 2, 0 },		NULL			},
	/*067*/	{ "PolyRectangle",		{ 1, 2, 0 },		NULL			},
	/*068*/	{ "PolyArc",			{ 1, 2, 0 },		NULL			},
	/*069*/	{ "FillPoly",			{ 1, 2, 0 },		NULL			},
	/*070*/	{ "PolyFillRectangle",		{ 1, 2, 0 },		NULL			},
	/*071*/	{ "PolyFillArc",		{ 1, 2, 0 },		NULL			},

	/*072*/	{ "PutImage",			{ 1, 2, 0 },		XlatePutImage	},
	/*073*/	{ "GetImage",			{ 1, 0 },		NULL		},
	/*074*/	{ "PolyText8",			{ 1, 2, 0 },		NULL			},
	/*075*/	{ "PolyText16",			{ 1, 2, 0 },		NULL			},
	/*076*/	{ "ImageText8",			{ 1, 2, 0 }, 		NULL			},
	/*077*/	{ "ImageText16",		{ 1, 2, 0 }, 		NULL			},

	/*078*/	{ "CreateColormap",		{ 1, 2, 3, 0 },		NULL			},
	/*079*/	{ "FreeColormap",		{ 1, 0 },		NULL			},
	/*080*/	{ "CopyColormapAndFree",	{ 1, 2, 0 },		NULL			},
	/*081*/	{ "InstallColormap",		{ 1, 0 },		NULL			},
	/*082*/	{ "UninstallColormap",		{ 1, 0 },		NULL			},
	/*083*/	{ "ListInstalledColormaps",	{ 1, 0 },		NULL		},
	/*084*/	{ "AllocColor",			{ 1, 0 },		NULL			},
	/*085*/	{ "AllocNamedColor",		{ 1, 0 },		NULL			},
	/*086*/	{ "AllocColorCells",		{ 1, 0 },		NULL			},
	/*087*/	{ "AllocColorPlanes",		{ 1, 0 },		NULL			},
	/*088*/	{ "FreeColors",			{ 1, 0 },		NULL			},
	/*089*/	{ "StoreColors",		{ 1, 0 },		XlateStoreColors	},
	/*090*/	{ "StoreNamedColor",		{ 1, 0 },		NULL			},
	/*091*/	{ "QueryColors",		{ 1, 0 },		XlateQueryColors	},
	/*092*/	{ "LookupColor",		{ 1, 0 },		NULL		},

	/*093*/	{ "CreateCursor",		{ 1, 2, -3, 0 },	NULL			},
	/*094*/	{ "CreateGlyphCursor",		{ 1, 2, 3, 0 },		NULL			},
	/*095*/	{ "FreeCursor",			{ 1, 0 },		NULL			},
	/*096*/	{ "RecolorCursor",		{ 1, 0 },		NULL			},

	/*097*/	{ "QueryBestSize",		{ 1, 0 },		NULL		},
	/*098*/	{ "QueryExtension",		{ 0 },			NULL		},
	/*099*/	{ "ListExtensions",		{ 0 },			NULL		},

	/*100*/	{ "ChangeKeyboardMapping",	{ 0 },			NULL		},
	/*101*/	{ "GetKeyboardMapping",		{ 0 },			NULL		},
	/*102*/	{ "ChangeKeyboardControl",	{ 0 },			NULL		},
	/*103*/	{ "GetKeyboardControl",		{ 0 },			NULL		},
	/*104*/	{ "Bell",			{ 0 },			NULL			},
	/*105*/	{ "ChangePointerControl",	{ 0 },			NULL		},
	/*106*/	{ "GetPointerControl",		{ 0 },			NULL		},
	/*107*/	{ "SetScreenSaver",		{ 0 },			NULL		},
	/*108*/	{ "GetScreenSaver",		{ 0 },			NULL		},

	/*109*/	{ "ChangeHosts",		{ 0 },			NULL		},
	/*110*/	{ "ListHosts",			{ 0 },			NULL		},
	/*111*/	{ "SetAccessControl",		{ 0 },			NULL		},
	/*112*/	{ "SetCloseDownMode",		{ 0 },			NULL			},
	/*113*/	{ "KillClient",			{ -1, 0 },		NULL		},
	/*114*/	{ "RotateProperties",		{ 1, 0 },		XlateRotateProps},
	/*115*/	{ "ForceScreenSaver",		{ 0 },			NULL		},
	/*116*/	{ "SetPointerMapping",		{ 0 },			NULL		},

	/*117*/	{ "GetPointerMapping",		{ 0 },			NULL		},
	/*118*/	{ "SetModifierMapping",		{ 0 },			NULL		},
	/*119*/	{ "GetModifierMapping",		{ 0 },			NULL		},

        /*120*/ { "PopUpInfo",                  { 1, 0 },               NULL            },
	/*121*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*122*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*123*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*124*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*125*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*126*/	{ "NOT DEFINED",		{ 0 },			NULL		},
	/*127*/	{ "NoOperation",		{ 0 },			NULL		}
};

#define	MAX_PACKET	127

/*
	Definition of X11 Event Table

	Each entry in the packet table takes the following format:

	\*004*\	{ "EventName",	Propogation,	{1,..,10},	HasSN,	TSOfs,	HProc,	VProc	},

		NNN		The element number of the array and also the X11 Major Opcode.  This is
				not actually part of the entry (it's commented), it's just there to help
				lost sheep.

		EventName	The standard MIT name for the event (used for debugging)

		{ 1..10 }	The positions of XIDs in the packet divided by 4, as is the usual format
				for things in X. If the number is negative, the XID in question may have
				the value NONE (0), in which case it does not need to be translated.  Be
				sure all entries are flush-left in the array and that all unused elements
				have zeros in them.  These will be multiplied by four once XTV starts.
				This avoids having to do extra multiplications while translating the
				packet, which takes up enough time as it is.  If there are no simple
				fixed translations to be done, make sure the first element is a zero.

		HasSN		TRUE if the event has a sequence number in it

		TSOfs		Offset of a timestamp if one exists, 0 otherwise

		HProc, VProc	Pointers to Host-end and Viewer-end processing functions, NULL
				if none

	A sample entry for the ButtonPress event (Code 4):

	\*004*\	{ "ButtonPress",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},

*/

	typedef struct _EventTableEntry {
		char	EventName[30];		/* MIT Standard Event Name */
		BYTE	Propogation;		/* Propogation flag */
		int	OffsetToID[10];		/* Table of offsets to simple XIDs */
		Bool	HasSeqNumber;		/* TRUE if event has a sequence number */
		int	TimeStampOffset;	/* Offset to timestamp, 0 if none */
		void	(*HostProcessFunc)();	/* Function to handle any special processing */
		void	(*ViewerProcessFunc)();	/* Function to handle any special processing */
	} EventTableEntry;

/* Definitions of propogation values */

#define	NEVER	0x00	/* Never ever ever ever ever ever ever ever ever send one of these */
#define	TOKEN	0x01	/* Send only when the token is present */
#define	ALWAYS	0xff	/* Always send these events */
#define UNK	0x00	/* Undecided...  NEVER for now to avoid harm */

	EventTableEntry	EventTable[40] = {

	/* #	  Name			Prop    Offsets         HasSN	TS	HProc	VProc */

	/*000*/	{ "(ERROR)",		TOKEN,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*001*/	{ "(REPLY)",		TOKEN,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*002*/	{ "KeyPress",		TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},

	/*003*/	{ "KeyRelease",		TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*004*/	{ "ButtonPress",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*005*/	{ "ButtonRelease",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*006*/	{ "MotionNotify",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*007*/	{ "EnterNotify",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*008*/	{ "LeaveNotify",	TOKEN,	{2,3,-4,0},	TRUE,	1,	NULL,	NULL	},
	/*009*/	{ "FocusIn",		TOKEN,	{1,0},		FALSE,	0,	NULL,	NULL	},

	/*010*/	{ "FocusOut",		TOKEN,	{1,0},		FALSE,	0,	NULL,	NULL	},
	/*011*/	{ "KeymapNotify",	TOKEN,	{ 0 },		FALSE,	0,	NULL,	NULL	},
	/*012*/	{ "Expose",		ALWAYS,	{1,0},		FALSE,	0,	NULL,	NULL	},
	/*013*/	{ "GraphicsExpose",	ALWAYS,	{1,0},		FALSE,	0,	NULL,	NULL	},
	/*014*/	{ "NoExpose",		TOKEN,	{1,0},		FALSE,	0,	NULL,	NULL	},
	/*015*/	{ "VisibilityNotify",	TOKEN,	{1,0},		FALSE,	0,	NULL,	NULL	},
	/*016*/	{ "CreateNotify",	TOKEN,	{1,2,0},	FALSE,	0,	NULL,	NULL	},
	/*017*/	{ "DestroyNotify",	TOKEN,	{1,2,0},	FALSE,	0,	NULL,	NULL	},
	/*018*/	{ "UnmapNotify",	TOKEN,	{1,2,0},	FALSE,	0,	NULL,	NULL	},
	/*019*/	{ "MapNotify",		TOKEN,	{1,2,0},	FALSE,	0,	NULL,	NULL	},

	/*020*/	{ "MapRequest",		TOKEN,	{1,2,0},	FALSE,	0,	NULL,	NULL	},
	/*021*/	{ "ReparentNotify",	TOKEN,	{1,2,3,0},	FALSE,	0,	NULL,	NULL	},
	/*022*/	{ "ConfigureNotify",	TOKEN,	{1,2,-3,0},	TRUE,	0,	NULL,	NULL	},
	/*023*/	{ "ConfigureRequest",	TOKEN,	{1,2,-3,0},	TRUE,	0,	NULL,	NULL	},
	/*024*/	{ "GravityNotify",	TOKEN,	{1,2,0},	TRUE,	0,	NULL,	NULL	},
	/*025*/	{ "ResizeRequest",	TOKEN,	{1,0},		TRUE,	0,	NULL,	NULL	},
	/*026*/	{ "CirculateNotify",	TOKEN,	{1,2,3,0},	TRUE,	0,	NULL,	NULL	},
	/*027*/	{ "CirculateRequest",	TOKEN,	{1,2,0},	TRUE,	0,	NULL,	NULL	},
	/*028*/	{ "PropertyNotify",	TOKEN,	{1,2,0},	TRUE,	3,	NULL,	NULL	},
	/*029*/	{ "SelectionClear",	TOKEN,	{2,3,0},	TRUE,	1,	NULL,	NULL	},

	/*030*/	{ "SelectionRequest",	TOKEN,	{2,3,4,5,-6,0},	TRUE,	-1,	NULL,	NULL	},
	/*031*/	{ "SelectionNotify",	TOKEN,	{2,3,4,-5,0},	TRUE,	-1,	NULL,	NULL	},
	/*032*/	{ "ColormapNotify",	TOKEN,	{1,-2,0},	TRUE,	0,	NULL,	NULL	},
	/*033*/	{ "ClientMessage",	TOKEN,	{1,2,0},	TRUE,	0,	NULL,	NULL	},
	/*034*/	{ "MappingNotify",	TOKEN,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*035*/	{ "(UNDEFINED)",	UNK,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*036*/	{ "(UNDEFINED)",	UNK,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*037*/	{ "(UNDEFINED)",	UNK,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*038*/	{ "(UNDEFINED)",	UNK,	{ 0 },		TRUE,	0,	NULL,	NULL	},
	/*039*/	{ "(UNDEFINED)",	UNK,	{ 0 },		TRUE,	0,	NULL,	NULL	},
};

#define	MAX_EVENT	34



#define SimpleReplace   2       /* Simple replace by table in RequestTableEntry */
#define ComplexReplace  3       /* Run both a simple replace and a special function */
#define NotPossible     5       /* Reply from server is not possible for the request */


typedef struct _ReplyTableEntry {
   char   PacketName[30];      /* MIT Standard Packet Name */
   BYTE   HandlingCode;      /* How TranslatePacket is to handle the request */
   int   OffsetToID[10];      /* Table of offsets to simple XIDs */
   void   (*ProcessFunc)();   /* Function to handle any special processing */
} ReplyTableEntry;

	ReplyTableEntry	ReplyTable[128] = { 

	/*000*/	{ "NOT DEFINED",		NotPossible,	{ 0 },			NULL		}, 
	/*001*/	{ "CreateWindow",		NotPossible,	{ 0 },	
NULL		},
	/*002*/	{ "ChangeWindowAttributes",	NotPossible,	{ 0 },		NULL		},
	/*003*/	{ "GetWindowAttributes",	SimpleReplace,	{ 2, 7, 0 },	NULL		},
	/*004*/	{ "DestroyWindow",		NotPossible,	{ 0 },		NULL		},
	/*005*/	{ "DestroySubwindows",		NotPossible,	{ 0 },		NULL		},
	/*006*/	{ "ChangeSaveSet",		NotPossible,	{ 0 },		NULL		},
	/*007*/	{ "ReparentWindow",		NotPossible,	{ 0 },		NULL		},
	/*008*/	{ "MapWindow",			NotPossible,	{ 0 },		NULL		},
	/*009*/	{ "MapSubwindows",		NotPossible,	{ 0 },		NULL		},
	/*010*/	{ "UnmapWindow",		NotPossible,	{ 0 },		NULL		},
	/*011*/	{ "UnmapSubwindows",		NotPossible,	{ 0 },		NULL		},
	/*012*/	{ "ConfigureWindow",		NotPossible,	{ 0 },		NULL		},
	/*013*/	{ "CirculateWindow",		NotPossible,	{ 0 },		NULL		},
	/*014*/	{ "GetGeometry",		SimpleReplace,	{ 2, 0 },	NULL		},
	/*015*/	{ "QueryTree",			ComplexReplace,	{ 1, 2, 3, 0 },	XlateQueryTree	},
	/*016*/	{ "InternAtom",			NotPossible,	{ 0 },		NULL		},
	/*017*/	{ "GetAtomName",		SimpleReplace,	{ 0 },		NULL		},
	/*018*/	{ "ChangeProperty",		NotPossible,	{ 0},		NULL		},
	/*019*/	{ "DeleteProperty",		NotPossible,	{ 0 },		NULL		},
	/*020*/	{ "GetProperty",		NotPossible,	{ 0 },	
NULL		},
	/*021*/	{ "ListProperties",		ComplexReplace,	{ 0 },		XlateListProp	},
	/*022*/	{ "SetSelectionOwner",		NotPossible,	{ 0 },		NULL		},
	/*023*/	{ "GetSelectionOwner",		SimpleReplace,	{ 2, 0 },	NULL		},
	/*024*/	{ "ConvertSelection",		NotPossible,	{ 0 },	
NULL		},
	/*025*/	{ "SendEvent",			NotPossible,	{ 0 },		NULL		},
	/*026*/	{ "GrabPointer",		SimpleReplace,	{ 0 },	
NULL		},
	/*027*/	{ "UngrabPointer",		NotPossible,	{ 0 },		NULL		},
	/*028*/	{ "GrabButton",			NotPossible,	{ 0 },	
NULL		},
	/*029*/	{ "UngrabButton",		NotPossible,	{ 0 },		NULL		},
	/*030*/	{ "ChangeActivePointerGrab",	NotPossible,	{ 0 },		NULL		},
	/*031*/	{ "GrabKeyboard",		SimpleReplace,	{ 0 },		NULL		},
	/*032*/	{ "UngrabKeyboard",		NotPossible,	{ 0 },		NULL		},
	/*033*/	{ "GrabKey",			SimpleReplace,	{ 1, 0 },	NULL		},
	/*034*/	{ "UngrabKey",			NotPossible,	{ 0 },		NULL		},
	/*035*/	{ "AllowEvents",		NotPossible,	{ 0 },		NULL		},
	/*036*/	{ "GrabServer",			NotPossible,	{ 0 },		NULL		},
	/*037*/	{ "UngrabServer",		NotPossible,	{ 0 },		NULL		},
	/*038*/	{ "QueryPointer",		SimpleReplace,	{ 2, 3, 0 },	NULL		},
	/*039*/	{ "GetMotionEvents",		SimpleReplace,	{ 0 },		NULL		},
	/*040*/	{ "TranslateCoordinates",	SimpleReplace,	{ 1, 2, 0 },	NULL		},
	/*041*/	{ "WarpPointer",		NotPossible,	{ 0 },		NULL		},
	/*042*/	{ "SetInputFocus",		NotPossible,	{ 0 },		NULL		},
	/*043*/	{ "GetInputFocus",		SimpleReplace,	{ 2, 0 },	NULL		},
	/*044*/	{ "QueryKeymap",		SimpleReplace,	{ 0 },		NULL		},
	/*045*/	{ "OpenFont",			NotPossible,	{ 0 },		NULL		},
	/*046*/	{ "CloseFont",			NotPossible,	{ 0 },		NULL		},
	/*047*/	{ "QueryFont",			SimpleReplace,	{ 0 },		NULL		},
	/*048*/	{ "QueryTextExtents",		SimpleReplace,	{ 0 },		NULL		},
	/*049*/	{ "ListFonts",			SimpleReplace,	{ 0 },		NULL		},
	/*050*/	{ "ListFontsWithInfo",		ComplexReplace,	{ 0 },		XlateListFonts	},
	/*051*/	{ "SetFontPath",		NotPossible,	{ 0 },		NULL		},
	/*052*/	{ "GetFontPath",		SimpleReplace,	{ 0 },		NULL		},
	/*053*/	{ "CreatePixmap",		NotPossible,	{ 0 },		NULL		},
	/*054*/	{ "FreePixmap",			NotPossible,	{ 0 },		NULL		},
	/*055*/	{ "CreateGC",			NotPossible,	{ 0 },		NULL		},
	/*056*/	{ "CrangeGC",			NotPossible,	{ 0 },		NULL		},
	/*057*/	{ "CopyGC",			NotPossible,	{ 0 },		NULL		},
	/*058*/	{ "SetDashes",			NotPossible,	{ 0 },		NULL		},
	/*059*/	{ "SetClipRectangles",		NotPossible,	{ 0 },		NULL		},
	/*060*/	{ "FreeGC",			NotPossible,	{ 0 },		NULL		},
	/*061*/	{ "ClearArea",			NotPossible,	{ 0 },		NULL		},
	/*062*/	{ "CopyArea",			NotPossible,	{ 0 },		NULL		},
	/*063*/	{ "CopyPlane",			NotPossible,	{ 0},		NULL		},
	/*064*/	{ "PolyPoint",			NotPossible,	{ 0 },		NULL		},
	/*065*/	{ "PolyLine",			NotPossible,	{ 0 },		NULL		},
	/*066*/	{ "PolySegment",		NotPossible,	{ 0 },		NULL		},
	/*067*/	{ "PolyRectangle",		NotPossible,	{ 0 },		NULL		},
	/*068*/	{ "PolyArc",			NotPossible,	{ 0 },		NULL		},
	/*069*/	{ "FillPoly",			NotPossible,	{ 0 },		NULL		},
	/*070*/	{ "PolyFillRectangle",		NotPossible,	{ 0 },		NULL		},
	/*071*/	{ "PolyFillArc",		NotPossible,	{ 0 },		NULL		},
	/*072*/	{ "PutImage",			NotPossible,	{ 0 },		NULL		},
	/*073*/	{ "GetImage",			SimpleReplace,	{ 2, 0 },	NULL		},
	/*074*/	{ "PolyText8",			NotPossible,	{ 0 },		NULL		},
	/*075*/	{ "PolyText16",			NotPossible,	{ 0 },		NULL		},
	/*076*/	{ "ImageText8",			NotPossible,	{ 0 }, 		NULL		},
	/*077*/	{ "ImageText16",		NotPossible,	{ 0 }, 		NULL		},
	/*078*/	{ "CreateColormap",		NotPossible,	{ 0 },		NULL		},
	/*079*/	{ "FreeColormap",		NotPossible,	{ 0 },		NULL		},
	/*080*/	{ "CopyColormapAndFree",	NotPossible,	{ 0 },		NULL		},
	/*081*/	{ "InstallColormap",		NotPossible,	{ 0 },		NULL		},
	/*082*/	{ "UninstallColormap",		NotPossible,	{ 0 },		NULL		},
	/*083*/	{ "ListInstalledColormaps",	ComplexReplace,	{ 0 },		XlateColormaps	},
	/*084*/	{ "AllocColor",			NotPossible,	{ 0 },		NULL		},
	/*085*/	{ "AllocNamedColor",		NotPossible,	{ 0 },		NULL		},
	/*086*/	{ "AllocColorCells",		NotPossible,	{ 0 },		NULL		},
	/*087*/	{ "AllocColorPlanes",		NotPossible,	{ 0 },		NULL		},
	/*088*/	{ "FreeColors",			NotPossible,	{ 0 },		NULL		},
	/*089*/	{ "StoreColors",		NotPossible,	{ 0 },		NULL		},
	/*090*/	{ "StoreNamedColor",		NotPossible,	{ 0 },		NULL		},
	/*091*/	{ "QueryColors",		SimpleReplace,	{ 0 },		NULL		},
	/*092*/	{ "LookupColor",		SimpleReplace,	{ 0 },		NULL		},
	/*093*/	{ "CreateCursor",		NotPossible,	{ 0 },	
NULL		},
	/*094*/	{ "CreateGlyphCursor",		NotPossible,	{ 0 },		NULL		},
	/*095*/	{ "FreeCursor",			NotPossible,	{ 0 },		NULL		},
	/*096*/	{ "RecolorCursor",		NotPossible,	{ 0 },		NULL		},
	/*097*/	{ "QueryBestSize",		SimpleReplace,	{ 0 },		NULL		},
	/*098*/	{ "QueryExtension",		SimpleReplace,	{ 0 },		NULL		},
	/*099*/	{ "ListExtensions",		SimpleReplace,	{ 0 },		NULL		},
	/*100*/	{ "ChangeKeyboardMapping",	NotPossible,	{ 0 },		NULL		},
	/*101*/	{ "GetKeyboardMapping",		SimpleReplace,	{ 0 },		NULL		},
	/*102*/	{ "ChangeKeyboardControl",	SimpleReplace,	{ 0 },		NULL		},
	/*103*/	{ "GetKeyboardControl",		SimpleReplace,	{ 0 },		NULL		},
	/*104*/	{ "Bell",			NotPossible,	{ 0 },		NULL		},
	/*105*/	{ "ChangePointerControl",	NotPossible,	{ 0 },		NULL		},
	/*106*/	{ "GetPointerControl",		SimpleReplace,	{ 0 },		NULL		},
	/*107*/	{ "SetScreenSaver",		NotPossible,	{ 0 },		NULL		},
	/*108*/	{ "GetScreenSaver",		SimpleReplace,	{ 0 },		NULL		},
	/*109*/	{ "ChangeHosts",		NotPossible,	{ 0 },		NULL		},
	/*110*/	{ "ListHosts",			SimpleReplace,	{ 0 },		NULL		},
	/*111*/	{ "SetAccessControl",		NotPossible,	{ 0 },		NULL		},
	/*112*/	{ "SetCloseDownMode",		NotPossible,	{ 0 },		NULL		},
	/*113*/	{ "KillClient",			NotPossible,	{ 0 },		NULL		},
	/*114*/	{ "RotateProperties",		NotPossible,	{ 0 },		NULL		},
	/*115*/	{ "ForceScreenSaver",		NotPossible,	{ 0 },		NULL		},
	/*116*/	{ "SetPointerMapping",		SimpleReplace,	{ 0 },		NULL		},
	/*117*/	{ "GetPointerMapping",		SimpleReplace,	{ 0 },		NULL		},
	/*118*/	{ "SetModifierMapping",		SimpleReplace,	{ 0 },		NULL		},
	/*119*/	{ "GetModifierMapping",		SimpleReplace,	{ 0 },		NULL		},
	/*120*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*121*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*122*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*123*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*124*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*125*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*126*/	{ "NOT DEFINED",		NotPossible,	{ 0 },		NULL		},
	/*127*/	{ "NoOperation",		NotPossible,	{ 0 },		NULL		}
};

	int	has_reply[128] = { 

	/*000 NOT DEFINED */		FALSE, 
	/*001 CreateWindow */		FALSE,
	/*002 ChangeWindowAttributes*/	FALSE,
	/*003 GetWindowAttributes*/	TRUE,
	/*004 DestroyWindow*/		FALSE,
	/*005 DestroySubwindows*/	FALSE,
	/*006 ChangeSaveSet*/		FALSE,
	/*007 ReparentWindow*/		FALSE,
	/*008 MapWindow*/		FALSE,
	/*009 MapSubwindows*/		FALSE,
	/*010 UnmapWindow*/		FALSE,
	/*011 UnmapSubwindows*/		FALSE,
	/*012 ConfigureWindow*/		FALSE,
	/*013 CirculateWindow*/		FALSE,
	/*014 GetGeometry*/		TRUE,
	/*015 QueryTree*/		TRUE,
	/*016 InternAtom*/		FALSE,
	/*017 GetAtomName*/		TRUE,
	/*018 ChangeProperty*/		FALSE,
	/*019 DeleteProperty*/		FALSE,
	/*020 GetProperty*/		FALSE,
	/*021 ListProperties*/		TRUE,
	/*022 SetSelectionOwner*/	FALSE,
	/*023 GetSelectionOwner*/	TRUE,
	/*024 ConvertSelection*/	FALSE,
	/*025 SendEvent*/		FALSE,
	/*026 GrabPointer*/		TRUE,
	/*027 UngrabPointer*/		FALSE,
	/*028 GrabButton*/		FALSE,
	/*029 UngrabButton*/		FALSE,
	/*030 ChangeActivePointerGrab*/	FALSE,
	/*031 GrabKeyboard*/		TRUE,
	/*032 UngrabKeyboard*/		FALSE,
	/*033 GrabKey*/			TRUE,
	/*034 UngrabKey*/		FALSE,
	/*035 AllowEvents*/		FALSE,
	/*036 GrabServer*/		FALSE,
	/*037 UngrabServer*/		FALSE,
	/*038 QueryPointer*/		TRUE,
	/*039 GetMotionEvents*/		TRUE,
	/*040 TranslateCoordinates*/	TRUE,
	/*041 WarpPointer*/		FALSE,
	/*042 SetInputFocus*/		FALSE,
	/*043 GetInputFocus*/		TRUE,
	/*044 QueryKeymap*/		TRUE,
	/*045 OpenFont*/		FALSE,
	/*046 CloseFont*/		FALSE,
	/*047 QueryFont*/		TRUE,
	/*048 QueryTextExtents*/	TRUE,
	/*049 ListFonts*/		TRUE,
	/*050 ListFontsWithInfo*/	TRUE,
	/*051 SetFontPath*/		FALSE,
	/*052 GetFontPath*/		TRUE,
	/*053 CreatePixmap*/		FALSE,
	/*054 FreePixmap*/		FALSE,
	/*055 CreateGC*/		FALSE,
	/*056 CrangeGC*/		FALSE,
	/*057 CopyGC*/			FALSE,
	/*058 SetDashes*/		FALSE,
	/*059 SetClipRectangles*/	FALSE,
	/*060 FreeGC*/			FALSE,
	/*061 ClearArea*/		FALSE,
	/*062 CopyArea*/		FALSE,
	/*063 CopyPlane*/		FALSE,
	/*064 PolyPoint*/		FALSE,
	/*065 PolyLine*/		FALSE,
	/*066 PolySegment*/		FALSE,
	/*067 PolyRectangle*/		FALSE,
	/*068 PolyArc*/			FALSE,
	/*069 FillPoly*/		FALSE,
	/*070 PolyFillRectangle*/	FALSE,
	/*071 PolyFillArc*/		FALSE,
	/*072 PutImage*/		FALSE,
	/*073 GetImage*/		TRUE,	
	/*074 PolyText8*/		FALSE,
	/*075 PolyText16*/		FALSE,
	/*076 ImageText8*/		FALSE,
	/*077 ImageText16*/		FALSE,
	/*078 CreateColormap*/		FALSE,
	/*079 FreeColormap*/		FALSE,
	/*080 CopyColormapAndFree*/	FALSE,
	/*081 InstallColormap*/		FALSE,
	/*082 UninstallColormap*/	FALSE,
	/*083 ListInstalledColormaps*/	TRUE,
	/*084 AllocColor*/		FALSE,
	/*085 AllocNamedColor*/		FALSE,
	/*086 AllocColorCells*/		FALSE,
	/*087 AllocColorPlanes*/	FALSE,
	/*088 FreeColors*/		FALSE,
	/*089 StoreColors*/		FALSE,
	/*090 StoreNamedColor*/		FALSE,
	/*091 QueryColors*/		TRUE,	
	/*092 LookupColor*/		TRUE,
	/*093 CreateCursor*/		FALSE,
	/*094 CreateGlyphCursor*/	FALSE,
	/*095 FreeCursor*/		FALSE,
	/*096 RecolorCursor*/		FALSE,
	/*097 QueryBestSize*/		TRUE,
	/*098 QueryExtension*/		TRUE,
	/*099 ListExtensions*/		TRUE,
	/*100 ChangeKeyboardMapping*/	FALSE,
	/*101 GetKeyboardMapping*/	TRUE,
	/*102 ChangeKeyboardControl*/	TRUE,
	/*103 GetKeyboardControl*/	TRUE,
	/*104 Bell*/			FALSE,
	/*105 ChangePointerControl*/	FALSE,
	/*106 GetPointerControl*/	TRUE,	
	/*107 SetScreenSaver*/		FALSE,
	/*108 GetScreenSaver*/		TRUE,
	/*109 ChangeHosts*/		FALSE,
	/*110 ListHosts*/		TRUE,
	/*111 SetAccessControl*/	FALSE,
	/*112 SetCloseDownMode*/	FALSE,
	/*113 KillClient*/		FALSE,
	/*114 RotateProperties*/	FALSE,
	/*115 ForceScreenSaver*/	FALSE,
	/*116 SetPointerMapping*/	TRUE,
	/*117 GetPointerMapping*/	TRUE,
	/*118 SetModifierMapping*/	TRUE,
	/*119 GetModifierMapping*/	TRUE,
	/*120 NOT DEFINED*/		FALSE,
	/*121 NOT DEFINED*/		FALSE,
	/*122 NOT DEFINED*/		FALSE,
	/*123 NOT DEFINED*/		FALSE,
	/*124 NOT DEFINED*/		FALSE,
	/*125 NOT DEFINED*/		FALSE,
	/*126 NOT DEFINED*/		FALSE,
	/*127 NoOperation*/		FALSE
};
