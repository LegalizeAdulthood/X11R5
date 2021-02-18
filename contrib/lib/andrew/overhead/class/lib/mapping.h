/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */



#ifndef mapping_DEFINED
#define mapping_DEFINED 1

/*
 * Package of routines to manipulate the mapping between
 * class names and class name keys.
 */

#include <class.h>	/* get data structures for classheader data */




struct MapEntryStruct {
    char * Name;
    char * Key;
    class_VersionNumberType Version;
    char * Data;
    int	PathIndex;
    struct MapEntryStruct * NameLink;
    struct MapEntryStruct * KeyLink;
};




/* 
 * Initialize this package.  All data is free'd,
 * data structures are initialized, etc.  0 return
 * indicates success, -1 indicates failure.
 */
extern int
InitializeMappingPackage(/* */);


/*
 * Clear all the entries in the mapping table.  
 * All the pointers are left pointing to NULL.
 * 0 is returned for success, -1 for failure.
 */
extern int
ClearAllMappingEntries(/* */);


/*
 * Enter the string path to the list of paths and return
 * the index assigned to this path.  -1 is returned if 
 * the path cannot be registered.
 */
extern int
EnterPathEntry(/* path */);


/*
 * Retrieve the a pointer to the path 
 * with registered as index.  NULL is returned if
 * something is wrong.
 */
extern char *
GetPathEntry(/* index */);


/*
 * Enter an entry into the mapping database.  0 return
 * indicates success, -1 indicates failure.
 */
extern int
EnterMapEntry(/* name, key, version, pathindex, data */);


/*
 * Use the class name to get string that is 
 * used to load the object.  NULL is returned
 * if something goes wrong.  The string
 * pointed to by the returned value must be 
 * free'd by the caller.
 */
extern char *
MapByName(/* name, version */);


/*
 * Use the key to get string that is 
 * used to load the object.  NULL is returned
 * if something goes wrong.  The string
 * pointed to by the returned value must be 
 * free'd by the caller.
 */
extern char *
MapByKey(/* key, version */);


/*
 * Use the class name to retrieve the mapping entry.
 * NULL is returned if something goes wrong.  The 
 * returned value points to a string that must not
 * be modified or free'd by the caller.
 */
extern struct MapEntryStruct *
RetrieveByName(/* name, version */);


/*
 * Use the key to retrieve the mapping entry.
 * NULL is returned if something goes wrong.  The 
 * returned value points to a string that must not
 * be modified or free'd by the caller.
 */
extern struct MapEntryStruct *
RetrieveByKey(/* key, version */);


/* 
 * Dump a formatted list of the mapping entries
 * to file.
 */
extern void
DumpMappingInfo(/* file */);


/* 
 * Dump a formatted list of the mapping statistics
 * to file.
 */
extern void
DumpMappingStats(/* file */);



#endif /* #ifndef mapping_DEFINED */

