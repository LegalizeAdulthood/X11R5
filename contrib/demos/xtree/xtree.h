/*
 * $XConsortium: xtree.h,v 1.1 90/05/04 15:15:49 jim Exp $
 *
 * Copyright 1990 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Cardinals.h>

extern char *ProgramName;		/* for error messages */

extern XtAppContext app_con;		/* our Application Context */
extern Widget toplevel;			/* master widget */

typedef struct _node {
    char *name;				/* name to be displayed */
    struct _node *parent;		/* parent of this node */
    struct _node *children;		/* list of children to chain */
    struct _node *siblings;		/* list of siblings to chain */
} Node;

extern Node rootnode;			/* invisible root of tree */

typedef int Format;
#define FORMAT_NAME 0
#define FORMAT_RESOURCE 1
#define FORMAT_NUMBER 2
#define FORMAT_SEPARATOR 3

#define XtRDataFormat "DataFormat"

typedef struct {
    char *separator;			/* indicates level */
    Format input_format;		/* way in which input is read */
    Format output_format;		/* which in which output is printed */
    int number_origin;			/* number format origin (0,1) */
} AppResources;

extern AppResources GlobalResources;	/* global resource data */

extern void create_ui ();		/* create forms, trees, etc. */
extern void read_data ();		/* read data from stream */
extern void build_tree ();		/* put data into tree */
extern void adjust_panner ();		/* fix panner size and location */
extern void circulate_panner_tree ();	/* circulate windows */
extern void cvt_data_format ();		/* string to dataformat */
