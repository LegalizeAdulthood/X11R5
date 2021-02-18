
# line 1 "nesscomp.z"
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v 1.3 1991/09/12 16:26:08 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
	char *ness_gra_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v 1.3 1991/09/12 16:26:08 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* ness.gra

	Grammar for the ness string expression language
*/

#include <stdio.h>

#include <andrewos.h>	/* for bcopy */
#include <nodeclss.h>
#include <nessmark.ih>
#include <nesssym.ih>
#include <ness.ih>
#include <lexdef.ih>
#include <lex.ih>
#include <interp.h>		/* for gen.h */
#include <compdefs.h>
#include <gen.h>
#include <call.h>
#include <error.h>
#include <envt.h>		/* for integer constants */
#include <nevent.h>

# define OR 257
# define AND 258
# define NOT 259
# define NE 260
# define GE 261
# define LE 262
# define UNARYOP 263
# define setID 264
# define setSTRINGCON 265
# define setINTCON 266
# define setREALCON 267
# define MARKER 268
# define BOOLEAN 269
# define INTEGER 270
# define REAL 271
# define OBJECT 272
# define VOID 273
# define FUNCTION 274
# define END 275
# define ON 276
# define EXTEND 277
# define FORWARD 278
# define MOUSE 279
# define MENU 280
# define KEYS 281
# define EVENT 282
# define RETURN 283
# define WHILE 284
# define DO 285
# define IF 286
# define THEN 287
# define ELSE 288
# define ELIF 289
# define EXIT 290
# define GOTOELSE 291
# define tokTRUE 292
# define tokFALSE 293
# define tokNULL 294
# define COLONEQUAL 295
# define APPEND 296
static char *reswords[] = {
	"marker",
	"boolean",
	"integer",
	"real",
	"object",
	"void",
	"function",
	"end",
	"on",
	"extend",
	"forward",
	"mouse",
	"menu",
	"keys",
	"event",
	"return",
	"while",
	"do",
	"if",
	"then",
	"else",
	"elif",
	"exit",
	"gotoelse",
	"true",
	"false",
	"null",
	"and",
	"or",
	"not",
	NULL
};
static short restoks[] = {
	MARKER,
	BOOLEAN,
	INTEGER,
	REAL,
	OBJECT,
	VOID,
	FUNCTION,
	END,
	ON,
	EXTEND,
	FORWARD,
	MOUSE,
	MENU,
	KEYS,
	EVENT,
	RETURN,
	WHILE,
	DO,
	IF,
	THEN,
	ELSE,
	ELIF,
	EXIT,
	GOTOELSE,
	tokTRUE,
	tokFALSE,
	tokNULL,
	AND,
	OR,
	NOT,
};
static char *thongs[] = {
	"/=",
	">=",
	"<=",
	"~:=",
	":=",
	NULL
};
static short thongtoks[] = {
	NE,
	GE,
	LE,
	APPEND,
	COLONEQUAL,
};
static struct lexdescription lexdescription = {
	(unsigned char **)reswords, restoks,
	(unsigned char **)thongs, thongtoks,
	setID, setINTCON, setREALCON, setSTRINGCON
};

	struct lexdescription *
nesscomp_lexdesc()
{
	return &lexdescription;
}
short yyexca[] ={
-1, 1,
	0, -1,
	-2, 0,
-1, 3,
	274, 10,
	-2, 2,
-1, 99,
	41, 118,
	-2, 119,
-1, 100,
	41, 40,
	-2, 10,
-1, 172,
	275, 72,
	-2, 46,
-1, 182,
	275, 74,
	-2, 46,
-1, 186,
	275, 72,
	-2, 46,
	};
# define YYNPROD 127
# define YYLAST 414
short yyact[]={

  53, 141,  24,  48,  53,  47, 168,  48,  82,  47,
 102, 145, 105, 103, 178,  30,  28,  29,  31, 173,
 107, 146, 106, 175, 176,  63,  64,  63,  64,  30,
  28,  29,  31, 167, 108, 123, 144,  13,  14,  15,
  16,  17,  21,  18,  64,  22,  19,  13,  14,  15,
  16,  17,  12,  38,  25, 184,  26, 166,   6, 161,
  32, 137,  39,  36,  63,  64,   8, 113,   6,  11,
  67,  78,  68,  23, 174,  20,  49,  54,   7,  45,
  44,  77,  67,  43,  68,  34,  75,  70,  69,  71,
 136,  76, 133, 135, 132,  83,  82,   3, 104, 149,
 116,  99,  81,  95,  80,  79,  87,  86,  65,  27,
 185, 180, 179, 169, 148, 165, 147, 142, 154, 153,
 152,  33,  85, 109,  62, 101,  37, 150, 100,  60,
  35,   4,   2, 115, 114,  61,  41,  46,  55,  66,
 159, 122, 118, 117,   1,   0,   0,   0,  90,  91,
  89,   0,   0,   0,   0,  92,  93,  94,   0,  96,
  97,   0,   0,   0,   0,   0,   0, 119,   0,   0,
 120,   0,   0,   0,   0, 112,   0,   0,   0, 131,
   0,   0,   0,   0,   0,   0, 126, 124, 125,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0, 140,
   0,   0,   0, 151,   0,   0, 120,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  42,
   0,   0,   0,   0,  56,  57,  58,  59,  56,  57,
  58,  59,  40,   0, 121,   0,   0,   0,   0,   0,
  56,   0,   0,   0,  13,  14,  15,  16,  17,   0,
 143,   0,  50,  51,  52,   5,  50,  51,  52, 127,
 129, 187, 130, 138, 139,   5, 128,  13,  14,  15,
  16,  17,  12, 160,  84,  10,   9,  13,  14,  15,
  16,  17,  12,  63,  64,  10,   9,  72,  74,  73,
   0, 171, 172, 170,   0,   0,   0,   0,  88, 177,
   0,   0, 182,   0, 181,   0,   0,   0, 186,   0,
   0,   0,   0,   0,  98,   0,   0,   0,   0, 110,
 111,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0, 134,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0, 155,   0,   0,   0,   0,
 156, 157, 158,   0,   0, 162, 163, 164,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0, 183 };
