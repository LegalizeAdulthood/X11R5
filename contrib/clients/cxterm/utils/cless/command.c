/*
 * User-level command processor.
 */

#include "less.h"
#include "position.h"
#include <setjmp.h>

extern jmp_buf main_loop;
extern int erase_char, kill_char;
extern int pr_type;
extern int sigs;
extern int ispipe;
extern int quit_at_eof;
extern int hit_eof;
extern int sc_width, sc_height;
extern int sc_window;
extern char *first_cmd;
extern char *every_first_cmd;
extern char version[];
extern char current_file[];
extern char *editor;

static char cmdbuf[90];		/* Buffer for holding a multi-char command */
#if SHELL_ESCAPE
static char shellcmd[200];	/* For holding last shell command for "!!" */
#endif
static char *cp;		/* Pointer into cmdbuf */
static int cmd_col;		/* Current column of the multi-char command */
static char mcc;		/* The multi-char command letter (e.g. '/') */
static char last_mcc;		/* The previous mcc */
static int screen_trashed;	/* The screen has been overwritten */

/*
 * Reset command buffer (to empty).
 */
cmd_reset()
{
	cp = cmdbuf;
}

/*
 * Backspace in command buffer.
 */
	static int
cmd_erase()
{
	if (cp == cmdbuf)
		/*
		 * Backspace past beginning of the string:
		 * this usually means abort the command.
		 */
		return (1);

	if (control_char(*--cp))
	{
		/*
		 * Erase an extra character, for the carat.
		 */
		backspace();
		cmd_col--;
	}
	backspace();
	cmd_col--;
	return (0);
}

/*
 * Set up the display to start a new multi-character command.
 */
start_mcc(c)
	int c;
{
	mcc = c;
	lower_left();
	clear_eol();
	putchr(mcc);
	cmd_col = 1;
}

/*
 * Process a single character of a multi-character command, such as
 * a number, or the pattern of a search command.
 */
	static int
cmd_char(c)
	int c;
{
	if (c == erase_char)
	{
		if (cmd_erase())
			return (1);
	} else if (c == kill_char)
	{
		/* {{ Could do this faster, but who cares? }} */
		while (cmd_erase() == 0)
			;
	} else
	{
		/*
		 * Append the character to the string,
		 * if there is room in the buffer and on the screen.
		 */
		if (cp < &cmdbuf[sizeof(cmdbuf)-1] && cmd_col < sc_width-3)
		{
			*cp++ = c;
			if (control_char(c))
			{
				putchr('^');
				cmd_col++;
				c = carat_char(c);
			}
			putchr(c);
			cmd_col++;
		} else
			bell();
	}
	return (0);
}

/*
 * Return the number currently in the command buffer.
 */
	static int
cmd_int()
{
	*cp = '\0';
	cp = cmdbuf;
	return (atoi(cmdbuf));
}

/*
 * Move the cursor to lower left before executing a command.
 * This looks nicer if the command takes a long time before
 * updating the screen.
 */
	static void
cmd_exec()
{
	lower_left();
	flush();
}

/*
 * Display the appropriate prompt.
 */
	static void
prompt()
{
	register char *p;

	if (first_cmd != NULL && *first_cmd != '\0')
		/*
		 * No prompt necessary if commands are from first_cmd
		 * rather than from the user.
		 */
		return;

	/*
	 * If nothing is displayed yet, display starting from line 1.
	 */
	if (position(TOP) == NULL_POSITION)
		jump_back(1);
	else if (screen_trashed)
		repaint();
	screen_trashed = 0;

	/*
	 * Select the proper prompt and display it.
	 */
	lower_left();
	clear_eol();
	p = pr_string();
	if (p == NULL)
		putchr(':');
	else
	{
		so_enter();
		putstr(p);
		so_exit();
	}
}

/*
 * Get command character.
 * The character normally comes from the keyboard,
 * but may come from the "first_cmd" string.
 */
	static int
getcc()
{
	if (first_cmd == NULL)
		return (getchr());

	if (*first_cmd == '\0')
	{
		/*
		 * Reached end of first_cmd input.
		 */
		first_cmd = NULL;
		if (cp > cmdbuf && position(TOP) == NULL_POSITION)
		{
			/*
			 * Command is incomplete, so try to complete it.
			 * There are only two cases:
			 * 1. We have "/string" but no newline.  Add the \n.
			 * 2. We have a number but no command.  Treat as #g.
			 * (This is all pretty hokey.)
			 */
			if (mcc != ':')
				/* Not a number; must be search string */
				return ('\n'); 
			else
				/* A number; append a 'g' */
				return ('g');
		}
		return (getchr());
	}
	return (*first_cmd++);
}

