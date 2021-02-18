/**********************************************************

  The ANSWER GARDEN PROJECT

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

  AnswerGarden.c

  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  July, 1989

***********************************************************/
#include <stdio.h>
#include "AG.h"
#include "FuncObj.h"
#include "StackObj.h"

#define MAXSHORT  32767

/***********************************

  USER SETTINGS

***********************************/

UserSettings user_settings =
{ 
    TRUE, 
    FALSE, 
};


/**************************************

  SORTA OBJECTS

  SortaObj's -- How node types
  get hooked into AG.  For each
  node type there must be a SortaObj
  record here.

******************************************/
extern Widget Open_SBrowser();
extern Widget Show_Grapher();
extern Widget Discuss_Create();
extern Widget Create_QA();
extern Widget Code_Create();

#ifdef SAO
extern Widget Open_ParamEditor();
extern Widget Open_PackageLoader();
extern XtCallbackProc Init_ParamEditor();
#endif

SortaObj sBrowser_obj = { "SBrowser", 0, NULLQUARK,  Open_SBrowser, 
			    NULL, NULL, False,
			    NULL, NULL, NULL};
SortaObj grapher_obj = {"Grapher", 0, NULLQUARK,  Show_Grapher, 
			  NULL, NULL, False,
			  NULL, NULL, NULL};
SortaObj discussion_obj = {"Discussion", 0, NULLQUARK,  Discuss_Create, 
			     NULL, NULL, False,
			     NULL, NULL, NULL};
SortaObj qa_obj = {"QA-Node", 0, NULLQUARK,  Create_QA, 
		     NULL, NULL, False,
		     NULL, NULL, NULL};
SortaObj code_obj = {"Code",0, NULLQUARK, Code_Create, 
		       NULL, NULL, False,
		       NULL, NULL, NULL};

#ifdef SAO
/* ParamEdit stuff */
SortaObj pfedit_obj = {"ParamEditor", 0, NULLQUARK, Open_ParamEditor, 
			 NULL, Init_ParamEditor,True, 
		         NULL, NULL, NULL};
SortaObj pfload_obj = {"PackageLoader", 0, NULLQUARK, Open_PackageLoader, 
			 NULL, NULL, True, 
		         NULL, NULL, NULL};
#endif


#ifdef JOURNAL
extern Widget Journal_Create();
SortaObj journal_obj = {"Journal",0, NULLQUARK, Journal_Create,
			  NULL, NULL, False,
			  NULL, NULL, NULL};
#endif JOURNAL
#ifdef CISTK
extern Widget Cistk_Create();
SortaObj cistk_obj = {"Cistk",0, NULLQUARK, Cistk_Create,
			  NULL, NULL, False,
			  NULL, NULL, NULL};
#endif CISTK


  /* The array of node types known to the Answer Garden */
  /* To create a new type, add it in here.  5/6/91  */
  /* Make sure that noaction_obj is always present */
SortaObj *sorta_objs[] = 
{
  &sBrowser_obj,
  &grapher_obj,
  &discussion_obj,
  &qa_obj,
  &code_obj,
#ifdef SAO
  &pfedit_obj,
  &pfload_obj,
#endif
#ifdef JOURNAL
  &journal_obj,
#endif
#ifdef CISTK
  &cistk_obj,
#endif
};


/************************

  GLOBALS 

************************/

GlobalInfo global_info;

