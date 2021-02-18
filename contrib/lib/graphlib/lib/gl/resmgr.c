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


#include <stdio.h>
#include "X11/gl/window.h"
#include "X11/gl/resmgr.h"
#include "X11/gl/stddef.h"
#include <ctype.h>
#include <math.h>

static char *boolrestrue[] = { "true", "on", "yes", NULL };
static char *boolresfalse[] = { "false", "off", "no", NULL };
static int initialized=0,listresources=0;

int GetCmdLineResource(cmdlinename,plusok)
char *cmdlinename;
int plusok;
{
  int loop;

  if (strcmp(cmdlinename,NO_CMDLINE_ARG)==0)
    return -1;
  for (loop=1;loop<s_argc;loop++) 
    if((*s_argv[loop]=='-' || (*s_argv[loop]=='+' && plusok)) /* -arg or +arg*/
       &&strcmp(cmdlinename,s_argv[loop]+1)==0) {
      GL_MarkArgument(loop,GL_ARG_USED);
      return loop;
    }
      
  return -1;
}

static char *GetResource(resourcename)
char *resourcename;
{
  char *value;

  if (strcmp(resourcename,NO_RESOURCE)==0)
    return NULL;
  if (listresources) 
    fprintf(stderr,"Looked for resource :%s\n",resourcename);
  if (value = XGetDefault(display, s_argv[0], resourcename)) 
    return value;
  if (value = XGetDefault(display, s_classname, resourcename))
    return value;
  return NULL;
}

static void do_boolean(resourcename,cmdlinename,cmdlinepos,resource,destptr)
int cmdlinepos;
char *resource,*resourcename,*cmdlinename;
Boolean *destptr;
{
  char *checkfalse,*lowercaseres;
  int loop;

  if (cmdlinepos!=-1) {
    *destptr = *s_argv[cmdlinepos]=='-';
    return;
  }
  if (strcmp(cmdlinename,NO_CMDLINE_ARG)!=0) {
    checkfalse = (char *)xmalloc(strlen(cmdlinename)+3);
    strcpy(checkfalse+2,cmdlinename);
    checkfalse[0]='n';
    checkfalse[1]='o';
    cmdlinepos = GetCmdLineResource(checkfalse);
    free(checkfalse);
    if (cmdlinepos!=-1) {
      *destptr = False;
      return;
    }
  }
  if (resource==NULL)
    return;
  lowercaseres = (char *)xmalloc(strlen(resource)+1);
  strcpy(lowercaseres,resource);
  for(checkfalse=lowercaseres;*checkfalse!='\0';checkfalse++)
    if (islower(*checkfalse))
      tolower(*checkfalse);
  
  for (loop=0;boolrestrue[loop]!=NULL;loop++)
    if(strcmp(boolrestrue[loop],lowercaseres)==0) {
      free(lowercaseres);
      *destptr = True;
      return;
    }
  for (loop=0;boolresfalse[loop]!=NULL;loop++)
    if (strcmp(boolresfalse[loop],lowercaseres)==0) {
      free(lowercaseres);
      *destptr = False;
      return;
    }
  free(lowercaseres);
  fprintf(stderr,"Warning:Not sure of boolean value of resource named %s\n",resourcename);
  fprintf(stderr,"Warning:The found resource was %s\n",resource);
}

static void do_long(resourcename,cmdlinename,cmdlinepos,resource,destptr)
int cmdlinepos;
char *resource,*resourcename,*cmdlinename;
long *destptr;
{
  char *loop;

  if (cmdlinepos!=-1&&resource==NULL) {
    fprintf(stderr,"Warning:Need integer argument after option %s. Option Ignored\n",cmdlinename);
    return;
  }
  for(loop=resource;*loop!='\0';loop++)
    if ((*loop<'0'||*loop>'9')&&(*loop!='-')) {
      fprintf(stderr,"Warning:integer argument %s invalid. Option Ignored\n",resource);
      return;
    }
  *destptr = atol(resource);
}
  
static void do_string(resourcename,cmdlinename,cmdlinepos,resource,destptr)
int cmdlinepos;
char *resource,*resourcename,*cmdlinename;
char **destptr;
{
  if (cmdlinepos!=-1&&resource==NULL) {
    fprintf(stderr,"Warning:Need string argument after option %s. Option Ignored\n",cmdlinename);
    return;
  }
  *destptr = resource;
}

static void do_function(resourcename,cmdlinename,cmdlinepos,resource,
			destptr,handlefunction)
int cmdlinepos;
char *resource,*resourcename,*cmdlinename;
GLPointer destptr;
VPTF handlefunction;
{
  if (cmdlinepos!=-1&&resource==NULL) {
    fprintf(stderr,"Warning:Need argument after option %s. Option Ignored\n",
	    cmdlinename);
    return;
  }
  handlefunction(resourcename,cmdlinename,resource,destptr);
}

void _GL_VA_init_resmgr(list)
GL_DataList list;
{
  if (initialized)
    return;
  initialized = 1;
  if (GetCmdLineResource("help-resources")!= -1) {
    listresources=1;
    fprintf(stderr,"Using Classname     :%s\n",s_classname);
    fprintf(stderr,"Using Program name  :%s\n",s_argv[0]);
  }
}

void _GL_VA_resmgr_nullproc()
{
}

void _GL_VA_handle_resmgr(list,modifier,pvar)
GL_DataList list;
long modifier;
va_list *pvar;
{
  char *resourcename;
  char *cmdlinename;
  GLPointer destptr;
  char *resource;
  int cmdlinepos;
  VPTF HandleFunction;

  if ((modifier<1) || (modifier>GL_RES_MAXVAL)) {
    fprintf(stderr,"Invalid Modifier #%d passed to resource manager\n",modifier);
    fprintf(stderr,"No cute message.  Bye for now.\n");
    exit(-1);
  }
  resourcename = va_arg(*pvar,char *);
  cmdlinename = va_arg(*pvar,char *);
  destptr = va_arg(*pvar,GLPointer);
  if ((cmdlinepos = GetCmdLineResource(cmdlinename))!=-1) {
    if (modifier!=GL_RES_BOOLEAN&&(cmdlinepos+1)<s_argc) {
      resource = s_argv[cmdlinepos+1];
      GL_MarkArgument(cmdlinepos+1,GL_ARG_USED);
    }
    else
      resource = NULL;
  } else
    resource = GetResource(resourcename,cmdlinename);
  if (modifier==GL_RES_FUNCTION)
    HandleFunction = va_arg(*pvar, VPTF);
  if (resource!=NULL||cmdlinepos!= -1)
    switch (modifier) {
    case GL_RES_BOOLEAN  : 
      do_boolean(resourcename,cmdlinename,cmdlinepos,resource,destptr);
      break;
    case GL_RES_LONG     : 
      do_long(resourcename,cmdlinename,cmdlinepos,resource,destptr);
      break;
    case GL_RES_STRING   : 
      do_string(resourcename,cmdlinename,cmdlinepos,resource,destptr);
      break;
    case GL_RES_FUNCTION :
      do_function(resourcename,cmdlinename,cmdlinepos,resource,
		  destptr,HandleFunction);
      break;
    }
}



