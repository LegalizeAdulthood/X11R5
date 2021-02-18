#include <stdio.h>
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
#define output(c) putc(c,yyout)
#define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
#define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
extern char *yysptr, yysbuf[];
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
int yymorfg;
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
/*
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 */
#ident  "%Z%%M% %I%     %E% SMI"

#include <stdio.h>

#define	TRUE	1
#define	FALSE	0

extern int	stdin_only;
extern char	curr_file[];	/* Name of file currently being read. */
extern int	curr_linenum;	/* line number in the current input file */
extern int	optind;
extern int	gargc;
extern char	**gargv;

/*
 * The following lex rule basically wants to recognize tokens 
 * that can change the current state of scanning source code.
 * Evertime such tokens are recognized, the specific handler will be
 * executed. All other tokens are treated as regular characters and 
 * they are all handled the same way.
 * The tricky part was not to miss any specification in ANSI-C code
 * that looks like a meaningful token but not a meaningful one and
 * should be treated as regular characters.
 * For example,
 *	c= '"';d='"'; printf("\"" "\(\)\\\"");
 *	c = ABgettext("Not a gettext");
 *	c = gettextXY("Not a gettext");
 *	c = ABgettextXY("Not a gettext");
 */
# define YYNEWLINE 10
yylex(){
int nstr; extern int yyprevious;
while((nstr = yylook()) >= 0)
yyfussy: switch(nstr){
case 0:
if(yywrap()) return(0); break;
case 1:
	{ handle_macro_line(); }
break;
case 2:
		{ handle_open_comment(); }
break;
case 3:
		{ handle_close_comment(); }
break;
case 4:
{ handle_dgettext(); }
break;
case 5:
	{ handle_gettext(); }
break;
case 6:
{ handle_textdomain(); }
break;
case 7:
case 8:
	case 9:
case 10:
	case 11:
	case 12:
	case 13:
	{ handle_character(); }
break;
case 14:
	{ handle_open_paren(); }
break;
case 15:
	{ handle_close_paren(); }
break;
case 16:
	{ handle_esc_newline(); }
break;
case 17:
	{ handle_comma (); }
break;
case 18:
	{ handle_newline(); }
break;
case 19:
	{ handle_quote (); }
break;
case 20:
	{ handle_spaces (); }
break;
case 21:
	{ handle_spaces (); }
break;
case 22:
	{ handle_character(); }
break;
case -1:
break;
default:
fprintf(yyout,"bad switch yylook %d",nstr);
} return(0); }
/* end of yylex */

/*
 * Since multiple files can be processed, yywrap() should keep feeding
 * all input files specified.
 */
