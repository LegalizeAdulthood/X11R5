/* $Header: /usr3/emu/include/RCS/xt_ops.h,v 1.12 90/10/13 18:43:12 jkh Exp Locker: me $ */

/*
 * This file is part of the PCS emu system.
 *
 * Copyright 1990 by PCS Computer Systeme, GmbH. Munich, West Germany.
 *
 *                        All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the names of PCS or the authors not
 * be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * PCS, THE AUTHORS, OR THEIR HOUSEPETS BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE. SO DON'T SUE US. THANK YOU.
 *
 */

/*
 * termCanvas operation codes.
 *
 * Author: Jordan K. Hubbard
 * Date: March 20th, 1990.
 * Description: Here are all the operation codes supported by the parser.
 *		This file is also meant to be included by users configuring
 * 		emu in their application defaults files.
 *
 * Revision History:
 *
 * $Log:	xt_ops.h,v $
 * Revision 1.12  90/10/13  18:43:12  jkh
 * Change to OP_CHANGE_FLUT and OP_OVERRIDE_TRANSLATIONS
 * 
 * Revision 1.11  90/10/12  14:04:58  jkh
 * Name changes.
 * 
 * Revision 1.10  90/08/31  16:58:10  me
 * changed the definition of OP_SCROLL_SCREEN_ABSOLUTE and -RELATIVE
 * 
 * Revision 1.9  90/08/29  16:58:31  me
 * added OP_SCROLL_SCREEN_ABSOLUTE and _RELATIVE
 * 
 * Revision 1.8  90/08/16  10:31:46  me
 * removed OP_INSERT_BLANKS
 * 
 * Revision 1.7  90/07/26  02:42:13  jkh
 * Added new copyright.
 * 
 * Revision 1.2  90/05/08  17:30:55  me
 * added the tab stuff
 * 
 * Revision 1.1  90/05/08  14:36:10  me
 * Initial revision
 * 
 *
 */

/*
 * Definitions for character attributes
 */

#define ATT_NONE	0x0
#define ATT_BOLD	1	/* 1 >> 0 */
#define ATT_UNDERL	2	/* 1 >> 1 */
#define ATT_BLINK	4	/* 1 >> 2 */
#define ATT_REVERSE	8	/* 1 >> 3 */
#define ATT_ITALIC	16	/* 1 >> 4 */

#define ATT_SELECTED	64	/* 1 >> 6 */
#define ATT_CONTINUE	128	/* 1 >> 7 */

#define ATT_FONTS	(ATT_BOLD | ATT_ITALIC)

#define ATT_BOL		256	/* 1 >> 8 */
#define ATT_EOL		512	/* 1 >> 9 */
#define ATT_BOS		1024	/* 1 >> 10*/
#define ATT_EOS		2048	/* 1 >> 11*/

#define MOTION_STOP	-1
#define MOTION_CONT	-2

/* Types for register contents. */
#define CB_INT_TYPE		1
#define CB_STR_TYPE		2
#define CB_CHAR_TYPE		3
#define CB_ANY_TYPE		4

/* Menu action codes */
#define MENU_ITEM_ACTIVATE	1
#define MENU_ITEM_DEACTIVATE	2
#define MENU_ITEM_MARK		3
#define MENU_ITEM_UNMARK	4

/*
 * Operation codes
 */
#define OP_START		0

/* basic character insertion */
#define OP_INSERT		0
/* movement */
#define OP_MOVE_ABS			1	/* Col/Line in X/Y */
#define OP_MOVE_REL			2	/* Col/Line in X/Y */
#define OP_MOVE_ABS_COLUMN		3	/* Col in X */
#define OP_MOVE_ABS_ROW			4	/* Line in Y */
#define OP_MOVE_REL_COLUMN		5	/* Delta Col in X */
#define OP_MOVE_REL_ROW			6	/* Delta Line in Y */
#define OP_MOVE_REL_ROW_SCROLLED 	7	/* e.g. for LF, delta in Y */
#define OP_IS_MOVE(op)			((op) >= 1 && (op) <= 6)
/* write modes */
#define OP_INSERT_MODE			8	/* no args */
#define OP_OVERWRITE_MODE		9	/* no args */
/* deleting */
#define OP_DELETE_CHARS			10	/* A is count */
#define OP_DELETE_TO_EOL		11	/* no args */
#define OP_DELETE_LINES			12	/* A is count */
#define OP_DELETE_TO_EOSCR		13	/* no args */
/* erasing */
#define OP_ERASE_CHARS			14	/* A is count */
#define OP_ERASE_LINE_LEFT		15	/* no args */
#define OP_ERASE_LINES			16	/* A is count */
#define OP_ERASE_FROM_TOSCR		17	/* no args */
#define OP_CLEAR_SCREEN			18	/* no args */
/* inserting */
				     /* 19 */
