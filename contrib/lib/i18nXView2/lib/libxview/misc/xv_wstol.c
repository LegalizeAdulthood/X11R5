#ifndef lint
#ifdef sccs
static  char sccsid[] = "@(#)xv_wstol.c 1.1 90/09/13; SMI";
#endif
#endif

/*
 * This is bug (memory trash) fixed version of JLE1.0.3 wstol, and
 * renamed to suitable for libxview internal use.
 */

/*
 * Copyright (c) 1988 by Sun Microsystems, Inc.
 * Copyright (c) 1988 by Nihon Sun Microsystems K.K.
 */

/*LINTLIBRARY*/
#include <euc.h>
#include <widec.h>
#include <ctype.h>
#include <wctype.h>
#define DIGIT(x)	(isdigit(x) ? (x) - '0' : \
			islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')
#define MBASE	('z' - 'a' + 1 + 10)

long
_xv_wstol(str, ptr, base)
register wchar_t *str;
wchar_t **ptr;
register int base;
{
	register long val;
	register int c;
	int xx, neg = 0;
	char	*euc_str, *m_pt;
	int	n;

	if (ptr != (wchar_t **)0)
		*ptr = str; /* in case no number is formed */
	if (base < 0 || base > MBASE) {
		return (0); /* base is invalid -- should be a fatal error */
	}

	if ( ! (euc_str = (char *)malloc((wslen(str) * sizeof(wchar_t)) + 1)))
		return 0;
	m_pt = euc_str;

	wstostr(euc_str,str);

	if (!isalnum(c = *euc_str)) {
		while (isspace(c)) {
			c = *++euc_str;
			++str;
		}
		switch (c) {
		case '-':
			neg++;
		case '+': /* fall-through */
			c = *++euc_str;
			++str;
		}
	}
	if (base == 0)
		if (c != '0')
			base = 10;
		else if (euc_str[1] == 'x' || euc_str[1] == 'X')
			base = 16;
		else
			base = 8;
	/*
	 * for any base > 10, the digits incrementally following
	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 */
	if (!isalnum(c) || (xx = DIGIT(c)) >= base) {
		free(m_pt);
		return (0); /* no number formed */
	}
	if (base == 16 && c == '0' && isxdigit(euc_str[2]) &&
	    (euc_str[1] == 'x' || euc_str[1] == 'X')) {
		c = *(euc_str += 2); /* skip over leading "0x" or "0X" */
		str += 2;
	}
	for (val = -DIGIT(c); isalnum(c = *++euc_str) && (xx = DIGIT(c)) < base; ) {
		/* accumulate neg avoids surprises near MAXLONG */
		val = base * val - xx;
		++str;
	}
	++str;
	if (ptr != (wchar_t **)0)
		*ptr = str;
	free(m_pt);
	return (neg ? val : -val);
}
