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


#include "global.h"
#include "xmailregexp.h"
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>

#ifdef vms
extern int	noshare errno;
extern int	noshare sys_nerr;
extern char	noshare *sys_errlist[];
#else
extern int	errno;
extern int	sys_nerr;
extern char	*sys_errlist[];
#endif

extern	char	otherBuf[BUFSIZ];

/*
** @(#)Autograph() - Add user's Sign or sign autograph to outgoing mail
**                   Then make button insensitive, to prevent multiple calls.
**                   If requested autograph signature does not exist, tell user.
*/
/* ARGSUSED */
XtCallbackProc
Autograph(w, C, call_data)
Widget	w;
caddr_t	C;
caddr_t	call_data;		/* unused */
{
 FILE		*fp;
 struct stat	st_buf;
 char		tmp[BUFSIZ];
 char		*autograph, *getenv();
 int		n;


 (void) strcpy(tmp, "Sign");	/* Default action is to use Sign autograph */
 if (*C == 'a') tmp[0] = 's';
 autograph = GetMailEnv(tmp);	/* First, see if an autograph exists */

 if (! autograph) {
    /*
    ** If no Sign or sign, see if the user has created a .signature file
    */
    st_buf.st_size = 0;
    (void) sprintf(tmp, "%s/.Signature", getenv("HOME"));
    if (*C == 'a')
       (void) sprintf(tmp, "%s/.signature", getenv("HOME"));
    if (stat(tmp, &st_buf) != -1 &&
        st_buf.st_size < BUFSIZ &&
       (fp = fopen(tmp, "r")) != NULL) {
       autograph = XtMalloc((unsigned) st_buf.st_size + 1);
       n = fread(autograph, sizeof(char), (int) st_buf.st_size, fp);
       autograph[n] = '\0';
       (void) fclose(fp);
      } else {
       (void) strcpy(tmp, "Cannot find a 'Sign'ature to append\n");
       if (*C == 'a') tmp[15] = 's';
       if (st_buf.st_size) {
          (void) sprintf(tmp,"Cannot append your .Signature (exceeds %d characters)\n",
                 BUFSIZ - 1);
          if (*C == 'a') tmp[20] = 's';
         }
       Bell(tmp);
       return;
      }
   }

 XtSetSensitive(w, False);	/* Don't let us be pressed more than once */
 if (*C == 'A')			/* also make other sign button inoperative */
    XtSetSensitive(XtNameToWidget(XtParent(w), "autograph"), False);
 else				/* if this was a request for lowercase sign */
    XtSetSensitive(XtNameToWidget(XtParent(w), "Autograph"), False);

 for (n = 0; n < BUFSIZ - 1 && *autograph; autograph++)
     if (*autograph == '\\' && *(autograph + 1) == 'n') {
        tmp[n++] = '\n';	/* Replace newline strings with a character */
        autograph++;
       } else tmp[n++] = *autograph;

 if (tmp[n - 1] != '\n')	/* make sure msg ends with a newline */
    tmp[n++] = '\n';
 tmp[n] = '\0';

 if ((fp = fopen(tmpName, "a")) != NULL) {
    (void) fwrite(tmp, sizeof(* tmp), strlen(tmp), fp);
    (void) fclose(fp);
   }
 XtFree(autograph);
} /* Autograph */


/*
** @(#)endDelivery() - catch the signal when the delivery child finishes
*/
int
endDelivery(signum)
int	signum;
{
 int	status;


 if (signum != SIGCHLD)
    return 1;

 if (mailpid == wait(&status))
    return 1;				/* in case mail child was what died */

 (void) signal(SIGCHLD, SIG_DFL);	/* turn off the endDelivery hook */

 return 1;
} /* endDelivery */


