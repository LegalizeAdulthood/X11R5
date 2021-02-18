/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keymap.ch,v 2.7 1991/09/12 19:22:15 bobg Exp $ */
/* $ACIS:keymap.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keymap.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidkeymap_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/keymap.ch,v 2.7 1991/09/12 19:22:15 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* keymap.ch -- A class that provides mappings of keys to procedures.
December, 1986 */

/* Number of entries in a keymap.  Should be power of two. */
#define keymap_MAXKEYS	256
/* Number of entries in a sparse map. Should be a multiple of 4 for good
 * structure packing. */
#define keymap_SPARSESIZE 16

/* These are the values that the return values from Lookup can take on. */

enum keymap_Types  {
    keymap_Empty,	/* no binding for this key */
    keymap_Proc,	/* binding is a proctable structure */
    keymap_Keymap	/* binding is another keymap */
};

struct keymap_sparsetable {
    int numValid;
    char keys[keymap_SPARSESIZE];
    enum keymap_Types types[keymap_SPARSESIZE];
    struct basicobject *objects[keymap_SPARSESIZE];
    long rocks[keymap_SPARSESIZE];
};    

struct keymap_fulltable {
    enum keymap_Types types[keymap_MAXKEYS];
    struct basicobject *objects[keymap_MAXKEYS];
    long rocks[keymap_MAXKEYS];
};

class keymap {
methods:
    BindToKey(char *keys, struct proctable_Entry *pe, long rock)	returns boolean;
    RemoveBinding(char *keys);
    InsertObject(long slot, struct basicobject *object, long rock, enum keymap_Types type);
    /* Lookup stuffs an object in the char * and returns an int that describes its type. */
    Lookup(char key, struct basicobject **object, long *rockP) returns enum keymap_Types;
data:
    union {
        struct keymap_sparsetable *sparse;
        struct keymap_fulltable *full;
    } table;
    boolean sparsep;
};
