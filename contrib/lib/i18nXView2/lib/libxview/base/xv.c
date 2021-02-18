#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv.c 50.5 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/* XView interface layer */

/* ------------------------------------------------------------------------- */

#include <stdio.h>
#include <sys/types.h>
#include <xview/attr.h>
#include <xview/pkg_public.h>
#include <xview/pkg.h>
#include <xview/notify.h>
#include <xview/generic.h>
#include <xview/server.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

/* ------------------------------------------------------------------------- */

/*
 * Public
 */

Xv_public   Xv_object	xv_create( /* parent, pkg, attrs */ );
Xv_public   Xv_object	xv_find( /* parent, pkg, attrs */ );
Xv_public   Xv_opaque	xv_set( /* object, attrs */ );
Xv_public   Xv_opaque	xv_get( /* object, attr, data* */ );
Xv_public   int	    	xv_destroy( /* object */ );
Xv_public   int	    	xv_destroy_immediate( /* object */ );
Xv_public   int	    	xv_destroy_safe( /* object */ );


/*
 * Private
 */

Xv_private  Xv_object	xv_create_avlist( /* parent, pkg, avlist */ );
Xv_private  Xv_object	xv_find_avlist( /* parent, pkg, avlist */ );
Xv_private  Xv_opaque	xv_set_avlist( /* object, avlist */ );
#ifdef _XV_GET_VARARGS
Xv_private  Xv_opaque	xv_get_varargs( /* object, attribute, varargs */ );
#endif _XV_GET_VARARGS
Xv_private  int	    	xv_destroy_status( /* object, status */ );
Xv_private  Destroy_status
    	    	    	xv_get_destruction_type();
Xv_private  void    	xv_set_destroy_interposer();
Xv_private  void    	xv_set_destruction_type();

typedef int     (*int_fnp) ();
typedef         Xv_opaque(*opaque_fnp) ();

/* used to automagically call initialze sv */
static	int 	    	xv_initialized;	/* = FALSE */
static	Destroy_status	destroy_status = DESTROY_CLEANUP;

#define  DELETE_WINDOW	0
#define  SAVE_YOURSELF	1


/*
 * An overview of "objects". Every valid object is of one of two types: 1) A
 * standard object: the pointer passed to and from the Sv client points to a
 * Xv_base.  The first field of that struct contains the value XV_OBJECT_SEAL
 * - a "magic" value that (hopefully) only occurs as the first 4 bytes of a
 * valid object. 2) An embedding object: the client pointer points into the
 * middle of a standard object.  This is required by the need for the pointer
 * to point to an old-style SunWindows object.  In this latter case, the
 * embedded object must have the following structure:
 * --------------------------- | Xv_base		  |
 * --------------------------- | Other "std" fields	  |
 * --------------------------- | Private fields	  |
 * --------------------------- | Embedding seal & offset |
 * --------------------------- -- Client ptr ->	| SunWindow object	  |
 * ---------------------------
 * 
 * The "drivers" for the generic Sv operations look to see if the first four
 * bytes of the memory addressed by the pointer given to them contain the
 * value XV_OBJECT_SEAL.  If not, then the 4 bytes preceding should contain
 * the OR of XV_EMBEDDING_SEAL and a number between 12 and 255. This latter
 * number is the sizeof() all of the fields above the SunWindow object in
 * this class of embedding objects.  [WARNING: Note that the pkg must be
 * careful about the alignment restriction between the "Embedding seal &
 * offset" and the "SunWindow object".
 */
#ifndef XV_OBJECT_SEAL
#define XV_OBJECT_SEAL		0xF0A58142
#endif XV_OBJECT_SEAL
#define XV_EMBEDDING_SEAL	0xF1B69200
#define XV_EMBEDDING_MASK	0xFFFFFF00

/*
 * Extern
 */
extern struct pixrectops server_image_ops;

#define MAX_NESTED_PKGS	20

/* ------------------------------------------------------------------------ */


