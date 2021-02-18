/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lextest.c,v 1.6 1989/06/01 10:09:12 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lextest.c,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ness/tokens/RCS/lextest.c,v 1.6 1989/06/01 10:09:12 wjh Exp $";
#endif

/*
lextest.c

	test the lex token scanning package
*/
/*
 *    $Log: lextest.c,v $
 * Revision 1.6  1989/06/01  10:09:12  wjh
 * revision for new securtiy system
 *
 * Revision 1.1  88/10/21  10:42:06  wjh
 * Initial revision
 *  
 * Revision 1.0  88/07/20  wjh
 * Created by WJHansen
 */


#include <stdio.h>
#include <class.h>
#include <toksym.ih>
#include <lex.ih>		/* must follow toksym.ih */
#include <lexdef.ih>
#include <text.ih>

#include <filetype.ih>
#include <attribs.h>

#define class_StaticEntriesOnly
#include <observe.ih>
#include <proctbl.ih>
#include <dataobj.ih>
#undef class_StaticEntriesOnly


struct text *text;	/* the source text */


/* yylval is used to send the symbol from lex to yacc
	yacc actually declares it as YYSTYPE 
	YYSTYPE should include toksym* as an option */
struct toksym *yylval = NULL;


/* the following is from an old grammar for ness */

# define setID 257
# define setSTRINGCON 258
# define setINTCON 259
# define setREALCON 260
# define MARKER 261
# define FUNCTION 262
# define COLONEQUAL 263
# define RETURN 264
# define WHILE 265
# define DO 266
# define IF 267
# define THEN 268
# define ELSE 269
# define tokTRUE 270
# define tokFALSE 271
# define DEFINEDAS 272
# define APPEND 273
# define LE 274
# define GE 275
# define NE 276
# define MEQ 277
# define MBEFORE 278
# define MAFTER 279

/* {{would you believe it ?!?!? On a Sun, the following has warning messages if the type
	is (unsigned char *).  Apparently a character string constant is always an
	array of signed characters.}} */
static char *symlist[] = {
	"marker",
	"function",
	"colonequal",
	"return",
	"while",
	"do",
	"if",
	"then",
	"else",
	"true",
	"false",
	0
};

static short symtok[] = {
	MARKER,
	FUNCTION,
	COLONEQUAL,
	RETURN,
	WHILE,
	DO,
	IF,
	THEN,
	ELSE,
	tokTRUE,
	tokFALSE,
};

static char *thonglist[] = {
	".>",
	".<",
	".=",
	"/=",
	">=",
	"<=",
	"~:=",
	":=",
	"==",
	0
};

static short thongtok[] = {
	MAFTER,
	MBEFORE, 
	MEQ, 
	NE, 
	GE, 
	LE, 
	APPEND, 
	COLONEQUAL, 
	DEFINEDAS, 
};

struct lexdescription lexdesc = {
	(unsigned char **)symlist, symtok, (unsigned char **)thonglist, thongtok,
	setID, setINTCON, setREALCON, setSTRINGCON,
};

printToksym(s)
	struct toksym *s;
{
	printf("%s",  toksym_TGetName(s));
	switch(s->toknum) {
	case setID:
		printf("  from (%d,%d)", s->loc, s->len);
		break;
	case setSTRINGCON:
		printf("  from (%d,%d)", s->loc, s->len);
		if (*s->header.sym.name == '\'') 
			printf ("  char \'%c\' (%d)", s->header.sym.name[1],
					s->header.sym.name[1]);
		break;
	case setINTCON:
		printf("  from (%d,%d)  val %d", s->loc, s->len, s->info.intval);
		break;
	case setREALCON:
		printf("  from (%d,%d)  val %.5g", s->loc, s->len, *s->info.realval);
		break;
	}
	printf("\n");
};



printRecent()
{
	long loc, len, endloc;
	loc = lex_RecentPosition(-3, &len);
	endloc = lex_RecentPosition(0, &len);
	endloc += len;
	for ( ; loc < endloc; loc ++)
		putchar(text_GetChar(text, loc)); 
}

yyerror(s)
	unsigned char *s;
{
	long loc, len;
	loc = lex_RecentPosition(0, &len);
	printf("ERROR: %s at %d (len %d)\n        Preceding text: \"", s, loc, len);
	printRecent();
	printf("\"\n");
}


main(argc, argv)
        char **argv;
{
	long toknum;
	struct lexdef *lexdef;
	struct lex *toklex;
	FILE *inputfile;

	long objectID;
	char *objectType;
	struct attributes *attributes;

	boolean Cswitch = FALSE;

	class_Init(".");		/* use current directory for dataobject path */

	observable_StaticEntry;
	proctable_StaticEntry;
	dataobject_StaticEntry;

	text = text_New();

	Cswitch = (argc == 3 && strcmp(argv[2], "-C") == 0);

	/* the old easy way:
		text_Read(text, fopen(argv[1], "r"), 0);
	the way it needs to be done: */

	if (argc < 2 || (inputfile=fopen(argv[1], "r" )) == NULL) {
		printf("File %s not found\n", argv[1]);
		exit(1);
	}
		
	if ((objectType = filetype_Lookup(inputfile, argv[1], &objectID, &attributes))
				!= NULL) 
		/* NULL means use default object. Text in this case. */
		/* If not default, make sure that the object type of the file is compatible
		 * with text.  */
		if (!class_IsTypeByName(objectType, "text")) {
			fprintf(stderr, "File is not a text object, its a %s\n", objectType);
			exit(1);
		}
	if (attributes != NULL)
		/* Gets things like readonly and others. Omit if you don't need the
		 * attributes. (You can pass NULL into filetype_Lookup instead of the
		 * attributes parameter.  */
		text_SetAttributes(text, attributes);
	text_Read(text, inputfile, objectID);


	lexdef = lexdef_Create(yyerror, toksym_New(), 
			(Cswitch) ? SlashStar : DashDash, 
			TRUE, &lexdesc);
	toklex = lex_Create(lexdef);
	lex_SetSource(toklex, text, 0, text_GetLength(text),
			toksym_GLOBAL, toksym_GLOBAL);

	printf("Reading first three tokens\n");
	toknum = yylex(); printToksym(yylval);
	toknum = yylex(); printToksym(yylval);
	toknum = yylex(); printToksym(yylval);

	printf("Restarting at the second token\n");
	lex_Repeat(-1);

	while ((toknum=yylex()) > 0) {
		printf("(%d) ", toknum );
		if (yylval == NULL)
			printf ("NonSymbol\n");
		else
			printToksym(yylval);
	}
	printf("\nlast of the text was: ");
	printRecent();
	printf("\n");
}





