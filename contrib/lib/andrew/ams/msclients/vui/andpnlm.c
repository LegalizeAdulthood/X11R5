/* ********************************************************************** *\
  *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
  *        For full copyright information see:'andrew/config/COPYRITE'     *
  \* ********************************************************************** */
static char *andpnlm_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/andpnlm.c,v 2.17 1991/09/12 15:52:10 bobg Exp $";

/*
  *      C Panel Handling Package
  *	Hardware-dependent routines
  *	Workstation/Termcap version
  *
  */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/ams/msclients/vui/RCS/andpnlm.c,v 2.17 1991/09/12 15:52:10 bobg Exp $ */

#include <andrewos.h> /* sys/time.h */
#include <andyenv.h>
#include <system.h>
#include <curses.h>
#include <ctype.h>
#include <signal.h>

#include <vui.h>
#include <panel.h>
#include <keycodes.h>
#include <vuidebug.h>

extern unsigned char msg_row, error_row, last_row, past_col, opt_row, bar_row, cursor_row, cursor_col, Termcap_AttrTable[], *AttrTablePtr;

static char charmap[256];
static int nostandout;
PRIVATE char saved;
PRIVATE unsigned char saved_row;        /* saved Cursor screen row   */
PRIVATE unsigned char saved_col;        /* saved Cursor screen col   */

#define	MAX_TC_LEN  50

PRIVATE	char TCkr[MAX_TC_LEN+1];	/* arrow key right */
PRIVATE	char TCku[MAX_TC_LEN+1];	/* arrow key up */
PRIVATE	char TCkd[MAX_TC_LEN+1];	/* arrow key down */
PRIVATE	char TCkl[MAX_TC_LEN+1];	/* arrow key left */
PRIVATE	char TCkN[MAX_TC_LEN+1];	/* page down */
PRIVATE	char TCkP[MAX_TC_LEN+1];	/* page up */
PRIVATE	char TCkh[MAX_TC_LEN+1];	/* home key*/
PRIVATE	char TCk1[MAX_TC_LEN+1];	/* F1 key */
PRIVATE	char TCk2[MAX_TC_LEN+1];	/* F2 key */
PRIVATE	char TCkI[MAX_TC_LEN+1];	/* Insert key */
PRIVATE	char TCkR[MAX_TC_LEN+1];	/* scroll up */
PRIVATE	char TCkF[MAX_TC_LEN+1];	/* scroll forward */
PRIVATE	char TCkE[MAX_TC_LEN+1];	/* Clear to eol -- simulate end key */

PRIVATE int  PredefinedKeys[] = {
    KEYCODE_CTRL_TAB,
    KEYCODE_SHIFT_TAB,
    KEYCODE_CTRL_LEFT,
    KEYCODE_CTRL_HOME,
    KEYCODE_PAGE_UP,
    KEYCODE_F1,
    KEYCODE_F2,
    KEYCODE_ALT_F1,
    -1
};

typedef struct keypad_struct {
    char *str;
    char *termcap_id;
    int   keycode;
} KEYPAD;

PRIVATE KEYPAD KeyPad[] = {
    TCkr, "kr", KEYCODE_RIGHT,
    TCkl, "kl", KEYCODE_LEFT,
    TCku, "ku", KEYCODE_UP,
    TCkd, "kd", KEYCODE_DOWN,
    TCkN, "kN", KEYCODE_PAGE_DOWN,
    TCkP, "kP", KEYCODE_PAGE_UP,
    TCkh, "kh", KEYCODE_HOME,
    TCk1, "k1", KEYCODE_F1,
    TCk2, "k2", KEYCODE_F2,
    TCkI, "kI", KEYCODE_INSERT,
    TCkR, "kR", KEYCODE_PAGE_UP,
    TCkF, "kF", KEYCODE_PAGE_DOWN,
    TCkE, "kE", KEYCODE_END,
    NIL,  NIL,  -1
};


char	*tgetstr();

KeyHit()
{
    static struct timeval poll = {0,0};
    FILE *rf[1];

    rf[0] = stdin;
    return(fselect(1, rf, 0, 0, &poll));
}

KeyIn()
{
    int ch, i;

    refresh();
    ch = getch()  & 0xff;
    if (ch == ((unsigned char)EOF)) { /* ch is defined as int! curses doesn't return error. */
	if (feof(stdin))
	    kill(getpid(), SIGTERM); /* lost connection, EOF on input */
    }
    ch &= 0x7f;
    if (ch == 26) { /* ctrl-z */
	SuspendProcess();
    }
    if (ch == 27) { /* ESC key */
	ch = (getch() & 0x7f) + 256;
	i = 0;
	while(PredefinedKeys[i] != -1)
	    if (PredefinedKeys[i++] == ch) return(ch);
	ch = CheckKeypad(ch, TRUE);
    }
    return(ch);
}

CheckKeypad(chin, prefixed)
int chin, prefixed;
{
    int  endbuf, i, ThereisHope = TRUE;
    char chbuf[MAX_TC_LEN+1];

    if (prefixed) {
	chbuf[0] = 27;
	chbuf[1] = chin - 256;
	chbuf[2] = '\0';
    } else {
	chbuf[0] = chin;
	chbuf[1] = '\0';
    }

    endbuf = strlen(chbuf);

    while (endbuf < MAX_TC_LEN) {
	ThereisHope = FALSE;
	i = 0;
	while (KeyPad[i].str != NIL) {
	    if (strcmp(KeyPad[i].str, chbuf) == 0)
		return(KeyPad[i].keycode);
	    if ((prefixed) && (strncmp(KeyPad[i].str, chbuf, endbuf) == 0))
		ThereisHope = TRUE;
	    i++;
	}
	if (!ThereisHope) break;
	chbuf[endbuf++] = getch() & 0x7f;
	chbuf[endbuf]   = '\0';
    }

    return(chin);
}

