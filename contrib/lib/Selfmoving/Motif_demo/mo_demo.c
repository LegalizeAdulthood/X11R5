
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%                      COPYRIGHT 1991 HUBERT RECHSTEINER                      %
%                                                                             %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee, provided  %
%  documentation, and that the name of the author not be used in advertising  %
%  or publicity pertaining to distribution of the software without specific,  %
%  written prior permission. The author makes no representations about the    %
%  suitability of this software for any purpose.  It is provided "as is"      %
%  without express or implied warranty.                                       %
%                                                                             %
%  THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,          %
%  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN        %
%  NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR           %
%  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS        %
%  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE      %
%  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE         %
%  USE OR PERFORMANCE OF THIS SOFTWARE.                                       %
%                                                                             %
%                                                                             %
%  Software Design       : Hubert Rechsteiner January 1991                    %
%                                                                             %
%  Further developments  : Doris Hirt                                         %
%                          Nicolas Repond                                     %
%                          Hubert Rechsteiner                                 %
%                          October 1991                                       %
%                                                                             %
%                                                                             %
%  Please send any improvements, bug fixes, useful modifications, and         %
%  comments to:  selfmoving@uliis.unil.ch                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

/* mo_demo: 	a demonstration program for the Selfmoving Widget, 
		using OSF/Motif */

static char SccsId[] = "@(#)mo_demo.c	3.1 	 10/4/91";

#ifdef VMS
#include "Intrinsic.h"
#include "StringDefs.h"
#include "Xm.h"
#include "RowColumn.h"
#include "BulletinB.h"
#include "PushB.h"
#include "PushBG.h"
#include "Label.h"
#include "Separator.h"
#include "Text.h"
#else
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/BulletinB.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/Label.h>
#include <Xm/Separator.h>
#include <Xm/Text.h>
#endif
#include "SelfM.h"

#define NB_MENUS	9


void menu_callback();

static void window(); /* procedure creating a new window */

static  Widget          toplevel;
static  Widget          selfmoving4,son4,son4_1,son4_2,son4_3;
static  Widget          selfmoving5,son5;
static  Widget	        pulldown,options[NB_MENUS],option_menu;

static	Widget          selfmoving_title_1,title_1_label;
static	Widget		new_window;
	int		n,identification,total_windows;

Widget			which_window[100]; /* array containing one selfmoving
			widget of each window that has been opened (up to 99 new
			windows may be opened). These widgets will be referenced
			to each time you select an action of the menu.
			Remember that you can reference any selfmoving of a 
			particular window : the action will be applied to all 
			widgets of this window. */

