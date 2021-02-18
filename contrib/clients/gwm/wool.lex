/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
** Copyright 1989 Massachusetts Institute of Technology
**/
/****************************\
* 			     *
*  Lex generator for Wool   *
* 			     *
\****************************/

%{

/* to have always at least MAX_TOKEN_LENGTH chars in all lexes */
#undef YYLMAX
#define YYLMAX MAX_TOKEN_LENGTH

/* here goes the definition of lex I/O, as macros for efficiency */

/* first, we forget the previous definitions, */
#undef output
#undef input
#undef unput
/* which were :
#define output(c) putc(c,yyout)
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):\
getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;\
*yysptr++=yytchar;}
*/

/* So here are OUR macros */

static int yyin_is_string;	/* 0 ==> reading from STREAM yyin */
				/* 1 ==> reading from STRING yystrin */
static int yyout_is_string;	/* 0 ==> printing on STREAM yyout */
		/* 1 ==> printing on WOOL_STRING_STREAM yystrout */
static char *yystrin;		/* current pointer on input string */
static WOOL_STRING_STREAM yystrout; /* current wool output stream */

#define output(c) {\
    if(yyout_is_string){\
        *(yystrout->ptr)++ = c;\
        *(yystrout->ptr) = '\0';\
        if((yystrout->ptr) >= (yystrout->last)){\
	    yyoutflush();\
	}\
    }else{\
	putc(c,yyout);\
    }\
}

#define input() (((\
    	yytchar =\
        (yysptr>yysbuf ?\
	    U(*--yysptr)\
	:   (yyin_is_string ?\
	        ( *yystrin ? *yystrin++ : 0)\
	    :   getc(yyin))))\
	==10 ?\
	    (yylineno++,yytchar)\
	:   yytchar)\
    ==EOF ?\
        0\
    :   yytchar)

#define unput(c) {\
    yytchar= (c);\
    if(yytchar=='\n')\
        yylineno--;\
    *yysptr++=yytchar;\
}

/* externally callable function for unput:
 */

wool_unput(buffer)
char *buffer;
{
    while (*buffer) {
	unput(*buffer);
	buffer++;
    }
}

/* counting the parentheses -- hack for wool_pool */

#define INC_PAR wool_pool_parentheses_level++
#define DEC_PAR if(wool_pool_parentheses_level)wool_pool_parentheses_level--
%}

