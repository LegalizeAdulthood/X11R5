/*
 * Copyright (c) 1991 by Sun Microsystems, Inc.
 */
#ident	"%Z%%M%	%I%	%E% SMI"

#include	<stdio.h>
#include	<string.h>

#define	TRUE	1
#define	FALSE	0
#define	MAX_PATH_LEN	1024
#define	MAX_DOMAIN_LEN	1024
#define	MAX_STRING_LEN  2048

#define	USAGE	"Usage:	xgettext [-a [-x exclude-file]] [-ns] \
[-c comment-tag]\n		[-d default-domain] [-m prefix] \
[-p pathname] files ...\n\
	xgettext -h\n"

#define	DEFAULT_DOMAIN	"messages"

extern char	yytext[];

/*
 * Contains a list of strings to be used to store ANSI-C style string.
 * Each quoted string is stored in one node.
 */
struct strlist_st {
	char			*str;
	struct strlist_st	*next;
};

/*
 * istextdomain	: Boolean telling if this node contains textdomain call.
 * isduplicate 	: Boolean telling if this node duplicate of any other msgid.
 * msgid	: contains msgid or textdomain if istextdomain is true.
 * comment	: comment extracted in case of -c option.
 * fname	: tells which file contains msgid.
 * linenum	: line number in the file.
 * next   	: Next node.
 */
struct element_st {
	char			istextdomain;
	char			isduplicate;
	struct strlist_st	*msgid;
	struct strlist_st	*comment;
	char			*fname;
	int			linenum;
	struct element_st	*next;
};

/*
 * dname	   : domain name. NULL if default domain.
 * gettext_head    : Head of linked list containing [d]gettext().
 * gettext_tail    : Tail of linked list containing [d]gettext().
 * textdomain_head : Head of linked list containing textdomain().
 * textdomain_tail : Tail of linked list containing textdomain().
 * next		   : Next node.
 *
 * Each domain contains two linked list.
 *	(gettext_head,  textdomain_head)
 * If -s option is used, then textdomain_head contains all
 * textdomain() calls and no textdomain() calls are stored in gettext_head.
 * If -s option is not used, textdomain_head is empty list and
 * gettext_head contains all gettext() dgettext(), and textdomain() calls.
 */
struct domain_st {
	char			*dname;
	struct element_st	*gettext_head;
	struct element_st	*gettext_tail;
	struct element_st	*textdomain_head;
	struct element_st	*textdomain_tail;
	struct domain_st	*next;
};

/*
 * There are two domain linked lists.
 * def_dom contains default domain linked list and
 * dom_head contains all other deomain linked lists to be created by
 * dgettext() calls.
 */
struct domain_st	*def_dom = NULL;
struct domain_st	*dom_head = NULL;
struct domain_st	*dom_tail = NULL;

/*
 * This linked list contains a list of strings to be excluded when
 * -x option is used.
 */
struct exclude_st {
	struct strlist_st	*exstr;
	struct exclude_st	*next;
} *excl_head;

/*
 * All option flags and values for each option if any.
 */
int	aflg = FALSE;
int	cflg;
char	*comment_tag;
int	dflg;
char	*default_domain;
int	hflg;
int	mflg;
char	*prefix;
int	nflg;
int	pflg;
char	*pathname;
int	sflg;
int	xflg;
char	*exclude_file;

/*
 * Each variable shows the current state of parsing input file.
 *
 * in_macro	 : Means currently processing macro section.
 *		   This is introduced to handle multi line macro.
 * in_comment    : Means inside comment block.
 * in_gettext    : Means inside gettext call.
 * in_dgettext   : Means inside dgettext call.
 * in_textdomain : Means inside textdomain call.
 * in_str	 : Means currently processing ANSI style string.
 * in_quote	 : Means currently processing double quoted string.
 * is_last_comment_line : Means the current line is the last line
 *			  of the comment block. This is necessary because
 *			  in_comment becomes FALSE when '* /' is encountered.
 */
int	in_macro		= FALSE;
int	in_comment		= FALSE;
int	in_gettext		= FALSE;
int	in_dgettext		= FALSE;
int	in_textdomain		= FALSE;
int	in_str			= FALSE;
int	in_quote		= FALSE;
int	is_last_comment_line	= FALSE;

/*
 * This variable contains the first line of gettext(), dgettext(), or
 * textdomain() calls.
 * This is necessary for multiple lines of a single call to store
 * the starting line.
 */
int	linenum_saved = 0;

char	stdin_only = FALSE;	/* Read input from stdin */

/*
 * curr_file    : Contains current file name processed.
 * curr_domain  : Contains the current domain for each dgettext().
 *		  This is NULL for gettext().
 * curr_line    : Contains the current line processed.
 * qstring_buf  : Contains the double quoted string processed.
 * curr_linenum : Line number being processed in the current input file.
 */
char	curr_file[MAX_PATH_LEN];
char	curr_domain[MAX_DOMAIN_LEN];
char	curr_line[MAX_STRING_LEN];
char	qstring_buf[MAX_STRING_LEN];
int	curr_linenum = 1;

/*
 * strhead  : This list contains ANSI style string.
 *		Each node contains double quoted string.
 * strtail  : This is the tail of strhead.
 * commhead : This list contains comments string.
 *		Each node contains one line of comment.
 * commtail : This is the tail of commhead.
 */
