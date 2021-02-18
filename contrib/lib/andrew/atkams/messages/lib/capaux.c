/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/capaux.c,v 1.25 1991/09/19 17:13:27 rr2b Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/capaux.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atkams/messages/lib/RCS/capaux.c,v 1.25 1991/09/19 17:13:27 rr2b Exp $ ";
#endif /* lint */

#include <andrewos.h>
#include <sys/param.h>
#include <cui.h>
#include <fdphack.h>
#include <errprntf.h>
   
#include <im.ih>
#include <frame.ih>
#include <envrment.ih>
#include <message.ih>
#include <text.ih>
#include <textv.ih>
#include <scroll.ih>

#include <ams.ih>
#include <text822.ih>
#include <text822v.ih>
#include <amsutil.ih>
#include <folders.ih>
#define AUXMODULE 1
#include <captions.eh>

extern int (*captextv_PreviousLineCmd)(),
	   (*captextv_ReverseSearchCmd)(),
	   (*captextv_ScrollScreenFwdCmd)(),
	   (*captextv_ScrollScreenBackCmd)(),
	   (*captextv_BeginningOfTextCmd)(),
	   (*captextv_BeginningOfLineCmd)(),
	   (*captextv_GlitchDownCmd)();

void captions_CaptionsCompound(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "captions", cmds);
}

void captions_CaptionsTextviewCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), self, "textview", cmds);
}

void captions_CaptionsFoldersCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), captions_GetFolders(self), "folders", cmds);
}

void captions_CaptionsBodiesCommand(self, cmds)
struct captions *self;
char *cmds;
{
    ams_GenericCompoundAction(ams_GetAMS(), captions_GetBodView(self), "t822view", cmds);
}

void captions_DownFocus(self)
struct captions *self;
{
    ams_Focus(captions_GetBodView(self));
}

void captions_UpFocus(self)
struct captions *self;
{
    ams_Focus(captions_GetFolders(self));
}

void captions__ClearMarks(self)
struct captions *self;
{
    int j, whicholdmark = 0;
    struct CaptionCache *hc;

    if (self->MarkCount <= 0) {
	message_DisplayString(NULL, 10, "There are no marked messages");
	return;
    }
    ams_WaitCursor(TRUE);
    if (self->OldMarks) free(self->OldMarks);
    self->OldMarks = (int *) malloc((1+self->MarkCount)*sizeof(long));
    if (self->OldMarks) self->OldMarkCount = self->MarkCount;
    for (j = 0; j < self->captioncachecount; ++j) {
	hc = &self->capcache[j];
	if (hc->IsMarked) {
	    if (self->OldMarks) self->OldMarks[whicholdmark++] = hc->cuid;
	    captions_ToggleMark(self, hc, hc->offset);
	}
    }
    self->MarkCount = 0;
    captions_PostMenus(self, NULL);
    message_DisplayString(NULL, 10, "All marks cleared; there are now no messages marked.");
    captions_WantUpdate(self, self);
    ams_WaitCursor(FALSE);
}

ClassifyMarkedByName(self, NameGiven)
struct captions *self;
char *NameGiven;
{
    int code;

    if (*NameGiven == '*') {
	++NameGiven;
	code = MARKACTION_APPENDBYNAME;
    } else {
	code = MARKACTION_CLASSIFYBYNAME;
    }
    captions_ActOnMarkedMessages(self, code, NameGiven);
}

void captions_SimulateLeftClick( self, rock )
    struct captions *self;
    long rock;
{
    captions_SimulateClick(self, TRUE);
}

void captions_SimulateRightClick( self, rock )
    struct captions *self;
    long rock;
{
    captions_SimulateClick(self, FALSE);
}

void captions__PrintVisibleMessage(self) 
struct captions *self;
{
    int flags = 0;

    if (self->VisibleCUID <1) {
	message_DisplayString(NULL, 10, "There is nothing to print.");
	return;
    }
    ams_WaitCursor(TRUE);
    if (self->CurrentFormatting & MODE822_ROT13) {
	flags |= AMS_PRINT_ROT13;
    }
    if (self->CurrentFormatting & MODE822_FIXEDWIDTH) {
	flags |= AMS_PRINT_FIXED;
    }
    message_DisplayString(NULL, 10, "Printing message; please wait...");
    im_ForceUpdate();
    if (ams_CUI_PrintBodyFromCUIDWithFlags(ams_GetAMS(), self->VisibleCUID, flags, NULL) == 0) {
	message_DisplayString(NULL, 10, "Message queued for printing.");
    }
    ams_WaitCursor(FALSE);
}


