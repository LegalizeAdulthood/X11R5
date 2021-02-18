%{

/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#undef input
#undef unput

static char *ttt ;

%}

modifiers	[bBeEfFgGiInNpPtSsTuUvVwWzZ0-9\+\|\-\=\@]*

%START format data final char_find int_find color_find participants_find

%%

 if (new_menu_start)
	{
	BEGIN 0 ;
	new_menu_start = False ;
	}

<INITIAL>\;			/* Semicolon to end options */
	{
	BEGIN format ;
	return ';' ;
	}

<INITIAL>equal_rows			/* equal rows option */
	{
	return EQUAL_ROWS ;
	}

<INITIAL>aligned_columns	/* aligned_columns option*/
	{
	return ALIGNED_COLUMNS ;
	}

<INITIAL>clip_on_shrink	/* clip_on_shrink option*/
	{
	return CLIP_ON_SHRINK ;
	}

<INITIAL>equal_columns|equal_cols	/* equal columns option */
	{
	return EQUAL_COLUMNS ;
	}

<INITIAL>tab		/* item separator option */
	{
	BEGIN char_find ;
	return STRING_BREAKER ;
	}

<INITIAL>lnl		/* lexical newline equivalent */
	{
	BEGIN char_find ;
	return LNL_EQUIVALENT ;
	}

<INITIAL>snl		/* string newline equivalent */
	{
	BEGIN char_find ;
	return SNL_EQUIVALENT ;
	}

<INITIAL>padding	/* inter,internal_width & inter,internal_height option*/
	{
	BEGIN int_find ;
	return PADDING ;
	}

<INITIAL>internals	/* internal_width & internal_height option*/
	{
	BEGIN int_find ;
	return INTERNALS ;
	}

<INITIAL>inters	/* inter_width & inter_height option*/
	{
	BEGIN int_find ;
	return INTERS ;
	}

<INITIAL>inter_width	/* inter_width option*/
	{
	BEGIN int_find ;
	return INTER_WIDTH ;
	}

<INITIAL>inter_height	/* inter_height option*/
	{
	BEGIN int_find ;
	return INTER_HEIGHT ;
	}

<INITIAL>internal_width	/* internal_width option*/
	{
	BEGIN int_find ;
	return INTERNAL_WIDTH ;
	}

<INITIAL>internal_height	/* internal_height option*/
	{
	BEGIN int_find ;
	return INTERNAL_HEIGHT ;
	}

<INITIAL>resize_participants	/* resize_participants option*/
	{
	BEGIN participants_find ;
	return RESIZE_PARTICIPANTS ;
	}

<INITIAL>([tpc]_)?background_color	/* t_background_color option*/
	{
	BEGIN color_find ;
	if (*yytext == 'b')
	    return BACKGROUND_COLOR ;
	else
	if (*yytext == 'c')
	    return C_BACKGROUND_COLOR ;
	else
	    return T_BACKGROUND_COLOR ;
	}

<INITIAL>([tpc]_)?foreground_color	/* t_foreground_color option*/
	{
	BEGIN color_find ;
	if (*yytext == 'f')
	    return FOREGROUND_COLOR ;
	else
	if (*yytext == 'c')
	    return C_FOREGROUND_COLOR ;
	else
	    return T_FOREGROUND_COLOR ;
	}

<INITIAL>([tpc]_)?border_color	/* t_border_color option*/
	{
	BEGIN color_find ;
	if (*yytext == 'b')
	    return BORDER_COLOR ;
	else
	if (*yytext == 'c')
	    return C_BORDER_COLOR ;
	else
	    return T_BORDER_COLOR ;
	}

<INITIAL>([tpc]_)?border_width	/* t_border_width option*/
	{
	BEGIN int_find ;
	if (*yytext == 'b')
	    return BORDER_WIDTH ;
	else
	if (*yytext == 'c')
	    return C_BORDER_WIDTH ;
	else
	    return T_BORDER_WIDTH ;
	}

<INITIAL>center|expand|box|allbox|doublebox|delim\(..\)|linesize\([0-9]+\) /* */
	{
	return IGNORED_OPTION ;
	/* ignored options */
	}

