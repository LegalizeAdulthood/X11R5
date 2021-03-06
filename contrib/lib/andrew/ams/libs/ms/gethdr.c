/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *gethdr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gethdr.c,v 2.7 1991/09/12 15:44:00 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gethdr.c,v 2.7 1991/09/12 15:44:00 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/gethdr.c,v $
*/
#include <andrewos.h>
#include <ms.h>
#include <hdrparse.h>

MS_GetHeaderContents(dirname, id, HeaderName, HeaderTypeNumber, HeaderBuf, lim) 
char *dirname, *id, *HeaderName; /* Passed in */
char *HeaderBuf; /* Passed out */
int HeaderTypeNumber, lim; /* Passed in  (I play with the former but do not pass it back) */
{
    struct MS_Directory *Dir;
    struct MS_Message *Msg;
    int len, i;
    char RawFileName[MAXPATHLEN+1];

    debug(1, ("MS_GetHeaderContents %s %s %s\n", dirname, id, HeaderName ? HeaderName : "NULL"));
    Msg = (struct MS_Message *) malloc (sizeof (struct MS_Message));
    if (!Msg) {
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_GETHEADERCONTENTS);
    }
    bzero(Msg, sizeof(struct MS_Message));
    Msg->OpenFD = -1;
    if (ReadOrFindMSDir(dirname, &Dir, MD_OK) != 0) {
	return(mserrcode);
    }
    GetBodyFileName(Dir->UNIXDir, id, RawFileName);
    if (ReadRawFile(RawFileName, Msg, FALSE)
	|| ParseMessageFromRawBody(Msg)) 
    {
	FreeMessage(Msg, TRUE);
	return(mserrcode);
    }
    *HeaderBuf = '\0'; /* Make sure to return an empty string if nothing found */
/* Now find header and copy it into HeaderBuf with limit lim */
    if (HeaderTypeNumber >= 0) {
	if (HeaderTypeNumber >= HP_END_PREDEFINED) {
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETHEADERCONTENTS);
	}
    } else {
	if (!HeaderName || !*HeaderName) {
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETHEADERCONTENTS);
	}
	for (i=0; Msg->ParsedStuff->HeadName[i]; ++i) {
	    if (!strcmp(HeaderName, Msg->ParsedStuff->HeadName[i])) {
		HeaderTypeNumber = i;
		break;
	    }
	}
	if (HeaderTypeNumber < 0) {
	    FreeMessage(Msg, TRUE);
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETHEADERCONTENTS);
	}
    }
    if (HeaderTypeNumber == HP_WIDEREPLY || HeaderTypeNumber == HP_ALLREPLY) {
	if (BuildReplyField(Msg) || 
	BuildWideReply(Msg, HeaderTypeNumber == HP_WIDEREPLY ? FALSE : TRUE)) {
	    FreeMessage(Msg, TRUE);
	    return(mserrcode);
	}
	strncpy(HeaderBuf, Msg->WideReply, lim);
	FreeMessage(Msg, TRUE);
	return(0);
    }
    if (HeaderTypeNumber == HP_REPLY_TO) {
	if (BuildReplyField(Msg)) {
	    return(mserrcode);
	}
	strncpy(HeaderBuf, Msg->ReplyTo, lim);
	FreeMessage(Msg, TRUE);
	return(0);
    }
    len = Msg->ParsedStuff->HeadBodyLen[HeaderTypeNumber];
    if (len <= 0) {
	HeaderBuf[0] = '\0';
	FreeMessage(Msg, TRUE);
	return(0);
    }
    if (len > lim) len = lim;
    strncpy(HeaderBuf, Msg->ParsedStuff->HeadBody[HeaderTypeNumber], len);
    HeaderBuf[len] = '\0';
    FreeMessage(Msg, TRUE);
    return(0);
}