/*
** @(#)Done() - Send composed message - if closure data says "Deliver"
*/
/* ARGSUSED */
XtCallbackProc
Done(w, closure, call_data)
Widget w;
caddr_t closure;
caddr_t call_data;
{
 Arg		args[1];
 FILE		*fp, *xf, *popen();
 String		m, p, q, record, folder;
 Widget		shell;
 char		*getenv(), *strchr();
 char		From[BUFSIZ], Copy[BUFSIZ], tmp[BUFSIZ], addressees[BUFSIZ];
 char		To[BUFSIZ], Subject[BUFSIZ], Cc[BUFSIZ], Bcc[BUFSIZ];
 long		clock;
 int		n, len;
#ifdef X_FACE
 int		faces_sendmail = 0;	/* Are we using faces.sendmail script */
#endif
 struct stat	st_buf;


 Bell(Default_Status_Info);

 shell = XtNameToWidget(toplevel, "topBox.commandPanel.Send.popup");
 if (! shell) return;			/* SOMEthing would be VERY wrong here */

 XtPopdown(shell);			/* remove from use but don't destroy */
/*
** Restore the sensitivity of the Autograph, Send, and reply buttons
*/
 XtSetSensitive(XtNameToWidget(XtParent(w), "Autograph"), True);
 XtSetSensitive(XtNameToWidget(XtParent(w), "autograph"), True);
 XtSetSensitive(XtNameToWidget(toplevel, "topBox.commandPanel.Send"), True);
 XtSetSensitive(XtNameToWidget(toplevel, "topBox.commandPanel.reply"), True);

 st_buf.st_size = 0;			/* (in case msg file does not exist) */
 (void) stat(tmpName, &st_buf);

 if (strcmp(closure, "Deliver") != 0) {
    n = st_buf.st_size;		/* the number of bytes in the message text */
    if (n == 0 && *closure == 'c')
       Bell("No text to save in your dead letter file\n");
    else {
       if (n && (*closure == 'c' || ! Confirm("REALLY discard this text"))) {
          if ((record = GetMailEnv("DEAD")) == NULL)
             (void) sprintf(Copy, "%s/dead.letter", getenv("HOME"));
          else {
             if (*record != '~')
                (void) strcpy(Copy, record);	/* take whatever is given */
             else {
                if (*(record+1) == '/')
                   (void) sprintf(Copy, "%s/%s", getenv("HOME"), &record[1]);
                else {
                   struct passwd *pw;

                   for (len = 0, n = 1; record[n] && record[n] != '/';)
                       tmp[len++] = record[n++];
                   tmp[len] = '\0';

                   if (pw = getpwnam(tmp))
                      (void) sprintf(Copy, "%s%s", pw->pw_dir, &record[n]);
                   else (void) strcpy(Copy, record);
                  }
               }
             XtFree(record);
            }
          st_buf.st_size = -1;		/* see if our target exists */
          (void) stat(Copy, &st_buf);
          if (fp = fopen(Copy, "a")) {
             xf = fopen(tmpName, "r");
             while (fgets(tmp, BUFSIZ, xf) != NULL) {
                   if (strncmp(tmp, "From ", 5) == 0) (void) fprintf(fp, ">");
                   (void) fprintf(fp, "%s", tmp);
                  }
             (void) fclose(xf);
             if (*tmp != '\n')
                (void) fprintf(fp, "\n");
             (void) fclose(fp);

             (void) sprintf(tmp, "\"%s\" [%s] (%d bytes)\n", Copy,
                 (st_buf.st_size >= 0) ? "Appended" : "New file", n);
             Bell(tmp);
            } else {
             (void) sprintf(tmp, "Could not write file %s - %s\n", Copy,
                    (errno < sys_nerr)? sys_errlist[errno] : "Unknown error");
             Bell(tmp); 
            }
         }
      }					/* end - if some message text exists */
    (void) unlink(tmpName);		/* remove the message text file */
   } else {				/* Deliver this message text */
    /*
    ** Call the alias() routine recursively, to de-alias the user's To:, Cc:,
    ** and Bcc: fields.
    */
    To[0] = Subject[0] = Cc[0] = Bcc[0] = otherBuf[0] = '\0';
    /*
    ** Retrieve current values from the header buffers
    */
    XtSetArg(args[0], XtNstring, &m);
    XtGetValues(XtNameToWidget(shell, "*SubjCc*To"), args, 1);
    if (*m) {
       (void) strcpy(addressees, m);
       /*
       ** alias() adds filenames to a list and returns just user addresses
       ** (it also automatically wraps long header lines)
       */
       for (p = alias(addressees); strcmp(p, addressees); p = alias(addressees))
           (void) strcpy(addressees, p);
       (void) strcpy(To, addressees);
      }
    
    XtSetArg(args[0], XtNstring, &m);
    XtGetValues(XtNameToWidget(shell, "*SubjCc*Subject"), args, 1);

    if (! *addressees || (! *m && st_buf.st_size == 0)) {
       if (! *addressees)
          Bell("No recipient specified\n"); 
       else
          Bell("No subject and no message body\n"); 
      } else {			/* if there is a message to be delivered... */
       (void) strcpy(Subject, m);
    
       XtSetArg(args[0], XtNstring, &m);
       XtGetValues(XtNameToWidget(shell, "*SubjCc*Cc"), args, 1);
       if (*m) {
          (void) strcpy(tmp, m);
          for (p = alias(tmp); strcmp(p, tmp); p = alias(tmp))
              (void) strcpy(tmp, p);

          if (*p) {
             (void) strcat(addressees, ", ");
             (void) strcat(addressees, p);
             (void) strcpy(Cc, p);
            }
         }

       XtSetArg(args[0], XtNstring, &m);
       XtGetValues(XtNameToWidget(shell, "*SubjCc*Bcc"), args, 1);
    
       if (*m) {
          (void) strcpy(tmp, m);
          for (p = alias(tmp); strcmp(p, tmp); p = alias(tmp))
              (void) strcpy(tmp, p);

          if (*p) {
             (void) strcat(addressees, ", ");
             (void) strcat(addressees, p);
             (void) strcpy(Bcc, p);
            }
         }
       /*
       ** To avoid a bug in Sun's sendmail (for occasionally not being able to
       ** find the name of the real author of the message), strip commas from
       ** the address list and pass the recipient names to sendmail on the
       ** command line, ala Mail.  Also, strip all but the actual address from
       ** any compound addresses present in the list.
       */
       for (q = addressees; *q; q++) {
           if (*q == ',') {		/* remove any commas */
              if (q[1] == ' ')		/* if comma is followed by a space */
                 bcopy(&q[1], q, strlen(q));		/* then shift left */
              else *q = ' ';		/* else substitute space for comma */
              q++;
             }
           if (*q == '\n') {		/* remove the newline tab from this */
              bcopy(&q[2], q, strlen(q) - 1);
             }
           if (p = strchr(q, ',')) {	/* find the end of next alias in list */
              n = (int) (p - q);
              bzero(tmp, n + 1);
              bcopy(q, tmp, n);		/* shorten our search to just this */
              if ((m = strchr(tmp, '<')) || (m = strchr(tmp, '('))) {
                 if (*m == '<') {	/* if its a chevroned address type */
                    if (! sscanf(tmp, "%*[^<]<%[^>]>", Copy))
                         (void) sscanf(tmp, "<%[^>]>", Copy);
                    bcopy(Copy, q, strlen(Copy));	/* stuff address only */
                    m = q + strlen(Copy);
                    bcopy(p, m, strlen(p) + 1);	/* shift rest of addressees */
                    if (q = strchr(q, ',')) q--;
                   } else {		/* must be a parenthetical style */
                    for (n = 0; tmp[n] && strchr(" \n\t", tmp[n]); n++);
                    if (1 == sscanf(&tmp[n], "%*[^)]) %s", Copy)) {
                       bcopy(Copy, q, strlen(Copy));
                       m = q + strlen(Copy);
                       bcopy(p, m, strlen(p) + 1);
                       if (q = strchr(q, ',')) q--;
                      } else {
                       (void) sscanf(&tmp[n], "%s", Copy);	/* address preceeds */
                       m = q + strlen(Copy);
                       bcopy(p, m, strlen(p) + 1);
                       if (q = strchr(q, ',')) q--;
                      }
                   }
                } else q = p - 1;	/* skip to alias end if not compound */
             } else {			/* last (or only) alias in the list */
              if ((m = strchr(q, '<')) || (m = strchr(q, '('))) {
                 if (*m == '<') {
                    if (! sscanf(q, "%*[^<]<%[^>]>", Copy))
                         (void) sscanf(q, "<%[^>]>", Copy);
                    bcopy(Copy, q, strlen(Copy) + 1);
                   } else {
                    for (p = q; strchr(" \n\t", *p); p++);
                    if (1 == sscanf(p, "%*[^)]) %s", Copy)) {
                       bcopy(Copy, q, strlen(Copy) + 1);
                      } else {
                       (void) sscanf(p, "%s", Copy);
                       p[strlen(Copy) + 1] = '\0';
                      }
                   }
                }
              break;			/* no more commas to be replaced */
             }
          }

       if ((p = GetMailEnv("sendmail")) == NULL)
            p = XtNewString("/usr/lib/sendmail");
#ifdef X_FACE
       else {
          faces_sendmail = (strcmp(&p[strlen(p) - 14], "faces.sendmail") == 0);
         }
#endif
       /*
       ** The following arguments are passed to the sendmail command:
       **
       ** -oi	don't accept a dot on a line by itself as message termination
       **
       ** -om	send to "me" too, if I am a member of an alias expansion
       */
       (void) sprintf(tmp, "%s -oi -om %s\n", p, addressees);
       XtFree(p);

       p      = GetMailEnv("outfolder");
       folder = GetMailEnv("folder");
       record = GetMailEnv("record");
       m      = getenv("HOME");
       /*
       ** Fork a child process to effect the message delivery functions.
       */
       if ((n = fork()) != 0) {			/* if we are not the child */
          if (n == -1) {			/* delivery fork failed ... */
             if (errno == ENOMEM)
                Bell("Not enough core for message delivery child\n");
             else
                Bell("No more processes - no message delivery!?!\n");
            } else				/* parent sets delivery hook */
             (void) signal(SIGCHLD, endDelivery);

          XtFree(p);
          XtFree(record);
          XtFree(folder);
          return;
         }
       /*
       ** Use this child process to effect the message delivery functions.
       **
       ** First, mail the header information and message text (in temporary
       ** file) using sendmail.
       */
       if (fp = popen(tmp, "w")) {
          /*
          ** First, write the header information.
          */
          if (*To)      (void) fprintf(fp, "To: %s\n", To);
          if (*Subject) (void) fprintf(fp, "Subject: %s\n", Subject);
          if (*InReply) (void) fprintf(fp, "%s\n", InReply);
          if (*Cc)      (void) fprintf(fp, "Cc: %s\n", Cc);
          if (*Bcc)     (void) fprintf(fp, "Bcc: %s\n", Bcc);
#ifdef X_FACE
          /*
          ** Look for, and add contents of, user's ~/.face file unless
          ** user is already invoking the faces.sendmail script, which
          ** does the job itself.  In that case, don't add info here.
          ** To accomodate both cases, look for and add header if none. 
          */
          if (! faces_sendmail) {
             (void) sprintf(tmp, "%s/.face", m);
             if (xf = fopen(tmp, "r")) {
                n = 1;
                while (fgets(tmp, BUFSIZ, xf) != NULL) {
                      if (n) {
                         n = 0;
                         if (strncmp(tmp, "X-Face:", 7) != 0)
                              (void) fprintf(fp, "X-Face:");
                        }
                      (void) fprintf(fp, "%s", tmp);
                     }
                (void) fclose(xf);
               }
            }
#endif X_FACE
          (void) fprintf(fp, "\n");	/* separate the header from any text */
          /*
          ** Now write message text (if any) to the sendmail process.  ANY
          ** line which begins with the keyword ``From '' gets prepended
          ** with a greater than (>) symbol.
          */

          if (st_buf.st_size) {
             xf = fopen(tmpName, "r");
             while (fgets(tmp, BUFSIZ, xf) != NULL) {
                   if (strncmp(tmp, "From ", 5) == 0) (void) fprintf(fp, ">");
                   (void) fprintf(fp, "%s", tmp);
                  }
             (void) fclose(xf);
             if (*tmp != '\n')
                (void) fprintf(fp, "\n");	/* ensure last line is blank */
            }
          (void) pclose(fp);
         } else {
          /*
          ** We can't use the Bell routine, because we are a child process
          */
          XtWarning("Could not establish connections with sendmail daemon"); 
         }

       (void) time(&clock);
       (void) sprintf(From, "From %s %24.24s\n", getenv("USER"), ctime(&clock));
       /*
       ** If user has set ``record'' in their .mailrc, add a message copy to
       ** that file.  Try to faithfully follow the description of the
       ** ``outfolder'' variable, even if Berkeley Mail doesn't appear to do so.
       ** To whit, if ``outfolder'' is set, "locate files used to record
       ** outgoing messages (the record variable, for one) in the directory
       ** specified by the folder variable, unless the pathname is absolute."
       ** (Quoted from Mail man page description of the outfolder variable.) 
       ** Consider both absolute and relative addressing (i.e. /* and ./*)
       ** Also accomodate tilde address values ~/ and ~username/.
       */
       if (record) {
          if (*record != '+' &&
             (*record == '/' || (*record == '.' && *(record+1) == '/') ||
             p == NULL || folder == NULL)) {
             (void) strcpy(Copy, record);
            } else {
             if (*record == '~') {
                if (*(record+1) == '/')
                   (void) sprintf(Copy, "%s/%s", m, &record[1]);
                else {
                   struct passwd *pw;

                   for (len = 0, n = 1; record[n] && record[n] != '/';)
                       tmp[len++] = record[n++];
                   tmp[len] = '\0';

                   if (pw = getpwnam(tmp))
                      (void) sprintf(Copy, "%s%s", pw->pw_dir, &record[n]);
                   else (void) strcpy(Copy, record);
                  }
               } else {
                if (*folder == '/')
                   (void) sprintf(Copy, "%s/%s", folder,
                        (*record == '+') ? &record[1] : record);
                else
                   (void) sprintf(Copy, "%s/%s/%s", m, folder,
                        (*record == '+') ? &record[1] : record);
               }
            }
          XtFree(record);

          if (fp = fopen(Copy, "a")) {
             (void) fprintf(fp, From);
             if (*To)      (void) fprintf(fp, "To: %s\n", To);
             if (*Subject) (void) fprintf(fp, "Subject: %s\n", Subject);
             if (*InReply) (void) fprintf(fp, "%s\n", InReply);
             if (*Cc)      (void) fprintf(fp, "Cc: %s\n", Cc);
             if (*Bcc)     (void) fprintf(fp, "Bcc: %s\n", Bcc);
#ifdef X_FACE
             if (! faces_sendmail) {
                (void) sprintf(tmp, "%s/.face", m);
                if (xf = fopen(tmp, "r")) {
                   n = 1;
                   while (fgets(tmp, BUFSIZ, xf) != NULL) {
                         if (n) {
                            n = 0;
                            if (strncmp(tmp, "X-Face:", 7) != 0)
                                 (void) fprintf(fp, "X-Face:");
                           }
                         (void) fprintf(fp, "%s", tmp);
                        }
                   (void) fclose(xf);
                  }
               }
#endif X_FACE
             (void) fprintf(fp, "\n");
             if (st_buf.st_size) {
                xf = fopen(tmpName, "r");
                while (fgets(tmp, BUFSIZ, xf) != NULL) {
                      if (strncmp(tmp, "From ", 5) == 0) (void) fprintf(fp, ">");
                      (void) fprintf(fp, "%s", tmp);
                     }
                (void) fclose(xf);
                if (*tmp != '\n')
                   (void) fprintf(fp, "\n");
               }
             (void) fclose(fp);
            } else {
             (void) sprintf(tmp, "Could not write file %s - %s\n", Copy,
                    (errno < sys_nerr)? sys_errlist[errno] : "Unknown error");
             /*
             ** Can't use the Bell routine internally, because we are a child
             */
             XtWarning(tmp); 
            }
         }			/* end - if record variable is set */
       /*
       ** If other addresses exist, add copies to those files and or folders
       */
       if (*otherBuf) {
          for (record = otherBuf; *record;) {
              for (q = record; *q && *q != ','; q++);
              n = 0;
              if (*q == ',') {
                 *q = '\0';
                 n = 1;
                }
              if (*record != '+' && 
                 (*record == '/' || (*record == '.' && *(record+1) == '/') ||
                 p == NULL || folder == NULL)) {
                 (void) strcpy(Copy, record);
                } else {
                 if (*record == '~') {
                    if (*(record+1) == '/')
                       (void) sprintf(Copy, "%s/%s", m, &record[1]);
                    else {
                       struct passwd *pw;

                       for (len = 0, n = 1; record[n] && record[n] != '/';)
                           tmp[len++] = record[n++];
                       tmp[len] = '\0';

                       if (pw = getpwnam(tmp))
                          (void) sprintf(Copy, "%s%s", pw->pw_dir, &record[n]);
                       else (void) strcpy(Copy, record);
                      }
                   } else {
                    if (*folder == '/')
                       (void) sprintf(Copy, "%s/%s", folder,
                                     (*record == '+') ? &record[1] : record);
                    else
                       (void) sprintf(Copy, "%s/%s/%s", m, folder,
                                     (*record == '+') ? &record[1] : record);
                   }
                }
              if (fp = fopen(Copy, "a")) {
                 (void) fprintf(fp, From);
                 if (*To)      (void) fprintf(fp, "To: %s\n", To);
                 if (*Subject) (void) fprintf(fp, "Subject: %s\n", Subject);
                 if (*InReply) (void) fprintf(fp, "%s\n", InReply);
                 if (*Cc)      (void) fprintf(fp, "Cc: %s\n", Cc);
                 if (*Bcc)     (void) fprintf(fp, "Bcc: %s\n", Bcc);
#ifdef X_FACE
                 if (! faces_sendmail) {
                    (void) sprintf(tmp, "%s/.face", m);
                    if (xf = fopen(tmp, "r")) {
                       n = 1;
                       while (fgets(tmp, BUFSIZ, xf) != NULL) {
                             if (n) {
                                n = 0;
                                if (strncmp(tmp, "X-Face:", 7) != 0)
                                     (void) fprintf(fp, "X-Face:");
                               }
                             (void) fprintf(fp, "%s", tmp);
                            }
                       (void) fclose(xf);
                      }
                   }
#endif X_FACE
                 (void) fprintf(fp, "\n");
                 if (st_buf.st_size) {
                    xf = fopen(tmpName, "r");
                    while (fgets(tmp, BUFSIZ, xf) != NULL) {
                          if (strncmp(tmp, "From ", 5) == 0) (void) fprintf(fp, ">");
                          (void) fprintf(fp, "%s", tmp);
                         }
                    (void) fclose(xf);
                    if (*tmp != '\n')
                       (void) fprintf(fp, "\n");
                   }
                 (void) fclose(fp);
                } else {
                 (void) sprintf(tmp, "Could not write file %s - %s\n", Copy,
                    (errno < sys_nerr)? sys_errlist[errno] : "Unknown error");
                 XtWarning(tmp); 
                }		/* end - if fopen of otherBuf file succeeds */
              if (n) *q++ = ',';
              record = q;
             }			/* end - for each record in otherBuf */
          XtFree(p);
          XtFree(folder);
         }			/* end - if there were records in otherBuf */
      }				/* end - if we wanted to deliver something */
    (void) unlink(tmpName);	/* remove any message text */
    exit(1);			/* this is the end of delivery child process */
    /* NOTREACHED */
   }
} /* Done */


