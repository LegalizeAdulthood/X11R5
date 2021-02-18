#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)cms.c 50.5 90/11/04";
#endif
#endif

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xview/cms.h>
#include <xview_private/cms_impl.h>

#ifdef OW_I18N
#include <xview_private/xv_i18n_impl.h>
#endif

/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL NOTICE
 *      file for terms of the license.
 */

Pkg_private void
cms_init_default_cms(cms)
    Cms_info        *cms;
{
    Xv_opaque	  	server;
    Display	    	*display;
    Xv_Colormap    	*cmap;
    int			screen_number;
    Visual		*visual;

    server = xv_get(cms->screen, SCREEN_SERVER);
    display = (Display *)xv_get(server, XV_DISPLAY);
    screen_number = (int)xv_get(cms->screen, SCREEN_NUMBER);

    cmap = xv_alloc(Xv_Colormap);
    cmap->id = DefaultColormap(display, screen_number);
    /* BUG: we shouldn't be using the class field of a visual */
    cms->visual_class = DefaultVisual(display, screen_number)->class;
    cmap->type = (cms->visual_class % 2) ? XV_DYNAMIC_CMAP : XV_STATIC_CMAP;
    cmap->visual_class = cms->visual_class;
    cmap->cms_list = cms;
    xv_set(cms->screen, SCREEN_COLORMAP_LIST, cmap, 0);

    cms->name = (char *)malloc(strlen(XV_DEFAULT_CMS) + 1);
    strcpy(cms->name, XV_DEFAULT_CMS);
    cms->type = XV_STATIC_CMS;
    cms->size = 2;
    cms->index_table =
	    xv_alloc_n(unsigned long *, 2 * sizeof(unsigned long));
    cms->index_table[0] = WhitePixel(display, screen_number);
    cms->index_table[1] = BlackPixel(display, screen_number);
    cms->cmap = cmap;
    STATUS_SET(cms, default_cms);
}

/*
 *	cms_free_colors() frees all the colors in the colormap that 
 *	have been allocated for this colormap segment.
 */
Pkg_private void
cms_free_colors(cms)
    Cms_info	*cms;
{
    Xv_opaque		server;
    Display		*display;
    register int	i;

    if ((cms->index_table == NULL) || (cms->cmap == NULL)) {
	return;
    }

    server = xv_get(cms->screen, SCREEN_SERVER);
    display = (Display *)xv_get(server, XV_DISPLAY);

    for (i = 0; i <= cms->size - 1; i++) {
        if (cms->index_table[i] != XV_INVALID_PIXEL) {
            XFreeColors(display, cms->cmap->id, &cms->index_table[i], 1, 0);
        }
    }
}

/*
 *	cms_set_name() sets the secified name for the cms.
 */
Pkg_private void
cms_set_name(cms, name)
    Cms_info    *cms;
    char	*name;
{
   if (cms->name != NULL) {
	xv_free(cms->name);
       	cms->name = NULL;
   }

   cms->name = (char *)malloc(strlen(name) + 1);
   strcpy(cms->name, name);
}


/* 
 *	cms_set_unique_name() generates & sets a unique name for the cms.
 */
Pkg_private void
cms_set_unique_name(cms)
    Cms_info    *cms;
{
    char	buf[20];

    if (cms->name != NULL) {
	xv_free(cms->name);
    }

    sprintf(buf, "%x", cms);
    cms->name = (char *)malloc(strlen("xv_cms_") + strlen(buf));
    sprintf(cms->name, "xv_cms_%s", buf);
}

/* 
 * cms_get_colors() returns the colors either as an array of Xv_Singlecolor,
 * as an array of XColors, or as an array of red, green and blue colors.
 */
