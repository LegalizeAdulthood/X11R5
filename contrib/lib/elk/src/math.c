/* Generic math functions
 */

#include <math.h>
#include <errno.h>

#include "scheme.h"

Object Generic_Multiply(), Generic_Divide();

Init_Math () {
    (void)srandom (getpid ());
}

Object Make_Fixnum (n) {
    Object num;

    SET(num, T_Fixnum, n);
    return num;
}

Object Make_Integer (n) register n; {
    if (FIXNUM_FITS(n))
	return Make_Fixnum (n);
    else
	return Integer_To_Bignum (n);
}

Object Make_Unsigned (n) register unsigned n; {
    if (FIXNUM_FITS_UNSIGNED(n))
	return Make_Fixnum (n);
    else
	return Unsigned_To_Bignum (n);
}

Object Fixnum_To_String (x, radix) Object x; {
    char buf[32];
    register char *p;
    register n = FIXNUM(x), neg = 0;

    if (n == 0)
	return Make_String ("0", 1);
    if (n < 0) {
	neg++;
	n = -n;
    }
    p = buf+31;
    *p = '\0';
    while (n > 0) {
	*--p = '0' + n % radix;
	if (*p > '9')
	    *p = 'A' + (*p - '9') - 1;
	n /= radix;
    }
    if (neg)
	*--p = '-';
    return Make_String (p, strlen (p));
}

Object Flonum_To_String (x) Object x; {
    char buf[32];

    sprintf (buf, FLONUM_FORMAT, FLONUM(x)->val);
    return Make_String (buf, strlen (buf));
}

Object P_Number_To_String (argc, argv) Object *argv; {
    int radix = 10;
    Object x = argv[0];

    if (argc == 2) {
	radix = Get_Integer (argv[1]);
	switch (radix) {
	case 2: case 8: case 10: case 16:
	    break;
	default:
	    Primitive_Error ("invalid radix: ~s", argv[1]);
	}
    }
    switch (TYPE(x)) {
    case T_Fixnum:
	return Fixnum_To_String (x, radix); break;
    case T_Bignum:
	return Bignum_To_String (x, radix); break;
    case T_Flonum:
	if (radix != 10)
	    Primitive_Error ("radix for reals must be 10");   /* bleah! */
	return Flonum_To_String (x); break;
    }
    /*NOTREACHED*/
}

Get_Integer (x) Object x; {
    switch (TYPE(x)) {
    case T_Fixnum:
	return FIXNUM(x);
    case T_Bignum:
	return Bignum_To_Integer (x);
    default:
	Wrong_Type (x, T_Fixnum);
    }
    /*NOTREACHED*/
}

Get_Index (n, obj) Object n, obj; {
    register size, i;

    i = Get_Integer (n);
    size = TYPE(obj) == T_Vector ? VECTOR(obj)->size : STRING(obj)->size;
    if (i < 0 || i >= size)
	Range_Error (n);
    return i;
}

Object Make_Reduced_Flonum (d) double d; {
    Object num;
    register char *p;
    int expo;

    if (floor (d) == d) {
	if (d == 0)
	    return Zero;
	(void)frexp (d, &expo);
	if (expo <= VALBITS-1)
	    return Make_Fixnum ((int)d);
    }
    p = Get_Bytes (sizeof (struct S_Flonum));
    SET(num, T_Flonum, (struct S_Flonum *)p);
    FLONUM(num)->tag = Null;
    FLONUM(num)->val = d;
    return num;
}

Object P_Integerp (x) Object x; {
    return TYPE(x) == T_Fixnum || TYPE(x) == T_Bignum ? True : False;
}

Object P_Rationalp (x) Object x; {
    return P_Integerp (x);
}

Object P_Realp (x) Object x; {
    register t = TYPE(x);
    return t == T_Flonum || t == T_Fixnum  || t == T_Bignum ? True : False;
}

Object P_Complexp (x) Object x; {
    return P_Realp (x);
}

Object P_Numberp (x) Object x; {
    return P_Complexp (x);
}

#define General_Generic_Predicate(prim,op,bigop) Object prim (x) Object x; {\
    register ret;\
    Check_Number (x);\
    switch (TYPE(x)) {\
    case T_Flonum:\
	ret = FLONUM(x)->val op 0; break;\
    case T_Fixnum:\
	ret = FIXNUM(x) op 0; break;\
    case T_Bignum:\
	ret = bigop (x); break;\
    }\
    return ret ? True : False;\
}

