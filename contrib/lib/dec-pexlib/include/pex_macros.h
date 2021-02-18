
/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*
 *  File Name:  
 *	pex_macros.h
 *
 *  Subsystem:
 *	common PEX global header files
 *
 *  Version: 
 *	0.9
 *
 *  Abstract:
 *	Define macros for general use for the common PEX project.
 *
 *  Keywords:
 *	Macros
 *
 *  Environment:
 *	#include'd by ALL common PEX `.c' files.
 *
 *  Author:
 *	Andrew F. Vesper
 *
 *  Creation Date: 27-July-1988
 *
 *  Modification History:
 *
 *  27-Jul-1988	AFV	Created  (modified from an earlier project)
 *  28-Jul-1988	AFV	Added FORMAT, renamed SYNOPSIS to EXAMPLE(S)
 *   2-Aug-1988 AFV	Fixed some bugs and typos.
 *  28-Nov-1988 PLN	Add Success and Failure
 *  30-Nov-1988 AFV	added COUNT_ONES macro
 *   8-Dec-1988 AFV	use built-ins for vaxc v3.0 if USE_BUILTINS is
 *			defined
 *   9-Dec-1988 AFV	define GET_NEXT_MASK_BIT_INDEX and GET_NEXT_MASK_BIT
 *  10-Feb-1989 JTF	define SIZEOF_VERTEX, etc.
 *  24-Mar-1989 AFV	added queue data types and queue manipulation macros.
 *  10-Apr-1989 AFV	move #pragma builtins to pragma_builtins.h
 *  11-Apr-1989 AFV	moved ARGS macro definition before use in declaring ffs
 *  30-Jun-1989	AFV	Added offsetof macro for pmax (supplied by Paula Womack)
 *  30-Jun-1989	AFV	Added test for MAX_SIZE_BUILTIN to call memxxx routines
 *			rather than using builtins if the size is too large.
 *  18-Aug-1989	AFV	Corrected spelling to MAX_SIZE_BUILTIN
 *  28-Sep-1989	AFV	Forced last arg of _FFS to (int *)
 *  13-Oct-1989	AFV	Update table of contents
 *  13-Oct-1989	AFV	Fix INSERT_INTO_QUEUE_BEFORE (. should be ->)
 *  12-Oct-1990	AFV	Add COPY_XFORM and COPY_STATIC_SIZE_AREA
 *  15-May-91	AFV	Get rid of SWAP macro
 */

#ifndef PEX_MACROS_DEFINED

#define PEX_MACROS_DEFINED

/*
 *  Table of Contents  (Routine names in order of appearance)
 */

/*
 *
 *	    Sizes of things
 *
 * M	SIZEOF_VERTEX
 * M	SIZEOF_VERTEX_OPTDATA
 * M	SIZEOF_FACET_OPTDATA
 * M	SIZEOF_VERTEX_INDEX
 * M	offsetof
 * 
 *	    Area manipulation
 *
 * M	COPY_AREA
 * M	COPY_LARGE_AREA
 * M	COPY_SMALL_AREA
 * M	ZERO_AREA
 * M	ZERO_LARGE_AREA
 * M	ZERO_SMALL_AREA
 * M	COPY_XFORM
 * M	COPY_STATIC_SIZE_AREA
 * 
 *	    Routine Prototypes
 *
 * M	ARGS
 * M	NO_ARGS
 *
 *	    Bit mask manipulation
 *
 * M	BIT
 * M	ANY_SET
 * M	ALL_SET
 * M	NONE_SET
 * M	SET_BITS
 * M	CLEAR_BITS
 * M	COUNT_ONES
 * M	GET_NEXT_MASK_BIT
 * M	GET_NEXT_MASK_BIT_INDEX
 *
 *	    Numeric tests and conversions
 *
 * M	IN_RANGE
 * M	OUT_OF_RANGE
 * M	MIN
 * M	MAX
 * M	ROUND
 * M	ABS
 * M	ABSF
 * M	EQUALS_APPROXIMATELY
 *
 *	    Miscellaneous
 *
 * M	COUNT_OF
 *
 *	    BOOL_ENUM testing
 *
 * M	IS_TRUE
 * M	IS_FALSE
 *
 *	    ISO Latin-1 character set tests and conversions
 *
 * M	LOWERCASE
 * M	UPPERCASE
 * M	ALPHABETIC
 * M	CONVERT_TO_LOWER
 * M	CONVERT_TO_UPPER
 *
 *	    QUEUE manipulation
 *
 * M	INIT_QUEUE_HEAD
 * M	INSERT_INTO_QUEUE_AFTER
 * M	INSERT_INTO_QUEUE_BEFORE
 * M	REMOVE_FROM_QUEUE
 * M    NEXT_QUEUE_LINK
 * M    PREVIOUS_QUEUE_LINK
 * M    FIRST_QUEUE_ELEMENT
 * M    LAST_QUEUE_ELEMENT
 * M    NEXT_QUEUE_ELEMENT
 * M    PREVIOUS_QUEUE_ELEMENT
 * M    IS_QUEUE_HEAD
 * 
 */

/*
 *  Include Files
 */

/*
 * "pex_features.h" MUST be #include'd BEFORE this file is.
 * "pex_standard_fixup.h" should be also.
 * <stdio.h> and <stdlib.h> must not be included after this file is.
 */

/*
 *  Constant, struct and typedef Definitions
 */

#ifdef USE_BUILTINS
# define MAX_SIZE_BUILTIN  65535
# include "use_builtins.h"	    /* USE_BUILTINS ONLY */
#endif

/*
 *  The BOOL_ENUM type must be used with IS_TRUE and IS_FALSE
 */

typedef enum bool_enum {BOOL_FALSE, BOOL_TRUE} BOOL_ENUM;

#if 0
#define	TRUE		(BOOL_TRUE)
#define FALSE		(BOOL_FALSE)
#endif

/*
 * Define some basic types.  Use these whenever a specific size or type
 * is needed.  If you just want a `generic int', use `int'.
 *
 * Use MASKxx for declaring variables to be used as bit masks.
 *
 * note: #define has been used rather than typedef where possible
 * to keep lint from complaing about a call to a routine that expects 
 * a INT32, for example, with a constant (default type of int).
 */

#if 0		/* do not define these here -- let pexlib.h define them */

#define INT32		long
#define INT16		short

#ifdef __STDC__
#define INT8		signed char
#else
#define INT8		char
#endif

#define CARD32		unsigned long
#define CARD16		unsigned short
#define CARD8		unsigned char

#endif

#define MASK32		unsigned long
#define MASK16		unsigned short
#define MASK8		unsigned char