Pkg_private int
cms_get_colors(cms, cms_index, cms_count, cms_colors, cms_x_colors, 
	red, green, blue)
    Cms_info            *cms;
    unsigned long       cms_index, cms_count;
    Xv_Singlecolor      *cms_colors;
    XColor		*cms_x_colors;
    unsigned char	*red, *green, *blue;
{
    register int        i;
    XColor              *xcolors = NULL;
    Xv_opaque           server;
    Display             *display;
    unsigned long	valid_pixel = XV_INVALID_PIXEL;

    /* 
     * Check to see if atleast one cell has been allocated among the 
     * ones being retrieved.
     */
    for (i = 0; i <= cms_count - 1; i++) {
	if (cms->index_table[cms_index + i] != XV_INVALID_PIXEL) {
	    valid_pixel = cms->index_table[cms_index + i];
	    break;
	}
    }

    /* none of the pixels being retrieved have been allocated */
    if (valid_pixel == XV_INVALID_PIXEL) {
	return(XV_ERROR);
    }

    server = xv_get(cms->screen, SCREEN_SERVER);
    display = (Display *)xv_get(server, XV_DISPLAY);

    if (!cms_x_colors) {
	if ((xcolors = (XColor *)malloc(cms_count * sizeof(XColor))) == NULL)
	    return(XV_ERROR);
    } else {
	xcolors = cms_x_colors;
    }

    for (i = 0; i <= cms_count - 1; i++) {
	if (cms->index_table[cms_index + i] != XV_INVALID_PIXEL)
  	    xcolors[i].pixel = cms->index_table[cms_index + i];
	else
	    xcolors[i].pixel = valid_pixel;
    }
    XQueryColors(display, cms->cmap->id, xcolors, cms_count);
    
    if (cms_colors) {
	for (i = 0; i <= cms_count - 1; i++) {
	    cms_colors[i].red = xcolors[i].red >> 8;
	    cms_colors[i].green = xcolors[i].green >> 8;
	    cms_colors[i].blue = xcolors[i].blue >> 8;
	}
    } else if (!cms_x_colors) {
	for (i = 0; i <= cms_count - 1; i++) {
	    red[i] = xcolors[i].red >> 8;
	    green[i]  = xcolors[i].green >> 8;
	    blue[i] = xcolors[i].blue >> 8;
	}
    }

    if (xcolors  && (xcolors != cms_x_colors))
        xv_free(xcolors);

    return(XV_OK);
}

Pkg_private int
cms_set_colors(cms, cms_colors, cms_x_colors, cms_index, cms_count)
    Cms_info       	*cms;
    Xv_Singlecolor	*cms_colors;
    XColor		*cms_x_colors;
    unsigned long	cms_index, cms_count;
{
    register int    i;
    XColor	    *xcolors = NULL;
    Xv_opaque	    server;
    Display    	    *display;
    int		    status;

    if (cms->index_table == NULL) {
	return(XV_ERROR);
    }

    server = xv_get(cms->screen, SCREEN_SERVER);
    display = (Display *)xv_get(server, XV_DISPLAY);

    if (cms_colors) {
        xcolors = xv_alloc_n(XColor *, cms_count * sizeof(XColor));
        for (i = 0; i <= cms_count - 1; i++) {
            (xcolors + i)->red = (unsigned short)(cms_colors + i)->red << 8;
            (xcolors + i)->green = (unsigned short)(cms_colors + i)->green << 8;
            (xcolors + i)->blue = (unsigned short)(cms_colors + i)->blue << 8;
            (xcolors + i)->flags = DoRed | DoGreen | DoBlue;
        } 
    } else if (cms_x_colors) {
	xcolors = cms_x_colors;
    }

    if (cms->type == XV_STATIC_CMS) {
	status =
	    cms_set_static_colors(display, cms, xcolors, cms_index, cms_count);
    } else {
	status =
	    cms_set_dynamic_colors(display, cms, xcolors, cms_index, cms_count);
    }

    if (xcolors != cms_x_colors) {
	xv_free(xcolors);
    }

    return(status);
}


