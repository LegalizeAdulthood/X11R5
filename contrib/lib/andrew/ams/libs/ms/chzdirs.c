/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/chzdirs.c,v 2.18 1991/09/12 15:42:03 bobg Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/chzdirs.c,v $
*/
/*
        chzdirs.c -- Routines for deciding in which directories to put mail

        This is some of the ugliest code in the message server.  It doesn't
                really need to be quite as ugly as it is, but the subtleties
                would make it moderately ugly in any event.

        Good luck reading it.

*/

char            ams_choosedirs_rcsid[] = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/libs/ms/RCS/chzdirs.c,v 2.18 1991/09/12 15:42:03 bobg Exp $";

#include <andyenv.h>
#include <ms.h>
#include <util.h>
#include <mailconf.h>
#include <hdrparse.h>
#include <stdio.h>
#include <andrewos.h>                  /* sys/file.h sys/time.h */
#include <pwd.h>
#include <sys/stat.h>
#ifdef WHITEPAGES_ENV
#include <wp.h>
#endif                                 /* WHITEPAGES_ENV */
#include <mail.h>
#include <dropoff.h>

extern char    *DayArray[], *MonArray[];        /* Just the names of days and
                                                 * months */

extern char    *pop();
extern char    *index(), *rindex();

#ifndef _IBMR2
extern char    *malloc(), *realloc();
#endif /* _IBMR2 */

extern char    *StripWhiteEnds(), *GenAuthField();
extern FILE    *qopen();
extern char     Me[], *MyPrettyAddress, home[], MyMailDomain[];

#define StackAbort(err) AMS_RETURN_ERRCODE(EMSBADDIRSPEC, err, EVIA_CHOOSEDIRECTORIES)
#define CheckPoppability(x) if (StackSize(x) <= 0) {StackAbort(EIN_POP);}
#define PARSESTACK 0
#define NESTSTACK 1

#if 0
static int      subsumes(s, t)
char           *s, *t;
{
    int             slen, tlen;
    char           *lastneeded;

    slen = strlen(s);
    tlen = strlen(t);
    if (slen < tlen)
        return (0);
    lastneeded = s + slen - tlen;
    while (s <= lastneeded && strncmp(s, t, tlen)) {
        ++s;
    }
    return (s <= lastneeded ? 1 : 0);
}

/* The following routine takes a parsed message and chooses the
        directories to which it should be added, using a to-be-defined
        structure to do the choosing.  It also sets a caption template
        list that will specify the caption format for each of the directories. */

#define MAXCMDWORD 100                 /* Max length of simple command (no
                                        * args) */

static char    *ValidCmds[] = {
#define CMD_BOGUS 0
    "$bogus",
#define CMD_QUOTE 1
    "$$",
#define CMD_CONTAINS 2
    "$contains",
#define CMD_ADDTODIR 3
    "$addtodir",
#define CMD_HEADER 4
    "$header",
#define CMD_EXEC 5
    "$exec",
#define CMD_POP 6
    "$pop",
#define CMD_DEFAULT 7
    "$default",
#define CMD_CLEAR 8
    "$clear",
#define CMD_COMMENT 9
    "$#",
#define CMD_PUSH 10
    "$push",
#define CMD_IF 11
    "$if",
#define CMD_ELSE 12
    "$else",
#define CMD_ENDIF 13
    "$endif",
#define CMD_STOP 14
    "$stop",
#define CMD_LCHEADER 15
    "$lcheader",
#define CMD_HEADERMAP 16
    "$headermap",
#define CMD_ADDHEAD 17
    "$addhead",
#define CMD_REJECT 18
    "$reject",
#define CMD_NONEMPTY 19
    "$nonempty",
#define CMD_ONEHEADERMAP 20
    "$oneheadermap",
#define CMD_DUPHEADERS 21
    "$dupheaders",
#define CMD_LCDUPHEADERS 22
    "$lcdupheaders",
#define CMD_LCSENDINGHEADERS 23
    "$lcsendingheaders",
#define CMD_EXTRACTMAPS 24
    "$extractmaps",
#define CMD_ONEHEADERMAPWITHEXTRA 25
    "$oneheadermapwithextra",
#define CMD_AUTHUSER 26
    "$authuser",
#define CMD_BUILDWIDEMAP 27
    "$buildwidemap",
#define CMD_REJECTIONSTO 28
    "$rejectionsto",
#define CMD_CONTAINSONEOF 29
    "$containsoneof",
#define CMD_BUILDNEWSGROUPS 30
    "$buildnewsgroups",
#define CMD_ZEROLIST 31
    "$zerolist",
#define CMD_EXTRACTLIBERALLY 32
    "$extractliberally",
#define CMD_RESEND 33
    "$resend",
#define CMD_REJECTCC 34
    "$rejectcc",
#define CMD_TRACELESSRESEND 35
    "$tracelessresend",
#define CMD_MAPHASHTOPLUS 36
    "$maphashtoplus",
#define CMD_MAPPLUSTOHASH 37
    "$mapplustohash",
#define CMD_PRESERVEHASHANDPLUS 38
    "$preservehashandplus",
#define CMD_DOTSTOHYPHEN 39
    "$dotstohyphen",
#define CMD_NAMELESSCAPTION 40
    "$namelesscaption",
#define CMD_UNFORMAT 41
    "$unformat",
#define CMD_RELAX 42
    "$relax",
#define CMD_DELHEAD 43
    "$delhead",
#define CMD_FINAL 44                   /* This line MUST change when new
                                        * commands are added */
    /* It should be ONE GREATER than the last valid command */
    0,
};

#define WIDEREPLYPREFIX "X-Andrew-WideReply:"


ChooseDirectories(Msg, DirList, CaptionTemplateList, NumDirs, DirLim, ChoiceSpec)
struct MS_Message *Msg;
char            DirList[MAXDIRINSERTIONS][MAXPATHLEN + 1];
struct MS_CaptionTemplate CaptionTemplateList[MAXDIRINSERTIONS];
int            *NumDirs, DirLim;
char           *ChoiceSpec;

{
    char           *CurrCmd, *poptemp;
    int             mycode;

    CurrCmd = malloc(500);
    if (!CurrCmd)
        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
    poptemp = malloc(500);
    if (!poptemp)
        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
    mycode = RealChooseDirectories(Msg, DirList, CaptionTemplateList, NumDirs, DirLim, ChoiceSpec, &CurrCmd, &poptemp);
    if (CurrCmd)
        free(CurrCmd);
    if (poptemp)
        free(poptemp);
    return (mycode);
}


