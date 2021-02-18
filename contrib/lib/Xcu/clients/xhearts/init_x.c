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
#include <sys/ioctl.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xfuncs.h>
#include <X11/Xcu/Wlm.h>
#include "hearts.h"

FILE *logerr ;

extern void moon_decision () ;
extern void hand_decision () ;
extern void card_picked () ;
extern void discard_picked () ;
extern void confirm_discard () ;
extern void confirm_receipt () ;
static void display_names () ;
static void install_callbacks () ;
static void get_widget_ids () ;
static void get_suit_colors () ;
static void find_program () ;
static void input_error () ;
static void syntax_error () ;
static void AddStringToPassingStyleConverter () ;
XtErrorHandler handler () ;
int Xhandler () ;

static XrmOptionDescRec options[] =
 {
  {"-goal",	"*goal", 		XrmoptionSepArg, NULL }
 ,{"-jack",	"*jackCounts",		XrmoptionNoArg, "True" }
 ,{"-pass",	"*passingStyle",	XrmoptionSepArg, NULL }
 ,{"-drop",	"*dropOnFirst",		XrmoptionNoArg, "True" }
 ,{"-players",	"*playerFile",		XrmoptionSepArg, NULL }
 ,{"-wlm",	"*wlm",			XrmoptionSepArg, NULL }
 ,{"-seed",	"*seed",		XrmoptionSepArg, NULL }
 ,{"-sleep",	"*sleep",		XrmoptionSepArg, NULL }
 ,{"-log",	"*logFile",		XrmoptionSepArg, NULL }
 } ;

static int	def_goal = 100 ;
static Boolean	def_false = False ;

#define XtNgoal	"goal"
#define XtCGoal	"Goal"

#define XtNpassingStyle	"passingStyle"
#define XtCPassingStyle	"PassingStyle"
#define XtRPassingStyle	"PassingStyle"

#define XtNdropOnFirst	"dropOnFirst"
#define XtCDropOnFirst	"dropOnFirst"

#define XtNjackCounts	"jackCounts"
#define XtCJackCounts	"JackCounts"

#define XtNseed		"seed"
#define XtCNrandSeed	"NrandSeed"

#define XtNsleep	"sleep"
#define XtCSleep	"Sleep"

#define XtNplayerFile	"playerFile"
#define XtCPlayerFile	"PlayerFile"

#define XtNwlm		"wlm"
#define XtCWlmFile	"WlmFile"

#define XtNlogFile	"logFile"
#define XtCLogFile	"LogFile"

#define offset(name,field) XtOffset(name *,field)

static XtResource history_resources[] =
    {
     {XtNgoal, XtCGoal, XtRInt, sizeof(int),
	offset(History, goal), XtRInt, (XPointer)&def_goal}
    ,{XtNpassingStyle, XtCPassingStyle, XtRPassingStyle, sizeof(PassingStyle),
	offset(History, passing_style), XtRString, (XPointer)"LeftRightAcross"}
    ,{XtNdropOnFirst, XtCDropOnFirst, XtRBoolean, sizeof(Boolean),
	offset(History, points_on_first_trick), XtRBoolean, (XPointer)&def_false}
    ,{XtNjackCounts, XtCJackCounts, XtRBoolean, sizeof(Boolean),
	offset(History, jack_counts), XtRBoolean, (XPointer)&def_false}
    } ;

static XtResource game_resources[] =
    {
     {XtNplayerFile, XtCPlayerFile, XtRString, sizeof(String),
	offset(Game, player_file), XtRString, (XPointer)"players"}
    ,{XtNlogFile, XtCLogFile, XtRString, sizeof(String),
	offset(Game, log_file), XtRString, (XPointer)NULL}
    ,{XtNwlm, XtCWlmFile, XtRString, sizeof(String),
	offset(Game, wlm_file), XtRString, (XPointer)"hearts"}
    ,{XtNseed, XtCNrandSeed, XtRString, sizeof(String),
	offset(Game, seed), XtRString, (XPointer)NULL}
    ,{XtNsleep, XtCSleep, XtRInt, sizeof(int),
	offset(Game, sleep_seconds), XtRString, (XPointer)"2"}
    } ;