void captions__PuntCurrent(self, GoToNext)
struct captions *self;
Boolean GoToNext;
{
    int loops = 1;
    struct im *im = captions_GetIM(self);
    char Scratch[100+MAXPATHLEN];

    if (im && im_ArgProvided(im)) {
	loops = im_Argument(im);
	if (loops < 0) {
	    loops = 1;
	} else if (loops > 1) {
	    sprintf(Scratch, "Do you really want to punt the next %d folders", loops);
	    if (!ams_GetBooleanFromUser(ams_GetAMS(), Scratch, FALSE)) {
		return;
	    }
	}
    }
    while (loops--) {
	if (self->captioncachecount > 0) {
	    SetSouthernmostPoint(self, self->capcache[self->captioncachecount-1].offset);
	}
	if (GoToNext) {
	    folders_NextFolder(captions_GetFolders(self), FALSE);
	}
	if (loops>0) im_ForceUpdate();
    }
}

void captions__ThisIsFlorida(self)
struct captions *self;
{
    int pos, len, whichcaption;
    struct environment *env;

    im_ForceUpdate();
    /* The following line makes things work better if this caption is the first one fetched into the caption cache but not the first in the folder */
    captions_GuaranteeFetchedRange(self, self->FolderSize - self->FetchedFromEnd - 1, self->FolderSize);
    pos = self->HighlightStartPos;
    (void) captions_FindCUIDByDocLocation(self, &pos, &len, &env, &whichcaption);
    SetSouthernmostPoint(self, -1); /* Always needed */
    if (--whichcaption >= 0) {
	pos = self->capcache[whichcaption].offset;
	(void) captions_FindCUIDByDocLocation(self, &pos, &len, &env, &whichcaption);
	SetSouthernmostPoint(self, pos);
    }
    im_ForceUpdate();
}

void CapBeginText(self)
struct captions *self;
{
    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    im_ForceUpdate();
    captextv_BeginningOfTextCmd((struct textview *) self);
}

void CapScrollBack(self)
struct captions *self;
{
    int min, pos, mylen, whichcaption;
    struct range total, seen, dot;
    struct environment *envptr;

    self->textscrollinterface->GetInfo(self, &total, &seen, &dot);
    pos = captions_DecodePosition(self, seen.beg);
    /* The following conservatively assume a 6 pt font, 4 pts spacing, maximum of one screen of scrolling */
    captions_FindCUIDByDocLocation(self, &pos, &mylen, &envptr, &whichcaption);
    min = self->FolderSize - self->FetchedFromEnd - (captions_GetLogicalHeight(self)/10) + whichcaption - 1;
    if (min < 0) min = 0;
    captions_GuaranteeFetchedRange(self, min, self->FolderSize);
    im_ForceUpdate();
    captextv_ScrollScreenBackCmd((struct textview *) self);
}

void captions__CapReverseSearch(self)
struct captions *self;
{
    captions_GuaranteeFetchedRange(self, 0, self->FolderSize);
    im_ForceUpdate();
    captextv_ReverseSearchCmd((struct textview *) self);
    captions_WantInputFocus(self, self);
}

void PreviousCaptionLine(self)
struct captions *self;
{
    int backupto = self->FolderSize - self->FetchedFromEnd -2;
    struct im *im = captions_GetIM(self);

    if (im && im_ArgProvided(im)) {
	backupto -= im_Argument(im);
    }
    if (captions_GetDotPosition(self) < 150) { /* poor heuristic */
	if (captions_GuaranteeFetchedRange(self, backupto, self->FolderSize)) return; /* error reported */
    }
    captextv_PreviousLineCmd((struct textview *) self);
}

void CapGlitchDown(self)
struct captions *self;
{
    int backupto = self->FolderSize - self->FetchedFromEnd -2;
    struct im *im = captions_GetIM(self);
    
    if (im && im_ArgProvided(im)) {
	backupto -= im_Argument(im);
    }
    if (captions_GuaranteeFetchedRange(self, backupto, self->FolderSize)) return;
    im_ForceUpdate();
    captextv_GlitchDownCmd((struct textview *) self);
}

