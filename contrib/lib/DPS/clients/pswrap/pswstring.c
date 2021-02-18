/*
 *  pswstring.c
 *
 * Copyright (C) 1988 by Adobe Systems Incorporated.
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notices appear in all copies and that
 * both those copyright notices and this permission notice appear in
 * supporting documentation and that the name of Adobe Systems
 * Incorporated not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.  If any portion of this software is changed, it cannot be
 * marketed under Adobe's trademarks and/or copyrights unless Adobe, in
 * its sole discretion, approves by a prior writing the quality of the
 * resulting implementation.
 * 
 * ADOBE MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THE SOFTWARE FOR
 * ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 * ADOBE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT OF THIRD PARTY RIGHTS.  IN NO EVENT SHALL ADOBE BE LIABLE
 * TO YOU OR ANY OTHER PARTY FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE, STRICT LIABILITY OR ANY OTHER ACTION ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.  ADOBE WILL NOT
 * PROVIDE ANY TRAINING OR OTHER SUPPORT FOR THE SOFTWARE.
 * 
 * PostScript, Display PostScript, and Adobe are trademarks of Adobe Systems
 * Incorporated registered in the U.S.A. and other countries.
 *
 * Author: Adobe Systems Incorporated
 */

#include <stdio.h>
#include <ctype.h>

#define outfil stdout
#define MAX_PER_LINE 16

extern int outlineno;		/* line number in output file */

int PSWStringLength(s) char *s; {
    register char *c = s;
    register int len = 0;

    while (*c != '\0') {	/* skip \\ and \ooo */
	if (*c++ == '\\') {
	    if (*c++ != '\\') c += 2;
	}
	len++;
    }
    return (len);
}

PSWOutputStringChars(s) char *s; {
    register char *c = s;
    register char b;
    register int perline = 0;
    
    while (*c != '\0') {
    putc('\'',outfil);
    switch (b = *c++) {
	    case '\\':
	        putc('\\',outfil);
		fputc(b = *c++,outfil);
	        if (b != '\\') {putc(*c++,outfil);putc(*c++,outfil);}
		break;
	    case '\'':
	        fprintf(outfil,"\\'");
		break;
	    case '\"':
	        fprintf(outfil,"\\\"");
		break;
	    case '\b':
	        fprintf(outfil,"\\b");
		break;
	    case '\f':
	        fprintf(outfil,"\\f");
		break;
/* avoid funny interpretations of \n, \r by MPW */
	    case '\012':
	        fprintf(outfil,"\\012"); perline++;
		break;
	    case '\015':
	        fprintf(outfil,"\\015"); perline++;
		break;
	    case '\t':
	        fprintf(outfil,"\\t");
		break;
	    default:
		putc(b,outfil); perline--;
		break;
	}
	putc('\'',outfil);
	if (*c != '\0') {
	    if (++perline >= MAX_PER_LINE) {
		fprintf(outfil,",\n     ");
		outlineno++;
	    }
	    else {putc(',',outfil);}
	    perline %= MAX_PER_LINE;
	}
    }
}


int PSWHexStringLength(s) char *s; {
    return ((int) (strlen(s)+1)/2);
}

PSWOutputHexStringChars(s)
    register char *s;
{
    register int perline = 0;
    char tmp[3];

    tmp[2] ='\0';
    while ((tmp[0] = *s++)!= '\0') {
	tmp[1] = *s ? *s++ : '\0';
	fprintf(outfil,"0x%s",tmp);
	if (*s != '\0') {
	    if (++perline >= MAX_PER_LINE) {
		fprintf(outfil,",\n     ");
		outlineno++;
	    }
	    else {putc(',',outfil);}
	    perline %= MAX_PER_LINE;
	}
    } /* while */
}
