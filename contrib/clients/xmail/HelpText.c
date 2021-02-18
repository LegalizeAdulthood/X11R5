/*
 * xmail - X window system interface to the mail program
 *
 * Copyright 1990,1991 by National Semiconductor Corporation
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of National Semiconductor Corporation not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * NATIONAL SEMICONDUCTOR CORPORATION MAKES NO REPRESENTATIONS ABOUT THE
 * SUITABILITY OF THIS SOFTWARE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS"
 * WITHOUT EXPRESS OR IMPLIED WARRANTY.  NATIONAL SEMICONDUCTOR CORPORATION
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO
 * EVENT SHALL NATIONAL SEMICONDUCTOR CORPORATION BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Michael C. Wagnitz - National Semiconductor Corporation
 *
 */

#include	"global.h"


String	Autograph_Info[] = {
		"Append your 'Sign'ature (if any) to the message text",
		"Append your 'sign'ature (if any) to the message text",
		NULL
		};

String	Copy_Info[] = {
                "Copy the selected message to the specified folder",
                "Copy the message to a folder named for it's author",
                NULL
                };

String	Default_Status_Info = "Use <Shift><Middle-Mouse-Button> for help on any window";

String	Delete_Info[] = {
                "Delete the currently selected message",
                "Undelete the selected or most recently deleted message",
                NULL
                };

String	Deliver_Info[] = {
		"Deliver this message",
		"Re-edit this message",
		"Cancel the request but save the text in the dead letter file",
		"Abort this request and discard the text",
		NULL
		};

String	Folder_Info[] = {
		"Access the specified folder, or select from the list",
		"Copy this folder name to the fileWindow",
		"Copy the folder path to the fileWindow or view the next level",
		NULL
		};

String	Hold_Info[] = {
                "Preserve the selected message in the system folder",
                NULL
                };

String	NewMail_Info[] = {
                "Re-read your system mail folder",
                "Incorporate new mail without committing any current changes",
                "Make xmail refresh the folder list next time it's displayed",
                NULL
                };

String	Print_Info[] = {
                "Print the selected message on the system printer",
                NULL
                };

String	Quit_Info[] = {
                "Quit xmail",
                "Leave xmail without committing any current changes",
                NULL
                };

String	Read_Info[] = {
                "Read the current message",
                "Read the next message",
                "Read the previous message",
                "Read the current message, and include full mail headers",
                NULL
                };

String	Reply_Info[] = {
                "Reply to the author of the message",
                "Reply to the author of, and include the message text",
                "Include all recipients of the original message in your reply",
                "Include all recipients, and the text, of this message",
                NULL
                };

String	Save_Info[] = {
                "Save the selected message in the specified folder",
                "Save the message in a folder named for it's author",
                "Write the message to the folder, but don't include headers",
                NULL
                };

String	Send_Info[] = {
                "Compose and send a new mail message",
                "Forward a copy of the selected mail message",
                NULL
                };

String	SendMail_Info[] = {
		"Specify the name(s) of the recipient(s)",
		"Enter the subject of this message",
		"Specify the names of any carbon copy recipients",
		"Enter the names of any blind carbon copy recipients",
		NULL
		};

String Abort_Help = "\n\n\n\n\n\n\
 Pressing the \"Abort\" button cancels delivery of the\n\
 message, and deletes any text that may have been created.";

String	Bcc_Help = "\n\
 Specify any blind carbon copy recipients for this message\n\
 using the Bcc: window.  Blind carbon recipient names are\n\
 removed from the message prior to delivery.\n\n\
 Pressing the right mouse button in this window displays\n\
 a list of your aliases.  Releasing the right mouse\n\
 button while one of the alias names is highlighted\n\
 copies that name into the window, preceding it with a\n\
 comma if necessary.\n\n\
 Standard editing functions associated with the text widget\n\
 are supported, and can be altered using the application\n\
 resources.";

