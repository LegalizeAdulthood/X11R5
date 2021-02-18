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

#ifdef mips
#include <stdlib.h>
#endif

extern	char	*mailrcFile();
char		otherBuf[BUFSIZ];
String		list = NULL;		/* list of aliases */
int		list_length;		/* size of alias list */

AliasRecPtr	*aliases = NULL;	/* array of mail alias strings	*/


alias_compare(a1, a2)
AliasRec **a1, **a2;
{
 return(strcmp((*a1)->name, (*a2)->name));
}


/*
** @(#)addtobuf() - add text to recipient or file and folder names buffer
**			drop any leading backslash from a recipient name
*/
void
addtobuf(text, buffer, current_length, otherBuffer)
char	*text;
char	*buffer;
int	current_length;
char	*otherBuffer;
{
 if (! strchr("./+", *text)) {
    if (*buffer) (void) strcat(buffer, ", ");
    if (current_length && current_length + strlen(text) > 71) {
       (void) strcat(buffer, "\n\t");	/* make it a continuation of header */
      }
    (void) strcat(buffer, (*text == '\\') ? &text[1] : text);
   } else {
    if (*otherBuffer) (void) strcat(otherBuffer, ", ");
       (void) strcat(otherBuffer, text);
   }
} /* addtobuf */


/*
** @(#) de_alias(text, buffer, current_line, otherBuffer)
*/
void
de_alias(text, buffer, current_length, otherBuffer)
char	*text;
char	*buffer;
int	current_length;
char	*otherBuffer;
{
 char	*cp, *list;
 int	in_comma;


 if (! strchr(text, ','))
    addtobuf(text, buffer, current_length, otherBuffer);
 else {
    for (list = text; *list;) {
        in_comma = 0;
        if (cp = strchr(list, ',')) {
           *cp = '\0';
           in_comma = 1;
          } else cp = list + strlen(list);
        addtobuf(list, buffer, current_length, otherBuffer);
        if (in_comma) *cp++ = ',';
        list = cp;
       }
   }
} /* de_alias */


/*
** @(#)alias() - return alias value(s) from mail, or name if no alias found
*/
char *
alias(name)
char	*name;
{
 static	char	tmp[BUFSIZ];
 static	char	buf[BUFSIZ];
 char		*line, *s, *p, *value;
 int		i, n;
 int		testing_for_compound_address;


 tmp[0] = '\0';
 if (name)
    (void) strcpy(tmp, name);
/*
** If not already done, extract the mail alias list and build the alias table.
** Provide support for the possibility of multiple names for an alias with or
** without comma separations.
*/
 if (! aliases) {
    if (mailpid) list = QueryMail("alias");
    else  if (! (list = GetMailrc("alias")))
                 list = (char *)XtCalloc(1, sizeof(char *));
/*
** count up the number of aliases in the list and allocate the list size
*/
    for (i = 1, p = list; *p; p++) if (*p == '\n') i++;
    aliases = (AliasRec **) XtMalloc((unsigned) (i + 1) * sizeof(AliasRec *));
/*
** Copy the pointers for the alias names and values into an array, marking
** the ends of each with a null and separating any multiple values with a
** comma.  Ensure there is no trailing comma in the value list.
*/
    for (n = 0, p = list; n < i && *p; n++, p++) {
        aliases[n] = (AliasRec *) XtMalloc(sizeof(AliasRec));
        for (; *p && (*p == ' ' || *p == '\t'); p++);
        for (aliases[n]->name = p; *p && *p != ' ' && *p != '\t'; p++);
        for (*p++ = '\0'; *p && strchr(" \t\"\'", *p); p++);
        testing_for_compound_address = True;
        for (aliases[n]->alias = p; *p && *p != '\n'; p++) {
/*
** if it contains a chevron or parenthesis, treat whole line as just one alias
*/
            if (testing_for_compound_address) {
               if ((s = strchr(p, '<')) || (s = strchr(p, '('))) {
                  if (line = strchr(p, '\n')) {	/* could be more than one */
                     if (s < line) {		/* if its within this alias */
                        p = line;		/* point to end of this alias */
                        if (*(p - 1) == '"' ||	/* could be true (no mailpid) */
                            *(p - 1) == '\'')
                            *(p - 1) = '\0';
                        break;
                       } else {
                        if (*s == '<' && (s = strchr(p, '(')) && s < line) {
                           p = line;		/* point to end of this alias */
                           if (*(p - 1) == '"' || /* possibly not in mail */
                               *(p - 1) == '\'')
                               *(p - 1) = '\0';
                           break;
                          } else testing_for_compound_address = False;
                       }
                    } else {			/* last entry of this record */
                     p += strlen(p);		/* point to the end of line */
                     break;
                    }
                 } else testing_for_compound_address = False;
              } /* end - if testing_for_compound_address */
            if ((*p == ' ' || *p == '\t') && *(p+1) && *(p+1) != '\n' &&
               *(p-1) != *p && *(p-1) != ',') *p = ',';
           }
        for (s = p - 1; strchr(", \t", *s); s--);
        if (strchr(", \t", *++s)) *s = '\0';
        if (*p) *p = '\0';
       }
    aliases[n] = NULL;

    qsort((char *)aliases, n, sizeof(char *), alias_compare);
   } /* end - if no alias list */
/*
** If input is made up of more than one word, check each word for aliasing.
** If it contains a chevron or parenthesis, it is a 'compound' address type.
** If line length approaches 80 characters, add a newline-tab and continue.
*/
 if (*(value = tmp)) {
    buf[0] = '\0'; line = buf;
    for (p = value; *p;) {		/* 'value' points to current 'word' */
        if (s = strchr(p, ',')) {
           *s++ = '\0';
           p = s;
          } else p = &value[strlen(value)];

        for (n = 0; aliases[n]; n++) {
            if (strcmp(value, aliases[n]->name) == 0) {
               if (line = strrchr(buf, '\t')) line++;
               else line = buf;

               de_alias(aliases[n]->alias, buf, strlen(line), otherBuf);
               break;
              }
           }

        if (! aliases[n]) {	/* If not an alias, use the name supplied. */
           if (line = strrchr(buf, '\t')) line++;
           else line = buf;

           de_alias(value, buf, strlen(line), otherBuf);
          }
        for (; *p && strchr(" \n\t", *p); p++);
        value = p;
       }
    value = buf;
   }
 return ((char *)value);
} /* alias */

