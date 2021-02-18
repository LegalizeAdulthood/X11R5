/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm19.ch,v 2.7 1991/09/12 20:17:27 bobg Exp $ */
/* $ACIS:tm19.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm19.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtm19_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm19.ch,v 2.7 1991/09/12 20:17:27 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * subclass of termulator implementing h19 escape sequences
 */

class tm19: termulator[tm] {
    classprocedures:
        InitializeObject(struct tm19 *self) returns boolean;
    overrides:
        GetTermcap() returns char *;
        GetTerm() returns char *;
    data:
        int savedX,savedY;
        int foldCount;
};
