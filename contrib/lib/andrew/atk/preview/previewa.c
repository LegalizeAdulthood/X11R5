/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/previewa.c,v 2.8 1991/09/12 16:28:23 bobg Exp $ */
/* $ACIS:previewa.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/previewa.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/preview/RCS/previewa.c,v 2.8 1991/09/12 16:28:23 bobg Exp $";
#endif /* lint */

/*
*
*	Main module for BE2 preview.
*		A program for previewing dvitroff input
*
*/
#include <andrewos.h>
#include <class.h>
#include <previewa.eh>
#include <app.ih>
#include <preview.ih>
#include <im.ih>
#include <signal.h>

char *DviBaseName=NULL;
char *DviFileName=NULL;
boolean DoScaling = TRUE;
FILE *filein;
boolean DviFileComplete;
boolean debug;

boolean previewapp__InitializeObject(classID,self)
struct classheader *classID;
struct previewapp *self;
{
    previewapp_SetMajorVersion(self, 7);
    previewapp_SetMinorVersion(self, 0);
    return TRUE;
}

boolean previewapp__ParseArgs(self,argc,argv)
struct previewapp *self;
int argc;
char **argv;
{
    if(!super_ParseArgs(self,argc,argv))
	return FALSE;

#define GETARGSTR(var)\
{\
    if((*argv)[2]!='\0')\
        var= ((*argv)[2]=='=' ? &(*argv)[3] : &(*argv)[2]);\
    else if(argv[1]==NULL){\
	fprintf(stderr,"%s: %s switch requires an argument.\n",previewapp_GetName(self),*argv);\
        return FALSE;\
    }else\
    	var= *++argv;\
}

    while(*++argv!=NULL){
	if(**argv=='-')
	    switch((*argv)[1]){
		case 'f':
		    DoScaling = FALSE;
		    break;
		case 'o':
		    GETARGSTR(DviBaseName);
		    break;
		default:
		    fprintf(stderr,"%s: unrecognized switch: %s\n", previewapp_GetName(self), *argv);
		    return FALSE;
	    }
	else{
	    DviFileName= *argv;
	    if(DviBaseName==NULL)
		DviBaseName= *argv;
	    DviFileComplete = TRUE;
	    if (debug)
		fprintf(stderr, "Dvi File is %s\n", DviFileName);
	}	    
    }

    return TRUE;
}

static struct preview *pv;
FinishUp()
{
    preview_FinalizeObject(pv);
    signal(SIGTERM, SIG_DFL);
}

boolean previewapp__Start(self)
struct previewapp *self;
{
    struct view *v;
    struct im *im;
    struct sigvec termVector;
#ifdef USEFRAME
    struct frame *fr;
#endif /* USEFRAME */

    if(!super_Start(self))
	return FALSE;

    debug = FALSE;

    if(DviFileName==NULL)
	filein=stdin;
    else{
	filein=fopen(DviFileName,"r");
	if(filein==NULL){
	    fprintf(stderr, "Can't open %s\n", DviFileName);
	    return FALSE;
	}
    }

    /*     print_StaticEntry; */
    pv = preview_Create(filein, (DviFileName==NULL ? "" : DviFileName), (DviBaseName==NULL ? "" : DviBaseName), DviFileComplete, DoScaling);
    if(pv == NULL) exit(-1);
    /* pv->debug = debug; */
    v= preview_GetApplicationLayer(pv);
    if((im = im_Create(NULL)) == NULL) {
	fprintf(stderr,"preview: Could not create new window; exiting.\n");
	return(FALSE);
    }
#ifdef USEFRAME
    if((fr = frame_New()) == NULL) {
	fprintf(stderr,"preview: Could not allocate enough memory; exiting.\n");
	return(FALSE);
    }
    frame_SetView(fr, v);
#else /* USEFRAME */
    im_SetView(im,v);
#endif /* USEFRAME */
    preview_WantInputFocus(pv,pv);
    termVector.sv_handler = FinishUp;
    termVector.sv_mask = (1 << (SIGURG - 1)); /* Block redraw signals while doing cleanup. */
    termVector.sv_onstack = 0;
    sigvec(SIGTERM, &termVector, NULL);
    return TRUE;
}

previewapp__Run(self)
struct previewapp *self;
{
    if(!previewapp_Fork(self))
	return -1;
    while(TRUE){
	im_KeyboardProcessor();
	if(preview_ReadyToQuit(pv)) break;
    }
    preview_FinalizeObject(pv);
    exit(0);
}
