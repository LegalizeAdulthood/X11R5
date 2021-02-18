/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *addhdr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/addhdr.c,v 2.9 1991/09/12 15:41:11 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/addhdr.c,v 2.9 1991/09/12 15:41:11 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/addhdr.c,v $
*/
#include <andrewos.h>
#include <ms.h>
#include <hdrparse.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

#define ALTER_ADD 0
#define ALTER_DEL 1

AddHeader(Msg, Head)
struct MS_Message *Msg;
char *Head;
{
    return(AlterMessage(Msg, ALTER_ADD, 0, Head));
}

DeleteHeader(Msg, num)
struct MS_Message *Msg;
int num;
{
    return(AlterMessage(Msg, ALTER_DEL, num, NULL));
}

DelHeaderByName(Msg, Head)
struct MS_Message *Msg;
char *Head;
{
    int i, len;

    len = strlen(Head);
    for (i=0; Msg->ParsedStuff->HeadName[i]; ++i) {
	if (Msg->ParsedStuff->HeadName[i][len] == '\0' && !lc2strncmp(Msg->ParsedStuff->HeadName[i], Head, len)) {
	    break;
	}
    }
    if (!Msg->ParsedStuff->HeadName[i]) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CHOOSEDIRECTORIES);
    }
    return(DeleteHeader(Msg, i));
}

AlterMessage(Msg, code, delhead, Head)
struct MS_Message *Msg;
int code, delhead;
char *Head;
{
    register char   *s;
    char *start;
    int     offset, len, dellen;

    if (code == ALTER_ADD) {
	debug(1,("Addheader\n"));
	offset = strlen(Head) + 1;
	s = malloc(offset + Msg->HeadSize + 1);
	if (!s) {
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
	}
	strcpy(s, Head); 
	strcat(s, "\n");
	strcat(s, Msg->RawBits);
	Msg->FullSize += offset;
	Msg->HeadSize += offset;
    } else {
	debug(1, ("Delete header\n"));
	if ((delhead < 0) || (delhead >= Msg->ParsedStuff->HeadsAllocated)) {
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_CHOOSEDIRECTORIES);
	}
	len = Msg->ParsedStuff->HeadBodyLen[delhead];
	start = Msg->ParsedStuff->HeadBody[delhead];
	if ((len < 0) || !start) {
	    return(0); /* Already not there */
	}
	if (start) {
	    --start; /* to work right when the header is empty */
	    while ((start > Msg->RawBits) && *start && *start != '\n') --start;
	}
	dellen = (Msg->ParsedStuff->HeadBody[delhead] - start) + len;
	if (start <= Msg->RawBits) ++dellen; /* to avoid a leading newline */
	s = malloc(1 + Msg->HeadSize - dellen);
	if (!s) AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
	if (start > Msg->RawBits) {
	    offset = start - Msg->RawBits;
	    bcopy(Msg->RawBits, s, offset);
	} else {
	    offset = 0;
	}
	bcopy(start + dellen, s+offset, Msg->HeadSize - (dellen + offset));
	Msg->FullSize -= dellen;
	Msg->HeadSize -= dellen;
    }
    /* Parsed stuff has pointers into raw message which must be fixed now */

    free(Msg->RawBits);
    Msg->RawBits = s;
    mserrcode = OnlyParseMessageFromRawBody(Msg);
    Msg->WeFiddled = TRUE;
    return(mserrcode);
}

