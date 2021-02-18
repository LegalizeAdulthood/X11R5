/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *bglisp_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/bglisp/RCS/bglisp.c,v 2.8 1991/09/12 17:09:52 bobg Exp $";


/*
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/bglisp/RCS/bglisp.c,v 2.8 1991/09/12 17:09:52 bobg Exp $ 
 *
 * $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/eli/bglisp/RCS/bglisp.c,v $ 
 */

#include  <bglisp.h>

EliProcessInfo_t EliProcessInfo;
char            ProgramName[100] = "bglisp";

int Terse, Report, Free;

EliSexp_t *lastBacktrace;

/* Change in command-line options:
 * -m means use malloc-based allocation;
 * -t means terse mode (no prompts or messages)
 *
 * Even in terse mode, errors get printed;
 * but they now go to stderr.
 */

main(argc, argv)
int             argc;
char          **argv;
{
    static EliState_t stStruct, *st;
    int i, j, MallocP = FALSE;
    void BGLISP_Prim_BTERR();

    Free = Report = Terse = FALSE;

    for (i = 1; i < argc; ++i) {
	if (argv[i][0] == '-') {
	    for (j = 1; j < strlen(argv[i]); ++j) {
		switch (argv[i][j]) {
		    case 'r':
			Report = TRUE;
			break;
		    case 't':
			Terse = TRUE;
			break;
		    case 'f':
			Free = TRUE;
			break;
		    case 'm':
			MallocP = TRUE;
			break;
		    default:
			fprintf(stderr, "Usage: %s [-rtfm]\n", argv[0]);
			exit(1);
			break;
		}
	    }
	}
	else {
	    fprintf(stderr, "Usage: %s [-rtfm]\n", argv[0]);
	    exit(1);
	}
    }

    st = &stStruct;
    EliInit(st, MallocP ? e_mem_malloc : e_mem_pool);
    EliClearErr(st);
    eliTraceStk_Purge(st, EliTraceStk(st));
    if (Free) {
	EliFreeAllTraceStackNodes(st);
	EliFreeAllEvalStackNodes(st);
    }
    eliPrimDefCompiled(st, "BTERR", BGLISP_Prim_BTERR);
    if (!(lastBacktrace = eliSexp_GetNew(st))) {
	fprintf(stderr, "\nCould not initialize!\n");
	exit(1);
    }
    EliClearErr(st);
    eliTraceStk_Purge(st, EliTraceStk(st));
    if (Free) {
	EliFreeAllTraceStackNodes(st);
	EliFreeAllEvalStackNodes(st);
    }
    if (!Terse)
	Welcome();
    ReadEvalPrint(st);
    if (!Terse)
	Cleanup();
}

Welcome()
{
    puts("BGLisp by Bob Glickstein and the Andrew Message System Group.");
    puts("This scaled-down Lisp interpreter is built upon ELI, the");
    puts("Embedded Lisp Interpreter.  ELI is the basis for FLAMES, the");
    puts("Filtering Language for the Andrew Message System.  BGLisp");
    puts("is thus a good test-bed for FLAMES functions.  ELI can be");
    puts("incorporated into any application requiring programmability");
    puts("in a Lisp-like language.\n\nTo exit BGLisp, type ^D\n");
    printf("This is ELI version %d.%d\n\n", EliProcessInfo.MajorVersion, EliProcessInfo.MinorVersion);
}

ReadEvalPrint(st)
EliState_t     *st;
{				/* Main interpreter loop */
    EliSexp_t      *sexp1, *sexp2;
    int             exit = FALSE, q;

    while (!exit) {
	if (Report) {
	    printf("Sexp Nodes in use:       %d/%d\n", st->numNodes, st->numTotalNodes);
	    printf("Cons Cells in use:       %d/%d\n", st->numConsCells, st->numTotalConsCells);
	    printf("Symbol Nodes in use:     %d/%d\n", st->numSymNodes, st->numTotalSymNodes);
	    printf("String Nodes in use:     %d/%d\n", st->numStrNodes, st->numTotalStrNodes);
	    printf("Function Nodes in use:   %d/%d\n", st->numFnNodes, st->numTotalFnNodes);
	    printf("Bucket Nodes in use:     %d/%d\n", st->numBucketNodes, st->numTotalBucketNodes);
	    printf("EvalStack Nodes in use:  %d/%d\n", st->numStkNodes, st->numTotalStkNodes);
	    printf("TraceStack Nodes in use: %d/%d\n", st->numErrStkNodes, st->numTotalErrStkNodes);
	}
	if (!Terse)
	    DisplayPrompt();
	sexp1 = eliGetSexp_trace(st, EliTraceStk(st));
	exit = EliProcessInfo.u_wrap;
	if (!exit) {
	    if (EliErr_ErrP(st)) {
		q = EliErr_ErrCode(st);
		fprintf(stderr, "\n*** An error occurred while parsing: Code=%d (%s)\n*** Location: %s\n", q, EliErrStr(q), EliErr_ErrLoc(st));
		if (EliErr_BadSexpP(st)) {
		    fprintf(stderr, "*** Expression: %s\n", EliSPutSexp(EliErr_BadSexp(st)));
		    fflush(stderr);
		}
	    }
	    else {
		if (!(sexp2 = eliSexp_GetNew_trace(st, EliTraceStk(st)))) {
		    q = EliErr_ErrCode(st);
		    fprintf(stderr, "\n*** An error occurred while allocating a result buffer: Code=%d (%s)\n*** Location: %s\n", q, EliErrStr(q), EliErr_ErrLoc(st));
		    if (EliErr_BadSexpP(st)) {
			fprintf(stderr, "*** Expression: %s\n", EliSPutSexp(EliErr_BadSexp(st)));
			fflush(stderr);
		    }
		}
		else {
		    eliEval(st, sexp1, sexp2);
		    if (EliErr_ErrP(st)) {
			if (EliErr_BacktraceP(st))
			    EliSexp_SetCons(st, lastBacktrace, EliErr_Backtrace(st));
			q = EliErr_ErrCode(st);
			fprintf(stderr, "\n*** An error occurred during evaluation: Code=%d (%s)\n*** Location: %s\n", q, EliErrStr(q), EliErr_ErrLoc(st));
			if (EliErr_BadSexpP(st)) {
			    fprintf(stderr, "*** Expression: %s\n", EliSPutSexp(EliErr_BadSexp(st)));
			}
			fflush(stderr);
		    }
		    else {
			EliDisplaySexp(sexp2);
			putchar('\n');	/* EliDisplaySexp() doesn't do this */
		    }
		}
	    }
	    EliClearErr(st);
	    eliTraceStk_Purge(st, EliTraceStk(st));
	    if (Free) {
		EliFreeAllTraceStackNodes(st);
		EliFreeAllEvalStackNodes(st);
	    }
	}
    }
}

DisplayPrompt()
{
    printf("BGLisp> ");
}


/* Say goodbye */

Cleanup()
{
    printf("\nThank you for using this fine software.\n    -Bob Glickstein\n\n");
}

void BGLISP_Prim_BTERR(st, arglist, resbuf)
EliState_t *st;
EliCons_t *arglist;
EliSexp_t *resbuf;
{
    if (EliSexp_GetType(lastBacktrace) == e_data_list)
	EliSexp_SetSexp(st, resbuf, lastBacktrace);
    else
	EliSexp_SetSym(st, resbuf, EliNilSym(st));
}