main(int argc,char *argv[])
{
	Widget          main;
        Widget	        selfmoving_label_1,label_1;
        Widget	        selfmoving_label_2,label_2;
        Widget	        selfmoving_label_3,label_3;
	Widget	        selfmoving_text_1,text_1;
	Widget	        selfmoving_text_2,text_2;
	Widget	        selfmoving_text_3,text_3;
        ArgList         arglist;
        Arg	        arg[10];
        int	        n,i;
        Dimension	height,borderwidth,baseline;
        short		marginheight,margintop,shadowthickness;
        XmFontList	fontlist;
        XmString	labelstring;

	toplevel = XtInitialize(argv[0],"mo_demo",NULL,
				0,&argc,argv);


        main = XtCreateManagedWidget("main",xmBulletinBoardWidgetClass,
                                      toplevel,arglist,0);
	total_windows=1; /* counter for the total amount of windows */

/********************START OF MENU CREATION**************************************/
         pulldown = XmCreatePulldownMenu(main,"pulldown",NULL,0);
         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align left",XmSTRING_DEFAULT_CHARSET));n++;
         options[0]=XmCreatePushButtonGadget(pulldown,"left",arg,n);
         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align right",XmSTRING_DEFAULT_CHARSET));n++;
         options[1]=XmCreatePushButtonGadget(pulldown,"right",arg,n);
         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align center",XmSTRING_DEFAULT_CHARSET));n++;
         options[2]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align top",XmSTRING_DEFAULT_CHARSET));n++;
         options[3]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align bottom",XmSTRING_DEFAULT_CHARSET));n++;
         options[4]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Align center vertical",
                        XmSTRING_DEFAULT_CHARSET));n++;
         options[5]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Activate/Inactivate the Selfmoving",
                          XmSTRING_DEFAULT_CHARSET));n++;
         options[6]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

	 n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("New window",XmSTRING_DEFAULT_CHARSET));n++;
         options[7]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Quit",XmSTRING_DEFAULT_CHARSET));n++;
         options[8]=XmCreatePushButtonGadget(pulldown,"right",arg,n);

         XtManageChildren(options,9);

         n=0;
         XtSetArg(arg[0],XmNlabelString,
                XmStringCreate("Try an option: ",XmSTRING_DEFAULT_CHARSET));n++;
         XtSetArg(arg[2],XmNsubMenuId,pulldown);n++;
         XtSetArg(arg[3],XmNmenuHistory,options[0]);n++;

         option_menu = XmCreateOptionMenu(main,"option_menu",arg,n);
         XtManageChild(option_menu);
         for (i=0;i<NB_MENUS;i++) {
	     identification=(total_windows*100)+i;
	     /* the identification number combines the number of the window with 
	     the number of the selected menu item. It is then decomposed in
	     the menu_callback, so that the right operation is applied to the 
	     right window */
	     /* *100 avoids rounding problems */
             XtAddCallback(options[i],XmNactivateCallback,menu_callback,
			    identification);
         }
	 
/*********************** *END OF MENU CREATION* *****************************/

/***************************** *TITLE* **************************************/
  
     	selfmoving_title_1 = XtCreateManagedWidget("selfmovingtitle1",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);

        title_1_label = XtCreateManagedWidget("title1label",xmLabelWidgetClass,
                                         selfmoving_title_1,arglist,0);

	which_window[total_windows]=selfmoving_title_1;
	/* for each window, one selfmoving widget must be stored (no matter 
	which one), so that the different actions of the menu can be executed
	on the correct window. */

/****************************** *END TITLE* **********************************/

