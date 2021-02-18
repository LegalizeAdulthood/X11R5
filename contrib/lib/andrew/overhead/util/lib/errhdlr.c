/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *errhdlr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/util/lib/RCS/errhdlr.c,v 1.4 1991/09/12 17:25:05 bobg Exp $";

/* $Header: */
/* $ACIS: */
/* $Source: */

#include <util.h>
#include <errno.h>
#include <andrewos.h>		/* <strings.h> */

char EH_Error_Msg[EH_ERR_MSG_BUF_SIZE];
EH_environment *_error_handler_env;

char *emalloc(size)
long size;
{
  /* Returns a pointer to a hunk of memory of size. 
     Errors are signalled. */
  char *tmp_ptr;

  EH_cond_error_on(!(tmp_ptr = (char *)malloc(size)),
		   EH_ret_code(EH_module_system,ENOMEM),
		   "Malloc failed");
  return(tmp_ptr);
}

char *erealloc(old, newsize)
char *old;
long newsize;
{
  /* Returns a pointer to a hunk of memory of newsize, with
     the contents of old (trunc) copied into it. 
     Errors are signalled.  */
  char *tmp_ptr;

  EH_cond_error_on(!(tmp_ptr = (char *)realloc(old, newsize)),
		   EH_ret_code(EH_module_system,ENOMEM),
		   "Realloc failed");
  return(tmp_ptr);
}


char *CopyString(old)
char *old;
{
  /* Returns a newly emalloc'd copy of the string.
     Errors from emalloc are passed up. */

  return(strcpy(emalloc(strlen(old)+1), old));
}

