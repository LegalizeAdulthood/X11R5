%{
/*
 * parser.l -- lex parser of algebraic chess moves for XBoard
 *
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
 *
 * This parser handles all forms of promotion.
 * XBoard and GnuChess only handle queen promotion.
 * The parser resolves ambiguous moves by searching and check-testing.
 * It also parses comments of the form [anything] and ! to end of line.
 */
#include "xboard.h"
#include <ctype.h>
#include <string.h>

enum { False, True };

#define A_I(ch)			(tolower(ch) - 'a')
#define NO_CONSTRAINT	-1
#undef YYLMAX
#define YYLMAX			2048
#undef input

#define KING_TEST(r, f, piece) { \
	if ((r >= 0) && (r < BOARD_SIZE) && (f >= 0) && (f < BOARD_SIZE)) { \
		if (boards[currentMove][r][f] == (ChessSquare) piece) { \
			currentMoveString[0] = f + 'a'; \
			currentMoveString[1] = r + '1'; \
			break; \
		} \
	} \
}

#define PIECE_TEST(r, f, piece) { \
	if ((r >= 0) && (r < BOARD_SIZE) && (f >= 0) && (f < BOARD_SIZE)) { \
		if (boards[currentMove][r][f] == (ChessSquare) piece) { \
			if (! CheckTest(rank, file, r, f)) { \
				currentMoveString[0] = f + 'a'; \
				currentMoveString[1] = r + '1'; \
				break; \
			} \
		} \
	} \
}

#define PROMOTION_PIECE(p) { \
	currentMoveString[5] = '\n'; \
	currentMoveString[6] = '\0'; \
	switch (p) { \
	case 'n': case 'N': \
		currentMoveString[4] = 'n'; \
		return WHITE_ON_MOVE \
			? (int) WhitePromotionKnight : (int) BlackPromotionKnight; \
	case 'b': case 'B': \
		currentMoveString[4] = 'b'; \
		return WHITE_ON_MOVE \
			? (int) WhitePromotionBishop : (int) BlackPromotionBishop; \
	case 'r': case 'R': \
		currentMoveString[4] = 'r'; \
		return WHITE_ON_MOVE \
			? (int) WhitePromotionRook : (int) BlackPromotionRook; \
	case 'q': case 'Q': \
		currentMoveString[4] = 'q'; \
		return WHITE_ON_MOVE \
			? (int) WhitePromotionQueen : (int) BlackPromotionQueen; \
	default: \
		return (int) BadMove; \
	} \
}

extern Board	boards[MAX_MOVES];
extern int		currentMove;
extern int		xboardDebug;
extern FILE		*gameFileFP;
char			currentMoveString[YYLMAX];

static int RookSearch P((int *rank, int *file,
	ChessSquare piece, Board b, int recurse, int constraint));
static int BishopSearch P((int *rank, int *file,
	ChessSquare piece, Board b, int recurse, int constraint));
static int CheckTest P((int rt, int ft, int rf, int ff));
static int yywrap P((void));
static int input P((void));
extern void CopyBoard P((Board to, Board from));

%}
%%

!.*$						{ return (int) Comment; }	/* combining doesn't work */
\[[^\]]*\]					{ return (int) Comment; }
[ \t\n]						{ ; }				/* skip white space */

[a-hA-H][1-8][a-hA-H][1-8]|[a-hA-H][1-8][a-hA-H][1-8]\(?[RrBbNnQq]\)?|[a-hA-H][1-8][xX-][a-hA-H][1-8]|[a-hA-H][1-8][xX-][a-hA-H][1-8]\(?[RrBbNnQq]\)?	{
/*
 * Simple algebraic move with promotion
 */
		/*
		 * remove the [xX-] to simplify the logic
		 */
		if ((yytext[2] == 'x') || (yytext[2] == 'X') || (yytext[2] == '-')) {
			int i;

			for (i = 2, yyleng--; i < yyleng + 1; i++)
				yytext[i] = yytext[i + 1];
		}

		strcpy(currentMoveString, yytext);
		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		if (yyleng == 4)
			return (int) NormalMove;
		if (yyleng > 5) {
			PROMOTION_PIECE(yytext[5]);
		} else {
			PROMOTION_PIECE(yytext[4]);
		}
	}

