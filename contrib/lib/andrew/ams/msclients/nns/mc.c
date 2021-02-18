/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *mc_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/mc.c,v 2.4 1991/09/12 15:51:30 bobg Exp $";


/* Methods for the Message Cache data type for the Netnews Snarfer
*/

/* BOGUS: Include relevant AMS header(s) here */

#include <big.h>

static int      MCHash(string)
char           *string;
{
    int             result = 0;
    char           *p = string;

    while (result += *p, *(p++));
    return (result % NUMMCBUCKETS);
}

void            MCInit(mc)
MCache_t       *mc;
{
    int             i;

    mc->HashFn = MCHash;
    for (i = 0; i < NUMMCBUCKETS; ++i)
	MCBInit(&(mc->buckets[i]));
}

void            MCPurge(mc)
MCache_t       *mc;
{
    int             i;

    for (i = 0; i < NUMMCBUCKETS; ++i)
	MCBPurge(&(mc->buckets[i]));
}

int             MCMake(mc, string, Msg)
MCache_t       *mc;
char           *string;
struct MS_Message *Msg;
{
    return (MCBMake(&(mc->buckets[(*(mc->HashFn)) (string)]), string, Msg));
}

void            MCDelete(mc, string)
MCache_t       *mc;
char           *string;
{
    MCBDelete(&(mc->buckets[(*(mc->HashFn)) (string)]), string);
}

struct MS_Message *MCFind(mc, string)
MCache_t       *mc;
char           *string;
{
    return (MCBFind(&(mc->buckets[(*(mc->HashFn)) (string)]), string));
}
