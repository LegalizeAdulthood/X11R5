/*      @(#)xv_i18n_impl.h 1.2 90/11/01 SMI	*/
/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *      file for terms of the license.
 */

#ifndef xview_xv_i18n_impl_h_DEFINED
#define xview_xv_i18n_impl_h_DEFINED

#ifdef OW_I18N
#define XV_I18N_MSG(d,s)	(dgettext(d,s))
#else
#define XV_I18N_MSG(d,s)	((s))
#endif

#endif xview_xv_i18n_impl_h_DEFINED
