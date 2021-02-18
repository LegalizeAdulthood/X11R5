#ifdef OW_I18N

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)font.c 70.2 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/xv_debug.h>
#include <sys/types.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/generic.h>
#include <xview/notify.h>
#include <xview/server.h>
#include <xview/window.h>
#include <xview_private/font_impl.h>
#include <string.h>
#include <sys/param.h>
#include <xview_private/xv_i18n_impl.h>

extern Pixfont 	*xv_pf_sys;
extern char    	*defaults_get_string();
extern char    	*strcpy();
extern Xv_opaque xv_default_server;
extern Xv_opaque xv_default_display;
extern char 	*index();
extern char	*getenv();
extern char	*xv_app_name;

Xv_private void	xv_x_char_info();
Pkg_private XID	xv_load_x_font();
Pkg_private void xv_unload_x_font();


/* Macros */
#define FONT_FIND_DEFAULT_FAMILY(linfo) \
		(linfo) ? (linfo)->default_family : DEFAULT_FONT_FAMILY
#define FONT_FIND_DEFAULT_SIZE(linfo) \
		(linfo) ? (linfo)->medium_size : DEFAULT_MEDIUM_FONT_SIZE

/* Definitions for decrypting xlfd names */
#define		DASH		'-'
#define		NUMXLFDFIELDS	14
#define		FOUNDRYPOS	1
#define		FAMILYPOS	2
#define		WEIGHTPOS	3
#define		SLANTPOS	4
#define		SETWIDTHNAMEPOS	5
#define		ADDSTYLENAMEPOS	6
#define		PIXSIZEPOS	7
#define		PTSIZEPOS	8
#define		XRESOLUTIONPOS	9

/* Definition for the XView style font set name */
#define		NUMFSFIELDS	2
#define		FSFAMILYPOS	0
#define		FSSTYLEPOS	1
#define		FSSIZEPOS	2

#define DEFAULT_SMALL_FONT		"-b&h-lucida-medium-r-normal-sans-*-100-*-*-*-*-*-*"
#define DEFAULT_MEDIUM_FONT		"-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*"
#define DEFAULT_LARGE_FONT		"-b&h-lucida-medium-r-normal-sans-*-140-*-*-*-*-*-*"
#define DEFAULT_XLARGE_FONT		"-b&h-lucida-medium-r-normal-sans-*-190-*-*-*-*-*-*"

#define DEFAULT_DEFAULT_FONT_NAME	"fixed"
#define DEFAULT_FONT_NAME		"-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*"

#define DEFAULT_LARGE_FONT_SIZE		14
#define DEFAULT_SMALL_FONT_SIZE		10
#define DEFAULT_MEDIUM_FONT_SIZE	12
#define DEFAULT_XLARGE_FONT_SIZE	19
#define DEFAULT_FONT_SIZE		DEFAULT_MEDIUM_FONT_SIZE
#define DEFAULT_FONT_SCALE		(int) WIN_SCALE_MEDIUM

#define DEFAULT_FONT_FAMILY		"lucida"
#define DEFAULT_FONT_FIXEDWIDTH_FAMILY	"lucidatypewriter"
#define DEFAULT_FONT_STYLE		""
#define DEFAULT_FONT_WEIGHT		"medium"
#define DEFAULT_FONT_SLANT		"r"
#define DEFAULT_FONT_SETWIDTHNAME	"normal"
#define DEFAULT_FONT_ADDSTYLENAME	"sans"

static int      font_default_scale = DEFAULT_FONT_SCALE;
static char    *font_default_fixedwidth_family = DEFAULT_FONT_FIXEDWIDTH_FAMILY;
static char    *font_default_style = DEFAULT_FONT_STYLE;

static char    *font_default_weight = DEFAULT_FONT_WEIGHT;
static char    *font_default_slant = DEFAULT_FONT_SLANT;
static char    *font_default_setwidthname = DEFAULT_FONT_SETWIDTHNAME;
static char    *font_default_addstylename = DEFAULT_FONT_ADDSTYLENAME;

static char    *sunview1_prefix = "/usr/lib/fonts/fixedwidthfonts/";

static Font_locale_info	    *fs_locales = NULL;

Xv_private Xv_Font xv_font_with_name();
static void     font_default_font();
static char    *font_default_font_from_scale();
static char    *font_determine_font_name();
static char    *font_rescale_from_font();
static void     font_decrypt_family_style();
static void     font_decrypt_rescale_info();
static int      font_decrypt_sv1_name();
static int      font_decrypt_xv_name();
static int      font_read_attrs();
static int      font_string_compare();
static int      font_string_compare_nchars();

/* font naming compatibility */
struct font_translation {
    char           *sv1_name;
    char           *sv1_foundry;
    char           *sv1_family;
    char           *sv1_style;
    char           *sv1_weight;
    char           *sv1_slant;
    char           *sv1_setwidthname;
    char           *sv1_addstylename;

    int             size;
    int             scale;	/* medium resolution */
};

struct xv_to_x_font_translation {
    char           *xv_family;
    char           *xv_style;
    char           *x_font_foundry;
    char           *x_font_family;
    char           *x_font_style;
    char           *x_font_weight;
    char           *x_font_slant;
    char           *x_font_setwidthname;
    char           *x_font_addstylename;
};

struct font_definitions {
    char           *xv_family;
    char           *xv_style;
    char           *foundry;
    char           *family;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;

    int             small_size;	/* medium resolution */
    int             medium_size;
    int             large_size;
    int             extra_large_size;
};

struct font_return_attrs {
    char           *name;
    char           *family;
    char           *style;
    char	   *xv_family;
    char           *xv_style;
    char           *foundry;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;
    char	    *locale;
    char	    **names;
    char	    *specifier;
    int             size;
    int             small_size;
    int             medium_size;
    int             large_size;
    int             extra_large_size;
    int             scale;
    Font_info      *resize_from_font;
    int             rescale_factor;
    int             free_name, free_family, free_style, 
		    free_weight, free_slant, free_foundry, 
		    free_setwidthname, free_addstylename;
    int		    type;
};
typedef struct font_return_attrs *Font_return_attrs;

typedef struct style_definitions  {
    char	*style;
    char	*weight;
    char	*slant;
}Style_defs;

struct _unknown {
    struct _unknown *next;
    char           *name;
};
typedef struct _unknown *Unknown;

/*
 * Style defs - conversion table from style -> (weight, slant)
 * The 'real' style entry must precede the semantic entry.
 * i.e. the entry for 'bold' precedes 'FONT_STYLE_BOLD'
 * This is necessary so that when a (weight, slant) -> style
 * conversion is being done, the 'real' entry is matched first.
 */
static struct style_definitions	known_styles[] = {
	/*
	 * 'real' entries
	 */
	"normal", "medium", "r",
	"bold", "bold", "r",
	"italic", "medium", "i",
	"oblique", "medium", "o",
	"bolditalic", "bold", "i",
	"boldoblique", "bold", "o",

	/*
	 * semantic entries
	 */
	DEFAULT_FONT_STYLE, "medium", "r",
	"FONT_STYLE_NORMAL", "medium", "r",
	"FONT_STYLE_BOLD", "bold", "r",
	"FONT_STYLE_ITALIC", "medium", "i",
	"FONT_STYLE_OBLIQUE", "medium", "o",
	"FONT_STYLE_BOLD_ITALIC", "bold", "i",
	"FONT_STYLE_BOLD_OBLIQUE", "bold", "o",
	0, 0, 0
    };

/*
 * Isa
 * Added foundry, weight, slant fields
 */
static struct font_translation known_sv1fonts[] = {
    "cmr.b.8", "*", "times", "bold", "bold", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.b.14", "*", "times", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cmr.r.8", "*", "times", 0, "medium", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.r.14", "*", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cour.b.10", "*", "courier", "bold", "bold", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.b.12", "*", "courier", "bold", "bold", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.b.14", "*", "courier", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.b.16", "*", "courier", "bold", "bold", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.b.18", "*", "courier", "bold", "bold", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.b.24", "*", "courier", "bold", "bold", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "cour.r.10", "*", "courier", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.r.12", "*", "courier", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.r.14", "*", "courier", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.r.16", "*", "courier", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.r.18", "*", "courier", 0, "medium", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.r.24", "*", "courier", 0, "medium", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "gallant.r.10", "b&h", "lucidatypewriter", 0, "medium", "r", "normal", "sans", 10, (int) WIN_SCALE_SMALL,
    "gallant.r.19", "b&h", "lucidatypewriter", 0, "medium", "r", "normal", "sans", 19, (int) WIN_SCALE_LARGE,
    "serif.r.10", "*", "times", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "serif.r.11", "*", "times", 0, "medium", "r", "normal", "", 11, (int) WIN_SCALE_SMALL,
    "serif.r.12", "*", "times", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_MEDIUM,
    "serif.r.14", "*", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "serif.r.16", "*", "times", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_EXTRALARGE,
    0, 0, 0, 0, 0, 0, 0, 0, 0, (int) WIN_SCALE_MEDIUM
};

/*
 * Isa
 * Added weight, slant fields
 */
static struct xv_to_x_font_translation known_openwin_fonts[] = {
    FONT_FAMILY_OLGLYPH, "", "sun", "open look glyph", "", "", "", "", "",
    FONT_FAMILY_OLGLYPH, FONT_STYLE_NORMAL, "sun", "open look glyph", "", "", "", "", "",

    FONT_FAMILY_OLCURSOR, "", "sun", "open look cursor", "", "", "", "", "",
    FONT_FAMILY_OLCURSOR, FONT_STYLE_NORMAL, "sun", "open look cursor", "", "", "", "", "",

    FONT_FAMILY_LUCIDA, "", "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_NORMAL, "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD, "b&h", "lucida", "bold", "bold", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_ITALIC, "b&h", "lucida", "italic", "medium", "i", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD_ITALIC, "b&h", "lucida", "bolditalic", "bold", "i", "normal", "sans",

    FONT_FAMILY_SANS_SERIF, "", "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_SANS_SERIF, FONT_STYLE_NORMAL, "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_SANS_SERIF, FONT_STYLE_BOLD, "b&h", "lucida", "bold", "bold", "r", "normal", "sans",
    FONT_FAMILY_SANS_SERIF, FONT_STYLE_ITALIC, "b&h", "lucida", "italic", "medium", "i", "normal", "sans",
    FONT_FAMILY_SANS_SERIF, FONT_STYLE_BOLD_ITALIC, "b&h", "lucida", "bolditalic", "bold", "i", "normal", "sans",

    FONT_FAMILY_LUCIDA_FIXEDWIDTH, "", "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_NORMAL, "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_BOLD, "b&h", "lucidatypewriter", "bold", "bold", "r", "normal", "sans",

    /*
     * Isa
     * Added for each entry in table that had FONT_STYLE_NORMAL, an equivalent entry with
     * style = "". This is so that the matches of FONT_FAMILY = "whatever", FONT_STYLE =
     * "" are sucessful.
     */
    FONT_FAMILY_ROMAN, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_SERIF, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_CMR, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_GALLENT, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_COUR, "", "*", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_NORMAL, "*", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD, "*", "courier", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_ITALIC, "*", "courier", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_OBLIQUE, "*", "courier", "oblique", "medium", "o", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_ITALIC, "*", "courier", "bolditalic", "bold", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_OBLIQUE, "*", "courier", "boldoblique", "bold", "o", "normal", "",

    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Font families that don't have sizes/style
 * So far only FONT_FAMILY_OLCURSOR
 */
static struct xv_to_x_font_translation size_style_less[] = {
    FONT_FAMILY_OLCURSOR, "", "sun", "open look cursor", "", "", "", "", "",
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Isa
 * Added weight, slant, setwidthname, addstylename fields
 * changed lucidasans to lucida
 */
static struct font_definitions known_xvfonts[] = {
    "lucida", 0, "b&h", "lucida", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bold", "b&h", "lucida", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bolditalic", "b&h", "lucida", "bold", "i", "normal", "sans", 10, 12, 14, 19,
    "lucida", "italic", "b&h", "lucida", "medium", "i", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", 0, "b&h", "lucidatypewriter", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", "bold", "b&h", "lucidatypewriter", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char    *
normalize_font_name(name)
    register char  *name;
{
    if (name == NULL) {
	char           *default_scale;
	/*
	 * NULL for name => use default font name. Warning: Database may have
	 * "" rather than NULL.
	 */
	name = defaults_get_string("font.name", "Font.Name", NULL);
	if (name == NULL || (strlen(name) == 0)) {
	    default_scale = (char *) defaults_get_string("window.scale", 
						"Window.Scale", "Medium");
	    name = font_default_font_from_scale(default_scale);
	}
    }
    if (font_string_compare_nchars(name, sunview1_prefix,
				   strlen(sunview1_prefix)) == 0) {
	/* Map SunView1.X font name into server name. */
	name += strlen(sunview1_prefix);	/* index into str */
    }
    return (name);
}

static char    *
font_default_font_from_scale(scale)
    register char  *scale;
{
    if (!scale)
	return DEFAULT_MEDIUM_FONT;

    if ((font_string_compare(scale, "small") == 0) ||
	(font_string_compare(scale, "Small") == 0)) {
	return DEFAULT_SMALL_FONT;
    } else if ((font_string_compare(scale, "medium") == 0) ||
	       (font_string_compare(scale, "Medium") == 0)) {
	return DEFAULT_MEDIUM_FONT;
    } else if ((font_string_compare(scale, "large") == 0) ||
	       (font_string_compare(scale, "Large") == 0)) {
	return DEFAULT_LARGE_FONT;
    } else if ((font_string_compare(scale, "Extra_large") == 0) ||
	       (font_string_compare(scale, "Extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_large") == 0)) {
	return DEFAULT_XLARGE_FONT;
    } else
	return DEFAULT_MEDIUM_FONT;
}

/* Called to free the font list when server is being destroyed. */
/*ARGSUSED*/
static void
font_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Font_info *font, *next;
    register int    ref_count;

    ASSERT(key == FONT_HEAD, _svdebug_always_on);
    for (font = (Font_info *) data; font; font = next) {
	next = font->next;	/* Paranoia in case xv_destroy is immediate */
	ref_count = (int) xv_get(FONT_PUBLIC(font), XV_REF_COUNT);
	if (ref_count == 0) {
	    xv_destroy(FONT_PUBLIC(font));
#ifdef _XV_DEBUG
	} else {
	    fprintf(stderr,
		    "Font %s has %d refs but server being destroyed.\n",
		    font->name, ref_count);
#endif
	}
    }
}

/* Called to free the unknown list when server is being destroyed. */
/*ARGSUSED*/
static void
font_unknown_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Unknown unknown, next;

    ASSERT(key == FONT_UNKNOWN_HEAD, _svdebug_always_on);
    for (unknown = (Unknown) data; unknown; unknown = next) {
	next = unknown->next;
	free(unknown->name);
	free((char *) unknown);
    }
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

static char *
parse_font_list(db, list, count)
XrmDatabase		db;
register char		*list;
int			count;
{
	XrmValue	xrm_result;
	int		len;
	char		*key, *type;

	/* Enforce a limit of 15 recursions */
	if (count > 15)
		return NULL;

	if (strncmp(list, FS_DEF, FS_DEF_LEN) == 0) {
		if ((key = strchr(list, ',')) != NULL) {
			key = skip_space(key + 1);
			return key;
		}
	} else if (strncmp(list, FS_ALIAS, FS_ALIAS_LEN) == 0) {
		xrm_result.size = 0;
		xrm_result.addr = NULL;
		if ((key = strchr(list, ',')) != NULL) {
			key = skip_space(key + 1);
			if (XrmGetResource(db, key, key, &type, &xrm_result)) 
				return parse_font_list(db,
						xrm_result.addr, count++);
		}
	}
	return NULL;
}

/* 
 * Given a database handle and a font set specfier, return a comma
 * separated list of fonts if a font set definition is found. 
 * If not, return NULL.
 */
static char *
get_font_set_list(db, key)
    XrmDatabase	    db;
    char	    *key;
{
    XrmValue    xrm_result;
    char	*type;

    if ((db == NULL) || (key == NULL))
	return (NULL);

    xrm_result.size = 0;
    xrm_result.addr = NULL;

    if (XrmGetResource(db, key, key, &type, &xrm_result) == True) 
	return(parse_font_list(db, xrm_result.addr, 0));

    return(NULL);
}

static 
free_font_set_list(list)
    char    **list;
{
    register int    i;

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
	list = (char **) malloc(count * sizeof(char *));

	for (i = 0, p2 = p1 = str; (p2 = strchr(p1, ',')) != NULL; i++) {
		p1 = skip_space(p1);
		j = skip_space_back(p2 - 1) - p1 + 1;
		list[i] = (char *) malloc(j + 1);
		strncpy(list[i], p1, j);
		list[i][j] = '\0';
		p1 = skip_space(p2 + 1);
	}

	list[i++] = strdup(skip_space(p1));
	list[i] = NULL;

	return(list);
}


static char *
find_font_locale(server, avlist)
    Xv_opaque		server;
    Attr_avlist     	avlist;
{
    char    		 *locale = NULL;
    register Attr_avlist attrs;

    for (attrs = avlist; attrs && *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	    case FONT_LOCALE:
	        locale = (char *) attrs[1];
	        break;

	    default:
	        break;
	}
    }

    if (locale == NULL)
       locale = (char *)xv_get(server, XV_LC_BASIC_LOCALE);

