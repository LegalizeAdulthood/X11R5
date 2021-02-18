/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3textv.c,v 1.8 1991/09/12 16:50:42 bobg Exp $
*/

/* M3textView, a ``Modula-3'' mode for BE2. */
#include <ctype.h>

#include <andrewos.h>
#include "class.h"

#include "im.ih"
#include "message.ih"
#include "m3text.ih"
#include "keymap.ih"
#include "keystate.ih"
#include "menulist.ih"
#include "proctbl.ih"
#include "bind.ih"
#include "envrment.ih"
#include "m3textv.eh"
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

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/m3text/RCS/m3textv.c,v 1.8 1991/09/12 16:50:42 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

static long BounceTime = 0;
static struct keymap *M_Map;
static struct menulist *M_Menus;

void parse(), paren(), brace(), vbar(), newline(), redo(), tab(), asterisk(), space(), compress(), compressAll(), decompressAll(), interface(), module();

static struct bind_Description m3textBindings[]={
    {"m3textview-paren",")",')', NULL,0, 0, paren, "Insert a paren, with balancing."},
    {"m3textview-bar","|",'|',NULL,0,0,vbar,""},/*RSKmod*/
    {"m3textview-period",".",'.',NULL,0,0,space,""},
    {"m3textview-comma",",",',',NULL,0,0,space,""},
    {"m3textview-retrn","\015",'\n',NULL,0,0,space,""},/*RSKadd*/
    {"m3textview-paren","]",']'},
    {"m3textview-brace","}",'}', NULL,0, 0, brace,"Insert a brace indented properly with balancing."},
    {"m3textview-asterisk","*",'*', NULL,0, 0, asterisk,""},
    {"m3textview-space"," ",' ', NULL,0, 0, space,""},
    {"m3textview-left-paren","(",'(', NULL,0, 0, space,""},
    {"m3textview-semicolon",";",';', NULL,0, 0, space,""},
    {"m3textview-redo-styles","\033r",0,"Modula-3 Text,Redo M-3 styles~10", 0,0, redo, "Wrap styles around comments and keywords in Modula-3 code."},
    {"m3textview-redo-styles","\033R",0},
    {"m3textview-tab", "\t", 0, "Modula-3 Text,Indent line/region~11", 0,0, tab, "Indents the current line or selection region."},
    {"m3textview-newline", "\n", 0, NULL,0, 0, newline,"Inserts a newline and indents properly."},
    {"m3textview-compress","\033o",0,"Modula-3 Text,Compress region~20", 0,0, compress, "Compresses a region of text for viewing general outline."},
    {"m3textview-compress-all","\033O",0,"Modula-3 Text,Compress all inner indents~21", 0,0, compressAll, "Compresses all lines indented (as far or) farther than the current line."},
    {"m3textview-decompress-all","\033U",0,"Modula-3 Text,Decompress all~25", 0,0, decompressAll, "Expands all compressed regions in document to normal size"},
    {"m3textview-display-interface","\033m",0,"Modula-3 Text,Display Interface~30", 0,0, interface, "Finds the interface where selected identifier is declared."},
    {"m3textview-display-module","\033M",0,"Modula-3 Text,Display Module~31", 0,0, module, "Finds the module where the procedure's code lies."},
    NULL
};
boolean m3textview__InitializeClass(classID)
struct classheader *classID;
{
    M_Menus = menulist_New();
    M_Map = keymap_New();

    bind_BindList(m3textBindings,M_Map,M_Menus,&m3textview_classinfo);
    return TRUE;
}

boolean m3textview__InitializeObject(classID, self)
struct classheader *classID;
struct m3textview *self;
{
    self->c_state = keystate_Create(self, M_Map);
    self->c_menus = menulist_DuplicateML(M_Menus, self);
    m3textview_SetBorder(self,5,5); 
    return TRUE;
}

long m3textview__SetBounceTime(classID, time)
struct classheader *classID;
long time;
{
    long retval = BounceTime;

    BounceTime = time;

    return retval;
}


