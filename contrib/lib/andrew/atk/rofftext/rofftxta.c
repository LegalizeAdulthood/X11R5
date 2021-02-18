/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.c,v 2.11 1991/09/12 16:30:55 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/rofftxta.c,v 2.11 1991/09/12 16:30:55 bobg Exp $ ";
#endif /* lint */

/*
 * app for rofftext
 * 
 */

#include <class.h>

#include <rofftxta.eh>
#include <text.ih>

#include <app.ih>
#include <rofftext.ih>

boolean rofftextapp__InitializeObject(classID,self)
struct classheader *classID;
struct rofftextapp *self;
{
    self->macrofile = NULL;
    self->RoffType = FALSE;
    self->outputfile = NULL;
    self->inputfile = NULL;
    self->argv = NULL;
    self->argc = 0;
    self->inputfiles = NULL;
    self->HelpMode = FALSE;
    self->BeCompletelyBogus = FALSE;
    rofftextapp_SetMajorVersion(self, 7);
    rofftextapp_SetMinorVersion(self, 0);
    rofftextapp_SetFork(self,FALSE);
    return  TRUE;
}


/*
 * usage statement
 */
static void show_usage(self)
struct rofftextapp *self;
{
    fprintf(stderr,
	"Usage: %s [-xhntbw] [-m macro file] [-o outputfile] [-] [file...]\n",
	    rofftextapp_GetName(self));
    fprintf(stderr,
"	-x: show this usage statement\n\
	-h: format for use with the ATK Help system.  Crush initial blank space\n\
	-w: print message about badly formed numbers\n\
	-n: pretend to be nroff (default)\n\
	-t: pretend to be troff\n\
	-m file: read in /usr/lib/tmac/tmac.file as a macro file\n\
	-o file: set output file to 'file'.  Default is standard output\n\
	- : use standard input as the file input\n\
	file: read in these files\n");
}
    

boolean rofftextapp__ParseArgs(self,argc,argv)
struct rofftextapp *self;
int argc;
char **argv;
{
    char temp2[128], *andrewdir, *getenv();
    boolean slash = FALSE;

    if(!super_ParseArgs(self,argc,argv))
	return FALSE;

#define GETARGSTR(var)\
{\
    if((*argv)[2]!='\0')\
        var= ((*argv)[2]=='=' ? &(*argv)[3] : &(*argv)[2]);\
    else if(argv[1]==NULL){\
	fprintf(stderr,"%s: %s switch requires an argument.\n",rofftextapp_GetName(self),*argv);\
        return FALSE;\
    }else {\
    	var= *++argv;\
        argc--;\
    }\
}

    while(*++argv!=NULL && **argv=='-') {
        boolean stop = FALSE;
        switch((*argv)[1]){
                char *temp;
	    case 'x':
		show_usage(self);
		exit(0);
            case 'n':
                self->RoffType = FALSE;
                break;
            case 't':
                self->RoffType = TRUE;
                break;
            case 'm':
		GETARGSTR(temp);
		switch(*temp) {
		    case 'm':
			if((andrewdir = getenv("ANDREWDIR")) == NULL)
			    andrewdir = "/usr/andrew";
			else if(andrewdir[strlen(andrewdir) - 1] == '/')
			    slash = TRUE;
			sprintf(temp2,"%s%slib/tmac/tmac.%s", andrewdir, 
				(slash ? "" : "/"), temp);
			break;
		    default:
			sprintf(temp2,"/usr/lib/tmac/tmac.%s", temp);
			break;
		}
		self->macrofile = StrDup(temp2);
                break;
            case 'o':
                GETARGSTR(self->outputfile);
                break;
            case 'h':
                self->HelpMode = TRUE;
                break;
            case 'b':
                self->BeCompletelyBogus = TRUE;
                break;
            case 'w':
                self->PrintWarnings = TRUE;
                break;
            case '\0':
                stop = TRUE;
                break; /* for stdin, use '-' */
            default:
                fprintf(stderr,"%s: unrecognized switch: %s\n", rofftextapp_GetName(self), *argv);
		show_usage(self);
                return FALSE;
        }
        if (stop)
            break;
        argc--;
    }

    /* are there input filenames? */

    if (*argv != NULL)
        self->argv = argv;
    self->argc = argc-1;

    return TRUE;
}

boolean rofftextapp__Start(self)
struct rofftextapp *self;
{
    return TRUE;
}

int rofftextapp__Run(self)
struct rofftextapp *self;
{
    struct rofftext *r;
    struct text *t;
    FILE *in,*out;
    char **ptr1,**ptr2;
    int size = sizeof(char *);

    if(!super_Start(self))
	return FALSE;

    r = rofftext_New();
    if(r==NULL)
	return FALSE;

    /* be bogus and copy argv into new array, */
    /* saving last string in self->inputfile for __Read. */

    self->inputfiles = (char **)malloc(self->argc * sizeof(char *));
    for(ptr1 = self->argv,ptr2 = self->inputfiles;(ptr1 != NULL) && (*ptr1 != NULL);ptr1++) {
        if (*(ptr1+1)==NULL) {
            *ptr2 = NULL;
            self->inputfile = *ptr1;
        }
        else {
            *ptr2++ = *ptr1;
            size += sizeof(char *);
        }
    }

    if (self->inputfile == NULL)
        in = stdin;
    else {
        in = fopen(self->inputfile,"r");
        r->filename = self->inputfile;
    }

    if (self->outputfile)
        out = fopen(self->outputfile,"w+");
    else
        out = stdout;

    if (self->BeCompletelyBogus) {
        t = text_New();
        fprintf(stderr,"Reading roff into text...");
        fflush(stderr);
        rofftext_ReadRoffIntoText(t,in,0,self->inputfiles);
        fprintf(stderr,"done.\n");
        fflush(stderr);
        text_Write(t,out,(long)t,0);
    }
    else {
        r->inputfiles = self->inputfiles;
        r->macrofile = self->macrofile;
        r->RoffType = self->RoffType;
        r->HelpMode = self->HelpMode;
        r->PrintWarnings = self->PrintWarnings;

        rofftext_Read(r,in,(long)r);
        rofftext_Write(r,out,(long)r,0);
    }
    fflush(stderr);
    fflush(stdout);
    return 0;
}
