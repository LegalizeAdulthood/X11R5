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

void determine_lead () ;
static void play () ;
static void score_trick () ;
static void clean_table () ;
void request_another_hand () ;

void
trick ()
{
int i ;
for (i=0;  i < N_PLAYERS;  i++)
    play () ;
score_trick () ;
}

static void
score_trick ()
{
int i, j ;
char text1[20], text2[20] ;

#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Current_trick : %d\n", CurrentTrick) ;
if (logerr) fprintf (logerr, "Total points taken so far is %d\n",
info->total_points_taken) ;
#endif
if (ThereArePointsInTrick || JackInThisTrick)
    {
    /*
     * See if this is the first trick where points were dropped
     * (Jack doesn't count)
     */
    if (ThereArePointsInTrick && !info->points_broken)
	{
	info->points_broken = True ;
	info->points_broken_trick = snap->n_tricks_played ;
	}
    /*
     * Update Points statistics
     */
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Player %d goes from %d to",
TrickWinner, info->n_points_taken[TrickWinner]) ;
#endif
    info->n_points_taken[TrickWinner] += HeartsInThisTrick ;
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, " %d\n", info->n_points_taken[TrickWinner]) ;
#endif
    if (QueenInThisTrick)
	{
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Player %d goes from %d to",
TrickWinner, info->n_points_taken[TrickWinner]) ;
#endif
	info->n_points_taken[TrickWinner] += QUEEN_POINTS ;
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, " %d\n", info->n_points_taken[TrickWinner]) ;
#endif
	}
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "\n- - - - - - - - - - -\n\n") ;
#endif

    /*
     * Now, show the points taken results in the player boxes
     */

    if (history->jack_counts && info->jack_played && (JackWinner==TrickWinner))
	{
	sprintf (text1, "Points:%3d - %d",
		info->n_points_taken[TrickWinner], JACK_POINTS) ;
	}
    else
	{
	sprintf (text1, "Points:%3d", info->n_points_taken[TrickWinner]) ;
	}

    for (i=0;  i < N_PLAYERS;  i++)
	{
	if (game->player[game->ccw[i]].physiology == HumanPhysiology)
	    {
	    Arg arg ;
	    int dir_index = TrickWinner + i ;
	    if (dir_index >= N_PLAYERS)
		dir_index -= N_PLAYERS ;
	    XtSetArg (arg, XtNlabel, text1) ;
	    XtSetValues (game->x.points[game->ccw[i]][dir_index], &arg, 1) ;
	    }
	}
    }


/*
 * Now, show the tricks taken results in the player boxes
 */

info->n_tricks_taken[TrickWinner]++ ;
sprintf (text2, "Tricks:%3d", info->n_tricks_taken[TrickWinner]) ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[game->ccw[i]].physiology == HumanPhysiology)
	{
	Arg arg ;
	int dir_index = TrickWinner + i ;
	if (dir_index >= N_PLAYERS)
	    dir_index -= N_PLAYERS ;
	XtSetArg (arg, XtNlabel, text2) ;
	XtSetValues (game->x.tricks[game->ccw[i]][dir_index], &arg, 1) ;
	}
    }

snap->n_tricks_played++ ;

/*
 * Make sure everybody gets a few seconds to see the results
 */

for (j=0;  j < game->sleep_seconds;  j++)
    {
    for (i=0;  i < N_PLAYERS;  i++)
	if (game->player[i].physiology == HumanPhysiology)
	    XcuWlmEvent (game->x.wlm_id[i]) ;
    sleep (1) ;
    }

return ;
}

