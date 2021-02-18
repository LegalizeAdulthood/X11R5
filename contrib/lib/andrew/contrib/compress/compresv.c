/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/compress/RCS/compresv.c,v 1.6 1991/09/12 16:49:54 bobg Exp $
*/

/*RSKmod butchered from 'fnotev.c'*/

#include <class.h>
#include <text.ih>
#include <textv.ih>
#include <view.ih>
#include <im.ih>
#include <rect.h>
#include <fontdesc.ih>
#include <cursor.ih>
#include <message.ih>
#include <ctype.h>
#include <environ.ih>
#include <compress.ih>
#include <envrment.ih>
#include <txttroff.ih>
#include <mark.ih>    /*RSKadd*/
#include "compresv.eh"

static char rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/compress/RCS/compresv.c,v 1.6 1991/09/12 16:49:54 bobg Exp $";
static char CopyRight[] = " Copyright (c) IBM Corp. 1989";

#define FONTNAME "andysans"
#define FONTSIZE 8
#define DataObject(A) (A->header.view.dataobject)
#define Text(A) ((struct text *) DataObject(A))
#define Compress(A) ((struct compress *) DataObject(A))
#define Graphic(A) (((struct view *)(A))->drawable)
#define CANTSWITCHTEXTVIEW 1 /* textview currently doesn't allow itself to switch to a new text */
static struct compressv *firstcompress=NULL;/*RSKadd*/
static struct envlist *firstenv=NULL;/*RSKadd*/

void compressv__GetOrigin(self, width, height, originX, originY)
    struct view *self;
    long width;
    long height;
    long *originX;
    long *originY;
{
    *originX = 0;
    *originY = height;
}

/* addenv(e) adds the view/environment into the text object that will be initialized when it's visible in the window */
void addenv(e)    /*RSKadd*/
    struct envlist *e;
    {
    e->envr=text_AddView(e->cp->parenttext, mark_GetPos(e->cp->cpmark), "compressv",e->cp);
    compress_NotifyObservers(e->cp,0);
    text_NotifyObservers(e->txt,0);
    }

/* removeenv(e) removes the embedded view, getting rid if the \377 characters that remain after the initialized views have been destroyed */
void removeenv(e)    /*RSKadd*/
    struct envlist *e;
    {
    long pos=mark_GetPos(e->cp->cpmark)-1;
    if (text_GetChar(e->txt,pos)>127)
	text_DeleteCharacters(e->txt,pos,1);
    /*environment_Remove(e->envr,mark_GetPos(e->cp->cpmark), mark_GetLength(e->cp->cpmark),environment_Any,TRUE);*/
    /*environment_Destroy(e->envr);*/
    e->envr=NULL;
    compress_NotifyObservers(e->cp,0);
    text_RegionModified(e->txt,pos,1);
    text_NotifyObservers(e->txt,0);
    }

/* RemoveBox(e,removeEnvToo) takes e out of the envlist linked list, and removes the embedded view if so requested */
void RemoveBox(e,removeEnvToo)    /*RSKadd*/
    struct envlist *e;
    boolean removeEnvToo;
    {
    if (removeEnvToo) removeenv(e);
    e->prevenv->nextenv=e->nextenv;
    e->nextenv->prevenv=e->prevenv;
    if (e==firstenv)
	{
	if (e->nextenv!=firstenv)
	    firstenv=e->nextenv;
	else
	    firstenv=NULL;
	}
    free(e);
    }

/* decompress(self,key) destroys an embedded view that has been initialized and restores the compressed text back to normal */
void decompress(self,key)    /*RSKadd*/
    struct compressv *self;
    long key;
    {
    struct compress *fn=(struct compress *)(self->header.view.dataobject);
    compress_Decompress(fn,self->parenttext);
    compressv_Destroy(self);
    RemoveBox(fn->envl,FALSE);
    }
    
struct view *compressv__Hit(self,action,mousex,mousey,numberOfClicks) 
struct compressv *self;
enum view_MouseAction action;
long mousex, mousey, numberOfClicks;
{
    if(action == view_LeftUp){
	decompress(self,0);
    }	
    return (struct view *) self;
}

/* DecompressAll(self) first destroys all the views that have been initialized, then removes all the environments that haven't been initialized as views yet */
void compressv__DecompressAll(classID)    /*RSKadd*/
    struct classheader *classID;
    {
    while (firstcompress!=NULL)
	decompress(firstcompress,0);
    while (firstenv!=NULL)
	{
	compress_Decompress(firstenv->cp,firstenv->cp->parenttext);
	RemoveBox(firstenv,TRUE);
	}
    }

/* MakeBox(self,cprs) adds a node to the envlist linked list, which will actually be put into the text on the next call to UnhideBoxes();  this assumes that HideBoxes() has been called previously to avoid problems with nested (and hence hidden) views leaving their cursor descriptions in the text object's window */
void compressv__MakeBox(classID,cprs)    /*RSKadd*/
    struct classheader *classID;
    struct compress *cprs;
    {
    struct envlist *newenv=(struct envlist *)malloc(sizeof(struct envlist));
    cprs->envl=newenv;
    /*newenv->envr=text_AddView(cprs->parenttext,mark_GetPos(cprs->cpmark), "compressv",cprs);*//*let UnhideBoxes do this*/
    newenv->txt=cprs->parenttext;
    newenv->cp=cprs;

    if (firstenv==NULL)/*RSKadd*/
	{
	firstenv=newenv;
	firstenv->nextenv=newenv;
	firstenv->prevenv=newenv;
        }
    else
	{
	newenv->prevenv=firstenv->prevenv;
	newenv->nextenv=firstenv;
	firstenv->prevenv->nextenv=newenv;
	firstenv->prevenv=newenv;
        }
    compress_NotifyObservers(cprs,0);
    }

