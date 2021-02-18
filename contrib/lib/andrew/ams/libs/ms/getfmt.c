/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfmt.c,v 2.6 1991/09/12 15:43:57 bobg Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfmt.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/getfmt.c,v 2.6 1991/09/12 15:43:57 bobg Exp $ ";
#endif /* lint */

#include <ms.h>
#include <hdrparse.h>

GetFormatFromMessage(Msg, ThisFormat, bufsize, IsBE2)
struct MS_Message *Msg;
char *ThisFormat;
int bufsize, *IsBE2;
{
    char HeadBuf[2000], fmttype[25], fmtvers[25], fmtresources[200];
    int len;

    ThisFormat[0] = '\0';
    len = Msg->ParsedStuff->HeadBodyLen[HP_CONTENTTYPE];
    if (len > 0) {
	if (len > (sizeof(HeadBuf) - 1)) len = sizeof(HeadBuf) - 1;
	strncpy(HeadBuf, Msg->ParsedStuff->HeadBody[HP_CONTENTTYPE], len);
	HeadBuf[len] = '\0';
	BreakDownContentTypeField(HeadBuf, fmttype, sizeof(fmttype), fmtvers, sizeof(fmtvers), fmtresources, sizeof(fmtresources));
	if (!lc2strncmp("x-be2", fmttype, strlen(fmttype))) {
	    strncpy(ThisFormat, fmtvers, bufsize);
	}
    } else {
	len = Msg->ParsedStuff->HeadBodyLen[HP_SCRIBEFORMAT];
	if (len > 0) {
	    if (len > (sizeof(HeadBuf) - 1)) len = sizeof(HeadBuf) - 1;
	    strncpy(HeadBuf, Msg->ParsedStuff->HeadBody[HP_SCRIBEFORMAT], len);
	    HeadBuf[len] = '\0';
	    strncpy(ThisFormat, StripWhiteEnds(HeadBuf), bufsize);
	}
    }
    *IsBE2 = (ThisFormat[0]) ? 1 : 0;
    return(0);
}

