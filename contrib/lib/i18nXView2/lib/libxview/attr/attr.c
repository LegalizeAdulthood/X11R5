#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)attr.c 50.5 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/pkg_public.h>
#include <xview/xv_error.h>
#include <xview_private/attr_impl.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif
/*
 * Note that changes to the basic attribute-value traversal should also be
 * made in attr_portable.c for portable implmentations.
 */


Xv_private FILE *help_find_file();

#define ANBUFSIZE 64
static char     attr_name_buf[ANBUFSIZE];	/* to hold name of attribute */

/*
 * attr_name - return a pointer to a string containing the name of
 * the specified attribute, or its hex value if not found.
 */
Xv_private char *
attr_name(attr)
    Attr_attribute attr;
{
    int		    attr_value;
    FILE	   *file_ptr;
    int		    found = FALSE;

    attr_name_buf[0] = 0;
    file_ptr = help_find_file("attr_names");
    if (file_ptr) {
	while (fscanf(file_ptr, "%x %s\n", &attr_value, attr_name_buf) != EOF) {
	    if (found = (attr_value == attr))
		break;
	}
	fclose(file_ptr);
    }
    if (!found)
	sprintf(attr_name_buf, "attr # 0x%08x", attr);
    return attr_name_buf;
}


/*
 * attr_create creates an avlist from the VARARGS passed on the stack.
 */
/*VARARGS2*/
Attr_avlist
attr_create(listhead, listlen, va_alist)
    Xv_opaque      *listhead;
    int             listlen;
va_dcl
{
    Attr_avlist     avlist;
    va_list         valist;

    va_start(valist);
    avlist = attr_make(listhead, listlen, valist);
    va_end(valist);
    return avlist;
}


/*
 * attr_make copies the attribute-value list pointed to by valist to the
 * storage pointed to by listhead, or some new storage if that is null.  If
 * listhead is not null, then the list must be less than or equal to listlen
 * ATTR_SIZE byte chunks; if not 0 is returned. The count of the avlist is
 * returned in *count_ptr, if count_ptr is not NULL.
 */
Attr_avlist
attr_make_count(listhead, listlen, valist, count_ptr)
    Attr_avlist     listhead;
    int             listlen;
    va_list         valist;
    int            *count_ptr;
{
    unsigned        count;
    extern char    *malloc();

#ifdef NON_PORTABLE
    count = (unsigned) attr_count((Attr_avlist) valist);
#else
    count = (unsigned) valist_count(valist);
#endif
    /*
     * if the user supplied storage space for attributes is not big enough
     * for the attributes in the attribute list, then exit!
     */
    if (listhead) {
	if (count > listlen) {
	    char            dummy[128];

	    (void) sprintf(dummy,
#ifdef	OW_I18N
		XV_I18N_MSG("xv_messages","Number of attributes(%d) exceeds max(%d)"), count, listlen);
#else
		"Number of attributes(%d) exceeds max(%d)", count, listlen);
#endif
	    xv_error(NULL,
		     ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		     ERROR_STRING, dummy,
		     0);
	}
    } else {
	listhead = (Attr_avlist) malloc(ATTR_SIZE * count);

    }
#ifdef NON_PORTABLE
    (void) attr_copy_avlist(listhead, (Attr_avlist) valist);
#else
    (void) attr_copy_valist(listhead, valist);
#endif

    if (count_ptr)
	*count_ptr = count;
    return (listhead);
}


#define	avlist_get(avlist)	*(avlist)++

/*
 * Copy count elements from list to dest. Advance both list and dest to the
 * next element after the last one copied.
 */
#define	avlist_copy(avlist, dest, count)	\
    { \
        bcopy((char *) avlist, (char *) dest, (int)(count * ATTR_SIZE)); \
        avlist += count; \
        dest += count; \
    }


/*
 * A macro to copy attribute values count is the number of Xv_opaque size
 * chunks to copy.
 */
#define	avlist_copy_values(avlist, dest, count) \
    if (count == 1) \
        *dest++ = avlist_get(avlist); \
    else { \
	avlist_copy(avlist, dest, count); \
    }


/*
 * attr_copy_avlist copies the attribute-value list from avlist to dest.
 * Recursive lists are collapsed into dest.
 */

