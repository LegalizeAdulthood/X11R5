/**********************************************************

  The ANSWER GARDEN PROJECT

  Util.c        misc. utility routines, statistics routines

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
**********************************************************************/
#include <ctype.h>
#include <stdio.h>

#include <X11/Xos.h>  /* includes appropriate time header */
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "AG.h"
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>

#include <pwd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
   
#ifdef macII
#  include <time.h>
#endif

#ifdef SYSV
#  include <time.h>
#else
#  ifdef vms
#    include <time.h>
#  else
#    ifdef CRAY
#      ifndef __TYPES__
#        define __TYPES__
#        include <sys/types.h>
#      endif
#    else
#       include <sys/types.h>
#    endif
#    include <sys/time.h>
#  endif
#endif

extern GlobalInfo global_info;

char *NodeService_Request_Node_Label();
char *getlogin();


  /* drop all ascii to lower case */
#define makelower(z)   (isupper(z) ? tolower(z) : z)

  /* Do a strcmp that is case insensitive */
strcmplo(x,y)
     register char *x;
     register char *y;
{
  if (x == NULL)
    {
      if (y != NULL)
	return(1);
      else 
	return(0);
    }
  if (y == NULL)
    return (-1);
  while (*x != (char)0 && *y != (char)0)
    if (makelower(*x) == makelower(*y))
      {
        ++x;
        ++y;
      }
    else
      break;

  /* structure assuming that most will be the same */
  if (makelower(*x) == makelower(*y))
    return(0);
  else if (makelower(*x) > makelower(*y))
    return(1);
  else
    return(-1);
}


  /* Do a strncmp that is case insensitive. */
strncmplo(x,y,n)
     register char *x;
     register char *y;
     int n;
{
  int i = 0;
  while (*x != (char)0 && *y != (char)0 && ++i < n)
    if (makelower(*x) == makelower(*y))
      {
        ++x;
        ++y;
      }
    else
      break;

  /* structure assuming that most will be the same */
  if (makelower(*x) == makelower(*y))
    return(0);
  else if (makelower(*x) > makelower(*y))
    return(1);
  else
    return(-1);
}

char *Remove_Tabs(str,str2)
     char *str;
     char *str2;
{

    register char *ptr = str;
    register char *ptr2 = str2;

    while ( *ptr != EOS)
	if (*ptr != '\t')
	    *(ptr2++) = *(ptr++);
	else
	    ptr++;

    *ptr2 = EOS;
    return(str2);
}

char *Remove_Control_Marks(str,str2)
     char *str;
     char *str2;
{

    register char *ptr = str;
    register char *ptr2 = str2;

    while ( *ptr != EOS)
	if (!(isspace(*ptr)) || *ptr != ' ')
	    *(ptr2++) = *(ptr++);
	else
	    ptr++;

    *ptr2 = EOS;
    return(str2);
}

  /* Utility to remove all characters considered space
     characters under Unix isspace() macro */
char *Util_Remove_WhiteSpace(str,str2)
     char *str;
     char *str2;
{

    register char *ptr = str;
    register char *ptr2 = str2;

    while ( *ptr != EOS)
	if (!(isspace(*ptr)))
	    *(ptr2++) = *(ptr++);
	else
	    ptr++;

    *ptr2 = EOS;
    return(str2);  /* returns corrected string */
}

  /* Same as Util_Remove_WhiteSpace except
     exit on first carriage return or linefeed */
char *Util_Remove_WhiteSpace_Until_LineEnd(str,buffer)
     char *str;
     char *buffer;
{
    register char *ptr = str;
    register char *ptr2 = buffer;

     /* Get rid of any space characters */    
    while (*ptr != EOS )
	if (!isspace(*ptr))
	    *(ptr2++) = *(ptr++);
	else 
	  if (*ptr != '\n' && *ptr != '\r')
	    ptr++;
	  else
	    break;
    
    *ptr2 = EOS;
    return(ptr);  /* returns ptr to the last char in the initial string */
}

  /* Same as Util_Remove_WhiteSpace except
     only removes leading whitespace. */
