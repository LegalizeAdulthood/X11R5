#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty_menu.c 70.6 91/10/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Ttysw menu initialization and call-back procedures
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/signal.h>

#include <stdio.h>
#include <ctype.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/win_input.h>
#include <xview/frame.h>
#include <xview/scrollbar.h>
#include <xview/ttysw.h>
#include <xview/termsw.h>
#include <xview/textsw.h>
#include <xview/notice.h>
#include <xview/openmenu.h>
#include <xview/selection.h>
#include <xview/sel_svc.h>
#include <xview/server.h>
#include <xview_private/tty_impl.h>
#include <xview_private/term_impl.h>
#include <xview/svrimage.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#include <X11/Xlib.h>
#include <X11/XSunExt.h>  
#endif OW_I18N

#define HELP_INFO(s) XV_HELP_DATA, s,

extern void     ttygetselection();
extern int      textsw_file_do_menu_action();
extern void     termsw_menu_set(), termsw_menu_clr();

#define EDITABLE		0
#define READ_ONLY		1
#define ENABLE_SCROLL		2
#define DISABLE_SCROLL		3



/* shorthand */
#define	iwbp	ttysw->ttysw_ibuf.cb_wbp
#define	irbp	ttysw->ttysw_ibuf.cb_rbp

/* ttysw walking menu definitions */

static Menu_item ttysw_menu_page_state();
Pkg_private void ttysw_show_walkmenu();

static void     ttysw_enable_scrolling();
static void     ttysw_disable_scrolling();
static void     ttysw_menu_page();
static void     ttysw_menu_copy();
static void     ttysw_menu_paste();



/* termsw walking menu definitions */

#ifndef OW_I18N
static char    *ttysw_scrolling_disable = "Disable Scrolling";
static char    *ttysw_scrolling_enable = "Enable Scrolling";
#endif


int             ITEM_DATA_KEY;


/* ttysw walking menu utilities */


Menu
ttysw_walkmenu(ttysw_folio_public)
    Tty             ttysw_folio_public;
{				/* This create a ttysw menu */
    Menu            ttysw_menu;
    Menu_item       page_mode_item, enable_scroll_item, copy_item, paste_item;

    ttysw_menu = xv_create(XV_SERVER_FROM_WINDOW(ttysw_folio_public), MENU,
			   HELP_INFO("ttysw:menu")
			   0);

    page_mode_item = xv_create(NULL,
			       MENUITEM,
			       MENU_STRING,
#ifdef OW_I18N
				  XV_I18N_MSG("xv_messages", "Disable Page Mode"),
#else
				  "Disable Page Mode",
#endif
			       MENU_ACTION, ttysw_menu_page,
			       MENU_GEN_PROC, ttysw_menu_page_state,
			       MENU_CLIENT_DATA, ttysw_folio_public,
			       HELP_INFO("ttysw:mdsbpage")
			       0);


    copy_item = xv_create(NULL,
			  MENUITEM,
			  MENU_STRING,
#ifdef OW_I18N
			  XV_I18N_MSG("xv_messages", "Copy"),
#else
			  "Copy",
#endif
			  MENU_ACTION, ttysw_menu_copy,
			  MENU_CLIENT_DATA, ttysw_folio_public,
			  HELP_INFO("ttysw:mcopy")
			  0);

    paste_item = xv_create(NULL,
			   MENUITEM,
			   MENU_STRING,
#ifdef OW_I18N
			   XV_I18N_MSG("xv_messages", "Paste"),
#else
			   "Paste",
#endif
			   MENU_ACTION, ttysw_menu_paste,
			   MENU_CLIENT_DATA, ttysw_folio_public,
			   HELP_INFO("ttysw:mpaste")
			   0);



    (void) xv_set(ttysw_menu,
		  MENU_TITLE_ITEM,
#ifdef OW_I18N
		  XV_I18N_MSG("xv_messages", "Term Pane"),
#else	
		  "Term Pane",
#endif
		  MENU_APPEND_ITEM, page_mode_item,
		  MENU_APPEND_ITEM, copy_item,
		  MENU_APPEND_ITEM, paste_item,
		  0);


    if (IS_TERMSW(ttysw_folio_public)) {
	enable_scroll_item = xv_create(NULL,
				       MENUITEM,
#ifdef OW_I18N
				       MENU_STRING,
						XV_I18N_MSG("xv_messages",
						       "Enable Scrolling"),
#else
				       MENU_STRING, ttysw_scrolling_enable,
#endif
				       MENU_ACTION, ttysw_enable_scrolling,
				       MENU_CLIENT_DATA, ttysw_folio_public,
				       HELP_INFO("ttysw:menscroll")
				       0);
	(void) xv_set(ttysw_menu,
		      MENU_APPEND_ITEM, enable_scroll_item, 0);

    }
    return (ttysw_menu);
}


