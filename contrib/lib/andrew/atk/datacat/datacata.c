/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/datacat/RCS/datacata.c,v 2.14 1991/09/12 16:12:07 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/datacat/RCS/datacata.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/datacat/RCS/datacata.c,v 2.14 1991/09/12 16:12:07 bobg Exp $";
#endif /* lint */
#include <sys/param.h> /* For MAXPATHLEN. */

/*
*	datacat - A program for concatening ez documents
*
*
*
*
*/
#include <class.h>
#include <andrewos.h>
#include <datacata.eh>
#include <text.ih>
#include <dataobj.ih>
#include <environ.ih>
#include <keymap.ih>
#include <filetype.ih>
/* output options */


extern int errno;

char *progname;
#define checkandprint(A) if(A) {datacatapp_PrintVersionNumber(self);A = FALSE;};
boolean verbose;

void datacatapp__ReadInitFile(self)
struct datacatapp *self;
{
}
doinsert(text,size,followlinks)
struct text * text;
long size;
boolean followlinks;
{
    long i,j,len;
    char name[1026],cname[1026],*c,*cn;
    FILE *f;
    boolean foundopen ;
    int charflag ;
    while((i = text_Index(text,size,'@',(len = text_GetLength(text)) - size)) >= 0){
	if((i == size || (text_GetChar(text,i - 1) == '\n')) &&  text_Strncmp(text,i,"@include",8) == 0 && i + 10 < len){
	    foundopen = FALSE;
	    charflag = 0;
	    cn = cname;
	    j = i + 8;
	    if(len > j + 1024) len = j + 1024;
	    for(c = name; j < len; j++){
		*c = text_GetChar(text,j);
		*cn++ = *c;
		if(*c == ')' || *c == '\n') break;
		if(*c == '(' ){
		    foundopen = TRUE;
		    continue;
		}
		if(*c == ' ' || *c == '\t'){
		    if(charflag == 1 ) charflag = 2;
		    continue;
		}
		if(charflag > 1){
		    /* found space in name */
		    charflag = 3;
		    break;
		}
		else charflag = 1;
		c++;
	    }
	    if(charflag > 0 && charflag < 3 && foundopen && *c == ')'){
		*c = '\0';
		filetype_CanonicalizeFilename(cname, name, 1024);
		if(verbose) {
		    fprintf(stderr,"Including  %s\n",cname);
		    fflush(stderr);
		}
		if((f = fopen(cname,"r") ) == NULL){
		    fprintf(stderr,"Can't open include file %s\n",cname);
		}
		else {
		    text_DeleteCharacters(text,i,j + 1 - i);
		    len = text_InsertFile(text,f,name, i);
		    fclose(f);
		    if(followlinks) size = i;
		    else size = i + len;
		    continue;
		}
	    }
	    else {
		*cn-- = '\0';
		if(*cn == '\n'){
		    *cn = '\0';
		    fprintf(stderr,"badly formed include line '@include%s'\n",cname);
		}
		else 		    
		    fprintf(stderr,"badly formed include line '@include%s...'\n",cname);
	    }

	}
	size = i + 1;
    }
}
boolean datacatapp__ParseArgs (self,argc, argv)
struct datacatapp *self;
    int argc;
    char **argv;
