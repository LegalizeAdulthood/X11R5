/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/describe.ch,v 2.7 1991/09/12 19:21:37 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/describe.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsiddescriber_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/describe.ch,v 2.7 1991/09/12 19:21:37 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <view.ih>

class describer[describe] {
methods:
    Describe(struct view * viewToDescribe, char * format, FILE * file, long rock) returns enum view_DescriberErrs;
};
