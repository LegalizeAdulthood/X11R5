/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *text_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/text822.c,v 1.23 1991/09/19 15:48:40 rr2b Exp $";

#include <ctype.h>
#include <andrewos.h>
#include <class.h>
#include <text822.eh>
#include <text.ih>
#include <style.ih>
#include <fontdesc.ih>
#include <envrment.ih>
#include <readscr.ih>
#include <rofftext.ih>
#include <filetype.ih>
#include <amsutil.ih>
#include <dataobj.ih>
#include <stylesht.ih>
#include <environ.ih>
#include <fnote.ih>
#include <ams.h>

#ifndef NOMETAMAIL
#include <sys/param.h>
#include <fdphack.h>
#undef popen /* BOGUS -- should be handled by fdphack */
#undef pclose /* ditto */
#include <ams.ih>
#include <message.ih>
#include <im.ih>

static void MetaOutput(fp, self)
FILE *fp;
struct text *self;
{
    char buf[1000];

    if (fgets(buf, sizeof(buf), fp) != NULL) {
        text_AlwaysInsertCharacters(self, text_GetLength(self), buf,
				    strlen(buf));
        text_NotifyObservers(self, 0);
        return;
    }
    if (errno != EWOULDBLOCK) {
        im_RemoveFileHandler(fp);
        pclose(fp);
        strcpy(buf, "\n--- Command execution terminated ---\n");
        text_AlwaysInsertCharacters(self, text_GetLength(self), buf,
				    strlen(buf));
    }
    text_NotifyObservers(self, 0);
}
#endif

extern char *index();
static char *EmptyMsgString = "<empty message>";
static struct style *FixedStyle, *BoldStyle, *FormatStyle, *TinyStyle, *GlobalStyle;
static char *myfontname = NULL;
static int myfontsize, UsingFootNote, PrintMinorHeaders;

boolean text822__InitializeObject(c, self)
struct classheader *c;
struct text822 *self;
{
    if (text822_ReadTemplate(self, "messages", FALSE)) {
	fprintf(stderr, "Could not read messages template!\n");
    }
    self->InstructionsStyle = style_New();
    style_SetFontSize(self->InstructionsStyle, style_ConstantFontSize, 16);
    style_SetJustification(self->InstructionsStyle, style_LeftJustified);
    style_SetFontFamily(self->InstructionsStyle, myfontname);
    self->BigBoldStyle = style_New();
    style_SetFontSize(self->BigBoldStyle, style_PreviousFontSize, 2);
    style_AddNewFontFace(self->BigBoldStyle, (long) fontdesc_Bold);
    text822_SetCopyAsText(self, TRUE);
    return(TRUE);
}

boolean text822__InitializeClass(c)
struct classheader *c;
{
    UsingFootNote = environ_GetProfileSwitch("usefootnote", TRUE);
    PrintMinorHeaders = environ_GetProfileSwitch("printminorheaders", TRUE);
    myfontname = amsutil_GetDefaultFontName();
    myfontsize = environ_GetProfileInt("fontsize", 12);
    FixedStyle = style_New();
    style_SetName(FixedStyle, "typewriter");
    style_SetFontFamily(FixedStyle, "andytype");
    style_AddNewFontFace(FixedStyle, (long) fontdesc_Fixed);
    /* The following is necessary if you don't want fixed-width printing to wrap almost every single line! */
    if (myfontsize >= 10) style_SetFontSize(FixedStyle, style_ConstantFontSize, 10);
    BoldStyle = style_New();
    style_SetName(BoldStyle, "bold");
    style_AddNewFontFace(BoldStyle, (long) fontdesc_Bold);
    style_SetFontFamily(BoldStyle, "andy");
    FormatStyle = style_New();
    style_SetName(FormatStyle, "formatnote");
    style_AddPassThru(FormatStyle);
    TinyStyle = style_New();
    style_SetFontSize(TinyStyle, style_ConstantFontSize, 8);
    style_SetName(TinyStyle, "smaller");
    GlobalStyle = style_New();
    style_SetJustification(GlobalStyle, style_LeftJustified);
    style_SetFontSize(GlobalStyle, style_ConstantFontSize, myfontsize);
    style_SetFontFamily(GlobalStyle, myfontname);
    return(TRUE);
}

