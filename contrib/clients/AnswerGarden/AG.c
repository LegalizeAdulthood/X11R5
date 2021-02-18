/**********************************************************

  The ANSWER GARDEN PROJECT

  AG.C  Answer Garden main module

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of MIT not be used in
advertising or publicity pertaining to distribution of the software
without specific, written prior permission.

Answer Garden is a trademark of the Massachusetts Institute of
Technology.  All rights reserved.

  AnswerGarden.c

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu

  RELEASE .79           OCTOBER 5, 1991            MIT CONTRIB TAPE
  

  system dependencies:
    1.  Program uses the "system" call to send mail.  See mailer types.
        As of 12/90, the system handles MH and /bin/mail.
    2.  Program uses gettimefday to get system time.

  To compile Answer Garden to disable editing, give cc the flag -DNO_EDIT.

  
  NOTE of personal disclaimer (READ THIS BEFORE READING THE CODE):
     Like most things that tend to smell after too long a time,
     code ought to have an expiration date.  This does.
     If you are reading this code more than 3 months
     its release date, don't bother.  I figured better ways
     of doing things most likely, and you should get the newer code.
     Contact me at the above email addresses.  

     (Thanks to Stephen Gildea for the idea.)

  Note of personal interest:
     Notwithstanding the above, I am extremely interested in bugs,
     suggestions, and general comments you might have about this code.
     I am particularly interested in the uses you (yes, you) may make
     of this code, so let me know...

                              Mark Ackerman
			      ("Ack")

  TO GET NEW RELEASES AND BUG FIXES
     Send your email address to ag-request@xv.mit.edu.

  TO GET NEW X INFORMATION NODES
     Send your email address to x-request@xv.mit.edu.
     



  


***********************************************************/

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include "AG.h"
#include "Knedit.h"
#include "Layout.h"
#include "Grapher.h"
#include "FuncObj.h"



#define AGRootGrapher "RootGrapher"
#define AGRoot        "Root"
#define AGMissingNode "MissingNode"
#define MissingFileMessage \
"Unable to open this node's physical file.  \n\
Please contact your site administrator. \n"

void Create_Node_Callback(), Create_Null_Node();
void Internal_Close_Callback();
Widget AG_Create_Node();
void Edit_Node_Callback();
void Show_Root_Grapher();
void Show_Root();
void Dynamic_Internal_Callback();
void Dynamic_Create_Node_Callback();
Boolean Util_Get_And_Check_Timestamp();
extern char *Util_Find_Shell_Name();

extern GlobalInfo global_info;
extern SortaObj *sorta_objs[];
extern UserSettings user_settings;
extern void Util_Open_ButtonBox();
extern void User_History_List_Add_Member();
extern void Edit_Initialize();

/****************************

  SBROWSER ROUTINES

****************************/


void General_Crush_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
    char *label;
    AGNodeTypes type;
    char *location;
    char buffer[MaxString];
    NodeInfoPtr node_info;
    Widget shell;

    if (call_data == NULL)
      return;

    if ((NodeService_Request_By_Name((char * )call_data,&node_info,
				     &label,&type,&location) >= 0))
	{
	  if ((shell = NodeService_Get_Shell(node_info)) != NULL)
	    Internal_Close_Callback(shell,(XtPointer)node_info,NULL);
	}
}	  


static void SBrowser_Destroy_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  /* not needed for now */
}


static void SBrowser_Back_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
    Widget knedit = (Widget)client_data;
    Dimension shown_height, height;
    Widget inner_form;
    Widget viewport;
    Position y;
    Position goto_y;

    Stat_Button_Hit(w);

    inner_form = XtParent(knedit);

    XtVaGetValues(knedit,XtNtotalHeight,(XtArgVal)&shown_height,
		  XtNheight,(XtArgVal)&height,NULL);
    XtVaGetValues(inner_form,XtNy,(XtArgVal)&y,NULL);

    goto_y = y+shown_height;
    if ( goto_y > 0)
      goto_y = 0;
    XtVaSetValues(inner_form,XtNy,(XtArgVal)goto_y, NULL);
    
}

static void SBrowser_Forward_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
    Widget knedit = (Widget)client_data;
    Dimension shown_height, height;
    Widget inner_form;
    Widget viewport;
    Position y;
    int temp;

    Stat_Button_Hit(w);

    inner_form = XtParent(knedit);

    XtVaGetValues(knedit,XtNtotalHeight,(XtArgVal)&shown_height,
		  XtNheight,(XtArgVal)&height,NULL);
    XtVaGetValues(inner_form,XtNy,(XtArgVal)&y,NULL);

    temp = y-shown_height;
    if ( (int)(temp + height) > 0 )
	XtVaSetValues(inner_form,XtNy,(XtArgVal)temp, NULL);
    

}


static int Geometry_Get_Stack(w,node_info)
     Widget w;
     NodeInfo *node_info;
{
  Widget shell;
  char name_buffer[MaxString];
  char class_buffer[MaxString];
  char resource_type[MaxString];
  XrmValue value;
  XrmValue to_value;
  Boolean rc;
  int j;

  if (global_info.n_stacks <= 0)
    return(0);  /* no stacks anyways */
  
  AGstrcpy(name_buffer,"AnswerGarden.");
  AGstrcat(name_buffer,NodeService_Get_Node_Name(node_info));
  AGstrcat(name_buffer,".");
  AGstrcat(name_buffer,XtNstack);
  AGstrcpy(class_buffer,"AnswerGarden.");
  AGstrcat(class_buffer,NodeService_Get_String_From_Type(
		       NodeService_Get_Type(node_info)));
  AGstrcat(class_buffer,".");
  AGstrcat(class_buffer,XtCStack);
  rc = XrmGetResource(XtDatabase(XtDisplay(w)),name_buffer,class_buffer,
		      resource_type,&value);
  if (!rc)
    return(0);
  else
    {
      j = atoi(value.addr);
      if (j > 0)
	return(j);
      else
	return(0);
    }
}  


#define DELTA 25
#define VIEWPORT_HEIGHT_FUDGE 50


