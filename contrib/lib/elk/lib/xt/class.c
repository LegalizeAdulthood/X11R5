#include "xt.h"

#include <X11/Xaw/Simple.h>

#define MAX_CLASS	        128
#define MAX_CALLBACK_PER_CLASS   10

typedef struct {
    char *name;
    int has_arg;
} CALLBACK_INFO;

typedef struct {
    WidgetClass class;
    char *name;
    CALLBACK_INFO cb[MAX_CALLBACK_PER_CLASS], *cblast;
    XtResourceList sub_resources;
    int num_resources;
} CLASS_INFO;

static CLASS_INFO ctab[MAX_CLASS], *clast = ctab;

Generic_Predicate (Class);

Generic_Simple_Equal (Class, CLASS, class);

Generic_Print (Class, "#[class %s]", CLASS(x)->name);

Object Make_Class (class, name) WidgetClass class; char *name; {
    register char *p;
    Object c;

    c = Find_Object (T_Class, (GENERIC)0, Match_Xt_Obj, class);
    if (Nullp (c)) {
	p = Get_Bytes (sizeof (struct S_Class));
	SET (c, T_Class, (struct S_Class *)p);
	CLASS(c)->tag = Null;
	CLASS(c)->class = class;
	CLASS(c)->name = name;
	Register_Object (c, (GENERIC)0, (PFO)0, 0);
    }
    return c;
}

Object Make_Widget_Class (class) WidgetClass class; {
    register CLASS_INFO *p;

    for (p = ctab; p < clast; p++)
	if (p->class == class)
	    return Make_Class (class, p->name);
    Primitive_Error ("undefined widget class");
    /*NOTREACHED*/
}

static Object P_Find_Class (name) Object name; {
    register char *s;
    register CLASS_INFO *p;
    Declare_C_Strings;

    Make_C_String (name, s);
    for (p = ctab; p < clast; p++) {
	if (streq (p->name, s)) {
	    Dispose_C_Strings;
	    return Make_Class (p->class, p->name);
	}
    }
    Primitive_Error ("no such widget class: ~s", name);
    /*NOTREACHED*/
}

static Object P_Class_Existsp (name) Object name; {
    register char *s;
    register CLASS_INFO *p;
    Declare_C_Strings;

    Make_C_String (name, s);
    for (p = ctab; p < clast; p++) {
	if (streq (p->name, s)) {
	    Dispose_C_Strings;
	    return True;
	}
    }
    Dispose_C_Strings;
    return False;
}

char *Class_Name (class) WidgetClass class; {
    register CLASS_INFO *p;

    for (p = ctab; p < clast && p->class != class; p++)
	;
    if (p == clast)
	return "unknown";
    return p->name;
}

void Get_Sub_Resource_List (class, rp, np) WidgetClass class;
	XtResourceList *rp; int *np; {
    register CLASS_INFO *p;

    for (p = ctab; p < clast && p->class != class; p++)
	;
    if (p == clast)
	Panic ("Get_Sub_Resource_List");
    *np = p->num_resources;
    *rp = p->sub_resources;
}

static Object P_Class_Resources (c) Object c; {
    Check_Type (c, T_Class);
    return Get_Resources (CLASS(c)->class, XtGetResourceList, 1);
}

static Object P_Class_Constraint_Resources (c) Object c; {
    Check_Type (c, T_Class);
    return Get_Resources (CLASS(c)->class, XtGetConstraintResourceList, 1);
}

static Object P_Class_Sub_Resources (c) Object c; {
    Check_Type (c, T_Class);
    return Get_Resources (CLASS(c)->class, Get_Sub_Resource_List, 0);
}

Define_Class (name, class, r, nr) char *name; WidgetClass class;
	XtResourceList r; {
    Error_Tag = "define-class";
    if (clast == ctab+MAX_CLASS)
	Primitive_Error ("too many widget classes");
    XtInitializeWidgetClass (class);
    clast->name = name;
    clast->class = class;
    clast->cb[0].name = XtNdestroyCallback;
    clast->cb[0].has_arg = 0;
    clast->cblast = clast->cb+1;
    clast->sub_resources = r;
    clast->num_resources = nr;
    clast++;
}

Define_Callback (cl, s, has_arg) char *cl, *s; {
    register CLASS_INFO *p;

    Error_Tag = "define-callback";
    for (p = ctab; p < clast; p++)
	if (streq (p->name, cl)) {
	    if (p->cblast == p->cb+MAX_CALLBACK_PER_CLASS)
		Primitive_Error ("too many callbacks for this class");
	    p->cblast->name = s;
	    p->cblast->has_arg = has_arg;
	    p->cblast++;
	    return;
	}
    Primitive_Error ("undefined class");
}

PFO Find_Callback_Converter (c, name, sname) WidgetClass c; char *name;
	Object sname; {
    register CLASS_INFO *p;
    register CALLBACK_INFO *q;
    PFO conv;

    for (p = ctab; p < clast; p++)
	if (p->class == c) {
	    for (q = p->cb; q < p->cblast; q++)
		if (streq (q->name, name)) {
		    if (q->has_arg) {
			char s[128], msg[256];

			/* First look for a class specific converter
			 * then for a general one:
			 */
			sprintf (s, "%s-%s", p->name, name);
			conv = Find_Converter_To_Scheme (s);
			if (conv == 0) {
			    conv = Find_Converter_To_Scheme (name);
			    if (conv == 0) {
				sprintf (msg,
				    "no callback converter for %s or %s",
					s, name);
				Primitive_Error (msg);
			    }
			}
			return conv;
		    } else return (PFO)0;
		}
	    Primitive_Error ("no such callback: ~s", sname);
	}
    Panic ("Find_Callback_Converter");
    /*NOTREACHED*/
}

init_xt_class () {
    Generic_Define (Class, "class", "class?");
    Define_Primitive (P_Find_Class,        "find-class",        1, 1, EVAL);
    Define_Primitive (P_Class_Resources,   "class-resources",   1, 1, EVAL);
    Define_Primitive (P_Class_Constraint_Resources, 
                               "class-constraint-resources",    1, 1, EVAL);
    Define_Primitive (P_Class_Sub_Resources,
			       "class-sub-resources",           1, 1, EVAL);
    Define_Primitive (P_Class_Existsp,     "class-exists?",     1, 1, EVAL);
    /*
     * Doesn't work with Motif-1.1:
     *
    Define_Class ("simple", simpleWidgetClass, (XtResourceList)0, 0);
     */
    Define_Class ("core", widgetClass, (XtResourceList)0, 0);
    Define_Class ("constraint", constraintWidgetClass, (XtResourceList)0, 0);
    Define_Class ("composite", compositeWidgetClass, (XtResourceList)0, 0);
}
