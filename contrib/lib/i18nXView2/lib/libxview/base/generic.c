#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)generic.c 50.4 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * generic.c: Support for "generic object" - basically, a property list.
 */

#include <xview/pkg.h>
#include <xview/notify.h>
#include <xview/generic.h>

#ifdef OW_I18N
#include  <X11/X.h>
#include  <X11/Xlib.h>
#include  <X11/Xresource.h>

Xv_private char    *xv_instance_app_name;
#endif OW_I18N

/* ------------------------------------------------------------------------- */

/*
 * Public
 */
extern Xv_pkg   xv_generic_pkg;


/*
 * Private
 */
/*
 * Escape hatch to cope with SunView and/or client bugs in ref. counting.
 */
Xv_private_data int xv_free_unreferenced;
static Attr_attribute xv_next_key;	/* = 0 for -A-R */

typedef struct _generic_node {
    struct _generic_node *next;
    Attr_attribute  key;
    Xv_opaque       data;
    void            (*copy_proc) ();
    void            (*remove_proc) ();
}               Generic_node;

#define HEAD(obj) XV_PRIVATE(Generic_node, Xv_generic_struct, obj)

static Generic_node *add_node();
static Generic_node *find_node();
static void	     delete_node();


/* ------------------------------------------------------------------------ */


/*
 * PORTABILITY ALERT! On a Sun, an attribute is an unsigned 32-bit quantity
 * with the high-order 8 bits used to hold the package identification.  Thus,
 * a valid attribute never has a 0 in its high-order 8 bits.  The following
 * routine relies on this fact to quickly generate up to 2^24 key values that
 * cannot be mistaken for valid attributes.  Note that 0 is not a valid
 * attribute or key.
 */
Xv_public       Attr_attribute
xv_unique_key()
{
    return (++xv_next_key);
}

/*ARGSUSED*/
static void
generic_data_free(object, data, key)
    Xv_object       object;
    Xv_opaque       data;
    Attr_attribute  key;
{
    if (data)
	free((char *) data);
}


/*ARGSUSED*/
Xv_private int
generic_init(parent, object, avlist)
    Xv_object	    parent;
    Xv_object	    object;
    Attr_avlist	    avlist;
{
#ifdef OW_I18N
    /* performance optimization */
    short	flag = TRUE;
    /* 
     * PERFORMANCE ALERT:
     * Currently, the generic object has no private data 
     * structure. The private data is simply a list of
     * XV_KEY_DATA nodes. This could be improved to make the
     * generic object store the owner, instance name and
     * instance list directly, instead of as XV_KEY_DATA nodes.
     * This also has an impact at destruction time, when the 
     * XV_KEY_DATA nodes have to be scanned to find the allocated 
     * memory that has to be freed.
     */
    for (; flag && *avlist; avlist = attr_next(avlist)) 
	switch ((Xv_generic_attr) (*avlist)) {
	    case XV_INSTANCE_NAME:
		generic_set_instance_name(parent, object, (char *)avlist[1]);
		 /* performance optimization */
		flag = FALSE;
		break;

	    default:
		break;
	}
#endif OW_I18N

    xv_set(object, XV_KEY_DATA, XV_OWNER, parent, 0);
    return XV_OK;
}


#ifdef OW_I18N
Xv_private 
generic_set_instance_name(parent, object, instance_name)
    Xv_object       parent;
    Xv_object       object;
    char	    *instance_name;
{
    Xv_opaque       	parent_quarks = NULL, quarks;
    short		null_parent = FALSE;
    int		  	num_quarks = 0;
    register int	i;

    if (instance_name == NULL)
	return;

    if (parent != NULL) 
        parent_quarks = xv_get(parent, XV_INSTANCE_QLIST); 
    else {
	null_parent = TRUE;
	parent_quarks = db_qlist_from_name(xv_instance_app_name, NULL);
    }

    quarks = db_qlist_from_name(instance_name, parent_quarks);
    if (quarks != NULL)    
        xv_set(object, XV_KEY_DATA, XV_INSTANCE_QLIST, quarks, NULL);
    if ((null_parent == TRUE) && (parent_quarks != NULL))
	free(parent_quarks);
}
#endif OW_I18N
    

