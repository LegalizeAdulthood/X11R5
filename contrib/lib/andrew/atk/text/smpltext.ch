/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/smpltext.ch,v 2.12 1991/09/12 19:52:25 bobg Exp $ */
/* $ACIS:smpltext.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/smpltext.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidsimpletext_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/smpltext.ch,v 2.12 1991/09/12 19:52:25 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* Simple text conatins a string of text that can be manipulated.
 * The string is maintained in two section divided by a gap.
 * Position is always given referring to the next character.
 * Thus if position is set to 0 then an InsertString will insert characters
 * at the beginning of the string.
 */

#define simpletext_VERSION 1

#include <mark.ih>

class simpletext[smpltext]: dataobject[dataobj] {
classprocedures:
    InitializeObject(struct simpletext *self)
      returns boolean;
    FinalizeObject(struct simpletext *self);

overrides:
    SetAttributes(struct attributes *attributes);

    Read(FILE *file, long id)
      returns long;
    Write(FILE *file, long writeid, int level)
      returns long;

methods:
    /* Operations for Marks and Fence */

    CreateMark(long pos, long length)
      returns struct mark *;
    RemoveMark(struct mark *mark);
    SetReadOnly(boolean readOnly);
    GetReadOnly() returns boolean;
    RegionModified(long pos, long len);
    LengthChanged(long pos, long len);

    /* Operations for text */

    Clear();
    InsertCharacters(long pos, char *str, long len)
      returns boolean;  /* FALSE if read-only */
    AlwaysInsertCharacters(long position, char *string, long size);
    AddInCharacter(long pos, char c);   /* Does not modify marks */
    DeleteCharacters(long pos, long len)
      returns boolean;  /* FALSE if read-only */
    AlwaysDeleteCharacters(long position, long size);
    ReplaceCharacters(long pos, long len, char *repStr, long repLen)
      returns boolean;  /* FALSE if read-only */
    AlwaysReplaceCharacters(long pos, long len, char *repStr, long repLen);

    GetLength()
      returns long;
    GetChar(long pos)
      returns long;
    GetBuf(long pos, long len, long *lenp)
      returns char *;
    GetBufEnd(long endpos, long len, long *lenp)
      returns char *;
    GetGap(long pos, long len)
      returns char *;

    GetPosForLine(long line) returns long;
    GetLineForPos(long pos) returns long;
    GetBeginningOfLine(long pos) returns long;
    GetEndOfLine(long pos) returns long;

    CopyText(long pos, struct thisobject *srctext, long srcpos, long len)
      returns boolean;  /* FALSE if read-only */
    AlwaysCopyText(long pos, struct thisobject *srctext, long srcpos, long len);
    Index(long pos, char c, long len)
      returns long;
    Strncmp(long pos, char *str, long len)
      returns int;
    Textncmp(long pos, struct thisobject *text, long pos2, long len)
      returns int;

    /* Operations for FILE I/O */

    ReadSubString(long pos, FILE *file, boolean quoteCharacters)
      returns long;
    WriteSubString(long pos, long len, FILE *file, boolean quoteCharacters);
    CopySubString(long pos, long len, char *buf, boolean quoteCharacters);
    HandleDataObject(long pos, struct dataobject *dop, FILE *file)
      returns long;
    HandleKeyWord(long pos, char *keyword, FILE *file)
      returns long;
    HandleCloseBrace(long pos, FILE *file)
      returns long;
    HandleBegindata(long pos, FILE *file)
      returns long;

macromethods:
    GetFence() mark_GetLength((self)->fence)
    SetFence(pos) mark_SetLength((self)->fence, pos)
    ClearFence() mark_SetLength((self)->fence, 0L)
    GetFenceMark() ((self)->fence)
/* These two are not currently implemented but are for future plans... */
    GetObjectInsertionFlag() ((self)->objectInsertionAllowed)
    SetObjectInsertionFlag(flag) ((self)->objectInsertionAllowed = (flag))

data:
    char *string;
    long lowSize;
    long gapSize;
    long length;
    struct mark *markList;
    struct mark *fence;
    boolean pendingReadOnly;
    boolean objectInsertionAllowed;
    long Version;
};
