/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *nonfatal_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/nonfatal.c,v 2.4 1991/09/12 15:46:24 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/nonfatal.c,v 2.4 1991/09/12 15:46:24 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/nonfatal.c,v $
*/
#include <stdio.h>
#include <errprntf.h>

/* This is in a separate file to make it easy for a no-snap client to override it if it so desires. */


NonfatalBizarreError(text)
char *text;
{
    BizarreError(text, ERR_WARNING);
}

