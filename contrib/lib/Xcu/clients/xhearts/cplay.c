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

typedef enum
    {
     Shooting
    ,Defending
    ,Minimizing
    } ShootMode ;

typedef enum
    {
     Following
    ,Sloughing
    } FollowMode ;

typedef enum
    {
     InOpponentHand
    ,InMyHand
    ,Played
    } CardStatus ;

typedef enum
    {
     LowestCard,
     HighestCard,
     SecondHighestCard,
     HighestLosingCard,
     LowestWinningCard
     } Choice ;

typedef struct
    {
    ShootMode	shoot ;
    FollowMode	follow ;
    CardStatus	card_status[N_SUITS][CARDS_PER_SUIT] ;
    Boolean	i_have_only_points ;
    Boolean	i_have_queen ;
    int		suit_leads[N_TRICKS] ;
    int		lowest_remaining[N_SUITS] ;
    int		lowest_remaining_in_other_hands[N_SUITS] ;
    int		highest_remaining_in_other_hands[N_SUITS] ;
    int		number_remaining_in_other_hands[N_SUITS] ;
    } PlayStatus ;

static PlayStatus _status[4], *status ;

#define Card(i,j)	(ph->suit[i][j])
#define Rank(i,j)	(info->deck[Card(i,j)].rank)

#define IndexOfLowestInSuit(i)	(ph->n_in_suit[i]-1)
#define IndexOfHighestInSuit	(0)
#define IndexOfSecondHighestInSuit	(1)
#define LowestInSuit(i)		(Rank(i, IndexOfLowestInSuit(i)))

#define SuitOfTrick(i,j)	(info->deck[history->trick[i].card[j]].suit)
#define TrickPlayer(i,j)	(history->trick[i].leader + j >= N_PLAYERS ? \
				 history->trick[i].leader + j - N_PLAYERS : \
				 history->trick[i].leader + j)

long data[3] ;
static void play_this_card () ;
int IndexOfChoice () ;

