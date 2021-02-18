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
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <setjmp.h>

#if !defined(lint) && !defined(SABER)
static char rcsid[] = "$Revision: acm by Riley Rainey; Revision 2.0 $";
#endif

extern struct servent *getservent();
int sdebug = 1;
int listen_socket;

void parseinfo (s, a, b, c)
char *s, *a, *b, *c; {

	char	*p;

	for (p=a; *s; ++s, ++p)
		if ((*p = *s) == ' ') {
			*p = '\0';
			break;
		}

	++ s;

	for (p=b; *s; ++s, ++p)
		if ((*p = *s) == ' ') {
			*p = '\0';
			break;
		}

	++ s;

	strcpy (c, s);

	return;
}

main (argc, argv)
int	argc;
char	*argv[]; {

	struct sockaddr_in sin;
#ifdef notdef
	struct	servent *sp;
#endif
	int	on = 1;
	int	i, background = 0;

/*
 *  parse arguments
 */

	for (i=1; i<argc; ++i) {

		if (*argv[i] == '-')
			switch (*(argv[i]+1)) {

			case 'b':
				background = 1;
				break;

			default:
				fprintf (stderr, "Invalid switch \"%s\"\n", argv[i]);
				break;
			}
	}

	if (sdebug) {
#ifdef notdef
		if ((sp = getservbyname ("acm", "tcp");
			fprintf (stderr, "can't find acm service\n");
			exit (1);
		}
#endif

		if ((listen_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
			perror ("socket");
			exit (1);
		}

		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = INADDR_ANY;
		sin.sin_port = htons(ACM_PORT);

		if (bind (listen_socket, &sin, sizeof(sin)) < 0) {
			perror ("bind");
			exit (1);
		}
	}
	else {
		listen_socket = 0;		/* inetd sets this up for us */
/*		freopen ("/people/riley/acm.error", "a", stderr); */
	}

    	(void) setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
		(char *) &on, sizeof on);
	(void) setsockopt(listen_socket, SOL_SOCKET, SO_KEEPALIVE,
		(char *) &on, sizeof on);
	ioctl(listen_socket, FIONBIO, (char *) &on);

	if (listen (listen_socket, 5) < 0) {
		perror ("listen");
		close (listen_socket);
		exit (1);
	}

	if (background)
#ifdef SYSV
		setpgrp (1);
#else
		setpgrp (0, 1);
#endif

	init ();
	input();

}

int peerdied = 0;

acm_sig_t deadpeer () {
	fprintf (stderr, "SIGPIPE\n");
	peerdied = 1;
#ifndef SIGVOID
	return(0);
#endif
}

static struct	sigvec	alrm, pipe;
int doUpdate = 0;

acm_sig_t myalarm () {
	doUpdate++;
	sigvec (SIGALRM, &alrm, (struct sigvec *) 0);
#ifndef SIGVOID
	return(0);
#endif
}

input () {

	fd_set	fdset, ofdset;
	int	news = -1, playerchange = 0, n, pno, addrlen;
	int	on = 1;
	struct	sockaddr addr;
	struct	itimerval update;
	char	*bp, buf[128], name[64], display[64], args[128];

	alrm.sv_handler = myalarm;
	alrm.sv_mask = 0;
#ifdef hpux
	alrm.sv_flags = SV_BSDSIG;
#else
	alrm.sv_flags = SV_INTERRUPT;
#endif
	sigvec (SIGALRM, &alrm, (struct sigvec *) 0);

/*
 *  Set real time clock to interrupt us every UPDATE_INTERVAL usecs.
 */

	update.it_interval.tv_sec = 0;
	update.it_interval.tv_usec = UPDATE_INTERVAL;
	update.it_value.tv_sec = 0;
	update.it_value.tv_usec = UPDATE_INTERVAL;
	setitimer (ITIMER_REAL, &update, 0);

	pipe.sv_handler = SIG_DFL;
	pipe.sv_mask = 0;
#ifdef hpux
	pipe.sv_flags = SV_BSDSIG;
#else
	pipe.sv_flags = SV_INTERRUPT;
#endif
	sigvec (SIGPIPE, &pipe, (struct sigvec *) 0);

	FD_ZERO (&ofdset);
	FD_ZERO (&fdset);
	FD_SET (listen_socket, &ofdset);

	for (;;) {

		sigsetmask (0);

		fdset = ofdset;
		pno = select (32, &fdset, (fd_set *) NULL, (fd_set *) NULL,
			(struct itimerval *) NULL);

		sigblock (SIGALRM);

		if (pno < 0) {
			FD_CLR (listen_socket, &fdset);
			if (news > 0)
				FD_CLR (news, &fdset);
		}

		if (FD_ISSET (listen_socket, &fdset) ||
			(news > 0 && FD_ISSET(news, &fdset))) {
			if (news == -1) {
				addrlen = sizeof (addr);
				news = accept(listen_socket, &addr, &addrlen);
				if (news > 0) {
					peerdied = 0;
					pipe.sv_handler = deadpeer;
					sigvec(SIGPIPE, &pipe, (struct sigvec *) 0);
					ioctl (news, FIONBIO, &on);
					FD_SET (news, &ofdset);
					bp = buf;
				}
			}
			if (news > 0 && FD_ISSET (news, &fdset)) {
				if ((n = read (news, bp, 1)) > 0) {
					if (*bp == '\n') {
						*bp = '\0';
						parseinfo (buf, display,
							name, args);
						if (newPlayer (news,
							display, name, args) == 0)
							write (news, "Ready.\n", 7);
							printf ("%s\n", display);
						close (news);
						news = -1;
					}
					else
						bp++;
					playerchange = 1;
				}
				if (n == 0)
					peerdied = 1;
			}
		}

		if (playerchange) {
			FD_ZERO (&ofdset);
			FD_SET (listen_socket, &ofdset);
			if (news >= 0)
				FD_SET (news, &ofdset);
			pipe.sv_handler = SIG_DFL;
			sigvec (SIGPIPE, &pipe, (struct sigvec *) 0);
			playerchange = 0;
		}

		if (doUpdate) {
			doUpdate = 0;
			redraw ();
		}

	}
}
