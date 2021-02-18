/***********************************************************
		Copyright IBM Corporation 1991

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.ch,v 1.4 1991/09/12 20:16:12 bobg Exp $ */
/* $ACIS:mtext.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmtext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtext.ch,v 1.4 1991/09/12 20:16:12 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * mtext.ch: Text subclass specialized for dealing with Modula-2 code.
 * Modifications to ctext donated by Rob Ryan 12/88
 */

#include <mark.ih>
#include <envrment.ih>

class mtext: text {
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
    InsertNewStyle(long pos,long len,struct style *style, boolean begflag, boolean endflag);
    GetEnvironment(long pos) returns struct environment *;
    GetStyle(long pos) returns struct style *;
    BackwardCheckWord(long pos,long first);
    StyleLine(long pos);
/*  classprocedures:
    InitializeObject(); */
  data:
    struct style *comment_style, *keyword_style, *root_style;
};