/*
 * Use ADDRESS to point to an arbitrary object.
 * Always cast to and from ADDRESS types:
 *
 *  PEX_TYPE * pt_ptr;
 *  ADDRESS  gen_ptr;
 *
 *	pt_ptr = (PEX_TYPE *) gen_ptr;
 *	gen_ptr = (ADDRESS) pt_ptr;
 *
 * Use ADDRESS only when different objects can be referenced from
 * a pointer and it is too awkward to use a `union'.
 */

#if defined(__STDC__) || defined (VAXC)
typedef void		* ADDRESS;
#else
typedef char		* ADDRESS;
#endif

/*
 * Define types for double-linked queue usage
 */

typedef struct QUEUE_HEAD_OR_LINK
{
    struct QUEUE_HEAD_OR_LINK		* next;
    struct QUEUE_HEAD_OR_LINK		* previous;
} QUEUE_HEAD, QUEUE_LINK;

/* 
 * apply the following to routines for descriptive purposes.
 * IMPORT means the routine is defined in another module and used in this one.
 * INTERNAL means the routine is defined and used only in this module.
 */

#define INTERNAL	static
#define IMPORT		/* for documentation only */

/* declare how routine parameters will be used */

#define INPUT		/* for documentation only */
#define OUTPUT		/* for documentation only */
#define INPUT_OUTPUT	/* for documentation only */

#ifdef __STDC__
#   define	CONSTANT	const
#   define	VOLATILE	volatile
#else
#   ifdef VAXC
#	define	VOLATILE	volatile
#	define	CONSTANT	readonly
#   else
#	define	VOLATILE	/* for documentation only */
#	define	CONSTANT	/* for documentation only */
#   endif
#endif

/* storage classes for shared data					*/

#ifdef VMS
#   define EXTERNAL_REF	globalref
#   define EXTERNAL_DEF	globaldef
#else
#   define EXTERNAL_REF	extern
#   define EXTERNAL_DEF	/* empty */
#endif

/* use INTERNAL storage class for module-wide data			*/

/* storage classes used within blocks					*/
/* note that PERSISTANT variables should be initialized			*/

#define REGISTER	register
#define LOCAL		auto
#define PERSISTANT	static

/*
 * Define commonly used constants
 */

#ifndef NULL
#   define	NULL	0
#endif

#ifndef EOF
#   define	EOF	(-1)
#endif

#ifndef EOS
#   define	EOS	'\0'
#endif

#define	FOREVER		(1 == 1)
#define NEVER		(0 == 1)

#ifdef USE_BUILTINS
# define LARGE_AREA_SIZE	10
#else
# define LARGE_AREA_SIZE	40
#endif

/* Success and Failure codes (do the check because X.H defines Success */

#ifndef	Success
#define	Success	0
#endif
#ifndef	Failure
#define	Failure -1
#endif


/*
 *+
 *  NAME:
 *     SIZEOF_VERTEX, SIZEOF_VERTEX_OPTDATA,
 *     SIZEOF_FACET_OPTDATA, SIZEOF_VERTEX_INDEX
 *
 *  FORMAT:
 *	int SIZEOF_VERTEX (pformat, flags, prm_type)
 *	int SIZEOF_VERTEX_OPTDATA (pformat, flags, prm_type)
 *	int SIZEOF_FACET_OPTDATA (pformat, flags)
 *	int SIZEOF_VERTEX_INDEX (flags)
 *
 *  ARGUMENTS:
 *     pformat	    pointer to pexFormat structure
 *     flags	    flags to indicate colors, normals, edges, etc.
 *     prm_type	    type of primitive - see #defines at beginning
 *
 *  RETURNS:
 *     size of vertex, optional data, index...
 *
 *  EXAMPLES:
 *     for ( i = 0 ; i < nvertices ; i++) {
 *	...
 *	...
 *	p += SIZEOF_VERTEX (pformat, flags, FILL_AREA_3D_DATA);
 *     }	
 *
 *  DESCRIPTION:
 *	Returns the size of the object, given the direct color format, the
 *	various primitive flags, and the primitive type. If the primitive
 *	type is specified using the constant definitions, the decisions
 *	based on primitive type will be constant-folded by the compiler,
 *	and therefore not executed at run-time.
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *
 *  AUTHOR:
 *     John T. Freitas
 *
 *  CAVEATS:
 *     None. Always works perfectly. (Keeping the GIGO principle in mind.)
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#define PRM_POLYLINE_4D		    10
#define PRM_POLYLINE_3D		    20
#define PRM_POLYLINE_SET	    30
#define PRM_FILL_AREA_3D_DATA	    40
#define PRM_FILL_AREA_SET_3D_DATA   50
#define PRM_TRIANGLE_STRIP_3D	    60
#define PRM_TRIANGLE_STRIP_4D	    70
#define PRM_QUAD_MESH		    80
#define PRM_INDEXED_POLY	    90


#define SIZEOF_VERTEX(pformat,flags,prm_type)		    \
(							    \
    ((prm_type == PRM_POLYLINE_4D) ||			    \
    (prm_type == PRM_TRIANGLE_STRIP_4D) ?		    \
        sizeof(pexCoord4D) :				    \
        sizeof(pexCoord3D))				    \
    +							    \
    SIZEOF_VERTEX_OPTDATA(pformat,flags,prm_type)	    \
)


#define SIZEOF_VERTEX_OPTDATA(pformat,flags,prm_type)	    \
(							    \
    ((flags & VERTEX_COLORS_MASK) ?			    \
	((flags & INDEXED_COLOR_MASK) ?			    \
	    sizeof(pexIndexedColor) :			    \
	    pex_color_size[pformat->directColorFormat]) :   \
	0)						    \
    +							    \
    ((prm_type == PRM_FILL_AREA_3D_DATA) ||		    \
    (prm_type == PRM_FILL_AREA_SET_3D_DATA) ||		    \
    (prm_type == PRM_TRIANGLE_STRIP_3D) ||		    \
    (prm_type == PRM_TRIANGLE_STRIP_4D) ||		    \
    (prm_type == PRM_QUAD_MESH) ||			    \
    (prm_type == PRM_INDEXED_POLY) ?			    \
	((flags & VERTEX_NORMALS_MASK) ?		    \
	    sizeof(pexCoord3D) :			    \
	    0) :					    \
	0)						    \
    +							    \
    ((prm_type == PRM_FILL_AREA_SET_3D_DATA) ||		    \
    (prm_type == PRM_TRIANGLE_STRIP_3D) ||		    \
    (prm_type == PRM_TRIANGLE_STRIP_4D) ?		    \
	((flags & VERTEX_EDGES_MASK) ?			    \
	    sizeof(pexSwitch) + 3 :   /* 3 = padding */	    \
	    0) :						    \
	0)						    \
)


