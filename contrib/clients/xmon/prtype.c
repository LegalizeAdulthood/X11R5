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
 * File: prtype.c
 *
 * Description: Type Printing for X11 protocol
 *
 * For each of the types we need a way to print that type.  Types are of
 * varieties:
 *
 * (1) BUILTIN -- we have a separate routine to interpret and print each
 * built-in type.
 *
 * (2) ENUMERATED, SET -- we have one routine which prints, given the data
 * and the list of values.
 *
 * (3) RECORDS -- a separate routine for each to print each field.
 */

#include "common.h"

#include "xmond.h"
#include "x11.h"

/*
 * we use indentation for two purposes:
 *
 * (1) To show substructure of records inside records ...
 *
 * (2) To separate the bytes from the client (on the left) from those from
 * the server (on the right).
 *
 * Each indention level is one tab (8 spaces).
 */
#define MaxIndent 10

#define NumberofAtoms 68
#define MAXline 78

/* function prototypes: */
/* prtype.c: */
static char *printrep P((unsigned short c ));
static short SizeofLeader P((void ));

/* end function prototypes */

extern int		    CurrentVerbose;
extern int		    debuglevel;
extern int		    littleEndian;

static char Leader[MaxIndent + 1];
static short	CurrentLevel = 0;

static char   *AtomTable[NumberofAtoms + 1] =
{
    "NONE", "PRIMARY", "SECONDARY", "ARC", "ATOM", "BITMAP", "CARDINAL",
    "COLORMAP", "CURSOR", "CUT_BUFFER0", "CUT_BUFFER1", "CUT_BUFFER2",
    "CUT_BUFFER3", "CUT_BUFFER4", "CUT_BUFFER5", "CUT_BUFFER6",
    "CUT_BUFFER7", "DRAWABLE", "FONT", "INTEGER", "PIXMAP", "POINT",
    "RECTANGLE", "RESOURCE_MANAGER", "RGB_COLOR_MAP", "RGB_BEST_MAP",
    "RGB_BLUE_MAP", "RGB_DEFAULT_MAP", "RGB_GRAY_MAP", "RGB_GREEN_MAP",
    "RGB_RED_MAP", "STRING", "VISUALID", "WINDOW", "WM_COMMAND",
    "WM_HINTS", "WM_CLIENT_MACHINE", "WM_ICON_NAME", "WM_ICON_SIZE",
    "WM_NAME", "WM_NORMAL_HINTS", "WM_SIZE_HINTS", "WM_ZOOM_HINTS",
    "MIN_SPACE", "NORM_SPACE", "MAX_SPACE", "END_SPACE", "SUPERSCRIPT_X",
    "SUPERSCRIPT_Y", "SUBSCRIPT_X", "SUBSCRIPT_Y", "UNDERLINE_POSITION",
    "UNDERLINE_THICKNESS", "STRIKEOUT_ASCENT", "STRIKEOUT_DESCENT",
    "ITALIC_ANGLE", "X_HEIGHT", "QUAD_WIDTH", "WEIGHT", "POINT_SIZE",
    "RESOLUTION", "COPYRIGHT", "NOTICE", "FONT_NAME", "FAMILY_NAME",
    "FULL_NAME", "CAP_HEIGHT", "WM_CLASS", "WM_TRANSIENT_FOR"
    };

/*
 * printrep:
 *
 * print representation of a character for debugging
 */
static char*
printrep (c)
	unsigned short	c;
{
    static char pr[8];

    if (c < 32)
	{
	    /* control characters */
	    pr[0] = '^';
	    pr[1] = c + 64;
	    pr[2] = '\0';
	}
    else if (c < 127)
	    {
		/* printing characters */
		pr[0] = c;
		pr[1] = '\0';
	    }
	else if (c == 127)
		return("<del>");
	    else if (c <= 0377)
		    {
			/* upper 128 codes from 128 to 255;
			 * print as \ooo - octal
			 */
			pr[0] = '\\';
			pr[3] = '0' + (c & 7);
			c = c >> 3;
			pr[2] = '0' + (c & 7);
			c = c >> 3;
			pr[1] = '0' + (c & 3);
			pr[4] = '\0';
		    }
		else
		    {
			/* very large number - print as 0xffff - 4 digit hex */
			sprintf(pr, "0x%04x", c);
		    }
    return(pr);
}

