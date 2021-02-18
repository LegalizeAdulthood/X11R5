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

static char SccsId[] = "@(#)SelfM.c	3.4 	 10/5/91";


#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#ifdef VMS
#include "IntrinsicP.h"
#include "Intrinsic.h"
#include "StringDefs.h"
#include "CoreP.h"
#include "Composite.h"
#include "CompositeP.h"
#else
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/CoreP.h>
#include <X11/Composite.h>
#include <X11/CompositeP.h>
#endif
#include "SelfMP.h"

/*Methods of the Selfmoving*/
static void                 ClassInitialize();
static void                 Initialize();
static void                 Realize();
static void                 Resize();
static void                 My_Expose();
static void                 Destroy();
static Boolean              SetValues();
static XtGeometryResult	    GeometryManager();
static void                 ChangeManaged();
static XtArgsProc	    InsertChild();

/*Actions of the Selfmoving*/
static void select();
static void extend_selection();
static void unselect();
static void move();
static void end_move();
static void restack_windows();
static void erase();

/*Accelerators of the Selfmoving*/
static void shiftbuttondown_acc();
static void buttondown_acc();
static void buttonmotion_acc();
static void buttonup_acc();
static void erase_acc();

/*Actions of the handles*/
static void start_resize();
static void do_resize();
static void stop_resize();

/*Utility functions*/
static void CvtStringToHrHandleType();
static void CvtStringToHrAlignType();
static void align_selected_widgets();
static void map_sizing_handles();
static void set_reverse_video();
static void show_selected_state();
static void show_normal_state();
static void set_sizing_handles_attributes();
static void add_brother();
static void remove_brother();
static void remove_all_brothers();
static Boolean test_move();
static void draw_rectangles();
static void move_widgets();
static void give_bounds();
static void grid_position();
static void give_rect();
static Position give_x_from_x_child();
static Position give_y_from_y_child();
static Position give_x_child_from_x();
static Position give_y_child_from_y();
/*List functions*/

static void new_header();
/*	Header	*header	    */
static void insert_widget();
/*     Header header;  */
/*     Widget w;	*/
static void delete_widget();
/*     Header header;	*/
/*     Widget w;	*/
static void delete_list();
/*Deletes the entire list containing w*/
/*     Header header;   */
/*     Widget w;	*/
static int nb_widgets_in_list();
/*Returns the number of widgets in the list*/
/*     Header header;   */
/*     Widget w;	*/
static void first_widget_in_list();
/*Returns the first widget in the widget's list*/
/*     Header header;   */
/*     Widget widget;   */
/*     Widget w;	*/
static void init_context();
/*   Header		    header;	    */
/*   Opaque		    *context;	    */
/*   Widget		    w;		    */
/*   int		    *nb_widgets;    */
static void next_widget();
/*   Opaque		    *context;	    */
/*   Widget		    *w;		    */
static void show_lists();
/*   Header header;			    */



/*
static char defaultTranslations[] = 
		    "Shift<Btn1Down>	    :	Extend_selection()  \n\
                     Ctrl<Btn1Down>	    :   Unselect()	    \n\
                     ~Shift ~Ctrl<Btn1Down> :   Select()	    \n\
                     ~Shift ~Ctrl<Btn1Up>   :   End_move()	    \n\
                     <Btn1Motion>	    :   Move()              \n\
                     <Visible>		    :   Restack_windows()";
*/

static char defaultTranslations[] = 
		    "~Ctrl Shift<Btn1Down>  	:   Extend_selection()  \n\
                     ~Shift Ctrl<Btn1Down>    	:   Unselect()	    	\n\
                     Shift Ctrl<Btn1Down>      	:   Erase()	   	\n\
                     <Btn1Down> 		:   Select()	    	\n\
                     <Btn1Up>   		:   End_move()	    	\n\
                     <Btn1Motion>	    	:   Move()              \n\
                     <Visible>		    	:   Restack_windows()";


static XtActionsRec actionsList[] = {
    { "Erase",(XtActionProc) erase }, 
    { "Select", (XtActionProc) select },
    { "Unselect", (XtActionProc) unselect },
    { "Extend_selection", (XtActionProc) extend_selection },
    { "Move", (XtActionProc) move },
    { "End_move", (XtActionProc) end_move },
    { "Restack_windows", (XtActionProc) restack_windows },
    { "Shiftbuttondown_acc", (XtActionProc) shiftbuttondown_acc },
    { "Buttondown_acc", (XtActionProc) buttondown_acc },
    { "Buttonmotion_acc", (XtActionProc) buttonmotion_acc },
    { "Buttonup_acc", (XtActionProc) buttonup_acc },
    { "Erase_acc", (XtActionProc) erase_acc },
    { "Start_resize", (XtActionProc) start_resize },
    { "Do_resize", (XtActionProc) do_resize },
    { "Stop_resize", (XtActionProc) stop_resize },
};



static char handle_table[] = 
                       "<Btn1Down>      : Start_resize() \n\
                        <Btn1Motion>    : Do_resize()  \n\
                        <Btn1Up>        : Stop_resize()";
/*
static XtActionsRec  handle_actionList[] = {
    { "Start_resize", (XtActionProc) start_resize },
    { "Do_resize", (XtActionProc) do_resize },
    { "Stop_resize", (XtActionProc) stop_resize },
};
*/
/*This would be the best table. But doesn'nt work with Motif!!!! When dragging*/
/*the mouse the Buttonmotion_acc is only called when the mouse leaves the main*/
/*window or crosses a Selfmoving. Furthermore accelerators trigerred by       */
/*KeyPress are not taken into account.                                        */
static char accelerator_table[] =
		       "#override \n\
                        Shift<Btn1Down>   	: Shiftbuttondown_acc() \n\
                        <Btn1Down>   		: Buttondown_acc()   	\n\
			<Btn1Motion>    	: Buttonmotion_acc() 	\n\
			<Btn1Up>		: Buttonup_acc()      	\n\
    			<Key>Delete		: Erase_acc()";

/*For motif use the following table!!!                                        */
/*
static char accelerator_table[] =
		       "#override \n\
                        ~Ctrl Shift<ButtonPress>   : Shiftbuttondown_acc() \n\
    			Shift Ctrl<ButtonPress>	   : Erase_acc()	   \n\
                        <ButtonPress>   	   : Buttondown_acc()      \n\
			<Btn1Motion>    	   : Buttonmotion_acc()    \n\
			<Btn1Up>		   : Buttonup_acc()";
*/

static XtResource resources[] = {
    {XtNxChild, XtCXChild, XtRPosition, sizeof(Position),
       XtOffset(HrSelfMovingWidget,selfMoving.x_child),XtRString, "0"},
    {XtNyChild, XtCYChild, XtRPosition, sizeof(Position),
       XtOffset(HrSelfMovingWidget,selfMoving.y_child),XtRString, "0"},
    {XtNselected, XtCSelected, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.selected),XtRString, "false"},
    {XtNallowMove, XtCAllowMove, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.allow_move),XtRString, "true"},
    {XtNactive, XtCActive, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.active),XtRString, "true"},
    {XtNsizingHandlesWidth, XtCSizingHandlesWidth, XtRDimension, 
       sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.sizing_handles_width),
       XtRString, "3"},
    {XtNsizingHandlesBorderWidth, XtCSizingHandlesBorderWidth, XtRDimension, 
       sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.sizing_handles_border_width),
       XtRString, "0"},
    {XtNgridOn, XtCGridOn, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.grid_on),XtRString, "false"},
    {XtNgrid, XtCGrid, XtRInt, sizeof(int),
       XtOffset(HrSelfMovingWidget,selfMoving.grid),XtRString, "3"},
    {XtNselectInReverseVideo, XtCSelectInReverseVideo, XtRBoolean,
       sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.select_in_reverse_video),
       XtRString, "false"},
    {XtNhorizontalLineOffset, XtCLineOffset, XtRInt, sizeof(int),
       XtOffset(HrSelfMovingWidget,selfMoving.horizontal_line_offset),
       XtRString, "0"},
    {XtNverticalLineOffset, XtCLineOffset, XtRInt, sizeof(int),
       XtOffset(HrSelfMovingWidget,selfMoving.vertical_line_offset),
       XtRString, "0"},
    {XtNshowHorizontalLine, XtCShowLine, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.show_horizontal_line),
       XtRString, "false"},
    {XtNshowVerticalLine, XtCShowLine, XtRBoolean, sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.show_vertical_line),
       XtRString, "false"},
    {XtNselectionBorder, XtCSelectionBorder, XtRInt, sizeof(int),
       XtOffset(HrSelfMovingWidget,selfMoving.selection_border),XtRString, "1"},
    {XtNnorthWestHandle, XtCDiagonalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.north_west_handle),
       XtRString, "SIZING_HANDLE"},
    {XtNnorthHandle, XtCVerticalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.north_handle),
       XtRString, "NO_HANDLE"},
    {XtNnorthEastHandle, XtCDiagonalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.north_east_handle),
       XtRString, "SIZING_HANDLE"},
    {XtNeastHandle, XtCHorizontalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.east_handle),
       XtRString, "NO_HANDLE"},
    {XtNsouthEastHandle, XtCDiagonalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.south_east_handle),
       XtRString, "SIZING_HANDLE"},
    {XtNsouthHandle, XtCVerticalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.south_handle),
       XtRString, "NO_HANDLE"},
    {XtNsouthWestHandle, XtCDiagonalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.south_west_handle),
       XtRString, "SIZING_HANDLE"},
    {XtNwestHandle, XtCHorizontalHandle, XtRHrHandleType, 
       sizeof(HrHandleType),
       XtOffset(HrSelfMovingWidget,selfMoving.west_handle),
       XtRString, "NO_HANDLE"},
    {XtNminimumWidth, XtCMinimum, XtRDimension, sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.minimum_width),XtRString, "0"},
    {XtNminimumHeight, XtCMinimum, XtRDimension, sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.minimum_height),XtRString, "0"},
    {XtNmaximumWidth, XtCMaximum, XtRDimension, sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.maximum_width),XtRString, "0"},
    {XtNmaximumHeight, XtCMaximum, XtRDimension, sizeof(Dimension),
       XtOffset(HrSelfMovingWidget,selfMoving.maximum_height),XtRString, "0"},
    {XtNalignment, XtCAlignment, XtRHrAlignType, sizeof(HrAlignType),
       XtOffset(HrSelfMovingWidget,selfMoving.alignment),
       XtRString,"ALIGN_NONE"},
    {XtNinstallAccelerators, XtCInstallAccelerators, XtRBoolean,
       sizeof(Boolean),
       XtOffset(HrSelfMovingWidget,selfMoving.install_accelerators),XtRString,
       "true"},
    {XtNhandleTranslations, XtCTranslations, XtRTranslationTable,
       sizeof(XtTranslations),
       XtOffset(HrSelfMovingWidget,selfMoving.handle_translations),XtRString,
       handle_table},
    {XtNselectCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.select_list),XtRCallback,NULL},
    {XtNunselectCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.unselect_list),XtRCallback,NULL},
    {XtNmoveCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.move_list),XtRCallback,NULL},
    {XtNduringMoveCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.during_move_list),
       XtRCallback,NULL},
    {XtNresizeCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.resize_list),XtRCallback,NULL},
    {XtNduringResizeCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.during_resize_list),
       XtRCallback,NULL},
    {XtNdoubleClickCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.double_click_list),
       XtRCallback,NULL},
    {XtNaccMoveCallback, XtCCallback,XtRCallback, sizeof(caddr_t),
       XtOffset(HrSelfMovingWidget,selfMoving.acc_move_list),
       XtRCallback,NULL},
};




HrSelfMovingClassRec HrselfMovingClassRec = {
    {
	(WidgetClass)&compositeClassRec,    /*superclass		    */
	"HrSelfMoving",			    /*class_name		    */
	sizeof(HrSelfMovingRec),            /*widget_size		    */
	ClassInitialize,		    /*class_initialize		    */
	NULL,				    /*class_part_initialize method  */
	FALSE,				    /*class_inited		    */
        Initialize,			    /*initialize method		    */
	NULL,				    /*initialize_hook method	    */
	Realize,			    /*realize method		    */
	actionsList,			    /*actions			    */
	XtNumber(actionsList),		    /*num_actions		    */
        resources,			    /*resources			    */
        XtNumber(resources),		    /*num_resources		    */
	NULLQUARK,			    /*xrm_class			    */
	TRUE,				    /*compress_motion		    */
	TRUE,				    /*compress_exposure		    */
	TRUE,				    /*compress_enterleave	    */
	TRUE,				    /*visible_interest		    */
	Destroy,			    /*destroy method		    */
	Resize,				    /*resize method		    */
	NULL,				    /*expose method		    */
	SetValues,			    /*set_values method		    */
	NULL,				    /*set_values_hook method	    */
	XtInheritSetValuesAlmost,	    /*set_values_almost method	    */
	NULL,				    /*get_values_hook method	    */
	NULL,				    /*accept_focus method	    */
	XtVersion,			    /*version			    */
	NULL,				    /*callback_private		    */
	defaultTranslations,		    /*tm_table			    */
	NULL,				    /*query_geometry		    */
	NULL,				    /*display_accelerator	    */
	NULL,				    /*extension			    */
    },
    {
	GeometryManager,		    /*geometry_manager method	    */
        ChangeManaged,			    /*change_managed method	    */
        (XtWidgetProc)InsertChild,	    /*insert_child method	    */
	XtInheritDeleteChild,		    /*delete_child method	    */
        NULL,				    /*extension			    */
    },
    { 
      NULL,				    /*selfmoving_list		    */
      NULL,				    /*brother_list		    */
    }
};

