/* Create a.out from running interpreter
 *
 * ECOFF version of dump contributed by George Hartzell
 *   <hartzell@boulder.colorado.edu>
 */

#include "scheme.h"

#ifdef CAN_DUMP

#include <sys/types.h>
#include <sys/stat.h>

#ifdef COFF
#  include <filehdr.h>
#  include <aouthdr.h>
#  include <scnhdr.h>
#  include <syms.h>
#  ifndef N_BADMAG
#    define N_BADMAG(x) (0)
#  endif
#else
#ifdef ECOFF
#  include <sys/file.h>
#  include <filehdr.h>
#  include <aouthdr.h>
#  include <scnhdr.h>
#  include <sym.h>
#else
#  include <a.out.h>
#endif
#endif

#ifdef hp9000s300
static int getpagesize () {
    return EXEC_PAGESIZE;
}
#endif

Object Dump_Control_Point;

Init_Dump () {
    Global_GC_Link (Dump_Control_Point);
}

#ifndef ECOFF

Object P_Dump (ofile) Object ofile; {
#ifdef COFF
    static struct scnhdr thdr, dhdr, bhdr, scn;
    static struct filehdr hdr;
    static struct aouthdr ohdr;
    unsigned bias;
    unsigned lnno_start, syms_start;
    unsigned text_scn_start, data_scn_start;
    unsigned data_end;
    int pagemask = PAGESIZE-1;
#else
    struct exec hdr, shdr;
    unsigned data_start, data_end;
    int pagemask = getpagesize () - 1;
#endif
    char *afn;
    register n;
    char buf[BUFSIZ];
    Object ret, port;
    int ofd, afd;
    GC_Node;

    if (!EQ (Curr_Input_Port, Standard_Input_Port) ||
	    !EQ (Curr_Output_Port, Standard_Output_Port))
	Primitive_Error ("cannot dump with current ports redirected");
    Flush_Output (Curr_Output_Port);
    Close_All_Files ();

    GC_Link (ofile);
    n = stksize ();
    Dump_Control_Point = Make_Control_Point (n);
    SETFAST(ret,saveenv (CONTROL(Dump_Control_Point)->stack));
    if (TYPE(ret) != T_Special) {
	Enable_Interrupts;
	return ret;
    }
    GC_Unlink;

    Disable_Interrupts;
    port = General_Open_File (ofile, 0, Null);
    ofd = dup (fileno (PORT(port)->file));
    P_Close_Port (port);
    if (ofd < 0)
	Primitive_Error ("out of file descriptors");

    if ((afd = open (A_Out_Name, 0)) == -1) {
	Saved_Errno = errno;
	close (ofd);
	Primitive_Error ("cannot open a.out file: ~E");
    }
    if (read (afd, (char *)&hdr, sizeof (hdr)) != sizeof (hdr)
	    || N_BADMAG(hdr)) {
badaout:
	close (ofd);
	close (afd);
	Primitive_Error ("corrupt a.out file");
    }
#ifdef COFF
    data_end = ((unsigned)sbrk (0) + pagemask) & ~pagemask;
    syms_start = sizeof (hdr);
    if (hdr.f_opthdr > 0) {
	if (read (afd, (char *)&ohdr, sizeof (ohdr)) != sizeof (ohdr))
	    goto badaout;
    }
    for (n = 0; n < hdr.f_nscns; n++) {
	if (read (afd, (char *)&scn, sizeof (scn)) != sizeof (scn))
	    goto badaout;
	if (scn.s_scnptr > 0 && syms_start < scn.s_scnptr + scn.s_size)
	    syms_start = scn.s_scnptr + scn.s_size;
	if (strcmp (scn.s_name, ".text") == 0)
	    thdr = scn;
	else if (strcmp (scn.s_name, ".data") == 0)
	    dhdr = scn;
	else if (strcmp (scn.s_name, ".bss") == 0)
	    bhdr = scn;
    }
    hdr.f_flags |= (F_RELFLG|F_EXEC);
    ohdr.dsize = data_end - ohdr.data_start;
    ohdr.bsize = 0;
    thdr.s_size = ohdr.tsize;
    thdr.s_scnptr = sizeof (hdr) + sizeof (ohdr)
	+ hdr.f_nscns * sizeof (thdr);
    lnno_start = thdr.s_lnnoptr;
    text_scn_start = thdr.s_scnptr;
    dhdr.s_paddr = dhdr.s_vaddr = ohdr.data_start;
    dhdr.s_size = ohdr.dsize;
    dhdr.s_scnptr = thdr.s_scnptr + thdr.s_size;
    data_scn_start = dhdr.s_scnptr;
    bhdr.s_paddr = bhdr.s_vaddr = ohdr.data_start + ohdr.dsize;
    bhdr.s_size = ohdr.bsize;
    bhdr.s_scnptr = 0;

    bias = dhdr.s_scnptr + dhdr.s_size - syms_start;
    if (hdr.f_symptr > 0)
	hdr.f_symptr += bias;
    if (thdr.s_lnnoptr > 0)
	thdr.s_lnnoptr += bias;

    if (write (ofd, (char *)&hdr, sizeof (hdr)) != sizeof (hdr)) {
badwrite:
	Saved_Errno = errno;
	close (ofd);
	close (afd);
	Primitive_Error ("error writing dump file: ~E");
    }
    if (write (ofd, (char *)&ohdr, sizeof (ohdr)) != sizeof (ohdr))
	goto badwrite;
    if (write (ofd, (char *)&thdr, sizeof (thdr)) != sizeof (thdr))
	goto badwrite;
    if (write (ofd, (char *)&dhdr, sizeof (dhdr)) != sizeof (dhdr))
	goto badwrite;
    if (write (ofd, (char *)&bhdr, sizeof (bhdr)) != sizeof (bhdr))
	goto badwrite;
    lseek (ofd, (long)text_scn_start, 0);
    if (write (ofd, (char *)ohdr.text_start, ohdr.tsize) != ohdr.tsize)
	goto badwrite;
    Was_Dumped = 1;
    lseek (ofd, (long)data_scn_start, 0);
    if (write (ofd, (char *)ohdr.data_start, ohdr.dsize) != ohdr.dsize)
	goto badwrite;
    lseek (afd, lnno_start ? (long)lnno_start : (long)syms_start, 0);
#else
    close (afd);
    data_start = hdr.a_text;
#ifdef sun
    data_start += pagemask+1;
#endif
    data_start = (data_start + SEGMENT_SIZE-1) & ~(SEGMENT_SIZE-1);
    data_end = (unsigned)sbrk (0);
    data_end = (data_end + pagemask) & ~pagemask;
    hdr.a_data = data_end - data_start;
    hdr.a_bss = 0;
    hdr.a_trsize = hdr.a_drsize = 0;

    afn = Loader_Input;
    if (afn[0] == 0)
	afn = A_Out_Name;
    if ((afd = open (afn, 0)) == -1) {
	Saved_Errno = errno;
	close (ofd);
	Primitive_Error ("cannot open symbol table file: ~E");
    }
    if (read (afd, (char *)&shdr, sizeof (shdr)) != sizeof (shdr)
	|| N_BADMAG(shdr)) {
	close (ofd);
	close (afd);
	Primitive_Error ("corrupt symbol table file");
    }
#ifdef hp9000s300
    hdr.a_lesyms = shdr.a_lesyms;
#else
    hdr.a_syms = shdr.a_syms;
#endif

    if (write (ofd, (char *)&hdr, sizeof (hdr)) != sizeof(hdr)) {
badwrite:
	Saved_Errno = errno;
	close (ofd);
	close (afd);
	Primitive_Error ("error writing dump file: ~E");
    }
    (void)lseek (ofd, (long)FILE_TEXT_START, 0);
    n = hdr.a_text - TEXT_LENGTH_ADJ;
    if (write (ofd, (char *)MEM_TEXT_START, n) != n)
	goto badwrite;
    Was_Dumped = 1;

#ifdef hp9000s300
    (void)lseek (ofd, (long)DATA_OFFSET(hdr), 0);
#endif
    if (Heap_Start > Free_Start) {
	n = (unsigned)Free_Start - data_start;
	if (write (ofd, (char *)data_start, n) != n)
	    goto badwrite;
	(void)lseek (ofd, (long)(Free_End - Free_Start), 1);
	n = Hp - Heap_Start;
	if (write (ofd, Heap_Start, n) != n)
	    goto badwrite;
	(void)lseek (ofd, (long)(Heap_End - Hp), 1);
	n = data_end - (unsigned)Heap_End;
	if (write (ofd, Heap_End, n) != n)
	    goto badwrite;
    } else {
	n = (unsigned)Hp - data_start;
	if (write (ofd, (char *)data_start, n) != n)
	    goto badwrite;
	(void)lseek (ofd, (long)(Free_End - Hp), 1);
	n = data_end - (unsigned)Free_End;
	if (write (ofd, Free_End, n) != n)
	    goto badwrite;
    }
#ifdef hp9000s300
    (void)lseek (afd, (long)LESYM_OFFSET(shdr), 0);
    (void)lseek (ofd, (long)LESYM_OFFSET(hdr), 0);
#else
    (void)lseek (afd, (long)N_SYMOFF(shdr), 0);
#endif

#endif
    while ((n = read (afd, buf, BUFSIZ)) > 0) {
	if (write (ofd, buf, n) != n)
	    goto badwrite;
    }
    if (n < 0) {
	Saved_Errno = errno;
	close (ofd);
	close (afd);
	Primitive_Error ("error reading symbol table: ~E");
    }
    close (afd);
    Set_File_Executable (ofd, port);
    close (ofd);
    Enable_Interrupts;
    return False;
}

