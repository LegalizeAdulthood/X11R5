/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messaux.c,v 1.8 1991/09/19 17:15:49 rr2b Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messaux.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/messaux.c,v 1.8 1991/09/19 17:15:49 rr2b Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <sys/param.h>
#include <cui.h>
#include <fdphack.h>
#include <errprntf.h>
#include <class.h>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <bind.ih>
#include <text.ih>
#include <textv.ih>
#include <environ.ih>
#include <search.ih>
#include <style.ih>
#include <stylesht.ih>
#include <envrment.ih>
#include <message.ih>
#include <ams.ih>
#include <amsutil.ih>
#include <text822.ih>
#include <text822v.ih>
#include <captions.ih>
#include <folders.ih>
#include <sendmsg.ih>
#define AUXMODULE 1
#include <messages.eh>
#undef AUXMODULE

extern         void BSM_CheckNewPlease();
extern         void BSM_ReadMailPlease();
extern         void BSM_ShowAllPlease();
extern         void BSM_ShowHelp();
extern         void BSM_ShowNewPlease();
extern         void BSM_ShowPersonalPlease();
extern         void BSM_ShowSubscribedPlease();
extern         boolean ClearSM();
extern         void messages_DuplicateWindow();
extern         void FSearchFPlease();
extern         void FSearchRPlease();
extern         struct t822view *GetBodies();
extern         struct captions *GetCaptions();
extern         struct captions *GetCaptionsNoCreate();
extern         struct folders *GetFolders();
extern         void MessagesFocusFolders();
extern         void MessagesFoldersCommand();
extern         void MessagesSendmessageCommand();

extern  int  (*messtextv_ForwardSearchCmd)(), (*messtextv_ReverseSearchCmd)();

void
messages__PostKeyState(self, ks)
struct messages *self;
struct keystate *ks;
{
    if (!ks) return;
    if (amsutil_GetOptBit(EXP_KEYSTROKES) 
		&& (self->WhatIAm != WHATIAM_BODIES
		    || text_GetReadOnly(
			(struct text *) t822view_GetDataObject(GetBodies(self))
		))) {
	keystate_AddBefore(self->mykeys, ks);
	super_PostKeyState(self, self->mykeys);
    } else {
	super_PostKeyState(self, ks);
    }
}

void messages__FinalizeObject(c, self)
struct classheader *c;
struct messages *self;
{
    menulist_Destroy(self->mymenulist);
    keystate_Destroy(self->mykeys);
    if (self->fileintomenulist) menulist_Destroy(self->fileintomenulist);
}

void messages__PostMenus(mess, ml)
struct messages *mess;
struct menulist *ml;
{
    CheckMenuMasks(mess);
    menulist_ClearChain(mess->mymenulist);
    if (ml) menulist_ChainAfterML(mess->mymenulist, ml, ml);
    if (mess->fileintomenulist) {
	menulist_ChainAfterML(mess->mymenulist, mess->fileintomenulist, mess->fileintomenulist);
    }
    super_PostMenus(mess, mess->mymenulist);
}

struct captions *GetCaptions(self)
struct messages *self;
{
    struct captions *c = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    c = folders_GetCaptions((struct folders *) self);
	    break;
	case WHATIAM_CAPTIONS:
	    c = (struct captions *) self;
	    break;
	case WHATIAM_BODIES:
	    c = t822view_GetCaptions((struct t822view *) self);
	    break;
    }
    if (!c) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(c);
}

struct t822view *GetBodies(self)
struct messages *self;
{
    struct t822view *tv = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    tv = captions_GetBodView(folders_GetCaptions((struct folders *) self));
	    break;
	case WHATIAM_CAPTIONS:
	    tv = captions_GetBodView((struct captions *) self);
	    break;
	case WHATIAM_BODIES:
	    tv = (struct t822view *) self;
	    break;
    }
    if (!tv) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(tv);
}

void MessagesFocusFolders(self)
struct messages *self;
{
    struct folders *f = GetFolders(self);
    folders_WantInputFocus(f, f);
}

void MessagesSendmessageCommand(self, cmds)
struct messages *self;
char *cmds;
{
    struct sendmessage *sm = folders_ExposeSend(GetFolders(self));
    if (sm) {
	ams_GenericCompoundAction(ams_GetAMS(), sm, "sendmessage", cmds);
    }
}

