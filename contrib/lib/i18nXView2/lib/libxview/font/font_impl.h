/*      @(#)font_impl.h 50.7 90/12/12 SMI      */

/***********************************************************************/
/*	                      font_impl.h			       */
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/***********************************************************************/

#ifndef font_impl_h_already_defined
#define font_impl_h_already_defined

#include <sys/types.h>
#include <sys/time.h>

#include <xview/pkg.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <xview/font.h>

#ifdef OW_I18N
#include <X11/Xresource.h>
#endif /*OW_I18N*/


#define	FONT_PRIVATE(font)	XV_PRIVATE(Font_info, Xv_font_struct, font)
#define	FONT_PUBLIC(font)	XV_PUBLIC(font)
#define	FONT_STANDARD(font, object)	XV_OBJECT_TO_STANDARD(XV_PUBLIC(font), \
							 \"font\", object)
#define FONT_PIXFONT_TO_PUBLIC(pixfont) \
		(((Pixfont_struct *)(pixfont))->public_self)

#define FONT_PIXFONT_STRUCT_TO_PIXFONT(pfs) (*((Pixfont **)pfs))

#define	FONT_PIX(font)		(Pixfont *)XV_PUBLIC(font)

#ifdef OW_I18N

/* definitions for font set file key words */
#define FS_DEF		"definition"
#define FS_DEF_LEN	strlen(FS_DEF)

#define FS_ALIAS	"alias"
#define FS_ALIAS_LEN	strlen(FS_ALIAS)

#define FS_SMALL_SIZE   	"xv_font_set.small"
#define FS_MEDIUM_SIZE  	"xv_font_set.medium"
#define FS_LARGE_SIZE   	"xv_font_set.large"
#define FS_XLARGE_SIZE  	"xv_font_set.extra_large"
#define FS_DEFAULT_FAMILY  	"xv_font_set.default_family"

#endif /*OW_I18N*/

/***********************************************************************/
/*	        	Structures 				       */
/***********************************************************************/

typedef struct {
    char	*pixfont[2+(5*256)];
    Xv_Font	public_self;
}Pixfont_struct;

#ifdef OW_I18N

typedef struct font_locale_info {
    char			*locale;
    XrmDatabase 		db;
    short			small_size;
    short			medium_size;
    short			large_size;
    short			xlarge_size;
    char			*default_family;
    struct font_locale_info    *next;
} Font_locale_info;

#endif /*OW_I18N*/

typedef struct font_info {
    Xv_Font	 	 public_self;	/* back pointer to public struct */
    Attr_pkg		 pkg;
    Xv_opaque	 	 parent;	/* back pointer to screen */
    Xv_opaque	 	 display;
    Xv_opaque	 	 server;
    struct font_info	*next;
    
    /* family, style and point-size */
    char		*name;

#ifdef OW_I18N
    Font_locale_info	*locale_info;
    char		*specifier;
#endif /*OW_I18N*/

    char		*foundry;
    char		*family;
    char		*style;
    
    char		*weight;
    char		*slant;
    char		*setwidthname;
    char		*addstylename;

    int			 scale;
    
    int			 size; /* for this scale */
    int			 small_size;
    int			 medium_size;
    int			 large_size;
    int			 extra_large_size;
    
    int			 ref_count;

    Font_type	 	 type;	/* text, glyph or cursor */

    char		*pixfont; /* pixfont for sunview compat */
    int			def_char_width;
    int			def_char_height;

    /* interface to Xlib */
#ifdef OW_I18N
    XFontSet		 set_id;
    XFontStruct		 **font_structs;
#else
    long unsigned	 xid;
    Xv_opaque 		 x_font_info;
#endif /*OW_I18N*/

    /* flags */
    unsigned		 has_glyph_prs:1;
    unsigned		 overlapping_chars:1;
} Font_info;




/* from font.c */
Pkg_private Xv_opaque    font_set_avlist();
Pkg_private Xv_opaque    font_get_attr();
Pkg_private int 	 font_init();
Pkg_private int 	 font_destroy_struct();
Pkg_private Xv_object 	 font_find_font();

#endif