    return(locale);
}

static void
initialize_locale_info(linfo)
    Font_locale_info	*linfo;
{
    XrmValue    xrm_result;
    char        *type;

    if (XrmGetResource(linfo->db, FS_SMALL_SIZE, FS_SMALL_SIZE, &type,
		&xrm_result) == True)
	linfo->small_size = atoi(xrm_result.addr);
    else 
	linfo->small_size = DEFAULT_SMALL_FONT_SIZE;
    
    if (XrmGetResource(linfo->db, FS_MEDIUM_SIZE, FS_MEDIUM_SIZE, &type,
		&xrm_result) == True)
	linfo->medium_size = atoi(xrm_result.addr);
    else 
	linfo->medium_size = DEFAULT_MEDIUM_FONT_SIZE;
    
    if (XrmGetResource(linfo->db, FS_LARGE_SIZE, FS_LARGE_SIZE, &type,
		&xrm_result) == True)
	linfo->large_size = atoi(xrm_result.addr);
    else 
	linfo->large_size = DEFAULT_LARGE_FONT_SIZE;
    
    if (XrmGetResource(linfo->db, FS_XLARGE_SIZE, FS_XLARGE_SIZE, &type,
		&xrm_result) == True)
	linfo->xlarge_size = atoi(xrm_result.addr);
    else 
	linfo->xlarge_size = DEFAULT_XLARGE_FONT_SIZE;

    if (XrmGetResource(linfo->db, FS_DEFAULT_FAMILY, FS_DEFAULT_FAMILY, &type,
		&xrm_result) == True)
	linfo->default_family = xrm_result.addr;
    else 
	linfo->default_family = DEFAULT_FONT_FAMILY;
}

static 
initialize_font_attrs(linfo, attrs)
Font_locale_info    *linfo;
Font_return_attrs   attrs;
{
    attrs->free_name 	    	= 
    attrs->free_foundry 	= 
    attrs->free_family 		= 
    attrs->free_style 		=
    attrs->free_weight 		= 
    attrs->free_slant 		=
    attrs->free_setwidthname 	=
    attrs->free_addstylename 	= 0;

    attrs->xv_family 	    = 
    attrs->xv_style 	    =
    attrs->locale 	    = 
    attrs->specifier 	    = 
    attrs->name 	    = 
    attrs->foundry 	    = 
    attrs->family 	    = 
    attrs->style 	    =
    attrs->weight 	    = 
    attrs->slant 	    =
    attrs->setwidthname     = 
    attrs->addstylename     = (char *)NULL;

    attrs->names = (char **)NULL;

    attrs->size		    = FONT_NO_SIZE;
    attrs->scale 	    = FONT_NO_SCALE;
    attrs->type 	    = FONT_TYPE_TEXT;
    attrs->rescale_factor    = 0;
    attrs->resize_from_font  = (Font_info *)NULL;

    if (linfo) {
        attrs->small_size  	= linfo->small_size;
        attrs->medium_size 	= linfo->medium_size;
        attrs->large_size  	= linfo->large_size;
        attrs->extra_large_size	= linfo->xlarge_size;
    } else {
        attrs->small_size  	= DEFAULT_SMALL_FONT_SIZE;
        attrs->medium_size 	= DEFAULT_MEDIUM_FONT_SIZE;
        attrs->large_size  	= DEFAULT_LARGE_FONT_SIZE;
        attrs->extra_large_size	= DEFAULT_XLARGE_FONT_SIZE;
    }
}


static int 
find_size_from_scale(linfo, scale)
    Font_locale_info    *linfo;
    int			scale;
{
    int    size;

    if (linfo == NULL)
	return(FONT_NO_SIZE);

    switch(scale) {
	case WIN_SCALE_SMALL:
	    size = linfo->small_size;
	    break;
	case WIN_SCALE_MEDIUM:
	    size = linfo->medium_size;
	    break;
	case WIN_SCALE_LARGE:
	    size = linfo->large_size;
	    break;
	case WIN_SCALE_EXTRALARGE:
	    size = linfo->xlarge_size;
	    break;
	default:
	    size = linfo->medium_size;
	    break;
    }
    return(size);
}

static int 
find_scale_from_string(str)
    char    *str;
{
    int	    scale = WIN_SCALE_MEDIUM;

    if (str != NULL) {
        if (!strcmp(str, "small") || !strcmp(str, "Small"))
	    scale = WIN_SCALE_SMALL;
        else if (!strcmp(str, "medium") || !strcmp(str, "Medium"))
	    scale = WIN_SCALE_MEDIUM;
        else if (!strcmp(str, "large") || !strcmp(str, "Large"))
	    scale = WIN_SCALE_LARGE;
        else if (!strcmp(str, "extra_large") || !strcmp(str, "extra_Large") ||
		!strcmp(str, "Extra_large") || !strcmp(str, "Extra_Large"))
	    scale = WIN_SCALE_EXTRALARGE;
    }

    return(scale);
}
    

static int 
find_size_from_defaults(linfo)
    Font_locale_info    *linfo;
{
    char    *str;
    int	    scale, size;

    str = defaults_get_string("window.scale", "Window.Scale", NULL);
    scale = find_scale_from_string(str);

    switch(scale) {
	case WIN_SCALE_SMALL:
	    size = linfo ? linfo->small_size : DEFAULT_SMALL_FONT_SIZE;

	case WIN_SCALE_LARGE:
	    size = linfo ? linfo->large_size : DEFAULT_LARGE_FONT_SIZE;

	case WIN_SCALE_EXTRALARGE:
	    size = linfo ? linfo->small_size : DEFAULT_XLARGE_FONT_SIZE;

	case WIN_SCALE_MEDIUM:
	default:
	    size = linfo ? linfo->medium_size : DEFAULT_MEDIUM_FONT_SIZE;
    }
    return(size);
}

static char *
find_family_from_attrs(attrs, linfo)
    struct font_return_attrs	*attrs;
    Font_locale_info    	*linfo;
{
    if (linfo == NULL)
	return(NULL);

    if ((attrs->xv_family == NULL) || 
		!strcmp(attrs->xv_family, FONT_FAMILY_DEFAULT))
    {
	attrs->family = linfo->default_family;
        attrs->free_family = 0;
    } else {
	 attrs->family = attrs->xv_family;
	 attrs->free_family = 0;
    }
}

static char *
find_style_from_attrs(attrs, linfo)
    struct font_return_attrs	*attrs;
    Font_locale_info    	*linfo;
{
    if (linfo == NULL)
	return(NULL);

    if ((attrs->xv_style == NULL) || 
		!strcmp(attrs->xv_style, FONT_STYLE_DEFAULT)) {
	attrs->style = "FONT_STYLE_NORMAL";
	attrs->free_style = 0;
    } else {
	attrs->style = attrs->xv_style;
	attrs->free_style = 0;
    }
}


static Font_locale_info *
find_font_locale_info(server, avlist)
    Xv_opaque           server;
    Attr_avlist         avlist;
{
    char 	    	*locale;
    Font_locale_info	*linfo;
    XrmDatabase	    	db = NULL, app_db;
    char	    	*str;
    char	    	filename[MAXPATHLEN];

    if ((locale = find_font_locale(server, avlist)) == NULL)
       return((Font_locale_info *)NULL);

    /* check list to see if font set locale info exists */
    for (linfo = fs_locales; linfo != NULL; linfo = linfo->next)
	if (!strcmp(linfo->locale, locale))
	    return(linfo);

    /* create locale specific font set database */
    if (str = getenv("OPENWINHOME")) {
       	    sprintf(filename, 
           	"%s/lib/locale/%s/OW_FONT_SETS/OpenWindows.fs",
               	str, locale);
                db = XrmGetFileDatabase(filename);
    }
 
    if (str = (char *)xv_get(server, XV_LOCALE_DIR)) {
  	sprintf(filename, "%s/%s/OW_FONT_SETS/%s\.fs", str, locale, 
	    xv_app_name);
        if (db == NULL)
	    db = XrmGetFileDatabase(filename);
        else if (app_db = XrmGetFileDatabase(filename))
            XrmMergeDatabases(app_db, &db);
    }
 
    if (db != NULL) {
        /* add to the font locale info list */
        linfo = (Font_locale_info *)malloc(sizeof(Font_locale_info));
        linfo->locale = strdup(locale);
        linfo->db = db;
        linfo->next = fs_locales;
        fs_locales = linfo;
        initialize_locale_info(linfo);
    } else 
	linfo = NULL;

    return(linfo);
}


int
font_init(parent_public, font_public, avlist)
    Xv_opaque       parent_public;
    Xv_font_struct *font_public;
    Attr_avlist     avlist;
{
    XFontSet            font_set = NULL;
    Xv_opaque           display, server;
    XFontSetExtents	*font_set_extents;
    char		**dummy; /* font_name_list */
    char                **fs_list, *base_list[2];
    char           	*font_name = NULL;
    char		*str;
    Font_locale_info    *linfo = NULL;
    short               free_fs_list = FALSE, free_font_name = FALSE;
    Font_info 		*font;
    Font_info      	*font_head;
    struct font_return_attrs    attrs;
    XrmDatabase		fs_db = NULL;
    

    base_list[0] = base_list[1] = (char *)NULL;
    fs_list = (char **)NULL;

    if (!parent_public) {
	parent_public = server = (Xv_opaque) xv_default_server;
	display = xv_get(parent_public, XV_DISPLAY);
    } else {
	Xv_pkg         *pkg = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	display = xv_get(parent_public, XV_DISPLAY);
	if (!display) {
            if ((Xv_pkg *) pkg == (Xv_pkg *) & xv_font_pkg) {
                Xv_Font         public = 0;
                Font_info      *private = 0;

                XV_OBJECT_TO_STANDARD(parent_public, "font_init", public);
                private = FONT_PRIVATE(public);
                parent_public = private->parent;
                display = private->display;
            } else
                display = xv_default_display;
        }
        if ((Xv_pkg *) pkg == (Xv_pkg *) & xv_server_pkg) {
            server = (Xv_opaque) parent_public;
        } else {
            server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
        }
    }

    if (linfo = find_font_locale_info(server, avlist))
	fs_db = linfo->db;

    initialize_font_attrs(linfo, &attrs);

    if (!font_read_attrs(&attrs, TRUE, linfo, avlist))
	(void) font_default_font(server, linfo, &attrs);

    /* 
     * Precedence: FONT_SET_SPECIFIER, FONT_NAMES, FONT_NAME, 
     *		   FONT_RESCALE_OF, FONT_FAMILY.
     */
    if (attrs.specifier) {
	attrs.name = attrs.specifier;
/*	(void)font_decrypt_xlfd_name(font_attrs); */
	if (str = get_font_set_list(fs_db, attrs.specifier)) {
	    fs_list = construct_font_set_list(str);
	    free_fs_list = TRUE;
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try fs name specifier [%s]\n", attrs.specifier);
#endif
	} else {
	    /* specifier is being used as the name */
	    goto attrs_name;
	}
    } else if (attrs.names) {
	fs_list = attrs.names;
    } else if (attrs.name) {
	char *name;

attrs_name:
	name = attrs.name = normalize_font_name(attrs.name);

	if (font_decrypt_sv1_name(&attrs) == 0) {
	    font_name = font_determine_font_name(&attrs, linfo);
	    if ((font_name == NULL) || (strlen(font_name) == 0))
                attrs.name = name; /* reset */
        } else  {
            /* not sv1 name */
            /*
             * If successfully decrypted xv name, try to construct
             * xlfd name.
             */
            if (font_decrypt_xv_name(&attrs, linfo) == 0)  {
                font_name = font_determine_font_name(&attrs, linfo);
                if ((font_name == NULL) || (strlen(font_name) == 0))
                    attrs.name = name; /* reset */
            }
            else  {
                /*
                 * If not sv1 name, not xv name, check if xlfd name
                 * Do not have to call font_determine_font_name here
                 * because the name is already in xlfd format.
                 */
                (void)font_decrypt_xlfd_name(&attrs, linfo);
            }
        }
	base_list[0] = attrs.name ? attrs.name : font_name;
        base_list[1] = NULL;
        fs_list = base_list;
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try font name [%s]\n", base_list[0]);
#endif
    } else if (attrs.resize_from_font) {
        font_name = font_rescale_from_font(
    			(Font_info *) attrs.resize_from_font,
                        (int) attrs.rescale_factor,
                        &attrs, linfo);
        if ((font_name == NULL) || (strlen(font_name) == 0))  {
            char            dummy[128];
 
            sprintf(dummy, "Attempt to rescale from font failed");
            xv_error(NULL,
                     ERROR_STRING, dummy,
                     ERROR_PKG, FONT,
                     0);
            return XV_ERROR;
        }

	if ((str = get_font_set_list(fs_db, font_name)) != NULL) {
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try fs name for resize [%s]\n", font_name);
#endif
                fs_list = construct_font_set_list(str);
                free_fs_list = TRUE;
		attrs.specifier = font_name;
            }

	if (fs_list == NULL) {
	    base_list[0] = attrs.name ? attrs.name : font_name;
            base_list[1] = NULL;
            fs_list = base_list;
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try resize font [%s]\n", base_list[0]);
#endif
	}
    } else {	
       /*
        * Attempt to find a font set definition using a concatenation of the
        * font family, style and pixel size. If a definition is not found,
        * generate a font name assuming only one font is required for that
        * locale.
        */
        fs_list = NULL;
        if (fs_db && (attrs.type == FONT_TYPE_TEXT)) {
            char        key[256];
	    int		size;

	    if (attrs.size != FONT_NO_SIZE)
		size = attrs.size;
	    else if (linfo && (attrs.scale != FONT_NO_SCALE))
		size = find_size_from_scale(linfo, attrs.scale);
	    else 
		size = find_size_from_defaults(linfo);

	    find_family_from_attrs(&attrs, linfo);
	    find_style_from_attrs(&attrs, linfo);

            sprintf(key, "%s-%s-%d", attrs.family, attrs.style, size);

            str = get_font_set_list(fs_db, key);
            if (str != NULL) {
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try fs name [%s]\n", key);
#endif
                fs_list = construct_font_set_list(str);
                free_fs_list = TRUE;
		attrs.specifier = key;
            }
        }
 
        if (fs_list == NULL) {
            font_name = font_determine_font_name(&attrs, linfo);
            if ((font_name == NULL) || (strlen(font_name) == 0)) {
                char            dummy[128];
 
                (void) sprintf(dummy, "Cannot load font '%s'",
                               (attrs.name) ? attrs.name : font_name);
                xv_error(NULL,
                         ERROR_STRING, dummy,
                         ERROR_PKG, FONT,
                         0);
                return XV_ERROR;
            } else
                free_font_name = 1;     /* free name later */
 
#ifdef FONT_DEBUG
		fprintf(stderr, "font: Try font [%s]\n", font_name);
#endif
            base_list[0] = font_name;
	    base_list[1] = NULL;
            fs_list = base_list;
        }
    }

    if (fs_list != NULL) {
	/* Glyph and cursor fonts are always created in "C" locale */
        font_set = (XFontSet) xv_load_font_set(display, 
			(attrs.type == FONT_TYPE_TEXT) ? attrs.locale : "C",
			fs_list);
        if (free_fs_list == TRUE)
            free_font_set_list(fs_list);
    }
    if (font_set == NULL) {
        char            buf[255];

        (void)sprintf(buf, "Cannot load font set '%s'",
                (attrs.name) ? attrs.name : font_name);
        xv_error(NULL,
            ERROR_STRING, buf,
            ERROR_PKG, FONT,
            0);
        if (free_font_name && (font_name != (char *) 0))
            free(font_name);
        return XV_ERROR;
    }
    font = (Font_info *) xv_alloc(Font_info);
    font_public->private_data = (Xv_opaque) font;
    font->public_self = (Xv_opaque) font_public;
    font->parent = parent_public;
 
    font->display = display;
    font->server = server;
    font->pkg = ATTR_PKG_FONT;
    font->pixfont = (char *)NULL;
    font->set_id = font_set;
    font->locale_info = linfo;
    (void) XFontsOfFontSet(font_set, &font->font_structs, &dummy);
    font_set_extents = XExtentsOfFontSet(font_set);
    font->def_char_width = font_set_extents->max_logical_extent.width;
    font->def_char_height = font_set_extents->max_logical_extent.height;
    
    (void) xv_set(font_public, XV_RESET_REF_COUNT, 0);
    font->type = attrs.type;

    font->small_size = (int) attrs.small_size;
    font->medium_size = (int) attrs.medium_size;
    font->large_size = (int) attrs.large_size;
    font->extra_large_size = (int) attrs.extra_large_size;

    if (attrs.size != FONT_NO_SIZE)
	font->size = (int) attrs.size;
    else 
	/* 
	 * Set up the size as the smaller of the ascent+descent 
	 * or bounds->max_bounds.ascent + bounds->max_bounds.descent.
	 * There is no good way to find out the point size.
	 */
	 font->size = font_set_extents->max_ink_extent.height;	
    if (attrs.scale != FONT_NO_SCALE)
	font->scale = (int)attrs.scale;
    else { 
        if (font->size <= font->small_size) 
	    font->scale = WIN_SCALE_SMALL;
    	else if (font->size <= font->medium_size) 
	    font->scale = WIN_SCALE_MEDIUM;
    	else if (font->size <= font->large_size) 
	    font->scale = WIN_SCALE_LARGE;
    	else 
	    font->scale = WIN_SCALE_EXTRALARGE;
    }

    if (attrs.foundry) {
        if (attrs.free_foundry) {
            font->foundry = attrs.foundry;   /* take malloc'd ptr */
        } else
            font->foundry = xv_strsave(attrs.foundry);
    }
    if (attrs.family) {
        if (attrs.free_family) {
            font->family = attrs.family;     /* take malloc'd ptr */
        } else
            font->family = xv_strsave(attrs.family);
    }         
    if (attrs.style) {
        if (attrs.free_style) {
            font->style = attrs.style;       /* take malloc'd ptr */
        } else
            font->style = xv_strsave(attrs.style);
    }         
    if (attrs.weight) {
        if (attrs.free_weight) {
            font->weight = attrs.weight;     /* take malloc'd ptr */
        } else
            font->weight = xv_strsave(attrs.weight);
    }         
    if (attrs.slant) {
        if (attrs.free_slant) {
            font->slant = attrs.slant;       /* take malloc'd ptr */
        } else
            font->slant = xv_strsave(attrs.slant);
    }         
    if (attrs.setwidthname) {
        if (attrs.free_setwidthname) {
            font->setwidthname = attrs.setwidthname; /* take malloc'd ptr */
        } else
            font->setwidthname = xv_strsave(attrs.setwidthname);
    }
    if (attrs.addstylename) {
        if (attrs.free_addstylename) {
            font->addstylename = attrs.addstylename; /* take malloc'd ptr */
        } else
            font->addstylename = xv_strsave(attrs.addstylename);
    }

    if (attrs.specifier)
        font->specifier = strdup(attrs.specifier);

    if (attrs.specifier && (attrs.specifier == attrs.name))
	font->name = font->specifier;
    else
        if (attrs.name) {
            if (attrs.free_name) {
                font->name = attrs.name;
            } else
                font->name = xv_strsave(attrs.name);
        } else if (font_name) {
            font->name = xv_strsave(font_name);
        }

 
    /* Add new font to server's list */
    if (font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD)) {
        font->next = font_head->next;
        font_head->next = font;
    } else {
        font->next = (Font_info *) 0;
        (void) xv_set(server,
                      XV_KEY_DATA, FONT_HEAD, font,
                      XV_KEY_DATA_REMOVE_PROC, FONT_HEAD, font_list_free,
                      0);
    }
 
    /* SunView1.X compatibility: set this font as default if appropriate.*/
    if ((xv_pf_sys == (Pixfont *) 0) &&
        (parent_public == (Xv_opaque) xv_default_server)) {
	if ((font_string_compare(attrs.name,
                                 normalize_font_name((char *)NULL)) == 0)) {
            xv_pf_sys = (Pixfont *)font_public;
            (void) xv_set(font_public, XV_INCREMENT_REF_COUNT, 0);
        }
    }
    return XV_OK;
}

