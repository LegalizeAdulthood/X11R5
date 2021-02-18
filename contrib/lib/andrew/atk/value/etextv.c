/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/etextv.c,v 2.8 1991/09/30 18:26:45 susan Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/etextv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/value/RCS/etextv.c,v 2.8 1991/09/30 18:26:45 susan Exp $ ";
#endif /* lint */
#include <class.h>

static struct keymap *ssmap;
#include <keystate.ih>
#include <proctbl.ih>
#include <keymap.ih>
#include <entrtext.ih>
#include <valuev.ih>
#include <etextv.eh>
#define Text(A) ((struct entertext *)(A->header.view.dataobject))
void etextview_CancelCommand(self)
register struct etextview *self; 
{
    struct entertext *txt = Text(self);
    if(entertext_Changed(txt)){
	if(entertext_buflen(txt))
	    entertext_SetChars(txt,entertext_GetString(txt) ,entertext_buflen(txt) - 1);
	else entertext_Clear(txt);
	entertext_NotifyObservers(txt,0);
    }
}
void etextview_ReturnCommand(self)
register struct etextview *self; 
{
    struct entertext *txt = Text(self);
    etextview_SetDotPosition(self,0);
    etextview_SetDotLength(self,0);
    entertext_updatebuf(txt);
}
void etextview_ClearCommand(self)
register struct etextview *self; 
{
    struct entertext *txt = Text(self);
    entertext_Clear(txt);
    entertext_NotifyObservers(txt,0);
}

void etextview__ReceiveInputFocus(self)
register struct etextview *self; 
{
    self->header.textview.hasInputFocus = TRUE;
    self->keystate->next = NULL;
    self->header.textview.keystate->next = NULL; /* Unforgivably bogus... */
    keystate_AddBefore(self->keystate, self->header.textview.keystate); /* Slightly bogus. */
    etextview_PostKeyState(self , self->keystate);
    if(Text(self) && self->ClearOnRIF) {
	entertext_Clear(Text(self));
	entertext_NotifyObservers(Text(self),0);
    }
    else etextview_WantUpdate(self,self);
    if(self->valueview)
	valueview_Highlight(self->valueview);
}
void etextview__LoseInputFocus(self)
register struct etextview *self; 
{
    if(self->valueview)
	valueview_Dehighlight(self->valueview);
    if(Text(self) && self->ResetOnLIF) {
	etextview_CancelCommand(self);
    }
    super_LoseInputFocus(self);
}
boolean etextview__InitializeObject(classID,self)
struct classheader *classID;
struct etextview *self;
{
    self->keystate = keystate_Create(self, ssmap);
    self->ClearOnRIF = FALSE;
    self->ResetOnLIF = FALSE;
    self->valueview = NULL;
    return TRUE;
}

boolean etextview__InitializeClass(classID)
struct classheader *classID;
{    
    struct classinfo *classInfo = &etextview_classinfo;
    struct proctable_Entry *tempProc;

    ssmap = keymap_New();
    tempProc=proctable_DefineProc("etextview-return-cmd", etextview_ReturnCommand, classInfo, NULL, "Handle enter key");
    keymap_BindToKey(ssmap, "\015", tempProc, 0);
    tempProc=proctable_DefineProc("etextview-cancel-cmd", etextview_CancelCommand, classInfo, NULL, "Handle ^G");
    keymap_BindToKey(ssmap, "\007", tempProc, 0);
    tempProc=proctable_DefineProc("etextview-clear-cmd", etextview_ClearCommand, classInfo, NULL, "Handle ^U");
    keymap_BindToKey(ssmap, "\025", tempProc, 0);
    return TRUE;
}