Global void
SetIndentLevel(which)
	short	which;
{
    short   i;

    if (which > MaxIndent)
	which = MaxIndent;
    if (which < 0)
	which = 0;
    if (which == CurrentLevel)
	return;

    /* set the indent level to <which> */
    /* -> set the Print Leader to <which> tabs */
    for (i = 0; i < which; i++)
	Leader[i] = '\t';
    Leader[which] = '\0';
    CurrentLevel = which;
}

Global void
ModifyIndentLevel(amount)
	short	amount;
{
    SetIndentLevel(CurrentLevel + amount);
}

static short
SizeofLeader ()
{
    return (CurrentLevel * 8);
}

/*
 * DumpItem:
 *
 * if we want verbose enough output, we will dump the buffer in hex
 */
Global void
DumpItem(name, fd, buf, n)
	char   *name;
	int	 fd;
	unsigned char *buf;
	long	n;
{
    if (n == 0)
	return;

    fprintf(stdout, "%s%20s (fd %d): ", Leader, name, fd);

    DumpHexBuffer(buf, n);
    fprintf(stdout, "\n");
}

Global void
PrintINT8(buf)
	unsigned char *buf;
{
    /* print a INT8 -- 8-bit signed integer */
    short   n = IByte (buf);
    if (n > 127)
	n = 256 - n;
    fprintf(stdout, "%d", n);
}

Global void
PrintINT16(buf)
	unsigned char *buf;
{
    /* print a INT16 -- 16-bit signed integer */
    long    n = IShort (buf);
    if (n > 32767)
	n = 65536 - n;
    fprintf(stdout, "%d", n);
}

Global void
PrintINT32(buf)
	unsigned char *buf;
{
    /* print a INT32 -- 32-bit signed integer */
    long    n = ILong (buf);
    fprintf(stdout, "%d", n);
}

Global void
PrintCARD8(buf)
	unsigned char *buf;
{
    /* print a CARD8 -- 8-bit unsigned integer */
    short   n = IByte (buf);
    fprintf(stdout, "%02x", (unsigned)(n & 0xff));
}

Global void
PrintCARD16(buf)
	unsigned char *buf;
{
    /* print a CARD16 -- 16-bit unsigned integer */
    unsigned long   n = IShort (buf);
    fprintf(stdout, "%04x", (unsigned)(n & 0xffff));
}

Global int
PrintCARD32(buf)
	unsigned char *buf;
{
    /* print a CARD32 -- 32-bit unsigned integer */
    unsigned long   n = ILong (buf);
    fprintf(stdout, "%08x", n);
    return(4);
}

Global int
PrintBYTE(buf)
	unsigned char *buf;
{
    /* print a BYTE -- 8-bit value */
    short   n = IByte (buf);
    fprintf(stdout, "%02x", n);
    return(1);
}

Global int
PrintCHAR8(buf)
	unsigned char *buf;
{
    /* print a CHAR8 -- 8-bit character */
    unsigned short   n = IByte (buf);
    fprintf(stdout, "%s", printrep(n));
    return(1);
}

Global int
PrintSTRING16(buf)
	unsigned char *buf;
{
    /* print a CHAR16 -- 16-bit character */
    unsigned short   n = IShort (buf);
    fprintf(stdout, "%s", printrep(n));
    return(1);
}

Global int
PrintSTR(buf)
	unsigned char *buf;
{
    /* STR have the length (1 byte) then a string of CHAR8 */
    short   n;
    short   i;

    n = IByte(buf++);
    for (i = 0; i < n; i++)
	fprintf(stdout, "%s", printrep(buf[i]));
    return(n+1);
}