Xv_private int
xv_set_embedding_data(object, std_object)
    Xv_opaque       object, std_object;
{
    Xv_embedding   *lu_ptr = (Xv_embedding *) object;
    char           *c_ptr = (char *) object;
    int             byte_offset = c_ptr - (char *) std_object;

    if (*((unsigned long *) object) == XV_OBJECT_SEAL)
	/*
	 * Argh: data in object looks like seal of standard object. This will
	 * break all future references to object, so give up!
	 */
	xv_error(object,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_STRING,
#ifdef OW_I18N
     		  XV_I18N_MSG("xv_messages",
			"data in object looks like seal of standard object"),
#else
     		  "data in object looks like seal of standard object",
#endif OW_I18N
		 0);

    if (byte_offset != (byte_offset & (~XV_EMBEDDING_MASK)))
	/* The embedding header is too large for our encoding scheme. */
	xv_error(object,
		 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
		 ERROR_STRING,
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages",
			"embedding header too large for our encoding scheme"),
#else
		 "embedding header too large for our encoding scheme",
#endif OW_I18N

		 0);

    lu_ptr--;
    *lu_ptr = XV_EMBEDDING_SEAL | byte_offset;
    return (byte_offset);
}


Xv_private      Xv_opaque
xv_object_to_standard(object, caller)
    Xv_opaque       object;
    char           *caller;
{
    /* BUG ALERT!  Should this routine allow multiple levels of embedding? */

    register char  *c_ptr;
    register Xv_embedding *lu_ptr;
    register Xv_base *ccom_object = (Xv_base *) object;
    int             byte_offset;

    /* Not a standard object: see if embedded object. */
    c_ptr = (char *) object;
    lu_ptr = (Xv_embedding *) object;
    lu_ptr--;
    if ((*lu_ptr & XV_EMBEDDING_MASK) != XV_EMBEDDING_SEAL) {
	xv_error(object,
		 ERROR_INVALID_OBJECT,
#ifdef OW_I18N
		    XV_I18N_MSG("xv_messages", "embedding seal incorrect"),
#else
		    "embedding seal incorrect",
#endif
		 ERROR_STRING, caller,
		 0);
	return ((Xv_opaque) 0);
    }
    /* Embedding seal is okay: extract the offset. */
    byte_offset = *lu_ptr & (~XV_EMBEDDING_MASK);
    if (byte_offset < sizeof(Xv_base) + sizeof(lu_ptr)) {
	xv_error(object,
		 ERROR_INVALID_OBJECT,
#ifdef OW_I18N
		    XV_I18N_MSG("xv_messages", "byte offset incorrect"),
#else
		    "byte offset incorrect",
#endif
		 ERROR_STRING, caller,
		 0);
	return ((Xv_opaque) 0);
    }
    /* Offset is okay: see if offset pointer address a standard object. */
    c_ptr -= byte_offset;
    ccom_object = (Xv_base *) c_ptr;
    if (ccom_object->seal != XV_OBJECT_SEAL) {
	xv_error(object,
		 ERROR_INVALID_OBJECT,
#ifdef OW_I18N
		    XV_I18N_MSG("xv_messages", "standard seal incorrect"),
#else
		    "standard seal incorrect",
#endif OW_I18N
		 ERROR_STRING, caller,
		 0);
	return ((Xv_opaque) 0);
    }
    /* Offset pointer is okay. */
    return ((Xv_opaque) ccom_object);
}

/*VARARGS2*/
Xv_public       Xv_object
xv_find(parent, pkg, va_alist)
    Xv_opaque       parent;
    register Xv_pkg *pkg;
va_dcl
{
#ifdef ATTR_MAKE
    Xv_opaque       avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    va_list         args;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);

    return xv_find_avlist(parent, pkg, avarray);
