
/***********************************************************************

bitvector.c - some routines for dealing with bitvectors

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

 **********************************************************************/

/*LINTLIBRARY*/

#include "util.h"
#undef	BITS_PER_VECTOR
#define BITS_PER_VECTOR	128
#include "bitvector.h"

int
bitcount(max, bv)
int max;
BitVector bv;
{
	register i, count;

	for(count = i = 0; i < max; i++)
		if(IS_SET(i, bv))
			count++;
	return count;
}

char *
BitVectorToString(max, bv)
BitVector bv;
{
	char *string = tempstring();
	register i;

	for(i = 0; i < max; i++)
		string[i] = IS_SET(i, bv) ? (i % 10) + '0' : '-' ;
	string[i] = '\0';
	return string;
}


void
StringToBitVector(string, max, bv)
char *string;
int max;
BitVector bv;
{
	register i;

	if(strlen(string) != max)
		error("StringToBitVector: strlen(%s)=%d != %d",
			string, strlen(string), max);

	for(i = 0; i < max; i++)
		if(string[i] != '-')
			BIT_SET(i, bv);
		else
			BIT_CLEAR(i, bv);		
}


SetBitVector(v)
register BitVector v;
{
	register int nints = INTS_PER_VECTOR;

	while(--nints >= 0)
		*v++ = -1;
}


ClearBitVector(nints, v)
register int nints;
register BitVector v;
{

	while(--nints >= 0)
		*v++ = 0;
}


AssignBitVector(nints, v1, v2)
register int nints;
register BitVector v1, v2;
{

	while(--nints >= 0)
		*v1++ = *v2++;
}

int
BitVectorDeQ(max, v)
register int max;
register BitVector v;
{
	register int i;
	for(i = 0; i < max; i++)
		if(IS_SET(i, v)) {
			BIT_CLEAR(i, v);
			return i;
		}
	return -1;

}

int *
BitVectorOr(v, v1, v2, ipv)
int *v;
register int *v1, *v2;
register int ipv;
{
	int *vv = v;
	do
		*vv++ = *v1++ | *v2++;
	while(--ipv > 0);
	return v;
}

int *
BitVectorAnd(v, v1, v2, ipv)
int *v;
register int *v1, *v2;
register int ipv;
{
	int *vv = v;
	do
		*vv++ = *v1++ & *v2++;
	while(--ipv > 0);
	return v;
}

int
BitVectorNoBitsSet(v, ipv)
register int *v;
register int ipv;
{
	do
		if(*v++) return 0;
	while(--ipv > 0);
	return 1;
}
