#include "xlib.h"

Generic_Predicate (Cursor);

Generic_Equal_Dpy (Cursor, CURSOR, cursor);

Generic_Print (Cursor, "#[cursor %u]", CURSOR(x)->cursor);

Generic_Get_Display (Cursor, CURSOR);

Object Make_Cursor (dpy, cursor) Display *dpy; Cursor cursor; {
    register char *p;
    Object c;

    if (cursor == None)
	return Sym_None;
    c = Find_Object (T_Cursor, (GENERIC)dpy, Match_X_Obj, cursor);
    if (Nullp (c)) {
	p = Get_Bytes (sizeof (struct S_Cursor));
	SET (c, T_Cursor, (struct S_Cursor *)p);
	CURSOR(c)->tag = Null;
	CURSOR(c)->cursor = cursor;
	CURSOR(c)->dpy = dpy;
	CURSOR(c)->free = 0;
	Register_Object (c, (GENERIC)dpy, P_Free_Cursor, 0);
    }
    return c;
}

Cursor Get_Cursor (c) Object c; {
    if (EQ(c, Sym_None))
	return None;
    Check_Type (c, T_Cursor);
    return CURSOR(c)->cursor;
}

Object P_Free_Cursor (c) Object c; {
    Check_Type (c, T_Cursor);
    if (!CURSOR(c)->free)
	XFreeCursor (CURSOR(c)->dpy, CURSOR(c)->cursor);
    Deregister_Object (c);
    CURSOR(c)->free = 1;
    return Void;
}

static Object P_Create_Cursor (srcp, maskp, x, y, f, b)
	Object srcp, maskp, x, y, f, b; {
    Pixmap sp = Get_Pixmap (srcp), mp;
    Display *d = PIXMAP(srcp)->dpy;

    mp = EQ(maskp, Sym_None) ? None : Get_Pixmap (maskp);
    return Make_Cursor (d, XCreatePixmapCursor (d, sp, mp,
	Get_Color (f), Get_Color (b), Get_Integer (x), Get_Integer (y)));
}

static Object P_Create_Glyph_Cursor (srcf, srcc, maskf, maskc, f, b)
	Object srcf, srcc, maskf, maskc, f, b; {
    Font sf = Get_Font (srcf), mf;
    Display *d = FONT(srcf)->dpy;

    mf = EQ(maskf, Sym_None) ? None : Get_Font (maskf);
    return Make_Cursor (d, XCreateGlyphCursor (d, sf, mf,
	Get_Integer (srcc), mf == None ? 0 : Get_Integer (maskc),
	Get_Color (f), Get_Color (b)));
}

static Object P_Recolor_Cursor (c, f, b) Object c, f, b; {
    Check_Type (c, T_Cursor);
    XRecolorCursor (CURSOR(c)->dpy, CURSOR(c)->cursor, Get_Color (f),
	Get_Color (b));
    return Void;
}

init_xlib_cursor () {
    Generic_Define (Cursor, "cursor", "cursor?");
    Define_Primitive (P_Cursor_Display, "cursor-display", 1, 1, EVAL);
    Define_Primitive (P_Free_Cursor,    "free-cursor",    1, 1, EVAL);
    Define_Primitive (P_Create_Cursor,  "create-cursor",  6, 6, EVAL);
    Define_Primitive (P_Create_Glyph_Cursor, "create-glyph-cursor",
							  6, 6, EVAL);
    Define_Primitive (P_Recolor_Cursor, "recolor-cursor", 3, 3, EVAL);
}