FNCP	[^-+\004 \t\n"'()0-9]
FNC	[^\004 \t\n"'()0-9]
NC	[^\004 \t\n"'()]
SIGN	[-+]

%%
.[\010]		;				/* handles backspacing */
.[\177]		;				/* handles deleting */
\"([^\\\"\n]|\\(.|\n)|\"\")*\"  return(STRING); /* strings */
\"([^\\\"\n]|\\(.|\n)|\"\")*\n  return(NON_CLOSED_STRING); /* error */
\;.*\n		;				/* comments */
[-+]?[0-9]+	return(NUMBER);			/* integers */
0[xX][0-9a-fA-F]+	return(HEX_NUMBER);	/* hex integers */
"("		{INC_PAR; return(LEFTPAR);}	/* start of list */
")"		{DEC_PAR; return(RIGHTPAR);}    /* end of list */
"'"		return(QUOTECHAR);		/* quoting */
"{"		{INC_PAR; return(LEFTBRA);}	/* (progn */
"}"		{DEC_PAR; return(RIGHTBRA);}	/* ) */
{FNCP}{NC}*	return (NAME);			/* identifier */
{SIGN}{FNC}{NC}*	return (NAME);		/* +foo */
{SIGN}		return (NAME);			/* + */
[ \t\n]		;				/* blanks */
[\004]		return(END_OF_FILE);		/* pseudo-EOF handling */
.		;				/* skip over control codes */
%%

/**********************\
* 		       *
*  WOOL's I/O package  *
* 		       *
\**********************/

/* 
 * yywrap
 * we treat EOF (or EOString) as a TOKEN, for yacc.
 */

yywrap(){		/* to make EOF a "normal" character */
    unput('\004');	/* EOF is pushed back on the input as ^D */
    return 0;		/* tell Lex there is more to read */
}

/*
 * yyinflush
 * to flush wool input buffers (i.e. the unput buffer)
 * stores old input buffer in arg (should be ~ 16) if not NULL
 */

void
yyinflush(wool_old_unput_buffer)
char *wool_old_unput_buffer;
{
    if (wool_old_unput_buffer) {
	if (yysptr != yysbuf)
	    strncpy(wool_old_unput_buffer, yysbuf, yysptr - yysbuf);
	wool_old_unput_buffer[yysptr - yysbuf] = '\0';
    }
    yysptr = yysbuf;
}

/*
 * yyoutflush
 * to flush wool output buffer.
 */

void
yyoutflush(){
    if(yyout_is_string){
	(*(yystrout->overflow_handler))(yystrout->buffer);
	yystrout->ptr = yystrout-> buffer;
    }else{
	fflush(yyout);
    }
}

/*
 * wool_input_redirect
 * to set wool's parsing to the stream or string argument.
 * arg1 = type (0 = string, 1 = stream);
 * arg2 = stream or string
 * arg3 = POINTER to (FILE *) or (char *) where will go the old stream
 *        (if NULL not set)(returns the old type)
 * arg4 = where to save contents of unput buffer (if not NULL)
 */

int wool_input_redirect(type, stream, oldstream_p, old_buffer_contents)
int type;
char *stream;
char **oldstream_p;
char *old_buffer_contents;
{
    int oldtype = yyin_is_string;
    if(oldstream_p) *oldstream_p =
    	(oldtype ? (char *) yystrin : (char *) yyin);
    yyinflush(old_buffer_contents);
    if(yyin_is_string = type){
	yystrin = stream;
    }else{
	yyin = (FILE *) stream;
    }
    return oldtype;
}

/*
 * wool_output_redirect
 * to set wool's outputs to the stream or string argument.
 * arg1 = type (0 = string, 1 = stream);
 * arg2 = stream or string
 * arg4 = POINTER to (FILE *) or WOOL_STRING_STREAM tu put the old stream
 *        (if NULL not set)(returns the old type)
 */

wool_output_redirect(type, stream, oldstream_p)
int type;
char *stream;
char **oldstream_p;
{
    int oldtype = yyout_is_string;
    if(oldstream_p) *oldstream_p =
    	(oldtype ? (char *) yystrout : (char *) yyout);
    yyoutflush();
    if(yyout_is_string = type){
	yystrout = (WOOL_STRING_STREAM) stream;
    }else{
	yyout = (FILE *) stream;
    }
    return oldtype;
}

/*
 * now, some functions to provide a printf - like service on wool's output.
 */

/* prints a string */

void
wool_puts(string)
register char *string;
{
    while(*string) output(*string++);
}

/* put a newline */

wool_newline()
{
    output('\n');
}

/* does a format with ONE arg. */

wool_printf(string, arg)
register char *string;
char *arg;
{
    static char wool_temp_string[MAX_TEMP_STRING_SIZE];
    sprintf(wool_temp_string, string, arg);
    wool_puts(wool_temp_string);
}

/* prints a char */

void
wool_putchar(c)
char c;
{
    output(c);
}

/*
 * function to make a WOOL_STRING_STREAM of a given capacity
 * arg1 = capactity in bytes
 * arg2 = user function to be called on buffer when overflow occurs
 */

WOOL_STRING_STREAM WOOL_STRING_STREAM_make(nbytes, handler)
int nbytes;
int (* handler)();
{
    WOOL_STRING_STREAM str = (WOOL_STRING_STREAM)
    	Malloc(sizeof(struct _WOOL_STRING_STREAM));
    str->buffer = (char *) Malloc(nbytes);
    str->ptr = str->buffer;
    str->last = str->buffer + nbytes -1;
    str->overflow_handler = handler;
    return str;
}

/* 
 * and the function to free a stream
 */

WOOL_STRING_STREAM_free (str)
WOOL_STRING_STREAM str;
{
    Free(str->buffer);
    Free(str);
}
