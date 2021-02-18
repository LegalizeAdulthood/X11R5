/*********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidsuite_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/apt/suite/RCS/suite.c,v 1.35 1990/06/13 19:01:45 gk5g Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Suite-object

MODULE	suite.c

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Suite-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY

END-SPECIFICATION  ************************************************************/
#include <graphic.ih>
#include <dataobj.ih>
#include <view.ih>
#include <bind.ih>
#include <im.ih>
#include <scroll.ih>
#include <cursor.ih>
#include <fontdesc.ih>
#include <proctbl.ih>
#include <keystate.ih>
#include <keymap.ih>
#include <text.ih>
#include <textv.ih>
#include <message.ih>
#include <apt.h>
#include <apt.ih>
#include <vector.ih>
#include <aptv.ih>
#include <suite.h>
#include <suite.eh>
#include <suitecv.ih>
#include <suiteev.ih>

#define	CurrentItem		    (self->current_item)
#define Apt			    (self->apt)
#define Items			    (self->items)
#define ItemWidth		    (self->itemwidth)
#define ItemHeight		    (self->itemheight)
#define ItemFixedWidth		    (self->itemfixedwidth)
#define ItemFixedHeight		    (self->itemfixedheight)
#define SetView			    (self->setview)
#define Bounds			    (self->bounds)
#define Container		    (self->container)
#define ContainerLeft		    (Container.left)
#define ContainerTop		    (Container.top)
#define ContainerWidth		    (Container.width)
#define ContainerHeight		    (Container.height)
#define Scroll			    (self->scroll)
#define ScrollType		    (self->scrolltype)
#define ScrollTop		    (ScrollType & scroll_TOP)
#define ScrollBottom		    (ScrollType & scroll_BOTTOM)
#define ScrollLeft		    (ScrollType & scroll_LEFT)
#define ScrollRight		    (ScrollType & scroll_RIGHT)
#define HScroll			    (ScrollTop || ScrollBottom)
#define VScroll			    (ScrollLeft || ScrollRight)
#define ScrollRect		    (self->scrollrect)
#define TitleCaption		    (self->title_caption)
#define TitleRect		    (self->title_rect)
#define TitleHighlightStyle	    (self->titlehighlightstyle)
#define TitleBorderStyle	    (self->titleborderstyle)
#define	TitleBorderSize		    (self->titlebordersize)
#define TitleFontName		    (self->titlefontname)
#define TitleFont		    (self->titlefont)
#define TitleFontSize		    (self->titlefontsize)
#define RealTitleFontSize	    (self->realtitlefontsize)
#define TitleFontType		    (self->titlefonttype)
#define	RealTitleFontType	    (self->realtitlefonttype)
#define TitleFontLow		    (self->titlefontlow)
#define TitleFontHigh		    (self->titlefonthigh)
#define ItemTitleFontName	    (self->itemtitlefontname)
#define ItemTitleFontSize	    (self->itemtitlefontsize)
#define RealItemTitleFontSize	    (self->realitemtitlefontsize)
#define ItemTitleFontType	    (self->itemtitlefonttype)
#define ItemTitleFontLow	    (self->itemtitlefontlow)
#define ItemTitleFontHigh	    (self->itemtitlefonthigh)
#define TitleDataObjectName	    (self->titledataobjectname)
#define TitleDataObject		    (self->titledataobject)
#define TitleDataObjectHandler	    (self->titledataobjecthandler)
#define TitlePlacement		    (self->titleplacement)
#define	TitleCaptionAlignment	    (self->titlecaptionalignment)
#define ItemTitlePlacement	    (self->itemtitleplacement)
#define	ItemTitleCaptionAlignment   (self->itemtitlecaptionalignment)
#define TitleHitHandler		    (self->titlehithandler)
#define TitleViewObjectName	    (self->titleviewobjectname)
#define TitleViewObject		    (self->titleviewobject)
#define TitleViewObjectHandler	    (self->titleviewobjecthandler)
#define	TitleHighlighted	    (self->titlehighlighted)
#define ItemOrder		    (self->itemorder)
#define AccessType		    (self->accesstype)
#define Arrangement		    (self->arrangement)
#define	List			    (Arrangement & suite_List)
#define	SingleColumn		    (Arrangement & suite_Column)
#define	SingleRow		    (Arrangement & suite_Row)
#define SelectionMode		    (self->selection_mode)
#define BorderStyle		    (self->borderstyle)
#define BorderSize		    (self->bordersize)
#define CaptionFontName		    (self->captionfontname)
#define CaptionFont		    (self->captionfont)
#define CaptionFontSize		    (self->captionfontsize)
#define RealCaptionFontSize	    (self->realcaptionfontsize)
#define CaptionFontType		    (self->captionfonttype)
#define CaptionFontLow		    (self->captionfontlow)
#define CaptionFontHigh		    (self->captionfonthigh)
#define	CaptionAlignment	    (self->captionalignment)
#define ItemBorderStyle		    (self->itemborderstyle)
#define ItemBorderSize		    (self->itembordersize)
#define ItemHighlightStyle	    (self->itemhighlightstyle)
#define ItemPassiveStyle	    (self->itempassivestyle)
#define ItemDataObjectHandler	    (self->itemdataobjecthandler)
#define ItemViewObjectHandler	    (self->itemviewobjecthandler)
#define ItemDataObjectName	    (self->itemdataobjectname)
#define ItemViewObjectName	    (self->itemviewobjectname)
#define HitHandler		    (self->hithandler)
#define	ClientAnchor		    (self->anchor)
#define Datum			    (self->datum)
#define FirstVisible		    (self->firstvisible)
#define NewFirstVisible		    (self->newfirstvisible)
#define LastVisible		    (self->lastvisible)
#define VisibleRows		    (self->visiblerows)
#define VisibleColumns		    (self->visiblecolumns)
#define Rows			    (self->rows)
#define Columns			    (self->columns)
#define NumVisible		    (self->numvisible)
#define Debug			    (self->debug)
#define ExceptionHandler	    (self->exception)
#define ExceptionStatus		    (self->exception_status)
#define ExceptionItem		    (self->exception_item)
#define SortHandler		    (self->sort_handler)
#define SortOrder		    (self->sortorder)
#define Cursor			    (self->cursor)
#define CursorFont		    (self->cursorfont)
#define CursorByte		    (self->cursorbyte)
#define CursorFontName		    (self->cursorfontname)    
#define ItemCursor		    (self->itemcursor)
#define ItemCursorFont		    (self->itemcursorfont)
#define ItemCursorByte		    (self->itemcursorbyte)
#define ItemCursorFontName	    (self->itemcursorfontname)    
#define ItemArray		    (self->itemarray)
#define YGutterSize		    (self->y_guttersize)
#define XGutterSize		    (self->x_guttersize)
#define TitleMWidth		    (self->title_m_width)
#define CaptionMWidth		    (self->caption_m_width)
#define HasFocus		    (self->has_focus)
#define ITEM(i)			    (struct suite_item *) vector_Item(Items,i)
#define SetTransferMode(v,m)	    if((m) != view_GetTransferMode(((struct view*)(v))))\
					view_SetTransferMode(((struct view*)(v)),(m));
#define SetFont(v,f)		    if((f) != view_GetFont(((struct view*)(v))))\
					view_SetFont(((struct view*)(v)),(f));
#define MAX(a,b)		    (((a)>(b))?(a):(b))
#define MIN(a,b)		    (((a)<(b))?(a):(b))
#define Active(item)		    (item->mode & item_Active)
#define Highlighted(item)	    (item->mode & item_Highlighted)
#define Normalized(item)	    (item->mode & item_Normalized)
#define Exposed(item)		    suite_ItemExposed(self,item)
#define DecrementRect(r,n)	    rectangle_InsetRect(r,n,n)
#define DEFAULT_FONT_RANGE	    4
#define SCROLL_WIDTH		    20
#define WindowSystem		    (self->wm_type)
#define WM			    1
#define IsWM			    (WindowSystem == WM)
#define	NumberLines(str)	     suiteev_LineCount(SetView,str)
#define	WrapStyle		    (self->wrappingstyle)
#define	MaxItemPosGiven		    (self->max_item_pos_given)
#define	IsLinked		    (suite_IsAncestor(self,suite_GetIM(self)))

static struct suite_item	    *GenerateItem();
static void			     SetEltAttrByName();
static void			     SetAttrByName();
static void			     SetEltValByName();
static void			     SetValByName();
static void			     SetItems();
static void			     SetSuiteAttribute();
static void			     SetItemAttribute();
static void			     SetSortRoutine();
static long			     Within();
static char			    *strip();
static void			     AllocNameSpace();
static void			     DrawTitle();
static void			     DrawOutline();
static long			     TitleSectionWidth();
static long			     TitleSectionHeight();
static void			     AssignSetAndTitleSpace();
static void			     PlaceTitle();
static void			     SetCaptionList();
static void			     ParseFontFullName();
static long			     AlphasortAscend();
static long			     NumericAscend();
static long			     AlphasortDescend();
static long			     NumericDescend();
static struct suite_item	    *AllocItem();
static void			     FinalizeItem();
static void			     HandleException();
static void			     DefaultExceptionHandler();
static void			     ValidateItem();
static long			     SortStub();
static void			     DrawRect();

static long
Within( x, y, left, top, width, height )
  register long x, y, left, top, width, height;
{
  return((x >= left) && (x <= left + width) && (y >= top) && (y <= top + height));
}

static long
WithinRect( x, y, r )
  register long			 x, y;
  register struct rectangle	*r;
{
  return(Within(x,y,r->left,r->top,r->width,r->height));
}

static char *
strip( str )
  register char *str;
{
  register char *tmp = NULL, *head = NULL;

  if(!str) return(str);
  tmp = head = str;
  while(*str == 040) str++;
  while(*tmp++ = *str++);
  tmp = head + strlen(head) - 1;
  while(*tmp == 040) tmp--;
  *(tmp+1) = '\0';
  return(head);
}

boolean
suite__InitializeClass( ClassID )
  register struct classheader *ClassID;
{
  proctable_DefineProc("suite-set-items", 
			SetItems, &suite_classinfo,
			"suite", "Set item list (colon separated list)");
  proctable_DefineProc("suite-set-attribute", 
			SetAttrByName, &suite_classinfo,
			"suite", "Set an attribute for suite");
  proctable_DefineProc("suite-set-item-attribute", 
			SetEltAttrByName, &suite_classinfo, 
			"suite", "Set an attribute for one item of a suite");
  proctable_DefineProc("suite-set-with-value-name", 
			SetValByName, &suite_classinfo,
			"suite", "Set an attribute for suite; pass name	of  value");
  proctable_DefineProc("suite-set-item-with-value-name", 
			SetEltValByName, &suite_classinfo, 
			"suite", "Set attr for an item;  pass name of value");
  return(TRUE);
}

