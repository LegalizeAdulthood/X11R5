/*
 * XBoard -- an Xt/Athena user interface for GNU Chess
 *
 * Dan Sears
 * Chris Sears
 *
 * XBoard borrows its colors, icon and piece bitmaps from XChess
 * which was written and is copyrighted by Wayne Christopher.
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
 *
 * Revision 1.0 90/10/31
 *		Initial release.
 *
 * Revision 1.1 91/01/26
 *		Major bug fix release.
 *
 * Revision 1.2 91/06/11
 *		Another major bug fix release.
 *		lex game file parser.
 *		Popup dialogs for file names.
 *		SYSV support.
 *
 * Revision 1.3 91/10/05
 *		Added gnuchess version 3.1+ patch level 3 to source tree for R5.
 */

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#ifdef	__STDC__
#include <stdlib.h>
#endif
#if	SYSTEM_FIVE || SYSV
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include <pwd.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/List.h>
#include <X11/Xaw/Label.h>
#include <X11/cursorfont.h>
#include "xboard.h"

#include "bitmaps/s_p.bm"
#include "bitmaps/s_r.bm"
#include "bitmaps/s_kt.bm"
#include "bitmaps/s_b.bm"
#include "bitmaps/s_q.bm"
#include "bitmaps/s_k.bm"

#include "bitmaps/ol_p.bm"
#include "bitmaps/ol_r.bm"
#include "bitmaps/ol_kt.bm"
#include "bitmaps/ol_b.bm"
#include "bitmaps/ol_q.bm"
#include "bitmaps/ol_k.bm"

#include "bitmaps/p_sm.bm"
#include "bitmaps/r_sm.bm"
#include "bitmaps/kt_sm.bm"
#include "bitmaps/b_sm.bm"
#include "bitmaps/q_sm.bm"
#include "bitmaps/k_sm.bm"

#include "bitmaps/p_sm_ol.bm"
#include "bitmaps/r_sm_ol.bm"
#include "bitmaps/kt_sm_ol.bm"
#include "bitmaps/b_sm_ol.bm"
#include "bitmaps/q_sm_ol.bm"
#include "bitmaps/k_sm_ol.bm"

#include "bitmaps/icon.bm"

void main P((int argc, char **argv));
void CreateGCs P((void));
void CreatePieces P((void));
void ReadBitmap P((String name, Pixmap *pm,
	char big_bits[], char small_bits[]));
void CreateGrid P((void));
int EventToSquare P((int x));
ChessSquare CharToPiece P((int c));
void DrawSquare P((int row, int column, ChessSquare piece));
void EventProc P((Widget widget, caddr_t unused, XEvent *event));
void DrawPosition P((Widget w, XExposeEvent *event));
void InitPosition P((void));
void CopyBoard P((Board to, Board from));
void SendCurrentBoard P((FILE *fp));
void HandleUserMove P((Widget w, XEvent *event));
void HandleMachineMove P((char *message));
void ReadGameFile P((void));
int ReadGameFileProc P((void));
void MakeMove P((ChessMove *move_type, int from_x, int from_y,
	int to_x, int to_y));
void InitChessProgram P((char *host_name, char *program_name, int *pid,
	FILE **to, FILE **from, XtIntervalId *xid));
void ShutdownChessPrograms P((char *message));
void CommentPopUp P((char *label));
void FileNamePopUp P((char *label, void (*proc)(char *name)));
void FileNameCallback P((Widget w, XtPointer client_data, XtPointer call_data));
void FileNameAction P((Widget w, XEvent *event));
void SelectCommand P((Widget w, XtPointer client_data, XtPointer call_data));
void GameProc P((void));
void QuitProc P((void));
int PlayFromGameFileProc P((char *name));
void MachinePlaysBlackProc P((void));
void ForwardProc P((void));
void ResetFileProc P((void));
void ResetProc P((void));
int SetupPositionFromFileProc P((char *name));
void MachinePlaysWhiteProc P((void));
void BackwardProc P((void));
void FlipProc P((void));
void SaveGameProc P((char *name));
void SwitchProc P((void));
void ForceProc P((void));
void HintProc P((void));
void SavePositionProc P((char *name));
void TwoMachinesPlayProc P((void));
void PauseProc P((void));
void Iconify P((void));
void SendToProgram P((char *message, FILE *fp));
void ReceiveFromProgram P((FILE *fp));
void DisplayMessage P((char *message));
void DisplayClocks P((int clock_mode));
void DisplayTimerLabel P((Widget w, char *color, time_t timer));
char *TimeString P((time_t tm));
void Usage P((void));
char *StrStr P((char *string, char *match));
#if	SYSTEM_FIVE || SYSV
char *PseudoTTY P((int *ptyv));
#else
void CatchPipeSignal P((void));
#endif
extern int yylex P((void));

/*
 * XBoard depends on Xt R4 or higher
 */
int xtVersion = XtSpecificationRelease;

int xScreen;
Display *xDisplay;
Window xBoardWindow;
GC lightSquareGC, darkSquareGC, lineGC, wdPieceGC, wlPieceGC,
	bdPieceGC, blPieceGC, wbPieceGC, bwPieceGC;
Pixmap solidPawnBitmap, solidRookBitmap, solidKnightBitmap, solidBishopBitmap,
	solidQueenBitmap, solidKingBitmap, outlinePawnBitmap, outlineRookBitmap,
	outlineKnightBitmap, outlineBishopBitmap, outlineQueenBitmap,
	outlineKingBitmap, iconPixmap;
Widget shellWidget, formWidget, boardWidget, commandsWidget, messageWidget,
	whiteTimerWidget, blackTimerWidget, nameWidget, widgetList[6], commentShell;
XSegment gridSegments[(BOARD_SIZE + 1) * 2];
XtIntervalId firstProgramXID = NULL, secondProgramXID = NULL,
	readGameXID = NULL, timerXID = NULL;
XFontStruct *labelFont;
XtAppContext appContext;
XtCallbackProc fileProc;

FILE *fromFirstProgFP, *toFirstProgFP, *fromSecondProgFP,
	*toSecondProgFP, *gameFileFP;
int currentMove = 0, forwardMostMove = 0, firstProgramPID = 0,
	secondProgramPID = 0, squareSize = BIG_SQUARE_SIZE, fromX = -1,
	fromY = -1, firstMove = True, flipView = False, forwardForce = False,
	twoProgramState = False, undoMode = False, xboardDebug,
	commentUp = False;
unsigned long timerForegroundPixel, timerBackgroundPixel;
MatchMode matchMode = MatchFalse;
GameMode gameMode = BeginningOfGame, lastGameMode = BeginningOfGame;
char moveList[MAX_MOVES][MOVE_LEN], parseList[MAX_MOVES][MOVE_LEN * 2],
	ptyname[24], *ttyname, *chessDir, *programName;

time_t whiteTimeRemaining, blackTimeRemaining;
extern char currentMoveString[];
extern char yytext[];

Board boards[MAX_MOVES], initialPosition = {
	{ WhiteRook, WhiteKnight, WhiteBishop, WhiteQueen,
		WhiteKing, WhiteBishop, WhiteKnight, WhiteRook },
	{ WhitePawn, WhitePawn, WhitePawn, WhitePawn,
		WhitePawn, WhitePawn, WhitePawn, WhitePawn },
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare,
		EmptySquare, EmptySquare, EmptySquare, EmptySquare },
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare,
		EmptySquare, EmptySquare, EmptySquare, EmptySquare },
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare,
		EmptySquare, EmptySquare, EmptySquare, EmptySquare },
	{ EmptySquare, EmptySquare, EmptySquare, EmptySquare,
		EmptySquare, EmptySquare, EmptySquare, EmptySquare },
	{ BlackPawn, BlackPawn, BlackPawn, BlackPawn,
		BlackPawn, BlackPawn, BlackPawn, BlackPawn },
	{ BlackRook, BlackKnight, BlackBishop, BlackQueen,
		BlackKing, BlackBishop, BlackKnight, BlackRook }
};

String buttonStrings[] = {
	"Quit", "Play From File", "Machine Black", "Forward",
	"Reset", "Setup From File", "Machine White", "Backward",
	"Flip View", "Save Game", "Switch Sides", "Force Moves",
	"Hint", "Save Position", "Two Machines", "Pause"
};

Arg shellArgs[] = {
	{ XtNwidth, 0 },
	{ XtNheight, 0 },
	{ XtNminWidth, 0 },
	{ XtNminHeight, 0 },
	{ XtNmaxWidth, 0 },
	{ XtNmaxHeight, 0 }
};

Arg boardArgs[] = {
	{ XtNborderWidth, 0 },
	{ XtNwidth, LINE_GAP + BOARD_SIZE * (BIG_SQUARE_SIZE + LINE_GAP) },
	{ XtNheight, LINE_GAP + BOARD_SIZE * (BIG_SQUARE_SIZE + LINE_GAP) }
};

Arg commandsArgs[] = {
	{ XtNborderWidth, 0 },
	{ XtNdefaultColumns, 4 },
	{ XtNforceColumns, True },
	{ XtNlist, (int) buttonStrings },
	{ XtNnumberStrings, XtNumber(buttonStrings) }
};

Arg messageArgs[] = {
	{ XtNborderWidth, 0 },
	{ XtNwidth, 250 },
	{ XtNjustify, (int) XtJustifyLeft }
};

Arg timerArgs[] = {
	{ XtNborderWidth, 0 },
	{ XtNjustify, (int) XtJustifyLeft }
};

Arg nameArgs[] = {
	{ XtNborderWidth, 0 },
	{ XtNwidth, 300 },
	{ XtNjustify, (int) XtJustifyLeft }
};

typedef struct {
	Pixel whitePieceColor;
	Pixel blackPieceColor;
	Pixel lightSquareColor;
	Pixel darkSquareColor;
	int movesPerSession;
	String initString;
	String whiteString;
	String blackString;
	String firstChessProgram;
	String secondChessProgram;
	String firstHost;
	String secondHost;
	String solidPawnBitmap;
	String solidRookBitmap;
	String solidBishopBitmap;
	String solidKnightBitmap;
	String solidQueenBitmap;
	String solidKingBitmap;
	String outlinePawnBitmap;
	String outlineRookBitmap;
	String outlineBishopBitmap;
	String outlineKnightBitmap;
	String outlineQueenBitmap;
	String outlineKingBitmap;
	String remoteShell;
	float timeDelay;
	int timeControl;
	String readGameFile;
	String readPositionFile;
	String saveGameFile;
	String savePositionFile;
	String matchMode;
	Boolean monoMode;
	Boolean debugMode;
	ClockMode clockMode;
	Boolean bigSizeMode;
	int searchTime;
} AppData, *AppDataPtr;

AppData appData;

