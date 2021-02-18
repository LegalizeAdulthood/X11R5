/*
 *	$Source: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/dayeditor.c,v $
 *	$Header: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/dayeditor.c,v 1.2 1991/10/04 23:18:46 bingle Exp $
 *      $Author: bingle $
 *      $Locker:  $
 *
 * 	Copyright (C) 1988 Massachusetts Institute of Technology	
 *	
 */


/*

   Permission to use, copy, modify, and distribute this
   software and its documentation for any purpose and without
   fee is hereby granted, provided that the above copyright
   notice appear in all copies and that both that copyright
   notice and this permission notice appear in supporting
   documentation, and that the name of M.I.T. not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.
   M.I.T. makes no representations about the suitability of
   this software for any purpose.  It is provided "as is"
   without express or implied warranty.

*/

/** Written by Roman J. Budzianowski - Project Athena, MIT **/

#ifndef lint
static char *rcsid_dayeditor_c = "$Header: /p/p4/X11R5/contrib/purdue/xcalendar/RCS/dayeditor.c,v 1.2 1991/10/04 23:18:46 bingle Exp $";
#endif lint

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Box.h>
#include <X11/Shell.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/AsciiSrc.h>
#include <X11/Xaw/Text.h>
#include <X11/Xaw/Paned.h>

#include <sys/dir.h>
#include <sys/stat.h>

#include <errno.h>
extern int errno;

#include "xcalendar.h"


static XtCallbackRec callbacks[] = {
   {NULL,NULL},
   {NULL,NULL}
};

List            editorList  = NULL;
Boolean         initialized = False;
char            *homedir    = NULL;
char            *calendarDir;

extern Cardinal displayedMonth, displayedYear;
void SaveButtonHandler();


Boolean already_open();
void SaveText();

void ChangeDay(editor)
DayEditor editor;

{
  Arg  args[1];
  char mon[4];
  char title[80];

  strncpy(mon, smonth[editor->month], 3);
  mon[3] =  '\0';

  sprintf(editor->filename,"xc%d%s%d",editor->day,mon,editor->year);

  editor->open = False;

  if(!ReadFile(editor))
    return;

  editor->used = True; 

  sprintf(title," %d %s %d ",
	  editor->day, smonth[editor->month], editor->year);

  XtSetArg(args[0],XtNlabel,title);
  XtSetValues(editor->title,args,1);

  XtSetArg(args[0],XtNiconName,title);
  XtSetValues(editor->shell,args,1);

  XtSetSensitive(editor->saveButton,False);

  XtAddEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,
	(caddr_t)editor);

  if(EmptyBuffer(editor) || already_open(editor->filename))
    XtSetSensitive(editor->clearEntry,False);
  else
    XtSetSensitive(editor->clearEntry,True);

  if (!already_open(editor->filename)) 
    editor->open = True;
}


void PrevDay(w, closure, call_data)
Widget w;
caddr_t closure;
caddr_t call_data;
{
    DayEditor editor = (DayEditor) closure;
    Cardinal numberofDays, firstDay;

    XtUnmapWidget(w);

    if (editor->saved == False) {
	SaveText(w, closure, call_data);
    }
    if (editor->day == 1) {
       if (editor->month == 1) {
	  editor->month = 12;
	  editor->year--;
       }
       else 
	  editor->month--;
       numberofDays = NumberOfDays(editor->month, editor->year);
       editor->day = numberofDays;
    }
    else 
       editor->day--;

    ChangeDay(editor);
    XtMapWidget(w);
}


void SuccDay(w, closure, call_data)
Widget w;
caddr_t closure;
caddr_t call_data;
{
    DayEditor editor = (DayEditor) closure;
    Cardinal numberofDays, firstDay;

    XtUnmapWidget(w);

    if (editor->saved == False) {
	SaveText(w, closure, call_data);
    }
    numberofDays = NumberOfDays(editor->month, editor->year);
    if (editor->day == numberofDays) {
       if (editor->month == 12) {
	  editor->month = 1;
	  editor->year++;
       }
       else 
	  editor->month++;
       editor->day = 1;
    }
    else 
       editor->day++;

    ChangeDay(editor);
    XtMapWidget(w);
}

Boolean already_open(filename)
char *filename;
{
	DayEditor editor;
	int i;
	
	if (editorList == NULL)
	    return(False);

	for (i=1; i<= ListLength(editorList); i++) {
	    editor = GetEditorFromList(editorList, i);
	    if (!strcmp(editor->filename, filename)) {
		if (editor->open == True) {
		    return(True);
		}
	    }
	}
	return(False);	
}

