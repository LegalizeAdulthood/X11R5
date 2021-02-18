/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXFrac.c,v 2.0 1991/07/23 02:05:35 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include "Xlibint.h"
#include "MVEXlibint.h"
#include "MVEXlib.h"
#include <stdio.h>

/*
 *    NAME
 *        MVEXMatchFraction - see if a fraction is within a list of ranges
 *
 *    SYNOPSIS
 */
Bool
MVEXMatchFraction (given, ranges, nranges, lower, higher)
    MVEXFraction *given;        /* in: the fraction to compare */
    MVEXFractionRange *ranges;  /* in: the range list to compare with */
    int nranges;                /* in: the number of ranges in the list */
    MVEXFraction *lower;        /* out: the closest lower fraction */
    MVEXFraction *higher;       /* out: the closest higher fraction */
/*
 *    DESCRIPTION
 *        For all the ranges, try to find an exact match and as you go test
 *        to see if the lower and higher fractions should be updated.
 *
 *    RETURNS
 *        True if the given fraction is exactly within one of the ranges,
 *        False otherwise
 */
{
    MVEXFractionRange *range;
    long min_num, max_num, min_den, max_den, try_num0, try_num1, try_den;
    long try_diff;
    register int j0, j1;
    Bool exact_num, exact_den;

/* Initialize at short limits */
    lower->numerator = -32768;
    lower->denominator = 1;
    higher->numerator = 32767;
    higher->denominator = 1;

/* Check all the ranges */
    for (j0 = 0; j0 < nranges; j0++) {
        range = &ranges[j0];
/* Find minimum and maximum numerators and denominators */
/* Check for exact fit too */
        exact_num = False;
/* Linear range numerator */
        if (range->num_type == MVEXLinearRange) {
            min_num = range->num_base;
            max_num = range->num_limit;
            if (given->numerator >= min_num && given->numerator <= max_num &&
                (given->numerator - min_num) % range->num_increment == 0) {
                exact_num = True;
            }
        }
/* Geometric range numerator */
        else {
            min_num = 1;
            for (j1 = 0; j1 < range->num_increment; j1++) {
                min_num *= range->num_base;
            }
            if (given->numerator == min_num) {
                exact_num = True;
            }
            max_num = min_num;
            while (j1 < range->num_limit) {
                max_num *= range->num_base;
                if (given->numerator == max_num) {
                    exact_num = True;
                }
                j1++;
            }
        }
/* Linear range denominator */
        exact_den = False;
        if (range->denom_type == MVEXLinearRange) {
            min_den = range->denom_base;
            max_den = range->denom_limit;
            if (given->denominator >= min_den && given->denominator <= max_den
                && (given->denominator - min_den) % range->denom_increment ==
                0) {
                exact_den = True;
            }
        }
/* Geometric range denominator */
        else {
            min_den = 1;
            for (j1 = 0; j1 < range->denom_increment; j1++) {
                min_den *= range->denom_base;
            }
            if (given->denominator == min_den) {
                exact_den = True;
            }
            max_den = min_den;
            while (j1 < range->denom_limit) {
                max_den *= range->denom_base;
                if (given->denominator == max_den) {
                    exact_den = True;
                }
                j1++;
            }
        }

/* Return the given fraction if it is an exact fit */
        if (exact_num && exact_den) {
            lower->numerator = given->numerator;
            lower->denominator = given->denominator;
            higher->numerator = given->numerator;
            higher->denominator = given->denominator;
            return True;
        }

/* Is bottom of range above current higher fraction? */
        if (min_num >= 0) {
            if (min_num * higher->denominator > max_den * higher->numerator) {
                continue;
            }
        } else {
            if (min_num * higher->denominator > min_den * higher->numerator) {
                continue;
            }
        }

/* Is top of range below current lower fraction? */
        if (max_num >= 0) {
            if (max_num * lower->denominator < min_den * lower->numerator) {
                continue;
            }
        } else {
            if (max_num * lower->denominator < max_den * lower->numerator) {
                continue;
            }
        }

/* Look at each denominator */
        for (try_den = min_den; try_den <= max_den;
            try_den = (range->denom_type == MVEXLinearRange) ?
            try_den + range->denom_increment :
            try_den * range->denom_base) {
/* If the trial denominator is too small, check the higher */
            if (min_num * given->denominator > given->numerator * try_den) {
                if (min_num * higher->denominator <
                    higher->numerator * try_den) {
                    higher->numerator = min_num;
                    higher->denominator = try_den;
                }
                if (min_num >= 0) {
                    continue;
                } else {
                    break;
                }
            }
/* If the trial denominator is too big, check the lower */
            if (max_num * given->denominator < given->numerator * try_den) {
                if (max_num * lower->denominator >
                    lower->numerator * try_den) {
                    lower->numerator = max_num;
                    lower->denominator = try_den;
                }
                if (max_num >= 0) {
                    break;
                } else {
                    continue;
                }
            }
/* Try the two numerators around the given fraction */
            if (range->num_type == MVEXLinearRange) {
                try_num0 = ((given->numerator * try_den / given->denominator) -
                    range->num_base) / range->num_increment *
                    range->num_increment + range->num_base;
                if (try_num0 * given->denominator >
                    given->numerator * try_den) {
                    try_num0 -= range->num_increment;
                }
                try_num1 = try_num0 + range->num_increment;
            } else {
                try_num0 = 1;
                while (try_num0 * given->denominator < try_den *
                    given->numerator) try_num0 *= range->num_base;
                try_num1 = try_num0;
                try_num0 /= range->num_base;
            }
/* Exit loop if both trial fractions not in range */
            if (try_num0 > max_num || try_num1 < min_num) continue;

/* Check the lesser trial fraction */
            try_diff = try_num0 * given->denominator - given->numerator
                * try_den;
/* Return numerically equivalent fraction */
            if (try_diff == 0) {
                lower->numerator = try_num0;
                lower->denominator = try_den;
                higher->numerator = try_num0;
                higher->denominator = try_den;
                return True;
            }
/* See if the lower fraction can be moved up */
            if (lower->numerator * try_den < try_num0 * lower->denominator) {
                lower->numerator = try_num0;
                lower->denominator = try_den;
            }
/* Check the greater trial fraction */
            try_diff = try_num1 * given->denominator - given->numerator
                * try_den;
/* Return numerically equivalent fraction */
            if (try_diff == 0) {
                lower->numerator = try_num1;
                lower->denominator = try_den;
                higher->numerator = try_num1;
                higher->denominator = try_den;
                return True;
            }
/* See if the higher fraction can be moved up */
            if (higher->numerator * try_den >
                try_num1 * higher->denominator) {
                higher->numerator = try_num1;
                higher->denominator = try_den;
            }
        }
    }

/* Change the initial values if they are still set */
    if (lower->numerator == -32768 && lower->denominator == 1) {
        lower->numerator = higher->numerator;
        lower->denominator = higher->denominator;
    } else if (higher->numerator == 32767 && higher->denominator == 1) {
        higher->numerator = lower->numerator;
        higher->denominator = lower->denominator;
    }

/* Return the "squeezed" closest lower and higher value fractions */
    return False;
}