WidgetClass hrSelfMovingWidgetClass = (WidgetClass) &HrselfMovingClassRec;


typedef enum { 
  north_west,north,north_east,east,south_east,south,south_west,west
} WhichHandleType;


#define superclass  (&compositeClassRec)

/*Symbols for accessing instance variables inherited from core*/
#define	    X(w)			w->core.x
#define	    Y(w)			w->core.y
#define	    WIDTH(w)			w->core.width
#define	    HEIGHT(w)			w->core.height
#define     BORDER_WIDTH(w)		w->core.border_width
#define     ACCELERATORS(w)		w->core.accelerators

/*Symbols for accessing instance variables*/
#define     X_CHILD(w)			w->selfMoving.x_child
#define     Y_CHILD(w)			w->selfMoving.y_child
#define     GRID_ON(w)			w->selfMoving.grid_on
#define     GRID(w)			w->selfMoving.grid
#define     ALLOW_MOVE(w)               w->selfMoving.allow_move
#define     CLICKED(w)			w->selfMoving.clicked
#define     FIRST(w)			w->selfMoving.first
#define     MOTION(w)			w->selfMoving.motion
#define	    X_FIELD(w)			w->selfMoving.x_field
#define	    Y_FIELD(w)			w->selfMoving.y_field
#define	    DELTA_X(w)			w->selfMoving.delta_x
#define	    DELTA_Y(w)			w->selfMoving.delta_y
#define	    HORIZONTAL_LINE_OFFSET(w)	w->selfMoving.horizontal_line_offset
#define	    VERTICAL_LINE_OFFSET(w)	w->selfMoving.vertical_line_offset
#define     SHOW_HORIZONTAL_LINE(w)	w->selfMoving.show_horizontal_line
#define     SHOW_VERTICAL_LINE(w)	w->selfMoving.show_vertical_line
#define	    DPY(w)			w->selfMoving.dpy
#define	    GC(w)			w->selfMoving.gc
#define	    PARENT_WIDTH(w)		w->selfMoving.parent_width
#define	    PARENT_HEIGHT(w)		w->selfMoving.parent_height
#define	    SELECTION_BORDER(w)		w->selfMoving.selection_border
#define	    SELECTED(w)			w->selfMoving.selected
#define	    SAVED_BORDER_WIDTH(w)	w->selfMoving.saved_border_width
#define     CHILD(w)			w->selfMoving.child
#define     SIZING_HANDLES_WIDTH(w)	w->selfMoving.sizing_handles_width
#define     SIZING_HANDLES(w)		w->selfMoving.sizing_handles
#define     SIZING_HANDLES_STATE(w)	w->selfMoving.sizing_handles_state
#define     INVISIBLE_WINDOW(w)		w->selfMoving.invisible_window
#define     SIZING_HANDLES_BORDER_WIDTH(w)	w->selfMoving.sizing_handles_border_width
#define     SELECT_IN_REVERSE_VIDEO(w)  w->selfMoving.select_in_reverse_video
#define     ACTIVE(w)			w->selfMoving.active
#define     NORTH_WEST_HANDLE(w)	w->selfMoving.north_west_handle
#define     NORTH_HANDLE(w)		w->selfMoving.north_handle
#define     NORTH_EAST_HANDLE(w)	w->selfMoving.north_east_handle
#define     EAST_HANDLE(w)		w->selfMoving.east_handle
#define     SOUTH_EAST_HANDLE(w)	w->selfMoving.south_east_handle
#define     SOUTH_HANDLE(w)		w->selfMoving.south_handle
#define     SOUTH_WEST_HANDLE(w)	w->selfMoving.south_west_handle
#define     WEST_HANDLE(w)		w->selfMoving.west_handle
#define	    MINIMUM_WIDTH(w)		w->selfMoving.minimum_width
#define	    MINIMUM_HEIGHT(w)		w->selfMoving.minimum_height
#define	    MAXIMUM_WIDTH(w)		w->selfMoving.maximum_width
#define	    MAXIMUM_HEIGHT(w)		w->selfMoving.maximum_height
#define	    INSTALL_ACCELERATORS(w)	w->selfMoving.install_accelerators
#define     HANDLE_CREATION(w)		w->selfMoving.handle_creation
#define	    HANDLE_TRANSLATIONS(w)      w->selfMoving.handle_translations
#define     NB_CHILDREN(w)		w->selfMoving.nb_children
#define     FIRST_EXPOSE(w)		w->selfMoving.first_expose
#define     FIRST_SIZING(w)		w->selfMoving.first_sizing
#define     DELTA_X_SIZING(w)		w->selfMoving.delta_x_sizing
#define     DELTA_Y_SIZING(w)		w->selfMoving.delta_y_sizing
#define	    WHICH_HANDLE(w)		w->selfMoving.which_handle
#define     OLD_X_RECT_SIZING(w)	w->selfMoving.old_x_rect_sizing
#define     OLD_Y_RECT_SIZING(w)	w->selfMoving.old_y_rect_sizing
#define     OLD_WIDTH_RECT_SIZING(w)	w->selfMoving.old_width_rect_sizing
#define     OLD_HEIGHT_RECT_SIZING(w)	w->selfMoving.old_height_rect_sizing
#define     DO_RESIZE(w)		w->selfMoving.do_resize
#define     ALIGNMENT(w)		w->selfMoving.alignment
#define	    DOUBLE_CLICK(w)		w->selfMoving.double_click
#define     OLD_TIME(w)			w->selfMoving.old_time
#define     X_ACC(w)			w->selfMoving.x_acc
#define     Y_ACC(w)			w->selfMoving.y_acc
#define     PROV_X_ACC(w)		w->selfMoving.prov_x_acc
#define     PROV_Y_ACC(w)		w->selfMoving.prov_y_acc
#define     XR_ACC(w)			w->selfMoving.xr_acc
#define     YR_ACC(w)			w->selfMoving.yr_acc
#define     WIDTH_ACC(w)		w->selfMoving.width_acc
#define     HEIGHT_ACC(w)		w->selfMoving.height_acc
#define     MOTION_ACC(w)		w->selfMoving.motion_acc
#define     FIRST_ACC(w)		w->selfMoving.first_acc
#define	    PLEASE_DO_MOTION(w)		w->selfMoving.please_do_motion

#define	    SELECT_LIST(w)              w->selfMoving.select_list
#define	    UNSELECT_LIST(w)            w->selfMoving.unselect_list
#define	    MOVE_LIST(w)                w->selfMoving.move_list
#define	    DURING_MOVE_LIST(w)         w->selfMoving.during_move_list
#define	    RESIZE_LIST(w)              w->selfMoving.resize_list
#define	    DURING_RESIZE_LIST(w)       w->selfMoving.during_resize_list
#define	    DOUBLE_CLICK_LIST(w)        w->selfMoving.double_click_list
#define	    ACC_MOVE_LIST(w)		w->selfMoving.acc_move_list /*dh*/

/*Symbols for accessing class variables*/
#define     BROTHER_LIST HrselfMovingClassRec.selfMoving_class.brother_list
#define     SELFMOVING_LIST HrselfMovingClassRec.selfMoving_class.selfmoving_list


/***********************************METHODS************************************/
  
static void ClassInitialize()
/*Called once when the first Selfmoving is created*/
{
    /*Creation of header for the Selfmoving_list and the brother list */
    new_header(&SELFMOVING_LIST);
    new_header(&BROTHER_LIST);

    XtAddConverter(XtRString,XtRHrHandleType,CvtStringToHrHandleType,NULL,0);
    XtAddConverter(XtRString,XtRHrAlignType,CvtStringToHrAlignType,NULL,0);
}


static void Initialize(request,new)
  HrSelfMovingWidget request;
  HrSelfMovingWidget new;
/*Called for every Selfmoving when it is created with a XtCreateWidget call*/
{
  HrSelfMovingWidget first_widget,w;
  Opaque	     context;
  XGCValues	     values;
  Arg		     wargs[5];
  int		     num,i,n=0;
  Dimension	     prov_border;
  XtAccelerators     acc;


  /*Putting the new Selfmoving in the selfmoving_list*/
  insert_widget(SELFMOVING_LIST,(Widget)new);

  /*Install accelerators for the first selfmoving in the window if there is*/
  /*one selfmoving with this resource set to true*/  
  if (nb_widgets_in_list(SELFMOVING_LIST,new)==1) {
      if (INSTALL_ACCELERATORS(new)) {
          acc = XtParseAcceleratorTable(accelerator_table);
          if (ACCELERATORS(new) == NULL) ACCELERATORS(new) = acc;
          XtInstallAccelerators(XtParent(new),new);
      } 
  }
  else { 
  /*Initialization of global resources for the following selmovings*/
  first_widget_in_list(SELFMOVING_LIST,(Widget)new,&first_widget);
  ACTIVE(new) = ACTIVE(first_widget);
  GRID_ON(new) = GRID_ON(first_widget);
  GRID(new) = GRID(first_widget);
  INSTALL_ACCELERATORS(new) = INSTALL_ACCELERATORS(first_widget);
  if (!ACTIVE(new)) SELECTED(new)=FALSE;
  }

  /*Creating the GC for drawing outlined lines*/
  n=0;
  XtSetArg(wargs[n],XtNforeground,&values.foreground);n++;
  XtSetArg(wargs[n],XtNbackground,&values.background);n++;
  XtGetValues(XtParent(request),wargs,n);

  values.foreground = values.foreground^values.background;
  values.line_style = LineOnOffDash;
  values.function   = GXxor;

  GC(new) = XtGetGC(new,GCForeground | GCFunction | GCLineStyle,&values);
  /*let's draw over all other widgets in the same window */
  XSetSubwindowMode(XtDisplay(new),GC(new),IncludeInferiors);  

  /*Initialization of internal data*/
  DPY(new) = XtDisplay(new);	    
  FIRST(new) = 1;
  MOTION(new) = FALSE;
  CLICKED(new) = FALSE;
  X_FIELD(new) = 0;
  Y_FIELD(new) = 0;
  NB_CHILDREN(new) = 0;
  FIRST_EXPOSE(new) = TRUE;
  SAVED_BORDER_WIDTH(new)=BORDER_WIDTH(new);
  INVISIBLE_WINDOW(new) = NULL;
  ALIGNMENT(new) = -1;

  if (SELECTED(new)) {
     prov_border=SELECTION_BORDER(new);
     /*Put the new widget in the brother list*/
     insert_widget(BROTHER_LIST,new);
  }
  else prov_border = BORDER_WIDTH(new);

  /*If one of the resources X_CHILD and Y_CHILD is non zero, we suppose that*/
  /*the programmer wants to specify the position by these resources.*/
  /*Otherwise X_CHILD and Y_CHILD will be calculated from X and Y*/
  if (!((X_CHILD(new)==0) && (Y_CHILD(new)==0))) {
      X(new)= X_CHILD(new) - prov_border;
      Y(new)= Y_CHILD(new) - prov_border;
  }
  else {
         X_CHILD(new) = X(new) + prov_border;
         Y_CHILD(new) = Y(new) + prov_border;
  }

  /*Filling an array with the state of every handle, just easier to loop*/
  n=0;
  SIZING_HANDLES_STATE(new)[n] = NORTH_WEST_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = NORTH_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = NORTH_EAST_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = EAST_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = SOUTH_EAST_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = SOUTH_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = SOUTH_WEST_HANDLE(request);n++;
  SIZING_HANDLES_STATE(new)[n] = WEST_HANDLE(request);n++;

/*Creating the handles according to the resource values                       */
  n=0;
  if (!ACTIVE(new) || !SELECTED(new)) {  
    XtSetArg(wargs[n],XtNmappedWhenManaged,False);n++;
  } 
  for (i=0;i<NB_HANDLES;i++) {
       HANDLE_CREATION(new) = TRUE; 
       /*We need this flag in insert_child to know if the new child is the*/
       /*real child or just a sizing_handle*/
       SIZING_HANDLES(new)[i] = XtCreateManagedWidget("test",
                                        widgetClass,new,NULL,0);
       HANDLE_CREATION(new) = FALSE;
       XtSetValues(SIZING_HANDLES(new)[i],wargs,n);
       if ((SIZING_HANDLES_STATE(new)[i]!=HrNO_HANDLE) &&
           (SIZING_HANDLES_STATE(new)[i]!=HrREGULAR_HANDLE)) {
	           XtOverrideTranslations(SIZING_HANDLES(new)[i],
                          HANDLE_TRANSLATIONS(request));
       }
  }
  DOUBLE_CLICK(new)=FALSE;
}/*End of Initialize*/



static void Realize(widget, valueMask, attributes)
  Widget		    widget; 
  XtValueMask		    *valueMask;
  XSetWindowAttributes	    *attributes;
/*Called when the widget's window is going to be created                      */
{
   HrSelfMovingWidget w = (HrSelfMovingWidget)widget;
   int i;

  /*Calling the Selfmoving's superclass realize method to create the*/
  /*Selfmoving's own window*/
  (*superclass->core_class.realize)(w,valueMask,attributes);
  
   /*Creating an X InputOnly window, as a child window of the Selfmoving's*/
   /*window*/
   INVISIBLE_WINDOW(w) = XCreateWindow(DPY(w),XtWindow(w),
               0,0,WIDTH(w),HEIGHT(w),0,
               CopyFromParent,InputOnly,CopyFromParent,0,NULL);

   if (ACTIVE(w) && SELECTED(w)) {
       if (SELECT_IN_REVERSE_VIDEO(w))
           set_reverse_video(CHILD(w));
       XtConfigureWidget((Widget)w,X(w),Y(w),WIDTH(w),HEIGHT(w),
                     SELECTION_BORDER(w));
   }
}/*End of Realize*/