Pkg_private void
ttysw_show_walkmenu(anysw_view_public, event)
    Tty_view        anysw_view_public;
    Event          *event;
{
    register Menu   menu;


    if (IS_TTY_VIEW(anysw_view_public)) {
	menu = (Menu) xv_get(TTY_FROM_TTY_VIEW(anysw_view_public), WIN_MENU);
    } else {
	Ttysw_folio     ttysw = TTY_PRIVATE_FROM_TERMSW_VIEW(anysw_view_public);
	Termsw_folio    termsw = TERMSW_FOLIO_FROM_TERMSW_VIEW(anysw_view_public);

	if (ttysw_getopt(ttysw, TTYOPT_TEXT)) {
	    ttysw->current_view_public = anysw_view_public;
	    menu = termsw->text_menu;
	    xv_set(menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
		   anysw_view_public, 0);
	} else if (ttysw->current_view_public == anysw_view_public)
	    menu = termsw->tty_menu;
	else {
	    menu = termsw->text_menu;
	    xv_set(menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
		   anysw_view_public, 0);
	}
    }

    if (!menu)
	return;

    /* insure that there are no caret render race conditions */
    termsw_menu_set();
    xv_set(menu, MENU_DONE_PROC, termsw_menu_clr, 0);

    menu_show(menu, anysw_view_public, event, 0);
}


/*
 * Menu item gen procs
 */
static          Menu_item
ttysw_menu_page_state(mi, op)
    Menu_item       mi;
    Menu_generate   op;
{
    Tty             ttysw_public;
    Ttysw_folio     ttysw;

    if (op == MENU_DISPLAY_DONE)
	return mi;

    ttysw_public = (Tty) menu_get(mi, MENU_CLIENT_DATA);
    ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_public);


    if (ttysw->ttysw_flags & TTYSW_FL_FROZEN)
	(void) menu_set(mi, MENU_STRING,
#ifdef OW_I18N
	XV_I18N_MSG("xv_messages", "Continue"),
#else
	"Continue",
#endif
			HELP_INFO("ttysw:mcont")
			0);
    else if (ttysw_getopt((caddr_t) ttysw, TTYOPT_PAGEMODE))
	(void) menu_set(mi, MENU_STRING,
#ifdef OW_I18N
	XV_I18N_MSG("xv_messages", "Disable Page Mode"),
#else
	"Disable Page Mode",
#endif
			HELP_INFO("ttysw:mdsbpage")
			0);
    else
	(void) menu_set(mi, MENU_STRING,
#ifdef OW_I18N
	XV_I18N_MSG("xv_messages", "Enable Page Mode "),
#else
	"Enable Page Mode ",
#endif
			HELP_INFO("ttysw:menbpage")
			0);
    return mi;
}



/*
 * Callout functions
 */





/* ARGSUSED */
static void
ttysw_menu_page(menu, mi)
    Menu            menu;
    Menu_item       mi;
{
    Tty             ttysw_public = (Tty) menu_get(mi, MENU_CLIENT_DATA);
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_public);


    if (ttysw->ttysw_flags & TTYSW_FL_FROZEN)
	(void) ttysw_freeze(ttysw->view, 0);
    else
	(void) ttysw_setopt(TTY_VIEW_HANDLE_FROM_TTY_FOLIO(ttysw), TTYOPT_PAGEMODE,
			    !ttysw_getopt((caddr_t) ttysw, TTYOPT_PAGEMODE));
}

/* ARGSUSED */


static void
ttysw_menu_copy(menu, mi)
    Menu            menu;
    Menu_item       mi;
{
    Tty             ttysw_public = (Tty) menu_get(mi, MENU_CLIENT_DATA);
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_public);


    if (!ttysw_do_copy(ttysw)) {
	Frame           frame = xv_get(ttysw_public, WIN_FRAME);
	notice_prompt(
		      frame,
		      (Event *) 0,
		      NOTICE_BUTTON_YES,
#ifdef OW_I18N
			 XV_I18N_MSG("xv_messages", "Continue"),
#else
			 "Continue",
#endif
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		      XV_I18N_MSG("xv_messages", "Please make a primary selection first."),
#else
		      "Please make a primary selection first.",
#endif
		      0,
		      0);

    }
}