General_Generic_Predicate (P_Zerop, ==, Bignum_Zero)
General_Generic_Predicate (P_Negativep, <, Bignum_Negative)
General_Generic_Predicate (P_Positivep, >, Bignum_Positive)

Object P_Evenp (x) Object x; {
    register ret;

    Check_Integer (x);
    switch (TYPE(x)) {
    case T_Fixnum:
	ret = !(FIXNUM(x) & 1); break;
    case T_Bignum:
	ret = Bignum_Even (x); break;
    }
    return ret ? True : False;
}

Object P_Oddp (x) Object x; {
    Object tmp;
    tmp = P_Evenp (x);
    return EQ(tmp,True) ? False : True;
}

Object P_Exactp (x) Object x; {
    Check_Number (x);
    return False;
}

Object P_Inexactp (x) Object x; {
    Check_Number (x);
    return True;
}

#define General_Generic_Compare(name,op,bigop) name (x, y) Object x, y; {\
    Object b; register ret;\
    GC_Node;\
    \
    switch (TYPE(x)) {\
    case T_Fixnum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    return FIXNUM(x) op FIXNUM(y);\
	case T_Flonum:\
	    return FIXNUM(x) op FLONUM(y)->val;\
	case T_Bignum:\
	    GC_Link (y);\
	    b = Integer_To_Bignum (FIXNUM(x));\
	    ret = bigop (b, y);\
	    GC_Unlink;\
	    return ret;\
	}\
    case T_Flonum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    return FLONUM(x)->val op FIXNUM(y);\
	case T_Flonum:\
	    return FLONUM(x)->val op FLONUM(y)->val;\
	case T_Bignum:\
	    return FLONUM(x)->val op Bignum_To_Double (y);\
	}\
    case T_Bignum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    GC_Link (x);\
	    b = Integer_To_Bignum (FIXNUM(y));\
	    ret = bigop (x, b);\
	    GC_Unlink;\
	    return ret;\
	case T_Flonum:\
	    return Bignum_To_Double (x) op FLONUM(y)->val;\
	case T_Bignum:\
	    return bigop (x, y);\
	}\
    }\
    /*NOTREACHED*/\
}

General_Generic_Compare (Generic_Equal,      ==, Bignum_Equal)
General_Generic_Compare (Generic_Less,        <, Bignum_Less)
General_Generic_Compare (Generic_Greater,     >, Bignum_Greater)
General_Generic_Compare (Generic_Eq_Less,    <=, Bignum_Eq_Less)
General_Generic_Compare (Generic_Eq_Greater, >=, Bignum_Eq_Greater)

Object General_Compare (argc, argv, op) Object *argv; register (*op)(); {
    register i;

    Check_Number (argv[0]);
    for (i = 1; i < argc; i++) {
	Check_Number (argv[i]);
	if (!(*op) (argv[i-1], argv[i]))
	    return False;
    }
    return True;
}

Object P_Generic_Equal (argc, argv) Object *argv; {
    return General_Compare (argc, argv, Generic_Equal);
}

Object P_Generic_Less (argc, argv) Object *argv; {
    return General_Compare (argc, argv, Generic_Less);
}

Object P_Generic_Greater (argc, argv) Object *argv; {
    return General_Compare (argc, argv, Generic_Greater);
}

Object P_Generic_Eq_Less (argc, argv) Object *argv; {
    return General_Compare (argc, argv, Generic_Eq_Less);
}

Object P_Generic_Eq_Greater (argc, argv) Object *argv; {
    return General_Compare (argc, argv, Generic_Eq_Greater);
}

