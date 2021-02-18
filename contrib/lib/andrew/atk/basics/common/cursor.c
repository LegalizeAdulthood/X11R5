/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/cursor.c,v 2.4 1991/09/12 15:57:58 bobg Exp $ */
/* $ACIS:cursor.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/cursor.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/cursor.c,v 2.4 1991/09/12 15:57:58 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <fontdesc.ih>
#include <graphic.ih>
#include <view.ih>
#include <im.ih>

#include <cursor.eh>


static char CursorTable[] = {  'a',
    'a','g','x','H','h','l','u','v','w','|',
    'J',':','C','R','M',';','/','^','s','U',
    'D','@','X','~','}' };
#define lookup(A) ((A >= 0 && A < Cursor_TABLELEN)?  CursorTable[A] :CursorTable[0])



void cursor__ChangeShape(self)
struct cursor *self; {
    printf("cursor_ChangeShape: missing method\n");
}


void cursor__SetGlyph(self,fd,ch)
struct cursor *self;
struct fontdesc *fd;
short ch;
{
     if (self->fillFont != fd || self->fillChar != ch)  {
	self->fillFont = fd;
	self->fillChar = ch;
	cursor_ChangeShape(self);
     }
}

void cursor__SetStandard(self,ch)
struct cursor *self;
short ch;
{
    short c = lookup(ch);

     if (self->fillFont != NULL || self->fillChar != c)  {
	self->fillFont = NULL;
	self->fillChar = c;
	cursor_ChangeShape(self);
    }
}

boolean cursor__InitializeObject(classID,self)
struct classheader *classID;
struct cursor *self;
{
	self->view = NULL;
	self->posted = NULL;
	self->windowim = NULL;
	self->next = NULL;
	self->processC = FALSE;
	self->changed = FALSE;
        self->fillChar = 0;
        self->fillFont = NULL;
	return TRUE;
}

struct cursor *cursor__Create(classID, view)
struct classheader *classID;
struct view *view;
{

	struct cursor *c = im_GetCursor();
	c->view = view;
	return(c);
}

void cursor__FinalizeObject(classID,self)
struct classheader *classID;
struct cursor *self;
{
	if(cursor_IsPosted(self)) im_RetractCursor(self->posted,self);
	if(cursor_IsWindowCursor(self)) im_SetWindowCursor(self->windowim,NULL);
	if(cursor_IsProcessCursor(self)) im_SetProcessCursor(NULL);
}