void
play_program (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
int discard_val[N_DISCARDS] ;
int discard_suit[N_DISCARDS] ;
int discard_rank[N_DISCARDS] ;
int discard_card[N_DISCARDS] ;
int real_rank[N_DISCARDS] ;
int n_in_suit[N_SUITS] ;
int d, i, j, k ;
int defensive_heart_thrown = False ;
static void find_the_initial_queen () ;
static void find_the_current_queen () ;
static void determine_initial_defense_potential () ;
static void determine_current_defense_potential () ;
static void find_trick_stats () ;

data[0] = player_index ;
status = &_status[player_index] ;

show_hand (player_index) ;

/*
 * Initialize a few things before playing to the first trick
 */

if (CurrentTrick == 0)
    {
    find_the_initial_queen (ph, player_index) ;
    determine_initial_defense_potential (ph, player_index) ;

    /*
     * Check for first lead of hand ( must lead 2 of clubs )
     */

    if (snap->n_played_to_trick == 0)
	{
	data[1] = SUIT_CLUBS ;
	data[2] = ph->n_in_suit[SUIT_CLUBS] - 1 ;
	card_picked (NULL, data, NULL) ;
	return ;
	}
    }

/*
 * Find some data before playing to any trick
 */

find_trick_stats (ph, player_index) ;
find_the_current_queen (ph, player_index) ;
determine_current_defense_potential (ph, player_index) ;

/*
 * If we are leading
 * (Need to check for points broken.  If not, we cannot lead a point unless
 *  that is all that is left.)
 */

if (snap->n_played_to_trick == 0)
    {
    if (status->shoot == Shooting)
	{
	}
    else
    if (status->shoot == Defending)
	{
	status->shoot = Minimizing ;
	}

    if (status->shoot == Minimizing)
	{
	int spade_differential ;
	/* Good leads are :
	 *	queen of spades if only one high spade outstanding
	 *	jack of spades or lower
	 *	highest diamond if suit not lead yet
	 *      remaining diamond of a doubleton if suit lead once
	 *      remaining club of a doubleton if suit lead once
	 *	lowest remaining card of any suit
	 *	low card if likely someone will play higher
	 */
	/* Bad leads are :
	 *	The Queen of spades
	 *	Ace or King of spades if queen still out
	 *	any spade if short in spades and hold Q, K, or A
	 *	High cards in oft-lead suits
	 */
	/* If no good leads, choose from non-bad leads :
	 *	lowest club unless guaranteed to lose
	 *	lowest diamond unless guaranteed to lose
	 *	lowest heart unless guaranteed to lose
	 *	lowest spade
	 */
	/*
	 * If still nothing selected, I'm hosed so choose :
	 *	lowest club, diamond, heart, spade
	 */

	/**
	 ** Looking For a GOOD Lead
	 **/

if (logerr)
fprintf (logerr, "%d == 1 && %d && %d > %d && (%d || %d)\n",
status->number_remaining_in_other_hands[SUIT_SPADES],
status->i_have_queen,
status->highest_remaining_in_other_hands[SUIT_SPADES],RANK_QUEEN,
info->points_broken, status->i_have_only_points) ;
fflush (logerr) ;
	if (
            (
	    status->i_have_queen &&
	    status->number_remaining_in_other_hands[SUIT_SPADES] == 1 &&
	    status->highest_remaining_in_other_hands[SUIT_SPADES] > RANK_QUEEN
	    &&
	    (info->points_broken || status->i_have_only_points)
	    )
	   )
	    {
	    int j ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing a guaranteed spade queen\n") ;
#endif
	    for (j=0;  j < ph->n_in_suit[SUIT_SPADES];  j++)
		{
		if (Rank(SUIT_SPADES,j) == RANK_QUEEN)
		    {
		    play_this_card (SUIT_SPADES, j) ;
		    return ;
		    }
		}
	    }

	/*
	 * We'll look for a low spade to lead.
	 * If there are spades outstanding, spade lead okay.
	 * If I have the Ace, King, or Queen, I must have at least 5 spades
	 * or more than there are left in other hands.
	 */
	spade_differential = ph->n_in_suit[SUIT_SPADES] -
	      status->number_remaining_in_other_hands[SUIT_SPADES] ;
	if (
	    ph->n_in_suit[SUIT_SPADES] &&
	    status->number_remaining_in_other_hands[SUIT_SPADES] &&
	    (
	     (
	     Rank(SUIT_SPADES,IndexOfHighestInSuit) < RANK_QUEEN
	     )
	     ||
	     ph->n_in_suit[SUIT_SPADES] >= 6
	     ||
	     spade_differential > 0
	    )
           )
	    {
	    for (i=0;  i < ph->n_in_suit[SUIT_SPADES];  i++)
		{
		/* TODO : don't do this if hearts are dropping! */

		if (Rank(SUIT_SPADES,i) <= RANK_JACK &&
		    Rank(SUIT_SPADES,i) <
		    status->highest_remaining_in_other_hands[SUIT_SPADES])
		    {
		    /* Play the highest spade less than the queen */
		    /* But don't do it if I have an unprotected A or K */
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing a low spade\n") ;
#endif
		    play_this_card (SUIT_SPADES, i) ;
		    return ;
		    }
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No low spade to play\n") ;
#endif

	/*
	 * Now look to see if I have a singleton diamond or one left
	 * of a doubleton diamond
	 */
	if (status->suit_leads[SUIT_DIAMONDS] == 0)
	    {
	    if (ph->n_in_suit[SUIT_DIAMONDS] < 5 &&
		ph->n_in_suit[SUIT_DIAMONDS] > 0)
		{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing a high diamond\n") ;
#endif
		play_this_card (SUIT_DIAMONDS, IndexOfHighestInSuit) ;
		return ;
		}
	    }
	else
	if (status->suit_leads[SUIT_DIAMONDS] == 1)
	    {
	    if (ph->n_in_suit[SUIT_DIAMONDS] == 1)
		{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing last diamond of a doubleton\n") ;
#endif
		play_this_card (SUIT_DIAMONDS, 0) ;
		return ;
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Not first round of diamonds or I don't have any\n") ;
#endif

	/*
	 * Now look to see if I have the remaining card of a club doubleton
	 */
	if (status->suit_leads[SUIT_CLUBS] == 1)
	    {
	    if (ph->n_in_suit[SUIT_CLUBS] == 1)
		{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing last club of a doubleton\n") ;
#endif
		play_this_card (SUIT_CLUBS, 0) ;
		return ;
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Not second round of clubs or I don't have one left\n") ;
#endif

	/*
	 * Now look to see if any of my lowest cards are the lowest remaining
	 * (With at least one other out there!)
	 * Do it in the order clubs, diamonds, hearts, spades
	 */
	for (i=N_SUITS-1;  i >= 0;  i--)
	    {
	    int card, rank ;
	    if (ph->n_in_suit[i] == 0)
		continue ;

	    if (
		LowestInSuit(i) == status->lowest_remaining[i] &&
		status->number_remaining_in_other_hands[i] > 0 &&
		(i != SUIT_HEARTS || info->points_broken)
	       )
		{
		if (i != SUIT_SPADES || !status->i_have_queen)
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing lowest remaining card of suit %d\n", i) ;
#endif
		    play_this_card (i, IndexOfLowestInSuit(i)) ;
		    return ;
		    }
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No lowest remaining cards in my hand\n") ;
#endif

	/*
	 * Lead a relatively low card (likely someone will play higher)
	 * To determine likelihood,
	 *  1) check the number of cards remaining in other hands that beat
	 *     my lowest.
	 *  2) check the number of hands (-1) that followed suit the last time
	 *  3) if (2) is greater than (1), it's a fair bet
	 */
	for (i=0;  i < N_SUITS;  i++)
	    {
	    int my_lowest ;
	    int cards_under_me ;
	    int followed_suit ;

	    /* Continue if I have no cards in this suit */
	    if (ph->n_in_suit[i] == 0)
		continue ;

	    /* Continue if opponents have no cards in this suit */
	    if (status->number_remaining_in_other_hands[i] == 0)
		continue ;

	    /* Continue if my worst is better than opponent's best */
	    if (LowestInSuit(i) > status->highest_remaining_in_other_hands[i])
		continue ;

	    /* continue if I can't lead a heart */
	    if (i == SUIT_HEARTS &&
		!info->points_broken &&
		!status->i_have_only_points)
		continue ;

	    my_lowest = LowestInSuit(i) ;
	    cards_under_me = 0 ;

	    for (j=my_lowest-1;  j >= 0;  j--)
		{
		if (status->card_status[i][j] != Played)
		    cards_under_me++ ;
		}

	    for (j=CurrentTrick-1;  j >= 0;  j--)
		{
		if (SuitOfTrick(j,0) == i)
		    break ;
		}
	    if (j < 0)
		{
		followed_suit = 3 ;
		}
	    else
		{
		followed_suit = 0 ;
		for (k=0;  k < N_PLAYERS;  k++)
		    {
		    if (TrickPlayer(j,k) == player_index)
			continue ;
		    if (SuitOfTrick(j,k) == SuitOfTrick(j,0))
			followed_suit++ ;
		    }
		}
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "cards_under_me, followed_suit: %d\n",
cards_under_me, followed_suit) ;
#endif
	    if (followed_suit > cards_under_me)
		{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing reasonably low card\n") ;
#endif
		play_this_card (i, IndexOfLowestInSuit(i)) ;
		return ;
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No reasonably low card to play\n") ;
#endif

	/**
	 ** Try to Avoid a BAD Lead
	 **/
	/* Bad leads are :
	 *	The Queen of spades
	 *	Ace or King of spades if queen still out
	 *	any spade if short in spades and hold Q, K, or A
	 *	High cards in oft-lead suits
	 */
	/* TODO:
	if (IHave(SUIT_SPADES, RANK_QUEEN))
	    MarkBad(SUIT_SPADES, RANK_QUEEN) ;
	if (IHave(SUIT_SPADES, RANK_KING) && !info->queen_played)
	    MarkBad(SUIT_SPADES, RANK_KING) ;
	if (IHave(SUIT_SPADES, RANK_ACE) && !info->queen_played)
	    MarkBad(SUIT_SPADES, RANK_ACE) ;
	*/

	/**
	 ** Try to select something non-bad
	 **/
	/* If no good leads, choose from non-bad leads :
	 *	lowest club unless guaranteed to lose
	 *	lowest diamond unless guaranteed to lose
	 *	lowest heart unless guaranteed to lose
	 *	lowest spade
	 */
	/*
	 * Now look to see if any of my lowest cards have some higher ones out
	 * Do it in the order clubs, diamonds, hearts, spades
	 */
	for (i=N_SUITS-1;  i >= 0;  i--)
	    {
	    int card, rank ;
	    if (ph->n_in_suit[i] == 0) /* continue if none in this suit */
		continue ;

	    if (i == SUIT_HEARTS &&	/* continue if I can't lead a heart */
		!info->points_broken &&
		!status->i_have_only_points)
		continue ;

	    for (j=CARDS_PER_SUIT-1;  j > LowestInSuit(i);  j--)
		{
		/* if there is a higher card out, I can lead it */
		/* Todo : rate each suit according to likelihood,
		   e.g., with only one person following suit, it can
		   be determined if he can beat me... */
		if (status->card_status[i][j] == InOpponentHand)
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing a card that has a chance\n") ;
#endif
		    play_this_card (i, IndexOfLowestInSuit(i)) ;
		    return ;
		    }
		}
	    }
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "I'm Hosed!\n") ;
#endif
	/*
	 * Well, I have to lead something...
	 * Do it in the order clubs, diamonds, hearts, spades
	 */
	for (i=N_SUITS-1;  i >= 0;  i--)
	    {
	    int card, rank ;
	    if (ph->n_in_suit[i] == 0) /* continue if none in this suit */
		continue ;

	    if (i == SUIT_HEARTS &&	/* continue if I can't lead a heart */
		!info->points_broken &&
		!status->i_have_only_points)
		continue ;

	    /* Playing highest avoids playing queen from AKQ */
	    play_this_card (i, IndexOfHighestInSuit) ;
	    }
	}
    }

/*
 * Else we are following
 */

else
    {
    if (status->follow == Following)
	{
	if (ph->n_in_suit[SuitLead] == 1)
	    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing my only one in the suit lead (%d)\n", SuitLead) ;
#endif
	    play_this_card (SuitLead, IndexOfHighestInSuit) ;
	    return ;
	    }

	if (status->shoot == Shooting)
	    {
	    }
	else
	if (status->shoot == Defending)
	    {
	    /* try to take hearts dropped in a trick
	     * ignore for now:
	     *     shooter is already guaranteed to lose this trick
	     *     possibility that swallowing queen is sometimes a good idea
	     */
	    if (HeartsInThisTrick && !QueenInThisTrick)
		{
		int sole_winner_lost = 0 ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Defensively trying to take a point\n") ;
#endif
	        /*
		 * What I want here is to check if the guy who has the points
		 * will not win this trick.  True if he has played and he
		 * has already played a card smaller than the trick taking rank.
		 */

		for (i=0;  i < snap->n_played_to_trick;  i++)
		    {
		    if (
			(TrickPlayer(CurrentTrick,i) == info->sole_point_holder)
			&&
			(info->sole_point_holder != TrickWinner)
		       )
			{
			sole_winner_lost = 1 ;
			break ;
			}
		    }

		if (sole_winner_lost)
		    ;
		else
		if (SuitLead != SUIT_SPADES ||
		    Rank(SuitLead,IndexOfHighestInSuit) < RANK_QUEEN)
		    {
		    if (Rank(SuitLead,IndexOfHighestInSuit) <
			info->trick_taking_rank &&
		        ph->n_in_suit[SuitLead] > 1)
			{
			play_this_card (SuitLead,
					IndexOfSecondHighestInSuit) ;
			}
		    else
			{
			play_this_card (SuitLead,
					IndexOfHighestInSuit) ;
			}
		    return ;
		    }
		}
	    /*
	     * If I can't do any of these, just minimize
	     */
	    status->shoot = Minimizing; 
	    }

	if (status->shoot == Minimizing)
	    {
	    Choice choice ;
	    /*
	     * if spades lead and I can lose the queen, play it
	     * if first round for diamonds or clubs, play the highest
	     * if fourth player, play highest if
	     *    no points in trick
	     *    no queen, and only one opponent has points, and he might win
	     *    else, play highestLosing
	     *    (TODO:will want to adjust if I don't want the lead)
	     * if spades, play the highest under the queen
	     * if must play higher, play lowestWinning
	     * else, play highestLosing
	     */
	    if (SuitLead == SUIT_SPADES && status->i_have_queen &&
		info->trick_taking_rank > RANK_QUEEN)
		{
		for (i=0;  i < 3;  i++)
		    {
		    if (Rank(SUIT_SPADES, i) == RANK_QUEEN)
			{
			play_this_card (SUIT_SPADES, i) ;
			return ;
			}
		    }
		}
	    if (status->suit_leads[SuitLead] == 1 &&
		(SuitLead == SUIT_CLUBS || SuitLead == SUIT_DIAMONDS) &&
		info->n_hearts_played[CurrentTrick] == 0 &&
		(!info->queen_played ||
		 info->queen_played_trick != CurrentTrick))
		{
		/*
		 * TODO : Always play low if guaranteed that I have
		 * gobs of low cards.  Why take any chance at all?
		 * (For example, 4D is lead.  I have 2,3,5,x,x...
		 *  Playing the 2 still leaves me with 2 safe low cards.)
		 */
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing High Club or Diamond\n") ;
#endif
		choice = HighestCard ;
		}
	    else
	    if (snap->n_played_to_trick == 3)
		{
#define IMustWinThisTrick (Rank(SuitLead,IndexOfLowestInSuit(SuitLead)) > \
			   info->trick_taking_rank)
		if (IMustWinThisTrick ||
		   (info->n_hearts_played[CurrentTrick] == 0 &&
		    (!info->queen_played ||
		     info->queen_played_trick != CurrentTrick)))
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing High in Last position\n") ;
#endif
		    if (SuitLead == SUIT_SPADES &&
			Rank(SUIT_SPADES,0) == RANK_QUEEN &&
			RANK_QUEEN > info->trick_taking_rank)
			{
			choice = SecondHighestCard ;
			}
		    else
			{
			choice = HighestCard ;
			}
		    }
		else
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing Low in Last position\n") ;
#endif
		    choice = HighestLosingCard ;
		    }
		}
	    else
	    if (SuitLead == SUIT_SPADES)
		{
		for (i=0;  i < ph->n_in_suit[SUIT_SPADES];  i++)
		    {
		    if (Rank(SUIT_SPADES,i) < RANK_QUEEN)
			break ;
		    }
		if (i < ph->n_in_suit[SUIT_SPADES])
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing high spade under the queen\n") ;
#endif
		    play_this_card (SUIT_SPADES, i) ;
		    return ;
		    }
		else
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing high spade\n") ;
#endif
		    play_this_card (SUIT_SPADES, 0) ;
		    return ;
		    }
		}
	    else
		{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Playing highest loser\n") ;
#endif
		choice = HighestLosingCard ;
		}

	    play_this_card (SuitLead, IndexOfChoice(ph,SuitLead,choice)) ;
	    return ;
	    }
	}
    else
    if (status->follow == Sloughing)
	{
	Boolean defending = False ;
	/*
	 * Need to check for first trick and points breakable.
	 * If not breakable, we can't drop a point (unless nothing but points!)
	 */
	if (status->shoot == Shooting)
	    {
	    }
	else
	if (status->shoot == Defending)
	    {
	    defending = True ;
	    status->shoot = Minimizing; 
	    }

	if (status->shoot == Minimizing)
	    {
	    float rank_factor, length_factor, suit_factor ;
	    float worst_factor, card_factor ;
	    int card_i, card_j ;
	    /*
	     * Good sloughs are:
	     *	The queen of spades
	     *  Thinly defended A,K of spades (see following rule)
	     *  High cards in suits with no low cards (short suits first)
	     *  Short suits
	     *  Hearts
	     */
if (logerr) fprintf (logerr, "defending: %d\n", defending) ;
	    if (status->card_status[SUIT_SPADES][RANK_QUEEN] == InMyHand)
		{
		if (CurrentTrick > 0 || history->points_on_first_trick)
		    {
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Sloughing the Queen of Spades\n") ;
#endif
		    for (i=0;  i < ph->n_in_suit[SUIT_SPADES];  i++)
			{
			if (Rank(SUIT_SPADES,i) == RANK_QUEEN)
			    {
			    play_this_card (SUIT_SPADES, i) ;
			    return ;
			    }
			}
		    }
		}
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "No queen to slough\n") ;
#endif

	    worst_factor = -1.0 ;
	    for (i=0;  i < N_SUITS;  i++)
		{
		if (i == SUIT_HEARTS &&
		    CurrentTrick==0 &&
		    !history->points_on_first_trick &&
		    !status->i_have_only_points)
		    continue ;
		for (j=0;  j < ph->n_in_suit[i];  j++)
		    {
		    float defense_factor = 1.0 ;
		    int high_out, low_out ;
		    /*
		     * Compute the defense_factor
		     */
		    /*
		     * Do not drop a protected heart honor
		     */
		    if (i == SUIT_HEARTS && j == 0 && defending)
			{
			/*
			 * Find out how many are still out that can beat
			 * my highest heart
			 */
			int n_able_to_beat = 0 ;
			for (k=Rank(i,j)+1;  k < CARDS_PER_SUIT;  k++)
			    if (status->card_status[i][k] == InOpponentHand)
				n_able_to_beat++ ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "There are %d hearts out that can beat my highest heart\n",
n_able_to_beat) ;
#endif
			/*
			 * If I have that many "protectors", I won't drop
			 * my highest heart
			 */
			if (ph->n_in_suit[i] > n_able_to_beat)
			    defense_factor = 0.0 ;
			}
		    /*
		     * (Ignore for now:)
		     * Do not drop ANY well-protected honor
		     */

		    /*
		     * Compute the suit_factor
		     */
		    if (i == SUIT_SPADES)
			{
			if (Rank(i,j) == RANK_ACE || Rank(i,j) == RANK_KING)
			    suit_factor = 1.0 ;
			else
			    suit_factor = 0.0 ;
			}
		    else
		    if (i == SUIT_HEARTS)
			{
			suit_factor = 0.8 ;
			}
		    else
			suit_factor = 0.6 ;

		    /*
		     * Compute the rank_factor
		     * (all cards which are higher than all of the opponents'
		     * cards are 1.0, all cards which are lower are 0.1, and
		     * those in between are scaled accordingly
		     */
		    if (status->number_remaining_in_other_hands[i] > 0)
			{
			if (Rank(i,j) >
			    status->highest_remaining_in_other_hands[i])
			    {
			    rank_factor = 1.0 ;
			    }
			else
			if (Rank(i,j) <
			    status->lowest_remaining_in_other_hands[i])
			    {
			    rank_factor = 0.05 ;
			    }
			else
			    {
			    rank_factor = ((float)Rank(i,j) -
			     status->lowest_remaining_in_other_hands[i]) /
			     ((float)status->highest_remaining_in_other_hands[i]
			      - status->lowest_remaining_in_other_hands[i]) ;
			    }
			}
		    else
			{
			rank_factor = 0.0 ;
			}
		    /*
		     * Compute the length_factor
		     * Singletons are 1.0
		     * Doubletons are 0.5
		     * Tripletons are 0.25, etc.
		     */
		    length_factor = 1.0 ;
		    for (k=2;  k < ph->n_in_suit[i];  k++)
			length_factor /= 2.0 ;
		    card_factor = suit_factor * rank_factor *
				  length_factor * defense_factor ;
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "card_factor : %5.3f = %4.2f * %4.2f * %4.2f * %4.2f\n",
card_factor, suit_factor, rank_factor, length_factor, defense_factor) ;
#endif
		    if (card_factor > worst_factor)
			{
			worst_factor = card_factor ;
			card_i = i ;
			card_j = j ;
			}
		    }
		}
	    play_this_card (card_i, card_j) ;
	    return ;
	    }
	}
    }

