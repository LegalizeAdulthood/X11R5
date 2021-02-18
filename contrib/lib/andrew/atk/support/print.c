/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.c,v 2.28 1991/09/12 16:31:47 bobg Exp $ */
/* $ACIS:print.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.c,v 2.28 1991/09/12 16:31:47 bobg Exp $";
#endif /* lint */

#include <andrewos.h>
#include <class.h>
#include <ctype.h>

#include <signal.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <view.ih>
#include <environ.ih>

#include <print.eh>

static void SetPrinterType();

/* The following defaults are used by the print software */
#define print_INDEXTROFF 42424 /*   produce troff index */

static char *print_formatcommand,*print_printcommand,*print_previewcommand,*print_pscprintcommand;
static char *print_spoolpath,*print_spooldir,*print_printertype;

/* The following ifndefs allow these defines to be set from the site.h file 
  They should probably all be set from site.h, but the following allows for backward compatibility */

#ifdef CMU_ENV
#ifndef print_FORMATCOMMAND
#define print_FORMATCOMMAND "eqn -T$PRINTERTYPE /tmp/%s.n  | troff -T$PRINTERTYPE - |" 
#endif
#ifndef print_PRINTCOMMAND
#define print_PRINTCOMMAND " print -T dvi -O %s"  
#endif
#ifndef print_PSCPRINTCOMMAND
#define print_PSCPRINTCOMMAND " print -T native -O %s"  
#endif
#ifndef print_PREVIEWCOMMAND
#define print_PREVIEWCOMMAND " preview -o %s"  
#endif
#ifndef print_SPOOLPATH
#define print_SPOOLPATH "/afs/andrew.cmu.edu/common/printing/spool/" 
#endif
#ifndef print_SPOOLDIR
#define print_SPOOLDIR "plum" 
#endif
#ifndef print_PRINTERTYPE
#define print_PRINTERTYPE "psc" 
#endif
#else /* CMU_ENV */
#if 0 /*IBM032_ENV*/
#ifndef print_FORMATCOMMAND
#define print_FORMATCOMMAND "cat /tmp/%s.n | /usr/ibm/eqn | /usr/ibm/troff -T3812 -t |" /* overridden by the profile 'formatcommand' entry */
#endif
#ifndef print_PRINTCOMMAND
#define print_PRINTCOMMAND "/usr/ucb/lpr -n -Ppp; rm -f /tmp/%s.n; rm -f /tmp/%s.n.dvi "  /* if overridden by the profile 'printcommand' entry, the troff file (usually /tmp/%s.n) won't be automatically deleted . Overriding will also cause any print arguments passed to the print command to be  ignored. */
#endif
#ifndef print_PSCPRINTCOMMAND
#define print_PSCPRINTCOMMAND "/usr/ucb/lpr -Ppp; rm -f /tmp/%s.n; rm -f /tmp/%s.n.dvi "
#endif
#ifndef print_PREVIEWCOMMAND
#define print_PREVIEWCOMMAND " preview -o %s"  /* if overridden by the profile 'previewcommand' entry, the troff file (usually /tmp/%s.n) won't be automatically deleted  */
#endif
#ifndef print_SPOOLPATH
#define print_SPOOLPATH "/usr/spool/" /* overridden by the profile 'print.spoolpath' entry */
#endif
#ifndef print_SPOOLDIR
#define print_SPOOLDIR "pp" /* overridden by the environment variable 'PRINTER' or the profile 'print.spooldir' entry */
#endif
#ifndef print_PRINTERTYPE
#define print_PRINTERTYPE "ibm3812" /* overridden by the contents of the file '.PrinterType' in the printer spool directory */
#endif
#else /* IBM032_ENV */
#ifdef EXPRES
#ifndef print_FORMATCOMMAND
#define print_FORMATCOMMAND "eqn -T$PRINTERTYPE /tmp/%s.n | troff -T$PRINTERTYPE - |" 
#endif
#ifndef print_PRINTCOMMAND
#define print_PRINTCOMMAND " lpr -n"  
#endif
#ifndef print_PSCPRINTCOMMAND
#define print_PSCPRINTCOMMAND " lpr"
#endif
#ifndef print_PREVIEWCOMMAND
#define print_PREVIEWCOMMAND " preview -o %s"  
#endif
#ifndef print_SPOOLPATH
#define print_SPOOLPATH "/usr/spool/" 
#endif
#ifndef print_SPOOLDIR
#define print_SPOOLDIR "lpr" 
#endif
#ifndef print_PRINTERTYPE
#define print_PRINTERTYPE "psc" 
#endif
#else /* EXPRES */
/* following ifndefs allow these to be set from the site.h file */
#ifndef print_FORMATCOMMAND
#define print_FORMATCOMMAND "eqn -T$PRINTERTYPE /tmp/%s.n  | troff -T$PRINTERTYPE - |" 
#endif
#ifndef print_PRINTCOMMAND
#define print_PRINTCOMMAND " lpr -n"  
#endif
#ifndef print_PSCPRINTCOMMAND
#define print_PSCPRINTCOMMAND " lpr"
#endif
#ifndef print_PREVIEWCOMMAND
#define print_PREVIEWCOMMAND " preview -o %s"  
#endif
#ifndef print_SPOOLPATH
#define print_SPOOLPATH "/usr/spool/" 
#endif
#ifndef print_SPOOLDIR
#define print_SPOOLDIR "lpr" 
#endif
#ifndef print_PRINTERTYPE
#define print_PRINTERTYPE "psc" 
#endif
#endif  /* EXPRES */
#endif /* IBM032_ENV */
#endif /* CMU_ENV */
/* The following strings are use for processing the ATK generated index */

