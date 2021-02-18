#ifdef OW_I18N
#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fontset.c 1.6 91/10/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <sys/param.h>
#include <sys/file.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "mem.h"
#include "i18n.h"


#define	FS_DEF		"definition"
#define	FS_DEF_LEN	(sizeof(FS_DEF)-1)
#define	FS_ALIAS	"alias"
#define	FS_ALIAS_LEN	(sizeof(FS_ALIAS)-1)
#define	FONT_SETS	"OpenWindows.fs"

XrmDatabase	FontSetDB = NULL;

extern char	*getenv();

typedef struct _fs_cache {
	char			*locale;
	char			*name;
	XFontSet		fs;
	int			count;
	struct _fs_cache	*next;
} fs_cache;

static fs_cache	*fsc_root;

static char	*last_font_locale = NULL;

static void	parseFontSetDefaults();
static char	*get_font_set_list();
static		free_font_set_list();
static char	**construct_font_set_list();
static fs_cache	*fsc_lookup_by_name();
static fs_cache	*fsc_lookup_by_fs();
static void	fsc_enter();
static void	fsc_remove();
static char	*skip_space();
static char	*skip_space_back();

XFontSet
loadQueryFontSet(dpy, font, locale)
Display	*dpy;
char	*font;
char	*locale;
{
	char		*current_lc_ctype;
	char		**miss;
	char		*fl_str;
	char		**fl;
	char		*single_fl = NULL;
	XFontSet	fs;
	fs_cache	*fsc;
        int		missing_charset_count;
        char		*def_string; 


	if ((fsc = fsc_lookup_by_name(locale, font)) != NULL)
	{
		fsc->count++;
		return fsc->fs;
	}

	if (last_font_locale == NULL || strcmp(last_font_locale, locale) != 0)
	{
		/*
		 * Setup the new font set database for this locale.
		 */
#ifdef DEBUG
		fprintf(stderr, "New font locale -> %s\n", locale);
#endif
		if (last_font_locale != NULL)
			free(last_font_locale);
		last_font_locale = MemNewString(locale);
		parseFontSetDefaults (last_font_locale);
	}
	

	/*
	 * Make sure we are talking same locale here.  There only one
	 * way we can tell the current locale to the XCreateFontSet is
	 * via setlocale!
	 */
	if (strcmp(locale, setlocale(LC_CTYPE, NULL)) != NULL)
	{
		current_lc_ctype = MemNewString(setlocale(LC_CTYPE, NULL));
		setlocale(LC_CTYPE, locale);
	}
	else
		current_lc_ctype = NULL;

	/*
	 * First of all, try FontSetDB.
	 */
	if ((fl_str = get_font_set_list(FontSetDB, font)) != NULL)
	{
	/*	fl = construct_font_set_list(fl_str); */
		miss = NULL;
		fs = XCreateFontSet(dpy, fl_str, &miss, &missing_charset_count, &def_string);
		/* free_font_set_list(fl); */
		if (fs != NULL && miss == NULL)
			goto ret;
		if (miss && (missing_charset_count > 0))
            	     XFreeStringList(miss);
        
    		if ((missing_charset_count > 0) && fs) {
		    XFreeFontSet(dpy, fs);
		    fs = NULL;
    	        }
	}

	/*
	 * Try "font" as a whole font set.
	 */
	single_fl = (char *)malloc(strlen(font) + 3);
	sprintf(single_fl, "%s,%c",font, NULL);
	miss = NULL;
	if ((fs = XCreateFontSet(dpy, single_fl, &miss, &missing_charset_count, &def_string)) != NULL
	 && miss == NULL)
		goto ret;	
	fs = NULL;
	
	

ret:
	if (single_fl)
	    free(single_fl);
	if (miss && (missing_charset_count > 0))
            XFreeStringList(miss);
        
    	if ((missing_charset_count > 0) && fs) {
		XFreeFontSet(dpy, fs);
		fs = NULL;
    	}
	if (current_lc_ctype != NULL)
	{
		setlocale(LC_CTYPE, current_lc_ctype);
		free(current_lc_ctype);
	}
	if (fs != NULL)
		fsc_enter(locale, font, fs);

	return fs;
}


freeFontSet(dpy, fs)
Display		*dpy;
XFontSet	fs;
{
	register fs_cache	*fsc;

	if ((fsc = fsc_lookup_by_fs(fs)) == NULL)
	{
		fprintf(stderr, "Freeing none cached font set\n");
		return;
	}

	if (--(fsc->count) <= 0)
		fsc_remove(dpy, fsc);
}


/*
 * parseFontSetDefaults - get locale specific font set name settings
 * from Font set name default file.  The FontSetDB is standalone DB,
 * which will not merge with others.
 */
static void
parseFontSetDefaults(locale)
char	*locale;
{
	char		filename[1024];
        char            *openWinPath;
	Bool		notFound = False;

        /* look for application default file */
	if ((openWinPath = getenv("OPENWINHOME")) != NULL)
	{
		(void) sprintf(filename, "%s/lib/locale/%s/OW_FONT_SETS/%s",
			openWinPath, locale, FONT_SETS);
#ifdef DEBUG
		fprintf(stderr, "Try to open the FontSetDB [%s]\n", filename);
#endif
		if (access(filename, R_OK) != 0)
			notFound = True;
	}
        else
		notFound = True;

	if (notFound)
	{
        	(void) sprintf(filename, "/usr/lib/X11/app-defaults/%s/%s",
			locale, FONT_SETS);
	}

	if (FontSetDB != NULL)
		XrmDestroyDatabase(FontSetDB);
	if ((FontSetDB = XrmGetFileDatabase(filename)) == NULL
					&& strcmp(locale, "C") != 0)
		fprintf(stderr, gettext("\
olwm: Warning: Could not find the font set DB for the '%s' locale.\n"),
			locale);
}


