/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtextv.c,v 1.8 1991/09/12 16:54:18 bobg Exp $ */
/* $ACIS:rmtextview.c 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtextv.c,v $ */

/* RMtextView, a ``Modula-2'' mode for BE2. (R-> Rochester contribution) */

#include <andrewos.h>
#include <ctype.h>

#include "class.h"

#include "im.ih"
#include "message.ih"
#include "rmtext.ih"
#include "keymap.ih"
#include "keystate.ih"
#include "menulist.ih"
#include "proctbl.ih"
#include "bind.ih"
#include "envrment.ih"
#include "rmtextv.eh"
#include "textv.ih"
#include "frame.ih"    /*RSKadd*/
#include "framecmd.ih"    /*RSKadd*/
#include "rect.h"    /*RSKadd*/
#include "view.ih"    /*RSKadd*/
#include "cursor.ih"    /*RSKadd*/
#include "buffer.ih"    /*RSKadd*/
#include "content.ih"    /*RSKadd*/
#include "compress.ih"    /*RSKadd*/
#include "compresv.ih"    /*RSKadd*/

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/rmtext/RCS/rmtextv.c,v 1.8 1991/09/12 16:54:18 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

static long BounceTime = 0;
static struct keymap *M_Map;
static struct menulist *M_Menus;

void parse(), paren(), brace(), vbar(), newline(), redo(), tab(), asterisk(), space(), compress(), compressAll(), decompressAll(), definition(), implementation();
void forceupperon(), forceupperoff();    /*D43K addition*/

static struct bind_Description rmtextBindings[]={
    {"rmtextview-paren",")",')', NULL,0, 0, paren, "Insert a paren, with balancing."},
    {"rmtextview-bar","|",'|',NULL,0,0,vbar,""},/*RSKmod*/
    {"rmtextview-period",".",'.',NULL,0,0,space,""},
    {"rmtextview-comma",",",',',NULL,0,0,space,""},
    {"rmtextview-retrn","\015",'\n',NULL,0,0,space,""},/*RSKadd*/
    {"rmtextview-paren","]",']'},
    {"rmtextview-brace","}",'}', NULL,0, 0, brace,"Insert a brace indented properly with balancing."},
    {"rmtextview-asterisk","*",'*', NULL,0, 0, asterisk,""},
    {"rmtextview-space"," ",' ', NULL,0, 0, space,""},
    {"rmtextview-left-paren","(",'(', NULL,0, 0, space,""},
    {"rmtextview-semicolon",";",';', NULL,0, 0, space,""},
    {"rmtextview-redo-styles","\033r",0,"Modula-2 Text,Redo M-2 styles~10", 0,0, redo, "Wrap styles around comments and keywords in Modula-2 code."},
    {"rmtextview-redo-styles","\033R",0},
    {"rmtextview-tab", "\t", 0, "Modula-2 Text,Indent line/region~11", 0,0, tab, "Indents the current line or selection region."},
    {"rmtextview-newline", "\n", 0, NULL,0, 0, newline,"Inserts a newline and indents properly."},
    {"rmtextview-compress","\033o",0,"Modula-2 Text,Compress region~20", 0,0, compress, "Compresses a region of text for viewing general outline."},
    {"rmtextview-compress-all","\033O",0,"Modula-2 Text,Compress all inner indents~21", 0,0, compressAll, "Compresses all lines indented (as far or) farther than the current line."},
    {"rmtextview-decompress-all","\033U",0,"Modula-2 Text,Decompress all~25", 0,0, decompressAll, "Expands all compressed regions in document to normal size"},
    {"rmtextview-display-definition","\033m",0,"Modula-2 Text,Display Definition~30", 0,0, definition, "Finds the definition module where selected identifier is declared."},
    {"rmtextview-display-implementation","\033M",0,"Modula-2 Text,Display Implementation~31", 0,0, implementation, "Finds the implementation module where the procedure's code lies."},
     NULL
};
boolean rmtextview__InitializeClass(classID)
struct classheader *classID;
{
    M_Menus = menulist_New();
    M_Map = keymap_New();

    bind_BindList(rmtextBindings,M_Map,M_Menus,&rmtextview_classinfo);
    return TRUE;
}

