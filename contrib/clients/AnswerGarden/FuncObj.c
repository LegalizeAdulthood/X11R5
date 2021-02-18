/**********************************************************

  The ANSWER GARDEN PROJECT

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


  Mark Ackerman
  MIT/Center for Coordination Science
  MIT/Project Athena

  ackerman@athena.mit.edu
  ackerman@xv.mit.edu

***********************************************************/


/* 
 * FuncObj.c - FuncObj object
 * 
 */

#include	<X11/IntrinsicP.h>
#include	<X11/StringDefs.h>
#include	"FuncObjP.h"
#include        "AGmacros.h"
#include        <ctype.h>

#define MAXCHILDREN  16
#define MAXBUF       256
#define ATSIGN       '@'
#define EOS          '\0'

/****************************************************************
 *
 * FuncObj Resources
 *
 ****************************************************************/

#define offset(field) XtOffset(FuncObj, field)
static XtResource resources[] = {
    {XtNcrushCallback, XtCCallback, XtRCallback, 
       sizeof(XtPointer),
       offset(funcObj.crush_callback), XtRCallback, (XtPointer)NULL},
    {XtNdynamicCallback, XtCCallback, XtRCallback, 
       sizeof(XtPointer),
       offset(funcObj.dynamic_callback), XtRCallback, (XtPointer)NULL},
    {XtNuseInstanceBlackboard, XtCUseInstanceBlackboard, XtRBoolean,
       sizeof(Boolean),
       offset(funcObj.use_instance_blackboard), XtRString, (XtPointer)"False"},
    {XtNblackboard, XtCBlackboard, XtRBlackboard,
       sizeof(BlackboardEntry *),
       offset(funcObj.blackboard), XtRBlackboard, (XtPointer)NULL},

};


    
/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

void Class_Part_Initialize();
void External_Handle();
int Parse_Command();
Boolean Trigger_Command();
void Destroy_Parsing_Tree();

#define SuperClass ((WidgetClass)&objectClassRec)

FuncObjClassRec funcObjClassRec = {
{
  
  /* superclass	        */	SuperClass,
  /* class_name	        */	"FuncObj",
  /* widget_size	*/	sizeof(FuncObjRec),
  /* class_initialize   */    NULL,
  /* class_part_initialize*/	Class_Part_Initialize,
  /* class_inited       */	FALSE,
  /* initialize	        */	NULL,
  /* initialize_hook    */	NULL,		
  /* pad                */    NULL,
  /* pad		  */	NULL,
  /* pad       	        */	0,
  /* resources	       */	resources,
  /* num_resources	*/      XtNumber(resources),
  /* xrm_class	        */	NULLQUARK,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* pad                */      FALSE,
  /* destroy		*/	NULL,
  /* pad		*/	NULL,
  /* pad		*/	NULL,
  /* set_values	        */      NULL,
  /* set_values_hook    */	NULL,			
  /* pad                */      NULL,
  /* get_values_hook    */	NULL,			
  /* pad                */      NULL,
  /* version		*/	XtVersion,
  /* callback_offsets   */      NULL,
  /* pad                */      NULL,
  /* pad                */      NULL,
  /* pad                */      NULL,
  /* extension	    */  NULL
},
{
  External_Handle,  /* one call does all */
  Parse_Command,    /* parse_command */
  Trigger_Command,  /* trigger_command */
  Destroy_Parsing_Tree, /* destroy_parsing_tree */
  False,                /* first_flag */
  NULL,                 /* blackboard */
  0,                    /* num_entries */
}
};

WidgetClass funcObjClass = (WidgetClass)&funcObjClassRec;



typedef struct _ParsingTree
{
  Boolean is_terminal;
  union
    {
      String terminal_token;
      struct _ParsingTreeSub
	{
	  int command;
	  struct _ParsingTree *children[MAXCHILDREN];
	  int nchildren;
	} non_terminal;
    } innards;
} ParsingTree;


/****************************************************************
 *
 * Private Routines
 *
 ****************************************************************/

static char *msg_string = 
  "%s Widget: The FuncObj Widget class method '%s' is undefined.";

static void Put_Class_Part_Msg(name,method)
     String name;
     String method;
{
  char buf[MAXBUF];
  sprintf(buf,msg_string,name,method);
  XtWarning(buf);
  XtWarning("A function must be defined or inherited.");
  XtWarning(buf);
}
  

  /* This code stolen from Xaw/Simple.c */
