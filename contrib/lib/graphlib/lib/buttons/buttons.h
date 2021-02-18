/*
 *   GraphLib - A library and collection of programs to make
 *              X programming easier.
 *
 *   (c) Copyright 1990,1991 Jeffrey Hutzelman, Eric Anderson 
 *
 * Our thanks to the contributed maintainers at andrew for providing
 * disk space for the development of this library.
 * My thanks to Geoffrey Collyer and Henry Spencer for providing the basis
 * for this copyright.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company, the Regents of the University of California, or
 * the Free Software Foundation.
 *
 * Permission is granted to anyone to use this software for any purpose on
 * any computer system, and to alter it and redistribute it freely, subject
 * to the following restrictions:
 *
 * 1. The authors are not responsible for the consequences of use of this
 *    software, no matter how awful, even if they arise from flaws in it.
 *
 * 2. The origin of this software must not be misrepresented, either by
 *    explicit claim or by omission.  Since few users ever read sources,
 *    credits must appear in the documentation.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.  Since few users
 *    ever read sources, credits must appear in the documentation.
 *
 * 4. This notice may not be removed or altered.
 */


/*	GraphLib: An X Windows Programming Library
	Developed at CMU Fall 1990 - Spring 1991

	Eric Anderson (ea08@andrew.cmu.edu, eanders@sura.net)
	Jeffrey Hutzelman (jh4o@andrew.cmu.edu, jhutz@drycas.club.cc.cmu.edu)

	gl_buttons.h: Header information for Buttons
	Button structure definitions
	Button function prototypes
	Button data symbols
	Button Creation Macros

	Revision History
	Date     Who	What
	-------- ---	------------------------------
	03/08/91 jth	Created
	04/01/91 jth	Changed object names

*/

#ifndef _GL_BUTTONS_
#define _GL_BUTTONS_

#include <X11/gl/window.h>

/* Button Structures */
typedef struct {
  char		*name;		/* The button's name */
  int		width,height;	/* Might need these to draw */
  Window	winid;		/* We draw to this */
  char	    	state;		/* Activation state:
				   0: Mouse outside, switch up
				   1: Mouse inside, switch up
				   2: Mouse inside, switch down
				   3: Mouse outside, switch down */
  int		flags;		/* Flags:
				   GL_Bchecked: button is on
				   GL_Bautocheck: change checkmark automatically
				   GL_Bradio: radio group member
				   GL_BaUpdate: always update
				   GL_BaAction: always call action
				*/
  int		bnum;		/* Button number (0 for any) */
  int     	bflag;		/* 1<<b indicates button #b down if we care */
  VPTF		action;		/* Action function */
  VPTF		draw[4];	/* Draw functions [state] */
  unsigned long fgc,bgc,txc;	/* Foreground, Background, Text colors */
} GL_ButtonInfo;

typedef struct {
  int 		size;		/* Number of members */
  int		active;		/* Active member index */
  int 		oactive;	/* Previously active member index */
  int		num;		/* Number entered so far */
  GL_ButtonInfo	**members;	/* Array of member pointers */
  VPTF		action;		/* Action procedure for group */
} GL_RadioGroup;

/* Button-related functions */
void _GL_VA_Init_Button();
void _GL_VA_Handle_Button();
void _GL_VA_Execute_Button();
void _GL_VA_Destroy_Button();

void _GL_Button_Click();
void _GL_Button_Release();
void _GL_Button_Expose();
void _GL_Button_MouseEnter();
void _GL_Button_MouseLeave();

GC _GL_ButtonGC;

GL_RadioGroup *GL_CreateRadGrp();
void GL_DestroyRadGrp();
void GL_DestroyButton();

/* Button-related data symbols */
#define GLB 2000 /* Buttons gets varargs entries 2000 - 2099 */
#define GL_BtnSwitch GLB
#define GL_BtnAction GLB+1
#define GL_BtnDraw GLB+2 /* use GL_BtnDraw + <state {0,1,2,3}> */
#define GL_BtnFG GLB+6
#define GL_BtnBG GLB+7
#define GL_BtnTX GLB+8
#define GL_BtnRadGrp GLB+9
#define GL_BtnFlags GLB+10

#define GL_Bchecked	0x01
#define GL_Bautocheck	0x02
#define GL_Bradio	0x04
#define GL_BaUpdate	0x08
#define GL_BaAction	0x10
#define GL_Binuse	0x20
#define GL_Bdestroy	0x40

/* Associated data keys.  Recall that all keys below 32768 belong to GL */

void _GL_VA_InitB_Athena();
void _GL_VA_InitB_CheckBox();
void _GL_VA_ExecB_CheckBox();
void _GL_VA_InitB_Mac();
void _GL_VA_ExecB_Mac();
void _GL_VA_InitB_NeXTBtn();
void _GL_VA_InitB_OpenBtn();
void _GL_VA_ExecB_OpenBtn();
void _GL_VA_InitB_OpenSink();
void _GL_VA_ExecB_OpenSink();
void _GL_VA_InitB_RadioBtn();
void _GL_VA_ExecB_RadioBtn();

#define GL_Button \
  _GL_VA_Init_Button,\
  _GL_VA_Handle_Button,\
  _GL_VA_Execute_Button,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_AthenaBtn \
  _GL_VA_InitB_Athena,\
  _GL_VA_Handle_Button,\
  _GL_VA_Execute_Button,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_CheckBox \
  _GL_VA_InitB_CheckBox,\
  _GL_VA_Handle_Button,\
  _GL_VA_ExecB_CheckBox,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_MacButton \
  _GL_VA_InitB_Mac,\
  _GL_VA_Handle_Button,\
  _GL_VA_ExecB_Mac,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_NeXTButton \
  _GL_VA_InitB_NeXTBtn,\
  _GL_VA_Handle_Button,\
  _GL_VA_Execute_Button,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_OpenButton \
  _GL_VA_InitB_OpenBtn,\
  _GL_VA_Handle_Button,\
  _GL_VA_ExecB_OpenBtn,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_OpenSinkButton \
  _GL_VA_InitB_OpenSink,\
  _GL_VA_Handle_Button,\
  _GL_VA_ExecB_OpenSink,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#define GL_RadioButton \
  _GL_VA_InitB_RadioBtn,\
  _GL_VA_Handle_Button,\
  _GL_VA_ExecB_RadioBtn,\
  _GL_VA_Destroy_Window,\
  _GL_VA_Return_Window

#endif