XtResource clientResources[] = {
	{
		"whitePieceColor", "WhitePieceColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, whitePieceColor), XtRString, WHITE_PIECE_COLOR
	}, {
		"blackPieceColor", "BlackPieceColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, blackPieceColor), XtRString, BLACK_PIECE_COLOR
	}, {
		"lightSquareColor", "LightSquareColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, lightSquareColor), XtRString, LIGHT_SQUARE_COLOR
	}, {
		"darkSquareColor", "DarkSquareColor", XtRPixel, sizeof(Pixel),
		XtOffset(AppDataPtr, darkSquareColor), XtRString, DARK_SQUARE_COLOR
	}, {
		"movesPerSession", "movesPerSession", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, movesPerSession), XtRImmediate,
		(XtPointer) MOVES_PER_SESSION
	}, {
		"initString", "initString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, initString), XtRString, INIT_STRING
	}, {
		"whiteString", "whiteString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, whiteString), XtRString, WHITE_STRING
	}, {
		"blackString", "blackString", XtRString, sizeof(String),
		XtOffset(AppDataPtr, blackString), XtRString, BLACK_STRING
	}, {
		"firstChessProgram", "firstChessProgram", XtRString, sizeof(String),
		XtOffset(AppDataPtr, firstChessProgram), XtRString, FIRST_CHESS_PROGRAM
	}, {
		"secondChessProgram", "secondChessProgram", XtRString, sizeof(String),
		XtOffset(AppDataPtr, secondChessProgram), XtRString,
		SECOND_CHESS_PROGRAM
	}, {
		"firstHost", "firstHost", XtRString, sizeof(String),
		XtOffset(AppDataPtr, firstHost), XtRString, FIRST_HOST
	}, {
		"secondHost", "secondHost", XtRString, sizeof(String),
		XtOffset(AppDataPtr, secondHost), XtRString, SECOND_HOST
	}, {
		"solidPawnBitmap", "solidPawnBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidPawnBitmap), XtRString, NULL
	}, {
		"solidRookBitmap", "solidRookBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidRookBitmap), XtRString, NULL
	}, {
		"solidKnightBitmap", "solidKnightBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidKnightBitmap), XtRString, NULL
	}, {
		"solidBishopBitmap", "solidBishopBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidBishopBitmap), XtRString, NULL
	}, {
		"solidQueenBitmap", "solidQueenBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidQueenBitmap), XtRString, NULL
	}, {
		"solidKingBitmap", "solidKingBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, solidKingBitmap), XtRString, NULL
	}, {
		"outlinePawnBitmap", "outlinePawnBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlinePawnBitmap), XtRString, NULL
	}, {
		"outlineRookBitmap", "outlineRookBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlineRookBitmap), XtRString, NULL
	}, {
		"outlineKnightBitmap", "outlineKnightBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlineKnightBitmap), XtRString, NULL
	}, {
		"outlineBishopBitmap", "outlineBishopBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlineBishopBitmap), XtRString, NULL
	}, {
		"outlineQueenBitmap", "outlineQueenBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlineQueenBitmap), XtRString, NULL
	}, {
		"outlineKingBitmap", "outlineKingBitmap", XtRString, sizeof(String),
		XtOffset(AppDataPtr, outlineKingBitmap), XtRString, NULL
	}, {
		"remoteShell", "remoteShell", XtRString, sizeof(String),
		XtOffset(AppDataPtr, remoteShell), XtRString, "rsh"
	}, {
		"timeDelay", "timeDelay", XtRFloat, sizeof(float),
		XtOffset(AppDataPtr, timeDelay), XtRString, (XtPointer) TIME_DELAY
	}, {
		"timeControl", "timeControl", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, timeControl), XtRImmediate,
		(XtPointer) TIME_CONTROL
	}, {
		"readGameFile", "readGameFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, readGameFile), XtRString, NULL
	}, {
		"readPositionFile", "readPositionFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, readPositionFile), XtRString, NULL
	}, {
		"saveGameFile", "saveGameFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, saveGameFile), XtRString, ""
	}, {
		"savePositionFile", "savePositionFile", XtRString, sizeof(String),
		XtOffset(AppDataPtr, savePositionFile), XtRString, ""
	}, {
		"matchMode", "matchMode", XtRString, sizeof(String),
		XtOffset(AppDataPtr, matchMode), XtRString, MATCH_MODE
	}, {
		"monoMode", "monoMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, monoMode), XtRImmediate, (XtPointer) False
	}, {
		"debugMode", "debugMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, debugMode), XtRImmediate, (XtPointer) False
	}, {
		"clockMode", "clockMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, clockMode), XtRImmediate, (XtPointer) True
	}, {
		"bigSizeMode", "bigSizeMode", XtRBoolean, sizeof(Boolean),
		XtOffset(AppDataPtr, bigSizeMode), XtRImmediate, (XtPointer) True
	}, {
		"searchTime","searchTime", XtRInt, sizeof(int),
		XtOffset(AppDataPtr, searchTime), XtRImmediate, (XtPointer) TIME_SEARCH
	}
};

Pixmap *pieceToSolid[] = {
	&solidPawnBitmap, &solidRookBitmap, &solidKnightBitmap,
	&solidBishopBitmap, &solidQueenBitmap, &solidKingBitmap,
	&solidPawnBitmap, &solidRookBitmap, &solidKnightBitmap,
	&solidBishopBitmap, &solidQueenBitmap, &solidKingBitmap
};

Pixmap *pieceToOutline[] = {
	&outlinePawnBitmap, &outlineRookBitmap, &outlineKnightBitmap,
	&outlineBishopBitmap, &outlineQueenBitmap, &outlineKingBitmap,
	&outlinePawnBitmap, &outlineRookBitmap, &outlineKnightBitmap,
	&outlineBishopBitmap, &outlineQueenBitmap, &outlineKingBitmap
};

char pieceToChar[] = {
	'P', 'R', 'N', 'B', 'Q', 'K',
	'p', 'r', 'n', 'b', 'q', 'k', '.'
};

XrmOptionDescRec shellOptions[] = {
	{ "-whitePieceColor", "whitePieceColor", XrmoptionSepArg, NULL },
	{ "-wpc", "whitePieceColor", XrmoptionSepArg, NULL },
	{ "-blackPieceColor", "blackPieceColor", XrmoptionSepArg, NULL },
	{ "-bpc", "blackPieceColor", XrmoptionSepArg, NULL },
	{ "-lightSquareColor", "lightSquareColor", XrmoptionSepArg, NULL },
	{ "-lsc", "lightSquareColor", XrmoptionSepArg, NULL },
	{ "-darkSquareColor", "darkSquareColor", XrmoptionSepArg, NULL },
	{ "-dsc", "darkSquareColor", XrmoptionSepArg, NULL },
	{ "-movesPerSession", "movesPerSession", XrmoptionSepArg, NULL },
	{ "-mps", "movesPerSession", XrmoptionSepArg, NULL },
	{ "-firstChessProgram", "firstChessProgram", XrmoptionSepArg, NULL },
	{ "-fcp", "firstChessProgram", XrmoptionSepArg, NULL },
	{ "-secondChessProgram", "secondChessProgram", XrmoptionSepArg, NULL },
	{ "-scp", "secondChessProgram", XrmoptionSepArg, NULL },
	{ "-firstHost", "firstHost", XrmoptionSepArg, NULL },
	{ "-fh", "firstHost", XrmoptionSepArg, NULL },
	{ "-secondHost", "secondHost", XrmoptionSepArg, NULL },
	{ "-sh", "secondHost", XrmoptionSepArg, NULL },
	{ "-solidPawnBitmap", "solidPawnBitmap", XrmoptionSepArg, NULL },
	{ "-spb", "solidPawnBitmap", XrmoptionSepArg, NULL },
	{ "-solidRookBitmap", "solidRookBitmap", XrmoptionSepArg, NULL },
	{ "-srb", "solidRookBitmap", XrmoptionSepArg, NULL },
	{ "-solidBishopBitmap", "solidBishopBitmap", XrmoptionSepArg, NULL },
	{ "-sbb", "solidBishopBitmap", XrmoptionSepArg, NULL },
	{ "-solidKnightBitmap", "solidKnightBitmap", XrmoptionSepArg, NULL },
	{ "-skb", "solidKnightBitmap", XrmoptionSepArg, NULL },
	{ "-solidQueenBitmap", "solidQueenBitmap", XrmoptionSepArg, NULL },
	{ "-sqb", "solidQueenBitmap", XrmoptionSepArg, NULL },
	{ "-solidKingBitmap", "solidKingBitmap", XrmoptionSepArg, NULL },
	{ "-skb", "solidKingBitmap", XrmoptionSepArg, NULL },
	{ "-outlinePawnBitmap", "outlinePawnBitmap", XrmoptionSepArg, NULL },
	{ "-opb", "outlinePawnBitmap", XrmoptionSepArg, NULL },
	{ "-outlineRookBitmap", "outlineRookBitmap", XrmoptionSepArg, NULL },
	{ "-orb", "outlineRookBitmap", XrmoptionSepArg, NULL },
	{ "-outlineBishopBitmap", "outlineBishopBitmap", XrmoptionSepArg, NULL },
	{ "-obb", "outlineBishopBitmap", XrmoptionSepArg, NULL },
	{ "-outlineKnightBitmap", "outlineKnightBitmap", XrmoptionSepArg, NULL },
	{ "-okb", "outlineKnightBitmap", XrmoptionSepArg, NULL },
	{ "-outlineQueenBitmap", "outlineQueenBitmap", XrmoptionSepArg, NULL },
	{ "-oqb", "outlineQueenBitmap", XrmoptionSepArg, NULL },
	{ "-outlineKingBitmap", "outlineKingBitmap", XrmoptionSepArg, NULL },
	{ "-okb", "outlineKingBitmap", XrmoptionSepArg, NULL },
	{ "-remoteShell", "remoteShell", XrmoptionSepArg, NULL },
	{ "-rsh", "remoteShell", XrmoptionSepArg, NULL },
	{ "-timeDelay", "timeDelay", XrmoptionSepArg, NULL },
	{ "-td", "timeDelay", XrmoptionSepArg, NULL },
	{ "-timeControl", "timeControl", XrmoptionSepArg, NULL },
	{ "-tc", "timeControl", XrmoptionSepArg, NULL },
	{ "-readGameFile", "readGameFile", XrmoptionSepArg, NULL },
	{ "-rgf", "readGameFile", XrmoptionSepArg, NULL },
	{ "-readPositionFile", "readPositionFile", XrmoptionSepArg, NULL },
	{ "-rpf", "readPositionFile", XrmoptionSepArg, NULL },
	{ "-saveGameFile", "saveGameFile", XrmoptionSepArg, NULL },
	{ "-sgf", "saveGameFile", XrmoptionSepArg, NULL },
	{ "-savePositionFile", "savePositionFile", XrmoptionSepArg, NULL },
	{ "-spf", "savePositionFile", XrmoptionSepArg, NULL },
	{ "-matchMode", "matchMode", XrmoptionSepArg, NULL },
	{ "-mm", "matchMode", XrmoptionSepArg, NULL },
	{ "-monoMode", "monoMode", XrmoptionSepArg, NULL },
	{ "-mono", "monoMode", XrmoptionSepArg, NULL },
	{ "-debugMode", "debugMode", XrmoptionSepArg, NULL },
	{ "-debug", "debugMode", XrmoptionSepArg, NULL },
	{ "-clockMode", "clockMode", XrmoptionSepArg, NULL },
	{ "-clock", "clockMode", XrmoptionSepArg, NULL },
	{ "-bigSizeMode", "bigSizeMode", XrmoptionSepArg, NULL },
	{ "-big", "bigSizeMode", XrmoptionSepArg, NULL },
	{ "-searchTime", "searchTime", XrmoptionSepArg, NULL },
	{ "-st", "searchTime", XrmoptionSepArg, NULL }
};

XtActionsRec boardActions[] = {
	{ "DrawPosition", DrawPosition },
	{ "HandleUserMove", HandleUserMove },
	{ "ResetProc", ResetProc },
	{ "ResetFileProc", ResetFileProc },
	{ "GameProc", GameProc },
	{ "QuitProc", QuitProc },
	{ "ForwardProc", ForwardProc },
	{ "BackwardProc", BackwardProc },
	{ "PauseProc", PauseProc },
	{ "Iconify", Iconify },
	{ "FileNameAction", FileNameAction }
};

char translationsTable[] =
	"<Expose>: DrawPosition() \n \
	 <BtnDown>: HandleUserMove() \n \
	 <BtnUp>: HandleUserMove() \n \
	 <Key>r: ResetFileProc() ResetProc() \n \
	 <Key>R: ResetFileProc() ResetProc() \n \
	 <Key>g: GameProc() \n \
	 <Key>G: GameProc() \n \
	 <Key>q: QuitProc() \n \
	 <Key>Q: QuitProc() \n \
	 <Message>WM_PROTOCOLS: QuitProc() \n \
	 <Key>f: ForwardProc() \n \
	 <Key>F: ForwardProc() \n \
	 <Key>b: BackwardProc() \n \
	 <Key>B: BackwardProc() \n \
	 <Key>p: PauseProc() \n \
	 <Key>P: PauseProc() \n \
	 <Key>i: Iconify() \n \
	 <Key>I: Iconify() \n \
	 <Key>c: Iconify() \n \
	 <Key>C: Iconify() \n";

String xboardResources[] = {
	"*font: -*-helvetica-medium-o-normal--*-140-*-*-*-*-*-*\n",
	"*Dialog*value.translations: #override \\n <Key>Return: FileNameAction()",
	NULL
};

