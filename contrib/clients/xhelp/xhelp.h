/*----------------------------------------------------------------------------------*
 :                           xhelp: A help facility for X                           :
 :                                   version 1.0                                    :    
 :----------------------------------------------------------------------------------:
 : Copyright 1991 by Mark Newsome (Auburn University)                               :
 :                                                                                  :
 : Permission to use, copy, modify, distribute, and sell this software and its      :
 : documentation for any purpose is hereby granted without fee, provided that       :
 : the above copyright notice appear in all copies and that both that               :
 : copyright notice and this permission notice appear in supporting                 :
 : documentation, and that the name of Auburn University not be used in advertising :
 : or publicity pertaining to distribution of the software without specific,        :
 : written prior permission.  Auburn University makes no representations about the  :
 : suitability of this software for any purpose.  It is provided "as is"            :
 : without express or implied warranty.                                             :
 :                                                                                  :
 : Author:  Mark Newsome                                                            :
 : E-mail:  mnewsome@eng.auburn.edu                                                 :
 : address: Computer Science and Engineering                                        :
 :          107 Dunstan Hall                                                        :
 :          Auburn University, AL, 36849                                            :
 *----------------------------------------------------------------------------------*/

/******************************************************************************
 **                                                                          **
 ** In order for your application to 'hook' into XHELP, you'll need to do    **
 ** the following:                                                           **
 **                                                                          **
 ** 1. #include "xhelp.h"   *** include this header file                     **
 **                                                                          **
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 ** 2. Use XHELP in developer mode to create a few sample help topics.       **
 **    Note that the first help topic should be utilized as an introduction  **
 **    to your application.                                                  **
 **                                                                          **
 **    To bring XHELP up in developer mode, run XHELP from the command line  **
 **    without any parameters.  Use the FILE menu to open an existing help   **
 **    file or "new" to create a new one.  Further discussion of the USER    ** 
 **    and DEVELOPER modes may be found using XHELP in the file "xhelp.hlp". **
 **                                                                          **
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 ** 3. Select the "IDs" selection of the EDIT menu to review the topicIDs    **
 **    that you've assigned to your topics.                                  **
 **                                                                          ** 
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 ** 4. To bring up XHELP from within your application, all you need to do    **
 **    is issue a call to "XhCallXHelp"                                      **
 **                                                                          **
 **    XhCallXhelp(w, helpfilename, topicId, topicName);                     **
 **                                                                          **
 **  - - - - - - - - - -  T H E  P A R A M E T E R S   - - - - - - - - - - - **
 ** Widget w;                                                                **
 **   *** Use your "toplevel" widget here.                                   **
 **                                                                          **
 ** String helpfilename;                                                     **
 **   *** This variable contains the name of your helpfile.  XHELP filenames **
 **       have an extension of ".hlp".  If you do not provide the extension, **
 **       XHELP will automatically tack it on.                               **
 **                                                                          **
 **       XHELP will search the PATH environment variable to try to find its **
 **       default xhelp.hlp if it cannot find your file.                     **
 **                                                                          **
 **                                                                          **
 ** String topicName;                                                        **
 **   *** This string should match the name of the help topic you specified  **
 **       in XHELP when creating the topic.  Not to worry though, if it      **
 **       doesn't match exactly.  It is displayed in an error message        **
 **       as a reminder if the topicID your application requests cannot be   **
 **       be honored.                                                        **
 **                                                                          **
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 ** 5. XHELP will display the (introductory) topic assigned to               **
 **    XHELP_INTRO_ID under the following conditions:                        **
 **                                                                          **
 **              a) the XHELP_INTRO_ID is requested                          **
 **              b) NULL is specified as the topicID                         **
 **              c) a requested topicID is malformed (!= 13 characters)      **
 **                                                                          **
 **    If a topicID is well-formed, but not found within the requested       **
 **    helpfile, then XHELP will issue an error message indicating the       **
 **    1) helpfilename  2) topicID and 3) topicName.  This message is        **
 **    to help you identify errors in coding the help requests.  The user    **
 **    is told to notify the "application developer".  Hopefully, they'll    **
 **    never see this message!                                               **
 **                                                                          **
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 ** 7. ABOUT THE TOPIC IDs                                                   **
 **                                                                          **
 **    A topic ID is a 13 character string used to identify the help topic.  **
 **    When you build a help file, it is better to use meaningful IDs to     **
 **    help you differentiate between help topics.                           **
 **                                                                          **
 **    In the case you don't want to make up IDs, XHELP will automatically   **
 **    fill this field with a unique code if you leave it empty.             **
 **    Here's the format used to manufacture topic IDs.	                     **
 **                                                                          ** 
 **        SystemDate%SystemTime,                                            **
 **                                                                          **
 **    where SystemDate takes the form MMDDYY and SystemTime is hhmmss.      **
 **                                                                          **
 ** ------------------------------------------------------------------------ **
 **                                                                          **
 **    Mark Newsome                                                          **
 **    January 1991, September 1991                                          **
 ******************************************************************************/