int
font_destroy_struct(font_public, status)
    Xv_font_struct *font_public;
    Destroy_status  status;
{
    register Font_info *font = FONT_PRIVATE(font_public);
    register Font_info *prev;
    register int    i;
    register struct pixchar
                   *pfc;
    Font_info      *font_head;
    Xv_opaque       display, server = font->server;
    Pixfont        *zap_font_public = 
	(Pixfont *) font->pixfont;

    if (status == DESTROY_CLEANUP) {
	/* PERFORMANCE ALERT: revive list package to consolidate code. */
	/* Remove the font from SunView's server list. */
	font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	if (!font_head) {
	    server = (Xv_opaque) xv_default_server;
	    font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	}
	if (((Xv_Font) FONT_PUBLIC(font_head)) == (Xv_Font) font_public) {
	    /* at head of list */
	    (void) xv_set(server, XV_KEY_DATA, FONT_HEAD, font->next, 0);
	} else {
	    for (prev = font_head; prev; prev = prev->next) {
		if (prev->next == font) {
		    prev->next = font->next;
		    break;
		}
	    }
#ifdef _XV_DEBUG
	    if (prev == 0)
		abort();
#endif
	}
	/* Free the storage allocated for glyphs. */
	if (zap_font_public)  {
  	    XFontStruct *x_font_info = (XFontStruct *)font->font_structs[0];
            int max_char = MIN(255, x_font_info->max_char_or_byte2);
            int min_char = MIN(255, x_font_info->min_char_or_byte2);

	    for (i = min_char, pfc = &(zap_font_public->pf_char[i]);
	         i <= max_char; i++, pfc++) {
	        if (pfc->pc_pr) {
		    xv_mem_destroy(pfc->pc_pr);
	        }
	    }
	}
	/* free string storage */
	if (font->name == font->specifier) {
	    /* specifier was used as the font name */
	    free(font->name);
	    font->name = font->specifier = NULL;
	}
	if (font->specifier)
	    free(font->specifier);
	if (font->name)
	    free(font->name);
	if (font->foundry)
	    free(font->foundry);
	if (font->family)
	    free(font->family);
	if (font->style)
	    free(font->style);
	if (font->weight)
	    free(font->weight);
	if (font->slant)
	    free(font->slant);
	if (font->setwidthname)
	    free(font->setwidthname);
	if (font->addstylename)
	    free(font->addstylename);
	/* Remove the font from X server list, and free our private data. */
	display = font->display;
	if (!display)
	    display = (Xv_opaque) xv_get(xv_default_server, XV_DISPLAY);
	XFreeFontSet((Display *) display, font->set_id);
	free(font);
    }
    return XV_OK;
}

Xv_private      Xv_Font
xv_font_with_name(server, name)
    Xv_opaque       server;
    char           *name;
{
    Xv_Font     font_public = NULL;
    char	*str;

    if (name || (name = defaults_get_string("font.name", "Font.Name", NULL)))
	font_public = (Xv_Font)xv_find(server, FONT,
				    FONT_SET_SPECIFIER, name,
				    NULL);

    if (font_public == NULL) 
	if (str = defaults_get_string("window.scale", "Window.Scale", NULL)) 
	    font_public = xv_find(server, FONT,  
				FONT_SCALE, find_scale_from_string(str),
				NULL);
	else
	    font_public = xv_find(server, FONT, NULL);

    if (font_public == NULL) 
	xv_error(NULL,
	    ERROR_SEVERITY, ERROR_RECOVERABLE,
	    ERROR_STRING, 
		XV_I18N_MSG("xv_messages", "Unable to open default font set \n"),
	    NULL);

    return(font_public);
}