void
main(argc, argv)
	int argc;
	char **argv;
{
	XSetWindowAttributes window_attributes;
	char buf[MSG_SIZ];
	Arg args[3];
	int length;

	setbuf(stdout, NULL); setbuf(stderr, NULL);
	programName = argv[0];

	shellWidget = XtAppInitialize(&appContext, "XBoard", shellOptions,
		XtNumber(shellOptions), &argc, argv, xboardResources, NULL, 0);

	if (argc > 1)
		Usage();

	if ((chessDir = (char *) getenv("CHESSDIR")) == NULL)
		chessDir = ".";

	XtGetApplicationResources(shellWidget, &appData, clientResources,
		XtNumber(clientResources), NULL, 0);

	xboardDebug = appData.debugMode;

	/*
	 * Determine matchMode state -- poor man's resource converter
	 */
	if (strcmp(appData.matchMode, "Init") == 0)
		matchMode = MatchInit;
	else if (strcmp(appData.matchMode, "Position") == 0)
		matchMode = MatchPosition;
	else if (strcmp(appData.matchMode, "Opening") == 0)
		matchMode = MatchOpening;
	else if (strcmp(appData.matchMode, "False") == 0)
		matchMode = MatchFalse;
	else {
		fprintf(stderr, "%s: bad matchMode option %s\n",
			appData.matchMode, programName);
		Usage();
	}

	xDisplay = XtDisplay(shellWidget);
	xScreen = DefaultScreen(xDisplay);

	if ((DisplayWidth(xDisplay, xScreen) < 800) ||
		(DisplayHeight(xDisplay, xScreen) < 800))
		appData.bigSizeMode = False;

	if (!appData.bigSizeMode) {
		squareSize = SMALL_SQUARE_SIZE;
		XtSetArg(boardArgs[1], XtNwidth,
			LINE_GAP + BOARD_SIZE * (SMALL_SQUARE_SIZE + LINE_GAP));
		XtSetArg(boardArgs[2], XtNheight,
			LINE_GAP + BOARD_SIZE * (SMALL_SQUARE_SIZE + LINE_GAP));
	}

	/*
	 * Detect if there are not enough colors are available and adapt.
	 */
	if (DefaultDepth(xDisplay, xScreen) <= 2)
		appData.monoMode = True;

	/*
	 * widget hierarchy
	 */
	formWidget = XtCreateManagedWidget("form",
		formWidgetClass, shellWidget, NULL, 0);

		widgetList[0] = whiteTimerWidget = XtCreateWidget("white time:",
			labelWidgetClass, formWidget, timerArgs, XtNumber(timerArgs));

		widgetList[1] = blackTimerWidget = XtCreateWidget("black time:",
			labelWidgetClass, formWidget, timerArgs, XtNumber(timerArgs));

		widgetList[2] = nameWidget = XtCreateWidget("",
			labelWidgetClass, formWidget, nameArgs, XtNumber(nameArgs));

		widgetList[3] = messageWidget = XtCreateWidget("message",
			labelWidgetClass, formWidget, messageArgs, XtNumber(messageArgs));

		widgetList[4] = commandsWidget = XtCreateWidget("commands",
			listWidgetClass, formWidget, commandsArgs, XtNumber(commandsArgs));

		widgetList[5] = boardWidget = XtCreateWidget("board",
			widgetClass, formWidget, boardArgs, XtNumber(boardArgs));

	XtManageChildren(widgetList, XtNumber(widgetList));

	/*
	 * Calculate the width of the timer labels.
	 */
	XtSetArg(args[0], XtNfont, &labelFont);
	XtGetValues(whiteTimerWidget, args, 1);
	if (appData.clockMode) {
		sprintf(buf, "White: %s ", TimeString(appData.timeControl * 60));
		length = XTextWidth(labelFont, buf, strlen(buf) - 1);
	} else
		length = XTextWidth(labelFont, "White  ", 7);
	XtSetArg(args[0], XtNwidth, length);
	XtSetValues(whiteTimerWidget, args, 1);
	XtSetValues(blackTimerWidget, args, 1);

	XtSetArg(args[0], XtNbackground, &timerForegroundPixel);
	XtSetArg(args[1], XtNforeground, &timerBackgroundPixel);
	XtGetValues(whiteTimerWidget, args, 2);

	/*
	 * formWidget uses these constraints but they are stored in the children.
	 */
	XtSetArg(args[0], XtNfromHoriz, whiteTimerWidget);
	XtSetValues(blackTimerWidget, args, 1);
	XtSetArg(args[0], XtNfromHoriz, blackTimerWidget);
	XtSetValues(nameWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, whiteTimerWidget);
	XtSetValues(messageWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, messageWidget);
	XtSetValues(commandsWidget, args, 1);
	XtSetArg(args[0], XtNfromVert, commandsWidget);
	XtSetValues(boardWidget, args, 1);

	XtRealizeWidget(shellWidget);

	xBoardWindow = XtWindow(boardWidget);

	/*
	 * Create an icon.
	 */
	iconPixmap = XCreateBitmapFromData(xDisplay, XtWindow(shellWidget),
		icon_bits, icon_width, icon_height);
	XtSetArg(args[0], XtNiconPixmap, iconPixmap);
	XtSetValues(shellWidget, args, 1);

	/*
	 * Create a cursor for the board widget.
	 */
	window_attributes.cursor = XCreateFontCursor(xDisplay, XC_hand2);
	XChangeWindowAttributes(xDisplay, xBoardWindow,
		CWCursor, &window_attributes);

	/*
	 * Inhibit shell resizing.
	 */
	XtGetValues(shellWidget, shellArgs, 2);
	shellArgs[4].value = shellArgs[2].value = shellArgs[0].value;
	shellArgs[5].value = shellArgs[3].value = shellArgs[1].value;
	XtSetValues(shellWidget, &shellArgs[2], 4);

	CreateGCs();
	CreateGrid();
	CreatePieces();

	XtAddCallback(commandsWidget, XtNcallback, SelectCommand, NULL);
	XtAppAddActions(appContext, boardActions, XtNumber(boardActions));

	XtSetArg(args[0], XtNtranslations,
		XtParseTranslationTable(translationsTable));
	XtSetValues(boardWidget, &args[0], 1);
	XtAddEventHandler(boardWidget, ExposureMask | ButtonPressMask
		| ButtonReleaseMask | Button1MotionMask | KeyPressMask,
		False, EventProc, NULL);

	DisplayMessage("");

	/*
	 * If there is to be a machine match, set it up.
	 */
	if (matchMode != MatchFalse)
		TwoMachinesPlayProc();
	else
		ResetProc();

	XtAppMainLoop(appContext);
}

void
CreateGCs()
{
	XtGCMask value_mask = GCLineWidth | GCLineStyle | GCForeground
		| GCBackground | GCFunction | GCPlaneMask;
	XGCValues gc_values;

	gc_values.plane_mask = AllPlanes;
	gc_values.line_width = LINE_GAP;
	gc_values.line_style = LineSolid;
	gc_values.function = GXcopy;

	gc_values.foreground = XBlackPixel(xDisplay, xScreen);
	gc_values.background = XBlackPixel(xDisplay, xScreen);
	lineGC = XtGetGC(shellWidget, value_mask, &gc_values);

	if (appData.monoMode) {
		gc_values.foreground = XWhitePixel(xDisplay, xScreen);
		gc_values.background = XBlackPixel(xDisplay, xScreen);
		lightSquareGC = wbPieceGC
			= XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = XBlackPixel(xDisplay, xScreen);
		gc_values.background = XWhitePixel(xDisplay, xScreen);
		darkSquareGC = bwPieceGC
			= XtGetGC(shellWidget, value_mask, &gc_values);
	} else {
		gc_values.foreground = appData.lightSquareColor;
		gc_values.background = appData.darkSquareColor;
		lightSquareGC = XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = appData.darkSquareColor;
		gc_values.background = appData.lightSquareColor;
		darkSquareGC = XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = appData.whitePieceColor;
		gc_values.background = appData.darkSquareColor;
		wdPieceGC = XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = appData.whitePieceColor;
		gc_values.background = appData.lightSquareColor;
		wlPieceGC = XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = appData.blackPieceColor;
		gc_values.background = appData.darkSquareColor;
		bdPieceGC = XtGetGC(shellWidget, value_mask, &gc_values);

		gc_values.foreground = appData.blackPieceColor;
		gc_values.background = appData.lightSquareColor;
		blPieceGC = XtGetGC(shellWidget, value_mask, &gc_values);
	}
}

void
CreatePieces()
{
	XSynchronize(xDisplay, True);	/* Work-around for xlib/xt buffering bug */

	ReadBitmap(appData.solidPawnBitmap, &solidPawnBitmap,
		solid_pawn_bits, pawn_small_bits);
	ReadBitmap(appData.solidRookBitmap, &solidRookBitmap,
		solid_rook_bits, rook_small_bits);
	ReadBitmap(appData.solidKnightBitmap, &solidKnightBitmap,
		solid_knight_bits, knight_small_bits);
	ReadBitmap(appData.solidBishopBitmap, &solidBishopBitmap,
		solid_bishop_bits, bishop_small_bits);
	ReadBitmap(appData.solidQueenBitmap, &solidQueenBitmap,
		solid_queen_bits, queen_small_bits);
	ReadBitmap(appData.solidKingBitmap, &solidKingBitmap,
		solid_king_bits, king_small_bits);

	if (appData.monoMode) {
		ReadBitmap(appData.outlinePawnBitmap, &outlinePawnBitmap,
			outline_pawn_bits, pawn_small_outline_bits);
		ReadBitmap(appData.outlineRookBitmap, &outlineRookBitmap,
			outline_rook_bits, rook_small_outline_bits);
		ReadBitmap(appData.outlineKnightBitmap, &outlineKnightBitmap,
			outline_knight_bits, knight_small_outline_bits);
		ReadBitmap(appData.outlineBishopBitmap, &outlineBishopBitmap,
			outline_bishop_bits, bishop_small_outline_bits);
		ReadBitmap(appData.outlineQueenBitmap, &outlineQueenBitmap,
			outline_queen_bits, queen_small_outline_bits);
		ReadBitmap(appData.outlineKingBitmap, &outlineKingBitmap,
			outline_king_bits, king_small_outline_bits);
	}

	XSynchronize(xDisplay, False);	/* Work-around for xlib/xt buffering bug */
}

void
ReadBitmap(name, pm, big_bits, small_bits)
	String name;
	Pixmap *pm;
	char big_bits[], small_bits[];
{
	int x_hot, y_hot;
	u_int w, h;

	if (name == NULL || XReadBitmapFile(xDisplay, xBoardWindow, name,
		&w, &h, pm, &x_hot, &y_hot) != BitmapSuccess
		|| w != squareSize || h != squareSize) {
		if (appData.bigSizeMode)
			*pm = XCreateBitmapFromData(xDisplay, xBoardWindow,
				big_bits, squareSize, squareSize);
		else *pm = XCreateBitmapFromData(xDisplay, xBoardWindow,
			small_bits, squareSize, squareSize);
	}
}

void
CreateGrid()
{
	int i;

	for (i = 0; i < BOARD_SIZE + 1; i++) {
		gridSegments[i].x1 = gridSegments[i + BOARD_SIZE + 1].y1 = 0;
		gridSegments[i].y1 = gridSegments[i].y2
			= LINE_GAP / 2 + (i * (squareSize + LINE_GAP));
		gridSegments[i].x2 = LINE_GAP + BOARD_SIZE * (squareSize + LINE_GAP);
		gridSegments[i + BOARD_SIZE + 1].x1 =
		gridSegments[i + BOARD_SIZE + 1].x2 = LINE_GAP / 2
			+ (i * (squareSize + LINE_GAP));
		gridSegments[i + BOARD_SIZE + 1].y2 =
			BOARD_SIZE * (squareSize + LINE_GAP);
	}
}

/*
 * If the user selects on a border boundary or off the board, return failure.
 * Otherwise map the event coordinate to the square.
 */
int
EventToSquare(x)
	int x;
{
	if (x < LINE_GAP)
		return -1;
	x -= LINE_GAP;
	if ((x % (squareSize + LINE_GAP)) >= squareSize)
		return -1;
	x /= (squareSize + LINE_GAP);
	if (x >= BOARD_SIZE)
		return -1;
	return x;
}

ChessSquare
CharToPiece(c)
	int c;
{
	switch (c) {
	default:
	case '.':	return EmptySquare;
	case 'P':	return WhitePawn;
	case 'R':	return WhiteRook;
	case 'N':	return WhiteKnight;
	case 'B':	return WhiteBishop;
	case 'Q':	return WhiteQueen;
	case 'K':	return WhiteKing;
	case 'p':	return BlackPawn;
	case 'r':	return BlackRook;
	case 'n':	return BlackKnight;
	case 'b':	return BlackBishop;
	case 'q':	return BlackQueen;
	case 'k':	return BlackKing;
	}
}

