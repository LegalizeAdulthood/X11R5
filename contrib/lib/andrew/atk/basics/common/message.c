/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.c,v 2.8 1991/09/12 15:59:23 bobg Exp $ */
/* $ACIS:message.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.c,v 2.8 1991/09/12 15:59:23 bobg Exp $";
#endif /* lint */

/* message.c
 * Provides application level interface to message handler facillities.
 */

#include <class.h>
#include <msghndlr.ih>
#include <view.ih>
#include <im.ih>
#include <message.eh>

int message__DisplayString(classID, view, priority, string)
    struct classheader *classID;
    struct view *view;
    int priority;
    char *string;
{

    struct msghandler *handler;

    if (view == NULL) {
        view = (struct view *) im_GetLastUsed();
        if (view == NULL)
            return -1;
    }

    handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }
    return msghandler_DisplayString(handler, priority, string);
}

int message__AskForString(classID, view, priority, prompt, defaultString, buffer, bufferSize)
    struct classheader *classID;
    struct view *view;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
{

    struct msghandler *handler;

    if (view == NULL) {
        view = (struct view *) im_GetLastUsed();
        if (view == NULL)
            return -1;
    }

    handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }
    return msghandler_AskForString(handler, priority, prompt, defaultString, buffer, bufferSize);
}

int message__AskForPasswd(classID, view, priority, prompt, defaultString, buffer, bufferSize)
    struct classheader *classID;
    struct view *view;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
{

    struct msghandler *handler;

    if (view == NULL) {
        view = (struct view *) im_GetLastUsed();
        if (view == NULL)
            return -1;
    }

    handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }
    return msghandler_AskForPasswd(handler, priority, prompt, defaultString, buffer, bufferSize);
}

int message__AskForStringCompleted(classID, view, priority, prompt, defaultString, buffer, bufferSize, keystate, completionProc, helpProc, functionData, flags)
    struct classheader *classID;
    struct view *view;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
    struct keymap *keystate;
    procedure completionProc, helpProc;
    long functionData;
    int flags;
{

    struct msghandler *handler;

    if (view == NULL) {
        view = (struct view *) im_GetLastUsed();
        if (view == NULL)
            return -1;
    }

    handler = (struct msghandler *) view_WantHandler(view, "message");
    if (handler == NULL) {
        return -1;
    }

    return msghandler_AskForStringCompleted(handler, priority, prompt, defaultString, buffer, bufferSize, keystate, completionProc, helpProc, functionData, flags);
}

int message__MultipleChoiceQuestion(classID, view, priority, prompt, defaultChoice, result, choices, abbrevKeys)
    struct classheader *classID;
    struct view *view;
    int priority;
    char *prompt;
    long defaultChoice;
    long *result;
    char **choices;
    char *abbrevKeys;
{

    struct msghandler *handler;

    if (view == NULL) {
        view = (struct view *) im_GetLastUsed();
        if (view == NULL)
            return -1;
    }

    handler = (struct msghandler *) view_WantHandler(view, "message");
    if (handler == NULL) {
        return -1;
    }

    return msghandler_MultipleChoiceQuestion(handler, priority, prompt, defaultChoice, result, choices, abbrevKeys);
}

void message__CancelQuestion(classID, view)
    struct classheader *classID;
    struct view *view;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler != NULL) {
        msghandler_CancelQuestion(handler);
    }
}
void message__Advice(classID, view,pp)
    struct classheader *classID;
    struct view *view;
    enum message_Preference pp;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler != NULL) {
        msghandler_Advice(handler,pp);
    }
}

int message__GetCurrentString(classID, view, buffer, bufferSize)
    struct classheader *classID;
    struct view *view;
    char *buffer;
    int bufferSize;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_GetCurrentString(handler, buffer, bufferSize);
}

int message__InsertCharacters(classID, view, pos, string, len)
    struct classheader *classID;
    struct view *view;
    int pos;
    char *string;
    int len;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_InsertCharacters(handler, pos, string, len);
}

int message__DeleteCharacters(classID, view, pos, len)
    struct classheader *classID;
    struct view *view;
    int pos, len;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_DeleteCharacters(handler, pos, len);
}

int message__GetCursorPos(classID, view)
    struct classheader *classID;
    struct view *view;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_GetCursorPos(handler);
}

int message__SetCursorPos(classID, view, pos)
    struct classheader *classID;
    struct view *view;
    int pos;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_SetCursorPos(handler, pos);
}

boolean message__Asking(classID, view)
    struct classheader *classID;
    struct view *view;
{

    struct msghandler *handler = (struct msghandler *) view_WantHandler(view, "message");

    if (handler == NULL) {
        return -1;
    }

    return msghandler_Asking(handler);
}
