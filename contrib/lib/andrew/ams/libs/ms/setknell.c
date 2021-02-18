/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *setknell_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setknell.c,v 2.7 1991/09/12 15:48:23 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setknell.c,v 2.7 1991/09/12 15:48:23 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/setknell.c,v $
*/
#include <mailconf.h>

int DeathKnell;

MS_SetDeathKnell(dk)
int dk;
{
    if(dk>AMS_MaximumDeathKnell)
     dk=AMS_MaximumDeathKnell;
    DeathKnell = dk;
    return(0);
}