/*
** @(#)DoIt() - send command - passed via client_data argument - to mail
*/
/* ARGSUSED */
XtCallbackProc
DoIt(w, closure, call_data)
Widget		w;
caddr_t		closure;
caddr_t		call_data;
{
 int		n;
 Arg		args[1];
 String		buf;


 SetCursor(NORMAL);
 (void) sprintf(Command, "%s\n", closure);
 if (mailpid) {				/* If connections are okay,... */
    SetCursor(WATCH);			/* will be reset by parser routine */
    if ((n = match(command_pattern, Command)) != C_FILE && n != C_NEWMAIL) {
       writeMail(Command);
      } else {				/* check for commit of any changes */
       XtSetArg(args[0], XtNlabel, &buf);
       XtGetValues(XtNameToWidget(toplevel, "topBox.titleBar.title"), args, 1);

       if (strcmp(&buf[strlen(buf) - 7], "deleted") ||
           strcmp(closure, "inc") == 0 ||
           Confirm("COMMIT all changes to this folder")) {
          writeMail(Command);
         }
      }
   } else if (C_NEWMAIL != match(command_pattern, Command))
         Bell("No current mail connection\n");		/* if not 'Newmail' */
     else {
         SetCursor(WATCH);		/* will be reset by parser routine */
         if (mailargc > 2 && strcmp(mailargv[mailargc - 2], "-f") == 0) {
            mailargc -= 2;		/* throw away any folder argument */
            mailargv[mailargc] = NULL;	/* and NULL end of argument list */
           }
         callMail(mailargv);		/* restart the mail connections */
         (void) strcpy(Command, "Start"); /* Let em know we've re-started */
         UnsetNewmail(w, (caddr_t) NULL, (caddr_t) NULL);
         In_Bogus_Mail_File = False;	/* reset this so titleBar will chg */
        }
} /* DoIt */


