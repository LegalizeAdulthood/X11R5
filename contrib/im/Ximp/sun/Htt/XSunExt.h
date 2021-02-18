/*
 *  	@(#)XSunExt.h 1.5 91/08/10
 *
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#ifndef XSunExtentions_DEFINED
#define XSunExtentions_DEFINED
 

#ifdef SVR4
#define bcopy(a,b,c) memmove(b,a,c)
#define bzero(a,b) memset(a,0,b)
#define index(a,b) strchr(a,b)
#define rindex(a,b) strrchr(a,b)
#else
#include <strings.h>
#endif

#if XlibSpecificationRelease != 5
#ifdef VISIBLEPOS
typedef enum {
    XIMVisiblePosMid = 1,/* Put visible_pos to the middle of the display. */
    XIMVisiblePosEnd = 2,/* Display the string including & before visible_pos*/
    XIMVisiblePosStart = 3,/* Display the string starting at visible_pos */
 } XIMTextVisiblePosType;
 
typedef struct _XIMText {
    unsigned short length;
    XIMFeedback *feedback;
    Bool encoding_is_wchar; 
    union {
	char *multi_byte;
	wchar_t *wide_char;
    } string; 
    XIMTextVisiblePosType	visible_type;  
    int 			visible_pos;
} XIMText;

#endif
#endif /* XlibSpecificationRelease */

/*
 * Sun's Aux Callback extension decalarations for future.
 */
#ifdef	NEW_JLE

/*
 *	Flag to specify the type of a field in Aux region.
 */
typedef	enum {
	AUX_FIELD_UNUSED  = 0,
	AUX_FIELD_INTERM  = 1,
	AUX_FIELD_DISPLAY = 2,
	AUX_FIELD_SELECT  = 3,
}  AuxFieldType;

/*
 *	Next structure is not attached to an actual callback.
 *	But NEW_JLE requires Display-Only field in Aux , to keep
 *	consistency we call it "DisplayCallback " for now.
 */
typedef struct _XIMDisplayStartCallbackStruct {
	int	flags;
	int	width;
	int	height;
	int	spos_x;
	int	spos_y;
} XIMDisplayStartCallbackStruct;

/* 
 *	Aux start must be called with enough information that determines
 *	the layout of aux-window and its fine-structure.
 *	Aux is actually a combination of pre-edit, lookup and display callbacks.
 */	
typedef struct _AuxStartFieid {
	AuxFieldType	ftyp;
	union {
		XIMValue			preedit; /* may not be used */
		XIMDisplayStartCallbackStruct	display;
		XIMLookupStartCallbackStruct	lookupchoice;
 	} field;
} AuxStartField;

typedef struct _XIMAuxStartCallbackStruct {
        XKeyEvent          *event;
	int		focus;	/* default focus field */
	int		count;  /* number of fields    */
	AuxStartFieid	*field;
}  XIMAuxStartCallbackStruct;

/*
 *	AuxDraw callback is invoked once per field.
 */
typedef struct _AuxDrawField {
	int             fnum;
	union {
		XIMText				preedit;  /* only for label */
		XIMText				display;
		XIMLookupDrawCallbackStruct	lookupchoice;
	}
} AuxDrawField;

typedef struct _XIMAuxDrawCallbackStruct {
	AuxDrawField	field;
} XIMAuxDrawCallbackStruct;

typedef struct _AuxProcessField {
	int		fnum;  /*?? This must be set in CB side ??*/
	union {
		XIMPreEditDrawStruct		preedit;
		XIMText				display;
			/* This field(display) may not have sense,
			   but we should keep for the sake of consistency */
		XIMLookupProcessCallbackStruct	lookupchoice;
	}
} AuxProcessField;
			
typedef	struct _XIMAuxProcessCallbackStruct {
	XKeyEvent          *event;
/*  not defined yet
	AuxOpCode	opcode; 
*/
	AuxProcessField	field;
}	

#else	/* NEW_JLE */
 