char *Util_Remove_Leading_WhiteSpace(str,buffer)
     char *str;
     char *buffer;
{
    register char *ptr = str;
    register char *ptr2 = buffer;
    register Boolean flag = False;

     /* Get rid of any space characters */    
    while (*ptr != EOS )
	if (!isspace(*ptr))
	  {
	    flag = True;
	    *(ptr2++) = *(ptr++);
	  }
        else if (*ptr == '\n' || *ptr == '\r')
	  break;
        else if (flag)
	    *(ptr2++) = *(ptr++);
        else 
	    ptr++;

    *ptr2 = EOS;
    return(buffer);  /* returns ptr to corrected string */
}

  /* Linear search through a (small) array.  Returns index of
     element in array or -1 if string is not in array. */
int Util_Search_Array(array,string,nstrings)
     char *array[];
     char *string;
     int nstrings;
{
  int i;
  for (i=0;i<nstrings;i++)
    if (!strcmplo(array[i],string))
      return(i);
  return(-1);
}



Util_Move_Token_To_String(newbuffer,buffer,nchar)   
     char *newbuffer;
     char *buffer;
     int nchar;
{
  if (buffer)
    {
      strncpy(newbuffer,buffer,nchar); 
      newbuffer[nchar] = EOS; 
    }
  else
    *newbuffer = EOS;
}



  /* Utility routine to return an ascii string with the 
     timestamp in it, including hour:minutes:seconds MM/DD/YY */
void Util_Get_Timestamp(timestamp)
     char *timestamp;
{
    long time_o_day;
    struct tm *temp;

    time_o_day = time(0);
    temp = localtime(&time_o_day);
    sprintf(timestamp,"%02d:%02d:%02d %02d/%02d/%02d",temp->tm_hour,
	    temp->tm_min,temp->tm_sec,(temp->tm_mon)+1,temp->tm_mday,
	    temp->tm_year);
}

  /* Utility routine that does the same as Util_Get_Timestamp,
     but also checks to see if the last time stamp was more than
     15 minutes ago.  If it was, the routine returns True; if not,
     the routine returns False. */
Boolean Util_Get_And_Check_Timestamp(timestamp)
     char *timestamp;
{
  long time_o_day;
  struct tm *temp;
  unsigned long current_day;
  unsigned long current_time;
  Boolean flag;
  unsigned long temp_time;
  static unsigned long old_day = 0;
  static unsigned long old_time = 0;

  flag = False;
  
  time_o_day = time(0);
  temp = localtime(&time_o_day);

  sprintf(timestamp,"%02d:%02d:%02d %02d/%02d/%02d",temp->tm_hour,
	  temp->tm_min,temp->tm_sec,(temp->tm_mon)+1,temp->tm_mday,
	  temp->tm_year);
  current_time = (long)( temp->tm_min) + (long)(temp->tm_hour)*100;
  current_day = (long)(temp->tm_yday) + (long)(temp->tm_year)*1000;
  if (current_day == old_day)
    {
      if (current_time - old_time > 15)
	flag = True;
    }
  else if (current_day - old_day > 1)
    flag = True;
  else /* check whether around midnight */
    {
      temp_time = current_time + 2400;
      if (temp_time - old_time > 15)
	flag = True;
    }
  old_time = current_time;
  old_day = current_day;
  return(flag);
}


void Util_Get_Date(date)
     char *date;
{
   long time_o_day;
   struct tm *temp;
   
   time_o_day = time(0);
   temp = localtime(&time_o_day);
   sprintf(date,"%02d/%02d/%02d",
	   (temp->tm_mon)+1,temp->tm_mday,
	   temp->tm_year);
   
}

Util_Get_LocalDate(month,day,year)
     int *month;
     int *day;
     int *year;
{
    long time_o_day;
    struct tm *temp;

    time_o_day = time(0);
    temp = localtime(&time_o_day);
    *month = temp->tm_mon + 1;
    *day = temp->tm_mday;
    *year = temp->tm_year;
}


