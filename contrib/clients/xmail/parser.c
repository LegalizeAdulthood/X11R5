/*
 * xmail - X window system interface to the mail program
 *
 * Copyright 1989 The University of Texas at Austin
 *
 * Author:	Po Cheung
 * Date:	March 10, 1989
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation.  The University of Texas at Austin makes no 
 * representations about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
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

#define	PARSER

#include	<ctype.h>
#include	"global.h"
#include	"xmailregexp.h"

#define	INIT		register char *sp=instring; int sed=1;
#define	GETC()		(*sp++)
#define	PEEKC()		(*sp)
#define	UNGETC(c)	(--sp)
#define	RETURN(c)	return
#define	ERROR(c)	regerr(c)

#include <regexp.h>

int	regexp_indx;

static	char	MailPrompt[80];		/* overkill on size, but to be safe */

extern	void	reset_mailbox();
extern	Boolean	Waiting;

static char *regexp_error[] = {
          "","","","","","","","","","","",
 /* 11 */ "Range endpoint too large", "","","","",
 /* 16 */ "Bad number", "","","","","","","","",
 /* 25 */ "``\\digit'' out of range", "","","","","","","","","","",
 /* 36 */ "Illegal or missing delimiter", "","","","",
 /* 41 */ "No remembered search string",
 /* 42 */ "\\(\\) imbalance",
 /* 43 */ "Too many \\(",
 /* 44 */ "More than 2 numbers given in \\{\\}",
 /* 45 */ "} expected after \\",
 /* 46 */ "First number exceeds second in \\{\\}", "", "",
 /* 49 */ "[] imbalance",
 /* 50 */ "Regular expression too long",
 NULL
};


regerr(c)
int	c;
{
 (void) fprintf(stderr, "xmail: %s (item %d).\n", regexp_error[c], regexp_indx);
 exit(0);
}


/*
**  @(#)compile_pattern() - compile regular expression patterns in a table.
**  A pattern table is an array of pattern records.
**  Each pattern record consists of a regular expression,
**  and a buffer for the compiled regular expression,
*/
void
compile_pattern(patternTable)
PatternRecPtr	patternTable;
{
 PatternRecPtr	cp;
 char		*bp;
 int		i;

 for (regexp_indx=0, cp=patternTable; cp->pat||cp->buf; regexp_indx++, cp++) {
     if (! cp->pat) continue;		/* skip over grouping separations */
     i = 2 * strlen(cp->pat) + 1;
     for (bp = cp->pat; *bp && (bp = (char *)strchr(bp, '[')); bp++) i += 17;
     bp = (char *) XtMalloc((unsigned) i);
     (void) compile(cp->pat, bp, &bp[i], '\0');
     cp->buf = bp;
    }
}

/*
** @(#)match() - string against regular expressions in pattern table.
*/
int
match(patternTable, string)
PatternRecPtr 	patternTable;
char		*string;
{
 PatternRecPtr	cp;
 int		id;

 if (strcmp(string, "") == NULL) return -1;
 for (id = 0, cp = patternTable; cp->pat || cp->buf; cp++) {
     if (! cp->pat) { id++; continue; }
     if (advance(string, cp->buf))
        return (id);
    }
 return (-1);
}


/*
** @(#)parser_init()- compile command and output pattern tables.
*/
void
parser_init()
{
 (void) compile_pattern(command_pattern);
 (void) compile_pattern(output_pattern);
}