boolean rmtextview__InitializeObject(classID, self)
struct classheader *classID;
struct rmtextview *self;
{
    self->c_state = keystate_Create(self, M_Map);
    self->c_menus = menulist_DuplicateML(M_Menus, self);
    rmtextview_SetBorder(self,5,5); 
    return TRUE;
}

long rmtextview__SetBounceTime(classID, time)
struct classheader *classID;
long time;
{
    long retval = BounceTime;

    BounceTime = time;

    return retval;
}


void rmtextview__PostKeyState(self, keystate)
struct rmtextview *self;
struct keystate *keystate;
{
    keystate_AddBefore(self->c_state, keystate);
    super_PostKeyState(self, self->c_state);
}

void rmtextview__PostMenus(self, menulist)
struct rmtextview *self;
struct menulist *menulist;
{
    menulist_ChainAfterML(self->c_menus, menulist, 0);
    super_PostMenus(self, self->c_menus);

}

static void redo(self)
struct rmtextview *self;
{
    struct rmtext *c = (struct rmtext *)self->header.view.dataobject;
    decompressAll(self,0);/*RSKadd*/
    rmtext_RedoStyles(c);
    rmtext_RegionModified(c, 0, rmtext_GetLength(c));
    rmtext_NotifyObservers(c, 0);
}

