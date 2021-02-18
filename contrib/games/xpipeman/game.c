/*
 * game.c  - Xpipeman
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

#include <X11/Intrinsic.h>
#include "xpipeman.h"

/* some of these are global */
int block_x, block_y, last_block_x, last_block_y;
int buttons_disabled;
int remaining_blocks;
int current_block; /*current block type*/



XtIntervalId current_callback;

int pipe_board[MAXX][MAXY];

int	score,
        bonus,
	level,
	game_active;

static int last_new_block;
static int num_blk[TOTALBLOCKS+1];  /*count number of type of each block*/
static int blktime, blkrstart,blkfull,blkstarttime;
static int flow_x,flow_y,flow_dir,flow_blocks,flow_time,flow_time_start;

#define FUP    0
#define FDOWN  3
#define FLEFT  1
#define FRIGHT 2

#define BLOCKED 4 

/* Model of the flow through the various blocks */

static int flow[TOTALBLOCKS+2][5]=
     { { BLOCKED,BLOCKED,BLOCKED,BLOCKED,BLOCKED}, /* EMPTY Block */
	 { BLOCKED,FRIGHT,FLEFT,BLOCKED,BLOCKED},   /*1*/
	 { FDOWN,BLOCKED,BLOCKED,FUP,BLOCKED},      /*2*/
	 { BLOCKED,BLOCKED,FDOWN,FRIGHT,BLOCKED},   /*3*/
	 { BLOCKED,FDOWN,BLOCKED,FLEFT,BLOCKED},    /*4*/
	 { FRIGHT,BLOCKED,FUP,BLOCKED,BLOCKED},     /*5*/
	 { FLEFT,FUP,BLOCKED,BLOCKED,BLOCKED},      /*6*/
	 { FDOWN,FRIGHT,FLEFT,FUP,BLOCKED},         /*7*/
	 { BLOCKED,FRIGHT,BLOCKED,BLOCKED,BLOCKED}, /*1 - 1way*/
	 { FDOWN,BLOCKED,BLOCKED,BLOCKED,BLOCKED},  /*2 - 1way*/
	 { BLOCKED,BLOCKED,BLOCKED,FRIGHT,BLOCKED}, /*3 - 1way*/
	 { BLOCKED,BLOCKED,BLOCKED,FLEFT,BLOCKED},  /*4 - 1way*/
	 { FRIGHT,BLOCKED,BLOCKED,BLOCKED,BLOCKED}, /*5 - 1way*/
	 { FLEFT,BLOCKED,BLOCKED,BLOCKED,BLOCKED},  /*6 - 1way*/
	 { FDOWN,BLOCKED,BLOCKED,FUP,BLOCKED},      /*8*/
	 { BLOCKED,FRIGHT,FLEFT,BLOCKED,BLOCKED},   /*9*/
       };

#define MAXLEVELS 35

