/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/cui.h,v 2.8 1991/09/12 19:15:49 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/cui.h,v $
*/
/* 
        cui.h -- Include file for message server/CUI clients.
 */

#include <ams.h>  /* Definitions common to server & client */

/* Version number for CUI-client intrface.  If an end user interface
        wishes to, it can track major and minor changes to the
        CUI and issue warnings when it thinks it is out of date */

#include <cuivers.h>

typedef short Boolean;
#define TRUE 1
#define FALSE 0

extern int CUIDebugging;
#define debug(n, x) ((CUIDebugging & (n)) ? printf x : 0)
/* Flags for debugging tell which procedures to debug.  A current list:
        (They should be ORed together as appropriate)
        1       Entry to each procedure
        2       Procedures in cui.c, the main routines
        4       Most cui library routines
        8       Very low-level debugging, not usually wanted
        16      cuisnap routines
        32      AMSID hashing performance
        64      AMS_RETURN_ERRCODE macro

	1024	trace all open and close calls (same value as in ms.h)
 */

/* Structure of snapshot as passed around by CUI library */

struct AugmentedSnapshot {
    char *DirName;
    char Raw[AMS_SNAPSHOTSIZE];
};


#define CUI_RPC_BUGOUT 0
#define CUI_RPC_RESTART 1
#define CUI_RPC_RETRY 2

#define WRITEFILECHUNK 2000

#define MORE_NO_MORE 42     /* Unique error code returned by moreprintf
                                when you type "q" */

#define MAXBODY 5000            /* Size of chunks in which files and
                                   messages will be delivered via SNAP */

/* Numbers too big for MAXBODY cause subtle bugs on the RT */

#define LINEMAX 1000            /* Size of longest legal command line
                                   input */

#define ELI_ERROR_TEXT_BUFLEN (1024)    /* A nice computerish number */

#define CheckPrompted(msg)  if (!arg || !*arg || *arg=='?') {\
    moreprintf("%s: ",(msg));	\
    arg=GetLine();	}

#define CheckQuestion(msg)  if (arg && *arg=='?') {\
    moreprintf("%s: ",(msg)); \
    arg=GetLine();  }


/* Some macros for backward compatibility */

#define MarkDirectoryForPurging CUI_MarkDirectoryForPurging
#define UnmarkDirectoryForPurging CUI_UnmarkDirectoryForPurging
#define GetAMSID CUI_GetAMSID
#define GetCuid CUI_GetCuid
