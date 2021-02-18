/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *prettyn_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/prettyn.c,v 2.12 1991/09/12 15:46:44 bobg Exp $";

/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/prettyn.c,v 2.12 1991/09/12 15:46:44 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/prettyn.c,v $
*/
#include <andyenv.h>
#include <andrewos.h>
#include <ms.h>
#include <hdrparse.h>
#include <util.h>
#include <svcconf.h>
#include <pwd.h>
#ifdef WHITEPAGES_ENV
#include <wp.h>
#endif /* WHITEPAGES_ENV */
#include <parseadd.h>

#define BIGFROM 24  /* c.f. bldcapt.c */
#define MINHOST 3

extern char *index(), *rindex();
extern char MyMailDomain[];

PARSED_ADDRESS *SingleAddress(AddrList, pCount)
PARSED_ADDRESS *AddrList;
int *pCount;
{
/* Count the number of addressees are in the initial list */
/* Return some single simple address */
	PARSED_ADDRESS *Single = NULL, *First = NULL;

	FOR_ALL_ADDRESSES(ThisAddr, AddrList, {
		switch (ThisAddr->Kind) {
	case SIMPLE_ADDRESS:
			(*pCount)++;
			Single = ThisAddr;
			break;
	case GROUP_ADDRESS:
			Single = SingleAddress(ThisAddr->Members, pCount);
			break;
	default:
			break;
		}
		if (!First) First = Single;
	})
	return(First);
}

FindPrettiestName(msg, FromBuf)
struct MS_Message *msg;
char *FromBuf;
{
    char MyFromBuf[1500];
    struct passwd *p;

    if (msg->ParsedStuff->HeadBody[HP_FROM]) {
	strncpy(MyFromBuf, msg->ParsedStuff->HeadBody[HP_FROM], msg->ParsedStuff->HeadBodyLen[HP_FROM]);
	MyFromBuf[msg->ParsedStuff->HeadBodyLen[HP_FROM]] = '\0';
    } else if (msg->ParsedStuff->HeadBody[HP_RESENTFROM]) {
	strncpy(MyFromBuf, msg->ParsedStuff->HeadBody[HP_RESENTFROM], msg->ParsedStuff->HeadBodyLen[HP_RESENTFROM]);
	MyFromBuf[msg->ParsedStuff->HeadBodyLen[HP_RESENTFROM]] = '\0';
    } else if (msg->ParsedStuff->HeadBody[HP_SENDER]) {
	strncpy(MyFromBuf, msg->ParsedStuff->HeadBody[HP_SENDER], msg->ParsedStuff->HeadBodyLen[HP_SENDER]);
	MyFromBuf[msg->ParsedStuff->HeadBodyLen[HP_SENDER]] = '\0';
    } else if (msg->ParsedStuff->HeadBody[HP_RETURNPATH]) {
	strncpy(MyFromBuf, msg->ParsedStuff->HeadBody[HP_RETURNPATH], msg->ParsedStuff->HeadBodyLen[HP_RETURNPATH]);
	MyFromBuf[msg->ParsedStuff->HeadBodyLen[HP_RETURNPATH]] = '\0';
    } else if (msg->ReplyTo) {
	strcpy(MyFromBuf, msg->ReplyTo);
    } else if (msg->AuthUid > 0) {
	errno = cpw_error = 0;
#ifdef WHITEPAGES_ENV
	p = getcpwuid(msg->AuthUid, (msg->AuthCell ? msg->AuthCell : WorkstationName));
	errno = ConvertWpErrToMSErr(cpw_error, E2BIG, FALSE);
#else /* WHITEPAGES_ENV */
	p = getpwuid(msg->AuthUid);
#endif /* WHITEPAGES_ENV */
	if (p == NULL) {
	    if (tfail(errno)) {
		strcpy(FromBuf, "<unknown (temp failure)>");
#ifdef WHITEPAGES_ENV
	    } else if (cpw_error == wperr_NoKeysFound) {
		sprintf(FromBuf, "<unknown (no user %d in cell %s)>", msg->AuthUid, (msg->AuthCell ? msg->AuthCell : WorkstationName));
#endif /* WHITEPAGES_ENV */
	    } else {
		sprintf(FromBuf, "<unknown (error %d)>", cpw_error);
	    }
	    return(0);
	} else {
	    char *uname;
	    GetNameFromGecos(p->pw_gecos, p->pw_name, (msg->AuthCell ? msg->AuthCell : WorkstationName), &uname);
	    if (uname) {
		ShrinkName(uname, FromBuf, BIGFROM);
		free(uname);
	    } else {
		strcpy(FromBuf, "<unknown (out of memory)>");
	    }
	    return(0);
	}
    } else {
	strcpy(FromBuf, "<unknown sender>");
	return(0);
    }
    return(FindPrettiestFromString(MyFromBuf, FromBuf));
}

