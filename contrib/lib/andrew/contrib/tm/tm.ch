/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm.ch,v 2.7 1991/09/12 20:17:24 bobg Exp $ */
/* $ACIS:tm.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidtermulator_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/contrib/tm/RCS/tm.ch,v 2.7 1991/09/12 20:17:24 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*
 * terminal emulator for text
 */

/* sunOS is the only place I know of that the "correct" way actually works */
#ifndef sunV3
#define INCORRECTSIGNALS
#endif /* sunV3 */

#define HACKEDNOECHO

struct cmd {
    struct cmd *next,*prev;
    char *string;
};

enum termulator_Mode {termulator_ModeCooked, termulator_ModeCbreak, termulator_ModeRaw};

#define INTRCHAR '\003'
#define QUITCHAR '\034'
#define STOPCHAR '\032'

class termulator[tm]: text {
    overrides:
        Clear();
        ViewName() returns char *;
	AlwaysInsertCharacters(long pos,char *buf,long len);
	ReadSubString(long pos,FILE *file,boolean quoteCharacters) returns long;
	Read(FILE *file, long id) returns long;
	Write(FILE *file, long writeID, int level) returns long;
    methods:
        StartProcess(char **args) returns int;
	GrabPrevCmd(char *str) returns char *;
	GrabNextCmd(char *str) returns char *;
	EnterCmd(char *string) returns boolean;
	RewindCmds();
	ProcessInput(char *text, long len);
	SendInput(char *text, long len);
	Submit();
	Signal(int signo);
	SetScreenSize(width,height);
	SetDispSize(width,height);
	ProcessOutput(char *str, long len);
	GetTermcap() returns char *;
	GetTerm() returns char *;
	Untermulate();
	/* the following are intended to be called from subclasses */
	GotoXY(int x,int y);
	WriteChars(char *chars, int numChars);
	DeleteChars(int numChars);
	ClearChars(int x,int numChars);
	InsertLines(int numLines);
	DeleteLines(int numLines);
	ClearLines(int y,int numLines);
	Scroll(int num);
	Newline(int num);
	Backspace(int num);
	CR();
	Tab();
	EOT();
    classprocedures:
	InitializeObject(struct termulator *self) returns boolean;
        FinalizeObject(struct termulator *self);
    macromethods:
	GetX() (self->x)
	GetY() (self->y)
	GetWidth() (self->width)
        GetHeight() (self->height)
	SetInsert(boolean b) (self->insert=(b))
	GetInsert() (self->insert)
	SetStandout(boolean b) (self->standout=(b))
        GetStandout() (self->standout)
	SetEscape(char c, char *(*action)()) (self->escapes[c]=(action))
        GetEscape(char c) (self->escapes[c])
	SetParseState(int (*rtn)()) (self->state=(rtn))
        GetParseState() (self->state)
	GetParseBuf() (self->parseBuf)
	GetParseBufLen() (self->parseLen)
	SetParseBufLen(int l) (self->parseLen=(l))
        GetParseBufMax() (sizeof(self->parseBuf))
    data:
        int screen,cursor,lastSubmit;
	int width,height;
	int dispWidth,dispHeight;
#ifdef HACKEDNOECHO
	char inpBuf[300];
	int inpLen;
#endif /* HACKEDNOECHO */
	struct tmview *inpView; /* manipulated by views */
	char parseBuf[100];
	int parseLen;
	char *cwd;
        int x,y;
	int pty;
	FILE *ptyFile;
	int pid;
	struct cmd *lastCmd, *curCmd;
	char **args;
	boolean insert,standout;
	char *errstr;
	int errno;
	struct style *cmdStyle,*noechoStyle,*soStyle,*screenStyle;
	struct environment *cmdEnv,*screenEnv;
	int mode;
	boolean remote;
	char *(*escapes[256])();
	char *(*state)();
	boolean currentlyReadingTemplateKluge;
};
