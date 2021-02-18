/* -*-C-*-
********************************************************************************
*
* File:         w_funtab.c
* RCS:          $Header: w_funtab.c,v 1.12 91/03/25 04:20:31 mayer Exp $
* Description:  Function table for winterp (replaces xlisp/xlftab.c)
* Author:       Niels Mayer, HPLabs
* Created:      Tue Jul 18 00:12:30 1989
* Modified:     Thu Oct  3 20:46:02 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_funtab.c,v 1.12 91/03/25 04:20:31 mayer Exp $";

/* xlftab.c - xlisp function table */
/*	Copyright (c) 1985, by David Michael Betz */

#include <stdio.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"
#include "app_funextn.h"	/* place application function extern declarations in app_funextn.h */

/* SUBR/FSUBR indicator */
#define S	SUBR
#define F	FSUBR

/* forward declarations */
LVAL xnotimp();
extern LVAL rmhash();
extern LVAL rmquote();
extern LVAL rmdquote();
extern LVAL rmbquote();
extern LVAL rmcomma();
extern LVAL rmlpar();
extern LVAL rmrpar();
extern LVAL rmsemi();
/* extern LVAL xnotimp(); */
/* extern LVAL xnotimp(); */
extern LVAL clnew();
extern LVAL clisnew();
extern LVAL clanswer();
extern LVAL obisnew();
extern LVAL obclass();
extern LVAL obshow();
/* extern LVAL xnotimp(); */
/* extern LVAL xnotimp(); */
/* extern LVAL xnotimp(); */
/* extern LVAL xnotimp(); */
extern LVAL xeval();
extern LVAL xapply();
extern LVAL xfuncall();
extern LVAL xquote();
extern LVAL xfunction();
extern LVAL xbquote();
extern LVAL xlambda();
extern LVAL xset();
extern LVAL xsetq();
extern LVAL xsetf();
extern LVAL xdefun();
extern LVAL xdefmacro();
extern LVAL xgensym();
extern LVAL xmakesymbol();
extern LVAL xintern();
extern LVAL xsymname();
extern LVAL xsymvalue();
extern LVAL xsymplist();
extern LVAL xget();
extern LVAL xputprop();
extern LVAL xremprop();
extern LVAL xhash();
extern LVAL xmkarray();
extern LVAL xaref();
extern LVAL xcar();
extern LVAL xcdr();
extern LVAL xcaar();
extern LVAL xcadr();
extern LVAL xcdar();
extern LVAL xcddr();
extern LVAL xcaaar();
extern LVAL xcaadr();
extern LVAL xcadar();
extern LVAL xcaddr();
extern LVAL xcdaar();
extern LVAL xcdadr();
extern LVAL xcddar();
extern LVAL xcdddr();
extern LVAL xcaaaar();
extern LVAL xcaaadr();
extern LVAL xcaadar();
extern LVAL xcaaddr();
extern LVAL xcadaar();
extern LVAL xcadadr();
extern LVAL xcaddar();
extern LVAL xcadddr();
extern LVAL xcdaaar();
extern LVAL xcdaadr();
extern LVAL xcdadar();
extern LVAL xcdaddr();
extern LVAL xcddaar();
extern LVAL xcddadr();
extern LVAL xcdddar();
extern LVAL xcddddr();
extern LVAL xcons();
extern LVAL xlist();
extern LVAL xappend();
extern LVAL xreverse();
extern LVAL xlast();
extern LVAL xnth();
extern LVAL xnthcdr();
extern LVAL xmember();
extern LVAL xassoc();
extern LVAL xsubst();
extern LVAL xsublis();
extern LVAL xremove();
extern LVAL xlength();
extern LVAL xmapc();
extern LVAL xmapcar();
extern LVAL xmapl();
extern LVAL xmaplist();
extern LVAL xrplca();
extern LVAL xrplcd();
extern LVAL xnconc();
extern LVAL xdelete();
extern LVAL xatom();
extern LVAL xsymbolp();
extern LVAL xnumberp();
extern LVAL xboundp();
extern LVAL xnull();
extern LVAL xlistp();
extern LVAL xconsp();
extern LVAL xminusp();
extern LVAL xzerop();
extern LVAL xplusp();
extern LVAL xevenp();
extern LVAL xoddp();
extern LVAL xeq();
extern LVAL xeql();
extern LVAL xequal();
extern LVAL xcond();
extern LVAL xcase();
extern LVAL xand();
extern LVAL xor();
extern LVAL xlet();
extern LVAL xletstar();
extern LVAL xif();
extern LVAL xprog();
extern LVAL xprogstar();
extern LVAL xprog1();
extern LVAL xprog2();
extern LVAL xprogn();
extern LVAL xgo();
extern LVAL xreturn();
extern LVAL xdo();
extern LVAL xdostar();
extern LVAL xdolist();
extern LVAL xdotimes();
extern LVAL xcatch();
extern LVAL xthrow();
extern LVAL xerror();
extern LVAL xcerror();
extern LVAL xbreak();
extern LVAL xcleanup();
extern LVAL xtoplevel();
extern LVAL xcontinue();
extern LVAL xerrset();
extern LVAL xbaktrace();
extern LVAL xevalhook();
extern LVAL xfix();
extern LVAL xfloat();
extern LVAL xadd();
extern LVAL xsub();
extern LVAL xmul();
extern LVAL xdiv();
extern LVAL xadd1();
extern LVAL xsub1();
extern LVAL xrem();
extern LVAL xmin();
extern LVAL xmax();
extern LVAL xabs();
extern LVAL xsin();
extern LVAL xcos();
extern LVAL xtan();
extern LVAL xexpt();
extern LVAL xexp();
extern LVAL xsqrt();
extern LVAL xrand();
extern LVAL xlogand();
extern LVAL xlogior();
extern LVAL xlogxor();
extern LVAL xlognot();
extern LVAL xlss();
extern LVAL xleq();
extern LVAL xequ();
extern LVAL xneq();
extern LVAL xgeq();
extern LVAL xgtr();
extern LVAL xstrcat();
extern LVAL xsubseq();
extern LVAL xstring();
extern LVAL xchar();
extern LVAL xread();
extern LVAL xprint();
extern LVAL xprin1();
extern LVAL xprinc();
extern LVAL xterpri();
extern LVAL xflatsize();
extern LVAL xflatc();
extern LVAL xopen();
extern LVAL xformat();
extern LVAL xclose();
extern LVAL xrdchar();
extern LVAL xpkchar();
extern LVAL xwrchar();
extern LVAL xreadline();
#ifdef WINTERP
extern LVAL Wut_Prim_LOAD();	/* w_utils.c: WINTERP uses it's own version of xload() */
#else
extern LVAL xload();
#endif				/* WINTERP */
extern LVAL xtranscript();
extern LVAL xgc();
extern LVAL xexpand();
extern LVAL xalloc();
extern LVAL xmem();
#ifdef SAVERESTORE
extern LVAL xsave();
extern LVAL xrestore();
#else
/* extern LVAL xnotimp(); */
/* extern LVAL xnotimp(); */
#endif
extern LVAL xtype();
extern LVAL xexit();
extern LVAL xpeek();
extern LVAL xpoke();
extern LVAL xaddrs();
extern LVAL xvector();
extern LVAL xblock();
extern LVAL xrtnfrom();
extern LVAL xtagbody();
extern LVAL xpsetq();
extern LVAL xflet();
extern LVAL xlabels();
extern LVAL xmacrolet();
extern LVAL xunwindprotect();
extern LVAL xpp();
extern LVAL xstrlss();
extern LVAL xstrleq();
extern LVAL xstreql();
extern LVAL xstrneq();
extern LVAL xstrgeq();
extern LVAL xstrgtr();
extern LVAL xstrilss();
extern LVAL xstrileq();
extern LVAL xstrieql();
extern LVAL xstrineq();
extern LVAL xstrigeq();
extern LVAL xstrigtr();
extern LVAL xintegerp();
extern LVAL xfloatp();
extern LVAL xstringp();
extern LVAL xarrayp();
extern LVAL xstreamp();
extern LVAL xobjectp();
extern LVAL xupcase();
extern LVAL xdowncase();
extern LVAL xnupcase();
extern LVAL xndowncase();
extern LVAL xtrim();
extern LVAL xlefttrim();
extern LVAL xrighttrim();
extern LVAL xwhen();
extern LVAL xunless();
extern LVAL xloop();
extern LVAL xsymfunction();
extern LVAL xfboundp();
extern LVAL xsend();
extern LVAL xsendsuper();
extern LVAL xprogv();
extern LVAL xcharp();
extern LVAL xcharint();
extern LVAL xintchar();
extern LVAL xrdbyte();
extern LVAL xwrbyte();
extern LVAL xmkstrinput();
extern LVAL xmkstroutput();
extern LVAL xgetstroutput();
extern LVAL xgetlstoutput();
extern LVAL xgcd();
extern LVAL xgetlambda();
extern LVAL xmacroexpand();
extern LVAL x1macroexpand();
extern LVAL xchrlss();
extern LVAL xchrleq();
extern LVAL xchreql();
extern LVAL xchrneq();
extern LVAL xchrgeq();
extern LVAL xchrgtr();
extern LVAL xchrilss();
extern LVAL xchrileq();
extern LVAL xchrieql();
extern LVAL xchrineq();
extern LVAL xchrigeq();
extern LVAL xchrigtr();
extern LVAL xuppercasep();
extern LVAL xlowercasep();
extern LVAL xbothcasep();
extern LVAL xdigitp();
extern LVAL xalphanumericp();
extern LVAL xchupcase();
extern LVAL xchdowncase();
extern LVAL xdigitchar();
extern LVAL xcharcode();
extern LVAL xcodechar();
extern LVAL xendp();
extern LVAL xremif();
extern LVAL xremifnot();
extern LVAL xdelif();
extern LVAL xdelifnot();
extern LVAL xtrace();
extern LVAL xuntrace();
extern LVAL xsort();
extern LVAL Prim_SYSTEM();
extern LVAL Prim_POPEN();
extern LVAL Prim_PCLOSE();
extern LVAL Widget_Class_Method_ISNEW();
extern LVAL Shell_Widget_Class_Method_ISNEW();
extern LVAL Popup_Shell_Widget_Class_Method_ISNEW();
extern LVAL Shell_Widget_Class_Method_REALIZE();
extern LVAL Shell_Widget_Class_Method_UNREALIZE();
extern LVAL Popup_Shell_Widget_Class_Method_POPUP();
extern LVAL Popup_Shell_Widget_Class_Method_POPDOWN();
extern LVAL Widget_Class_Method_MANAGE();
extern LVAL Widget_Class_Method_UNMANAGE();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_ISNEW();
extern LVAL Xm_Form_Widget_Class_Method_ISNEW();
extern LVAL Xm_List_Widget_Class_Method_ISNEW();
extern LVAL Xm_Text_Widget_Class_Method_ISNEW();
extern LVAL Xm_Message_Box_Widget_Class_Method_ISNEW();
extern LVAL Xm_Row_Column_Widget_Class_Method_ISNEW();
extern LVAL Xm_Selection_Box_Widget_Class_Method_ISNEW();
extern LVAL Widget_Class_Method_SET_VALUES();
extern LVAL Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Text_Widget_Class_Method_GET_STRING();
extern LVAL Wto_Prim_XtAddTimeOut();
extern LVAL Wto_Prim_XtRemoveTimeout();
extern LVAL Wcb_Prim_XtRemoveCallback();
extern LVAL Widget_Class_Method_SET_CALLBACK();
extern LVAL Widget_Class_Method_REMOVE_ALL_CALLBACKS();
extern LVAL Wut_Prim_XAllocColor();
extern LVAL Wpm_Prim_XmGetPixmap();
extern LVAL Wpm_Prim_XmInstallImage();
extern LVAL Wpm_Prim_XmUninstallImage();
extern LVAL Wpm_Prim_XmGetImageFromFile();
extern LVAL Widget_Class_Method_UPDATE_DISPLAY();
extern LVAL Wut_UserClick_To_WidgetObj();
extern LVAL Widget_Class_Method_DESTROY();
extern LVAL Widget_Class_Method_PARENT();
extern LVAL Xm_Text_Widget_Class_Method_GET_LAST_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_SET_STRING();
extern LVAL Xm_Text_Widget_Class_Method_REPLACE();
extern LVAL Xm_Text_Widget_Class_Method_GET_EDITABLE();
extern LVAL Xm_Text_Widget_Class_Method_SET_EDITABLE();
extern LVAL Xm_Text_Widget_Class_Method_GET_MAX_LENGTH();
extern LVAL Xm_Text_Widget_Class_Method_SET_MAX_LENGTH();
extern LVAL Xm_Text_Widget_Class_Method_GET_SELECTION();
extern LVAL Xm_Text_Widget_Class_Method_SET_SELECTION();
extern LVAL Xm_Text_Widget_Class_Method_CLEAR_SELECTION();
extern LVAL Xm_Text_Widget_Class_Method_GET_TOP_CHARACTER();
extern LVAL Xm_Text_Widget_Class_Method_SET_TOP_CHARACTER();
extern LVAL Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_XY_TO_POS();
extern LVAL Xm_Text_Widget_Class_Method_POS_TO_XY();
extern LVAL Xm_Text_Widget_Class_Method_SHOW_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_SCROLL();
extern LVAL Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY();
extern LVAL Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY();
extern LVAL Xm_List_Widget_Class_Method_ADD_ITEM();
extern LVAL Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED();
extern LVAL Xm_List_Widget_Class_Method_DELETE_ITEM();
extern LVAL Xm_List_Widget_Class_Method_DELETE_POS();
extern LVAL Xm_List_Widget_Class_Method_SELECT_ITEM();
extern LVAL Xm_List_Widget_Class_Method_SELECT_POS();
extern LVAL Xm_List_Widget_Class_Method_DESELECT_ITEM();
extern LVAL Xm_List_Widget_Class_Method_DESELECT_POS();
extern LVAL Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_SET_POS();
extern LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_POS();
extern LVAL Xm_List_Widget_Class_Method_SET_ITEM();
extern LVAL Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM();
extern LVAL Xm_List_Widget_Class_Method_ITEM_EXISTS();
extern LVAL Xm_List_Widget_Class_Method_SET_HORIZ_POS();
extern LVAL Xm_List_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_List_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Drawing_Area_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Drawing_Area_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Drawn_Button_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Drawn_Button_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Row_Column_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Toggle_Button_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Toggle_Button_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Command_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Command_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Scale_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Scale_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Text_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Text_Widget_Class_Method_SET_CALLBACK();
extern LVAL Widget_Class_Method_GET_VALUES();
extern LVAL Xm_Cascade_Button_Widget_Class_Method_HIGHLIGHT();
extern LVAL Xm_Command_Widget_Class_Method_GET_CHILD();
extern LVAL Xm_Command_Widget_Class_Method_SET_VALUE();
extern LVAL Xm_Command_Widget_Class_Method_APPEND_VALUE();
extern LVAL Xm_Command_Widget_Class_Method_ERROR();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH();
extern LVAL Xm_Main_Window_Widget_Class_SET_AREAS();
extern LVAL Xm_Main_Window_Widget_Class_SEP1();
extern LVAL Xm_Main_Window_Widget_Class_SEP2();
extern LVAL Xm_Message_Box_Widget_Class_Method_GET_CHILD();
extern LVAL Xm_Row_Column_Widget_Class_Method_MENU_POSITION();
extern LVAL Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET();
extern LVAL Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET();
extern LVAL xdefstruct();
extern LVAL xstrtypep();
extern LVAL xmkstruct();
extern LVAL xcpystruct();
extern LVAL xstrref();
extern LVAL xstrset();
extern LVAL xasin();
extern LVAL xacos();
extern LVAL xatan();
extern LVAL Prim_FSCANF_FIXNUM();
extern LVAL Prim_FSCANF_STRING();
extern LVAL Prim_FSCANF_FLONUM();
extern LVAL Prim_COPY_ARRAY();
extern LVAL Prim_ARRAY_INSERT_POS();
extern LVAL Prim_ARRAY_DELETE_POS();
extern LVAL Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING();
extern LVAL Xm_Scale_Widget_Class_Method_SET_VALUE();
extern LVAL Xm_Scale_Widget_Class_Method_GET_VALUE();
extern LVAL Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE();
extern LVAL Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE();
extern LVAL Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS();
extern LVAL Xm_Selection_Box_Widget_Class_Method_GET_CHILD();
extern LVAL Xm_Toggle_Button_Widget_Class_Method_GET_STATE();
extern LVAL Xm_Toggle_Button_Widget_Class_Method_SET_STATE();
extern LVAL Xm_Toggle_Button_Gadget_Class_Method_GET_STATE();
extern LVAL Xm_Toggle_Button_Gadget_Class_Method_SET_STATE();
extern LVAL Wtx_Prim_XT_PARSE_TRANSLATION_TABLE();
extern LVAL Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE();
extern LVAL Widget_Class_Method_OVERRIDE_TRANSLATIONS();
extern LVAL Widget_Class_Method_AUGMENT_TRANSLATIONS();
extern LVAL Widget_Class_Method_UNINSTALL_TRANSLATIONS();
extern LVAL Widget_Class_Method_INSTALL_ACCELERATORS();
extern LVAL Widget_Class_Method_INSTALL_ALL_ACCELERATORS();
extern LVAL Widget_Class_Method_ADD_EVENT_HANDLER();
extern LVAL Widget_Class_Method_SET_EVENT_HANDLER();
extern LVAL Widget_Class_Method_BUILD_EVENT_MASK();
extern LVAL Weh_Prim_REMOVE_EVENT_HANDLER();
extern LVAL Wxt_Prim_XT_MANAGE_CHILDREN();
extern LVAL Wxt_Prim_XT_UNMANAGE_CHILDREN();
extern LVAL Widget_Class_Method_ADD_TAB_GROUP();
extern LVAL Widget_Class_Method_REMOVE_TAB_GROUP();
extern LVAL Widget_Class_Method_ADD_GRAB();
extern LVAL Widget_Class_Method_REMOVE_GRAB();
extern LVAL Widget_Class_Method_IS_COMPOSITE();
extern LVAL Widget_Class_Method_IS_CONSTRAINT();
extern LVAL Widget_Class_Method_IS_SHELL();
extern LVAL Widget_Class_Method_IS_PRIMITIVE();
extern LVAL Widget_Class_Method_IS_GADGET();
extern LVAL Widget_Class_Method_IS_MANAGER();
extern LVAL Widget_Class_Method_SET_SENSITIVE();
extern LVAL Widget_Class_Method_SET_MAPPED_WHEN_MANAGED();
extern LVAL Widget_Class_Method_IS_MANAGED();
extern LVAL Widget_Class_Method_IS_REALIZED();
extern LVAL Widget_Class_Method_IS_SENSITIVE();
extern LVAL Widget_Class_Method_WINDOW();
extern LVAL Widget_Class_Method_MAP();
extern LVAL Widget_Class_Method_UNMAP();
extern LVAL Xm_Bulletin_Board_Widget_Class_Method_ISNEW();
extern LVAL Wxms_Prim_XM_STRING_CREATE();
extern LVAL Wxms_Prim_XM_STRING_DIRECTION_CREATE();
extern LVAL Wxms_Prim_XM_STRING_SEPARATOR_CREATE();
extern LVAL Wxms_Prim_XM_STRING_SEGMENT_CREATE();
extern LVAL Wxms_Prim_XM_STRING_CREATE_L_TO_R();
extern LVAL Wxms_Prim_XM_STRING_GET_L_TO_R();
extern LVAL Wxms_Prim_XM_STRING_CONCAT();
extern LVAL Wxms_Prim_XM_STRING_COPY();
extern LVAL Wxms_Prim_XM_STRING_BYTE_COMPARE();
extern LVAL Wxms_Prim_XM_STRING_COMPARE();
extern LVAL Wxms_Prim_XM_STRING_LENGTH();
extern LVAL Wxms_Prim_XM_STRING_EMPTY();
extern LVAL Wxms_Prim_XM_STRING_LINE_COUNT();
extern LVAL Wcls_Prim_WIDGETOBJP();
extern LVAL Wut_Prim_X_STORE_COLOR();
extern LVAL Wut_Prim_X_ALLOC_N_COLOR_CELLS_NO_PLANES();
extern LVAL Xm_Command_Widget_Class_Method_ISNEW();
extern LVAL Application_Shell_Widget_Class_Method_GET_ARGV();
extern LVAL Application_Shell_Widget_Class_Method_SET_ARGV();
extern LVAL Xm_Command_Widget_Class_Method_GET_HISTORY_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_GET_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_GET_SELECTED_ITEMS();
extern LVAL Xm_Selection_Box_Widget_Class_Method_GET_LIST_ITEMS();
extern LVAL Widget_Class_Method_HAS_CALLBACKS();
extern LVAL Widget_Class_Method_EXISTS_P();
extern LVAL Application_Shell_Widget_Class_Method_ISNEW();
extern LVAL Wxm_Prim_XM_SET_MENU_CURSOR();
extern LVAL Wut_Prim_GET_MOUSE_LOCATION(); /* uunet!cimshop!rhess */
extern LVAL Wto_Prim_TIMEOUT_ACTIVE_P();

