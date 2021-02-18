/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctextv.c,v 1.4 1991/09/12 16:53:54 bobg Exp $ */
/* $ACIS:rctextv.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctextv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rctext/RCS/rctextv.c,v 1.4 1991/09/12 16:53:54 bobg Exp $";
#endif /* lint */

/* RCTextView, a ``c'' mode for BE2. (R-> Rochester contribution) */

#include <ctype.h>

#include <class.h>

#include <im.ih>
#include <message.ih>
#include <rctext.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <proctbl.ih>
#include <bind.ih>

#include "cursor.ih"    /*RSKadd*/
#include "view.ih"    /*RSKadd*/
#include "compress.ih"    /*RSKadd*/
#include "compresv.ih"    /*RSKadd*/

#include <rctextv.eh>

static long BounceTime = 0;
static struct keymap *C_Map;
static struct menulist *C_Menus;

#define isident(c) (isalnum(c) || (c) == '_')

void paren(),brace(),newline(),redo(),tab(), slash();
void colon(), rctextv_rename();

void compress(), compressAll(), decompressAll(); /*RSKadd*/

static struct bind_Description rctextBindings[]={
    {"rctextview-paren",")",')',NULL,0,0,paren,"Insert a paren, with balancing."},
    {"rctextview-paren","]",']'},
    {"rctextview-brace","}",'}',NULL,0,0,brace,"Insert a brace indented properly with balancing."},
    {"rctextview-redo-styles","\033r",0,"C-Text,Redo C styles~10",0,0,redo,"Wrap styles around comments and function names in C code."},
    {"rctextview-redo-styles","\033R",0},
    {"rctextview-tab", "\t",0,"C-Text,Indent line/region~11",0,0,tab,"Indents the current line or selection region."},
    {"rctextview-newline", "\n",0,NULL,0,0,newline,"Inserts a newline and indents properly."},
    {"rctextview-slash","/",'/',NULL,0,0,slash,"Insert slash and restyle if end of comment."},
    {"rctextview-colon",":",':',NULL,0,0,colon,"Insert colon and reindent line."},
    {"rctextview-compress","\033o",0,"C-Text,Compress region~20", 0,0, compress, "Compresses a region of text for viewing general outline."}, /*RSKadd*/
    {"rctextview-compress-all","\033O",0,"C-Text,Compress all inner indents~21", 0,0, compressAll, "Compresses all lines indented (as far or) farther than the current line."}, /*RSKadd*/
    {"rctextview-decompress-all","\033U",0,"C-Text,Decompress all~25", 0,0, decompressAll, "Expands all compressed regions in document to normal size"}, /*RSKadd*/
    {"rctextview-rename-identifier","\033Q",0,"C-Text,Rename Identifier~30",0,
      0,rctextv_rename,"Rename identifier in selection region."}, /*RSKmod*/
    NULL
};

boolean rctextview__InitializeClass(classID)
struct classheader *classID;
{
    C_Menus = menulist_New();
    C_Map = keymap_New();

    bind_BindList(rctextBindings,C_Map,C_Menus,&rctextview_classinfo);
    return TRUE;
}

boolean rctextview__InitializeObject(classID, self)
struct classheader *classID;
struct rctextview *self;
{
    self->c_state = keystate_Create(self, C_Map);
    self->c_menus = menulist_DuplicateML(C_Menus, self);
    rctextview_SetBorder(self,5,5);
    return TRUE;
}

void rctextview__FinalizeObject(classID, self)
struct classheader *classID;
struct rctextview *self;
{
    keystate_Destroy(self->c_state);
    menulist_Destroy(self->c_menus);
}

long rctextview__SetBounceTime(classID, time)
struct classheader *classID;
long time;
{
    long retval = BounceTime;

    BounceTime = time;

    return retval;
}

