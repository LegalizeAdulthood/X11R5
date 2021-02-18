/* -*-C-*-
********************************************************************************
*
* File:         xlmath.c
* RCS:          $Header: xlmath.c,v 1.5 91/03/24 22:25:12 mayer Exp $
* Description:  xlisp built-in arithmetic functions
* Author:       David Michael Betz
* Created:      
* Modified:     Fri Oct  4 04:06:29 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: xlmath.c,v 1.5 91/03/24 22:25:12 mayer Exp $";


#include "xlisp.h"
#include <math.h>

/* external variables */
extern LVAL true;

/* forward declarations */
LOCAL FORWARD LVAL unary();	/* NPM: changed this to LOCAL */
LOCAL FORWARD LVAL binary();	/* NPM: changed this to LOCAL */
LOCAL FORWARD LVAL predicate();	/* NPM: changed this to LOCAL */
LOCAL FORWARD LVAL compare();	/* NPM: changed this to LOCAL */

/* binary functions */
LVAL xadd()    { return (binary('+')); } /* + */
LVAL xsub()    { return (binary('-')); } /* - */
LVAL xmul()    { return (binary('*')); } /* * */
LVAL xdiv()    { return (binary('/')); } /* / */
LVAL xrem()    { return (binary('%')); } /* rem */
LVAL xmin()    { return (binary('m')); } /* min */
LVAL xmax()    { return (binary('M')); } /* max */
LVAL xexpt()   { return (binary('E')); } /* expt */
LVAL xlogand() { return (binary('&')); } /* logand */
LVAL xlogior() { return (binary('|')); } /* logior */
LVAL xlogxor() { return (binary('^')); } /* logxor */

/* xgcd - greatest common divisor */
LVAL xgcd()
{
    FIXTYPE m,n,r;
    LVAL arg;

    if (!moreargs())			/* check for identity case */
	return (cvfixnum((FIXTYPE)0));
    arg = xlgafixnum();
    n = getfixnum(arg);
    if (n < (FIXTYPE)0) n = -n;		/* absolute value */
    while (moreargs()) {
	arg = xlgafixnum();
	m = getfixnum(arg);
	if (m < (FIXTYPE)0) m = -m;	/* absolute value */
	for (;;) {			/* euclid's algorithm */
	    r = m % n;
	    if (r == (FIXTYPE)0)
		break;
	    m = n;
	    n = r;
	}
    }
    return (cvfixnum(n));
}

/* binary - handle binary operations */
LOCAL LVAL binary(fcn)
  int fcn;
{
    FIXTYPE ival,iarg;
    FLOTYPE fval,farg;
    LVAL arg;
    int mode;

    /* get the first argument */
    arg = xlgetarg();

    /* set the type of the first argument */
    if (fixp(arg)) {
	ival = getfixnum(arg);
	mode = 'I';
    }
    else if (floatp(arg)) {
	fval = getflonum(arg);
	mode = 'F';
    }
    else
	xlerror("bad argument type",arg);

    /* treat a single argument as a special case */
    if (!moreargs()) {
	switch (fcn) {
	case '-':
	    switch (mode) {
	    case 'I':
		ival = -ival;
		break;
	    case 'F':
		fval = -fval;
		break;
	    }
	    break;
	case '/':
	    switch (mode) {
	    case 'I':
		checkizero(ival);
		ival = 1 / ival;
		break;
	    case 'F':
		checkfzero(fval);
		fval = 1.0 / fval;
		break;
	    }
	}
    }

    /* handle each remaining argument */
    while (moreargs()) {

	/* get the next argument */
	arg = xlgetarg();

	/* check its type */
	if (fixp(arg)) {
	    switch (mode) {
	    case 'I':
	        iarg = getfixnum(arg);
	        break;
	    case 'F':
	        farg = (FLOTYPE)getfixnum(arg);
		break;
	    }
	}
	else if (floatp(arg)) {
	    switch (mode) {
	    case 'I':
	        fval = (FLOTYPE)ival;
		farg = getflonum(arg);
		mode = 'F';
		break;
	    case 'F':
	        farg = getflonum(arg);
		break;
	    }
	}
	else
	    xlerror("bad argument type",arg);

	/* accumulate the result value */
	switch (mode) {
	case 'I':
	    switch (fcn) {
	    case '+':	ival += iarg; break;
	    case '-':	ival -= iarg; break;
	    case '*':	ival *= iarg; break;
	    case '/':	checkizero(iarg); ival /= iarg; break;
	    case '%':	checkizero(iarg); ival %= iarg; break;
	    case 'M':	if (iarg > ival) ival = iarg; break;
	    case 'm':	if (iarg < ival) ival = iarg; break;
	    case '&':	ival &= iarg; break;
	    case '|':	ival |= iarg; break;
	    case '^':	ival ^= iarg; break;
	    default:	badiop();
	    }
	    break;
	case 'F':
	    switch (fcn) {
	    case '+':	fval += farg; break;
	    case '-':	fval -= farg; break;
	    case '*':	fval *= farg; break;
	    case '/':	checkfzero(farg); fval /= farg; break;
	    case 'M':	if (farg > fval) fval = farg; break;
	    case 'm':	if (farg < fval) fval = farg; break;
	    case 'E':	fval = pow(fval,farg); break;
	    default:	badfop();
	    }
    	    break;
	}
    }

    /* return the result */
    switch (mode) {
    case 'I':	return (cvfixnum(ival));
    case 'F':	return (cvflonum(fval));
    }
}

