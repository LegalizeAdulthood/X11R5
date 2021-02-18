/*
 * xpipeman.h  - Xpipeman
 *
 * Send Constructive comments, bug reports, etc. to either
 *
 *          JANET: pavern@uk.ac.man.cs
 *
 *  or      INER : pavern%cs.man.ac.uk@nsfnet-relay.ac.uk
 *
 * All other comments > /dev/null !!
 * 
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
 */

/* 
 * from main.c
 */
extern Display 	*display;
extern Window 	playfield;
extern Widget 	playfield_widget;
extern GC 	gc,
		cleargc;
extern Pixel 	fg, 
		bg;

extern void 	update_score();
extern void 	update_level();
extern void 	update_bonus();
extern void 	update_remain();

/*
 * from graphics.c
 */

/* defines the order of the blocks in the pixmap array */

#define NUMBLOCKS 8     /* number of empty blocks */
#define TOTALBLOCKS (ONEWAY+6)   /* including one way blocks */
#define NoBlksDefined BAR+1
#define EMPTY 	0
#define ONEWAY NUMBLOCKS
#define HFULL TOTALBLOCKS
#define VFULL (HFULL+1)
#define FILLBLKSTART (VFULL+1)
#define OBSRT   (FILLBLKSTART+50)
#define OBSFIN  (OBSRT+6)
#define SHOWFLOW (OBSFIN +1)
#define START    (SHOWFLOW +4)
#define STARTFULL (START+1)
#define ARROW  (STARTFULL +1)
#define BAR    (ARROW +1)
#define NUM_TMP_CURSOR_PIXMAPS (BAR+5)


extern XtEventHandler redisplay_level();

extern void 	init_pixmaps(), 
                display_allpixmaps(),
		display_level(), 
		free_pixmaps(), 
		show_movement(),
                redraw_block(),
                redisplay_sequence();


#define CELLSIZE 40

#define pos_to_coord( _pos_ ) (( _pos_ ) * CELLSIZE + 5)
#define coord_to_pos( _pos_ ) ((( _pos_ ) - 5) / CELLSIZE)

/*
 * from actions.c
 */


extern XtActionProc	do_nothing_action(),
			move_action(),
                        move_here_action(),
                        fast_flow_action(),
                        place_action();

extern XtEventHandler 	pointer_moved();


/*
 * from score.c
 */

#ifndef SCORE_FILE
#  define SCORE_FILE "/usr/games/lib/xpipescores"
#endif

#ifndef MAXSCORES
#  define MAXSCORES 20
#endif

extern void 	check_score(), 
		create_high_score_popup();

extern void	show_scores_callback();

extern char *score_filename;

/*
 * from game.c
 */

# 	define 	MAXX 15
#  	define 	MAXY 10


#define FASTFLOW 100

#define LEFT    1
#define RIGHT   2
#define UP      4
#define DOWN    8
#define STILL   16

#define for_each	for(x=0;x<MAXX;x++) \
			  for(y=0;y<MAXY;y++)

/* I know, I KNOW... global variables! */

extern XtIntervalId current_callback;

extern int 	block_x, block_y,level,
           	last_block_x, last_block_y;

extern int	pipe_board[MAXX][MAXY];

extern int      buttons_disabled; /*stops button events during level change*/

extern int 	score,
		game_active;

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

#define INXRANGE( _x_ )  (((_x_) >=0) && ((_x_)<(MAXX-3)))
#define INYRANGE( _y_ )  (((_y_) >=0) && ((_y_)<MAXY))

extern void 	new_game(), 
		add_score(), 
		new_level(),
                increment_sequence(),
                reset_block_origin(),
                remove_block(),
                speed_up_flow(),
                draw_flow(),
                increment_flow(),
                block_replace();
extern int  	can_go(),
                can_place(),
                place_block();
extern int      current_block;

/*
   From info.c
*/
extern void  show_info(),
             create_info_popup();

/*
   From popup.c
*/

extern void  show_level_over_popup(),
             show_game_over_popup(),
	     level_over_popdown(),
	     game_over_popdown(),
	     create_general_popups();
