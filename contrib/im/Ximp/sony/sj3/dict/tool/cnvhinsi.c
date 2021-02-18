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

struct gram_code {
	char	*name;
	int	code;
} gramtbl[] = {
#include "GramTable"

};

#define	GramMax	(sizeof(gramtbl)/sizeof(struct gram_code) - 1)

int	cnvhinsi(buf)
u_char	*buf;
{
	register int	min;
	register int	max;
	register int	mid;
	register int	i;

	min = 0;
	max = GramMax;
	while (min <= max) {
		mid = (min + max) / 2;

		i = strcmp(buf, gramtbl[mid].name);

		if (i < 0) {
			max = mid - 1;
		}
		else if (i > 0) {
			min = mid + 1;
		}
		else {
			return(gramtbl[mid].code);
		}
	}

	return 0;
}

char	*hinsi_str(code)
int	code;
{
	int	i;

	for (i = 0 ; i <= GramMax ; i++) {
		if (gramtbl[i].code == code) return gramtbl[i].name;
	}

	return "Err";
}
