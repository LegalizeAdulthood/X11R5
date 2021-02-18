/*

			  I n i t i a l i z e . c

    $Header: Initialize.c,v 1.0 91/10/04 17:00:58 rthomson Exp $

    Rich Thomson
    Design Systems Division
    Evans & Sutherland Computer Corporation

	Copyright (C) 1990, 1991, Evans & Sutherland Computer Corporation
*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			   Salt Lake City, Utah

			    All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its 
    documentation for any purpose and without fee is hereby granted, 
    provided that the above copyright notice appear in all copies and that
    both that copyright notice and this permission notice appear in 
    supporting documentation, and that the names of Evans & Sutherland not be
    used in advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.  

    EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#include <stdio.h>
#include "PEXt.h"

/*
			 File Local Variables
*/
int PEXtError;				/* for PEX error numbers */

/*
			      PEXtSanityCheck

  This routine checks the global variable used to store error codes
  returned from PEX.  If the error code is non-zero, it prints out a
  diagnostic message and dies.
*/
void PEXtSanityCheck(File, Line, Routine)
     char *File, *Routine;
     int Line;
{
  if (PEXtError)
    {
      fprintf(stderr, "(file %s; line %d):\n", File, Line);
      fprintf(stderr, "\t?unexpected PEX error %d in routine %s\n", 
	      PEXtError, Routine);
      dump_names(stderr, 0, PEXtError);
      exit(1);
    }
}

/*
				PEXtOpenPEX

  This routine attempts to open PEX on the given display, initializes
  the resource database using the given Name and ClassName and parsing
  any relevant command line arguments specified by argc_p and argv.
  The error file for PEX is given by error_file.  This parameter is a
  pointer to the file name to be opened, or NULL if stderr is to be used.
  
  The X11 display connection should already be opened and the toolkit
  command line arguments parsed.

  The routine returns a non-zero result when PEX was actually opened
*/
Boolean PEXtOpenPEX(display, error_file, Name, ClassName, argc_p, argv)
     Display *display;			/* display connection */
     char *error_file;			/* name of error file */
     char *Name, *ClassName;		/* program name and class */
     int *argc_p;			/* pointer to arg count */
     char *argv[];			/* argument vector */
{
  Pxphigs_info pex_info;		/* for opening PEX */
  Psys_st state;			/* for inquiring system state */

  pex_info.display = display;		/* display connection */
  pex_info.rmdb = NULL;			/* database */
  pex_info.appl_id.name = Name;		/* for resolving resource attr's */
  pex_info.appl_id.class_name = ClassName; /* class name */
  pex_info.args.argc_p = argc_p;
  pex_info.args.argv = argv;		/* for parsing arguments */
  pex_info.flags.no_monitor = 1;	/* don't run Phigs monitor */
  pex_info.flags.force_client_SS = 0;	/* server structure traversal */

  popen_xphigs(error_file, PDEF_MEM_SIZE, PXPHIGS_INFO_DISPLAY |
	       PXPHIGS_INFO_APPL_ID | PXPHIGS_INFO_ARGS |
	       PXPHIGS_INFO_FLAGS_NO_MON | PXPHIGS_INFO_FLAGS_CLIENT_SS,
	       &pex_info);		/* attempt to open PEX */
  pinq_sys_st(&state);			/* did we really open it? */

  return (state == PSYS_ST_PHOP);
}

/*
  PEXtInitialize

  This routine first adds PEX resource converters and then creates a
  toplevel shell widget by calling XtInitialize.  The widget ID of the
  toplevel shell created is then returned.
*/
Widget PEXtInitialize(AppContextHandle, AppClass,
		      Options, NumOptions,
		      ArgCountHandle, ArgVector,
		      FallbackResources,
		      Args, NumArgs)
     XtAppContext *AppContextHandle;
     String AppClass;
     XrmOptionDescList Options;
     Cardinal NumOptions;
     Cardinal *ArgCountHandle;
     String *ArgVector;
     String *FallbackResources;
     ArgList Args;
     Cardinal NumArgs;
{
  Widget topLevel;
  String AppName, slash;
					/* open the toolkit */
  topLevel = XtAppInitialize(AppContextHandle, AppClass,
			     Options, NumOptions,
			     ArgCountHandle, ArgVector,
			     FallbackResources,
			     Args, NumArgs);

					/* attempt to open PEX */
  AppName = ArgVector[0];
  while (slash = (String) index(AppName, '/'))
    AppName = slash+1;

  if (!PEXtOpenPEX(XtDisplay(topLevel), PDEF_ERR_FILE, AppName, AppClass,
		   ArgCountHandle, ArgVector))
    XtAppError(*AppContextHandle, "Couldn't open PEX on specified display.");

  PEXtRegisterConverters();		/* add PEX resource converters */

  return topLevel;			/* return topLevel widget */
}