/*ARGSUSED*/
static void
ttysw_menu_paste(menu, mi)
    Menu            menu;
    Menu_item       mi;
{
    Tty             ttysw_public = (Tty) menu_get(mi, MENU_CLIENT_DATA);
    Ttysw_folio     ttysw = TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw_public);

    if (!ttysw_do_paste(ttysw)) {
	Frame           frame = xv_get(ttysw_public, WIN_FRAME);
	notice_prompt(
		      frame,
		      (Event *) 0,
		      NOTICE_BUTTON_YES, "Continue",
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		      XV_I18N_MSG("xv_messages", "Please Copy text onto clipboard first."),
#else
		      "Please Copy text onto clipboard first.",
#endif
		      0,
		      0);

    }
}

/* termsw walking menu definitions */

ttysw_set_menu(termsw_public)
    Termsw          termsw_public;
{
    Menu            history_menu, scroll_menu, editor_menu, mode_menu;
    Menu_item       edit_item, find_item, extras_item, history_item, mode_item,
                    scroll_item, editor_item;
    Menu_item       editiable_item, readonly_item, enable_editor_item, disable_editor_item,
                    enable_scroll_item, disable_scroll_item, store_item, clear_item;
    Xv_Server	    server;
    Termsw_folio    termsw_folio = TERMSW_PRIVATE(termsw_public);
    Textsw          textsw = termsw_public;
    int             ttysw_mode_action();
    int             ttysw_enable_editor();
    int             ttysw_disable_editor();


    server = XV_SERVER_FROM_WINDOW(termsw_public);
    termsw_folio->text_menu = xv_create(server, MENU,
					HELP_INFO("ttysw:mterms")
					0);

    /* History sub menu */
    history_menu = xv_create(server, MENU,
			     MENU_CLIENT_DATA, textsw,
			     HELP_INFO("ttysw:mhistory")
			     0);
    mode_menu = xv_create(server, MENU_CHOICE_MENU,
			  HELP_INFO("ttysw:mmode")
			  0);

    editiable_item = xv_create(NULL,
			       MENUITEM,
#ifdef OW_I18N
			       MENU_STRING,
					XV_I18N_MSG("xv_messages", "Editable"),
#else
			       MENU_STRING, "Editable",
#endif
			       MENU_VALUE, EDITABLE,
			       MENU_ACTION, ttysw_mode_action,
			       MENU_CLIENT_DATA, textsw,
			       HELP_INFO("ttysw:mmode")
			       0);
    readonly_item = xv_create(NULL,
			      MENUITEM,
#ifdef OW_I18N
			      MENU_STRING,
				       XV_I18N_MSG("xv_messages", "Read Only"),
#else
			      MENU_STRING, "Read Only",
#endif
			      MENU_VALUE, READ_ONLY,
			      MENU_ACTION, ttysw_mode_action,
			      MENU_CLIENT_DATA, textsw,
			      HELP_INFO("ttysw:mmode")
			      0);

    xv_set(mode_menu,
	   MENU_APPEND_ITEM, editiable_item,
	   MENU_APPEND_ITEM, readonly_item,
	   MENU_DEFAULT_ITEM, readonly_item,
	   0);

    mode_item = (Menu_item) xv_create(NULL,
				      MENUITEM,
#ifdef OW_I18N
				      MENU_STRING,
					XV_I18N_MSG("xv_messages", "Mode"),
#else
				      MENU_STRING, "Mode",
#endif
				      MENU_PULLRIGHT, mode_menu,
				      HELP_INFO("ttysw:mmode")
				      0);

    store_item = xv_create(NULL,
			   MENUITEM,
			   MENU_STRING,
#ifdef OW_I18N
			   XV_I18N_MSG("xv_messages", "Store log as new file "),
#else
			   "Store log as new file ",
#endif
			   MENU_ACTION, textsw_file_do_menu_action,
			   MENU_VALUE, TEXTSW_MENU_STORE,
			   MENU_CLIENT_DATA, textsw,
			   HELP_INFO("textsw:mstorelog")
			   0);
    clear_item = xv_create(NULL,
			   MENUITEM,
			   MENU_STRING,
#ifdef OW_I18N
			   XV_I18N_MSG("xv_messages", "Clear log"),
#else
			   "Clear log",
#endif
			   MENU_ACTION, textsw_file_do_menu_action,
			   MENU_VALUE, TEXTSW_MENU_RESET,
			   MENU_CLIENT_DATA, textsw,
			   HELP_INFO("textsw:mclearlog")
			   0);

    xv_set(history_menu,
	   MENU_APPEND_ITEM, mode_item,
	   MENU_APPEND_ITEM, store_item,
	   MENU_APPEND_ITEM, clear_item,
	   0);

    history_item = (Menu_item) xv_create(NULL,
					 MENUITEM,
					 MENU_STRING,
#ifdef OW_I18N
					 XV_I18N_MSG("xv_messages", "History"),
#else
					 "History",
#endif
					 MENU_PULLRIGHT, history_menu,
					 HELP_INFO("ttysw:mhistory")
					 0);

    edit_item = (Menu) xv_create(NULL,
				 MENUITEM,
				 MENU_STRING,
#ifdef OW_I18N
				 XV_I18N_MSG("xv_messages", "Edit"),
#else
				 "Edit",
#endif
		 MENU_PULLRIGHT, xv_get(termsw_public, TEXTSW_SUBMENU_EDIT),
				 HELP_INFO("ttysw:medit")
				 0);

    find_item = (Menu) xv_create(NULL,
				 MENUITEM,
				 MENU_STRING,
#ifdef OW_I18N
				 XV_I18N_MSG("xv_messages", "Find"),
#else
				 "Find",
#endif
		 MENU_PULLRIGHT, xv_get(termsw_public, TEXTSW_SUBMENU_FIND),
				 HELP_INFO("ttysw:mfind")
				 0);

    extras_item = (Menu) xv_create(NULL,
				   MENUITEM,
				   MENU_STRING,
#ifdef OW_I18N
				   XV_I18N_MSG("xv_messages", "Extras"),
#else
				   "Extras",
#endif
	      MENU_PULLRIGHT, xv_get(termsw_public, TEXTSW_EXTRAS_CMD_MENU),
				   HELP_INFO("ttysw:mcommands")
				   0);

    /* Editor sub menu */
    editor_menu = xv_create(server, MENU_CHOICE_MENU,
			    HELP_INFO("textsw:meditor")
			    0);

    enable_editor_item = xv_create(NULL,
				   MENUITEM,
				   MENU_STRING,
#ifdef OW_I18N
				   XV_I18N_MSG("xv_messages", "Enable"),
#else
				   "Enable",
#endif
				   MENU_ACTION, ttysw_enable_editor,
				   MENU_CLIENT_DATA, textsw,
				   HELP_INFO("textsw:meneditor")
				   0);
    disable_editor_item = xv_create(NULL,
				    MENUITEM,
				    MENU_STRING,
#ifdef OW_I18N
				    XV_I18N_MSG("xv_messages", "Disable"),
#else
				    "Disable",
#endif
				    MENU_CLIENT_DATA, textsw,
				    MENU_ACTION, ttysw_disable_editor,
				    HELP_INFO("textsw:mdiseditor")
				    0);

    xv_set(editor_menu,
	   MENU_APPEND_ITEM, enable_editor_item,
	   MENU_APPEND_ITEM, disable_editor_item,
	   MENU_DEFAULT_ITEM, enable_editor_item,
	   0);
    editor_item = (Menu) xv_create(NULL,
				   MENUITEM,
				   MENU_STRING,
#ifdef OW_I18N
				   XV_I18N_MSG("xv_messages", "File Editor"),
#else
				   "File Editor",
#endif
				   MENU_PULLRIGHT, editor_menu,
				   HELP_INFO("ttysw:meditor")
				   0);

    /* Scrolling sub menu */
    scroll_menu = xv_create(server, MENU_CHOICE_MENU,
			    HELP_INFO("textsw:mscroll")
			    0);

    enable_scroll_item = xv_create(NULL,
				   MENUITEM,
#ifdef OW_I18N
				   MENU_STRING,
					XV_I18N_MSG("xv_messages",
						    "Enable Scrolling"),
#else
				   MENU_STRING, ttysw_scrolling_enable,
#endif
				   MENU_VALUE, ENABLE_SCROLL,
				   MENU_CLIENT_DATA, textsw,
				   HELP_INFO("textsw:menscroll")
				   0);
    disable_scroll_item = xv_create(NULL,
				    MENUITEM,
#ifdef OW_I18N
				    MENU_STRING,
					XV_I18N_MSG("xv_messages",
						    "Disable Scrolling"),
#else
				    MENU_STRING, ttysw_scrolling_disable,
#endif
				    MENU_VALUE, DISABLE_SCROLL,
				    MENU_ACTION, ttysw_disable_scrolling,
				    MENU_CLIENT_DATA, textsw,
				    HELP_INFO("textsw:mdisscroll")
				    0);

    xv_set(scroll_menu,
	   MENU_APPEND_ITEM, enable_scroll_item,
	   MENU_APPEND_ITEM, disable_scroll_item,
	   HELP_INFO("textsw:mscroll")
	   0);
    scroll_item = (Menu) xv_create(NULL,
				   MENUITEM,
				   MENU_STRING,
#ifdef OW_I18N
				   XV_I18N_MSG("xv_messages", "Scrolling"),
#else
				   "Scrolling",
#endif
				   MENU_PULLRIGHT, scroll_menu,
				   HELP_INFO("ttysw:mscroll")
				   0);

    (void) xv_set(termsw_folio->text_menu,
		  MENU_TITLE_ITEM,
#ifdef OW_I18N
		  XV_I18N_MSG("xv_messages", "Term Pane"),
#else
		  "Term Pane",
#endif
		  MENU_APPEND_ITEM, history_item,
		  MENU_APPEND_ITEM, edit_item,
		  MENU_APPEND_ITEM, find_item,
		  MENU_APPEND_ITEM, extras_item,
		  MENU_APPEND_ITEM, editor_item,
		  MENU_APPEND_ITEM, scroll_item,
		  0);
}