[a-hA-H][1-8]|[a-hA-H][1-8]\(?[RrBbNnQq]\)?	{
/*
 * Pawn move with promotion
 */
		int rank = yytext[1] - '1', file = A_I(yytext[0]);

		currentMoveString[0] = yytext[0];
		currentMoveString[1] = yytext[1];
		currentMoveString[2] = yytext[0];
		currentMoveString[3] = yytext[1];
		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		if (WHITE_ON_MOVE) {
			if (boards[currentMove][rank - 1][file] == WhitePawn)
				currentMoveString[1]--;
			else
				currentMoveString[1] -= 2;
		} else {
			if (boards[currentMove][rank + 1][file] == BlackPawn)
				currentMoveString[1]++;
			else
				currentMoveString[1] += 2;
		}

		if (yyleng == 2)
			return (int) NormalMove;
		if (yyleng > 3) {
			PROMOTION_PIECE(yytext[3]);
		} else {
			PROMOTION_PIECE(yytext[2]);
		}
	}

[a-h][a-h]|[a-h][a-h]\([RrBbNnQq]\)|[a-h][xX][a-h]|[a-h][xX][a-h]\([RrBbNnQq]\)	{
/*
 * Pawn capture with promotion
 */
		int i, p, file0, file1;

		/*
		 * remove the [xX] to simplify the logic
		 */
		if ((yytext[1] == 'x') || (yytext[1] == 'X'))
			for (i = 1, yyleng--; i < yyleng + 1; i++)
				yytext[i] = yytext[i + 1];

		file0 = A_I(yytext[0]);
		file1 = A_I(yytext[1]);

		currentMoveString[0] = yytext[0];
		currentMoveString[2] = yytext[1];
		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		if (WHITE_ON_MOVE) {
			for (i = 1; i < BOARD_SIZE - 1; i++) {
				if (boards[currentMove][i][file0] == WhitePawn) {
					p = (int) boards[currentMove][i + 1][file1];
					if (((i == 4) && (p == (int) EmptySquare)	/* en passant */
						&& (boards[currentMove][i][file1] == BlackPawn)) ||
						((p >= (int) BlackPawn) && (p <= (int) BlackQueen))) {

						currentMoveString[1] = '1' + i;
						currentMoveString[3] = '1' + i + 1;
						if (p == EmptySquare)
							return (int) WhiteCapturesEnPassant;
						if (yyleng == 2)
							return (int) NormalMove;
						PROMOTION_PIECE(yytext[3]);
					}
				}
			}
		} else {
			for (i = 1; i < BOARD_SIZE - 1; i++) {
				if (boards[currentMove][i][file0] == BlackPawn) {
					p = (int) boards[currentMove][i - 1][file1];
					if (((i == 3) && (p == (int) EmptySquare)	/* en passant */
						&& (boards[currentMove][i][file1] == WhitePawn)) ||
						((p >= (int) WhitePawn) && (p <= (int) WhiteQueen))) {

						currentMoveString[1] = '1' + i;
						currentMoveString[3] = '1' + i - 1;
						if (p == EmptySquare)
							return (int) BlackCapturesEnPassant;
						if (yyleng == 2)
							return (int) NormalMove;
						PROMOTION_PIECE(yytext[3]);
					}
				}
			}
		}

		return (int) BadMove;
	}

[a-h][a-h][1-8]|[a-h][a-h][1-8]\([RrBbNnQq]\)|[a-h][xX][a-h][1-8]|[a-h][xX][a-h][1-8]\([RrBbNnQq]\)	{
/*
 * ambiguous Pawn capture with promotion
 */
		int i;

		/*
		 * remove the [xX] to simplify the logic
		 */
		if ((yytext[1] == 'x') || (yytext[1] == 'X'))
			for (i = 1, yyleng--; i < yyleng + 1; i++)
				yytext[i] = yytext[i + 1];

		currentMoveString[0] = yytext[0];
		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		if (WHITE_ON_MOVE) {
			if ((yytext[2] == '5') &&
				(boards[currentMove][4][A_I(yytext[0])] == WhitePawn) &&
				(boards[currentMove][5][A_I(yytext[1])] == EmptySquare)) {
				currentMoveString[1] = '5';
				currentMoveString[2] = yytext[1];
				currentMoveString[3] = '6';
				return (int) WhiteCapturesEnPassant;
			}
		} else {
			if ((yytext[2] == '4') &&
				(boards[currentMove][3][A_I(yytext[0])] == BlackPawn) &&
				(boards[currentMove][2][A_I(yytext[1])] == EmptySquare)) {
				currentMoveString[1] = '4';
				currentMoveString[2] = yytext[1];
				currentMoveString[3] = '3';
				return (int) BlackCapturesEnPassant;
			}
		}

		currentMoveString[2] = yytext[1];
		currentMoveString[3] = yytext[2];
		if (WHITE_ON_MOVE)
			currentMoveString[1] = yytext[2] - 1;
		else
			currentMoveString[1] = yytext[2] + 1;

		if (yyleng == 3)
			return (int) NormalMove;

		PROMOTION_PIECE(yytext[4]);
	}

