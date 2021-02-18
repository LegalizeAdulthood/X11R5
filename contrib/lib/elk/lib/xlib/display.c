#include "xlib.h"

static Display_Visit (dp, f) Object *dp; int (*f)(); {
    (*f)(&DISPLAY(*dp)->after);
}

Generic_Predicate (Display);

Generic_Equal (Display, DISPLAY, dpy);

static Display_Print (d, port, raw, depth, length) Object d, port; {
    Printf (port, "#[display %u %s]", (unsigned)DISPLAY(d)->dpy,
	DisplayString (DISPLAY(d)->dpy));
}

Object Make_Display (finalize, dpy) Display *dpy; {
    char *p;
    Object d;

    d = Find_Object (T_Display, (GENERIC)dpy, Match_X_Obj);
    if (Nullp (d)) {
	p = Get_Bytes (sizeof (struct S_Display));
	SET (d, T_Display, (struct S_Display *)p);
	DISPLAY(d)->dpy = dpy;
	DISPLAY(d)->free = 0;
	DISPLAY(d)->after = False;
	Register_Object (d, (GENERIC)dpy, finalize ? P_Close_Display :
	    (PFO)0, 1);
    }
    return d;
}

static Object P_Open_Display (argc, argv) Object *argv; {
    register char *s;
    Object name;
    Display *dpy;
    Declare_C_Strings;

    if (argc == 1) {
	name = argv[0];
	Make_C_String (name, s);
	if ((dpy = XOpenDisplay (s)) == 0)
	    Primitive_Error ("cannot open display ~s", name);
	Dispose_C_Strings;
    } else if ((dpy = XOpenDisplay ((char *)0)) == 0)
	Primitive_Error ("cannot open display");
    return Make_Display (1, dpy);
}

Object P_Close_Display (d) Object d; {
    register struct S_Display *p;

    Check_Type (d, T_Display);
    p = DISPLAY(d);
    if (!p->free) {
	Terminate_Group ((GENERIC)p->dpy);
	XCloseDisplay (p->dpy);
    }
    Deregister_Object (d);
    p->free = 1;
    return Void;
}

static Object P_Display_Default_Root_Window (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Window (0, DISPLAY(d)->dpy,
	DefaultRootWindow (DISPLAY(d)->dpy));
}

static Object P_Display_Default_Colormap (d) Object d; {
    register Display *dpy;

    Check_Type (d, T_Display);
    dpy = DISPLAY(d)->dpy;
    return Make_Colormap (0, dpy, DefaultColormap (dpy, DefaultScreen (dpy)));
}

static Object P_Display_Default_Gcontext (d) Object d; {
    register Display *dpy;

    Check_Type (d, T_Display);
    dpy = DISPLAY(d)->dpy;
    return Make_Gc (0, dpy, DefaultGC (dpy, DefaultScreen (dpy)));
}

static Object P_Display_Default_Depth (d) Object d; {
    register Display *dpy;

    Check_Type (d, T_Display);
    dpy = DISPLAY(d)->dpy;
    return Make_Fixnum (DefaultDepth (dpy, DefaultScreen (dpy)));
}

static Object P_Display_Default_Screen_Number (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (DefaultScreen (DISPLAY(d)->dpy));
}

Get_Screen_Number (dpy, scr) Display *dpy; Object scr; {
    register s;

    if ((s = Get_Integer (scr)) < 0 || s > ScreenCount (dpy)-1)
	Primitive_Error ("invalid screen number");
    return s;
}

static Object P_Display_Cells (d, scr) Object d, scr; {
    Check_Type (d, T_Display);
    return Make_Integer (DisplayCells (DISPLAY(d)->dpy,
	Get_Screen_Number (DISPLAY(d)->dpy, scr)));
}

static Object P_Display_Planes (d, scr) Object d, scr; {
    Check_Type (d, T_Display);
    return Make_Integer (DisplayPlanes (DISPLAY(d)->dpy, 
	Get_Screen_Number (DISPLAY(d)->dpy, scr)));
}

