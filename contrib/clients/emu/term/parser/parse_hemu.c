#ifndef lint
static char *rcsid = "$Header: /usr3/emu/term/parser/RCS/parse_dumb.c,v 1.1 90/11/20 17:46:22 jkh Exp $";
#endif

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
 * This file implements some simple minded hard parsers.
 *
 * Author: Michael W. Elbel
 * Date: 8. August 1991
 * Description: Hardcoded parser that implements the emu Emulation
 *
 * Revision History:
 *
 * $Log: $
 */

#include "TermP.h"

/* Sets buf to the next character or exits if end of buffer */
#define next buf++; while (count && !*buf) { buf++; count--; }  \
             if (!count) goto getout ; else count--

/* gets an int from buf into tmp, including test for the ';' */
#define getint \
	tmp = 0; \
	next; \
	if (*buf == '-') { /* negative */ \
	     sign = -1; \
	     next; \
	} \
	else { \
	      sign = 1; \
	} \
	while (isdigit(*buf)) { \
	     tmp = tmp*10 + (int)(*buf - '0'); \
	     next; \
	} \
	if (*buf != ';') { \
	     break; \
	} \
        tmp *= sign

/* sets a comblock register with an int value */
#define setreg(reg, val) (XpTermSetRegister(w, reg, CB_INT_TYPE, val))

/* dispatches a request to the canvas */
#define dispatch(code) w->term.chars_to_parse = count; \
     		       XpTermDispatchRequest(w, code)

/*
 * Implement the emu Emulation (see tdesc/emu.tdesc) hardcoded.
 */