boolean
suite__InitializeObject( ClassID, self )
  register struct classheader	*ClassID;
  register struct suite		*self;
{
  IN(suite_InitializeObject);
  if(!(Items = vector_Create(100,3)) || 
     !(SetView = suiteev_New()) || 
     !(Apt = apt_New())) 
    HandleException(self,NULL,suite_InsufficientSpace);
  suite_SetDataObject(self,Apt);
  Datum = 0;
  CurrentItem = NULL;
  SetView->parent = self;
  ContainerLeft = ContainerTop = ContainerWidth = ContainerHeight = 0;
  ItemOrder = suite_RowMajor;
  TitleCaption = NULL;
  rectangle_SetRectSize(&TitleRect,0,0,0,0);
  TitleHighlightStyle = suite_Invert;
  TitleBorderStyle = suite_None;
  TitleBorderSize = 1;
  TitleDataObjectName = NULL;
  TitleDataObject = NULL;
  TitleDataObjectHandler = NULL;    
  TitleViewObjectHandler = NULL;    
  TitleHighlighted = FALSE;
  Arrangement = suite_Matrix | suite_Balanced;
  SelectionMode = suite_Exclusive;
  TitlePlacement = suite_Top;
  TitleCaptionAlignment = suite_Middle | suite_Center;
  ItemTitlePlacement = TitlePlacement;
  ItemTitleCaptionAlignment = suite_Middle | suite_Center;
  BorderSize = 1;
  BorderStyle = suite_Rectangle;
  Scroll = NULL;
  rectangle_SetRectSize(&ScrollRect,0,0,0,0);
  ScrollType = 0;
  HitHandler = 0;
  TitleHitHandler = 0;
  TitleViewObjectName = NULL;
  TitleViewObject = NULL;
  TitleFontName = NULL;
  AllocNameSpace(&TitleFontName,"andytype");
  TitleFont = NULL;
  RealTitleFontSize = TitleFontSize = 12;
  TitleFontLow = TitleFontHigh = 0;
  TitleFontType = RealTitleFontType = fontdesc_Plain;
  ItemTitleFontName = NULL;
  AllocNameSpace(&ItemTitleFontName,"andytype");
  RealItemTitleFontSize = ItemTitleFontSize = 10;
  ItemTitleFontLow = ItemTitleFontHigh = 0;
  ItemTitleFontType = fontdesc_Plain;
  ItemViewObjectName = ItemDataObjectName = 0;
  ItemViewObjectHandler = ItemDataObjectHandler = 0;
  CaptionFontName = NULL;
  AllocNameSpace(&CaptionFontName,"andytype");
  CaptionFont = NULL;
  RealCaptionFontSize = 0;
  CaptionFontSize = 10;
  CaptionFontLow = CaptionFontHigh = 0;
  CaptionFontType = fontdesc_Plain;
  CaptionAlignment = suite_Middle | suite_Center;
  ItemBorderStyle = suite_Rectangle;
  ItemBorderSize = 1;
  ItemHighlightStyle = suite_Invert;
  ItemPassiveStyle = suite_Pale;
  AccessType = suite_ReadOnly;
  FirstVisible = NewFirstVisible = LastVisible = NULL;
  VisibleRows = Rows = VisibleColumns = Columns = 0; 
  ItemHeight = ItemWidth = ItemFixedWidth = ItemFixedHeight = 0;
  debug = Debug = 0;
  ExceptionHandler = 0;
  ExceptionStatus = 0;
  ExceptionItem = NULL;
  Cursor = NULL;
  CursorFontName = NULL;
  CursorFont = NULL;
  CursorByte = 0;
  ItemCursor = NULL;
  ItemCursorFontName = NULL;
  ItemCursorFont = NULL;
  ItemCursorByte = 0;
  SortHandler = 0;
  SortOrder = 0;
  ClientAnchor = 0;
  XGutterSize = YGutterSize = 1;
  ItemArray = NULL;
  HasFocus = FALSE;
  WindowSystem = 0;
  suite_SetOptions(self,aptv_SuppressControl | 
		    aptv_SuppressBorder | 
		    aptv_SuppressEnclosures);
  WrapStyle = suite_LeftIndent;
  MaxItemPosGiven = 0;
  OUT(suite_InitializeObject);
  return(TRUE);
}

void
suite__FinalizeObject( ClassID, self )
  struct classheader	*ClassID;
  struct suite		*self;
{
  IN(suite_FinalizeObject);
  suite_ClearAllItems(self);
  if(Apt) apt_Destroy(Apt);
  if(Scroll) scroll_Destroy(Scroll);
  if(TitleCaption) free(TitleCaption);
  if(TitleDataObjectName) free(TitleDataObjectName);
  if(TitleViewObjectName) free(TitleViewObjectName);
  if(TitleDataObject) dataobject_Destroy(TitleDataObject);
  if(TitleViewObject) view_Destroy(TitleViewObject);
  if(TitleFontName) free(TitleFontName);
  if(CaptionFontName) free(CaptionFontName);
  if(ItemTitleFontName) free(ItemTitleFontName);
  if(ItemViewObjectName) free(ItemViewObjectName);
  if(ItemDataObjectName) free(ItemDataObjectName);
  OUT(suite_FinalizeObject);
}

void
suite__ReceiveInputFocus( self )
  register struct suite   *self;
{
  IN(suite_ReceiveInputFocus);
  HasFocus = TRUE;
  super_ReceiveInputFocus(self);
  OUT(suite_ReceiveInputFocus);
}

void
suite__LoseInputFocus( self )
  register struct suite   *self;
{
  IN(suite_LoseInputFocus);
  HasFocus = FALSE;
  super_LoseInputFocus(self);
  OUT(suite_LoseInputFocus);
}

struct suite *
suite__Create( ClassID, suite, anchor )
  struct classheader	    *ClassID;
  suite_Specification	    *suite;
  unsigned		     anchor;
{
  register struct suite	    *self = NULL;

  if(!(self = suite_New())) 
    HandleException(self,NULL,suite_InsufficientSpace);
  ClientAnchor = anchor;    
  while(suite && suite->attribute) {
    SetSuiteAttribute(self,suite->attribute,suite->value);
    suite++;
  }
  if(SortHandler)
    if(Items) {
      vector_SetSortRoutine(Items,SortStub);
      vector_Sort(Items);
    }
  if(TitleDataObject && TitleDataObjectHandler) 
    TitleDataObjectHandler(ClientAnchor,self,NULL,suite_TitleObject);
  if(TitleViewObject && TitleViewObjectHandler)
    TitleViewObjectHandler(ClientAnchor,self,NULL,suite_TitleObject);
  if(Items) {
    FirstVisible = ITEM(0); 
    LastVisible = ITEM(vector_Count(Items) - 1);
  }
  OUT(suite_Create);
  return(self);
}

void
suite__DestroyItem( self, item )
  struct suite		*self;
  struct suite_item	*item;
{
  IN(suite_DestroyItem);
  if(Items && item) {
    if(Active(item) && Exposed(item)) {
      item->mode &= ~item_Active;
      if(IsLinked) suite_HideItem(self,item);
    }
    vector_RemoveItem(Items,item);
    FinalizeItem(item);
    item = NULL;
  }
  OUT(suite_DestroyItem);
}

struct suite_item *
suite__CreateItem( self, name, datum )
  register struct suite		*self;
  register char			*name;
  register long			 datum;
{
  register struct suite_item	*item = NULL;

  IN(CreateItem);
  item = GenerateItem(self,NULL,name,datum);
  OUT(CreateItem);
  return(item);
}

static long
BreakSorter( item1, item2 )
  register long   *item1, *item2;
{
  if(!item1 || !item2) return(0);
  if(*item1 < *item2) return(-1);
  else if(*item1 == *item2) return(0);
  else return(1);
}


static struct suite_item *
GenerateItem( self, spec, name, datum )
  register struct suite		*self;
  register suite_Specification  *spec;
  register char			*name;
  register long			 datum;
{
  register struct suite_item	*item = NULL;

  IN(GenerateItem);
  if(!(item = AllocItem()))
    HandleException(self,NULL,suite_InsufficientSpace);
  if(!Items) {
    if(!(Items = vector_Create(100,3))) {
      HandleException(self,NULL,suite_InsufficientSpace);
    }
    else {
      if(SortHandler) vector_SetSortRoutine(Items,SortStub);
      else if(SortOrder) SetSortRoutine(self);
    }
  }
  if(spec) /* from static declaration */
    while(spec && spec->attribute) {
      SetItemAttribute(self,item,spec->attribute,spec->value);
      spec++;
    }
  else {	   /* from CreateItem() */
    if(name) AllocNameSpace(&item->name,name);
    item->datum = datum;
  }
  item->suite = self;
  item->exposed = TRUE;
  item->mode = item_Active | item_Normalized;
  Breaks(item) = vector_Create(5,2);
  vector_SetSortRoutine(Breaks(item),BreakSorter);
  vector_AddItem(Items,item);
  OUT(GenerateItem);
  return(item);
}

static boolean
TitleFits( self, title_rect, title_sub_rect )
  register struct suite      *self;
  register struct rectangle  *title_rect, *title_sub_rect;
{
  register boolean	        status = TRUE;

  IN(TitleFits);
  if((title_sub_rect->left <= title_rect->left) ||
     ((title_sub_rect->left + title_sub_rect->width) >= 
      (title_rect->left + title_rect->width)) ||
     (title_sub_rect->top <= title_rect->top) ||
     ((title_sub_rect->top + title_sub_rect->height) >= 
      (title_rect->top + title_rect->height)) )
      status = FALSE;
  OUT(TitleFits);
  return(status);
}

static boolean
ItemsFit( self )
  register struct suite	    *self;
{
  long			     width = 0, height = 0;
  register long		     numItems = suiteev_NumberExposed(SetView);
  register boolean	     status = TRUE;

  IN(ItemsFit);
  if(numItems == 0 || List) return(TRUE);
  suiteev_MaxStringSize(SetView,&width,&height);
  if(((ItemWidth - (2 * ItemBorderSize)) < width) || 
     ((ItemHeight - (2 * ItemBorderSize)) < height) ||
     ((width + ( 2 * ItemBorderSize )) > ContainerWidth) || 
     ((height +( 2 * ItemBorderSize )) > ContainerHeight)) {
      status = FALSE;
  }
  OUT(ItemsFit);
  return(status);
}

#define	TITLE_ONLY		    0
#define	CAPTIONS_ONLY		    1

static void
DecrementFontSizes( self, which )
  struct suite	    *self;
  boolean	     which;
{
  if((which == TITLE_ONLY) && (RealTitleFontSize > TitleFontLow)) {
    RealTitleFontSize -= 2;
    TitleFont = fontdesc_Create(TitleFontName,RealTitleFontType,RealTitleFontSize);
  }
  if((which == CAPTIONS_ONLY) && (RealCaptionFontSize > CaptionFontLow)) {
    RealCaptionFontSize -= 2;
    CaptionFont = fontdesc_Create(CaptionFontName,CaptionFontType,RealCaptionFontSize);
  }
}

boolean
Arrange( self, rect )
  register struct suite	    *self;
  register struct rectangle *rect;
{
  struct rectangle	     suite, title_sect, title;
  register boolean	     title_fits = FALSE, captions_fit = FALSE;
  register long		     needs_scroll = FALSE;

  IN(Arrange);
  rectangle_SetRectSize(&suite,rect->left,rect->top,rect->width,rect->height);
  while(TRUE) {
    DecrementRect(rect,BorderSize);
    if(TitleCaption) {
      AssignSetAndTitleSpace(self,&title_sect,rect);
      PlaceTitle(self,&title_sect,&title);
    }
    if(ScrollLeft) {
      rect->left += SCROLL_WIDTH;
      rect->width -= SCROLL_WIDTH;
    }	    
    if(ScrollTop) {
      rect->top += SCROLL_WIDTH;
      rect->height -= SCROLL_WIDTH;
    }
    if(ScrollBottom) rect->height -= SCROLL_WIDTH;
    if(ScrollRight) rect->width -= SCROLL_WIDTH;
    rectangle_SetRectSize(rect,0,0,rect->width,rect->height);
    if(!List) suiteev_Arrange(SetView,rect);
    if(TitleCaption && !(title_fits = TitleFits(self,&title_sect,&title))) {
      if(RealTitleFontSize > TitleFontLow)
        DecrementFontSizes(self,TITLE_ONLY);
      else title_fits = TRUE;
    }
    if(!(captions_fit = ItemsFit(self))) {
      if(RealCaptionFontSize > CaptionFontLow)
	DecrementFontSizes(self,CAPTIONS_ONLY);
      else captions_fit = TRUE;
    }
    if(TitleCaption) {
      if(title_fits && captions_fit) break;
    }
    else if(captions_fit) break;
    rectangle_SetRectSize(rect,suite.left,suite.top,suite.width,suite.height);
  }
  if(Scroll) needs_scroll = TRUE;
  OUT(Arrange);
  return(needs_scroll);
}

static void
SetMWidths( self )
  struct suite		*self;
{
  struct fontdesc_charInfo	 M_Info;

  IN(SetMWidths);
  if(CaptionFont) {
    fontdesc_CharSummary(CaptionFont,suite_GetDrawable(self),'m',&M_Info);
    CaptionMWidth = M_Info.width;
  }
  if(TitleFont) {
    fontdesc_CharSummary(TitleFont,suite_GetDrawable(self),'m',&M_Info);    
    TitleMWidth = M_Info.width;
  }
  OUT(SetMWidths);
}

void
suite__Update( self )
  register struct suite   *self;
{
  IN(suite_Update);
  suiteev_Clear(SetView);
  suiteev_Update(SetView);
  if(Scroll) scroll_Update(Scroll);
  OUT(suite_Update);
}