#ifdef WINTERP_MOTIF_11
extern LVAL Wxm_Prim_XM_GET_COLORS();
extern LVAL Xm_Arrow_Button_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Arrow_Button_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Push_Button_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Push_Button_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Cascade_Button_Gadget_Class_Method_HIGHLIGHT();
extern LVAL Xm_List_Widget_Class_Method_ADD_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_DELETE_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_DELETE_ITEMS_POS();
extern LVAL Xm_List_Widget_Class_Method_DELETE_ALL_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_REPLACE_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_REPLACE_ITEMS_POS();
extern LVAL Xm_List_Widget_Class_Method_SET_ADD_MODE();
extern LVAL Xm_List_Widget_Class_Method_ITEM_POS();
extern LVAL Xm_List_Widget_Class_Method_GET_MATCH_POS();
extern LVAL Xm_List_Widget_Class_Method_GET_SELECTED_POS();
extern LVAL Xm_Main_Window_Widget_Class_SEP3();
extern LVAL Xm_Row_Column_Widget_Class_Method_GET_POSTED_FROM_WIDGET();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_STRING();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_LAST_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_STRING();
extern LVAL Xm_Text_Field_Widget_Class_Method_REPLACE();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_EDITABLE();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_EDITABLE();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_MAX_LENGTH();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_MAX_LENGTH();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_SELECTION();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_SELECTION();
extern LVAL Xm_Text_Field_Widget_Class_Method_CLEAR_SELECTION();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_INSERTION_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_GET_CURSOR_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_CURSOR_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_INSERTION_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_SET_CURSOR_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_CURSOR_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_SELECTION_POSITION();
extern LVAL Xm_Text_Field_Widget_Class_Method_XY_TO_POS();
extern LVAL Xm_Text_Field_Widget_Class_Method_POS_TO_XY();
extern LVAL Xm_Text_Field_Widget_Class_Method_SHOW_POSITION();
extern LVAL Xm_Text_Widget_Class_Method_SET_HIGHLIGHT();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_HIGHLIGHT();
extern LVAL Xm_Text_Widget_Class_Method_INSERT();
extern LVAL Xm_Text_Field_Widget_Class_Method_INSERT();
extern LVAL Xm_Text_Widget_Class_Method_SET_ADD_MODE();
extern LVAL Xm_Text_Field_Widget_Class_Method_SET_ADD_MODE();
extern LVAL Xm_Text_Widget_Class_Method_GET_ADD_MODE();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_ADD_MODE();
extern LVAL Xm_Text_Widget_Class_Method_REMOVE();
extern LVAL Xm_Text_Field_Widget_Class_Method_REMOVE();
extern LVAL Xm_Text_Widget_Class_Method_COPY();
extern LVAL Xm_Text_Field_Widget_Class_Method_COPY();
extern LVAL Xm_Text_Widget_Class_Method_CUT();
extern LVAL Xm_Text_Field_Widget_Class_Method_CUT();
extern LVAL Xm_Text_Widget_Class_Method_PASTE();
extern LVAL Xm_Text_Field_Widget_Class_Method_PASTE();
extern LVAL Xm_Text_Widget_Class_Method_GET_BASELINE();
extern LVAL Xm_Text_Field_Widget_Class_Method_GET_BASELINE();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_GET_DIR_LIST_ITEMS();
extern LVAL Xm_File_Selection_Box_Widget_Class_Method_GET_FILE_LIST_ITEMS();
extern LVAL Xm_List_Widget_Class_Method_PARENT();
extern LVAL Xm_Text_Widget_Class_Method_PARENT();
extern LVAL Widget_Class_Method_NAME();
extern LVAL Xm_Row_Column_Widget_Class_Method_GET_SUB_MENU_WIDGET();
extern LVAL Widget_Class_Method_GET_CHILDREN();
extern LVAL Widget_Class_Method_CALL_ACTION_PROC();
extern LVAL Wxt_Prim_XT_RESOLVE_PATHNAME();
extern LVAL Wxm_Prim_XM_SET_FONT_UNITS();
extern LVAL Wxms_Prim_XM_STRING_HAS_SUBSTRING();
extern LVAL Wxm_Prim_XM_TRACKING_LOCATE();
extern LVAL Wxm_Prim_XM_CONVERT_UNITS();
extern LVAL Wxms_Prim_XM_CVT_CT_TO_XM_STRING();
extern LVAL Wxms_Prim_XM_CVT_XM_STRING_TO_CT();
extern LVAL Widget_Class_Method_PROCESS_TRAVERSAL();
#endif				/* WINTERP_MOTIF_11 */

