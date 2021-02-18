#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#) quark.c 50.5 90/11/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include  <X11/X.h>
#include  <X11/Xlib.h>
#include  <X11/Xresource.h>
#include  <xview/xview.h>
#include  <xview_private/db_impl.h>
#include  <xview/pkg.h>

Xv_opaque resource_type_conv();

/* 
 *    Utilities to deal with quark lists and such.
 */
Xv_private Xv_opaque 
db_name_from_qlist(qlist) 
    XrmQuarkList    qlist;
{   
    register int    i;  

    if (qlist == NULL)
	return(NULL);
    
    for (i = 0; qlist[i] != NULLQUARK; i++) 
                ;
    if (i != 0) 
        return((Xv_opaque)XrmQuarkToString(qlist[i - 1]));
    else 
        return((Xv_opaque)NULL);
}

Xv_private XrmQuarkList
db_qlist_from_name(name, parent_quarks)
    char    	    *name;
    XrmQuarkList    parent_quarks;
{
    register int    i;
    int		    num_quarks = 0;
    XrmQuarkList    quarks;

    if (name == NULL) 
	return(NULL);

    if (parent_quarks != NULL) {
        for ( ;parent_quarks[num_quarks] != NULLQUARK; num_quarks++)
			;
        quarks = (XrmQuarkList) xv_calloc(num_quarks + 2, sizeof(XrmQuark));
        for (i = 0; i < num_quarks; i++)
	    quarks[i] = parent_quarks[i];
    } else {
	quarks = (XrmQuarkList) xv_calloc(2, sizeof(XrmQuark));
	i = 0;
    }

    quarks[i++] = XrmStringToQuark(name);
    quarks[i] = NULLQUARK;

    return(quarks);
}

Xv_private Xv_opaque
db_get_data(db, instance_qlist, attr_name, type, default_value)
    XID			db;
    XrmQuarkList	instance_qlist;
    char		*attr_name;
    int			type;
    Xv_opaque		default_value;
{
    Xv_opaque           result;
    XrmRepresentation   quark_type;
    XrmValue            value;
    XrmQuark            qlist[MAX_NESTED_PKGS+2];
    register int	i;

    for (i = 0; instance_qlist[i] != NULLQUARK; i++)
	qlist[i] = instance_qlist[i];
    qlist[i++] = XrmStringToQuark(attr_name);
    qlist[i] = NULLQUARK;

    if (XrmQGetResource(db, qlist, qlist, &quark_type, &value) == True)
	result = 
	    (Xv_opaque)resource_type_conv(value.addr, type, default_value);
    else 
	result = default_value;
    
    return(result);
}
    

static Xv_opaque
resource_type_conv(str, xv_type, def_val)
        char            *str;
        int             xv_type;
        Xv_opaque       def_val;
{
    Xv_opaque           to_val;

    switch (xv_type) {
        case XV_INT:
          db_cvt_string_to_int(str, &to_val);
          return (to_val);
 
        case XV_BOOLEAN:
          db_cvt_string_to_bool(str, &to_val);
          return (to_val);
 
        case XV_CHAR:
          db_cvt_string_to_char(str, &to_val);
          return (to_val);
 
        case XV_STRING:
          to_val = (Xv_opaque) str;
          return (to_val);
 
        case XV_WSTRING:
           db_cvt_string_to_wcs(str, &to_val);
           return (to_val);
 
        default:
          return (def_val);
    }
}

