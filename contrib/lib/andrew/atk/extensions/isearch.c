/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/extensions/RCS/isearch.c,v 2.10 1991/09/12 16:20:18 bobg Exp $ */
/* $ACIS:isearch.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/extensions/RCS/isearch.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/extensions/RCS/isearch.c,v 2.10 1991/09/12 16:20:18 bobg Exp $";
#endif /* lint */

#include <class.h>

#include <bind.ih>
#include <text.ih>
#include <textv.ih>
#include <mark.ih>
#include <search.ih>
#include <message.ih>
#include <im.ih>
#include <environ.ih>

#include <isearch.eh>

static boolean useSelectionRegion;

#define MAXSTRING 256

static char LastString[MAXSTRING] = {0};

void strappend(dst, src, ch, len)
char *dst, *src;
int ch, len;
{
    /* copy as much of the source as we can. */
    while (--len > 0 && (*dst = *src++) != '\0')
        dst++;

    /* If there is room for the character, add it. */
    if (len > 0)
        *dst++ = ch;

    /* Add the null. Note, the above tests should leave us room. */
    *dst = '\0';
}

static boolean dosearch(tv, txt, pos, string, forwardp, contForward, contBackward, errmsg)
struct textview *tv;
struct text *txt;
struct mark *pos;
char *string, *errmsg;
boolean forwardp;
boolean contForward;
boolean contBackward;
{
    int c, fudge;
    boolean results;
    struct mark *newpos;
    char newstring[MAXSTRING], *newerr;
    static char prompt[MAXSTRING+256];
    long loc;
    struct SearchPattern *pattern;
    boolean newContForward = contForward;
    boolean newContBackward = contBackward;

    do {
	if (strlen (string) > 0) strcpy(LastString, string);
        if (pos != NULL) {
            textview_SetDotPosition(tv, mark_GetPos(pos));
            textview_SetDotLength(tv, mark_GetLength(pos));
            textview_FrameDot(tv, mark_GetPos(pos));
            sprintf(prompt, "Search %s: %s", forwardp ? "forward" : "backward", string);
        }
        else {
            if (errmsg != NULL)
                sprintf(prompt, "Search %s: %s [%s]", forwardp ? "forward" : "backward", string, errmsg);
            else
                sprintf(prompt, "Failing search %s: %s", forwardp ? "forward" : "backward", string);
        }
        message_DisplayString(tv, 0, prompt);

        im_ForceUpdate();

        c = im_GetCharacter(textview_GetIM(tv));

        if (c == '\r')
            c = '\n';

        if ((c < ' ' && c != '\n') || c == '\177') {
            switch (c) {
                case -1:
                case 'G'-64:
                    /* Punt. */
                    message_DisplayString(tv, 0, "Cancelled.");
                    return FALSE;
                case 'S'-64:
                case 'R'-64:
                    forwardp = (c == 'S'-64);
		    strcpy(newstring, LastString);
		    newContForward = TRUE;
		    newContBackward = TRUE;
                    break;
                case 'H'-64:
                case '\177':
		    LastString[0] = '\0';   /* Let caller set it if it is to be non-empty */
                    return TRUE;
                default:
                    message_DisplayString(tv, 0, "");
                    im_DoKey(textview_GetIM(tv), c);
                    return FALSE;
            }
        }
        else
            strappend(newstring, string, c, sizeof(newstring));

	if (newstring[0] != '\0') {
	    loc = -1;
            newpos = NULL;
            pattern = NULL;
            newerr = search_CompilePattern(newstring, &pattern);
            if (newerr == NULL) {
                if (strcmp(string, newstring) == 0)
                    fudge = 1;
                else
                    fudge = 0;

		if (forwardp) {
		    if (contForward) {
			loc = search_MatchPattern(txt, textview_GetDotPosition(tv) + fudge, pattern);
		    }
		}
		else {
		    if (contBackward) {
			loc = search_MatchPatternReverse(txt, textview_GetDotPosition(tv) - fudge, pattern);
		    }
		}

                if (loc >= 0) {
		    newpos = text_CreateMark(txt, loc, search_GetMatchLength());
		    newContForward = TRUE;
		    newContBackward = TRUE;
		}
		else {
		    if (forwardp) {
			newContForward = FALSE;
		    }
		    else {
			newContBackward = FALSE;
		    }
		}
            }

            results = dosearch(tv, txt, newpos, newstring, forwardp, newContForward, newContBackward, newerr);
            if (newpos != NULL) {
                text_RemoveMark(txt, newpos);
                mark_Destroy(newpos);
            }
        }
        else
            results = dosearch(tv, txt, pos, newstring, forwardp, TRUE, TRUE, NULL);
    } while (results);
    return FALSE;
}

static void search(tv, key)
struct textview *tv;
long key;
{
    struct text *txt = (struct text *)(tv->header.view.dataobject);
    long pos = textview_GetDotPosition(tv);
    long len = textview_GetDotLength(tv);
    struct mark *mark = text_CreateMark(txt, pos, len);
    char *searchString = NULL;

    if (useSelectionRegion && len != 0) {
	char unquotedString[1000];

	if (len >= sizeof(unquotedString) - 1) {
	    message_DisplayString(tv, 0, "Search string too long - continuing with truncating string.");
	    len = sizeof(unquotedString) -1;
	}
	    
	text_CopySubString(txt, pos, len, unquotedString, FALSE);
	searchString = search_GetQuotedSearchString(unquotedString, NULL, 0);
    }
	    
    while (dosearch(tv, txt, mark, (searchString != NULL) ? searchString : "", key == 'S'-64, TRUE, TRUE, NULL))
        ;
    
    if (searchString != NULL) {
	free(searchString);
    }
    text_RemoveMark(txt, mark);
    mark_Destroy(mark);
}


boolean incsearch__InitializeClass()
{
    static struct bind_Description fns[] = {
        {"incsearch-forward", NULL, 0, NULL, 0, 0, search, "Search forward incrementally.", "incsearch"},
        {"incsearch-backward", NULL, 0, NULL, 0, 0, search, "Search backward incrementally.", "incsearch"},
        {NULL},
    };
    struct classinfo *textviewClassinfo;

    useSelectionRegion = environ_GetProfileSwitch("incsearchUseSelectionRegion", TRUE);

    textviewClassinfo = class_Load("textview");
    if (textviewClassinfo != NULL) {
        bind_BindList(fns, NULL, NULL, textviewClassinfo);
        return TRUE;
    }
    else
        return FALSE;
}
