/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tpllist.c,v 2.14 1991/09/12 16:36:23 bobg Exp $ */
/* $ACIS:tpllist.c 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tpllist.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/tpllist.c,v 2.14 1991/09/12 16:36:23 bobg Exp $";
#endif /* lint */


#include <class.h>
#define AUXMODULE 1
#include <text.eh>

#include <environ.ih>
#include <filetype.ih>
#include <style.ih>
#include <stylesht.ih>


#define StateInit		0
#define StateSawSlash		1
#define StateSawKeyword		2

struct templatelist  {
    struct stylesheet *sSheet;
    struct templatelist *next;
    boolean hasText;
};

struct templatelist *tlHead = NULL;

static struct templatelist *FindTemplate(ssptr)
    struct stylesheet *ssptr;
{
    register struct templatelist *tPtr;
    
    for (tPtr = tlHead; tPtr != NULL && tPtr->sSheet != ssptr; tPtr = tPtr->next)
        ;
    return tPtr;
}

static struct templatelist *text_AddTemplate(ssptr)
    struct stylesheet *ssptr;
{
    struct templatelist *newTL;

    if (FindTemplate(ssptr) != NULL) return NULL;
    newTL = (struct templatelist *) malloc(sizeof(struct templatelist));
    newTL->next = tlHead;
    tlHead = newTL;
    newTL->sSheet = ssptr;
    newTL->hasText = TRUE;
    return newTL;
}

static struct templatelist *text_FindTemplateByName(templateName)
    char *templateName;
{
    register struct templatelist *tPtr;
    
    for (tPtr = tlHead; tPtr != NULL && strcmp(tPtr->sSheet->templateName, templateName) != 0; tPtr = tPtr->next)
        ;
    return tPtr;
}

static void text_OverrideStyles(ssptr, templateptr)
    struct stylesheet *ssptr;
    struct stylesheet *templateptr;
{
    register int i;
    register struct style **styles, *overridestyle;

    for (i = 0, styles = ssptr->styles; i < ssptr->nstyles; i++, styles++)
	(*styles)->template = 0;
    for (i = 0, styles = templateptr->styles; i < templateptr->nstyles; i++, styles++) {
	if (overridestyle = stylesheet_Find(ssptr, (*styles)->name))
	     style_Copy((*styles), overridestyle);
	else {
	    overridestyle = style_New();
	    style_Copy((*styles), overridestyle);
	    stylesheet_Add(ssptr, overridestyle);
	}
    }
    stylesheet_SetTemplateName(ssptr, templateptr->templateName);
    ssptr->version++;
}

static void SetGlobalStyleInText(self)
struct text *self;
{
    struct style *styleptr = stylesheet_GetGlobalStyle(self->styleSheet);

    if (styleptr == NULL) {
	/* Need to add global style to template */

	styleptr = style_New();
	style_SetName(styleptr, "global");
	styleptr->template = 1;
	stylesheet_Add(self->styleSheet, styleptr);
    }
    text_SetGlobalStyle(self, styleptr);
}

