/*
** Copyright (c) 1990 David E. Smyth
**
** Redistribution and use in source and binary forms are permitted
** provided that the above copyright notice and this paragraph are
** duplicated in all such forms and that any documentation, advertising
** materials, and other materials related to such distribution and use
** acknowledge that the software was developed by David E. Smyth.  The
** name of David E. Smyth may not be used to endorse or promote products
** derived from this software without specific prior written permission.
** THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
** WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
*/

/*
*
* Subsystem_group:
*
*     Widget Creation Library - OpenLook Resource Interpreter
*
* Module_description:
*
*     This module contains registration routine for all OpenLook
*     widget constructors and classes.  
*
*     The two functions XopRegisterAll() and OriRegisterOpenLook()
*     are exactly equivalent.
*
* Module_interface_summary: 
*
*     void XopRegisterAll     ( XtAppContext app )
*     void OriRegisterOpenLook ( XtAppContext app )
*
* Module_history:
*   Contact Kim Gillies at gillies@noao.edu with suggestions or comments.
*                                                  
*   mm/dd/yy  initials  function  action
*   --------  --------  --------  ---------------------------------------------
*   06/19/90  D.Smyth   all	  create.
*   12/01/90  K.Gillies all       modified for OLIT support.
*   09/27/91  MarBru    table     Conditionalized table support
*   10/05/91  MarBru    Quarks    Added quark definitions from Ori
*
* Design_notes:
*    
*******************************************************************************
*/
/*
*******************************************************************************
* Include_files.
*******************************************************************************
*/

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xol/OpenLook.h>
#include <Xol/memutil.h>
#include <Xol/buffutil.h>
#include <Xol/textbuff.h>
#include <Xol/Dynamic.h>

#include <Xol/AbbrevMenu.h>
#include <Xol/BaseWindow.h>
#include <Xol/BulletinBo.h>
#include <Xol/Caption.h>
#include <Xol/CheckBox.h>
#include <Xol/ControlAre.h>
#include <Xol/Exclusives.h>
#include <Xol/FCheckBox.h>
#include <Xol/FExclusive.h>
#include <Xol/FNonexclus.h>
#include <Xol/FooterPane.h>
#include <Xol/Form.h>
#include <Xol/Gauge.h>

#include <Xol/Menu.h>
#include <Xol/MenuButton.h>
#include <Xol/Nonexclusi.h>
#include <Xol/Notice.h>
#include <Xol/OblongButt.h>
#include <Xol/PopupWindo.h>
#include <Xol/RectButton.h>
#include <Xol/Scrollbar.h>
#include <Xol/ScrolledWi.h>
#include <Xol/ScrollingL.h>
#include <Xol/Slider.h>
#include <Xol/StaticText.h>
#include <Xol/Stub.h>
#include <Xol/TextEdit.h>
#include <Xol/TextField.h>

#ifdef TABLE
#include <Xp/Table.h>
#endif

#include "../Wc/WcCreate.h"
#include "../Wc/WcCreateP.h"


/* This is included so I could use the 2.5 OLIT libraries with
 * x11r5.  These values have been made static in the Xt/Converters.c
 * file.  I'm not sure what effects this has, but it seems to work.
 * Alternate solutions will be gratefully received!
 * MARBRU: And I am also intializing those ....
 */
#ifdef XtSpecificationRelease && XtSpecificationRelease > 4
XrmQuark  XtQInt;
XrmQuark  XtQFontStruct;
XrmQuark  XtQBoolean;
XrmQuark  XtQFont;
XrmQuark  XtQString;
#endif


/* Define this to include the list demo callbacks */
#define INCLUDEDEMOS

void OriRegisterOpenLook();

/* This converter is included for the scrolling list viewHeight
 * resource.
 */
/*
void CvtStringToCardinal(args, num_args, fromVal, toVal)
  XrmValuePtr args;
  Cardinal *num_args;
  XrmValuePtr fromVal;
  XrmValuePtr toVal;
{
  static unsigned int l;

  if (*num_args != 0)
    XtWarningMsg("wrongParameters", "cvtStringToCardinal","XtToolkitError",
      "String to Cardinal conversion needs no extra arguments",
      (String *)NULL, (Cardinal *)NULL);
  if (sscanf((char *)fromVal->addr, "%ud", &l) == 1) {
    (*toVal).size = sizeof(unsigned int);
    (*toVal).addr = (caddr_t)&l;
  } else {
    XtStringConversionWarning((char *) fromVal->addr, XtRCardinal);
  }
}
*/
/* The following is included to support testing of scrolling lists
 * only if INCLUDEDEMOS is defined.
 */
#ifdef INCLUDEDEMOS