#else
    Attr_avlist	    avlist = (Attr_avlist) &va_alist;

    if (*avlist == (Attr_attribute) ATTR_LIST) {
	/*
	 * This is a recursive attribute-value list: copy the original list,
	 * and expand the inline ATTR_LIST.
	 */
	Attr_attribute	avarray[ATTR_STANDARD_SIZE];
	Attr_avlist     avlist_copy = avarray;

	(void) attr_make(avlist_copy, ATTR_STANDARD_SIZE, (va_list) avlist);
	return xv_find_avlist(parent, pkg, avlist_copy);
    } else {
	return xv_find_avlist(parent, pkg, avlist);
    }
#endif				/* ATTR_MAKE */
}

Xv_private      Xv_object
xv_find_avlist(parent, pkg, avlist)
    Xv_opaque       parent;
    register Xv_pkg *pkg;
    Attr_attribute     avlist[ATTR_STANDARD_SIZE];
{
    register Xv_pkg *find_pkg;
    Attr_avlist     attrs;
    Xv_object       object = NULL;
    int             auto_create = TRUE, auto_create_seen = FALSE;

    for (attrs = (Attr_avlist) avlist;
	 *attrs; attrs = attr_next(attrs)) {
	switch ((int) attrs[0]) {
	  case XV_AUTO_CREATE:
	    auto_create = (int) attrs[1];
	    auto_create_seen = TRUE;
	    break;
	  default:
	    break;
	}
	if (auto_create_seen)	/* optimize if at front of list */
	    break;
    }
    if (!auto_create && !xv_initialized)
	return object;		/* why bother */

    /*
     * Now see if this is the first call to xv_find().
     */
    if (!xv_initialized) {
	xv_initialized = TRUE;
	/* use and xv_init attrs from the avlist */
	xv_init(ATTR_LIST, avlist, 0);
	/* create the default server */
	if (pkg != SERVER)
	    if (!xv_create(XV_NULL, SERVER, 0))
		(void) xv_connection_error((char *)NULL);
    }
    /*
     * Run through the pkg list looking for object "find" support. If we can
     * find an object of the same type as pkg, return that rather than
     * creating a new one.
     */
    for (find_pkg = pkg; find_pkg; find_pkg = find_pkg->parent_pkg) {
	if (find_pkg->find) {
	    object = (find_pkg->find) (parent, pkg, avlist);
	    if (object)
		break;
	}
    }
    if (!object && auto_create)
	object = xv_create_avlist(parent, pkg, avlist);

    return object;
}

/*VARARGS2*/
Xv_public       Xv_object
xv_create(parent, pkg, va_alist)
    	    	Xv_opaque   parent;
    register	Xv_pkg	   *pkg;
va_dcl
{
    Xv_object       object;
#ifdef ATTR_MAKE
    Attr_attribute  avarray[ATTR_STANDARD_SIZE];
    Attr_avlist     avlist = avarray;
    va_list         args;
#else
    Attr_avlist	    avlist = (Attr_avlist) &va_alist;
#endif				/* ATTR_MAKE */

    /* Check to see if copy rather than vanilla create */
    if (pkg == XV_COPY) {
	/* Parent is actually sibling for copy. */
	XV_OBJECT_TO_STANDARD(parent, "xv_create", object);
	if (!object)
	    return (XV_NULL);
	pkg = ((Xv_base *) object)->pkg;
	if (!pkg)
	    return (XV_NULL);
	*avlist++ = (Attr_attribute) XV_COPY_OF;
	*avlist++ = (Attr_attribute) parent;
	parent = xv_get(parent, (Attr_attribute) XV_OWNER);	/* May be 0 */
    }
#ifdef ATTR_MAKE
    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);

    return xv_create_avlist(parent, pkg, avarray);
#else
    if (*avlist == (Attr_attribute) ATTR_LIST) {
	/*
	 * This is a recursive attribute-value list: copy the original list,
	 * and expand the inline ATTR_LIST.
	 */
	Attr_attribute	avarray[ATTR_STANDARD_SIZE];
	Attr_avlist     avlist_copy = avarray;

	(void) attr_make(avlist_copy, ATTR_STANDARD_SIZE, (va_list) avlist);
	return xv_create_avlist(parent, pkg, avlist_copy);
    } else
	return xv_create_avlist(parent, pkg, avlist);