void
suite__FullUpdate( self, type, left, top, width, height )
  register struct suite		    *self;
  register enum view_UpdateType      type;
  register long			     left, top, width, height;
{
  struct rectangle		     *arrange_rect = NULL, 
				     *curse_rect = NULL, title;
  register boolean		      needs_scroll = FALSE;

  IN(suite_FullUpdate);
  if(!strcmp("AndrewWM",suite_GetWindowManagerType(self)))
    WindowSystem = WM;
  if((type != view_FullRedraw) && (type != view_LastPartialRedraw)) return;
  suite_GetVisualBounds(self,&Bounds);
  suite_GetVisualBounds(self,&Container);
  if(!(BorderStyle & suite_None) && (BorderSize > 0)) {
    DecrementRect(&Bounds,1);
    DecrementRect(&Container,1);
  }
  arrange_rect = rectangle_Duplicate(&Container);
  SetTransferMode(self,graphic_WHITE);
  suite_FillRect(self,&Container,suite_WhitePattern(self));
  curse_rect = rectangle_Duplicate(&Container);
  if(!Cursor) {
    Cursor = cursor_Create(self);
    if(Cursor && CursorFont && (CursorByte != suite_NoCursor)) 
      cursor_SetGlyph(Cursor,CursorFont,CursorByte);
    else if(CursorByte != suite_NoCursor)
      cursor_SetStandard(Cursor,CursorByte);
    else cursor_SetStandard(Cursor,Cursor_Octagon);
  }
  RealCaptionFontSize = (List?CaptionFontSize:MAX(CaptionFontSize,CaptionFontHigh));
  CaptionFont = fontdesc_Create(CaptionFontName,CaptionFontType,RealCaptionFontSize);
  RealTitleFontSize = MAX(TitleFontSize,TitleFontHigh);
  TitleFont = fontdesc_Create(TitleFontName,RealTitleFontType,RealTitleFontSize);
  SetMWidths(self);
  if(Scroll) scroll_LinkTree(Scroll,self);
  else suiteev_LinkTree(SetView,self);
  needs_scroll = Arrange(self,arrange_rect);
  SetMWidths(self);
  DrawOutline(self,&Container,BorderSize,BorderStyle);
  if(TitleCaption || TitleViewObjectName || TitleDataObjectName ) {
    AssignSetAndTitleSpace(self,&title,&Container);
    DrawTitle(self,&title);
    if(TitleHighlighted) suite_HighlightTitle(self);
  }
  if(Scroll && needs_scroll) { DEBUG(Scroll);
    ScrollRect.left = ContainerLeft; ScrollRect.top = ContainerTop;
    ScrollRect.width = ContainerWidth; ScrollRect.height = ContainerHeight;
    if(ScrollLeft) {
      curse_rect->left += SCROLL_WIDTH; curse_rect->width -= SCROLL_WIDTH;
      ContainerLeft += SCROLL_WIDTH; ContainerWidth -= SCROLL_WIDTH;
    }
    if(ScrollRight) {
      curse_rect->width -= SCROLL_WIDTH;
      ContainerWidth -= SCROLL_WIDTH;
    }
    if(ScrollTop) {
      curse_rect->top += SCROLL_WIDTH; curse_rect->height -= SCROLL_WIDTH;
      ContainerTop += SCROLL_WIDTH; ContainerHeight -= SCROLL_WIDTH;
    }
    if(ScrollBottom) {
      curse_rect->width -= SCROLL_WIDTH;
      ContainerWidth -= SCROLL_WIDTH;
    }
    scroll_InsertView(Scroll,self,&ScrollRect);
    scroll_FullUpdate(Scroll, view_FullRedraw, 0, 0, ScrollRect.width, ScrollRect.height);
  }
  else { DEBUG(No Scroll);
    suiteev_InsertView(SetView,self,&Container);
    suiteev_FullUpdate(SetView,view_FullRedraw,0,0, ContainerWidth, ContainerHeight);
  }
  if(Cursor) suite_PostCursor(self,curse_rect,Cursor);
  if(curse_rect) free(curse_rect);
  OUT(suite_FullUpdate);
}


static void
DrawTitle( self, rect )
  register struct suite	    *self;
  register struct rectangle *rect;
{
  register long		     title_lines = NumberLines(TitleCaption),
			     vert_point = 0, horiz_point = 0, i;
  struct rectangle	     title;
  char			    *tmp_title = NULL;
  register char		    *str = NULL, *next_str = NULL, *newline = NULL;
  long			     align = 0;
  boolean		     titleFits = FALSE;

  IN(DrawTitle);
  SetTransferMode(self,graphic_WHITE);
  suite_FillRect(self,rect,suite_WhitePattern(self));
  SetTransferMode(self,graphic_BLACK);
  if(TitleCaption && (*TitleCaption != '\0')) {
    AllocNameSpace(&tmp_title,TitleCaption);
    str = next_str = tmp_title;
    DecrementRect(rect,1);
    DrawOutline(self,rect,TitleBorderSize,TitleBorderStyle);
    suite_SetClippingRect(self,rect);
    RealTitleFontSize = MAX(TitleFontSize,TitleFontHigh);
    while(!titleFits) {
      PlaceTitle(self,rect,&title);
      TitleFont = fontdesc_Create(TitleFontName,RealTitleFontType,RealTitleFontSize);
      if((titleFits = TitleFits(self,rect,&title)) || 
	 (RealTitleFontSize <= TitleFontLow)) break;
      else DecrementFontSizes(self,TITLE_ONLY);
    }
    horiz_point = title.left + title.width/2;
    vert_point = title.top + RealTitleFontSize/2;
    align = graphic_BETWEENTOPANDBOTTOM;
    if(TitleCaptionAlignment & suite_Left) {
      horiz_point = title.left + 1;
      align |= graphic_ATLEFT;
    }
    else if(TitleCaptionAlignment & suite_Right) {
      horiz_point = title.left + title.width - 1;
      align |= graphic_ATRIGHT;
    }
    else align |= graphic_BETWEENLEFTANDRIGHT;
    if(TitleFont) SetFont(self,TitleFont);
    for( i = 0 ; i < title_lines ; i++ ) {
      if(newline = (char*)rindex(next_str,'\n')) {
	next_str = newline + 1;
	*newline = NULL;
      }
      suite_MoveTo(self,horiz_point,vert_point);
      suite_DrawString(self,str,align);
      vert_point += RealTitleFontSize;
      str = next_str;
    }
    if(tmp_title) free(tmp_title);
    suite_ClearClippingRect(self);
  }
  else if(TitleViewObjectName || TitleDataObjectName) {
    if(TitleDataObjectName) {
      if(!TitleDataObject)
	TitleDataObject = 
	    (struct dataobject*)class_NewObject(TitleDataObjectName);
      if(TitleDataObject && TitleDataObjectHandler) 
        TitleDataObjectHandler(ClientAnchor,self,NULL,suite_TitleObject);
    }
    if(!TitleViewObject) {
      if(!TitleViewObjectName)
	AllocNameSpace(&TitleViewObjectName,
		       dataobject_ViewName(TitleDataObject));
      TitleViewObject = (struct view*)class_NewObject(TitleViewObjectName);
      if(TitleViewObject && TitleViewObjectHandler)
        TitleViewObjectHandler(ClientAnchor,self,NULL,suite_TitleObject);
    }
    view_SetDataObject(TitleViewObject,TitleDataObject);
    DrawOutline(self,rect,TitleBorderSize,TitleBorderStyle);
    view_InsertView(TitleViewObject,self,rect);
    view_FullUpdate(TitleViewObject,view_FullRedraw,0,0,
		     rect->width,rect->height);
  }
  else {
    if(Debug) printf("Title gone; calling FullUpdate.\n");
    suite_FullUpdate(self,view_FullRedraw,0,0,Bounds.width,Bounds.height);
  }
  OUT(DrawTitle);
}

static void
CenterBiggestCircleInRect( self, rect )
  register struct suite		*self;
  register struct rectangle	*rect;
{
  register long    circum = MIN(rect->width,rect->height), rad = circum/2;
  register long    centerX = rect->left + rect->width/2, centerY = rect->top + rect->height/2; 
  rectangle_SetRectSize(rect,centerX - rad,centerY - rad,circum,circum);
}

static void
DrawOutline( self, rect, width, style )
  register struct suite		*self;
  register struct rectangle	*rect;
  register short		 width;
  register unsigned		 style;
{
  register long			 i = 0;
  register struct rectangle	*inner = NULL;
  register long			 X1 = 0, Y1 = 0, X2 = 0, Y2 = 0;

  IN(DrawOutline);
  SetTransferMode(self,graphic_BLACK);
  if(style & (suite_Invisible | suite_None)) return;
  else if(style & suite_Line) {
    if(TitlePlacement & suite_Top) {
      X1 = rect->left; Y1 = rect->top + rect->height;
      X2 = X1 + rect->width; Y2 = Y1;
    }
    else if(TitlePlacement & suite_Bottom) {
      X1 = rect->left; Y1 = rect->top;
      X2 = X1 + rect->width; Y2 = Y1;
    }
    else if(TitlePlacement & suite_Left) {
      X1 = rect->left + rect->width; Y1 = rect->top;
      X2 = X1; Y2 = Y1 + rect->height;
    }
    else if(TitlePlacement & suite_Right) {
      X1 = rect->left; Y1 = rect->top;
      X2 = X1; Y2 = Y1 + rect->height;
    }
    suite_MoveTo(self,X1,Y1);
    suite_DrawLineTo(self,X2,Y2);
  }
  else if(style & suite_Rectangle) { 
    for( i = 0 ; i < width ; i++ ) {
      DrawRect(self,rect);
      DecrementRect(rect,1);
    }
  }
  else if(style & suite_Roundangle) { 
    inner = rectangle_Duplicate(rect);
    DecrementRect(inner,5);
    suite_DrawRRect(self,rect,inner);
    DecrementRect(rect,5);
  }
  else if(style & suite_Circle)
    CenterBiggestCircleInRect(self,rect);
  if((style & suite_Oval) || (style & suite_Circle)) {
    for( i = 0 ; i < width ; i++ ) {
      suite_DrawOval(self,rect);
      DecrementRect(rect,1);
    }
  }
  SetTransferMode(self,graphic_BLACK);
  OUT(DrawOutline);
}

struct view *
suite__Hit( self, action, x, y, numberOfClicks )
  register struct suite		    *self;
  register enum	view_MouseAction     action;
  register long			     x, y, numberOfClicks;
{
  struct view			    *retval = (struct view*)self;

  IN(suite_Hit);
  if(Scroll) {
    if(WithinRect(x,y,&ScrollRect))
      retval = scroll_Hit(Scroll,action,x - ScrollRect.left,
			   y - ScrollRect.top,numberOfClicks);
  }
  else {
    if(Within(x - ContainerLeft,y - ContainerTop,0,0,
	       ContainerWidth,ContainerHeight))
      retval = suiteev_Hit(SetView,action,x - ContainerLeft,y - ContainerTop,
			    numberOfClicks);
  }
  if(WithinRect(x,y,&TitleRect)) {
    if(TitleHitHandler) 
      TitleHitHandler(ClientAnchor,self,NULL,suite_TitleObject,
		       action,x,y,numberOfClicks);
    else if(HitHandler)
      HitHandler(ClientAnchor,self,NULL,suite_TitleObject,
		  action,x,y,numberOfClicks);
    else if(TitleViewObject) 
      retval = view_Hit(TitleViewObject,action,x,y,numberOfClicks);
  }
  return((struct view*)retval);
}