#define General_Generic_Operator(name,op,bigop) Object name (x, y)\
	Object x, y; {\
    Object b1, b2, ret; register i;\
    GC_Node2;\
    \
    switch (TYPE(x)) {\
    case T_Fixnum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    i = FIXNUM(x) op FIXNUM(y);\
	    if (FIXNUM_FITS(i))\
		return Make_Fixnum (i);\
	    b1 = b2 = Null;\
	    GC_Link2 (b1, b2);\
	    b1 = Integer_To_Bignum (FIXNUM(x));\
	    b2 = Integer_To_Bignum (FIXNUM(y));\
	    ret = bigop (b1, b2);\
	    GC_Unlink;\
	    return ret;\
	case T_Flonum:\
	    return Make_Reduced_Flonum (FIXNUM(x) op FLONUM(y)->val);\
	case T_Bignum:\
	    GC_Link (y);\
	    b1 = Integer_To_Bignum (FIXNUM(x));\
	    ret = bigop (b1, y);\
	    GC_Unlink;\
	    return ret;\
	}\
    case T_Flonum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    return Make_Reduced_Flonum (FLONUM(x)->val op FIXNUM(y));\
	case T_Flonum:\
	    return Make_Reduced_Flonum (FLONUM(x)->val op FLONUM(y)->val);\
	case T_Bignum:\
	    return Make_Reduced_Flonum (FLONUM(x)->val op\
		Bignum_To_Double (y));\
	}\
    case T_Bignum:\
	switch (TYPE(y)) {\
	case T_Fixnum:\
	    GC_Link (x);\
	    b1 = Integer_To_Bignum (FIXNUM(y));\
	    ret = bigop (x, b1);\
	    GC_Unlink;\
	    return ret;\
	case T_Flonum:\
	    return Make_Reduced_Flonum (Bignum_To_Double (x) op\
		FLONUM(y)->val);\
	case T_Bignum:\
	    return bigop (x, y);\
	}\
    }\
    /*NOTREACHED*/\
}

General_Generic_Operator (Generic_Plus,      +, Bignum_Plus)
General_Generic_Operator (Generic_Minus,     -, Bignum_Minus)

Object P_Inc (x) Object x; {
    Check_Number (x);
    return Generic_Plus (x, One);
}

Object P_Dec (x) Object x; {
    Check_Number (x);
    return Generic_Minus (x, One);
}

Object General_Operator (argc, argv, start, op) Object *argv, start;
	register Object (*op)(); {
    register i;
    Object accum;

    if (argc > 0)
	Check_Number (argv[0]);
    accum = start;
    switch (argc) {
    case 0:
	break;
    case 1:
	accum = (*op) (accum, argv[0]); break;
    default:
	for (accum = argv[0], i = 1; i < argc; i++) {
	    Check_Number (argv[i]);
	    accum = (*op) (accum, argv[i]);
	}
    }
    return accum;
}

Object P_Generic_Plus (argc, argv) Object *argv; {
    return General_Operator (argc, argv, Zero, Generic_Plus);
}

Object P_Generic_Minus (argc, argv) Object *argv; {
    return General_Operator (argc, argv, Zero, Generic_Minus);
}

Object P_Generic_Multiply (argc, argv) Object *argv; {
    return General_Operator (argc, argv, One, Generic_Multiply);
}

Object P_Generic_Divide (argc, argv) Object *argv; {
    return General_Operator (argc, argv, One, Generic_Divide);
}

Object Generic_Multiply (x, y) Object x, y; {
    Object b, ret;

    switch (TYPE(x)) {
    case T_Fixnum:
	switch (TYPE(y)) {
	case T_Fixnum:
	    ret = Fixnum_Multiply (FIXNUM(x), FIXNUM(y));
	    if (Nullp (ret)) {
		b = Integer_To_Bignum (FIXNUM(x));
		return Bignum_Fixnum_Multiply (b, y);
	    }
	    return ret;
	case T_Flonum:
	    return Make_Reduced_Flonum (FIXNUM(x) * FLONUM(y)->val);
	case T_Bignum:
	    return Bignum_Fixnum_Multiply (y, x);
	}
    case T_Flonum:
	switch (TYPE(y)) {
	case T_Fixnum:
	    return Make_Reduced_Flonum (FLONUM(x)->val * FIXNUM(y));
	case T_Flonum:
	    return Make_Reduced_Flonum (FLONUM(x)->val * FLONUM(y)->val);
	case T_Bignum:
	    return Make_Reduced_Flonum (FLONUM(x)->val * Bignum_To_Double (y));
	}
    case T_Bignum:
	switch (TYPE(y)) {
	case T_Fixnum:
	    return Bignum_Fixnum_Multiply (x, y);
	case T_Flonum:
	    return Make_Reduced_Flonum (Bignum_To_Double (x) * FLONUM(y)->val);
	case T_Bignum:
	    return Bignum_Multiply (x, y);
	}
    }
    /*NOTREACHED*/
}