struct strlist_st	*strhead = NULL;
struct strlist_st	*strtail = NULL;
struct strlist_st	*commhead = NULL;
struct strlist_st	*commtail = NULL;

/*
 * gargc : Same as argc. Used to pass argc to lex routine.
 * gargv : Same as argv. Used to pass argc to lex routine.
 */
int	gargc;
char	**gargv;
FILE	*fp;

/*
 * Utility functions to malloc a node and initialize fields.
 */
struct domain_st  *NEW_DOMAIN();
struct strlist_st *NEW_STRLIST();
struct element_st *NEW_ELEMENT();
struct exclude_st *NEW_EXCLUDE();

/*
 * Main program of xgettext.
 */
main(argc, argv)
	int	argc;
	char	**argv;
{
	extern char	*optarg;
	extern int	optind;
	int		opterr;
	int		c;

	initialize_globals();

	while ((c = getopt(argc, argv, "hax:nsc:d:m:p:")) != EOF) {
		switch (c) {
		case 'a':
			aflg = TRUE;
			break;
		case 'c':
			cflg = TRUE;
			comment_tag = optarg;
			break;
		case 'd':
			dflg = TRUE;
			default_domain = optarg;
			break;
		case 'h':
			hflg = TRUE;
			break;
		case 'm':
			mflg = TRUE;
			prefix = optarg;
			break;
		case 'n':
			nflg = TRUE;
			break;
		case 'p':
			pflg = TRUE;
			pathname = optarg;
			break;
		case 's':
			sflg = TRUE;
			break;
		case 'x':
			xflg = TRUE;
			exclude_file = optarg;
			break;
		case '?':
			opterr = TRUE;
			break;
		}
	}

	/* if -h is used, ignore all other options. */
	if (hflg == TRUE) {
		fprintf(stderr, USAGE);
		print_help();
		exit(0);
	}

	/* -x can be used only with -a */
	if ((xflg == TRUE) && (aflg == FALSE))
		opterr = TRUE;

	if (opterr == TRUE) {
		fprintf(stderr, USAGE);
		exit(2);
	}

	/* error, if no files are specified. */
	if (optind == argc) {
		fprintf(stderr, USAGE);
		exit(2);
	}

	if (xflg == TRUE) {
		read_exclude_file();
	}

	/* If files are -, then read from stdin */
	if (argv[optind][0] == '-') {
		stdin_only = TRUE;
		optind++;
	} else {
		stdin_only = FALSE;
	}

	/* Store argc and argv to pass to yylex() */
	gargc = argc;
	gargv = argv;

#ifdef DEBUG
	printf("optind=%d\n", optind);
	{
	int i = optind;
	for (; i < argc; i++) {
		printf("   %d, <%s>\n", i, argv[i]);
	}
	}
#endif

	if (stdin_only == FALSE) {
		if ((fp = freopen (argv[optind], "r", stdin)) == NULL) {
			fprintf(stderr,
			"ERROR, can't open input file: %s\n", argv[optind]);
			exit (2);
		}
		strcpy (curr_file, gargv[optind]);
		optind++;
	}

	/*
	 * Process input.
	 */
	yylex();

#ifdef DEBUG
	print_all_domain();
#endif

	/*
	 * Write out all .po files.
	 */
	write_all_files();

	exit(0);

} /* main */

/*
 * Prints help information for each option.
 */
print_help()
{
	fprintf(stderr, "\n");
	fprintf(stderr,
		"-a\t\t\tfind ALL strings\n");
	fprintf(stderr,
		"-c <comment-tag>\tget comments containing <flag>\n");
	fprintf(stderr,
	"-d <default-domain>\tuse <default-domain> for default domain\n");
	fprintf(stderr,
		"-h\t\t\tHelp\n");
	fprintf(stderr,
		"-m <prefix>\t\tfill in msgstr with <prefix>msgid\n");
	fprintf(stderr,
		"-n\t\t\tline# file name and line number info in output\n");
	fprintf(stderr,
		"-p <pathname>\t\tuse <pathname> for output file directory\n");
	fprintf(stderr,
		"-s\t\t\tgenerate sorted output files\n");
	fprintf(stderr,
"-x <exclude-file>\texclude strings in file <exclude-file> from output\n");
	fprintf(stderr,
		"-\t\t\tread stdin, use as a filter (input only)\n");

} /* print_help */

/*
 * Handler for MACRO line which starts with #.
 */
handle_macro_line()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		in_macro = TRUE;
	}

} /* handle_macro_line */

/*
 * Handler for "/*" in input file.
 */
handle_open_comment()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat(qstring_buf, yytext);
	} else {
		in_comment = TRUE;
		is_last_comment_line = FALSE;
		/*
		 * If there is any comment extracted before accidently,
		 * clean it up and start the new comment again.
		 */
		free_strlist (commhead);
		commhead = NULL;
	}
}

/*
 * Handler for "* /" in input file.
 */
handle_close_comment()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat(qstring_buf, yytext);
	} else {
		in_comment = FALSE;
		is_last_comment_line = TRUE;
	}
}

/*
 * Handler for "gettext" in input file.
 */
handle_gettext()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		in_gettext = TRUE;
		linenum_saved = curr_linenum;
		/*
		 * gettext will be put into default domain .po file
		 * curr_domain does not change for gettext.
		 */
		curr_domain[0] = NULL;
	}

} /* handle_gettext */

