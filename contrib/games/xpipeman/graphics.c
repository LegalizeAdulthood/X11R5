/*
 * graphics.c  - Xpipeman
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
#include <X11/Shell.h>
#include "xpipeman.h"
#include "bitmaps1.h"
#include "bitmaps2.h"
#include "icon.h"

static Pixmap iconP;


static Pixmap tmp_pixmap[NUM_TMP_CURSOR_PIXMAPS]; 

/*----------------------------------------------------------------------*/

void
init_pixmaps(top_shell)
  Widget top_shell;
{
/*
 * Let's make some pixmaps and some cursors.
 * And then let's set the iconpixmap.
 */
  Pixmap tmpP;
  XColor fgcolor,bgcolor;
  Arg arg;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  fgcolor.pixel = fg;
  bgcolor.pixel = bg;
  fgcolor.flags =  DoRed | DoGreen | DoBlue;
  bgcolor.flags =  DoRed | DoGreen | DoBlue;
  XQueryColor(display,DefaultColormapOfScreen(XtScreen(playfield_widget)), &fgcolor);
  XQueryColor(display,DefaultColormapOfScreen(XtScreen(playfield_widget)), &bgcolor);


  iconP =  XCreateBitmapFromData(display,playfield,
		       icon_bits,icon_width,icon_height);

  tmp_pixmap[EMPTY] = XCreateBitmapFromData(display,playfield,
		       empty_bits, empty_width, empty_height);

  tmp_pixmap[1] = XCreateBitmapFromData(display,playfield, b1_bits,
					b1_width, b1_height);

  tmp_pixmap[2] = XCreateBitmapFromData(display,playfield, b2_bits,
					b2_width, b2_height);

  tmp_pixmap[3] = XCreateBitmapFromData(display,playfield, b3_bits,
					b3_width, b3_height);

  tmp_pixmap[4] = XCreateBitmapFromData(display,playfield, b4_bits,
					b4_width, b4_height);

  tmp_pixmap[5] = XCreateBitmapFromData(display,playfield, b5_bits,
					b5_width, b5_height);

  tmp_pixmap[6] = XCreateBitmapFromData(display,playfield, b6_bits,
					b6_width, b6_height);

  tmp_pixmap[7] = XCreateBitmapFromData(display,playfield, b7_bits,
					b7_width, b7_height);

  tmp_pixmap[ONEWAY] = XCreateBitmapFromData(display,playfield, b11w_bits,
					b11w_width, b11w_height);

  tmp_pixmap[ONEWAY+1] = XCreateBitmapFromData(display,playfield, b21w_bits,
					b21w_width, b21w_height);

  tmp_pixmap[ONEWAY+2] = XCreateBitmapFromData(display,playfield, b31w_bits,
					b31w_width, b31w_height);

  tmp_pixmap[ONEWAY+3] = XCreateBitmapFromData(display,playfield, b41w_bits,
					b41w_width, b41w_height);

  tmp_pixmap[ONEWAY+4] = XCreateBitmapFromData(display,playfield, b51w_bits,
					b51w_width, b51w_height);

  tmp_pixmap[ONEWAY+5] = XCreateBitmapFromData(display,playfield, b61w_bits,
					b61w_width, b61w_height);

  tmp_pixmap[HFULL] = XCreateBitmapFromData(display,playfield, b8_bits,
					b8_width, b8_height);

  tmp_pixmap[VFULL] = XCreateBitmapFromData(display,playfield, b9_bits,
					b9_width, b9_height);

  tmp_pixmap[FILLBLKSTART] = XCreateBitmapFromData(display,playfield, b11_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+1] = XCreateBitmapFromData(display,playfield, b12_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+2] = XCreateBitmapFromData(display,playfield, b13_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+3] = XCreateBitmapFromData(display,playfield, b14_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+4] = XCreateBitmapFromData(display,playfield, b15_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+5] = XCreateBitmapFromData(display,playfield, b21_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+6] = XCreateBitmapFromData(display,playfield, b22_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+7] = XCreateBitmapFromData(display,playfield, b23_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+8] = XCreateBitmapFromData(display,playfield, b24_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+9] = XCreateBitmapFromData(display,playfield, b25_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+10] = XCreateBitmapFromData(display,playfield, b31_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+11] = XCreateBitmapFromData(display,playfield, b32_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+12] = XCreateBitmapFromData(display,playfield, b33_bits,
				block_width, block_height);

  tmp_pixmap[FILLBLKSTART+13] = XCreateBitmapFromData(display,playfield, b34_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+14] = XCreateBitmapFromData(display,playfield, b35_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+15] = XCreateBitmapFromData(display,playfield, b41_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+16] = XCreateBitmapFromData(display,playfield, b42_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+17] = XCreateBitmapFromData(display,playfield, b43_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+18] = XCreateBitmapFromData(display,playfield, b44_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+19] = XCreateBitmapFromData(display,playfield, b45_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+20] = XCreateBitmapFromData(display,playfield, b51_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+21] = XCreateBitmapFromData(display,playfield, b52_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+22] = XCreateBitmapFromData(display,playfield, b53_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+23] = XCreateBitmapFromData(display,playfield, b54_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+24] = XCreateBitmapFromData(display,playfield, b55_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+25] = XCreateBitmapFromData(display,playfield, b61_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+26] = XCreateBitmapFromData(display,playfield, b62_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+27] = XCreateBitmapFromData(display,playfield, b63_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+28] = XCreateBitmapFromData(display,playfield, b64_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+29] = XCreateBitmapFromData(display,playfield, b65_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+30] = XCreateBitmapFromData(display,playfield, b71_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+31] = XCreateBitmapFromData(display,playfield, b72_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+32] = XCreateBitmapFromData(display,playfield, b73_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+33] = XCreateBitmapFromData(display,playfield, b74_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+34] = XCreateBitmapFromData(display,playfield, b75_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+35] = XCreateBitmapFromData(display,playfield, b76_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+36] = XCreateBitmapFromData(display,playfield, b77_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+37] = XCreateBitmapFromData(display,playfield, b78_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+38] = XCreateBitmapFromData(display,playfield, b79_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+39] = XCreateBitmapFromData(display,playfield, b7a_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+40] = XCreateBitmapFromData(display,playfield, b81_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+41] = XCreateBitmapFromData(display,playfield, b82_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+42] = XCreateBitmapFromData(display,playfield, b83_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+43] = XCreateBitmapFromData(display,playfield, b84_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+44] = XCreateBitmapFromData(display,playfield, b85_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+45] = XCreateBitmapFromData(display,playfield, b91_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+46] = XCreateBitmapFromData(display,playfield, b92_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+47] = XCreateBitmapFromData(display,playfield, b93_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+48] = XCreateBitmapFromData(display,playfield, b94_bits,
					block_width, block_height);

  tmp_pixmap[FILLBLKSTART+49] = XCreateBitmapFromData(display,playfield, b95_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT] = XCreateBitmapFromData(display,playfield, ob1_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT+1] = XCreateBitmapFromData(display,playfield, ob2_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT+2] = XCreateBitmapFromData(display,playfield, ob3_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT+3] = XCreateBitmapFromData(display,playfield, ob4_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT+4] = XCreateBitmapFromData(display,playfield, ob5_bits,
					block_width, block_height);

  tmp_pixmap[OBSRT+5] = XCreateBitmapFromData(display,playfield, ob6_bits,
					block_width, block_height);

  tmp_pixmap[OBSFIN] = XCreateBitmapFromData(display,playfield, ob7_bits,
					block_width, block_height);

  tmp_pixmap[SHOWFLOW] = XCreateBitmapFromData(display,playfield, fbar_bits,
					fbar_width, fbar_height);

  tmp_pixmap[SHOWFLOW+1] = XCreateBitmapFromData(display,playfield, fbar1_bits,
					fbar_width, fbar_height);

  tmp_pixmap[SHOWFLOW+2] = XCreateBitmapFromData(display,playfield, fbar2_bits,
					fbar_width, fbar_height);

  tmp_pixmap[SHOWFLOW+3] = XCreateBitmapFromData(display,playfield, flowst_bits,
					flowst_width, flowst_height);

  tmp_pixmap[START] = XCreateBitmapFromData(display,playfield, start_bits,
					start_width, start_height);

  tmp_pixmap[STARTFULL] = XCreateBitmapFromData(display,playfield, startfull_bits,
					startfull_width, startfull_height);

  tmp_pixmap[ARROW] = XCreateBitmapFromData(display,playfield, arrow_bits,
					arrow_width, arrow_height);

  tmp_pixmap[BAR] = XCreateBitmapFromData(display,playfield, bar_bits,
					bar_width, bar_height);

  XtSetArg(arg,XtNiconPixmap,iconP);
  XtSetValues(top_shell,&arg,1);
}


