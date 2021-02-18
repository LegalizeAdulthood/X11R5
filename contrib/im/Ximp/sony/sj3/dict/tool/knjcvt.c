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

#include <stdio.h>
#include <sys/types.h>
#include "sj_const.h"
#include "sj_struct.h"

typedef struct div_list {
	int	len;
	u_char	code[2];
	struct div_list *child;
} DivList;

extern	HindoRec *askknj[];
extern	int	askknj_num;

static	free_divlist(p)
register DivList *p;
{
	register DivList *q;

	while (p) {
		q = p -> child;
		Free(p);
		p = q;
	}
}

static	DivList	*make_divrec()
{
	register DivList *p;

	p = (DivList *)Malloc(sizeof(DivList));
	p -> child = NULL;

	return p;
}

static	make_divlist(parent, knj, len)
DivList	*parent;
u_char	*knj;
int	len;
{
	int	i, j;
	int	num;
	int	minlen;
	int	tmplen;
	DivList	tmprec;
	DivList	*minrec;

	minrec = make_divrec();

	if (len == 0) {
		free_divlist(minrec);

		parent -> child = NULL;
		return 0;
	}
	else if (len == 1) {
		minrec -> len     = len;
		minrec -> code[0] = *knj;

		parent -> child = minrec;
		return len;
	}

	if (codesize(*knj) == 1) {
		minrec -> len     = 1;
		minrec -> code[0] = *knj;
		minlen = make_divlist(minrec, knj + 1, len - 1) + 1;
	}
	else {
		minrec -> len     = 2;
		minrec -> code[0] = *knj;
		minrec -> code[1] = *(knj + 1);
		minlen = make_divlist(minrec, knj + 2, len - 2) + 2;
	}

	for (i = 0 ; i < len ; i = j) {
		j = i + codesize(*(knj + i));

		if (j <= 2 || !(num = isknjexist(knj, j))) continue;

		tmprec.len     = 2;
		tmprec.code[0] = (OffsetAssyuku | ((num >> 8) & 0xff));
		tmprec.code[1] = (num & 0xff);
		tmprec.child   = NULL;
		tmplen = make_divlist(&tmprec, knj + j, len - j) + 2;

		if (tmplen < minlen) {
			free_divlist(minrec -> child);

			*minrec = tmprec;
			minlen = tmplen;
		}
		else
			free_divlist(tmprec.child);
	}

	for (i = 0 ; i < askknj_num ; i++) {
		if ((j = askknj[i] -> klen) > len) continue;

		if (string_cmp(askknj[i]->kptr, j, knj, j)) continue;

		tmprec.len     = 1;
		tmprec.code[0] = (KanjiAssyuku | i);
		tmprec.child   = NULL;
		tmplen = make_divlist(&tmprec, knj + j, len - j) + 1;

		if (tmplen < minlen) {
			free_divlist(minrec -> child);

			*minrec = tmprec;
			minlen = tmplen;
		}
		else
			free_divlist(tmprec.child);
	}

	parent -> child = minrec;

	return minlen;
}

u_char	*knjofscvt(ptr, len, ret)
u_char	*ptr;
int	len;
int	*ret;
{
	int	i;
	u_char	*p, *q;
	DivList parent, *dl;

	askknj_num = 0;

	parent.child = NULL;
	i = make_divlist(&parent, ptr, len);
	*ret = i;

	p = (u_char *)Malloc(i);
	for (q = p, dl = parent.child ; dl ; dl = dl -> child) {
		*q++ = dl -> code[0];
		if (dl -> len != 1) {
			*q++ = dl -> code[1];
		}
	}

	free_divlist(parent.child);

	return p;
}

u_char	*knjcvt(ptr, len, ret)
u_char	*ptr;
int	len;
int	*ret;
{
	int	i;
	u_char	*p, *q;
	DivList parent, *dl;

	parent.child = NULL;
	i = make_divlist(&parent, ptr, len);
	*ret = i;

	p = (u_char *)Malloc(i);
	for (q = p, dl = parent.child ; dl ; dl = dl -> child) {
		*q = (dl -> child) ? 0 : KanjiStrEnd;
		*q++ |= dl -> code[0];
		if (dl -> len != 1) {
			*q++ = dl -> code[1];
		}
	}

	free_divlist(parent.child);

if (i != q - p) {
	printf("\202\261\202\261\n");
	exit(1);
}
	return p;
}
