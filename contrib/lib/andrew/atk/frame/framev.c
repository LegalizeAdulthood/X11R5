/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.c,v 2.17 1991/09/12 16:21:38 bobg Exp $ */
/* $ACIS:framev.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/frame/RCS/framev.c,v 2.17 1991/09/12 16:21:38 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <im.ih>
#include <event.ih>
#include <graphic.ih> /* Prevents include nesting problems. */
#include <keymap.ih>
#include <proctbl.ih>
#include <keystate.ih>
#include <buffer.ih>
#include <mark.ih>
#include <text.ih>
#include <bind.ih>
#include <menulist.ih>
#include <message.ih> /* For message_HelpItem enum declaration. */
#include <framemsg.ih>
#include <frame.ih>
#include <style.ih>
#include <stylesht.ih>
#include <fontdesc.ih>

#include <framev.eh>

static struct keymap *frameviewKeymap;
static struct menulist *frameviewMenulist;

#define Text(self) ((struct text *) self->header.view.dataobject)

boolean frameview__InitializeObject(classID, self)
    struct classheader *classID;
    struct frameview *self;
{

    struct style *defaultStyle;

    self->messageLine = NULL;
    self->keystate = keystate_Create(self, frameviewKeymap);
    self->menulist = menulist_DuplicateML(frameviewMenulist, self);
    self->event = NULL;
    self->transientMark = NULL;
    self->amLosingInputFocus = FALSE;
    frameview_SetBorder(self, 2, 2);
    /* This next piece of code depends on textview allocating a new style for every textview's default style. That way we can change the meaning of this style for our textview only without copying the style. If the textview behavior changes, this will have to change as well. */
    if ((defaultStyle = frameview_GetDefaultStyle(self)) != NULL) {

        style_SetJustification(defaultStyle, style_LeftJustified);
    }
    return TRUE;
}

void frameview__FinalizeObject(classID, self)
    struct classheader *classID;
    struct frameview *self;
{

    keystate_Destroy(self->keystate);
    if(self->menulist) menulist_Destroy(self->menulist);
    if (self->transientMark != NULL) {
        text_RemoveMark(self->messageLine->messageText, self->transientMark);
        mark_Destroy(self->transientMark);
    }
}

struct frameview *frameview__Create(classID, messageLine)
    struct classheader *classID;
    struct framemessage *messageLine;
{

    struct frameview *temp = frameview_New();

    frameview_SetMessageLine(temp, messageLine);
    return temp;
}

void frameview__Update(self)
    struct frameview *self;
{

    int pos;

    if (frameview_GetDotPosition(self) < (pos = text_GetFence(Text(self)))) {
        frameview_SetDotPosition(self, pos);
        frameview_SetDotLength(self, 0);
    }

    super_Update(self);
}

void frameview__WantInputFocus(self, requestor)
    struct frameview *self;
    struct view *requestor;
{

    if (requestor != (struct view *) self || framemessage_Asking(self->messageLine))
        super_WantInputFocus(self, requestor);
}



void frameview__LoseInputFocus(self)
    struct frameview *self;
{
    super_LoseInputFocus(self);

    if ((! self->amLosingInputFocus) && framemessage_Asking(self->messageLine) && (!(self->messageLine->flags & message_Mandatory))) {
        self->messageLine->oldInputFocus = NULL;
        framemessage_CancelQuestion(self->messageLine);
        framemessage_DisplayString(self->messageLine, 0, "Cancelled.");
    }
    self->amLosingInputFocus = FALSE;
}

void frameview__PostMenus(self, menulist)
    struct frameview *self;
    struct menulist *menulist;
{
    if (menulist != self->menulist) {
        menulist_ClearChain(self->menulist);
        menulist_ChainAfterML(self->menulist, menulist, (long) menulist);
    }
    super_PostMenus(self, self->menulist);
}

void frameview__PostKeyState(self, keystate)
    struct frameview *self;
    struct keystate *keystate;
{

    if (self->messageLine->keystate != NULL) {
        keystate_AddBefore(self->keystate, keystate);
        keystate_AddBefore(self->messageLine->keystate, self->keystate);
        super_PostKeyState(self, self->messageLine->keystate);
    }
    else {
        keystate_AddBefore(self->keystate, keystate);
        super_PostKeyState(self, self->keystate);
    }
}

void frameview__SetMessageLine(self, messageLine)
    struct frameview *self;
    struct framemessage *messageLine;
{

    if (self->messageLine != NULL) {

        text_RemoveMark(self->messageLine->messageText, self->transientMark);
        mark_Destroy(self->transientMark);

        if (framemessage_Asking(self->messageLine))
            framemessage_CancelQuestion(self->messageLine);
    }