#else

/* The ECOFF version of P_Dump: */

#define BADAOUT(err_msg) {\
    close (newfd); close (aout_fd);\
    Primitive_Error (err_msg);\
}

#define BADWRITE(err_msg) {\
    Saved_Errno = errno;\
    close (newfd); close (aout_fd);\
    Primitive_Error (x);\
}

#define CHECK_SCNHDR(ptr, name, flags) {\
    if (strcmp (hdr.section[i].s_name, name) == 0) {\
	if (hdr.section[i].s_flags != flags)\
	    Primitive_Error ("bad flags in section header");\
        ptr = hdr.section + i;\
	i++;\
    } else {\
	ptr = 0;\
    }\
}

#define WRITE(fd, buf, size, errmsg)\
    if (write (fd, buf, size) != size)\
	Primitive_Error (errmsg);

#define SEEK(fd, pos, errmsg)\
    if (lseek (fd, pos, L_SET) != pos)\
	Primitive_Error (errmsg);

#define BUFSIZE 8192

static struct scnhdr *text_section;
static struct scnhdr *init_section;
static struct scnhdr *rdata_section;
static struct scnhdr *data_section;
static struct scnhdr *lit8_section;
static struct scnhdr *lit4_section;
static struct scnhdr *sdata_section;
static struct scnhdr *sbss_section;
static struct scnhdr *bss_section;

