/**********************************************************


  The ANSWER GARDEN PROJECT:  CISTK.C

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

  cistk.c   this is a prototype node type to handle remote
              database accesses

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
  ackerman@athena.mit.edu
  ackerman@xv.mit.edu


***********************************************************/

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Viewport.h>
#include "Layout.h"
#include "AG.h"

extern UserSettings user_settings;
extern HistoryNode *history_list;
extern HistoryNode *current_history_node;

extern GlobalInfo global_info;

#define CISTK_BUFSIZ 2048

typedef struct _CistkFieldRec
{
   Widget text;
   struct _CistkFieldRec *next;
} CistkFieldRec;

typedef struct _CistkInfo
{
   NodeInfoPtr node_info;
   Widget text;
   char *buffer;
   CistkFieldRec *root;
   CistkFieldRec *last;
} CistkInfo;

static void Cistk_Put_Error(w,msg)
     Widget w;
     char *msg;
{
   XtWarning(msg);
}

static void Cistk_Destructor(cistk_info)
     CistkInfo *cistk_info;
{
  CistkFieldRec *temp;
  CistkFieldRec *next;
  if (!cistk_info)
     {
       Util_Debug("Cistk Destructor:  null cistk info");
       return;
     }
  temp = cistk_info->root;
  while (temp)
    {
      next = temp->next;
      XtFree(temp);
      temp = next;
    }
  if (cistk_info->buffer != NULL)
    XtFree(cistk_info->buffer);
  XtFree(cistk_info);
}



static CistkInfo *Cistk_New(node_info)
     NodeInfoPtr node_info;
{
   CistkInfo *cistk_info;
   if ((cistk_info = (CistkInfo *)XtMalloc(sizeof(CistkInfo))) == NULL)
      return(NULL);
   cistk_info->node_info = node_info;
   cistk_info->text = NULL;
   cistk_info->root = NULL;
   cistk_info->last = NULL;
   cistk_info->buffer = NULL;
   return(cistk_info);
}

static void Cistk_Destructor_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
   CistkInfo *cistk_info = (CistkInfo *)client_data;
   Cistk_Destructor(cistk_info);
}

char *query_header = "\
(sleep 2; echo \"mackerma\"; sleep 1; echo \"fun_city\"; sleep 2; \
echo \"cd /u/class2 \";sleep 1;echo \"sqlcmd\";sleep 3;echo ";

char *query_tail =  "\
; echo \"exit\";sleep 5;echo \"logout\")";



#ifdef OLDAPI
Widget Cistk_Create(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
#else
Widget Cistk_Create(w,node_info,geometry,misc_info,num_info)
     Widget w;
     NodeInfoPtr node_info;
     String geometry;
     String *misc_info;
     int num_info;
#endif
{
    Widget shell;
    Widget text;
    Widget form;
    Widget command[5];
    Widget button_form;

    char filestring[MaxString];
    char msg[MaxString];
    char *file_body_buffer;
    int len;
    CistkInfo *cistk_info;
    char *filename;
    Widget viewport;
    Widget inner_form;
    Widget last_widget;
    int i;
    char *stream_buffer;
    FILE *fp;
    int stream_size;
    char c;
    char buffer[CISTK_BUFSIZ]; /* for the command, can go over 256 */

#ifdef OLDAPI
    NodeInfoPtr node_info = (NodeInfoPtr) client_data;
#endif
    filename = NodeService_Get_Location (node_info);

    if (filename == NULL)
      {
	 Cistk_Put_Error(w,"null name");
	 return(NULL);
      }

    if ((cistk_info = Cistk_New(node_info)) == NULL)
       {
	  Cistk_Put_Error(w,"unable to allocate memory ");
	  return(NULL);
       }

    Form_Filename(filestring,filename);
    if (!NodeService_Open_File(node_info,filestring))
      {
	sprintf(msg,"can't open %s",filename);
	Cistk_Put_Error(w,msg);
	Cistk_Destructor(cistk_info);
	return(NULL);
      }


    shell = XtVaCreatePopupShell(NodeService_Get_Node_Name(node_info),
				 topLevelShellWidgetClass,
				 global_info.main_shell,
				 XtNinput,(XtArgVal)True,
				 XtNallowShellResize, (XtArgVal)True,
				 XtNtitle, (XtArgVal)
				   NodeService_Get_Label(node_info),
				 NULL);

    form = XtCreateManagedWidget("cistkForm",formWidgetClass,shell,
				 NULL,0);

    viewport = XtVaCreateManagedWidget("cistkViewport",viewportWidgetClass,
				       form,NULL);
    inner_form = XtCreateManagedWidget("cistkInnerForm",
				       formWidgetClass,viewport,
				       NULL,0);

    last_widget = NULL;


    file_body_buffer = NodeService_Get_Text(node_info);


    /*  This is *not* going to work unless you 
    sprintf(buffer,"%s \"%s\" %s | telnet machine.mit.edu | temp2",
	    query_header,file_body_buffer,query_tail);
    
    

    if ((fp = popen(buffer,"r")) == NULL)
      {
	XtWarning("unable to start cis-tk query");
	return(NULL);
      }


    i = 0;
    if ((stream_buffer = XtMalloc(CISTK_BUFSIZ)) == NULL)
      {
	XtWarning("no memory.  aborting request....");
	pclose(fp);
	return(NULL);
      }
    else
      stream_size = CISTK_BUFSIZ;
	  
    while ((c = fgetc(fp)) != EOF)
      {
	stream_buffer[i] = c;
	if (++i >=  stream_size)
	  {
	    if ((stream_buffer = XtRealloc(stream_buffer,
					   stream_size+CISTK_BUFSIZ))
		== NULL)
	      {
		XtWarning("out of memory.  trying to continue...");
		i--;
		break;
	      }
	    else
	      stream_size = stream_size + CISTK_BUFSIZ;
	  }
      }	      

    stream_buffer[i] = EOS;

    pclose(fp);
	
    cistk_info->buffer = stream_buffer; /* so can be freed later */
    
    len = AGstrlen(file_body_buffer);

    cistk_info->text = 
       text = XtVaCreateManagedWidget("cistkText",asciiTextWidgetClass,
				      inner_form,
				      XtNtype, (XtArgVal)XawAsciiString,
				      XtNstring, (XtArgVal) stream_buffer,
				      XtNeditType, (XtArgVal)XawtextRead,
#ifdef OLD
				      XtNscrollHorizontal, 
				         (XtArgVal)XawtextScrollWhenNeeded,
#endif
  				      XtNscrollVertical, 
				         (XtArgVal)XawtextScrollWhenNeeded,
				      XtNfromVert, (XtArgVal)last_widget,
				      XtNfromHoriz, (XtArgVal)NULL,
				      XtNautoFill, (XtArgVal) True,
				      NULL);


    Util_Open_Minimal_ButtonBox(form,&button_form,command);
    XtVaSetValues(button_form,XtNfromVert,(XtArgVal)viewport,NULL);

    XtAddCallback(command[2],XtNcallback,Dynamic_Question_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[1],XtNcallback,Dynamic_Help_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[0],XtNcallback,Dynamic_Close_Callback,
		  (XtPointer)node_info);

    XtAddCallback(shell,XtNdestroyCallback,Cistk_Destructor_Callback,
		  (XtPointer)cistk_info);
    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);
    return(shell);
}









