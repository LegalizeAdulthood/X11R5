#include <varargs.h>

#include "xlib.h"

Object Sym_None;

Match_X_Obj (x, v) Object x; va_list v; {
    register type = TYPE(x);

    if (type == T_Display) {
	return 1;
    } else if (type == T_Gc) {
	return va_arg (v, GC) == GCONTEXT(x)->gc;
    } else if (type == T_Pixel) {
	return va_arg (v, unsigned long) == PIXEL(x)->pix;
    } else if (type == T_Pixmap) {
	return va_arg (v, Pixmap) == PIXMAP(x)->pm;
    } else if (type == T_Window) {
	return va_arg (v, Window) == WINDOW(x)->win;
    } else if (type == T_Font) {
	return va_arg (v, Font) == FONT(x)->id;
    } else if (type == T_Colormap) {
	return va_arg (v, Colormap) == COLORMAP(x)->cm;
    } else if (type == T_Color) {
	return va_arg (v, unsigned int) == COLOR(x)->c.red
	    && va_arg (v, unsigned int) == COLOR(x)->c.green
	    && va_arg (v, unsigned int) == COLOR(x)->c.blue;
    } else if (type == T_Cursor) {
	return va_arg (v, Cursor) == CURSOR(x)->cursor;
    } else if (type == T_Atom) {
	return va_arg (v, Atom) == ATOM(x)->atom;
    } else Panic ("Match_X_Obj");
    return 0;
}

Object P_Window_Unique_Id (w) Object w; {
    register id;

    Check_Type (w, T_Window);
    id = Unique_Id (w);
    return id > 0 ? Make_Fixnum (id) : False;
}

init_xlib_objects () {
    Define_Symbol (&Sym_None, "none");
    Define_Primitive (P_Window_Unique_Id, "window-unique-id", 1, 1, EVAL);
}