struct headers {
    struct filehdr fhdr;
    struct aouthdr aout;
    struct scnhdr section[10];
};

Object P_Dump (ofile) Object ofile; {
    Object port;		/* the port for the new exectable */
    Object ret;			/* the object from saveenv */
    char buf[BUFSIZE];
    int i;			/* a simple counter */
    int newsyms;
    int symrel;
    int nread;			/* number of bytes read */
    int newfd;
    int aout_fd;
    int scnptr;			/* the section being worked on */
    int size;			/* size of stack */
    int vaddr;
    int pagesize;
    unsigned data_end;
    static struct headers hdr;
    GC_Node;

    if (!EQ (Curr_Input_Port, Standard_Input_Port) ||
	!EQ (Curr_Output_Port, Standard_Output_Port))
    Primitive_Error ("cannot dump with current ports redirected");
    Flush_Output (Curr_Output_Port);
    Close_All_Files ();

    GC_Link (ofile);
    size = stksize ();
    Dump_Control_Point = Make_Control_Point (size);
    SETFAST(ret,saveenv (CONTROL(Dump_Control_Point)->stack));
    if (TYPE(ret) != T_Special) {
	Enable_Interrupts;
	return ret;
    }
    GC_Unlink;

    /* Open the new executable file (ofile)
    */
    Disable_Interrupts;
    port = General_Open_File (ofile, 0, Null);
    newfd = dup (fileno (PORT(port)->file));
    P_Close_Port (port);
    if (newfd < 0)
	Primitive_Error ("out of file descriptors");
  
    /* Open the executable file (the one that contains our code)
    */
    if ((aout_fd = open (A_Out_Name, 0)) == -1) {
	Saved_Errno = errno;
	close (newfd);
	Primitive_Error ("cannot open a.out file: ~E");
    }

    hdr = *((struct headers *)TEXT_START);
  
    if (hdr.fhdr.f_magic != MIPSELMAGIC && hdr.fhdr.f_magic != MIPSEBMAGIC)
	BADAOUT("input file has bad magic number");

    if (hdr.fhdr.f_opthdr != sizeof(hdr.aout))
	BADAOUT("input file's a.out header has the wrong size");
  
    if (hdr.aout.magic != ZMAGIC)
	BADAOUT("input file is not a ZMAGIC file");

    i = 0;
    CHECK_SCNHDR(text_section,  _TEXT,  STYP_TEXT);
    CHECK_SCNHDR(init_section,  _INIT,  STYP_INIT);
    CHECK_SCNHDR(rdata_section, _RDATA, STYP_RDATA);
    CHECK_SCNHDR(data_section,  _DATA,  STYP_DATA);
#ifdef _LIT8
    CHECK_SCNHDR(lit8_section,  _LIT8,  STYP_LIT8);
    CHECK_SCNHDR(lit4_section,  _LIT4,  STYP_LIT4);
#endif
    CHECK_SCNHDR(sdata_section, _SDATA, STYP_SDATA);
    CHECK_SCNHDR(sbss_section,  _SBSS,  STYP_SBSS);
    CHECK_SCNHDR(bss_section,   _BSS,   STYP_BSS);
    if (i != hdr.fhdr.f_nscns)
	Primitive_Error ("~s sections expected, ~s found",
	    Make_Fixnum (hdr.fhdr.f_nscns), Make_Fixnum (i));

    Was_Dumped = 1;

    pagesize = getpagesize();
    data_end = ((unsigned)sbrk (0) + pagesize - 1) & (-pagesize);
    hdr.aout.dsize = data_end - DATA_START;
    hdr.aout.bsize =0;
  
    {
	extern __start();
	hdr.aout.entry = (unsigned)__start;
    }

    hdr.aout.bss_start = hdr.aout.data_start + hdr.aout.dsize;

    data_section->s_vaddr = DATA_START + rdata_section->s_size;
    data_section->s_paddr = DATA_START + rdata_section->s_size;

    data_section->s_size = data_end - DATA_START;
    data_section->s_scnptr = rdata_section->s_scnptr + rdata_section->s_size;

    vaddr = data_section->s_vaddr + data_section->s_size;
    scnptr = data_section->s_scnptr + data_section->s_size;
    if (lit8_section) {
	lit8_section->s_vaddr = data_section->s_vaddr + data_section->s_size;
	lit8_section->s_paddr = data_section->s_paddr + data_section->s_size;
	lit8_section->s_scnptr = data_section->s_scnptr + data_section->s_size;
    }
    if (sdata_section && lit8_section) {
	sdata_section->s_vaddr = lit8_section->s_vaddr + lit8_section->s_size;
	sdata_section->s_paddr = lit8_section->s_paddr + lit8_section->s_size;
	sdata_section->s_scnptr = lit8_section->s_scnptr + lit8_section->s_size;
    }
    if (sbss_section) {
	sbss_section->s_vaddr = sdata_section->s_vaddr + sdata_section->s_size;
	sbss_section->s_paddr = sdata_section->s_paddr + sdata_section->s_size;
	sbss_section->s_scnptr = sdata_section->s_scnptr+sdata_section->s_size;
    }
    if (bss_section) {
	bss_section->s_vaddr = sbss_section->s_vaddr + sbss_section->s_size;
	bss_section->s_paddr = sbss_section->s_paddr + sbss_section->s_size;
	bss_section->s_scnptr = sbss_section->s_scnptr + sbss_section->s_size;
    }

    WRITE(newfd, TEXT_START, hdr.aout.tsize,
	"problem writing text section to output file");

    WRITE(newfd, DATA_START, hdr.aout.dsize,
	"problem writing data section to output file");
  
    /* Set up and copy the symbol table to the new executable
    */
    SEEK(aout_fd, hdr.fhdr.f_symptr, "can't seek to start of symbol table");

    if ((nread = read (aout_fd, buf, BUFSIZE)) < sizeof (HDRR))
	Primitive_Error ("problem reading symbol table");

#define symhdr ((pHDRR)buf)
    newsyms = hdr.aout.tsize + hdr.aout.dsize;
    symrel = newsyms - hdr.fhdr.f_symptr;
    hdr.fhdr.f_symptr = newsyms;
    symhdr->cbLineOffset += symrel;
    symhdr->cbDnOffset += symrel;
    symhdr->cbPdOffset += symrel;
    symhdr->cbSymOffset += symrel;
    symhdr->cbOptOffset += symrel;
    symhdr->cbAuxOffset += symrel;
    symhdr->cbSsOffset += symrel;
    symhdr->cbSsExtOffset += symrel;
    symhdr->cbFdOffset += symrel;
    symhdr->cbRfdOffset += symrel;
    symhdr->cbExtOffset += symrel;
#undef symhdr

    do {
	if (write (newfd, buf, nread) != nread)
	    Primitive_Error ("problem writing symbols to output file");
	if ((nread = read (aout_fd, buf, BUFSIZE)) < 0)
	    Primitive_Error ("problem reading symbols");
    } while (nread != 0);

    SEEK(newfd, 0L, "problem seeking to start of header");
    WRITE(newfd, &hdr, sizeof(hdr), "problem writing header of output file");
    close (aout_fd);
    Set_File_Executable (newfd, port);
    close (newfd);
    Enable_Interrupts;
    return False;
}
#endif

static Set_File_Executable (fd, port) Object port; {
    struct stat st;

    if (fstat (fd, &st) != -1) {
	int omask = umask (0);
	(void)umask (omask);
#ifdef FCHMOD_BROKEN
	{
	    Object f = PORT(port)->name;
	    register n = STRING(f)->size;
	    register char *s = alloca (n+1);
	    bcopy (STRING(f)->data, s, n);
	    s[n] = '\0';
	    (void)chmod (s, st.st_mode & 0777 | 0111 & ~omask);
	}
#else
	(void)fchmod (fd, st.st_mode & 0777 | 0111 & ~omask);
#endif
    }
}

#endif
