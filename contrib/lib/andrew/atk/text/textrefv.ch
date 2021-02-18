/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textrefv.ch,v 1.2 1991/09/12 19:52:57 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textrefv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_textrefv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textrefv.ch,v 1.2 1991/09/12 19:52:57 bobg Exp $ ";
#endif

class textrefv: fnotev {
overrides:
    Hit (enum view_MouseAction action, long x, long y, long numberOfClicks) returns struct view *;
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct textrefv * self) returns boolean;
data :
    char *cref;
    long loc;
};

