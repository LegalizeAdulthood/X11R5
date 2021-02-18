/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.ch,v 2.6 1991/09/12 19:47:43 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_hash_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/hash.ch,v 2.6 1991/09/12 19:47:43 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* A hash table
 *
 */

#define hash_BUCKETS 32

class hash {

classprocedures:
    InitializeObject(struct hash *self) returns boolean;
    FinalizeObject(struct hash *self);
    InitializeClass() returns boolean;
methods:
    Lookup(char *key) returns char *;
    Store(char *key, char *value);
    Delete(char *key) returns char *;
    Rename(char *key, char *new) returns char *;
    Clear(procedure valFree);
    Debug();
macromethods:
    SetHash(fn) (self->hash = (fn))
data:
    struct glist *buckets[hash_BUCKETS];
    procedure hash;
};
