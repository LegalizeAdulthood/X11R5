#ifndef _TRIE_H_INCLUDE
#define _TRIE_H_INCLUDE

#include "bitstream.h"
     
/* $Header: /usr3/Src/emu/term/parser/RCS/trie.h,v 2.10 90/11/20 17:48:16 jkh Exp Locker: jkh $ */
     
/*
 * This file is part of the PCS emu program.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * Useful defines for the trie parser.
 *
 * Author: Terry Jones and Jordan K. Hubbard
 * Date: March 14th, 1990.
 * Description: All the macro and type declarations for the parser section
 *		of the emu widget.
 *
 * Revision History:
 *
 * $Log:	trie.h,v $
 * Revision 2.10  90/11/20  17:48:16  jkh
 * 11th hour fix.
 * 
 * Revision 2.9  90/11/20  17:46:38  jkh
 * Alpha.
 * 
 * Revision 1.1  90/03/14  18:18:15  terry
 * Initial revision
 * 
 *
 */

/* Maximum number of characters in a string sequence */
#define MAX_STR_SEQ	(BUF_SZ / 2)

/* Maximum token size for META_TOKEN */
#define MAX_TOKEN	120

/* Maximum push depth for IOP lists */
#define MAX_IOPL	32

#define STACK_SZ   32

/* A stack */
typedef struct _stack {
     int sp;
     struct {
	  int type;
	  Generic value;
     } items[STACK_SZ];
} Stack;

#define TRIE_T		1
#define RPARSE_T	2

/* A request ID node. Just holds an opcode. */
typedef struct _request {
     int request_id;
     struct _request* next;
} Request;

/* An iop node. Holds the operation type and an argument. */
typedef struct _iop {
     short type;
     int value;
#ifdef IOP_DEBUG
     int break;
#endif
     struct _iop *next;
} Iop;

/* A chain of IOPs */
typedef struct _iopchain {
     Iop *head, *tail;
} IopChain;

/* A state in the trie. */
typedef struct _state {
     int final;				/* is this a final state? */
     struct _transition {
	  int label;			/* the transition character */
	  struct _state *state;		/* pointer back to parent */
	  IopChain iop;			/* any associated actions */
	  struct _transition *next;	/* next transaction in the chain */
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
	  int id;
#endif
     } *transition_list;
     Request *request_list;		/* requests to generate on final */
#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
     int id;
#endif
} State;

typedef struct _transition Transition;

/* A reverse parser list element. */
typedef struct _rparse {
     int id;
     IopChain ops;
     struct _rparse *next;
} Rparse;

/* A possible state for the current state list. */
typedef struct _possible_state {
     State *state;
     Transition *trans;
     Generic value;
     int aux;
     int handle_state;
     Iop *iopl[MAX_IOPL];
     bit_decl(mask, MAX_IOPL);
     int niopl;
     struct _possible_state *next;
} Possible_state;

/*
 * Iop 'characters' for the %d, %s and %c transitions.
 * These values must be outside the range of normal chars.
 */
#define READ_INT_TRAN		257
#define READ_STR_TRAN		258
#define READ_CHAR_TRAN		259
#define EPSILON_TRAN            260

/* codes for the iop transitions. */
#define META_READ_INT		0
#define META_READ_STR		1
#define META_CHAR_PUSH		2
#define META_INT_PUSH		3
#define META_STR_PUSH		4

#define META_ADD		'+'
#define META_AND		'&'
#define META_BUFFER		'b'	/* parm */
#define META_CALL		'C'
#define META_CAST		'@'
#define META_CHAR		'c'
#define META_COMP		'~'
#define META_DIV		'/'
#define META_DROP		'X'
#define META_DUP		'D'
#define META_ELSE		':'
#define META_ENDIF		';'
#define META_EQ			'='
#define META_FREE		'f'
#define META_GET		'g'	/* parm */
#define META_GOTO		'j'	/* parm */
#define META_GOTOU		'\012'  /* unsatisfied goto */
#define META_GT			'>'
#define META_IF			'?'
#define META_IGET		'G'
#define META_INT		'd'
#define META_IPOP_TO_REG	'P'
#define META_LABEL		'L'	/* parm */
#define META_LAND		'A'
#define META_LNOT		'!'
#define META_LOR		'O'
#define META_LT			'<'
#define META_MENU		'M'
#define META_MOD		'm'
#define META_MUL		'*'
#define META_NOP		';'
#define META_NOT		'^'
#define META_OR			'|'
#define META_POP_TO_REG		'p'	/* parm */
#define META_ROP		'R'
#define META_SIGNAL		'k'
#define META_STR		's'
#define META_STRLEN		'l'
#define META_SUB		'-'
#define META_SWAP		'S'
#define META_TITLE		't'
#define META_ICON		'i'
#define META_TOKEN		'$'
#define META_TTY		'T'	/* parm */
#define META_UP			'U'

/* Sub-selector (parm) types for various ops */
#define TTY_GET			'g'
#define TTY_SET			's'
#define TTY_IGET		'i'
#define TTY_SGET		'd'

/* Return values and states for handle() and friends. */
#define RECOGNISED		0
#define IN_PROGRESS		1
#define FLUSH			2
#define DELETE_STATE		5
#define HANDLE_NORMAL		6
#define HANDLE_READ_INT		7
#define HANDLE_READ_STR		8

Import Request *add_request(), *get_request(), *get_request_list();
Import State *add_transition(), *follow_transition();
Import Transition *has_transition();
Import Generic pop(), check_pop();
Import int handle_as_if(), next_char(), compare_iop();
Import int same_requests();
Import void emit(), handle_init(), handle_iops(), parse_adjust();
Import void push(), parse_add(), rparse_add(), dispatch_requests();
Import void destroy_current_states();
Import void add_iop(), handle_iops();
Import Rparse *rparse_find();
Import Possible_state *clean_state_list();
Import char *read_int();

/* Nuking routines */
Import void destroy_iops();
Import void destroy_requests();
Import void destroy_state();
Import void destroy_transition();
Import void destroy_possible();
Import void destroy_rparse_list();

/* Allocation routines */
Import Transition *new_transition();
Import State *new_state();
Import Iop *new_iop();
Import Request *new_request();
Import Possible_state *new_possible_state();
Import Rparse *new_rparse();

#if defined(TRIE_TESTING) || defined(TRACK_MALLOC)
Import int req_count;
Import int iop_count;
Import int state_count;
Import int trans_count;
Import int rparse_count;
Import int possible_state_count;
#endif /* TRIE_TESTING || TRACK_MALLOC */

#ifdef TRIE_TESTING
#ifdef MALLOC_TEST
#define malloc Malloc
#define free Free
Import char *Malloc();
Import void Free();
#endif /* MALLOC_TEST */
#include <stdio.h>
Import int transition_count();
Import Request *exactly_in_trie();
Import void parse_file();
Import void print_requests(), print_reverse();
Import void print_iop(), print_state(), print_transition();
Import void print_trie(), trie_memory();
#endif /* TRIE_TESTING */

#endif /* _TRIE_H_INCLUDE */
