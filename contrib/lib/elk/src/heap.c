/* Heap management, garbage collection, etc.
 */

#include "scheme.h"

#define Recursive_Visit(p) {\
    register Object *pp = p;\
    if (Types[TYPE(*pp)].haspointer) Visit (pp);\
}

char *Heap_Start,
     *Hp,                     /* First free byte */
     *Heap_End,               /* Points behind free bytes */
     *Free_Start,             /* Start of free area */
     *Free_End;               /* Points behind free area */

int GC_In_Progress;

GCNODE *GC_List;

static Object *Global_GC_Buf[GLOBAL_GC_OBJ];
static int Next_Global;

static void (*After_GC[AFTER_GC_FUNCS])();
static Next_After_GC;

static void (*Before_GC[BEFORE_GC_FUNCS])();
static Next_Before_GC;

static char *To;
static Object V_Garbage_Collect_Notifyp;

Init_Heap () {
    Define_Variable (&V_Garbage_Collect_Notifyp, "garbage-collect-notify?",
	True);
}

Make_Heap (size) {
    register unsigned k = 1024 * size;
    register unsigned s = 2 * k;

    if (s > VALMASK)
	Fatal_Error ("heap size too large (%u KBytes max)", VALMASK/2048);
    if ((Hp = Heap_Start = sbrk (s)) == (char *)-1)
	Fatal_Error ("cannot allocate heap (%u bytes)", s);
    Heap_End = Heap_Start + k;
    Free_Start = Heap_End;
    Free_End = Free_Start + k;
}

char *Get_Bytes (n) {
    register char *p = Hp;

    if (GC_Debug) {
	P_Collect ();
	p = Hp;
    }
    ALIGN(p);
    if (p + n > Heap_End) {
	(void)P_Collect ();
	p = Hp;
	ALIGN(p);
	if (p + n > Heap_End - HEAP_MARGIN)
	    Uncatchable_Error ("Out of heap space");
    }
    Hp = p + n;
    return p;
}

char *Get_Const_Bytes (n) {
    return Get_Bytes (n);
}

Register_Before_GC (f) void (*f)(); {
    if (Next_Before_GC >= BEFORE_GC_FUNCS)
	Fatal_Error ("too many before-GC functions");
    Before_GC[Next_Before_GC++] = f;
}

Call_Before_GC () {
    register i;

    for (i = 0; i < Next_Before_GC; i++)
	(*Before_GC[i])();
}

Register_After_GC (f) void (*f)(); {
    if (Next_After_GC >= AFTER_GC_FUNCS)
	Fatal_Error ("too many after-GC functions");
    After_GC[Next_After_GC++] = f;
}

Call_After_GC () {
    register i;

    for (i = 0; i < Next_After_GC; i++)
	(*After_GC[i])();
}

Object P_Collect () {
    register char *tmp;
    register i, msg = 0;
    Object a[2];

    if (!Interpreter_Initialized)
	Fatal_Error ("heap too small (increase heap size)");
    if (GC_In_Progress)
	Fatal_Error ("GC while GC in progress");
    Disable_Interrupts;
    GC_In_Progress = 1;
    Call_Before_GC ();
    if (GC_Debug) {
	printf ("."); (void)fflush (stdout);
    } else if (Truep (Val (V_Garbage_Collect_Notifyp))) {
	msg++;
	Format (Standard_Output_Port, "[Garbage collecting... ", 23, 0,
	    (Object *)0);
	fflush (stdout);
    }
    To = Free_Start;
    for (i = 0; i < Next_Global; i++)
	Visit (Global_GC_Buf[i]);
    Visit_GC_List (GC_List, 0);
    Visit_Wind (First_Wind, 0);
    Hp = To;
    tmp = Heap_Start; Heap_Start = Free_Start; Free_Start = tmp;
    tmp = Heap_End; Heap_End = Free_End; Free_End = tmp;
    if (!GC_Debug) {
	if (msg) {
	    a[0] = Make_Fixnum ((Hp-Heap_Start) / 1024);
	    a[1] = Make_Fixnum ((Heap_End-Heap_Start) / 1024);
	    Format (Standard_Output_Port, "~sK of ~sK]~%", 13, 2, a);
	}
    }
    Call_After_GC ();
    GC_In_Progress = 0;
    Enable_Interrupts;
    return Void;
}

