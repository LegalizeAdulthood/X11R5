/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrtext.c,v 1.1 1991/09/30 18:16:13 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrtext.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/value.R4/RCS/entrtext.c,v 1.1 1991/09/30 18:16:13 susan Exp $ ";
#endif /* lint */

#include <class.h>
#include <entrtext.eh>
#include <stylesht.ih>
#include <style.ih>
#include <envrment.ih>
#include <fontdesc.ih>
#define INITIALSIZE 64
boolean entertext__InitializeObject(classID,self)
struct classheader *classID;
struct entertext *self;
{
    if((self->buf = malloc(INITIALSIZE)) == NULL)return FALSE;
    self->buflen = 0;
    self->realbuflen = INITIALSIZE;
    *(self->buf) = '\0';
    self->mod = -1;
    self->needswrap = TRUE;
    self->Style = NULL;
    return TRUE;
}
boolean entertext__FinalizeObject(classID,self)
struct classheader *classID;
struct entertext *self;
{
    free(self->buf);
    return TRUE;
}
void entertext__updatebuf(self)
struct entertext *self;
{
    long len = entertext_GetLength(self) + 1;
    self->needswrap = FALSE;
    if(self->realbuflen < len){
	self->buf = realloc(self->buf,len);
	self->realbuflen = len;
    }
    self->buflen = len;
    self->mod = entertext_GetModified(self);
    len--;
    if(len > 0) entertext_CopySubString(self,0,len,self->buf,FALSE);
    self->buf[len] = '\0';
    entertext_NotifyObservers(self,entertext_BUFCHANGEDFLAG);
    if( entertext_GetGlobalStyle(self) != NULL)
	entertext_SetGlobalStyle(self, NULL);
    self->needswrap = TRUE;
}
boolean entertext__Changed(self)
struct entertext *self;
{
    return (boolean)(self->mod != entertext_GetModified(self));
}
void entertext__SetChars(self,str,len)
struct entertext *self;
char *str;
int len;
{
    self->needswrap = FALSE;
    entertext_Clear(self);
    if(len && str && *str) entertext_InsertCharacters(self,0,str,len);
    self->needswrap = TRUE;
}
static checkstyles(self)
struct entertext *self;
{
    
    if(self->needswrap && entertext_GetGlobalStyle(self) == NULL){
	if ((self->Style = stylesheet_Find(self->header.text.styleSheet, "italic")) == NULL){
		self->Style = style_New();
		style_SetName(self->Style, "italic");
		stylesheet_Add(self->header.text.styleSheet, self->Style);
		style_AddNewFontFace(self->Style, fontdesc_Italic);
	    }
	entertext_SetGlobalStyle(self, self->Style);
    }
}
boolean entertext__InsertCharacters(self, pos, str, len)
struct entertext *self;
long pos;
char *str;
long len;  
{
    if(!super_InsertCharacters(self, pos, str, len)) return FALSE;
    checkstyles(self);
    return TRUE;
}
boolean entertext__DeleteCharacters(self, pos, len)
struct entertext *self;
long pos;
long len;  {
    if(!super_DeleteCharacters(self, pos, len)) return FALSE;
    if(entertext_GetLength(self) > 0)
	checkstyles(self);
    return TRUE;
}

char *entertext__ViewName(self)
struct entertext *self;
{
    return ("etextview");
}