/*
** @(#)DoPrint() - Call the PrintMsg action routine from a callback
*/
/* ARGSUSED */
XtCallbackProc
DoPrint(w, closure, call_data)
Widget	w;
caddr_t closure;
caddr_t	call_data;
{
 PrintMsg(w, NULL, NULL, NULL);
} /* DoPrint */


/*
** @(#)DoQuit() - Terminate xmail after first closing mail connection
*/
/* ARGSUSED */
XtCallbackProc
DoQuit(w, closure, call_data)
Widget w;
caddr_t closure;
caddr_t call_data;
{
 Arg		args[1];
 String		buf;


 if (mailpid) {				/* check for commit of any changes */
    XtSetArg(args[0], XtNlabel, &buf);
    XtGetValues(XtNameToWidget(toplevel, "topBox.titleBar.title"), args, 1);

    if (*closure != 'q' || strcmp(&buf[strlen(buf) - 7], "deleted") ||
        Confirm("Changes in folder.  REALLY quit")) {
       (void) sprintf(Command, "%s\n", closure);
       writeMail(Command);
      } else return;
   }

 XtFree(XMail.iconGeometry);
 XtFree(XMail.MFileName);
 XtFree(XMail.editorCommand);

 XtDestroyWidget(toplevel);

 XCloseDisplay(XtDisplay(toplevel));

 _exit(0);
} /* DoQuit */