static void Resize(w)
  HrSelfMovingWidget w;
/*Called when the size of the widget is change by a XtSetValues call or       */
/*a XtConfigureWidget call.                                                   */
{
  /*Setting the INVISIBLE_WINDOW to the right size*/
  if (INVISIBLE_WINDOW(w)!=NULL) {
    XResizeWindow(DPY(w),INVISIBLE_WINDOW(w),WIDTH(w),HEIGHT(w));
  }
  /*Putting the handles at the right place*/
  set_sizing_handles_attributes(w);
}/*End of Resize*/


static void Destroy(w)
  HrSelfMovingWidget w;
{ 
  Widget            widget,next;
  Opaque            my_context;
  int	            nb_widgets;
  XtAccelerators    acc;
/* This called when XtDestroyWidget is called from the application or from    */
/* the widget code.                                                           */
/* What is happening here is pretty tricky. If we call XtDestroyWidget from   */
/* within the widget code, XtDestroyWidget will only mark these widgets as    */
/* deleted. The destroy method will only be called when returning from the    */
/* event handler. See erase_acc                                               */

  /*Getting the first widget in that window                                   */
  init_context(SELFMOVING_LIST,&my_context,(Widget)w,&nb_widgets);
  next_widget(&my_context,&next);

  /*If the Selmoving is selected, remove it from the brother list             */
  if (SELECTED(w)) {
     remove_brother(w); 
  }

  /*If it is the first one in the window, install the accelerators on the*/
  /*next one if there is one*/
  if ((next == (Widget)w) && (nb_widgets>1)) {
    next_widget(&my_context,&next);
    if (INSTALL_ACCELERATORS(w)==TRUE){
       acc = XtParseAcceleratorTable(accelerator_table);
       ACCELERATORS(next) = acc;
       XtInstallAccelerators(XtParent(w),next);
    }
  }
  /*Removing this Selfmoving from the list*/
  delete_widget(SELFMOVING_LIST,(Widget)w);

  /*Freeing GC and removing Callback lists*/
  XtReleaseGC(w,GC(w));
  XtRemoveAllCallbacks(w,XtNselectCallback,SELECT_LIST(w));
  XtRemoveAllCallbacks(w,XtNunselectCallback,UNSELECT_LIST(w));    
  XtRemoveAllCallbacks(w,XtNmoveCallback,MOVE_LIST(w));
  XtRemoveAllCallbacks(w,XtNduringMoveCallback,DURING_MOVE_LIST(w));
  XtRemoveAllCallbacks(w,XtNresizeCallback,RESIZE_LIST(w));
  XtRemoveAllCallbacks(w,XtNduringResizeCallback,DURING_RESIZE_LIST(w));
  XtRemoveAllCallbacks(w,XtNdoubleClickCallback,DOUBLE_CLICK_LIST(w));
  XtRemoveAllCallbacks(w,XtNaccMoveCallback,ACC_MOVE_LIST(w));
}/*End of Destroy method*/


static Boolean SetValues(current, request, new)
  HrSelfMovingWidget current;
  HrSelfMovingWidget request;
  HrSelfMovingWidget new;