#define SIZEOF_FACET_OPTDATA(pformat,flags)		    \
(							    \
    ((flags & FACET_COLORS_MASK) ?			    \
	((flags & INDEXED_COLOR_MASK) ?			    \
	    sizeof(pexIndexedColor) :			    \
	    pex_color_size[pformat->directColorFormat]) :   \
	0)						    \
    +							    \
    ((flags & FACET_NORMALS_MASK) ?			    \
	sizeof(pexCoord3D) :				    \
	0)						    \
)


#define SIZEOF_VERTEX_INDEX(flags)			    \
(							    \
    sizeof(int)						    \
    +							    \
    ((flags & VERTEX_EDGES_MASK) ?			    \
	sizeof(pexSwitch) + 1 :	    /* 1 = Padding */	    \
	0)						    \
)



/*
 *+
 *  NAME:
 *     offsetof
 *
 *  FORMAT:
 *     int offsetof (cstruct, element)
 *
 *  ARGUMENTS:
 *     cstruct	    name of c structure
 *     element	    name of element in the c structure
 *
 *  RETURNS:
 *     number of bytes from the start of the structure to the given
 *     element.
 *
 *  EXAMPLES:
 *     i = offsetof (COORD3D,x);
 *
 *  DESCRIPTION:
 *     computes the offset of the given member in the given structure.
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *	None
 *
 *  AUTHOR:
 *     Paula Womack
 *
 *  CAVEATS:
 *     none
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#ifndef offsetof
# define offsetof(_cstruct_,_element_)	\
    ( (int) & ( ((_cstruct_ *) 0) -> _element_) )
#endif


/*
 *+
 *  NAME:
 *     COPY_AREA, COPY_SMALL_AREA, COPY_LARGE_AREA
 *
 *  FORMAT:
 *     void COPY_AREA(char *from, char *to, int size);
 *     void COPY_SMALL_AREA(char *from, char *to, int size);
 *     void COPY_LARGE_AREA(char *from, char *to, int size);
 *     void COPY_XFORM (pexMatrix from, pexMatrix to);
 *     void COPY_STATIC_SIZE_AREA (char * from, char * to, int size);
 *
 *  ARGUMENTS:
 *     from	    address of source memory block
 *     to	    address of destination memory block
 *     size	    number of bytes to copy
 *
 *  RETURNS:
 *     void
 *
 *  EXAMPLES:
 *     COPY_AREA (old, new, n * sizeof (STR));
 *     COPY_STATIC_SIZE_AREA (old, new, sizeof (STR));
 *
 *  DESCRIPTION:
 *     Copies the given number of bytes.  If the _SMALL_ version is
 *     used, it expands into a loop, copying a byte at a time. The _LARGE_
 *     version calls memcpy.  The plain version chooses one or the other
 *     by comparing with LARGE_AREA_SIZE.
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *     ZERO_AREA etc.
 *
 *  AUTHOR:
 *     Andrew F. Vesper
 *
 *  CAVEATS:
 *     It is not defined what happens when the areas overlap.
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */


#define COPY_AREA(_from_,_to_,_size_)   \
    {if ( (_size_) >= LARGE_AREA_SIZE) \
        { COPY_LARGE_AREA(_from_,_to_,_size_); } \
     else {COPY_SMALL_AREA(_from_,_to_,_size_);}}

#ifdef USE_BUILTINS
# define COPY_LARGE_AREA(_from_,_to_,_size_) \
    { if ( (_size_) <= MAX_SIZE_BUILTIN) \
	{_MOVC3 ((_size_),(char *)(_from_),(char *)(_to_)); } \
      else memcpy (_to_,_from_,_size_);}
#else
# define COPY_LARGE_AREA(_from_,_to_,_size_) \
    memcpy (_to_,_from_,_size_)
#endif

#define COPY_SMALL_AREA(_from_,_to_,_size_) \
    {register char *_f = (char *)(_from_), *_t = (char *)(_to_); \
	register int _c = (_size_); while (--_c >= 0) *_t++ = *_f++;}

typedef struct
{
    float	m[4][4];
} * XformPtr;

#define COPY_XFORM(_from_,_to_)	\
{ *( (XformPtr) (_to_)) = * ( (XformPtr) (_from_)); }

#define COPY_STATIC_SIZE_AREA(_from_,_to_,_size_)	\
    { typedef struct {char xxx[_size_];} _sss;		\
      *((_sss *) _to_) = *((_sss *) _from_);		\
    }

/*
 *+
 *  NAME:
 *     ZERO_AREA, ZERO_SMALL_AREA, ZERO_LARGE_AREA
 *
 *  FORMAT:
 *     void ZERO_AREA(char *area, int size);
 *     void ZERO_SMALL_AREA(char *area, int size);
 *     void ZERO_LARGE_AREA(char *area, int size);
 *
 *  ARGUMENTS:
 *     area	    address of memory block
 *     size	    number of bytes to zero
 *
 *  RETURNS:
 *     void
 *
 *  EXAMPLES:
 *     ZERO_AREA (new, sizeof (STR));
 *
 *  DESCRIPTION:
 *     Zero's the given number of bytes.  If the _SMALL_ version is
 *     used, it expands into a loop, zeroing a byte at a time. The _LARGE_
 *     version calls memset.  The plain version chooses one or the other
 *     by comparing with LARGE_AREA_SIZE.
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *     COPY_AREA etc.
 *
 *  AUTHOR:
 *     Andrew F. Vesper
 *
 *  CAVEATS:
 *     None
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#define ZERO_AREA(_area_,_size_)   \
    {if ( (_size_) >= LARGE_AREA_SIZE) {ZERO_LARGE_AREA(_area_,_size_);} \
    else ZERO_SMALL_AREA (_area_, _size_);}

#ifdef USE_BUILTINS
# define ZERO_LARGE_AREA(_area_,_size_) \
    { if ( (_size_) <= MAX_SIZE_BUILTIN) \
	{unsigned long _dummy; \
	    _MOVC5 (0, (char *) 0, 0, (_size_), (char *)(_area_), \
	    (unsigned short *) &_dummy ); } \
      else memset (_area_,0,_size_);}
#else
# define ZERO_LARGE_AREA(_area_,_size_)   \
    memset (_area_,0,_size_)
#endif

#define ZERO_SMALL_AREA(_area_,_size_)   \
    {register char *_a = (char *) (_area_); \
	register int _c = (_size_); while (--_c >= 0) *_a++ = 0;}

/*+
 *  NAME: 
 *	ARGS, NO_ARGS		(MACROS)
 *
 *  FORMAT:
 *	ARGS (arglist)
 *	NO_ARGS
 *	    
 *
 *  ARGUMENTS:
 *	arglist	    (ARGS) parenthesised list of argument types (with or 
 *		    without names)
 *	none	    (NO_ARGS)
 *
 *  RETURNS:
 *	Nothing.  Use only when declaring external routines in a header file
 *	or when declaring internal routines in a .c file.
 *
 *  EXAMPLES:
 *      INTERNAL int internal_routine ARGS ((
 *		int arg1, float *arg2));
 *
 *	IMPORT int * external_routine NO_ARGS;
 *
 *  DESCRIPTION:
 *	These macros allow conditional compilation of function prototypes,
 *	the new ANSI C Draft Standard way of declaring routines.
 *	Note that you must use double parentheses with ARGS.
 *
 *	The prototypes are enabled by #define'ing ROUTINE_TEMPLATES
 *	(see pex_features.h).
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	You MUST use double parentheses with ARGS.
 */

