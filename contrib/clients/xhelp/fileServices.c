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
/*
 *  Some of this code came from xedit.c, by DEC Corp. 
 *
 *                        COPYRIGHT 1987
 *                 DIGITAL EQUIPMENT CORPORATION
 *                     MAYNARD, MASSACHUSETTS
 *                      ALL RIGHTS RESERVED.
 *
 * THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE AND
 * SHOULD NOT BE CONSTRUED AS A COMMITMENT BY DIGITAL EQUIPMENT CORPORATION.
 * DIGITAL MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE FOR
 * ANY PURPOSE.  IT IS SUPPLIED "AS IS" WITHOUT EXPRESS OR IMPLIED WARRANTY.
 *
 * IF THE SOFTWARE IS MODIFIED IN A MANNER CREATING DERIVATIVE COPYRIGHT RIGHTS,
 * APPROPRIATE LEGENDS MAY BE PLACED ON THE DERIVATIVE WORK IN ADDITION TO THAT
 * SET FORTH ABOVE.
 *
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Digital Equipment Corporation not be
 * used in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 */


#include "fileServices.h"
#define MAX_FILES 100
#define MAX_FILENAME_LENGTH	20
#define MAX_PATH_LENGTH  2000

/******* from util.c in xedit.c *******/
typedef enum {NO_READ, READ_OK, WRITE_OK} FileAccess;

char *path, *xhelpPath;
extern Widget toplevel;

/*----------------------*/
void fileServices_init() 
/*----------------------*/
{
   path=(String)XtCalloc(1,200); 
   getcwd(path,80);
/*
   fprintf(stderr, "XHelp: xhelpPath=[%s]\ncwd=[%s]\n",xhelpPath,path);
*/
   if ((xhelpPath = (char *)getenv("PATH"))==NULL)  /* last resort--we'll search PATH */
      xhelpPath = ".";  /* whoops, look in current dir only */
}

/*---------------*/
void cd(directory)
/*---------------*/
	String directory;
{
   String commandstr, s;
   commandstr = XtCalloc(1, 4+strlen(directory));
   sprintf(commandstr, "cd %s", directory);
   chdir(directory);

#ifdef debug
fprintf(stderr,"XHelp: cd: path=%s\nrequested dir:[%s]\n",path,directory);
#endif debug
  if (strcmp(directory,"./")==0) return; /* do nothing */ 
 
  /*** ../ means go back one level ***/
  if ((strcmp(directory,"../")==0) && (strlen(path)>0)) {
     if (s = (char *)strrchr(path, '/')) {
        *s = '\0'; 
     } /* if */ 
  } /* if ../ */
  else {
    strcat(path,"/");
    strncat(path,directory,strlen(directory)-1); /* get rid of newline */
  } /** else **/
} /** cd() **/
 
/*------------------------*/
char * Getdir(path,pattern)   
/*------------------------*/
	char * path, *pattern;
{
  static char * line[MAX_FILES];
  FILE *dirfile;
  int i, j, amt;
  String commandstr; 
  DIRECTORY_MGR *dm;
  DIR_ENTRY *de;
  int sort_mode=1;

#ifdef debug
fprintf(stderr, "XHelp: Getdir() path=[%s]  pattern=[%s]\n", path, pattern);
#endif debug

  i=0;
  dm = DirectoryMgrSimpleOpen(path,sort_mode,"*");
  if (dm == NULL) {
       fprintf(stderr,"XHelp: Can't open directory mgr for '%s'\n",path);
       return(NULL); 
  } /** if **/
  while (i<MAX_FILES) { /** let's get all the directory items first **/
     de = DirectoryMgrNextEntry(dm);  
     if (de==NULL) break;
     if (DirEntryIsDir(de)) {
        line[i]=(String)XtCalloc(1,MAX_FILENAME_LENGTH+1);
        sprintf(line[i], "%s/",DirEntryFileName(de));
        /** line[i]=XtNewString(DirEntryFileName(de)); **/
        i++;
     } /** if **/
  } /** while **/

  DirectoryMgrSimpleRefilter(dm,pattern);
  while (i<MAX_FILES) { /** get all the help files **/
     de = DirectoryMgrNextEntry(dm);
     if (de == NULL) break;
        if (line[i]==NULL)
            line[i]=(String)XtCalloc(1,MAX_FILENAME_LENGTH+1);
 
        strcpy(line[i], DirEntryFileName(de));
     i++;
  /*   DirEntryDump(stderr,de); */
  } /** while **/
  DirectoryMgrClose(dm);
  line[i]=NULL;
  return (char *) line;
} /*** Getdir() ***/

/*-----------------------------------------------------*/
FILE *SearchAndOpen(searchpathname, filename, wherefound)
/*-----------------------------------------------------*/
	char *searchpathname, *filename, *wherefound;
{
   char *i, *j, apath[MAX_PATH_LENGTH], try[MAX_PATH_LENGTH];
   FILE *result=NULL;
   String tempstr;

   if (strstr(filename, ".hlp")==0) {
       sprintf(try, "%s.hlp", filename);
       filename=XtNewString(try);
   };

   if ( (result=fopen(filename, "r"))!=NULL){ /** look in current dir **/
       strcpy(wherefound, path);
       return result;
     };

   /**********| ok, now let's search supplied path |**********/
   i=j=searchpathname;
   while ( (j = (char *)strchr(i, ':')) != NULL) {
      strncpy(apath, i, j-i); apath[j-i+1]='\0';
      sprintf(try, "%s/%s", apath,filename);
      if ((result=fopen(try, "r")) != NULL) {
          strcpy(wherefound, apath);
          return result; 
      } /** if **/
      i=j=j+1;
  } /** while **/
  sprintf(try, "%s/%s",i,filename);
  result = fopen(try, "r"); 
  if (result != NULL) 
      strcpy(wherefound, i);
  else
      wherefound[0]='\0';  /** we don't want wherefound to have garbage in it      **/
                           /** --we're going to prepend it to filename when saving **/
  return result; 
} /** SearchAndOpen() **/

/*------------------------------------------*/
FileAccess CheckFilePermissions(file, exists)
/*------------------------------------------*/
        char *file;
        Boolean *exists;
{
    char temp[BUFSIZ], *ptr;
    if (access(file, F_OK) == 0) {
        *exists = TRUE;
        if (access(file, R_OK) != 0)
            return(NO_READ);
        if (access(file, R_OK | W_OK) == 0)
            return(WRITE_OK);
        return(READ_OK);
    }
    *exists = FALSE;
    strcpy(temp, file);
    if ( (ptr = rindex(temp, '/')) == NULL)
        strcpy(temp, ".");
    else
        *ptr = '\0';
    if (access(temp, R_OK | W_OK | X_OK) == 0)
        return(WRITE_OK);
    return(NO_READ);
} /** CheckFilePermissions() **/


