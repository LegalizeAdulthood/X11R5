#include "xt.h"

extern void XtManageChildren(), XtUnmanageChildren();

static Object P_Destroy_Widget();

Generic_Predicate (Widget);

Generic_Equal (Widget, WIDGET, widget);

Generic_Print (Widget, "#[widget %u]", POINTER(x));

Object Make_Widget (widget) Widget widget; {
    register char *p;
    Object w;

    if (widget == 0)
	return Sym_None;
    w = Find_Object (T_Widget, (GENERIC)0, Match_Xt_Obj, widget);
    if (Nullp (w)) {
	p = Get_Bytes (sizeof (struct S_Widget));
	SET (w, T_Widget, (struct S_Widget *)p);
	WIDGET(w)->tag = Null;
	WIDGET(w)->widget = widget;
	WIDGET(w)->free = 0;
	XtAddCallback (widget, XtNdestroyCallback, Destroy_Callback_Proc,
	    (XtPointer)0);
	Register_Object (w, (GENERIC)0, P_Destroy_Widget, 0);
    }
    return w;
}

Check_Widget (w) Object w; {
    Check_Type (w, T_Widget);
    if (WIDGET(w)->free)
	Primitive_Error ("invalid widget: ~s", w);
}

Check_Widget_Class (w, class) Object w; WidgetClass class; {
    Check_Widget (w);
    if (XtClass (WIDGET(w)->widget) != class)
	Primitive_Error ("widget not of expected class: ~s", w);
}

static Object P_Destroy_Widget (w) Object w; {
    Check_Widget (w);
    XtDestroyWidget (WIDGET(w)->widget);
    return Void;
}

static Object P_Create_Shell (argc, argv) Object *argv; {
    register char *sn = 0, *sc = 0;
    ArgList a;
    Object name = argv[0], class = argv[1], w = argv[2], d = argv[3], ret;
    Declare_C_Strings;

    if (!EQ(name, False))
	Make_C_String (name, sn);
    if (!EQ(class, False))
	Make_C_String (class, sc);
    Check_Type (w, T_Class);
    Check_Type (d, T_Display);
    Encode_Arglist (argc-4, argv+4, a, (Widget)0, CLASS(w)->class);
    ret =  Make_Widget (XtAppCreateShell (sn, sc, CLASS(w)->class,
	DISPLAY(d)->dpy, a, (Cardinal)(argc-4)/2));
    Dispose_C_Strings;
    return ret;
}

static Object P_Create_Widget (argc, argv) Object *argv; {
    ArgList a;
    char *name = 0;
    Object x = argv[0], class, parent, ret;
    Declare_C_Strings;

    if (TYPE(x) != T_Class) {
	Make_C_String (x, name);
	argv++; argc--;
    }
    class = argv[0];
    parent = argv[1];
    Check_Type (class, T_Class);
    Check_Widget (parent);
    if (name == 0)
	name = CLASS(class)->name;
    Encode_Arglist (argc-2, argv+2, a, WIDGET(parent)->widget,
	CLASS(class)->class);
    ret =  Make_Widget (XtCreateWidget ((String)name, CLASS(class)->class,
	WIDGET(parent)->widget, a, (Cardinal)(argc-2)/2));
    Dispose_C_Strings;
    return ret;
}

static Object P_Realize_Widget (w) Object w; {
    Check_Widget (w);
    XtRealizeWidget (WIDGET(w)->widget);
    return Void;
}

static Object P_Unrealize_Widget (w) Object w; {
    Check_Widget (w);
    XtUnrealizeWidget (WIDGET(w)->widget);
    return Void;
}

static Object P_Widget_Realizedp (w) Object w; {
    Check_Widget (w);
    return XtIsRealized (WIDGET(w)->widget) ? True : False;
}

static Object P_Widget_Display (w) Object w; {
    Check_Widget (w);
    return Make_Display (0, XtDisplay (WIDGET(w)->widget));
}

static Object P_Widget_Parent (w) Object w; {
    Check_Widget (w);
    return Make_Widget (XtParent (WIDGET(w)->widget));
}