/*
** @(#)DoSet() - send specified set request to mail and destroy current menu.
**		 To accommodate those systems where mail cannot handle the
**		 'set no' commands, convert 'set no's to unsets.
*/
/* ARGSUSED */
XtCallbackProc
DoSet(w, closure, call_data)
Widget	w;
caddr_t	closure;
caddr_t	call_data;
{
 char	*c, buf[32];


 if (! mailpid)
    Bell("No current mail connection\n");
 else {
    SetCursor(WATCH);
    c = w->core.name;
    if (strlen(c) > 5 && strcmp(&c[strlen(c) - 6], "expert") == 0)
       if (*c == 'n')
            XMail.expert = (Boolean) 0;
       else XMail.expert = (Boolean) 1;
    else {
       if (*c == 'n')
          (void) sprintf(buf, "unset %s", &c[2]);
       else
          (void) sprintf(buf, "set %s", c);

       c = QueryMail(buf);
       XtFree(c);
      }

    XtDestroyWidget(XtParent(XtParent(w)));
    SetCursor(NORMAL);
   }
} /* DoSet */


/* ARGSUSED */
/*
** @(#)DoWith() - send client_data command to mail with selected msg number
*/
XtCallbackProc
DoWith(w, client_data, call_data)
Widget	w;
caddr_t	client_data;
caddr_t	call_data;
{
 int	num = 0;


 if (! mailpid) Bell("No current mail connection\n");
 else {
    SetCursor(WATCH);
    num = SelectionNumber(*client_data == 'u');

    if (num) (void) sprintf(Command, "%s %d\n", client_data, num);
    else (void) sprintf(Command, "%s \n", client_data);

    writeMail(Command);
   }
} /* DoWith */


/* ARGSUSED */
/*
** @(#)DropIt() - callback to destroy the current folder popup list(s)
*/
XtCallbackProc
DropIt(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
 Widget	popup = XtNameToWidget(toplevel,"topBox.commandPanel.Folder.popupList");
 if (popup)
    XtDestroyWidget(popup);
 SetCursor(NORMAL);
} /* DropIt */


/*
** @(#)GetAliasName() - retrieve alias name from button label
*/
/* ARGSUSED */
XtCallbackProc
GetAliasName(w, client_data, call_data)
Widget	w;
caddr_t	client_data;			/* unused */
caddr_t	call_data;			/* unused */
{
 Arg		args[1];
 String		alias_name;
 Widget		shell;


 XtSetArg(args[0], XtNlabel, &alias_name);
 XtGetValues(w, (ArgList) args, 1);	/* get this entry's label value */

 shell = XtParent(XtParent(w));		/* aliasList<-table<-entry */
 XtPopdown(shell);

 shell = XtParent(shell);		/* (To|Cc|Bcc)<-aliasList */

 if (TextGetLastPos(shell))		/* if some alias is already in there */
    writeTo(shell, ", ", APPEND);	/* add comma separator between names */

 writeTo(shell, alias_name, APPEND);
} /* GetAliasName */


