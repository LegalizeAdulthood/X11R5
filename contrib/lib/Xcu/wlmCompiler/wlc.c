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


/* *
 * * This code is completely original.  Permission is granted to do with
 * * it as one wishes.  I disclaim any responsibility whatsoever for
 * * any bad things that may happen because of using it.
 * */

#include <stdio.h>
#include <X11/Xos.h>
#include <sys/stat.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xresource.h>
#include "CuStack.h"
#include "wlc.h"

/* Private Definitions */

static char error_text[200] ;

/* Declarations of "private" calls */

static void	read_layout () ;
static void	compile () ;
static void	fetus_print () ;
static void	managed_print () ;
static void	resources_print () ;
static void	directives_print () ;
static void	pad_print () ;

static int	input () ;
static void	unput () ;
static void	yyerror () ;
static int	yywrap () ;

/**
 ***
 **** Wlm Compiler
 ***
 **/

#define WLM_MAX_UNPUT 2000
#define WLM_MAX_CONTEXT 2000

typedef struct
    {
    String input_buffer ;
    Cardinal input_pointer ;
    char unput_buffer[WLM_MAX_UNPUT] ;
    Cardinal unput_pointer ;
    char context_buffer[WLM_MAX_CONTEXT] ;
    int context_ptr ; /* must not be Cardinal */
    Cardinal line_number ;
    String cur_file ;
    } WlmInputRecord ;

static WlmInputRecord *layout_input ;

static WlmCompilerPart _ww, *ww = &_ww ;

main (argc, argv)
    int argc ;
    char *argv[] ;
{
char *input_file ;
int argv_length ;
if (argc < 2)
    {
    fprintf (stderr, "Usage: %s WLDL_file\n", argv[0]) ;
    exit (1) ;
    }

argv_length = strlen(argv[1]) ;
if (strcmp(&argv[1][argv_length-3], ".wl") == 0)
    {
    /* This permits extensioned or extensionless input */
    argv[1][argv_length-3] = '\0' ;
    argv_length -= 3 ;
    }
ww->compiled_file = XtMalloc (argv_length + 4) ;
ww->class_file = XtMalloc (argv_length + 4) ;
ww->wldl_file = XtMalloc (argv_length + 4) ;
input_file = XtMalloc (argv_length + 4) ;
sprintf (ww->compiled_file, "%s.wc", argv[1]) ;
sprintf (ww->class_file, "%s.cl", argv[1]) ;
sprintf (ww->wldl_file, "%s.lw", argv[1]) ;
sprintf (input_file, "%s.wl", argv[1]) ;

/*
 * We'll read the file as one big string, then parse it.
 */

layout_input = (WlmInputRecord *) XtMalloc (sizeof (WlmInputRecord)) ;
read_layout (input_file, layout_input) ;
yyparse () ;
compile (ww->child_stack) ;

exit (0) ;
}

/***** **** *** ** * read_layout * ** *** **** *****/

static void
read_layout (file, layout_input)
    String file ;
    WlmInputRecord *layout_input ;
{
FILE *fd ;
struct stat buf ;

fd = fopen (file, "r") ;
if ((int)fd <= 0)
    {
    char *my_text = "Wlm: Couldn't open (%s) for reading\n" ;
    char *etext = XtMalloc (strlen(my_text) + strlen(file)) ;
    sprintf (etext, my_text, file) ;
    XtError (etext) ;
    }

/*
 * Find out how big the layout file is.
 * Read it in as one big chunk.
 * Initialize some useful variables
 */

fstat (fileno(fd), &buf) ;
layout_input->input_buffer = XtMalloc (buf.st_size + 1) ;
fread (layout_input->input_buffer, buf.st_size, 1, fd) ;
layout_input->input_buffer[buf.st_size] = '\0' ;

Cu_copy_ds (&layout_input->cur_file, file) ; /* for error messages */
layout_input->input_pointer = 0 ;
layout_input->unput_pointer = 0 ;
layout_input->line_number = 1 ;
fclose (fd) ;

return ;
}

typedef struct WlmClassnameList
    {
    String name ;
    struct WlmClassnameList *next ;
    } WlmClassnameList ;

typedef struct WlmWldlList
    {
    String name ;
    struct WlmWldlList *next ;
    } WlmWldlList ;