#ifdef ROUTINE_TEMPLATES
#   define ARGS(_args)	    _args
#   define NO_ARGS	    (void)
#else
#   define ARGS(_args)	    ()
#   define NO_ARGS	    ()
#endif

/*+
 *  NAME: 
 *	BIT		(MACRO)
 *
 *  FORMAT:
 *	MASKxx BIT (bit_number)
 *	    int		bit_number;
 *
 *  ARGUMENTS:
 *	bit_number	The number of the bit: 0 is the low order bit.
 *
 *  RETURNS:
 *	A `MASKxx' with the appropriate bit set.
 *
 *  EXAMPLES:
 *      mask = BIT (1);
 *	#define  MASK_BIT_XXX	BIT (0)
 *
 *  DESCRIPTION:
 *	BIT is useful for setting up bit masks.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The argument must range between 0 and n - 1 where `n' is the
 *	number of bits in the desired result.  For example, if you are
 *	using a MASK8 variable to store the masks, the argument must
 *	be between 0 and 7 inclusive.
 */

#define BIT(_n)	(1 << (_n) )

/*+
 *  NAME: 
 *	ANY_SET, ALL_SET, NONE_SET	(MACROS)
 *
 *  FORMAT:
 *	boolean ANY_SET (mask, bits)
 *	boolean ALL_SET (mask, bits)
 *	boolean NONE_SET (mask, bits)
 *	    MASKxx	    mask;
 *	    MASKxx	    bits;
 *
 *  ARGUMENTS:
 *	mask	MASKxx variable you wish to test
 *	bits	MASKxx of bits you wish to test
 *
 *  RETURNS:
 *	A boolean
 *
 *  EXAMPLES:
 *      if (ANY_SET (mask, MASK_VALUE_1 | MASK_VALUE_2)) ...
 *      if (ALL_SET (mask, MASK_VALUE_1 | MASK_VALUE_2)) ...
 *      if (NONE_SET (mask, MASK_VALUE_1 | MASK_VALUE_2)) ...
 *
 *  DESCRIPTION:
 *	Use these macros to test for bits set or clear in a mask.
 *	They help reduce risk of improper precedence assumptions.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The second argument is evaluated twice in ALL_SET.  Don't
 *	use a function call or expression with side effects (e.g. *q++)
 */


#define ANY_SET(_mask, _bits)	( ( (_mask) & (_bits) ) != 0)

#define ALL_SET(_mask, _bits)   ( ( (_mask) & (_bits) ) == (_bits) )

#define NONE_SET(_mask, _bits)	( ( (_mask) & (_bits) ) == 0)


/*+
 *  NAME: 
 *	SET_BITS, CLEAR_BITS		(MACROS)
 *
 *  FORMAT:
 *	MASKxx SET_BITS (mask, bits)
 *	MASKxx CLEAR_BITS (mask, bits)
 *	    MASKxx	    mask;
 *	    MASKxx	    bits;
 *
 *  ARGUMENTS:
 *	mask	MASKxx variable you wish to modify
 *	bits	MASKxx of bits you wish to set or clear
 *
 *  RETURNS:
 *	The resultant MASKxx is set in the mask variable and is also
 *	returned, if you care to use one of these macros as an expression.
 *
 *  EXAMPLES:
 *      SET_BITS (mask, MASK_VALUE_1 | MASK_VALUE_2);
 *      CLEAR_BITS (mask, MASK_VALUE_1 | MASK_VALUE_2);
 *
 *  DESCRIPTION:
 *	Use these macros to set or clear bits in a mask.
 *	They help reduce risk of improper precedence assumptions.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	I suggest you never use these as an expression to make sure
 *	anyone reading the code knows that the first argument is changed.
 */

#define SET_BITS(_mask, _bits)	(_mask |= (_bits) )

#define CLEAR_BITS(_mask, _bits)  (_mask &= ~ (_bits) )