captions_PurgeDeletions(ci)
struct captions *ci;
{
    message_DisplayString(NULL, 10, "Purging deletions; please wait...");
    im_ForceUpdate();
    ams_WaitCursor(TRUE);
    if (ams_CUI_PurgeDeletions(ams_GetAMS(), ci->FullName) == 0) {
	message_DisplayString(NULL, 10, "Purging complete.");
    }
    ams_WaitCursor(FALSE);
}

void captions__ClearAndUpdate(ci, ConsiderPurging, SaveState) 
struct captions *ci;
int ConsiderPurging, SaveState;
{
    struct text *d;

    d = ci->CaptText;
    if (SaveState) captions_MakeCachedUpdates(ci);
    if (ConsiderPurging
	 && ams_CUI_DoesDirNeedPurging(ams_GetAMS(), ci->FullName)
	 && (amsutil_GetOptBit(EXP_PURGEONQUIT)
	     || ams_GetBooleanFromUser(ams_GetAMS(), "Do you want to purge the deleted messages", FALSE))) {
	captions_PurgeDeletions(ci);
    }
    ResetCaptionNotBody(ci);
    if (ci->MarkCount > 0) {
	/* We clear the marks this way so that they can be restored later if so desired.  */
	captions_ClearMarks(ci);
	ci->MarkCount = 0;
    }
    if (ci->FullName) {
	free(ci->FullName);
	ci->FullName = NULL;
    }
    if (ci->ShortName) {
	free(ci->ShortName);
	ci->ShortName = NULL;
    }
    ci->captioncachecount = 0;
    ci->FetchedFromStart = ci->FetchedFromEnd = ci->FolderSize = 0;
    SetSouthernmostPoint(ci, -1);
    text_ClearCompletely(d);
    text_SetGlobalStyle(d, ci->GlobalCapStyle);
    captions_SetDotPosition(ci, 0);
    captions_SetDotLength(ci, 0);
    captions_PostMenus(ci, NULL);
    captions_WantUpdate(ci, ci);
    im_ForceUpdate();
}

