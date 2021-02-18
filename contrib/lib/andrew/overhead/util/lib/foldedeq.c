/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/foldedeq.c,v 2.5 1991/09/12 17:25:33 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/foldedeq.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

int FoldTRT[256] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, '!', '"', '#', '$', '%',
    '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c',
    'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\', ']', '^', '_', '`', 'a',
    'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 127,
    128, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, '!', '"', '#', '$', '%',
    '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4',
    '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'a', 'b', 'c',
    'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
    's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '[', '\\', ']', '^', '_', '`', 'a',
    'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
    'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~', 127,
};

/*
  if (s1 < s2) lexicographically, return a value less than 0;
  if (s1 == s2) return 0;
  if (s1 > s2) return a value greater than 0;

  High bits are disregarded EXCEPT for character 128.
  */

lc_strcmp(s1, s2)
    register unsigned char *s1;
    register unsigned char *s2;
{
    while (*s1 && (FoldTRT[*s1] == FoldTRT[*s2])) {
	++s1;
	++s2;
    }

    return FoldTRT[*s1] - FoldTRT[*s2];
}

lc_strncmp(s1, s2, n)
    register unsigned char *s1;
    register unsigned char *s2;
{
    while (n && *s1 && (FoldTRT[*s1] == FoldTRT[*s2])) {
	--n;
	++s1;
	++s2;
    }

    return n == 0 ? 0 : FoldTRT[*s1] - FoldTRT[*s2];
}

FoldedEQ(s1, s2)
    register unsigned char *s1;
    register unsigned char *s2;
{
    return (lc_strcmp(s1, s2) == 0);
}

FoldedEQn(s1, s2, n)
    register unsigned char *s1;
    register unsigned char *s2;
    register int n;
{
    return (lc_strncmp(s1, s2, n) == 0);
}