#ifndef WINTERP_MOTIF_11	/* HP_GRAPH_WIDGET doesn't work with 1.1 */
#ifdef HP_GRAPH_WIDGET		/* if HP_GRAPH_WIDGET defined */
extern LVAL Xm_Graph_Widget_Class_Method_ISNEW();
extern LVAL Xm_Arc_Widget_Class_Method_ISNEW();
extern LVAL Xm_Graph_Widget_Class_Method_ADD_CALLBACK();
extern LVAL Xm_Graph_Widget_Class_Method_SET_CALLBACK();
extern LVAL Xm_Graph_Widget_Class_Method_CENTER_AROUND_WIDGET();
extern LVAL Xm_Graph_Widget_Class_Method_DESTROY_ALL_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_DESTROY_ALL_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_DESTROY_SELECTED_ARCS_OR_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_GET_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_GET_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_GET_ARCS_BETWEEN_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_GET_NODE_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_GET_ROOTS();
extern LVAL Xm_Graph_Widget_Class_Method_GET_SELECTED_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_GET_SELECTED_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_INPUT_OVER_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_INSERT_ROOTS();
extern LVAL Xm_Graph_Widget_Class_Method_IS_POINT_IN_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_IS_SELECTED_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_IS_SELECTED_NODE();
extern LVAL Xm_Graph_Widget_Class_Method_MOVE_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_MOVE_NODE();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_NODE_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_ROOTS();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_SELECTED_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_NUM_SELECTED_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_MOVE_ALL();
extern LVAL Xm_Graph_Widget_Class_Method_LAYOUT();
extern LVAL Xm_Graph_Widget_Class_Method_RELAY_SUBGRAPH();
extern LVAL Xm_Graph_Widget_Class_Method_REMOVE_ARC_BETWEEN_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_REMOVE_ROOTS();
extern LVAL Xm_Graph_Widget_Class_Method_SELECT_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_SELECT_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_SELECT_NODE();
extern LVAL Xm_Graph_Widget_Class_Method_SELECT_NODES();
extern LVAL Xm_Graph_Widget_Class_Method_UNSELECT_ARC();
extern LVAL Xm_Graph_Widget_Class_Method_UNSELECT_ARCS();
extern LVAL Xm_Graph_Widget_Class_Method_UNSELECT_NODE();
extern LVAL Xm_Graph_Widget_Class_Method_UNSELECT_NODES();
#endif				/* HP_GRAPH_WIDGET */
#endif				/* notdef(WINTERP_MOTIF_11) */

#ifdef hpux
extern LVAL Wut_Prim_X_REFRESH_DISPLAY(); /* I'm too lazy to make this portable */
#endif				/* hpux */

/*
 * The function table -- funtab[].
 *
 * NOTE:
 * The order and number of entries in funtab[] must correspond to the
 * number and order of the enumerations (function indexes) created in
 * w_funtab.h.
 *
 * When adding entries to funtab[], make sure to update the #define
 * INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp to the entry number of the last
 * element added to funtab[], then add a corresponding entry in w_funtab.h
 * and update LAST_FUNTAB_POINTER_USED_BY_libWinterp to the last entry in
 * w_funtab.h. (You will also need to declare the function "extern" above.)
 *
 * Function Wfu_Funtab_Sanity_Check() is called from main() and checks to make
 * sure that the number of entries in funtab[] correspond to the number of
 * indexes in w_funtab.h. This test catches the majority of errors in keeping
 * the funtab[] table up to date with the pointers in w_funtab.h. Note however
 * that the test will only work if you keep
 * LAST_FUNTAB_POINTER_USED_BY_libWinterp and
 * INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp up to date.
 *
 * In order to decouple your c-language extensions to WINTERP from changes in
 * future releases of WINTERP, you should not add new entries to w_funtab.c
 * and w_funtab.h. To add a function or method <function> to WINTERP, you should
 * (1) declare the new function as "extern" by adding a line of the form
 * "extern LVAL <function>();" in app_funextn.h;
 * (2) create a symbolic index for the function or method by adding a line
 * "FTAB_<function>," to app_funidx.h;
 * (3) associate a lisp symbol name <FUNCTION-NAME> (must be all uppercase)
 * to the C function <function> by adding the following line to app_fundecl.h:
 * "{"<FUNCTION-NAME>", S, <function>},"
 *
 * If you are adding a method, rather than a function, do the same in
 * steps (1) and (2), but for (3) you should add a line like the following to
 * app_fundecl.h: "{NULL, S, <function>},".
 * To associate a message keyword <:MESSAGE> to a method on <widgetclass>,
 * you must call xladdmsg() in the portion of a file that initializes
 * a class. An example of such a call is:
 * xladdmsg(<widgetclass>, "<:MESSAGE>", FTAB_<function>).
 * The file defining a class and it's methods should #include w_funtab.h in
 * order to get FTAB_<function>.
 * See files wc_*.c for examples of defining methods/messages.
 */
