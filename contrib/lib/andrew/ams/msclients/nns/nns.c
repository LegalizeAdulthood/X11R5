/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *nns_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/nns/RCS/nns.c,v 2.9 1991/09/12 15:51:41 bobg Exp $";


/* nns - The Netnews Snarfer
** by Bob Glickstein and the Andrew Message System Group
** This program is a !&@^#% messageserver client.
*/

#include <andrewos.h>
#include <big.h>

#define ALT_GOURMAND_DIR ("/afs/andrew.cmu.edu/usr0/netbb/.MESSAGES/netnews/alt/gourmand")
#define ALT_GOURMAND_STR ("RECIPE:")


/* The following structure holds global program options
*/

Globals_t       Globals;

int             MSDebugging = 0;       /* I shouldn't have to declare these */
unsigned char  *SnapVersionString = NULL;

 /* ...or these... */

BizarreError(text, level)              /* Dummy function from cuinosnap, which
                                        * libmessageserver needs! */
char           *text;
int             level;
{
    fprintf(stderr, "<nns:warning>%s (%d)\n", text, level);
}

Machine_HandleClientSignal(signum, ActNormal)   /* Ditto */
int             signum, *ActNormal;
{
    printf("Machine_HandleClientSignal was called (signum=%d)\n", signum);
}

main(argc, argv, envp)
int             argc;
char          **argv, **envp;
{
    int             opt, status, dummy;
    GList_t         GList;
    extern int      optind;
    extern char    *optarg;

    Globals.Options.verbose = Globals.Options.runOnce = Globals.Options.dontDo = Globals.Options.sortByTime = FALSE;
    Globals.Options.mailboxDir = DEFAULTMAILBOX;
    Globals.Options.NetnewsRoot = NETNEWSROOT;
    Globals.Options.ActiveGroups = NULL;
    while ((opt = getopt(argc, argv, "1tnvd:r:g:")) != EOF) {
	switch (opt) {
	    case 'd':
		if (optarg[strlen(optarg) - 1] != '/') {
		    fputs("Warning: because of programmer stupidity, you must include a trailing slash after directory names\n", stderr);
		    exit(TSLASH);
		}
		Globals.Options.mailboxDir = optarg;
		Verbiage(5, "Set mailbox directory to:");
		Verbiage(5, optarg);
		break;
	    case 'r':
		if (optarg[strlen(optarg) - 1] != '/') {
		    fputs("Warning: because of programmer stupidity, you must include a trailing slash after directory names\n", stderr);
		    exit(TSLASH);
		}
		Globals.Options.NetnewsRoot = optarg;
		Verbiage(5, "Set Netnews root to:");
		Verbiage(5, optarg);
		break;
	    case 'g':
		Globals.Options.ActiveGroups = optarg;
		Verbiage(5, "Set list of active newsgroups to:");
		Verbiage(5, optarg);
		break;
	    case 't':
		Globals.Options.sortByTime = TRUE;
		Verbiage(5, "Sorting by time");
		break;
	    case 'v':
		++(Globals.Options.verbose);    /* Many -v's means very
					 * verbose */
		break;
	    case '1':
		Globals.Options.runOnce = TRUE;
		Verbiage(5, "Running only once");
		break;
	    case 'n':
		Globals.Options.dontDo = TRUE;
		Verbiage(5, "The don't-do flag was used, but is not yet implemented!");
		exit(NOTIMPLEMENTED);
		break;
	    default:
		fprintf(stderr, "Usage: %s [-1tnv] [-d <mailbox>/] [-r <Netnews root>/] [-g <existing newsgroups>]\n", argv[0]);
		exit(STUPIDUSER);
		break;
	}
    }
    GLInit(&GList);
    Verbiage(4, "Initialized Grouplist");
    STInit(&(Globals.STable));
    Verbiage(4, "Initialized String Table");
    MS_Initialize(&dummy, FALSE);
    Verbiage(4, "Initialized Messageserver");
    if (Globals.Options.runOnce) {
	if ((status = GLBuildList(&GList, Globals.Options.mailboxDir)) > 0) {
	    Verbiage(2, "Built the grouplist; about to process...");
	    ProcessList(&GList);
	    Verbiage(2, "Processed the grouplist");
	    /*          GLPurge(&GList); */
	    /*          STPurge(&(Globals.STable)); About to exit--don't need to clean up.  */
	}
	else {
	    if (status) {
		switch (status) {
		    case CANT_OPEN_DIR:
			fprintf(stderr, "%s: Fatal error!  Couldn't open directory %s\n", argv[0], Globals.Options.mailboxDir);
			exit(DIRPROBLEMS);
			break;
		    default:
			fprintf(stderr, "%s: Fatal error!  Unknown error in GLBuildList\n", argv[0]);
			exit(UNKNOWN);
			break;
		}
	    }
	}
	MS_UpdateState();
	exit(0);
    }
    else {
	while (TRUE) {
	    if ((status = GLBuildList(&GList, Globals.Options.mailboxDir)) > 0) {
		Verbiage(2, "Built the grouplist; about to process...");
		ProcessList(&GList);
		Verbiage(2, "Processed the grouplist");
		GLPurge(&GList);
	    }
	    else {
		if (status) {
		    switch (status) {
			case CANT_OPEN_DIR:
			    fprintf(stderr, "%s: Fatal error!  Couldn't open directory %s\n", argv[0], Globals.Options.mailboxDir);
			    exit(DIRPROBLEMS);
			    break;
			default:
			    fprintf(stderr, "%s: Fatal error!  Unknown error in GLBuildList\n", argv[0]);
			    exit(UNKNOWN);
			    break;
		    }
		}
		else {
		    Verbiage(1, "Going to sleep");
		    sleep(RETRY_INTERVAL);
		}
	    }
	}
    }
}