static int obmaps[MAXLEVELS][MAXY]=
{{0,0,0,0,0,0,0,0,0,0}, /*0*/
 {0,0,0,0,7,0,0,0,0,0},
 {0,0,9,0,0,0,0,3,0,0},
 {0,2,0,0,6,0,0,0,4,0},
 {0,0,4,0,6,0,8,0,0,0},
 {0,9,0,1,0,8,0,2,0,0}, /*5*/
 {0,0,0,5,4,6,8,7,0,0},
 {0,3,0,10,0,6,0,1,8,0},
 {0,2,7,0,5,0,8,6,3,0},
 {0,1,2,6,8,0,9,4,7,0},
 {0,9,1,8,2,7,3,6,4,0}, /*10*/
 {0,4,6,3,7,2,8,1,9,0},
 {6,6,6,6,6,0,6,6,6,6},
 {8,8,8,8,8,8,8,8,0,8},
 {0,2,0,0,6,0,0,0,4,0},  /* level 14 repeat but with one way bits */
 {0,0,4,0,6,0,8,0,0,0},
 {0,9,0,1,0,8,0,2,0,0},
 {0,0,0,5,4,6,8,7,0,0},
 {0,3,0,10,0,6,0,1,8,0},
 {0,2,7,0,5,0,8,6,3,0},
 {0,1,2,6,8,0,9,4,7,0},  /*20*/
 {0,9,1,8,2,7,3,6,4,0},
 {0,4,6,3,7,2,8,1,9,0},
 {6,6,6,6,6,0,6,6,6,6},
 {8,8,8,8,8,8,8,8,0,8},  /* level 24 start to wind up the speed */
 {2,9,3,8,4,7,5,6,6,5},
 {0,1,2,6,8,0,9,4,7,0},
 {0,9,1,8,2,7,3,6,4,0},
 {0,4,6,3,7,2,8,1,9,0},
 {4,4,4,0,4,4,4,0,4,4},  /*30 */
 {0,6,6,6,6,6,6,6,6,0},
 {5,5,5,5,0,5,5,5,5,0},
 {6,6,6,6,6,0,6,6,6,6},
 {3,3,3,3,3,3,3,3,3,0},
 {2,9,3,8,4,7,5,6,6,5}  /*35 levels*/
};


/* Minimum number of connected blocks to complete each stage*/
static int blocksrequired[MAXLEVELS]=
    {4,5,6,7,8,9,10,11,12,13,14,15,20,25,       /* 0  -13 */
     10,12,14,16,20,24,28,32,36,40,             /* 14 -23 */
     35,36,37,38,40,41,42,43,44,45,50};         /* 24 -34*/

/*flow rates in mSecs*/
static int flowrate[MAXLEVELS]=
      {1000,1000,1000,1000,1000,980,970,950,920,900,850,825,800,775, /* 0 -13*/
       1000,980,970,950,920,900,850,825,800,775,                     /*14 -23*/
       775,725,700,650,600,550,500,500,475,450,400};             /*24- 34*/

static int flowstart[MAXLEVELS]={50000,50000,50000,50000,50000,  /* 0 -4 */
				 45000,45000,45000,45000,45000,  /* 5 -9*/
				 40000,40000,40000,40000,50000,  /* 10 -14*/
				 48000,46000,44000,42000,40000,  /* 15 -19 */
				 39000,38000,37000,36000,35000,  /* 20 -24 */
				 32000,30000,28000,26000,24000,  /* 25 -29 */
				 22000,20000,18000,16000,14000}; /* 30 -34 */

static int MaxBlockNo;  /*maxmimum block allowed in random sequence*/

static int onewaytime=14;  /*start oneway blocks on level 14*/
static int oneway=1;   /* number of 1 way blocks in the sequence at present*/
#define MAXONEWAYS 2   /* maximum one ways in the sequence at any one time*/

void start_new_level();
void level_over();
void game_over();
void reset_score();
void show_when_flow();
/*----------------------------------------------------------------------*/
void
new_score(n)
  int n;
{
  score += n;
  update_score(score);
}
/*----------------------------------------------------------------------*/
void
new_bonus(n)
  int n;
{
  bonus += n;
  update_bonus(bonus);
}
/*----------------------------------------------------------------------*/
void
reset_score()
{
  score =0;
  update_score(score);
}
/*----------------------------------------------------------------------*/
void
reset_bonus()
{
  bonus =0;
  update_bonus(bonus);
}
/*----------------------------------------------------------------------*/
void
reset_level()
{
  level =0;
  update_level(level);
}

