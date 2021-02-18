/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  WOOL_OBJECT  String  *
*  BODY		        *
* 		        *
\***********************/

#include "EXTERN.h"
#include <stdio.h>
#include "wool.h"
#include "wl_number.h"
#include "wl_atom.h"
#include "wl_active.h"
#include "wl_pointer.h"
#include "wl_name.h"
#include "wl_list.h"
#include "INTERN.h"
#include "wl_string.h"

char * re_comp();

/*
 * Constructor:
 * WLString_make
 * argument 1: the string, which will be COPIED.
 */

WOOL_String 
WLString_make(s)
char           *s;		/* the string itself */
{
    WOOL_String     object;

    if (s[0] == '\0')
	return NIL_STRING;
    object = (WOOL_String)
	Malloc(sizeof(struct _WOOL_String) + strlen(s));
    zrt_put(object);
    object -> type = WLString;
    strcpy(object -> string, s);
    return object;
}

WOOL_String 
WLString_n_make(n)
int	n;
{
    WOOL_String     object = (WOOL_String)
    Malloc(sizeof(struct _WOOL_String) + n);

    zrt_put(object);
    object -> type = WLString;
    return object;
}

NIL_STRING_make()
{
    NIL_STRING = (WOOL_String) Malloc(sizeof(struct _WOOL_String));
    zrt_put(NIL_STRING);
    NIL_STRING -> type = WLString;
    NIL_STRING -> string[0] = '\0';
    increase_reference(NIL_STRING);
}

/*
 * WLString_print:
 * We print strings surrounded by double quotes.
 */

WOOL_OBJECT 
WLString_print(obj)
WOOL_String     obj;
{
    wool_puts(obj -> string);
    return (WOOL_OBJECT) obj;
}

/*
 * WLString_free
 * just frees the structure, so we use WLNumber_free.
 */

/* 
 * WLString_execute
 * just the same error as with atoms, let's use it.
 */

/*
 * WLString_equal
 * tests 2 strings for equality (returns it if true)
 */

WOOL_OBJECT
WLString_equal(s1, s2)
WOOL_String s1, s2;
{
    if (!is_a_string(s2) || strcmp(s1 -> string, s2 -> string))
	return NIL;
    else
	return (WOOL_OBJECT) s1;
}

/*
 * string manipulation routines
 */

static char *strings_temp_buffer;
static int strings_temp_buffer_size = 256;

WOOL_OBJECT
add_strings(argc,argv)
int             argc;
WOOL_String     argv[];
{
    int             required_length = 0, i;

    for (i = 0; i < argc; i++) {
	must_be_string(argv[i], i);
	if (argv[i] != (WOOL_String) NIL)
	    required_length += strlen(argv[i] -> string);
    }
    if (!strings_temp_buffer)
	strings_temp_buffer = (char *) Malloc(strings_temp_buffer_size);
    if (required_length >= strings_temp_buffer_size) {
	strings_temp_buffer_size = Max(2 * strings_temp_buffer_size,
				       required_length+1);
	strings_temp_buffer = (char *)
	    Realloc(strings_temp_buffer, strings_temp_buffer_size);
    }
    strings_temp_buffer[0] = '\0';
    for (i = 0; i < argc; i++)
	if (argv[i] != (WOOL_String) NIL)
	    strcat(strings_temp_buffer, argv[i] -> string);
    return (WOOL_OBJECT) WLString_make(strings_temp_buffer);
}

/*
 * To know if an object can be used as a string (atom, pointer or active)
 */

int 
must_be_string(obj, n)
WOOL_OBJECT	obj;
int		n;
{
    if (obj -> type != WLString
	&& obj -> type != WLAtom
	&& obj -> type != WLActive
	&& obj -> type != WLPointer
	&& obj -> type != WLName
	&& obj != NIL)
	bad_argument(obj, n, WOOL_TYPE_P_NAME(WLString));
}

int 
is_a_string(obj)
WOOL_OBJECT	obj;
{
    if (obj -> type != WLString
        && obj -> type != WLAtom
        && obj -> type != WLActive
        && obj -> type != WLPointer
        && obj -> type != WLName
        && obj != NIL)
 	return 0;
    else
	return 1;
 }


