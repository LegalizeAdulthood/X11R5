/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *chimpv_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/champ/RCS/chimpv.c,v 2.5 1991/09/12 16:03:15 bobg Exp $";

#include "class.h"
#include "scroll.ih"
#include "chlistv.ih"
#include "enode.ih"
#include "enodev.ih"
#include "chimp.ih"
#include "chimpv.eh"

boolean chimpview__InitializeObject(c, self)
struct classheader *c;
struct chimpview *self;
{
    struct scroll *s = scroll_New();

    self->env = enodeview_New();
    self->lv = chlistview_New();
    if (!self->lv || !s || !self->env) return(FALSE);
    enodeview_SetChimpview(self->env, self);
    scroll_SetView(s, self->lv);
    chimpview_SetUp(self, s, self->env, 50, lpair_PERCENTAGE, lpair_VERTICAL, TRUE);
    return(TRUE);
}

void chimpview__SetDataObject(self, c)
struct chimpview *self;
struct chimp *c;
{
    chlistview_SetDataObject(self->lv, c);
    enodeview_SetDataObject(self->env, c->en);
}