typedef struct _XIMAuxStartCallbackStruct {
        XKeyEvent       *event;
        XIMText         *label;
        XIMText         *text;
}  XIMAuxStartCallbackStruct;

typedef XIMText  XIMAuxDrawCallbackStruct;

#endif	NEW_JLE
 

/*
 * LookupChoice Region
 */


#ifdef OLD_LOOKUP
typedef struct  {
    XIMText        *value;
}  ChoiceObject;

typedef struct {
    ChoiceObject    choices;
    int             max_len;
    int             n_choices;
    int             choice_per_window;
}  XIMChoices;
#endif /* OLD_LOOKUP */

typedef enum {
    XIMCB_Success,
    XIMCB_FatalError,
}  XIMCBResult;
 
 
typedef enum    {
    DrawUpHorizontally = 0 ,
    DrawUpVertically = 1 ,
} DrawUpDirection ;
 
typedef enum    {
    XIMOwnsLabel = 0 ,
    CBOwnsLabel = 1 ,
} WhoOwnsLabel ;

typedef struct {
    int     choice_per_window;  /* Number of choices can be display
				 * in the region
				 */
    int     nrows;
    int     ncolumns;
    DrawUpDirection DrawUpDirection ;
    WhoOwnsLabel WhoOwnsLabel;      /* For callback to tell XIM
				     * whether it wants to control
				     * what label should be for the choices.
				     */
} LayoutInfo ;
 
typedef  enum {
    HasNotBeenNegotiated = 0,
    XIMIsMaster = 1,
    CBIsMaster  = 2
} WhoIsMaster ;
 
typedef struct _XIMLookupStartCallbackStruct {
    XKeyEvent      *event ;
    WhoIsMaster     WhoIsMaster ; /* For callback to tell whether
				   * is going to be a master
				   */
    LayoutInfo      *XIMPreference ;
    LayoutInfo      *CBPreference ;
} XIMLookupStartCallbackStruct;

typedef struct _XIMChoiceObject{
    XIMText         *label;
    XIMText         *value;
} XIMChoiceObject;

typedef struct _XIMLookupDrawCallbackStruct{
    XIMChoiceObject *choices;       /* the lookup choices */
    int              n_choices;      /* Total number of lookup choices */
    int              max_len;        /* Max number of characters per
				      * choice item
				      */
    int              index_of_first_candidate ;
    int              index_of_last_candidate ;
} XIMLookupDrawCallbackStruct;

typedef struct _XIMLookupProcessCallbackStruct {
    XKeyEvent       *event;
    int              index_of_choice_selected;/* Set by callback for the
					       * selected choice.
					       * XIM_UNKNOW_KEYSYM and
					       * XIM_UNDETERMINED are
					       * also possible value.
					       */
} XIMLookupProcessCallbackStruct;
/*
 * Values for XIMLookupProcessCallbackStruct.index_of_choice_selected
 */
#define		XIM_UNKNOWN_KEYSYM	0x77777777
#define		XIM_UNDETERMINED	0x77777776

/*
 * Sun's extensions to IMValues
 */
#define XIMLookupCallbacks	0x4000000L
#define XIMAuxCallbacks		0x8000000L     

/*
 * Ximp extentions to XIC attributes
 */
/*
 * Ximp extentions to XIC attributes
 */
#define XNExtXimp_Backfront             "XNExtXimp_Backfront"
#define XNExtXimp_Statuswindow          "XNExtXimp_Statuswindow"
#define XNExtXimp_Conversion	        "XNExtXimp_Conversion"
#define XNExtXimp_Error		        "XNExtXimp_Error"
#define XNExtXimp_LookupStartCallback   "XNExtXimp_LookupStartCallback"
#define XNExtXimp_LookupDrawCallback    "XNExtXimp_LookupDrawCallback"
#define XNExtXimp_LookupDoneCallback    "XNExtXimp_LookupDoneCallback"
#define XNExtXimp_LookupProcessCallback "XNExtXimp_LookupProcessCallback"
#define XNExtXimp_LookupAttributes      "XNExtXimp_LookupAttributes"
#define XNExtXimp_AuxStartCallback      "XNExtXimp_AuxStartCallback"
#define XNExtXimp_AuxDrawCallback       "XNExtXimp_AuxDrawCallback"
#define XNExtXimp_AuxProcessCallback    "XNExtXimp_AuxProcessCallback"
#define XNExtXimp_AuxDoneCallback       "XNExtXimp_AuxDoneCallback"
#define XNExtXimp_AuxAttributes         "XNExtXimp_AuxAttributes"