String Cancel_Help = "\n\n\n\n\n\
 Pressing the \"Cancel\" button causes delivery of this\n\
 message to be abandoned, but saves a copy of the message\n\
 text in your dead letter file (default $HOME/dead.letter)\n\
 without any header information.";

String	Cc_Help = "\n\n\
 Specify carbon copy recipients for this message using the\n\
 Cc: window.  Multiple recipients should be comma separated.\n\n\
 Pressing the right mouse button in this window displays a\n\
 list of your aliases.  Releasing the right mouse button\n\
 while one of the alias names is highlighted copies that\n\
 name into this window, preceded with a comma if needed.\n\n\
 Standard editing functions associated with the text widget\n\
 are supported, and can be altered using the application\n\
 resources.";

String	Copy_Help = "\n\
 Pressing this button copies the current message to the\n\
 file or folder named in the File: window, without marking\n\
 the original for removal.  (If a folder name is not\n\
 specified, it will be copied to your mbox folder.)\n\n\
 The right mouse button pops up a menu for both the copy\n\
 and Copy commands.\n\n\
 The uppercase (Copy) command copies the message to a file\n\
 whose name is derived from the author of the message,\n\
 rather than from any name currently in the File: window.\n\n\
 See also the save command help for additional information.";

String	Delete_Help = "\
 Pressing this button removes the selected message from the\n\
 current folder.\n\n\
 The right mouse button displays a menu for both the delete\n\
 and undelete commands.\n\n\
 Undelete restores the selected message (if it has been\n\
 marked 'D'eleted), otherwise the deleted message with the\n\
 smallest message number is restored.\n\n\
 Undelete may be used repeatedly to restore all deleted\n\
 messages if changes have not yet been committed.\n\n\
 See also preserve, save, folder, Newmail, and quit.";

String	Deliver_Help = "\n\n\n\n\n\
 The Deliver button will send the composed message text\n\
 to your sendmail daemon for delivery to the specified\n\
 recipient(s).";

String	File_Help = "\n\
 The File: window provides a mechanism for specifying a\n\
 file or folder name for use by other xmail commands.\n\n\
 Text entered beyond the right margin of the window will\n\
 cause a horizontal scrollbar to appear, for scrolling the\n\
 displayed text to the left or right as needed for viewing.\n\n\
 Pressing the backspace or delete key erases a previously\n\
 entered character.  Control_W (^W) erases a previous word\n\
 and Control_U (^U) the entire line.\n\n\
 See the folder and save commands for more information.";

String	Folder_Help = "\
 The left button changes the active folder to the one\n\
 specified in the File: window.  Changing folders COMMITs\n\
 any modifications to the current folder.\n\n\
 The right button brings up a menu of folders from which\n\
 to choose.  Clicking the left button on a directory\n\
 (indicated by trailing slash) while still holding down\n\
 the right, expands that directory if it has files.\n\
 Releasing the right button copies the selected name to\n\
 the File: window.\n\n\
 The current menu can be erased (to include new folders\n\
 created since the menu was last generated) by using the\n\
 Drop option, provided in the Newmail command button menu.";

String	Hold_Help = "\n\
 The Preserve command marks the selected message for\n\
 retention in the system mailbox.  Normally when a message\n\
 is 'saved' to some other folder file, it is automatically\n\
 deleted from your system folder.  'Preserve' causes mail\n\
 to not remove the original from your system mail folder.\n\n\
 Pressing the right mouse button on the Preserve command\n\
 presents a menu of three mail (and one xmail) settings\n\
 that may be toggled while currently operating in xmail.\n\n\
 Commands listed denote what state each variable will be\n\
 changed to, if the appropriate label is highlighted when\n\
 the right mouse button is released.";

String	Index_Help = "\
 The index window displays a list of mail messages in the\n\
 current folder, as well as the currently selected message.\n\n\
 Pressing the left mouse button anywhere on a line marks\n\
 the message as selected (for other commands) but does not\n\
 automatically display the message in the text window.\n\n\
 Pressing the right button selects and also automatically\n\
 displays the message.\n\
 See also the read command help for additional information.\n\n\
 The index window header list may be scrolled using either\n\
 the scrollbar and mouse or keyboard keys.  See the Text\n\
 window help for a description of the keys supported.";

