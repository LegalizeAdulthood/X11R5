/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keystate.ch,v 2.8 1991/09/12 19:22:28 bobg Exp $ */
/* $ACIS:keystate.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keystate.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidkeystate_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keystate.ch,v 2.8 1991/09/12 19:22:28 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* keystate.ch -- A class that keep track of partially evaluated sequences of keystrokes.
December, 1986 */


#define keystate_VERSION	1

/* Return values from ApplyKey: */

enum keystate_ApplyKeyValues  {
    keystate_ProcFound,		/* pe has an entry */
    keystate_NoBinding,		/* key is undefined */
    keystate_Pending		/* keep feeding chars */
};

/* Return values from DoProc: */

enum keystate_DoProcValues {
    keystate_ProcCalled,	/* success */
    keystate_TypeMismatch,	/* couldn't find object of correc type */
    keystate_NoProc		/* couldn't load proc */
};

#include <keymap.ih>

class keystate {
macromethods:
    GetObject() (self->object)
methods:
    SetObject(struct basicobject *object);
    AddBefore(struct keystate *ks) returns struct keystate *;
    AddAfter(struct keystate *ks) returns struct keystate *;
    /* Routines for playing keystokes. */
    Reset();	/* inits whole chain and clears arg junk */
    ApplyKey(char key, struct proctable_Entry **ppe, long *rockP, struct basicobject **pobject) returns enum keystate_ApplyKeyValues;
    DoProc(struct proctable_Entry *pe, long rock, struct basicobject *object) returns enum keystate_DoProcValues;
    /* Zap a chain of keystates. */
    FreeChain();	/* Calls destroy on each keystate object */
    SetOverride(procedure function, long functionData);
    GetOverride(procedure *function, long *functionData);
classprocedures:
    Create(struct basicobject *object, struct keymap *keymap) returns struct keystate *;
    /* These are internal to the implementation. */
    Allocate() returns struct keystate *;
    Deallocate(struct keystate *ks);

data:
    struct keystate *next;	/* links together keymaps */
    struct keymap *orgMap;	/* keymap to use */
    struct keymap *curMap;	/* where we are in traversal */
    struct basicobject *object;	/* first argument to function */
    enum keymap_Types (*function)(); /* Override function to trap keystrokes. */
    long functionData; /* Rock for the override function. */
};