/*
 * Handler for "dgettext" in input file.
 */
handle_dgettext()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		in_dgettext = TRUE;
		linenum_saved = curr_linenum;
		/*
		 * dgettext will be put into domain file specified.
		 * curr_domain will follow.
		 */
		curr_domain[0] = NULL;
	}

} /* handle_dgettext */

/*
 * Handler for "textdomain" in input file.
 */
handle_textdomain()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		in_textdomain = TRUE;
		linenum_saved = curr_linenum;
		curr_domain[0] = NULL;
	}

} /* handle_textdomain */

/*
 * Handler for '(' in input file.
 */
handle_open_paren()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		if ((in_gettext == TRUE) ||
		    (in_dgettext == TRUE) ||
		    (in_textdomain == TRUE)) {
			in_str = TRUE;
		}
	}

} /* handle_open_paren */

/*
 * Handler for ')' in input file.
 */
handle_close_paren()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		if ((in_gettext == TRUE) ||
		    (in_dgettext == TRUE) ||
		    (in_textdomain == TRUE)) {
			add_str_to_element_list (in_textdomain);
			in_str = FALSE;
			in_gettext = FALSE;
			in_dgettext = FALSE;
			in_textdomain = FALSE;
		} else if (aflg == TRUE) {
			end_ansi_string ();
		}
	}

} /* handle_close_paren */

/*
 * Handler for '\\n' in input file.
 *
 * This is a '\' followed by new line.
 * This can be treated like a new line except when this is a continuation
 * of a ANSI-C string.
 * If this is a part of ANSI string, treat the current line as a double
 * quoted string and the next line is the start of the double quoted
 * string.
 */
handle_esc_newline()
{
	if (cflg == TRUE)
		strcat (curr_line, "\\");

	curr_linenum++;

	if (in_macro == TRUE)	{
		curr_line[0] = NULL;
		return;
	}

	if (in_quote == TRUE) {
		add_qstring_to_str ();
	} else if ((in_comment == TRUE) ||
	    (is_last_comment_line == TRUE)) {
		add_line_to_comment();
	}

	curr_line[0] = NULL;


} /* handle_esc_newline */

/*
 * Handler for '"' in input file.
 */
handle_quote()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_comment == TRUE) {
		/* Do nothing */
	} else if ((in_gettext == TRUE) ||
			(in_dgettext == TRUE) ||
			(in_textdomain == TRUE)) {
		if (in_str == TRUE) {
			if (in_quote == FALSE) {
				in_quote = TRUE;
			} else {
				add_qstring_to_str();
				in_quote = FALSE;
			}
		}
	} else if (aflg == TRUE) {
		/*
		 * The quote is found outside of gettext, dgetext, and
		 * textdomain. Everytime a quoted string is found,
		 * add it to the string list.
		 * in_str stays TRUE until ANSI string ends.
		 */
		if (in_str == TRUE) {
			if (in_quote == TRUE) {
				in_quote = FALSE;
				add_qstring_to_str();
			} else {
				in_quote = TRUE;
			}
		} else {
			in_str = TRUE;
			in_quote = TRUE;
			linenum_saved = curr_linenum;
		}
	}

} /* handle_quote */

/*
 * Handler for ' ' or TAB in input file.
 */
handle_spaces ()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	}

} /* handle_spaces */

/*
 * Flattens a linked list containing ANSI string to the one string.
 */
copy_strlist_to_str (str, strlist)
	struct strlist_st	*strlist;
	char			*str;
{
	struct strlist_st	*p;

	str[0] = NULL;

	if (strlist != NULL) {
		p = strlist;
		while (p != NULL) {
			if (p->str != NULL) {
				strcat (str, p->str);
			}
			p = p->next;
		}
	}

} /* copy_strlist_to_str */

/*
 * Handler for ',' in input file.
 */
handle_comma()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		if (in_str == TRUE) {
			if (in_dgettext == TRUE) {
				copy_strlist_to_str (curr_domain, strhead);
				free_strlist (strhead);
				strhead = NULL;
			} else if (aflg == TRUE) {
				end_ansi_string();
			}
		}
	}

} /* handle_comma */

/*
 * Handler for ',' in input file.
 */
handle_character()
{
	if (cflg == TRUE)
		strcat (curr_line, yytext);

	if (in_macro == TRUE)	return;

	if (in_quote == TRUE) {
		strcat (qstring_buf, yytext);
	} else if (in_comment == FALSE) {
		if (in_str == TRUE) {
			if (aflg == TRUE) {
				end_ansi_string();
			}
		}
	}

} /* handle_character */

/*
 * Handler for ',' in input file.
 */
handle_newline()
{
	curr_linenum++;

	if (in_macro == TRUE) {
		in_macro = FALSE;
		return;
	}

	/*
	 * in_quote is always FALSE here for ANSI-C code.
	 */
	if ((in_comment == TRUE) ||
	    (is_last_comment_line == TRUE)) {
		add_line_to_comment();
	}

	curr_line[0] = NULL;

} /* handle_newline */

/*
 * Process ANSI string.
 */
end_ansi_string()
{
	if ((aflg == TRUE) &&
	    (in_str == TRUE) &&
	    (in_gettext == FALSE) &&
	    (in_dgettext == FALSE) &&
	    (in_textdomain == FALSE)) {
		add_str_to_element_list (FALSE);
		in_str = FALSE;
	}

} /* end_ansi_string */

