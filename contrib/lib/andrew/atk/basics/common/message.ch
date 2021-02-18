/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.ch,v 2.12 1991/09/12 19:22:36 bobg Exp $ */
/* $ACIS:message.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidmessage_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/message.ch,v 2.12 1991/09/12 19:22:36 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* message.H
 * Classprocedures to interface to message handlers.
 *
 */

#define message_PROGRAMMERVERSION 2

/* Return values for completion functions. */
#ifndef msghandler_DEFINED /* Hack, hack hack... */
enum message_CompletionCode {
    message_Complete, /* The string returned is a unique completion of the user input. */
    message_CompleteValid, /* The string returned is a completion and a valid substring of another complete. */
    message_Valid, /* The string returned is a valid substring of a complete entry. Must be at least as long a user's input. */
    message_Invalid /* The string returned is the longest viable prefix of the user's input. */
};

/* Values for help procedure calls. */
enum message_HelpItem {
    message_HelpListItem, /* This call is an item to be added to the list of possible completions. */
    message_HelpGenericItem /* This call contains text with no notable features. Presumably for the user to read. */
};

enum message_Preference {
    message_NoBlock = 3,
    message_OnTop = 2, 
    message_OnBottom = 1, 
    message_Center = 0
};


#define message_MustMatch 1 /* Can only return valid completions if this flag is set. */
#define message_InitialHelp 2 /* Display help at beggining if this flag is set. */
#define message_NoInitialString 4 /* Do not use default as initial string. */
#define message_Mandatory 8 /* Indicates that question must be answered. It cannot be punted. */
#endif /* msghandler_DEFINED  */

/* All buffer size arguments in this module are as returned by sizeof(buffer).
 * That is, they include the NUL at the end of the string.
 */
package message {
    classprocedures:
        DisplayString(struct view *view, int priority, char *string) returns int;
        AskForString(struct view *view, int priority, char *prompt, char *defaultString, char *buffer, int bufferSize) returns int;
        AskForPasswd(struct view *view, int priority, char *prompt, char *defaultString, char *buffer, int bufferSize) returns int;
        AskForStringCompleted(struct view *view, int priority, char *prompt, char *defaultString, char *buffer, int bufferSize,
                              struct keymap *keystate, procedure completionProc, procedure helpProc, long functionData, int flags) returns int;
        MultipleChoiceQuestion(struct view *view, int priority, char *prompt, long defaultChoice, long *result, char **choices, char *abbrevKeys) returns int;
        CancelQuestion(struct view *view);
	Advice(struct view *view, enum message_Preference pref);
/* These next calls are only applicable during an AskFor... operation. */
        GetCurrentString(struct view *view, char *buffer, int bufferSize) returns int;
        InsertCharacters(struct view *view, int pos, char *string, int len) returns int;
        DeleteCharacters(struct view *view, int pos, int len) returns int;
        GetCursorPos(struct view *view) returns int;
        SetCursorPos(struct view *view, int pos) returns int;
        Asking(struct view *view) returns boolean;
};
