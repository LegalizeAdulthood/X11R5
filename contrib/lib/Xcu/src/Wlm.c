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

/*
 * XcuWlm widget
 */

#include <stdio.h> /* need this for FILE */
#include <X11/Xos.h>
#include <sys/stat.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xresource.h>
#include <X11/Xcu/Bmgr.h>
#include <X11/Xcu/Button.h>
#include <X11/Xcu/ButtonP.h>
#include <X11/Xcu/WlmP.h>

void Xcu_copy_ds () ;

/* Private Definitions */

static char error_text[200] ;

/*
 * resource declarations
 */

#define Offset(field) XtOffset(XcuWlmWidget, wlm.field)

static XtResource resources[] =
{
   {XtNfile,      XtCFile,   XtRString,  sizeof(String),
    Offset(layout_file), XtRString,	(XPointer) NULL}
  ,{XtNexposeCallback, XtCCallback, XtRCallback, sizeof(XPointer), 
    Offset(expose_callbacks), XtRCallback, (XPointer)NULL}
} ;

#undef Offset

/* Declarations of "toolkit" calls */

static XtGeometryResult	GeometryManager () ;
static void		ChangeManaged () ;
static void		ClassInitialize () ;
static void		Initialize () ;
static Boolean		SetValues () ;
static void		Resize () ;

/* Declarations of "private" calls */

static void	ReadLayout () ;
static void	read_layout () ;
static void	InitializeLayout () ;
extern void	make_tag_class_list () ;
static void	realize_child () ;
static void	bmgr_setup () ;
static void	bmgr_search () ;
static void	directive_setup () ;
static void	directive_search () ;
static void	wlm_central () ;
static void	add_to_widget_list () ;
static void	add_to_sample_list () ;
static void	CvtStringToWidget () ;
static void	CvtStringToProcedure () ;
static void	AddStringToWidgetConverter () ;
static void	AddStringToProcedureConverter () ;
static void	find_public_procedures () ;
static void	callback_not_found_warning () ;
static void	callback_scan () ;
static void	get_int_float_string () ;
static void	callback_set_bool () ;
static void	callback_set_float () ;
static void	callback_set_int () ;
static void	callback_set_string () ;
static void	callback_get () ;
static void	callback_indirect () ;
static void	callback_channel () ;
static void	dequeue_event () ;
static void	enqueue_event () ;
static void	create_resource_database () ;
static void	ParseXcuWlmLayout () ;
static void	get_child () ;
static void	get_manage () ;
static void	get_resource () ;
static void	get_directive () ;
static int	get_int() ;
static String	get_word() ;
static String	get_string() ;
static void	my_app_sync() ;
static void	get_resource_path() ;

static XcuWlmArgType	convert_string_to_arg () ;
static WidgetClassList *get_widget_class_list () ;
static WidgetNameList  *get_widget_name_list () ;
static WidgetIdList    *get_widget_id_list () ;
static Widget		get_widget_id () ;
static Widget		get_callback_widget () ;
static XtProc		get_proc_list () ;
static WidgetClass	get_widget_class () ;
static String		get_resource_info () ;

/*
 * Class Record Constant
 */

