/*
*******************************************************************************
* Copyright 1991 by Auto-trol Technology Corporation, Denver, Colorado.
*
*                        All Rights Reserved
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appears on all copies and that both the
* copyright and this permission notice appear in supporting documentation
* and that the name of Auto-trol not be used in advertising or publicity
* pertaining to distribution of the software without specific, prior written
* permission.
*
* Auto-trol disclaims all warranties with regard to this software, including
* all implied warranties of merchantability and fitness, in no event shall
* Auto-trol be liable for any special, indirect or consequential damages or
* any damages whatsoever resulting from loss of use, data or profits, whether
* in an action of contract, negligence or other tortious action, arising out
* of or in connection with the use or performance of this software.
*******************************************************************************
* SCCS_data: @(#)PreR5Enum.c 1.1 ( 19 Sep 91 )
*
* Subsystem_group:
*
*     Widget Creation Library
*
* Module_description:
*
*     This module contains the functions used to provide the X11 R5
*     XrmEnumerateDatabase for the pre-X11R5 environments.
*
*     As this code is based upon the MIT X Consortium sample Xlib 
*     implementation of Xrm.c, no guarantee can be given that this code 
*     will execute with any vendor-specific Xlib implementation, which 
*     may have chosen a completely different internal Xrm database 
*     representation.
*
*     This code attemptes to be R5 compatible, however, some of the sample 
*     X11 R5 implementation/behavior does not seem to quite match the 
*     specification.  In particular, specification "*a.b:" will match a 
*     pattern "a.a.a.a" up to four times (many similar patterns have the 
*     same problem). Also, only limited testing has been performed for
*     XrmEnumOneLevel code (currently not used by Wcl).
*
*
* Module_history:
*     Author: Martin Brunecky, Auto-trol Technology, 09/23/91
*
*     mm/dd/yy  initials  function  action
*     --------  --------  --------  ------------------------------------------
*     09/23/91  MarBru    All       Created
*
* Design_notes:
*
*     For testing purposes, MIT Xrm DB data dumping code is included here.
*
*******************************************************************************
*/ 
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include        <stdio.h>
#include        <ctype.h>


/* The hash size from MIT X11R4. If your vendor used another one, tough luck */
#define HASHSIZE    64 

/* Pick a reasonable value for maximum depth of resource database */
#define MAXDBDEPTH 200

/* New R5 definitions, not available with X11R4 */
#ifndef XrmEnumAllLevels
#define XrmEnumAllLevels 0
#define XrmEnumOneLevel  1
typedef caddr_t XPointer;
#endif

#ifdef  DEBUG
#define ONDEBUG(x) {x}
#else
#define ONDEBUG(x) {/* x */}
#endif


/*
** Types defined in  vendor supplied X11 header files, but expanded here
** typedef struct _XrmHashBucketRec        *XrmHashBucket;
** typedef XrmHashBucket   *XrmHashTable;
** typedef XrmHashTable XrmSearchList[];
*/
typedef Bool (*DBEnumProc)();

typedef struct _XrmHashBucketRec {
    XrmHashBucket       next;       /* Next entry in this hash chain        */
    XrmQuark            quark;      /* Quark for string                     */
    XrmRepresentation   type;       /* Representation of value (if any)     */
    XrmValue            value;      /* Value of this node (if any)          */
    XrmHashTable        tables[2];  /* Hash table pointers for tight, loose */
    char                hasValues[2]; /* Do any buckets in table have value */
} XrmHashBucketRec;


/* closure used in enumerate database */
typedef struct _EClosure {
    XrmDatabase db;                     /* the database */
    DBEnumProc proc;                    /* the user proc */
    XPointer closure;                   /* the user closure */
    XrmBindingList bindings;            /* binding list */
    XrmQuarkList quarks;                /* quark list */
    int mode;                           /* XrmEnum<kind> */
} EClosureRec, *EClosure;

#define ALL        MAXDBDEPTH
#define ONE        1
#define NEXT       (-1)