/*ARGSUSED*/
static void
panel_button_proc(item, event)
    Panel_item     *item;
    Event          *event;
{
    Textsw          textsw = (Textsw) xv_get(item, XV_KEY_DATA, ITEM_DATA_KEY);
    Menu            menu = (Menu) xv_get(item, PANEL_ITEM_MENU);
    Panel           p_menu = (Panel) xv_get(menu, MENU_PIN_WINDOW);
    Menu_item       menu_item;
    Menu            pullr_menu;
    int             num_items, i;

    xv_set(menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
	   textsw, 0);
    if (p_menu) {
	num_items = (int) xv_get(menu, MENU_NITEMS);
	for (i = 1; i <= num_items; i++) {
	    menu_item = (Menu_item) xv_get(menu, MENU_NTH_ITEM, i);
	    if (menu_item) {
		pullr_menu = (Menu) xv_get(menu_item, MENU_PULLRIGHT);
		if (pullr_menu)
		    xv_set(pullr_menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
			   textsw, 0);
	    }
	}
    }
}

static void
create_textedit_panel_item(panel, textsw)
    Panel           panel;
    Textsw          textsw;
{
    Panel_item      file_panel_item, edit_panel_item, display_panel_item, find_panel_item;

    if (!ITEM_DATA_KEY)
		ITEM_DATA_KEY = xv_unique_key();

    file_panel_item = xv_create(panel, PANEL_BUTTON,
				PANEL_LABEL_STRING,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "File"),
#else
				"File",
#endif
				PANEL_NOTIFY_PROC, panel_button_proc,
		PANEL_ITEM_MENU, (Menu) xv_get(textsw, TEXTSW_SUBMENU_FILE),
				0);
    display_panel_item = xv_create(panel, PANEL_BUTTON,
				   PANEL_LABEL_STRING,
#ifdef OW_I18N
				   XV_I18N_MSG("xv_messages", "View"),
#else
				   "View",
#endif
				   PANEL_NOTIFY_PROC, panel_button_proc,
		PANEL_ITEM_MENU, (Menu) xv_get(textsw, TEXTSW_SUBMENU_VIEW),
				   0);

    edit_panel_item = xv_create(panel, PANEL_BUTTON,
				PANEL_LABEL_STRING,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Edit"),
#else
				"Edit",
#endif
				PANEL_NOTIFY_PROC, panel_button_proc,
		PANEL_ITEM_MENU, (Menu) xv_get(textsw, TEXTSW_SUBMENU_EDIT),
				0);


    find_panel_item = xv_create(panel, PANEL_BUTTON,
				PANEL_LABEL_STRING,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Find"),
#else
				"Find",
#endif
				PANEL_NOTIFY_PROC, panel_button_proc,
		PANEL_ITEM_MENU, (Menu) xv_get(textsw, TEXTSW_SUBMENU_FIND),
				0);

    xv_set(file_panel_item, XV_KEY_DATA, ITEM_DATA_KEY,
	   textsw, 0);
    xv_set(display_panel_item, XV_KEY_DATA, ITEM_DATA_KEY,
	   textsw, 0);
    xv_set(edit_panel_item, XV_KEY_DATA, ITEM_DATA_KEY,
	   textsw, 0);
    xv_set(find_panel_item, XV_KEY_DATA, ITEM_DATA_KEY,
	   textsw, 0);

    window_fit_height(panel);

}



