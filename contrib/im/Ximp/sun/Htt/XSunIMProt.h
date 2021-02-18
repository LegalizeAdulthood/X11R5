#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)XSunIMProt.h 1.4 91/08/10";
#endif
#endif

/******************************************************************

              Copyright 1990, 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sun Microsystems, Inc.
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
Sun Microsystems, Inc. makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

Sun Microsystems Inc. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL Sun Microsystems, Inc. BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Hideki Hiura (hhiura@Sun.COM)
	  				     Sun Microsystems, Inc.
******************************************************************/

/*
 * Lookup choices protocol
 *
 * 0) IMM->IML IML_SEND_EVENT (to start lookup choices region)
 * 1) IMM<-IML IMM_SET_REGION
 * 2) IMM<-IML IMM_LOOKUP_DRAW
 * 3) IMM->IML IML_SEND_EVENT (whole key event)
 * 4) IMM<-IML IMM_LOOKUP_PROCESS (with keysym)
 * 5) IMM->IML IML_SEND_RESULTS (wchar_t of usr's choice)
 *    sequence 3-4-5 might be repeated until commit key is typed
 * 6) IMM<-IML IMM_LOOKUP_DONE
 * 7) IMM<-IML IMM_COMMIT
 */

#ifndef xim_imlogic_DEFINED
#define xim_imlogic_DEFINED

#ifndef Private
#define Private static
#define Public /**/
#endif  Private

/*
 * IMM to IML opcode
 */
typedef unsigned int  IML_Opcode;

#define IML_OPEN  0              /* Create a conversion context IC */
                                 /* Operand: NIL */
                                 /* return.opcode: NIL. */
                                 /* Status>0?Session ID:ERROR reason */
#define IML_CLOSE  1             /* Close the IC */
                                 /* Operand: NIL*/
                                 /* return.opcode: NIL. */
#define IML_RESET  2             /* Reset the IC */
                                 /* Operand: NIL*/
                                 /* return.opcode: NIL. */
#define IML_GET_STATUS  3        /* Get IML status */
                                 /* Operand: ActionKey*/
                                 /* return.opcode: KEY_INFO */
                                 /* return.operand: Keysym */
#define IML_GET_KEY_MAP  4       /* Get keymap */
                                 /* Operand: keymaps_t or NULL */
                                 /* if NULL, IML returns whole function keys
				  * info which IML knows. if keymap_t, IML
				  * returns only
				  * specified functions.
				  */
                                 /* return.opcode: KEY_INFO  */
                                 /* return.operand: keymaps_t */
#define IML_SET_STATUS  5        /* Set IML status */
                                 /* Operand:  */
                                 /* return.opcode: KEY_INFO */
#define IML_SET_KEY_MAP  6       /* Set keymap */
                                 /* Operand: keymaps_t */
                                 /* return.opcode: KEY_INFO */
                                 /* return.operand: keymaps_t */
#define IML_SEND_EVENT  7        /* Send a key event */
                                 /* Operand: key_struct*/
                                 /* List of returns and each is:  */
                                 /* return.opcode:   */
                                 /* return.operand:  */

#define IML_SET_KEYBOARD_STATE  8/* Set global keyboard state  */
                                 /* Operand: IMKeyState */
                                 /* return.opcode: NIL. */
/*#define IML_SET_CHOICE  9  	 /* Send the usr's choice to IMLogic */
                                 /* Operand: wchar_t */
                                 /* return.opcode: IMM_LOOKUP_DONE */
                                 /* return.operand: NIL */
                                 /* return.opcode: IMM_COMMIT */
                                 /* return.operand: wchar_t (usr's choice)*/
/*#define IML_CONVERSION_ON  10  /* Tell IMLogic I got conversion on key */
                                 /* Operand:  */
                                 /* return.opcode: IMM_SET_REGION */
                                 /* return.operand: STATUS */
                                 /* return.opcode: IMM_STATUS_OPEN */
                                 /* return.operand: NIL */
                                 /* return.opcode: IMM_STATUS_DRAW */
                                 /* return.operand: XIMStatusCBData */
	
#define IML_SEND_RESULTS  11	 /* Send a CB results */
				 /* Operand can be anything passed from IML */
				 /* return.opcode: can be anything */
#define IML_COMMIT      12       /* Request IM to commit intermediate text */
                                 /* Operand: */
                                 /* return.opcode: IMM_COMMIT */
                                 /* return.operand: multibyte string */
/*
 * IML_SET_KEYBOARD_STATE operand
 */
typedef enum {
    MODE_SWITCH_ON = 0,
    MODE_SWITCH_OFF = 1,
} IMKeyState ;



/*
 * This is for public_status in iml_session_t/status in IC.
 * Used by IML_SET_STATUS/IML_GET_STATUS
 */
typedef int iml_status_t ;

#define IMLSTATUS_Henkan_Mode                 0x00000001 
#define IMLSTATUS_Mode_switch                 0x00000002
#define IMLSTATUS_Backfront                   0x00000004  /* if 1, backend */
#define IMLSTATUS_Backfront_Recommendation    0x00000008  /* if 1, backend */