void BSM_ShowHelp(self)
struct messages *self;
{
    if (GetCaptions(self)->FullName) {
	folders_ExplainDir(GetFolders(self), GetCaptions(self)->FullName, GetCaptions(self)->ShortName);
    } else {
	t822view_ShowHelp(GetBodies(self), NULL);
    }
}

boolean ClearSM(self)
struct captions *self;
{
    struct sendmessage *sm = folders_ExposeSend(captions_GetFolders(self));
    if (!sm) return(TRUE);
    sendmessage_Reset(sm);
    if (sendmessage_HasChanged(sm)) return(TRUE);
    return(FALSE);
}

void BSM_CheckNewPlease(self)
struct messages *self;
{
    folders_UpdateMsgs(GetFolders(self), 0, NULL, TRUE);
}

void BSM_ReadMailPlease(self)
struct messages *self;
{
    folders_ReadMail(GetFolders(self), TRUE);
}

void BSM_ShowNewPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_NEWONLY);
}

void BSM_ShowPersonalPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_MAIL_FOLDERS);
}

void BSM_ShowAllPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_ALL_FOLDERS);
}

void BSM_ShowSubscribedPlease(self)
struct messages *self;
{
    folders_Reconfigure(GetFolders(self), LIST_SUBSCRIBED);
}

void messages_DuplicateWindow(self)
struct messages *self;
{
    if (self->WhatIAm == WHATIAM_FOLDERS) {
	struct folders *f = folders_New();

	ams_InstallInNewWindow(folders_GetApplicationLayer(f), "messages-folders", "Message Folders", environ_GetProfileInt("folders.width", 600), environ_GetProfileInt("folders.height", 120), f);
    } else if (self->WhatIAm == WHATIAM_CAPTIONS) {
	struct captions *f = captions_New();

	ams_InstallInNewWindow(captions_GetApplicationLayer(f), "messages-captions", "Message Captions", environ_GetProfileInt("captions.width", 600), environ_GetProfileInt("captions.height", 120), f);
    } else if (self->WhatIAm == WHATIAM_BODIES) {
	struct t822view *f = t822view_New();
	struct text *t = text_New();

	t822view_SetDataObject(f, t);
	ams_InstallInNewWindow(t822view_GetApplicationLayer(f), "messages-bodies", "Message Bodies", environ_GetProfileInt("bodies.width", 600), environ_GetProfileInt("bodies.height", 120), f);
    }
	
}

struct captions *GetCaptionsNoCreate(self)
struct messages *self;
{
    struct captions *c = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    c = ((struct folders *) self)->mycaps;
	    break;
	case WHATIAM_CAPTIONS:
	    c = (struct captions *) self;
	    break;
	case WHATIAM_BODIES:
	    c = ((struct t822view *) self)->mycaps;
	    break;
    }
    return(c);
}

struct folders *GetFolders(self)
struct messages *self;
{
    struct folders *f = NULL;
    switch(self->WhatIAm) {
	case WHATIAM_FOLDERS:
	    f = (struct folders *) self;
	    break;
	case WHATIAM_CAPTIONS:
	    f = captions_GetFolders((struct captions *) self);
	    break;
	case WHATIAM_BODIES:
	    f = captions_GetFolders(t822view_GetCaptions((struct t822view *) self));
	    break;
    }
    if (!f) {
	ams_ReportError(ams_GetAMS(), "Out of memory; a core dump is imminent.", ERR_FATAL, FALSE, 0);
    }
    return(f);
}

void MessagesFoldersCommand(self, cmds)
struct messages *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), GetFolders(self), "folders", cmds);
}

void FSearchFPlease(self)
struct messages *self;
{
    messtextv_ForwardSearchCmd((struct textview *) GetFolders(self));
    messages_WantInputFocus(self, self);
}

void FSearchRPlease(self)
struct messages *self;
{
    messtextv_ReverseSearchCmd((struct textview *) GetFolders(self));
    messages_WantInputFocus(self, self);
}

/* The following routine tries to make a text object "prettier" by transforming x_\010 into and underlined x, and by extracting literal ATK data streams (e.g. from rejected mail) */

void text_CleanUpGlitches(self)
struct text *self;
{
    struct style *uss;
    struct SearchPattern *Pattern = NULL;
    int loc = 0, tmploc=0;
    char *tp, c1, c2;

