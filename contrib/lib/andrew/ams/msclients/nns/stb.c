/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *stb_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/stb.c,v 2.4 1991/09/12 15:51:47 bobg Exp $";


/* Methods for string table buckets
*/

#include <big.h>

void            STBInit(stb)
STableBucket_t *stb;
{
    stb->num = stb->size = 0;
    stb->entries = NULL;
}

char           *STBFind(stb, string)
STableBucket_t *stb;
char           *string;
{
    int             i;
    char           *result = NULL;

    for (i = 0; (i < stb->num) && (!result); ++i) {
	if (!strcmp(string, STBEGetString(&(stb->entries[i]))))
	    result = STBEGetString(&(stb->entries[i]));
    }
    return (result);
}

char           *STBMake(stb, string)
STableBucket_t *stb;
char           *string;
{
    char           *tmp = NULL;

    if (STBGrowIfNecessary(stb)) {
	if (tmp = (char *) malloc(strlen(string) + 1)) {
	    strcpy(tmp, string);
	    STBESetString(&(stb->entries[(stb->num)++]), tmp);
	}
    }
    return (tmp);
}

void            STBPurge(stb)
STableBucket_t *stb;
{
    int             i;

    for (i = 0; i < stb->num; ++i)
	free(STBEGetString(&(stb->entries[i])));
    free(stb->entries);
    stb->num = stb->size = 0;
    stb->entries = NULL;
}

static int      STBGrowIfNecessary(stb)
STableBucket_t *stb;
{
    STableBucketEntry_t *tmp;

    if (stb->num == stb->size) {
	if (stb->size) {
	    if (!(tmp = (STableBucketEntry_t *) realloc(stb->entries, (stb->size + STB_GROWSIZE) * sizeof(STableBucketEntry_t))))
		return (FALSE);
	}
	else {
	    if (!(tmp = (STableBucketEntry_t *) malloc(STB_GROWSIZE * sizeof(STableBucketEntry_t))))
		return (FALSE);
	}
	stb->entries = tmp;
	stb->size += STB_GROWSIZE;
    }
    return (TRUE);
}
