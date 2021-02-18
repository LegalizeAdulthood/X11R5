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


static void	col_exceeded () ;
static void	row_exceeded () ;
static void	find_strings () ;
static void	mismatch () ;
static void	no_rows () ;

static XcuTblWidget tw_ ; /* no other way to get it into yylex... */
static Boolean new_menu_start = False ;

%}

%start	init

%union	{
	float	f_val ;
	int	i_val ;
	char	*s_val ;
	char	c_val ;
	}

%token <i_val>	INTEGER
%token <c_val>	CHARACTER
%token <s_val>	STRING
%token <s_val>	ITEM_ELEMENT
%token 		HSPAN_ELEMENT
%token 		VSPAN_ELEMENT
%token 		EQUAL_ROWS
%token 		EQUAL_COLUMNS
%token 		ALIGNED_COLUMNS
%token 		CLIP_ON_SHRINK
%token 		EQUAL_COLUMNS
%token 		IGNORED_OPTION
%token 		TAMPERSAND
%token 		CONTINUATION
%token 		STRING_BREAKER
%token 		LNL_EQUIVALENT
%token 		SNL_EQUIVALENT
%token 		INTER_WIDTH
%token 		INTER_HEIGHT
%token 		INTERNAL_WIDTH
%token 		INTERNAL_HEIGHT
%token 		RESIZE_PARTICIPANTS
%token 		PADDING
%token 		INTERS
%token 		INTERNALS
%token 		CHILDREN
%token 		BACKGROUND_COLOR
%token 		T_BACKGROUND_COLOR
%token 		C_BACKGROUND_COLOR
%token 		FOREGROUND_COLOR
%token 		T_FOREGROUND_COLOR
%token 		C_FOREGROUND_COLOR
%token 		BORDER_COLOR
%token 		T_BORDER_COLOR
%token 		C_BORDER_COLOR
%token 		BORDER_WIDTH
%token 		T_BORDER_WIDTH
%token 		C_BORDER_WIDTH
%token 		TS
%token 		TE
%token 		NL
%token 		DOT_NL
%token 		ERROR

%%

init	:
	    {
	    /* One time initializations */
	    tw->tbl.null_elements = (ElementRecord *)
				    XtCalloc (1, sizeof (ElementRecord)) ;
	    tw->tbl.temp_elements = (ElementRecord *)
				    XtCalloc (MAX_TBL_COLS, sizeof (ElementRecord)) ;
	    tw_ = tw ;
	    }
	| init tbl
	;

tbl : dot_ts _options format_series dot_nl data dot_te
	{
	new_menu_start = True ;
	}
    ;

dot_nl	: '.'
	| DOT_NL
	;

dot_ts  :
	| TS NL
	;

dot_te	:
	| TE
	;

_options :
	 | options ';' NL
	 | options ';'
	 ;

format_series	: format
		| format_series NL format
		;

format : element_series
    {
    Cardinal i ;

    if (tw->tbl.rows == MAX_TBL_ROWS)
	row_exceeded () ;

    /* allocate just enough space for the number of columns in this row */

    tw->tbl.item[tw->tbl.rows] = (ElementRecord *)
			XtCalloc (tw->tbl.n_cols, sizeof (ElementRecord)) ;
    tw->tbl.cols[tw->tbl.rows] = tw->tbl.n_cols ;

    /* copy the elements found into the permanent record */

    for (i=0;  i < tw->tbl.n_cols;  i++)
	{
	tw->tbl.item[tw->tbl.rows][i] = tw->tbl.temp_elements[i] ;
	tw->tbl.temp_elements[i] = *tw->tbl.null_elements ;
	}
    tw->tbl.rows++ ;

    /* re-initialize for next row */

    tw->tbl.n_cols = 0 ;
    }
	;

data	:
	| data_group_series
	;

data_group_series : data_group
		  | data_group_series data_group
		  ;

