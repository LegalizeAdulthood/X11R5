/* Copyright (c) 1991 Sun Microsystems */
/* All Rights Reserved */

#ident  "@(#)gettext.c  1.0     04/11/91 "

#ifdef __STDC__
#pragma weak bindtextdomain = _bindtextdomain
#pragma weak textdomain = _textdomain
#pragma weak gettext = _gettext
#pragma weak dgettext = _dgettext
#endif

#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/file.h>
#include <sys/stat.h>

#define	DEFAULT_DOMAIN		"messages"
#define	DEFAULT_BINDING		"/usr/lib/locale"
#define	BINDINGLISTDELIM	':'
#define	MAX_DOMAIN_LENGTH	257 /* 256 + Null terminator */
#define	MAX_MSG			64

#define	LEAFINDICATOR		-99 /* must match with msgfmt.h */

struct domain_binding {
	char	*domain;	/* domain name */
	char	*binding;	/* binding directory */
	struct	domain_binding *next;
};

struct struct_mo_info {
	int	message_mid;		/* middle message id */
	int	message_count;		/* Total no. of messages */
	int	string_count_msgid;	/* total msgid length */
	int	string_count_msg;	/* total msgstr length */
	int	message_struct_size;	/* size of message_struct_size */
};

struct message_struct {
	int	less;		/* index of left leaf */
	int	more;		/* index of right leaf */
	int	msgid_offset;	/* msgid offset in mo file. */
	int	msg_offset;	/* msgstr offset in mo file */
};

struct message_so {
	char 	*path;			/* name of message shared object */
	int	fd;			/* file descriptor */
	struct struct_mo_info *mess_file_info;
					/* information of message file */
	struct message_struct *message_list;
					/* message list */
	char	*msg_ids;		/* actual message ids */
	char	*msgs;			/* actual messages */
};

#ifdef __STDC__
char * _textdomain (const char *);
char * _bindtextdomain (const char *, const char *);
char * _dgettext (const char *, const char *);
char * _gettext (const char *);
#else
char * textdomain ();
char * bindtextdomain ();
char * dgettext ();
char * gettext ();
#endif

#ifdef __STDC__
static char	*key_2_text(struct message_so *, const char *);
#else
static char	*key_2_text();
#endif

static struct domain_binding *defaultbind = 0;
static struct domain_binding *firstbind = 0;

/*
 * Build path.
 * This does simple insertion of fixed string "LC_MESSAGES" and ".mo".
 * <predicate>/<locale>/LC_MESSAGES/<domain>.mo
 */
static char    *
build_path(buf, predicate, locale, domain)
	char	*buf, *predicate, *locale, *domain;
{
	strcpy(buf, predicate);
	strcat(buf, "/");
	strcat(buf, locale);
	strcat(buf, "/LC_MESSAGES/");
	strcat(buf, domain);
	strcat(buf, ".mo");

} /* build_path */

/*
 * This builds initial default bindlist.
 */
static void
initbindinglist()
{
	defaultbind =
	(struct domain_binding *) malloc(sizeof (struct domain_binding));
	defaultbind->domain = strdup("");
	defaultbind->binding = strdup(DEFAULT_BINDING);
	defaultbind->next = 0;

} /* initbindinglist */

#ifdef __STDC__
char *
_bindtextdomain (domain, binding)
	const char	*domain;
	const char	*binding;
#else
char *
bindtextdomain (domain, binding)
	char	*domain;
	char	*binding;
#endif
{
	struct domain_binding	*bind, *prev;

	/*
	 * Initialize list
	 * If list is empty, create a default binding node.
	 */
	if (!defaultbind) {
		initbindinglist();
	}

	/*
	 * If domain is a NULL pointer, no change will occure regardless
	 * of binding value. Just return NULL.
	 */
	if (!domain) {
		return (NULL);
	}

	/*
	 * Global Binding is not supported any more.
	 * Just return NULL if domain is NULL string.
	 */
	if (*domain == '\0') {
		return (NULL);
	}

	/* linear search for binding, rebind if found, add if not */
	bind = firstbind;
	prev = 0;	/* Two pointers needed for pointer operations */

	while (bind) {
		if (!strcmp(domain, bind->domain)) {
			/*
			 * Domain found.
			 * If binding is NULL, then Query
			 */
			if (!binding) {
				return (bind->binding);
			}

			/* replace existing binding with new binding */
			if (bind->binding) {
				free(bind->binding);
			}
			bind->binding = strdup(binding);
#ifdef DEBUG
			printlist();
#endif
			return (bind->binding);
		}
		prev = bind;
		bind = bind->next;
	} /* while (bind) */

	/* domain has not been found in the list at this point */
	if (binding) {
		/*
		 * domain is not found, but binding is not NULL.
		 * Then add a new node to the end of linked list.
		 */
		bind = (struct domain_binding *)
			malloc(sizeof (struct domain_binding));
		bind->domain = strdup(domain);
		bind->binding = strdup(binding);
		bind->next = 0;
		if (prev) {
			/* reached the end of list */
			prev->next = bind;
		} else {
			/* list was empty */
			firstbind = bind;
		}
#ifdef DEBUG
		printlist();
#endif
		return (bind->binding);
	} else {
		/* Query of domain which is not found in the list */
		return (defaultbind->binding);
	} /* if (binding) */

	/* Must not reach here */

} /* bindtextdomain */

