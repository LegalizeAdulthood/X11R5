/* -*-C-*-
********************************************************************************
*
* File:         w_resources.c
* RCS:          $Header: w_resources.c,v 1.15 91/03/24 18:22:43 mayer Exp $
* Description:  WINTERP interfaces to Motif and Xtoolkit resources
* Author:       Niels Mayer, HPLabs
* Created:      Sat Jul 22 04:42:12 1989
* Modified:     Fri Oct  4 20:16:24 1991 (Niels Mayer) mayer@hplnpm
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
*
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_resources.c,v 1.15 91/03/24 18:22:43 mayer Exp $";

/*
 * <limits.h> defines machine dependent limits on sizes of numbers, if your
 * machine doesn't have this, then your compiler doesn't conform to standards
 * XPG2, XPG3, POSIX.1, FIPS 151-1 and you should complain to the manufacturer.
 * 
 * If for some reason your system isn't standards-conforming, you may work
 * around this problem by using the following definitions (assuming 32 bit machine):
 * 
 * #define USHRT_MAX 65535
 * #define SHRT_MIN (-32768)
 * #define SHRT_MAX 32767
 * #define INT_MAX 2147483647
 * #define INT_MIN (-2147483647 - 1)
 * #define LONG_MIN (-2147483647 - 1)
 */
#include <limits.h>

#include <stdio.h>
#include <Xm/XmP.h>		/* need XmP.h, rather than Xm.h because we need to access object.parent and some core fields directly */
#include <X11/ShellP.h>		/* need ShellP.h rather than Shell.h because we access ((ShellWidget) widgetID)->core */

#include "winterp.h"		/* this must occur before doing "#ifdef WINTERP_MOTIF_11" */

#ifdef WINTERP_MOTIF_11
#include <Xm/MwmUtil.h>
#else				/* Motif 1.0 */
#include <X11/MwmUtil.h>
#include <Xm/Text.h>		/* needed to define XmTextScanType for "XmRSelectionArray" -- in Motif 1.1, this has moved to <Xm/Xm.h> */
#endif				/* WINTERP_MOTIF_11 */

#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_XmString.h"

#ifdef HP_GRAPH_WIDGET
#include <Xm/Graph.h>
#include <Xm/Arc.h>
#endif				/* HP_GRAPH_WIDGET */

extern char temptext[];	/* from winterp.c */
static void Wres_Free_This_Later();

/* these are used by :get_child methods on various widget classes. */
LVAL s_XmDIALOG_WORK_AREA,	/* FileSB SelectioB */
  s_XmDIALOG_APPLY_BUTTON,	/* SelectioB MessageB FileSB */
  s_XmDIALOG_CANCEL_BUTTON,	/* FileSB MessageB SelectioB */
  s_XmDIALOG_DEFAULT_BUTTON,	/* FileSB MessageB SelectioB */
  s_XmDIALOG_OK_BUTTON,		/*FileSB MessageB SelectioB  */
#ifdef WINTERP_MOTIF_11
  s_XmDIALOG_DIR_LIST,		/* FileSB */
  s_XmDIALOG_DIR_LIST_LABEL,	/* FileSB */
#endif /* WINTERP_MOTIF_11 */
  s_XmDIALOG_FILTER_LABEL,	/* FileSB */
  s_XmDIALOG_FILTER_TEXT,	/* FileSB */
  s_XmDIALOG_HELP_BUTTON,	/* FileSB MessageB SelectioB */
  s_XmDIALOG_LIST,		/* FileSB SelectioB  */
  s_XmDIALOG_HISTORY_LIST,	/* Command */
  s_XmDIALOG_LIST_LABEL,	/* FileSB SelectioB */
  s_XmDIALOG_MESSAGE_LABEL,	/* MessageB */
  s_XmDIALOG_SELECTION_LABEL,	/* FileSB SelectioB */
  s_XmDIALOG_PROMPT_LABEL,	/* Command */
  s_XmDIALOG_SYMBOL_LABEL,	/* MessageB */
  s_XmDIALOG_TEXT,		/* FileSB SelectioB */
  s_XmDIALOG_COMMAND_TEXT,	/* Command */
  s_XmDIALOG_SEPARATOR;		/* FileSB MessageB SelectioB */



typedef union {
  XtPointer XtPointer_value;	/* XtPointer == Opaque == caddr_t == XtAccelerators, etc */
  XtEnum    XtEnum_value;
  Boolean   Boolean_value;
  Bool      Bool_value;
  int       int_value;
#ifdef WINTERP_MOTIF_11
  KeySym    KeySym_value;
  XmTextPosition XmTextPosition_value; /* XmTextPosition == long */
#else
  char      char_value;
#endif				/* WINTERP_MOTIF_11 */
  Dimension Dimension_value;
  Position  Position_value;
  short     short_value;
  String    String_value;
} GetValues_Union;

typedef struct _Resource_Enums {
  char*  printname;		/* name of enumerated resource value (a lisp keyword -- must be uppercase and begin w/ ':') */
  XtEnum c_value;		/* the Xtoolkit/Motif value */
  LVAL   lisp_value;		/* the Lisp symbol id associated w/ printname (set in Wres_Init() by proc Init_Enumerated_Type_Syms() */
} Resource_Enums;

typedef struct _Resource_Class {
  char*           XmR_type;	/* the XmR* resource representation -- the "type" of the Xt resource needed by XtConvert() */
  char            LVAL_type;	/* enumerated type -- the type of the LVAL datum to be set via XtSetValues */
  LVAL            (*resource_val_to_LVAL_converter)( /* GetValues_Union */ ); /* ptr to function that converts resource from C to Lisp (for XtGetValues) */
  XtArgVal        (*LVAL_to_resource_val_converter)( /*LVAL*/ ); /* ptr to function that converts resource from Lisp to C (for XtSetValues) */
  Resource_Enums* enums_alist;	/* for enumerated resources, this points to a NULL terminated array of MotifValue/LispValue pairs */
} Resource_Class;

typedef struct _Resource_Instance {
  char*           printname;	/* lisp printname for the resource */
  Resource_Class* class;	/* pointer to Resource_Class structure -- the value of 'symbol' in lisp */
  char*           name;		/* holds the XmN* name */
  LVAL            symbol;	/* holds the lisp symbol assoc'd w/ this resource -- set in Wres_Init() */
} Resource_Instance;


/*****************************************************************************
 * This procedure is the generic  C/Motif/Xt-enumerated-type to
 * Lisp-enumeration-symbol converter. The conversion is based on the
 * Resource_Enums* array that is set for each enumerated type resource class.
 ****************************************************************************/
static LVAL Cvt_XtEnum_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  register XtEnum value = res_val.XtEnum_value;
  register Resource_Enums* alist = &(resource->class->enums_alist[0]);

  while (alist->printname && (value != alist->c_value))
    alist++;
  if (alist->printname)		/* if not at end of alist, then (alist->c_value == value) */
    return (alist->lisp_value);	/* return the symbol LVAL assoc'd w/ value */
  else {			/* else give error message */
    sprintf(temptext,
	    "Cvt_XtEnum_to_LVAL() internal error: Could not convert value %lu retrieved by XtGetValues() to enumerated type XmR%s.",
	    (unsigned long) value,
	    resource->class->XmR_type);
    xlerror(temptext, resource->symbol); 
  }
}

/*****************************************************************************
 * Essentially same as above, except that the resource value expected is
 * declared as 'int' rather than 'unsigned char'. This procedure should
 * go away as soon as Xt/Motif resource declarations are made consistent.
 ****************************************************************************/
static LVAL Cvt_Enum_Int_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  register int value = res_val.int_value;
  register Resource_Enums* alist = &(resource->class->enums_alist[0]);

  while (alist->printname && (value != alist->c_value))
    alist++;
  if (alist->printname)		/* if not at end of alist, then (alist->c_value == value) */
    return (alist->lisp_value);	/* return the symbol LVAL assoc'd w/ value */
  else {			/* else give error message */
    sprintf(temptext,
	    "Cvt_XtEnum_to_LVAL() internal error: Could not convert value %lu retrieved by XtGetValues() to enumerated type XmR%s.",
	    (unsigned long) value,
	    resource->class->XmR_type);
    xlerror(temptext, resource->symbol); 
  }
}

/*****************************************************************************
 * This procedure is the generic Lisp-enumeration-symbol to
 * C/Motif/Xt-enumerated-type converter. The conversion is based on the
 * Resource_Enums* array that is set for each enumerated type resource class.
 ****************************************************************************/
static XtArgVal Cvt_LVAL_to_XtEnum(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  register Resource_Enums* alist = &(resource->class->enums_alist[0]);

  while (alist->printname && (lisp_val != alist->lisp_value))
    alist++;
  if (alist->printname)		/* if not at end of alist, then (alist->lisp_value == lisp_val) */
    return ((XtArgVal) alist->c_value);	/* return the C value assoc'd w/ lisp_val */
  else {			/* else give error message */
    char* cptr;
    sprintf(temptext,
	    "Note: Resources of type XmR%s may only be set to the following enumerated values:\n",
	    resource->class->XmR_type);
    errputstr(temptext);

    alist = &(resource->class->enums_alist[0]);
    cptr = &(temptext[0]);
    *cptr++ = '\t';
    *cptr++ = '[';
    while (alist->printname) {
      int len = strlen(alist->printname);
      strncpy(cptr, alist->printname, len);
      cptr += len;
      *cptr++ = ' ';
      alist++;
    }
    *cptr++ = ']';
    *cptr++ = '\n';
    *cptr++ = '\000';
    errputstr(temptext); 

    sprintf(temptext,
	    "Attempted to set resource %s of type XmR%s to invalid value (see note above).",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}


/*****************************************************************************
 * This procedure must be called in Wres_Init() for each Resource_Enums* array
 * set up in defining an enumerated resource.
 ****************************************************************************/
static void Init_Enumerated_Type_Syms(enums_alist)
     Resource_Enums* enums_alist;
{
  register Resource_Enums* alist = &(enums_alist[0]);
  while (alist->printname) {
    alist->lisp_value = xlenter(alist->printname);
    alist++;
  }
}

/*****************************************************************************
 * Wres_Check_Value_Against_Minimum() and Wres_Check_Value_Against_Maximum()
 * are called in converting from lisp to XmRDimension, XmRInt, 
 * XmRPosition, XmRShort in procedures Cvt_LVAL_to_XmR...()
 ****************************************************************************/
static void Wres_Check_Value_Against_Minimum(value, minimum, resource, lisp_val)
     long value;
     long minimum;
     Resource_Instance* resource;
     LVAL lisp_val;
{
  if (value < minimum) {
    sprintf(temptext,
	    "Value of resource %s (type XmR%s) must be a FIXNUM >= %ld.",
	    resource->printname,
	    resource->class->XmR_type,
	    minimum);
    xlerror(temptext, lisp_val);
  }
}

static void Wres_Check_Value_Against_Maximum(value, maximum, resource, lisp_val)
     long value;
     long maximum;
     Resource_Instance* resource;
     LVAL lisp_val;
{
  if (value > maximum) {
    sprintf(temptext,
	    "Value of resource %s (type XmR%s) must be a FIXNUM <= %ld.",
	    resource->printname,
	    resource->class->XmR_type,
	    maximum);
    xlerror(temptext, lisp_val);
  }
}


/**************************************************************************/
/****************** R E S O U R C E    C O N V E R T E R S ****************/
/**************************************************************************/


/******************************************************************************/
static LVAL Cvt_XmRAcceleratorTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (res_val.XtPointer_value ? cv_xtaccelerators((XtAccelerators) res_val.XtPointer_value) : NIL);
}

static XtArgVal Cvt_LVAL_to_XmRAcceleratorTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) get_xtaccelerators(lisp_val));
}

static Resource_Class resclass_XmRAcceleratorTable = {
  XmRAcceleratorTable,		
  XLTYPE_XtAccelerators,
  Cvt_XmRAcceleratorTable_to_LVAL, 
  Cvt_LVAL_to_XmRAcceleratorTable}; /* sizeof (XtAccelerators) */



/******************************************************************************/
static Resource_Enums XmRAlignment_enums_alist[] = {
  {":ALIGNMENT_BEGINNING",	XmALIGNMENT_BEGINNING,		NULL},
  {":ALIGNMENT_CENTER",		XmALIGNMENT_CENTER,		NULL},
  {":ALIGNMENT_END",		XmALIGNMENT_END,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRAlignment = {
  XmRAlignment,	
  SYMBOL,			/* XmRAlignment is an enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRAlignment_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRArrowDirection_enums_alist[] = {
  {":ARROW_UP",		XmARROW_UP,	NULL},
  {":ARROW_DOWN",	XmARROW_DOWN,	NULL},
  {":ARROW_LEFT",	XmARROW_LEFT,	NULL},
  {":ARROW_RIGHT",	XmARROW_RIGHT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRArrowDirection = {
  XmRArrowDirection,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRArrowDirection_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRAttachment_enums_alist[] = {
  {":ATTACH_NONE",		XmATTACH_NONE,		NULL},
  {":ATTACH_FORM",		XmATTACH_FORM,		NULL},
  {":ATTACH_OPPOSITE_FORM",	XmATTACH_OPPOSITE_FORM,	NULL},
  {":ATTACH_WIDGET",		XmATTACH_WIDGET,	NULL},
  {":ATTACH_OPPOSITE_WIDGET",	XmATTACH_OPPOSITE_WIDGET, NULL},
  {":ATTACH_POSITION",		XmATTACH_POSITION,	NULL},
  {":ATTACH_SELF",		XmATTACH_SELF,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRAttachment = {
  XmRAttachment,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRAttachment_enums_alist
  };



/******************************************************************************/
/*
 * Resource representation type XmRBool is essentially the same as boolean.
 * It is only used for the resource XmNinput.
 * Resource instances of type XmRBool unfortunately cannot be thrown into
 * resource class XmRBoolean because type Boolean=='unsigned char' while
 * type Bool=='int'. If XmRBool was treated as XmRBoolean, then doing :GET_VALUES
 * on such a resource would retrieve invalid values.
 *
 * This entire resource class should be thrown away if and when Motif and Xt get
 * some more consistently named/typed resources.
 */
static LVAL Cvt_XmRBool_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  extern LVAL true;
  if (res_val.Bool_value)
    return (true);
  else
    return (NIL);
}

static XtArgVal Cvt_LVAL_to_XmRBool(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  if (lisp_val == NIL)
    return ((XtArgVal) ((Bool) FALSE));
  else
    return ((XtArgVal) ((Bool) TRUE));
}

static Resource_Class resclass_XmRBool = {
  XmRBool,
  FREE,			/* any kind of LVAL is ok here, if == NIL, then false, else true. */
  Cvt_XmRBool_to_LVAL, 
  Cvt_LVAL_to_XmRBool}; /* sizeof(Bool) == sizeof(int) */


/******************************************************************************/
#ifdef WINTERP_MOTIF_11
/*
 * Resource representation type XmRBooleanDimension is essentially the same as
 * boolean. In Motif 1.1, it is only used for the resource XmNshowAsDefault.
 *
 * Resource instances of type XmRBooleanDimension unfortunately cannot be thrown
 * into resource class XmRBoolean because type Boolean=='unsigned char' while
 * XmRBooleanDimension is sizeof(Dimension)=='unsigned short'. If
 * XmRBooleanDimension was treated as XmRBoolean, then doing :GET_VALUES
 * on such a resource would retrieve invalid values.
 *
 * This entire resource class should be thrown away if and when Motif and Xt get
 * some more consistently named/typed resources.
 */
static LVAL Cvt_XmRBooleanDimension_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  extern LVAL true;
  if (res_val.Dimension_value)
    return (true);
  else
    return (NIL);
}

static XtArgVal Cvt_LVAL_to_XmRBooleanDimension(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  if (lisp_val == NIL)
    return ((XtArgVal) ((Dimension) FALSE));
  else
    return ((XtArgVal) ((Dimension) TRUE));
}

static Resource_Class resclass_XmRBooleanDimension = {
  XmRBooleanDimension,
  FREE,			/* any kind of LVAL is ok here, if == NIL, then false, else true. */
  Cvt_XmRBooleanDimension_to_LVAL, 
  Cvt_LVAL_to_XmRBooleanDimension}; /* sizeof(Dimension) == sizeof(unsigned short) */
#endif				/* WINTERP_MOTIF_11 */



/******************************************************************************/
static LVAL Cvt_XmRBoolean_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  extern LVAL true;
  if (res_val.Boolean_value)
    return (true);
  else
    return (NIL);
}

static XtArgVal Cvt_LVAL_to_XmRBoolean(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  if (lisp_val == NIL)
    return ((XtArgVal) ((Boolean) FALSE));
  else
    return ((XtArgVal) ((Boolean) TRUE));
}

static Resource_Class resclass_XmRBoolean = {
  XmRBoolean,
  FREE,			/* any kind of LVAL is ok here, if == NIL, then false, else true. */
  Cvt_XmRBoolean_to_LVAL, 
  Cvt_LVAL_to_XmRBoolean}; /* sizeof(Boolean) == sizeof(char) */



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static LVAL Cvt_XmRButtonTypeTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  /*
   * XmRButtonType resource XmNbuttonType is a create-time only resource.
   * it cannot be retrieved via :GET_VALUES.
   */
  sprintf(temptext,
	  "Motif disallows :GET_VALUES on resource of type XmR%s.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol);
}

static Resource_Enums XmRButtonType_enums_alist[] = {
  {":PUSHBUTTON",	XmPUSHBUTTON,		NULL},
  {":TOGGLEBUTTON",	XmTOGGLEBUTTON,		NULL},
  {":CHECKBUTTON",	XmCHECKBUTTON,		NULL},
  {":RADIOBUTTON",	XmRADIOBUTTON,		NULL},
  {":CASCADEBUTTON",	XmCASCADEBUTTON,	NULL},
  {":SEPARATOR",	XmSEPARATOR,		NULL},
  {":DOUBLE_SEPARATOR",	XmDOUBLE_SEPARATOR,	NULL},
  {":TITLE",		XmTITLE,		NULL},
  {NULL, NULL, NULL}
};

/*
 * This procedure was mutated from Cvt_LVAL_to_XtEnum(). Couldn't
 * use Cvt_LVAL_to_XtEnum() directly because we need to call XtFree()
 * on the <typetable> parameter before signaling an error.
 *
 * Note: this is very similar to Cvt_LVAL_to_XmTextScanType()
 * if changing/fixing this, fix the other. These should be merged, but I'm lazy.
 */
static XmButtonType Cvt_LVAL_to_XmRButtonType(lisp_val, resource, typetable)
     LVAL               lisp_val;
     Resource_Instance* resource;
     XmButtonTypeTable typetable;
{
  register Resource_Enums* alist = &(XmRButtonType_enums_alist[0]);

  while (alist->printname && (lisp_val != alist->lisp_value))
    alist++;
  if (alist->printname)		/* if not at end of alist, then (alist->lisp_value == lisp_val) */
    return ((XmButtonType) alist->c_value); /* return the C value assoc'd w/ lisp_val */
  else {			/* else give error message */
    char* cptr;

    XtFree(typetable);		/* must free the XmButtonTypeTable created below in Cvt_LVAL_to_XmRButtonType() */

    sprintf(temptext,
	    "Note: Resources of type XmR%s may only be set to the following enumerated values:\n",
	    resource->class->XmR_type);
    errputstr(temptext);

    alist = &(XmRButtonType_enums_alist[0]);
    cptr = &(temptext[0]);
    *cptr++ = '\t';
    *cptr++ = '[';
    while (alist->printname) {
      int len = strlen(alist->printname);
      strncpy(cptr, alist->printname, len);
      cptr += len;
      *cptr++ = ' ';
      alist++;
    }
    *cptr++ = ']';
    *cptr++ = '\n';
    *cptr++ = '\000';
    errputstr(temptext); 

    sprintf(temptext,
	    "Attempted to set resource %s of type XmR%s to invalid value (see note above).",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

#define XMBUTTONTYPETABLE_SIZE_INCREMENT 20
static XtArgVal Cvt_LVAL_to_XmRButtonTypeTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  int size, i;
  XmButtonTypeTable typetable;

  /*
   * if argument is a vector, then step through array, converting each
   * lisp button-type symbol to a motif unsigned char enumeration.
   */
  if (vectorp(lisp_val)) {
    size = getsize(lisp_val);
    typetable = (XmButtonTypeTable) XtMalloc((unsigned) (size * sizeof(XmButtonType)));

    for (i = 0; i < size; i++)
      typetable[i] = Cvt_LVAL_to_XmRButtonType(getelement(lisp_val, i), resource, typetable); /* will free typetable if this proc. signalls an error */

    Wres_Free_This_Later((XtPointer) typetable, XtFree); /* free typetable after the resource is set. */
    return ((XtArgVal) typetable);
  }
  /*
   * if argument is a list, then cdr through list converting each
   * lisp button-type symbol to a motif unsigned char enumeration.
   */
  else if (consp(lisp_val)) {
    size = XMBUTTONTYPETABLE_SIZE_INCREMENT;
    typetable = (XmButtonTypeTable) XtMalloc((unsigned) (size * sizeof(XmButtonType)));

    for (i = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), i++) {
      if (i >= size) {		/* make sure it'll fit into allocated typetable */
	size += XMBUTTONTYPETABLE_SIZE_INCREMENT;
	typetable = (XmButtonTypeTable) XtRealloc(typetable, (unsigned) (size * sizeof(XmButtonType)));	/* will free typetable if this proc. signalls an error */
      }
      typetable[i] = Cvt_LVAL_to_XmRButtonType(car(lisp_val), resource, typetable);
    }
    if (lisp_val) {		/* if loop terminated due to list pointer not being a CONS cell */
      XtFree(typetable);
      sprintf(temptext,
	      "Resource %s (type XmR%s) expected a list of ButtonType keywords --found invalid XmButtonTypeTable list element.",
	      resource->printname,
	      resource->class->XmR_type);
      xlerror(temptext, lisp_val);
    }
    Wres_Free_This_Later((XtPointer) typetable, XtFree); /* free typetable after the resource is set. */
    return ((XtArgVal) typetable);
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else {
    sprintf(temptext,
	    "Resource %s (type XmR%s) expected a list or vector of ButtonType keywords.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

static Resource_Class resclass_XmRButtonTypeTable = {
  XmRButtonType,	
  FREE,				/* can either be a vector or a list */
  Cvt_XmRButtonTypeTable_to_LVAL, 
  Cvt_LVAL_to_XmRButtonTypeTable}; /* sizeof(XmButtonTypeTable) == sizeof(XmButtonType *) == sizeof(unsigned char *) */
#endif /* WINTERP_MOTIF_11 */



/******************************************************************************/
static LVAL Cvt_XmRCallback_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  sprintf(temptext,
	  "Method :GET_VALUES not supported on resources of type XmR%s.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol); 
}

static XtArgVal Cvt_LVAL_to_XmRCallback(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  sprintf(temptext,
	  "Setting resources of type XmR%s not supported. Use methods :SET_CALLBACK or :ADD_CALLBACK instead.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol); 
}

static Resource_Class resclass_XmRCallback = {
  XmRCallback,
  FREE,				/* should be XLTYPE_XtCallbackList; use FREE here so that above error messages are used, rather than "In Widget Arglist: expected resource value of type Callback. - (FOO)" */
  Cvt_XmRCallback_to_LVAL,
  Cvt_LVAL_to_XmRCallback};	/* sizeof(XtCallbackList) which
				   is a pointer to a struct holding a *fn()
				   and the closure data. */



/********************************************************************************/
/*
 * Motif 1.1 BUGFIX: 1.1 defines resources XmNmnemonic and XmNoptionMnemonic
 * as XmRKeySym. Functionally, however, a character value is used here.
 * Note however that the character value must be  sizeof(KeySym), thus for
 * retrieval, we use a new element in union GetValues_Union -- KeySym_value.
 */
#ifdef WINTERP_MOTIF_11

static LVAL Cvt_XmRKeySym_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvchar((int) res_val.KeySym_value));
}

static XtArgVal Cvt_LVAL_to_XmRKeySym(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) ((KeySym) getchcode(lisp_val)));
}

static Resource_Class resclass_XmRKeySym = {
  XmRKeySym,
  CHAR,	
  Cvt_XmRKeySym_to_LVAL,		/* use cvchar of ascii value of character */
  Cvt_LVAL_to_XmRKeySym};		/* sizeof(KeySym) == sizeof(XID) == sizeof(long) */

#else				/* MOTIF 1.0 */

static LVAL Cvt_XmRChar_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvchar((int) res_val.char_value));
}

static XtArgVal Cvt_LVAL_to_XmRChar(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) ((char) getchcode(lisp_val)));
}

static Resource_Class resclass_XmRChar = {
  XmRChar,
  CHAR,	
  Cvt_XmRChar_to_LVAL,		/* use cvchar of ascii value of character */
  Cvt_LVAL_to_XmRChar};		/* sizeof(char) */

