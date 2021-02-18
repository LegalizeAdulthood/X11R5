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

#include "sj_kcnv.h"

Void	free_jrec(), free_clrec();

JREC	*free_jlst(p)
Reg1	JREC	*p;
{
	JREC	*jpk;
	JREC	*jp;
	JREC	*jn;

	jp = NULL;

	for ( ; p ; p = jn) {

		jn = p -> jsort;

		if (p -> count) {
			if (jp)
				jp -> jsort = p;
			else
				jpk = p;
			jp = p;
		}

		else {
			free_jrec(p);
		}
	}

	if (jp) jp -> jsort = NULL;

	return jpk;
}

CLREC	*free_clst(p, l)
Reg1	CLREC	*p;
Int	l;
{
	CLREC	*cpk;
	CLREC	*cp;
	CLREC	*cn;
	Int	i = FALSE;

	cp = cpk = NULL;

	for ( ; p ; p = cn) {

		cn = p -> clsort;

		if ((Int)p -> cllen == l) {
			if (p == selcl) {
				if (cp) {
					p -> clsort = cpk;
					cpk = p;
				}
				else
					cpk = cp = p;
			}
			else {
				if (cp)
					cp -> clsort = p;
				else
					cpk = p;
				cp = p;
			}
		}

		else {
			(p -> jnode -> count)--;

			free_clrec(p);
		}
	}

	if (cp) cp -> clsort = NULL;

	return cpk;
}

Void	free_clall(p)
Reg1	CLREC	*p;
{
	CLREC	*next;

	while (p) {
		(p -> jnode -> count)--;
		next = p -> clsort;
		free_clrec(p);
		p = next;
	}
}

Void	free_jall(p)
Reg1	JREC	*p;
{
	JREC	*next;

	while (p) {
		next = p -> jsort;
		free_jrec(p);
		p = next;
	}
}

Void	freework()
{
	free_clall(maxclptr);
	clt1st = maxclptr = NULL;

	free_jall(maxjptr);
	jrt1st = maxjptr = NULL;
}

