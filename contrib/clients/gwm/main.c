/**************************************\
* 				       *
*  main routines for wool interpreter  *
* 				       *
\**************************************/

#define WLPROFILE_USER_VARIABLE "WOOLPROFILE"
#define WLPATH_SHELL_VARIABLE   "WOOLPATH"

#include <signal.h>
#include <stdio.h>
#include "EXTERN.h"
#include "wool.h"
#include "wl_atom.h"
#include "wl_coll.h"
#include "wl_func.h"
#include "wl_list.h"
#include "wl_number.h"
#include "wl_string.h"
#include "wl_pointer.h"
#include "wl_active.h"
#include "wl_name.h"
#include "yacc.h"

char    *RCS_Header =	/* the version */
"$Id: gwm.shar,v 1.57 1991/10/04 13:37:58 colas Exp $";
char            buffer[8192];	/* input buffer */

WOOL_OBJECT wool_eval_expr;
DECLARE_strchr;

timeout_handler();		/* alarm handler */
#define MAX_DURATION 0		/* max time of an eval in seconds */
WOOL_Atom       timeout;	/* the wool atom "timeout" */
void wool_types_init();

main_init();

main(argc, argv)
int             argc;
char           *argv[];

{
    char *s;
    extern char    *wool_fix_path();
    /* option parsing */

    wool_user_profile_name = ".woolrc";
    wool_text_extension = ".wl";

    print_banner();
    /* initialize paths (put .:$HOME before built-ins) */
    wool_path = wool_fix_path(DEFAULT_WLPATH);
    if ((s = (char *) getenv(WLPROFILE_USER_VARIABLE)) && (s[0] != '\0'))
	wool_user_profile_name = s;	     
    if ((s = (char *) getenv(WLPATH_SHELL_VARIABLE)) && (s[0] != '\0'))
	wool_path = s;

    /* first, initialize wool */
    wool_init(wool_types_init);
    /* get the atoms we want */
    timeout = wool_atom("timeout");
    /* then tell it to go back here after an error */
    set_wool_error_resume_point();	/* wool_error will jump here */

    /* main routine: read/eval/print */
    do {
	/* initialize the input "pool" of wool */
	wool_pool(NULL);

	/*
	 * then we prompt the user until we think we have a complete
	 * expression (as we are told by the return code of wool_pool) 
	 */
	do {
	    int             i;

	    wool_puts("? ");
	    for (i = 0; i < 2 * wool_pool_parentheses_level; i++)
		wool_putchar(' ');
	    if (!gets(buffer)) {

		/*
		 * as we use wool_pool, we must take care of the end_of_file
		 * ourselves. 
		 */
		wool_puts("Bye.\n");
		exit(0);
	    }
	} while (wool_pool(buffer));

	/* so, now we can read/eval/print this pooled text 
	 * add newline at the end for lex problems
	 */
	strcat(wool_pool_buffer, "\n");
	wool_input_redirect(1, wool_pool_buffer, NULL, NULL);
	

	/*
	 * we read all the expressions of this line, the NULL returned by
	 * wool_read meaning the end of the input (here the pool) 
	 */
	while (wool_read()) {
	    /* we set the timeout for an eval to timeout */
	    alarm(timeout -> c_val ?
		  ((WOOL_Number) (timeout -> c_val)) -> number
		  : MAX_DURATION);
	    if (wool_eval_expr = wool_eval(wool_read_expr)) {
		wool_puts(" = ");
		wool_print(wool_eval_expr);
		wool_putchar('\n');
	    }
	}
    } while (1);
}

/*
 * Test initialisations
 */

int variable = 8;

print_var()
{
    printf("Var is %d\n", variable);
    return NULL;
}

int (*fp)();

/* what to do when a wool-error occurs?
 */

wool_error_handler()
{
}

/* wool_end is exiting */
wool_end(n)
int n;
{
#ifdef MONITOR
    moncontrol(0);			/* do not trace ending code */
#endif /* MONITOR */
    exit(n);
}


/* handler called by wool before execing a son after a fork
 */

wool_clean_before_exec()
{
}

char *
strchr_nth(s, c, n)
char *s, c;
int n;
{
    while (s && n) {
	s = strchr(s + 1, c);
	n--;
    }
    return s;
}

print_banner()
{
    char banner[128];
    int banner_len = strchr_nth(RCS_Header, ' ', 4)
	- strchr_nth(RCS_Header, ' ', 2) - 1;

    strncpy(banner, strchr_nth(RCS_Header, ' ', 2) + 1, banner_len);
    banner[banner_len] = '\0';
    wool_printf("WOOL toplevel %s\n", banner);
}

/* you must initialize here all the used wool types in your application */

void
wool_types_init()
{
#define init_wool_type(type, name) type[0]=(WOOL_METHOD)wool_atom(name)
    init_wool_type(WLActive, "active");
    init_wool_type(WLAtom, "atom");
    init_wool_type(WLCollection, "collection");
    init_wool_type(WLQuotedExpr, "quoted-expr");
    init_wool_type(WLSubr, "subr");
    init_wool_type(WLFSubr, "fsubr");
    init_wool_type(WLExpr, "expr");
    init_wool_type(WLFExpr, "fexpr");
    init_wool_type(WLList, "list");
    init_wool_type(WLName, "name");
    init_wool_type(WLNamespace, "namespace");
    init_wool_type(WLNumber, "number");
    init_wool_type(WLPointer, "pointer");
    init_wool_type(WLString, "string");
#undef init_wool_type
}
