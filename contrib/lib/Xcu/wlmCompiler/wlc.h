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


/* Wlm widget private definitions */

#ifndef _WlmCompiler_h
#define _WlmCompiler_h

/*
 * Prelude
 */

typedef enum {
	     WlmLocalContext, WlmGlobalContext
	     } WlmContext ;

typedef struct WlmManageItem
    {
    String widget ;
    String type ;
    String value ;
    struct WlmManageItem *next ;
    } WlmManageItem ;

#define MAX_DIRECTIVE_ARGUMENTS 10
#define MAX_DIRECTIVE_CALLDATA_ITEMS 10
#define MAX_DIRECTIVE_CALLDATA_INDICES 2

typedef enum
    {
     WlmDirectiveEquivalence
    ,WlmDirectiveNonEquivalence
    ,WlmDirectiveGreaterThan
    ,WlmDirectiveLessThan
    ,WlmDirectiveGreaterThanOrEqualTo
    ,WlmDirectiveLessThanOrEqualTo
    } WlmDirectiveOperator ;

typedef struct Resource
    {
    String name ;
    String class ;
    String value ;
    WlmContext context ;
    struct Resource *next ;
    } Resource ;

typedef struct Directive
    {
    String callback_name ;
    String call_data_converter[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    String call_data[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    WlmDirectiveOperator call_data_operator[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    int call_data_index[MAX_DIRECTIVE_CALLDATA_ITEMS]
			[MAX_DIRECTIVE_CALLDATA_INDICES] ;
    int      n_call_comparisons ;
    Cardinal n_call_indices[MAX_DIRECTIVE_CALLDATA_ITEMS] ;
    String target_class ;
    String target_name ;
    Resource resource ;
    String procedure ;
    Cardinal n_arguments ;
    String argument_converters[MAX_DIRECTIVE_ARGUMENTS] ;
    String argument_strings[MAX_DIRECTIVE_ARGUMENTS] ;
    long client_data[3] ;
    struct Directive *next ;
    } Directive ;

typedef struct Fetus
    {
    String class_name ;
    Cardinal n_resources ;
    Resource *resource ;
    Cardinal n_directives ;
    Directive *directives ;
    Cardinal n_children ;
    struct Fetus **children ;
    Cardinal n_manage_list ;
    WlmManageItem *manage_list ;
    Widget id ;
    } Fetus ;

typedef struct
    {
    String layout_file ;
    String compiled_file ;
    String class_file ;
    String wldl_file ;
    Cardinal cur_depth ;
    Cardinal max_depth ;
    Stack *child_stack ;
    } WlmCompilerPart ;

#endif _WlmCompiler_h
