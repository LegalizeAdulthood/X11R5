#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)defaults.c 50.8 90/11/04 SMI";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * This file contains routines for reading in and accessing an X11 defaults
 * database.  Any errors are printed on the console as warning messeges.
 */

#include <stdio.h>		/* Standard I/O library */
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <xview/defaults.h>
#include <xview/xv_error.h>
#include <xview/pkg.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

/* ---------------------------------------------------------- */

static char     defaults_returned_value[DEFAULTS_MAX_VALUE_SIZE];

#ifdef OW_I18N
/* 
 * The current implementation of the defaults package has no 
 * notion of per server defaults. This has to be changed. 
 * Currently the XID of the database associated with each server 
 * is stored in the server object. However, in order to prevent the 
 * defaults package from breaking, we have to set the defaults_rdb
 * global variable to be the XID of the database associated with the 
 * last server that was created. This is extremely ugly and must 
 * be fixed.
 */
XrmDatabase	defaults_rdb;/* merged defaults database */
#else
static XrmDatabase defaults_rdb;/* merged defaults database */
#endif OW_I18N

static Bool     symbol_equal();

extern Display *xv_default_display;

/* ---------------------------------------------------------- */



/*****************************************************************/
/* Exported routines:                                            */
/*****************************************************************/

/*
 * NOTE: Any returned string pointers should be considered temporary at best.
 * If you want to hang onto the data, make your own private copy of the
 * string!
 */

/*
 * defaults_exists(path_name, status) will return TRUE if Path_Name exists in
 * the database.
 */
Bool
defaults_exists(name, class)
    char           *name;
    char           *class;
{
    char            type[20];
    XrmValue        value;

    return XrmGetResource(defaults_rdb, name, class, type, &value);
}

/*
 * defaults_get_boolean(name, class, default) will lookup name and class in
 * the defaults database and return TRUE if the value is "True", "Yes", "On",
 * "Enabled", "Set", "Activated", or "1".  FALSE will be returned if the
 * value is "False", "No", "Off", "Disabled", "Reset", "Cleared",
 * "Deactivated", or "0".  If the value is none of the above, a warning
 * message will be displayed and Default will be returned.
 */
Bool
defaults_get_boolean(name, class, default_bool)
    char           *name;
    char           *class;
    Bool            default_bool;	/* Default value */
{
    static Defaults_pairs bools[] = {
	"True", (int) True,
	"False", (int) False,
	"Yes", (int) True,
	"No", (int) False,
	"On", (int) True,
	"Off", (int) False,
	"Enabled", (int) True,
	"Disabled", (int) False,
	"Set", (int) True,
	"Reset", (int) False,
	"Cleared", (int) False,
	"Activated", (int) True,
	"Deactivated", (int) False,
	"1", (int) True,
	"0", (int) False,
	NULL, -1,
    };
    char           *string_value;	/* String value */
    register Bool   value;	/* Value to return */

    string_value = defaults_get_string(name, class, (char *) NULL);
    if (string_value == NULL) {
	return default_bool;
    }
    value = (Bool) defaults_lookup(string_value, bools);
    if ((int) value == -1) {
	char            buffer[64];

#ifdef OW_I18N
	(void) sprintf(buffer, XV_I18N_MSG("xv_messages",
		
		"\"%s\" is an unrecognized boolean value (Defaults package)"), string_value);
#else
	(void) sprintf(buffer,
		"\"%s\" is an unrecognized boolean value (Defaults package)",
		string_value);
#endif OW_I18N

	xv_error(NULL,
		 ERROR_STRING, buffer,
		 0);
	value = default_bool;
    }
    return value;
}

/*
 * defaults_get_character(name, class, default) will lookup name and class in
 * the defaults database and return the resulting character value.  Default
 * will be returned if any error occurs.
 */
char
defaults_get_character(name, class, default_character)
    char           *name;
    char           *class;
    char            default_character;	/* Default return value */
{
    register char  *string_value;	/* String value */

    string_value = defaults_get_string(name, class, (char *) NULL);
    if (string_value == NULL) {
	return default_character;
    }
    if (strlen(string_value) != 1) {
	char            buffer[64];

#ifdef OW_I18N
	sprintf(buffer, XV_I18N_MSG("xv_messages",
		
		"\"%s\" is not a character constant (Defaults package)"), string_value);
#else
	sprintf(buffer, "\"%s\" is not a character constant (Defaults package)",
		string_value);
#endif OW_I18N

	xv_error(NULL,
		 ERROR_STRING, buffer,
		 0);
	return default_character;
    }
    return string_value[0];
}

/*
 * defaults_get_enum(name, class, pairs) will lookup the value associated
 * with name and class, scan the Pairs table and return the associated value.
 * If no match is found, an error is generated and the value associated with
 * last entry (i.e. the NULL entry) is returned.
 */
int
defaults_get_enum(name, class, pairs)
    char           *name;
    char           *class;
    Defaults_pairs *pairs;	/* Pairs table */
{
    return defaults_lookup(defaults_get_string(name, class, (char *) NULL),
			   pairs);
}

