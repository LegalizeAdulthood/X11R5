/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctextv.c,v 2.16 1991/09/12 16:12:01 bobg Exp $ */
/* $ACIS:ctextv.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctextv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/ctext/RCS/ctextv.c,v 2.16 1991/09/12 16:12:01 bobg Exp $";
#endif /* lint */

/* CTextView, a ``c'' mode for BE2. */

#include <ctype.h>

#include <class.h>
#include <im.ih>
#include <message.ih>
#include <ctext.ih>
#include <keymap.ih>
#include <keystate.ih>
#include <menulist.ih>
#include <proctbl.ih>
#include <bind.ih>

#include <textv.ih>

#include <ctextv.eh>

static long BounceTime = 0;
static struct keymap *C_Map;
static struct menulist *C_Menus;

#define isident(c) (isalnum(c) || (c) == '_')

void paren(),brace(),newline(),redo(),tab(), slash();
void colon(), ctextv_rename();

static struct bind_Description ctextBindings[]={
    {"ctextview-paren",")",')',NULL,0,0,paren,"Insert a paren, with balancing."},
    {"ctextview-paren","]",']'},
    {"ctextview-brace","}",'}',NULL,0,0,brace,"Insert a brace indented properly with balancing."},
    {"ctextview-redo-styles","\033r",0,"C-Text,Redo C styles~10",0,0,redo,"Wrap styles around comments and function names in C code."},
    {"ctextview-redo-styles","\033R",0},
    {"ctextview-tab", "\t",0,"C-Text,Indent line/region~11",0,0,tab,"Indents the current line or selection region."},
    {"ctextview-newline", "\n",0,NULL,0,0,newline,"Inserts a newline and indents properly."},
    {"ctextview-slash","/",'/',NULL,0,0,slash,"Insert slash and restyle if end of comment."},
    {"ctextview-colon",":",':',NULL,0,0,colon,"Insert colon and reindent line."},
    {"ctextview-rename-identifier","\033Q",0,"C-Text,Rename Identifier~20",0,
      0,ctextv_rename,"Rename identifier in selection region."},
    NULL
};

boolean ctextview__InitializeClass(classID)
struct classheader *classID;
{
    C_Menus = menulist_New();
    C_Map = keymap_New();

    bind_BindList(ctextBindings,C_Map,C_Menus,&ctextview_classinfo);
    return TRUE;
}

boolean ctextview__InitializeObject(classID, self)
struct classheader *classID;
struct ctextview *self;
{
    self->c_state = keystate_Create(self, C_Map);
    self->c_menus = menulist_DuplicateML(C_Menus, self);
    ctextview_SetBorder(self,5,5);
    return TRUE;
}

void ctextview__FinalizeObject(classID, self)
struct classheader *classID;
struct ctextview *self;
{
    keystate_Destroy(self->c_state);
    menulist_Destroy(self->c_menus);
}

long ctextview__SetBounceTime(classID, time)
struct classheader *classID;
long time;
{
    long retval = BounceTime;

    BounceTime = time;

    return retval;
}

void ctextview__PostKeyState(self, keystate)
struct ctextview *self;
struct keystate *keystate;
{
    keystate_AddBefore(self->c_state, keystate);
    super_PostKeyState(self, self->c_state);
}

void ctextview__PostMenus(self, menulist)
struct ctextview *self;
struct menulist *menulist;
{
    menulist_ChainAfterML(self->c_menus, menulist, 0);
    super_PostMenus(self, self->c_menus);
}


static void redo(self)
struct ctextview *self;
{
    struct ctext *c = (struct ctext *)self->header.view.dataobject;

    ctext_RedoStyles(c);
    ctext_RegionModified(c, 0, ctext_GetLength(c));
    ctext_NotifyObservers(c, 0);
}