yywrap()
{
	FILE	*fp;

	if ((optind >= gargc) || (stdin_only == TRUE)) {
		return (1);
	} else {
		/*
		 * gargv still contains not-processed input files.
		 */
		freopen (gargv[optind], "r", stdin);
		if ((fp = freopen (gargv[optind], "r", stdin)) == NULL) {
			fprintf(stderr, "ERROR, can't open input file: %s\n",
					gargv[optind]);
		}
#ifdef DEBUG
		printf("In yywrap(), opening file  %d, <%s>\n",
				optind, gargv[optind]);
#endif
		/*
		 * Reset global file name and line number for the new file.
		 */
		strcpy (curr_file, gargv[optind]);
		curr_linenum = 0;

		optind++;

		return (0);
	}

} /* yywrap */
int yyvstop[] = {
0,
/* actions for state 2 */
22,
0,
/* actions for state 3 */
21,
22,
0,
/* actions for state 4 */
18,
0,
/* actions for state 5 */
20,
22,
0,
/* actions for state 6 */
19,
22,
0,
/* actions for state 7 */
22,
0,
/* actions for state 8 */
14,
22,
0,
/* actions for state 9 */
15,
22,
0,
/* actions for state 10 */
22,
0,
/* actions for state 11 */
17,
22,
0,
/* actions for state 12 */
22,
0,
/* actions for state 13 */
7,
22,
0,
/* actions for state 14 */
22,
0,
/* actions for state 15 */
7,
22,
0,
/* actions for state 16 */
7,
22,
0,
/* actions for state 17 */
7,
22,
0,
/* actions for state 18 */
1,
22,
0,
/* actions for state 21 */
3,
0,
/* actions for state 22 */
2,
0,
/* actions for state 23 */
7,
0,
/* actions for state 24 */
16,
0,
/* actions for state 25 */
11,
0,
/* actions for state 26 */
12,
0,
/* actions for state 27 */
13,
0,
/* actions for state 28 */
10,
0,
/* actions for state 29 */
7,
0,
/* actions for state 30 */
7,
0,
/* actions for state 31 */
7,
0,
/* actions for state 32 */
8,
0,
/* actions for state 34 */
7,
0,
/* actions for state 35 */
7,
0,
/* actions for state 36 */
7,
0,
/* actions for state 37 */
9,
0,
/* actions for state 38 */
7,
0,
/* actions for state 39 */
7,
0,
/* actions for state 40 */
7,
0,
/* actions for state 41 */
7,
0,
/* actions for state 42 */
7,
0,
/* actions for state 43 */
7,
0,
/* actions for state 44 */
7,
0,
/* actions for state 45 */
7,
0,
/* actions for state 46 */
7,
0,
/* actions for state 47 */
7,
0,
/* actions for state 48 */
5,
7,
0,
/* actions for state 49 */
7,
0,
/* actions for state 50 */
4,
7,
0,
/* actions for state 51 */
7,
0,
/* actions for state 52 */
7,
0,
/* actions for state 53 */
6,
7,
0,
0};
# define YYTYPE char
struct yywork { YYTYPE verify, advance; } yycrank[] = {
0,0,	0,0,	1,3,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	1,4,	1,5,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	2,4,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,6,	0,0,	1,7,	
8,20,	15,25,	0,0,	21,34,	
1,8,	1,9,	1,10,	1,11,	
13,23,	1,12,	20,33,	34,38,	
1,13,	1,14,	2,6,	11,22,	
2,7,	2,19,	0,0,	0,0,	
0,0,	2,8,	2,9,	2,10,	
2,11,	15,26,	2,12,	0,0,	
0,0,	2,13,	0,0,	15,27,	
15,28,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,15,	8,21,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,16,	0,0,	0,0,	
1,17,	0,0,	17,31,	18,32,	
16,30,	30,35,	2,15,	0,0,	
0,0,	0,0,	0,0,	0,0,	
0,0,	1,18,	2,16,	15,29,	
0,0,	2,17,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
31,36,	32,37,	2,18,	35,39,	
36,40,	37,41,	39,42,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	40,43,	41,44,	42,45,	
43,46,	14,24,	44,47,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	14,24,	14,24,	14,24,	
14,24,	45,48,	46,49,	47,50,	
48,51,	50,52,	52,53,	53,54,	
0,0};
struct yysvf yysvec[] = {
0,	0,	0,
yycrank+-1,	0,		0,			/* state 0 */
yycrank+-18,	yysvec+1,	0,			/* state 1 */
yycrank+0,	0,		yyvstop+1,		/* state 2 */
yycrank+0,	0,		yyvstop+3,		/* state 3 */
yycrank+0,	0,		yyvstop+6,		/* state 4 */
yycrank+0,	0,		yyvstop+8,		/* state 5 */
yycrank+0,	0,		yyvstop+11,		/* state 6 */
yycrank+2,	0,		yyvstop+14,		/* state 7 */
yycrank+0,	0,		yyvstop+16,		/* state 8 */
yycrank+0,	0,		yyvstop+19,		/* state 9 */
yycrank+4,	0,		yyvstop+22,		/* state 10 */
yycrank+0,	0,		yyvstop+24,		/* state 11 */
yycrank+2,	0,		yyvstop+27,		/* state 12 */
yycrank+74,	0,		yyvstop+29,		/* state 13 */
yycrank+27,	0,		yyvstop+32,		/* state 14 */
yycrank+5,	yysvec+14,	yyvstop+34,		/* state 15 */
yycrank+5,	yysvec+14,	yyvstop+37,		/* state 16 */
yycrank+6,	yysvec+14,	yyvstop+40,		/* state 17 */
yycrank+0,	0,		yyvstop+43,		/* state 18 */
yycrank+7,	0,		0,			/* state 19 */
yycrank+5,	0,		0,			/* state 20 */
yycrank+0,	0,		yyvstop+46,		/* state 21 */
yycrank+0,	0,		yyvstop+48,		/* state 22 */
yycrank+0,	yysvec+14,	yyvstop+50,		/* state 23 */
yycrank+0,	0,		yyvstop+52,		/* state 24 */
yycrank+0,	0,		yyvstop+54,		/* state 25 */
yycrank+0,	0,		yyvstop+56,		/* state 26 */
yycrank+0,	0,		yyvstop+58,		/* state 27 */
yycrank+0,	0,		yyvstop+60,		/* state 28 */
yycrank+8,	yysvec+14,	yyvstop+62,		/* state 29 */
yycrank+16,	yysvec+14,	yyvstop+64,		/* state 30 */
yycrank+13,	yysvec+14,	yyvstop+66,		/* state 31 */
yycrank+0,	0,		yyvstop+68,		/* state 32 */
yycrank+8,	0,		0,			/* state 33 */
yycrank+19,	yysvec+14,	yyvstop+70,		/* state 34 */
yycrank+20,	yysvec+14,	yyvstop+72,		/* state 35 */
yycrank+21,	yysvec+14,	yyvstop+74,		/* state 36 */
yycrank+0,	0,		yyvstop+76,		/* state 37 */
yycrank+22,	yysvec+14,	yyvstop+78,		/* state 38 */
yycrank+64,	yysvec+14,	yyvstop+80,		/* state 39 */
yycrank+66,	yysvec+14,	yyvstop+82,		/* state 40 */
yycrank+66,	yysvec+14,	yyvstop+84,		/* state 41 */
yycrank+48,	yysvec+14,	yyvstop+86,		/* state 42 */
yycrank+59,	yysvec+14,	yyvstop+88,		/* state 43 */
yycrank+77,	yysvec+14,	yyvstop+90,		/* state 44 */
yycrank+82,	yysvec+14,	yyvstop+92,		/* state 45 */
yycrank+90,	yysvec+14,	yyvstop+94,		/* state 46 */
yycrank+84,	yysvec+14,	yyvstop+96,		/* state 47 */
yycrank+0,	yysvec+14,	yyvstop+98,		/* state 48 */
yycrank+104,	yysvec+14,	yyvstop+101,		/* state 49 */
yycrank+0,	yysvec+14,	yyvstop+103,		/* state 50 */
yycrank+97,	yysvec+14,	yyvstop+106,		/* state 51 */
yycrank+93,	yysvec+14,	yyvstop+108,		/* state 52 */
yycrank+0,	yysvec+14,	yyvstop+110,		/* state 53 */
0,	0,	0};
struct yywork *yytop = yycrank+203;
struct yysvf *yybgin = yysvec+1;
char yymatch[] = {
  0,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,  10,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
  1,   1,   1,   1,   1,   1,   1,   1, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,   1,   1,   1,   1,   1,   1, 
  1,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,   1,   1,   1,   1,  48, 
  1,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,  48,  48,  48,  48,  48, 
 48,  48,  48,   1,   1,   1,   1,   1, 
0};
char yyextra[] = {
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,
0};
#ifndef lint
static	char ncform_sccsid[] = "@(#)ncform 1.6 88/02/08 SMI"; /* from S5R2 1.2 */
#endif

