/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#define MODE822_NORMAL 1
#define MODE822_FORMAT 2
#define MODE822_FIXEDWIDTH 4
#define MODE822_ROT13 8

class text822: text {
    overrides:
      Read(FILE *file, long id) returns long;
      ReadSubString(long pos, FILE *file, boolean quoteCharacters) returns long;
      Write(FILE *file, long writeID, int level) returns long;
      ViewName() returns char *;
      Clear();
      ClearCompletely();
    methods:
      ReadAsText(FILE *file, long id) returns long;
    classprocedures:
      InitializeClass() returns boolean;
      ReadIntoText(struct text *d, FILE *fp, int Mode, char *ContentTypeOverride, int *len, boolean IsReallyTextObject, int *bodystart, int *ignorepos, struct text *auxheadtext) returns boolean;
      ResetGlobalStyle(struct text *t);
    data:
      struct style *InstructionsStyle, *BigBoldStyle;
};