#ifdef DEBUG
printlist()
{
	struct domain_binding	*ppp;

	fprintf(stderr, "===Printing default list and regural list\n");
	fprintf(stderr, "   Default domain=<%s>, binding=<%s>\n",
		defaultbind->domain, defaultbind->binding);

	ppp = firstbind;
	while (ppp) {
		fprintf(stderr, "   domain=<%s>, binding=<%s>\n",
			ppp->domain, ppp->binding);
		ppp = ppp->next;
	}
}
#endif


/*
 * textdomain() sets or queries the name of the current domain of
 * the active LC_MESSAGES locale category.
 */
#ifdef __STDC__
char *
_textdomain (domain)
	const char	*domain;
#else
char *
textdomain (domain)
	char		*domain;
#endif
{

	static char	current_domain[MAX_DOMAIN_LENGTH + 1] = DEFAULT_DOMAIN;

	/* Query is performed for NULL domain pointer */
	if (domain == NULL) {
		return (current_domain);
	}

	/* check for error. */
	if (strlen(domain) > (unsigned int) MAX_DOMAIN_LENGTH) {
		return (NULL);
	}

	/*
	 * Calling textdomain() with a null domain string sets
	 * the domain to the default domain.
	 * If non-null string is passwd, current domain is changed
	 * to the new domain.
	 */
	if (*domain == '\0') {
		strcpy(current_domain, DEFAULT_DOMAIN);
	} else {
		strcpy(current_domain, domain);
	}

	return (current_domain);
} /* textdomain */


/*
 * gettext() is a pass-thru to dgettext() with a NULL pointer passed
 * for domain.
 */
#ifdef __STDC__
char *
_gettext (msg_id)
	const char	*msg_id;
#else
char *
gettext (msg_id)
	char		*msg_id;
#endif
{
	char *return_str;

#ifdef __STDC__
	return_str = _dgettext (NULL, msg_id);
#else
	return_str = dgettext (NULL, msg_id);
#endif
	return (return_str);
}


/*
 * In dgettext() call, domain is valid only for this call.
 */
#ifdef __STDC__
char *
_dgettext (domain, msg_id)
	const char	*domain;
	const char	*msg_id;
#else
char *
dgettext (domain, msg_id)
	char	*domain;
	char	*msg_id;