void m3textview__PostKeyState(self, keystate)
struct m3textview *self;
struct keystate *keystate;
{
    keystate_AddBefore(self->c_state, keystate);
    super_PostKeyState(self, self->c_state);
}

void m3textview__PostMenus(self, menulist)
struct m3textview *self;
struct menulist *menulist;
{
    menulist_ChainAfterML(self->c_menus, menulist, 0);
    super_PostMenus(self, self->c_menus);

}

static void redo(self)
struct m3textview *self;
{
    struct m3text *c = (struct m3text *)self->header.view.dataobject;
    decompressAll(self,0);/*RSKadd*/
    m3text_RedoStyles(c);
    m3text_RegionModified(c, 0, m3text_GetLength(c));
    m3text_NotifyObservers(c, 0);
}

static void match_parens(self, key)
struct m3textview *self;
char key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    long start = m3textview_GetDotPosition(self), openparen = m3text_ReverseBalance(ct, start), pos;
    char buffer[256], *ptr;
    int i;

    if (openparen != EOF)
    {
	if (!m3textview_Visible(self, openparen)) {

	    for (pos = openparen; pos > 0; pos--)
	    {
		if (m3text_GetChar(ct, pos) == '\n')
		{
		    pos++;
		    break;
		}
	    }

	    ptr = buffer;
	    for (i = sizeof(buffer) - 1; i > 0; i--)
		if ((*ptr++ = m3text_GetChar(ct, pos++)) == '\n')
		    break;
	    *ptr = NULL;

	    message_DisplayString(self, 0, buffer);
	}
	else /* if (BounceTime == 0) */
	{
	    m3textview_SetDotPosition(self, openparen);
	    m3textview_SetDotLength(self, start - openparen);
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

static void paren(self,	key)	    /*RSKmod*/
struct m3textview *self;
char key; /* must be char for "&" to work. */
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    int count = im_Argument(m3textview_GetIM(self)), i, oldpos,pos;
    long start,openparen;/*RSKadd*/
    m3textview_CollapseDot(self);
    oldpos=pos = m3textview_GetDotPosition(self);
    for (i = 0; i < count; i++, pos++)
	m3text_InsertCharacters(ct, pos, &key, 1);
    if(oldpos) m3text_BackwardCheckWord(ct,oldpos-1,0);/*RSKmod copied this line from 'space()' so it will catch keywds/idents such as 'procedure blahblah(i: INTEGER);'*/
    m3textview_SetDotPosition(self, pos);
    m3textview_FrameDot(self, pos);
    start = m3textview_GetDotPosition(self); openparen = m3text_ReverseBalance(ct, start);/*RSKadd*/
    if(oldpos&&(m3text_GetChar(ct,oldpos-1)=='*')&&(m3text_GetStyle(ct,oldpos-1)==ct->comment_style))
    {
	if (m3text_GetStyle(ct,openparen)!=ct->comment_style)/*RSKadd; only stops italicizing if matching "(*" was NOT italicized (not nested)*/
	    if(environment_Remove(ct->header.text.rootEnvironment,oldpos,2,environment_Style,FALSE)) m3text_SetModified(ct);
    }

    match_parens(self, key);

    m3text_NotifyObservers(ct, 0);
}

static void brace(self, key)
struct m3textview *self;
char key; /* must be char for "&" to work. */
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    int count = im_Argument(m3textview_GetIM(self)), i, pos = m3textview_GetDotPosition(self);

    m3textview_CollapseDot(self);
    pos = m3textview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	m3text_InsertCharacters(ct, pos, &key, 1);

    m3textview_SetDotPosition(self, pos);
    m3textview_FrameDot(self, pos);

    m3text_ReindentLine(ct,pos);

    match_parens(self, key);

    m3text_NotifyObservers(ct, 0);
}