Global int
PrintWINDOW(buf)
	unsigned char *buf;
{
    /* print a WINDOW -- CARD32	 plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "WIN %08x", n);
    return(4);
}

Global void
PrintWINDOWD(buf)
	unsigned char *buf;
{
    /* print a WINDOWD -- CARD32  plus 0 = PointerWindow, 1 = InputFocus */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "PointerWindow");
    else if (n == 1)
	    fprintf(stdout, "InputFocus");
	else
	    PrintWINDOW(buf);
}

Global void
PrintWINDOWNR(buf)
	unsigned char *buf;
{
    /* print a WINDOWNR -- CARD32  plus 0 = None, 1 = PointerRoot */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else if (n == 1)
	    fprintf(stdout, "PointerRoot");
	else
	    PrintWINDOW(buf);
}


Global void
PrintPIXMAP(buf)
	unsigned char *buf;
{
    /* print a PIXMAP -- CARD32	 plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "PXM %08x", n);
}

Global void
PrintPIXMAPNPR(buf)
	unsigned char *buf;
{
    /* print a PIXMAPNPR -- CARD32  plus 0 = None, 1 = ParentRelative */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else if (n == 1)
	    fprintf(stdout, "ParentRelative");
	else
	    PrintPIXMAP(buf);
}

Global void
PrintPIXMAPC(buf)
	unsigned char *buf;
{
    /* print a PIXMAPC -- CARD32  plus 0 = CopyFromParent */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "CopyFromParent");
    else
	PrintPIXMAP(buf);
}


Global void
PrintCURSOR(buf)
	unsigned char *buf;
{
    /* print a CURSOR -- CARD32	 plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "CUR %08x", n);
}


Global void
PrintFONT(buf)
	unsigned char *buf;
{
    /* print a FONT -- CARD32  plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "FNT %08x", n);
}


Global void
PrintGCONTEXT(buf)
	unsigned char *buf;
{
    /* print a GCONTEXT -- CARD32 */
    long    n = ILong (buf);
    fprintf(stdout, "GXC %08x", n);
}


Global int
PrintCOLORMAP(buf)
	unsigned char *buf;
{
    /* print a COLORMAP -- CARD32 plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "CMP %08x", n);
    return(4);
}

Global void
PrintCOLORMAPC(buf)
	unsigned char *buf;
{
    /* print a COLORMAPC -- CARD32 plus 0 = CopyFromParent */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "CopyFromParent");
    else
	PrintCOLORMAP(buf);
}


Global void
PrintDRAWABLE(buf)
	unsigned char *buf;
{
    /* print a DRAWABLE -- CARD32 */
    long    n = ILong (buf);
    fprintf(stdout, "DWB %08x", n);
}

Global void
PrintFONTABLE(buf)
	unsigned char *buf;
{
    /* print a FONTABLE -- CARD32 */
    long    n = ILong (buf);
    fprintf(stdout, "FTB %08x", n);
}

/*
 * For atoms, we print the built-in atoms.  We could expand to printing
 *  the user defined ones, too.
 */

Global int
PrintATOM(buf)
	unsigned char *buf;
{
    /* print a ATOM -- CARD32 plus 0 = None */
    long    n = ILong (buf);
    if (0 <= n && n <= NumberofAtoms)
	fprintf(stdout, "<%s>", AtomTable[n]);
    else
	fprintf(stdout, "ATM %08x", n);
    return(4);
}

Global void
PrintATOMT(buf)
	unsigned char *buf;
{
    /* print a ATOMT -- CARD32 plus 0 = AnyPropertyType */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "AnyPropertyType");
    else
	PrintATOM(buf);
}


Global void
PrintVISUALID(buf)
	unsigned char *buf;
{
    /* print a VISUALID -- CARD32 plus 0 = None */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "None");
    else
	fprintf(stdout, "VIS %08x", n);
}

Global void
PrintVISUALIDC(buf)
	unsigned char *buf;
{
    /* print a VISUALIDC -- CARD32 plus 0 = CopyFromParent */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "CopyFromParent");
    else
	PrintVISUALID(buf);
}


Global void
PrintTIMESTAMP(buf)
	unsigned char *buf;
{
    /* print a TIMESTAMP -- CARD32 plus 0 as the current time */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "CurrentTime");
    else
	fprintf(stdout, "TIM %08x", n);
}