#define offset(field) XtOffsetOf(GlobalInfo, field)
static XtResource resources[] = { 
    {XtNdirectory, XtCDirectory, XtRString, sizeof(String),
	 offset(directory), XtRString, (XtPointer)NULL},
    {XtNmailerType, XtCMailerType, XtRString, sizeof(String),
	 offset(mailer_type), XtRString, (XtPointer)"mh"},
    {XtNeditMode, XtCEditMode, XtRBoolean, sizeof(Boolean),
	 offset(edit_mode), XtRString, (XtPointer)"False"},
    {XtNstatisticsFile, XtCStatisticsFile, XtRString, sizeof(String),
         offset(statistics_file), XtRString, (XtPointer)NULL},
    {XtNglobalExpertList, XtCExpertList, XtRString, sizeof(String),
         offset(global_expert_list), XtRString, (XtPointer)NULL},
    {XtNorganization, XtCOrganization, XtRString, sizeof(String),
         offset(organization), XtRString, (XtPointer)NULL},
    {XtNmailName, XtCMailName, XtRString, sizeof(String),
         offset(mail_name), XtRString, (XtPointer)NULL},
    {XtNmailMachineName, XtCMailMachineName, XtRString, sizeof(String),
         offset(mail_machine_name), XtRString, (XtPointer)NULL},
    {XtNdomainName, XtCDomainName, XtRString, sizeof(String),
         offset(domain_name), XtRString, (XtPointer)NULL},
    {XtNnodeExpert, XtCNodeExpert, XtRString, sizeof(String),
         offset(node_expert), XtRString, (XtPointer)NULL},
    {XtNstartupNode, XtCStartupNode, XtRString, sizeof(String),
         offset(startup_node), XtRString, (XtPointer)NULL},
    {XtNgrapherMaxHeight, XtCHeight, XtRDimension, sizeof(Dimension),
         offset(grapher_max_height), XtRImmediate, (XtPointer)MAXSHORT},
    {XtNgrapherMaxWidth, XtCWidth,  XtRDimension, sizeof(Dimension),
         offset(grapher_max_width), XtRImmediate, (XtPointer)MAXSHORT},
    {XtNlocalExpertList, XtCExpertList, XtRString, sizeof(String),
         offset(local_expert_list), XtRString, (XtPointer)NULL},
    {XtNuseMail, XtCUseMail, XtRBoolean, sizeof(Boolean),
	 offset(use_mail), XtRString, (XtPointer)"True"},
    {XtNuseLocalExpert, XtCUseLocalExpert, XtRBoolean, sizeof(Boolean),
	 offset(use_local_expert), XtRString, (XtPointer)"False"},
    {XtNtrackingServerAddress, XtCServerAddress, XtRString, sizeof(String),
	 offset(tracking_server_address), XtRString, (XtPointer)NULL},
    {XtNnStacks, XtCNStacks, XtRInt, sizeof(int),
       offset(n_stacks),XtRImmediate,(XtPointer)0},

};
#undef offset

Globals_Initialize(shell)
     Widget shell;
{
  int i;
  char buffer[MaxString];

  /* Set up the global_info structure.  We need to copy 
     the Directory and mailer type into our space.  Directory
     is where to find the information-base for AG; mailer
     type is which mailer to use. */
  XtGetApplicationResources(shell,(XtPointer) &global_info,
			    resources,XtNumber(resources), NULL, 0);

  global_info.main_shell = shell;
  global_info.directory = XtNewString(global_info.directory);
  global_info.mailer_type = XtNewString(global_info.mailer_type);
  global_info.statistics_file = XtNewString(global_info.statistics_file);
  global_info.global_expert_list = XtNewString(global_info.global_expert_list);
  global_info.organization = XtNewString(global_info.organization);
  global_info.mail_name = XtNewString(global_info.mail_name);
  global_info.mail_machine_name = XtNewString(global_info.mail_machine_name);
  global_info.domain_name = XtNewString(global_info.domain_name);
  global_info.node_expert = XtNewString(global_info.node_expert);
  global_info.startup_node = XtNewString(global_info.startup_node);
  global_info.local_expert_list = XtNewString(global_info.local_expert_list);
  global_info.tracking_server_address = 
    XtNewString(global_info.tracking_server_address);

  if (global_info.n_stacks < 0)
    global_info.n_stacks = 0;
  if (global_info.n_stacks > 0)
    global_info.stacks = (Widget *)
      XtMalloc(sizeof(Widget) * global_info.n_stacks);
  for (i=0;i<global_info.n_stacks;i++)
    {
      sprintf(buffer,"stack%d",i+1);
      global_info.stacks[i] = XtVaCreateWidget(buffer,stackObjClass,
					       global_info.main_shell,NULL);
    }
  
  global_info.AG_quark = XrmStringToQuark("AnswerGarden");
  global_info.icon_context = XUniqueContext();
  global_info.n_sorta_objs = XtNumber(sorta_objs);
  global_info.history_list = NULL;
  /* let's see if I can get away with this */
  global_info.global_func_obj = XtVaCreateWidget("GlobalFuncObj",
						 funcObjClass,
						 global_info.main_shell,
						 NULL);
					       
  
  if (global_info.directory == NULL)
    {
      XtWarning("This session with Answer Garden cannot find the directory");
      XtWarning("\twith the data files.  Check the XAPPLRESDIR environment");
      XtWarning("\tvariable (which should point at the application resource");
      XtWarning("\tcalled AnswerGarden).  If that is correct, then check");
      XtWarning("\tto make sure that the Directory (or directory) resource");
      XtWarning("\tis set to the correct directory for the Answer Garden");
      XtWarning("\tinformation files.");
      XtWarning("AnswerGarden is exiting...\n");
      exit(127);
    }
  
}