/*
 * Initialize global variables if necessary.
 */
initialize_globals()
{
	default_domain = strdup (DEFAULT_DOMAIN);
	curr_domain[0] = NULL;
	curr_file[0] = NULL;
	qstring_buf[0] = NULL;

	/*
	 * Command line arguments variable.
	 */
	comment_tag = NULL;
	exclude_file = NULL;
	prefix = NULL;
	pathname = NULL;

} /* initialize_globals() */

/*
 * Extract only string part when read a exclude file by removing
 * keywords (e.g. msgid) and heading and trailing blanks and
 * double quotes.
 */
trim_line (line)
	char	*line;
{
	int	i, p, len;
	int	first = 0;
	int	last = 0;
	char	c;

	len = strlen (line);

	/*
	 * Find the position of the last non-whitespace character.
	 */
	i = len - 1;
	while (1) {
		c = line[i--];
		if ((c != ' ') && (c != '\n') && (c != '\t')) {
			last = ++i;
			break;
		}
	}

	/*
	 * Find the position of the first non-whitespace character
	 * by skipping "msgid" initially.
	 */
	if (strncmp ("msgid ", line, 6) == 0) {
		i = 5;
	} else {
		i = 0;
	}

	while (1) {
		c = line[i++];
		if ((c != ' ') && (c != '\n') && (c != '\t')) {
			first = --i;
			break;
		}
	}

	/*
	 * For Backward compatibility, we consider both double quoted
	 * string and non-quoted string.
	 * The double quote is removed before being stored if exists.
	 */
	if (line[first] == '"') {
		first++;
	}
	if (line[last] == '"') {
		last--;
	}

	/*
	 * Now copy the valid part of the string.
	 */
	p = first;
	for (i = 0; i <= (last-first); i++) {
		line[i] = line[p++];
	}
	line [i] = NULL;

} /* trim_line */

/*
 * Read exclude file and stores it in the global linked list.
 */
read_exclude_file()
{
	FILE	*fp;
	struct exclude_st	*tmp_excl;
	struct strlist_st	*tail;
	int			ignore_line;
	char			line [MAX_STRING_LEN];

	if ((fp = fopen(exclude_file, "r")) == NULL) {
		fprintf(stderr, "ERROR, can't open exclude file: %s\n",
				exclude_file);
		exit (2);
	}

	ignore_line = TRUE;
	while (fgets(line, MAX_STRING_LEN, fp) != NULL) {
		/*
		 * Line starting with # is a comment line and ignored.
		 * Blank line is ignored, too.
		 */
		if ((line[0] == '\n') || (line[0] == '#')) {
			continue;
		} else if (strncmp (line, "msgstr", 6) == 0) {
			ignore_line = TRUE;
		} else if (strncmp (line, "domain", 6) == 0) {
			ignore_line = TRUE;
		} else if (strncmp (line, "msgid", 5) == 0) {
			ignore_line = FALSE;
			tmp_excl = NEW_EXCLUDE();
			tmp_excl->exstr = NEW_STRLIST();
			trim_line (line);
			tmp_excl->exstr->str = strdup (line);
			tail = tmp_excl->exstr;
			/*
			 * Prepend new exclude string node to the list.
			 */
			tmp_excl->next = excl_head;
			excl_head = tmp_excl;
		} else {
			/*
			 * If more than one line of string forms msgid,
			 * append it to the string linked list.
			 */
			if (ignore_line == FALSE) {
				trim_line (line);
				tail->next = NEW_STRLIST();
				tail->next->str = strdup (line);
				tail = tail->next;
			}
		}
	} /* while */

#ifdef DEBUG
	tmp_excl = excl_head;
	while (tmp_excl != NULL) {
		printf("============================\n");
		tail = tmp_excl->exstr;
		while (tail != NULL) {
			printf("%s###\n", tail->str);
			tail = tail->next;
		}
		tmp_excl = tmp_excl->next;
	}
#endif

} /* read_exclude_file */

/*
 * Get next character from the string list containing ANSI style string.
 * This function returns three valus. (p, *m, *c).
 * p is returned by return value and, *m and *c are returned by changing
 * values in the location pointed.
 *
 *  p : points node in the linked list for ANSI string.
 *	Each node contains double quoted string.
 *  m : The location of the next characters in the double quoted string
 *	as integer index in the string.
 *	When it gets to end of quoted string, the next node will be
 *	read and m starts as zero for every new node.
 *  c : Stores the value of the characterto be returned.
 */
