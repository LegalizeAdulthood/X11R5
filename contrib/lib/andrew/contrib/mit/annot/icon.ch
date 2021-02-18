/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#define icon_TitleChanged 34935	/* random integers... */
#define icon_SizeChanged 1894
#define icon_ChildChanged 2435
#define icon_ChangedSomehow 90235

class icon : dataobject[dataobj] {
classprocedures:
  InitializeObject(struct thisobject *self) returns boolean;
  InitializeClass() returns boolean;
overrides:
    Read (FILE *file, long id) returns long;
    Write (FILE *file, long writeid, int level) returns long;
methods:
    SetSize(long w,long h);
    GetSize(long * w, long * h);
    SetChild(struct dataobject * child);
    GetChild() returns struct dataobject *;
    SetTitle(char * title);
    GetTitle() returns char *;
data:
    struct dataobject * child;
    long width, height;
    char * title;
};
