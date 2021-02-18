/* Read and manage symbol tables from object modules
 *
 * MIPS ECOFF support contributed by George Hartzell
 *   <hartzell@boulder.colorado.edu>
 *
 * HP 9000/3xx support contributed by Paul Breslaw <paul@mecazh.uu.ch>
 *
 * MACH-O support contributed by Chris Maeda (cmaeda@cs.cmu.edu)
 */

#include "scheme.h"

#if defined(CAN_LOAD_OBJ) || defined (INIT_OBJECTS)

#ifdef COFF
#  include <filehdr.h>
#  include <syms.h>
#  undef TYPE         /* ldfnc.h defines a TYPE macro. */
#  include <ldfcn.h>
#  undef TYPE
#  define TYPE(x) ((int)((x) >> TYPESHIFT))
#else
#ifdef ECOFF
#  include <filehdr.h>
#  include <aouthdr.h>
#  include <sys/file.h>
#  include <syms.h>
#else
#ifdef MACH_O
#  include <nlist.h>
#  include <rld.h>
#else
#  include <a.out.h>
#  include <sys/types.h>
#endif
#endif
#endif

#ifdef MACH_O

static SYMTAB *Grovel_Over_Nlist (symcmd, nl, strtab, text_sect)
    struct symtab_command *symcmd; /* ptr to MACH-O symtab command */
    struct nlist nl[];             /* ptr to symbol table */
    char *strtab;                  /* ptr to string table */
    long text_sect;                /* # of text section */ {

    SYMTAB *tab;
    register SYM *sp, **nextp;
    long i;

    tab = (SYMTAB *) Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    tab->strings = 0;
    nextp = &tab->first;

    /* Grovel over the file's nlist, extracting global symbols that
     * have a section mumber equal to the number of the text section:
     */
    for (i = 0; i < symcmd->nsyms; i++) {
	if ((nl[i].n_type & (N_TYPE|N_EXT)) == (N_SECT|N_EXT) &&
		nl[i].n_sect == text_sect) {
	    sp = (SYM *)Safe_Malloc (sizeof (SYM));
	    sp->name = strtab + nl[i].n_un.n_strx;
	    sp->value = nl[i].n_value;
	    sp->next = 0;
	    *nextp = sp;
	    nextp = &sp->next;
	}
    }
    return tab;
}

SYMTAB *Snarf_Symbols (mhdr) struct mach_header *mhdr; {
    struct load_command *ld_cmd;
    struct symtab_command *sym_cmd;
    struct segment_command *seg_cmd;
    struct section *sp;
    struct nlist *symtab = 0;
    char *cmdptr, *strtab;
    long i, j, text_sect = 0;

    /* Loop through the load commands, find the symbol table and
     * the segment command carrying the text section to determine
     * the number of the text section:
     */
    cmdptr = (char *)mhdr + sizeof (struct mach_header);
    for (i = 0; i < mhdr->ncmds; i++) {
	ld_cmd = (struct load_command *)cmdptr;
	if (ld_cmd->cmd == LC_SYMTAB && !symtab) {
	    sym_cmd = (struct symtab_command *)ld_cmd;
	    symtab = (struct nlist *)((char *)mhdr + sym_cmd->symoff);
	    strtab = (char *)mhdr + sym_cmd->stroff;
	} else if (ld_cmd->cmd == LC_SEGMENT && !text_sect) {
	    seg_cmd = (struct segment_command *)ld_cmd;
	    sp = (struct section *)
		((char *)ld_cmd + sizeof (struct segment_command));
	    for (j = 1; j <= seg_cmd->nsects && !text_sect; j++, sp++)
		if (strcmp (sp->sectname, SECT_TEXT) == 0)
		    text_sect = j;
	}
	cmdptr += ld_cmd->cmdsize;
    }
    if (!symtab)
	Primitive_Error ("couldn't find symbol table in object file");
    if (!text_sect)
	Primitive_Error ("couldn't find text section in object file");
    return Grovel_Over_Nlist (sym_cmd, symtab, strtab, text_sect);
}

SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    extern char *_mh_execute_header;
    return Snarf_Symbols ((struct mach_header *)&_mh_execute_header);
}

#else
#ifdef COFF