#ifndef DIVERTPLAINTROFF
#define DIVERTPLAINTROFF " 2>&1 | sort -f | indexpro"
#endif
#ifndef DIVERTPRINTTROFF
#define DIVERTPRINTTROFF " 2>&1 | sort -f | indexpro | troff -ms -T$PRINTERTYPE"
#endif

#if defined(hp9000s300) && HP_OS < 70
print_sigAlrm()
{ }
#endif /* hp9000s300 */

static insert(src,c)
char *src,*c;
{   /* inserts string src into the begining of string c , assumes enough space */
    char *p,*enddest;
    enddest = c + strlen(c);
    p = enddest + strlen(src);
    while(enddest >= c) *p-- = *enddest-- ;
    for(p = src; *p != '\0';p++)
	*c++ = *p;
}
static char *shove(dest,search,src)
register char *dest,*search,*src;
{   /* shove the string src into dest after the string search */
    int searchlen;
    searchlen = strlen(search);
    while(*dest){
	if(*dest == *search && strncmp(dest,search,searchlen) == 0){
	    insert(src,dest + searchlen);
	    return(dest);
	}
	dest++;
    }
    return NULL;
}
static void normalize(s)
char *s;
{
    register char *c;
    for(c = s + strlen(s) - 1; c >= s; c--){
	if(!isalnum(*c)){
	    insert("\\",c);
	}
    }
}
int print__ProcessView(classID, v, print, dofork,DocumentName,prarg)
struct classheader *classID;
struct view *v;
int print;
int dofork; 
char *DocumentName,*prarg;
{
    /*  Mostly Gosling Code from PrintDoc in BE 1's BasicIO.c */
    char    PrintCommandFormat[400];
    char    PrintCommand[600];
    char    tname[400],tmpname[400];
    char dviname[400];
    static int  seq = 1;
    char   *p,*pp;
    char   *q;
    char    dname[400];
    char   *dnameptr;
    FILE  *outf;
    char pt[128] ;
    struct stat buf;
/*     boolean indexonly = FALSE; */
    if(environ_Get("troffpostprocessor"))
	environ_Delete("troffpostprocessor");
    SetPrinterType(pt);
    if(print == print_PRINTPOSTSCRIPT && strcmp(pt,"psc") != 0 && strcmp(pt,"postscript") != 0){
	return (-1);
    }
    if (DocumentName == NULL || *DocumentName == '\0') {
	sprintf(dname, "%d", getpid());
    }
    else {
	p = rindex(DocumentName, '/');
	p = (p == 0) ? DocumentName
	    : p + 1;
	strcpy(dname, p);
    }
    dnameptr = &dname[strlen(dname)];
    sprintf(tmpname, "/tmp/%s.n", dname);

    while (1)  {
	while (stat(tmpname, &buf) == 0) {
	    sprintf(dnameptr, ".%d", seq++);
	    sprintf(tmpname, "/tmp/%s.n", dname);
	}
	sprintf(dviname, "/tmp/%s.dvi", dname);
	if (stat(dviname, &buf)/*  && errno == ENOENT */) break;
	sprintf(dnameptr, ".%d", seq++);
	sprintf(tmpname, "/tmp/%s.n", dname);
    }
    if((outf = fopen(tmpname,"w")) == 0) return(-1);
    if(print == print_PRINTPOSTSCRIPT)
	view_Print(v,outf,"PostScript","PostScript",1);
    else 
	view_Print(v,outf,"troff","PostScript",1);
    fclose(outf);
    strcpy(PrintCommandFormat, print_formatcommand);
    if((q = environ_Get("TroffArgs")) != NULL){
	if(q[strlen(q) - 1] != ' '){
	    strcpy(PrintCommand,q);
	    strcat(PrintCommand," ");
	    q = PrintCommand;
	}
	shove(PrintCommandFormat,"troff ",q);
#if DEBUG
puts(PrintCommandFormat);
fflush(stdout);
#endif /* DEBUG */
    }
    
    if(print == print_INDEXTROFF || (q = environ_Get("IndexOnly")) != NULL){
	/* Set up troff so it only produces the error output,
	 this containts the indexinformation which is them piped through
	 an external program that processes it and pipes it through 
	 another troff process whose output ends up back in the pipe
	 set up by this command */
/*	indexonly = TRUE; */
	if(shove(PrintCommandFormat,"troff ","-z ") == NULL){
	    fprintf(stderr,"Can't process index without troff\n");
	    fflush(stderr);
	}
	else {
	    q = rindex(PrintCommandFormat,'|');
	    if(q != NULL){
		if(print == print_INDEXTROFF){
		    strcpy(q,DIVERTPLAINTROFF);
		}
		else {
		    insert(DIVERTPRINTTROFF,q);
		}
	    }
	}
    }
    else if((q = environ_Get("troffpostprocessor")) != NULL){
	char pbuf[2048],*ppp,fbuf[1024];
	sprintf(fbuf,print_formatcommand,dname, dname, dname, dname, dname);
	for(ppp = fbuf + strlen(fbuf); ppp > fbuf; ppp--)
	      if(*ppp == '|') {*ppp = '\0'; break;}
	if(shove(fbuf,"troff ","-z ") == NULL){
	    fprintf(stderr,"Can't process cross references without troff\n");
	    fflush(stderr);
	}
	else {
	    sprintf(pbuf,"%s %s \"%s\"",q,tmpname,fbuf);
	    system(pbuf);
	}
    }
    normalize(dname); /* This uses backslashes to quote all of the 
			non-alphanumeric characters in dname so the strings
			used for printing don't get confuse the shell.
                                          Instances of tmpname below get quoted with double
                                          quotes for the same reason */
    p = &PrintCommandFormat[strlen(PrintCommandFormat)];
    if(print == print_INDEXTROFF){
/*	strcpy(p,print_printcommand); ?????????????????????????????*/
    }	
    else switch(print){	
	case print_PREVIEWTROFF:
	    /* Preview Command */
	    q = (char *) environ_GetProfile("previewcommand");
	    if (q == 0){
		q = tname;
		if(dofork)
		    sprintf(q,"%s;rm \"%s\"",print_previewcommand,tmpname);
		else
		    strcpy(q,print_previewcommand);
	    }
	    strcpy(p, q);
	    break;
	case print_PREVIEWPOSTSCRIPT:
	    /* unimplemented */
	    return -1;
	case print_PRINTTROFF:
	case print_PRINTPOSTSCRIPT:
	default:
	    /* Print Command */
	    pp = (print == print_PRINTPOSTSCRIPT)? "pscprintcommand" : "printcommand";
	    q = (char *) environ_GetProfile(pp);
	    if (q == 0){
		pp =  (print == print_PRINTPOSTSCRIPT) ? print_pscprintcommand : print_printcommand ;
		if(prarg == NULL || *prarg == '\0'){
		    q = tname;
		    if(dofork)
			sprintf(q,"%s;rm \"%s\"",pp,tmpname);
		    else
			strcpy(q,pp);
		}
		else {
		    q = tname;
		    if(dofork)
			sprintf(q,"%s %s; rm \"%s\"",pp,prarg,tmpname);
		    else
			sprintf(q,"%s \"%s\"",pp,prarg,tmpname);
		}
	    }
	    strcpy(p, q);
	    break;
    }
    sprintf(PrintCommand, PrintCommandFormat, dname, dname, dname, dname, dname);
#if DEBUG
puts(PrintCommand);
fflush(stdout);
#endif /*DEBUG */
    if (dofork) {
#if defined(hp9000s300) && HP_OS < 70
        {
	  int status;
	  struct sigvec vecAlrm;
	  struct itimerval timer;
	  
	  /** enable an interval timer so we can escape from wait(); **/
	  vecAlrm.sv_handler = print_sigAlrm;
	  vecAlrm.sv_mask = 0;
	  vecAlrm.sv_flags = 0;
	  sigvector(SIGALRM, &vecAlrm, 0);
	  timer.it_value.tv_sec = 0;
	  timer.it_value.tv_usec = 100000;
	  timer.it_interval.tv_sec = 0;
	  timer.it_interval.tv_usec = 100000;
	  setitimer(ITIMER_REAL, &timer, 0);
	  
	  while (wait(&status) > 0) ;
	  
	  /** disable the timer **/
	  timer.it_value.tv_sec = 0;
	  timer.it_value.tv_usec = 0;
	  setitimer(ITIMER_REAL, &timer, 0);
	}
#else /* hp9000s300 */
	while (wait3(0, 1, 0) > 0);
#endif /* hp9000s300 */
	if (osi_vfork() == 0) {
	    int pg,fd;
	    int numfds = getdtablesize();

	    close(0);
	    open("/dev/null", 2);
	    dup2(0, 1);
	    for (fd = 3; fd < numfds; fd++)
		close(fd);
#ifdef hpux
	    setpgrp2(0, pg = getpid());
#else /* hpux */
#ifdef SGI_4D_ENV
	    setpgrp();
#else
	    setpgrp(0, pg = getpid());
#endif
#endif /* hpux */
#if !SY_AIX221
	    setpriority(PRIO_PGRP, pg, 10);
#endif /* #ifndef AIX */
	    execlp("/bin/sh", "sh", "-c", PrintCommand, 0);
	    exit(0);
	}
    }
    else  {
	FILE *temp,*popen();
#if SY_AIX221 || SY_AIX12 || SY_AIX31
	signal(SIGCLD, SIG_DFL);
#endif
	if ((temp = popen(PrintCommand, "w")))  {
/*	    while(( c = getc(temp))!= EOF) putc(c,stdout); */
	    if (pclose(temp))  {
		fprintf(stderr, "Print request using the command:\n");
		fprintf(stderr, "\t%s\n", PrintCommand);
		fprintf(stderr, "probably did not complete due - returned the\n");
		fprintf(stderr, "following error message:\n\t");
		perror("");
		unlink(tmpname);
		return(-1);
	    }
	}
	else {
	    fprintf(stderr,"Could not execute the following print command:\n");
	    fprintf(stderr,"\t%s\n", PrintCommand);
	    unlink(tmpname);
	    return(-1);
	}
	unlink(tmpname);
    }
    return(0);
}

