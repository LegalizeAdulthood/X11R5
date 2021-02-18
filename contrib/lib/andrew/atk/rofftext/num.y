%{
/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
#include <class.h>
#include <ctype.h>
#include <rofftext.ih>

double atof();

static double v_Result;
static int v_DefaultScale;
static char *StrPtr;
static int *SF;
static struct rofftext *rt;
%}

%start list

%union {
    double dval;
    enum scale_type sval;
}


%token <dval> CONST
%token <sval> SCALE

%type <dval> expr

%left '|' '&' '+' '-' '*' '/' '%' '>' '<' '='
%left UMINUS /* supplies precedence for unary minus */
%right SCALE

%% /* beginning of rules */

list	:	/* empty */
	|	list stat
        |       list error
			{	yyerrok; yyclearin; }
	;

stat	:	expr
                       {	v_Result = $1; }
	;

expr	:	'(' expr ')'
			{	$$ = $2; }
	|	expr '+' expr
			{	$$ = $1 + $3; }
	|	expr '-' expr
			{	$$ = $1 - $3; }
	|	expr '*' expr
			{	$$ = $1 * $3; }
	|	expr '/' expr
			{	$$ = $1 / $3; }
	|	expr '>' expr
			{	$$ = ($1 > $3); }
	|	expr '<' expr
			{	$$ = ($1 < $3); }
	|	expr '<' '=' expr
			{	$$ = ($1 <= $4); }
	|	expr '>' '=' expr
			{	$$ = ($1 >= $4); }
	|	expr '=' '=' expr
			{	$$ = ($1 == $4); }
	|	expr '=' expr
			{	$$ = ($1 == $3); }
	|	expr '&' expr
			{	$$ = ($1 && $3); }
	|	expr ':' expr
			{	$$ = ($1 || $3); }
	|	expr '%' expr
			{	$$ = (double)(((int)$1) % ((int)$3)); }
	|	'-' expr	%prec UMINUS
			{	$$ = - $2; }
        |       CONST SCALE
                        {       $$ = $1 * SF[(int)$2]; }
	|	CONST
                        {       $$ = $1 * SF[(int)v_DefaultScale]; }
	;

%%	/* start of programs */

static char *num_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/num.y,v 2.4 1991/09/13 16:51:40 bobg Exp $";

yylex()  /* lexical analysis routine */
{
    int c;

    while((c = *StrPtr++) == ' ') {}
    if (isalpha(c)){
        switch(c) {
            case 'i': yylval.sval = scale_i;
                break;
            case 'c': yylval.sval = scale_c;
                break;
            case 'P': yylval.sval = scale_P;
                break;
            case 'm': yylval.sval = scale_m;
                break;
            case 'n': yylval.sval = scale_n;
                break;
            case 'p': yylval.sval = scale_p;
                break;
            case 'u': yylval.sval = scale_u;
                break;
            case 'v': yylval.sval = scale_v;
                break;
            default:  yylval.sval = scale_u;
                break;
        }
        return (SCALE);
    }
    if(isdigit(c) || c == '.'){
        /* gobble digits, points */
        char Buf[51],*cp = Buf;
        int dot=0;
        for(;(cp-Buf)<50;++cp,c = *StrPtr++) {
            *cp = c;
            if (isdigit(c)) continue;
            if (c=='.') {
                if(dot++) return '.';
                continue;
            }
            /* end of number */
            break;
        }
        *cp = '\0';
        if(((cp-Buf)>=50) && rt->PrintWarnings)
            fprintf(stderr,"rofftext: warning: constant too big, truncating\n");
        else
            StrPtr--; /* push back last char read */
        yylval.dval = atof(Buf);
        return (CONST);
    }
    if (c=='\0') {
        c = EOF;
        StrPtr--;   /* forever EOF */
    }
    return (c);
}


yyerror(s) char *s;{
    if (rt->PrintWarnings) {
        fprintf(stderr,"rofftext: warning: number parsing error, %s line %d: %s\n",rt->Input->t->filename,rt->Input->t->LineNumber,s);
    }
}

       
EvalString(self,str,result,scale,absolute,relative)
struct rofftext *self;
char *str;
int *result;
int scale;
boolean *absolute,*relative;
{
    int sign = 0,temp;
    boolean ab,rel;

    v_DefaultScale = scale;
    rt = self;
    SF = self->ScaleFactor;
    switch (*str) {
        case '-':
            rel = 1;
            sign = 1;
            ab = 0;
            str++;
            break;
        case '+':
            rel = 1;
            ab = 0;
            str++;
            break;
        case '|':
            ab = 1;
            rel = 0;
            str++;
            break;
        default:
            ab = 0;
            rel = 0;
    }
            
    StrPtr = str;
    if (yyparse() == 0) {
        temp = (sign)?(-v_Result):(v_Result);
        *result = temp;
    }
    else {
        *result = 0;
    }

    if (absolute)
        *absolute = ab;
    if (relative)
        *relative = rel;
}