/*Called for every XtSetValues call on that widget                            */
{
    HrSelfMovingWidget	    first_widget,w;
    Opaque		    context;
    XtAccelerators	    acc;
    int			    num,i;

   /*Setting of the XtNx and XtNy resources. We move the widget with*/
   /*XtMoveWidget and change the XtNxChild and XtNyChild resources.*/
   /*WARNING: If the parent is authoritative, it may refuse the  position*/
   /*change!!!!! The Selfmoving has no way to know about that, because there*/
   /*is no way in Xt for a widget to know if it has been moved by its parent!*/
   /*It this case the Selfmoving may act randomly.........*/

   if (X(current) != X(new)) {
       XtMoveWidget(new,X(new), Y(new));
       X_CHILD(new) = give_x_child_from_x(new);        
   }

   if (Y(current) != Y(new)) {
       XtMoveWidget(new,X(new), Y(new));
       Y_CHILD(new) = give_y_child_from_y(new);        
   }

   if (BORDER_WIDTH(current) != BORDER_WIDTH(new)) {
   /*If the border_width changes, we leave the child at the same position.*/
   /*To achieve this we change the XtNx and XtNy resources and the*/
   /*border_width with a call to XtConfigureWidget.*/
       if (SELECTED(new)) {
           SAVED_BORDER_WIDTH(new) = BORDER_WIDTH(new);
           BORDER_WIDTH(new) = SELECTION_BORDER(current);
       }
       else {
           XtConfigureWidget(new,
                         give_x_from_x_child(new),
                         give_y_from_y_child(new),
                         WIDTH(new), HEIGHT(new),
                         BORDER_WIDTH(new));
       }
   }

   if (SELECTION_BORDER(current) != SELECTION_BORDER(new)) {
       if (SELECTED(new)) {
           XtConfigureWidget(new,
                         give_x_from_x_child(new),
                         give_y_from_y_child(new),
                         WIDTH(new), HEIGHT(new),
                         SELECTION_BORDER(new));
       }
   }

   if (X_CHILD(current) != X_CHILD(new))
       XtMoveWidget(new,X_CHILD(new) - BORDER_WIDTH(new),Y(new));

   if (Y_CHILD(current) != Y_CHILD(new))
       XtMoveWidget(new,X(new), Y_CHILD(new) - BORDER_WIDTH(new));

   if (SELECTED(current) != SELECTED(new)) {
      if (XtIsRealized((Widget) new)) {
         /*We put new so the changes we make will be registred*/
         if (SELECTED(new)) show_selected_state(new);
         else show_normal_state(new);
      }
   }

   if (ACTIVE(current) != ACTIVE(new)) {
      remove_all_brothers(new);
      if (XtIsRealized((Widget) new)) {
         if (SELECTED(new)) show_normal_state(new);
      }
      init_context(SELFMOVING_LIST,&context,new,&num);
      for (i=1;i<=num;i++) {
          next_widget(&context,&w);
          ACTIVE(w) = ACTIVE(new);                      
          if (XtIsRealized((Widget) w)) {
             if (ACTIVE(w)) {
                 XRaiseWindow(DPY(w),INVISIBLE_WINDOW(w));
                 XMapWindow(DPY(w),INVISIBLE_WINDOW(w));
             }
             else {
                 XUnmapWindow(DPY(w),INVISIBLE_WINDOW(w));
             }
           }
       }
   }
        

   if (ALIGNMENT(current) != ALIGNMENT(new)) {
       if (ALIGNMENT(new) != HrALIGN_NONE) {
         align_selected_widgets(ALIGNMENT(new),new);
         ALIGNMENT(new) = HrALIGN_NONE;
       }
   }

   if (!INSTALL_ACCELERATORS(current) && INSTALL_ACCELERATORS(new)) {
       first_widget_in_list(SELFMOVING_LIST,(Widget)new,&first_widget);
       if(!first_widget->core.accelerators) {
         acc = XtParseAcceleratorTable(accelerator_table);
         first_widget->core.accelerators = acc;
         XtInstallAccelerators(XtParent(first_widget),first_widget);
       }
       else if (!INSTALL_ACCELERATORS(new)) {
         XtError("Impossible to remove the accelerators!");
       }
   }

   if (HANDLE_TRANSLATIONS(current) != HANDLE_TRANSLATIONS(new)) {
       for (i=0;i<NB_HANDLES;i++) {
         if ((SIZING_HANDLES_STATE(new)[i]!=HrNO_HANDLE) &&
             (SIZING_HANDLES_STATE(new)[i]!=HrREGULAR_HANDLE)) {
                    XtOverrideTranslations(SIZING_HANDLES(new)[i],
                           HANDLE_TRANSLATIONS(new));
         }
       }
   }

   if (GRID_ON(current) != GRID_ON(new)) {
       init_context(SELFMOVING_LIST,&context,new,&num);
       for (i=1;i<=num;i++) {
         next_widget(&context,&w);
         GRID_ON(w) = GRID_ON(new);
       }
   }

   if (GRID(current) != GRID(new)) {
       if (GRID(new)>0) {
         init_context(SELFMOVING_LIST,&context,new,&num);
         for (i=1;i<=num;i++) {
           next_widget(&context,&w);
           GRID(w) = GRID(new);
         }
       }
   }

   if (SIZING_HANDLES_WIDTH(current) != SIZING_HANDLES_WIDTH(new)) {
       set_sizing_handles_attributes(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (SIZING_HANDLES_BORDER_WIDTH(current) !=
                        SIZING_HANDLES_BORDER_WIDTH(new)) {
       set_sizing_handles_attributes(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (NORTH_WEST_HANDLE(current) != NORTH_WEST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[0] = NORTH_WEST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (NORTH_HANDLE(current) != NORTH_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[1] = NORTH_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (NORTH_EAST_HANDLE(current) != NORTH_EAST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[2] = NORTH_EAST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (EAST_HANDLE(current) != EAST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[3] = EAST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (SOUTH_EAST_HANDLE(current) != SOUTH_EAST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[4] = SOUTH_EAST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (SOUTH_HANDLE(current) != SOUTH_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[5] = SOUTH_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (SOUTH_WEST_HANDLE(current) != SOUTH_WEST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[6] = SOUTH_WEST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }

   if (WEST_HANDLE(current) != WEST_HANDLE(new)) {
       SIZING_HANDLES_STATE(new)[7] = WEST_HANDLE(new);
       if (SELECTED(new)) map_sizing_handles(new,TRUE);
   }
   return FALSE;
}/*End of SetValues*/


static XtGeometryResult GeometryManager(son, request, reply)
  Widget	    son;
  XtWidgetGeometry  *request;
  XtWidgetGeometry  *reply;
/*This method is called if the Selfmovingchild calls XtMakeGeometryRequest or */
/*XtMakeResizeRquest to ask a geometry change. The Selfmoving will allow any  */
/*change, except a position change of the child                               */
{
  HrSelfMovingWidget w = (HrSelfMovingWidget) son->core.parent;
  Dimension	    width,height,border_width; 

 
/*If the child asks a change in borderwidth, width or height we accept the
 * change and set the new value in the child. For x and y we do nothing.
 * We configure the Selfmoving then we return XtGeometryYes. The Intrinsics
 * will then configure the child according to its x,y,width,heigth and 
 * borderwidth resources. In this way the changes in x or y are not taken
 * into account. Is this a good practice?????????
*/


  width  = WIDTH(son);
  height = HEIGHT(son);
  border_width = BORDER_WIDTH(son);

  if (request->request_mode & CWBorderWidth) {
     border_width = request->border_width;
     BORDER_WIDTH(son) = border_width;
  }

  if (request->request_mode & CWWidth) {
     width = request->width;
     WIDTH(son) = width;
  }
  if (request->request_mode & CWHeight) {
     height = request->height;
     HEIGHT(son) = height;
  }
/* This is not recommanded. And not necessary
  XtConfigureWidget(son,X(son),Y(son),width,height,border_width);
*/
  if (!SELECTED(w)) {
     XtConfigureWidget(w,X(w),Y(w),width+2*border_width,
                       height+2*border_width,SAVED_BORDER_WIDTH(w));
  }
  else {
     XtConfigureWidget(w,X(w),Y(w),width+2*border_width,
                       height+2*border_width,SELECTION_BORDER(w));
  }

  XResizeWindow(DPY(w),INVISIBLE_WINDOW(w),width+2*border_width,
                   height+2*border_width);

  set_sizing_handles_attributes(w);

  return XtGeometryYes;
}/*End of GeometryManager*/


static void ChangeManaged(w)
  HrSelfMovingWidget w;
{
/*Called by XtRealize in the initial geometry management round and            */
/*every time a child is managed or unmanaged, but this should not happen      */
/*If the widget is realized we should change the size of the invisible        */
/*window in addition of changing the widget size and the layout of the        */
/*sizing handles                                                              */

  if (HEIGHT(CHILD(w))<MINIMUM_HEIGHT(w)) HEIGHT(CHILD(w)) = MINIMUM_HEIGHT(w);
  else if (HEIGHT(CHILD(w))>MAXIMUM_HEIGHT(w) && MAXIMUM_HEIGHT(w))
      HEIGHT(CHILD(w)) = MAXIMUM_HEIGHT(w);
  if (WIDTH(CHILD(w))<MINIMUM_WIDTH(w)) WIDTH(CHILD(w)) = MINIMUM_WIDTH(w);
  else if (WIDTH(CHILD(w))>MAXIMUM_WIDTH(w) && MAXIMUM_WIDTH(w))
      WIDTH(CHILD(w)) = MAXIMUM_WIDTH(w);

  XtConfigureWidget(w,X(w),Y(w),
                    WIDTH(CHILD(w))+2*BORDER_WIDTH(CHILD(w)),
                    HEIGHT(CHILD(w))+2*BORDER_WIDTH(CHILD(w)) ,
                    BORDER_WIDTH(w));
  set_sizing_handles_attributes(w);
}/*End of ChangeManaged*/



static XtArgsProc InsertChild(w,args,num_args)
    Widget	w;
    ArgList	args;
    Cardinal    *num_args;
/*Called every time a new direct child of the Selfmoving is created           */
{
    CompositeWidgetClass    my_superclass;
    HrSelfMovingWidget	    my_self = (HrSelfMovingWidget)XtParent(w);
    Arg			    my_args[2];
    int			    n;
    Dimension		    width,height;
    char		    *name;

    
    if (!HANDLE_CREATION(my_self)) {
       /*If it is not a handle we check the number of children*/
       (NB_CHILDREN(my_self))++;
       if (NB_CHILDREN(my_self)>1) {
          XtError("SelfMoving widget supports only one child!!");
       }
       CHILD(my_self)=w;
    }
    /*In every case we call the superclass' insert_child method*/
    (superclass->composite_class.insert_child) (w,args,num_args);
}/*End of Insert_Child*/

/*****************END OF METHODS***********************************************/




/*****************ACTIONS******************************************************/

static void select(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*This action is called by default, when the user clicks with button1 on the  */
/*Selfmoving without holding the <SHIFT> or the <CTRL> key                    */
{
     Position		    x_interne,y_interne;
     int		    n;
     Arg		    arg[2];
     HrAnyCallbackStruct    anycall;
     int		    x_mouse = event->xbutton.x;
     int		    y_mouse = event->xbutton.y;


     if (ACTIVE(w)) {
       if (!SELECTED(w)) {
         remove_all_brothers((Widget) w);
         add_brother((Widget) w);
         show_selected_state(w);
         anycall.reason = HrCR_SELECT;
         anycall.event = event;
         anycall.x = (Position) (X_CHILD(w) + x_mouse);
         anycall.y = (Position) (Y_CHILD(w) + y_mouse);
         XtCallCallbacks(w,XtNselectCallback,&anycall);
       }

       /*We need this for the motions*/
       n=0;
       XtSetArg(arg[n],XtNwidth,&PARENT_WIDTH(w));n++;
       XtSetArg(arg[n],XtNheight,&PARENT_HEIGHT(w));n++;
       XtGetValues(XtParent((Widget)w),arg,n);
       DELTA_X(w)=event->xbutton.x;
       DELTA_Y(w)=event->xbutton.y;
       MOTION(w) = FALSE;
       FIRST(w)  = TRUE;
       CLICKED(w)= TRUE;

       if (DOUBLE_CLICK(w)) {
         if ((event->xbutton.time-OLD_TIME(w)) < XtGetMultiClickTime(DPY(w))) {
	   anycall.reason = HrCR_DOUBLE_CLICK;
	   anycall.event = event;
	   anycall.x = (Position) (X_CHILD(w) + x_mouse);
	   anycall.y = (Position) (Y_CHILD(w) + y_mouse);
	   XtCallCallbacks(w,XtNdoubleClickCallback,&anycall);
           DOUBLE_CLICK(w) = FALSE;
         }
       }
       else DOUBLE_CLICK(w)=TRUE;
       OLD_TIME(w)=event->xbutton.time;
     }
}/*Enf of select*/




static void unselect(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*Called when the user clicks on the widget with the <CTRL> key pressed       */
{
  Arg	    		args[2];
  int	   	 	n,i,num_brothers;
  HrAnyCallbackStruct	anycall;
  int	    		x_mouse = event->xbutton.x;
  int	    		y_mouse = event->xbutton.y;
  Opaque    		context;
  HrSelfMovingWidget	self;
  

  if (SELECTED(w)) {
      anycall.reason = HrCR_UNSELECT;
      anycall.event = event;
      anycall.x = (Position) (X_CHILD(w) + x_mouse);
      anycall.y = (Position) (Y_CHILD(w) + y_mouse);
      init_context(BROTHER_LIST,&context,w,&num_brothers);
      for (i=0;i<num_brothers;i++){
	next_widget(&context,&self);
	show_normal_state(self);
	XtCallCallbacks(self,XtNunselectCallback,&anycall);
      }	
      delete_list(BROTHER_LIST,w);
  }
}/*End of unselect*/


static void extend_selection(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*Called when the user clicks on the widget with the <SHIFT> key pressed      */
{
  Arg		   	args[2];
  HrAnyCallbackStruct	anycall;
  Position	    	x,y;
  int		    	n;
  int		    	x_mouse = event->xbutton.x;
  int		    	y_mouse = event->xbutton.y;
   
  if (ACTIVE(w)) {
    anycall.event = event;
    anycall.x = (Position) (X_CHILD(w) + x_mouse);
    anycall.y = (Position) (Y_CHILD(w) + y_mouse);
    if (SELECTED(w)) {  
      remove_brother((Widget) w);
      anycall.reason = HrCR_UNSELECT;
      XtCallCallbacks(w,XtNunselectCallback,&anycall);
      show_normal_state(w);
    }
    else {
      add_brother((Widget) w);
      anycall.reason = HrCR_SELECT;
      XtCallCallbacks(w,XtNselectCallback,&anycall);
      show_selected_state(w);
    }
  }
}/*End of extend_selection*/


static void move(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*Called when the user is dragging the mouse                                  */
{
  Opaque		context;
  HrSelfMovingWidget	brother;
  Dimension		field_width  = w->core.width;
  Dimension		field_height = w->core.height;
  Boolean		ok,ok_move;
  int			prov_x,prov_y,x_field,y_field;
  int			min_x,max_x,min_y,max_y,n,i;
  static int    	old_x_field,old_y_field;
  HrAnyCallbackStruct	anycall;
  int			x_mouse = event->xbutton.x;
  int			y_mouse = event->xbutton.y;

  ok_move=TRUE;
  init_context(BROTHER_LIST,&context,w,&n);
  for (i=0;i<n;i++) {
      next_widget(&context,&brother);
      ok_move=ok_move && ALLOW_MOVE(brother);
  }
  if (CLICKED(w) && ok_move) {
      anycall.reason = HrCR_DURING_MOVE;
      anycall.event = event;
      anycall.x = (Position) (X_CHILD(w) + x_mouse);
      anycall.y = (Position) (Y_CHILD(w) + y_mouse);
      XtCallCallbacks(w,XtNduringMoveCallback,&anycall);

      prov_x = X(w) + SELECTION_BORDER(w) + event->xmotion.x - DELTA_X(w) +
               VERTICAL_LINE_OFFSET(w);
      prov_y = Y(w) + SELECTION_BORDER(w) + event->xmotion.y - DELTA_Y(w) +
               HORIZONTAL_LINE_OFFSET(w);

      if (GRID_ON(w)) {
          grid_position(&prov_x,GRID(w));
          grid_position(&prov_y,GRID(w));
      }
      x_field = prov_x -  VERTICAL_LINE_OFFSET(w);
      y_field = prov_y -  HORIZONTAL_LINE_OFFSET(w);

      give_bounds(w,x_field,y_field,&min_x,&max_x,&min_y,&max_y);
      if (min_x<0) {
                    if ((-min_x % GRID(w))==0)
                         x_field+=((-min_x/GRID(w)))*GRID(w);
                    else x_field+=((-min_x/GRID(w))+1)*GRID(w);
      }

      if (min_y<0) {
                    if ((-min_y % GRID(w))==0)
                         y_field+=((-min_y/GRID(w)))*GRID(w);
                    else y_field+=((-min_y/GRID(w))+1)*GRID(w);
      }
      /*If we don't cast PARENT_WIDTH in int the result will be surprising*/
      /*when max_x becomes negatif, especially in the logical expression*/
      if (max_x>(int)PARENT_WIDTH(w)) {
                x_field-=(((max_x-(int)PARENT_WIDTH(w))/GRID(w))+1)*GRID(w);
      }

      if (max_y>(int)PARENT_HEIGHT(w)) { 
                y_field-=(((max_y-(int)PARENT_HEIGHT(w))/GRID(w))+1)*GRID(w);
      }

      if (FIRST(w)) {   
          FIRST(w)--;
          MOTION(w)=TRUE;
          X_FIELD(w) = x_field;
          Y_FIELD(w) = y_field;
          old_x_field = x_field;
          old_y_field = y_field;
          draw_rectangles(w);
      }
      else { 
             if ((old_x_field!=x_field)||(old_y_field!=y_field)) {
               draw_rectangles(w);   
               X_FIELD(w) = x_field;
               Y_FIELD(w) = y_field;
               old_x_field = x_field;
               old_y_field = y_field;
               draw_rectangles(w);
             }
       }
  }
}/*End of move*/


static void end_move(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*end_move is called when the user releases the mouse.                       */
{   Position	x,y;
    int		x_mouse = event->xbutton.x;
    int		y_mouse = event->xbutton.y;
    HrAnyCallbackStruct	anycall;

   if (CLICKED(w)) {
     CLICKED(w) = FALSE;
     FIRST(w)++;
     if (MOTION(w)) {
       draw_rectangles(w);
       move_widgets(w);
       /*Hrchange: call with the callback struct..*/
       anycall.reason = HrCR_MOVE;
       anycall.event = event;
       anycall.x = (Position) (X_CHILD(w) + x_mouse);
       anycall.y = (Position) (Y_CHILD(w) + y_mouse);
       XtCallCallbacks(w,XtNmoveCallback,&anycall);
     }/*if*/
   }
}/*End of end_move*/


static void restack_windows(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
{
   int	    i;

   if (ACTIVE(w)) {
     XMapWindow(DPY(w),INVISIBLE_WINDOW(w));
     XRaiseWindow(DPY(w),INVISIBLE_WINDOW(w)); 
       for (i=0;i<NB_HANDLES;i++) {
           if (SIZING_HANDLES_STATE(w)[i]!=HrNO_HANDLE) {
                 XRaiseWindow(DPY(w),XtWindow(SIZING_HANDLES(w)[i])); 
            }
       }
   }
   else XUnmapWindow(DPY(w),INVISIBLE_WINDOW(w));
}/*End of restack_windows*/


static void erase(w,event)
    HrSelfMovingWidget	w;
    XEvent		*event;
/*This is an action that allows you to destroy a selected widget with a       */
/*combination of keys (Shift Control Button1Down). See Destroy method         */
{
  XtDestroyWidget(w);
}/*End of erase*/

/********************End of the actions****************************************/


/**************************Accelerators****************************************/

static void shiftbuttondown_acc(w, event)
  HrSelfMovingWidget    w;
  XEvent                *event;
/*This accelerator is called when the user intends to extend a selection      */
/*The code is the same as in buttondown_acc except that we don't have to      */
/*unselect the brothers.                                                      */
{
   if (ACTIVE(w)) {
     X_ACC(w) = event->xbutton.x;
     Y_ACC(w) = event->xbutton.y;
     MOTION_ACC(w) = FALSE;
     FIRST_ACC(w)  =TRUE;
     PLEASE_DO_MOTION(w) =   TRUE;
   }
}/*End of shiftbuttondown_acc*/



static void buttondown_acc(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*This is an accelerator called when the user clicks in the parent window     */
/*It just unselects all brothers.                                             */
{ 
  HrAnyCallbackStruct	anycall;
  int	    		i,num_brothers;
  int	    		x_mouse = event->xbutton.x;
  int	    		y_mouse = event->xbutton.y;
  Opaque    		context;
  HrSelfMovingWidget	self;
 
   if(ACTIVE(w)) {
     X_ACC(w) = event->xbutton.x;
     Y_ACC(w) = event->xbutton.y;
     MOTION_ACC(w) = FALSE;
     FIRST_ACC(w)  =TRUE; 
     PLEASE_DO_MOTION(w) =   TRUE;
     init_context(BROTHER_LIST,&context,w,&num_brothers);
     if (num_brothers>0) {
         anycall.reason = HrCR_UNSELECT;
         anycall.event = event;
         anycall.x = (Position) x_mouse;
         anycall.y = (Position) y_mouse;
         for (i=0;i<num_brothers;i++){
           next_widget(&context,&self);
           show_normal_state(self);    
           XtCallCallbacks(self,XtNunselectCallback,&anycall);
         }
         delete_list(BROTHER_LIST,w);	
     }
   }
}/*End of buttondown_acc*/



static void buttonmotion_acc(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*This is an accelerator called when the use is dragging the mouse in the     */
/*Selfmovingfather's window. It just has to draw an outlined rectangle        */
/*according to the mouse motion.                                              */
{
  HrAnyCallbackStruct	anycall;
  int			x_mouse = event->xbutton.x;
  int			y_mouse = event->xbutton.y;


  if (PLEASE_DO_MOTION(w)) {
     if (FIRST_ACC(w))  { 
         /*If we are called for the fisrt time just draw one rectangle        */
         FIRST_ACC(w) = !FIRST_ACC(w);
         MOTION_ACC(w)=TRUE;
         PROV_X_ACC(w) = event->xbutton.x;
         PROV_Y_ACC(w) = event->xbutton.y;
         give_rect(X_ACC(w),Y_ACC(w),PROV_X_ACC(w),PROV_Y_ACC(w),
                   &XR_ACC(w),&YR_ACC(w),&WIDTH_ACC(w),&HEIGHT_ACC(w));
         XDrawRectangle(DPY(w),XtWindow(XtParent(w)),GC(w),
                   XR_ACC(w),YR_ACC(w),(unsigned int)WIDTH_ACC(w),
                   (unsigned int)HEIGHT_ACC(w));
         }
     else 
     {   /*If it's not the first time we are called, we erase the old         */
         /*rectangle and draw a new one.                                      */
         give_rect(X_ACC(w),Y_ACC(w),PROV_X_ACC(w),PROV_Y_ACC(w),
                   &XR_ACC(w),&YR_ACC(w),&WIDTH_ACC(w),&HEIGHT_ACC(w));
         XDrawRectangle(DPY(w),XtWindow(XtParent(w)),GC(w),
                   XR_ACC(w),YR_ACC(w),(unsigned int)WIDTH_ACC(w),
                   (unsigned int)HEIGHT_ACC(w));


         PROV_X_ACC(w) = event->xbutton.x;
         PROV_Y_ACC(w) = event->xbutton.y;

         give_rect(X_ACC(w),Y_ACC(w),PROV_X_ACC(w),PROV_Y_ACC(w),
                   &XR_ACC(w),&YR_ACC(w),&WIDTH_ACC(w),&HEIGHT_ACC(w));
         XDrawRectangle(DPY(w),XtWindow(XtParent(w)),GC(w),
                   XR_ACC(w),YR_ACC(w),(unsigned int)WIDTH_ACC(w),
                   (unsigned int)HEIGHT_ACC(w));

     }/*end if first*/
  }
  anycall.reason = HrCR_ACC_MOVE;
  anycall.event = event;
  anycall.x = (Position) (x_mouse);
  anycall.y = (Position) (y_mouse);
  XtCallCallbacks(w,XtNaccMoveCallback,&anycall);
}/*End of buttonmotion_acc*/


static void buttonup_acc(w, event)
  HrSelfMovingWidget	w;
  XEvent		*event;
/*That is called when the user releases the mouse button after a click in     */
/*in the Selfmovingfather's window.                                           */
{
  Opaque		context;
  HrSelfMovingWidget	son;
  int			i,n;
  HrAnyCallbackStruct	anycall;
  int			x_mouse = event->xbutton.x;
  int			y_mouse = event->xbutton.y;	

  if (MOTION_ACC(w)) {
      MOTION_ACC(w) = FALSE;
    /*If the user really dragged the mouse, erase the old rectangle*/
    give_rect(X_ACC(w),Y_ACC(w),PROV_X_ACC(w),PROV_Y_ACC(w),
                 &XR_ACC(w),&YR_ACC(w),&WIDTH_ACC(w),&HEIGHT_ACC(w));
    XDrawRectangle(DPY(w),XtWindow(XtParent(w)),GC(w),
                 XR_ACC(w),YR_ACC(w),(unsigned int)WIDTH_ACC(w),
                 (unsigned int)HEIGHT_ACC(w));
    /*We select all selfmovings intersecting with the*/
    /*rectangle drawn by the user.*/
    init_context(SELFMOVING_LIST,&context,(Widget)w,&n);
    if (n>0) {
      for(i=0;i<n;i++) {
        next_widget(&context,&son);
        if(WIDTH_ACC(w)>(int) X_CHILD(son)-XR_ACC(w) &&
             (int) WIDTH(CHILD(son))>XR_ACC(w)-(int) X_CHILD(son)) {
          if(HEIGHT_ACC(w)>(int) Y_CHILD(son)-YR_ACC(w) &&
               (int) HEIGHT(CHILD(son))>YR_ACC(w)-(int) Y_CHILD(son)) {
            if(!SELECTED(son)) {
              add_brother((Widget) son);
              show_selected_state(son);
	      anycall.reason = HrCR_SELECT;
	      anycall.event = event;
	      anycall.x = (Position)(x_mouse);
	      anycall.y = (Position)(y_mouse); /*dh*/
              XtCallCallbacks(son,XtNselectCallback,&anycall);
            }
          }
        }
      }
    }
  }
  PLEASE_DO_MOTION(w) = FALSE;
}/*End of buttonup_acc*/


static void erase_acc(w, event)
  HrSelfMovingWidget   w;
  XEvent               *event;
/* This called when the user presses the <DEL> Key in the Selfmovingfather's  */
/* window.                                                                    */
/* What is happening here is pretty tricky. We call XtDestroyWidget for ecah  */
/* selected Selfmoving in the window. But XtDestroyWidget will only mark these*/ /* widgets as deleted. The destroy method will only be called when returning  */
/*from the event handler. That is when leaving this procedure. That's why     */
/*we can just loop throught all brothers without worrying about anything else.*/
{
  Opaque               context,context_2;
  int                  num_brothers, i;
  HrSelfMovingWidget   brother;
  Header	       widgets_to_destroy;

  init_context(BROTHER_LIST, &context, w, &num_brothers);
  for (i = 1; i <= num_brothers; i++) {
      next_widget(&context, &brother);
      XtDestroyWidget(brother);

  }
}

/**************************End of Accelerators*********************************/


/******************Actions of the handles for resizing*************************/

static void start_resize(w, event)
  Widget w;
  XEvent *event;
/*That action is called when the user clicks with MB1 in a handle that allows */
/*resizing.                                                                   */
{
  HrSelfMovingWidget	    self = (HrSelfMovingWidget) (XtParent(w));
  int			    n,i;
  Arg			    arg[5];
  Position		    x,y;

  
  CLICKED(self) = TRUE;
  FIRST_SIZING(self) = TRUE;
  DO_RESIZE(self) = FALSE;
  /*Finding which_handle is envolved*/
  for (i=0;i<NB_HANDLES;i++) {
      if (SIZING_HANDLES(self)[i]==w) {
         WHICH_HANDLE(self) = i;
         break;
      }
  }
      
  n=0;
  XtSetArg(arg[n],XtNx,&x);n++;
  XtSetArg(arg[n],XtNy,&y);n++;
  XtGetValues(w,arg,n);
  /*Offsets for changing coordinate from the system of the handle to the      */
  /*system of the father of self                                              */
  DELTA_X_SIZING(self) = X(self) + BORDER_WIDTH(self)+x;
  DELTA_Y_SIZING(self) = Y(self) + BORDER_WIDTH(self)+y;
  
  /*We need the size of the parent to put limitations on the new size         */
  n=0;
  XtSetArg(arg[n],XtNwidth,&PARENT_WIDTH(self));n++;
  XtSetArg(arg[n],XtNheight,&PARENT_HEIGHT(self));n++;
  XtGetValues(XtParent(XtParent((Widget)w)),arg,n);
}/*End of start_resize*/


static void do_resize(w, event)
  Widget w;
  XEvent *event;
/*Drawing the outlined rectangle for resizing                                 */
/*We suppose that minHeight and minWidth will never be bigger than the widget */
/*and that maxHeight and maxWidth will nerver be smaller than the widget      */
{
  HrSelfMovingWidget	self = (HrSelfMovingWidget) (XtParent(w));
  int			x_start_sizing,y_start_sizing;
  int			x_rect_sizing,y_rect_sizing;
  int			width_rect_sizing,height_rect_sizing;
  Boolean		draw_resize = TRUE;
  static Boolean	go_right;
  /*Coordinates of the mouse in the main window*/
  int			mouse_x = event->xmotion.x+DELTA_X_SIZING(self); 
  int			mouse_y = event->xmotion.y+DELTA_Y_SIZING(self); 
  HrAnyCallbackStruct	anycall;

  /*This code is really illisible... But what is common between all these*/
  /*cases????????????*/
  /*We must draw the rectangle showing the new size of the Selfmoving and*/
  /*erase the old one. This is pretty sophisticated since the user may*/
  /*specify maximun and minimum values for the width and the hight.*/
  /*Furthermore, the handle may be configured to allow only vertical or*/
  /*horizontal resizing!!! I am just happy that only one widget is resized*/
  /*and not all brothers; and we do not align on a grid value...*/

  switch (WHICH_HANDLE(self)) {
	case	north_west  : x_start_sizing = X_CHILD(self)+WIDTH(self);
                              y_start_sizing = Y_CHILD(self)+HEIGHT(self);
                                x_rect_sizing = 
                                   event->xmotion.x + DELTA_X_SIZING(self);
                              y_rect_sizing = 
                                 event->xmotion.y + DELTA_Y_SIZING(self);
                              width_rect_sizing = 
                                   x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                   y_start_sizing - y_rect_sizing;
                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }
			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    x_rect_sizing = x_start_sizing - MAXIMUM_WIDTH(self);
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			      }
			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_rect_sizing = x_start_sizing - MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }
			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing+ = x_rect_sizing;
				 x_rect_sizing = 0;
			      }
                              if (y_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing+ = y_rect_sizing;
				 y_rect_sizing = 0;
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_rect_sizing = X_CHILD(self);
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_rect_sizing = Y_CHILD(self);
				  height_rect_sizing = HEIGHT(self);
                              }

                              break;

	case	north	    : /*if ((FIRST_SIZING(self)) && 
				 ((mouse_x)>
                                  (WIDTH(self)/2 + X_CHILD(self)))) {
                                 go_right = TRUE;
                              }*/
			      /*We forget this idea and let the user resize   */
			      /*only to the right side of the widget          */
			      go_right = TRUE;
			      if (go_right) {
                                    x_start_sizing = (mouse_x-X_CHILD(self))*2 + 
                                                      X_CHILD(self);;
                                    y_start_sizing = Y_CHILD(self)+HEIGHT(self);
				    x_rect_sizing = X_CHILD(self);
				    y_rect_sizing = mouse_y;
				    width_rect_sizing = 
                                    x_start_sizing - x_rect_sizing;
                                    height_rect_sizing =
                                       y_start_sizing - y_rect_sizing; 

                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }

			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			            x_start_sizing = X_CHILD(self) + 
					    MAXIMUM_WIDTH(self);
			      }

			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_start_sizing =
					x_rect_sizing - MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }


			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_start_sizing>(int)PARENT_WIDTH(self)) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing = PARENT_WIDTH(self)- 
						    x_rect_sizing;
				 x_start_sizing = PARENT_WIDTH(self);
			      }
                              if (y_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing+ = y_rect_sizing;
				 y_rect_sizing = 0;
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_start_sizing = X_CHILD(self) + WIDTH(self);;
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_rect_sizing = Y_CHILD(self);
				  height_rect_sizing = HEIGHT(self);
                              }
                              }/*go_right*/	    
			       break;

	case	north_east  : x_start_sizing = mouse_x;
                              y_start_sizing = Y_CHILD(self)+HEIGHT(self);
                                x_rect_sizing = 
                                   X_CHILD(self);
                              y_rect_sizing = 
                                   mouse_y;
                              width_rect_sizing = 
                                   x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                   y_start_sizing - y_rect_sizing;
                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }
			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    x_start_sizing = x_rect_sizing + MAXIMUM_WIDTH(self);
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			      }
			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_start_sizing = x_rect_sizing + MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }
			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_rect_sizing = y_start_sizing - MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_start_sizing>(int)PARENT_WIDTH(self)) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing = PARENT_WIDTH(self) - 
					    x_rect_sizing;
				 x_start_sizing = PARENT_WIDTH(self);
			      }
                              if (y_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing+ = y_rect_sizing;
				 y_rect_sizing = 0;
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_start_sizing = X_CHILD(self) + WIDTH(self);
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_rect_sizing = Y_CHILD(self);
				  height_rect_sizing = HEIGHT(self);
                              }

                              break;

	case	east	    : x_start_sizing = mouse_x;
                              y_start_sizing = (mouse_y-Y_CHILD(self))*2
                                               + Y_CHILD(self);
			      x_rect_sizing = X_CHILD(self);
			      y_rect_sizing = Y_CHILD(self);
			      width_rect_sizing = 
                                    x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                       y_start_sizing - y_rect_sizing; 

                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }

			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			            x_start_sizing = X_CHILD(self) + 
					    MAXIMUM_WIDTH(self);
			      }

			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_start_sizing =
					x_rect_sizing + MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }


			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_start_sizing>(int)PARENT_WIDTH(self)) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing = PARENT_WIDTH(self)- 
						    x_rect_sizing;
				 x_start_sizing = PARENT_WIDTH(self);
			      }
                              if (y_rect_sizing>PARENT_HEIGHT(self)) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing = PARENT_HEIGHT(self) - 
							y_rect_sizing;
				 y_start_sizing = PARENT_HEIGHT(self);
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_start_sizing = X_CHILD(self) + WIDTH(self);;
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_start_sizing = Y_CHILD(self) + HEIGHT(self);
				  height_rect_sizing = HEIGHT(self);
                              }
			       break;

	case	south_east  : x_start_sizing = mouse_x;
                              y_start_sizing = mouse_y;
                                x_rect_sizing = 
                                   X_CHILD(self);
                              y_rect_sizing = 
                                   Y_CHILD(self);
                              width_rect_sizing = 
                                   x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                   y_start_sizing - y_rect_sizing;
                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }
			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    x_start_sizing = x_rect_sizing + MAXIMUM_WIDTH(self);
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			      }
			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_start_sizing = x_rect_sizing + MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }
			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_start_sizing>(int)PARENT_WIDTH(self)) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing = PARENT_WIDTH(self) - 
					    x_rect_sizing;
				 x_start_sizing = PARENT_WIDTH(self);
			      }
                              if (y_start_sizing>(int)PARENT_HEIGHT(self)) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing = PARENT_HEIGHT(self) -
							y_rect_sizing;
				 y_start_sizing = PARENT_HEIGHT(self);
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_start_sizing = X_CHILD(self) + WIDTH(self);
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_start_sizing = Y_CHILD(self);
				  height_rect_sizing = HEIGHT(self);
                              }

                              break;


	case	south	    : x_start_sizing = (mouse_x-X_CHILD(self))*2 + 
                                                      X_CHILD(self);;
                              y_start_sizing = mouse_y;
			      x_rect_sizing = X_CHILD(self);
			      y_rect_sizing = Y_CHILD(self);
			      width_rect_sizing = 
                                       x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                       y_start_sizing - y_rect_sizing; 

                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }

			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			            x_start_sizing = X_CHILD(self) + 
					    MAXIMUM_WIDTH(self);
			      }

			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_start_sizing =
					x_rect_sizing + MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }


			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_start_sizing>(int)PARENT_WIDTH(self)) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing = PARENT_WIDTH(self)- 
						    x_rect_sizing;
				 x_start_sizing = PARENT_WIDTH(self);
			      }
                              if (y_start_sizing>(int)PARENT_HEIGHT(self)) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing = PARENT_HEIGHT(self) -
							y_rect_sizing;
				 y_start_sizing = PARENT_HEIGHT(self);
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_start_sizing = X_CHILD(self) + WIDTH(self);;
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_start_sizing = Y_CHILD(self) + HEIGHT(self);
				  height_rect_sizing = HEIGHT(self);
                              }
			       break;

	case	south_west  : x_start_sizing = X_CHILD(self) + WIDTH(self);
                              y_start_sizing = mouse_y;
                                x_rect_sizing = 
                                   mouse_x;
                              y_rect_sizing = 
                                  Y_CHILD(self);
                              width_rect_sizing = 
                                   x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                   y_start_sizing - y_rect_sizing;
                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }
			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    x_rect_sizing = x_start_sizing - MAXIMUM_WIDTH(self);
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			      }
			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_rect_sizing = x_start_sizing - MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }
			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing+ = x_rect_sizing;
				 x_rect_sizing = 0;
			      }
                              if (y_rect_sizing>(int)PARENT_HEIGHT(self)) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing = PARENT_HEIGHT(self) - 
							y_rect_sizing;
				 y_rect_sizing = PARENT_HEIGHT(self);
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_rect_sizing = X_CHILD(self);
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_start_sizing = Y_CHILD(self) + HEIGHT(self);
				  height_rect_sizing = HEIGHT(self);
                              }

                              break;


	case	west	    : x_start_sizing = X_CHILD(self) + WIDTH(self);
                              y_start_sizing = (mouse_y-Y_CHILD(self))*2
                                               + Y_CHILD(self);
			      x_rect_sizing = mouse_x;
			      y_rect_sizing = Y_CHILD(self);
			      width_rect_sizing = 
                                    x_start_sizing - x_rect_sizing;
                              height_rect_sizing =
                                       y_start_sizing - y_rect_sizing; 

                              if ((width_rect_sizing<=0)||
                                  (height_rect_sizing<=0)) {
                                 draw_resize = FALSE;
				 DO_RESIZE(self) = FALSE;
				 x_rect_sizing = 0;
				 y_rect_sizing = 0;
				 width_rect_sizing = 0;
				 height_rect_sizing = 0;
				 break;
                              }

			      if ((MAXIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing>(int)MAXIMUM_WIDTH(self))) {
				    width_rect_sizing = MAXIMUM_WIDTH(self);
			            x_rect_sizing = x_start_sizing -
					    MAXIMUM_WIDTH(self);
			      }

			      if ((MAXIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing>(int)MAXIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MAXIMUM_HEIGHT(self);
				    height_rect_sizing = MAXIMUM_HEIGHT(self);
			      }
			      if ((MINIMUM_WIDTH(self)>0) && 
                                  (width_rect_sizing<(int)MINIMUM_WIDTH(self))) {
				    x_rect_sizing =
					x_start_sizing - MINIMUM_WIDTH(self);
				    width_rect_sizing = MINIMUM_WIDTH(self);
			      }


			      if ((MINIMUM_HEIGHT(self)>0) && 
                                  (height_rect_sizing<(int)MINIMUM_HEIGHT(self))) {
				    y_start_sizing = y_rect_sizing + MINIMUM_HEIGHT(self);
				    height_rect_sizing = MINIMUM_HEIGHT(self);
			      }
                              if (x_rect_sizing<0) {
                                 /*The user is draging outside the main window*/
				 width_rect_sizing + = x_rect_sizing;
				 x_rect_sizing = 0;
			      }
                              if (y_start_sizing>PARENT_HEIGHT(self)) {
                                 /*The user is draging outside the main window*/
				 height_rect_sizing = PARENT_HEIGHT(self) - 
							y_rect_sizing;
				 y_start_sizing = PARENT_HEIGHT(self);
			      }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrVERTICAL_SIZING_HANDLE) {
                                  x_rect_sizing = X_CHILD(self);
				  width_rect_sizing = WIDTH(self);
                              }
                              if (SIZING_HANDLES_STATE(self)[WHICH_HANDLE(self)]==
                                  HrHORIZONTAL_SIZING_HANDLE) {
                                  y_start_sizing = Y_CHILD(self) + HEIGHT(self);
				  height_rect_sizing = HEIGHT(self);
                              }
			       break;


    }/*switch*/
	    
  if (CLICKED(self)) {
     if (FIRST_SIZING(self)) {

        if (draw_resize) {
            FIRST_SIZING(self) = !FIRST_SIZING(self);
	    DO_RESIZE(self) = TRUE;

            XDrawRectangle(DPY(self),XtWindow(XtParent(self)),GC(self),
                         x_rect_sizing,y_rect_sizing,
                         width_rect_sizing,height_rect_sizing);
        } 
     }
     else {
	   if ( (x_rect_sizing!=OLD_X_RECT_SIZING(self)) ||
	        (y_rect_sizing!=OLD_Y_RECT_SIZING(self)) ||
		(width_rect_sizing!=OLD_WIDTH_RECT_SIZING(self)) ||
		(height_rect_sizing!=OLD_HEIGHT_RECT_SIZING(self)) ) {
          
                  XDrawRectangle(DPY(self),XtWindow(XtParent(self)),GC(self),
                         OLD_X_RECT_SIZING(self),OLD_Y_RECT_SIZING(self),
                         OLD_WIDTH_RECT_SIZING(self),OLD_HEIGHT_RECT_SIZING(self));    
           

           if (draw_resize) {
               DO_RESIZE(self) = TRUE;
               XDrawRectangle(DPY(self),XtWindow(XtParent(self)),GC(self),
                         x_rect_sizing,y_rect_sizing,
                         width_rect_sizing,height_rect_sizing);           
      
           }
           } /*Big if */

    }
           OLD_X_RECT_SIZING(self) = x_rect_sizing;
           OLD_Y_RECT_SIZING(self) = y_rect_sizing;
           OLD_WIDTH_RECT_SIZING(self) = width_rect_sizing;
           OLD_HEIGHT_RECT_SIZING(self) = height_rect_sizing;


	  /*Calling the XtNduringResizeCallback list*/  
	   anycall.reason = HrCR_DURING_RESIZE;
	   anycall.event = event;
	   anycall.x = (Position) mouse_x;
	   anycall.y = (Position) mouse_y ;
	   XtCallCallbacks(self,XtNduringResizeCallback,&anycall); 

  }

}/*End of do_resize*/


