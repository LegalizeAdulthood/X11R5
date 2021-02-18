/*	@(#)curs_impl.h 50.3 90/10/16 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef  _xview_cursor_impl_h_already_include
#define  _xview_cursor_impl_h_already_include

#include <xview/cursor.h>
#include <xview_private/draw_impl.h>

#ifdef OW_I18N
#include <X11/Xresource.h>
#endif
#define CURSOR_PRIVATE(cursor_public)	\
	    XV_PRIVATE(Cursor_info, Xv_cursor_struct, cursor_public)
#define CURSOR_PUBLIC(cursor_private)  XV_PRIVATE(cursor_private)

#define	DONT_SHOW_CURSOR	0x00000001
#define	FREE_SHAPE		0x00000080
#define	show_cursor(cursor)		(!((cursor)->flags & DONT_SHOW_CURSOR))
#define	free_shape(cursor)		((cursor)->flags & FREE_SHAPE)

#ifdef SUNVIEW1
#define	SHOW_CROSSHAIRS		0x00000002
#define	SHOW_HORIZ_HAIR		0x00000004
#define	SHOW_VERT_HAIR		0x00000008
#define	FULLSCREEN		0x00000010
#define	HORIZ_BORDER_GRAVITY	0x00000020
#define	VERT_BORDER_GRAVITY	0x00000040

#define	show_crosshairs(cursor)		((cursor)->flags & SHOW_CROSSHAIRS)
#define	show_horiz_hair(cursor)		((cursor)->flags & SHOW_HORIZ_HAIR)
#define	show_vert_hair(cursor)		((cursor)->flags & SHOW_VERT_HAIR)
#define	cursor_fullscreen(cursor)	((cursor)->flags & FULLSCREEN)
#define	horiz_border_gravity(cursor)	((cursor)->flags & HORIZ_BORDER_GRAVITY)
#define	vert_border_gravity(cursor)	((cursor)->flags & VERT_BORDER_GRAVITY)
#endif


typedef struct {
	Xv_opaque	public_self;	/* back pointer */
	short	cur_xhot, cur_yhot;	/* offset of mouse position from shape*/
	int	cur_src_char, cur_mask_char;	/* source and mask characters */
	int	cur_function;		/* relationship of shape to screen */
	Pixrect *cur_shape;		/* memory image to use */
	int	flags;			/* various options */
	unsigned long  cursor_id;	/* X cursor id		     */
	Xv_object  root;		/* root handle		     */
	Xv_singlecolor	fg, bg;		/* fg/bg color of cursor */
#ifdef SUNVIEW1
	short	horiz_hair_thickness;	/* horizontal crosshair height */
	int	horiz_hair_op;		/*            drawing op       */
	int	horiz_hair_color;	/*            color            */ 
	short	horiz_hair_length;	/*            width           */ 
	short	horiz_hair_gap;		/*            gap             */ 

	short	vert_hair_thickness;	/* vertical crosshair width  */
	int	vert_hair_op;		/*          drawing op       */
	int	vert_hair_color;	/*          color            */ 
	short	vert_hair_length;	/*          height           */ 
	short	vert_hair_gap;   	/*          gap              */ 
#endif
} Cursor_info;
/* from cursor.c */
Pkg_private  Xv_opaque       cursor_set_internal();
Pkg_private  int             cursor_create_internal();
Pkg_private  Xv_opaque       cursor_get_internal();
Pkg_private  int             cursor_destroy_internal();

#ifdef notdef
/*
 * The following two macros are intended to make it easy for novice users to
 * make their own cursors without having to understand how to use mpr_static.
 */

#define DEFINE_CURSOR_FROM_IMAGE(name, hot_x, hot_y, func, image)	\
	static struct mpr_data CAT(name,_data) =			\
	    {mpr_linebytes(16,1), (short *)(image), {0, 0}, 0, 0};	\
	extern struct pixrectops mem_ops;				\
	static struct pixrect CAT(name,_mpr) =				\
	    {&mem_ops, 16, 16, 1, (caddr_t)&CAT(name,_data)};		\
	static struct cursor name = {(hot_x), (hot_y), (func), &CAT(name,_mpr)};

#define	DEFINE_CURSOR(name, hot_x, hot_y, func, i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16)					\
	static short CAT(name,_image)[16] = {				\
	    i1, i2, i3, i4, i5, i6, i7, i8,				\
	    i9, i10, i11, i12, i13, i14, i15, i16};			\
	DEFINE_CURSOR_FROM_IMAGE(name, (hot_x), (hot_y), func, CAT(name,_image))
#endif
#endif	_xview_cursor_impl_h_already_included