void rctextview__PostKeyState(self, keystate)
struct rctextview *self;
struct keystate *keystate;
{
    keystate_AddBefore(self->c_state, keystate);
    super_PostKeyState(self, self->c_state);
}

void rctextview__PostMenus(self, menulist)
struct rctextview *self;
struct menulist *menulist;
{
    menulist_ChainAfterML(self->c_menus, menulist, 0);
    super_PostMenus(self, self->c_menus);
}


static void redo(self)
struct rctextview *self;
{
    struct rctext *c = (struct rctext *)self->header.view.dataobject;
    decompressAll(self,0);/*RSKadd*/
    rctext_RedoStyles(c);
    rctext_RegionModified(c, 0, rctext_GetLength(c));
    rctext_NotifyObservers(c, 0);
}


static void match_parens(self, key)
struct rctextview *self;
char key;
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    long start = rctextview_GetDotPosition(self), openparen = rctext_ReverseBalance(ct, start), pos;
    char buffer[256], *ptr;
    int i;

    if (openparen != EOF) {
        if (!rctextview_Visible(self, openparen)) {
            for (pos = openparen; pos > 0; pos--) {
                if (rctext_GetChar(ct, pos) == '\n') {
                    pos++;
                    break;
                }
            }

            ptr = buffer;
            for (i = sizeof(buffer) - 1; i > 0; i--)
                if ((*ptr++ = rctext_GetChar(ct, pos++)) == '\n')
                    break;
            *ptr = NULL;

            message_DisplayString(self, 0, buffer);
        }
        else /* if (BounceTime == 0) */ {
            rctextview_SetDotPosition(self, openparen);
            rctextview_SetDotLength(self, start - openparen);
        }
        /* else Deal with bounce */
    }
    else
        switch (key) {
            case '}':
                message_DisplayString(self, 0, "No matching open brace.");
                break;
            case ')':
                message_DisplayString(self, 0, "No matching open parenthesis");
                break;
            case ']':
                message_DisplayString(self, 0, "No matching open bracket.");
                break;
            default:
                message_DisplayString(self, 0, "Parenthesis mis-match.\n");
        }
}
    
static void paren(self, key)
struct rctextview *self;
char key; /* must be char for "&" to work. */
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int count = im_Argument(rctextview_GetIM(self)), i, pos;

    rctextview_CollapseDot(self);
    pos = rctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        rctext_InsertCharacters(ct, pos, &key, 1);
	
    rctextview_SetDotPosition(self, pos);
    rctextview_FrameDot(self, pos);

    match_parens(self, key);

    rctext_NotifyObservers(ct, 0);
}

static void brace(self, key)
struct rctextview *self;
char key; /* must be char for "&" to work. */
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int count = im_Argument(rctextview_GetIM(self)), i, pos = rctextview_GetDotPosition(self);

    rctextview_CollapseDot(self);
    pos = rctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        rctext_InsertCharacters(ct, pos, &key, 1);

    rctextview_SetDotPosition(self, pos);
    rctextview_FrameDot(self, pos);

    rctext_ReindentLine(ct,pos);

    match_parens(self, key);

    rctext_NotifyObservers(ct, 0);
}

static void slash(self, key)
struct rctextview *self;
char key;
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int count = im_Argument(rctextview_GetIM(self)), i, pos = rctextview_GetDotPosition(self);

    rctextview_CollapseDot(self);
    pos = rctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        rctext_InsertCharacters(ct, pos, &key, 1);

    rctextview_SetDotPosition(self, pos);
    rctextview_FrameDot(self, pos);

    rctext_RecommentBackward(ct, pos);
    rctext_NotifyObservers(ct, 0);
}

