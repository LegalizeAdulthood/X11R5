/*----------------------------------------------------------------------------------*
 :                           xhelp: A help facility for X                           :
 :                                   version 1.0		                    :
 :----------------------------------------------------------------------------------:
 : Copyright 1991 by Mark Newsome (Auburn University)                               :
 :                                                                                  :
 : Permission to use, copy, modify, distribute, and sell this software and its      :
 : documentation for any purpose is hereby granted without fee, provided that       :
 : the above copyright notice appear in all copies and that both that               :
 : copyright notice and this permission notice appear in supporting                 :
 : documentation, and that the name of Auburn University not be used in advertising :
 : or publicity pertaining to distribution of the software without specific,        :
 : written prior permission.  Auburn University makes no representations about the  :
 : suitability of this software for any purpose.  It is provided "as is"            :
 : without express or implied warranty.                                             :
 :                                                                                  :
 : Author:  Mark Newsome                                                            :
 : E-mail:  mnewsome@eng.auburn.edu                                                 :
 : address: Computer Science and Engineering                                        :
 :          107 Dunstan Hall                                                        :
 :          Auburn University, AL, 36849                                            :
 :----------------------------------------------------------------------------------:
 : xhelp has been tested using X11R4 on the IBM 3090, Sun Sparc and IBM RS6000.     :
 : It has been also tested using X11R5 on the RS6000.                               :
 *----------------------------------------------------------------------------------*/
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Scrollbar.h>
#include <X11/Shell.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xos.h>
#include "ScrolledList.h"
#include "fileServices.h"
extern char *path, *xhelpPath;
#define DEFAULT_HELPFILE  	"xhelp.hlp"
#define OK_STR  		"ok"
#define CANCEL_STR 		"cancel"
#define HELP_PATTERN 		"*.hlp"
#define NEW_TEXT 		"<Enter Text Here>"
#define INIT_TEXT_SOURCE 	-1

/******************************************
 **  Labels for the FILE and EDIT menus  ** 
 ******************************************/
#define FILE_OPEN_STR		"open"
#define FILE_NEW_STR		"new"
#define FILE_SAVE_STR		"save"
#define FILE_ABOUT_STR		"about"
#define EDIT_ADD_STR		"add"
#define EDIT_DELETE_STR		"delete"  
#define EDIT_REORDER_STR	"reorder"
#define EDIT_IDS_STR		"IDs"

#define QUIT_STR		"QUIT"

#define DEVELOPER_MODE 		1
#define USER_MODE		2

#define TOPIC_ID_SIZE		13
#define TOPIC_NAME_SIZE		81
#define TOPIC_DELETE		0
#define TOPIC_IDS		1
#define TOPIC_SELECT		2
#define MAX_TOPICS              50
#define MAX_PATH_LENGTH         128

/************************************** 
 **  Bitmaps for the icons           **
 **************************************/
#include "./bitmaps/xhelp.help.xbm"
#include "./bitmaps/xhelp.helpmask.xbm"
#include "./bitmaps/xhelp.exclaim.xbm"
#include "xhelp.h"
#include "./bitmaps/xhelp.index.xbm"
#include "./bitmaps/xhelp.search.xbm"
#include "./bitmaps/xhelp.forward.xbm"
#include "./bitmaps/xhelp.backward.xbm"
#define COPYRIGHT_MSG  "(C) 1991, Mark Newsome\nAuburn University"
static String indexButtonStr, searchButtonStr;

static String fallback_resources[] = {
    "",  /** used to manufacture the editType resource for asciiText 
          ** XtSetArg(wargs[n], XtNeditType, XawtextEdit); doesn't work
          ** on the asciiTextWidget???!!!  
          **/
    "*outerForm.defaultDistance:		1",
    "*topicList*font:				fixed",

    "*msgForm*background:			yellow",
    "*msgForm.Command.background:		white",
    "*msgForm.msgIcon.background:		white",

    "*promptDialog.background:			yellow",
    "*promptDialog*valueBox.value.width:	290",
    "*promptDialog*valueBox.orientation:	vertical",
    "*promptDialog*valueBox*Text*editType:	edit",
    "*promptDialog*font:                 	-*-helvetica-*-r-*-*-*-120-*-*-*-*-*-*",
    "*promptDialog*valueBox*Text*borderWidth:	2",
    "*promptDialog*valueBox*Text*background:	white",
    "*promptDialog*valueBox*value*borderColor:	black",
    "*promptDialog*valueBox*id*borderColor:	white",
    "*promptDialog*valueBox.background:		lightBlue",

    "*promptDialog*valueBox*Label.borderColor:	lightBlue",
    "*promptDialog*valueBox.Label.foreground:	black",
    "*promptDialog*valueBox.Label.background:  	lightBlue",
    "*promptDialog*valueBox*Label.width:	50",
    "*promptDialog*Text*horizDistance:        	0",

    "*promptDialog.label.foreground:     	black",
    "*promptDialog.label.justify:		left",
    "*promptDialog.label.background:     	yellow",
    "*promptDialog.label.borderColor:		yellow",
    "*promptDialog.dialogControls.background: 	steelblue",
    "*promptDialog*resizable:  			True",

    "*outerForm.defaultDistance: 		1",
    "*topicList*font:			 	fixed",
    "*Paned*Command.borderWidth:		2",
    "*Paned*Command.borderColor:		black",
    "*Paned*Command.internalWidth:		2",
    "*Paned*Command.internalHeight:		2",
    "*Paned*Label*background:			lightBlue",
    "*Paned*Label*Width:			13",
    "*Paned*Label*label:			",
    "*Paned*Label*resize:			FALSE",
    "*Paned.left:				chainLeft",
    "*Paned.right:				chainRight",
    "*Paned.top:				chainTop",
    "*Paned.bottom:				chainTop",
    "*Paned*showGrip:				FALSE",
    "*Paned.internalBorderWidth:		4",
    "*Paned.internalBorderColor:		lightBlue",
    "*Paned.borderColor:			lightBlue",
    "*Paned.borderWidth:			4",
    "*quit.accelerators:	<KeyPress>q:	set() highlight() notify()",
    "*valueBox*Label.borderColor:		lightBlue",
    "*valueBox.Label.foreground:		black",
    "*valueBox.Label.background:		lightBlue",
    "*value.horizDistance:			0",
    "*id.horizDistance:				0",
    "*textFile*font:			-*-lucidatypewriter-*-*-*-*-*-120-*-*-*-*-*-*",
    "*topicPopup*Command.width:			50",
    "*topicForm*Command.width:			50",

    /* "search" is the Text widget's search popup */
    "*search*Form.background:			yellow",
    "*search*Form.Label.background:		yellow",
    "*search*Form*Command.borderColor:		steelblue",

    "*new*promptDialog.Label.label:     	Enter NEW help filename",
    "*new*promptDialog.value: 			",
    "*save*promptDialog.Label.label:    	Save help file?",
    "*QUIT*promptDialog.Label.label:		Save help file before exiting?",
    "*QUIT*promptDialog*cancel.label:		No",
    "*add*promptDialog.Label.label:     	ADD A NEW TOPIC",
    "*add*promptDialog.value: 			",
    "*delete*topicForm*topicLabel.Label:     	Select Topic To Delete",
    "*indexButton*topicForm*topicLabel.Label: 	Select Topic To Display",
    "*topicPopup*Command.width:			50",

    "*IDs*topicForm*topicLabel.Label:		Topic IDs (for Application Developer)",
    "*IDs*topicForm*Command.Label:		OK", /** relabel cancel button **/
    "*topicForm.background:          		yellow",
    "*topicForm.Label.background:    		yellow",
    "*topicForm*Command.width:			50",

"*font:				-*-helvetica-bold-r-*-*-*-120-*-*-*-*-*-*",
"*textFile*font:	 	-*-lucidatypewriter-*-r-*-*-*-120-*-*-*-*-*-*",
"*dirForm*dirpathLabel.font:    -bitstream-charter-bold-r-normal--19-180-75-75-p-119-iso8859-1",

/*****************************************************************************	
   specifying "*dirForm*background: yellow"  would also change the dialog box
*******************************************************************************/
    "*dirForm*dirLabel.background:		yellow",
    "*dirForm.background:			yellow",
    "*dirForm*dirpathLabel.background:		yellow",
    "*dirForm*Command.vertDistance:  		40",
    "*dirForm*Command.horizDistance: 		170",
    "*dirForm*Command.borderColor:		Black",
    "*dirForm*dirpathLabel.label:    		Current directory",
    "*dirForm*dirpathLabel.width:       	400",
    "*dirForm*cwdLabel.width:        		393",
    "*dirForm*dirLabel.label:       		Select Help File",
    "*dirForm*dirLabel.width:       		400",
    "*dirForm*ScrolledList.width:		390",
    "*dirForm*ScrolledList.height:		300",
    "*dirForm*cwdLabel.justify:			right",	

    "*textFile.width:				438",
    "*textFile.height: 				300",
    "*textFile*type: 				string",
    "*textFile*scrollVertical: 			whenNeeded",
    "*textFile*scrollHorizontal: 		whenNeeded",
    "*textFile*Scrollbar.width:			10",
    "*textFile*useStringInPlace: 		False",
    "*textFile.top:				chainTop",

    "*outerForm.background:  			steelblue",
    "*menuForm.borderWidth:			0",
    "*menuForm.background:			steelblue",
    "*menuForm.orientation:			horizontal",
    "*menuForm*fileMenu*borderWidth:		3",
    "*menuForm*editMenu.label:			Topic",
    "*menuForm*editMenu*borderWidth:		3",
    "*menuForm*Command.borderColor:		lightblue",
    "*menuForn*Command.borderWidth:		1",
    "*menuForm*Command.width:			60",
    "*menuForm*MenuButton.borderColor:		lightblue",
    "*menuForm*MenuButton.width:		60",
    "*menuForm*MenuButton.borderWidth:		1",
    "*menuForm.top:				chainTop",
    "*menuForm.bottom:				chainTop",

    "*aboutLabel.label:				X Help Facility V1.0",
    "*aboutForm.Label.background:		yellow",
    "*aboutForm.background:			yellow",
    "*aboutForm.Command.background:	  	white",
    "*aboutForm.Command.borderColor:		black",	
    "*aboutForm.aboutIcon.background:		white",
    "*topicList.width:				400",
    "*topicList.height:				200",

    "*titleLabel.background:			black",
    "*titleLabel.foreground:			white",
    "*titleLabel.resize:			FALSE",
    "*titleLabel.borderWidth:			1",
    "*titleLabel.borderColor:			lightblue",

    "*titleLabel.promptDialog.label.label:	Edit Topic Name",
    "*titleLabel.top:				chainTop",
    "*titleLabel.bottom:			chainTop",

    NULL,
};