void
DrawSquare(row, column, piece)
	int row, column;
	ChessSquare piece;
{
	int square_color, x, y;

	if (flipView)
		column = (BOARD_SIZE - 1) - column;
	else
		row = (BOARD_SIZE - 1) - row;

	square_color = ((column % 2 == 1) && (row % 2 == 1))
		|| ((column % 2 == 0) && (row % 2 == 0));

	x = LINE_GAP + column * (squareSize + LINE_GAP);
	y = LINE_GAP + row * (squareSize + LINE_GAP);

	if (piece == EmptySquare)
		XFillRectangle(xDisplay, xBoardWindow, square_color
			? lightSquareGC : darkSquareGC, x, y, squareSize, squareSize);
	else if (appData.monoMode) {
		if (square_color)
			XCopyPlane(xDisplay, (int) piece < (int) BlackPawn
				? *pieceToOutline[(int) piece] : *pieceToSolid[(int) piece],
				xBoardWindow, bwPieceGC, 0, 0, squareSize, squareSize, x, y, 1);
		else XCopyPlane(xDisplay, (int) piece < (int) BlackPawn
				? *pieceToSolid[(int) piece] : *pieceToOutline[(int) piece],
				xBoardWindow, wbPieceGC, 0, 0, squareSize, squareSize, x, y, 1);
	} else {
		if (square_color)
			XCopyPlane(xDisplay, *pieceToSolid[(int) piece],
				xBoardWindow, (int) piece < (int) BlackPawn ? wlPieceGC
				: blPieceGC, 0, 0, squareSize, squareSize, x, y, 1);
		else XCopyPlane(xDisplay, *pieceToSolid[(int) piece],
				xBoardWindow, (int) piece < (int) BlackPawn ? wdPieceGC
				: bdPieceGC, 0, 0, squareSize, squareSize, x, y, 1);
	}
}

void
EventProc(widget, unused, event)
	Widget widget;
	caddr_t unused;
	XEvent *event;
{
	if (event->type == MappingNotify) {
		XRefreshKeyboardMapping((XMappingEvent *) event);
		return;
	}

	if (!XtIsRealized(widget))
		return;

	if ((event->type == ButtonPress) || (event->type == ButtonRelease))
		if (event->xbutton.button != Button1)
			return;

	switch (event->type) {
	case Expose:
		DrawPosition(widget, (XExposeEvent *) event);
		break;
	default:
		return;
	}
}

/*
 * event handler for redrawing the board
 */
void
DrawPosition(w, event)
	Widget w;
	XExposeEvent *event;
{
	Arg args[1];
	int i, j;

	XtSetArg(args[0], XtNiconic, False);
	XtSetValues(shellWidget, args, 1);

	/*
	 * It would be simpler to clear the window with XClearWindow()
	 * but this causes a very distracting flicker.
	 */
	XDrawSegments(xDisplay, xBoardWindow, lineGC,
		gridSegments, (BOARD_SIZE + 1) * 2);

	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			DrawSquare(i, j, boards[currentMove][i][j]);

	XSync(xDisplay, False);
}

void
InitPosition()
{
	currentMove = forwardMostMove = 0;
	CopyBoard(boards[0], initialPosition);
	DrawPosition(boardWidget, (XExposeEvent *) NULL);
}

void
CopyBoard(to, from)
	Board to, from;
{
	int i, j;

	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			to[i][j] = from[i][j];
}

void
SendCurrentBoard(fp)
	FILE *fp;
{
	char message[MSG_SIZ];
	ChessSquare *bp;
	int i, j;

	SendToProgram("edit\n", fp);
	SendToProgram("#\n", fp);
	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		bp = &boards[currentMove][i][0];
		for (j = 0; j < BOARD_SIZE; j++, bp++) {
			if ((int) *bp < (int) BlackPawn) {
				sprintf(message, "%c%c%c\n", pieceToChar[(int) *bp],
					'a' + j, '1' + i);
				SendToProgram(message, fp);
			}
		}
	}

	SendToProgram("c\n", fp);
	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		bp = &boards[currentMove][i][0];
		for (j = 0; j < BOARD_SIZE; j++, bp++) {
			if (((int) *bp != (int) EmptySquare)
				&& ((int) *bp >= (int) BlackPawn)) {
				sprintf(message, "%c%c%c\n",
					pieceToChar[(int) *bp - (int) BlackPawn], 'a' + j, '1' + i);
				SendToProgram(message, fp);
			}
		}
	}

	SendToProgram(".\n", fp);
}

/*
 * event handler for parsing user moves
 */
void
HandleUserMove(w, event)
	Widget w;
	XEvent *event;
{
	ChessMove move_type;
	int to_x, to_y;
	char user_move[MSG_SIZ];

	if ((w != boardWidget) || (matchMode != MatchFalse))
		return;

	switch (gameMode) {
	case EndOfGame:
	case PlayFromGameFile:
		return;
	case MachinePlaysWhite:
		if (WHITE_ON_MOVE)
			return;
		break;
	case MachinePlaysBlack:
		if (!WHITE_ON_MOVE)
			return;
		break;
	default:
		break;
	}

	switch (event->type) {
	case ButtonPress:
		if ((fromX >= 0) || (fromY >= 0))
			return;
		if (((fromX = EventToSquare(event->xbutton.x)) < 0)
			|| ((fromY = EventToSquare(event->xbutton.y)) < 0)) {
			fromX = fromY = -1;
			break;
		}
		if (flipView)
			fromX = BOARD_SIZE - 1 - fromX;
		else
			fromY = BOARD_SIZE - 1 - fromY;
		break;
	case ButtonRelease:
		if ((fromX < 0) || (fromY < 0))
			return;
		if (((to_x = EventToSquare(event->xbutton.x)) < 0)
			|| ((to_y = EventToSquare(event->xbutton.y)) < 0)) {
			fromX = fromY = -1;
			break;
		}
		if (flipView)
			to_x = BOARD_SIZE - 1 - to_x;
		else
			to_y = BOARD_SIZE - 1 - to_y;

		if ((fromX == to_x) && (fromY == to_y)) {
			fromX = fromY = -1;
			break;
		}

		MakeMove(&move_type, fromX, fromY, to_x, to_y);

		switch (move_type) {
		default:
			fprintf(stderr, "%s: Can't happen (yet)\n", programName);
			break;
		case WhitePromotionQueen:
		case BlackPromotionQueen:
			sprintf(user_move, "%c%c%c%cq\n",
				'a' + fromX, '1' + fromY, 'a' + to_x, '1' + to_y);
			break;
		case NormalMove:
		case WhiteKingSideCastle:
		case WhiteQueenSideCastle:
		case WhiteCapturesEnPassant:
		case BlackKingSideCastle:
		case BlackQueenSideCastle:
		case BlackCapturesEnPassant:
			sprintf(user_move, "%c%c%c%c\n",
				'a' + fromX, '1' + fromY, 'a' + to_x, '1' + to_y);
			break;
		}

		fromX = fromY = -1;

		SendToProgram(user_move, toFirstProgFP);
		strcpy(moveList[currentMove - 1], user_move);

		if (forwardForce) {
			if (gameMode == PauseGame)
				PauseProc();
			if (gameMode == MachinePlaysWhite)
				SendToProgram(appData.whiteString, toFirstProgFP);
			else if (gameMode == MachinePlaysBlack)
				SendToProgram(appData.blackString, toFirstProgFP);
		}

		switch (gameMode) {
		case PauseGame:
			PauseProc();	/* a user move restarts a paused game */
		case ForceMoves:
			forwardForce = False;
			break;
		case BeginningOfGame:
		case SetupPosition:
			lastGameMode = gameMode = MachinePlaysBlack;
		case MachinePlaysBlack:
			/*
			 * gnuchess prefers to be told that it is black when it is on move.
			 */
			if (firstMove) {
				firstMove = False;
				if (currentMove > 1)	/* If it is being forced */
					SendToProgram(appData.blackString, toFirstProgFP);
			}
		case MachinePlaysWhite:
		default:
			break;
		}
		break;
	}
}

void
HandleMachineMove(message)
	char *message;
{
	char machine_move[MSG_SIZ], buf1[MSG_SIZ], buf2[MSG_SIZ];
	int i, j, from_x, from_y, to_x, to_y;
	ChessMove move_type;

	if (strncmp(message, "warning:", 8) == 0) {
		DisplayMessage(message);
		return;
	}

	/*
	 * If either host fails, reset to using localhost for both.
	 * Display an error message.
	 */
	if ((StrStr(message, "unknown host") != NULL)
		|| (StrStr(message, "No remote directory") != NULL)
		|| (StrStr(message, "not found") != NULL)) {
		ShutdownChessPrograms("");
		strcpy(appData.firstHost, "localhost");
		strcpy(appData.secondHost, "localhost");
		if (matchMode != MatchFalse)
			TwoMachinesPlayProc();
		else
			ResetProc();
		strcpy(buf1, "using localhost - ");
		strcat(buf1, message);
		DisplayMessage(buf1);
		return;
	}

	/*
	 * If the move is illegal, cancel it and redraw the board.
	 */
	if (strncmp(message, "Illegal move", 12) == 0) {
		if (gameMode == PlayFromGameFile) {
			lastGameMode = gameMode;
			gameMode = BeginningOfGame;
			if (WHITE_ON_MOVE)
				sprintf(buf1, "Illegal move: %d. %s",
					(currentMove / 2) + 1, moveList[currentMove - 1]);
			else
				sprintf(buf1, "Illegal move: %d. ... %s",
					(currentMove / 2) + 1, moveList[currentMove - 1]);
			ShutdownChessPrograms(buf1);
			return;
		}

		currentMove--;
		if (gameMode != ForceMoves)
			DisplayClocks(DisplayTimers);
		sprintf(buf1, "Illegal move: %s", moveList[currentMove]);
		DisplayMessage(buf1);

		/*
		 * Only redraw the squares that have changed.
		 */
		for (i = 0; i < BOARD_SIZE; i++)
			for (j = 0; j < BOARD_SIZE; j++)
				if (boards[currentMove][i][j] != boards[currentMove + 1][i][j])
					DrawSquare(i, j, boards[currentMove][i][j]);

		XSync(xDisplay, False);
		return;
	}

	if (strncmp(message, "Hint:", 5) == 0) {
		DisplayMessage(message);
		return;
	}

	/*
	 * win, lose or draw
	 */
	if (strncmp(message, "White", 5) == 0) {
		ShutdownChessPrograms("White wins");
		strcpy(moveList[currentMove++], "White wins");
		return;
	} else if (strncmp(message, "Black", 5) == 0) {
		ShutdownChessPrograms("Black wins");
		strcpy(moveList[currentMove++], "Black wins");
		return;
	} else if (strncmp(message, "opponent mates!", 15) == 0) {
		if (gameMode == MachinePlaysBlack)
			ShutdownChessPrograms("White wins");
		else
			ShutdownChessPrograms("Black wins");
		strcpy(moveList[currentMove++], "mate");
		return;
	} else if (strncmp(message, "computer mates!", 15) == 0) {
		if (gameMode == MachinePlaysWhite)
			ShutdownChessPrograms("White wins");
		else
			ShutdownChessPrograms("Black wins");
		strcpy(moveList[currentMove++], "mate");
		return;
	} else if (strncmp(message, "Draw", 4) == 0) {
		ShutdownChessPrograms("Draw");
		strcpy(moveList[currentMove++], "draw");
		return;
	}

	/*
	 * normal machine reply move
	 */
	if (StrStr(message, "...") != NULL) {
		sscanf(message, "%s %s %s", buf1, buf2, machine_move);
		if (machine_move[0] == '\0')
			return;
	} else
		return; /* ignore noise */

	strcpy(moveList[currentMove], machine_move);

	from_x = machine_move[0] - 'a';
	from_y = machine_move[1] - '1';
	to_x = machine_move[2] - 'a';
	to_y = machine_move[3] - '1';

	MakeMove(&move_type, from_x, from_y, to_x, to_y);

	switch (gameMode) {
	case PauseGame:
	case SetupPosition:
	case EndOfGame:
	default:
		break;
	case ForceMoves:
	case PlayFromGameFile:
		strncat(machine_move, "\n", 1);
		SendToProgram(machine_move, toFirstProgFP);
		break;
	case TwoMachinesPlay:
		strncat(machine_move, "\n", 1);
		SendToProgram(machine_move, WHITE_ON_MOVE
			? toSecondProgFP : toFirstProgFP);
		/*
		 * gnuchess prefers to be told that it is black when it is on move.
		 */
		if (firstMove) {
			firstMove = False;
			SendToProgram(appData.blackString, toFirstProgFP);
		}
		break;
	}
}

void
ReadGameFile()
{
	for (;;) {
		if (!ReadGameFileProc())
			return;
		if (matchMode == MatchOpening)
			continue;
		readGameXID = XtAppAddTimeOut(appContext,
			(int) (1000 * appData.timeDelay), ReadGameFile, NULL);
		break;
	}
}

