/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/hdrparse.h,v 2.5 1991/09/12 19:15:58 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/hdrs/RCS/hdrparse.h,v $
*/
/*
    This is the list of the parts of the ParsedMsg structure.
    ANY ADDITIONS to this next part of the list should be reflected in the 
    InitializeMS routine in init.c 

    It is included here because the ParsedMsg structure is available only to the MS,
	but these symbolic values may be passed to the MS by clients.

*/
#define HP_REPLY_TO 0
#define HP_FROM 1
#define HP_SENDER 2
#define HP_TO 3
#define HP_CC 4
#define HP_ACK 5
#define HP_ACKTO 6
#define HP_ACKTYPE 7
#define HP_DATE 8
#define HP_SUBJECT 9
#define HP_ENCLOSURE 10
#define HP_NEWSGROUP 11
#define HP_NEWSGROUPS 12
#define HP_WIDEREPLY 13
#define HP_ATTENTION 14
#define HP_DIRECTORYCREATION 15
#define HP_RETURNPATH 16
#define HP_SCRIBEFORMAT 17
#define HP_RESENTFROM 18
#define HP_RESENTTO 19
#define HP_ALLREPLY 20
#define HP_AUTHENTICATED_AS 21
#define HP_RECEIVED 22
#define HP_MESSAGEID 23
#define HP_OLDVOTE 24
#define HP_VOTEREQUEST 25
#define HP_VOTETO 26
#define HP_VOTECHOICES 27
#define HP_CONTENTTYPE 28
#define HP_UNSUPPORTEDTYPE 29
#define HP_BCC 30
#define HP_INREPLYTO 31
#define HP_REFERENCES 32
#define HP_DISTRIBUTION 33
#define HP_MESSAGESIZE 34
#define HP_RESENTDATE 35
#define HP_RESENTMESSAGEID 36
#define HP_REDISTRIBUTION 37

#define HP_END_PREDEFINED 38  /* Index of first non-standard header */
/* Any changes to the above list require corresponding changes to the string array in init.c */