struct strlist_st *
get_next_ch (p, m, c)
	struct strlist_st	*p;
	int			*m;
	char			*c;
{
	char	ch, oct, hex;
	int	value, i;

	/*
	 * From the string list, find non-null string first.
	 */
	while (1) {
		if (p == NULL) {
			break;
		} else if (p->str == NULL)  {
			p = p->next;
		} else if (p->str[*m] == NULL) {
			p = p->next;
			*m = 0;
		} else {
			break;
		}
	}

	/*
	 * No more character is available.
	 */
	if (p == NULL) {
		*c = 0;
		return (NULL);
	}

	/*
	 * Check if the character back slash.
	 * If yes, ANSI defined escape sequence rule is used.
	 */
	if (p->str[*m] != '\\') {
		*c = p->str[*m];
		*m = *m + 1;
		return (p);
	} else {
		/*
		 * Get next character after '\'.
		 */
		*m = *m + 1;
		ch = p->str[*m];
		switch (ch) {
		case 'a':
			*c = '\a';
			break;
		case 'b':
			*c = '\b';
			break;
		case 'f':
			*c = '\f';
			break;
		case 'n':
			*c = '\n';
			break;
		case 'r':
			*c = '\r';
			break;
		case 't':
			*c = '\t';
			break;
		case 'v':
			*c = '\v';
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
			/*
			 * Get maximum of three octal digits.
			 */
			value = ch;
			for (i = 0; i < 2; i++) {
				*m = *m + 1;
				oct = p->str[*m];
				if ((oct >= '0') && (oct <= '7')) {
					value = value * 8 + (oct - '0');
				} else {
					*m = *m - 1;
					break;
				}
			}
			*c = value;
#ifdef DEBUG
			/* fprintf(stderr, "octal=%d\n", value); */
#endif
			break;
		case 'x':
			value = 0;
			/*
			 * Remove all heading zeros first and
			 * get one or two valuid hexadecimal charaters.
			 */
			*m = *m + 1;
			while (p->str[*m] == '0') {
				*m = *m + 1;
			}
			value = 0;
			for (i = 0; i < 2; i++) {
				hex = p->str[*m];
				*m = *m + 1;
				if (isdigit(hex)) {
					value = value * 16 + (hex - '0');
				} else if (isxdigit(hex)) {
					hex = tolower(hex);
					value = value * 16 + (hex - 'a' + 10);
				} else {
					*m = *m - 1;
					break;
				}
			}
			*c = value;
#ifdef DEBUG
			fprintf(stderr, "hex=%d\n", value);
#endif
			*m = *m - 1;
			break;
		default :
			/*
			 * Undefined by ANSI.
			 * Just ignore "\".
			 */
			*c = p->str[*m];
			break;
		}
		/*
		 * Advance pointer to point the next character to be parsed.
		 */
		*m = *m + 1;
		return (p);
	}

} /* get_next_ch */

/*
 * Compares two msgids.
 * Comparison is done by values, not by characters represented.
 * For example, '\t', '\011' and '0x9' are identical values.
 * Return values are same as in strcmp.
 *   1   if  msgid1 > msgid2
 *   0   if  msgid1 = msgid2
 *  -1   if  msgid1 < msgid2
 */
msgidcmp (id1, id2)
	struct strlist_st	*id1, *id2;
{
	char	c1, c2;
	int	m1, m2;

	m1 = 0;
	m2 = 0;
	while (1) {
		id1 = get_next_ch (id1, &m1, &c1);
		id2 = get_next_ch (id2, &m2, &c2);

		if ((c1 == 0) && (c2 == 0)) {
			return (0);
		}

		if (c1 > c2) {
			return (1);
		} else if (c1 < c2) {
			return (-1);
		}
	}

} /* msgidcmp */

/*
 * Check if a ANSI string (which is a linked list itself) is a duplicate
 * of any string in the list of ANSI string.
 */
isduplicate (list, str)
	struct element_st	*list;
	struct strlist_st	*str;
{
	struct element_st	*p;

	if (list == NULL) {
		return (FALSE);
	}

	p = list;
	while (p != NULL) {
		if (p->msgid != NULL) {
			if (msgidcmp (p->msgid, str) == 0) {
				return (TRUE);
			}
		}
		p = p->next;
	}

	return (FALSE);

} /* isduplicate */

/*
 * Extract a comment line and add to the linked list containing
 * comment block.
 * Each comment line is stored in the node.
 */
add_line_to_comment ()
{
	struct strlist_st	*tmp_str;

	tmp_str = NEW_STRLIST();
	tmp_str->str = strdup (curr_line);
	tmp_str->next = NULL;

	if (commhead == NULL) {
		/* Empty comment list */
		commhead = tmp_str;
		commtail = tmp_str;
	} else {
		/* append it to the list */
		commtail->next = tmp_str;
		commtail = commtail->next;
	}

	is_last_comment_line = FALSE;

} /* add_line_to_comment */

/*
 * Add a double quoted string to the linked list containing ANSI string.
 */
add_qstring_to_str ()
{
	struct strlist_st	*tmp_str;

	tmp_str = NEW_STRLIST();
	tmp_str->str = strdup (qstring_buf);
	tmp_str->next = NULL;

	if (strhead == NULL) {
		/* Null ANSI string */
		strhead = tmp_str;
		strtail = tmp_str;
	} else {
		/* Append it to the ANSI string linked list */
		strtail->next = tmp_str;
		strtail = strtail->next;
	}

	qstring_buf[0] = NULL;

} /* add_qstring_to_str */

/*
 * Finds the head of domain nodes given domain name.
 */
