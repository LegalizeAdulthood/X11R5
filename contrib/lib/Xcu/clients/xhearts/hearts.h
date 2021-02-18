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

#define N_PLAYERS		4
#define N_SUITS			4
#define N_SUITS_PLUS_ONE	5
#define N_DISCARDS		3
#define N_TRICKS		13
#define CARDS_PER_PLAYER	13
#define CARDS_PER_SUIT		13
#define CARDS_PER_DECK		52
#define SHOOT_POINTS		26

#define SUIT_SPADES	0
#define SUIT_HEARTS	1
#define SUIT_DIAMONDS	2
#define SUIT_CLUBS	3

#define RANK_ACE	12
#define RANK_KING	11
#define RANK_QUEEN	10
#define RANK_JACK	9
#define RANK_TEN	8
#define RANK_NINE	7
#define RANK_EIGHT	6
#define RANK_SEVEN	5
#define RANK_SIX	4
#define RANK_FIVE	3
#define RANK_FOUR	2
#define RANK_THREE	1
#define RANK_TWO	0

#define HEART_POINTS	 1
#define QUEEN_POINTS	13
#define JACK_POINTS	10

#define HEART_POINT_CARDS	13
#define QUEEN_POINT_CARDS	 1
#define JACK_POINT_CARDS	 1

typedef void (*Program)() ;

typedef enum
    {
    HumanPhysiology,
    ComputerPhysiology
    } Physiology ;

typedef enum
    {
    NullMoonDecision,
    LowerMe,
    RaiseOthers
    } MoonDecision ;

typedef enum
    {
    NullHandDecision,
    AnotherHand,
    NoMore
    } HandDecision ;

typedef enum
    {
    NullClear,
    ClearOnLegalPlay,
    ClearOnLegalDiscard,
    ClearOnReceipt
    } PlayerError ;

typedef enum
    {
    NullMode,
    DiscardMode,
    ConfirmDMode,
    ConfirmRMode,
    PlayMode
    } PlayerMode ;

typedef enum
    {
    InitialPass,
    LeftPass,
    RightPass,
    AcrossPass,
    KeepPass
    } CurrentPass ;

typedef enum
    {
    LeftRightAcross,
    LeftRight,
    LeftRightAcrossKeep,
    LeftRightKeep
    } PassingStyle ;

typedef struct
    {
    XtAppContext app ;
    Display	*dpy[N_PLAYERS] ;
    Widget	wlm_id[N_PLAYERS] ;
    Widget	top[N_PLAYERS] ;
    Widget	table_deck[N_PLAYERS] ;
    Widget	play_tbl[N_PLAYERS] ;
    Widget	exchange_tbl[N_PLAYERS] ;
    Widget	exchange_deck[N_PLAYERS] ;
    Widget	exchange_discard[N_PLAYERS] ;
    Widget	exchange_waiting[N_PLAYERS] ;
    Widget	exchange_confirm_d[N_PLAYERS] ;
    Widget	exchange_confirm_r[N_PLAYERS] ;
    Widget	moon_decision[N_PLAYERS] ;
    Widget	hand_decision[N_PLAYERS] ;
    Widget	*hands[N_PLAYERS][N_SUITS] ;
    Widget	score[N_PLAYERS][N_PLAYERS] ;
    Widget	points[N_PLAYERS][N_PLAYERS] ;
    Widget	tricks[N_PLAYERS][N_PLAYERS] ;
    Widget	plays[N_PLAYERS][N_PLAYERS] ;
    Widget	discards[N_PLAYERS][N_DISCARDS] ;
    Widget	discard_bitmaps[N_PLAYERS][N_DISCARDS] ;
    Widget	discard_suit[N_PLAYERS][N_DISCARDS][N_SUITS_PLUS_ONE] ;
    Widget	south_bitmap[N_PLAYERS] ;
    Widget	north_bitmap[N_PLAYERS] ;
    Widget	east_bitmap[N_PLAYERS] ;
    Widget	west_bitmap[N_PLAYERS] ;
    Widget	discard_bitmap[N_PLAYERS][N_DISCARDS] ;
    Widget	south_suit[N_PLAYERS][N_SUITS_PLUS_ONE] ;
    Widget	north_suit[N_PLAYERS][N_SUITS_PLUS_ONE] ;
    Widget	east_suit[N_PLAYERS][N_SUITS_PLUS_ONE] ;
    Widget	west_suit[N_PLAYERS][N_SUITS_PLUS_ONE] ;
    } XStruct ;

