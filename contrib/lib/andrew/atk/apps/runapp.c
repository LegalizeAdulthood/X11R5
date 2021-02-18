/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apps/RCS/runapp.c,v 2.16 1991/09/12 15:56:10 bobg Exp $ */
/* $ACIS:runapp.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apps/RCS/runapp.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apps/RCS/runapp.c,v 2.16 1991/09/12 15:56:10 bobg Exp $";
#endif /* lint */


/* driver for generic applications
 */

#include <andrewos.h>
#include <stdio.h>
#include <class.h>

#include <im.ih>
#include <app.ih>


extern int errno;

#ifndef RUNAPP
#define RUNAPP "runapp"
#endif /* RUNAPP */

#define APPSUFFIX "app"
#define DEFAULTAPP "eza"

static void usage()
{
    fprintf(stderr,"usage:\t%s [-npdD] {-l classname} appclass args...\n",RUNAPP);
    exit(-1);
}

static void initClass()
{
    char *classPath;
    
    classPath = (char *) AndrewDir("/dlib/atk");
    class_Init(classPath);
}

static char *leaf(path)
char *path;
{
    char *p=rindex(path,'/');
    if(p==NULL)
	return path;
    else
	return p+1;
}

/* strip off the suffix, if any */
static void stripSuffix(buf,suffix)
char *buf,*suffix;
{
    char *end=buf+strlen(buf)-strlen(suffix);
    if(strcmp(suffix,end)==0)
	*end='\0';
}

main(argc,argv)
int argc;
char **argv;
{
    struct application *app;
    char appclass[200];
    boolean NoSub=FALSE,addedSuffix;
    boolean staticLoad=TRUE;
    int exitCode = -1;

#if sys_sun3_41 || sys_sun4_41
    int fd;

    /*
     * XXX - force "/dev/zero" to be open as a file descriptor one
     * greater than the first available one, as a workaround for a
     * 4.1 bug (also present in 4.1.1) in the run-time loader.
     * (Fixed in System V Release 4, allegedly.)
     */
    fd = open("/dev/zero", O_RDWR);
    dup(fd);		/* one greater */
    close(fd);
#endif

    initClass();

    argv[0] = leaf(argv[0]); /* Canonicalize the name of the application so we don't have to worry about it anywhere else... */

    if(strcmp(argv[0],RUNAPP)==0){
	boolean prependDir=TRUE;
	extern int doload_trace;

	if(*++argv==NULL)
	    usage();

	while(**argv=='-'){
	    switch(*++*argv){
		case 'n':
		    NoSub=TRUE;
		    break;
		case 'p':
		    prependDir=FALSE;
		    break;
		case 'd':
                    /* the hp800 doload supports different
                     * levels of debugging
                     */
		    doload_trace++;
		    break;
		case 'l':
		    if(*++*argv=='\0')
			argv++;
		    if(*argv==NULL || **argv=='\0')
			fprintf(stderr,
				"%s: The -l switch requires a classname as an argument.\n",
				RUNAPP);
		    else if(class_Load(*argv)==NULL)
			fprintf(stderr,
				"%s: Couldn't load the class %s.\n",
				RUNAPP,
				*argv);
		    *argv+=strlen(*argv)-1;
		    break;
		case 'D':
		    staticLoad=FALSE;
		    break;
		default:
		    usage();
	    }

	    if(*++*argv!='\0')
		fprintf(stderr,
			"%s: switches cannot be concatenated in a single argument.\n",
			RUNAPP);

	    argv++;
	    argc--;
	}

	/* search the directory we got the app from for other do's? */
	if(prependDir){
	    char *dirEnd=rindex(*argv,'/');
	    if(dirEnd!=NULL){
		int tempChar = dirEnd[1];

		dirEnd[1]='\0'; /* temporarily. Use [1] to handle application in root correctly. */
		class_PrependClassPath(*argv);
		dirEnd[1]=tempChar; /* restore it */
	    }
	}

	strcpy(appclass,leaf(*argv));
	argc--;

	addedSuffix=FALSE;

	stripSuffix(*argv,APPSUFFIX);
    }else{
	strcpy(appclass,leaf(*argv));
	strcat(appclass,APPSUFFIX);
	addedSuffix=TRUE;
    }

    if(staticLoad)
	doStaticLoads();

    if(!class_IsTypeByName(appclass,"application"))
	if(class_Error!=class_ErrNone){
	    if(class_Error==class_ErrLoad && errno!=0)
		perror(RUNAPP);
	    fprintf(stderr,"%s: Error loading the application %s.\n",RUNAPP,appclass);
	    exit(-1);
	}else if(NoSub){
	    fprintf(stderr,"%s: There is no known application called %s.\n",RUNAPP,appclass);
	    exit(-1);
	}else{
	    if(addedSuffix)
		stripSuffix(appclass,APPSUFFIX);

	    if(!class_IsTypeByName(appclass,"dataobject")){
		if(class_Error==class_ErrLoad && errno!=0)
		    perror(RUNAPP);
		if(class_Error==class_ErrNone)
		    fprintf(stderr,"%s: There is no known application or datatype called %s.\n",RUNAPP,appclass);
		else
		    fprintf(stderr,"%s: Error loading the %s object.\n",RUNAPP,appclass);
		exit(-1);
	    }else
		strcpy(appclass,DEFAULTAPP);
	}

    app=(struct application *)class_NewObject(appclass);
    if(app==NULL){
	fprintf(stderr,"%s: Error creating the %s object.\n",RUNAPP,appclass);
	exit(-1);
    }
    if(application_GetName(app)==NULL){
	application_SetName(app,leaf(*argv));	/* just make sure */
	im_SetProgramName(leaf(*argv));
    }
    else
	im_SetProgramName(application_GetName(app));
 
   if(application_GetPrintVersionFlag(app) == TRUE){
       application_PrintVersionNumber(app);
    }
    /* From now on, it's assumed that applications will print their own error messages */

    if(application_ParseArgs(app,argc,argv)){
	application_ReadInitFile(app);
	if(application_Start(app)){
	    exitCode=application_Run(app);
	    application_Stop(app);
	}
    }

    exit(exitCode);
}
