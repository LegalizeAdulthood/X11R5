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


%{
/* global declaractions */

Stack *fetus_stack ;
Stack *incl_stack ;

static Directive *directive ;

static char *resource_unify () ;
static char *perm_string = "String" ;

%}

%start	init

%union	{
	float	f_val ;
	int	i_val ;
	char	*s_val ;
	char	c_val ;
	WlmDirectiveOperator op_val ;
	}

%type <op_val>	test_operator
%type <s_val>	resource_word
%token <s_val>	WORD
%token <s_val>	STRING
%token		IF_KEY
%token		SET_VALUES_KEY
%token		MANAGE_KEY
%token		INCL_KEY
%token 		ERROR

%%

init		:
    {
    Cu_stack_create ("Fetus", &fetus_stack, 100) ;
    Cu_stack_create ("Child", &ww->child_stack, 100) ;
    Cu_stack_create ("Include", &incl_stack, 10) ;
    ww->cur_depth = 0 ;
    ww->max_depth = 0 ;
    }
		| init widget_equivalent
		;

widget_equivalent : widget
		  | inclusion
		  ;

inclusion	:	INCL_KEY STRING
	{
	Cu_stack_push (incl_stack, (int) layout_input) ;
	layout_input = (WlmInputRecord *)XtCalloc(1, sizeof (WlmInputRecord)) ;
	read_layout ($2, layout_input) ;
	}
		;

widget		: widget_header '{' widget_body '}'
    {
    Fetus *fetus ;
    Fetus *parent ;
    /*
     * pop off fetus stack,
     * pop children (if any) from child_stack
     * push the widget to child stack
     */
    fetus = (Fetus *) Cu_stack_pop (fetus_stack) ;
    parent = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    if (parent)
	{
	parent->n_children++ ;
	}
    if (fetus->n_children)
	{
	Cardinal i ;

	fetus->children = (Fetus **)
			  XtMalloc (fetus->n_children * sizeof (Fetus *)) ;
	for (i=0;  i < fetus->n_children;  i++)
	    {
	    fetus->children[i] = (Fetus *) Cu_stack_pop (ww->child_stack) ;
	    }
	}
    Cu_stack_push (ww->child_stack, (int) fetus) ;
    ww->cur_depth-- ;
    }
		| widget_header '{' '}'
		;

widget_header	: WORD
    {
    /* allocate and push onto fetus stack */
    Fetus *fetus = (Fetus *) XtMalloc (sizeof (Fetus)) ;
    fetus->class_name = $1 ;
    fetus->n_directives = 0 ;
    fetus->directives = NULL ;
    fetus->n_children = 0 ;
    fetus->n_resources = 0 ;
    fetus->resource = NULL ;
    fetus->n_manage_list = 0 ;
    fetus->manage_list = NULL ;
    Cu_stack_push (fetus_stack, (int) fetus) ;
    ww->cur_depth++ ;
    if (ww->cur_depth > ww->max_depth)
	ww->max_depth = ww->cur_depth ;
    }
		;

widget_body	: widget_element
		| widget_body widget_element
		;

widget_element	: directive
    {
    Fetus *fetus = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    Directive *save = fetus->directives ;
    fetus->directives = directive ;
    directive->next = save ;
    fetus->n_directives++ ;
    }
		| widget_resource
		| manage_resource
		| global_resource
		| widget_equivalent
		;

global_resource	: '*' resource_word STRING
    {
    Fetus *fetus = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    Resource *save = fetus->resource ;
    fetus->resource = (Resource *) XtMalloc (sizeof(Resource)) ;
    fetus->resource->name = resource_unify ($2) ;
    fetus->resource->value = $3 ;
    fetus->resource->context = WlmGlobalContext ;
    fetus->resource->next = save ;
    fetus->n_resources++ ;
    }
		;

resource_word : WORD
    {
    }
	      | WORD '*' resource_word
    {
    $$ = XtMalloc (strlen($1) + strlen($3) + 2) ;
    sprintf ($$, "%s*%s", $1, $3) ;
    }
	      ;

widget_resource	: WORD STRING
    {
    Fetus *fetus = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    Resource *save = fetus->resource ;
    fetus->resource = (Resource *) XtMalloc (sizeof(Resource)) ;
    fetus->resource->name = resource_unify ($1) ;
    fetus->resource->value = $2 ;
    fetus->resource->context = WlmLocalContext ;
    fetus->resource->next = save ;
    fetus->n_resources++ ;
    }
		;

manage_resource : MANAGE_KEY manage_args
		;

manage_args	: manage_arg
		| manage_args maybe_comma manage_arg
		;