long
suite__Reset( self, state )
  register struct suite		    *self;
  register long			     state;
{
  register long			     i = 0, status = 0;
  register struct suite_item	    *item = NULL;
  boolean			     onScreen = FALSE, doFullRedraw = FALSE;
  boolean			     doContainerRedraw = FALSE;

  if(!(state & suite_Defer)) state |= suite_Immediate;
  if(state & suite_Clear) {
    suite_ClearAllItems(self);
    suite_SetSuiteAttribute(self,suite_TitleCaption(NULL));
    doFullRedraw = TRUE;
  }
  if(state & suite_ClearItems) {
    suite_ClearAllItems(self);
    doContainerRedraw = TRUE;
  }
  if(state & suite_ClearTitle) {
    suite_SetSuiteAttribute(self,suite_TitleCaption(NULL));
    doFullRedraw = TRUE;
  }
  if(state & suite_Activate) {
    if(Items && ITEM(0)) {
      while(item = ITEM(i++)) {
	if(FirstVisible && (FirstVisible == item)) onScreen = TRUE;
  	if(Exposed(item) && !Active(item)) {
	  suite_ActivateItem(self,item);
	  if((state & suite_Immediate) && onScreen)
	    suiteev_ItemNormalize(SetView,item);
	}
	else if(Exposed(item) && Highlighted(item)) 
	  if((state & suite_Immediate) && onScreen)
	    suiteev_ItemNormalize(SetView,item);
	  else if((state & suite_Expose) && !Exposed(item)) {
	    item->exposed = TRUE;
	    doContainerRedraw = TRUE;
	  }
	item->mode = ((item_Active | item_Normalized) & ~item_Highlighted);
	if(LastVisible && (LastVisible == item)) onScreen = FALSE;
      }
    }
    else status = -1;
  }
  if(state & suite_Normalize) {
    if(Items && ITEM(0)) {
      while(item = ITEM(i++)) {
	if(FirstVisible && (FirstVisible == item)) onScreen = TRUE;
	if(Exposed(item) && Active(item) && Highlighted(item)) {
	  if((state & suite_Immediate) && onScreen)
	    suiteev_ItemNormalize(SetView,item);
	  else item->mode = ((item_Active | item_Normalized) & 
			     ~item_Highlighted);
	}
	else if((state & suite_Expose) && !Exposed(item)) {
	  item->exposed = TRUE;
	  doContainerRedraw = TRUE;
	}
	if(LastVisible && (LastVisible == item)) onScreen = FALSE;
      }
    }
  }
  if(doFullRedraw && IsLinked) {
    suiteev_Clear(SetView);
    suite_FullUpdate(self,view_FullRedraw,0,0,0,0);
  }
  else if(doContainerRedraw && IsLinked) {
    suiteev_Clear(SetView);
    suiteev_FullUpdate(SetView,view_FullRedraw,
			0,0,ContainerWidth,ContainerHeight);
    if(Scroll) scroll_Update(Scroll);
  }
  return(status);
}

void
suite__ClearAllItems( self )
  struct suite			*self;
{
  register int			 i = 0, count = 0;
  register struct suite_item	*item = NULL;

  IN(suite_ClearAllItems);
  if(Items && ITEM(0)) {
    count = vector_Count(Items);
    for( i = 0 ; i < count ; i++ ){ 
      item = ITEM(i);
      FinalizeItem(item);
      item = NULL;
    }
    vector_Destroy(Items);
    Items = NULL;
  }
  NewFirstVisible = FirstVisible = LastVisible = NULL;
  OUT(suite_ClearAllItems);
}

static long
TitleSectionWidth( self )
  struct suite    *self;
{
  register char   *title = NULL, *newline = NULL, *tmp = NULL;
  long		   numLines = 0, XWidth = 0, YWidth = 0, maxWidth = 0;
  register long    i = 0;

  if(TitleCaption) {
    tmp = title = (char*) malloc(strlen(TitleCaption) + 1);
    strcpy(title,TitleCaption);
    numLines = NumberLines(TitleCaption);
    for ( i = 0 ; i < numLines ; i++ ) {
      if(newline = (char*) rindex(tmp,'\n')) {
        *newline = NULL;
	fontdesc_StringBoundingBox(TitleFont,suite_GetDrawable(self),tmp,
			    &XWidth,&YWidth);
	tmp = newline + 1;
      }
      else 
	  fontdesc_StringBoundingBox(TitleFont,suite_GetDrawable(self),tmp,
				&XWidth,&YWidth);
      if(XWidth > maxWidth) maxWidth = XWidth;
    }
    free(title);
  }
  else if(TitleViewObjectName || TitleDataObjectName) {
    if(TitlePlacement & (suite_Top | suite_Bottom))
      maxWidth = ContainerWidth;
    else maxWidth = ContainerWidth/3;
  }
  return(maxWidth);
}

static long
TitleSectionHeight( self )
  struct suite *self;
{
  IN(TitleSectionHeight);
  if(TitleCaption) return(NumberLines(TitleCaption) * RealTitleFontSize);
  else if(TitleViewObjectName || TitleDataObjectName) {
    if(TitlePlacement & (suite_Top | suite_Bottom))
      return(ContainerHeight/3);
    else return(ContainerHeight);
  }
  OUT(TitleSectionHeight);
}

static void
AssignSetAndTitleSpace( self, title, container )
  register struct suite		*self;
  register struct rectangle	*title, *container;
{
  register long			 TitleHeight = TitleSectionHeight(self), 
				 TitleWidth = TitleSectionWidth(self);

  switch(TitlePlacement) {
    case suite_Top:
      title->left = container->left;
      title->width = container->width;
      title->top = container->top;
      title->height = TitleHeight + TitleMWidth;
      container->top = title->top + title->height;
      container->height -= title->height;
      break;
    case suite_Bottom:
      title->left = container->left;
      title->width = container->width;
      title->height = TitleHeight + TitleMWidth;
      title->top = container->top + container->height - title->height;
      container->height -= title->height;
      break;
    case suite_Left:
      title->left = container->left;
      title->top = container->top;
      title->width = TitleWidth + TitleMWidth;
      title->height = container->height;
      container->left = title->left + title->width;
      container->width -= title->width;
      break;
    case suite_Right:
      title->top = container->top;
      title->height = container->height;
      title->width = TitleWidth + TitleMWidth;
      title->left = container->left + container->width - title->width;
      container->width -= title->width;
      break;
  }
  rectangle_SetRectSize(&TitleRect,title->left,title->top,
	title->width,title->height);
}

static void
PlaceTitle( self, title_sect, title )
  struct suite		*self;
  struct rectangle	*title_sect, *title;
{
  register long		 Width = TitleSectionWidth(self), 
			 Height = TitleSectionHeight(self);
  register unsigned	 alignment = TitleCaptionAlignment;

  title->left = title_sect->left + (title_sect->width - Width)/2;
  title->top = title_sect->top + (title_sect->height - Height)/2;
  title->width = Width; title->height = Height;
  if(alignment & suite_Left) { 
    title->left = title_sect->left + 2;
  }
  else if(alignment & suite_Right) {
    title->left = title_sect->left + (title_sect->width - Width) - 2;
  }
  if(alignment & suite_Top) {
    title->top = title_sect->top + 2;
  }
  else if(alignment & suite_Bottom) {
    title->top = title_sect->top + (title_sect->height - Height) - 2;
  }
}

static void
SetCaptionList( self, captions )
  register struct suite	    *self;
  register char		   **captions;
{
  register char		   **ptr = captions;

  suite_ClearAllItems(self);
  if(ptr && *ptr && **ptr) {
    if(!(Items = vector_Create(100,3))) 
      HandleException(self,NULL,suite_InsufficientSpace);
    else {
      if(SortHandler) 
	vector_SetSortRoutine(Items,SortStub);
      else if(SortOrder) 
	SetSortRoutine(self);
    }
    while(*ptr)
      if(!(suite_CreateItem(self,*ptr++,0)))
	HandleException(self,NULL,suite_InsufficientSpace);
    if(Items && ITEM(0)) {
      FirstVisible = ITEM(0);
      LastVisible = ITEM(vector_Count(Items) - 1);
    }
  }
}

static void
ParseFontFullName( self, fullname, familyName, buffSize, size, type )
  register struct suite	    *self;
  register char		    *fullname, *familyName;
  register long		     buffSize;
  register long		    *size, *type;
{
  if(fullname && *fullname) fontdesc_ExplodeFontName(fullname,familyName,
	buffSize,(long)type,(long)size);
}

static void
ChangeItemCaption( self, item, caption )
  register struct suite		*self;
  register struct suite_item	*item;
  register char			*caption;
{
  register struct text		*txt = NULL;
  register struct suitecv	*CV = NULL;

  ValidateItem(self,item);
  AllocNameSpace(&item->caption,caption);
  if(item_AccessType & suite_ReadWrite) {
    text_Clear(txt = (struct text*) item->dataobject);
    text_InsertCharacters(txt,0,item->caption,strlen(item->caption));
    suitecv_WantUpdate(CV = (struct suitecv*)item->viewobject,CV);
  }
  suiteev_ItemUpdate(SetView,item);
}

void
suite__PassivateItem( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  register long			 mode = 0;

  if(item) { 
    if(!Active(item)) return;
    if(IsLinked && (ItemPassiveStyle & suite_Removed)) 
      suite_HideItem(self,item);
    else {
      mode = (item->mode & ~item_Active & ~item_Highlighted) | 
	item_Normalized;
      if(ItemPassiveStyle & suite_Invisible) mode |= suite_Invisible;
      item->mode = mode;
      if(IsLinked) suiteev_ItemUpdate(SetView,item);
    }
  }
}

void
suite__ActivateItem( self, item )
  struct suite		*self;
  struct suite_item	*item;
{
  register long		 mode = 0;

  if(item) {
    if(Active(item)) return;
    if(IsLinked && (ItemPassiveStyle & suite_Removed))
      suite_ExposeItem(self,item);
    else {
      mode = (item->mode | item_Active | item_Normalized) & 
	~item_Highlighted;
      if(ItemPassiveStyle & suite_Invisible) mode |= ~suite_Invisible;
      item->mode = mode;
      if(IsLinked) suiteev_ItemUpdate(SetView,item);
    }
  }
}

static struct suite_item*
AllocItem()
{
  return((struct suite_item*) calloc(1,sizeof(struct suite_item)));
}

static void
FinalizeItem( item )
  register struct suite_item *item;
{
  IN(FinalizeItem);
  if(Breaks(item))		vector_Destroy(Breaks(item));
  if(item->caption)		free(item->caption);
  if(item->title)		free(item->title);
  if(item->titlefontname)	free(item->titlefontname);
  if(item->captionfontname)	free(item->captionfontname);
  if(item->viewobjectname)	free(item->viewobjectname);
  if(item->dataobjectname)	free(item->dataobjectname);
  if(item->viewobject)	{
    view_UnlinkTree(item->viewobject);
    view_Destroy(item->viewobject);
    item->viewobject = NULL;
  }
/*===
    if(item->dataobject)	dataobject_Destroy(item->dataobject);
gk5g 5/1/89
===*/
  free(item);
  OUT(FinalizeItem);
}

static void
SetSortRoutine( self )
  register struct suite   *self;
{
  if(suite_Ascend & SortOrder) {
    if(suite_Alphabetic & SortOrder) 
      vector_SetSortRoutine(Items,AlphasortAscend);
    else if(suite_Numeric & SortOrder)
      vector_SetSortRoutine(Items,NumericAscend);
  }
  else if(suite_Descend & SortOrder) {
    if(suite_Alphabetic & SortOrder)
      vector_SetSortRoutine(Items,AlphasortDescend);
    else if(suite_Numeric & SortOrder)
      vector_SetSortRoutine(Items,NumericDescend);
    }
}

static long
SortStub( item1, item2 )
  register struct suite_item	 **item1;
  register struct suite_item	 **item2;
{
  register struct suite		*self = NULL;
  register long			status = 0;
  if(item1 && *item1 && item2 && *item2) {
    self = (*item1)->suite;
    if(self) status = SortHandler(ClientAnchor,self,*item1,*item2);
  }
  return status;
}

void
suite__Sort( self, mode, handler )
  register struct suite	*self;
  register unsigned	 mode;
  register long	       (*handler)();
{
  IN(suite_Sort);
  suiteev_Clear(SetView);
  if(Items && ITEM(0)) { 
    DEBUG(Sort);
    if(mode) SortOrder = mode;
    if(handler) { DEBUG(Handler);
      SortHandler = (long (*)())handler;
      vector_SetSortRoutine(Items,SortStub);
    }
    else SetSortRoutine(self);
    vector_Sort(Items);
    FirstVisible = ITEM(0);
    LastVisible = ITEM(vector_Count(Items) - 1);
    suiteev_FullUpdate(SetView,view_FullRedraw,0,0,
			ContainerWidth,ContainerHeight);
    if(Scroll) scroll_Update(Scroll);
  }
  OUT(suite_Sort);
}

void
suite__Apply( self, proc, anchor, datum )
  register struct suite	     *self;
  register long		    (*proc)();
  register unsigned	      anchor, datum;
{
  register int		      i = 0;
  register struct suite_item *item = NULL;
  register long		      status = 0;

  if(Items && ITEM(0))
    while((item = ITEM(i++)) && (status >= 0)) 
      status = proc(anchor,self,item,datum);
}

static long
AlphasortAscend( item1, item2 )
  register struct suite_item   **item1, **item2;
{
  register char		 *str1 = NULL, *str2 = NULL;

  if(!item1 || !*item1 || !item2 || !*item2) return(0);
  if((*item1)->caption) str1 = (*item1)->caption;
  else str1 = (*item1)->name;
  if((*item2)->caption) str1 = (*item2)->caption;
  else str2 = (*item2)->name;
  return(strcmp(str1,str2));
}