void AG_Geometry(shell,node_info)
     Widget shell;
     NodeInfo *node_info;
{
  int stack_num;
  Position x;
  Position y;
  static  char geometry_string[MaxString];

  if (global_info.n_stacks <= 0)
    return;  /* no stacks anyways */

  stack_num = Geometry_Get_Stack(shell,node_info);

  if (stack_num == 0)
    return; /* not set geometry */

  AG_Stack_Add_Shell(global_info.stacks[stack_num-1],shell,&x,&y);
  sprintf(geometry_string,"+%d+%d",x,y);
  XtVaSetValues(shell,XtNgeometry,(XtArgVal)geometry_string,NULL);

#ifdef OLD
  static Position curx = 0;
  static Position cury = 0;
  static Boolean go_right = True;
  static Boolean go_down = True;
  
  if (go_right)
    {
      if (curx+FUDGE_WIDTH >=
	  WidthOfScreen(XtScreen(first_node)))
	go_right = False;
    }
  else if (curx - DELTA <= 0)
    go_right = True;
  
  if (go_down)
    {
      if (cury+FUDGE_HEIGHT >=
	  HeightOfScreen(XtScreen(first_node)))
	go_down = False;
    }
  else if (cury - DELTA <= 0)
    go_down = True;
  
  curx += (go_right) ? DELTA : -DELTA;
  cury += (go_down) ? DELTA : -DELTA;

#endif
}



#ifdef OLDAPI
Widget Open_SBrowser(junk_w,client_data,call_data)
     Widget junk_w;
     XtPointer client_data;
     XtPointer call_data;
#else
Widget Open_SBrowser(junk_w,node_info,geometry,misc_info,num_info)
     Widget junk_w;
     NodeInfoPtr node_info;
     String geometry;
     String *misc_info;
     int num_info;
#endif
{
    Widget shell;
    Widget knedit;
    Widget box;
    Widget command[5];
    Widget form;
    Arg arglist[10];
    char filestring[MaxString];

    Dimension width;
    Dimension total;
    Dimension hspace;
    WidgetClass create_class;
    int i;
    Widget viewport;
    Widget inner_form;
    Dimension total_height;
    Dimension height;
    Boolean force_size;
    char name_string[MaxString];
    char *filename;
    char msg[MaxString];
    int n;
    char *node_name;
#ifdef OLDAPI
    NodeInfoPtr node_info = (NodeInfoPtr) client_data;
#endif

    filename = NodeService_Get_Location(node_info);

    if (filename == NULL)
      {
	 XtWarning("null name passed to discussion node.  Continuing...");
	 return(NULL);
      }

    Form_Filename(filestring,filename);
    if (!NodeService_Open_File(node_info,filestring))
      {
	sprintf(msg,"Error in opening node at location %s.",filename);
	XtWarning(msg);
	XtWarning("Continuing...");
	NodeService_Close_File(node_info);
	return(NULL);
      }


    create_class = topLevelShellWidgetClass;


    n=0;
    XtSetArg(arglist[n],XtNinput,(XtArgVal)True);n++;
    XtSetArg(arglist[n],XtNallowShellResize,(XtArgVal)True);n++;
    XtSetArg(arglist[n],XtNtitle,(XtArgVal)NodeService_Get_Label(node_info));
    n++;

#ifdef OLDAPI
    if ((char *)call_data != NULL)
      {
	XtSetArg(arglist[n],XtNgeometry,(XtArgVal)call_data);n++;
      }
#else
    if (geometry != NULL)
      {
	XtSetArg(arglist[n],XtNgeometry,(XtArgVal)geometry);n++;
      }
#endif


    /* Make it a transient if this is not the Root and the
       user has selected auto-placement */


    shell = XtCreatePopupShell(NodeService_Get_Node_Name(node_info),
			       create_class,
			       global_info.main_shell,
			       arglist,n);

    form =  XtCreateManagedWidget("sbrowserForm",formWidgetClass,
			  shell, NULL,0);

    Util_Open_ButtonBox(form,&box,command,NULL,NULL,0);
    total = 0;
    for (i=0;i<5;i++)
      {
	XtVaGetValues(command[i],XtNwidth,(XtArgVal)&width,NULL);
	total = total+width+4;
      }
    width = total;  /* for use below */


    XtAddCallback(command[2],XtNcallback,Dynamic_Question_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[1],XtNcallback,Dynamic_Help_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[0],XtNcallback,Dynamic_Close_Callback,
		  (XtPointer)node_info);

    viewport = XtVaCreateManagedWidget("sbrowserViewport",layoutWidgetClass,
				       form,
				       XtNheight, (XtArgVal)50,
				       XtNborderWidth, (XtArgVal)0,
				       NULL);


      /* Todo: Get rid of this by making knedit subclass of form */
    inner_form =  XtVaCreateManagedWidget("sbrowserInnerForm",formWidgetClass,
					  viewport,
 					  XtNborderWidth, (XtArgVal)0,
					  NULL);

    node_name = NodeService_Get_Node_Name(node_info);
    knedit = XtVaCreateManagedWidget("sbrowserKnedit",
				     kneditWidgetClass,inner_form,
				     XtNnodeName, (XtArgVal)node_name,
				     XtNfilename, 
				       (XtArgVal)NodeService_Get_Text(
							 node_info),
				     XtNwidth, (XtArgVal)width,
				     XtNborderWidth, (XtArgVal)0,
				     XtNeditMode, 
				       (XtArgVal)global_info.edit_mode,
				     XtNfuncObj,
				       (XtArgVal)global_info.global_func_obj,
				     NULL);

    XtAddCallback(knedit,XtNinternalButtonCallback,Create_Node_Callback,
		  (XtPointer)knedit); 
    XtAddCallback(knedit,XtNeditCallback,Edit_Node_Callback,
		  (XtPointer)knedit);
    XtAddCallback(knedit,XtNdynamicCallback,Dynamic_Internal_Callback,
		  (XtPointer)knedit);
    XtAddCallback(knedit,XtNdestroyCallback, SBrowser_Destroy_Callback, 
		  (XtPointer)node_info);


    XtVaGetValues(knedit,XtNtotalHeight, (XtArgVal)&total_height,
		         XtNheight, (XtArgVal) &height,
		         XtNforceSize, (XtArgVal) &force_size,
		         NULL);
    
    if (height > total_height)
	{
	    XtSetSensitive(command[3],True);
	    XtSetSensitive(command[4],True);
	    XtAddCallback(command[3],XtNcallback,SBrowser_Forward_Callback,
			  (XtPointer)knedit);
	    XtAddCallback(command[4],XtNcallback,SBrowser_Back_Callback,
			  (XtPointer)knedit);
	}

    if (force_size)
      height = total_height + VIEWPORT_HEIGHT_FUDGE;

      /* Get viewport to be right size.  Assumption is that knedit
	 reports its total height != height if paging is desired */
    XtVaSetValues(viewport,XtNheight, (XtArgVal)total_height, 
		  XtNwidth, (XtArgVal)width, NULL);

    
    XtVaSetValues(box,XtNfromVert,(XtArgVal)viewport,NULL);

    AG_Geometry(shell,node_info);
    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);


    return(shell);
}

  /* For historical continuity:  the singular remains of
     piggie.c. */