/**************************************************************************\
* 									   *
* the general  match package						   *
* (match regular-expression string [level])				   *
* returns the sub-string in the levelth enclosing \( and \) or NIL_STRING  *
* or string or NIL if no level given					   *
* 									   *
\**************************************************************************/

WOOL_String
WLString_match(argc, argv)
int	argc;
WOOL_String	argv[];
{
    int             result, i;
    char           *subst, *s, *comp_error;
    WOOL_String     wl_subst;
    WOOL_List	    wl_list;
    
    if (argc < 2)
      wool_error(BAD_NUMBER_OF_ARGS, argc);
    must_be_string(argv[0], 0);
    must_be_string(argv[1], 1);
    if ((comp_error = re_comp(argv[0] -> string)) ||
	((result = re_exec(argv[1] -> string)) == -1)) {
	if (comp_error)
	  wool_printf("%\n", comp_error);
	wool_error("match: Bad regular expression, %s", argv[0] -> string);
    }
    if (result) {
        switch (argc) {
	 case 2:
	    return argv[1];
	 case 3:
	    must_be_number(argv[2], 2);
	    if (result =
		re_subst(((WOOL_Number) argv[2]) -> number, &subst)) {
		wl_subst = WLString_n_make(result + 1);
		strncpy(wl_subst -> string, subst, result);
		wl_subst -> string[result] = '\0';
		return wl_subst;
	    } else {
		return NIL_STRING;
	    }
	 default:
	    wl_list = wool_list_make(argc - 2);
	    bzero(wl_list -> list, wl_list -> size * sizeof (WOOL_OBJECT));
	    for (i = 2; i< argc; i++) {
		must_be_number(argv[i], i);
		if (result =
		    re_subst(((WOOL_Number) argv[i]) -> number, &subst)) {
		    wl_subst = WLString_n_make(result + 1);
		    strncpy(wl_subst -> string, subst, result);
		    wl_subst -> string[result] = '\0';
		    increase_reference(wl_list -> list[i - 2] =
				       (WOOL_OBJECT) wl_subst);
		} else {
		    increase_reference(wl_list -> list[i - 2] =
				       (WOOL_OBJECT) NIL_STRING);
		}
	    }
	    return (WOOL_String) wl_list;
        }
    } else {
	return (argc == 2 ? (WOOL_String) NIL : NIL_STRING);
    }
}

/*
 * routines to do regular expression matching
 *
 * Entry points:
 *
 *	re_comp(s)
 *		char *s;
 *	 ... returns 0 if the string s was compiled successfully,
 *		     a pointer to an error message otherwise.
 *	     If passed 0 or a null string returns without changing
 *           the currently compiled re (see note 11 below).
 *
 *	re_exec(s)
 *		char *s;
 *	 ... returns 1 if the string s matches the last compiled regular
 *		       expression, 
 *		     0 if the string s failed to match the last compiled
 *		       regular expression, and
 *		    -1 if the compiled regular expression was invalid 
 *		       (indicating an internal error).
 *
 *	re_subst(n, &p)
 *		int n;
 *		char *p;
 *	 ... returns in p the string matching the nth \(, returns the
 *		    number of chars matching
 *
 * The strings passed to both re_comp and re_exec may have trailing or
 * embedded newline characters; they are terminated by nulls.
 *
 * The regular expressions recognized are described below. This description
 * is essentially the same as that for ed.
 *
 *	A regular expression specifies a set of strings of characters.
 *	A member of this set of strings is said to be matched by
 *	the regular expression.  In the following specification for
 *	regular expressions the word `character' means any character but NUL.
 *
 *	1.  Any character except a special character matches itself.
 *	    Special characters are the regular expression delimiter plus
 *	    \ [ . and sometimes ^ * $.
 *	2.  A . matches any character.
 *	3.  A \ followed by any character except a digit or ( )
 *	    matches that character.
 *	4.  A nonempty string s bracketed [s] (or [^s]) matches any
 *	    character in (or not in) s. In s, \ has no special meaning,
 *	    and ] may only appear as the first letter. A substring 
 *	    a-b, with a and b in ascending ASCII order, stands for
 *	    the inclusive range of ASCII characters.
 *	5.  A regular expression of form 1-4 followed by * matches a
 *	    sequence of 0 or more matches of the regular expression.
 *	6.  A regular expression, x, of form 1-8, bracketed \(x\)
 *	    matches what x matches.
 *	7.  A \ followed by a digit n matches a copy of the string that the
 *	    bracketed regular expression beginning with the nth \( matched.
 *	8.  A regular expression of form 1-8, x, followed by a regular
 *	    expression of form 1-7, y matches a match for x followed by
 *	    a match for y, with the x match being as long as possible
 *	    while still permitting a y match.
 *	9.  A regular expression of form 1-8 preceded by ^ (or followed
 *	    by $), is constrained to matches that begin at the left
 *	    (or end at the right) end of a line.
 *	10. A regular expression of form 1-9 picks out the longest among
 *	    the leftmost matches in a line.
 *	11. An empty regular expression stands for a copy of the last
 *	    regular expression encountered.
 */

