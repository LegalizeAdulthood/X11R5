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


#include <X11/Intrinsic.h>
#include "hearts.h"

Game _game, *game = &_game ;
History _history, *history = &_history ;
PvtHistory pvt_history[N_PLAYERS] ;
Snap _snap, *snap = &_snap ;
Info _info, *info = &_info ;

main(argc, argv)
    int argc ;
    char **argv ;
{
void initialize() ;
void deal() ;
void exchange() ;
void trick() ;
void cleanup() ;
void score_hand() ;
void determine_lead() ;
void play() ;
Boolean	game_over() ;
Boolean	hand_over() ;

initialize (argc, argv) ;

while (!game_over())
    {
    deal () ;
    exchange () ;
    while (!hand_over ())
	{
	determine_lead () ;
	if (game->conceded)
	    continue ;
	trick () ;
	}
    score_hand () ;
    request_another_hand () ;
    }

cleanup() ;
}

Boolean 
game_over ()
{
int i ;
/*
 * The game is over whenever one person quits
 */
if (game->hand_decision == NoMore)
    return True ;
/*
 * The game is over whenever one person has reached the goal score
 */
for (i=0;  i <  N_PLAYERS;  i++)
    if (game->player[i].score >= history->goal)
	return True ;

return False ;
}

void
cleanup ()
{
/* Show the results */
/* Sleep, quit */
/*
while (1) wait_for_player_events () ;
*/
}

/* TODO : Shooting */
/* TODO : further macroize computer strategy */
/* TODO : ifdef'd Xaw,Xm version */
/* TODO : overridable colors ? */
/* TODO : randomize seat positions each hand (option) */
/* TODO : option to conceal identity of opponents */
/* TODO : history mechanism (record of every game) */
/* TODO : "duplicate" */
/* TODO : bitmaps */