static void match_parens(self, key)
struct ctextview *self;
char key;
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    long start = ctextview_GetDotPosition(self), openparen = ctext_ReverseBalance(ct, start), pos;
    char buffer[256], *ptr;
    int i;

    if (openparen != EOF) {
        if (!ctextview_Visible(self, openparen)) {
            for (pos = openparen; pos > 0; pos--) {
                if (ctext_GetChar(ct, pos) == '\n') {
                    pos++;
                    break;
                }
            }

            ptr = buffer;
            for (i = sizeof(buffer) - 1; i > 0; i--)
                if ((*ptr++ = ctext_GetChar(ct, pos++)) == '\n')
                    break;
            *ptr = NULL;

            message_DisplayString(self, 0, buffer);
        }
	ctextview_SetDotPosition(self, openparen);
	ctextview_SetDotLength(self, start - openparen);
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
struct ctextview *self;
char key; /* must be char for "&" to work. */
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int count = im_Argument(ctextview_GetIM(self)), i, pos;

    /* Ctext needs to know about vi command mode (ecch!) */
    if (ctextview_GetEditor(self) == VI &&
	 ctextview_GetVIMode(self) == COMMAND )
	return;

    ctextview_CollapseDot(self);
    pos = ctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        ctext_InsertCharacters(ct, pos, &key, 1);
	
    ctextview_SetDotPosition(self, pos);
    ctextview_FrameDot(self, pos);

    match_parens(self, key);

    ctext_NotifyObservers(ct, 0);
}

static void brace(self, key)
struct ctextview *self;
char key; /* must be char for "&" to work. */
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int count = im_Argument(ctextview_GetIM(self)), i, pos = ctextview_GetDotPosition(self);
    struct proctable_Entry	*textviewEntry;

    /* Ctext needs to know about vi command mode (ecch!) */
    if (ctextview_GetEditor(self) == VI &&
	 ctextview_GetVIMode(self) == COMMAND )
    {
	if ( (textviewEntry = proctable_Lookup("textview-forward-para")) != NULL )
	    (*textviewEntry->proc) ((struct textview *) self, key);
	else
	    message_DisplayString(self, 0,
				  "Can`t find textview-forward-para in proctable");
	return;
    }

    ctextview_CollapseDot(self);
    pos = ctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        ctext_InsertCharacters(ct, pos, &key, 1);

    ctextview_SetDotPosition(self, pos);
    ctextview_FrameDot(self, pos);

    ctext_ReindentLine(ct,pos);

    match_parens(self, key);

    ctext_NotifyObservers(ct, 0);
}

static void slash(self, key)
struct ctextview *self;
char key;
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int count = im_Argument(ctextview_GetIM(self)), i, pos = ctextview_GetDotPosition(self);
    struct proctable_Entry	*textviewEntry;

    /* Ctext needs to know about vi command mode (ecch!) */
    if ( ctextview_GetEditor(self) == VI &&
	 ctextview_GetVIMode(self) == COMMAND )
    {
	if ( (textviewEntry = proctable_Lookup("textview-search")) != NULL )
	    (*textviewEntry->proc) ((struct textview *) self, key);
	else
	    message_DisplayString(self, 0,
				  "Can`t find textview-search in proctable");
	return;
    }

    ctextview_CollapseDot(self);
    pos = ctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        ctext_InsertCharacters(ct, pos, &key, 1);

    ctextview_SetDotPosition(self, pos);
    ctextview_FrameDot(self, pos);

    ctext_RecommentBackward(ct, pos);
    ctext_NotifyObservers(ct, 0);
}

static void tab(self, key)
struct ctextview *self;
int key;
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int pos = ctextview_GetDotPosition(self), len = ctextview_GetDotLength(self), c = 0;
    struct mark *mark=ctext_CreateMark(ct,pos,len);

    if(len>0)
        ctext_Indent(ct,mark);
    else{
        int	oldPos=pos;

        do
            pos--;
        while (pos>=0 && (c=ctext_GetChar(ct,pos))!='\n' && isspace(c));

        if(pos<0 || c=='\n'){	/* indent line */
            mark_SetPos(mark,pos+1);
	    ctextview_SetDotPosition(self,ctext_Indent(ct,mark));
        }else		/* insert whitespace to next column */
            ctextview_SetDotPosition(self,ctext_TabAndOptimizeWS(ct,oldPos,4));
    }

    ctext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    ctext_NotifyObservers(ct,0);
}

