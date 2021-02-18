/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/addlib.c,v 2.5 1991/09/12 15:51:56 bobg Exp $ */
/* $ACIS: $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/addlib.c,v 2.5 1991/09/12 15:51:56 bobg Exp $ ";
#endif /* lint */

/* Program to run on the PC that mimics the Unix ar command's syntax and 
generates the proper calls to the Microsoft library manager.  */
#ifdef IBMPC
#include <process.h>
#endif /* IBMPC */
#include <errno.h>
#include <signal.h>
#include <stdio.h>


main(argc, argv)
char **argv;
int argc;
{
char lib[256];
char *newlib, *nl2;
int rc = 0;
    argv++;
    strcpy(lib, "lib ");
/*  printf("on library %s\n", *argv); */
    strcat(lib, *argv++);
    while (*argv != NULL) {
       strcat(lib, " +");
       strcat(lib, *argv++);
    }
    strcat(lib, ";");
       rc = system(lib);
/*     printf("Exec: %s\n", lib); */
       if (rc) {
          switch (rc) {
          case ENOENT: printf("COMMAND.COM not found.\n"); break;
          case ENOMEM: printf("Insufficient memory.\n"); break;
          case E2BIG:  printf("Environment space > 32K\n"); break;
          case ENOEXEC: printf("COMMAND.COM has invalid format.\n"); break;
          }
       }
}