int
captions_InsertCaptions(ci, shortname, dname, StartTime, ShowAll)
struct captions *ci;
char *shortname, *dname, *StartTime;
Boolean ShowAll;
{
    int totalbytes, numbytes, cuid, status, addlen, highstart, highlen, envstart, inspos, insertct = 0, IsDup, myfirstcuid = 0;
    char date64[AMS_DATESIZE+1], olddate64[AMS_DATESIZE+1], newdate[AMS_DATESIZE+1], firstdate[AMS_DATESIZE+1], captionbuf[100*AMS_SNAPSHOTSIZE], ErrorText[256], *DirName, *ThisCaption, *s;
    Boolean UseHighDensity, MayModify, IsRead;
    struct environment *et, *et2;
    int PositionDot = 1, NewCt, TotalCt, ProtCode;
    long errcode;
    struct CaptionCache *tempcache = NULL;
    int tempcachesize, tempcachecount;

    if ((errcode = ams_CUI_DisambiguateDir(ams_GetAMS(), dname, &DirName)) != 0) {
	if (*dname == '/' && ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
	    ams_CUI_HandleMissingFolder(ams_GetAMS(), dname);
	    return(-1);
	} else if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
	    sprintf(ErrorText, "%s: temporarily unavailable (net/server problem)", shortname);
	} else if (ams_AMS_ERRNO(ams_GetAMS()) == EACCES) {
	    sprintf(ErrorText, "'%s' is private; you don't have read-access or are unauthenticated.", shortname);
	} else {
	    sprintf(ErrorText, "The folder %s is not readable.", shortname);
	}
	message_DisplayString(NULL, 75, ErrorText);
	return(-1);
    }
    UseHighDensity = ! (amsutil_GetOptBit(EXP_WHITESPACE));
    if (!StartTime) {
	long mytime;

	im_ForceUpdate();
	errcode = ams_MS_GetDirInfo(ams_GetAMS(), DirName, &ProtCode, &TotalCt);
	if (errcode) {
	    if (ams_AMS_ERRNO(ams_GetAMS()) == ENOENT) {
		ams_CUI_HandleMissingFolder(ams_GetAMS(), dname);
		return(-1);
	    }
	    sprintf(ErrorText, "Cannot look up information about %s", DirName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, TRUE, errcode);
	    return(-1);
	}
	ci->IsFullMail =  (ProtCode == AMS_DIRPROT_FULLMAIL) ? TRUE : FALSE;
	ci->FolderSize = TotalCt;
	if (errcode = ams_MS_GetNewMessageCount(ams_GetAMS(), DirName, &NewCt, &TotalCt, olddate64, TRUE)) {
	    sprintf(ErrorText, "Couldn't get date information for %s", DirName);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_CRITICAL, TRUE, errcode);
	    return(-1);
	}
	if (NewCt == TotalCt) ShowAll = TRUE;
	if (ShowAll) {
	    date64[0] = '\0';
	    NewCt = TotalCt;
	} else {
	    mytime = amsutil_conv64tolong(olddate64);
	    if (mytime>0) {
		strcpy(date64, amsutil_convlongto64(mytime-1, 0));
	    }
	}
	inspos = insertct;
   } else {
	strcpy(date64, StartTime);
    }
    insertct = 0;
    inspos = 0;
    newdate[0] = '\0';
    totalbytes = 0;
    s = NULL;
    if (StartTime) {
	tempcache = (struct CaptionCache *) malloc(25 * sizeof(struct CaptionCache));
	tempcachesize = 25;
	tempcachecount = 0;
    }
    do {
	if ((errcode = ams_CUI_GetHeaders(ams_GetAMS(), DirName, date64, captionbuf, sizeof(captionbuf), totalbytes, &numbytes, &status, FALSE)) != 0) {
	    captions_WantUpdate(ci, ci);
	    if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
		sprintf(ErrorText, "%s: temporarily unavailable (net/server problem)", DirName);
		ams_ReportSuccess(ams_GetAMS(), ErrorText);
	    } else {
	        sprintf(ErrorText, "Could not get captions for %s", dname);
		ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	    }
	    if (tempcache) free(tempcache);
	    return(-1);
	}
	if (numbytes <= 0) break;
	totalbytes += numbytes;
	for (s=captionbuf; s-captionbuf  < numbytes; s+= AMS_SNAPSHOTSIZE) {
	    cuid = ams_CUI_GetCuid(ams_GetAMS(), AMS_ID(s), DirName, &IsDup);
	    if (ci->firstcuid == cuid) {
		/* All done -- we are back to where we started */
		status = 0;
		break;
	    }
	    MayModify = AMS_GET_ATTRIBUTE(s, AMS_ATT_MAYMODIFY) ? 1 : 0;
	    if (MayModify) {
		IsRead = AMS_GET_ATTRIBUTE(s, AMS_ATT_UNSEEN) ? FALSE : TRUE;
	    } else {
		IsRead = (!StartTime && (myfirstcuid || ShowAll)) ? FALSE : TRUE;
	    }
	    if (!myfirstcuid) {
		myfirstcuid = cuid;
		strcpy(firstdate, AMS_DATE(s));
	    }
	    MakeCaptionLine(&ThisCaption, cuid, s, amsutil_GetOptBit(EXP_FIXCAPTIONS), &highstart, &highlen, ci->IsFullMail, IsDup, IsRead);
	    if (!UseHighDensity) {
		strcat(ThisCaption, "\n");
	    }
	    addlen = strlen(ThisCaption);
	    text_AlwaysInsertCharacters(ci->CaptText, inspos, ThisCaption, addlen);
	    if (!StartTime && PositionDot) {
		captions_SetDotPosition(ci, inspos+addlen-1);
		captions_FrameDot(ci, inspos);
		PositionDot = 0;
	    }
	    insertct += addlen;
	    et2 = environment_InsertStyle(ci->CaptText->rootEnvironment, inspos, ci->IconicStyle, 1);
	    environment_SetLength(et2, 3);
	    envstart = inspos + 2;
	    et = environment_InsertStyle(ci->CaptText->rootEnvironment, envstart, AMS_GET_ATTRIBUTE(s, AMS_ATT_DELETED) ? ci->DeletedStyle : ci->NormalCaptionStyle, 1);
	    environment_SetLength(et, addlen -3);
	    if (StartTime) {
		AddCaptionToCacheEntry(&tempcache, &tempcachecount, &tempcachesize, cuid, inspos, et, et2, MayModify, s, IsDup);
	    } else {
		AddCaptionToCacheEntry(&ci->capcache, &ci->captioncachecount, &ci->captioncachesize, cuid, inspos, et, et2, MayModify, s, IsDup);
	    }
	    ++ci->FetchedFromEnd; /* sometimes from start? */
	    if (ci->IsFullMail) {
		et = environment_InsertStyle(et, 0, ci->MailStyle, 1);
		environment_SetLength(et, addlen - 3); 
	    }
	    if (highlen) {
		et = environment_InsertStyle(et, inspos+highstart - envstart, ci->HighlightStyle, 1);
		environment_SetLength(et, highlen);
	    }
	    inspos += addlen;
	}
    } while (status > 0);
    if (myfirstcuid) ci->firstcuid = myfirstcuid;
    if (GetSouthernmostPoint(ci) == 0) { /* No southernmost point yet */
	int newsouth;

	if ((ci->substatus == AMS_SHOWALLSUBSCRIBED)
	    || ShowAll
	    || (ci->captioncachecount <= 0)
	    || (!StartTime && myfirstcuid && (strcmp(firstdate, olddate64)< 0))) {
	    newsouth = -1;
	    captions_SetDotPosition(ci, 0);
	} else {
	    newsouth = ci->capcache[0].offset;
	}
	SetSouthernmostPoint(ci, newsouth);
    }
    if (status < 0) {
	sprintf(ErrorText, "Couldn't read all of the captions successfully");
	ams_ReportError(ams_GetAMS(), ErrorText, ERR_CRITICAL, TRUE, errcode);
	if (tempcache) free(tempcache);
	return(-1);
    }
    if (ci->firstcuid) {
	if (s && !StartTime) {
	    strcpy(newdate, AMS_DATE(s - AMS_SNAPSHOTSIZE));
	}
    }
    captions_WantUpdate(ci, ci);
    if (StartTime) {
	if (tempcache) {
	    MergeTwoCacheEntries(ci, tempcache, tempcachecount, tempcachesize, inspos);
	    free(tempcache);
	}
    } else {
	char CommText[500];

	if (ci->substatus == AMS_SHOWALLSUBSCRIBED) {
	    sprintf(CommText, " (%s; All %d shown) ", ams_DescribeProt(ams_GetAMS(), ProtCode), TotalCt);
	} else {
	    sprintf(CommText, " (%s; %d new of %d) ", ams_DescribeProt(ams_GetAMS(), ProtCode), NewCt, TotalCt);
	}
	ci->CommentText = malloc(1+strlen(CommText));
	if (ci->CommentText) {
	    strcpy(ci->CommentText, CommText);
	}
    }
    return(0);
}