long text822__Read(self, fp, id)
struct text822 *self;
FILE *fp;
long id;
{
    int len, bs, ig;

    text822_Clear(self);
    text822_SetID(self, text822_UniqueID(self));
    if (text822_ReadIntoText(self, fp, MODE822_NORMAL, NULL, &len, FALSE, &bs, &ig, NULL)) {
	return(dataobject_NOREADERROR);
    }
    return(dataobject_BADFORMAT);
}

long text822__ReadSubString(self, pos, fp, quoteCharacters)
struct text822 *self;
long pos;
FILE *fp;
boolean quoteCharacters;
{
    int len, bs, ig;

    text822_ReadIntoText(self, fp, MODE822_NORMAL, NULL, &len, FALSE, &bs, &ig, NULL);
    return(len);
}

boolean text822__ReadIntoText(ch, d, fp, Mode, ContentTypeOverride, len, IsReallyTextObject, BodyStart, IgnorePosition, AuxHeadText)
struct classheader *ch;
struct text822 *d;
FILE *fp;
int Mode;
char *ContentTypeOverride;
int *len;
boolean IsReallyTextObject;
int *BodyStart, *IgnorePosition;
struct text *AuxHeadText;
{
    struct environment *et;
    char LineBuf[5000], ScribeFormatVersion[100], *ColonLocation, c, ContentType[100];
    int HighlightThisOne, ShowPos, linelen, ColonOffset;
    boolean ReadRaw, SawEndData;
    int SVers, pos, fnlen, didbig, didsmall;
    char fmttype[25], fmtvers[25], fmtresources[200], *sfmttype, *sfmtvers;
    char **MyHeadsArray;
    static char *SendHeadsArray[] = {
	"if-type-unsupported", "content-type",
	"x-andrew-scribeformat", "x-andrew-message-size",
	"x-andrew-text822mode", NULL};
    boolean showheads, usefn, printminors;
    boolean showallbutkeys = amsutil_GetOptBit(EXP_SHOWALLBUTKEYS);
    struct fnote *fn;