<char_find,int_find,color_find,participants_find>[\(\)] /* parentheses */
	{
	if (*yytext == ')')
	    BEGIN 0 ;
	return yytext[0] ;
	}

<char_find,int_find,color_find,participants_find>[ \t] /* white space */
	{
	}

<char_find>.		/* single character */
	{
	yylval.c_val = yytext[0] ;
	return CHARACTER ;
	}

<int_find>-?[0-9]+	/* integers */
	{
	yylval.i_val = atoi(yytext) ;
	return INTEGER ;
	}

<color_find>[A-Za-z][^ \t\)]*	/* string (color, font, etc.) */
	{
	XrmValue from, to ;

	from.size = sizeof (String) ;
	from.addr = (caddr_t) yytext ;
	XtConvert ((Widget)tw_, XtRString, &from, XtRPixel, &to) ;
	yylval.i_val = (int)(*((Pixel *)to.addr)) ;
	return INTEGER ;
	}

<participants_find>\,		/* ignore commas */
	{
	}

<participants_find>inters 		/* resize_participants */
	{
	return INTERS ;
	}

<participants_find>internals 		/* resize_participants */
	{
	return INTERNALS ;
	}

<participants_find>children 		/* resize_participants */
	{
	return CHILDREN ;
	}

<INITIAL>^\.TS			/* table_start (optional) */
	{
	return TS ;
	}

<INITIAL,format>{modifiers}[\_\=aAnN]{modifiers} /* illegal item element */
    {
    char *my_text1 = "XcuTbl: Illegal lex item (one of _=aAnN) in (%s)\n%s" ;
    char *my_text2 = "Legal items are (lLrRcCsS^).\n" ;
    char *etext = XtMalloc (strlen(my_text1) + strlen(my_text2) +
			    strlen(yytext)) ;
    sprintf (etext, my_text1, yytext, my_text2) ;
    XtError (etext) ;
    }

<INITIAL,format>{modifiers}[lLrRcC]{modifiers} /* legal item element */
	{
	Xcu_copy_ds (&ttt, yytext) ;
	yylval.s_val = ttt ;
	return ITEM_ELEMENT ;
	}

<INITIAL,format>\.[ \t]*\n		/* Dot to end formats */
	{
	BEGIN data ;
	return DOT_NL ;
	}

<INITIAL,format>[sS] 			/* horizontal span element */
	{
	return HSPAN_ELEMENT ;
	}

<INITIAL,format>\^ 			/* vertical span element */
	{
	return VSPAN_ELEMENT ;
	}

<INITIAL,format>\\\n			/* continuation */
	{
	return CONTINUATION ;
	}

<INITIAL,format>\.			/* period */
	{
	/* Kludge city! I don't know why the code commented out doesn't work */
	/* Lex fails to recognize the following string when it appears */
	unput ('\n') ;
	unput ('.') ;
	/***
	BEGIN data ;
	return '.' ;
	***/
	}

<INITIAL,format>\n			/* newline */
	{
	return NL ;
	}

<INITIAL,format>^[-=][ \t]*\n		/* lines ignored */
    {
    }

<INITIAL,format>^#.*\n			/* comments	*/
    {
    }

<INITIAL,format>[ \t]*			/* white space */
    {
    }

<INITIAL,format>.   			/* anything else */
    {
    ytblerror ("Lexical error") ;
    }

<data>^\.T\&			/* table_ampersand */
	{
	BEGIN format ;
	return TAMPERSAND ;
	}

<data>^[^.].*\n			/* data items */
	{
	if (yyleng >=2 && strcmp ("\\\n", &yytext[yyleng-2]) == 0)
		{
		yytext[yyleng-2] = '\0' ;
		yyleng -= 2 ;
		yymore () ;
		}
	else
		{
		yytext[yyleng-1] = '\0' ;
		Xcu_copy_ds (&ttt, yytext) ;
		yylval.s_val = ttt ;
		if (yyleng > 1)
		    return STRING ;
		}
	}

<data>^\n		/* trailing newlines */
	{
	}

^\.TE[ \t\n]*			/* table_end (optional) */
	{
	BEGIN final ;
	return TE ;
	}

<final>[.\n]				/* ignore all	*/
    {
    }

%%