void
determine_lead ()
{
int i, j ;
Player *player ;

snap->n_played_to_trick = 0 ;
info->n_hearts_played[CurrentTrick] = 0 ;

if (CurrentTrick == 0)
    {
    /*
     * Search for the two of clubs and set that player to the leader
     */
    for (i=0;  i < N_SUITS;  i++)
	{
	if (strcmp (game->suit[i].name, "Clubs") != 0)
	    continue ;
	for (j=0;  j < CARDS_PER_SUIT;  j++)
	    {
	    if (strcmp (game->suit[i].rank[j].symbol, "2") == 0)
		{
		int card_number = game->suit[i].rank[j].card ;
		Leader = game->deck[card_number].dealt_to ;
		return ;
		}
	    }
	}
    }
else
    {
    /*
     * set leader to the winner of the last trick
     */
    Leader = LastTrickWinner ;
    }

/*
 * Do a check here to see if this player can claim the remaining tricks
 * This is true if the worst card in each suit is higher than the highest
 * cards in the suits of the opponents.
 */

player = &game->player[Leader] ;

for (i=0;  i < N_SUITS;  i++)
    {
    int worst_card, worst_card_rank ;
    if (pvt_history[Leader].n_in_suit[i] == 0)
	continue ;
    worst_card = pvt_history[Leader].suit[i][pvt_history[Leader].n_in_suit[i]-1] ;
    worst_card_rank = info->deck[worst_card].rank ;
    for (j=0;  j < N_PLAYERS;  j++)
	{
	int best_card, best_card_rank ;
	Player *player = &game->player[j] ;
	if (j == Leader)
	    continue ;
	if (pvt_history[j].n_in_suit[i] == 0)
	    continue ;
	best_card = pvt_history[j].suit[i][0] ;
	best_card_rank = info->deck[best_card].rank ;
	if (best_card_rank > worst_card_rank)
	    break ;
	}
    if (j < N_PLAYERS)
	break ;
    }

if (i == N_SUITS)
    game->conceded = True ;

return ;
}

static void
play ()
{
int i ;
int player_index = ActivePlayer ;
Player *player = &game->player[player_index] ;
/*
 * if leader
 *    if first_trick
 *      call for lead of 2 of clubs,
 *    else
 *	prompt for lead
 * else
 *     prompt for play
 */
if (snap->n_played_to_trick == 0)
    {
    /* Time for a lead */
    if (CurrentTrick == 0)
	{
	/* Since it's the first lead, we'll request lead of the 2 of clubs */
	proc_message ("Please lead the two of clubs.", player_index) ;
	}
    else
	{
	proc_message ("It's your lead.", player_index) ;
	}
    }
else
    proc_message ("It's your play.", player_index) ;

player->mode = PlayMode ;
if (player->physiology == HumanPhysiology)
    {
    for (
	player->legal_play = False ;
	!player->legal_play ;
	wait_for_player_events ()
	)
	;
    }
else
    (*game->player[player_index].play_program)
	(&pvt_history[player_index], player_index) ;

snap->n_played_to_trick++ ;
player->mode = NullMode ;
proc_message (" ", player_index) ;
if (player->error_mode == ClearOnLegalPlay)
    {
    err_message (" ", player_index) ;
    player->error_mode = NullClear ;
    }

return ;
}