FUNDEF funtab[] = {
  /*
   * NOTE-FROM-NPM: if you re-order any entries between index 0-15, you must
   * also change the #defines in xlisp/xlisp.h accordingly:
   * #define FT_RMHASH   0
   * #define FT_RMQUOTE	 1
   * #define FT_RMDQUOTE 2
   * #define FT_RMBQUOTE 3
   * #define FT_RMCOMMA  4
   * #define FT_RMLPAR   5
   * #define FT_RMRPAR   6
   * #define FT_RMSEMI   7
   * -- #define xxxxxx   8 --
   * -- #define yyyyyy   9 --
   * #define FT_CLNEW	 10
   * #define FT_CLISNEW	 11
   * #define FT_CLANSWER 12
   * #define FT_OBISNEW	 13
   * #define FT_OBCLASS	 14
   * #define FT_OBSHOW	 15
   */

  /* read macro functions */
  {	NULL,				S, rmhash		}, /*   0 */
  {	NULL,				S, rmquote		}, /*   1 */
  {	NULL,				S, rmdquote		}, /*   2 */
  {	NULL,				S, rmbquote		}, /*   3 */
  {	NULL,				S, rmcomma		}, /*   4 */
  {	NULL,				S, rmlpar		}, /*   5 */
  {	NULL,				S, rmrpar		}, /*   6 */
  {	NULL,				S, rmsemi		}, /*   7 */
  {	NULL,				S, xnotimp		}, /*   8 */
  {	NULL,				S, xnotimp		}, /*   9 */

  /* methods */
  {	NULL,				S, clnew		}, /*  10 */
  {	NULL,				S, clisnew		}, /*  11 */
  {	NULL,				S, clanswer		}, /*  12 */
  {	NULL,				S, obisnew		}, /*  13 */
  {	NULL,				S, obclass		}, /*  14 */
  {	NULL,				S, obshow		}, /*  15 */
  {	NULL,				S, xnotimp		}, /*  16 */
  {	NULL,				S, xnotimp		}, /*  17 */
  {	NULL,				S, xnotimp		}, /*  18 */
  {	NULL,				S, xnotimp		}, /*  19 */

  /* evaluator functions */
  {	"EVAL",				S, xeval		}, /*  20 */
  {	"APPLY",			S, xapply		}, /*  21 */
  {	"FUNCALL",			S, xfuncall		}, /*  22 */
  {	"QUOTE",			F, xquote		}, /*  23 */
  {	"FUNCTION",			F, xfunction		}, /*  24 */
  {	"BACKQUOTE",			F, xbquote		}, /*  25 */
  {	"LAMBDA",			F, xlambda		}, /*  26 */

  /* symbol functions */
  {	"SET",				S, xset			}, /*  27 */
  {	"SETQ",				F, xsetq		}, /*  28 */
  {	"SETF",				F, xsetf		}, /*  29 */
  {	"DEFUN",			F, xdefun		}, /*  30 */
  {	"DEFMACRO",			F, xdefmacro		}, /*  31 */
  {	"GENSYM",			S, xgensym		}, /*  32 */
  {	"MAKE-SYMBOL",			S, xmakesymbol		}, /*  33 */
  {	"INTERN", 			S, xintern		}, /*  34 */
  {	"SYMBOL-NAME",			S, xsymname		}, /*  35 */
  {	"SYMBOL-VALUE",			S, xsymvalue		}, /*  36 */
  {	"SYMBOL-PLIST",			S, xsymplist		}, /*  37 */
  {	"GET",				S, xget			}, /*  38 */
  {	"PUTPROP", 			S, xputprop		}, /*  39 */
  {	"REMPROP",			S, xremprop		}, /*  40 */
  {	"HASH",				S, xhash		}, /*  41 */

  /* array functions */
  {	"MAKE-ARRAY",			S, xmkarray		}, /*  42 */
  {	"AREF",				S, xaref		}, /*  43 */
			
  /* list functions */
  {	"CAR",				S, xcar			}, /*  44 */
  {	"CDR",				S, xcdr			}, /*  45 */
			
  {	"CAAR",				S, xcaar		}, /*  46 */
  {	"CADR",				S, xcadr		}, /*  47 */
  {	"CDAR",				S, xcdar		}, /*  48 */
  {	"CDDR",				S, xcddr		}, /*  49 */

  {	"CAAAR",			S, xcaaar		}, /*  50 */
  {	"CAADR",			S, xcaadr		}, /*  51 */
  {	"CADAR",			S, xcadar		}, /*  52 */
  {	"CADDR",			S, xcaddr		}, /*  53 */
  {	"CDAAR",			S, xcdaar		}, /*  54 */
  {	"CDADR",			S, xcdadr		}, /*  55 */
  {	"CDDAR",			S, xcddar		}, /*  56 */
  {	"CDDDR",			S, xcdddr		}, /*  57 */

  {	"CAAAAR", 			S, xcaaaar		}, /*  58 */
  {	"CAAADR",			S, xcaaadr		}, /*  59 */
  {	"CAADAR",			S, xcaadar		}, /*  60 */
  {	"CAADDR",			S, xcaaddr		}, /*  61 */
  {	"CADAAR",		 	S, xcadaar		}, /*  62 */
  {	"CADADR",			S, xcadadr		}, /*  63 */
  {	"CADDAR",			S, xcaddar		}, /*  64 */
  {	"CADDDR",			S, xcadddr		}, /*  65 */
  {	"CDAAAR",			S, xcdaaar		}, /*  66 */
  {	"CDAADR",			S, xcdaadr		}, /*  67 */
  {	"CDADAR",			S, xcdadar		}, /*  68 */
  {	"CDADDR",			S, xcdaddr		}, /*  69 */
  {	"CDDAAR",			S, xcddaar		}, /*  70 */
  {	"CDDADR",			S, xcddadr		}, /*  71 */
  {	"CDDDAR",			S, xcdddar		}, /*  72 */
  {	"CDDDDR",			S, xcddddr		}, /*  73 */

  {	"CONS",				S, xcons		}, /*  74 */
  {	"LIST",				S, xlist		}, /*  75 */
  {	"APPEND",			S, xappend		}, /*  76 */
  {	"REVERSE",			S, xreverse		}, /*  77 */
  {	"LAST",				S, xlast		}, /*  78 */
  {	"NTH",				S, xnth			}, /*  79 */
  {	"NTHCDR",			S, xnthcdr		}, /*  80 */
  {	"MEMBER",			S, xmember		}, /*  81 */
  {	"ASSOC",			S, xassoc		}, /*  82 */
  {	"SUBST", 			S, xsubst		}, /*  83 */
  {	"SUBLIS",			S, xsublis		}, /*  84 */
  {	"REMOVE",			S, xremove		}, /*  85 */
  {	"LENGTH",			S, xlength		}, /*  86 */
  {	"MAPC",				S, xmapc		}, /*  87 */
  {	"MAPCAR",			S, xmapcar		}, /*  88 */
  {	"MAPL",				S, xmapl		}, /*  89 */
  {	"MAPLIST",			S, xmaplist		}, /*  90 */
			
  /* destructive list functions */
  {	"RPLACA",			S, xrplca		}, /*  91 */
  {	"RPLACD",			S, xrplcd		}, /*  92 */
  {	"NCONC",			S, xnconc		}, /*  93 */
  {	"DELETE",			S, xdelete		}, /*  94 */

  /* predicate functions */
  {	"ATOM",				S, xatom		}, /*  95 */
  {	"SYMBOLP",			S, xsymbolp		}, /*  96 */
  {	"NUMBERP",			S, xnumberp		}, /*  97 */
  {	"BOUNDP",			S, xboundp 		}, /*  98 */
  {	"NULL",				S, xnull		}, /*  99 */
  {	"LISTP",			S, xlistp		}, /* 100 */
  {	"CONSP",			S, xconsp		}, /* 101 */
  {	"MINUSP",			S, xminusp 		}, /* 102 */
  {	"ZEROP",			S, xzerop		}, /* 103 */
  {	"PLUSP",			S, xplusp		}, /* 104 */
  {	"EVENP",			S, xevenp		}, /* 105 */
  {	"ODDP",				S, xoddp		}, /* 106 */
  {	"EQ",				S, xeq			}, /* 107 */
  {	"EQL",				S, xeql			}, /* 108 */
  {	"EQUAL",			S, xequal		}, /* 109 */

  /* special forms */
  {	"COND",				F, xcond		}, /* 110 */
  {	"CASE",				F, xcase		}, /* 111 */
  {	"AND",				F, xand			}, /* 112 */
  {	"OR",				F, xor			}, /* 113 */
  {	"LET",				F, xlet			}, /* 114 */
  {	"LET*",				F, xletstar		}, /* 115 */
  {	"IF",				F, xif			}, /* 116 */
  {	"PROG",				F, xprog		}, /* 117 */
  {	"PROG*",			F, xprogstar		}, /* 118 */
  {	"PROG1",			F, xprog1		}, /* 119 */
  {	"PROG2",			F, xprog2		}, /* 120 */
  {	"PROGN",			F, xprogn		}, /* 121 */
  {	"GO",				F, xgo			}, /* 122 */
  {	"RETURN",			F, xreturn  		}, /* 123 */
  {	"DO",				F, xdo			}, /* 124 */
  {	"DO*",				F, xdostar  		}, /* 125 */
  {	"DOLIST",			F, xdolist  		}, /* 126 */
  {	"DOTIMES",			F, xdotimes		}, /* 127 */
  {	"CATCH",			F, xcatch		}, /* 128 */
  {	"THROW",			F, xthrow		}, /* 129 */
	
  /* debugging and error handling functions */
  {	"ERROR",			S, xerror		}, /* 130 */
  {	"CERROR",			S, xcerror  		}, /* 131 */
  {	"BREAK",			S, xbreak		}, /* 132 */
  {	"CLEAN-UP",			S, xcleanup		}, /* 133 */
  {	"TOP-LEVEL",			S, xtoplevel		}, /* 134 */
  {	"CONTINUE",			S, xcontinue		}, /* 135 */
  {	"ERRSET", 			F, xerrset  		}, /* 136 */
  {	"BAKTRACE",			S, xbaktrace		}, /* 137 */
  {	"EVALHOOK",			S, xevalhook		}, /* 138 */

  /* arithmetic functions */
  {	"TRUNCATE",			S, xfix			}, /* 139 */
  {	"FLOAT",			S, xfloat		}, /* 140 */
  {	"+",				S, xadd			}, /* 141 */
  {	"-",				S, xsub			}, /* 142 */
  {	"*",				S, xmul			}, /* 143 */
  {	"/",				S, xdiv			}, /* 144 */
  {	"1+",				S, xadd1		}, /* 145 */
  {	"1-",				S, xsub1		}, /* 146 */
  {	"REM",				S, xrem			}, /* 147 */
  {	"MIN",				S, xmin			}, /* 148 */
  {	"MAX",				S, xmax			}, /* 149 */
  {	"ABS",				S, xabs			}, /* 150 */
  {	"SIN",				S, xsin			}, /* 151 */
  {	"COS",				S, xcos			}, /* 152 */
  {	"TAN",				S, xtan			}, /* 153 */
  {	"EXPT",				S, xexpt		}, /* 154 */
  {	"EXP",				S, xexp			}, /* 155 */
  {	"SQRT",		  		S, xsqrt		}, /* 156 */
  {	"RANDOM",			S, xrand		}, /* 157 */
			
  /* bitwise logical functions */
  {	"LOGAND",			S, xlogand  		}, /* 158 */
  {	"LOGIOR",			S, xlogior  		}, /* 159 */
  {	"LOGXOR",			S, xlogxor  		}, /* 160 */
  {	"LOGNOT",			S, xlognot  		}, /* 161 */

  /* numeric comparison functions */
  {	"<",				S, xlss			}, /* 162 */
  {	"<=",				S, xleq			}, /* 163 */
  {	"=",				S, xequ			}, /* 164 */
  {	"/=",				S, xneq			}, /* 165 */
  {	">=",				S, xgeq			}, /* 166 */
  {	">",				S, xgtr			}, /* 167 */
			
  /* string functions */
  {	"STRCAT",			S, xstrcat  		}, /* 168 */
  {	"SUBSEQ",			S, xsubseq  		}, /* 169 */
  {	"STRING",			S, xstring  		}, /* 170 */
  {	"CHAR",				S, xchar		}, /* 171 */

  /* I/O functions */
  {	"READ",				S, xread		}, /* 172 */
  {	"PRINT",			S, xprint		}, /* 173 */
  {	"PRIN1",			S, xprin1		}, /* 174 */
  {	"PRINC",			S, xprinc		}, /* 175 */
  {	"TERPRI",			S, xterpri  		}, /* 176 */
  {	"FLATSIZE",			S, xflatsize		}, /* 177 */
  {	"FLATC",			S, xflatc		}, /* 178 */
			
  /* file I/O functions */
  {	"OPEN",				S, xopen		}, /* 179 */
  {	"FORMAT",			S, xformat  		}, /* 180 */
  {	"CLOSE",			S, xclose		}, /* 181 */
  {	"READ-CHAR",			S, xrdchar  		}, /* 182 */
  {	"PEEK-CHAR",			S, xpkchar  		}, /* 183 */
  {	"WRITE-CHAR",			S, xwrchar  		}, /* 184 */
  {	"READ-LINE",			S, xreadline		}, /* 185 */

  /* system functions */
#ifdef WINTERP			/* WINTERP uses it's own version of xload() */
  {	"LOAD",				S, Wut_Prim_LOAD	}, /* 186 */  
#else
  {	"LOAD",				S, xload		}, /* 186 */
#endif				/* WINTERP */
  {	"DRIBBLE",			S, xtranscript		}, /* 187 */

  /* functions specific to xldmem.c */
  {	"GC",				S, xgc			}, /* 188 */
  {	"EXPAND",			S, xexpand  		}, /* 189 */
  {	"ALLOC",			S, xalloc		}, /* 190 */
  {	"ROOM",				S, xmem			}, /* 191 */
#ifdef SAVERESTORE
  {	"SAVE",				S, xsave		}, /* 192 */
  {	"RESTORE",			S, xrestore		}, /* 193 */
#else
  {	NULL,				S, xnotimp		}, /* 192 */
  {	NULL,				S, xnotimp		}, /* 193 */
#endif
  /* end of functions specific to xldmem.c */

  {	"TYPE-OF",			S, xtype		}, /* 194 */
  {	"EXIT",				S, xexit		}, /* 195 */
  {	"PEEK",				S, xpeek		}, /* 196 */
  {	"POKE",				S, xpoke		}, /* 197 */
  {	"ADDRESS-OF",			S, xaddrs		}, /* 198 */

  /* new functions and special forms */
  {	"VECTOR",			S, xvector  		}, /* 199 */
  {	"BLOCK",			F, xblock		}, /* 200 */
  {	"RETURN-FROM",			F, xrtnfrom		}, /* 201 */
  {	"TAGBODY",			F, xtagbody		}, /* 202 */
  {	"PSETQ",			F, xpsetq		}, /* 203 */
  {	"FLET",				F, xflet		}, /* 204 */
  {	"LABELS",			F, xlabels  		}, /* 205 */
  {	"MACROLET",			F, xmacrolet		}, /* 206 */
  {	"UNWIND-PROTECT",		F, xunwindprotect	}, /* 207 */
  {	"PPRINT",			S, xpp			}, /* 208 */
  {	"STRING<",			S, xstrlss  		}, /* 209 */
  {	"STRING<=",			S, xstrleq  		}, /* 210 */
  {	"STRING=",			S, xstreql  		}, /* 211 */
  {	"STRING/=",			S, xstrneq  		}, /* 212 */
  {	"STRING>=",			S, xstrgeq  		}, /* 213 */
  {	"STRING>",			S, xstrgtr  		}, /* 214 */
  {	"STRING-LESSP",			S, xstrilss		}, /* 215 */
  {	"STRING-NOT-GREATERP",		S, xstrileq		}, /* 216 */
  {	"STRING-EQUAL",			S, xstrieql		}, /* 217 */
  {	"STRING-NOT-EQUAL",		S, xstrineq		}, /* 218 */
  {	"STRING-NOT-LESSP",		S, xstrigeq		}, /* 219 */
  {	"STRING-GREATERP",		S, xstrigtr		}, /* 220 */
  {	"INTEGERP",			S, xintegerp		}, /* 221 */
  {	"FLOATP",			S, xfloatp  		}, /* 222 */
  {	"STRINGP",			S, xstringp		}, /* 223 */
  {	"ARRAYP",			S, xarrayp  		}, /* 224 */
  {	"STREAMP",			S, xstreamp		}, /* 225 */
  {	"OBJECTP",			S, xobjectp		}, /* 226 */
  {	"STRING-UPCASE",		S, xupcase  		}, /* 227 */
  {	"STRING-DOWNCASE",		S, xdowncase		}, /* 228 */
  {	"NSTRING-UPCASE",		S, xnupcase		}, /* 229 */
  {	"NSTRING-DOWNCASE",		S, xndowncase		}, /* 230 */
  {	"STRING-TRIM",			S, xtrim		}, /* 231 */
  {	"STRING-LEFT-TRIM",		S, xlefttrim		}, /* 232 */
  {	"STRING-RIGHT-TRIM",		S, xrighttrim		}, /* 233 */
  {	"WHEN",				F, xwhen		}, /* 234 */
  {	"UNLESS",			F, xunless  		}, /* 235 */
  {	"LOOP",				F, xloop		}, /* 236 */
  {	"SYMBOL-FUNCTION",		S, xsymfunction		}, /* 237 */
  {	"FBOUNDP",			S, xfboundp		}, /* 238 */
  {	"SEND",				S, xsend		}, /* 239 */
  {	"SEND-SUPER",			S, xsendsuper		}, /* 240 */
  {	"PROGV",			F, xprogv		}, /* 241 */
  {	"CHARACTERP",			S, xcharp		}, /* 242 */
  {	"CHAR-INT",			S, xcharint		}, /* 243 */
  {	"INT-CHAR",			S, xintchar		}, /* 244 */
  {	"READ-BYTE",			S, xrdbyte  		}, /* 245 */
  {	"WRITE-BYTE",			S, xwrbyte  		}, /* 246 */
  {	"MAKE-STRING-INPUT-STREAM", 	S, xmkstrinput		}, /* 247 */
  {	"MAKE-STRING-OUTPUT-STREAM",	S, xmkstroutput		}, /* 248 */
  {	"GET-OUTPUT-STREAM-STRING",	S, xgetstroutput	}, /* 249 */
  {	"GET-OUTPUT-STREAM-LIST",	S, xgetlstoutput	}, /* 250 */
  {	"GCD",				S, xgcd			}, /* 251 */
  {	"GET-LAMBDA-EXPRESSION", 	S, xgetlambda		}, /* 252 */
  {	"MACROEXPAND",			S, xmacroexpand		}, /* 253 */
  {	"MACROEXPAND-1",		S, x1macroexpand	}, /* 254 */
  {	"CHAR<",			S, xchrlss  		}, /* 255 */
  {	"CHAR<=",			S, xchrleq  		}, /* 256 */
  {	"CHAR=",			S, xchreql  		}, /* 257 */
  {	"CHAR/=",			S, xchrneq  		}, /* 258 */
  {	"CHAR>=",			S, xchrgeq  		}, /* 259 */
  {	"CHAR>",			S, xchrgtr  		}, /* 260 */
  {	"CHAR-LESSP",			S, xchrilss		}, /* 261 */
  {	"CHAR-NOT-GREATERP",		S, xchrileq		}, /* 262 */
  {	"CHAR-EQUAL",			S, xchrieql		}, /* 263 */
  {	"CHAR-NOT-EQUAL",		S, xchrineq		}, /* 264 */
  {	"CHAR-NOT-LESSP",		S, xchrigeq		}, /* 265 */
  {	"CHAR-GREATERP",		S, xchrigtr		}, /* 266 */
  {	"UPPER-CASE-P",			S, xuppercasep		}, /* 267 */
  {	"LOWER-CASE-P",			S, xlowercasep		}, /* 268 */
  {	"BOTH-CASE-P",			S, xbothcasep		}, /* 269 */
  {	"DIGIT-CHAR-P",			S, xdigitp		}, /* 270 */
  {	"ALPHANUMERICP",		S, xalphanumericp	}, /* 271 */
  {	"CHAR-UPCASE",			S, xchupcase		}, /* 272 */
  {	"CHAR-DOWNCASE",		S, xchdowncase		}, /* 273 */
  {	"DIGIT-CHAR",			S, xdigitchar		}, /* 274 */
  {	"CHAR-CODE",			S, xcharcode		}, /* 275 */
  {	"CODE-CHAR",			S, xcodechar		}, /* 276 */
  {	"ENDP",				S, xendp		}, /* 277 */
  {	"REMOVE-IF",			S, xremif		}, /* 278 */
  {	"REMOVE-IF-NOT",		S, xremifnot		}, /* 279 */
  {	"DELETE-IF",			S, xdelif		}, /* 280 */
  {	"DELETE-IF-NOT",		S, xdelifnot		}, /* 281 */
  {	"TRACE",			F, xtrace		}, /* 282 */
  {	"UNTRACE",			F, xuntrace		}, /* 283 */
  {	"SORT",				S, xsort		}, /* 284 */

  /*
   * note: various entries added in xlisp 2.1 would go here, but since
   * I just got the 2.1 sources other stuff has been added in the 
   * meantine, and I don't feel like renumbering everything.
   * See index 394-402...
   */

  {"SYSTEM",	S, Prim_SYSTEM}, /* 285 */
  {"POPEN",	S, Prim_POPEN},	/* 286 */
  {"PCLOSE",	S, Prim_PCLOSE}, /* 287 */
  {NULL, S, Widget_Class_Method_ISNEW},	/* 288 */
  {NULL, S, Shell_Widget_Class_Method_ISNEW}, /* 289 */
  {NULL, S, Popup_Shell_Widget_Class_Method_ISNEW}, /* 290 */ 
  {NULL, S, Shell_Widget_Class_Method_REALIZE},	/* 291 */
  {NULL, S, Shell_Widget_Class_Method_UNREALIZE}, /* 292 */
  {NULL, S, Popup_Shell_Widget_Class_Method_POPUP}, /* 293 */
  {NULL, S, Popup_Shell_Widget_Class_Method_POPDOWN}, /* 294 */
  {NULL, S, Widget_Class_Method_MANAGE}, /* 295 */
  {NULL, S, Widget_Class_Method_UNMANAGE}, /* 296 */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_ISNEW}, /* 297 */
  {NULL, S, Xm_Form_Widget_Class_Method_ISNEW},	/* 298 */
  {NULL, S, Xm_List_Widget_Class_Method_ISNEW},	/* 299 */
  {NULL, S, Xm_Text_Widget_Class_Method_ISNEW},	/* 300 */
  {NULL, S, Xm_Message_Box_Widget_Class_Method_ISNEW}, /* 301 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_ISNEW}, /* 302 */
  {NULL, S, Xm_Selection_Box_Widget_Class_Method_ISNEW}, /* 303 */
  {NULL, S, Widget_Class_Method_SET_VALUES}, /* 304 */
  {NULL, S, Widget_Class_Method_ADD_CALLBACK}, /* 305 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_STRING}, /* 306 */
  {"XT_ADD_TIMEOUT",		S, Wto_Prim_XtAddTimeOut}, /* 307 */
  {"XT_REMOVE_TIMEOUT",		S, Wto_Prim_XtRemoveTimeout}, /* 308 */
  {"XT_REMOVE_CALLBACK",	S, Wcb_Prim_XtRemoveCallback}, /* 309 */
  {NULL, S, Widget_Class_Method_SET_CALLBACK}, /* 310 */
  {NULL, S, Widget_Class_Method_REMOVE_ALL_CALLBACKS}, /* 311 */
  {"X_ALLOC_COLOR",		S, Wut_Prim_XAllocColor}, /* 312 */
  {"XM_GET_PIXMAP",		S, Wpm_Prim_XmGetPixmap}, /* 313 */
  {"XM_INSTALL_IMAGE",		S, Wpm_Prim_XmInstallImage}, /* 314 */
  {"XM_UNINSTALL_IMAGE",	S, Wpm_Prim_XmUninstallImage}, /* 315 */
  {"XM_GET_XIMAGE_FROM_FILE",	S, Wpm_Prim_XmGetImageFromFile}, /* 316 */
  {NULL, S, Widget_Class_Method_UPDATE_DISPLAY}, /* 317 */
  {"GET_MOUSED_WIDGET",		S, Wut_UserClick_To_WidgetObj},	/* 318 */
  {NULL, S, Widget_Class_Method_DESTROY}, /* 319 */
  {NULL, S, Widget_Class_Method_PARENT}, /* 320 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_LAST_POSITION}, /* 321 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_STRING}, /* 322 */
  {NULL, S, Xm_Text_Widget_Class_Method_REPLACE}, /* 323 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_EDITABLE}, /* 324 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_EDITABLE}, /* 325 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_MAX_LENGTH}, /* 326 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_MAX_LENGTH}, /* 327  */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_SELECTION},	/* 328 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_SELECTION},	/* 329 */
  {NULL, S, Xm_Text_Widget_Class_Method_CLEAR_SELECTION}, /* 330 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_TOP_CHARACTER}, /* 331 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_TOP_CHARACTER}, /* 332 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_INSERTION_POSITION}, /* 333 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_INSERTION_POSITION}, /* 334 */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_SELECTION_POSITION}, /* 335 */
  {NULL, S, Xm_Text_Widget_Class_Method_XY_TO_POS}, /* 336 */
  {NULL, S, Xm_Text_Widget_Class_Method_POS_TO_XY}, /* 337 */
  {NULL, S, Xm_Text_Widget_Class_Method_SHOW_POSITION},	/* 338 */
  {NULL, S, Xm_Text_Widget_Class_Method_SCROLL}, /* 339 */
  {NULL, S, Xm_Text_Widget_Class_Method_DISABLE_REDISPLAY}, /* 340 */
  {NULL, S, Xm_Text_Widget_Class_Method_ENABLE_REDISPLAY}, /* 341 */
  {NULL, S, Xm_List_Widget_Class_Method_ADD_ITEM}, /* 342 */
  {NULL, S, Xm_List_Widget_Class_Method_ADD_ITEM_UNSELECTED}, /* 343 */
  {NULL, S, Xm_List_Widget_Class_Method_DELETE_ITEM}, /* 344 */
  {NULL, S, Xm_List_Widget_Class_Method_DELETE_POS}, /* 345 */
  {NULL, S, Xm_List_Widget_Class_Method_SELECT_ITEM}, /* 346 */
  {NULL, S, Xm_List_Widget_Class_Method_SELECT_POS}, /* 347 */
  {NULL, S, Xm_List_Widget_Class_Method_DESELECT_ITEM},	/* 348 */
  {NULL, S, Xm_List_Widget_Class_Method_DESELECT_POS}, /* 349 */
  {NULL, S, Xm_List_Widget_Class_Method_DESELECT_ALL_ITEMS}, /* 350 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_POS}, /* 351 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_BOTTOM_POS}, /* 352 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_ITEM}, /* 353 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_BOTTOM_ITEM}, /* 354 */
  {NULL, S, Xm_List_Widget_Class_Method_ITEM_EXISTS}, /* 355 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_HORIZ_POS},	/* 356 */
  {NULL, S, Xm_List_Widget_Class_Method_ADD_CALLBACK}, /* 357 */
  {NULL, S, Xm_List_Widget_Class_Method_SET_CALLBACK}, /* 358 */
  {NULL, S, Xm_Drawing_Area_Widget_Class_Method_ADD_CALLBACK}, /* 359 */
  {NULL, S, Xm_Drawing_Area_Widget_Class_Method_SET_CALLBACK}, /* 360 */
  {NULL, S, Xm_Drawn_Button_Widget_Class_Method_ADD_CALLBACK}, /* 361 */
  {NULL, S, Xm_Drawn_Button_Widget_Class_Method_SET_CALLBACK}, /* 362 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_ADD_CALLBACK}, /* 363 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_SET_CALLBACK}, /* 364 */
  {NULL, S, Xm_Scroll_Bar_Widget_Class_Method_ADD_CALLBACK}, /* 365 */
  {NULL, S, Xm_Scroll_Bar_Widget_Class_Method_SET_CALLBACK}, /* 366 */
  {NULL, S, Xm_Toggle_Button_Widget_Class_Method_ADD_CALLBACK},	/* 367 */
  {NULL, S, Xm_Toggle_Button_Widget_Class_Method_SET_CALLBACK},	/* 368 */
  {NULL, S, Xm_Selection_Box_Widget_Class_Method_ADD_CALLBACK},	/* 369 */
  {NULL, S, Xm_Selection_Box_Widget_Class_Method_SET_CALLBACK},	/* 370 */
  {NULL, S, Xm_Command_Widget_Class_Method_ADD_CALLBACK}, /* 371 */
  {NULL, S, Xm_Command_Widget_Class_Method_SET_CALLBACK}, /* 372 */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_ADD_CALLBACK}, /* 373 */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_SET_CALLBACK}, /* 374 */
  {NULL, S, Xm_Scale_Widget_Class_Method_ADD_CALLBACK},	/* 375 */
  {NULL, S, Xm_Scale_Widget_Class_Method_SET_CALLBACK},	/* 376 */
  {NULL, S, Xm_Text_Widget_Class_Method_ADD_CALLBACK}, /* 377 */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_CALLBACK}, /* 378 */
  {NULL, S, Widget_Class_Method_GET_VALUES}, /* 379 */
  {NULL, S, Xm_Cascade_Button_Widget_Class_Method_HIGHLIGHT}, /* 380 */
  {NULL, S, Xm_Command_Widget_Class_Method_GET_CHILD}, /* 381 */
  {NULL, S, Xm_Command_Widget_Class_Method_SET_VALUE}, /* 382 */
  {NULL, S, Xm_Command_Widget_Class_Method_APPEND_VALUE}, /* 383 */
  {NULL, S, Xm_Command_Widget_Class_Method_ERROR}, /* 384 */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_GET_CHILD}, /* 385 */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_DO_SEARCH}, /* 386 */
  {NULL, S, Xm_Main_Window_Widget_Class_SET_AREAS}, /* 387 */
  {NULL, S, Xm_Main_Window_Widget_Class_SEP1}, /* 388 */
  {NULL, S, Xm_Main_Window_Widget_Class_SEP2}, /* 389 */
  {NULL, S, Xm_Message_Box_Widget_Class_Method_GET_CHILD}, /* 390 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_MENU_POSITION}, /* 391 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_OPTION_LABEL_GADGET}, /* 392 */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_OPTION_BUTTON_GADGET}, /* 393 */
  {"DEFSTRUCT",			F, xdefstruct		}, /* 394 */
  {"%STRUCT-TYPE-P",		S, xstrtypep		}, /* 395 */
  {"%MAKE-STRUCT",		S, xmkstruct		}, /* 396 */
  {"%COPY-STRUCT",		S, xcpystruct		}, /* 397 */
  {"%STRUCT-REF",		S, xstrref		}, /* 398 */
  {"%STRUCT-SET",		S, xstrset		}, /* 399 */
  {"ASIN",			S, xasin		}, /* 400 */
  {"ACOS",			S, xacos		}, /* 401 */
  {"ATAN",			S, xatan		}, /* 402 */
  {"FSCANF-FIXNUM",		S, Prim_FSCANF_FIXNUM	}, /* 403 */
  {"FSCANF-STRING",		S, Prim_FSCANF_STRING	}, /* 404 */
  {"FSCANF-FLONUM",		S, Prim_FSCANF_FLONUM	}, /* 405 */
  {"COPY-ARRAY",		S, Prim_COPY_ARRAY	}, /* 406 */
  {"ARRAY-INSERT-POS",		S, Prim_ARRAY_INSERT_POS}, /* 407 */
  {"ARRAY-DELETE-POS",		S, Prim_ARRAY_DELETE_POS}, /* 408 */
  {NULL, S, Shell_Widget_Class_Method_IS_MOTIF_WM_RUNNING}, /* 409 */
  {NULL, S, Xm_Scale_Widget_Class_Method_SET_VALUE}, /* 410 */
  {NULL, S, Xm_Scale_Widget_Class_Method_GET_VALUE}, /* 411 */
  {NULL, S, Xm_Scroll_Bar_Widget_Class_Method_SET_VALUE}, /* 412 */
  {NULL, S, Xm_Scroll_Bar_Widget_Class_Method_GET_VALUE}, /* 413 */
  {NULL, S, Xm_Scrolled_Window_Widget_Class_Method_SET_AREAS}, /* 414 */
  {NULL, S, Xm_Selection_Box_Widget_Class_Method_GET_CHILD}, /* 415 */
  {NULL, S, Xm_Toggle_Button_Widget_Class_Method_GET_STATE}, /* 416 */
  {NULL, S, Xm_Toggle_Button_Widget_Class_Method_SET_STATE}, /* 417 */
  {NULL, S, Xm_Toggle_Button_Gadget_Class_Method_GET_STATE}, /* 418 */
  {NULL, S, Xm_Toggle_Button_Gadget_Class_Method_SET_STATE}, /* 419 */
  {"XT_PARSE_TRANSLATION_TABLE", S, Wtx_Prim_XT_PARSE_TRANSLATION_TABLE}, /* 420 */
  {"XT_PARSE_ACCELERATOR_TABLE", S, Wtx_Prim_XT_PARSE_ACCELERATOR_TABLE}, /* 421 */
  {NULL, S, Widget_Class_Method_OVERRIDE_TRANSLATIONS},	/* 422 */
  {NULL, S, Widget_Class_Method_AUGMENT_TRANSLATIONS}, /* 423 */
  {NULL, S, Widget_Class_Method_UNINSTALL_TRANSLATIONS}, /* 424 */
  {NULL, S, Widget_Class_Method_INSTALL_ACCELERATORS}, /* 425 */
  {NULL, S, Widget_Class_Method_INSTALL_ALL_ACCELERATORS}, /* 426 */
  {NULL, S, Widget_Class_Method_ADD_EVENT_HANDLER}, /* 427 */
  {NULL, S, Widget_Class_Method_SET_EVENT_HANDLER}, /* 428 */
  {NULL, S, Widget_Class_Method_BUILD_EVENT_MASK}, /* 429 */
  {"REMOVE_EVENT_HANDLER",	S, Weh_Prim_REMOVE_EVENT_HANDLER}, /* 430 */
  {"XT_MANAGE_CHILDREN",	S, Wxt_Prim_XT_MANAGE_CHILDREN}, /* 431 */
  {"XT_UNMANAGE_CHILDREN",	S, Wxt_Prim_XT_UNMANAGE_CHILDREN}, /* 432 */
  {NULL, S, Widget_Class_Method_ADD_TAB_GROUP},	/* 433 */
  {NULL, S, Widget_Class_Method_REMOVE_TAB_GROUP}, /* 434 */
  {NULL, S, Widget_Class_Method_ADD_GRAB}, /* 435 */
  {NULL, S, Widget_Class_Method_REMOVE_GRAB}, /* 436 */
  {NULL, S, Widget_Class_Method_IS_COMPOSITE}, /* 437 */
  {NULL, S, Widget_Class_Method_IS_CONSTRAINT},	/* 438 */
  {NULL, S, Widget_Class_Method_IS_SHELL}, /* 439 */
  {NULL, S, Widget_Class_Method_IS_PRIMITIVE}, /* 440 */
  {NULL, S, Widget_Class_Method_IS_GADGET}, /* 441 */
  {NULL, S, Widget_Class_Method_IS_MANAGER}, /* 442 */
  {NULL, S, Widget_Class_Method_SET_SENSITIVE},	/* 443 */
  {NULL, S, Widget_Class_Method_SET_MAPPED_WHEN_MANAGED}, /* 444 */
  {NULL, S, Widget_Class_Method_IS_MANAGED}, /* 445 */
  {NULL, S, Widget_Class_Method_IS_REALIZED}, /* 446 */
  {NULL, S, Widget_Class_Method_IS_SENSITIVE}, /* 447 */
  {NULL, S, Widget_Class_Method_WINDOW}, /* 448 */
  {NULL, S, Widget_Class_Method_MAP}, /* 449 */
  {NULL, S, Widget_Class_Method_UNMAP},	/* 450 */
  {NULL, S, Xm_Bulletin_Board_Widget_Class_Method_ISNEW}, /* 451 */
  {"XM_STRING_CREATE",		S, Wxms_Prim_XM_STRING_CREATE},	/* 452 */
  {"XM_STRING_DIRECTION_CREATE",S, Wxms_Prim_XM_STRING_DIRECTION_CREATE}, /* 453 */
  {"XM_STRING_SEPARATOR_CREATE",S, Wxms_Prim_XM_STRING_SEPARATOR_CREATE}, /* 454 */
  {"XM_STRING_SEGMENT_CREATE",	S, Wxms_Prim_XM_STRING_SEGMENT_CREATE},	/* 455 */
  {"XM_STRING_CREATE_L_TO_R",	S, Wxms_Prim_XM_STRING_CREATE_L_TO_R}, /* 455 */
  {"XM_STRING_GET_L_TO_R",	S, Wxms_Prim_XM_STRING_GET_L_TO_R}, /* 457 */
  {"XM_STRING_CONCAT",		S, Wxms_Prim_XM_STRING_CONCAT},	/* 458 */
  {"XM_STRING_COPY",		S, Wxms_Prim_XM_STRING_COPY}, /* 459 */
  {"XM_STRING_BYTE_COMPARE",	S, Wxms_Prim_XM_STRING_BYTE_COMPARE}, /* 460 */
  {"XM_STRING_COMPARE",		S, Wxms_Prim_XM_STRING_COMPARE}, /* 461 */
  {"XM_STRING_LENGTH",		S, Wxms_Prim_XM_STRING_LENGTH},	/* 462 */
  {"XM_STRING_EMPTY",		S, Wxms_Prim_XM_STRING_EMPTY}, /* 463 */
  {"XM_STRING_LINE_COUNT",	S, Wxms_Prim_XM_STRING_LINE_COUNT}, /* 464 */
  {"WIDGETOBJP",		S, Wcls_Prim_WIDGETOBJP}, /* 465 */
  {"X_STORE_COLOR",		S, Wut_Prim_X_STORE_COLOR}, /* 466 */
  {"X_ALLOC_N_COLOR_CELLS_NO_PLANES", S, Wut_Prim_X_ALLOC_N_COLOR_CELLS_NO_PLANES}, /* 467 */
  {NULL, S, Xm_Command_Widget_Class_Method_ISNEW}, /* 468 */
  {NULL, S, Application_Shell_Widget_Class_Method_GET_ARGV}, /* 469 */
  {NULL, S, Application_Shell_Widget_Class_Method_SET_ARGV}, /* 470 */
  {NULL, S, Xm_Command_Widget_Class_Method_GET_HISTORY_ITEMS}, /* 471 */
  {NULL, S, Xm_List_Widget_Class_Method_GET_ITEMS}, /* 472 */
  {NULL, S, Xm_List_Widget_Class_Method_GET_SELECTED_ITEMS}, /* 473 */
  {NULL, S, Xm_Selection_Box_Widget_Class_Method_GET_LIST_ITEMS}, /* 474 */
  {NULL, S, Widget_Class_Method_HAS_CALLBACKS},	/* 475 */
  {NULL, S, Widget_Class_Method_EXISTS_P}, /* 476 */
  {NULL, S, Application_Shell_Widget_Class_Method_ISNEW}, /* 477 */
  {"XM_SET_MENU_CURSOR", S, Wxm_Prim_XM_SET_MENU_CURSOR}, /* 478 */
  {"GET_MOUSE_LOCATION", S, Wut_Prim_GET_MOUSE_LOCATION}, /* 479 -- uunet!cimshop!rhess */
  {"TIMEOUT_ACTIVE_P", S, Wto_Prim_TIMEOUT_ACTIVE_P}, /* 480 */

  /* Constant needed by Wfu_Funtab_Sanity_Check() */