void
fit_termsw_panel_and_textsw(frame, termsw_folio)
    Frame           frame;
    register Termsw_folio termsw_folio;

{
    Rect            rect, panel_rect, textsw_rect;
    int             termsw_height;

    rect = *((Rect *) xv_get(TERMSW_PUBLIC(termsw_folio), WIN_RECT));

    termsw_height = (rect.r_height / 3);
    xv_set(TERMSW_PUBLIC(termsw_folio), XV_HEIGHT, termsw_height, 0);

    panel_rect = *((Rect *) xv_get(termsw_folio->textedit_panel, WIN_RECT));
    panel_rect.r_left = rect.r_left;
    panel_rect.r_top = rect.r_top + termsw_height + 1;
    panel_rect.r_width = rect.r_width;

    xv_set(termsw_folio->textedit_panel, WIN_RECT, &panel_rect,
	   XV_SHOW, TRUE, 0);

    textsw_rect.r_left = panel_rect.r_left;
    textsw_rect.r_top = panel_rect.r_top + panel_rect.r_height + 1;
    textsw_rect.r_width = panel_rect.r_width;
    textsw_rect.r_height = rect.r_height - (panel_rect.r_top + panel_rect.r_height);

    xv_set(termsw_folio->textedit, WIN_RECT, &textsw_rect,
	   XV_SHOW, TRUE, 0);

    window_fit(frame);

}

