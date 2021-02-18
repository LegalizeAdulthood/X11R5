#include "xlib.h"

static Object Sym_Gc;

Generic_Predicate (Gc);

Generic_Equal_Dpy (Gc, GCONTEXT, gc);

Generic_Print (Gc, "#[gcontext %u]", GCONTEXT(x)->gc->gid);

Generic_Get_Display (Gc, GCONTEXT);

Object Make_Gc (finalize, dpy, g) Display *dpy; GC g; {
    register char *p;
    Object gc;

    if (g == None)
	return Sym_None;
    gc = Find_Object (T_Gc, (GENERIC)dpy, Match_X_Obj, g);
    if (Nullp (gc)) {
	p = Get_Bytes (sizeof (struct S_Gc));
	SET (gc, T_Gc, (struct S_Gc *)p);
	GCONTEXT(gc)->tag = Null;
	GCONTEXT(gc)->gc = g;
	GCONTEXT(gc)->dpy = dpy;
	GCONTEXT(gc)->free = 0;
	Register_Object (gc, (GENERIC)dpy, finalize ? P_Free_Gc :
	    (PFO)0, 0);
    }
    return gc;
}

static Object P_Create_Gc (w, g) Object w, g; {
    unsigned long mask;

    Check_Type (w, T_Window);
    mask = Vector_To_Record (g, GC_Size, Sym_Gc, GC_Rec);
    return Make_Gc (1, WINDOW(w)->dpy,
	XCreateGC (WINDOW(w)->dpy, WINDOW(w)->win, mask, &GCV));
}

static Object P_Copy_Gc (gc, w) Object gc, w; {
    GC dst;

    Check_Type (gc, T_Gc);
    Check_Type (w, T_Window);
    dst = XCreateGC (WINDOW(w)->dpy, WINDOW(w)->win, 0L, &GCV);
    XCopyGC (WINDOW(w)->dpy, GCONTEXT(gc)->gc, ~0L, dst);
    return Make_Gc (1, WINDOW(w)->dpy, dst);
}

static Object P_Change_Gc (gc, g) Object gc, g; {
    unsigned long mask;

    Check_Type (gc, T_Gc);
    mask = Vector_To_Record (g, GC_Size, Sym_Gc, GC_Rec);
    XChangeGC (GCONTEXT(gc)->dpy, GCONTEXT(gc)->gc, mask, &GCV);
    return Void;
}

Object P_Free_Gc (g) Object g; {
    Check_Type (g, T_Gc);
    if (!GCONTEXT(g)->free)
	XFreeGC (GCONTEXT(g)->dpy, GCONTEXT(g)->gc);
    Deregister_Object (g);
    GCONTEXT(g)->free = 1;
    return Void;
}

static Object P_Query_Best_Size (d, w, h, shape) Object d, w, h, shape; {
    unsigned int rw, rh;

    Check_Type (d, T_Display);
    if (!XQueryBestSize (DISPLAY(d)->dpy, Symbols_To_Bits (shape, 0,
	    Shape_Syms), DefaultRootWindow (DISPLAY(d)->dpy),
	    Get_Integer (w), Get_Integer (h), &rw, &rh))
	Primitive_Error ("cannot query best shape");
    return Cons (Make_Fixnum (rw), Make_Fixnum (rh));
}

static Object P_Set_Gcontext_Clip_Rectangles (gc, x, y, v, ord)
	Object gc, x, y, v, ord; {
    register XRectangle *p;
    register i, n;
    Alloca_Begin;

    Check_Type (gc, T_Gc);
    Check_Type (v, T_Vector);
    n = VECTOR(v)->size;
    Alloca (p, XRectangle*, n * sizeof (XRectangle));
    for (i = 0; i < n; i++) {
	Object rect = VECTOR(v)->data[i];
	Check_Type (rect, T_Pair);
	if (Internal_Length (rect) != 4)
	    Primitive_Error ("invalid rectangle: ~s", rect);
	p[i].x = Get_Integer (Car (rect)); rect = Cdr (rect);
	p[i].y = Get_Integer (Car (rect)); rect = Cdr (rect);
	p[i].width = Get_Integer (Car (rect)); rect = Cdr (rect);
	p[i].height = Get_Integer (Car (rect));
    }
    XSetClipRectangles (GCONTEXT(gc)->dpy, GCONTEXT(gc)->gc, Get_Integer (x),
	Get_Integer (y), p, n, Symbols_To_Bits (ord, 0, Ordering_Syms));
    Alloca_End;
    return Void;
}

static Object P_Set_Gcontext_Dashlist (gc, off, v) Object gc, off, v; {
    register char *p;
    register i, n, d;
    Alloca_Begin;

    Check_Type (gc, T_Gc);
    Check_Type (v, T_Vector);
    n = VECTOR(v)->size;
    Alloca (p, char*, n);
    for (i = 0; i < n; i++) {
	d = Get_Integer (VECTOR(v)->data[i]);
	if (d < 0 || d > 255)
	    Range_Error (VECTOR(v)->data[i]);
	p[i] = d;
    }
    XSetDashes (GCONTEXT(gc)->dpy, GCONTEXT(gc)->gc, Get_Integer (off), p, n);
    Alloca_End;
    return Void;
}

#define ValidGCValuesBits \
    (GCFunction | GCPlaneMask | GCForeground | GCBackground | GCLineWidth |\
    GCLineStyle | GCCapStyle | GCJoinStyle | GCFillStyle | GCFillRule |\
    GCTile | GCStipple | GCTileStipXOrigin | GCTileStipYOrigin | GCFont |\
    GCSubwindowMode | GCGraphicsExposures | GCClipXOrigin | GCClipYOrigin |\
    GCDashOffset | GCArcMode)

static Object P_Get_Gc_Values (gc) Object gc; {
    unsigned long mask = ValidGCValuesBits;

    Check_Type (gc, T_Gc);
    if (!XGetGCValues (GCONTEXT(gc)->dpy, GCONTEXT(gc)->gc, mask, &GCV))
	Primitive_Error ("cannot get gcontext values");
    return Record_To_Vector (GC_Rec, GC_Size, Sym_Gc, GCONTEXT(gc)->dpy,
	mask);
}

init_xlib_gcontext () {
    Define_Symbol (&Sym_Gc, "gcontext");
    Generic_Define (Gc, "gcontext", "gcontext?");
    Define_Primitive (P_Gc_Display,      "gcontext-display",    1, 1, EVAL);
    Define_Primitive (P_Create_Gc,       "xlib-create-gcontext",2, 2, EVAL);
    Define_Primitive (P_Copy_Gc,         "copy-gcontext",       2, 2, EVAL);
    Define_Primitive (P_Change_Gc,       "xlib-change-gcontext",2, 2, EVAL);
    Define_Primitive (P_Free_Gc,         "free-gcontext",       1, 1, EVAL);
    Define_Primitive (P_Query_Best_Size, "query-best-size",     4, 4, EVAL);
    Define_Primitive (P_Set_Gcontext_Clip_Rectangles,
	"set-gcontext-clip-rectangles!",                        5, 5, EVAL);
    Define_Primitive (P_Set_Gcontext_Dashlist,
	"set-gcontext-dashlist!",                               3, 3, EVAL);
    Define_Primitive (P_Get_Gc_Values,
			"xlib-get-gcontext-values",             1, 1, EVAL);
}