struct domain_st *
find_domain_node (dname)
	char	*dname;
{
	struct domain_st	*tmp_dom, *p;

	/*
	 * If -a option is specified everything will be written to the
	 * default domain file.
	 */
	if (aflg == TRUE) {
		if (def_dom == NULL) {
			def_dom = NEW_DOMAIN();
		}
		return (def_dom);
	}

	if ((dname == NULL) ||
	    (dname[0] == NULL) ||
	    (strcmp(dname, default_domain) == 0)) {
		if (def_dom == NULL) {
			def_dom = NEW_DOMAIN();
		}
		if (strcmp (dname, default_domain) == 0) {
			fprintf(stderr,
		"%s \"%s\" is used in dgettext of file:%s line:%d.\n",
				"Warning: default domain name",
				default_domain, curr_file, curr_linenum);
		}
		return (def_dom);
	} else {
		p = dom_head;
		while (p != NULL) {
			if (strcmp(p->dname, dname) == 0) {
				return (p);
			}
			p = p->next;
		}

		tmp_dom = NEW_DOMAIN();
		tmp_dom->dname = strdup (dname);

		if (dom_head == NULL) {
			dom_head = tmp_dom;
			dom_tail = tmp_dom;
		} else {
			dom_tail->next = tmp_dom;
			dom_tail = dom_tail->next;
		}
		return (tmp_dom);
	}

} /* find_domain_node */

/*
 * Frees the ANSI string linked list.
 */
free_strlist (ptr)
	struct strlist_st	*ptr;
{
	struct strlist_st	*p;

	p = ptr;
	ptr = NULL;
	while (p != NULL) {
		ptr = p->next;
		free (p->str);
		free (p);
		p = ptr;
	}

} /* free_strlist */

/*
 * Finds if a ANSI string is contained in the exclude file.
 */
isexcluded (strlist)
	struct strlist_st	*strlist;
{
	struct exclude_st	*p;

	p = excl_head;
	while (p != NULL) {
		if (msgidcmp(p->exstr, strlist) == 0) {
			return (TRUE);
		}
		p = p->next;
	}
	return (FALSE);

} /* isexcluded */

/*
 * Finds if a comment block is to be extracted.
 *
 * When -c option is specified, find out if comment block contains
 * comment-tag as a token separated by blanks. If it does, this
 * comment block is associated with the next msgid encountered.
 * Comment block is a linked list where each node contains one line
 * of comments.
 */
isextracted (strlist)
	struct strlist_st	*strlist;
{
	struct strlist_st	*p;
	char			*first, *pc;


	p = strlist;
	while (p != NULL) {
		first = strdup(p->str);
		while ((first != NULL) && (first[0] != NULL)) {
			pc = first;
			while (1) {
				if (*pc == NULL) {
					break;
				} else if ((*pc == ' ') || (*pc == '\t')) {
					*pc++ = NULL;
					break;
				}
				pc++;
			}
			if (strcmp (first, comment_tag) == 0) {
				return (TRUE);
			}
			first = pc;
		}
		p = p->next;
	} /* while */

	/*
	 * Not found.
	 */
	return (FALSE);

} /* isextracted */

/*
 * Adds ANSI string to the domain element list.
 */
add_str_to_element_list (istextdomain)
	int	istextdomain;
{
	struct element_st	*tmp_elem;
	struct element_st	*p, *q;
	struct domain_st	*tmp_dom;
	int			result;

	/*
	 * This can happen if something like gettext(USAGE) is used
	 * and it is impossible to get msgid for this gettext.
	 * Since -x option should be used in this kind of cases,
	 * it is OK not to catch msgid.
	 */
	if (strhead == NULL) {
		return;
	}

	/*
	 * The global variable curr_domain contains either NULL
	 * for default_domain or domain name for dgettext().
	 */
	tmp_dom = find_domain_node (curr_domain);

	/*
	 * If this msgid is in the exclude file,
	 * then free the linked list and return.
	 */
	if ((istextdomain == FALSE) &&
	    (isexcluded (strhead) == TRUE)) {
		free_strlist (strhead);
		strhead = strtail = NULL;
		return;
	}

	tmp_elem = NEW_ELEMENT();
	tmp_elem->msgid = strhead;
	tmp_elem->istextdomain = istextdomain;
	/*
	 * If -c option is specified and TAG matches,
	 * then associate the comment to the next [d]gettext() calls
	 * encountered in the source code.
	 * textdomain() calls will not have any effect.
	 */
	if (istextdomain == FALSE) {
		if ((cflg == TRUE) && (commhead != NULL)) {
			if (isextracted (commhead) == TRUE) {
				tmp_elem->comment = commhead;
			} else {
				free_strlist (commhead);
			}
		}
		commhead = commhead = NULL;
	}

	tmp_elem->linenum = linenum_saved;
	tmp_elem->fname = strdup (curr_file);


	if (sflg == TRUE) {
		/*
		 * If this is textdomain() call and -s option is specified,
		 * append this node to the textdomain linked list.
		 */
		if (istextdomain == TRUE) {
			if (tmp_dom->textdomain_head == NULL) {
				tmp_dom->textdomain_head = tmp_elem;
				tmp_dom->textdomain_tail = tmp_elem;
			} else {
				tmp_dom->textdomain_tail->next = tmp_elem;
				tmp_dom->textdomain_tail = tmp_elem;
			}
			strhead = strtail = NULL;
			return;
		}

		/*
		 * Insert the node to the properly sorted position.
		 */
		q = NULL;
		p = tmp_dom->gettext_head;
		while (p != NULL) {
			result = msgidcmp (strhead, p->msgid);
			if (result == 0) {
				/*
				 * Duplicate id. Do not store.
				 */
				free_strlist (strhead);
				strhead = strtail = NULL;
				return;
			} else if (result > 0) {
				/* move to the next node */
				q = p;
				p = p->next;
			} else {
				tmp_elem->next = p;
				if (q != NULL) {
					q->next = tmp_elem;
				} else {
					tmp_dom->gettext_head = tmp_elem;
				}
				strhead = strtail = NULL;
				return;
			}
		} /* while */

		/*
		 * New msgid is the largest or empty list.
		 */
		if (q != NULL) {
			/* largest case */
			q->next = tmp_elem;
		} else {
			/* empty list */
			tmp_dom->gettext_head = tmp_elem;
		}
	} else {
		/*
		 * Check if this msgid is already in the same domain.
		 */
		if (tmp_dom != NULL) {
			if (isduplicate (tmp_dom->gettext_head,
					tmp_elem->msgid) == TRUE) {
				tmp_elem->isduplicate = TRUE;
			}
		}
		/*
		 * If -s option is not specified, then everything
		 * is stored in gettext linked list.
		 */
		if (tmp_dom->gettext_head == NULL) {
			tmp_dom->gettext_head = tmp_elem;
			tmp_dom->gettext_tail = tmp_elem;
		} else {
			tmp_dom->gettext_tail->next = tmp_elem;
			tmp_dom->gettext_tail = tmp_elem;
		}
		strhead = strtail = NULL;
		return;
	}

	strhead = strtail = NULL;

} /* add_str_to_element_list */