/*
 * IML_SEND_EVENT operand
 */
				/* it increased */
typedef struct {
    Status status ;		/* XBufferOverFlow XLookupNone XLookupChars
				 *  XLookupKeySym XLookupBoth */
    KeySym keysym ;		/* keysym return */
    char   string[4] ; 		/* 4 is dummy size. variable length */
} committed_string_t ;

/*
 * IML_GET_KEYMAP/IML_SET_KEYMAP operand
 * これを使用する主な目的は、変換 ON/OFF キーの設定である。
 */

typedef enum {
    KEYMAP_IN_WCS = 1,
    KEYMAP_IN_MBS = 2,
    KEYMAP_IN_KEYSYM = 3,
} keymap_type_t ;

typedef struct {
    KeySym  keysym ;
    unsigned int state ;
    unsigned int mode ;
} fkey ;

typedef union {
    wchar_t *wcs ;
    char    *mbs ;
    KeySym  *keysyms ;
} function_key_sequence_t ;

typedef enum {
    CONVERSION_ON_KEY = 1,
    CONVERSION_OFF_KEY = 2,
} function_key_t ;

typedef struct {
    keymap_type_t 	    keymap_type ;
    function_key_t 	    function_key ;
    function_key_sequence_t function_key_sequence ;
    int                     sequence_length ;
} keymap_t ;

typedef struct {
    int		n ;        /* number of keymaps array */
    keymap_t    keymaps[1]; /* variable length */
} keymaps_t ;

/* ------------------------------------------------------------ */
    
/*
 * IML to IMM opcode 
 */
typedef unsigned int  IMM_Opcode ;

#define IMM_NOP                  0 /* Nop */
                                   /* operand : */
#define IMM_RESET                1 /* Reset the IC */
                                   /* operand : */
#define IMM_COMMIT               2 /* Returning a committed string */
                                   /* operand : wchar_t *comitted_string */
#define IMM_SET_STATUS           3 /* iml status i.e.ConvOn/off kana etc..*/
                                   /* operand : NIL */
#define IMM_PREEDIT_START        4 /* Call the preedit_start callback */
                                   /* operand: XIMPreEditCBData */
#define IMM_PREEDIT_DRAW         5 /* Call the preedit_draw callback */
                                   /* operand : im_textinfo */
#define IMM_PREEDIT_CARET        6 /* Call the preedit_caret callback */
#define IMM_PREEDIT_DONE         7 /* Call the preedit_done call back */
                                   /* operand: NIL */
#define IMM_LOOKUP_START         8 /* Call the preedit_start callback */
                                   /* operand: XIMLookupCBData */
#define IMM_LOOKUP_DRAW          9 /* Call the lookup_draw callback */
                                   /* list of 
                                   /* operand : wchar_t *choice*/
#define IMM_LOOKUP_PROCESS      10 /* Call the lookup_process callback */
                                   /* operand : keysym */
#define IMM_LOOKUP_DONE         11 /* Call the lookup_done callback */
                                   /* operand : NIL */
#define IMM_KEY_INFO            12 /* Return the key that corresponds to */
				   /* specified function*/
                                   /* operand : keymaps_t */
#define IMM_PUT_QUEUE           13 /* Put the string back to X queue */
                                   /* operand : */
#define IMM_STATUS_START        14 /* Call the preedit_start callback */
                                   /* operand: XIMPreEditCBData */
#define IMM_STATUS_DRAW         15 /* Call the status_draw callback */
                                   /* operand :  XIMStatusCBData */
#define IMM_STATUS_DONE         16 /* Call the status_draw callback */
                                   /* operand : NIL */
#define IMM_AUX_START           17 /* Call the Aux_start callback */
                                   /* operand: XIMAuxCBData */
#define IMM_AUX_DRAW            18 /* Call the aux_draw callback */
                                   /* list of 
                                   /* operand : wchar_t *choice*/
#define IMM_AUX_PROCESS         19 /* Call the aux_process callback */
                                   /* operand : keysym */
#define IMM_AUX_DONE            20 /* Call the aux_done callback */
#define MAX_IMM_OPCODE          21 /* maximam opcode value */

#define IMM_CB_RESULT_REQUIRED  0x80000000 

/*
 * IMM_SET_REGION operand
 */

typedef enum {
    PREEDIT = 0,       
    LOOKUP = 1,         
    STATUS = 2,         
    AUX = 3,      	
}    IMM_Region ;

/*
 * iml_inst structure
 */
typedef struct _iml_inst {
    struct      _iml_inst        *next;
    int				size_of_operand ; 
    IMM_Opcode                  opcode;
    caddr_t                     operand;
} iml_inst;

/*
 * IMM_UPDATE_ALL_STATUS
 */


typedef enum {
    DIFFERENT_TRANSACTION = 0 ,
    SAME_TRANSACTION = 0x1025 
} IMTransaction ;

Public KeySym IMLogic();

#endif  xim_imlogic_DEFINED