/* HideBoxes(self) destroys all the views that have been initialized and removes the uninitialized environments, but leaves the envlist linked list intact so that the boxes can be easily restored.  This should be used by the parent text's Write routine to prevent the boxes from being saved to disk as \377 characters. */
void compressv__HideBoxes(classID)    /*RSKadd*/
    struct classheader *classID;
    {
    while (firstcompress!=NULL) compressv_Destroy(firstcompress);
    if (firstenv!=NULL)
    {
	struct envlist *e;
        removeenv(firstenv);
	e=firstenv->nextenv;
	while (e!=firstenv)
	    {
            removeenv(e);
	    e=e->nextenv;
	    }
	}
    }

/* UnhideBoxes(self) goes through the envlist linked list and puts all the boxes back.  Any that are visible in the parent textview's window will be initialized into views automatically. */
void compressv__UnhideBoxes(classID)    /*RSKadd*/
    struct classheader *classID;
    {
    if (firstenv!=NULL)
    {
	struct envlist *e;
	addenv(firstenv);
	e=firstenv->nextenv;
	while (e!=firstenv)
	    {
	    addenv(e);
	    e=e->nextenv;
	    }
	}
    }

enum view_DSattributes compressv__DesiredSize(self, width, height, pass, desiredwidth, desiredheight)
struct compressv *self;
long width, height;
enum view_DSpass pass;
long *desiredwidth, *desiredheight;
{
    *desiredwidth = 120;/*80 to fit "compressed text" in box*/
    *desiredheight = FONTSIZE+4;
    return(view_HeightFlexible | view_WidthFlexible);
}
static DoUpdate(self,full)
struct compressv *self;
boolean full;
{
/*  struct compress *fn=(struct compress *)(self->header.view.dataobject);
    if (compress_IsHidden(fn,fn->parenttext,mark_GetPos(fn->cpmark))) return;*/
    struct rectangle enclosingRect;
    char boxmsg[256];/*RSKadd*/
    int numlines;
    enclosingRect.top = 0; enclosingRect.left = 0;
    enclosingRect.width  = compressv_GetLogicalWidth(self) -2 ;
    enclosingRect.height = compressv_GetLogicalHeight(self) -2 ;
    compressv_SetTransferMode(self,graphic_WHITE);
    compressv_EraseRect(self,&(enclosingRect));

    compressv_SetTransferMode(self,graphic_INVERT);
    enclosingRect.width++;
    enclosingRect.height++;
    compressv_DrawRect(self,&(enclosingRect));

    enclosingRect.width++; enclosingRect.height++;/*RSKadd*/
    compressv_PostCursor(self,&(enclosingRect),self->cursor);
    compressv_SetFont(self,self->fd);
    compressv_MoveTo(self,0,1);

    numlines = compress_GetLines((struct compress*) (self->header.view.dataobject));
    if (numlines > 1 )
	sprintf(boxmsg," compressed %d lines\0", numlines);/*RLQmod*/
    else
	sprintf(boxmsg," compressed %d line\0", numlines);/*RLQmod*/
    compressv_DrawString(self,boxmsg,(view_ATTOP | view_ATLEFT));/*RSKmod*/

}
void compressv__LinkTree(self, parent)
register struct compressv *self;
struct view *parent;
{
    super_LinkTree(self,parent);
    while(!class_IsTypeByName(class_GetTypeName(parent),"textview")){
	if((parent = parent->parent) == NULL ) return;
    }
    self->parenttext = (struct text *) view_GetDataObject(parent);
    self->parentview = (struct textview *) parent;
}
void compressv__FullUpdate(self,type,left,top,width,height)
struct compressv *self;
enum view_UpdateType type;
long left,top,width,height;
{
    DoUpdate(self,TRUE);
}
void compressv__Update(self)
struct compressv *self;
{
    DoUpdate(self,FALSE);
}

/* InitializeObject(classID,self) adds the compress view to the compressv linked list, so the whole lot can be destroyed later when doing a Save or Decompress All */
boolean compressv__InitializeObject(classID,self)
struct classheader *classID;
struct compressv *self;
{
    self->fd = fontdesc_Create(FONTNAME,0,FONTSIZE);
    self->cursor = cursor_Create(self);
    cursor_SetStandard(self->cursor,Cursor_CrossHairs);
    self->parenttext = NULL;
    if (firstcompress==NULL)/*RSKadd*/
	{
	firstcompress=self;
	firstcompress->nextcompress=self;
	firstcompress->prevcompress=self;
        }
    else
	{
	self->prevcompress=firstcompress->prevcompress;
	self->nextcompress=firstcompress;
	firstcompress->prevcompress->nextcompress=self;
	firstcompress->prevcompress=self;
        }
    return TRUE;
}

/* FinalizeObject(classID,self) removes the compress view from the compressv linked list */
void compressv__FinalizeObject(classID,self)    /*RSKadd*/
    struct classheader *classID;
    struct compressv *self;
    {
    self->parenttext=NULL;
    self->parentview=NULL;
    self->fd=NULL;
    if (self->cursor!=NULL) cursor_Destroy(self->cursor);
    self->prevcompress->nextcompress=self->nextcompress;
    self->nextcompress->prevcompress=self->prevcompress;
    if (self==firstcompress)
	{
	if (self->nextcompress!=firstcompress)
	    firstcompress=self->nextcompress;
	else
	    firstcompress=NULL;
        }
    }
boolean compressv__InitializeClass(classID)
struct classheader *classID;
{
    firstcompress=NULL;/*RSKadd*/
    return TRUE;
}