static WlmClassnameList *listC ;
static WlmWldlList *listW ;

static void
compile (stack)
    Stack *stack ;
{
FILE *fd ;
WlmClassnameList *chkC ;
WlmWldlList *chkW ;

listC = NULL ;
fd = fopen (ww->compiled_file, "w") ;
fprintf (fd, "#WLDL\n") ;
fprintf (fd, "%d\n", ww->max_depth) ;
fetus_print (fd, (Fetus *)Cu_stack_peek(stack, STACK_NEXT_POP), 0) ;
fclose (fd) ;

fd = fopen (ww->class_file, "w") ;
for (chkC=listC;  chkC != NULL;  chkC = chkC->next)
    {
    fprintf (fd, "%s\n", chkC->name) ;
    }
fclose (fd) ;

fd = fopen (ww->wldl_file, "w") ;
for (chkW=listW;  chkW != NULL;  chkW = chkW->next)
    {
    fprintf (fd, "%s\n", chkW->name) ;
    }
fclose (fd) ;

return ;
}

#define LSTRNG(x) strlen(x), x

static void
fetus_print (fd, fetus, pad)
    FILE *fd ;
    Fetus *fetus ;
    int pad ;
{
Resource *resource ;
Directive *directive ;
WlmManageItem *managed ;
Cardinal j ;

WlmClassnameList *chk ;
for (chk=listC;  chk != NULL;  chk = chk->next)
    {
    if (strcmp (chk->name, fetus->class_name) == 0)
	break ;
    }
if (chk == NULL)
    {
    chk = listC ;
    listC = (WlmClassnameList *) XtMalloc (sizeof (WlmClassnameList)) ;
    listC->name = fetus->class_name ;
    listC->next = chk ;
    }

fprintf (fd, "\n") ;
pad_print (fd, pad) ;
fprintf (fd, "%d %s\n", LSTRNG(fetus->class_name)) ;

pad_print (fd, pad) ;
fprintf (fd, "%d\n", fetus->n_resources) ;
for (resource=fetus->resource;  resource != NULL;  resource=resource->next)
    {
    resources_print (fd, resource, pad) ;
    if (strcmp (fetus->class_name, "Wlm") == 0 &&
        strcmp (resource->name, "file") == 0)
	{
	WlmWldlList *save = listW ;
	listW = (WlmWldlList *) XtMalloc (sizeof (WlmWldlList)) ;
	listW->name = resource->value ;
	listW->next = save ;
	}
    }

pad_print (fd, pad) ;
fprintf (fd, "%d\n", fetus->n_directives) ;
for (directive=fetus->directives;  directive != NULL; directive=directive->next)
    directives_print (fd, directive, pad) ;

pad_print (fd, pad) ;
fprintf (fd, "%d\n", fetus->n_manage_list) ;
for (managed=fetus->manage_list;  managed != NULL; managed=managed->next)
    managed_print (fd, managed, pad) ;

pad_print (fd, pad) ;
fprintf (fd, "%d\n", fetus->n_children) ;
pad += 2 ;
for (j=0;  j < fetus->n_children;  j++)
    fetus_print (fd, fetus->children[j], pad) ;

return ;
}

static void
managed_print (fd, managed, pad)
    FILE *fd ;
    WlmManageItem *managed ;
    int pad ;
{
pad_print (fd, pad) ;
fprintf (fd, "%d (%s) %d %s %d (%s)\n",
    LSTRNG(managed->widget), LSTRNG(managed->type), LSTRNG(managed->value)) ;
return ;
}

static void
resources_print (fd, resource, pad)
    FILE *fd ;
    Resource *resource ;
    int pad ;
{
pad_print (fd, pad) ;
fprintf (fd, "%d %s %d (%s) %d %s\n",
    LSTRNG(resource->name), LSTRNG(resource->value),
    (resource->context == WlmLocalContext) ? strlen("Local") : strlen("Global"),
    (resource->context == WlmLocalContext) ? "Local" : "Global") ;
return ;
}