short yypact[]={

-1000,-1000,-1000,   9,-1000,-231,-1000,-293,-220,-209,
-250,-204,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-1000,
-1000,-1000,-250,  41,-1000,-201,-1000,-212,-1000,-1000,
-1000,-1000,-1000,-1000,-202, -40,-1000,-1000,-1000,-1000,
-193,-1000,-1000,  27,  44, -55,-1000,-1000,-1000,-1000,
-1000,-1000,-1000,-1000,  56,-1000,-1000,-1000,-1000,-1000,
  55,  -1,-1000,-1000,-1000, -40, -36, -36, -36,-1000,
-1000,-1000,-1000,-1000,-1000, -36, -36, -36,-1000, -36,
 -36, -40,-1000,-1000,-264,-241, -40, -40,-1000,  39,
  44,  44, -55, -55, -55, -36, -55, -55,  26,-1000,
-221,-1000,-1000,-1000,-1000,-1000,-1000,-1000,-264, -24,
-214,-1000,-1000,-1000,  53,  48, -40,  52,  46,-203,
-1000,-1000,-1000,-1000, -32,-294,-1000, -23,-263,-1000,
-1000,-204,-1000,-1000,-193,-1000,-221,-1000,-1000,-1000,
  41,-1000, -40,-1000,-1000,-1000,-1000, -40, -40, -40,
-1000,-205, -40, -40, -40,-193,-193,-230,-193,-1000,
-242,-1000,-193,-193,-193,-279,-1000,-264,-1000,-1000,
-1000,-256,-265,-264,-261,-1000,-1000,-1000,-264,-1000,
 -40,-1000,-1000,-232,-1000,-1000,-265,-1000 };
short yypgo[]={

   0, 144, 143, 142, 122, 141,  74,  78,  73, 140,
  98, 125,  69, 139,  66,  77, 138, 137, 232, 136,
  83,  80,  79, 134, 133,  76, 132,  97, 131, 130,
 129, 128, 127, 126, 124, 123, 120, 119, 118, 117,
 116, 115, 114, 113, 112, 111, 110, 108, 107, 106,
 105, 104, 103, 102, 101, 100,  99 };
short yyr1[]={

   0,  26,   1,  27,  27,  12,  12,  12,  12,  12,
  14,  14,  14,  10,  10,  10,  10,  11,  11,  11,
  11,  11,  11,  28,  28,  28,  28,  28,  28,  28,
  29,  28,  30,  31,  32,  28,  33,  28,  34,  28,
   2,   2,   3,   3,   9,   4,  35,   4,   5,  36,
   5,  37,   5,   5,  38,   5,   5,  39,   5,   5,
   5,   5,  40,  41,   5,   5,  42,  43,   5,   7,
   8,   8,   6,  44,   6,  45,  46,   6,  13,  13,
  13,  13,  13,  13,  18,  47,  18,  48,  18,  49,
  18,  19,  19,  20,  20,  20,  21,  21,  21,  21,
  22,  50,  22,  51,  22,  22,  22,  22,  22,  52,
  22,  53,  22,  17,  17,  54,  25,  23,  23,  55,
  24,  56,  24,  15,  16,  16,  16 };
short yyr2[]={

   0,   0,   2,   0,   2,   1,   1,   1,   1,   1,
   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,
   1,   1,   1,   2,   2,   2,   2,   3,   1,   2,
   0,   4,   0,   0,   0,  10,   0,   6,   0,   7,
   0,   1,   2,   4,   3,   0,   0,   3,   1,   0,
   4,   0,   4,   2,   0,   4,   1,   0,   3,   2,
   2,   2,   0,   0,   8,   2,   0,   0,   9,   2,
   0,   3,   0,   0,   3,   0,   0,   7,   1,   1,
   1,   1,   1,   1,   1,   0,   3,   0,   4,   0,
   4,   1,   3,   1,   3,   3,   1,   3,   3,   3,
   1,   0,   3,   0,   3,   1,   1,   1,   1,   0,
   4,   0,   4,   1,   1,   0,   5,   1,   0,   0,
   2,   0,   4,   1,   1,   1,   1 };
short yychk[]={

-1000,  -1, -26, -27, -28, 256,  59,  -7, -14, 277,
 276, -12, 273, 268, 269, 270, 271, 272, 274, 277,
 -12, 273, 276,  -8, 295, 274, 265, -10, 280, 281,
 279, 282, 264, -10,  44, -29, 264, -33, 265, 264,
 -18, -19, 259, -20, -21, -22, -17,  45,  43, -25,
 292, 293, 294,  40, -15, -16, 264, 265, 266, 267,
 -30, -27, -34, 257, 258, -47, -13,  43,  45,  61,
  60,  62, 260, 262, 261,  42,  47,  37, 126, -50,
 -51, -53,  40,  40, 275,  -4, -48, -49, -18, -20,
 -21, -21, -22, -22, -22, -52, -22, -22, -18, -54,
 -31, -11, 274, 277, -10, 276, 286, 284, 275, -35,
 -18, -18, -22,  41, -23, -24, -55,  -2,  -3, -14,
 -12, -11,  -5,  59, -15,  -7, -25, 283, 290, 284,
 286, -12,  41,  44, -18,  41,  44, 264, 295, 296,
  -8, 295, -39, 273,  59, 274, 284, -40, -42, -56,
 -32, -14, -36, -37, -38, -18, -18, -18, -18,  -9,
  -4, 264, -18, -18, -18, -41, 287, 275, 285, -43,
 -11,  -4,  -4, 275,  -6, 288, 289, -11, 275, -44,
 -45, -11,  -4, -18, 287, -46,  -4,  -6 };
