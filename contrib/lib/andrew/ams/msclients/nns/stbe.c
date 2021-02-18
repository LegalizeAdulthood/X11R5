/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *stbe_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/stbe.c,v 2.4 1991/09/12 15:51:50 bobg Exp $";


/* Methods for String Table Bucket Entries
*/

#include <big.h>

char           *STBEGetString(stbe)
STableBucketEntry_t *stbe;
{
    return (*stbe);
}

void            STBESetString(stbe, string)
STableBucketEntry_t *stbe;
char           *string;
{
    *stbe = string;
}