int captions__InsertUpdatesInDocument(ci, shortname, dname, ShowFirst)
struct captions *ci;
char *shortname, *dname;
Boolean ShowFirst;
{
    int code, substatus;
    long errcode;
    char DumBuf[1+MAXPATHLEN];
    boolean HadFullName = TRUE;

    ams_WaitCursor(TRUE);
    captions_ClearAndUpdate(ci, FALSE, TRUE);
    if (!dname || !*dname) {
	HadFullName = FALSE;
	errcode = ams_CUI_DisambiguateDir(ams_GetAMS(), shortname, &dname);
	if (errcode) {
	    if (ams_vdown(ams_GetAMS(), ams_AMS_ERRNO(ams_GetAMS()))) {
		sprintf(DumBuf, "%s: temporarily unavailable (net/server problem)", shortname);
	    } else if (ams_AMS_ERRNO(ams_GetAMS()) == EACCES) {
		sprintf(DumBuf, "'%s' is private; you don't have read-access or are unauthenticated.", shortname);
	    } else {
		sprintf(DumBuf, "The folder %s is not readable.", shortname);
	    }
	    message_DisplayString(NULL, 75, DumBuf);
	    return(-1);
	}
    }
    errcode = ams_MS_GetSubscriptionEntry(ams_GetAMS(), dname, DumBuf, &substatus);
    if (errcode) {
	ams_WaitCursor(FALSE);
	ams_ReportError(ams_GetAMS(), "Could not get subscription entry", ERR_WARNING, TRUE, errcode);
	return(-1);
    }
    if (!HadFullName && ci->myfold) {
	folders_AlterSubscriptionStatus(ci->myfold, dname, substatus, shortname);
    }
    if (substatus == AMS_PRINTSUBSCRIBED) {
	ams_CUI_PrintUpdates(ams_GetAMS(), dname, shortname);
    }
    ci->CommentText = NULL;
    ci->firstcuid = -1;
    ci->FullName = malloc(1+strlen(dname));
    if (ci->FullName) strcpy(ci->FullName, dname);
    ci->ShortName = malloc(1+strlen(shortname));
    if (ci->ShortName) strcpy(ci->ShortName, shortname);
    ci->substatus = substatus;
    captions_SetLabel(ci, shortname);
    code = captions_InsertCaptions(ci, shortname, dname, NULL, substatus == AMS_SHOWALLSUBSCRIBED ? TRUE : FALSE);
    if (ci->CommentText) {
	folders_HighlightFolder(captions_GetFolders(ci), dname, ci->CommentText); 
	free(ci->CommentText);
	ci->CommentText = NULL;
    }

    im_ForceUpdate();
    if (ShowFirst) {
	if (substatus == AMS_SHOWALLSUBSCRIBED) {
	    int len, pos;

	    pos = ci->capcache[0].offset;
	    if (ci->captioncachecount > 1) {
		len = ci->capcache[1].offset - pos;
	    } else if (ci->captioncachecount > 0) {
		len = text_GetLength(ci->CaptText) - pos;
	    } else {
		len = 0;
	    }
	    if (len) captions_DisplayNewBody(ci, ci->capcache[0].cuid, pos, len, ci->capcache[0].env);
	} else {
	    captions_ShowMore(ci, FALSE, FALSE, FALSE);
	}
    } else {
	/* Pre-fetch the first message */
	if (ci->captioncachecount > 1) {
	    ams_CUI_PrefetchMessage(ams_GetAMS(), ci->capcache[1].cuid, 0);
	} else if (ci->captioncachecount > 0) {
	    ams_CUI_PrefetchMessage(ams_GetAMS(), ci->capcache[0].cuid, 0);
	}
    }
    captions_WantInputFocus(ci, ci);
    ams_WaitCursor(FALSE);
    return(code);
}

