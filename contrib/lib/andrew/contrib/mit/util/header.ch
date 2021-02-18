/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#define header_HEADER 0
#define header_FOOTER 1

#define header_ltext 0
#define header_ctext 1
#define header_rtext 2
#define header_TEXTS 3

#define ALWAYS_ACTIVE_MODE

class header: dataobject[dataobj] {
classprocedures:
    Create(int type, char *left, char *center, char *right) returns struct thisobject *;
    InitializeObject(struct thisobject *self) returns boolean;
    FinalizeObject(struct thisobject *self);
    InitializeClass() returns boolean;
overrides:

    ObservedChanged(struct observable *t,long value);
    Read (FILE *file, long id) returns long;
    Write (FILE *file, long writeid, int level) returns long;
    ViewName() returns char *;
methods:
    SetHeader(int which, char *str);
data:
    long  where;
    struct text *texts[header_TEXTS];
    boolean active[header_TEXTS];
};
