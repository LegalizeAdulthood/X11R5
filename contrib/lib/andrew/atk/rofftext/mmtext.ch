/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* mmtext.ch: link to rofftext -mm */

#include <mark.ih>

class mmtext: rofftext {
classprocedures:
    InitializeObject(struct mmtext *self) returns boolean;
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
