/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
static char *mail_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/delivery/mail/RCS/mail.c,v 1.8 1991/09/12 15:24:41 bobg Exp $";


/*
		mail.c -- Warn user & postmaster that /bin/mail is no longer supported.
*/


#include <stdio.h>
#include <pwd.h>

main(argc, argv)
{
    /* 1st warn user -- no longer supported */
    fputs("WARNING: /bin/mail is no longer supported --\n\t", stderr);
    fputs("if you need assistance, please\n\tsend mail to `advisor'.  Notifying\n\tpostmaster...", stderr);
    fflush(stderr);

    /* Send message to postmaster */
    postmaster(argv);
    fputs("sent\n", stderr);
    exit(1);
}

static char *date()
{
    extern long time();
    extern char *ctime();
    long clock;

    clock = time(0);
    return ctime(&clock);
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

static postmaster(argv)
    char *argv[];
{
    static char sendmail[] = "/usr/lib/sendmail -oi postmaster";
    extern FILE *popen();
    extern pclose();
    register FILE *fout;
    register int i;

    fout = popen(sendmail, "w");
    if (fout == NULL)
	return fputs("WARNING: can't send mail to postmaster\n", stderr);

    /* Header */
    fputs("Subject: /bin/mail invoked\n\n", fout);

    /* Body of message */
    fprintf(fout, "/bin/mail was invoked on host %s on %s\n", host(), date());
    fprintf(fout, "User was %s\n", user());
    fprintf(fout, "\nCommand: %s", argv[0]);
    for (i=1; argv[i]!=NULL; i++)
	fprintf(fout, " %s", argv[i]);
    fputc('\n', fout);

    pclose(fout);
}
