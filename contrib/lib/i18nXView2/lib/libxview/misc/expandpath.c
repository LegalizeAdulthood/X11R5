#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)expandpath.c 50.5 90/10/16 SMI";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Sun Microsystems, Inc.
 */

/*-

 Handles:
   ~/ => home dir
   ~user/ => user's home dir
   If the environment variable a = "foo" and b = "bar" then:
	$a	=>	foo
	$a$b	=>	foobar
	$a.c	=>	foo.c
	xxx$a	=>	xxxfoo
	${a}!	=>	foo!
	\$a	=>	\$a

 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <ctype.h>
#ifndef SVR4
#include <strings.h>
#else SVR4
#include <string.h>
#endif SVR4
#ifdef OW_I18N
#include <xview/xv_i18n.h>
#include <widec.h>
#endif


/* input name in nm, pathname output to buf. */

void
expand_path(nm, buf)
    register char  *nm, *buf;
{
    register char  *s, *d;
    char            lnm[MAXPATHLEN];
    int             q;
    register char  *trimchars = "\n \t";
    char           *getenv();

    /* Strip off leading & trailing whitespace and cr */
    while (index(trimchars, *nm) != NULL)
	nm++;
    s = nm + (q = strlen(nm)) - 1;
    while (q-- && index(trimchars, *s) != NULL)
	*s = '\0';

    s = nm;
    d = lnm;
    q = nm[0] == '\\' && nm[1] == '~';

    /* Expand inline environment variables */
    while (*d++ = *s) {
	if (*s == '\\') {
	    if (*(d - 1) = *++s) {
		s++;
		continue;
	    } else
		break;
	} else if (*s++ == '$') {
	    register char  *start = d;
	    register        braces = *s == '{';
	    register char  *value;
	    while (*d++ = *s)
		if (braces ? *s == '}' : !isalnum(*s))
		    break;
		else
		    s++;
	    *--d = 0;
	    value = getenv(braces ? start + 1 : start);
	    if (value) {
		for (d = start - 1; *d++ = *value++;);
		d--;
		if (braces && *s)
		    s++;
	    }
	}
    }

    /* Expand ~ and ~user */
    nm = lnm;
    s = "";
    if (nm[0] == '~' && !q) {	/* prefix ~ */
	if (nm[1] == '/' || nm[1] == 0) {	/* ~/filename */
	    if (s = getenv("HOME")) {
		if (*++nm)
		    nm++;
	    }
	} else {		/* ~user/filename */
	    register char  *nnm;
	    register struct passwd *pw;
	    for (s = nm; *s && *s != '/'; s++);
	    nnm = *s ? s + 1 : s;
	    *s = 0;
	    pw = (struct passwd *) getpwnam(nm + 1);
	    if (pw == 0) {
		*s = '/';
		s = "";
	    } else {
		nm = nnm;
		s = pw->pw_dir;
	    }
	}
    }
    d = buf;
    if (*s) {
	while (*d++ = *s++);
	*(d - 1) = '/';
    }
    s = nm;
    while (*d++ = *s++);
}

#ifdef OW_I18N
void
expand_path_wc(nm, buf)
    register CHAR  *nm, *buf;

{
#ifdef notdef /* Not implemented yet. */

    register CHAR  *s, *d;
    CHAR            lnm[MAXPATHLEN];
    int             q;
    register CHAR  *trimchars = "\n \t";
    CHAR           *GETENV();
    char	    name_mb[MAXPATHLEN];
    CHAR    	    pw_dir_wc[MAXPATHLEN];	


    /* Strip off leading & trailing whitespace and cr */
    while (INDEX(trimchars, *nm) != NULL)
	nm++;
    s = nm + (q = STRLEN(nm)) - 1;
    while (q-- && INDEX(trimchars, *s) != NULL)
	*s = '\0';

    s = nm;
    d = lnm;
    q = nm[0] == '\\' && nm[1] == '~';

    /* Expand inline environment variables */
    while (*d++ = *s) {
	if (*s == '\\') {
	    if (*(d - 1) = *++s) {
		s++;
		continue;
	    } else
		break;
	} else if (*s++ == '$') {
	    register CHAR  *start = d;
	    register        braces = *s == '{';
	    register CHAR  *value;
	    	
	    while (*d++ = *s)
		if (braces ? *s == '}' : !(iswalpha(*s) && iswdigit(*s)))
		    break;
		else
		    s++;
	    *--d = 0;
	    value = GETENV(braces ? start + 1 : start);
	    if (value) {
		for (d = start - 1; *d++ = *value++;);
		d--;
		if (braces && *s)
		    s++;
	    }
	}
    }

    /* Expand ~ and ~user */
    nm = lnm;
    s = "";
    if (nm[0] == '~' && !q) {	/* prefix ~ */
	if (nm[1] == '/' || nm[1] == 0) {	/* ~/filename */
	    if (s = GETENV("HOME")) {
		if (*++nm)
		    nm++;
	    }
	} else {		/* ~user/filename */
	    register CHAR  *nnm;
	    register struct passwd *pw;
	    for (s = nm; *s && *s != '/'; s++);
	    nnm = *s ? s + 1 : s;
	    *s = 0;
	    name_mb[0] = NULL;
	    wstostr(nm+1, name_mb);
	    pw = (struct passwd *) getpwnam(name_mb);
	    if (pw == 0) {
		*s = '/';
		s = "";
	    } else {
		nm = nnm;
		pw_dir_wc[0] = NULL;
		strtows(pw->pw_dir, pw_dir_wc);
		s = pw_dir_wc;
	    }
	}
    }
    d = buf;
    if (*s) {
	while (*d++ = *s++);
	*(d - 1) = '/';
    }
    s = nm;
    while (*d++ = *s++);
#endif  /* notdef */    
}
#endif