/*
 * defaults_get_integer(name, class, default) will lookup name and class in
 * the defaults database and return the resulting integer value. Default will
 * be returned if any error occurs.
 */
int
defaults_get_integer(name, class, default_integer)
    char           *name;
    char           *class;
    int             default_integer;	/* Default return value */
{
    register char   chr;	/* Temporary character */
    Bool            error;	/* TRUE => an error has occurred */
    Bool            negative;	/* TRUE => Negative number */
    register int    number;	/* Resultant value */
    register char  *cp;		/* character pointer */
    char           *string_value;	/* String value */

    string_value = defaults_get_string(name, class, (char *) NULL);
    if (string_value == NULL) {
	return default_integer;
    }
    /* Convert string into integer (with error chacking) */
    error = False;
    negative = False;
    number = 0;
    cp = string_value;
    chr = *cp++;
    if (chr == '-') {
	negative = True;
	chr = *cp++;
    }
    if (chr == '\0')
	error = True;
    while (chr != '\0') {
	if ((chr < '0') || (chr > '9')) {
	    error = True;
	    break;
	}
	number = number * 10 + chr - '0';
	chr = *cp++;
    }
    if (error) {
	char            buffer[64];

#ifdef OW_I18N
	sprintf(buffer, XV_I18N_MSG("xv_messages", 
		"\"%s\" is not an integer (Defaults package)"),
		 string_value);
#else
	sprintf(buffer,
		"\"%s\" is not an integer (Defaults package)", string_value);
#endif OW_I18N

	xv_error(NULL,
		 ERROR_STRING, buffer,
		 0);
	return default_integer;
    }
    if (negative)
	number = -number;
    return number;
}

/*
 * defaults_get_integer_check(name, class, default, mininum, maximum) will
 * lookup name and class in the defaults database and return the resulting
 * integer value. If the value in the database is not between Minimum and
 * Maximum (inclusive), an error message will be printed.  Default will be
 * returned if any error occurs.
 */
int
defaults_get_integer_check(name, class, default_int, minimum, maximum)
    char           *name;
    char           *class;
    int             default_int;/* Default return value */
    int             minimum;	/* Minimum value */
    int             maximum;	/* Maximum value */
{
    int             value;	/* Return value */

    value = defaults_get_integer(name, class, default_int);
    if ((minimum <= value) && (value <= maximum))
	return value;
    else {
	char            buffer[128];

#ifdef OW_I18N
	sprintf(buffer, XV_I18N_MSG("xv_messages", 
		"The value of name \"%s\" (class \"%s\") is %d,\nwhich is not between %d and %d. (Defaults package)"),
		name, class, value, minimum, maximum);
#else
	sprintf(buffer, "The value of name \"%s\" (class \"%s\") is %d,\nwhich is not between %d and %d. (Defaults package)",
		name, class, value, minimum, maximum);
#endif OW_I18N

	xv_error(NULL,
		 ERROR_STRING, buffer,
		 0);
	return default_int;
    }
}

/*
 * defaults_get_string(instance, class, default) will lookup and return the
 * null-terminated string value assocatied with instance and class in the
 * defaults database.  Default will be returned if any error occurs.
 */
char           *
defaults_get_string(instance, class, default_string)
    char           *instance;	/* Usually start with a lowercase letter */
    char           *class;	/* Usually start with an uppercase letter */
    char           *default_string;	/* Default return value */
{
    char            type[20];
    int             length;
    XrmValue        value;
    char	   *begin_ptr;
    char	   *end_ptr;
    char	   *word_ptr;

    if (!XrmGetResource(defaults_rdb, instance, class, type, &value))
	return default_string;

/*
 *  Strip all the preceding and trailing blanks of *value.addr 
 */

    word_ptr = defaults_returned_value;
    begin_ptr = value.addr;
    while (isspace (*begin_ptr)) ++begin_ptr;
    length = MIN(value.size - 1, DEFAULTS_MAX_VALUE_SIZE - 1);
    end_ptr = value.addr + length - 1;
    while (isspace (*end_ptr)) --end_ptr;
    for (; begin_ptr <= end_ptr; begin_ptr++)
    {
	*word_ptr=*begin_ptr;
	++word_ptr;
    }
    *word_ptr = '\0';

    return defaults_returned_value;
}

/*
 * set the resource to the specified character value
 */
void
defaults_set_character(resource, value)
    char           *resource;
    char            value;
{
    char            str[2];

    str[0] = value;
    str[1] = '\0';
    defaults_set_string(resource, str);
}

/*
 * set the resource to the specified bool/int value
 */
void
defaults_set_boolean(resource, value)
    char           *resource;
    Bool            value;
{
    static char    *yes_str = "True";
    static char    *no_str = "False";

    (void) defaults_set_string(resource,
			       ((int) value) ? yes_str : no_str);
}

/*
 * set the resource to the specified integer value
 */