#endif				/* ATTR_MAKE */
}

Xv_private      Xv_object
xv_create_avlist(parent, pkg, avlist)
    	    	Xv_opaque	parent;
    register	Xv_pkg	       *pkg;
    	    	Attr_attribute  avlist[ATTR_STANDARD_SIZE];
{
    Xv_object       object = XV_NULL;
    Xv_base        *ccom_object;
    Xv_pkg         *pkg_stack[MAX_NESTED_PKGS];
    register Xv_pkg **pkgp, **loop_pkgp;
    register int    error_code = 0;
    int             embedding_offset, total_offset;
    Attr_attribute  argv[2];

    /*
     * first see if this is the first call to xv_create().
     */
    if (!xv_initialized) {
	xv_initialized = TRUE;
	/* use and xv_init attrs from the avlist */
	xv_init(ATTR_LIST, avlist, 0);
	/* create the default server */
	if (pkg != SERVER)
	    if (!xv_create(XV_NULL, SERVER, 0))
		(void) xv_connection_error((char *)NULL);
    }
    /*
     * Create object which begins as instance of no package.  It becomes an
     * instance of successively more specialized packages as each layer is
     * initialized.
     */
    object = xv_alloc_n(Xv_object, pkg->size_of_object);
    ccom_object = (Xv_base *) object;
    ccom_object->seal = XV_OBJECT_SEAL;
    /*
     * Stack packages oldest to youngest (base to client-visible pkg).
     */
    pkgp = pkg_stack;
    while (pkg) {
	*pkgp++ = pkg;
	pkg = pkg->parent_pkg;
    }
    /*
     * Execute stacked functions.
     */
    total_offset = 0;
    loop_pkgp = pkgp;
    while (pkg_stack <= --loop_pkgp && error_code == 0) {
	/* set object up as an instance of *pkgp */
	ccom_object->pkg = *loop_pkgp;
	if ((*loop_pkgp)->init) {	/* Ignore missing functions */
	    embedding_offset = 0;
	    /* BUG ALERT! Most init routines don't know about the 4th arg. */
	    error_code = ((*loop_pkgp)->init) (parent, object, avlist,
					       &embedding_offset);
	    total_offset += embedding_offset;
	}
    }

    if (error_code) {
	if (ccom_object->pkg->parent_pkg) {
	    /* Demote object to last successful package and destroy it. */
	    ccom_object->pkg = ccom_object->pkg->parent_pkg;
	    (void) xv_destroy_status(object, DESTROY_CLEANUP);
	}
	object = NULL;
    } else {
	/*
	 * Pass the avlist back to object to allow "normal" xv_set processing
	 * to do most (or all) of the work associated with the avlist.  Then,
	 * make a final pass via XV_END_CREATE to allow the object to do any
	 * "post-create" processing. Finally, set up for notifier destroy
	 * processing, using the public handle returned to client!
	 */
	(void) xv_set_avlist(object, avlist);

	/* Call set routines with XV_END_CREATE in init order */
	argv[0] = XV_END_CREATE;
	argv[1] = 0;
	loop_pkgp = pkgp;
	while (pkg_stack <= --loop_pkgp && error_code == 0) {
	    if ((*loop_pkgp)->set) {	/* Ignore missing functions */
		/*
		 * some set procs may return XV_SET_DONE, so deal with this.
		 */
		error_code = (int) ((*loop_pkgp)->set) (object, argv);
		switch (error_code) {
		  case (int) XV_OK:
		  case (int) XV_SET_DONE:
		    break;

		  default:
		    (void) xv_destroy_status(object, DESTROY_CLEANUP);
		    return (NULL);
		}
	    }
	}

	object = (Xv_opaque) ((char *) object + total_offset);
    }

    return (object);
}