/* This whole thing is a horrible hack --- the two separate routines simply allow
        simpler storage management for poptemp */

RealChooseDirectories(Msg, DirList, CaptionTemplateList, NumDirs, DirLim, ChoiceSpec, CurrCmd, poptemp)
struct MS_Message *Msg;
char            DirList[MAXDIRINSERTIONS][MAXPATHLEN + 1];
struct MS_CaptionTemplate CaptionTemplateList[MAXDIRINSERTIONS];
int            *NumDirs, DirLim;
char           *ChoiceSpec, **CurrCmd, **poptemp;

{
    char           *NextCmd = ChoiceSpec, CommandWord[MAXCMDWORD], *Args, DefaultBuf[MAXPATHLEN + 1], ErrorText[256], RejectionsTo[500], RejectCC[500];
    Boolean         ParsingError = FALSE, MustDoDefault = FALSE;
    int             i = 0, Cindex, Rejections = 0, parsingerrorcode = 0, NamelessCaption = 0, Relaxing = 0;

    debug(1, ("ChooseDirectories\n"));
    RejectionsTo[0] = '\0';
    RejectCC[0] = '\0';
    clearstack(PARSESTACK);
    clearstack(NESTSTACK);
    *NumDirs = 0;
    DefaultBuf[0] = '\0';
    while (!ParsingError && NextCmd && *NextCmd) {
        if (AdvanceCmd(&NextCmd, CurrCmd)) {
            ParsingError = TRUE;
            parsingerrorcode = mserrcode;
            break;
        }
        debug(8, ("Current command line is %s\n", *CurrCmd));
restart:
        if (**CurrCmd == '$') {
            for (i = 0, Args = *CurrCmd;
                 *Args && *Args != ' ' && *Args != '\t' && *Args != '\n';
                 ++Args, ++i) {
                CommandWord[i] = *Args;
            }
            CommandWord[i] = '\0';
            Cindex = 0;
            while (Cindex < CMD_FINAL && strcmp(CommandWord, ValidCmds[Cindex])) {
                ++Cindex;
            }
            if (Cindex < 0 || Cindex >= CMD_FINAL) {
                Cindex = CMD_BOGUS;
            }
        }
        else {
            Cindex = CMD_PUSH;
            Args = *CurrCmd;
        }
        Args = StripWhiteEnds(Args);
        Stack_MapHashPlusAsAppropriate(Args);   /* in stack.c */
        switch (Cindex) {
            case CMD_NAMELESSCAPTION:
                NamelessCaption = 1;
                break;
            case CMD_DOTSTOHYPHEN:
                {
                    char           *s;

                    CheckPoppability(PARSESTACK);
                    if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    strcpy(*poptemp, pop(PARSESTACK));
                    for (s = *poptemp; *s; ++s) {
                        if (*s == '.')
                            *s = '-';
                    }
                    if (push(*poptemp, FALSE, PARSESTACK) == -1)
                        StackAbort(EIN_PUSH);
                    break;
                }
            case CMD_MAPHASHTOPLUS:
                Stack_MapHashes();
                break;
            case CMD_MAPPLUSTOHASH:
                Stack_MapPluses();
                break;
            case CMD_PRESERVEHASHANDPLUS:
                Stack_MapNoChars();
                break;
            case CMD_NONEMPTY:
                if (PushNonEmptiness(PARSESTACK) == -1)
                    StackAbort(EIN_PUSH);
                break;
            case CMD_QUOTE:
                if (push(*CurrCmd + 1, FALSE, PARSESTACK) == -1)
                    StackAbort(EIN_PUSH);
                break;
            case CMD_CONTAINS:
                CheckPoppability(PARSESTACK);
                if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);

                strcpy(*poptemp, pop(PARSESTACK));
                CheckPoppability(PARSESTACK);
                if (push(subsumes(*poptemp, pop(PARSESTACK)) ? "1" : "0", FALSE, PARSESTACK) == -1)
                    StackAbort(EIN_PUSH);
                break;
            case CMD_CONTAINSONEOF:
                {
                    char           *poptemp2, *s, *t;
                    Boolean         Matched = FALSE;

                    CheckPoppability(PARSESTACK);
                    if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);

                    strcpy(*poptemp, pop(PARSESTACK));
                    poptemp2 = malloc(1 + strlen(*poptemp));
                    if (!poptemp2) {
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    }
                    strcpy(poptemp2, *poptemp);
                    CheckPoppability(PARSESTACK);
                    if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL) {
                        free(poptemp2);
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    }
                    strcpy(*poptemp, pop(PARSESTACK));
                    ReduceWhiteSpace(*poptemp);
                    t = *poptemp;
                    while (t) {
                        s = index(t, ' ');
                        if (s)
                            *s++ = '\0';
                        if (subsumes(poptemp2, t)) {
                            debug(8, ("%s seems to subsume %s\n", poptemp2, t));
                            Matched = TRUE;
                            break;
                        }
                        t = s;
                    }
                    free(poptemp2);
                    if (push(Matched ? "1" : "0", FALSE, PARSESTACK) == -1)
                        StackAbort(EIN_PUSH);
                    break;
                }
            case CMD_ADDTODIR:
                {
                    char           *captemp = NULL, Fname[MAXPATHLEN + 1];

                    bzero(&(CaptionTemplateList[*NumDirs]), sizeof(struct MS_CaptionTemplate));
                    if (NamelessCaption) {
                        CaptionTemplateList[*NumDirs].basictype = BASICTEMPLATE_NOFROM;
                    }
                    if ((captemp = index(Args, ',')) != NULL) {
                        *captemp++ = '\0';
                        NonfatalBizarreError("Cannot yet process caption templates -- unimplemented feature");
                    }
                    if (MS_DisambiguateFile(Args, Fname, TRUE) != 0) {
                        return (mserrcode);
                    }
                    if (NotOnList(DirList, Fname, *NumDirs)) {
                        strcpy(DirList[*NumDirs], Fname);
                        if (++*NumDirs >= MAXDIRINSERTIONS) {
                            sprintf(ErrorText, "Messages may not be posted on more than %d directories at a time", MAXDIRINSERTIONS - 1);
                            *NumDirs = 0;
                            if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                return (mserrcode);
                            }
                            ++Rejections;
                            NextCmd = NULL;
                            break;
                        }
                    }
                    break;
                }
            case CMD_HEADER:
            case CMD_LCHEADER:
            case CMD_DUPHEADERS:
            case CMD_LCDUPHEADERS:
            case CMD_LCSENDINGHEADERS:
                {
                    char           *pstring = NULL;
                    int             plen = 0, addlen, myindex;

                    for (myindex = 0; Msg->ParsedStuff->HeadName[myindex] != NULL; ++myindex) {
                        if ((Cindex != CMD_LCSENDINGHEADERS && !strcmp(Args, Msg->ParsedStuff->HeadName[myindex]))
                            || (Cindex == CMD_LCSENDINGHEADERS &&
                          (!strcmp("to", Msg->ParsedStuff->HeadName[myindex])
                        || !strcmp("cc", Msg->ParsedStuff->HeadName[myindex])
                           || !strcmp("apparently-to", Msg->ParsedStuff->HeadName[myindex])
                           || !strcmp("resent-to", Msg->ParsedStuff->HeadName[myindex])))) {
                            addlen = Msg->ParsedStuff->HeadBodyLen[myindex];
                            if (addlen > 0) {
                                if (pstring && (Cindex == CMD_DUPHEADERS || Cindex == CMD_LCDUPHEADERS || Cindex == CMD_LCSENDINGHEADERS)) {
                                    pstring = realloc(pstring, plen + addlen + 4);
                                    plen += addlen + 2;
                                    strcat(pstring, ", ");
                                    strncat(pstring, Msg->ParsedStuff->HeadBody[myindex], addlen);
                                }
                                else {
                                    pstring = malloc(addlen + 1);
                                    plen += addlen;
                                    strncpy(pstring, Msg->ParsedStuff->HeadBody[myindex], addlen);
                                }
                                pstring[plen] = '\0';
                            }
                            if (Cindex == CMD_LCSENDINGHEADERS)
                                continue;
                            if (pstring) {
                                if (Cindex == CMD_HEADER || Cindex == CMD_LCHEADER || !Msg->ParsedStuff->IsDuplicated[myindex]) {
                                    if (push(pstring, (Cindex == CMD_LCHEADER || Cindex == CMD_LCDUPHEADERS), PARSESTACK) == -1) {
                                        free(pstring);
                                        StackAbort(EIN_PUSH);
                                    }
                                    free(pstring);
                                    break;
                                }
                            }
                        }
                    }
                    if (pstring && Cindex == CMD_LCSENDINGHEADERS) {
                        if (push(pstring, TRUE, PARSESTACK) == -1) {
                            free(pstring);
                            StackAbort(EIN_PUSH);
                        }
                        free(pstring);
                    }
                    else
                        if (!pstring || Msg->ParsedStuff->HeadName[i] == NULL) {
                            if (push("", FALSE, PARSESTACK) == -1)
                                StackAbort(EIN_PUSH);
                        }
                    break;
                }
            case CMD_EXEC:
                for (i = atoi(Args); i > 0; --i) {
                    CheckPoppability(PARSESTACK);
                    pop(PARSESTACK);
                }
                CheckPoppability(PARSESTACK);
                if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);

                strcpy(*poptemp, pop(PARSESTACK));
                *CurrCmd = realloc(*CurrCmd, 1 + strlen(*poptemp));
                if (!*CurrCmd)
                    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                strcpy(*CurrCmd, *poptemp);
                goto restart;
            case CMD_POP:
                i = atoi(Args);
                if (i == 0)
                    i = 1;
                while (i-- > 0) {
                    CheckPoppability(PARSESTACK);
                    pop(PARSESTACK);
                }
                break;
            case CMD_DEFAULT:
                if (MS_DisambiguateFile(Args, DefaultBuf, TRUE) != 0) {
                    AMS_RETURN_ERRCODE(ENOENT, EIN_DISAMB, EVIA_CHOOSEDIRECTORIES);
                }
                break;
            case CMD_CLEAR:
                clearstack(PARSESTACK);
                break;
            case CMD_COMMENT:
                break;
            case CMD_PUSH:
                if (push(Args, FALSE, PARSESTACK) == -1)
                    StackAbort(EIN_PUSH);
                break;
            case CMD_IF:

                /*
                 * Do not need to do anything for true -- execute sequentially
                 */
                CheckPoppability(PARSESTACK);
                if (atoi(pop(PARSESTACK))) {    /* test true, enter then
                                                 * clause */
                    if (push("if", FALSE, NESTSTACK) == -1) {
                        StackAbort(EIN_PUSH);
                    }
                    if (push(Args, FALSE, NESTSTACK) == -1) {
                        StackAbort(EIN_PUSH);
                    }
                }
                else {
                    char           *elsetext, *endiftext;

                    elsetext = malloc(7 + strlen(Args));
                    if (!elsetext)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    endiftext = malloc(9 + strlen(Args));
                    if (!endiftext) {
                        free(elsetext);
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    }
                    sprintf(elsetext, "$else %s", Args);
                    sprintf(endiftext, "$endif %s", Args);
                    while (TRUE) {
                        if (AdvanceCmd(&NextCmd, CurrCmd)) {
                            parsingerrorcode = mserrcode;
                            ParsingError = TRUE;
                            break;
                        }
                        if (!strncmp(endiftext, *CurrCmd, strlen(endiftext))) {
                            break;
                        }
                        if (!strncmp(elsetext, *CurrCmd, strlen(elsetext))) {
                            if (push("else", FALSE, NESTSTACK) == -1) {
                                free(elsetext);
                                free(endiftext);
                                StackAbort(EIN_PUSH);
                            }
                            if (push(elsetext + 6, FALSE, NESTSTACK) == -1) {
                                free(elsetext);
                                free(endiftext);
                                StackAbort(EIN_PUSH);
                            }
                            break;
                        }
                    }
                    free(elsetext);
                    free(endiftext);
                }
                break;
            case CMD_ELSE:
                {
                    char           *endiftext;

                    CheckPoppability(NESTSTACK);
                    if (strcmp(pop(NESTSTACK), Args)) {
                        AMS_RETURN_ERRCODE(EMSDIRNEST, EIN_ELSEPARSE, EVIA_CHOOSEDIRECTORIES);
                    }
                    CheckPoppability(NESTSTACK);
                    if (strcmp(pop(NESTSTACK), "if")) {
                        AMS_RETURN_ERRCODE(EMSDIRNEST, EIN_ELSEPARSE, EVIA_CHOOSEDIRECTORIES);
                    }
                    endiftext = malloc(8 + strlen(Args));
                    if (!endiftext) {
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    }
                    sprintf(endiftext, "$endif %s", Args);
                    while (TRUE) {
                        if (AdvanceCmd(&NextCmd, CurrCmd)) {
                            parsingerrorcode = mserrcode;
                            ParsingError = TRUE;
                            break;
                        }
                        if (!strncmp(endiftext, *CurrCmd, strlen(endiftext))) {
                            break;
                        }
                    }
                    free(endiftext);
                    break;
                }
            case CMD_ENDIF:
                {
                    char           *thispop;

                    CheckPoppability(NESTSTACK);
                    if (strcmp(pop(NESTSTACK), Args)) {
                        AMS_RETURN_ERRCODE(EMSDIRNEST, EIN_ENDIFPARSE, EVIA_CHOOSEDIRECTORIES);
                    }
                    CheckPoppability(NESTSTACK);
                    thispop = pop(NESTSTACK);
                    if (strcmp(thispop, "if") && strcmp(thispop, "else")) {
                        AMS_RETURN_ERRCODE(EMSDIRNEST, EIN_ENDIFPARSE, EVIA_CHOOSEDIRECTORIES);
                    }
                    break;
                }
            case CMD_STOP:
                NextCmd = NULL;
                break;
            case CMD_ADDHEAD:
                if (AddHeader(Msg, Args))
                    return (mserrcode);
                break;
            case CMD_DELHEAD:
                if (DelHeaderByName(Msg, StripWhiteEnds(Args)))
                    return (mserrcode);
                break;
            case CMD_ONEHEADERMAPWITHEXTRA:
            case CMD_ONEHEADERMAP:
            case CMD_HEADERMAP:
                {
                    char           *s, *t, *sdum, FileTemp[MAXPATHLEN + 1], ParentName[MAXPATHLEN + 1], alteredname[150], InitFileTemp[MAXPATHLEN + 1], AfterParent[150];
                    int             CreateLevel, LevelCount, primarycode, missingcode, tempcode;
                    Boolean         HasParent = TRUE;

                    debug(8, ("Headermap\n"));
                    CheckPoppability(PARSESTACK);
                    if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);

                    strcpy(*poptemp, pop(PARSESTACK));
                    if (*poptemp[0] == '\0')
                        break;         /* Do not map a nonexistent header */
                    for (s = Args; *s && *s != ' '; ++s) {
                        ;
                    }
                    if (*s) {
                        *s++ = '\0';
                    }
                    CreateLevel = atoi(Args);
                    Args = (*s) ? s : NULL;

                    debug(8, ("Mapping header contents %s onto root %s createlevel %d\n", *poptemp, Args ? Args : "<NO ROOT>", CreateLevel));
                    for (s = StripWhiteEnds(*poptemp); *s; s = t) {
                        debug(8, ("Top of loop, s=\"%s\"\n", s));
                        while (*s && (*s == ' ' || *s == ',' || *s == '\t'))
                            ++s;
                        for (t = s; *t && *t != ' ' && *t != '\t' && *t != ','; ++t) {
                             /* NO-OP */ ;
                        }
                        if (*t) {
                            *t++ = '\0';
                        }
                        debug(8, ("Considering component %s\n", s));
                        LevelCount = 1;
                        strcpy(alteredname, s);
                        for (sdum = alteredname; *sdum; ++sdum) {
                            switch (*sdum) {
                                case '.':
                                    *sdum = '/';        /* Drop through */
                                case '/':
                                    ++LevelCount;
                            }
                        }
                        if (Args) {
                            sprintf(InitFileTemp, "%s/%s", Args, alteredname);
                        }
                        else {
                            strcpy(InitFileTemp, alteredname);
                        }
                        strcpy(ParentName, InitFileTemp);
                        for (sdum = ParentName + strlen(ParentName) - 1;
                             sdum > ParentName && *sdum != '/'; --sdum);
                        if (*sdum == '/') {
                            *sdum = '\0';
                            strcpy(AfterParent, sdum + 1);
                            for (sdum--; sdum > ParentName && *sdum != '/'; --sdum);
                            if (*sdum == '/' && !strncmp(sdum + 1, MS_TREEROOT, strlen(MS_TREEROOT))) {
                                HasParent = FALSE;
                            }
                        }
                        else {
                            HasParent = FALSE;
                        }
                        if (InitFileTemp[0] == '/') {
/*                      strcpy(FileTemp, InitFileTemp);
                        missingcode = access(FileTemp, F_OK);
*/
                            missingcode = MS_DisambiguateFile(InitFileTemp, FileTemp, TRUE);
                            if (missingcode)
                                strcpy(FileTemp, InitFileTemp);
                        }
                        else {
                            missingcode = MS_DisambiguateFile(InitFileTemp, FileTemp, TRUE);
                            if (missingcode) {
                                tempcode = MS_DisambiguateFile(ParentName, FileTemp, TRUE);
                                if (tempcode) {
                                    if (vdown(AMS_ERRNO)) {
                                        return (mserrcode);
                                    }
                                    sprintf(ErrorText, "MS-Daemon-Error: Cannot create message directory %s -- parent directory does not exist (%d,%d,%d)", s, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);

                                    if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                        return (mserrcode);
                                    }
                                    ++Rejections;
                                    break;
                                }
                                strcpy(ParentName, FileTemp);
                                strcat(FileTemp, "/");
                                strcat(FileTemp, AfterParent);
                            }
                        }
                        if (missingcode) {
                            struct MS_Directory *Dir;

                            mserrcode = missingcode;
                            if (AMS_ERRNO != ENOENT) {
                                return (mserrcode);
                            }
                            if (CreateLevel < 0 || LevelCount <= CreateLevel) {
                                sprintf(ErrorText, "User is not authorized to create message directory %s", s);
                                if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                    return (mserrcode);
                                }
                                ++Rejections;
                                break;
                            }
                            if (CreateNewMSDirectory(FileTemp, &Dir, TRUE) != 0) {
                                if (vdown(AMS_ERRNO)) {
                                    return (mserrcode);
                                }
                                sprintf(ErrorText, "Unable to create new message directory %s", FileTemp);
                                if (AMS_ERRNO == ENOENT) {
                                    if (Relaxing) {
                                        debug(8, ("Can't create %s but I'm relaxed anyway, dude\n", FileTemp));
                                        continue;
                                    }
                                    strcat(ErrorText, " (Nonexistent parent)");
                                }
                                else
                                    if (AMS_ERRNO == EACCES) {
                                        strcat(ErrorText, " (Permission denied)");
                                    }
                                    else
                                        if (AMS_ERRNO == EINVAL) {
                                            strcat(ErrorText, " (Illegal folder name)");
                                        }
                                        else {
                                            char            FooBuf[20];

                                            sprintf(FooBuf, " (Error %d)", AMS_ERRNO);
                                            strcat(ErrorText, FooBuf);
                                        }
                                if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                    return (mserrcode);
                                }
                                ++Rejections;
                                continue;
                            }