#endif
{
	char	msgfile[MAXPATHLEN];
	char	binding[MAXPATHLEN + 1], *bptr;

	char	*current_locale;
	char	*current_domain;
	char	*current_binding;

	struct stat	statbuf;
	int		fd = -1;
	caddr_t		addr;

	int		msg_inc;
	int		msg_count;
	int		path_found;
	int		errno_save = errno;
	char		*result;

	static int	top_list = 0;	/* top of message_so list */
	static int	first_free = 0;	/* first free entry in list */
	static int	last_entry_seen = -1;	/* try this one first */
	static struct message_so mess_so[MAX_MSG];
	static struct message_so cur_mess_so;	/* holds current message */

	/* setlocale never fails. */
	current_locale = setlocale(LC_MESSAGES, NULL);

	/* If C locale, return the original msgid immediately. */
	if ((current_locale[0] == 'C') &&
	    (current_locale[1] == 0)) {
		errno = errno_save;
#ifdef __STDC__
		return ((char *)msg_id);
#else
		return (msg_id);
#endif
	}

	/*
	 * Query the current domain if domain argument is NULL pointer
	 */
	if (domain == NULL) {
#ifdef __STDC__
		current_domain = _textdomain (NULL);
#else
		current_domain = textdomain (NULL);
#endif

	} else if (strlen(domain) > (unsigned int)MAX_DOMAIN_LENGTH) {
		errno = errno_save;
#ifdef __STDC__
		return ((char *)msg_id);
#else
		return (msg_id);
#endif
	} else if (*domain == '\0') {
		current_domain = DEFAULT_DOMAIN;
	} else {
#ifdef __STDC__
		current_domain = (char *)domain;
#else
		current_domain = domain;
#endif
	}

	/*
	 * Query of current binding.
	 * If there is no binding, error and just return msg_id.
	 */
#ifdef __STDC__
	if (current_binding = _bindtextdomain (current_domain, NULL)) {
#else
	if (current_binding = bindtextdomain (current_domain, NULL)) {
#endif

		/*
		 * binding is the form of "bind1:bind2:bind3:"
		 */
		while (*current_binding) {
			/* skip empty binding */
			while (*current_binding == ':') {
				current_binding++;
			}

			memset (binding, 0, sizeof (binding));
			bptr = binding;
			/* get binding */
			while ((*bptr++ = *current_binding++) &&
				(*current_binding != ':'));

			if (binding[0]) {
				build_path(msgfile, binding,
					current_locale, current_domain);
			} else {
				continue;
			}

			/*
			 * At this point, msgfile contains full path for
			 * domain.
			 * Look up cache entry first. If cache misses,
			 * then search domain look-up table.
			 */
			path_found = 0;
			if ((last_entry_seen >= 0) &&
			!strcmp (msgfile, mess_so[last_entry_seen].path)) {
				path_found = 1;
				msg_inc = last_entry_seen;
			} else {
				msg_inc = top_list;
				while (msg_inc < first_free) {
					if (!strcmp (msgfile,
						mess_so[msg_inc].path)) {
						path_found = 1;
						break;
					} /* if */
					msg_inc++;
				} /* while */
			} /* if */

			/*
			 * Even if msgfile was found in the table,
			 * It is not guaranteed to be a valid file.
			 * To be a valid file, fd must not be -1 and
			 * mmaped address (mess_file_info) must have
			 * valid contents.
			 */
			if (path_found) {
				last_entry_seen = msg_inc;
				if (mess_so[msg_inc].fd != -1 &&
				    mess_so[msg_inc].mess_file_info !=
				    (struct struct_mo_info *) - 1) {
					cur_mess_so = mess_so[msg_inc];
					/* file is valid */
					result = key_2_text(&cur_mess_so,
							msg_id);
					errno = errno_save;
					return (result);
				} else {
					/* file is not valid */
					continue;
				}
			}

			/*
			 * Been though entire table and not found.
			 * Open a new entry if there is space.
			 */
			if (msg_inc == MAX_MSG) {
				/* not found, no more space */
				errno = errno_save;
#ifdef __STDC__
				return ((char *)msg_id);
#else
				return (msg_id);
#endif
			}
			if (first_free == MAX_MSG) {
				/* no more space */
				errno = errno_save;
#ifdef __STDC__
				return ((char *)msg_id);
#else
				return (msg_id);
#endif
			}

			/*
			 * There is an available entry in the table, so make
			 * a message_so for it and put it in the table,
			 * return msg_id if message file isn't opened -or-
			 * isn't mmap'd correctly
			 */
			fd = open(msgfile, O_RDONLY);

			mess_so[first_free].fd = fd;
			mess_so[first_free].path = strdup(msgfile);

			if (fd == -1) {
				first_free++;
				continue;
			}
			fstat(fd, &statbuf);
			addr = mmap(0, statbuf.st_size, PROT_READ,
					MAP_SHARED, fd, 0);
			close(fd);

			mess_so[first_free].mess_file_info =
					(struct struct_mo_info *) addr;
			if (addr == (caddr_t) - 1) {
				first_free++;
				continue;
			}

			/* get message_list array start address */
			mess_so[first_free].message_list =
				(struct message_struct *)
				& mess_so[first_free].mess_file_info[1];

			/* find how many messages in file */
			msg_count = mess_so[first_free].mess_file_info->
							message_count;
			/* get msgid string pool start address */
			mess_so[first_free].msg_ids =
				(char *) &mess_so[first_free].
						message_list[msg_count];

			/* get msgstr string pool start address */
			mess_so[first_free].msgs = (char *)
				mess_so[first_free].msg_ids +
				mess_so[first_free].mess_file_info->
							string_count_msgid;

			cur_mess_so = mess_so[first_free];
			first_free++;

			result = key_2_text(&cur_mess_so, msg_id);
			errno = errno_save;
			return (result);

		} /* while current_binding */

	} /* if current_binding */

	errno = errno_save;
#ifdef __STDC__
	return ((char *)msg_id);
#else
	return (msg_id);
#endif
} /* dgettext */



/*
 * key_2_text() translates msd_id into target string.
 */
#ifdef __STDC__
static char *
key_2_text(messages, key_string)
	struct message_so	*messages;
	const char		*key_string;
#else
static char *
key_2_text(messages, key_string)
	struct message_so	*messages;
	char			*key_string;
#endif
{
	register int		check;
	register int		val;
	register char		*msg_id_str;
	struct message_struct	check_mess_list;

	check = (*messages).mess_file_info->message_mid;
	for (;;) {
		check_mess_list = (*messages).message_list[check];
		msg_id_str = (*messages).msg_ids
				+ check_mess_list.msgid_offset;

		/*
		 * To maintain the compatibility with Zeus mo file,
		 * msg_id's are stored in descending order.
		 * If the ascending order is desired, change "msgfmt.c"
		 * and switch msg_id_str and key_string in the following
		 * strcmp() statement.
		 */
		val = strcmp(msg_id_str, key_string);
		if (val < 0) {
			if (check_mess_list.less == LEAFINDICATOR) {
#ifdef __STDC__
				return ((char *)key_string);
#else
				return (key_string);
#endif
			} else {
				check = check_mess_list.less;
			}
		} else if (val > 0) {
			if (check_mess_list.more == LEAFINDICATOR) {
#ifdef __STDC__
				return ((char *)key_string);
#else
				return (key_string);
#endif
			} else {
				check = check_mess_list.more;
			}
		} else {
			return ((*messages).msgs + check_mess_list.msg_offset);
		} /* if ((val= ...) */
	} /* for (;;) */

} /* key_2_string */