Attr_avlist
attr_copy_avlist(dest, avlist)
    register Attr_avlist dest;
    register Attr_avlist avlist;
{
    register Attr_attribute attr;
    register unsigned cardinality;

    while (attr = (Attr_attribute) avlist_get(avlist)) {
	cardinality = ATTR_CARDINALITY(attr);
	switch (ATTR_LIST_TYPE(attr)) {
	  case ATTR_NONE:	/* not a list */
	    *dest++ = (Xv_opaque) attr;
	    avlist_copy_values(avlist, dest, cardinality);
	    break;

	  case ATTR_NULL:	/* null terminated list */
	    *dest++ = (Xv_opaque) attr;
	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:
		/*
		 * Note that this only checks the first four bytes for the
		 * null termination. Copy each value element until we have
		 * copied the null termination.
		 */
		do {
		    avlist_copy_values(avlist, dest, cardinality);
		} while (*(dest - 1));
		break;

	      case ATTR_LIST_IS_PTR:
		*dest++ = avlist_get(avlist);
		break;
	    }
	    break;

	  case ATTR_COUNTED:	/* counted list */
	    *dest++ = (Xv_opaque) attr;
	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:{
		    register unsigned count;

		    *dest = avlist_get(avlist);	/* copy the count */
		    count = ((unsigned) *dest++) * cardinality;
		    avlist_copy_values(avlist, dest, count);
		}
		break;

	      case ATTR_LIST_IS_PTR:
		*dest++ = avlist_get(avlist);
		break;
	    }
	    break;

	  case ATTR_RECURSIVE:	/* recursive attribute-value list */
	    if (cardinality != 0)	/* don't strip it */
		*dest++ = (Xv_opaque) attr;

	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:
		dest = attr_copy_avlist(dest, avlist);
		if (cardinality != 0)	/* don't strip it */
		    dest++;	/* move past the null terminator */
		avlist = attr_skip(attr, avlist);
		break;

	      case ATTR_LIST_IS_PTR:
		if (cardinality != 0)	/* don't collapse inline */
		    *dest++ = avlist_get(avlist);
		else {
		    Attr_avlist     new_avlist = (Attr_avlist)
		    avlist_get(avlist);
		    if (new_avlist)
			/*
			 * Copy the list inline -- don't move past the null
			 * termintor. Here both the attribute and null
			 * terminator will be stripped away.
			 */
			dest = attr_copy_avlist(dest, new_avlist);
		}
		break;
	    }
	    break;
	}
    }
    *dest = 0;
    return (dest);
}


/*
 * attr_count counts the number of slots in the av-list avlist. Recursive
 * lists are counted as being collapsed inline.
 */
int
attr_count(avlist)
    Attr_avlist     avlist;
{
    /* count the null termination */
    return (attr_count_avlist(avlist, (Attr_attribute) NULL) + 1);
}


int
attr_count_avlist(avlist, last_attr)
    register Attr_avlist avlist;
    register Attr_attribute last_attr;  /* BUG ALERT: Not used any more */
{
    register Attr_attribute attr;
    register unsigned count = 0;
    register unsigned num;
    register unsigned cardinality;

    while (attr = (Attr_attribute) * avlist++) {
	count++;		/* count the attribute */
	cardinality = ATTR_CARDINALITY(attr);
	last_attr = attr;
	switch (ATTR_LIST_TYPE(attr)) {
	  case ATTR_NONE:	/* not a list */
	    count += cardinality;
	    avlist += cardinality;
	    break;

	  case ATTR_NULL:	/* null terminated list */
	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:
		/*
		 * Note that this only checks the first four bytes for the
		 * null termination.
		 */
		while (*avlist++)
		    count++;
		count++;	/* count the null terminator */
		break;

	      case ATTR_LIST_IS_PTR:
		count++;
		avlist++;
		break;
	    }
	    break;

	  case ATTR_COUNTED:	/* counted list */
	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:
		num = ((unsigned) (*avlist)) * cardinality + 1;
		count += num;
		avlist += num;
		break;
	      case ATTR_LIST_IS_PTR:
		count++;
		avlist++;
		break;
	    }
	    break;

	  case ATTR_RECURSIVE:	/* recursive attribute-value list */
	    if (cardinality == 0)	/* don't include the attribute */
		count--;

	    switch (ATTR_LIST_PTR_TYPE(attr)) {
	      case ATTR_LIST_IS_INLINE:
		count += attr_count_avlist(avlist, attr);
		if (cardinality != 0)	/* count the null terminator */
		    count++;
		avlist = attr_skip(attr, avlist);
		break;

	      case ATTR_LIST_IS_PTR:
		if (cardinality != 0) {	/* don't collapse inline */
		    count++;
		    avlist++;
		} else if (*avlist)
		    /*
		     * Here we count the elements of the recursive list as
		     * being inline. Don't count the null terminator.
		     */
		    count += attr_count_avlist((Attr_avlist) * avlist++,
					       attr);
		else
		    avlist++;
		break;
	    }
	    break;
	}
    }
    return count;
}


/*
 * attr_skip_value returns a pointer to the attribute after the value pointed
 * to by avlist.  attr should be the attribute which describes the value at
 * avlist.
 */
Attr_avlist
attr_skip_value(attr, avlist)
    register Attr_attribute attr;
    register Attr_avlist avlist;
{
    switch (ATTR_LIST_TYPE(attr)) {
      case ATTR_NULL:
	if (ATTR_LIST_PTR_TYPE(attr) == ATTR_LIST_IS_PTR)
	    avlist++;
	else
	    while (*avlist++);
	break;

      case ATTR_RECURSIVE:
	if (ATTR_LIST_PTR_TYPE(attr) == ATTR_LIST_IS_PTR)
	    avlist++;
	else
	    while (attr = (Attr_attribute) * avlist++)
		avlist = attr_skip_value(attr, avlist);
	break;

      case ATTR_COUNTED:
	if (ATTR_LIST_PTR_TYPE(attr) == ATTR_LIST_IS_PTR)
	    avlist++;
	else
	    avlist += ((int) *avlist) * ATTR_CARDINALITY(attr) + 1;
	break;

      case ATTR_NONE:
	avlist += ATTR_CARDINALITY(attr);
	break;
    }
    return avlist;
}