void Util_Get_UserName(buffer)
     char *buffer;
{
  static char remember_name[MaxString] = "";
#ifndef SYS_V
   int uid;
#else
   unsigned short uid;
#endif
   struct passwd *pw;
   char *name;

  if (remember_name[0] != EOS)
    {
      AGstrcpy(buffer,remember_name);
      return;
    }

  if ((name = getlogin()) != NULL)
    AGstrcpy(buffer,name);
  else
    {
      uid = getuid();
      if ((pw = getpwuid(uid)) == NULL)
	AGstrcpy(buffer,"unknown");
      else
	AGstrcpy(buffer,pw->pw_name);
    }
  AGstrcpy(remember_name,buffer);
}

void Util_Get_UserInfo(buffer)
     char *buffer;
{
#ifndef SYS_V
   int uid;
#else
   unsigned short uid;
#endif
   struct passwd *pw;
   char *name;
   uid = getuid();
   if ((pw = getpwuid(uid)) == NULL)
     AGstrcpy(buffer,"unknown");
   else
     AGstrcpy(buffer,pw->pw_gecos);
}

void Util_Get_UserName_With_Machine(buffer)
   char *buffer;
{
   char hostname[MaxString];
   char user_name[MaxString];
   struct hostent *host_entry;
   char *official_name;
   char *ptr;

   host_entry = NULL;


     /* If there is a mail machine available, use it. */
   if (global_info.mail_name)
      {
	 AGstrcpy(buffer,global_info.mail_name);
	 return;
      }


   Util_Get_UserName(user_name);
   AGstrcpy(buffer,user_name);
   AGstrcat(buffer,"@");
   if (global_info.mail_machine_name)
     {
       AGstrcat(buffer,global_info.mail_machine_name);
       return;
     }

   gethostname(hostname,MaxString);
   if (*hostname >= '0' && *hostname <= '9') 
     {
       unsigned long host_address = inet_addr(hostname);
       if (host_address != -1)
	 host_entry = gethostbyaddr((char *) &host_address, 4, AF_INET);
     }
   if (! host_entry)
     host_entry = gethostbyname(hostname);
   official_name = host_entry->h_name;
   if ((ptr = AGindex(official_name,'.')) == NULL)
     {
       /* No domain name. */
       AGstrcat(buffer,official_name);
       AGstrcat(buffer,global_info.domain_name);
       /* if domain_name is NULL, there's not much I can do */
     }
   else 
     {
       /* Hope for the best and believe that it's a full 
	  domain name */
       AGstrcat(buffer,official_name);
     }
 
}




  /* Utility to replace a malloc'd string.  Routine frees old
     string, malloc's new string, and then assigns new string
     to variable.   Routine requires a pointer to the location. */
void Util_Replace_String(string_loc,value_string)
     char **string_loc;
     char *value_string;
{
  if (string_loc == NULL)
    return;
  if (*string_loc) 
    {
      XtFree(*string_loc);
    }
    /* Reset value of string anyway.  If value is NULL,
       XtNewString will return NULL */
  *string_loc = XtNewString(value_string);
}
  

    
/**********************

  HISTORY LIST

***********************/

    
static HistoryNode *history_list = NULL;
static HistoryNode *current_history_node = NULL;

static void User_History_Delete_List()
{
  HistoryNode *temp;
  HistoryNode *next;

  temp = history_list;
  while (temp)
    {
      next = (HistoryNode *)(temp->next);
      XtFree(temp);
      temp = next;
    }
  history_list = NULL;
  current_history_node = NULL;
}


void User_History_List_Add_Member(node_name,type_name)
     char *node_name;
     char *type_name;
{
    struct timeval tp;
    struct timezone tzp;
    HistoryNode *new_history_node;
    char time_stamp[MaxString];

    if ((new_history_node = 
	 (HistoryNode *)XtMalloc(sizeof(HistoryNode))) == NULL)
	{
	    XtWarning("cannot allocate memory for history node");
	    return;
	}

    if (Util_Get_And_Check_Timestamp(time_stamp))
      User_History_Delete_List();

    if (node_name != NULL)
	new_history_node->node_name = XtNewString(node_name);
    else
	new_history_node->node_name = XtNewString("NULL");

    if (node_name != NULL)
	new_history_node->type_name = XtNewString(type_name);
    else
	new_history_node->type_name = XtNewString("NULL");

    AGstrcpy(new_history_node->time,time_stamp);
    new_history_node->next = NULL;

    if (history_list == NULL)
	history_list = new_history_node;
    else
	current_history_node->next = (char *)new_history_node;
    current_history_node = new_history_node;
}