static void bye_bye(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
   Stat_Button_Hit(w);
   exit(0);
}

Widget Open_Control(parent,node_name,call_data)
     Widget parent;
     char *node_name;
     XtPointer call_data;
{
    Widget knedit;
    Widget box;
    Widget command;
    char filename[MaxString];
    char msg[MaxString];
    Widget layout;
    Widget form;
    Arg arglist[10];
    int n;
    NodeInfoPtr node_info;
    char *label;
    char *location;
    AGNodeTypes type;
    Widget shell;
    Widget buttonbox;
    Widget buttons[5];

    if (NodeService_Request_By_Name(node_name,&node_info,
				    &label,&type,&location) < 0)
      {
	sprintf(msg,"unable to locate %s in node service",node_name);
	XtWarning(msg);
	return(NULL);
      }

    n=0;
    XtSetArg(arglist[n],XtNinput,(XtArgVal)True);n++;
    XtSetArg(arglist[n],XtNallowShellResize, (XtArgVal)True);n++;
    /* title will get set in the resource file to "Answer Garden" */

    if ((char *)call_data != NULL)
      {
	XtSetArg(arglist[n],XtNgeometry,(XtArgVal)call_data);n++;
      }
    
    shell = XtCreatePopupShell((char *)node_name,applicationShellWidgetClass,
			       global_info.main_shell,
			       arglist,n);


    box =  XtCreateManagedWidget("control",formWidgetClass,
			  shell, NULL,0);

    Form_Filename(filename,location);
    if (NodeService_Open_File(node_info,filename))
      XtSetArg(arglist[0],XtNfilename,
	       (XtArgVal)NodeService_Get_Text(node_info));
    else
      XtSetArg(arglist[0],XtNfilename,(XtArgVal)MissingFileMessage);

    knedit = XtCreateManagedWidget("controlKnedit",
				   kneditWidgetClass,box,
			 	   arglist,1);
    buttonbox = XtVaCreateManagedWidget("controlButtonBox",
					formWidgetClass,box,
					XtNfromVert,(XtArgVal)knedit,
					NULL);

    command = XtVaCreateManagedWidget("button1",commandWidgetClass,
			  buttonbox, NULL);
    XtAddCallback(command,XtNcallback,bye_bye,NULL);

    command = XtVaCreateManagedWidget("button2",commandWidgetClass,
				      buttonbox,
				      XtNfromHoriz, (XtArgVal)command,
				      NULL);
    command = XtVaCreateManagedWidget("button3",grapherButtonWidgetClass,
			      buttonbox, 
			      XtNselectionString, (XtArgVal)AGRootGrapher,
			      XtNeditMode, (XtArgVal) global_info.edit_mode,
				      XtNfromHoriz, (XtArgVal)command,
				      NULL);

    XtAddCallback(command,XtNcallback,Show_Root_Grapher,
		  (XtPointer)AGRootGrapher);

    command = XtVaCreateManagedWidget("button4",grapherButtonWidgetClass,
			      buttonbox, 
			      XtNselectionString, (XtArgVal) AGRoot,
			      XtNeditMode, (XtArgVal) global_info.edit_mode,
				      XtNfromHoriz, (XtArgVal)command,
				      NULL);
    XtAddCallback(command,XtNcallback,Show_Root,(XtPointer)AGRoot);
    command = XtVaCreateManagedWidget("button5",commandWidgetClass,
				      buttonbox, 
				      XtNfromHoriz, (XtArgVal)command,
				      NULL);


    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);
    NodeService_Register_Open_Node(node_info,shell,NULL,NULL);
    return(shell);
}



	
	 



/****************************

  NODE CREATION

*****************************/


typedef struct _DynamicTable
{
  XrmQuark name_quark;
  XtCallbackProc callback; /*void (*callback)(); */
} DynamicTable;

static int dynamic_table_counter = -1; /* since routine inc's to start */
static int max_dynamic_table = 32;
static DynamicTable *dynamic_table;

Boolean Dynamic_Initialize()
{
  if ((dynamic_table = 
       (DynamicTable *)XtMalloc(sizeof(DynamicTable) * max_dynamic_table)) 
      == NULL)
    return(False);
  return(True);
}

Boolean Dynamic_Add_Callback(string,callback)
     String string;
     XtCallbackProc callback;
{
  if (!dynamic_table)
    return(False);
  if (string == NULL || string[0] == EOS || callback == NULL || 
      AGstrlen(string) > MaxString)
    {
      Util_Debug("illegal string or callback ptr passed to DynamicAddCallback");
      return(False);
    }

  if (++dynamic_table_counter >= max_dynamic_table)
    {
      max_dynamic_table = max_dynamic_table + (int)(max_dynamic_table/2);
      if ((dynamic_table = 
	   (DynamicTable *)XtRealloc(max_dynamic_table * sizeof(DynamicTable)))
	  == NULL)
	{
	  Util_Debug("unable to alloc in DynamicAddCallback");
	  return(False);
	}
    }
  
  dynamic_table[dynamic_table_counter].name_quark
    = XrmStringToQuark(string);
  dynamic_table[dynamic_table_counter].callback = callback;
  return(True);
}

static Boolean Dynamic_Internal_Trigger(w,client_data,call_data,string)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
     char *string;
{
  int i;
  XrmQuark temp_quark;
  if (!dynamic_table)
    return(False);

  if (string == NULL || string[0] == EOS || AGstrlen(string) > MaxString)
    {
      Util_Debug("illegal string or callback ptr passed to DynamicCallback");
      return(False);
    }

  temp_quark = XrmStringToQuark(string);
  for (i=0;i<=dynamic_table_counter;i++)
    if (dynamic_table[i].name_quark == temp_quark)
      {
	(dynamic_table[i].callback)(w,client_data,call_data);
	return(True);
      }
  return(False);
}