int helpmode=DEVELOPER_MODE;
Widget toplevel, outerForm, dialog, box, menuForm, iconForm,
       file, entry, fileMenu, tempW, quit,
       textFile, titleLabel, edit, editMenu, topBox;

typedef  struct {
	Widget popup;
        int action;
} pass_struct;

static XrmOptionDescRec options[] = {
{"-font",      "*AsciiText*font",       XrmoptionSepArg,        NULL}
};

struct topic_rec {  /** where our topics live **/
   char *topic;
   int    size;
   char *text;
   char *id;
} topic_rec[MAX_TOPICS]; 

int topic_count, topic_select;

char  myhelp_filename[256], 
      myhelp_path[256],
      myhelp_id[TOPIC_ID_SIZE], 
      myhelp_topicname[TOPIC_NAME_SIZE];

static void 
  Action(), CreateIconButtons(), Finish(), Quit(),  SaveBeforeQuit(),
  ChangeTitleBar(), PopupPrompt(), PopupDirectory(), PopupTopics(), 
  Activate(), PopupMsgBox(), CreateBox(), WriteHelp(), ChangeTextSource(), 
  PopupAbout(), DisplayMessage(), GetAppHelpData();

/* ARGSUSED */
/*---------------------------*/
static void SetField(new, old)
/*---------------------------*/
Widget new, old;
{
  Pixel new_border, old_border, new_bg;

  if (!XtIsSensitive(new)) {
    XBell(XtDisplay(old), 0);   /* Don't set field to an inactive Widget. */
    return;
  }

  XtVaGetValues(new, XtNborderColor, &new_border, XtNbackground, &new_bg, NULL);
  XtVaGetValues(old, XtNborderColor, &old_border, NULL);

#ifdef debug
fprintf(stderr, "SetField: new=[%s] old=[%s] parent=[%s]\n", XtName(new), XtName(old),
	XtName(XtParent(XtParent(XtParent(new))) ) );
#endif debug

  if (new_bg != new_border) return; /* selected same */
  XtVaSetValues(old, XtNborderColor, (XtArgVal) new_border, XtNdisplayCaret, FALSE, NULL);
  XtVaSetValues(new, XtNborderColor, (XtArgVal) old_border, XtNdisplayCaret, TRUE, NULL);
  XtSetKeyboardFocus(XtParent(XtParent(XtParent(new))), new);
} /* SetField() */

/* ARGSUSED */
/*--------------------------------------------*/
void TextSetField(w, event, params, num_params)
/*--------------------------------------------*/
Widget w;
XEvent *event;
String * params;
Cardinal * num_params;
{
  Widget value, id;
  Widget new, old;

  value=XtNameToWidget(toplevel, "*value");
  id=XtNameToWidget(toplevel, "*id");

  if (*num_params != 1) {
    fprintf(stderr, "*** Error: SetField Action must have",
                    "exactly one argument. ***", TRUE);
    return;

  }
  switch (params[0][0]) {
  case 'i':
  case 'I':
    old = value;
    new = id;
    break;
  case 'v':
  case 'V':
    old = id;
    new = value;
    break;
  default:
    fprintf(stderr, "*** Error: SetField Action's first Argument must",
                    "be either 'value' or 'id'. ***", TRUE);
    return;
  }
  SetField(new, old);
}

static char * MakeTopicId();
static int OpenHelpFile();
static Pixmap MakePixmapFromBitmap();

/************************************************************
 **            The FILE and EDIT structures                **
 ************************************************************/
typedef struct {
         String name;
         void   (*function)();
         XtPointer	client_data;
} editMenuStruct, fileMenuStruct;

static editMenuStruct EditMenuItems[] = {
        {EDIT_ADD_STR, 		PopupPrompt,	(XtPointer) NULL},
        {EDIT_DELETE_STR,	PopupTopics,	(XtPointer) TOPIC_DELETE}, 
/*********************************************************
        {EDIT_REORDER_STR,	PopupPrompt,	(XtPointer) NULL},
**********************************************************/ 
        {EDIT_IDS_STR, 		PopupTopics,	(XtPointer) TOPIC_IDS},
}; 

static fileMenuStruct FileMenuItems[] = {
	{FILE_OPEN_STR,		PopupDirectory,	NULL},
	{FILE_NEW_STR,		PopupPrompt,	NULL},
	{FILE_SAVE_STR,		PopupPrompt,	NULL},
	{FILE_ABOUT_STR,  	PopupAbout,	NULL},
}; 

/*------------------------------------------*/
static void beep(w, event, params, num_params)
/*------------------------------------------*/
        Widget w; XEvent *event; String *params; Cardinal num_params;
{       XBell(XtDisplay(w), 100);
} /** beep() **/