User_History_Print(fp)
     FILE *fp;
{
    HistoryNode *ptr;

    ptr = history_list;
    while (ptr)
	{
	    fprintf(fp,"UserHistory:\t((%s) (%s) (%s))\n",
		    ptr->node_name,ptr->type_name,
		    ptr->time);
	    ptr = (HistoryNode *)ptr->next;
	}
}


User_History_Make_List(label_array,name_array,max_entries,entries_returned)
     char *label_array[];
     char *name_array[];
     int max_entries;
     int *entries_returned;
{
    HistoryNode *ptr;
    int num;
    char *label;
    NodeInfoPtr node_info;
    AGNodeTypes type;
    char *location;
    int skip;
    int i;
    int j;

    num = 0;
    ptr = history_list;
    while (ptr)
	{
	  num++;
	  ptr = (HistoryNode *)ptr->next;
	}

    if (num > max_entries)
      skip = num - max_entries;
    else
      skip = 0;

    i = 0;
    j = 0;
    ptr = history_list;
    while (ptr)
	{
	  if (j++ < skip)
	    continue;
	  /* The list must stay constant, so I'm going to use
	     the node service names instead of the history list
	     names - the history list disappears after 15 minutes */
	  if ((NodeService_Request_By_Name(ptr->node_name,&node_info,
					   &label,&type,&location) >= 0))
	    {
	      name_array[i] = NodeService_Get_Node_Name(node_info);
	      label_array[i] = label;
	    }
	  /* else
	    {
	      skip it 
	    } */
	  i++;
	  ptr = (HistoryNode *)ptr->next;
	}
    *entries_returned = i;
}


char *Util_Find_Shell_Name(w)
     Widget w;
{
  Widget tw;
  
  tw = w;
  while (!XtIsSubclass(tw,shellWidgetClass))
    tw = XtParent(tw);
  return(XtName(tw));
}


/************************

  STATISTICS

************************/

#define StatOpenMacro() \
{ \
  if ((fp = fopen(global_info.statistics_file,"a")) == NULL) \
    { \
      XtWarning("unable to open statistics file.  Trying to continue..."); \
      XtWarning("  Please contact site administrator at once."); \
      return; \
    } \
}


Stat_Node_Create(w,node_info)
     Widget w;
     NodeInfoPtr node_info;
{
  FILE *fp;
  char buffer[MaxString];
  char **header_values;
  char *organization;
#ifndef NO_STAT
  Util_Get_UserName(buffer);
   
  /* In pathological cases (eg, nodes not in the NodeService), node_info may 
     come in as NULL.*/
  header_values = NodeService_Get_Defined_Headers(node_info);
  if (header_values != NULL)
    {
      organization = header_values[FileServHeaderOrganization];
      if (organization == NULL) /* an error condition */
	organization = "Unknown";
    }
  else 
    organization = "Not Determinable";
      
  StatOpenMacro();
  fprintf(fp,"NC %-20s %-16s opened \"%s\" (%s) of type \"%s\" (%s)\n",
	  buffer,
	  current_history_node->time,
	  current_history_node->node_name,
	  NodeService_Request_Node_Label(current_history_node->node_name),
	  current_history_node->type_name,
	  organization);
  fflush(fp);
  fclose(fp);
#endif
}

Stat_Start_User(w)
     Widget w;
{
  FILE *fp;
  char buffer[MaxString];

#ifndef NO_STAT
  Util_Get_UserInfo(buffer);

  StatOpenMacro();
  fprintf(fp,"UI %s\n",buffer);
  fflush(fp);
  fclose(fp);
#endif
}


Stat_Button_Hit(w)
     Widget w;
{
  FILE *fp;
  char *label;
  char timestamp[MaxString];
  char buffer[MaxString];
  char *shell_name;

#ifndef NO_STAT  
  label = NULL;
  XtVaGetValues(w,XtNlabel,&label,NULL);
  Util_Get_Timestamp(timestamp);
  Util_Get_UserName(buffer);

  shell_name = NULL;
  if (label != NULL)
    shell_name = Util_Find_Shell_Name(w);
  if (shell_name == NULL)
    shell_name = "No Name";

  StatOpenMacro();
  fprintf(fp,"BH %-20s %-16s hit button \"%s\" on node \"%s\"\n",
	  buffer,
	  timestamp,
	  label,
	  shell_name);

  fflush(fp);
  fclose(fp);
#endif
}