/*----------------------------------------------------------------------*/

void display_allpixmaps()
{
  /* displays all the pixmaps used to construct the pipe and their filling*/
   int x,y;
   for (x=1 ; x < 10; x++)
     pipe_board[x-1][0]=x;

   for (x=2 ; x<8; x++)
     for (y=1; y<6; y++)
        pipe_board[x-2][y]= (x*5) +(y-1);
   x=8;
     for (y=1; y<10; y++)
        pipe_board[x-2][y]= (x*5) +(y-1);
   for (x=9 ; x<11; x++)
     for (y=1; y<6; y++)
        pipe_board[x-2][y]= (x*5) +(y-1) + 5;
   display_level();
 }
/*----------------------------------------------------------------------*/

void
display_level()
{
/* Naive refresh algorithm.... */
  int x,y;

  XClearWindow(display,playfield);
  for(y=0;y<MAXY;y++)
  {
    for(x=0;x<MAXX;x++)
         if (pipe_board[x][y] < (BAR+1))
        {
            XCopyPlane(display,tmp_pixmap[pipe_board[x][y]],playfield,gc,0,0,
                       block_width,block_height,
                       pos_to_coord(x),pos_to_coord(y),1);
	  }
  }

  if (INXRANGE(block_x) && INYRANGE(block_y))       /*overlay current block */
   XCopyPlane(display,tmp_pixmap[current_block],playfield,gc,0,0,
        block_width,block_height,
        pos_to_coord(block_x),pos_to_coord(block_y),1);

}
/*----------------------------------------------------------------------*/