int
ReadGameFileProc()
{
	int from_x, from_y, to_x, to_y;
	ChessMove move_type;
	char move[MSG_SIZ];

	if (gameFileFP == NULL)
		return (int) False;

	if ((gameMode == EndOfGame) || (gameMode == BeginningOfGame)
		|| feof(gameFileFP)) {
		fclose(gameFileFP);
		gameFileFP = NULL;
		return (int) False;
	}

	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}

	move_type = (ChessMove) yylex();

	if (xboardDebug) {
		if (move_type == BadMove)
			fprintf(stderr, "Parsed BadMove: %s\n", yytext);
		else if (move_type == Comment)
			fprintf(stderr, "Parsed Comment: %s\n", yytext);
		else if ((move_type ==  WhiteWins) || (move_type == BlackWins)
			|| (move_type == GameIsDrawn) || (move_type == StartGame))
			fprintf(stderr, "Parsed EndOfGame: %s\n", yytext);
		else
			fprintf(stderr, "Parsed %s into %s", yytext, currentMoveString);
	}

	switch (move_type) {
	case Comment:
		CommentPopUp(yytext);
		return (int) True;
	case WhiteKingSideCastle:
		from_x = 4;
		from_y = 0;
		to_x = 6;
		to_y = 0;
		break;
	case WhiteQueenSideCastle:
		from_x = 4;
		from_y = 0;
		to_x = 2;
		to_y = 0;
		break;
	case BlackKingSideCastle:
		from_x = 4;
		from_y = 7;
		to_x = 6;
		to_y = 7;
		break;
	case BlackQueenSideCastle:
		from_x = 4;
		from_y = 7;
		to_x = 2;
		to_y = 7;
		break;
	case WhiteCapturesEnPassant:
	case BlackCapturesEnPassant:
	case WhitePromotionQueen:
	case BlackPromotionQueen:
	case NormalMove:
		from_x = tolower(currentMoveString[0]) - 'a';
		from_y = tolower(currentMoveString[1]) - '1';
		to_x = tolower(currentMoveString[2]) - 'a';
		to_y = tolower(currentMoveString[3]) - '1';
		break;
	case 0:						/* end of file */
		strcpy(parseList[currentMove], "End Of Game");
	case WhiteWins:
		if (move_type == WhiteWins)
			strcpy(parseList[currentMove], "1-0");
	case BlackWins:
		if (move_type == BlackWins)
			strcpy(parseList[currentMove], "0-1");
	case GameIsDrawn:
		if (move_type == GameIsDrawn)
			strcpy(parseList[currentMove], "1/2-1/2");
		CopyBoard(boards[currentMove + 1], boards[currentMove]);
		forwardMostMove = currentMove++;
	default:
	case BadMove:
	case WhitePromotionRook:	/* TODO: not handled yet */
	case BlackPromotionRook:
	case WhitePromotionBishop:
	case BlackPromotionBishop:
	case WhitePromotionKnight:
	case BlackPromotionKnight:
		if (move_type == (ChessMove) 0)
			DisplayMessage("End Of Game");
		else if (move_type == BadMove) {
				if (WHITE_ON_MOVE)
					sprintf(move, "Bad move: %d. %s",
						(currentMove / 2) + 1, yytext);
				else
					sprintf(move, "Bad move: %d. ... %s",
						(currentMove / 2) + 1, yytext);
			DisplayMessage(move);
		} else
			DisplayMessage(yytext);
		lastGameMode = gameMode;
		gameMode = BeginningOfGame;
		if (readGameXID != NULL) {
			XtRemoveTimeOut(readGameXID);
			readGameXID = NULL;
		}
		fclose(gameFileFP);
		gameFileFP = NULL;
		return (int) False;
	}

	SendToProgram(currentMoveString, toFirstProgFP);
	strncpy(moveList[currentMove], currentMoveString, MOVE_LEN);

	MakeMove(&move_type, from_x, from_y, to_x, to_y);

	return (int) True;
}

/*
 * MakeMove() displays moves.  If they are illegal, GNU chess will detect
 * this and send an Illegal move message.  XBoard will then retract the move.
 * The clockMode False case is tricky because it displays the player on move.
 */
