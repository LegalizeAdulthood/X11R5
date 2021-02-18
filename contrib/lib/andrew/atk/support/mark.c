/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.c,v 2.4 1991/09/12 16:31:36 bobg Exp $ */
/* $ACIS:mark.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/mark.c,v 2.4 1991/09/12 16:31:36 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <mark.eh>

/* Crank out marks in 4k blocks. */
#define DESIREDBLOCKSIZE 4096
/* Number of marks per block */
#define NUMPERBLOCK DESIREDBLOCKSIZE / sizeof(struct mark)
#define BLOCKSIZE NUMPERBLOCK * sizeof(struct mark)

static struct mark *freeMarks = NULL;
static struct mark *lastBlock = NULL;

struct mark *mark__Allocate(classID)
struct classheader *classID;
{

    static int lastIndex = NUMPERBLOCK; /* Force a block malloc on first call. */

    if (freeMarks) {
        struct mark *tempMark = freeMarks;
        freeMarks = freeMarks->next;
        return tempMark;
    }
    if (lastIndex >= NUMPERBLOCK) {
        lastBlock = (struct mark *) malloc(BLOCKSIZE);
        lastIndex = 0;
    }
    return &lastBlock[lastIndex++];
}

void mark__Deallocate(classID, self)
struct classheader *classID;
    struct mark *self;
{

    self->next = freeMarks;
    freeMarks = self;
}

boolean mark__InitializeObject(classID, self)
struct classheader *classID;
struct mark *self;  {
    self->next = NULL;
    self->pos = 0;
    self->length = 0;
    self->modified = FALSE;
    self->objectFree = FALSE;
    self->includeBeginning = FALSE;
    self->includeEnding = TRUE;

    return TRUE;
}

void mark__SetStyle(self, beginning, ending)
struct mark *self;
boolean beginning;
boolean ending;  {
    self->includeBeginning = beginning;
    self->includeEnding = ending;
}

struct mark *mark__NewWithStyle(classID, beginning, ending)
struct classheader *classID;
boolean beginning;
boolean ending;  {
    struct mark *nmark;

    nmark = mark_New();
    nmark->includeBeginning = beginning;
    nmark->includeEnding = ending;
    return nmark;
}

void mark__UpdateMarks(self, pos, size)
    struct mark *self;
    long pos;
    long size;
{

    struct mark *mark;
    long tpos;
    long tsize;
    long endPos;

    if (size == 0) return;
    
    for (mark = self; mark != NULL; mark = mark_GetNext(mark))  {
	tpos = pos;
	tsize = size;
	if (tpos <= (endPos = mark_GetEndPos(mark)))  {
	    if (tsize > 0)  {
		if (tpos == endPos)  {
		    if (mark_IncludeEnding(mark))  {
			mark_SetModified(mark, TRUE);
			mark_SetLength(mark, mark_GetLength(mark) + tsize);
		    }
		}
		else if (tpos < mark_GetPos(mark) || tpos == mark_GetPos(mark) && ! mark_IncludeBeginning(mark))
		    mark_SetPos(mark, mark_GetPos(mark) + tsize);
		else {
		    mark_SetLength(mark, mark_GetLength(mark) + tsize);
		    mark_SetModified(mark, TRUE);
		}
	    }
	    else if (tpos < endPos) {
		if (tpos-tsize <= mark_GetPos(mark))  {
/* 		    Deleted region is before the mark
 */		    
		    mark_SetPos(mark, mark_GetPos(mark) + tsize);
		}
		else  {
		    if (tpos <= mark_GetPos(mark))  {
/* 			Delete portion before the mark
 */			
		    tsize += mark_GetPos(mark) - tpos;
			mark_SetPos(mark, tpos);
		    }
		    
/* 		    Reset the size of the deleted region to only include the mark
 */		    
		    if (tpos - tsize > endPos)
			tsize = tpos - endPos;
			
/* 		    Delete the characters from the mark
 */		    
		    mark_SetLength(mark, mark_GetLength(mark)+ tsize);
		    if (mark_GetLength(mark) < 0) mark_SetLength(mark, 0);
		    mark_SetModified(mark, TRUE);
		}
	    }
	}
    }
}
