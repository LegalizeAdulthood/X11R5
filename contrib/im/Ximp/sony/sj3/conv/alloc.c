/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <sys/types.h>
#include "sj_rename.h"
#include "sj_typedef.h"
#include "sj_const.h"
#include "sj_var.h"
#include "sj_struct.h"

#define	JREC_NUM	128
#define	CLREC_NUM	128

static	JREC	*jrec = NULL;
static	CLREC	*clrec = NULL;

JREC	*alloc_jrec()
{
	register JREC	*p;
	register int	i;

	if (jrec == NULL) {
		jrec = (JREC *)malloc(sizeof(JREC) * JREC_NUM);
		if (jrec == NULL) return (JREC *)malloc(sizeof(JREC));
		for (i = 0, p = jrec ; i < JREC_NUM - 1 ; i++, p++)
			p -> jsort = p + 1;
		p -> jsort = NULL;
	}
	p = jrec;
	jrec = jrec -> jsort;

	return p;
}

Void	free_jrec(p)
JREC	*p;
{
	if (p != NULL) {
		p -> jsort = jrec;
		jrec = p;
	}
}

CLREC	*alloc_clrec()
{
	register CLREC	*p;
	register int	i;

	if (clrec == NULL) {
		clrec = (CLREC *)malloc(sizeof(CLREC) * CLREC_NUM);
		if (clrec == NULL) return (CLREC *)malloc(sizeof(CLREC));
		for (i = 0, p = clrec ; i < CLREC_NUM - 1 ; i++, p++)
			p -> clsort = p + 1;
		p -> clsort = NULL;
	}
	p = clrec;
	clrec = clrec -> clsort;

	return p;
}

Void	free_clrec(p)
CLREC	*p;
{
	if (p != NULL) {
		p -> clsort = clrec;
		clrec = p;
	}
}