static long
NumericAscend( item1, item2 )
  register struct suite_item    **item1, **item2;
{
  if(!item1 || !*item1 || !item2 || !*item2) return(0);
  if((*item1)->datum > (*item2)->datum) return(1);
  else if((*item1)->datum < (*item2)->datum) return(-1);
  else return(0);
}

static long
AlphasortDescend( item1, item2 )
  register struct suite_item   **item1, **item2;
{
  register char			*str1 = NULL, *str2 = NULL;

  if(!item1 || !*item1 || !item2 || !*item2) return(0);
  if((*item1)->caption) str1 = (*item1)->caption;
  else str1 = (*item1)->name;
  if((*item2)->caption) str1 = (*item2)->caption;
  else str2 = (*item2)->name;
  return(-1 * strcmp(str1,str2));
}

static long
NumericDescend( item1, item2 )
  register struct suite_item	**item1, **item2;
{
  if(!item1 || !*item1 || !item2 || !*item2) return(0);
  if((*item1)->datum < (*item2)->datum) return(-1);
  else if((*item1)->datum > (*item2)->datum) return(1);
  else return(0);
}


static void
AllocNameSpace( target, source )
  register char **target, *source;
{
  if(target && *target) {
    free(*target);
    *target = NULL;
  }
  if(source && *source) {
    *target = malloc(strlen(source)+1);
    if(*target) strcpy(*target,source);
  }
  else *target = NULL;
}
    
struct suite_item **
suite__SelectedItems( self, number )
  register struct suite		*self;
  register long			*number;
{
  register struct suite_item	*item = NULL;
  register int			 i = 0;
  register long			 count = 0, index = 0;

  if(Items && ITEM(0)) {
    while(item = ITEM(i++))
      if(Exposed(item) && Active(item) && Highlighted(item)) count++;
    suiteev_AllocItemArray(SetView,count);
    if(ItemArray) {
      i = 0;
      while(item = ITEM(i++))
	if(Exposed(item) && Active(item) && Highlighted(item)) 
	  ItemArray[index++] = item;
    }
  }
  if(number) *number = count;
  return(ItemArray);
}

void
suite__SetDebug( self, value )
  register struct suite	    *self;
  register boolean	     value;
{
  self->debug = SetView->debug = value;
}

static void
SetArrangementAttribute( self, value )
  register struct suite    *self;
  register unsigned long    value;
{
  if(value & suite_List) 
    Arrangement |= suite_List;
  if(value & suite_Matrix) {
    Arrangement &= (~suite_Row & ~suite_Column);
    Arrangement |= suite_Matrix;
  }
  if(value & suite_Row) {
    Arrangement &= (~suite_Matrix & ~suite_Column);
    Arrangement |= suite_Row;
  }
  if(value & suite_Column) {
    Arrangement &= (~suite_Matrix & ~suite_Row);
    Arrangement |= suite_Column;
  }
  if(value & suite_Balanced) {
    Arrangement &= ~suite_Unbalanced;
    Arrangement |= suite_Balanced;
  }
  if(value & suite_Unbalanced) {
    Arrangement &= ~suite_Balanced;
    Arrangement |= suite_Unbalanced;
  }
  if(value & suite_RowLine) Arrangement |= suite_RowLine;
  if(value & suite_ColumnLine) Arrangement |= suite_ColumnLine;
}

static void
SetBorderStyleAttribute( self, border_style, value )
  register struct suite	*self;
  register unsigned int	*border_style;
  register long		 value;
{
  if(value & suite_Invisible) {
    *border_style &= ~suite_None;
    *border_style |= suite_Invisible;
  }
  if(value & suite_Rectangle) {
    *border_style &= (~suite_Roundangle & ~suite_Circle & 
		       ~suite_Oval & ~suite_None);
    *border_style |= suite_Rectangle;
  }
  if(value & suite_Roundangle) {
    *border_style &= (~suite_Rectangle & ~suite_Circle & 
		       ~suite_Oval & ~suite_None);
    *border_style |= suite_Roundangle;
  }
  if(value & suite_Circle) {
    *border_style &= (~suite_Rectangle & ~suite_Roundangle & 
		       ~suite_Oval & ~suite_None);
    *border_style |= suite_Circle;
  }
  if(value & suite_Oval) {
    *border_style &= (~suite_Rectangle & ~suite_Roundangle &
		       ~suite_Circle & ~suite_None);
    *border_style |= suite_Oval;
  }
  if(value & suite_Line) {
    *border_style &= (~suite_Rectangle & ~suite_Roundangle &
		       ~suite_Circle & ~suite_None & ~suite_Oval);
    *border_style |= suite_Line;
  }
  if(value & suite_None) {
    *border_style &= (~suite_Rectangle & ~suite_Roundangle &
		       ~suite_Circle & ~suite_Oval);
    *border_style |= suite_None;
  }
}

static void
SetSuiteAttribute( self, attribute, value )
  register struct suite	    *self;
  register long		     attribute, value;    
{
  char			     Name[100], *tmp = NULL;
  long int		     Size, Type;

  IN(SetSuiteAttribute);
  switch(attribute) {
	case suite_titlecaption:
	        AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&TitleCaption,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_titlehighlightstyle:
		TitleHighlightStyle = value;
		break;
	case suite_titleborderstyle:
	        SetBorderStyleAttribute(self,&TitleBorderStyle,value);
		break;
	case suite_titlebordersize:
		TitleBorderSize = value;
		break;
	case suite_titledataobjectname:
	        AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&TitleDataObjectName,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_itemorder:
		ItemOrder = value;
		break;
	case suite_itemcaptionlist:
		SetCaptionList(self,(char**)value);
		break;
	case suite_itemwidth:
		ItemFixedWidth = value;
		break;
	case suite_itemheight:
		ItemFixedHeight = value;
		break;
	case suite_itemspec:
		GenerateItem(self,(suite_Specification*)value, NULL, NULL);
		break;
	case suite_selectionmode:
		SelectionMode = (unsigned)value;
		break;
	case suite_borderstyle:
	        SetBorderStyleAttribute(self,&BorderStyle,value);
		break;
	case suite_bordersize:
		BorderSize = value;
		break;
	case suite_hithandler:
		self->hithandler = (long(*)())value;
		break;
	case suite_arrangement:
		SetArrangementAttribute(self,value);
		break;
	case suite_scroll:
		ScrollType = value;
		Scroll = scroll_Create(SetView,ScrollType);
		break;
	case suite_titleplacement:
		TitlePlacement = value;
		break;
	case suite_titlecaptionalignment:
	        TitleCaptionAlignment = value;
	        break;
	case suite_titlefontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&TitleFontName,Name);
		TitleFontSize = Size;
		if(TitleFontLow ==0) TitleFontLow = Size - DEFAULT_FONT_RANGE;
		if(TitleFontHigh == 0) TitleFontHigh = Size + DEFAULT_FONT_RANGE;
		RealTitleFontType = TitleFontType = Type;
		TitleFont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
		break;
	case suite_titleviewobjectname:
	        AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&TitleViewObjectName,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_titleviewobjecthandler:
		TitleViewObjectHandler = (long(*)())value;
		break;
	case suite_titledataobjecthandler:
		TitleDataObjectHandler = (long(*)())value;
		break;
	case suite_titlefontlow:
		self->titlefontlow = value;
		break;
	case suite_titlefonthigh:
		TitleFontHigh = value;
		break;
        case suite_itemcaptionalignment:
	        CaptionAlignment = value;
	        break;
	case suite_itemtitleplacement:
		ItemTitlePlacement = value;
		break;
	case suite_titlehithandler:
		TitleHitHandler = (long(*)())value;
		break;
	case suite_itemcaptionfontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&CaptionFontName,Name);
		CaptionFontSize = Size;
		if(CaptionFontLow == 0) CaptionFontLow = Size - DEFAULT_FONT_RANGE;
		if(CaptionFontHigh == 0) CaptionFontHigh = Size + DEFAULT_FONT_RANGE;
		CaptionFontType = Type;
		CaptionFont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
		break;
	case suite_itemcaptionfontlow:
		CaptionFontLow = value;
		break;
	case suite_itemcaptionfonthigh:
		CaptionFontHigh = value;
		break;
	case suite_itemtitlefontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		if(tmp) free(tmp);
		AllocNameSpace(&ItemTitleFontName,Name);
		ItemTitleFontSize = Size;
		if(ItemTitleFontLow == 0) ItemTitleFontLow = Size - DEFAULT_FONT_RANGE;
		if(ItemTitleFontHigh == 0) ItemTitleFontHigh = Size + DEFAULT_FONT_RANGE;
		ItemTitleFontType = Type;
		break;
	case suite_itemtitlefontlow:
		self->itemtitlefontlow = value;
		break;
	case  suite_itemtitlefonthigh:
		self->itemtitlefonthigh = value;
		break;
        case suite_fontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&TitleFontName,Name);
		TitleFontSize = Size;
		if(TitleFontLow == 0) TitleFontLow = Size - DEFAULT_FONT_RANGE;
		if(TitleFontHigh == 0) TitleFontHigh = Size + DEFAULT_FONT_RANGE;
		RealTitleFontType = TitleFontType = Type;
		TitleFont = suite_BuildFont(self,tmp,&Size);
		AllocNameSpace(&ItemTitleFontName,Name);
		ItemTitleFontSize = Size;
		if(ItemTitleFontLow == 0) ItemTitleFontLow = Size - DEFAULT_FONT_RANGE;
		if(ItemTitleFontHigh == 0) ItemTitleFontHigh = Size + DEFAULT_FONT_RANGE;
		ItemTitleFontType = Type;
		TitleFont = suite_BuildFont(self,tmp,&Size);
		AllocNameSpace(&CaptionFontName,Name);
		CaptionFontSize = Size;
		if(CaptionFontLow == 0) CaptionFontLow = Size - DEFAULT_FONT_RANGE;
		if(CaptionFontHigh == 0) CaptionFontHigh = Size + DEFAULT_FONT_RANGE;
		CaptionFontType = Type;
		CaptionFont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
	        break;
	case suite_itemborderstyle:
	        SetBorderStyleAttribute(self,&ItemBorderStyle,value);
		break;
	case suite_itembordersize:
		self->itembordersize = value;
		break;
	case suite_itemhighlightstyle:
		ItemHighlightStyle = value;
		break;
	case suite_itempassivestyle:
		ItemPassiveStyle = value;
		break;
	case suite_datum:
		Datum = value;
		break;
	case suite_accesstype:
		AccessType = (unsigned)value;
		break;
	case suite_itemdataobjectname:
	        AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&ItemDataObjectName,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_itemdataobjecthandler:
		ItemDataObjectHandler = (long(*)())value;
		break;
	case suite_itemviewobjecthandler:
		ItemViewObjectHandler = (long(*)())value;
		break;
	case suite_itemviewobjectname:
	        AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&ItemViewObjectName,strip(tmp));
		if(tmp) free(tmp);
		break;
	    case suite_guttersize:
		YGutterSize = XGutterSize = value;
		break;
	case suite_verticalguttersize:
		YGutterSize = value;
		break;
	case suite_horizontalguttersize:
		XGutterSize = value;
		break;
	case suite_sortmode:
		SortOrder = value;
		if(Items) SetSortRoutine(self);
		break;
	case suite_sorthandler:
		SortHandler = (long(*)())value;
		if(Items) vector_SetSortRoutine(Items,SortStub);
		break;
	case suite_cursorfontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&CursorFontName,Name);
		CursorFont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
		if(Cursor) {
		    cursor_Destroy(Cursor);
		    Cursor = NULL;
		}
		break;
	case suite_cursorbyte:
		CursorByte = (char) value;
		if(Cursor) {
		    cursor_Destroy(Cursor);
		    Cursor = NULL;
		}
		break;
	case suite_itemcursorfontname:
	        AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&ItemCursorFontName,Name);
		ItemCursorFont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
		if(ItemCursor) {
		    cursor_Destroy(ItemCursor);
		    ItemCursor = NULL;
		}
		break;
	case suite_itemcursorbyte:
		ItemCursorByte = (char) value;
		if(ItemCursor) {
		    cursor_Destroy(ItemCursor);
		    ItemCursor = NULL;
		}
		break;
	case suite_wrappingstyle:
		WrapStyle = (short) value;
		break;
	default: fprintf( stderr, "Suite: Unknown Suite Attribute (%d)\n",attribute);
  }
  OUT(SetSuiteAttribute);
}