static void stop_resize(w, event)
  Widget w;
  XEvent *event;
/*That action is called when the user releases the mouse button after dragging*/
/*starting in a handle.                                                       */
{
  HrSelfMovingWidget	self = (HrSelfMovingWidget) (XtParent(w));
  HrAnyCallbackStruct	anycall;

  /*We just erase the old rectangle. Then we configure the Selfmoving and its */
  /*child. The XtConfigureWidget call will produce a call to the resize method*/
  /*which will resize the invisible_window and configure thehandles.          */
  if (DO_RESIZE(self)) {
    XDrawRectangle(DPY(self),XtWindow(XtParent(self)),GC(self),
		   OLD_X_RECT_SIZING(self),OLD_Y_RECT_SIZING(self),
		   OLD_WIDTH_RECT_SIZING(self),OLD_HEIGHT_RECT_SIZING(self));
  
    XtConfigureWidget(CHILD(self),
		    X(CHILD(self)),
		    Y(CHILD(self)),
		    OLD_WIDTH_RECT_SIZING(self)-2*BORDER_WIDTH(CHILD(self)),
		    OLD_HEIGHT_RECT_SIZING(self)-2*BORDER_WIDTH(CHILD(self)),
                    BORDER_WIDTH(CHILD(self)));

    X_CHILD(self) = OLD_X_RECT_SIZING(self);
    Y_CHILD(self) = OLD_Y_RECT_SIZING(self);

    XtConfigureWidget(self,
		    OLD_X_RECT_SIZING(self)-BORDER_WIDTH(self),
		    OLD_Y_RECT_SIZING(self)-BORDER_WIDTH(self),
		    OLD_WIDTH_RECT_SIZING(self),
		    OLD_HEIGHT_RECT_SIZING(self),
                    BORDER_WIDTH(self));

    anycall.reason = HrCR_RESIZE;
    anycall.event = event;
/*hrchange: this is not sure
    anycall.x = (Position) event->xmotion.x + DELTA_X_SIZING(self);
    anycall.y = (Position) event->xbutton.y + DELTA_Y_SIZING(self);
*/
    anycall.x = (Position) event->xmotion.x + X(w) + X(self);
    anycall.y = (Position) event->xbutton.y + Y(w) + Y(self);


    XtCallCallbacks(self,XtNresizeCallback,&anycall);    
  }   
}/*End of stop_resize*/


