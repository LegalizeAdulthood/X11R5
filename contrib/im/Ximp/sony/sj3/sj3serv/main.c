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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include "Const.h"

#ifndef lint
static char rcsid_sony[] = "$Header: main.c,v 1.1 91/09/04 23:06:51 hosoe Locked $ SONY;";
#endif

void	_exit();

extern	int	fork_flag;
extern	char	*lock_file;

static	void	opening()
{
	extern	char	*version_number;
	extern	char	*time_stamp;

	printf("Kana-Kanji Conversion Server Version %s\r\n", version_number);
	printf("Copyright (c) 1990 by Sony Corporation\r\n");
	printf("Created at %s\r\n\n", time_stamp);
}

#ifdef	LOCK_FILE
int	make_lockfile()
{
	int	fd;

	fd = open(lock_file, O_CREAT | O_EXCL, 0600);
	if (fd < 0) return ERROR;
	close(fd);
	return 0;
}
int	erase_lockfile()
{
	return unlink(lock_file);
}
#endif


static	int	signal_handler(sig, code, scp)
int	sig;
int	code;
struct sigcontext *scp;
{
	warning_out("signal %d, code %d catched.", sig, code);
}
static	int	terminate_handler(sig, code, scp)
int	sig;
int	code;
struct sigcontext *scp;
{
	close_socket();
	closeall();
#ifdef	LOCK_FILE
	erase_lockfile();
#endif
	exit(0);
}
server_terminate()
{
	close_socket();
	closeall();
#ifdef	LOCK_FILE
	erase_lockfile();
#endif
	exit(0);
}

static	void	exec_fork()
{
	int	tmp;

	if (fork_flag && (tmp = fork())) {
		if (tmp < 0) {
			fprintf(stderr, "Can't fork child process\r\n");
			fflush(stderr);
		}
		else {
			signal(SIGCHLD, _exit);
			signal(SIGHUP , SIG_IGN);
			signal(SIGINT , SIG_IGN);
			signal(SIGQUIT, SIG_IGN);
			signal(SIGTSTP, SIG_IGN);
			signal(SIGTERM, _exit);
			wait(0);
			_exit(0);
		}
	}

	signal(SIGHUP,  signal_handler);
	signal(SIGINT,  terminate_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTERM, terminate_handler);
	if (fork_flag)
		signal(SIGTSTP, SIG_IGN);
}


static	void	leave_tty()
{
	int	tmp;

	tmp  = open_error();
	tmp |= open_log();
	tmp |= open_debug();
	kill(getppid(), SIGTERM);
	fclose(stdin);
	fclose(stdout);
	if (!tmp) fclose(stderr);

	if (fork_flag) {
		if ((tmp = open("/dev/tty", O_RDWR)) >= 0) {
			ioctl(tmp, TIOCNOTTY, 0);
			close(tmp);
		}
	}
}

int	main(argc, argv)
int	argc;
char	**argv;
{
	opening();
	if (setuid(geteuid())) {
		fprintf(stderr, "error at setuid.\r\n"); exit(1);
	}

	parse_arg(argc, argv);
	read_runcmd();
	set_default();

#ifdef	LOCK_FILE
	if (make_lockfile() == ERROR) {
		fprintf(stderr, "Already running...\r\n"); exit(1);
	}
#endif

	exec_fork();

	socket_init();
	open_socket();

	leave_tty();

	preload_dict();
	communicate();

	close_socket();
	closeall();
#ifdef	LOCK_FILE
	erase_lockfile();
#endif
	exit(0);
}