static String OlTestItems[] = {
	"test1", "test2", "test3", "test4", "a long test5",
	"test6", "test7", "test8", "a long test10",
};
#define NOLTESTITEMS	(sizeof(OlTestItems)/sizeof(String))

/* Private data for each list */
typedef struct OlSLDemoData {
  OlListToken current;
  Widget this;         /* This sl if needed */
} OlSLDemoData;


/* Constructor */
Widget OlScrollingListDemoCreate ( /* Widget w, 
			  String name, 
			  ArgList args, 
			  Cardinal num_args */ );
/* Demo callbacks */
static void OlScrollingListDemoSelectCB(/* Widget w, caddr_t client_data,
					caddr_t call_data */ );

static void OlScrollingListDemoVerifyCB( /* Widget w,
					    caddr_t client_data,
					    caddr_t call_data */ );
static void OlScrollingListDemoDeleteCB( /* Widget w, caddr_t client_data,
				    caddr_t call_data */ );
static void OlScrollingListDemoDeleteCB(/* Widget w, caddr_t client_data,
				    caddr_t call_data */ );


/* Global function pointers to list routines */
static OlListToken	(*SLAddItem)();
static void		(*SLDeleteItem)();
static void		(*SLEditClose)();
static void		(*SLEditOpen)();
static void		(*SLTouchItem)();
static void		(*SLUpdateView)();
static void		(*SLViewItem)();

static Widget OlScrollingListDataFetch(ref, name)
Widget ref;
char *name;
{
  Widget list;

  if (name == NULL) {
    fprintf(stderr, "List Demo callbacks need a list argument.\n");
    return NULL;
  }
  list =  WcFullNameToWidget(ref, name);
  if (list == NULL) {
    fprintf(stderr, "List Demo Edit could not find widget: `%s'\n", name);
    return NULL;
  }
/*
  fprintf(stderr, "list: %d, ref: %d, %s\n", list, ref, name);
*/
  return list;
}

/* Function OlScrollingListDemoCreate
 * Creates a scrolling list, adds test data.
 */
Widget OlScrollingListDemoCreate(parent, name, inargs, nargs)
  Widget parent;
  String name;
  Arg *inargs;
  Cardinal nargs;
{
  Widget list;
  OlListItem item;
  OlSLDemoData *oldata;
  static int first = 1;          /* first time flag */
  int i;

  oldata = (OlSLDemoData *)XtMalloc(sizeof(OlSLDemoData));
 
  list = XtCreateWidget(name, scrollingListWidgetClass,
	parent, inargs, nargs);
  /* Fetch the list functions for global use. */
  if (first) {              /* Just do this the first time */
    XtVaGetValues(list,
		  XtNapplAddItem, &SLAddItem,
		  XtNapplDeleteItem, &SLDeleteItem,
		  XtNapplEditClose, &SLEditClose,
		  XtNapplEditOpen, &SLEditOpen,
		  XtNapplTouchItem, &SLTouchItem,
		  XtNapplUpdateView, &SLUpdateView,
		  XtNapplViewItem, &SLViewItem, NULL);
  }
  first = 0;

  oldata = (OlSLDemoData *)XtMalloc(sizeof(OlSLDemoData));
  /* Set up the demo list data */
  oldata->this = list;
  oldata->current = (OlListToken)0;
  XtVaSetValues(list, XtNuserData, (XtPointer)oldata, NULL);

  /* Pump in the test data */
  for (i = 0; i < NOLTESTITEMS; i++) {
    item.label_type = OL_STRING;
    item.label = OlTestItems[i];
    item.attr = 0;
    (void)(*SLAddItem)(list, 0, (OlListToken)0, item);
  }
  return list;
}

/* Used with the userMakeCurrent callback, changes the selected item. */
static void OlScrollingListDemoSelectCB(w, client_data, call_data)
  Widget w;
  XtPointer client_data;
  XtPointer call_data;
{
  OlListItem *item;
  OlListToken token;
  OlSLDemoData *oldata;

  token = (OlListToken)call_data;
  item = OlListItemPointer(token);

  /* This widget is an sl so just get the user data */
  XtVaGetValues(w, XtNuserData, (XtPointer *)&oldata, NULL);

  if (token == oldata->current) {
    fprintf(stderr, "selected: %s, again.\n", item->label);
  } else {
    fprintf(stderr, "selected: %s.\n", item->label);

    item->attr |= OL_LIST_ATTR_CURRENT;
    /* tell list of change */ 
    (*SLTouchItem)(w, token);
    /* make it visible  */
    (*SLViewItem)(w, token);
    if (oldata->current != 0) {
      item = OlListItemPointer(oldata->current);
      item->attr &= ~OL_LIST_ATTR_CURRENT;
      (*SLTouchItem)(w, oldata->current);
    }
    oldata->current = token;
  }
}

