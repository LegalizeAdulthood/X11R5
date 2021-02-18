/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Jeffrey Hutzelman, Eric Anderson 
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */


/*	GraphLib: An X Windows Programming Library
	Developed at CMU Fall 1990 - Spring 1991
	
	Eric Anderson (ea08@andrew.cmu.edu, eanders@sura.net)
	Jeffrey Hutzelman (jh4o@andrew.cmu.edu, jhutz@drycas.club.cc.cmu.edu)
	
	gl_menus.c
	GraphLib menus
	
	Revision History
	Date     Who	What
	-------- ---	------------------------------
	04/05/91 jth	Created
	7Jun91   eaa    Modified to remove constant XTranslateCoordinates.
	*/

#include <stdio.h>
#include <X11/cursorfont.h>
#include "X11/gl/buttons.h"
#include "X11/gl/menus.h"
#include "X11/gl/textobj.h"
#include "X11/gl/invertrect.h"
#include "X11/gl/lineobj.h"
#include "X11/gl/fillpolyobj.h"

static int _GL_MenuInitFlag=0;
static Cursor _GL_MenuDefaultCursor;
GC _GL_MenuGC;

static void _GL_PopupMenuRelease(),_GL_PopupMenuPress();

static void _GL_CheckMenuInit()
/* check to see if the menu library has been initialized.  If not,
   initialize it.  This is so the user doesn't have to bother with it. */
{
  if (!_GL_MenuInitFlag)
    {
      _GL_MenuDefaultCursor=XCreateFontCursor(display,XC_left_ptr);
      GL_GetGC(RootWindow(display,screen),&_GL_MenuGC, GL_Done);
      _GL_MenuInitFlag=1;
    }
}

static void _InvertMenuItem(iinfo,state)
GL_MenuItemInfo *iinfo;
int state;
{
  GL_InvertObject(iinfo->TextObject,state);
  GL_InvertObject(iinfo->InvObject,state);
  if (iinfo->LineObject!=NULL) 
    GL_InvertObject(iinfo->LineObject,state);
  if (iinfo->PolyObject!=NULL)
    GL_InvertObject(iinfo->PolyObject,state);
}

static void _GL_AddMenuItem(minfo,iinfo)
     GL_MenuInfo *minfo;
     GL_MenuItemInfo *iinfo;
/* Add an item at the end of a menu's item list.  This does the
   actual list insertion, without any checks; therefore, it
   should only be called by the menu code.  It also adds the
   neccessary parent pointer if the new item is a submenu
   minfo: menu to add item to
   iinfo: item to add
   */
{
  GL_MenuItemInfo *ilist;

  if (!minfo->items) {
    minfo->items=iinfo;
  }
  else {
    for (ilist=minfo->items;ilist->next;ilist=ilist->next);
    ilist->next=iinfo;
  }
  iinfo->next=NULL;
}


/*** VA FUNCTIONS FOR MENU CREATION ***/