static char    *
font_determine_font_name(attrs, locale_info)
    Font_return_attrs 	attrs;
    Font_locale_info	*locale_info;
{
    char    name[512], this_name[512];
    int     size;
    char    *font_default_family = FONT_FIND_DEFAULT_FAMILY(locale_info);

    /*
     * Return null if no family/style specified
     */
    if ( (attrs->family == (char *) NULL)
	&& (attrs->style == (char *) NULL) 
	&& (attrs->weight == (char *) NULL) 
	&& (attrs->slant == (char *) NULL) )
	return (char *) NULL;

    if (font_string_compare(attrs->family, FONT_FAMILY_DEFAULT) == 0) {
	if (attrs->free_family) {
	    free(attrs->family);
	    attrs->free_family = 0;
	}
	if (font_default_family) {
	    attrs->family = xv_strsave(font_default_family);
	    attrs->free_family = 1;
	} else {
	    attrs->family = (char *) 0;
	    attrs->free_family = 0;
	}
    }
    if (font_string_compare(attrs->family, FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0) {
	if (attrs->free_family) {
	    free(attrs->family);
	    attrs->free_family = 0;
	}
	if (font_default_fixedwidth_family) {
	    attrs->family = xv_strsave(font_default_fixedwidth_family);
	    attrs->free_family = 1;
	} else {
	    attrs->family = (char *) 0;
	    attrs->free_family = 0;
	}
    }
    if (font_string_compare(attrs->style, FONT_STYLE_DEFAULT) == 0) {
	if (attrs->free_style) {
	    free(attrs->style);
	    attrs->free_style = 0;
	}
	if (font_default_style) {
	    attrs->style = xv_strsave(font_default_style);
	    attrs->free_style = 1;
	} else {
	    attrs->style = (char *) 0;
	    attrs->free_style = 0;
	}
    }
    /*
     * Convert font style into weight and slant
     */
    if ((attrs->style) && (!(attrs->weight) || !(attrs->slant)))  {
        if (font_convert_style(attrs))  {
	    char	dummy[128];

	    /*
	     * If cannot convert style into any weight/slant combination
	     * give 'normal' style - medium/roman
	     */
	    sprintf(dummy, "Font style %s is not known, using style 'normal' instead",
		attrs->style);
	    xv_error(NULL,
		    ERROR_STRING, dummy,
		    ERROR_PKG, FONT,
		    0);

	    if (attrs->free_style)  {
	        free(attrs->style);
	        attrs->free_style = 0;
	    }
	    if (attrs->free_weight)  {
	        free(attrs->weight);
	        attrs->free_weight = 0;
	    }
	    if (attrs->free_slant)  {
	        free(attrs->slant);
	        attrs->free_slant = 0;
	    }
	    attrs->style = xv_strsave("normal");
	    attrs->free_style = 1;
	    attrs->weight = xv_strsave("medium");
	    attrs->free_weight = 1;
	    attrs->slant = xv_strsave("r");
	    attrs->free_slant = 1;
	}
    }

    if (attrs->size == FONT_SIZE_DEFAULT) {
	attrs->size = font_size_from_scale_and_sizes(
							attrs->scale,
	     attrs->small_size = (attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : attrs->small_size,
	   attrs->medium_size = (attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : attrs->medium_size,
	     attrs->large_size = (attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : attrs->large_size,
							attrs->extra_large_size = (attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : attrs->extra_large_size);
	if (attrs->size == FONT_NO_SIZE)
	    attrs->size = FONT_FIND_DEFAULT_SIZE(locale_info);
    }

    /*
     * If just check scale without checking size, the scale will precede over 
     * whatever size is specified. So, do both.
     */
    if ((attrs->scale == FONT_SCALE_DEFAULT) && (attrs->size < 0)) {
	attrs->scale = DEFAULT_FONT_SCALE;
	attrs->size = font_size_from_scale_and_sizes(
							attrs->scale,
	     attrs->small_size = (attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : attrs->small_size,
	   attrs->medium_size = (attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : attrs->medium_size,
	     attrs->large_size = (attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : attrs->large_size,
							attrs->extra_large_size = (attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : attrs->extra_large_size);
	if (attrs->size == FONT_NO_SIZE)
	    attrs->size = FONT_FIND_DEFAULT_SIZE(locale_info);
    }

    size = ((attrs->size != FONT_NO_SIZE) || (attrs->size >= 0))
	? attrs->size : font_size_from_scale_and_sizes(
							  attrs->scale,
						       attrs->small_size,
						      attrs->medium_size,
						       attrs->large_size,
						attrs->extra_large_size);

    this_name[0] = '\0';
    name[0] = '\0';

    sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(attrs->foundry ? attrs->foundry:"*"), 
			(attrs->family ? attrs->family:"*"), 
			(attrs->weight ? attrs->weight:"*"), 
			(attrs->slant ? attrs->slant:"*"), 
			(attrs->setwidthname ? attrs->setwidthname:"*"),
			(attrs->addstylename ? attrs->addstylename:"*") );

    /*
     * If at this time, we still don't have a usable size, use the default
     */
    if ((size != FONT_NO_SIZE) && (size > 0)) {
	attrs->size = size;
    } else  {
	attrs->size = size = FONT_FIND_DEFAULT_SIZE(locale_info);
    }

    sprintf(this_name, "%s-*-%d-*-*-*-*-*-*", name, (10*size));
    
    /*
     * Isa
     * Check if no important attrs managed to be filled in
     * BUG!!?? What are important attributes. The list below
     * is probably too long a list. Perhaps family, style 
     * alone is sufficient
    if ( !(attrs->family) &&
	 !(attrs->weight) &&
	 !(attrs->slant) )  {

	return (char *) NULL;
    }
     */

    if (this_name[0]) {		/* any name constructed */
	attrs->name = xv_strsave(this_name);
	attrs->free_name = 1;
	if (font_decrypt_sv1_name(attrs) != 0) {
	    /* not sv1 name */
	    if (font_decrypt_xv_name(attrs, locale_info) != 0)
		/* not xv name */
		font_decrypt_xlfd_name(attrs, locale_info);
	};
	return (char *) attrs->name;
    } else
	return (char *) NULL;
}

static int
font_size_from_scale_and_sizes(scale, small, med, large, xlarge)
    int             scale, small, med, large, xlarge;
{
    switch (scale) {
      case WIN_SCALE_SMALL:
	if (small == FONT_NO_SIZE)
	    return DEFAULT_SMALL_FONT_SIZE;
	return small;
      case WIN_SCALE_MEDIUM:
	if (med == FONT_NO_SIZE)
	    return DEFAULT_MEDIUM_FONT_SIZE;
	return med;
      case WIN_SCALE_LARGE:
	if (large == FONT_NO_SIZE)
	    return DEFAULT_LARGE_FONT_SIZE;
	return large;
      case WIN_SCALE_EXTRALARGE:
	if (xlarge == FONT_NO_SIZE)
	    return DEFAULT_XLARGE_FONT_SIZE;
	return xlarge;
      default:
	return FONT_NO_SIZE;
    }
}

static char    *
font_rescale_from_font(font, scale, attrs, locale_info)
    Font_info      	*font;
    int             	scale;
    struct font_return_attrs *attrs;
    Font_locale_info	*locale_info;
{
    char           *font_name = NULL;
    char            new_name[256], name[512];
    int             desired_scale;
    int             fs = 0;

    if (!font)			/* if possibly not set? */
	return (char *) font_name;
    name[0] = '\0';
    if ((scale < (int) WIN_SCALE_SMALL) ||
	(scale > (int) WIN_SCALE_EXTRALARGE) ||
	(scale == FONT_NO_SCALE))  {
	char	dummy[128];

	sprintf(dummy, "Bad scale value:%d", (int)scale);
	xv_error(NULL,
	        ERROR_STRING, dummy,
	        ERROR_PKG, FONT,
	        0);
	return (char *) font_name;	/* no scaling */
    }

    if ((font->family == 0) && (font->style == 0) && 
		(font->weight == 0) && (font->slant == 0)) {
	if (font->name == 0)
	    return (char *) font_name;	/* you're really out of luck! */
	/*
	 * Try to decrypt the attributes from the name
	 */
	if (font_decrypt_xlfd_name(attrs, locale_info) == -1)  {
	    return (char *) font_name;	/* you're REALLY out of luck! */
	}
    }
    
	/* munch everything together */
	if (locale_info != NULL && strcmp(locale_info->locale, "C") != 0
		&& font->type == FONT_TYPE_TEXT)
	{
            fs = 1;
	    sprintf(name, "%s-%s",
			(font->family ? font->family:"*"),
		        (font->style ? font->style:"*"));
	    if (font->family) {
		attrs->family = xv_strsave(font->family);
		attrs->free_family = 1;
	    }
	    if (font->style) {
		attrs->style = xv_strsave(font->style);
		attrs->free_style = 1;
	    }
	} else {
            sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(font->foundry ? font->foundry:"*"), 
			(font->family ? font->family:"*"), 
			(font->weight ? font->weight:"*"), 
			(font->slant ? font->slant:"*"), 
			(font->setwidthname ? font->setwidthname:"*"),
			(font->addstylename ? font->addstylename:"*") );


	    if (font->foundry) {
		attrs->foundry = xv_strsave(font->foundry);
		attrs->free_foundry = 1;
	    }
	    if (font->family) {
		attrs->family = xv_strsave(font->family);
		attrs->free_family = 1;
	    }
	    if (font->style) {
		attrs->style = xv_strsave(font->style);
		attrs->free_style = 1;
	    }
	    if (font->weight) {
		attrs->weight = xv_strsave(font->weight);
		attrs->free_weight = 1;
	    }
	    if (font->slant) {
		attrs->slant = xv_strsave(font->slant);
		attrs->free_slant = 1;
	    }
	    if (font->setwidthname) {
		attrs->setwidthname = xv_strsave(font->setwidthname);
		attrs->free_setwidthname = 1;
	    }
	    if (font->addstylename) {
		attrs->addstylename = xv_strsave(font->addstylename);
		attrs->free_addstylename = 1;
	    }
	}

    switch (scale) {
      case WIN_SCALE_SMALL:
	desired_scale = font->small_size;
	break;
      case WIN_SCALE_MEDIUM:
	desired_scale = font->medium_size;
	break;
      case WIN_SCALE_LARGE:
	desired_scale = font->large_size;
	break;
      case WIN_SCALE_EXTRALARGE:
	desired_scale = font->extra_large_size;
	break;
      default:
	desired_scale = -1;
    }
    if (desired_scale == -1)
	return (char *) font_name;	/* no font that scale */
    new_name[0] = '\0';

    /*
     * If cannot get a size, give the default
     */
    if ((desired_scale == FONT_NO_SIZE) || (desired_scale <= 0)) {
	desired_scale = FONT_FIND_DEFAULT_SIZE(locale_info);
    }
    
    if (fs)
	sprintf(new_name, "%s-%d", name, desired_scale);
    else
	sprintf(new_name, "%s-*-%d-*-*-*-*-*-*", name, (10*desired_scale));

    attrs->name = xv_strsave(new_name);
    attrs->free_name = 1;
    attrs->size = desired_scale;
    attrs->scale = scale;
    attrs->small_size = font->small_size;
    attrs->medium_size = font->medium_size;
    attrs->large_size = font->large_size;
    attrs->extra_large_size = font->extra_large_size;
    attrs->type = font->type;

    return (attrs->name);
}


static int
xv_strcmp(str1, str2)
    char           *str1, *str2;
{
    if ((str1 == NULL) || (str2 == NULL)) {
        return (int) -1;
    } else
        return (int) strcmp(str1, str2);
}


struct font_info *
font_search_list(font_list, linfo, font_name, attrs)
    struct font_info		*font_list;
    Font_locale_info    	*linfo;
    char			*font_name;
    struct font_return_attrs    attrs;
{
    while (font_list) {
	if ((linfo == font_list->locale_info) && 
		(!xv_strcmp(font_name, font_list->name) ||
		    (!xv_strcmp(attrs.family, font_list->family) &&
		    !font_string_compare(attrs.style, font_list->style) &&
		    ((font_list->size == attrs.size) && 
		     (font_list->scale == attrs.scale) &&
		     (font_list->type == attrs.type)))))
   		return (font_list);
        font_list = font_list->next;
   }

   return(NULL);
}

/*ARGSUSED*/
Xv_object
font_find_font(parent_public, pkg, avlist)
    Xv_opaque       parent_public;
    Xv_pkg         *pkg;
    Attr_avlist     avlist;
{
    Xv_opaque			server;
    Font_locale_info    	*linfo;
    struct font_return_attrs    attrs;
    struct font_info 		*font_list, *finfo;
    char			*font_name;

    if (!parent_public) 
	server = (Xv_opaque) xv_default_server;
    else {
	Xv_pkg         *pkgtype = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_server_pkg) 
	    server = parent_public;
	else 
	    server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
    }    

    linfo = find_font_locale_info(server, avlist);

    initialize_font_attrs(linfo, &attrs);

    if (!font_read_attrs(&attrs, FALSE, linfo, avlist))
	(void) font_default_font(server, linfo, &attrs);

    font_list = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
    finfo = NULL;

    /* 
     * Precedence: FONT_SET_SPECIFIER, FONT_NAMES, FONT_NAME,
     *		   FONT_RESCALE_OF, FONT_FAMILY.
     */
    if (attrs.specifier) {
	for (finfo = font_list; finfo != NULL; finfo = finfo->next)
	    if ((linfo == finfo->locale_info) && 
			!xv_strcmp(attrs.specifier, finfo->specifier))
		break;

	/* 
	 * If a font set with this font set specifier was not found,
	 * check if the specifier is being used as the font name for 
	 * codeset 0 in any font set in this locale.
	 */
	if (finfo == NULL)
	    for (finfo = font_list; finfo != NULL; finfo = finfo->next)
	        if ((linfo == finfo->locale_info) &&
			!xv_strcmp(attrs.specifier, finfo->name))
		    break;
    } else if (attrs.name) {
	attrs.name = normalize_font_name(attrs.name);
        if (font_decrypt_sv1_name(&attrs) != 0) {
            /* not sv1 name */
            if (font_decrypt_xv_name(&attrs, linfo) != 0) {
                /* not xv name */
                font_decrypt_xlfd_name(&attrs, linfo);
            }
        }    
	finfo = font_search_list(font_list, linfo, attrs.name, attrs);
    } else if (attrs.resize_from_font) {
        font_name = font_rescale_from_font(
                                        (Font_info *) attrs.resize_from_font,
                                           (int) attrs.rescale_factor,
                                           &attrs, linfo);
	if (font_name && strlen(font_name)) 
	    finfo = font_search_list(font_list, linfo, font_name, attrs);
    } else {
	char        key[256];
	int         size;

	if (linfo && (attrs.type == FONT_TYPE_TEXT)) {
	    if (attrs.size != FONT_NO_SIZE)
	        size = attrs.size;
	    else if (linfo && (attrs.scale != FONT_NO_SCALE))
       	        size = find_size_from_scale(linfo, attrs.scale);
	    else
	        size = find_size_from_defaults(linfo);

	    find_family_from_attrs(&attrs, linfo);
	    find_style_from_attrs(&attrs, linfo); 
    
	    sprintf(key, "%s-%s-%d", attrs.family, attrs.style, size); 
    
	    for (finfo = font_list; finfo != NULL; finfo = finfo->next)
	        if ((linfo == finfo->locale_info) && 
		        !xv_strcmp(key, finfo->specifier))
		    break;
	}

	if (finfo == NULL) {
	    font_name = font_determine_font_name(&attrs, linfo);
	    if (font_name && strlen(font_name))
		finfo = font_search_list(font_list, linfo, font_name, attrs);
	}
    }

    font_free_font_return_attr_strings(&attrs);

    if (finfo != NULL) {
	(void) xv_set(FONT_PUBLIC(finfo), XV_INCREMENT_REF_COUNT, 0);
	return(FONT_PUBLIC(finfo));
    }

    return((Xv_object)NULL);
}

font_free_font_return_attr_strings(attrs)
    struct font_return_attrs *attrs;
{
    if (attrs->free_name) {
	free(attrs->name);
	attrs->free_name = 0;
    }
    if (attrs->free_foundry) {
	free(attrs->foundry);
	attrs->free_foundry = 0;
    }
    if (attrs->free_family) {
	free(attrs->family);
	attrs->free_family = 0;
    }
    if (attrs->free_style) {
	free(attrs->style);
	attrs->free_style = 0;
    }
    if (attrs->free_weight) {
	free(attrs->weight);
	attrs->free_weight = 0;
    }
    if (attrs->free_slant) {
	free(attrs->slant);
	attrs->free_slant = 0;
    }
    if (attrs->free_setwidthname) {
	free(attrs->setwidthname);
	attrs->free_setwidthname = 0;
    }
    if (attrs->free_addstylename) {
	free(attrs->addstylename);
	attrs->free_addstylename = 0;
    }
}

/*
 * the following proc is a wrapper for strcmp() strncmp() such that it will
 * return =0 if both strings are NULL and !=0 if one or other is NULL, and
 * standard strcmp otherwise. BUG: strcmp will seg fault if either str is
 * NULL.
 */
static int
font_string_compare(str1, str2)
    char           *str1, *str2;
{
    if ((str1 == NULL) && (str2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((str1 == NULL) || (str2 == NULL)) {
	return (int) -1;	/* avoid seg fault */
    } else
	return (int) strcmp(str1, str2);
}

static int
font_string_compare_nchars(str1, str2, n_chars)
    char           *str1, *str2;
    int             n_chars;
{
    int             result;
    int             len1 = (str1) ? strlen(str1) : 0;
    int             len2 = (str2) ? strlen(str2) : 0;
    if ((len1 == 0) && (len2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((len1 && !len2) || (!len1 && len2)) {
	return (int) -1;	/* They're different strings */
    } else if ((!len1) || (!len2)) {
	return (int) -1;	/* avoid seg fault */
    } else
	result = strncmp(str1, str2, n_chars);
    return (int) result;
}

static int
font_decrypt_sv1_name(attrs)
    Font_return_attrs attrs;
{
    struct font_translation current_entry;
    int             i;

    if (attrs->name == NULL)
	return (0);
    for (i = 0, current_entry = known_sv1fonts[i];
	 current_entry.sv1_name;
	 current_entry = known_sv1fonts[i]) {
	if (font_string_compare_nchars(current_entry.sv1_name, attrs->name,
				     strlen(current_entry.sv1_name)) == 0) {
	    attrs->foundry = current_entry.sv1_foundry;

	    attrs->family = current_entry.sv1_family;
	    attrs->style = current_entry.sv1_style;
	    attrs->weight = current_entry.sv1_weight;
	    attrs->slant = current_entry.sv1_slant;
	    attrs->setwidthname = current_entry.sv1_setwidthname;
	    attrs->addstylename = current_entry.sv1_addstylename;

	    attrs->size = current_entry.size;
	    attrs->scale = current_entry.scale;
	    return (0);
	}
	i++;
    }
    return (-1);
}

static int
font_decrypt_xv_name(attrs, locale_info)
    Font_return_attrs	attrs;
    Font_locale_info	*locale_info;
{
    struct font_definitions current_entry;
    int             i;

    char            family_style[128], nosize_entry[128], small_entry[128], 
		    med_entry[128], large_entry[128], xlarge_entry[128];
    int             len_of_name, match = 0;

    if (attrs->name == NULL)
	return (0);
    if (locale_info != NULL && strcmp(locale_info->locale, "C") != 0)
    if (locale_info != NULL
     && strcmp(locale_info->locale, "C") != 0
     && attrs->type == FONT_TYPE_TEXT)
	return (0);
    len_of_name = strlen(attrs->name);
    for (i = 0, current_entry = known_xvfonts[i];
	 current_entry.xv_family;
	 current_entry = known_xvfonts[i]) {
	family_style[0] = '\0';
	nosize_entry[0] = '\0';
	small_entry[0] = '\0';
	med_entry[0] = '\0';
	large_entry[0] = '\0';
	xlarge_entry[0] = '\0';

	if (current_entry.xv_style != (char *) 0) {
	    (void) sprintf(family_style, "%s-%s",
			current_entry.xv_family,
			current_entry.xv_style);
	}
	else  {
	    (void) sprintf(family_style, "%s",
			current_entry.xv_family);
			
	}

	sprintf(nosize_entry, "%s", family_style);
	sprintf(small_entry, "%s-%d", family_style, current_entry.small_size);
	sprintf(med_entry, "%s-%d", family_style, current_entry.medium_size);
	sprintf(large_entry, "%s-%d", family_style, current_entry.large_size);
	sprintf(xlarge_entry, "%s-%d", family_style, current_entry.extra_large_size);
	if (font_string_compare_nchars(small_entry, attrs->name,
			      MAX(strlen(small_entry), len_of_name)) == 0) {
	    attrs->foundry = current_entry.foundry;
	    attrs->family = current_entry.family;
	    attrs->weight = current_entry.weight;
	    attrs->slant = current_entry.slant;
	    attrs->setwidthname = current_entry.setwidthname;
	    attrs->addstylename = current_entry.addstylename;

	    attrs->style = current_entry.xv_style;
	    attrs->size = current_entry.small_size;
	    attrs->scale = (int) WIN_SCALE_SMALL;
	    attrs->small_size = current_entry.small_size;
	    attrs->medium_size = current_entry.medium_size;
	    attrs->large_size = current_entry.large_size;
	    attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(med_entry, attrs->name,
				MAX(strlen(med_entry), len_of_name)) == 0) {
	    attrs->foundry = current_entry.foundry;
	    attrs->family = current_entry.family;
	    attrs->weight = current_entry.weight;
	    attrs->slant = current_entry.slant;
	    attrs->setwidthname = current_entry.setwidthname;
	    attrs->addstylename = current_entry.addstylename;

	    attrs->style = current_entry.xv_style;
	    attrs->size = current_entry.medium_size;
	    attrs->scale = (int) WIN_SCALE_MEDIUM;
	    attrs->small_size = current_entry.small_size;
	    attrs->medium_size = current_entry.medium_size;
	    attrs->large_size = current_entry.large_size;
	    attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(large_entry, attrs->name,
			      MAX(strlen(large_entry), len_of_name)) == 0) {
	    attrs->foundry = current_entry.foundry;
	    attrs->family = current_entry.family;
	    attrs->weight = current_entry.weight;
	    attrs->slant = current_entry.slant;
	    attrs->setwidthname = current_entry.setwidthname;
	    attrs->addstylename = current_entry.addstylename;

	    attrs->style = current_entry.xv_style;
	    attrs->size = current_entry.large_size;
	    attrs->scale = (int) WIN_SCALE_LARGE;
	    attrs->small_size = current_entry.small_size;
	    attrs->medium_size = current_entry.medium_size;
	    attrs->large_size = current_entry.large_size;
	    attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(xlarge_entry, attrs->name,
			     MAX(strlen(xlarge_entry), len_of_name)) == 0) {
	    attrs->foundry = current_entry.foundry;
	    attrs->family = current_entry.family;
	    attrs->weight = current_entry.weight;
	    attrs->slant = current_entry.slant;
	    attrs->setwidthname = current_entry.setwidthname;
	    attrs->addstylename = current_entry.addstylename;

	    attrs->style = current_entry.xv_style;
	    attrs->size = current_entry.extra_large_size;
	    attrs->scale = (int) WIN_SCALE_EXTRALARGE;
	    attrs->small_size = current_entry.small_size;
	    attrs->medium_size = current_entry.medium_size;
	    attrs->large_size = current_entry.large_size;
	    attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(nosize_entry, attrs->name,
			     MAX(strlen(nosize_entry), len_of_name)) == 0) {
	    attrs->foundry = current_entry.foundry;
	    attrs->family = current_entry.family;
	    attrs->weight = current_entry.weight;
	    attrs->slant = current_entry.slant;
	    attrs->setwidthname = current_entry.setwidthname;
	    attrs->addstylename = current_entry.addstylename;

	    attrs->style = current_entry.xv_style;
	    attrs->size = FONT_FIND_DEFAULT_SIZE(locale_info);
	    attrs->scale = font_default_scale;
	    attrs->small_size = current_entry.small_size;
	    attrs->medium_size = current_entry.medium_size;
	    attrs->large_size = current_entry.large_size;
	    attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	}

	if (match)  {
	    return(0);
	}

	i++;
    }
    return (-1);
}

static void
font_decrypt_family_style(return_attrs, locale_info)
    Font_return_attrs 	return_attrs;
    Font_locale_info	*locale_info;
{
    struct xv_to_x_font_translation current_entry;
    register int    i;
    char           *requested_family = return_attrs->family;
    char           *requested_style = return_attrs->style;
    char    *font_default_family = FONT_FIND_DEFAULT_FAMILY(locale_info);

    /*
     * If family/style is not specified, give the default
     */
    if (!(requested_family) && !(return_attrs->name))  {
	requested_family = return_attrs->family = font_default_family;
    }

    if (!(requested_style) && !(return_attrs->name))  {
	requested_style = return_attrs->style = FONT_STYLE_DEFAULT;
    }

    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT) == 0)
	requested_family = return_attrs->family
	    = font_default_family;
    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0)
	requested_family = return_attrs->family
	    = font_default_fixedwidth_family;
    if (!requested_family)
	requested_family = "";	/* avoid segv in strlen */
    if (font_string_compare(requested_style,
			    FONT_STYLE_DEFAULT) == 0)
	requested_style = return_attrs->style
	    = font_default_style;
    if (!requested_style)
	requested_style = "";	/* avoid segv in strlen */
    /* otherwise, leave family/style alone */

    if (requested_family && strlen(requested_family)) {
	for (i = 0, current_entry = known_openwin_fonts[i];
	     current_entry.xv_family;
	     current_entry = known_openwin_fonts[i]) {

	    if (((font_string_compare_nchars(current_entry.xv_family,
			   requested_family, strlen(requested_family)) == 0)
		 || (font_string_compare_nchars(current_entry.x_font_family,
			  requested_family, strlen(requested_family)) == 0))
		&& ((font_string_compare_nchars(current_entry.xv_style,
			     requested_style, strlen(requested_style)) == 0)
		  || (font_string_compare_nchars(current_entry.x_font_style,
			 requested_style, strlen(requested_style)) == 0))
		&& (locale_info == NULL
		  || strcmp(locale_info->locale, "C") == 0
		  || return_attrs->type != FONT_TYPE_TEXT)
									) {
		/* assign x font names */

		return_attrs->foundry = current_entry.x_font_foundry;
		return_attrs->family = current_entry.x_font_family;
		return_attrs->style = current_entry.x_font_style;
		return_attrs->weight = current_entry.x_font_weight;
		return_attrs->slant = current_entry.x_font_slant;
		return_attrs->setwidthname = current_entry.x_font_setwidthname;
		return_attrs->addstylename = current_entry.x_font_addstylename;

		return;
	    } else
		i++;
	}
    }
}

static void
font_decrypt_rescale_info(return_attrs, locale_info)
    Font_return_attrs return_attrs;
    Font_locale_info	*locale_info;
{
    /* determine family-style of font wished to rescaled against */
    if (return_attrs->resize_from_font) {
	if (!return_attrs->family && return_attrs->resize_from_font->family)
	    return_attrs->family = return_attrs->resize_from_font->family;
	if (!return_attrs->style && return_attrs->resize_from_font->style)
	    return_attrs->style = return_attrs->resize_from_font->style;
	return_attrs->scale = return_attrs->rescale_factor;
	(void) font_decrypt_family_style(return_attrs, locale_info);
    }
}

static void
font_decrypt_size(return_attrs, locale_info)
    Font_return_attrs 	return_attrs;
    Font_locale_info	*locale_info;
{
    /* determine size */
    if (return_attrs->size == FONT_SIZE_DEFAULT) {
	return_attrs->size = (int) FONT_FIND_DEFAULT_SIZE(locale_info);
    }
}

static void
font_decrypt_scale(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine scale */
    if (return_attrs->scale == FONT_SCALE_DEFAULT) {
	return_attrs->scale = (int) font_default_scale;
    }
}

static void
font_check_size_style_less(return_attrs)
    Font_return_attrs return_attrs;
{
    struct xv_to_x_font_translation current_entry;
    register int    i;
    char           *requested_family = return_attrs->family;
    char	name[256];

    if (!(return_attrs->name) && requested_family && strlen(requested_family)) {
	for (i = 0, current_entry = size_style_less[i];
	     current_entry.xv_family;
	     current_entry = size_style_less[i]) {

	    if (((font_string_compare_nchars(current_entry.xv_family,
			   requested_family, strlen(requested_family)) == 0)
		 || (font_string_compare_nchars(current_entry.x_font_family,
			  requested_family, strlen(requested_family)) == 0))) {
		/* assign x font names */

		return_attrs->foundry = current_entry.x_font_foundry;
		return_attrs->family = current_entry.x_font_family;

                sprintf(name, "-%s-%s-*-*-*-*-*-*-*-*-*-*-*-*",
		    (return_attrs->foundry ? return_attrs->foundry:"*"), 
		    (return_attrs->family ? return_attrs->family:"*") );
                return_attrs->name = xv_strsave(name);
                return_attrs->free_name = 1;

		return;
	    } else
		i++;
	}
    }
}

static int
font_read_attrs(return_attrs, consume_attrs, locale_info, avlist)
    Font_return_attrs 	return_attrs;
    int             	consume_attrs;
    Font_locale_info	*locale_info;
    Attr_avlist     	avlist;
{
    register Attr_avlist attrs;
    int             font_attrs_exist = 0;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case FONT_LOCALE:
	    return_attrs->locale = (char *) attrs[1];
	    font_attrs_exist = 1;
	    break;
	  case FONT_NAMES:
	    return_attrs->names = (char **) attrs[1];
	    font_attrs_exist = 1;
	    break;
	  case FONT_SET_SPECIFIER:
	    return_attrs->specifier = (char *) attrs[1];
	    font_attrs_exist = 1;
	    break;
	  case FONT_NAME:
	    return_attrs->name = (char *) attrs[1];
	    font_attrs_exist = 1;
	    break;
	  case FONT_FAMILY:
	    font_attrs_exist = 1;
	    return_attrs->xv_family = 
	    return_attrs->family    = (char *) attrs[1];
	    break;
	  case FONT_STYLE:
	    font_attrs_exist = 1;
	    return_attrs->xv_style = 
	    return_attrs->style    = (char *) attrs[1];
	    break;
	  case FONT_SIZE:
	    font_attrs_exist = 1;
	    return_attrs->size = (int) attrs[1];
	    break;
	  case FONT_SCALE:
	    font_attrs_exist = 1;
	    return_attrs->scale = (int) attrs[1];
	    break;
	  case FONT_TYPE:
	    font_attrs_exist = 1;
	    return_attrs->type = (int) attrs[1];
	    break;
	  case FONT_SIZES_FOR_SCALE:{
		font_attrs_exist = 1;
		return_attrs->small_size = (int) attrs[1];
		return_attrs->medium_size = (int) attrs[2];
		return_attrs->large_size = (int) attrs[3];
		return_attrs->extra_large_size = (int) attrs[4];
		break;
	    }
	  case FONT_RESCALE_OF:{
		Xv_opaque       pf = (Xv_opaque) attrs[1];
		Xv_Font         font = 0;

		XV_OBJECT_TO_STANDARD(pf, "font_read_attrs", font);
		font_attrs_exist = 1;
		return_attrs->resize_from_font = (Font_info *) FONT_PRIVATE(
								      font);
		return_attrs->rescale_factor = (int) attrs[2];
		break;
	    }
	  default:
	    break;
	}

	if (consume_attrs)
	    ATTR_CONSUME(attrs[0]);
    }

    if (!font_attrs_exist || return_attrs->specifier || return_attrs->names) {
	return (font_attrs_exist);
    } else {
	font_decrypt_family_style(return_attrs, locale_info);
	font_decrypt_size(return_attrs, locale_info);
	font_decrypt_scale(return_attrs);
	font_decrypt_rescale_info(return_attrs, locale_info);
        font_check_size_style_less(return_attrs);
	return (font_attrs_exist);
    }
}

static void
font_default_font(server, locale_info, return_attrs)
    Xv_opaque	      	server;
    Font_locale_info	*locale_info;
    Font_return_attrs 	return_attrs;
{
    return_attrs->locale = (char *)xv_get(server, XV_LC_BASIC_LOCALE);
    return_attrs->family = FONT_FIND_DEFAULT_FAMILY(locale_info);
    return_attrs->style = font_default_style;
    return_attrs->weight = font_default_weight;
    return_attrs->slant = font_default_slant;
    return_attrs->setwidthname = font_default_setwidthname;
    return_attrs->addstylename = font_default_addstylename;
    return_attrs->size = find_size_from_defaults(locale_info);
    return_attrs->scale = (int) font_default_scale;
}

font_convert_style(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_style;

    len_of_style = (return_attrs->style) ? strlen(return_attrs->style):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if (font_string_compare_nchars(current_entry.style, return_attrs->style,
			      MAX(strlen(current_entry.style), len_of_style)) == 0) {

	    return_attrs->weight = current_entry.weight;
	    return_attrs->slant = current_entry.slant;

	    return(0);
	}
    }

    return(-1);
}

font_convert_weightslant(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_weight, len_of_slant;

    len_of_weight = (return_attrs->weight) ? strlen(return_attrs->weight):0;
    len_of_slant = (return_attrs->slant) ? strlen(return_attrs->slant):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if ( (font_string_compare_nchars(current_entry.weight, return_attrs->weight,
			      MAX(strlen(current_entry.weight), len_of_weight)) == 0) &&
	     (font_string_compare_nchars(current_entry.slant, return_attrs->slant,
			      MAX(strlen(current_entry.slant), len_of_slant)) == 0) ) {

	    return_attrs->style = current_entry.style;

	    return(0);
	}
    }

    return(-1);
}


int
font_dashcount(str)
char	*str;
{
    register char	*p1, *p2 = str;
    int		count = 0;

    if (str == NULL)  {
	return(0);
    }

    p1 = index(str, DASH);

    if (p1 == NULL)  {
	return(0);
    }

    ++count;

    while(p2 != NULL)  {
	p2 = index(p1+1, DASH);

	if (p2 != NULL)  {
	    ++count;
	    p1 = p2;
	}
    }

    return(count);
}

char *
strip_xlfd(str, pos)
char	*str;
int	pos;
{
    char	*p1 = str, *p2;

    if ((str == NULL) || (pos < 0))  {
	return((char *)NULL);
    }
    
    while(pos)  {
	p2 = index(p1, DASH);
	if (p2 == NULL)  {
	    return((char *)NULL);
	}
	--pos;
	p1 = p2 + 1;
    }

    /*
     * the string at position `pos` spans p1 to 
     * strip_xlfd(str, pos+1)-1
     */
    return(p1);
}

static int
font_decrypt_xlfd_name(attrs, linfo)
Font_return_attrs attrs;
Font_locale_info  *linfo;
{
    int			tempSize;
    int			medsize, largesize, xlargesize;
    char		tempName[255];
    char		*foundry = NULL;
    char		*family = NULL;
    char		*style = NULL;
    char		*weight = NULL;
    char		*slant = NULL;
    char		*setwidthname = NULL;
    char		*addstylename = NULL;
    char		*pixsize = NULL;
    char		*ptsize = NULL;
    char		*xres = NULL;

    if (attrs->name == NULL)  {
	return(-1);
    }

    if (font_dashcount(attrs->name) != NUMXLFDFIELDS)  {
	if (font_dashcount(attrs->name) != NUMFSFIELDS) {
	    return(-1);
	} else {
	   /* Ok, this name looks like XView style font set name to me */
	   sprintf(tempName, "%s", attrs->name);
	   family = strip_xlfd(tempName, FSFAMILYPOS);
	   style = strip_xlfd(tempName, FSSTYLEPOS);
	   ptsize = strip_xlfd(tempName, FSSIZEPOS);
	   *(style-1) = '\0'; 
	   *(ptsize-1) = '\0';

	   if ((*family) && (*family != DASH))  {
		attrs->family = xv_strsave(family);
	        attrs->free_family = 1;
	   }
	   if ((*style) && (*style != DASH))  {
		attrs->style = xv_strsave(style);
	        attrs->free_style = 1;
	   }
	   if ((*ptsize) && (*ptsize != DASH))  {
		attrs->size = atoi(ptsize);
	   }

	   return 0;
	}
    }

    sprintf(tempName, "%s", attrs->name);

    foundry = strip_xlfd(tempName, FOUNDRYPOS);
    family = strip_xlfd(tempName, FAMILYPOS);
    weight = strip_xlfd(tempName, WEIGHTPOS);
    slant = strip_xlfd(tempName, SLANTPOS);
    setwidthname = strip_xlfd(tempName, SETWIDTHNAMEPOS);
    addstylename = strip_xlfd(tempName, ADDSTYLENAMEPOS);
    pixsize = strip_xlfd(tempName, PIXSIZEPOS);
    ptsize = strip_xlfd(tempName, PTSIZEPOS);
    xres = strip_xlfd(tempName, XRESOLUTIONPOS);

    *(family-1) = '\0';
    *(weight-1) = '\0';
    *(slant-1) = '\0';
    *(setwidthname-1) = '\0';
    *(addstylename-1) = '\0';
    *(pixsize-1) = '\0';
    *(ptsize-1) = '\0';
    *(xres-1) = '\0';

    if ((*foundry) && (*foundry != DASH))  {
	attrs->foundry = xv_strsave(foundry);
	attrs->free_foundry = 1;
    }

    if ((*family) && (*family != DASH))  {
	attrs->family = xv_strsave(family);
	attrs->free_family = 1;
    }

    if ((*weight) && (*weight != DASH))  {
	attrs->weight = xv_strsave(weight);
	attrs->free_weight = 1;
    }

    if ((*slant) && (*slant != DASH))  {
	attrs->slant = xv_strsave(slant);
	attrs->free_slant = 1;
    }

    if ( (*setwidthname) && (*setwidthname != DASH))  {
	attrs->setwidthname = xv_strsave(setwidthname);
	attrs->free_setwidthname = 1;
    }

    if ( (*addstylename) && (*addstylename != DASH))  {
	attrs->addstylename = xv_strsave(addstylename);
	attrs->free_addstylename = 1;
    }

    if ((*ptsize) && (*ptsize != DASH) && (*ptsize != '*'))  {
	tempSize = atoi(ptsize);
	attrs->size = tempSize / 10;

	medsize = (attrs->medium_size < 0) 
		? DEFAULT_MEDIUM_FONT_SIZE : attrs->medium_size;
	if (attrs->size < medsize)  {
	    attrs->scale = (int)WIN_SCALE_SMALL;
	}
	else  {
	    largesize = (attrs->large_size < 0) 
		? DEFAULT_LARGE_FONT_SIZE : attrs->large_size;
	    if (attrs->size < largesize)  {
	        attrs->scale = (int)WIN_SCALE_MEDIUM;
	    }
	    else  {
	        xlargesize = (attrs->extra_large_size < 0) 
		    ? DEFAULT_XLARGE_FONT_SIZE : attrs->extra_large_size;
		if (attrs->size < xlargesize)  {
	            attrs->scale = (int)WIN_SCALE_LARGE;
		}
		else  {
	            attrs->scale = (int)WIN_SCALE_EXTRALARGE;
		}
	    }
	}
    }

    if ((attrs->weight) && (attrs->slant))  {
	font_convert_weightslant(attrs);
    }

    return (0);
}

font_setup_pixfont(font_public)
Xv_font_struct	*font_public;
{
    register struct pixchar *pfc;
    Font_info		*xv_font_info = FONT_PRIVATE(font_public);
    XFontStruct         *x_font_info = xv_font_info->font_structs[0];
    Pixfont		*pixfont = (Pixfont *)xv_get(font_public, FONT_PIXFONT);
    int		i, default_x, default_y, 
		max_char, min_char;

    default_x = xv_font_info->def_char_width;
    default_y = xv_font_info->def_char_height;

    /*
     * Pixfont compat
     */
    max_char = MIN(255, x_font_info->max_char_or_byte2);
    min_char = MIN(255, x_font_info->min_char_or_byte2);

    pixfont->pf_defaultsize.x = default_x;
    pixfont->pf_defaultsize.y = default_y;
    for (i = min_char, pfc = &(pixfont->pf_char[i]);
	 i <= MIN(255, max_char);	/* "i" cannot ever be >255 - pixfont
					 * compat */
	 i++, pfc++) {
	    xv_x_char_info((XFontStruct *) x_font_info, i - min_char,
		       &pfc->pc_home.x, &pfc->pc_home.y,
		       &pfc->pc_adv.x, &pfc->pc_adv.y,
		       &pfc->pc_pr);
    }
}

font_check_var_height(variable_height_font, x_font_info)
int	*variable_height_font;
XFontStruct	*x_font_info;
{
    *variable_height_font = FALSE;
}

font_check_overlapping(neg_left_bearing, x_font_info)
int	*neg_left_bearing;
XFontStruct	*x_font_info;
{
    *neg_left_bearing = (x_font_info->min_bounds.lbearing < 0);
}

font_init_pixfont(font_public)
Xv_font_struct	*font_public;
{
    Font_info		*xv_font_info = FONT_PRIVATE(font_public);
    Pixfont_struct	*pf_rec;
    
    pf_rec = (Pixfont_struct *)malloc(sizeof(Pixfont_struct));

    xv_font_info->pixfont = (char *)(pf_rec);

    font_setup_pixfont(font_public);
    pf_rec->public_self = (Xv_font)font_public;

    return(XV_OK);

}
#else

#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)font.c 70.2 91/07/08";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/xv_debug.h>
#include <sys/types.h>
#include <X11/Xlib.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/generic.h>
#include <xview/notify.h>
#include <xview/server.h>
#include <xview/window.h>
#include <xview_private/font_impl.h>

/*
 * Public
 */

extern Pixfont *xv_pf_sys;
extern char    *defaults_get_string();
extern char    *strcpy();
extern Xv_opaque xv_default_server;
extern Xv_opaque xv_default_display;

extern char *index();

/*
 * Private
 */
Xv_private void xv_x_char_info();
Pkg_private XID xv_load_x_font();
Pkg_private void xv_unload_x_font();

/*
 * Definitions for decrypting xlfd names
 */
#define		DASH		'-'
#define		NUMXLFDFIELDS	14
#define		FOUNDRYPOS	1
#define		FAMILYPOS	2
#define		WEIGHTPOS	3
#define		SLANTPOS	4
#define		SETWIDTHNAMEPOS	5
#define		ADDSTYLENAMEPOS	6
#define		PIXSIZEPOS	7
#define		PTSIZEPOS	8
#define		XRESOLUTIONPOS	9

#define DEFAULT_SMALL_FONT		"-b&h-lucida-medium-r-normal-sans-*-100-*-*-*-*-*-*"
#define DEFAULT_SMALL_FONT_SIZE		10
#define DEFAULT_MEDIUM_FONT		"-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*"
#define DEFAULT_MEDIUM_FONT_SIZE	12
#define DEFAULT_LARGE_FONT		"-b&h-lucida-medium-r-normal-sans-*-140-*-*-*-*-*-*"
#define DEFAULT_LARGE_FONT_SIZE		14
#define DEFAULT_XLARGE_FONT		"-b&h-lucida-medium-r-normal-sans-*-190-*-*-*-*-*-*"
#define DEFAULT_XLARGE_FONT_SIZE	19

#ifndef _XVIEW_DEFAULT_FONT
#define DEFAULT_DEFAULT_FONT_NAME	"fixed"
#else
#define DEFAULT_DEFAULT_FONT_NAME	"xview-default-font"
#endif				/* _XVIEW_DEFAULT_FONT */

#define DEFAULT_FONT_NAME		"-b&h-lucida-medium-r-normal-sans-*-120-*-*-*-*-*-*"
							/* default:FAMILY-STYLE-S
							 * IZE */

#define DEFAULT_FONT_FAMILY		"lucida"
#define DEFAULT_FONT_FIXEDWIDTH_FAMILY	"lucidatypewriter"
#define DEFAULT_FONT_STYLE		""

/*
 * Default font weight, slant
 */
#define DEFAULT_FONT_WEIGHT		"medium"
#define DEFAULT_FONT_SLANT		"r"
#define DEFAULT_FONT_SETWIDTHNAME	"normal"
#define DEFAULT_FONT_ADDSTYLENAME	"sans"
#define DEFAULT_FONT_SCALE		(int) WIN_SCALE_MEDIUM

static int      font_default_scale = DEFAULT_FONT_SCALE;
static int      font_default_size = 12;
static char    *font_default_family = DEFAULT_FONT_FAMILY;
static char    *font_default_fixedwidth_family = DEFAULT_FONT_FIXEDWIDTH_FAMILY;
static char    *font_default_style = DEFAULT_FONT_STYLE;

static char    *font_default_weight = DEFAULT_FONT_WEIGHT;
static char    *font_default_slant = DEFAULT_FONT_SLANT;
static char    *font_default_setwidthname = DEFAULT_FONT_SETWIDTHNAME;
static char    *font_default_addstylename = DEFAULT_FONT_ADDSTYLENAME;

static char    *sunview1_prefix = "/usr/lib/fonts/fixedwidthfonts/";

Xv_private Xv_Font xv_font_with_name();
static void     font_default_font();
static char    *font_default_font_from_scale();
static char    *font_determine_font_name();
static char    *font_rescale_from_font();
static void     font_decrypt_family_style();
static void     font_decrypt_rescale_info();
static int      font_decrypt_sv1_name();
static int      font_decrypt_xv_name();
static int      font_read_attrs();
static int      font_string_compare();
static int      font_string_compare_nchars();

/* font naming compatibility */

struct font_translation {
    char           *sv1_name;
    char           *sv1_foundry;
    char           *sv1_family;
    char           *sv1_style;
    char           *sv1_weight;
    char           *sv1_slant;
    char           *sv1_setwidthname;
    char           *sv1_addstylename;

    int             size;
    int             scale;	/* medium resolution */
};

struct xv_to_x_font_translation {
    char           *xv_family;
    char           *xv_style;
    char           *x_font_foundry;
    char           *x_font_family;
    char           *x_font_style;
    char           *x_font_weight;
    char           *x_font_slant;
    char           *x_font_setwidthname;
    char           *x_font_addstylename;
};

struct font_definitions {
    char           *xv_family;
    char           *xv_style;
    char           *foundry;
    char           *family;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;

    int             small_size;	/* medium resolution */
    int             medium_size;
    int             large_size;
    int             extra_large_size;
};

struct font_return_attrs {
    char           *name;
    char           *family;
    char           *style;
    char           *foundry;
    char           *weight;
    char           *slant;
    char           *setwidthname;
    char           *addstylename;
    int             size;
    int             small_size;
    int             medium_size;
    int             large_size;
    int             extra_large_size;
    int             scale;
    Font_info      *resize_from_font;
    int             rescale_factor;
    int             free_name, free_family, free_style, 
		    free_weight, free_slant, free_foundry, 
		    free_setwidthname, free_addstylename;
};
typedef struct font_return_attrs *Font_return_attrs;

typedef struct style_definitions  {
    char	*style;
    char	*weight;
    char	*slant;
}Style_defs;

struct _unknown {
    struct _unknown *next;
    char           *name;
};
typedef struct _unknown *Unknown;

/*
 * Style defs - conversion table from style -> (weight, slant)
 * The 'real' style entry must precede the semantic entry.
 * i.e. the entry for 'bold' precedes 'FONT_STYLE_BOLD'
 * This is necessary so that when a (weight, slant) -> style
 * conversion is being done, the 'real' entry is matched first.
 */
static struct style_definitions	known_styles[] = {
	/*
	 * 'real' entries
	 */
	"normal", "medium", "r",
	"bold", "bold", "r",
	"italic", "medium", "i",
	"oblique", "medium", "o",
	"bolditalic", "bold", "i",
	"boldoblique", "bold", "o",

	/*
	 * semantic entries
	 */
	"FONT_STYLE_NORMAL", "medium", "r",
	"FONT_STYLE_BOLD", "bold", "r",
	"FONT_STYLE_ITALIC", "medium", "i",
	"FONT_STYLE_OBLIQUE", "medium", "o",
	"FONT_STYLE_BOLD_ITALIC", "bold", "i",
	"FONT_STYLE_BOLD_OBLIQUE", "bold", "o",
	0, 0, 0
    };

/*
 * Isa
 * Added foundry, weight, slant fields
 */
static struct font_translation known_sv1fonts[] = {
    "cmr.b.8", "*", "times", "bold", "bold", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.b.14", "*", "times", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cmr.r.8", "*", "times", 0, "medium", "r", "normal", "", 8, (int) WIN_SCALE_SMALL,
    "cmr.r.14", "*", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "cour.b.10", "*", "courier", "bold", "bold", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.b.12", "*", "courier", "bold", "bold", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.b.14", "*", "courier", "bold", "bold", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.b.16", "*", "courier", "bold", "bold", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.b.18", "*", "courier", "bold", "bold", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.b.24", "*", "courier", "bold", "bold", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "cour.r.10", "*", "courier", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "cour.r.12", "*", "courier", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_SMALL,
    "cour.r.14", "*", "courier", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_MEDIUM,
    "cour.r.16", "*", "courier", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_MEDIUM,
    "cour.r.18", "*", "courier", 0, "medium", "r", "normal", "", 18, (int) WIN_SCALE_LARGE,
    "cour.r.24", "*", "courier", 0, "medium", "r", "normal", "", 24, (int) WIN_SCALE_EXTRALARGE,
    "gallant.r.10", "b&h", "lucidatypewriter", 0, "medium", "r", "normal", "sans", 10, (int) WIN_SCALE_SMALL,
    "gallant.r.19", "b&h", "lucidatypewriter", 0, "medium", "r", "normal", "sans", 19, (int) WIN_SCALE_LARGE,
    "serif.r.10", "*", "times", 0, "medium", "r", "normal", "", 10, (int) WIN_SCALE_SMALL,
    "serif.r.11", "*", "times", 0, "medium", "r", "normal", "", 11, (int) WIN_SCALE_SMALL,
    "serif.r.12", "*", "times", 0, "medium", "r", "normal", "", 12, (int) WIN_SCALE_MEDIUM,
    "serif.r.14", "*", "times", 0, "medium", "r", "normal", "", 14, (int) WIN_SCALE_LARGE,
    "serif.r.16", "*", "times", 0, "medium", "r", "normal", "", 16, (int) WIN_SCALE_EXTRALARGE,
    0, 0, 0, 0, 0, 0, 0, 0, 0, (int) WIN_SCALE_MEDIUM
};

/*
 * Isa
 * Added weight, slant fields
 */
static struct xv_to_x_font_translation known_openwin_fonts[] = {
    FONT_FAMILY_OLGLYPH, "", "sun", "open look glyph", "", "", "", "", "",
    FONT_FAMILY_OLGLYPH, FONT_STYLE_NORMAL, "sun", "open look glyph", "", "", "", "", "",

    FONT_FAMILY_OLCURSOR, "", "sun", "open look cursor", "", "", "", "", "",
    FONT_FAMILY_OLCURSOR, FONT_STYLE_NORMAL, "sun", "open look cursor", "", "", "", "", "",

    FONT_FAMILY_LUCIDA, "", "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_NORMAL, "b&h", "lucida", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD, "b&h", "lucida", "bold", "bold", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_ITALIC, "b&h", "lucida", "italic", "medium", "i", "normal", "sans",
    FONT_FAMILY_LUCIDA, FONT_STYLE_BOLD_ITALIC, "b&h", "lucida", "bolditalic", "bold", "i", "normal", "sans",

    FONT_FAMILY_LUCIDA_FIXEDWIDTH, "", "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_NORMAL, "b&h", "lucidatypewriter", "", "medium", "r", "normal", "sans",
    FONT_FAMILY_LUCIDA_FIXEDWIDTH, FONT_STYLE_BOLD, "b&h", "lucidatypewriter", "bold", "bold", "r", "normal", "sans",

    /*
     * Isa
     * Added for each entry in table that had FONT_STYLE_NORMAL, an equivalent entry with
     * style = "". This is so that the matches of FONT_FAMILY = "whatever", FONT_STYLE =
     * "" are sucessful.
     */
    FONT_FAMILY_ROMAN, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_ROMAN, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_SERIF, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_SERIF, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_CMR, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_CMR, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_GALLENT, "", "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_NORMAL, "*", "times", "roman", "medium", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD, "*", "times", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_ITALIC, "*", "times", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_GALLENT, FONT_STYLE_BOLD_ITALIC, "*", "times", "bolditalic", "bold", "i", "normal", "",

    FONT_FAMILY_COUR, "", "*", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_NORMAL, "*", "courier", "", "medium", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD, "*", "courier", "bold", "bold", "r", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_ITALIC, "*", "courier", "italic", "medium", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_OBLIQUE, "*", "courier", "oblique", "medium", "o", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_ITALIC, "*", "courier", "bolditalic", "bold", "i", "normal", "",
    FONT_FAMILY_COUR, FONT_STYLE_BOLD_OBLIQUE, "*", "courier", "boldoblique", "bold", "o", "normal", "",

    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Font families that don't have sizes/style
 * So far only FONT_FAMILY_OLCURSOR
 */
static struct xv_to_x_font_translation size_style_less[] = {
    FONT_FAMILY_OLCURSOR, "", "sun", "open look cursor", "", "", "", "", "",
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*
 * Isa
 * Added weight, slant, setwidthname, addstylename fields
 * changed lucidasans to lucida
 */
static struct font_definitions known_xvfonts[] = {
    "lucida", 0, "b&h", "lucida", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bold", "b&h", "lucida", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    "lucida", "bolditalic", "b&h", "lucida", "bold", "i", "normal", "sans", 10, 12, 14, 19,
    "lucida", "italic", "b&h", "lucida", "medium", "i", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", 0, "b&h", "lucidatypewriter", "medium", "r", "normal", "sans", 10, 12, 14, 19,
    "lucidatypewriter", "bold", "b&h", "lucidatypewriter", "bold", "r", "normal", "sans", 10, 12, 14, 19,
    0, 0, 0, 0, 0, 0, 0, 0, 0
};

static char    *
normalize_font_name(name)
    register char  *name;
{
    if (name == NULL) {
	char           *default_scale;
	/*
	 * NULL for name => use default font name. Warning: Database may have
	 * "" rather than NULL.
	 */
	name = defaults_get_string("font.name", "Font.Name", NULL);
	if (name == NULL || (strlen(name) == 0)) {
	    default_scale = (char *) defaults_get_string("window.scale", 
						"Window.Scale", "Medium");
	    name = font_default_font_from_scale(default_scale);
	}
    }
    if (font_string_compare_nchars(name, sunview1_prefix,
				   strlen(sunview1_prefix)) == 0) {
	/* Map SunView1.X font name into server name. */
	name += strlen(sunview1_prefix);	/* index into str */
    }
    return (name);
}

static char    *
font_default_font_from_scale(scale)
    register char  *scale;
{
    if (!scale)
	return DEFAULT_MEDIUM_FONT;

    if ((font_string_compare(scale, "small") == 0) ||
	(font_string_compare(scale, "Small") == 0)) {
	return DEFAULT_SMALL_FONT;
    } else if ((font_string_compare(scale, "medium") == 0) ||
	       (font_string_compare(scale, "Medium") == 0)) {
	return DEFAULT_MEDIUM_FONT;
    } else if ((font_string_compare(scale, "large") == 0) ||
	       (font_string_compare(scale, "Large") == 0)) {
	return DEFAULT_LARGE_FONT;
    } else if ((font_string_compare(scale, "Extra_large") == 0) ||
	       (font_string_compare(scale, "Extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_Large") == 0) ||
	       (font_string_compare(scale, "extra_large") == 0)) {
	return DEFAULT_XLARGE_FONT;
    } else
	return DEFAULT_MEDIUM_FONT;
}

/* Called to free the font list when server is being destroyed. */
/*ARGSUSED*/
static void
font_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Font_info *font, *next;
    register int    ref_count;

    ASSERT(key == FONT_HEAD, _svdebug_always_on);
    for (font = (Font_info *) data; font; font = next) {
	next = font->next;	/* Paranoia in case xv_destroy is immediate */
	ref_count = (int) xv_get(FONT_PUBLIC(font), XV_REF_COUNT);
	if (ref_count == 0) {
	    xv_destroy(FONT_PUBLIC(font));
#ifdef _XV_DEBUG
	} else {
	    fprintf(stderr,
		    "Font %s has %d refs but server being destroyed.\n",
		    font->name, ref_count);
#endif
	}
    }
}

/* Called to free the unknown list when server is being destroyed. */
/*ARGSUSED*/
static void
font_unknown_list_free(server, key, data)
    Xv_object       server;
    Font_attribute  key;
    Xv_opaque       data;
{
    register Unknown unknown, next;

    ASSERT(key == FONT_UNKNOWN_HEAD, _svdebug_always_on);
    for (unknown = (Unknown) data; unknown; unknown = next) {
	next = unknown->next;
	free(unknown->name);
	free((char *) unknown);
    }
}


/*
 * Initialize font. This routine creates a new font (because xv_create always
 * allocates a new instance) and can not be used to get a handle to an
 * existing font.
 */
int
font_init(parent_public, font_public, avlist)
    Xv_opaque       parent_public;
    Xv_font_struct *font_public;
    Attr_avlist     avlist;
{
    XID 	    xid;
    Xv_opaque       display, x_font_info;
    register Font_info *font;
    register struct pixchar *pfc;
    int             font_attrs_exist;
    Xv_opaque       server;

    int             i, default_x, default_y, max_char, min_char;
#ifdef CHECK_OVERLAPPING_CHARS
    int             neg_left_bearing;
#endif
#ifdef CHECK_VARIABLE_HEIGHT
    int             variable_height_font;
#endif				/* CHECK_VARIABLE_HEIGHT */
    Font_info      *font_head;
    int             need_to_free_font_name;	/* font_name if malloc'd */
    char           *font_name = NULL;
    struct font_return_attrs my_attrs;

    /*
     * initialization
     */
    my_attrs.free_name = my_attrs.free_foundry = my_attrs.free_family = my_attrs.free_style 
		= my_attrs.free_weight = my_attrs.free_slant 
		= my_attrs.free_setwidthname 
		= my_attrs.free_addstylename = (int) 0;

    my_attrs.name = my_attrs.foundry = my_attrs.family = my_attrs.style 
			= my_attrs.weight = my_attrs.slant 
			= my_attrs.setwidthname = my_attrs.addstylename = (char *)0;
    /*
    my_attrs.small_size = my_attrs.medium_size = my_attrs.large_size =
	my_attrs.extra_large_size = (int) FONT_NO_SIZE;
    */
    my_attrs.size = (int) FONT_NO_SIZE;
    my_attrs.small_size = DEFAULT_SMALL_FONT_SIZE;
    my_attrs.medium_size = DEFAULT_MEDIUM_FONT_SIZE;
    my_attrs.large_size = DEFAULT_LARGE_FONT_SIZE;

    my_attrs.extra_large_size = DEFAULT_XLARGE_FONT_SIZE;
    my_attrs.scale = FONT_NO_SCALE;
    my_attrs.rescale_factor = (int) 0;
    my_attrs.resize_from_font = (Font_info *) 0;

    /*
     * Get the optional creation arguments
     */
    font_attrs_exist = font_read_attrs(&my_attrs, TRUE, avlist);
    if (!font_attrs_exist)
	(void) font_default_font(&my_attrs);

    if (!parent_public) {
	/* xv_create ensures that xv_default_server is valid. */
	parent_public = (Xv_opaque) xv_default_server;
	display = xv_get(parent_public, XV_DISPLAY);
	server = (Xv_opaque) xv_default_server;
    } else {
	Xv_pkg         *pkgtype = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	display = xv_get(parent_public, XV_DISPLAY);
	if (!display) {
	    if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_font_pkg) {
		Xv_Font         real_parent_public = 0;
		Font_info      *real_parent_private = 0;

		XV_OBJECT_TO_STANDARD(parent_public, "font_init", real_parent_public);
		real_parent_private = FONT_PRIVATE(real_parent_public);
		parent_public = real_parent_private->parent;
		display = real_parent_private->display;
	    } else
		display = xv_default_display;
	}
	if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_server_pkg) {
	    server = (Xv_opaque) parent_public;
	} else {
	    server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
	}
    }

    if (my_attrs.name) {
	char *name = my_attrs.name = normalize_font_name(my_attrs.name);
	if (font_decrypt_sv1_name(&my_attrs) == 0) {
	    font_name = font_determine_font_name(&my_attrs);
	    if ((font_name == NULL) || (strlen(font_name) == 0))
		my_attrs.name = name; /* reset */
	} else  {
	    /* not sv1 name */
	    /*
	     * If successfully decrypted xv name, try to construct
	     * xlfd name.
	     */
	    if (font_decrypt_xv_name(&my_attrs) == 0)  {
	        font_name = font_determine_font_name(&my_attrs);
	        if ((font_name == NULL) || (strlen(font_name) == 0))
		    my_attrs.name = name; /* reset */
	    }
	    else  {
		/*
		 * If not sv1 name, not xv name, check if xlfd name
		 * Do not have to call font_determine_font_name here 
		 * because the name is already in xlfd format.
		 */
		(void)font_decrypt_xlfd_name(&my_attrs);
	    }
	}
    } else if (my_attrs.resize_from_font) {
	font_name = font_rescale_from_font(
					(Font_info *) my_attrs.resize_from_font,
					   (int) my_attrs.rescale_factor,
					   &my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0))  {
	    char            dummy[128];

	    sprintf(dummy, "Attempt to rescale from font failed");
	    xv_error(NULL,
		     ERROR_STRING, dummy,
		     ERROR_PKG, FONT,
		     0);
	    return XV_ERROR;
	}
    } else {
	font_name = font_determine_font_name(&my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0)) {
	    char            dummy[128];

	    (void) sprintf(dummy, "Cannot load font '%s'",
			   (my_attrs.name) ? my_attrs.name : font_name);
	    xv_error(NULL,
		     ERROR_STRING, dummy,
		     ERROR_PKG, FONT,
		     0);
	    return XV_ERROR;
	} else
	    need_to_free_font_name = 1;	/* free name later */
    }

    xid = xv_load_x_font((Display *) display,
			 (my_attrs.name) ? my_attrs.name : font_name,
			 &x_font_info,
			 &default_x, &default_y,
			 &max_char, &min_char);
    if (xid == 0) {
	char            dummy[128];

	(void) sprintf(dummy, "Cannot load font '%s'",
		       (my_attrs.name) ? my_attrs.name : font_name);
	xv_error(NULL,
		 ERROR_STRING, dummy,
		 ERROR_PKG, FONT,
		 0);
	if (need_to_free_font_name && (font_name != (char *) 0))
	    free(font_name);
	return XV_ERROR;
    }

#ifdef CHECK_VARIABLE_HEIGHT
    font_check_var_height(&variable_height_font, &x_font_info);
#endif

#ifdef CHECK_OVERLAPPING_CHARS
    font_check_overlapping(&neg_left_bearing, &x_font_info);
#endif

    font = (Font_info *) xv_alloc(Font_info);

    /*
     * set forward and back pointers
     */
    font_public->private_data = (Xv_opaque) font;
    font->public_self = (Xv_opaque) font_public;
    font->parent = parent_public;

    font->display = display;
    font->server = server;
    font->pkg = ATTR_PKG_FONT;
    font->pixfont = (char *)NULL;
    font->xid = xid;
    font->def_char_width = ((XFontStruct *)x_font_info)->max_bounds.width;
    font->def_char_height = ((XFontStruct *)x_font_info)->ascent 
	+ ((XFontStruct *)x_font_info)->descent;
    font->x_font_info = x_font_info;

    (void) xv_set(font_public, XV_RESET_REF_COUNT, 0);
    font->type = FONT_TYPE_TEXT;

#ifdef CHECK_OVERLAPPING_CHARS
    if (neg_left_bearing
#ifdef CHECK_VARIABLE_HEIGHT
	|| variable_height_font
#endif				/* CHECK_VARIABLE_HEIGHT */
	)
	font->overlapping_chars = TRUE;
#endif				/* CHECK_OVERLAPPING_CHARS */

    if (my_attrs.foundry) {
	if (my_attrs.free_foundry) {
	    font->foundry = my_attrs.foundry;	/* take malloc'd ptr */
	} else
	    font->foundry = xv_strsave(my_attrs.foundry);
    }
    if (my_attrs.family) {
	if (my_attrs.free_family) {
	    font->family = my_attrs.family;	/* take malloc'd ptr */
	} else
	    font->family = xv_strsave(my_attrs.family);
    }
    if (my_attrs.style) {
	if (my_attrs.free_style) {
	    font->style = my_attrs.style;	/* take malloc'd ptr */
	} else
	    font->style = xv_strsave(my_attrs.style);
    }
    if (my_attrs.weight) {
	if (my_attrs.free_weight) {
	    font->weight = my_attrs.weight;	/* take malloc'd ptr */
	} else
	    font->weight = xv_strsave(my_attrs.weight);
    }
    if (my_attrs.slant) {
	if (my_attrs.free_slant) {
	    font->slant = my_attrs.slant;	/* take malloc'd ptr */
	} else
	    font->slant = xv_strsave(my_attrs.slant);
    }
    if (my_attrs.setwidthname) {
	if (my_attrs.free_setwidthname) {
	    font->setwidthname = my_attrs.setwidthname;	/* take malloc'd ptr */
	} else
	    font->setwidthname = xv_strsave(my_attrs.setwidthname);
    }
    if (my_attrs.addstylename) {
	if (my_attrs.free_addstylename) {
	    font->addstylename = my_attrs.addstylename;	/* take malloc'd ptr */
	} else
	    font->addstylename = xv_strsave(my_attrs.addstylename);
    }
    if (my_attrs.name) {
	if (my_attrs.free_name) {
	    font->name = my_attrs.name;
	} else
	    font->name = xv_strsave(my_attrs.name);
    } else if (font_name) {
	font->name = xv_strsave(font_name);
    }
    font->size = (int) my_attrs.size;
    font->scale = (int) my_attrs.scale;
    font->small_size = (int) my_attrs.small_size;
    font->medium_size = (int) my_attrs.medium_size;
    font->large_size = (int) my_attrs.large_size;
    font->extra_large_size = (int) my_attrs.extra_large_size;

    /*
     * Add new font to server's list
     */
    if (font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD)) {
	font->next = font_head->next;
	font_head->next = font;
    } else {
	font->next = (Font_info *) 0;
	(void) xv_set(server,
		      XV_KEY_DATA, FONT_HEAD, font,
		      XV_KEY_DATA_REMOVE_PROC, FONT_HEAD, font_list_free,
		      0);
    }

    /*
     * SunView1.X compatibility: set this font as default if appropriate.
     */
    if ((xv_pf_sys == (Pixfont *) 0) &&
	(parent_public == (Xv_opaque) xv_default_server)) {
	if ((font_string_compare(my_attrs.name,
				 normalize_font_name((char *)NULL)) == 0)
	/*
	 * || (font_string_compare(my_attrs.name, DEFAULT_DEFAULT_FONT_NAME)
	 * == 0)
	     */ ) {
	    xv_pf_sys = (Pixfont *)font_public;
	    (void) xv_set(font_public, XV_INCREMENT_REF_COUNT, 0);
	}
    }
    return XV_OK;
}

