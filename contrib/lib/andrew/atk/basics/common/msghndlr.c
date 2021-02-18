/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/msghndlr.c,v 2.11 1991/09/12 15:59:27 bobg Exp $ */
/* $ACIS:msghndlr.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/msghndlr.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/msghndlr.c,v 2.11 1991/09/12 15:59:27 bobg Exp $";
#endif /* lint */

/* Complete bogosity. */

#include <class.h>
#include <msghndlr.eh>

int msghandler__DisplayString(self, priority, string)
    struct msghandler *self;
    int priority;
    char *string;
{
    return -1;
}

int msghandler__AskForString(self, priority, prompt, defaultString, buffer, bufferSize)
    struct msghandler *self;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
{
    return -1;
}

int msghandler__AskForPasswd(self, priority, prompt, defaultString, buffer, bufferSize)
    struct msghandler *self;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
{
    return msghandler_AskForString(self,priority,prompt,defaultString,buffer,bufferSize);
}

int msghandler__AskForStringCompleted(self, priority, prompt, defaultString, buffer, bufferSize, keystate, completionProc, helpProc, functionData, flags)
    struct msghandler *self;
    int priority;
    char *prompt, *defaultString, *buffer;
    int bufferSize;
    struct keystate *keystate;
    procedure completionProc, helpProc;
    long functionData;
    int flags;
{
    return -1;
}

int msghandler__MultipleChoiceQuestion(self, priority, prompt, defaultChoice, result, choices, abbrevKeys)
    struct msghandler *self;
    int priority;
    char *prompt;
    long defaultChoice;
    long *result;
    char **choices;
    char *abbrevKeys;
{
    return -1;
}

void msghandler__CancelQuestion(self)
    struct msghandler *self;
{
}

int msghandler__GetCurrentString(self, buffer, bufferSize)
    struct msghandler *self;
    char *buffer;
    int bufferSize;
{
    return -1;
}

int msghandler__InsertCharacters(self, pos, string, len)
    struct msghandler *self;
    int pos;
    char *string;
    int len;
{
    return -1;
}

int msghandler__DeleteCharacters(self, pos, len)
    struct msghandler *self;
    int pos, len;
{
	return 0;
}

int msghandler__GetCursorPos(self)
    struct msghandler *self;
{
    return -1;
}

int msghandler__SetCursorPos(self, pos)
    struct msghandler *self;
    int pos;
{
    return -1;
}

boolean msghandler__Asking(self)
    struct msghandler *self;
{
    return FALSE;
}

void msghandler__Advice( self,pp)
struct msghandler *self;
enum message_Preference pp;
{
}