Boolean InitEditors()
{
   char                 *getenv();
   DIR                  *dirp;
   struct direct	*dp;
   char                 yearStr[5];
   Cardinal             firstDay;
   int                  error_status;
   struct stat          dir_status;

   sprintf(yearStr,"%d",displayedYear);
   firstDay = FirstDay(month,year);

   /* open the ~/Calendar directory - create if necessary */
   homedir = getenv("HOME");

   calendarDir = XtMalloc(strlen(homedir) + 10);
   strcpy(calendarDir,homedir);
   strcat(strcat(calendarDir,"/"),"Calendar");

   error_status = stat(calendarDir,&dir_status);

   if(error_status == -1)
     if(errno == ENOENT){		/* not found */
      /* it should be a popup asking the user ...*/
      fprintf(stderr,"%s doesn't exist : Creating...",calendarDir);

      if( mkdir(calendarDir,0700) == -1 ){
	 XBell( XtDisplay(toplevel) ,100);
	 fprintf(stderr,"Couldn't create %s directory.\n");
	 perror("xcalendar:mkdir:");
	 fflush(stderr);
	 return False;
      }
   }
   else{
      XtWarning("Cannot stat ~/Calendar directory");
      return False;
   }

   chdir(calendarDir);

   dirp = (DIR *) opendir(calendarDir);

   if(dirp == NULL) {
      XtWarning("Cannot open  ~/Calendar (maybe it's not a directory)");
      return False;
   }

   while	((dp = readdir(dirp)) != NULL){
      char *s;
      /* find entries from the displayedMonth and store them in a list */

      s = (char *)index(dp->d_name,*smonth[displayedMonth]);

      if (s != NULL && !strncmp(s+3,yearStr,4) && !strncmp(s,smonth[displayedMonth],3))
	MarkDayEntry(GetWidgetFromList(daylist,atoi(dp->d_name + 2) + firstDay -1),True);
   }
 
   if(!editorList)
     editorList = CreateList(5,sizeof(DayEditor));

   closedir(dirp);

   initialized = True;
   return True;
}

void EditDayEntry(w, closure, call_data)
       Widget w;
       caddr_t closure;
       caddr_t call_data;
{
  int i;
  DayEditor editor = NULL;
  Arg       args[1];
  char      title[80];
  Boolean   success;
  char      mon[4];

  if(!initialized){
     success = InitEditors();
     if(!success)
       return;
  }

  /* find free editor */

  for(i=1;i <= ListLength(editorList); i++){
    editor = GetEditorFromList(editorList,i);
    if(editor->used)
      editor = NULL;
    else
      break;
 }

  if(editor == NULL){
     editor = CreateDayEditor("dayEditor",XawtextEdit, XawtextWrapWord, NULL);
     PushEditorOnList(editorList,editor);
  }

  editor->button = w;
  editor->day    = (int)closure;
  editor->month  = displayedMonth;
  editor->year   = displayedYear;
  editor->saved  = True;

  editor->filename = XtMalloc(12);
  strncpy(mon,smonth[editor->month],3);
  mon[3]='\0';

  sprintf(editor->filename,"xc%d%s%d",editor->day,mon,editor->year);

  if(!ReadFile(editor))
    return;

  editor->used = True; 

  sprintf(title," %d %s %d ",
	  editor->day, smonth[editor->month], editor->year);

  XtSetArg(args[0],XtNlabel,title);
  XtSetValues(editor->title,args,1);

  XtSetArg(args[0],XtNiconName,title);
  XtSetValues(editor->shell,args,1);

  XtSetSensitive(editor->saveButton,False);

 
  XtAddEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,(caddr_t)editor);

  if(EmptyBuffer(editor) || already_open(editor->filename))
    XtSetSensitive(editor->clearEntry,False);
  else
    XtSetSensitive(editor->clearEntry,True);

  if (!already_open(editor->filename))
    editor->open = True;

  StartEditor(editor);

}

void SaveButtonHandler(w,data,event)
     Widget w;
     caddr_t data;
     XEvent *event;
{
   DayEditor editor = (DayEditor)data;
   char temp;

   if(XLookupString((XKeyEvent *)event,&temp,1,NULL,NULL) == 0)
     return;

   if (editor->open == False)
     return;

   XtSetSensitive(editor->saveButton,True);
   editor->saved=False;
   XtSetSensitive(editor->clearEntry,True);
   XtRemoveEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,data);

}