/*
** @(#)GetFolderName() - retrieve full folder name from button labels
*/
/* ARGSUSED */
XtCallbackProc
GetFolderName(w, client_data, call_data)
Widget	w;
caddr_t	client_data, call_data;
{
 Arg		args[1];
 Cardinal	k, n, x;
 String		folder_name;
 Widget		shell;
 char		tmp[BUFSIZ], buf[BUFSIZ];


 XtSetArg(args[0], XtNlabel, &folder_name);
 XtGetValues(w, (ArgList) args, 1);

 if (! call_data)				/* just a simple label name */
    (void) sprintf(buf, "File: %s", folder_name);
 else {						/* multiple stack of names */
    tmp[0] = '\0';
    shell = w;
    (void) sscanf(call_data, "%d", &n);		/* using the nesting depth */

    for (x = 1, k = (n * 2) + n - 1; k; k--) {
        shell = shell->core.parent;		/* travel up the widget tree */
        if (++x == 3) {				/* when we get to a label... */
           x = 0;
           (void) strcpy(buf, shell->core.name); /* stuff label name in */
           (void) strcat(buf, tmp);		/* front of previous labels */
           (void) strcpy(tmp, buf);		/* to build a complete path */
          }
       }
    (void) sprintf(buf, "File: +%s%s", tmp, folder_name);
   }
 writeTo(XtNameToWidget(toplevel, "topBox.commandPanel.fileWindow"),
         buf, REPLACE);
} /* GetFolderName */


/*
** @(#)ReEdit() - Call the editMail routine to re-edit a message
*/
/* ARGSUSED */
XtCallbackProc
ReEdit(w, closure, call_data)
Widget	w;
caddr_t closure;
caddr_t	call_data;
{
 Widget	Popup;


 Popup = XtNameToWidget(toplevel, "topBox.commandPanel.Send.popup");

 if (! Popup) {
    XBell(XtDisplay(toplevel), 33);
    return;				/* SOMEthing must be VERY wrong here */
   }

 XtPopdown(Popup);			/* pop down the send popup */

 editMail();				/* re-edit the message file */

 return;
} /* ReEdit */


