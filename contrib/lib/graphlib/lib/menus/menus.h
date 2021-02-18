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

	gl_menu.h
	Header file for GraphLib menus

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	04-03-91 jth	Created

*/

#ifndef __GL_MENU__
#define __GL_MENU__

#include <X11/gl/window.h>

int GL_AttachButtonMenu(/*Window menu,button*/);

#if 0
typedef struct _GL_MenuParent {
  struct _GL_MParent_s *next;
  struct _GL_MInfo_s *menu;
} GL_MenuParent;
#endif

typedef struct _GL_MenuItemInfo {
  char *name;			/* Item title */
  int pixPos, pixW, pixH;	/* pixel position & size */
  int flags;			/* flags:
				 GL_Msubmenu - this is a submenu
				 GL_Mactive - this is active
				 GL_Mdimmed - this item is disabled
				 GL_Mtitle - this item is a title; don't hilight it
				 GL_Mseparator - draw a line under the item
				 */
  VPTF draw;			/* Draw function:
				   The draw function is called as:
				   draw(minfo,iinfo,alist), where:
				   minfo=GL_MenuInfo pointer for the menu
				   iinfo=GL_MenuItemInfo pointer for the item
				   alist=GL_AssocListPtr for the menu */
  union {
    VPTF action;		/* Action function: called as standard callback */
    struct _GL_MenuInfo *submenu; /* Submenu pointer */
  } data;
  struct _GL_MenuItemInfo *next;	/* Pointer to next item in list */
  GLPointer TextObject,InvObject,LineObject,PolyObject;
} GL_MenuItemInfo;

typedef struct _GL_MenuInfo {
  Window winid,attachid;			/* Menu's window ID */
  GL_MenuItemInfo *items;	/* Pointer to head of item list */
  int pixW, pixH;		/* width and height of menu */
  int pixX, pixY;               /* X,Y position of menu */
  int pixNext;			/* next Y pixel position to use */
  unsigned titlefg,titlebg;	/* Title colors */
  unsigned itemfg,itembg;	/* Item colors */
  unsigned hilitefg,hilitebg;	/* Hilighted colors */
  unsigned dimfg,dimbg;		/* Dimmed colors */
  unsigned framec,shadowc;	/* Frame and shadow color */
  int framew,shadoww;		/* Frame and shadow width */
/*  int fgc,bgc,txc,dimc;		/* Colors */
  int clicks;			/* mask of which buttons are down */
  Window wintopassto;           /* Window to send the button release to. */
} GL_MenuInfo;

void _GL_VA_Init_Menu();
void _GL_VA_Handle_Menu();
void _GL_VA_Execute_Menu();
void _GL_VA_Destroy_Menu();
void _GL_VA_Init_MItem();
void _GL_VA_Handle_MItem();
void _GL_VA_Execute_MItem();
void _GL_VA_Destroy_MItem();
GLPointer _GL_VA_Return_MItem();

void _GL_DrawMenuTextItem();	/* Draw a standard plaintext item */
void _GL_MenuExpose();		/* Expose a menu */
void _GL_MenuRelease();		/* Button-release for non-button menus */
void _GL_MenuMotion();		/* Motion within the menu */
void _GL_MenuEnter();		/* Mouse enters menu */
void _GL_MenuLeave();		/* Mouse leaves menu */
void _GL_MenuButtonAction();	/* Handle a menu tied to a button */

#define GL_Msubmenu	0x01
#define GL_Mactive	0x02
#define GL_Mdimmed	0x04
#define GL_Mtitle	0x08
#define GL_Mseparator   0x10


#define GLM 2100
#define GL_MenuTitleColors GLM
#define GL_MenuItemColors GLM+1
#define GL_MenuHiliteColors GLM+2
#define GL_MenuDimColors GLM+3
#define GL_MenuFrame GLM+4
#define GL_MenuShadow GLM+5
#define GL_MenuFrameCol GLM+6
#define GL_MenuShadowCol GLM+7
#define GL_ComplexMenuItem GLM+8
#define GL_MenuItem GLM+9
#define GL_MenuWindow GLM+10

#define GLMI 2200
#define GL_MItemMenu GLMI
#define GL_MItemName GLMI+1
#define GL_MItemH GLMI+2
#define GL_MItemW GLMI+3
#define GL_MItemAction GLMI+4
#define GL_MSubMenu GLMI+5
#define GL_MItemDim GLMI+6
#define GL_MItemTitle GLMI+7
#define GL_MItemSeparator GLMI+8

#define GL_Menu \
_GL_VA_Init_Menu, \
_GL_VA_Handle_Menu, \
_GL_VA_Execute_Menu, \
_GL_VA_Destroy_Menu, \
_GL_VA_Return_Window

#define GL_MItem \
_GL_VA_Init_MItem, \
_GL_VA_Handle_MItem, \
_GL_VA_Execute_MItem, \
_GL_VA_Destroy_MItem, \
_GL_VA_Return_MItem

#endif /* __GL_MENU__ */