/*
** @(#)GetMailEnv() - Get environment value from mail or shell
**                    Accommodate the case of trailing blanks on the item.
**		      Expand environment variables.
*/
char *
GetMailEnv(item)
char	*item;
{
 char		*mailenv, *s, *c, *value, *getenv();
 char		buf[BUFSIZ];
 register int	length;


 value = NULL;
 (void) strcpy(buf, item);
 for (length = 0; buf[length] && buf[length] != ' '; length++);
 buf[length] = '\0';

 if (! mailpid) {
    if (! (value = GetMailrc(buf))) {
       if ((s = getenv(buf)) != NULL)
          value = XtNewString(s);
      }
   } else {
    mailenv = QueryMail("set");

    for (s = mailenv; *s && strncmp(s, buf, length); s++)
        for (; *s && *s != '\n'; s++);

    if (! *s) {
       if (s = getenv(buf))
          value = XtNewString(s);
      } else {
       for (; *s && *s != '"' && *s != "'"[0] && *s != '\n'; s++);
       if (! *s || *s == '\n') {	/* variable is flag only, no value */
          value = XtNewString("True");	/* substitute boolean for no value */
         } else {
          for (c = ++s; *c && *c != *(s - 1); c++);
          length = c - s;
          value = XtMalloc((unsigned) length + 1);
          (void) strncpy(value, s, length);
          value[length] = '\0';
	}
      }
    *mailenv = '\0';
    XtFree(mailenv);
   }
  /*
   * Expand shell variables in value
   */
  if (value) {
     char *v, *e, *nvalue;
     while (s = strchr(value, '$')) {
        for (c = s + 1; *c && !strchr(" \t\n$/\"\'", *c); c++);
        length = c - s - 1;
        (void) strncpy(buf, s + 1, length);
        buf[length] = '\0';
        if (*buf == '{') {			/* if variable is braced... */
           bcopy(buf + 1, buf, strlen(buf) - 2);
           buf[strlen(buf) - 2] = '\0';
          }
        if (!(e = getenv(buf)))
	   e = "";
        if (nvalue = XtMalloc((unsigned) strlen(value) - length + strlen(e) + 2)) {
           for (c = nvalue, v = value; v != s; *c++ = *v++);
           for (s = e; *s; *c++ = *s++);
           for (v += length + 1; *v; *c++ = *v++);
           *c = '\0';
           XtFree(value);
           value = nvalue;
          }
       }
    }
 return(value);
} /* GetMailEnv */


/*
** @(#)mailrcFile() - Return a path to environment or default .mailrc file
*/
char *
mailrcFile()
{
 char		*s, *getenv();
 static char	buf[BUFSIZ];

 if (s = getenv("MAILRC"))
    (void) strcpy(buf, s);
 else {
    if ((s = getenv("HOME")) == NULL) s = "";
    (void) sprintf(buf, "%s/.mailrc", s);
   }
 return((char *)buf);
} /* mailrcFile */


/*
** @(#)add_to_list - save buffer of aliases in the global alias list
*/
void
add_to_list(s)
String	s;
{
 extern String	list;
 extern	int	list_length;


 if (! list) {
    list_length = BUFSIZ;
    list = XtMalloc((unsigned) list_length);
    list[0] = '\0';
   }

 for (; *s == ' ' || *s == '\t'; s++);		/* skip leading whitespace */
 if (strlen(list) + strlen(s) + 1 > list_length) {
    list_length += BUFSIZ;
    list = XtRealloc(list, list_length);
   }
 (void) strcat(list, s);			/* append the entire line */
} /* end - add_to_list */