static void asterisk(self, key)
struct m3textview *self;
char key; /* must be char for "&" to work. */
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    int count = im_Argument(m3textview_GetIM(self)), i,oldpos, pos = m3textview_GetDotPosition(self);

    m3textview_CollapseDot(self);
    oldpos=pos = m3textview_GetDotPosition(self);

    for (i = 0; i < count; i++, pos++)
	m3text_InsertCharacters(ct, pos, &key, 1);
    if(oldpos) {
	if(m3text_GetChar(ct,oldpos-1)=='(') {
	    m3text_InsertNewStyle(ct,oldpos,count+1, ct->comment_style,TRUE,FALSE);

	}
    }
    m3textview_SetDotPosition(self, pos);

    m3text_NotifyObservers(ct, 0);
}

static void tab(self, key)
struct m3textview *self;
long key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    int pos = m3textview_GetDotPosition(self), len = m3textview_GetDotLength(self), c = 0;
    struct mark *mark=m3text_CreateMark(ct,pos,len);

    if(len>0)
	m3text_Indent(ct,mark);
    else
    {
	int	oldPos=pos;

	do
	    pos--;
	while (pos>=0 && (c=m3text_GetChar(ct,pos))!='\n' && isspace(c));

	if(pos<0 || c=='\n')	/* indent line */
	{
	    mark_SetPos(mark,pos+1);
	    m3textview_SetDotPosition(self,m3text_Indent(ct,mark));
	}
	else		/* insert whitespace to next column */
	    m3textview_SetDotPosition(self,m3text_TabAndOptimizeWS(ct,oldPos,4));
    }

    m3text_RemoveMark(ct,mark);
    mark_Destroy(mark);

    m3text_NotifyObservers(ct,0);
}
static void space(self,key)
struct m3textview *self;
char key;
{
    int oldpos,pos,count=im_Argument(m3textview_GetIM(self));
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    m3textview_CollapseDot(self);
    oldpos=pos=m3textview_GetDotPosition(self);
    while(count--) m3text_InsertCharacters(ct,pos++,&key,1);
    if(oldpos) m3text_BackwardCheckWord(ct,oldpos-1,0);
    m3textview_SetDotPosition(self,pos);
    m3text_NotifyObservers(ct,0);
}

static void vbar(self,key)    /*RSKadd*/
struct m3textview *self;
char key;
{
    int oldpos,pos,count=im_Argument(m3textview_GetIM(self));
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    m3textview_CollapseDot(self);
    oldpos=pos=m3textview_GetDotPosition(self);
    while(count--) m3text_InsertCharacters(ct,pos++,&key,1);
    if(oldpos) m3text_BackwardCheckWord(ct,oldpos-1,0);
    m3textview_SetDotPosition(self,pos);
    m3text_ReindentLine(ct,oldpos);
    m3text_NotifyObservers(ct,0);
}

static void newline(self, key)
struct m3textview *self;
long key;
{
    int newlines = im_Argument(m3textview_GetIM(self));
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    int c;
    long pos,end;
    struct mark *mark;

    m3textview_CollapseDot(self);
    pos=m3textview_GetDotPosition(self);
    if(pos) m3text_BackwardCheckWord(ct,pos-1,0);
    m3text_ReindentLine(ct,pos);
    end=pos=m3textview_GetDotPosition(self); /* may have changed with ReindentLine */
    while(pos>0 && ((c=m3text_GetChar(ct,pos-1))==' ' || c=='\t'))
	pos--;
    if(pos<end)
	m3text_DeleteCharacters(ct,pos,end-pos);

    while(newlines--)
	m3text_InsertCharacters(ct,pos++,"\n",1);


    mark=m3text_CreateMark(ct,pos,0);

    m3textview_SetDotPosition(self,m3text_Indent(ct,mark));
    m3text_RemoveMark(ct,mark);
    mark_Destroy(mark);
    m3text_NotifyObservers(ct,0);
}