/* Used to check a new value when return has been hit */
static void OlScrollingListDemoVerifyCB(w, client_data, call_data)
  Widget w;
  XtPointer client_data;
  XtPointer call_data;
{
  OlSLDemoData *oldata;
  OlTextFieldVerify *verifyfield;
  Widget list;

  if((list = OlScrollingListDataFetch(w, (char *)client_data)) == NULL) {
    return;
  }
  XtVaGetValues(list, XtNuserData, (XtPointer *)&oldata, NULL);
    
  if (oldata == NULL) {
    fprintf(stderr, "Verify found no list data.\n");
  } else {
    verifyfield = (OlTextFieldVerify *)call_data;
    fprintf(stderr, "New data is: %s, but it will be ignored.\n", 
        verifyfield->string);
    (*SLTouchItem)(oldata->this, oldata->current);
    (*SLEditClose)(oldata->this);
  }
}

/* Used when an edit starts */
static void OlScrollingListDemoEditCB(w, client_data, call_data)
  Widget w;
  XtPointer client_data;
  XtPointer call_data;
{
  OlSLDemoData *oldata;
  Widget list; 

  if((list = OlScrollingListDataFetch(w, (char *)client_data)) == NULL) {
    return;
  }
  XtVaGetValues(list, XtNuserData, (XtPointer *)&oldata, NULL);

  if (oldata == NULL) {
    fprintf(stderr, "List Demo Edit found no list data.\n");
  } else {
    (*SLEditOpen)(oldata->this, FALSE, oldata->current);
  }
}

/* Delete an item from the list */
static void OlScrollingListDemoDeleteCB(w, client_data, call_data)
  Widget w;
  caddr_t client_data;
  caddr_t call_data;
{
  OlSLDemoData *oldata;
  OlListItem *item;
  OlListDelete *delList;
  Widget list;

 /* Check to see if this was a cut and ignore */
  delList = (OlListDelete *)call_data;
  if (delList != NULL) {
    fprintf(stderr, "The delete demo doesn't cut more than one.\n");
    return;
  }
  if((list = OlScrollingListDataFetch(w, (char *)client_data)) == NULL) {
    return;
  }
  XtVaGetValues(list, XtNuserData, (XtPointer *)&oldata, NULL);

  if (oldata == NULL) {
    fprintf(stderr, "Delete found no list data.\n");
  } else {
    item = OlListItemPointer(oldata->current);
    /* Unselect */
    item->attr &= ~OL_LIST_ATTR_CURRENT;
    (*SLTouchItem)(oldata->this, oldata->current);
    (*SLDeleteItem)(oldata->this, oldata->current);
    oldata->current = 0;
  }
}

static void registerListDemoCBs (app)
  XtAppContext app;
{
  WcRegisterCallback(app, "OlScrollingListDemoSelectCB", 
		     OlScrollingListDemoSelectCB);
  WcRegisterCallback(app, "OlScrollingListDemoEditCB",
		     OlScrollingListDemoEditCB);
  WcRegisterCallback(app, "OlScrollingListDemoVerifyCB",
		     OlScrollingListDemoVerifyCB);
  WcRegisterCallback(app, "OlScrollingListDemoDeleteCB",
		     OlScrollingListDemoDeleteCB);
}
#endif /* INCLUDEDEMOS */

void XopRegisterAll();

void OriRegisterOpenLook ( app )
    XtAppContext app;
{
    XopRegisterAll(app);
}

#define RCN( name, class ) WcRegisterClassName ( app, name, class )
#define RCP( name, class ) WcRegisterClassPtr  ( app, name, class )
#define RCR( name, func )  WcRegisterConstructor(app, name, func  )

