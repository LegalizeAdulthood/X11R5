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

/* XcuWlm widget private definitions */

#ifndef _Xcu_WlmP_h
#define _Xcu_WlmP_h

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <X11/CompositeP.h>	/* Superclass Private Include File	*/
#include <X11/Xcu/Wlm.h>		/* Class Public Include File		*/
#include <X11/Xcu/ProcedureR.h>
#include <X11/Xcu/BmgrR.h>

#define TAG_CLASS_ENTRY(ww,tag,wclass) \
	{ \
	XcuWlmTagClassList *save = ww->wlm.tag_class_list ; \
	ww->wlm.tag_class_list = (XcuWlmTagClassList *) \
				 XtMalloc (sizeof (XcuWlmTagClassList)) ; \
	ww->wlm.tag_class_list->quark = XrmStringToQuark (tag) ; \
	ww->wlm.tag_class_list->class = wclass ; \
	ww->wlm.tag_class_list->next = save ; \
	}

/*
 * Prelude
 */

typedef void (*XcuProc) () ;

typedef enum {
	     XcuWlmCroakOnNotFound, XcuWlmAddOnNotFound, XcuWlmNullOnNotFound
	     } XcuWlmNotFoundAction ;
typedef enum {
	     XcuWlmStringArg, XcuWlmCharArg, XcuWlmFloatArg, XcuWlmArgValArg
	     } XcuWlmArgType ;
typedef enum {
	     XcuWlmLocalContext, XcuWlmGlobalContext
	     } XcuWlmContext ;
typedef enum {
	     XcuWlmGetIntType, XcuWlmGetFloatType,
	     XcuWlmGetStringType, XcuWlmGetBoolType
	     } XcuWlmGetType ;
typedef enum {
	     XcuWlmRequestNotSatisfied, XcuWlmRequestSatisfied
	     } XcuWlmSREstatus ;
typedef enum {
	     XcuWlmInactiveHistory, XcuWlmRecordHistory
	     } XcuWlmHistoryMode ;
typedef enum {
	     XcuWlmIndirectQueue,  XcuWlmSetFloatQueue, XcuWlmSetIntQueue,
	     XcuWlmSetStringQueue, XcuWlmGetQueue,      XcuWlmScanQueue,
	     XcuWlmSetBoolQueue, XcuWlmChannelQueue
	     } XcuWlmQueueType ;

typedef enum {
	     XcuWlmResourceInfoClass,  XcuWlmResourceInfoRepresentation
	     } XcuWlmResourceInfoType ;

typedef struct XcuWlmManageItem
    {
    String widget ;
    String type ;
    String value ;
    } XcuWlmManageItem ;

typedef struct XcuWlmSreRecord
    {
    XcuWlmSREstatus status ;
    struct XcuWlmSreRecord *next ;
    } XcuWlmSreRecord ;

typedef struct XcuWlmQueueRecord
    {
    XcuWlmQueueType type ;
    Widget w ;
    caddr_t client ;
    caddr_t call ;
    struct XcuWlmQueueRecord *next ;
    } XcuWlmQueueRecord ;

typedef struct
    {
    String name ;
    Widget id ;
    } XcuWlmWidgetList ;

typedef struct XcuWlmTagClassList
    {
    XrmQuark quark ;
    WidgetClass class ;
    struct XcuWlmTagClassList *next ;
    } XcuWlmTagClassList ;

/* TODO: make bigger temps in menu.y,main.c and then allocate and transfer */
/* or save/null method */

#define MAX_DIRECTIVE_ARGUMENTS 10
#define MAX_DIRECTIVE_CALLDATA_ITEMS 10
#define MAX_DIRECTIVE_CALLDATA_INDICES 2

typedef union
    {
    Widget widget ;
    String string ;
    } UValues ;

typedef enum
    {
     XcuWlmDirectiveEquivalence
    ,XcuWlmDirectiveNonEquivalence
    ,XcuWlmDirectiveGreaterThan
    ,XcuWlmDirectiveLessThan
    ,XcuWlmDirectiveGreaterThanOrEqualTo
    ,XcuWlmDirectiveLessThanOrEqualTo
    } XcuWlmDirectiveOperator ;

typedef struct XcuWlmSampleList
    {
    XcuProc sample_proc ;
    Widget id ;
    struct XcuWlmSampleList *next ;
    } XcuWlmSampleList ;

typedef struct
    {
    XtCallbackProc callback_proc ;
    caddr_t  client_data ;
    XcuWlmWidget ww ;
    } XcuWlmIndirectRecord ;

typedef struct
    {
    XtCallbackProc callback_proc ;
    caddr_t  client_data ;
    XcuWlmWidget ww ;
    } XcuWlmChannelRecord ;

