/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#include <stdio.h>
#include <ctype.h>
void wlm_incl_trunc () ;

/*
 # The algorithm for this is to strip off the lower case letters,
 # a word at a time, starting with the trailing words, until the basename
 # is 9 characters or fewer.  Example:
 #   VeryLongWidgetName.h (too long)
 #   VeryLongWidgetN.h (still too long)
 #   VeryLongWN.h (still too long)
 #   VeryLWN.h (okay now)
 # But, when down to the last word, only strip just enough letters.
 # Examples:
 #   AtrociouslyLongWidgetName.h -> AtrociLWN.h
 #   Supercallifragilistic.h -> Supercall.h
 */

main (argc, argv)
    int argc ;
    char **argv ;
{
if (argc < 2)
    {
    fprintf (stderr, "Usage: %s basename_string\n", argv[0]) ;
    exit (1) ;
    }

wlm_incl_trunc (argv[1]) ;
exit (0) ;
}

#define MAX_WORDS 9

static void get_chunks () ;

void
wlm_incl_trunc (in)
    char *in ;
{
char out[10] ;
char *chunks[MAX_WORDS] ;
int letter_count[MAX_WORDS] ;
int i ;
int count ;
int n_chunks ;
int total_letter_count ;

/*
 * First, separate the name into word chunks.
 * OneTwoThree -> One Two Three
 */

get_chunks (chunks, letter_count, &total_letter_count,
	    MAX_WORDS, &n_chunks, in) ;

/*
 * Subtract the lowercase letter count until okay
 */

for (i=n_chunks-1;  i > 0;  i--)
    {
    if (total_letter_count <= 9)
	break ;
    total_letter_count -= letter_count[i]-1 ;
    letter_count[i] = 1 ;
    }

if (total_letter_count > 9)
    letter_count[0] -= total_letter_count-9 ;

count = 0 ;
for (i=0;  i < n_chunks;  i++)
    {
    strncpy (&out[count], chunks[i], letter_count[i]) ;
    count += letter_count[i] ;
    }
out[count] = '\0' ;
printf ("%s", out) ;
return ;
}

static void
get_chunks (chunks, letter_count, total_letter_count, max_chunks, n_chunks, in)
    char *chunks[] ;
    int letter_count[] ;
    int *total_letter_count ;
    int max_chunks ;
    int *n_chunks ;
    char *in ;
{
char *ptr ;
char *original_in = in ;
*n_chunks = 0 ;
*total_letter_count = 0 ;

while (*in != '\0')
    {
    if (*n_chunks == max_chunks)
	{
	fprintf (stderr,
	"Ridiculous number of words (greater than %d) in classname (%s)\n",
	max_chunks, original_in) ;
	exit (1) ;
	}
    letter_count[*n_chunks] = 1 ;
    *total_letter_count += 1 ;
    /* skip initial capital letter */
    ptr = in+1 ;

    /* increment until next word or end of text */
    while (!isupper(*ptr) && *ptr != '\0')
	{
	ptr++ ;
	letter_count[*n_chunks]++ ;
	*total_letter_count += 1 ;
	}

    /* copy the word found into a chunk */
    chunks[*n_chunks] = (char *)malloc (letter_count[*n_chunks]+1) ;
    strncpy (chunks[*n_chunks], in, letter_count[*n_chunks]) ;
    chunks[*n_chunks][letter_count[*n_chunks]] = '\0' ;

    in = ptr ;
    *n_chunks += 1 ;
    }
return ;
}