/*
 * constants for re's
 */
#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL	6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	18

#define	CSTAR	01

#define	ESIZE	512
#define	NBRA	9
#define	comerr(msg) {expbuf[0] = 0; numbra = 0; return(msg); }

static	char	expbuf[ESIZE], *braslist[NBRA], *braelist[NBRA];
static	char	circf;
static 	int	numbra;
static  int	advance();

/*
 * compile the regular expression argument into a dfa
 */

char *
re_comp(sp)
char	*sp;
{
    int    c;
    char  *ep = expbuf;
    int             cclcnt;
    char           *lastep = 0;
    char            bracket[NBRA];
    char           *bracketp = &bracket[0];
    static char    *retoolong = "Regular expression too long";

    numbra = 0;
    if (sp == 0 || *sp == '\0') {
	if (*ep == 0)
	    return ("No previous regular expression");
	return (0);
    }
    if (*sp == '^') {
	circf = 1;
	sp++;
    } else
	circf = 0;
    for (;;) {
	if (ep >= &expbuf[ESIZE])
	    comerr(retoolong);
	if ((c = *sp++) == '\0') {
	    if (bracketp != bracket)
		comerr("unmatched \\(");
	    *ep++ = CEOF;
	    *ep++ = 0;
	    return (0);
	}
	if (c != '*')
	    lastep = ep;
	switch (c) {

	case '.':
	    *ep++ = CDOT;
	    continue;

	case '*':
	    if (lastep == 0 || *lastep == CBRA || *lastep == CKET)
		goto defchar;
	    *lastep |= CSTAR;
	    continue;

	case '$':
	    if (*sp != '\0')
		goto defchar;
	    *ep++ = CDOL;
	    continue;

	case '[':
	    *ep++ = CCL;
	    *ep++ = 0;
	    cclcnt = 1;
	    if ((c = *sp++) == '^') {
		c = *sp++;
		ep[-2] = NCCL;
	    }
	    do {
		if (c == '\0')
		    comerr("missing ]");
		if (c == '-' && ep[-1] != 0) {
		    if ((c = *sp++) == ']') {
			*ep++ = '-';
			cclcnt++;
			break;
		    }
		    while (ep[-1] < c) {
			*ep = ep[-1] + 1;
			ep++;
			cclcnt++;
			if (ep >= &expbuf[ESIZE])
			    comerr(retoolong);
		    }
		}
		*ep++ = c;
		cclcnt++;
		if (ep >= &expbuf[ESIZE])
		    comerr(retoolong);
	    } while ((c = *sp++) != ']');
	    lastep[1] = cclcnt;
	    continue;

	case '\\':
	    if ((c = *sp++) == '(') {
		if (numbra >= NBRA)
		    comerr("too many \\(\\) pairs");
		*bracketp++ = numbra;
		*ep++ = CBRA;
		*ep++ = numbra++;
		continue;
	    }
	    if (c == ')') {
		if (bracketp <= bracket)
		    comerr("unmatched \\)");
		*ep++ = CKET;
		*ep++ = *--bracketp;
		continue;
	    }
	    if (c >= '1' && c < ('1' + NBRA)) {
		*ep++ = CBACK;
		*ep++ = c - '1';
		continue;
	    }
	    *ep++ = CCHR;
	    *ep++ = c;
	    continue;

    defchar:
	default:
	    *ep++ = CCHR;
	    *ep++ = c;
	}
    }
}