int yylineno =1;
# define YYU(x) x
# define NLSTATE yyprevious=YYNEWLINE
char yytext[YYLMAX];
struct yysvf *yylstate [YYLMAX], **yylsp, **yyolsp;
char yysbuf[YYLMAX];
char *yysptr = yysbuf;
int *yyfnd;
extern struct yysvf *yyestate;
int yyprevious = YYNEWLINE;
yylook(){
	register struct yysvf *yystate, **lsp;
	register struct yywork *yyt;
	struct yysvf *yyz;
	int yych, yyfirst;
	struct yywork *yyr;
# ifdef LEXDEBUG
	int debug;
# endif
	char *yylastch;
	/* start off machines */
# ifdef LEXDEBUG
	debug = 0;
# endif
	yyfirst=1;
	if (!yymorfg)
		yylastch = yytext;
	else {
		yymorfg=0;
		yylastch = yytext+yyleng;
		}
	for(;;){
		lsp = yylstate;
		yyestate = yystate = yybgin;
		if (yyprevious==YYNEWLINE) yystate++;
		for (;;){
# ifdef LEXDEBUG
			if(debug)fprintf(yyout,"state %d\n",yystate-yysvec-1);
# endif
			yyt = yystate->yystoff;
			if(yyt == yycrank && !yyfirst){  /* may not be any transitions */
				yyz = yystate->yyother;
				if(yyz == 0)break;
				if(yyz->yystoff == yycrank)break;
				}
			*yylastch++ = yych = input();
			yyfirst=0;
		tryagain:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"char ");
				allprint(yych);
				putchar('\n');
				}
# endif
			yyr = yyt;
			if ( (int)yyt > (int)yycrank){
				yyt = yyr + yych;
				if (yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
# ifdef YYOPTIM
			else if((int)yyt < (int)yycrank) {		/* r < yycrank */
				yyt = yyr = yycrank+(yycrank-yyt);
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"compressed state\n");
# endif
				yyt = yyt + yych;
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transitions */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				yyt = yyr + YYU(yymatch[yych]);
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"try fall back character ");
					allprint(YYU(yymatch[yych]));
					putchar('\n');
					}
# endif
				if(yyt <= yytop && yyt->verify+yysvec == yystate){
					if(yyt->advance+yysvec == YYLERR)	/* error transition */
						{unput(*--yylastch);break;}
					*lsp++ = yystate = yyt->advance+yysvec;
					goto contin;
					}
				}
			if ((yystate = yystate->yyother) && (yyt= yystate->yystoff) != yycrank){
# ifdef LEXDEBUG
				if(debug)fprintf(yyout,"fall back to state %d\n",yystate-yysvec-1);
# endif
				goto tryagain;
				}
