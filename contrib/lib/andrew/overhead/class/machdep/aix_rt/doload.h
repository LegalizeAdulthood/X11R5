/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* 
	doload.h - environment for dynamic loader

	Author:  John H Howard - April 9, 1987
 */



/* here is the state during a load */

#ifdef hpux  /* nlist is defined in /usr/lib/nlist.h... not the same! */
#define nlist nlist_
#endif /* hpux */

typedef enum doload_mode_enum {
	Load,			/* .. loading .do file */
	List,			/* .. listing .do or .o file */
	Fix			/* .. converting .o to .do file */
} doload_mode;

struct doload_environment {
    doload_mode mode;		/* operating mode */
    int fd;			/* input file descriptor */
    jmp_buf errorJump;		/* error exit */
    int problems;		/* count of problems encountered */
    struct exec header;		/* header at beginning of a.out file */
    char *text;			/* text segment */
    char *data;			/* data segment */
#ifndef AIX
    struct relocation_info *rtab; /* relocation table */
    struct nlist *symtab;	/* symbol table */
    char *stringtab;		/* string table */
    struct nlist *newsym;	/* replacement symbol table */
#else /* #ifndef AIX */
    struct reloc *rtab;		/* relocation table */
    struct syment *symtab;	/* symbol table */
    char *stringtab;		/* string table */
    struct syment *newsym;	/* replacement symbol table */
#endif /* #ifndef AIX */
    int newsymcount;		/* number of new symbols */
#ifdef hpux
    int newsymsize;		/* size of new symbol table */
    struct nlist **stab_entries; /* list of pointers to symbol table entries */
    int alloced_stab_entries,     /* number of slots malloced */
        filled_stab_entries;      /* number of slots filled */
#endif /* hpux */
};



#if defined(sun) && defined(sparc)
#define relocation_info reloc_info_sparc
#define r_symbolnum r_index
#endif

extern int doload_trace;

extern char doload_extension[];

#ifndef AIX
extern void *(*doload() )();
#else
extern char *doload();
#endif /* AIX */


#ifdef hpux
#define maxString 256 /* max size string expected in a symbol table entry */
#define symSize(pSym) \
  (sizeof *pSym + pSym->n_length) /* struct size + string length (NO TRAILING ZERO!) */
#define nextSym(pSym) \
  (struct nlist *)((char *)pSym + symSize(pSym))
#define symString(pSym) \
  ((char *)pSym + sizeof *pSym)
#define stabStrCmp(str, pSym) \
  ((pSym->n_length == strlen(str)) ? \
   strncmp(symString(pSym), str, pSym->n_length) \
   : -1)
#define stabStrCpy(str, pSym) \
  { \
  bcopy(symString(pSym), str, pSym->n_length); \
  str[pSym->n_length] = '\0'; \
  }
#endif /* hpux */

