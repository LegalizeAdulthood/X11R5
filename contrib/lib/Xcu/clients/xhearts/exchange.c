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

static Boolean all_players_have_exchanged () ;
static void initialize_not_exchanged () ;
static void get_next_pass_direction () ;
static void exchange_cleanup () ;
static void announce_pass_direction () ;
static void discard_selected () ;
static void undiscard_selected () ;
static void show_received_discard () ;
static void receive_discards () ;
static void get_computer_discards () ;
void confirm_receipt () ;

void
exchange ()
{
get_next_pass_direction () ;
if (snap->current_pass == KeepPass)
    return ;

announce_pass_direction () ;
initialize_not_exchanged () ;
get_computer_discards () ;

for (  ;
    !all_players_have_exchanged () ;
    wait_for_player_events ()
    )
    ;

exchange_cleanup () ;
}

static void
initialize_not_exchanged ()
{
int i ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    game->player[i].exchanged = False ;
    game->player[i].exchange_confirmed = False ;
    game->player[i].discard_confirmed = False ;
    game->player[i].n_chosen = 0 ;
    game->player[i].mode = DiscardMode ;
    }
}

static Boolean
all_players_have_exchanged ()
{
int i ;

/*
 * First see if there are any new exchanges to be performed
 * For this to happen, a player must have confirmed his discards,
 * the player passing to him must have confirmed his, and
 * the player must not have already exchanged.
 */

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (
       !game->player[i].exchanged &&
       game->player[i].discard_confirmed &&
       game->player[game->player[i].pass_from].discard_confirmed
       )
	{
	receive_discards (i) ;
	}
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (!game->player[i].exchange_confirmed)
	{
	return False ;
	}
    }

return True ;
}

static void
receive_discards (player_index)
    int player_index ;
{
int i ;
int passer_index = game->player[player_index].pass_from ;
char text_string[100] ;
/*
 * Need to change the deck pointers,
 * Show the cards in the discard area
 * Ask for confirmation
 */
game->player[player_index].exchanged = True ;
game->player[player_index].n_chosen = 0 ;
game->player[player_index].mode = ConfirmRMode ;

for (i=0;  i < N_DISCARDS;  i++)
    {
    int card_number ;
    card_number = pvt_history[player_index].cards_passed[i] ;

    /* nobody's if not claimed by others yet */
    if (game->deck[card_number].dealt_to == player_index)
	game->deck[card_number].dealt_to = N_PLAYERS ;
    card_number = pvt_history[passer_index].cards_passed[i] ;
    game->deck[card_number].dealt_to = player_index ;
    show_received_discard(player_index, card_number);
    pvt_history[player_index].cards_received[i] = card_number ;
    }

if (game->player[player_index].physiology == HumanPhysiology)
    {
    sprintf (text_string, "Received from\n%s\n(Click to get)",
	     game->player[passer_index].name_string) ;
    XcuWlmSetValue (game->x.wlm_id[player_index],
		    "XcuCommand", "confirm_receipt", XtNlabel, text_string) ;
    XcuDeckRaiseWidget (game->x.exchange_deck[player_index],
			game->x.exchange_confirm_r[player_index]) ;
    }
else
    {
    long data[1] ;
    data[0] = (long) player_index ;
    confirm_receipt (NULL, data, NULL) ;
    }

return ;
}

static void
get_next_pass_direction ()
{
int i ;
switch (history->passing_style)
  {
  case LeftRight :
    {
    switch (snap->current_pass)
      {
      case LeftPass	: { snap->current_pass = RightPass ; break ; }
      case InitialPass	:
      case RightPass	: { snap->current_pass = LeftPass ; break ; }
      }
    break ;
    }
  case LeftRightAcross :
    {
    switch (snap->current_pass)
      {
      case LeftPass	: { snap->current_pass = RightPass ; break ; }
      case RightPass	: { snap->current_pass = AcrossPass ; break ; }
      case InitialPass	:
      case AcrossPass	: { snap->current_pass = LeftPass ; break ; }
      }
    break ;
    }
  case LeftRightAcrossKeep :
    {
    switch (snap->current_pass)
      {
      case LeftPass	: { snap->current_pass = RightPass ; break ; }
      case RightPass	: { snap->current_pass = AcrossPass ; break ; }
      case AcrossPass	: { snap->current_pass = KeepPass ; break ; }
      case InitialPass	:
      case KeepPass	: { snap->current_pass = LeftPass ; break ; }
      }
    break ;
    }
  case LeftRightKeep :
    {
    switch (snap->current_pass)
      {
      case LeftPass	: { snap->current_pass = RightPass ; break ; }
      case RightPass	: { snap->current_pass = KeepPass ; break ; }
      case InitialPass	:
      case KeepPass	: { snap->current_pass = LeftPass ; break ; }
      }
    break ;
    }
  }

for (i=0;  i < N_PLAYERS;  i++)
    {
    int target ;
    switch (snap->current_pass)
      {
      case LeftPass	: { target = i+1 ; break ; }
      case RightPass	: { target = i-1 ; break ; }
      case AcrossPass	: { target = i+2 ; break ; }
      }
    if (target < 0)
	target = N_PLAYERS - 1 ;
    if (target >= N_PLAYERS)
	target -= N_PLAYERS ;
    game->player[target].pass_from = i ;
    }
return ;
}

