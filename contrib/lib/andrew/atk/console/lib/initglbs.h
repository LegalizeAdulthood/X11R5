/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/initglbs.h,v 2.11 1991/09/12 19:28:17 bobg Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/initglbs.h,v $ */

#ifndef lint
static char *rcsidinitGlobalid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/console/lib/RCS/initglbs.h,v 2.11 1991/09/12 19:28:17 bobg Exp $";
#endif /* lint */

/* 
 * global initialization needed for dynamically loaded console
 */

#include <console.h> 
/* Below is taken from original console.c -- ghoti */

ConLibType conlib[MAXCONFILES];
char *libpaths[MAXLIBS];

long libnum = 0;
long connum = 0;
long BasicLib = -1;
long LocalLib = -1;
long UserLib = -1;

char ConFile[1024] = "";
int MYDEBUGGING = FALSE;
struct fontdesc *FontsAvail[MAXNUMFONTS];
char *AvailFontNames[MAXNUMFONTS] = { "" };
int AvailFontPts[MAXNUMFONTS];
int FontCount = 0;
int ExternalsInUse = 0;
FILE *VenusIn, *ErrorsIn, *ConsoleIn;
char ErrTxt[256] = "";
char *Nullity = "";
char *StatusServer = NULL; /* for vopcon server machine */
int ScaleFactor = 100;
int VMPollCt = 1,
    DiskPollCt = 1,
    MailPollCt = 1,
    PrintPollCt = 1,
    DirPollCt = 1,
    WindowPollCt = 1,
    NetPollCt = 1,
    VenusPollCt = 1,
    ClockPollCt = 1,
    FPAPollCt = 1;
boolean ErrorInputPending = FALSE;
boolean ConsoleSocketInputPending = FALSE;
boolean VenusInputPending = FALSE;
boolean WindowInputPending = FALSE;
int LastMailMod= 0;
boolean DidInitGVM = FALSE,
    DidInitVenus = FALSE,
    DidInitPrinting = FALSE,
    DidInitNet = FALSE,
    DidInitErrLog = FALSE;
char *envmail = NULL;
char FontFamily[20] = "andysans";
struct RegionLog RegionLogs[NUMREGIONLOGS + 1] = {NULL};
struct InternalVariable IntrnlVars[NUMINTERNALVARIABLES + 1];
struct datum Numbers[DisplayTypeCount + 1];
struct display *VeryFirstDisplay;
struct fontdesc *PromptFont;
struct fontdesc *EventFont;
char PromptFontName[50] = "andysans10";

extern int PreLogError(),
    PreLogReport(),
    PreLogUser(),
    PreLogSilly();


extern int DrawDebug(),
    DrawDebug(),
    DrawEKGGraph(),
    DrawGauge(),
    DrawDial(),
    DrawIndicator(),
    DrawBarGraph(),
    RingAlarm(),
    SignalTrouble(),
    LogError(),
    LogReport(),
    LogUser(),
    LogSilly(),
    DrawLog(),
    DrawTitle(),
    DrawNothing();

int (*(prefunctions[]))() = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    PreLogError,
    PreLogReport,
    PreLogUser,
    PreLogSilly,
    NULL,
    NULL,
    NULL
};



int (*(functions[]))() = {
    DrawDebug,
    DrawDebug,
    DrawEKGGraph,
    DrawGauge,
    DrawDial,
    DrawIndicator,
    DrawBarGraph,
    RingAlarm,
    SignalTrouble,
    LogError,
    LogReport,
    LogUser,
    LogSilly,
    DrawLog,
    DrawTitle,
    DrawNothing
};

int DefaultErrorPriority = 4,
    HighestPriority = 4,
    Period = 1,
    MinHeight = 500,
    MaxHeight = 500,
    MinWidth = 500,
    MaxWidth = 500,
    VMPollFreq = 2,
    DiskPollFreq = 60,
    MailPollFreq = 60,
    PrintPollFreq = 60,
    DirPollFreq = 60,
    WindowPollFreq = 60,
    NetPollFreq = 60,
    VenusPollFreq = 30,
    ClockPollFreq = 2,
    FPAPollFreq = 3600;
boolean DoTroubleChecking = FALSE,
    DoVenusChecking = FALSE,
    DoVMStat = FALSE,
    DoDiskFreeStat = FALSE,
    DoVenusMonitoring = FALSE,
    DoMailChecking = FALSE,
    DoPrintChecking = FALSE,
    DoDirChecking = FALSE,
    DoCheckClock = FALSE,
    DoWindows = FALSE,
    DoLOAD = FALSE,
    DoCPU = FALSE,
    DoVM = FALSE,
    DoPAGING = FALSE,
    DoMEM = FALSE,
    DoQUEUE = FALSE,
    DoINTS = FALSE,
    DoNDSTAT = FALSE,
    DoNetChecking = FALSE,
    DoPROCESSES = FALSE,
    PauseEnqueuedEvents = FALSE,
    FPAchecked = FALSE,
    RingingAlarm = FALSE;

/* added to handle expanded path names  --MKM */
char *xPaths[MAXCONSOLEPATHS] = { (char *) NULL };
char *xNames[MAXCONSOLEPATHS] = { (char *) NULL };
char *ConLib[MAXCONSOLES] = { NULL };
int ConNum = 0;
int IntVarCount[NUMINTERNALVARIABLES + 1];