void
redisplay_sequence()
{
/* Naive refresh algorithm.... for sequence */
  int x=(MAXX-2),y;
  for(y=0;y<MAXY;y++)
  {
    if (pipe_board[x][y] < (BAR+1))
        {
            XCopyPlane(display,tmp_pixmap[pipe_board[x][y]],playfield,gc,0,0,
                       block_width,block_height,
                       pos_to_coord(x),pos_to_coord(y),1);
	  }
  }
  x = MAXX -3 ; y = MAXY -1;
  if (pipe_board[x][y] < (BAR+1))
        {
            XCopyPlane(display,tmp_pixmap[pipe_board[x][y]],playfield,gc,0,0,
                       block_width,block_height,
                       pos_to_coord(x),pos_to_coord(y),1);
	  }
}
/*----------------------------------------------------------------------*/

void
redraw_block(x,y)
{
/*redraws a single block*/
  XCopyPlane(display,tmp_pixmap[pipe_board[x][y]],playfield,gc,0,0,
             block_width,block_height,
              pos_to_coord(x),pos_to_coord(y),1);

}
/*----------------------------------------------------------------------*/

void
show_movement()
{
/* 
 * redraws previous position and overlays current block on new position.
 */
  int x,y;

  if (INXRANGE(last_block_x) && INYRANGE(last_block_y))
    redraw_block(last_block_x,last_block_y);

  
  if (INXRANGE(block_x) && INYRANGE(block_y))
   XCopyPlane(display,tmp_pixmap[current_block],playfield,gc,0,0,
        block_width,block_height,
        pos_to_coord(block_x),pos_to_coord(block_y),1);

}

/*----------------------------------------------------------------------*/

/*ARGSUSED*/
XtEventHandler
redisplay_level(w, closure, event)
  Widget w;
  caddr_t closure;
  XExposeEvent *event;
{
/* refresh entire screen -- e.g. expose events.  Just a little naive. */
  display_level();
  XFlush(display);
}

/*----------------------------------------------------------------------*/

void free_pixmaps()
{
  int i;

  for(i=0;i++;i<NUM_TMP_CURSOR_PIXMAPS) 
    XFreePixmap(display,tmp_pixmap[i]);

}



/*----------------------------------------------------------------------*/



