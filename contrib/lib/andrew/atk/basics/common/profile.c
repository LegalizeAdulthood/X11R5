/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *profile_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/profile.c,v 2.15 1991/09/12 16:00:11 bobg Exp $";


#include <class.h>

#ifndef NeXT
#include <a.out.h>
#endif
#include <andrewos.h> /* sys/file.h */
#include <sys/param.h>

/*
 * AIX is SYSVish when it comes to profiling.
 */
#ifdef AIX
#ifndef SYSV
#define SYSV 1
#endif /* #ifndef SYSV */
#endif /* #ifdef AIX */

#if (!defined(AIX) && !defined(_IBMR2) && !defined(HP300_ENV) && !defined(M_UNIX) && !defined(SGI_4D_ENV))
#include <sys/gprof.h>
#endif /* (!defined(AIX) && !defined(_IBMR2) && !defined(HP300_ENV)) */

#include <profile.eh>

#define SCALE_1_TO_1 0x10000L

static char *profBuf=NULL;
#ifndef SYSV
static long profBufSize;
static char monfilename[MAXPATHLEN];
static struct phdr header;
#endif /* SYSV */

/* Allocate the profiling buffer */
static int allocProfBuf(size)
{
#ifndef SYSV
    if(profBuf!=NULL && size!=profBufSize)
	free(profBuf);

    profBufSize=size;

    profBuf=(char *)malloc(profBufSize);
    if (profBuf == NULL) 
	return FALSE;

    bzero(profBuf,profBufSize);

    return TRUE;
#else /* SYSV */
    return FALSE;
#endif /* SYSV */
}

int profile__StartClass(classId,classname,filename)
struct classheader *classId;
char *classname,*filename;
{
#ifndef SYSV
    struct classinfo *info=class_Load(classname);

    if(info==NULL || class_GetTextBase(info)==NULL)
	return FALSE;

    return profile_Start(class_GetTextBase(info),
			 class_GetTextLength(info),
			 filename);
#else /* SYSV */
    return FALSE;
#endif /* SYSV */
}

int profile__Start(classId,textbase,textlength,filename)
struct classheader *classId;
char *textbase;
long textlength;
char *filename;
{
#ifndef SYSV
    if(textbase == NULL){
	textbase = (char *)profile_STATICBASE;
	textlength = ((char *)class_GetEText()) - textbase;
    }

    profile_Stop(); /* in case one is already started */

    if(!allocProfBuf(textlength))
	return FALSE;

    /* save data for writing to file */
    header.lpc = (char *)profile_STATICBASE; /* not correct, but what gprof understands */ 
    header.hpc=header.lpc+textlength;
    header.ncnt=textlength+sizeof(header);

    if(filename==NULL)
	strcpy(monfilename,profile_DEFAULTFILE);
    else
	strcpy(monfilename,filename);

    /* the following code is to replace the monitor call below  */
    /*  monitor (textbase,textbase+textlength, profBuf, profBufSize, 0); */
    profil(profBuf,profBufSize,textbase,SCALE_1_TO_1);

    return TRUE;
#else /* SYSV */
    return FALSE;
#endif /* SYSV */
}

int profile__Active(classId)
struct classheader *classId;
{
    return (profBuf!=NULL);
}

int profile__Stop(classId)
struct classheader *classId;
{
#ifndef SYSV
    if(profBuf!=NULL){
	int fd;

	/* monitor(0); */
	profil(0,0,0,0);

	fd=open(monfilename,O_CREAT|O_TRUNC|O_WRONLY,0666);
	if(fd<0)
	    return FALSE;

	write(fd,&header,sizeof(header));
	write(fd,profBuf,profBufSize);
	close(fd);

	free(profBuf);
	profBuf=NULL;

	return TRUE;
    }else
	return FALSE;
#else /* SYSV */
    return FALSE;
#endif /* SYSV */
}