static Object P_Widget_Name (w) Object w; {
    char *s;

    Check_Widget (w);
    s = XtName (WIDGET(w)->widget);
    return Make_String (s, strlen (s));
}

static Object P_Widget_To_Window (w) Object w; {
    Check_Widget (w);
    return Make_Window (0, XtDisplay (WIDGET(w)->widget),
	XtWindow (WIDGET(w)->widget));
}

static Object P_Widget_Compositep (w) Object w; {
    Check_Widget (w);
    return XtIsComposite (WIDGET(w)->widget) ? True : False;
}

static Object Manage_Unmanage (children, f) Object children; void (*f)(); {
    register i, n;
    Widget *buf;
    Object tail;
    Alloca_Begin;

    Check_List (children);
    n = Internal_Length (children);
    Alloca (buf, Widget*, n * sizeof (Widget));
    for (i = 0, tail = children; i < n; i++, tail = Cdr (tail)) {
	Object w = Car (tail);
	Check_Widget (w);
	buf[i] = WIDGET(w)->widget;
    }
    f (buf, n);
    Alloca_End;
    return Void;
}

static Object P_Manage_Children (children) Object children; {
    return Manage_Unmanage (children, XtManageChildren);
}

static Object P_Unmanage_Children (children) Object children; {
    return Manage_Unmanage (children, XtUnmanageChildren);
}

static Object P_Widget_Managedp (w) Object w; {
    Check_Widget (w);
    return XtIsManaged (WIDGET(w)->widget) ? True : False;
}

static Object P_Widget_Class (w) Object w; {
    Check_Widget (w);
    return Make_Widget_Class (XtClass (WIDGET(w)->widget));
}

static Object P_Widget_Superclass (w) Object w; {
    Check_Widget (w);
    if (XtClass (WIDGET(w)->widget) == widgetClass)
	return Sym_None;
    return Make_Widget_Class (XtSuperclass (WIDGET(w)->widget));
}

static Object P_Widget_Subclassp (w, c) Object w, c; {
    Check_Widget (w);
    Check_Type (c, T_Class);
    return XtIsSubclass (WIDGET(w)->widget, CLASS(c)->class) ? True : False;
}

static Object P_Set_Mapped_When_Managed (w, m) Object w, m; {
    Check_Widget (w);
    Check_Type (m, T_Boolean);
    XtSetMappedWhenManaged (WIDGET(w)->widget, EQ(m, True));
    return Void;
}

static Object P_Map_Widget (w) Object w; {
    Check_Widget (w);
    XtMapWidget (WIDGET(w)->widget);
    return Void;
}

static Object P_Unmap_Widget (w) Object w; {
    Check_Widget (w);
    XtUnmapWidget (WIDGET(w)->widget);
    return Void;
}

static Object P_Set_Values (argc, argv) Object *argv; {
    ArgList a;
    Widget w;
    register i, n = (argc-1)/2;
    Alloca_Begin;

    Check_Widget (argv[0]);
    w = WIDGET(argv[0])->widget;
    Encode_Arglist (argc-1, argv+1, a, w, XtClass (w));
    XtSetValues (w, a, (Cardinal)n);
    for (i = 0; i < n; i++)
	if (streq (a[i].name, XtNdestroyCallback))
	    Fiddle_Destroy_Callback (w);
    Alloca_End;
    return Void;
}

static Object P_Get_Values (argc, argv) Object *argv; {
    Widget w;

    Check_Widget (argv[0]);
    w = WIDGET(argv[0])->widget;
    return Get_Values (w, argc-1, argv+1);
}

static Object P_Widget_Context (w) Object w; {
    Check_Widget (w);
    return Make_Context (XtWidgetToApplicationContext (WIDGET(w)->widget));
}

static Object P_Set_Sensitive (w, s) Object w, s; {
    Check_Widget (w);
    Check_Type (s, T_Boolean);
    XtSetSensitive (WIDGET(w)->widget, EQ(s, True));
    return Void;
}

