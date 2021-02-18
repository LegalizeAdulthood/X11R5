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
#include <X11/StringDefs.h>
#include "hearts.h"

void show_deal () ;
void order_hand () ;
static void shuffle () ;
static int sdeck[CARDS_PER_DECK] ;

#define N_SHUFFLES 100

void 
deal ()
{
int i, j ;
char text1[20] ;
char text2[20] ;

shuffle () ;

for (i=0;  i <  CARDS_PER_PLAYER;  i++)
    {
    for (j=0;  j <  N_PLAYERS;  j++)
	{
	int card_number ;
	card_number = sdeck[i*N_PLAYERS + j] ;
	game->deck[card_number].dealt_to = j ;
	pvt_history[j].cards_dealt[i] = card_number ;
	}
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    game->player[i].n_chosen = 0 ;
    info->n_points_taken[i] = 0 ;
    info->n_tricks_taken[i] = 0 ;
    order_hand (i) ;
    show_deal (i) ;
    }

/*
 * Clear out the old table
 * Clear out the old discards
 * Initialize the results in the player points and tricks boxes
 */

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_DISCARDS;  j++)
	{
	Arg arg ;
	XtSetArg (arg, XtNlabel, " ") ;
	XtSetValues (game->x.discards[i][j], &arg, 1) ;
	}
    }

sprintf (text1, "Points:%3d", 0) ;
sprintf (text2, "Tricks:%3d", 0) ;
for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_PLAYERS;  j++)
	{
	Arg arg ;
	XtSetArg (arg, XtNlabel, text1) ;
	XtSetValues (game->x.points[i][j], &arg, 1) ;
	XtSetArg (arg, XtNlabel, text2) ;
	XtSetValues (game->x.tricks[i][j], &arg, 1) ;
	}
    }

/*
 * Initialize the per-deal variables
 */

CurrentTrick = 0 ;
info->total_hearts_played = 0 ;
info->n_point_cards_taken = 0 ;
info->total_points_taken = 0 ;
info->points_broken = False ;
info->jack_played = False ;
info->queen_played = False ;
game->conceded = False ;

return ;
}

static void
shuffle ()
{
int i, j ;

#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "\nThe Shuffle\n") ;
#endif
for (i=0;  i < CARDS_PER_DECK;  i++)
    sdeck[i] = i ;

for (j=0;  j < N_SHUFFLES;  j++)
    {
    for (i=0;  i < CARDS_PER_DECK;  i++)
	{
	int n, temp ;
#ifdef NRAND48
	n = nrand48(history->xsubi) % CARDS_PER_DECK ;
#else
	n = rand() % CARDS_PER_DECK ;
#endif
	temp = sdeck[i] ;
	sdeck[i] = sdeck[n] ;
	sdeck[n] = temp ;
	}
    }

#ifndef NO_LOGGING
for (i=0;  i < CARDS_PER_DECK;  i++)
    if (logerr) fprintf (logerr, "%d ", sdeck[i]) ;
if (logerr) fprintf (logerr, "\n\n") ;
#endif

return ;
}

void
clear_hands ()
{
int i, j, k ;
Arg arg ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_SUITS;  j++)
	{
	for (k=0;  k < pvt_history[i].n_in_suit[j];  k++)
	    {
	    XtSetArg (arg, XtNlabel, " ") ;
	    XtSetValues (game->x.hands[i][j][k], &arg, 1) ;
	    }
	}
    }
return ;
}

void
show_deal (i)
    int i ;
{
int j, k ;
Arg arg ;

if (game->player[i].physiology == ComputerPhysiology)
    return ;

for (j=0;  j < N_SUITS;  j++)
    {
    for (k=0;  k < pvt_history[i].n_in_suit[j];  k++)
	{
	char name_string[40] ;
	int card_number = pvt_history[i].suit[j][k] ;
	int card_rank = info->deck[card_number].rank ;
	sprintf (name_string, "%s%d", game->suit[j].character, k) ;
	XtSetArg (arg, XtNlabel, game->suit[i].rank[card_rank].symbol) ;
	XtSetValues (game->x.hands[i][j][k], &arg, 1) ;
	}
    }
return ;
}

void
order_hand (n)
    int n ;
{
int i, j, k ;

/*
 * This is made easy by the fact that we can just look through the
 * deck structure for cards in this particular hand.  Since
 * the cards are arranged in order, we'll just fill the suit arrays
 * as they appear...
 */

for (i=0;  i < N_SUITS;  i++)
    {
    int *current_number_in_suit = &pvt_history[n].n_in_suit[i] ;
    *current_number_in_suit = 0 ;
    for (j=info->cards_per_suit[i]-1;  j >= 0;  j--)
	{
	int card_number = game->suit[i].rank[j].card ;
	if (game->deck[card_number].dealt_to == n)
	    {
	    /*
	     * But we don't want to order in any that have been selected
	     * as discards
	     */
	    for (k=0;  k < game->player[n].n_chosen;  k++)
		{
		if (pvt_history[n].cards_passed[k] == card_number)
		    break ;
		}
	    if (k == game->player[n].n_chosen)
		{
		pvt_history[n].suit[i][*current_number_in_suit] = card_number ;
		(*current_number_in_suit)++ ;
		}
	    }
	}
    }
return ;
}
