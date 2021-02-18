#include "xlib.h"

extern XDrawText(), XDrawText16();

static Two_Byte (format) Object format; {
    Check_Type (format, T_Symbol);
    if (EQ(format, Intern ("1-byte")))
	return 0;
    else if (EQ(format, Intern ("2-byte")))
	return 1;
    Primitive_Error ("index format must be '1-byte or '2-byte");
    /*NOTREACHED*/
}

static Get_1_Byte_Char (x) Object x; {
    register c = Get_Integer (x);
    if (c < 0 || c > 255)
	Range_Error (x);
    return c;
}

static Get_2_Byte_Char (x) Object x; {
    register c = Get_Integer (x);
    if (c < 0 || c > 65535)
	Range_Error (x);
    return c;
}

/* Calculation of text widths and extents should not be done using
 * the Xlib functions.  For instance, the values returned by
 * XTextExtents() are only shorts and can therefore overflow for
 * long strings.
 */

static Object Internal_Text_Metrics (font, t, f, width) Object font, t, f; {
    char *s;
    XChar2b *s2;
    XFontStruct *info;
    Object *data;
    register i, n;
    int dir, fasc, fdesc;
    Alloca_Begin;

    Check_Type (font, T_Font);
    info = FONT(font)->info;
    Check_Type (t, T_Vector);
    n = VECTOR(t)->size;
    data = VECTOR(t)->data;
    if (Two_Byte (f)) {
	Alloca (s2, XChar2b*, n * sizeof (XChar2b));
	for (i = 0; i < n; i++) {
	    register c = Get_2_Byte_Char (data[i]);
	    s2[i].byte1 = (c >> 8) & 0xff;
	    s2[i].byte2 = c & 0xff;
	}
	if (width)
	    i = XTextWidth16 (info, s2, n);
	else
	    XTextExtents16 (info, s2, n, &dir, &fasc, &fdesc, &CI);
    } else {
	Alloca (s, char*, n);
	for (i = 0; i < n; i++)
	    s[i] = Get_1_Byte_Char (data[i]);
	if (width)
	    i = XTextWidth (info, s, n);
	else
	    XTextExtents (info, s, n, &dir, &fasc, &fdesc, &CI);
    }
    Alloca_End;
    return width ? Make_Integer (i) : Record_To_Vector (Char_Info_Rec,
	Char_Info_Size, Sym_Char_Info, FONT(font)->dpy, ~0L);
}

static Object P_Text_Width (font, t, f) Object font, t, f; {
    return Internal_Text_Metrics (font, t, f, 1);
}

static Object P_Text_Extents (font, t, f) Object font, t, f; {
    return Internal_Text_Metrics (font, t, f, 0);
}

static Object P_Draw_Image_Text (d, gc, x, y, t, f) Object d, gc, x, y, t, f; {
    Display *dpy;
    Drawable dr = Get_Drawable (d, &dpy);
    Object *data;
    register i, n;
    char *s;
    XChar2b *s2;
    Alloca_Begin;

    Check_Type (gc, T_Gc);
    Check_Type (t, T_Vector);
    n = VECTOR(t)->size;
    data = VECTOR(t)->data;
    if (Two_Byte (f)) {
	Alloca (s2, XChar2b*, n * sizeof (XChar2b));
	for (i = 0; i < n; i++) {
	    register c = Get_2_Byte_Char (data[i]);
	    s2[i].byte1 = (c >> 8) & 0xff;
	    s2[i].byte2 = c & 0xff;
	}
	XDrawImageString16 (dpy, dr, GCONTEXT(gc)->gc, Get_Integer (x),
	    Get_Integer (y), s2, n);
    } else {
	Alloca (s, char*, n);
	for (i = 0; i < n; i++)
	    s[i] = Get_1_Byte_Char (data[i]);
	XDrawImageString (dpy, dr, GCONTEXT(gc)->gc, Get_Integer (x),
	    Get_Integer (y), s, n);
    }
    Alloca_End;
    return Void;
}

static Object P_Draw_Poly_Text (d, gc, x, y, t, f) Object d, gc, x, y, t, f; {
    Display *dpy;
    Drawable dr = Get_Drawable (d, &dpy);
    Object *data;
    register i, n, j, k;
    int twobyte, nitems;
    XTextItem *items;
    int (*func)();
    Alloca_Begin;

    Check_Type (gc, T_Gc);
    twobyte = Two_Byte (f);
    func = twobyte ? XDrawText16 : XDrawText;
    Check_Type (t, T_Vector);
    if ((n = VECTOR(t)->size) == 0)
	return Void;
    for (data = VECTOR(t)->data, i = 0, nitems = 1; i < n; i++)
	if (TYPE(data[i]) == T_Font) nitems++;
    Alloca (items, XTextItem*, nitems * sizeof (XTextItem));
    items[0].delta = 0;
    items[0].font = None;
    for (j = k = i = 0; i <= n; i++) {
	if (i == n || TYPE(data[i]) == T_Font) {
	    items[j].nchars = i-k;
	    if (twobyte) {
		register XChar2b *p;
		Alloca (((XTextItem16 *)items)[j].chars, XChar2b*,
			(i-k) * sizeof (XChar2b));
		p = ((XTextItem16 *)items)[j].chars;
		for ( ; k < i; k++, p++) {
		    register c = Get_2_Byte_Char (data[k]);
		    p->byte1 = (c >> 8) & 0xff;
		    p->byte2 = c & 0xff;
		}
	    } else {
		register char *p;
		Alloca (items[j].chars, char*, i-k);
		p = items[j].chars;
		for ( ; k < i; k++)
		    *p++ = Get_1_Byte_Char (data[k]);
	    }
	    k++;
	    j++;
	    if (i < n) {
		items[j].delta = 0;
		Open_Font_Maybe (data[i]);
		items[j].font = FONT(data[i])->id;
	    }
	}
    }
    (*func)(dpy, dr, GCONTEXT(gc)->gc, Get_Integer (x), Get_Integer (y),
	items, nitems);
    Alloca_End;
    return Void;
}

init_xlib_text () {
    Define_Primitive (P_Text_Width,       "text-width",        3, 3, EVAL);
    Define_Primitive (P_Text_Extents,     "xlib-text-extents", 3, 3, EVAL);
    Define_Primitive (P_Draw_Image_Text,  "draw-image-text",   6, 6, EVAL);
    Define_Primitive (P_Draw_Poly_Text,   "draw-poly-text",    6, 6, EVAL);
}