/*
 *    NAME
 *        MVEXFractionCount - enumerate the fractions in a list of ranges
 *
 *    SYNOPSIS
 */
int
MVEXFractionCount(ranges, nranges)
    MVEXFractionRange *ranges;
    int nranges;
/*
 *    DESCRIPTION
 *        Count the fractions
 *
 *    RETURNS
 *        the number of fractions
 */
{
    int num_cnt, denom_cnt, i, total, incToZero;
    Bool zeroCounted = False, zero = False;

    total = 0;
    for (i = 0; i < nranges; i++, ranges++) {
	if (ranges->num_type == MVEXLinearRange) {
	    num_cnt = 1 + (ranges->num_limit - ranges->num_base)
			/ ranges->num_increment;
	    /*
	     * eliminate all but one zero numerator.
	     */
	    if ((ranges->num_base < 0 && ranges->num_limit < 0)
	     || (ranges->num_base > 0 && ranges->num_limit > 0))
		; /* doesn't cross zero */
	    else {
		/*
		 * If there is a zero, reduce the numerator count
		 * by one, and perhaps add it in later.
		 */
		incToZero = - (ranges->num_base / ranges->num_increment);
		if (ranges->num_base + (ranges->num_increment * incToZero) == 0) {
		    num_cnt = (ranges->num_limit - ranges->num_base)
				/ ranges->num_increment;
		    zero = True;
		}
	    }
	} else {
	    num_cnt = ranges->num_limit - ranges->num_base + 1;
	}

	if (ranges->denom_type == MVEXLinearRange)
	    denom_cnt = 1 + (ranges->denom_limit - ranges->denom_base)
			/ ranges->denom_increment;
	else
	    denom_cnt = ranges->denom_limit - ranges->denom_base + 1;

	total += num_cnt * denom_cnt;
	if (zero && !zeroCounted) {
	    zeroCounted = True;
	    zero = False;
	    total++;
	}
    }
    return (total);
}

static int FractionEqual(frac1, frac2)
    MVEXFraction *frac1, *frac2;
{
    double val1, val2;

    val1 = (double) frac1->numerator / (double) frac1->denominator;
    val2 = (double) frac2->numerator / (double) frac2->denominator;
    if (val1 < val2)
	return (-1);
    else if (val1 > val2)
	return (1);
    return (0);
}