/*----------------------------------------------------------------------*/
void
new_game()
{
 buttons_disabled=1;
 game_active= 1; 
 reset_score();
 reset_bonus();
 reset_level();
 all_popdown();
 flow_blocks=0;
 current_block = 0;
 new_level();

}
/*----------------------------------------------------------------------*/
int
place_block()
{
  if (can_place(block_x,block_y))
      {
	remove_block();    /* from *random* sequence stats */
        if (pipe_board[block_x][block_y] != EMPTY)
	  new_score(-20);   /* changing blocks costs double points! */
	pipe_board[block_x][block_y] = current_block;   
	redraw_block(block_x,block_y);
	increment_sequence();
	reset_block_origin();
	return 1;
      }
  else
    return 0;
}
/*----------------------------------------------------------------------*/
void
reset_block_origin()
/*resets the coordinates for the new block*/
{
  block_x = MAXX-3;
  block_y = MAXY-1;
}
/*----------------------------------------------------------------------*/

void
remove_block()
/*removes block from frequency table */
{
  num_blk[current_block]--;
  if (current_block >= ONEWAY) oneway--;
}

/*----------------------------------------------------------------------*/

void
place_objects(levelnum)
int levelnum;
{ int i;
  for (i=0 ; i <MAXY; i++)
    { if (obmaps[levelnum][i] !=0 )
	{
	  pipe_board[obmaps[levelnum][i]][i] = 
	    ((int)random() %((OBSFIN-OBSRT)+1)) + OBSRT;
	}
    }
}
/*----------------------------------------------------------------------*/

void
increment_sequence()
{
  int y,blk_num;
   blk_num = last_new_block;
   pipe_board[MAXX-3][MAXY-1] = pipe_board[MAXX-2][MAXY-1];

    for (y=MAXY-1; y >0 ; y--)
      pipe_board[MAXX-2][y] = pipe_board[MAXX-2][y-1];

  while (blk_num == last_new_block) {
    blk_num = (int)random() % (MaxBlockNo-1);  /*don't count empty block*/
    blk_num = blk_num+1;
    if ((blk_num >= ONEWAY) && (oneway >=MAXONEWAYS))
      blk_num = last_new_block;
    else {
      if (num_blk[blk_num] > 1) blk_num = last_new_block;
      else if (blk_num != last_new_block) num_blk[blk_num]++;
      if (blk_num >= ONEWAY && (blk_num != last_new_block)) oneway++;
    }
  }
  pipe_board[MAXX-2][0] = blk_num;
  last_new_block = blk_num;

  redisplay_sequence();  
  current_block = pipe_board[MAXX-3][MAXY-1];

}
/*----------------------------------------------------------------------*/
  
void
initialise_sequence()
{
    int  y,blk_num=0,last_blk_num =0;
    oneway = 1;
    for (y=1 ; y<= MaxBlockNo; y++)
      num_blk[y] = 0;   /*only 2 of each block allowed any any time */

    blk_num = (int)random() % (MaxBlockNo-1);  /*don't count empty block*/
    blk_num = last_blk_num = blk_num+1;
    pipe_board[MAXX-3][MAXY-1] = blk_num;
    num_blk[blk_num]++;
        
    for (y =(MAXY-1) ; y >= 0 ; y--)   /*set up block sequence */
      { 
	while (blk_num == last_blk_num) {
	  blk_num = (int)random() % (MaxBlockNo-1);  /*don't count empty block*/
	  blk_num = blk_num+1;
	  if ((blk_num >= ONEWAY) && (oneway>= MAXONEWAYS)) 
	    blk_num = last_blk_num;
	  else {
	    if (num_blk[blk_num] > 1) blk_num = last_blk_num;
	    else if (blk_num != last_blk_num) num_blk[blk_num]++;
	    if (blk_num >= ONEWAY && (blk_num != last_blk_num)) oneway++;
	  }
	}
        
        last_blk_num = blk_num;
        pipe_board[MAXX-2][y] = blk_num;
      }
    current_block = pipe_board[MAXX-3][MAXY-1];
    last_new_block  = pipe_board[MAXX-2][0];
  }

