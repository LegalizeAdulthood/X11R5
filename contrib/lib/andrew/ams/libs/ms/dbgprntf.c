/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *dbgprntf_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgprntf.c,v 2.4 1991/09/12 15:42:41 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgprntf.c,v 2.4 1991/09/12 15:42:41 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/dbgprntf.c,v $
*/
#include <stdio.h>

/*VARARGS*/
dbgprintf(format, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20)
char   *format,
       *s1,
       *s2,
       *s3,
       *s4,
       *s5,
       *s6,
       *s7,
       *s8,
       *s9,
       *s10,
       *s11,
       *s12,
       *s13,
       *s14,
       *s15,
       *s16,
       *s17,
       *s18,
       *s19,
       *s20;
{
    safefprintf(stdout, format, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15, s16, s17, s18, s19, s20);
}

