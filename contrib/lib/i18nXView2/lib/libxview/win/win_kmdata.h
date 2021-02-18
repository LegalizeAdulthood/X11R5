/*	@(#)win_kmdata.h 20.25 90/05/23 SMI	*/

/* 
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/* win_keymap_data.h -- pre-parsed keymap data */

#ifndef win_keymap_data_DEFINED
#define win_keymap_data_DEFINED

#include <X11/keysym.h>
#include <xview/win_input.h>
#include <xview_private/win_ascii.h>

unsigned int		win_keymap[] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
/*
 * TTY Functions, cleverly chosen to map to ascii, for convenience of
 * programming, but could have been arbitrary (at the cost of lookup
 * tables in client code.
 */

	XK_BackSpace,
	XK_Tab,
	XK_Linefeed,
	XK_Clear,
	NULL,
	XK_Return,

	NULL, NULL,

	/* BUG: On X11/NeWS, Keysym F36 and F37 happen to fall into a couple
		of holes in the win_keymap table.  We will use them for
		now, but this needs to be fixed before MIT decides to put
		real keysyms here.
	*/

	KEY_TOP(11),					/* XK_SunF36 */
	KEY_TOP(12),					/* XK_SunF37 */

	NULL,

	XK_Pause,
#ifndef XK_Scroll_Lock
        NULL,
#else    
        /* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock
	NULL, NULL, NULL, NULL, NULL, NULL,

	XK_Escape,

	NULL, NULL, NULL, NULL,

	/* International & multi-key character composition */

	XK_Multi_key,
	XK_Kanji,

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* Cursor control & motion */

	XK_Home,
	KEY_RIGHT(10),					/* XK_Left  */
	KEY_RIGHT(8),					/* XK_Up    */
	KEY_RIGHT(12),					/* XK_Right */
	KEY_RIGHT(14),					/* XK_Down  */
	XK_Prior,
	XK_Next,
	XK_End,
	XK_Begin,

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	/* Misc Functions */
 
	XK_Select,
	XK_Print,
	XK_Execute,
	XK_Insert,
	NULL,
	XK_Undo,
	XK_Redo,
	XK_Menu,
	XK_Find,
	XK_Cancel,
	XK_Help,
	SHIFT_BREAK,					/* XK_Break */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	SHIFT_ALTG,					/* XK_script_switch */
	SHIFT_NUMLOCK,					/* XK_Num_Lock      */

 	/* Keypad Functions, keypad numbers cleverly chosen to map to ascii */

	XK_KP_Space,

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	XK_KP_Tab,

	NULL, NULL, NULL,

	XK_KP_Enter,

	NULL, NULL, NULL,

	XK_KP_F1,
	XK_KP_F2,
	XK_KP_F3,
	XK_KP_F4,

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	XK_KP_Multiply,
	XK_KP_Add,
	XK_KP_Separator,
	XK_KP_Subtract,
	XK_KP_Decimal,
	XK_KP_Divide,
	XK_KP_0,
	XK_KP_1,
	XK_KP_2,
	XK_KP_3,
	XK_KP_4,
	XK_KP_5,
	XK_KP_6,
	XK_KP_7,
	XK_KP_8,
	XK_KP_9,

	NULL, NULL, NULL,

	XK_KP_Equal,

  /*
   * Auxilliary Functions; note the duplicate definitions for left and right
   * function keys;  Sun keyboards and a few other manufactures have such
   * function key groups on the left and/or right sides of the keyboard.
   * We've not found a keyboard with more than 35 function keys total.
   */
	KEY_TOP(1),					/* XK_F1  */
	KEY_TOP(2),		        		/* XK_F2  */
	KEY_TOP(3),					/* XK_F3  */
	KEY_TOP(4),					/* XK_F4  */
	KEY_TOP(5),					/* XK_F5  */
	KEY_TOP(6),					/* XK_F6  */
	KEY_TOP(7),					/* XK_F7  */
	KEY_TOP(8),					/* XK_F8  */
	KEY_TOP(9),					/* XK_F9  */
	KEY_TOP(10),					/* XK_F10 */
	KEY_LEFT(1),					/* XK_L1  */
	KEY_LEFT(2),					/* XK_L2  */
	KEY_LEFT(3),					/* XK_L3  */
	KEY_LEFT(4),					/* XK_L4  */
	KEY_LEFT(5),					/* XK_L5  */
	KEY_LEFT(6),					/* XK_L6  */
	KEY_LEFT(7),					/* XK_L7  */
	KEY_LEFT(8),					/* XK_L8  */
	KEY_LEFT(9),					/* XK_L9  */
	KEY_LEFT(10),					/* XK_L10 */
	KEY_RIGHT(1),					/* XK_R1  */
	KEY_RIGHT(2),					/* XK_R2  */
	KEY_RIGHT(3),					/* XK_R3  */
	KEY_RIGHT(4),					/* XK_R4  */
	KEY_RIGHT(5),					/* XK_R5  */
	KEY_RIGHT(6),					/* XK_R6  */
	KEY_RIGHT(7),					/* XK_R7  */
	KEY_RIGHT(8),					/* XK_R8  */
	KEY_RIGHT(9),					/* XK_R9  */
	KEY_RIGHT(10),					/* XK_R10 */
	KEY_RIGHT(11),					/* XK_R11 */
	KEY_RIGHT(12),					/* XK_R12 */
	KEY_RIGHT(13),					/* XK_R13 */
	KEY_RIGHT(14),					/* XK_R14 */
	KEY_RIGHT(15),					/* XK_R15 */

	/* Modifiers */
 
	SHIFT_LEFT,					/* XK_Shift_L    */
	SHIFT_RIGHT,					/* XK_Shift_R    */
	SHIFT_CTRL,					/* XK_Control_L  */
	SHIFT_CTRL,					/* XK_Control_R  */
	SHIFT_CAPSLOCK,					/* XK_Caps_Lock  */
	SHIFT_LOCK,					/* XK_Shift_Lock */
	SHIFT_META,					/* XK_Meta_L     */
	SHIFT_META,					/* XK_Meta_R     */
	SHIFT_ALT,					/* XK_Alt_L	 */
	SHIFT_ALTG,					/* XK_Alt_R	 */
	XK_Super_L,
	XK_Super_R,
	XK_Hyper_L,
	XK_Hyper_R,

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	XK_Delete
};