static void match_parens(self, key)
struct rmtextview *self;
char key;
{
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    long start = rmtextview_GetDotPosition(self), openparen = rmtext_ReverseBalance(ct, start), pos;
    char buffer[256], *ptr;
    int i;

    if (openparen != EOF)
    {
	if (!rmtextview_Visible(self, openparen)) {

	    for (pos = openparen; pos > 0; pos--)
	    {
		if (rmtext_GetChar(ct, pos) == '\n')
		{
		    pos++;
		    break;
		}
	    }

	    ptr = buffer;
	    for (i = sizeof(buffer) - 1; i > 0; i--)
		if ((*ptr++ = rmtext_GetChar(ct, pos++)) == '\n')
		    break;
	    *ptr = NULL;

	    message_DisplayString(self, 0, buffer);
	}
	else /* if (BounceTime == 0) */
	{
	    rmtextview_SetDotPosition(self, openparen);
	    rmtextview_SetDotLength(self, start - openparen);
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
struct rmtextview *self;
char key; /* must be char for "&" to work. */
{
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    int count = im_Argument(rmtextview_GetIM(self)), i, oldpos,pos;
    long start,openparen;/*RSKadd*/
    rmtextview_CollapseDot(self);
    oldpos=pos = rmtextview_GetDotPosition(self);
    for (i = 0; i < count; i++, pos++)
	rmtext_InsertCharacters(ct, pos, &key, 1);
    if(oldpos) rmtext_BackwardCheckWord(ct,oldpos-1,0);/*RSKmod copied this line from 'space()' so it will catch keywds/idents such as 'procedure blahblah(i: INTEGER);'*/
    rmtextview_SetDotPosition(self, pos);
    rmtextview_FrameDot(self, pos);
    start = rmtextview_GetDotPosition(self); openparen = rmtext_ReverseBalance(ct, start);/*RSKadd*/
    if(oldpos&&(rmtext_GetChar(ct,oldpos-1)=='*')&&(rmtext_GetStyle(ct,oldpos-1)==ct->comment_style))
    {
	if (rmtext_GetStyle(ct,openparen)!=ct->comment_style)/*RSKadd; only stops italicizing if matching "(*" was NOT italicized (not nested)*/
	    if(environment_Remove(ct->header.text.rootEnvironment,oldpos,2,environment_Style,FALSE)) rmtext_SetModified(ct);
    }
    match_parens(self, key);

    rmtext_NotifyObservers(ct, 0);
}

static void brace(self, key)
struct rmtextview *self;
char key; /* must be char for "&" to work. */
{
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    int count = im_Argument(rmtextview_GetIM(self)), i, pos = rmtextview_GetDotPosition(self);

    rmtextview_CollapseDot(self);
    pos = rmtextview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	rmtext_InsertCharacters(ct, pos, &key, 1);

    rmtextview_SetDotPosition(self, pos);
    rmtextview_FrameDot(self, pos);

    rmtext_ReindentLine(ct,pos);

    match_parens(self, key);

    rmtext_NotifyObservers(ct, 0);
}

static void asterisk(self, key)
struct rmtextview *self;
char key; /* must be char for "&" to work. */
{
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    int count = im_Argument(rmtextview_GetIM(self)), i,oldpos, pos = rmtextview_GetDotPosition(self);

    rmtextview_CollapseDot(self);
    oldpos=pos = rmtextview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	rmtext_InsertCharacters(ct, pos, &key, 1);
    if(oldpos) {
	if(rmtext_GetChar(ct,oldpos-1)=='(') {
	    rmtext_InsertNewStyle(ct,oldpos,count+1, ct->comment_style,TRUE,FALSE);
	    
	}
    }
    rmtextview_SetDotPosition(self, pos);
    
    rmtext_NotifyObservers(ct, 0);
}

static void tab(self, key)
struct rmtextview *self;
long key;
{
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    int pos = rmtextview_GetDotPosition(self), len = rmtextview_GetDotLength(self), c = 0;
    struct mark *mark=rmtext_CreateMark(ct,pos,len);

    if(len>0)
	rmtext_Indent(ct,mark);
    else
    {
	int	oldPos=pos;

	do
	    pos--;
	while (pos>=0 && (c=rmtext_GetChar(ct,pos))!='\n' && isspace(c));

	if(pos<0 || c=='\n')	/* indent line */
	{
	    mark_SetPos(mark,pos+1);
	    rmtextview_SetDotPosition(self,rmtext_Indent(ct,mark));
	}
	else		/* insert whitespace to next column */
	    rmtextview_SetDotPosition(self,rmtext_TabAndOptimizeWS(ct,oldPos,4));
    }

    rmtext_RemoveMark(ct,mark);
    mark_Destroy(mark);

    rmtext_NotifyObservers(ct,0);
}
static void space(self,key)
struct rmtextview *self;
char key;
{
    int oldpos,pos,count=im_Argument(rmtextview_GetIM(self));
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    rmtextview_CollapseDot(self);
    oldpos=pos=rmtextview_GetDotPosition(self);
    while(count--) rmtext_InsertCharacters(ct,pos++,&key,1);
    if(oldpos) rmtext_BackwardCheckWord(ct,oldpos-1,0);
    rmtextview_SetDotPosition(self,pos);
    rmtext_NotifyObservers(ct,0);
}
    
static void vbar(self,key)    /*RSKadd*/
struct rmtextview *self;
char key;
{
    int oldpos,pos,count=im_Argument(rmtextview_GetIM(self));
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    rmtextview_CollapseDot(self);
    oldpos=pos=rmtextview_GetDotPosition(self);
    while(count--) rmtext_InsertCharacters(ct,pos++,&key,1);
    if(oldpos) rmtext_BackwardCheckWord(ct,oldpos-1,0);
    rmtextview_SetDotPosition(self,pos);
    rmtext_ReindentLine(ct,oldpos);
    rmtext_NotifyObservers(ct,0);
}

static void newline(self, key)
struct rmtextview *self;
long key;
{
    int newlines = im_Argument(rmtextview_GetIM(self));
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    int c;
    long pos,end;
    struct mark *mark;

    rmtextview_CollapseDot(self);
    pos=rmtextview_GetDotPosition(self);
    if(pos) rmtext_BackwardCheckWord(ct,pos-1,0);
    rmtext_ReindentLine(ct,pos);
    end=pos=rmtextview_GetDotPosition(self); /* may have changed with ReindentLine */
    while(pos>0 && ((c=rmtext_GetChar(ct,pos-1))==' ' || c=='\t'))
	pos--;
    if(pos<end)
	rmtext_DeleteCharacters(ct,pos,end-pos);

    while(newlines--)
	rmtext_InsertCharacters(ct,pos++,"\n",1);
    

    mark=rmtext_CreateMark(ct,pos,0);

    rmtextview_SetDotPosition(self,rmtext_Indent(ct,mark));
    rmtext_RemoveMark(ct,mark);
    mark_Destroy(mark);
    rmtext_NotifyObservers(ct,0);
}

/* Set the data object's data item forceUpper to TRUE and */
/* change the menu list entry to Force Upper Off */
static void forceupperon(self, rock)    /*DHGadd*/
struct rmtextview *self;
long rock; /* required by key bindings, but not used */
{
    struct proctable_Entry *proc;

    rmtext_SetForceUpper((struct rmtext *)self->header.view.dataobject,TRUE);

       /* delete existing menu item */
    menulist_DeleteFromML(self->c_menus, "Modula-2 Text,Force Upper On~40");

       /* define the procedure for the new menu option */
    proc = proctable_DefineProc("rmtextview-force-upper-off", forceupperoff, &rmtextview_classinfo, NULL, "Turn off auto upper casing of keywords");

       /* add Force Upper Off option to M2 Text menu card */
    menulist_AddToML(self->c_menus, "Modula-2 Text,Force Upper Off~40", proc, 0, 0);

    rmtextview_WantInputFocus(self, self);
}

/* Set the data object's data item forceUpper to FALSE and */
/* change the menu list entry to Force Upper On */
static void forceupperoff(self, rock)    /*DHGadd*/
struct rmtextview *self;
long rock; /* required by key bindings, but not used */
{
    struct proctable_Entry *proc;

    rmtext_SetForceUpper((struct rmtext *)self->header.view.dataobject,FALSE);

       /* delete existing menu item */
    menulist_DeleteFromML(self->c_menus, "Modula-2 Text,Force Upper Off~40");

       /* define the procedure for the new menu option */
    proc = proctable_DefineProc("rmtextview-force-upper-on", forceupperon, &rmtextview_classinfo, NULL, "Turn on auto upper casing of keywords");

       /* add Force Upper On option to M2 Text menu card */
    menulist_AddToML(self->c_menus, "Modula-2 Text,Force Upper On~40", proc, 0, 0);

    rmtextview_WantInputFocus(self, self);
}
/*-- Override-- */
/* Associate the view with the data object, and */
/* initialize the force upper menu option */
void rmtextview__SetDataObject( self, dataobj )    /*DHGadd*/
struct rmtextview *self;
struct rmtext *dataobj;
{
    /*struct proctable_Entry *proc;*/

    /* call overriden SetDataObject */
    super_SetDataObject( self, dataobj );

    if ( rmtext_GetForceUpper( dataobj ) )
	forceupperon(self,0);
    else
	forceupperoff(self,0);
}

static void compress(self, key)    /*RSKadd*/
    struct rmtextview *self;
    long key;
    {
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    struct compress *fn;
    long pos,len;

    if (rmtext_GetReadOnly(ct))
	{
        message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
	}
    fn = compress_New();
    compressv_HideBoxes();
    pos = rmtextview_GetDotPosition(self);
    len = rmtextview_GetDotLength(self);
    if (len==0 && rmtext_GetChar(ct,pos)=='\n') pos--;
    compress_Compress(fn,ct,pos,len);
    pos=mark_GetPos(fn->cpmark); len=mark_GetLength(fn->cpmark);
    if (compress_IsHidden(fn,ct,pos+1))
	{
	compress_Destroy(fn);
	compressv_UnhideBoxes();
	return;
	}
    /*if (environment_Remove(ct->header.text.rootEnvironment,pos,len, environment_Style,FALSE))*/ 
        rmtext_SetModified(ct);
    rmtext_AddStyle(ct,pos,len,fn->compressed_style);

    rmtextview_SetDotPosition(self,mark_GetPos(fn->cpmark));
    fn->parentview=(struct textview *)(self->header.view.dataobject);
    compressv_MakeBox(fn);

    compressv_UnhideBoxes();
    rmtext_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
    rmtextview_CollapseDot(self);
    rmtext_NotifyObservers(ct,0);
    }

static void compressAll(self, key)    /*RSKadd*/
    struct rmtextview *self;
    long key;
    {
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    struct compress *fn;
    long pos=1, len, oldpos = rmtextview_GetDotPosition(self);
    int curInd;

    struct im *im=(struct im *)view_GetIM(&(self->header.view)); /*RSKadd*/
    struct cursor *cr=cursor_Create(self->header.view); /*RSKadd*/
    if (rmtext_GetReadOnly(ct))
	{
        message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
	}
    cursor_SetStandard(cr,Cursor_Wait); /*RSKadd*/
    im_SetWindowCursor(im,cr); /*RSKadd*/
    if (rmtext_GetChar(ct,oldpos)=='\n') oldpos--;
    curInd=compress_CurrentIndentation(ct,oldpos);
    rmtextview_CollapseDot(self);
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
		rmtext_AddStyle(ct,pos,len,fn->compressed_style);
		if (oldpos>pos && oldpos<pos+len)
		    rmtextview_SetDotPosition(self,pos);
		compressv_MakeBox(fn);
		rmtext_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
		}
	    pos+=mark_GetLength(fn->cpmark)+1;
	    }
	else compress_Destroy(fn);
	}
    compressv_UnhideBoxes();
    rmtext_NotifyObservers(ct,0);
    im_SetWindowCursor(im,NULL); /*RSKadd*/
    }

