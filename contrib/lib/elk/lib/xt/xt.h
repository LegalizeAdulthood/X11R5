#include "../xlib/xlib.h"

#define Object FOO
#  include <X11/Intrinsic.h>
#  include <X11/Core.h>
#  include <X11/StringDefs.h>
#undef Object

#if XtSpecificationRelease < 4
 #error "Xt Release 3 or earlier no longer supported"
#endif

#if XtSpecificationRelease >= 5
#  define XT_RELEASE_5_OR_LATER
#endif

typedef XtArgVal (*PFX)();

int T_Context;
int T_Class;
int T_Widget;
int T_Identifier;

#define CONTEXT(x)	((struct S_Context *)POINTER(x))
#define CLASS(x)	((struct S_Class *)POINTER(x))
#define WIDGET(x)	((struct S_Widget *)POINTER(x))
#define IDENTIFIER(x)   ((struct S_Identifier *)POINTER(x))

struct S_Context {
    Object tag;
    XtAppContext context;
    char free;
};

struct S_Class {
    Object tag;
    WidgetClass class;
    char *name;
};

struct S_Widget {
    Object tag;
    Widget widget;
    char free;
};

struct S_Identifier {
    Object tag;
    char type;
    XtPointer val;
    int num;
    char free;
};

extern Match_Xt_Obj();
extern Object Make_Widget_Class(), Make_Context(), Make_Widget(), Make_Id();
extern Object Get_Values(), Get_Resources(), Get_Callbackfun();
extern WidgetClass widgetClass;    /* The `core' class */
extern WidgetClass constraintWidgetClass;
extern WidgetClass compositeWidgetClass;
extern XtPointer Use_Id();
extern Xt_Warning();
extern void XtGetResourceList(), XtGetConstraintResourceList();
extern void Destroy_Callback_Proc();
extern PFO Find_Callback_Converter(), Find_Converter_To_Scheme();
extern PFX Find_Converter_To_C();
extern char *Class_Name();
extern XtTranslations Get_Translations();
extern XtAccelerators Get_Accelerators();
extern void Action_Hook();


#define Encode_Arglist(ac,av,to,widget,class) {\
    Alloca (to, Arg*, ((ac)+1)/2 * sizeof (Arg));\
    Convert_Args (ac, av, to, widget, class);\
}

#define streq(a,b) (strcmp ((a), (b)) == 0)
