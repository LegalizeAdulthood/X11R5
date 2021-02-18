/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* mantext.ch: link to rofftext -man */

#include <mark.ih>

class mantext: rofftext {
classprocedures:
    InitializeObject(struct mantext *self) returns boolean;
  overrides:
    Read(FILE *file, long id) returns long;
  methods:
    GetLinePos(long line) returns long;
    SetLinePos(long line, long pos);
    GetFilename(char *filename);
  data:
    int nLines, nAlloc;
    struct mark **lineMark;
    char filename[500];
};
