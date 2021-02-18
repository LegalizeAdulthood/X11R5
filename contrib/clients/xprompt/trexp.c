static char *SCCS_Id = "@(#)trexp.c\tver 1.3 (91/09/28 14:23:30) brachman@cs.ubc.ca";

/* vi: set tabstop=4 : */

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

#include <stdio.h>

#include "trexp.h"

#ifdef DEBUG
main()
{
	int st;
	char buf[BUFSIZ];
	trexp *t;

	printf("Enter the expression:\n");
	gets(buf);
	if ((t = trcomp(buf)) == NULL) {
		fprintf(stderr, "Compile error.\n");
		exit(1);
	}
	printf("Number in set: %d\n", trnset(t));
	trshow(stdout, t);
	while (1) {
		printf("Character?\n");
		if (gets(buf) == NULL)
			break;
		if (trexec(t, buf[0]))
			printf("Yes.\n");
		else
			printf("No.\n");
	}
}
#endif DEBUG

/*
 * trcomp - compile a transliteration string
 *
 * Returns a pointer to a trexp structure if successful and NULL
 * if the control string is improperly formed.
 *
 * The macro trexec may subsequently be used with the compiled expression
 * and a single character.
 * The value is 1 if the character is in the set, 0 otherwise.
 * The value of the macro trnset, passed the pointer to the compiled
 * expression, is the number of elements specified by the expression.
 *
 * The argument to trcomp is a string specifying individual characters
 * and ranges of characters as used in the tr(1) command:
 * a) a backslash followed by 1, 2, or 3 octal digits stands for the ASCII
 *    character corresponding to the octal value.  The value is limited to
 *    8 bits.
 * b) a backslash followed by a non-octal digit character stands for the
 *    character.
 * c) other ASCII characters stand for themselves
 * d) the range of characters between two characters, <c1> and <c2>,
 *    is specified by sequence of the form <c1>-<c2> when <c1> comes before
 *    <c2> in the ASCII character set
 *
 * The space allocated by trcomp() may be released using free()
 */

static int next_token();

trexp *
trcomp(exp)
char *exp;
{
	register int i, low, high;
	char *ptr, *save;
	trexp *t;
	char *malloc();

	if ((t = (trexp *) malloc(sizeof(trexp))) == NULL)
		return(NULL);
	for (i = 0; i < sizeof(trexp); i++)
		t->exp[i] = 0;
	t->nset = 0;
	ptr = exp;
	while (*ptr != '\0') {
		if ((low = next_token(&ptr)) < 0)
			return(NULL);
		if (*ptr == '-') {
			save = ptr++;
			if ((high = next_token(&ptr)) < 0)
				return(NULL);
			if (low < high) {
				for (i = low; i <= high; i++) {
					if (!t->exp[i]) {
						t->exp[i] = 1;
						t->nset++;
					}
				}
			}
			else {
				ptr = save;
				if (!t->exp[low]) {
					t->exp[low] = 1;
					t->nset++;
				}
			}
		}
		else {
			if (!t->exp[low]) {
				t->exp[low] = 1;
				t->nset++;
			}
		}
	}
	return(t);
}

#define isoctdigit(d)	(d >= '0' && d <= '7')

static int
next_token(str)
char **str;
{
	register char *p;
	int val;

	p = *str;
	if (*p == '\\') {
		p++;
		if (isoctdigit(*p)) {
			val = *p++ - '0';
			if (isoctdigit(*p)) {
				val = val * 8 + *p++ - '0';
				if (isoctdigit(*p))
					val = val * 8 + *p++ - '0';
			}
			*str = p;
			return(val & 0377);
		}
		else {
			if (*p == '\0')		/* Premature eos */
				return(-1);
			*str = p + 1;
			return(*p & 0377);
		}
	}
	*str = p + 1;
	return(*p & 0377);
}

void
trshow(f, t)
FILE *f;
trexp *t;
{
	register int i, j, k;
	static char *ascii_tab[] = {
		"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL",
		"BS",  "HT",  "NL",  "VT",  "NP",  "CR",  "SO",  "SI",
		"DEL", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
		"CAN", "EM",  "SUB", "ESC", "FS",  "GS",  "RS",  "US",
		"SP",  "!",   "\"",  "#",   "$",   "%",   "&",   "'",
		"(",   ")",   "*",   "+",   ",",   "-",   ".",   "/",
		"0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",
		"8",   "9",   ":",   ";",   "<",   "=",   ">",   "?",
		"@",   "A",   "B",   "C",   "D",   "E",   "F",   "G",
		"H",   "I",   "J",   "K",   "L",   "M",   "N",   "O",
		"P",   "Q",   "R",   "S",   "T",   "U",   "V",   "W",
		"X",   "Y",   "Z",   "[",   "\\",  "]",   "^",   "_",
		"`",   "a",   "b",   "c",   "d",   "e",   "f",   "g",
		"h",   "i",   "j",   "k",   "l",   "m",   "n",   "o",
		"p",   "q",   "r",   "s",   "t",   "u",   "v",   "w",
		"x",   "y",   "z",   "{",   "|",   "}",   "~",   "DEL"
	};

	for (i = 0; i < 32; i++) {
		for (j = 0; j < 8; j++) {
			k = i * 8 + j;
			if (t->exp[k]) {
				if (k > 0177)
					fprintf(f, "|%03o    ", k, t->exp[k]);
				else
					fprintf(f, "|%03o %3s", k, ascii_tab[k]);
			}
			else
				fprintf(f, "|       ");
		}
		fprintf(f, "|\n");
	}
}

#ifdef NOTDEF
/*
 * This is the old, non-compiling version
 */
strrange(str, ch)
char *str;
int ch;
{
	int low, high;
	char *ptr, *save;

	ch &= 0377;
	ptr = str;
	while (*ptr != '\0') {
		if ((low = next_token(&ptr)) < 0)
			return(-1);
		if (ch == low)
			return(1);
		if (*ptr == '-') {
			save = ptr++;
			if ((high = next_token(&ptr)) < 0)
				return(-1);
			if (low < high) {
				if (ch >= low && ch <= high)
					return(1);
			}
			else
				ptr = save;
		}
	}
	return(0);
}
#endif NOTDEF