static void Dynamic_Internal_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  int i;
  String string;
  XrmQuark temp_quark;

  AGGrapherButtonCallbackStruct *callback_struct = 
    (AGGrapherButtonCallbackStruct *)call_data;

  if (!dynamic_table)
    return;
  string = callback_struct->params[0];

  Dynamic_Internal_Trigger(w,client_data,call_data,string);
}

void General_Dynamic_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  int i;
  String string;
  XrmQuark temp_quark;
  FuncObjReturnStruct new_return_struct;
  FuncObjReturnStruct *real_return_struct;
  int new_nparams;
  String *new_params;

  if (!dynamic_table)
    return;


  if (XtIsSubclass(w,funcObjClass))
    {
      real_return_struct = (FuncObjReturnStruct *)call_data;
      if (real_return_struct->nparams <= 0)
	return;
      string = real_return_struct->params[0];
      new_nparams = real_return_struct->nparams - 1;
      if ((new_params = (String *)XtMalloc(sizeof(String)*new_nparams))
	  == NULL)
	return;
      new_return_struct.nparams = new_nparams;
      for (i=1;i<real_return_struct->nparams;i++)
	new_params[i-1] = real_return_struct->params[i];
      new_return_struct.params = new_params;
      Dynamic_Internal_Trigger(global_info.main_shell,client_data,
			       (XtPointer)&new_return_struct,string);
      XtFree((char *)real_return_struct);

    }
  else
    {
      /* Don't know where it's from - why would anyone call this routine
	 as a callback instead of just calling the specified callback? 
	 We'll just hope for the best. */
    string = (char *)call_data;
    Dynamic_Internal_Trigger(w,client_data,call_data,string);
  }

}


static void Dynamic_Create_Node_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  int i;
  String second_param;


  AGGrapherButtonCallbackStruct *callback_struct = 
    (AGGrapherButtonCallbackStruct *)call_data;

  if (!dynamic_table)
    return;

  second_param = callback_struct->params[1];
  
  if (second_param != NULL  && second_param[0] != EOS)
#ifdef OLDAPI
    AG_Create_Node(w,second_param);
#else
  AG_Create_Node(w,callback_struct->selection_string,
		 &callback_struct->params[2],
		 callback_struct->num_params-2);
#endif
  else
    Util_Debug("illegal string passed to Dynamic_Create_Node_Callback");
}



void Dynamic_Button_Callback(w,client_data,call_data,callback_name,
			     default_proc)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
     String callback_name;
     XtCallbackProc default_proc;
{
  Widget shell;
  char name_buffer[MaxString];
  char class_buffer[MaxString];
  char resource_type[MaxString];
  XrmValue value;
  XrmValue to_value;
  Boolean rc;

  NodeInfoPtr node_info = (NodeInfoPtr) client_data;

  AGstrcpy(name_buffer,"AnswerGarden.");
  AGstrcat(name_buffer,NodeService_Get_Node_Name(node_info));
  AGstrcat(name_buffer,".");
  AGstrcat(name_buffer,callback_name);
  AGstrcpy(class_buffer,"AnswerGarden.");
  AGstrcat(class_buffer,NodeService_Get_String_From_Type(
		       NodeService_Get_Type(node_info)));
  AGstrcat(class_buffer,".");
  AGstrcat(class_buffer,callback_name);
  rc = XrmGetResource(XtDatabase(XtDisplay(w)),name_buffer,class_buffer,
		      resource_type,&value);
  if (rc && dynamic_table)
    {
      if (!Dynamic_Internal_Trigger(w,client_data,call_data,value.addr))
	(default_proc)(w,client_data,call_data);
    }
  else
    (default_proc)(w,client_data,call_data);
}  


void Dynamic_Close_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Dynamic_Button_Callback(w,client_data,call_data,XtNcloseCallback,
			  General_Close_Callback);
}
			  
void Dynamic_Help_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Dynamic_Button_Callback(w,client_data,call_data,XtNhelpCallback,
			  General_Help_Callback);
}
			  
void Dynamic_Question_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Dynamic_Button_Callback(w,client_data,call_data,XtNquestionCallback,
			  General_Question_Callback);
}
			  




  /* General callback for creating a new node (ie, following a link).
     Get the node name (part of call_data) and pass it on to the 
     proper routine. */
static void Create_Node_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  /*** Right now this struct is the same for Knedits and Graphers.
    This routine ought to check the type of w and cast appropriately ****/
  AGGrapherButtonCallbackStruct *callback_struct = 
    (AGGrapherButtonCallbackStruct *)call_data;

  /* selection string is the node name */
#ifdef OLDAPI
  AG_Create_Node(w,callback_struct->selection_string);
#else
  AG_Create_Node(w,callback_struct->selection_string,callback_struct->params,
		 callback_struct->num_params);
#endif
}


static void Create_Null_Node(w)
     Widget w;
{
    char *label;
    AGNodeTypes type;
    char *location;
    NodeInfoPtr node_info;
    char buffer[MaxString];
    char *node_name = AGMissingNode;

    /* request node service here to avoid a loop */
    if ((NodeService_Request_By_Name(node_name,&node_info,
				     &label,&type,&location) < 0))
	{
	  sprintf(buffer, "missing name %s in node service.  Continuing...",
		  node_name);
	  XtWarning(buffer);
	  User_History_List_Add_Member(node_name,"NotInNodeService");
	  Stat_Node_Create(w,NULL);
	  return;
	}
    else
#ifdef OLDAPI
      AG_Create_Node(w,node_name);
#else
      AG_Create_Node(w,node_name,NULL,0);
#endif
}

#ifdef OLDAPI
Widget AG_Create_Node(w,node_name)
     Widget w;
     char *node_name;
#else
Widget AG_Create_Node(w,node_name,misc_info,num_info)
     Widget w;
     char *node_name;
     String *misc_info;
     int num_info;