void _GL_VA_Init_Menu(list)
     GL_DataList list;
{
  GL_MenuInfo *minfo;
  GL_WindowInfo *wininfo;
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Creating new menu\n");
#endif
  
  _GL_CheckMenuInit();
  
  _GL_VA_Init_Window(list);
  minfo=(GL_MenuInfo *)xmalloc(sizeof(GL_MenuInfo));
  wininfo=(GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  
  wininfo->CWattr.cursor=_GL_MenuDefaultCursor;
  wininfo->borderwidth = -1;
  wininfo->CWattr.override_redirect=1;
  wininfo->CWattr.background_pixmap = None;
  wininfo->width=wininfo->height=0;
  minfo->items=0;
  minfo->pixNext=0;
  minfo->pixW=minfo->pixH=0;
  minfo->pixX=minfo->pixY=0;
  minfo->titlefg=minfo->itemfg=minfo->dimfg=minfo->hilitebg=default_foreground;
  minfo->titlebg=minfo->itembg=minfo->dimbg=minfo->hilitefg=default_background;
  minfo->framec=minfo->shadowc=default_foreground;
  minfo->framew=minfo->shadoww = -1;
  minfo->wintopassto=0;
  minfo->attachid = 0;
  minfo->clicks = 0;
  GL_Add_To_DataList(list,_GL_VA_Init_Menu,minfo);
}

static GL_MenuItemInfo *init_MenuItem()
{
  GL_MenuItemInfo *iinfo=(GL_MenuItemInfo *)xmalloc(sizeof(GL_MenuItemInfo));
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Creating a menu item\n");
#endif
  
  iinfo->name=0;
  iinfo->pixPos=0;
  iinfo->pixW=0;
  iinfo->pixH=0;
  iinfo->flags=0;
  iinfo->draw=_GL_DrawMenuTextItem;
  iinfo->data.action=0;
  iinfo->next=0;
  return iinfo;
}

static GL_MenuItemInfo *Handle_SimpleMenuItem(pvar)
va_list *pvar;
{
  GL_MenuItemInfo *iinfo = init_MenuItem();
  char *temp;
  
  temp = va_arg(*pvar,char *);
  iinfo->name = xmalloc(strlen(temp)+1);
  strcpy(iinfo->name,temp);
  iinfo->pixW=XTextWidth(default_font,iinfo->name,strlen(iinfo->name))+10;
  iinfo->pixH = default_font->max_bounds.ascent+
    default_font->max_bounds.descent+2;
  iinfo->data.action = va_arg(*pvar,VPTF);
  return iinfo;
}
  
static void Insert_Item(iinfo,minfo)
GL_MenuItemInfo *iinfo;
GL_MenuInfo *minfo;
{
  iinfo->pixPos=minfo->pixNext;
  if (iinfo->pixW > minfo->pixW) minfo->pixW=iinfo->pixW;
  minfo->pixNext=iinfo->pixPos+iinfo->pixH;
  if (minfo->pixNext>minfo->pixH) minfo->pixH=minfo->pixNext;
  _GL_AddMenuItem(minfo,iinfo);
}

void _GL_VA_Handle_Menu(list,modifier,pvar)
     GL_DataList list;
     long modifier;
     va_list *pvar;
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(list,_GL_VA_Init_Menu);
  GL_WindowInfo *wininfo=
    (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  GL_MenuItemInfo *iinfo;

#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Parsing option #%ld\n",modifier);
#endif
  
  switch (modifier) 
    {
    case GL_MenuTitleColors:
      minfo->titlefg=va_arg(*pvar,unsigned);
      minfo->titlebg=va_arg(*pvar,unsigned);
      break;
    case GL_MenuItemColors:
      minfo->itemfg=va_arg(*pvar,unsigned);
      minfo->itembg=va_arg(*pvar,unsigned);
      break;
    case GL_MenuHiliteColors:
      minfo->hilitefg=va_arg(*pvar,unsigned);
      minfo->hilitebg=va_arg(*pvar,unsigned);
      break;
    case GL_MenuDimColors:
      minfo->dimfg=va_arg(*pvar,unsigned);
      minfo->dimbg=va_arg(*pvar,unsigned);
      break;
    case GL_MenuFrame: minfo->framew=va_arg(*pvar,int);break;
    case GL_MenuShadow: 
      minfo->shadoww=va_arg(*pvar,int);
      if (minfo->framew<0) minfo->framew=1;
      break;
    case GL_MenuFrameCol: minfo->framec=va_arg(*pvar,int);break;
    case GL_MenuShadowCol: minfo->shadowc=va_arg(*pvar,int);break;
    case GL_ComplexMenuItem: 
      iinfo = (GL_MenuItemInfo *)GL_Handle_Varargs(GL_MItem,pvar);
      if (!iinfo) break;
      Insert_Item(iinfo,minfo);
      break;
    case GL_MenuItem:
      iinfo = Handle_SimpleMenuItem(pvar);
      if (!iinfo) break;
      Insert_Item(iinfo,minfo);
      break;      
    case GL_MenuWindow:
      minfo->attachid=va_arg(*pvar, Window);
      break;
    default: _GL_VA_Handle_Window(list,modifier,pvar);
    }
}


void _GL_VA_Execute_Menu(list)
     GL_DataList list;
{
  GL_MenuItemInfo *loop;
  GLPointer textobject,invobject,lineobject,polyobject;
  register int x,y,w,h;
  
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(list,_GL_VA_Init_Menu);
  GL_WindowInfo *wininfo=
    (GL_WindowInfo *)GL_Search_DataList(list,_GL_VA_Init_Window);
  GL_EventMgrInfo *einfo = 
    (GL_EventMgrInfo *)GL_Search_DataList(list,_GL_VA_Init_EventMgr);
  
  if (minfo->framew<0) minfo->framew=0;
  if (minfo->shadoww<0) minfo->shadoww=0;
  if (wininfo->width) minfo->pixW=wininfo->width;
  if (wininfo->height) minfo->pixH=wininfo->height;
  minfo->pixW += minfo->shadoww + 2 * minfo->framew;
  minfo->pixH += minfo->shadoww + 2 * minfo->framew;
  wininfo->width = minfo->pixW;
  wininfo->height = minfo->pixH;
  wininfo->CWattr.event_mask = ExposureMask | ButtonReleaseMask | 
    ButtonPressMask | PointerMotionMask;
  if (wininfo->borderwidth == -1) {
    if (minfo->shadoww || minfo->framew)
      wininfo->borderwidth=0;
    else
      wininfo->borderwidth=1;
  }
  _GL_VA_Execute_Window(list);
  minfo->winid=einfo->winid;
/*  XSetWindowBackgroundPixmap(display, minfo->winid, None);*/
  GL_AddWindowAssocInfo(einfo->winid,_GL_VA_Init_Menu,(GLPointer)minfo);
  GL_AddFuncPtr(einfo->winid,
    Expose, _GL_MenuExpose,
    MotionNotify, _GL_MenuMotion,
    ButtonRelease, _GL_PopupMenuRelease,
    ButtonPress, _GL_PopupMenuPress,
    GL_Done);
  for(loop=minfo->items;loop!=NULL;loop=loop->next) {
    /*@@@ redo to use x,y,w,h */
    x=minfo->framew;
    y=minfo->framew+loop->pixPos;
    w=minfo->pixW-2*minfo->framew-minfo->shadoww;
    h=loop->pixH;
    if ((loop->flags&GL_Mseparator))
      lineobject = GL_Create(GL_LineObject,
			     GLLO_LinePos, minfo->framew,
			       minfo->framew+loop->pixPos+loop->pixH-1,
			       minfo->framew+minfo->pixW-2*minfo->framew-
			         minfo->shadoww,
			       minfo->framew+loop->pixPos+loop->pixH-1,
			     GL_Done);
    else
      lineobject = NULL;
    if (loop->flags&GL_Msubmenu) 
      polyobject = GL_Create(GL_FilledPolygonObject,
			     GLPO_Closed,
			     GLPO_NumPoints, 3,
			     GLPO_Points,
			       x+w-2, y+h/2,
			       x+w-7, (y+h/2-5)<(y+2)?(y+2):(y+h/2-5),
			       x+w-7, (y+h/2+5)>(y+h-2)?(y+h-2):(y+h/2+5),
			     GL_Done);
    else
      polyobject = NULL;
    textobject = GL_Create(GL_TextObject,
			   GLTO_String, loop->name,
			   GLO_X, minfo->framew+5,
			   GLO_Y, minfo->framew+loop->pixPos,
			   GL_Done);
    invobject = GL_Create(GL_InvertibleRectangle,
			  GLO_X, minfo->framew,
			  GLO_Y, minfo->framew+loop->pixPos,
			  GLIR_InvertRect, minfo->framew, minfo->framew+loop->pixPos,
			    minfo->pixW-2*minfo->framew-minfo->shadoww,
			    loop->pixH,
			  GL_Done);
    GL_InstallObject(invobject,minfo->winid);
    GL_InstallObject(textobject,minfo->winid);
    if (lineobject!=NULL)
      GL_InstallObject(lineobject,minfo->winid);
    if (polyobject!=NULL)
      GL_InstallObject(polyobject,minfo->winid);
    loop->TextObject=textobject;
    loop->InvObject=invobject;
    loop->PolyObject=polyobject;
    loop->LineObject=lineobject;
  }
/*  if (minfo->attachid!=0)
    XGrabButton(display,AnyButton,AnyModifier,minfo->attachid,True,
		ButtonPressMask|ButtonReleaseMask|PointerMotionMask,
		GrabModeAsync,GrabModeAsync,None,None);*/
}


void _GL_VA_Destroy_Menu(list)
     GL_DataList list;
{
  _GL_VA_Destroy_Window(list);
}


/*** VA FUNCTIONS FOR MENU ITEM CREATION ***/

void _GL_VA_Init_MItem(list)
     GL_DataList list;
{
  GL_MenuItemInfo *iinfo = init_MenuItem();
  GL_Add_To_DataList(list,_GL_VA_Init_MItem,iinfo);
}


void _GL_VA_Handle_MItem(list,modifier,pvar)
     GL_DataList list;
     long modifier;
     va_list *pvar;
{
  GL_MenuInfo *sminfo=0;
  GL_MenuItemInfo *iinfo=(GL_MenuItemInfo *)GL_Search_DataList(list,_GL_VA_Init_MItem);
  Window smwin;
  char *temp;
  
#ifdef GL_DEBUG_FULL
  fprintf(stderr,"Parsing option #%ld\n",modifer);
#endif
  
  switch (modifier)
    {
    case GL_MItemName:
      if (iinfo->name) free(iinfo->name);
      temp=va_arg(*pvar,char *);
      iinfo->name=xmalloc(strlen(temp)+1);
      strcpy(iinfo->name,temp);
      break;
    case GL_MItemH: iinfo->pixH=va_arg(*pvar,int);break;
    case GL_MItemW: iinfo->pixW=va_arg(*pvar,int);break;
    case GL_MItemAction:
      iinfo->flags &= ~GL_Msubmenu;
      iinfo->data.action = va_arg(*pvar,VPTF);
      break;
    case GL_MSubMenu:
      smwin=(Window)GL_Handle_Varargs(GL_Menu,pvar);
      sminfo=(GL_MenuInfo *)GL_Search_DataList(GL_GetDataList(smwin),
					       _GL_VA_Init_Menu);
      iinfo->flags |= GL_Msubmenu;
      iinfo->data.submenu = sminfo;
      break;
    case GL_MItemDim: iinfo->flags |= GL_Mdimmed;break;
    case GL_MItemTitle: iinfo->flags |= GL_Mtitle;break;
    case GL_MItemSeparator: iinfo->flags |= GL_Mseparator;break;
    default:
      fprintf(stderr,"There was a goofy modifier passed to this function.\n");
      fprintf(stderr,"I really don't know what to do with it.\n");
      fprintf(stderr,"I see two possiblilities.  One, the user of the library\n");
      fprintf(stderr,"barfed, and did their call wrong.  Alternatively, some\n");
      fprintf(stderr,"function which called me didn't handle what it was\n");
      fprintf(stderr,"supposed to.  Either way, the number was %d\n",modifier);
      fprintf(stderr,"and I'm so confused I'm going to exit.  Bye Bye Bucko.\n");
      exit(-1);
    }
}


void _GL_VA_Execute_MItem(list)
     GL_DataList list;
{
  GL_MenuItemInfo *iinfo=(GL_MenuItemInfo *)GL_Search_DataList(list,_GL_VA_Init_MItem);
  
  if (!iinfo->name)
    {
      iinfo->name=(char *)xmalloc(strlen("Menu Item")+1);
      strcpy(iinfo->name,"Menu Item");
    }
  
  if (!iinfo->pixW)
    {
      iinfo->pixW=XTextWidth(default_font,iinfo->name,strlen(iinfo->name))+10;
      if (iinfo->flags & GL_Msubmenu) iinfo->pixW += 10;
    }
  if (!iinfo->pixH)
    iinfo->pixH =
      default_font->max_bounds.ascent+default_font->max_bounds.descent+2;
}


void _GL_VA_Destroy_MItem(list)
     GL_DataList list;
{
}


GLPointer _GL_VA_Return_MItem(list)
{
  GL_MenuItemInfo *iinfo=(GL_MenuItemInfo *)GL_Search_DataList(list,_GL_VA_Init_MItem);
  return (GLPointer)iinfo;
}


static void _GL_DrawMenuTextItem(minfo,iinfo,x,y,w,h)
     GL_MenuInfo *minfo;
     GL_MenuItemInfo *iinfo;
     int x,y,w,h;
/* Draw a text item - the default draw routine
   minfo: menu info structure
   iinfo: item to draw
   alist: assoc list for that menu (for custom routines)
*/
{
#ifdef DOOMED_HAHAHAH
  switch (iinfo->flags & (GL_Mactive | GL_Mdimmed | GL_Mtitle))
    {
    case 0:
      XSetForeground(display,_GL_MenuGC,minfo->itembg);
      XFillRectangle(display,minfo->winid,_GL_MenuGC,x,y,w,h);
      XSetForeground(display,_GL_MenuGC,minfo->itemfg);
      break;
    case GL_Mtitle:
    case GL_Mtitle | GL_Mactive:
      XSetForeground(display,_GL_MenuGC,minfo->titlebg);
      XFillRectangle(display,minfo->winid,_GL_MenuGC,x,y,w,h);
      XSetForeground(display,_GL_MenuGC,minfo->titlefg);
      break;
    case GL_Mactive:		/* active */
      XSetForeground(display,_GL_MenuGC,minfo->hilitebg);
      XFillRectangle(display,minfo->winid,_GL_MenuGC,x,y,w,h);
      XSetForeground(display,_GL_MenuGC,minfo->hilitefg);
      XFlush(display);
      break;
    default:			/* dimmed */
      XSetForeground(display,_GL_MenuGC,minfo->dimbg);
      XFillRectangle(display,minfo->winid,_GL_MenuGC,x,y,w,h);
      XSetForeground(display,_GL_MenuGC,minfo->dimfg);
      if (minfo->dimfg==minfo->itemfg)
	{
	  XSetStipple(display,_GL_MenuGC,default_grey);
	  XSetFillStyle(display,_GL_MenuGC,FillStippled);
	}
      break;
    }
  XDrawString(display,minfo->winid,_GL_MenuGC,5+x,default_font->max_bounds.ascent+y,
    iinfo->name,strlen(iinfo->name));
  if (iinfo->flags & GL_Mseparator &&
    (!(iinfo->flags & GL_Mactive) || iinfo->flags & GL_Mtitle || iinfo->flags & GL_Mdimmed))
    XDrawLine(display, minfo->winid, _GL_MenuGC, x, y + h - 1,
      x + w - 1, y + h - 1);
  if (iinfo->flags & GL_Msubmenu)
    {
      XPoint points[3];
      points[0].x=x + w - 2;
      points[1].x=x + w - 7;
      points[2].x=x + w - 7;
      points[0].y=y + h/2;
      points[1].y=points[0].y - 5;
      points[2].y=points[0].y + 5;
      if (points[1].y < y + 2)
	points[1].y = y + 2;
      if (points[2].y > y + h - 2)
	points[2].y = y + h - 2;
      XFillPolygon(display,minfo->winid,_GL_MenuGC,points,3,Convex,CoordModeOrigin);
    }
  XSetFillStyle(display,_GL_MenuGC,FillSolid);
#endif
}


static void _GL_MapSubmenu(minfo,iinfo)
GL_MenuInfo *minfo;
GL_MenuItemInfo *iinfo;
/* Map a submenu - this takes care of figuring out where
   the submenu should be, moving it, and mapping it.
   minfo: the submenu's parent
   iinfo: the item whose submenu is to be mapped
*/
{
  GL_MenuInfo *sminfo=iinfo->data.submenu;
  int x,y;
  Window child;

  XTranslateCoordinates(display,minfo->winid,RootWindow(display,screen),
    minfo->pixW-minfo->shadoww,iinfo->pixPos+minfo->framew,&x,&y,&child);
  XMoveWindow(display,sminfo->winid,x,y);
  XMapRaised(display,sminfo->winid);
  sminfo->pixX=x;
  sminfo->pixY=y;
}


static void _GL_UnmapMenu(minfo,flag)
     GL_MenuInfo *minfo;
     int flag;
/* Unmap a menu and all its submenus:
   this is used when the button is released, and when a
   submenu becomes inactive.
   minfo: menu to unmap
   flags: whether to clear all the active flags as well as unmapping
*/
{
  GL_MenuItemInfo *iinfo;
  for (iinfo=minfo->items;iinfo && !(iinfo->flags & GL_Mactive);iinfo=iinfo->next);
  if (iinfo)
    {
      if (iinfo->flags & GL_Msubmenu) _GL_UnmapMenu(iinfo->data.submenu,flag);
      if (flag) iinfo->flags &= ~GL_Mactive;
      _InvertMenuItem(iinfo,0);
    }
  XUnmapWindow(display,minfo->winid);
}


#if 0
static int _GL_CheckParentChain(minfo,pminfo)
GL_MenuInfo *minfo, *pminfo;
/* Check to see that minfo isn't equal to parent or
   any if parent's parents.
   Return 0 if there's a problem; non-zero if not.
*/
{
  GL_MenuParent *pinfo;
  if (parent==minfo) return 0;
  for (pinfo=pminfo->parent;pinfo;pinfo=pinfo->next)
    if (!_GL_CheckParentChain(minfo,pinfo->menu)) return 0;
  return 1;
}


int _GL_CheckSubmenuChain(minfo,pminfo)
GL_MenuInfo *minfo,*pminfo;
/* Check to see if a menu is in a valid submenu chain:
   Each time a submenu is added, this function gets called.
   It checks to make sure that this menu is not being
   made a submenu of itself nor of any of its children.
   It does this by traversing the item list, looking for
   submenus, and checking to make sure that none of the submenus
   are in this menu's parent chain.  Lots of fun, but it
   prevents me from having to deal with recursive submenus.
   Returns zero if there's a problem, and non-zero if not.
   minfo: the menu to check
*/
{
  GL_MenuParent *pinfo;
  GL_MenuItemInfo *iinfo;
  if (!GL_CheckParentChain(minfo,pminfo)) return 0;
  for (iinfo=minfo->items;iinfo;iinfo=iinfo->next)
    if ((iinfo->flags & GL_Msubmenu) && !_GL_CheckSubmenuChain(iinfo->data.submenu,pminfo)) return 0;
  return 1;
}
#endif

static void _GL_MenuExpose(report,alist)
     XEvent *report;
     GL_DataList alist;
/* Handle expose events for a menu.  Basically, loop
   through the whole item list, drawing each item
*/
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(alist,_GL_VA_Init_Menu);
  GL_MenuItemInfo *iinfo;
  int i;

  for (iinfo=minfo->items;iinfo;iinfo=iinfo->next)
    if (iinfo->draw)
      (iinfo->draw)(minfo,iinfo,minfo->framew,minfo->framew+iinfo->pixPos,minfo->pixW-2*minfo->framew-minfo->shadoww,iinfo->pixH,alist);
  if (minfo->framew)
    {
      XSetForeground(display,_GL_MenuGC,minfo->framec);
      for (i=0;i<minfo->framew;i++)
	XDrawRectangle(display,minfo->winid,_GL_MenuGC,i,i,minfo->pixW-2*i-minfo->shadoww-1,minfo->pixH-2*i-minfo->shadoww-1);
    }
  if (minfo->shadoww)
    {
      XSetForeground(display,_GL_MenuGC,minfo->shadowc);
      for (i=0;i<minfo->shadoww;i++)
	{
	XDrawLine(display,minfo->winid,_GL_MenuGC,minfo->shadoww+1,minfo->pixH-i-1,minfo->pixW,minfo->pixH-i-1);
	XDrawLine(display,minfo->winid,_GL_MenuGC,minfo->pixW-i-1,minfo->shadoww+1,minfo->pixW-i-1,minfo->pixH);
	}
    }
}

