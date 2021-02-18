#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_path.c 1.2 90/09/16";
#endif
#endif


/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/dir.h>

extern int stat();
extern char *index(), *getenv(), *calloc();

char *
in_path(filename, nameof_pathvar)
    char *filename, *nameof_pathvar;
{
    struct stat	component_stat;
    static char	component[MAXPATHLEN+1];
    char	*path, *tmp;
    char 	*end = NULL;
    int		len, lastchar; 
    int		allocd = 0;

    if (!(path = getenv(nameof_pathvar))) {
	return (NULL);
    }
	
    lastchar = (len = strlen(path)) - 1;
    if (len == 0) {
	return (NULL);
    }

    if (path[lastchar] != ':') {
	tmp = calloc(1, len+2);
	allocd = 1;
	strcpy(tmp, path);
	path = tmp;
	path[len] = ':';
    }

    while (end = index(path, ':')) {
	if (!(len = (int) (end - path))) {
	    path++;
	    continue;
	}

	bcopy(path, component, len);
	component[len] = '\0';

	if (component[len-1] != '/') {
	    strcat(component, "/");
	}
	strcat(component, filename);
	if (!stat(component, &component_stat)) {
	    if (allocd) {
		free (tmp);
	    }
	    return (component);
	} else {
	    path = end + 1;
	}

	bzero(component, sizeof(component));
    }
    if (allocd) {
	free (tmp);
    }
    return ((char *) NULL);
}
