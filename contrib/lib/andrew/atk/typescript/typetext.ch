/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.ch,v 2.7 1991/09/12 20:04:48 bobg Exp $ */
/* $ACIS:typetext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtypetext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.ch,v 2.7 1991/09/12 20:04:48 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* typetext.H
 * Class header for a basic fenced view.
 *
 */

class typetext: text {
overrides:
    HandleBegindata(long pos,FILE *file) returns long;
    HandleKeyWord(long pos, char *keyword, FILE *file) returns long;
    HandleCloseBrace(long pos, FILE *file) returns long;
    ViewName() returns char *;
    GetModified() returns long;
data:
int hashandler;
};