void captions__ResetVisibleCaption(h)
struct captions *h;
{
    struct t822view *bv = captions_GetBodView(h);

    ResetCaptionNotBody(h);
    text_ClearCompletely(captions_GetBodDoc(h));
    text822_ResetGlobalStyle(captions_GetBodDoc(h));
    if (t822view_GetIM(bv) && im_GetInputFocus(t822view_GetIM(bv)) == NULL) t822view_WantInputFocus(bv, bv);
    t822view_SetDotPosition(bv, 0);
    t822view_SetDotLength(bv, 0);
    t822view_FrameDot(bv, 0);
    t822view_WantUpdate(bv, bv);
}

int captions__GetBodyFromCUID(ci, cuid, Mode, ContentTypeOverride)
struct captions *ci;
int cuid, Mode;
char *ContentTypeOverride;
{
    struct text *d;
    struct t822view *bv;
    char FileName[1+MAXPATHLEN], ErrorText[500];
    int ShouldDelete, IgnorePos, len;
    FILE *fp;
    struct im *im;

    d = captions_GetBodDoc(ci);
    bv = captions_GetBodView(ci);
    text_ClearCompletely(d);
    text822_ResetGlobalStyle(d);
    im = t822view_GetIM(bv);
    if (im && im_GetInputFocus(im) == NULL) t822view_WantInputFocus(bv, bv);
    if (ams_CUI_ReallyGetBodyToLocalFile(ams_GetAMS(), cuid, FileName, &ShouldDelete, !ams_CUI_SnapIsRunning(ams_GetAMS()))
    || ams_CUI_GetSnapshotFromCUID(ams_GetAMS(), cuid, ci->VisibleSnapshot)) {
	return(-1); /* Error message already reported */
    }
    fp = fopen(FileName, "r");
    if (!fp) {
	ams_ReportError(ams_GetAMS(), "Cannot read message body file", ERR_WARNING, FALSE, 0);
	return(-1);
    }
    if (!text822_ReadIntoText(d, fp, Mode, ContentTypeOverride, &len, TRUE, &ci->StartOfRealBody, &IgnorePos, NULL)) {
	ams_ReportError(ams_GetAMS(), "Cannot read text822 object", ERR_WARNING, FALSE, 0);
	fclose(fp);
	return(-1);
    }
    fclose(fp);
    ci->CurrentFormatting = Mode;
    if (ShouldDelete) {
	if (unlink(FileName)) {
	    sprintf(ErrorText, "Cannot unlink local file %s (%d)", FileName, errno);
	    ams_ReportError(ams_GetAMS(), ErrorText, ERR_WARNING, FALSE, 0);
	}
    }
    t822view_SetTopPosition(bv, IgnorePos+1);
    t822view_SetDotPosition(bv, IgnorePos+1);
    t822view_SetDotLength(bv, 0);

    text_SetReadOnly(d, TRUE);
    MarkVisibleMessageSeen(ci);
    t822view_WantUpdate(bv, bv);
    captions_WantUpdate(ci, ci);
    im = t822view_GetIM(bv);
#ifdef NOWAYJOSE
    if (im) {
	im_ExposeWindow(im);
    }
#endif /* NOWAYJOSE */
    im_ForceUpdate();
    ams_SubtleDialogs(ams_GetAMS(), TRUE);
    ams_CUI_ProcessMessageAttributes(ams_GetAMS(), cuid, ci->VisibleSnapshot);
    ams_SubtleDialogs(ams_GetAMS(), FALSE);
    return (0);
}