FindPrettiestFromString(MyFromBuf, FromBuf)
char *MyFromBuf, *FromBuf;
{
    char *RealFrom, *s, *oparen, *cparen;
    int dummy = 0, len;
    PARSED_ADDRESS *Addr, *ListHead;

    debug(1, ("Finding prettiest in %s\n", MyFromBuf));
    if (ParseAddressList(MyFromBuf, &ListHead) != PA_OK) {
	/* illegal format, do what we can */
	RealFrom = MyFromBuf;
	oparen = index(RealFrom, '(');
	if (oparen) RealFrom = ++oparen;
	len = strlen(RealFrom);
	if (len > BIGFROM) {
	    RealFrom[BIGFROM] = '\0';
	}
	if (!index(RealFrom, '@')) {
	    RealFrom[BIGFROM-2] = '@';
	    RealFrom[BIGFROM-1] = '?';
	}
	strcpy(FromBuf, RealFrom);
	debug(1, ("Illegal address, returning %s\n", RealFrom));
	return(0);
    }
    Addr = SingleAddress(ListHead, &dummy);
    if (!Addr || (dummy < 1)) { /* empty address! */
	*FromBuf = '\0';
	FreeAddressList(ListHead);
	return(0);
    }
    if (Addr->RoutePhrase) {
	char *s;

	s = index(Addr->RoutePhrase, '(');
	if (s && (s > (Addr->RoutePhrase + 8))) {
	    /* This routephrase has something that looks like a comment in it.
	     The only time we've seen this it was something like 
	    "John Doe (jdoe%foo@bar.baz)" <jdoe%foo@bar.baz>
	     So we're currently tuned to that case... */
	    *s = '\0';
	}
	RealFrom = Addr->RoutePhrase;
    } else {
	ADDRESS_COMMENT *This;

	RealFrom = NULL;
	for (This = Addr->Comments; This != NULL; This = This->Next) {
	    if (This->Text && This->Text[0]) {
		char *s;

		RealFrom = This->Text;
		if (*RealFrom = '(') {
		    ++RealFrom;
		    s = rindex(RealFrom, ')');
		    if (s) *s = '\0';
		}
		break;
	    }
	}
	if (!RealFrom) {
	    RealFrom = Addr->LocalPart;
	}
    }
    s= RealFrom;
    while (TRUE) {
	oparen = index(s, '(');
	if (oparen) {
	    cparen = rindex(s, ')');
	    if (cparen && (cparen > oparen)) {
		s = ++oparen;
		*cparen = '\0';
		continue;
	    }
	}
	break;
    }
    debug(1, ("OK, I'm working with %s\n", s));
    ShrinkName(s, FromBuf, BIGFROM-1-MINHOST);
    strcat(FromBuf, "@");
    if (Addr->Hosts->Prev && Addr->Hosts->Prev->Name) {
	len = BIGFROM - strlen(FromBuf);
	strncat(FromBuf, Addr->Hosts->Prev->Name, len);
	FromBuf[BIGFROM] = '\0';
    } else {
	for (len = MINHOST; len; --len) {
	    strcat(FromBuf, "?");
	}
    }
    FreeAddressList(ListHead);
    return(0);
}