/*ARGSUSED*/
int
ttysw_enable_editor(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    Textsw          textsw = (Textsw) (menu_get(cmd_item, MENU_CLIENT_DATA));
    Frame           frame = (Frame) xv_get(textsw, WIN_FRAME);
    register        Termsw_folio
                    termsw_folio = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    Xv_opaque       my_font = xv_get(textsw, WIN_FONT);


    if (termsw_folio->first_view->next) {
	(void) notice_prompt(
			     frame,
			     (Event *) 0,
			     NOTICE_NO_BEEPING, FALSE,
			     NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
	      XV_I18N_MSG("xv_messages", "\
Please destroy all split views before enabling File Editor.\n\
Press \"Continue\" to proceed."),
#else
	      "Please destroy all split views before enabling File Editor.",
			     "Press \"Continue\" to proceed.",
#endif
			     0,
			     NOTICE_BUTTON_YES,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			     NOTICE_TRIGGER, ACTION_STOP,
			     0);
	return;
    }
    if (!termsw_folio->textedit) {
	termsw_folio->textedit_panel = xv_create(frame, PANEL,
				     WIN_BELOW, TERMSW_PUBLIC(termsw_folio),
					     PANEL_LAYOUT, PANEL_HORIZONTAL,
						 XV_SHOW, FALSE,
				    XV_WIDTH, (int) xv_get(frame, XV_WIDTH),
						 0);

	termsw_folio->textedit = xv_create(frame, TEXTSW,
					   WIN_FONT, my_font,
				    WIN_BELOW, termsw_folio->textedit_panel,
					   XV_SHOW, FALSE,
					   0);

	(void) create_textedit_panel_item(termsw_folio->textedit_panel, termsw_folio->textedit);

    }
    if ((int) xv_get(termsw_folio->textedit, XV_SHOW)) {
	(void) notice_prompt(
			     frame,
			     (Event *) 0,
			     NOTICE_NO_BEEPING, FALSE,
			     NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			     XV_I18N_MSG("xv_messages", "\
Textedit is already created.\n\
Press \"Continue\" to proceed."),
#else
			     "Textedit is already created.",
			     "Press \"Continue\" to proceed.",
#endif
			     0,
			     NOTICE_BUTTON_YES,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			     NOTICE_TRIGGER, ACTION_STOP,
			     0);
	return;
    }
    (void) fit_termsw_panel_and_textsw(frame, termsw_folio);

    /* Change default to "Disable editor" */
    xv_set(cmd_item, MENU_SELECTED, FALSE, 0);
    xv_set(cmd_menu, MENU_DEFAULT, 2, 0);
}


