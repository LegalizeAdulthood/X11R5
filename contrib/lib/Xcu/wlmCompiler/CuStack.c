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


#include <stdio.h>
#include "CuStack.h"

void
Cu_stack_create (name, stack, chunk_size)
    char *name ;
    Stack **stack ;
    int chunk_size ;
{
(*stack) = (Stack *) malloc (sizeof (Stack)) ;
(*stack)->n = 0 ;
(*stack)->name = (char *) malloc (strlen(name)+1) ;
strcpy ((*stack)->name, name) ;
Cu_grow_create (&(*stack)->grow, chunk_size) ;
return ;
}

void
Cu_stack_push (stack, value)
    Stack *stack ;
    int value ;
{
Cu_grow_check (stack->grow, stack->n) ;
stack->grow->s[stack->n++] = value ;
return ;
}

int 
Cu_stack_pull (stack)
    Stack *stack ;
{
int i, bottom ;

bottom = stack->grow->s[0] ;
--stack->n ;
for (i=0;  i < stack->n;  i++)
	stack->grow->s[i] = stack->grow->s[i+1] ;
return bottom ;
}

int
Cu_stack_pop (stack)
    Stack *stack ;
{
int ret_value ;
ret_value = stack->grow->s[--stack->n] ;
return ret_value ;
}


int 
Cu_stack_peek (stack, index)
    Stack *stack ;
    int index ;
{
int ret_value ;
if (index == STACK_NEXT_POP)
    index = stack->n - 1 ;
else
if (index == STACK_NEXT_PULL)
    index = 0 ;

if (stack->n)
	ret_value = stack->grow->s[index] ;
else
	ret_value = NULL ;
return ret_value ;
}

void
Cu_stack_kill (stack, number)
    Stack *stack ;
    int number ;
{
stack->n -= number ;
return ;
}

int
Cu_stack_size (stack)
    Stack *stack ;
{
return stack->n ;
}
