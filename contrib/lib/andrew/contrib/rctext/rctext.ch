/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctext.ch,v 1.2 1991/09/12 20:16:39 bobg Exp $ */
/* $ACIS:rctext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrctext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctext.ch,v 1.2 1991/09/12 20:16:39 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* rctext.ch: Text subclass specialized for dealing with C code. */

#include <mark.ih>

class rctext: text {
  overrides:
    SetAttributes(struct attributes *atts);
    Clear();
    Read(FILE *file, long id) returns long;
    Write(FILE *file, long writeID, int level) returns long;
  methods:
    Indent(struct mark *range) returns long;
    ReindentLine(long pos);
    RecommentBackward(long pos);
    ReverseBalance(long pos) returns long;
    RedoStyles();
    TabAndOptimizeWS(long pos, int inc) returns long;
  data:
    struct style *comment_style, *fnname_style, *root_style;
};
