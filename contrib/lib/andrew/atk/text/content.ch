/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */


struct content_chapentry {
    struct mark *rem,*loc;
    struct content_chapentry *next;
    int which,space;
};

class content : text {
overrides:
      ObservedChanged (struct observable *changed, long value);
      ViewName() returns char *;
methods:
      reinit();
      SetSourceText(struct text *txt);
      UpdateSource(long pos,long len);
      Enumerate(long pos,long len,char *start) returns long;
      Denumerate(long pos,long len);
      StringToInts(long pos,int *lev) returns long;
      locate(long pos) returns struct mark *;
      CopyEntry(long pos,long len,char *buf,long buflen) returns  struct content_chapentry*;
classprocedures:
      FinalizeObject(struct content *self);
      InitializeObject(struct content *self) returns boolean;
data:
      struct content_chapentry *entry,*indexentry;
      struct text *srctext;
      char *names[48];
      int namecount;
      int InUpdate;
      int enumerate;
      char *ChapNumber;
      boolean doindent,isindented;
      int chapcount;
};

 
