/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */




#include <class.h>



/*
 * some constants, etc.
 */

#define INDEXLOCKFILE "index.lck"
#define INDEXFILE "index"
#define INDEXNEWFILE "index.new"




struct IndexEntry {
    char * Name;
    char * Key;
    unsigned long Version;
    char * Data;
};






extern struct IndexEntry *
ReadEntry(/* fd */);

extern boolean
WriteEntry(/* fd, entry */);


extern struct IndexEntry *
NewEntry(/* */);

extern struct IndexEntry *
CreateEntry(/* name, key, version, data */);

extern void
DestroyEntry(/* entry */);


extern char * 
GetEntryName(/* entry */);

extern char * 
GetEntryKey(/* entry */);

extern unsigned long
GetEntryVersion(/* entry */);

extern char * 
GetEntryData(/* entry */);


extern boolean
SetEntryName(/* entry, name */);

extern boolean
SetEntryKey(/* entry, key */);

extern boolean
SetEntryVersion(/* entry, version */);

extern boolean
SetEntryData(/* entry, data */);







