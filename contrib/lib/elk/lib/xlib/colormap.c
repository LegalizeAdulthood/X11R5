#include "xlib.h"

Generic_Predicate (Colormap);

Generic_Equal_Dpy (Colormap, COLORMAP, cm);

Generic_Print (Colormap, "#[colormap %u]", COLORMAP(x)->cm);

Generic_Get_Display (Colormap, COLORMAP);

Object Make_Colormap (finalize, dpy, cmap) Display *dpy; Colormap cmap; {
    register char *p;
    Object cm;

    if (cmap == None)
	return Sym_None;
    cm = Find_Object (T_Colormap, (GENERIC)dpy, Match_X_Obj, cmap);
    if (Nullp (cm)) {
	p = Get_Bytes (sizeof (struct S_Colormap));
	SET (cm, T_Colormap, (struct S_Colormap *)p);
	COLORMAP(cm)->tag = Null;
	COLORMAP(cm)->cm = cmap;
	COLORMAP(cm)->dpy = dpy;
	COLORMAP(cm)->free = 0;
	Register_Object (cm, (GENERIC)dpy, finalize ? P_Free_Colormap :
	    (PFO)0, 0);
    }
    return cm;
}

Colormap Get_Colormap (c) Object c; {
    Check_Type (c, T_Colormap);
    return COLORMAP(c)->cm;
}

Object P_Free_Colormap (c) Object c; {
    Check_Type (c, T_Colormap);
    if (!COLORMAP(c)->free)
	XFreeColormap (COLORMAP(c)->dpy, COLORMAP(c)->cm);
    Deregister_Object (c);
    COLORMAP(c)->free = 1;
    return Void;
}

init_xlib_colormap () {
    Generic_Define (Colormap, "colormap", "colormap?");
    Define_Primitive (P_Colormap_Display, "colormap-display", 1, 1, EVAL);
    Define_Primitive (P_Free_Colormap,    "free-colormap",    1, 1, EVAL);
}
