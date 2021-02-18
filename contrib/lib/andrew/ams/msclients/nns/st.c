/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *st_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/st.c,v 2.4 1991/09/12 15:51:45 bobg Exp $";


/* Methods for the String Table data type for the Netnews Snarfer
*/

#include <big.h>

static int      STHash(string)
char           *string;
{
    int             result = 0;
    char           *p = string;

    while (result += *p, *(p++));
    return (result % NUMSTBUCKETS);
}

void            STInit(st)
STable_t       *st;
{
    int             i;

    st->HashFn = STHash;
    for (i = 0; i < NUMSTBUCKETS; ++i)
	STBInit(&(st->buckets[i]));
}

void            STPurge(st)
STable_t       *st;
{
    int             i;

    for (i = 0; i < NUMSTBUCKETS; ++i)
	STBPurge(&(st->buckets[i]));
}

char           *STFindOrMake(st, string)
STable_t       *st;
char           *string;
{
    char           *tmp = STFind(st, string);

    if (!tmp)
	tmp = STMake(st, string);
    return (tmp);
}

char           *STFind(st, string)
STable_t       *st;
char           *string;
{
    int             hash = (*(st->HashFn)) (string);
    char           *tmp = STBFind(&(st->buckets[hash]), string);

    return (tmp);
}

char           *STMake(st, string)
STable_t       *st;
char           *string;
{
    return (STBMake(&(st->buckets[(*(st->HashFn)) (string)]), string));
}
