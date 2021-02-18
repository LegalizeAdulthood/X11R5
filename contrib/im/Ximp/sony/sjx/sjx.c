/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	sjx.c
 *	SJX X11 Kana-Kanji Conversion Window
 *	Multi-Clients Server
 *	Copyright (c) 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Thu Jun 23 11:31:31 JST 1988
 */
/*
 * $Header: sjx.c,v 1.2 91/09/30 21:17:17 root Exp $ SONY;
 */

#include <X11/X.h>
#include <sys/errno.h>
#include "common.h"
#include "sjx.h"
#include "key.h"

static char *sccsid  = "@(#)sjx.c 2.21X 91/04/18 SONY;";
static char *rcsid   = "$Header: sjx.c,v 1.2 91/09/30 21:17:17 root Exp $ SONY;";
static char *sjxd = "SJX Date Thu Apr 18 02:19:16 JST 1991";
static char *sjxv = "SJX Version 2.21X Copyright (c) Sony Corporation";

int		keyvalue;
int		codevalue;
int		convert_status = 0;
int		henkan_flag = 0;

extern int	vflag;

char		*prog_name;
char		server_name[LONGLENGTH];

parse_argv (argc, argv)
int	argc;
char	**argv;
{
	char	*s;
	char	*rindex ();

	if ((s = rindex (argv[0], '/')) == 0)
		prog_name = argv[0];
	else
		prog_name = s + 1;
	InitArgv (argc, argv);
	if (vflag > 1)
		fprintf (stderr, "program name %s\n", prog_name);
}

setsjserv (name)
char	*name;
{
	if (server_name[0] == NULL && name != NULL)
		strcpy (server_name, name);
}

/* get user name, server name */
init_env ()
{
	char	*getenv ();

	setsjserv (getenv ("SJ3SERV"));
}

SJ_init ()
{
	extern int	done2 ();
	extern int	makecore ();

	InitConversion ();
	set_term ();
	getsjrc ();
	SJ2_henkan_init ();
	(void) signal (SIGTERM, done2);
	(void) signal (SIGINT, done2);
	(void) signal (SIGQUIT, makecore);
	(void) signal (SIGBUS, makecore);
	(void) signal (SIGSEGV, makecore);
	(void) signal (SIGFPE, makecore);
}

set_term ()
{
	extern char	term_name[], *prog_name;

	strcpy (term_name, prog_name);
}

main (argc, argv)
int	argc;
char	**argv;
{
	parse_argv (argc, argv);
	init_env ();
	SJ_init ();

	xdisplay_init ();

	X_init (argc, argv);

	beep ();
	convert ();
}

convert_stat ()
{
	Xlc_conversion (1);	/* 1: on	*/
	MapWindow ();
	convert_status = 1;
	stat_conv_on ();
	stat_conv (1);
	convert_status = 0;
	UnmapWindow ();
	Xlc_conversion (0);	/* 0: off	*/
}

convert ()
{
	extern int	Direct;
	extern int	allways_buff;
	extern int	no_flush;
	register int	c;
	register Conversion	*cv;

	cv = GetConversion ();
	allways_buff = 1;
	no_flush = 0;
	cv->BuffMode = BUFF;

	set_guide_line (KEY_NORMAL);
	print_guide_line ();

	stat_init ();
	exec_mode (KEY_ZHIRA);

	Xlc_conversion (0);	/* 0: off */

	while (1) {
		if ((c = inkey ()) == EOF)
			continue;
		/* It is important to reassign conversion after inkey	*/
		cv = GetConversion ();
		switch (keyvalue) {
			case KEY_NORMAL:
				proto_out ();
				break;
			case KEY_CONV:
				henkan_flag = 1;
				set_guide_line (KEY_CONV);
				print_guide_line ();
				convert_stat ();
				set_guide_line (KEY_NORMAL);
				print_guide_line ();
				henkan_flag = 0;
				break;
			default:
				proto_out ();
				break;
		}
	}
}

static int	unget_char = EOF;
static int	unget_value;

unget_key (c, v)
int	c;
int	v;
{
	unget_char = c;
	unget_value = v;
}

inkey ()
{
	register Conversion	*cv;
	extern int		max_buflen;
	extern int		keylen;
	extern int		inchar;
	u_short			row, col;

	if (unget_char != EOF) {
		keyvalue = unget_value;
		inchar = unget_char;
		unget_char = EOF;
		RaiseWindow ();
		return (inchar);
	}
	while (1) {
		if (keylen > 1) {
			inchar = next_keybuff ();
			keyvalue = codevalue = KEY_NORMAL;
			break;
		}

		proc_proto ();

		codevalue = KEY_IGNORE;

		dispatch ();

		cv = GetConversion ();
		CursorRead (&row, &col);
		if (row > cv->line || (row == cv->line && col >= cv->column))
			max_buflen = 1;
		else
			max_buflen = 0;
				/*
				 * These controls overflow.
				 * max_buflen is checked in stat_conv.
				 * Before map, row == 1, T_line == 0.
				 * If overflow is occured, row == T_line.
				 */
		keyvalue = codevalue;
		if (keylen == 0 && keyvalue == KEY_NORMAL)
			continue;
		if (keyvalue != KEY_IGNORE)
			break;
	}
	RaiseWindow ();
	return (inchar);
}

dispatch ()
{
	for (;;) {
#ifndef PROTO1
		if (Xdispatch ())
			break;
#endif /* PROTO1 */
#ifdef PROTO1
		if (proto1_dispatch ())
			break;
#endif /* PROTO1 */
	}
}

done2 ()
{
	SJ2_henkan_end ();		/* add 1989.02.25 masaki	*/
	EndConversion ();
#ifdef PROTO1
	SJX_end ();
#endif /* PROTO1 */
#ifdef PROTO2
	proto2_end ();
#endif /* PROTO2 */
	printf ("\n\rsjx done.\n\r");
	exit (0);
}

makecore ()
{
	SJ2_henkan_end ();
	EndConversion ();
	SJX_end ();
	proto2_end ();
	printf ("\n\rsjx terminate.\n\r");

	chdir ("/tmp");
	setgid (getgid());
	setuid (getuid());
	abort ();
}