static void decompressAll(self, key)    /*RSKadd*/
    struct rmtextview *self;
    long key;
    {
    compressv_DecompressAll();
    }

/*-----------------RSKmod copied from 'compile.c'-------------------*/
struct finderInfo {
    struct frame *myFrame, *otherFrame, *bestFrame;
    struct buffer *myBuffer;
};

boolean rectangle_IsEmptyRect(/*TestedRectangle*/stupid) /*RSKmod added this procedure because the linker said 'doload: Undefined symbol: _rectangle_IsEmptyRect' even though I included 'rect.h'*/
    struct rectangle *stupid;
    {
    return FALSE; /*I don't know why, it just sounds safer than TRUE*/
    }

static boolean FrameFinder(frame, info)
    struct frame *frame;
    struct finderInfo *info;
{

    struct rectangle bogus;

    if (info->myFrame == frame)
        return FALSE;
    if (info->otherFrame != NULL && info->bestFrame != NULL)
	return TRUE;
    frame_GetVisualBounds(frame, &bogus);
    if (!rectangle_IsEmptyRect(&bogus))
        if (frame_GetBuffer(frame) == info->myBuffer) {
            info->bestFrame = frame;
            return FALSE;
        }
        else {
            info->otherFrame = frame;
            return FALSE;
        }
    return FALSE;
}

