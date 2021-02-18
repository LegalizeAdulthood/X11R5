/*
 * Copyright 1991 University of Wisconsin-Madison
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of Wisconsin-Madison not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The University of
 * Wisconsin-Madison makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF WISCONSIN-MADISON DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF WISCONSIN-MADISON BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:   Marvin Solomon
 *           Computer Sciences Department
 *           University of Wisconsin, Madison WI
 *           solomon@cs.wisc.edu or seismo!uwvax!solomon
 *
 * Current Maintainer:
 *
 *           Tim Theisen           Systems Programmer
 * Internet: tim@cs.wisc.edu       Department of Computer Sciences
 *     UUCP: uwvax!tim             University of Wisconsin-Madison
 *    Phone: (608)262-0438         1210 West Dayton Street
 *      FAX: (608)262-9777         Madison, WI   53706
 */
/*
 * Functions for manipulating an "index" of places where pages start.
 */
#include "xproof.h"
#include <assert.h>

#ifndef lint
static char rcsid[] = "$Header: /src/X11/uw/xproof/RCS/index.c,v 3.5 1991/10/04 22:05:44 tim Exp $";
#endif

/* Index of pages in document (not used if input is stdin) */
static struct {
	long p_seek;
	int  p_number;
} page_index[MAXPAGES];

static int indexSize;

/* Make an index of "seek pointers" to tell were pages start.
 * Return the number of pages.
 */
makeIndex(f)
register FILE *f;
{
	int n;
	long ftell();
	register int c;

	(void) fseek(f, 0L, 0);

	for (indexSize = 0; indexSize < MAXPAGES; indexSize++) {
		page_index[indexSize].p_seek = 0;
		page_index[indexSize].p_number = 0;
	}
	indexSize = 0;

	/* finite automaton searching for EOF or "\np" */
	for (;;) {
		c = getc(f);
		for (;;) {
			if (c == EOF)  {
				(void) fseek(f, 0L, 0);
				return indexSize;
			}
			if (c != '\n') break;
			c = getc(f);
			if (c == 'p') {
				/* success: found "\np" */
				(void)fscanf(f, "%d", &n);
				if (indexSize >= MAXPAGES) {
					(void)fprintf(stderr, "too many pages (%d max)\n",MAXPAGES);
					(void)fseek(f, 0L, 0);
					return indexSize;
				}
				while ((c=getc(f))!='\n' && c!=EOF) /* skip */;
				page_index[indexSize].p_seek = ftell(f);
				page_index[indexSize].p_number = n;
				indexSize++;
				break;
			}
		}
	}
}

int pageNumber(page)
int page;
{
	if (page < 0 || page >= indexSize) {
		fprintf(stderr,"internal error: lookup page page %d\n",page);
		return (long)0;
	}
	return page_index[page].p_number;
}
	
long seekPointer(page)
int page;
{
	if (page < 0 || page >= indexSize) {
		fprintf(stderr,"internal error: seek to page %d\n",page);
		return (long)0;
	}
	return page_index[page].p_seek;
}

/* Find a page numbered abs(new).  Search starting at 'current' and search
 * forward if new>0, backward if new<0 (new is never zero).  If we can't
 * find such a page, try searching in the opposite direction.  If we still
 * don't find it, return -1.
 */
/* Find a page numbered 'new'.  Start at position 'current' and serach forward
 * circularly.  Return the index of a page numbered 'new', or -1 if none
 * exists.
 */
int findPage(current, new)
int current, new;
{
	int i;

	for (i=current + 1; i<indexSize; i++)
		if (page_index[i].p_number == new) return i;
	for (i=0; i<=current; i++)
		if (page_index[i].p_number == new) return i;
	return -1;
}
