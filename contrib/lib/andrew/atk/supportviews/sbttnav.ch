/* ********************************************************************** *\
 *         Copyright IBM Corporation 1991 - All Rights Reserved           *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

struct groups {
    struct sbutton_prefs *prefs;
    struct menulist *ml;
    struct groups *next;
    int prio;
};

class sbttnav : sbuttonv {
overrides:
    Touch(int ind, enum view_MouseAction action) returns boolean;
    PostMenus(struct menulist *ml);
classprocedures:
    InitializeClass() returns boolean;
    InitializeObject(struct thisobject *self) returns boolean;
    FinalizeObject(struct thisobject *self);
data:
    struct menulist *ml;
    struct groups *groups;
    int groupcount;
    int dragging;
    struct cursor *cursor;
};