long
suite__SetSuiteAttribute( self, attribute, value )
  register struct suite	    *self;
  register long		     attribute, value;
{  
  register long		     status = 0;

  SetSuiteAttribute(self,attribute,value);
  return(status);
}

static void
ChangeSuiteAttribute( self, attribute, value )
  register struct suite	    *self;
  register long		     attribute, value;    
{
  struct rectangle	    *title_rect = rectangle_Duplicate(&TitleRect);

  SetSuiteAttribute(self,attribute,value);
  switch(attribute) {
	case suite_titleviewobjectname:
	    if(TitleViewObject) {
		view_UnlinkTree(TitleViewObject);
		view_Destroy(TitleViewObject);
		TitleViewObject = NULL;
	    }
	    DrawTitle(self,title_rect);
	    if(TitleHighlighted) suite_HighlightTitle(self);
	    break;
	case suite_titledataobjectname:
	    view_UnlinkTree(TitleViewObject);
	    view_Destroy(TitleViewObject);
	    free(TitleViewObjectName);
	    TitleViewObjectName = NULL;
	    dataobject_Destroy(TitleDataObject);
	    TitleDataObject = NULL;
	    TitleViewObject = NULL;
	    DrawTitle(self,title_rect);
	    if(TitleHighlighted) suite_HighlightTitle(self);
	    break;
	case suite_titleplacement:
	case suite_titlecaptionalignment:
	case suite_titlehighlightstyle:
	case suite_titleborderstyle:
	case suite_titlebordersize:
	case suite_titlecaption:
	    DrawTitle(self,title_rect);
	    if(TitleHighlighted) suite_HighlightTitle(self);
	    break;
	case suite_itemorder:
	case suite_arrangement:
	case suite_itemcaptionalignment:
	case suite_itemtitleplacement:
	case suite_itemtitlecaptionalignment:
	case suite_itemborderstyle:
	case suite_itempassivestyle:
	case suite_itemcursorfontname:
	case suite_itemcursorbyte:
	case suite_wrappingstyle:
	    suiteev_Clear(SetView);
	    suiteev_FullUpdate(SetView,view_FullRedraw,0,0,
		ContainerWidth,ContainerHeight);
	    if(Scroll) scroll_Update(Scroll);
	    break;
	case suite_itemwidth:
	case suite_itemheight:
	case suite_itemcaptionlist:
	case suite_scroll:
	case suite_borderstyle:
	case suite_bordersize:
	case suite_titlefontlow:
	case suite_titlefonthigh:
	case suite_fontname:
	case suite_itemcaptionfontname:
	case suite_itemcaptionfontlow:
	case suite_itemcaptionfonthigh:
	case suite_itemtitlefontname:
	case suite_itemtitlefontlow:
	case suite_itemtitlefonthigh:
	case suite_itembordersize:
	case suite_itemhighlightstyle:
	case suite_accesstype:
	case suite_itemdataobjectname:
	case suite_itemviewobjectname:
	case suite_guttersize:
        case suite_verticalguttersize:
	case suite_horizontalguttersize:
	case suite_cursorfontname:
	case suite_cursorbyte:
	case suite_titlefontname:
	    suiteev_Clear(SetView);
	    suite_FullUpdate(self,view_FullRedraw,0,0,Bounds.width,Bounds.height);
	    break;
	case suite_sortmode:
	case suite_sorthandler:
	    suite_Sort(self,SortOrder,SortHandler);
	    break;
	case suite_titledataobjecthandler:
	case suite_selectionmode:
	case suite_hithandler:
	case suite_titleviewobjecthandler:
	case suite_titlehithandler:
	case suite_datum:
	    break;
	default: fprintf(stderr,"Suite: Unknown Suite Attribute (%d)\n",attribute);
  }
}

long
suite__ChangeSuiteAttribute( self, attribute, value )
  register struct suite	    *self;
  register long		     attribute, value;
{  
  register long		     status = 0;

  ChangeSuiteAttribute(self,attribute,value);
  return(status);
}

long
suite__SuiteAttribute( self, attribute )
  register struct suite	    *self;
  register long		     attribute;
{
  register long		     value = NULL;

  switch(attribute) {
	case suite_titlecaption:	    value = (long)TitleCaption;		    break;
	case suite_titlehighlightstyle:	    value = (long)TitleHighlightStyle;	    break;
	case suite_titleborderstyle:	    value = (long)TitleBorderStyle;	    break;
	case suite_titlebordersize:	    value = (long)TitleBorderSize;	    break;
	case suite_titledataobjectname:	    value = (long)TitleDataObjectName;	    break;
	case suite_itemorder:		    value = (long)ItemOrder;		    break;
	case suite_itemwidth:		    value = (long)ItemFixedWidth;	    break;
	case suite_itemheight:		    value = (long)ItemFixedHeight;	    break;
	case suite_selectionmode:	    value = (long)SelectionMode;		    break;
	case suite_borderstyle:		    value = (long)BorderStyle;		    break;
	case suite_bordersize:		    value = (long)BorderSize;		    break;
	case suite_hithandler:		    value = (long)self->hithandler;	    break;
	case suite_arrangement:		    value = (long)Arrangement;		    break;
	case suite_scroll:		    value = (long)Scroll;		    break;
	case suite_titleplacement:	    value = (long)TitlePlacement;	    break;
        case suite_titlecaptionalignment:   value = (long)TitleCaptionAlignment;
	  break;
        case suite_fontname:
	case suite_titlefontname:	    value = (long)TitleFontName;	    break;
	case suite_titleviewobject:	    value = (long)TitleViewObject;	    break;
	case suite_titleviewobjectname:	    value = (long)TitleViewObjectName;	    break;
	case suite_titleviewobjecthandler:  value = (long)TitleViewObjectHandler;   break;
	case suite_titledataobjecthandler:  value = (long)TitleDataObjectHandler;   break;
	case suite_titledataobject:	    value = (long)TitleDataObject;	    break;
	case suite_titlefontlow:	    value = (long)self->titlefontlow;	    break;
	case suite_titlefonthigh:	    value = (long)TitleFontHigh;	    break;
	case suite_itemcaptionalignment:    value = (long)CaptionAlignment;	    break;
	case suite_itemtitleplacement:	    value = (long)ItemTitlePlacement;	    break;
	case suite_itemtitlecaptionalignment:    
	  value = (long)ItemTitleCaptionAlignment;	    
	  break;
	case suite_titlehithandler:	    value = (long)TitleHitHandler;	    break;
	case suite_itemcaptionfontname:	    value = (long)CaptionFontName;	    break;
	case suite_itemcaptionfontlow:	    value = (long)CaptionFontLow;	    break;
	case suite_itemcaptionfonthigh:	    value = (long)CaptionFontHigh;	    break;
	case suite_itemtitlefontname:	    value = (long)ItemTitleFontName;	    break;
	case suite_itemtitlefontlow:	    value = (long)self->itemtitlefontlow;   break;
	case suite_itemtitlefonthigh:	    value = (long)self->itemtitlefonthigh;  break;
	case suite_itemborderstyle:	    value = (long)ItemBorderStyle;	    break;
	case suite_itembordersize:	    value = (long)self->itembordersize;    break;
	case suite_itemhighlightstyle:	    value = (long)ItemHighlightStyle;	    break;
	case suite_itempassivestyle:	    value = (long)ItemPassiveStyle;	    break;
	case suite_datum:		    value = (long)Datum;			    break;
	case suite_accesstype:		    value = (long)AccessType;		    break;
	case suite_itemdataobjectname:	    value = (long)ItemDataObjectName;	    break;
	case suite_itemdataobjecthandler:   value = (long)ItemDataObjectHandler;    break;
	case suite_itemviewobjecthandler:   value = (long)ItemViewObjectHandler;    break;
	case suite_itemviewobjectname:	    value = (long)ItemViewObjectName;	    break;
	case suite_guttersize:
	case suite_verticalguttersize:	    value = (long)YGutterSize;		    break;
	case suite_horizontalguttersize:    value = (long)XGutterSize;		    break;
	case suite_sortmode:		    value = (long)SortOrder;		    break;
	case suite_cursorbyte:		    value = (long)CursorByte;		    break;
	case suite_cursorfontname:	    value = (long)CursorFontName;	    break;
        case suite_wrappingstyle:	    value = (long)WrapStyle;		    break;
    default:
	    fprintf(stderr,"Suite: Unknown Suite Attribute (%d)\n",attribute);
	    break;
  }
  return(value);
}

void
suite__ExposeItem( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  item->exposed = TRUE;
  suiteev_Clear(SetView);
  suiteev_FullUpdate(SetView,view_FullRedraw,0,0, ContainerWidth, ContainerHeight);
  if(Scroll) scroll_Update(Scroll);
}

void
suite__HideItem( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  item->exposed = FALSE;
  suiteev_Clear(SetView);
  suiteev_FullUpdate(SetView,view_FullRedraw,0,0, ContainerWidth, ContainerHeight);
  if(Scroll) scroll_Update(Scroll);
}


boolean
suite__ItemHighlighted( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  if(!item) return(FALSE);
  return(Highlighted(item));
}

long
suite__HighlightItem( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  register long			 status = 0, i = 0;
  boolean			 onScreen = FALSE;
  register struct suite_item	*this_one = NULL;

  if(!Items || !ITEM(0) || !item) return;
  if(IsLinked) {
    i = vector_Subscript(Items,(long)FirstVisible);
    if(SelectionMode & suite_Exclusive) 
      suite_Reset(self,suite_Normalize);
    while(this_one = ITEM(i++))
      if(item == this_one) {
	onScreen = TRUE;
	break;
      }
      else if(this_one == LastVisible) 
	break;
    if(Exposed(item) && !Active(item))
      suite_ActivateItem(self,item);
  }
  if(onScreen) 
    suiteev_ItemHighlight(SetView,item);
  else 
    item->mode = ((item_Active | item_Highlighted) & ~item_Normalized);
  CurrentItem = item;
  return(status);
}

boolean
suite__ItemNormalized( self, item )
  register struct suite		*self;
  register struct suite_item    *item;
{
  if(!item) return(FALSE);
  return(Normalized(item));
}

long
suite__NormalizeItem( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  register long			 status = 0, i = 0;
  boolean			 onScreen = FALSE;
  register struct suite_item	*this_one = NULL;

  if(!Items || !ITEM(0) || !item) return;
  if(IsLinked) {
    i = vector_Subscript(Items,(long)FirstVisible);
    if(SelectionMode & suite_Exclusive) 
      suite_Reset(self,suite_Normalize);
    while(this_one = ITEM(i++))
      if(item == this_one) {
	onScreen = TRUE;
	break;
      }
      else if(this_one == LastVisible) 
	break;
    if(Exposed(item) && !Active(item))
      suite_ActivateItem(self,item);
  }
  if(onScreen) 
    suiteev_ItemNormalize(SetView,item);
  else item->mode = ((item_Active | item_Normalized) & ~item_Highlighted);
  CurrentItem = item;
  return(status);
}

boolean
suite__ItemActivated( self, item )
  register struct suite		*self;
  register struct suite_item	*item;
{
  if(!item) return(FALSE);
  return(Active(item));
}

