/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textref.c,v 1.2 1991/09/12 16:35:45 bobg Exp $ */
/* $ACIS:textref.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textref.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/text/RCS/textref.c,v 1.2 1991/09/12 16:35:45 bobg Exp $";
#endif /* lint */

#include <class.h>
#include <textref.eh>
char * textref__ViewName(self)
struct textref *self;
{
    return "textrefv";
}
static char *textref__GetRef(self,size,buf)
struct textref *self;
long size;
char *buf;
{
    char *c,*name;
    long realsize,i;
    i = 0;
    realsize = textref_GetLength(self);
    while(realsize > 0 && textref_GetChar(self,i) == ' ') {i++; realsize--;}
    if(size > realsize) size = realsize;
    textref_CopySubString(self,i,size,buf, FALSE);
    c = buf;
    if(*c == '#'){
	while(*c != ' ' && *c != '\t' && *c != '\0')c++;
	if (*c == '\0') c = buf;
	else c++;
	if (*c == '\0') c = buf;
    }
    for(name = c; *c != '\0'; c++) if(*c == ' ' || *c == '\t') *c = '-';
    c--;
    while(*c == '-') *c-- = '\0';
    return name;
}
