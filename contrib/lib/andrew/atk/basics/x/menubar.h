/* **********************************************************************o *\
 *         Copyright IBM Corporation 1990,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/menubar.h,v 1.8 1991/08/24 18:08:18 rr2b Exp $ */
/* $ACIS:im.c 1.6$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/menubar.h,v $ */

#define mb_FullRedraw 0
#define mb_Update 1
#define mb_Exposed 2

struct prefs {
    Display *dpy;
    int ColorDisplay;
    int refcount;
    unsigned long topshadowPixel, cardtopshadowPixel;
    unsigned long bottomshadowPixel, cardbottomshadowPixel;
    unsigned long menutitlesPixel;
    unsigned long menubackgroundPixel;
    unsigned long carditemsPixel;
    unsigned long cardbackgroundPixel;
    unsigned long grayitemPixel;
    unsigned long graytitlePixel;
    Pixmap topshadowPixmap, cardtopshadowPixmap;
    Pixmap bottomshadowPixmap, cardbottomshadowPixmap;
    Pixmap grayPixmap;
    Pixmap grayitemStipple;
    Pixmap graytitleStipple;
    XColor whiteColor, blackColor;
    XFontStruct *titlefont;
    XFontStruct *itemfont;
    XFontStruct *iconfont;
    int depth;
    int menubarheight;
    int vspacing, hspacing;
    int padding;
    int groupspacing;
    struct timeval activatetime;
    unsigned int holdbutton;
    int grayPercentage, topshadowPercentage;
    struct prefs *next;
};

struct gcs {
    GC draw;
    GC select,selecterase;
    unsigned long topshadowPixel, bottomshadowPixel;
    Pixmap topshadowPixmap, bottomshadowPixmap;
    unsigned long grayPixel, whitePixel;
    Pixmap grayStipple;
};

struct mbinit {
    Display *dpy;	    /* display the menu is on */
    int	color;		    /* non-zero if this is a color display */
    Window client;	    /* window the menubar is for */
    Window menuw;	    /* window for the menu card */
    Window cascadew;	    /* window for first level of cascading */
    struct gcs titlegcs;    /* gcs used to draw the menubar */
    struct gcs cardgcs;	    /* gcs for the menu card */	    
    struct gcs cmcgcs;	    /* gcs for the first level of cascading */
    struct prefs *prefs;	    /* user configurable things */
    int x,y;
    unsigned int w,h;
    int	everdrawn;	    /* whether this menubar has ever been drawn */
    int	(*HandleExpose)();  /* function to be called with X expose events for the client window when
	a menu is up */
    long *ExposeData;	    /* Data to be passed to the HandleExpose function for the client */
    void (*FreeItem)();	    /* free a menu items data */
};

    
    
    
struct titem {
    int y;
    char *name;		/* name of this item */
    int	prio;		/* priority of this item */
    struct titem *next;	/* next item */
    char *data;		/* data to be reported when this item is choosen */
    int active;	/* whether this item is active */
    int submenu;	/* true if this item is a submenu */
};

struct tmenu {
    int x,y,w,h,mx,mw,ww,wh;
    int vert,horiz;
    char *title;    /* title if this is a toplevel menu */
    int	titlelen;   /* length of the title */
    int	prio;	    /* priority if this is a toplevel menu */
    int	iwidth;	    /* maximum width of any item */
    int	nitems;	    /* number of items in this menu */
    int lastitem;
    unsigned int groupmask; /* groups which have items have their bit set... groups numbered 0-9 */
    int	groupcount; /* number of item groups used in this menu */
    struct titem *items;    /* linked list of the items */
    struct titem **lookup;
    struct tmenu *next;	/* next menu in the overflow list*/
    Window window;
    struct gcs gcs;
};

struct menubar {
    struct mbinit *mbi;
    char *mainmenu,*moretitle;
    int	lastvm;  /* index of last menu title visible on the menubar (aside from more menu) */
    struct tmenu *overflow;	/* linked list of menus which don't fit */
    struct tmenu **menus;   /* array of all the toplevel menus */
    struct tmenu *lastmenu;	/* the last toplevel menu put up */
    struct tmenu *lasteventin;
    struct tmenu *moremenu;
    int	nmenus;		    /* number of top level menus */
    int	mmenus;		    /* number of pointers allocated for menus */
    int	lastmenuindex;	    /* index into menus of the top menu currently displayed */
    int resort;	    /* whether the menubar needs re-sorting */
    void (*MenuFunction)(); /* function to be called for a menu selection */
    char *MenuFunctionData; /* data for the MenuFunction */
    char *data;		    /* more data */
    int morewidth;
    Bool refit;
};

#ifdef __STDC__
typedef char *(*GetDefaultsFunction)(Display *, char *);
#else
typedef char *(*GetDefaultsFunction)();
#endif

#ifndef __STDC__
extern struct mbinit *mb_Init();
extern void mb_InitWindows();
extern struct menubar *mb_Create();
extern void mb_AddSelection();
extern void mb_DeleteSelection();
extern void mb_HandleConfigure();
extern void mb_Activate();
extern void mb_RedrawMenubar();
extern struct prefs *mb_GetPrefsForDisplay();
extern void mb_SetItemStatus();
extern void mb_HandleConfigure();
extern void mb_KeyboardActivate();
#else
extern struct mbinit *mb_Init();
extern void mb_InitWindows();
extern struct menubar *mb_Create();
extern void mb_AddSelection();
extern void mb_DeleteSelection();
extern void mb_HandleConfigure();
extern void mb_Activate();
extern void mb_RedrawMenubar();
extern struct prefs *mb_GetPrefsForDisplay();
extern void mb_SetItemStatus();
extern void mb_HandleConfigure();
extern void mb_KeyboardActivate();
#endif