void
init_x_phase_1 (argc, argv)
    int argc ;
    char **argv ;
{
unsigned int saved_argc ;
char **saved_argv ;
Arg wlm_arg ;
int i, j, n ;
Arg args[2] ;
FILE *fd = (FILE *) NULL ;
XStruct *x = &game->x ;
int noblock = 1 ;

XtToolkitInitialize() ;
x->app = XtCreateApplicationContext () ;
XtSetErrorHandler (handler) ;
XSetIOErrorHandler (handler) ;
XSetErrorHandler (Xhandler) ;

saved_argc = argc ;
saved_argv = (char **)XtMalloc (argc * sizeof(char *)) ;
bcopy (argv, saved_argv, argc * sizeof(char *)) ;

/*
 * I have to search for the -players flag myself!
 * The options list can only be specified in calls that open a display,
 * and the players file contains the display info :-(
 */

for (i=1;  i < argc-1; i++)
    {
    if (strcmp ("-players", argv[i]) == 0)
	{
	fd = fopen (argv[i+1], "r") ; 
	if (!fd)
	    input_error (argv[i+1], argv) ;
	break ;
	}
    }

if (fd == (FILE *) NULL)
    {
    fd = fopen ("players", "r") ;
    if (!fd)
	input_error ("players", argv) ;
    }

logerr = stderr ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    char name_string[40] ;
    char display_string[40] ;
    for (j=0;j<40;j++)name_string[j]=0 ;
    fscanf (fd, "%s", name_string) ;
    game->player[i].name_string = XtNewString(name_string) ;
    if (game->player[i].name_string[0] == ':')
	{
	game->player[i].physiology = ComputerPhysiology ;
	find_program (i) ;
	}
    else
	game->player[i].physiology = HumanPhysiology ;
    fscanf (fd, "%s", display_string) ;
    if (game->player[i].physiology == HumanPhysiology)
	{
	fprintf (logerr, "Opening a connection to (%s)\n", display_string) ;
	x->dpy[i] = XtOpenDisplay (x->app, display_string, "xhearts", "XHearts",
				    options, XtNumber(options), &argc, argv ) ;
	if (!x->dpy[i])
	    {
	    fprintf (logerr, "Couldn't open display (%s)\n", display_string) ;
	    exit (1) ;
	    }
	n=0;
	XtSetArg(args[n], XtNargc, saved_argc) ; n++ ;
	XtSetArg(args[n], XtNargv, saved_argv) ; n++ ;
	x->top[i] = XtAppCreateShell ("xhearts", "XHearts",
				    applicationShellWidgetClass,
				    x->dpy[i], args, n) ;
	if (i == 0)
	    {
	    if (argc != 1)
		syntax_error (argv) ;
	    AddStringToPassingStyleConverter () ;
	    XtGetApplicationResources (game->x.top[0], game,
				game_resources, XtNumber(game_resources),
				NULL, 0) ;
	    XtGetApplicationResources (game->x.top[0], history,
				history_resources, XtNumber(history_resources),
				NULL, 0) ;
	    }
	XtSetArg (wlm_arg, XtNfile, game->wlm_file) ;
fprintf (stderr, "Using (%s) as the compiled layout file\n", game->wlm_file) ;
	sprintf (name_string, "wlm%d", i) ;
	x->wlm_id[i] = XtCreateManagedWidget ( name_string, xcuWlmWidgetClass,
						x->top[i], &wlm_arg, 1 ) ;
	XtRealizeWidget (x->top[i]) ;
	}
    }

if (game->log_file)
    logerr = fopen (game->log_file, "w") ;
else
    logerr = NULL ;

return ;
}

void
init_x_phase_2 ()
{
display_names () ;
install_callbacks () ;
get_widget_ids () ;
get_suit_colors () ;

return ;
}

static void
get_suit_colors ()
{
int i, j ;
for (i=0;  i < N_PLAYERS;  i++)
    {
    static String suit_colors[] = { "Black", "Red", "Red", "Black" } ;
    XrmValue string_value ;
    XrmValue pixel_value ;
    game->suit[i].color_string = XtNewString (suit_colors[i]) ;
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_SUITS;  j++)
	{
	string_value.addr = suit_colors[j] ;
	string_value.size = strlen(suit_colors[j]) ;
	XtConvert ((Widget)game->x.top[i], XtRString, &string_value,
			           XtRPixel, &pixel_value) ;
	game->suit[j].color[i] = *((Pixel *)pixel_value.addr) ; 
	}
    }
return ;
}