/*
 *    NAME
 *        MVEXMatchFractionInList - is a fraction in a list of fractions?
 *
 *    SYNOPSIS
 */
Bool
MVEXMatchFractionInList(fraction, list, count, lower, higher, index_return)
    MVEXFraction *fraction;     /* in: the fraction to find */
    MVEXFraction *list;         /* in: the list to search */
    int count;                  /* in: length of list */
    MVEXFraction *lower;	/* out: nearest lower fraction */
    MVEXFraction *higher;	/* out: nearest higher fraction */
    int *index_return;          /* out: index of lower */
/*
 *    DESCRIPTION
 *        Find a fraction in a list of fractions, or determine if it is
 *        below, inside or above the list.
 *        If exact match (numerically equivalent), index_return is the index
 *        of one of the fractions in the list that matches and lower and higher
 *        have the same value.
 *        If the fraction does not match but is inside the list, index_return
 *        is the index of lower, and lower and higher have different values.
 *        If the fraction is below the list, index_return is 0 and lower and
 *        higher are both the lowest value in the list.
 *        If the fraction is above the list, index_return is count - 1 and
 *        lower and higher are both the highest value in the list.
 *
 *    RETURNS
 *	  True if the fraction is an exact match.
 *	  False otherwise.
 */
{
    int status, i;
    /*
     * XXX this should be a binary search...  we lied!
     */
    for (i = 0; i < count; i++) {
        status = FractionEqual(fraction, list + i);
	if (status == 0) {
	    *lower = *higher = list[i];
	    *index_return = i;
	    return (True);
	}

	if (status < 0) { /* we just passed our mark */
	    if (i == 0) {
		*lower = list[0];
		*higher = list[0];
	    } else {
		*lower = list[--i];
		*higher = list[i + 1];
	    }
	    *index_return = i;
	    return (False);
	}
    }

    /* The fraction is above the list */
    *lower = list[--i];
    *higher = list[i];
    *index_return = i;
    return (False);
}

static int power(n, exp)
    int n, exp;
{
    int value, i;

    for (value = 1, i = 0; i < exp; i++)
	value *= n;
    return (value);
}

#define FirstNumber(range, which)				\
	(which = ((range)->which/**/_type == MVEXLinearRange) ?	\
	      (range)->which/**/_base				\
	    : power((range)->which/**/_base,			\
		    which/**/_exp = (range)->which/**/_increment))

#define LastNumber(range, which)				\
	(((range)->which/**/_type == MVEXLinearRange) ?		\
	      (which == (range)->which/**/_limit)			\
	    : (which/**/_exp == (range)->which/**/_limit))

#define NextNumber(range, which)				\
	(which = ((range)->which/**/_type == MVEXLinearRange) ?	\
	      which + (range)->which/**/_increment			\
	    : power((range)->which/**/_base, which/**/_exp++))

/*
 *    NAME
 *        MVEXFractionList - convert a list of ranges to a list of fractions
 *
 *    SYNOPSIS
 */
MVEXFraction *
MVEXFractionList(range, nranges, length_return)
    MVEXFractionRange *range;
    int nranges;
    int *length_return;
/*
 *    DESCRIPTION
 *        Generate a list of fractions (with only one zero entry, if any)
 *	  that corresponds to the specified set of ranges.  The result
 *	  is in sorted order.
 *
 *    RETURNS
 *	  pointer to an array of fractions
 *	  length of returned array
 */
{
    MVEXFraction *list;
    int num, denom, num_exp, denom_exp; /* used by macros above */
    int i, total, last;
    Bool zeroListed = False;

    *length_return = 0;
    total = MVEXFractionCount(range, nranges);
    if (total == 0)
	return ((MVEXFraction *) 0);

    list = (MVEXFraction *)Xmalloc(total * sizeof(MVEXFraction));
    if (list == (MVEXFraction *)0)
	return ((MVEXFraction *) 0);

    last = 0;
    for (i = 0; i < nranges; i++, range++) {
	FirstNumber(range, num);
	for (;;) {
	    FirstNumber(range, denom);
	    for (;;) {
		list[ last   ].numerator = num;
		list[ last++ ].denominator = denom;
		if (num == 0) {
		    list[ last-1 ].denominator = 1;
		    if (zeroListed) {
			last--;
			break;
		    }
		    zeroListed = True;
		    break;
		}
		if (LastNumber(range, denom))
		    break;
		NextNumber(range, denom);
	    }
	    if (LastNumber(range, num))
		break;
	    NextNumber(range, num);
	}
    }

    if (last != total) {
	fprintf(stderr, "MVEXFractionList: count & list mismatch\n");
    }
    qsort(list, total, sizeof(MVEXFraction), FractionEqual);

    *length_return = total;
    return (list);
}