#endif
{
    char *label;
    AGNodeTypes type;
    char *location;
    char buffer[MaxString];
    Widget shell;
    NodeInfoPtr node_info;
    char *geometry;
    char geometry_string[MaxString];
    Dimension height;
    Dimension width;
    Position x;
    Position y;
    Boolean set_geometry;
    Widget old_shell;

    if ((NodeService_Request_By_Name(node_name,&node_info,
				     &label,&type,&location) < 0))
	{
	  sprintf(buffer, "missing name %s in node service.  Continuing...",
		  node_name);
	  XtWarning(buffer);
	  if (strcmplo(node_name,AGMissingNode))
	    Create_Null_Node(w);
	  else
	    XtWarning("Missing NodeService info for \"MissingNode\"");
	  User_History_List_Add_Member(node_name,"NotInNodeService");
	  Stat_Node_Create(w,NULL);
	  return(NULL);
	}

    if (location[0] == NULL)
	{
	    User_History_List_Add_Member(node_name,"MissingLocation");
	    Stat_Node_Create(w,node_info);
	    if (strcmplo(node_name,AGMissingNode))
	      Create_Null_Node(w);
	    else
	      XtWarning("Missing location for \"MissingNode\"");
	    return(NULL);
	}

    if (!strcmplo(node_name,"noaction")) /* may not be sbrowser type */
	{
	  User_History_List_Add_Member(node_name,"NoAction");
	  Stat_Node_Create(w,node_info);
/*	  XtWarning("node service returns no action on this node"); */
	  return(NULL);
	}

    
    if (type < 0)
      {
	sprintf(buffer,"node type for \"%s\" is unknown.   Continuing...", 
		node_name);
	XtWarning(buffer);
	XtWarning("\tPlease contact your site administrator.");
	User_History_List_Add_Member(node_name,"Unknown");
	Stat_Node_Create(w,node_info);
	if (strcmplo(node_name,AGMissingNode))
	  Create_Null_Node(w);
	else
	  XtWarning("Bad node type for \"MissingNode\"");
	return(NULL);
      }

    /* This should change to a workproc going through the list
       of open files checking whether any are dirty, and automatically
       setvaluing the nodes correctly. */


    /* Interesting problem:  the shell widgets do not necessarily
       return a valid geometry resource (including returning NULL
       in the default case).  Therefore I am creating a geometry
       string myself. */

    set_geometry = False;
    if ((old_shell = NodeService_Get_Shell(node_info)) != NULL)
      {
	if (!NodeService_Has_Node_Changed(node_info)) 
	  {
	    AG_Raise_Shell(old_shell);
	    User_History_List_Add_Member(node_name,
				      NodeService_Get_String_From_Type(type));
	    Stat_Node_Raise(w,node_info);
	    return(NULL);
	  }
	else
	  {
	    XtVaGetValues(old_shell,XtNheight,&height,
			  XtNwidth,&width, XtNx,&x, XtNy,&y, NULL);
	    sprintf(geometry_string,"%dx%d+%d+%d",width,height,x,y);
	    set_geometry = True;
	  }
      }
	
    
    if (!sorta_objs[type]->virtual_node)
      if (!Check_Node_Physical_Existance(location))
	{
	  /* avoid an infinite loop on MissingNode */
	  if (strcmplo(node_name,AGMissingNode))
	    Create_Null_Node(w);
	  else
	    {
	      XtWarning("Missing file for \"MissingNode\".  Continuing...");
	      XtWarning("\tPlease contact your site administrator.");
	    }
	  User_History_List_Add_Member(node_name,"NoPhysicalFile");
	  Stat_Node_Create(w,node_info);
	  return(NULL);
	}

    User_History_List_Add_Member(node_name,
				 NodeService_Get_String_From_Type(type));
    if (!sorta_objs[type]->class_created 
	&& sorta_objs[type]->class_create_function != NULL)
      {
	(sorta_objs[type]->class_create_function)(w,node_info,NULL);
	sorta_objs[type]->class_created = True;
      }
    if (sorta_objs[type]->create_function == NULL)
      {
	sprintf(buffer,"no create function for node type %d\n",
		type);
	Util_Debug(buffer);
	return(NULL);
      }
#ifdef OLDAPI
    if (!set_geometry)
      shell = (sorta_objs[type]->create_function)(w,
						  (XtPointer)node_info,
						  NULL);
    else     
      {
	XtDestroyWidget(old_shell);
	shell = (sorta_objs[type]->create_function)(w,
					     (XtPointer)node_info,
					     (XtPointer)geometry_string);
      }
#else
    if (!set_geometry)
      {
	shell = (sorta_objs[type]->create_function)(w,
					     node_info,
					     NULL,
       					     misc_info,
					     num_info);
      }
    else     
      {
	XtDestroyWidget(old_shell);
	shell = (sorta_objs[type]->create_function)(w,
					     node_info,
					     geometry_string,
       					     misc_info,
					     num_info);

      }
#endif	
    NodeService_Register_Open_Node(node_info,shell,NULL,NULL);
    Stat_Node_Create(w,node_info);

    return(shell);
}

static void Grapher_Destroy_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  /* not needed for now */
}


static void Show_Root_Grapher(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  NodeInfoPtr node_info;
  char *label;
  char *location;
  AGNodeTypes type;
  char buffer[MaxString];
  Widget shell;
  AGGrapherButtonCallbackStruct *callback_struct =
    (  AGGrapherButtonCallbackStruct *) call_data;
    
  char *node_name = AGRootGrapher;

  /*** CHANGE THIS ***/
  if (callback_struct->notify_type == AGGrapherButtonEditNotify) 
    /* must be edit */
    {
      Edit_Node_Callback(w,client_data,call_data);
    }
  else
    {
#ifdef OLDAPI
      AG_Create_Node(w,node_name);
#else
      AG_Create_Node(w,node_name,callback_struct->params,
		     callback_struct->num_params);
#endif
    }
  
}



