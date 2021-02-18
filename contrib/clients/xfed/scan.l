%o 15000
%a 15000

%{

/*
 * Copyright 1988 by Siemens
 *		All Rights Reserved
 *
 * Permission to use, copy, modify and distribute this software is
 * hereby granted, provided that this copyright notice appear in all
 * copies and that the copyright notice appear in supporting documentation.
 * 
 * written 16-17 Mar 1988 by claus gittinger
 *
 * atomizer for .bdf files
 *
 */
/*
 * Modified by Jeffrey Lee and Mark Moraes @ the University of Toronto
 * to make it handle a larger subset of BDF. It now reads in all the
 * font files in X.V11R3 without complaints.
 */

#include "y.tab.h"
#include "defs.h"

int lineno = 1;
int stringMode = 0;

%}

DIGIT		[0-9]
HEXDIGIT	[0-9a-fA-F]
HEX		{HEXDIGIT}+
ID              [a-zA-Z][-a-zA-Z0-9._]*
%START AA
%%

STARTFONT           {
			return STARTFONT;
		    }
SIZE                return SIZE;
FONTBOUNDINGBOX     return FONTBOUNDINGBOX;
COMMENT" ".*        {
			yylval.sval = malloc(strlen(yytext+8)+1);
			strcpy(yylval.sval, yytext+8);
			return COMMENT;
		    }

STARTPROPERTIES     {
			BEGIN AA;
			return STARTPROPERTIES;
		    }
ENDPROPERTIES" "*   {
			BEGIN 0;
			return ENDPROPERTIES;
		    }
<AA>{ID}" ".*       {
			yylval.sval = malloc(strlen(yytext)+1);
			strcpy(yylval.sval, yytext);
			return PROPERTY;
		    }

CHARS               return CHARS;
ENCODING            return ENCODING;
SWIDTH              return SWIDTH;
DWIDTH              return DWIDTH;
BBX                 return BBX;
BITMAP              return BITMAP;
ENDCHAR             return ENDCHAR;
ENDFONT             return ENDFONT;
FONT" ".*           {
			yylval.sval = malloc(strlen(yytext+5)+1);
			strcpy(yylval.sval, yytext+5);
			return FONT;
		    }
STARTCHAR" ".*      {
			yylval.sval = malloc(strlen(yytext+10)+1);
			strcpy(yylval.sval, yytext+10);
			return STARTCHAR;
		    }
[0-9]+              {
			if (stringMode) {
			    yylval.sval = malloc(strlen(yytext)+1);
			    strcpy(yylval.sval, yytext);
			    return STRING;
			} else
			    sscanf(yytext, "%d", &yylval.ival);
			return NUMBER;
		    }

-[0-9]+             {
			sscanf(yytext, "%d", &yylval.ival);
			return NUMBER;
		    }

{HEX}               {
			if (stringMode) {
			    yylval.sval = malloc(strlen(yytext)+1);
			    strcpy(yylval.sval, yytext);
			    return STRING;
			}
			sscanf(yytext, "%x", &yylval.ival);
			return NUMBER;
		    }

{ID}                {
			yylval.sval = malloc(strlen(yytext)+1);
			strcpy(yylval.sval, yytext);
			return IDENTIFIER;
		    }

\.                  return DOT;

\".*\"              {
			yylval.sval = malloc(strlen(yytext)+1);
			strcpy(yylval.sval, yytext);
			return STRING;
		    }

\t                  ;

\n                  {
			lineno++;
			return NEWLINE;
		    }


.                   ;

%%

