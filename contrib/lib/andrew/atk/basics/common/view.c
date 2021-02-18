/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/view.c,v 2.11 1991/09/12 16:00:34 bobg Exp $ */
/* $ACIS:view.c 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/view.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/common/RCS/view.c,v 2.11 1991/09/12 16:00:34 bobg Exp $";
#endif /* lint */



#include <class.h>
#include <view.eh>

#include <atom.ih>
#include <atomlist.ih>
#include <dataobj.ih>
#include <fontdesc.ih>
#include <graphic.ih>
#include <describe.ih>

#define min(v1,v2) ((v1)<(v2) ? (v1) : (v2))
#define view_STARTHEIGHT 150
struct atom * A_name;
struct atom * A_atomlist;
struct atom * A_context;

boolean view__InitializeClass(classID)
    struct classheader *classID;
{
  A_name = atom_Intern("name");
  A_context = atom_Intern("context");
  A_atomlist = atom_Intern("atomlist");
  return TRUE;
}

boolean view__InitializeObject(classID, self)
    struct classheader *classID;
    struct view *self;
{
    self->imPtr = NULL;
    self->parent = NULL;
    self->dataobject = NULL;
    self->drawable = graphic_CreateGraphic();
    self->name = NULL;
    self->className = NULL;
    self->name_explicitly_set = FALSE;
    return TRUE;
}

void view__FinalizeObject(classID, self)
    struct classheader *classID;
    struct view *self;
{
    if (self->dataobject != NULL)  {
	dataobject_RemoveObserver(self->dataobject, self);
    }
    if (self->name != NULL) {
	atomlist_Destroy(self->name);
	atomlist_Destroy(self->className);
    }
    if (self->drawable != NULL)  {
	graphic_Destroy(self->drawable);
    }
}

void view__ObservedChanged(self, changed, value)
struct view *self;
struct observable *changed;
long value;
{
    if (changed == (struct observable *) self->dataobject)  {
	if (value == observable_OBJECTDESTROYED)
	    self->dataobject = NULL;
	else
	    view_WantUpdate(self, self);
    }
}

void view__SetDataObject(self, dataobject)
struct view *self;
struct dataobject *dataobject;
{
    struct atomlist *newname;
    struct atomlist *context;

    if(self->dataobject==dataobject)
	return;

    if (self->dataobject != NULL)
	dataobject_RemoveObserver(self->dataobject, self);

    self->dataobject=dataobject;
    if (dataobject != NULL)
	dataobject_AddObserver(dataobject, self);

    if (!self->name_explicitly_set)
	if (dataobject != NULL && 
	    dataobject_Get(dataobject, A_name, &A_atomlist, (long *) &newname))
	    self->name = atomlist_Copy(newname);
	else if (self->name == NULL)
	    self->name = atomlist_StringToAtomlist(class_GetTypeName(self));

    if (self->className == NULL)
	self->className = atomlist_StringToAtomlist(class_GetTypeName(self));

    if (dataobject != NULL && 
	 dataobject_Get(dataobject, A_context, &A_atomlist, (long *) &context))
    {
	atomlist_JoinToBeginning(self->name, context);
	atomlist_JoinToBeginning(self->className, context);
    }
}

struct view *view__GetApplicationLayer(self)
    struct view *self;
{
    return self;
}

void view__DeleteApplicationLayer(self,applicationLayer)
    struct view *self;
    struct view *applicationLayer;
{
}