void            ProcessList(gl)
GList_t        *gl;
{
    GListEntry_t   *gle;
    int             i, numUnlinks, numPosts, numGroups;
    struct MS_Directory Dir;
    char            currentNewsgroup[NEWSGROUPNAMELEN + 1], actualDir[MAXPATHLEN + 1], *gleFilename, *gleNewsgroup, *subdirPtr, actualFile[MAXPATHLEN + 1], *filePtr;
    MCache_t        MCache;

    Verbiage(3, "In ProcessList");
    numUnlinks = numPosts = numGroups = 0;
    bzero(&Dir, sizeof(struct MS_Directory));
    Dir.fd = -1;
    Dir.LastIDHit = -1;
    Dir.OpenMode = -1;
    Dir.AttrNames = NULL;
    Dir.UNIXDir = NULL;
    Dir.IDs = NULL;
    Dir.NumIDs = 0;
    currentNewsgroup[0] = '\0';
    strcpy(actualDir, Globals.Options.NetnewsRoot);
    subdirPtr = actualDir + strlen(Globals.Options.NetnewsRoot);
    strcpy(actualFile, Globals.Options.mailboxDir);
    filePtr = actualFile + strlen(Globals.Options.mailboxDir);
    MCInit(&MCache);
    Verbiage(4, "Initialized Message Cache");
    for (i = 0; i < GLGetNum(gl); ++i) {
	gle = GLGetEntry(gl, i);
	if (!GLEGetIgnore(gle)) {
	    gleFilename = GLEGetFilename(gle);
	    Verbiage(1, "Processing file:");
	    Verbiage(1, gleFilename);
	    gleNewsgroup = GLEGetNewsgroup(gle);
	    Verbiage(1, "For newsgroup:");
	    Verbiage(1, gleNewsgroup);
	    if (*currentNewsgroup) {
		if (strcmp(currentNewsgroup, gleNewsgroup)) {
		    CloseMSDir(&Dir, MD_APPEND);
		    Verbiage(2, "Closed newsgroup:");
		    Verbiage(2, currentNewsgroup);
		    strcpy(subdirPtr, gleNewsgroup);
		    DotsToSlashesInPlace(subdirPtr);
		    bzero(&Dir, sizeof(struct MS_Directory));
		    Dir.UNIXDir = actualDir;
		    Dir.fd = -1;       /* This is BOGUS, BOGUS, BOGUS */
		    Dir.LastIDHit = -1;
		    Dir.OpenMode = -1;
		    Dir.AttrNames = NULL;
		    Dir.AttrCount = -1;
		    if (OpenMSDirectory(&Dir, MD_APPEND)) {
			Verbiage(2, "Open of newsgroup failed; ignoring this file");
			GLIgnore(gl, gleFilename);
			currentNewsgroup[0] = '\0';
		    }
		    else {
			strcpy(currentNewsgroup, gleNewsgroup);
			if (ReadOldMSDirectoryHead(&Dir)) {
			    Verbiage(2, "Read of MS directory head failed; ignoring file");
			    CloseMSDir(&Dir, MD_APPEND);
			    GLIgnore(gl, gleFilename);
			    currentNewsgroup[0] = '\0';
			}
			else {
			    Verbiage(3, "Opened newsgroup");
			    ++numGroups;
			}
		    }
		}
	    }
	    else {
		strcpy(subdirPtr, gleNewsgroup);
		DotsToSlashesInPlace(subdirPtr);
		Dir.UNIXDir = actualDir;
		Dir.fd = -1;           /* This is BOGUS, BOGUS, BOGUS */
		Dir.LastIDHit = -1;
		Dir.OpenMode = -1;
		Dir.AttrNames = NULL;
		Dir.AttrCount = -1;
		if (OpenMSDirectory(&Dir, MD_APPEND)) {
		    Verbiage(2, "Open of newsgroup failed; ignoring this file");
		    GLIgnore(gl, gleFilename);
		    currentNewsgroup[0] = '\0';
		}
		else {
		    strcpy(currentNewsgroup, gleNewsgroup);
		    if (ReadOldMSDirectoryHead(&Dir)) {
			Verbiage(2, "Read of MS directory head failed; ignoring file");
			CloseMSDir(&Dir, MD_APPEND);
			GLIgnore(gl, gleFilename);
			currentNewsgroup[0] = '\0';
		    }
		    else {
			Verbiage(3, "Opened newsgroup");
			++numGroups;
		    }
		}
	    }
	    if (*currentNewsgroup) {   /* Was it a successful OpenMSDirectory? */
		strcpy(filePtr, gleFilename);
		if (!ProcessEntry(gle, actualFile, &Dir, &MCache, &numUnlinks)) {
		    Verbiage(2, "ProcessEntry failed; ignoring file");
		    GLIgnore(gl, gleFilename);
		}
		else
		    ++numPosts;
	    }
	}
    }
    if (*currentNewsgroup)
	CloseMSDir(&Dir, MD_APPEND);
    MCPurge(&MCache);                  /* If I write this code right, this
					 * should be a no-op */
    if (numUnlinks != 0)
	printf("# %d /usr/net\n", numUnlinks);  /* the only thing going to
						 * stdout. */
    if (numUnlinks != 0 || numPosts != 0 || numGroups != 0) {
	printf("(Processed %d files from dir %s to be %d postings in %d newsgroups.)\n",
	       numUnlinks, Globals.Options.mailboxDir, numPosts, numGroups);
    }
}