Stat_Sent_Mail(w)
     Widget w;
{
  FILE *fp;
  char *label;
  char timestamp[MaxString];
  char buffer[MaxString];

#ifndef NO_STAT  
  Util_Get_Timestamp(timestamp);
  Util_Get_UserName(buffer);

  StatOpenMacro();
  fprintf(fp,"SM %-20s %-16s sent mail\n",
	  buffer,
	  timestamp);
  fflush(fp);
  fclose(fp);
#endif
}

Stat_Node_Raise(w,node_info)
     Widget w;
     NodeInfoPtr node_info;
{
  FILE *fp;
  char *label;
  char timestamp[MaxString];
  char buffer[MaxString];

#ifndef NO_STAT  
  Util_Get_Timestamp(timestamp);
  Util_Get_UserName(buffer);

  StatOpenMacro();
  fprintf(fp,"NR %-20s %-16s raised \"%s\" (%s) of type \"%s\"\n",
	  buffer,
	  current_history_node->time,
	  current_history_node->node_name,
	  NodeService_Request_Node_Label(current_history_node->node_name),
	  current_history_node->type_name);
  fflush(fp);
  fclose(fp);
#endif
}

void Util_Open_Minimal_ButtonBox(parent,form,buttons)
     Widget parent;
     Widget *form;
     Widget buttons[3];
{
  Widget command[5];
  Widget box;
  
  box =  XtVaCreateManagedWidget("buttonBox",formWidgetClass,
				 parent,
				 XtNhorizDistance,(XtArgVal)0,
				 NULL);
  
  buttons[0] = XtVaCreateManagedWidget("button1",commandWidgetClass,
				       box, NULL);
  buttons[1] = XtVaCreateManagedWidget("button2",commandWidgetClass,
				       box, 
				       XtNfromHoriz, (XtArgVal)buttons[0],
				       NULL);
  buttons[2] = XtVaCreateManagedWidget("button3",commandWidgetClass,
				       box, 
				       XtNfromHoriz, (XtArgVal)buttons[1],
				       NULL);
   
  *form = box;
}

void Util_Open_ButtonBox(parent,form,buttons,
			 extra_buttons,extra_button_names,n_extra_buttons)
     Widget parent;
     Widget *form;
     Widget buttons[5];
     Widget extra_buttons[];
     char *extra_button_names[];
     int n_extra_buttons;
{
   Widget box;
   int i;

   Util_Open_Minimal_ButtonBox(parent,&box,buttons);

   if (n_extra_buttons > 0)
      {
	 extra_buttons[0] = 
	    XtVaCreateManagedWidget(extra_button_names[0],
				    commandWidgetClass, box,
				    XtNfromHoriz, (XtArgVal)buttons[2],
				    NULL);
       }

   for (i=1;i<n_extra_buttons;i++)
      {
	 extra_buttons[i] = 
	    XtVaCreateManagedWidget(extra_button_names[0],
				    commandWidgetClass,
				    box,
				    XtNfromHoriz, (XtArgVal)extra_buttons[i-1],
				    NULL);
      }

   if (n_extra_buttons > 0)
     buttons[3] = XtVaCreateManagedWidget("button4",commandWidgetClass,
					  box, 
					  XtNfromHoriz, (XtArgVal)
					  extra_buttons[n_extra_buttons-1],
					  NULL);
   else
     buttons[3] = XtVaCreateManagedWidget("button4",commandWidgetClass,
					  box, 
					  XtNfromHoriz, (XtArgVal)buttons[2],
					  NULL);
   
   buttons[4] = XtVaCreateManagedWidget("button5",commandWidgetClass,
					box, 
					XtNfromHoriz,(XtArgVal)buttons[3],
					NULL);

   *form = box;

}


Util_Debug(msg)
     char *msg;
{
#ifdef DEBUG
  fprintf(stderr,"%s\n",msg);
#endif
}