/* There should be a way to set the caption template! */
                            if (HasParent) {
                                if (AddParentalMessage(Msg, ParentName, s, FileTemp) != 0) {
                                    if (vdown(AMS_ERRNO)) {
                                        return (mserrcode);
                                    }
                                    primarycode = mserrcode;
                                    sprintf(ErrorText, "Could not add message to parent (%s) of %s (%d, %d, %d)", ParentName, s, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
                                    if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                        return (primarycode);
                                    }
                                    return (primarycode);
                                }
                            }
                        }
/* There should be a way to set the caption template! */
                        bzero(&(CaptionTemplateList[*NumDirs]), sizeof(struct MS_CaptionTemplate));
                        if (NamelessCaption) {
                            CaptionTemplateList[*NumDirs].basictype = BASICTEMPLATE_NOFROM;
                        }
                        if (NotOnList(DirList, FileTemp, *NumDirs)) {
                            strcpy(DirList[*NumDirs], FileTemp);
                            if (++*NumDirs >= MAXDIRINSERTIONS) {
                                sprintf(ErrorText, "Messages may not be posted on more than %d directories at a time", MAXDIRINSERTIONS - 1);
                                *NumDirs = 0;
                                if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                    return (mserrcode);
                                }
                                ++Rejections;
                                NextCmd = NULL;
                                break;
                            }
                        }
                        if (Cindex == CMD_ONEHEADERMAPWITHEXTRA) {
                            char            NewHeader[250];

                            strcpy(NewHeader, pop(PARSESTACK));
                            strcat(NewHeader, s);
                            strcat(NewHeader, pop(PARSESTACK));
                            if (AddHeader(Msg, NewHeader))
                                return (mserrcode);
                        }
                        if (Cindex != CMD_HEADERMAP)
                            *t = '\0';
                    }
                    break;
                }
            case CMD_ZEROLIST:
                *NumDirs = 0;
                break;
            case CMD_REJECT:
                if (RejectMessage(Msg, Args, RejectionsTo, RejectCC)) {
                    return (mserrcode);
                }
                ++Rejections;
                break;
            case CMD_RESEND:
            case CMD_TRACELESSRESEND:
                if (ResendMessageFromMailbox(Msg, Args, (Cindex == CMD_RESEND) ? TRUE : FALSE)) {
                    return (mserrcode);
                }
                ++Rejections;          /* Well, a bad variable name, I guess */
                break;
            case CMD_REJECTIONSTO:
                strncpy(RejectionsTo, Args, sizeof(RejectionsTo));
                break;
            case CMD_REJECTCC:
                strncpy(RejectCC, Args, sizeof(RejectCC));
                break;
            case CMD_AUTHUSER:
                {
                    struct passwd  *p;

                    if (Msg->AuthUid < 0) {
                        if (push("<UNAUTHENTICATED>", FALSE, PARSESTACK) == -1)
                            StackAbort(EIN_PUSH);
                    }
                    else
                        if (Msg->AuthUid == 0) {
                            if (push("root", FALSE, PARSESTACK) == -1)
                                StackAbort(EIN_PUSH);
                        }
                        else {
                            p = getcpwuid(Msg->AuthUid, Msg->AuthCell ? Msg->AuthCell : WorkstationName);
                            if (p) {
                                if (push(p->pw_name, FALSE, PARSESTACK) == -1)
                                    StackAbort(EIN_PUSH);
                            }
                            else {
                                if (vdown(errno)) {
                                    if (push("<TEMPORARILY UNRECOGNIZED USER>", FALSE, PARSESTACK) == -1)
                                        StackAbort(EIN_PUSH);
                                }
                                else {
                                    if (push("<UNKNOWN USER>", FALSE, PARSESTACK) == -1)
                                        StackAbort(EIN_PUSH);
                                }
                            }
                        }
                    break;
                }
            case CMD_EXTRACTMAPS:
            case CMD_EXTRACTLIBERALLY:

                /*
                 * Take what is on top of the stack, and extract all mappable
                 * "To" forms from it, putting the result back on the stack.
                 * E.g. turn "bb#foo.bar, yes, bb#x.y" into "foo.bar, x.y".
                 */
                {
                    int             len;
                    Boolean         Ready = TRUE, SawQuote = FALSE;
                    char           *s, *t, *r, *q;
                    char           *NewString;

                    Args = StripWhiteEnds(Args);
                    CheckPoppability(PARSESTACK);
                    if ((*poptemp = realloc(*poptemp, StackTopSize(PARSESTACK))) == NULL)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);

                    strcpy(*poptemp, pop(PARSESTACK));
                    debug(8, ("Poptemp is \"%s\"\n", *poptemp));
                    len = strlen(Args);
                    t = *poptemp + strlen(*poptemp) - len;
                    NewString = malloc(len + strlen(*poptemp) + 5);
                    if (!NewString)
                        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                    *NewString = '\0';
                    for (s = *poptemp; s <= t; ++s) {
                        if (Ready) {
                            if (!strncmp(s, Args, len)) {
                                s += len;
                                r = s;
                                while (*s && *s != ',')
                                    ++s;
                                *s = '\0';
                                if (SawQuote) {
                                    q = rindex(r, '"');
                                    if (q)
                                        *q = '\0';
                                }
                                if (Cindex == CMD_EXTRACTLIBERALLY) {
                                    do {
                                        q = rindex(r, ' ');
                                        if (q)
                                            *q = '\0';
                                    } while (q);
                                    do {
                                        q = rindex(r, '\n');
                                        if (q)
                                            *q = '\0';
                                    } while (q);
                                    do {
                                        q = rindex(r, ';');
                                        if (q)
                                            *q = '\0';
                                    } while (q);
                                    do {
                                        q = rindex(r, '>');
                                        if (q)
                                            *q = '\0';
                                    } while (q);
                                    do {
                                        q = rindex(r, ')');
                                        if (q)
                                            *q = '\0';
                                    } while (q);
                                }
                                do {
                                    q = rindex(r, '@');
                                    if (q)
                                        *q = '\0';
                                } while (q);
                                do {
                                    q = rindex(r, '%');
                                    if (q)
                                        *q = '\0';
                                } while (q);
                                do {
                                    q = rindex(r, '<');
                                    if (q)
                                        *q = '\0';
                                } while (q);
                                do {
                                    q = rindex(r, '(');
                                    if (q)
                                        *q = '\0';
                                } while (q);
                                NewString = realloc(NewString, strlen(NewString) + strlen(r) + 4);
                                if (!NewString)
                                    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
                                if (*NewString) {
                                    strcat(NewString, ", ");
                                }
                                strcat(NewString, r);
                            }
                            else {
                                if (*s == '"') {
                                    SawQuote = TRUE;
                                }
                                else
                                    if (*s != ' ' && *s != '\t' && *s != '\n') {
                                        Ready = (Cindex == CMD_EXTRACTLIBERALLY) ? TRUE : FALSE;
                                    }
                            }
                        }
                        else {
                            if (*s == ',' || Cindex == CMD_EXTRACTLIBERALLY) {
                                Ready = TRUE;
                                SawQuote = FALSE;
                            }
                        }
                    }
                    debug(8, ("Extract maps, result: \"%s\"\n", NewString));
                    if (push(NewString, FALSE, PARSESTACK) == -1)
                        StackAbort(EIN_PUSH);
                    free(NewString);
                    break;
                }
            case CMD_BUILDWIDEMAP:
                {
                    int             i;
                    char            MyNewHead[2000], *s, *prefix, *suffix;

                    debug(8, ("Building wide map from existing directory specs\n"));
                    if (*NumDirs <= 0)
                        break;         /* No empty wide reply fields */
                    prefix = Args;
                    suffix = index(Args, ' ');
                    if (suffix)
                        *suffix++ = '\0';
                    strcpy(MyNewHead, "X-Andrew-WideReply: ");
                    for (i = 0; i < *NumDirs; ++i) {
                        for (s = DirList[i]; s;) {
                            if (!strncmp(s, "/.MESSAGES", 10)) {
                                s = index(s + 1, '/');
                                break;
                            }
                            s = index(s + 1, '/');
                        }
                        if (i)
                            strcat(MyNewHead, ", ");
                        strcat(MyNewHead, prefix);
                        if (s) {
                            strcat(MyNewHead, s + 1);
                        }
                        else {
                            strcat(MyNewHead, DirList[i]);
                        }
                        if (suffix)
                            strcat(MyNewHead, suffix);
                    }
                    for (s = MyNewHead; *s; ++s) {
                        if (*s == '/')
                            *s = '.';
                    }
                    if (AddHeader(Msg, MyNewHead))
                        return (mserrcode);
                    debug(8, ("Added wide reply field %s\n", MyNewHead));
                    break;
                }
            case CMD_BUILDNEWSGROUPS:
                {
                    int             i;
                    char            MyNewHead[2000], *s, *prefix;
                    struct passwd  *p;

                    debug(8, ("Building newsgroups header from existing directory specs\n"));
                    if (Msg->AuthUid <= 0) {
                        sprintf(MyNewHead, "Path: %s!%s+uid%d", MyMailDomain, PostmasterName, Msg->AuthUid);
                    }
                    else {
                        p = getcpwuid(Msg->AuthUid, MyMailDomain);
                        if (p) {
                            sprintf(MyNewHead, "Path: %s!%s+", MyMailDomain, p->pw_name);
                        }
                        else {
                            sprintf(MyNewHead, "Path: %s!%s+uid%d", MyMailDomain, PostmasterName, Msg->AuthUid);
                        }
                    }
                    if (AddHeader(Msg, MyNewHead))
                        return (mserrcode);
                    debug(8, ("Added path header %s\n", MyNewHead));
                    prefix = Args;
                    strcpy(MyNewHead, "Newsgroups: ");
                    for (i = 0; i < *NumDirs; ++i) {
                        for (s = DirList[i]; s;) {
                            if (!strncmp(s, "/.MESSAGES", 10)) {
                                s = index(s + 1, '/');
                                break;
                            }
                            s = index(s + 1, '/');
                        }
                        if (i)
                            strcat(MyNewHead, ", ");
                        ++s;
                        if (strlen(s) <= strlen(prefix) || strncmp(s, prefix, strlen(prefix))) {
                            sprintf(ErrorText, "Cannot build newsgroups header for message going to %s", DirList[i]);
                            if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                                return (mserrcode);
                            }
                            ++Rejections;
                            break;
                        }
                        strcat(MyNewHead, s + strlen(prefix));
                    }
                    if (Rejections)
                        break;
                    for (s = MyNewHead; *s; ++s) {
                        if (*s == '/')
                            *s = '.';
                    }
                    if (AddHeader(Msg, MyNewHead))
                        return (mserrcode);
                    debug(8, ("Added newsgroups field %s\n", MyNewHead));
                    break;
                }
            case CMD_UNFORMAT:
                if (UnformatMessage(Msg))
                    return (mserrcode);
                break;
            case CMD_RELAX:
                Relaxing = 1;
                break;
            default:
                AMS_RETURN_ERRCODE(EMSBADDIRSPEC, EIN_DIRSPECPARSE, EVIA_CHOOSEDIRECTORIES);
        }
    }
    if (ParsingError) {
        if (parsingerrorcode) {
            return (parsingerrorcode);
        }
        else {
            AMS_RETURN_ERRCODE(EMSBADDIRSPEC, EIN_DIRSPECPARSE, EVIA_CHOOSEDIRECTORIES);
        }
    }
    if (MustDoDefault) {
        if (!DefaultBuf[0]) {
            AMS_RETURN_ERRCODE(EMSORPHANMSG, EIN_DIRSPECPARSE, EVIA_CHOOSEDIRECTORIES);
        }
        if (NotOnList(DirList, DefaultBuf, *NumDirs)) {
            strcpy(DirList[*NumDirs], DefaultBuf);
            bzero(&(CaptionTemplateList[*NumDirs]), sizeof(struct MS_CaptionTemplate));
            if (NamelessCaption) {
                CaptionTemplateList[*NumDirs].basictype = BASICTEMPLATE_NOFROM;
            }
            if (++*NumDirs >= MAXDIRINSERTIONS) {
                sprintf(ErrorText, "Messages may not be posted on more than %d directories at a time", MAXDIRINSERTIONS - 1);
                *NumDirs = 0;
                if (RejectMessage(Msg, ErrorText, RejectionsTo, RejectCC)) {
                    return (mserrcode);
                }
                return (0);
            }
        }
    }
    if (!*NumDirs) {
        if (Rejections)
            return (0);
        if (!DefaultBuf[0]) {
            AMS_RETURN_ERRCODE(EMSORPHANMSG, EIN_DIRSPECPARSE, EVIA_CHOOSEDIRECTORIES);
        }
        else {
            strcpy(DirList[0], DefaultBuf);
            bzero(&(CaptionTemplateList[*NumDirs]), sizeof(struct MS_CaptionTemplate));
            if (NamelessCaption) {
                CaptionTemplateList[*NumDirs].basictype = BASICTEMPLATE_NOFROM;
            }
            *NumDirs = 1;
            return (0);
        }
    }
    return (0);
}

