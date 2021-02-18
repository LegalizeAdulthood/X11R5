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
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3text.ch,v 1.2 1991/09/12 20:15:10 bobg Exp $
*/

/*
 * m3text.ch: Text subclass specialized for dealing with Modula-3 code.
 * Modifications to ctext donated by Rob Ryan 12/88
 */

#include <mark.ih>
#include <envrment.ih>

class m3text: text {
  overrides:
    SetAttributes(struct attributes *atts);
    Clear();
    Read(FILE *file, long id) returns long;
    Write(FILE *file, long writeID, int level) returns long;/*RSKadd*/
  methods:
    IsAlnum(char ch) returns boolean;
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
    CurrentIndent(long pos) returns int; /*RSKadd*/
  classprocedures:
    IsAlnum(char ch) returns boolean; /*RSKadd*/
  data:
    struct style *comment_style, *keyword_style, *ident_style, *compress_style, *function_style, *root_style; /*RSKmod*/
};