ViewEqual(frame, view)
    struct frame *frame;
    struct view *view;
    {
#if 1
    return (frame_GetView(frame) == view);
#else /* 1 */
    return (view_GetIM((struct view *) frame) == view_GetIM(view))
#endif /* 1 */
    }

struct frame *FindByView(view)
    struct rmtextview *view;
    {
    return frame_Enumerate(ViewEqual, view);
    }

/* Find a window other that the one that contains this inset.  Create one if we have to. */
static struct view *PutInAnotherWindow(view, buffer, forceWindow)
    struct rmtextview *view;
    struct buffer *buffer;
    int forceWindow;
{

    boolean FrameFinder();
    struct frame *frame;
    struct finderInfo myInfo;

    myInfo.myFrame = FindByView(view);
    myInfo.otherFrame = NULL;
    myInfo.bestFrame = NULL;
    myInfo.myBuffer = buffer;
    frame_Enumerate(FrameFinder, &myInfo);
    frame = (myInfo.bestFrame != NULL) ? myInfo.bestFrame : ((myInfo.otherFrame != NULL) ? myInfo.otherFrame : NULL);
    if (frame == NULL) {

        struct im *newIM;

        if (!forceWindow)
            return NULL;
        if((newIM = im_Create(NULL)) == NULL) {
	    fprintf(stderr,"rmtext: Could not create new window.\n");
	    return(NULL);
	}
        if((frame = frame_Create(buffer)) == NULL) {
	    fprintf(stderr,"rmtext: Could not allocate enough memory.\n");
	    if(newIM) im_Destroy(newIM);
	    return(NULL);
	}
        im_SetView(newIM, frame);

/* This is here because the frame_Create call can't set the input focus
 * because the frame didn't have a parent when it called view_WantInputFocus.
 * This is bogus but hard to fix...
 */
        view_WantInputFocus(frame_GetView(frame), frame_GetView(frame));
    }
    else if (frame_GetBuffer(frame) != buffer)
	frame_SetBuffer(frame, buffer, TRUE);
    return frame_GetView(frame);
}
/*------------------------------------------------------*/

long tcpos(txt,strng)
    struct content *txt;
    char *strng;
    {
    long pos=0, end=content_GetLength(txt);
    char buf[256];
    while (pos<end)
	{
	int i=0;
	while (i<256 && pos+i<end && (buf[i]=content_GetChar(txt,pos+i))!='\n')
	    i++;
	buf[i]='\0';
	if (strcmp(strng,buf)==0)
	    return pos;
	pos+=i+1;
	}
    return -1;
    }
    
