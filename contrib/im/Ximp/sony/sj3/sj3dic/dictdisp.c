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

dictdisp(output)
char	*output;
{
	unsigned char	buf[BUFSIZ];
	unsigned char	*p;
	FILE	*fp;
	int	flg;
	int	i;

	if (output && *output) {
		fp = fopen(output, "w");
		if (!fp) {
			error_out("%s \202\252\203\111\201\133\203\166\203\223\202\305\202\253\202\334\202\271\202\361", output);
			exit(1);
		}
	}
	else
		fp = stdout;

	flg = sj3_getdict(buf);
	while (flg == 0) {
		p = buf;
		printout(fp, p);
		i = strlen(p);
		if (i < 24)
			while (i < 24) {
				printout(fp, "\t");
				i += 8;
			}
		else
			printout(fp, " ");
		while (*p++) ;
		printout(fp, p);
		i = strlen(p);
		if (i < 24)
			while (i < 24) {
				printout(fp, "\t");
				i += 8;
			}
		else
			printout(fp, " ");
		while (*p++) ;
		printout(fp, hns2str(*p));
		printout(fp, ":\n");

		flg = sj3_nextdict(buf);
	}

	if (fp != stdout) fclose(fp);
}

