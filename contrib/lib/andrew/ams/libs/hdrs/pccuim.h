/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/pccuim.h,v 2.7 1991/09/12 19:16:14 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/pccuim.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/pccuim.h,v 2.7 1991/09/12 19:16:14 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*   Machine dependent include file for CUI.C  -- PC Version    */
#include <fcntl.h>

/* For instructional purposes only */
#define EOF_STRING "enter Ctrl-Z"

/* Number of snapshots to request from ms in one gulp */
#define CUIMACH_GULPSIZE 10

/* Define this so we can disclude things that aren't really necessary for pc */
#if defined(IBMPC) && !defined(DEBUG)
#define SMALL_MEMORY_MODEL
#endif

/* Declare this so cuilib on pc does the write thing */

extern char *ap_Shorten();
