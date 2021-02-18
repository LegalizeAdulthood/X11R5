
/*
****************************************************************************

              Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution of
the software without specific, written prior permission.  
Sun Microsystems, Inc. makes no representations about the suitability
of this software for any purpose.  It is provided "as is" without 
express or implied warranty.

SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL  SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF 
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Auther: Hiroshi Watanabe	(nabe@japan.sun.com) Sun Microsystems, Inc.
	Tomonori Shioda		(shioda@japan.sun.com) Sun Microsystems, Inc.

****************************************************************************
*/

#include <stdio.h>
#include <string.h>

FILE *
open_maddress_file(fp_madd)
	FILE	*fp_madd;
{
	char	madd_fname[512];

	sprintf(madd_fname, "%s/.shinbun_address", getenv("HOME"));
	if((fp_madd = fopen(madd_fname, "r")) == NULL){
	        sprintf(madd_fname,"%s/shinbun_address",getenv("SHINBUNHOME"));
		fp_madd = fopen(madd_fname, "r");
	}
	return fp_madd;
}

char *
search_madd(fp_madd, madd)
	char	*madd;
	FILE	*fp_madd;
{
	char	tmp[512], tmp1[256], tmp2[256];

	while((fgets(tmp, 512, fp_madd)) != NULL){
		sscanf(tmp, "%s %s", tmp1, tmp2);
		if(strncmp(madd, tmp1, strlen(madd)) == 0){
			fclose(fp_madd);
			return tmp2;
		}
	}
	fclose(fp_madd);
	return madd;
}

char *
convert_madd_to_name(madd)
	char	*madd;
{
	FILE	*fp_madd;
	char	*tmpc;

	while(*madd == ' '){
		madd++;
	}
	tmpc = madd;

	while(*tmpc != ' ' && *tmpc != '\t' && *tmpc != '\n' && *tmpc != '\0'){
		tmpc++;
	}
	*tmpc = '\0';

	fp_madd = open_maddress_file(fp_madd);
	if(fp_madd == NULL){
		return madd;
	}
	return (search_madd(fp_madd, madd));
}

#ifdef BRACKET_DEBUG
main()
{
	char	add[256];

	while(gets(add) != NULL){
		printf("%s\n", convert_madd_to_name(add));
	}
}
#endif BRACKET_DEBUG
