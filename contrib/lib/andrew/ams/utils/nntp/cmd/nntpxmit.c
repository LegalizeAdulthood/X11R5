/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *nntpxmit_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/utils/nntp/cmd/RCS/nntpxmit.c,v 1.15 1991/09/12 15:53:59 bobg Exp $";

/*
** nntpxmit - transmit netnews articles across the internet with nntp
**
** This program is for transmitting netnews between sites that offer the
** NNTP service, internet style.  Ideally, there are two forms of
** transmission that can be used in this environment, since the
** communication is interactive (and relatively immediate, when compared
** with UUCP).  They are:  passive poll (what have you gotten lately?) and
** active send (I have `x', do you want it?).  The USENET as a whole
** uniformly uses active send, and where the communication is batched
** (e.g. UUCP, or electronic mail) the software sends without even asking,
** unless it can determine that the article has already been to some site.
**
** It turns out that when you implement passive poll, you have to be
** *very* careful about what you (the server) tell the client, because
** something that you might wish to restrict distribution of (either
** internal newsgroups, or material posted in the international groups in
** local distributions) will otherwise leak out.  It is the case that if
** the server doesn't tell the client that article `x' is there, the
** client won't ask for it.  If the server tells about an article which
** would otherwise stay within some restricted distribution, the onus is
** then on the client to figure out that the article is not appropriate to
** post on its local system.  Of course, at that point, we have already
** wasted the network bandwidth in transferring the article...
**
** This is a roundabout way of saying that this program only implements
** active send.  There will have to be once-over done on the NNTP spec
** before passive poll goes in, because of the problems that I have cited.
**
** Erik E. Fair	<ucbvax!fair>, Oct 14, 1985
*/
#include <stdio.h>
#include <system.h>
#include <andrewos.h> /* sys/types.h sys/file.h strings.h syslog.h */
#include <ctype.h>
#include <sys/dir.h>
#ifdef HAS_SYSEXITS
#include <sysexits.h>
#endif /* HAS_SYSEXITS */
#include <errprntf.h>
#include <sys/param.h>
#include <util.h>
#include <mailconf.h>
#include <defs.h>
#include <header.h>
#include <respcode.h>
#include <nntpxmit.h>

extern	int	errno;
extern	char	*errmsg();
char    *progname = "nntpxmit";
char	*USAGE = "USAGE: nntpxmit file [user [orgname]]";
char	retmsg[LBUFLEN];
FILE	*getfp();
#define ERR_BADART  510
int test = 0;   /* Turn on to keep articles from actually being posted*/
int	GotPath, GotOrg;

main(ac, av)
int	ac;
char	*av[];
{
	register int	i;
	int status;
	char *file, *user, *orgname;

	if (ac < 2) {
		strcpy(retmsg, USAGE);
		fprintf(stdout, "%s\n", USAGE);
		exit(EX_USAGE);
	}
	errno = 0;
	if (CheckAMSConfiguration() != 0) {
	    sprintf(retmsg, "Configuration failed: %s", UnixError(errno));
	    printf("%s\n", retmsg);
	    exit(EX_TEMPFAIL);
	}

	file = av[1];
	user = orgname = NULL;
	if (ac > 2) user = av[2];
	if (ac > 3) orgname = av[3];

	for(i = 4; i < ac; i++) {
		if (av[i][0] == '-') {
			switch(av[i][1]) {
			case 't':
				fprintf(stderr, "%s: testing only.\n", progname);
				test = 1;
				break;
			default:
				sprintf(retmsg, "No such option: -%c (%d)", av[i][1], EX_USAGE);
				fprintf(stdout, "%s\n", retmsg);
				exit(EX_USAGE);
			}
			continue;
		}
	}

	status = sendnews(NNTPhost, file, user, orgname);
	fprintf(stdout, "%s\n", retmsg);
	exit(status);

}

/*
** Given a hostname to connect to, and a filename (which contains a
** netnews article), send those article to the named host using NNTP.
** ORGANIZATION: 
headers are added also.
*/
static sendnews(host, file, user, orgname)
char	*host, *file, *user, *orgname;
{
	register int	code;
	register FILE	*fp;
	char    tmpmsg[BUFSIZ], buf[BUFSIZ], file2[MAXPATHLEN];
	int status = EX_OK;
	/*
	** if no news to send, return
	*/

	if ((status = hello(host)) != EX_OK)
		return(status);
	if (!(fp = fopen(file, "r"))) {
	    if (vdown(errno)) {
		sprintf(retmsg, "AFS is apparently down for %s (%d)",
		    file, errno);
		return(EX_TEMPFAIL);
	    }
	    sprintf(retmsg, "Cannot open article file %s for reading", file);
	    return(EX_NOINPUT);
	}

	code = ihave(fp);
	if (!(GotPath && GotOrg))
		add_headers(fp, user, orgname, file, file2);
#ifdef DEBUG
	{
	char b[100];
	printf("Message:\n");
	sprintf(b, "cat %s", file2);
	printf("End of Message\n");
	system(b);
	}
#endif /* DEBUG */
	switch(code) {
		case CONT_XFER:
			if ((status = sendfile(fp)) != EX_OK) {
				sprintf(tmpmsg,
					"Transmission of article failed (%d)",
					status);
				strcat(retmsg, tmpmsg);
				fclose(fp);
				goodbye(DONT_WAIT);
				if (!test) unlink(file2);
				return(status);
			}
			fclose(fp);

			code = readreply(buf, sizeof(buf));
			if (code != OK_XFERED) { 
			    if (code == ERR_XFERFAIL)  {
				sprintf(retmsg,
				   "Temporarily unable to transmit article: %s",
					buf);
				status = EX_TEMPFAIL;
			    }
			    else {
				sprintf(retmsg,
					"Article not transferred (%d): %s",
					code, buf);
				if (code >= 100 && code < 400)
				    status = EX_OK;
				if ((code>=400 && code<500) || code == FAIL) 
				    status = EX_TEMPFAIL;
				else 
				    status = EX_UNAVAILABLE;
			    }
	    
			}
			else sprintf(retmsg,
				     "Article transferred successfully");
			break;
		case ERR_XFERFAIL: 
			fclose(fp);
			status = EX_TEMPFAIL;
			break;
		case ERR_XFERRJCT: 
			fclose(fp);
			status = EX_UNAVAILABLE;
			break;
		case ERR_GOTIT: 
			fclose(fp);
			status = EX_OK;
			break;
		case ERR_BADART:
			fclose(fp);
			status = EX_DATAERR;
			break;
		default:
			fclose(fp);
			if (code >= 100 && code < 400)
			    status = EX_OK;
			if (code >= 400 && code < 500)
			    status = EX_TEMPFAIL;
			else 
			    status = EX_UNAVAILABLE;
			break;


	}
	if ((code != ERR_GOTIT) && (status == EX_OK)) 
	    sprintf(retmsg, "Article transferred sucessfully");
	unlink(file2);
	goodbye(WAIT);
	return(status);
}

