/* Copyright 1989 GROUPE BULL -- See licence conditions in file COPYRIGHT
 * Copyright 1989 Massachusetts Institute of Technology
 */
/***********************\
* 		        *
*  parsing definitions  *
* 		        *
\***********************/

#ifndef INCLUDE_YACC_H
#define INCLUDE_YACC_H

typedef struct _WOOL_STRING_STREAM {
    char           *buffer;
    char           *ptr;
    char           *last;
    int             (*overflow_handler) ();
}              *WOOL_STRING_STREAM;

#define AHEAD_BUF_SIZE 16

/* max token length */

#define MAX_TOKEN_LENGTH 1024

/* exported objects */

EXT char       *wool_pool_buffer 
INIT(NULL);
EXT int wool_pool_buffer_size 
INIT(252);
EXT int wool_pool_parentheses_level 
INIT(0);
EXT char       *wool_is_reading_file 
INIT(NULL);

/* exported functions */

EXT char       *strip_string();
EXT WOOL_OBJECT make_quoted_expression();
EXT int         wool_input_redirect();
EXT int         wool_output_redirect();
EXT void         wool_puts();
EXT void         wool_putchar();
EXT void         yyoutflush();
EXT void         yyinflush();

#ifdef DO_INIT
extern long     atol();

#else
#define END_OF_FILE 257
extern int      yylineno;

#endif
#endif /* INCLUDE_YACC_H */