char * view__DescriptionObject(self,format,rock)
    struct view *self;
    char * format;
    long rock; /* supposed to be an aribtrarily pointer */
{
#define	MaxObjName 128
    /* The default is to take the name of the view, and if it is appended by the string "view", replace "view" with format, otherwise append format. If we don't have any format, then use "describer" as the format */

    static char tmpString[MaxObjName];	    /* Name of describer object */
    char * viewName;		    /* Name of current view */
    short viewLen;		    /* Length of view name */
    short formatLen;		    /* Length of format description */
    short viewCopyAmount, formatCopyAmount; /* temps for building describer name */

    /* First see if we have a format name, if not, then we will just use
       "describer" */
    if (!format) format = "describer";
    formatLen = strlen(format);

    viewName = class_GetTypeName(self);
    viewLen = strlen(viewName);
    /* Should we use strncmp or explode the tests? Depends on when we expect to fail */
    if ( (viewLen >= 4) && (viewName[viewLen-4] == 'v') &&
	 (viewName[viewLen-3] == 'i') && (viewName[viewLen-2] == 'e') &&
	 (viewName[viewLen-1] == 'w')) {
	/* We have a cannonical "object||view" name, so replace "view"
	   by the format type */
	viewCopyAmount = min(viewLen-4,MaxObjName-1);
	 }
    else {
	viewCopyAmount = min(viewLen,MaxObjName-1);
    }

    /* Copy over view name (or as much as will fit) */
    strncpy(tmpString,viewName,viewCopyAmount);

    /* See how much of format name can fit */
    if ((viewCopyAmount + formatLen) > (MaxObjName - 1))
	    formatCopyAmount = (MaxObjName - 1) - viewCopyAmount;
    else formatCopyAmount = formatLen;

    /* Copy over format (or as  much as will fit) */
    strncpy(tmpString+viewCopyAmount,format,formatCopyAmount);

    /* Terminate the string */
    tmpString[viewCopyAmount+formatCopyAmount] = NULL;

    /* Note: we reuse the space, hope there are not multiple calls */
    return tmpString;
}

enum view_DescriberErrs view__Describe(self,format,file,rock)
    struct view * self;
    char * format;
    FILE * file;
    long rock;
{
    struct describer * descObject;
    char * descObjectName;

    /* Get the name of the object to be used for format conversion */
    descObjectName = view_DescriptionObject(self,format,rock);

    /* Make sure we have the name of the description object */
    if (!descObjectName) return(view_NoDescribeString);

    /* Try to load the object */
    descObject = (struct describer *) class_NewObject(descObjectName);

    /* Make sure we got the object */
    if (!descObject) return(view_NoDescribeObject);

    /* Check to see if the class is correct */
    /* How is this done now adays? Do I have to walk up the classinfo tree myself */
    if (!class_IsTypeByName(descObjectName,"describer")) 
	return(view_WrongSubclassDescription);

    /* Everything seems to be in order, call the description procedure */
    return describer_Describe(descObject,self,format,file,rock);

}

void view__FullUpdate(self, type, left, top, width, height)
    struct view *self;
    enum view_UpdateType type;
    long left;
    long top;
    long width;
    long height;
{
}

void view__Update(self)
    struct view *self;
{
}

void view__Print(self, file, processor, finalFormat, topLevel)
    struct view *self;
    FILE *file;
    char *processor;
    char *finalFormat;
    boolean topLevel;
{
}

struct view *view__Hit(self, action, x, y, numberOfClicks)
    struct view *self;
    enum view_MouseAction action;
    long x;
    long y;
    long numberOfClicks;
{
    return self;
}

enum view_DSattributes view__DesiredSize(self, width, height, pass, dWidth, dHeight)
     struct view *self;
     long width;
     long height;
     enum view_DSpass pass;
     long *dWidth;
     long *dHeight;
{
    *dWidth = width;
    *dHeight = (height > 2048) ? view_STARTHEIGHT :height;
    return view_HeightFlexible | view_WidthFlexible;
}

void view__GetOrigin(self, width, height, originX, originY)
    struct view *self;
    long width;
    long height;
    long *originX;
    long *originY;
{
    *originX = 0;
    *originY = 0;
}

void view__ReceiveInputFocus(self)
    struct view *self;
{
    if(self->parent!=NULL){
	view_PostMenus(self,NULL);
	view_PostKeyState(self,NULL);
    }
}

void view__LoseInputFocus(self)
    struct view *self;
{
}

void view__WantUpdate(self, requestor)
    struct view *self;
    struct view *requestor;
{
    if (self->parent != NULL)
	view_WantUpdate(self->parent, requestor);
}

void view__WantInputFocus(self, requestor)
    struct view *self;
    struct view *requestor;
{
    if (self->parent != NULL)
	view_WantInputFocus(self->parent, requestor);
}