/* This routine parses the contents of a template file */
long text__ReadTemplate(self, templateName, inserttemplatetext)
    struct text *self;
    char *templateName;
    boolean inserttemplatetext;
{
    FILE *fileptr;
    int c, statecode, i;
    struct stylesheet *templateptr = NULL;
    struct templatelist *tlPtr;
    struct style *styleptr;
    char filename[1000], string[1000], *tpath;
    boolean overrideTemplate = TRUE;

    if ((tlPtr = text_FindTemplateByName(templateName)) != NULL && (templateptr = tlPtr->sSheet) != NULL)   {
	if  (inserttemplatetext && tlPtr->hasText && ! self->header.simpletext.pendingReadOnly)
	    overrideTemplate = FALSE;
	else  {
	    /* probably should stat file here to see if it's changed */
	    text_OverrideStyles(self->styleSheet, templateptr);
	    SetGlobalStyleInText(self);
	    return NULL;
	}
    }

    if (templateName[0] == '/') {
	strcpy(filename, templateName);
	fileptr = fopen(filename, "r");
    }
    else {
        tpath = environ_Get("TEMPLATEPATH");
        if (!tpath || !*tpath)
            tpath = environ_GetProfile("atktemplatepath");
        if (!tpath || !*tpath)
            tpath = environ_GetProfile("be2templatepath");
	if (!tpath || !*tpath) {
	    tpath = environ_AndrewDir("/lib/tpls");
	}
	if (!tpath || !*tpath) {
	    tpath = environ_AndrewDir("/lib/templates");
	}
        if (!tpath || !*tpath)
            tpath = ".";
        do {
            char pathBuffer[100], *thisPath;

            thisPath = pathBuffer;
            while ((*tpath == ' ') || (*tpath == ':'))
                tpath++;
            while ((*tpath != ' ') && (*tpath != ':') && (*tpath != '\0') && (thisPath < (pathBuffer + sizeof(pathBuffer) - 1)))
                *thisPath++ = *tpath++;
            *thisPath = '\0';
            sprintf(filename, "%s/%s.tpl", pathBuffer, templateName);
            fileptr = fopen(filename,"r");
            if (!fileptr) {
                sprintf(filename, "%s/%s.template", pathBuffer, templateName);
                fileptr = fopen(filename,"r");
            }
            if (!fileptr) {
                sprintf(filename, "%s/%s", pathBuffer, templateName);
                fileptr = fopen(filename,"r");
            }
        } while ((!fileptr) && (*tpath != '\0'));
    }

    if (!fileptr) {
	fprintf(stderr, "Template '%s' not found.\n", templateName);
	return -1;
    }

    if (templateptr == NULL)  {
	templateptr = (struct stylesheet *) stylesheet_New();
	stylesheet_SetTemplateName(templateptr, templateName);
    }
    if (tlPtr == NULL)  {
	tlPtr = text_AddTemplate(templateptr);
    }
    else  {
        tlPtr->sSheet = templateptr;
	tlPtr->hasText = TRUE;
    }

    if (inserttemplatetext && ! self->header.simpletext.pendingReadOnly) {
	long objectID;
	char *tempTemplateName;

	(void) filetype_Lookup(fileptr, filename, &objectID, NULL);
	tempTemplateName = self->templateName;
	self->templateName = NULL;
	text_Read(self, fileptr, objectID);	/* only for new files */
	self->templateName = tempTemplateName;
	tlPtr->hasText = (text_GetLength(self) != 0);
	fclose(fileptr);
	if (overrideTemplate)  {
	    register long i;
	    register struct style **styles;

	    stylesheet_SetTemplateName(self->styleSheet, templateptr->templateName);
	    text_OverrideStyles(templateptr, self->styleSheet);
	    for (i = 0, styles = templateptr->styles; i < templateptr->nstyles; i++, styles++) {
		(*styles)->template = 1;
	    }
	    for (i = 0, styles = self->styleSheet->styles; i < self->styleSheet->nstyles; i++, styles++) {
		(*styles)->template = 1;
	    }
	}
	else
	    text_OverrideStyles(self->styleSheet, templateptr);
	SetGlobalStyleInText(self);
	return NULL;
    }
    i = 0;
    statecode = StateInit;
    if ((statecode != StateInit) && (statecode != StateSawSlash) &&
	(statecode != StateSawKeyword)) {
	if (fileptr) fclose(fileptr);
	return -2;
    }
    while ((c = getc(fileptr)) != EOF) {
        switch (statecode) {
	    case StateInit:
		if (c == '\\') {	    /* beginning of keyword */
		    statecode = StateSawSlash;
		}
		else if ((c == 0x83) || ((0xc0 & c) == 0xc0) || (c == 0x80)) {
		    fprintf(stderr, "Old style template - ignoring.\n");
		    fclose(fileptr);
		    return -1;
		}
		else ;		/* only used for existing files, don't put out text */
		break;
	    case StateSawSlash:
		if ((c != '\\') && (c != '{') && (c != '}')) {
		    string[i++] = c;
		    statecode = StateSawKeyword;
		}
		else {
		    statecode = StateInit;		/* don't put out text */
		}
		break;
	    case StateSawKeyword:
		if (c == '{') {
		    string[i] = '\0'; i = 0;
		    if (strcmp(string, "textdsversion") == 0)  {
			long versionnumber = 0;

			while ((c = getc(fileptr)) != EOF && c != '}')
			    versionnumber = versionnumber * 10 + (c - '0');
			while (c != EOF && (c = getc(fileptr)) != '\n');

			/* Handle outdated data stream versions here */
			statecode = StateInit;
		    }
		    else if (strcmp(string, "define") == 0) {
			if (stylesheet_Read(templateptr, fileptr, 1) != 0) {
			    if (fileptr) fclose(fileptr);
			    return -2;
			}
			statecode = StateInit;
		    }
		    else if (strcmp(string, "template") == 0) {
			fprintf(stderr, "Recursive templates disallowed - ignoring.\n");
			statecode = StateInit;
		    }
		    else if (strcmp(string, "global") == 0) {
			/* handle global attributes here */
			statecode = StateInit;
		    }
		    else {
			statecode = StateInit;		/* ignore other keywords */
		    }
		}
		else string[i++] = c;
		break;
	}
    }
    fclose(fileptr);
    text_OverrideStyles(self->styleSheet, templateptr);
    SetGlobalStyleInText(self);
    return NULL;
}