data_group : data_series
	   | TAMPERSAND NL format_series DOT_NL data_series
	   ;

data_series : stringline
	    | data_series stringline
	    ;

stringline : STRING
	{
	find_strings ($1, tw) ; 
	}
	   ;

options : option
	| options option
	| options NL option
	;

option	: EQUAL_ROWS
	    {
	    tw->tbl.equal_rows = True ;
	    }
	| EQUAL_COLUMNS
	    {
	    tw->tbl.equal_columns = True ;
	    }
	| ALIGNED_COLUMNS
	    {
	    tw->tbl.aligned_columns = True ;
	    }
	| CLIP_ON_SHRINK
	    {
	    tw->tbl.clip_on_shrink = True ;
	    }
	| IGNORED_OPTION
	    {
	    }
	| STRING_BREAKER '(' CHARACTER ')'
	    {
	    tw->tbl.string_breaker = $3 ;
	    }
	| LNL_EQUIVALENT '(' CHARACTER ')'
	    {
	    layout_input->lnl_equivalent = $3 ;
	    }
	| SNL_EQUIVALENT '(' CHARACTER ')'
	    {
	    layout_input->snl_equivalent = $3 ;
	    }
	| INTER_WIDTH '(' INTEGER ')'
	    {
	    tw->tbl.inter_width = $3 ;
	    }
	| INTER_HEIGHT '(' INTEGER ')'
	    {
	    tw->tbl.inter_height = $3 ;
	    }
	| INTERNAL_WIDTH '(' INTEGER ')'
	    {
	    tw->tbl.internal_width = $3 ;
	    }
	| INTERNAL_HEIGHT '(' INTEGER ')'
	    {
	    tw->tbl.internal_height = $3 ;
	    }
	| INTERS '(' INTEGER ')'
	    {
	    tw->tbl.inter_width = $3 ;
	    tw->tbl.inter_height = $3 ;
	    }
	| INTERNALS '(' INTEGER ')'
	    {
	    tw->tbl.internal_width = $3 ;
	    tw->tbl.internal_height = $3 ;
	    }
	| PADDING '(' INTEGER ')'
	    {
	    tw->tbl.inter_width = $3 ;
	    tw->tbl.inter_height = $3 ;
	    tw->tbl.internal_width = $3 ;
	    tw->tbl.internal_height = $3 ;
	    }
	| BORDER_WIDTH '(' INTEGER ')'
	    {
	    tw->core.border_width = $3 ;
	    tw->tbl.child_border_width = $3 ;
	    }
	| T_BORDER_WIDTH '(' INTEGER ')'
	    {
	    tw->core.border_width = $3 ;
	    }
	| C_BORDER_WIDTH '(' INTEGER ')'
	    {
	    tw->tbl.child_border_width = $3 ;
	    }
	| BORDER_COLOR '(' INTEGER ')'
	    {
	    tw->core.border_pixel = $3 ; /* see comment below */
	    tw->tbl.child_border_color = $3 ;
	    }
	| T_BORDER_COLOR '(' INTEGER ')'
	    {
	    /*
	     * This doesn't work!
	     */
	    tw->core.border_pixel = $3 ;
	    }
	| C_BORDER_COLOR '(' INTEGER ')'
	    {
	    tw->tbl.child_border_color = $3 ;
	    }
	| FOREGROUND_COLOR '(' INTEGER ')'
	    {
	    tw->tbl.child_foreground_color = $3 ;
	    }
	| C_FOREGROUND_COLOR '(' INTEGER ')'
	    {
	    tw->tbl.child_foreground_color = $3 ;
	    }
	| BACKGROUND_COLOR '(' INTEGER ')'
	    {
	    tw->core.background_pixel = $3 ;
	    tw->tbl.child_background_color = $3 ;
	    }
	| T_BACKGROUND_COLOR '(' INTEGER ')'
	    {
	    tw->core.background_pixel = $3 ;
	    }
	| C_BACKGROUND_COLOR '(' INTEGER ')'
	    {
	    tw->tbl.child_background_color = $3 ;
	    }
	| resize_participants '(' participants ')'
	    {
	    }
	;

