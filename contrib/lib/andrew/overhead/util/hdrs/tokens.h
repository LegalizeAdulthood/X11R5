/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/hdrs/RCS/tokens.h,v 2.8 1991/09/12 20:35:12 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/hdrs/RCS/tokens.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/hdrs/RCS/tokens.h,v 2.8 1991/09/12 20:35:12 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
		tokens.h -- Include file for fiddling with tokens.
*/

#include <andyenv.h>

/* Length of packed tokens */
#ifdef AFS_ENV
#define	TOKENS_LEN	((12 * sizeof(long int) + 32))
#else /* AFS_ENV */
#define TOKENS_LEN 4
#endif /* AFS_ENV */

#define TokIsPrimary	3
#define TokNotPrimary	2
#define TokLocalPrimary	5
#define TokLocalNotPrimary	4