#ifdef OLDAPI
Widget Show_Grapher(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
#else
Widget Show_Grapher(w,node_info,geometry,misc_info,num_info)
     Widget w;
     NodeInfoPtr node_info;
     String geometry;
     String *misc_info;
     int num_info;
#endif

{
    Widget grapher;
    Widget shell;
    Widget viewport;
    Widget form;
    char complete_filename[MaxString];
    char msg[MaxString];
    Arg arglist[10];
    int n;
    Dimension height;
    Dimension width;
    Boolean flag;
#ifdef OLDAPI
    NodeInfoPtr node_info = (NodeInfoPtr) client_data;
#endif
    char *filename;
    Widget button_box;
    Widget command[5];

    filename = NodeService_Get_Location(node_info);

    if (filename == NULL)
      {
	 XtWarning("null name passed to discussion node.  Continuing...");
	 return(NULL);
      }

    Form_Filename(complete_filename,filename);
    if (!NodeService_Open_File(node_info,complete_filename))
      {
	sprintf(msg,"Error in opening node at location %s.",filename);
	XtWarning(msg);
	XtWarning("Continuing...");
	NodeService_Close_File(node_info);
	return(NULL);
      }
    n = 0;
    XtSetArg(arglist[n],XtNinput,(XtArgVal)True);n++;
    XtSetArg(arglist[n],XtNallowShellResize,(XtArgVal)True);n++;
    XtSetArg(arglist[n],XtNtitle,(XtArgVal)NodeService_Get_Label(node_info));
      n++;

#ifdef OLDAPI    
    if (call_data != NULL)
      {
	XtSetArg(arglist[n],XtNgeometry,(XtArgVal)call_data);n++;
      }
#else
    if (geometry != NULL)
      {
	XtSetArg(arglist[n],XtNgeometry,(XtArgVal)geometry);n++;
      }
#endif
    shell = XtCreatePopupShell(NodeService_Get_Node_Name(node_info),
			       topLevelShellWidgetClass,
			       global_info.main_shell,
			       arglist,n);

    viewport = XtVaCreateManagedWidget("grapherViewport",viewportWidgetClass,
				       shell,
				       XtNallowHoriz,(XtArgVal)True,
				       XtNallowVert,(XtArgVal)True,
				       NULL);

    form = XtVaCreateManagedWidget("grapherForm",formWidgetClass,
				       viewport,
				       NULL);

    /* Create only 3 buttons */
    Util_Open_Minimal_ButtonBox(form,&button_box,command);

    XtAddCallback(command[2],XtNcallback,Dynamic_Question_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[1],XtNcallback,Dynamic_Help_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[0],XtNcallback,Dynamic_Close_Callback,
		  (XtPointer)node_info);

    grapher = XtVaCreateManagedWidget("grapher",grapherWidgetClass,
			      form,
			      XtNtext,
                                     (XtArgVal)NodeService_Get_Text(node_info),
			      XtNborderWidth, (XtArgVal)0,
			      XtNfromVert, (XtArgVal)button_box,
			      XtNvertDistance, (XtArgVal)0,
			      XtNeditMode, (XtArgVal)global_info.edit_mode,
			      NULL);

    NodeService_Free_Buffer(node_info);

    XtAddCallback(grapher,XtNcallback,Create_Node_Callback,grapher);
    XtAddCallback(grapher,XtNeditCallback,Edit_Node_Callback,grapher);
    XtAddCallback(grapher,XtNdynamicCallback,Dynamic_Internal_Callback,
		  grapher);
    XtAddCallback(shell,XtNdestroyCallback,Grapher_Destroy_Callback,
		  node_info);

    /* Unfortunately, there is no way to do this by subclassing Viewport
       to do this nicely.  Believe me, I tried. -- MSA */

    XtVaGetValues(grapher,XtNheight,&height,XtNwidth,&width,NULL);
    flag = False;
    if (height > global_info.grapher_max_height)
      {
	flag = True;
	height = global_info.grapher_max_height;
      }
    if (width > global_info.grapher_max_width)
      {
	width = global_info.grapher_max_width;
	flag = True;
      }
    if (flag)
      XtVaSetValues(viewport,XtNheight,(XtArgVal)height,
		    XtNwidth,(XtArgVal)width,
		    NULL);

    AG_Geometry(shell,node_info);
    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);
    return(shell);
}

static void Show_Root(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  NodeInfoPtr node_info;
  char *label;
  char *location;
  AGNodeTypes type;
  char buffer[MaxString];
  Widget shell;
  AGGrapherButtonCallbackStruct *callback_struct =
    (  AGGrapherButtonCallbackStruct *) call_data;

  char *node_name = AGRoot;

  /*** CHANGE THIS ***/
  if (callback_struct->notify_type == AGGrapherButtonEditNotify)
    /* must be edit */
    {
      Edit_Node_Callback(w,client_data,call_data);
    }
  else
    {
#ifdef OLDAPI
      AG_Create_Node(w,node_name);
#else
      AG_Create_Node(w,node_name,callback_struct->params,
		     callback_struct->num_params);
#endif
      
    }
}

static void Internal_Close_Callback(w,client_data,call_data,delete_flag)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
     Boolean delete_flag;
{
  Widget shell;
  int stack_num;
  NodeInfoPtr node_info = (NodeInfoPtr) client_data;

  if ((shell = NodeService_Get_Shell(node_info)) != NULL)
    {
      if (delete_flag)
	XtDestroyWidget(shell);
    }
  else
    Util_Debug("General Close Callback:  missing shell");
      
  if (global_info.n_stacks > 0)
    {
      stack_num = Geometry_Get_Stack(shell,node_info);
      if (stack_num > 0)
	AG_Stack_Delete_Shell(global_info.stacks[stack_num-1],shell);
    }
  NodeService_Register_Closed_Node(node_info,shell,NULL,NULL);
}

void AG_Close_Node(w,node_info)
     Widget w;
     NodeInfoPtr node_info;
{
  Widget shell;

  Stat_Button_Hit(w);
  Internal_Close_Callback(w,(XtPointer)node_info,NULL,False);
}



void General_Close_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Widget shell;
  NodeInfoPtr node_info = (NodeInfoPtr) client_data;

  Stat_Button_Hit(w);
  Internal_Close_Callback(w,client_data,call_data,True);
}



