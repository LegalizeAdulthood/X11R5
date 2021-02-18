/* -*-C-*-
********************************************************************************
*
* File:         w_pixmap.c
* RCS:          $Header: w_pixmap.c,v 1.3 91/03/14 03:13:50 mayer Exp $
* Description:  Interfaces to Motif's Pixmap/XImage routines.
* Author:       Niels Mayer, HPLabs
* Created:      Thu Sep 28 18:59:42 1989
* Modified:     Thu Oct  3 20:56:05 1991 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r5 contrib tape release
*
* WINTERP Copyright 1989, 1990, 1991 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_pixmap.c,v 1.3 91/03/14 03:13:50 mayer Exp $";

#include <stdio.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

/* 

Destroying unused Pixmaps and XImage's:

Pixmaps are used by widgets to display graphical information. By keeping
track of the widget and the resource under which the pixmap is stored, we
can figure out when to free up pixmaps by using XmDestroyPixmap() to update
the reference count on pixmaps used in winterp and free the memory for
pixmaps that are no longer referenced. This is certainly useful and good to
do because I expect pixmaps to take up a reasonable amount of client and
server space, so we should be efficient about their use, especially because
motif has made it easy to do so. 

Furthermore, we don't want users manually calling XmDestroyPixmap() on a
pixmap that is still referenced -- this will cause a guaranteed coredump
due to an "X Protocol Error". 

Unfortunately, the XImage* behind the pixmap still seems to persist even if
all pixmaps based on a particular Ximage no longer exists. Since XImages
can be created inside XmGetPixmap(), or through a XmSttring-->Xm*Pixmap*
conversion it will be very hard to get a hold of the actual XImage pointer
when all we have access to are the name-strings of the previously installed
images or the filenames of new images installed. Either we'll have to
prevent creating cached XImages inside XmGetPixmap() and the
XmSttring-->Xm*Pixmap* converters, and do all this ourselves, or we can
just punt. I'm going to punt on this.

The alternative for handling XImages is to let the users that care about
space taken up by XImages to use
XmInstallImage(_XmGetImageFromFile(<image_file_name>)) at application
startup time, and then destroy the <XImage_obj> themselves when they're not
needed.  Destroying an XImage after creating the necessary pixmaps from it
shouldn't cause any protocol errors, so this is the thing to do.

Every time a Pixmap is accessed either through XmGetPixmap() or through
winterp's string-->pixmap converter (which ends up calling XmGetPixmap()),
the reference count on that pixmap is automatically increased. We want to
symetrically reduce the reference count (by calling XmDestroyPixmap()) for
each pixmap reference that is no longer used. When the reference count is
reduced to 0, the pixmap itself is freed. We do this by creating an atomic
type XLTYPE_Pixmap to store the X Pixmap pointer; every reference to this
pixmap will create a pixmap reference object which points to this
XLTYPE_Pixmap object. Code in this module will keep track of the pixmap
references made, and will remove any pixmap-ref-objs from v_savedobjs when
the pixmap is no longer referenced. If no more references to the
XLTYPE_Pixmap exist, then it will be garbage collected -- code inside the
garbage collector will call XmDestroyPixmap() on these unused pixmaps.

In each pixmap-reference object, we store
1) a pointer to an object of type LVAL_Pixmap which is returned by
   primitive xm_get_pixmap.
2) name of widget resource storing the pixmap (the winterp symbol for the res)
3) the widget-object of the pixmap.

We remove pixmap-ref-objs from v_savedobjs when
1) the widget which references the pixmap gets destroyed
2) the widget resource that references the pixmap gets set to a new value
3) the pixmap is no longer referenced within the lisp system. (see note below)

In case (1) we want to hash the pixmap into the widget's v_savedobjs
hashbucket. When the widget's destroy callback gets called, all the objects
in the hashbucket corresonding to that widget are destroyed (callbacks,
pixmaps, etc).

In case (2), since we use the scheme mentioned above in (1) to store
pixmap-objs, we can just search thru the hashbucket looking for the
pixmap-obj on the approprate widget and widget-resource symbol. If we
find a pixmap-obj, remove it from the list and replace it with the new one.

In case (3), when no pixmap-ref-objs pointing to a particular XLTYPE_Pixmap
exist, that XLTYPE_Pixmap will get garbage collected and it's pixmap will
be deallocated by calling XmDestroyPixmap(). Note that if XmGetPixmap is
called N times on a particular pixmap, we create N XLTYPE_Pixmap objects
corresponding to the N refcount on that Pixmap stored inside Motif. Each
time the XLTYPE_Pixmap gets gc'd, the refcount gets reduced inside Motif.

The only other way a pixmap can be referenced is via XmGetPixmap() or
:get_values on a pixmap resource, in which case it's XmDestroyPimap()
operation should only happen when that pixmap was no longer referenced
neither in lisp variables nor as pixmap resources. To do this, we create a
new XLTYPE_Pixmap for each reference.

*/

