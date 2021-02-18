/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtextv.c,v 1.10 1991/09/12 16:53:17 bobg Exp $ */
/* $ACIS:mtextview.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtextv.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/mtext/RCS/mtextv.c,v 1.10 1991/09/12 16:53:17 bobg Exp $";
#endif /* lint */

/* MtextView, a ``Modula-2'' mode for BE2. */
#include <ctype.h>

#include "class.h"

#include "im.ih"
#include "message.ih"
#include "mtext.ih"
#include "keymap.ih"
#include "keystate.ih"
#include "menulist.ih"
#include "proctbl.ih"
#include "bind.ih"
#include "envrment.ih"
#include "mtextv.eh"
#include "textv.ih"

static long BounceTime = 0;
static struct keymap *M_Map;
static struct menulist *M_Menus;

void parse(), paren(),brace(),newline(),redo(),tab(),asterisk(),space();

static struct bind_Description mtextBindings[]={
    {"mtextview-paren",")",')', NULL,0, 0, paren, "Insert a paren, with balancing."},
    {"mtextview-bar","|",'|',NULL,0,0,space,""},
    {"mtextview-period",".",'.',NULL,0,0,space,""},
    {"mtextview-comma",",",',',NULL,0,0,space,""},
    {"mtextview-paren","]",']'},
    {"mtextview-brace","}",'}', NULL,0, 0, brace,"Insert a brace indented properly with balancing."},
    {"mtextview-asterisk","*",'*', NULL,0, 0, asterisk,""},
    {"mtextview-space"," ",' ', NULL,0, 0, space,""},
    {"mtextview-left-paren","(",'(', NULL,0, 0, space,""},
    {"mtextview-semicolon",";",';', NULL,0, 0, space,""},
    {"mtextview-redo-styles","\033r",0,"Modula-2 Text,Redo M-2 styles~10", 0,0, redo, "Wrap styles around comments and keywords in Modula-2 code."},
    {"mtextview-redo-styles","\033R",0},
    {"mtextview-tab", "\t", 0, "Modula-2 Text, Indent line/region~11", 0,0, tab, "Indents the current line or selection region."},
    {"mtextview-newline", "\n", 0, NULL,0, 0, newline," Inserts a newline and indents properly."},
    NULL
};
boolean mtextview__InitializeClass(classID)
struct classheader *classID;
{
    M_Menus = menulist_New();
    M_Map = keymap_New();

    bind_BindList(mtextBindings,M_Map,M_Menus,&mtextview_classinfo);
    return TRUE;
}

boolean mtextview__InitializeObject(classID, self)
struct classheader *classID;
struct mtextview *self;
{
    self->c_state = keystate_Create(self, M_Map);
    self->c_menus = menulist_DuplicateML(M_Menus, self);
    mtextview_SetBorder(self,5,5); 
    return TRUE;
}

long mtextview__SetBounceTime(classID, time)
struct classheader *classID;
long time;
{
    long retval = BounceTime;

    BounceTime = time;

    return retval;
}


void mtextview__PostKeyState(self, keystate)
struct mtextview *self;
struct keystate *keystate;
{
    keystate_AddBefore(self->c_state, keystate);
    super_PostKeyState(self, self->c_state);
}

void mtextview__PostMenus(self, menulist)
struct mtextview *self;
struct menulist *menulist;
{
    menulist_ChainAfterML(self->c_menus, menulist, 0);
    super_PostMenus(self, self->c_menus);

}

static void redo(self)
struct mtextview *self;
{
    struct mtext *c = (struct mtext *)self->header.view.dataobject;

    mtext_RedoStyles(c);
    mtext_RegionModified(c, 0, mtext_GetLength(c));
    mtext_NotifyObservers(c, 0);
}