    /* CLEANUP PART ONE:  DEAL WITH _\010 -type underlining */
    uss = stylesheet_Find(self->styleSheet, "underline");
    tp = search_CompilePattern("[_\010]", &Pattern);
    if (tp) {
	message_DisplayString(NULL, 10, tp);
    } else while (tmploc >= 0) {
	tmploc = search_MatchPattern(self, loc, Pattern);
	if (tmploc >= 0) {
	    c1 = text_GetChar(self, tmploc);
	    c2 = text_GetChar(self, tmploc+1);
	    if (c1 == '_') {
		if (c2 == '\010') {
		    /* highlight next char */
		    environment_WrapStyle(self->rootEnvironment, tmploc+2, 1, uss);
		    text_AlwaysDeleteCharacters(self, tmploc, 2);
		} else {
		    /* Handle multiple _ here */
		    int underscores = 1, erases = 0, pos = tmploc+1;
		    while(text_GetChar(self, pos) == '_') {
			++underscores;
			++pos;
		    }
		    while (text_GetChar(self, pos) == '\010') {
			++erases;
			++pos;
		    }
		    if (underscores == erases) {
			text_AlwaysDeleteCharacters(self, tmploc, erases+underscores);
			environment_WrapStyle(self-> rootEnvironment, tmploc, erases, uss);
		    }
		}
	    } else if (c1 == '\010') {
		if (c2 == '_') {
		    /* highlight previous char */
		    environment_WrapStyle(self->rootEnvironment, tmploc-1, 1, uss);
		    text_AlwaysDeleteCharacters(self, tmploc, 2);
		} else {
		    /* Handle multiple \010 here */
		    int underscores = 0, erases = 1, pos = tmploc+1;
		    while (text_GetChar(self, pos) == '\010') {
			++erases;
			++pos;
		    }
		    while(text_GetChar(self, pos) == '_') {
			++underscores;
			++pos;
		    }
		    if (underscores == erases) {
			text_AlwaysDeleteCharacters(self, tmploc, erases+underscores);
			environment_WrapStyle(self->rootEnvironment, tmploc-erases, erases, uss);
		    }
		}
	    }
	}
	loc = tmploc + 1;
    }
    /* CLEANUP PART TWO:  DEAL WITH EMBEDDED ATK DATASTREAM, E.G. IN MAIL REJECTIONS */
    loc = tmploc = 0;
    tp = search_CompilePattern("\\begindata", &Pattern);
    if (tp) {
	message_DisplayString(NULL, 10, tp);
    } else do {
	tmploc = search_MatchPattern(self, loc, Pattern);
	if (tmploc >= 0) {
	    char Buf[200];
	    int i, endloc;
	    FILE *fp;

	    --tmploc; /* I'm not sure why, maybe a bogosity with the way CompilePattern handles the backslash! */
	    strcpy(Buf, "\\end");
	    i=4; 
	    while (TRUE) {
		Buf[i] = text_GetChar(self, tmploc+2+i);
		if (Buf[i] == '\n') break;
		++i;
	    }
	    Buf[i] = NULL;
	    tp = search_CompilePattern(Buf, &Pattern);
	    if (tp) {
		message_DisplayString(NULL, 10, tp);
	    } else {
		endloc = search_MatchPattern(self, tmploc, Pattern);
		if (endloc > tmploc) {
		    /* Found it!  Now we need to turn it into ATK! */
		    endloc += strlen(Buf);
		    sprintf(Buf, "/tmp/clean.%d", getpid());
		    fp = fopen(Buf, "w");
		    if (fp) {
			/* This next loop is dumb -- is there a way to get to simpletext_Write from here? */
			for (i=tmploc; i<endloc; ++i) {
			    putc(text_GetChar(self, i), fp);
			}
			fclose(fp);
			text_AlwaysDeleteCharacters(self, tmploc, endloc-tmploc);
			text_AlwaysInsertFile(self, NULL, Buf, tmploc);
			unlink(Buf);
		    }
		}
	    }
	}
	loc = tmploc+1;
    } while (tmploc >= 0);
    /* FINALLY, MAKE SURE OUR CHANGES GET NOTICED */
    text_NotifyObservers(self, 0);   
}