void General_Help_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{

  char *label;
  int type;
  char *location;
  char buffer[MaxString];
  char msg[MaxString];
  NodeInfoPtr help_node_info;
  NodeInfoPtr node_info = (NodeInfoPtr)client_data;

  AGstrcpy(buffer,NodeService_Get_Node_Name(node_info));
  AGstrcat(buffer,".hp");

  Stat_Button_Hit(w);

  if ((NodeService_Request_By_Name(buffer,&help_node_info,
				   &label,&type,&location) < 0))
    {
      Create_Null_Node(w);
      sprintf(msg,"missing help node for '%s'",
		NodeService_Get_Label(node_info));
      XtWarning(msg);
      Util_Debug("Missing help node in GeneralHelpCallback");
      return;
    }
  
#ifdef OLDAPI
    AG_Create_Node(w,buffer);
#else
  AG_Create_Node(w,buffer,NULL,0);
#endif

}

static void Iconification_Watcher(w,client_data,event,continue_on)
     Widget w;
     XtPointer client_data;
     XEvent *event;
     Boolean *continue_on;
{
  Window window;
  caddr_t data;
  
  switch (event->type)
    {
    case MapNotify:
      data = NULL;
      if (XFindContext(XtDisplay(w),XtWindow(w),global_info.icon_context,
		       &data) > 0  || (int)data != MapNotify)
	XSaveContext(XtDisplay(w),XtWindow(w),global_info.icon_context,
		     (caddr_t)(MapNotify));
      break;
    case UnmapNotify:
      data = NULL;
      if (XFindContext(XtDisplay(w),XtWindow(w),global_info.icon_context,
		       &data) > 0  || (int)data != UnmapNotify)
	XSaveContext(XtDisplay(w),XtWindow(w),global_info.icon_context,
		     (caddr_t)(UnmapNotify));
      break;
    case DestroyNotify:
      XDeleteContext(XtDisplay(w),XtWindow(w),global_info.icon_context);
      break;
    }
  *continue_on = True;
}
      
static void Iconification_Destroy_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  /* w is the shell */
  XDeleteContext(XtDisplay(w),XtWindow(w),global_info.icon_context);
}  

AG_Register_Open_Node(shell)
     Widget shell;
{
  XtAddEventHandler(shell,StructureNotifyMask,False,Iconification_Watcher);
  XtAddCallback(shell,XtNdestroyCallback,Iconification_Destroy_Callback,
		(XtPointer)shell);
}

void AG_Raise_Shell(widget)
     Widget widget;
{
  Window window;
  Window w2;
  Screen *the_screen;
  Display *the_display;
  Window parent;
  Window root;
  Window *children;
  int nchildren;
  Widget shell;
  int data;

  the_screen = XtScreen(widget);
  window = XtWindow(widget);
  the_display = XtDisplay(widget);
  /* I should get a shell in as the widget here, so there's not
     much point in going through the toolkit to get the parents.
     See if the window manager has put anything between the shell
     and the root window */

  if (!XQueryTree(the_display,window,&root,&parent,&children,&nchildren))
    return;  /* something is wrong */
  while ((parent != RootWindowOfScreen(the_screen)))
    {
      window = parent;
      XFree((char *)children);
      if (!XQueryTree(the_display,window,&root,&parent,&children,&nchildren))
	{
	  Util_Debug("unclear state in AGRaiseShell");
	  return; /* something is wrong */
	}
    }
  XFree((char *)children);

  /* just to make it more interesting, we have gotten mapnotify and
     unmap notify on the shell's window instead of the toplevel window */
  if (XFindContext(the_display,XtWindow(widget),
		   global_info.icon_context,(caddr_t)&data) == 0)
    {
      if (data == UnmapNotify) /* it's an icon */
	{
	  XtVaSetValues(widget,XtNiconic,(XtArgVal)False,NULL);
	  XtMapWidget(widget);
	}
      else
	XRaiseWindow(the_display,window);
    }
  else /* we're lost, so we might as well try */
    {
      XRaiseWindow(the_display,window);
      Util_Debug("missing xcontext in AGRaiseShell");
    }

}

void History_Close_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Widget shell = (Widget) client_data;
  global_info.history_list = NULL;
  XtDestroyWidget(shell);
}


void Show_History_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  char **name_array = (char **)client_data;
  XawListReturnStruct *lr_struct = (XawListReturnStruct *)call_data;
  char *node_name;

  node_name = name_array[lr_struct->list_index];

  if (node_name != NULL && node_name[0] != EOS 
      && AGstrlen(node_name) <= MaxString)
#ifdef OLD_API
    AG_Create_Node(w,node_name);
#else
  AG_Create_Node(w,node_name,NULL,0);
#endif
}
  

#define MaxHistoryList 100
#define FUDGE_LIST 10
#define FUDGE_SCROLL 20

Show_More_History_List()
{
  int num;
  static char *name_array[MaxHistoryList+1];
  static char *label_array[MaxHistoryList+1];

  User_History_Make_List(label_array,name_array,MaxHistoryList,&num);

  if (num == 0)
    {
      XtWarning("No history list yet.");
      return;
    }

  label_array[num++] = NULL;
  name_array[num++] = NULL;


  XtRemoveAllCallbacks(global_info.history_list,XtNcallback);
  XawListChange(global_info.history_list,label_array,0,0,True);
  XtAddCallback(global_info.history_list,XtNcallback,Show_History_Callback,
		(XtPointer)name_array);
}

