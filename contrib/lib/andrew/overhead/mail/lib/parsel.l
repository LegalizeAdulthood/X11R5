%{

/***********************************************************
		Copyright IBM Corporation 1991

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

***********************************************************/

/*
		Lexical analyzer for address parsing.
*/


#include <parseadd.h>
#include "parsey.h"

#define NIL	0

typedef unsigned char bool;
#define FALSE	0
#define TRUE	1

extern char *StrCopy();
bool EatComment(), GetString(), GetDomainLit();

YYSTYPE yylval;

#ifndef hp9000s300
extern char yytext[];
#endif

static char *CurrentLine, *CurrentChar, *cbuffer;

/* Chain of () comments found in address */
ADDRESS_COMMENT *yycomments, *LastComment;
extern ADDRESS_COMMENT *MakeComment();

SetNextLine(line)
    char *line;
{
    extern int ParseErrorReason;

    CurrentLine = line;
    CurrentChar = CurrentLine;
    yycomments = NIL;
    ParseErrorReason = PA_SYNTAX_ERROR;
}

#undef input
static char input()
{
    register char c;

    c = *CurrentChar++;
    if ((c & 0x80) || c == 04)	/* High-order bit is on, or it's a ^D */
	return 01;
    else
	return c;
}

#undef unput
static unput(c)
    char c;
{
    /* Only push back if not at beginning or end of line */
    if (CurrentChar > CurrentLine)
	*--CurrentChar = c;
}

#undef YYLMAX
#define YYLMAX 400

int yywrap()
{
    return 1;
}

%}

LWSP			[ \t]
ATOMCHARS		[!#-\'\*\+\-/-9=\?A-Z\^-~]
WORD			{ATOMCHARS}+
DWORD			({WORD}(\.{WORD})+)
ALIST			({WORD}({LWSP}+{WORD})+)
DHACK			(({WORD}\.+)|(\.+{WORD})|(\.\.+))

%%

\n{LWSP}+		|

{LWSP}+			;

\(			{
			  register ADDRESS_COMMENT *c;
			  char *s;

			  cbuffer = yytext+1;
			  if (!EatComment()) return BADTOKEN;
			  s = StrCopy(yytext);
			  if (s == NIL) return BADTOKEN;
			  c = MakeComment(s);
			  if (c == NIL) return BADTOKEN;
			  if (yycomments == NIL)
			      yycomments = c;
			  else
			      LastComment -> Next = c;
			  LastComment = c;
			}

\"			{
			  if (GetString()) {
			      yylval.u_string = StrCopy(yytext);
			      if (yylval.u_string == NIL)
				  return BADTOKEN;
			      else
				  return QUOTEDSTRING;
			  } else
			      return BADTOKEN;
			}

\[			{
			  if (GetDomainLit()) {
			      yylval.u_string = StrCopy(yytext);
			      if (yylval.u_string == NIL)
				  return BADTOKEN;
			      else
				  return DOMAINLIT;
			  } else
			      return BADTOKEN;
			}

({WORD}{LWSP}+)*{DHACK}	{
			  yylval.u_string = StrCopy(yytext);
			  if (yylval.u_string == NIL)
			      return BADTOKEN;
			  else
			      return DOTLIST;
			}

\<			return '<';
\>			return '>';
@			return '@';
\.			return '.';
\;			return ';';
:			return ':';
\,			return ',';
\\			return '\\';

{DWORD}			{
			  yylval.u_string = StrCopy(yytext);
			  if (yylval.u_string == NIL)
			      return BADTOKEN;
			  else
			      return DOTWORD;
			}

{ALIST}			{
			  yylval.u_string = StrCopy(yytext);
			  if (yylval.u_string == NIL)
			      return BADTOKEN;
			  else
			      return ATOMLIST;
			}

{WORD}			{
		  	  yylval.u_string = StrCopy(yytext);
			  if (yylval.u_string == NIL)
			      return BADTOKEN;
			  else
			      return ATOM;
			}

.			return BADTOKEN;

%%
static char *parsel_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/mail/lib/RCS/parsel.l,v 2.8 1991/09/21 15:09:11 rr2b Exp $";


static bool LWSP_char(c)
    char c;
{
    return (c == ' ' || c == '\t');
}

/* Eat a comment -- returns with closing ) as current char */

static bool EatComment()
{
    /* Just ate ( */
    for (;;) {
	register char c;

	c = input();
	switch (c) {
	    case 0:		return FALSE;
	    case ')':		*cbuffer++ = ')';
				*cbuffer = '\0';
				return TRUE;
	    case '(':		*cbuffer++ = '(';
				if (!EatComment()) return FALSE;
				break;
	    case '\\':		*cbuffer++ = '\\';
				c = input();
				if (c == 0) return FALSE;
				*cbuffer++ = c;
				break;
	    case '\n':		c = input();
				if (!LWSP_char(c)) return FALSE;
				do { c = input(); } while (LWSP_char(c));
				unput(c);
				*cbuffer++ = ' ';
				break;
	    default:		*cbuffer++ = c;
				break;
	}
    }
}

/*
   Read a quoted string -- leaves closing quote as current char.
*/

static bool GetString()
{
    register char *next;

    /* Already seen " */
    for (next=yytext+1; ; ) {
	register char c;

	c = input();
	switch (c) {
	    case 0:	return FALSE;
	    case '"':	*next++ = '"';
			*next = '\0';
			return TRUE;
	    case '\\':	*next++ = '\\';
			c = input();
			if (c == 0) return FALSE;
			*next++ = c;
			break;
	    case '\n':	c = input();
			if (!LWSP_char(c)) return FALSE;
			do { c = input(); } while (LWSP_char(c));
			*next++ = ' ';
			unput(c);
			break;
	    default:	*next++ = c;
			break;
	}
    }
}

/*
   Read a domain literal leaves closing quote as current char.
*/

static bool GetDomainLit()
{
    register char *next;

    /* Already seen [ */
    for (next=yytext+1; ; ) {
	register char c;

	c = input();
	switch (c) {
	    case 0:
	    case '[':		return FALSE;
	    case ']':		*next++ = ']';
				*next = '\0';
				return TRUE;
	    case '\\':		c = input();
				if (c == 0) return FALSE;
				*next++ = c;
				break;
	    case '\n':		c = input();
				if (!LWSP_char(c)) return FALSE;
				do { c = input(); } while (LWSP_char(c));
				*next++ = ' ';
				unput(c);
				break;
	    default:		*next++ = c;
				break;
	}
    }
}