/*VARARGS1*/
Xv_public       Xv_opaque
xv_set(object, va_alist)
    Xv_opaque       object;
va_dcl
{
#ifdef ATTR_MAKE
    Attr_avlist     avlist[ATTR_STANDARD_SIZE];
    va_list         args;

    va_start(args);
    (void) attr_make(avlist, ATTR_STANDARD_SIZE, args);
    va_end(args);

    return xv_set_avlist(object, avlist);
#else
    Attr_avlist	    avlist = (Attr_avlist) &va_alist;

    if (*avlist == (Attr_attribute) ATTR_LIST) {
	/*
	 * This is a recursive attribute-value list: copy the original list,
	 * and expand the inline ATTR_LIST.
	 */
	Attr_attribute	avarray[ATTR_STANDARD_SIZE];
	Attr_avlist     avlist_copy = avarray;

	(void) attr_make(avlist_copy, ATTR_STANDARD_SIZE, (va_list) avlist);
	return xv_set_avlist(object, avlist_copy);
    } else
	return xv_set_avlist(object, avlist);
#endif				/* ATTR_MAKE */
}

Xv_private      Xv_opaque
xv_set_pkg_avlist(object, pkg, avlist)
    register Xv_object object;
    register Xv_pkg *pkg;
    Attr_avlist     avlist;
/* Caller must guarantee that object is a standard, not embedded, object. */
{
    register Xv_opaque error_code;

    /*
     * Execute the set procs youngest to oldest (client-visible pkg to base
     * ). e.g. canvas-window-generic
     */
    for (; pkg; pkg = pkg->parent_pkg) {
	if (!pkg->set)
	    continue;

	/*
	 * we are done if the set proc says so or returns the bad attribute.
	 * If XV_SET_DONE is returned, the rest of the set has been done
	 * (using xv_super_set_avlist()), so return XV_OK now.
	 */
	if ((error_code = (*(pkg->set)) (object, avlist)) != XV_OK) {
	    return (error_code == XV_SET_DONE) ? XV_OK : error_code;
	}
    }

    return XV_OK;
}

Xv_private      Xv_opaque
xv_set_avlist(passed_object, avlist)
    Xv_opaque       passed_object;
    Attr_avlist     avlist;
{
    register Xv_opaque object;

    XV_OBJECT_TO_STANDARD(passed_object, "xv_set", object);
    if (!object)
	return (Xv_opaque) XV_ERROR;

    return xv_set_pkg_avlist(object, ((Xv_base *) object)->pkg, avlist);
}

Xv_public       Xv_opaque
xv_super_set_avlist(object, pkg, avlist)
    register Xv_opaque object;
    register Xv_pkg *pkg;
    Attr_avlist     avlist;
/* Caller must guarantee that object is a standard, not embedded, object. */
{
    return xv_set_pkg_avlist(object, pkg->parent_pkg, avlist);
}

/*VARARGS2*/
Xv_public       Xv_opaque
xv_get(passed_object, attr, va_alist)
    Xv_opaque       passed_object;
    Attr_attribute  attr;
