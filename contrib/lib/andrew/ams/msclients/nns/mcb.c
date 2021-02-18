/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *mcb_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/mcb.c,v 2.4 1991/09/12 15:51:33 bobg Exp $";


/* Methods for Message Cache Buckets
*/

/* BOGUS: include relevant AMS header(s) here */

#include <big.h>

void            MCBInit(mcb)
MCacheBucket_t *mcb;
{
    *mcb = NULL;
}

struct MS_Message *MCBFind(mcb, string)
MCacheBucket_t *mcb;
char           *string;
{
    MCacheBucketEntry_t *mcbe = *mcb;
    struct MS_Message *result = NULL;

    while (mcbe && !result) {
	if (strcmp(MCBEGetFilename(mcbe), string))
	    mcbe = MCBEGetNext(mcbe);
	else
	    result = MCBEGetMsg(mcbe);
    }
    return (result);
}

int             MCBMake(mcb, string, Msg)
MCacheBucket_t *mcb;
char           *string;
struct MS_Message *Msg;
{
    MCacheBucketEntry_t *tmp = (MCacheBucketEntry_t *) malloc(sizeof(MCacheBucketEntry_t));

    if (tmp) {
	MCBESet(tmp, string, Msg, *mcb);
	*mcb = tmp;
	return (TRUE);
    }
    return (FALSE);
}

void            MCBDelete(mcb, string)
MCacheBucket_t *mcb;
char           *string;
{
    MCacheBucketEntry_t *mcbe = *mcb, *prevmcbe = NULL;

    while (mcbe) {
	if (strcmp(MCBEGetFilename(mcbe), string)) {
	    prevmcbe = mcbe;
	    mcbe = MCBEGetNext(mcbe);
	}
	else {
	    if (prevmcbe)
		MCBESetNext(prevmcbe, MCBEGetNext(mcbe));
	    else
		*mcb = MCBEGetNext(mcbe);
	    free(mcbe);
	    mcbe = NULL;
	}
    }
}

void            MCBPurge(mcb)
MCacheBucket_t *mcb;
{
    MCacheBucketEntry_t *mcbe = *mcb, *next;

    while (mcbe) {
	next = MCBEGetNext(mcbe);
	free(mcbe);
	mcbe = next;
    }
    *mcb = NULL;
}