/*----------------------------------------------------------------------*/
void
level_over(data,id)
caddr_t data;
XtIntervalId *id;
{    /* find any unconnected blocks  -10 for each one! */
  int x,y; 
 for(y=0;y<MAXY;y++)
  {
    for(x=0;x<MAXX-3;x++)
      { if ((pipe_board[x][y] >0 ) && (pipe_board[x][y] < TOTALBLOCKS))
	  {
	    new_score(-10);
	    pipe_board[x][y] = EMPTY;
	    redraw_block(x,y);
	  }
      }
  }
  if (flow_blocks >= blocksrequired[level-1])
    {
      show_level_over_popup();
    }
  else
    {
      show_game_over_popup();
      current_callback = XtAddTimeOut(3000,game_over,NULL);
    }
}
/*----------------------------------------------------------------------*/
void
game_over(data,id)
caddr_t data;
XtIntervalId *id;
{
  game_active =0;
  game_over_popdown();
  nomore_popdown();
  new_score(bonus);
  check_score(score,(level-1));
  reset_bonus();
}
/*----------------------------------------------------------------------*/
void
start_new_level()
{
  if (level > 0)                  /* add in the bonus from previous level */
    {
      new_score(bonus);
      reset_bonus();
    }
  new_level();
}
/*----------------------------------------------------------------------*/
void
new_level()
{
  int x,y,tmp;

  if (level >= MAXLEVELS){
    game_active =0 ;
    show_nomore_popup();
    current_callback = XtAddTimeOut(3000,game_over,NULL);
  }
  else 
    {
      update_level(level);

      for_each                            /* clean out the array */
	{
	  pipe_board[x][y] = EMPTY;
	}
      
      for (y=0 ; y < MAXY-1 ; y++)         /*draw 2 vertical bars*/
	{
	  pipe_board[MAXX-3][y] = BAR;
	  pipe_board[MAXX-1][y] = BAR;
	}
      pipe_board[MAXX-1][MAXY-1] = BAR;
      pipe_board[MAXX-3][MAXY-2] = ARROW;
      pipe_board[MAXX-3][1]      = SHOWFLOW+3;

      for (y=2 ; y < 5 ; y ++)
	pipe_board[MAXX-3][y] = SHOWFLOW;
      if (level >= onewaytime)
	MaxBlockNo = TOTALBLOCKS;
      else
	MaxBlockNo = NUMBLOCKS;

      initialise_sequence();
      place_objects(level);
      do
	{
	  flow_x = ((int)random()%(MAXX-6)) +2;   /* assign start to new space */
	  flow_y = ((int)random()%(MAXY-4)) +1;
	}
      while ((pipe_board[flow_x][flow_y] != EMPTY) ||
	     (pipe_board[flow_x+1][flow_y] != EMPTY) ||
	     (pipe_board[flow_x+1][flow_y+1] != EMPTY) ||
	     (pipe_board[flow_x+1][flow_y-1] != EMPTY));
      
      pipe_board[flow_x][flow_y] = START;
      
      remaining_blocks=blocksrequired[level];
      update_remain(remaining_blocks);
      flow_dir = FRIGHT;  /* must flow right out of the start block */
      flow_blocks = 0;
      flow_time = flowrate[level];
      flow_time_start = flowstart[level]/ 6 ;

      blkstarttime =0;
      blktime=3;
      blkrstart=0;
      blkfull = STARTFULL;

      if (current_callback != NULL)
	XtRemoveTimeOut(current_callback);
      current_callback = XtAddTimeOut(flow_time_start,show_when_flow,NULL);
      
      reset_block_origin();
      display_level();
      
      level++;
      buttons_disabled=0;
    }
}
/*----------------------------------------------------------------------*/