void XopRegisterAll ( app )
    XtAppContext app;
{
    ONCE_PER_XtAppContext( app );


#ifdef XtSpecificationRelease && XtSpecificationRelease > 4
    XtQInt         = XrmStringToQuark(XtRInt);
    XtQFontStruct  = XrmStringToQuark(XtRFontStruct);
    XtQBoolean     = XrmStringToQuark(XtRBoolean);
    XtQFont        = XrmStringToQuark(XtRFont);
    XtQString      = XrmStringToQuark(XtRString);
#endif
/*
    XtAddConverter       (XtRString,
                          XtRCardinal,
                          CvtStringToCardinal,
                          (XtConvertArgList)NULL,
                          (Cardinal)0);
*/
#ifdef INCLUDEDEMOS
    registerListDemoCBs(app);

    /* -- register all OpenLook widget constructors */
    RCR("ScrollingListDemo", OlScrollingListDemoCreate);
#endif /* INCLUDEDEMOS */

#ifdef TABLE
    /* -- register Table widget classes */
    RCN("Table",                        tableWidgetClass        );
    RCP("tableWidgetClass",             tableWidgetClass        );
#endif

    /* -- register all OpenLook widget classes */
    RCN("AbbrevMenu",			abbrevMenuButtonWidgetClass	);
    RCP("abbrevMenuButtonWidgetClass",	abbrevMenuButtonWidgetClass	); 
    RCN("BaseWindow",			baseWindowShellWidgetClass	);
    RCP("baseWindowShellWidgetClass",	baseWindowShellWidgetClass	);
    RCN("BulletinBoard",		bulletinBoardWidgetClass	);
    RCP("bulletinBoardWidgetClass",	bulletinBoardWidgetClass       	);
    RCN("Caption",			captionWidgetClass	        );
    RCP("captionWidgetClass",		captionWidgetClass	        );
    RCN("Checkbox",			checkBoxWidgetClass	        );
    RCP("checkBoxWidgetClass",		checkBoxWidgetClass     	);
    RCN("ControlArea",			controlAreaWidgetClass  	);
    RCP("controlAreaWidgetClass",	controlAreaWidgetClass  	);
    RCN("Exclusives",			exclusivesWidgetClass   	);
    RCP("exclusivesWidgetClass",	exclusivesWidgetClass	        );
    RCN("FlatCheckbox",			flatCheckBoxWidgetClass 	);
    RCP("flatCheckWidgetClass",		flatCheckBoxWidgetClass 	); 
    RCN("FlatExclusives",		flatExclusivesWidgetClass	);
    RCP("flatExclusivesWidgetClass",	flatExclusivesWidgetClass	);
    RCN("FlatNonexclusives",		flatNonexclusivesWidgetClass	);
    RCP("flatNonexclusivesWidgetClass",	flatNonexclusivesWidgetClass	);

    RCN("FooterPanel",			footerPanelWidgetClass  	);
    RCP("footerPanelWidgetClass",	footerPanelWidgetClass  	);
    RCN("Form",			        formWidgetClass         	);
    RCP("formWidgetClass",      	formWidgetClass         	);
    RCN("Gauge",		        gaugeWidgetClass         	);
    RCP("gaugeWidgetClass",	        gaugeWidgetClass  	        );

    /* Menus */
    RCN("Menu",		                menuShellWidgetClass	);
    RCP("menuShellWidgetClass",	        menuShellWidgetClass	); 
    RCN("Menubutton",		        menuButtonWidgetClass	);
    RCP("menuButtonWidgetClass",	menuButtonWidgetClass	); 
    RCN("Menubuttongadget",	        menuButtonGadgetClass	);
    RCP("menuButtonGadgetClass",	menuButtonGadgetClass	); 

    RCN("Nonexclusives",		nonexclusivesWidgetClass	);
    RCP("nonexclusivesWidgetClass",     nonexclusivesWidgetClass	);
    RCN("Notice",		        noticeShellWidgetClass	);
    RCP("noticeShellWidgetClass",      	noticeShellWidgetClass	); 
    RCN("Oblongbutton",		        oblongButtonWidgetClass	);
    RCP("oblongButtonWidgetClass",      oblongButtonWidgetClass	);   
    RCN("Oblongbuttongadget",	        oblongButtonGadgetClass	);
    RCP("oblongButtonGadgetClass",      oblongButtonGadgetClass	);

    RCN("Popupwindow",		        popupWindowShellWidgetClass	);
    RCP("popupWindowShellWidgetClass", 	popupWindowShellWidgetClass	); 
 
    RCN("Rectbutton",			rectButtonWidgetClass	);
    RCP("rectButtonWidgetClass",       	rectButtonWidgetClass	);
    RCN("Scrollbar",			scrollbarWidgetClass	);
    RCP("scrollbarWidgetClass",		scrollbarWidgetClass	); 
    RCN("ScrollingList",                scrollingListWidgetClass );
    RCP("scrollingListWidgetClass",     scrollingListWidgetClass );
    RCN("ScrolledWindow",               scrolledWindowWidgetClass);
    RCP("scrolledWindowWidgetClass",    scrolledWindowWidgetClass);
    RCN("Slider",			sliderWidgetClass	);
    RCP("sliderWidgetClass",		sliderWidgetClass	);
    /* Text */
    RCN("Statictext",			staticTextWidgetClass	);
    RCP("staticTextWidgetClass",	staticTextWidgetClass	);
    RCN("TextEdit",                     textEditWidgetClass     );
    RCP("textEditWidgetClass",          textEditWidgetClass     );
    RCN("Textfield",                    textFieldWidgetClass    );
    RCP("textFieldWidgetClass",         textFieldWidgetClass    );

    RCN("Stub",				stubWidgetClass		);
    RCP("stubWidgetClass",		stubWidgetClass		);
}