static void Class_Part_Initialize(class)
    WidgetClass class;
{
  register FuncObjClass foc = (FuncObjClass)class;
  
  if (foc->funcObj_class.external_handle == NULL)
    {
      Put_Class_Part_Msg(foc->object_class.class_name,"external_handle");
      foc->funcObj_class.external_handle = External_Handle;
    }
  if (foc->funcObj_class.external_handle == NULL)
    {
      Put_Class_Part_Msg(foc->object_class.class_name,"parse_command");
      foc->funcObj_class.parse_command = Parse_Command;
    }
  if (foc->funcObj_class.external_handle == NULL)
    {
      Put_Class_Part_Msg(foc->object_class.class_name,"trigger_command");
      foc->funcObj_class.trigger_command = Trigger_Command;
    }
  if (foc->funcObj_class.external_handle == NULL)
    {
      Put_Class_Part_Msg(foc->object_class.class_name,"destroy_parsing_tree");
      foc->funcObj_class.destroy_parsing_tree = Destroy_Parsing_Tree;
    }

    if (foc->funcObj_class.external_handle == XtInheritExternalHandle)
	foc->funcObj_class.external_handle = External_Handle;
    if (foc->funcObj_class.parse_command == XtInheritParseCommand)
	foc->funcObj_class.parse_command = Parse_Command;
    if (foc->funcObj_class.trigger_command == XtInheritTriggerCommand)
	foc->funcObj_class.trigger_command = Trigger_Command;
    if (foc->funcObj_class.destroy_parsing_tree == XtInheritDestroyParsingTree)
	foc->funcObj_class.destroy_parsing_tree = Destroy_Parsing_Tree;

}


#define DoProcess 0
#define DoSystem  1
#define DoClose   2
#define DoDynamic 3

#define NCommandStrings 5


static char *commandStrings[NCommandStrings] = {
  "process",
  "system",
  "close",
  "crush",
  "dynamic"
};

static int commandNumber[NCommandStrings] = 
{
  DoProcess,
  DoSystem,
  DoClose,
  DoClose,
  DoDynamic
};


static int nCommandStrings[NCommandStrings] = {7,6,5,5,7};


/*************************************************************************
    Find_Command
       routine determines what command string is next in the buffer

    incoming vars
        buffer     text buffer
	i          present location in the buffer
    return vars
        i          location at the end of the command string
	delimiter  what delimiter is being used for the command (ie,
	             next delimiter after the end of the command)
    returns
        command id (ie, index into various command arrays)
    error-returns
        value < 0

*/

static int Find_Command(buffer,i,delimiter)
     char *buffer;
     int *i;
     int *delimiter;
{
    int j;
    int command;
    int string;
    

    for (j=0;j<NCommandStrings;j++)
	if (strncmplo(&(buffer[*i]),commandStrings[j],
			nCommandStrings[j]) == 0)
	    break;

    if (j >= NCommandStrings)
	return(-1);


    command = commandNumber[j];
#ifdef OLD
    string = j;
    if ((*delimiter = Find_Delimiter(buffer,*i+nCommandStrings[string])) < 0)
	return(*delimiter);
    
    *i = *i+nCommandStrings[string];
#endif
    return(command);
}
	       

static ParsingTree *Create_Parse_Node(buf,start,end,parent,root)
     char *buf;
     int start;
     int end;
     ParsingTree *parent;
     ParsingTree **root;
{
  ParsingTree *child;
  int command;
  int temp;
  int delimiter;
  int i;
  char msgbuf[MAXBUF];
  char string[MAXBUF];

  if ((child = (ParsingTree  *)XtMalloc(sizeof(ParsingTree))) == NULL)
      {
	Util_Debug("out of space in CreateParseNode");
	return(NULL);
      }
  if (buf[start] == ATSIGN)
    {
      /* correct Find_Command API later */
      i = start+1;
      if ((command = Find_Command(buf,&i,&delimiter)) < 0)
	{
	  AGMakeString(string,&buf[start],&buf[end]);
	  if (AGstrlen(string) < MAXBUF/2) /* arbitrary */
	    sprintf(msgbuf,"unknown command %s in CreateParseNode",string);
	  else
	    sprintf(msgbuf,"unknown command (too long) in CreateParseNode",
		    string);
	  Util_Debug(msgbuf);
	  return(NULL);
	}
      child->is_terminal = False;
      child->innards.non_terminal.command = command;
      child->innards.non_terminal.nchildren = 0;
    }
  else
    {
      AGMakeString(string,&buf[start],&buf[end]);
      child->is_terminal = True;
      child->innards.terminal_token = XtNewString(string);
    }

  if (parent == NULL) /* it should be the root */
    {
      *root = child;
      return(child);
    }
      
  if (parent->is_terminal == True)
    {
      Util_Debug("trying to have child of terminal in CreateParseNode");
      return(NULL);
    }
  temp = parent->innards.non_terminal.nchildren++;
  parent->innards.non_terminal.children[temp] = child;
  return(child);
}

    
#define CheckForLeftDelimiter(c)  \
  (c == '(' ||  c == '[' ||  c == '<' ||  c == '{')

#define CheckForRightDelimiter(c)  \
  (c == ')' ||  c == ']' ||  c == '>' ||  c == '}')

#define CheckForFieldSeparator(c)  \
  (c == ',' )