static void
announce_pass_direction ()
{
int i, j ;
char pass_text[30] ;

sprintf (pass_text, "Pass %d cards ", N_DISCARDS) ;
switch (snap->current_pass)
  {
  case LeftPass : { strcat (pass_text, "left") ; break ; }
  case RightPass : { strcat (pass_text, "right") ; break ; }
  case AcrossPass : { strcat (pass_text, "across") ; break ; }
  }

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    proc_message (pass_text, i) ;
    for (j=0;  j < N_DISCARDS;  j++)
	{
	XcuDeckRaiseWidget(game->x.discard_bitmap[i][j],
			   game->x.discard_suit[i][j][N_SUITS]);
	}
    XcuDeckRaiseWidget (game->x.table_deck[i], game->x.exchange_tbl[i]) ;
    XcuDeckRaiseWidget (game->x.exchange_deck[i], game->x.exchange_discard[i]) ;
    }
return ;
}

static void
exchange_cleanup ()
{
}

/*
 * client_data[0] : player number
 * client_data[1] : suit number
 * client_data[2] : rank number
 */

void
card_picked (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int i_player = (int) data[0] ;
Player *player = &game->player[i_player] ;

/* For debugging */
if (logerr) fprintf (logerr, "card_picked (%d)\n", i_player) ;
if (logerr) fflush (logerr) ;
show_card (i_player, data[1], data[2]) ;

switch (game->player[i_player].mode)
  {
  case DiscardMode :
    {
    discard_selected (data) ;
    break ;
    }
  case PlayMode :
    {
    play_selected (data) ;
    break ;
    }
  case ConfirmDMode :
    {
    err_message ("You must unselect one!", i_player) ;
    player->error_mode = ClearOnLegalDiscard ;
    break ;
    }
  case ConfirmRMode :
    {
    err_message ("You must acknowledge receipt!", i_player) ;
    player->error_mode = ClearOnReceipt ;
    break ;
    }
  case NullMode :
    {
    err_message ("Patience, Patience!", i_player) ;
    player->error_mode = ClearOnLegalPlay ;
    break ;
    }
  }

refresh_screens () ;

return ;
}

static void
show_received_discard (player_index, card_number)
    int player_index ;
    int card_number ;
{
Player *player = &game->player[player_index] ;
int deck_suit_index = info->deck[card_number].suit ;
int deck_rank_index = info->deck[card_number].rank ;
Arg args[2] ;

/*
 * Display the card in the discard region
 */

if (game->player[player_index].physiology == HumanPhysiology)
    {
    char discard_string[20] ;
    sprintf (discard_string, "discard%d", player->n_chosen) ;
/***
fprintf(stderr, "show_received_discard (%d) (%d) [%d] [%d]\n",
player_index, card_number, deck_suit_index, deck_rank_index);
fprintf(stderr, "%d (%s)\n", player->n_chosen, discard_string) ;
fprintf(stderr, "%d (%s) (%s)\n", game->x.wlm_id[player_index],
game->suit[deck_suit_index].color_string,
game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
***/
    XcuWlmSetValue (game->x.wlm_id[player_index],
		"XcuCommand", discard_string,
		XtNforeground, game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[player_index],
		"XcuCommand", discard_string, XtNlabel,
		game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen],
       game->x.discard_suit[player_index][player->n_chosen][deck_suit_index]);
    }

player->n_chosen++ ;

return ;
}