/**************************END OF HANDLE ACTIONS*******************************/




/*****************************UTILITY FONCTIONS********************************/

static void CvtStringToHrHandleType(args,nargs,fromVal,toVal)
    XrmValuePtr	    args, fromVal, toVal;
    int		    *nargs;
{
/*This function is a converter which is allowing the Intrinsics to            */
/*transform a string in a HrHandleType value                                  */

  static HrHandleType	result;
  char		*no_string,*regular_string,*sizing_string,*horizontal_string,
                *vertical_string;

  no_string	    = "NO_HANDLE";
  regular_string    = "REGULAR_HANDLE";
  sizing_string	    = "SIZING_HANDLE";
  horizontal_string = "HORIZONTAL_SIZING_HANDLE";
  vertical_string   = "VERTICAL_SIZING_HANDLE";

  if (*nargs!=0)
     XtWarning("String to HrHandleType needs no arguments");

  if (strcmp((char *)fromVal->addr,no_string)==0) {
     toVal->size=sizeof(HrHandleType);
     result=HrNO_HANDLE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,regular_string)==0) {
     toVal->size=sizeof(HrHandleType);
     result=HrREGULAR_HANDLE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,sizing_string)==0) {
     toVal->size=sizeof(HrHandleType);
     result=HrSIZING_HANDLE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,horizontal_string)==0) {
     toVal->size=sizeof(HrHandleType);
     result=HrHORIZONTAL_SIZING_HANDLE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,vertical_string)==0) {
     toVal->size=sizeof(HrHandleType);
     result=HrVERTICAL_SIZING_HANDLE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  XtStringConversionWarning(fromVal->addr,"XtRHrHandleType");
  toVal->addr = NULL;
  toVal->size = 0;
}/*End of CvtStringToHrHandleType*/



static void CvtStringToHrAlignType(args,nargs,fromVal,toVal)
    XrmValuePtr	    args, fromVal, toVal;
    int		    *nargs;
{
/*This function is a converter which is allowing the Intrinsics to            */
/*transform a string in a HrAlignType value                                   */
  static HrHandleType	result;
  char		*left_string,*right_string,*center_string,*top_string,
                *bottom_string,*center_vertical_string,*none_string;

  left_string		    =	"ALIGN_LEFT";
  right_string		    =	"ALIGN_RIGHT";
  center_string		    =	"ALIGN_CENTER";
  top_string		    =	"ALIGN_TOP";
  bottom_string		    =	"ALIGN_BOTTOM";
  center_vertical_string    =	"ALIGN_CENTER_VERTICAL";
  none_string		    =   "ALIGN_NONE";

  if (*nargs!=0)
     XtWarning("String to HrAlignType needs no arguments");

  if (strcmp((char *)fromVal->addr,left_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_LEFT;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,right_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_RIGHT;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,center_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_CENTER;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,top_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_TOP;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,bottom_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_BOTTOM;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,center_vertical_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_CENTER_VERTICAL;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  if (strcmp((char *)fromVal->addr,none_string)==0) {
     toVal->size=sizeof(HrAlignType);
     result=HrALIGN_NONE;
     toVal -> addr = (caddr_t) &result;
     return;
  }

  XtStringConversionWarning(fromVal->addr,"XtRHrAlignType");
  toVal->addr = NULL;
  toVal->size = 0;
}/*End of CvtStringToHrAlignType*/


static void align_selected_widgets(alignment, w)
  HrAlignType	    alignment;
  Widget	    w;
/*Aligns all selected Selfmovings in the same window. All these Selfmovings   */
/*are aligned on the first brother in the list, according to the value of     */
/*alignment. The user must select at first the widget he wants to use as      */
/*reference.                                                                  */
{
  int			i;
  Position		align_x,align_y,prov,center_x,center_y;
  HrSelfMovingWidget	brother;
  HrSelfMovingWidget	self;
  Arg			arg[5];
  Opaque		context;
  int			num_brothers;
    
  /*Utility list function intializing an opaque context for the widget list*/
  /*in the same window as w. Returns the number of widgets in that list.*/
  init_context(BROTHER_LIST,&context,w,&num_brothers);


  if (num_brothers>0) {
    next_widget(&context,&self);
    switch(alignment) {
	case	HrALIGN_LEFT :  
			        align_x = X_CHILD(self);
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
				    XtSetArg(arg[0],XtNxChild,align_x);
                                    XtSetValues(brother,arg,1);
				}
				break;

	case	HrALIGN_RIGHT :
				align_x = X_CHILD(self) + WIDTH(self);
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
                                    prov = align_x - WIDTH(brother);
				    XtSetArg(arg[0],XtNxChild,prov);
                                    XtSetValues(brother,arg,1);
				}
				break;

	case	HrALIGN_CENTER :center_x = X_CHILD(self)+WIDTH(self)/2; 
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
                                    align_x = center_x - WIDTH(brother)/2;
				    XtSetArg(arg[0],XtNxChild,align_x);
                                    XtSetValues(brother,arg,1);
				}
				break;

	case	HrALIGN_TOP :  
			        align_y = Y_CHILD(self) +
					HORIZONTAL_LINE_OFFSET(self);
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
				    prov = align_y - 
					HORIZONTAL_LINE_OFFSET(brother);
				    XtSetArg(arg[0],XtNyChild,prov);
                                    XtSetValues(brother,arg,1);
				}
				break;

	case	HrALIGN_BOTTOM :
				align_y = Y_CHILD(self) + HEIGHT(self);
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
                                    prov = align_y - HEIGHT(brother);
				    XtSetArg(arg[0],XtNyChild,prov);
                                    XtSetValues(brother,arg,1);
				}
				break;

	case	HrALIGN_CENTER_VERTICAL :
				center_y = Y_CHILD(self)+HEIGHT(self)/2; 
			        for (i=1;i<num_brothers;i++) {
				    next_widget(&context,&brother);
                                    align_y = center_y - HEIGHT(brother)/2;
				    XtSetArg(arg[0],XtNyChild,align_y);
                                    XtSetValues(brother,arg,1);
				}
				break;



        default		      : break;
        }
  }
}/*End of align_selected_widgets*/



