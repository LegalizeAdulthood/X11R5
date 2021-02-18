/* $Header: /usr3/Src/emu/misc/RCS/strutils.c,v 1.3 90/10/19 00:21:55 jkh Exp Locker: jkh $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * General string-oriented utility functions.
 *
 * Author: Jordan K. Hubbard
 * Date: April 23rd, 1990.
 * Description: Various "general purpose" string functions to support
 *		xterm+
 *
 * Revision History:
 *
 * $Log:	strutils.c,v $
 * Revision 1.3  90/10/19  00:21:55  jkh
 * Added strcomp().
 * 
 * Revision 1.2  90/07/26  02:46:32  jkh
 * Added new copyright.
 * 
 * Revision 1.1  90/06/15  00:30:31  jkh
 * Initial revision
 * 
 *
 */

#include "common.h"

/* Return the basename of a filename */
Export String
basename(name)
String name;
{
     register String cp;
     Import String rindex();
     
     return((cp = rindex(name, '/')) ? cp + 1 : name);
}

/* malloc and conditionally zero a region */
Export Generic
malloc_and_clear(nbytes)
int nbytes;
{
     register Generic ret;

     if ((ret = (Generic)XtMalloc(nbytes)) != NULL)
	  bzero(ret, nbytes);
     return ret;
}

/* safely compare two strings; works even if one or both are NULL */
Export int
strcomp(s1, s2)
register String s1, s2;
{
     if (s1 && s2)
	  return(strcmp(s1, s2));
     else if (!s1 && !s2)
	  return 0;
     else if (!s1 && s2)
	  return -1;
     else
	  return 1;
}

/*
 * Convert backslashed constants in a string in-place (string will never
 * get longer).
 */

/* This will lose on non-ascii architectures. I don't really care. */
#define DIGIT(x) \
     (isdigit(x) ? (x) - '0' : islower(x) ? (x) + 10 - 'a' : (x) + 10 - 'A')

Export String
backslash_convert(str)
register String str;
{
     register String keep, str2;
     int val = 0;

     /*
      * Convert backslashed character forms into their native formats
      * (I.E.: "\n" and "\t" into '\n' and '\t' characters, etc).
      */
     keep = str2 = str;

     while (*str) {
	  if (*str != '\\' && *str != '^')
	       *(str2++) = *(str++);
	  else if (*str == '^') {
	       if (*(++str) == '^')
		    *(str2++) = '^';
	       else
		    *(str2++) = (islower(*str) ? *str - 96 : *str - 64);
	       ++str;
	  }
	  else switch (*(++str)) {
	  case '\\':
	       *(str2++) = '\\';
	       ++str;
	       break;

	  case 'e':
	  case 'E':
	       *(str2++) = '\033';
	       ++str;
	       break;
	       
	  case 'n':
	       *(str2++) = '\n';
	       ++str;
	       break;
	       
	  case 't':
	       *(str2++) = '\t';
	       ++str;
	       break;
	       
	  case 's':
	       *(str2++) = ' ';
	       ++str;
	       break;
	       
	  case 'r':
	       *(str2++) = '\r';
	       ++str;
	       break;
	       
	  case 'f':
	       *(str2++) = '\f';
	       ++str;
	       break;
	       
	  case 'b':
	       *(str2++) = '\b';
	       ++str;
	       break;
	       
	  case 'v':
	       *(str2++) = '\13';
	       ++str;
	       break;
	       
	  case 'z':
	       ++str;
	       break;
	       
	  case '0': case '1': case '2': case '3': case '4':
	  case '5': case '6': case '7': case '8': case '9':
	       /* Three digit octal constant */
	       if (*str >= '0' && *str <= '3'
		   && *(str + 1) >= '0' && *(str + 1) <= '7'
		   && *(str + 2) >= '0' && *(str + 2) <= '7') {
		    
		    val = (DIGIT(*str) << 6) + (DIGIT(*(str + 1)) << 3)
			 + (DIGIT(*(str + 2)));
		    
		    /* if val is zero, you may lose big */
		    *(str2++) = val;
		    str += 3;
		    break;
	       }
	       
	       /*
		* One or two digit hex constant. If two are there
		* they will both be taken. Use \z to split them up if
		* this behaviour is not desired.
		*/
	       if (*str == '0' && (*(str + 1) == 'x' || *(str + 1) == 'X')
		   && isxdigit(*(str + 2))) {
		    val = DIGIT(*(str + 2));
		    if (isxdigit(*(str + 3))) {
			 val = (val << 4) + DIGIT(*(str + 3));
			 str += 4;
		    }
		    else
			 str += 3;
		    *(str2++) = val;
		    break;
	       }
	       
	       /* Two or three decimal digits */
	       if (isdigit(*(str + 1))) {
		    val = DIGIT(*str) * 10 + DIGIT(*(str + 1));
		    if (isdigit(*(str + 2))) {
			 val = 10 * val + DIGIT(*(str + 2));
			 str += 3;
		    }
		    else
			 str += 2;
		    *(str2++) = val;
		    break;
	       }
	       
	       /* Otherwise it's a single decimal digit */
	       *(str2++) = val;
	       str++;
	       break;
	       
	  default:
	       /* default is to leave the sequence alone */
	       *(str2++) = '\\';
	       *(str2++) = *(str++);
	       break;
	  }
     }
     *str2 = '\0';
     return(keep);
}
