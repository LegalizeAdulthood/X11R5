#define CompositeSimple
/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */

#include <X11/Xcu/RcP.h>

static void yyerror () ;
static int yywrap () ;
static int input () ;
static void unput () ;

/* Private Definitions */

XcuRcClassRec xcuRcClassRec =
{
  { /* core_class fields */
    /* superclass         */    (WidgetClass) &xcuTblClassRec,
    /* class_name         */    "Rc",
    /* widget_size        */    sizeof(XcuRcRec),
    /* class_initialize   */    NULL,
    /* class_part_init    */    NULL,
    /* class_inited       */    FALSE,
    /* initialize         */    NULL,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    NULL,
    /* num_resources      */    0,
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    TRUE,
    /* compress_exposure  */    TRUE,
    /* compress_enterleave*/    TRUE,
    /* visible_interest   */    FALSE,
    /* destroy            */    NULL,
    /* resize             */    XtInheritResize,
    /* expose             */    XtInheritExpose,
    /* set_values         */    NULL,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook    */    NULL,
    /* accept_focus       */    NULL,
    /* version            */    XtVersion,
    /* callback_private   */    NULL,
    /* tm_table           */    NULL,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	XtInheritDisplayAccelerator,
    /* extension	  */	NULL,
  },
  { /* composite_class fields */
    /* geometry_manager   */   XtInheritGeometryManager,
    /* change_managed     */   XtInheritChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension	  */   NULL,
  },
  { /* constraint_class fields */
    /* subresources       */   NULL,
    /* subresource_count  */   0,
    /* constraint_size    */   sizeof(XcuRcConstraintsRec),
    /* initialize         */   NULL,
    /* destroy            */   NULL,
    /* set_values         */   NULL
  },
#ifdef CompositeSimple
  {
    XtInheritChangeSensitive, /* Simple Class fields */
    XtInheritDrawFacets,
    XtInheritDrawHighlight
  },
#endif
  { /* tbl_class fields   */
    /* interpret_string   */   MyString
  },
  { /* rc_class fields    */
    /* empty  		  */   NULL
  }
} ;

WidgetClass xcuRcWidgetClass = (WidgetClass)&xcuRcClassRec ;

/***** **** *** ** * InterpretString * ** *** **** *****/

static char *rc_format_string ;
static char *output_format ;
static char mystring[200] ;

static String MyString (string)
    char *string ;
{
rc_format_string = string ;
output_format = mystring ;
yyparse () ;
return mystring ;
}

#include "Rc.y.c"

static void
yyerror (s)
    char *s ;
{
char *etext = "XcuRc: yacc parsing error\n" ;
XtError (etext) ;
}

static int
yywrap ()
{
return 1 ;
}

static int inbuffer[100] ;
static int inpointer = 0 ;

static int
input ()
{
int t ;
if (inpointer)
    {
    t = inbuffer[inpointer-1] ;
    inpointer-- ;
    }
else
    {
    t = *rc_format_string++ ;
    }

return t ;
}

static void
unput (c)
    int c ;
{
inbuffer[inpointer++] = c ;
return ;
}