/*
** @(#)get_mailrc - item and return it's value
**                  Handle continuation lines, source of additional files, more
**                  than one occurance of the item, and aliases/groups.
*/
int
get_mailrc(item, value, filename)
char	*item;
char	**value;
char	*filename;
{
 FILE	*fp = NULL;
 char	*c, *d, *s = NULL;
 char	e[1], tmp[BUFSIZ], buf[BUFSIZ];
 int	unset, n;
 int	status = 0;
 int	negated = strncmp(item, "no", 2) == 0 ? 1 : 0;
 int	size = strlen(item) - 2 * negated;
 int	we_want_aliases = (strcmp(item, "alias") == 0);


 if ((fp = fopen(filename, "r")) != NULL) {
    s = fgets(buf, BUFSIZ, fp);
    while (s) {
       if (strncmp(buf, "source", 6) == 0) {
          bcopy(buf + 7, buf, strlen(buf) - 6);
          LASTCH(buf) = '\0';			/* drop the newline character */
          if (buf[0] == '$' || (buf[0] == '~' && buf[1] == '/')) {
             for (unset = 0, n = 1; buf[n] && buf[n] != '/'; n++)
                 if (! strchr("{()}", buf[n]))
                    tmp[unset++] = buf[n];
             tmp[unset] = '\0';
             if (tmp[0])
                d = getenv(tmp);
             else
                d = getenv("HOME");
             (void) sprintf(tmp, "%s%s", d, &buf[n]);
             (void) strcpy(buf, tmp);
            }
          if ((status = get_mailrc(item, &s, buf))) {
             if (we_want_aliases) *value = list;
             else {
                XtFree(*value);
                *value = s;
               }
            } /* end - if something was found in another source */
         } else {
          if (we_want_aliases) {
             if (! (strncmp(buf, "alias", 5) && strncmp(buf, "group", 5))) {
                status = 1;		/* we have found at least one alias */
                add_to_list(&buf[5]);
                if (buf[strlen(buf) - 2] == '\\') {	/* alias is continued */
                   list[strlen(list) - 2] = '\0';	/* drop the "\\n" */
                   while (s = fgets(buf, BUFSIZ, fp)) {
                         if (strlen(s) > 1 && s[strlen(s) - 2] == '\\') {
                            (void) strcpy(&buf[strlen(buf) - 2], " "); /* drop "\\n" */
                            add_to_list(s);		/* add continuations */
                           } else break;
                        }
                   if (s) add_to_list(s);	/* don't forget the last line */
                  } /* end - if this is a continued alias definition line */
               } /* end - if we found a match */
             *value = list;
            } else {		/* I'm looking for 'set' or 'unset' records */
             if (! (strncmp(buf, "set", 3) && strncmp(buf, "unset", 5))) {
                unset = (buf[0] == 'u')? 1:0;	/* find out which one matched */
                s = &buf[(unset?5:3)];		/* starting just beyond 'set' */
                while (*s) {			/* could be multiple assigns */
                   for (; *s && strchr(" \t\\\n", *s); s++);	/* next word */
                   if (*s) {
                      if ((strncmp(s, item, size) != 0) && (!unset ||
                         !negated || strncmp(s, &item[2], size) != 0))
                         for (;*s&&!strchr(" \t\\\n",*s);s++); /* skip item */
                      else {
                         status = 1;		/* we have at least one match */
                         s = s + size;		/* look for a value to it */
                         if (*s++ != '=') {	/* variable only, no value */
                            XtFree(*value);
                            *value = (unset && ! negated)? NULL: XtNewString("True");
                           } else {
                            if (*s == '"' || *s == "'"[0])	/* if quoted */
                               for (c = s + 1; *c && *c != *s; c++);
                            else
                               for (c = s--; *c && !strchr(" \t\\\n", *c); c++);
                            e[0] = *c;		/* save the current character */
                            *c = '\0';		/* mark the end of the value */
                            d = c + 1;		/* save point after value */
                            c = s + 1;		/* point to start of value */
                            XtFree(*value);	/* toss any previous */
                            *value = XtNewString(c);	/* keep latest value */
                            s = d--;		/* look for any more matches */
                            *d = e[0];		/* restore saved character */
                           } /* end - if boolean variable or valued item */
                        } /* end - we have a match */
                     } /* end - if some word(s) still exist on the line */
                   if (! *s && buf[strlen(buf) - 2] == '\\') /* if continued */
                      s = fgets(buf, BUFSIZ, fp);	/* then keep looking */
                  } /* end - while examining this set|unset record */
               } /* end - if we have a set|unset record */
            } /* end - looking for alias or set/unset records */
         } /* end - if not a ``source'' record */
       s = fgets(buf, BUFSIZ, fp);	/* now read the next line of the file */
      } /* end - while something to read in the mailrc file */
    (void) fclose(fp);
   }
 return(status);
}
/* end - get_mailrc */


/*
** @(#)GetMailrc() - Get mail environment variable value from the mailrc file
**		     Added support for source'd files within the .mailrc file
*/
char *
GetMailrc(item)
char	*item;
{
 char	*s = NULL;

 (void) get_mailrc(item, &s, mailrcFile());

 return((char *)s);
} /* GetMailrc */