    self->messageLine = messageLine;
    self->transientMark = text_CreateMark(messageLine->messageText, 0, 0);
    mark_SetStyle(self->transientMark, FALSE, FALSE);
}

struct framemessage *frameview__GetMessageLine(self)
    struct frameview *self;
{

    return self->messageLine;
}

static void EraseMessage(self)
    struct frameview *self;
{

    self->event = NULL;
    if (mark_GetLength(self->transientMark) == 0) /* Be safe... */
        return;
    text_AlwaysDeleteCharacters(Text(self), mark_GetPos(self->transientMark), mark_GetLength(self->transientMark));
    frameview_WantUpdate(self, self);
}
static void CleanMessageState(self)
    struct frameview *self;
{
    if (self->event != NULL)
        event_Cancel(self->event);

    EraseMessage(self);
    keystate_SetOverride(self->keystate, NULL, 0);
}

static enum keymap_Types KeyEraseMessage(self, key, entry, rockP)
    struct frameview *self;
    long key;
    struct basicobject *entry;
    long *rockP;
{
    CleanMessageState(self);
    return keymap_Lookup(self->keystate->curMap, key, entry, rockP);
}

static void TransientMessage(self, message)
    struct frameview *self;
    char *message;
{

    int pos, len;

    pos = text_GetLength(Text(self));
    mark_SetPos(self->transientMark, pos);
    text_AlwaysInsertCharacters(Text(self), pos, message, len = strlen(message));
    keystate_SetOverride(self->keystate, KeyEraseMessage, (long) self);
    frameview_WantUpdate(self, self);
    mark_SetLength(self->transientMark, len);
    self->event = im_EnqueueEvent((procedure) EraseMessage, (char *) self, event_SECtoTU(4));
}

static void Punt(self, key)
    struct frameview *self;
    long key;
{

    if (!(self->messageLine->flags & message_Mandatory)) {
        framemessage_CancelQuestion(self->messageLine);
        framemessage_DisplayString(self->messageLine, 0, "Cancelled.");
    }
    else
        framemessage_DisplayString(self->messageLine, 0, "Question must be answered.");
}

static void CompletionMessage(self, code)
    struct frameview *self;
    enum message_CompletionCode code;
{
    switch (code) {
        case message_Invalid:
            TransientMessage(self, "  [No Match]");
            break;
        case message_Valid:
            TransientMessage(self, "  [Incomplete]");
            break;
        case message_Complete:
            TransientMessage(self, "  [Confirm]");
            break;
        case message_CompleteValid:
            TransientMessage(self, "  [Others]");
            break;
    }
}

void frameview__Return(self, key)
    struct frameview *self;
    long key;
{
    register struct framemessage *messageLine = self->messageLine;
    int startpos, endpos;

    /* give feedback that Return was pressed   -wjh */
    self->amLosingInputFocus = TRUE;
    frameview_WantInputFocus(self, NULL);
    im_ForceUpdate();		/* display text with it all highlighted */

    if (mark_GetLength(self->transientMark) != 0) CleanMessageState(self);
    if ((messageLine->flags & message_MustMatch) 
		&& (messageLine->completionProc != NULL)) {
	/* the MustMatch flag is on and there is a completion proc::
		check to see if the string entered matches */
        framemessage_GetCurrentString(messageLine, messageLine->textBuffer, 
				messageLine->maxTextSize);
        if (!((messageLine->flags & message_NoInitialString) &&
              *messageLine->textBuffer == '\0' && messageLine->hasDefault)) {
	    /* If not default... */

            enum message_CompletionCode code;

            code = (*messageLine->completionProc)(messageLine->textBuffer, 
			messageLine->completionData, messageLine->textBuffer,
			messageLine->maxTextSize);
            if ((code != message_Complete) && (code != message_CompleteValid)) {
		CompletionMessage(self, code);
		frameview_WantInputFocus(self, self);
		im_ForceUpdate();
                return;		/* user is NOT really done. */
            }
            else {
		/* copy the possibly-modified text back to the display text */
                startpos = text_GetFence(Text(self));
                endpos = text_GetLength(Text(self));

                text_AlwaysDeleteCharacters(Text(self), startpos, 
				endpos - startpos);
                text_AlwaysInsertCharacters(Text(self), startpos, 
				messageLine->textBuffer, 
				strlen(messageLine->textBuffer));
            }
        }
    }

    if (framemessage_Asking(messageLine))
        im_KeyboardExit();
}