static void map_sizing_handles(w, map)
  HrSelfMovingWidget	w;
  Boolean		map;
/*Maps and raises the handles, or unmaps them according to the boolean value  */
/*map.                                                                        */
{
   int		i;

   for (i=0;i<NB_HANDLES;i++) {
      if (SIZING_HANDLES_STATE(w)[i]!=HrNO_HANDLE && map) {
          if (XtIsRealized(SIZING_HANDLES(w)[i])) {
              XtMapWidget(SIZING_HANDLES(w)[i]);
              XRaiseWindow(DPY(w),XtWindow(SIZING_HANDLES(w)[i]));
          }
      }
      else {
          /*We call this to provoque an exposure event on the windows        */
          /*beneath the invisible window                                     */
          if (XtIsRealized(SIZING_HANDLES(w)[i])) {
              XLowerWindow(DPY(w),XtWindow(SIZING_HANDLES(w)[i]));           
              XtUnmapWidget(SIZING_HANDLES(w)[i]);
          }
      }
   }
}/*End of map_sizing_handles*/


static void set_reverse_video(w)
  Widget w;
{
/*Sets in reverse video the widget tree rooted at w by a recursive traversal  */
/*of the widget tree.                                                         */

  Arg	    arg[2];
  int	    n,i;
  Pixel	    foreground,background;

  /*Lets exchange the foreground and background colors ot the w widget        */
  n=0;
  XtSetArg(arg[n],XtNforeground,&foreground);n++;
  XtSetArg(arg[n],XtNbackground,&background);n++;
  XtGetValues(w,arg,n);

  n=0;
  XtSetArg(arg[n],XtNforeground,background);n++;
  XtSetArg(arg[n],XtNbackground,foreground);n++;
  XtSetValues(w,arg,n);

  /*If w is a composite widget,  we call recursivly the set_reverse_video*/
  /*for each child.*/
  if (XtIsComposite(w)) {
     for (i=0;i<((CompositeRec *)w)->composite.num_children;i++) {
         set_reverse_video(((CompositeRec *)w)->composite.children[i]);
     }/*for*/
  }/*if*/
}/*End of set_reverse_video*/


static void show_selected_state(w)
  HrSelfMovingWidget w;
/*Shows the widget w in his selected state                                    */
{
   /*If the XtNselectInReverseVideo resource is true call set_reverse_video*/
   if (SELECT_IN_REVERSE_VIDEO(w))
       set_reverse_video(CHILD(w));   
   /*Set the XtNselected resource to true*/
   SELECTED(w)=TRUE;
   /*We have to configure the Selfmoving, as the border_width may not be the*/
   /*same in the selected than in the unselected state.*/
   XtConfigureWidget((Widget)w,
                     X(w)+(SAVED_BORDER_WIDTH(w)-SELECTION_BORDER(w)),
                     Y(w)+(SAVED_BORDER_WIDTH(w)-SELECTION_BORDER(w)),
                     WIDTH(w),HEIGHT(w),
                     SELECTION_BORDER(w));  
   /*Make thehandles appear.*/
   map_sizing_handles(w,TRUE);
}/*End of show_selected_state*/


static void show_normal_state(w)
  HrSelfMovingWidget w;
/*Shows the widget w in his normal state                                      */
{
   /*See the commets above in  show_selected_state*/
   if (SELECT_IN_REVERSE_VIDEO(w))
       set_reverse_video(CHILD(w));
   SELECTED(w)=FALSE;
   XtConfigureWidget((Widget)w,
                     X(w)-(SAVED_BORDER_WIDTH(w)-SELECTION_BORDER(w)),
                     Y(w)-(SAVED_BORDER_WIDTH(w)-SELECTION_BORDER(w)),
                     WIDTH(w),HEIGHT(w),
                     SAVED_BORDER_WIDTH(w));  
   map_sizing_handles(w,FALSE);
}/*End of show_normal_state*/


static void set_sizing_handles_attributes(w)
  HrSelfMovingWidget w;
{
/*Don't use SetValues here because this would make a call to geometry         */ /*manager which would call again this function and so on.....                 */


   /*We put each handle, if it exists, at the right position*/
   XtConfigureWidget(SIZING_HANDLES(w)[0],0,0,
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[1],
       (WIDTH(w)/2)-(SIZING_HANDLES_WIDTH(w)/2)-SIZING_HANDLES_BORDER_WIDTH(w),
                    0,
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[2],
       WIDTH(w)-SIZING_HANDLES_WIDTH(w)-2*SIZING_HANDLES_BORDER_WIDTH(w),
                    0,
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[3],
       WIDTH(w)-SIZING_HANDLES_WIDTH(w)-2*SIZING_HANDLES_BORDER_WIDTH(w),
      (HEIGHT(w)/2)-(SIZING_HANDLES_WIDTH(w)/2)-SIZING_HANDLES_BORDER_WIDTH(w), 
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[4],
       WIDTH(w)-SIZING_HANDLES_WIDTH(w)-2*SIZING_HANDLES_BORDER_WIDTH(w),
       HEIGHT(w)-SIZING_HANDLES_WIDTH(w) -2*SIZING_HANDLES_BORDER_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[5],
      (WIDTH(w)/2)-(SIZING_HANDLES_WIDTH(w)/2)-SIZING_HANDLES_BORDER_WIDTH(w),
      HEIGHT(w)-SIZING_HANDLES_WIDTH(w) -2*SIZING_HANDLES_BORDER_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[6],
      0,
      HEIGHT(w)-SIZING_HANDLES_WIDTH(w) -2*SIZING_HANDLES_BORDER_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));

   XtConfigureWidget(SIZING_HANDLES(w)[7],
      0,
      (HEIGHT(w)/2)-(SIZING_HANDLES_WIDTH(w)/2)-SIZING_HANDLES_BORDER_WIDTH(w), 
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_WIDTH(w),
                    SIZING_HANDLES_BORDER_WIDTH(w));
}/*End of set_sizing_handles_attributes*/



static void add_brother(w)
  Widget w;
/*Adds a widget in the brother list. Exists for historicl reasons...          */
{ 
  insert_widget(BROTHER_LIST,w);
}/*End of add_brother*/



static void remove_brother(w)
  Widget w;
/*Removes a widget in the brother list. Exists for historicl reasons...       */
{
    delete_widget(BROTHER_LIST,w);
}/*End of remove_brothers*/


static void remove_all_brothers(w)
  Widget w;
{
/*Remove all brothers from the brother list and set the resource selected     */
/*to FALSE for all widgets except the calling one                             */
    int	    i;
    Arg	    args[1];
    Opaque  context;
    Widget  brother;
    int	    n;
    
    init_context(BROTHER_LIST,&context,w,&n);
    if (n>0) {
       XtSetArg(args[0],XtNselected,FALSE);
       for (i=0;i<n;i++) {
         next_widget(&context,&brother);
         if (brother!=w) 
            XtSetValues(brother,args,1);
       }/*for*/
       delete_list(BROTHER_LIST,w);
    }/*if*/
}/*End of remove_all_brothers*/


static Boolean test_move(w, x_field, y_field)
  HrSelfMovingWidget	w;
  Position		x_field;
  Position		y_field;
/*HrChange: This is no more used. Has been replace by give_bounds. Just keep  */
/*it, may be useful later.                                                    */
{
    int			delta_x,delta_y,i;
    HrSelfMovingWidget	brother;
    Arg			arg[2];
    XRectangle		rect,prov_rect;
    int			n; 
    Dimension		width,height;
    Boolean		ok=TRUE;
    Region		region;
    Opaque		context;
    

    n=0;
    XtSetArg(arg[n],XtNwidth,&width);n++;
    XtSetArg(arg[n],XtNheight,&height);n++;
    XtGetValues(XtParent((Widget)w),arg,n);

    rect.x=0;
    rect.y=0;
    rect.width=(short)width;
    rect.height=(short)height;
    region=XCreateRegion();
    XUnionRectWithRegion(&rect,region,region);


    init_context(BROTHER_LIST,&context,w,&n);
    for (i=0;i<n;i++) {
        next_widget(&context,&brother);
         if (brother==w) {
             prov_rect.x=x_field;
             prov_rect.y=y_field;
             prov_rect.width=w->core.width;
             prov_rect.height=w->core.height;
             if (XRectInRegion(region,(int) prov_rect.x,(int)prov_rect.y,
                   (unsigned int)prov_rect.width,(unsigned int)prov_rect.height)
                 !=RectangleIn) ok=FALSE;            
        }
        else { 
               delta_x = w->core.x - brother->core.x;
               delta_y = w->core.y - brother->core.y;
             prov_rect.x=x_field - delta_x;
             prov_rect.y=y_field - delta_y;
             prov_rect.width=brother->core.width;
             prov_rect.height=brother->core.height;
             if (XRectInRegion(region,(int) prov_rect.x,(int)prov_rect.y,
                  (unsigned int)prov_rect.width,(unsigned int)prov_rect.height)
                 !=RectangleIn) ok=FALSE;            
        }
     }
     return ok;
}/*test_move*/             



static void draw_rectangles(w)
  HrSelfMovingWidget w;
/*Draws outlined rectangles and eventually outlined lines according to the    */
/*XtNshowVerticalLine and XtNshowHorizontal line resources. This is called    */
/*by the move action while the user is moving some Selfmovings.               */
{
    int			delta_x,delta_y,i;
    HrSelfMovingWidget	brother;
    int			n;
    Opaque		context;

    /*We use the privte instance variables x_field and y_field, which give*/
    /*the position of the Selfmoving being dragged by the user.*/
    init_context(BROTHER_LIST,&context,w,&n);
    for (i=0;i<n;i++) {
        next_widget(&context,&brother);
        if (brother==w) {
            if (SHOW_HORIZONTAL_LINE(w)) {
                XDrawLine(DPY(w),XtWindow(XtParent(w)),GC(w),
                          0,Y_FIELD(w)+HORIZONTAL_LINE_OFFSET(w),
                          X_FIELD(w),Y_FIELD(w)+HORIZONTAL_LINE_OFFSET(w));
                XDrawLine(DPY(w),XtWindow(XtParent(w)),GC(w),
                          X_FIELD(w)+WIDTH(w),
                          Y_FIELD(w)+HORIZONTAL_LINE_OFFSET(w),
                          PARENT_WIDTH(w),Y_FIELD(w)+HORIZONTAL_LINE_OFFSET(w));
            }

            if (SHOW_VERTICAL_LINE(w)) {
                XDrawLine(DPY(w),XtWindow(XtParent(w)),GC(w),
                          X_FIELD(w)+VERTICAL_LINE_OFFSET(w),0,
                          X_FIELD(w)+VERTICAL_LINE_OFFSET(w),Y_FIELD(w));
                XDrawLine(DPY(w),XtWindow(XtParent(w)),GC(w),
                          X_FIELD(w)+VERTICAL_LINE_OFFSET(w),Y_FIELD(w)+HEIGHT(w),
                          X_FIELD(w)+VERTICAL_LINE_OFFSET(w),PARENT_HEIGHT(w));
            }
            XDrawRectangle(w->selfMoving.dpy,XtWindow(XtParent(w)),
                      w->selfMoving.gc,w->selfMoving.x_field,
                      w->selfMoving.y_field,w->core.width,w->core.height);
        }/*if (brother==w)*/
        else { 
	       delta_x = X_CHILD(w) - X_CHILD(brother);
	       delta_y = Y_CHILD(w) - Y_CHILD(brother);
               XDrawRectangle(w->selfMoving.dpy,XtWindow(XtParent(w)),
                              w->selfMoving.gc,
                              w->selfMoving.x_field-delta_x,
                              w->selfMoving.y_field-delta_y,
                              brother->core.width,brother->core.height);
        }/*else*/
    }/*for*/
}/*End of draw_rectangles*/             



