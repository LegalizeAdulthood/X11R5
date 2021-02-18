/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
 *	Macros, manifest constants, and typedefs for PCSERVER
 *
 *	Author - Larry K. Raper
 *	Version 2.1
 *	(C) Copyright IBM Corporation 1985, 1986
 *	Program Property of IBM
 *
 *	2.1  10/86  Added PCS_GETHOMEDIR service
 */

#include <amsenv.h>
#include <stdio.h>

     /* Services */
#define PCS_OPEN		1
#define PCS_CLOSE		2
#define PCS_READ		3
#define PCS_WRITE		4
#define PCS_DIRSEARCH		5
#define PCS_REMOVEFILES 	6
#define PCS_RENAMEFILES 	7
#define PCS_MKDIR		8
#define PCS_RMDIR		9
#define PCS_CHMOD		10
#define PCS_SPACEQUERY		11
#define PCS_EXECUTE		12
#define PCS_TIMEOFDAY		13
#define PCS_FORCETERM		14
#define PCS_TIMESTAMP		15
#define PCS_GETHOMEDIR		16

     /* Return Codes */
#define PCS_SUCCESS		0
#define PCS_NOFILE		1
#define PCS_NOPATH		2
#define PCS_NOSPACE		3
#define PCS_NOACCESS		4
#define PCS_BADFILT		5
#define PCS_BADINTEN		6
#define PCS_NOHANDLE		7
#define PCS_EOF 		8
#define PCS_NOMATCH		9
#define PCS_NOTEMPTY		10
#define PCS_BADMODE		11
#define PCS_BADARG		12
#define PCS_EXECFAIL		13

     /* Processing intent */
#define PCS_INPUT		0
#define PCS_OUTPUT		1
#define PCS_UNSPECIFIED 	2

     /* File attributes */
#define PCS_NORMAL		1
#define PCS_DIRECTORY		2
#define PCS_HIDDEN		4
#define PCS_READONLY		8

#define PRIVATE static

#ifdef DEBUG
#define DBGMSG(level,prtargs)\
	if (level < PCS_debuglevel)\
	    {printf("[PCSERV] ");printf prtargs;printf("\n");fflush(stdout);}\
	else
#else /* DEBUG */
#define DBGMSG(level,prtargs) {}
#endif /* DEBUG */

#ifdef MAXNAMLEN

typedef char NAME[MAXNAMLEN+1];
typedef char PATH[MAXPATHLEN+1];

/* A PC file name */
typedef struct {
    NAME name;
    NAME ext;
    char hidden;   /* 1 = hidden */
    } PCNAME;

#endif /* MAXNAMLEN */

#define DOTS 1
#define NODOTS 0
#define LARGEST_MSG (8192+100)
#define OUTPUT_LIMIT (6*1024)

