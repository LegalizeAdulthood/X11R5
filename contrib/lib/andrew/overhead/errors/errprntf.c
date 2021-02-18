/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/errors/RCS/errprntf.c,v 2.5 1991/09/12 17:12:42 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/errors/RCS/errprntf.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/overhead/errors/RCS/errprntf.c,v 2.5 1991/09/12 17:12:42 bobg Exp $";
#endif /* lint */

/* *************************************************************** 

 errprntf.c:  Routine for printing Andrew-standard errors.

		********************************************
		**** For documentation, see errprntf.h **** 
		********************************************

*/

#include <stdio.h>
#include "errprntf.h"

#define CONTROLMAX 1000  /* Longest printf control string */

errprintf(application, type, log, id, format, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20)

int type;
char *application, *log, *id, *format, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, 
    *s9, *s10, *s11, *s12, *s13, *s14, *s15, *s16, *s17, *s18, *s19, *s20;
{
    char ControlString[CONTROLMAX], *typestr;
    int numfields;

    if (type < 0 || type > 9) type = 0;
    numfields = 1;
    if (application) numfields = 2;
    if (log) numfields = 3;
    if (id) numfields = 4;
    sprintf(ControlString, "<%%s%%s%%s%%s%%s%%s%%s>%s\n", format);
    if (type == ERR_CRITICAL) {
	typestr = "critical";
    } else if (type <= ERR_WARNING) {
	typestr = "warning";
    } else if (type <= ERR_MONITOR) {
	typestr = "monitor";
    } else {
	typestr = "debug";
    }
    return(safefprintf(stderr, ControlString, 
	typestr, 
	(numfields > 1) ? ":" : "",
	application ? application : "",
	(numfields > 2) ? ":" : "",
	log ? log : "",
	(numfields > 3) ? ":" : "",
	id ? id : "",
	 s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, 
	s12, s13, s14, s15, s16, s17, s18, s19, s20));
}

safefprintf(fp, control, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20)
FILE *fp;
char *control, *s1, *s2, *s3, *s4, *s5, *s6, *s7, *s8, *s9, *s10, *s11, *s12, *s13, *s14, *s15, *s16, *s17, *s18, *s19, *s20;
{
    fprintf(fp, control, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20);
    fflush(fp);
    if (ferror(fp)) {
	fp = freopen("/dev/console", "w", fp);
	if (fp == NULL) return(-1);
	fprintf(fp, control, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20);
	fflush(fp);
	if (ferror(fp)) return(-1);
    }
    return(0);
}
