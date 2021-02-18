/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/filetype.ch,v 2.8 1991/09/12 19:21:47 bobg Exp $ */
/* $ACIS:filetype.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/filetype.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidfiletype_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/filetype.ch,v 2.8 1991/09/12 19:21:47 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* filetype.h
 * Declarations for the filetype package.
 *
 */


package filetype {
    classprocedures:
        AddEntry(char *extesion, char *dataName, char *attributes);
        AddExistingAttributes(char *extension, char *dataName, char *attributes);
        DeleteEntry(char *extension) returns int; /* The return value here is dubious... */
/* NOTE: filetype_Lookup.
 *    The value returned through the attributes parameter to this function
 *    contains pointers to static storage. Its value should be used before
 *    the next call to this routine. If this can't be guaranteed, the
 *    programmer must copy the attributes list.
 */
        Lookup(FILE *file, char *filename, /* OUT parameter */ long *objectID,
              /* OUT parameter */ struct attributes **attributes) returns char *;
        CanonicalizeFilename(char *canonicalName, char *name, int maxSize);
};
