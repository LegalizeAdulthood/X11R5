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

***********************************************************/
#ifndef _AnswerG_h
#define _AnswerG_h

#include <ctype.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include "AGmacros.h" /* some string macros to handle Suns */

/* These will mutate into site resources */
  /* where to find the information-base */


/* The choices for 
   XtNmailerType are BinMail and MH (capitalization doesn't matter)
*/


#define XtNdirectory "directory"
#define XtNmailerType "mailerType"
#define XtNmailOrganizationName "mailOrganizationName"
#define XtNeditMode "editMode"
#define XtNstatisticsFile "statisticsFile"
#define XtNglobalExpertList "globalExpertList"
#define XtNorganization "organization"
#define XtNmailName "mailName"
#define XtNmailMachineName "mailMachineName"
#define XtNdomainName "domainName"
#define XtNnodeExpert "nodeExpert"
#define XtNstartupNode "startupNode"
#define XtNcloseCallback "closeCallback"
#define XtNhelpCallback "helpCallback"
#define XtNquestionCallback "questionCallback"
#define XtNgrapherMaxHeight "grapherMaxHeight"
#define XtNgrapherMaxWidth "grapherMaxWidth"
#define XtNuseMail "useMail"
#define XtNlocalExpertList "localExpertList"
#define XtNuseLocalExpert "useLocalExpert"
#define XtNtrackingServerAddress "trackingServerAddress"
#define XtNstack "stack"
#define XtNnStacks "nStacks"

#define XtCDirectory "Directory"
#define XtCMailerType "MailerType"
#define XtCMailOrganizationName "MailOrganizationName"
#define XtCEditMode "EditMode"
#define XtCStatisticsFile "StatisticsFile"
#define XtCOrganization "Organization"
#define XtCMailName "MailName"
#define XtCMailMachineName "MailMachineName"
#define XtCDomainName "DomainName"
#define XtCNodeExpert "NodeExpert"
#define XtCStartupNode "StartupNode"
#define XtCExpertList "ExpertList"
#define XtCUseMail "UseMail"
#define XtCUseLocalExpert "UseLocalExpert"
#define XtCServerAddress "ServerAddress"
#define XtCStack "Stack"
#define XtCNStacks "NStacks"

  /* where to temporarily store the mail message */
#define MailUserFile ".agdraft"

  /* For data collection */
#define MailQuestionNotificationList "ackerman@athena.mit.edu"
#define AGVersionNumber "0.78"



#define MaxString 256

typedef struct _FileInfo FileInfo, *FileInfoPtr;

typedef struct _NodeServiceListMember NodeInfo, *NodeInfoPtr;

typedef struct _EditInfo EditInfo, *EditInfoPtr;

  /* used to keep track of user history */
typedef struct _history_node
{
  char *node_name;
  char *type_name;
  char time[22]; /* could be 19 or even 18 */
  char *next;    /* link to next node */
} HistoryNode;



  /* The user settings -- will be expanded.  See AnswerGarden.c for
     defaults - these will mutate into application resources. */
typedef struct _user_settings
{
    Boolean auto_placement;
    Boolean auto_delete;
} UserSettings;

#define EOS '\0'

#define AGNodeTypes int

typedef enum {
  ShowNone,
  Organization,
  Author,
} ShowAuthor;

typedef enum {
  NotMeaningful,
  Low,
  High,
} AuthorityLevel;

typedef enum {
/*  NotMeaningful,*/  /**????***/
  ShortAnswer,
  Discourse,
} ViewType;


typedef struct _AG_Nonpredefined_Header
{
  XtPointer magic;
  char *label;
  char *value;
  struct _AG_Nonpredefined_Header *next;
} AG_Nonpredefined_Header;

/*
The header should contain at least the following labels:

static char *header_array =
{
  "author",
  "show_author",
  "author_organization",
  "node_expert",
  "expiration_date",
  "last_modifier",
  "last_mod_date",
  "mod_num",

};

This array is contained in FileServ.c.
*/

