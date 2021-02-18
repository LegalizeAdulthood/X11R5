/**********************************************************


  The ANSWER GARDEN PROJECT:  JOURNAL.C

Copyright 1989, 1990, 1991 by the Massachusetts Institute of Technology,
Cambridge, Massachusetts.

  journal.c   a prototype node type for handling journal entries

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

typedef struct _JournalFieldRec
{
   Widget text;
   struct _JournalFieldRec *next;
} JournalFieldRec;

typedef struct _JournalInfo
{
   NodeInfoPtr node_info;
   Widget text;
   JournalFieldRec *root;
   JournalFieldRec *last;
} JournalInfo;

static void Journal_Put_Error(w,msg)
     Widget w;
     char *msg;
{
   XtWarning(msg);
}

static void Journal_Destructor(journal_info)
     JournalInfo *journal_info;
{
  JournalFieldRec *temp;
  JournalFieldRec *next;
  if (!journal_info)
     {
       Util_Debug("Journal Destructor:  null journal info");
       return;
     }
  temp = journal_info->root;
  while (temp)
    {
      next = temp->next;
      XtFree(temp);
      temp = next;
    }
  XtFree(journal_info);
}

static void Journal_FieldRec_New(journal_info,widget)
     JournalInfo *journal_info;
     Widget widget;
{
  JournalFieldRec *temp;
  if (!journal_info)
    {
      Util_Debug("Journal FieldRecNew: illegal journal info");
      return;
    }
  if ((temp = (JournalFieldRec *) XtMalloc(sizeof(JournalFieldRec))) == NULL)
    return;
  if (journal_info->last)
    {
      journal_info->last->next = temp;
      journal_info->last = temp;
    }
  else
    {
      journal_info->last = temp;
      journal_info->root = temp;
    }
  temp->text = widget;
  temp->next = NULL;
}


static JournalInfo *Journal_New(node_info)
     NodeInfoPtr node_info;
{
   JournalInfo *journal_info;
   if ((journal_info = (JournalInfo *)XtMalloc(sizeof(JournalInfo))) == NULL)
      return(NULL);
   journal_info->node_info = node_info;
   journal_info->text = NULL;
   journal_info->root = NULL;
   journal_info->last = NULL;
   return(journal_info);
}

static void Journal_Destructor_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
   JournalInfo *journal_info = (JournalInfo *)client_data;
   Journal_Destructor(journal_info);
}

static void Journal_New_Text_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
  Widget button = (Widget)client_data;
  XtSetSensitive(button,True);
   /* This comes in with the widget as the src ? */
  XtRemoveAllCallbacks(w,XtNcallback);
}


static void Journal_Save_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
   JournalInfo *journal_info = (JournalInfo *)client_data;
   char *text_buffer;

#ifdef NEXT
   XtVaGetValues(journal_info->project_name,XtNstring,&string,NULL);
#endif   


   XtVaGetValues(journal_info->text,XtNstring,&text_buffer,NULL);
   
   NodeService_Save_File(journal_info->node_info,text_buffer);

   /* set the "save" button to insensitive until there is some
      changed text 7/7/91 */
   XtSetSensitive(w,False);
   XtAddCallback(XawTextGetSource(journal_info->text),
		 XtNcallback,Journal_New_Text_Callback,
		 (XtPointer)w);
}