static void
SetItemAttribute( self, item, attribute, value )
  register struct suite		*self;
  register struct suite_item	*item;
  register long			 attribute, value;    
{
  char				 Name[101], *tmp = NULL;
  long				 Size, Type;

  switch(attribute) {
	case suite_itemname:
                AllocNameSpace(&tmp,(char*)value);
	        AllocNameSpace(&item->name,strip(tmp));
	        if(tmp) free(tmp);
	        break;
	case suite_itemposition:
	        MaxItemPosGiven = MAX(MaxItemPosGiven,value);
	        item->position = value;
		break;
	case suite_itemcaption:
                AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&item->caption,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_itemcaptionfontname:
                AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		if(tmp) free(tmp);
		AllocNameSpace(&item->captionfontname,Name);
		item->captionfontsize = Size;
		item->captionfonttype = Type;
		item->captionfont = NULL;
		break;
	case suite_itemtitlecaption:
                AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&item->title,strip(tmp));
		if(tmp) free(tmp);
		break;
	case suite_itemtitlefontname:
                AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		if(tmp) free(tmp);
		AllocNameSpace(&item->titlefontname,Name);
		item->titlefontsize = Size;
		item->titlefonttype = Type;
		item->titlefont = NULL;
		break;
	case suite_itemtitleplacement:
		item->titleplacement = value;
		break;
	case suite_itemtitlecaptionalignment:
	        item->titlecaptionalignment = value;
	        break;
	case suite_itemcaptionalignment:
	        item->captionalignment = value;
	        break;
	case  suite_accesstype:
		if((item->accesstype = (unsigned)value) == suite_ReadWrite)
		    suiteev_SetItemToReadWrite(SetView,item);
		break;
	case suite_itemviewobjectname:
                AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&item->viewobjectname,strip(tmp));
		if(tmp) free(tmp);
	        if(item->viewobject) {
		    view_UnlinkTree(item->viewobject);
		    view_Destroy(item->viewobject);
	        }
		item->viewobject = 
		    (struct view*)class_NewObject(item_ViewObjectName);
		break;
	case suite_itemdataobjectname:
                AllocNameSpace(&tmp,(char*)value);
		AllocNameSpace(&item->dataobjectname,strip(tmp));
		if(tmp) free(tmp);
	        if(item->dataobject) dataobject_Destroy(item->dataobject);
		item->dataobject = 
		    (struct dataobject*)class_NewObject(item_DataObjectName);
		break;
	case suite_itemdataobjecthandler:
		item->dataobjecthandler = (long(*)())value;
		break;
	case suite_itemviewobjecthandler:
		item->viewobjecthandler = (long(*)())value;
		break;
	case suite_itemhithandler:
		item->hithandler = (long(*)())value;
		break;
	case suite_itemdatum:
		item->datum = value;
		break;
	case suite_bordersize:
		item->bordersize = (short)value;
		break;
	case suite_itemhighlightstyle:
		item->highlightstyle = value;
		break;
	case suite_itemcursorfontname:
                AllocNameSpace(&tmp,(char*)value);
		ParseFontFullName(self,strip(tmp),Name,100,&Size,&Type);
		AllocNameSpace(&item->cursorfontname,Name);
		item->cursorfont = suite_BuildFont(self,tmp,&Size);
		if(tmp) free(tmp);
		if(item->cursor) {
		    cursor_Destroy(item->cursor);
		    item->cursor = NULL;
		}
		break;
	case suite_cursorbyte:
		item->cursorbyte = (char) value;
		if(item->cursor) {
		    cursor_Destroy(item->cursor);
		    item->cursor = NULL;
		}
		break;
	case suite_itemcursorbyte:
		item->cursorbyte = (char) value;
		if(item->cursor) {
		    cursor_Destroy(item->cursor);
		    item->cursor = NULL;
		}
		break;
	default:
		fprintf(stderr, "Suite: Unknown Item Attribute (%d)\n", attribute);
		break;
  }
}

long
suite__SetItemAttribute( self, item, attribute, value )
  register struct suite		*self;
  register struct suite_item	*item;
  register long			 attribute, value;
{  
  register long			 status = 0;

  SetItemAttribute(self,item,attribute,value);
  return(status);
}

static void
ChangeItemAttribute( self, item, attribute, value )
  register struct suite		*self;
  register struct suite_item	*item;
  register long			 attribute, value;    
{
  char				*tmp = NULL;

  SetItemAttribute(self,item,attribute,value);
  switch(attribute) {
	case suite_itemcaption:
	    AllocNameSpace(&tmp,(char*)value);
	    ChangeItemCaption(self,item,strip(tmp));
	    if(tmp) free(tmp);
	    break;
	case suite_itemdataobjectname:
	    if(item->viewobject) {
		view_UnlinkTree(item->viewobject);
		view_Destroy(item->viewobject);
	    }
	    AllocNameSpace(&item->viewobjectname, dataobject_ViewName(item->dataobject));
	    item->viewobject = 
	      (struct view*) class_NewObject(item->viewobjectname);
	    view_SetDataObject(item->viewobject, item->dataobject);
	    break;
	case suite_itemviewobjectname:
	    if(item->dataobject)
		view_SetDataObject(item->viewobject, item->dataobject);
	    suiteev_ItemUpdate(SetView, item);
	    break;
	case suite_itemdataobjecthandler: 
	case suite_itemhithandler: 
	case suite_itemdatum:
	case suite_itemviewobjecthandler:    
	case suite_itemcaptionfontlow:
	case suite_itemcaptionfonthigh:
	case suite_itemtitlefontlow:
	case suite_itemtitlefonthigh:
	    break;
	case suite_itemcaptionfontname:
	case suite_itemtitlecaption:
	case suite_itemtitlefontname:
	case suite_itemtitleplacement:
	case suite_itemtitlecaptionalignment:
	case suite_itemcaptionalignment:
	case suite_accesstype:
	case suite_itembordersize:
	case suite_itemborderstyle:
	case suite_itemhighlightstyle:
	case suite_itempassivestyle:
	case suite_itemcursorfontname:
	case suite_itemcursorbyte:
	case suite_itemheight:
	case suite_itemwidth:
	case suite_itemposition:
	    suiteev_ItemUpdate(SetView,item);
	    break;
	default:
	    fprintf(stderr,"Suite: Unknown Item Attribute (%d)\n",attribute);
	    break;
  }
}

long
suite__ChangeItemAttribute( self, item, attribute, value )
  register struct suite		*self;
  register struct suite_item    *item;
  register long			 attribute, value;
{  
  register long			 status = 0;

  ChangeItemAttribute(self,item,attribute,value);
  return(status);
}

long
suite__ItemAttribute( self, item, attribute )
  register struct suite		*self;
  register struct suite_item    *item;
  register long			 attribute;
{  
  register long			 value = 0;

  switch(attribute) {
	case suite_itemposition:
	        if(Items)
		    value = vector_Subscript(Items,(long)item);
		if(value != -1)	value += 1;		break;
	case suite_itemcaption:
		value = (long) item_Caption;		break;
        case suite_itemname:		    
		value =	(long) item_Name;		break;
	case suite_itemcaptionfontname:
		value = (long) item->captionfontname;	break;
	case suite_itemtitlecaption:
		value = (long) item->title;		break;
	case suite_itemtitlefontname:
		value = (long) item->titlefontname;	break;
	case suite_itemtitleplacement:
		value = (long) item->titleplacement;	break;
	case suite_itemtitlecaptionalignment:
		value =	(long) item_TitleCaptionAlignment;  break;
	case  suite_itemcaptionalignment:
		value = (long) item_CaptionAlignment;	break;
	case  suite_accesstype:
		value = (long) item_AccessType;		break;
	case suite_itemdataobjectname:
		value = (long) item->dataobjectname;	break;
	case suite_itemdataobjecthandler:
		value = (long) item->dataobjecthandler;	break;
	case suite_itemviewobjecthandler:
		value = (long) item->viewobjecthandler;	break;
	case suite_itemviewobjectname:
		value = (long) item->viewobjectname;	break;
	case suite_itemhithandler:
		value = (long) item->hithandler;	break;
	case suite_itemdatum:
		value = (long) item->datum;		break;
	case suite_bordersize:
		value = (long) item->bordersize;	break;
	case suite_itemhighlightstyle:
		value = (long) item->highlightstyle;	break;
	case suite_itemcursorfontname:
		value = (long) item->cursorfontname;	break;
	case suite_itemcursorbyte:
		value = (long) item->cursorbyte;	break;
	default:
		fprintf(stderr,"Suite: Unknown Item Attribute (%d)\n",attribute);
		break;
  }
  return(value);
}

struct suite_item *
suite__ItemOfDatum( self, datum )
  register struct suite		*self;
  register long			 datum;
{
  register struct suite_item	*item = NULL;
  register int			 i = 0;

  if(Items && ITEM(0))
    while(item = ITEM(i++)) if(datum == (long)item->datum) 
      return(item);
  return(NULL);
}

struct suite_item **
suite__ItemsOfDatum( self, datum )
  register struct suite		*self;
  register long			 datum;
{
  register int			 i = 0;
  register struct suite_item	*item = NULL;
  register int			 count = 0;

  if(Items && ITEM(0)) {
    while(item = ITEM(i++)) if(datum == (long)item->datum) count++;
    suiteev_AllocItemArray(SetView,count);
    i = count = 0;
    while(item = ITEM(i++))
      if(datum == (long)item->datum) ItemArray[count++] = item;
  }
  else return(NULL);
  return(ItemArray);
}

struct suite_item *
suite__ItemOfName( self, name )
  register struct suite		*self;
  register char			*name;
{
  register struct suite_item	*item = NULL;
  register int			 i = 0;

  if(Items && ITEM(0))
    while(item = ITEM(i++)) 
      if((!name || !(*name)) &&
	  (suite_ItemAttribute(self,item,suite_ItemName(0)) == (long)name))
	  return(item);
      else if(!strcmp(name,suite_ItemAttribute(self,item,suite_ItemName(0))))
	  return(item);
  return(NULL);
}

struct suite_item **
suite__ItemsOfName( self, name )
  register struct suite		*self;
  register char			*name;
{
  register int			 i = 0, count = 0;
  register struct suite_item	*item = NULL;

  if(Items && ITEM(0)) {
    while(item = ITEM(i++))
      if((!name || !(*name)) &&
	  (suite_ItemAttribute(self,item,suite_ItemName(0)) == (long)name))
	  count++;
      else if(!strcmp(name,suite_ItemAttribute(self,item,suite_ItemName(0)))) 
	  count++;
    suiteev_AllocItemArray(SetView,count);
    i = count = 0;
    while(item = ITEM(i++))
      if((!name || !(*name)) &&
	  (suite_ItemAttribute(self,item,suite_ItemName(0)) == (long)name))
	  ItemArray[count++] = item;
      else if(!strcmp(name,suite_ItemAttribute(self,item,suite_ItemName(0))))
	  ItemArray[count++] = item;
  }
  else return(NULL);
  return(ItemArray);
}

struct suite_item *
suite__ItemAtPosition( self, position )
  register struct suite		*self;
  register long			 position;
{
  register struct suite_item	*item = NULL;

  IN(suite_ItemAtPosition);
  if((position > 0) && (position < (suite_ItemCount(self) + 1)))
    item = ITEM(position-1);
  OUT(suite_ItemAtPosition);
  return(item);
}

static void
DefaultExceptionHandler( self )
  register struct suite    *self;
{
  char			    msg[1000];
  long			    result;
  static char		   *continue_choice[2] = {"continue",0};

  sprintf(msg, "Suite: DefaultExceptionHandler:: exception code '%d' detected.",
	suite_ExceptionCode(self) );
  message_MultipleChoiceQuestion(self,100,msg,0,&result,continue_choice,NULL);
  if(ExceptionItem) {
    sprintf(msg, "Suite: DefaultExceptionHandler:: exception item caption '%s'.",
	     suite_ItemAttribute(self,ExceptionItem,suite_ItemCaption(0)));
    message_MultipleChoiceQuestion(self,100,msg,0,&result,continue_choice,NULL);
  }
}

static void
HandleException( self, item, code )
  register struct suite		*self;
  register struct suite_item	*item;
  register long			code;
{
  DEBUGdt(Code,code);
  ExceptionStatus = code;
  ExceptionItem = item;
  if(ExceptionHandler) 
    ExceptionHandler(ClientAnchor,self,item,ExceptionStatus);
  else DefaultExceptionHandler(self);
}

static void
ValidateItem( self, item )
  register struct suite		    *self;
  register struct suite_item	    *item;
{
  if(!item || !Items || 
      (Items && (vector_Subscript(Items,(unsigned int)item)) == -1))
    HandleException(self,item,suite_NonExistentItem);
}

void
suite__HighlightTitle( self )
  struct suite		*self;
{
  unsigned		 type = 0;
  struct rectangle	*rect = rectangle_Duplicate(&TitleRect);

  if(TitleHighlightStyle & (suite_Bold | suite_Italic)) {
    if(TitleHighlightStyle & suite_Bold) type = fontdesc_Bold;
    if(TitleHighlightStyle & suite_Italic) type |= fontdesc_Italic;
    TitleFont = fontdesc_Create(TitleFontName,RealTitleFontType = type,TitleFontSize);
    DrawTitle(self,rect);
  }
  else if(TitleHighlightStyle == suite_Invert) {
    SetTransferMode(self,graphic_INVERT);
    suite_FillRect(self,rect,suite_BlackPattern(self));
  }
  else if(TitleHighlightStyle == suite_Border) {
    DecrementRect(rect,1);
    DrawOutline(self,rect,TitleBorderSize,TitleBorderStyle);
  }
  TitleHighlighted = TRUE;
}

void
suite__NormalizeTitle( self )
  struct suite		*self;
{
  struct rectangle	*rect = rectangle_Duplicate(&TitleRect);