short yydef[]={

   1,  -2,   3,  -2,   4,   0,  28,  70,   0,   0,
   0,  12,  11,   5,   6,   7,   8,   9,  23,  24,
  25,  26,   0,  29,  30,   0,  36,   0,  13,  14,
  15,  16,  69,  27,   0,   0,  32,   3,  38,  71,
  31,  84,  85,  91,  93,  96, 100, 101, 103, 105,
 106, 107, 108, 111, 113, 114, 123, 124, 125, 126,
   0,  10,  45,  87,  89,   0,   0,   0,   0,  78,
  79,  80,  81,  82,  83,   0,   0,   0, 109,   0,
   0,   0, 115,  33,   0,  46,   0,   0,  86,  92,
  94,  95,  97,  98,  99,   0, 102, 104,   0,  -2,
  -2,  37,  17,  18,  19,  20,  21,  22,   0,   0,
  88,  90, 110, 112,   0, 117,   0,   0,  41,   0,
  12,  39,  47,  48,   0,  70,  56,  57,   0,  62,
  66,   0, 116, 121, 120,  34,  10,  42,  49,  51,
  53,  54,   0,  59,  61,  60,  65,   0,   0,   0,
  45,   0,   0,   0,   0,  58,  63,   0, 122,  35,
  46,  43,  50,  52,  55,   0,  67,   0,  45,  45,
  44,  46,  -2,   0,   0,  73,  75,  64,   0,  45,
   0,  68,  -2,   0,  76,  45,  -2,  77 };

# line 3 "yaccpar"
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1990 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v 1.3 1991/09/12 16:26:08 bobg Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
char *yaccpar_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/objects/RCS/nesscomp.c,v 1.3 1991/09/12 16:26:08 bobg Exp $";
#endif

/*

	Ness version of yaccpar 

	Parser utilizing the tables generated by yacc

adapted by revision from code with this notice:
 * 5799-WZQ (C) COPYRIGHT = NONE
 * LICENSED MATERIALS - PROPERTY OF IBM
 *
 * Source: /ibm/acis/usr/src/usr.bin/yacc/RCS/yaccpar,v 

however, the original code is a lot like the yaccpar on most systems,
so it is more likely owned by AT&T, if anybody

For that matter, this version is substantially revised from any other, 
including the addition of an exception table index, the elimination of goto's,
	and the fact that no variables are global.
*/

/* semantics routines can reset parser state with these macros */
# define YYERROR goto yyerrlab
# define YYACCEPT return(0)
# define YYABORT return(1)
# define yyclearin yychar = -1  /* force read next token */
# define yyerrok yyerrflag = 0	/* force finish error scan mode */

/* constants used in the syntax tables to flag errors */
# define YYERRCODE 256
# define YYFLAG -1000

# ifndef YYMAXDEPTH
# define YYMAXDEPTH 200
# endif

# ifdef YYDEBUG
int yydebug = 0; /* 1 for debugging */
# endif