# endif
			else
				{unput(*--yylastch);break;}
		contin:
# ifdef LEXDEBUG
			if(debug){
				fprintf(yyout,"state %d char ",yystate-yysvec-1);
				allprint(yych);
				putchar('\n');
				}
# endif
			;
			}
# ifdef LEXDEBUG
		if(debug){
			fprintf(yyout,"stopped at %d with ",*(lsp-1)-yysvec-1);
			allprint(yych);
			putchar('\n');
			}
# endif
		while (lsp-- > yylstate){
			*yylastch-- = 0;
			if (*lsp != 0 && (yyfnd= (*lsp)->yystops) && *yyfnd > 0){
				yyolsp = lsp;
				if(yyextra[*yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-*yyfnd) != 1 && lsp > yylstate){
						lsp--;
						unput(*yylastch--);
						}
					}
				yyprevious = YYU(*yylastch);
				yylsp = lsp;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
# ifdef LEXDEBUG
				if(debug){
					fprintf(yyout,"\nmatch ");
					sprint(yytext);
					fprintf(yyout," action %d\n",*yyfnd);
					}
# endif
				return(*yyfnd++);
				}
			unput(*yylastch);
			}
		if (yytext[0] == 0  /* && feof(yyin) */)
			{
			yysptr=yysbuf;
			return(0);
			}
		yyprevious = yytext[0] = input();
		if (yyprevious>0)
			output(yyprevious);
		yylastch=yytext;
# ifdef LEXDEBUG
		if(debug)putchar('\n');
# endif
		}
	}
yyback(p, m)
	int *p;
{
if (p==0) return(0);
while (*p)
	{
	if (*p++ == m)
		return(1);
	}
return(0);
}
	/* the following are only used in the lex library */
yyinput(){
	return(input());
	}
yyoutput(c)
  int c; {
	output(c);
	}
yyunput(c)
   int c; {
	unput(c);
	}
