/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiterv.ch,v 2.9 1991/09/12 19:19:05 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiterv.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_arbiterview_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/adew/RCS/arbiterv.ch,v 2.9 1991/09/12 19:19:05 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
#define arbiterview_SEPCHAR ':'

class arbiterview[arbiterv]: celview[celv] {
overrides:
    ObservedChanged (struct observable *changed, long value);
    SetDataObject(struct dataobject *ls);
    WantHandler(char *handlerName) returns struct basicobject *;
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    LinkTree(struct view *parent);
methods:
    GetDataName() returns char *;
    registername(struct celview *cv,char *refname) returns int;
    InitCell(struct celview *cv);
    DeleteCell(struct celview *cv);
    CreateCon(struct text *EditText) returns boolean;
    lookupname(char *ViewName) returns struct celview *;
    ArbRead(char *frs);
    InitArbcon();
    GetArbName(char *buf,long buflen) returns long;
    AddHandler(int (*handler)(),long rock);
    InTree() returns boolean;
macromethods:
    GetCurrentCelview(cl) (self->currentcelview)
    GetCopyMode() (self->CopyMode)
    SetCopyMode(bool) self->CopyMode = bool;
    GetCelMode() (self->CelMode)
    SetCelMode(bool) self->CelMode = bool;
classprocedures:
    InitializeClass()returns boolean;
    FinalizeObject(struct arbiterv *self);
    InitializeObject(struct arbiterv *self) returns boolean;
    GetNamedObject(struct view *vw,char *ObjectName) returns struct dataobject *;
    GetNamedView(struct view *vw,char *ViewName) returns struct view *;
    GetNamedCelview(struct view *vw,char *ViewName) returns struct celview *;
    SetIgnoreUpdates(struct view *vw,boolean val);
    GetFirstLink() returns struct arbiterview*;
    FindArbByName(char *str) returns struct arbiterview *;
    FindArb(struct view *vw) returns struct arbiterview *;
data:
    struct celview **celviewlist;
    struct celview *currentcelview;
    int celcount , celsize;
    struct value *obval,*vwval,*objectchoice,*viewchoice,*applicationchoice,*NameChoice,*CelNameVal;
    struct text *EditText;
    struct arbiterview *next;
    boolean NeedsInit,CopyMode,CelMode;
    int (*handler)();
    long hrock;
};

