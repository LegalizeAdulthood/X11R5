/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *sendmail_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/delivery/sendmail/RCS/sendmail.c,v 1.10 1991/09/12 15:39:34 bobg Exp $";

/*
		sendmail.c -- Replacement for sendmail on workstations.
*/


#include <pwd.h>
#include <stdio.h>
#include <system.h>
#include <andrewos.h>
#ifdef HAS_SYSEXITS
#include <sysexits.h>
#endif /* HAS_SYSEXITS */

#include <util.h>
#include <mailconf.h>

typedef unsigned char bool;
#define FALSE	0
#define TRUE	1

#define NIL	0

bool readto, ignoredots;

main(argc, argv)
    int argc;
    char *argv[];
{
    register int i, nrecps;

    nrecps = 0;
    ignoredots = FALSE;

    CheckAMSConfiguration();
    /* Process arguments */
    for (i=1; argv[i] != NIL; i++) {
	register char *arg;

	arg = argv[i];
	switch (*arg) {

	    case '-':	switch (*++arg) {
			    case 'v':	fputs("[SENDMAIL] Warning: -v is no longer supported\n", stderr);
					break;
			    case 't':	unsupp(argv, argv[i]);
			    case 'b':	if (*++arg == 'm') break;
					unsupp(argv, argv[i]);
			    case 'o':	if (*++arg == 'i')
					    ignoredots = TRUE;
					else
					    unsupp(argv, argv[i]);
			    case 'i':
			    case 'm':
			    case 's':	break;
			    default:	unsupp(argv, argv[i]);
			}
			break;

	    default:	nrecps++;
			break;
	}
    }

    if (nrecps == 0) unsupp(argv, "No recipient addresses specified");

    /* It's okay, just deliver the mail */
    exit(deliver(argv, nrecps));
}

static deliver(argv, nrecps)
    char *argv[];
    int nrecps;
{
    char **qargs;
    register FILE *qmail;
    register bool lookfordot;
    register int i, q;

    /* Get space for args */
	/* Allow space for arg[0], -i & NIL */
    qargs = (char **) calloc(sizeof(char *), nrecps+4);
    if (qargs == NIL) {
	fputs("[SENDMAIL] Out of memory, delivery aborted\n", stderr);
	postmaster(argv, "Out of memory in deliver");
	exit(EX_OSERR);
    }

    /* Construct command -- add list of recipients */
    qargs[0] = "queuemail";
    qargs[1] = "-i";
    qargs[2] = "-a";
    for (i=1, q=3; argv[i]!=NIL; i++)
	if (*argv[i] != '-')	/* Must be a recipient */
	    qargs[q++] = argv[i];

    /* Null-terminate it */
    qargs[q] = NIL;

    /* Fork queuemail & send it message */
    qmail = (FILE *) qopen(queuemail, qargs, "w");
    if (qmail == NIL) {
	fputs("[SENDMAIL] Can't invoke queuemail\n", stderr);
	exit(EX_UNAVAILABLE);
    }

    lookfordot = FALSE;
    for (;;) {
	register int c;

	c = getchar();
	switch (c) {
	    case '\n':	lookfordot = TRUE;
			break;
	    case EOF:	return qclose(qmail);
	    case '.':	if (!ignoredots && lookfordot) {
			    c = getchar();
			    if (c == '\n')
				return qclose(qmail);
			    fputc('.', qmail);
			}
	    default:	lookfordot = FALSE;
	}
	fputc(c, qmail);
    }
}

static unsupp(argv, problem)
    char *argv[], *problem;
{
    fputs("[SENDMAIL] /usr/lib/sendmail was invoked with an unimplemented\n", stderr);
    fprintf(stderr, "\tor improper option: %s\n", problem);
    fputs("Postmaster is being made aware of this problem...", stderr);
    fflush(stderr);

    postmaster(argv, problem);
    exit(EX_USAGE);
}

static char *host()
{
#define NAMELEN 256
    static char name[NAMELEN];

    if (GetHostDomainName(name, NAMELEN) < 0)
	strcpy(name, "UNKNOWN");
    else
	name[NAMELEN-1] = '\0';
    return name;
}

static char *user()
{
    struct passwd *pw;

    pw = getpwuid(getuid());
    if (pw == NULL)
	return "NO ENTRY";
    else
	return pw -> pw_name;
}

static postmaster(argv, problem)
    char *argv[], *problem;
{
    static char *args[] = { "queuemail", "-i", NIL, NIL }; 
    register FILE *fout;
    register int i;

    args[2] = PostmasterTitle;
    fout = (FILE *) qopen(queuemail, args, "w");
    if (fout == NULL)
	return fputs("WARNING: can't send mail to postmaster\n", stderr);

    /* Header */
    fprintf(fout, "Date: %s", arpadate());
    fprintf(fout, "From: /usr/lib/sendmail <%s%s@%s>\n", PostmasterName,
		(AMS_DeliverySystem ? "+" : ""), WorkstationCell);
    fprintf(fout, "Subject: /usr/lib/sendmail invoked (%s)\n\n", problem);

    /* Body of message */
    fprintf(fout, "/usr/lib/sendmail was invoked on host %s at %s\n", host(), arpadate());
    fprintf(fout, "User %s, WS cell %s\n", user(), WorkstationCell);
    fprintf(fout, "Problem: %s\n", problem);
    fprintf(fout, "\nCommand: %s", argv[0]);
    for (i=1; argv[i]!=NULL; i++)
	fprintf(fout, " %s", argv[i]);
    fputc('\n', fout);

    qclose(fout);
    fputs("done\n", stderr);
}