Pkg_private int 
cms_set_static_colors(display, cms, xcolors, cms_index, cms_count)
    Display		*display;
    Cms_info            *cms;
    XColor              *xcolors;
    unsigned long       cms_index, cms_count;
{
    int			status;
    Xv_Colormap		*cmap_list, *cmap = NULL;
    Visual              *visual;
    XVisualInfo		vinfo;
    int                 screen_num;

    /* 
     * Always attempt to allocate read-only colors from the default 
     * colormap. If the allocation fails, try any other colormaps 
     * that have been previously created. If that fails allocate a 
     * PseudoColor colormap and allocate the read-only cells from it.
     * Note: No provision to allocate out of an available StaticColor 
     * colormap, when a PseudoColor colormap is the default.
     */
    if (cms->cmap == NULL) {
	cmap_list = (Xv_Colormap *)xv_get(cms->screen, SCREEN_COLORMAP_LIST);

	for (cmap = cmap_list; cmap != NULL; cmap = cmap->next) {
	    if (cms->visual_class == cmap->visual_class) {
		status = cms_alloc_static_colors(display, cms, cmap, xcolors,
						 cms_index, cms_count);
		if (status == XV_ERROR) {
		    cms->cmap = cmap;
		    cms_free_colors(cms);
		    cms->cmap = NULL;
		} else {
		    cms->cmap = cmap;
		    cms->next = cmap->cms_list;
		    cmap->cms_list = cms;
		    break;
		}
	    }
	}
	
	if (cmap == NULL) {
	    /* could not use any of the currently available colormaps */
	    cmap = xv_alloc(Xv_Colormap);
            screen_num = (int)xv_get(cms->screen, SCREEN_NUMBER);
	    if (XMatchVisualInfo(display, screen_num, DefaultDepth(display, screen_num),
				 cms->visual_class, &vinfo))
	      visual = vinfo.visual;
	    else 
		visual = DefaultVisual(display, screen_num);
	    cmap->visual_class = visual->class;
	    cmap->type = (visual->class % 2) ? XV_STATIC_CMAP : XV_DYNAMIC_CMAP;
            cmap->id = XCreateColormap(display, 
				       RootWindow(display, screen_num),
				       visual, AllocNone);
	    cms->cmap = cmap;
	    cmap->cms_list = cms;
	    
	    /*
             * Add colormap to the screen's colormap list. The default 
             * colormap is always the first element in the colormap list.
             */
            cmap->next = cmap_list->next;
            cmap_list->next = cmap;

    	    status = cms_alloc_static_colors(display, cms, cmap, xcolors,
			cms_index, cms_count);
	    if (status == XV_ERROR) {
 		cms_free_colors(cms);
		cms->cmap = NULL;
	    }
	}
    } else {
	status = cms_alloc_static_colors(display, cms, cms->cmap,
			xcolors, cms_index, cms_count);
    }

    return(status);
}

Pkg_private int
cms_alloc_static_colors(display, cms, cmap, xcolors, cms_index, cms_count)
    Display             *display;
    Cms_info            *cms;
    Xv_Colormap		*cmap;
    XColor              *xcolors;
    unsigned long       cms_index, cms_count;
{
    unsigned long       *pixel;
    register int        i;

    if (xcolors == NULL)
	return(XV_OK);

    for (i = 0; i <= cms_count - 1; i++) {
	pixel = &cms->index_table[cms_index + i];

	/* static cms pixels are write-once only */
	if (*pixel == XV_INVALID_PIXEL) {
	    if (!XAllocColor(display, cmap->id, xcolors + i)) {
		return(XV_ERROR);
	    }
	    *pixel = (xcolors + i)->pixel;
	}
    }

    return(XV_OK);
}