Global void
PrintRESOURCEID(buf)
	unsigned char *buf;
{
    /* print a RESOURCEID -- CARD32 plus 0 = AllTemporary */
    long    n = ILong (buf);
    if (n == 0)
	fprintf(stdout, "AllTemporary");
    else
	fprintf(stdout, "RID %08x", n);
}


Global int
PrintKEYSYM(buf)
	unsigned char *buf;
{
    /* print a KEYSYM -- CARD32 */
    long    n = ILong (buf);
    fprintf(stdout, "KYS %08x", n);
    return(4);
}

Global int
PrintKEYCODE(buf)
	unsigned char *buf;
{
    /* print a KEYCODE -- CARD8 */
    unsigned short n = IByte (buf);
    fprintf(stdout, "%d (%s)", n, printrep(n));
    return(1);
}

Global void
PrintKEYCODEA(buf)
	unsigned char *buf;
{
    /* print a KEYCODEA -- CARD8 plus 0 = AnyKey */
    long    n = IByte (buf);
    if (n == 0)
	fprintf(stdout, "AnyKey");
    else
	PrintKEYCODE(buf);
}


Global void
PrintBUTTON(buf)
	unsigned char *buf;
{
    /* print a BUTTON -- CARD8 */
    unsigned short n = IByte (buf);
    fprintf(stdout, "%d (%s)", n, printrep(n));
}

Global void
PrintBUTTONA(buf)
	unsigned char *buf;
{
    /* print a BUTTONA -- CARD8 plus 0 = AnyButton */
    long    n = IByte (buf);
    if (n == 0)
	fprintf(stdout, "AnyButton");
    else
	PrintBUTTON(buf);
}


/*
 * PrintEVENTFORM:
 *
 * This is an interesting cheat -- we call DecodeEvent to print an event
 * should work, but its never been tried.
 */
Global void
PrintEVENTFORM(buf)
	unsigned char *buf;
{
    /* print an EVENT_FORM -- event format */
    DecodeEvent((Server *)NULL, buf, (long)-1, False);
}

Global void
PrintENUMERATED(buf, length, ValueList)
	unsigned char *buf;
	short	length;
	struct ValueListEntry  *ValueList;
{
    long   n;
    struct ValueListEntry  *p;

    if (length == 1)
	n = IByte(buf);
    else if (length == 2)
	    n = IShort(buf);
	else
	    n = ILong(buf);

    p = ValueList;
    while (p != NULL && p->Value != n)
	p = p->Next;

    if (p != NULL)
	fprintf(stdout, "%s", p->Name);
    else
	fprintf(stdout, "**INVALID** (%d)", n);
}

Global void
PrintSET(buf, length, ValueList)
	unsigned char *buf;
	short	length;
	struct ValueListEntry  *ValueList;
{
    unsigned long   n;
    struct ValueListEntry  *p;
    Bool MatchesAll = False;
    Bool FoundOne = False;

    if (length == 1)
	n = IByte(buf);
    else if (length == 2)
	    n = IShort(buf);
	else
	    n = ILong(buf);

    if (n != 0)
	{
	    /* first check if the value matches ALL of the bits. */
	    MatchesAll = True;
	    for (p = ValueList; MatchesAll && (p != NULL); p = p->Next)
		{
		    if ((p->Value & n) == 0)
			    MatchesAll = False;
		}

	    if (!MatchesAll)
		/* if it matches some but not all print only
		 * those it matches
		 */
		for (p = ValueList; p != NULL; p = p->Next)
		    {
			if ((p->Value & n) != 0)
			    {
				if (FoundOne)
				    fprintf(stdout, " | ");
				fprintf(stdout, "%s", p->Name);
				FoundOne = True;
			    }
		    }
	}

    if (MatchesAll)
	fprintf(stdout, "<ALL>");
    else if (!FoundOne)
	fprintf(stdout, "0");
}