  RealTitleFontType = TitleFontType;
  if(TitleHighlightStyle & (suite_Bold | suite_Italic)) {
    TitleFont = fontdesc_Create(TitleFontName,RealTitleFontType,TitleFontSize);
    DrawTitle(self,rect);
  }
  else if(TitleHighlightStyle == suite_Invert) {
    SetTransferMode(self,graphic_INVERT);
    suite_FillRect(self,rect,suite_BlackPattern(self));
  }
  else if(TitleHighlightStyle == suite_Border) {
    DrawTitle(self,rect);
  }
  TitleHighlighted = FALSE;
}

static void 
DrawRectSize(self,x, y,width,height)
  struct suite	    *self;
  long		     x, y, width, height;
{
  long left = x;
  long right = x+width-1;
  long top = y;
  long bottom = y+height-1;

  if(left > right) left = right;
  if(top > bottom) top = bottom;

  suite_MoveTo(self,left,top);
  suite_DrawLineTo(self,right,top);
  suite_DrawLineTo(self,right,bottom);
  suite_DrawLineTo(self,left,bottom);
  suite_DrawLineTo(self,left,top);
}

static void 
DrawRect(self, Rect)
  struct suite	    *self;
  struct rectangle  *Rect;
{
  DrawRectSize(self,rectangle_Left(Rect),rectangle_Top(Rect), rectangle_Width(Rect), rectangle_Height(Rect));
}

/***  Following in support of Ness  ***/

struct nametbl {
	unsigned char	*name;
	long		 index;
	struct nametbl	*valtbl;
};


/*** Ordering ***/
static struct nametbl OrderVals[] = {
	{ (unsigned char*)"suite_ColumnMajor", suite_ColumnMajor,NULL },
	{ (unsigned char*)"suite_RowMajor", suite_RowMajor, NULL },
	{ (unsigned char*)NULL, 0, NULL } 
};

/***  Arrangements  ***/
static struct nametbl ArrangementVals[] = {
	{ (unsigned char*)"suite_Matrix", suite_Matrix, NULL },
	{ (unsigned char*)"suite_Column", suite_Column, NULL },
	{ (unsigned char*)"suite_Row", suite_Row, NULL },
	{ (unsigned char*)"suite_Balanced", suite_Balanced, NULL },
	{ (unsigned char*)"suite_Unbalanced", suite_Unbalanced, NULL },
	{ (unsigned char*)"suite_List", suite_List, NULL },
	{ (unsigned char*)"suite_RowLine", suite_RowLine, NULL },
	{ (unsigned char*)"suite_ColumnLine", suite_ColumnLine, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/***  BorderStyles  ***/
static struct nametbl BorderStyleVals[] = {
	{ (unsigned char*)"suite_Rectangle", suite_Rectangle, NULL },
	{ (unsigned char*)"suite_Roundangle", suite_Roundangle, NULL },
	{ (unsigned char*)"suite_Circle", suite_Circle, NULL },
	{ (unsigned char*)"suite_Oval", suite_Oval, NULL },
	{ (unsigned char*)"suite_Invisible", suite_Invisible, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/***  Selection Modes   ***/
static struct nametbl ModeVals[] = {
	{ (unsigned char*)"suite_Exclusive", suite_Exclusive, NULL },
	{ (unsigned char*)"suite_Inclusive", suite_Inclusive, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/***  TitlePlacements  ***/
static struct nametbl PlacementVals[] = {
	{ (unsigned char*)"suite_Left", suite_Left, NULL },
	{ (unsigned char*)"suite_Right", suite_Right, NULL },
	{ (unsigned char*)"suite_Top", suite_Top, NULL },
	{ (unsigned char*)"suite_Bottom", suite_Bottom, NULL },
	{ (unsigned char*)"suite_Center", suite_Center, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/***  Highlight Styles ***/
static struct nametbl HighlightStyleVals[] = {
	{ (unsigned char*)"suite_Invert", suite_Invert, NULL },
	{ (unsigned char*)"suite_Border", suite_Border, NULL },
	{ (unsigned char*)"suite_Bold", suite_Bold, NULL },
	{ (unsigned char*)"suite_Italic", suite_Italic, NULL },
	{ (unsigned char*)"suite_Pale", suite_Pale, NULL },
	{ (unsigned char*)"suite_None", suite_None, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/*** Item Types ***/
static struct nametbl AccessTypeVals[] = {
	{ (unsigned char*)"suite_ReadOnly", suite_ReadOnly, NULL },
	{ (unsigned char*)"suite_ReadWrite", suite_ReadWrite, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

/*** Sort Modes ***/
static struct nametbl SortModeVals[] = {
	{ (unsigned char*)"suite_Alphabetic", suite_Alphabetic, NULL },
	{ (unsigned char*)"suite_Numeric", suite_Numeric, NULL },
	{ (unsigned char*)"suite_Ascend", suite_Ascend, NULL },
	{ (unsigned char*)"suite_Descend", suite_Descend, NULL },
	{ (unsigned char*)"suite_ByCaption", suite_ByCaption, NULL },
	{ (unsigned char*)"suite_ByTitle", suite_ByTitle, NULL },
	{ (unsigned char*)"suite_ByDatum", suite_ByDatum, NULL },
	{ (unsigned char*)NULL, 0, NULL }
};

	/*  suite attributes */
static struct nametbl attrTable[] = {
	{ (unsigned char*)"suite_title", suite_titlecaption, NULL },
	{ (unsigned char*)"suite_titleborderstyle", suite_titleborderstyle, BorderStyleVals },
	{ (unsigned char*)"suite_titlehighlightstyle", suite_titlehighlightstyle, HighlightStyleVals },
	{ (unsigned char*)"suite_titlebordersize", suite_titlebordersize, NULL },
	{ (unsigned char*)"suite_titledataobjectname", suite_titledataobjectname, NULL },
	{ (unsigned char*)"suite_titledataobjecthandler", suite_titledataobjecthandler, NULL },
	{ (unsigned char*)"suite_selectionmode", suite_selectionmode, ModeVals },
	{ (unsigned char*)"suite_titleplacement", suite_titleplacement, PlacementVals },
	{ (unsigned char*)"suite_borderstyle", suite_borderstyle, BorderStyleVals },
	{ (unsigned char*)"suite_hithandler", suite_hithandler, NULL },
	{ (unsigned char*)"suite_arrangement", suite_arrangement, ArrangementVals },
	{ (unsigned char*)"suite_scroll", suite_scroll,	PlacementVals },
	{ (unsigned char*)"suite_titlefontname", suite_titlefontname, NULL },
	{ (unsigned char*)"suite_titleviewobjectname", suite_titleviewobjectname, NULL },
	{ (unsigned char*)"suite_titleviewobjecthandler", suite_titleviewobjecthandler, NULL },
	{ (unsigned char*)"suite_titlefontlow", suite_titlefontlow, NULL },
	{ (unsigned char*)"suite_titlefonthigh", suite_titlefonthigh, NULL },
	{ (unsigned char*)"suite_titlehithandler", suite_titlehithandler, NULL },
	{ (unsigned char*)"suite_itemcaptionfontname", suite_itemcaptionfontname, NULL },
	{ (unsigned char*)"suite_itemcaptionfontlow", suite_itemcaptionfontlow, NULL },
	{ (unsigned char*)"suite_itemcaptionfonthigh", suite_itemcaptionfonthigh, NULL },
	{ (unsigned char*)"suite_itemborderstyle", suite_itemborderstyle, BorderStyleVals },
	{ (unsigned char*)"suite_itembordersize", suite_itembordersize, NULL },
	{ (unsigned char*)"suite_itemhighlightstyle", suite_itemhighlightstyle, HighlightStyleVals },
	{ (unsigned char*)"suite_itempassivestyle", suite_itempassivestyle, HighlightStyleVals },
	{ (unsigned char*)"suite_itemorder", suite_itemorder, OrderVals },
	{ (unsigned char*)"suite_itemcaptionlist", suite_itemcaptionlist, NULL },
	{ (unsigned char*)"suite_itemspec", suite_itemspec, NULL },
	{ (unsigned char*)"suite_bordersize", suite_bordersize, NULL },
	{ (unsigned char*)"suite_datum", suite_datum, NULL },
	{ (unsigned char*)"suite_itemcaption", suite_itemcaption, NULL },
	{ (unsigned char*)"suite_itemtitle", suite_itemtitlecaption, NULL },
	{ (unsigned char*)"suite_itemtitlefontname", suite_itemtitlefontname, NULL },
	{ (unsigned char*)"suite_itemtitleplacement", suite_itemtitleplacement, PlacementVals },
	{ (unsigned char*)"suite_accesstype", suite_accesstype, AccessTypeVals },
	{ (unsigned char*)"suite_itemdataobjectname", suite_itemdataobjectname, NULL },
	{ (unsigned char*)"suite_itemdataobjecthandler", suite_itemdataobjecthandler, NULL },
	{ (unsigned char*)"suite_itemviewobjectname", suite_itemviewobjectname, NULL },
	{ (unsigned char*)"suite_itemviewobjecthandler", suite_itemviewobjecthandler, NULL },
	{ (unsigned char*)"suite_itemhithandler", suite_itemhithandler, NULL },
	{ (unsigned char*)"suite_itemwidth", suite_itemwidth, NULL },
	{ (unsigned char*)"suite_itemheight", suite_itemheight, NULL },
	{ (unsigned char*)"suite_itemtitlefontlow", suite_itemtitlefontlow, NULL },
	{ (unsigned char*)"suite_itemtitlefonthigh", suite_itemtitlefonthigh, NULL },
	{ (unsigned char*)"suite_guttersize", suite_guttersize,	NULL },
	{ (unsigned char*)"suite_verticalguttersize", suite_verticalguttersize,	NULL },
	{ (unsigned char*)"suite_horizontalguttersize", suite_horizontalguttersize, NULL },
	{ (unsigned char*)"suite_sortmode", suite_sortmode, SortModeVals },
	{ (unsigned char*)NULL, 0, NULL }
};

static struct nametbl*
FindIndex( tbl, name )
  register struct nametbl  *tbl;
  register unsigned char   *name;
{
  register struct nametbl  *e;

  for(e = tbl; e->name != NULL && strcmp(name, e->name) != 0; e++) {}
  if(e->name == NULL) return(NULL);
  return(e);
}

static void
SetItems(self, elts)
  register struct suite	*self;
  register char		*elts;
{
  register char		*tmp = NULL, *ret = NULL, **captions = NULL;
  char			*copy = NULL;
  register int		 count = 1, i = 0;

  IN(SetItems);
  if(elts && *elts) {
    tmp = elts;
    AllocNameSpace(&copy,elts);
    if(!copy) return;
    while(ret = (char*)index(tmp,':')) {
      count++;
      tmp = ++ret;
    }
    if(count>0) {
      tmp = copy;
      if(!(captions = (char**) calloc(count+1,sizeof(char*)))) {
	free(copy);
	return;
      }
      for(i = 0;i<count;i++) {
	captions[i] = tmp;
	if(!(ret = (char*)index(tmp,':'))) break;
	*ret = '\0';
	tmp = ++ret;
      } 
      SetCaptionList(self,captions);
    }
    if(copy) free(copy);
  }
  OUT(SetItems);
}

static void
SetAttrByName( self, attr, val )
  register struct suite    *self;
  register unsigned char   *attr;
  register long		    val;
{
  register struct nametbl  *tblelt;

  tblelt = FindIndex(attrTable,attr);
  if(tblelt != NULL) SetSuiteAttribute(self,tblelt->index,val);
}

static void
SetEltAttrByName(self, item, attr, val)
  register struct suite    *self;
  register unsigned char   *item;
  register unsigned char   *attr;
  register long		    val;
{
	/* XXX  DEFER SetEltAttrByName */
}


static void
SetValByName( self, attr, cval )
  register struct suite    *self;
  register unsigned char   *attr;
  register unsigned char   *cval;
{
  register struct nametbl  *tblelt, *valindex;

  tblelt = FindIndex(attrTable,attr);
  if(tblelt->name == NULL) return;
  if (tblelt->valtbl == NULL) return;
  valindex = FindIndex(tblelt->valtbl,cval);
  if(valindex == NULL) return;
  SetSuiteAttribute(self,tblelt->index,valindex->index);
}

static void
SetEltValByName( self, item, attr, cval )
  register struct suite    *self;
  register unsigned char   *item;
  register unsigned char   *attr;
  register unsigned char   *cval;
{
	/* XXX  DEFER SetEltValByName */
}