yyparse() 
{
	YYSTYPE yylval;		/* side return from yylex
			(with lex.h, yylex() is redefined to pass &yylval to lex.c) */
	short yys[YYMAXDEPTH];	/* state stack */
	YYSTYPE yyv[YYMAXDEPTH]; /* store value for each stacked state */

	int yychar = -1; 	/* current input token number */
	int yynerrs = 0; 	/* number of errors */
	short yyerrflag = 0;  	/* error recovery: assume recovered when this is 
		decremented back to zero once for each successfully accepted token */

	register YYSTYPE *yypvt;	/* pt to value stack for semantics routines */
	register short yystate;	/* current state */
	YYSTYPE yyval;		/* current token value */
	register short *yyps;		/* pt to state stack */
	register short yyn;		/* temporary */
	register YYSTYPE *yypv;	/* pt to value stack */
	register short *yyxi;		/* temporary */
	short yyj, yym;		/* temporaries */
	static short **yyxx = NULL;	/* exception table index */

	yystate = 0;	/* bottom state */
	/* yyval = not initialized */
	yynerrs = 0;
	yyps= yys-1;	/* &yys[-1] */
	yypv= yyv-1;	/* &yyv[-1] */

	if (yyxx == NULL) {
		/* build exception table index */
		yyn = 0;	/* find maximum state number */
		for ( yyxi = yyexca; yyxi < yyexca + sizeof(yyexca)/sizeof(short);
				yyxi+=2 )
			if ( *yyxi == -1 && yyxi[1] > yyn )
				yyn = yyxi[1];
		yyxx = (short **)malloc((yyn+1)*sizeof(short *)); /* allocate table */
		for ( yyxi = yyexca; yyxi < yyexca + sizeof(yyexca)/sizeof(short);
				yyxi+=2 )
			if ( *yyxi == -1 )
				yyxx[yyxi[1]] = yyxi+2;  /* point to 1st entry */
	}

while (1) {

	/* in each cycle around this main loop we push a state, and then
	 generate the next state by shift, reduce, or error processing */

	/* put a state and value onto the stack */

		/* ( the following statement had an error:  > instead of >=
			this would have allowed use of the illegal element
			yys[YYMAXDEPTH] ) */
	if( ++yyps >= &yys[YYMAXDEPTH] ) { 
		yyerror( "yacc stack overflow" ); 
		return(1);
	}
	*yyps = yystate;
	*++yypv = yyval;

	/* process the new state */
	yyn = yypact[yystate];

	if( yyn > YYFLAG ) {
		/* check for shift */

		/* read next token */
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar=0;
		yyn += yychar;
		if( yyn >= 0 && yyn < YYLAST 
				&& yychk[ yyn = yyact[ yyn ] ] == yychar ) { 
			/* valid shift */

#			ifdef YYDEBUG
			if( yydebug ) printf( " char 0%o causes shift to state %d\n",
					yychar, yyn );
#			endif

			yychar = -1;	/* absorb current token */
			yyval = yylval;
			yystate = yyn;
			if( yyerrflag > 0 ) --yyerrflag;
			continue;	/* go stack the new state */
		}
	}

	/* not a shift operation: take default state action */
	yyn = yydef[yystate];
	if( yyn == -2 ) {
		/* be sure we have a token to look at */
		if( yychar<0 ) if( (yychar=yylex())<0 ) yychar = 0;

		/* look through exception table */
		for ( yyxi = yyxx[yystate];  *yyxi >= 0; yyxi += 2 )
			if( *yyxi == yychar ) break;
		yyn = yyxi[1];
		if( yyn < 0 ) return(0);   /* accept */
	}

	if( yyn == 0 ) {
		/* error ... attempt to resume parsing */

		switch( yyerrflag ) {

		case 0:   /* brand new error */

			yyerror( "syntax error" );
	yyerrlab:
			++yynerrs;
			/* DROP THROUGH */

		case 1:
		case 2: /* incompletely recovered error ... try again */

			yyerrflag = 3;  /* must accept 3 tokens
				before restarting scan */

			/* find a state where "error" is a legal shift action */

			while ( TRUE ) {
			   if ( yyps < yys )	/* reached stack bottom */
				return (1);	/* abort */
			   yyn = yypact[*yyps] + YYERRCODE;
			   if( yyn >= 0 && yyn < YYLAST 
					&& yychk[yyact[yyn]] == YYERRCODE )
				break; /* found a state where "error" is valid */
#			   ifdef YYDEBUG
			   if( yydebug ) printf( "error recovery pops state %d, uncovers %d\n", 
				*yyps, yyps[-1] );
#			   endif

			   --yyps;  /* pop stack */
			   --yypv;
			}
			yystate = yyact[yyn];  /* simulate a shift of "error" */
			continue;	/* go stack the error state */

		case 3:  /* no shift yet; clobber input char */

#			ifdef YYDEBUG
			if( yydebug ) printf( "error recovery discards char %d\n", yychar );
#			endif

			if( yychar == 0 ) 
				return(1); /* don't discard EOF, abort */
			yychar = -1;
			/* try again in the same state */
			yystate = *yyps--;
			yyval = *yypv--;
			continue;	/* go push the state we just popped */
		} /* end switch ( yyerrflag ) */
	} /* end if ( yyn == 0 ) */

	/* reduction by production yyn */

	yym = yyn;	/* save yyn for the case operand below */

	/* pop off the states and values for righthand side of rule */
	yyj = yyr2[yyn];
	yyps -= yyj;
	yypvt = yypv;   /* for semantics: point to values for right-hand-side */
	yypv -= yyj;
	yyval = yypv[1];	/* default value is first non-terminal on the right */

	/* consult goto table to find next state */
	yyn = yyr1[yyn];
	yyj = yypgo[yyn] + *yyps + 1;
	if( yyj >= YYLAST || yychk[ yystate = yyact[yyj] ] != -yyn ) 
		yystate = yyact[yypgo[yyn]];
	/* yystate is set here. yyval will be set in semantics routines */

#	ifdef YYDEBUG
	if( yydebug ) printf("reduce by production %d   new state is %d\n",yym, yystate);
#	endif

	switch ( yym ) {

/* semantics rules follow */
			
case 1:
# line 97 "nesscomp.z"
{ /* this code creates the function for the initialization */
			   boolean junk;
			   curNess->InitFunc  =  nesssym_NLocate("init*", 
					lexdef_GetPrototype(curLex->def),
					lex_GetScope(curLex), &junk);
			   curNess->InitFunc->flags =  flag_function | flag_ness;
			   curNess->InitFunc->type = Tfunc;
			   curNess->InitFunc->parent.ness = curNess;
			   startfunc(curNess->InitFunc); 
			   nesssym_NGetINode(curNess->InitFunc, funcnode)->functype 
			   		= Tvoid; 
			   genSaveFuncState();
			} break;
case 2:
# line 111 "nesscomp.z"
{ /* this code terminates the function for the initialization */
			   genRestoreFuncState(curNess->InitFunc);
			   finishfunc(curNess->InitFunc, NULL); 
			} break;
case 5:
# line 129 "nesscomp.z"
{ yyval.i = Tstr; genSaveStmtStart(0); } break;
case 6:
# line 131 "nesscomp.z"
{ yyval.i = Tbool; genSaveStmtStart(0); } break;
case 7:
# line 133 "nesscomp.z"
{ yyval.i = Tlong; genSaveStmtStart(0); } break;
case 8:
# line 135 "nesscomp.z"
{ yyval.i = Tdbl; genSaveStmtStart(0); } break;
case 9:
# line 137 "nesscomp.z"
{ yyval.i = Tptr; genSaveStmtStart(0); } break;
case 10:
# line 143 "nesscomp.z"
{ yyval.i = Tstr; } break;
case 11:
# line 145 "nesscomp.z"
{ yyval.i = Tvoid; } break;
case 23:
# line 172 "nesscomp.z"
{ if (isFuncStart(yypvt[-0].s, TRUE, 2)) {
				abortfunc(); 
				errsynch(0, &yychar, &yyerrflag);
			    } else YYERROR; } break;
case 24:
# line 178 "nesscomp.z"
{ if (isFuncStart(yypvt[-0].s, TRUE, 2)) {
				/* XXX ought to close existing EXTEND, if any */
				abortfunc(); 
				errsynch(0, &yychar, &yyerrflag);
			    } else YYERROR; } break;
case 25:
# line 185 "nesscomp.z"
{ if (isFuncStart(yypvt[-0].i, FALSE, 3)) {
				abortfunc(); 
				errsynch(0, &yychar, &yyerrflag);
			    } else YYERROR; } break;
case 26:
# line 191 "nesscomp.z"
{ struct sym *tok, *curtok;
			   /* if this is part ot /return void/ we remain in error state;
				otherwise, it may be a global declaration
				which would signal the end of error state */
			   lex_Repeat(-1);  lex_NextToken(&tok); lex_NextToken(&curtok);
			   if (strcmp(sym_GetName(tok), "return") == 0) YYERROR;
			   if (isFuncStart(yypvt[-0].s, FALSE, 3)) {
				abortfunc(); 
				errsynch(0, &yychar, &yyerrflag);
			    } else YYERROR; } break;
case 27:
# line 204 "nesscomp.z"
{ /* XXX should we open an EXTEND, if none ??? */
			   /* we accept the ON only if it is the first item on its line */
			    if (lex_RecentIndent(-1) < 999) {
				abortfunc(); 
				errsynch(-1, &yychar, &yyerrflag);
			    }   else YYERROR; } break;
case 29:
# line 215 "nesscomp.z"
{ struct nesssym *id, *nid;
			   yypvt[-1].s->next = yypvt[-0].s;
			   ProcessIdList(yypvt[-1].s->type, yypvt[-1].s, flag_var | flag_globalvar); 
			   for (id = yypvt[-1].s; id != NULL; id = nid) {
				nid = id->next;	/* genLinkGlobal changes the link */
				genLinkGlobal(id);
			   	nesssym_NSetInfo(id, TSysMarkRef, makeGlobal()); 
					/* XXX makeGlobal() may not be right 
					for all types */
			   }} break;
case 30:
# line 228 "nesscomp.z"
{ yypvt[-1].s->flags = flag_var | flag_globalvar;
			   genLinkGlobal(yypvt[-1].s);
			   nesssym_NSetInfo(yypvt[-1].s, TSysMarkRef, makeGlobal()); 
					/* XXX makeGlobal() may not be right 
					for all types */
			   yyval.varnode = genvarnode(yypvt[-1].s);
			   genSaveStmtStart(-2);
			   genRestoreFuncState(curNess->InitFunc);
			   predpush(FALSE, yyval.varnode->loc+yyval.varnode->len, 'X'); } break;
case 31:
# line 238 "nesscomp.z"
{ predvalue(&yypvt[-0].exprnode->type);  predpop(); 
			   demandnodetype(yypvt[-0].exprnode, ((yypvt[-1].varnode)->sym)->type);
			   exprnode_Destroy(yypvt[-0].exprnode);
			   genvarstore(yypvt[-1].varnode); 
			   genSaveFuncState(); } break;
case 32:
# line 247 "nesscomp.z"
{ long len;
			   if (yypvt[-0].s->flags == (flag_function | flag_forward)) {
				/* we are defining a function that was forward referenced
					discard the pointer to the callnode
					(the object code still refers to it)  */
				yypvt[-0].s->flags = flag_function | flag_ness;
				nesssym_NSetINode(yypvt[-0].s, funcnode, NULL);
				yyval.s = yypvt[-0].s;
			   }
			   else yyval.s = uniqueinscope(yypvt[-0].s, flag_function | flag_ness, 0);
			   yyval.s->type = Tfunc;
			   if (curNess->CurrentObject == NULL) 
				yyval.s->parent.ness = curNess;
			   else {
				yyval.s->parent.nesssym = curNess->CurrentObject;
				yyval.s->flags |= flag_xfunc;
			   }
			   genSaveStmtStart(-1);
			   predpush(predcond, lex_RecentPosition(-2, &len), 'L');
			   startfunc(yyval.s); 
			   nesssym_NGetINode(yyval.s, funcnode)->functype 
						= yypvt[-2].i; } break;
case 33:
# line 270 "nesscomp.z"
{ yyval.varnode = varnode_Create(lex_RecentPosition(0,0),
				0, yypvt[-1].s, (struct toksym *)yypvt[-0].s); } break;
case 34:
# line 274 "nesscomp.z"
{ parencheck(yypvt[-2].varnode, yypvt[-0].s);
			   yyval.s = yypvt[-2].varnode->sym;
			   varnode_Destroy(yypvt[-2].varnode);
			   nesssym_NGetINode(yyval.s, funcnode)->parmlist
						= yypvt[-1].s; } break;
case 35:
# line 280 "nesscomp.z"
{ predpop(); finishfunc(yypvt[-1].s, yypvt[-0].s); } break;
case 36:
# line 284 "nesscomp.z"
{ long len;
			   genSaveStmtStart(-1);
			   predpush(predcond, lex_RecentPosition(-1, &len), 'E');
			   yyval.s = neventStartExtend(curNess->CurrentObject, yypvt[-0].s);
			   curNess->saveAttrDest = curNess->AttrDest;
			   curNess->AttrDest = &(nesssym_NGetINode(yyval.s, objnode)->attrs);
			   curNess->CurrentObject = yyval.s;
			   curNess->CurrentObject->parent.ness = curNess;  } break;
case 37:
# line 295 "nesscomp.z"
{ genCheckEndtag(yypvt[-0].s, EXTEND);
			   neventFinishExtend(yypvt[-3].s);  predpop();
			   curNess->AttrDest = curNess->saveAttrDest;
			   curNess->CurrentObject = NULL; } break;
case 38:
# line 303 "nesscomp.z"
{ long len;
			   genSaveStmtStart(-2);
			   predpush(predcond, lex_RecentPosition(-1, &len), 'V');
			   yyval.s = neventStartEvent(curNess->CurrentObject, yypvt[-1].s, yypvt[-0].s); } break;
case 39:
# line 310 "nesscomp.z"
{ genCheckEndtag(yypvt[-0].s, ON);
			   yypvt[-3].s = neventFinishEvent(yypvt[-3].s, yypvt[-2].s, yypvt[-0].s);   predpop();
			   yypvt[-3].s->parent.nesssym = curNess->CurrentObject; } break;
case 40:
# line 318 "nesscomp.z"
{ yyval.s = NULL; } break;
case 42:
# line 327 "nesscomp.z"
{ yyval.s = uniqueinscope(yypvt[-0].s,  flag_var | flag_parmvar, 0);  
			   yyval.s->type = yypvt[-1].i; } break;
case 43:
# line 334 "nesscomp.z"
{ yyval.s = uniqueinscope(yypvt[-0].s,  flag_var | flag_parmvar, 0);
			   yyval.s->type = yypvt[-1].i;
			   yyval.s->next = yypvt[-3].s; } break;
case 44:
# line 343 "nesscomp.z"
{ genCheckEndtag(yypvt[-0].s, FUNCTION);  yyval.s = yypvt[-2].s; } break;
case 45:
# line 353 "nesscomp.z"
{ yyval.s = NULL; } break;
case 46:
# line 356 "nesscomp.z"
{ genSaveStmtStart(0);
			   yyval.s = yypvt[-0].s; } break;
case 47:
# line 359 "nesscomp.z"
{ yyval.s = appendlists(yypvt[-1].s, yypvt[-0].s); } break;
case 48:
# line 365 "nesscomp.z"
{ yyval.s = NULL; } break;
case 49:
# line 369 "nesscomp.z"
{  yyval.varnode = varIsStorable(yypvt[-1].varnode);
			   predpush(FALSE, yyval.varnode->loc+yyval.varnode->len, 'X'); } break;
case 50:
# line 372 "nesscomp.z"
{ predvalue(&yypvt[-0].exprnode->type);  predpop(); 
			   demandnodetype(yypvt[-0].exprnode, yypvt[-1].varnode->sym->type);
			   exprnode_Destroy(yypvt[-0].exprnode);
			   genvarstore(yypvt[-1].varnode);  yyval.s = NULL; } break;
case 51:
# line 379 "nesscomp.z"
{ yyval.varnode = varIsStorable(yypvt[-1].varnode); 
			   demandsymboltype(yypvt[-1].varnode->sym, Tstr);
			   predpush(predcond, yypvt[-1].varnode->loc, 'X');	/* save loc for runtime error */
			   genvarref(yypvt[-1].varnode->sym);
			   predpop(); } break;
case 52:
# line 385 "nesscomp.z"
{ demandnodetype(yypvt[-0].exprnode, Tstr);
			   if (curNess->accesslevel >= ness_codeYellow)
			 	genop('A');	/* APPEND */
			   else {
				long loc, len;
				loc = yypvt[-1].varnode->loc + yypvt[-1].varnode->len;
				len = yypvt[-0].exprnode->loc - loc;
				SaveError(":Text append may modify a file", loc, len);
				genop('y');	/* pop, leave unchanged original */
			   }
			   exprnode_Destroy(yypvt[-0].exprnode);
			   genvarstore(yypvt[-1].varnode);  
			   yyval.s = NULL; } break;
case 53:
# line 401 "nesscomp.z"
{ yyval.s = yypvt[-1].s; yyval.s->next = yypvt[-0].s; 
			ProcessIdList(yyval.s->type, yyval.s, flag_var | flag_localvar);  } break;
case 54:
# line 406 "nesscomp.z"
{ yyval.varnode = genvarnode(yypvt[-1].s);
			   ProcessIdList(yypvt[-1].s->type, yyval.varnode->sym, 
					flag_var | flag_localvar); 
			   varIsStorable(yyval.varnode);
			   predpush(FALSE, yyval.varnode->loc+yyval.varnode->len, 'X'); } break;
case 55:
# line 412 "nesscomp.z"
{ predvalue(&yypvt[-0].exprnode->type);  predpop(); 
			   demandnodetype(yypvt[-0].exprnode, yypvt[-1].varnode->sym->type);
			   exprnode_Destroy(yypvt[-0].exprnode);
			   yyval.s = yypvt[-1].varnode->sym;  /* access before destroy varnode */
			   genvarstore(yypvt[-1].varnode); } break;
case 56:
# line 419 "nesscomp.z"
{ if (yypvt[-0].exprnode->type != Tvoid)
				genop('y');  /* POP - discardvalue  */
			   exprnode_Destroy(yypvt[-0].exprnode);
			   yyval.s = NULL; } break;
case 57:
# line 425 "nesscomp.z"
{ long loc, len;  loc = lex_RecentPosition(0, &len);
			   predpush(FALSE, loc + len, 'X'); } break;
case 58:
# line 428 "nesscomp.z"
{ predvalue(&(yypvt[-0].exprnode->type));  predpop(); 
			   genreturn(TRUE); 
			   exprnode_Destroy(yypvt[-0].exprnode);
			   yyval.s = NULL; } break;
case 59:
# line 435 "nesscomp.z"
{ genreturn(FALSE); yyval.s = NULL; } break;
case 60:
# line 439 "nesscomp.z"
{ genreturn(FALSE); yyval.s = NULL; } break;
case 61:
# line 444 "nesscomp.z"
{ genreturn(FALSE); yyval.s = NULL; } break;
case 62:
# line 447 "nesscomp.z"
{ long loc, len;  loc = lex_RecentPosition(0, &len);
			   yyval.predstatenode = predpush(FALSE, loc + len, 'W'); } break;
case 63:
# line 450 "nesscomp.z"
{ predbranch(yypvt[-0].exprnode);  predfixdropthru(); 
			   exprnode_Destroy(yypvt[-0].exprnode);
			   yyval.predstatenode = yypvt[-1].predstatenode; } break;
case 64:
# line 457 "nesscomp.z"
{ genCheckEndtag(yypvt[-0].s, WHILE);
			   genbranch('g', yypvt[-4].predstatenode->objloc);
			   predfixtarget();  predpop();
			   yyval.s = yypvt[-2].s; } break;
case 65:
# line 464 "nesscomp.z"
{ predexit('W');  yyval.s = NULL; } break;
case 66:
# line 467 "nesscomp.z"
{ long loc, len;  loc = lex_RecentPosition(0, &len);
			   predpush(FALSE, loc + len, 'I'); } break;
case 67:
# line 471 "nesscomp.z"
{ predbranch(yypvt[-1].exprnode);  predfixdropthru(); 
			   exprnode_Destroy(yypvt[-1].exprnode); } break;
case 68:
# line 477 "nesscomp.z"
{ genCheckEndtag(yypvt[-0].s, IF);
			   predpop();  yyval.s = appendlists(yypvt[-3].s, yypvt[-2].s); } break;
case 69:
# line 485 "nesscomp.z"
{ yyval.s = uniqueinscope(yypvt[-0].s, flag_var, 0); 
			   yyval.s->type = yypvt[-1].i; } break;
case 70:
# line 492 "nesscomp.z"
{ yyval.s =NULL; } break;
case 71:
# line 496 "nesscomp.z"
{ yyval.s = appendlists(yypvt[-2].s, uniqueinscope(yypvt[-0].s, flag_var, 0)); } break;
case 72:
# line 503 "nesscomp.z"
{ predfixtarget();   yyval.s = NULL; } break;
case 73:
# line 506 "nesscomp.z"
{ yyval.i = genbranch('g', 0);  predfixtarget(); } break;
case 74:
# line 508 "nesscomp.z"
{ fixbranch(yypvt[-1].i);  yyval.s = yypvt[-0].s; } break;
case 75:
# line 511 "nesscomp.z"
{ long loc, len;  int fixloc = genbranch('g', 0);
			   predfixtarget();
			   loc = lex_RecentPosition(0, &len);
			   yyval.predstatenode = predpush(FALSE, loc + len, 'I'); 
			   yyval.predstatenode->fixuploc = fixloc; } break;
case 76:
# line 518 "nesscomp.z"
{ predbranch(yypvt[-1].exprnode);  predfixdropthru(); 
			   exprnode_Destroy(yypvt[-1].exprnode); } break;
case 77:
# line 522 "nesscomp.z"
{ fixbranch(yypvt[-5].predstatenode->fixuploc);
			   predpop();  yyval.s = appendlists(yypvt[-1].s, yypvt[-0].s); } break;
case 78:
# line 530 "nesscomp.z"
{ yyval.i = predEQ; } break;
case 79:
# line 532 "nesscomp.z"
{ yyval.i = predLT; } break;
case 80:
# line 534 "nesscomp.z"
{ yyval.i = predGT; } break;
case 81:
# line 536 "nesscomp.z"
{ yyval.i = predNE; } break;
case 82:
# line 538 "nesscomp.z"
{ yyval.i = predLE; } break;
case 83:
# line 540 "nesscomp.z"
{ yyval.i = predGE; } break;
case 85:
# line 556 "nesscomp.z"
{ yyval.i = lex_RecentPosition(0, 0);
			   predcond = ! predcond; } break;
case 86:
# line 559 "nesscomp.z"
{ predbranch(yypvt[-0].exprnode);  predcond = ! predcond;  
			   yyval.exprnode = yypvt[-0].exprnode;  yyval.exprnode->type = Tbra;
			   yyval.exprnode->len += yyval.exprnode->loc - yypvt[-1].i;
			   yyval.exprnode->loc = yypvt[-1].i; } break;
case 87:
# line 566 "nesscomp.z"
{ long loc, len;
			   predbranch(yypvt[-1].exprnode); preddropthru(predOR);
			   if (predcond == FALSE) predfixtarget(); 
			   else predfixdropthru();
			   yyval.exprnode = yypvt[-1].exprnode;  
			   loc = lex_RecentPosition (0, &len);
			   predpush(predcond, loc + len, 'X'); } break;
case 88:
# line 574 "nesscomp.z"
{ predbranch(yypvt[-0].exprnode);  
			   predfixdropthru();  predpop();
			   yyval.exprnode = exprnode_Combine(yypvt[-1].exprnode, yypvt[-0].exprnode, Tbra); } break;
case 89:
# line 580 "nesscomp.z"
{ predbranch(yypvt[-1].exprnode); preddropthru(predAND);
			   if (predcond == TRUE) predfixtarget(); 
			   yyval.exprnode = yypvt[-1].exprnode; } break;
case 90:
# line 584 "nesscomp.z"
{ predbranch(yypvt[-0].exprnode);
			   yyval.exprnode = exprnode_Combine(yypvt[-1].exprnode, yypvt[-0].exprnode, Tbra); } break;
case 92:
# line 596 "nesscomp.z"
{ gencomp(yypvt[-2].exprnode, yypvt[-0].exprnode);
			   predtarget (yypvt[-1].i);
			   yyval.exprnode = exprnode_Combine(yypvt[-2].exprnode, yypvt[-0].exprnode, Tbra); } break;
case 94:
# line 606 "nesscomp.z"
{ yyval.exprnode = genarith('+', yypvt[-2].exprnode, yypvt[-0].exprnode); } break;
case 95:
# line 609 "nesscomp.z"
{ yyval.exprnode = genarith('-', yypvt[-2].exprnode, yypvt[-0].exprnode); } break;
case 97:
# line 617 "nesscomp.z"
{ yyval.exprnode = genarith('*', yypvt[-2].exprnode, yypvt[-0].exprnode); } break;
case 98:
# line 620 "nesscomp.z"
{ yyval.exprnode = genarith('/', yypvt[-2].exprnode, yypvt[-0].exprnode); } break;
case 99:
# line 623 "nesscomp.z"
{ yyval.exprnode = genarith('%', yypvt[-2].exprnode, yypvt[-0].exprnode); } break;
case 100:
# line 629 "nesscomp.z"
{ yyval.exprnode = exprnode_Create(yypvt[-0].varnode->sym->type, NULL, FALSE, 
						yypvt[-0].varnode->loc, yypvt[-0].varnode->len);
			   predpush(predcond, yypvt[-0].varnode->loc, 'X');	/* save loc for runtime error */
			   if (yypvt[-0].varnode->sym->flags & flag_const)
				genconstref(yypvt[-0].varnode->sym);
			   else
				genvarref(yypvt[-0].varnode->sym);
			   predpop();
			   varnode_Destroy(yypvt[-0].varnode); } break;
case 101:
# line 640 "nesscomp.z"
{ yyval.i = lex_RecentPosition(0, 0); } break;
case 102:
# line 642 "nesscomp.z"
{ if (yypvt[-0].exprnode->type == Tdbl) {genop('H'); genop('_');}
			   else {demandnodetype(yypvt[-0].exprnode, Tlong);  genop('_');}
			   yyval.exprnode = yypvt[-0].exprnode;   yyval.exprnode->len += yyval.exprnode->loc - yypvt[-1].i;
			   yyval.exprnode->loc = yypvt[-1].i; } break;
case 103:
# line 648 "nesscomp.z"
{ yyval.i = lex_RecentPosition(0, 0); } break;
case 104:
# line 650 "nesscomp.z"
{ yyval.exprnode = yypvt[-0].exprnode;  yyval.exprnode->len += yyval.exprnode->loc - yypvt[-1].i;
			   yyval.exprnode->loc = yypvt[-1].i; } break;
case 105:
# line 654 "nesscomp.z"
{ if (yypvt[-0].exprnode->type == Tvoid) {
				genop('0');
				yypvt[-0].exprnode->type = Tlong;
			   }
			   yyval.exprnode = yypvt[-0].exprnode; } break;
case 106:
# line 661 "nesscomp.z"
{ long loc, len;
			   loc = lex_RecentPosition(0, &len);
			   yyval.exprnode = exprnode_Create(Tbool, NULL, FALSE, loc, len);
			   genop('1'); } break;
case 107:
# line 667 "nesscomp.z"
{ long loc, len;
			   loc = lex_RecentPosition(0, &len);
			   yyval.exprnode = exprnode_Create(Tbool, NULL, FALSE, loc, len);
			   genop('9'); } break;
case 108:
# line 673 "nesscomp.z"
{ long loc, len;
			   loc = lex_RecentPosition(0, &len);
			   yyval.exprnode = exprnode_Create(Tptr, NULL, FALSE, loc, len);
			   genop('^'); } break;
case 109:
# line 680 "nesscomp.z"
{ demandnodetype(yypvt[-1].exprnode, Tstr);
			   if ( ! yypvt[-1].exprnode->IsCat) {
				genop('q');	/* NEWBASE */
				genop('z');	/* SWAP */
				genop('r');	/* REPLACE */
			   }
			   yyval.exprnode = yypvt[-1].exprnode; } break;
case 110:
# line 688 "nesscomp.z"
{ demandnodetype(yypvt[-0].exprnode, Tstr);
			   genop('A');	/* append */
			   yyval.exprnode = exprnode_Combine(yypvt[-1].exprnode, yypvt[-0].exprnode, Tstr);
			   yyval.exprnode->IsCat = TRUE; } break;
case 111:
# line 694 "nesscomp.z"
{ long loc, len;
			   loc = lex_RecentPosition(0, &len);
			   yyval.varnode = varnode_Create(loc, 0, NULL, (struct toksym *)yypvt[-0].s);
			   predpush(predcond, loc + len, 'X'); } break;
case 112:
# line 700 "nesscomp.z"
{ long loc, len;
			   parencheck(yypvt[-2].varnode, yypvt[-0].s);
			   loc = lex_RecentPosition(0, &len);
			   predfixdropthru();  predpop();
			   yypvt[-1].exprnode->len = loc+len - yypvt[-2].varnode->loc;
			   yypvt[-1].exprnode->loc = yypvt[-2].varnode->loc;  yyval.exprnode = yypvt[-1].exprnode;
			   varnode_Destroy(yypvt[-2].varnode); } break;
case 113:
# line 713 "nesscomp.z"
{ yyval.varnode = yypvt[-0].varnode; } break;
case 114:
# line 716 "nesscomp.z"
{ yyval.varnode = yypvt[-0].varnode; } break;
case 115:
# line 723 "nesscomp.z"
{ yyval.varnode = varIsFunction(yypvt[-1].varnode);
			   yyval.varnode->paren = (struct toksym *)yypvt[-0].s;
			   predpush(predcond, yyval.varnode->loc, 'A'); } break;
case 116:
# line 728 "nesscomp.z"
{ parencheck(yypvt[-2].varnode , yypvt[-0].s);
			   yyval.exprnode = callFunc(yypvt[-2].varnode, yypvt[-1].exprnode);
			   predpop(); } break;
case 117:
# line 736 "nesscomp.z"
{ yyval.exprnode = yypvt[-0].exprnode; } break;
case 118:
# line 739 "nesscomp.z"
{ yyval.exprnode = NULL; } break;
case 119:
# line 746 "nesscomp.z"
{ long loc, len;  loc = lex_RecentPosition(0, &len);
			   predpush(FALSE, loc + len, 'A'); } break;
case 120:
# line 749 "nesscomp.z"
{ predvalue(&yypvt[-0].exprnode->type);  predpop();
			   yyval.exprnode = yypvt[-0].exprnode; } break;
case 121:
# line 754 "nesscomp.z"
{ long loc, len;  loc = lex_RecentPosition(0, &len);
			   predpush(FALSE, loc + len, 'A'); } break;
case 122:
# line 757 "nesscomp.z"
{ predvalue(&yypvt[-0].exprnode->type);  predpop();
			   yyval.exprnode = yypvt[-0].exprnode;  yypvt[-0].exprnode->next = yypvt[-3].exprnode; } break;
case 123:
# line 766 "nesscomp.z"
{ yyval.varnode = genvarnode(yypvt[-0].s); } break;
case 124:
# line 772 "nesscomp.z"
{ /* there are 4 cases:  EmptyStringToken, short string,
				styled short string, and long string.  
				The EmptyStringToken has name '"'.
				A short string has a name beginning with " and
				 continuing with the \ reduced value of the string.  
				A styled short string  has the name ~ddd and
				a long string has a name /ddd, where
				ddd is an integer.  */
			   long loc, len;
			   loc = lex_RecentPosition(0, &len);
			   if ( ! (yypvt[-0].s->flags & flag_const)) 
				/* need to allocate new const */
				nesssym_NSetInfo(yypvt[-0].s, TSysMarkRef, 
					makeStyledConst(curNess, /*the text*/
						yypvt[-0].s->header.toksym.loc, 
						yypvt[-0].s->header.toksym.len,
						(*nesssym_NGetName(yypvt[-0].s) == '"'
						|| *nesssym_NGetName(yypvt[-0].s) == '~')
					));
			   yypvt[-0].s->type = Tstr;  yypvt[-0].s->flags = flag_const;
			   yyval.varnode = varnode_Create(loc, len, yypvt[-0].s, NULL); } break;
case 125:
# line 795 "nesscomp.z"
{ long loc, len;
			   if ( ! (yypvt[-0].s->flags & flag_const)) {
				/* need to allocate new const */
				TSysMarkRef g = makeGlobal();
				struct longstkelt *l;
				l = (struct longstkelt *)(&SysMarkLowEnd[g]);
				l->hdr = longHdr;
				l->v = nesssym_NGetInfo(yypvt[-0].s, long);
				nesssym_NSetInfo(yypvt[-0].s, TSysMarkRef, g);
				yypvt[-0].s->type = Tlong;  yypvt[-0].s->flags = flag_const;
			   }
			   loc = lex_RecentPosition(0, &len);
			   yyval.varnode = varnode_Create(loc, len, yypvt[-0].s, NULL); } break;
case 126:
# line 809 "nesscomp.z"
{ long loc, len;
			   if ( ! (yypvt[-0].s->flags & flag_const)) {
				/* need to allocate new const */
				double *dp;
				TSysMarkRef g = makeGlobal();
				struct dblstkelt *d;
				d = (struct dblstkelt *)(&SysMarkLowEnd[g]);
				d->hdr = dblHdr;
				dp = nesssym_NGetInfo(yypvt[-0].s, double *);
				d->v = *dp;
				nesssym_NSetInfo(yypvt[-0].s, TSysMarkRef, g);
				yypvt[-0].s->type = Tdbl;  yypvt[-0].s->flags = flag_const;
			   }
			   loc = lex_RecentPosition(0, &len);
			   yyval.varnode = varnode_Create(loc, len, yypvt[-0].s, NULL); } break;
# line 226 "yaccpar"
/* semantics rules precede */

	} /* end of switch( yym ) */

} /* end while(1) main loop */

}  /* end of Ness version of yaccpar */
