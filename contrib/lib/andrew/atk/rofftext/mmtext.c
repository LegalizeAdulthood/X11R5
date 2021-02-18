/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

static char *mmtext_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/rofftext/RCS/mmtext.c,v 1.4 1991/09/12 16:30:25 bobg Exp $";

/* ********************************************************************** *\
 *         Copyright AT&T Bell Laboratories - All Rights Reserved         *
 *        For full copyright information see:'andrew/config/COPYRITE.att' *
\* ********************************************************************** */

/* mmtext: link to rofftext -mm. */

#include <class.h>

#include <buffer.ih>
#include <environ.ih>
#include <mark.ih>
#include <rofftext.ih>
#include <text.ih>

#include <mmtext.eh>

#define INITIAL 100
#define INCREMENT 1000

boolean mmtext__InitializeObject(classID,self)
struct classheader *classID;
struct mmtext *self;
{
    self->nLines = 0;
    self->nAlloc = INITIAL;
    self->filename[0] = '\0';
    self->lineMark = (struct mark **)malloc(self->nAlloc*sizeof(struct mark *));
    if (self->lineMark == NULL) return FALSE;
    mmtext_SetLinePos(self, 0L, 0L);
    return TRUE;
}

long mmtext__Read(self, file, id)
struct mmtext *self;
FILE *file;
long id; {
    long tmpRetValue;
    struct rofftext *r = (struct rofftext *)self;
    struct buffer *buf = buffer_FindBufferByData(self);

    /* copy the filename that was put into rofftext via SetAttributes */
    if (r->filename != NULL) strcpy(self->filename, r->filename);
    r->inputfiles = (char **)malloc(2 * sizeof(char *));
    r->inputfiles[0] = NULL;
    r->inputfiles[1] = NULL;
    r->filename = NULL;

    r->macrofile = environ_AndrewDir("/lib/tmac/tmac.m");
    r->RoffType = FALSE;
    r->HelpMode = FALSE;

    tmpRetValue = super_Read(r, file, (long)r); 

    /* set read-only for buffer */
    if (buf != NULL) {
	struct text *text = (struct text *)buffer_GetData(buf);
	buffer_SetReadOnly(buf, TRUE);
	if (class_IsTypeByName(class_GetTypeName(text), "text")) {
	    text_SetReadOnly(text, TRUE);
	}
    }

    return tmpRetValue;

}

long mmtext__GetLinePos(self, line)
struct mmtext *self;
long line; {
    line -= 2;
    if (line > self->nLines) line = self->nLines;
    if (line < 0) line = 0;
    return mark_GetPos(self->lineMark[line]);
}

void mmtext__SetLinePos(self, line, pos)
struct mmtext *self;
long line;
long pos; {
    if (line < 0) return;
    if (line >= self->nAlloc) {
	self->nAlloc += INCREMENT;
	self->lineMark = (struct mark **)realloc(self->lineMark, self->nAlloc*sizeof(struct mark *));
    }
    if (self->lineMark == NULL) return;
    self->lineMark[line] = mmtext_CreateMark(self, pos, 0);
    self->nLines = line;
    /*  must make sure we have not skipped some */
}


void mmtext__GetFilename(self, filename)
struct mmtext *self;
char *filename; {
    strcpy(filename, self->filename);
}