/*
 * Main command processor.
 * Accept and execute commands until a quit command, then return.
 */
	public void
commands()
{
	register int c;
	register int n;
	register int scroll = 10;

	last_mcc = 0;
	setjmp(main_loop);
	mcc = 0;

	for (;;)
	{
		/*
		 * Display prompt and accept a character.
		 */
		psignals();	/* See if any signals need processing */

		if (quit_at_eof && (quit_at_eof + hit_eof) > 2)
			/*
			 * After hitting end-of-file for the second time,
			 * automatically advance to the next file.
			 * If there are no more files, quit.
			 */
			next_file(1);

		cmd_reset();
		prompt();
		c = getcc();

	again:
		if (sigs)
			continue;

		if (mcc)
		{
			/*
			 * We are in a multi-character command.  
			 * All chars until newline go into the command buffer.
			 * (Note that mcc == ':' is a special case that
			 *  means a number is being entered.)
			 */
			if (mcc != ':' && (c == '\n' || c == '\r'))
			{
				char *p;
				static char fcbuf[100];

				/*
				 * Execute the command.
				 */
				*cp = '\0';
				cmd_exec();
				switch (mcc)
				{
				case '/': case '?':
					search(mcc, cmdbuf, n);
					break;
				case '+':
					for (p = cmdbuf;  *p == '+' || *p == ' ';  p++) ;
					if (*p == '\0')
						every_first_cmd = NULL;
					else
					{
						strtcpy(fcbuf, p, sizeof(fcbuf));
						every_first_cmd = fcbuf;
					}
					break;
				case '-':
					toggle_option(cmdbuf);
					break;
				case 'E':
					/*
					 * Ignore leading spaces 
					 * in the filename.
					 */
					for (p = cmdbuf;  *p == ' ';  p++) ;
					edit(glob(p));
					break;
#if SHELL_ESCAPE
				case '!':
					/*
					 * !! just uses whatever is in shellcmd.
					 * Otherwise, copy cmdbuf to shellcmd,
					 * replacing any '%' with the current
					 * file name.
					 */
					if (*cmdbuf != '!')
					{
						register char *fr, *to;
						to = shellcmd;
						for (fr = cmdbuf;
							*fr != '\0';  fr++)
						{
							if (*fr != '%')
								*to++ = *fr;
							else
							{
								strcpy(to,
								 current_file);
								to += strlen(to);
							}
						}
						*to = '\0';
					}
					lsystem(shellcmd);
					screen_trashed = 1;
					error("!done");
					break;
#endif
				}
				mcc = 0;
			} else
			{
				if (mcc == ':' && (c < '0' || c > '9') &&
					c != erase_char && c != kill_char)
				{
					/*
					 * This is not part of the number
					 * we were entering.  Process
					 * it as a regular character.
					 */
					mcc = 0;
					goto again;
				}

				/*
				 * Append the char to the command buffer.
				 */
				if (cmd_char(c))
				{
					/* Abort the multi-char command. */
					mcc = 0;
					continue;
				}
				c = getcc();
				goto again;
			}
		} else switch (c)
		{
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			/*
			 * First digit of a number.
			 */
			start_mcc(':');
			goto again;

		case 'f':
		case ' ':
		case CONTROL('F'):
			/*
			 * Forward one screen.
			 */
			n = cmd_int();
			if (n <= 0)
				n = sc_window;
			cmd_exec();
			forward(n, 1);
			break;

		case 'b':
		case CONTROL('B'):
			/*
			 * Backward one screen.
			 */
			n = cmd_int();
			if (n <= 0)
				n = sc_window;
			cmd_exec();
			backward(n, 1);
			break;

		case 'e':
		case 'j':
		case '\r':
		case '\n':
		case CONTROL('E'):
			/*
			 * Forward N (default 1) line.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			cmd_exec();
			forward(n, 0);
			break;

		case 'y':
		case 'k':
		case CONTROL('K'):
		case CONTROL('Y'):
			/*
			 * Backward N (default 1) line.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			cmd_exec();
			backward(n, 0);
			break;

		case 'd':
		case CONTROL('D'):
			/*
			 * Forward N lines 
			 * (default same as last 'd' or 'u' command).
			 */
			n = cmd_int();
			if (n > 0)
				scroll = n;
			cmd_exec();
			forward(scroll, 0);
			break;

		case 'u':
		case CONTROL('U'):
			/*
			 * Forward N lines 
			 * (default same as last 'd' or 'u' command).
			 */
			n = cmd_int();
			if (n > 0)
				scroll = n;
			cmd_exec();
			backward(scroll, 0);
			break;

		case 'R':
			/*
			 * Flush buffers, then repaint screen.
			 * Don't flush the buffers on a pipe!
			 */
			if (!ispipe)
				ch_init(0);
			/* Fall thru */
		case 'r':
		case CONTROL('R'):
		case CONTROL('L'):
			/*
			 * Repaint screen.
			 */
			repaint();
			break;

		case 'g':
			/*
			 * Go to line N, default beginning of file.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			cmd_exec();
			jump_back(n);
			break;

		case 'p':
		case '%':
			/*
			 * Go to a specified percentage into the file.
			 */
			n = cmd_int();
			if (n < 0)
				n = 0;
			if (n > 100)
				n = 100;
			cmd_exec();
			jump_percent(n);
			break;

		case 'G':
			/*
			 * Go to line N, default end of file.
			 */
			n = cmd_int();
			cmd_exec();
			if (n <= 0)
				jump_forw();
			else
				jump_back(n);
			break;

		case '=':
		case CONTROL('G'):
			/*
			 * Print file name, etc.
			 */
			error(eq_message());
			break;
			
		case 'V':
			/*
			 * Print version number, without the "@(#)".
			 */
			error(version+4);
			break;

		case 'q':
			/*
			 * Exit.
			 */
			/*setjmp(main_loop);*/
			quit();

		case '/':
		case '?':
			/*
			 * Search for a pattern.
			 * Accept chars of the pattern until \n.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			start_mcc(c);
			last_mcc = c;
			c = getcc();
			goto again;

		case 'n':
			/*
			 * Repeat previous search.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			start_mcc(last_mcc);
			cmd_exec();
			search(mcc, (char *)NULL, n);
			mcc = 0;
			break;

		case 'H':
			/*
			 * Help.
			 */
			lower_left();
			clear_eol();
			putstr("help");
			cmd_exec();
			help();
			screen_trashed = 1;
			break;

		case 'E':
			/*
			 * Edit a new file.  Get the filename.
			 */
			cmd_reset();
			start_mcc('E');
			putstr("xamine: ");	/* This looks nicer */
			cmd_col += 8;
			c = getcc();
			goto again;
			
		case '!':
#if SHELL_ESCAPE
			/*
			 * Shell escape.
			 */
			cmd_reset();
			start_mcc('!');
			c = getcc();
			goto again;
#else
			error("Command not available");
			break;
#endif

		case 'v':
#if EDITOR
			if (ispipe)
			{
				error("Cannot edit standard input");
				break;
			}
			sprintf(cmdbuf, "%s %s", editor, current_file);
			lsystem(cmdbuf);
			ch_init(0);
			screen_trashed = 1;
			break;
#else
			error("Command not available");
			break;
#endif

		case 'N':
			/*
			 * Examine next file.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			next_file(n);
			break;

		case 'P':
			/*
			 * Examine previous file.
			 */
			n = cmd_int();
			if (n <= 0)
				n = 1;
			prev_file(n);
			break;

		case '-':
			/*
			 * Toggle a flag setting.
			 */
			cmd_reset();
			start_mcc('-');
			c = getcc();
			goto again;

		case '+':
			cmd_reset();
			start_mcc('+');
			c = getcc();
			goto again;

		case 'm':
			/*
			 * Set a mark.
			 */
			lower_left();
			clear_eol();
			putstr("mark: ");
			c = getcc();
			if (c == erase_char || c == kill_char)
				break;
			setmark(c);
			break;

		case '\'':
			/*
			 * Go to a mark.
			 */
			lower_left();
			clear_eol();
			putstr("goto mark: ");
			c = getcc();
			if (c == erase_char || c == kill_char)
				break;
			gomark(c);
			break;

		default:
			bell();
			break;
		}
	}
}