int
font_destroy_struct(font_public, status)
    Xv_font_struct *font_public;
    Destroy_status  status;
{
    register Font_info *font = FONT_PRIVATE(font_public);
    register Font_info *prev;
    register int    i;
    register struct pixchar
                   *pfc;
    Font_info      *font_head;
    Xv_opaque       display, server = font->server;
    Pixfont        *zap_font_public = 
	(Pixfont *) font->pixfont;

    if (status == DESTROY_CLEANUP) {
	/* PERFORMANCE ALERT: revive list package to consolidate code. */
	/* Remove the font from SunView's server list. */
	font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	if (!font_head) {
	    server = (Xv_opaque) xv_default_server;
	    font_head = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
	}
	if (((Xv_Font) FONT_PUBLIC(font_head)) == (Xv_Font) font_public) {
	    /* at head of list */
	    (void) xv_set(server, XV_KEY_DATA, FONT_HEAD, font->next, 0);
	} else {
	    for (prev = font_head; prev; prev = prev->next) {
		if (prev->next == font) {
		    prev->next = font->next;
		    break;
		}
	    }
#ifdef _XV_DEBUG
	    if (prev == 0)
		abort();
#endif
	}
	/* Free the storage allocated for glyphs. */
	if (zap_font_public)  {
	    XFontStruct	*x_font_info = (XFontStruct *)font->x_font_info;
            int max_char = MIN(255, x_font_info->max_char_or_byte2);
            int min_char = MIN(255, x_font_info->min_char_or_byte2);

	    for (i = min_char, pfc = &(zap_font_public->pf_char[i]);
	         i <= max_char; i++, pfc++) {
	        if (pfc->pc_pr) {
		    xv_mem_destroy(pfc->pc_pr);
	        }
	    }
	}
	/* free string storage */
	if (font->name)
	    free(font->name);
	if (font->foundry)
	    free(font->foundry);
	if (font->family)
	    free(font->family);
	if (font->style)
	    free(font->style);
	if (font->weight)
	    free(font->weight);
	if (font->slant)
	    free(font->slant);
	if (font->setwidthname)
	    free(font->setwidthname);
	if (font->addstylename)
	    free(font->addstylename);
	/* Remove the font from X server list, and free our private data. */
	display = font->display;
	if (!display)
	    display = (Xv_opaque) xv_get(xv_default_server, XV_DISPLAY);
	xv_unload_x_font((Display *) display, font->x_font_info);
	free(font);
    }
    return XV_OK;
}

