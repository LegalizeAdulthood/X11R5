/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.c,v 2.11 1991/09/12 16:38:36 bobg Exp $ */
/* $ACIS:typetext.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/typescript/RCS/typetext.c,v 2.11 1991/09/12 16:38:36 bobg Exp $";
#endif /* lint */

/* typetext.c
 * Code for typescript to ignore styles and dataobjects in read text.
 *
 */

#define GETANDTEST(C,file) ((C = getc(file)) != EOF && C != NULL)
#define TESTEND(C) (C == EOF || C == NULL)

#include <class.h>
#include <typetext.eh>
#include <style.ih>
#include <stylesht.ih>

long typetext__HandleKeyWord(self, pos, keyword, file)
struct typetext *self;
long pos;
char *keyword;
FILE *file;  {
    register long c;
    if ((strcmp(keyword, "textdsversion") == 0) || 
	(strcmp(keyword, "define") == 0) ||
	(strcmp(keyword, "template") == 0) ){
   	 	while (GETANDTEST(c,file) && c != '}') ;
    		if(TESTEND(c)) return 0;
    		while (GETANDTEST(c,file) && c != '\n');
	}
    if(strcmp(keyword, "view") == 0)
	return super_HandleKeyWord(self, pos, keyword, file);
    return 0;
}

long typetext__HandleCloseBrace(self, pos, file)
struct typetext *self;
long pos;
FILE *file;  {
    return 0;
}

long typetext__HandleBegindata(self,pos,file)
struct typetext *self;
long pos;
FILE *file;
{
return super_HandleBegindata(self,pos,file);
}

char *typetext__ViewName(self)
    struct typetext *self;
{
    return "typescript";
}

boolean typetext__InitializeObject(classID, self)
    struct classheader *classID;
    struct typetext *self;
{
    self->hashandler = FALSE;
    typetext_SetCopyAsText(self,TRUE);
    typetext_SetWriteAsText(self,TRUE);
    typetext_SetObjectInsertionFlag(self,FALSE);
    return TRUE;
}

long typetext__GetModified(self)
    struct typetext *self;
{
    return 0;
}