/*
 * Ximp properties for extented XIC attribute
 */
#define XIMP_EXT_XIMP_CONVERSION           "_XIMP_EXT_XIMP_CONVERSION"
#define XIMP_EXT_XIMP_BACKFRONT            "_XIMP_EXT_XIMP_BACKFRONT"
#define XIMP_EXT_XIMP_STATUSWINDOW         "_XIMP_EXT_XIMP_STATUSWINDOW"
#define XIMP_EXT_XIMP_CONVERSION	   "_XIMP_EXT_XIMP_CONVERSION"
#define XIMP_EXT_XIMP_ERROR		   "_XIMP_EXT_XIMP_ERROR"
#define XIMP_EXT_XIMP_AUXSTARTCALLBACK     "_XIMP_EXT_XIMP_AUXSTARTCALLBACK"
#define XIMP_EXT_XIMP_AUXDRAWCALLBACK      "_XIMP_EXT_XIMP_AUXDRAWCALLBACK"
#define XIMP_EXT_XIMP_AUXPROCESSCALLBACK   "_XIMP_EXT_XIMP_AUXPROCESSCALLBACK"
#define XIMP_EXT_XIMP_AUXDONECALLBACK      "_XIMP_EXT_XIMP_AUXDONECALLBACK"
#define XIMP_EXT_XIMP_LOOKUPSTARTCALLBACK  "_XIMP_EXT_XIMP_LOOKUPSTARTCALLBACK"
#define XIMP_EXT_XIMP_LOOKUPDRAWCALLBACK   "_XIMP_EXT_XIMP_LOOKUPDRAWCALLBACK"
#define XIMP_EXT_XIMP_LOOKUPDONECALLBACK   "_XIMP_EXT_XIMP_LOOKUPDONECALLBACK"
#define XIMP_EXT_XIMP_LOOKUPPROCESSCALLBACK "_XIMP_EXT_XIMP_LOOKUPPROCESSCALLBACK"

/*
 * Possible values of XSetICValues(XNExtXimp_Conversion, XXX );
 */
#define	XIMEnable	1
#define	XIMDisable	0

/*
 * Possible values of XSetICValues(XNExtXimp_Backfront, XXX );
 */
#define	IMServBackend	1
#define	IMServFrontend  0


#endif XSunExtentions_DEFINED

/*
 * Temporary solution
 */
#define XIMValue XPointer
#define XNConversion XNExtXimp_Conversion
#define XSetICValue XSetICValues
#define XGetICValue XGetICValues
#define XIMPlain 0L
#define XNLookupStartCallback   "XNExtXimp_LookupStartCallback"
#define XNLookupDrawCallback    "XNExtXimp_LookupDrawCallback"
#define XNLookupDoneCallback    "XNExtXimp_LookupDoneCallback"
#define XNLookupProcessCallback "XNExtXimp_LookupProcessCallback"
#define XNAuxStartCallback      "XNExtXimp_AuxStartCallback"
#define XNAuxDrawCallback       "XNExtXimp_AuxDrawCallback"
#define XNAuxProcessCallback    "XNExtXimp_AuxProcessCallback"
#define XNAuxDoneCallback       "XNExtXimp_AuxDoneCallback"
#define BitmapType	XIMBitmapType
#ifndef xim_im_DEFINED
#define xim_im_DEFINED
#endif  ~xim_im_DEFINED
#ifndef immgr_cb_DEFINED
#define immgr_cb_DEFINED
#endif immgr_cb_DEFINED
#ifndef xim_ic_DEFINED
#define xim_ic_DEFINED
#endif  ~xim_ic_DEFINED