va_dcl
{
    register Xv_pkg *pkg;
    int             status;
    Xv_opaque       result;
    va_list         args;
    Xv_opaque       object;

    XV_OBJECT_TO_STANDARD(passed_object, "xv_get", object);
    if (!object)
	return (Xv_opaque) 0;

    /*
     * Execute the get procs youngest to oldest (client-visible pkg to base
     * ). e.g. canvas-window-generic
     */
    for (pkg = ((Xv_base *) object)->pkg; pkg; pkg = pkg->parent_pkg) {
	if (!pkg->get)
	    continue;

	/*
	 * Assume object will handle the get. Object should set status to
	 * XV_ERROR if not handled
	 */
	status = XV_OK;
	/*
	 * Do va_start() every time to insure each pkg gets the start of the
	 * varargs.
	 */
	va_start(args);
	/* ask the object to handle the get */
	result = (*(pkg->get)) (object, &status, (Attr_attribute) attr, args);
	va_end(args);

	if (status == XV_OK) {
	    /* result is the answer -- return it */
	    return result;
	}
    }

    /*
     * None of the packages handled the get.  There are three possibilities,
     * but for all of them xv_get() should return 0: a) Attribute is valid,
     * but not for this object.  This is suspect, as it implies the caller is
     * relying on the fact that xv_get() returns 0 if no package recognizes a
     * valid attribute. b) Attribute is valid, and for this object.  Either
     * one or more of the packages' get procedures are in error, or there has
     * been a problem with the .h - .c file correspondence, possibly caused
     * by compilation phase error where not all of the object files in the
     * executable were compiled against the same version of the .h files. c)
     * Attribute is invalid.  Unfortunately, this is undetectable.
     */
    return 0;
}


#ifdef _XV_GET_VARARGS

/* if client already has handled varargs list use this procedure */
/* to start the get. We have duplicated the code from xv_get */
/* rather then shared so the generic xv_get doesn't incur */
/* another procedure call, and so the safety value in xv_get */
/* of restarting the varargs list for each package remains */
/* for the generic case. */

/* We should review whether xv_get should call xv_get_avlist */

/*VARARGS2*/
Xv_private      Xv_opaque
xv_get_varargs(passed_object, attr, valist)
    Xv_opaque       passed_object;
    Attr_attribute  attr;
    va_list         valist;
{
    register Xv_pkg *pkg;
    Xv_opaque       object;
    int             status;
    Xv_opaque       result;
    va_list         args;

    XV_OBJECT_TO_STANDARD(passed_object, "xv_get", object);
    if (!object)
	return (Xv_opaque) 0;

    /*
     * Execute the get procs youngest to oldest (client-visible pkg to base
     * ). e.g. canvas-window-generic
     */
    for (pkg = ((Xv_base *) object)->pkg; pkg; pkg = pkg->parent_pkg) {
	if (!pkg->get)
	    continue;

	/*
	 * Assume object will handle the get. Object should set status to
	 * XV_ERROR if not handled
	 */
	status = XV_OK;
	/*
	 * Assume client has done va_start. xv_get calls va_start() every
	 * time to insure each pkg gets the start of the varargs. we aren't.
	 * This should still work.
	 */
	/* ask the object to handle the get */
	result = (*(pkg->get)) (object, &status, attr, args);

	if (status == XV_OK) {
	    /* result is the answer -- return it */
	    return result;
	}
    }

    /*
     * None of the packages handled the get.  There are three possibilities,
     * but for all of them xv_get() should return 0: a) Attribute is valid,
     * but not for this object.  This is suspect, as it implies the caller is
     * relying on the fact that xv_get() returns 0 if no package recognizes a
     * valid attribute. b) Attribute is valid, and for this object.  Either
     * one or more of the packages' get procedures are in error, or there has
     * been a problem with the .h - .c file correspondence, possibly caused
     * by compilation phase error where not all of the object files in the
     * executable were compiled against the same version of the .h files. c)
     * Attribute is invalid.  Unfortunately, this is undetectable.
     */
    return 0;
}

#endif _XV_GET_VARARGS

/*
 * A fast overview of destruction. Conforming with SunView 1.X, xv_destroy()
 * is an immediate destroy that goes through the Notifier mechanisms.
 * However, since not all objects are guaranteed to already be registered
 * with the Notifier, xv_destroy() (and xv_destroy_safe()) always register
 * the object with the Notifier.  This causes buggy code that would have
 * worked in SunView 1.X to fail in SunView 2, because a second (incorrect)
 * destroy in SunView 1.X was ignored because the Notifier would no longer
 * know about the object as a side-effect of the first (correct) destroy.
 * xv_destroy_safe() exists to allow internal reference counting to work in
 * cases where the reference count temporarily drops to 0 (thereby triggering
 * a destroy) but then goes back up.  This scenario occurs when the client
 * code has temporarily stashed a reference to a shared object in one of its
 * local frames. xv_destroy_check() allows emulation of SunView 1.X's
 * window_destroy. xv_destroy_immediate() is used internally to SunView 2 to
 * "pre-walk" dependence hierarchies that the Notifier is not able to
 * recognize.
 */
