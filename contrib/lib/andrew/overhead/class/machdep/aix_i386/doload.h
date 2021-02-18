/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
	doload.h - environment for dynamic loader

	Author:  John H Howard - April 9, 1987
 */



/* here is the state during a load */

typedef enum doload_mode_enum {
	Load,                   /* .. loading .do file */
	List,                   /* .. listing .do or .o file */
	Fix                     /* .. converting .o to .do file */
} doload_mode;

#define DOLOAD_MAXSCN 8         /* max number of sections */

struct doload_environment {
    doload_mode mode;           /* operating mode */
    int fd;                     /* input file descriptor */
    jmp_buf errorJump;          /* error exit */
    int problems;               /* count of problems encountered */
    FILHDR filehdr;             /* COFF file header */
    AOUTHDR aouthdr;            /* auxiliary header */
    SCNHDR scn[DOLOAD_MAXSCN];  /* section headers */
    char *segptr[DOLOAD_MAXSCN];/* segment address */
    RELOC *rtab;                /* relocation table */
    SYMENT *symtab;             /* symbol table */
    char *stringtab;            /* string table */
    SYMENT *newsym;             /* replacement symbol table */
    int newsymcount;            /* number of new symbols */
};


extern int doload_trace;

#define doload_extension ".do"

extern void *(*doload() )();