static void compress(self, key)    /*RSKadd*/
struct m3textview *self;
long key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    struct compress *fn;
    long pos,len;

    if (m3text_GetReadOnly(ct))
    {
	message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
    }
    fn = compress_New();
    compressv_HideBoxes();
    pos= m3textview_GetDotPosition(self);
    len= m3textview_GetDotLength(self);
    if (len==0 && m3text_GetChar(ct,pos)=='\n') pos--;
    compress_Compress(fn,ct,pos,len);
    pos=mark_GetPos(fn->cpmark); len=mark_GetLength(fn->cpmark);
    if (compress_IsHidden(fn,ct,pos+1))
    {
	compress_Destroy(fn);
	compressv_UnhideBoxes();
	return;
    }
    /*if (environment_Remove(ct->header.text.rootEnvironment,pos,len, environment_Style,FALSE))*/ 
    m3text_SetModified(ct);
    m3text_AddStyle(ct,pos,len,fn->compressed_style);

    m3textview_SetDotPosition(self,mark_GetPos(fn->cpmark));
    fn->parentview=(struct textview *)(self->header.view.dataobject);
    compressv_MakeBox(fn);

    compressv_UnhideBoxes();
    m3text_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
    m3textview_CollapseDot(self);
    m3text_NotifyObservers(ct,0);
}

static void compressAll(self, key)    /*RSKadd*/
struct m3textview *self;
long key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    struct compress *fn;
    long pos=1, len, oldpos = m3textview_GetDotPosition(self);
    int curInd;

    if (m3text_GetReadOnly(ct))
    {
	message_DisplayString(self, 0, "Cannot compress read-only files.");
	return;
    }
    else{
	struct im *im=(struct im *)view_GetIM(&(self->header.view)); /*RSKadd*/
	struct cursor *cr=cursor_Create(self->header.view); /*RSKadd*/
	cursor_SetStandard(cr,Cursor_Wait); /*RSKadd*/
	im_SetWindowCursor(im,cr); /*RSKadd*/
	if (m3text_GetChar(ct,oldpos)=='\n') oldpos--;
	curInd=compress_CurrentIndentation(ct,oldpos);
	m3textview_CollapseDot(self);
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
		    m3text_AddStyle(ct,pos,len,fn->compressed_style);
		    if (oldpos>pos && oldpos<pos+len)
			m3textview_SetDotPosition(self,pos);
		    compressv_MakeBox(fn);
		    m3text_RegionModified(ct,mark_GetPos(fn->cpmark)-1, mark_GetLength(fn->cpmark)+1);
		}
		pos+=mark_GetLength(fn->cpmark)+1;
	    }
	    else compress_Destroy(fn);
	}
	compressv_UnhideBoxes();
	m3text_NotifyObservers(ct,0);
	im_SetWindowCursor(im,NULL); /*RSKadd*/
    }
}

static void decompressAll(self, key)    /*RSKadd*/
struct m3textview *self;
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
struct m3textview *view;
{
    return frame_Enumerate(ViewEqual, view);
}

/* Find a window other that the one that contains this inset.  Create one if we have to. */
static struct view *PutInAnotherWindow(view, buffer, forceWindow)
struct m3textview *view;
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
	    fprintf(stderr,"m3text: Could not create new window.\n");
	    return(NULL);
	}
	if((frame = frame_Create(buffer)) == NULL) {
	    fprintf(stderr,"m3text: Could not allocate enough memory.\n");
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
struct m3textview *self;
char *bufname,*proc,*filename;
{
    struct view *view=(struct view *)(m3textview_GetIM(self)->topLevel);
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
		    struct m3textview *m3newv=(struct m3textview *)newview;
		    m3textview_SetDotPosition(m3newv,mark_GetPos(loc));
		    m3textview_SetDotLength(m3newv,mark_GetLength(loc));
		    m3textview_FrameDot(m3newv,mark_GetPos(loc));
		}
	    }
	}
    }
}

boolean fileExists(self, filename)    /*RSKadd*/
struct m3textview *self;
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