[RrBbNnQqKk][a-hA-H][1-8]|[RrBbNnQqKk][xX][a-hA-H][1-8]	{
/*
 * piece move
 */
		int p, rank, file;

		if ((yytext[1] == 'x') || (yytext[1] == 'X')) {
			currentMoveString[2] = yytext[2];
			currentMoveString[3] = yytext[3];
			rank = yytext[3] - '1'; file = A_I(yytext[2]);
		} else {
			currentMoveString[2] = yytext[1];
			currentMoveString[3] = yytext[2];
			rank = yytext[2] - '1'; file = A_I(yytext[1]);
		}

		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		switch (yytext[0]) {
		case 'R': case 'r':
			if (!RookSearch(&rank, &file, WHITE_ON_MOVE ? WhiteRook
				: BlackRook, boards[currentMove], (int) True, NO_CONSTRAINT))
				return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		case 'B': case 'b':
			if (!BishopSearch(&rank, &file, WHITE_ON_MOVE ? WhiteBishop
				: BlackBishop, boards[currentMove], (int) True, NO_CONSTRAINT))
				return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		case 'N': case 'n':
			p = (int) (WHITE_ON_MOVE ? WhiteKnight : BlackKnight);
			PIECE_TEST(rank - 2, file - 1, p);
			PIECE_TEST(rank - 1, file - 2, p);
			PIECE_TEST(rank + 1, file - 2, p);
			PIECE_TEST(rank + 2, file - 1, p);
			PIECE_TEST(rank + 2, file + 1, p);
			PIECE_TEST(rank + 1, file + 2, p);
			PIECE_TEST(rank - 1, file + 2, p);
			PIECE_TEST(rank - 2, file + 1, p);
			return (int) BadMove;
			break;
		case 'Q': case 'q':
			if (!RookSearch(&rank, &file, WHITE_ON_MOVE ? WhiteQueen
				: BlackQueen, boards[currentMove], (int) True, NO_CONSTRAINT))
				if (!BishopSearch(&rank, &file, WHITE_ON_MOVE ? WhiteQueen
					: BlackQueen, boards[currentMove], (int) True, NO_CONSTRAINT))
					return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		case 'K': case 'k':
			p = (int) (WHITE_ON_MOVE ? WhiteKing : BlackKing);
			KING_TEST(rank - 1, file - 1, p);
			KING_TEST(rank, file - 1, p);
			KING_TEST(rank + 1, file - 1, p);
			KING_TEST(rank + 1, file, p);
			KING_TEST(rank + 1, file + 1, p);
			KING_TEST(rank, file + 1, p);
			KING_TEST(rank - 1, file + 1, p);
			KING_TEST(rank - 1, file, p);
			break;
		}

		return (int) NormalMove;
	}

[RrBbNnQqKk][a-hA-H0-7][a-hA-H][1-8]|[RrBbNnQqKk][a-hA-H0-7][xX][a-hA-H][1-8]	{
/*
 * ambiguous piece move
 */
		int p, c, rank, file;

		if ((yytext[2] == 'x') || (yytext[2] == 'X')) {
			currentMoveString[2] = yytext[3];
			currentMoveString[3] = yytext[4];
			rank = yytext[4] - '1'; file = A_I(yytext[3]);
		} else {
			currentMoveString[2] = yytext[2];
			currentMoveString[3] = yytext[3];
			rank = yytext[3] - '1'; file = A_I(yytext[2]);
		}

		currentMoveString[4] = '\n';
		currentMoveString[5] = '\0';

		c = tolower(yytext[1]);

		switch (yytext[0]) {
		case 'R': case 'r':
			if (!RookSearch(&rank, &file, WHITE_ON_MOVE ? WhiteRook
				: BlackRook, boards[currentMove], (int) True, yytext[1]))
				return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		case 'B': case 'b':
			if (!BishopSearch(&rank, &file, WHITE_ON_MOVE ? WhiteBishop
				: BlackBishop, boards[currentMove], (int) True, yytext[1]))
				return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		case 'N': case 'n':
			p = (int) (WHITE_ON_MOVE ? WhiteKnight : BlackKnight);
			if (isalpha(c)) {
				c -= 'a';
				if ((file - 1) == c) {
					PIECE_TEST(rank - 2, file - 1, p);
					PIECE_TEST(rank + 2, file - 1, p);
				}
				if ((file - 2) == c) {
					PIECE_TEST(rank - 1, file - 2, p);
					PIECE_TEST(rank + 1, file - 2, p);
				}
				if ((file + 1) == c) {
					PIECE_TEST(rank + 2, file + 1, p);
					PIECE_TEST(rank - 2, file + 1, p);
				}
				if ((file + 2) == c) {
					PIECE_TEST(rank + 1, file + 2, p);
					PIECE_TEST(rank - 1, file + 2, p);
				}
			} else {
				c -= '1';
				if ((rank - 2) == c) {
					PIECE_TEST(rank - 2, file - 1, p);
					PIECE_TEST(rank - 2, file + 1, p);
				}
				if ((rank - 1) == c) {
					PIECE_TEST(rank - 1, file - 2, p);
					PIECE_TEST(rank - 1, file + 2, p);
				}
				if ((rank + 2) == c) {
					PIECE_TEST(rank + 2, file - 1, p);
					PIECE_TEST(rank + 2, file + 1, p);
				}
				if ((rank + 1) == c) {
					PIECE_TEST(rank + 1, file - 2, p);
					PIECE_TEST(rank + 1, file + 2, p);
				}
			}
			break;
		case 'Q': case 'q':
			if (!RookSearch(&rank, &file, WHITE_ON_MOVE ? WhiteQueen
				: BlackQueen, boards[currentMove], (int) True, yytext[1]))
				if (!BishopSearch(&rank, &file, WHITE_ON_MOVE ? WhiteQueen
					: BlackQueen, boards[currentMove], (int) True, yytext[1]))
					return (int) BadMove;

			currentMoveString[0] = file + 'a';
			currentMoveString[1] = rank + '1';

			break;
		}

		return (int) NormalMove;
	}