/* Since we are non-interactive, everything is done in this function so we
can handle the args as they come */
    {register int i;
    char *DocumentName ;
    long clen;
    FILE *ofile,*f,*fopen();
    struct text *tx;
    boolean pv = TRUE;
    boolean ipros = FALSE;
    long oldlength;
    boolean followlinks = FALSE;
#ifdef LOCKANDDELETE
    int LockAndDelete = 0;
#endif /* LOCKANDDELETE */
    /* initialize as if ez */
    ((struct application * )self)->name = "ez";
    verbose = FALSE;
    super_ReadInitFile(self);
    ((struct application * )self)->name = "datacat";
    DocumentName = NULL;
    ofile = stdout; f = NULL;
    progname = argv[0];
    tx = text_New();
    for (i=1;i<argc;i++){
	if (argv[i][0] == '-'){
	    switch(argv[i][1]){
		case 'i':
		    ipros = TRUE ; /*process includes */
		    break;
		case 'I':
		    ipros = TRUE ; /*process includes */
		    followlinks = TRUE;
		    break;
		case 'q': 
		    pv = FALSE;
		    break;
		case 'v':
		    pv = TRUE;
		    verbose = TRUE;
		    break;
		case 'O':
		case 'o':
		    if(argv[i][2] != '\0')
			DocumentName = &argv[i][2];
		    else if(++i  < argc){
			DocumentName = argv[i];
		    }
		    else {
			checkandprint(pv);
			usage();
		    }
		    if((ofile = (fopen(DocumentName,"w"))) == NULL){
			checkandprint(pv);
			fprintf(stderr,"Can't open %s for output\n",DocumentName);
			exit(1);
		    }
		    break;

#ifdef LOCKANDDELETE
		case 'z':
		    LockAndDelete = 1;
		    break;
#endif /* LOCKANDDELETE */

		default:
		    checkandprint(pv);
		    if(text_GetLength(tx) == 0){
			fprintf(stderr,"bad flag %s\n", argv[i]);
			usage();
		    }
		    else 
			fprintf(stderr,"bad flag %s - ignoring\n", argv[i]);
		    break;
	    }
	}
	else {
#ifndef LOCKANDDELETE
	    if((f = fopen(argv[i],"r") ) == NULL){
		checkandprint(pv);
		fprintf(stderr,"Can't open %s\n",argv[i]);
		continue;
	    }
#else /* LOCKANDDELETE */
	    if((f = fopen(argv[i],(LockAndDelete ? osi_F_READLOCK : "r")) ) == NULL){
		checkandprint(pv);
		fprintf(stderr,"Can't open %s\n",argv[i]);
		continue;
	    }
	    if (LockAndDelete) {
		if (osi_ExclusiveLockNoBlock(fileno(f))){
		    checkandprint(pv);
		    fprintf(stderr, "Cannot lock %s (%d)\n", argv[i], errno);
		    fclose(f);
		    continue;
		}
	    }
#endif /* LOCKANDDELETE */
	    checkandprint(pv);
	    if(verbose){
		fprintf(stderr,"concatenating  %s\n",argv[i]);
		fflush(stderr);
	    }
	    oldlength = text_GetLength(tx);
	    text_InsertCharacters(tx,oldlength,"\n",1);
	    text_InsertFile(tx,f,argv[i], oldlength);
	    fclose(f);
	    clen = text_GetLength(tx);
	    if(text_GetChar(tx,clen - 2) == '\n')
		text_DeleteCharacters(tx,clen - 2,1);
	    if(ipros) doinsert(tx,oldlength,followlinks);
#ifdef LOCKANDDELETE
	    if (LockAndDelete && unlink(argv[i])) {
		fprintf(stderr, "Cannot delete file %s\n", argv[i]);
	    }
#endif /* LOCKANDDELETE */
	}
    }
    text_Write(tx,ofile,1,0);
    return TRUE;
    }
datacatapp__Run(self)
struct datacatapp *self;
{   /* we are already done */
    exit(0);
}
void datacatapp__FinalizeObject(classID,self)
struct classheader *classID;
struct datacatapp *self;
{
}
boolean datacatapp__InitializeObject(classID,self)
struct classheader *classID;
struct datacatapp *self;
{
    datacatapp_SetMajorVersion(self,1);
    datacatapp_SetMinorVersion(self,0);
    datacatapp_SetPrintVersionFlag(self,FALSE);
    return TRUE;

}
usage(){
	fprintf(stderr,"usage: %s  <-quiet> <-verbose> < -o OutputFileName> file[s] \n",progname);
	fflush(stderr);
	exit(2);
	}