static void _GL_MenuRelease(report,alist)
     XEvent *report;
     GL_DataList alist;
/* Handle the release of the mouse button in a menu:
   search for the active item.  If none, do nothing.  If
   it's a submenu, recurse.  If it's a simple item, call
   it's action function, if any.
*/
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(alist,_GL_VA_Init_Menu);
  GL_MenuItemInfo *iinfo;
  
  _GL_UnmapMenu(minfo,0);		/* don't clear active flags */

  for (;;)
    {
      for (iinfo=minfo->items;iinfo && !(iinfo->flags & GL_Mactive);iinfo=iinfo->next);
      if (iinfo)
	{
	  iinfo->flags &= ~GL_Mactive;
	  _InvertMenuItem(iinfo,0);
	  if (iinfo->flags & GL_Msubmenu)
	    {
	      minfo=iinfo->data.submenu;
	      continue;
	    }
	}
      break;
    }
  if (iinfo && iinfo->data.action)
    (iinfo->data.action)(report,GL_GetDataList(minfo->winid),minfo->winid);
}


static void _GL_XMenuMotion(report,minfo)
XEvent *report;
GL_MenuInfo *minfo;
/* Handle motion of the mouse pointer in a menu:
   There are three possible cases:

   The new active item and the old active item are equal:
     No action is necessary; the hilighted item hasn't changed - either they're
     the same item, or nothing is hilighted at all.

   The new active item is non-null, but different from the old active item:
     We must unmark the old item and unmap the submenu if there is one.
     Then, mark the new item and map it's submenu, if any.

   The new active item is null, and the old active item is not:
     If the old active item has a submenu, it is possible that the pointer
     is in the submenu.  To deal with this, we must call ourselves on the submenu.
*/
{
  GL_MenuItemInfo *oinfo,*iinfo;
  int x=report->xmotion.x,y=report->xmotion.y;
  XEvent newreport;
  Window child;

  for (oinfo=minfo->items;oinfo && !(oinfo->flags & GL_Mactive);oinfo=oinfo->next);
  if (x<0 || y<0 || x >= minfo->pixW-minfo->shadoww || y >= minfo->pixH-minfo->shadoww) iinfo=0;
  else
    for (iinfo=minfo->items;iinfo && y >= minfo->framew + iinfo->pixPos + iinfo->pixH;iinfo=iinfo->next);
  if (iinfo==oinfo) return;
  if (iinfo)
    {
      if (oinfo)
	{
	  oinfo->flags &= ~GL_Mactive;
	  _InvertMenuItem(oinfo,0);
	  if (oinfo->flags & GL_Msubmenu)
	    _GL_UnmapMenu(oinfo->data.submenu,1);
	}
      iinfo->flags |= GL_Mactive;
      if (!(iinfo->flags&GL_Mtitle)) 
	_InvertMenuItem(iinfo,1);
      if (iinfo->flags & GL_Msubmenu)
	{
	_GL_MapSubmenu(minfo,iinfo);
	XMapWindow(display,iinfo->data.submenu->winid);
	}
    }
  else
    {
      if (oinfo->flags & GL_Msubmenu)
	{
	  newreport = *report;
	  newreport.xmotion.x=newreport.xmotion.x_root-oinfo->data.submenu->pixX;
	  newreport.xmotion.y=newreport.xmotion.y_root-oinfo->data.submenu->pixY;
	  newreport.xmotion.window=oinfo->data.submenu->winid;
	  _GL_XMenuMotion(&newreport,oinfo->data.submenu);
	  oinfo=0;
	}
      else
	{
	  oinfo->flags &= ~GL_Mactive;
	  _InvertMenuItem(oinfo,0);
	}
    }
  if (oinfo && oinfo->draw)
    (oinfo->draw)(minfo,oinfo,minfo->framew,minfo->framew+oinfo->pixPos,
      minfo->pixW-2*minfo->framew-minfo->shadoww,oinfo->pixH,GL_GetDataList(minfo->winid));
  if (iinfo && iinfo->draw)
    (iinfo->draw)(minfo,iinfo,minfo->framew,minfo->framew+iinfo->pixPos,
      minfo->pixW-2*minfo->framew-minfo->shadoww,iinfo->pixH,GL_GetDataList(minfo->winid));
}