/* Returns success/failure */

int             ProcessEntry(gle, fullFilename, Dir, mc, UnlinkP)
GListEntry_t   *gle;
char           *fullFilename;
struct MS_Directory *Dir;
MCache_t       *mc;
int            *UnlinkP;
{
    struct MS_Message *Msg;
    int             shouldWrite, inCache;
    struct MS_CaptionTemplate CaptionTemplate;
    char            NewFileName[MAXPATHLEN + 1];

    Verbiage(3, "In ProcessEntry");
    if (ShouldAlreadyBeCached(gle)) {
	Verbiage(2, "Message should be cached...");
	if (Msg = MCFind(mc, GLEGetFilename(gle))) {
	    Verbiage(2, "...and was");
	    inCache = TRUE;
	}
	else {
	    Verbiage(2, "...but wasn't");
	    inCache = FALSE;
	    if (!(Msg = (struct MS_Message *) malloc(sizeof(struct MS_Message))))
		return (FALSE);
	    bzero(Msg, sizeof(struct MS_Message));
	    CaptionTemplate.datetype = DATETYPE_FROMHEADER;
	    CaptionTemplate.basictype = BASICTEMPLATE_NORMAL;
	    if (ReadRawFile(fullFilename, Msg, TRUE)
		|| ParseMessageFromRawBody(Msg)
		|| AddNetnewsWideReplyHeader(Msg)
		|| (Msg->AuthUid = 0, Msg->AuthName = Msg->AuthCell = NULL, FALSE)
		|| InventID(Msg)
		|| BuildDateField(Msg)
		|| BuildReplyField(Msg)
		|| BuildAttributesField(Msg)
		|| BuildCaption(Msg, &CaptionTemplate, FALSE)) {
		Verbiage(1, "Something failed building the message structure");
		FreeMessage(Msg, TRUE);
		return (FALSE);
	    }
	}
    }
    else {
	inCache = FALSE;
	if (!(Msg = (struct MS_Message *) malloc(sizeof(struct MS_Message))))
	    return (FALSE);
	bzero(Msg, sizeof(struct MS_Message));
	CaptionTemplate.datetype = DATETYPE_FROMHEADER;
	CaptionTemplate.basictype = BASICTEMPLATE_NORMAL;
	if (ReadRawFile(fullFilename, Msg, TRUE)
	    || ParseMessageFromRawBody(Msg)
	    || AddNetnewsWideReplyHeader(Msg)
	    || (Msg->AuthUid = 0, Msg->AuthName = Msg->AuthCell = NULL, FALSE)
	    || InventID(Msg)
	    || BuildDateField(Msg)
	    || BuildReplyField(Msg)
	    || BuildAttributesField(Msg)
	    || BuildCaption(Msg, &CaptionTemplate, FALSE)) {
	    Verbiage(1, "Something failed building the message structure");
	    FreeMessage(Msg, TRUE);
	    return (FALSE);
	}
    }

    /*
      * Now we have a parsed message structure, whose file may or may not be
      * open, and which may or may not have just been created (alternative is
								* that it came from the cache)
      */

    if (Msg->OpenFD < 0) {             /* It needs to be opened */
	if ((Msg->OpenFD = open(fullFilename, O_RDONLY, 0)) < 0) {
	    Verbiage(2, "Couldn't reopen cached message; ignoring file.");
	    if (inCache) {
		MCDelete(mc, GLEGetFilename(gle));      /* This is cool, since
							 * GLIgnore will be
							 * called */
		Verbiage(2, "Message removed from cache");
	    }
	    FreeMessage(Msg, TRUE);
	    return (FALSE);
	}
	Verbiage(3, "Reopened cached message");
    }

    if (ShouldBeCached(gle)) {
	Verbiage(2, "Message belongs in cache");
	if (!inCache) {
	    Verbiage(2, "Trying to place into cache");
	    if (inCache = MCMake(mc, GLEGetFilename(gle), Msg))
		Verbiage(2, "Successfully placed in cache");
	    else
		Verbiage(2, "Caching failed");
	}
    }
    else {
	if (inCache) {
	    MCDelete(mc, GLEGetFilename(gle));
	    Verbiage(2, "Message removed from cache");
	    inCache = FALSE;
	}
    }

    shouldWrite = TRUE;

    /* Now for the alt.gourmand hack */
    if ((!strcmp(Dir->UNIXDir, ALT_GOURMAND_DIR))
	 && (!strncmp(Msg->ParsedStuff->HeadBody[HP_SUBJECT], ALT_GOURMAND_STR, strlen(ALT_GOURMAND_STR)))) {
	if (AddHeader(Msg,
		      "Content-Type: troff; 0; /usr/lib/tmac/tmac.an,/usr/lib/tmac/tmac.recipe")) {
	    if (inCache) {
		MCDelete(mc, GLEGetFilename(gle));
	    }
	    FreeMessage(Msg, TRUE);
	    return (FALSE);
	}
    }

    if (IsMessageAlreadyThere(Msg, Dir)) {
	Verbiage(1, "Duplicate message won't be added");
	shouldWrite = FALSE;
    }

    if (shouldWrite) {
	sprintf(NewFileName, "%s/+%s", Dir->UNIXDir, AMS_ID(Msg->Snapshot));

	Verbiage(1, "Mailbox file is going to database as file:");
	Verbiage(1, NewFileName);

	/*
	 * Note -- we used to try to rename the file, but this never works
	 * from a local disk into Vice
	 */

	if (WritePureFile(Msg, NewFileName, FALSE, 0644)) {
	    printf("nns (0): WritePureFile failed on Mailbox file %s\n     destined for %s,\n      with Msg->OpenFD of %d (ms: %d, %d, %d)\n", GLEGetFilename(gle), NewFileName, Msg->OpenFD, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
	    if (inCache) {
		MCDelete(mc, GLEGetFilename(gle));      /* This is cool, since
							 * GLIgnore will be
							 * called */
		Verbiage(2, "Message removed from cache");
	    }
	    FreeMessage(Msg, TRUE);
	    return (FALSE);
	}
	else {
	    Verbiage(2, "Wrote the file; adding to msgdir...");
	    if (AppendMessageToMSDir(Msg, Dir)) {
		Verbiage(1, "AppendMessageToMSDir failed; unlinking body...");
		unlink(NewFileName);
		if (inCache) {
		    MCDelete(mc, GLEGetFilename(gle));  /* This is cool, since
							 * GLIgnore will be
							 * called */
		    Verbiage(2, "Removed message from cache");
		}
		FreeMessage(Msg, TRUE);
		return (FALSE);
	    }
	}
    }


    if (Msg->OpenFD >= 0) {
	Verbiage(1, "Msg->OpenFD is open");
	close(Msg->OpenFD);            /* BOGUS: Return value?  Use vclose? */
	Verbiage(3, "Now it's closed");
	Msg->OpenFD = -1;
    }

    if (ShouldUnlink(gle)) {
	Verbiage(1, "Unlinking the file");
	if (unlink(fullFilename) == 0)
	    ++(*UnlinkP);              /* Ignore errors except for statistics. */
    }
    else
	Verbiage(2, "Not unlinking the file yet");

    if (!inCache)
	FreeMessage(Msg, TRUE);
    return (TRUE);
}

void            DotsToSlashesInPlace(string)
char           *string;
{
    char           *p = string, *index();

    while (p = index(p, '.'))
	*(p++) = '/';
}

int             ShouldAlreadyBeCached(gle)
GListEntry_t   *gle;
{
    return (GLEGetBefore(gle) > 0);
}

int             ShouldBeCached(gle)
GListEntry_t   *gle;
{
    return (GLEGetAhead(gle) > 0);
}

int             ShouldRename(gle)
GListEntry_t   *gle;
{
    return (!GLEGetAhead(gle));
}

int             ShouldUnlink(gle)
GListEntry_t   *gle;
{
    return (!(GLEGetAhead(gle) || GLEGetIgnore(gle)));
}

void            Verbiage(level, string)
int             level;
char           *string;
{
    if (level <= Globals.Options.verbose)
	fprintf(stderr, "nns (%d): %s (ms: %d, %d, %d)\n", level, string, AMS_ERRNO, AMS_ERRCAUSE, AMS_ERRVIA);
}

int             AddNetnewsWideReplyHeader(Msg)
struct MS_Message *Msg;
{
    char            header[GROUPSLINESIZE], *gptr, tmpchar, tmpchar2, *endptr, *end2, *index();
    int             first = TRUE;

    sprintf(header, "X-Andrew-WideReply: ");
    if (gptr = Msg->ParsedStuff->HeadBody[HP_NEWSGROUPS]) {
	endptr = gptr + Msg->ParsedStuff->HeadBodyLen[HP_NEWSGROUPS];
	tmpchar = *endptr;
	*endptr = '\0';
	while (gptr < endptr) {
	    if (!(end2 = index(gptr, ',')))
		end2 = endptr;
	    tmpchar2 = *end2;
	    *end2 = '\0';
	    if (first)
		first = FALSE;
	    else
		strcat(header, ",");
	    strcat(header, "netnews.");
	    strcat(header, gptr);
	    *end2 = tmpchar2;
	    gptr = end2 + 1;
	}
	*endptr = tmpchar;
	if (first) {
	    Verbiage(1, "Empty newsgroups header; can't build widereply field");
	    return (0);
	}
	Verbiage(3, "Adding following widereply header:");
	Verbiage(3, header);
	return (AddHeader(Msg, header));
    }
    Verbiage(1, "No newsgroups header at all!");
    return (0);
}