static int
xv_destroy_internal(object, check_when, destroy_when, destruction_type)
    Xv_opaque       object;
    Notify_event_type check_when, destroy_when;
    short           destruction_type;
{
    if (!object)
	return XV_ERROR;

    notify_set_destroy_func(object, xv_destroy_status);

    if (destruction_type == DELETE_WINDOW) {
	if (notify_post_destroy(object, DESTROY_CHECKING, check_when) ==
	    NOTIFY_DESTROY_VETOED) {
	    /* If vetoed then insure destruction status if CLEANUP */
	    xv_set_destruction_type(DESTROY_CLEANUP);
	    return XV_ERROR;
	}
	notify_post_destroy(object, xv_get_destruction_type(), destroy_when);
    } else
	/*
	 * SAVE_YOURSELF which cannot be vetoed and is only a one phase
	 * destruction method.
	 */
	notify_post_destroy(object, DESTROY_SAVE_YOURSELF, destroy_when);

    return XV_OK;
}

Xv_public int
xv_destroy(object)
    Xv_opaque       object;
{
    return xv_destroy_internal(object, NOTIFY_IMMEDIATE, NOTIFY_IMMEDIATE,
			       DELETE_WINDOW);
}

Xv_public int
xv_destroy_check(object)
    Xv_opaque       object;
{
    return xv_destroy_internal(object, NOTIFY_IMMEDIATE, NOTIFY_SAFE,
			       DELETE_WINDOW);
}

Xv_public int
xv_destroy_immediate(object)
    Xv_opaque       object;
{
    return xv_destroy_internal(object, NOTIFY_IMMEDIATE, NOTIFY_IMMEDIATE,
			       DELETE_WINDOW);
}

Xv_public int
xv_destroy_safe(object)
    Xv_opaque       object;
{
    return xv_destroy_internal(object, NOTIFY_SAFE, NOTIFY_SAFE,
			       DELETE_WINDOW);
}

Xv_public int
xv_destroy_save_yourself(object)
    Xv_opaque       object;
{
    return xv_destroy_internal(object, NOTIFY_SAFE, NOTIFY_SAFE,
			       SAVE_YOURSELF);
}

Xv_public int
xv_destroy_status(passed_object, status)
    Xv_object       passed_object;
    Destroy_status  status;
{
    register Xv_pkg *pkg;
    Xv_opaque       object;

    XV_OBJECT_TO_STANDARD(passed_object, "xv_destroy_status", object);
    if (!object)
	return XV_ERROR;
    /*
     * Execute the destroy procs, youngest to oldest (client-visible pkg to
     * base), e.g. canvas-window-generic.
     */
    for (pkg = ((Xv_base *) object)->pkg; pkg; pkg = pkg->parent_pkg) {
	if (!pkg->destroy)	/* Ignore missing functions */
	    continue;
	/*
	 * If any one pkg vetoes the destroy, call it quits. BUG ALERT:
	 * quitting when status != DESTROY_CHECKING leaves the object in an
	 * inconsistent state.  Perhaps it should become an instance of the
	 * pkg that incorrectly vetoed.
	 */
	if ((pkg->destroy) (object, status) != XV_OK) {
	    if (status == DESTROY_CHECKING) {
		notify_veto_destroy();
		/* We got vetoed, insure status is set to CLEANUP for next   */
		/* destroy						     */
		xv_set_destruction_type(DESTROY_CLEANUP);
	    } else {
		char            dummy[128];

#ifdef OW_I18N
		(void) sprintf(dummy, "%s: %s '%s',\n\t%s (%d) - %s.",
			   XV_I18N_MSG("xv_messages", "xv_destroy_status"),
			   XV_I18N_MSG("xv_messages", "internal error in package"),
			   pkg->name,
			   XV_I18N_MSG("xv_messages",
				"attempted veto during wrong phase"),
			   (int) status,
			   XV_I18N_MSG("xv_messages", "send bug report"));
#else
		(void) sprintf(dummy, "%s: %s '%s',\n\t%s (%d) - %s.",
			   "xv_destroy_status", "internal error in package",
			     pkg->name, "attempted veto during wrong phase",
			       (int) status, "send bug report");
#endif
		xv_error(object,
			 ERROR_STRING, dummy,
			 0);
	    }
	    return XV_ERROR;
	}
	if ((status != DESTROY_CHECKING) && (status != DESTROY_SAVE_YOURSELF)) {
	    /* unlink package from instance */
	    ((Xv_base *) object)->pkg = pkg->parent_pkg;
	}
    }
    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	xv_free(object);
    }
    return XV_OK;
}