manage_arg	: STRING ':' WORD STRING
    {
    /* For now, presume that "Button" is the only valid class */
    Fetus *fetus = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    WlmManageItem *save = fetus->manage_list ;
    fetus->manage_list = (WlmManageItem *) XtMalloc (sizeof(WlmManageItem)) ;
    fetus->manage_list->widget = $1 ;
    fetus->manage_list->type = $3 ;
    fetus->manage_list->value = $4 ;
    fetus->manage_list->next = save ;
    fetus->n_manage_list++ ;
    }
		| STRING
    {
    Fetus *fetus = (Fetus *) Cu_stack_peek (fetus_stack, STACK_NEXT_POP) ;
    WlmManageItem *save = fetus->manage_list ;
    fetus->manage_list = (WlmManageItem *) XtMalloc (sizeof(WlmManageItem)) ;
    fetus->manage_list->widget = $1 ;
    fetus->manage_list->type = perm_string ;
    fetus->manage_list->value = $1 ;
    fetus->manage_list->next = save ;
    fetus->n_manage_list++ ;
    }
		;

directive : directive_head callback_name '(' test_group ')' r_target_info
	  | directive_head callback_name '(' test_group ')' f_target_info
	  ;

directive_head	: IF_KEY
    {
    /* Make sure that everything is zeroed */
    directive = (Directive *) XtCalloc (1, sizeof(Directive)) ;
    directive->n_call_comparisons = 0 ;
    }
		;

callback_name	: WORD
    {
    directive->callback_name = $1 ;
    }
		;

test_group	:
		| test_series
		;

test_series	: test_pair
		| test_series test_conjunction test_pair
		;

test_pair	: WORD STRING test_operator WORD test_indices
    {
    directive->call_data_converter[directive->n_call_comparisons] = $1 ;
    directive->call_data[directive->n_call_comparisons] = $2 ;
    directive->call_data_operator[directive->n_call_comparisons] = $3 ;
    directive->n_call_comparisons++ ;
    }
		| WORD test_indices test_operator WORD STRING
    {
    directive->call_data_converter[directive->n_call_comparisons] = $4 ;
    directive->call_data[directive->n_call_comparisons] = $5 ;
    directive->call_data_operator[directive->n_call_comparisons] = $3 ;
    directive->n_call_comparisons++ ;
    }
		;

r_target_info	: SET_VALUES_KEY '(' r_target_args ')'
		;

r_target_args	: target_class target_name ',' resource_name maybe_comma  resource_value
		;

f_target_info	: procedure_name '(' f_target_args ')'
		;

f_target_args	: target_class target_name target_args
		;

maybe_comma	:
		| ','
		;

target_class	: WORD
    {
    directive->target_class = $1 ;
    }

target_name	: STRING
    {
    directive->target_name = $1 ;
    }

resource_name	: WORD
    {
    directive->resource.name = resource_unify ($1) ;
    }

procedure_name	: WORD
    {
    directive->procedure = $1 ;
    directive->n_arguments = 0 ;
    }

resource_value	: STRING
    {
    directive->resource.value = $1 ;
    }

target_args	: 
	 	| ',' argument_list
		;

argument_list	: argument
		| argument_list ',' argument
		;

argument	: WORD STRING
    {
    directive->argument_converters[directive->n_arguments] = $1 ;
    directive->argument_strings[directive->n_arguments] = $2 ;
    directive->n_arguments++ ;
    }
		;

test_conjunction : '&' '&'
		 ;

test_operator	: '=' '='
    {
    $$ = WlmDirectiveEquivalence ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		| '!' '='
    {
    $$ = WlmDirectiveNonEquivalence ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		| '>' 
    {
    $$ = WlmDirectiveGreaterThan ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		| '<'
    {
    $$ = WlmDirectiveLessThan ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		| '>' '='
    {
    $$ = WlmDirectiveGreaterThanOrEqualTo ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		| '<' '='
    {
    $$ = WlmDirectiveLessThanOrEqualTo ;
    directive->n_call_indices[directive->n_call_comparisons] = 0 ;
    }
		;

test_indices	:
		| index_series
		;

index_series	: test_index
		| index_series test_index
		;

test_index	: '[' WORD ']'
    {
    directive->call_data_index[directive->n_call_comparisons]
			      [directive->n_call_indices[directive->n_call_comparisons]] = atoi($2) ;
    directive->n_call_indices[directive->n_call_comparisons]++ ;
    }
		;

%%

/*
 * -v => y.output
 * -d => y.tab.h
 * -l => no #lines (for -g (cc))
 *
 * left recursion is better (as in init above)
 */

#include <ctype.h>

static char *
resource_unify (string)
    char *string ;
{
/* replace occurrences of "_x" with "X" */
int length = strlen (string) ;
Cardinal i, j ;
Cardinal start =0 ;

if (strncmp (string, "XtN", 3) == 0)
    start = 3 ;

for (i=start;  i < length-1;  i++)
    {
    if (string[i] == '_' && islower(string[i+1]))
	{
	string[i] = toupper(string[i+1]) ;
	for (j=i+1;  j < length-1;  j++)
	    string[j] = string[j+1] ;
	string[j] = '\0' ;
	}
    }

return &string[start] ;
}

static int
yywrap ()
{
XtFree (layout_input->input_buffer) ;
XtFree (layout_input) ;

if (Cu_stack_size (incl_stack) == 0)
    {
    return 1 ;
    }

layout_input = (WlmInputRecord *) Cu_stack_pop (incl_stack) ;
return 0 ;
}

#include "wlc.lex.c"
