#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon_load.c 50.7 90/11/04";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 * 
 */

#include <stdio.h>
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/memvar.h>
#include <xview/rect.h>
#include <xview_private/icon_impl.h>
#include <xview_private/draw_impl.h>
#include <xview_private/pw_impl.h>
#include <xview/icon_load.h>

#ifdef OW_I18N 
#include <xview_private/xv_i18n_impl.h>  
#endif OW_I18N

#define NULL_PIXRECT	((struct pixrect *)0)
#define NULL_PIXFONT	((struct pixfont *)0)
#ifndef SVR4
extern char    *sprintf();
#endif SVR4
extern Pixrect *xv_mem_create();

FILE           *
icon_open_header(from_file, error_msg, info)
    char           *from_file, *error_msg;
    Xv_icon_header_info *info;
/* See comments in icon_load.h */
{
#define INVALID	-1
    register int    c;
    char            c_temp;
    register FILE  *result;

    if (from_file == "" ||
	(result = fopen(from_file, "r")) == NULL) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages","Cannot open file %s.\n"), from_file);
#else
	(void) sprintf(error_msg, "Cannot open file %s.\n", from_file);
#endif	OW_I18N
	goto ErrorReturn;
    }
    info->depth = INVALID;
    info->height = INVALID;
    info->format_version = INVALID;
    info->valid_bits_per_item = INVALID;
    info->width = INVALID;
    info->last_param_pos = INVALID;
    /*
     * Parse the file header
     */
    do {
	if ((c = fscanf(result, "%*[^DFHVW*]")) == EOF)
	    break;
	switch (c = getc(result)) {
	  case 'D':
	    if (info->depth == INVALID) {
		c = fscanf(result, "epth=%d", &info->depth);
		if (c == 0)
		    c = 1;
		else
		    info->last_param_pos = ftell(result);
	    }
	    break;
	  case 'H':
	    if (info->height == INVALID) {
		c = fscanf(result, "eight=%d", &info->height);
		if (c == 0)
		    c = 1;
		else
		    info->last_param_pos = ftell(result);
	    }
	    break;
	  case 'F':
	    if (info->format_version == INVALID) {
		c = fscanf(result, "ormat_version=%d",
			   &info->format_version);
		if (c == 0)
		    c = 1;
		else
		    info->last_param_pos = ftell(result);
	    }
	    break;
	  case 'V':
	    if (info->valid_bits_per_item == INVALID) {
		c = fscanf(result, "alid_bits_per_item=%d",
			   &info->valid_bits_per_item);
		if (c == 0)
		    c = 1;
		else
		    info->last_param_pos = ftell(result);
	    }
	    break;
	  case 'W':
	    if (info->width == INVALID) {
		c = fscanf(result, "idth=%d", &info->width);
		if (c == 0)
		    c = 1;
		else
		    info->last_param_pos = ftell(result);
	    }
	    break;
	  case '*':
	    if (info->format_version == 1) {
		c = fscanf(result, "%c", &c_temp);
		if (c_temp == '/')
		    c = 0;	/* Force exit */
		else
		    (void) ungetc(c_temp, result);
	    }
	    break;
	  default:{
#ifdef	OW_I18N
		(void) sprintf(error_msg, XV_I18N_MSG("xv_messages", "icon file %s parse failure\n"), from_file);
#else
		(void) sprintf(error_msg, "icon file %s parse failure\n", from_file);
#endif	OW_I18N
		goto ErrorReturn;
	    }
	}
    } while (c != 0 && c != EOF);
    if (c == EOF || info->format_version != 1) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages","%s has invalid header format.\n"), from_file);
#else
	(void) sprintf(error_msg, "%s has invalid header format.\n", from_file);
#endif	OW_I18N
	goto ErrorReturn;
    }
    if (info->depth == INVALID)
	info->depth = 1;
    if (info->height == INVALID)
	info->height = 64;
    if (info->valid_bits_per_item == INVALID)
	info->valid_bits_per_item = 16;
    if (info->width == INVALID)
	info->width = 64;
    if (info->depth != 1) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages", "Cannot handle Depth of %d.\n"), info->depth);
#else
	(void) sprintf(error_msg, "Cannot handle Depth of %d.\n", info->depth);
#endif	OW_I18N
	goto ErrorReturn;
    }
    if (info->valid_bits_per_item != 16 &&
	info->valid_bits_per_item != 32) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages","Cannot handle Valid_bits_per_item of %d.\n"),
#else
	(void) sprintf(error_msg, "Cannot handle Valid_bits_per_item of %d.\n",
#endif	OW_I18N
		       info->valid_bits_per_item);
	goto ErrorReturn;
    }
    if ((info->width % 16) != 0) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages","Cannot handle Width of %d.\n"), info->width);
#else
	(void) sprintf(error_msg, "Cannot handle Width of %d.\n", info->width);
#endif	OW_I18N
	goto ErrorReturn;
    }
    return (result);

ErrorReturn:
    if (result)
	(void) fclose(result);
    return (NULL);
#undef INVALID
}