    if (text822_ReadTemplate(d, "messages", FALSE)) {
	fprintf(stderr, "Could not read messages template!\n");
    }
    text822_SetGlobalStyle(d, GlobalStyle);
    if (AuxHeadText) {
	MyHeadsArray = SendHeadsArray;
	showheads = TRUE;
    } else {
	MyHeadsArray = amsutil_GetKeyHeadsArray();
	showheads = !amsutil_GetOptBit(EXP_SHOWNOHEADS);
    }
    if (IsReallyTextObject) {
	usefn = FALSE;
	printminors = TRUE;
    } else {
	printminors = PrintMinorHeaders;
	usefn = PrintMinorHeaders ? UsingFootNote : FALSE;
    }
    if (AuxHeadText) usefn = FALSE;
restart:
    didbig = 0;
    didsmall = 0;
    *IgnorePosition = text822_GetLength(d);
    if (usefn) {
	fn = fnote_New();
	if (!fn) {
	    fprintf(stderr, "Could not create footnote object!\n");
	    usefn = FALSE;
	} else {
	    fnlen = 0;
	    text822_AlwaysInsertCharacters(d, *IgnorePosition, "\n\n", 2);
	    text822_AlwaysAddView(d, *IgnorePosition, "fnotev", fn);
	    fnote_addenv(fn,d,*IgnorePosition);
	    fnote_Close(fn, d);
	    ShowPos = *IgnorePosition;
	}
    }
    if (!usefn) { /* Not just an else clause if fnote_New fails */
	if (!AuxHeadText) text822_AlwaysInsertCharacters(d, *IgnorePosition, "\n\n", 2);
	ShowPos = *IgnorePosition + 1;
    }
    ReadRaw = TRUE;
    SawEndData = FALSE;
    HighlightThisOne = FALSE;
    ScribeFormatVersion[0] = '\0';
    ContentType[0] = '\0';
    fmtvers[0] = '\0';
    fmttype[0] = '\0';
    fmtresources[0] = '\0';
    while (fgets(LineBuf, sizeof(LineBuf), fp)) {
	linelen = strlen(LineBuf);
	c = LineBuf[0];
	if (c == '\n') break; /* done with headers */
	if (c == ' ' || c == '\t') {
	    ColonOffset = 0;
	} else {
	    /* Decide where to show it */
	    ColonLocation = index(LineBuf, ':');
	    if (ColonLocation) {
		*ColonLocation = '\0';
		ColonOffset = ColonLocation - LineBuf;
	    } else {
		ColonOffset = 0;
	    }
	    if (!amsutil_lc2strncmp("content-type", LineBuf, sizeof(LineBuf))) {
		
		strncpy(ContentType, ColonLocation ? ColonLocation+1 : "", sizeof(ContentType));
	    } else if (!amsutil_lc2strncmp("x-andrew-scribeformat", LineBuf, sizeof(LineBuf))) {
		
		strncpy(ScribeFormatVersion, ColonLocation ? ColonLocation+1 : "", sizeof(ScribeFormatVersion));
	    } else if ((Mode == MODE822_NORMAL) && !amsutil_lc2strncmp("x-andrew-text822mode", LineBuf, sizeof(LineBuf))) {
		int newmode;
		newmode = ColonLocation ? (atoi(ColonLocation+1)) : MODE822_NORMAL;
		Mode = 0;
		if (newmode & AMS_PRINT_FIXED) Mode |= MODE822_FIXEDWIDTH;
		if (newmode & AMS_PRINT_ROT13) Mode |= MODE822_ROT13;
	    }
	    if (showheads && MyHeadsArray) {
		int i;

		if (AuxHeadText || showallbutkeys) {
		    HighlightThisOne = TRUE;
		} else {
		    HighlightThisOne = FALSE;
		}
		for (i=0; MyHeadsArray[i]; ++i) {
		    if (!amsutil_lc2strncmp(MyHeadsArray[i], LineBuf, sizeof(LineBuf))) {
			HighlightThisOne = !HighlightThisOne;
			break;
		    }
		}
	    }
	    if (ColonLocation) *ColonLocation = ':';
	}
	if (AuxHeadText) {
	    /* We're really being called by sendmessage, with two text objects */
	    int hlen = text_GetLength(AuxHeadText);
	    if (HighlightThisOne) {
		
		text_AlwaysInsertCharacters(AuxHeadText, hlen, LineBuf, linelen);
		hlen += linelen;
	    }
	} else {
	    /* Normal case -- put important ones in front, others in back */
	    if (HighlightThisOne) {
		++didbig;
		text822_AlwaysInsertCharacters(d, ShowPos, LineBuf, linelen);
		if (ColonOffset) {
		    et = environment_InsertStyle(((struct text *)d)->rootEnvironment, ShowPos, BoldStyle, 1);
		    environment_SetLength(et, ColonOffset);
		}
		ShowPos += linelen;
	    } else {
		if (printminors) {
		    ++didsmall;
		    if (usefn) {
			fnote_AlwaysInsertCharacters(fn, fnlen, LineBuf, linelen);
			if (ColonOffset) {
			    et = environment_InsertStyle(((struct text *)fn)->rootEnvironment, fnlen, BoldStyle, 1);
			    environment_SetLength(et, ColonOffset);
			}
			fnlen += linelen;
			/* The next line prevents a troff bug with enormous footnotes */
			if (((didbig*14) + (didsmall*10)) > 600) usefn = FALSE;
		    } else {
			text822_AlwaysInsertCharacters(d, *IgnorePosition, LineBuf, linelen);
			et = environment_InsertStyle(((struct text *)d)->rootEnvironment, *IgnorePosition, TinyStyle, 1);
			environment_SetLength(et, linelen);
			environment_SetStyle(et, FALSE, FALSE);
			if (ColonOffset) {
			    et = environment_InsertStyle(et, 0, BoldStyle, 1);
			    environment_SetLength(et, ColonOffset);
			}
			*IgnorePosition += linelen;
			ShowPos += linelen;
		    }
		}
	    }
	}
    } /* All done with headers, ready for body */
    if (AuxHeadText) {
	/* We don't want the very last newline */
	int hlen = text_GetLength(AuxHeadText) -1;
	char c = text_GetChar(AuxHeadText, hlen);
	if (c == '\n') {
	    text_AlwaysDeleteCharacters(AuxHeadText, hlen, 1);
	}
    }
    if (usefn) {
	/* Ditto */
	char c = text822_GetChar(d, ShowPos - 1);
	if (c == '\n') {
	    text822_AlwaysDeleteCharacters(d, ShowPos-1, 1);
	}
    }
    ShowPos = *BodyStart = text822_GetLength(d);
    if (Mode & MODE822_FIXEDWIDTH) {
	et = environment_InsertStyle(((struct text *)d)->rootEnvironment, *BodyStart, FixedStyle, 1);
	environment_SetLength(et, text822_GetLength(d) - *BodyStart);
	environment_SetStyle(et, TRUE, TRUE);
    }
    if (!ContentTypeOverride) {
	ContentTypeOverride = ContentType;
    }
    if (ContentTypeOverride[0]) {
	amsutil_BreakDownContentTypeField(ContentTypeOverride, fmttype, sizeof(fmttype), fmtvers, sizeof(fmtvers), fmtresources, sizeof(fmtresources));
    } else if (ScribeFormatVersion[0]) {
	linelen = strlen(ScribeFormatVersion);
	ScribeFormatVersion[linelen-1] = '\0';
	strcpy(fmttype, "x-be2");
	strcpy(fmtvers, ScribeFormatVersion);
	fmtresources[0] = '\0';
    }
    for (sfmttype = fmttype; *sfmttype && isspace(*sfmttype); ++sfmttype) {
	;
    }
    for (sfmtvers = fmtvers; *sfmtvers && isspace(*sfmtvers); ++sfmtvers) {
	;
    }
    if (sfmttype && sfmttype[0]) {
	if (!amsutil_lc2strncmp("x-be2", sfmttype, strlen(sfmttype))) {
	    ReadRaw = FALSE;
	    SVers = atoi(fmtvers);
	    if (SVers < 10) {	
		while(fgets(LineBuf, sizeof(LineBuf), fp)) {
		    if (!strncmp(LineBuf, "\\enddata{text822", 16)) {
			SawEndData = TRUE;
			break;
		    }
		    linelen = strlen(LineBuf);
		    text822_AlwaysInsertCharacters(d, ShowPos, LineBuf, linelen);
		    ShowPos += linelen;
		}
		if (readscr_Begin(d, *BodyStart, text822_GetLength(d) - *BodyStart, TRUE, fmtvers, TRUE)) {
		    /* Succeeded in handling as scribe file */
		}
	    } else {
		int myid = 0;
		char *objtype;
		int pos;

		/* New BE2 datastream, call Read routine */
		pos = ftell(fp);
		objtype = filetype_Lookup(fp, NULL, myid, NULL);
		if (ftell(fp) == 0) {
		    fseek(fp, pos, 0);
		}
		if (objtype && !strcmp(objtype, "text")) {
		    text822_SetReadOnly(d, FALSE);
		    if (IsReallyTextObject) {
			*len = text822_ReadSubString(d, ShowPos, fp, TRUE);
		    } else {
			*len = super_ReadSubString(d, ShowPos, fp, TRUE);
		    }
		} else {
		    ReadRaw = TRUE;
		}
	    }
	} else if (!amsutil_lc2strncmp("troff", sfmttype, strlen(sfmttype))) {
	    char **resources = (char **) amsutil_BreakDownResourcesIntoArray(fmtresources);

	    rofftext_ReadRoffIntoText(d, fp, ShowPos, resources);
	    if (resources) free(resources);
	    ReadRaw = FALSE;
#ifndef NOMETAMAIL
        } else if (!environ_Get("NOMETAMAIL") && IsReallyTextObject &&
amsutil_lc2strncmp("text", sfmttype, strlen(sfmttype))) {
            /* IsReallyTextObject test ensures we don't run metamail when
printing!!! */
            char TmpFileName[1+MAXPATHLEN], LineBuf[1000], Cmd[1+MAXPATHLEN],
Msg[50+MAXPATHLEN], TmpFile2[1+MAXPATHLEN];
            FILE *fp2;

            sprintf(Msg, "Do you want to run an interpreter for this '%s' format mail", sfmttype);
            if (environ_Get("MM_NOASK")
                || ams_GetBooleanFromUser(ams_GetAMS(), Msg, TRUE)) {
                ams_CUI_GenLocalTmpFileName(ams_GetAMS(), TmpFileName);
                ams_CUI_GenLocalTmpFileName(ams_GetAMS(), TmpFile2);
                fp2 = (FILE *) fopen (TmpFileName, "w");
                if (fp2) {
                    fseek(fp, 0, 0);
                    while (fgets(LineBuf, sizeof(LineBuf), fp)) {
                        fputs(LineBuf, fp2);
                    }
                    fclose(fp2);
                    sprintf(Cmd, "metamail -m messages -z -x -d -q %s 2>&1", TmpFileName);
                    sprintf(Msg, "Executing: %s\n", Cmd);
                    linelen = strlen(Msg);
                    text822_AlwaysInsertCharacters(d, ShowPos, Msg, linelen);
                    ShowPos += strlen(Msg);
                    fp2 = (FILE *) popen(Cmd, "r");
                    im_AddFileHandler(fp2, MetaOutput, d, 0);
                }
            }
#endif
	}
    }
    if (ReadRaw) {
	while(fgets(LineBuf, sizeof(LineBuf), fp)) {
	    if (!strncmp(LineBuf, "\\enddata{text822", 16)) {
		SawEndData = TRUE;
		break;
	    }
	    linelen = strlen(LineBuf);
	    text822_AlwaysInsertCharacters(d, ShowPos, LineBuf, linelen);
	    ShowPos += linelen;
	}
    }
    if (!SawEndData) {
	fgets(LineBuf, sizeof(LineBuf), fp); /* Just eat it */
	while (LineBuf[0] == '\n') {
	    LineBuf[0] = '\0';
	    fgets(LineBuf, sizeof(LineBuf), fp);
	}
	if (strncmp(LineBuf, "\\enddata{text822", 16)) {
/*	    fprintf(stderr, "Missing enddata in text822 -- saw %s instead\n", LineBuf); */
	}
    }
    if (!feof(fp)) {
	pos = ftell(fp);
	fgets(LineBuf, sizeof(LineBuf), fp);
	if (strncmp(LineBuf, "\\begindata{text822", 18)) {
	    fseek(fp, pos, 0);
	} else {
	    /* text822 is funny this way -- it always peeks at and reads any following text822 objects, allowing concatenation but disallowing embedding consecutive text822 objects. */
	    struct text *t = (struct text *) d;

	    pos = text822_GetLength(d);
	    text822_AlwaysInsertCharacters(d, pos, "\n.bp\n\n", 6);

	    /* Make sure there is no style at the end that grows to include the new stuff */
	    if (environment_Remove(t->rootEnvironment, pos, 6, environment_Style, TRUE)) {
		text_SetModified(t);
	    }
	    text_RegionModified(t, pos, 6);
	    text_NotifyObservers(t, 0);

	    et = environment_InsertStyle(((struct text *)d)->rootEnvironment, pos+1, FormatStyle, 1);
	    environment_SetLength(et, 4);
	    goto restart;
	}
    }
    pos = text822_GetLength(d);
    text822_AlwaysDeleteCharacters(d, pos--, 1);
    if (Mode & MODE822_ROT13) {
	RotateThirteen(d, *BodyStart);
    }
    if (pos < 3 && !AuxHeadText) {
	text822_AlwaysInsertCharacters(d, 0, EmptyMsgString, sizeof(EmptyMsgString) - 1);
	*BodyStart = 0;
    }
    text822_SetGlobalStyle(d, GlobalStyle);
    return(TRUE);
}

