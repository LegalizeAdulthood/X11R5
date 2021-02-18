/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *champ_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/champ.c,v 2.3 1991/09/12 16:03:03 bobg Exp $";

#include "class.h"
#include "champ.eh"

extern struct eventnode *readdateintoeventnode();

int champ__ReadDatesFromChampPath(c, path)
struct classheader *c;
char *path;
{
    return(ReadDatesFromChampPath(path));
}

void champ__ClearAllFlaggedEvents(c)
struct classheader *c;
{
    ClearAllFlaggedEvents();
}

int champ__FlagEventsMatchingDate(c, thisdate)
struct classheader *c;
struct tm *thisdate;
{
    return(FlagEventsMatchingDate(thisdate));
}


void champ__IterateFlaggedEvents(c, proc, rock)
struct classheader *c;
int (*proc)();
long rock;
{
    IterateFlaggedEvents(proc, rock);
}

void champ__IncrementDate(c, d)
struct classheader *c;
struct tm *d;
{
    IncrementDate(d);
}

struct eventnode *champ__ReadDateIntoEventNode(c, str)
struct classheader *c;
char *str;
{
    return(readdateintoeventnode(str));
}