/*
 * Write all domain linked list to the files.
 */
write_all_files()
{
	struct domain_st	*tmp;

	/*
	 * Write out default domain file.
	 */
	write_one_file (def_dom);

	/*
	 * If dgettext() exists and -a option is not used,
	 * then there are non-empty linked list.
	 */
	tmp = dom_head;
	while (tmp != NULL) {
		write_one_file (tmp);
		tmp = tmp->next;
	}

} /* write_all_files */

/*
 * Writes one domain list to the file.
 */
write_one_file(head)
	struct domain_st	*head;
{
	FILE			*fp;
	char			fname [MAX_PATH_LEN];
	char			dname [MAX_DOMAIN_LEN];
	struct element_st	*p;

	/*
	 * If head is NULL, then it still has to create .po file
	 * so that it will guarantee that the previous .po file was
	 * alwasys deleted.
	 * This is why checking NULL pointer has been moved to after
	 * creating  .po file.
	 */

	/*
	 * If domain name is NULL, it is the default domain list.
	 * The domain name is either "messages" or specified by option -d.
	 * The default domain name is contained in default_domain variable.
	 */
	dname[0] = NULL;
	if ((head != NULL) &&
	    (head->dname != NULL)) {
		strcpy (dname, head->dname);
	} else {
		strcpy (dname, default_domain);
	}

	/*
	 * path is the current directory if not specified by option -p.
	 */
	fname[0] = 0;
	if (pflg == TRUE) {
		strcat (fname, pathname);
		strcat (fname, "/");
	}
	strcat (fname, dname);
	strcat (fname, ".po");

	if ((fp = fopen(fname, "w")) == NULL) {
		fprintf(stderr,
			"ERROR, can't open output file: %s\n", fname);
		exit (2);
	}

	fprintf(fp, "domain \"%s\"\n", dname);

	/* See comments above in the beginning of this function */
	if (head == NULL) return;

	/*
	 * There are separate storage for textdomain() calls if
	 * -s option is used (textdomain_head linked list).
	 * Otherwise, textdomain() is mixed with gettext(0 and dgettext().
	 * If mixed, the boolean varaible istextdomain is used to see
	 * if the current node contains textdomain() or [d]gettext().
	 */
	if (sflg == TRUE) {
		p = head->textdomain_head;
		while (p != NULL) {
			/*
			 * textdomain output line already contains
			 * FIle name and line number information.
			 * Therefore, does not have to check for nflg.
			 */
			output_textdomain (fp, p);
			p = p->next;
		}
	}

	p = head->gettext_head;
	while (p != NULL) {

		/*
		 * Comment is printed only if -c is used and
		 * associated with gettext or dgettext.
		 * textdomain is not associated with comments.
		 */
		if ((cflg == TRUE) &&
		    (p->istextdomain != TRUE)) {
			output_comment (fp, p->comment);
		}

		/*
		 * If -n is used, then file number and line number
		 * information is printed.
		 * In case of textdomain(), this information is redundant
		 * and is not printed.
		 */
		if ((nflg == TRUE) && (p->istextdomain == FALSE)) {
			fprintf(fp, "# File:%s, line:%d\n",
					p->fname, p->linenum);
		}

		/*
		 * Depending on the type of node, output textdomain comment
		 * or msgid.
		 */
		if ((sflg == FALSE) &&
		    (p->istextdomain == TRUE)) {
			output_textdomain(fp, p);
		} else {
			output_msgid (fp, p->msgid, p->isduplicate);
		}
		p = p->next;

	} /* while */

	fclose(fp);

} /* write_onefiles */

/*
 * Prints out textdomain call as a comment line with file name and
 * the line number information.
 */
output_textdomain (fp, p)
	FILE			*fp;
	struct element_st	*p;
{

	if (p == NULL) return;

	/*
	 * Write textdomain() line as a comment.
	 */
	fprintf(fp, "# File:%s, line:%d, textdomain(\"%s\");\n",
		p->fname, p->linenum,  p->msgid->str);

} /* output_textdomain */

/*
 * Prints out comments from linked list.
 */
output_comment (fp, p)
	FILE			*fp;
	struct strlist_st	*p;
{
	if (p == NULL) return;