static void
get_widget_ids ()
{
int i,j,k ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    game->x.table_deck[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "table") ;
    game->x.play_tbl[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuTbl", "play") ;
    game->x.exchange_tbl[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuTbl", "exchange") ;
    game->x.exchange_deck[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "exchange") ;
    game->x.exchange_discard[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "discard") ;
    game->x.exchange_waiting[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "waiting") ;
    game->x.exchange_confirm_d[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuCommand", "confirm_discard") ;
    game->x.exchange_confirm_r[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuCommand", "confirm_receipt") ;
    game->x.moon_decision[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuTbl", "moon_decision") ;
    game->x.hand_decision[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuTbl", "hand_decision") ;
    game->x.south_bitmap[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "south") ;
    game->x.north_bitmap[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "north") ;
    game->x.west_bitmap[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "west") ;
    game->x.east_bitmap[i] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuDeck", "east") ;
    for (j=0;  j < N_SUITS;  j++)
	{
	char bitmap_string[20] ;
	sprintf (bitmap_string, "south_%s", game->suit[j].name) ;
	game->x.south_suit[i][j] = XcuWlmInquireWidget
			      (game->x.wlm_id[i], "Label", bitmap_string) ;
	sprintf (bitmap_string, "north_%s", game->suit[j].name) ;
	game->x.north_suit[i][j] = XcuWlmInquireWidget
			      (game->x.wlm_id[i], "Label", bitmap_string) ;
	sprintf (bitmap_string, "west_%s", game->suit[j].name) ;
	game->x.west_suit[i][j] = XcuWlmInquireWidget
			      (game->x.wlm_id[i], "Label", bitmap_string) ;
	sprintf (bitmap_string, "east_%s", game->suit[j].name) ;
	game->x.east_suit[i][j] = XcuWlmInquireWidget
			      (game->x.wlm_id[i], "Label", bitmap_string) ;
	}
    game->x.south_suit[i][j] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "south_Blank") ;
    game->x.north_suit[i][j] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "north_Blank") ;
    game->x.west_suit[i][j] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "west_Blank") ;
    game->x.east_suit[i][j] = XcuWlmInquireWidget
			  (game->x.wlm_id[i], "XcuLabel", "east_Blank") ;
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_SUITS;  j++)
	{
	game->x.hands[i][j] = (Widget *) XtMalloc (info->cards_per_suit[j] *
						   sizeof (Widget)) ;
	for (k=0;  k < info->cards_per_suit[j];  k++)
	    {
	    char name_string[40] ;
	    int card_number = game->suit[j].rank[k].card ;
	    int card_rank = info->deck[card_number].rank ;
	    sprintf (name_string, "%s%d", game->suit[j].character, k) ;
	    game->x.hands[i][j][k] =
		XcuWlmInquireWidget
		(game->x.wlm_id[i], "XcuCommand", name_string) ;
	    }
	}
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_PLAYERS;  j++)
	{
	char points_string[20] ;
	char tricks_string[20] ;
	char score_string[20] ;
	char plays_string[20] ;

	sprintf (points_string, "%s_points", game->seat[j]) ;
	sprintf (tricks_string, "%s_tricks", game->seat[j]) ;
	sprintf (score_string, "%s_score", game->seat[j]) ;
	sprintf (plays_string, "%s_card", game->seat[j]) ;

	game->x.points[i][j] =
	    XcuWlmInquireWidget (game->x.wlm_id[i], "XcuLabel", points_string) ;
	game->x.tricks[i][j] =
	    XcuWlmInquireWidget (game->x.wlm_id[i], "XcuLabel", tricks_string) ;
	game->x.score[i][j] =
	    XcuWlmInquireWidget (game->x.wlm_id[i], "XcuLabel", score_string) ;
	game->x.plays[i][j] =
	    XcuWlmInquireWidget (game->x.wlm_id[i], "XcuLabel", plays_string) ;
	}
    }

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    for (j=0;  j < N_DISCARDS;  j++)
	{
	char discard_string[20] ;
	sprintf (discard_string, "discard%d", j) ;
	game->x.discards[i][j] =
	    XcuWlmInquireWidget(game->x.wlm_id[i], "XcuCommand",discard_string);
	sprintf (discard_string, "discard%d", j) ;
	game->x.discard_bitmap[i][j] =
	    XcuWlmInquireWidget(game->x.wlm_id[i], "XcuDeck",discard_string);
	for (k=0;  k < N_SUITS; k++)
	    {
	    sprintf (discard_string, "%s%d", game->suit[k].name, j) ;
	    game->x.discard_suit[i][j][k] =
		XcuWlmInquireWidget(game->x.wlm_id[i], "Label",
				    discard_string);
	    }
	sprintf (discard_string, "Blank%d", j) ;
	game->x.discard_suit[i][j][k] =
	    XcuWlmInquireWidget(game->x.wlm_id[i], "XcuLabel",
				discard_string);
	}
    }