static void
discard_selected (data)
    long *data ;
{
int player_index = (int) data[0] ;
int suit_index = (int) data[1] ;
int rank_index = (int) data[2] ;
Player *player = &game->player[player_index] ;
int card_number = pvt_history[player_index].suit[suit_index][rank_index] ;
int deck_suit_index = info->deck[card_number].suit ;
int deck_rank_index = info->deck[card_number].rank ;
Arg args[2] ;

/*
 * First, display the card in the discard region
 */

if (rank_index >= pvt_history[player_index].n_in_suit[suit_index])
    {
    err_message ("Fumble fingers!", player_index) ;
    player->error_mode = ClearOnLegalDiscard ;
    return ;
    }

if (player->error_mode == ClearOnLegalDiscard)
    {
    err_message (" ", player_index) ;
    player->error_mode = NullClear ;
    }

pvt_history[player_index].cards_passed[player->n_chosen] = card_number ;
if (game->player[player_index].physiology == HumanPhysiology)
    {
    char name_string[20] ;
    sprintf (name_string, "discard%d", player->n_chosen) ;
    XcuWlmSetValue (game->x.wlm_id[player_index],
		"XcuCommand", name_string, XtNforeground,
		game->suit[deck_suit_index].color_string) ;
    XcuWlmSetValue (game->x.wlm_id[player_index],
		"XcuCommand", name_string, XtNlabel,
		game->suit[deck_suit_index].rank[deck_rank_index].symbol) ;
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen],
       game->x.discard_suit[player_index][player->n_chosen][0]);
    XSync (XtDisplay(game->x.wlm_id[player_index]), False) ;
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen],
       game->x.discard_suit[player_index][player->n_chosen][1]);
    XSync (XtDisplay(game->x.wlm_id[player_index]), False) ;
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen],
       game->x.discard_suit[player_index][player->n_chosen][2]);
    XSync (XtDisplay(game->x.wlm_id[player_index]), False) ;
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen],
       game->x.discard_suit[player_index][player->n_chosen][deck_suit_index]);
    }

/*
 * Now, remove the card from the hand
 * Collapse the labels left in the row and blank out the last one
 */

if (game->player[player_index].physiology == HumanPhysiology)
    {
    XtSetArg (args[0], XtNlabel, " ") ;
    XtSetValues (game->x.hands[player_index][suit_index]
	      [pvt_history[player_index].n_in_suit[suit_index]-1], args, 1) ;
    }

player->n_chosen++ ;
pvt_history[player_index].n_in_suit[suit_index]-- ;
order_hand (player_index) ;
show_deal (player_index) ;

/*
 * Finally, see if this is the last discard to be selected
 */

if (player->n_chosen == N_DISCARDS)
    {
    player->mode = ConfirmDMode ;
    if (game->player[player_index].physiology == HumanPhysiology)
	{
	XcuDeckRaiseWidget (game->x.exchange_deck[player_index],
			    game->x.exchange_confirm_d[player_index]) ;
	}
    }
return ;
}

static void
undiscard_selected (data)
    long *data ;
{
int i ;
int player_index = (int) data[0] ;
int discard_number = (int) data[1] ;
Player *player = &game->player[player_index] ;
int card_number = pvt_history[player_index].cards_passed[discard_number] ;
int deck_suit_index = info->deck[card_number].suit ;
int deck_rank_index = info->deck[card_number].rank ;
Arg args[2] ;

/*
 * First, undisplay the last card in the discard region
 */

if (discard_number >= player->n_chosen)
    {
    err_message ("Fumble fingers!", player_index) ;
    player->error_mode = ClearOnLegalDiscard ;
    return ;
    }

if (player->error_mode == ClearOnLegalDiscard)
    {
    err_message (" ", player_index) ;
    player->error_mode = NullClear ;
    }

if (game->player[player_index].physiology == HumanPhysiology)
    {
#ifndef NO_LOGGING
if (logerr) fprintf(logerr,"XtSetValues : undiscard_selected(%d,%d)\n",
player_index, player->n_chosen-1) ;
#endif
    XtSetArg (args[0], XtNlabel, " ") ;
    XtSetValues (game->x.discards[player_index][player->n_chosen - 1], args, 1);
    XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][player->n_chosen-1],
       game->x.discard_suit[player_index][player->n_chosen-1][N_SUITS]);
    }

/*
 * Collapse left
 */