#define FileServHeaderAuthor 0
#define FileServHeaderShowAuthor 1
#define FileServHeaderOrganization 2
#define FileServHeaderNodeExpert  3
#define FileServHeaderExpirationDate 4
#define FileServHeaderLastModifier  5
#define FileServHeaderLastModDate  6
#define FileServHeaderModNum  7

#define NFileServHeaderValues 8


FileInfo *AG_File_Open();
char *AG_File_Extract_Text();


#define AGTypeToken XrmQuark

typedef Widget (*AGCreateProc)();
typedef void (*AGClassCreateProc)();
typedef struct
{
  char *name;
  AGTypeToken token;
  XrmQuark name_quark;
  AGCreateProc create_function;
  AGClassCreateProc class_create_function;
  AGClassCreateProc initialize_function;
  Boolean virtual_node;
  Boolean class_created;
  XtCallbackProc help_callback;
  XtCallbackProc question_callback;
  XtCallbackProc close_callback;
} SortaObj;

typedef struct 
{
    String directory;
    String mailer_type;
    Boolean edit_mode;
    Widget main_shell;
    int n_sorta_objs;
    String statistics_file;
    String organization;
    String global_expert_list;
    String mail_name;
    String mail_machine_name;
    String domain_name;
    String node_expert;
    String startup_node;
    XrmQuark AG_quark;
    XContext icon_context;
    Widget  global_func_obj;
    Dimension grapher_max_height;
    Dimension grapher_max_width;
    Boolean use_mail;
    Boolean use_local_expert;
    String local_expert_list;
    String tracking_server_address;
    Widget history_list;
    int n_stacks;
    Widget *stacks;
} GlobalInfo;

/******* misc ************/


void General_Close_Callback();
void General_Help_Callback();
void General_Question_Callback();
void Dynamic_Close_Callback();
void Dynamic_Help_Callback();
void Dynamic_Question_Callback();

Widget AG_Create_Node();
void General_Question_Callback();
void General_Close_Callback();
void Open_TextBrowser();
char *getenv();
Boolean CommService_Initialize();
void AG_Raise_Shell();

/***** NODESERV.C *****/

char *NodeService_Get_Node_Name();
char *NodeService_Get_Label();
char *NodeService_Get_TypeString();
char *NodeService_Get_Location();
AGNodeTypes NodeService_Get_Type();
AGNodeTypes NodeService_Get_Type_From_String();
char **NodeService_Get_Defined_Headers();
char *NodeService_Get_Text();
Widget NodeService_Get_Shell();
EditInfo *NodeService_Get_EditNIPInfo();
EditInfo *NodeService_Get_EditFileInfo();
FileInfoPtr NodeService_Get_FileInfo();
Boolean NodeService_Set_Defined_Headers();
char *NodeService_Get_String_From_Type();


char *NodeSerice_Node_String_From_Type();
int NodeService_Node_Int_From_Type();
int NodeService_Node_Type_From_Int();

int NodeService_Request_Fields();
int NodeService_Request_By_Name();

void NodeService_Register_Open_Node();
void NodeService_Register_Open_File();
void NodeService_Register_Closed_Node();
void NodeService_Register_Closed_Edit();
Boolean NodeService_Has_Node_Changed();

NodeInfoPtr NodeService_Add_Node();
int NodeService_Replace_Node();


Boolean NodeService_Save_File();
Boolean NodeService_Save_New_File();
void NodeService_Close_File();
Boolean NodeService_Open_File();

Boolean NodeService_Check_NodeInfo();

int NodeService_Initialize();


/**** FILESERV.C *****/

AG_File_Filename_Set();
void Form_Filename();
Boolean Check_Node_Physical_Existance();

/**** UTIL.C ****/
void Util_Open_ButtonBox();
char *Util_Remove_Leading_WhiteSpace();
char *Util_Remove_WhiteSpace();
void Util_Get_Date();
void Util_Replace_String();
void Util_Get_UserName_With_Machine();
void User_History_List_Add_Member();

char *Remove_WhiteSpace();  
char *Remove_Control_Marks();
char *Remove_Tabs();

#endif /*_AnswerG_h*/