/*------------------------*/
static char * MakeTopicId()
/*------------------------*/
{  /** returns a unique id to differentiate topics from each other **/
   struct tm *today=NULL;
   struct timeval t;
   static   char * id;

   id = (char *)XtCalloc(1,TOPIC_ID_SIZE+1);   
   gettimeofday(&t, NULL);
   today=localtime(&t.tv_sec);
   sprintf(id, "%02d%02d%02d%%%02d%02d%02d", 
        today->tm_mon+1, today->tm_mday, today->tm_year,
        today->tm_hour, today->tm_min, today->tm_sec);
   return id;
} /** MakeTopicId() **/

/*----------------------------------------------------------*/
static void DestroyPopupPrompt(widget, client_data, call_data) 
/*----------------------------------------------------------*/
	Widget widget; XtPointer client_data, call_data;
{
   int i;
   String parentname, buttonname, widgetname;
   Widget textWidget;
   static char *tempstr;
   Widget popup = (Widget) client_data;
   parentname = XtName(popup);
   buttonname = XtName(widget);
   widgetname = XtName(XtParent(popup));

#ifdef debug
   fprintf(stderr,"XHelp: DestroyPopupPrompt():  parent=[%s]\n  butt(w)=%s\n parent(p(butt))=%s\n",
      parentname, buttonname,XtName(XtParent(XtParent(widget))));
#endif debug
 if (strcmp(buttonname,OK_STR)==0) {
   if ( (strcmp(parentname,FILE_SAVE_STR)==0) || (strcmp(parentname, QUIT_STR)==0) ){
      textWidget = XtNameToWidget(popup, "*value"); 
      XtVaGetValues(textWidget, XtNstring, &tempstr, NULL);
      WriteHelp(tempstr);
     }
   else 
     if (strcmp(parentname, EDIT_ADD_STR) ==0) { /* add a topic */
          String idstr;
          topic_select=topic_count++;  /* remember it goes 0..topic_count-1*/
          topic_rec[topic_select].text=(String) XtCalloc(1,strlen(NEW_TEXT)+1);
          strcpy(topic_rec[topic_select].text, NEW_TEXT); 
          topic_rec[topic_select].size=strlen(topic_rec[topic_select].text);
          textWidget = XtNameToWidget(popup, "*value"); 
          XtVaGetValues(textWidget, XtNstring, &tempstr, NULL);
          XtVaGetValues(XtNameToWidget(popup, "*id"), XtNstring, &idstr, NULL);
          if ((i=strlen(idstr))>0) {
             topic_rec[topic_select].id=(String) XtCalloc(1,14);
             strncpy(topic_rec[topic_select].id,idstr,13);
             for (;i<13;i++) topic_rec[topic_select].id[i]=' ';
          } else
             topic_rec[topic_select].id=MakeTopicId();
#ifdef debug 
     fprintf(stderr, "the string is [%s] id=[%s] %d\n", tempstr, topic_rec[topic_select].id, 
		strlen(topic_rec[topic_select].id));
#endif debug
          topic_rec[topic_select].topic= XtNewString(tempstr);
          ChangeTextSource(topic_select);
       } /* add */
     else
       if (strcmp(parentname, FILE_NEW_STR)==0) { /* create new help file */
          ChangeTextSource(INIT_TEXT_SOURCE);
          topic_count=1;
          topic_select=0;  /* remember it goes 0..topic_count-1*/
          topic_rec[topic_select].text=(String) XtCalloc(1,strlen(NEW_TEXT)+1);
          strcpy(topic_rec[topic_select].text, NEW_TEXT);
          topic_rec[topic_select].topic= XtNewString("None"); 
          topic_rec[topic_select].id=XtNewString(XHELP_INTRO_ID);
          topic_rec[topic_select].size=strlen(topic_rec[topic_select].text);
          textWidget = XtNameToWidget(popup, "*value"); 
          XtVaGetValues(textWidget, XtNstring, &tempstr, NULL);
          strcpy(myhelp_filename, tempstr);
/***********************************************************************/
/****  add checks to see if file already exists, or invalid fileanme ***/
/***********************************************************************/
          ChangeTitleBar(myhelp_filename);
          ChangeTextSource(topic_select);
          XtSetSensitive(XtNameToWidget(toplevel, "*save"), TRUE);
       } /* new */
        else
            if (strcmp(parentname, "titleLabel")==0) { /* edit TOPIC NAME */
                textWidget = XtNameToWidget(popup, "*value"); 
                XtVaGetValues(textWidget, XtNstring, &tempstr, NULL);
                topic_rec[topic_select].topic=XtNewString(tempstr);
                ChangeTextSource(topic_select); 
            } /** edit **/
#ifdef debug
            else
               if (strcmp(parentname, EDIT_IDS_STR)==0) { /** print **/
   fprintf(stderr, "XHelp: printing topicIds\n");
   for (i=0;i<topic_count; i++){
      fprintf(stderr,"XHelp: topicId=[%s]\t\ttopic name=[%s]\n",
          topic_rec[i].id, topic_rec[i].topic);
                  } /** for **/
               } /** print **/
#endif debug
   } /* if OK */
   XtDestroyWidget(popup);
   if (strcmp(parentname,QUIT_STR)==0) Quit(toplevel, NULL, NULL);
} /*** DestroyPopupPrompt() ***/