	/*
	 * Write comment section.
	 */
	while (p != NULL) {
		fprintf(fp, "# %s\n", p->str);
		p = p->next;
	}

} /* output_comment */

/*
 * Prints out msgid along with msgstr.
 */
output_msgid (fp, p, duplicate)
	FILE			*fp;
	struct strlist_st	*p;
	int			duplicate;
{
	struct strlist_st	*q;

	if (p == NULL) return;

	/*
	 * Write msgid section.
	 * If duplciate flag is ON, prepend "# " in front of every line
	 * so that they are considered as comment lines in .po file.
	 */
	if (duplicate == TRUE) {
		fprintf(fp, "# ");
	}
	fprintf(fp, "msgid  \"%s\"\n", p->str);
	q = p->next;
	while (q != NULL) {
		if (duplicate == TRUE) {
			fprintf(fp, "# ");
		}
		fprintf(fp, "       \"%s\"\n", q->str);
		q = q->next;
	}

	/*
	 * Write msgstr section.
	 * if -m option is specified, prepend <prefix> to msgid.
	 */
	if (duplicate == TRUE) {
		fprintf(fp, "# ");
	}
	if (mflg == TRUE) {
		fprintf(fp, "msgstr \"%s%s\"\n", prefix, p->str);
		q = p->next;
		while (q != NULL) {
			if (duplicate == TRUE) {
				fprintf(fp, "# ");
			}
			fprintf(fp, "       \"%s\"\n", q->str);
			q = q->next;
		}
	} else {
		fprintf(fp, "msgstr\n");
	}

} /* output_msgid */

/*
 * Malloc a new element node and initialize fields.
 */
struct element_st *NEW_ELEMENT()
{
	struct element_st *tmp;

	tmp = (struct element_st *) malloc (sizeof (struct element_st));
	tmp->istextdomain = FALSE;
	tmp->isduplicate = FALSE;
	tmp->msgid = NULL;
	tmp->comment = NULL;
	tmp->fname = NULL;
	tmp->linenum = NULL;
	tmp->next = NULL;

	return (tmp);

} /* NEW_ELEMENT */

/*
 * Malloc a new domain node and initialize fields.
 */
struct domain_st *NEW_DOMAIN()
{
	struct domain_st *tmp;

	tmp = (struct domain_st *)malloc(sizeof (struct domain_st));
	tmp->dname = NULL;
	tmp->gettext_head = NULL;
	tmp->gettext_tail = NULL;
	tmp->textdomain_head = NULL;
	tmp->textdomain_tail = NULL;
	tmp->next = NULL;

	return (tmp);

} /* NEW_DOMAIN */

/*
 * Malloc a new string list node and initialize fields.
 */
struct strlist_st *NEW_STRLIST()
{
	struct strlist_st *tmp;

	tmp = (struct strlist_st *)malloc(sizeof (struct strlist_st));
	tmp->str = NULL;
	tmp->next = NULL;

	return (tmp);

} /* NEW_STRLIST */

/*
 * Malloc a new exclude string list node and initialize fields.
 */
struct exclude_st *NEW_EXCLUDE()
{
	struct exclude_st *tmp;

	tmp = (struct exclude_st *)malloc(sizeof (struct exclude_st));
	tmp->exstr = NULL;
	tmp->next = NULL;

	return (tmp);

} /* NEW_EXCLUDE */

#ifdef DEBUG
/*
 * Debug print routine. Compiled only with DEBUG on.
 */
print_element_list(q)
	struct element_st	*q;
{
	struct strlist_st	*r;

	while (q != NULL) {
		printf("   istextdomain = %d\n", q->istextdomain);
		printf("   isduplicate  = %d\n", q->isduplicate);
		printf("   msgid = <%s>\n", q->msgid->str);
		r = q->msgid->next;
		while (r != NULL) {
			printf("           <%s>\n", r->str);
			r = r->next;
		}

		if (q->comment == NULL) {
			printf("   comment = <NULL>\n");
		} else {
			printf("   comment = <%s>\n", q->comment->str);
			r = q->comment->next;
			while (r != NULL) {
				printf("             <%s>\n", r->str);
				r = r->next;
			}
		}

		if (q->fname == NULL) {
			printf("   fname = <NULL>\n");
		} else {
			printf("   fname = <%s>\n", q->fname);
		}
		printf("   linenum = %d\n", q->linenum);
		printf("\n");
		q = q->next;
	}
}

/*
 * Debug print routine. Compiled only with DEBUG on.
 */
print_all_domain()
{
	struct domain_st	*p;
	struct element_st	*q;

	printf("\n======= default_domain ========\n");
	p = def_dom;
	if (p != NULL) {
		if (p->dname == NULL) {
			printf("domain_name = <%s>\n", "<NULL>");
		} else {
			printf("domain_name = <%s>\n", p->dname);
		}
		q = p->gettext_head;
		print_element_list (q);

		q = p->textdomain_head;
		print_element_list (q);
	}

	printf("======= domain list ========\n");
	p = dom_head;
	while (p != NULL) {
		if (p->dname == NULL) {
			printf("domain_name = <%s>\n", "<NULL>");
		} else {
			printf("domain_name = <%s>\n", p->dname);
		}
		q = p->gettext_head;
		print_element_list (q);

		q = p->textdomain_head;
		print_element_list (q);

		p = p->next;
	} /* while */

} /* print_all_domain */
#endif
