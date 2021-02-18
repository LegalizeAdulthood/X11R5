#include "xlib.h"

Generic_Predicate (Pixmap);

Generic_Equal_Dpy (Pixmap, PIXMAP, pm);

Generic_Print (Pixmap, "#[pixmap %u]", PIXMAP(x)->pm);

Generic_Get_Display (Pixmap, PIXMAP);

Object Make_Pixmap (dpy, pix) Display *dpy; Pixmap pix; {
    register char *p;
    Object pm;

    if (pix == None)
	return Sym_None;
    pm = Find_Object (T_Pixmap, (GENERIC)dpy, Match_X_Obj, pix);
    if (Nullp (pm)) {
	p = Get_Bytes (sizeof (struct S_Pixmap));
	SET (pm, T_Pixmap, (struct S_Pixmap *)p);
	PIXMAP(pm)->tag = Null;
	PIXMAP(pm)->pm = pix;
	PIXMAP(pm)->dpy = dpy;
	PIXMAP(pm)->free = 0;
	Register_Object (pm, (GENERIC)dpy, P_Free_Pixmap, 0);
    }
    return pm;
}

Pixmap Get_Pixmap (p) Object p; {
    Check_Type (p, T_Pixmap);
    return PIXMAP(p)->pm;
}

Object P_Free_Pixmap (p) Object p; {
    Check_Type (p, T_Pixmap);
    if (!PIXMAP(p)->free)
	XFreePixmap (PIXMAP(p)->dpy, PIXMAP(p)->pm);
    Deregister_Object (p);
    PIXMAP(p)->free = 1;
    return Void;
}

static Object P_Create_Pixmap (d, w, h, depth) Object d, w, h, depth; {
    Display *dpy;
    Drawable dr = Get_Drawable (d, &dpy);

    return Make_Pixmap (dpy, XCreatePixmap (dpy, dr, Get_Integer (w),
	Get_Integer (h), Get_Integer (depth)));
}

static Object P_Create_Bitmap_From_Data (win, data, pw, ph)
	Object win, data, pw, ph; {
    register w, h;

    Check_Type (win, T_Window);
    Check_Type (data, T_String);
    w = Get_Integer (pw);
    h = Get_Integer (ph);
    if (w * h > 8 * STRING(data)->size)
	Primitive_Error ("bitmap too small");
    return Make_Pixmap (WINDOW(win)->dpy,
	XCreateBitmapFromData (WINDOW(win)->dpy, WINDOW(win)->win,
	    STRING(data)->data, w, h));
}

static Object P_Create_Pixmap_From_Bitmap_Data (win, data, pw, ph, fg, bg,
	depth) Object win, data, pw, ph, fg, bg, depth; {
    register w, h;

    Check_Type (win, T_Window);
    Check_Type (data, T_String);
    w = Get_Integer (pw);
    h = Get_Integer (ph);
    if (w * h > 8 * STRING(data)->size)
	Primitive_Error ("bitmap too small");
    return Make_Pixmap (WINDOW(win)->dpy,
	XCreatePixmapFromBitmapData (WINDOW(win)->dpy, WINDOW(win)->win,
	    STRING(data)->data, w, h, Get_Pixel (fg), Get_Pixel (bg),
		Get_Integer (depth)));
}

static Object P_Read_Bitmap_File (d, fn) Object d, fn; {
    Display *dpy;
    Drawable dr = Get_Drawable (d, &dpy);
    char *filename;
    unsigned width, height;
    int r, xhot, yhot;
    Pixmap *bitmap;
    Object t, ret, x;
    GC_Node2;
    Declare_C_Strings;

    Make_C_String (fn, filename);
    Disable_Interrupts;
    r = XReadBitmapFile (dpy, dr, filename, &width, &height, &bitmap,
	&xhot, &yhot);
    Enable_Interrupts;
    Dispose_C_Strings;
    if (r != BitmapSuccess)
	return Bits_To_Symbols ((unsigned long)r, 0, Bitmapstatus_Syms);
    t = ret = P_Make_List (Make_Fixnum (5), Null);
    GC_Link2 (ret, t);
    x = Make_Pixmap (dpy, bitmap);
    Car (t) = x; t = Cdr (t);
    Car (t) = Make_Fixnum (width); t = Cdr (t);
    Car (t) = Make_Fixnum (height); t = Cdr (t);
    Car (t) = Make_Fixnum (xhot); t = Cdr (t);
    Car (t) = Make_Fixnum (yhot);
    GC_Unlink;
    return ret;
}

static Object P_Write_Bitmap_File (argc, argv) Object *argv; {
    Object file;
    Pixmap pm;
    char *filename;
    int ret, xhot = -1, yhot = -1;
    Declare_C_Strings;

    file = argv[0];
    Make_C_String (file, filename);
    pm = Get_Pixmap (argv[1]);
    if (argc == 5)
	Primitive_Error ("both x-hot and y-hot must be specified");
    if (argc == 6) {
	xhot = Get_Integer (argv[4]);
	yhot = Get_Integer (argv[5]);
    }
    Disable_Interrupts;
    ret = XWriteBitmapFile (PIXMAP(argv[1])->dpy, filename, pm,
	Get_Integer (argv[2]), Get_Integer (argv[3]), xhot, yhot);
    Enable_Interrupts;
    Dispose_C_Strings;
    return Bits_To_Symbols ((unsigned long)ret, 0, Bitmapstatus_Syms);
}

init_xlib_pixmap () {
    Generic_Define (Pixmap, "pixmap", "pixmap?");
    Define_Primitive (P_Pixmap_Display,    "pixmap-display",    1, 1, EVAL);
    Define_Primitive (P_Free_Pixmap,       "free-pixmap",       1, 1, EVAL);
    Define_Primitive (P_Create_Pixmap,     "create-pixmap",     4, 4, EVAL);
    Define_Primitive (P_Create_Bitmap_From_Data,
			"create-bitmap-from-data",              4, 4, EVAL);
    Define_Primitive (P_Create_Pixmap_From_Bitmap_Data,
			"create-pixmap-from-bitmap-data",       7, 7, EVAL);
    Define_Primitive (P_Read_Bitmap_File,  "read-bitmap-file",  2, 2, EVAL);
    Define_Primitive (P_Write_Bitmap_File, "write-bitmap-file", 4, 6, VARARGS);
}