static void newline(self, key)
struct ctextview *self;
long key;
{
    int newlines = im_Argument(ctextview_GetIM(self));
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int c;
    long pos,end;
    struct mark *mark;
    struct proctable_Entry	*textviewEntry;

    /* Ctext needs to know about vi command mode (ecch!) */
    if (ctextview_GetEditor(self) == VI &&
	 ctextview_GetVIMode(self) == COMMAND )
    {
	if ( (textviewEntry = proctable_Lookup("textview-next-line")) != NULL )
	    (*textviewEntry->proc) ((struct textview *) self, key);
	else
	    message_DisplayString(self, 0,
				  "Can`t find textview-next-line in proctable");
	return;
    }

    ctextview_CollapseDot(self);
    ctext_ReindentLine(ct,ctextview_GetDotPosition(self));

    end=pos=ctextview_GetDotPosition(self); /* may have changed with ReindentLine */
    while(pos>0 && ((c=ctext_GetChar(ct,pos-1))==' ' || c=='\t'))
        pos--;
    if(pos<end)
        ctext_DeleteCharacters(ct,pos,end-pos);

    while(newlines--)
        ctext_InsertCharacters(ct,pos++,"\n",1);

    mark=ctext_CreateMark(ct,pos,0);

    ctextview_SetDotPosition(self,ctext_Indent(ct,mark));

    ctext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    ctext_NotifyObservers(ct,0);
}

static void colon(self, key)
struct ctextview *self;
char key;
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int count = im_Argument(ctextview_GetIM(self)), i, pos;
    struct proctable_Entry	*textviewEntry;

    /* Ctext needs to know about vi command mode (ecch!) */
    if (ctextview_GetEditor(self) == VI &&
	 ctextview_GetVIMode(self) == COMMAND )
    {
	if ( (textviewEntry = proctable_Lookup("textview-vi-command")) != NULL )
	    (*textviewEntry->proc) ((struct textview *) self, key);
	else
	    message_DisplayString(self, 0,
				  "Can`t find textview-next-line in proctable");
	return;
    }

    ctextview_CollapseDot(self);
    pos = ctextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
        ctext_InsertCharacters(ct, pos, &key, 1);
	
    ctextview_SetDotPosition(self, pos);
    ctextview_FrameDot(self, pos);

    ctext_ReindentLine(ct, pos);
    ctext_NotifyObservers(ct, 0);
}

static void ctextv_rename(self, key)
struct ctextview *self;
long key;
{
    struct ctext *ct = (struct ctext *)self->header.view.dataobject;
    int pos, len, newlen;
    boolean insideQuotes, esc, discardIdent, skippingReplaced;
    char orig[40], rep[40];
    int origlen, replen, skipReplaced;

    pos = ctextview_GetDotPosition(self);
    newlen = len = ctextview_GetDotLength(self);

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
      (isident(ctext_GetChar(ct, pos)) && pos != 0 &&
       isident(ctext_GetChar(ct, pos - 1)));

    insideQuotes = FALSE;       /* Likely a correct assumption */
    esc = FALSE;
    skippingReplaced = FALSE;

    for (; len >= origlen; pos++, len--) {
        int c = ctext_GetChar(ct, pos);
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
	if(skippingReplaced) {
	   if(--skipReplaced > 0)
	       continue;
	   else 
	       skippingReplaced = FALSE;
	}
        if (ctext_Strncmp(ct, pos, orig, origlen) == 0 &&
          ! isident(ctext_GetChar(ct, pos + origlen))) {
            ctext_ReplaceCharacters(ct, pos, origlen, rep, replen);
            len -= origlen - replen - 1;
            newlen += replen - origlen;
	    skippingReplaced = TRUE;
	    skipReplaced = replen;
        }
    }

    ctextview_SetDotLength(self, newlen);
    ctext_NotifyObservers(ct, 0);
}