/******************************************************************************
 *
 ******************************************************************************/
void Wpm_Set_Pixmap_Reference(lval_pixmap, o_widget, lval_resname)
     LVAL lval_pixmap;		/* XLTYPE_Pixmap */
     LVAL o_widget;		/* widget on which this resource got set */
     LVAL lval_resname;		/* SYMBOL */
{
  extern LVAL v_savedobjs;	/* w_savedobjs.c */
  int i = Wso_Hash(o_widget);	/* note that we hash all pixmapobjs on the same widget to the same hashbucket */
  LVAL l_hbucket = getelement(v_savedobjs, i);
  LVAL obj, refobj, l_prev = NIL;

#ifdef DEBUG
  fprintf(stderr, "\nWpm_Set_Pixmap_Reference(PixmapID=%lu, WidgetID=%lu, ResnameID=%lu).\n", (unsigned long) get_pixmap(lval_pixmap), (unsigned long) o_widget, (unsigned long) lval_resname);
#endif

  /* go thru hashbucket, stopping if hit end, or if hit match */
  while (l_hbucket
	 && !((obj = car(l_hbucket))
	      && (ntype(obj) == XLTYPE_PIXMAP_REFOBJ)
	      && (get_pixref_widget(obj) == o_widget)
	      && (get_pixref_resname(obj) == lval_resname))) {
	 l_prev = l_hbucket;
	 l_hbucket = cdr(l_hbucket);
       }

  /* remove the previously referenced pixmapobj --> allow it to be gc'd */
  if (l_hbucket) {		/* if something matched */
#ifdef DEBUG
    fprintf(stderr, "	Removing PixmapRef w/ PixmapID=%lu, WidgetID=%lu, ResnameID=%lu.\n", (unsigned long) get_pixmap(get_pixref_pixmap(car(l_hbucket))), (unsigned long) get_pixref_widget(car(l_hbucket)), (unsigned long) get_pixref_resname(car(l_hbucket)));
#endif
    if (!l_prev)		/* first elt matched */
      setelement(v_savedobjs, i, cdr(l_hbucket)); /* remove first elt */
    else
      rplacd(l_prev, cdr(l_hbucket)); /* remove elt pointed to by l_hbucket */
  }
#ifdef DEBUG
  else
    fprintf(stderr, "	Didn't remove any previous PixmapRef's\n");
#endif

  /* save a new reference pixmap-obj under o_widget and lval_resname. */
  xlstkcheck(3);
  xlprotect(lval_pixmap);
  xlsave(l_hbucket);
  xlsave(refobj);

  refobj = new_pixrefobj();	/* create a "reference object" */
  set_pixref_pixmap(refobj, lval_pixmap); /* point it to the pixmap it references */
  set_pixref_widget(refobj, o_widget); /* set the widget referencing the pixmap */
  set_pixref_resname(refobj, lval_resname); /* set the resource name on that widget */
  l_hbucket = cons(refobj, getelement(v_savedobjs, i));	/* add reference obj to hashbucket */
  setelement(v_savedobjs, i, l_hbucket); /* store it in in v_savedobjs so it won't get gc'd */
  xlpopn(3);
}