int captions__DisplayNewBody(captions, thisCUID, linestart, linelen, env)
struct captions *captions;
int thisCUID, linestart, linelen;
struct environment *env;
{
    char *id, *dir;
    int WasDeleted, IsDeleted, checkvis;

    if (captions->captioncachecount <= 0) return;
    ams_WaitCursor(TRUE);
    /* Want to increase the likelihood the next line is visible unless this is the last line, in which case we simply want to ensure this line is visible. */
    if ((linestart + linelen + 1) < text_GetLength(captions->CaptText)) {
	checkvis = linestart+linelen+1;
    } else {
	checkvis = linestart;
    }
    if (!captions_Visible(captions, checkvis)) {
	captions_SetTopPosition(captions, linestart);
    }
    if (captions->HighlightLen) {
	RemoveHighlighting(captions);
    }
    captions->HighlightStartPos = linestart;
    captions->HighlightLen = linelen;
    captions->HighlightEnv = env;
    if (captions->HighlightEnv->data.style == captions->ActiveDeletedStyle
    || captions->HighlightEnv->data.style == captions->DeletedStyle) {
	text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveDeletedStyle);
	WasDeleted = 1;
    } else {
	text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveCaptionStyle);
	WasDeleted = 0;
    }
    t822view_WantUpdate(captions_GetBodView(captions), captions_GetBodView(captions));
    captions_WantUpdate(captions, captions);
    im_ForceUpdate();
    if (thisCUID == captions->VisibleCUID) {
	SetSouthernmostPoint(captions, linestart);
	ams_WaitCursor(FALSE);
	return(0);
    }
    captions->VisibleCUID = thisCUID;
    if (captions_GetBodyFromCUID(captions, thisCUID, MODE822_NORMAL, NULL)) {
	/* error was reported */
	ams_WaitCursor(FALSE);
	ResetCaptionNotBody(captions);
	return(-1);
    }
    IsDeleted = AMS_GET_ATTRIBUTE(captions->VisibleSnapshot, AMS_ATT_DELETED) ? 1 : 0;
    if (IsDeleted != WasDeleted) {
	/* Someone else just deleted or undeleted it! */
	if (IsDeleted) {
	    message_DisplayString(NULL, 25, "Someone has just deleted this message, but not yet purged it.");
	    text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveDeletedStyle);
	} else {
	    text_SetEnvironmentStyle(captions->CaptText, captions->HighlightEnv, captions->ActiveCaptionStyle);
	    message_DisplayString(NULL, 25, "Someone has just undeleted this message.");
	}	
	captions_AlterDeletedIcon(captions, captions->HighlightStartPos, IsDeleted);
	captions_WantUpdate(captions, captions);
    }
    SetSouthernmostPoint(captions, linestart);
    im_ForceUpdate();
    if (!ams_CUI_GetAMSID(ams_GetAMS(), thisCUID, &id, &dir)) {
	ams_MS_PrefetchMessage(ams_GetAMS(), dir, id, 1);
    }
    ams_WaitCursor(FALSE);
    return(0);
}

void captions__SetLabel(self, label)
struct captions *self;
char *label;
{
    if (self->myframe) {
	frame_SetTitle(self->myframe, label);
    }
}