static int
icon_read_pr(fd, header, pr)
    register FILE  *fd;
    register Xv_icon_header_info *header;
    register struct pixrect *pr;
{
    register int    c, i, j, index, width;
    register struct mpr_data *mprdata;
    long            value;

    mprdata = (struct mpr_data *) (pr->pr_data);

    if (header->width == 48)
	width = 64;
    else
	width = header->width;

    for (i = 0; i < header->height; i++) {
	for (j = 0; j < width / 16; j++) {
	    c = fscanf(fd, " 0x%lx,", &value);
	    if (c == 0 || c == EOF)
		break;

	    index = j + i * mprdata->md_linebytes / 2;
	    switch (header->valid_bits_per_item) {
	      case 16:
		mprdata->md_image[index] = value;
		break;
#ifdef sun
	      case 32:
		mprdata->md_image[index] = (value >> 16);
		mprdata->md_image[index] = (value & 0xFFFF);
		break;
#endif
	      default:
		xv_error(NULL,
			 ERROR_SEVERITY, ERROR_NON_RECOVERABLE,
			 ERROR_STRING,
#ifdef	OW_I18N
			     XV_I18N_MSG("xv_messages","icon file header valid bits not 16 or 32"),
#else
			     "icon file header valid bits not 16 or 32",
#endif	OW_I18N
			 ERROR_PKG, ICON,
			 0);
	    }
	}
    }
}

struct pixrect *
icon_load_mpr(from_file, error_msg)
    char           *from_file, *error_msg;
/* See comments in icon_load.h */
{
    register FILE  *fd;
    Xv_icon_header_info header;
    register struct pixrect *result;

    fd = icon_open_header(from_file, error_msg, &header);
    if (fd == NULL)
	return (NULL_PIXRECT);
    /*
     * Allocate the pixrect and read the actual bits making up the icon.
     */
    result = xv_mem_create(header.width, header.height, header.depth);
    if (result == NULL_PIXRECT) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages", "Cannot create memory pixrect %dx%dx%d.\n"),
		       header.width, header.height, header.depth);
#else
	(void) sprintf(error_msg, "Cannot create memory pixrect %dx%dx%d.\n",
		       header.width, header.height, header.depth);
#endif	OW_I18N
	goto Return;
    }
    (void) icon_read_pr(fd, &header, result);
Return:
    (void) fclose(fd);
    return (result);
}


Server_image
icon_load_svrim(from_file, error_msg)
    char           *from_file, *error_msg;
{
    register FILE  *fd;
    Display	   *display;
    GC		    gc;
    Xv_icon_header_info header;
    Xv_Drawable_info *info;
    register struct pixrect *mpr;
    Server_image result;

    fd = icon_open_header(from_file, error_msg, &header);
    if (fd == NULL)
	return (NULL);
    /*
     * Allocate the memory pixrect and read the actual bits making up the icon.
     */
    mpr = xv_mem_create(header.width, header.height, header.depth);
    if (mpr == NULL_PIXRECT) {
#ifdef	OW_I18N
	(void) sprintf(error_msg, XV_I18N_MSG("xv_messages", "Cannot create memory pixrect %dx%dx%d.\n"),
		       header.width, header.height, header.depth);
#else
	(void) sprintf(error_msg, "Cannot create memory pixrect %dx%dx%d.\n",
		       header.width, header.height, header.depth);
#endif	OW_I18N
	goto Return;
    }
    (void) icon_read_pr(fd, &header, mpr);

    /* 
     * Create the Server Image from the memory pixrect.
     */
    result = xv_create(0, SERVER_IMAGE,
	XV_WIDTH,	header.width,
	XV_HEIGHT,	header.height,
	SERVER_IMAGE_DEPTH, header.depth,
	0);
    DRAWABLE_INFO_MACRO(result, info);
    display = xv_display(info);
    gc = xv_gc(result, info);
    xv_set_gc_op(display, info, gc, PIX_SRC, XV_USE_CMS_FG, XV_DEFAULT_FG_BG);
    XSetPlaneMask(display, gc, (0x1 << mpr->pr_depth) - 1);
    xv_rop_mpr_internal(display, xv_xid(info), gc,
	0, 0, mpr->pr_width, mpr->pr_height, mpr, 0, 0, info, TRUE);
    xv_free(mpr);
Return:
    (void) fclose(fd);
    return (result);
}


int
icon_init_from_pr(icon_public, pr)
    Icon            icon_public;
    register struct pixrect *pr;
/* See comments in icon_load.h */
{
    Xv_icon_info   *icon = ICON_PRIVATE(icon_public);

    icon->ic_mpr = pr;
    /*
     * Set the icon's size and graphics area to match its pixrect's extent.
     */
    icon->ic_gfxrect.r_top = icon->ic_gfxrect.r_left = 0;
    icon->ic_gfxrect.r_width = pr->pr_size.x;
    icon->ic_gfxrect.r_height = pr->pr_size.y;
    /*
     * By default, the icon has no text or associated area.
     */
    icon->ic_textrect = rect_null;
    icon->ic_text = NULL;
    /*
     * Also, by default, the icon has no background pattern.
     */
    icon->ic_background = NULL_PIXRECT;
    icon->ic_flags = 0;
}

int
icon_load(icon_public, from_file, error_msg)
    Icon            icon_public;
    char           *from_file, *error_msg;
/* See comments in icon_load.h */
{
    register struct pixrect *pr;

    if (!icon_public)
	return (XV_ERROR);

    pr = icon_load_mpr(from_file, error_msg);
    if (pr == NULL_PIXRECT)
	return (XV_ERROR);
    (void) icon_init_from_pr(icon_public, pr);
    return (XV_OK);
}