typedef struct
    {
    float *target ;
    float  value ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmSetFloatRecord ;

typedef struct
    {
    Boolean *target ;
    Boolean  value ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmSetBoolRecord ;

typedef struct
    {
    int *target ;
    int  value ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmSetIntRecord ;

typedef struct
    {
    String *target ;
    String  value ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmSetStringRecord ;

typedef struct
    {
    XcuWlmGetType type ;
    caddr_t    target ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmGetRecord ;

typedef struct
    {
    String format ;
    caddr_t t0,  t1,  t2,  t3,  t4,  t5,  t6,  t7,  t8,  t9,
	    t10, t11, t12, t13, t14, t15, t16, t17, t18, t19 ;
    XcuWlmWidget ww ;
    XcuWlmConnectionType wlm_type ;
    } XcuWlmScanRecord ;

typedef struct XcuWlmCallbackList
    {
    Widget   widget_id ;
    XrmQuark widget_name_quark ;
    XrmQuark widget_class_quark ;
    XrmQuark callback_name_quark ;
    struct XcuWlmCallbackList *next ;
    } XcuWlmCallbackList ;

typedef struct Resource
    {
    String name ;
    String class ;
    String value ;
    XcuWlmContext context ;
    } Resource ;

typedef struct Directive
    {
    String callback_name ;
    String *call_data_converter ;
    String *call_data ;
    XcuWlmDirectiveOperator *call_data_operator ;
    int **call_data_index ;
    int n_call_comparisons ;
    Cardinal *n_call_indices ;
    String target_class ;
    String target_name ;
    Resource resource ;
    String procedure ;
    Cardinal n_arguments ;
    String *argument_converters ;
    String *argument_strings ;
    long client_data[3] ;
    } Directive ;

typedef struct Fetus
    {
    WidgetClass class ;
    XrmQuark class_name ;
    String widget_name ;
    Cardinal n_resources ;
    Resource *resources ;
    Cardinal n_directives ;
    Directive *directives ;
    Cardinal n_children ;
    struct Fetus *children ;
    Cardinal n_manage_list ;
    XcuWlmManageItem *manage_list ;
    Widget id ;
    } Fetus ;

typedef struct XcuWlmProcedureList
    {
    XrmQuark	         quark ;
    XcuProc	  	 procedure ;
    struct XcuWlmProcedureList *next ;
    } XcuWlmProcedureList ;

typedef struct WidgetIdList
    {
    Widget	   id ;
    struct WidgetIdList   *next ;
    } WidgetIdList ;

typedef struct WidgetNameList
    {
    XrmQuark	   quark ;
    WidgetIdList   *widget_id_list ;
    struct WidgetNameList *next ;
    } WidgetNameList ;

typedef struct WidgetClassList
    {
    XrmQuark	    quark ;
    WidgetNameList  *widget_name_list ;
    XcuWlmProcedureList	 *proc_list ;
    struct WidgetClassList *next ;
    } WidgetClassList ;

/*
 * The new fields added to superclass widget structure
 */

typedef struct _XcuWlmPart
    {
    /* resources */
    String layout_file ;
    XtCallbackList expose_callbacks;

    /* private state */
    XrmQuark conversion_class ;
    Cardinal parent_depth ;
    Cardinal cur_depth ;
    Cardinal max_depth ;
    XrmQuark *quarks_list ;
    XrmBinding *bindings_list ;
    XrmDatabase *database ;
    XcuWlmTagClassList *tag_class_list ;
    XcuWlmProcedureList *proc_list ;
    XcuWlmSampleList *sample_list ;
    WidgetClassList *widget_class_list ;
    XcuWlmSreRecord *sre_status ;
    XcuWlmQueueRecord *queue_ptr ;
    XcuWlmQueueRecord *history_ptr ;
    XcuWlmHistoryMode history_mode ;
    Boolean sampling_active ;
    Boolean first_device ;
    Boolean kill_request ;
    Fetus *child ;
    long event_stamp ;
    } XcuWlmPart ;

/*
 * The complete widget instance structure for this widget
 */

typedef struct _XcuWlmRec
    {
    CorePart		core ;
    CompositePart	composite ;
    XcuWlmPart		wlm ;
    } XcuWlmRec ;

typedef struct
    {
    int not_used ;
    } XcuWlmClassPart ;

/*
 * The complete widget class structure for the widget
 */

typedef struct _XcuWlmClassRec
    {
    CoreClassPart	core_class ;
    CompositeClassPart	composite_class ;
    XcuWlmClassPart	wlm_class ;
    } XcuWlmClassRec ;

/*
 * The name of a "constant" of the generic widget class structure
 */

extern XcuWlmClassRec xcuWlmClassRec ;

#endif /* _Xcu_WlmP_h */