void
speed_up_flow()
{ int i;
 if (current_callback != NULL)
   XtRemoveTimeOut(current_callback);
 flow_time = FASTFLOW;
 current_callback= XtAddTimeOut(flow_time,draw_flow,NULL);
 for (i=2 ; i <5 ; i++)
   {      /* make sure flow start meter full */
     pipe_board[MAXX-3][i] = SHOWFLOW+2 ;
     redraw_block(MAXX-3,i);
   }

}
/*----------------------------------------------------------------------*/
void
show_when_flow(data,id)
caddr_t data;
XtIntervalId *id;
{ int start_flowing=0;

  blkstarttime++;

  switch (blkstarttime) {
    case 1: {
      pipe_board[MAXX-3][4] = SHOWFLOW+1;
      redraw_block(MAXX-3,4);
      break;
    }
    case 2: {
      pipe_board[MAXX-3][4] = SHOWFLOW+2;
      redraw_block(MAXX-3,4);
      break;
    }
    case 3: {
      pipe_board[MAXX-3][3] = SHOWFLOW+1;
      redraw_block(MAXX-3,3);
      break;
    }
    case 4: {
      pipe_board[MAXX-3][3] = SHOWFLOW+2;
      redraw_block(MAXX-3,3);
      break;
    }
    case 5: {
      pipe_board[MAXX-3][2] = SHOWFLOW+1;
      redraw_block(MAXX-3,2);
      break;
    }
    case 6: {
      pipe_board[MAXX-3][2] = SHOWFLOW+2;
      redraw_block(MAXX-3,2);
      start_flowing =1;

      break;
    }
    default: {}
    }
   if (start_flowing)
     {
      if (current_callback != NULL)
	XtRemoveTimeOut(current_callback);
      current_callback = XtAddTimeOut(flow_time_start,draw_flow,NULL);
    }
  else
    {
      if (current_callback != NULL)
	XtRemoveTimeOut(current_callback);
      current_callback = XtAddTimeOut(flow_time_start,show_when_flow,NULL);
    }
}
    
/*----------------------------------------------------------------------*/
void
draw_flow(data,id)
caddr_t data;
XtIntervalId *id;
{
  current_callback = NULL;
  if (blktime < 2)
    {
      pipe_board[flow_x][flow_y] = blkrstart++;
      current_callback= XtAddTimeOut(flow_time,draw_flow,NULL);
      redraw_block(flow_x,flow_y);
      blktime++;
    }
  else
    {
      pipe_board[flow_x][flow_y] = blkfull;
      redraw_block(flow_x,flow_y);
      increment_flow();
    } 
}
  
/*----------------------------------------------------------------------*/
void
increment_flow()

{
  int new_dir,i,failed=0;
  caddr_t data;  /*dummy variables*/
  XtIntervalId *id;
      switch (flow_dir) {
      case FLEFT: { 
	flow_x--;
	break;
      }
      case FRIGHT: { 
	flow_x++;
	break;
      }
      case FDOWN: { 
	flow_y++;
	break;
      }
      case FUP: { 
	flow_y--;
	break;
      }
      }

      if ((!(INYRANGE(flow_y) && INXRANGE(flow_x))) ||
	  (pipe_board[flow_x][flow_y] >= FILLBLKSTART)) failed=TRUE;
      else if (!(failed = ((new_dir= flow[pipe_board[flow_x][flow_y]][flow_dir^3]) == BLOCKED)))
       flow_blocks++;


      if (failed) {
	buttons_disabled = 1;
	if (current_callback != NULL)
	  XtRemoveTimeOut(current_callback);
	current_callback = XtAddTimeOut(3000,level_over,NULL);
      }
      else
	{
	  block_replace(flow_dir^3);
          if (remaining_blocks >0)
	    update_remain(--remaining_blocks);
	  flow_dir = new_dir;
	  draw_flow(data,id);
	}
}    

void
block_replace(flw_dir)
int flw_dir;

