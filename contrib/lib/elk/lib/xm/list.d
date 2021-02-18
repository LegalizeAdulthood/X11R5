;;; -*-Scheme-*-

(define-widget-type 'list "List.h")

(define-widget-class 'list 'xmListWidgetClass)

(prolog

"static Object String_Table_To_Scheme (tab, len) XmString *tab; {
    Object ret, tail;
    char *text;
    GC_Node2;

    tail = ret = P_Make_List (Make_Fixnum (len), Null);
    GC_Link2 (ret, tail);
    for ( ; len > 0; len--, tail = Cdr (tail)) {
	if (!XmStringGetLtoR (*tab++, XmSTRING_DEFAULT_CHARSET, &text))
	    text = \"\";
	Car (tail) = Make_String (text, strlen (text));
    }
    GC_Unlink;
    return ret;
}")

(prolog

"static SYMDESCR Type_Syms[] = {
   { \"initial\",      XmINITIAL },
   { \"modification\", XmMODIFICATION },
   { \"addition\",     XmADDITION },
   { 0, 0}
};")

(prolog

"static Object Get_List_CB (p) XmListCallbackStruct *p; {
    Object ret, s;
    char *text;
    GC_Node2;

    if (!XmStringGetLtoR (p->item, XmSTRING_DEFAULT_CHARSET, &text))
	text = \"\";
    ret = s = Make_String (text, strlen (text));
    GC_Link2 (ret, s);
    ret = Cons (ret, Null);
    if (p->reason == XmCR_MULTIPLE_SELECT
	    || p->reason == XmCR_EXTENDED_SELECT) {
	s = String_Table_To_Scheme (p->selected_items, p->selected_item_count);
	ret = Cons (s, ret);
	s = Bits_To_Symbols ((unsigned long)p->selection_type, 0, Type_Syms);
	ret = Cons (s, ret);
    } else {
	ret = Cons (Make_Integer (p->item_position), ret);
    }
    s = Get_Any_CB ((XmAnyCallbackStruct *)p);
    ret = Cons (Cdr (s), ret);
    ret = Cons (Car (s), ret);
    GC_Unlink;
    return ret;
}")

(define-callback 'list 'browseSelectionCallback   #t)
(define-callback 'list 'defaultActionCallback     #t)
(define-callback 'list 'extendedSelectionCallback #t)
(define-callback 'list 'multipleSelectionCallback #t)
(define-callback 'list 'singleSelectionCallback   #t)

(define list-callback->scheme
"   return Get_List_CB ((XmListCallbackStruct *)x);")

(c->scheme 'list-browseSelectionCallback   list-callback->scheme)
(c->scheme 'list-defaultActionCallback     list-callback->scheme)
(c->scheme 'list-extendedSelectionCallback list-callback->scheme)
(c->scheme 'list-multipleSelectionCallback list-callback->scheme)
(c->scheme 'list-singleSelectionCallback   list-callback->scheme)
