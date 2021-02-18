/*
 * $XConsortium: wcstol.c,v 1.4 91/07/01 11:02:27 xguest Exp $
 */

/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by the Massachusetts Institute of Technology
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the names of OMRON and M.I.T.  not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. OMRON and M.I.T. 
 * make no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * OMRON AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL OMRON OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author: Noritoshi Demizu	OMRON Corporation
 *
 */

#include "wcharint.h"

#define	NULL	0

/*
 * Here's private function declarations
 */
#if NeedFunctionPrototypes
static int checkbase(wchar_t**,int);	/* checks base			*/
static int basevalue(wchar_t*);		/* returns the base expected	*/
static int wc2i(wchar_t);		/* convert wchar to integer	*/
#else
static int checkbase();	/* checkbase(ptr,base) -- checks base		*/
static int basevalue();	/* basevalue(wcs) -- returns the base expected	*/
static int wc2i();	/* wc2i(wc) -- convert wchar to integer value	*/
#endif


/* wcstol(wcs, ptr, base) -- convert wstring to integer
 *   IN:
 *	wcs:	string to be scanned as number
 *	base:	base of the number
 *   OUT:
 *	ptr:	next character of the number scanned, if ptr is not null.
 *   RETURN VALUE:
 *		value of the number
 *   ON ERROR:
 *		return value is 0 and *ptr==wcs.
 */
#if NeedFunctionPrototypes
long wcstol(wchar_t *wcs, wchar_t **ptr, int base)
#else
long wcstol(wcs, ptr, base)
 wchar_t	*wcs;
 wchar_t	**ptr;
 int		base;
#endif
{
    wchar_t *cur;	/* pointer where we are looking at	*/
    wchar_t *start;	/* starting point of the number		*/
    long value;		/* value of wchar string		*/
    int  sign;		/* positive value or negative value	*/
    int  temp;		/* value of current wchar		*/

    /* first of all, looking at first character of wcs */
    cur = wcs;

    /* skipping spaces */
    while (_iswspace(*cur)) cur++;

    /* checking base */
    base = checkbase(&cur, base);
    if (base == 0) {
	/* invalid base value */
	if (ptr != NULL) *ptr = wcs;
	return 0;
    }

    /* get positive value or negative value */
    if (*cur == _atowc('-')) {
	sign = -1;
	cur ++;
    } else {
	sign = 1;
    }

    start = cur;
    value = 0;
    for (; *cur != WNULL; cur++) {
	temp = wc2i(*cur);
	if ( temp < 0 || temp >= base ) break;
	value = value * base + temp;
    }
    if (start == cur) {
	/* there's no number */
	if (ptr != NULL) *ptr = wcs;
	return 0;
    }
    if (ptr != NULL) *ptr = cur;
    return (sign>0) ? value : -value;
}


/* checkbase(ptr,base) -- checks whether base is valid or not.
 *   IN:
 *	ptr:	pointer to string to be scanned.
 *	base:	copy of the argument of wcstol()
 *   OUT:
 *	ptr:	next character where scanning should be started at
 *   RETURN VALUE:
 *	0:	invalid base or string
 *	other:	valid base value ( 0 < ret <= 36 )
 */
static int
#if NeedFunctionPrototypes
 checkbase(wchar_t** ptr,int base)
#else
 checkbase(ptr, base)
 wchar_t** ptr;
 int	base;
#endif
{
    register wchar_t *cur;	/* pointer where we are looking at	*/

    cur = *ptr;
    switch (base) {
     case 16:
	/* leading "0x" should be skipped */
	if (basevalue(cur)==16) *ptr = &cur[2];	/* skipping leading "0x" */
	return base;
     case 0:
	/* base should be computed from wcs */
	base = basevalue(cur);
	if (base < 0) return 0;		/* the str is not a number */
	if (base == 16) *ptr = &cur[2];	/* skipping leading "0x" */
	return base;
     default:
	/* check the range of base */
	if (base < 0 || base > 36) return 0;	/* invalid base range */
	return base;
    }
}

/* basevalue(wcs) -- returns the base expected
 *   IN:
 *	wcs:	wchar string to be scanned
 *   RETURN VALUE:
 *	16: starting with "0x" or "0X" followed by hex-digit
 *	 8: starting with "0"
 *	10: starting with "1"-"9" and "-"
 */
static int
#if NeedFunctionPrototypes
 basevalue(register wchar_t *wcs)
#else
 basevalue(wcs)
 register wchar_t *wcs;
#endif
{
    /* first character should be digit or "-" */
    if (_iswdigit(wcs[0])) {
	if (wcs[0] == _atowc('0')) {
	    if ((wcs[1] == _atowc('x') || wcs[1] == _atowc('X'))
		&& _iswxdigit(wcs[2])) {
		return 16;	/* "0xH" or "0XH" (hexdecimal) */
	    } else {
		return 8;	/* "0" or "0D" (octal) */
	    }
	} else {
	    return 10;		/* "1" - "9" (decimal) */
	}
    } else if (wcs[0] == _atowc('-')) {
	return 10;		/* "-" (perhaps decimal) */
    } else {
	/* The wchar string is not a number */
	return -1;
    }
}

/* wc2i(wc) -- convert wchar to integer value
 *   IN:
 *	wc:	wchar to be computed
 *   RETURN VALUE:
 *	-1:	not a digit
 *	other:	integer value
 */
static int
#if NeedFunctionPrototypes
 wc2i(register wchar_t wc)
#else
 wc2i(wc)
 register wchar_t	wc;
#endif
{
    if ( _iswdigit(wc) ) return (_wctoa(wc) - '0');
    if ( _iswupper(wc) ) return (_wctoa(wc) - 'A' + 10 );
    if ( _iswlower(wc) ) return (_wctoa(wc) - 'a' + 10 );
    return -1;
}