Xv_private      Xv_Font
xv_font_with_name(server, name)
    Xv_opaque       server;
    char           *name;
{
    register Font_info *font, *font_list;
    Xv_Font         font_public;

    name = normalize_font_name(name);
    /*
     * Get "open fonts" list for specified server (may not be one yet), and
     * look to see if specified font has already been opened.
     */
    font_list = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD);
    font_public = (font_list) ? FONT_PUBLIC(font_list) : (Xv_Font) 0;
    for (font = font_list; font; font = font->next) {
	if (font_string_compare(name, font->name) == 0)
	    break;
    }

    if (font) {
	font_public = FONT_PUBLIC(font);
    } else {
	/*
	 * Get "unknown fonts" list (may not be one yet) and see if we have
	 * already tried to open specified font previously.
	 */
	Unknown         unknown_list = (Unknown) xv_get(server,
					    XV_KEY_DATA, FONT_UNKNOWN_HEAD);
	Unknown         unknown;
	for (unknown = unknown_list; unknown; unknown = unknown->next) {
	    if (font_string_compare(name, unknown->name) == 0)
		break;
	}
	if (unknown == 0) {
	    /*
	     * Try to open the font, since it is not yet available
	     */
	    font_public = xv_create(server, FONT,
				    FONT_NAME, name,
				    0);
	    if (!font_public) {
		/* Add this name to list of unknown fonts. */
		unknown = xv_alloc(struct _unknown);
		unknown->name = xv_strsave(name);
		if (unknown_list) {
		    unknown->next = unknown_list->next;
		    unknown_list->next = unknown;
		} else {
		    unknown->next = (Unknown) 0;
		    (void) xv_set(server,
				  XV_KEY_DATA, FONT_UNKNOWN_HEAD, unknown,
				  XV_KEY_DATA_REMOVE_PROC, FONT_UNKNOWN_HEAD,
				  font_unknown_list_free,
				  0);
		}
		/*
		 * Added comparison to DEFAULT_DEFAULT_FONT_NAME to prevent infinite loop
		 * of recursive calls
		 */
		if ((font_string_compare(name, DEFAULT_FONT_NAME) != 0) && 
		    (font_string_compare(name, DEFAULT_DEFAULT_FONT_NAME)) != 0)  {
		    char            dummy[128];

		    (void) sprintf(dummy, "Attempting to load font '%s%' instead",
				   DEFAULT_FONT_NAME);
		    xv_error(NULL,
			     ERROR_STRING, dummy,
			     ERROR_PKG, FONT,
			     0);
		    font_public =  xv_font_with_name(server, DEFAULT_FONT_NAME);
		    if (!font_public)  {
		        (void) sprintf(dummy, "Failed to load default font '%s%'",
				   DEFAULT_FONT_NAME);
		        xv_error(NULL,
			     ERROR_STRING, dummy,
			     ERROR_PKG, FONT,
			     0);
		    }
		    return (font_public);
		}
		if (font_string_compare(name, DEFAULT_DEFAULT_FONT_NAME) != 0) {
		    char            dummy[128];

		    (void) sprintf(dummy, "Loading default font '%s%' instead",
				   DEFAULT_DEFAULT_FONT_NAME);
		    xv_error(NULL,
			     ERROR_STRING, dummy,
			     ERROR_PKG, FONT,
			     0);
		    font_public =  xv_font_with_name(server, DEFAULT_DEFAULT_FONT_NAME);
		    if (!font_public)  {
		        (void) sprintf(dummy, "Failed to load default font '%s%'",
				   DEFAULT_DEFAULT_FONT_NAME);
		        xv_error(NULL,
			     ERROR_STRING, dummy,
			     ERROR_PKG, FONT,
			     0);
		    }
		    return (font_public);
		}

	    }
	} else {
	    /*
	     * Font known not to exist: return default.
	     */
	    if (font_string_compare(name, DEFAULT_FONT_NAME) == 0) {
		/* look for "fixed" font */
		return (xv_font_with_name(server, DEFAULT_DEFAULT_FONT_NAME));
	    } else
		return (xv_font_with_name(server, DEFAULT_FONT_NAME));
	}
    }
    return (font_public);
}