static Object P_Display_String (d) Object d; {
    register char *s;

    Check_Type (d, T_Display);
    s = DisplayString (DISPLAY(d)->dpy);
    return Make_String (s, strlen (s));
}

static Object P_Display_Vendor (d) Object d; {
    register char *s;
    Object ret, name;
    GC_Node;

    Check_Type (d, T_Display);
    s = ServerVendor (DISPLAY(d)->dpy);
    name = Make_String (s, strlen (s));
    GC_Link (name);
    ret = Cons (Null, Make_Fixnum (VendorRelease (DISPLAY(d)->dpy)));
    Car (ret) = name;
    GC_Unlink;
    return ret;
}

static Object P_Display_Protocol_Version (d) Object d; {
    Check_Type (d, T_Display);
    return Cons (Make_Fixnum (ProtocolVersion (DISPLAY(d)->dpy)),
	Make_Fixnum (ProtocolRevision (DISPLAY(d)->dpy)));
}

static Object P_Display_Screen_Count (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (ScreenCount (DISPLAY(d)->dpy));
}

static Object P_Display_Image_Byte_Order (d) Object d; {
    Check_Type (d, T_Display);
    return Bits_To_Symbols ((unsigned long)ImageByteOrder (DISPLAY(d)->dpy),
	0, Byte_Order_Syms);
}

static Object P_Display_Bitmap_Unit (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (BitmapUnit (DISPLAY(d)->dpy));
}

static Object P_Display_Bitmap_Bit_Order (d) Object d; {
    Check_Type (d, T_Display);
    return Bits_To_Symbols ((unsigned long)BitmapBitOrder (DISPLAY(d)->dpy),
	0, Byte_Order_Syms);
}

static Object P_Display_Bitmap_Pad (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (BitmapPad (DISPLAY(d)->dpy));
}

static Object P_Display_Width (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (DisplayWidth (DISPLAY(d)->dpy,
	DefaultScreen (DISPLAY(d)->dpy)));
}

static Object P_Display_Height (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (DisplayHeight (DISPLAY(d)->dpy,
	DefaultScreen (DISPLAY(d)->dpy)));
}

static Object P_Display_Width_Mm (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (DisplayWidthMM (DISPLAY(d)->dpy,
	DefaultScreen (DISPLAY(d)->dpy)));
}

static Object P_Display_Height_Mm (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Fixnum (DisplayHeightMM (DISPLAY(d)->dpy,
	DefaultScreen (DISPLAY(d)->dpy)));
}

static Object P_Display_Motion_Buffer_Size (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Integer ((int)XDisplayMotionBufferSize (DISPLAY(d)->dpy));
}

static Object P_Display_Flush_Output (d) Object d; {
    Check_Type (d, T_Display);
    XFlush (DISPLAY(d)->dpy);
    return Void;
}

static Object P_Display_Wait_Output (d, discard) Object d, discard; {
    Check_Type (d, T_Display);
    Check_Type (discard, T_Boolean);
    XSync (DISPLAY(d)->dpy, EQ(discard, True));
    return Void;
}

static Object P_No_Op (d) Object d; {
    Check_Type (d, T_Display);
    XNoOp (DISPLAY(d)->dpy);
    return Void;
}

static Object P_List_Depths (d, scr) Object d, scr; {
    int num;
    register *p, i;
    Object ret;

    Check_Type (d, T_Display);
    if (!(p = XListDepths (DISPLAY(d)->dpy,
	    Get_Screen_Number (DISPLAY(d)->dpy, scr), &num)))
	return False;
    ret = Make_Vector (num, Null);
    for (i = 0; i < num; i++)
	VECTOR(ret)->data[i] = Make_Fixnum (p[i]);
    XFree (p);
    return ret;
}