static void move_widgets(w)
  HrSelfMovingWidget w;
/*Moves all brothers according to the values of X_FIELD and Y_FIELD.          */
{
    int			delta_x,delta_y,i;
    HrSelfMovingWidget	brother;
    int			n;
    Opaque		context;

    init_context(BROTHER_LIST,&context,w,&n);
    for (i=0;i<n;i++) {
        next_widget(&context,&brother);
        if (brother!=w) { 
           /*We first move only the brothers in order to preserve the core.x*/
           /*and core.y fields of w*/
	       delta_x = X_CHILD(w) - X_CHILD(brother);
	       delta_y = Y_CHILD(w) - Y_CHILD(brother);
               XtMoveWidget(brother,
                            X_FIELD(w)-delta_x-SELECTION_BORDER(brother),
                            Y_FIELD(w)-delta_y-SELECTION_BORDER(brother));
               X_CHILD(brother) = X(brother) + SELECTION_BORDER(brother);
               Y_CHILD(brother) = Y(brother) + SELECTION_BORDER(brother);
        }
     }
     XtMoveWidget(w,X_FIELD(w)-SELECTION_BORDER(w),
                    Y_FIELD(w)-SELECTION_BORDER(w));
     X_CHILD(w) = X(w) + SELECTION_BORDER(w);
     Y_CHILD(w) = Y(w) + SELECTION_BORDER(w);

}/*End of move_widgets*/             



static void give_bounds(w, p_x_field, p_y_field, min_x, max_x, min_y, max_y)
  HrSelfMovingWidget	w;
  int			p_x_field;
  int			p_y_field;
  int			*min_x, *max_x;
  int			*min_y, *max_y;
/*Calculates the extreme values of the position of the selected widgets       */
{
    int			x_field = (int)p_x_field;
    int			y_field = (int)p_y_field;
    int			delta_x,delta_y,i;
    HrSelfMovingWidget	brother;
    int			n;
    Opaque		context;

    init_context(BROTHER_LIST,&context,w,&n);
    for (i=0;i<n;i++) {
        next_widget(&context,&brother);
        if (brother==w) {
            if (i==0) { *min_x = x_field;*min_y=y_field;
                        *max_x = x_field + WIDTH(w);
                        *max_y = y_field + HEIGHT(w);
            }/*if (i==0)*/
            else {
                   if (x_field<*min_x) *min_x = x_field;
                   if (y_field<*min_y) *min_y = y_field;
                   if (x_field + WIDTH(w)>*max_x)  
                              *max_x = x_field + WIDTH(w);
                   if (y_field + HEIGHT(w)>*max_y)  
                              *max_y = y_field + HEIGHT(w);
            }/*else*/
        }/*if (brother==w)*/
        else { 
               delta_x = X(w) - X(brother);
               delta_y = Y(w) - Y(brother);

               if (i==0) { *min_x = x_field - delta_x;
                           *min_y = y_field - delta_y;
                           *max_x = *min_x + WIDTH(brother);
                           *max_y = *min_y + HEIGHT(brother);
               }
               else {
                   if (x_field - delta_x<*min_x) *min_x = 
                             x_field - delta_x;
                   if (y_field -delta_y<*min_y)  *min_y = 
                             y_field - delta_y;
                   if (x_field-delta_x + WIDTH(brother)>*max_x)  
                              *max_x = x_field-delta_x + WIDTH(brother);
                   if (y_field-delta_y + HEIGHT(brother)>*max_y)  
                              *max_y = y_field - delta_y + HEIGHT(brother);
              }
        }/*else if (brother==w)*/
    }/*for*/
}/*End of give_bounds*/             





static void grid_position(pos, grid)
  int *pos;
  int grid;
/*Calculates the nearest location on the grid for pos                         */
{
   int	    prov=(int)*pos;

   if ((prov % grid)>(grid/2))  prov=(((prov/grid)+1)*grid);
   else prov= ((prov/grid)*grid);

   *pos=prov;
}/*End of grid_position*/



static void  give_rect(x1,y1,x2,y2,xr,yr,width,height)
  int x1,y1,x2,y2,*xr,*yr,*width,*height;
/*Cette procedure donne un rectangle par son coin superieur gauche, sa largeur*/
/*, sa hauteur en partant de ses 4 sommets                                    */
{
  if (x2>x1)
  {
   if (y2>y1)
     { *xr = x1;*yr = y1; *width = x2-x1; *height = y2-y1;}
   else
     { *xr = x1; *yr = y2; *width = x2-x1; *height = y1-y2; }
  }
  else
  {
    if (y2>y1)
      { *xr = x2; *yr = y1; *width = x1-x2; *height = y2-y1; }
    else
      { *xr = x2; *yr = y2; *width = x1-x2; *height = y1-y2; }  
  }
}/*End of give_rect*/

static Position give_x_from_x_child(w)
    HrSelfMovingWidget  w;
{
     return  X_CHILD(w) - BORDER_WIDTH(w);
}

static Position give_y_from_y_child(w)
    HrSelfMovingWidget  w;
{
     return  Y_CHILD(w) - BORDER_WIDTH(w);
}

static Position give_x_child_from_x(w)
    HrSelfMovingWidget  w;
{
     return  X(w) + BORDER_WIDTH(w);
}

static Position give_y_child_from_y(w)
    HrSelfMovingWidget  w;
{
     return  Y(w) + BORDER_WIDTH(w);
}

/*************************LIST FUNCTIONS***************************************/

/*This is for testing purpose...
#define XtParent(w) (Widget)((w/10)*100)
#define Widget  int
End of test*/

typedef struct _widgetbox *WidgetBoxPtr; 
typedef struct _widgetbox {
    Widget		w;
    WidgetBoxPtr	next;
} WidgetBox;


typedef struct _widgetlistbox *WidgetListBoxPtr;
typedef struct _widgetlistbox {
    int			nb_widgets;
    Widget		parent;
    WidgetBoxPtr	first_widget;
    WidgetListBoxPtr	next;
} WidgetListBox;

struct _header {
    int			nb_lists;
    WidgetListBoxPtr	first_list;
};

typedef WidgetBoxPtr WidgetListContext;



static void new_header(header)
	Header	*header;
{
   (*header) = (Header) malloc(sizeof(struct _header));
   (*header) -> nb_lists = 0;
   (*header) -> first_list = NULL;
}


static void search_list(header,w,list)
    Header		header;
    Widget		w;
    WidgetListBoxPtr	*list;
/*Looking for the header of the widget list for the display of w              */
/*Returns NULL if w is the first widget in his window                         */
{
  Widget parent = XtParent(w);
  int	 i;

  *list = header->first_list;
  for (i=1;i <= header->nb_lists;i++) {
      if ((*list)->parent == parent) break;
      else *list = (*list)->next;
  }
}

static void insert_in_widget_list(list_header,w)
/*Insert a new widget at the end of the widget list whose header is           */
/*list_header                                                                 */
    WidgetListBoxPtr	list_header;
    Widget		w;
{
   WidgetBoxPtr	    prov;
   int		    i;

   prov = list_header -> first_widget;
   if (prov == NULL) {
        prov  = (WidgetBoxPtr) malloc(sizeof(WidgetBox));
        prov -> w = w;
        prov -> next = NULL;
        list_header -> first_widget = prov;
   }
   else {
      for (i=1;i<list_header->nb_widgets;i++) {
          prov = prov -> next;
      }
      prov -> next = (WidgetBoxPtr) malloc(sizeof(WidgetBox));
      prov = prov -> next;
      prov -> w = w;
      prov -> next = NULL;
   }
   (list_header -> nb_widgets)++;
}


static void insert_widget(header,w)
     Header header;
     Widget w;
{
    WidgetListBoxPtr	list;

    /*finding the header of the widget list having same parent as w*/
    search_list(header,w,&list);
    if (list == NULL) {
        /*If there is no list for that parent, create a new list and put*/
        /*it at first place in the list of headers*/
        list = (WidgetListBoxPtr) malloc(sizeof(WidgetListBox));
        list -> nb_widgets = 0;
	list -> parent = XtParent(w);
        list -> first_widget = NULL;
        list -> next = (header) -> first_list;
        (header) -> first_list = list;
        ((header) -> nb_lists)++;
    }    
    insert_in_widget_list(list,w);
}

static void delete_list_header(header,list)
/*This is an utility function to delete a list header                         */
    Header		header;
    WidgetListBoxPtr	*list;
{
    WidgetListBoxPtr    prov_list,prov_list_2;
    int			i;

      if (header->first_list == (*list)) {
         prov_list = (*list);
         header->first_list = prov_list->next;
         free(prov_list);
      }
      else {
             prov_list = header->first_list;
	     for (i=1;i<header->nb_lists;i++) {
                 if (prov_list->next == (*list)) break;
                 prov_list = prov_list -> next;
             }
             prov_list_2 = prov_list -> next;
             prov_list -> next = prov_list_2 -> next;
             free(prov_list_2);
      }
      (header->nb_lists)--;                 
}


static void delete_widget_in_list(header,list,w)
/*This is an utility function to delete one widget in a given list            */
    Header		header;
    WidgetListBoxPtr	*list;
    Widget		w;
{
    int			i;
    WidgetBoxPtr	prov,prov_2;
    WidgetListBoxPtr    prov_list,prov_list_2;

    if ( (*list)->first_widget -> w == w) {
       prov = (*list)->first_widget;
       (*list)->first_widget = prov->next;
       free(prov);
    }
    else {
           prov = (*list)->first_widget;
           for (i=1;i<(*list)->nb_widgets;i++) {
               if (prov->next->w == w) break;
               prov=prov->next;
           }
           if (prov->next == NULL) {
              XtError("Delete widget in list: Widget not found");
           }
           prov_2 = prov->next;
           prov->next = prov_2->next;
           free(prov_2);
    }
    ((*list)->nb_widgets)--;
    if (((*list)->nb_widgets) == 0) {
      /*Let's delete the header of this empty list*/
      delete_list_header(header,list);
    }
}



static void delete_widget(header,w)
     Header header;
     Widget w;
{
    WidgetListBoxPtr	list;

    search_list(header,w,&list);
    if (list == NULL) {
       XtError("delete_widget: You tried to delete a widget that was not in the widget list");
    }
    else {
           delete_widget_in_list(header,&list,w);
    }          
}


static void delete_list(header,w)
    /*Deletes the entire list containing w*/
     Header header;
     Widget w;
{
    WidgetListBoxPtr	list;
    WidgetBoxPtr	prov,prov_2;
    int			i;

    search_list(header,w,&list);
    if (list == NULL) {
       XtError("delete_list: You tried to delete a widget that was not in the widget list");
    }
    else {
	   prov_2 = list->first_widget;
	   for (i=1;i<list->nb_widgets;i++) {
                prov = prov_2;
		prov_2 = prov->next;
                free(prov);
	   }
           free(prov_2);
           delete_list_header(header,&list);
    }    
}

static int nb_widgets_in_list(header,w)
/*Returns the number of widgets in the list                                   */
     Header header;   
     Widget w;	
{
    WidgetListBoxPtr	    list; 

    search_list(header,w,&list);
    if (list==NULL) {
       return 0;
    }
    else {
       return (list -> nb_widgets);
    }
}
  
static void first_widget_in_list(header,widget,w)
/*Returns the first widget in the widget's list                               */
     Header header;
     Widget widget;
     Widget *w;	
{
    WidgetListBoxPtr	    list; 

    search_list(header,widget,&list);
    if (list==NULL) XtError("The list is empty...");
    else *w=(list->first_widget)->w;
}

static void init_context(header,context,w,nb_widgets)
   Header		    header;
   WidgetListContext	    *context;
   Widget		    w;
   int			    *nb_widgets;
{
     WidgetListBoxPtr	    list; 

    search_list(header,w,&list);
    if (list==NULL) {
       (*nb_widgets) = 0;
       (*context) = NULL;
    }
    else {
       (*nb_widgets) = list -> nb_widgets;
       (*context) = list -> first_widget;
    }
}    

static void next_widget(context,w)
   WidgetListContext	    *context;
   Widget		    *w;
{
    if ( (*context) != NULL) {
      *w = ((WidgetBoxPtr) (*context)) -> w;
      (*context) = (*context) -> next;
    }
    else
      XtError("Error in next_widget. There are no more Widgets...");
}
     

static void show_lists(header)
   Header header;
{
   WidgetListBoxPtr    prov_list_header;
   WidgetBoxPtr	       prov_widget;
   int	    i,j;

   printf("Il y a %d listes\n",header->nb_lists);
   prov_list_header = header -> first_list;
   for (i=1;i<=header->nb_lists;i++) {
       printf("Liste no %d , il y a %d widgets\n",i,
                 prov_list_header->nb_widgets);
       printf("     ");
       prov_widget = prov_list_header -> first_widget;
       for (j=1;j<=prov_list_header -> nb_widgets;j++) {
            printf("%d",prov_widget->w);
            prov_widget = prov_widget -> next;
       }
       printf("\n"); 
      prov_list_header = prov_list_header -> next;
   }
}
