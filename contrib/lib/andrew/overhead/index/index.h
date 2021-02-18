/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/index.h,v 2.4 1991/09/12 20:26:29 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/index.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/index/RCS/index.h,v 2.4 1991/09/12 20:26:29 bobg Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * here are the various record headers.  Records come in two major types, the 
 * primary records, which consist of an IPrimary subrecord, an IBucketList subrecord
 * and an IData subrecord, in that order, and a secondary record, consisting of an
 * ISecondary subrecord.
 
 * An IPrimary record is followed by an 8 byte UID, and a null-terminated keyword.
 * A bucket-list is followed by an array of 16 bit (network byte order, kiddies)
 * integers, terminated by an 0xffff entry.
 * An IData subrecord is followed by an uninterpreted null-terminated byte string.

 * Note that no byte strings, keys or names may contain any characters
 * less than or equal to IMax.
 
 * The database itself is comprised of a dir full of two types of files.  One file, named
 * V<version number>.<hash-table size> describes the version number and hash
 * table size.  The others, named H<number> contain the various hash buckets.
 * Each hash bucket contains a set of primary and secondary records whose keys
 * hash to the appropriate bucket.
 */

#define MAXSTRLENGTH		1024		/* max string size */
#define INDEXVERSION		1

struct recordID {
    long word1;		/* hash value */
    long word2;		/* counter */
};

#define IPRIMARY			1
#define IBUCKETLIST		2
#define IDATA			3

#define ISECONDARY		8
/* leave 9 unused, it is a tab */

#define IMAX				26

struct Index {
    long version;
    char *pathName;
    short hashTableSize;
    struct indexBucket *blist;
};

#define MAXHL		10
struct hashList {
    struct hashList *next;
    long nentries;
    short entries[MAXHL];
};

struct indexBucket {
    struct indexBucket *next;
    long hashIndex;
    long nextID;
    struct indexComponent *list;
    char modified;
};

struct indexComponent {
    struct indexComponent *next;
    char *name;
    struct hashList *hashes;
    struct recordID id;
    char *data;
    char primary;				/* true iff primary record */
};

#define INDEXOK			0
#define INDEXNOVERSION	(-1)
#define IBADV			(-2)
#define INDEXNOENT		(-3)

#define rset(a,b) (a).word1 = (b).word1; (a).word2 = (b).word2;
#define req(a,b) ((a).word1 == (b).word1 && (a).word2 == (b).word2)
#define rhash(a) (a).word1

struct recordSet {
    long count;				/* in-use slots */
    long acount;				/* allocated slots */
    struct recordID *data;
};

extern struct indexBucket *index_CGet();
extern struct hashList *index_NewHL();
extern struct indexComponent *index_FindID();
extern FILE *index_HashOpen();
extern struct Index *index_Open();
extern struct recordSet *index_GetPrimarySet();
extern struct recordSet *index_GetAnySet();
extern struct indexBucket *index_ReadIndex();
extern struct indexBucket *index_CGetHash();
extern struct indexBucket *index_CGet();
extern struct recordSet *recordset_New();
extern long index_Hash();