/******************************************************************************
 * This routine is called by the garbage collector on any unreferenced pixmap
 * nodes. Note that XmDestroyPixmap() gets called on Pixmaps created by
 * was created by calling primitive XM_GET_PIXMAP or via an indirect XtConvert()
 * call in Wres_Append_LispArglist_To_XtArglist() (which also calls XmGetPixmap())
 * This is done because we must ensure that we only call XmDestroyPixmap()
 * once per invocation of XmGetPixmap() lest a pixmap be deallocated while it
 * is still ref'd inside Motif.
 ******************************************************************************/
void Wpm_Decr_Refcount_Or_Free_Pixmap(p)
     LVAL p;			/* XLTYPE_Pixmap */
{
  extern Screen*  screen;	/* global in winterp.c */

#ifdef DEBUG
  fprintf(stderr, "\nWpm_Decr_Refcount_Or_Free_Pixmap(PixmapID=%lu).\n", (unsigned long) get_pixmap(p));
#endif

  if (!XmDestroyPixmap(screen, get_pixmap(p)))
    xlerror("Internal error in garbage collecting a pixmap -- XmDestroyPixmap() couldn't find pixmap in pixmap cache.",
	    p);
}


/******************************************************************************
 * (XM_GET_PIXMAP <image-name> <foreground> <background>)
 *
 * <image-name> is a string representing a XImage that has been
 * cached via XmInstallImage(). If such an XImage isn't found, then
 * <image-name> is treated as the filename for a X10 or X11 bitmap file.
 * You need to give a full pathname to the bitmap file, or alternately,
 * you may set the environment variable XBMLANGPATH to the directories to
 * be searched for bitmap files. XBMLANGPATH defaults to
 * /usr/lib/X11/%L/bitmaps/%N/%B:/usr/lib/X11/%L/bitmaps/%B:/usr/lib/X11/bitmaps/%B:/usr/include/X11/bitmaps/%B
 *
 * In additions to images you have installed, Motif features 10 preinstalled
 * images: "background", "25_foreground", "50_foreground", * "75_foreground",
 * "vertical", "horizontal", "slant_right", "slant_left", "menu_cascade", "menu_checkmark".
 *
 * <forground> and <background> may be strings repreenting color names, or
 * values of type XLTYPE_Pixel. (generated by X_ALLOC_COLOR, or
 * via widget-method :get_values :XMN_FOREGROUND or :XMN_BACKGROUND.
 *
 * This function returns a value of type XLTYPE_Pixmap, which is suitable for
 * passing on to any Pixmap-valued widget resource so as to display an image
 * inside a widget.
 *
 * Note that there is no interface to XmDestroyPixmap()-- unreferenced pixmaps
 * are automatically destroyed by winterp during garbage collection.
 ******************************************************************************/
