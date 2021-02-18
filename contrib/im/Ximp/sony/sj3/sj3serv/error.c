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
#include "sj_typedef.h"

extern	char	program_name[];
extern	int	debug_level;
extern	int	client_fd;

static	char	*tty_name = "/dev/tty";
static	FILE	*errfp = NULL;
static	FILE	*logfp = NULL;
static	FILE	*dbgfp = NULL;

open_error()
{
	extern	char	*error_file;
	int	flg = 0;

	if (error_file && *error_file) {
		if (!strcmp(error_file, tty_name)) {
			errfp = stderr;
			flg = -1;
		}
		else if (!(errfp = fopen(error_file, "a")))
			fprintf(stderr, "Can't open \"%s\"\r\n", error_file);
	}
	else
		errfp = NULL;

	return flg;
}
error_out(s, p1, p2, p3, p4, p5)
char	*s;
int	p1, p2, p3, p4, p5;
{
	char	tmp[BUFSIZ];

	if (errfp) {
		sprintf(tmp, s, p1, p2, p3, p4, p5);
		fprintf(errfp, "%s: %s\r\n", program_name, tmp);
		fflush(errfp);
	}
	exit(1);
}
warning_out(s, p1, p2, p3, p4, p5)
char	*s;
int	p1, p2, p3, p4, p5;
{
	char	tmp[BUFSIZ];

	if (errfp) {
		sprintf(tmp, s, p1, p2, p3, p4);
		fprintf(errfp, "%s: warning: %s\r\n", program_name, tmp);
		fflush(errfp);
	}
}

open_log()
{
	extern	char	*log_file;
	int	flg = 0;

	if (log_file && *log_file) {
		if (!strcmp(log_file, tty_name)) {
			logfp = stderr;
			flg = -1;
		}
		else
			logfp = fopen(log_file, "a");
		if (logfp) {
			long	t;

			time(&t);
			fprintf(logfp, "%s: log started at %s\r",
				program_name, ctime(&t));
			fflush(logfp);
		}
		else
			fprintf(stderr, "Can't open \"%s\"\r\n", log_file);
	}
	else
		logfp = NULL;

	return flg;
}
logging_out(s, p1, p2, p3, p4, p5)
char	*s;
int	p1, p2, p3, p4, p5;
{
	char	tmp[BUFSIZ];

	if (logfp) {
		sprintf(tmp, s, p1, p2, p3, p4, p5);
		fprintf(logfp, "%s\r\n", tmp);
		fflush(logfp);
	}
}

open_debug()
{
	extern	char	*debug_file;
	int	flg = 0;

	if (debug_file && *debug_file) {
		if (!strcmp(debug_file, tty_name)) {
			dbgfp = stderr;
			flg = -1;
		}
		else  if (!(dbgfp = fopen(debug_file, "a")))
			fprintf(stderr, "Can't open \"%s\"\r\n", debug_file);
	}
	else
		dbgfp = NULL;

	return flg;
}
debug_out(lvl, s, p1, p2, p3, p4, p5)
int	lvl;
char	*s;
int	p1, p2, p3, p4, p5;
{
	if (lvl <= debug_level && dbgfp) {
		fprintf(dbgfp, s, p1, p2, p3, p4, p5);
		fflush(dbgfp);
	}
}

