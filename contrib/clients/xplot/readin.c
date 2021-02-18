/* Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

                        All Rights Reserved

In order to better advertise the GNU 'plot' graphics package, the
standard GNU copyleft on this software has been lifted, and replaced
by the following:
   Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

xplot is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY.  No author or distributor accepts responsibility to
anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GNU General Public License for full details. Also:

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.  */

#include "config.h"
#include <memory.h>
#include <errno.h>
#define LINE 512

extern char *data;
extern int data_len;
extern int data_head;
extern int data_tail;

extern int high_byte_first;

extern int debug_flag;

extern int eoffound;
int endinp;

/* Have to do my own buffering: */
/* Modified now, to use the new Resize_Yes data buffer */

int readit(strm)
FILE *strm;
{
	int newbytes;

	resize_check(); /* Good time to check */
#ifdef DEBUG
	fprintf(stderr, "%d remaining in buf\n", data_tail - data_head);
#endif
	endinp = 0;
	errno = 0;
#ifdef Resize_Yes
	if (data_tail + LINE > data_len) {
		data_len += LINE;
		if (data != NULL)
			data = (char *) realloc(data, data_len);
		else
			data = (char *) malloc(data_len);
	}
#else /* Resize_Yes */
	if (data_tail > data_head)
		bcopy(data + data_head, data, data_tail - data_head);
	data_tail -= data_head;
	data_head = 0;
#endif /* Resize_Yes */
	if ((newbytes = fread(data + data_tail, sizeof(char),
		data_len - data_tail, strm)) == 0) {
			endinp = 1;
	/* Should we repeat this after sleeping a short while? */
/* Only way to find end-of-file on fread is to get an error other than
   WOULDBLOCK on a read call */
		if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
			eoffound = 1; /* End of data! */
		}
#ifdef DEBUG
	fprintf(stderr, "No chars read, errno = %d\n", errno);
#endif
		return 0;
	}
	data_tail += newbytes;
	return 1;
}

int getstring(ptr,strm)
char *ptr;
FILE *strm;
{
	char *cp;
	int lenp;

	while ((cp = memchr(data + data_head, '\n',
			data_tail - data_head)) == NULL){
		data_head--; /* Want to keep beginning of command */
		if (!readit(strm)) /* Try reading more data */
			return 0;
		data_head++;
	}
	lenp = cp - data - data_head + 1;
	bcopy(data + data_head,ptr,lenp); /* Transfer string to ptr */
	ptr[lenp - 1] = 0; /* Eliminate the newline too */
	data_head += lenp;
#ifdef DEBUG
	if (debug_flag) fprintf(stderr,"%s",ptr);
#endif
	return lenp;
}

int getcoords(x,n,strm)
short *x;
int n;
FILE *strm;
{
	unsigned char c,d;
	int i;

	while (data_tail - data_head < 2*n) {
		data_head--;
		if (!readit(strm))
			return 0;
		data_head++;
	}
	for (i=0;i< n;i++) {
		if (high_byte_first == 1) {
			d = data[data_head++];
			c = data[data_head++];
		} else {
			c = data[data_head++];
			d = data[data_head++];
		}
			
		x[i] = (short) ((d << 8) | c);
#ifdef DEBUG
		if (debug_flag) fprintf(stderr," %d",x[i]);
#endif
	}
#ifdef DEBUG
	if (debug_flag) fprintf(stderr,"\n");
#endif
	return 1;
}