static void tab(self, key)
struct rctextview *self;
int key;
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int pos = rctextview_GetDotPosition(self), len = rctextview_GetDotLength(self), c = 0;
    struct mark *mark=rctext_CreateMark(ct,pos,len);

    if(len>0)
	{
	struct im *im=(struct im *)view_GetIM(&(self->header.view)); /*RSKadd*/
	struct cursor *cr=cursor_Create(self->header.view); /*RSKadd*/
	cursor_SetStandard(cr,Cursor_Wait); /*RSKadd*/
	im_SetWindowCursor(im,cr); /*RSKadd*/
        rctext_Indent(ct,mark);
	im_SetWindowCursor(im,NULL); /*RSKadd*/
	}
    else{
        int	oldPos=pos;

        do
            pos--;
        while (pos>=0 && (c=rctext_GetChar(ct,pos))!='\n' && isspace(c));

        if(pos<0 || c=='\n'){	/* indent line */
            mark_SetPos(mark,pos+1);
	    rctextview_SetDotPosition(self,rctext_Indent(ct,mark));
        }else		/* insert whitespace to next column */
            rctextview_SetDotPosition(self,rctext_TabAndOptimizeWS(ct,oldPos,4));
    }

    rctext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    rctext_NotifyObservers(ct,0);
}

static void newline(self, key)
struct rctextview *self;
long key;
{
    int newlines = im_Argument(rctextview_GetIM(self));
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int c;
    long pos,end;
    struct mark *mark;

    rctextview_CollapseDot(self);
    rctext_ReindentLine(ct,rctextview_GetDotPosition(self));

    end=pos=rctextview_GetDotPosition(self); /* may have changed with ReindentLine */
    while(pos>0 && ((c=rctext_GetChar(ct,pos-1))==' ' || c=='\t'))
        pos--;
    if(pos<end)
        rctext_DeleteCharacters(ct,pos,end-pos);

    while(newlines--)
        rctext_InsertCharacters(ct,pos++,"\n",1);

    mark=rctext_CreateMark(ct,pos,0);

    rctextview_SetDotPosition(self,rctext_Indent(ct,mark));

    rctext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    rctext_NotifyObservers(ct,0);
}

static void colon(self, key)
struct rctextview *self;
char key;
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int count = im_Argument(rctextview_GetIM(self)), i, pos;

    rctextview_CollapseDot(self);
    pos = rctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        rctext_InsertCharacters(ct, pos, &key, 1);
	
    rctextview_SetDotPosition(self, pos);
    rctextview_FrameDot(self, pos);

    rctext_ReindentLine(ct, pos);
    rctext_NotifyObservers(ct, 0);
}

static void rctextv_rename(self, key)
struct rctextview *self;
long key;
{
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    int pos, len, newlen;
    boolean insideQuotes, esc, discardIdent;
    char orig[40], rep[40];
    int origlen, replen;

    pos = rctextview_GetDotPosition(self);
    newlen = len = rctextview_GetDotLength(self);

    if (len == 0) {
        message_DisplayString(self, 0, "No region selected\n");
        return;
    }

    if (message_AskForString(self, 0, "Replace identifier: ",
      NULL, orig, sizeof (orig)) < 0 || orig[0] == '\0' ||
      message_AskForString(self, 0, "New string: ", NULL,
      rep, sizeof (rep)) < 0) {
        message_DisplayString(self, 0, "Cancelled.");
        return;
    }

    origlen = strlen(orig);
    replen = strlen(rep);

    /* Skip an identifier that's partially outside region */

    discardIdent =
      (isident(rctext_GetChar(ct, pos)) && pos != 0 &&
       isident(rctext_GetChar(ct, pos - 1)));

    insideQuotes = FALSE;       /* Likely a correct assumption */
    esc = FALSE;

    for (; len >= origlen; pos++, len--) {
        int c = rctext_GetChar(ct, pos);
        if (esc) {
            esc = FALSE;
            continue;
        }
        if (c == '\\') {
            esc = TRUE;
            continue;
        }
        if (discardIdent) {
            if (isident(c))
                continue;
            discardIdent = FALSE;
        }
        if (c == '"')
            insideQuotes = ! insideQuotes;
        if (insideQuotes || ! isident(c))
            continue;
        discardIdent = TRUE;
        if (rctext_Strncmp(ct, pos, orig, origlen) == 0 &&
          ! isident(rctext_GetChar(ct, pos + origlen))) {
            rctext_ReplaceCharacters(ct, pos, origlen, rep, replen);
            len -= origlen - replen - 1;
            newlen += replen - origlen;
        }
    }

    rctextview_SetDotLength(self, newlen);
    rctext_NotifyObservers(ct, 0);
}

