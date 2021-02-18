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
 * Project: XMON - An X protocol monitor
 *
 * File: select_args.h
 *
 * Description: macros to handle the arguments to select().
 */

#ifndef SELECT_H
#define SELECT_H

#include <sys/param.h>

#define MAXSOCKS (NOFILE - 1)
#define mskcnt ((MAXSOCKS + 31) / 32)	/* size of bit array */

#if (mskcnt==1)
#define BITMASK(i) (1 << (i))
#define MASKIDX(i) 0
#endif
#if (mskcnt>1)
#define BITMASK(i) (1 << ((i) & 31))
#define MASKIDX(i) ((i) >> 5)
#endif

#define MASKWORD(buf, i) buf[MASKIDX(i)]
#define BITSET(buf, i) MASKWORD(buf, i) |= BITMASK(i)
#define BITCLEAR(buf, i) MASKWORD(buf, i) &= ~BITMASK(i)
#define GETBIT(buf, i) (MASKWORD(buf, i) & BITMASK(i))

#if (mskcnt==1)
#define COPYBITS(src, dst) dst[0] = src[0]
#define CLEARBITS(buf) buf[0] = 0
#define MASKANDSETBITS(dst, b1, b2) dst[0] = (b1[0] & b2[0])
#define ORBITS(dst, b1, b2) dst[0] = (b1[0] | b2[0])
#define UNSETBITS(dst, b1) (dst[0] &= ~b1[0])
#define ANYSET(src) (src[0])
#endif
#if (mskcnt==2)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
	      dst[0] = (b1[0] & b2[0]);\
	      dst[1] = (b1[1] & b2[1])
#define ORBITS(dst, b1, b2)  \
	      dst[0] = (b1[0] | b2[0]);\
	      dst[1] = (b1[1] | b2[1])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]
#define ANYSET(src) (src[0] || src[1])
#endif
#if (mskcnt==3)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
	      dst[0] = (b1[0] & b2[0]);\
	      dst[1] = (b1[1] & b2[1]);\
	      dst[2] = (b1[2] & b2[2])
#define ORBITS(dst, b1, b2)  \
	      dst[0] = (b1[0] | b2[0]);\
	      dst[1] = (b1[1] | b2[1]);\
	      dst[2] = (b1[2] | b2[2])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]; \
		      dst[2] &= ~b1[2]
#define ANYSET(src) (src[0] || src[1] || src[2])
#endif
#if (mskcnt==4)
#define COPYBITS(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];\
	      dst[3] = src[3]
#define CLEARBITS(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0; buf[3] = 0
#define MASKANDSETBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] & b2[0]);\
		      dst[1] = (b1[1] & b2[1]);\
		      dst[2] = (b1[2] & b2[2]);\
		      dst[3] = (b1[3] & b2[3])
#define ORBITS(dst, b1, b2)  \
		      dst[0] = (b1[0] | b2[0]);\
		      dst[1] = (b1[1] | b2[1]);\
		      dst[2] = (b1[2] | b2[2]);\
		      dst[3] = (b1[3] | b2[3])
#define UNSETBITS(dst, b1) \
		      dst[0] &= ~b1[0]; \
		      dst[1] &= ~b1[1]; \
		      dst[2] &= ~b1[2]; \
		      dst[3] &= ~b1[3]
#define ANYSET(src) (src[0] || src[1] || src[2] || src[3])
#endif

#if (mskcnt>4)
#define COPYBITS(src, dst) bcopy((caddr_t) src, (caddr_t) dst,\
		 mskcnt*sizeof(long))
#define CLEARBITS(buf) bzero((caddr_t) buf, mskcnt*sizeof(long))
#define MASKANDSETBITS(dst, b1, b2)  \
	      { int cri;	    \
	    for (cri=0; cri<mskcnt; cri++)  \
		  dst[cri] = (b1[cri] & b2[cri]); }
#define ORBITS(dst, b1, b2)  \
	      { int cri;	    \
	      for (cri=0; cri<mskcnt; cri++)	\
		  dst[cri] = (b1[cri] | b2[cri]); }
#define UNSETBITS(dst, b1) \
	      { int cri;	    \
	      for (cri=0; cri<mskcnt; cri++)	\
		  dst[cri] &= ~b1[cri];	 }
/* ANYSET is a function defined in main.c */
#endif

#endif /* SELECT_H */