void
play_selected (data)
    long *data ;
{
int i, j ;
int player_index = (int) data[0] ;
int suit_index = (int) data[1] ;
int rank_index = (int) data[2] ;
char name_string[20] ;
Player *player = &game->player[player_index] ;
int card_number = pvt_history[player_index].suit[suit_index][rank_index] ;
int deck_suit_index = info->deck[card_number].suit ;
int deck_rank_index = info->deck[card_number].rank ;
int dir_index ;
Arg arg ;

/*
 * First, check the legality of the play
 */

if (rank_index >= pvt_history[player_index].n_in_suit[suit_index])
    {
    err_message ("Fumble fingers!", player_index) ;
    player->error_mode = ClearOnLegalPlay ;
    return ;
    }

if (snap->n_played_to_trick == 0)
    {
    /*
     * It's a lead, so the following restrictions apply:
     *	1) If it is the first lead, it must be the 2 of clubs
     *  2) O.w., it cannot be a point card if points have not broken
     *     * Unless the player has nothing but point cards
     */
    if (CurrentTrick == 0)
	{
	if (deck_suit_index != SUIT_CLUBS || deck_rank_index != RANK_TWO)
	    {
	    err_message("Read my lips! The deuce of Clubs!", Leader);
	    player->error_mode = ClearOnLegalPlay ;
	    return ;
	    }
	}
    else
	{
	if (!info->points_broken)
	    {
	    if (deck_suit_index == SUIT_HEARTS ||
	       (deck_suit_index == SUIT_SPADES &&
		deck_rank_index == RANK_QUEEN))
		{
		/*
		 * Attempt to lead a point with points not broken.
		 * He must have only point cards for this to be legal
		 */
		for (i=0;  i < N_SUITS;  i++)
		    {
		    for (j=0;  j < pvt_history[player_index].n_in_suit[i];  j++)
			{
			int held_card = pvt_history[player_index].suit[i][j] ;
			if (info->deck[held_card].suit == SUIT_HEARTS)
			    continue ;
			if (info->deck[held_card].suit == SUIT_SPADES &&
			    info->deck[held_card].rank == RANK_QUEEN)
			    continue ;
			err_message ("You are not allowed to lead a point yet!",
				 player_index) ;
			player->error_mode = ClearOnLegalPlay ;
			return ;
			}
		    }
		}
	    }
	}
    /*
     * If here, it was a legal lead
     * Clear the table of the last trick
     */
    TrickWinner = player_index ;
    info->trick_taking_rank = deck_rank_index ;

    clean_table () ;
    }
else
    {
    /*
     * It's not a lead, so the following restrictions apply:
     *	1) Must follow suit
     *  2) If sloughing, there may be a rule that forbids sloughing
     *     points on the first trick...
     */
    if (deck_suit_index != SuitLead)
	{
	if (pvt_history[player_index].n_in_suit[SuitLead] > 0)
	    {
	    err_message ("That's a renege! Try again.", player_index) ;
	    player->error_mode = ClearOnLegalPlay ;
	    return ;
	    }
	if (
	   CurrentTrick == 0 &&
	   !history->points_on_first_trick &&
	    (
	    deck_suit_index == SUIT_HEARTS ||
	    (deck_suit_index == SUIT_SPADES && deck_rank_index == RANK_QUEEN)
	    )
	   )
	    {
	    err_message ("No point dumping on first trick! Try again.",
		     player_index) ;
	    player->error_mode = ClearOnLegalPlay ;
	    return ;
	    }
	/*
	 * If here, it was a legal play, even though suit was not followed
	 */
	}
    else
	{
	/* check to see if this card may take the trick */
	/* lower rank_indices are the higher ranking cards :-( */
	if (deck_rank_index > info->trick_taking_rank)
	    {
	    info->trick_taking_rank = deck_rank_index ;
	    TrickWinner = player_index ;
	    }
	}
    }

history->trick[CurrentTrick].card[snap->n_played_to_trick] = card_number ;
player->legal_play = True ;

if (deck_suit_index == SUIT_HEARTS)
    {
    info->n_hearts_played[CurrentTrick]++ ;
    info->total_points_taken += HEART_POINTS ;
    info->n_point_cards_taken++  ;
    }

if (deck_suit_index == SUIT_SPADES && deck_rank_index == RANK_QUEEN)
    {
    info->queen_played = True ;
    info->queen_played_trick = CurrentTrick ;
    info->total_points_taken += QUEEN_POINTS ;
    info->n_point_cards_taken++ ;
    }

if (history->jack_counts &&
    deck_suit_index == SUIT_DIAMONDS &&
    deck_rank_index == RANK_JACK)
    {
    info->jack_played = True ;
    info->jack_played_trick = CurrentTrick ;
    info->n_point_cards_taken++ ;
    }

/*
 * Then, show the play on the table
 */

/************
 I wish I knew why this code doesn't work.
 I get "Cannot access per-display info"
for (i=0;  i < N_PLAYERS;  i++)
    {
    Arg args[2] ;
    int dir_index = player_index + i ;
    if (dir_index >= N_PLAYERS)
	dir_index -= N_PLAYERS ;
    XtSetArg (args[0], XtNforeground, game->suit[deck_suit_index].color[game->ccw[i]]) ;
    XtSetArg (args[1], XtNlabel, card_string) ;
    XtSetValues (game->x.plays[game->ccw[i]][dir_index], args, 2) ;
    }
****************/
{
int dir_index = player_index ;
if (game->player[dir_index].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "south_card",
	XtNforeground, game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "south_card",
	XtNlabel, game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.south_bitmap[dir_index],
		       game->x.south_suit[dir_index][deck_suit_index]);
    }
dir_index++ ;
if (dir_index == N_PLAYERS)
    dir_index = 0 ;
if (game->player[dir_index].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "east_card",
	XtNforeground, game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "east_card",
	XtNlabel, game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.east_bitmap[dir_index],
		       game->x.east_suit[dir_index][deck_suit_index]);
    }
dir_index++ ;
if (dir_index == N_PLAYERS)
    dir_index = 0 ;