/******************************** *TEXT_1* ***********************************/

     	selfmoving_text_1 = XtCreateManagedWidget("selfmovingtext1",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        text_1 = XtCreateManagedWidget("text1",xmTextWidgetClass,
                                         selfmoving_text_1,arglist,0);
	n=0;
	XtSetArg(arg[n],XtNhorizontalLineOffset,
		(int)XmTextGetBaseline(text_1));n++;
	XtSetValues(selfmoving_text_1,arg,n);
/*
       XtInstallAccelerators(main,selfmoving_text_1);
*/

/********************************* *TEXT_2* **********************************/

     	selfmoving_text_2 = XtCreateManagedWidget("selfmovingtext2",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        text_2 = XtCreateManagedWidget("text2",xmTextWidgetClass,
                                         selfmoving_text_2,arglist,0);
	n=0;
	XtSetArg(arg[n],XtNhorizontalLineOffset,
		(int)XmTextGetBaseline(text_2));n++;
	XtSetValues(selfmoving_text_2,arg,n);

/********************************* *TEXT_3* **********************************/

     	selfmoving_text_3 = XtCreateManagedWidget("selfmovingtext3",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        text_3 = XtCreateManagedWidget("text3",xmTextWidgetClass,
                                         selfmoving_text_3,arglist,0);
	n=0;
	XtSetArg(arg[n],XtNhorizontalLineOffset,
		(int)XmTextGetBaseline(text_3));n++;
	XtSetValues(selfmoving_text_3,arg,n);

/********************************* *LABEL_1* *********************************/
  
     	selfmoving_label_1 = XtCreateManagedWidget("selfmovinglabel1",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        label_1 = XtCreateManagedWidget("label1",xmLabelWidgetClass,
                                         selfmoving_label_1,arglist,0);

        n=0;
        XtSetArg(arg[n],XmNheight,&height);n++;
        XtSetArg(arg[n],XmNmarginHeight,&marginheight);n++;  
        XtSetArg(arg[n],XmNborderWidth,&borderwidth);n++;  
        XtSetArg(arg[n],XmNmarginTop,&margintop);n++;  
        XtSetArg(arg[n],XmNshadowThickness,&shadowthickness);n++;  
        XtSetArg(arg[n],XmNfontList,&fontlist);n++;  
        XtSetArg(arg[n],XmNlabelString,&labelstring);n++;  
        XtGetValues(label_1,arg,n);

        baseline=XmStringBaseline(fontlist,labelstring);

        n=0;
        XtSetArg(arg[n],XtNhorizontalLineOffset,
                   (int)borderwidth+
                   (int)marginheight+(int)margintop+(int)shadowthickness
                   +(int)baseline);n++;
        XtSetValues(selfmoving_label_1,arg,n);


/********************************* *LABEL_2* *********************************/

     	selfmoving_label_2 = XtCreateManagedWidget("selfmovinglabel2",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        label_2 = XtCreateManagedWidget("label2",xmLabelWidgetClass,
                                         selfmoving_label_2,arglist,0);

        n=0;
        XtSetArg(arg[n],XmNheight,&height);n++;
        XtSetArg(arg[n],XmNmarginHeight,&marginheight);n++;  
        XtSetArg(arg[n],XmNborderWidth,&borderwidth);n++;  
        XtSetArg(arg[n],XmNmarginTop,&margintop);n++;  
        XtSetArg(arg[n],XmNshadowThickness,&shadowthickness);n++;  
        XtSetArg(arg[n],XmNfontList,&fontlist);n++;  
        XtSetArg(arg[n],XmNlabelString,&labelstring);n++;  
        XtGetValues(label_2,arg,n);

        baseline=XmStringBaseline(fontlist,labelstring);

        n=0;
        XtSetArg(arg[n],XtNhorizontalLineOffset,
                   (int)borderwidth+
                   (int)marginheight+(int)margintop+(int)shadowthickness
                   +(int)baseline);n++;
        XtSetValues(selfmoving_label_2,arg,n);

/******************************** *LABEL_3* *********************************/

     	selfmoving_label_3 = XtCreateManagedWidget("selfmovinglabel3",
                                           hrSelfMovingWidgetClass,main,
					   NULL,0);
        label_3 = XtCreateManagedWidget("label3",xmLabelWidgetClass,
                                         selfmoving_label_3,arglist,0);

        n=0;
        XtSetArg(arg[n],XmNheight,&height);n++;
        XtSetArg(arg[n],XmNmarginHeight,&marginheight);n++;  
        XtSetArg(arg[n],XmNborderWidth,&borderwidth);n++;  
        XtSetArg(arg[n],XmNmarginTop,&margintop);n++;  
        XtSetArg(arg[n],XmNshadowThickness,&shadowthickness);n++;  
        XtSetArg(arg[n],XmNfontList,&fontlist);n++;  
        XtSetArg(arg[n],XmNlabelString,&labelstring);n++;  
        XtGetValues(label_3,arg,n);

        baseline=XmStringBaseline(fontlist,labelstring);

        n=0;
        XtSetArg(arg[n],XtNhorizontalLineOffset,
                   (int)borderwidth+
                   (int)marginheight+(int)margintop+(int)shadowthickness
                   +(int)baseline);n++;
        XtSetValues(selfmoving_label_3,arg,n);


     	selfmoving4 = XtCreateManagedWidget("selfmoving4",
                                           hrSelfMovingWidgetClass,main,
					   arglist,0);
        son4=XtCreateManagedWidget("son4",xmBulletinBoardWidgetClass,
                       selfmoving4,NULL,0);
        son4_1 = XtCreateManagedWidget("son4_1",xmLabelWidgetClass,
                                         son4,arglist,0);

        son4_2 = XtCreateManagedWidget("son4_2",xmPushButtonWidgetClass,
                                        son4,arglist,0);

     	selfmoving5 = XtCreateManagedWidget("selfmoving5",
                                           hrSelfMovingWidgetClass,main,
					   arglist,0);
        son5 = XtCreateManagedWidget("son5",
                                            xmSeparatorWidgetClass,
                                            selfmoving5,arglist,0);



	XtRealizeWidget(toplevel);
	XtMainLoop();
}
/************************ *END OF THE MAIN PROGRAM* ************************/



/************************** *PROCEDURE MENU_CALLBACK* **********************/

void menu_callback( w, client_data, any)
    Widget  w;
    caddr_t client_data;
    XmAnyCallbackStruct	any;

/* XmAnyCallbackSruct collects widget specific data, client_data collects 
the identification number */

{
  Arg		    arg[2];
  Boolean	    active;
  int		    k,j;/* k=number of the selected item of the menu,
			 j=number of the window to which it applies */

    j=(int)client_data/100;
    k=(int)client_data-(j*100);

    if (j==1){
    /* main window */

  switch(k) {
    /* switch on the different items of the menu */
    case 0   :     XtSetArg(arg[0],XtNalignment,HrALIGN_LEFT);
		   break;
    case 1   :     XtSetArg(arg[0],XtNalignment,HrALIGN_RIGHT);
		   break;
    case 2   :     XtSetArg(arg[0],XtNalignment,HrALIGN_CENTER);
		   break;
    case 3   :     XtSetArg(arg[0],XtNalignment,HrALIGN_TOP);
		   break;
    case 4   :     XtSetArg(arg[0],XtNalignment,HrALIGN_BOTTOM);
		   break;
    case 5   :     XtSetArg(arg[0],XtNalignment,HrALIGN_CENTER_VERTICAL);
		   break;
    case 6   :     
                   XtSetArg(arg[0],XtNactive,&active);
                   XtGetValues(selfmoving_title_1,arg,1);
                   XtSetArg(arg[0],XtNactive,!active);
		   break;
		   
		   /*case 7: creation of a new window */
    case 7  :	   new_window=XmCreateBulletinBoardDialog(toplevel,"new_window",
							    NULL,0);
		   n = 0;
		   XtSetArg(arg[n], XmNwidth,800);n++;
		   XtSetArg(arg[n], XmNheight,800);n++;
		   XtSetValues(new_window,arg,n);
		   window(new_window);
		   XtManageChild(new_window);
		   break;
    case 8   :     exit(1);

  }}
  else  /* all windows but the main window */
{
 switch(k) {
    case 0   :     XtSetArg(arg[0],XtNalignment,HrALIGN_LEFT);
		   break;
    case 1   :     XtSetArg(arg[0],XtNalignment,HrALIGN_RIGHT);
		   break;
    case 2   :     XtSetArg(arg[0],XtNalignment,HrALIGN_CENTER);
		   break;
    case 3   :     XtSetArg(arg[0],XtNalignment,HrALIGN_TOP);
		   break;
    case 4   :     XtSetArg(arg[0],XtNalignment,HrALIGN_BOTTOM);
		   break;
    case 5   :     XtSetArg(arg[0],XtNalignment,HrALIGN_CENTER_VERTICAL);
		   break;
    case 6   :     
                   XtSetArg(arg[0],XtNactive,&active);
                   XtGetValues(which_window[j],arg,1);
                   XtSetArg(arg[0],XtNactive,!active);
		   break;
		   
    case 7   :     exit(1);
}}
  XtSetValues(which_window[j],arg,1);
  /* applies the selected action to the window j. Remember that it is sufficent
  to set the values to one selfmoving widget of the window: the action will be 
  completed on all the widgets of the corresponding window.*/
  
}
/********************** *END OF PROCEDURE MENU_CALLBACK* ********************/


/************************* *PROCEDURE WINDOW* ******************************/
/* procedure window creates a new window with selfmoving widgets that are
   independant of the main window */


static void window (parent)
    Widget  parent;

{   
    Widget  child,button_child;
    Widget  pulldown_child,options[NB_MENUS],option_menu_child;
    Widget  selfmoving_text_1,text_1;
    Widget  selfmoving_title_child,title_child_label;
   
    int	    n,i;
    ArgList warglist;
    Arg	    warg[10];
    

/* Selfmoving Widget, child of parent created in the menu_callback */
    child = XtCreateManagedWidget("child",hrSelfMovingWidgetClass,
                                  parent,NULL,0);

/* button, son of selfmovingwidget child */
    button_child = XtCreateManagedWidget("button_child",xmPushButtonWidgetClass,
                                    child,NULL,0);

/* Selfmovingwidget title */
    selfmoving_title_child = XtCreateManagedWidget("selfmovingtitlechild",
				hrSelfMovingWidgetClass,parent,NULL,0);

    title_child_label = XtCreateManagedWidget("titlechildlabel",xmLabelWidgetClass,
				selfmoving_title_child,warglist,0);

/* text 1 */
    selfmoving_text_1=XtCreateManagedWidget("selfmovingtext1",
					    hrSelfMovingWidgetClass,
					    parent,NULL,0);

    text_1=XtCreateManagedWidget("text1",xmTextWidgetClass,selfmoving_text_1,
				    warglist,0);
    n=0;
    XtSetArg(warg[n],XtNhorizontalLineOffset,
		(int)XmTextGetBaseline(text_1));n++;
    XtSetValues(selfmoving_text_1,warg,n);

/* number of the new window and storing of one selfmoving widget for
 this window */
    total_windows++;
    which_window[total_windows]=selfmoving_text_1;

/********************START OF MENU CREATION************************************/
     
	 pulldown_child = XmCreatePulldownMenu(parent,"pulldown_child",NULL,0);
         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align left",XmSTRING_DEFAULT_CHARSET));n++;
         options[0]=XmCreatePushButtonGadget(pulldown_child,"left",warg,n);
         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align right",XmSTRING_DEFAULT_CHARSET));n++;
         options[1]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);
         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align center",XmSTRING_DEFAULT_CHARSET));n++;
         options[2]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align top",XmSTRING_DEFAULT_CHARSET));n++;
         options[3]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align bottom",XmSTRING_DEFAULT_CHARSET));n++;
         options[4]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Align center vertical",
                        XmSTRING_DEFAULT_CHARSET));n++;
         options[5]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Activate/Inactivate the Selfmoving",
                          XmSTRING_DEFAULT_CHARSET));n++;
         options[6]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Quit",XmSTRING_DEFAULT_CHARSET));n++;
         options[7]=XmCreatePushButtonGadget(pulldown_child,"right",warg,n);

         XtManageChildren(options,8);

         n=0;
         XtSetArg(warg[0],XmNlabelString,
                XmStringCreate("Try an option: ",XmSTRING_DEFAULT_CHARSET));n++;
         XtSetArg(warg[2],XmNsubMenuId,pulldown_child);n++;
         XtSetArg(warg[3],XmNmenuHistory,options[0]);n++;

         option_menu_child = XmCreateOptionMenu(parent,"option_menu_child",
						warg,n);
         XtManageChild(option_menu_child);
         for (i=0;i<NB_MENUS-1;i++) {
	     identification=total_windows*100+i;
             XtAddCallback(options[i],XmNactivateCallback,
			    menu_callback,identification);
         }

/********************** *END OF MENU CREATION* *************************/    
    
}