/* checkizero - check for integer division by zero */
LOCAL checkizero(iarg)		/* NPM: changed this to LOCAL at request of jsp@glia.biostr.washington.edu (Jeff Prothero) */
  FIXTYPE iarg;
{
    if (iarg == 0)
	xlfail("division by zero");
}

/* checkfzero - check for floating point division by zero */
LOCAL checkfzero(farg)		/* NPM: changed this to LOCAL at request of jsp@glia.biostr.washington.edu (Jeff Prothero) */
  FLOTYPE farg;
{
    if (farg == 0.0)
	xlfail("division by zero");
}

/* checkfneg - check for square root of a negative number */
LOCAL checkfneg(farg)		/* NPM: changed this to LOCAL at request of jsp@glia.biostr.washington.edu (Jeff Prothero) */
  FLOTYPE farg;
{
    if (farg < 0.0)
	xlfail("square root of a negative number");
}

/* unary functions */
LVAL xlognot() { return (unary('~')); } /* lognot */
LVAL xabs()    { return (unary('A')); } /* abs */
LVAL xadd1()   { return (unary('+')); } /* 1+ */
LVAL xsub1()   { return (unary('-')); } /* 1- */
LVAL xsin()    { return (unary('S')); } /* sin */
LVAL xcos()    { return (unary('C')); } /* cos */
LVAL xtan()    { return (unary('T')); } /* tan */
LVAL xasin()   { return (unary('s')); } /* asin */
LVAL xacos()   { return (unary('c')); } /* acos */
LVAL xatan()   { return (unary('t')); } /* atan */
LVAL xexp()    { return (unary('E')); } /* exp */
LVAL xsqrt()   { return (unary('R')); } /* sqrt */
LVAL xfix()    { return (unary('I')); } /* truncate */
LVAL xfloat()  { return (unary('F')); } /* float */
LVAL xrand()   { return (unary('?')); } /* random */

/* unary - handle unary operations */
LOCAL LVAL unary(fcn)
  int fcn;
{
    FLOTYPE fval;
    FIXTYPE ival;
    LVAL arg;

    /* get the argument */
    arg = xlgetarg();
    xllastarg();

    /* check its type */
    if (fixp(arg)) {
	ival = getfixnum(arg);
	switch (fcn) {
	case '~':	ival = ~ival; break;
	case 'A':	ival = (ival < 0 ? -ival : ival); break;
	case '+':	ival++; break;
	case '-':	ival--; break;
	case 'I':	break;
	case 'F':	return (cvflonum((FLOTYPE)ival));
	case '?':	ival = (FIXTYPE)osrand((int)ival); break;
	default:	badiop();
	}
	return (cvfixnum(ival));
    }
    else if (floatp(arg)) {
	fval = getflonum(arg);
	switch (fcn) {
	case 'A':	fval = (fval < 0.0 ? -fval : fval); break;
	case '+':	fval += 1.0; break;
	case '-':	fval -= 1.0; break;
	case 'S':	fval = sin(fval); break;
	case 'C':	fval = cos(fval); break;
	case 'T':	fval = tan(fval); break;
	case 's':	fval = asin(fval); break;
	case 'c':	fval = acos(fval); break;
	case 't':	fval = atan(fval); break;
	case 'E':	fval = exp(fval); break;
	case 'R':	checkfneg(fval); fval = sqrt(fval); break;
	case 'I':	return (cvfixnum((FIXTYPE)fval));
	case 'F':	break;
	default:	badfop();
	}
	return (cvflonum(fval));
    }
    else
	xlerror("bad argument type",arg);
}

