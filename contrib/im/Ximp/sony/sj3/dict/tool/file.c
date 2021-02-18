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
#include <sys/stat.h>

char	*Malloc();

typedef	struct filelist {
	FILE	*fp;
	char	*fname;
	struct filelist *next;
} FileList;

static	FileList *flist = NULL;

static	char	*get_fname(fp)
FILE	*fp;
{
	FileList *p;

	for (p = flist ; p ; p = p -> next)
		if (p -> fp == fp) return p -> fname;
	return "some file";
}

FILE	*Fopen(filename, type)
char	*filename, *type;
{
	FILE	*fp;
	char	*p;
	FileList *fl;

	if (!(fp = fopen(filename, type))) {
		fprintf(stderr, "Can't open %s mode \"%s\"\n", filename, type);
		exit(1);
	}

	fl = (FileList *)Malloc(sizeof(*fl));
	fl -> fname = Malloc(strlen(filename) + 1);
	strcpy(fl -> fname, filename);
	fl -> fp = fp;
	fl -> next = flist;

	return fp;
}

Fclose(fp)
FILE	*fp;
{
	FileList *p, *q;

	if (fclose(fp) == EOF) {
		fprintf(stderr, "Close error in %s\n", get_fname(fp));
		exit(1);
	}

	for (p = flist, q = NULL ; p ; q = p, p = p -> next) {
		if (p -> fp == fp) {
			if (q)
				q -> next = p -> next;
			else
				flist = p -> next;
			free(p -> fname);
			free(p);
			break;
		}
	}
}

Fseek(fp, ofs, ptr)
FILE	*fp;
long	ofs;
int	ptr;
{
	if (fseek(fp, ofs, ptr) == 0) return;

	fprintf(stderr, "Seek error in %s\n", get_fname(fp));
	exit(1);
}

Ftell(fp)
FILE	*fp;
{
	return ftell(fp);
}

Fsize(filename)
char	*filename;
{
	struct	stat	buf;

	if (stat(filename, &buf) == 0) return buf.st_size;

	fprintf(stderr, "Stat error in %s\n", filename);
	exit(1);
}

Fread(p, s, n, fp)
char	*p;
int	s, n;
FILE	*fp;
{
	if (fread(p, s, n, fp) == n) return;

	fprintf(stderr, "Read error in %s\n", get_fname(fp));
	exit(1);
}

Fwrite(p, s, n, fp)
char	*p;
int	s, n;
FILE	*fp;
{
	if (fwrite(p, s, n, fp) == n) return;

	fprintf(stderr, "Write error in %s\n", get_fname(fp));
	exit(1);
}

Fgetc(fp)
FILE	*fp;
{
	return fgetc(fp);
}

Fflush(fp)
FILE	*fp;
{
	fflush(fp);
}