void frameview__Complete(self, key)
    struct frameview *self;
    long key;
{
    register struct framemessage *messageLine = self->messageLine;
    enum message_CompletionCode code;
    int len, startpos, endpos;

    if (mark_GetLength(self->transientMark) != 0)
        CleanMessageState(self);

    if (messageLine->completionProc != NULL) {
        framemessage_GetCurrentString(messageLine,
            messageLine->textBuffer, messageLine->maxTextSize);

        code = (*messageLine->completionProc)(messageLine->textBuffer,
            messageLine->completionData, messageLine->textBuffer,
            messageLine->maxTextSize);

        if ((code != message_Invalid) ||
          (self->messageLine->flags & message_MustMatch)) {
	    startpos = text_GetFence(Text(self));
	    endpos = text_GetLength(Text(self));
	    text_AlwaysDeleteCharacters(Text(self), startpos, endpos - startpos);
            len = strlen(messageLine->textBuffer);
            text_AlwaysInsertCharacters(Text(self), startpos,
                messageLine->textBuffer, len);
            frameview_SetDotPosition(self, startpos + len);
        }
        CompletionMessage(self, code);
    } else if (self->keystate->next != NULL) {
        struct proctableEntry *procTableEntry;
        struct basicobject *object;
	long rock;

        if (keystate_ApplyKey(self->keystate->next, (long) ' ',
          &procTableEntry, &rock, &object) == keystate_ProcFound)
            keystate_DoProc(self->keystate->next, procTableEntry, rock, object);
    }
}

/* This function sucks. -Z- */
static long InsertSorted(doc, pos, string)
    struct text *doc;
    long pos;
    char *string;
{
    int c, c2;
    long initPos, len;
    char *tempString;

    do {
        initPos = pos;
        tempString = string;
        while (((c = text_GetChar(doc, pos)) == *tempString) && (*tempString != '\0') && (c != EOF)) {
            pos++;
            tempString++;
        }
        while ((c2 = text_GetChar(doc, pos)) != '\n' && c2 != EOF)
            pos++;
        if (c2 == '\n')
            pos++;
    } while (c != EOF && (c < *tempString));
    if (c == EOF)
        initPos = pos;

    len = strlen(string);
    text_InsertCharacters(doc, initPos, string, len);
    return initPos + len;
}

struct helpRock {
    struct text *doc;
    long insertPos;
};

static void HelpWork(helpRock, helpType, itemString, itemInfo)
struct helpRock *helpRock;
enum message_HelpItem helpType;
char *itemString;
char *itemInfo;
{
    if (helpType == message_HelpGenericItem) {
        long len;

        len = strlen(itemString);
        text_InsertCharacters(helpRock->doc,
            helpRock->insertPos, itemString, len);
        helpRock->insertPos += len;
    } else { /* helpType == message_HelpListItem */
        long pos, len;

        pos = InsertSorted(helpRock->doc, helpRock->insertPos, itemString);

        if (itemInfo != NULL) {
            text_InsertCharacters(helpRock->doc, pos++, "\t", 1);
            len = strlen(itemInfo);
            text_InsertCharacters(helpRock->doc, pos, itemInfo, len);
            pos += len;
        }

        if (text_GetChar(helpRock->doc, pos) != '\n')
            text_InsertCharacters(helpRock->doc, pos++, "\n", 1);
    }
}

