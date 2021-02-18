/* Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.

                        All Rights Reserved

In order to better advertise the GNU 'plot' graphics package, the
standard GNU copyleft on this software has been lifted, and replaced
by the following:
   Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both the copyright notice and this permission notice appear in
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

xplot is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY.  No author or distributor accepts responsibility to
anyone for the consequences of using it or for whether it serves any
particular purpose or works at all, unless he says so in writing.
Refer to the GNU General Public License for full details. Also:

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.  */

#include "config.h"
#include "patchlevel.h"
#include <signal.h>

char *prog;
char *bgname = NULL;
char *displayname = NULL;
char *fgname = NULL;
char *fontname = NULL;
char *geom = NULL;
int high_byte_first = 0;
int killlast = 0;
int reverse = 0;
FILE *instrm = stdin;

int forkthistime, fork_flag = 1;

int debug_flag;

#define MAXFILELIST 1024

char *filelist[MAXFILELIST]; /* Keep track of the command line named files */
int numfiles = 0;

int main(argc, argv)
int argc;
char *argv[];
{
	int i;
	FILE *tmp_file;

	prog = argv[0];

/* Handle the input options, including the ones that used
   to be handled by the toolkit. */

	for (i=1; i < argc; i++){
/* Not using getopt: Be somewhat inflexible... */
		if (argv[i][0] == '-') {
			if ((!strcmp(argv[i],"-bg")) ||
					(!strcmp(argv[i], "-background"))) {
				if ((bgname = argv[++i]) == NULL) {
					usage();
					exit(-1);
				}
				continue; /* Next arg */
			}
			if ((!strcmp(argv[i],"-c")) ||
					(!strcmp(argv[i], "-copying"))) {
/* Copying information */	       display_copying();
					exit(0);
			}
#ifndef DONT_HANDLE_SIGHUP
			if (!strcmp(argv[i], "-C")) {
/* do not ignore sighup. */	     signal(SIGHUP, SIG_IGN);
					continue;
			}
#endif
			if ((!strcmp(argv[i], "-d")) ||
					(!strcmp(argv[i],"-display"))) {
/* Set display */	       displayname = argv[++i];
				continue;
			}
#ifdef DEBUG
			if ((!strcmp(argv[i], "-D")) ||
				(!strcmp(argv[i], "-Debug"))) {
/* Turn on debugging */		 debug_flag = 1;
					continue;
			}
#endif
			if ((!strcmp(argv[i],"-fg")) ||
				(!strcmp(argv[i], "-foreground"))) {
				if ((fgname = argv[++i]) == NULL) {
					usage();
					exit(-1);
				}
				continue; /* Next arg */
			}
			if ((!strcmp(argv[i], "-fn")) ||
					(!strcmp(argv[i], "-font"))) {
				if ((fontname = argv[++i]) == NULL) {
					usage();
					exit(-1);
				}
				continue; /* Next arg */
			}
			if (!strcmp(argv[i],"-geometry")) {
				if ((geom = argv[++i]) == NULL) {
					usage();
					exit(-1);
				}
				continue; /* Next arg */
			}
			if ((!strcmp(argv[i], "-h")) ||
				(!strcmp(argv[i], "-help"))) {
/* Give some help */		    usage();
					exit(0);
			}
			if ((!strcmp(argv[i], "-H")) ||
				(!strcmp(argv[i], "-High"))) {
/* Byte ordering */		     high_byte_first = 1;
					continue;
			}
			if ((!strcmp(argv[i], "-L")) ||
				(!strcmp(argv[i], "-Low"))) {
					high_byte_first = -1;
					continue;
			}
			if (!strcmp(argv[i], "-n")) {
/* Control windows */	   killlast = 1;
				continue;
			}
			if (!strcmp(argv[i], "-N")) {
/* No forking */			fork_flag = 0;
					continue;
			}
			if ((!strcmp(argv[i],"-rv")) ||
					(!strcmp(argv[i], "-reverse"))) {
/* Reverse bg and fg */	 reverse = 1;
				continue;
			}
			if ((!strcmp(argv[i], "-v")) ||
				(!strcmp(argv[i], "-V"))) {
/* Show version of program */	   display_version();
					exit(0);
			}
			if ((!strcmp(argv[i], "-w")) ||
				(!strncmp(argv[i], "-Waranty", 2))) {
/* Warranty... */		       display_warranty();
					exit(0);
			}
			/* Default */
			usage();
			exit(0);
		}
	/* If it got past the above, the argument must be a data file */
		tmp_file = fopen (argv[i], "r");
		if (tmp_file == NULL) {
		  (void) fprintf (stderr, "%s: Can't find file `%s'\n",
				  prog, argv[i]);
		} else {
/* Store the filename for now */
			filelist[numfiles++] = argv[i];
			if (numfiles >= MAXFILELIST) {
				fprintf(stderr, "%s: Too many data files!\n",
						prog);
				exit(-1);
			}
/* We're going to read it all in later */
			fclose(tmp_file);
		}
	}

/* Now read in the data files, and run... */

	/* First case: read from stdin => Do funny things to make
		plots appear immediately rather than later. */

	if (numfiles == 0) {
		forkthistime = fork_flag;
		openpl(1);
		closepl();
	} else { /* Read from the named files -- Nuttin funny here */
		for (i=0; i < numfiles; i++) {
			if ((i < numfiles - 1) && (killlast))
				forkthistime = 0; 
			else
				forkthistime = fork_flag;
			instrm = fopen (filelist[i], "r");
			if ((i == 0) || (!killlast && forkthistime)) {
			    openpl(1); /* Open a new window! */
			    if (!killlast && forkthistime) closepl();
			}
			else
			    openpl(0); /* Continue plotting in the old one */
		}
	}
	if (!forkthistime) /* Didn't fork, so let's wait right here */
		buttonwait();
}