return ;
}

static void
find_the_initial_queen (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
/* in hand from where vs. passed to whom vs. unknown */
return ;
}

static void
find_the_current_queen (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
/* queen in hand, ahead, behind, or unknown position in current trick */
return ;
}

static void
determine_initial_defense_potential (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
/*
 * if we have a protected heart honor we are defending
 */
/*
 * if currently shooting, find out if we still are
 *    if not still shooting, find out if we need to be defending
 * if currently defending, find out if we still are
 * if currently minimizing, we just return
 */
status->shoot = Defending ;
return ;
}

static void
determine_current_defense_potential (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
int i ;
/*
 * if hearts are split we no longer need to defend
 */
int n_have_points_in_hand = 0 ;
for (i=0;  i < N_PLAYERS;  i++)
    {
    if (info->n_points_taken[i])
	{
	info->sole_point_holder = i ;
	n_have_points_in_hand++ ;
	}
    }
if (n_have_points_in_hand > 1)
    status->shoot = Minimizing ;
else
    status->shoot = Defending ;
if (logerr) fprintf (logerr, "status->shoot = %d\n", status->shoot) ;
return ;
}

static void
find_trick_stats (ph, player_index)
    PvtHistory *ph ;
    int player_index ;
{
int i,j,k ;
/*
 * how many cards in this suit have been played
 * how many times this suit has been led
 * whether we are following or sloughing
 */
/*
 * (Do it from scratch every time)
 * Set every card to "in opponent's hand"
 * Set cards dealt to me "in my hand"
 * Set cards passed by me "in opponent's hand"
 * Set cards passed to me "in my hand"
 * Set cards played "played"
 */

for (i=0;  i < N_SUITS;  i++)
    {
    status->suit_leads[i] = 0 ;
    for (j=0;  j < CARDS_PER_SUIT;  j++)
	{
	status->card_status[i][j] = InOpponentHand ;
	}
    }

for (i=0;  i < CARDS_PER_PLAYER;  i++)
    {
    int card, suit, rank ;
    card = ph->cards_dealt[i] ;
    suit = info->deck[card].suit ;
    rank = info->deck[card].rank ;
    status->card_status[suit][rank] = InMyHand ;
    }

if (snap->current_pass != KeepPass)
    {
    for (i=0;  i < N_DISCARDS;  i++)
	{
	int card, suit, rank ;
	card = ph->cards_passed[i] ;
	suit = info->deck[card].suit ;
	rank = info->deck[card].rank ;
	status->card_status[suit][rank] = InOpponentHand ;
	card = ph->cards_received[i] ;
	suit = info->deck[card].suit ;
	rank = info->deck[card].rank ;
	status->card_status[suit][rank] = InMyHand ;
	}
    }


for (i=0;  i <= CurrentTrick;  i++)
    {
    for (j=0;  j < N_PLAYERS;  j++)
	{
	int card, suit, rank ;
	if (i == CurrentTrick && j >= snap->n_played_to_trick)
	    continue ;
	card = history->trick[i].card[j] ;
	suit = info->deck[card].suit ;
	rank = info->deck[card].rank ;
	status->card_status[suit][rank] = Played ;
	if (j == 0)
	    status->suit_leads[suit]++ ;
	}
    }

for (i=0;  i < N_SUITS;  i++)
    {
    for (j=0;  j < CARDS_PER_SUIT;  j++)
	{
	if (status->card_status[i][j] != Played)
	    {
	    status->lowest_remaining[i] = j ;
	    break ;
	    }
	}
    }

for (i=0;  i < N_SUITS;  i++)
    {
    status->number_remaining_in_other_hands[i] = 0 ;
    for (j=0;  j < CARDS_PER_SUIT;  j++)
	{
	if (status->card_status[i][j] == InOpponentHand)
	    {
	    status->number_remaining_in_other_hands[i]++ ;
	    }
	}
    }

for (i=0;  i < N_SUITS;  i++)
    {
    status->lowest_remaining_in_other_hands[i] = -1 ;
    for (j=0;  j < CARDS_PER_SUIT;  j++)
	{
	if (status->card_status[i][j] == InOpponentHand)
	    {
	    status->lowest_remaining_in_other_hands[i] = j ;
	    break ;
	    }
	}
    }

for (i=0;  i < N_SUITS;  i++)
    {
    status->highest_remaining_in_other_hands[i] = -1 ;
    for (j=CARDS_PER_SUIT-1;  j >= 0;  j--)
	{
	if (status->card_status[i][j] == InOpponentHand)
	    {
	    status->highest_remaining_in_other_hands[i] = j ;
	    break ;
	    }
	}
    }

/*
 * See if I have the queen
 */

status->i_have_queen = False ;

for (j=0;  j < ph->n_in_suit[SUIT_SPADES];  j++)
    {
    if (Rank(SUIT_SPADES,j) == RANK_QUEEN)
	{
	status->i_have_queen = True ;
	break ;
	}
    }

/*
 * See if I have only points in my hand
 */

status->i_have_only_points = True ;

for (i=0;  i < N_SUITS;  i++)
    {
    for (j=0;  j < ph->n_in_suit[i];  j++)
	{
	if (i != SUIT_HEARTS && (i != SUIT_SPADES || Rank(i,j) != RANK_QUEEN))
	    {
	    status->i_have_only_points = False ;
	    break ;
	    }
	}
    if (j < ph->n_in_suit[i])
	break ;
    }

/*
 * See whether I am following or sloughing
 */

if (snap->n_played_to_trick)
    {
    status->follow = ph->n_in_suit[SuitLead] ? Following : Sloughing ;
    }

show_stats (status) ;
return ;
}