static void
directives_print (fd, directive, pad)
    FILE *fd ;
    Directive *directive ;
    int pad ;
{
Cardinal i ;
pad_print (fd, pad) ;
fprintf (fd, "%d %s %d %s %d %s %d %d %s\n",
    LSTRNG(directive->callback_name), LSTRNG(directive->target_class),
    LSTRNG(directive->target_name), directive->n_call_comparisons,
    directive->resource.name ?
	strlen("XtSetValues") : strlen("PublicFunction"),
    directive->resource.name ?
	"XtSetValues" : "PublicFunction") ;

pad += 1 ;
for (i=0;  i < directive->n_call_comparisons;  i++)
    {
    Cardinal j ;
    static char *op_text[] = { "==", "!=", ">", "<", ">=", "<=" } ;

    pad_print (fd, pad) ;
    fprintf (fd, "%d %s %d (%s) %d %d %s %d :",
	LSTRNG(directive->call_data_converter[i]),
	LSTRNG(directive->call_data[i]),
	directive->call_data_operator[i],
	LSTRNG(op_text[(int)directive->call_data_operator[i]]),
	directive->n_call_indices[i]) ;

    for (j=0;  j < directive->n_call_indices[i];  j++)
	fprintf (fd, " %d", directive->call_data_index[i][j]) ;
    fprintf (fd, "\n") ;
    }
pad -= 1 ;

if (directive->resource.name)
    {
    pad_print (fd, pad) ;
    fprintf (fd, "%d %s %d (%s) %d %s %d (%s)\n",
	LSTRNG(directive->target_class), LSTRNG(directive->target_name),
	LSTRNG(directive->resource.name), LSTRNG(directive->resource.value)
	) ; 
    }
else
    {
    Cardinal i ;
    pad_print (fd, pad) ;
    fprintf (fd, "%d %s %d\n", LSTRNG(directive->procedure),
		directive->n_arguments) ;

    for (i=0;  i < directive->n_arguments;  i++)
	{
	pad_print (fd, pad) ;
	fprintf (fd, "%d %s %d (%s)\n",
	    LSTRNG(directive->argument_converters[i]),
	    LSTRNG(directive->argument_strings[i])) ;
	}
    }

return ;
}

static void
pad_print (fd, pad)
    FILE *fd ;
    int pad ;
{
while (pad--)
    fprintf (fd, " ") ;
return ;
}


/**
 ** The routines used to parse a wlm description
 **/

/***** **** *** ** * "yyerror" * ** *** **** *****/

static void
yyerror (s)
    char *s ;
{
Cardinal length ;
String my_text = "Wlm: Parsing error (%s)\nLine # %d in file '%s':\n" ;
String etext ;
length = strlen(my_text) + strlen(s) + WLM_MAX_CONTEXT + 20 ;
etext = XtMalloc (length) ;
sprintf (etext, my_text, s, layout_input->line_number, layout_input->cur_file) ;
strcat (etext, layout_input->context_buffer) ;
XtError (etext) ;
}

/***** **** *** ** * the lex input routine * ** *** **** *****/

static int
input ()
{
int t ;
if (layout_input->unput_pointer)
    {
    t = layout_input->unput_buffer[layout_input->unput_pointer-1] ;
    layout_input->unput_pointer-- ;
    }
else
    {
    t = layout_input->input_buffer[layout_input->input_pointer++] ;
    }

if (t == '\n')
    {
    layout_input->line_number++ ;
    layout_input->context_ptr = 0 ;
    }
else
if (layout_input->context_ptr < WLM_MAX_CONTEXT)
    {
    layout_input->context_buffer[layout_input->context_ptr++] = t ;
    layout_input->context_buffer[layout_input->context_ptr] = '\0' ;
    }
else
    {
    sprintf (error_text,
	    "Insufficient context buffer space for input character (%c)\n", t) ;
    XtWarning (error_text) ;
    }

return t ;
}

/***** **** *** ** * the lex unput routine * ** *** **** *****/

static void
unput (c)
    int c ;
{
layout_input->unput_buffer[layout_input->unput_pointer++] = c ;

/*
 * Just wipe out currrent line context characters.  Don't try to retrieve those
 * already cycled out. (It seems highly unlikely that anything significant
 * will ever be lost by this.)
 */

if (layout_input->context_ptr > 0)
    layout_input->context_ptr-- ;

if (c == '\n')
    if (layout_input->line_number > 1)
	layout_input->line_number-- ;

return ;
}

#include "wlc.y.c"

