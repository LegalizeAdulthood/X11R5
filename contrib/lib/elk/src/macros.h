#ifndef MACROS_H
#define MACROS_H

#ifndef sigmask
#define sigmask(n)  (1 << ((n)-1))
#endif

#define Nullp(x)    ((TYPE(x) == T_Null))
#define Truep(x)    (!EQ(x,False) && !Nullp(x))
#define Car(x)      PAIR(x)->car
#define Cdr(x)      PAIR(x)->cdr
#define Val(x)      Cdr(x)
#define Cons        P_Cons
#define Begin       P_Begin
#define Assq(x,y)   General_Assoc(x,y,0)
#define Print(x)    General_Print_Object (x, Curr_Output_Port, 0)
#define Numeric(t)  (t == T_Fixnum || t == T_Flonum || t == T_Bignum)

#define Whitespace(c) (c == ' ' || c == '\t' || c == '\014' || c == '\n')
#define Delimiter(c) (c == ';' || c == ')' || c == '(' || c == '#' || c == '"')

#ifdef USE_SIGNAL
#  define Disable_Interrupts (void)signal (SIGINT, SIG_IGN)
#  define Enable_Interrupts  (void)signal (SIGINT, Intr_Handler)
#else
#  define Disable_Interrupts (void)sigblock (sigmask (SIGINT))
#  define Enable_Interrupts  (void)sigsetmask (0)
#endif

/* Align heap addresses */
#ifdef ALIGN8
#  define ALIGN(ptr) ((ptr) = (char *)(((int)(ptr) + 7) & ~7))
#else
#  define ALIGN(ptr) ((ptr) = (char *)(((int)(ptr) + 3) & ~3))
#endif

/* Normalize stack addresses */
#define NORM(addr)  ((int)(addr) + delta)

/* Used in special forms: */
#define TC_Prolog   register _t = Tail_Call
#define TC_Disable  Tail_Call = 0
#define TC_Enable   Tail_Call = _t

#define TAG_FUN    -1
#define TAG_ARGS   -2
#define TAG_ENV    -3

#define GC_Node     GCNODE gc1
#define GC_Node2    GCNODE gc1, gc2
#define GC_Node3    GCNODE gc1, gc2, gc3
#define GC_Node4    GCNODE gc1, gc2, gc3, gc4
#define GC_Node5    GCNODE gc1, gc2, gc3, gc4, gc5
#define GC_Node6    GCNODE gc1, gc2, gc3, gc4, gc5, gc6

#define Tag_Node    GC_Node3

#define Tag_Link(args,fun,env) {\
    gc1.gclen = TAG_ARGS; gc1.gcobj = &args; gc1.next = GC_List;\
    gc2.gclen = TAG_FUN;  gc2.gcobj = &fun;  gc2.next = &gc1;\
    gc3.gclen = TAG_ENV;  gc3.gcobj = &env;  gc3.next = &gc2; GC_List = &gc3;\
}

#define GC_Link(x) {\
    gc1.gclen = 0; gc1.gcobj = &x; gc1.next = GC_List; GC_List = &gc1;\
}

#define GC_Link2(x1,x2) {\
    gc1.gclen = 0; gc1.gcobj = &x1; gc1.next = GC_List;\
    gc2.gclen = 0; gc2.gcobj = &x2; gc2.next = &gc1; GC_List = &gc2;\
}

#define GC_Link3(x1,x2,x3) {\
    gc1.gclen = 0; gc1.gcobj = &x1; gc1.next = GC_List;\
    gc2.gclen = 0; gc2.gcobj = &x2; gc2.next = &gc1;\
    gc3.gclen = 0; gc3.gcobj = &x3; gc3.next = &gc2; GC_List = &gc3;\
}

#define GC_Link4(x1,x2,x3,x4) {\
    gc1.gclen = 0; gc1.gcobj = &x1; gc1.next = GC_List;\
    gc2.gclen = 0; gc2.gcobj = &x2; gc2.next = &gc1;\
    gc3.gclen = 0; gc3.gcobj = &x3; gc3.next = &gc2;\
    gc4.gclen = 0; gc4.gcobj = &x4; gc4.next = &gc3; GC_List = &gc4;\
}

#define GC_Link5(x1,x2,x3,x4,x5) {\
    gc1.gclen = 0; gc1.gcobj = &x1; gc1.next = GC_List;\
    gc2.gclen = 0; gc2.gcobj = &x2; gc2.next = &gc1;\
    gc3.gclen = 0; gc3.gcobj = &x3; gc3.next = &gc2;\
    gc4.gclen = 0; gc4.gcobj = &x4; gc4.next = &gc3;\
    gc5.gclen = 0; gc5.gcobj = &x5; gc5.next = &gc4; GC_List = &gc5;\
}

#define GC_Link6(x1,x2,x3,x4,x5,x6) {\
    gc1.gclen = 0; gc1.gcobj = &x1; gc1.next = GC_List;\
    gc2.gclen = 0; gc2.gcobj = &x2; gc2.next = &gc1;\
    gc3.gclen = 0; gc3.gcobj = &x3; gc3.next = &gc2;\
    gc4.gclen = 0; gc4.gcobj = &x4; gc4.next = &gc3;\
    gc5.gclen = 0; gc5.gcobj = &x5; gc5.next = &gc4;\
    gc6.gclen = 0; gc6.gcobj = &x6; gc6.next = &gc5; GC_List = &gc6;\
}

#define GC_Unlink (GC_List = gc1.next)

#define Global_GC_Link(x) _Global_GC_Link(&x)


#define Check_Type(x,t) {\
    if (TYPE(x) != t) Wrong_Type (x, t);\
}

#define Check_List(x) {\
    if (TYPE(x) != T_Pair && !Nullp (x)) Wrong_Type_Combination (x, "list");\
}

#define Check_Number(x) {\
    register t = TYPE(x);\
    if (!Numeric (t)) Wrong_Type_Combination (x, "number");\
}

#define Check_Integer(x) {\
    register t = TYPE(x);\
    if (t != T_Fixnum && t != T_Bignum) Wrong_Type (x, T_Fixnum);\
}

#define Check_Mutable(x) {\
    if (ISCONST(x))\
	Primitive_Error ("attempt to modify constant");\
}

#ifdef PORTABLE
#  define copy_jmpbuf(from,to) bcopy((char *)from, (char *)to, sizeof(jmp_buf))
#endif

#endif