Xv_opaque
generic_set_avlist(object, avlist)
    Xv_object       object;
    Attr_avlist     avlist;
{
    Attr_attribute  key;
    register Generic_node *node;
    Generic_node   *prev, *existing_node;
    int             ref_count;
    register Xv_opaque error_code = NULL;
    Xv_object       parent;

    for (; *avlist; avlist = attr_next(avlist)) {
	switch ((Xv_generic_attr) (*avlist)) {
	  case XV_KEY_DATA:
	    key = (int) avlist[1];
	    existing_node = node = find_node(object, key, &prev);
	    if (!node) {
		node = add_node(object, key);
	    }
	    if (node) {
		if (existing_node && existing_node->data != avlist[2]) {
		    if (existing_node->remove_proc)
			(existing_node->remove_proc) (object,
						      existing_node->key,
						      existing_node->data);
		}
		node->data = avlist[2];
		node->remove_proc = NULL;
	    } else {
		error_code = *avlist;
	    }
	    break;
	  case XV_KEY_DATA_COPY_PROC:
	  case XV_KEY_DATA_REMOVE:
	  case XV_KEY_DATA_REMOVE_PROC:
	    key = (int) avlist[1];
	    node = find_node(object, key, &prev);
	    if (node) {
		switch ((Xv_generic_attr) (*avlist)) {
		  case XV_KEY_DATA_COPY_PROC:
		    node->copy_proc = (void (*) ()) avlist[2];
		    break;
		  case XV_KEY_DATA_REMOVE:
		    delete_node(object, node, prev);
		    break;
		  case XV_KEY_DATA_REMOVE_PROC:
		    node->remove_proc = (void (*) ()) avlist[2];
		    break;
		}
	    } else {
		error_code = *avlist;
	    }
	    break;

	  case XV_LABEL:
	  case XV_NAME:
	    /* PERFORMANCE ALERT: is recursion too slow? */
	    (void) xv_set(object,
			  XV_KEY_DATA, avlist[0], avlist[1],
		      XV_KEY_DATA_REMOVE_PROC, avlist[0], generic_data_free,
			  0);
	    break;

	  case XV_OWNER:
	  case XV_STATUS:
	    /* PERFORMANCE ALERT: is recursion too slow? */
	    if (avlist[1]) {
		(void) xv_set(object, XV_KEY_DATA, avlist[0], avlist[1], 0);
	    } else {
		(void) xv_set(object, XV_KEY_DATA_REMOVE, avlist[0], 0);
	    }
	    break;

	  case XV_REF_COUNT:
	    /* PERFORMANCE ALERT: is ref count as property too slow? */
	    /*
	     * Object is only destroyed if its reference count has been
	     * decremented to 0; an explicit reset to 0 will not trigger the
	     * destroy.  Since xv_destroy_safe does not really attempt to
	     * destroy the object until the process unwinds back to the
	     * Notifier, the reference count may become non-zero in the
	     * meantime, thus aborting the destroy.
	     */
	    key = (Attr_attribute) XV_REF_COUNT;
	    existing_node = node = find_node(object, key, &prev);
	    if (node) {
		ref_count = (int) node->data;
	    } else {
		node = add_node(object, key);
#ifdef _XV_DEBUG
		if ((int) avlist[1] != XV_RC_SPECIAL)
		    abort();
#endif
		node->remove_proc = NULL;
		ref_count = 0;
	    }
	    if (node) {
		switch ((int) avlist[1]) {
		  case XV_RC_SPECIAL:
		    ref_count = 0;
		    break;
		  case XV_RC_SPECIAL + 1:
		    ref_count++;
		    break;
		  case XV_RC_SPECIAL - 1:
		    ref_count--;
		    if (xv_free_unreferenced && existing_node &&
			ref_count == 0) {
			(void) xv_destroy_safe(object);
		    }
		    break;
		  default:
		    ref_count = (int) avlist[1];
		    break;
		}
#ifdef _XV_DEBUG
		if (ref_count < 0)
		    abort();
#endif
		node->data = (Xv_opaque) ref_count;
	    } else {
		error_code = *avlist;
	    }
	    break;

	  case XV_COPY_OF:
	  case XV_END_CREATE:
	    /* Explicitly ignore these: they are for sub-classes */
	    break;
	  case ATTR_NOP0:
	  case ATTR_NOP1:
	  case ATTR_NOP2:
	  case ATTR_NOP3:
	  case ATTR_NOP4:
	    /* Explicitly ignore these: they are meant to be ignored */
	    break;

	  case XV_SHOW:	/* XV_SHOW should be handled by individual
				 * packages. */
	    break;
	  default:
	    (void) xv_check_bad_attr(XV_GENERIC_OBJECT,
				     (Attr_attribute) * avlist);
	    break;
	}
    }

    return error_code;
}