#define INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp 480

#ifdef WINTERP_MOTIF_11
  {"XM_GET_COLORS", S, Wxm_Prim_XM_GET_COLORS},	/*  */
  {NULL, S, Xm_Arrow_Button_Widget_Class_Method_ADD_CALLBACK}, /*  */
  {NULL, S, Xm_Arrow_Button_Widget_Class_Method_SET_CALLBACK}, /*  */
  {NULL, S, Xm_Push_Button_Widget_Class_Method_ADD_CALLBACK}, /*  */
  {NULL, S, Xm_Push_Button_Widget_Class_Method_SET_CALLBACK}, /*  */
  {NULL, S, Xm_Cascade_Button_Gadget_Class_Method_HIGHLIGHT}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_ADD_ITEMS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_DELETE_ITEMS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_DELETE_ITEMS_POS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_DELETE_ALL_ITEMS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_REPLACE_ITEMS},	/*  */
  {NULL, S, Xm_List_Widget_Class_Method_REPLACE_ITEMS_POS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_SET_ADD_MODE}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_ITEM_POS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_GET_MATCH_POS},	/*  */
  {NULL, S, Xm_List_Widget_Class_Method_GET_SELECTED_POS}, /*  */
  {NULL, S, Xm_Main_Window_Widget_Class_SEP3}, /*  */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_GET_POSTED_FROM_WIDGET}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_STRING}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_LAST_POSITION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_STRING}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_REPLACE},	/*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_EDITABLE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_EDITABLE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_MAX_LENGTH}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_MAX_LENGTH}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_SELECTION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_SELECTION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_CLEAR_SELECTION},	/*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_INSERTION_POSITION}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_CURSOR_POSITION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_CURSOR_POSITION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_INSERTION_POSITION}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_CURSOR_POSITION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_CURSOR_POSITION}, /* */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_SELECTION_POSITION}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_XY_TO_POS}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_POS_TO_XY}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SHOW_POSITION}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_HIGHLIGHT},	/*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_HIGHLIGHT}, /* */
  {NULL, S, Xm_Text_Widget_Class_Method_INSERT}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_INSERT}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_SET_ADD_MODE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_SET_ADD_MODE}, /* */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_ADD_MODE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_ADD_MODE}, /* */
  {NULL, S, Xm_Text_Widget_Class_Method_REMOVE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_REMOVE}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_COPY}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_COPY}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_CUT}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_CUT}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_PASTE},	/*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_PASTE}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_GET_BASELINE}, /*  */
  {NULL, S, Xm_Text_Field_Widget_Class_Method_GET_BASELINE}, /* */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_GET_DIR_LIST_ITEMS}, /*  */
  {NULL, S, Xm_File_Selection_Box_Widget_Class_Method_GET_FILE_LIST_ITEMS}, /*  */
  {NULL, S, Xm_List_Widget_Class_Method_PARENT}, /*  */
  {NULL, S, Xm_Text_Widget_Class_Method_PARENT}, /*  */
  {NULL, S, Widget_Class_Method_NAME}, /*  */
  {NULL, S, Xm_Row_Column_Widget_Class_Method_GET_SUB_MENU_WIDGET}, /*  */
  {NULL, S, Widget_Class_Method_GET_CHILDREN}, /* */
  {NULL, S, Widget_Class_Method_CALL_ACTION_PROC}, /* */
  {"XT_RESOLVE_PATHNAME", S, Wxt_Prim_XT_RESOLVE_PATHNAME}, /*  */
  {"XM_SET_FONT_UNITS", S, Wxm_Prim_XM_SET_FONT_UNITS},	/*  */
  {"XM_STRING_HAS_SUBSTRING", S, Wxms_Prim_XM_STRING_HAS_SUBSTRING}, /*  */
  {"XM_TRACKING_LOCATE", S, Wxm_Prim_XM_TRACKING_LOCATE}, /*  */
  {"XM_CONVERT_UNITS", S, Wxm_Prim_XM_CONVERT_UNITS}, /*  */
  {"XM_CVT_CT_TO_XM_STRING", S, Wxms_Prim_XM_CVT_CT_TO_XM_STRING}, /*  */
  {"XM_CVT_XM_STRING_TO_CT", S, Wxms_Prim_XM_CVT_XM_STRING_TO_CT}, /*  */
  {NULL, S, Widget_Class_Method_PROCESS_TRAVERSAL}, /*  */
  /*
   | {NULL, S, },   -* xxx *-
   */
  /* Constant needed by Wfu_Funtab_Sanity_Check() */
