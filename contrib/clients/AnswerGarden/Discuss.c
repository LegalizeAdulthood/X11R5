/**********************************************************


  The ANSWER GARDEN PROJECT:  DISCUSS.C

Copyright 1989, 1990 by the Massachusetts Institute of Technology,
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

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena
  ackerman@athena.mit.edu
  ackerman@xv.mit.edu

  July, 1989

***********************************************************/


#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Label.h>
#include "AG.h"

extern UserSettings user_settings;
extern HistoryNode *history_list;
extern HistoryNode *current_history_node;

extern GlobalInfo global_info;


static void DiscussType_Put_Error(w,msg)
     Widget w;
     char *msg;
{
   XtWarning(msg);
}

static void DiscussType_Destructor_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  /* currently unnecessary */
}

#define DiscussTypeName(name,type,str)  AGstrcpy(name,type); \
                                        AGstrcat(name,str)

static Widget DiscussType_Create(w,node_info,geometry,node_type_string)
     Widget w;
     NodeInfoPtr node_info;
     XtPointer geometry;
     char *node_type_string;
{
    Widget shell;
    Widget text;
    Widget form;
    Widget command[5];
    Widget button_form;
    Widget label;

    Arg arglist[10];
    char filestring[MaxString];
    char *file_body_buffer;
    char msg[MaxString];
    char resource_name[MaxString];

    char *filename;

    filename = NodeService_Get_Location (node_info);

    if (filename == NULL)
      {
	 DiscussType_Put_Error(w,
			   "null name passed to discussion node.  Continuing...");
	 return(NULL);
      }

    Form_Filename(filestring,filename);
    if (! NodeService_Open_File(node_info,filestring))
      {
	sprintf(msg,"error in %s file %s.  Continuing...",node_type_string,
		 filename);
	DiscussType_Put_Error(w,msg);
	NodeService_Close_File(node_info);
	return(NULL);
      }

    if (geometry == NULL)
      shell = XtVaCreatePopupShell(NodeService_Get_Node_Name(node_info),
				   topLevelShellWidgetClass,
				   global_info.main_shell,
				   XtNinput,(XtArgVal)True,
				   XtNallowShellResize, (XtArgVal)True,
				   XtNtitle, (XtArgVal)
				     NodeService_Get_Label(node_info),
				   NULL);
    else
      shell = XtVaCreatePopupShell(NodeService_Get_Node_Name(node_info),
				   topLevelShellWidgetClass,
				   global_info.main_shell,
				   XtNinput,(XtArgVal)True,
				   XtNallowShellResize, (XtArgVal)True,
				   XtNgeometry,(XtArgVal)geometry,
				   XtNtitle, (XtArgVal)
				     NodeService_Get_Label(node_info),
				   NULL);


    DiscussTypeName(resource_name,node_type_string,"Form");
    form = XtCreateManagedWidget(resource_name,formWidgetClass,shell,
				 NULL,0);

    DiscussTypeName(resource_name,node_type_string,"Label");
    label = XtVaCreateManagedWidget(resource_name,labelWidgetClass,form,
				  NULL);

    file_body_buffer = NodeService_Get_Text(node_info);

    DiscussTypeName(resource_name,node_type_string,"Text");
    text = XtVaCreateManagedWidget(resource_name,asciiTextWidgetClass,form,
		     XtNtype, (XtArgVal)XawAsciiString,
		     XtNstring, (XtArgVal) file_body_buffer,
		     XtNeditType, (XtArgVal)XawtextRead,
		     XtNscrollHorizontal, 
					  (XtArgVal)XawtextScrollWhenNeeded,
		     XtNscrollVertical, 
					  (XtArgVal)XawtextScrollWhenNeeded,
		     XtNuseStringInPlace, (XtArgVal) True,
		     XtNlength, (XtArgVal) AGstrlen(file_body_buffer),
		     XtNfromVert, (XtArgVal)label,
		     XtNfromHoriz, (XtArgVal)NULL,
		     NULL);


    Util_Open_ButtonBox(form,&button_form,command,NULL,NULL,0);
    XtVaSetValues(button_form,XtNfromVert,(XtArgVal)text,NULL);

    XtAddCallback(command[2],XtNcallback,Dynamic_Question_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[1],XtNcallback,Dynamic_Help_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[0],XtNcallback,Dynamic_Close_Callback,
		  (XtPointer)node_info);

    XtAddCallback(shell,XtNdestroyCallback,DiscussType_Destructor_Callback,
		  (XtPointer)node_info);

    AG_Geometry(shell,node_info);
    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);
    return(shell);
}








#ifdef OLDAPI
Widget Code_Create(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;

{
  NodeInfoPtr node_info = (NodeInfoPtr)client_data;
  return(DiscussType_Create(w,node_info,call_data,"code"));
}
#else
Widget Code_Create(w,node_info,geometry,misc_info,num_info)
     Widget w;
     NodeInfoPtr node_info;
     String geometry;
     String *misc_info;
     int num_info;
{
  return(DiscussType_Create(w,node_info,geometry,"code"));
}
#endif

#ifdef OLDAPI
Widget Discuss_Create(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  NodeInfoPtr node_info = (NodeInfoPtr)client_data;

  return(DiscussType_Create(w,node_info,call_data,"discussion"));

}
#else
Widget Discuss_Create(w,node_info,geometry,misc_info,num_info)
     Widget w;
     NodeInfoPtr node_info;
     String geometry;
     String *misc_info;
     int num_info;
{
  return(DiscussType_Create(w,node_info,geometry,"discussion"));
}

#endif