void SaveText(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor editor = (DayEditor)closure;
   int saveStatus;

   if(editor->filename == NULL)
     return;

   if (editor->open == False)
     return;
  
   if((saveStatus=write_to_file(editor->filename,editor->buffer,TextSize(editor))) == -1 )
     return;

   if(saveStatus == 0){

      if (editor->month == displayedMonth && editor->year == displayedYear)
      MarkDayEntry(GetWidgetFromList(daylist, 
				     editor->day + FirstDay(displayedMonth,displayedYear) - 1),
		   False);
      XtSetSensitive(editor->clearEntry,False);
   }
   else
     if (editor->month == displayedMonth && editor->year == displayedYear)
       MarkDayEntry(GetWidgetFromList(daylist, editor->day +
				      FirstDay(displayedMonth,displayedYear) - 1),True);
   
   XtSetSensitive(editor->saveButton,False);
   
   XtAddEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,(caddr_t)editor);

   editor->saved = True;
}

void ExitEditor(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor editor = (DayEditor)closure;

   if(editor->saved == False)
     SaveText(w,closure,call_data);
  
   XtRemoveEventHandler(editor->editor,KeyPressMask,False,
			SaveButtonHandler,(caddr_t)editor);
   
   editor->used = False;

   XtPopdown(editor->shell);
   
   editor->open = False;
   editor->year = displayedYear;
   editor->month = displayedMonth;
   if (editor->button == help_button)
       XtSetSensitive(editor->button,True);
}


void ClearEntry(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor    editor = (DayEditor)closure;

   bzero(editor->buffer,appResources.textBufferSize);

   ChangeTextSource(editor,editor->buffer);

   SaveText(w,closure,call_data);

}


DayEditor CreateDayEditor(name,mode,options,initialtext)
     char           *name;
     XawTextEditType mode;
     int            options;
     char           *initialtext;
{
   DayEditor editor = (DayEditor)XtCalloc(1,sizeof(DayEditorRec));
   Arg       args[7];
   Widget    frame,title,text,controls;

   Widget    daybuttons;

   editor->shell = XtCreatePopupShell(name,topLevelShellWidgetClass,toplevel, NULL, 0);

   frame = XtCreateWidget("editorFrame", panedWidgetClass, editor->shell, args, 0);

   daybuttons = XtCreateManagedWidget("daybuttons", formWidgetClass, frame, args, 0);

   callbacks[0].callback = PrevDay;
   callbacks[0].closure = (caddr_t)editor;
   XtSetArg(args[0], XtNcallback, callbacks);
   XtSetArg(args[1], XtNfromHoriz, NULL);
   XtSetArg(args[2], XtNleft, XtChainLeft);
   XtSetArg(args[3], XtNright, XtChainLeft);

   editor->prevday = XtCreateManagedWidget("prevday", commandWidgetClass, 
		     daybuttons, args, 4);

    /** create title **/

   XtSetArg(args[0],XtNskipAdjust,True);

   XtSetArg(args[1], XtNfromHoriz, editor->prevday);
   XtSetArg(args[2], XtNleft, XtChainLeft);
   XtSetArg(args[3], XtNright, XtChainRight);

   title = XtCreateManagedWidget("editorTitle", labelWidgetClass, daybuttons, 
		args, 4);
   editor->title = title;

   editor->mode = mode;
   editor->buffer = XtCalloc(appResources.textBufferSize, sizeof(char));

   callbacks[0].callback = SuccDay;
   callbacks[0].closure = (caddr_t)editor;
   XtSetArg(args[0], XtNcallback, callbacks);
   XtSetArg(args[1], XtNfromHoriz, title);
   XtSetArg(args[2], XtNleft, XtChainRight);
   XtSetArg(args[3], XtNright, XtChainRight);

   editor->succday = XtCreateManagedWidget("succday", commandWidgetClass, 
		     daybuttons, args, 4);
   
   if(initialtext) strcpy(editor->buffer,initialtext);

   editor->bufSize = appResources.textBufferSize;

   XtSetArg(args[0],XtNstring, editor->buffer);
   XtSetArg(args[1],XtNeditType,mode);
   XtSetArg(args[2],XtNlength,appResources.textBufferSize);
   XtSetArg(args[3],XtNwrap, options);

   text = XtCreateManagedWidget("editor", asciiTextWidgetClass, frame, args, 4);
   editor->editor = text;

   XtSetArg(args[0],XtNfromVert,text); 
   XtSetArg(args[1],XtNskipAdjust,True);
   controls = XtCreateManagedWidget("editorControls", boxWidgetClass,frame, args,2);

   callbacks[0].callback = ExitEditor;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   XtCreateManagedWidget("doneButton", commandWidgetClass, controls, args, 1);

   callbacks[0].callback = SaveText;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   editor->saveButton = XtCreateManagedWidget("saveButton", commandWidgetClass, controls, args, 1);

   callbacks[0].callback = ClearEntry;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   editor->clearEntry = XtCreateManagedWidget("clearEntry", commandWidgetClass, controls, args, 1);

   XtManageChild(frame); 
   
   XtRealizeWidget(editor->shell);

   return editor;

}