/*
 *+
 *  NAME:
 *     COUNT_ONES
 *
 *  FORMAT:
 *     int COUNT_ONES (mask);
 *     MASK32	    mask;
 *
 *  ARGUMENTS:
 *     mask	    a 32 bit mask
 *
 *  RETURNS:
 *     The number of bits set (i.e. 1) in the mask.
 *
 *  EXAMPLES:
 *     n = COUNT_ONES (my_mask);
 *     int  n = COUNT_ONES (COMPILE_TIME_CONSTANT_MASK);
 *
 *  DESCRIPTION:
 *
 *  This algorithm was taken from the VAX C notes file.
 *  Here is an edited version of the note.
 *  
 *  <<< TURRIS::NOTE$:[NOTES$LIBRARY]VAXC.NOTE;2 >>>
 *  -< VAX C Notes >-
 *  Note 493.4    help:Macrodef. for bit-vector-initialisation       4 of 12
 *  VIDEO::LEICHTERJ "Jerry Leichter"            94 lines  14-FEB-1987 10:48
 *  -< CORRECT clever BITCOUNT macro >-
 *
 *  For those who are really curious, here's how the algorithm works.
 *
 *  Consider a 4-bit binary number x, written as abcd.  We would like to
 *  replace it by the number of 1-bits it contains.  Since this is just the sum
 *  of its bits, BITCOUNT(x)=a+b+c+d=((x>>3)&1)+((x>>2)&1)+((x>>1)&1)+(x&1).
 *  This works, but there is a more clever approach.  x's value is given by
 *  8a+4b+2c+d. If x is considered unsigned, then (x>>1) is 0abc or 4a+2b+c.
 *  So x-(x>>1) is 4a+2b+c+d - part of the way to a+b+c+d.  Repeating this
 *  shift-and-subtract, we see that x-(x>>1)-(x>>2) is 2a+b+c+d, and so
 *
 *  	BITCOUNT_4(x) = x-(x>>1)-(x>>2)-(x>>3)
 *
 *  Now consider a full 32-bit x.  It can be viewed as a vector of 8 4-bit
 *  numbers, and a shift applies to each of them independently.  Of course, the
 *  same shift brings in bits from the next 4-bit number to the left, not zero
 *  bits; but we can mask those out.  Once we've done this, we know that there
 *  can't be any "borrows" from 4-bit number to 4-bit number, from the
 *  computation above for the individual 4-bit quantities.  So:
 *
 *	BITCOUNT_4_4(x) = ((x) - (((x)>>1)&0x77777777)
 *			     - (((x)>>2)&0x33333333)
 *			     - (((x)>>3)&0x11111111))
 *
 * simply replaces each 4-bit sequence in x by the number of bits in that
 * sequence.  Now all we need to do is sum them up.  Since each 4 bits can
 * contain at most the number 4, adding two together gives a sum that's at most
 * 8, so will still fit in 4 bits.  So we shift and add:
 *
 *	(BX_(x)+(BX_(x)>>4))
 *
 * Now the rightmost 4 bits contain the number of 1 bits in the rightmost 8
 * bits of x; the next 4 bits we ignore; the next 4 bits again are a count for
 * the next byte in x; and so on.  We mask off the "ignored" bits:
 *
 *	((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F)
 *
 * and once again the sum of all the 4-bit sequences is the value we want.  We
 * could shift and add twice more - perhaps the best approach in executable
 * code where we could save a temporary result - but here every shift and add
 * requires re-computing all the partial results, as in the two calls to BX_()
 * above - so we use a trick.  First, since we've put the sums in every other
 * "rightmost" 4-bit position, and zeroes in the rest of the positions, we can
 * as well consider the number before as a vector of 8-bit quantities, each
 * counting up the number of 1's in an 8-bit sequence in x.  But we can equally
 * think of this object as a base-256 number.  Useful fact:  For a base b
 * number, the sum of the digits mod b-1 is the same as the number mod b-1.
 * (For example, consider b=10, and the number 324:
 *
 *	324 = 100*3+10*2+4 = (99+1)*3+(9+1)*2+4 = (99*3+9*2) + 3+2+4
 *
 * so 324 mod 9 = (99*3+9*2) + 3+2+4 mod 9 = 3+2+4 mod 9.  So, the quantitity
 * we are looking for, the sum of the base-256 digits, is congruent, mod 255,
 * to quantity we've computed.  But the actual number of bits in 32 bits is at
 * most 32, so it makes no difference if we compute mod 255 or not.  Hence,
 * just taking the remainder mod 255 - % 255 in the expansion - gives us the
 * desired result.
 *
 * -- Jerry
 *     
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *     The original note.
 *
 *  AUTHOR:
 *     Jerry Leichter
 *     Andrew F. Vesper
 *
 *  CAVEATS:
 *     None
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#define COUNT_ONES(_x_)	(((BX_(_x_)+(BX_(_x_)>>4)) & 0x0F0F0F0F) % 255)

#define  BX_(_x_)			\
    ((_x_) - (((_x_)>>1)&0x77777777)	\
	   - (((_x_)>>2)&0x33333333)	\
	   - (((_x_)>>3)&0x11111111))

/*
 *+
 *  NAME:
 *     GET_NEXT_MASK_BIT, GET_NEXT_MASK_BIT_INDEX
 *
 *  FORMAT:
 *     void GET_NEXT_MASK_BIT (master_mask, mask_return);
 *     MASK32		master_mask;
 *     MASK32		mask_return;
 *
 *     void GET_NEXT_MASK_BIT_INDEX (master_mask, index_return);
 *     MASK32		master_mask;
 *     int		index_return;
 *
 *  ARGUMENTS:
 *     master_mask	a mask from which to extract the lowest bit and
 *			to clear that bit
 *     mask_return	a variable into which to put a mask containing that
 *			lowest bit
 *     index_return	a variable into which to put the index of the lowest
 *			bit (0 to 31)
 *
 *  RETURNS:
 *     none
 *
 *  EXAMPLES:
 *     GET_NEXT_MASK_BIT_INDEX (mask, index);
 *     GET_NEXT_MASK_BIT (mask, this_mask);
 *
 *  DESCRIPTION:
 *     These macros are used to take apart a mask, fetching the lowest bit
 *     that is set each time they are called.  They clear that bit in
 *     the master mask so that next time they return the next lowest bit.
 *     _BIT returns a mask with that one bit set, _BIT_INDEX returns the
 *     index of the bit (0 to 31)
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *     {@references@}
 *
 *  AUTHOR:
 *     Andrew F. Vesper
 *
 *  CAVEATS:
 *     If the master mask is all clear, the results are not defined.
 *
 *     The arguments should be variables, and MUST not contain side effects.
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#ifdef USE_BUILTINS

# define GET_NEXT_MASK_BIT_INDEX(_master_,_index_)	\
    {							\
    (void) _FFS (0, 32, &(_master_), (int *) &(_index_));	\
    (_master_) &= ~ (1 << (_index_));			\
    }

# define GET_NEXT_MASK_BIT(_master_,_new_)		\
    {							\
    int _index;	    					\
    (void) _FFS (0, 32, &(_master_), &_index);		\
    (_new_) = 1 << _index;				\
    (_master_) &= ~ (_new_);				\
    }

#else

  IMPORT int ffs ARGS ((MASK32));
  
# define GET_NEXT_MASK_BIT_INDEX(_master_,_index_)	\
    {							\
    (_index_) = ffs ((_master_)) - 1;			\
    (_master_) &= ~ (1 << (_index_));			\
    }
    
# define GET_NEXT_MASK_BIT(_master_,_new_)		\
    {							\
    register MASK32 _index;				\
    _index = ffs ((_master_)) - 1;			\
    (_new_) = 1 << _index;				\
    (_master_) &= ~ (_new_);				\
    }

#endif

/*+
 *  NAME: 
 *	IN_RANGE, OUT_OF_RANGE	(MACROS)
 *
 *  FORMAT:
 *	boolean IN_RANGE (low, test, high)
 *	boolean OUT_OF_RANGE (low, test, high)
 *	    <numeric>	    low;
 *	    <numeric>	    test;
 *	    <numeric>	    high;
 *
 *  ARGUMENTS:
 *	low	    low value of range  (int or float)
 *	test	    test value (int or float)
 *	high	    high value of range  (int or float)
 *
 *  RETURNS:
 *	A boolean
 *
 *  EXAMPLES:
 *      if (IN_RANGE (0, blob_number, MAX_NUMBER_BLOBS)) ...
 *      if (OUT_OF_RANGE (0.0, angle, 360.0)) ...
 *
 *  DESCRIPTION:
 *	These macros are handy for testing values against ranges.
 *	They make it much more obvious what the low and high values
 *	of the range really are.  Note that the range is always inclusive
 *	of both the low and the high values.
 *
 *	I suggest that all three arguments be the same type (int/float).
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The `test' argument is evaluated once or twice.  Don't
 *	use a function call or expression with side effects (e.g. *q++)
 */

#define IN_RANGE(_min,_value,_max) ((_min) <= (_value) && (_value) <= (_max))