if (game->player[dir_index].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "north_card",
	XtNforeground, game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "north_card",
	XtNlabel, game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.north_bitmap[dir_index],
		       game->x.north_suit[dir_index][deck_suit_index]);
    }
dir_index++ ;
if (dir_index == N_PLAYERS)
    dir_index = 0 ;
if (game->player[dir_index].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "west_card",
	XtNforeground, game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[dir_index], "XcuLabel", "west_card",
	XtNlabel, game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.west_bitmap[dir_index],
		       game->x.west_suit[dir_index][deck_suit_index]);
    }
}

/*
 * Now, remove the card from the hand
 * Collapse the labels left in the row and blank out the last one
 */

if (game->player[player_index].physiology == HumanPhysiology)
    {
    XtSetArg (arg, XtNlabel, " ") ;
    XtSetValues (game->x.hands[player_index][suit_index]
	      [pvt_history[player_index].n_in_suit[suit_index]-1], &arg, 1) ;

    }

pvt_history[player_index].n_in_suit[suit_index]-- ;
game->deck[card_number].dealt_to = N_PLAYERS ;

order_hand (player_index) ;
show_deal (player_index) ;
player->mode = NullMode ;

return ;
}

void
score_hand ()
{
int i, j ;
char text0[20] ;
char text1[20] ;
char text2[20] ;
char text[20] ;

/*
 * First check to see if anybody claimed the rest of the tricks
 */

if (game->conceded)
    {
    for (i=CurrentTrick;  i < N_TRICKS;  i++)
	info->trick_winner[i] = Leader ;

#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Player %d goes from %d to",
Leader, info->n_points_taken[Leader]) ;
#endif
    info->n_points_taken[Leader] += (SHOOT_POINTS - info->total_points_taken) ;
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, " %d\n", info->n_points_taken[Leader]) ;
#endif

    /* Just for the heck of it, put all the goodies in this trick */
    if (!info->jack_played)
	{
	info->jack_played = True ;
	info->jack_played_trick = CurrentTrick ;
	}
    if (!info->queen_played)
	{
	info->queen_played = True ;
	info->queen_played_trick = CurrentTrick ;
	}

    if (history->jack_counts && info->jack_played && (JackWinner==TrickWinner))
	{
	sprintf (text1, "Points:%3d - %d",
		info->n_points_taken[TrickWinner], JACK_POINTS) ;
	}
    else
	{
	sprintf (text1, "Points:%3d", info->n_points_taken[TrickWinner]) ;
	}

    for (i=0;  i < N_PLAYERS;  i++)
	{
	if (game->player[game->ccw[i]].physiology == HumanPhysiology)
	    {
	    Arg arg ;
	    int dir_index = TrickWinner + i ;
	    if (dir_index >= N_PLAYERS)
		dir_index -= N_PLAYERS ;
	    XtSetArg (arg, XtNlabel, text1) ;
	    XtSetValues (game->x.points[game->ccw[i]][dir_index], &arg, 1) ;
	    }
	}
    }

/*
 * Next check to see if anybody shot the moon
 */

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (info->n_points_taken[i] == SHOOT_POINTS)
	break ;
    }

if (i < N_PLAYERS)
    {
    /*  A Shoot! */
    char text[100] ;

    sprintf (text, "%s shot the moon!", game->player[i].name_string) ;

    if (game->player[i].physiology == HumanPhysiology)
	{
	XcuDeckRaiseWidget (game->x.table_deck[i], game->x.moon_decision[i]) ;

	for (
	    game->moon_decision = NullMoonDecision;
	    game->moon_decision == NullMoonDecision;
	    wait_for_player_events ()
	    /*
	    XcuWlmRequest (game->x.wlm_id[i])
	    */
	    ) ;
	}
    else
	game->moon_decision = RaiseOthers ;

    for (j=0;  j < N_PLAYERS;  j++)
	{
	if (game->moon_decision == RaiseOthers)
	    info->n_points_taken[j] = SHOOT_POINTS ;
	else
	    info->n_points_taken[j] = 0 ;
	/* Announce it! */
	proc_message (text, j) ;
	}
    if (game->moon_decision == RaiseOthers)
	info->n_points_taken[i] = 0 ;
    else
	info->n_points_taken[i] = -SHOOT_POINTS ;
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Adding %d to player %d's score of %d\n",
info->n_points_taken[i], i, game->player[i].score) ;
#endif
    game->player[i].score += info->n_points_taken[i] ;
    }

