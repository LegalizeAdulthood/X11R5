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
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include "sj_const.h"

FILE	*infp = NULL;
FILE	*outfp = NULL;

char	*Zalloc();
FILE	*Fopen();

static	char	*idxtop = NULL;

main(argc, argv)
int	argc;
char	**argv;
{
	int	makelist();

	parse(argc, argv);

	while (readline()) setline(makelist);
	flush_douon();
	makehead();

	return 0;
}

char	*make_idxlist(name)
char	*name;
{
	int	i;
	char	*p;

	i = strlen(name);
	p = Zalloc(i + 2);
	strcpy(p, name);
	return p;
}
char	*get_idxlist(name)
char	*name;
{
	FILE	*fp;
	int	i;
	char	*p, *q, *r;

	fp = Fopen(name, "r");

	i = Fsize(name);

	p = Zalloc(i + 2);
	Fseek(fp, 0L, 0);
	Fread(p, 1, i, fp);
	Fclose(fp);

	for (q = r = p ; *q ; ) {
		while (*q <= ' ') q++;
		while (*q > ' ') *r++ = *q++;
		*r++ = 0;
		q++;
	}
	*r = 0;

	i = 0;
	for (q = p ; *q ; ) {
		if ((fp = fopen(q, "r")) != NULL)
			fclose(fp);
		else {
			fprintf(stderr, "Can't open %s mode \"r\"\n", q);
			i++;
		}
		while (*q++) ;
	}
	if (i) exit(1);

	return p;
}

parse(argc, argv)
int	argc;
char	**argv;
{
	int	i;
	char	*progname = NULL;
	char	*p;

	if (progname = rindex(argv[0], '/'))
		progname++;
	else
		progname = argv[0];

	if (argc != 3) usage(progname);

	p = argv[1];
	idxtop = (*p == '-') ? get_idxlist(p + 1) : make_idxlist(p);

	outfp = Fopen(argv[2], "w");
}

get_number(ptr)
register char	*ptr;
{
	register int	i = 0;

	while (*ptr >= '0' && *ptr <= '9') {
		i = i * 10 + (*ptr++ - '0');
	}

	return(i);
}

usage(name)
char	*name;
{
	fprintf(stderr, "Usage: %s visual_file binary_file\n",
		name ? name : "encode");
	exit(0);
}

int	getch()
{
	if (infp != NULL) {
		int	c;

		if ((c = Fgetc(infp)) != EOF) return c;

		Fclose(infp);
		infp = NULL;

		while (*idxtop++) ;
	}

	if (*idxtop) {
		infp = Fopen(idxtop, "r");

		fprintf(stderr, "Reading %s\n", idxtop);
		fflush(stderr);

		return getch();
	}

	return EOF;
}

mark_file(out)
FILE	*out;
{
	static	char	pathname[MAXPATHLEN];
	static	long	pos;

	if (out) {
		FILE	*fp;
		int	c;

		fprintf(out, "%s:\t", pathname);
		fp = Fopen(pathname, "r");
		Fseek(fp, pos, 0);
		c = Fgetc(fp);
		while (c != EOF && c != '\n') {
			fputc(c, out);
			c = Fgetc(fp);
		}
		fputc('\n', out);
		Fclose(fp);
	}
	else {
		strcpy(pathname, idxtop);
		pos = infp ? Ftell(infp) : 0;
	}
}