SuspendProcess()
{
#ifdef vax
    /* vaxes have problem with CTRL-Z */
    if (!nostandout) standend();
    noraw();
    echo();
#endif /* vax */
#ifdef SIGTSTP
    printf("\nUse 'fg' to return to VUI.\n");
    kill(getpid(),SIGTSTP);
#endif /* SIGTSTP */
#ifdef vax
    noecho();
    raw();
    wrefresh(curscr);
#endif /* vax */
}

SetCursorPosn()
{
    move(cursor_row, cursor_col);
}

CursorOff ()
{
    if (saved) return;
    saved_row = cursor_row;
    cursor_row = error_row;
    saved_col = cursor_col;
    cursor_col = 0;
    saved = TRUE;
    move(cursor_row, cursor_col);
}

CursorOn ()
{
    if (saved && cursor_row == error_row && cursor_col == 0) {
	cursor_row = saved_row;
	cursor_col = saved_col;
    }
    move(cursor_row, cursor_col);
    saved = FALSE;
}

ClearLine (line_no)
int line_no;
{
    int y, x;

    getyx(stdscr, y, x);
    move(line_no, 0);
    clrtoeol();
    move(y, x);
}

PRIVATE SetupDefaultVideo (change_size)
int change_size;
{
    int i;
    char *ISstring,*TCbufptr,TCbuf[1024], *tmpptr;

    initscr();
#if (SY_B4x != 0)
    if (CM == NIL) {
	printf("TERM is undefined, or your terminal lacks the ability to run this program.");
	exit(1);
    }
#endif /* (SY_B4x != 0) */

    /* send the InistializationString to terminal */

    TCbufptr = TCbuf;
    ISstring = tgetstr("is",&TCbufptr);    
    if (ISstring != 0) {
	printf("%s",ISstring);
	debug((2,"Sent InitializationString to terminal.\n"));
    }
    /* End InitializationString */

    for (i=0; KeyPad[i].str != NIL; i++) {
	KeyPad[i].str[0] = '\0';
	TCbufptr = TCbuf;
	if ((tmpptr = tgetstr(KeyPad[i].termcap_id, &TCbufptr)) != NIL) {
	    if ((*tmpptr) && (*tmpptr == 27)) { /* starts with ESC key */
		strncpy(KeyPad[i].str, tmpptr, MAX_TC_LEN);
		KeyPad[i].str[MAX_TC_LEN+1] = '\0';
	    }
	}
    }

    
#if (SY_B4x != 0)
    nostandout = (SO == NIL);
#else /* (SY_B4x != 0) */
    nostandout = 1;
#endif /* (SY_B4x != 0) */
    /*  savetty(); initscr() already does this */
    noecho();
    raw();
    if (change_size) {
	last_row = error_row = LINES - 1;
	msg_row = last_row - 1;
	opt_row = msg_row - 1;
	bar_row = opt_row - 1;
    }
    AttrTablePtr = Termcap_AttrTable;
    for (i=0; i<128; i++) charmap[i] = i;
    for (i=128; i<256; i++) charmap[i] = '$';
    charmap[0315] = '=';
    charmap[0311] = '+';
    charmap[0272] = '|';
    charmap[0310] = '+';
    charmap[0273] = '+';
    charmap[0274] = '+';
    charmap[0265] = '|';
    charmap[0306] = '|';
}

PRIVATE DosCursor (cursor_row)
int cursor_row;
{
    move(cursor_row, 0);
}

SaveVideoEnv (vp, change_size)
VIDEOPARMS *vp;
int change_size;
{
    SetupDefaultVideo (change_size);
}

RestoreVideoEnv (vp, clearp, cursor_row)
VIDEOPARMS *vp;
int clearp;
int cursor_row;
{
    move(cursor_row, 0);
    clrtobot();
    DosCursor (cursor_row);
    refresh();
    /*  resetty(); endwin() already does this */
    endwin();
}

ShowString (data, row, col, len, attr)
unsigned char *data;
char row, col, len, attr;
{
    int shift=0;
    static unsigned char ch, blanks[] = {
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};

	debug((1, "(%d, %d)[%d|%d] '%.*s'\n", row, col, len, attr, len, data));
	if (len <= 0) return;
	if ((attr == RINVIS) || (attr == INVISI) || (data == (unsigned char *)NULL))
	    data = blanks;
	move(row, col);
	if (*(AttrTablePtr + attr)) {
	    standout();
	    shift = nostandout;
	}
	while (len--) {
	    if (col == past_col) {
		debug((2,"Going around the horn. Row=%d, col=%d, len=%d\n",row,col,len));
		col=0; row++;
	    }
	    ch = (*data)?charmap[*(data++)]:' ';
	    if (ch<' ') ch = '.';
	    if (shift && isascii(ch) && islower(ch)) ch = toupper(ch);
	    addch(ch);
	}
	standend();
	move(cursor_row, cursor_col);
}

ErrorBeep ()
{
    putchar('\07');
}

ClearScreen()
{
    clear();
}

FIELD *RedrawScreen (curfield)	/* mas V1.3 */
FIELD *curfield;
{
    wrefresh(curscr);
    return (curfield);
}

ShowError(msg)
PRMPT *msg;
{
    ShowMsg(msg);
    ErrorBeep ();
}

UpdateScreen()
{
    refresh();
}