static Object P_List_Pixmap_Formats (d) Object d; {
    register XPixmapFormatValues *p;
    int num;
    register i;
    Object ret;
    GC_Node;

    Check_Type (d, T_Display);
    if (!(p = XListPixmapFormats (DISPLAY(d)->dpy, &num)))
	return False;
    ret = Make_Vector (num, Null);
    GC_Link (ret);
    for (i = 0; i < num; i++) {
	Object t = P_Make_List (Make_Fixnum (3), Null);
	VECTOR(ret)->data[i] = t;
	Car (t) = Make_Fixnum (p[i].depth); t = Cdr (t);
	Car (t) = Make_Fixnum (p[i].bits_per_pixel); t = Cdr (t);
	Car (t) = Make_Fixnum (p[i].scanline_pad);
    }
    GC_Unlink;
    XFree (p);
    return ret;
}

init_xlib_display () {
    T_Display = Define_Type (0, "display", NOFUNC, sizeof (struct S_Display),
	Display_Equal, Display_Equal, Display_Print, Display_Visit);
    Define_Primitive (P_Displayp,        "display?",         1, 1, EVAL);
    Define_Primitive (P_Open_Display,    "open-display",     0, 1, VARARGS);
    Define_Primitive (P_Close_Display,   "close-display",    1, 1, EVAL);
    Define_Primitive (P_Display_Default_Root_Window,
			"display-default-root-window",       1, 1, EVAL);
    Define_Primitive (P_Display_Default_Colormap,
			"display-default-colormap",          1, 1, EVAL);
    Define_Primitive (P_Display_Default_Gcontext,
			"display-default-gcontext",          1, 1, EVAL);
    Define_Primitive (P_Display_Default_Depth,
			"display-default-depth",             1, 1, EVAL);
    Define_Primitive (P_Display_Default_Screen_Number,
			"display-default-screen-number",     1, 1, EVAL);
    Define_Primitive (P_Display_Cells,   "display-cells",    2, 2, EVAL);
    Define_Primitive (P_Display_Planes,  "display-planes",   2, 2, EVAL);
    Define_Primitive (P_Display_String,  "display-string",   1, 1, EVAL);
    Define_Primitive (P_Display_Vendor,  "display-vendor",   1, 1, EVAL);
    Define_Primitive (P_Display_Protocol_Version,
			"display-protocol-version",          1, 1, EVAL);
    Define_Primitive (P_Display_Screen_Count,
			"display-screen-count",              1, 1, EVAL);
    Define_Primitive (P_Display_Image_Byte_Order,
			"display-image-byte-order",          1, 1, EVAL);
    Define_Primitive (P_Display_Bitmap_Unit,
			"display-bitmap-unit",               1, 1, EVAL);
    Define_Primitive (P_Display_Bitmap_Bit_Order,
			"display-bitmap-bit-order",          1, 1, EVAL);
    Define_Primitive (P_Display_Bitmap_Pad,
			"display-bitmap-pad",                1, 1, EVAL);
    Define_Primitive (P_Display_Width,   "display-width",    1, 1, EVAL);
    Define_Primitive (P_Display_Height,  "display-height",   1, 1, EVAL);
    Define_Primitive (P_Display_Width_Mm,"display-width-mm", 1, 1, EVAL);
    Define_Primitive (P_Display_Height_Mm,
			"display-height-mm",                 1, 1, EVAL);
    Define_Primitive (P_Display_Motion_Buffer_Size,
			"display-motion-buffer-size",        1, 1, EVAL);
    Define_Primitive (P_Display_Flush_Output,
			"display-flush-output",              1, 1, EVAL);
    Define_Primitive (P_Display_Wait_Output,
			"display-wait-output",               2, 2, EVAL);
    Define_Primitive (P_No_Op,           "no-op",            1, 1, EVAL);
    Define_Primitive (P_List_Depths,      "list-depths",     2, 2, EVAL);
    Define_Primitive (P_List_Pixmap_Formats,
			"list-pixmap-formats",               1, 1, EVAL);
}