void
defaults_set_integer(resource, value)
    char           *resource;
    int             value;
{
    char            str[12];	/* a string large enough to hold 2^32 in
				 * decimal */

    (void) sprintf(str, "%d", value);
    defaults_set_string(resource, str);
}

/*
 * set the resource to the specified string value
 */
void
defaults_set_string(resource, value)
    char           *resource;
    char           *value;
{
    XrmPutStringResource(&defaults_rdb, resource, value);
}

/*
 * defaults_init_db initializes the X11 Resource Manager.
 */
void
defaults_init_db()
{
    XrmInitialize();
}

/*
 * defaults_load_db(filename) will load the server database if filename is
 * NULL, or the database residing in the specified file.
 */
void
defaults_load_db(filename)
    char           *filename;
{
    XrmDatabase     new_db = NULL;

    if (filename)
	new_db = XrmGetFileDatabase(filename);
    else {
	if (!xv_default_display) {

#ifdef OW_I18N
	    xv_error(0,
		     ERROR_STRING, XV_I18N_MSG("xv_messages", 
		       "Unable to load server Resource Manager property -\n\
no server defined (Defaults package)"),
		     0);
#else
	    xv_error(0,
		     ERROR_STRING,
		       "Unable to load server Resource Manager property -\n\
no server defined (Defaults package)",
		     0);
#endif OW_I18N

	    return;
	}
	if (xv_default_display->xdefaults)
	    new_db = XrmGetStringDatabase(xv_default_display->xdefaults);
    }
    if (new_db)
	XrmMergeDatabases(new_db, &defaults_rdb);
}


/*
 * defaults_store_db(filename) will write the defaults database to the
 * specified file, and update the server Resource Manager property.
 */
void
defaults_store_db(filename)
    char           *filename;
{
    char           *buffer;
    FILE           *file;
    struct stat     file_status;

    /* Write the database to the specified file. */
    XrmPutFileDatabase(defaults_rdb, filename);

    /* Update the server Resource mManager property. */
    if (!xv_default_display) {
	xv_error(0,

#ifdef OW_I18N
		 ERROR_STRING, XV_I18N_MSG("xv_messages", 
		   "Unable to update server Resource Manager property -\n\
no server defined (Defaults package)"),
#else
		 ERROR_STRING,
		   "Unable to update server Resource Manager property -\n\
no server defined (Defaults package)",
#endif OW_I18N

		 0);
	return;
    }
    if (stat(filename, &file_status))
	goto store_db_error;
    if (!(buffer = calloc(1, (unsigned)file_status.st_size)))
	goto store_db_error;
    if (!(file = fopen(filename, "r")))
	goto store_db_error;
    if (fread(buffer, 1, (int) file_status.st_size, file) < file_status.st_size)
	goto store_db_error;
    XChangeProperty(xv_default_display, RootWindow(xv_default_display, 0),
		    XA_RESOURCE_MANAGER, XA_STRING, 8, PropModeReplace,
		    buffer, file_status.st_size);
    XSync(xv_default_display, 0);
store_db_cleanup:
    if (file)
	(void) fclose(file);
    if (buffer)
	free(buffer);
    return;

store_db_error:
    xv_error(0,

#ifdef OW_I18N
	     ERROR_STRING, XV_I18N_MSG("xv_messages",
	     
     "Unable to update server Resource Manager property (Defaults package)"),
#else
	     ERROR_STRING,
     "Unable to update server Resource Manager property (Defaults package)",
#endif OW_I18N

	     0);
    goto store_db_cleanup;
}


/*
 * defaults_lookup(name, pairs) will linearly scan the Pairs data structure
 * looking for Name.  The value associated with Name will be returned. If
 * Name can not be found in Pairs, the value assoicated with NULL will be
 * returned.  (The Pairs data structure must be terminated with NULL.)
 */
int
defaults_lookup(name, pairs)
    register char  *name;	/* Name to look up */
    register Defaults_pairs *pairs;	/* Default */
{
    register Defaults_pairs *pair;	/* Current pair */

    for (pair = pairs; pair->name != NULL; pair++) {
	if (name == NULL)
	    continue;
	if (symbol_equal(name, pair->name))
	    break;
    }
    return pair->value;
}


/*****************************************************************/
/* Private routines:                                             */
/*****************************************************************/

static          Bool
symbol_equal(symbol1, symbol2)
    register char  *symbol1;	/* First symbol */
    register char  *symbol2;	/* Second symbol */
{
    register char   chr1;	/* Character from first symbol */
    register char   chr2;	/* Character from second symbol */

    while (True) {
	chr1 = *symbol1++;
	if (('A' <= chr1) && (chr1 <= 'Z'))
	    chr1 += 'a' - 'A';
	chr2 = *symbol2++;
	if (('A' <= chr2) && (chr2 <= 'Z'))
	    chr2 += 'a' - 'A';
	if (chr1 != chr2)
	    return FALSE;
	if (chr1 == '\0')
	    return TRUE;
    }
}