void putFileIntoNewWindow(self, bufname, proc, filename)    /*RSKadd*/
    struct rmtextview *self;
    char *bufname,*proc,*filename;
    {
    struct view *view=(struct view *)(rmtextview_GetIM(self)->topLevel);
    struct buffer *bf;
    struct view *newview;
    static struct content *TofC=NULL;
    struct mark *loc;
    if ((bf=buffer_FindBufferByFile(filename))==NULL)
	{
	bf=buffer_GetBufferOnFile(filename,FALSE);
	if (bf)
	    {
	    buffer_ReadFile(bf,filename);
	    buffer_SetName(bf,bufname);
	    }
	}
    if (bf)
	{
	if (view->dataobject!=buffer_GetData(bf)) /*not viewing Int/Mod already*/
	    if((newview=PutInAnotherWindow(self,bf,TRUE)) == NULL)
		return;
	if (proc && strlen(proc)>0)
	    {
	    if (!TofC)
		TofC=content_New();
	    if (TofC)
		{
		long pos;
		content_SetSourceText(TofC,buffer_GetData(bf));
		pos=tcpos(TofC,proc);
		if (pos<0) return;
		loc=content_locate(TofC, pos);
		if (loc)
		    {
		    struct rmtextview *mnewv=(struct rmtextview *)newview;
		    rmtextview_SetDotPosition(mnewv,mark_GetPos(loc));
		    rmtextview_SetDotLength(mnewv,mark_GetLength(loc));
		    rmtextview_FrameDot(mnewv,mark_GetPos(loc));
		    }
		}
	    }
	}
    }

boolean fileExists(self, filename)    /*RSKadd*/
    struct rmtextview *self;
    char *filename;
    {
    FILE *fp;
    if ((fp=fopen(filename,"r"))==NULL)
	return FALSE;
    else
	{
	fclose(fp);
	return TRUE;
        }
    }

boolean findDefinitionOrImplementation(self, name, proc, implementation)    /*RSKadd*/
    struct rmtextview *self;
    char *name,*proc;
    boolean implementation;
    {
    char path[1024],bufname[256];
    int pos=0,oldpos=0;
    char mpath[1024];

    /*find buffer name and filename*/
    strcpy(bufname,name);
    if (implementation)
	{
	strcat(bufname,"-implementation\0");
	strcat(name,".mod\0");
	}
    else
	{
	strcat(bufname,"-definition\0");
	strcat(name,".def\0");
	}
    /*check current working directory*/
    strcpy(path,"./\0"); strcat(path,name);
    if (fileExists(self,path))
	{
	putFileIntoNewWindow(self,bufname,proc,path);
	return TRUE;
        }
    /*check MPath (environment variable)*/
    strcpy(mpath,getenv("MPath"));
    if (mpath!=NULL && strlen(mpath)>0 && mpath[0]!=' ')
        {
	while (pos<strlen(mpath))
	    {
	    while (pos<strlen(mpath) && mpath[pos]!=':') pos++;
	    strncpy(path,mpath+oldpos,pos-oldpos);
	    path[pos-oldpos]='\0';
	    oldpos= ++pos;
	    strcat(path,"/\0"); strcat(path,name);
	    if (fileExists(self,path))
	        {
		putFileIntoNewWindow(self,bufname,proc,path);
		return TRUE;
	        }
	    }
	}
    /*check m2mi (environment variable)*/
    strcpy(mpath,getenv("m2mi"));
    pos=oldpos=0;
    if (mpath!=NULL && strlen(mpath)>0 && mpath[0]!=' ')
        {
	while (pos<strlen(mpath))
	    {
	    while (pos<strlen(mpath) && mpath[pos]!=':') pos++;
	    strncpy(path,mpath+oldpos,pos-oldpos);
	    path[pos-oldpos]='\0';
	    oldpos= ++pos;
	    strcat(path,"/\0"); strcat(path,name);
	    if (fileExists(self,path))
	        {
		putFileIntoNewWindow(self,bufname,proc,path);
		return TRUE;
	        }
	    }
	}
    /*check m2mp (environment variable)*/
    strcpy(mpath,getenv("m2mp"));
    pos=oldpos=0;
    if (mpath!=NULL && strlen(mpath)>0 && mpath[0]!=' ')
        {
	while (pos<strlen(mpath))
	    {
	    while (pos<strlen(mpath) && mpath[pos]!=':') pos++;
	    strncpy(path,mpath+oldpos,pos-oldpos);
	    path[pos-oldpos]='\0';
	    oldpos= ++pos;
	    strcat(path,"/\0"); strcat(path,name);
	    if (fileExists(self,path))
	        {
		putFileIntoNewWindow(self,bufname,proc,path);
		return TRUE;
	        }
	    }
	}
    /*check DEFAULT_SYM_DIR*/
#ifdef DEFAULT_SYM_DIR
    strcpy(path,DEFAULT_SYM_DIR); strcat(path,"/\0"); strcat(path,name);
    if (fileExists(self,path))
	{
	putFileIntoNewWindow(self,bufname,proc,path);
	return TRUE;
        }
#else /*DEFAULT_SYM_DIR*/
    message_DisplayString(self, 0, "Your Makefile should define a DEFAULT_SYM_DIR.");
#endif /*DEFAULT_SYM_DIR*/
    return FALSE;
    }