static void _GL_MenuMotion(report,alist)
XEvent *report;
GL_DataList alist;
/* the menu motion event handler */
{
  _GL_XMenuMotion(report,GL_Search_DataList(alist,_GL_VA_Init_Menu));
}


static void _GL_MenuButtonAction(report,alist)
XEvent *report;
GL_DataList alist;
/* Handle a menu as the action of a button.  If it's a button release,
   we just call _GL_MenuRelease.  If it's a button press, we have to set
   up all sorts of neat stuff.
*/
{
  GL_ButtonInfo *binfo=(GL_ButtonInfo *)GL_Search_DataList(alist,_GL_VA_Init_Button);
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(alist,_GL_VA_Init_Menu);
  GL_MenuItemInfo *iinfo;
  int x,y,rel_x,rel_y;
  unsigned int nchildren;
  unsigned int w,h,bw,depth;
  Window root,parent,*children,child;

  if (report->type==ButtonRelease)
  /* Called when released. */
    return;
  
  XGetGeometry(display,binfo->winid,&root,&rel_x,&rel_y,&w,&h,&bw,&depth);
  XQueryTree(display,binfo->winid,&root,&parent,&children,&nchildren);
  XTranslateCoordinates(display,parent,root,rel_x,rel_y,&x,&y,&child);
  XMoveWindow(display,minfo->winid,x,y);
  for (iinfo=minfo->items;iinfo && (iinfo->pixPos > report->xbutton.y);iinfo=iinfo->next);
  if (iinfo)
    {
      iinfo->flags |= GL_Mactive;
      _InvertMenuItem(iinfo,0);
      if (iinfo->flags & GL_Msubmenu)
	_GL_MapSubmenu(minfo,iinfo);
    }
  XMapRaised(display,minfo->winid);
  XGrabPointer(display,minfo->winid,True,PointerMotionMask|
	       ButtonReleaseMask|ButtonPressMask,
	       GrabModeAsync,GrabModeAsync,None,
	       _GL_MenuDefaultCursor,report->xbutton.time);
  GL_AddFuncPtr(binfo->winid,
		MotionNotify, _GL_MenuMotion,
		GL_Done);
}