/*------------------------------------------------------*/
static void PopupMsgBox(button, client_data, call_data)
/*------------------------------------------------------*/
        Widget  button; XtPointer client_data, call_data;
{
    Widget      msgPopup, msgForm, msgLabel;
    Widget      okButton, msgIcon ;
    Position    x, y;
    Dimension   width, height;
    int i,j;
    Display *dsp = XtDisplay(toplevel);
    int scn = DefaultScreen(dsp);
    Pixmap  iconpixmap;

    XtVaGetValues(button, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(button, (Position)(width / 2),(Position)(height / 2),&x,&y);

    msgPopup = XtVaCreatePopupShell("msgPopup", transientShellWidgetClass,
               toplevel,  XtNx, x, XtNy, y, NULL );

    msgForm = XtCreateManagedWidget("msgForm",formWidgetClass,msgPopup,NULL,0);

    iconpixmap=MakePixmapFromBitmap(exclaim_bits, exclaim_width, exclaim_height);

    msgIcon=XtVaCreateManagedWidget("msgIcon", labelWidgetClass, msgForm,
                XtNfromHoriz, NULL, XtNfromVert, NULL,
                XtNbitmap, iconpixmap, XtNborderWidth, 2, NULL);

    msgLabel=XtVaCreateManagedWidget("msgLabel", labelWidgetClass, msgForm,
                XtNfromHoriz, msgIcon, XtNfromVert, NULL,
                XtNlabel, (char *) call_data, XtNborderWidth, 0, NULL);

    okButton = XtVaCreateManagedWidget(OK_STR, commandWidgetClass, msgForm,
                XtNfromVert, msgIcon, XtNwidth, exclaim_width+8, XtNborderWidth, 2,NULL);

    XtAddCallback(okButton, XtNcallback, DestroyPopupPrompt, (XtPointer)msgPopup);
    XtPopup(msgPopup,  XtGrabExclusive /* XtGrabNone*/);
} /** PopupMsgBox() **/

/*--------------------------------------------*/
void ReturnPressed(w, event, params, num_params)
/*--------------------------------------------*/
        Widget w; XEvent *event; String *params; Cardinal num_params;
{
#ifdef debug
   fprintf(stderr, "returnPressed:  ok=[%s]  return value=%d\n",
      XtName(XtNameToWidget(XtParent(XtParent(w)), "*ok")));
#endif debug
      XtCallCallbacks(XtNameToWidget(XtParent(XtParent(w)), "*ok"), XtNcallback, NULL);
} /** ReturnPressed() **/

/*---------------------------------------------------*/
static void PopupPrompt(button, client_data, call_data)
/*---------------------------------------------------*/
        Widget button; XtPointer client_data, call_data;
{
   XtTranslations value_trans_table, id_trans_table;
   Widget popup, promptDialog,  valueBox, value, valueLabel, id, idLabel,
          controlsBox, controlsOk, controlsCancel, label ;
   Position x,y;
   String buttonName;
   Dimension width, height;
   int i;
   static char tempstr[128], labelstr[30];
   static XtActionsRec myActionsTable[]= {
	{"pressedReturn" ,ReturnPressed},
	{"textSetField",TextSetField}, 
   };

   static char defaultTranslations[]=
       "Ctrl<Key>J:	pressedReturn( ctrlJ ) \n\
	Ctrl<Key>O:	pressedReturn( ctrlO ) \n\
	Ctrl<Key>M:	pressedReturn( ctrlM ) \n\
  	<Key>Return:	pressedReturn( Return )";

   static char valueTranslations[]=
   	"<Key>Return:	textSetField(id) \n\
	<Btn1Down>:     textSetField(value) \n\
   	<Key>Tab:       textSetField(id)";

   static char idTranslations[] =
   	"<Key>Return:   textSetField(value) \n\
   	<Btn1Down>:     textSetField(id) \n\
   	<Key>Tab:       textSetField(value)";

   buttonName=XtName(button);
   XtVaGetValues(button, XtNwidth, &width, XtNheight, &height,NULL);
   XtTranslateCoords(button, (Position) (width / 2), (Position) (height+10), &x, &y);

/*
   fprintf(stderr, "PopupPrompt: button=[%s] x=%d  x=%d\n", buttonName, x,y);
*/
   popup = XtVaCreatePopupShell(XtName(button), transientShellWidgetClass,toplevel,
               	XtNx,x, XtNy, y, NULL);

   promptDialog=XtVaCreateManagedWidget("promptDialog", formWidgetClass, popup, NULL);

   label=       XtVaCreateManagedWidget("label", labelWidgetClass, promptDialog, NULL);

   valueBox=    XtVaCreateManagedWidget("valueBox", formWidgetClass,
		promptDialog, XtNfromVert, label,NULL);

   if ((strcmp(buttonName, FILE_SAVE_STR)==0)
 	 || (strcmp(buttonName, QUIT_STR)==0)
	 || (strcmp(buttonName, FILE_NEW_STR)==0)) 
       strcpy(labelstr, "Filename:");
   else 
       strcpy(labelstr, "Topic Header:");
   if (strcmp(buttonName, FILE_SAVE_STR)==0 || strcmp(buttonName, QUIT_STR)==0) {
      sprintf(tempstr, "%s/%s", myhelp_path,myhelp_filename);
   } else
      tempstr[0]='\0';
  
   valueLabel	= XtVaCreateManagedWidget("valueLabel", labelWidgetClass, valueBox, 
			XtNlabel, labelstr, XtNwidth, 97, NULL);
   value	= XtVaCreateManagedWidget("value",asciiTextWidgetClass,valueBox,
			XtNstring,tempstr,XtNfromHoriz, valueLabel,
			XtNwidth, 290, XtNdisplayCaret, TRUE, NULL);

   if (strcmp(XtName(button), EDIT_ADD_STR)==0) {
      idLabel	=XtVaCreateManagedWidget("idLabel", labelWidgetClass, valueBox, 
   			XtNfromVert, valueLabel, XtNlabel, "ID (13 chars):",
			XtNwidth, 97, NULL);
      id   	=XtVaCreateManagedWidget("id", asciiTextWidgetClass, valueBox, 
			XtNfromVert, valueLabel, XtNfromHoriz, idLabel,  XtNstring,"", 
			XtNdisplayCaret, FALSE, NULL);
      id_trans_table = XtParseTranslationTable(idTranslations);
      XtOverrideTranslations(id, id_trans_table);
      value_trans_table = XtParseTranslationTable(valueTranslations);
      XtOverrideTranslations(value, value_trans_table);
   } else {
      value_trans_table = XtParseTranslationTable(defaultTranslations);
      XtOverrideTranslations(value, value_trans_table);
   }
   XtSetKeyboardFocus(popup, value);
   XtAppAddActions(XtWidgetToApplicationContext(button),myActionsTable, XtNumber(myActionsTable));

   controlsBox= XtVaCreateManagedWidget("dialogControls",boxWidgetClass,promptDialog,
		XtNfromVert, label, XtNfromHoriz, valueBox, NULL);

   controlsOk=  XtVaCreateManagedWidget(OK_STR, commandWidgetClass,controlsBox,
		XtNwidth, 77, NULL);
   XtAddCallback(controlsOk, XtNcallback, DestroyPopupPrompt, (XtPointer)popup);

   controlsCancel=XtVaCreateManagedWidget(CANCEL_STR, commandWidgetClass,
		controlsBox, XtNwidth, 77, NULL);
   XtAddCallback(controlsCancel, XtNcallback, DestroyPopupPrompt, (XtPointer) popup);
   XtPopup(popup, XtGrabExclusive);
} /** PopupPrompt() **/

/*------------------------------------------------------*/
static void PopupDirectory(button, client_data, call_data)
/*------------------------------------------------------*/
	Widget  button; XtPointer client_data, call_data;
{
    Widget      dirPopup, dirList, dirForm, dirLabel, dirViewPort;
    Widget      dircancelButton, cwdLabel, dirpathLabel;
    Position    x, y;
    Dimension   width, height;
    int i,j;
    static pass_struct pass_rec;

    XtVaGetValues(button, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(button), (Position) (width / 2), (Position) (height / 2), &x, &y);

    dirPopup = XtVaCreatePopupShell("dirPopup", transientShellWidgetClass,
               	XtParent(button),  XtNx, x, XtNy, y, NULL);
    
    dirForm = XtVaCreateManagedWidget("dirForm", formWidgetClass, dirPopup, NULL); 

    dirLabel = XtVaCreateManagedWidget("dirLabel", labelWidgetClass,
          	dirForm, XtNborderWidth, 0, NULL);

    dirpathLabel=XtVaCreateManagedWidget("dirpathLabel", labelWidgetClass, dirForm, 
		XtNfromVert, dirLabel, XtNvertDistance, 50, XtNborderWidth,0, NULL); 

    cwdLabel=XtVaCreateManagedWidget("cwdLabel", labelWidgetClass, dirForm, 
		XtNfromVert, dirpathLabel, XtNlabel, path, XtNborderWidth,1, NULL);

    /**********************  The directory list **********************/
    dirList = XtVaCreateManagedWidget("dirList", scrolledListWidgetClass,
                dirForm, XtNfromVert, cwdLabel, XtNfromHoriz, NULL, 
		XtNlist, Getdir(path,HELP_PATTERN), NULL);
    pass_rec.popup = dirPopup;
    pass_rec.action= 0;  /** unused **/  
    XtAddCallback(dirList, XtNcallback, Activate, (XtPointer) &pass_rec);
    dircancelButton = XtVaCreateManagedWidget(CANCEL_STR, commandWidgetClass, dirForm, NULL);
    XtAddCallback(dircancelButton,XtNcallback,DestroyPopupPrompt,(XtPointer) dirPopup);
    /*****************************************************************/
    XtPopup(dirPopup,  XtGrabExclusive /* XtGrabNone*/);
} /* PopupDirectory */

/*---------------------------*/
static void CreateBox(parent)
/*---------------------------*/
	Widget parent;
{   Widget box, scrollbar, label;
    box = XtVaCreateManagedWidget(NULL, boxWidgetClass, parent, NULL);
    scrollbar= XtVaCreateManagedWidget("scrollbar", scrollbarWidgetClass, box, NULL);
    label= XtVaCreateManagedWidget("scroll_label", labelWidgetClass, box, NULL);
} /*** CreateBox() ***/

/*------------------------------*/
void static DisplayMessage(w,msg)
/*------------------------------*/
  	Widget w;
	String msg;
{
  fprintf(stderr,"XHelp: Message=%s\n", msg);
  PopupMsgBox(w, NULL, msg);
} /*** displayMessage() ***/

/*----------------------------------------------------*/
static void PopupTopics(button, topic_action, call_data)
/*----------------------------------------------------*/
	Widget button; XtPointer topic_action, call_data;
{
    Widget      topicPopup, topicList, topicForm, topicViewPort, cancelButton, topicLabel;
    Position    x, y;
    Dimension   width, height;
    static 	String topics[MAX_TOPICS]/* =NULL */ ; 
    Widget tempW;
    static pass_struct pass_rec;

    /****** must be static, list widget needs access to them after  ****** 
     ****** this function terminates....                            ******/
    int i,j;
    XtVaGetValues(button, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(button), (Position) (width / 2), (Position)
              (height / 2), &x, &y);
/*------------------------------------------------------------------------------------*
 : Since we cannot plant the popup on a menuObject (Gadget!),  we'll stick it on      :
 : toplevel, but we'll give the popup the name of the button so our resources can be  :
 : specified by the menu selection which called it .                                  :
 *------------------------------------------------------------------------------------*/
    topicPopup = XtVaCreatePopupShell(XtName(button), transientShellWidgetClass, toplevel,
		XtNx, x, XtNy, y, NULL);

    topicForm = XtVaCreateManagedWidget("topicForm", formWidgetClass, topicPopup, NULL);

    topicLabel = XtVaCreateManagedWidget("topicLabel", labelWidgetClass,
          	topicForm, XtNborderWidth,0,NULL);

    cancelButton = XtVaCreateManagedWidget(CANCEL_STR,commandWidgetClass,topicForm,
		XtNfromHoriz, NULL, XtNfromVert, NULL, XtNhorizDistance, 355, NULL);

    XtAddCallback(cancelButton, XtNcallback, DestroyPopupPrompt,
              /* CLIENT DATA --->*/ (XtPointer) /*topicForm*/ topicPopup);
    /**********************  The topic list **********************/
    topicViewPort=XtVaCreateManagedWidget("topicViewPort", viewportWidgetClass,
            topicForm, XtNfromVert, cancelButton,NULL);
#ifdef debug
    fprintf(stderr,"XHelp: PopupTopics() topic parent= %s  %d\n", 
         XtName(XtParent(button)), (int)topic_action);
#endif debug

    for (i=0;i<topic_count;i++){  /** build the list **/
         if (topics[i]==NULL)  
            topics[i]=(String) XtCalloc(1,50);
         if (((int)topic_action) == TOPIC_IDS)
            sprintf(topics[i],"%12s  %-30s",topic_rec[i].id,topic_rec[i].topic);
         else 
            sprintf(topics[i], "%-30s", topic_rec[i].topic);
    }
    if (topics[i] != NULL) XtFree(topics[i]);
    topics[i]=(String)NULL; /* end list here: */

    topicList = XtVaCreateManagedWidget("topicList", scrolledListWidgetClass,
                topicViewPort, XtNlist, topics, NULL);

    pass_rec.action= (int)topic_action;
    pass_rec.popup = topicPopup;
    XtAddCallback(topicList, XtNcallback, Activate, (XtPointer) &pass_rec);
    XtPopup(topicPopup,  XtGrabExclusive /* XtGrabNone*/);
} /** PopupTopics **/

/*----------------------------------------------------*/
static Pixmap MakePixmapFromBitmap(bits, width, height)
/*----------------------------------------------------*/
        char bits[];
        int width, height;
{
   Display *dsp = XtDisplay(toplevel);
   int scn= DefaultScreen(dsp);

   return( XCreatePixmapFromBitmapData(
             dsp,
             DefaultRootWindow(dsp),
             bits,
             width,
             height,
             BlackPixel(dsp, scn),
             WhitePixel(dsp, scn),
             DefaultDepth(dsp,scn)));
} /** MakePixmapFromBitmap() **/

/*---------------------------------------------------*/
static void PopupAbout(button, client_data, call_data)
/*---------------------------------------------------*/
        Widget  button;
        XtPointer client_data, call_data;
{
    Widget      aboutPopup, aboutForm, aboutLabel;
    Widget      okButton, aboutIcon, authorLabel;
    Position    x, y;
    Dimension   width, height;
    int i,j;
    Display *dsp = XtDisplay(toplevel);
    int scn = DefaultScreen(dsp);
    Pixmap  iconpixmap;

    XtVaGetValues(button, XtNwidth, &width, XtNheight, &height, NULL);
    XtTranslateCoords(XtParent(button), (Position) (width / 2), (Position)
              (height / 2), &x, &y);

    aboutPopup = XtVaCreatePopupShell("aboutPopup", transientShellWidgetClass,
               XtParent(button),  XtNx, x, XtNy, y, NULL );

    aboutForm = XtCreateManagedWidget("aboutForm", formWidgetClass,
                aboutPopup, NULL, 0);

    /****************** define program icon ******************/
    iconpixmap=MakePixmapFromBitmap(help_bits, help_width, help_height);

    aboutIcon=XtVaCreateManagedWidget("aboutIcon", labelWidgetClass, aboutForm, 
 		XtNfromHoriz, NULL, XtNfromVert, NULL,
		XtNbitmap, iconpixmap, XtNborderWidth, 2, NULL);
  
    aboutLabel=XtVaCreateManagedWidget("aboutLabel", labelWidgetClass, aboutForm, 
		XtNfromHoriz, aboutIcon, XtNfromVert, NULL, 
 		XtNborderWidth, 0, NULL);

    authorLabel=XtVaCreateManagedWidget("authorLabel", labelWidgetClass, aboutForm,
		XtNfromHoriz, aboutIcon, XtNfromVert, aboutLabel,
		XtNborderWidth, 0, XtNlabel, COPYRIGHT_MSG, NULL);
      
    okButton = XtVaCreateManagedWidget(OK_STR, commandWidgetClass, aboutForm, 
		XtNfromVert, aboutIcon, XtNwidth, help_width+8, XtNborderWidth, 2,NULL);

    XtAddCallback(okButton, XtNcallback, DestroyPopupPrompt, (XtPointer)aboutPopup);
    XtPopup(aboutPopup,  XtGrabExclusive /* XtGrabNone*/);
} /** PopupAbout() **/

/*--------------------------------*/
static void ChangeTitleBar(filename)
/*--------------------------------*/
	String filename;
{
   char  tempstr[80];
   sprintf(tempstr, "XHELP: %s",filename);
   XtVaSetValues(toplevel,XtNtitle, tempstr,NULL);
} /* ChangeTitleBar */ 

/*------------------------------------------*/
static int FindTopicIndex(topicId, topicName) 
/*------------------------------------------*/
	char *topicId, *topicName;
{
   int i;
   for (i=0; i<topic_count; i++) {
      if (strcmp(topic_rec[i].id, topicId)==0) {
          return i; 
      } /** if **/
   } /** for **/
   return -1;  /** means we didn't find it **/
} /** FindTopicIndex() **/ 

/*-------------------------------------------------------------*/
static int OpenHelp(searchpathname,filename, topicId, topicName)
/*-------------------------------------------------------------*/
	char *searchpathname, *filename, *topicId, *topicName;
{  FILE * infile=NULL;
   char tempstr[MAX_PATH_LENGTH], pathname[MAX_PATH_LENGTH],  
          myfilename[MAX_PATH_LENGTH];
   int i,j;
   Widget  displayW;
   static char currentfilename[MAX_PATH_LENGTH]="";
   static char wherefound[MAX_PATH_LENGTH]="";

#ifdef debug
fprintf(stderr, "XHelp: OpenHelp() [%s]   [%s]\n", filename, currentfilename);	    
#endif debug

  if ( (!filename) || (!currentfilename) || (strcmp(filename,currentfilename)!=0)) { 
      topic_select=INIT_TEXT_SOURCE;
      ChangeTextSource(topic_select);
  
      infile=SearchAndOpen(searchpathname, filename, wherefound);
      if (infile !=NULL) { 
         strcpy(currentfilename, filename);  /** save filename **/ 
#ifdef debug
fprintf(stderr, "XHelp: OpenHelp() both=[%s/%s] \n",wherefound, filename);
#endif debug
         strcpy(myhelp_filename, filename);
         strcpy(myhelp_path, wherefound);
         /** myhelp_filename is our global var for saving the path/filename we opened **/ 
         fscanf(infile,"%d", &topic_count); 	/* gobble count and \n */
         fgetc(infile);

         for(j=0;j<topic_count;j++){
             topic_rec[j].id = (String) XtCalloc(1,TOPIC_ID_SIZE+1);
             for (i=0;i<TOPIC_ID_SIZE; i++) topic_rec[j].id[i]=fgetc(infile);
             topic_rec[j].topic = (String) XtCalloc(1,TOPIC_NAME_SIZE+1);
             for (i=0; (topic_rec[j].topic[i]=fgetc(infile)) !='\n' ;i++);
             topic_rec[j].topic[i]='\0';

             fscanf(infile,"%d", &topic_rec[j].size);
             fgetc(infile);
             topic_rec[j].text = (String)XtCalloc(1, topic_rec[j].size+1);
             if (topic_rec[j].text == NULL) {
                 sprintf(tempstr,"XHelp: OpenHelp() could not allocate memory");
                 DisplayMessage(toplevel, tempstr);
                 exit(-1);         
             }
             for (i=0; i<topic_rec[j].size; i++) 
                 topic_rec[j].text[i] = fgetc(infile);
             topic_rec[j].text[topic_rec[j].size]='\0';
         } /* for */
         fclose(infile);
         ChangeTitleBar(filename);
         if (helpmode==DEVELOPER_MODE) /* it doesn't exist in USER mode */
            XtSetSensitive(XtNameToWidget(toplevel, "*save"), TRUE); /* turn on save */
      } /** if fopen **/
      else {
          sprintf(tempstr, "XHelp: Could not open [%s]",filename);
          if (helpmode==DEVELOPER_MODE) /* it doesn't exist in USER mode */
             XtSetSensitive(XtNameToWidget(toplevel, "*save"), FALSE); /* whoops, turn off save */
          DisplayMessage(toplevel,tempstr);
          return 0;
       } /** else **/
  }/** if filename != currentfilename **/

  if ((topic_select=FindTopicIndex(topicId,topicName)) == -1) {
      sprintf(tempstr, "XHelp: TopicId [%s] not found.\nTopicName=[%s]\nFilename=[%s]\n\nConsult Application developer.\n",
		topicId, topicName, filename);  
      DisplayMessage(toplevel,tempstr);
      topic_select = 0; /** default to welcome, which is the first topic */
  } 
  ChangeTextSource(topic_select);  
  return 1;
} /*** OpenHelp() ***/

/*----------------------------*/
static void WriteHelp(filename)
/*----------------------------*/
	String filename;
{  FILE * outfile;
   int j;
   Widget writeHelp;
   static  char tempstr[MAX_PATH_LENGTH+50];  /** extra to allow for msg below **/

   ChangeTextSource(topic_select); /* to get text out of widget */
   if ((outfile=fopen(filename,"w"))==NULL) {
        sprintf(tempstr,"Could not write to [%s]\nFile Not Saved!", filename);
        DisplayMessage(toplevel,tempstr); 
        return;
      };
#ifdef debug
   fprintf(stderr, "XHelp: WriteHelp()  filename=%s\n", filename);
#endif debug
   fprintf(outfile, "%d\n",topic_count);
   for (j=0;j<topic_count;j++)
         fprintf(outfile, "%s%s\n%d\n%s", 
             topic_rec[j].id,
             topic_rec[j].topic,
             strlen(topic_rec[j].text),
             topic_rec[j].text);
   fclose(outfile);
} /*** WriteHelp() ***/

/*------------------------*/
static void GetAppHelpData()
/*------------------------*/
{
    int         type, format, nitems, left;
    char        *retdata, *i, *j;

   /*** Hooks to allow apps to call us ***/
   if (XGetWindowProperty(XtDisplay(toplevel), 
    		DefaultRootWindow(XtDisplay(toplevel)), 
		XHELP_FILE, 0L, (long) BUFSIZ,
		FALSE, XA_STRING,
		&type, &format, &nitems, &left, &retdata) == Success &&
	            type == XA_STRING) {
#ifdef debug
        fprintf(stderr, "XHelp: GetAppHelpData() type=%d,  format=%d,   nitems=%d   left=%d\n",
            type, format, nitems, left);
        fprintf(stderr,"XHelp: -->%s\n",(char*) retdata);
#endif debug
        i=(char *)strchr(retdata,':');
        j=(char *)strrchr(retdata,':');
        *i='\0';*j='\0';

        strcpy(myhelp_filename, retdata);
        strcpy(myhelp_id, (i+1)); 
        strcpy(myhelp_topicname, (j+1));

        /* if a proper topic_id is not provided by the app, then default to XHELP_INTRO_ID */ 
        if (j-i-1 !=13)  strcpy(myhelp_id, XHELP_INTRO_ID);
  
#ifdef debug
        fprintf(stderr,"XHelp: GetAppHelpData() filename=%s\n",myhelp_filename);
        fprintf(stderr,"XHelp: GetAppHelpdata() topicId=%s\n",myhelp_id);
        fprintf(stderr,"XHelp: GetAppHelpData() topicName=%s\n",myhelp_topicname);
#endif debug
        OpenHelp(xhelpPath,myhelp_filename, myhelp_id, myhelp_topicname);
   } /*** if ***/ 
} /*** GetAppHelpData() ***/

/*--------------------------------------------*/
static void Activate(w, client_data, call_data)
/*--------------------------------------------*/
	Widget w;
	XtPointer client_data, call_data;

/**** call_data contains the item selection from the scrolledList widget ****/
/**** client_data contains the popup widget ***/
{
    Widget tempW;
    float top;
    int i,n;
    int nitems=0, longest=0; /* 0 means let list widget compute these */
    Boolean resize=TRUE;
    Arg args[5];
    pass_struct pass_rec;

    ScrolledListReturnStruct *item = (ScrolledListReturnStruct*)call_data;
    /**** The ScrolledList widget so graciously deposits our selection into call_data ****/

    pass_rec = *((pass_struct *) client_data);
/****
    fprintf(stderr, "Activate: action= %d\n", pass_rec.action);
    fprintf(stderr, "Activate: popup=%s\n", XtName(pass_rec.popup));
*****/

    if (item->index == -1) return;  /** blank area of list selected! **/
    i=strlen(item->string)-1;
    tempW=XtParent(XtParent(w));

#ifdef debug
    fprintf(stderr, "XHelp: Activate() widget=[%s]  call=[%d]  client=[%d]\n", 
               XtName(w),(int) call_data, (int) client_data );
    fprintf(stderr, "XHelp: Activate() Parent(p(w))=[%s]  P(tempW)=[%s]\n",XtName(XtParent(XtParent(w))),
               XtName(XtParent(tempW)));
#endif debug

    if (strcmp(XtName(w), "dirList")==0) {
       if (item->string[i]=='/') {
          cd(item->string);  /** change directory ***/  
#ifdef debug
fprintf(stderr, "Activate: path=%s, help_pattern=%s\n",path, HELP_PATTERN);
#endif debug

          ScrolledListSetList(w, Getdir(path, HELP_PATTERN),nitems,resize);
          /** update path label **/
          XtVaSetValues(XtNameToWidget(XtParent(w), "*cwdLabel"), XtNlabel, path, NULL); 
       } /** if **/
       else { /*** file selected, get rid of directory popup ***/
          strcpy(myhelp_filename,(item->string)); 
          OpenHelp(path,myhelp_filename, XHELP_INTRO_ID,""); 
          DestroyPopupPrompt(w,(XtPointer)pass_rec.popup,(XtPointer)call_data); 
       } /** else **/
    } /** if dirList **/ 
    else { /** topicList **/
       topic_select = item->/**list_**/index;
       if (pass_rec.action == TOPIC_DELETE) {
           if (topic_count>1) {
               for (i=topic_select; i<topic_count-1; i++) {
                    topic_rec[i].topic=topic_rec[i+1].topic;
                    topic_rec[i].size=topic_rec[i+1].size;
                    topic_rec[i].text=topic_rec[i+1].text;
                    topic_rec[i].id=topic_rec[i+1].id;
               } /** for **/
               topic_select=0;
               topic_count--;
           } /** if **/    
           else {
              topic_select=0;  /** we'll always keep one topic for the "welcome" topic **/
              topic_rec[topic_select].text=(String) XtCalloc(1,strlen(NEW_TEXT)+1);
              topic_rec[topic_select].topic= XtNewString("None");
              topic_rec[topic_select].id= XtNewString(XHELP_INTRO_ID);
              topic_rec[topic_select].size=strlen(topic_rec[topic_select].text);
              strcpy(topic_rec[topic_select].text, NEW_TEXT);
           } /** else **/ 
#ifdef debug
       fprintf(stderr, "XHelp: Activate() deleting=[%s]\n", topic_rec[topic_select].topic);
#endif debug
       } /** if strcmp **/
        ChangeTextSource(topic_select);
        DestroyPopupPrompt(w, /**CLIENT DATA-->**//** tempW,**/
               (XtPointer) pass_rec.popup,  /** CALL DATA --> **/(XtPointer)call_data);
    } /** else **/
#ifdef debug
#undef debug
#endif debug
}  /*** Activate() ***/

/*----------------------*/
static void PopupSearch()
/*----------------------*/
{  /** call the text widget's search function **/	
   char *tmp="forward";
   XtCallActionProc(textFile, "search", NULL, &tmp, 1);
} /*** PopupSearch() ***/

/*----------------------------------*/
static void CreateIconButtons(parent)
/*----------------------------------*/
	Widget parent;
{   Widget button;
    XtVaCreateManagedWidget("filler1", labelWidgetClass, parent, NULL);
    button=XtVaCreateManagedWidget("indexButton", commandWidgetClass, parent,
        XtNbitmap, XCreateBitmapFromData (XtDisplay(toplevel), XtScreen(toplevel)->root,
        index_bits, index_width, index_height), XtNskipAdjust, TRUE, NULL); 
    XtAddCallback(button, XtNcallback, PopupTopics, (XtPointer)TOPIC_SELECT );

    button=XtVaCreateManagedWidget("backwardButton", commandWidgetClass, parent,
        XtNbitmap, XCreateBitmapFromData (XtDisplay(toplevel), XtScreen(toplevel)->root,
        backward_bits, backward_width, backward_height), XtNskipAdjust, TRUE, NULL);
    XtAddCallback(button, XtNcallback, Action, (XtPointer) button );

    button=XtVaCreateManagedWidget("forwardButton", commandWidgetClass, parent,
        XtNbitmap, XCreateBitmapFromData (XtDisplay(toplevel), XtScreen(toplevel)->root,
        forward_bits, forward_width, forward_height), XtNskipAdjust, TRUE, NULL);
    XtAddCallback( button, XtNcallback, Action, (XtPointer) button );

    button=XtVaCreateManagedWidget("searchButton", commandWidgetClass, parent,
        XtNbitmap, XCreateBitmapFromData (XtDisplay(toplevel), XtScreen(toplevel)->root,
        search_bits, search_width, search_height), XtNskipAdjust, TRUE, NULL);
    XtVaCreateManagedWidget("filler2", labelWidgetClass, parent, NULL);
    XtAddCallback(button, XtNcallback, PopupSearch, NULL); 
} /*** CreateIconButtons() ***/

/****************************************/
static void ChangeTextSource(topic_select)
/****************************************/
	int topic_select;
{
   static int prev_topic_select=INIT_TEXT_SOURCE;
   Arg          wargs[5];
   int n, modifiedsize;
   String modifiedtext;
   Widget old = XawTextGetSource(textFile);
#ifdef debug
   fprintf(stderr,"XHelp: ChangeTextSource() ChangeText: prev=%d  new=%d\n", prev_topic_select, topic_select);
#endif debug
   if ( ((topic_select != INIT_TEXT_SOURCE) && (topic_select <0)) 
      || (topic_select>topic_count-1) ) {
          DisplayMessage(toplevel, "Invalid topic selected\n");
          return;
   }
   if (prev_topic_select != INIT_TEXT_SOURCE ) {
      n=0;
      XtSetArg(wargs[n], XtNstring, &modifiedtext); n++;
      XtGetValues(old, wargs, n);    
     modifiedsize=strlen(modifiedtext);
      if (modifiedsize>topic_rec[prev_topic_select].size)  {
         XtFree(topic_rec[prev_topic_select].text);
         topic_rec[prev_topic_select].text= (String) XtMalloc(modifiedsize+1);
      }
      strcpy(topic_rec[prev_topic_select].text, modifiedtext);
      topic_rec[prev_topic_select].size=modifiedsize;
#ifdef debug
      fprintf(stderr,"ChangeText: topic=%s\n",topic_rec[prev_topic_select].topic);
      fprintf(stderr,"ChangeText:  topic_count=%d\n",topic_count);
#endif debug
   } /* prev_topic_select */

   prev_topic_select=topic_select;

   if (topic_select == INIT_TEXT_SOURCE) {
      n=0;
      XtSetArg(wargs[n], XtNlabel, "NONE");n++;
      XtSetValues(titleLabel, wargs, n);

      n=0;
      XtSetArg(wargs[n], XtNstring, ""); n++;
      XtSetArg(wargs[n], XtNlength, 0); n++;
      XtSetArg(wargs[n], XtNtype, XawAsciiString); n++; }
   else {
      n=0;
      XtSetArg(wargs[n], XtNlabel, topic_rec[topic_select].topic);n++;
      XtSetValues(titleLabel, wargs, n);

      n=0;
      XtSetArg(wargs[n],XtNstring, topic_rec[topic_select].text); n++;
      XtSetArg(wargs[n],XtNlength, topic_rec[topic_select].size); n++;
      XtSetArg(wargs[n],XtNtype, XawAsciiString); n++;
    }

   XawTextSetSource(textFile,
                   XtCreateWidget("textFile",  asciiSrcObjectClass,
                   textFile, wargs, n), (XawTextPosition)0);

   XtDestroyWidget(old);
} /*** ChangeTextSource() ***/

/*******************************************/
static void Action(widget, closure, callData)
/*******************************************/
        Widget widget;
        XtPointer closure, callData;
{
   Arg wargs[5];
   int i,n;
   String name;

   name = XtName(widget);
#ifdef debug
   fprintf(stderr,"action name=%s\n", name);
#endif debug
     if (strcmp(name, "backwardButton")==0) {
        if (topic_select>0) topic_select--;
     } /* if */
     else  /* forwardButton */
         if (topic_select < (topic_count-1)) topic_select ++;

#ifdef debug
   fprintf(stderr,"Action:%d Selected topic=[%s]\n",topic_select,
            topic_rec[topic_select].topic);
#endif debug
   ChangeTextSource(topic_select);
} /*** Action() ***/

/*--------------------------------------------*/
static void Quit(widget, client_data, call_data)
/*--------------------------------------------*/
   	Widget widget; XtPointer client_data, call_data;
{
   fprintf(stderr, "End XHelp.\n");
   XtDestroyWidget(toplevel);
}

/*-------------------------------------------------------*/
static void SaveBeforeQuit(widget, client_data, call_data)
/*-------------------------------------------------------*/
   	Widget widget; XtPointer client_data, call_data;
{ /** Do we ask to save before quitting? **/
   if (topic_count && (helpmode==DEVELOPER_MODE))  
      PopupPrompt(widget, (XtPointer) NULL, (XtPointer) NULL);
   else 
      Quit(widget, (XtPointer) NULL, (XtPointer) NULL);
} /* SaveBeforeQuit() */

/*--------------------------------------------*/
static void Finish(widget, closure, callData)
/*--------------------------------------------*/
        Widget widget;
        XtPointer closure, callData;
{
   Display *display;
   display=XtDisplay(toplevel);
   XhSetHelpStatusProperty(toplevel, XHELP_NOT_ACTIVE);
   XDeleteProperty(display,DefaultRootWindow(display), XHELP_FILE);
   XDeleteProperty(display,DefaultRootWindow(display), XHELP_STATUS);
   XSync(display, FALSE); /* process everything before quitting! */
   exit(0);
} /*** Finish() ***/

/*-----------------*/
void main(argc, argv)
/*-----------------*/
int argc;
char **argv;
{
    XtAppContext app_con;
    Widget tempW;
    Display *dsp;
    int scn;
    Pixmap iconpixmap;
    Window root;
    XEvent event;	
    Widget lastwidget = NULL; 
    Dimension myWidth, myBorderWidth;
 
    int i,n;
    String tempmsg;

    fileServices_init();

  /****| set help mode |************************************/
   if (argc > 1) helpmode = USER_MODE;
  /*********************************************************/
 
  /*********************************************************/
  /* We'll just create this as a fallback--couldn't get it to
     work widget creation time: 
	XtNeditType, XawtextRead *or* XawtextEdit */
    fallback_resources[0]=(String) XtCalloc(1,80);
    sprintf(fallback_resources[0],"%s   %s",
           "*textFile*editType:", (helpmode==USER_MODE?"read":"edit"));
  /*********************************************************/
   
    toplevel = XtAppInitialize(&app_con, "XHelp", options, XtNumber(options),
		&argc, argv, fallback_resources, NULL, ZERO);

    /** Add string to function bindings for our app actions **/
/****
    XtAppAddActions(app_con, actionTable, XtNumber(actionTable));
*****/

    dsp = XtDisplay(toplevel);
    scn = DefaultScreen(dsp);
    root= DefaultRootWindow(dsp);

  /****************** define program icon ******************/
    iconpixmap = MakePixmapFromBitmap(help_bits,help_width, help_height);
    XtVaSetValues (toplevel, 
    XtNiconPixmap, XCreateBitmapFromData (XtDisplay(toplevel),
          XtScreen(toplevel)->root, help_bits, help_width, help_height),
      XtNiconMask, XCreateBitmapFromData (XtDisplay(toplevel),
          XtScreen(toplevel)->root, helpmask_bits, helpmask_width, helpmask_height), NULL);
  /**********************************************************/
  XtAddCallback( toplevel, XtNdestroyCallback, Finish, NULL );
  outerForm =  
     XtCreateManagedWidget( "outerForm", formWidgetClass,toplevel,NULL,0);

  /**************************** menu creation ****************************/
  menuForm=XtCreateManagedWidget("menuForm",boxWidgetClass, outerForm, NULL,0);
/* ---| DEVELOPER_MODE |--------------------------------------------------*/
  if (helpmode == DEVELOPER_MODE) {
     file = XtVaCreateManagedWidget( "FILE", menuButtonWidgetClass, menuForm,
           XtNmenuName, "fileMenu",NULL);
     fileMenu = XtCreatePopupShell("fileMenu", simpleMenuWidgetClass,file,NULL,0); 
  for (i=0; i<XtNumber(FileMenuItems); i++) {
      entry = XtVaCreateManagedWidget(FileMenuItems[i].name, smeBSBObjectClass, fileMenu, 
          	XtNlabel, FileMenuItems[i].name, NULL);
      XtAddCallback(entry, XtNcallback, FileMenuItems[i].function,
		(XtPointer)FileMenuItems[i].client_data);	
  } /* for */ 
  /***********************************************************************/
  lastwidget=edit=XtVaCreateManagedWidget("edit",menuButtonWidgetClass,menuForm,
         XtNfromHoriz, file, XtNmenuName, "editMenu", XtNlabel, "EDIT", NULL);
  editMenu=XtCreatePopupShell("editMenu",simpleMenuWidgetClass,edit,NULL,0);
  XtVaCreateManagedWidget("Line", smeLineObjectClass,editMenu, XtNborderWidth,0,NULL); 
  for (i=0; i<XtNumber(EditMenuItems); i++) {
      char * item =  EditMenuItems[i].name;
      entry=XtVaCreateManagedWidget(item,smeBSBObjectClass,editMenu, XtNlabel, item, NULL);
      XtAddCallback(entry, XtNcallback, EditMenuItems[i].function, (XtPointer)
                EditMenuItems[i].client_data);
  } /* for */
  } /** DEVELOPER_MODE **/
/* ----| DEVELOPER_MODE |-------------------------------------------------*/
  quit=XtVaCreateManagedWidget(QUIT_STR, commandWidgetClass, menuForm, 
		XtNfromHoriz, lastwidget, NULL);
  XtAddCallback(quit, XtNcallback, SaveBeforeQuit, (XtPointer) helpmode);

  textFile = XtVaCreateManagedWidget("textFile", asciiTextWidgetClass, outerForm,
       XtNdisplayCaret, (helpmode==USER_MODE?FALSE:TRUE),
       XtNstring, topic_rec[topic_select].text,
       XtNlength, topic_rec[topic_select].size,
       NULL);

  XtVaGetValues(textFile, XtNwidth, &myWidth, XtNborderWidth, &myBorderWidth, NULL);
  iconForm=XtVaCreateManagedWidget("iconForm",panedWidgetClass,outerForm,
	XtNfromVert, menuForm, XtNorientation, "horizontal", NULL);

  CreateIconButtons(iconForm);
  titleLabel= XtVaCreateManagedWidget("titleLabel", commandWidgetClass, outerForm, 
 	XtNfromVert, iconForm,
	XtNlabel, topic_rec[topic_select].topic,
 	XtNwidth, myWidth+myBorderWidth,  
	XtNborderWidth, 1, NULL);

  if (helpmode==DEVELOPER_MODE)
      XtAddCallback(titleLabel, XtNcallback, PopupPrompt, NULL); 

  XtVaSetValues(textFile, XtNfromVert,titleLabel,NULL); /* now position it in place */
  XhCreateHelpAtoms(toplevel); /* create the interclient comm atoms */ 
  XhSetHelpStatusProperty(toplevel,XHELP_ACTIVE);
  XtRealizeWidget(toplevel); /* Realize everthing */
  if (helpmode == USER_MODE) {
     if (!(OpenHelp(xhelpPath,argv[1],(argc>2?argv[2]:XHELP_INTRO_ID), "")))  
        OpenHelp(xhelpPath, DEFAULT_HELPFILE,XHELP_INTRO_ID,"");
  }
  else {
     OpenHelp(xhelpPath, DEFAULT_HELPFILE,XHELP_INTRO_ID,""); 
  }
  /*** Request property change event for the root window ***/
    XSelectInput(dsp, root, PropertyChangeMask); 
  while (TRUE) { /* we must use own own loop to get events for root window */
      XtAppNextEvent(app_con,&event);
      switch (event.type) {
         case PropertyNotify:
            if  (event.xproperty.window == root) {  
               if (event.xproperty.atom == XHELP_FILE)
                   GetAppHelpData();
               }
            else
 	       XtDispatchEvent(&event);
	       break;
         default:
	    XtDispatchEvent(&event);
     } /** switch **/ 
   } /** while **/
} /* main() */
/******************* xhelp.c **********************/
