/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ltext/RCS/ltext.ch,v 2.5 1989/02/17 16:34:47 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ltext/RCS/ltext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_ltext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/ltext/RCS/ltext.ch,v 2.5 1989/02/17 16:34:47 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* ltext.ch: Text subclass specialized for dealing with Lisp code. */

class ltext: text {
  classprocedures:
    InitializeClass() returns boolean;
  overrides:
    SetAttributes(struct attributes *atts);
    Clear();
    Read(FILE *file, long id) returns long;
  methods:
    Indent(struct mark *range) returns long;
    ReindentLine(struct mark *place);
    ReverseBalance(long pos, int type) returns long;
    RedoStyles();
    TabAndOptimizeWS(long pos, int inc) returns long;
  data:
    struct style *comment_style, *fnname_style, *root_style;
};
