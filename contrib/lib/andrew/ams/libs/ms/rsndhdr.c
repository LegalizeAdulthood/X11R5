/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *rsndhdr_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rsndhdr.c,v 2.9 1991/09/12 15:48:07 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rsndhdr.c,v 2.9 1991/09/12 15:48:07 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/rsndhdr.c,v $
*/
#include <andyenv.h>
#include <stdio.h>
#include <pwd.h>
#include <svcconf.h>
#include <util.h>
#include <ms.h>
#include <hdrparse.h>
#include <parseadd.h>
#include <mail.h>
#ifdef WHITEPAGES_ENV
#include <wp.h>
#endif /* #ifdef WHITEPAGES_ENV */

extern PARSED_ADDRESS *SingleAddress();
extern char MyMailDomain[];

AuthenticReSentHeader(msg)
struct MS_Message *msg;
{
    char *s;
    int len, outType, numitems;
    PARSED_ADDRESS *AddrHead, *SingAddr;
    char *IDpart, *PostID;
    struct passwd *p;

    if (msg->AuthUid == 0 || msg->AuthCell == NULL) return (0);
    s = msg->ParsedStuff->HeadBody[HP_RESENTFROM];
    if (!s) {
	return(0);
    }
    len = msg->ParsedStuff->HeadBodyLen[HP_RESENTFROM];
    s[len] = '\0';
    if (ParseAddressList(s, &AddrHead) != PA_OK) {
	s[len] = '\n';
	return(0);
    }
    s[len] = '\n';
    numitems = 0;
    SingAddr = SingleAddress(AddrHead, &numitems);
    if (numitems != 1) {
	FreeAddressList(AddrHead);
	return(0);
    }
    if (la_KindDomain(SingAddr, &outType, &IDpart, &PostID, msg->AuthCell) != laerr_NoError) {
	FreeAddressList(AddrHead);
	return(0);
    }
    FreeAddressList(AddrHead);
    if (outType != latype_LocalID) {
	if (IDpart) free(IDpart);
	return(0);
    }
    p = getcpwuid(msg->AuthUid, msg->AuthCell);
    if (p == NULL || strcmp(p->pw_name, IDpart)) {
	if (IDpart) free(IDpart);
	return(0);
    }
    if (IDpart) free(IDpart);
    return(1);
}
