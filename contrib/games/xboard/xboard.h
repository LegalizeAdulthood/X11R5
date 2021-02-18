/*
 * XBoard -- an Xt/Athena user interface for GNU Chess
 *
 * Dan Sears
 * Chris Sears
 *
 * Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
#define BOARD_SIZE				8				/* rarely changed */
#define BIG_SQUARE_SIZE			80
#define SMALL_SQUARE_SIZE		64
#define LINE_GAP				3
#define MAX_MOVES				512
#define MSG_SIZ					256
#define DIALOG_SIZE				250
#define MOVE_LEN				8
#define TIME_CONTROL			5				/* in minutes */
#define TIME_DELAY				"1.0"			/* seconds between moves */
#define TIME_SEARCH				0				/* don't specify search time */
#define MOVES_PER_SESSION		40				/* moves per TIME_CONTROL */
#define WHITE_ON_MOVE			((int) ((currentMove % 2) == 0))
#define FIRST_CHESS_PROGRAM		"gnuchessx"
#define SECOND_CHESS_PROGRAM	"gnuchessx"
#define FIRST_HOST				"localhost"
#define SECOND_HOST				"localhost"
#define MATCH_MODE				"False"
#define INIT_STRING				"new\nbeep\nrandom\neasy\n"
#define WHITE_STRING			"white\ngo\n"
#define BLACK_STRING			"black\ngo\n"
#define WHITE_PIECE_COLOR		"#FFFFCC"
#define BLACK_PIECE_COLOR		"#202020"
#define LIGHT_SQUARE_COLOR		"#C8C365"
#define DARK_SQUARE_COLOR		"#77A26D"

typedef enum {
	BeginningOfGame, MachinePlaysWhite, MachinePlaysBlack, TwoMachinesPlay,
	ForceMoves, PlayFromGameFile, SetupPosition, PauseGame, EndOfGame
} GameMode;

typedef enum {
	MatchFalse, MatchInit, MatchPosition, MatchOpening
} MatchMode;

typedef enum {
	WhitePawn, WhiteRook, WhiteKnight, WhiteBishop, WhiteQueen, WhiteKing,
	BlackPawn, BlackRook, BlackKnight, BlackBishop, BlackQueen, BlackKing,
	EmptySquare
} ChessSquare;

typedef ChessSquare Board[BOARD_SIZE][BOARD_SIZE];

typedef enum {
	WhiteKingSideCastle = 1, WhiteQueenSideCastle,
	WhitePromotionKnight, WhitePromotionBishop,
	WhitePromotionRook, WhitePromotionQueen,
	BlackPromotionKnight, BlackPromotionBishop,
	BlackPromotionRook, BlackPromotionQueen,
	BlackKingSideCastle, BlackQueenSideCastle,
	WhiteCapturesEnPassant, BlackCapturesEnPassant, NormalMove,
	WhiteWins, BlackWins, GameIsDrawn, StartGame, BadMove, Comment
} ChessMove;

typedef enum {			/* Same order as buttonStrings */
	ButtonQuit, ButtonPlayFromFile, ButtonMachinePlaysBlack, ButtonForward,
	ButtonReset, ButtonSetupFromFile, ButtonMachinePlaysWhite, ButtonBackward,
	ButtonFlip, ButtonSaveGame, ButtonSwitch, ButtonForce,
	ButtonHint, ButtonSavePosition, ButtonTwoMachinesPlay, ButtonPause
} Button;

typedef enum {
	ResetTimers, DisplayTimers, StopTimers, ReStartTimers
} ClockMode;

/*
 * Various compatibility grunge
 */
#ifdef __STDC__
#define	P(args)		args
#define VOID_PROC	(void(*)(char*))
#else
#define P(args)		()
#define VOID_PROC
#endif