LVAL Wpm_Prim_XmGetPixmap()
{
  extern Display* display;	/* global in winterp.c */
  extern Screen*  screen;	/* global in winterp.c */
  extern Colormap colormap;	/* global in winterp.c */
  XColor        screenColor;
  LVAL          str_image_name, lval_foreground, lval_background;
  Pixel         foreground, background;
  Pixmap        pixmap;

  str_image_name = xlgastring();

  lval_foreground = xlgetarg();
  if (stringp(lval_foreground)) {
    if (!XParseColor(display, colormap, (char*) getstring(lval_foreground), &screenColor))
      xlerror("XParseColor() couldn't parse <foreground> color specification.", lval_foreground);
    if (!XAllocColor(display, colormap, &screenColor))
      xlerror("XAllocColor() couldn't allocate specified <foreground> color.", lval_foreground);
    foreground = screenColor.pixel;
  }
  else if (pixel_p(lval_foreground))
    foreground = get_pixel(lval_foreground);
  else
    xlerror("Bad type for <foreground> argument. Expected either a STRING or a PIXEL value.", lval_foreground);

  lval_background = xlgetarg();
  if (stringp(lval_background)) {
    if (!XParseColor(display, colormap, (char*) getstring(lval_background), &screenColor))
      xlerror("XParseColor() couldn't parse <background> color specification.", lval_background);
    if (!XAllocColor(display, colormap, &screenColor))
      xlerror("XAllocColor() couldn't allocate specified <background> color.", lval_background);
    background = screenColor.pixel;
  }
  else if (pixel_p(lval_background))
    background = get_pixel(lval_background);
  else
    xlerror("Bad type for <background> argument. Expected either a STRING or a PIXEL value.", lval_background);

  xllastarg();

  pixmap = XmGetPixmap(screen, (char*) getstring(str_image_name), foreground, background);
  if (pixmap == XmUNSPECIFIED_PIXMAP)
    xlerror("XmGetPixmap() couldn't create a pixmap from given specification.", str_image_name);

#ifdef DEBUG
  fprintf(stderr, "\nXmGetPixmap() returned PixmapID=%lu.\n", (unsigned long) pixmap);
#endif

  return (cv_pixmap(pixmap));
}


/******************************************************************************
 * (xm_install_image <ximage> <image_name>)
 * where <ximage> is an XImage-type object as returned by XM_GET_XIMAGE_FROM_FILE
 * <image_name> is a string, the name under which the XImage is cached
 *
 * returns T if success,  NIL if a NULL <ximage>, or duplicate <image_name>
 * is given.
 ******************************************************************************/
LVAL Wpm_Prim_XmInstallImage()
{
#ifndef WINTERP_MOTIF_11	/* In 1.0 only, this is missing from Xm.h */
  extern Boolean XmInstallImage(); /* lib/Xm/ImageCache.c */
#endif				/* WINTERP_MOTIF_11 */
  extern LVAL true;		/* xlglob.c */
  XImage* image = get_ximage(xlga_ximage());
  char*   name = (char*) getstring(xlgastring());
  xllastarg();
  if (XmInstallImage(image, name)) /* XmInstallImage() makes a local copy of <name>, the <name> argument can thus be safely freed via garbage collection */
    return (true);
  else
    return (NIL);
}


/******************************************************************************
 * (xm_uninstall_image <ximage>)
 * where <ximage> is an XImage-type object as returned by XM_GET_XIMAGE_FROM_FILE
 *
 * returns T if success,  NIL if a NULL <ximage>, or <ximage> cannot be
 * found in the image cache.
 ******************************************************************************/
LVAL Wpm_Prim_XmUninstallImage()
{
#ifndef WINTERP_MOTIF_11	/* In 1.0 only, this is missing from Xm.h */
  extern Boolean XmUninstallImage(); /* lib/Xm/ImageCache.c */
#endif				/* WINTERP_MOTIF_11 */
  extern LVAL true;		/* xlglob.c */
  XImage* image = get_ximage(xlga_ximage());
  xllastarg();
  if (XmUninstallImage(image))
    return (true);
  else
    return (NIL);
}


/******************************************************************************
 * (xm_get_ximage_from_file <filepath>)
 * where <filepath> is the full name of the file containing the XImage data.
 *
 * returns an XImage object if an XImage was successfully retrieved from the
 * given file; NIL if failure.
 ******************************************************************************/
LVAL Wpm_Prim_XmGetImageFromFile()
{
  XImage* image;
  extern XImage* _XmGetImageFromFile(); /* lib/Xm/ReadImage.c */
  char* filename = (char*) getstring(xlgastring());
  xllastarg();
  if (image = _XmGetImageFromFile(filename))
    return (cv_ximage(image));
  else
    return (NIL);
}