/* ARGSUSED */
ttysw_disable_editor(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    Textsw          textsw = (Textsw) (menu_get(cmd_item, MENU_CLIENT_DATA));
    Frame           frame = (Frame) xv_get(textsw, WIN_FRAME);
    register        Termsw_folio
                    termsw_folio = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    Event           ie;
    extern int      win_getmouseposition();
    extern int      textsw_empty_document();
    Rect            rect;

    if ((!termsw_folio->textedit) ||
	(!(int) xv_get(termsw_folio->textedit, XV_SHOW))) {
	(void) notice_prompt(
			     frame,
			     (Event *) 0,
			     NOTICE_NO_BEEPING, FALSE,
			     NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
			     XV_I18N_MSG("xv_messages", "\
No textedit is enabled yet.\n\
Press \"Continue\" to proceed."),
#else
			     "No textedit is enabled yet.",
			     "Press \"Continue\" to proceed.",
#endif
			     0,
			     NOTICE_BUTTON_YES,
#ifdef OW_I18N
				XV_I18N_MSG("xv_messages", "Continue"),
#else
				"Continue",
#endif
			     NOTICE_TRIGGER, ACTION_STOP,
			     0);
	return;
    }
    (void) win_getmouseposition(termsw_folio->textedit, &ie.ie_locx, &ie.ie_locy);
    if (textsw_empty_document(termsw_folio->textedit, &ie) == XV_ERROR)
	return;

    /* Change default to "Enable editor" */
    xv_set(cmd_item, MENU_SELECTED, FALSE, 0);
    xv_set(cmd_menu, MENU_DEFAULT, 1, 0);

    rect = *((Rect *) xv_get(termsw_folio->textedit, WIN_RECT));


    xv_set(termsw_folio->textedit, XV_SHOW, FALSE, 0);
    xv_set(termsw_folio->textedit_panel, XV_SHOW, FALSE, 0);

    xv_set(TERMSW_PUBLIC(termsw_folio),
	   XV_HEIGHT, rect.r_top + rect.r_height - 1,
	   XV_WIDTH, rect.r_width, 0);
    window_fit(frame);


}

/* ARGSUSED */
ttysw_mode_action(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    Textsw          textsw = (Textsw) (menu_get(cmd_item, MENU_CLIENT_DATA));
    register        Termsw_folio
                    termsw = TERMSW_FOLIO_FOR_VIEW(TERMSW_VIEW_PRIVATE_FROM_TEXTSW(textsw));
    int             value = (int) menu_get(cmd_item, MENU_VALUE, 0);

    Textsw_index    tmp_index, insert;

    if ((value == READ_ONLY) && !termsw->append_only_log) {
	tmp_index = (int) textsw_find_mark(textsw, termsw->pty_mark);
	insert = (Textsw_index) xv_get(textsw, TEXTSW_INSERTION_POINT);
	if (insert != tmp_index) {
	    (void) xv_set(textsw, TEXTSW_INSERTION_POINT, tmp_index, 0);
	}
	termsw->read_only_mark =
	    textsw_add_mark(textsw,
		      termsw->cooked_echo ? tmp_index : TEXTSW_INFINITY - 1,
			    TEXTSW_MARK_READ_ONLY);
	termsw->append_only_log = TRUE;
    } else if ((value == EDITABLE) && termsw->append_only_log) {
	textsw_remove_mark(textsw, termsw->read_only_mark);
	termsw->append_only_log = FALSE;
    }
}

/*ARGSUSED*/
static void
ttysw_enable_scrolling(menu, mi)
    Menu            menu;
    Menu_item       mi;