static Object P_Sensitivep (w) Object w; {
    Check_Widget (w);
    return XtIsSensitive (WIDGET(w)->widget) ? True : False;
}

static Object P_Window_To_Widget (w) Object w; {
    Check_Type (w, T_Window);
    return Make_Widget (XtWindowToWidget (WINDOW(w)->dpy,
	WINDOW(w)->win));
}

static Object P_Name_To_Widget (root, name) Object root, name; {
    register char *s;
    Object ret;
    Declare_C_Strings;

    Check_Widget (root);
    Make_C_String (name, s);
    ret = Make_Widget (XtNameToWidget (WIDGET(root)->widget, s));
    Dispose_C_Strings;
    return ret;
}

static Object P_Widget_Translate_Coordinates (w, x, y) Object w, x, y; {
    Position root_x, root_y;

    Check_Widget (w);
    XtTranslateCoords (WIDGET(w)->widget, Get_Integer (x), Get_Integer (y),
	&root_x, &root_y);
    return Cons (Make_Fixnum (root_x), Make_Fixnum (root_y));
}

init_xt_widget () {
    Generic_Define (Widget, "widget", "widget?");
    Define_Primitive (P_Destroy_Widget,    "destroy-widget",    1, 1, EVAL);
    Define_Primitive (P_Create_Shell,      "create-shell",  4, MANY, VARARGS);
    Define_Primitive (P_Create_Widget,     "create-widget", 2, MANY, VARARGS);
    Define_Primitive (P_Realize_Widget,    "realize-widget",    1, 1, EVAL);
    Define_Primitive (P_Unrealize_Widget,  "unrealize-widget",  1, 1, EVAL);
    Define_Primitive (P_Widget_Realizedp,  "widget-realized?",  1, 1, EVAL);
    Define_Primitive (P_Widget_Display,    "widget-display",    1, 1, EVAL);
    Define_Primitive (P_Widget_Parent,     "widget-parent",     1, 1, EVAL);
    Define_Primitive (P_Widget_Name,       "widget-name",       1, 1, EVAL);
    Define_Primitive (P_Widget_To_Window,  "widget->window",    1, 1, EVAL);
    Define_Primitive (P_Widget_Compositep, "widget-composite?", 1, 1, EVAL);
    Define_Primitive (P_Manage_Children,   "manage-children",   1, 1, EVAL);
    Define_Primitive (P_Unmanage_Children, "unmanage-children", 1, 1, EVAL);
    Define_Primitive (P_Widget_Managedp,   "widget-managed?",   1, 1, EVAL);
    Define_Primitive (P_Widget_Class,      "widget-class",      1, 1, EVAL);
    Define_Primitive (P_Widget_Superclass, "widget-superclass", 1, 1, EVAL);
    Define_Primitive (P_Widget_Subclassp,  "widget-subclass?",  2, 2, EVAL);
    Define_Primitive (P_Set_Mapped_When_Managed,
				  "set-mapped-when-managed!",   2, 2, EVAL);
    Define_Primitive (P_Map_Widget,        "map-widget",        1, 1, EVAL);
    Define_Primitive (P_Unmap_Widget,      "unmap-widget",      1, 1, EVAL);
    Define_Primitive (P_Set_Values,        "set-values!",   1, MANY, VARARGS);
    Define_Primitive (P_Get_Values,        "get-values",    1, MANY, VARARGS);
    Define_Primitive (P_Widget_Context,    "widget-context",    1, 1, EVAL);
    Define_Primitive (P_Set_Sensitive,     "set-sensitive!",    2, 2, EVAL);
    Define_Primitive (P_Sensitivep,        "widget-sensitive?", 1, 1, EVAL);
    Define_Primitive (P_Window_To_Widget,  "window->widget",    1, 1, EVAL);
    Define_Primitive (P_Name_To_Widget,    "name->widget",      2, 2, EVAL);
    Define_Primitive (P_Widget_Translate_Coordinates,
				"widget-translate-coordinates", 3, 3, EVAL);
}
