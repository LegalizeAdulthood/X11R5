;;; -*-Scheme-*-

(define-widget-type 'grip "Grip.h")

(define-widget-class 'grip 'gripWidgetClass)

(define-callback 'grip 'callback #t)

(c->scheme 'grip-callback
"   Object args, ret, t = Null;
    register i;
    GripCallData p = (GripCallData)x;
    GC_Node2;

    args = Get_Event_Args (p->event);
    ret = Cons (Copy_List (args), Null);
    Destroy_Event_Args (args);
    GC_Link2 (ret, t);
    t = P_Make_List (Make_Fixnum (p->num_params), Null);
    for (i = 0, Cdr (ret) = t; i < p->num_params; i++, t = Cdr (t)) {
	Object s = Make_String (p->params[i], strlen (p->params[i]));
	Car (t) = s;
    }
    GC_Unlink;
    return ret;")