static void match_parens(self, key)
struct mtextview *self;
char key;
{
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    long start = mtextview_GetDotPosition(self), openparen = mtext_ReverseBalance(ct, start), pos;
    char buffer[256], *ptr;
    int i;

    if (openparen != EOF)
    {
	if (!mtextview_Visible(self, openparen)) {

	    for (pos = openparen; pos > 0; pos--)
	    {
		if (mtext_GetChar(ct, pos) == '\n')
		{
		    pos++;
		    break;
		}
	    }

	    ptr = buffer;
	    for (i = sizeof(buffer) - 1; i > 0; i--)
		if ((*ptr++ = mtext_GetChar(ct, pos++)) == '\n')
		    break;
	    *ptr = NULL;

	    message_DisplayString(self, 0, buffer);
	}
	else /* if (BounceTime == 0) */
	{
	    mtextview_SetDotPosition(self, openparen);
	    mtextview_SetDotLength(self, start - openparen);
	}
	/* else Deal with bounce */
    }
    else
	switch (key)
	{
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
struct mtextview *self;
char key; /* must be char for "&" to work. */
{
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    int count = im_Argument(mtextview_GetIM(self)), i, oldpos,pos;
    mtextview_CollapseDot(self);
    oldpos=pos = mtextview_GetDotPosition(self);
    
    for (i = 0; i < count; i++, pos++)
	mtext_InsertCharacters(ct, pos, &key, 1);
    if(oldpos&&(mtext_GetChar(ct,oldpos-1)=='*')&&(mtext_GetStyle(ct,oldpos-1)==ct->comment_style)) {
	if(environment_Remove(ct->header.text.rootEnvironment,oldpos,2,environment_Style,FALSE)) mtext_SetModified(ct);
    }
    mtextview_SetDotPosition(self, pos);
    mtextview_FrameDot(self, pos);

    match_parens(self, key);

    mtext_NotifyObservers(ct, 0);
}

static void brace(self, key)
struct mtextview *self;
char key; /* must be char for "&" to work. */
{
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    int count = im_Argument(mtextview_GetIM(self)), i, pos = mtextview_GetDotPosition(self);

    mtextview_CollapseDot(self);
    pos = mtextview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	mtext_InsertCharacters(ct, pos, &key, 1);

    mtextview_SetDotPosition(self, pos);
    mtextview_FrameDot(self, pos);

    mtext_ReindentLine(ct,pos);

    match_parens(self, key);

    mtext_NotifyObservers(ct, 0);
}

static void asterisk(self, key)
struct mtextview *self;
char key; /* must be char for "&" to work. */
{
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    int count = im_Argument(mtextview_GetIM(self)), i,oldpos, pos = mtextview_GetDotPosition(self);

    mtextview_CollapseDot(self);
    oldpos=pos = mtextview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	mtext_InsertCharacters(ct, pos, &key, 1);
    if(oldpos) {
	if(mtext_GetChar(ct,oldpos-1)=='(') {
	    mtext_InsertNewStyle(ct,oldpos,count+1, ct->comment_style,TRUE,FALSE);
	    
	}
    }
    mtextview_SetDotPosition(self, pos);
    
    mtext_NotifyObservers(ct, 0);
}

static void tab(self, key)
struct mtextview *self;
long key;
{
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    int pos = mtextview_GetDotPosition(self), len = mtextview_GetDotLength(self), c = 0;
    struct mark *mark=mtext_CreateMark(ct,pos,len);

    if(len>0)
	mtext_Indent(ct,mark);
    else
    {
	int	oldPos=pos;

	do
	    pos--;
	while (pos>=0 && (c=mtext_GetChar(ct,pos))!='\n' && isspace(c));

	if(pos<0 || c=='\n')	/* indent line */
	{
	    mark_SetPos(mark,pos+1);
	    mtextview_SetDotPosition(self,mtext_Indent(ct,mark));
	}
	else		/* insert whitespace to next column */
	    mtextview_SetDotPosition(self,mtext_TabAndOptimizeWS(ct,oldPos,4));
    }

    mtext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    mtext_NotifyObservers(ct,0);
}
static void space(self,key)
struct mtextview *self;
char key;
{
    int oldpos,pos,count=im_Argument(mtextview_GetIM(self));
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    mtextview_CollapseDot(self);
    oldpos=pos=mtextview_GetDotPosition(self);
    while(count--) mtext_InsertCharacters(ct,pos++,&key,1);
    if(oldpos) mtext_BackwardCheckWord(ct,oldpos-1,0);
    mtextview_SetDotPosition(self,pos);
    mtext_NotifyObservers(ct,0);
}
    
static void newline(self, key)
struct mtextview *self;
long key;
{
    int newlines = im_Argument(mtextview_GetIM(self));
    struct mtext *ct = (struct mtext *)self->header.view.dataobject;
    int c;
    long pos,end;
    struct mark *mark;

    mtextview_CollapseDot(self);
    pos=mtextview_GetDotPosition(self);
    if(pos) mtext_BackwardCheckWord(ct,pos-1,0);
    mtext_ReindentLine(ct,pos);
    end=pos=mtextview_GetDotPosition(self); /* may have changed with ReindentLine */
    while(pos>0 && ((c=mtext_GetChar(ct,pos-1))==' ' || c=='\t'))
	pos--;
    if(pos<end)
	mtext_DeleteCharacters(ct,pos,end-pos);

    while(newlines--)
	mtext_InsertCharacters(ct,pos++,"\n",1);
    

    mark=mtext_CreateMark(ct,pos,0);

    mtextview_SetDotPosition(self,mtext_Indent(ct,mark));
    mtext_RemoveMark(ct,mark);
    mark_Destroy(mark);
    mtext_NotifyObservers(ct,0);
}
