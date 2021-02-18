/* SCCS_Id = "@(#)trexp.h\tver 1.3 (91/09/28 14:23:31) brachman@cs.ubc.ca" */

/*
 * Copyright 1989, 1990, 1991 by
 * Barry Brachman and the University of British Columbia
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of U.B.C. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  U.B.C. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#define trexec(t, ch)		(t->exp[ch & 0377])
#define trnset(t)			(t->nset)

typedef struct trexp {
	char exp[256];
	int nset;
} trexp;

trexp *trcomp();
void trshow();