return ;
}

static void
install_callbacks ()
{
int i,j,k ;
long *data ;
for (k=0;  k < N_PLAYERS;  k++)
    {
    if (game->player[k].physiology == ComputerPhysiology)
	continue ;
    /*
     * A callback for each of the possible cards in the hand
     */
    for (i=0;  i < N_SUITS;  i++)
	{
	for (j=0;  j < info->cards_per_suit[i];  j++)
	    {
	    char name_string[40] ;
	    sprintf (name_string, "%s%d", game->suit[i].character, j) ;
	    data = (long *) XtMalloc (3 * sizeof(long)) ;
	    data[0] = k ;
	    data[1] = i ;
	    data[2] = j ;
	    XcuWlmAddCallback ( game->x.wlm_id[k], NULL,
			       "XcuCommand", name_string, "callback",
			       card_picked, data ) ;
	    }
	}
    /*
     * A callback for each of the discard positions
     */
    for (i=0;  i < N_DISCARDS;  i++)
	{
	char name_string[40] ;
	sprintf (name_string, "discard%d", i) ;
	data = (long *) XtMalloc (2 * sizeof(long)) ;
	data[0] = k ;
	data[1] = i ;
	XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", name_string,
			    "callback", discard_picked, data ) ;
	}
    /*
     * A callback for each of the 2 exchange commands
     */
    data = (long *) XtMalloc (1 * sizeof(long)) ;
    data[0] = k ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "confirm_discard",
			"callback", confirm_discard, data ) ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "confirm_receipt",
			"callback", confirm_receipt, data ) ;

    /*
     * A callback for each of the 2 shoot the moon decision commands
     */
    data = (long *) XtMalloc (1 * sizeof(long)) ;
    data[0] = LowerMe ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "lower_me",
			"callback", moon_decision, data ) ;
    data = (long *) XtMalloc (1 * sizeof(long)) ;
    data[0] = RaiseOthers ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "raise_others",
			"callback", moon_decision, data ) ;

    /*
     * A callback for each of the 2 hand decision commands
     */
    data = (long *) XtMalloc (2 * sizeof(long)) ;
    data[0] = k ;
    data[1] = AnotherHand ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "another_hand",
			"callback", hand_decision, data ) ;
    data = (long *) XtMalloc (2 * sizeof(long)) ;
    data[0] = k ;
    data[1] = NoMore ;
    XcuWlmAddCallback (game->x.wlm_id[k], NULL, "XcuCommand", "no_more",
			"callback", hand_decision, data ) ;
    }
return ;
}

#include <X11/Xcu/WlmP.h>
#include <X11/Xcu/Tbl.h>
#include <X11/Xcu/Deck.h>
#include <X11/Xcu/Command.h>
#include <X11/Xcu/Label.h>
#include <X11/Xaw/Label.h>

void
make_tag_class_list (ww)
    XcuWlmWidget ww ;
{
TAG_CLASS_ENTRY(ww, "XcuWlm", xcuWlmWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuCommand", xcuCommandWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuTbl", xcuTblWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuDeck", xcuDeckWidgetClass) ;
TAG_CLASS_ENTRY(ww, "XcuLabel", xcuLabelWidgetClass) ;
TAG_CLASS_ENTRY(ww, "Label", labelWidgetClass) ;
return ;
}

void 
refresh_screens ()
{
int i, j ;
XEvent event ;

for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == HumanPhysiology)
	{
#define N_REFRESHES 200
	for (j=0;  j < N_REFRESHES;  j++)
	    XcuWlmEvent (game->x.wlm_id[i]) ;
	}
    }
return ;
}

void 
wait_for_player_events ()
{
int i ;
XEvent event ;

XtAppPeekEvent (game->x.app, &event) ;
for (i=0;  i < N_PLAYERS;  i++)
    {
    if (game->player[i].physiology == HumanPhysiology)
	{
	if (event.xany.display == game->x.dpy[i])
	    {
	    XcuWlmEvent (game->x.wlm_id[i]) ;
	    break ;
	    }
	}
    }
return ;
}