static void
play_this_card (suit, index)
    int suit ;
    int index ;
{
data[1] = suit ;
data[2] = index ;
card_picked (NULL, data, NULL) ;
return ;
}

int
IndexOfChoice (ph, suit, choice)
    PvtHistory *ph ;
    int suit ;
    Choice choice  ;
{
int i ;
if (choice == HighestCard)
    {
    return 0 ;
    }
else
if (choice == SecondHighestCard)
    {
    return 1 ;
    }
else
if (choice == HighestLosingCard)
    {
    for (i=ph->n_in_suit[suit]-1;  i >= 0;  i--)
	if (Rank(suit,i) > info->trick_taking_rank)
	    break ;
    if (i < ph->n_in_suit[suit]-1)
	{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Selected Highest Loser\n") ;
#endif
	return i+1 ;
	}
    else
	{
#ifndef NO_LOGGING
if (logerr)
fprintf (logerr, "Selected Lowest Winner\n") ;
#endif
	return i ;
	}
    }
}

show_stats (status)
    PlayStatus *status ;
{
int i,j ;
#ifndef NO_LOGGING
if (!logerr)
	return ;
fprintf (logerr, "\n") ;
for (i=0;  i < N_SUITS;  i++)
    {
    for (j=CARDS_PER_SUIT-1; j >= 0;  j--)
	{
	if (status->card_status[i][j] == InOpponentHand)
	    {
	    fprintf (logerr, "o ") ;
	    }
	else
	if (status->card_status[i][j] == InMyHand)
	    {
	    fprintf (logerr, "m ") ;
	    }
	else
	    {
	    fprintf (logerr, ". ") ;
	    }
	}
    fprintf (logerr, "  (%s)\n", game->suit[i].name) ;
    }

fprintf (logerr, "\n") ;
fprintf (logerr, "number_remaining:  ") ;
for (i=0;  i < N_SUITS;  i++)
    fprintf (logerr, "%3d", status->number_remaining_in_other_hands[i]) ;
fprintf (logerr, "\n") ;

fprintf (logerr, "highest_remaining: ") ;
for (i=0;  i < N_SUITS;  i++)
	fprintf (logerr, "%3d", status->highest_remaining_in_other_hands[i]) ;
fprintf (logerr, "\n") ;

fprintf (logerr, "lowest_remaining:  ") ;
for (i=0;  i < N_SUITS;  i++)
	fprintf (logerr, "%3d", status->lowest_remaining_in_other_hands[i]) ;
fprintf (logerr, "\n") ;

fprintf (logerr, "suit_leads      :  ") ;
for (i=0;  i < N_SUITS;  i++)
	fprintf (logerr, "%3d", status->suit_leads[i]) ;
fprintf (logerr, "\n") ;

fprintf (logerr, "CurrentTrick		: %d\n", CurrentTrick) ;
fprintf (logerr, "queen_played		: %d\n", info->queen_played) ;
fprintf (logerr, "queen_played_trick	: %d\n", info->queen_played_trick) ;
fprintf (logerr, "points_broken		: %d\n", info->points_broken) ;
fprintf (logerr, "i_have_only_points	: %d\n", status->i_have_only_points) ;
fprintf (logerr, "trick_taking_rank	: %d\n", info->trick_taking_rank) ;
#endif

return ;
}