#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static LVAL Cvt_XmRKeySymTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  /*
   * XmRKeySymTable resource XmNbuttonMnemonics  is a create-time only resource.
   * it cannot be retrieved via :GET_VALUES.
   */
  sprintf(temptext,
	  "Cannot do :GET_VALUES on resource of type XmR%s.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol);
}

#define KEYSYMTABLE_SIZE_INCREMENT 20
static XtArgVal Cvt_LVAL_to_XmRKeySymTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  int size, i;
  XmKeySymTable keysymtable;
  LVAL elt;

  /*
   * if argument is a vector, then step through array, converting each
   * lisp <char> to a C KeySym.
   */
  if (vectorp(lisp_val)) {
    size = getsize(lisp_val);
    keysymtable = (XmKeySymTable) XtMalloc((unsigned) (size * sizeof(KeySym)));

    for (i = 0; i < size; i++) {
      elt = getelement(lisp_val, i);
      if (charp(elt))
	keysymtable[i] = (KeySym) getchcode(elt);
      else {
	XtFree(keysymtable);
	sprintf(temptext,
		"Resource %s (type XmR%s) expects a sequence of CHARs -- found a non CHAR element in array.",
		resource->printname,
		resource->class->XmR_type);
	xlerror(temptext, elt);
      }
    }
    Wres_Free_This_Later((XtPointer) keysymtable, XtFree); /* free keysymtable after the resource is set. */
    return ((XtArgVal) keysymtable);
  }
  /*
   * if argument is a list, then cdr through list converting each
   * lisp <char> to a C KeySym.
   */
  else if (consp(lisp_val)) {
    size = KEYSYMTABLE_SIZE_INCREMENT;
    keysymtable = (XmKeySymTable) XtMalloc((unsigned) (size * sizeof(KeySym)));

    for (i = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), i++) {
      if (i >= size) {		/* make sure it'll fit into allocated keysymtable */
	size += KEYSYMTABLE_SIZE_INCREMENT;
	keysymtable = (XmKeySymTable) XtRealloc(keysymtable, (unsigned) (size * sizeof(KeySym)));
      }
      elt = car(lisp_val);
      if (charp(elt))
	keysymtable[i] = (KeySym) getchcode(elt);
      else {
	XtFree(keysymtable);
	sprintf(temptext,
		"Resource %s (type XmR%s) expects a sequence of CHARs -- found a non CHAR element in list.",
		resource->printname,
		resource->class->XmR_type);
	xlerror(temptext, elt);
      }
    }

    if (lisp_val) {		/* if loop terminated due to list pointer not being a CONS cell */
      XtFree(keysymtable);
      sprintf(temptext,
	      "Resource %s (type XmR%s) expects a sequence of CHARs -- a non CHAR list-sequence was given.",
	      resource->printname,
	      resource->class->XmR_type);
      xlerror(temptext, lisp_val);
    }

    Wres_Free_This_Later((XtPointer) keysymtable, XtFree); /* free keysymtable after the resource is set. */
    return ((XtArgVal) keysymtable);
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else {
    sprintf(temptext,
	    "Resource %s (type XmR%s) expects a sequence/list of CHARs.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

static Resource_Class resclass_XmRKeySymTable = {
  XmRKeySymTable,		/* note that this resource was declared as XmRPointer, but that's not specific enuf... */
  FREE,				/* can either be a vector or a list */
  Cvt_XmRKeySymTable_to_LVAL, 
  Cvt_LVAL_to_XmRKeySymTable};	/* sizeof(XmKeySymTable) */
#endif				/* WINTERP_MOTIF_11 */

/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static Resource_Enums XmRCommandWindowLocation_enums_alist[] = {
  {":COMMAND_ABOVE_WORKSPACE",	XmCOMMAND_ABOVE_WORKSPACE,	NULL},
  {":COMMAND_BELOW_WORKSPACE",	XmCOMMAND_BELOW_WORKSPACE,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRCommandWindowLocation = {
  XmRCommandWindowLocation,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRCommandWindowLocation_enums_alist
  };
#endif /* WINTERP_MOTIF_11 */



/********************************************************************************/
static Resource_Enums XmRDefaultButtonType_enums_alist[] = {
  {":DIALOG_OK_BUTTON",		XmDIALOG_OK_BUTTON,	NULL},
  {":DIALOG_CANCEL_BUTTON",	XmDIALOG_CANCEL_BUTTON, NULL},
  {":DIALOG_HELP_BUTTON",	XmDIALOG_HELP_BUTTON,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRDefaultButtonType = {
  XmRDefaultButtonType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRDefaultButtonType_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRDeleteResponse_enums_alist[] = {
  {":DESTROY",		XmDESTROY,	NULL},
  {":UNMAP",		XmUNMAP,	NULL},
  {":DO_NOTHING",	XmDO_NOTHING,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRDeleteResponse = {
  XmRDeleteResponse,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRDeleteResponse_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRDialogStyle_enums_alist[] = {
#ifndef WINTERP_MOTIF_11
  {":DIALOG_WORK_AREA",		XmDIALOG_WORK_AREA,		NULL},
  {":DIALOG_MODELESS",		XmDIALOG_MODELESS,		NULL},
  {":DIALOG_APPLICATION_MODAL",	XmDIALOG_APPLICATION_MODAL,	NULL},
  {":DIALOG_SYSTEM_MODAL",	XmDIALOG_SYSTEM_MODAL,		NULL},
#else
  {":DIALOG_MODELESS",		XmDIALOG_MODELESS,		NULL},
  {":DIALOG_PRIMARY_APPLICATION_MODAL",	XmDIALOG_PRIMARY_APPLICATION_MODAL, NULL},
  /*
   * In motif 1.1, XmDIALOG_APPLICATION_MODAL
   * "is for compatibility only. Its use is deprecated.
   * Since this is obsolete, and conflicts with
   * XmDIALOG_PRIMARY_APPLICATION_MODAL, we obsolete it early.
   *
   {":DIALOG_APPLICATION_MODAL",	XmDIALOG_APPLICATION_MODAL,	NULL}
   */
  {":DIALOG_FULL_APPLICATION_MODAL",	XmDIALOG_FULL_APPLICATION_MODAL, NULL},
  {":DIALOG_SYSTEM_MODAL",		XmDIALOG_SYSTEM_MODAL,		NULL},
  /*
   * In Motif 1.1, XmDIALOG_WORK_AREA has same value (0) as XmDIALOG_MODELESS
   * which means that if you set :DIALOG_WORK_AREA, method :GET_VALUES will
   * retrieve :DIALOG_MODELESS (due to ordering...)
   */
  {":DIALOG_WORK_AREA",			XmDIALOG_WORK_AREA,		NULL},
#endif
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRDialogStyle = {
  XmRDialogStyle,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRDialogStyle_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRDialogType_enums_alist[] = {
#ifndef WINTERP_MOTIF_11
  {":DIALOG_ERROR",		XmDIALOG_ERROR,		NULL},
  {":DIALOG_INFORMATION",	XmDIALOG_INFORMATION,	NULL},
  {":DIALOG_MESSAGE",		XmDIALOG_MESSAGE,	NULL},
  {":DIALOG_QUESTION",		XmDIALOG_QUESTION,	NULL},
  {":DIALOG_WARNING",		XmDIALOG_WARNING,	NULL},
  {":DIALOG_WORKING",		XmDIALOG_WORKING,	NULL},
  {":DIALOG_PROMPT",		XmDIALOG_PROMPT,	NULL},
  {":DIALOG_SELECTION",		XmDIALOG_SELECTION,	NULL},
  {":DIALOG_COMMAND",		XmDIALOG_COMMAND,	NULL},
#else
  /*
   * MOTIF 1.1 is buggy (what else is new :-(), as it defines multiple symbols
   * with same values.
   * From Xm.h:
   * -- defines for dialog type --
   * #define XmDIALOG_ERROR        1
   * #define XmDIALOG_INFORMATION  2
   * #define XmDIALOG_MESSAGE      3
   * #define XmDIALOG_QUESTION     4
   * #define XmDIALOG_WARNING      5
   * #define XmDIALOG_WORKING      6
   * -- Defines for selection dialog type: --
   * #define XmDIALOG_WORK_AREA      0
   * #define XmDIALOG_PROMPT         1
   * #define XmDIALOG_SELECTION      2
   * #define XmDIALOG_COMMAND        3
   * #define XmDIALOG_FILE_SELECTION 4
   *
   * Stopgap solution is to have method :GET_VALUES return value
   * :DIALOG_ERROR for :DIALOG_PROMPT
   * :DIALOG_INFORMATION for :DIALOG_SELECTION
   * :DIALOG_MESSAGE for :DIALOG_COMMAND
   *
   * Note that for backwards compatibility and sensible naming
   * of resources, I am still allowing method :SET_VALUES to be
   * given values :DIALOG_PROMPT, :DIALOG_SELECTION, and :DIALOG_COMMAND. Note
   * that this will cause a bug for people expecting the values to be eq... 
   */
 {":DIALOG_ERROR",	XmDIALOG_ERROR,		NULL},
 {":DIALOG_INFORMATION",XmDIALOG_INFORMATION,	NULL},
 {":DIALOG_MESSAGE",	XmDIALOG_MESSAGE,	NULL},
 {":DIALOG_QUESTION",	XmDIALOG_QUESTION,	NULL},
 {":DIALOG_WARNING",	XmDIALOG_WARNING,	NULL},
 {":DIALOG_WORKING",	XmDIALOG_WORKING,	NULL},
  /* Motif 1.1 bug -- this value of enumerated type XmRDialogType conflicts w/ XmDIALOG_ERROR */
 {":DIALOG_PROMPT",	XmDIALOG_PROMPT,	NULL},
  /*  Motif 1.1 bug -- this value of enumerated type XmRDialogType conflicts w/ XmDIALOG_INFORMATION */
 {":DIALOG_SELECTION",	XmDIALOG_SELECTION,	NULL},
  /* Motif 1.1 bug -- this value of enumerated type XmRDialogType conflicts w/ XmDIALOG_MESSAGE */
 {":DIALOG_COMMAND",	XmDIALOG_COMMAND,	NULL},
  /* Motif 1.1 bug -- this value of enumerated type XmRDialogType conflicts w/ XmDIALOG_QUESTION */
 {":DIALOG_FILE_SELECTION", XmDIALOG_FILE_SELECTION, NULL},
#endif /* WINTERP_MOTIF_11 */
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRDialogType = {
  XmRDialogType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRDialogType_enums_alist
  };



/********************************************************************************/
static LVAL Cvt_XmRDimension_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvfixnum((FIXTYPE) res_val.Dimension_value));
}

static XtArgVal Cvt_LVAL_to_XmRDimension(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  long value = (long) getfixnum(lisp_val); 

  /* The Motif 1.0 Xtoolkit intrinsics list Dimension as being the following:
          typedef Cardinal Dimension; -- Size in pixels -- (where Cardinal == unsigned int)
     In Motif 1.1, this has changed to 
          typedef unsigned short Dimension; -- Size in pixels --

     I'm going with the 'unsigned short' -- for backwards compatibility w/ Motif 1.0, it won't
     matter if the range of XmRDimension is limited to the range of unsigned shorts...  */
  
  Wres_Check_Value_Against_Minimum(value, 0L, resource, lisp_val);
  Wres_Check_Value_Against_Maximum(value, (long) USHRT_MAX, resource, lisp_val); /* USHRT_MAX is max val of unsigned short from <limits.h> */

  return ((XtArgVal) ((Dimension) value));
}

#ifdef WINTERP_MOTIF_11

static Resource_Class resclass_XmRVerticalDimension = {
  XmRVerticalDimension,	
  FIXNUM,			/* note that only positive FIXNUMS ok */
  Cvt_XmRDimension_to_LVAL, 
  Cvt_LVAL_to_XmRDimension};	/* sizeof(Dimension) == sizeof(unsigned short) */

static Resource_Class resclass_XmRHorizontalDimension = {
  XmRHorizontalDimension,	
  FIXNUM,			/* note that only positive FIXNUMS ok */
  Cvt_XmRDimension_to_LVAL, 
  Cvt_LVAL_to_XmRDimension};	/* sizeof(Dimension) == sizeof(unsigned short) */

#else /* MOTIF 1.0 */

static Resource_Class resclass_XmRDimension = {
  XmRDimension,	
  FIXNUM,			/* note that only positive FIXNUMS ok */
  Cvt_XmRDimension_to_LVAL, 
  Cvt_LVAL_to_XmRDimension};	/* sizeof(Dimension) == sizeof(unsigned int) */

#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
/*
 * NOTE WORKAROUND TO MOTIF BUG: this enumeration type can't use the general
 * enumeration converters Cvt_LVAL_to_XtEnum() and Cvt_XtEnum_to_LVAL()
 * because bugs in the source of Motif 1.0 and 1.1 declare this resource rep type
 * 'int'. All other enumerated types are declared as 'XtEnum' == 'unsigned char'
 */
static Resource_Enums XmREditMode_enums_alist[] = {
  {":MULTI_LINE_EDIT",	XmMULTI_LINE_EDIT,	NULL},
  {":SINGLE_LINE_EDIT",	XmSINGLE_LINE_EDIT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmREditMode = {
  XmREditMode,	
  SYMBOL,			/* enumerated type */
  Cvt_Enum_Int_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(int) !! -- note: this is a Motif BUG v 1.0 and 1.1 */
  XmREditMode_enums_alist
  };



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static Resource_Enums XmRFileTypeMask_enums_alist[] = {
  {":FILE_DIRECTORY",	XmFILE_DIRECTORY,	NULL},
  {":FILE_REGULAR",	XmFILE_REGULAR,		NULL},
  {":FILE_ANY_TYPE",	XmFILE_ANY_TYPE,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRFileTypeMask = {
  XmRFileTypeMask,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRFileTypeMask_enums_alist
  };
#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
static LVAL Cvt_XmRFontList_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  sprintf(temptext,
	  "Resource %s -- XmR%s --> Lisp type converter not implemented.",
	  resource->printname,
	  resource->class->XmR_type);
  xlfail(temptext);
}
static XtArgVal Cvt_LVAL_to_XmRFontList(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  /*
    Note that all XmFontList resources are copied into the Motif
    widget's space and thereafter managed by that widget. That
    means we don't need to worry about making an additional
    reference to the lisp XmFontList value that was passed in.
    When the lisp XmString is no longer referenced, it will
    be freed via garbage collection. 
    */
  sprintf(temptext,
	  "Resource %s -- Lisp-->XmR%s type converter not implemented.",
	  resource->printname,
	  resource->class->XmR_type);
  xlerror(temptext, lisp_val);
}
static Resource_Class resclass_XmRFontList = {
  XmRFontList,	
  XLTYPE_XmFontList,
  Cvt_XmRFontList_to_LVAL, 
  Cvt_LVAL_to_XmRFontList};	/* sizeof (XmFontList) */



/********************************************************************************/
static Resource_Enums XmRIndicatorType_enums_alist[] = {
  {":N_OF_MANY",	XmN_OF_MANY,	NULL},
  {":ONE_OF_MANY",	XmONE_OF_MANY,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRIndicatorType = {
  XmRIndicatorType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRIndicatorType_enums_alist
  };



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
/*
 * Need to include <X11/StringDefs.h> because Motif 1.1 <Xm/Xm.h> forgot
 * to define resource representation type XtRInitialState/XmRInitialState.
 */
#include <X11/StringDefs.h>

/*
 * NOTE WORKAROUND TO Xt BUG: this enumeration type can't use the general
 * enumeration converters Cvt_LVAL_to_XtEnum() and Cvt_XtEnum_to_LVAL()
 * because bugs in the source of the X11r4 Xt/Shell.c declare this resource
 * rep type 'int'. All other enumerated types are declared as
 * 'XtEnum' == 'unsigned char'
 */
static Resource_Enums XtRInitialState_enums_alist[] = {
  {":WITHDRAWN_STATE",	WithdrawnState,	NULL},
  {":NORMAL_STATE",	NormalState,	NULL},
  {":ICONIC_STATE",	IconicState,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XtRInitialState = {
  XtRInitialState,
  SYMBOL,			/* enumerated type */
  Cvt_Enum_Int_to_LVAL,
  Cvt_LVAL_to_XtEnum,		/* sizeof(int)!! -- note: this is a BUG in Xt/Shell.c */
  XtRInitialState_enums_alist
  };
#endif /* WINTERP_MOTIF_11 */


/********************************************************************************/
static LVAL Cvt_XmRInt_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvfixnum((FIXTYPE) res_val.int_value));
}

static XtArgVal Cvt_LVAL_to_XmRInt(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  long value = (long) getfixnum(lisp_val); 

  Wres_Check_Value_Against_Minimum(value, (long) INT_MIN, resource, lisp_val); /* INT_MIN is min val of int from <limits.h> */
  Wres_Check_Value_Against_Maximum(value, (long) INT_MAX, resource, lisp_val); /* INT_MAX is max val of int from <limits.h> */

  return ((XtArgVal) ((int) value));
}

static Resource_Class resclass_XmRInt = {
  XmRInt,
  FIXNUM,
  Cvt_XmRInt_to_LVAL,
  Cvt_LVAL_to_XmRInt};		/* sizeof(int) */

#ifdef WINTERP_MOTIF_11

static Resource_Class resclass_XmRHorizontalInt = {
  XmRHorizontalInt,
  FIXNUM,
  Cvt_XmRInt_to_LVAL,
  Cvt_LVAL_to_XmRInt};		/* sizeof(int) */

static Resource_Class resclass_XmRVerticalInt = {
  XmRVerticalInt,
  FIXNUM,
  Cvt_XmRInt_to_LVAL,
  Cvt_LVAL_to_XmRInt};		/* sizeof(int) */

#endif				/* WINTERP_MOTIF_11 */

/********************************************************************************/
static Resource_Enums XmRKeyboardFocusPolicy_enums_alist[] = {
  {":EXPLICIT",		XmEXPLICIT,	NULL},
  {":POINTER",		XmPOINTER,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRKeyboardFocusPolicy = {
  XmRKeyboardFocusPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRKeyboardFocusPolicy_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRLabelType_enums_alist[] = {
  {":PIXMAP",	XmPIXMAP,	NULL},
  {":STRING",	XmSTRING,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRLabelType = {
  XmRLabelType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRLabelType_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRListSizePolicy_enums_alist[] = {
  {":VARIABLE",			XmVARIABLE,		NULL},
  {":CONSTANT",			XmCONSTANT,		NULL},
  {":RESIZE_IF_POSSIBLE",	XmRESIZE_IF_POSSIBLE,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRListSizePolicy = {
  XmRListSizePolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRListSizePolicy_enums_alist
  };



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static Resource_Enums XmRMultiClick_enums_alist[] = {
  {":MULTICLICK_DISCARD",	XmMULTICLICK_DISCARD,	NULL},
  {":MULTICLICK_KEEP",		XmMULTICLICK_KEEP,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRMultiClick = {
  XmRMultiClick,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRMultiClick_enums_alist
  };
#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static Resource_Enums XmRNavigationType_enums_alist[] = {
#ifdef WINTERP_MOTIF_111	/* a new navigation type symbol added in 1.1.1 */
  {":DYNAMIC_DEFAULT_TAB_GROUP",XmDYNAMIC_DEFAULT_TAB_GROUP,NULL},
#endif WINTERP_MOTIF_111
  {":NONE",			XmNONE,			NULL},
  {":TAB_GROUP",		XmTAB_GROUP,		NULL},
  {":STICKY_TAB_GROUP",		XmSTICKY_TAB_GROUP,	NULL},
  {":EXCLUSIVE_TAB_GROUP",	XmEXCLUSIVE_TAB_GROUP,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRNavigationType = {
  XmRNavigationType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRNavigationType_enums_alist
  };
#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
static Resource_Enums XmROrientation_enums_alist[] = {
  {":NO_ORIENTATION",	XmNO_ORIENTATION,	NULL},
  {":VERTICAL",		XmVERTICAL,		NULL},
  {":HORIZONTAL",	XmHORIZONTAL,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmROrientation = {
  XmROrientation,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmROrientation_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRPacking_enums_alist[] = {
  {":NO_PACKING",	XmNO_PACKING,	NULL},
  {":PACK_TIGHT",	XmPACK_TIGHT,	NULL},
  {":PACK_COLUMN",	XmPACK_COLUMN,	NULL},
  {":PACK_NONE",	XmPACK_NONE,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRPacking = {
  XmRPacking,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRPacking_enums_alist
  };



/********************************************************************************/
static LVAL Cvt_XmRPixel_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cv_pixel((Pixel) res_val.XtPointer_value));
}

static XtArgVal Cvt_LVAL_to_XmRPixel(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) get_pixel(lisp_val));
}

static Resource_Class resclass_XmRPixel = {
  XmRPixel,
  XLTYPE_Pixel,
  Cvt_XmRPixel_to_LVAL, 
  Cvt_LVAL_to_XmRPixel};	/* sizeof(Pixel) */


/********************************************************************************/
static LVAL Cvt_Pixmap_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (res_val.XtPointer_value ? cv_pixmap((Pixmap) res_val.XtPointer_value) : NIL);
}

static XtArgVal Cvt_LVAL_to_Pixmap(lisp_val, resource, o_widget)
     LVAL lisp_val;		/* XLTYPE_Pixmap */
     Resource_Instance* resource;
     LVAL               o_widget;
{
  extern void Wpm_Set_Pixmap_Reference(); /* w_pixmap.c */
  
  /* pixmap is referenced inside widget, so don't allow pixmap to be garbage
     collected & XmDestroyPixmap()'d until it is no longer referenced by
     anybody. */
  Wpm_Set_Pixmap_Reference(lisp_val, o_widget, resource->symbol);

  return ((XtArgVal) get_pixmap(lisp_val));
}

#ifdef WINTERP_MOTIF_11
static Resource_Class resclass_XmRXmBackgroundPixmap = {
  XmRXmBackgroundPixmap,
  XLTYPE_Pixmap,
  Cvt_Pixmap_to_LVAL,
  Cvt_LVAL_to_Pixmap};	/* sizeof(Pixmap) == sizeof(unsigned long) */
#else				/* MOTIF 1.0 */
static Resource_Class resclass_XmRPixmap = {
  XmRPixmap,
  XLTYPE_Pixmap,
  Cvt_Pixmap_to_LVAL,
  Cvt_LVAL_to_Pixmap};	/* sizeof(Pixmap) == sizeof(unsigned long) */
#endif				/* WINTERP_MOTIF_11 */

static Resource_Class resclass_ForegroundPixmap = {
  "ForegroundPixmap",		/* == XmRManForegroundPixmap + XmRPrimForegroundPixmap + XmRGadgetPixmap */
  XLTYPE_Pixmap,
  Cvt_Pixmap_to_LVAL,
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_HighlightPixmap = {
  "HighlightPixmap",		/* == XmRPrimHighlightPixmap + XmRManHighlightPixmap */
  XLTYPE_Pixmap,
  Cvt_Pixmap_to_LVAL,
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_TopShadowPixmap = {
  "TopShadowPixmap",		/* == XmRPrimTopShadowPixmap + XmRManTopShadowPixmap */
  XLTYPE_Pixmap,
  Cvt_Pixmap_to_LVAL,
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_BottomShadowPixmap = {
  "BottomShadowPixmap",		/* == XmRPrimBottomShadowPixmap + XmRManBottomShadowPixmap */
  XLTYPE_Pixmap, 
  Cvt_Pixmap_to_LVAL, 
  Cvt_LVAL_to_Pixmap};



/********************************************************************************/
static LVAL Cvt_XmRPosition_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvfixnum((FIXTYPE) res_val.Position_value));
}

static XtArgVal Cvt_LVAL_to_XmRPosition(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  long value = (long) getfixnum(lisp_val);

  /* The Motif 1.0 Xtoolkit intrinsics list Position as being the following:
          typedef int Position;  -- Offset from 0 coordinate --
     In Motif 1.1, this has changed to 
          typedef short Position; -- Offset from 0 coordinate --

     I'm going with the 'short' -- for backwards compatibility w/ Motif 1.0, it won't
     matter if the range of XmRPosition is limited to the range of shorts...  */

  Wres_Check_Value_Against_Minimum(value, (long) SHRT_MIN, resource, lisp_val);
  Wres_Check_Value_Against_Maximum(value, (long) SHRT_MAX, resource, lisp_val);

  return ((XtArgVal) ((Position) value));
}

#ifdef WINTERP_MOTIF_11
static Resource_Class resclass_XmRHorizontalPosition = {
  XmRHorizontalPosition,
  FIXNUM,
  Cvt_XmRPosition_to_LVAL,
  Cvt_LVAL_to_XmRPosition};	/* sizeof(Position): in Motif 1.1 == sizeof(short) */
static Resource_Class resclass_XmRVerticalPosition = {
  XmRVerticalPosition,
  FIXNUM,
  Cvt_XmRPosition_to_LVAL,
  Cvt_LVAL_to_XmRPosition};	/* sizeof(Position): in Motif 1.1 == sizeof(short) */
#else				/* MOTIF 1.0 */
static Resource_Class resclass_XmRPosition = {
  XmRPosition,
  FIXNUM,
  Cvt_XmRPosition_to_LVAL,
  Cvt_LVAL_to_XmRPosition};	/* sizeof(Position): in Motif 1.0 == sizeof(int) */
#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
static Resource_Enums XmRProcessingDirection_enums_alist[] = {
  {":MAX_ON_TOP",	XmMAX_ON_TOP,	NULL},
  {":MAX_ON_BOTTOM",	XmMAX_ON_BOTTOM,NULL},
  {":MAX_ON_LEFT",	XmMAX_ON_LEFT,	NULL},
  {":MAX_ON_RIGHT",	XmMAX_ON_RIGHT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRProcessingDirection = {
  XmRProcessingDirection,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRProcessingDirection_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRResizePolicy_enums_alist[] = {
  {":RESIZE_NONE",	XmRESIZE_NONE,	NULL},
  {":RESIZE_GROW",	XmRESIZE_GROW,	NULL},
  {":RESIZE_ANY",	XmRESIZE_ANY,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRResizePolicy = {
  XmRResizePolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRResizePolicy_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRRowColumnType_enums_alist[] = {
  {":WORK_AREA",	XmWORK_AREA,	NULL},
  {":MENU_BAR",		XmMENU_BAR,	NULL},
  {":MENU_PULLDOWN",	XmMENU_PULLDOWN,NULL},
  {":MENU_POPUP",	XmMENU_POPUP,	NULL},
  {":MENU_OPTION",	XmMENU_OPTION,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRRowColumnType = {
  XmRRowColumnType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRRowColumnType_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRScrollBarDisplayPolicy_enums_alist[] = {
  {":AS_NEEDED",	XmAS_NEEDED,	NULL},
  {":STATIC",		XmSTATIC,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRScrollBarDisplayPolicy = {
  XmRScrollBarDisplayPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRScrollBarDisplayPolicy_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRScrollBarPlacement_enums_alist[] = {
  {":TOP_LEFT",		XmTOP_LEFT,	NULL},
  {":TOP_RIGHT",	XmTOP_RIGHT,	NULL},
  {":BOTTOM_LEFT",	XmBOTTOM_LEFT,	NULL},
  {":BOTTOM_RIGHT",	XmBOTTOM_RIGHT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRScrollBarPlacement = {
  XmRScrollBarPlacement,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRScrollBarPlacement_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRScrollingPolicy_enums_alist[] = {
  {":AUTOMATIC",		XmAUTOMATIC,		NULL},
  {":APPLICATION_DEFINED",	XmAPPLICATION_DEFINED,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRScrollingPolicy = {
  XmRScrollingPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRScrollingPolicy_enums_alist
  };



/********************************************************************************/
static LVAL Cvt_XmRSelectionArray_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  /*
   * XmRSelectionArray resource XmNselectionArray cannot be retrieved
   * via :GET_VALUES because it isn't null terminated. To retrieve this,
   * create a method :GET_SELECTION_ARRAY on XM_TEXT_*_WIDGET_CLASS...
   */
  sprintf(temptext,
	  "Cannot do :GET_VALUES on resource of type XmR%s.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol);
}

static Resource_Enums XmTextScanType_enums_alist[] = {
  {":SELECT_POSITION",	XmSELECT_POSITION,	NULL},
  {":SELECT_WHITESPACE",XmSELECT_WHITESPACE,	NULL},
  {":SELECT_WORD",	XmSELECT_WORD,		NULL},
  {":SELECT_LINE",	XmSELECT_LINE,		NULL},
  {":SELECT_ALL",	XmSELECT_ALL,		NULL},
#ifdef WINTERP_MOTIF_11
  {":SELECT_PARAGRAPH",	XmSELECT_PARAGRAPH,	NULL},
#endif				/* WINTERP_MOTIF_11 */
  {NULL, NULL, NULL}
};

/*
 * This procedure was mutated from Cvt_LVAL_to_XtEnum(). Couldn't
 * use Cvt_LVAL_to_XtEnum() directly because we need to call XtFree()
 * on the <selarray> parameter before signaling an error.
 *
 * Note: this is very similar to Cvt_LVAL_to_XmRButtonType()
 * if changing/fixing this, fix the other. These should be merged, but I'm lazy.
 */
static XmTextScanType Cvt_LVAL_to_XmTextScanType(lisp_val, resource, selarray)
     LVAL               lisp_val;
     Resource_Instance* resource;
     XmTextScanType*    selarray;
{
  register Resource_Enums* alist = &(XmTextScanType_enums_alist[0]);

  while (alist->printname && (lisp_val != alist->lisp_value))
    alist++;
  if (alist->printname)		/* if not at end of alist, then (alist->lisp_value == lisp_val) */
    return ((XmTextScanType) alist->c_value); /* return the C value assoc'd w/ lisp_val */
  else {			/* else give error message */
    char* cptr;

    XtFree(selarray);		/* must free the selectionarray created below in Cvt_LVAL_to_XmRSelectionArray() */

    sprintf(temptext,
	    "Note: Resources of type XmR%s may only be set to the following enumerated values:\n",
	    resource->class->XmR_type);
    errputstr(temptext);

    alist = &(XmTextScanType_enums_alist[0]);
    cptr = &(temptext[0]);
    *cptr++ = '\t';
    *cptr++ = '[';
    while (alist->printname) {
      int len = strlen(alist->printname);
      strncpy(cptr, alist->printname, len);
      cptr += len;
      *cptr++ = ' ';
      alist++;
    }
    *cptr++ = ']';
    *cptr++ = '\n';
    *cptr++ = '\000';
    errputstr(temptext); 

    sprintf(temptext,
	    "Attempted to set resource %s of type XmR%s to invalid value (see note above).",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

#define SELECTIONARRAY_SIZE_INCREMENT 20
static XtArgVal Cvt_LVAL_to_XmRSelectionArray(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  int size, i;
  XmTextScanType* selarray;

  /*
   * if argument is a vector, then step through array, converting each
   * lisp text scan type symbol to a C XmTextScanType enum.
   */
  if (vectorp(lisp_val)) {
    size = getsize(lisp_val);
    selarray = (XmTextScanType*) XtMalloc((unsigned) (size * sizeof(XmTextScanType)));

    for (i = 0; i < size; i++)
      selarray[i] = Cvt_LVAL_to_XmTextScanType(getelement(lisp_val, i), resource, selarray); /* will free selarray if this proc. signalls an error */

    Wres_Free_This_Later((XtPointer) selarray, XtFree); /* free selarray after the resource is set. */
    return ((XtArgVal) selarray);
  }
  /*
   * if argument is a list, then cdr through list converting each
   * lisp text scan type symbol to a C XmTextScanType enum.
   */
  else if (consp(lisp_val)) {
    size = SELECTIONARRAY_SIZE_INCREMENT;
    selarray = (XmTextScanType*) XtMalloc((unsigned) (size * sizeof(XmTextScanType)));

    for (i = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), i++) {
      if (i >= size) {		/* make sure it'll fit into allocated selarray */
	size += SELECTIONARRAY_SIZE_INCREMENT;
	selarray = (XmTextScanType*) XtRealloc(selarray, (unsigned) (size * sizeof(XmTextScanType))); /* will free selarray if this proc. signalls an error */
      }
      selarray[i] = Cvt_LVAL_to_XmTextScanType(car(lisp_val), resource, selarray);
    }
    if (lisp_val) {		/* if loop terminated due to list pointer not being a CONS cell */
      XtFree(selarray);
      sprintf(temptext,
	      "Resource %s (type XmR%s) expected a list of XmTextScanType keywords -- found invalid XmRSelectionArray list element.",
	      resource->printname,
	      resource->class->XmR_type);
      xlerror(temptext, lisp_val);
    }
    Wres_Free_This_Later((XtPointer) selarray, XtFree); /* free selarray after the resource is set. */
    return ((XtArgVal) selarray);
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else {
    sprintf(temptext,
	    "Resource %s (type XmR%s) expected a list or vector of XmTextScanType keywords.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

static Resource_Class resclass_XmRSelectionArray = {
  "SelectionArray",		/* note that this resource was declared as XmRPointer, but that's not specific enuf... */
  FREE,				/* can either be a vector or a list */
  Cvt_XmRSelectionArray_to_LVAL, 
  Cvt_LVAL_to_XmRSelectionArray}; /* sizeof(XmTextScanType*) /



/********************************************************************************/
static Resource_Enums XmRSelectionPolicy_enums_alist[] = {
  {":SINGLE_SELECT",	XmSINGLE_SELECT,	NULL},
  {":MULTIPLE_SELECT",	XmMULTIPLE_SELECT,	NULL},
  {":EXTENDED_SELECT",	XmEXTENDED_SELECT,	NULL},
  {":BROWSE_SELECT",	XmBROWSE_SELECT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRSelectionPolicy = {
  XmRSelectionPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRSelectionPolicy_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRSeparatorType_enums_alist[] = {
  {":NO_LINE",			XmNO_LINE,		NULL},
  {":SINGLE_LINE",		XmSINGLE_LINE,		NULL},
  {":DOUBLE_LINE",		XmDOUBLE_LINE,		NULL},
  {":SINGLE_DASHED_LINE",	XmSINGLE_DASHED_LINE,	NULL},
  {":DOUBLE_DASHED_LINE",	XmDOUBLE_DASHED_LINE,	NULL},
  {":SHADOW_ETCHED_OUT",	XmSHADOW_ETCHED_OUT,	NULL},
  {":SHADOW_ETCHED_IN",		XmSHADOW_ETCHED_IN,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRSeparatorType = {
  XmRSeparatorType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRSeparatorType_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRShadowType_enums_alist[] = {
  {":SHADOW_ETCHED_IN",		XmSHADOW_ETCHED_IN,	NULL},
  {":SHADOW_ETCHED_OUT",	XmSHADOW_ETCHED_OUT,	NULL},
  {":SHADOW_IN",		XmSHADOW_IN,		NULL},
  {":SHADOW_OUT",		XmSHADOW_OUT,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRShadowType = {
  XmRShadowType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRShadowType_enums_alist
  };



/********************************************************************************/

static LVAL Cvt_XmRShort_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvfixnum((FIXTYPE) res_val.short_value));
}

static XtArgVal Cvt_LVAL_to_XmRShort(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  long value = (long) getfixnum(lisp_val); 

  Wres_Check_Value_Against_Minimum(value, (long) SHRT_MIN, resource, lisp_val);
  Wres_Check_Value_Against_Maximum(value, (long) SHRT_MAX, resource, lisp_val);

  return ((XtArgVal) ((short) value));
}

static Resource_Class resclass_XmRShort = {
  XmRShort,
  FIXNUM,
  Cvt_XmRShort_to_LVAL,
  Cvt_LVAL_to_XmRShort};	/* sizeof(short) */



/********************************************************************************/
/*
 * For getvalues, I'm assuming that we need to create our own copy of the string
 * in lisp and that the returned string is just a pointer into the widget's 
 * "memory space". I'm assuming that the string returned by getvalues does not
 * need to be deallocated.
 */
static LVAL Cvt_XmRString_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (res_val.String_value ? cvstring((char*) res_val.String_value) : NIL);
}

/* 
 * For setvalues and widget creation, I'm assuming that all string valued arguments
 * get copied in the widget, or are just temporarily referenced. Cases where this
 * doesn't happen should be considered a Motif bug. The reason to be concerned is
 * that Strings no longer being referenced are good candidates for being garbage
 * collected sometime later (the next time cons() or newnode() is called, for example).
 * Passing the string into setvalues/create  doesn't  count as a reference...
*/
static XtArgVal Cvt_LVAL_to_XmRString(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) ((String) getstring(lisp_val)));
}

static Resource_Class resclass_XmRString = {
  XmRString,
  STRING,
  Cvt_XmRString_to_LVAL,
  Cvt_LVAL_to_XmRString};	/* sizeof(String) == sizeof(char*) */

/*
 * A version of Cvt_LVAL_to_XmRString() which copies the string... needed
 * because XmNgeometry is a fucked up resource in Motif 1.1.
 */
static XtArgVal Cvt_LVAL_to_XmRString_for_XmNgeometry(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) XtNewString((String) getstring(lisp_val)));
}

static Resource_Class resclass_XmRString_for_XmNgeometry = {
  XmRString,
  STRING,
  Cvt_XmRString_to_LVAL,
  Cvt_LVAL_to_XmRString_for_XmNgeometry}; /* sizeof(String) == sizeof(char*) */



/********************************************************************************/
static Resource_Enums XmRStringDirection_enums_alist[] = {
  {":STRING_DIRECTION_L_TO_R",	XmSTRING_DIRECTION_L_TO_R,	NULL},
  {":STRING_DIRECTION_R_TO_L",	XmSTRING_DIRECTION_R_TO_L,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRStringDirection = {
  XmRStringDirection,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRStringDirection_enums_alist
  };



/********************************************************************************/
static LVAL Cvt_XmRStringTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  /*
   * XmRStringTable resources XmNbuttonAccelerators and XmNbuttonMnemonicCharSets
   * is a create-time only resource. They cannot be retrieved via :GET_VALUES.
   * XmNargv could be read, but we need to access XmNargc to find out how long
   * it is. Therefore access XmNargv/XmNargc via method :GET_ARGV
   * on APPLICATION_SHELL_WIDGET_CLASS.
   */
  sprintf(temptext, "Cannot do :GET_VALUES on resource of type XmR%s.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol);
}

#define STRINGTABLE_SIZE_INCREMENT 20
static XtArgVal Cvt_LVAL_to_XmRStringTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  int size, i;
  String* stringtable;
  LVAL elt;

  /*
   * if argument is a vector, then step through array of lisp <string>.
   */
  if (vectorp(lisp_val)) {
    size = getsize(lisp_val);
    stringtable = (String*) XtMalloc((unsigned) ((size + 1) * sizeof(String)));

    for (i = 0; i < size; i++) {
      elt = getelement(lisp_val, i);
      if (stringp(elt))
	stringtable[i] = (String) getstring(elt);
      else {
	XtFree(stringtable);
	sprintf(temptext,
		"Resource %s (type XmR%s) expects a sequence of STRINGs -- found a non STRING element in sequence.",
		resource->printname,
		resource->class->XmR_type);
	xlerror(temptext, elt);
      }
    }
    stringtable[i] = NULL;	/* null terminate */
    Wres_Free_This_Later((XtPointer) stringtable, XtFree); /* free stringtable after the resource is set. */
    return ((XtArgVal) stringtable);
  }
  /*
   * if argument is a list, then cdr through list of lisp <string>
   */
  else if (consp(lisp_val)) {
    size = STRINGTABLE_SIZE_INCREMENT;
    stringtable = (String*) XtMalloc((unsigned) ((size + 1) * sizeof(String)));

    for (i = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), i++) {
      if (i >= size) {		/* make sure it'll fit into allocated stringtable */
	size += STRINGTABLE_SIZE_INCREMENT;
	stringtable = (String*) XtRealloc(stringtable, (unsigned) ((size + 1) * sizeof(String)));
      }
      elt = car(lisp_val);
      if (stringp(elt))
	stringtable[i] = (String) getstring(elt);
      else {
	XtFree(stringtable);
	sprintf(temptext,
		"Resource %s (type XmR%s) expects a sequence of STRINGs -- found a non STRING element in sequence.",
		resource->printname,
		resource->class->XmR_type);
	xlerror(temptext, elt);
      }
    }

    if (lisp_val) {		/* if loop terminated due to list pointer not being a CONS cell */
      XtFree(stringtable);
      sprintf(temptext,
	      "Resource %s (type XmR%s) expects a sequence of STRINGs -- a non STRING sequence was given.",
	      resource->printname,
	      resource->class->XmR_type);
      xlerror(temptext, lisp_val);
    }

    stringtable[i] = NULL;	/* null terminate */
    Wres_Free_This_Later((XtPointer) stringtable, XtFree); /* free stringtable after the resource is set. */
    return ((XtArgVal) stringtable);
  }
  /*
   * if argument wasn't list or vector, then error
   */
  else {
    sprintf(temptext,
	    "Resource %s (type XmR%s) expects a sequence/list of STRINGs.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }
}

static Resource_Class resclass_XmRStringTable = {
  XmRStringTable,		/* note that this resource was declared as XmRPointer, but that's not specific enuf... */
  FREE,				/* can either be a vector or a list */
  Cvt_XmRStringTable_to_LVAL, 
  Cvt_LVAL_to_XmRStringTable};	/* sizeof(String*) */



/********************************************************************************/
#ifdef WINTERP_MOTIF_11
static LVAL Cvt_XmRTextPosition_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cvfixnum((FIXTYPE) res_val.XmTextPosition_value));
}

static XtArgVal Cvt_LVAL_to_XmRTextPosition(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  long value = (long) getfixnum(lisp_val); 

  /* XmTextPosition == long. Since FIXTYPE==long, there's no point
     in limiting the range. However, the first position in the text widget is 0, so we
     limit range to non-negative fixnums */

  Wres_Check_Value_Against_Minimum(value, 0L, resource, lisp_val);

  return ((XtArgVal) ((XmTextPosition) value));
}

static Resource_Class resclass_XmRTextPosition = {
  XmRTextPosition,	
  FIXNUM,			/* note that only non-negative FIXNUMS ok */
  Cvt_XmRTextPosition_to_LVAL, 
  Cvt_LVAL_to_XmRTextPosition};	/* sizeof(XmTextPosition) == sizeof(long) */

#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
static LVAL Cvt_XmRTranslationTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (cv_xttranslations((XtTranslations) res_val.XtPointer_value));
}

static XtArgVal Cvt_LVAL_to_XmRTranslationTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) get_xttranslations(lisp_val));
}

static Resource_Class resclass_XmRTranslationTable = {
  XmRTranslationTable,
  XLTYPE_XtTranslations,
  Cvt_XmRTranslationTable_to_LVAL,
  Cvt_LVAL_to_XmRTranslationTable}; /* sizeof(XtTranslations) */



/********************************************************************************/
static Resource_Enums XmRWhichButton_enums_alist[] = {
  {":BUTTON1",	Button1,	NULL},
  {":BUTTON2",	Button2,	NULL},
  {":BUTTON3",	Button3,	NULL},
  {":BUTTON4",	Button4,	NULL},
  {":BUTTON5",	Button5,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRWhichButton = {
  XmRWhichButton,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRWhichButton_enums_alist
  };



/********************************************************************************/
static LVAL Cvt_XmRWindow_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  return (res_val.XtPointer_value ? cv_window((Window) res_val.XtPointer_value) : NIL);
}

static XtArgVal Cvt_LVAL_to_XmRWindow(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  return ((XtArgVal) get_window(lisp_val));
}

static Resource_Class resclass_XmRWindow = {
  XmRWindow,
  XLTYPE_Window,
  Cvt_XmRWindow_to_LVAL, 
  Cvt_LVAL_to_XmRWindow};	/* sizeof(Window) == sizeof(XID) == sizeof(unsigned long) */


/********************************************************************************/
#ifdef WINTERP_MOTIF_11
/*
 * Note -- resource XmNwinGravity is defined as XmRInt, but it is really an
 * enumerated type -- one of 10 different specifiers for "window gravity".
 * This is a fake representation type -- there really isn't a "XmRWindowGravity"
 */
static Resource_Enums XmRWindowGravity_enums_alist[] = {
  {":FORGET_GRAVITY",		ForgetGravity,		NULL},
  {":NORTH_WEST_GRAVITY",	NorthWestGravity,	NULL},
  {":NORTH_GRAVITY",		NorthGravity,		NULL},
  {":NORTH_EAST_GRAVITY",	NorthEastGravity,	NULL},
  {":WEST_GRAVITY",		WestGravity,		NULL},
  {":CENTER_GRAVITY",		CenterGravity,		NULL},
  {":EAST_GRAVITY",		EastGravity,		NULL},
  {":SOUTH_WEST_GRAVITY",	SouthWestGravity,	NULL},
  {":SOUTH_GRAVITY",		SouthGravity,		NULL},
  {":SOUTH_EAST_GRAVITY",	SouthEastGravity,	NULL},
  {":STATIC_GRAVITY",		StaticGravity,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRWindowGravity = {
  "WindowGravity",	
  SYMBOL,			/* enumerated type */
  Cvt_Enum_Int_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(int) */
  XmRWindowGravity_enums_alist
  };
#endif				/* WINTERP_MOTIF_11 */



/********************************************************************************/
static LVAL Cvt_XmRWidget_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  return (Wcls_WidgetID_To_WIDGETOBJ((Widget) res_val.XtPointer_value));
}

static XtArgVal Cvt_LVAL_to_XmRWidget(lisp_val, resource, o_widget)
     LVAL lisp_val;		/* XLTYPE_WIDGETOBJ */
     Resource_Instance* resource;
     LVAL               o_widget;
{
  Widget widget_id;
  if (widget_id = get_widgetobj_widgetID(lisp_val))
    return ((XtArgVal) widget_id);
  else
    xlerror("WIDGETOBJ has been :destroy'd or hasn't been initialized by :isnew.", lisp_val);
}

static Resource_Class resclass_XmRWidget = {
  "Widget",			
  XLTYPE_WIDGETOBJ,
  Cvt_XmRWidget_to_LVAL,	/* cvt WidgetID back to widget object */
  Cvt_LVAL_to_XmRWidget};	/* cvt widget object to widget ID (sizeof(Widget))
				   make sure this has <widgetclass> in super */



/********************************************************************************/
static LVAL Cvt_XmRXmString_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
#ifdef WINTERP_MOTIF_11
#ifdef WINTERP_MOTIF_111	/* MOTIF 1.1.1 version */
  /*
   * Motif 1.1.1 documentation claims that XmStrings retrieved via XtGetValues()
   * are copies of the internal resource value, and that XmStringFree() must be
   * called by the application to free the copyied value. In WINTERP, by passing
   * the XmString pointer to cv_xmstring, we can be assured that the lisp-XmString
   * will get freed upon garbage collection when the node is no longer referenced.
   */
  return (res_val.XtPointer_value ? cv_xmstring((XmString) res_val.XtPointer_value) : NIL);
#else				/* MOTIF 1.1 version */
  /*
   * Motif 1.1's README states:
   * |        Compound String Resources Inconsistently Copied
   * | 
   * |        In this release, XtGetValues for a resource whose value is a
   * |        compound string sometimes does and sometimes does not copy
   * |        the returned string.  Following is a list of some known
   * |        resources whose XmString values are not copied (this list
   * |        may not be exhaustive):
   * | 
   * |             XmBulletinBoard     XmNdialogTitle
   * |             XmFileSelectionBox  XmNdirectory
   * |                                 XmNnoMatchString
   * |             XmRowColumn         XmNlabelString
   * |             XmScale             XmNtitleString
   *
   * Handling the above would require special casing for all XmRString resources,
   * which I don't have time to do. They fixed this stupidity in 1.1.1 (see above),
   * but certainly in a suboptimal fashion -- yet more wasteful copying occurs because
   * all widgets copy their internal XmStrings upon XtGetValues().
   *
   * The upshot of all this is that for Motif 1.1 (but not 1.1.1), there will be a memory
   * leak in XtGetValues() on XmString resources that copy the internal resource value.
   * We must make a copy here because it would be worse to XmStringFree() resources that
   * returned a pointer whose memory was managed by Motif.
   */
  return (res_val.XtPointer_value ? cv_xmstring(XmStringCopy((XmString) res_val.XtPointer_value)) : NIL);
#endif /* WINTERP_MOTIF_111 */
#else				/* MOTIF 1.0 version */
  /*
   * In Motif 1.0, XmStrings() returned via XtGetValues() were temporary pointers
   * that has to be copied via XmStringCopy()...
   */
  return (res_val.XtPointer_value ? cv_xmstring(XmStringCopy((XmString) res_val.XtPointer_value)) : NIL);
#endif /* WINTERP_MOTIF_11 */
}

static XtArgVal Cvt_LVAL_to_XmRXmString(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  /*
    Note that all XmString resources are copied into the Motif
    widget's space and thereafter managed by that widget. That
    means we don't need to worry about making an additional
    reference to the lisp XmString value that was passed in.
    When the lisp XmString is no longer referenced, it will
    be freed via garbage collection. 
  */
  return ((XtArgVal) get_xmstring(lisp_val));
}

static Resource_Class resclass_XmRXmString = {
  XmRXmString,
  XLTYPE_XmString,
  Cvt_XmRXmString_to_LVAL,
  Cvt_LVAL_to_XmRXmString};	/* sizeof(XmString) == sizeof(char*) */



/********************************************************************************/
static LVAL Cvt_XmRXmStringTable_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  /*
    For doing XtGetValues(), we need to look at the size of the created
    XmStringTable divided by the sizeof(XmString), and use this value
    to return a vector of XmStrings... We need to make copies of these
    since it is just a pointer to internal data which is managed internally.
    Unfortunately, since we can't find out the size of the XmStringTable,
    nor are XmStringTable's null terminated, we cannot implement this
    converter. -- it must be implemented as a method on the appropriate
    widget class. 
    */
  sprintf(temptext,
	  "Method :GET_VALUES not supported on resources of type XmR%s -- use method :GET_xxx_ITEMS method on appropriate widget class instead.",
	  resource->class->XmR_type);
  xlerror(temptext, resource->symbol); 
}

/*
 * For XmStringTable XtSetValues(), or for widget args during creation, the fn
 * Cvt_LVAL_to_XmRXmStringTable will convert a vector/list of strings/XmStrings
 * into an XmStringTable,  which is an array of XmStrings. Both the array and
 * it's XmStrings are copied into the widget upon :setvalues/:new and are
 * managed by the widget afterwards. Therefore, all the XmStrings created
 * here are eventually freed by calling Wres_Free_C_Arglist_Data().
 */
static XtArgVal Cvt_LVAL_to_XmRXmStringTable(lisp_val, resource, o_widget)
     LVAL               lisp_val;
     Resource_Instance* resource;
     LVAL               o_widget;
{
  SuperXmStringTable superstrtab = Wxms_Cvt_LispStringSequence_to_SuperXmStringTable(lisp_val);
  Wres_Free_This_Later((XtPointer) superstrtab, Wxms_Free_SuperXmStringTable);
  return ((XtArgVal) superstrtab->xmstrtab);
}

static Resource_Class resclass_XmRXmStringTable = {
  XmRXmStringTable,	
  FREE,				/* can either be a vector or a list */
  Cvt_XmRXmStringTable_to_LVAL, 
  Cvt_LVAL_to_XmRXmStringTable}; /* sizeof(XmStringTable) == sizeof(XmString*) */



/********************************************************************************/
static Resource_Enums XmRUnitType_enums_alist[] = {
  {":PIXELS",		XmPIXELS,		NULL},
  {":100TH_MILLIMETERS",Xm100TH_MILLIMETERS,	NULL},
  {":1000TH_INCHES",	Xm1000TH_INCHES,	NULL},
  {":100TH_POINTS",	Xm100TH_POINTS,		NULL},
  {":100TH_FONT_UNITS",	Xm100TH_FONT_UNITS,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRUnitType = {
  XmRUnitType,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRUnitType_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRVisualPolicy_enums_alist[] = {
  {":VARIABLE",		XmVARIABLE,	NULL},
  {":CONSTANT",		XmCONSTANT,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRVisualPolicy = {
  XmRVisualPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRVisualPolicy_enums_alist
  };



/********************************************************************************/
/*
 * Note: in Xm/RowColumn.c, XmNentryClass/XmCEntryClass resource is coded
 * as being of representation type XmRInt, when it's value is supposed to hold
 * a widget class pointer. So we create a fake, new representation type
 * XmRWidgetClass for this resource.
 */
static LVAL Cvt_XmRWidgetClass_to_LVAL(res_val, resource)
     GetValues_Union    res_val;
     Resource_Instance* resource;
{
  extern LVAL Wcls_WidgetClassID_To_WIDGETCLASSOBJ(); /* w_classes.c */
  return (Wcls_WidgetClassID_To_WIDGETCLASSOBJ((WidgetClass) res_val.XtPointer_value));
}

static XtArgVal Cvt_LVAL_to_XmRWidgetClass(lisp_val, resource, o_widget)
     LVAL lisp_val;		/* OBJECT */
     Resource_Instance* resource;
     LVAL               o_widget;
{
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  WidgetClass result;

  if (xlclass_p(lisp_val) && !(result = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(lisp_val))) {
    sprintf(temptext,
	    "Resource %s (type XmR%s) expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lisp_val);
  }

  return ((XtArgVal) result);
}

static Resource_Class resclass_XmRWidgetClass = {
  "WidgetClass",	
  OBJECT,			/* must be a widget class object */
  Cvt_XmRWidgetClass_to_LVAL, 
  Cvt_LVAL_to_XmRWidgetClass};	/* sizeof(WidgetClass) == sizeof(* struct _WidgetClassRec) */



/*----------------------------------------------------------------------------
 *-- RESOURCE CONVERTERS FOR THE HP GRAPH WIDGET (currently Motif 1.0 only) --
 *----------------------------------------------------------------------------*/
#ifdef HP_GRAPH_WIDGET

/********************************************************************************/
static Resource_Enums XmRArcDirection_enums_alist[] = {
  {":DIRECTED",		XmDIRECTED,		NULL},
  {":BIDIRECTED",	XmBIDIRECTED,		NULL},
  {":UNDIRECTED",	XmUNDIRECTED,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRArcDirection = {
  XmRArcDirection,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRArcDirection_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRLineStyle_enums_alist[] = {
  {":LINE_SOLID",		XmLineSolid,		NULL},
  {":LINE_ON_OFF_DASH",		XmLineOnOffDash,	NULL},
  {":LINE_DOUBLE_DASH",		XmLineDoubleDash,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRLineStyle = {
  XmRLineStyle,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRLineStyle_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRCapStyle_enums_alist[] = {
  {":CAP_NOT_LAST",	XmCapNotLast,		NULL},
  {":CAP_BUTT",		XmCapButt,		NULL},
  {":CAP_ROUND",	XmCapRound,		NULL},
  {":CAP_PROJECTING",	XmCapProjecting,	NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRCapStyle = {
  XmRCapStyle,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRCapStyle_enums_alist
  };



/********************************************************************************/
static Resource_Enums XmRArcDrawMode_enums_alist[] = {
  {":POSITION_FIXED",		XmPOSITION_FIXED,		NULL},
  {":POSITION_RELATIVE",	XmPOSITION_RELATIVE,		NULL},
  {NULL, NULL, NULL}
};

static Resource_Class resclass_XmRArcDrawMode = {
  XmRArcDrawMode,	
  SYMBOL,			/* enumerated type */
  Cvt_XtEnum_to_LVAL, 
  Cvt_LVAL_to_XtEnum,		/* sizeof(unsigned char) */
  XmRArcDrawMode_enums_alist
  };

#endif				/* HP_GRAPH_WIDGET */


/**************************************************************************/
/************     T A B L E     O F    R E S O U R C E S     **************/
/************                                                **************/
/**************************************************************************/
/*
 * each entry in table below is a triple of Resource_Instance structures:
 * 	'char* printname'	'Resource_Class* class'		'char* name'
 * Note that 'LVAL symbol' part of Resource_Instance is set in Wres_Init().
 */
static Resource_Instance resource_table[] = {
  /*----------------------------------------------------------------------------
   *---------------------------- XmRAcceleratorTable ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_TEXT_ACCELERATORS", &resclass_XmRAcceleratorTable, XmNtextAccelerators}, /* Xm/SelectionB.c */
  {":XMN_ACCELERATORS", &resclass_XmRAcceleratorTable, XmNaccelerators}, /* Xt/Core.c M1.1(Xm/BulletinB.c) */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRAlignment -------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ALIGNMENT", &resclass_XmRAlignment, XmNalignment}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_MESSAGE_ALIGNMENT", &resclass_XmRAlignment, XmNmessageAlignment}, /* Xm/MessageB.c */
  {":XMN_ENTRY_ALIGNMENT", &resclass_XmRAlignment, XmNentryAlignment}, /* Xm/RowColumn.c*/
  /*----------------------------------------------------------------------------
   *----------------------------  XmRArrowDirection  ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ARROW_DIRECTION", &resclass_XmRArrowDirection, XmNarrowDirection}, /* Xm/ArrowB.c Xm/ArrowBG.c */
  /*----------------------------------------------------------------------------
   *------------------------------ XmRAttachment  ------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_TOP_ATTACHMENT", &resclass_XmRAttachment, XmNtopAttachment}, /* Xm/Form.c */
  {":XMN_BOTTOM_ATTACHMENT", &resclass_XmRAttachment, XmNbottomAttachment}, /* Xm/Form.c */
  {":XMN_LEFT_ATTACHMENT", &resclass_XmRAttachment, XmNleftAttachment},	/* Xm/Form.c */
  {":XMN_RIGHT_ATTACHMENT", &resclass_XmRAttachment, XmNrightAttachment}, /* Xm/Form.c */
  /*----------------------------------------------------------------------------
   *--------------------------------- XmRBool ----------------------------------
   *----------------------------------------------------------------------------*/
  /*
   * Resource representation type XmRBool is essentially the same as boolean.
   * It is only used for the resource XmNinput.
   * Resource instances of type XmRBool unfortunately cannot be thrown into
   * resource class XmRBoolean because type Boolean=='unsigned char' while
   * type Bool=='int'. If XmRBool was treated as XmRBoolean, then doing :GET_VALUES
   * on such a resource would retrieve invalid values.
   *
   * This entire resource class should be thrown away if and when Motif and Xt get
   * some more consistently named/typed resources.
   */
  {":XMN_INPUT", &resclass_XmRBool, XmNinput}, /* Xm/Vendor.c Xt/Shell.c */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRBooleanDimension ---------------------------
   *----------------------------------------------------------------------------*/
  /*
   * Resource representation type XmRBooleanDimension is essentially the same as
   * boolean. In Motif 1.1, it is only used for the resource XmNshowAsDefault.
   *
   * Resource instances of type XmRBooleanDimension unfortunately cannot be thrown
   * into resource class XmRBoolean because type Boolean=='unsigned char' while
   * XmRBooleanDimension is sizeof(Dimension)=='unsigned short'. If
   * XmRBooleanDimension was treated as XmRBoolean, then doing :GET_VALUES
   * on such a resource would retrieve invalid values.
   *
   * This entire resource class should be thrown away if and when Motif and Xt get
   * some more consistently named/typed resources.
   */
#ifdef WINTERP_MOTIF_11
  {":XMN_SHOW_AS_DEFAULT", &resclass_XmRBooleanDimension, XmNshowAsDefault}, /* Xm:PushB.c Xm:PushBG.c */
#else				/* MOTIF 1.0 */
  {":XMN_SHOW_AS_DEFAULT", &resclass_XmRShort, XmNshowAsDefault}, /* Xm:PushB.c Xm:PushBG.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *--------------------------------- XmRBoolean -------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ALLOW_OVERLAP", &resclass_XmRBoolean, XmNallowOverlap}, /* Xm/BulletinB.c */
  {":XMN_AUTO_UNMANAGE", &resclass_XmRBoolean, XmNautoUnmanage}, /* Xm/BulletinB.c Xm/Command.c Xm/FileSB.c */
  {":XMN_DEFAULT_POSITION", &resclass_XmRBoolean, XmNdefaultPosition}, /* Xm/BulletinB.c Xm/Command.c */
  {":XMN_NO_RESIZE", &resclass_XmRBoolean, XmNnoResize}, /* Xm/BulletinB.c */
  {":XMN_TRAVERSAL_ON", &resclass_XmRBoolean, XmNtraversalOn}, /* M1.0(Xm/Gadget.c Xm/Primitive.c Xm/Scale.c) M1.1(Xm/CascadeB.c Xm/CascadeBG.c Xm/DrawnB.c Xm/Gadget.c Xm/Label.c Xm/LabelG.c Xm/Manager.c Xm/Primitive.c Xm/PushB.c Xm/PushBG.c Xm/ScrollBar.c Xm/SeparatoG.c Xm/Separator.c Xm/ToggleB.c Xm/ToggleBG.c) */
  {":XMN_PUSH_BUTTON_ENABLED", &resclass_XmRBoolean, XmNpushButtonEnabled}, /* Xm/DrawnB.c */
  {":XMN_LIST_UPDATED", &resclass_XmRBoolean, XmNlistUpdated}, /* Xm/FileSB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_DIRECTORY_VALID", &resclass_XmRBoolean, XmNdirectoryValid}, /* Xm/FileSB.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_RUBBER_POSITIONING", &resclass_XmRBoolean, XmNrubberPositioning}, /* Xm/Form.c */
  {":XMN_RESIZABLE", &resclass_XmRBoolean, XmNresizable}, /* Xm/Form.c */
  {":XMN_HIGHLIGHT_ON_ENTER", &resclass_XmRBoolean, XmNhighlightOnEnter}, /* Xm/Gadget.c Xm/Primitive.c Xm/Scale.c */
  {":XMN_RECOMPUTE_SIZE", &resclass_XmRBoolean, XmNrecomputeSize}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_AUTOMATIC_SELECTION", &resclass_XmRBoolean, XmNautomaticSelection}, /* Xm/List.c */
  {":XMN_SHOW_SEPARATOR", &resclass_XmRBoolean, XmNshowSeparator}, /* Xm/MainW.c */
  {":XMN_MINIMIZE_BUTTONS", &resclass_XmRBoolean, XmNminimizeButtons}, /* Xm/MessageB.c Xm/SelectionB.c */
  {":XMN_REFIGURE_MODE", &resclass_XmRBoolean, XmNrefigureMode}, /* Xm/PanedW.c */
  {":XMN_SEPARATOR_ON", &resclass_XmRBoolean, XmNseparatorOn}, /* Xm/PanedW.c */
  {":XMN_ALLOW_RESIZE", &resclass_XmRBoolean, XmNallowResize}, /* Xm/PanedW.c */
  {":XMN_SKIP_ADJUST", &resclass_XmRBoolean, XmNskipAdjust}, /* Xm/PanedW.c */
  {":XMN_FILL_ON_ARM", &resclass_XmRBoolean, XmNfillOnArm}, /* Xm/PushB.c & Xm/PushBG.c */
  {":XMN_RESIZE_WIDTH", &resclass_XmRBoolean, XmNresizeWidth}, /* Xm/RowColumn.c Xm/TextOut.c Xm/TextF.c: */
  {":XMN_RESIZE_HEIGHT", &resclass_XmRBoolean, XmNresizeHeight}, /* Xm/RowColumn.c Xm/TextOut.c */
  {":XMN_ADJUST_LAST", &resclass_XmRBoolean, XmNadjustLast}, /* Xm/RowColumn.c */
  {":XMN_ADJUST_MARGIN", &resclass_XmRBoolean, XmNadjustMargin}, /* Xm/RowColumn.c */
  {":XMN_RADIO_ALWAYS_ONE", &resclass_XmRBoolean, XmNradioAlwaysOne}, /* Xm/RowColumn.c */
  {":XMN_IS_HOMOGENEOUS", &resclass_XmRBoolean, XmNisHomogeneous}, /* Xm/RowColumn.c */
  {":XMN_POPUP_ENABLED", &resclass_XmRBoolean, XmNpopupEnabled}, /* Xm/RowColumn.c */
  {":XMN_RADIO_BEHAVIOR", &resclass_XmRBoolean, XmNradioBehavior}, /* Xm/RowColumn.c */
  {":XMN_IS_ALIGNED", &resclass_XmRBoolean, XmNisAligned}, /* Xm/RowColumn.c */
  {":XMN_SHOW_VALUE", &resclass_XmRBoolean, XmNshowValue}, /* Xm/Scale.c */
  {":XMN_SHOW_ARROWS", &resclass_XmRBoolean, XmNshowArrows}, /* Xm/ScrollBar.c */
  {":XMN_MUST_MATCH", &resclass_XmRBoolean, XmNmustMatch}, /* Xm/SelectionB.c */
  {":XMN_AUTO_SHOW_CURSOR_POSITION", &resclass_XmRBoolean, XmNautoShowCursorPosition}, /* Xm/Text.c */
  {":XMN_EDITABLE", &resclass_XmRBoolean, XmNeditable},	/* Xm/Text.c Xm/TextF.c XmGraph/Graph.c */
  {":XMN_CURSOR_POSITION_VISIBLE", &resclass_XmRBoolean, XmNcursorPositionVisible}, /* Xm/TextOut.c Xm/TextF.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_VERIFY_BELL", &resclass_XmRBoolean, XmNverifyBell}, /* Xm/Text.c Xm/TextF.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_PENDING_DELETE", &resclass_XmRBoolean, XmNpendingDelete}, /* Xm/TextIn.c Xm/TextF.c */
  {":XMN_WORD_WRAP", &resclass_XmRBoolean, XmNwordWrap}, /* Xm/TextOut.c */
  {":XMN_SCROLL_VERTICAL", &resclass_XmRBoolean, XmNscrollVertical}, /* Xm/TextOut.c */
  {":XMN_SCROLL_HORIZONTAL", &resclass_XmRBoolean, XmNscrollHorizontal}, /* Xm/TextOut.c */
  {":XMN_SCROLL_LEFT_SIDE", &resclass_XmRBoolean, XmNscrollLeftSide}, /* Xm/TextOut.c */
  {":XMN_SCROLL_TOP_SIDE", &resclass_XmRBoolean, XmNscrollTopSide}, /* Xm/TextOut.c */
  {":XMN_VISIBLE_WHEN_OFF", &resclass_XmRBoolean, XmNvisibleWhenOff}, /* Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_SET", &resclass_XmRBoolean, XmNset}, /* Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_INDICATOR_ON", &resclass_XmRBoolean, XmNindicatorOn}, /* Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_FILL_ON_SELECT", &resclass_XmRBoolean, XmNfillOnSelect}, /* Xm/ToggleB.c Xm/ToggleBG.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_USE_ASYNC_GEOMETRY", &resclass_XmRBoolean, XmNuseAsyncGeometry}, /* Xm/VendorE.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_MAPPED_WHEN_MANAGED", &resclass_XmRBoolean, XmNmappedWhenManaged}, /* Xt/Core.c */
  {":XMN_ANCESTOR_SENSITIVE", &resclass_XmRBoolean, XmNancestorSensitive}, /* Xt/RectObj.c Xt/Shell.c */
  {":XMN_SENSITIVE", &resclass_XmRBoolean, XmNsensitive}, /* Xt/RectObj.c */
  {":XMN_ALLOW_SHELL_RESIZE", &resclass_XmRBoolean, XmNallowShellResize}, /* Xt/Shell.c */
  {":XMN_SAVE_UNDER", &resclass_XmRBoolean, XmNsaveUnder}, /* Xt/Shell.c */
  {":XMN_OVERRIDE_REDIRECT", &resclass_XmRBoolean, XmNoverrideRedirect}, /* Xt/Shell.c */
  {":XMN_WAIT_FOR_WM", &resclass_XmRBoolean, XmNwaitForWm}, /* Xt/Shell.c */
  {":XMN_TRANSIENT", &resclass_XmRBoolean, XmNtransient}, /* Xt/Shell.c */
  {":XMN_ICONIC", &resclass_XmRBoolean, XmNiconic}, /* Xt/Shell.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_INITIAL_RESOURCES_PERSISTENT", &resclass_XmRBoolean, XmNinitialResourcesPersistent}, /* Xt/Resources.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *------------------------------  XmRButtonType  -----------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_BUTTON_TYPE", &resclass_XmRButtonTypeTable, XmNbuttonType}, /* Xm/Simple.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *-------------------------------  XmRCallback  ------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ACTIVATE_CALLBACK", &resclass_XmRCallback, XmNactivateCallback}, /* Xm/ArrowB.c Xm/ArrowBG.c Xm/CascadeB.c Xm/CascadeBG.c Xm/DrawnB.c Xm/PushB.c Xm/PushBG.c Xm/Text.c Xm/TextF.c XmGraph:Arc.c */
  {":XMN_ARM_CALLBACK", &resclass_XmRCallback, XmNarmCallback},	/* Xm/ArrowB.c Xm/ArrowBG.c Xm/DrawnB.c Xm/PushB.c Xm/PushBG.c Xm/ToggleB.c Xm/ToggleBG.c XmGraph:Arc.c */
  {":XMN_DISARM_CALLBACK", &resclass_XmRCallback, XmNdisarmCallback}, /* Xm/ArrowB.c Xm/ArrowBG.c Xm/DrawnB.c Xm/PushB.c Xm/PushBG.c Xm/ToggleB.c Xm/ToggleBG.c XmGraph:Arc.c */
  {":XMN_FOCUS_CALLBACK", &resclass_XmRCallback, XmNfocusCallback}, /* Xm/BulletinB.c Xm/Text.c */
  {":XMN_LOSING_FOCUS_CALLBACK", &resclass_XmRCallback, XmNlosingFocusCallback}, /* Xm/BulletinB.c Xm/Text.c Xm/TextF.c */
  {":XMN_MAP_CALLBACK", &resclass_XmRCallback, XmNmapCallback},	/* Xm/BulletinB.c Xm/RowColumn.c */
  {":XMN_UNMAP_CALLBACK", &resclass_XmRCallback, XmNunmapCallback}, /* Xm/BulletinB.c Xm/RowColumn.c */
  {":XMN_CASCADING_CALLBACK", &resclass_XmRCallback, XmNcascadingCallback}, /* Xm/CascadeB.c Xm/CascadeBG.c */
  {":XMN_COMMAND_ENTERED_CALLBACK", &resclass_XmRCallback, XmNcommandEnteredCallback}, /* Xm/Command.c */
  {":XMN_COMMAND_CHANGED_CALLBACK", &resclass_XmRCallback, XmNcommandChangedCallback}, /* Xm/Command.c */
  {":XMN_RESIZE_CALLBACK", &resclass_XmRCallback, XmNresizeCallback}, /* Xm/DrawingA.c Xm/DrawnB.c */
  {":XMN_EXPOSE_CALLBACK", &resclass_XmRCallback, XmNexposeCallback}, /* Xm/DrawingA.c Xm/DrawnB.c */
  {":XMN_INPUT_CALLBACK", &resclass_XmRCallback, XmNinputCallback}, /* Xm/DrawingA.c */
  {":XMN_HELP_CALLBACK", &resclass_XmRCallback, XmNhelpCallback}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c */
  {":XMN_SINGLE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNsingleSelectionCallback}, /* Xm/List.c */
  {":XMN_MULTIPLE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNmultipleSelectionCallback}, /* Xm/List.c */
  {":XMN_EXTENDED_SELECTION_CALLBACK", &resclass_XmRCallback, XmNextendedSelectionCallback}, /* Xm/List.c */
  {":XMN_BROWSE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNbrowseSelectionCallback}, /* Xm/List.c */
  {":XMN_DEFAULT_ACTION_CALLBACK", &resclass_XmRCallback, XmNdefaultActionCallback}, /* Xm/List.c */ 
  {":XMN_OK_CALLBACK", &resclass_XmRCallback, XmNokCallback}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_CANCEL_CALLBACK", &resclass_XmRCallback, XmNcancelCallback}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_ENTRY_CALLBACK", &resclass_XmRCallback, XmNentryCallback}, /* Xm/RowColumn.c */
  {":XMN_VALUE_CHANGED_CALLBACK", &resclass_XmRCallback, XmNvalueChangedCallback}, /* Xm/Scale.c Xm/ScrollBar.c Xm/Text.c Xm/TextF.c Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_DRAG_CALLBACK", &resclass_XmRCallback, XmNdragCallback}, /* Xm/Scale.c Xm/ScrollBar.c */
  {":XMN_INCREMENT_CALLBACK", &resclass_XmRCallback, XmNincrementCallback}, /* Xm/ScrollBar.c */
  {":XMN_DECREMENT_CALLBACK", &resclass_XmRCallback, XmNdecrementCallback}, /* Xm/ScrollBar.c */
  {":XMN_PAGE_INCREMENT_CALLBACK", &resclass_XmRCallback, XmNpageIncrementCallback}, /* Xm/ScrollBar.c */
  {":XMN_PAGE_DECREMENT_CALLBACK", &resclass_XmRCallback, XmNpageDecrementCallback}, /* Xm/ScrollBar.c */
  {":XMN_TO_TOP_CALLBACK", &resclass_XmRCallback, XmNtoTopCallback}, /* Xm/ScrollBar.c */
  {":XMN_TO_BOTTOM_CALLBACK", &resclass_XmRCallback, XmNtoBottomCallback}, /* Xm/ScrollBar.c */
  {":XMN_NO_MATCH_CALLBACK", &resclass_XmRCallback, XmNnoMatchCallback}, /* Xm/SelectioB.c */
  {":XMN_APPLY_CALLBACK", &resclass_XmRCallback, XmNapplyCallback}, /* Xm/SelectioB.c */
  {":XMN_MODIFY_VERIFY_CALLBACK", &resclass_XmRCallback, XmNmodifyVerifyCallback}, /* Xm/Text.c Xm/TextF.c */
  {":XMN_MOTION_VERIFY_CALLBACK", &resclass_XmRCallback, XmNmotionVerifyCallback}, /* Xm/Text.c Xm/TextF.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_GAIN_PRIMARY_CALLBACK", &resclass_XmRCallback, XmNgainPrimaryCallback}, /* Xm/Text.c Xm/TextF.c */
  {":XMN_LOSE_PRIMARY_CALLBACK", &resclass_XmRCallback, XmNlosePrimaryCallback}, /* Xm/Text.c Xm/TextF.c */
  {":XMN_FOCUS_MOVED_CALLBACK", &resclass_XmRCallback, XmNfocusMovedCallback}, /* Xm/VendorE.c */
  {":XMN_REALIZE_CALLBACK", &resclass_XmRCallback, XmNrealizeCallback},	/* Xm/VendorE.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_DESTROY_CALLBACK", &resclass_XmRCallback, XmNdestroyCallback},	/* Xt/Object.c */
  {":XMN_POPUP_CALLBACK", &resclass_XmRCallback, XmNpopupCallback}, /* Xt/Shell.c */
  {":XMN_POPDOWN_CALLBACK", &resclass_XmRCallback, XmNpopdownCallback},	/* Xt/Shell.c */
  /*----------------------------------------------------------------------------
   *------------------------- XmRCommandWindowLocation --------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_COMMAND_WINDOW_LOCATION", &resclass_XmRCommandWindowLocation, XmNcommandWindowLocation}, /* Xm/MainW.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRKeySym ----------------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_MNEMONIC", &resclass_XmRKeySym, XmNmnemonic}, /* Xm/Label.c Xm/LabelG.c Xm/RowColumn.c */
  {":XMN_OPTION_MNEMONIC", &resclass_XmRKeySym, XmNoptionMnemonic}, /* Xm/Simple.c */
#else				/* MOTIF 1.0 */
  {":XMN_MNEMONIC", &resclass_XmRChar, XmNmnemonic}, /* Xm/Label.c Xm/LabelG.c Xm/RowColumn.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRKeySymTable --------------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_BUTTON_MNEMONICS", &resclass_XmRKeySymTable, XmNbuttonMnemonics}, /* Xm/Simple.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *--------------------------- XmRHorizontalDimension -------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  /*
   * Note that Motif 1.1 inconsistently declares resource XmNspacing:
   *	XmRDimension -- Xm/ScrolledW.c
   *	XmRHorizontalDimension -- Xm/RowColumn.c Xm/ToggleB.c Xm/ToggleBG.c
   *	XmRVerticalDimension -- Xm/PanedW.c
   * As a compromise, I'm throwing all uses into XmRHorizontalDimension.
   */
  {":XMN_SPACING", &resclass_XmRHorizontalDimension, XmNspacing}, /* Xm/ScrolledW.c Xm/PanedW.c Xm/RowColumn.c Xm/ToggleB.c Xm/ToggleBG.c */
  /* 
   * In Motif 1.1, Xm/DrawnB.c and Xm/ScrolledW.c inconsistently declare XmNshadowThickness
   * as XmRDimension. Here, we throw these resources in with XmRHorizontalDimension
   * since all other uses of XmNshadowThickness are XmRHorizontalDimension.
   *
   * Note: above inconsistency fixed in Motif 1.1.1.
   */
  {":XMN_SHADOW_THICKNESS", &resclass_XmRHorizontalDimension, XmNshadowThickness}, /* Xm/DrawnB.c Xm/ScrolledW.c Xm/BulletinB.c Xm/CascadeB.c Xm/CascadeBG.c Xm/Gadget.c Xm/Label.c Xm/LabelG.c Xm/Manager.c Xm/Primitive.c Xm/PushB.c Xm/PushBG.c Xm/RowColumn.c */
  {":XMN_MARGIN_WIDTH", &resclass_XmRHorizontalDimension, XmNmarginWidth}, /* Xm/BulletinB.c Xm/DrawingA.c Xm/Form.c Xm/Frame.c Xm/Label.c Xm/LabelG.c Xm/PanedW.c Xm/RowColumn.c Xm/Text.c Xm/TextF.c */
  {":XMN_HIGHLIGHT_THICKNESS", &resclass_XmRHorizontalDimension, XmNhighlightThickness}, /* Xm/CascadeB.c Xm/CascadeBG.c Xm/DrawnB.c Xm/Gadget.c Xm/Label.c Xm/LabelG.c Xm/Primitive.c Xm/PushB.c Xm/PushBG.c Xm/Scale.c Xm/ScrollBar.c Xm/SeparatoG.c Xm/Separator.c Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_HORIZONTAL_SPACING", &resclass_XmRHorizontalDimension, XmNhorizontalSpacing}, /* Xm:Form.c */
  /*
   * XmNwidth is declared as XmRHorizontalDimension, except in Xm/Vendor.c, where it's a XmRShellHorizDim
   * and in Xt/RectObj.c, declared as XtRDimension. We throw both of those cases in with this resource class
   * XmRHorizontalDimension.
   */
  {":XMN_WIDTH", &resclass_XmRHorizontalDimension, XmNwidth}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c Xm/Scale.c Xm/Vendor.c */
  /*
   * Note that resource XmNborderWidth is inconsistently declared as XmRDimension in Xm/Sash.c and Xt/RectObj.c,
   * and as XmRShellHorizDim in Xm/Vendor.c.
   * Here, we throw these resources in with XmRHorizontalDimension since all other uses of XmNborderWidth are
   * XmRHorizontalDimension
   */
  {":XMN_BORDER_WIDTH", &resclass_XmRHorizontalDimension, XmNborderWidth}, /* Xm/Gadget.c Xm/Sash.c Xt/RectObj.c Xm/Manager.c Xm/Primitive.c Xm/Vendor.c */
  {":XMN_MARGIN_LEFT", &resclass_XmRHorizontalDimension, XmNmarginLeft}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_MARGIN_RIGHT", &resclass_XmRHorizontalDimension, XmNmarginRight}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_LIST_MARGIN_WIDTH", &resclass_XmRHorizontalDimension, XmNlistMarginWidth}, /* Xm/List.c */
  {":XMN_MAIN_WINDOW_MARGIN_WIDTH", &resclass_XmRHorizontalDimension, XmNmainWindowMarginWidth}, /* Xm/MainW.c */
  {":XMN_SASH_WIDTH", &resclass_XmRHorizontalDimension, XmNsashWidth}, /* Xm/PanedW.c */
  {":XMN_SASH_SHADOW_THICKNESS", &resclass_XmRHorizontalDimension, XmNsashShadowThickness}, /* Xm/PanedW.c */
  {":XMN_DEFAULT_BUTTON_SHADOW_THICKNESS", &resclass_XmRHorizontalDimension, XmNdefaultButtonShadowThickness}, /* Xm/PushB.c Xm/PushBG.c */ /* a new resource for 1.1 */
  {":XMN_ENTRY_BORDER", &resclass_XmRHorizontalDimension, XmNentryBorder}, /* Xm/RowColumn.c */
  {":XMN_SCALE_WIDTH", &resclass_XmRHorizontalDimension, XmNscaleWidth}, /* Xm/Scale.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_WIDTH", &resclass_XmRHorizontalDimension, XmNscrolledWindowMarginWidth}, /* Xm/ScrolledW.c */
  {":XMN_MARGIN", &resclass_XmRHorizontalDimension, XmNmargin},	/* Xm/SeparatoG.c Xm/Separator.c */
#else				/* MOTIF 1.0 */
  /*
   * Note potential BUG: in Motif 1.0 spapshot #11, XmNSpacing is declared as short
   * in ToggleB.c/ToggleBG.c; declared as Dimension (unsigned int) in 
   * RowColumn.c (listed as short in docs); and declared as int in PanedW.c and Xm/ScrolledW.c.
   * It seems like this might cause weird results to be retrieved if we're
   * retrieving a short as an unsigned int. Perhaps we should retrieve as a
   * short? In either case this is a bug in Motif. (Fixed in 1.1)
   */
  {":XMN_SPACING", &resclass_XmRDimension, XmNspacing},	/* Xm/PanedW.c Xm/RowColumn.c Xm/ToggleB.c Xm/ToggleBG.c */
  {":XMN_SHADOW_THICKNESS", &resclass_XmRShort, XmNshadowThickness}, /* Xm:BulletinB.c Xm:CascadeB.c Xm:CascadeBG.c Xm:DrawnB.c Xm:Gadget.c Xm:Label.c Xm:LabelG.c Xm:Manager.c Xm:Primitive.c Xm:PushB.c Xm:PushBG.c Xm:ScrolledW.c */
  /*
   * Note potential BUG: in Motif 1.0 spapshot #11, XmNmarginWidth is declared as
   * short in Xm/BulletinB.c Xm/DrawingA.c Xm/Frame.c Xm/Label.c Xm/LabelG.c
   * Xm/PanedW.c and Xm/Text.c, but declared as Dimension (unsigned int) for
   * RowColumn.c. I'm going with the majority here, but it'll make R/C buggy.
   * Use :XMN_RCMARGIN_WIDTH for workaround. (Fixed in 1.1)
   */
  {":XMN_MARGIN_WIDTH", &resclass_XmRShort, XmNmarginWidth}, /* Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c Xm:LabelG.c Xm:PanedW.c Xm:Text.c */
  {":XMN_HIGHLIGHT_THICKNESS", &resclass_XmRShort, XmNhighlightThickness}, /* Xm:Gadget.c Xm:Primitive.c Xm:Scale.c */
  {":XMN_HORIZONTAL_SPACING", &resclass_XmRInt, XmNhorizontalSpacing}, /* Xm:Form.c */
  {":XMN_WIDTH", &resclass_XmRDimension, XmNwidth}, /* Xt/RectObj.c */
  {":XMN_BORDER_WIDTH", &resclass_XmRDimension, XmNborderWidth}, /* Xt:RectObj.c Xm:Gadget.c Xm:Manager.c Xm:Primitive.c Xm:Sash.c */
  {":XMN_MARGIN_LEFT", &resclass_XmRShort, XmNmarginLeft}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MARGIN_RIGHT", &resclass_XmRShort, XmNmarginRight}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_LIST_MARGIN_WIDTH", &resclass_XmRShort, XmNlistMarginWidth}, /* Xm:List.c */
  {":XMN_MAIN_WINDOW_MARGIN_WIDTH", &resclass_XmRShort, XmNmainWindowMarginWidth}, /* Xm:MainW.c */
  {":XMN_SASH_WIDTH", &resclass_XmRDimension, XmNsashWidth}, /* Xm:PanedW.c */
  {":XMN_SASH_SHADOW_THICKNESS", &resclass_XmRInt, XmNsashShadowThickness}, /* Xm:PanedW.c */
  {":XMN_ENTRY_BORDER", &resclass_XmRDimension, XmNentryBorder}, /* Xm:RowColumn.c */
  {":XMN_SCALE_WIDTH", &resclass_XmRDimension, XmNscaleWidth}, /* Xm:Scale.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_WIDTH", &resclass_XmRShort, XmNscrolledWindowMarginWidth}, /* Xm:ScrolledW.c */
  {":XMN_MARGIN", &resclass_XmRShort, XmNmargin}, /* Xm:Separator.c Xm:SeparatoG.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRVerticalDimension --------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  /*
   * XmNheight is defined as XmRShellVertDim in Xm/Vendor.c and XtRDimension in Xt/RectObj.c
   * Here, we throw those uses into XmRVerticalDimension.
   */
  {":XMN_HEIGHT", &resclass_XmRVerticalDimension, XmNheight}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c Xm/Scale.c Xm/Vendor.c Xt/RectObj.c */
  {":XMN_MARGIN_HEIGHT", &resclass_XmRVerticalDimension, XmNmarginHeight}, /* Xm/BulletinB.c Xm/DrawingA.c Xm/Form.c Xm/Frame.c Xm/Label.c Xm/LabelG.c Xm/PanedW.c Xm/RowColumn.c Xm/Text.c Xm/TextF.c */
  {":XMN_VERTICAL_SPACING", &resclass_XmRVerticalDimension, XmNverticalSpacing}, /* Xm/Form.c */
  {":XMN_MARGIN_TOP", &resclass_XmRVerticalDimension, XmNmarginTop}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_MARGIN_BOTTOM", &resclass_XmRVerticalDimension, XmNmarginBottom}, /* Xm/Label.c Xm/LabelG.c */
  {":XMN_LIST_SPACING", &resclass_XmRVerticalDimension, XmNlistSpacing}, /* Xm/List.c */
  {":XMN_LIST_MARGIN_HEIGHT", &resclass_XmRVerticalDimension, XmNlistMarginHeight}, /* Xm/List.c */
  {":XMN_MAIN_WINDOW_MARGIN_HEIGHT", &resclass_XmRVerticalDimension, XmNmainWindowMarginHeight}, /* Xm/MainW.c */
  {":XMN_SASH_HEIGHT", &resclass_XmRVerticalDimension, XmNsashHeight}, /* Xm/PanedW.c */
  {":XMN_PANE_MINIMUM", &resclass_XmRVerticalDimension, XmNpaneMinimum}, /* Xm/PanedW.c */ /* a new resource for 1.1 */
  {":XMN_PANE_MAXIMUM", &resclass_XmRVerticalDimension, XmNpaneMaximum}, /* Xm/PanedW.c */ /* a new resource for 1.1 */
  {":XMN_SCALE_HEIGHT", &resclass_XmRVerticalDimension, XmNscaleHeight}, /* Xm/Scale.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_HEIGHT", &resclass_XmRVerticalDimension, XmNscrolledWindowMarginHeight}, /* Xm/ScrolledW.c */
  {":XMN_INDICATOR_SIZE", &resclass_XmRVerticalDimension, XmNindicatorSize}, /* Xm/ToggleB.c Xm/ToggleBG.c */ /* a new resource for 1.1 */
#else				/* MOTIF 1.0 */
  {":XMN_HEIGHT", &resclass_XmRDimension, XmNheight}, /* Xt:RectObj.c */
  /*
   * Note potential BUG: in Motif 1.0 spapshot #11, XmNmarginHeight is declared as
   * short in Xm/BulletinB.c Xm/DrawingA.c Xm/Frame.c XmLabel.c Xm/LabelG.c
   * Xm/PanedW.c and Xm/Text.c, but declared as Dimension (unsigned int) for
   * RowColumn.c. I'm going with the majority here, but it'll make R/C buggy.
   * For workaround, use :XMN_RCMARGIN_HEIGHT.
   */
  {":XMN_MARGIN_HEIGHT", &resclass_XmRShort, XmNmarginHeight}, /* Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c Xm:LabelG.c Xm:PanedW.c Xm:Text.c */
  {":XMN_VERTICAL_SPACING", &resclass_XmRInt, XmNverticalSpacing}, /* Xm:Form.c */
  {":XMN_MARGIN_TOP", &resclass_XmRShort, XmNmarginTop}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MARGIN_BOTTOM", &resclass_XmRShort, XmNmarginBottom}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_LIST_SPACING", &resclass_XmRShort, XmNlistSpacing}, /* Xm:List.c */
  {":XMN_LIST_MARGIN_HEIGHT", &resclass_XmRShort, XmNlistMarginHeight},	/* Xm:List.c */
  {":XMN_MAIN_WINDOW_MARGIN_HEIGHT", &resclass_XmRShort, XmNmainWindowMarginHeight}, /* Xm:MainW.c */
  {":XMN_SASH_HEIGHT", &resclass_XmRDimension, XmNsashHeight}, /* Xm:PanedW.c */
  {":XMN_SCALE_HEIGHT", &resclass_XmRDimension, XmNscaleHeight}, /* Xm:Scale.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_HEIGHT", &resclass_XmRShort, XmNscrolledWindowMarginHeight}, /* Xm:ScrolledW.c */
  /*
   * the following are aliases to XmNmarginWidgth and XmNmarginHeight
   * This is a workaround to a Motif 1.0 bug which inconsistently declares
   * these resources only for Xm/RowColumn.c. Not needed for Motif 1.1.
   */
  {":XMN_RCMARGIN_WIDTH", &resclass_XmRDimension, XmNmarginWidth}, /* Xm:RowColumn.c -- this is a special alias for r/c widget due to name conflict*/
  {":XMN_RCMARGIN_HEIGHT", &resclass_XmRDimension, XmNmarginHeight}, /* Xm:RowColumn.c -- this is a special alias for r/c widget due to name conflic*/
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *--------------------------- XmRDefaultButtonType ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_DEFAULT_BUTTON_TYPE", &resclass_XmRDefaultButtonType, XmNdefaultButtonType}, /* Xm/MessageB.c */
  /*----------------------------------------------------------------------------
   *------------------------------ XmRDeleteResponse ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_DELETE_RESPONSE", &resclass_XmRDeleteResponse, XmNdeleteResponse}, /* Xm/DialogS.c Xm/VendorE.c */
  /*----------------------------------------------------------------------------
   *-------------------------------  XmRDialogStyle ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_DIALOG_STYLE", &resclass_XmRDialogStyle, XmNdialogStyle}, /* Xm/BulletinB.c */
  /*----------------------------------------------------------------------------
   *------------------------------  XmRDialogType  -----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_DIALOG_TYPE", &resclass_XmRDialogType, XmNdialogType},	/* Xm/Command.c Xm/FileSB.c Xm/MessageB.c Xm/SelectioB.c */
  /*----------------------------------------------------------------------------
   *------------------------------- XmREditMode --------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_EDIT_MODE", &resclass_XmREditMode, XmNeditMode}, /* Xm/Text.c */
  /*----------------------------------------------------------------------------
   *------------------------------ XmRFileTypeMask -----------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_FILE_TYPE_MASK", &resclass_XmRFileTypeMask, XmNfileTypeMask}, /* Xm/FileSB.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *-------------------------------- XmRFontList -------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_BUTTON_FONT_LIST", &resclass_XmRFontList, XmNbuttonFontList}, /* Xm/BulletinB.c */
  {":XMN_LABEL_FONT_LIST", &resclass_XmRFontList, XmNlabelFontList}, /* Xm/BulletinB.c */
  {":XMN_TEXT_FONT_LIST", &resclass_XmRFontList, XmNtextFontList}, /* Xm/BulletinB.c */
  {":XMN_FONT_LIST", &resclass_XmRFontList, XmNfontList}, /* Xm/Label.c Xm/LabelG.c Xm/List.c Xm:Scale.c Xm/TextF.c Xm:TextOut.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_DEFAULT_FONT_LIST", &resclass_XmRFontList, XmNdefaultFontList}, /* Xm/MenuShell.c Xm/VendorE.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *----------------------------   XmRIndicatorType  ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_INDICATOR_TYPE", &resclass_XmRIndicatorType, XmNindicatorType}, /* Xm/ToggleB.c Xm/ToggleBG.c */
  /*----------------------------------------------------------------------------
   *------------------------------ XtRInitialState -----------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_INITIAL_STATE", &resclass_XtRInitialState, XmNinitialState}, /* Xt:Shell.c */
#else				/* MOTIF 1.0 */
  {":XMN_INITIAL_STATE", &resclass_XmRInt, XmNinitialState}, /* Xt:Shell.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *----------------------------   XmRHorizontalInt  ---------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_BASE_WIDTH", &resclass_XmRHorizontalInt, XmNbaseWidth}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */ /* New resource for Motif 1.1 */
  {":XMN_MIN_WIDTH", &resclass_XmRHorizontalInt, XmNminWidth}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MAX_WIDTH", &resclass_XmRHorizontalInt, XmNmaxWidth}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_WIDTH_INC", &resclass_XmRHorizontalInt, XmNwidthInc}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MIN_ASPECT_X", &resclass_XmRHorizontalInt, XmNminAspectX}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MAX_ASPECT_X", &resclass_XmRHorizontalInt, XmNmaxAspectX}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_ICON_X", &resclass_XmRHorizontalInt, XmNiconX}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
#else				/* MOTIF 1.0 */
  {":XMN_MIN_WIDTH", &resclass_XmRInt, XmNminWidth}, /* Xt:Shell.c */
  {":XMN_MAX_WIDTH", &resclass_XmRInt, XmNmaxWidth}, /* Xt:Shell.c */
  {":XMN_WIDTH_INC", &resclass_XmRInt, XmNwidthInc}, /* Xt:Shell.c */
  {":XMN_MIN_ASPECT_X", &resclass_XmRInt, XmNminAspectX}, /* Xt:Shell.c */
  {":XMN_MAX_ASPECT_X", &resclass_XmRInt, XmNmaxAspectX}, /* Xt:Shell.c */
  {":XMN_ICON_X", &resclass_XmRInt, XmNiconX}, /* Xt:Shell.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *----------------------------        XmRInt       ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_MAPPING_DELAY", &resclass_XmRInt, XmNmappingDelay}, /* Xm/CascadeB.c Xm/CascadeBG.c */
  {":XMN_HISTORY_ITEM_COUNT", &resclass_XmRInt, XmNhistoryItemCount}, /* Xm/Command.c */
  {":XMN_HISTORY_MAX_ITEMS", &resclass_XmRInt, XmNhistoryMaxItems}, /* Xm/Command.c */
  {":XMN_HISTORY_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNhistoryVisibleItemCount}, /* Xm:Command.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_DIR_LIST_ITEM_COUNT", &resclass_XmRInt, XmNdirListItemCount}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_FILE_LIST_ITEM_COUNT", &resclass_XmRInt, XmNfileListItemCount}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_FRACTION_BASE", &resclass_XmRInt, XmNfractionBase}, /* Xm/Form.c */
  {":XMN_TOP_POSITION", &resclass_XmRInt, XmNtopPosition}, /* Xm/Form.c (constraint res) [Xm:Text.c in Motif 1.0 only] */
  {":XMN_BOTTOM_POSITION", &resclass_XmRInt, XmNbottomPosition}, /* Xm/Form.c (constraint res) */
  {":XMN_LEFT_POSITION", &resclass_XmRInt, XmNleftPosition}, /* Xm/Form.c (constraint res) */
  {":XMN_RIGHT_POSITION", &resclass_XmRInt, XmNrightPosition}, /* Xm/Form.c (constraint res) */
  {":XMN_TOP_OFFSET", &resclass_XmRInt, XmNtopOffset}, /* Xm:Form.c (constraint res) */
  {":XMN_BOTTOM_OFFSET", &resclass_XmRInt, XmNbottomOffset}, /* Xm:Form.c (constraint res) */
  {":XMN_LEFT_OFFSET", &resclass_XmRInt, XmNleftOffset}, /* Xm:Form.c (constraint res) */
  {":XMN_RIGHT_OFFSET", &resclass_XmRInt, XmNrightOffset}, /* Xm:Form.c (constraint res) */
  {":XMN_ITEM_COUNT", &resclass_XmRInt, XmNitemCount}, /* Xm:List.c */
  {":XMN_SELECTED_ITEM_COUNT", &resclass_XmRInt, XmNselectedItemCount},	/* Xm:List.c */
  {":XMN_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNvisibleItemCount}, /* Xm:List.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_TOP_ITEM_POSITION", &resclass_XmRInt, XmNtopItemPosition}, /* Xm/List.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_DOUBLE_CLICK_INTERVAL", &resclass_XmRInt, XmNdoubleClickInterval}, /* Xm/List.c XmGraph/Graph.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_POST_FROM_COUNT", &resclass_XmRInt, XmNpostFromCount},	/* Xm/RowColumn.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_VALUE", &resclass_XmRInt, XmNvalue}, /* Xm/Scale.c Xm/ScrollBar.c */
  {":XMN_MAXIMUM", &resclass_XmRInt, XmNmaximum}, /* Xm/Scale.c Xm/ScrollBar.c [Xm:PanedW.c in Motif 1.0 only] */
  {":XMN_MINIMUM", &resclass_XmRInt, XmNminimum}, /* Xm/Scale.c Xm/ScrollBar.c [Xm:PanedW.c in Motif 1.0 only] */
#ifdef WINTERP_MOTIF_11
  {":XMN_SCALE_MULTIPLE", &resclass_XmRInt, XmNscaleMultiple}, /* Xm/Scale.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_SLIDER_SIZE", &resclass_XmRInt, XmNsliderSize}, /* Xm:ScrollBar.c */
  {":XMN_INCREMENT", &resclass_XmRInt, XmNincrement}, /* Xm:ScrollBar.c */
  {":XMN_PAGE_INCREMENT", &resclass_XmRInt, XmNpageIncrement}, /* Xm:ScrollBar.c */
  {":XMN_INITIAL_DELAY", &resclass_XmRInt, XmNinitialDelay}, /* Xm:ScrollBar.c */
  {":XMN_REPEAT_DELAY", &resclass_XmRInt, XmNrepeatDelay}, /* Xm:ScrollBar.c */
  {":XMN_LIST_ITEM_COUNT", &resclass_XmRInt, XmNlistItemCount},	/* Xm:SelectionB.c */
  {":XMN_LIST_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNlistVisibleItemCount}, /* Xm:SelectionB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_BUTTON_COUNT", &resclass_XmRInt, XmNbuttonCount}, /* Xm/Simple.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_POST_FROM_BUTTON", &resclass_XmRInt, XmNpostFromButton}, /* Xm/Simple.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_BUTTON_SET", &resclass_XmRInt, XmNbuttonSet}, /* Xm/Simple.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_MAX_LENGTH", &resclass_XmRInt, XmNmaxLength}, /* Xm:Text.c Xm/TextF.c */
  {":XMN_BLINK_RATE", &resclass_XmRInt, XmNblinkRate}, /* Xm/TextF.c Xm/TextOut.c */
  {":XMN_SELECTION_ARRAY_COUNT", &resclass_XmRInt, XmNselectionArrayCount}, /* Xm/TextF.c Xm/TextIn.c */
  {":XMN_SELECT_THRESHOLD", &resclass_XmRInt, XmNselectThreshold}, /* Xm/TextF.c Xm/TextIn.c */
  {":XMN_MWM_DECORATIONS", &resclass_XmRInt, XmNmwmDecorations}, /* Xm/VendorE.c */ /* value should be logior of MWM_DECOR_ALL MWM_DECOR_BORDER MWM_DECOR_RESIZEH MWM_DECOR_TITLE MWM_DECOR_MENU MWM_DECOR_MINIMIZE MWM_DECOR_MAXIMIZE */
  {":XMN_MWM_FUNCTIONS", &resclass_XmRInt, XmNmwmFunctions}, /* Xm/VendorE.c */ /* value should be logior of MWM_FUNC_ALL MWM_FUNC_RESIZE MWM_FUNC_MOVE MWM_FUNC_MINIMIZE MWM_FUNC_MAXIMIZE MWM_FUNC_CLOSE */
  {":XMN_MWM_INPUT_MODE", &resclass_XmRInt, XmNmwmInputMode}, /* Xm/VendorE.c */ /* value should be logior of MWM_INPUT_MODELESS MWM_INPUT_PRIMARY_APPLICATION_MODAL MWM_INPUT_SYSTEM_MODAL MWM_INPUT_FULL_APPLICATION_MODAL */
  {":XMN_DEPTH", &resclass_XmRInt, XmNdepth}, /* Xt/Core.c Xt/Shell.c */
  {":XMN_WM_TIMEOUT", &resclass_XmRInt, XmNwmTimeout}, /* Xt/Shell.c */
#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
  {":XMN_ARGC", &resclass_XmRInt, XmNargc}, /* Xt/Shell.c (application shell only) */
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */
  /*----------------------------------------------------------------------------
   *----------------------------    XmRVerticalInt   ---------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_BASE_HEIGHT", &resclass_XmRVerticalInt, XmNbaseHeight}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */ /* New resource for Motif 1.1 */
  {":XMN_MIN_HEIGHT", &resclass_XmRVerticalInt, XmNminHeight}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MAX_HEIGHT", &resclass_XmRVerticalInt, XmNmaxHeight}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_HEIGHT_INC", &resclass_XmRVerticalInt, XmNheightInc}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MIN_ASPECT_Y", &resclass_XmRVerticalInt, XmNminAspectY}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_MAX_ASPECT_Y", &resclass_XmRVerticalInt, XmNmaxAspectY}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
  {":XMN_ICON_Y", &resclass_XmRVerticalInt, XmNiconY}, /* Xm/Vendor.c Xt/Shell.c (as XmRInt) */
#else				/* MOTIF 1.0 */
  {":XMN_MIN_HEIGHT", &resclass_XmRInt, XmNminHeight}, /* Xt:Shell.c */
  {":XMN_MAX_HEIGHT", &resclass_XmRInt, XmNmaxHeight}, /* Xt:Shell.c */
  {":XMN_HEIGHT_INC", &resclass_XmRInt, XmNheightInc}, /* Xt:Shell.c */
  {":XMN_MIN_ASPECT_Y", &resclass_XmRInt, XmNminAspectY}, /* Xt:Shell.c */
  {":XMN_MAX_ASPECT_Y", &resclass_XmRInt, XmNmaxAspectY}, /* Xt:Shell.c */
  {":XMN_ICON_Y", &resclass_XmRInt, XmNiconY}, /* Xt:Shell.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *-------------------------- XmRKeyboardFocusPolicy --------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_KEYBOARD_FOCUS_POLICY", &resclass_XmRKeyboardFocusPolicy, XmNkeyboardFocusPolicy}, /* Xm/VendorE.c */
  /*----------------------------------------------------------------------------
   *-------------------------------- XmRLabelType ------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_LABEL_TYPE", &resclass_XmRLabelType, XmNlabelType}, /* Xm/Label.c Xm/LabelG.c */
  /*----------------------------------------------------------------------------
   *----------------------------  XmRListSizePolicy  ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_LIST_SIZE_POLICY", &resclass_XmRListSizePolicy, XmNlistSizePolicy}, /* Xm/List.c */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRMultiClick ------------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_MULTI_CLICK", &resclass_XmRMultiClick, XmNmultiClick}, /* Xm/ArrowB.c Xm/ArrowBG.c Xm/DrawnB.c Xm/PushB.c Xm/PushBG.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *----------------------------- XmRNavigationType ----------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_NAVIGATION_TYPE", &resclass_XmRNavigationType, XmNnavigationType}, /* Xm/Gadget.c Xm/List.c Xm/MainW.c Xm/Manager.c Xm/Primitive.c Xm/RowColumn.c Xm/ScrollBar.c Xm/Text.c Xm/TextF.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *------------------------------- XmROrientation -----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ORIENTATION", &resclass_XmROrientation, XmNorientation}, /* Xm/RowColumn.c Xm/Scale.c Xm/ScrollBar.c Xm/SeparatoG.c Xm/Separator.c XmGraph/Graph.c */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRPacking ---------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_PACKING", &resclass_XmRPacking, XmNpacking}, /* Xm/RowColumn.c */
  /*----------------------------------------------------------------------------
   *---------------------------------- XmRPixel --------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_FOREGROUND", &resclass_XmRPixel, XmNforeground}, /* Xm/Manager.c Xm/Primitive.c Xm/ScrollBar.c XmGraph:Arc.c */
  {":XMN_BACKGROUND", &resclass_XmRPixel, XmNbackground}, /* Xm/Manager.c Xm:Primitive.c Xt/Core.c */
  {":XMN_HIGHLIGHT_COLOR", &resclass_XmRPixel, XmNhighlightColor}, /* Xm/Manager.c Xm/Primitive.c XmGraph:Arc.c */
  {":XMN_TOP_SHADOW_COLOR", &resclass_XmRPixel, XmNtopShadowColor}, /* Xm/Manager.c Xm/Primitive.c */
  {":XMN_BOTTOM_SHADOW_COLOR", &resclass_XmRPixel, XmNbottomShadowColor}, /* Xm/Manager.c Xm/Primitive.c */
  {":XMN_ARM_COLOR", &resclass_XmRPixel, XmNarmColor}, /* Xm/PushB.c Xm/PushBG.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_TROUGH_COLOR", &resclass_XmRPixel, XmNtroughColor}, /* Xm/ScrollBar.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_SELECT_COLOR", &resclass_XmRPixel, XmNselectColor}, /* Xm/ToggleB.c Xm/ToggleBG.c */  
  {":XMN_BORDER_COLOR", &resclass_XmRPixel, XmNborderColor}, /* Xt/Core.c */
  /*----------------------------------------------------------------------------
   *----------------------------------  "Pixmap"  ------------------------------
   *----------------------------------------------------------------------------*/
  /*
   * _XmRXmBackgroundPixmap -- note that this resource rep type has a very weird
   * side-effect-causing kludge-of-a-converter in Motif 1.1. It would seem this
   * converter needs to be used (by calling XtConvert(), rather than doing the
   * conversion ourselves in Wres_Get_LispArglist(). <Insert gestalt of puking
   * synaesthesia here>
   */
#ifdef WINTERP_MOTIF_11
  {":XMN_BACKGROUND_PIXMAP", &resclass_XmRXmBackgroundPixmap, XmNbackgroundPixmap},	/* /Xt/Core.c -- XmRPixmap; Xm/Manager.c Xm/Primitive.c -- XmRXmBackgroundPixmap */
#else
  {":XMN_BACKGROUND_PIXMAP", &resclass_XmRPixmap, XmNbackgroundPixmap},	/* Xt/Core.c Xm/Manager.c Xm/Primitive.c -- XmRPixmap */
#endif				/* WINTERP_MOTIF_11 */
  /*
   * "ForegroundPixmap" == XmRManForegroundPixmap + XmRPrimForegroundPixmap + XmRGadgetPixmap
   */
  {":XMN_SYMBOL_PIXMAP", &resclass_ForegroundPixmap, XmNsymbolPixmap}, /* in Xm/MessageB.c -- XmRManForegroundPixmap */
  {":XMN_CASCADE_PIXMAP", &resclass_ForegroundPixmap, XmNcascadePixmap}, /* Xm/CascadeB.c -- XmRPrimForegroundPixmap; Xm/CascadeBG.c -- XmRGadgetPixmap */
  {":XMN_LABEL_PIXMAP", &resclass_ForegroundPixmap, XmNlabelPixmap}, /* Xm/Label.c -- XmRPrimForegroundPixmap; Xm/LabelG.c -- XmRGadgetPixmap  */
  {":XMN_LABEL_INSENSITIVE_PIXMAP", &resclass_ForegroundPixmap, XmNlabelInsensitivePixmap}, /* Xm/Label.c -- XmRPixmap Xm:LabelG.c -- XmRGadgetPixmap */
  {":XMN_ARM_PIXMAP", &resclass_ForegroundPixmap, XmNarmPixmap}, /* in Xm/PushB.c -- XmRPrimForegroundPixmap; Xm/PushBG.c -- XmRGadgetPixmap */
  {":XMN_SELECT_PIXMAP", &resclass_ForegroundPixmap, XmNselectPixmap}, /* Xm/ToggleB.c -- XmRPrimForegroundPixmap; Xm/ToggleBG.c -- XmRGadgetPixmap */
  {":XMN_SELECT_INSENSITIVE_PIXMAP", &resclass_ForegroundPixmap, XmNselectInsensitivePixmap}, /* Xm/ToggleB.c -- XmRPixmap; Xm/ToggleBG.c -- XmRGadgetPixmap */
  {":XMN_ICON_PIXMAP", &resclass_ForegroundPixmap, XmNiconPixmap}, /* Xm/Vendor.c -- XmRPixmap; Xt/Shell.c (in Motif1.1 decld as XtRBitmap which is a pixmap of depth 1) */
  {":XMN_ICON_MASK", &resclass_ForegroundPixmap, XmNiconMask},	/* Xt/Shell.c  (in Motif1.1 decld as XtRBitmap which is a pixmap of depth 1) */
  {":XMN_BORDER_PIXMAP", &resclass_ForegroundPixmap, XmNborderPixmap},	/* Xt/Core.c -- XmRPixmap */
  /*
   * "HighlightPixmap" == XmRPrimHighlightPixmap + XmRManHighlightPixmap
   */
  {":XMN_HIGHLIGHT_PIXMAP", &resclass_HighlightPixmap, XmNhighlightPixmap}, /* in Xm/Primitive.c -- XmRPrimHighlightPixmap; Xm/Manager.c -- XmRManHighlightPixmap */
  /*
   * "TopShadowPixmap" == XmRPrimTopShadowPixmap + XmRManTopShadowPixmap
   */
  {":XMN_TOP_SHADOW_PIXMAP", &resclass_TopShadowPixmap, XmNtopShadowPixmap}, /* in Xm/Primitive.c -- XmRPrimTopShadowPixmap; Xm/Manager.c -- XmRManTopShadowPixmap */
  /*
   * "BottomShadowPixmap" == XmRPrimBottomShadowPixmap + XmRManBottomShadowPixmap
   */
  {":XMN_BOTTOM_SHADOW_PIXMAP", &resclass_BottomShadowPixmap, XmNbottomShadowPixmap}, /* in Xm/Primitive.c -- XmRPrimBottomShadowPixmap; Xm/Manager.c -- XmRManBottomShadowPixmap */
  /*----------------------------------------------------------------------------
   *----------------- XmRShellHorizPos & XmRHorizontalPosition -----------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_X", &resclass_XmRHorizontalPosition, XmNx}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c Xm/Vendor.c(as XmRShellHorizPos) Xt/RectObj.c(as XtRPosition) Xt/Shell.c(as XtRPosition) */
  {":XMN_SASH_INDENT", &resclass_XmRHorizontalPosition, XmNsashIndent},	/* Xm/PanedW.c */
#else				/* MOTIF 1.0 */
  {":XMN_X", &resclass_XmRPosition, XmNx}, /* Xt:RectObj.c */
  {":XMN_SASH_INDENT", &resclass_XmRPosition, XmNsashIndent}, /* Xm:PanedW.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *---------------- XmRShellVertPos & XmRVerticalPosition  --------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_Y", &resclass_XmRVerticalPosition, XmNy}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c Xm/Vendor.c(as XmRShellVertPos) Xt/RectObj.c(as XtRPosition) Xt/Shell.c(as XtRPosition) */
#else				/* MOTIF 1.0 */
  {":XMN_Y", &resclass_XmRPosition, XmNy}, /* Xt:RectObj.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *----------------------------  XmRProcessingDirection  ----------------------
   *----------------------------------------------------------------------------*/
  {":XMN_PROCESSING_DIRECTION", &resclass_XmRProcessingDirection, XmNprocessingDirection}, /* Xm/Scale.c Xm/ScrollBar.c */
  /*----------------------------------------------------------------------------
   *------------------------------ XmRResizePolicy  ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_RESIZE_POLICY", &resclass_XmRResizePolicy, XmNresizePolicy}, /* Xm/BulletinB.c Xm/Command.c Xm/DrawingA.c */
  /*----------------------------------------------------------------------------
   *----------------------------- XmRRowColumnType -----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ROW_COLUMN_TYPE", &resclass_XmRRowColumnType, XmNrowColumnType}, /* Xm:RowColumn.c */
  /*----------------------------------------------------------------------------
   *-------------------------- XmRScrollBarDisplayPolicy -----------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SCROLL_BAR_DISPLAY_POLICY", &resclass_XmRScrollBarDisplayPolicy, XmNscrollBarDisplayPolicy}, /* Xm/List.c Xm/ScrolledW.c */
  /*----------------------------------------------------------------------------
   *-------------------------- XmRScrollBarPlacement ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SCROLL_BAR_PLACEMENT", &resclass_XmRScrollBarPlacement, XmNscrollBarPlacement}, /* Xm/ScrolledW.c */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRScrollingPolicy ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SCROLLING_POLICY", &resclass_XmRScrollingPolicy, XmNscrollingPolicy}, /* Xm/ScrolledW.c */
  /*----------------------------------------------------------------------------
   *---------------------------- "XmRSelectionArray" ---------------------------
   *----------------------------------------------------------------------------*/
  /**
   ** NOTE that there is no "XmRSelectionArray" resource type in Motif/Xt.
   ** Resource XmNselectionArray is declared as XmRPointer, and you can't
   ** do much with such a vague type.
   **/
  {":XMN_SELECTION_ARRAY", &resclass_XmRSelectionArray, XmNselectionArray}, /* Xm/TextF.c Xm/TextIn.c */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRSelectionPolicy ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SELECTION_POLICY", &resclass_XmRSelectionPolicy, XmNselectionPolicy}, /* Xm/List.c */
  /*----------------------------------------------------------------------------
   *----------------------------- XmRSeparatorType  ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SEPARATOR_TYPE", &resclass_XmRSeparatorType, XmNseparatorType}, /* Xm/SeparatoG.c Xm/Separator.c */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRShadowType ------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_SHADOW_TYPE", &resclass_XmRShadowType, XmNshadowType},	/* Xm/BulletinB.c Xm/DrawnB.c Xm/Frame.c */
  /*----------------------------------------------------------------------------
   *--------------------------------  XmRShort  --------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_NUM_COLUMNS", &resclass_XmRShort, XmNnumColumns}, /* Xm/RowColumn.c */
  {":XMN_DECIMAL_POINTS", &resclass_XmRShort, XmNdecimalPoints}, /* Xm/Scale.c */
  {":XMN_TEXT_COLUMNS", &resclass_XmRShort, XmNtextColumns}, /* Xm/SelectionB.c */
  {":XMN_COLUMNS", &resclass_XmRShort, XmNcolumns}, /* Xm/TextF.c Xm/TextOut.c */
  {":XMN_ROWS", &resclass_XmRShort, XmNrows}, /* Xm/TextOut.c */
  /*----------------------------------------------------------------------------
   *--------------------------------- XmRString --------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_ACCELERATOR", &resclass_XmRString, XmNaccelerator}, /* Xm/Label.c Xm/LabelG.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_MENU_POST", &resclass_XmRString, XmNmenuPost}, /* Xm/RowColumn.c */ /* a new resource for Motif 1.1 */
  {":XMN_MNEMONIC_CHAR_SET", &resclass_XmRString, XmNmnemonicCharSet},	/* Xm/Label.c /Xm/LabelG.c Xm/RowColumn.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_MENU_ACCELERATOR", &resclass_XmRString, XmNmenuAccelerator}, /* Xm/RowColumn.c */
  /*
   * Note potential BUG due to Motif's braindamage -- XmRString resource XmNvalue in Xm/Text.c
   * conflicts with XmNvalue which is used as an int in Xm/Scale.c and Xm/ScrollBar.c. 
   * For those wanting to use XtGetValues()/XtSetValues(), an alias resource called
   * :XMN_STRING has been provided. Alternately, use XmText widget methods
   * :GET_STRING and :SET_STRING.
   */
  {":XMN_STRING", &resclass_XmRString, XmNvalue}, /* Xm/TextF.c Xm:Text.c */
  {":XMN_MWM_MENU", &resclass_XmRString, XmNmwmMenu}, /* Xm/VendorE.c */
  /* {":DEFAULT_VIRTUAL_BINDINGS", &resclass_XmRString, "defaultVirtualBindings"}, -- this is a weird resource, doesn't seem settable */ /* Xm/VirtKeys.c */ /* a new resource for Motif 1.1 */
  /*
   * XmNgeometry is supposed to be a normal XmRString resource. In reality, it behaves somewhat
   * differently, at least in Motif 1.1. XmRString resources are supposed to be copied
   * into the widget at creation-time. This one doesn't get copied, and you would
   * see potential errors when the string value you set for XmNgeometry gets garbage-collected...
   * The solution is to create a special version of resclass_XmRString which copies the
   * string...
   */
  {":XMN_GEOMETRY", &resclass_XmRString_for_XmNgeometry, XmNgeometry}, /* Xt/Shell.c */
  {":XMN_TITLE", &resclass_XmRString, XmNtitle}, /* Xt/Shell.c */
  {":XMN_ICON_NAME", &resclass_XmRString, XmNiconName},	/* Xt:Shell.c -- toplevelshell only */
  /*----------------------------------------------------------------------------
   *--------------------------- XmRStringDirection -----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_STRING_DIRECTION", &resclass_XmRStringDirection, XmNstringDirection}, /* Xm/Label.c Xm/LabelG.c Xm/List.c Xm/Manager.c(in Motif 1.1) Xm:BulletinB.c(in Motif 1.0) */
  /*----------------------------------------------------------------------------
   *----------------------------- XmRStringTable -------------------------------
   *---------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  /*
   * XmNbuttonMnemonicCharSets is declared as XmRCharSetTable, however, it looks
   * like CharSets are just strings, and so CharSetTable == StringTable.
   */
  {":XMN_BUTTON_MNEMONIC_CHAR_SETS", &resclass_XmRStringTable, XmNbuttonMnemonicCharSets}, /* Xm/Simple.c */
  {":XMN_BUTTON_ACCELERATORS", &resclass_XmRStringTable, XmNbuttonAccelerators}, /* Xm/Simple.c */
#endif				/* WINTERP_MOTIF_11 */
  /*
   * XmNargv is declared as an XtRStringArray in 1.1, and as XmRPointer in 1.0. In
   * reality, it expects a table of strings, so putting this resource under
   * XmRStringTable seems logical.
   *
   * Unfortunately, XmNargv doesn't work in the way that most resources work -- 
   * it does not copy the strings in the table of strings. Since the other
   * XmRStringTable resources above do that, XmNargv should not be settable
   * via XtSetValues() nor XtCreate...(). Use
   * method (send <application-shell-widget> :SET_ARGV). Likewise, to retrieve
   * this resource, use method (send <application-shell-widget> :GET_ARGV)
   */
#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
  {":XMN_ARGV", &resclass_XmRStringTable, XmNargv}, /* Xt/Shell.c (application shell only) */
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRTextPosition ----------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  {":XMN_TOP_CHARACTER", &resclass_XmRTextPosition, XmNtopCharacter}, /* Xm/Text.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_CURSOR_POSITION", &resclass_XmRTextPosition, XmNcursorPosition}, /* Xm/Text.c Xm/TextF.c */
#else				/* MOTIF 1.0 */
  {":XMN_CURSOR_POSITION", &resclass_XmRInt, XmNcursorPosition}, /* Xm:Text.c */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *---------------------------- XmRTranslationTable ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_TEXT_TRANSLATIONS", &resclass_XmRTranslationTable, XmNtextTranslations}, /* Xm/BulletinB.c */
  {":XMN_TRANSLATIONS", &resclass_XmRTranslationTable, XmNtranslations}, /* Xt/Core.c */
  /*----------------------------------------------------------------------------
   *-------------------------------- XmRUnitType -------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_UNIT_TYPE", &resclass_XmRUnitType, XmNunitType}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c */
  {":XMN_SHELL_UNIT_TYPE", &resclass_XmRUnitType, XmNshellUnitType}, /* Xm/Vendor.c -- note: this is listed as XmRShellUnitType which is same as XmRUnitType */
  /*----------------------------------------------------------------------------
   *------------------------------  XmRVisualPolicy ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_VISUAL_POLICY", &resclass_XmRVisualPolicy, XmNvisualPolicy}, /* Xm/ScrolledW.c */
  /*----------------------------------------------------------------------------
   *------------------------------- XmRWhichButton -----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_WHICH_BUTTON", &resclass_XmRWhichButton, XmNwhichButton}, /* Xm/RowColumn.c */
  /*----------------------------------------------------------------------------
   *-------------------------------- XmRWidget ---------------------------------
   *----------------------------------------------------------------------------*/
  /**
   ** Note that representation type XmRWidget has been "underloaded" in Motif. 
   ** That is, resources declard as XmRWindow, XmRMenuWidget, and XmRWidget have
   ** values that are widgetID's (assoc'd w/ WIDGETOBJ's in WINTERP).
   ** Therefore I've put all these together into type XmRWidget. Since Motif 1.1
   ** has no string-->widget resource converter we don't need to worry about the
   ** naming diffs between XmRWindow,XmRMenuWidget and XmRWidget.
   **/
#ifdef WINTERP_MOTIF_11
  {":XMN_TRANSIENT_FOR", &resclass_XmRWidget, XmNtransientFor}, /* Xt/Shell.c --- transient shell only */ /* a new resource for Motif 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  /**
   ** The following resources were declared as XmRMenuWidget, but they're being
   ** thrown into type XmRWidget since the resource values represent widgetID's.
   **/
  {":XMN_SUB_MENU_ID", &resclass_XmRWidget, XmNsubMenuId}, /* Xm/CascadeB.c Xm/CascadeBG.c Xm/RowColumn.c */
  {":XMN_MENU_HELP_WIDGET", &resclass_XmRWidget, XmNmenuHelpWidget}, /* Xm/RowColumn.c -- used only for XmMENU_BAR */
  {":XMN_MENU_HISTORY", &resclass_XmRWidget, XmNmenuHistory}, /* Xm/RowColumn.c -- last activated menu entry */

  /**
   ** The following resources were declared as XmRWindow, but they're being
   ** thrown into type XmRWidget since the resource values represent widgetID's.
   ** (Note that the declaration doesn't correspond to usage in source nor
   ** verbiage in documentation)
   **/
  {":XMN_DEFAULT_BUTTON", &resclass_XmRWidget, XmNdefaultButton}, /* Xm/BulletinB.c */
  {":XMN_CANCEL_BUTTON", &resclass_XmRWidget, XmNcancelButton},	/* Xm/BulletinB.c */
  {":XMN_TOP_WIDGET", &resclass_XmRWidget, XmNtopWidget}, /* Xm/Form.c */
  {":XMN_BOTTOM_WIDGET", &resclass_XmRWidget, XmNbottomWidget},	/* Xm/Form.c */
  {":XMN_LEFT_WIDGET", &resclass_XmRWidget, XmNleftWidget}, /* Xm/Form.c */
  {":XMN_RIGHT_WIDGET", &resclass_XmRWidget, XmNrightWidget}, /* Xm/Form.c */
  {":XMN_HORIZONTAL_SCROLL_BAR", &resclass_XmRWidget, XmNhorizontalScrollBar}, /* Xm/List.c Xm/ScrolledW.c */
  {":XMN_VERTICAL_SCROLL_BAR", &resclass_XmRWidget, XmNverticalScrollBar}, /* Xm/List.c Xm/ScrolledW.c */
  {":XMN_COMMAND_WINDOW", &resclass_XmRWidget, XmNcommandWindow}, /* Xm/MainW.c */
  {":XMN_MENU_BAR", &resclass_XmRWidget, XmNmenuBar}, /* Xm/MainW.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_MESSAGE_WINDOW", &resclass_XmRWidget, XmNmessageWindow}, /* Xm/MainW.c */ /* a new resource for Motif 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_WORK_WINDOW", &resclass_XmRWidget, XmNworkWindow}, /* Xm/ScrolledW.c */
  {":XMN_CLIP_WINDOW", &resclass_XmRWidget, XmNclipWindow}, /* Xm/ScrolledW.c */
#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
  /* 
   * XmNdesktopParent/XmNlogicalParent resources are strange:
   * 	It seems to be a part of the extension/desktop object, not a widget;
   * 	It has no documentation.
   * Therefore, I'm assuming this is an internal resource and I'm not
   * going to bother interfacing it.
   */
  {":XMN_DESKTOP_PARENT", &resclass_XmRWidget, XmNdesktopParent}, /* Xm/Desktop.c: */
  {":XMN_LOGICAL_PARENT", &resclass_XmRWidget, XmNlogicalParent}, /* Xm/ExtObject.c */
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */
  /*----------------------------------------------------------------------------
   *------------------------------ "XmRWidgetClass" ----------------------------
   *----------------------------------------------------------------------------*/
  /*
   * Note: in Xm/RowColumn.c, XmNentryClass/XmCEntryClass resource is coded
   * as being of representation type XmRInt, when it's value is supposed to hold
   * a widget class pointer. So we create a fake, new representation type
   * XmRWidgetClass for this resource.
   */
  {":XMN_ENTRY_CLASS", &resclass_XmRWidgetClass, XmNentryClass}, /* Xm:RowColumn.c */
  /*----------------------------------------------------------------------------
   *----------------------------      XmRWindow      ---------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_WINDOW_GROUP", &resclass_XmRWindow, XmNwindowGroup}, /* Xm/Vendor.c Xt/Shell.c */
  {":XMN_ICON_WINDOW", &resclass_XmRWindow, XmNiconWindow}, /* Xt/Shell.c */
  /*----------------------------------------------------------------------------
   *----------------------------  XmRWindowGravity   ---------------------------
   *----------------------------------------------------------------------------*/
#ifdef WINTERP_MOTIF_11
  /*
   * Note -- resource XmNwinGravity is defined as XmRInt, but it is really an
   * enumerated type -- one of 10 different specifiers for "window gravity".
   * This is a fake representation type -- neither Motif nor Xt define "XmRWindowGravity"
   */
  {":XMN_WIN_GRAVITY", &resclass_XmRWindowGravity, XmNwinGravity}, /* Xt/Shell.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *-------------------------------- XmRXmString -------------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_DIALOG_TITLE", &resclass_XmRXmString, XmNdialogTitle},	/* Xm/BulletinB.c */
  {":XMN_PROMPT_STRING", &resclass_XmRXmString, XmNpromptString}, /* Xm/Command.c */
  {":XMN_COMMAND", &resclass_XmRXmString, XmNcommand}, /* Xm/Command.c */
  {":XMN_LABEL_STRING", &resclass_XmRXmString, XmNlabelString},	/* Xm/DrawnB.c Xm/Label.c Xm/LabelG.c Xm/RowColumn.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_DIRECTORY", &resclass_XmRXmString, XmNdirectory}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_PATTERN", &resclass_XmRXmString, XmNpattern}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
  {":XMN_DIR_LIST_LABEL_STRING", &resclass_XmRXmString, XmNdirListLabelString}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_FILTER_LABEL_STRING", &resclass_XmRXmString, XmNfilterLabelString}, /* Xm/FileSB.c */
  {":XMN_DIR_MASK", &resclass_XmRXmString, XmNdirMask},	/* Xm/FileSB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_NO_MATCH_STRING", &resclass_XmRXmString, XmNnoMatchString}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_DIR_SPEC", &resclass_XmRXmString, XmNdirSpec},	/* Xm/FileSB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_FILE_LIST_LABEL_STRING", &resclass_XmRXmString, XmNfileListLabelString}, /* Xm/FileSB.c */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_APPLY_LABEL_STRING", &resclass_XmRXmString, XmNapplyLabelString}, /* Xm/FileSB.c Xm/SelectioB.c */
  {":XMN_ACCELERATOR_TEXT", &resclass_XmRXmString, XmNacceleratorText},	/* Xm/Label.c Xm/LabelG.c */
  {":XMN_MESSAGE_STRING", &resclass_XmRXmString, XmNmessageString}, /* Xm/MessageB.c */
  {":XMN_OK_LABEL_STRING", &resclass_XmRXmString, XmNokLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_CANCEL_LABEL_STRING", &resclass_XmRXmString, XmNcancelLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_HELP_LABEL_STRING", &resclass_XmRXmString, XmNhelpLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_TITLE_STRING", &resclass_XmRXmString, XmNtitleString},	/* Xm/Scale.c */
  {":XMN_SELECTION_LABEL_STRING", &resclass_XmRXmString, XmNselectionLabelString}, /* Xm/SelectioB.c */
  {":XMN_LIST_LABEL_STRING", &resclass_XmRXmString, XmNlistLabelString}, /* Xm/FileSB.c Xm/SelectioB.c */
  {":XMN_TEXT_STRING", &resclass_XmRXmString, XmNtextString}, /* Xm/SelectioB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_OPTION_LABEL", &resclass_XmRXmString, XmNoptionLabel}, /* Xm/Simple.c -- XmSimpleMenu */ /* a new resource for MOTIF 1.1 */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *------------------------------ XmRXmStringTable ----------------------------
   *----------------------------------------------------------------------------*/
  {":XMN_HISTORY_ITEMS", &resclass_XmRXmStringTable, XmNhistoryItems}, /* Xm/Command.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_DIR_LIST_ITEMS", &resclass_XmRXmStringTable, XmNdirListItems}, /* Xm/FileSB.c */
  {":XMN_FILE_LIST_ITEMS", &resclass_XmRXmStringTable, XmNfileListItems}, /* Xm/FileSB.c */
#endif				/* WINTERP_MOTIF_11 */
  {":XMN_ITEMS", &resclass_XmRXmStringTable, XmNitems},	/* Xm/List.c */
  {":XMN_SELECTED_ITEMS", &resclass_XmRXmStringTable, XmNselectedItems}, /* Xm/List.c */
  {":XMN_LIST_ITEMS", &resclass_XmRXmStringTable, XmNlistItems}, /* Xm/SelectioB.c */
#ifdef WINTERP_MOTIF_11
  {":XMN_BUTTONS", &resclass_XmRXmStringTable, XmNbuttons}, /* Xm/Simple.c -- Note: creation-time only resource */
  {":XMN_BUTTON_ACCELERATOR_TEXT", &resclass_XmRXmStringTable, XmNbuttonAcceleratorText}, /* Xm/Simple.c -- Note: creation-time only resource */
#endif				/* WINTERP_MOTIF_11 */
  /*----------------------------------------------------------------------------
   *------ RESOURCES FOR THE HP GRAPH WIDGET (currently Motif 1.0 only) --------
   *----------------------------------------------------------------------------*/
#ifdef HP_GRAPH_WIDGET
  {":XMN_TO", &resclass_XmRWidget, XmNto}, /* XmGraph/Arc.c */
  {":XMN_FROM", &resclass_XmRWidget, XmNfrom}, /* XmGraph/Arc.c */
  {":XMN_INTERACTIVE_ARC_DIRECTION", &resclass_XmRArcDirection, XmNinteractiveArcDirection}, /* XmGraph/Graph.c */
  {":XMN_ARC_DIRECTION", &resclass_XmRArcDirection, XmNarcDirection}, /* XmGraph/Arc.c */
  {":XMN_HIGHLIGHT", &resclass_XmRBoolean, XmNhighlight}, /* XmGraph/Arc.c */  
  {":XMN_MAP_LABEL", &resclass_XmRBoolean, XmNmapLabel}, /* XmGraph/Arc.c */  
  {":XMN_MOVABLE_NODES", &resclass_XmRBoolean, XmNmovableNodes}, /* XmGraph/Graph.c */  
  {":XMN_AUTO_LAYOUT_MODE", &resclass_XmRBoolean, XmNautoLayoutMode}, /* XmGraph/Graph.c */  
  {":XMN_RE_LAYOUT", &resclass_XmRBoolean, XmNreLayout}, /* XmGraph/Graph.c */  
  {":XMN_REORIENT", &resclass_XmRBoolean, XmNreorient},	/* XmGraph/Graph.c */  
  {":XMN_TWINS_VISIBLE", &resclass_XmRBoolean, XmNtwinsVisible}, /* XmGraph/Graph.c */  
  {":XMN_DELTA", &resclass_XmRInt, XmNdelta}, /* XmGraph/Arc.c */
  {":XMN_ARC_WIDTH", &resclass_XmRInt, XmNarcWidth}, /* XmGraph/Arc.c */
  {":XMN_DASHES", &resclass_XmRInt, XmNdashes},	/* XmGraph/Arc.c */
  {":XMN_DASH_OFFSET", &resclass_XmRInt, XmNdashOffset}, /* XmGraph/Arc.c */
  {":XMN_CHILD_SPACING", &resclass_XmRShort, XmNchildSpacing}, /* XmGraph/Graph.c */
  {":XMN_SIBLING_SPACING", &resclass_XmRShort, XmNsiblingSpacing}, /* XmGraph/Graph.c */
  /*
    {":XmNfont", &resclass_XmRFontStruct, XmNfont},
    The XM_ARC_WIDGET_CLASS defines a resource XmNfont, but I'm ignoring it 
    for now because XmGraph will eventually support the correct resource -- XmNfontList
    */
  {":XMN_LABEL", &resclass_XmRString, XmNlabel}, /* XmGraph/Arc.c */
  {":XMN_DEFAULT_LABEL", &resclass_XmRXmString, XmNdefaultLabel}, /* XmGraph/Graph.c */
  {":XMN_STYLE", &resclass_XmRLineStyle, XmNstyle}, /* XmGraph/Arc.c */
  {":XMN_CAP_STYLE", &resclass_XmRCapStyle, XmNcapStyle}, /* XmGraph/Arc.c */
  {":XMN_ARC_DRAW_MODE", &resclass_XmRArcDrawMode, XmNarcDrawMode}, /* XmGraph/Graph.c */
  {":XMN_ARC_EDITED_CALLBACK", &resclass_XmRCallback, XmNarcEditedCallback}, /* XmGraph/Arc.c */
  {":XMN_NEW_ARC_CALLBACK", &resclass_XmRCallback, XmNnewArcCallback}, /* XmGraph/Graph.c */
  {":XMN_NEW_NODE_CALLBACK", &resclass_XmRCallback, XmNnewNodeCallback}, /* XmGraph/Graph.c */
  {":XMN_NODE_MOVED_CALLBACK", &resclass_XmRCallback, XmNnodeMovedCallback}, /* XmGraph/Graph.c */
  {":XMN_ARC_MOVED_CALLBACK", &resclass_XmRCallback, XmNarcMovedCallback}, /* XmGraph/Graph.c */
  {":XMN_DEFAULT_ACTION_CALLBACK", &resclass_XmRCallback, XmNdefaultActionCallback}, /* XmGraph/Graph.c */
  {":XMN_DELETE_NODE_CALLBACK", &resclass_XmRCallback, XmNdeleteNodeCallback}, /* XmGraph/Graph.c */
  {":XMN_DELETE_ARC_CALLBACK", &resclass_XmRCallback, XmNdeleteArcCallback}, /* XmGraph/Graph.c */
  {":XMN_SELECT_NODE_CALLBACK", &resclass_XmRCallback, XmNselectNodeCallback}, /* XmGraph/Graph.c */
  {":XMN_DESELECT_CALLBACK", &resclass_XmRCallback, XmNdeselectCallback}, /* XmGraph/Graph.c */
  {":XMN_SELECT_ARC_CALLBACK", &resclass_XmRCallback, XmNselectArcCallback}, /* XmGraph/Graph.c */
  {":XMN_SELECT_SUBGRAPH_CALLBACK", &resclass_XmRCallback, XmNselectSubgraphCallback}, /* XmGraph/Graph.c */
  {":XMN_DEFAULT_NODE_CLASS", &resclass_XmRWidgetClass, XmNdefaultNodeClass}, /* XmGraph/Graph.c */
#endif				/* HP_GRAPH_WIDGET */

#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT /* WINTERP IGNORES THE FOLLOWING RESOURCES: */
  {":XMN_SIMPLE_CALLBACK", &resclass_XmRCallbackProc, XmNsimpleCallback}, /* Xm/Simple.c */
  /*
   * XmNchildren and XmNnumChildren are retrieved through method
   * (send <composite> :get_children) -- wc_WIDGET.c:Widget_Class_Method_GET_CHILDREN().
   * Justification: these resources are read-only so it doesn't make sense to make a
   * normal XtGetValues()/XtSetValues() interface. Furthermore, since the array
   * XmNchildren isn't NULL terminated we'd have to pickup XmNnumChildren...
   * that would require a gross hack and it's much simpler to provide a method for that functionality.
   */
  {":XMN_NUM_CHILDREN", &resclass_XmRCardinal, XmNnumChildren},	/* Xt/Composite.c */ /* READ-ONLY resource */
  {":XMN_CHILDREN", &resclass_XmRWidgetList, XmNchildren},	/* Xt/Composite.c */ /* READ-ONLY resource */
  /*
   * XmNpostFromList and XmNpostFromCount seem to be internal resources on
   * XmRowColumn, so I'm not going to interface 'em. If you need these, add
   * a method on XM_ROW_COLUMN_WIDGET_CLASS that is similar to
   * wc_WIDGET.c:Widget_Class_Method_GET_CHILDREN().
   */
  {":XMN_POST_FROM_COUNT", &resclass_XmRInt, XmNpostFromCount},	/* Xm/RowColumn.c */ /* READ-ONLY resource */
  {":XMN_POST_FROM_LIST", &resclass_XmRWidgetList, XmNpostFromList},	/* Xm/RowColumn.c */ /* READ-ONLY resource */
  {":XMN_COLORMAP", &resclass_XtRColormap, XmNcolormap},	/* Xt/Core.c Xt/Shell.c */
  /*
   * TO-DO LATER For now, I'm not going to bother with these resources, since
   * I don't have any other atom-level interfaces in WINTERP yet. 
   */
  /* {":XMN_ATOM", &resclass_XmRAtom, XmNatom}, -- looks like a non-widget resource */ /* Xm/Protocols.c */
  {":XMN_TITLE_ENCODING", &resclass_XmRAtom, XmNtitleEncoding}, /* Xt/Shell.c */
  {":XMN_ICON_NAME_ENCODING", &resclass_XmRAtom, XmNiconNameEncoding}, /* Xt/Shell.c */
  /* 
   * XmRExtensionType resource XmNextensionType is a strange resource:
   * 	It seems to be a part of the extension object, not a widget;
   * 	It has no external resource converter interface;
   * 	It has no documentation.
   * Therefore, I'm assuming this is an internal resource and I'm not
   * going to bother interfacing it.
   */
  {":XMN_EXTENSION_TYPE", &resclass_XmRExtensionType, XmNextensionType}, /* Xm/Desktop.c Xm/ExtObject.c Xm/Protocols.c Xm/VendorE.c */
  /* 
   * XmRFunction resources can't be set/accessed in WINTERP:
   * This resource class is a C function-pointer. The kinds of things done by
   * these procedures are probalby too low-level to interface to the WINTERP-Lisp
   * evaluator (i.e. an interface making them work like callbacks).
   * If you need access to these in WINTERP the simplest thing to do is create a
   * trivial subclass of the desired widget such that this resource gets set to the
   * desired creation/insert/popup procedure that is implemented in C
   */
  {":XMN_OUTPUT_CREATE", &resclass_XmRFunction, XmNoutputCreate}, /* Xm/Text.c */
  {":XMN_INPUT_CREATE", &resclass_XmRFunction, XmNinputCreate},	/* Xm/Text.c */
  {":XMN_INSERT_POSITION", &resclass_XmRFunction, XmNinsertPosition}, /* Xt/Composite.c */
  {":XMN_CREATE_POPUP_CHILD_PROC", &resclass_XmRFunction, XmNcreatePopupChildProc}, /* Xt/Shell.c */
  /*
   * NOTE: since we're using the userdata field to hold a backpointer to the
   * widget object, then we shouldn't make it accessible to the user
   */ 
   {:XMN_USER_DATA,  &resclass_XmRPointer,  XmNuserData}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c */
  /*
   * You should write XmText XmNsource procedures in C anyways...
   * I'm not going to try to figure out how to interface this.
   */
  {":XMN_SOURCE", &resclass_XmRPointer, XmNsource}, /* Xm/Text.c */
  /*
   * XmRProc is a function pointer to a function declared:
   * void DoFileSearch(fs, search_data)
   *	XmFileSelectionBoxWidget fs;
   *	XmFileSelectionBoxCallbackStruct *search_data;
   *
   * I don't expect that people will be setting the file search procedure
   * inside lisp, since it needs to to alot of low level munging with the
   * list widget, therefore, for now, I'm going to ignore this type, and the
   * two associated resources. There's plenty of more important things to
   * get working... -- NPM.
   */
  {":XMN_QUALIFY_SEARCH_DATA_PROC", &resclass_XmRProc, XmNqualifySearchDataProc}, /* Xm/FileSB.c */
  {":XMN_DIR_SEARCH_PROC", &resclass_XmRProc, XmNdirSearchProc}, /* Xm/FileSB.c */
  {":XMN_FILE_SEARCH_PROC", &resclass_XmRProc, XmNfileSearchProc}, /* Xm/FileSB.c */
  {":XMN_SCREEN", &resclass_XmRScreen, XmNscreen}, /* Xt/Core.c */
  {":XMN_VISUAL", &resclass_XmRVisual, XmNvisual}, /* Xt/Shell.c */
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */

  {NULL, NULL, NULL, NULL}	/* BY ALL MEANS NECESSARY -- end of table marker */
};

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define FREEABLES_SIZE_INCREMENT 20
static struct Argval_Deallocator_Pair {
  XtPointer pointer;
  void (*deallocator)();
} *freeables; /* initialized in Wres_Init() */
static int freeables_size;	/* maximum index based on size of freeables */
static int freeables_end_idx;	/* index of last elt in freeables */

/******************************************************************************
 * Wres_Free_C_Arglist_Data():
 *
 * This procedure frees any unneeded structures that were created by
 * Wres_Get_LispArglist(). It should be called only after the C arglist
 * returned by Wres_Get_LispArglist() is used by a widget creation routine
 * (see :isnew methods on widgets) or by XtSetValues().
 ******************************************************************************/
void Wres_Free_C_Arglist_Data()
{
  int i = 0;
  while (i < freeables_end_idx) {
    (*(freeables[i].deallocator))(freeables[i].pointer);
    i++;
  }
  freeables_end_idx = 0;	/* reset */
}


/******************************************************************************
 * Wres_Free_This_Later():
 *
 * Stores a pointer to an allocated chunk of memory, and a procedure to deallocate
 * that memory. This will get called upon calling Wres_Free_C_Arglist_Data().
 ******************************************************************************/
static void Wres_Free_This_Later(pointer, deallocator)
     XtPointer pointer;
     void (*deallocator)();
{
  if (freeables_end_idx >= freeables_size) {
    freeables = (struct Argval_Deallocator_Pair *) XtRealloc(freeables, (unsigned) ((freeables_size + FREEABLES_SIZE_INCREMENT) * sizeof(struct Argval_Deallocator_Pair)));
    freeables_size += FREEABLES_SIZE_INCREMENT;
  }
  freeables[freeables_end_idx].pointer = pointer;
  freeables[freeables_end_idx].deallocator = deallocator;
  freeables_end_idx++;
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

static Pixmap Wres_Get_Pixmap(scr, fg, bg, lval_resval, resource, o_widget)
     Screen*            scr;
     Pixel              fg;
     Pixel              bg;
     LVAL               lval_resval; /* STRING */
     Resource_Instance* resource;
     LVAL               o_widget; /* WIDGETOBJ */
{
  extern void Wpm_Set_Pixmap_Reference(); /* w_pixmap.c */
  Pixmap result;

  if ((result = XmGetPixmap(scr, (char*) getstring(lval_resval), fg, bg)) == XmUNSPECIFIED_PIXMAP) {
    sprintf(temptext,
	    "Resource %s (type XmR%s) -- XmGetPixmap() couldn't create a pixmap from given string pixmap specification.",
	    resource->printname,
	    resource->class->XmR_type);
    xlerror(temptext, lval_resval);
  }
  Wpm_Set_Pixmap_Reference(cv_pixmap(result), o_widget, resource->symbol);
  return (result);
}

/*****************************************************************************
 * Wres_Get_LispArglist():
 *
 * This routine will get all remaining arguments off of the argument stack and
 * treat them as a list of name/value pairs for arguments. It returns an ArgList
 * representing the Xtoolkit version of the arglist passed in, which MUST NOT
 * BE FREED BY THE CALLER.
 *
 * This subroutine will signal an error if
 * (1) an invalid resource name is used;
 * (2) the value associated with the resource name is of the wrong type
 *     (each resource_name expects a unique resource value, if the value
 *     is a string instead of a value of the expected type, then a resource
 *     converter is called to convert this value. That may too result in
 *     an error being signaled if the resource converter couldn't be found...)
 *
 * This routine will return with the argument stack empty, or it will signal
 * an error.
 ****************************************************************************/
#define XTARGLIST_SIZE_INCREMENT 20
static ArgList xt_arglist;	/* init'd in Wres_Init() */
static Cardinal xt_arglist_size; /* init'd in Wres_Init() */
ArgList Wres_Get_LispArglist(o_widget, widgetID, prepend_args, prepend_nargs, nargs)
     LVAL     o_widget;
     Widget   widgetID;		/* for calling XtConvert if type conv needed */
     ArgList  prepend_args;	/* arguments to be prepended to result */
     Cardinal prepend_nargs;	/* number of arguments to be prepended */
     Cardinal *nargs;		/* returns number of args in result */
{
  ArgList             xt_arg;
  Cardinal            lisp_nargs;
  LVAL                lval_resname, lval_resval;
  Resource_Instance  *resource;

  /*
   * Various XmRString-->XmR* resource-type converters below will use value of
   * widgetID to access widget-internal structures such as core.screen core.depth
   * core.background_pixel core.colormap  manager.foreground primitive.foreground
   * manager.highlight_color primitive.highlight_color
   * manager.top_shadow_color primitive.top_shadow_color 
   * 
   * Therefore, if widget is a gadget, then set widgetID to gadget's manager to
   * avoid problems with XtConvert() accessing nonexistant fields
   * in gadget rec. 
   */
  if (XmIsGadget(widgetID))
    widgetID = ((Object) widgetID)->object.parent;
    
  if ((xlargc & 1) != 0)	/* if there's an odd number of args on the stack */
    xlerror("Widget argument list must consist of pairs of resource-name & value.\n	An odd number of arguments were found, the first being:",
	    *xlargv);

  lisp_nargs = xlargc / 2;	/* the number of name/value pairs */
  *nargs = lisp_nargs + prepend_nargs;

  if (*nargs > xt_arglist_size) { /* make sure xt_arglist is big enough */
    xt_arglist = (ArgList) XtRealloc(xt_arglist, (unsigned) (*nargs * sizeof(Arg)));
    xt_arglist_size = *nargs;
  }
  xt_arg = xt_arglist;

  /* copy prepend_args into result ArgList */
  while (prepend_nargs--)
    *xt_arg++ = *prepend_args++;

  /*
   * now append lisp_arglist into result ArgList:
   * starting at xt_arg,
   * while more name/value pairs to process
   * 		get resource name, set xt_arg->name
   *		get lisp resource value
   *		convert lisp value to C value
   *		set xt_arg->value
   *		go to next xt_arg.
   */
  for (	/* xt_arg */ ; lisp_nargs-- ; xt_arg++) {	
    /* 
     * get resource name
     */
    lval_resname = nextarg();
    if (xtresource_p(lval_resname))
      resource = get_xtresource(lval_resname);
    else if (symbolp(lval_resname)) { /* hasn't been eVALuated yet... */
      lval_resname = getvalue(lval_resname); /* get the value */
      if (xtresource_p(lval_resname))
	resource = get_xtresource(lval_resname);
      else
	xlerror("In Widget Arglist: Invalid resource keyword in widget resource list.", lval_resname);
    }
    else
      xlerror("In Widget Arglist: Invalid resource name in widget resource list.", lval_resname);

    xt_arg->name = resource->name;

    /* 
     * get resource value.
     */
    lval_resval = nextarg();

    /* 
     * check to see if the lval_resval is of the type corresponding to resource.
     * And if it is, then call appropriate LVAL-->XtArgVal procedure to convert
     * from a lisp argument to the C equivalent.
     */
    if ((lval_resval && (ntype(lval_resval) == resource->class->LVAL_type))
	|| (resource->class->LVAL_type == FREE)) /* FREE stands for any LVAL type (eg, for XmRBoolean, XmRXmStringTable) */
      xt_arg->value = (*(resource->class->LVAL_to_resource_val_converter))(lval_resval, resource, o_widget);

    /*
     * Otherwise, if lisp value is a string, try to do a string to resource->class conversion.
     */
    else if (stringp(lval_resval)) {

      /* 
       * If the lisp value is a string, and we expect an XmString, override the
       * default XmRString-->XmRXmString conversion that would be done below
       * (via XtConvert()) since the Motif 1.0 built-in resource converter
       * _XmCvtStringToXmString() uses XmStringCreate(). We want to use
       * XmStringCreateLtoR() which will allow us to create XmStrings with
       * multiple lines by embedding '\n' characters inside our strings.
       * Also, by preventing XtConvert() below from doing the String-->XmString
       * conversion, we can free any converted XmStrings without having to worry
       * about Xt's kludgy resource cacheing getting in our way.
       */
      if (resource->class == &resclass_XmRXmString) {
	xt_arg->value = (XtArgVal) XmStringCreateLtoR((char*) getstring(lval_resval), XmSTRING_DEFAULT_CHARSET);
	Wres_Free_This_Later((XtPointer) xt_arg->value, XmStringFree); /* XmStrings are copied into widget upon create/setvalues, so this XmString may be freed later */
      }

      /*
       * Convert a string-->Pixmap, using foreground color as foreground of Pixmap.
       * This handles resources declared as XmRPrimForegroundPixmap,
       * XmRGadgetPixmap, XmRManForegroundPixmap. Note that if the widgetID
       * passed into Wres_Get_LispArglist() is a gadget, code above will
       * set widgetID to the gadget's manager, thus handling gadgets and
       * XmRString-->XmRGadgetPixmap conversions correctly.
       */
      else if (resource->class == &resclass_ForegroundPixmap) {
	extern Screen* screen;	/* winterp.c */
	extern Pixel default_foreground_pixel, default_background_pixel; /* winterp.c */

	if (XmIsManager(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmManagerWidget) widgetID)->core.screen,
						     ((XmManagerWidget) widgetID)->manager.foreground,
						     ((XmManagerWidget) widgetID)->core.background_pixel,				     
						     lval_resval, resource, o_widget);
	else if (XmIsPrimitive(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmPrimitiveWidget) widgetID)->core.screen,
						     ((XmPrimitiveWidget) widgetID)->primitive.foreground,
						     ((XmPrimitiveWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XtIsShell(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((ShellWidget) widgetID)->core.screen,
						     default_foreground_pixel,
						     ((ShellWidget) widgetID)->core.background_pixel,				     
						     lval_resval, resource, o_widget);
	else
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(screen,
						     default_foreground_pixel,
						     default_background_pixel,
						     lval_resval, resource, o_widget);
      }

      /*
       * Convert a string-->Pixmap, using highlight color as foreground of Pixmap.
       * This handles resources declared as XmRPrimHighlightPixmap XmRManHighlightPixmap
       */
      else if (resource->class == &resclass_HighlightPixmap) {
	extern Screen* screen;	/* winterp.c */
	extern Pixel default_foreground_pixel, default_background_pixel; /* winterp.c */

	if (XmIsManager(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmManagerWidget) widgetID)->core.screen,
						     ((XmManagerWidget) widgetID)->manager.highlight_color,
						     ((XmManagerWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XmIsPrimitive(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmPrimitiveWidget) widgetID)->core.screen,
						     ((XmPrimitiveWidget) widgetID)->primitive.highlight_color,
						     ((XmPrimitiveWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XtIsShell(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((ShellWidget) widgetID)->core.screen,
						     default_foreground_pixel,
						     ((ShellWidget) widgetID)->core.background_pixel,				     
						     lval_resval, resource, o_widget);
	else
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(screen,
						     default_foreground_pixel,
						     default_background_pixel,
						     lval_resval, resource, o_widget);
      }

      /*
       * Convert a string-->Pixmap, using topshadow color as foreground of Pixmap.
       * This handles resources declared as XmRPrimTopShadowPixmap XmRManTopShadowPixmap.
       */
      else if (resource->class == &resclass_TopShadowPixmap) {
	extern Screen* screen;	/* winterp.c */
	extern Pixel default_foreground_pixel, default_background_pixel; /* winterp.c */

	if (XmIsManager(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmManagerWidget) widgetID)->core.screen,
						     ((XmManagerWidget) widgetID)->manager.top_shadow_color,
						     ((XmManagerWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XmIsPrimitive(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmPrimitiveWidget) widgetID)->core.screen,
						     ((XmPrimitiveWidget) widgetID)->primitive.top_shadow_color,
						     ((XmPrimitiveWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XtIsShell(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((ShellWidget) widgetID)->core.screen,
						     default_foreground_pixel,
						     ((ShellWidget) widgetID)->core.background_pixel,				     
						     lval_resval, resource, o_widget);
	else
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(screen,
						     default_foreground_pixel,
						     default_background_pixel,
						     lval_resval, resource, o_widget);
      }

      /*
       * Convert a string-->Pixmap, using bottomshadow color as foreground of Pixmap.
       * This handles resources declared as XmRPrimBottomShadowPixmap XmRManBottomShadowPixmap.
       */
      else if (resource->class == &resclass_BottomShadowPixmap) {
	extern Screen* screen;	/* winterp.c */
	extern Pixel default_foreground_pixel, default_background_pixel; /* winterp.c */

	if (XmIsManager(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmManagerWidget) widgetID)->core.screen,
						     ((XmManagerWidget) widgetID)->manager.bottom_shadow_color,
						     ((XmManagerWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XmIsPrimitive(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((XmPrimitiveWidget) widgetID)->core.screen,
						     ((XmPrimitiveWidget) widgetID)->primitive.bottom_shadow_color,
						     ((XmPrimitiveWidget) widgetID)->core.background_pixel,
						     lval_resval, resource, o_widget);
	else if (XtIsShell(widgetID))
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(((ShellWidget) widgetID)->core.screen,
						     default_foreground_pixel,
						     ((ShellWidget) widgetID)->core.background_pixel,				     
						     lval_resval, resource, o_widget);
	else
	  xt_arg->value = (XtArgVal) Wres_Get_Pixmap(screen,
						     default_foreground_pixel,
						     default_background_pixel,
						     lval_resval, resource, o_widget);
      }

      /*
       * For XmRXmBackgroundPixmap, we must call Motif resource converter since it has a special
       * side effect. (Which is a very ugly hack on the part of the Motif implementation).
       * If Visual.c:_XmCvtStringToBackgroundPixmap() changes, then we may need to add conversion
       * routines similar to above.
       */
#ifdef THE_FOLLOWING_CODE_IS_COMMENTED_OUT
#ifdef WINTERP_MOTIF_11
      else if (resource->class == &resclass_XmRXmBackgroundPixmap) {
      }
#else				/* MOTIF 1.0 */
      else if (resource->class == &resclass_XmRPixmap) { /* Motif 1.0 also uses a weird side-effecting converter for XmRPixmap */
      }
#endif				/* WINTERP_MOTIF_11 */
#endif				/* THE_FOLLOWING_CODE_IS_COMMENTED_OUT */

      /* 
       * if none of the conversions above matched, we call XtConvert() to convert
       * the resource from a string representaion to the representation type 
       * specified in resource->class->XmR_type.
       */
      else {
	XrmValue from, to;
	from.size = (unsigned int) getslength(lval_resval);
	from.addr = (XtPointer) getstring(lval_resval);
	XtConvert(widgetID, XmRString, &from, resource->class->XmR_type, &to);
	if (to.addr == NULL) {	/* error if conversion failed */
	  sprintf(temptext, "In Widget Arglist, error in converting XmRstring-->XmR%s.", resource->class->XmR_type);
	  xlerror(temptext, lval_resval);	
	}
	xt_arg->value = *((XtArgVal *) to.addr);
      }
    }				/* end: "else if (stringp(lval_resval))" */
    else {
      sprintf(temptext, "In Widget Arglist: expected resource value of type XmR%s.", resource->class->XmR_type);
      xlerror(temptext, lval_resval);
    }
  }				/* end: "for ( ; lisp_nargs-- ; xt_arg++)" */
  return (xt_arglist);
}


/******************************************************************************
 * Wres_Get_GetValues_ArgList() // Wres_GetValues_ArgList_To_Lisp()
 * 
 * Wres_Get_GetValues_ArgList() will get all remaining arguments off of the
 * argument stack and treat them as a list of name/symbol pairs for arguments
 * to WIDGET_CLASS method :get_values. It returns an ArgList representing
 * the Xtoolkit version of the arglist passed in, which MUST NOT BE FREED
 * BY THE CALLER. XtGetValues() should then be called on the arglist, and
 * then the symbols from the arglist get set to values retrieved from
 * XtGetValues() in proc Wres_GetValues_ArgList_To_Lisp()
 * 
 *
 * Wres_Get_GetValues_ArgList() will signal an error if
 * (1) an invalid resource name is used;
 * (2) lack of C->Lisp converter, or error in conversion.
 *
 * Wres_Get_GetValues_ArgList() will return with the argument stack empty,
 * or it will signal an error.
 ******************************************************************************/

typedef struct _GetValues_Info {

  /*
   * this MUST be the first field in this struct. struct _GetValues_Info is
   * pointed to by (XtArgVal).value and this location gets written to by
   * XtGetValues(). XtGetValues() ends up writing data up to
   * sizeof(XtArgVal)==sizeof(XtPointer) at the location pointed to by
   * (XtArgVal).value. Note that since between 1-4 bytes get written here,
   * we can't use casting to retrieve the value, rather we must retrieve the
   * type by accessing a union member when data is passed in to the
   * function (*resource_val_to_LVAL_converter)().
   */
  GetValues_Union data; 

  /*
   * a pointer to the resource information, including the function that takes 'data'
   * slot and returns it's LISP equivalent:
   * (->resource->class->resource_val_to_LVAL_converter)(GetValues_Union data ; Resource_Instance* resource; )
   */
  Resource_Instance* resource;

  /*
   * a 'setf' place form for telling Wres_GetValues_ArgList_To_Lisp()
   * where to put getvalues result
   */
  LVAL place_for_result;

} GetValues_Info;

#define GETARGLIST_SIZE_INCREMENT 20
static ArgList getarglist;	/* init'd in Wres_Init() */
static Cardinal getarglist_size; /* init'd in Wres_Init() */

ArgList Wres_Get_GetValues_ArgList(numargs)
     Cardinal *numargs;
{
  ArgList             xt_arg;
  Cardinal            argcount;

  if ((xlargc & 1) != 0)	/* if there's an odd number of args on the stack */
    xlerror(":get_values argument list must consist of pairs of resource-name & symbol.\n	An odd number of arguments were found, the first being:",
	    *xlargv);
  
  *numargs = argcount = xlargc / 2; /* the number of name/value pairs */

  if (argcount > getarglist_size) { /* make sure getarglist is big enough */
    getarglist = (ArgList) XtRealloc(getarglist, (unsigned) (argcount * sizeof(Arg)));
    while (getarglist_size < argcount)
      getarglist[getarglist_size++].value = (XtArgVal) XtMalloc(sizeof(GetValues_Info)); /* note bogus jacking in of object larger than XtArgVal for use by Wres_GetValues_ArgList_To_Lisp() */
    /* xt_arglist_size = argcount; */
  }

  for (xt_arg = getarglist;  (argcount--); xt_arg++) { /* while more name/value pairs to process */
    Resource_Instance  *resource;
    GetValues_Info     *elt;
    LVAL                lval_resname;

    /* 
     * get <resource-name> keyword and retrieve its Resource_Instance structure
     */
    lval_resname = nextarg();
    if (xtresource_p(lval_resname))
      resource = get_xtresource(lval_resname);
    else if (symbolp(lval_resname)) { /* hasn't been eVALuated yet... */
      lval_resname = getvalue(lval_resname); /* get the value */
      if (xtresource_p(lval_resname))
	resource = get_xtresource(lval_resname);
      else
	xlerror("Invalid resource keyword in widget :get_values resource list.", lval_resname);
    }
    else
      xlerror("Invalid resource name in widget :get_values resource list.", lval_resname);
    xt_arg->name = resource->name;

    elt = (GetValues_Info *) xt_arg->value;

    /*
     * get <placeform> argument indicating where retrieved value should go.
     */
    elt->place_for_result = xlgetarg();	/* get a place to put val */
    if (!( null(elt->place_for_result) || symbolp(elt->place_for_result) || consp(elt->place_for_result) ))
      xlerror(":get_values expected a <place> form, either NIL, a symbol, or a place-form.", elt->place_for_result);
    
    /* 
     * Set the Resource_Instance pointer for the resource class.
     */
    elt->resource = resource;

    /*
     * Set the 'data' slot to a known-but-implausible XtPointer-sized value so that we can
     * check (in Wres_GetValues_ArgList_To_Lisp()) whether XtGetValues() writes into the data
     * slot. We check for this value in Wres_GetValues_ArgList_To_Lisp() and report an error
     * if the value is still there. CAN YOU SAY HACK?? SURE YOU CAN...
     */
    elt->data.XtPointer_value = (XtPointer) LONG_MIN; /* LONG_MIN from <limits.h> -- if this is a 2'sC machine, sets 'data' to 11111111 11111111 11111111 11111111 */
    
  }
  return (getarglist);
}


LVAL Wres_GetValues_ArgList_To_Lisp(arglist, numargs)
     ArgList arglist;
     Cardinal numargs;
{
  GetValues_Info *elt;
  LVAL result,cur,next,lval = NIL;
  xlstkcheck(3);
  xlsave(result);
  xlsave(next);
  xlsave(lval);
    
  while (numargs--) {
    elt = (GetValues_Info *) (*arglist++).value;

    /* 
     * A quick check to see if the resource name was valid for this widget.
     * In Wres_Get_GetValues_ArgList(), we set elt->data.XtPointer_value = LONG_MIN;
     * If the resource was invalid, this value won't get touched. Therefore we
     * check to see if LONG_MIN is still there, and if so, we report an error.
     */
    if (elt->data.XtPointer_value == (XtPointer) LONG_MIN)
      xlerror("in :GET_VALUES -- invalid resource name for this widget class", elt->resource->symbol);
    
    /*
     * Convert elt->data (set by XtGetValues() call) to a lisp value depending on the
     * resource type. elt->data is written into by XtGetValues which only writes
     * the number of bytes required by the type of the resource. Since the type of the
     * data written is dependent on the resource-type, we cannot just coerce the result
     * to a XtPointer and pass that onto the (resource_val_to_LVAL_converter)() conversion
     * routine. Instead, we define elt->data as a union of all the possible types used by
     * Xt resources, and the (resource_val_to_LVAL_converter)() will then use the
     * appropriate part of the union depending on the type of data it expects to retrieve.
     * The converter then returns the appropriate lisp value.
     */
    lval = (*(elt->resource->class->resource_val_to_LVAL_converter))(elt->data, elt->resource);

    /*
     * set retrieved & converted resource value to <placeform> set in elt->place_for_result.
     */
    if (!(elt->place_for_result)) { /* if NIL, then return the value as function's result */
      next = cons(lval, NIL);
      if (result) {
	rplacd(cur, next);
	cur = next;
      }
      else
	result = cur = next;
    }
    else			/* stolen from xsetf() */
      if (symbolp(elt->place_for_result)) /* if place_for_result is a symbol, set the symbol's value to lval */
	xlsetvalue(elt->place_for_result, lval);
      else if (consp(elt->place_for_result)) /* else if it's a placeform, then set lval to datastruct spec'd by placeform */
	placeform(elt->place_for_result, lval);
      else
	xlfail("bad place form");
  }
  
  xlpopn(3);
  return (result);
}


/*****************************************************************************
 * This accesses the symbol value of a resource instance object (whose type
 * is opaque outside of this module). It is used in xlprint.c:xlprint().
 *
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ****************************************************************************/
LVAL Wres_Get_Symbol(res)
     LVAL res;
{
  return (get_xtresource(res)->symbol);
}

/*****************************************************************************
 * This accesses the XmN* name of a resource instance object (whose type
 * is opaque outside of this module). 
 *
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ****************************************************************************/
char* Wres_Get_Name(res)
     LVAL res;
{
  return (get_xtresource(res)->name);
}

/******************************************************************************
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ******************************************************************************/ 
Boolean Wres_Is_Callback_P(res)
     LVAL res;
{
  return (get_xtresource(res)->class == &resclass_XmRCallback);
}


/*****************************************************************************
 * This initializes the resource symbols in xlisp. It is called in main().
 ****************************************************************************/
Wres_Init()
{

  /* initialize resource symbols and values */
  {
    register Resource_Instance *resource;
    register LVAL              sym;

    for (resource = &(resource_table[0]); resource->printname; resource++) {
      sym = resource->symbol = xlenter(resource->printname);
      setvalue(sym, cv_xtresource(resource));
    }
  }

  /* set up storage space for Wres_Get_LispArglist() */
  xt_arglist = (ArgList) XtMalloc((unsigned) (XTARGLIST_SIZE_INCREMENT * sizeof(Arg)));
  xt_arglist_size = XTARGLIST_SIZE_INCREMENT;

  /* set up storage space used by Wres_Free_C_Arglist_Data() and Wres_Free_This_Later() */
  freeables_size = FREEABLES_SIZE_INCREMENT;
  freeables_end_idx = 0;
  freeables = (struct Argval_Deallocator_Pair *) XtMalloc((unsigned) (FREEABLES_SIZE_INCREMENT * sizeof(struct Argval_Deallocator_Pair)));

  /* set up storage space for Wres_Get_GetValues_ArgList() and Wres_GetValues_ArgList_To_Lisp() */
  getarglist = (ArgList) XtMalloc((unsigned) (GETARGLIST_SIZE_INCREMENT * sizeof(Arg))); 
  getarglist_size = 0;
  while (getarglist_size < GETARGLIST_SIZE_INCREMENT)
    getarglist[getarglist_size++].value = (XtArgVal) XtMalloc(sizeof(GetValues_Info)); /* note bogus jacking in of object larger than XtArgVal for use by Wres_Set_Lisp_Vars_From_GetValues_ArgList() */
  /* getarglist_size = GETARGLIST_SIZE_INCREMENT */

  /* initialize keyword symbols defined in 'Resource_Enums' tables */
  Init_Enumerated_Type_Syms(XmRAlignment_enums_alist);
  Init_Enumerated_Type_Syms(XmRArrowDirection_enums_alist);
  Init_Enumerated_Type_Syms(XmRAttachment_enums_alist);
  Init_Enumerated_Type_Syms(XmRDefaultButtonType_enums_alist);
  Init_Enumerated_Type_Syms(XmRDeleteResponse_enums_alist);
  Init_Enumerated_Type_Syms(XmRDialogStyle_enums_alist);
  Init_Enumerated_Type_Syms(XmRDialogType_enums_alist);
  Init_Enumerated_Type_Syms(XmREditMode_enums_alist);
  Init_Enumerated_Type_Syms(XmRIndicatorType_enums_alist);
  Init_Enumerated_Type_Syms(XmRKeyboardFocusPolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmRLabelType_enums_alist);
  Init_Enumerated_Type_Syms(XmRListSizePolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmROrientation_enums_alist);
  Init_Enumerated_Type_Syms(XmRPacking_enums_alist);
  Init_Enumerated_Type_Syms(XmRProcessingDirection_enums_alist);
  Init_Enumerated_Type_Syms(XmRResizePolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmRRowColumnType_enums_alist);
  Init_Enumerated_Type_Syms(XmRScrollBarDisplayPolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmRScrollBarPlacement_enums_alist);
  Init_Enumerated_Type_Syms(XmRScrollingPolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmTextScanType_enums_alist);
  Init_Enumerated_Type_Syms(XmRSelectionPolicy_enums_alist);
  Init_Enumerated_Type_Syms(XmRSeparatorType_enums_alist);
  Init_Enumerated_Type_Syms(XmRShadowType_enums_alist);
  Init_Enumerated_Type_Syms(XmRStringDirection_enums_alist);
  Init_Enumerated_Type_Syms(XmRWhichButton_enums_alist);
  Init_Enumerated_Type_Syms(XmRUnitType_enums_alist);
  Init_Enumerated_Type_Syms(XmRVisualPolicy_enums_alist);
#ifdef WINTERP_MOTIF_11
  Init_Enumerated_Type_Syms(XmRButtonType_enums_alist);
  Init_Enumerated_Type_Syms(XmRWindowGravity_enums_alist);
  Init_Enumerated_Type_Syms(XmRCommandWindowLocation_enums_alist);
  Init_Enumerated_Type_Syms(XmRFileTypeMask_enums_alist);
  Init_Enumerated_Type_Syms(XtRInitialState_enums_alist);
  Init_Enumerated_Type_Syms(XmRMultiClick_enums_alist);
  Init_Enumerated_Type_Syms(XmRNavigationType_enums_alist);
#endif
#ifdef HP_GRAPH_WIDGET
  Init_Enumerated_Type_Syms(XmRArcDirection_enums_alist);
  Init_Enumerated_Type_Syms(XmRLineStyle_enums_alist);
  Init_Enumerated_Type_Syms(XmRCapStyle_enums_alist);
  Init_Enumerated_Type_Syms(XmRArcDrawMode_enums_alist);
#endif

  /* initialize keyword symbols used by :get_child methods on various widget classes */
  s_XmDIALOG_WORK_AREA		= xlenter(":DIALOG_WORK_AREA");
  s_XmDIALOG_APPLY_BUTTON	= xlenter(":DIALOG_APPLY_BUTTON");
  s_XmDIALOG_CANCEL_BUTTON	= xlenter(":DIALOG_CANCEL_BUTTON");
  s_XmDIALOG_DEFAULT_BUTTON	= xlenter(":DIALOG_DEFAULT_BUTTON");
  s_XmDIALOG_OK_BUTTON		= xlenter(":DIALOG_OK_BUTTON");
#ifdef WINTERP_MOTIF_11
  s_XmDIALOG_DIR_LIST		= xlenter(":DIALOG_DIR_LIST");
  s_XmDIALOG_DIR_LIST_LABEL	= xlenter(":DIALOG_DIR_LIST_LABEL");
#endif				/* WINTERP_MOTIF_11 */
  s_XmDIALOG_FILTER_LABEL	= xlenter(":DIALOG_FILTER_LABEL");
  s_XmDIALOG_FILTER_TEXT	= xlenter(":DIALOG_FILTER_TEXT");
  s_XmDIALOG_HELP_BUTTON	= xlenter(":DIALOG_HELP_BUTTON");
  s_XmDIALOG_LIST		= xlenter(":DIALOG_LIST");
  s_XmDIALOG_HISTORY_LIST	= xlenter(":DIALOG_HISTORY_LIST");
  s_XmDIALOG_LIST_LABEL		= xlenter(":DIALOG_LIST_LABEL");
  s_XmDIALOG_MESSAGE_LABEL	= xlenter(":DIALOG_MESSAGE_LABEL");
  s_XmDIALOG_SELECTION_LABEL	= xlenter(":DIALOG_SELECTION_LABEL");
  s_XmDIALOG_PROMPT_LABEL	= xlenter(":DIALOG_PROMPT_LABEL");
  s_XmDIALOG_SYMBOL_LABEL	= xlenter(":DIALOG_SYMBOL_LABEL");
  s_XmDIALOG_TEXT		= xlenter(":DIALOG_TEXT");
  s_XmDIALOG_COMMAND_TEXT	= xlenter(":DIALOG_COMMAND_TEXT");
  s_XmDIALOG_SEPARATOR		= xlenter(":DIALOG_SEPARATOR");

  {
    LVAL sym;

    /* logior-able values for resource :XMN_MWM_FUNCTIONS */
    sym = xlenter("MWM_FUNC_ALL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_ALL));
    sym = xlenter("MWM_FUNC_RESIZE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_RESIZE));
    sym = xlenter("MWM_FUNC_MOVE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_MOVE));
    sym = xlenter("MWM_FUNC_MINIMIZE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_MINIMIZE));
    sym = xlenter("MWM_FUNC_MAXIMIZE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_MAXIMIZE));
    sym = xlenter("MWM_FUNC_CLOSE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_FUNC_CLOSE));

    /* logior-able values for resource :XMN_MWM_DECORATIONS */
    sym = xlenter("MWM_DECOR_ALL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_ALL));
    sym = xlenter("MWM_DECOR_BORDER");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_BORDER));
    sym = xlenter("MWM_DECOR_RESIZEH");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_RESIZEH));
    sym = xlenter("MWM_DECOR_TITLE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_TITLE));
    sym = xlenter("MWM_DECOR_MENU");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_MENU));
    sym = xlenter("MWM_DECOR_MINIMIZE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_MINIMIZE));
    sym = xlenter("MWM_DECOR_MAXIMIZE");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_DECOR_MAXIMIZE));

    /* logior-able values for resource :XMN_MWM_INPUT_MODE */
    sym = xlenter("MWM_INPUT_MODELESS");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_INPUT_MODELESS));
#ifdef WINTERP_MOTIF_11
    sym = xlenter("MWM_INPUT_PRIMARY_APPLICATION_MODAL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_INPUT_PRIMARY_APPLICATION_MODAL));
#else /* MOTIF 1.0 */
    sym = xlenter("MWM_INPUT_APPLICATION_MODAL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_INPUT_APPLICATION_MODAL));
#endif /* WINTERP_MOTIF_11 */
    sym = xlenter("MWM_INPUT_SYSTEM_MODAL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_INPUT_SYSTEM_MODAL));
#ifdef WINTERP_MOTIF_11
    sym = xlenter("MWM_INPUT_FULL_APPLICATION_MODAL");
    setvalue(sym, cvfixnum((FIXTYPE) MWM_INPUT_FULL_APPLICATION_MODAL));
#endif /* WINTERP_MOTIF_11 */
  }
}