char *text822__ViewName(t)
struct text822 *t;
{
    return("textview"); /* t822view is not necessary */
}

long text822__Write(self, fp, writeID, level)
struct text822 *self;
FILE *fp;
long writeID;
int level;
{
    int bodystart, len;
    unsigned char ch;
    boolean SawNewline = FALSE, SawPrevNewline = FALSE;

    if (self->header.dataobject.writeID != writeID)  {
	self->header.dataobject.writeID = writeID;
	fprintf(fp, "\\begindata{%s, %ld}\n", class_GetTypeName(self), dataobject_UniqueID(&self->header.dataobject));
	len = text822_GetLength(self);
	for (bodystart = 0; bodystart <= len; ++bodystart) {
	    ch = text822_GetChar(self, bodystart);
	    if (ch == '\n') {
		if (SawNewline) {
		    if (SawPrevNewline) break;
		    SawPrevNewline = TRUE;
		} else {
		    fputc(ch, fp);
		}
		SawNewline = TRUE;
	    } else {
		fputc(ch, fp);
		SawNewline = FALSE;
	    }
	}
	fprintf(fp, "\n\\textdsversion{%d}\n", 12); /* BOGUS -- CAN'T GET RIGHT */
	if (((struct text *) self)->styleSheet->templateName)
	    fprintf(fp, "\\template{%s}\n", ((struct text *) self)->styleSheet->templateName);
	stylesheet_Write(((struct text *) self)->styleSheet, fp);
	text822_WriteSubString(self, bodystart, text822_GetLength(self) - bodystart, fp, TRUE);
	fprintf(fp, "\\enddata{%s,%d}\n", class_GetTypeName(self), self->header.dataobject.id);
	fflush(fp);
    }
    return self->header.dataobject.id;
}