Show_History_List()
{
  Widget shell;
  Widget list;
  static char *name_array[MaxHistoryList+1];
  static char *label_array[MaxHistoryList+1];
  int num;
  Widget form;
  Widget buttons[5];
  Widget buttonbox;
  Widget parent;
  Widget sibling;
  Widget view;
  Widget inner_form;
  Dimension width, width_list;

  User_History_Make_List(label_array,name_array,MaxHistoryList,&num);

  if (num == 0)
    {
      XtWarning("No history list yet.");
      return;
    }

  label_array[num++] = NULL;
  name_array[num++] = NULL;

    
  shell = XtVaCreatePopupShell("HistoryList",
			       topLevelShellWidgetClass,
			       global_info.main_shell,
			       XtNallowShellResize, (XtArgVal)True,
			       XtNinput, (XtArgVal)True, 
			       XtNtitle, (XtArgVal)"History List",
			       NULL);
  
  form = XtVaCreateManagedWidget("historyListForm",formWidgetClass,
				 shell,
				 NULL);
  view = XtVaCreateManagedWidget("historyListViewport",viewportWidgetClass,
				 form,
				 XtNforceBars, (XtArgVal)True,
				 XtNallowVert, (XtArgVal) True,
				 NULL);
  inner_form = XtVaCreateManagedWidget("historyListInnerForm",
				       formWidgetClass,view,NULL);
  global_info.history_list = 
    list = XtVaCreateManagedWidget("historyListList",
				   listWidgetClass,
				   inner_form,
				   XtNdefaultColumns, (XtArgVal)1,
				   XtNlist, (XtArgVal)label_array,
				   NULL);
  
  Util_Open_Minimal_ButtonBox(form,&buttonbox,buttons);
  XtVaSetValues(buttonbox,XtNfromVert,(XtArgVal)view,NULL);

  XtVaGetValues(buttonbox,XtNwidth,(XtArgVal)&width,NULL);
  XtVaGetValues(list,XtNwidth,(XtArgVal)&width_list,NULL);
  
  width = (width_list < width + FUDGE_LIST) ? width+FUDGE_LIST : width_list;

  width = width_list + FUDGE_SCROLL;

  XtVaSetValues(view,XtNwidth,(XtArgVal)width,NULL);
  XtAddCallback(list,XtNcallback,Show_History_Callback,
		(XtPointer)name_array);
  XtAddCallback(buttons[0],XtNcallback,History_Close_Callback,(XtPointer)shell);

  XtRealizeWidget(shell);
  XtPopup(shell,XtGrabNone);
  
}

#undef FUDGE_LIST
#undef FUDGE_SCROLL;

void Other_Menu_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  int menu_button = (int)client_data;
  Dimension width, hieght;
  Position x,y;
  char geometry_string[MaxString];

  switch (menu_button)
    {
      /* Cannot pass on w since w is actually an object.  For creating
	 additional shells, we need a widget (4/3/91). */
    case 0:
      if (global_info.history_list == NULL)
	Show_History_List();
      else
	Show_More_History_List();
      break;
    case 1:
       break;
    default:
      Util_Debug("Unknown value in other callback.  Continuing....");
    }
}

#define NumOtherFunctions 3

void Other_Initialize()
{
  char buffer[MaxString];
  int i;
  Widget menu_button[NumOtherFunctions];
  Widget menu_shell;

    /* Xaw requires this */
  XawSimpleMenuAddGlobalActions(XtWidgetToApplicationContext(
					    global_info.main_shell));

  menu_shell =
    XtVaCreatePopupShell("otherMenuShell",simpleMenuWidgetClass,
			 global_info.main_shell,
			 XtNlabel, (XtArgVal) "Other Functions",
			 NULL);
  XtVaCreateManagedWidget("otherMenuLine",smeLineObjectClass,menu_shell,
			  NULL);
			  
  for (i=0;i<NumOtherFunctions;i++)
    {
      sprintf(buffer,"otherMenuButton%1d",i+1);
      menu_button[i] = XtVaCreateManagedWidget(buffer,smeBSBObjectClass,
					menu_shell,NULL);
      XtAddCallback(menu_button[i],XtNcallback,Other_Menu_Callback,
		    (XtPointer)i);
    }

  XtVaSetValues(menu_shell,
		XtNpopupOnEntry, (XtArgVal) menu_button[0],
		NULL);


}

AG_Initialize()
{
  int i;
  char name_buffer[MaxString];
  char class_buffer[MaxString];

  /* Initialize all the flags indicating whether the class initialize
     functions have already been triggered off to False */
  Other_Initialize();

  for (i=0;i<global_info.n_sorta_objs;i++)
    sorta_objs[i]->class_created = False;

  /* One time node type initializations; used primarily to add 
     dynamic callbacks */

  for (i=0;i<global_info.n_sorta_objs;i++)
    if (sorta_objs[i]->initialize_function != NULL)
      (sorta_objs[i]->initialize_function)();

      
  Dynamic_Add_Callback("CreateNode",Dynamic_Create_Node_Callback);
  Dynamic_Add_Callback("GeneralCloseCallback",General_Close_Callback);
  Dynamic_Add_Callback("GeneralQuitCallback",bye_bye);
  Dynamic_Add_Callback("GeneralHelpCallback",General_Help_Callback);

  XtAddCallback(global_info.global_func_obj,XtNcrushCallback,
		General_Crush_Callback,NULL);

  XtAddCallback(global_info.global_func_obj,XtNdynamicCallback,
		General_Dynamic_Callback,NULL);

}



main(argc,argv)
     int argc;
     char *argv[];
{
    Widget shell;
    Widget layout;
    Widget first_shell;

    /* Set up the shell widget.  */
  
    shell = 
      XtInitialize(argv[0],"AnswerGarden",NULL,0,&argc,argv);

    /* Need to do this early since it's got all the global info,
       including the number of SortaObjs */
    Globals_Initialize(shell);
    Dynamic_Initialize();

      /* Stat must be after Globals since it needs the name of
	 the statistics file from the appdefaults resource. */
    Stat_Start_User(shell);

    if (CommService_Initialize(shell) < 0)
      {
	XtWarning("Unable to initialize Answer Garden's communications");
	XtWarning("\tservice.  You will not be able to ask a question.");
	XtWarning("\tContinuing....");
      }

#ifdef NO_EDIT
      /* If no editing is allowed, hard-wire edit_mode to be False,
	 disabling it everywhere.  AnswerGarden.c explicitly passes
	 XtNeditMode in the arglist to set the resource
	 when creating SBrowsers and Graphers.  SBrowsers and Graphers
	 explicitly pass the resource to GrapherButtons. */
    global_info.edit_mode = False;
#endif

    if (NodeService_Initialize() < 0)
      {
	XtWarning("Empty or missing NodeService file (AGNodeFile)");
	XtWarning("You are unlikely to get good results.  Continuing...");
      }

    AG_Initialize();

/*    
    FileService_Initialize(shell);
*/

    Edit_Initialize(shell); /* must before open_control for correct
			       setup of grapher button actions for
			       edit menu */

    if (global_info.startup_node == NULL)
      {
	first_shell = Open_Control(shell,"Control",NULL);
      }
    else
#ifdef OLDAPI
      first_shell = AG_Create_Node(shell,global_info.startup_node);
#else
      first_shell = AG_Create_Node(shell,global_info.startup_node,NULL,0);
#endif
    if (first_shell == NULL)
      return;

    XtMainLoop();
}






