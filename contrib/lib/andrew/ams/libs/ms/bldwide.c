/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *bldwide_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldwide.c,v 2.10 1991/09/12 15:41:53 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldwide.c,v 2.10 1991/09/12 15:41:53 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldwide.c,v $
*/
#include <ms.h>
#include <hdrparse.h>

#ifndef _IBMR2
extern char *malloc(), *realloc();
#endif /* _IBMR2 */

BuildWideReply(Msg, IncludeFrom)
struct MS_Message *Msg;
Boolean IncludeFrom;
{
    struct ParsedMsg *PStuff;
    char *NewWide;
    int WLen;

    debug(1, ("BuildWideReply\n"));
    PStuff = Msg->ParsedStuff;
    Msg->WideReply = malloc(WLen = 2000);
    if (Msg->WideReply == NULL) {
	AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
    }
    if (IncludeFrom && PStuff->HeadBody[HP_ALLREPLY]) {
	if ((Msg->ParsedStuff->HeadBodyLen[HP_ALLREPLY]+5) > WLen) {
	    WLen = Msg->ParsedStuff->HeadBodyLen[HP_ALLREPLY]+5;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* Use strncat when we could use strncpy, to guarantee null-termination */
	*Msg->WideReply = 0;
	strncat(Msg->WideReply, PStuff->HeadBody[HP_ALLREPLY], PStuff->HeadBodyLen[HP_ALLREPLY]);
	return(0);
    }
    if (PStuff->HeadBody[HP_WIDEREPLY]) {
	if ((Msg->ParsedStuff->HeadBodyLen[HP_WIDEREPLY]+5) > WLen) {
	    WLen = Msg->ParsedStuff->HeadBodyLen[HP_WIDEREPLY]+5+80;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* Use strncat when we could use strncpy, to guarantee null-termination */
	*Msg->WideReply = 0;
	strncat(Msg->WideReply, PStuff->HeadBody[HP_WIDEREPLY], PStuff->HeadBodyLen[HP_WIDEREPLY]);
	debug(16, ("Wide Reply field is <%s>\n", Msg->WideReply));
	if (IncludeFrom && Msg->ReplyTo && *Msg->ReplyTo) {
	    if ((strlen(Msg->ReplyTo)+strlen(Msg->WideReply)+5) > WLen) {
		WLen = strlen(Msg->ReplyTo)+strlen(Msg->WideReply)+5;
		Msg->WideReply = realloc(Msg->WideReply, WLen);
		if (Msg->WideReply == NULL) {
		    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
		}
	    }
	    strcat(Msg->WideReply, ", ");
	    strcat(Msg->WideReply, Msg->ReplyTo);
	}
	return(0);
    }
    *Msg->WideReply = 0;
    if (PStuff->HeadBody[HP_TO]) {
	if ((Msg->ParsedStuff->HeadBodyLen[HP_TO]+strlen(Msg->WideReply)+5) > WLen) {
	    WLen = Msg->ParsedStuff->HeadBodyLen[HP_TO]+strlen(Msg->WideReply)+5+80;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* Note that only the first one omits the comma addition */
	/* Use strncat when we could use strncpy, to guarantee null-termination */
	*Msg->WideReply = 0;
	strncat(Msg->WideReply, PStuff->HeadBody[HP_TO], PStuff->HeadBodyLen[HP_TO]);
    }
#ifdef NOTDEF
    if (PStuff->HeadBody[HP_CC]) {
	if ((PStuff->HeadBodyLen[HP_CC]+strlen(Msg->WideReply)+5) > WLen) {
	    WLen = PStuff->HeadBodyLen[HP_CC]+strlen(Msg->WideReply)+5+80;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* The aforementioned comma addition */
	if (*Msg->WideReply) strcat(Msg->WideReply, ", ");
	strncat(Msg->WideReply, PStuff->HeadBody[HP_CC], PStuff->HeadBodyLen[HP_CC]);
    }
#endif /* NOTDEF */
    if (PStuff->HeadBody[HP_RESENTTO]) {
	if ((PStuff->HeadBodyLen[HP_RESENTTO]+strlen(Msg->WideReply)+5) > WLen) {
	    WLen = PStuff->HeadBodyLen[HP_RESENTTO]+strlen(Msg->WideReply)+5+80;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* The aforementioned comma addition */
	if (*Msg->WideReply) strcat(Msg->WideReply, ", ");
	strncat(Msg->WideReply, PStuff->HeadBody[HP_RESENTTO], PStuff->HeadBodyLen[HP_RESENTTO]);
    }
    if (IncludeFrom && Msg->ReplyTo) {
	if ((strlen(Msg->ReplyTo)+strlen(Msg->WideReply)+5) > WLen) {
	    WLen = strlen(Msg->ReplyTo)+strlen(Msg->WideReply)+5+80;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	/* The aforementioned comma addition */
	if (*Msg->WideReply) strcat(Msg->WideReply, ", ");
	strcat(Msg->WideReply, Msg->ReplyTo);
    }
    if (PStuff->HeadBody[HP_BCC]) {
	if ((PStuff->HeadBodyLen[HP_BCC]+strlen(Msg->WideReply)+5) > WLen) {
	    WLen = PStuff->HeadBodyLen[HP_BCC]+strlen(Msg->WideReply)+5;
	    Msg->WideReply = realloc(Msg->WideReply, WLen);
	    if (Msg->WideReply == NULL) {
		AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDWIDEREPLY);
	    }
	}
	if (*Msg->WideReply) strcat(Msg->WideReply, ", ");
	strncat(Msg->WideReply, PStuff->HeadBody[HP_BCC], PStuff->HeadBodyLen[HP_BCC]);
    }
    if (!StripMyselfFromAddressList(Msg->WideReply, &NewWide)) {
	free(Msg->WideReply);
	Msg->WideReply = NewWide;
    }
    debug(16, ("Wide Reply field is <%s>\n", Msg->WideReply));
    return(0);
}