static void SetPrinterType (printertype) 
char *printertype;
 {
    char   *RealSpoolDir = NULL;
    static char TempSpoolDir[1000];
    struct stat buf;
    char *cp;
    char currentprinter[100];
    char *SpoolPath = print_spoolpath;
    char *str;

    cp = (char *) environ_Get("PRINTER");
    if (cp == NULL)
        cp = environ_GetProfile("print.spooldir");
    strcpy(currentprinter, (cp) ? cp : print_spooldir);
    
    str = environ_GetProfile("print.spoolpath");
    if (str)  {
	SpoolPath = (char *) strcpy(malloc(strlen(str) + 1), str);
    }
    
    if (index(currentprinter, '/') != 0) {
	if (stat(currentprinter, &buf) == 0)
	    RealSpoolDir = currentprinter;
    }
    else {
    
     /* Look at SpoolPath to find the right currentprinter */
    
	char   *p;
	char   *r;

	r = TempSpoolDir;
	p = SpoolPath;
	while (1) {
	    if (*p == '\0' || *p == ':') {
		if (r != TempSpoolDir) {
		    *r++ = '/';
		    strcpy(r, currentprinter);
		    if (stat(TempSpoolDir, &buf) == 0) {
		    
		     /* Found a spool directory */
		    
			RealSpoolDir = TempSpoolDir;
			break;
		    }
		    r = TempSpoolDir;
		}
		if (*p == '\0')
		    break;
	    }
	    else
		if (r != TempSpoolDir || *p != ' ') {
		    *r++ = *p;
		}
	    p++;
	}
    }

    if (RealSpoolDir) {
	FILE *tfile;

	strcat(RealSpoolDir, "/.PrinterType");
	if ((tfile = fopen(RealSpoolDir, "r")))  {
	    fscanf(tfile, "%s", printertype);
	    fclose (tfile);
	    environ_Put("PRINTERTYPE", printertype);
	    return;
	}
	else
	    environ_Put("PRINTERTYPE", print_printertype);
    }
    else  {
        environ_Put("PRINTERTYPE", print_printertype);
    }
    strcpy(printertype,print_printertype);
}