{
   int curr_block,first,setalready=0,blk_score=0,blk_bonus=0;
   curr_block = pipe_board[flow_x][flow_y];
   blk_score=10;  /* 10 points for connectin a pipe */
   switch (curr_block) {
   case 1:{
     if (flw_dir == FLEFT)
        first = 1;
     else first = 0;
     break;
   }
   case 2:{
     if (flw_dir == FUP)
        first = 1;
     else first = 0;
     break;
   }
   case 3:{
     if (flw_dir == FRIGHT)
        first = 1;
     else first = 0;
     break;
   }
   case 4:{
     if (flw_dir == FLEFT)
        first = 1;
     else first = 0;
     break;
   }
   case 5:{
     if (flw_dir == FRIGHT)
        first = 1;
     else first = 0;
     break;
   }
   case 6:{
     if (flw_dir == FLEFT)
        first = 1;
     else first = 0;
     break;
   }
   case 7:{
     if (flw_dir == FLEFT){
       blkrstart = FILLBLKSTART + (curr_block-1)*5;
       blkfull   = HFULL;}

     else if (flw_dir==FRIGHT) {
       blkrstart = FILLBLKSTART + (curr_block-1)*5 +3;
       blkfull   = HFULL; }  

     else if (flw_dir==FUP){
       blkrstart = FILLBLKSTART + (curr_block-1)*5 +5;
       blkfull   = VFULL;}

     else if (flw_dir==FDOWN) {
       blkrstart = FILLBLKSTART + (curr_block-1)*5 +3+5;
       blkfull   = VFULL; }  
     setalready = 1;

     break;
   }
   case ONEWAY:
   case (ONEWAY+1):{
     blkrstart = FILLBLKSTART + (curr_block-ONEWAY)*5;
     blkfull   = FILLBLKSTART + (curr_block-ONEWAY)*5 + 2;
     setalready =1;
     blk_bonus = 10;
     break;
   }
   case (ONEWAY+2):
   case (ONEWAY+3):
   case (ONEWAY+4):
   case (ONEWAY+5):{
     blkrstart = FILLBLKSTART + (curr_block-ONEWAY)*5 +3;
     blkfull   = FILLBLKSTART + (curr_block-ONEWAY)*5 + 2;
     setalready =1;
     blk_bonus = 10;
     break;
   }
   case HFULL:{
     if (flw_dir == FUP)
	 blkrstart = FILLBLKSTART + (8-1)*5 +5;
     else 
       blkrstart = FILLBLKSTART + (8-1)*5 +3+5;
       blkfull = FILLBLKSTART + (8-1)*5 +2+5;
     setalready = 1;
     blk_bonus = 40;  /*40 points for creating a loop */
     break;
   }
   case VFULL:{
     if (flw_dir == FLEFT)
	 blkrstart = FILLBLKSTART + (9-1)*5 +5;
     else 
       blkrstart = FILLBLKSTART + (9-1)*5 +3+5;
       blkfull = FILLBLKSTART + (9-1)*5 +2+5;
     setalready = 1;
     blk_bonus = 40;  /*40 points for creating a loop */
     break;
   }
   default:
     break;
   }

   if (!setalready)
     {
       if (first)
	 blkrstart = FILLBLKSTART + (curr_block-1)*5;
       else
	 blkrstart = FILLBLKSTART + (curr_block-1)*5 +3;
       blkfull = FILLBLKSTART + (curr_block-1)*5 +2;
     }
   blktime=0;
   new_score(blk_score);
   new_bonus(blk_bonus);
}  
/*----------------------------------------------------------------------*/
int
can_go(x,y)
  int x,y;
{

  if( INYRANGE(y) ) 
    if( INXRANGE(x) ) {
      if (pipe_board[x][y] == BAR)    return 0;
    }

  if( !INXRANGE(x) )  return 0;
  if( !INYRANGE(y) )  return 0;

  return 1;
}
/*----------------------------------------------------------------------*/
int
can_place(x,y)
  int x,y;
{

  if( INYRANGE(y) ) 
    if( INXRANGE(x) ) {
      if (pipe_board[x][y] >= FILLBLKSTART) return 0;
    }

  if( !INXRANGE(x) )  return 0;
  if( !INYRANGE(y) )  return 0;

  return 1;
}