/*
** Read the header of a netnews article, snatch the message-id therefrom,
** and ask the remote if they have that one already. If an error occurs
** an explanatory messages will be put in Retmsg.  
*/
static ihave(fp)
FILE	*fp;
{
	struct hbuf	header;
	char	scr[LBUFLEN];

	bzero(&header, sizeof(header));
	if (rfc822read(&header, fp, scr) == EX_OK) {
		register int	code;
		char	buf[BUFSIZ];

		/*
		** If an article shows up without a message-id,
		** we scream bloody murder. That's one in
		** the `can't ever happen' category.
		*/
		if (header.ident[0] == '\0') {
			sprintf(retmsg, "This article has no Message-ID: field.");
			return(ERR_BADART);
		}
/* a little error checking on the Message-ID field */

		if (header.ident[0] != '<') {
		    sprintf(retmsg, "Missing < at beginning of Message-ID: field.");
		    return(ERR_BADART);
		}
		if (header.ident[strlen(header.ident) -1] != '>') {
		    sprintf(retmsg, "Missing < at end of Message-ID: field.");
		    return(ERR_BADART);
		}
		if (!index(header.ident, '@')) {
		    sprintf(retmsg, "Missing @ in Message-ID: field.");
		    return(ERR_BADART);
		}

		sprintf(buf, "IHAVE %s", header.ident);
#ifdef DEBUG
		fprintf(stderr, ">>>%s\n",buf);
#endif /* DEBUG */
		code = converse(buf, sizeof(buf));

		switch(code) {
		case CONT_XFER:
			rewind(fp);
			return(code);
		case ERR_GOTIT:
			sprintf(retmsg,"Server already has this article; this copy of it not sent: %d",code);
			return(code);
		case ERR_XFERFAIL:
			sprintf(retmsg,"Article transfer failed: %s.",buf);
			return(code);
		case ERR_XFERRJCT:
			sprintf(retmsg,"The NNTP receiver on pt.cs.cmu.edu rejected this article with: %s.",buf);
			return(code);
		case ERR_CMDSYN:
			sprintf(retmsg,"The NNTP receiver on pt.cs.cmu.edu rejected this article after our IHAVE command with: %s.  (Perhaps the Message-ID: field has a syntax error.)",buf);
			return(code);
		case ERR_COMMAND:
			sprintf(retmsg,"The NNTP receiver on pt.cs.cmu.edu found a syntax error in our IHAVE command and rejected this article with: %s.",buf);
			return(code);
		default:
			sprintf(retmsg,"The NNTP receiver on pt.cs.cmu.edu rejected this article with: %s.",buf);
			return(code);

		}

	}
	return(ERR_BADART);

}
/* if not already present, add PATH and ORGANIZATION headers
*/
static add_headers(fp, user, orgname, file, file2)
FILE *fp;
char *user, *orgname, *file, *file2;
{
	FILE	*wfp;
	char	abuf[BUFSIZ];


	sprintf(file2,"/tmp/temp.%d",getpid());
	if (!(wfp = fopen(file2,"w"))) {
	    if (vdown(errno)) {
		sprintf(retmsg,"AFS is apparently down for temp file %s (%d)",
		    file2,errno);
		return(EX_TEMPFAIL);
	    }
	    sprintf(retmsg,"Cannot open temp file %s for writing",file2);
	    return(EX_CANTCREAT);
	}

	if (!GotPath) fprintf(wfp, "Path: %s!%s\n", ThisDomain, (user == NULL || user[0] == '\0' ? NNTPuser : user));
	if (!GotOrg) {
	    fputs("Organization: ", wfp);
	    if (orgname != NULL && orgname[0] != '\0' && isupper(orgname[0])) {
		fputs(orgname, wfp);
	    } else {
		fputs(Organization, wfp);
	    }
	    fputc('\n', wfp);
	}
	while (fgets(abuf,sizeof(abuf),fp))
		fputs(abuf,wfp);
	fclose(wfp);
	fclose(fp);
	if (!(fp = fopen(file2,"r"))) {
	    if (vdown(errno)) {
		    sprintf(retmsg,"AFS is apparently down for %s (%d)",
			file2,errno);
		    unlink(file2);
		    return(EX_TEMPFAIL);
	    }
	    unlink(file2);
	    sprintf(retmsg,"Cannot open %s for reading",file2);
	    return(EX_NOINPUT);
	}
}
