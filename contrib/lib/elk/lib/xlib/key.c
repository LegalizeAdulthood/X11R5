#include "xlib.h"

static Object P_Display_Min_Keycode (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Integer (DISPLAY(d)->dpy->min_keycode);
}

static Object P_Display_Max_Keycode (d) Object d; {
    Check_Type (d, T_Display);
    return Make_Integer (DISPLAY(d)->dpy->max_keycode);
}

static Object P_Display_Keysyms_Per_Keycode (d) Object d; {
    Check_Type (d, T_Display);
    /* Force initialization: */
    Disable_Interrupts;
    (void)XKeycodeToKeysym (DISPLAY(d)->dpy, DISPLAY(d)->dpy->min_keycode, 0);
    Enable_Interrupts;
    return Make_Integer (DISPLAY(d)->dpy->keysyms_per_keycode);
}

static Object P_String_To_Keysym (s) Object s; {
    register char *str;
    KeySym k;
    Declare_C_Strings;

    Make_C_String (s, str);
    k = XStringToKeysym (str);
    Dispose_C_Strings;
    return k == NoSymbol ? False : Make_Unsigned ((unsigned)k);
}

static Object P_Keysym_To_String (k) Object k; {
    register char *s;

    s = XKeysymToString (Get_Integer (k));
    return s ? Make_String (s, strlen (s)) : False;
}

static Object P_Keycode_To_Keysym (d, k, index) Object d, k, index; {
    Object ret;

    Check_Type (d, T_Display);
    Disable_Interrupts;
    ret = Make_Unsigned (XKeycodeToKeysym (DISPLAY(d)->dpy, Get_Integer (k),
	Get_Integer (index)));
    Enable_Interrupts;
    return ret;
}

static Object P_Keysym_To_Keycode (d, k) Object d, k; {
    Object ret;

    Check_Type (d, T_Display);
    Disable_Interrupts;
    ret = Make_Unsigned (XKeysymToKeycode (DISPLAY(d)->dpy, Get_Integer (k)));
    Enable_Interrupts;
    return ret;
}

static Object P_Lookup_String (d, k, mask) Object d, k, mask; {
    XKeyEvent e;
    char buf[1024];
    register len;
    KeySym keysym_return;
    XComposeStatus status_return;

    Check_Type (d, T_Display);
    e.display = DISPLAY(d)->dpy;
    e.keycode = Get_Integer (k);
    e.state = Symbols_To_Bits (mask, 1, State_Syms);
    Disable_Interrupts;
    len = XLookupString (&e, buf, 1024, &keysym_return, &status_return);
    Enable_Interrupts;
    return Make_String (buf, len);
}

static Object P_Rebind_Keysym (d, k, mods, str) Object d, k, mods, str; {
    KeySym *p;
    register i, n;
    Alloca_Begin;

    Check_Type (d, T_Display);
    Check_Type (str, T_String);
    Check_Type (mods, T_Vector);
    n = VECTOR(mods)->size;
    Alloca (p, KeySym*, n * sizeof (KeySym));
    for (i = 0; i < n; i++)
	p[i] = Get_Integer (VECTOR(mods)->data[i]);
    XRebindKeysym (DISPLAY(d)->dpy, Get_Integer (k), p, n, 
	STRING(str)->data, STRING(str)->size);
    Alloca_End;
    return Void;
}

static Object P_Refresh_Keyboard_Mapping (w, event) Object w, event; {
    static XMappingEvent fake;

    Check_Type (w, T_Window);
    fake.type = MappingNotify;
    fake.display = WINDOW(w)->dpy;
    fake.window = WINDOW(w)->win;
    fake.request = Symbols_To_Bits (event, 0, Mapping_Syms);
    XRefreshKeyboardMapping (&fake);
    return Void;
}

init_xlib_key () {
    Define_Primitive (P_Display_Min_Keycode, "display-min-keycode",
							      1, 1, EVAL);
    Define_Primitive (P_Display_Max_Keycode, "display-max-keycode",
							      1, 1, EVAL);
    Define_Primitive (P_Display_Keysyms_Per_Keycode,
			"display-keysyms-per-keycode",        1, 1, EVAL);
    Define_Primitive (P_String_To_Keysym,  "string->keysym",  1, 1, EVAL);
    Define_Primitive (P_Keysym_To_String,  "keysym->string",  1, 1, EVAL);
    Define_Primitive (P_Keycode_To_Keysym, "keycode->keysym", 3, 3, EVAL);
    Define_Primitive (P_Keysym_To_Keycode, "keysym->keycode", 2, 2, EVAL);
    Define_Primitive (P_Lookup_String,     "lookup-string",   3, 3, EVAL);
    Define_Primitive (P_Rebind_Keysym,     "rebind-keysym",   4, 4, EVAL);
    Define_Primitive (P_Refresh_Keyboard_Mapping,
			"refresh-keyboard-mapping",           2, 2, EVAL);
}