/*
 * This routine should only be invoked from the item added to the ttysw menu
 * when sw is created as a termsw.  It relies on the menu argument being the
 * handle for the ttysw menu.
 */
{
    /* The textsw handle is really a termsw handle */
    Textsw          textsw = (Textsw) (menu_get(mi, MENU_CLIENT_DATA));
    Ttysw_view_handle ttysw_view_handle = TTY_VIEW_PRIVATE_FROM_ANY_PUBLIC(textsw);
    Termsw_folio    termsw_folio = TERMSW_PRIVATE((Termsw) textsw);

#ifdef OW_I18N
    Ttysw_folio     ttysw_folio = TTY_PRIVATE_FROM_ANY_PUBLIC(textsw);
    	    
    if (ttysw_folio->ic) {
#ifdef POSTPUBLICREVIEW_BC    
        int	    *conv_on = NULL;
	if (!XGetICValues(ttysw_folio->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on && *conv_on == XIMEnable) {
	        Frame           frame = xv_get(textsw, WIN_FRAME);
	        XFree(conv_on);
	        notice_prompt(frame, (Event *) 0,
		      NOTICE_BUTTON_YES,
		      XV_I18N_MSG("xv_messages", "Continue"),
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
		      XV_I18N_MSG("xv_messages", "Cannot enable scrolling during conversion on."),
		      0,
		      0);
		return;		      
            }	              
	}
	if (conv_on)
	    XFree(conv_on);
#else /* POSTPUBLICREVIEW_BC */
        int	    conv_on;
	if (!XGetICValues(ttysw_folio->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on == XIMEnable) {
	        Frame           frame = xv_get(textsw, WIN_FRAME);
	        notice_prompt(frame, (Event *) 0,
		      NOTICE_BUTTON_YES,
		      XV_I18N_MSG("xv_messages", "Continue"),
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
		      XV_I18N_MSG("xv_messages", "Cannot enable scrolling during conversion on."),
		      0,
		      0);
		return;		      
            }	              
	}
#endif  /* POSTPUBLICREVIEW_BC */	    
    }    
#endif
    

    if (termsw_folio->ok_to_enable_scroll) {
	ttysw_setopt(ttysw_view_handle, TTYOPT_TEXT, 1);
    } else {
	Frame           frame = xv_get(textsw, WIN_FRAME);
	notice_prompt(
		      frame,
		      (Event *) 0,
		      NOTICE_BUTTON_YES,
#ifdef OW_I18N
			 XV_I18N_MSG("xv_messages", "Continue"),
#else
			 "Continue",
#endif
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		      XV_I18N_MSG("xv_messages", "Exiting vi will enable scrolling."),
#else
		      "Exiting vi will enable scrolling.",
#endif
		      0,
		      0);

    }
}

/* ARGSUSED */
static void
ttysw_disable_scrolling(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    /* The textsw handle is really a termsw handle */
    Textsw          textsw = (Textsw) (menu_get(cmd_item, MENU_CLIENT_DATA));
    Ttysw_view_handle ttysw_view = TTY_VIEW_PRIVATE_FROM_ANY_PUBLIC(textsw);
    Ttysw_folio     ttysw_folio = TTY_FOLIO_FROM_TTY_VIEW_HANDLE(ttysw_view);
    
#ifdef OW_I18N
    Termsw_folio     termsw_folio = TERMSW_PRIVATE_FROM_TTY_PRIVATE(ttysw_folio);
    	    
    if (termsw_folio->ic) {
#ifdef POSTPUBLICREVIEW_BC    
        int	    *conv_on = NULL;
	if (!XGetICValues(termsw_folio->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on && *conv_on == XIMEnable) { 
	        Frame           frame = xv_get(textsw, WIN_FRAME);
	        XFree(conv_on);
	        notice_prompt(frame, (Event *) 0,
		      NOTICE_BUTTON_YES,
		      XV_I18N_MSG("xv_messages", "Continue"),
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
		      XV_I18N_MSG("xv_messages", "Cannot disable scrolling during conversion on."),
		      0,
		      0);
		return;	 
            }	              
	}
	if (conv_on)
	    XFree(conv_on);
#else /* POSTPUBLICREVIEW_BC */
	int	conv_on;
	if (!XGetICValues(termsw_folio->ic, XNExtXimp_Conversion, &conv_on, NULL)) {
            if (conv_on == XIMEnable) { 
	        Frame           frame = xv_get(textsw, WIN_FRAME);
	        notice_prompt(frame, (Event *) 0,
		      NOTICE_BUTTON_YES,
		      XV_I18N_MSG("xv_messages", "Continue"),
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
		      XV_I18N_MSG("xv_messages", "Cannot disable scrolling during conversion on."),
		      0,
		      0);
		return;	 
            }	              
	}

#endif /* POSTPUBLICREVIEW_BC */	    
    }	    
#endif

    if (ttysw_getopt(ttysw_folio, TTYOPT_TEXT)) {
	ttysw_setopt(ttysw_view, TTYOPT_TEXT, 0);
    } else {
	Frame           frame = xv_get(textsw, WIN_FRAME);
	notice_prompt(frame,
		      (Event *) 0,
		      NOTICE_BUTTON_YES,
#ifdef OW_I18N
			 XV_I18N_MSG("xv_messages", "Continue"),
#else
			 "Continue",
#endif
		      NOTICE_TRIGGER, ACTION_STOP,	/* yes or no */
		      NOTICE_MESSAGE_STRINGS,
#ifdef OW_I18N
		    XV_I18N_MSG("xv_messages", "Only one termsw view can turn into a ttysw at a time."),
#else
		    "Only one termsw view can turn into a ttysw at a time.",
#endif
		      0,
		      0);

    }
    xv_set(cmd_menu, MENU_DEFAULT, 1, 0);
}