/*
** @(#)Reply() - send a reply to the author of the selected message
**               include its text and/or copy the other recipients, if asked.
*/
/* ARGSUSED */
XtCallbackProc
Reply(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
 Widget		sb = XtNameToWidget(toplevel, "topBox.commandPanel.Send");
 struct	sigvec	ovec;
 String		p, q, r, old_From;
 String		txt, ccList, author, subject, others, date, reference, empty;
 String		Recipient, SubjBuf, CcBuf, BccBuf;
 char		*us, *getlogin();
 int		fd;
 int		erasable = 0;
 int		alwaysIgnore;
 

 (void) sigvec(SIGCHLD, NULL, &ovec);
 if (ovec.sv_handler != SIG_DFL) {
    Bell("Still delivering your previous message.  Please try again later.\n");
    return;
   }

 XtSetSensitive(sb, False);	/* Don't start more than one composition */
 XtSetSensitive(XtNameToWidget(toplevel, "topBox.commandPanel.reply"), False);

 txt = ccList = author = subject = others = date = reference = empty = old_From = "";
 if (*client_data != 's') {
    if ((fd = open(tmpName, O_WRONLY | O_CREAT, 0600)) == -1) {
       Bell("xmail: Cannot open the temp file\n");
       return;
      }

    if (p = GetMailEnv("alwaysignore")) {
       XtFree(p);
       alwaysIgnore = (strchr("SRA", *client_data)) ? 1 : 0;
      } else alwaysIgnore = 0;

    if (alwaysIgnore)		/* do we need to include a limited copy? */
       (void) sprintf(Command, "p %d", SelectionNumber(False));
    else
       (void) sprintf(Command, "P %d", SelectionNumber(False));

    txt = QueryMail(Command);

    switch (*client_data) {
       case 'S':
            (void) write(fd, "---------- Begin Forwarded Message ----------\n", 46);
            /*
            ** Any lines that begin with keyword 'From ' get a leading symbol
            */
            for (q = r = txt; *r;) {
                for (; *r && *r != '\n'; r++);
                    if (*r == '\n') {		/* For each line of text */
                       *r = '\0';		/* temporarily mark off eos, */
                       if (strncmp(q, "From ", 5) == 0)
                          (void) write(fd, ">", 1);
                       (void) write(fd, q, strlen(q));	/* this line of text, */
                       (void) write(fd, "\n", 1);	/* the ending newline */
                       *r = '\n';		/* and replace for later use */
                       q = ++r;
                      }
               }
            if (*q) {				/* should never happen, but. */
               if (strncmp(q, "From ", 5) == 0)
                  (void) write(fd, ">", 1);
               (void) write(fd, q, strlen(q));	/* write last line of text */
               (void) write(fd, "\n", 1);	/* and an ending newline. */
              }
            (void) write(fd, "----------- End Forwarded Message -----------\n", 46);
            break;
       case 'R':
       case 'A':
            if ((p = GetMailEnv("indentprefix")) != NULL)
               erasable = 1;
            else
               p = "\t";
            for (q = r = txt; *r;) {
                for (; *r && *r != '\n'; r++);
                    if (*r == '\n') {	/* For each line of insert */
                       *r = '\0';		/* temporarily mark off eos, */
                       (void) write(fd, p, strlen(p));	/* write indent prefix, */
                       (void) write(fd, q, strlen(q));	/* this line of text, */
                       (void) write(fd, "\n", 1);	/* and missing newline, */
                       *r = '\n';		/* and replace for later use */
                       q = ++r;
                      }
               }
            if (*q) {				/* should never happen, but. */
               (void) write(fd, p, strlen(p));	/* write the indent prefix, */
               (void) write(fd, q, strlen(q));	/* this last line of text, */
               (void) write(fd, "\n", 1);	/* and an ending newline. */
              }
            if (erasable)
               XtFree(p);
            break;
      } /* end - switch on client_data */

    (void) close(fd);
/*
** strip author, subject, and Carbon copy information from the selected message
*/
    if (alwaysIgnore) {		/* get full headers for data (top 100 lines) */
       XtFree(txt);
       txt = QueryMail("unset alwaysignore");
       XtFree(txt);
       (void) sprintf(Command, "top %d", SelectionNumber(False));
       txt = QueryMail(Command);
       p = QueryMail("set alwaysignore");
       XtFree(p);
      }
    for (p = txt; *p; p++) {
        if (strcmp(p, empty) == 0 || strncmp(p, "Status:", 7) == 0) break;

        if (strncmp(p, "From ", 5) == 0 ) {
           old_From = p + 5;
           for (p = old_From; *p && *p != ' ' && *p != '\n'; p++);
           if (*p) *p++ = '\0';			/* drop the date portion now */
           for (; *p && *p != '\n'; p++);	/* skip to end of this line */
          }

        else if (strncmp(p, "Return-Path:", 12) == 0) {
           reference = p + 13;			/* step past the header */
           for (p = reference; *p && *p != '\n'; p++);
           if (*p) *p = '\0';			/* drop the trailing newline */
          }

        else if (strncmp(p, "Date:", 5) == 0) {
           date = p + 6;
           for (p = date; *p && *p != '\n'; p++);
           if (*p) *p = '\0';
          }

        else if (strncmp(p, "From:", 5) == 0) {
           author = p + 6;
           for (p = author; *p && *p != '\n'; p++);
           while (*(p+1) && strchr(" \t", *(p+1))) {
                 *p = ' ';		/* change this newline to a space */
                 *(p+1) = ' ';		/* change possible tab to a space */
                 for (p++; *p && *p != '\n'; p++);
                }
           if (*p) *p = '\0';
          }

        else if (strncmp(p, "To:", 3) == 0) {
           others = p + 4;
           for (p = others; *p && *p != '\n'; p++);
           while (*(p+1) && strchr(" \t", *(p+1))) {
                 *p = ' ';		/* change this newline to a space */
                 *(p+1) = ' ';		/* change possible tab to a space */
                 for (p++; *p && *p != '\n'; p++);
                }
           if (*p) *p = '\0';
          }

        else if (strncmp(p, "Subject:", 8) == 0) {
           subject = p + 9;
           for (p = subject; *p && *p != '\n'; p++);
           while (*(p+1) && strchr(" \t", *(p+1))) {
                 *p = ' ';		/* change this newline to a space */
                 *(p+1) = ' ';		/* change possible tab to a space */
                 for (p++; *p && *p != '\n'; p++);
                }
           if (*p) *p = '\0';
          }

        else if (strncmp(p, "Cc:", 3) == 0) {
           ccList = p + 4;
           for (p = ccList; *p && *p != '\n'; p++);
           while (*(p+1) && strchr(" \t", *(p+1))) {
                 *p = ' ';		/* change this newline to a space */
                 *(p+1) = ' ';		/* change possible tab to a space */
                 for (p++; *p && *p != '\n'; p++);
                }
           if (*p) *p = '\0';
          }
        else for (; *p && *p != '\n'; p++);
       } /* end - for all of message body */

    if (*client_data != 'a' && *client_data != 'A') {
       ccList = empty;	/* If not [rR]eplyall, make sender enter any Cc: */
       others = empty;
      } else {		/* otherwise, remove ourself from the others list */
       us = getlogin();
       if (! us) {
          struct passwd *pw = getpwuid(getuid());

          if (pw)
             us = pw->pw_name;
         }
       for (p = others; *us && *p; p++) {
           if (strncmp(p, us, strlen(us)) == 0) {
              for (us = p + strlen(us); *us && *us != ',' && *us != ' ';) us++;
              for (; *us && (*us == ',' || *us == ' ');) us++;
              for (; *us;) *p++ = *us++;
              *p = '\0';
              break;
             }
           while (*p && *p != ',' && *p != ' ') p++;
           while (*p && (*p == ',' || *p == ' ')) p++;
           p--;
          }
      }
   } /* end - if client_data does not equal 's' */

 Recipient = SubjBuf = CcBuf = BccBuf = NULL;
 InReply[0] = '\0';
/*
** If message did not have a 'From:', use 'Return-Path:' for reply recipient.
** If message also did not have a 'Return-Path', use the older style 'From '.
*/
 if (*client_data != 'S' && *client_data != 's') {
    if (! *author && *reference) 
       author = reference;
    if (! *author && *old_From)
       author = old_From;
    Recipient = XtNewString(author);
/*
** If author's name consists of a compound address (i.e. name <address>,
** (Name) address, or equivalents...) strip off the real address portion
** (i.e. that portion not in parens, but possibly between chevrons).
*/
    if ((p = strchr(Recipient, '(')) || (p = strchr(Recipient, '<')))
       switch (*p) {
          case '(': q = strchr(p, ')');		/* skipping past the parens */
                    if (p == Recipient) {	/* '(Name) address' format */
                       for (q++; *q && (*q == ' ' || *q == '\t'); q++);
                       bcopy(p, Recipient, strlen(p) + 1);
                      } else {			/* 'address (Name)' format */
                       for (; (p-1) > Recipient &&
                            (*(p-1) == ' ' || *(p-1) == '\t'); p--);
                       *p = '\0';
                      }
                    break;

          case '<': q = strchr(p++, '>');
                    *q = '\0';	/* '<address> Name' or 'Name <address>' */
                    bcopy(p, Recipient, strlen(p) + 1);
                    break;
         }
/*
** If the user wishes to include all recipients of the original
** message in this message, include those others in the address.
*/
    if (*others && (*client_data == 'a' || *client_data == 'A')) {
       p = XtRealloc(Recipient, strlen(Recipient) + strlen(others) + 3);
       Recipient = p;
       if (LASTCH(Recipient) && LASTCH(Recipient) != ',')
          (void) strcat(Recipient, ", ");
       (void) strcat(Recipient, others);
       for (p = Recipient + strlen(Recipient) - 1; *p == ' ' || *p == ','; p--);
       *++p = '\0';			/* drop any trailing ", " garbage */
      }
   }

 if (*client_data != 's' && *author && *date) {
    r = (*client_data == 'S') ? "Forwarding" : "In-Reply-To";
    (void) sprintf(InReply, "%s: Mail from '%s'\n      dated: %s", r, author, date);
   }

 if (*subject) {
    SubjBuf = XtMalloc((unsigned) strlen(subject) + 5);
    if (strncmp(subject, "Re:", 3) != 0 &&
        strncmp(subject, "Re;", 3) != 0 &&
        strncmp(subject, "RE:", 3) != 0 &&
        strncmp(subject, "RE;", 3) != 0 &&
        strncmp(subject, "re:", 3) != 0 &&
        strncmp(subject, "re;", 3) != 0)
       (void) strcpy(SubjBuf, "Re: ");
    (void) strcat(SubjBuf, subject);
   }

 CcBuf = XtNewString(ccList);

 XtFree(txt);

 sendMail(sb);

 if (! Recipient)
    writeTo(XtNameToWidget(sb, "*To"), "", REPLACE);
 else {
    writeTo(XtNameToWidget(sb, "*To"), Recipient, REPLACE);
    XtFree(Recipient);
   }

 if (! SubjBuf)
    writeTo(XtNameToWidget(sb, "*Subject"), "", REPLACE);
 else {
    writeTo(XtNameToWidget(sb, "*Subject"), SubjBuf, REPLACE);
    XtFree(SubjBuf);
   }

 if (! CcBuf)
    writeTo(XtNameToWidget(sb, "*Cc"), "", REPLACE);
 else {
    writeTo(XtNameToWidget(sb, "*Cc"), CcBuf, REPLACE);
    XtFree(CcBuf);
   }

 if (! BccBuf)
    writeTo(XtNameToWidget(sb, "*Bcc"), "", REPLACE);
 else {
    writeTo(XtNameToWidget(sb, "*Bcc"), BccBuf, REPLACE);
    XtFree(BccBuf);
   }

 editMail();
} /* Reply */