SYMTAB *Snarf_Symbols (lf, ep) LDFILE *lf; {
    SYMTAB *tab;
    register SYM *sp, **nextp;
    SYMENT sym;
    long inx;
    char *p;
    extern char *ldgetname();

    if (ldtbseek (lf) == FAILURE) {
	ldclose (lf, NULL);
	Primitive_Error ("can't ldtbseek");
    }
    tab = (SYMTAB *)Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    tab->strings = 0;
    nextp = &tab->first;
    while (1) {
	inx = ldtbindex (lf);
	if (ldtbread (lf, inx, &sym) == FAILURE)
	    break;
	if (sym.n_scnum == N_UNDEF || sym.n_scnum == N_DEBUG
		|| sym.n_scnum > HEADER(lf).f_nscns)
	    continue;
	if ((p = ldgetname (lf, &sym)) == NULL)
	    continue;
	sp = (SYM *)Safe_Malloc (sizeof (SYM));
	sp->name = Safe_Malloc (strlen (p) + 1);
	strcpy (sp->name, p);
	sp->value = sym.n_value;
	*nextp = sp;
	nextp = &sp->next;
	*nextp = 0;
    }
    return tab;
}

SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    LDFILE *f;
    SYMTAB *tab;

    if ((f = ldopen (name, NULL)) == FAILURE)
	Primitive_Error ("can't ldopen a.out file");
    tab = Snarf_Symbols (f);
    ldclose (f);
    return tab;
}

#else
#ifdef ECOFF

#define READ(fd, buf, size, errmsg)\
    if (read (fd, buf, size) != size)\
        Primitive_Error (errmsg);

#define SEEK(fd, pos, errmsg)\
    if (lseek (fd, pos, L_SET) != pos)\
        Primitive_Error (errmsg);

SYMTAB *Snarf_Symbols (fp) FILE *fp; {
    int fd;			/* a file descriptor */
    long fdi;			/* a counter for the file desc table */
    pFDR file_desc;		/* pointer to the filedesc table */
    struct filehdr file_hdr;	/* pointer to the file header */
    char *strbase;
    char *strings;
    HDRR sym_hdr;		/* pointer to symbolic header */
    long symi;			/* a counter for the local symbol table */
    pSYMR symbol;		/* pointer to symbol table */

    SYMTAB *tab;
    char *p;
    SYM *sp, **nextp;

    fd = fileno(fp);

    /* rewind the object file, since who knows what it's status is... */
    SEEK(fd, 0, "unable to rewind object file");

    /* read in the file header */
    READ(fd, &file_hdr, sizeof(file_hdr), "unable to read file header");

    /* seek to the start of the symbolic header */
    SEEK(fd, file_hdr.f_symptr, "unable to seek to symbolic header");

    /* read in the symbolic header */
    READ(fd, &sym_hdr, sizeof(sym_hdr), "unable to read symbolic header");

    tab = (SYMTAB *)Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    tab->strings = 0;
    nextp = &tab->first;

    SEEK(fd, sym_hdr.cbSymOffset, "unable to seek to symbol table");
    symbol = (pSYMR)Safe_Malloc (sym_hdr.isymMax * sizeof (SYMR));
    READ(fd, symbol, (sym_hdr.isymMax * sizeof(SYMR)),
       "unable to read symbol table");

    SEEK(fd, sym_hdr.cbSsOffset, "unable to seek to string table");
    strings = Safe_Malloc (sym_hdr.issMax);
    READ(fd, strings, sym_hdr.issMax, "unable to read string table");

    SEEK(fd, sym_hdr.cbFdOffset, "unable to seek to file descriptor table");
    file_desc = (pFDR)Safe_Malloc (sym_hdr.ifdMax * sizeof(FDR));
    READ(fd, file_desc, (sym_hdr.ifdMax * sizeof(FDR)),
	"unable to read file descriptor table");

    /* for each file in the file descriptor table do:
    */
    for (fdi = 0; fdi < sym_hdr.ifdMax; fdi++) {
	strbase = strings + file_desc[fdi].issBase;
	for (symi = file_desc[fdi].isymBase;
		symi < file_desc[fdi].csym + file_desc[fdi].isymBase;
		symi++) {
	    if (symbol[symi].st == stProc && symbol[symi].sc == scText) {
		p = symbol[symi].iss + strbase;
	
		/* allocate another node in the symbol table list */
		sp = (SYM *)Safe_Malloc (sizeof (SYM));
		sp->name = Safe_Malloc (strlen (p) + 1);
	
		/* set the new nodes values */
		strcpy (sp->name, p);
		sp->value = symbol[symi].value;
	
		/* link the node into the linked list */
		*nextp = sp;
		nextp = &sp->next;
		*nextp = 0;
	    }
	}
    }
    return tab;
}


SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    FILE *fp;
    SYMTAB *tab;
  
    if ((fp = fopen (name, "r")) == NULL)
	Primitive_Error ("can't open a.out file");
    tab = Snarf_Symbols (fp);
    fclose (fp);
    return tab;
}

#else
#ifdef hp9000s300

/* On the HP9000 an nlist entry contains a fixed length
 * part consisting of the symbol information, plus a variable
 * length part, the name without a '\0' terminator. 
 * We don't know how much space to allocate for the names 
 * until we have read them all.
 * The solution here is to save all the names on the fly
 * in a table that is grown in units of STRING_BLOCK bytes,
 * using realloc to expand it on demand.
 */
 
#define STRING_BLOCK 8192

char *Safe_Realloc (ptr, size) char *ptr; int size; {
    char *ret;

    ret = ptr ? (char *)realloc( ptr, size ) : (char *)malloc( size );
    if (ret == 0)
	Primitive_Error ("not enough memory to allocate ~s bytes",
	    Make_Fixnum (size));
    return ret;
}

SYMTAB *Snarf_Symbols (f, ep) FILE *f; struct exec *ep; {
    SYMTAB       *tab;
    register SYM *sp;
    register SYM **nextp;
    int          strsiz = 0; /* running total length of names read, */
			     /*   each '\0' terminated */ 
    int          nread = 0;  /* running total of bytes read from symbol table */
    int          max = 0;    /* current maximum size of name table */
    char         *names = 0; /* the name table */
    struct nlist_ nl;

    tab = (SYMTAB *)Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    tab->strings = 0;
    nextp = &tab->first;

    (void)fseek (f, (long)LESYM_OFFSET(*ep), 0);

    while (nread < ep->a_lesyms) {
	if (fread ((char *)&nl, sizeof (nl), 1, f) != 1) {
	    Free_Symbols (tab);
	    fclose (f);
	    Primitive_Error ("corrupt symbol table in object file");
	}

        nread += sizeof( nl );

	if (nl.n_length == 0) {
            continue;
        }
        else if (nl.n_length + strsiz + 1 > max) {
            max += STRING_BLOCK;
            names = Safe_Realloc( names, max );
        }

        if (fread( names + strsiz, 1, nl.n_length, f ) != nl.n_length) {
	    Free_Symbols (tab);
	    fclose (f);
	    Primitive_Error ("corrupt symbol table in object file");
	}
        else {
            nread += nl.n_length; 
            names[ strsiz + nl.n_length ] = '\0';
        }
	if ((nl.n_type & N_TYPE) != N_TEXT) {
	    strsiz += nl.n_length +1;
	    continue;
	}
	sp = (SYM *)Safe_Malloc (sizeof (SYM));
	sp->name = (char *)strsiz;
        strsiz += (nl.n_length + 1);
	sp->value = nl.n_value;
	*nextp = sp;
	nextp = &sp->next;
	*nextp = 0;
    }

    tab->strings = names;
 
    for (sp = tab->first; sp; sp = sp->next)
	sp->name += (unsigned)names;

    return tab;
}

SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    struct exec hdr;
    FILE *f;
    SYMTAB *tab;

    if ((f = fopen (name, "r")) == NULL)
	Primitive_Error ("can't open a.out file");
    if (fread ((char *)&hdr, sizeof hdr, 1, f) != 1) {
	fclose (f);
	Primitive_Error ("can't read a.out header");
    }
    tab = Snarf_Symbols (f, &hdr);
    fclose (f);
    return tab;
}

#else
#ifdef hp9000s800

SYMTAB *Snarf_Symbols (f, hp) FILE *f; struct header *hp; {
    SYMTAB *tab;
    register SYM *sp, **nextp;
    register n;
    struct symbol_dictionary_record r;

    tab = (SYMTAB *)Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    nextp = &tab->first;
    tab->strings = Safe_Malloc (hp->symbol_strings_size);
    (void)fseek (f, (long)hp->symbol_strings_location, SEEK_SET);
    if (fread (tab->strings, hp->symbol_strings_size, 1, f) != 1) {
	fclose (f);
	Free_Symbols (tab);
	Primitive_Error ("corrupt string table in object file");
    }
    (void)fseek (f, (long)hp->symbol_location, SEEK_SET);
    for (n = hp->symbol_total; n > 0; n--) {
	if (fread ((char *)&r, sizeof r, 1, f) != 1) {
	    fclose (f);
	    Free_Symbols (tab);
	    Primitive_Error ("corrut symbol table in object file");
	}
	if (r.symbol_type != ST_ENTRY)
	    continue;
	sp = (SYM *)Safe_Malloc (sizeof (SYM));
	sp->name = tab->strings + r.name.n_strx;
	sp->value = r.symbol_value;
	*nextp = sp;
	nextp = &sp->next;
	*nextp = 0;
    }
    return tab;
}

SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    struct header hdr;
    FILE *f;
    SYMTAB *tab;

    if ((f = fopen (name, "r")) == NULL)
	Primitive_Error ("can't open a.out file");
    if (fread ((char *)&hdr, sizeof hdr, 1, f) != 1) {
	fclose (f);
	Primitive_Error ("can't read a.out header");
    }
    tab = Snarf_Symbols (f, &hdr);
    fclose (f);
    return tab;
}

#else

SYMTAB *Snarf_Symbols (f, ep) FILE *f; struct exec *ep; {
    SYMTAB *tab;
    register SYM *sp, **nextp;
    int nsyms, strsiz;
    struct nlist nl;

    tab = (SYMTAB *)Safe_Malloc (sizeof (SYMTAB));
    tab->first = 0;
    tab->strings = 0;
    nextp = &tab->first;
    (void)fseek (f, (long)N_SYMOFF(*ep), 0);
    for (nsyms = ep->a_syms / sizeof (nl); nsyms > 0; nsyms--) {
	if (fread ((char *)&nl, sizeof (nl), 1, f) != 1) {
	    Free_Symbols (tab);
	    fclose (f);
	    Primitive_Error ("corrupt symbol table in object file");
	}
	if (nl.n_un.n_strx == 0 || nl.n_type & N_STAB)
	    continue;
#ifndef ibm023
	if ((nl.n_type & N_TYPE) != N_TEXT)
	    continue;
#endif
	sp = (SYM *)Safe_Malloc (sizeof (SYM));
	sp->name = (char *)nl.n_un.n_strx;
	sp->value = nl.n_value;
	*nextp = sp;
	nextp = &sp->next;
	*nextp = 0;
    }
    if (fread ((char *)&strsiz, sizeof (strsiz), 1, f) != 1) {
strerr:
	Free_Symbols (tab);
	fclose (f);
	Primitive_Error ("corrupt string table in object file");
    }
    if (strsiz <= 4)
	goto strerr;
    tab->strings = Safe_Malloc (strsiz);
    strsiz -= 4;
    if (fread (tab->strings+4, 1, strsiz, f) != strsiz)
	goto strerr;
    for (sp = tab->first; sp; sp = sp->next)
	sp->name = tab->strings + (long)sp->name;
    return tab;
}

SYMTAB *Open_File_And_Snarf_Symbols (name) char *name; {
    struct exec hdr;
    FILE *f;
    SYMTAB *tab;

    if ((f = fopen (name, "r")) == NULL)
	Primitive_Error ("can't open a.out file");
    if (fread ((char *)&hdr, sizeof hdr, 1, f) != 1) {
	fclose (f);
	Primitive_Error ("can't read a.out header");
    }
    tab = Snarf_Symbols (f, &hdr);
    fclose (f);
    return tab;
}

#endif
#endif
#endif
#endif
#endif

Free_Symbols (tab) SYMTAB *tab; {
    register SYM *sp;

    for (sp = tab->first; sp; sp = sp->next) {
#if defined(COFF) || defined(ECOFF)
	free (sp->name);
#endif
	free ((char *)sp);
    }
    if (tab->strings)
	free (tab->strings);
    free (tab);
}

Call_Initializers (tab, addr) SYMTAB *tab; char *addr; {
    register SYM *sp;
    register got_one = 0;

    for (sp = tab->first; sp; sp = sp->next) {
#if defined(COFF) || defined(ECOFF) || defined(hp9000s800)
	if ((char *)sp->value >= addr
		&& ((bcmp (sp->name, "init_", 5) == 0 && sp->name[5] != '_')
		|| bcmp (sp->name, "__sti__", 7) == 0
		|| bcmp (sp->name, "_STI", 4) == 0)) {
#else
	if (sp->name[0] == '_' && (char *)sp->value >= addr
		&& ((bcmp (sp->name, "_init_", 6) == 0 && sp->name[6] != '_')
		|| bcmp (sp->name, "___sti__", 8) == 0
		|| bcmp (sp->name, "__STI", 5) == 0)) {
#endif
	    got_one = 1;
	    ((int (*)())sp->value)();
	}
    }
    return got_one;
}

#endif