#undef INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp
#define INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp 553
#endif				/* WINTERP_MOTIF_11 */

#ifndef WINTERP_MOTIF_11	/* HP_GRAPH_WIDGET doesn't work with 1.1 */
#ifdef HP_GRAPH_WIDGET		/* if HP_GRAPH_WIDGET defined */
  {NULL, S, Xm_Graph_Widget_Class_Method_ISNEW}, /*  */
  {NULL, S, Xm_Arc_Widget_Class_Method_ISNEW}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_ADD_CALLBACK},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_SET_CALLBACK},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_CENTER_AROUND_WIDGET},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_DESTROY_ALL_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_DESTROY_ALL_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_DESTROY_SELECTED_ARCS_OR_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_ARCS_BETWEEN_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_NODE_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_ROOTS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_SELECTED_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_GET_SELECTED_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_INPUT_OVER_ARC}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_INSERT_ROOTS},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_IS_POINT_IN_ARC}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_IS_SELECTED_ARC}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_IS_SELECTED_NODE}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_MOVE_ARC}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_MOVE_NODE}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_NODE_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_ROOTS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_SELECTED_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_NUM_SELECTED_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_MOVE_ALL}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_LAYOUT}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_RELAY_SUBGRAPH}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_REMOVE_ARC_BETWEEN_NODES}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_REMOVE_ROOTS},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_SELECT_ARC}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_SELECT_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_SELECT_NODE}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_SELECT_NODES},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_UNSELECT_ARC},	/*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_UNSELECT_ARCS}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_UNSELECT_NODE}, /*  */
  {NULL, S, Xm_Graph_Widget_Class_Method_UNSELECT_NODES}, /*  */

  /* Constant needed by Wfu_Funtab_Sanity_Check() */