/*
** @(#)Save() - (or copy) a message to specified folder or mbox
*/
/* ARGSUSED */
XtCallbackProc
Save(w, cmd, call_data)
Widget w;
caddr_t cmd;
caddr_t call_data;
{
 char		*getenv();
 Arg			args[1];
 Cardinal		num;
 String			p, q, r, FBuf;
 XawTextPosition	pos;
 Widget			fileWindow;


 if (! mailpid) Bell("No current mail connection\n");
 else {
    SetCursor(WATCH);			/* reset by parser routines */
    num = SelectionNumber(False);	/* no current message returns zero */
    if (*cmd == 'C' || *cmd == 'S' || num == 0) {
       if (num) {
          (void) sprintf(Command, "%s %d\n", cmd, num);
         } else {
          (void) sprintf(Command, "%s \n", cmd);
         }
      } else {
       fileWindow = XtNameToWidget(toplevel, "topBox.commandPanel.fileWindow");
       pos = TextGetLastPos(fileWindow);
       if (pos - StartPos > 0) {
          XtSetArg(args[0], XtNstring, &FBuf);
          XtGetValues(fileWindow, args, 1);

          if (FBuf[pos] != '\0') FBuf[pos] = '\0';
          p = XtNewString(&FBuf[StartPos]);
          (void) sprintf(Command, "%s %d %s\n", cmd, num, p);
          XtFree(p);
         } else {
          /*
          ** If no specified filename, use the mbox pointer.  We MUST include
          ** it here, because specifying the message number for the action
          ** would be interpreted as a filename, if we didn't append one.
          */
          if ((p = GetMailEnv("MBOX"))) {
               q = GetMailEnv("outfolder");
               r = GetMailEnv("folder");
             if (*p != '+' && (*p == '/' || (*p == '.' && *(p+1) == '/') ||
                  q == NULL || r == NULL)) {
                (void) sprintf(Command, "%s %d %s\n", cmd, num, p);
               } else {
                if (*r == '/')
                   (void) sprintf(Command, "%s %d %s/%s\n", cmd, num, r, (*p == '+') ? &p[1] : p);
                else
                   (void) sprintf(Command, "%s %d %s/%s/%s\n", cmd, num,
                           getenv("HOME"), r, (*p == '+') ? &p[1] : p);
               }
             XtFree(r);
             XtFree(q);
             XtFree(p);
            }
         }
      }
    writeMail(Command);
   }
} /* Save */


/*
** @(#)SetNewmail - Highlight Newmail button to attract user attention
*/
/* ARGSUSED */
XtCallbackProc
SetNewmail(w, client_data, call_data)
Widget	w;			/* unused */
caddr_t	client_data;		/* unused */
caddr_t	call_data;		/* unused */
{
 Widget	cw;

 if (! Highlighted) {
    cw = XtNameToWidget(toplevel, "topBox.commandPanel.Newmail");
    XSetWindowBackgroundPixmap(XtDisplay(toplevel), XtWindow(cw), hatch);
    XtUnmapWidget(cw);
    XtMapWidget(cw);
    Highlighted = 1;
    reset_mailbox(XtNameToWidget(toplevel, "icon.mailbox"), 0);
   }
} /* SetNewmail */


/*
** @(#)UnsetNewmail - Remove Newmail button highlighting
*/
/* ARGSUSED */
XtCallbackProc
UnsetNewmail(w, client_data, call_data)
Widget	w;			/* unused */
caddr_t	client_data;		/* unused */
caddr_t	call_data;		/* unused */
{
 Widget	cw = XtNameToWidget(toplevel, "topBox.commandPanel.Newmail");

 if (Highlighted) {
    XSetWindowBackground(XtDisplay(toplevel), XtWindow(cw), cw->core.background_pixel);
    XtUnmapWidget(cw);
    XtMapWidget(cw);
    Highlighted = 0;
    reset_mailbox(XtNameToWidget(toplevel, "icon.mailbox"), 1);
   }
} /* UnsetNewmail */