Xv_opaque
generic_get(object, status, attr, args)
    Xv_object       object;
    int            *status;	/* Initialized by caller. */
    Attr_attribute  attr;
    va_list         args;
{
    Attr_attribute  key;
    register Xv_opaque result;
    register Generic_node *node;
    Generic_node   *prev;

    /* Don't set *status to XV_ERROR unless attribute is unrecognized! */
    switch (attr) {
      case XV_KEY_DATA:
      case XV_KEY_DATA_COPY_PROC:
      case XV_KEY_DATA_REMOVE_PROC:
	key = va_arg(args, Attr_attribute);
	node = find_node(object, key, &prev);
	if (node) {
	    switch (attr) {
	      case XV_KEY_DATA:
		result = node->data;
		break;
	      case XV_KEY_DATA_COPY_PROC:
		result = (Xv_opaque) node->copy_proc;
		break;
	      case XV_KEY_DATA_REMOVE_PROC:
		result = (Xv_opaque) node->remove_proc;
		break;
	    }
	} else {
	    result = 0;
	}
	break;

#ifdef OW_I18N
      case XV_INSTANCE_NAME:
	  result = xv_get(object, XV_KEY_DATA, XV_INSTANCE_NAME);
	  if (result == NULL) {
	      result = xv_get(object, XV_KEY_DATA, XV_INSTANCE_QLIST);
	      if (result != NULL) {
	          result = db_name_from_qlist(result);
		  xv_set(object, XV_KEY_DATA, XV_INSTANCE_NAME, result);
	      }
	  }
	  break;
#endif OW_I18N

#ifdef OW_I18N
      case XV_INSTANCE_QLIST:
#endif OW_I18N
      case XV_LABEL:
      case XV_NAME:
      case XV_OWNER:
      case XV_STATUS:
	/* PERFORMANCE ALERT!  Is recursion too slow? */
	result = xv_get(object, (Attr_attribute) XV_KEY_DATA, attr);
	break;
      case XV_REF_COUNT:
	node = find_node(object, (Attr_attribute) XV_REF_COUNT, &prev);
	if (node) {
	    result = node->data;
	} else {
	    result = 0;
	}
	break;
      case XV_TYPE:
	result = (Xv_opaque) ((Xv_base *) object)->pkg;
	break;
      case XV_IS_SUBTYPE_OF:{
	    register Xv_pkg *pkg = ((Xv_base *) object)->pkg;
	    register Xv_pkg *super_pkg;

	    super_pkg = va_arg(args, Xv_pkg *);
	    while (pkg) {
		if (pkg == super_pkg) {
		    return (Xv_opaque) TRUE;
		}
		pkg = pkg->parent_pkg;
	    }
	    result = FALSE;
	    break;
	}
      case XV_SELF:
	result = object;
	break;
#ifdef OW_I18N
      case XV_IM:
        result = NULL;
        break;
#endif
      default:
	if (xv_check_bad_attr(XV_GENERIC_OBJECT, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	result = 0;
	break;
    }

    return result;
}

int
generic_destroy(object, status)
    Xv_object       object;
    Destroy_status  status;
{
    register Generic_node *node;

    switch (status) {
      case DESTROY_CHECKING:
	if (xv_get(object, (Attr_attribute) XV_REF_COUNT))
	    /* Assume decrement followed by increment => don't destroy. */
	    return (XV_ERROR);
	break;
      case DESTROY_CLEANUP:
	while (node = HEAD(object)) {
	    delete_node(object, node, (Generic_node *) 0);
	}
	break;
      case DESTROY_PROCESS_DEATH:
      default:
	break;
    }
    return XV_OK;
}

static Generic_node *
add_node(object, key)
    Xv_object       object;
    Attr_attribute  key;
{
    register Generic_node *node;

    node = xv_alloc(Generic_node);
    node->next = HEAD(object);
    /*
     * The following line of code was changed to remove casting of lvalue
     * HEAD(object) = node;
     */
    ((Xv_generic_struct *) (object))->private_data = (Xv_opaque) node;
    node->key = key;

    return node;
}

static Generic_node *
find_node(object, key, prev)
    register Xv_object object;
    register Attr_attribute key;
    register Generic_node **prev;
{
    register Generic_node *node;

    if (HEAD(object)) {
	if (HEAD(object)->key == key) {
	    node = HEAD(object);
	    *prev = (Generic_node *) NULL;
	} else {
	    for (*prev = HEAD(object), node = (*prev)->next; node;
		 *prev = node, node = (*prev)->next) {
		if (node->key == key)
		    break;
	    }
	}
    } else {
	*prev = node = (Generic_node *) NULL;
    }
    return node;
}

static void
delete_node(object, node, prev)
    Xv_object       object;
    register Generic_node *node, *prev;
{
    if (prev) {
	prev->next = node->next;
    } else {
	/*
	 * The following line was changed to avoid casting the lvalue
	 * HEAD(object) = node->next;
	 */
	((Xv_generic_struct *) (object))->private_data = (Xv_opaque) node->next;
    }
    if (node->remove_proc)
	(node->remove_proc) (object, node->key, node->data);
    xv_free(node);
}
