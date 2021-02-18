/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *bldreply_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldreply.c,v 2.8 1991/09/12 15:41:49 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldreply.c,v 2.8 1991/09/12 15:41:49 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/bldreply.c,v $
*/
#include <ms.h>
#include <hdrparse.h>

#ifndef _IBMR2
extern char *malloc();
#endif /* _IBMR2 */

BuildReplyField(Msg)
struct MS_Message *Msg;
{
    struct ParsedMsg *PStuff;

    debug(1, ("Building Reply field\n"));
    PStuff = Msg->ParsedStuff;
    if (PStuff->HeadBody[HP_REPLY_TO]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_REPLY_TO]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_REPLY_TO], PStuff->HeadBodyLen[HP_REPLY_TO]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_REPLY_TO]] = '\0';
    } else if (PStuff->HeadBody[HP_FROM]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_FROM]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_FROM], PStuff->HeadBodyLen[HP_FROM]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_FROM]] = '\0';
    } 
/* The following are commented out on cfe's advice: */
#ifdef UNDEFINEDGARBAGE
    else if (PStuff->HeadBody[HP_RESENTFROM]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_RESENTFROM]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_RESENTFROM], PStuff->HeadBodyLen[HP_RESENTFROM]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_RESENTFROM]] = '\0';
    } else if (PStuff->HeadBody[HP_SENDER]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_SENDER]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_SENDER], PStuff->HeadBodyLen[HP_SENDER]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_SENDER]] = '\0';
    } else if (PStuff->HeadBody[HP_RETURNPATH]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_RETURNPATH]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_RETURNPATH], PStuff->HeadBodyLen[HP_RETURNPATH]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_RETURNPATH]] = '\0';
    }
#endif /* UNDEFINEDGARBAGE */
    debug(16, ("Reply to field is <%s>\n", Msg->ReplyTo));
    return(0);
}