#include <stdio.h>
#include <X11/Xatom.h>
#define XHELP_NOT_ACTIVE 	1234	
#define XHELP_ACTIVE   		4321	
Atom  XHELP_FILE, XHELP_STATUS;
void XhCreateHelpAtoms();

/*---------------------------------------------------------------------------------------*
 :     Everything between this and the double bars below is all you'll need to use XHELP :
 *---------------------------------------------------------------------------------------*/

/*---------------------------------------------------------* 
 :  Code XHELP_INTRO_ID for the topicID when you want the  :
 :  introduction helptopic to be displayed.  XHELP         :
 :  automagically assigns this ID to the very first HELP   :
 :  topic when the help file is created.                   :
 *---------------------------------------------------------*/
#define XHELP_INTRO_ID		"000000%000000"   

/*----------------------------------------------*/
int XhCallXHelp(w,helpfilename, topicId, topicName)
/*----------------------------------------------*/
        Widget w;
        String helpfilename, topicId, topicName;
{
   int xhelpStatus;
   static count=0;
   char name[80], cmdstr[80];
   int result;

   if (!count) {
      XhCreateHelpAtoms(w);
      count++;
   }

   if ((topicId == NULL) ||  (strlen(topicId)!=13))  /** the topic id must be 13 in length **/
       topicId=XHELP_INTRO_ID;

   if (topicName == NULL)
       topicName = "";

   if ((xhelpStatus=XhGetHelpStatusProperty(w))==XHELP_ACTIVE) {  /** XHELP alive? **/
       sprintf(name, "%s:%s:%s",helpfilename,topicId,topicName);
       XChangeProperty(XtDisplay(w),
             DefaultRootWindow(XtDisplay(w)),
             XHELP_FILE, XA_STRING, 8, PropModeReplace, (unsigned char *)name,
             strlen(name));
   } else { /** XHELP is not around, let's wake it up **/
         sprintf(cmdstr,"xhelp %s %s %s & \n",
               helpfilename,topicId,topicName); /** invoke xhelp in USER mode **/
         result=system(cmdstr);
         if (result)
            fprintf(stderr, "Could not invoke XHelp, return code =%d\n", result);
         else { 
            /*---------------------------------------------------------* 
             : It's loading.  Let's not let the user bring up multiple :
             : instances of XHELP by repeatedly pressing a help button :
             *---------------------------------------------------------*/
            XtSetSensitive(w, FALSE);
            while (XhGetHelpStatusProperty(w)==XHELP_NOT_ACTIVE);
            XtSetSensitive(w, TRUE);
         }
       }
   return xhelpStatus;  /** ignore this return code for now **/  
} /** XhCallXhelp() **/

/*--------------------------------------------------------------------------------------
 *--------------------------------------------------------------------------------------*/ 

/*-----------------------------*/
void XhSetHelpStatusProperty(w,arg)
/*-----------------------------*/
	Widget w;
        int arg;
{
     XChangeProperty(XtDisplay(w),
         DefaultRootWindow(XtDisplay(w)),
         XHELP_STATUS, 
         XA_INTEGER,
         8,
         PropModeReplace,
         &arg,sizeof(int));
} /** XhSetHelpStatusProperty() **/

/*-------------------*/
void XhCreateHelpAtoms(w)
/*-------------------*/
	Widget w;
{ Display * dpy = XtDisplay(w);
     XHELP_FILE = XInternAtom(dpy, "XHelp File", 0);
     XHELP_STATUS = XInternAtom(dpy, "XHelp Status", 0);
} /** CreateHelpAtoms() **/

/*------------------------*/
int XhGetHelpStatusProperty(w)
/*------------------------*/
{
   Atom actual_type;
   int  actual_format, nitems, leftover;
   int  *retdata = NULL ;

   if (XGetWindowProperty(XtDisplay(w),
            DefaultRootWindow(XtDisplay(w)),
            XHELP_STATUS,
            0L,
            (long) BUFSIZ,
            FALSE,  /** server deletes data upon receipt **/
            XA_INTEGER,
            &actual_type, &actual_format,
            &nitems, &leftover, &retdata) == Success && actual_type == XA_INTEGER) {
        if (*retdata == XHELP_ACTIVE)
          return XHELP_ACTIVE;
        else
          return XHELP_NOT_ACTIVE;
    } else
        return XHELP_NOT_ACTIVE;
} /** XhGetHelpStatusProperty() **/


