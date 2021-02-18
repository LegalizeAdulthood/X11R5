/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                                                             %
%                                                                             %
%                                                                             %
%  		       COPYRIGHT 1991 HUBERT RECHSTEINER		      %
%			                                                      %
%                                                                             %
%  Permission to use, copy, modify, distribute, and sell this software and    %
%  its documentation for any purpose is hereby granted without fee, provided  %   %  that the above copyright notice appear in all copies and that both that    %    %  copyright notice and this permission notice appear in  supporting          %
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
%									      %
%  Further developments  : Doris Hirt 					      %
%                          Nicolas Repond 				      %
%                          Hubert Rechsteiner				      %
%                          October 1991					      %
%									      %
%									      %
%  Please send any improvements, bug fixes, useful modifications, and         %
%  comments to:  selfmoving@uliis.unil.ch                                     %
%                                                                             %
%                                                                             %
%                                                                             %
%                                                                             %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/

static char SccsId[] = "@(#)aw_demo.c	3.1 	 10/4/91";




#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

#include <X11/Se/SelfM.h>

#define N_LABELS 3
#define N_TEXTS 3

void MenuSelect();

static String mnames[]={
		"item1",
		"item2",
		"item3",
		"item4",
                "item5",
                "item6",
                "item7",
		"quit"};
static String mentries[]={
                "Align left",
                "Align right",
                "Align center",
                "Align top",
                "Align bottom",
                "Align center vertical",
                "Activate/Inactivate the Selfmoving",
                "Quit"};

static Widget toplevel,selfmoving4,son4,son4_1,son4_2,son4_2_1;
static Widget self_title,title_label;

main(argc,argv)
int	argc;
char	**argv;
{
    XtAppContext app_context;
    Widget main,menu,button,entry;
    Widget labels[10],label_selfs[3];
    Widget texts[10],text_selfs[3];
    Arg args[5];
    Dimension borderwidth,internalheight;
    Position topmargin;
    XFontStruct *font;
    char prov[15];
    int n,i,baseline;

    toplevel=XtVaAppInitialize(&app_context,"SELFMOVINGD",NULL,0,
                               &argc,argv,NULL,NULL);

    main=XtCreateManagedWidget("main",formWidgetClass,toplevel,NULL,0);

    XawFormDoLayout(main, False);

    button=XtCreateWidget("mbutton",menuButtonWidgetClass,
                          main,NULL,0);

    menu=XtCreatePopupShell("menu",simpleMenuWidgetClass,button,NULL,0);
    for(i=0;i<8;i++) {
        n=0;
        XtSetArg(args[n],XtNlabel,mentries[i]);n++;
        entry=XtCreateManagedWidget(mnames[i],smeBSBObjectClass,menu,args,n);
        XtAddCallback(entry,XtNcallback,MenuSelect,i);
    }

/*******************************TITLE****************************************/

    self_title=XtCreateWidget("selftitle",hrSelfMovingWidgetClass,
                     main,NULL,0);

    title_label=XtCreateManagedWidget("titlelabel",labelWidgetClass,
                     self_title,NULL,0);

    for (i=1;i<=N_LABELS;i++) {
        sprintf(prov,"selflabel%d",i);
        label_selfs[i]=XtVaCreateWidget(prov,
               hrSelfMovingWidgetClass,main,
               XtNsizingHandlesWidth,8,XtNleft,XawChainLeft,
               XtNright,XawChainLeft,XtNtop,XawChainTop,XtNbottom,
               XawChainTop,XtNselectInReverseVideo,True,NULL);
        sprintf(prov,"label%d",i);
        labels[i]=XtCreateManagedWidget(prov,
               labelWidgetClass,label_selfs[i],NULL,0);
        n=0;
        XtSetArg(args[n],XtNinternalHeight,&internalheight);n++;
        XtSetArg(args[n],XtNborderWidth,&borderwidth);n++;
        XtSetArg(args[n],XtNfont,&font);n++;
        XtGetValues(labels[i],args,n);

        baseline=font->ascent;

        n=0;
        XtSetArg(args[n],XtNhorizontalLineOffset,
               (int)borderwidth+(int)internalheight+baseline);n++;
        XtSetValues(label_selfs[i],args,n);
    }

    for (i=1;i<=N_TEXTS;i++) {
        sprintf(prov,"selftext%d",i);
        text_selfs[i]=XtVaCreateWidget(prov,
               hrSelfMovingWidgetClass,main,
               XtNsizingHandlesWidth,8,XtNleft,XawChainLeft,
               XtNright,XawChainLeft,XtNtop,XawChainTop, XtNbottom,
               XawChainTop,XtNselectInReverseVideo,True,NULL);
        sprintf(prov,"text%d",i);
        texts[i]=XtCreateManagedWidget(prov,
               asciiTextWidgetClass,text_selfs[i],NULL,0);
        n=0;
        XtSetArg(args[n],XtNtopMargin,&topmargin);n++;
        XtSetArg(args[n],XtNborderWidth,&borderwidth);n++;
        XtSetArg(args[n],XtNfont,&font);n++;
        XtGetValues(texts[i],args,n);

        baseline=font->ascent;

        n=0;
        XtSetArg(args[n],XtNhorizontalLineOffset,
                (int)topmargin+(int)borderwidth+baseline);n++;
        XtSetValues(text_selfs[i],args,n);
    }

    selfmoving4=XtCreateWidget("selfmoving4",hrSelfMovingWidgetClass,
                      main,NULL,0);

    son4=XtVaCreateManagedWidget("son4",formWidgetClass,
                      selfmoving4,XtNwidth,100,XtNheight,140,NULL);

    son4_1 = XtCreateWidget("son4_1",labelWidgetClass,
                      son4,NULL,0);

    son4_2 = XtCreateWidget("son4_2",boxWidgetClass,
                      son4,NULL,0);

    son4_2_1 = XtCreateManagedWidget("son4_2_1",commandWidgetClass,
                      son4_2,NULL,0);

/*******************************TEXT_1****************************************/

    XtRealizeWidget(toplevel);
    XtRealizeWidget(button);
    XtMapWidget(button);
    XtRealizeWidget(self_title);
    XtMapWidget(self_title);
    for(i=1;i<=N_LABELS;i++) {
        XtRealizeWidget(label_selfs[i]);
        XtMapWidget(label_selfs[i]);
    }
    for(i=1;i<=N_TEXTS;i++) {
        XtRealizeWidget(text_selfs[i]);
        XtMapWidget(text_selfs[i]);
    }
    XtRealizeWidget(selfmoving4);
    XtMapWidget(selfmoving4);
    XtRealizeWidget(son4_1);
    XtMapWidget(son4_1);
    XtRealizeWidget(son4_2);
    XtMapWidget(son4_2);
    XtAppMainLoop(app_context);
}

void MenuSelect(widget,client_data,call_data)
Widget		widget;
XtPointer	client_data;
XtPointer	call_data;
{
    Arg arg[2];
    Boolean active;
    int pane_num=(int) client_data;

    switch(pane_num) {
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
    case 6   :     XtSetArg(arg[0],XtNactive,&active);
                   XtGetValues(self_title,arg,1);
                   XtSetArg(arg[0],XtNactive,!active);
		   break;
    case 7   :     exit(1);
    }
    XtSetValues(self_title,arg,1);    
}