/*
** @(#)parse() - command and output and call appropriate handler
*/
void
parse(msg)
String	msg;
{
 Cardinal	j, k, msgnum;
 Widget		button, icon, iw;
 char		tmp[128], *c, *s;
 static	Boolean	first_time = True;


 j = match(command_pattern, Command);
 switch (j) {
/*
** If error on startup, mail has terminated connection.  Remove our input
** handler, close the file from our end, and indicate that no connection
** exists.  Otherwise, establish conversation requirements and display the
** current message.  If *Show_Last: resource is NOT False, show latest, if
** none are newer.  We delay setting the icon event handler until now, to
** ensure mail startup completed before being interrupted by a map event,
** for the case where we have been started iconic.
*/
    case C_START :  if (O_BELL == match(output_pattern, msg)) {
                       if (mailpid) {
                          XtRemoveInput(mailInputId);
                          (void) close(mail_fd);
                          mailpid = 0;
                          if (MailPrompt)
                             MailPrompt[0] = NULL;
                         }
		       if (strncmp(msg, "No mail for ", 12) == 0)
			  Bell("No mail in your system mailbox\n");
		       else Bell(msg);
		       (void) UpdateTitleBar("No current folder");
                       XtFree(msg);
		       *msg = '\0';
                      } else {
/*
** To prevent a race condition (?) when starting the application iconic,
** (which would cause this loop to repeat multiple times), test the value
** of the ``screen'' mail environment variable.  If 10,000 then we're done.
*/
                       j = True;
                       if (c = GetMailEnv("screen")) {
                          j = (strcmp("10000", c) != 0) ? True : False;
                          XtFree(c);
                         }
                       if (j) {			/* if not yet set to 10000... */
                          (void) strcpy(tmp, "set screen=10000 toplines=100 ");

                          /* default action for xmail is hold (ala mailtool) */

                          if (c = GetMailrc("nohold")) XtFree(c);
                          else (void) strcat(tmp, "hold");

                          c = QueryMail(tmp);
                          XtFree(c);

                          (void) strcpy(tmp, "unset crt replyall cmd");

                          c = QueryMail(tmp);
                          XtFree(c);

                          if (msgnum = file_handler()) {
                             (void) sprintf(tmp, "%d", msgnum);
                             XtFree(msg);
                             msg = QueryMail(tmp);
                            }
                          Bell("");		/* reset any worthy-ness flag */
                          Bell(Default_Status_Info);
                         }
                      }
                    if (first_time) {		/* only need to do this once */
                       Waiting = FALSE;
                       first_time = False;
                       icon = XtNameToWidget(toplevel, "icon");
                       XtAddEventHandler(icon, StructureNotifyMask, False,
                                         icon_handler, NULL);
                       if (In_Bogus_Mail_File) {
                          (void) unlink(sprintf(tmp, "%s+", tmpName));
                          XtFree(msg);
		          *msg = '\0';		/* ignore 'skipping' message */
                         }
                      }
	            break;
/*
** In response to normal or change folder commands, test output.  If an
** error, display message in status window, and ring bell unless command
** was save, Save to author, or undelete.  If our current mail message
** folder is the bogus folder, erase any text and index and note state.
** If we have text of next mail message, tell index_handler to mark its
** number, else retrieve the appropriate message text (which also marks
** its number).  If *Show_Last: resource is NOT False, file cmd shows
** latest, if none new.
*/
    case C_EXEC  :
    case C_FILE  :  j = match(output_pattern, msg);
		    switch (j) {
		       case O_BELL:
		                    if (strchr("SsCcw", Command[0]) != NULL) {
		                       if (isdigit(msg[strlen(msg) - 2]))
		                          LASTCH(msg) = '\0';
		                       else Command[0] = '\0';
		                      }
				    if (strncmp(msg, tmpName, strlen(tmpName)) == 0)
				       Bell("No mail in your system mailbox");
				    else Bell(msg);
                                    XtFree(msg);
/*
** Save our current message number, because asking about our 'file' status
** will reset mail's idea of the 'current message' count to one, regardless.
*/
                                    msgnum = SelectionNumber(False);
/*
** Now ask for 'file' status to determine if we are still caught in our 'bogus'
** mailfile, in order to generate a more meaningful title-bar status message.
*/
                                    msg = QueryMail("file");
                                    if (strncmp(&msg[1], tmpName, strlen(tmpName)) == 0) {
		                       (void) UpdateTitleBar("No current folder");
                                       iw = XtNameToWidget(toplevel, "topBox");
                                       writeTo(XtNameToWidget(iw, "indexWindow"), " ", REPLACE);
                                       writeText(" ");
                                      } else {
                                       (void) UpdateTitleBar(msg);
                                       XtFree(msg);
/*
** If not in our bogus mail folder, reset the current message
** number in Mail by again pointing at that numbered message.
*/
                                       (void) sprintf(tmp, "f %d", msgnum);
                                       msg = QueryMail(tmp);
                                       if (Command[0] != NULL)
                                          if (strchr("Ssw", Command[0]))
                                             markIndex(">S");
                                      }
                                    XtFree(msg);
                                    *msg='\0';
                                    break;
		       case O_EXEC: msgnum = index_handler(0, 0);
                                    break;
		       case O_FILE: msgnum = file_handler();
		                    c = strchr(msg, '\n');
		                    if ((int)(c - msg) < strlen(msg) - 1) {
                                       *c = '\0';	/* don't bell this */
				       Bell("");	/* clear worthy flag */
				       Bell(msg);
		                      }
                                    XtFree(msg);
                                    *msg = '\0';
                                    if (msgnum) {
                                       (void) sprintf(tmp, "%d", msgnum);
                                       msg = QueryMail(tmp);
                                      }
		                    break;
	              }
                    break;
/*
** When deleting a message, simply mark that index line with a capital 'D'.
** If un-deleting, mark that index line with a blank ' '.  If no current
** index, restore it.  If autoprinting, mail will include text of next msg.
*/
    case C_DELETE : if (O_BELL == match(output_pattern, msg)) {
                       Bell(msg);
                       XtFree(msg);
                       *msg = '\0';
                      } else {
                       msgnum = SelectionNumber(False);
                       if (Command[0] == 'd') {
                          if (msgnum) {
                             markIndex("D");
		             msgnum = index_handler(msgnum+1, 0);
                            }
                         } else {		/* if we are 'undeleting' */
		          c = QueryMail("=");
		          (void) sscanf(c, "%d", &msgnum);
		          XtFree(c);
                          iw = XtNameToWidget(toplevel, "topBox.indexWindow");
                          if (TextGetLastPos(iw) < (XawTextPosition) 4) {
                             c = QueryMail("h");
                             writeTo(iw, c, REPLACE);
		             XtFree(c);
                            }
		          msgnum = index_handler(msgnum, 1);
                         }
                       c = QueryMail("file");	/* resets current msg to 1 */
                       (void) UpdateTitleBar(c);
		       XtFree(c);

                       if (msgnum == 0)	{	/* if was last msg in file */
                          XtFree(msg);
                          msg = XtNewString("\n");	/* erase its text */
                         } else {		/* reset mail's idea of the */
                          (void) sprintf(tmp, "f %d", msgnum); /* current message */
                          c = QueryMail(tmp);
                          XtFree(c);
                         }
                       if (! *msg)		/* Don't allow memory leaks */
                          XtFree(msg);		/* to accumulate if no text */
                      }
		    break;
/*
** In response to a request to view new mail, first reset the mailbox flag.
** If mail came back with an error, complain to the user.  Otherwise, clear
** the Newmail command button highlighting. Then display any new mail text.
*/
    case C_NEWMAIL:
		    button = XtNameToWidget(toplevel, "topBox.commandPanel.Folder");
		    UnsetNewmail(button, NULL, NULL);
                    if (O_BELL == match(output_pattern, msg)) {
                       if (strncmp(msg, "No mail for ", 12) == 0) {
                          Bell("No mail in your system mailbox\n");
                         } else { Bell(msg); }
                       XtFree(msg);
                       *msg = '\0';
                      } else {
                       Bell("");		/* reset any worthy-ness flag */
                       Bell(Default_Status_Info);
                       msgnum = file_handler();
		       if (*msg) {
		          s = msg;
		          c = strchr(msg, '\n');
		          if ((c - s) + 1 < strlen(msg)) {
                             *c = '\0';	/* don't bell this */
			     Bell(msg);
                            }
                         }
                       XtFree(msg);
                       *msg = '\0';
                       if (msgnum) {
                          (void) sprintf(tmp, "%d", msgnum);
                          msg = QueryMail(tmp);
                         }
                      }
		    break;

           default: j = match(output_pattern, msg);
		    switch (j) {
                       case O_BELL:
                            Bell(msg);
                            XtFree(msg);
                            *msg = '\0';
		            break;
/*
** If output is from the print command, display a status message
*/
		       case O_PRINT:
                            (void) sscanf(&Command[2], "%d", &j);
                            c = strrchr(msg, '/');
                            (void) sscanf(c, "/%d", &k);
                            (void) sprintf(tmp,
                             "Message %d sent to printer -- %d bytes\n", j, k);
                            Bell(tmp);
                            XtFree(msg);
                            *msg = '\0';
		            break;
/*
** If we didn't specifically ask for it, and its not an error, just toss it.
** Preserve post processing moved here, in case we weren't allowed to do it.
*/
                       default:
                            if (strncmp(Command, "preserve", 8) == 0)
                               markIndex("P");

                            XtFree(msg);
                            *msg = '\0';
		            break;
                      }
		    break;
   }
/*
** If any text remains, display it in the bottom (text) window.
*/
 if (*msg) {
    for (j = strlen(msg)-1; j > 1 && msg[j] == '\n' && msg[j-1] == '\n'; j--);
    msg[++j] = '\0';			/* drop all but the last newline */
    writeText(msg);
    XtFree(msg);
    *msg = '\0';
   }
 SetCursor(NORMAL);
} /* parse */