boolean findInterfaceOrModule(self, name, proc, module)    /*RSKadd*/
struct m3textview *self;
char *name,*proc;
boolean module;
{
    char path[1024],bufname[256];
    int pos=0,oldpos=0;

    /*find buffer name and filename*/
    strcpy(bufname,name);
    if (module)
	strcat(bufname,"-module\0");
    else
	strcat(bufname,"-interface\0");
    strcat(name,".m\0");
    /*check current working directory*/
    strcpy(path,"./\0"); strcat(path,name);
    if (fileExists(self,path))
    {
	putFileIntoNewWindow(self,bufname,proc,path);
	return TRUE;
    }
    else {
	/*check M3Path*/
	char m3path[1024];
	strcpy(m3path,getenv("M3Path"));
	if (m3path!=NULL && strlen(m3path)>0 && m3path[0]!=' ')
	{
	    while (pos<strlen(m3path))
	    {
		while (pos<strlen(m3path) && m3path[pos]!=':') pos++;
		strncpy(path,m3path+oldpos,pos-oldpos);
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
	/*check DEFAULT_SY_DIR*/
#ifdef DEFAULT_SY_DIR
	strcpy(path,DEFAULT_SY_DIR); strcat(path,"/\0"); strcat(path,name);
	if (fileExists(self,path))
	{
	    putFileIntoNewWindow(self,bufname,proc,path);
	    return TRUE;
	}
#else /*DEFAULT_SY_DIR*/
	message_DisplayString(self, 0, "Your Makefile should define a DEFAULT_SY_DIR.");
#endif /*DEFAULT_SY_DIR*/
	return FALSE;
    }
}

boolean isalnum_orperiod(ch)
char ch;
{
    return (m3text_IsAlnum(ch)||(ch=='.'));
}

static void interface(self, key)    /*RSKadd*/
struct m3textview *self;
long key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    long pos,oldpos;
    char name[256],proc[256];

    /*extract interface name from document*/
    m3textview_CollapseDot(self);
    pos=oldpos=m3textview_GetDotPosition(self);
    while (pos>0 && isalnum_orperiod(m3text_GetChar(ct,pos-1))) pos--;
    oldpos=pos;
    while (m3text_IsAlnum(name[pos-oldpos]=m3text_GetChar(ct,pos)) && pos-oldpos<256) pos++;
    name[pos-oldpos]='\0';
    /*extract procedure name if present*/
    if (m3text_GetChar(ct,pos)=='.')
    {
	oldpos= ++pos;
	while (m3text_IsAlnum(proc[pos-oldpos]=m3text_GetChar(ct,pos)) && pos-oldpos<256) pos++;
	proc[pos-oldpos]='\0';
    }
    else
	proc[0]='\0';
    if (strlen(name)>0)
    {
	if (!findInterfaceOrModule(self,name,proc,FALSE))
	{
	    char msg[300];
	    sprintf(msg, "Couldn't find %s.", name);
	    message_DisplayString(self, 0, msg);
	}
    }
    else
	message_DisplayString(self, 0, "Cursor must be on an import name.");
    m3text_NotifyObservers(ct, 0);
}

static void module(self, key)    /*RSKadd*/
struct m3textview *self;
long key;
{
    struct m3text *ct = (struct m3text *)self->header.view.dataobject;
    long pos,oldpos;
    char name[256],proc[256];

    /*extract module name from document*/
    m3textview_CollapseDot(self);
    pos=oldpos=m3textview_GetDotPosition(self);
    while (pos>0 && isalnum_orperiod(m3text_GetChar(ct,pos-1))) pos--;
    oldpos=pos;
    while (m3text_IsAlnum(name[pos-oldpos]=m3text_GetChar(ct,pos)) && pos-oldpos<256) pos++;
    name[pos-oldpos]='\0';
    /*extract procedure name if present*/
    if (m3text_GetChar(ct,pos)=='.')
    {
	oldpos= ++pos;
	while (m3text_IsAlnum(proc[pos-oldpos]=m3text_GetChar(ct,pos)) && pos-oldpos<256) pos++;
	proc[pos-oldpos]='\0';
    }
    else
	proc[0]='\0';
    if (strlen(name)>0)
    {
	strcat(name,"Impl\0");
	if (!findInterfaceOrModule(self,name,proc,TRUE))
	{
	    char msg[300];
	    sprintf(msg, "Couldn't find %s.", name);
	    message_DisplayString(self, 0, msg);
	}
    }
    else
	message_DisplayString(self, 0, "Cursor must be on a module name.");
    m3text_NotifyObservers(ct, 0);
}