static int      AdvanceCmd(NextCmd, CurrCmd)
char          **NextCmd;
char          **CurrCmd;
{
    char           *LastCmd;

    LastCmd = *NextCmd;
    while (LastCmd && *LastCmd && (*LastCmd == ' ' || *LastCmd == '\t')) {
        ++LastCmd;
    }
    while (*NextCmd && **NextCmd && **NextCmd != '\n') {
        ++*NextCmd;
    }
    if (*NextCmd == LastCmd) {
        AMS_RETURN_ERRCODE(EMSPREMATUREEOF, EIN_PARAMCHECK, EVIA_CHOOSEDIRECTORIES);
    }
    *CurrCmd = realloc(*CurrCmd, *NextCmd - LastCmd + 1);
    if (!*CurrCmd) {
        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
    }
    strncpy(*CurrCmd, LastCmd, *NextCmd - LastCmd);
    (*CurrCmd)[*NextCmd - LastCmd] = '\0';
    while (*NextCmd && **NextCmd && (**NextCmd == ' ' || **NextCmd == '\t' || **NextCmd == '\n')) {
        ++*NextCmd;
    }
    return (0);
}

#endif                                 /* 0 */

int             RejectMessage(Msg, Text, RejectionsTo, RejectCC)
struct MS_Message *Msg;
char           *Text, *RejectionsTo, *RejectCC;
{
    FILE           *fp;
    char            RejTo[1500], BBM[500], TempFile[1 + MAXPATHLEN], *Cmd[3], ThisFormat[50], BigBuf[5000];
    int             i, errsave, code, IsBE2Format, SkippedBytes, bytesleft, bytestoread;

    if (Msg->ReplyTo == NULL) {
        BuildReplyField(Msg);
    }
    if (RejectionsTo && *RejectionsTo) {
        if (*RejectionsTo == '/') {
            struct MS_Directory *Dir;
            char           *MyHead, NewFileName[1 + MAXPATHLEN];

            MyHead = malloc(110 + strlen(MyPrettyAddress) + strlen(Text));
            if (!MyHead) {
                AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_CHOOSEDIRECTORIES);
            }
            sprintf(MyHead, "X-Rejection-Message: Message Server for %s rejected this message for the following reason:\n\t%s", MyPrettyAddress, Text);
            AddHeader(Msg, MyHead);
            if (ReadOrFindMSDir(RejectionsTo, &Dir, MD_APPEND)) {
                return (mserrcode);
            }
            if (BuildAttributesField(Msg)
                || BuildDateField(Msg, DATETYPE_FROMHEADER)
                || InventID(Msg)
                || BuildCaption(Msg, NULL, FALSE)) {
                errsave = mserrcode;
                CloseMSDir(Dir, MD_APPEND);
                mserrcode = errsave;
                return (mserrcode);
            }

            if (IsMessageAlreadyThere(Msg, Dir)) {
                CloseMSDir(Dir, MD_APPEND);
                return (0);
            }

            sprintf(NewFileName, "%s/+%s", Dir->UNIXDir, AMS_ID(Msg->Snapshot));
            if (WritePureFile(Msg, NewFileName, FALSE, 0666)) {
                errsave = mserrcode;
                CloseMSDir(Dir, MD_APPEND);
                return (mserrcode = errsave);
            }

            if (AppendMessageToMSDir(Msg, Dir)) {
                errsave = mserrcode;
                CloseMSDir(Dir, MD_APPEND);
                return (mserrcode = errsave);
            }

            if (CloseMSDir(Dir, MD_APPEND)) {
                return (mserrcode);
            }
            return (0);
        }
        Cmd[0] = RejectionsTo;
    }
    else {
        if (Msg->ParsedStuff->HeadBody[HP_RETURNPATH]) {
            strcpy(RejTo, "Message sender ");   /* for obscure RFC822 reasons */
            i = Msg->ParsedStuff->HeadBodyLen[HP_RETURNPATH];
            strncat(RejTo, Msg->ParsedStuff->HeadBody[HP_RETURNPATH], i);
            Cmd[0] = RejTo;
        }
        else {
            Cmd[0] = Msg->ReplyTo;
        }
    }
    if (RejectCC && *RejectCC) {
        Cmd[1] = RejectCC;
    }
    else {
        if (RejectCC) {
            sprintf(BBM, "BBoard.Maintainer@%s", MyMailDomain);
            Cmd[1] = BBM;
        }
        else
            Cmd[1] = NULL;
    }
    Cmd[2] = NULL;
    GenTempName(TempFile);
    if ((fp = fopen(TempFile, "w")) == NULL) {
        AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_REJECTMESSAGE);
    }
    GetFormatFromMessage(Msg, ThisFormat, sizeof(ThisFormat), &IsBE2Format);
    if (IsBE2Format) {
        fprintf(fp, "Content-Type: X-BE2; %s\nIf-Type-Unsupported: alter\n", ThisFormat);
    }
    fprintf(fp, "Date: %sFrom: Message Server for %s\nTo: %s\nSubject: Rejected Message\n", arpadate(), MyPrettyAddress, Cmd[0]);
    if (Cmd[1])
        fprintf(fp, "CC: %s\n\n", Cmd[1]);
    else
        fprintf(fp, "\n");
    if (lseek(Msg->OpenFD, Msg->BodyOffsetInFD, L_SET) < 0) {
        fclose(fp);
        unlink(TempFile);
        AMS_RETURN_ERRCODE(errno, EIN_LSEEK, EVIA_REJECTMESSAGE);
    }
    if (IsBE2Format && (atoi(ThisFormat) >= 10)) {
        EmitBE2PrefixAndLSeekPastIt(Msg->OpenFD, fp, &SkippedBytes);
    }
    else {
        SkippedBytes = 0;
    }
    fprintf(fp, "The following message was delivered properly, but was automatically rejected\nby the recipient's message server for the following reason: \n\n>>>");
    fflush(fp);
    PrintQuotingFormatting(fp, Text, ThisFormat, strlen(Text));
    fflush(fp);
    fprintf(fp, " <<<\n\n-- The rejected message is shown below --\n\n");
    PrintQuotingFormatting(fp, Msg->RawBits, ThisFormat, Msg->HeadSize);
    bytesleft = Msg->FullSize - Msg->HeadSize - SkippedBytes;
    while (bytesleft > 0) {
        bytestoread = (bytesleft > (sizeof(BigBuf) - 1)) ? (sizeof(BigBuf) - 1) : bytesleft;
        if (read(Msg->OpenFD, BigBuf, bytestoread) != bytestoread) {
            fclose(fp);
            unlink(TempFile);
            AMS_RETURN_ERRCODE(errno, EIN_READ, EVIA_REJECTMESSAGE);
        }
        if (fwriteallchars(BigBuf, bytestoread, fp) != bytestoread) {
            fclose(fp);
            unlink(TempFile);
            AMS_RETURN_ERRCODE(errno, EIN_FWRITE, EVIA_REJECTMESSAGE);
        }
        bytesleft -= bytestoread;
    }
    errno = 0;
    if (ferror(fp) || feof(fp)) {
        fclose(fp);
        AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_REJECTMESSAGE);
    }
    if (vfclose(fp) != 0) {
        AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_REJECTMESSAGE);
    }
    code = dropoff(Cmd, TempFile, "<>", home, 0);
    unlink(TempFile);
    code = ConvertDropoffCode(code);
    if (code) {
        AMS_RETURN_ERRCODE(code, EIN_DROPOFF, EVIA_REJECTMESSAGE);
    }
    return (0);
}