XcuWlmClassRec xcuWlmClassRec =
{
  { /* core_class fields  */
    /* superclass         */    (WidgetClass) &compositeClassRec,
    /* class_name         */    "Wlm",
    /* widget_size        */    sizeof(XcuWlmRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    NULL,
    /* class_inited       */    False,
    /* initialize         */    Initialize,
    /* initialize_hook    */    NULL,
    /* realize            */    XtInheritRealize,
    /* actions            */    NULL,
    /* num_actions        */    0,
    /* resources          */    resources,
    /* num_resources      */    XtNumber(resources),
    /* xrm_class          */    NULLQUARK,
    /* compress_motion    */    True,
    /* compress_exposure  */    True,
    /* compress_enterleave*/    True,
    /* visible_interest   */    False,
    /* destroy            */    NULL,
    /* resize             */    Resize,
    /* expose             */    NULL,
    /* set_values         */    SetValues,
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
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   XtInheritInsertChild,
    /* delete_child       */   XtInheritDeleteChild,
    /* extension	  */   NULL,
  },
  { /* wlm_class fields   */
    /* no entries   */   NULL
  }
} ;

/*
 * Class Record Pointer
 */

WidgetClass xcuWlmWidgetClass = (WidgetClass) &xcuWlmClassRec ;

/**
 ***
 **** Toolkit Callable Procedures
 ***
 **/

static XrmQuark Bmgr_quark ;

/***** **** *** ** * ClassInitialize * ** *** **** *****/

static void
ClassInitialize ()
{
Bmgr_quark = XrmStringToQuark ("XcuBmgr") ; /* one time initialization */

AddStringToProcedureConverter () ; /* add a couple resource converters	  */
XcuAddStringToBmgrTypeConverter () ;
XcuAddStringToFloatConverter () ;
return ;
}

/***** **** *** ** * Initialize * ** *** **** *****/

static void
Initialize (request, new)
    Widget request, new ;
{
XcuWlmWidget ww = (XcuWlmWidget) new ;

/*
 * allocate space for and copy resources specified by address
 */

if (ww->wlm.layout_file != NULL) 
    Xcu_copy_ds (&ww->wlm.layout_file, ww->wlm.layout_file) ;

/*
 * Initialize some private non-resource fields
 */

ww->wlm.sre_status = NULL ;
ww->wlm.sampling_active = False ;
ww->wlm.first_device = True ;
ww->wlm.kill_request = False ;
ww->wlm.database = NULL ;
ww->wlm.history_ptr = NULL ;
ww->wlm.history_mode = XcuWlmInactiveHistory ;
ww->wlm.queue_ptr = NULL ;
ww->wlm.sample_list = NULL ;
ww->wlm.widget_class_list = NULL ;
ww->wlm.tag_class_list = NULL ;
ww->wlm.event_stamp = 0 ;
ww->wlm.parent_depth = 0 ;
ww->core.border_width = 0 ;
ww->core.width = 1 ;
ww->core.height = 1 ;

ReadLayout (ww) ;		   /* read the layout			  */
make_tag_class_list (ww) ;	   /* construct a correspondence table of */
				   /* widget class strings and pointers	  */
				   /* (client supplies this routine)	  */
InitializeLayout (ww) ;		   /* analyze the layout		  */

return ;
}
/***** **** *** ** * SetValues * ** *** **** *****/

static Boolean
SetValues (current, request, new)
    Widget current, request, new ;
{
XcuWlmWidget cw = (XcuWlmWidget) current ;
XcuWlmWidget nw = (XcuWlmWidget) new ;

if (cw->wlm.layout_file != nw->wlm.layout_file)
    { /* TODO if ever needed */ }

return False ;
}

/***** **** *** ** * ChangeManaged * ** *** **** *****/

/*
 * ChangeManaged is called when there is a change in the set of children
 */

static void
ChangeManaged (w)
    Widget w ;
{
Cardinal i ;
Dimension large_width = 0 ;
Dimension large_height = 0 ;
XcuWlmWidget ww = (XcuWlmWidget) w ;
/*
 * We want our window to be the same size as our largest kid...
 */

for (i=0;  i < ww->composite.num_children;  i++)
    {
    XtWidgetGeometry preferred, request ;
    request.request_mode = (CWWidth | CWHeight) ;
    XtQueryGeometry (ww->composite.children[i], &request, &preferred) ;
    if (preferred.width + 2*preferred.border_width > large_width)
	large_width = preferred.width + 2*preferred.border_width ;
    if (preferred.height + 2*preferred.border_width > large_height)
	large_height = preferred.height + 2*preferred.border_width ;
    }

ww->core.width = large_width ;
ww->core.height = large_height ;

return ;
}

/***** **** *** ** * GeometryManager * ** *** **** *****/

static XtGeometryResult
GeometryManager (w, req, ret)
    Widget w ;
    XtWidgetGeometry *req ;
    XtWidgetGeometry *ret ;
{
XtGeometryResult result ;
XcuWlmWidget ww = (XcuWlmWidget) w->core.parent ;

/*
 * Since this is only a container class, just rubber stamp all requests...
 */

result = XtMakeGeometryRequest ((Widget)ww, req, ret) ;
if (result == XtGeometryYes)
    {
    if (!(req->request_mode & XtCWQueryOnly))
	{
	w->core.width = req->width ;
	w->core.height = req->height ;
	}
    }
else
if (result == XtGeometryAlmost)
    {
    /* We'll accept whatever compromise is offered */
    XtMakeGeometryRequest ((Widget)ww, ret, (XtWidgetGeometry *)NULL) ;
    if (!(req->request_mode & XtCWQueryOnly))
	{
	w->core.width = ret->width ;
	w->core.height = ret->height ;
	}
    }

return result ;
}

/***** **** *** ** * Resize * ** *** **** *****/

static void
Resize (w)
    Widget w ;
{
Cardinal i ;
XcuWlmWidget ww = (XcuWlmWidget) w ;
/*
 * Inform the children of the resize
 */

for (i=0;  i < ww->composite.num_children;  i++)
    {
    if (!XtIsSubclass (ww->composite.children[i], xcuBmgrWidgetClass))
      XtResizeWidget (ww->composite.children[i], w->core.width, w->core.height,
		      ww->composite.children[i]->core.border_width) ;
    }
return ;
}

/**
 ***
 **** Private Procedures
 ***
 **/

#define WLM_MAX_UNPUT 100
#define WLM_MAX_CONTEXT 100

static String undefined_name = "wlm_undef" ;
static Cardinal counter = 0 ;

typedef struct
    {
    String input_buffer ;
    Cardinal input_pointer ;
    char unput_buffer[WLM_MAX_UNPUT] ;
    Cardinal unput_pointer ;
    char context_buffer[WLM_MAX_CONTEXT] ;
    int context_ptr ; /* must not be Cardinal */
    Cardinal line_number ;
    String cur_file ;
    String file0 ;
    } XcuWlmInputRecord ;

static XcuWlmInputRecord *layout_input ;

/***** **** *** ** * ReadLayout * ** *** **** *****/

static void
ReadLayout (ww)
    XcuWlmWidget ww ;
{
char *resource_path = NULL ;
if (ww->wlm.layout_file == NULL) 
    {
    XtError ("XcuWlm: No layout file!\n") ;
    }
/*
 * Layout is contained in a file.
 * We'll read the file as one big string, then parse it.
 */

layout_input = (XcuWlmInputRecord *) XtMalloc (sizeof (XcuWlmInputRecord)) ;
read_layout (ww->wlm.layout_file, layout_input) ;
XtFree (ww->wlm.layout_file) ;
ww->wlm.layout_file = NULL ;

ParseXcuWlmLayout (ww) ;

/*
 * Now that we know how deep the layout is, we can allocate some space
 * for resource list entries.
 * Really do need 2 extra allocations!
 * (1 for NULL entry, and 1 for something else I can't remember right now...)
 */

get_resource_path (ww, &resource_path) ;

if (resource_path)
    {
    char *p ;
    for (p=resource_path;  *p != NULL;  p++)
	if (*p == '.' | *p == '*')
	    ww->wlm.parent_depth++ ;
    }

ww->wlm.quarks_list = (XrmQuark *) XtMalloc
    ((ww->wlm.max_depth + ww->wlm.parent_depth + 2) * sizeof (XrmQuark)) ;
ww->wlm.bindings_list = (XrmBinding *) XtMalloc
    ((ww->wlm.max_depth + ww->wlm.parent_depth + 2) * sizeof (XrmBinding)) ;

if (resource_path)
    XrmStringToBindingQuarkList (resource_path, ww->wlm.bindings_list,
						ww->wlm.quarks_list);

return ;
}

static void
get_resource_path (ww, path)
    XcuWlmWidget ww ;
    char **path ;
{
char *old = NULL ;
char *curr = NULL ;
Widget p ;

for (p=ww->core.parent; p!=NULL; p=p->core.parent)
    {
    char *text = p->core.name ?
		 p->core.name : XtClass(p)->core_class.class_name ;
    int old_length = (old == NULL) ? 0 : strlen(old) ;
    curr = XtMalloc (strlen(text) + old_length + 2) ;
    sprintf (curr, ".%s%s", text, old ? old : "") ;
    if (old)
	XtFree (old) ;
    old = curr ;
    }

*old = '.' ;
*path = XtMalloc (strlen(old)+1) ;
strcpy (*path, old) ;
return ;
}

/***** **** *** ** * read_layout * ** *** **** *****/

static void
read_layout (file, layout_input)
    String file ;
    XcuWlmInputRecord *layout_input ;
{
FILE *fd ;
struct stat buf ;
String input_file ;
int argv_length ;

argv_length = strlen(file) ;
if (strcmp(&file[argv_length-3], ".wc") == 0)
    {
    /* This permits extensioned or extensionless input */
    file[argv_length-3] = '\0' ;
    argv_length -= 3 ;
    }
input_file = XtMalloc (argv_length + 4) ;
sprintf (input_file, "%s.wc", file) ;

fd = fopen (input_file, "r") ;
if ((int)fd <= 0)
    {
    char *my_text = "XcuWlm: Couldn't open (%s) for reading\n" ;
    char *etext = XtMalloc (strlen(my_text) + strlen(input_file)) ;
    sprintf (etext, my_text, input_file) ;
    XtError (etext) ;
    }

/*
 * Find out how big the layout file is.
 * Read it in as one big chunk.
 * Initialize some useful variables
 */

fstat (fileno(fd), &buf) ;
layout_input->input_buffer = XtMalloc (buf.st_size + 1) ;
fread (layout_input->input_buffer, (int)buf.st_size, 1, fd) ;
layout_input->input_buffer[buf.st_size] = '\0' ;

Xcu_copy_ds (&layout_input->cur_file, input_file) ; /* for error messages */
Xcu_copy_ds (&layout_input->file0, input_file) ; /* for error messages */
layout_input->input_pointer = 0 ;
layout_input->unput_pointer = 0 ;
layout_input->line_number = 1 ;
fclose (fd) ;

return ;
}

/***** **** *** ** * InitializeLayout * ** *** **** *****/

static void
InitializeLayout (ww)
    XcuWlmWidget ww ;
{
Display *dpy = XtDisplay ((Widget)ww) ;
XrmDatabase XtDefaultDB = XtDatabase(dpy) ;
/*
 * Having parsed the grammar, we now convert the fetuses into children!
 * This involves numerous  passes through the child list.
 * We'll start by extracting all resource information and inserting it into
 * the default resource database.
 */

ww->wlm.cur_depth = ww->wlm.parent_depth ;
create_resource_database (ww, ww->wlm.child, True) ;
if (XtDefaultDB == NULL)
    {
    XtDefaultDB = ww->wlm.database ;
    dpy->db = ww->wlm.database ;
    }
else
    XrmMergeDatabases (ww->wlm.database, &XtDefaultDB) ;

/*
 * Create the widget tree...
 */

realize_child (ww, (Widget)ww, ww->wlm.child, True) ;

/*
 * Now that there exists a list of the widgets in the layout,
 * create converters for the name strings.
 */

AddStringToWidgetConverter (ww) ;

/*
 * Look through the list again, this time creating the Bmgr widgets.
 */

bmgr_search (ww, ww->wlm.child) ;

/*
 * Find all the public procedures of the widget classes in the layout
 */

find_public_procedures (ww) ;

/*
 * Once through the list again to find all the directives
 */

directive_search (ww, ww->wlm.child) ;

return ;
}

/***** **** *** ** * create_resource_database * ** *** **** *****/

static void
create_resource_database (ww, child, root_wlm)
    XcuWlmWidget ww ;
    Fetus *child ;
    Boolean root_wlm ;
{
int	 i ; /* must not be Cardinal */
/* Compiler may complain about these being used before set, but it's bogus */
XrmQuark save_quark1, save_quark2, save_quark3 ;
XrmBinding save_binding1, save_binding2, save_binding3 ;
Cardinal save_depth ;

if (child->class_name == Bmgr_quark)
    {
    /*
     * Treat BmgrWidgets specially (will be children of root_wlm)
     * Save the resource list entries that will be demolished.
     */
    save_binding1 = ww->wlm.bindings_list[ww->wlm.parent_depth+1] ;
    save_binding2 = ww->wlm.bindings_list[ww->wlm.parent_depth+2] ;
    save_binding3 = ww->wlm.bindings_list[ww->wlm.parent_depth+3] ;
    save_quark1 = ww->wlm.quarks_list[ww->wlm.parent_depth+1] ;
    save_quark2 = ww->wlm.quarks_list[ww->wlm.parent_depth+2] ;
    save_quark3 = ww->wlm.quarks_list[ww->wlm.parent_depth+3] ;
    save_depth = ww->wlm.cur_depth ;
    ww->wlm.cur_depth = ww->wlm.parent_depth+1 ;
    }

/*
 * Find out if this widget has a name.
 * If not, assign it one.
 */

child->class = get_widget_class (ww, child->class_name) ;

for (i=0;  i < child->n_resources;  i++)
    if (strcmp (child->resources[i].name, "name") == 0)
	child->widget_name = child->resources[i].value ;

if (child->widget_name == NULL)
    {
    String new_name = (String) XtMalloc (strlen(undefined_name)+10) ;
    sprintf (new_name, "%s%d", undefined_name, counter) ;
    child->widget_name = new_name ;
    counter++ ;
    }

/*
 * Enter all specified resources in the database, except callbacks and names
 */  

ww->wlm.bindings_list[ww->wlm.cur_depth] = root_wlm ?
	XrmBindLoosely :
	XrmBindTightly ;

ww->wlm.quarks_list[ww->wlm.cur_depth] = root_wlm ?
	XrmStringToQuark (ww->core.name) :
	XrmStringToQuark (child->widget_name) ;

for (i=0;  i < child->n_resources;  i++)
    {
    Resource *resource = &child->resources[i] ;
    resource->class = get_resource_info (child->class, resource->name,
					 XcuWlmResourceInfoClass) ;
    if ((!resource->name || strcmp (resource->name, "name") != 0) &&
        (!resource->class || strcmp (resource->class, "Callback") != 0))
	{
	if (resource->context == XcuWlmLocalContext)
	    {
	    ww->wlm.bindings_list[ww->wlm.cur_depth+1] = XrmBindTightly ;
	    ww->wlm.quarks_list[ww->wlm.cur_depth+1] =
		    XrmStringToQuark (resource->name) ;
	    ww->wlm.quarks_list[ww->wlm.cur_depth+2] = NULLQUARK ;
	    }
	else
	    {
	    XrmStringToBindingQuarkList (resource->name,
		&ww->wlm.bindings_list[ww->wlm.cur_depth+1],
		&ww->wlm.quarks_list[ww->wlm.cur_depth+1]) ;
	    ww->wlm.bindings_list[ww->wlm.cur_depth+1] = XrmBindLoosely ;
	    }
	XrmQPutStringResource (&ww->wlm.database,
				ww->wlm.bindings_list,
				ww->wlm.quarks_list,
				resource->value) ;
	}
    }

if (child->class_name == Bmgr_quark)
    {
    ww->wlm.bindings_list[ww->wlm.parent_depth+1] = save_binding1 ;
    ww->wlm.bindings_list[ww->wlm.parent_depth+2] = save_binding2 ;
    ww->wlm.bindings_list[ww->wlm.parent_depth+3] = save_binding3 ;
    ww->wlm.quarks_list[ww->wlm.parent_depth+1] = save_quark1 ;
    ww->wlm.quarks_list[ww->wlm.parent_depth+2] = save_quark2 ;
    ww->wlm.quarks_list[ww->wlm.parent_depth+3] = save_quark3 ;
    ww->wlm.cur_depth = save_depth ;
    }

if (child->class_name != Bmgr_quark)
    ww->wlm.cur_depth++ ;

/* descend the widget tree */
for (i = child->n_children - 1;  i >=0;  i--)
    create_resource_database (ww, &child->children[i], False) ;

if (child->class_name != Bmgr_quark)
    ww->wlm.cur_depth-- ;

return; 
}

/***** **** *** ** * realize_child * ** *** **** *****/

static void
realize_child (ww, parent, child, root_wlm)
    XcuWlmWidget ww ;
    Widget parent ;
    Fetus *child ;
    Boolean root_wlm ;
{
int	 i ; /* must not be Cardinal */
Widget	 parent_id ;


if (root_wlm)
    {
    child->id = (Widget) ww ;
    parent_id = child->id ;
    }
else
if (child->class_name == Bmgr_quark)
    {
    child->id = XtCreateManagedWidget 
	(
	child->widget_name,
	child->class,
	(Widget) ww,   /* Bmgr's are all children of root wlm */
	NULL,
	0
	) ;
    parent_id = parent ;
    }
else
    {
    child->id = XtCreateManagedWidget 
	(
	child->widget_name,
	child->class,
	parent,
	NULL,
	0
	) ;
    parent_id = child->id ;
    }

add_to_widget_list (ww, child) ; /* compile a list of widget class and names */

/* descend the widget tree */
for (i = child->n_children - 1;  i >=0;  i--)
    realize_child (ww, parent_id, &child->children[i], False) ;

return; 
}

/***** **** *** ** * convert_string_to_arg * ** *** **** *****/

static XcuWlmArgType
convert_string_to_arg (ww, class, name, value, arg)
    XcuWlmWidget ww ;
    String class ;
    String name ;
    String value ;
    Arg *arg ;
{
XrmValue string_value ;
XrmValue converted_value ;
char wwidgetclass[100] ;
WidgetClassList *class_list ;

string_value.addr = (XPointer) value ;
string_value.size = strlen (string_value.addr) ;
if (strcmp (class, "Boolean") == 0)
    {
    /* KLUDGE!  Booleans seem to bomb! */
    if (*value == 't' || *value == 'T')
	XtSetArg (*arg, name, 1) ;
    else
	XtSetArg (*arg, name, 0) ;
    return XcuWlmCharArg ;
    }
/*
 * Also need to search out StringToWidget conversions.  They need the
 * name of the wlm widget tacked on front...
 */
strcpy (wwidgetclass, class) ;
for (class_list = ww->wlm.widget_class_list;
     class_list != NULL;
     class_list = class_list->next)
    {
    if (strcmp (class, XrmQuarkToString(class_list->quark)) == 0)
	{
	sprintf (wwidgetclass, "%s%s", ww->core.name,
				      XrmQuarkToString (class_list->quark)) ;
	break ;
	}
    }
XtConvert ((Widget)ww,  XtRString,  &string_value,
	       wwidgetclass,	   &converted_value) ;


/* TODO : This is not very portable */
if (strcmp(class, "String") == 0)
    {
    /* It's the address we're interested in */
    XtSetArg (*arg, name, (XtArgVal)((String) converted_value.addr)) ;
    return XcuWlmStringArg ;
    }
else
if (strcmp(class, "Boolean") == 0)
    {
    /* 1 byte values */
    XtSetArg (*arg, name, (XtArgVal) (*converted_value.addr)) ;
    return XcuWlmCharArg ;
    }
else
    {
    /* word values */
    XtSetArg (*arg, name, *((XtArgVal *)converted_value.addr)) ;
    return XcuWlmArgValArg ;
    }
}

/***** **** *** ** * add_to_widget_list * ** *** **** *****/

/*
 * Here's the scoop.
 * Since there can be a variety of widget classes, an unknown number of
 * named widgets in each class, and perhaps even more than one widget
 * of the same name, there are linked lists for each, with each entry
 * pointing to the next item in the list.
 */

static void
add_to_widget_list (ww, child)
    XcuWlmWidget ww ;
    Fetus *child ;
{
WidgetClassList *class_list ;
WidgetNameList *name_list ;
WidgetIdList *id_list ;
Widget w = child->id ;
/*
 * Look up list by classname, then corename, then id#
 * Each of these calls has the side effect of adding an entry as needed
 */
if (child->widget_name)
    {
    class_list = get_widget_class_list (&ww->wlm.widget_class_list,
					child->class_name, XcuWlmAddOnNotFound) ;
    name_list = get_widget_name_list (class_list, child->widget_name, XcuWlmAddOnNotFound);
    id_list = get_widget_id_list (name_list, w, XcuWlmAddOnNotFound) ;
    /* id_list is not used now */
    }

return ;
}

/***** **** *** ** * find_public_procedures * ** *** **** *****/

/*
 * Here's the scoop on public procedures.
 * Since I need to access them via a String handle, I must insist that
 * widgets associate their public procedures with a text string and 
 * return this list with a call to XtGetValues, using the resource
 * name XtNprocedureList.  I will maintain a separate list for each
 * widget class so that the names need not be unique across classes.
 * The procedure list is a null terminated array of XtProcedureList entries .
 */

static void
find_public_procedures (ww)
    XcuWlmWidget ww ;
{
WidgetClassList *class_list ;
/*
 * Cycle through the class list, ask an arbitrary representative of
 * that class what the list looks like, and enter them in the proc list.
 */
for (class_list = ww->wlm.widget_class_list;
     class_list != NULL;
     class_list = class_list->next)
    {
    Arg arg ;
    XtProcedureList *procedure_list = NULL ;
    XcuWlmProcedureList *proc_list ;
    XcuWlmProcedureList **last_ptr ;
    Widget arbitrary = class_list->widget_name_list->widget_id_list->id ;

    arg.name = XtNprocedureList ;
    arg.value = (XtArgVal) &procedure_list ;
    XtGetValues (arbitrary, &arg, ONE) ;
    if (procedure_list == NULL)
	continue ;
    /* I must allocate space for this list here -- the arbitrary widget
     * may be destroyed!
     */
    last_ptr = &class_list->proc_list ;
    for (  ; procedure_list->name ;  procedure_list++)
	{
	(*last_ptr) = (XcuWlmProcedureList *) XtMalloc (sizeof(XcuWlmProcedureList)) ;
	proc_list = *last_ptr ;
	proc_list->quark = XrmStringToQuark (procedure_list->name) ;
	proc_list->procedure = procedure_list->procedure ;
	proc_list->next = NULL ;
	last_ptr = &proc_list->next ;
	}
    }

return ;
}

/***** **** *** ** * get_proc_list * ** *** **** *****/

static XtProc
get_proc_list (proc_list, proc)
    XcuWlmProcedureList *proc_list ;
    String proc ;
{
/*
 */
XrmQuark quark = XrmStringToQuark (proc) ;

for ( ;
       proc_list != NULL;
         proc_list = proc_list->next)
    {
    if (proc_list->quark == quark)
	{
	return proc_list->procedure ;
	}
    }

return NULL ;
}

/***** **** *** ** * get_widget_id * ** *** **** *****/

static Widget
get_widget_id (ww, class, name)
XcuWlmWidget ww ;
String class ;
String name ;
{
XrmValue     string_value ;
XrmValue     converted_value ;
Widget id ;
char wwidgetname[100] ;
sprintf (wwidgetname, "%s%s", ww->core.name, class) ;

string_value.addr = (XPointer) name ;
string_value.size = strlen (name) ;

XtConvert ((Widget)ww, XtRString, &string_value, wwidgetname, &converted_value);
id = *((Widget *) converted_value.addr) ;
return id ;
}

/***** **** *** ** * get_widget_class * ** *** **** *****/

static WidgetClass
get_widget_class (ww, class_name)
    XcuWlmWidget ww ;
    XrmQuark class_name ;
{
XcuWlmTagClassList *list ;

for (list = ww->wlm.tag_class_list;
     list != NULL;
     list = list->next)
    {
    if (list->quark == class_name)
	return list->class ;
    }

sprintf (error_text, "Widget class (%s) not found\n",
XrmQuarkToString(class_name)) ;
XtError (error_text) ;
return list->class ; /* Only here to shut up lint */
}

/***** **** *** ** * get_widget_class_list * ** *** **** *****/

static WidgetClassList *
get_widget_class_list (initial_list, widget_class, not_found_action)
    WidgetClassList **initial_list ;
    XrmQuark widget_class ;
    XcuWlmNotFoundAction not_found_action ;
{
WidgetClassList *widget_class_list ;
WidgetClassList **last_ptr = initial_list ;

for (widget_class_list = *initial_list;
       widget_class_list != NULL;
         widget_class_list = widget_class_list->next)
    {
    if (widget_class_list->quark == widget_class)
	return widget_class_list ;
    last_ptr = &widget_class_list->next ;
    }

if (not_found_action == XcuWlmCroakOnNotFound)
    {
    sprintf (error_text, "Widget class (%s) not found\n", widget_class) ;
    XtError (error_text) ;
    }
else
if (not_found_action == XcuWlmNullOnNotFound)
    {
    return NULL ;
    }
/* if here, no list found, so we add one */

widget_class_list = (WidgetClassList *) XtMalloc (sizeof (WidgetClassList)) ;
widget_class_list->quark = widget_class ;
widget_class_list->next = NULL ;
widget_class_list->widget_name_list = NULL ;
widget_class_list->proc_list = NULL ;
*last_ptr = widget_class_list ;

return widget_class_list ;
}

/***** **** *** ** * get_widget_name_list * ** *** **** *****/

static WidgetNameList *
get_widget_name_list (class_list, widget_name, not_found_action)
    WidgetClassList *class_list ;
    String widget_name ;
    XcuWlmNotFoundAction not_found_action ;
{
XrmQuark quark = XrmStringToQuark (widget_name) ;
WidgetNameList *widget_name_list ;
WidgetNameList **last_ptr = &class_list->widget_name_list ;

for (widget_name_list = class_list->widget_name_list;
     widget_name_list != NULL;
     widget_name_list = widget_name_list->next)
    {
    if (widget_name_list->quark == quark)
	return widget_name_list ;
    last_ptr = &widget_name_list->next ;
    }

if (not_found_action == XcuWlmCroakOnNotFound)
    {
    sprintf (error_text, "Widget name (%s) not found\n", widget_name) ;
    XtError (error_text) ;
    }
else
if (not_found_action == XcuWlmNullOnNotFound)
    {
    return NULL ;
    }
/* if here, no list found, so we add one */

widget_name_list = (WidgetNameList *) XtMalloc (sizeof (WidgetNameList)) ;
widget_name_list->quark = quark ;
widget_name_list->next = NULL ;
widget_name_list->widget_id_list = NULL ;
*last_ptr = widget_name_list ;

return widget_name_list ;
}

/***** **** *** ** * get_widget_id_list * ** *** **** *****/

static WidgetIdList *
get_widget_id_list (name_list, widget_id, not_found_action)
    WidgetNameList *name_list ;
    Widget widget_id ;
    XcuWlmNotFoundAction not_found_action ;
{
WidgetIdList *widget_id_list ;
WidgetIdList **last_ptr = &name_list->widget_id_list ;

for (widget_id_list = name_list->widget_id_list;
     widget_id_list != NULL;
     widget_id_list = widget_id_list->next)
    {
    if (widget_id_list->id == widget_id)
	return widget_id_list ;
    last_ptr = &widget_id_list->next ;
    }

if (not_found_action == XcuWlmCroakOnNotFound)
    {
    sprintf (error_text, "Widget id (%d) not found\n", widget_id) ;
    XtError (error_text) ;
    }
else
if (not_found_action == XcuWlmNullOnNotFound)
    {
    return NULL ;
    }
/* if here, no list found, so we add one */

widget_id_list = (WidgetIdList *) XtMalloc (sizeof (WidgetIdList)) ;
widget_id_list->id = widget_id ;
widget_id_list->next = NULL ;
*last_ptr = widget_id_list ;

return widget_id_list ;
}

/***** **** *** ** * bmgr_search * ** *** **** *****/

static void
bmgr_search (ww, child)
    XcuWlmWidget ww ;
    Fetus *child ;
{
int i ; /* must not be Cardinal */
if (child->class_name == Bmgr_quark)
    bmgr_setup (ww, child) ;

for (i = child->n_children - 1;  i >=0;  i--)
    bmgr_search (ww, &child->children[i]) ;

return; 
}

/***** **** *** ** * directive_search * ** *** **** *****/

static void
directive_search (ww, child)
    Fetus *child ;
    XcuWlmWidget ww ;
{
int i ; /* must not be Cardinal */
if (child->directives)
    directive_setup (ww, child) ;

for (i = child->n_children - 1;  i >=0;  i--)
    directive_search (ww, &child->children[i]) ;

return; 
}

/***** **** *** ** * bmgr_setup * ** *** **** *****/

static void
bmgr_setup (ww, child)
    XcuWlmWidget ww ;
    Fetus *child ;
{
XcuButtonWidget *button_list ;
XPointer *value_list ;
int i ;

/*
 * Look widgets up in a list of name/id pairs.
 */

if (!child->n_manage_list)
    {
    char text[200] ;
    sprintf (text, "XcuWlm: Bmgr (%s) with no children!\n", child->widget_name) ;
    XtError (text) ;
    }
button_list = (XcuButtonWidget *) XtMalloc (child->n_manage_list * sizeof (XcuButtonWidget)) ;
value_list = (XPointer *) XtMalloc (child->n_manage_list * sizeof (XPointer)) ;

for (i=0;  i < child->n_manage_list;  i++)
    {
    Arg arg ;
    button_list[i] = (XcuButtonWidget)
	get_widget_id(ww, "XcuButton", child->manage_list[i].widget);
    if (child->manage_list[i].type != NULL)
	{
	convert_string_to_arg (ww, child->manage_list[i].type, "",
			       child->manage_list[i].value, &arg) ;
	value_list[i] = (XPointer) arg.value ;
	}
    /***
    else
    if (child->use_names_as_calldata)
	{
	value_list[i] = (XPointer) button_list[i]->core.name ;
	}
    ***/
    else
	{
	value_list[i] = (XPointer) button_list[i]->label.label ;
	/*
	value_list[i] = NULL ;
	*/
	}
    }

XcuBmgrManage ((XcuBmgrWidget)child->id, button_list, value_list,
		child->n_manage_list) ;
return ;
}

/***** **** *** ** * directive_setup * ** *** **** *****/

static void
directive_setup (ww, child)
    XcuWlmWidget ww ;
    Fetus *child ;
{
Cardinal i, j ;

for (i=0;  i < child->n_directives;  i++)
    {
    Directive *directive = &child->directives[i] ;
    /*
     * for each unique callback in the directives, add a callback
     */
    for (j=0;  j < i;  j++)
	{
	if (strcmp (child->directives[i].callback_name,
		    child->directives[j].callback_name) == 0)
	    break ;
	}
    if (j == i)
	{
	directive->client_data[0] = (long) ww ;
	directive->client_data[1] =
	    (long) directive->callback_name ;
	directive->client_data[2] = (long) child ;
	XtAddCallback (child->id, directive->callback_name,
		       wlm_central, (XPointer) directive->client_data) ; 
	add_to_sample_list (ww, XrmQuarkToString(child->class_name), child->id);
	}
    }
return ;
}

/***** **** *** ** * wlm_central * ** *** **** *****/

static void
wlm_central (w, client, call)
    Widget w ;
    XPointer client ; /* [0] is the wlm widget,
		      * [1] is the callback_name,
		      * [2] is the child
		      */
    XPointer call ;
{
Cardinal i ;
long *long_client = (long *) client ;
XcuWlmWidget ww =		  (XcuWlmWidget) long_client[0] ;
String    call_callback = (String)    long_client[1] ;
Fetus     *child =	  (Fetus *)   long_client[2] ;
Widget	   id ;

/*
 * Check for directive actions
 */

for (i=0;  i < child->n_directives;  i++)
    {
    Cardinal j ;
    Directive *directive = &child->directives[i] ;
    if (strcmp(directive->callback_name, call_callback) != 0)
	continue ; /* not the callback for this directive */

    /*
     * We need to check each comparison -- all must be true to trigger action
     */
    for (j=0;  j < directive->n_call_comparisons;  j++)
	{
	Boolean comparison ;
	int k ; /* Must not be Cardinal */
	XcuWlmArgType case_number ;
	XPointer **ptr ;
	Arg arg ;
	/*
	 * 1) convert call_data to real data via call_data_converter
	 * 2) find data in caller
	 * 3) compare using call_data_operator
	 */
	case_number = convert_string_to_arg
				(ww, directive->call_data_converter[j],
			             "", directive->call_data[j], &arg) ;
	/*
	 * For each index except the last, do a dereference
	 * WARNING: This code assumes that all pointers are created equal...
	 */
	ptr = (XPointer **) call ;
	for (k=0;  k < (int)directive->n_call_indices[j] - 1;  k++)
	    {
	    ptr = (XPointer **) *(call + directive->call_data_index[j][k]) ;
	    }

	/*
	 * For the last dereference, we absolutely must branch according
	 * to type...
	 * TODO: For now,
	 * these are hardwired, but eventually they will be generated
	 * by a compile-time preprocessor and included...
	 */

	switch (case_number)
	  {
	  case XcuWlmArgValArg :
	    {
	    XtArgVal value ;
	    if (directive->n_call_indices[j] == 0)
		{
		union { XPointer **ptr ; XtArgVal value ; } uni ;
		uni.ptr = ptr ;
		value = uni.value ;
		}
	    else
		value = *(((XtArgVal *)ptr) + directive->call_data_index[j][k]);
	    switch (directive->call_data_operator[j])
	      {
	      case XcuWlmDirectiveEquivalence :
		{
		comparison = (value == arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveNonEquivalence :
		{
		comparison = (value != arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThan :
		{
		comparison = (value > arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThan :
		{
		comparison = (value < arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThanOrEqualTo :
		{
		comparison = (value >= arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThanOrEqualTo :
		{
		comparison = (value <= arg.value ? True : False) ;
		break ;
		}
	      }
	    break ;
	    }
	  case XcuWlmFloatArg :
	    {
	    /*
	     * This seems like a disgusting hack, but...
	     */
	    float value ;
	    float arg_value ;
	    union { XtArgVal i ; float f ; } i_to_f ;

	    i_to_f.i = arg.value ;
	    arg_value = i_to_f.f ;

	    if (directive->n_call_indices[j] == 0)
		{
		union { XPointer **ptr ; float value ; } uni ;
		uni.ptr = ptr ;
		value = uni.value ;
		}
	    else
		{
		value = *(((float *) ptr) + directive->call_data_index[j][k]) ;
		}

	    switch (directive->call_data_operator[j])
	      {
	      case XcuWlmDirectiveEquivalence :
		{
		comparison = (value == arg_value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveNonEquivalence :
		{
		comparison = (value != arg_value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThan :
		{
		comparison = (value > arg_value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThan :
		{
		comparison = (value < arg_value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThanOrEqualTo :
		{
		comparison = (value >= arg_value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThanOrEqualTo :
		{
		comparison = (value <= arg_value ? True : False) ;
		break ;
		}
	      }
	    break ;
	    }
	  case XcuWlmCharArg :
	    {
	    char value ;
	    if (directive->n_call_indices[j] == 0)
		{
		union { XPointer **ptr ; char value ; } uni ;
		uni.ptr = ptr ;
		value = uni.value ;
		}
	    else
		value = *(((char *) ptr) + directive->call_data_index[j][k]) ;
	    switch (directive->call_data_operator[j])
	      {
	      case XcuWlmDirectiveEquivalence :
		{
		comparison = (value == arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveNonEquivalence :
		{
		comparison = (value != arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThan :
		{
		comparison = (value > arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThan :
		{
		comparison = (value < arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThanOrEqualTo :
		{
		comparison = (value >= arg.value ? True : False) ;
		break ;
		}
	      case XcuWlmDirectiveLessThanOrEqualTo :
		{
		comparison = (value <= arg.value ? True : False) ;
		break ;
		}
	      }
	    break ;
	    }
	  case XcuWlmStringArg :
	    {
	    String value ;
	    if (directive->n_call_indices[j] == 0)
		{
		union { XPointer **ptr ; String value ; } uni ;
		uni.ptr = ptr ;
		value = uni.value ;
		}
	    else
		value = *(((String *) ptr) + directive->call_data_index[j][k]) ;
	    switch (directive->call_data_operator[j])
	      {
	      case XcuWlmDirectiveEquivalence :
		{
		comparison = (strcmp (value,
				     (String) arg.value) == 0) ?
				True : False ;
		break ;
		}
	      case XcuWlmDirectiveNonEquivalence :
		{
		comparison = (strcmp (value,
				     (String) arg.value) != 0) ?
				True : False ;
		break ;
		}
	      case XcuWlmDirectiveGreaterThan :
	      case XcuWlmDirectiveLessThan :
	      case XcuWlmDirectiveGreaterThanOrEqualTo :
	      case XcuWlmDirectiveLessThanOrEqualTo :
		{
		/* TODO : Do something with these ? */
		/* comparison is potentially uninitialized */
		break ;
		}
	      }
	    break ;
	    }
	  }


	if (comparison == False)
	    break ;
	}

    if (j < directive->n_call_comparisons)
	{
	continue ; /* do nothing, there was something not matched */
	}
    /*
     * Action has been triggered
     * First find the target widget, then branch on the two possibilities
     */

    id = get_widget_id (ww, directive->target_class, directive->target_name) ;

    if (directive->resource.name)
	{
	/* The directive is to modify a widget's resource */
	Arg arg ;

	if (strcmp (directive->resource.value, "CALL") == 0)
	    {
	    XtSetArg (arg, directive->resource.name, call) ;
	    }
	else
	    convert_string_to_arg (ww,
			   get_resource_info (XtClass(id),
					      directive->resource.name,
					      XcuWlmResourceInfoRepresentation),
			   directive->resource.name,
			   directive->resource.value, &arg) ;
	XtSetValues (id, &arg, ONE) ;
	}
    else
	{
	XrmValue string_value ;
	XrmValue converted_value ;
	Cardinal k ;
#define MAX_WLM_PROCEDURE_ARGS 10
	Arg arg[MAX_WLM_PROCEDURE_ARGS] ;
	/* The directive is to call a widget's public procedure */
	string_value.addr = (XPointer) directive->procedure ;
	string_value.size = strlen (string_value.addr) ;

	ww->wlm.conversion_class = XrmStringToQuark(directive->target_class) ;
	XtConvert ((Widget)ww,  XtRString, &string_value,
			 "Procedure", &converted_value) ;
	for (k=0;  k < directive->n_arguments;  k++)
	    {
	    convert_string_to_arg (ww,
				   directive->argument_converters[k],
				   "",
				   directive->argument_strings[k],
				   &arg[k]) ;
	    }
	if (*((XcuProc *)converted_value.addr) == 0)
	    {
	    String errortext = XtMalloc (strlen(directive->procedure) +
					 strlen(directive->target_class) + 80);
	    sprintf (errortext,
	     "XcuWlm: Public function specified (%s:%s)\n\tdoes not exist!\n",
		 directive->target_class, directive->procedure) ;
	    XtError (errortext) ;
	    }
	(*(*((XcuProc *) converted_value.addr)))(id, arg[0].value,
	arg[1].value, arg[2].value, arg[3].value, arg[4].value, arg[5].value,
	arg[6].value, arg[7].value, arg[8].value, arg[9].value) ;
	}
    }

return ;
}

/***** **** *** ** * get_resource_info * ** *** **** *****/

/*
 * given a widget class and a resource name,
 * this looks up the class of the resource
 */

static String
get_resource_info (class, name, info_type)
    WidgetClass class ;
    String name ;
    XcuWlmResourceInfoType info_type ;
{
Cardinal i ;
XtResourceList t_resource ;
Cardinal nt_resource ;
String t_info ;


XtGetResourceList (class, &t_resource, &nt_resource) ;

for (i=0;  i < nt_resource;  i++)
    {
    if (strcmp (name, t_resource[i].resource_name) == 0)
	break ;
    }

if (i == nt_resource)
    {
    XtFree (t_resource) ;
    return NULL ;
    }

if (info_type == XcuWlmResourceInfoClass)
    {
    Xcu_copy_ds (&t_info, t_resource[i].resource_class) ;
    }
else
    {
    Xcu_copy_ds (&t_info, t_resource[i].resource_type) ;
    }

XtFree (t_resource) ;

return t_info ;
}

/***** **** *** ** * AddStringToWidgetConverter * ** *** **** *****/

static void
AddStringToWidgetConverter (ww)
    XcuWlmWidget ww ;
{
WidgetClassList *class_list ;
XtConvertArgRec *widgetConvertArgs ;
/*
 * for each widget class, add a converter
 */
for (class_list = ww->wlm.widget_class_list;
     class_list != NULL;
     class_list = class_list->next)
    {
    char wwidgetname[100] ;
    sprintf (wwidgetname, "%s%s", ww->core.name,
				  XrmQuarkToString (class_list->quark)) ;
    widgetConvertArgs = (XtConvertArgRec *)
			XtMalloc (2 * sizeof(XtConvertArgRec)) ;
    widgetConvertArgs[0].address_mode = XtAddress ;
    widgetConvertArgs[0].address_id = (XPointer) class_list ;
    widgetConvertArgs[0].size = (Cardinal) sizeof(WidgetClassList *) ;
    widgetConvertArgs[1].address_mode = XtAddress ;
    widgetConvertArgs[1].address_id = (XPointer) ww->core.name ;
    widgetConvertArgs[1].size = (Cardinal) strlen (ww->core.name) ;

    XtAddConverter (XtRString, wwidgetname,
		    CvtStringToWidget,
		    widgetConvertArgs, TWO) ;
    }
return ;
}

/***** **** *** ** * AddStringToProcedureConverter * ** *** **** *****/

static void
AddStringToProcedureConverter ()
{
static XtConvertArgRec procedureConvertArgs[] =
    {
     {
     XtBaseOffset,
     (XPointer) XtOffset (XcuWlmWidget, wlm.widget_class_list),
     sizeof (WidgetClassList *)
     }
    ,{
     XtBaseOffset,
     (XPointer) XtOffset (XcuWlmWidget, wlm.conversion_class),
     sizeof (XrmQuark)
     }
    } ;

XtAddConverter (XtRString, "Procedure", CvtStringToProcedure,
		procedureConvertArgs, XtNumber (procedureConvertArgs)) ;
return ;
}

/***** **** *** ** * CvtStringToWidget * ** *** **** *****/

/*
 * Convert String To Widget
 *   Given a widget name, 
 *   and the wlm class list of the widget,
 *   It returns the first id in the IdList for the appropriate widget
 *   TODO: How and whether or not to handle multiple id's
 */

static void
CvtStringToWidget (args, num_args, fromVal, toVal)
    XrmValuePtr args ;		/* contains the address of the class list */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
String	widget_name = (String) fromVal->addr ;

WidgetClassList *class_list = (WidgetClassList *) args[0].addr ;
WidgetNameList *name_list ;

name_list = get_widget_name_list (class_list, widget_name, XcuWlmCroakOnNotFound) ;

toVal->size = sizeof (WidgetIdList *) ;
toVal->addr = (XPointer) &name_list->widget_id_list->id ;

return ;
}

/***** **** *** ** * CvtStringToProcedure * ** *** **** *****/

/*
 * Convert String To Procedure
 *   Given a procedure name
 *   and the wlm procedure list of the wlm widget that contains the widget,
 *   It returns the address of the appropriate public function
 */

static void
CvtStringToProcedure (args, num_args, fromVal, toVal)
    XrmValuePtr args ;		/* contains the address of the proc list */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
String proc_name = (String) fromVal->addr ;
XrmQuark widget_class = *((XrmQuark *) args[1].addr) ;
WidgetClassList *initial_class_list = *((WidgetClassList **) args[0].addr) ;
WidgetClassList *class_list ;

static XtProc procedure ;

class_list = get_widget_class_list (&initial_class_list, widget_class,
				    XcuWlmCroakOnNotFound) ;
procedure = get_proc_list (class_list->proc_list, proc_name) ;

toVal->size = sizeof (WidgetIdList *) ;
toVal->addr = (XPointer) &procedure ;

return ;
}

/**
 ** The routines to read in the compiled layout
 **/

#define get_pad() *ptr += pad
#define get_nl() (*ptr)++

/***** **** *** ** * ParseXcuWlmLayout * ** *** **** *****/

static void
ParseXcuWlmLayout (ww)
    XcuWlmWidget ww ;
{
String ptr ;
ptr = layout_input->input_buffer ;
if (*ptr != '#' ||  *(ptr+1) != 'W' || *(ptr+2) != 'L' ||
		    *(ptr+3) != 'D' || *(ptr+4) != 'L')
    {
    String errortext = XtMalloc (strlen(layout_input->file0) + 80) ;
    sprintf (errortext,
	     "XcuWlm: File specified (%s) is not a compiled layout file!\n",
	     layout_input->file0) ;
    XtError (errortext) ;
    }
ptr+=6 ;

ww->wlm.max_depth = get_int (&ptr) ;
ww->wlm.child = (Fetus *) XtCalloc (1, sizeof (Fetus)) ;
get_child (ww->wlm.child, &ptr, 0) ;
return ;
}

/***** **** *** ** * get_child * ** *** **** *****/

static void
get_child (child, ptr, pad)
    Fetus *child ;
    String *ptr ;
    int pad ;
{
Cardinal i ;

get_nl() ;
get_pad() ;
child->class_name = XrmStringToQuark (get_word(ptr)) ;

get_pad() ;
child->n_resources = get_int(ptr) ;

if (child->n_resources)
child->resources = (Resource *)
		    XtMalloc (child->n_resources * sizeof (Resource)) ;
for (i=0;  i < child->n_resources;  i++)
    {
    get_resource (&child->resources[i], ptr, pad) ;
    }

get_pad() ;
child->n_directives = get_int(ptr) ;

if (child->n_directives)
child->directives = (Directive *)
		    XtMalloc (child->n_directives * sizeof (Directive)) ;
for (i=0;  i < child->n_directives;  i++)
    get_directive (&child->directives[i], ptr, pad) ;

get_pad() ;
child->n_manage_list = get_int(ptr) ;

if (child->n_manage_list)
child->manage_list = (XcuWlmManageItem *)
		    XtMalloc (child->n_manage_list * sizeof (XcuWlmManageItem)) ;
for (i=0;  i < child->n_manage_list;  i++)
    get_manage (&child->manage_list[i], ptr, pad) ;

get_pad() ;
child->n_children = get_int(ptr) ;

if (child->n_children)
child->children = (Fetus *) XtCalloc (child->n_children, sizeof (Fetus)) ;
pad += 2 ;
for (i=0;  i < child->n_children;  i++)
    get_child (&child->children[i], ptr, pad) ;

return ;
}

/***** **** *** ** * get_int * ** *** **** *****/

static int
get_int (ptr)
    String *ptr ;
{
int value ;
String p = *ptr ;
/*
 * ptr is now looking at something like: 5
 * Find the end of ascii integer and put a null there; use atoi to convert it.
 * Advance master pointer to point to just past that null,
 * and return the integer.
 */

while (*p != ' ' && *p != '\n')
    p++ ;
*p = '\0'; 
value = atoi(*ptr) ;
*ptr = p+1 ;
return value ;
}

/***** **** *** ** * get_word * ** *** **** *****/

static String
get_word (ptr)
    String *ptr ;
{
int length ;
String p ;
/*
 * ptr is now looking at something like: 5 Label
 * Find the end of ascii integer and put a null there; use atoi to convert it.
 * Then put a null at the end of the word to be found.
 * Advance master pointer to point to just past that null,
 * and return a pointer to the word.
 */

length = get_int(ptr) ;
p = *ptr ;
p[length] = '\0' ;
*ptr = p + length + 1 ;
return p ;
}

/***** **** *** ** * get_string * ** *** **** *****/

static String
get_string (ptr)
    String *ptr ;
{
int length ;
String p ;
/*
 * Same as get_word, but string is enclosed in parentheses
 */

length = get_int(ptr) ;
p = *ptr + 1 ;
p[length] = '\0' ;
*ptr = p + length + 2 ;
return p ;
}

/***** **** *** ** * get_resource * ** *** **** *****/

static void
get_resource (resource, ptr, pad)
    Resource *resource ;
    String *ptr ;
    int pad ;
{
String context ;
get_pad() ;
resource->name = get_word(ptr) ;
resource->value = get_string(ptr) ;
context = get_word(ptr) ;
resource->context = (strlen(context)==5) ? XcuWlmLocalContext : XcuWlmGlobalContext ;

return ;
}

/***** **** *** ** * get_directive * ** *** **** *****/

static void
get_directive (directive, ptr, pad)
    Directive *directive ;
    String *ptr ;
    int pad ;
{
Cardinal i, j,  n ;
String dtype ;

get_pad() ;
directive->callback_name = get_word(ptr) ;
directive->target_class = get_word(ptr) ;
directive->target_name = get_word(ptr) ;
n = directive->n_call_comparisons = get_int(ptr) ;
dtype = get_word(ptr) ;

pad++ ;
directive->call_data_converter = (String *) XtMalloc (n * sizeof (String)) ;
directive->call_data = (String *) XtMalloc (n * sizeof (String)) ;
directive->call_data_operator = (XcuWlmDirectiveOperator *)
	XtMalloc (n * sizeof (XcuWlmDirectiveOperator)) ;
directive->n_call_indices = (Cardinal *) XtMalloc (n * sizeof (Cardinal)) ;
directive->call_data_index = (int **) XtMalloc (n * sizeof (int *)) ;

for (i=0;  i < n;  i++)
    {
    String dummy ;
    get_pad() ;
    directive->call_data_converter[i] = get_word(ptr) ;
    directive->call_data[i] = get_string(ptr) ;
    directive->call_data_operator[i] = (XcuWlmDirectiveOperator) get_int(ptr) ;
    dummy = get_word(ptr) ; /* there only for visualization of operator */
    directive->n_call_indices[i] = (Cardinal) get_int(ptr) ;
    (*ptr) += 2 ; /* skip the ": " */
    directive->call_data_index[i] =
	(int *) XtMalloc (directive->n_call_indices[i] * sizeof (int)) ;
    for (j=0;  j < directive->n_call_indices[i];  j++)
	directive->call_data_index[i][j] = get_int(ptr) ;
    }
pad-- ;

get_pad() ;
if (strcmp(dtype, "XtSetValues") == 0)
    {
    directive->target_class = get_word(ptr) ;
    directive->target_name = get_string(ptr) ;
    directive->resource.name = get_word(ptr) ;
    directive->resource.value = get_string(ptr) ;
    }
else
    {
    directive->resource.name = NULL ;
    directive->procedure = get_word(ptr) ;
    directive->n_arguments = get_int(ptr) ;
    directive->argument_converters =
	(String *) XtMalloc (directive->n_arguments * sizeof(String)) ;
    directive->argument_strings =
	(String *) XtMalloc (directive->n_arguments * sizeof(String)) ;
    for (i=0;  i < directive->n_arguments;  i++)
	{
	get_pad() ;
	directive->argument_converters[i] = get_word(ptr) ;
	directive->argument_strings[i] = get_string(ptr) ;
	}
    }

return ;
}

/***** **** *** ** * get_manage * ** *** **** *****/

static void
get_manage (managed, ptr, pad)
    XcuWlmManageItem *managed ;
    String *ptr ;
    int pad ;
{
get_pad() ;
managed->widget = get_string(ptr) ;
managed->type = get_word(ptr) ;
managed->value = get_string(ptr) ;

return ;
}

/**
 ***
 **** Public Procedures
 ***
 **/

/**
 ** Device Functions
 **/

/***** **** *** ** * XcuWlmSample * ** *** **** *****/

void
XcuWlmSample (ww)
    XcuWlmWidget ww ;
{
XcuWlmSampleList *sample_list ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSample requires arg to be subclass of xcuWlmWidgetClass");
    }

if (ww->wlm.first_device)
    {
    ww->wlm.first_device = False ;
    }

ww->wlm.sampling_active = True ;

for (sample_list=ww->wlm.sample_list;
     sample_list != NULL;
     sample_list = sample_list->next)
     {
     (*sample_list->sample_proc)(sample_list->id) ;
     }

ww->wlm.sampling_active = False ;

return ;
}

/***** **** *** ** * XcuWlmRequest * ** *** **** *****/

void
XcuWlmRequest (ww)
    XcuWlmWidget ww ;
{
XcuWlmAppRequest (XtWidgetToApplicationContext((Widget)ww), ww) ;
return ;
}

/***** **** *** ** * XcuWlmAppRequest * ** *** **** *****/

void
XcuWlmAppRequest (app, ww)
    XtAppContext app ;
    XcuWlmWidget ww ;
{
XcuWlmSreRecord *save ;
/*
 * if there is a wlmEvent already queued, return it
 * o.w., wait for something to happen
 */
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmRequest requires arg to be subclass of xcuWlmWidgetClass");
    }
if (ww->wlm.first_device)
    {
    XcuWlmSample (ww) ;
    ww->wlm.first_device = False ;
    }

if (ww->wlm.queue_ptr != NULL)
    {
    dequeue_event (ww) ;
    ww->wlm.event_stamp++ ;
    return ;
    }

/*
 * Since requests can be recursive, we have to keep a stack of them
 */

save = ww->wlm.sre_status ;
ww->wlm.sre_status = (XcuWlmSreRecord *) XtMalloc (sizeof (XcuWlmSreRecord)) ;
ww->wlm.sre_status->status = XcuWlmRequestNotSatisfied ;
ww->wlm.sre_status->next = save ;

while (ww->wlm.sre_status->status == XcuWlmRequestNotSatisfied)
    {
    XEvent event ;

    XtAppNextEvent (app, &event) ;
    if (ww->wlm.kill_request)
	{
	ww->wlm.kill_request = False ;
	break ;
	}
    XtDispatchEvent (&event) ;
    }
my_app_sync (app) ;

ww->wlm.event_stamp++ ;
save = ww->wlm.sre_status ;
ww->wlm.sre_status = ww->wlm.sre_status->next ;
XtFree (save) ;

return ;
}

/***** **** *** ** * XcuWlmKillRequest * ** *** **** *****/

void
XcuWlmKillRequest (ww)
    XcuWlmWidget ww ;
{
/*
 * if there is a wlmEvent already queued, return
 * o.w., add a dummy event
 */
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmRequest requires arg to be subclass of xcuWlmWidgetClass");
    }

ww->wlm.kill_request = True ;
XPutBackEvent (XtDisplay(ww), NULL) ;
XFlush (XtDisplay(ww)) ;

return ;
}

/***** **** *** ** * XcuWlmEvent * ** *** **** *****/

Boolean
XcuWlmEvent (ww)
    XcuWlmWidget ww ;
{
return XcuWlmAppEvent (XtWidgetToApplicationContext((Widget)ww), ww) ;
}

/***** **** *** ** * XcuWlmAppEvent * ** *** **** *****/

Boolean
XcuWlmAppEvent (app, ww)
    XtAppContext app ;
    XcuWlmWidget ww ;
{
XcuWlmSreRecord *save ;
Boolean return_status ;
/*
 * if there is a wlmEvent already queued, return it
 * o.w., wait for something to happen
 */
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmEvent requires arg to be subclass of xcuWlmWidgetClass");
    }
if (ww->wlm.first_device)
    {
    XcuWlmSample (ww) ;
    ww->wlm.first_device = False ;
    }

if (ww->wlm.queue_ptr != NULL)
    {
    dequeue_event (ww) ;
    return True ;
    }

save = ww->wlm.sre_status ;
ww->wlm.sre_status = (XcuWlmSreRecord *) XtMalloc (sizeof (XcuWlmSreRecord)) ;
ww->wlm.sre_status->status = XcuWlmRequestNotSatisfied ;
ww->wlm.sre_status->next = save ;

if (XtAppPending(app))
    {
    /***
    XEvent event ;

    XtNextEvent (&event) ;
    XtDispatchEvent (&event) ;
    ***/
    my_app_sync (app) ;
    }

return_status = (ww->wlm.sre_status->status == XcuWlmRequestSatisfied) ;
save = ww->wlm.sre_status ;
ww->wlm.sre_status = ww->wlm.sre_status->next ;
XtFree (save) ;

if (return_status)
    ww->wlm.event_stamp++ ;

return return_status ;
}

/**
 ** Association Functions
 **/

/***** **** *** ** * XcuWlmAddCallback * ** *** **** *****/

void
XcuWlmAddCallback (ww, connection_type,
		widget_class, widget_name, callback_name, 
		callback_proc, client_data)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    XtCallbackProc callback_proc ;
    XPointer client_data ;
{
Widget callback_widget ;
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmAddCallback requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    /* match */
    if (!((int)connection_type & (int)XcuWLM_BYPASS))
	{
	XcuWlmIndirectRecord *record ;
	record = (XcuWlmIndirectRecord *) XtMalloc (sizeof (XcuWlmIndirectRecord)) ;
	record->callback_proc = callback_proc ;
	record->client_data = client_data ;
	record->ww = ww ;
	XtAddCallback (callback_widget, callback_name,
		       callback_indirect, (XPointer) record) ;
	}
    else
	{
	XtAddCallback (callback_widget, callback_name,
		       callback_proc, client_data) ;
	}

    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmAddCallback", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

/***** **** *** ** * XcuWlmAddInput * ** *** **** *****/

void
XcuWlmAddInput (ww, connection_type, channel, callback_proc, client_data)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    int channel ;
    XtCallbackProc callback_proc ;
    XPointer client_data ;
{
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmAddCallback requires arg0 to be subclass of xcuWlmWidgetClass");
    }
if (!((int)connection_type & (int)XcuWLM_BYPASS))
	{
	XcuWlmIndirectRecord *record ;
	record = (XcuWlmIndirectRecord *) XtMalloc (sizeof (XcuWlmIndirectRecord)) ;
	record->callback_proc = callback_proc ;
	record->client_data = client_data ;
	record->ww = ww ;
	XtAddInput (channel, XtInputReadMask,
		       callback_channel, (XPointer) record) ;
	}
else
    {
    XtAddInput (channel, XtInputReadMask,
		   callback_proc, client_data) ;
    }

return ;
}

/***** **** *** ** * XcuWlmSetFloat, Int, String * ** *** **** *****/

void
XcuWlmSetFloat (ww, connection_type, widget_class, widget_name,
		  callback_name, target, value)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    float *target ;
    float value ;
{
XcuWlmSetFloatRecord *record ;
Widget callback_widget ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSetFloat requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmSetFloatRecord *) XtMalloc (sizeof (XcuWlmSetFloatRecord)) ;
    record->value = value ;
    record->target = target ;
    record->ww = ww ;
    record->wlm_type = connection_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_set_float, (XPointer) record) ;
    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmSetFloat", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

void
XcuWlmSetBoolean (ww, connection_type, widget_class, widget_name,
	        callback_name, target, value)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    Boolean *target ;
    Boolean value ;
{
XcuWlmSetBoolRecord *record ;
Widget callback_widget ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSetBoolean requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmSetBoolRecord *) XtMalloc (sizeof (XcuWlmSetBoolRecord)) ;
    record->value = value ;
    record->target = target ;
    record->ww = ww ;
    record->wlm_type = connection_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_set_bool, (XPointer) record) ;
    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmSetBoolean", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

void
XcuWlmSetInt (ww, connection_type, widget_class, widget_name,
	        callback_name, target, value)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    int *target ;
    int value ;
{
XcuWlmSetIntRecord *record ;
Widget callback_widget ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSetInt requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmSetIntRecord *) XtMalloc (sizeof (XcuWlmSetIntRecord)) ;
    record->value = value ;
    record->target = target ;
    record->ww = ww ;
    record->wlm_type = connection_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_set_int, (XPointer) record) ;
    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmSetInt", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

void
XcuWlmSetString (ww, connection_type, widget_class, widget_name,
		  callback_name, target, value)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    String *target ;
    String value ;

{
XcuWlmSetStringRecord *record ;
Widget callback_widget ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSetString requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmSetStringRecord *) XtMalloc (sizeof (XcuWlmSetStringRecord)) ;
    Xcu_copy_ds (&record->value, value) ;
    record->target = target ;
    record->ww = ww ;
    record->wlm_type = connection_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_set_string, (XPointer) record) ;
    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmSetString", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

/***** **** *** ** * XcuWlmGetFloat, Int, String * ** *** **** *****/

void
XcuWlmGetFloat (ww, connection_type, widget_class, widget_name,
		  callback_name, target)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    float *target ;
{
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetFloat requires arg0 to be subclass of xcuWlmWidgetClass");
    }
get_int_float_string (ww, widget_class, widget_name,
			   callback_name, (XPointer *) target,
			   "XcuWlmGetFloat", XcuWlmGetFloatType, connection_type) ;
return ;
}

void
XcuWlmGetBoolean (ww, connection_type, widget_class, widget_name,
		callback_name, target)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    Boolean *target ;
{
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetBoolean requires arg0 to be subclass of xcuWlmWidgetClass");
    }
get_int_float_string (ww, widget_class, widget_name,
			   callback_name, (XPointer *) target,
			   "XcuWlmGetBoolean", XcuWlmGetBoolType, connection_type) ;
}

void
XcuWlmGetInt (ww, connection_type, widget_class, widget_name,
		callback_name, target)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    int *target ;
{
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetInt requires arg0 to be subclass of xcuWlmWidgetClass");
    }
get_int_float_string (ww, widget_class, widget_name,
			   callback_name, (XPointer *) target,
			   "XcuWlmGetInt", XcuWlmGetIntType, connection_type) ;
}

void
XcuWlmGetString (ww, connection_type, widget_class, widget_name,
		   callback_name, target)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    String *target ;
{
if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetString requires arg0 to be subclass of xcuWlmWidgetClass");
    }
get_int_float_string (ww, widget_class, widget_name,
			   callback_name, (XPointer *) target,
			   "XcuWlmGetString", XcuWlmGetStringType, connection_type) ;
return ;
}

/***** **** *** ** * XcuWlmGetScan * ** *** **** *****/

void
XcuWlmGetScan (ww, connection_type,
	widget_class, widget_name, callback_name, format,
	t0, t1, t2, t3, t4, t5, t6, t7, t8, t9,
	t10, t11, t12, t13, t14, t15, t16, t17, t18, t19)
    XcuWlmWidget ww ;
    XcuWlmConnectionType connection_type ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    String format ;
    XPointer t0,  t1,  t2,  t3,  t4,  t5,  t6,  t7,  t8,  t9,
	    t10, t11, t12, t13, t14, t15, t16, t17, t18, t19 ;
{
XcuWlmScanRecord *record ;
Widget callback_widget ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetScan requires arg0 to be subclass of xcuWlmWidgetClass");
    }
callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmScanRecord *) XtMalloc (sizeof (XcuWlmScanRecord)) ;
    Xcu_copy_ds (&record->format, format) ;
    record->t0 = t0 ; record->t1 = t1 ; record->t2 = t2 ;
    record->t3 = t3 ; record->t4 = t4 ; record->t5 = t5 ;
    record->t6 = t6 ; record->t7 = t7 ; record->t8 = t8 ;
    record->t9 = t9 ; record->t10 = t10 ; record->t11 = t11 ;
    record->t12 = t12 ; record->t13 = t13 ; record->t14 = t14 ;
    record->t15 = t15 ; record->t16 = t16 ; record->t17 = t17 ;
    record->t18 = t18 ; record->t19 = t19 ;
    record->ww = ww ;
    record->wlm_type = connection_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_scan, (XPointer) record) ;
    if ((int)connection_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning ("XcuWlmGetScan", widget_class, widget_name,
				callback_name) ;
    }

return ;
}

/**
 ** Convenience Functions
 **/

/***** **** *** ** * XcuWlmInquireWidget * ** *** **** *****/

Widget
XcuWlmInquireWidget (ww, widget_class, widget_name)
    XcuWlmWidget ww ;
    String widget_class ;
    String widget_name ;
{
WidgetClassList *class_list ;
WidgetNameList *name_list ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmInquireWidget requires arg0 to be subclass of xcuWlmWidgetClass");
    }
class_list = get_widget_class_list (&ww->wlm.widget_class_list,
	      XrmStringToQuark(widget_class), XcuWlmNullOnNotFound) ;
if (class_list == NULL)
    {
    sprintf (error_text, "XcuWlmInquireWidget: Class (%s) not found\n",
		widget_class) ;
    XtWarning (error_text) ;
    return NULL ;
    }

name_list = get_widget_name_list (class_list, widget_name, XcuWlmNullOnNotFound);
if (name_list == NULL)
    {
    sprintf (error_text,
		"XcuWlmInquireWidget: Name (%s) not found in Class (%s)\n",
		 widget_name, widget_class) ;
    XtWarning (error_text) ;
    return NULL ;
    }

/* TODO: for now just return the first one */
return name_list->widget_id_list->id ;
}

/***** **** *** ** * XcuWlmSetValue * ** *** **** *****/

void
XcuWlmSetValue (ww, widget_class, widget_name, resource_name, resource_value)
    XcuWlmWidget ww ;
    String widget_class ;
    String widget_name ;
    String resource_name ;
    String resource_value ;
{
Arg arg ;
WidgetClassList *class_list ;
WidgetNameList *name_list ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmSetValue requires arg0 to be subclass of xcuWlmWidgetClass");
    }
class_list = get_widget_class_list (&ww->wlm.widget_class_list,
	      XrmStringToQuark(widget_class), XcuWlmNullOnNotFound) ;
if (class_list == NULL)
    {
    sprintf (error_text, "XcuWlmSetValue: Class (%s) not found\n", widget_class) ;
    XtWarning (error_text) ;
    return ;
    }

name_list = get_widget_name_list (class_list, widget_name, XcuWlmNullOnNotFound);
if (name_list == NULL)
    {
    sprintf (error_text, "XcuWlmSetValue: Name (%s) not found in Class (%s)\n",
	     widget_name, widget_class) ;
    XtWarning (error_text) ;
    return ;
    }

/* TODO: for now just do it on the first one */
convert_string_to_arg (ww,
	get_resource_info (XtClass(name_list->widget_id_list->id),
			   resource_name, XcuWlmResourceInfoRepresentation),
	resource_name, resource_value, &arg) ;
XtSetValues (name_list->widget_id_list->id, &arg, ONE) ;
my_app_sync (XtWidgetToApplicationContext ((Widget) ww)) ;
return ;
}

/***** **** *** ** * XcuWlmGetValue * ** *** **** *****/

void
XcuWlmGetValue (ww, widget_class, widget_name, resource_name, target)
    XcuWlmWidget ww ;
    String widget_class ;
    String widget_name ;
    String resource_name ;
    XPointer target ;
{
Arg arg ;
WidgetClassList *class_list ;
WidgetNameList *name_list ;

if (!XtIsSubclass ((Widget)ww, (WidgetClass)xcuWlmWidgetClass)) {
    XtError("XcuWlmGetValue requires arg0 to be subclass of xcuWlmWidgetClass");
    }
class_list = get_widget_class_list (&ww->wlm.widget_class_list,
	      XrmStringToQuark(widget_class), XcuWlmNullOnNotFound) ;
if (class_list == NULL)
    {
    sprintf (error_text, "XcuWlmGetValue: Class (%s) not found\n", widget_class) ;
    XtWarning (error_text) ;
    return ;
    }

name_list = get_widget_name_list (class_list, widget_name, XcuWlmNullOnNotFound);
if (name_list == NULL)
    {
    sprintf (error_text, "XcuWlmGetValue: Name (%s) not found in Class (%s)\n",
	     widget_name, widget_class) ;
    XtWarning (error_text) ;
    return ;
    }

arg.name = resource_name ;
arg.value = (XtArgVal) target ;
/* TODO: for now just inquire of the first one */
XtGetValues (name_list->widget_id_list->id, &arg, ONE) ;

return ;
}

/**
 ** private functions to the public functions
 **/

static void
add_to_sample_list (ww, widget_class, callback_widget)
    XcuWlmWidget ww ;
    String widget_class ;
    Widget callback_widget ;
{
WidgetClassList *class_list ;
XtProc procedure ;

/* TODO: all strings to quarks */
/*
 * TODO: for each widget class, have an entry for the sample procedure.
 *       no reason to do a full scale search every time
 */
class_list = get_widget_class_list (&ww->wlm.widget_class_list,
	      XrmStringToQuark(widget_class), XcuWlmCroakOnNotFound) ;
procedure = get_proc_list (class_list->proc_list, "Sample") ;
if (procedure)
    {
    XcuWlmSampleList *sample_list ;
    XcuWlmSampleList *save ;

    sample_list = (XcuWlmSampleList *) XtMalloc (sizeof (XcuWlmSampleList)) ;
    sample_list->sample_proc = procedure ;
    sample_list->id = callback_widget ;
    save = ww->wlm.sample_list ;
    ww->wlm.sample_list = sample_list ;
    sample_list->next = save ;
    }
else
    {
    sprintf (error_text, "%s\n%s: (%s)\n",
	"No sample routine in the widget class for which",
	"a sample has been requested",
	widget_class) ;
    }
return ;
}

static Widget
get_callback_widget (ww, widget_class, widget_name, callback_name)
    XcuWlmWidget ww ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
{
Widget id = get_widget_id (ww, widget_class, widget_name) ;
XtCallbackStatus status = XtHasCallbacks (id, callback_name) ;
if (status == XtCallbackNoList)
    {
    sprintf (error_text, "Widget (%s/%s) does not have a callback (%s)\n",
    widget_class, widget_name, callback_name) ;
    XtWarning (error_text) ;
    return NULL ;
    }
return id ;
}

static void
get_int_float_string (ww, widget_class, widget_name,
			   callback_name, target,
			   text, get_type, wlm_type)
    XcuWlmWidget ww ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
    XPointer *target ;
    String text ;
    XcuWlmGetType get_type ;
    XcuWlmConnectionType wlm_type ;
{
XcuWlmGetRecord *record ;
Widget callback_widget ;

callback_widget = get_callback_widget (ww, widget_class, widget_name,
					callback_name) ;
if (callback_widget != NULL)
    {
    record = (XcuWlmGetRecord *) XtMalloc (sizeof (XcuWlmGetRecord)) ;
    record->type = get_type ;
    record->target = (XPointer) target ;
    record->ww = ww ;
    record->wlm_type = wlm_type ;
    XtAddCallback (callback_widget, callback_name,
		   callback_get, (XPointer) record) ;
    if ((int)wlm_type & (int)XcuWLM_SAMPLING)
	{
	add_to_sample_list (ww, widget_class, callback_widget) ;
	}
    }
else
    {
    callback_not_found_warning (text, widget_class, widget_name,
				callback_name) ;
    }

return ;
}

static void
callback_indirect (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmIndirectRecord *record = (XcuWlmIndirectRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmIndirectQueue,
		   w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmIndirectQueue,
		       w, client, call) ;
    (*record->callback_proc) (w, record->client_data, call) ;
    record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_channel (client, channel, id)
    XPointer client ;
    int *channel ;
    XtInputId *id ;
{
XcuWlmChannelRecord *record = (XcuWlmChannelRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmChannelQueue,
		   client, channel, id) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmChannelQueue,
		       client, channel, id) ;
    (*record->callback_proc) (client, channel, id) ;
    record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_set_float (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmSetFloatRecord *record = (XcuWlmSetFloatRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmSetFloatQueue,
		   w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmSetFloatQueue,
		       w, client, call) ;
    *(record->target) = record->value ;
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS))
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_set_bool (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmSetBoolRecord *record = (XcuWlmSetBoolRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmSetBoolQueue,
		   w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmSetBoolQueue,
		       w, client, call) ;
    *(record->target) = record->value ;
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS))
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_set_int (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmSetIntRecord *record = (XcuWlmSetIntRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmSetIntQueue, w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmSetIntQueue,
		       w, client, call) ;
    *(record->target) = record->value ;
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS))
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_set_string (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmSetStringRecord *record = (XcuWlmSetStringRecord *) client ;

/*** If we could trust the client programmer not to pass us a garbage
 *** pointer initially, this would make sure that a lot of useless 
 *** Strings are not lying about.  But it will surely be disaster,
 *** since that kind of bug is extremely difficult to track down.
 *** (I could stick another field in XcuWlmSetStringRecord that keeps
 ***  track of the number of times executed and free all strings after
 ***  the first...)
if (*(record->target))
    XtFree (*(record->target)) ;
***
***/
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmSetStringQueue,
		   w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmSetStringQueue,
		       w, client, call) ;
    Xcu_copy_ds (&(*(record->target)), record->value) ;
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS))
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_get (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmGetRecord *record = (XcuWlmGetRecord *) client ;
if (record->ww->wlm.sre_status == NULL && !record->ww->wlm.sampling_active)
    {
    /* save this action if there are no requests oustanding */
    /* and if sampling is not active */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmGetQueue, w, client, call) ;
    }
else
    {
    union {
	  XPointer caddr;
	  int	  i ;
	  float   f ;
	  String  s ;
	  } uni ;
    uni.caddr = call ;

    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmGetQueue,
		       w, client, call) ;

    switch (record->type)
      {
      case XcuWlmGetBoolType :
	{
	*((Boolean *) record->target) = (Boolean) uni.i ;
	break ;
	}
      case XcuWlmGetIntType :
	{
	*((int *) record->target) = uni.i ;
	break ;
	}
      case XcuWlmGetFloatType :
	{
	*((float *) record->target) = uni.f ;
	break ;
	}
      case XcuWlmGetStringType :
	{
	Xcu_copy_ds (&(*((String *) record->target)), uni.s) ;
	break ;
	}
      }
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS) &&
	!record->ww->wlm.sampling_active)
	{
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
	}
    }

return ;
}

static void
callback_scan (w, client, call)
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmScanRecord *record = (XcuWlmScanRecord *) client ;
if (record->ww->wlm.sre_status == NULL)
    {
    /* save this action if there are no requests oustanding */
    enqueue_event (&record->ww->wlm.queue_ptr, XcuWlmScanQueue, w, client, call) ;
    }
else
    {
    if (record->ww->wlm.history_mode == XcuWlmRecordHistory)
	enqueue_event (&record->ww->wlm.history_ptr, XcuWlmScanQueue,
		       w, client, call) ;
    sscanf (*((String *) call), record->format,
	record->t0, record->t1, record->t2, record->t3,
	record->t4, record->t5, record->t6, record->t7,
	record->t8, record->t9, record->t10, record->t11,
	record->t12, record->t13, record->t14, record->t15,
	record->t16, record->t17, record->t18, record->t19) ;
    if (!((int)record->wlm_type & (int)XcuWLM_BYPASS))
	record->ww->wlm.sre_status->status = XcuWlmRequestSatisfied ;
    }

return ;
}

static void
callback_not_found_warning (func, widget_class, widget_name, callback_name)
    String func ;
    String widget_class ;
    String widget_name ;
    String callback_name ;
{
sprintf (error_text, "%s%s(),%s\n%s(%s)\n%s(%s)\n%s(%s)\n",
"While doing a ", func, " the following callback was not found:",
"    Widget Class  : ", widget_class,
"    Widget Name   : ", widget_name,
"    Callback Name : ", callback_name) ;
XtWarning (error_text) ;
return ;
}

static void
enqueue_event (wlm_queue_ptr, type, w, client, call)
    XcuWlmQueueRecord **wlm_queue_ptr ;
    XcuWlmQueueType type ;
    Widget w ;
    XPointer client ;
    XPointer call ;
{
XcuWlmQueueRecord *save ;
XcuWlmQueueRecord *queue_record ;

queue_record = (XcuWlmQueueRecord *) XtMalloc (sizeof (XcuWlmQueueRecord)) ;
queue_record->type = type ;
queue_record->w = w ;
queue_record->client = client ;
queue_record->call = call ;
save = *wlm_queue_ptr ;
*wlm_queue_ptr = queue_record ;
queue_record->next = save ;

return ;
}

static void
dequeue_event (ww)
    XcuWlmWidget ww ;
{
XcuWlmQueueRecord *record = ww->wlm.queue_ptr ;
XcuWlmSreRecord *save ;

if (record == NULL)
    {
    XtError ("??? Attempt to dequeue from an empty queue ???\n") ;
    }

ww->wlm.queue_ptr = record->next ;

save = ww->wlm.sre_status ;
ww->wlm.sre_status = (XcuWlmSreRecord *) XtMalloc (sizeof (XcuWlmSreRecord)) ;
ww->wlm.sre_status->status = XcuWlmRequestNotSatisfied ;
ww->wlm.sre_status->next = save ;

switch (record->type)
  {
  case XcuWlmChannelQueue :
    {
    callback_channel (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmIndirectQueue :
    {
    callback_indirect (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmSetFloatQueue :
    {
    callback_set_float (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmSetIntQueue :
    {
    callback_set_int (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmSetStringQueue :
    {
    callback_set_string (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmGetQueue :
    {
    callback_get (record->w, record->client, record->call) ;
    break ;
    }
  case XcuWlmScanQueue :
    {
    callback_scan (record->w, record->client, record->call) ;
    break ;
    }
  default :
    {
    }
  }

XtFree (record) ;
save = ww->wlm.sre_status ;
ww->wlm.sre_status = ww->wlm.sre_status->next ;
XtFree (save) ;

return ;
}


static void
my_app_sync (app)
    XtAppContext app ;
{
while (XtAppPending(app))
    {
    XEvent event ;

    XtAppNextEvent (app, &event) ;
    XtDispatchEvent (&event) ;
    }
return ;
}