/*
 * The next two routines (and associated typedef and declarations) exist to
 * make it appear that all SunView 2 objects always register destroy funcs
 * with the Notifier without actually paying the storage and list-searching
 * costs that such registration requires.
 */
typedef         Notify_error(*Notify_error_func) ();
static Notify_error_func save_interpose_destroy_func;
extern Notify_error_func notify_set_destroy_interposer();

static          Notify_error
xv_destroy_interposer(nclient, func)
    Notify_client   nclient;
    Notify_func     func;
{
    Notify_error    status = save_interpose_destroy_func(nclient, func);

    if (status == NOTIFY_UNKNOWN_CLIENT || status == NOTIFY_NO_CONDITION) {
	/*
	 * nclient is not registered; so hope caller knows what it is doing,
	 * assume nclient is actually a SunView 2 object, and register it,
	 * then retry the interposition.
	 */
	notify_set_destroy_func(nclient, xv_destroy);
	status = save_interpose_destroy_func(nclient, func);
    }
    return (status);
}

Xv_private void
xv_set_destroy_interposer()
{
    save_interpose_destroy_func =
    notify_set_destroy_interposer(xv_destroy_interposer);
}

Xv_private int
xv_check_bad_attr(pkg, attr)
    register Xv_pkg *pkg;
    Attr_attribute  attr;
/*
 * At first glance the return values seem to be backwards.  However, if the
 * specified package was meant to handle the attribute, it wants to return a
 * zero value and have that accepted as the result, which requires it to not
 * modify *status, and thus we return XV_OK below in that case.
 */
{
    if ((
	 pkg->attr_id == ATTR_PKG(attr)
#ifdef ATTR_CONSUME_BUGS_FIXED
	 || pkg == XV_GENERIC_OBJECT
#endif
	) && !ATTR_CONSUMED(attr)) {
	xv_error(NULL,
		 ERROR_BAD_ATTR, attr,
		 0);
	return XV_OK;
    }
    return XV_ERROR;
}

/*
 * xv_calloc() -- error interposing version of calloc() for use with
 * xv_alloc() macro.
 */
Xv_private char *
xv_calloc(n, size)
    unsigned        n;
    unsigned        size;
{
    register char  *p;

    if (!(p = (char *) calloc(n, size)))
	xv_error(NULL,
		 ERROR_LAYER, ERROR_SYSTEM,
		 0);

    return p;
}

Xv_private      Destroy_status
xv_get_destruction_type()
{
    return destroy_status;
}

Xv_private void
xv_set_destruction_type(type)
    Destroy_status  type;
{
    destroy_status = type;
}

/*
 * This is used to determine whether or not a server object has been created
 * yet.
 */
Xv_private int
xv_has_been_initialized()
{
    return (int) xv_initialized;
}
