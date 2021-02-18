/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *getvconf_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getvconf.c,v 1.7 1991/09/12 15:44:26 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getvconf.c,v 1.7 1991/09/12 15:44:26 bobg Exp $
*/

#include <andyenv.h>
#include <andrewos.h>
#include <ms.h>
#include <stdio.h>

#define StrEql(a,b)(strcmp(a,b)==0)
/*
  dummy routine to get the rest of ams working again
*/
long MS_GetVConfig(key,vers,result)
char *key;
char *vers;
char *result;
{if(key==0)key="(null)";
 if(vers==0)vers="(null)";
 if (!access("/debug.getvconfig", F_OK)) /*print debugging info?*/
  fprintf(stderr,"MS_GetVConfig:key=%s,vers=%s",key,vers); 
 fflush(stderr);
 if(key==0)
  strcpy(result,"");
 else if(StrEql(key,"motd"))
  strcpy(result,"/etc/motd");
 else if(StrEql(key,"expire"))
  strcpy(result,"0,");
 else
  strcpy(result,"");
 return 0;
}
