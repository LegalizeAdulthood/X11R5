/**********************************************************

  The ANSWER GARDEN PROJECT

  FILESERV.H Some declarations

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

FileInfo *AG_File_Open_NewFile();
FileInfo *AG_File_Open();
void AG_File_Free_Buffer();
Boolean AG_File_Check_Write_Permission();
Boolean AG_File_Save();
void AG_File_Close();
void FileService_Register_Open_Node();
FileInfo *FileService_Register_Open_NonFS_Node();
Boolean FileService_Register_Closed_File();
Boolean FileService_Register_Closed_Edit();

Boolean FileService_Has_Node_Changed();

Boolean FileService_Set_Defined_Headers();

EditInfo *FileService_Get_EditNIPInfo();
EditInfo *FileService_Get_EditFileInfo();

char *FileService_Get_Text();
Widget FileService_Get_Shell();
char **FileService_Get_Defined_Headers();
AG_Nonpredefined_Header *FileService_Get_Nonpredefined_Headers();

