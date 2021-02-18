/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.ch,v 2.6 1991/09/12 19:49:03 bobg Exp $ */
/* $ACIS:print.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidprint_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/support/RCS/print.ch,v 2.6 1991/09/12 19:49:03 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* The following defines are to be used as the second argument to print_ProcessView 
	or the first argument to GetPrintCmd */

#define print_PRINTPOSTSCRIPT 129 /*  send postscript directly to printer */
#define print_PREVIEWPOSTSCRIPT  128 /* preview postscript */
#define print_PRINTTROFF 1	/* send through troff and print  (default) */
#define print_PREVIEWTROFF 0	/* send through troff and preview */

package print {
classprocedures:
    ProcessView(struct view *v,int print,int dofork,char *DocumentName,char *prarg) returns int;
    InitializeClass() returns boolean;
    GetPrintCmd(int print) returns char *;
};