void frameview__Help(self, key)
struct frameview *self;
long key;
{
    register struct framemessage *messageLine = self->messageLine;
    struct buffer *LastBuffer;

    /* For now, there is no help for frames without buffers. */

    if (mark_GetLength(self->transientMark) != 0)
        CleanMessageState(self);

    if (messageLine->helpProc != NULL &&
      frame_GetBuffer(messageLine->frame) != NULL) {
        struct buffer *helpBuffer = frame_GetHelpBuffer(messageLine->frame);

        if (helpBuffer != NULL && buffer_GetData(helpBuffer) != NULL) {
            struct text *helpDoc = (struct text *) buffer_GetData(helpBuffer);
            struct helpRock helpRock;

            text_Clear(helpDoc);

            helpRock.doc = helpDoc;
            helpRock.insertPos = 0;

            framemessage_GetCurrentString(messageLine,
              messageLine->textBuffer, messageLine->maxTextSize);

            (*messageLine->helpProc)(messageLine->textBuffer, messageLine->completionData, HelpWork, &helpRock);

	    if (helpRock.insertPos == 0) {
                register char *s;
		register int len;
                static struct style *boldStyle = NULL,
                   *ulineStyle = NULL, *fixedStyle = NULL;

                if (boldStyle == NULL) {
                    boldStyle = style_New();
                    style_AddNewFontFace(boldStyle, fontdesc_Bold);
                    ulineStyle = style_New();
                    style_AddUnderline(ulineStyle);
                    fixedStyle = style_New();
                    style_SetFontFamily(fixedStyle, "andytype");
                    style_AddNewFontFace(fixedStyle, fontdesc_Fixed);
                    style_SetFontSize(fixedStyle, style_PreviousFontSize, -2);
                    style_AddNoWrap(fixedStyle);
                }

                s = "Name               Size Sav Object   File\n";
                text_InsertCharacters(helpDoc, 0, s, strlen(s));
                text_AddStyle(helpDoc, 0, 4, ulineStyle);
                text_AddStyle(helpDoc, 19, 4, ulineStyle);
                text_AddStyle(helpDoc, 24, 3, ulineStyle);
                text_AddStyle(helpDoc, 28, 6, ulineStyle);
                text_AddStyle(helpDoc, 37, 4, ulineStyle);

                text_AddStyle(helpDoc, 0, text_GetLength(helpDoc), fixedStyle);

                if (messageLine->textBuffer[0] == '\0') {
                    s = "Possible completions:\n\n";
                    len = strlen(s);
                    text_InsertCharacters(helpDoc, helpRock.insertPos, s, len);
                    helpRock.insertPos += len;
                } else {
                    s = "Possible completions for ``";
                    len = strlen(s);
                    text_InsertCharacters(helpDoc, helpRock.insertPos, s, len);
                    helpRock.insertPos += len;

                    len = strlen(messageLine->textBuffer);
                    text_InsertCharacters(helpDoc, helpRock.insertPos,
                        messageLine->textBuffer, len);
                    helpRock.insertPos += len;

                    s = "'':\n\n";
                    len = strlen(s);
                    text_InsertCharacters(helpDoc, helpRock.insertPos, s, len);
                    helpRock.insertPos += len;
                }

                text_AddStyle(helpDoc, 0, helpRock.insertPos, boldStyle);
	    }

            LastBuffer = frame_SetHelpBuffer(messageLine->frame,
                helpBuffer, FALSE);
	    if (messageLine->realBuffer == NULL)
                messageLine->realBuffer = LastBuffer;
            text_NotifyObservers(helpDoc, 0);
        }
    } else if (self->keystate->next != NULL) {
        struct proctableEntry *procTableEntry;
        struct basicobject *object;
	long rock;

        if (keystate_ApplyKey(self->keystate->next, (long) '?',
          &procTableEntry, &rock, &object) == keystate_ProcFound)
            keystate_DoProc(self->keystate->next, procTableEntry, rock, object);
    }
}

void Kill(self, key)
    struct frameview *self;
    long key;
{

    int len = text_GetLength(Text(self));
    int pos = text_GetFence(Text(self));

    text_DeleteCharacters(Text(self), pos, len - pos);
    frameview_WantUpdate(self, self);
}

static struct bind_Description frameviewBindings[]={
    {"frameview-punt","\003",0,NULL,0,0,Punt,"^G in message line."},
    {"frameview-punt","\007",0},
    {"frameview-punt","\030\003",0},
    {"frameview-return","\r",0,NULL,0,0,frameview__Return,"<CR> in message line."},
    {"frameview-complete"," ",0,NULL,0,0,frameview__Complete,"Attempt to complete a user response."},
    {"frameview-kill","\025",0,NULL,0,0,Kill,"Kill to beginning of line."},
    {"frameview-help","?",0,NULL,0,0,frameview__Help,"Provide help on current question."},
    {NULL, NULL, 0, "Plainer"},
    {NULL, NULL, 0, "Plainest"},
    {NULL, NULL, 0, "Quit"},
    {NULL, NULL, 0, "File,Insert File"},
    {NULL, NULL, 0, "File,Add Template"},
    {NULL, NULL, 0, "Search/Spell,Forward"},
    {NULL, NULL, 0, "Search/Spell,Backward"},
    {NULL, NULL, 0, "Search/Spell,Search Again"},
    {NULL, NULL, 0, "Search/Spell,Query Replace"},
    {NULL, NULL, 0, "Search/Spell,Check Spelling"},
    {NULL, NULL, 0, "Page,Insert Pagebreak"},
    {NULL, NULL, 0, "Page,Next Page"},
    {NULL, NULL, 0, "Page,Previous Page"},
    {NULL, NULL, 0, "Page,Insert Footnote"},
    {NULL, NULL, 0, "Page,Open Footnotes"},
    {NULL, NULL, 0, "Page,Close Footnotes"},
    {NULL, NULL, 0, "Page,Table of Contents"},
    NULL
};

boolean frameview__InitializeClass(classID)
    struct classheader *classID;
{
    frameviewKeymap = keymap_New();
    frameviewMenulist = menulist_New();
    bind_BindList(frameviewBindings, frameviewKeymap, frameviewMenulist, &frameview_classinfo);
    return TRUE;
}