void view__WantNewSize(self, requestor)
    struct view *self;
    struct view *requestor;
{
/* This function tree climbs one level only. */
    if (self == requestor && self->parent != NULL)
        view_WantNewSize(self->parent, requestor);
}

struct basicobject * view__WantHandler(self, handlerName)
    struct view *self;
    char *handlerName;
{
    if (self->parent != NULL)
	return view_WantHandler(self->parent, handlerName);
    else
	return NULL;
}

char *view__WantInformation(self, key)
    struct view *self;
    char *key;
{
    if (self->parent != NULL)
	return view_WantInformation(self->parent, key);
    else
	return NULL;
}

void view__PostKeyState(self, keystate)
    struct view *self;
    struct keystate *keystate;
{
    if (self->parent != NULL)
	view_PostKeyState(self->parent, keystate);
}

void view__PostMenus(self, menulist)
    struct view *self;
    struct menulist *menulist;
{
    if (self->parent != NULL)
	view_PostMenus(self->parent, menulist);
}

void view__RetractCursor(self, cursor)
    struct view *self;
    struct cursor *cursor;
{
    if (self->parent != NULL)
	view_RetractCursor(self->parent, cursor);
}
void view__RetractViewCursors(self, requestor)
    struct view *self;
    struct view *requestor;
{
    if (self->parent != NULL)
	view_RetractViewCursors(self->parent, requestor);
}
void view__PostCursor(self,rec, cursor)
    struct view *self;
    struct rectangle *rec;
    struct cursor *cursor;
{
    if (self->parent != NULL)
	view_PostCursor(self->parent, rec, cursor);
}

void view__PostDefaultHandler(self, handlerName, handler)
    struct view *self;
    char *handlerName;
    struct basicobject *handler;
{
    if (self->parent != NULL)
	view_PostDefaultHandler(self->parent, handlerName, handler);
}

void view__InsertViewRegion(self, parent, region)
struct view *self;
struct view *parent;
struct region *region;
{
    graphic_InsertGraphicRegion(self->drawable, parent->drawable, region);
    self->imPtr = parent->imPtr;
    self->parent = parent;
}

void view__InsertView(self, parent, enclosingRectangle)
    struct view *self, *parent;
    struct rectangle *enclosingRectangle;
{

    graphic_InsertGraphic(self->drawable,parent->drawable,enclosingRectangle);
    self->imPtr = parent->imPtr;
    self->parent = parent;
}

void view__InsertViewSize(self,parent,xOriginInParent,yOriginInParent, width, height)
struct view *self, *parent;
long xOriginInParent, yOriginInParent, width, height;
{

    graphic_InsertGraphicSize(self->drawable,parent->drawable,xOriginInParent,yOriginInParent, width, height);
    self->imPtr = parent->imPtr;
    self->parent = parent;
}

void view__InsertGraphic(self, parent, enclosingRectangle)
    struct view *self, *parent;
    struct rectangle *enclosingRectangle;
{
    view_InsertView(self,parent,enclosingRectangle);
}

char *view__GetInterface(self, type)
    struct view *self;
    char *type;
/* Note: This routine is a placeholder for a future function. It is needed now to get scrollbars to work. - William Lott. */
{
    return NULL;
}

void view__LinkTree(self, parent)
    struct view *self;
    struct view *parent;
{

    if (parent != NULL) {
        graphic_InsertGraphicSize(self->drawable,parent->drawable, 0, 0, 0, 0);
        self->imPtr = parent->imPtr;
        self->parent = parent;
    }
    else {
        self->imPtr = NULL;
        rectangle_SetRectSize(&self->drawable->localBounds, 0, 0, 0, 0);
    }
}

void view__UnlinkTree(self)
    struct view *self;
{
    struct view *parent = self->parent;

    view_LinkTree(self, NULL);
    self->parent = NULL;
    if (parent != NULL)
        view_UnlinkNotification(parent, self);
}

void view__UnlinkNotification(self, unlinkedTree)
    struct view *self;
    struct view *unlinkedTree;
{

    if (self->parent != NULL)
        view_UnlinkNotification(self->parent, unlinkedTree);
}

