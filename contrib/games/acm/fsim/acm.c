/*
 *	acm : an aerial combat simulator for X
 *
 *	Written by Riley Rainey,  riley@mips.com
 *
 *	Permission to use, copy, modify and distribute (without charge) this
 *	software, documentation, images, etc. is granted, provided that this 
 *	comment and the author's name is retained.
 *
 */
 
#include "manifest.h"
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>

extern char *getenv ();
extern struct servent *getservent();

jmp_buf	dead;

done () {
	longjmp (dead, 1);
}

main (argc, argv)
int	argc;
char	*argv[]; {

	char	*display;
	char	*host;
	char	myhost[64];
	char	args[128];
	int	s, n = 1;
	struct passwd	*pwent, *getpwuid();
	struct sockaddr_in sin;
	struct hostent	*h = (struct hostent *) NULL;

	if ((host = getenv("ACMSERVER")) == NULL)
		if (argc >= 2) {
			host = argv[1];
			n = 2;
		}
		else {
			fprintf (stderr, "usage: %s server-hostname\n", argv[0]);
			exit (1);
		}

	if ((pwent = getpwuid(getuid())) == NULL) {
		fprintf (stderr, "Yow!\n");
		exit (1);
	}

	if ((display = getenv("DISPLAY")) == NULL) {
		fprintf (stderr, "Excuse me, but you have no DISPLAY.\n\
How do you use X, anyway?\n");
		exit (1);
	}

/*
 *  Sometimes we end up with a DISPLAY value that won't jive on the
 *  network (e.g. ":0.0") -- fix these cases.  Also, from Michael Pall,
 *  if the acm server is running on the local system, allow "unix:*" and
 *  "local:*" connections.
 */

	gethostname (myhost, sizeof(myhost));
	if (*display == ':') {
		strcat (myhost, display);
		display = myhost;
	}
	else if (strcmp (myhost, host)) {
		if (strncmp ("unix:", display, 5) == 0) {
			strcat (myhost, display+4);
			display = myhost;
		}
		else if (strncmp ("local:", display, 6) == 0) {
			strcat (myhost, display+5);
			display = myhost;
		}
	}

	if ((sin.sin_addr.s_addr = inet_addr (host)) != -1) {
		sin.sin_family = AF_INET;
	}
	else if ((h = gethostbyname (host)) != 0) {
		sin.sin_family = h->h_addrtype;
		bcopy (h->h_addr, &sin.sin_addr, h->h_length);
	}
	else {
		fprintf (stderr, "Unknown host \"%s\"\n", host);
		exit (1);
	}

	sin.sin_port = htons(ACM_PORT);

	signal (SIGPIPE, done);

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		perror ("can't get a socket");
		exit (1);
	}

	if (connect (s, (struct sockaddr *) &sin, sizeof (sin)) < 0) {
		perror ("can't connect to server");
		close (s);
		exit (1);
	}

	if (argv[n] != (char *) NULL) 
		strcpy (args, argv[n++]);
	else
		strcpy (args, "");

	for (; argv[n] != (char *) NULL; ++n) {
		strcat (args, "|");
		strcat (args, argv[n]);
	}

	if (setjmp (dead) == 0)
		handshake (s, display, pwent->pw_name, args);

	exit (0);
}

handshake (s, dpy, name, args)
int	s;
char	*dpy, *name, *args; {

	char	info[128];
	int	n;

	sprintf (info, "%s %s %s\n", dpy, name, args);
	write (s, info, strlen (info));
	while ((n = read (s, info, sizeof (info))) > 0)
		write (fileno (stdout), info, n);

	return 0;
}