Global void
PrintField(buf, start, length, FieldType, name)
    unsigned char *buf;
    short   start;
    short   length;
    short   FieldType;
    char   *name;
{
    if (length == 0)
	return;
    if (CurrentVerbose <= 0)
	return;

    fprintf(stdout, "%s%20s: ", Leader, name);

    if (debuglevel & 8)
	DumpHexBuffer(&(buf[start]), (long)length);

    switch (TD[FieldType].Type)
    {
    case BUILTIN:
	(*TD[FieldType].PrintProc)(&buf[start]);
	break;

    case ENUMERATED:
	PrintENUMERATED(&buf[start], length, TD[FieldType].ValueList);
	break;

    case SET:
	PrintSET(&buf[start], length, TD[FieldType].ValueList);
	break;

    case RECORD:
	ModifyIndentLevel(1);
	fprintf(stdout, "\n");
	if (CurrentVerbose < 3)
	    return;
	(*TD[FieldType].PrintProc)(&buf[start]);
	ModifyIndentLevel(-1);
	break;
    }
    fprintf(stdout, "\n");
    fflush(stdout);
}

/*
 * PrintList:
 *
 * Print a list of things.  The things are of type <ListType>.
 * They start at <buf>.	 There are <number> things in the list
 */
Global int
PrintList(buf, number, ListType, name)
	unsigned char *buf;
	long   number;
	short	ListType;
	char   *name;
{
    long    n;
    long    i;
    long    sum;

    if (number == 0)
	return(0);

    fprintf(stdout, "%s%20s: (%d)\n", Leader, name, number);
    if (CurrentVerbose < 3)
	return(0);

    ModifyIndentLevel(1);
    sum = 0;
    for (i = 0; i < number; i++)
	{
	    switch (TD[ListType].Type)
	    {
	    case BUILTIN:
		n = (*TD[ListType].PrintProc)(buf);
		break;
	    case RECORD:
		n = (*TD[ListType].PrintProc)(buf);
		break;
	    default:
		fprintf(stdout, "**INVALID**");
		n = 0;
		break;
	    }
	    buf = buf + n;
	    sum = sum + n;
	    fprintf(stdout, "%s---\n", Leader);
	}

    ModifyIndentLevel(-1);
    return(sum);
}

/*
 * PrintListSTR:
 *
 * Print a list of STRs.  Similar to PrintList
 * They start at <buf>.	 There are <number> things in the list
 */
Global void
PrintListSTR(buf, number, name)
	unsigned char *buf;
	long   number;
	char   *name;
{
    long    n;
    long    i;
    long    sum;

    if (number == 0)
	return;

    fprintf(stdout, "%s%20s: (%d)\n", Leader, name, number);
    if (CurrentVerbose < 3)
	return;

    ModifyIndentLevel(1);
    sum = 0;
    for (i = 0; i < number; i++)
	{
	    fprintf(stdout, "%s", Leader);
	    n = PrintSTR(buf);
	    buf = buf + n;
	    sum = sum + n;
	    fprintf(stdout, "\n");
	}

    ModifyIndentLevel(-1);
    return;
}

Global int
PrintBytes(buf, number, name)
	unsigned char	buf[];
	long   number;
	char   *name;
{
    /* print a list of BYTE -- 8-bit character */
    long   i;
    short   column;

    if (number == 0)
	return(0);

    fprintf(stdout, "%s%20s: ", Leader, name);
    column = SizeofLeader() + 25;
    for (i = 0; i < number; i++)
	{
	    if (column > 80)
		{
		    if (CurrentVerbose < 3)
			break;
		    fprintf(stdout, "\n%s%20s: ", Leader, "");
		    column = SizeofLeader() + 25;
		}
	    fprintf(stdout, "%02x ",((unsigned int) buf[i]));
	    column += 3;
	}
    fprintf(stdout, "\n");

    return((int)number);
}

/*
 * print a String of CHAR8 -- 8-bit characters
 */
Global int
PrintString8(buf, number, name)
	unsigned char	 buf[];
	short	number;
	char   *name;
{
    short   i;

    if (number == 0)
	return(0);

    fprintf(stdout, "%s%20s: \"", Leader, name);
    for (i = 0; i < number; i++)
	fprintf(stdout, "%s", printrep(buf[i]));
    fprintf(stdout, "\"\n");

    return(number);
}

