/*
 * Copyright 1991 Cornell University
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Cornell U. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Cornell U. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * CORNELL UNIVERSITY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORNELL UNIVERSITY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Gene W. Dykes, Program of Computer Graphics
 *          580 Theory Center, Cornell University, Ithaca, NY 14853
 *          (607) 255-6713   gwd@graphics.cornell.edu
 */


#include <stdio.h>
#include <X11/Intrinsic.h>
#include "hearts.h"

void show_hand () ;
void show_card () ;
void show_card2 () ;
extern void card_picked () ;

#define NA 0 /* Not Applicable */

static short value[N_SUITS][CARDS_PER_SUIT][CARDS_PER_SUIT] =
    {
     /* Spades */
     {
      { NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  2 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  3 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  4 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  5 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  6 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  7 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  8 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  9 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /* 10 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /* Jack */
     ,{ NA,10,10, 8, 3, 1, 0, 0, 0, 0, 0, 0, 0 } /* Queen */
     ,{ NA,10, 9, 7, 4, 0, 0, 0, 0, 0, 0, 0, 0 } /* King */
     ,{ NA,10, 9, 7, 4, 0, 0, 0, 0, 0, 0, 0, 0 } /* Ace */
     }
     /* Hearts */
    ,{
      { NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  2 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  3 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  4 */
     ,{ NA, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  5 */
     ,{ NA,10, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  6 */
     ,{ NA,10, 7, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0 } /*  7 */
     ,{ NA,10,10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5 } /*  8 */
     ,{ NA,10,10, 8, 7, 6, 5, 4, 3, 2, 1, 0, 0 } /*  9 */
     ,{ NA,10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0 } /* 10 */
     ,{ NA, 9, 7, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0 } /* Jack */
     ,{ NA, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0 } /* Queen */
     ,{ NA, 7, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0 } /* King */
     ,{ NA, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /* Ace */
     }
     /* Diamonds */
    ,{
      { NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  2 */
     ,{ NA, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  3 */
     ,{ NA, 4, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  4 */
     ,{ NA, 6, 5, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  5 */
     ,{ NA, 8, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  6 */
     ,{ NA, 9, 7, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0 } /*  7 */
     ,{ NA,10, 7, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0 } /*  8 */
     ,{ NA,10, 8, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0 } /*  9 */
     ,{ NA,10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0 } /* 10 */
     ,{ NA,10, 9, 7, 5, 3, 1, 0, 0, 0, 0, 0, 0 } /* Jack */
     ,{ NA,10, 9, 8, 6, 3, 2, 0, 0, 0, 0, 0, 0 } /* Queen */
     ,{ NA,10,10, 9, 7, 4, 1, 0, 0, 0, 0, 0, 0 } /* King */
     ,{ NA,10,10, 9, 7, 4, 1, 0, 0, 0, 0, 0, 0 } /* Ace */
     }
     /* Clubs */
    ,{
      { NA, 9, 7, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0 } /*  2 */
     ,{ NA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  3 */
     ,{ NA, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  4 */
     ,{ NA, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  5 */
     ,{ NA, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 } /*  6 */
     ,{ NA, 7, 5, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0 } /*  7 */
     ,{ NA, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0 } /*  8 */
     ,{ NA, 9, 7, 6, 3, 1, 0, 0, 0, 0, 0, 0, 0 } /*  9 */
     ,{ NA,10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0 } /* 10 */
     ,{ NA,10, 9, 7, 5, 3, 0, 0, 0, 0, 0, 0, 0 } /* Jack */
     ,{ NA,10, 9, 8, 6, 4, 1, 0, 0, 0, 0, 0, 0 } /* Queen */
     ,{ NA,10,10, 9, 7, 5, 2, 0, 0, 0, 0, 0, 0 } /* King */
     ,{ NA,10,10, 9, 7, 6, 3, 0, 0, 0, 0, 0, 0 } /* Ace */
     }
    } ;

void
discard_program (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
long data[3] ;
int discard_val[N_DISCARDS] ;
int discard_suit[N_DISCARDS] ;
int discard_rank[N_DISCARDS] ;
int discard_card[N_DISCARDS] ;
int real_rank[N_DISCARDS] ;
int n_in_suit[N_SUITS] ;
int d, i, j ;
int defensive_heart_thrown = False ;

data[0] = player_index ;

show_hand (player_index) ;

for (i=0;  i < N_SUITS;  i++)
    {
    /* Need to decide based on original number in suit */
    n_in_suit[i] = ph->n_in_suit[i] ;
    }

for (d=0;  d < N_DISCARDS;  d++)
  {
  discard_val[d] = -1 ;
  for (i=0;  i < CARDS_PER_PLAYER;  i++)
    {
    int card = ph->cards_dealt[i] ;
    int suit = info->deck[card].suit ;
    int rank = info->deck[card].rank ;
    int val = value[suit][rank][n_in_suit[suit]] ;
#ifndef NO_LOGGING
show_card2 (suit, rank) ;
if (logerr)
fprintf (logerr, " (%d) ", val) ;
#endif
    if (val > discard_val[d] ||
	(val == discard_val[d] && rank > real_rank[d]))
	{
	for (j=0;  j < d;  j++)
	    {
	    if (card == discard_card[j])
		break ;
	    }
	if (j < d)
	    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No because already discarded\n") ;
#endif
	    continue ;
	    }
	if (defensive_heart_thrown && suit == SUIT_HEARTS &&
	    rank <= RANK_TEN && rank >= RANK_SIX &&
	    ph->n_in_suit[suit] > 2)
	    {
/* TODO: refine this */
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No because defensive heart already thrown\n") ;
#endif
	    continue ;
	    }
        if (!defensive_heart_thrown &&
	    suit == SUIT_HEARTS &&
	    info->deck[card].rank <= RANK_TEN &&
	    info->deck[card].rank >= RANK_SIX)
	    {
	    int highest_heart_card =
	      ph->suit[SUIT_HEARTS][0] ;
	    int heart_rank = info->deck[highest_heart_card].rank ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Checking whether to throw a defensive heart (%d, %d, %d)\n",
n_in_suit[SUIT_HEARTS], highest_heart_card, heart_rank) ;
#endif
	    if (
	          ( n_in_suit[SUIT_HEARTS] >= 2 && heart_rank == RANK_ACE) ||
		  ( n_in_suit[SUIT_HEARTS] >= 3 && heart_rank == RANK_KING) ||
		  ( n_in_suit[SUIT_HEARTS] >= 4 && heart_rank == RANK_QUEEN) ||
		  ( n_in_suit[SUIT_HEARTS] >= 5 && heart_rank == RANK_JACK)
	       )
		  {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No need to throw a defensive heart\n") ;
#endif
		  continue ;
		  }
	     else
		  {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Defensive heart may be thrown\n") ;
#endif
		  }
	     }
	discard_val[d] = val ;
	discard_suit[d] = suit ;
	discard_card[d] = card ;
	real_rank[d] = rank ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Best yet\n") ;
#endif
	/* TODO : need to make it easier to find the location in the hand */
	for (j=0;  j < ph->n_in_suit[suit];  j++)
	    {
	    if (ph->suit[suit][j] == card)
		{
		discard_rank[d] = j ;
		break ;
		}
	    }
	}
#ifndef NO_LOGGING
else
if (logerr)
fprintf (logerr, "No because value lower\n") ;
#endif
    }
  data[1] = discard_suit[d] ;
  data[2] = discard_rank[d] ;
  card_picked (NULL, data, NULL) ;
    if (discard_suit[d] == SUIT_HEARTS &&
	info->deck[discard_card[d]].rank <= RANK_TEN &&
	info->deck[discard_card[d]].rank >= RANK_SIX)
      {
      defensive_heart_thrown = True ;
      }
  }

return ;
}

void
show_hand (pi)
    int pi ;
{
int i, j, n=0 ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "\n\n") ;
for (i=0;  i < N_SUITS;  i++)
    {
    if (logerr)
    fprintf (logerr, "%s : ", game->suit[i].character) ;
    for (j=0;  j < pvt_history[pi].n_in_suit[i]; j++)
	{
	int card = pvt_history[pi].suit[i][j] ;
	int suit = info->deck[card].suit ;
	int rank = info->deck[card].rank ;
	int val = value[suit][rank][pvt_history[pi].n_in_suit[suit]] ;
	if (logerr)
	fprintf (logerr, "%3s(%2d) ", game->suit[suit].rank[rank].symbol, val) ;
	n++ ;
	}
    if (logerr)
    fprintf (logerr, "\n") ;
    }
if (logerr)
fprintf (logerr, "\n") ;
if (n + CurrentTrick != 13)
if (logerr)
fprintf (logerr, "*** Number of Cards is Wrong! *** (%d,%d)\n",
n, CurrentTrick) ;
#endif
}

void
show_card (pi, i, j)
    int pi ;
    long i, j ;
{
int card = pvt_history[pi].suit[i][j] ;
int suit = info->deck[card].suit ;
int rank = info->deck[card].rank ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, " %3s%s\n",
game->suit[suit].rank[rank].symbol,
game->suit[suit].character) ;
#endif
}

void
show_card2 (suit, rank)
    int suit, rank ;
{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, " %3s%s",
game->suit[suit].rank[rank].symbol,
game->suit[suit].character) ;
#endif
}

/* TODO: If I am going to hold the queen, then priority for short-suiting */
/* TODO: Whether or not to throw two of clubs depends on remaining clubs */
/* TODO: Whether or not to throw the queen from a four card suit depends
	 on the likelihood of being short-suited */
/* TODO: Whether or not to throw the queen depends highly on which direction
	 we are passing. */