void
MakeMove(move_type, from_x, from_y, to_x, to_y)
	ChessMove *move_type;
	int from_x, from_y, to_x, to_y;
{
	char message[MSG_SIZ];
	int move;

	if ((gameMode != PlayFromGameFile)
		&& (gameMode != ForceMoves) && appData.clockMode)
		DisplayClocks(DisplayTimers);

	CopyBoard(boards[currentMove + 1], boards[currentMove]);
	forwardMostMove = ++currentMove;

	move = (currentMove + 1) / 2;

	if (!appData.clockMode)
		DisplayClocks(DisplayTimers);

	if (from_y == 0 && from_x == 4				/* white king-side castle */
			&& boards[currentMove][from_y][from_x] == WhiteKing
			&& to_y == 0 && to_x == 6) {
		*move_type = WhiteKingSideCastle;
		boards[currentMove][0][7] = EmptySquare;
		boards[currentMove][0][6] = WhiteKing;
		boards[currentMove][0][5] = WhiteRook;
		boards[currentMove][0][4] = EmptySquare;
		DrawSquare(0, 7, boards[currentMove][0][7]);
		DrawSquare(0, 6, boards[currentMove][0][6]);
		DrawSquare(0, 5, boards[currentMove][0][5]);
		DrawSquare(0, 4, boards[currentMove][0][4]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. %s", move, yytext);
		else
			sprintf(message, "%d. 0-0", move);
	} else if (from_y == 0 && from_x == 4		/* white queen-side castle */
			&& boards[currentMove][from_y][from_x] == WhiteKing
			&& to_y == 0 && to_x == 2) {
		*move_type = WhiteQueenSideCastle;
		boards[currentMove][0][0] = EmptySquare;
		boards[currentMove][0][2] = WhiteKing;
		boards[currentMove][0][3] = WhiteRook;
		boards[currentMove][0][4] = EmptySquare;
		DrawSquare(0, 0, boards[currentMove][0][0]);
		DrawSquare(0, 2, boards[currentMove][0][2]);
		DrawSquare(0, 3, boards[currentMove][0][3]);
		DrawSquare(0, 4, boards[currentMove][0][4]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. %s", move, yytext);
		else
			sprintf(message, "%d. 0-0-0", move);
	} else if (from_y == 6						/* white pawn promotion */
			&& boards[currentMove][from_y][from_x] == WhitePawn && to_y == 7) {
		*move_type = WhitePromotionQueen;		/* TODO: gnuchess limitation */
		boards[currentMove][6][from_x] = EmptySquare;
		boards[currentMove][7][to_x] = WhiteQueen;
		DrawSquare(6, from_x, boards[currentMove][6][from_x]);
		DrawSquare(7, to_x, boards[currentMove][7][to_x]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. %s", move, yytext);
		else
			sprintf(message, "%d. %c8(Q)", move, from_x + 'a');
	} else if ((from_y == 4)					/* white captures en passant */
			&& (to_x != from_x)
			&& (boards[currentMove][from_y][from_x] == WhitePawn)
			&& (boards[currentMove][to_y][to_x] == EmptySquare)) {
		*move_type = WhiteCapturesEnPassant;
		boards[currentMove][from_y][from_x] = EmptySquare;
		boards[currentMove][to_y][to_x] = WhitePawn;
		boards[currentMove][to_y - 1][to_x] = EmptySquare;
		DrawSquare(from_y, from_x, boards[currentMove][from_y][from_x]);
		DrawSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
		DrawSquare(to_y - 1, to_x, boards[currentMove][to_y - 1][to_x]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. %s", move, yytext);
		else
			sprintf(message, "%d. %c%c ep", move, from_x + 'a', to_x + 'a');
	} else if (from_y == 7 && from_x == 4		/* black king-side castle */
			&& boards[currentMove][from_y][from_x] == BlackKing
			&& to_y == 7 && to_x == 6) {
		*move_type = BlackKingSideCastle;
		boards[currentMove][7][4] = EmptySquare;
		boards[currentMove][7][5] = BlackRook;
		boards[currentMove][7][6] = BlackKing;
		boards[currentMove][7][7] = EmptySquare;
		DrawSquare(7, 7, boards[currentMove][7][7]);
		DrawSquare(7, 6, boards[currentMove][7][6]);
		DrawSquare(7, 5, boards[currentMove][7][5]);
		DrawSquare(7, 4, boards[currentMove][7][4]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. ... %s", move, yytext);
		else
			sprintf(message, "%d. ... 0-0", move);
	} else if (from_y == 7 && from_x == 4		/* black queen-side castle */
			&& boards[currentMove][from_y][from_x] == BlackKing
			&& to_y == 7 && to_x == 2) {
		*move_type = BlackQueenSideCastle;
		boards[currentMove][7][0] = EmptySquare;
		boards[currentMove][7][2] = BlackKing;
		boards[currentMove][7][3] = BlackRook;
		boards[currentMove][7][4] = EmptySquare;
		DrawSquare(7, 0, boards[currentMove][7][0]);
		DrawSquare(7, 2, boards[currentMove][7][2]);
		DrawSquare(7, 3, boards[currentMove][7][3]);
		DrawSquare(7, 4, boards[currentMove][7][4]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. ... %s", move, yytext);
		else
			sprintf(message, "%d. ... 0-0-0", move);
	} else if (from_y == 1						/* black pawn promotion */
			&& boards[currentMove][from_y][from_x] == BlackPawn && to_y == 0) {
		*move_type = BlackPromotionQueen;		/* TODO: gnuchess limitation */
		boards[currentMove][1][from_x] = EmptySquare;
		boards[currentMove][0][to_x] = BlackQueen;
		DrawSquare(1, from_x, boards[currentMove][1][from_x]);
		DrawSquare(0, to_x, boards[currentMove][0][to_x]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. ... %s", move, yytext);
		else
			sprintf(message, "%d. ... %c8(Q)", move, from_x + 'a');
	} else if ((from_y == 3)					/* black captures en passant */
			&& (to_x != from_x)
			&& (boards[currentMove][from_y][from_x] == BlackPawn)
			&& (boards[currentMove][to_y][to_x] == EmptySquare)) {
		*move_type = BlackCapturesEnPassant;
		boards[currentMove][from_y][from_x] = EmptySquare;
		boards[currentMove][to_y][to_x] = BlackPawn;
		boards[currentMove][to_y + 1][to_x] = EmptySquare;
		DrawSquare(from_y, from_x, boards[currentMove][from_y][from_x]);
		DrawSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
		DrawSquare(to_y + 1, to_x, boards[currentMove][to_y + 1][to_x]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. ... %s", move, yytext);
		else
			sprintf(message, "%d. ... %c%c ep", move, from_x + 'a', to_x + 'a');
	} else {
		*move_type = NormalMove;
		boards[currentMove][to_y][to_x] = boards[currentMove][from_y][from_x];
		boards[currentMove][from_y][from_x] = EmptySquare;
		DrawSquare(to_y, to_x, boards[currentMove][to_y][to_x]);
		DrawSquare(from_y, from_x, boards[currentMove][from_y][from_x]);
		if (gameMode == PlayFromGameFile)
			sprintf(message, "%d. %s%s", move,
				WHITE_ON_MOVE ? "... " : "", yytext);
		else
			sprintf(message, "%d. %s%c%c%c%c", move, WHITE_ON_MOVE ? "... "
				: "", from_x + 'a', from_y + '1', to_x + 'a', to_y + '1');
	}

	DisplayMessage(message);
	strcpy(parseList[currentMove - 1], message);
	XSync(xDisplay, False);
}

void
InitChessProgram(host_name, program_name, pid, to, from, xid)
	char *host_name, *program_name;
	int *pid;
	FILE **to, **from;
	XtIntervalId *xid;
{
	char time_control[10], moves_per_session[10], time_search[10];
	int to_prog[2], from_prog[2];
	FILE *from_fp, *to_fp;

#if	SYSTEM_FIVE || SYSV
	if ((ttyname = PseudoTTY(&to_prog[1])) == NULL) {
		perror(program_name);
		exit(1);
	}

	from_prog[0] = to_prog[1];
	to_prog[0] = from_prog[1] = open(ttyname, O_RDWR, 0);
#else
	signal(SIGPIPE, CatchPipeSignal);
	pipe(to_prog);
	pipe(from_prog);
#endif

	if ((*pid = fork()) == 0) {
#if	!SYSTEM_FIVE && !SYSV
		signal(SIGPIPE, CatchPipeSignal);
#ifdef	O_NDELAY
		fcntl(from_prog[0], F_SETFL, O_NDELAY);
#endif
#endif
		dup2(to_prog[0], 0);
		dup2(from_prog[1], 1);
		close(to_prog[0]);
		close(to_prog[1]);
		close(from_prog[0]);
		close(from_prog[1]);
		dup2(1, fileno(stderr));	/* force stderr to the pipe */
		sprintf(time_control, "%d", appData.timeControl);
		sprintf(time_search, "%d", appData.searchTime);
		sprintf(moves_per_session, "%d", appData.movesPerSession);
		if (strcmp(host_name, "localhost") == 0) {
			if (appData.searchTime <= 0)
				execlp(program_name, program_name,
					moves_per_session, time_control, (char *) NULL);
			else
				execlp(program_name, program_name, time_search, (char *) NULL);
		} else {
			if (appData.searchTime <= 0)
				execlp(appData.remoteShell, appData.remoteShell,
					host_name, program_name, moves_per_session,
					time_control, (char *) NULL);
			else
				execlp(appData.remoteShell, appData.remoteShell,
					host_name, program_name, time_search, (char *) NULL);
		}

		perror(program_name);
		exit(1);
	}

	close(to_prog[0]);
	close(from_prog[1]);

	*from = from_fp = fdopen(from_prog[0], "r");
	*to = to_fp = fdopen(to_prog[1], "w");
	setbuf(from_fp, NULL); setbuf(to_fp, NULL);

	*xid = XtAppAddInput(appContext, fileno(from_fp), XtInputReadMask,
		ReceiveFromProgram, from_fp);

	SendToProgram(appData.initString, to_fp);
}

void
ShutdownChessPrograms(message)
	char *message;
{
	lastGameMode = gameMode;
	gameMode = EndOfGame;
	DisplayMessage(message);

	if (firstProgramPID != 0) {
		fclose(fromFirstProgFP);
		fclose(toFirstProgFP);
		fromFirstProgFP = toFirstProgFP = NULL;
		kill(firstProgramPID, SIGTERM);
	}

	if (secondProgramPID != 0) {
		fclose(fromSecondProgFP);
		fclose(toSecondProgFP);
		fromSecondProgFP = toSecondProgFP = NULL;
		kill(secondProgramPID, SIGTERM);
	}

	if (firstProgramXID != NULL)
		XtRemoveInput(firstProgramXID);
	if (secondProgramXID != NULL)
		XtRemoveInput(secondProgramXID);
	if (readGameXID != NULL)
		XtRemoveTimeOut(readGameXID);

	firstProgramXID = secondProgramXID = readGameXID = NULL;
	firstProgramPID = secondProgramPID = 0;

	DisplayClocks(StopTimers);
}

void
CommentPopUp(label)
	char *label;
{
	Arg args[2];
	Position x, y;
	Dimension bw_width, pw_width;

	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}

	DisplayMessage("Comment");

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtGetValues(boardWidget, args, 1);

	XtSetArg(args[0], XtNresizable, True);
	XtSetArg(args[1], XtNwidth, bw_width);

	commentShell = XtCreatePopupShell("Comment",
		transientShellWidgetClass, commandsWidget, args, 2);

	XtSetArg(args[0], XtNlabel, label);

	(void) XtCreateManagedWidget("commentLabel", labelWidgetClass,
		commentShell, args, 1);

	XtRealizeWidget(commentShell);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtGetValues(commentShell, args, 1);

	XtTranslateCoords(shellWidget, (bw_width - pw_width) / 2, -50, &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(commentShell, args, 2);

	XtPopup(commentShell, XtGrabNone);
	commentUp = True;
}

void
FileNamePopUp(label, proc)
	char *label;
	void (*proc) P((char *name));
{
	Arg args[2];
	Widget popup, dialog;
	Position x, y;
	Dimension bw_width, pw_width;

	fileProc = (XtCallbackProc) proc;

	XtSetArg(args[0], XtNwidth, &bw_width);
	XtGetValues(boardWidget, args, 1);

	XtSetArg(args[0], XtNresizable, True);
	XtSetArg(args[1], XtNwidth, DIALOG_SIZE);

	popup = XtCreatePopupShell("File Name Prompt",
		transientShellWidgetClass, commandsWidget, args, 2);

	XtSetArg(args[0], XtNlabel, label);
	XtSetArg(args[1], XtNvalue, "");

	dialog = XtCreateManagedWidget("dialog", dialogWidgetClass,
		popup, args, 2);

	XawDialogAddButton(dialog, "ok", FileNameCallback, (XtPointer) dialog);
	XawDialogAddButton(dialog, "cancel", FileNameCallback, (XtPointer) dialog);

	XtRealizeWidget(popup);

	XtSetArg(args[0], XtNwidth, &pw_width);
	XtGetValues(popup, args, 1);

	XtTranslateCoords(boardWidget, (bw_width - pw_width) / 2, 10, &x, &y);

	XtSetArg(args[0], XtNx, x);
	XtSetArg(args[1], XtNy, y);
	XtSetValues(popup, args, 2);

	XtPopup(popup, XtGrabExclusive);

	XtSetKeyboardFocus(shellWidget, popup);
}

void
FileNameCallback(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	String name;
	Arg args[1];

	XtSetArg(args[0], XtNlabel, &name);
	XtGetValues(w, args, 1);

	if (strcmp(name, "cancel") == 0) {
		XtPopdown(w = XtParent(XtParent(w)));
		XtDestroyWidget(w);
		return;
	}

	FileNameAction(w, NULL);
}

void
FileNameAction(w, event)
	Widget w;
	XEvent *event;
{
	char buf[MSG_SIZ];
	String name;

	name = XawDialogGetValueString(w = XtParent(w));

	if ((name != NULL) && (*name != '\0')) {
		strcpy(buf, name);
		XtPopdown(w = XtParent(w));
		XtDestroyWidget(w);
		(*fileProc)(buf);	/* I can't see a way not to use a global here */
		return;
	}

	XtPopdown(w = XtParent(w));
	XtDestroyWidget(w);
}

void
SelectCommand(w, client_data, call_data)
	Widget w;
	XtPointer client_data, call_data;
{
	XawListReturnStruct *list_return = XawListShowCurrent(w);

	fromX = fromY = -1;

	if ((gameMode == PauseGame)
		&& (list_return->list_index != (int) ButtonPause))
		PauseProc();

	switch ((Button) list_return->list_index) {
	case ButtonQuit:
		QuitProc();
		break;
	case ButtonBackward:
		BackwardProc();
		break;
	case ButtonForward:
		ForwardProc();
		break;
	case ButtonFlip:
		FlipProc();
		break;
	case ButtonReset:
		ResetFileProc();
		ResetProc();
		break;
	case ButtonSaveGame:
		if (appData.saveGameFile[0] == '\0')
			FileNamePopUp("Filename for saved game?", SaveGameProc);
		else
			SaveGameProc(appData.saveGameFile);
		break;
	case ButtonSavePosition:
		if (appData.savePositionFile[0] == '\0')
			FileNamePopUp("Filename for saved position?", SavePositionProc);
		else
			SaveGameProc(appData.savePositionFile);
		break;
	case ButtonHint:
		HintProc();
		break;
	case ButtonSwitch:
		SwitchProc();
		break;
	case ButtonSetupFromFile:
		if (gameMode != BeginningOfGame)
			break;
		if (appData.readPositionFile == NULL)
			FileNamePopUp("Position file name?",
				VOID_PROC SetupPositionFromFileProc);
		else
			(void) SetupPositionFromFileProc(appData.readPositionFile);
		break;
	case ButtonPlayFromFile:
		GameProc();
		break;
	case ButtonMachinePlaysBlack:
		MachinePlaysBlackProc();
		break;
	case ButtonMachinePlaysWhite:
		MachinePlaysWhiteProc();
		break;
	case ButtonTwoMachinesPlay:
		TwoMachinesPlayProc();
		break;
	case ButtonForce:
		ForceProc();
		break;
	case ButtonPause:
		PauseProc();
		break;
	}

	XawListUnhighlight(w);
}

/*
 * Button procedures
 */
void
QuitProc()
{
	ShutdownChessPrograms("Quitting");
	exit(0);
}

int
PlayFromGameFileProc(name)
	char *name;
{
	char buf[MSG_SIZ], error_buf[MSG_SIZ];
	Arg args[1];

	if (gameMode != BeginningOfGame)
		return (int) False;

	if (appData.readGameFile != name) {
		if (appData.readGameFile)
			XtFree(appData.readGameFile);
		appData.readGameFile = XtMalloc(strlen(name));
		strcpy(appData.readGameFile, name);
	}

	if (name[0] == '/')
		strcpy(buf, name);
	else {
		strcpy(buf, chessDir);
		strcat(buf, "/");
		strcat(buf, name);
	}

	if ((gameFileFP = fopen(buf, "r")) == NULL) {
		strcpy(error_buf, buf);
		sprintf(buf, "Can't open %s", error_buf);
		DisplayMessage(buf);
		XtFree(appData.readGameFile);
		appData.readGameFile = NULL;
		return (int) False;
	}

	lastGameMode = gameMode;
	gameMode = PlayFromGameFile;
	InitPosition();
	DisplayClocks(StopTimers);
	if (firstProgramXID == NULL)
		InitChessProgram(appData.firstHost, appData.firstChessProgram,
			&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
			&firstProgramXID);
	SendToProgram(appData.initString, toFirstProgFP);
	SendToProgram("force\n", toFirstProgFP);

	/*
	 * If the file doesn't immediately start with what looks like a game ...
	 */
	if (fgetc(gameFileFP) != '1') {
		fseek(gameFileFP, 0, 0);
		fgets(buf, MSG_SIZ, gameFileFP);
		buf[strlen(buf) - 1] = '\0';
		XtSetArg(args[0], XtNlabel, buf);
		XtSetValues(nameWidget, args, 1);
	} else
		fseek(gameFileFP, 0, 0);

	for (;;)
		if ((ChessMove) yylex() == StartGame)
			break;

	ReadGameFile();

	return (int) True;
}

void
MachinePlaysBlackProc()
{
	if ((gameMode == EndOfGame) || (gameMode == PlayFromGameFile)
		|| (matchMode != MatchFalse) || WHITE_ON_MOVE)
		return;

	undoMode = False;
	lastGameMode = gameMode = MachinePlaysBlack;
	SendToProgram(appData.blackString, toFirstProgFP);
}

void
ForwardProc()
{
	char buf[MSG_SIZ];
	int i, j;

	if (gameMode == BeginningOfGame) {
		if (currentMove != 0)
			return;
		if (appData.readGameFile != NULL) {
			if (PlayFromGameFileProc(appData.readGameFile)) {
				XtRemoveTimeOut(readGameXID);
				readGameXID = NULL;
			}
		}
		return;
	}

	if ((gameMode == EndOfGame) || (matchMode != MatchFalse)
		|| ((currentMove == 0) && (forwardMostMove == 0)))
		return;

	if ((gameMode != PlayFromGameFile) && (currentMove >= forwardMostMove))
		return;

	if (gameMode != PauseGame) {
		PauseProc();
		if (readGameXID != NULL) {
			XtRemoveTimeOut(readGameXID);
			readGameXID = NULL;
		}
	}

	if (currentMove >= forwardMostMove) {
		ReadGameFileProc();
		if ((strcmp(parseList[currentMove], "1-0") == 0)
			|| (strcmp(parseList[currentMove], "0-1") == 0)
			|| (strcmp(parseList[currentMove], "1/2-1/2") == 0)
			|| (strcmp(parseList[currentMove], "End Of Game") == 0))
			DisplayMessage(parseList[currentMove]);
		return;
	}

	if ((forwardForce == False) && (gameMode != PlayFromGameFile)) {
		forwardForce = True;
		SendToProgram("force\n", toFirstProgFP);
	}

	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			if (boards[currentMove + 1][i][j] != boards[currentMove][i][j])
				DrawSquare(i, j, boards[currentMove + 1][i][j]);

	DisplayMessage(parseList[currentMove]);
	strcpy(buf, moveList[currentMove++]);
	SendToProgram(buf, toFirstProgFP);
}

void
ResetFileProc()
{
	Arg args[1];
	char *buf = "";

	if (appData.readGameFile);
		XtFree(appData.readGameFile);
	if (appData.readPositionFile);
		XtFree(appData.readPositionFile);
	appData.readGameFile = appData.readPositionFile = NULL;
	XtSetArg(args[0], XtNlabel, buf);
	XtSetValues(nameWidget, args, 1);
	if (gameFileFP != NULL) {
		fclose(gameFileFP);
		gameFileFP = NULL;
	}
}

void
ResetProc()
{
	twoProgramState = flipView = forwardForce = False;
	matchMode = MatchFalse;
	firstMove = True;

	ShutdownChessPrograms("");
	lastGameMode = gameMode = BeginningOfGame;
	InitPosition();
	InitChessProgram(appData.firstHost, appData.firstChessProgram,
		&firstProgramPID, &toFirstProgFP, &fromFirstProgFP, &firstProgramXID);
	DisplayClocks(ResetTimers);
	if (commentUp) {
		XtPopdown(commentShell);
		XtDestroyWidget(commentShell);
		commentUp = False;
	}
}

int
SetupPositionFromFileProc(name)
	char *name;
{
	char *p, line[MSG_SIZ], buf[MSG_SIZ];
	Board initial_position;
	Arg args[1];
	FILE *fp;
	int i, j;

	if (gameMode != BeginningOfGame)
		return (int) False;

	if (appData.readPositionFile != name) {
		if (appData.readPositionFile)
			XtFree(appData.readPositionFile);
		appData.readPositionFile = XtMalloc(strlen(name));
		strcpy(appData.readPositionFile, name);
	}

	if (name[0] == '/')
		strcpy(buf, name);
	else {
		strcpy(buf, chessDir);
		strcat(buf, "/");
		strcat(buf, name);
	}

	if ((fp = fopen(buf, "r")) == NULL) {
		strcpy(line, buf);
		sprintf(buf, "Can't open %s", line);
		DisplayMessage(buf);
		XtFree(appData.readPositionFile);
		appData.readPositionFile = NULL;
		return (int) False;
	}

	lastGameMode = gameMode = SetupPosition;

	if (firstProgramXID == NULL)
		InitChessProgram(appData.firstHost, appData.firstChessProgram,
			&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
			&firstProgramXID);

	/*
	 * skip header information in position file
	 */
	fgets(line, MSG_SIZ, fp);
	line[strlen(line) - 1] = '\0';
	XtSetArg(args[0], XtNlabel, line);
	XtSetValues(nameWidget, args, 1);
	fgets(line, MSG_SIZ, fp);
	fgets(line, MSG_SIZ, fp);

	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		fgets(line, MSG_SIZ, fp);
		for (p = line, j = 0; j < BOARD_SIZE; p++) {
			if (*p == ' ')
				continue;
			initial_position[i][j++] = CharToPiece(*p);
		}
	}
	fclose(fp);

	currentMove = forwardMostMove = 0;
	CopyBoard(boards[0], initial_position);
	SendCurrentBoard(toFirstProgFP);
	DrawPosition(boardWidget, (XExposeEvent *) NULL);

	return (int) True;
}

void
MachinePlaysWhiteProc()
{
	if ((gameMode == EndOfGame) || (gameMode == PlayFromGameFile)
		|| (matchMode != MatchFalse) || !WHITE_ON_MOVE)
		return;

	undoMode = False;
	lastGameMode = gameMode = MachinePlaysWhite;
	SendToProgram(appData.whiteString, toFirstProgFP);
}

void
BackwardProc()
{
	int i, j;

	if ((currentMove <= 0) || (matchMode != MatchFalse)
		|| (gameMode == EndOfGame))
		return;

	if (currentMove == 1) {
		ResetProc();
		return;
	}

	if ((WHITE_ON_MOVE && gameMode == MachinePlaysWhite)
		|| (!WHITE_ON_MOVE && gameMode == MachinePlaysBlack)) {
		DisplayMessage("Wait until your turn");
		return;
	}

	if (gameMode == BeginningOfGame)
		gameMode = lastGameMode;
	else
		SendToProgram("undo\n", toFirstProgFP);

	if (gameMode != PauseGame) {
		PauseProc();
		if (readGameXID != NULL) {
			XtRemoveTimeOut(readGameXID);
			readGameXID = NULL;
		}
	}

	undoMode = True;
	currentMove--;
	DisplayMessage(parseList[currentMove - 1]);

	for (i = 0; i < BOARD_SIZE; i++)
		for (j = 0; j < BOARD_SIZE; j++)
			if (boards[currentMove][i][j] != boards[currentMove + 1][i][j])
				DrawSquare(i, j, boards[currentMove][i][j]);
}

void
FlipProc()
{
	flipView = !flipView;
	DrawPosition(boardWidget, (XExposeEvent *) NULL);
}

void
SaveGameProc(name)
	char *name;
{
	char buf[MSG_SIZ], white_move[MSG_SIZ], black_move[MSG_SIZ];
	int i, len, move = 0;
	time_t tm;

	if ((gameFileFP = fopen(name, "w")) == NULL) {
		sprintf(buf, "Can't open %s", name);
		DisplayMessage(buf);
		return;
	}

	tm = time((time_t *) NULL);
	gethostname(buf, MSG_SIZ);

	fprintf(gameFileFP, "%s game file -- %s", programName, ctime(&tm));
	switch (lastGameMode) {
	case MachinePlaysWhite:
		fprintf(gameFileFP, "\t%s@%s vs. %s@%s\n", appData.firstChessProgram,
			appData.firstHost, getpwuid(getuid())->pw_name, buf);
		break;
	case MachinePlaysBlack:
		fprintf(gameFileFP, "\t%s@%s vs. %s@%s\n", getpwuid(getuid())->pw_name,
			buf, appData.firstChessProgram, appData.firstHost);
		break;
	case TwoMachinesPlay:
		fprintf(gameFileFP, "\t%s@%s vs. %s@%s\n",
			appData.secondChessProgram, appData.secondHost,
			appData.firstChessProgram, appData.firstHost);
		break;
	default:
		fprintf(gameFileFP, "\n");
		break;
	}
	fprintf(gameFileFP, "\talgebraic\n");

	for (i = 0; i < currentMove;) {
		/*
		 * get rid of '\n' added to send the move to GNU Chess
		 */
		len = strlen(moveList[i]);
		strcpy(white_move, moveList[i++]);
		if (white_move[len - 1] == '\n')
			white_move[len - 1] = NULL;
		fprintf(gameFileFP, "%d. %s ", ++move, white_move);

		if (i >= currentMove) {
			fprintf(gameFileFP, "\n");
			break;
		}

		if ((len = strlen(moveList[i])) == 0) {
			fprintf(gameFileFP, "\n");
			break;
		}
		strcpy(black_move, moveList[i++]);
		if (black_move[len - 1] == '\n')
			black_move[len - 1] = NULL;
		fprintf(gameFileFP, "%s\n", black_move);
	}

	fclose(gameFileFP);
	gameFileFP = NULL;
}

void
SwitchProc()
{
	switch (gameMode) {
	default:
		return;
	case MachinePlaysWhite:
		lastGameMode = gameMode = MachinePlaysBlack;
		break;
	case BeginningOfGame:
	case MachinePlaysBlack:
		if (currentMove == 0) {
			MachinePlaysWhiteProc();
			return;
		}
		lastGameMode = gameMode = MachinePlaysWhite;
		break;
	}

	SendToProgram("switch\n", toFirstProgFP);
}

void
ForceProc()
{
	if ((gameMode == EndOfGame) || (gameMode == PlayFromGameFile))
		return;

	switch (gameMode) {
	case MachinePlaysWhite:
		if (WHITE_ON_MOVE && !undoMode) {
			DisplayMessage("Wait until your move");
			return;
		}
		break;
	case MachinePlaysBlack:
		if (!WHITE_ON_MOVE && !undoMode) {
			DisplayMessage("Wait until your move");
			return;
		}
	case BeginningOfGame:
		break;
	default:
		return;
	}

	lastGameMode = gameMode = ForceMoves;
	DisplayClocks(StopTimers);
	SendToProgram("force\n", toFirstProgFP);
}

void
HintProc()
{
	switch (gameMode) {
	case MachinePlaysWhite:
	case MachinePlaysBlack:
	case BeginningOfGame:
		SendToProgram("hint\n", toFirstProgFP);
	default:
		return;
	}
}

void
SavePositionProc(name)
	char *name;
{
	char buf[MSG_SIZ], host_name[MSG_SIZ];
	FILE *fp;
	time_t tm;
	int i, j;

	if ((fp = fopen(name, "w")) == NULL) {
		sprintf(buf, "Can't open %s", name);
		DisplayMessage(buf);
		return;
	}

	tm = time((time_t *) NULL);
	gethostname(host_name, MSG_SIZ);

	fprintf(fp, "%s position file -- %s", programName, ctime(&tm));
	switch (gameMode) {
	case MachinePlaysWhite:
		fprintf(fp, "\t%s@%s vs. %s@%s\n", appData.firstChessProgram,
			appData.firstHost, getpwuid(getuid())->pw_name, host_name);
		break;
	case MachinePlaysBlack:
		fprintf(fp, "\t%s@%s vs. %s@%s\n", getpwuid(getuid())->pw_name,
			host_name, appData.firstChessProgram, appData.firstHost);
		break;
	case TwoMachinesPlay:
		fprintf(fp, "\t%s@%s vs. %s@%s\n", appData.secondChessProgram,
			appData.secondHost, appData.firstChessProgram, appData.firstHost);
		break;
	default:
		fprintf(fp, "\n");
		break;
	}
	fprintf(fp, "\n");

	for (i = BOARD_SIZE - 1; i >= 0; i--) {
		for (j = 0; j < BOARD_SIZE; j++) {
			fprintf(fp, "%c", pieceToChar[(int) boards[currentMove][i][j]]);
			fputc(j == BOARD_SIZE - 1 ? '\n' : ' ', fp);
		}
	}

	fclose(fp);
}

void
TwoMachinesPlayProc()
{
	int i;

	if ((gameMode == EndOfGame) || (twoProgramState == True))
		return;

	if (!WHITE_ON_MOVE) {
		DisplayMessage("White must be on move");
		return;
	}

	twoProgramState = True;

	if (matchMode == MatchFalse) {
		switch (gameMode) {
		case MachinePlaysWhite:
		case MachinePlaysBlack:
		case PauseGame:
		case TwoMachinesPlay:
		case PlayFromGameFile:
			return;
		case ForceMoves:
			matchMode = MatchOpening;
			break;
		case SetupPosition:
			matchMode = MatchPosition;
			break;
		case BeginningOfGame:
		default:
			matchMode = MatchInit;
			break;
		}
	}

	flipView = forwardForce = False;
	firstMove = False;
	DisplayClocks(ResetTimers);
	DisplayClocks(ReStartTimers);

	switch (matchMode) {
	case MatchOpening:
		if (firstProgramXID == NULL) {
			if (appData.readGameFile == NULL) {
				DisplayMessage("Select game file first");
				return;
			}
			InitChessProgram(appData.firstHost, appData.firstChessProgram,
				&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
				&firstProgramXID);
			if (!PlayFromGameFileProc(appData.readGameFile)) {
				ShutdownChessPrograms("Bad game file");
				return;
			}
			DrawPosition(boardWidget, (XExposeEvent *) NULL);
		}
		InitChessProgram(appData.secondHost, appData.secondChessProgram,
			&secondProgramPID, &toSecondProgFP, &fromSecondProgFP,
			&secondProgramXID);
		SendToProgram("force\n", toSecondProgFP);
		for (i = 0; i < currentMove; i++)
			SendToProgram(moveList[i], toSecondProgFP);
		lastGameMode = gameMode = TwoMachinesPlay;
		firstMove = True;
		SendToProgram(appData.whiteString, toSecondProgFP);
		break;
	case MatchPosition:
		if (firstProgramXID == NULL) {
			if (appData.readPositionFile == NULL) {
				DisplayMessage("Select position file first");
				return;
			}
			InitChessProgram(appData.firstHost, appData.firstChessProgram,
				&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
				&firstProgramXID);
			if (!SetupPositionFromFileProc(appData.readPositionFile))
				return;
		}
		InitChessProgram(appData.secondHost, appData.secondChessProgram,
			&secondProgramPID, &toSecondProgFP, &fromSecondProgFP,
			&secondProgramXID);
		SendCurrentBoard(toSecondProgFP);
		lastGameMode = gameMode = TwoMachinesPlay;
		firstMove = True;
		SendToProgram(appData.whiteString, toSecondProgFP);
		break;
	case MatchInit:
		InitPosition();
		if (firstProgramXID == NULL)
			InitChessProgram(appData.firstHost, appData.firstChessProgram,
				&firstProgramPID, &toFirstProgFP, &fromFirstProgFP,
				&firstProgramXID);
		InitChessProgram(appData.secondHost, appData.secondChessProgram,
			&secondProgramPID, &toSecondProgFP, &fromSecondProgFP,
			&secondProgramXID);
		lastGameMode = gameMode = TwoMachinesPlay;
		SendToProgram(appData.whiteString, toSecondProgFP);
	default:
		break;
	}
}

void
PauseProc()
{
	static GameMode previous_mode = PauseGame;

	switch (gameMode) {
	case ForceMoves:
	case EndOfGame:
		return;
	case PauseGame:
		gameMode = previous_mode;
		previous_mode = PauseGame;
		DisplayClocks(ReStartTimers);
		DisplayMessage("");
		break;
	case PlayFromGameFile:
		if (readGameXID == NULL)
			readGameXID = XtAppAddTimeOut(appContext,
				(int) (1000 * appData.timeDelay), ReadGameFile, NULL);
		else {
			XtRemoveTimeOut(readGameXID);
			readGameXID = NULL;
		}
		DisplayMessage("Pausing");
		break;
	case BeginningOfGame:		/* play game from files end here at the end */
	case MachinePlaysWhite:
	case MachinePlaysBlack:
	case TwoMachinesPlay:
	case SetupPosition:
		if (currentMove == 0)	/* don't pause if no one has moved */
			return;
		previous_mode = gameMode;
		gameMode = PauseGame;
		DisplayClocks(StopTimers);
		DisplayMessage("Pausing");
		break;
	}
}

void
GameProc()
{
	if (gameMode != BeginningOfGame)
		return;
	if (appData.readGameFile == NULL)
		FileNamePopUp("Game file name?", VOID_PROC PlayFromGameFileProc);
	else
		(void) PlayFromGameFileProc(appData.readGameFile);
}

void
Iconify()
{
	Arg args[1];

	fromX = fromY = -1;

	XtSetArg(args[0], XtNiconic, True);
	XtSetValues(shellWidget, args, 1);
}

void
SendToProgram(message, fp)
	char *message;
	FILE *fp;
{
	if (xboardDebug)
		fprintf(stderr, "Sending to %s: %s\n",
			fp == toFirstProgFP ? "first" : "second", message);

	if (message[strlen(message) - 1] != '\n')
		fprintf(fp, "\n%s\n", message);
	else
		fputs(message, fp);
}

void
ReceiveFromProgram(fp)
	FILE *fp;
{
	char message[MSG_SIZ], *end_str;

	if (fgets(message, MSG_SIZ, fp) == NULL)
		return;

	if ((end_str = (char *) strchr(message, '\r')) != NULL)
		*end_str = '\0';
	if ((end_str = (char *) strchr(message, '\n')) != NULL)
		*end_str = '\0';

	if (xboardDebug)
		fprintf(stderr, "Received from %s: %s\n",
			fp == fromFirstProgFP ? "first" : "second", message);
	HandleMachineMove(message);
}

void
DisplayMessage(message)
	char *message;
{
	Arg arg;

	XtSetArg(arg, XtNlabel, message);
	XtSetValues(messageWidget, &arg, 1);
}

/*
 * DisplayClocks manages the game clocks.
 *
 * In tournament play, black starts the clock and then white makes a move.
 * DisplayClocks starts black's clock when white makes the first move.
 * Also, DisplayClocks doesn't account for network lag so it could get
 * out of sync with GNU Chess's clock -- but then, referees are always right.
 */
void
DisplayClocks(clock_mode)
	int clock_mode;
{
	switch (clock_mode) {
	case ResetTimers:
		if (timerXID != NULL) {
			XtRemoveTimeOut(timerXID);
			timerXID = NULL;
		}

		whiteTimeRemaining = appData.timeControl * 60;
		DisplayTimerLabel(whiteTimerWidget, "White", whiteTimeRemaining);

		blackTimeRemaining = appData.timeControl * 60;
		DisplayTimerLabel(blackTimerWidget, "Black", blackTimeRemaining);
		break;
	case DisplayTimers:
		if (gameMode == PauseGame)
			return;
		if (appData.clockMode) {
			if (WHITE_ON_MOVE) {
				if ((currentMove > 0) || (gameMode == TwoMachinesPlay))
					whiteTimeRemaining--;
				if (whiteTimeRemaining <= 0) {
					whiteTimeRemaining = 0;
					DisplayMessage("White's flag dropped");
				}
			} else {
				blackTimeRemaining--;
				if (blackTimeRemaining <= 0) {
					blackTimeRemaining = 0;
					DisplayMessage("Black's flag dropped");
				}
			}
		}

		DisplayTimerLabel(whiteTimerWidget, "White", whiteTimeRemaining);
		DisplayTimerLabel(blackTimerWidget, "Black", blackTimeRemaining);

		/*
		 * reset clocks when time control is acheived
		 */
		if (appData.clockMode && (currentMove != 0)
			&& ((currentMove % (appData.movesPerSession * 2)) == 0)
			&& (whiteTimeRemaining > 0) && (blackTimeRemaining > 0)) {
			whiteTimeRemaining = appData.timeControl * 60;
			blackTimeRemaining = appData.timeControl * 60;
		}

		if (timerXID != NULL) {
			XtRemoveTimeOut(timerXID);
			timerXID = NULL;
		}
		if (appData.clockMode)
			timerXID = XtAppAddTimeOut(appContext,
				1000, DisplayClocks, DisplayTimers);
		break;
	case StopTimers:
		if (timerXID == NULL)
			return;
		XtRemoveTimeOut(timerXID);
		timerXID = NULL;
		break;
	case ReStartTimers:
		if (timerXID != NULL)
			return;

		DisplayTimerLabel(whiteTimerWidget, "White", whiteTimeRemaining);
		DisplayTimerLabel(blackTimerWidget, "Black", blackTimeRemaining);

		if (appData.clockMode)
			timerXID = XtAppAddTimeOut(appContext,
				1000, DisplayClocks, DisplayTimers);
		break;
	}
}

void
DisplayTimerLabel(w, color, timer)
	Widget w;
	char *color;
	time_t timer;
{
	char buf[MSG_SIZ];
	Arg args[3];

	if (appData.clockMode) {
		sprintf(buf, "%s: %s", color, TimeString(timer));
		XtSetArg(args[0], XtNlabel, buf);
	} else
		XtSetArg(args[0], XtNlabel, color);

	if (((color[0] == 'B') && WHITE_ON_MOVE)
		|| ((color[0] == 'W') && !WHITE_ON_MOVE)) {
		XtSetArg(args[1], XtNbackground, timerForegroundPixel);
		XtSetArg(args[2], XtNforeground, timerBackgroundPixel);
	} else {
		XtSetArg(args[1], XtNbackground, timerBackgroundPixel);
		XtSetArg(args[2], XtNforeground, timerForegroundPixel);
	}

	XtSetValues(w, args, 3);
}

char *
TimeString(tm)
	time_t tm;
{
	int second, minute, hour, day;
	static char buf[32];

	if (tm >= (60 * 60 * 24)) {
		day = (int) (tm / (60 * 60 * 24));
		tm -= day * 60 * 60 * 24;
	} else
		day = 0;

	if (tm >= (60 * 60)) {
		hour = (int) (tm / (60 * 60));
		tm -= hour * 60 * 60;
	} else
		hour = 0;

	if (tm >= 60) {
		minute = (int) (tm / 60);
		tm -= minute * 60;
	} else
		minute = 0;

	second = tm % 60;

	if (day > 0)
		sprintf(buf, " %d:%02d:%02d:%02d ", day, hour, minute, second);
	else if (hour > 0)
		sprintf(buf, " %d:%02d:%02d ", hour, minute, second);
	else
		sprintf(buf, " %2d:%02d ", minute, second);

	return buf;
}

void
Usage()
{
	fprintf(stderr, "Usage %s:\n", programName);
	fprintf(stderr, "\tstandard Xt options\n");
	fprintf(stderr, "\t-wpc or -whitePieceColor color\n");
	fprintf(stderr, "\t-bpc or -blackPieceColor color\n");
	fprintf(stderr, "\t-lsc or -lightSquareColor color\n");
	fprintf(stderr, "\t-dsc or -darkSquareColor color\n");
	fprintf(stderr, "\t-mps or -movesPerSession moves\n");
	fprintf(stderr, "\t-fcp or -firstChessProgram program_name\n");
	fprintf(stderr, "\t-scp or -secondChessProgram program_name\n");
	fprintf(stderr, "\t-fh or -firstHost host_name\n");
	fprintf(stderr, "\t-sh or -secondHost host_name\n");
	fprintf(stderr, "\t-spb or -solidPawnBitmap file_name\n");
	fprintf(stderr, "\t-srb or -solidRookBitmap file_name\n");
	fprintf(stderr, "\t-sbb or -solidBishopBitmap file_name\n");
	fprintf(stderr, "\t-skb or -solidKnightBitmap file_name\n");
	fprintf(stderr, "\t-sqb or -solidQueenBitmap file_name\n");
	fprintf(stderr, "\t-skb or -solidKingBitmap file_name\n");
	fprintf(stderr, "\t-opb or -outlinePawnBitmap file_name\n");
	fprintf(stderr, "\t-orb or -outlineRookBitmap file_name\n");
	fprintf(stderr, "\t-obb or -outlineBishopBitmap file_name\n");
	fprintf(stderr, "\t-okb or -outlineKnightBitmap file_name\n");
	fprintf(stderr, "\t-oqb or -outlineQueenBitmap file_name\n");
	fprintf(stderr, "\t-okb or -outlineKingBitmap file_name\n");
	fprintf(stderr, "\t-rsh or -remoteShell shell_name\n");
	fprintf(stderr, "\t-td or -timeDelay seconds\n");
	fprintf(stderr, "\t-tc or -timeControl minutes\n");
	fprintf(stderr, "\t-rgf or -readGameFile file_name\n");
	fprintf(stderr, "\t-rpf or -readPositionFile file_name\n");
	fprintf(stderr, "\t-sgf or -saveGameFile file_name\n");
	fprintf(stderr, "\t-spf or -savePositionFile file_name\n");
	fprintf(stderr, "\t-mm or -matchMode (False | Init | Position | Opening)\n");
	fprintf(stderr, "\t-mono or -monoMode (True | False)\n");
	fprintf(stderr, "\t-debug or -debugMode (True | False)\n");
	fprintf(stderr, "\t-clock or -clockMode (True | False)\n");
	fprintf(stderr, "\t-big or -bigSizeMode (True | False)\n");
	fprintf(stderr, "\t-st or -searchTime seconds\n");

	exit(0);
}

/*
 * This is necessary because some C libraries aren't ANSI C compliant yet.
 */
char *
StrStr(string, match)
	char *string, *match;
{
	int i, length;

	length = strlen(match);

	for (i = strlen(string) - length; i >= 0; i--, string++)
		if (!strncmp(match, string, (size_t) length))
			return string;

	return NULL;
}

#if	SYSTEM_FIVE || SYSV
char *
PseudoTTY(ptyv)
	int *ptyv;
{
	struct stat stb;
	int c, i;

	for (c = 'p'; c <= 'z'; c++)
		for (i = 0; i < 16; i++) {
#ifdef	HPUX
			sprintf(ptyname, "/dev/ptym/pty%c%x", c, i);
#else
#ifdef	RTU
			sprintf(ptyname, "/dev/pty%x", i);
#else
			sprintf(ptyname, "/dev/pty%c%x", c, i);
#endif
#endif

#ifdef	IRIS
			*ptyv = open("/dev/ptc", O_RDWR | O_NDELAY, 0);
			if (*ptyv < 0)
				return NULL;
			if (fstat(*ptyv, &stb) < 0)
				return NULL;
#else
			if (stat(ptyname, &stb) < 0)
				return NULL;
			*ptyv = open(ptyname, O_RDWR | O_NDELAY, 0);
#endif

			if (*ptyv >= 0) {
#ifdef	HPUX
				sprintf(ptyname, "/dev/pty/tty%c%x", c, i);
#else
#ifdef	RTU
				sprintf(ptyname, "/dev/ttyp%x", i);
#else
#ifdef	IRIS
				sprintf(ptyname, "/dev/ttyq%d", minor(stb.st_rdev));
#else
				sprintf(ptyname, "/dev/tty%c%x", c, i);
#endif
#endif
#endif

#ifndef	UNIPLUS
				if (access(ptyname, 6) != 0) {
					close(*ptyv);
#ifdef	IRIS
					return NULL;
#else
					continue;
#endif
				}
#endif

#ifdef	FIONBIO
			{
				int on = 1;
				ioctl(*ptyv, FIONBIO, &on);
			}
#endif

#ifdef	IBMRTAIX
			signal(SIGHUP, SIG_IGN);
#endif
			return ptyname;
		}
	}

	return NULL;
}

#else

void
CatchPipeSignal()
{
	/*
	 * If a game is just being played out and there is no reason for
	 * gnuchessr to be used, so ignore pipe signals.  This allows people
	 * to use xboard as a chess board without need for gnuchess.
	 */
	switch (gameMode) {
	case ForceMoves:
	case PlayFromGameFile:
	case SetupPosition:
	case BeginningOfGame:
		break;
	default:
		fprintf(stderr, "Caught pipe signal\n");
		exit(0);
	}
}
#endif