#define OUT_OF_RANGE(_min,_value,_max) ((_min) > (_value) || (_value) > (_max))


/*+
 *  NAME: 
 *	MAX, MIN		(MACROS)
 *
 *  FORMAT:
 *	<numeric> MAX (arg_1, arg_2)
 *	<numeric> MIN (arg_1, arg_2)
 *	    <numeric>	    arg_1;
 *	    <numeric>	    arg_2;
 *
 *  ARGUMENTS:
 *	arg_1	    first argument (int or float)
 *	arg_2	    second argument (int or float)
 *
 *  RETURNS:
 *	The maximum or minimum of the two arguments.  The type
 *	depends on the type of the arguments.
 *
 *  EXAMPLES:
 *      number_blobs = MIN (number_blobs_wanted, MAX_POSSIBLE_BLOBS);
 *      total = MAX (total, count);
 *
 *  DESCRIPTION:
 *	Use these to find the maximum or minimum of two arguments.
 *	They are particularly useful in expressions.
 *
 *	Both arguments must be the same type.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	One of the arguments is evaluated twice.  Don't use a function
 *	call or an expression with side effects (e.g. *q++)
 */

#define MIN( _a, _b ) ( (_a) < (_b) ? (_a) : (_b) )
#define MAX( _a, _b ) ( (_a) > (_b) ? (_a) : (_b) )


/*+
 *  NAME: 
 *	ROUND		(MACRO)
 *
 *  FORMAT:
 *	int ROUND (float_value)
 *	    float/double    float_value;
 *
 *  ARGUMENTS:
 *	float_value	a floating point value
 *
 *  RETURNS:
 *	The int closest to the argument.
 *
 *  EXAMPLES:
 *      int_variable = ROUND (float_value);
 *
 *  DESCRIPTION:
 *	ROUND implements the mathematic function of rounding to the
 *	nearest integer.  If the argument is halfway between two integers
 *	then ROUND returns the lower of the two integers. (i.e. 2.5 => 2)
 *	ROUND works correctly for both positive and negative arguments.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The argument is evaluated twice.  Don't use a function
 *	call or an expression with side effects (e.g. *q++)
 */

#define ROUND( _x ) \
    ( (int) ( ( (_x) > 0.0 ) ? ( (_x) + 0.5 ) : ( (_x) - 0.5 ) ) )

/*+
 *  NAME: 
 *	ABS, ABSF		(MACROS)
 *
 *  FORMAT:
 *	int ABS (value)
 *	    int	    value;
 *
 *	float/double ABSF (value)
 *	    float/double	    value;
 *	    
 *
 *  ARGUMENTS:
 *	value	    the value of which you wish to find the absolute value.
 *		    This should be `int' for ABS and `float' or `double' 
 *		    for ABSF
 *
 *  RETURNS:
 *	The absolute value.
 *
 *  EXAMPLES:
 *      width_bits = ABS (width) * resolution;
 *	slant = ABSF (slant);
 *
 *  DESCRIPTION:
 *	These routines are particularly helpful in expressions.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The argument is evaluated twice.  Don't use a function
 *	call or an expression with side effects (e.g. *q++)
 */

#define ABS( _x ) ( (_x) > 0 ? (_x) : -(_x) )

#define ABSF( _x ) ( (_x) > 0.0 ? (_x) : -(_x) )

/*+
 *  NAME: 
 *	EQUALS_APPROXIMATELY		(MACRO)
 *
 *  FORMAT:
 *	float/double EQUALS_APPROXIMATELY (value, standard, error)
 *	    float/double	value;
 *	    float/double	standard;
 *	    float/double	error;
 *	    
 *
 *  ARGUMENTS:
 *	value	    the value to be tested (usually float/double)
 *	standard    the `proper' value (usually float/double)
 *	error	    the acceptable error (usually float/double)
 *
 *  RETURNS:
 *	A boolean
 *
 *  EXAMPLES:
 *      if (EQUALS_APPROXIMATELY (test, 10.0, 0.01)) ...
 *
 *  DESCRIPTION:
 *	This macro tests the given value against the acceptable range
 *	of `standard' - `error' to `standard' + `error'.
 *	It is intended to be used for floating point comparisons --
 *	use IN_RANGE and OUT_OF_RANGE for int's.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The various arguments may be evaluated once or twice.  Don't 
 *	use a function call or an expression with side effects (e.g. *q++)
 */

#define EQUALS_APPROXIMATELY(_value,_standard, _error)    \
    (IN_RANGE ((_standard) - (_error), (_value), (_standard) + (_error)))


/*+
 *  NAME: 
 *	COUNT_OF		(MACRO)
 *
 *  FORMAT:
 *	int COUNT_OF (array)
 *	    <any_type> array[];
 *	    
 *
 *  ARGUMENTS:
 *	array	    an array of any type
 *
 *  RETURNS:
 *	int
 *
 *  EXAMPLES:
 *      for (i = 0; i < COUNT_OF (table); i++) ...
 *	char *table_2 [COUNT_OF (table_1)];
 *
 *  DESCRIPTION:
 *	Use COUNT_OF to report the number of elements in an array.
 *	If the array is declared or defined with a size known to the
 *	compiler (i.e. anything but EXTERNAL_REF int arr[];), 
 *	COUNT_OF returns a compile time constant that can be used
 *	as the bounds of another array.  Otherwise, COUNT_OF returns
 *	a link time constant that cannot be used as the bounds of another
 *	array.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	The argument must be an array.
 */

#define COUNT_OF(_array)	( sizeof (_array) / sizeof (_array[0]) )


/*+
 *  NAME: 
 *	IS_TRUE, IS_FALSE	(MACROS)
 *
 *  FORMAT:
 *	boolean = IS_TRUE (bool_var)
 *	boolean = IS_FALSE (bool_var)
 *	    BOOL_ENUM	bool_var;
 *
 *  ARGUMENTS:
 *	bool_var    The variable you wish to test
 *
 *  RETURNS:
 *	a boolean
 *
 *  EXAMPLES:
 *      if (IS_TRUE (i_am_happy)) ...
 *      if (IS_FALSE (i_am_never_sick_at_sea)) ...
 *
 *  DESCRIPTION:
 *	IS_TRUE and IS_FALSE MUST be used to test BOOL variables.
 *	If PARANOID is #define'd (see pex_features.h) then the
 *	variable is checked against BOOL_TRUE and BOOL_FALSE and if
 *	it is neither (i.e. junk), the routine pex_bad_bool_value is called.
 *	If PARANOID is not #define'd, the two macros test the low order
 *	bit, which is turned into a BLBS or BLBC instruction by the
 *	VAX C compilers.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  CAVEATS:
 *	Use only with variables of type BOOL_ENUM.  When PARANOID is #define'd,
 *	the argument is evaluated once or twice, so do not
 *	use a function call or an expression with side effects (e.g. *q++)
 */