char *print__GetPrintCmd(ClassID,print)
struct classheader *ClassID;
int print;
{
    char *q;
    switch(print){
	case print_PREVIEWTROFF:
	    if((q = (char *) environ_GetProfile("previewcommand")) != NULL)
		return q;
	    return print_previewcommand;
	case print_PREVIEWPOSTSCRIPT:
	    /* unimplemented */
	    return NULL;
	case print_PRINTPOSTSCRIPT:
	    if((q = (char *) environ_GetProfile("pscprintcommand")) != NULL) return q;
	    return print_pscprintcommand;
	case print_PRINTTROFF:
	default:
	    if((q = (char *) environ_GetProfile("printcommand")) != NULL) return q;
	    return print_printcommand;
    }
}

boolean print__InitializeClass(ClassID)
struct classheader *ClassID;
{
    char *foo;
    if((foo =environ_GetConfiguration("printcommand")) == NULL)
	print_printcommand = print_PRINTCOMMAND;
    else{
	print_printcommand = malloc(strlen(foo) + 1);
	strcpy(print_printcommand,foo);
    }
    if((foo =environ_GetConfiguration("previewcommand")) == NULL)
	print_previewcommand = print_PREVIEWCOMMAND;
    else{
	print_previewcommand = malloc(strlen(foo) + 1);
	strcpy(print_previewcommand,foo);
    }
    if(((foo = (char *) environ_GetProfile("formatcommand")) != NULL) ||
	((foo =environ_GetConfiguration("formatcommand")) != NULL)){
	print_formatcommand = malloc(strlen(foo) + 1);
	strcpy(print_formatcommand,foo);
    }
    else print_formatcommand = print_FORMATCOMMAND;
    if((foo =environ_GetConfiguration("pscprintcommand")) == NULL)
	print_pscprintcommand = print_PSCPRINTCOMMAND;
    else{
	print_pscprintcommand = malloc(strlen(foo) + 1);
	strcpy(print_pscprintcommand,foo);
    }
    if((foo =environ_GetConfiguration("spoolpath")) == NULL)
	print_spoolpath = print_SPOOLPATH;
    else{
	print_spoolpath = malloc(strlen(foo) + 1);
	strcpy(print_spoolpath,foo);
    }
    if((foo =environ_GetConfiguration("spooldir")) == NULL)
	print_spooldir = print_SPOOLDIR;
    else{
	print_spooldir = malloc(strlen(foo) + 1);
	strcpy(print_spooldir,foo);
    }
    if((foo =environ_GetConfiguration("printertype")) == NULL)
	print_printertype = print_PRINTERTYPE;
    else{
	print_printertype = malloc(strlen(foo) + 1);
	strcpy(print_printertype,foo);
    }
    return TRUE;
}
