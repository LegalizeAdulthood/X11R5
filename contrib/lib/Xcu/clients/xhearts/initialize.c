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
#include <X11/Xos.h>
#include "hearts.h"

static String seat_names[] = { "south", "west", "north", "east" } ;
static String suit_names[] = { "Spades", "Hearts", "Diamonds", "Clubs" } ;
static String suit_symbols[] =
	{ "spade.xbm", "heart.xbm", "diamond.xbm", "club.xbm" } ;
static String suit_characters[] = { "S", "H", "D", "C" } ;
static String suit_ranks[] = { "2", "3", "4", "5", "6", "7", "8",
			       "9", "T", "J", "Q",  "K", "A" } ;

void 
initialize (argc, argv)
    int argc ;
    char **argv ;
{
int iseed ;
int i, j, k ;
int card_number ;

init_x_phase_1 (argc, argv) ;

snap->current_pass = InitialPass ;
info->n_point_cards = HEART_POINT_CARDS + QUEEN_POINT_CARDS ;

#ifdef NRAND48
if (game->seed)
    {
    sscanf (game->seed, "%u %u %u",
	    &history->xsubi[0], &history->xsubi[1], &history->xsubi[2]) ;
    }
else
    {
    struct timeval tp_, *tp = &tp_ ;
    struct timezone tzp_, *tzp = &tzp_ ;
    gettimeofday (tp, tzp) ;
    history->xsubi[0] = (short) ((int)((tp->tv_sec & 0x0000ffff))) ;
    history->xsubi[1] = (short) ((int)((tp->tv_sec & 0xffff0000)>>16)) ;
    history->xsubi[2] = (short) ((int)(tp->tv_usec & 0x0000ffff)) ;
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Seed: %u %u %u\n",
history->xsubi[0], history->xsubi[1], history->xsubi[2]) ;
#endif
    }
#else
if (game->seed)
    {
    iseed = atoi(game->seed) ;
    }
else
    {
    struct timeval tp_, *tp = &tp_ ;
    struct timezone tzp_, *tzp = &tzp_ ;
    gettimeofday (tp, tzp) ;
    iseed = tp->tv_sec ;
    }
srand (iseed) ;
if (logerr) fprintf (logerr, "Seed: %d\n", iseed) ;
fprintf (stderr, "Seed: %d\n", iseed) ;
#endif NRAND48

if (history->jack_counts)
    {
    info->n_point_cards += JACK_POINT_CARDS ;
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    int ccw = (N_PLAYERS - i) % N_PLAYERS ; /* 0, 3, 2, 1 */
    int cw =  i ;			    /* 0, 1, 2, 3 */
    game->ccw[i] = ccw ;
    game->cw[i] = cw ;
    game->player[i].score = 0 ;
    game->player[i].mode = NullMode ;
    game->player[i].error_mode = NullClear ;
    game->seat[i] = XtNewString (seat_names[i]) ;
    }

for (card_number=0, i=0;  i < N_SUITS;  i++)
    {
    game->suit[i].symbol = XtNewString (suit_symbols[i]) ; 
    game->suit[i].name = XtNewString (suit_names[i]) ; 
    game->suit[i].character = XtNewString (suit_characters[i]) ; 
    info->cards_per_suit[i] = CARDS_PER_SUIT ;
    game->suit[i].rank = (Rank *)XtMalloc(info->cards_per_suit[i]*sizeof(Rank));
    for (j=0;  j < N_PLAYERS;  j++)
	{
	for (k=0;  k < N_SUITS;  k++)
	    pvt_history[j].suit[k] = (int *) XtMalloc (info->cards_per_suit[i] *
							sizeof (int)) ;
	}
    for (j=0;  j < info->cards_per_suit[i];  j++, card_number++)
	{
	game->suit[i].rank[j].symbol = XtNewString (suit_ranks[j]) ; 
	info->deck[card_number].suit = i ;
	info->deck[card_number].rank = j ;
	game->suit[i].rank[j].card = card_number ;
	}
    }

init_x_phase_2 () ;

return ;
}