#ifdef PARANOID

#   define IS_TRUE(_bool_var)  \
    ((_bool_var) == BOOL_TRUE ? (1==1) \
	: ((_bool_var) == BOOL_FALSE ? (0==1) : pex_bad_bool_value (_bool_var)))
#   define IS_FALSE(_bool_var) \
    ((_bool_var) == BOOL_FALSE ? (1==1) \
	: ((_bool_var) == BOOL_TRUE ? (0==1) : pex_bad_bool_value (_bool_var)))

IMPORT int pex_bad_bool_value ARGS ( (BOOL_ENUM) );

#else

#   define IS_TRUE(x)  ( ( (int) (x)) & 1 )
#   define IS_FALSE(x)  ( ! ( ( (int) (x)) & 1 ) )

#endif

/*+
 *  NAME: 
 *	LOWERCASE, UPPERCASE, ALPHABETIC 	(MACROS)
 *
 *  FORMAT:
 *	boolean LOWERCASE (ch)
 *	boolean UPPERCASE (ch)
 *	boolean ALPHABETIC (ch)
 *	    int		ch;
 *
 *  ARGUMENTS:
 *	ch	 character to test (8 bit, ISO Latin 1)
 *
 *  RETURNS:
 *	boolean
 *
 *  EXAMPLES:
 *      if (LOWERCASE (ch)) ...
 *      if (UPPERCASE (ch)) ...
 *      if (ALPHABETIC (ch)) ...
 *
 *  DESCRIPTION:
 *	These routines test the argument to see if the value,
 *	considered as a character in the ISO Latin 1 alphabet
 *	(ASCII in GL, ISO Latin-1 supplemental in GR), is considered
 *	`lower case', `upper case' or `alphabetic'.
 *
 *	There are two characters which are defined as ALPHABETIC
 *	which are neither LOWERCASE nor UPPERCASE.  These are 
 *	0337 (Esset, German double s, `ß') and 0377 (lower case y 
 *	with diaresis, ÿ).  Although ß is really lower case, the 
 *	upper case equivalent of ß is the two characters SS and 
 *	CONVERT_TO_UPPER only returns one value.  Lower case y with 
 *	diaresis has no uppercase code in ISO Latin-1.
 *
 *	Because of these two characters, LOWERCASE really means that the
 *	argument is lower case AND that it has a single byte upper 
 *	case equivalent.  This is pretty much what VMS decided to do: 
 *	try typing badcmdß to DCL.
 *
 *	It would be easier if ISO had not put times (×) and divide (÷)
 *	in the middle of the characters.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  SEE ALSO:
 *	CONVERT_TO_LOWER, CONVERT_TO_UPPER
 *
 *  CAVEATS:
 *	See DESCRIPTION for a discussion of ß and y with diaresis.
 */

#define LOWERCASE(_c)		\
    ( IN_RANGE('a', _c, 'z') || IN_RANGE (0340, _c, 0366) \
	|| IN_RANGE (0370, _c, 0376) )

#define UPPERCASE(_c)		\
    ( IN_RANGE('A', _c, 'Z') || IN_RANGE (0300, _c, 0326) \
	|| IN_RANGE (0330, _c, 0336) )

#define ALPHABETIC(_c)		\
    ( IN_RANGE ('A', _c, 'Z') || IN_RANGE ('a', _c, 'z')	    \
	|| IN_RANGE (0300, _c, 0326) || IN_RANGE (0330, _c, 0366)   \
	|| IN_RANGE (0370, _c, 0377) )

/*+
 *  NAME: 
 *	CONVERT_TO_LOWER, CONVERT_TO_UPPER (MACROS)
 *
 *  FORMAT:
 *	int CONVERT_TO_LOWER (ch_value)
 *	int CONVERT_TO_UPPER (ch_value)
 *	    int		ch_value;
 *
 *  ARGUMENTS:
 *	ch_value	character to convert (8 bit, ISO Latin 1)
 *
 *  RETURNS:
 *	The converted character (8 bit, ISO Latin 1)
 *
 *  EXAMPLES:
 *      ch_var = CONVERT_TO_LOWER (upper_case_ch_value);
 *      ch_var = CONVERT_TO_UPPER (lower_case_ch_value);
 *
 *  DESCRIPTION:
 *	Given an ISO Latin-1 charcter of the proper case, these macros
 *	return the equivalent character in the other case.
 *
 *	The argument given MUST have the proper case.  Esset (ß) and
 *	lower case y with diaresis are not considered to be either
 *	upper case or lower case.
 *
 *  DIAGNOSTICS:
 *	None.
 *
 *  AUTHOR:
 *	Andrew F. Vesper
 *
 *  SEE ALSO:
 *	UPPERCASE, LOWERCASE, ALPHABETIC
 *
 *  CAVEATS:
 *	If applied to arbitrary characters, it will return junk.
 *	CONVERT_TO_LOWER is only defined on arguments for which UPPERCASE
 *	returns true. CONVERT_TO_UPPER is only defined on arguments for 
 *	which LOWERCASE returns true.
 */

#define CONVERT_TO_LOWER(_c)	\
    ( (_c) + 040)

#define CONVERT_TO_UPPER(_c)	\
    ( (_c) - 040)