static int Parse_Command(buf,location,max_buf,parent,root)
     char *buf;
     int *location;
     int max_buf;
     ParsingTree *parent;
     ParsingTree **root;
{
  ParsingTree *child;
  int start;
  int rc;

  rc = 1;
  while (*location < max_buf && isspace(buf[*location]))
    (*location)++;
  
  if (*location >= max_buf)
    {
      Util_Debug("past end of buffer in ParseCommand");
      return(-1);
    }

  start = *location;
  while (*location < max_buf)
    {
      if (CheckForLeftDelimiter(buf[*location]))
	{
	  if ((child = Create_Parse_Node(buf,start,*location-1,parent,root)) 
	      == NULL)
	    return(-1);
	  ++(*location);
	  rc = Parse_Command(buf,location,max_buf,child,root);
	  return(rc);
	}
      else if (CheckForRightDelimiter(buf[*location]))
	{
	  if ((child = Create_Parse_Node(buf,start,*location-1,parent,root)) 
	      == NULL)
	    return(-1);
	  return(1);
	}
      else if (CheckForFieldSeparator(buf[*location]))
	{
	  if (start == *location)
	    return(1);
	  if ((child = Create_Parse_Node(buf,start,*location-1,parent,root)) 
	      == NULL)
	    return(-1);
	  ++(*location);
	  rc = Parse_Command(buf,location,max_buf,parent,root);
	  return(rc);
	}
      ++(*location);
    }
  return(rc);
}
				    





static Boolean Trigger_Command(fob,parsing_tree)
     FuncObj fob;
     ParsingTree *parsing_tree;
     
{
  int i;
  ParsingTree *child;
  char buffer[MAXBUF];
  int nchildren;
  FuncObjReturnStruct return_struct;
  String *params;

  if (!parsing_tree->is_terminal)
    switch(parsing_tree->innards.non_terminal.command)
      {
      case DoProcess:
      case DoSystem:
	child = parsing_tree->innards.non_terminal.children[0];
	if (child != NULL)
	  system(child->innards.terminal_token);
	return(True);
      case DoClose:
	child = parsing_tree->innards.non_terminal.children[0];
	if (child != NULL)
	  XtCallCallbackList(fob,fob->funcObj.crush_callback,
			   (XtPointer)child->innards.terminal_token);
	return(True);
      case DoDynamic:
	nchildren = parsing_tree->innards.non_terminal.nchildren;
	if (nchildren > 0)
	  {
	    if ((params = (String*)
		 XtMalloc(sizeof(String)*nchildren)) == NULL)
	      return(False);
	    return_struct.nparams = nchildren;
	    for (i=0;i<nchildren;i++)
	      {
		child = parsing_tree->innards.non_terminal.children[i];
		params[i] = child->innards.terminal_token;
	      }
	    return_struct.params = params;
	    XtCallCallbackList(fob,fob->funcObj.dynamic_callback,
			       (XtPointer)&return_struct);
	    XtFree((char *)params);
	    return(True);
	  }
	return(False);
	
      default:
	/* shouldn't be here */
	break;
      }
  
  return(True);
}


static void External_Handle(w,buffer,loc,error_flag,parse_only)
     Widget w;
     char *buffer;
     int *loc;
     Boolean *error_flag;
     Boolean parse_only;
{
  FuncObj fob = (FuncObj)w;
  ParsingTree *parsing_root;
  FuncObjClass fob_class = (FuncObjClass)(fob->object.widget_class);

  /* TODO: ***
     If this is doing parse_only, it's doing a lot of work for nothing.
     Probably this ought to just find the end delimiter and call it
     a day - or pass the tree back to the caller in case it wants to
     cache it. */

  parsing_root = NULL;
  if ((fob_class->funcObj_class.parse_command)
      (buffer,loc,AGstrlen(buffer),NULL,&parsing_root) < 0)
    {
      *error_flag = True;
    }
  else
    if (!parse_only)
      (fob_class->funcObj_class.trigger_command)(fob,parsing_root);

  (fob_class->funcObj_class.destroy_parsing_tree)(parsing_root);
}

  
static void Destroy_Parsing_Tree(parsing_tree)
     ParsingTree *parsing_tree;
{
  int i;
  
  if (parsing_tree == NULL)
    return;

  if (parsing_tree->is_terminal)
    {
      if (parsing_tree->innards.terminal_token != NULL)
	XtFree(parsing_tree->innards.terminal_token);
    }
  else
    {
      for (i=0;i<parsing_tree->innards.non_terminal.nchildren;i++)
	Destroy_Parsing_Tree(parsing_tree->innards.non_terminal.children[i]);
    }
  XtFree(parsing_tree);
  return;
}

void AGFuncObjHandle(w,buffer,location,error_flag,parse_only)
     Widget w;
     char *buffer;
     int *location;
     Boolean *error_flag;
     Boolean parse_only;
{
  FuncObj fob = (FuncObj)w;
  FuncObjClass fob_class = (FuncObjClass)(fob->object.widget_class);
  (fob_class->funcObj_class.external_handle)
      (w,buffer,location,error_flag,parse_only);
}