String	NewMail_Help = "\
 The Newmail button will be highlighted with a hatching\n\
 pattern whenever new mail has arrived for your account.\n\n\
 Pressing the NewMail button accesses your system folder\n\
 (if mail exists) and includes any new mail delivered\n\
 since the last time your system folder was accessed.\n\
 This action also resets any button highlighting, and\n\
 COMMITS any changes that may have been made to the\n\
 current folder.  (Confirmation provided if not expert.)\n\n\
 The right mouse button provides a menu for committing\n\
 changes, incorporating new messages without committing\n\
 changes (only if currently in your system folder), or\n\
 causing xmail to redetermine the list of mail folders.";

String	Print_Help = "\n\n\n\
 Pressing this button causes the selected message to be\n\
 printed on the system printer.\n\n\
 A message will be displayed in the status window\n\
 indicating which message was printed, along with its\n\
 size in bytes.\n\n\
 See also the Index window for help on selecting a message.";

String	Quit_Help = "\n\n\
 quit - terminates xmail, committing changes made to the\n\
 current folder.\n\n\
 The right mouse button pops up a menu for the quit and\n\
 exit commands.\n\n\
 Use exit when you wish to leave xmail without committing\n\
 the current folder's changes.\n\n\
 See also the folder, Newmail, and delete commands.";

String	Read_Help = "\n\
 Pressing this button causes the selected message to be\n\
 displayed in the text (bottom) window.\n\n\
 Current message selection is indicated by caret (>) in\n\
 the index (top) window.\n\n\
 The right mouse button displays a menu for reading the\n\
 current message, reading the next or previous message,\n\
 or reading the selected message with full headers (this\n\
 overrides any suppression of normally ignored fields).\n\n\
 See also the Index window help for more information.";

String ReEdit_Help = "\n\n\n\n\n\
 Use the ReEdit selection to perform additional changes\n\
 to the current message composition.  When completed, you\n\
 will again be presented with the send completion menu.";

String	Reply_Help = "\n\n\
 reply - initiates the mechanism to create and send a mail\n\
 message to the author of a current message.\n\n\
 The right mouse button displays a menu for selecting\n\
 'reply', 'reply included', 'replyall' (which addresses all\n\
 recipients of the original mail), or 'replyall included'.\n\
 The include options automatically copy the original\n\
 message into the new one.\n\nSee also the Send command.";

String	Save_Help = "\n\
 save - copies a message to the file or folder named in\n\
 the File: window, marking the original for deletion.\n\
 If none is specified, the name in your environment MBOX\n\
 is used ($HOME/mbox by default).\n\n\
 The right button pops up a menu of related commands.\n\n\
 A written (vs. copied) message does not include mail\n\
 headers or trailing blank line.\n\n\
 Uppercase commands create a file named from the author of\n\
 the message, rather than any name in the File: window.";

String	Send_Help = "\n\
 send - initiates the mechanism to create and send a\n\
 message to the designated recipient(s).  The right mouse\n\
 button displays a menu which provides the choice of\n\
 'send'ing, or 'forwarding a message'.  Forwarding includes\n\
 the text of the current message in the new one.\n\n\
 As with the 'reply' command, a dialog box is provided to\n\
 specify the name of the recipient(s), subject, and to whom\n\
 to send copies of this message.  Pressing the 'Deliver'\n\
 button completes the operation.\n\n\
 See also the reply command help for additional information.";

String	Sign_Help = "\n\n\n\
 Pressing this Autograph button will append your .mailrc\n\
 defined 'Sign'ature (uppercase 'S') text to the current\n\
 message composition, replacing any backslash-'n' character\n\
 combinations (\\n) found in it with true newline characters.\n\
 (See the Mail(1) man page for details on defining a Sign.)\n\n\
 If your .mailrc does not contain a Sign definition, xmail\n\
 will look for a .Signature file in your home directory,\n\
 appending its contents, if any, to the current composition.";

