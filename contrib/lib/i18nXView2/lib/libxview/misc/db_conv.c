#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)db_conv.c 50.7 90/12/20";
#endif
#endif

#include <xview/xview.h>
#include <xview/xv_i18n.h>
#include <xview_private/xv_i18n_impl.h>
#include <xview_private/db_impl.h>

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

/*
Pkg_private void
db_cvt_to_string(from_value, from_type, to_value)
    Xv_opaque   from_value;
    int         from_type;
    char        *to_value;
{
    if (from_type == XV_STRING)
        return;

    switch(from_type) {
        case XV_INT:
            db_cvt_int_to_string(from_value, to_value);
            break;

        case XV_BOOLEAN:
            db_cvt_bool_to_string(from_value, to_value);
            break;

        case XV_CHAR:
            db_cvt_char_to_string(from_value, to_value);
            break;

	case XV_WSTRING:
	    db_cvt_wcs_to_string(from_value, to_value);
	    break;

        default:
            xv_error(NULL, ERROR_STRING,
		XV_I18N_MSG("xv_messages", "Unknown type for type conversion"),
                0);
    }          
}


Pkg_private int
db_cvt_from_string(from_value, to_type, to_value)
    char	*from_value;
    int		to_type;
    Xv_opaque   *to_value;
{
    int	    status;

    if (from_value == NULL) {
	return(XV_ERROR);
    } else if (to_type == XV_STRING) {
	*to_value = (Xv_opaque)from_value;
	return(XV_OK);
    }

    switch(to_type) {
	case XV_INT:
	    status = db_cvt_string_to_int(from_value, to_value);
	    break;

	case XV_BOOLEAN:
	    status = db_cvt_string_to_bool(from_value, to_value);
	    break;

	case XV_CHAR:
	    status = db_cvt_string_to_char(from_value, to_value);
	    break;

	case XV_WSTRING:
	    status = db_cvt_string_to_wcs(from_value, to_value);
	    break;
	
	default:
	    status = XV_ERROR;
	    break;
    }
    return(status);
}

*/

Pkg_private int
db_cvt_string_to_wcs(from_value, to_value)
    char	*from_value;
    Xv_opaque	*to_value;
{
    static wchar_t *cvtbufptr = NULL;
    static int	buflen = 0;
    int		newlen, status;
    char	*ptr;

    if (buflen < (newlen=strlen(from_value)+1)) {
        if (cvtbufptr != NULL) {
	    free (cvtbufptr);
	}
	cvtbufptr = (wchar_t *)xv_calloc(1, newlen*sizeof(wchar_t));
	buflen = newlen;
    }

    status = mbstowcs(from_value, cvtbufptr, newlen);
    *to_value = (Xv_opaque) cvtbufptr;
    return ((status == -1) || (status > newlen-1) ? XV_ERROR : XV_OK);
}

Pkg_private int
db_cvt_string_to_int(from_value, to_value)
    char        *from_value;
    Xv_opaque   *to_value;
{
    char  	*ptr; 

    *to_value = (Xv_opaque)strtol(from_value, &ptr, 10);
    return(((ptr == from_value) || (*ptr != '\0')) ? XV_ERROR : XV_OK);
}


Pkg_private int
db_cvt_string_to_bool(from_value, to_value)
    char        *from_value;
    Xv_opaque   *to_value;
{
#define DB_BOOL_VALUES	16
    static char *db_bool_table[DB_BOOL_VALUES] = {
	"true","false",
        "yes", "no",
        "on", "off",
        "enabled",  "disabled",
        "set", "reset",
	"set", "cleared",
        "activated", "deactivated",
        "1", "0",
    };
    register int	i;
    register char   	chr1, chr2;
    register char	*symbol1, *symbol2;

    for (i = 0; i < DB_BOOL_VALUES; i++) {
	symbol1 = *(db_bool_table + i);
	symbol2 = from_value;
	while ((chr1 = *symbol1++) != '\0') {
	    chr2 = *symbol2++;
	    if (('A' <= chr2) && (chr2 <= 'Z'))
		chr2 += 'a' - 'A';
	    if (chr1 != chr2)
		break;
  	}
	if (chr1 == NULL) {
	    *to_value = (i % 2) ? False : True;
	    return(XV_OK);
	}
    }
    return(XV_ERROR);
}


Pkg_private int
db_cvt_string_to_char(from_value, to_value)
    char        *from_value;
    Xv_opaque   *to_value;
{
    *to_value = from_value[0];
    return(XV_OK);
}

