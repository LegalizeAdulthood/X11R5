/*      "@(#)aux.h 50.4 91/01/15"		 */

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *      file for terms of the license.
 */

#ifndef	aux_DEFINED
#define	aux_DEFINED

#define	AUX_UDATA_INFO(udata)	((AuxInfo *)(*(Xv_opaque *)udata))
#define	AUX_KEY_DATA		101

typedef	struct _aux_info {
	Display		*dpy;
	XIC		ic;
	Bool		state;
	Frame		frame;
	Panel		panel;
	Panel_item	item;
	Xv_object	p_obj;
        Frame		p_frame;
}  AuxInfo;
	
#endif		~aux_DEFINED