int GL_AttachButtonMenu(menu,button)
Window menu,button;
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(GL_GetDataList(menu),_GL_VA_Init_Menu);
  GL_ButtonInfo *binfo=(GL_ButtonInfo *)GL_Search_DataList(GL_GetDataList(button),_GL_VA_Init_Button);
/*  XWindowAttributes oldattr;
  XSetWindowAttributes newattr;*/

  if (!minfo || !binfo) return 0;
  if (binfo->action) return 0;

  binfo->flags |= GL_BaAction;
  binfo->action=_GL_MenuButtonAction;
/*  XGetWindowAttributes(display,binfo->winid,&oldattr);
  newattr.event_mask = oldattr.your_event_mask | PointerMotionMask;
  XChangeWindowAttributes(display,binfo->winid,CWEventMask,&newattr);*/
  GL_AddWindowAssocInfo(button,_GL_VA_Init_Menu,(GLPointer)minfo);
  minfo->wintopassto=button;
  return 1;
}

static void _GL_PopupMenuRelease(report,alist)
XEvent *report;
GL_DataList alist;
{
  Window origwin;
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(alist,_GL_VA_Init_Menu);

  minfo->clicks &= ~(1<<(report->xbutton.button));
  if (minfo->clicks != 0) 
    return;
  _GL_MenuRelease(report,alist);
  XUngrabPointer(display,CurrentTime);
  if (minfo->wintopassto) {
    origwin = report->xany.window;
    report->xany.window = minfo->wintopassto;
    GL_ProcessEvent(report);
    report->xany.window = origwin;
  }
}