resize_participants : RESIZE_PARTICIPANTS
		{
		tw->tbl.resize_participants = XcuRESIZE_NONE ;
		}
		    ;

participants : participant
	     | participants participant
	     ;

participant  : INTERS
		{
		tw->tbl.resize_participants =
		   (XcuResizeParticipants) ((int)tw->tbl.resize_participants |
					   (int)XcuRESIZE_INTERS) ;
		}
	     | INTERNALS
		{
		tw->tbl.resize_participants =
		   (XcuResizeParticipants) ((int)tw->tbl.resize_participants |
					   (int)XcuRESIZE_INTERNALS) ;
		}
	     | CHILDREN
		{
		tw->tbl.resize_participants =
		   (XcuResizeParticipants) ((int)tw->tbl.resize_participants |
					   (int)XcuRESIZE_CHILDREN) ;
		}
	     ;

element_series	: element
		| element_series element
		;

element	: ITEM_ELEMENT
    {
    char *ptr ;
    int nc = tw->tbl.n_cols ;

    if (nc == MAX_TBL_COLS)
	col_exceeded () ;

    tw->tbl.temp_elements[nc].primary = TBL_ITEM ;

    /* check for qualifiers */

    ptr = $1 ;
    while (*ptr != 0)
	{
	switch (*ptr++)
	  {
	  case 'l' : case 'L' :
	    {
	    tw->tbl.temp_elements[nc].justify = XcuJUSTIFY_LEFT ;
	    break ;
	    }
	  case 'r' : case 'R' :
	    {
	    tw->tbl.temp_elements[nc].justify = XcuJUSTIFY_RIGHT ;
	    break ;
	    }
	  case 'c' : case 'C' :
	    {
	    tw->tbl.temp_elements[nc].justify = XcuJUSTIFY_CENTER;
	    break ;
	    }
	  case 'e' : case 'E' :
	    {
	    tw->tbl.temp_elements[nc].e = True ;
	    break ;
	    }
	  case '@' :
	    {
	    tw->tbl.temp_elements[nc].f = True ;
	    break ;
	    }
	  case 'g' : case 'G' :
	    {
	    switch (*ptr)
	      {
	      case 'n' : case 'N' :
		{
		switch (*(ptr+1))
		  {
		  case 'e' : case 'E' :
		    {
		    tw->tbl.temp_elements[nc].g = XcuNORTH_EAST_GRAVITY ;
		    ptr += 2 ;
		    break ;
		    }
		  case 'w' : case 'W' :
		    {
		    tw->tbl.temp_elements[nc].g = XcuNORTH_WEST_GRAVITY ;
		    ptr += 2 ;
		    break ;
		    }
		  default :
		    {
		    tw->tbl.temp_elements[nc].g = XcuNORTH_GRAVITY ;
		    ptr += 1 ;
		    break ;
		    }
		  }
		break ;
		}
	      case 's' : case 'S' :
		{
		switch (*(ptr+1))
		  {
		  case 'e' : case 'E' :
		    {
		    tw->tbl.temp_elements[nc].g = XcuSOUTH_EAST_GRAVITY ;
		    ptr += 2 ;
		    break ;
		    }
		  case 'w' : case 'W' :
		    {
		    tw->tbl.temp_elements[nc].g = XcuSOUTH_WEST_GRAVITY ;
		    ptr += 2 ;
		    break ;
		    }
		  default :
		    {
		    tw->tbl.temp_elements[nc].g = XcuSOUTH_GRAVITY ;
		    ptr += 1 ;
		    break ;
		    }
		  }
		break ;
		}
	      case 'e' : case 'E' :
		{
		tw->tbl.temp_elements[nc].g = XcuEAST_GRAVITY ;
		ptr++ ;
		break ;
		}
	      case 'w' : case 'W' :
		{
		tw->tbl.temp_elements[nc].g = XcuWEST_GRAVITY ;
		ptr++ ;
		break ;
		}
	      case 'c' : case 'C' :
		{
		tw->tbl.temp_elements[nc].g = XcuCENTER_GRAVITY ;
		ptr++ ;
		break ;
		}
	      default :
		{
		char *my_text = "XcuTbl: Invalid gravity specifier: (%c)\n" ;
		char *etext = XtMalloc (strlen(my_text) + 1) ;
		sprintf (etext, my_text, *ptr) ;
		XtWarning (etext) ;
		break ;
		}
	      }
	    break ;
	    }
	  default :
	    {
	    break ;
	    }
	  }
	}

    XtFree ($1) ;
    tw->tbl.n_cols++ ;
    }
	| VSPAN_ELEMENT
    {
    if (tw->tbl.n_cols == MAX_TBL_COLS)
	col_exceeded () ;
    tw->tbl.temp_elements[tw->tbl.n_cols++].primary = TBL_VSPAN ;
    }
	| HSPAN_ELEMENT
    {
    if (tw->tbl.n_cols == MAX_TBL_COLS)
	col_exceeded () ;
    tw->tbl.temp_elements[tw->tbl.n_cols++].primary = TBL_HSPAN ;
    }
	;

