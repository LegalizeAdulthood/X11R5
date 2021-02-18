/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffutil.h,v 2.7 1991/09/18 23:22:26 gk5g Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffutil.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/roffutil.h,v 2.7 1991/09/18 23:22:26 gk5g Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef AIX
/* utilities for rofftext */
extern int Add2Buf();
extern BUF NewBuf();
extern int CreateEnvirons();
extern int DestroyEnvirons();
extern int PushEnviron();
extern int PopEnviron();
extern struct diversionLevel *CreateDiversion();
extern int DestroyDiversion();
extern int PushDiversion();
extern int PopDiversion();
#endif /* AIX */