void
parseHEmu(w, client_data, parse_buf)
TermWidget w;
caddr_t client_data;
String parse_buf;
{
     int tmp, sign, count =  w->term.chars_to_parse;
     String buf = parse_buf;
     String buf2;
     
     for (; count > 0; buf++) {
	  /* First see how much we can insert in one go */
	  for (buf2 = buf, tmp = 0; count && (*buf2 > 31);
	       buf2++, tmp++, count--)
	       ;
	  if (tmp) {
	       w->term.chars_to_parse = count;
	       XpTermInsertText(w, buf, tmp);
	       buf = buf2;
	  }
	  if (count) {
	       /* Check for known control characters next */
	       count--;
	       switch (*buf) {
		  case '\0':  /* \0 is ignored */
		    break;
		  case '\r':	/* Carriage return */
		    setreg('x', 0);
		    dispatch(OP_MOVE_ABS_COLUMN);
		    break;
		    
		  case '\n':	/* Newline */
		    setreg('y', 1);
		    dispatch(OP_MOVE_REL_ROW_SCROLLED);
		    break;
		    
		  case '\b':	/* Backspace */
		    setreg('x', -1);
		    dispatch(OP_MOVE_REL_COLUMN);
		    break;
		    
		  case '\007':	/* Bell */
		    dispatch(OP_RING_BELL);
		    break;
		  case '\t':	/* Tab */
		    XpTermDispatchRequest(w, OP_HOR_TAB);
		    break;
		  case '\033':	/* ESC */
		    /*
		     * The buffer might be empty before we've parsed
		     * the rest of the escape sequence.
		     * Save the actual buffer position (including the ESC)
		     */
		    buf2 = buf;
		    
		    next;
		    switch (*buf) {
		       case 'A': /* move 1 char up */
			 setreg('y', -1);
			 dispatch(OP_MOVE_REL_ROW);
			 break;
		       case 'B': /* move 1 char down */
			 setreg('y', 1);
			 dispatch(OP_MOVE_REL_ROW);
			 break;
		       case 'C': /* move 1 char left */
			 setreg('x', -1);
			 dispatch(OP_MOVE_REL_COLUMN);
			 break;
		       case 'D': /* move 1 char right */
			 setreg('x', 1);
			 dispatch(OP_MOVE_REL_COLUMN);
			 break;
		       case 'E': /* absolute Positioning '\EE<row>;<col>;' */
			 /* Read the row */
			 getint;
			 setreg('y', tmp);
			 /* Read the column */
			 getint;
			 setreg('x', tmp);
			 dispatch(OP_MOVE_ABS);
			 break;
		       case 'F': /* move 1 char up, scrolled */
			 XpTermSetRegister(w, 'y', CB_INT_TYPE, -1);
			 dispatch(OP_MOVE_REL_ROW_SCROLLED);
			 break;
		       case 'G': /* move 1 char down, scrolled */
			 setreg('y', 1);
			 dispatch(OP_MOVE_REL_ROW_SCROLLED);
			 break;
		       case 'H': /* move 1 line x 0 down, scrolled */
			 setreg('y', 1);
			 dispatch(OP_MOVE_REL_ROW_SCROLLED);
			 setreg('x', 0);
			 dispatch(OP_MOVE_ABS_COLUMN);
			 break;
		       case 'I': /* delete characters '\EI<num>;' */
			 /* Read the num */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_DELETE_CHARS);
			 break;
		       case 'J': /* erase characters '\EJ<num>;' */
			 /* Read the num */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_ERASE_CHARS);
			 break;
		       case 'K': /* delete to end of line */
			 dispatch(OP_DELETE_TO_EOL);
			 break;
		       case 'L': /* erase left part of line */
			 dispatch(OP_ERASE_LINE_LEFT);
			 break;
		       case 'M': /* erase lines  '\EL<num>;' */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_ERASE_LINES);
			 break;
		       case 'N': /* delete to end of screen */
			 dispatch(OP_DELETE_TO_EOSCR);
			 break;
		       case 'O': /* erase from top of screen */
			 dispatch(OP_ERASE_FROM_TOSCR);
			 break;
		       case 'P': /* clear screen */
			 dispatch(OP_CLEAR_SCREEN);
			 break;
		       case 'Q': /* insert lines  '\EQ<num>;' */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_INSERT_LINES);
			 break;
		       case 'R': /* delete lines  '\ER<num>;' */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_DELETE_LINES);
			 break;
		       case 'S': /* clear all attributes */
			 setreg('b', 15);
			 dispatch(OP_CLEAR_ATTRIBUTE);
			 break;
		       case 'T': /* set reverse attribute */
			 setreg('a', ATT_REVERSE);
			 dispatch(OP_SET_ATTRIBUTE);
			 break;
		       case 'U': /* set bold attribute */
			 setreg('a', ATT_BOLD);
			 dispatch(OP_SET_ATTRIBUTE);
			 break;
		       case 'V': /* set underline attribute */
			 setreg('a', ATT_UNDERL);
			 dispatch(OP_SET_ATTRIBUTE);
			 break;
		       case 'W': /* set blink attribute */
			 setreg('a', ATT_BLINK);
			 dispatch(OP_SET_ATTRIBUTE);
			 break;
		       case 'X': /* set overwrite mode */
			 dispatch(OP_OVERWRITE_MODE);
			 break;
		       case 'Y': /* set insert mode */
			 dispatch(OP_INSERT_MODE);
			 break;
		       case 'Z': /* turn cursor off*/
			 dispatch(OP_CURSOR_OFF);
			 break;
		       case 'a': /* turn cursor on */
			 dispatch(OP_CURSOR_ON);
			 break;
		       case 'b': /* set normal video */
			 dispatch(OP_NORMAL_VIDEO);
			 break;
		       case 'c': /* set reverse video */
			 dispatch(OP_REVERSE_VIDEO);
			 break;
		       case 'd': /* set cursor positioning absolute */
			 dispatch(OP_CURSOR_POS_ABSOLUTE);
			 break;
		       case 'e': /* set cursor positioning relative to  */
			         /* scrolling region */
			 dispatch(OP_CURSOR_POS_REL_TO_SCR_REG);
			 break;
		       case 'f': /* turn autowrap off */
			 dispatch(OP_DONT_WRAP);
			 break;
		       case 'g': /* turn autowrap on */
			 dispatch(OP_WRAP_AROUND);
			 break;
		       case 'h': /* set tab at current column */
			 dispatch(OP_SET_TAB_CUR_COL);
			 break;
		       case 'i': /* clear tab at current column */
			 dispatch(OP_CLEAR_TAB_CUR_COL);
			 break;
		       case 'j': /* clear all tabs */
			 dispatch(OP_CLEAR_ALL_TABS);
			 break;
		       case 'k': /* set scrolling region '\Ek<start>;<end>;' */
			 /* Read the start row */
			 getint;
			 setreg('a', tmp);
			 /* Read the end row */
			 getint;
			 setreg('b', tmp);
			 dispatch(OP_SET_SCROLL_REGION);
			 break;
		       case 'l': /* reset the scrolling region */
			 setreg('a', 0);
			 /* Ask for the screen size */
			 w->term.immediate = True;
			 dispatch(OP_CANVAS_SIZE);
			 tmp = (int) XpTermGetRegister(w, 'y', NULL);
			 setreg('b', tmp - 1);
			 dispatch(OP_SET_SCROLL_REGION);
			 break;
		       case 'm': /* set screen size '\Em<width>;<height>; */
			 /* Read the width */
			 getint;
			 setreg('x', tmp);
			 /* Read the column */
			 getint;
			 setreg('y', tmp);
			 dispatch(OP_SET_SCREEN_SIZE);
			 break;
		       case 'n': /* set the application keypad */
			 XpTermSetRegister(w, 'a', CB_STR_TYPE, "appKeypad");
			 dispatch(OP_OVERRIDE_TRANSLATIONS);
			 break;
		       case 'o': /* set the numeric keypad */
			 XpTermSetRegister(w, 'a', CB_STR_TYPE, "numKeypad");
			 dispatch(OP_OVERRIDE_TRANSLATIONS);
			 break;
		       case 'p': /* move rows relative '\Ep<num>;' */
			 /* Read the number */
			 getint;
			 setreg('y', tmp);
			 dispatch(OP_MOVE_REL_ROW);
			 break;
		       case 'q': /* move columns relative '\Eq<num>;' */
			 /* Read the number */
			 getint;
			 setreg('x', tmp);
			 dispatch(OP_MOVE_REL_COLUMN);
			 break;
		       case 'r': /* change fg color '\Er<color num>;' */
			 /* Read the number */
			 getint;
			 setreg('a', tmp);
			 dispatch(OP_CHANGE_FG_COLOR);
			 break;
		       case 's': /* change bg color '\Er<color num>;' */
			 /* Read the number */
			 getint;
			 setreg('b', tmp);
			 dispatch(OP_CHANGE_BG_COLOR);
			 break;
		       case ']': /* set title and icon '\E]0;<text>\007' */
			 {
			      unsigned char tmpstr[128];
			      Arg args[2];
			      
			      next;
			      if (*buf != '0')
				   break;
			      next;
			      if (*buf != ';')
				   break;
			      next;
			      for (tmp = 0;
				   *buf != '\007' && tmp < sizeof(tmpstr) - 1;
				   tmp++)
			      {
				   tmpstr[tmp] = *buf;
				   next;
			      }
			      tmpstr[tmp] = '\0';
			      
			      XtSetArg(args[0], XtNiconName, tmpstr);
			      XtSetArg(args[1], XtNtitle, tmpstr);
			      XtSetValues(XtParent(w), args, 2);
		         }
			 break;
			 
			 /*
			 * There is no default. if \E<unknown> is
			 * found the sequence will be eaten.
			 */
		    }
		    break;
		    
		  default:
		    /* Don't know what to do with it, insert it I guess */
		    w->term.chars_to_parse = count;
		    XpTermInsertText(w, buf, 1);
	       }
	  }
     }
     /* Update byte count to say we did it all */
     w->term.chars_to_parse = 0;
     return;

     /*
      * If we are in the middle of an escape sequence when the buffer
      * is empty, copy the rest to the beginning of the buffer, and
      * set chars_to_parse accordingly.
      */
   getout:
     w->term.chars_to_parse = buf - buf2;
     
     if (buf2 != parse_buf)
	  while (buf2 < buf)
	       *parse_buf++ = *buf2++;
}
