/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffstyl.h,v 2.9 1991/09/18 23:22:26 gk5g Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffstyl.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffstyl.h,v 2.9 1991/09/18 23:22:26 gk5g Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#ifndef AIX
/* style code for be2roff */

extern int InitText();
extern int WriteText();
extern int TextPut();
extern int ChangeStyle();
extern int CloseStyle();
extern int CloseAllStyles();
#endif /* AIX */
