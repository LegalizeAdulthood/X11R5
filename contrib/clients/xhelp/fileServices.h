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
#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/stat.h>
#include "DirectoryMgr.h"
#include "Directory.h"

void cd(/* char *directory */);
char *Getdir(/* char *path, *pattern */);
FILE *SearchAndOpen(/* char *searchpathname, *filename, *wherefound *.);
FileAccess CheckFilePermissions(/* char *file, Boolean *exists */ );

