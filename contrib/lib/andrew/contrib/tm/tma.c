/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tma.c,v 2.7 1991/09/12 16:56:26 bobg Exp $ */
/* $ACIS:tma.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tma.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tma.c,v 2.7 1991/09/12 16:56:26 bobg Exp $";
#endif /* lint */

#include <andrewos.h>
#include <class.h>

#include <tma.eh>

#include <app.ih>
#include <im.ih>
#include <frame.ih>
#include <tm19.ih>
#include <tmv.ih>
#include <environ.ih>

boolean tmapp__InitializeObject(classID,self)
struct classheader *classID;
struct tmapp *self;
{
    self->args=NULL;
    self->title=NULL;
    self->fileMenus=FALSE;
    self->menufile=NULL;
    self->rows=self->cols=0;
    return  TRUE;
}

boolean tmapp__ParseArgs(self,argc,argv)
struct tmapp *self;
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
	fprintf(stderr,"%s: %s switch requires an argument.\n",tmapp_GetName(self),*argv);\
        return FALSE;\
    }else\
    	var= *++argv;\
}

    while(*++argv!=NULL && **argv=='-')
	switch((*argv)[1]){
		char *temp;
	    case 't':
		GETARGSTR(self->title);
		break;
	    case 'F':
		self->fileMenus=TRUE;
		break;
	    case 'm':
		GETARGSTR(self->menufile);
		break;
	    case 'r':
		GETARGSTR(temp);
		self->rows=atoi(temp);
		break;
	    case 'c':
		GETARGSTR(temp);
		self->cols=atoi(temp);
		break;
	    default:
		fprintf(stderr,"%s: unrecognized switch: %s\n", tmapp_GetName(self), *argv);
		return FALSE;
	}

    if(*argv!=NULL)
	self->args=argv;

    return TRUE;
}

boolean tmapp__Start(self)
struct tmapp *self;
{
    struct tm19 *tm;
    struct tmview *tmv;
    struct im *im;
    struct frame *frame;

    if(!super_Start(self))
	return FALSE;

    if(self->args==NULL){
	static char *args[2];
	args[0]=environ_Get("SHELL");
	if(args[0]==NULL)
	    args[0]="/bin/csh";
	args[1]=NULL;
	self->args=args;
    }

    {
	char *rindex(),*p=rindex(self->args[0],'/');
	if(p==NULL)
	    im_SetProgramName(self->args[0]);
	else
	    im_SetProgramName(p+1);
    }

    tm=tm19_New();
    if(tm==NULL)
	return FALSE;

    if(self->rows>0 || self->cols>0){
	if(self->rows==0)
	    self->rows=tm19_GetHeight(tm);
	if(self->cols==0)
	    self->cols=tm19_GetWidth(tm);
	tm19_SetScreenSize(tm,self->cols,self->rows);
    }

    tm19_StartProcess(tm,self->args);

    tmv=tmview_New();
    if(tmv==NULL)
	goto destroytm;

    tmview_SetFileMenus(tmv,self->fileMenus);
    tmview_SetTitle(tmv,self->title);

    if(self->menufile!=NULL)
	tmview_ReadShMenus(tmv,self->menufile);
    else{
	char *menupref=environ_GetProfile("shmenu");
	char *home=environ_Get("HOME");
	char buf[500];

	if(menupref!=NULL)
	   tmview_ReadShMenus(tmv,menupref);
	else if(home==NULL ||
		!(sprintf(buf,"%s/.shmenu",home),
		  tmview_ReadShMenus(tmv,buf))){
	    
	    char *fileName;
	    
	    fileName = environ_AndrewDir("/lib/shmenu");
	    tmview_ReadShMenus(tmv,fileName);
	}
    }

    tmview_SetDataObject(tmv,tm);

    frame=frame_New();
    if(frame==NULL) {
	fprintf(stderr,"tm: Could not allocate enough memory; exiting.\n");
	goto destroytmv;
    }

    frame_SetView(frame,tmview_GetApplicationLayer(tmv));
    frame_SetCommandEnable(frame,TRUE);

    im=im_Create(NULL);
    if(im==NULL) {
	fprintf(stderr,"tm: Could not create new window; exiting.\n");
	goto destroyframe;
    }

    im_SetView(im,frame);

    tmview_WantInputFocus(tmv,tmv);

    return TRUE;

destroyframe:
    frame_Destroy(frame);
destroytmv:
    tmview_Destroy(tmv);
destroytm:
    tm19_Destroy(tm);

    return FALSE;
}
