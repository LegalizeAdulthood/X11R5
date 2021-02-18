/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *gle_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/gle.c,v 2.4 1991/09/12 15:51:27 bobg Exp $";


/* Methods for Group List Entries
*/

#include <big.h>

void            GLESetBefore(gle, val)
GListEntry_t   *gle;
int             val;
{
    gle->before = val;
}

void            GLESetAhead(gle, val)
GListEntry_t   *gle;
int             val;
{
    gle->ahead = val;
}

char           *GLEGetFilename(gle)
GListEntry_t   *gle;
{
    return (gle->filename);
}

void            GLESet(gle, filename, newsgroup, ahead, before, ignore, time)
GListEntry_t   *gle;
char           *filename, *newsgroup;
int             ahead, before, ignore;
long            time;
{
    gle->filename = filename;
    gle->newsgroup = newsgroup;
    gle->ahead = ahead;
    gle->before = before;
    gle->ignore = ignore;
    gle->time = time;
}

void            GLESetIgnore(gle, ignore)
GListEntry_t   *gle;
int             ignore;
{
    gle->ignore = ignore;
}

int             GLEGetIgnore(gle)
GListEntry_t   *gle;
{
    return (gle->ignore);
}

char           *GLEGetNewsgroup(gle)
GListEntry_t   *gle;
{
    return (gle->newsgroup);
}

int             GLEGetBefore(gle)
GListEntry_t   *gle;
{
    return (gle->before);
}

int             GLEGetAhead(gle)
GListEntry_t   *gle;
{
    return (gle->ahead);
}

int             GLECompare(gle1, gle2)
GListEntry_t   *gle1, *gle2;
{
    int             result = strcmp(gle1->newsgroup, gle2->newsgroup);

    if (!result)
	result = (int) (gle1->time - gle2->time);
    return (result);
}