%%

/***** **** *** ** * find_strings * ** *** **** *****/

static void
find_strings (text, tw)
    char *text ;
    XcuTblWidget tw ;
{
/*
 * One quick pass to find out the number of sub-strings
 */
int *first_char ;
int *n_chars ;
Cardinal i, j ;
Cardinal istring ;
Cardinal n_strings ;
int length = strlen (text) ;

tw->tbl.n_cols = 0 ;
for (i=0;  i < length;  i++)
    {
    if (text[i] == tw->tbl.string_breaker)
	{
	tw->tbl.n_cols++ ;
	}
    }
tw->tbl.n_cols++ ;

/*
 * Check to see if there is a corresponding format row.
 * If not, duplicate the last format row.
 */

if (tw->tbl.data_row + 1 > tw->tbl.rows)
    {
    Cardinal i ;

    if (tw->tbl.rows == 0)
	no_rows () ;
    if (tw->tbl.rows == MAX_TBL_ROWS)
	row_exceeded () ;

    /* allocate just enough space for the number of columns in this row */

    tw->tbl.item[tw->tbl.rows] =
	(ElementRecord *) XtCalloc (tw->tbl.cols[tw->tbl.rows-1],
				    sizeof (ElementRecord)) ;
    /* make a copy of the previous row */
    for (i=0;  i < tw->tbl.cols[tw->tbl.rows-1];  i++)
	tw->tbl.item[tw->tbl.rows][i] = tw->tbl.item[tw->tbl.rows-1][i] ;
    tw->tbl.cols[tw->tbl.rows] = tw->tbl.cols[tw->tbl.rows-1] ;
    tw->tbl.rows++ ;
    }

/*
 * Check to see that the number of data items matches the number of format items
 */

 {
 Cardinal n_items = 0 ;
 for (i=0;  i < tw->tbl.cols[tw->tbl.data_row];  i++)
    if (tw->tbl.item[tw->tbl.data_row][i].primary == TBL_ITEM ||
	tw->tbl.item[tw->tbl.data_row][i].primary == TBL_VSPAN)
	n_items++ ;

 if (tw->tbl.n_cols != n_items)
    {
    mismatch ((Cardinal)tw->tbl.n_cols, n_items, tw->tbl.data_row);
    }
 }

/*
 * Do another pass of the string to find the starting points and lengths
 * of the the substrings
 */

first_char = (int *) XtMalloc (tw->tbl.n_cols * sizeof (int)) ;
n_chars = (int *) XtMalloc (tw->tbl.n_cols * sizeof (int)) ;

n_strings = 0 ;
first_char[0] = 0 ;

for (i=0;  i < length;  i++)
    {
    if (text[i] == tw->tbl.string_breaker)
	{
	n_chars[n_strings] = i - first_char[n_strings] ;
	n_strings++ ;
	first_char[n_strings] = i+1 ;
	}
    }
n_chars[n_strings] = length - first_char[n_strings] ;

/*
 * Now copy the substrings into the item records
 */

istring = 0 ;
for (j=0;  j < tw->tbl.cols[tw->tbl.data_row];  j++)
    {
    if (tw->tbl.item[tw->tbl.data_row][j].primary != TBL_ITEM)
	{
	if (tw->tbl.item[tw->tbl.data_row][j].primary == TBL_VSPAN)
	    istring++ ;
	continue ;
	}
    tw->tbl.item[tw->tbl.data_row][j].text = XtMalloc (n_chars[istring] + 1) ;
    strncpy (tw->tbl.item[tw->tbl.data_row][j].text,
	     &text[first_char[istring]], n_chars[istring]) ;
    tw->tbl.item[tw->tbl.data_row][j].text[n_chars[istring]] = '\0' ;
    /* Convert instances of "\n" to '\n' */
    convert_nl (tw->tbl.item[tw->tbl.data_row][j].text) ;

    istring++ ;
    }

tw->tbl.data_row++ ;
tw->tbl.n_cols = 0 ;

XtFree ((char *) first_char) ;
XtFree ((char *) n_chars) ;

return ;
}