static void _GL_PopupMenuPress(report,alist)
XEvent *report;
GL_DataList alist;
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(alist,_GL_VA_Init_Menu);

  minfo->clicks|=(1<<report->xbutton.button);
}

void GL_HandleMenu(report,mwin)
XEvent *report;
Window mwin;
{
  GL_MenuInfo *minfo=(GL_MenuInfo *)GL_Search_DataList(GL_GetDataList(mwin),_GL_VA_Init_Menu);
  GL_MenuItemInfo *iinfo;
  int x,y,new_x,new_y;
  Window child;

  if (report->type != ButtonPress) return;
  XTranslateCoordinates(display,report->xbutton.window,
			RootWindow(display,screen),
			report->xbutton.x,report->xbutton.y,
			&new_x,&new_y,&child);
  x = new_x - minfo->pixW / 2;
  if (x < 0) x = 0;
  if (x + minfo->pixW > DisplayWidth(display,screen)) 
    x = DisplayWidth(display,screen) - minfo->pixW;
  y = new_y - minfo->items->pixH / 2; /* height of top item */
  if (y < 0) y = 0;
  if (y + minfo->pixH > DisplayHeight(display,screen)) 
    y = DisplayHeight(display,screen) - minfo->pixH;
  XMoveWindow(display,minfo->winid,new_x,new_y);
  for (iinfo=minfo->items;
       iinfo && (iinfo->pixPos > report->xbutton.y);
       iinfo=iinfo->next)
    ;
  if (iinfo)
    {
      iinfo->flags |= GL_Mactive;
      _InvertMenuItem(iinfo,0);
      if (iinfo->flags & GL_Msubmenu)
	_GL_MapSubmenu(minfo,iinfo);
    }
  XMapRaised(display,minfo->winid);
  XGrabPointer(display,minfo->winid,False,ButtonReleaseMask | 
	       ButtonPressMask | PointerMotionMask,
	       GrabModeAsync,GrabModeAsync,None,_GL_MenuDefaultCursor,
	       report->xbutton.time);
  minfo->clicks|=(1<<report->xbutton.button);
}
