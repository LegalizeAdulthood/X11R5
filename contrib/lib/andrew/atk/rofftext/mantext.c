/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *mantext_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/mantext.c,v 1.2 1991/09/12 16:30:22 bobg Exp $";


/* mantext: link to rofftext -man. */

#include <class.h>

#include <mark.ih>
#include <rofftext.ih>

#include <mantext.eh>

#define INITIAL 100
#define INCREMENT 1000

boolean mantext__InitializeObject(classID,self)
struct classheader *classID;
struct mantext *self;
{
    self->nLines = 0;
    self->nAlloc = INITIAL;
    self->filename[0] = '\0';
    self->lineMark = (struct mark **)malloc(self->nAlloc*sizeof(struct mark *));
    if (self->lineMark == NULL) return FALSE;
    mantext_SetLinePos(self, 0L, 0L);
    return TRUE;
}

long mantext__Read(self, file, id)
struct mantext *self;
FILE *file;
long id; {
    long tmpRetValue;
    struct rofftext *r = (struct rofftext *)self;

    /* copy the filename that was put into rofftext via SetAttributes */
    if (r->filename != NULL) strcpy(self->filename, r->filename);
    r->inputfiles = (char **)malloc(2 * sizeof(char *));
    r->inputfiles[0] = NULL;
    r->inputfiles[1] = NULL;
    r->filename = NULL;

    r->macrofile = "/usr/lib/tmac/tmac.an";
    r->RoffType = FALSE;
    r->HelpMode = FALSE;

    tmpRetValue = super_Read(r, file, (long)r); 

    return tmpRetValue;

}

long mantext__GetLinePos(self, line)
struct mantext *self;
long line; {
    line -= 2;
    if (line > self->nLines) line = self->nLines;
    if (line < 0) line = 0;
    return mark_GetPos(self->lineMark[line]);
}

void mantext__SetLinePos(self, line, pos)
struct mantext *self;
long line;
long pos; {
    if (line < 0) return;
    if (line >= self->nAlloc) {
	self->nAlloc += INCREMENT;
	self->lineMark = (struct mark **)realloc(self->lineMark, self->nAlloc*sizeof(struct mark *));
    }
    if (self->lineMark == NULL) return;
    self->lineMark[line] = mantext_CreateMark(self, pos, 0);
    self->nLines = line;
    /*  must make sure we have not skipped some */
}


void mantext__GetFilename(self, filename)
struct mantext *self;
char *filename; {
    strcpy(filename, self->filename);
}