unsigned char		win_keymap_sem[] = {

/*
 *		No Modifiers   (Range 0-255)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	ACTION_ERASE_CHAR_BACKWARD & 0xFF,		/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	NULL,						/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock
	NULL,
#else
	/* BUG: Only in R4. */
	XK_Scroll_Lock,					/* XK_Scroll_Lock */
#endif XK_Scroll_Lock
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	ACTION_GO_DOCUMENT_START & 0xFF,		/* XK_Home    */
	ACTION_GO_CHAR_BACKWARD & 0xFF,			/* XK_Left    */
	ACTION_GO_COLUMN_BACKWARD & 0xFF,		/* XK_Up      */
	ACTION_GO_CHAR_FORWARD & 0xFF,			/* XK_Right   */
	ACTION_GO_COLUMN_FORWARD & 0xFF,		/* XK_Down    */
	ACTION_GO_PAGE_BACKWARD & 0xFF,			/* XK_Prior   */
	ACTION_GO_PAGE_FORWARD & 0xFF,			/* XK_Next    */
	ACTION_GO_DOCUMENT_END & 0xFF,			/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	ACTION_INSERT & 0xFF,				/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	ACTION_HELP & 0xFF,				/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	ACTION_STOP & 0xFF,				/* XK_L1  */
	ACTION_AGAIN & 0xFF,				/* XK_L2  */
	ACTION_PROPS & 0xFF,				/* XK_L3  */
	ACTION_UNDO & 0xFF,				/* XK_L4  */
	ACTION_FRONT & 0xFF,				/* XK_L5  */
	ACTION_COPY & 0xFF,				/* XK_L6  */
	ACTION_OPEN & 0xFF,				/* XK_L7  */
	ACTION_PASTE & 0xFF,				/* XK_L8  */
	ACTION_FIND_FORWARD & 0xFF,			/* XK_L9  */
	ACTION_CUT & 0xFF,				/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	ACTION_GO_DOCUMENT_START & 0xFF,		/* XK_R7  */
	ACTION_GO_COLUMN_BACKWARD & 0xFF,		/* XK_R8  */
	ACTION_GO_PAGE_BACKWARD & 0xFF,			/* XK_R9  */
	ACTION_GO_CHAR_BACKWARD & 0xFF,			/* XK_R10  */
	ACTION_GO_LINE_FORWARD & 0xFF, 			/* XK_R11 */
	ACTION_GO_CHAR_FORWARD & 0xFF,			/* XK_R12 */
	ACTION_GO_DOCUMENT_END & 0xFF,			/* XK_R13 */
	ACTION_GO_COLUMN_FORWARD & 0xFF,		/* XK_R14 */
	ACTION_GO_PAGE_FORWARD & 0xFF,			/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	ACTION_ERASE_CHAR_BACKWARD & 0xFF,		/* XK_Delete     */
/*
 *		Control Modifier  (Range: 256-511)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	ACTION_SELECT_FIELD_FORWARD & 0xFF,		/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	ACTION_GO_DOCUMENT_END & 0xFF,			/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
	/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	ACTION_HELP & 0xFF,				/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */
/*
 *		Meta Modifier  (Range: 512-767)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	ACTION_DO_IT & 0xFF,				/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
	/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	ACTION_HELP & 0xFF,				/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */
/*
 *		Control/Meta Modifier (Range: 768-1023)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	NULL,						/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
	/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	NULL,						/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */

/*
 *		Shift Modifier  (Range: 1024-1279)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	ACTION_ERASE_CHAR_FORWARD & 0xFF,		/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	NULL,						/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
	NULL,
#else    
	/* BUG: Only in R4. */
	XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	ACTION_GO_DOCUMENT_START & 0xFF,                /* XK_Home    */
	ACTION_GO_CHAR_BACKWARD & 0xFF,                 /* XK_Left    */
	ACTION_GO_COLUMN_BACKWARD & 0xFF,               /* XK_Up      */
	ACTION_GO_CHAR_FORWARD & 0xFF,                  /* XK_Right   */
	ACTION_GO_COLUMN_FORWARD & 0xFF,                /* XK_Down    */
	ACTION_GO_PAGE_BACKWARD & 0xFF,                 /* XK_Prior   */
	ACTION_GO_PAGE_FORWARD & 0xFF,                  /* XK_Next    */
	ACTION_GO_DOCUMENT_END & 0xFF,                  /* XK_End     */
	NULL,                                           /* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	ACTION_HELP & 0xFF,				/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	ACTION_BACK & 0xFF,				/* XK_L5  */
	NULL,						/* XK_L6  */
	ACTION_CLOSE & 0xFF,				/* XK_L7  */
	NULL,						/* XK_L8  */
	ACTION_FIND_BACKWARD & 0xFF,			/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	ACTION_ERASE_CHAR_FORWARD & 0xFF,		/* XK_Delete     */

/*
 *		Shift/Control Modifier  (Range: 1280-1535)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	ACTION_SELECT_FIELD_BACKWARD & 0xFF,		/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	ACTION_GO_DOCUMENT_START & 0xFF,		/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
	/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	NULL,						/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */
/*
 *		Shift/Meta Modifier  (Range: 1536-1791)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	NULL,						/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
	/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	NULL,						/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */
/*
 *		Shift/Control/Meta Modifier  (Range: 1792-2047)
 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,						/* XK_BackSpace */
	NULL,						/* XK_Tab       */
	NULL,						/* XK_Linefeed  */
	NULL,						/* XK_Clear     */
	NULL,
	NULL,						/* XK_Return    */

	NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Pause     */
#ifndef XK_Scroll_Lock 
        NULL,
#else    
/* BUG: Only in R4. */
        XK_Scroll_Lock,                 		/* XK_Scroll_Lock */
#endif XK_Scroll_Lock 
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Escape    */

	NULL, NULL, NULL, NULL,

	NULL,						/* XK_Multi_key */
	NULL,						/* XK_Kanji     */

	NULL, NULL, NULL,                         NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Home    */
	NULL,						/* XK_Left    */
	NULL,						/* XK_Up      */
	NULL,						/* XK_Right   */
	NULL,						/* XK_Down    */
	NULL,						/* XK_Prior   */
	NULL,						/* XK_Next    */
	NULL,						/* XK_End     */
	NULL,						/* XK_Begin   */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Select  */
	NULL,						/* XK_Print   */
	NULL,						/* XK_Execute */
	NULL,						/* XK_Insert  */
	NULL,
	NULL,						/* XK_Undo    */
	NULL,						/* XK_Redo    */
	NULL,						/* XK_Menu    */
	NULL,						/* XK_Find    */
	NULL,						/* XK_Cancel  */
	NULL,						/* XK_Help    */
	NULL,						/* XK_Break   */
	
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Mode_switch */
	NULL,						/* XK_Num_Lock    */

	NULL,						/* XK_KP_Space    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_KP_Tab      */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Enter    */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_F1 */
	NULL,						/* XK_KP_F2 */
	NULL,						/* XK_KP_F3 */
	NULL,						/* XK_KP_F4 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL,

	NULL,						/* XK_KP_Multiply  */
	NULL,						/* XK_KP_Add       */
	NULL,						/* XK_KP_Separator */
	NULL,						/* XK_KP_Subtract  */
	NULL,						/* XK_KP_Decimal   */
	NULL,						/* XK_KP_Divide    */
	NULL,						/* XK_KP_0 */
	NULL,						/* XK_KP_1 */
	NULL,						/* XK_KP_2 */
	NULL,						/* XK_KP_3 */
	NULL,						/* XK_KP_4 */
	NULL,						/* XK_KP_5 */
	NULL,						/* XK_KP_6 */
	NULL,						/* XK_KP_7 */
	NULL,						/* XK_KP_8 */
	NULL,						/* XK_KP_9 */

	NULL, NULL, NULL,

	NULL,						/* XK_KP_Equal */

	NULL,						/* XK_F1  */
	NULL,			        		/* XK_F2  */
	NULL,						/* XK_F3  */
	NULL,						/* XK_F4  */
	NULL,						/* XK_F5  */
	NULL,						/* XK_F6  */
	NULL,						/* XK_F7  */
	NULL,						/* XK_F8  */
	NULL,						/* XK_F9  */
	NULL,						/* XK_F10 */
	NULL,						/* XK_L1  */
	NULL,						/* XK_L2  */
	NULL,						/* XK_L3  */
	NULL,						/* XK_L4  */
	NULL,						/* XK_L5  */
	NULL,						/* XK_L6  */
	NULL,						/* XK_L7  */
	NULL,						/* XK_L8  */
	NULL,						/* XK_L9  */
	NULL,						/* XK_L10 */
	NULL,						/* XK_R1  */
	NULL,						/* XK_R2  */
	NULL,						/* XK_R3  */
	NULL,						/* XK_R4  */
	NULL,						/* XK_R5  */
	NULL,						/* XK_R6  */
	NULL,						/* XK_R7  */
	NULL,						/* XK_R8  */
	NULL,						/* XK_R9  */
	NULL,						/* XK_R10 */
	NULL,						/* XK_R11 */
	NULL,						/* XK_R12 */
	NULL,						/* XK_R13 */
	NULL,						/* XK_R14 */
	NULL,						/* XK_R15 */

	NULL,						/* XK_Shift_L    */
	NULL,						/* XK_Shift_R    */
	NULL,						/* XK_Control_L  */
	NULL,						/* XK_Control_R  */
	NULL,						/* XK_Caps_Lock  */
	NULL,						/* XK_Shift_Lock */
	NULL,						/* XK_Meta_L     */
	NULL,						/* XK_Meta_R     */
	NULL,						/* XK_Alt_L      */
	NULL,						/* XK_Alt_R      */
	NULL,						/* XK_Super_L    */
	NULL,						/* XK_Super_R    */
	NULL,						/* XK_Hyper_L    */
	NULL,						/* XK_Hyper_R    */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL,

	NULL,						/* XK_Delete     */
};


#endif win_keymap_data_DEFINED