static Bool EnumTables (db, level, do_tight, do_loose, do_value, eclosure)
    XrmHashBucket   db;            /* database bucket at this level */
    unsigned        level;         /* level number, start at 0      */
    int             do_tight;      /* process tight list at this level*/
    int             do_loose;      /* process loose list at this level*/
    Bool            do_value;      /* process res. value at this level*/
    EClosureRec    *eclosure;      /* pass-through context          */
{
    XrmDatabase             dbase;
    XrmBindingList          bindings;
    XrmQuarkList            quarks;
    DBEnumProc              proc;
    caddr_t                 closure;
    int                     mode;     /* not used */
    register int            i;
    register XrmHashBucket  bucket;
    register XrmHashTable   table;


#define ENUMTable(binding)                                                  \
{                                                                           \
    table = db->tables[(int) binding];                                      \
    if (table != NULL) {                                                    \
        if (do_tight == NEXT) do_tight = ALL;                               \
        if (do_loose == NEXT) do_loose = ALL;                               \
        bindings[level] = binding;                                          \
        quarks[level+1] = NULLQUARK;                                        \
        for (i=0; i < HASHSIZE; i++) {                                      \
            bucket = table[i];                                              \
            while (bucket != NULL) {                                        \
                quarks[level] = bucket->quark;                              \
                if (EnumTables(bucket, level+1, do_tight - 1,            \
                    do_loose - 1, True, eclosure))                          \
                   return True;                                             \
                bucket = bucket->next;                                      \
            }                                                               \
        }                                                                   \
    }                                                                       \
} /* ENUMTable */

    if (db == NULL || level >= MAXDBDEPTH) return False;

    dbase    = eclosure->db;
    bindings = eclosure->bindings;
    quarks   = eclosure->quarks;
    proc     = eclosure->proc;
    closure  = eclosure->closure;

    if ( do_tight > 0 ) ENUMTable(XrmBindTightly);
    if ( do_loose > 0 ) ENUMTable(XrmBindLoosely);

    /* Call provided procedure for the resource in this table */
    if ( do_value && db->value.addr != NULL) {
        quarks[level] = NULLQUARK;
        if ((*proc)( dbase, bindings, quarks, &db->type, &(db->value), closure))
            return True;
    }
    return False;
#undef ENUMTable
}
static Bool EnumMatchingTables();
/*
  -- Match 
******************************************************************************
  In current bucket's table(loose/tight) find the bucket matching a given
  name(or class name) and process it depending on Enum mode.
  This routine should be inlined or turned into a macro, but for the sake
  of debugging this is more usefull -)
*/
static Bool Match (match, binding, db, names, classes, level, eclosure )

    XrmQuark       *match;
    XrmBinding      binding;
    XrmHashBucket   db;
    XrmQuarkList    names;
    XrmQuarkList    classes;
    unsigned        level;
    EClosureRec    *eclosure;
{                                      
    register int            i;
    register XrmHashBucket  bucket;
    register XrmHashTable   table;

    table = db->tables[(int) binding];  
    if (table != NULL) {  /* find the bucket (DB) with matching name */   
        for (i=0, bucket=NULL; i < HASHSIZE && bucket == NULL ; i++) {      
            bucket = table[i];                                              
            while (bucket != NULL && bucket->quark != *match )              
                bucket = bucket->next;                                      
            }                                                               
        if (bucket) {    /* found a DB bucket with matching name */          
           eclosure->bindings[level] = binding;
           eclosure->quarks  [level] = bucket->quark;   
           if ( names[1] ) {    /* next search level is specified, recurse */
              if (EnumMatchingTables( bucket, names+1, classes+1, level+1,  
                  eclosure))                                                
              return True;                                                  
           } else {             /* reached an end of search path in the DB */
              if ( eclosure->mode == XrmEnumAllLevels ) { 
                 if (EnumTables(bucket,level+1, ALL, ALL, False, eclosure))
                    return True;                                
              } else /*      mode == XrmEnumOneLevel */ {
                 if (EnumTables(bucket,level+1, ONE, ONE, False, eclosure))
                    return True;                                
              }                                                             
           }                                                                
        }  /* if (bucket) */                                                
    } /* if (table) */    

    return False;

} /* Match */


static Bool EnumMatchingTables (db, names, classes, level, eclosure)
    XrmHashBucket   db;
    XrmQuarkList    names;
    XrmQuarkList    classes;
    unsigned        level;
    EClosureRec    *eclosure;
{
#define MATCHandENUM(match, binding) \
        if ( Match (match, binding, db, names, classes, level, eclosure ) ) \
             return True;

    if (db == NULL || level >= MAXDBDEPTH) return False;

    /* Match the TIGHT bindings at THIS DB LEVEL and recurse */
    MATCHandENUM ( names,   XrmBindTightly );
    MATCHandENUM ( classes, XrmBindTightly );

    /* Process LOOSE bindings at THIS DB level, if any */ 
    if ( db->tables[ XrmBindLoosely ] )
       {

       if ( eclosure->mode == XrmEnumAllLevels )
          /* For AllLevels, LOOSE matches entire tree from loose table down */
          {
          if (EnumTables( db, level, NEXT, ALL, False, eclosure ))
             return True;
          }
          else
          /* For OneLevel, we can only pick appropriate leaf resources */
          {
          /* Get all loose bound leaves at this DB level: a.a.a*anything: */
          if (EnumTables( db, level, 0, ONE, False, eclosure ))
             return ( True );

          /* any subsequent name/class match gives another set of leaves */
          /* or the entire next level of search path / database matching */
          /* problem: spec *a.b: will match path a.a.a.a 4 times etc.... */
          while (1)
             { 
             MATCHandENUM ( names,   XrmBindLoosely );
             MATCHandENUM ( classes, XrmBindLoosely );
             names++;
             classes++;
             if ( *names == NULLQUARK ) 
                break;
             }
          }
      } 

    return False;

} /* EnumMatchingTables */
   
/* 
 * call the proc for every value in the database, arbitrary order.
 * stop if the proc returns True.
 */
Bool XrmEnumerateDatabase(db, names, classes, mode, proc, closure)
    XrmDatabase         db;
    XrmNameList         names;
    XrmClassList        classes;
    int                 mode;
    DBEnumProc          proc;
    XPointer            closure;
{
    XrmBinding          bindings[MAXDBDEPTH+2];
    XrmQuark            quarks[MAXDBDEPTH+2];
    EClosureRec         eclosure;

    if (!db) return False;

    eclosure.db       = db;
    eclosure.proc     = proc;
    eclosure.closure  = closure;
    eclosure.bindings = bindings;
    eclosure.quarks   = quarks;
    eclosure.mode     = mode;

    if ( !names || *names == NULLQUARK )
       return EnumTables (db, 0, ALL, ALL, False, &eclosure);
    else
       return EnumMatchingTables (db, names, classes, 0, eclosure);
}