#define	FILEBUF	4096


/*
** @(#)QueryMail() - Sends a command and returns corresponding output.
** If called by the Xt input procedure readMail, no command is included.
*/
char *
QueryMail(cmd)
char	*cmd;
{
 int		j, outputsize, size, dlen, length = 0;
 char		*temp, s[FILEBUF];
 static char	*output;


 if (*MailPrompt) length = strlen(MailPrompt);
/*
** allocate one block to 'output' to begin with
*/
 outputsize = FILEBUF;
 output = XtMalloc((unsigned) outputsize);
 output[0] = '\0';

 if (! mailpid)
    Bell("No current mail program connection\n");
 else {
    if (*cmd) {				/* allow us to be called by readMail */
       if (LASTCH(cmd) != '\n')
          (void) sprintf(s, "%s\n", cmd);
       else
          (void) strcpy(s, cmd);
       writeMail(s);
      }

    for (;;) {
        if ((size = read(mail_fd, s, FILEBUF)) < 1) {	/* EOF or an error? */
           if (! *output) {
              (void) strcpy(output, "No current mail service connection\n");
              if (mailpid) {
                 XtRemoveInput(mailInputId);
                 (void) close(mail_fd);
                 mailpid = 0;
                 if (MailPrompt)
                    MailPrompt[0] = NULL;
                }
             }
           break;
          }

        if (size < FILEBUF)
           s[size] = '\0';

        if (strlen(output) + size > outputsize) {
           outputsize += FILEBUF;
           temp = XtRealloc(output, outputsize);
           output = temp;
          }
        (void) strcat(output, s);

        /*
        ** If we have no prompt and are just starting, get the prompt string.
        ** This makes the GROSS assumption that we will have gotten it here.
        */
        if (*output && ! *MailPrompt && strcmp(Command, "Start") == 0) {
           if ((temp = strrchr(output, '\n')) != NULL)
              (void) strcpy(MailPrompt, temp + 1);
           else
              (void) strcpy(MailPrompt, output);
           length = strlen(MailPrompt);
          }

        j = strlen(output) - length;
        if (j < 0) j = 0;		/* no references before zero */
        if (*MailPrompt && strcmp(&output[j], MailPrompt) == 0) {
           output[j] = '\0';		/* Drop prompt from end of text */
           break;			/* and return from read routine */
          }

        if (O_BELL == match(output_pattern,output) && !strcmp(Command,"Start"))
           break;
       }

    if (*cmd) 
       LASTCH(output) = '\0';		/* drop newline for normal queries */
   }

 temp = XtRealloc(output, strlen(output) + 1);
 output = temp;

 return(output);
} /* QueryMail */