static void
display_names ()
{
int i ;
XStruct *x = &game->x ;

/*
 * Every player sees himself as South
 */

for (i=0;  i < N_PLAYERS;  i++)
    {
    int j = i ;
    if (game->player[i].physiology == ComputerPhysiology)
	continue ;
    XcuWlmSetValue (game->x.wlm_id[i], "XcuLabel", "Splayer",
			XtNlabel, game->player[j].name_string) ;

    if (++j == N_PLAYERS)
	j = 0 ;
    XcuWlmSetValue (game->x.wlm_id[i], "XcuLabel", "Wplayer",
			XtNlabel, game->player[j].name_string) ;

    if (++j == N_PLAYERS)
	j = 0 ;
    XcuWlmSetValue (game->x.wlm_id[i], "XcuLabel", "Nplayer",
			XtNlabel, game->player[j].name_string) ;

    if (++j == N_PLAYERS)
	j = 0 ;
    XcuWlmSetValue (game->x.wlm_id[i], "XcuLabel", "Eplayer",
			XtNlabel, game->player[j].name_string) ;
    }
return ;
}

void
proc_message (msg, player)
    String msg ;
    int player ;
{
if (game->player[player].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[player], "XcuLabel", "proc_msg",
			XtNlabel, msg) ;
    }
return ;
}

void
err_message (msg, player)
    String msg ;
    int player ;
{
if (game->player[player].physiology == HumanPhysiology)
    {
    XcuWlmSetValue (game->x.wlm_id[player], "XcuLabel", "err_msg",
			XtNlabel, msg) ;
    }
return ;
}

static void
find_program (player_index)
    int player_index ;
{
extern void discard_program() ;
extern void play_program() ;
/* String description is in &game->player[player_index].name_string[1] */
game->player[player_index].discard_program = discard_program ;
game->player[player_index].play_program = play_program ;
}

static void
input_error (text, argv)
    String text ;
    char **argv ;
{
fprintf (stderr, "%s : Couldn't open file (%s)\n", argv[0], text) ;
exit (1) ;
}

static void
syntax_error (argv)
    char **argv ;
{
fprintf (stderr, "Usage: %s\n", argv[0]) ;
fprintf (stderr, "      -goal %%d\n") ;
fprintf (stderr, "      -jack\n") ;
fprintf (stderr, "      -pass LeftRight     | LeftRightAcross     |\n") ;
fprintf (stderr, "            LeftRightKeep | LeftRightAcrossKeep\n") ;
fprintf (stderr, "      -drop\n") ;
fprintf (stderr, "      -players filename\n") ;
fprintf (stderr, "      -wlm filename\n") ;
#ifdef NRAND48
fprintf (stderr, "      -seed \"%%d %%d %%d\"\n") ;
#else
fprintf (stderr, "      -seed \"%%d\"\n") ;
#endif
fprintf (stderr, "      -sleep \"%%d\"\n") ;
exit (1) ;
}

static void CvtStringToPassingStyle() ;	/* Resource Converter */
static XrmQuark	XrmQElr, XrmQElra, XrmQElrak, XrmQElrk ;/* Passing styles */

static void
AddStringToPassingStyleConverter ()
{
XrmQElr   = XrmStringToQuark("leftright") ;
XrmQElra = XrmStringToQuark("leftrightacross") ;
XrmQElrak  = XrmStringToQuark("leftrightacrosskeep") ;
XrmQElrk  = XrmStringToQuark("leftrightkeep") ;
XtAddConverter (XtRString, XtRPassingStyle, CvtStringToPassingStyle, NULL, 0) ;
return ;
}

#include <ctype.h>

static void CvtStringToPassingStyle (args, num_args, fromVal, toVal)
    XrmValuePtr *args ;		/* unused */
    Cardinal	*num_args ;	/* unused */
    XrmValuePtr fromVal ;
    XrmValuePtr toVal ;
{
static PassingStyle	e ; /* must be static! */
XrmQuark	q ;
char	*s = (char *) fromVal->addr ;

if (s == NULL) return ;

q = XcuSimplifyArg (s, "xcu") ;

toVal->size = sizeof(PassingStyle) ;
toVal->addr = (XPointer) &e ;

if (q == XrmQElr)  { e = LeftRight;		return; }
if (q == XrmQElra) { e = LeftRightAcross;	return; }
if (q == XrmQElrak){ e = LeftRightAcrossKeep;	return; }
if (q == XrmQElrk) { e = LeftRightKeep;		return; }

toVal->size = 0 ;
toVal->addr = NULL ;
return ;
}

XtErrorHandler
handler (message)
    char *message ;
{
fprintf (stderr, "\nHANDLER\n%s\n", message) ;
abort () ;
}

int
Xhandler (dpy, s)
    Display *dpy ;
    XErrorEvent *s ;
{
char buf[200] ;
XGetErrorText (dpy, s->error_code, buf, 200) ;
fprintf (stderr, "\nHANDLER\n%s\n", buf) ;
abort () ;
}