static char    *
font_determine_font_name(my_attrs)
    Font_return_attrs my_attrs;
{
    char            name[512], this_name[512];
    int             size;

    /*
     * Return null if no family/style specified
     */
    if ( (my_attrs->family == (char *) NULL)
	&& (my_attrs->style == (char *) NULL) 
	&& (my_attrs->weight == (char *) NULL) 
	&& (my_attrs->slant == (char *) NULL) )
	return (char *) NULL;

    if (font_string_compare(my_attrs->family, FONT_FAMILY_DEFAULT) == 0) {
	if (my_attrs->free_family) {
	    free(my_attrs->family);
	    my_attrs->free_family = 0;
	}
	if (font_default_family) {
	    my_attrs->family = xv_strsave(font_default_family);
	    my_attrs->free_family = 1;
	} else {
	    my_attrs->family = (char *) 0;
	    my_attrs->free_family = 0;
	}
    }
    if (font_string_compare(my_attrs->family, FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0) {
	if (my_attrs->free_family) {
	    free(my_attrs->family);
	    my_attrs->free_family = 0;
	}
	if (font_default_fixedwidth_family) {
	    my_attrs->family = xv_strsave(font_default_fixedwidth_family);
	    my_attrs->free_family = 1;
	} else {
	    my_attrs->family = (char *) 0;
	    my_attrs->free_family = 0;
	}
    }
    if (font_string_compare(my_attrs->style, FONT_STYLE_DEFAULT) == 0) {
	if (my_attrs->free_style) {
	    free(my_attrs->style);
	    my_attrs->free_style = 0;
	}
	if (font_default_style) {
	    my_attrs->style = xv_strsave(font_default_style);
	    my_attrs->free_style = 1;
	} else {
	    my_attrs->style = (char *) 0;
	    my_attrs->free_style = 0;
	}
    }
    /*
     * Convert font style into weight and slant
     */
    if ((my_attrs->style) && (!(my_attrs->weight) || !(my_attrs->slant)))  {
        if (font_convert_style(my_attrs))  {
	    char	dummy[128];

	    /*
	     * If cannot convert style into any weight/slant combination
	     * give 'normal' style - medium/roman
	     */
	    sprintf(dummy, "Font style %s is not known, using style 'normal' instead",
		my_attrs->style);
	    xv_error(NULL,
		    ERROR_STRING, dummy,
		    ERROR_PKG, FONT,
		    0);

	    if (my_attrs->free_style)  {
	        free(my_attrs->style);
	        my_attrs->free_style = 0;
	    }
	    if (my_attrs->free_weight)  {
	        free(my_attrs->weight);
	        my_attrs->free_weight = 0;
	    }
	    if (my_attrs->free_slant)  {
	        free(my_attrs->slant);
	        my_attrs->free_slant = 0;
	    }
	    my_attrs->style = xv_strsave("normal");
	    my_attrs->free_style = 1;
	    my_attrs->weight = xv_strsave("medium");
	    my_attrs->free_weight = 1;
	    my_attrs->slant = xv_strsave("r");
	    my_attrs->free_slant = 1;
	}
    }

    if (my_attrs->size == FONT_SIZE_DEFAULT) {
	my_attrs->size = font_size_from_scale_and_sizes(
							my_attrs->scale,
	     my_attrs->small_size = (my_attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : my_attrs->small_size,
	   my_attrs->medium_size = (my_attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : my_attrs->medium_size,
	     my_attrs->large_size = (my_attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : my_attrs->large_size,
							my_attrs->extra_large_size = (my_attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : my_attrs->extra_large_size);
	if (my_attrs->size == FONT_NO_SIZE)
	    my_attrs->size = font_default_size;
    }

    /*
     * If just check scale without checking size, the scale will precede over 
     * whatever size is specified. So, do both.
     */
    if ((my_attrs->scale == FONT_SCALE_DEFAULT) && (my_attrs->size < 0)) {
	my_attrs->scale = DEFAULT_FONT_SCALE;
	my_attrs->size = font_size_from_scale_and_sizes(
							my_attrs->scale,
	     my_attrs->small_size = (my_attrs->small_size == FONT_NO_SIZE) ?
			     DEFAULT_SMALL_FONT_SIZE : my_attrs->small_size,
	   my_attrs->medium_size = (my_attrs->medium_size == FONT_NO_SIZE) ?
			   DEFAULT_MEDIUM_FONT_SIZE : my_attrs->medium_size,
	     my_attrs->large_size = (my_attrs->large_size == FONT_NO_SIZE) ?
			     DEFAULT_LARGE_FONT_SIZE : my_attrs->large_size,
							my_attrs->extra_large_size = (my_attrs->extra_large_size == FONT_NO_SIZE) ?
		     DEFAULT_XLARGE_FONT_SIZE : my_attrs->extra_large_size);
	if (my_attrs->size == FONT_NO_SIZE)
	    my_attrs->size = font_default_size;
    }

    size = ((my_attrs->size != FONT_NO_SIZE) || (my_attrs->size >= 0))
	? my_attrs->size : font_size_from_scale_and_sizes(
							  my_attrs->scale,
						       my_attrs->small_size,
						      my_attrs->medium_size,
						       my_attrs->large_size,
						my_attrs->extra_large_size);

    this_name[0] = '\0';
    name[0] = '\0';

    sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(my_attrs->foundry ? my_attrs->foundry:"*"), 
			(my_attrs->family ? my_attrs->family:"*"), 
			(my_attrs->weight ? my_attrs->weight:"*"), 
			(my_attrs->slant ? my_attrs->slant:"*"), 
			(my_attrs->setwidthname ? my_attrs->setwidthname:"*"),
			(my_attrs->addstylename ? my_attrs->addstylename:"*") );

    /*
     * If at this time, we still don't have a usable size, use the default
     */
    if ((size != FONT_NO_SIZE) && (size > 0)) {
	my_attrs->size = size;
    } else  {
	my_attrs->size = size = font_default_size;
    }

    sprintf(this_name, "%s-*-%d-*-*-*-*-*-*", name, (10*size));
    
    /*
     * Isa
     * Check if no important attrs managed to be filled in
     * BUG!!?? What are important attributes. The list below
     * is probably too long a list. Perhaps family, style 
     * alone is sufficient
    if ( !(my_attrs->family) &&
	 !(my_attrs->weight) &&
	 !(my_attrs->slant) )  {

	return (char *) NULL;
    }
     */

    if (this_name[0]) {		/* any name constructed */
	my_attrs->name = xv_strsave(this_name);
	my_attrs->free_name = 1;
	if (font_decrypt_sv1_name(my_attrs) != 0) {
	    /* not sv1 name */
	    if (font_decrypt_xv_name(my_attrs) != 0)
		/* not xv name */
		font_decrypt_xlfd_name(my_attrs);
	};
	return (char *) my_attrs->name;
    } else
	return (char *) NULL;
}

static int
font_size_from_scale_and_sizes(scale, small, med, large, xlarge)
    int             scale, small, med, large, xlarge;
{
    switch (scale) {
      case WIN_SCALE_SMALL:
	if (small == FONT_NO_SIZE)
	    return DEFAULT_SMALL_FONT_SIZE;
	return small;
      case WIN_SCALE_MEDIUM:
	if (med == FONT_NO_SIZE)
	    return DEFAULT_MEDIUM_FONT_SIZE;
	return med;
      case WIN_SCALE_LARGE:
	if (large == FONT_NO_SIZE)
	    return DEFAULT_LARGE_FONT_SIZE;
	return large;
      case WIN_SCALE_EXTRALARGE:
	if (xlarge == FONT_NO_SIZE)
	    return DEFAULT_XLARGE_FONT_SIZE;
	return xlarge;
      default:
	return FONT_NO_SIZE;
    }
}

static char    *
font_rescale_from_font(font, scale, attrs)
    Font_info      *font;
    int             scale;
    struct font_return_attrs *attrs;
{
    char           *font_name = NULL;
    char            new_name[256], name[512];
    int             desired_scale;

    if (!font)			/* if possibly not set? */
	return (char *) font_name;
    name[0] = '\0';
    if ((scale < (int) WIN_SCALE_SMALL) ||
	(scale > (int) WIN_SCALE_EXTRALARGE) ||
	(scale == FONT_NO_SCALE))  {
	char	dummy[128];

	sprintf(dummy, "Bad scale value:%d", (int)scale);
	xv_error(NULL,
	        ERROR_STRING, dummy,
	        ERROR_PKG, FONT,
	        0);
	return (char *) font_name;	/* no scaling */
    }

    if ((font->family == 0) && (font->style == 0) && 
		(font->weight == 0) && (font->slant == 0)) {
	if (font->name == 0)
	    return (char *) font_name;	/* you're really out of luck! */
	/*
	 * Try to decrypt the attributes from the name
	 */
	if (font_decrypt_xlfd_name(attrs) == -1)  {
	    return (char *) font_name;	/* you're REALLY out of luck! */
	}
    }
    
	/* munch everything together */
        sprintf(name, "-%s-%s-%s-%s-%s-%s",
			(font->foundry ? font->foundry:"*"), 
			(font->family ? font->family:"*"), 
			(font->weight ? font->weight:"*"), 
			(font->slant ? font->slant:"*"), 
			(font->setwidthname ? font->setwidthname:"*"),
			(font->addstylename ? font->addstylename:"*") );


	if (font->foundry) {
		attrs->foundry = xv_strsave(font->foundry);
		attrs->free_foundry = 1;
	}
	if (font->family) {
		attrs->family = xv_strsave(font->family);
		attrs->free_family = 1;
	}
	if (font->style) {
		attrs->style = xv_strsave(font->style);
		attrs->free_style = 1;
	}
	if (font->weight) {
		attrs->weight = xv_strsave(font->weight);
		attrs->free_weight = 1;
	}
	if (font->slant) {
		attrs->slant = xv_strsave(font->slant);
		attrs->free_slant = 1;
	}
	if (font->setwidthname) {
		attrs->setwidthname = xv_strsave(font->setwidthname);
		attrs->free_setwidthname = 1;
	}
	if (font->addstylename) {
		attrs->addstylename = xv_strsave(font->addstylename);
		attrs->free_addstylename = 1;
	}

    switch (scale) {
      case WIN_SCALE_SMALL:
	desired_scale = font->small_size;
	break;
      case WIN_SCALE_MEDIUM:
	desired_scale = font->medium_size;
	break;
      case WIN_SCALE_LARGE:
	desired_scale = font->large_size;
	break;
      case WIN_SCALE_EXTRALARGE:
	desired_scale = font->extra_large_size;
	break;
      default:
	desired_scale = -1;
    }
    if (desired_scale == -1)
	return (char *) font_name;	/* no font that scale */
    new_name[0] = '\0';

    /*
     * If cannot get a size, give the default
     */
    if ((desired_scale == FONT_NO_SIZE) || (desired_scale <= 0)) {
	desired_scale = font_default_size;
    }
    
    sprintf(new_name, "%s-*-%d-*-*-*-*-*-*", name, (10*desired_scale));

    attrs->name = xv_strsave(new_name);
    attrs->free_name = 1;
    attrs->size = desired_scale;
    attrs->scale = scale;
    attrs->small_size = font->small_size;
    attrs->medium_size = font->medium_size;
    attrs->large_size = font->large_size;
    attrs->extra_large_size = font->extra_large_size;

    return (attrs->name);
}


/*ARGSUSED*/
Xv_object
font_find_font(parent_public, pkg, avlist)
    Xv_opaque       parent_public;
    Xv_pkg         *pkg;
    Attr_avlist     avlist;
{
    int             font_attrs_exist;
    struct font_info *font_list;
    struct font_return_attrs my_attrs;
    char           *font_name = (char *) NULL;
    char           *this_name;
    int             need_to_free_font_name = 0;
    Xv_opaque       server;

    /* initialization */
    my_attrs.free_name = my_attrs.free_foundry = my_attrs.free_family = my_attrs.free_style 
		= my_attrs.free_weight = my_attrs.free_slant 
		= my_attrs.free_setwidthname 
		= my_attrs.free_addstylename = (int) 0;

    my_attrs.name = my_attrs.foundry = my_attrs.family 
			= my_attrs.style = my_attrs.weight 
			= my_attrs.slant = my_attrs.setwidthname 
			= my_attrs.addstylename = (char *)0;
    my_attrs.size = (int) FONT_NO_SIZE;
    /*
    my_attrs.small_size = my_attrs.medium_size = my_attrs.large_size =
	my_attrs.extra_large_size = (int) FONT_NO_SIZE;
    */
    my_attrs.small_size = DEFAULT_SMALL_FONT_SIZE;
    my_attrs.medium_size = DEFAULT_MEDIUM_FONT_SIZE;
    my_attrs.large_size = DEFAULT_LARGE_FONT_SIZE;
    my_attrs.extra_large_size = DEFAULT_XLARGE_FONT_SIZE;

    my_attrs.scale = FONT_NO_SCALE;
    my_attrs.rescale_factor = (int) 0;
    my_attrs.resize_from_font = (Font_info *) 0;

    font_attrs_exist = font_read_attrs(&my_attrs, FALSE, avlist);	/* get attrs */
    if (!font_attrs_exist)
	(void) font_default_font(&my_attrs);

    if (!parent_public) {
	/* xv_create/xv_find ensures that xv_default_server is valid. */
	server = (Xv_opaque) xv_default_server;
    } else {
	Xv_pkg         *pkgtype = (Xv_pkg *) xv_get(parent_public, XV_TYPE);
	if ((Xv_pkg *) pkgtype == (Xv_pkg *) & xv_server_pkg) {
	    server = parent_public;
	} else {
	    server = (Xv_opaque) XV_SERVER_FROM_WINDOW(parent_public);
	}
    }

    if (my_attrs.name) {
	my_attrs.name = normalize_font_name(my_attrs.name);
	if (font_decrypt_sv1_name(&my_attrs) != 0) {
	    /* not sv1 name */
	    if (font_decrypt_xv_name(&my_attrs) != 0) {
		/* not xv name */
		font_decrypt_xlfd_name(&my_attrs);
	    }
	}
    } else if (my_attrs.resize_from_font) {
	font_name = font_rescale_from_font(
					(Font_info *) my_attrs.resize_from_font,
					   (int) my_attrs.rescale_factor,
					   &my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0))
	    return ((Xv_object) 0);
    } else {
	font_name = font_determine_font_name(&my_attrs);
	if ((font_name == NULL) || (strlen(font_name) == 0)) {
	    return ((Xv_object) 0);
	} else {
	    font_name = xv_strsave(font_name);
	    need_to_free_font_name = 1;	/* free name later */
	}
    }

    this_name = (font_name) ? font_name : my_attrs.name;
    if (font_list = (Font_info *) xv_get(server, XV_KEY_DATA, FONT_HEAD)) {
	while (font_list) {
	    if (((font_string_compare(this_name, font_list->name) == 0)
		 && (this_name != (char *) 0)
		 && (font_list->name != (char *) 0))
	    /*
	     * first above for name, else below for family/style/size/scale
	     * match
	     */
		|| (((font_string_compare(my_attrs.family, font_list->family) == 0)
		     && (my_attrs.family != (char *) 0)
		     && (font_list->family != (char *) 0))
	     && (font_string_compare(my_attrs.style, font_list->style) == 0)
		    && ((font_list->size == my_attrs.size) &&
			(font_list->scale == my_attrs.scale)))) {
		if (need_to_free_font_name) {
		    free(font_name);
		}
		font_free_font_return_attr_strings(&my_attrs);
		(void) xv_set(FONT_PUBLIC(font_list), XV_INCREMENT_REF_COUNT, 0);
		return (FONT_PUBLIC(font_list));
	    }
	    font_list = font_list->next;
	}
    }
    return ((Xv_object) 0);
}


font_free_font_return_attr_strings(attrs)
    struct font_return_attrs *attrs;
{
    if (attrs->free_name) {
	free(attrs->name);
	attrs->free_name = 0;
    }
    if (attrs->free_foundry) {
	free(attrs->foundry);
	attrs->free_foundry = 0;
    }
    if (attrs->free_family) {
	free(attrs->family);
	attrs->free_family = 0;
    }
    if (attrs->free_style) {
	free(attrs->style);
	attrs->free_style = 0;
    }
    if (attrs->free_weight) {
	free(attrs->weight);
	attrs->free_weight = 0;
    }
    if (attrs->free_slant) {
	free(attrs->slant);
	attrs->free_slant = 0;
    }
    if (attrs->free_setwidthname) {
	free(attrs->setwidthname);
	attrs->free_setwidthname = 0;
    }
    if (attrs->free_addstylename) {
	free(attrs->addstylename);
	attrs->free_addstylename = 0;
    }
}

/*
 * the following proc is a wrapper for strcmp() strncmp() such that it will
 * return =0 if both strings are NULL and !=0 if one or other is NULL, and
 * standard strcmp otherwise. BUG: strcmp will seg fault if either str is
 * NULL.
 */
static int
font_string_compare(str1, str2)
    char           *str1, *str2;
{
    if ((str1 == NULL) && (str2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((str1 == NULL) || (str2 == NULL)) {
	return (int) -1;	/* avoid seg fault */
    } else
	return (int) strcmp(str1, str2);
}

static int
font_string_compare_nchars(str1, str2, n_chars)
    char           *str1, *str2;
    int             n_chars;
{
    int             result;
    int             len1 = (str1) ? strlen(str1) : 0;
    int             len2 = (str2) ? strlen(str2) : 0;
    if ((len1 == 0) && (len2 == NULL)) {
	return (int) 0;		/* they're the same (ie. nothing */
    } else if ((len1 && !len2) || (!len1 && len2)) {
	return (int) -1;	/* They're different strings */
    } else if ((!len1) || (!len2)) {
	return (int) -1;	/* avoid seg fault */
    } else
	result = strncmp(str1, str2, n_chars);
    return (int) result;
}

static int
font_decrypt_sv1_name(my_attrs)
    Font_return_attrs my_attrs;
{
    struct font_translation current_entry;
    int             i;

    if (my_attrs->name == NULL)
	return (0);
    for (i = 0, current_entry = known_sv1fonts[i];
	 current_entry.sv1_name;
	 current_entry = known_sv1fonts[i]) {
	if (font_string_compare_nchars(current_entry.sv1_name, my_attrs->name,
				     strlen(current_entry.sv1_name)) == 0) {
	    my_attrs->foundry = current_entry.sv1_foundry;

	    my_attrs->family = current_entry.sv1_family;
	    my_attrs->style = current_entry.sv1_style;
	    my_attrs->weight = current_entry.sv1_weight;
	    my_attrs->slant = current_entry.sv1_slant;
	    my_attrs->setwidthname = current_entry.sv1_setwidthname;
	    my_attrs->addstylename = current_entry.sv1_addstylename;

	    my_attrs->size = current_entry.size;
	    my_attrs->scale = current_entry.scale;
	    return (0);
	}
	i++;
    }
    return (-1);
}

static int
font_decrypt_xv_name(my_attrs)
    Font_return_attrs my_attrs;
{
    struct font_definitions current_entry;
    int             i;

    char            family_style[128], nosize_entry[128], small_entry[128], 
		    med_entry[128], large_entry[128], xlarge_entry[128];
    int             len_of_name, match = 0;

    if (my_attrs->name == NULL)
	return (0);
    len_of_name = strlen(my_attrs->name);
    for (i = 0, current_entry = known_xvfonts[i];
	 current_entry.xv_family;
	 current_entry = known_xvfonts[i]) {
	family_style[0] = '\0';
	nosize_entry[0] = '\0';
	small_entry[0] = '\0';
	med_entry[0] = '\0';
	large_entry[0] = '\0';
	xlarge_entry[0] = '\0';

	if (current_entry.xv_style != (char *) 0) {
	    (void) sprintf(family_style, "%s-%s",
			current_entry.xv_family,
			current_entry.xv_style);
	}
	else  {
	    (void) sprintf(family_style, "%s",
			current_entry.xv_family);
			
	}

	sprintf(nosize_entry, "%s", family_style);
	sprintf(small_entry, "%s-%d", family_style, current_entry.small_size);
	sprintf(med_entry, "%s-%d", family_style, current_entry.medium_size);
	sprintf(large_entry, "%s-%d", family_style, current_entry.large_size);
	sprintf(xlarge_entry, "%s-%d", family_style, current_entry.extra_large_size);
	if (font_string_compare_nchars(small_entry, my_attrs->name,
			      MAX(strlen(small_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.small_size;
	    my_attrs->scale = (int) WIN_SCALE_SMALL;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(med_entry, my_attrs->name,
				MAX(strlen(med_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.medium_size;
	    my_attrs->scale = (int) WIN_SCALE_MEDIUM;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(large_entry, my_attrs->name,
			      MAX(strlen(large_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.large_size;
	    my_attrs->scale = (int) WIN_SCALE_LARGE;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(xlarge_entry, my_attrs->name,
			     MAX(strlen(xlarge_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = current_entry.extra_large_size;
	    my_attrs->scale = (int) WIN_SCALE_EXTRALARGE;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	} else if (font_string_compare_nchars(nosize_entry, my_attrs->name,
			     MAX(strlen(nosize_entry), len_of_name)) == 0) {
	    my_attrs->foundry = current_entry.foundry;
	    my_attrs->family = current_entry.family;
	    my_attrs->weight = current_entry.weight;
	    my_attrs->slant = current_entry.slant;
	    my_attrs->setwidthname = current_entry.setwidthname;
	    my_attrs->addstylename = current_entry.addstylename;

	    my_attrs->style = current_entry.xv_style;
	    my_attrs->size = font_default_size;
	    my_attrs->scale = font_default_scale;
	    my_attrs->small_size = current_entry.small_size;
	    my_attrs->medium_size = current_entry.medium_size;
	    my_attrs->large_size = current_entry.large_size;
	    my_attrs->extra_large_size = current_entry.extra_large_size;
	    match = 1;
	}

	if (match)  {
	    return(0);
	}

	i++;
    }
    return (-1);
}

static void
font_decrypt_family_style(return_attrs)
    Font_return_attrs return_attrs;
{
    struct xv_to_x_font_translation current_entry;
    register int    i;
    char           *requested_family = return_attrs->family;
    char           *requested_style = return_attrs->style;

    /*
     * If family/style is not specified, give the default
     */
    if (!(requested_family) && !(return_attrs->name))  {
	requested_family = return_attrs->family = FONT_FAMILY_DEFAULT;
    }

    if (!(requested_style) && !(return_attrs->name))  {
	requested_style = return_attrs->style = FONT_STYLE_DEFAULT;
    }

    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT) == 0)
	requested_family = return_attrs->family
	    = font_default_family;
    if (font_string_compare(requested_family,
			    FONT_FAMILY_DEFAULT_FIXEDWIDTH) == 0)
	requested_family = return_attrs->family
	    = font_default_fixedwidth_family;
    if (!requested_family)
	requested_family = "";	/* avoid segv in strlen */
    if (font_string_compare(requested_style,
			    FONT_STYLE_DEFAULT) == 0)
	requested_style = return_attrs->style
	    = font_default_style;
    if (!requested_style)
	requested_style = "";	/* avoid segv in strlen */
    /* otherwise, leave family/style alone */

    if (requested_family && strlen(requested_family)) {
	for (i = 0, current_entry = known_openwin_fonts[i];
	     current_entry.xv_family;
	     current_entry = known_openwin_fonts[i]) {

	    if (((font_string_compare_nchars(current_entry.xv_family,
			   requested_family, strlen(requested_family)) == 0)
		 || (font_string_compare_nchars(current_entry.x_font_family,
			  requested_family, strlen(requested_family)) == 0))
		&& ((font_string_compare_nchars(current_entry.xv_style,
			     requested_style, strlen(requested_style)) == 0)
		  || (font_string_compare_nchars(current_entry.x_font_style,
			 requested_style, strlen(requested_style)) == 0))) {
		/* assign x font names */

		return_attrs->foundry = current_entry.x_font_foundry;
		return_attrs->family = current_entry.x_font_family;
		return_attrs->style = current_entry.x_font_style;
		return_attrs->weight = current_entry.x_font_weight;
		return_attrs->slant = current_entry.x_font_slant;
		return_attrs->setwidthname = current_entry.x_font_setwidthname;
		return_attrs->addstylename = current_entry.x_font_addstylename;
		return;
	    } else
		i++;
	}
    }
}

static void
font_decrypt_rescale_info(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine family-style of font wished to rescaled against */
    if (return_attrs->resize_from_font) {
	if (!return_attrs->family && return_attrs->resize_from_font->family)
	    return_attrs->family = return_attrs->resize_from_font->family;
	if (!return_attrs->style && return_attrs->resize_from_font->style)
	    return_attrs->style = return_attrs->resize_from_font->style;
	return_attrs->scale = return_attrs->rescale_factor;
	(void) font_decrypt_family_style(return_attrs);
    }
}

static void
font_decrypt_size(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine size */
    if (return_attrs->size == FONT_SIZE_DEFAULT) {
	return_attrs->size = (int) font_default_size;
    }
}

static void
font_decrypt_scale(return_attrs)
    Font_return_attrs return_attrs;
{
    /* determine scale */
    if (return_attrs->scale == FONT_SCALE_DEFAULT) {
	return_attrs->scale = (int) font_default_scale;
    }
}

static void
font_check_size_style_less(return_attrs)
    Font_return_attrs return_attrs;
{
    struct xv_to_x_font_translation current_entry;
    register int    i;
    char           *requested_family = return_attrs->family;
    char	name[256];

    if (!(return_attrs->name) && requested_family && strlen(requested_family)) {
	for (i = 0, current_entry = size_style_less[i];
	     current_entry.xv_family;
	     current_entry = size_style_less[i]) {

	    if (((font_string_compare_nchars(current_entry.xv_family,
			   requested_family, strlen(requested_family)) == 0)
		 || (font_string_compare_nchars(current_entry.x_font_family,
			  requested_family, strlen(requested_family)) == 0))) {
		/* assign x font names */

		return_attrs->foundry = current_entry.x_font_foundry;
		return_attrs->family = current_entry.x_font_family;

                sprintf(name, "-%s-%s-*-*-*-*-*-*-*-*-*-*-*-*",
		    (return_attrs->foundry ? return_attrs->foundry:"*"), 
		    (return_attrs->family ? return_attrs->family:"*") );
                return_attrs->name = xv_strsave(name);
                return_attrs->free_name = 1;

		return;
	    } else
		i++;
	}
    }
}

static int
font_read_attrs(return_attrs, consume_attrs, avlist)
    Font_return_attrs return_attrs;
    int             consume_attrs;
    Attr_avlist     avlist;
{
    register Attr_avlist attrs;
    int             font_attrs_exist = 0;

    for (attrs = avlist; *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case FONT_NAME:
	    return_attrs->name = (char *) attrs[1];
	    font_attrs_exist = 1;
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_FAMILY:
	    font_attrs_exist = 1;
	    return_attrs->family = (char *) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_STYLE:
	    font_attrs_exist = 1;
	    return_attrs->style = (char *) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SIZE:
	    font_attrs_exist = 1;
	    return_attrs->size = (int) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SCALE:
	    font_attrs_exist = 1;
	    return_attrs->scale = (int) attrs[1];
	    if (consume_attrs)
		ATTR_CONSUME(attrs[0]);
	    break;
	  case FONT_SIZES_FOR_SCALE:{
		font_attrs_exist = 1;
		return_attrs->small_size = (int) attrs[1];
		return_attrs->medium_size = (int) attrs[2];
		return_attrs->large_size = (int) attrs[3];
		return_attrs->extra_large_size = (int) attrs[4];
		if (consume_attrs)
		    ATTR_CONSUME(attrs[0]);
		break;
	    }
	  case FONT_RESCALE_OF:{
		Xv_opaque       pf = (Xv_opaque) attrs[1];
		Xv_Font         font = 0;

		XV_OBJECT_TO_STANDARD(pf, "font_read_attrs", font);
		font_attrs_exist = 1;
		return_attrs->resize_from_font = (Font_info *) FONT_PRIVATE(
								      font);
		return_attrs->rescale_factor = (int) attrs[2];
		if (consume_attrs)
		    ATTR_CONSUME(attrs[0]);
		break;
	    }
	  default:
	    break;
	}
    }

    if (!font_attrs_exist) {
	return (font_attrs_exist);
    } else {
	font_decrypt_family_style(return_attrs);
	font_decrypt_size(return_attrs);
	font_decrypt_scale(return_attrs);
	font_decrypt_rescale_info(return_attrs);
        font_check_size_style_less(return_attrs);
	return (font_attrs_exist);
    }
}

static void
font_default_font(return_attrs)
    Font_return_attrs return_attrs;
{
    return_attrs->family = font_default_family;
    return_attrs->style = font_default_style;
    return_attrs->weight = font_default_weight;
    return_attrs->slant = font_default_slant;
    return_attrs->setwidthname = font_default_setwidthname;
    return_attrs->addstylename = font_default_addstylename;
    return_attrs->size = font_default_size;
    return_attrs->scale = (int) font_default_scale;
}

font_convert_style(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_style;

    len_of_style = (return_attrs->style) ? strlen(return_attrs->style):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if (font_string_compare_nchars(current_entry.style, return_attrs->style,
			      MAX(strlen(current_entry.style), len_of_style)) == 0) {

	    return_attrs->weight = current_entry.weight;
	    return_attrs->slant = current_entry.slant;

	    return(0);
	}
    }

    return(-1);
}

font_convert_weightslant(return_attrs)
Font_return_attrs return_attrs;
{
    Style_defs	current_entry;
    int		i, len_of_weight, len_of_slant;

    len_of_weight = (return_attrs->weight) ? strlen(return_attrs->weight):0;
    len_of_slant = (return_attrs->slant) ? strlen(return_attrs->slant):0;

    for (i=0, current_entry = known_styles[i]; current_entry.weight; 
		current_entry = known_styles[++i])  {
	if ( (font_string_compare_nchars(current_entry.weight, return_attrs->weight,
			      MAX(strlen(current_entry.weight), len_of_weight)) == 0) &&
	     (font_string_compare_nchars(current_entry.slant, return_attrs->slant,
			      MAX(strlen(current_entry.slant), len_of_slant)) == 0) ) {

	    return_attrs->style = current_entry.style;

	    return(0);
	}
    }

    return(-1);
}


int
font_dashcount(str)
char	*str;
{
    register char	*p1, *p2 = str;
    int		count = 0;

    if (str == NULL)  {
	return(0);
    }

    p1 = index(str, DASH);

    if (p1 == NULL)  {
	return(0);
    }

    ++count;

    while(p2 != NULL)  {
	p2 = index(p1+1, DASH);

	if (p2 != NULL)  {
	    ++count;
	    p1 = p2;
	}
    }

    return(count);
}

char *
strip_xlfd(str, pos)
char	*str;
int	pos;
{
    char	*p1 = str, *p2;

    if ((str == NULL) || (pos < 0))  {
	return((char *)NULL);
    }
    
    while(pos)  {
	p2 = index(p1, DASH);
	if (p2 == NULL)  {
	    return((char *)NULL);
	}
	--pos;
	p1 = p2 + 1;
    }

    /*
     * the string at position `pos` spans p1 to 
     * strip_xlfd(str, pos+1)-1
     */
    return(p1);
}

static int
font_decrypt_xlfd_name(my_attrs)
Font_return_attrs my_attrs;
{
    int			tempSize;
    int			medsize, largesize, xlargesize;
    char		tempName[255];
    char		*foundry = NULL;
    char		*family = NULL;
    char		*weight = NULL;
    char		*slant = NULL;
    char		*setwidthname = NULL;
    char		*addstylename = NULL;
    char		*pixsize = NULL;
    char		*ptsize = NULL;
    char		*xres = NULL;

    if (my_attrs->name == NULL)  {
	return(-1);
    }

    if (font_dashcount(my_attrs->name) != NUMXLFDFIELDS)  {
	return(-1);
    }

    sprintf(tempName, "%s", my_attrs->name);

    foundry = strip_xlfd(tempName, FOUNDRYPOS);
    family = strip_xlfd(tempName, FAMILYPOS);
    weight = strip_xlfd(tempName, WEIGHTPOS);
    slant = strip_xlfd(tempName, SLANTPOS);
    setwidthname = strip_xlfd(tempName, SETWIDTHNAMEPOS);
    addstylename = strip_xlfd(tempName, ADDSTYLENAMEPOS);
    pixsize = strip_xlfd(tempName, PIXSIZEPOS);
    ptsize = strip_xlfd(tempName, PTSIZEPOS);
    xres = strip_xlfd(tempName, XRESOLUTIONPOS);

    *(family-1) = '\0';
    *(weight-1) = '\0';
    *(slant-1) = '\0';
    *(setwidthname-1) = '\0';
    *(addstylename-1) = '\0';
    *(pixsize-1) = '\0';
    *(ptsize-1) = '\0';
    *(xres-1) = '\0';

    if ((*foundry) && (*foundry != DASH))  {
	my_attrs->foundry = xv_strsave(foundry);
	my_attrs->free_foundry = 1;
    }

    if ((*foundry) && (*family != DASH))  {
	my_attrs->family = xv_strsave(family);
	my_attrs->free_family = 1;
    }

    if ((*weight) && (*weight != DASH))  {
	my_attrs->weight = xv_strsave(weight);
	my_attrs->free_weight = 1;
    }

    if ((*slant) && (*slant != DASH))  {
	my_attrs->slant = xv_strsave(slant);
	my_attrs->free_slant = 1;
    }

    if ( (*setwidthname) && (*setwidthname != DASH))  {
	my_attrs->setwidthname = xv_strsave(setwidthname);
	my_attrs->free_setwidthname = 1;
    }

    if ( (*addstylename) && (*addstylename != DASH))  {
	my_attrs->addstylename = xv_strsave(addstylename);
	my_attrs->free_addstylename = 1;
    }

    if ((*ptsize) && (*ptsize != DASH) && (*ptsize != '*'))  {
	tempSize = atoi(ptsize);
	my_attrs->size = tempSize / 10;

	medsize = (my_attrs->medium_size < 0) 
		? DEFAULT_MEDIUM_FONT_SIZE : my_attrs->medium_size;
	if (my_attrs->size < medsize)  {
	    my_attrs->scale = (int)WIN_SCALE_SMALL;
	}
	else  {
	    largesize = (my_attrs->large_size < 0) 
		? DEFAULT_LARGE_FONT_SIZE : my_attrs->large_size;
	    if (my_attrs->size < largesize)  {
	        my_attrs->scale = (int)WIN_SCALE_MEDIUM;
	    }
	    else  {
	        xlargesize = (my_attrs->extra_large_size < 0) 
		    ? DEFAULT_XLARGE_FONT_SIZE : my_attrs->extra_large_size;
		if (my_attrs->size < xlargesize)  {
	            my_attrs->scale = (int)WIN_SCALE_LARGE;
		}
		else  {
	            my_attrs->scale = (int)WIN_SCALE_EXTRALARGE;
		}
	    }
	}
    }

    if ((my_attrs->weight) && (my_attrs->slant))  {
	font_convert_weightslant(my_attrs);
    }

    return (0);
}

font_setup_pixfont(font_public)
Xv_font_struct	*font_public;
{
    register struct pixchar *pfc;
    Font_info		*xv_font_info = FONT_PRIVATE(font_public);
    XFontStruct		*x_font_info = (XFontStruct *)xv_font_info->x_font_info;
    Pixfont		*pixfont = (Pixfont *)xv_get(font_public, FONT_PIXFONT);
    int		i, default_x, default_y, 
		max_char, min_char;

    default_x = xv_font_info->def_char_width;
    default_y = xv_font_info->def_char_height;

    /*
     * Pixfont compat
     */
    max_char = MIN(255, x_font_info->max_char_or_byte2);
    min_char = MIN(255, x_font_info->min_char_or_byte2);

    pixfont->pf_defaultsize.x = default_x;
    pixfont->pf_defaultsize.y = default_y;
    for (i = min_char, pfc = &(pixfont->pf_char[i]);
	 i <= MIN(255, max_char);	/* "i" cannot ever be >255 - pixfont
					 * compat */
	 i++, pfc++) {
	    xv_x_char_info((XFontStruct *) x_font_info, i - min_char,
		       &pfc->pc_home.x, &pfc->pc_home.y,
		       &pfc->pc_adv.x, &pfc->pc_adv.y,
		       &pfc->pc_pr);
    }
}

font_check_var_height(variable_height_font, x_font_info)
int	*variable_height_font;
XFontStruct	*x_font_info;
{
    *variable_height_font = FALSE;
}

font_check_overlapping(neg_left_bearing, x_font_info)
int	*neg_left_bearing;
XFontStruct	*x_font_info;
{
    *neg_left_bearing = (x_font_info->min_bounds.lbearing < 0);
}

font_init_pixfont(font_public)
Xv_font_struct	*font_public;
{
    Font_info		*xv_font_info = FONT_PRIVATE(font_public);
    Pixfont_struct	*pf_rec;
    
    pf_rec = (Pixfont_struct *)malloc(sizeof(Pixfont_struct));

    xv_font_info->pixfont = (char *)(pf_rec);

    font_setup_pixfont(font_public);
    pf_rec->public_self = (Xv_font)font_public;

    return(XV_OK);

}

#endif /*OW_I18N*/