Visit (p) register Object *p; {
    register Object *tag;
    register t, size, reloc;

again:
    t = TYPE(*p);
    if (!Types[t].haspointer)
	return;
    tag = (Object *)POINTER(*p);
    if ((char *)tag >= Free_Start && (char *)tag < Free_End)
	return;
    if (TYPE(*tag) == T_Broken_Heart) {
	SETPOINTER(*p, POINTER(*tag));
	return;
    }
    ALIGN(To);
    switch (t) {
    case T_Bignum:
	size = sizeof (struct S_Bignum) - sizeof (gran_t)
	       + BIGNUM(*p)->size * sizeof (gran_t);
	bcopy ((char *)tag, To, size);
	break;
    case T_Flonum:
	size = sizeof (struct S_Flonum);
	*(struct S_Flonum *)To = *(struct S_Flonum *)tag;
	break;
    case T_Symbol:
	size = sizeof (struct S_Symbol);
	*(struct S_Symbol *)To = *(struct S_Symbol *)tag;
	break;
    case T_Pair:
    case T_Environment:
	size = sizeof (struct S_Pair);
	*(struct S_Pair *)To = *(struct S_Pair *)tag;
	break;
    case T_String:
	size = sizeof (struct S_String) + STRING(*p)->size - 1;
	bcopy ((char *)tag, To, size);
	break;
    case T_Vector:
	size = sizeof (struct S_Vector) + (VECTOR(*p)->size - 1) *
	    sizeof (Object);
	bcopy ((char *)tag, To, size);
	break;
    case T_Primitive:
	size = sizeof (struct S_Primitive);
	*(struct S_Primitive *)To = *(struct S_Primitive *)tag;
	break;
    case T_Compound:
	size = sizeof (struct S_Compound);
	*(struct S_Compound *)To = *(struct S_Compound *)tag;
	break;
    case T_Control_Point:
#ifdef PORTABLE
	size = sizeof (struct S_Control);
#else
	size = sizeof (struct S_Control) + CONTROL(*p)->size - 1;
	reloc = To - (char *)tag;
#endif
	bcopy ((char *)tag, To, size);
	break;
    case T_Promise:
	size = sizeof (struct S_Promise);
	*(struct S_Promise *)To = *(struct S_Promise *)tag;
	break;
    case T_Port:
	size = sizeof (struct S_Port);
	*(struct S_Port *)To = *(struct S_Port *)tag;
	break;
    case T_Autoload:
	size = sizeof (struct S_Autoload);
	*(struct S_Autoload *)To = *(struct S_Autoload *)tag;
	break;
    case T_Macro:
	size = sizeof (struct S_Macro);
	*(struct S_Macro *)To = *(struct S_Macro *)tag;
	break;
    case T_Broken_Heart:
	Panic ("broken heart in GC");
    default:
	if (t < 0 || t >= MAX_TYPE || !Types[t].name)
	    Panic ("bad type in GC");
	if (Types[t].size == NOFUNC)
	    size = Types[t].const_size;
	else
	    size = (*Types[t].size)(*p);
	bcopy ((char *)tag, To, size);
    }
    SETPOINTER(*p, To);
    SET(*tag, T_Broken_Heart, To);
    To += size;
    if (To > Free_End)
	Panic ("free exhausted in GC");
    if (Types[t].visit == NOFUNC)
	return;
    switch (t) {
    case T_Symbol:
	Recursive_Visit (&SYMBOL(*p)->next);
	Recursive_Visit (&SYMBOL(*p)->name);
	Recursive_Visit (&SYMBOL(*p)->value);
	p = &SYMBOL(*p)->plist;
	goto again;
    case T_Pair:
    case T_Environment:
	Recursive_Visit (&PAIR(*p)->car);
	p = &PAIR(*p)->cdr;
	goto again;
    case T_Vector: {
	    register i, n;
	    for (i = 0, n = VECTOR(*p)->size; i < n; i++)
		Recursive_Visit (&VECTOR(*p)->data[i]);
	    break;
	}
    case T_Compound:
	Recursive_Visit (&COMPOUND(*p)->closure);
	Recursive_Visit (&COMPOUND(*p)->env);
	p = &COMPOUND(*p)->name;
	goto again;
    case T_Control_Point:
#ifdef PORTABLE
	if (CONTROL(*p)->callable) {
	    Visit_GC_List (CONTROL(*p)->gclist, 0);
	    Visit_Wind (CONTROL(*p)->firstwind, 0);
	}
#else
	*(int *)(CONTROL(*p)->stack) += reloc;
	Visit_GC_List (CONTROL(*p)->gclist, *(int *)(CONTROL(*p)->stack));
	Visit_Wind (CONTROL(*p)->firstwind, *(int *)(CONTROL(*p)->stack));
#endif
	p = &CONTROL(*p)->env;
	goto again;
    case T_Promise:
	Recursive_Visit (&PROMISE(*p)->env);
	p = &PROMISE(*p)->thunk;
	goto again;
    case T_Port:
	p = &PORT(*p)->name;
	goto again;
    case T_Autoload:
	Recursive_Visit (&AUTOLOAD(*p)->file);
	p = &AUTOLOAD(*p)->env;
	goto again;
    case T_Macro:
	Recursive_Visit (&MACRO(*p)->body);
	p = &MACRO(*p)->name;
	goto again;
    default:
	(*Types[t].visit)(p, Visit);
    }
}

Visit_GC_List (list, delta) GCNODE *list; {
    register GCNODE *gp, *p;
    register n;
    register Object *vec;

    for (gp = list; gp; gp = p->next) {
	p = (GCNODE *)NORM(gp);
	if (p->gclen <= 0) {
	    Visit ((Object *)NORM(p->gcobj));
	} else {
	    vec = (Object *)NORM(p->gcobj);
	    for (n = 0; n < p->gclen-1; n++)
		Visit (&vec[n]);
	}
    }
}

Visit_Wind (list, delta) WIND *list; {
    register WIND *wp, *p;

    for (wp = list; wp; wp = p->next) {
	p = (WIND *)NORM(wp);
	Visit (&p->inout);
    }
}

_Global_GC_Link (x) Object *x; {
    if (Next_Global == GLOBAL_GC_OBJ)
	Fatal_Error ("too many global GC links");
    Global_GC_Buf[Next_Global++] = x;
}

char *Safe_Malloc (size) {
    char *ret;

    if ((ret = malloc (size)) == 0)
	Primitive_Error ("not enough memory to malloc ~s bytes",
	    Make_Fixnum (size));
    return ret;
}