static char *
parse_font_list(db, list, count)
XrmDatabase		db;
register char		*list;
int			count;
{
	XrmValue	xrm_result;
	char		*key, *type;

	/* Enforce a limit of 15 recursions */
	if (count > 15)
		return NULL;

#ifdef DEBUG
	fprintf(stderr, "list -> [%s]\n", list);
#endif
	if (strncmp(list, FS_DEF, FS_DEF_LEN) == 0) 
	{
		if ((key = strchr(list, ',')) != NULL)
		{
			key = skip_space(key + 1);
			return key;
		}
	}
	else if (strncmp(list, FS_ALIAS, FS_ALIAS_LEN) == 0)
	{
		xrm_result.size = 0;
		xrm_result.addr = NULL;
		if ((key = strchr(list, ',')) != NULL)
		{
			key = skip_space(key + 1);
			if (XrmGetResource(db, key, key, &type, &xrm_result)) 
				return parse_font_list(db,
						xrm_result.addr, count++);
		}
	}
#ifdef DEBUG
	fprintf(stderr, "There are no such key\n");
#endif
	return NULL;
}


/* 
 * Given a database handle and a font set specfier, return a comma
 * separated list of fonts if a font set definition is found. 
 * If not, return NULL.
 */
static char *
get_font_set_list(db, key)
XrmDatabase	db;
char		*key;
{
	XrmValue	xrm_result;
	char		*type;

	if ((db == NULL) || (key == NULL))
		return (NULL);

	xrm_result.size = 0;
	xrm_result.addr = NULL;

#ifdef DEBUG
	fprintf(stderr, "Looking for key [%s] in FontSetDB...\n", key);
#endif
	if (XrmGetResource(db, key, key, &type, &xrm_result) == True) 
		return(parse_font_list(db, xrm_result.addr, 0));
#ifdef DEBUG
	fprintf(stderr, "There are no such key\n");
#endif
	return NULL;
}


static
free_font_set_list(list)
char	**list;
{
	register int	i;

	for (i = 0; list[i] != NULL; i++)
		free(list[i]);
	free(list);
}


static char **
construct_font_set_list(str)
char    *str;
{
	register char	*p1, *p2;
	register int	i, j;
	char		**list;
	int		count;

	if (str == NULL)
		return(NULL);

	for (count = 0, p1 = str; *p1 != NULL; count++, p1++)
		if ((p1 = strchr(p1, ',')) == NULL)
			break;

	count += 2 /* one for last part, another one for the NULL */;
	list = (char **) MemAlloc(count * sizeof(char *));

	for (i = 0, p2 = p1 = str; (p2 = strchr(p1, ',')) != NULL; i++)
	{
		p1 = skip_space(p1);
		j = skip_space_back(p2 - 1) - p1 + 1;
		list[i] = (char *) MemAlloc(j + 1);
		strncpy(list[i], p1, j);
		list[i][j] = '\0';
		p1 = skip_space(p2 + 1);
	}

	list[i++] = MemNewString(skip_space(p1));
	list[i] = NULL;

#ifdef DEBUG
fprintf(stderr, "font list has been constructed from [%s] - %d\n", str, count);
for (i = 0; list[i] != NULL; i++)
	fprintf(stderr, "\t%d: [%s]\n", i, list[i]);
#endif

	return(list);
}


static char *
skip_space(p)
register char	*p;
{
	while (isspace(*p))
		p++;
	return p;
}

static char *
skip_space_back(p)
register char	*p;
{
	while (isspace(*p))
		p--;
	return p;
}


static fs_cache *
fsc_lookup_by_name(locale, name)
char	*locale;
char	*name;
{
	register fs_cache	*fsc;

	for (fsc = fsc_root; fsc != NULL; fsc = fsc->next)
		if (strcmp(fsc->locale, locale) == 0
		 && strcmp(fsc->name, name) == 0)
			break;
	return fsc;
}


static fs_cache *
fsc_lookup_by_fs(fs)
XFontSet	fs;
{
	register fs_cache	*fsc;

	for (fsc = fsc_root; fsc != NULL; fsc = fsc->next)
		if (fsc->fs == fs)
			break;
	return fsc;
}


static void
fsc_enter(locale, name, fs)
char		*locale;
char		*name;
XFontSet	fs;
{
	register fs_cache	*new;

	new = MemNew(fs_cache);
	new->locale = MemNewString(locale);
	new->name = MemNewString(name);
	new->count = 1;
	new->fs = fs;
	new->next = fsc_root;
	fsc_root = new;
}


static void
fsc_remove(dpy, afsc)
Display			*dpy;
register fs_cache	*afsc;
{
	register fs_cache	*fsc1, *fsc2;

	MemFree(afsc->locale);
	MemFree(afsc->name);
	XFreeFontSet(dpy, afsc->fs);
	/*
	 * Typically, we should free the "RM_FONTVAR->fsb", but our
	 * current font set library(libmltext) just returning the part
	 * of "RM_FONTVAR->fs", hence we can not.
	 * XFree(RM_FONTVAR->fsb);
	 */
	
	fsc2 = NULL;
	for (fsc1 = fsc_root; fsc1 != NULL; fsc1 = fsc1->next)
	{
		if (fsc1 == afsc)
		{
			if (fsc2 == NULL)
				fsc_root = afsc->next;
			else
				fsc2->next = afsc->next;
			MemFree(afsc);
			return;
		}
		fsc2 = fsc1;
	}
	fprintf(stderr, "Removing none existed font cache\n");
}
	
#endif /*OW_I18N*/