Pkg_private int
cms_set_dynamic_colors(display, cms, xcolors, cms_index, cms_count)
    Display             *display;
    Cms_info            *cms;
    XColor              *xcolors;
    unsigned long       cms_index, cms_count;
{
    Xv_Colormap		*cmap_list, *cmap = NULL;
    Visual		*visual;
    XVisualInfo		vinfo;
    int			screen_num;
    register int	i;

    /*
     * Always allocate out of a PseudoColor colormap. If the default colormap 
     * is PseudoColor, and enough free cells are available, good. If not, 
     * try and allocate out of any other PseudoColor colormap. If none, 
     * create a new PseudoColor colormap.
     */
    if (cms->cmap == NULL) { 
	cmap_list = (Xv_Colormap *)xv_get(cms->screen, SCREEN_COLORMAP_LIST);
	for (cmap = cmap_list; cmap != NULL; cmap = cmap->next) {
	    if (cms->visual_class == cmap->visual_class) {
		if (XAllocColorCells(display, cmap->id, True, NULL,
			0, cms->index_table, cms->size)) 
		    break;	
	    }
	}

	if (cmap == NULL) {
	    cmap = xv_alloc(Xv_Colormap);
	    screen_num = (int)xv_get(cms->screen, SCREEN_NUMBER);
	    if (XMatchVisualInfo(display, screen_num, DefaultDepth(display, screen_num),
				 cms->visual_class, &vinfo)) 
	      visual = vinfo.visual;
	    else 
		visual = DefaultVisual(display, screen_num);
	    cmap->visual_class = visual->class;
	    cmap->type = (visual->class % 2) ? XV_STATIC_CMAP : XV_DYNAMIC_CMAP;
            cmap->id = XCreateColormap(display, 
				       RootWindow(display, screen_num),
				       visual, AllocNone);
	    cms->cmap = cmap;
	    cmap->cms_list = cms;
	    if (!XAllocColorCells(display, cmap->id, True, NULL,
			0, cms->index_table, cms->size)) {
		xv_free(cmap);
		return(XV_ERROR);
	    }

	    /*
	     * Add colormap to the screen's colormap list. The default colormap  
	     * is always the first element in the colormap list.
	     */
	    cmap->next = cmap_list->next;
	    cmap_list->next = cmap;
	} else {
	    cms->cmap = cmap;
	    cms->next = cmap->cms_list;
	    cmap->cms_list = cms;
	}
    }

    if (xcolors != NULL) {
        for (i = 0; i <= cms_count - 1; i++) {
	    (xcolors + i)->pixel = cms->index_table[cms_index + i];
        }
        XStoreColors(display, cms->cmap->id, xcolors, cms_count);
    }

    return(XV_OK);
}

XColor *
cms_parse_named_colors(cms, named_colors)
    Cms_info            *cms;
    char		**named_colors;
{
    XColor		*xcolors;
    int	    		count = 0;
    Display             *display;
    Xv_opaque           server;
    Cms			default_cms_public;
    Cms_info		*default_cms;

    if ((named_colors == NULL) || (*named_colors == NULL))
	return(NULL);

    while (named_colors[count])
	++count;

    xcolors = (XColor *)malloc(count * sizeof(XColor));

    server = xv_get(cms->screen, SCREEN_SERVER);
    display = (Display *)xv_get(server, XV_DISPLAY);
    default_cms_public = (Cms)xv_get(cms->screen, SCREEN_DEFAULT_CMS);
    default_cms = CMS_PRIVATE(default_cms_public);

    for (--count; count >= 0; --count) {
	if (!XParseColor(display, default_cms->cmap->id, named_colors[count],
	    	xcolors+count)) {
	    xv_error(NULL,
		 ERROR_STRING,
#ifdef OW_I18N
		 XV_I18N_MSG("xv_messages", "Unable to find RGB values for a named color"),
#else
		 "Unable to find RGB values for a named color",
#endif
		 ERROR_PKG, CMS,
		 0);
	    return(NULL);
	}
    }

    return(xcolors);
}