typedef struct
    {
    String	symbol ;
    int		card ;
    } Rank ;

typedef struct
    {
    String	symbol ;
    Pixel	color[N_PLAYERS] ;
    String	color_string ;
    String	character ;
    String	name ;
    Rank	*rank; 
    } Suit ;

typedef struct
    {
    int		suit ;
    int		rank ;
    } Card ;

typedef struct
    {
    Boolean	dealt ;
    int		dealt_to ;
    } Deal ;

typedef struct
    {
    int		card[N_PLAYERS] ;
    int		leader ;
    } Trick ;

typedef struct
    {
    int		score ;
    Boolean	exchanged ;
    Boolean	discard_confirmed ;
    Boolean	exchange_confirmed ;
    Boolean	legal_play ;
    PlayerError	error_mode ;
    int		n_chosen ;
    PlayerMode	mode ;
    int		pass_from ;
    HandDecision hand_decision ;
    Physiology	physiology ;
    Program	discard_program ;
    Program	play_program ;
    String	name_string ;
    } Player ;

typedef struct
    {
    /*
     * These are useful public history attributes
     */
    int		goal ;
    short	xsubi[3] ;
    PassingStyle passing_style ;
    int		n_discards ;
    Boolean	jack_counts ;
    Boolean	points_on_first_trick ;
    Trick	trick[N_TRICKS] ;		
    } History ;

typedef struct
    {
    /*
     * These are private history attributes
     */
    int		cards_dealt[CARDS_PER_PLAYER] ;
    int		cards_passed[N_DISCARDS] ;
    int		cards_received[N_DISCARDS] ;
    int		n_in_suit[N_SUITS] ;
    int		*suit[N_SUITS] ;
    } PvtHistory ;

typedef struct
    {
    String	seed ;
    int		sleep_seconds ;
    Boolean	conceded ;
    String	seat[N_PLAYERS] ;
    int		ccw[N_PLAYERS] ;
    int		cw[N_PLAYERS] ;
    MoonDecision moon_decision ;
    HandDecision hand_decision ;
    String	player_file ;
    String	wlm_file ;
    String	log_file ;

    Player	player[N_PLAYERS] ;
    Deal	deck[CARDS_PER_DECK] ;
    XStruct	x ;
    Suit	suit[N_SUITS] ;
    } Game ;

typedef struct
    {
    /*
     * This is a snapshot of the current state of the game
     */
    int		n_tricks_played ;
    int		n_played_to_trick ;
    CurrentPass current_pass ;
    } Snap ;

typedef struct
    {
    /*
     * These are tables of just about anything one would want to know
     * if examining the state of the game
     */
    int		total_points_taken ;
    int		n_point_cards ;
    int		n_point_cards_taken ;
    int		n_points_taken[N_PLAYERS] ;
    int		n_tricks_taken[N_PLAYERS] ;
    int		total_hearts_played ;
    int		trick_winner[N_TRICKS] ;
    int		n_hearts_played[N_TRICKS] ;
    Boolean	jack_played ;		
    int		jack_played_trick ;
    Boolean	queen_played ;		
    int		queen_played_trick ;
    Boolean	points_broken ;
    int		points_broken_trick ;
    Card	deck[CARDS_PER_DECK] ;
    int		cards_per_suit[N_SUITS] ;
    int		trick_taking_rank ;
    int		sole_point_holder ;
    } Info ;

#define CurrentTrick	(snap->n_tricks_played)
#define SuitLead	(info->deck[history->trick[CurrentTrick].card[0]].suit)
#define Leader		(history->trick[CurrentTrick].leader)
#define ActivePlayer	(active_player (game, history, snap, info))
#define TrickWinner	(info->trick_winner[CurrentTrick])
#define LastTrickWinner	(info->trick_winner[CurrentTrick - 1])
#define JackWinner	(info->trick_winner[info->jack_played_trick])
#define ThereArePointsInTrick \
   ( \
    (info->n_hearts_played[CurrentTrick] > 0) || \
    (info->queen_played && (info->queen_played_trick == CurrentTrick))\
   )
#define HeartsInThisTrick (info->n_hearts_played[CurrentTrick])
#define QueenInThisTrick \
    (info->queen_played && (info->queen_played_trick == CurrentTrick))
#define JackInThisTrick \
    (info->jack_played && (info->jack_played_trick == CurrentTrick))

extern Game *game ;
extern History *history ;
extern PvtHistory pvt_history[N_PLAYERS] ;
extern Snap *snap ;
extern Info *info ;

extern FILE *logerr ;
