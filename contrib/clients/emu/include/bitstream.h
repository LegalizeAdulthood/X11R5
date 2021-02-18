#ifndef	_BITSTREAM_H_INCLUDE
#define	_BITSTREAM_H_INCLUDE

/* $Header: /usr3/emu/include/RCS/bitstream.h,v 1.4 90/10/12 14:02:17 jkh Exp Locker: jkh $ */

/*
 * This file is public domain. It was written by Paul Vixie, currently
 * of Digital Equipment Corportation. Because of the organization of
 * the emu system, a number of system dependent macros have been moved
 * into os.h, thus this file is no longer stand-alone. If you need to
 * use it for something else, make a copy of both it and os.h and merge
 * the two appropriately.
 */

/*
 * bitstring.h - bit string manipulation macros
 *
 * vix 26feb87 [written]
 * vix 03mar87 [fixed stupid bug in setall/clearall]
 * vix 25mar87 [last-minute cleanup before mod.sources gets it]
 *
 * $Log:	bitstream.h,v $
 * Revision 1.4  90/10/12  14:02:17  jkh
 * Changed comment.
 * 
 * Revision 1.3  90/10/12  13:59:17  jkh
 * *** empty log message ***
 * 
 * Revision 1.2  90/08/26  02:23:31  jkh
 * Silly.
 * 
 * Revision 1.1  90/08/26  01:53:38  jkh
 * Initial revision
 * 
 *
 */

/*
 * there is something like this in 4.3, but that's licensed source code that
 * I'd rather not depend on, so I'll reinvent the wheel (incompatibly).
 */

/*
 * except for the number of bits per int, and the other constants, this should
 * port painlessly just about anywhere.  please #ifdef any changes with your
 * compiler-induced constants (check the CC man page, it'll be something like
 * 'vax' or 'mc68000' or 'sun' or some such).  also please mail any changes
 * back to me (ucbvax!dual!ptsfa!vixie!paul) for inclusion in future releases.
 */

/* (macros used internally) */

/* how many int's in a string of N bits? */
#define	_bit_size(N) (((N) / _bit_intsiz) + (((N) % _bit_intsiz) ? 1 : 0))

/* which int of the string is bit N in?	 */
#define	_bit_intn(N) ((N) / _bit_intsiz)

/* mask for bit N in it's int */
#define	_bit_mask(N) (1 << ((N) % _bit_intsiz))

/* (macros used externally) */

/* declare (create) Name as a string of N bits */
#define	bit_decl(Name, N)	_bit_type Name[_bit_size(N)]

/* declare (reference) Name as a bit string */
#define	bit_ref(Name)		_bit_type *Name

/*
 * allocate a dynamic bitstring, thanks to:
 *	lef@nlm.nih.gov.arpa
 *	L Fitzpatrick
 *	National Library of Medicine
 *	Bethesda, MD 10894
 */
#define bit_new(N) \
     (_bit_type *) malloc((unsigned)_bit_size(N) * sizeof(_bit_type));

/* is bit N of string Name set? */
#define	bit_test(Name, N)	((Name)[_bit_intn(N)] & _bit_mask(N))

/* set bit N of string Name */
#define	bit_set(Name, N)	{ (Name)[_bit_intn(N)] |= _bit_mask(N); }

/* clear bit N of string Name */
#define	bit_clear(Name, N)	{ (Name)[_bit_intn(N)] &= ~_bit_mask(N); }

/* set bits 0..N in string Name */
#define	bit_setall(Name, N) \
	{	register _bit_i; \
		for (_bit_i = _bit_size(N)-1; _bit_i >= 0; _bit_i--) \
			Name[_bit_i]=_bit_1s; \
	}

/* clear bits 0..N in string Name */
#define	bit_clearall(Name, N) \
	{	register _bit_i; \
		for (_bit_i = _bit_size(N)-1; _bit_i >= 0; _bit_i--) \
			Name[_bit_i]=_bit_0s; \
	}

#endif	/* _BITSTREAM_H_INCLUDE */
