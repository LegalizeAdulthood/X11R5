/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

class ps: icon {
 overrides:
   SetChild(struct dataobject * child);
    Read (FILE *file, long id) returns long;
    Write (FILE *file, long writeid, int level) returns long;
 methods:
 macromethods:
    GetPixelWidth() ((self)->pwidth)
    GetPixelHeight() ((self)->pheight)
    SetPixelWidth(value)  ((self)->pwidth=(value))
    SetPixelHeight(value)  ((self)->pheight=(value))
 classprocedures:
    InitializeObject(struct thisobject *self) returns boolean;
    InitializeClass() returns boolean;
 data:
    long pwidth;
    long pheight;
};