boolean isalnum_orperiod(ch)
    char ch;
    {
    return (rmtext_IsAlnum(ch)||(ch=='.'));
    }

static void definition(self, key)    /*RSKadd*/
    struct rmtextview *self;
    long key;
    {
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    long pos,oldpos;
    char name[256],proc[256];

    /*extract interface name from document*/
    rmtextview_CollapseDot(self);
    pos=oldpos=rmtextview_GetDotPosition(self);
    while (pos>0 && isalnum_orperiod(rmtext_GetChar(ct,pos-1))) pos--;
    oldpos=pos;
    while (rmtext_IsAlnum(name[pos-oldpos]=rmtext_GetChar(ct,pos)) && pos-oldpos<256) pos++;
    name[pos-oldpos]='\0';
    /*extract procedure name if present*/
    if (rmtext_GetChar(ct,pos)=='.')
	{
	oldpos= ++pos;
	while (rmtext_IsAlnum(proc[pos-oldpos]=rmtext_GetChar(ct,pos)) && pos-oldpos<256) pos++;
	proc[pos-oldpos]='\0';
	}
    else
	proc[0]='\0';
    if (strlen(name)>0)
        {
	if (!findDefinitionOrImplementation(self,name,proc,FALSE))
	    {
	    char msg[300];
	    sprintf(msg, "Couldn't find %s.", name);
	    message_DisplayString(self, 0, msg);
	    }
	}
    else
	message_DisplayString(self, 0, "Cursor must be on a definition name.");
    rmtext_NotifyObservers(ct, 0);
    }

static void implementation(self, key)    /*RSKadd*/
    struct rmtextview *self;
    long key;
    {
    struct rmtext *ct = (struct rmtext *)self->header.view.dataobject;
    long pos,oldpos;
    char name[256],proc[256];

    /*extract module name from document*/
    rmtextview_CollapseDot(self);
    pos=oldpos=rmtextview_GetDotPosition(self);
    while (pos>0 && isalnum_orperiod(rmtext_GetChar(ct,pos-1))) pos--;
    oldpos=pos;
    while (rmtext_IsAlnum(name[pos-oldpos]=rmtext_GetChar(ct,pos)) && pos-oldpos<256) pos++;
    name[pos-oldpos]='\0';
    /*extract procedure name if present*/
    if (rmtext_GetChar(ct,pos)=='.')
	{
	oldpos= ++pos;
	while (rmtext_IsAlnum(proc[pos-oldpos]=rmtext_GetChar(ct,pos)) && pos-oldpos<256) pos++;
	proc[pos-oldpos]='\0';
	}
    else
	proc[0]='\0';
    if (strlen(name)>0)
        {
	if (!findDefinitionOrImplementation(self,name,proc,TRUE))
	    {
	    char msg[300];
	    sprintf(msg, "Couldn't find %s.", name);
	    message_DisplayString(self, 0, msg);
	    }
	}
    else
	message_DisplayString(self, 0, "Cursor must be on an implementation name.");
    rmtext_NotifyObservers(ct, 0);
    }
