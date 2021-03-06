static char *doload_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/class/machdep/hp_pa_risc/RCS/doload.c,v 1.3 1991/09/12 17:04:23 bobg Exp $";

/*
 *      Copyright BellSouth 1991 - All Rights Reserved
 *      For full copyright information see:'andrew/config/COPYRITE.bls'.
 */

/* 
 *	doload.c - dynamic loader for class system
 *          for HP 9000/800's running HPUX 7.0
 *      Written by David Anderson
 *	Thu 7 March 1991
 *
 *      Loosely based on Zalman's RS6000 version.
 */

#include <andrewos.h>
#include <stdio.h>
#include <sys/param.h> /* For MAXPATHLEN */
#include <stddef.h> /* For NULL and the like. */
#include <errno.h>
#include <dl.h>

#define TRUE 1
#define FALSE 0

/* This should be removed, but the machine independent code declares
 * doload_Extension external expecting it to be defined in the machdep
 * code. I suppose one might want to set this to something other than ".do"
 * but for now we will do it the way it has always been done.
 */
char doload_extension[] = ".do";

int doload_trace=0;		/* nonzero if debugging */
extern int errno;

/* doload: Load a dynamic object.
 *
 * Basically this just uses HP's dynamic loader and relies on it
 * to do the right thing.
 */

char *doload(inFD, name, bp, lenP, path) /* return pointer to entry point, */
                                         /* or NULL if error */
/* UNUSED */ int inFD;			 /* open fd for package */
char *name;  		/* name of package being loaded */
char **bp;              /* base address of package */
long *lenP;		/* size of text segment */
char *path;		/* Pathname of package being loaded */
{
    shl_t *status;
    int status2;
    long i;
    void (*entry)();
    char *dummy;
    char entryname[256];
    char libpath[MAXPATHLEN];

/* In theory, one might be able to provide the correct values for bp and len,
 * but doindex is going to free the memory associated with a dynamic object
 * before loading another dynamic object to prevent memory bloat.
 *
 * Therefore this routine fakes something for doindex to free. Of course
 * the correct way to do this is to add a routine to the doload interface to
 * unload a dynamic object. (See the commented out doload_unload below.)
 * If doindex and all the doload files for other machines are ever fixed,
 * this code can be eliminated.
 *
 * Note that the loss of correct bp and len info is not to big a deal since
 * it is mostly just used for debugging.
 */
    dummy = (char *) malloc(1);
    if (dummy == NULL)
        return NULL;
    *bp = dummy;
    *lenP = 1;

    status = shl_load(path, BIND_DEFERRED, 0L);
    if (status == 0) {
	fprintf(stderr, "\ndoload: error loading %s from %s\n", name, path);
	fprintf(stderr, "          errno %d\n", errno);
        return NULL;
    }

    for (i=0; name[i] != '\0' && name[i] != '.'; i++) {
	entryname[i] = name[i];
    }
    entryname[i] = '\0';
    strcat(entryname, "__GetClassInfo");
    status2 = shl_findsym(&status, entryname, TYPE_PROCEDURE, &entry);
    if (status2 == -1) {
        fprintf(stderr, "doload: %s is undefined\n", entryname);
        return NULL;
    }

    if (doload_trace > 0) {
	fprintf(stderr, "doload: loaded %s from %s, entry: %lx\n",
		name, path, entry);
    }

    return entry;
}
