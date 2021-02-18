/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *butter_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/butter.c,v 2.4 1991/10/03 15:55:57 bobg Exp $";

#include "class.h"
#include "butter.eh"
#include "fontdesc.ih"
#include "graphic.ih"
#include "cursor.ih"

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

boolean
butter__InitializeObject(c, self)
struct classheader *c;
struct butter *self;
{
    self->text = NULL;
    self->mycursor=NULL;
    self->HitFunction = NULL;
    self->rockptr = NULL;
    self->rockint = 0;
    self->myfontdesc = NULL;
    self->myfontname = NULL;
    self->myfontsize = 12;
    self->myfonttype = fontdesc_Bold;
    return(TRUE);
}

void
butter__SetText(self, txt)
struct butter *self;
char *txt;
{
    if (self->text) free(self->text);
    self->text = malloc(1+strlen(txt));
    strcpy(self->text, txt);
    butter_NotifyObservers(self, 0);
}

void
butter__SetButtonFont(self, f)
struct butter *self;
struct fontdesc *f;
{
    self->myfontdesc = f;
    butter_NotifyObservers(self, 0);
}

void
butter__SetHitFunction(self, f)
struct butter *self;
int (*f)();
{
    self->HitFunction = f;
}

void
butter__SetRocks(self, r1, r2)
struct butter *self;
char *r1;
int r2;
{
    self->rockptr = r1;
    self->rockint = r2;
}