Object Generic_Divide (x, y) Object x, y; {
    register t = TYPE(y);
    Object b, ret;
    GC_Node2;

    if (t == T_Fixnum ? FIXNUM(y) == 0 :
	(t == T_Flonum ? FLONUM(y) == 0 : Bignum_Zero (y)))
	Range_Error (y);
    switch (TYPE(x)) {
    case T_Fixnum:
	switch (t) {
	case T_Fixnum:
	    return Make_Reduced_Flonum ((double)FIXNUM(x) / (double)FIXNUM(y));
	case T_Flonum:
	    return Make_Reduced_Flonum ((double)FIXNUM(x) / FLONUM(y)->val);
	case T_Bignum:
	    GC_Link (y);
	    b = Integer_To_Bignum (FIXNUM(x));
	    ret = Bignum_Divide (b, y);
	    GC_Unlink;
	    if (EQ(Cdr (ret),Zero))
		return Car (ret);
	    return Make_Reduced_Flonum ((double)FIXNUM(x) /
		    Bignum_To_Double (y));
	}
    case T_Flonum:
	switch (t) {
	case T_Fixnum:
	    return Make_Reduced_Flonum (FLONUM(x)->val / (double)FIXNUM(y));
	case T_Flonum:
	    return Make_Reduced_Flonum (FLONUM(x)->val / FLONUM(y)->val);
	case T_Bignum:
	    return Make_Reduced_Flonum (FLONUM(x)->val / Bignum_To_Double (y));
	}
    case T_Bignum:
	switch (t) {
	case T_Fixnum:
	    GC_Link (x);
	    ret = Bignum_Fixnum_Divide (x, y);
	    GC_Unlink;
	    if (EQ(Cdr (ret),Zero))
		return Car (ret);
	    return Make_Reduced_Flonum (Bignum_To_Double (x) /
		    (double)FIXNUM(y));
	case T_Flonum:
	    return Make_Reduced_Flonum (Bignum_To_Double (x) / FLONUM(y)->val);
	case T_Bignum:
	    GC_Link2 (x, y);
	    ret = Bignum_Divide (x, y);
	    GC_Unlink;
	    if (EQ(Cdr (ret),Zero))
		return Car (ret);
	    return Make_Reduced_Flonum (Bignum_To_Double (x) /
		    Bignum_To_Double (y));
	}
    }
    /*NOTREACHED*/
}

Object P_Abs (x) Object x; {
    register i;

    Check_Number (x);
    switch (TYPE(x)) {
    case T_Fixnum:
	i = FIXNUM(x);
	return i < 0 ? Make_Integer (-i) : x;
    case T_Flonum:
	return Make_Reduced_Flonum (fabs (FLONUM(x)->val));
    case T_Bignum:
	return Bignum_Abs (x);
    }
    /*NOTREACHED*/
}

Object General_Integer_Divide (x, y, rem) Object x, y; {
    register fx = FIXNUM(x), fy = FIXNUM(y);
    Object b, ret;
    GC_Node;

    Check_Integer (x);
    Check_Integer (y);
    if (TYPE(y) == T_Fixnum ? FIXNUM(y) == 0 : Bignum_Zero (y))
	Range_Error (y);
    switch (TYPE(x)) {
    case T_Fixnum:
	switch (TYPE(y)) {
	case T_Fixnum:
	    return Make_Fixnum (rem ? (fx % fy) : (fx / fy));
	case T_Bignum:
	    GC_Link (y);
	    b = Integer_To_Bignum (fx);
	    GC_Unlink;
	    ret = Bignum_Divide (b, y);
done:
	    return rem ? Cdr (ret) : Car (ret);
	}
    case T_Bignum:
	switch (TYPE(y)) {
	case T_Fixnum:
	    ret = Bignum_Fixnum_Divide (x, y);
	    goto done;
	case T_Bignum:
	    ret = Bignum_Divide (x, y);
	    goto done;
	}
    }
    /*NOTREACHED*/
}

Object P_Quotient (x, y) Object x, y; {
    return General_Integer_Divide (x, y, 0);
}

Object P_Remainder (x, y) Object x, y; {
    return General_Integer_Divide (x, y, 1);
}

Object P_Modulo (x, y) Object x, y; {
    Object rem, xneg, yneg;
    GC_Node2;

    GC_Link2 (x, y);
    rem = General_Integer_Divide (x, y, 1);
    xneg = P_Negativep (x);
    yneg = P_Negativep (y);
    if (!EQ(xneg,yneg))
	rem = Generic_Plus (rem, y);
    GC_Unlink;
    return rem;
}

