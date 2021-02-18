/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptext.ch,v 1.2 1991/09/12 20:16:30 bobg Exp $ */
/* $ACIS:ptext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidptext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ptext/RCS/ptext.ch,v 1.2 1991/09/12 20:16:30 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * ptext.ch:  Text subclass specialized for dealing with Pascal Code
 */

#include <mark.ih>
#include <envrment.ih>

#define idstyle_PLAIN   0
#define idstyle_UPPER   1
#define idstyle_LOWER   2
#define idstyle_CAPIT   3

class ptext: text {
  overrides:
    SetAttributes(struct attributes *atts);
    Clear();
    Read(FILE *file, long id) returns long;
  methods:
    Indent(struct mark *range) returns long;
    ReindentLine(long pos);
    ReverseBalance(long pos) returns long;
    RedoStyles();
    TabAndOptimizeWS(long pos, int inc) returns long;
    /* Added by Rob Ryan 10/88 */
    InsertNewStyle(long pos,long len,struct style *style,
        boolean begflag, boolean endflag);
    GetEnvironment(long pos) returns struct environment *;
    GetStyle(long pos) returns struct style *;
    BackwardCheckWord(long pos,long first);
    StyleLine(long pos);
  data:
    struct style *comment_style, *keyword_style, *root_style;
};