display_version()
{
	(void) fprintf(stderr, "\
%s version %s, Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.\n\
%s comes with ABSOLUTELY NO WARRANTY; type `%s -warranty' for details.\n\
This is free software, and you are welcome to redistribute it;\n\
Type `%s -copying' to view the copying conditions.\n",
		    prog, PATCHLEVEL, prog, prog, prog);
}

usage()
{
	fprintf(stderr, "\
%s reads binary plot(5) data and takes these arguments:\n", prog);
	fprintf(stderr, "-bg colorname [background color]\n");
	fprintf(stderr, "-c [show copying information]\n");
	fprintf(stderr, "-display dispname\n");
	fprintf(stderr, "-fg colorname [foreground color]\n");
	fprintf(stderr, "-fn fontname [font for labels]\n");
	fprintf(stderr, "-geometry geom [Standard X geometry string]\n");
	fprintf(stderr, "-h [give this help]\n");
	fprintf(stderr, "-H [high byte first ordering]\n");
	fprintf(stderr, "-L [low byte first]\n");
	fprintf(stderr, "-n [kill previous xplot windows on the display]\n");
	fprintf(stderr, "-N [No forking]\n");
	fprintf(stderr, "-rv [reverse foreground and background]\n");
	fprintf(stderr, "-v [show version number]\n");
	fprintf(stderr, "-w [show warranty]\n");
}

display_warranty()
{
fprintf(stderr, "\
%s is distributed in the hope that it will be useful, but WITHOUT\n\
ANY WARRANTY.  No author or distributor accepts responsibility to\n\
anyone for the consequences of using it or for whether it serves any\n\
particular purpose or works at all, unless he says so in writing.\n\
Refer to the GNU General Public License for full details. Also:\n\n\
DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING\n\
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL\n\
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR\n\
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,\n\
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,\n\
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS\n\
SOFTWARE.\n", prog);
}

display_copying()
{
	fprintf(stderr, "\
Copyright (C) 1989, 1990, 1991 Free Software Foundation, Inc.\n\n\
                        All Rights Reserved\n\n\
In order to better advertise the GNU 'plot' graphics package, the\n\
standard GNU copyleft on this software has been lifted, and replaced\n\
by the following:\n");

	fprintf(stderr, "\
   Permission to use, copy, modify, and distribute this software and its\n\
documentation for any purpose and without fee is hereby granted,\n\
provided that the above copyright notice appear in all copies and that\n\
both the copyright notice and this permission notice appear in\n\
supporting documentation, and that the names of Digital or MIT not be\n\
used in advertising or publicity pertaining to distribution of the\n\
software without specific, written prior permission.\n");
}