Object gcd (x, y) Object x, y; {
    Object r, z;
    GC_Node2;

    Check_Integer (x);
    Check_Integer (y);
    GC_Link2 (x, y);
    while (1) {
	z = P_Zerop (x);
	if (EQ(z,True)) {
	    r = y;
	    break;
	}
	z = P_Zerop (y);
	if (EQ(z,True)) {
	    r = x;
	    break;
	}
	r = General_Integer_Divide (x, y, 1);
	x = y;
	y = r;
    }
    GC_Unlink;
    return r;
}

Object P_Gcd (argc, argv) Object *argv; {
    return P_Abs (General_Operator (argc, argv, Zero, gcd));
}

Object lcm (x, y) Object x, y; {
    Object ret, p, z;
    GC_Node3;

    ret = Null;
    GC_Link3 (x, y, ret);
    ret = gcd (x, y);
    z = P_Zerop (ret);
    if (!EQ(z,True)) {
	p = Generic_Multiply (x, y);
	ret = General_Integer_Divide (p, ret, 0);
    }
    GC_Unlink;
    return ret;
}

Object P_Lcm (argc, argv) Object *argv; {
    return P_Abs (General_Operator (argc, argv, One, lcm));
}

#define General_Conversion(name,op) Object name (x) Object x; {\
    double d; int expo;\
    \
    Check_Number (x);\
    if (TYPE(x) != T_Flonum)\
	return x;\
    d = op (FLONUM(x)->val);\
    (void)frexp (d, &expo);\
    return (expo <= VALBITS-1) ? Make_Fixnum ((int)d) : Double_To_Bignum (d);\
}

#define trunc(x) (x)
#define round(x) ((x) >= 0 ? (x) + 0.5 : (x) - 0.5)

General_Conversion (P_Floor, floor)
General_Conversion (P_Ceiling, ceil)
General_Conversion (P_Truncate, trunc)
General_Conversion (P_Round, round)

double Get_Double (x) Object x; {
    Check_Number (x);
    switch (TYPE(x)) {
    case T_Fixnum:
	return (double)FIXNUM(x);
    case T_Flonum:
	return FLONUM(x)->val;
    case T_Bignum:
	return Bignum_To_Double (x);
    }
    /*NOTREACHED*/
}

Object General_Function (x, y, fun) Object x, y; double (*fun)(); {
    double d, ret;

    d = Get_Double (x);
    errno = 0;
    if (Nullp (y))
	ret = (*fun) (d);
    else
	ret = (*fun) (d, Get_Double (y));
    if (errno == ERANGE || errno == EDOM)
	Range_Error (x);
    return Make_Reduced_Flonum (ret);
}

Object P_Sqrt (x) Object x; { return General_Function (x, Null, sqrt); }

Object P_Exp (x) Object x; { return General_Function (x, Null, exp); }

Object P_Log (x) Object x; { return General_Function (x, Null, log); }

Object P_Sin (x) Object x; { return General_Function (x, Null, sin); }

Object P_Cos (x) Object x; { return General_Function (x, Null, cos); }

Object P_Tan (x) Object x; { return General_Function (x, Null, tan); }

Object P_Asin (x) Object x; { return General_Function (x, Null, asin); }

Object P_Acos (x) Object x; { return General_Function (x, Null, acos); }

Object P_Atan (argc, argv) Object *argv; {
    register a2 = argc == 2;
    return General_Function (argv[0], a2 ? argv[1] : Null, a2 ? 
	(double(*)())atan2 : (double(*)())atan);
}

Object Min (x, y) Object x, y; {
    return Generic_Less (x, y) ? x : y;
}

Object Max (x, y) Object x, y; {
    return Generic_Less (x, y) ? y : x;
}

Object P_Min (argc, argv) Object *argv; {
    return General_Operator (argc, argv, argv[0], Min);
}

Object P_Max (argc, argv) Object *argv; {
    return General_Operator (argc, argv, argv[0], Max);
}

Object P_Random () {
    extern long random();
    return Make_Integer ((int)random ());
}

Object P_Srandom (x) Object x; {
    Check_Integer (x);
    (void)srandom (Get_Integer (x));
    return x;
}