static void Journal_Date_Callback(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
{
   JournalInfo *journal_info = (JournalInfo *)client_data;
   char *text_buffer;
   char date[MaxString];
   XawTextBlock text_block;
   char new_stuff[MaxString];
   Widget text;
   int len;

   text = journal_info->text;
   
   Util_Get_Date(date);
   AGstrcpy(new_stuff,"\n\n@date(");
   AGstrcat(new_stuff,date);
   AGstrcat(new_stuff,")\n\n");
   XtVaGetValues(text,XtNstring,&text_buffer,NULL);
   len = AGstrlen(text_buffer);
   

   text_block.firstPos = 0;
   text_block.length = AGstrlen(new_stuff);
   text_block.ptr = new_stuff;
   text_block.format = FMT8BIT;
   
   XawTextReplace(text,len,len,&text_block);

   len = AGstrlen(text_buffer) + AGstrlen(new_stuff);

   XawTextSetInsertionPoint(text,len);
}

static Widget Journal_Create_Fields(journal_info,inner_form,fields_root)
     JournalInfo *journal_info;
     Widget inner_form;
     AG_Nonpredefined_Header *fields_root;
{
  AG_Nonpredefined_Header *temp;
  AG_Nonpredefined_Header *next;
  Widget label;
  Widget widget;
  
  temp = fields_root;
  
  if (temp)
    widget = XtVaCreateManagedWidget("journalFieldTopLabel",
				     labelWidgetClass,inner_form,
				     NULL);
  else
    widget = NULL;

  while (temp)
    {
      label = XtVaCreateManagedWidget("journalFieldLabel",
				      labelWidgetClass,inner_form,
				      XtNlabel,(XtArgVal)temp->label,
				      XtNfromVert,(XtArgVal)widget,
				      NULL);
      widget = XtVaCreateManagedWidget("journalFieldText",
				       asciiTextWidgetClass,
				       inner_form,
				       XtNtype, (XtArgVal)XawAsciiString,
				       XtNstring,(XtArgVal)temp->value,
				       XtNfromVert,(XtArgVal)widget,
				       XtNeditType, (XtArgVal)XawtextEdit,
				       XtNfromHoriz, (XtArgVal)label,
				       NULL);
      Journal_FieldRec_New(journal_info,widget);
      temp = temp->next;
    }
  return(widget);
}  


#ifdef OLDAPI
Widget Journal_Create(w,client_data,call_data)
     Widget w;
     XtPointer client_data;
     XtPointer call_data;
#else
Widget Journal_Create(w,node_info,geometry,misc_info,num_info)
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
    JournalInfo *journal_info;
    char *filename;
    AG_Nonpredefined_Header *fields_root;
    Widget viewport;
    Widget inner_form;
    Widget last_widget;
#ifdef OLDAPI
    NodeInfoPtr node_info = (NodeInfoPtr) client_data;
#endif
    filename = NodeService_Get_Location (node_info);

    if (filename == NULL)
      {
	 Journal_Put_Error(w,"null name");
	 return(NULL);
      }

    if ((journal_info = Journal_New(node_info)) == NULL)
       {
	  Journal_Put_Error(w,"unable to allocate memory ");
	  return(NULL);
       }

    Form_Filename(filestring,filename);
    if (!NodeService_Open_File(node_info,filestring))
      {
	sprintf(msg,"can't open %s",filename);
	Journal_Put_Error(w,msg);
	Journal_Destructor(journal_info);
	return(NULL);
      }

    fields_root = NodeService_Get_Nonpredefined_Headers(node_info);

    shell = XtVaCreatePopupShell(NodeService_Get_Node_Name(node_info),
				 topLevelShellWidgetClass,
				 global_info.main_shell,
				 XtNinput,(XtArgVal)True,
				 XtNallowShellResize, (XtArgVal)True,
				 XtNtitle, (XtArgVal)
				 NodeService_Get_Label(node_info),
				 NULL);

    form = XtCreateManagedWidget("journalForm",formWidgetClass,shell,
				 NULL,0);

    viewport = XtVaCreateManagedWidget("journalViewport",viewportWidgetClass,
				       form,NULL);
    inner_form = XtCreateManagedWidget("journalInnerForm",
				       formWidgetClass,viewport,
				       NULL,0);

    last_widget = Journal_Create_Fields(journal_info,inner_form,fields_root);


    file_body_buffer = NodeService_Get_Text(node_info);

    len = AGstrlen(file_body_buffer);

    journal_info->text = 
       text = XtVaCreateManagedWidget("journalText",asciiTextWidgetClass,
				      inner_form,
				      XtNtype, (XtArgVal)XawAsciiString,
				      XtNstring, (XtArgVal) file_body_buffer,
				      XtNeditType, (XtArgVal)XawtextEdit,
#ifdef OLD
				      XtNscrollHorizontal, 
				         (XtArgVal)XawtextScrollWhenNeeded,
  				      XtNscrollVertical, 
				         (XtArgVal)XawtextScrollWhenNeeded,
#endif				      
				      XtNfromVert, (XtArgVal)last_widget,
				      XtNfromHoriz, (XtArgVal)NULL,
				      XtNautoFill, (XtArgVal) True,
				      XtNinsertPosition, (XtArgVal) len,
				      NULL);


    Util_Open_ButtonBox(form,&button_form,command,NULL,NULL,0);
    XtVaSetValues(button_form,XtNfromVert,(XtArgVal)viewport,NULL);
    XtAddCallback(command[4],XtNcallback,Journal_Save_Callback, 
		  (XtPointer)journal_info);
    /* set the "save" button to insensitive until there is some
       changed text 7/7/91 */
    XtSetSensitive(command[4],False);
    XtAddCallback(XawTextGetSource(text),
			XtNcallback,Journal_New_Text_Callback,
			(XtPointer)command[4]);
    XtAddCallback(command[3],XtNcallback,Journal_Date_Callback,
		  (XtPointer)journal_info);

    XtAddCallback(command[2],XtNcallback,Dynamic_Question_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[1],XtNcallback,Dynamic_Help_Callback,
		  (XtPointer)node_info);
    XtAddCallback(command[0],XtNcallback,Dynamic_Close_Callback,
		  (XtPointer)node_info);

    XtAddCallback(shell,XtNdestroyCallback,Journal_Destructor_Callback,
		  (XtPointer)journal_info);
    AG_Geometry(shell,node_info);
    XtRealizeWidget(shell);
    XtPopup(shell,XtGrabNone);
    return(shell);
}