RotateThirteen(d, start)
struct text *d;
int start;
{
    register char *cp,*ecp;
    long len, lengotten;
    char *tbuf;

    if (start < 0) start = 0;
    len = text_GetLength(d) - start;
    while (len > 0) {
	tbuf = text_GetBuf(d, start, len, &lengotten);
	for (cp = tbuf, ecp = cp+lengotten; cp<ecp; ++cp) {
	    if ( (*cp >= 0x41 && *cp <= 0x5a) || 
		(*cp >= 0x61 && *cp <= 0x7a) )
		*cp = (((((*cp -1 ) & 0X1F) + 13)  % 26) + 1)
		  | (*cp & 0XE0);
	}
	text_ReplaceCharacters(d, start, lengotten, tbuf, lengotten);
	start += lengotten;
	len -= lengotten;
    }
}

void text822__Clear(self)
struct text822 *self;
{
    super_Clear(self);
    if (text822_ReadTemplate(self, "messages", FALSE)) {
	fprintf(stderr, "Could not read messages template!\n");
    }
    text822_SetGlobalStyle(self, GlobalStyle);
}

void text822__ClearCompletely(self)
struct text822 *self;
{
    super_ClearCompletely(self);
    if (text822_ReadTemplate(self, "messages", FALSE)) {
	fprintf(stderr, "Could not read messages template!\n");
    }
    text822_SetGlobalStyle(self, GlobalStyle);
}

long text822__ReadAsText(self, fp, id)
struct text822 *self;
FILE *fp;
long id;
{
    return(super_Read(self, fp, id));
}

void text822__ResetGlobalStyle(c, t)
struct classheader *c;
struct text822 *t;
{
    text822_SetGlobalStyle(t, GlobalStyle);
}