Boolean ReadFile(editor)
       DayEditor editor;
{
   int size;
   Arg args[3];

   XtSetArg(args[0], XtNeditType, XawtextEdit);
   XtSetValues(editor->editor, args, 1);

   bzero(editor->buffer,editor->bufSize);

   size = read_file(editor->filename, editor->buffer, editor->bufSize);

   if(size == -1)
     return False;
   else if( size > editor->bufSize){
      editor->buffer = XtRealloc(editor->buffer,size + 1);
      editor->bufSize = size + 1;
      size = read_file(editor->filename, editor->buffer, editor->bufSize);
      if(size == -1)
	return False;
   }

#define ALREADYOPEN "	** Editor Already Open! ** \n\n"

   XtSetArg(args[1], XtNeditType, XawtextEdit);
   if (already_open(editor->filename)) {
	static char *sb_tmp;

	sb_tmp = XtMalloc(editor->bufSize + strlen(ALREADYOPEN) + 1);
	strcpy(sb_tmp, ALREADYOPEN);
	strcat(sb_tmp, editor->buffer);
	XtFree(editor->buffer);
	editor->buffer = sb_tmp;
	editor->bufSize = editor->bufSize + strlen(ALREADYOPEN) + 1;
	XtSetArg(args[1], XtNeditType, XawtextRead);
   }
   ChangeTextSource(editor,editor->buffer);

   XtSetArg(args[0], XtNstring, editor->buffer);
   XtSetValues(editor->editor, args, 2);

   return True;
}

void ChangeTextSource(editor,newtext)
     DayEditor editor;
     char      *newtext;
{
   Arg          args[5];
   Widget old = XawTextGetSource(editor->editor);

   XtSetArg(args[0],XtNstring, newtext);
   XtSetArg(args[1],XtNlength, editor->bufSize); 
   XtSetArg(args[2],XtNeditType,editor->mode); 
   XtSetArg(args[3],XtNtype, XawAsciiString);
   XtSetArg(args[4],XtNuseStringInPlace, True);


   XawTextSetSource(editor->editor,
		   XtCreateWidget("textSource", asciiSrcObjectClass, 
		   editor->editor, args, 5), (XawTextPosition)0);
		   
   XtDestroyWidget(old);
}

void StartEditor(editor)
       DayEditor editor;
{

   XtPopup(editor->shell ,XtGrabNone);

}

void CloseEditors()
{
   int i;
   DayEditor editor;

   if(editorList == NULL)
     return;

   for(i=1; i <= ListLength(editorList) ; i++){
      editor = GetEditorFromList(editorList,i);
      if(editor->saved == False)
	SaveText(NULL,(caddr_t)editor,NULL);
   }
}


int read_file(filename,buffer,size)
     char *filename;
     char *buffer;
     int  size;
{
   int fd, len;
   struct stat sb;

   if((fd = open(filename, O_RDONLY, 0666)) < 0) {
      if(errno == ENOENT)
	return 0;
      else{
	 perror("xcalendar:open");
	 return(-1);
      }
   }
   
   if(fstat(fd, &sb) != 0) {
      close(fd);
      perror("xcalendar:fstat:");
      return(-1);
   }

   if(sb.st_size == 0){
      unlink(filename);
      return 0;
   }

   len = (size < sb.st_size)? size : sb.st_size;

   if(read(fd, buffer, len) < len) {
      close(fd);
      fprintf(stderr, "Couldn't read all file %s\n",filename);
      perror("xcalendar:read:");
      return(-1);
   }

   close(fd);

   return sb.st_size;

}

int write_to_file(filename,buffer,len)
     char *filename;
     char *buffer;
     int  len;
{
   int fd;

   if((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT , 0666)) < 0) {
      perror("xcalendar:open");
      return(-1);
   }
   
   if(len == 0){
      unlink(filename);
      return 0;
   }

   if(write(fd, buffer, len)  < len) {
      fprintf(stderr, "Sorry couldn't write all file %s\n", filename);
      perror("xcalendar:write:");
      close(fd);
      return(-1);
   }

   close(fd);

   return len;
}