static int      ConvertDropoffCode(code)
int             code;
{
    switch (code) {
        case D_OK:
        case D_OK_WARN:
        case D_LOCALQ:
            return (0);
        case D_CANT_QUEUE:
            return (EMSDROPOFFNOQUEUE);
        case D_BAD_PARMS:
            return (EMSDROPOFFBADPARMS);
        case D_TEMP_FAIL:
            return (EMSDROPOFFTEMPFAIL);
        case D_BAD_MESGFILE:
            return (EMSDROPOFFBADMSGFILE);
        case D_OSERR:
            return (EMSDROPOFFOSERR);
        default:
            return (EMSUNKNOWN);
    }
}

int             ResendMessageFromMailbox(Msg, Addressee, AddResendHeads)
struct MS_Message *Msg;
char           *Addressee;
Boolean         AddResendHeads;
{
    FILE           *fp;
    char            TempFile[1 + MAXPATHLEN], *retpath = NULL, *Cmd[2], BigBuf[5000], *authfield;
    int             savepos, code, bytesleft, bytestoread;

    GenTempName(TempFile);
    while (Msg->ParsedStuff->HeadBody[HP_AUTHENTICATED_AS]) {
        DeleteHeader(Msg, HP_AUTHENTICATED_AS);
    }
    while (Msg->ParsedStuff->HeadBody[HP_RECEIVED]) {

        /*
         * Remove all old Received headers - prevent loops and "too many hops"
         */
        DeleteHeader(Msg, HP_RECEIVED);
    }
    if ((fp = fopen(TempFile, "w")) == NULL) {
        AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_RESENDMESSAGE);
    }
    if (AddResendHeads) {
        /* BOGUS: The call to newmid() in the following comes from submsg.c! */
        fprintf(fp, "ReSent-Message-ID: %s\nReSent-Date: %sReSent-From: %s\nReSent-To: %s\n", newmid(), arpadate(), MyPrettyAddress, Addressee);
    }
    if (fwriteallchars(Msg->RawBits, Msg->HeadSize, fp) != Msg->HeadSize) {
        fclose(fp);
        unlink(TempFile);
        AMS_RETURN_ERRCODE(errno, EIN_FWRITE, EVIA_RESENDMESSAGE);
    }
    bytesleft = Msg->FullSize - Msg->HeadSize;
    if (lseek(Msg->OpenFD, Msg->BodyOffsetInFD, L_SET) < 0) {
        fclose(fp);
        unlink(TempFile);
        AMS_RETURN_ERRCODE(errno, EIN_LSEEK, EVIA_RESENDMESSAGE);
    }
    while (bytesleft > 0) {
        bytestoread = (bytesleft > (sizeof(BigBuf) - 1)) ? (sizeof(BigBuf) - 1) : bytesleft;
        if (read(Msg->OpenFD, BigBuf, bytestoread) != bytestoread) {
            fclose(fp);
            unlink(TempFile);
            AMS_RETURN_ERRCODE(errno, EIN_READ, EVIA_RESENDMESSAGE);
        }
        if (fwriteallchars(BigBuf, bytestoread, fp) != bytestoread) {
            fclose(fp);
            unlink(TempFile);
            AMS_RETURN_ERRCODE(errno, EIN_FWRITE, EVIA_RESENDMESSAGE);
        }
        bytesleft -= bytestoread;
    }
    if (vfclose(fp) != 0) {
        unlink(TempFile);
        AMS_RETURN_ERRCODE(errno, EIN_VFCLOSE, EVIA_RESENDMESSAGE);
    }
    if (!AddResendHeads) {
        retpath = Msg->ParsedStuff->HeadBody[HP_RETURNPATH];
        if (retpath) {
            savepos = Msg->ParsedStuff->HeadBodyLen[HP_RETURNPATH];
            retpath[savepos] = '\0';
        }
    }
    Cmd[0] = Addressee;
    Cmd[1] = NULL;
    authfield = GenAuthField(Msg);
    if (!authfield) {
        AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_RESENDMESSAGE);
    }
    code = dropoff_auth(Cmd, TempFile, retpath, home, 0, authfield);
    free(authfield);
    unlink(TempFile);
    code = ConvertDropoffCode(code);
    if (code) {
        AMS_RETURN_ERRCODE(code, EIN_DROPOFF, EVIA_RESENDMESSAGE);
    }
    return (0);
}

#if 0
static int      NotOnList(DirList, Name, NumDirs)
char            DirList[MAXDIRINSERTIONS][MAXPATHLEN + 1];
char           *Name;
int             NumDirs;

{
    int             i;

    for (i = 0; i < NumDirs; ++i) {
        if (!strcmp(DirList[i], Name)) {
            return (FALSE);
        }
    }
    return (TRUE);
}

#endif                                 /* 0 */