/***** **** *** ** * row_exceeded * ** *** **** *****/

static void
row_exceeded ()
{
char *my_text = "XcuTbl: MAX_TBL_ROWS exceeded: %d\n" ;
char *etext = XtMalloc (strlen(my_text) + 5) ;
sprintf (etext, my_text, MAX_TBL_ROWS) ;
XtError (etext) ;
}

/***** **** *** ** * no_rows * ** *** **** *****/

static void
no_rows ()
{
XtError ("XcuTbl: No format items found before data!\n") ;
}

/***** **** *** ** * col_exceeded * ** *** **** *****/

static void
col_exceeded ()
{
char *my_text = "XcuTbl: MAX_TBL_COLS exceeded: %d\n" ;
char *etext = XtMalloc (strlen(my_text) + 5) ;
sprintf (etext, my_text, MAX_TBL_COLS) ;
XtError (etext) ;
}

/***** **** *** ** * mismatch * ** *** **** *****/

static void
mismatch (data_items, format_items, row_number)
Cardinal data_items ;
Cardinal format_items ;
Cardinal row_number ;
{
char *my_text = "XcuTbl: item/format mismatch in data row # %d (%d != %d)\n" ;
char *etext = XtMalloc (strlen(my_text) + 15) ;
sprintf (etext, my_text, row_number, data_items, format_items) ;
XtError (etext) ;
}

/***** **** *** ** * convert_nl * ** *** **** *****/

/*
 * if no newline equivalent,
 * convert instances of "\n" into '\n' and collapse the remainder of the text
 * else,
 * convert the equivalent into a newline
 */

static void
convert_nl (text)
    char *text ;
{
Cardinal i, j ;
Cardinal len = strlen(text) ;

for (i=0;  i < len;  i++)
    {
    if (layout_input->snl_equivalent == -1)
	{
	if (text[i] == '\\' && text[i+1] == 'n')
	    {
	    text[i] = '\n' ;
	    j = i+1 ;
	    while (text[j] != 0)
		{
		text[j] = text[j+1] ;
		j++ ;
		}
	    }
	}
    else
    if (text[i] == layout_input->snl_equivalent)
	text[i] = '\n' ;
    }
return ;
}

#include "Tbl.lex.c"