player->n_chosen-- ;
for (i=discard_number;  i < player->n_chosen;  i++)
    {
    int suit_index ;
    int rank_index ;
    pvt_history[player_index].cards_passed[i] =
	pvt_history[player_index].cards_passed[i+1] ;
    suit_index = info->deck[pvt_history[player_index].cards_passed[i]].suit ;
    rank_index = info->deck[pvt_history[player_index].cards_passed[i]].rank ;

    if (game->player[player_index].physiology == HumanPhysiology)
	{
	XtSetArg (args[0],  XtNforeground,
			    game->suit[suit_index].color[player_index]) ;
	XtSetArg (args[1], XtNlabel,
			   game->suit[suit_index].rank[rank_index].symbol) ;
#ifndef NO_LOGGING
if (logerr)fprintf(logerr,"XtSetValues :   undiscard_selected(%d,%d) [%d:%d]\n",
player_index, i, suit_index, game->suit[suit_index].color[player_index]) ;
#endif
	XtSetValues (game->x.discards[player_index][i], args, 2) ;
	XcuDeckRaiseWidget(game->x.discard_bitmap[player_index][i],
	   game->x.discard_suit[player_index][i][suit_index]);
	}
    }

/*
 * Now, add the card back into the hand
 */

pvt_history[player_index].n_in_suit[deck_suit_index]++ ;
order_hand (player_index) ;
show_deal (player_index) ;

if (player->mode == ConfirmDMode)
    {
    player->mode = DiscardMode ;
    if (game->player[player_index].physiology == HumanPhysiology)
	{
	XcuDeckRaiseWidget (game->x.exchange_deck[player_index],
			    game->x.exchange_discard[player_index]) ;
	}
    }
return ;
}

/*
    if (!game->player[i].exchanged)
 * client_data[0] : game pointer
 * client_data[1] : player number
 * client_data[2] : discard number
 */

void
discard_picked (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int i_player = (int) data[0] ;
int discard_number = (int) data[1] ;
Player *player = &game->player[i_player] ;

#ifndef NO_LOGGING
if (logerr)fprintf (logerr, "Discard Picked (%d)\n", discard_number) ;
#endif

switch (game->player[i_player].mode)
  {
  case ConfirmDMode :
  case DiscardMode :
    {
    undiscard_selected (data) ;
    break ;
    }
  case ConfirmRMode :
    {
    err_message ("You must acknowledge receipt!", i_player) ;
    player->error_mode = ClearOnReceipt ;
    break ;
    }
  case NullMode :
    {
    err_message ("Patience, Patience!", i_player) ;
    player->error_mode = ClearOnLegalPlay ;
    break ;
    }
  }
return ;
}

/*
 * client_data[0] : game pointer
 * client_data[1] : player number
 */

void
confirm_discard (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int player_index = (int) data[0] ;
int passer_index = game->player[player_index].pass_from ;
char text_string[100] ;

game->player[player_index].mode = NullMode ;
game->player[player_index].discard_confirmed = True ;

proc_message (" ", player_index) ;

if (!game->player[passer_index].discard_confirmed)
    {
    if (game->player[player_index].physiology == HumanPhysiology)
	{
	sprintf (text_string, "Waiting for\n%s's\ndiscards",
		 game->player[passer_index].name_string) ;
	XcuWlmSetValue (game->x.wlm_id[player_index],
			"XcuLabel", "waiting", XtNlabel, text_string) ;

	XcuDeckRaiseWidget (game->x.exchange_deck[player_index],
			    game->x.exchange_waiting[player_index]) ;
	}
    }
return ;
}

void
confirm_receipt (card, client_data, call_data)
    Widget card ;
    XPointer client_data ;
    XPointer call_data ;
{
long *data = (long *) client_data ;
int player_index = (int) data[0] ;
Player *player = &game->player[player_index] ;

if (player->error_mode == ClearOnReceipt)
    {
    err_message (" ", player_index) ;
    player->error_mode = NullClear ;
    }

player->mode = NullMode ;
player->n_chosen = 0 ;
order_hand (player_index) ;
show_deal (player_index) ;
player->exchange_confirmed = True ;
if (game->player[player_index].physiology == HumanPhysiology)
    {
    XcuDeckRaiseWidget (game->x.table_deck[player_index],
			game->x.play_tbl[player_index]) ;
    }
return ;
}

static void
get_computer_discards ()
{
int i ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	{
	(*game->player[i].discard_program)(&pvt_history[i], i) ;
        game->player[i].discard_confirmed = True ;
	}
    }
return ;
}

