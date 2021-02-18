/*

Copyright 1991 by OTC Limited


Permission to use, copy, modify, distribute, and sell this software and its
documentation for any purpose is hereby granted without fee, provided that
the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation, and that the name of OTC Limited not be used in advertising or
publicity pertaining to distribution of the software without specific,
written prior permission. OTC Limited makes no representations about the
suitability of this software for any purpose. It is provided "as is"
without express or implied warranty.

Any person supplied this software by OTC Limited may make such use of it
including copying and modification as that person desires providing the
copyright notice above appears on all copies and modifications including
supporting documentation.

The only conditions and warranties which are binding on OTC Limited in
respect of the state, quality, condition or operation of this software
are those imposed and required to be binding by statute (including the
Trade Practices Act 1974). and to the extent permitted thereby the
liability, if any, of OTC Limited arising from the breach of such
conditions or warranties shall be limited to and completely discharged
by the replacement of this software and otherwise all other conditions
and warranties whether express or implied by law in respect of the
state, quality, condition or operation of this software which may
apart from this paragraph be binding on OTC Limited are hereby
expressly excluded and negatived.

Except to the extent provided in the paragraph immediately above OTC
Limited shall have no liability (including liability in negligence) to
any person for any loss or damage consequential or otherwise howsoever
suffered or incurred by any such person in relation to the software
and without limiting the generality thereof in particular any loss or
damage consequential or otherwise howsoever suffered or incurred by
any such person caused by or resulting directly or indirectly from any
failure, breakdown, defect or deficiency of whatsoever nature or kind
of or in the software.

*/

/*
 * File: common.c
 *
 * Description: Standard header file
 *
 */

#ifndef COMMON_H
#define COMMON_H

#include    <ctype.h>
#include    <sys/types.h>
#include    <string.h>
#include    <stdio.h>

typedef	    short		Bool;
typedef	    caddr_t		Pointer;
typedef	    void		(*VoidCallback)();  /* ptr to void function */
typedef	    int			(*IntCallback)();   /* ptr to int function */

#define	    Global		/* Global */
#ifndef True
#define	    True		(1)
#define	    False		(0)
#endif
#define	    Streq(a,b)		(strcmp(a,b) == 0)
#define	    Strneq(a,b,n)	(strncmp(a,b,n) == 0)
#define	    abs(x)		(((x) < 0) ? (-(x)) : (x))
#define	    max(n1, n2)		(((n1) < (n2)) ? (n2) :	 (n1))
#define	    min(n1, n2)		(((n1) > (n2)) ? (n2) :	 (n1))
#define	    Tmalloc(type)	(type *)malloc(sizeof(type))
#define	    Tcalloc(n, type)	(type *)calloc(n, sizeof(type))
#define	    Tfree(ptr)		free((char *)(ptr))

#ifdef	__STDC__
#define	    P(args)		args
#else
#define	    P(args)		()
#endif

void *malloc P((int size));
void *realloc P((char *ptr, int size));
void *calloc P((int nelem, int elsize));
void free P((char *ptr));

#endif	/* COMMON_H */