String	sign_Help = "\n\n\n\
 Pressing this autograph button will append your .mailrc\n\
 defined 'sign'ature (lowercase 's') text to the current\n\
 message composition, replacing any backslash-'n' character\n\
 combinations (\\n) found in it with true newline characters.\n\
 (See the Mail(1) man page for details on defining a sign.)\n\n\
 If your .mailrc does not contain a sign definition, xmail\n\
 will look for a .signature file in your home directory,\n\
 appending its contents, if any, to the current composition.";

String	Status_Help = "\n\n\n\
 The Status window displays xmail program info, status, and\n\
 error messages.\n\n\
 Error messages usually cause the terminal bell to ring, to\n\
 attract the user's attention.\n\n\
 The BELL can be disabled, by either including the command\n\
 line option -nb, or by specifying the '*bellRing' resource\n\
 as False.";

String	Subject_Help = "\n\n\
 The Subject: window provides a method for specifying the\n\
 subject for this mail message.\n\n\
 Text entered beyond the right margin of the window will\n\
 cause a horizontal scrollbar to appear, for scrolling the\n\
 displayed text to the left or right as needed for viewing.\n\n\
 Pressing the backspace or delete key erases a previously\n\
 entered character, while Control_W (^W) erases a previous\n\
 word and Control_U (^U) to the beginning of the line.";

String	Text_Help = "\
 The text window displays the currently selected message.\n\
 See the Index window help on how to select a message.\n\n\
 Text and index windows may be scrolled using either the\n\
 mouse and scrollbar, or the keyboard keys in the manner\n\
 of text display programs such as \"more\" and \"less\".\n\n\
 spacebar - scroll down a page      b - scroll up a page\n\
 return   - scroll down a line      k - scroll up a line\n\
   G      - go to end of text       ' - go to top of text\n\n\
 The both windows also support keyboard access to each of\n\
 the command buttons.  See the xmail man page for details\n\
 or press the question mark key (?) within the text window.";

String	Text2_Help = "\
Keyboard key defaults that duplicate command button actions:\n\n\
 a - REPLY to all recipients    A  - REPLY to all included\n\
 c - COPY a message             C  - COPY to Author file\n\
 d - DELETE the message         u  - UNDELETE a message\n\
 f - READ with full headers     F  - FORWARD a message\n\
 m - MAIL a message             M  - REPLY to a message\n\
 i - incorporate NEW MAIL       N  - retrieve NEW MAIL\n\
 p - read PREVIOUS message      P  - PRINT a message\n\
 r - READ the message           R  - REPLY included\n\
 s - SAVE current message       S  - SAVE to author\n\
 n - read the NEXT message      w  - WRITE current message\n\
 q - QUIT, committing changes   x  - EXIT, no commits\n\
 ? - DISPLAY this help info   <LF> - REMOVE this help info";

String	Title_Help = "\n\n\n\n\
The Titlebar window displays the xmail program name and the\n\
current version number, along with the name of the current\n\
mail folder being accessed, and its message count(s).\n\n\
Pressing the left mouse button on the National Semiconductor\n\
Corporation logo in the top left corner of the title bar\n\
issues an XLib request to iconify this xmail application.";

String	To_Help = "\n\
 Use the To: window to specify the recipients for this mail\n\
 message.  Text entered beyond the right margin of the\n\
 window will cause a horizontal scrollbar to appear, for\n\
 scrolling the displayed text left or right as desired for\n\
 viewing.  Pressing the backspace or delete key erases the\n\
 previously entered character, while Control_W (^W) erases\n\
 a previous word and Control_U (^U) to the start of a line.\n\n\
 Pressing the right mouse button in this window displays a\n\
 list of your alias names.  Releasing the right mouse\n\
 button while one of the names is highlighted will copy\n\
 that name into the window, prepending it with a comma if\n\
 needed.";

/* HelpText */