boolean view__IsAncestor(self, possibleAncestor)
    struct view *self;
    struct view *possibleAncestor;
{
    while (self != NULL && self != possibleAncestor)
        self = self->parent;
    return self != NULL;
}

char * view__GetWindowManagerType(self)
struct view * self;{
    if ((self)->drawable) return graphic_GetWindowManagerType((self)->drawable);
    return "";
}

long view__GetDevice(self)
struct view * self;{
    if ((self)->drawable) return graphic_GetDevice((self)->drawable);
    return 0;
}

static void EnsureName(self)
     struct view * self;
{
  if (self->name == NULL)
    self->name = atomlist_StringToAtomlist(class_GetTypeName(self));
  if (self->className == NULL)
    self->className = atomlist_StringToAtomlist(class_GetTypeName(self));
}

void view__SetName( self, name )
     struct view * self;
     struct atomlist * name;
{
  self->name = atomlist_Copy(name);
  self->name_explicitly_set = TRUE;
}

struct atomlist * view__GetName( self )
     struct view * self;
{
  EnsureName(self);
  return self->name;
}

struct atomlist * view__GetClass( self )
     struct view * self;
{
  EnsureName(self);
  return self->className;
}

short view__GetParameter( self, name, type, data )
     struct view * self;
     struct atomlist * name;
     struct atom * type;
     long * data;
{
  struct atomlist * dup;
  short val;
  dup = atomlist_Copy(name);
  val = view_GetResource( self, name, dup, type, data );
  atomlist_Destroy(dup);
  return val;
}


short view__GetResource( self, name, class, type, data )
     struct view * self;
     struct atomlist * name;
     struct atomlist * class;
     struct atom * type;
     long * data;
{
  struct atoms * nameMark = atomlist_Mark( name );
  struct atoms * classMark = atomlist_Mark( class );
  short gotit = FALSE;

  EnsureName(self);

  if (self->parent != NULL)
    {
      atomlist_JoinToBeginning( name, self->name );
      atomlist_JoinToBeginning( class, self->className );
      gotit = view_GetResource(self->parent, name, class, type, data );
      atomlist_Cut( name, nameMark );
      atomlist_Cut( class, classMark );
    }
  return gotit;
}


void view__GetManyParameters( self, resources, name, class )
     struct view * self;
     struct resourceList * resources;
     struct atomlist * name;
     struct atomlist * class;
{
  struct atoms * nameMark = NULL;
  struct atoms * classMark = NULL;
  struct atomlist * passname;
  struct atomlist * passclass;
  EnsureName(self);
  if (self->parent != NULL)
    {
      if (name == NULL)
	passname = self->name;
      else
	{
	  nameMark = atomlist_Mark(name);
	  atomlist_JoinToBeginning(name,self->name);
	  passname = name;
	}
      if (class == NULL)
	passclass = self->className;
      else
	{
	  classMark = atomlist_Mark(class);
	  atomlist_JoinToBeginning(class,self->className);
	  passclass = class;
	}
      view_GetManyParameters( self->parent, resources, passname, passclass );
      if (name != NULL)
	atomlist_Cut(name,nameMark);
      if (class != NULL)
	atomlist_Cut(class,classMark);
    }
}

void view__PostResource( self, path, type, data )
     struct view * self;
     struct atomlist * path;
     struct atom * type;
     long data;
{
  struct atoms * pathMark = atomlist_Mark( path );
  EnsureName(self);
  if (self->parent != NULL)
    {
      atomlist_JoinToBeginning( path, self->name );
      view_PostResource( self->parent, path, type, data );
      atomlist_Cut( path, pathMark );
    }
}
void view__InitChildren(self)
struct view *self;
{
    /*
      All parent views are responsible for overriding this method.
      This is a request to instantiate all one's children.
      i.e. insure that all child views 
      1. exist and
      2. are linked into the view tree.
 
      view_InitChild() should be called recursively on all children so 
	  this call filters down the view tree. 
    */
}
boolean view__CanView(self,TypeName)
struct view *self;
char *TypeName;
{
    /* 
      Views should return TRUE or FALSE depending on whether they are
      capable of viewing dataobjects of the specified name.
      Currently returns TRUE by default for backward compatibility.
	  */
    return TRUE;
}