/* unary predicates */
LVAL xminusp() { return (predicate('-')); } /* minusp */
LVAL xzerop()  { return (predicate('Z')); } /* zerop */
LVAL xplusp()  { return (predicate('+')); } /* plusp */
LVAL xevenp()  { return (predicate('E')); } /* evenp */
LVAL xoddp()   { return (predicate('O')); } /* oddp */

/* predicate - handle a predicate function */
LOCAL LVAL predicate(fcn)
  int fcn;
{
    FLOTYPE fval;
    FIXTYPE ival;
    LVAL arg;

    /* get the argument */
    arg = xlgetarg();
    xllastarg();

    /* check the argument type */
    if (fixp(arg)) {
	ival = getfixnum(arg);
	switch (fcn) {
	case '-':	ival = (ival < 0); break;
	case 'Z':	ival = (ival == 0); break;
	case '+':	ival = (ival > 0); break;
	case 'E':	ival = ((ival & 1) == 0); break;
	case 'O':	ival = ((ival & 1) != 0); break;
	default:	badiop();
	}
    }
    else if (floatp(arg)) {
	fval = getflonum(arg);
	switch (fcn) {
	case '-':	ival = (fval < 0); break;
	case 'Z':	ival = (fval == 0); break;
	case '+':	ival = (fval > 0); break;
	default:	badfop();
	}
    }
    else
	xlerror("bad argument type",arg);

    /* return the result value */
    return (ival ? true : NIL);
}

/* comparison functions */
LVAL xlss() { return (compare('<')); } /* < */
LVAL xleq() { return (compare('L')); } /* <= */
LVAL xequ() { return (compare('=')); } /* = */
LVAL xneq() { return (compare('#')); } /* /= */
LVAL xgeq() { return (compare('G')); } /* >= */
LVAL xgtr() { return (compare('>')); } /* > */

/* compare - common compare function */
LOCAL LVAL compare(fcn)
  int fcn;
{
    FIXTYPE icmp,ival,iarg;
    FLOTYPE fcmp,fval,farg;
    LVAL arg;
    int mode;

    /* get the first argument */
    arg = xlgetarg();

    /* set the type of the first argument */
    if (fixp(arg)) {
	ival = getfixnum(arg);
	mode = 'I';
    }
    else if (floatp(arg)) {
	fval = getflonum(arg);
	mode = 'F';
    }
    else
	xlerror("bad argument type",arg);

    /* handle each remaining argument */
    for (icmp = TRUE; icmp && moreargs(); ival = iarg, fval = farg) {

	/* get the next argument */
	arg = xlgetarg();

	/* check its type */
	if (fixp(arg)) {
	    switch (mode) {
	    case 'I':
	        iarg = getfixnum(arg);
	        break;
	    case 'F':
	        farg = (FLOTYPE)getfixnum(arg);
		break;
	    }
	}
	else if (floatp(arg)) {
	    switch (mode) {
	    case 'I':
	        fval = (FLOTYPE)ival;
		farg = getflonum(arg);
		mode = 'F';
		break;
	    case 'F':
	        farg = getflonum(arg);
		break;
	    }
	}
	else
	    xlerror("bad argument type",arg);

	/* compute result of the compare */
	switch (mode) {
	case 'I':
	    icmp = ival - iarg;
	    switch (fcn) {
	    case '<':	icmp = (icmp < 0); break;
	    case 'L':	icmp = (icmp <= 0); break;
	    case '=':	icmp = (icmp == 0); break;
	    case '#':	icmp = (icmp != 0); break;
	    case 'G':	icmp = (icmp >= 0); break;
	    case '>':	icmp = (icmp > 0); break;
	    }
	    break;
	case 'F':
	    fcmp = fval - farg;
	    switch (fcn) {
	    case '<':	icmp = (fcmp < 0.0); break;
	    case 'L':	icmp = (fcmp <= 0.0); break;
	    case '=':	icmp = (fcmp == 0.0); break;
	    case '#':	icmp = (fcmp != 0.0); break;
	    case 'G':	icmp = (fcmp >= 0.0); break;
	    case '>':	icmp = (fcmp > 0.0); break;
	    }
	    break;
	}
    }

    /* return the result */
    return (icmp ? true : NIL);
}

/* badiop - bad integer operation */
LOCAL badiop()
{
    xlfail("bad integer operation");
}

/* badfop - bad floating point operation */
LOCAL badfop()
{
    xlfail("bad floating point operation");
}
