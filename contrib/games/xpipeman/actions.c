/*
 * actions.c  - Xpipeman
 *
 * Send Constructive comments, bug reports, etc. to either
 *
 *          JANET: pavern@uk.ac.man.cs
 *
 *  or      INER : pavern%cs.man.ac.uk@nsfnet-relay.ac.uk
 *
 * All other comments > /dev/null !!
 * 
 * Copyright 1991 Nigel Paver
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the author's name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE 
 * AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *
 *
 * Acknowledgements to Brian Warkentine (brian@sun.COM) for his xrobots
 * program (copyright  1989) which I cannibalized to write this program
 */


#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <math.h>
#include "xpipeman.h"

/*----------------------------------------------------------------------*/
/*ARGSUSED*/
XtActionProc
do_nothing_action(w,event,params,num_params)
  Widget w;
  XEvent *event;
  String *params;
  Cardinal *num_params;
{
  /* do nothing */
}

/*----------------------------------------------------------------------*/
/*ARGSUSED*/
XtActionProc
move_here_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
  /* called to move a block to a specific place - does NOT place it */
  int tmp_block_x, tmp_block_y;

  if((!game_active) || buttons_disabled) return;

  tmp_block_x = coord_to_pos(event->x);
  tmp_block_y = coord_to_pos(event->y);
  if (can_go(tmp_block_x,tmp_block_y))
    {
      if ((block_x == MAXX-3) && ( block_y == MAXY-1)) /*special case */ 
	{
	  pipe_board[MAXX-3][MAXY-1] = EMPTY; /* first move */
          redraw_block(MAXX-3,MAXY-1);
	}
      last_block_x = block_x;
      last_block_y = block_y;

      block_x = tmp_block_x;
      block_y = tmp_block_y;
      show_movement();
    }
  XFlush(display);
}

/*----------------------------------------------------------------------*/
/*ARGSUSED*/
XtActionProc
display_pixmap_action(w,event,params,num_params)
  Widget w;
  XEvent *event;
  String *params;
  Cardinal *num_params;
{
  if (!(game_active))
     display_allpixmaps();
}
/*----------------------------------------------------------------------*/
/*ARGSUSED*/
XtActionProc
fast_flow_action(w,event,params,num_params)
  Widget w;
  XEvent *event;
  String *params;
  Cardinal *num_params;
{
  if((!game_active) || buttons_disabled) return;
   speed_up_flow(); 
}

/*----------------------------------------------------------------------*/

/*ARGSUSED*/
XtActionProc
move_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
/* 
 *  Called to move the block with the keyboard keys.
 */

  int diff_x=0, diff_y=0;
  
  int  tmp_num_params;

  tmp_num_params= *num_params;

  if((!game_active) || buttons_disabled) return;

  while((tmp_num_params)--) {

    if(!strcmp("right",*(params+tmp_num_params)))	diff_x = 1;
    if(!strcmp("left", *(params+tmp_num_params)))	diff_x = -1;
    if(!strcmp("up",   *(params+tmp_num_params)))	diff_y = -1;
    if(!strcmp("down", *(params+tmp_num_params)))	diff_y = 1;
  }

  if( can_go(block_x+diff_x,block_y+diff_y) ) {
    if ((block_x == MAXX-3) && ( block_y == MAXY-1)) /*special case */ 
      {
	pipe_board[MAXX-3][MAXY-1] = EMPTY; /* first move */
        redraw_block(MAXX-3,MAXY-1);
      }

    last_block_x = block_x;
    last_block_y = block_y;

    block_x += diff_x;
    block_y += diff_y;
    show_movement();
  }
  XFlush(display);
}



/*ARGSUSED*/
XtActionProc
place_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
/* 
 Places a block 
 */
  int diff_x, diff_y;

  if((!game_active) || buttons_disabled) return;
  if (!place_block()) XBell(XtDisplay(w),100);
}


/*ARGSUSED*/
XtEventHandler
pointer_moved(w, closure, event)
  Widget w;
  caddr_t closure;
  XPointerMovedEvent *event;
{
  if(game_active) ;

}