/*
 * print a String of CHAR16 -- 16-bit characters
 */
Global int
PrintString16(buf, number, name)
	unsigned char	 buf[];
	short	number;
	char   *name;
{
    short   i;
    unsigned short   c;

    if (number == 0)
	return(0);

    fprintf(stdout, "%s%20s: \"", Leader, name);
    for (i = 0; i < number; i += 2)
	{
	    c = IShort(&buf[i]);
	    fprintf(stdout, "%s", printrep(c));
	}
    fprintf(stdout, "\"\n");

    return(number);
}

/*
    A Value List is two things:

    (1) A controlling bitmask.	For each one bit in the control,
    a value is in the list.
    (2) A list of values.
*/

Global void
PrintValues(control, clength, ctype, values, name)
	unsigned char	*control;
	short	clength;
	short	ctype;
	unsigned char	*values;
	char   *name;
{
    long    cmask;
    struct ValueListEntry  *p;

    /* first get the control mask */
    if (clength == 1)
	cmask = IByte(control);
    else if (clength == 2)
	    cmask = IShort(control);
	else
	    cmask = ILong(control);

    /* now if it is zero, ignore and return */
    if (cmask == 0)
	return;

    /* there are bits in the controlling bitmask, figure out which */
    /* the ctype is a set type, so this code is similar to PrintSET */
    fprintf(stdout, "%s%20s:\n", Leader, name);
    ModifyIndentLevel(1);
    for (p = TD[ctype].ValueList; p != NULL; p = p->Next)
	{
	    if ((p->Value & cmask) != 0)
		{
#ifdef THIS_IS_A_BUG /* on littleEndian machines */
		    short   m = 4 - p->Length;
#else
		    short m;
		    if (littleEndian)
			m=0;
		    else
			m = 4 - p->Length;
#endif
		    PrintField(values, m, p->Length, p->Type, p->Name);
		    values += 4;
		}
	}
    ModifyIndentLevel(-1);
}

/* PolyText8 and PolyText16 take lists of characters with possible
    font changes in them. */

Global void
PrintTextList8(buf, length, name)
	unsigned char *buf;
	short	length;
	char   *name;
{
    short   n;

    fprintf(stdout, "%s%20s:\n", Leader, name);
    while (length > 1)
	{
	    n = IByte(&buf[0]);
	    if (n != 255)
		{
		    PrintField(buf, 1, 1, INT8, "delta");
		    PrintString8(&buf[2], n, "text item 8 string");
		    buf += n + 2;
		    length -= n + 2;
		}
	    else
		{
		    PrintField(buf, 1, 4, FONT, "font-shift-id");
		    buf += 4;
		    length -= 4;
		}
	}
}

Global void
PrintTextList16(buf, length, name)
	unsigned char *buf;
	short	length;
	char   *name;
{
    short   n;

    fprintf(stdout, "%s%20s:\n", Leader, name);
    while (length > 1)
	{
	    n = IByte(&buf[0]);
	    if (n != 255)
		{
		    PrintField(buf, 1, 1, INT8, "delta");
		    PrintString16(&buf[2], n, "text item 16 string");
		    buf += n + 2;
		    length -= n + 2;
		}
	    else
		{
		    PrintField(buf, 1, 4, FONT, "font-shift-id");
		    buf += 4;
		    length -= 4;
		}
	}
}

Global void
DumpHexBuffer(buf, n)
	unsigned char *buf;
	long	n;
{
    short   i;
    short   column;
    char    h[6] /* one hex or octal character */ ;

    column = 27 + SizeofLeader();
    for (i = 0; i < n; i++)
	{
	    /* get the hex representations */
	    sprintf(h, "%02x",(0xff & buf[i]));

	    /* check if these characters will fit on this line */
	    if ((column + strlen(h) + 1) > MAXline)
		{
		    /* line will be too long -- print it */
		    fprintf(stdout, "\n");
		    column = 0;
		}
	    fprintf(stdout, "%s ", h);
	    column += 3;
	}
}