static void compress(self, key)    /*RSKadd*/
    struct rctextview *self;
    long key;
    {
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    struct compress *fn;
    long pos,len;

    if (rctext_GetReadOnly(ct))
	{
        message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
	}
    fn = compress_New();
    compressv_HideBoxes();
    pos= rctextview_GetDotPosition(self);
    len= rctextview_GetDotLength(self);
    if (len==0 && rctext_GetChar(ct,pos)=='\n') pos--;
    compress_Compress(fn,ct,pos,len);
    pos=mark_GetPos(fn->cpmark); len=mark_GetLength(fn->cpmark);
    if (compress_IsHidden(fn,ct,pos+1))
	{
	compress_Destroy(fn);
	compressv_UnhideBoxes();
	return;
	}
    /*if (environment_Remove(ct->header.text.rootEnvironment,pos,len, environment_Style,FALSE))*/ 
        rctext_SetModified(ct);
    rctext_AddStyle(ct,pos,len,fn->compressed_style);

    rctextview_SetDotPosition(self,mark_GetPos(fn->cpmark));
    fn->parentview=(struct textview *)(self->header.view.dataobject);
    compressv_MakeBox(fn);

    compressv_UnhideBoxes();
    rctext_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
    rctextview_CollapseDot(self);
    rctext_NotifyObservers(ct,0);
    }

static void compressAll(self, key)    /*RSKadd*/
    struct rctextview *self;
    long key;
    {
    struct rctext *ct = (struct rctext *)self->header.view.dataobject;
    struct compress *fn;
    long pos=1, len, oldpos = rctextview_GetDotPosition(self);
    int curInd;

    if (rctext_GetReadOnly(ct))
	{
        message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
	}
    else{
	struct im *im=(struct im *)view_GetIM(&(self->header.view)); /*RSKadd*/
	struct cursor *cr=cursor_Create(self->header.view); /*RSKadd*/
	cursor_SetStandard(cr,Cursor_Wait); /*RSKadd*/
	im_SetWindowCursor(im,cr); /*RSKadd*/
	if (rctext_GetChar(ct,oldpos)=='\n') oldpos--;
	curInd=compress_CurrentIndentation(ct,oldpos);
	rctextview_CollapseDot(self);
	compressv_HideBoxes();
	while (pos>0)
	{
	    fn=compress_New();
	    pos=compress_NextCompressLoc(fn,ct,pos,curInd);
	    if (pos>0)
	    {
		pos=mark_GetPos(fn->cpmark); len=mark_GetLength(fn->cpmark);
		if (compress_IsHidden(fn,ct,pos+1))
		    compress_Destroy(fn);
		else
		{
		    rctext_AddStyle(ct,pos,len,fn->compressed_style);
		    if (oldpos>pos && oldpos<pos+len)
			rctextview_SetDotPosition(self,pos);
		    compressv_MakeBox(fn);
		    rctext_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
		}
		pos+=mark_GetLength(fn->cpmark)+1;
	    }
	    else compress_Destroy(fn);
	}
	compressv_UnhideBoxes();
	rctext_NotifyObservers(ct,0);
	im_SetWindowCursor(im,NULL); /*RSKadd*/
    }
}

static void decompressAll(self, key)    /*RSKadd*/
    struct rctextview *self;
    long key;
    {
    compressv_DecompressAll();
    }