#undef INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp
#define INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp 521
#endif				/* HP_GRAPH_WIDGET */
#endif				/* notdef(WINTERP_MOTIF_11) */

#ifdef hpux			/* I'm too lazy to make this portable */
  {"X_REFRESH_DISPLAY",		S, Wut_Prim_X_REFRESH_DISPLAY},	/* */
#endif

  /*
   * applications built on winterp may add new function table entries via
   * this include file. The indexes of entries in this file must correspond
   * to the indexes computed by c in app_fundecl.h
   */
#include "app_fundecl.h"

  {0,0,0}			/* end of table marker */

};

/* xnotimp - function table entries that are currently not implemented */
LOCAL LVAL xnotimp()
{
    xlfail("function not implemented");
}

/*******************************************************************************
 * A sanity check called from main() which ensures that the number of indexes to
 * function table entries (in w_funtab.h) corresnpond to the actual number of
 * entries in funtab[] in this file.
 *
 * LAST_FUNTAB_POINTER_USED_BY_libWinterp is declared in w_funtab.h, and
 * INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp is declared above.
 *******************************************************************************/
void Wfu_Funtab_Sanity_Check()
{
  extern char* app_name;	/* winterp.c */

  if (LAST_FUNTAB_POINTER_USED_BY_libWinterp != INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp) {
    (void) fprintf(stderr,"%s: Fatal error: w_funtab.c out of sync with w_funtab.h\n\tLAST_FUNTAB_POINTER_USED_BY_libWinterp=%d\n\tINDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp=%d\n\t -- correct errors in those files and recompile!\n",
		   app_name,
		   LAST_FUNTAB_POINTER_USED_BY_libWinterp,
		   INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp);
    exit(1);
  }
}