/*
 *+
 *  NAME:
 *     INIT_QUEUE_HEAD
 *     INSERT_INTO_QUEUE_AFTER
 *     INSERT_INTO_QUEUE_BEFORE
 *     REMOVE_FROM_QUEUE
 *     NEXT_QUEUE_LINK
 *     PREVIOUS_QUEUE_LINK
 *     FIRST_QUEUE_ELEMENT
 *     LAST_QUEUE_ELEMENT
 *     NEXT_QUEUE_ELEMENT
 *     PREVIOUS_QUEUE_ELEMENT
 *     IS_QUEUE_HEAD
 *
 *  FORMAT:
 *     void INIT_QUEUE_HEAD (head);
 *     void INSERT_INTO_QUEUE_BEFORE (element_or_head, new_element);
 *     void INSERT_INTO_QUEUE_AFTER (element_or_head, new_element);
 *     void REMOVE_FROM_QUEUE (existing_element);
 *     void NEXT_QUEUE_LINK (element_or_head);
 *     void PREVIOUS_QUEUE_LINK (element_or_head);
 *     _record_ * FIRST_QUEUE_ELEMENT(head,_record_,_field_);
 *     _record_ * LAST_QUEUE_ELEMENT(head,_record_,_field_);
 *     _record_ * NEXT_QUEUE_ELEMENT (pointer, _record_, _field_);
 *     _record_ * PREVIOUS_QUEUE_ELEMENT (pointer, _record_, _field_);
 *     boolean IS_QUEUE_HEAD (head, element)
 *
 *     QUEUE_HEAD head;
 *     QUEUE_LINK new_element;
 *     QUEUE_LINK existing_element;
 *     _record_ is a typedef name or `struct' followed by a struct tag.
 *     _field_ is the name of a field in _record_
 *
 *  ARGUMENTS:
 *     head		    QUEUE_HEAD structure
 *     new_element	    new element to be inserted
 *     existing_element	    element currently in queue
 *
 *  RETURNS:
 *     None
 *
 *  EXAMPLES:
 *
 *  Given:
 *	QUEUE_HEAD list_head;
 *	typedef struct
 *	{
 *	    QUEUE_LINK	    link;
 *	    QUEUE_HEAD	    head;
 *	} TYPE;
 *
 *	TYPE *str, *el1, *elem;
 *	
 *     INIT_QUEUE_HEAD (list_head);
 *     INIT_QUEUE_HEAD (str->head);
 *     INSERT_INTO_QUEUE_AFTER (list_head, str->link);
 *     INSERT_INTO_QUEUE_BEFORE (list_head, str->link);
 *     INSERT_INTO_QUEUE_AFTER (el1->link, elem->link);
 *     REMOVE_FROM_QUEUE (elem->link);
 *     elem = NEXT_QUEUE_LINK (elem->link);
 *     elem = NEXT_QUEUE_LINK (list_head);
 *     elem = FIRST_QUEUE_ELEMENT (list_head, TYPE, link);
 *     elem = LAST_QUEUE_ELEMENT (list_head, TYPE, link);
 *     elem = NEXT_QUEUE_ELEMENT (elem, TYPE, link);
 *     elem = PREVIOUS_QUEUE_ELEMENT (elem, TYPE, link);
 *     if (IS_QUEUE_HEAD (str->head, elem->link)) break;
 *
 *  DESCRIPTION:
 *     Manipulate queues
 *
 *  DIAGNOSTICS:
 *     None
 *
 *  SEE ALSO:
 *     {@references@}
 *
 *  AUTHOR:
 *     Andrew F. Vesper
 *
 *  CAVEATS:
 *     {@description_or_none@}
 *
 *  OTHER INFORMATION:
 *     [@non-local-references@]
 *     [@pre-and-post-conditions@]
 */

#define INIT_QUEUE_HEAD(_head_)			\
    {(_head_).next = &(_head_);			\
     (_head_).previous = &(_head_);		\
    }

#define INSERT_INTO_QUEUE_AFTER(_elem_,_element_)	\
    {QUEUE_LINK * _next = (_elem_).next;	\
     (_elem_).next = &(_element_);	\
     _next->previous = &(_element_);	\
     (_element_).previous = &(_elem_);	\
     (_element_).next = _next;	    		\
    }

#define INSERT_INTO_QUEUE_BEFORE(_elem_,_element_)	\
    {QUEUE_LINK * _prev = (_elem_).previous;	\
     (_elem_).previous = &(_element_);		\
     _prev->next = &(_element_);	\
     (_element_).previous = _prev;		\
     (_element_).next = &(_elem_);		\
    }


#define REMOVE_FROM_QUEUE(_element_)		\
    {QUEUE_LINK * _next;			\
     QUEUE_LINK * _prev;			\
     _next = (_element_).next;			\
     _prev = (_element_).previous;		\
     _prev->next = _next;			\
     _next->previous = _prev;			\
    }

#define NEXT_QUEUE_LINK(_head_or_elem_)	\
    ( (_head_or_elem_)->next)

#define PREVIOUS_QUEUE_LINK(_head_or_elem_)	\
    ( (_head_or_elem_)->previous)

#define FIRST_QUEUE_ELEMENT(_header_,_record_,_field_)	\
    ( (_record_ *)				\
	( ((char *) (_header_).next)	\
	    - offsetof (_record_,_field_)	\
	)					\
    )

#define LAST_QUEUE_ELEMENT(_header_,_record_,_field_)	\
    ( (_record_ *)				\
	( ((char *) (_header_).previous)	\
	    - offsetof (_record_,_field_)	\
	)					\
    )

#define NEXT_QUEUE_ELEMENT(_pointer_,_record_,_field_)	\
    ( (_record_ *)				\
	( ((char *) (_pointer_->_field_).next) \
	    - offsetof (_record_,_field_)	\
	)					\
    )

#define PREVIOUS_QUEUE_ELEMENT(_pointer_,_record_,_field_) \
    ( (_record_ *)				\
	( ((char *) (_pointer_->_field_).previous) \
	    - offsetof (_record_,_field_)	\
	) \
    )
 
#define IS_QUEUE_HEAD(_head_, _element_)	\
    ( ((ADDRESS) &(_head_)) == (ADDRESS) (_element_) )

#endif /* PEX_MACROS_DEFINED */
/*  DEC/CMS REPLACEMENT HISTORY, Element PEX_MACROS.H */
/*  *16    4-MAR-1991 10:57:03 VESPER "Change COPY_STATIC_SIZE_AREA to work on Ultrix compilers" */
/*  *15   15-OCT-1990 11:23:39 VESPER "Added COPY_STATIC_SIZE_AREA and COPY_XFORM" */
/*  *14   19-JAN-1990 10:41:30 VESPER "Work around a compiler bug" */
/*  *13   18-OCT-1989 10:35:48 VESPER "Updated table of contents and fixed INSERT_INTO_QUEUE_BEFORE" */
/*  *12   22-AUG-1989 16:25:37 VESPER "USE_BUILTINS now works" */
/*  *11   14-JUL-1989 11:23:37 VESPER "Added offsetof for pmax, fixed builtins" */
/*  *10    3-MAY-1989 08:11:24 VESPER "Fix problem with ffs declaration" */
/*  *9    11-APR-1989 10:12:56 VESPER "Move #pragma bultins to use_builtins.h" */
/*  *8    12-FEB-1989 11:25:09 FREITAS "add macros to find the size of vertices, opt_data, etc." */
/*  *7    14-DEC-1988 13:39:48 VESPER "fixed built-ins" */
/*  *6     9-DEC-1988 16:33:53 VESPER "added new macros: GET_NEXT_MASK_BIT and GET_NEXT_MASK_BIT_INDEX" */
/*  *5    30-NOV-1988 15:28:39 VESPER "compiles, development continues" */
/*  *4    30-NOV-1988 10:48:42 NISHIMOTO "add Success and Failure codes" */
/*  *3    18-NOV-1988 13:39:24 VESPER "added COPY_AREA and ZERO_AREA macros" */
/*  *2     3-OCT-1988 12:48:43 VESPER "pexlib now compiles" */
/*  *1     8-AUG-1988 14:02:18 VESPER "Standard Macros for ALL PEX modules" */
/*  DEC/CMS REPLACEMENT HISTORY, Element PEX_MACROS.H */