000|0-0-0|ooo|OOO|o-o-o|O-O-O	{
		strcpy(currentMoveString, "O-O-O\n");
		if (WHITE_ON_MOVE)
			return (int) WhiteQueenSideCastle;
		else
			return (int) BlackQueenSideCastle;
	}

00|0-0|oo|OO|o-o|O-O	{
		strcpy(currentMoveString, "O-O\n");
		if (WHITE_ON_MOVE)
			return (int) WhiteKingSideCastle;
		else
			return (int) BlackKingSideCastle;
	}

[Rr][Ee][Ss][Ii][Gg][Nn]	{
		if (WHITE_ON_MOVE)
			return (int) BlackWins;
		else
			return (int) WhiteWins;
	}

[Mm][Aa][Tt][Ee]			{
		if (WHITE_ON_MOVE)
			return (int) WhiteWins;
		else
			return (int) BlackWins;
	}

[Dd][Rr][Aa][Ww]|1\/2		{ return (int) GameIsDrawn; }
[Ww][Hh][Ii][Tt][Ee]		{ return (int) WhiteWins; }
[Bb][Ll][Aa][Cc][Kk]		{ return (int) BlackWins; }
1-0							{ return (int) WhiteWins; }
0-1							{ return (int) BlackWins; }

[1-9][0-9]*\.|[1-9][0-9]*	{			/* Parse and skip move numbers */
	if ((yyleng == 1) && (yytext[0] == '1'))
		return (int) StartGame;
	if ((yyleng == 2) && (yytext[0] == '1') && (yytext[1] == '.'))
		return (int) StartGame;
}

.							{ ; }		/* Skip everything else */

%%