if (history->jack_counts)
    {
#ifndef NO_LOGGING
if (logerr) fprintf (logerr, "Subtracting %d from player %d's score of %d\n",
JACK_POINTS, i, game->player[JackWinner].score) ;
#endif
    game->player[JackWinner].score -= JACK_POINTS ;
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    Arg arg ;
    sprintf (text, "Score:%4d", game->player[i].score) ;
    XtSetArg (arg, XtNlabel, text) ;
    for (j=0;  j < N_PLAYERS;  j++)
	{
	if (game->player[j].physiology == HumanPhysiology)
	    {
	    int dir_index = game->ccw[j] + i ;
	    if (dir_index >= N_PLAYERS)
		dir_index -= N_PLAYERS ;
	    XtSetValues (game->x.score[j][dir_index], &arg, 1) ;
	    }
	}
    }

clean_table () ;

return ;
}

static void
clean_table ()
{
int i, j ;
for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_PLAYERS;  j++)
	{
	char name_string[20] ;
	sprintf (name_string, "%s_card", game->seat[j]) ;
	XcuWlmSetValue(game->x.wlm_id[i], "XcuLabel", name_string,
					      XtNlabel," ");
	}
    XcuDeckRaiseWidget(game->x.south_bitmap[i], game->x.south_suit[i][N_SUITS]);
    XcuDeckRaiseWidget(game->x.east_bitmap[i], game->x.east_suit[i][N_SUITS]);
    XcuDeckRaiseWidget(game->x.west_bitmap[i], game->x.west_suit[i][N_SUITS]);
    XcuDeckRaiseWidget(game->x.north_bitmap[i], game->x.north_suit[i][N_SUITS]);
    }
return ;
}

Boolean
hand_over ()
{
int i ;

if (CurrentTrick == CARDS_PER_PLAYER)
    {
    for (i=0;  i < N_PLAYERS;  i++)
	{
	game->player[i].mode = NullMode ;
	proc_message ("Hand is over. No tricks left.", i) ;
	}
    return True ;
    }
else
if (game->conceded)
    {
    for (i=0;  i < N_PLAYERS;  i++)
	{
	game->player[i].mode = NullMode ;
	proc_message ("Hand is over. Remainder won.", i) ;
	}
    clear_hands (history, pvt_history, snap, info) ;
    return True ;
    }
else
if (info->n_point_cards_taken == info->n_point_cards)
    {
    for (i=0;  i < N_PLAYERS;  i++)
	{
	game->player[i].mode = NullMode ;
	proc_message ("Hand is over. No points left.", i) ;
	}
    clear_hands () ;
    return True ;
    }

return False ;
}

void
moon_decision (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int decision = (int) data[0] ;

game->moon_decision = decision ;
return ;
}

void
hand_decision (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int player_index = (int) data[0] ;
int decision = (int) data[1] ;

game->player[player_index].hand_decision = decision ;
XcuDeckRaiseWidget (game->x.table_deck[player_index],
		    game->x.play_tbl[player_index]) ;

return ;
}

void 
request_another_hand ()
{
int i ;
/*
 * Ask everybody if they want to play another hand
 */

for (i=0;  i < N_PLAYERS;  i++)
    {
    game->player[i].hand_decision = NullHandDecision ;
    if (game->player[i].physiology == HumanPhysiology)
	XcuDeckRaiseWidget (game->x.table_deck[i], game->x.hand_decision[i]) ;
    }

/*
 * Wait for everybody to vote
 */

game->hand_decision = AnotherHand ;

for (;;)
    {
    wait_for_player_events () ;
    for (i=0;  i < N_PLAYERS;  i++)
	{
	if (game->player[i].physiology == ComputerPhysiology)
	    continue ;
	if (game->player[i].hand_decision == NullHandDecision)
	    break ;
	if (game->player[i].hand_decision == NoMore)
	    break ;
	}
    if (i == N_PLAYERS)
	break ;
    if (game->player[i].hand_decision == NoMore)
	{
	game->hand_decision = NoMore ;
	break ;
	}
    }
return ;
}

int
active_player ()
{
int position = Leader + snap->n_played_to_trick ;
if (position >= N_PLAYERS)
    position -= N_PLAYERS ;
return position ;
}
