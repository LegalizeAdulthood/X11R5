#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)xv_usage.c 50.1 90/12/12";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>

/*
 * Print usage message to stderr
 */
void
xv_usage(name)
    char           *name;
{
    (void) fprintf(stderr, "usage of %s generic window arguments:\n\
FLAG\t(LONG FLAG)\t\tARGS\t\tNOTES\n\
-Ww\t(-width)\t\tcolumns\n\
-Wh\t(-height)\t\tlines\n\
-Ws\t(-size)\t\t\tx y\n\
-Wp\t(-position)\t\tx y\n\
\t(-geometry)\t\"[WxH][{+|-}X{+|-}Y]\"\t(X geometry)\n\
-WP\t(-icon_position)\tx y\n\
-Wl\t(-label)\t\t\"string\"\n\
\t(-title)\t\t\"string\"\t(Same as -label)\n\
-Wi\t(-iconic)\t\t(Application will come up closed)\n\
+Wi\t(+iconic)\t\t(Application will come up open)\n\
-Wt\t(-font)\t\t\tfontname\n\
-fn\t\t\t\tfontname\t\n\
-Wx\t(-scale)\t\tsmall | medium | large | extra_large\n\
-Wf\t(-foreground_color)\tred green blue\t0-255 (no color-full color)\n\
-fg\t(-foreground)\t\tcolorname\t(X Color specification)\n\
-Wb\t(-background_color)\tred green blue\t0-255 (no color-full color)\n\
-bg\t(-background)\t\tcolorname\t(X Color specification)\n\
-WI\t(-icon_image)\t\tfilename\n\
-WL\t(-icon_label)\t\t\"string\"\n\
-WT\t(-icon_font)\t\tfilename\n\
-Wr\t(-display)\t\t\"server_name:screen\"\n\
-Wdr\t(-disable_retained)\t\n\
-Wdxio\t(-disable_xio_error_handler)\t\n\
-Wfsdb\t(-fullscreendebug)\t\n\
-Wfsdbs\t(-fullscreendebugserver)\t\n\
-Wfsdbp\t(-fullscreendebugptr)\t\n\
-Wfsdbk\t(-fullscreendebugkbd)\t\n\
-WS\t(-defeateventsecurity)\t\n\
-sync\t(-synchronous)\t\t\t\t(Force a synchronous connection)\n\
+sync\t(+synchronous)\t\t\t\t(Make an asynchronous connection)\n\
-Wd\t(-default)\t\tresource value\t(Set the X resource to value)\n\
-xrm\t\t\t\tresource:value\t(Set the X resource to value)\n\
-WH\t(-help)\t\n",
		   name ? name : "");

    exit(97);
}