static int
RookSearch(rank, file, piece, b, recurse, constraint)
	int *rank, *file, recurse, constraint;
	ChessSquare piece;
	Board b;
{
	int i, r = *rank, f = *file;

	/*
	 * Start from the target space and work outwards towards the piece.
	 * This is necessary for check testing.
	 */
	for (i = f + 1;; i++) {
		if (i >= BOARD_SIZE)
			break;
		if (isalpha(constraint) && (i != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (i != (constraint - '1')))
			continue;
		if ((b[r][i] != EmptySquare) && (b[r][i] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, r, i))
				continue;
		if (b[r][i] == piece) {
			*file = i;
			return (int) True;
		}
	}

	for (i = f - 1;; i--) {
		if (i < 0)
			break;
		if (isalpha(constraint) && (i != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (i != (constraint - '1')))
			continue;
		if ((b[r][i] != EmptySquare) && (b[r][i] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, r, i))
				continue;
		if (b[r][i] == piece) {
			*file = i;
			return (int) True;
		}
	}

	for (i = r + 1;; i++) {
		if (i >= BOARD_SIZE)
			break;
		if (isdigit(constraint) && (i != (constraint - '1')))
			continue;
		if (isalpha(constraint) && (i != A_I(constraint)))
			continue;
		if ((b[i][f] != EmptySquare) && (b[i][f] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, f))
				continue;
		if (b[i][f] == piece) {
			*rank = i;
			return (int) True;
		}
	}

	for (i = r - 1;; i--) {
		if (i < 0)
			break;
		if (isdigit(constraint) && (i != (constraint - '1')))
			continue;
		if (isalpha(constraint) && (i != A_I(constraint)))
			continue;
		if ((b[i][f] != EmptySquare) && (b[i][f] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, f))
				continue;
		if (b[i][f] == piece) {
			*rank = i;
			return (int) True;
		}
	}

	return (int) False;
}

static int
BishopSearch(rank, file, piece, b, recurse, constraint)
	int *rank, *file, recurse, constraint;
	ChessSquare piece;
	Board b;
{
	int i, j, r = *rank, f = *file;

	/*
	 * Start from the target space and work outwards towards the piece.
	 * This is necessary for check testing.
	 */
	for (i = r + 1, j = f + 1;; i++, j++) {
		if ((i >= BOARD_SIZE) || (j >= BOARD_SIZE))
			break;
		if (isalpha(constraint) && (j != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (j != (constraint - '1')))
			continue;
		if ((b[i][j] != EmptySquare) && (b[i][j] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, j))
				continue;
		if (b[i][j] == piece) {
			*rank = i;
			*file = j;
			return (int) True;
		}
	}

	for (i = r + 1, j = f - 1;; i++, j--) {
		if ((i >= BOARD_SIZE) || (j < 0))
			break;
		if (isalpha(constraint) && (j != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (j != (constraint - '1')))
			continue;
		if ((b[i][j] != EmptySquare) && (b[i][j] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, j))
				continue;
		if (b[i][j] == piece) {
			*rank = i;
			*file = j;
			return (int) True;
		}
	}

	for (i = r - 1, j = f + 1;; i--, j++) {
		if ((i < 0) || (j >= BOARD_SIZE))
			break;
		if (isalpha(constraint) && (j != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (j != (constraint - '1')))
			continue;
		if ((b[i][j] != EmptySquare) && (b[i][j] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, j))
				continue;
		if (b[i][j] == piece) {
			*rank = i;
			*file = j;
			return (int) True;
		}
	}

	for (i = r - 1, j = f - 1;; i--, j--) {
		if ((i < 0) || (j < 0))
			break;
		if (isalpha(constraint) && (j != A_I(constraint)))
			continue;
		if (isdigit(constraint) && (j != (constraint - '1')))
			continue;
		if ((b[i][j] != EmptySquare) && (b[i][j] != piece))
			break;
		if (recurse)
			if (CheckTest(r, f, i, j))
				continue;
		if (b[i][j] == piece) {
			*rank = i;
			*file = j;
			return (int) True;
		}
	}

	return (int) False;
}

static int
CheckTest(rt, ft, rf, ff)
	int rt, ft, rf, ff;
{
	int rk, fk, rank, file;
	Board b;

	CopyBoard(b, boards[currentMove]);
	b[rt][ft] = b[rf][ff];
	b[rf][ff] = EmptySquare;

	/*
	 * the bad move must have uncovered check by a rook, bishop or queen.
	 */
	if (WHITE_ON_MOVE) {				/* Is the white king in check? */
		for (rk = 0; rk < BOARD_SIZE; rk++) {
			for (fk = 0; fk < BOARD_SIZE; fk++) {
				if (b[rk][fk] == WhiteKing) {
					rank = rk; file = fk;
					if (BishopSearch(&rank, &file, BlackBishop,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (RookSearch(&rank, &file, BlackRook,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (BishopSearch(&rank, &file, BlackQueen,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (RookSearch(&rank, &file, BlackQueen,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					return (int) False;
				}
			}
		}
	} else {							/* Is the black king in check? */
		for (rk = 0; rk < BOARD_SIZE; rk++) {
			for (fk = 0; fk < BOARD_SIZE; fk++) {
				if (b[rk][fk] == BlackKing) {
					rank = rk; file = fk;
					if (BishopSearch(&rank, &file, WhiteBishop,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (RookSearch(&rank, &file, WhiteRook,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (BishopSearch(&rank, &file, WhiteQueen,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					rank = rk; file = fk;
					if (RookSearch(&rank, &file, WhiteQueen,
						b, (int) False, NO_CONSTRAINT))
						return (int) True;
					return (int) False;
				}
			}
		}
	}

	return (int) False;
}

static int
yywrap()
{
	return (int) True;
}

static int
input()
{
	if (feof(gameFileFP))
		return 0;
	else
		return fgetc(gameFileFP);
}