/* 
 * match the argument string against the compiled re
 */

int
re_exec(p1)
char	*p1;
{
    char  *p2 = expbuf;
    int    c;
    int             rv;

    for (c = 0; c < NBRA; c++) {
	braslist[c] = 0;
	braelist[c] = 0;
    }
    if (circf)
	return ((advance(p1, p2)));

    /*
     * fast check for first character 
     */
    if (*p2 == CCHR) {
	c = p2[1];
	do {
	    if (*p1 != c)
		continue;
	    if (rv = advance(p1, p2))
		return (rv);
	} while (*p1++);
	return (0);
    }

    /*
     * regular algorithm 
     */
    do
	if (rv = advance(p1, p2))
	    return (rv);
    while (*p1++);
    return (0);
}

/* 
 * try to match the next thing in the dfa
 */

static	int
advance(lp, ep)
char	*lp, *ep;
{
    char  *curlp;
    int             ct, i;
    int             rv;

    for (;;)
	switch (*ep++) {

	case CCHR:
	    if (*ep++ == *lp++)
		continue;
	    return (0);

	case CDOT:
	    if (*lp++)
		continue;
	    return (0);

	case CDOL:
	    if (*lp == '\0')
		continue;
	    return (0);

	case CEOF:
	    return (1);

	case CCL:
	    if (cclass(ep, *lp++, 1)) {
		ep += *ep;
		continue;
	    }
	    return (0);

	case NCCL:
	    if (cclass(ep, *lp++, 0)) {
		ep += *ep;
		continue;
	    }
	    return (0);

	case CBRA:
	    braslist[*ep++] = lp;
	    continue;

	case CKET:
	    braelist[*ep++] = lp;
	    continue;

	case CBACK:
	    if (braelist[i = *ep++] == 0)
		return (-1);
	    if (backref(i, lp)) {
		lp += braelist[i] - braslist[i];
		continue;
	    }
	    return (0);

	case CBACK | CSTAR:
	    if (braelist[i = *ep++] == 0)
		return (-1);
	    curlp = lp;
	    ct = braelist[i] - braslist[i];
	    while (backref(i, lp))
		lp += ct;
	    while (lp >= curlp) {
		if (rv = advance(lp, ep))
		    return (rv);
		lp -= ct;
	    }
	    continue;

	case CDOT | CSTAR:
	    curlp = lp;
	    while (*lp++);
	    goto star;

	case CCHR | CSTAR:
	    curlp = lp;
	    while (*lp++ == *ep);
	    ep++;
	    goto star;

	case CCL | CSTAR:
	case NCCL | CSTAR:
	    curlp = lp;
	    while (cclass(ep, *lp++, ep[-1] == (CCL | CSTAR)));
	    ep += *ep;
	    goto star;

    star:
	    do {
		lp--;
		if (rv = advance(lp, ep))
		    return (rv);
	    } while (lp > curlp);
	    return (0);

	default:
	    return (-1);
	}
}

backref(i, lp)
int	i;
char	*lp;
{
    char  *bp;

    bp = braslist[i];
    while (*bp++ == *lp++)
	if (bp >= braelist[i])
	    return (1);
    return (0);
}

int
cclass(set, c, af)
char	*set, c;
int	af;
{
    int    n;

    if (c == 0)
	return (0);
    n = *set++;
    while (--n)
	if (*set++ == c)
	    return (af);
    return (!af);
}

int
re_subst(n, pp)
int 	n;
char	**pp;
{
    int length;

    if(n > numbra) {
	return (int) wool_error("match: Too many \"(\" asked (%d) ", n);
    }
    *pp = braslist[n-1];
    length = braelist[n-1] - *pp;
    return length;
}
