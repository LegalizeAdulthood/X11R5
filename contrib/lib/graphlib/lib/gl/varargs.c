/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Eric Anderson 
 * Buttons and menus mostly by Jeffrey Hutzelman
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */


#include "X11/gl/varargs.h"
#include "X11/gl/patchlevel.h"
#include <stdio.h>
#include <varargs.h>

char PatchLevel_LibGL[] = PATCHLEVEL_LIBGL;

void CheckAlloc(what)
GLPointer what;
{
  if (what!=NULL)
    return;
  fprintf(stderr,"Outa memory, Outa Memory you need to buy more memory.\n");
  fprintf(stderr,"What a pathetic depressing cheap system you are running\n");
  fprintf(stderr,"on.  Geez some people, go buy a real computer.\n");
  exit(-1);
}

GLPointer xmalloc(size)
int size;
{
  GLPointer ret;

  if (ret=(GLPointer)malloc(size))
    return ret;
  fprintf(stderr,"Error in the Null Zone.\n");
  fprintf(stderr,"Nullidity, nothingness, the blackness of space.\n");
  fprintf(stderr,"A pointer was null, life isn't worth anything.\n");
  fprintf(stderr,"All of your memory is gone, and so is mine, I can \n");
  fprintf(stderr,"remember but so little of what used to be.\n");
  fprintf(stderr,"Dying, death comes slowly, and my memory leaks away.\n");
  exit(-1);
/* NOTREACHED */
}
  
static void GL_Err_DataListKeyCollision()
{
  fprintf(stderr,"Major Mondo Warning, a key has been used for two things.\n");
  fprintf(stderr,"Among other uncool problems, any function asking for this key,\n");
  fprintf(stderr,"Will always recieve the last one registered.\n");
  fprintf(stderr,"You probably have a collision of keys with two extensions.\n");
  fprintf(stderr,"It would be good to investigate the code.\n");
  fprintf(stderr,"As this is probably real bad, death is coming.\n");
  fprintf(stderr,"But just think, How would you feel about death if she was your\n");
  fprintf(stderr,"older sister? - Sandman\n");
  exit(-1);
}

GL_DataList GL_Init_DataList()
{
  GL_DataList z;

  z = (GL_DataList)xmalloc(sizeof (struct _GL_DataList));
  z->data=NULL;
  z->next=NULL;
  z->key=0;
  return z;
}

GLPointer GL_Search_DataList(List,Key)
GL_DataList List;
VPTF Key;
{
  for(;List!=NULL&&List->key!=Key;List=List->next);
  if (List==NULL) {
    fprintf(stderr,"Death and drek.\n");
    fprintf(stderr,"We search and we do not find.\n");
    fprintf(stderr,"So we abandon our hopes and commit ourselves\n");
    fprintf(stderr,"To the blackness of death.\n");
    exit(-1);
  }
  return List->data;
}

int GL_Key_In_DataList(List,Key)
GL_DataList List;
VPTF Key;
{
  for(;List!=NULL&&List->key!=Key;List=List->next);
  return List!=NULL;
}

void GL_Add_To_DataList(List,Key,Data)
GL_DataList List;
VPTF Key;
GLPointer Data;
{
  GL_DataList entry;

  for (entry= List;entry!=NULL;entry=entry->next)
    if (entry->key==Key) 
      GL_Err_DataListKeyCollision();

  entry = (GL_DataList)xmalloc(sizeof(struct _GL_DataList));
  entry->data=Data;
  entry->key=Key;
  entry->next=List->next;
  List->next=entry;
}

void GL_Delete_DataList(List)
GL_DataList List;
{
  GL_DataList loop,delete;
  
  for(loop=List;loop!=NULL;) {
    delete=loop;
    loop=loop->next;
    free(delete);
  }
}
/* Set returner to NULL to do modify stuff, then the datalist
 parameter has to be valid, otherwise, it doesn't*/
GLPointer GL_Handle_Varargs(Initializer,Handler,Executer,Destroyer,Returner,
			  pvar,datalist)
VPTF Initializer,Handler,Executer,Destroyer;
PointerPTF Returner;
GL_DataList datalist;
va_list *pvar;
{
  GL_DataList workingdata=datalist;
  long modtype;
  GLPointer returnval;

  checkinit();
  if (Returner!=NULL) {
    workingdata = GL_Init_DataList();
    Initializer(workingdata);
  }
  for(modtype=va_arg(*pvar,int);modtype!=GL_Done;modtype=va_arg(*pvar,int)) 
    Handler(workingdata,modtype,pvar);
  if (Returner!=NULL) {
    Executer(workingdata);
    returnval = Returner(workingdata);
    Destroyer(workingdata);
  }
  if (Returner!=NULL) {
    GL_Delete_DataList(workingdata);
    return returnval;
  } else
    return NULL;
}

GLPointer GL_Create(Initializer,Handler,Executer,Destroyer,Returner,va_alist)
VPTF Initializer,Handler,Executer,Destroyer;
PointerPTF Returner;
va_dcl
{
  va_list pvar;
  va_start(pvar);
  return GL_Handle_Varargs(Initializer,Handler,Executer,Destroyer,Returner,
			   &pvar);
}

void GL_Modify(Initializer,Handler,Executer,Destroyer,Returner,datalist,
	       va_alist)
VPTF Initializer,Handler,Executer,Destroyer;
PointerPTF Returner;
GL_DataList datalist;
va_dcl
{
  va_list pvar;
  
  va_start(pvar);
  (void)GL_Handle_Varargs(Initializer,Handler,Executer,Destroyer,NULL,
			  &pvar,datalist);
}