#define OP_INSERT_LINES			20	/* A is count */
/* define the scrolling region */
#define OP_SET_SCROLL_REGION		21	/* Start Line in A, End in B */
#define OP_RING_BELL			22	/* toot the horn */
/* Tabs */
#define OP_HOR_TAB			23	/* no args */
#define OP_SET_TAB_CUR_COL		24	/* no args */
#define OP_SET_TAB_COL			25	/* column in X */
#define OP_SET_TABS_EQ_WIDTH		26	/* tab width in A */
#define OP_CLEAR_TAB_CUR_COL		27	/* no args */
#define OP_CLEAR_TAB_COL		28	/* column in X */
#define OP_CLEAR_ALL_TABS		29	/* nor args */
/* Text Attributes */
#define OP_SET_ATTRIBUTE		30	/* ATT in A */
#define OP_CLEAR_ATTRIBUTE		31	/* ATT in B */
/*
 * Changing the Keyboard Translations e.g. for different Codes on the
 * Cursor Keys and Number Keys depending on the Keyboard mode.
 */
#define OP_OVERRIDE_TRANSLATIONS	32	/* name in A */
/*
 * Change the FLUT e.g. for different character sets.
 */
#define OP_CHANGE_FLUT			33	/* name in A, offset in B */

/*
 * Reports from the canvas to the parser for further use in
 * the parser.
 * They all have no arguments, the canvas however fills the comblock
 * with data.
 */
#define OP_CANVAS_SIZE    		34	/* rep. Cols in X Lines in Y */
#define OP_CANVAS_CURSOR_POS		35	/* rep. Col in X Line in Y */
#define OP_CANVAS_ATTRIBS		36	/* rep. Attributes in A */
#define OP_CANVAS_SCROLL_REGION		37	/* rep. Start in A, End in B */
#define OP_CANVAS_WRAP_MODE		38	/* rep. wrap mode in A */
#define OP_CANVAS_REVERSE_MODE		39	/* rep. rev. mode in A */
#define OP_CANVAS_CURSOR_ON		40	/* rep. cursor state in A */
#define OP_CANVAS_CURSOR_BLINKING	41	/* rep. if cursor blinks in A*/
#define OP_CANVAS_CURSOR_SIZE		42	/* rep. curs. size in X and Y*/

/*
 * clear redraw the screen
 */
#define OP_REDRAW_SCREEN		43	/* no args */

/*
 * change the current fonts
 */
#define OP_CHANGE_FONTS			44	/* normal in A, bold in B */

/*
 * scrolling the screen and the saved area
 */
#define OP_SCROLL_SCREEN_ABSOLUTE	45	/* percentage in a */
#define OP_SCROLL_SCREEN_RELATIVE	46	/* delta percentage in a */

/*
 * general modes
 */
#define OP_CURSOR_OFF			47	/* no args */
#define OP_CURSOR_ON			48	/* no args */
#define OP_SET_SCREEN_SIZE		49	/* Cols in X, Lines in Y */
#define OP_WRAP_AROUND			50	/* no args */
#define OP_DONT_WRAP			51	/* no args */
#define OP_CURSOR_POS_REL_TO_SCR_REG	52	/* no args */
#define OP_CURSOR_POS_ABSOLUTE		53	/* no args */
#define OP_REVERSE_VIDEO		54	/* no args */
#define OP_NORMAL_VIDEO			55	/* no args */
#define OP_SAVE_FLUT			56	/* no args */
#define OP_RESTORE_FLUT			57	/* no args */
#define OP_SET_CURSOR_BLINK		58	/* Flag in A */
#define OP_SET_CURSOR_SIZE		59	/* Width in X, Height in Y */

/*
 * Color stuff
 */
#define OP_CHANGE_FG_COLOR 		60
#define OP_CHANGE_BG_COLOR		61
#define OP_SET_CIT_CELL			62
#define OP_CANVAS_DISPLAY_CELLS		63

/* last OP */
#define OP_END				63

/* special OP NO-OP, MUST be bigger than OP_END */
#define OP_NOP				9999

/* number of OPs */
#define OP_NUM (OP_END - OP_START + 1)


/*
 * Reverse Opcodes for the reverse parser
 */

#define ROP_START			0
/* basic character insertion */
#define ROP_INSERT			0
/* not so basic character insertion */
#define ROP_INSERT_CHAR			1	/* decimal char in A */
/* give back Cursor Position */
#define ROP_SCREEN_SIZE			2	/* Cols in X, Lines in Y */
#define ROP_CURSOR_POS			3	/* Col in X, Line in Y */
#define ROP_CUR_ATTRIBS			4	/* Attributes in A */
#define ROP_SCROLL_REGION		5	/* Start Line in A, End in B */
		
/* initialization Codes start at 100 */
/*
 * Initialization for the canvas , does everything necessary to set up the
 * canvas for the current emulation.
 */
#define ROP_INIT_CANVAS			100	/* no args */
#define ROP_INIT_TTY			101	/* no args */
#define ROP_INIT_MENUS			102	/* no args */
