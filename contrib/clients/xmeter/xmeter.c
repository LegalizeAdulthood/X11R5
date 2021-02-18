/*
 * xmeter.c - Display histogram of rstat(3) output for multiple hosts.
 *
 * Copyright (c) 1991, Bob Schwartzkopf
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the RAND Corporation not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The RAND Corporation
 * makes no representations about the suitability of this software for
 * any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author: Bob Schwartzkopf, The RAND Corporation
 *
 * Suggestions for improvements and bug fixes can be sent to "bobs@rand.org".
 * As my schedule permits I'll try to incorporate them.
 */

#ifndef lint
static char	*RCSid="$Header: /usr/rand/src/bin/xmeter/RCS/xmeter.c,v 1.8 1991/09/16 21:01:04 bobs Exp $";
#endif lint

/*
 * $Log: xmeter.c,v $
 * Revision 1.8  1991/09/16  21:01:04  bobs
 * Fix bugs handling down hosts.
 * Add composite stats swap, disks, page and pkts.
 * Add popup window with version number, scale and downtime (if applicable).
 *
 * Revision 1.7  1991/06/03  22:21:43  root
 * Add -v option to print xmeter version number.
 * Add defstat option.
 * Add code to clear stripchart when watched stat is changed.
 * When host is down fork process to wait for it to come up instead of
 * having toplevel process wait for timeouts.
 *
 * Revision 1.6  90/09/28  20:32:34  root
 * Add explicit closes of sockets back in, apparently some versions of
 * clnt_destroy() don't do it automatically.
 * 
 * Revision 1.5  90/09/18  20:47:03  root
 * Allow user specification of foreground colors.
 * 
 * Revision 1.4  90/08/20  14:18:01  root
 * Added menus, column and row options, background bitmaps, and user
 * specifiable programs to be invoked when graphs change state.
 * 
 * Revision 1.3  90/06/07  16:17:06  bobs
 * Removed "retries".
 * Changed name of paned widgets to host name displayed in that widget.
 * Used actual time between rstat calls instead of "update" interval in
 * computing rates in functions that return values to stripchart widgets.
 * Removed "ost" variable, rewrote functions that return values to
 * stripcharts.
 * Fixed bug in fsys, fcpu and fuser that could cause divide by 0 errors.
 * Fixed rpc timeout handling in getmeter and getport.
 * Use RSTATVERS_TIME instead of RSTATVERS, which isn't defined in SunOS 4.1.
 * 
 * Revision 1.2  90/05/04  12:31:52  bobs
 * Fix memory leak in getport().  Wasn't freeing resources allocated by
 * clntudp_create when clnt_call failed.  Also removed explicit calls
 * to close sockets since clnt_destroy() does this.
 * 
 * Revision 1.1  90/04/30  14:33:59  bobs
 * Initial revision
 * 
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/wait.h>
#ifndef sgi
#include <sys/dk.h>
#endif sgi
#include <signal.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpcsvc/rstat.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/StripChart.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xmu/Xmu.h>
#include "patchlevel.h"

#define MAJORVERSION	1
#define DMSG		"down"
#define OK		0
#define WARN		1
#define ERROR		2
#define FATAL		3
#define MAXBACKS	(FATAL+1)
#define STATMENU	"statmenu"
#ifndef FSCALE
#define FSCALE		(1 << 8)
#endif

/*
 * There is one METER structure per strip chart.  If multiple stats
 * on a single host are being watched we save rstatd calls by sharing
 * part of this structure (SHMETER) between the different meters watching
 * the same host.
 */
typedef struct _shmeter {
    char		*name;	/* Host name				*/
    struct sockaddr_in	addr;
    CLIENT		*clnt;
    int			s;	/* Socket				*/
    struct statstime	st[2];	/* Keep last 2 stats to compute diffs	*/
    int			idx;	/* Index into st array			*/
    int			refcnt;	/* Meters sharing this structure	*/
    int			curcnt;	/* Meters who've displayed current data	*/
    int			first;	/* TRUE when only 1 rstat has been done	*/
    int			pid;	/* Pid of proc waiting for host		*/
    time_t		when;	/* Time we first notice host down	*/
    struct _shmeter	*nxt;	/* Link these together			*/
} SHMETER;

typedef struct _meter {
    char		*label;
    int			stat;		/* Which stat to watch		*/
    Widget		pdwidget;	/* Paned widget			*/
    Widget		mbwidget;	/* MenuButton widget		*/
    Widget		scwidget;	/* StripChart widget		*/
    int			oldstate;	/* Save state so know when to	*/
					/* change backgrounds		*/
    SHMETER		*sh;		/* Shared info			*/
    Pixmap		pm;		/* Current background pixmap	*/
    int			oldjumpscroll;	/* Old jumpscroll value		*/
    struct _meter	*nxt;
} METER;

/*
 * There is a STATDATA structure for each statistic that can be monitored.
 */
typedef struct {
    int		scale;		/* Each stat is scaled			*/
    int		(*val)();	/* Function that computes this stat	*/
    char	*name;		/* Name of stat for menu widget		*/
} STATDATA;

/*
 * xmeter supports 4 background colors and pixmaps that it will switch
 * between as a meter changes state, called OK, WARN, ERROR and FATAL.
 * The background bitmaps are stored in BITMAP structures.
 */
typedef struct {
  int		w;				/* Width of bitmap	*/
  int		h;				/* Height of bitmap	*/
  Pixmap	bm;				/* Bitmap		*/
} BITMAP;

/*
 * Functions
 */
extern char	*malloc ();
extern char	*index ();
int		getstatus ();
int		changestat ();
int		selecthost ();
int		freechild ();
METER		*initmeter ();
int		fcoll (), fcpu (), fierr (), fintr (), fipkt (), fload ();
int		foerr (), fopkt (), fpage (), fpgpgin (), fpgpgout ();
int		fpswpin (), fpswpout (), fswt (), fsys (), fuser ();
int		fpkts (), fdisks (), fswap (), ferr ();
void		popupscale ();
void		popdownscale ();

STATDATA		sd[] = {
#define S_COLL		0
  {1, fcoll, "coll"},
#define S_CPU		S_COLL+1
  {1, fcpu, "cpu"},
#define S_DISKS		S_CPU+1
  {1, fdisks, "disks"},
#define S_ERR		S_DISKS+1
  {1, ferr, "err"},
#define S_IERR		S_ERR+1
  {1, fierr, "ierr"},
#define S_INTR		S_IERR+1
  {1, fintr, "intr"},
#define S_IPKT		S_INTR+1
  {1, fipkt, "ipkt"},
#define S_LOAD		S_IPKT+1
  {1, fload, "load"},
#define S_OERR		S_LOAD+1
  {1, foerr, "oerr"},
#define S_OPKT		S_OERR+1
  {1, fopkt, "opkt"},
#define S_PAGE		S_OPKT+1
  {1, fpage, "page"},
#define S_PGPGIN	S_PAGE+1
  {1, fpgpgin, "pgpgin"},
#define S_PGPGOUT	S_PGPGIN+1
  {1, fpgpgout, "pgpgout"},
#define S_PKTS		S_PGPGOUT+1
  {1, fpkts, "pkts"},
#define S_PSWPIN	S_PKTS+1
  {1, fpswpin, "pswpin"},
#define S_PSWPOUT	S_PSWPIN+1
  {1, fpswpout, "pswpout"},
#define S_SWAP		S_PSWPOUT+1
  {1, fswap, "swap"},
#define S_SWT		S_SWAP+1
  {1, fswt, "swt"},
#define S_SYS		S_SWT+1
  {1, fsys, "sys"},
#define S_USER		S_SYS+1
  {1, fuser, "user"},
};
#define DEFSTATNAME	"load"
#define MAXSTAT		(sizeof (sd) / sizeof (STATDATA))
#define MAXSTATNAME	10		/* Max length of stat name	*/

static char		*progname;
Pixel			back[MAXBACKS];		/* Meter backgrounds	*/
Pixel			fore[MAXBACKS];		/* Meter foregrounds	*/
Pixel			hl[MAXBACKS];		/* Meter highlights	*/
Pixel			bd[MAXBACKS];		/* Meter borders	*/
Pixel			ibd[MAXBACKS];		/* Meter internal bds	*/
Pixel			lback[MAXBACKS];	/* Label backgrounds	*/
Pixel			lfore[MAXBACKS];	/* Label foregrounds	*/
BITMAP			bitmap[MAXBACKS];	/* Stripchart bg pixmaps*/
Pixmap			bitmapdef = XtUnspecifiedPixmap;
int			warnLevel;		/* Warning level	*/
int			errorLevel;		/* Error level		*/
static struct timeval	ptto = {0, 0};		/* RPC timeout interval	*/
static struct timeval	tto = {0, 0};		/* Total timeout	*/
static SHMETER		*shmeters = NULL;	/* List of SHMETERs	*/
static METER		*selected;		/* Current meter	*/
static METER		*meterlist = NULL;	/* List of METERs	*/
int			columns;		/* Columns to display	*/
int			rows;			/* Rows	to display	*/
int			timeout;		/* RPC timeout		*/
int			retries;		/* RPC retries		*/
int			loadscaledef = FSCALE;	/* Scale for load ave.	*/
Boolean			shortname;		/* Short hostname flag	*/
String			prog[MAXBACKS];		/* Alert programs	*/
String			defstat;		/* Default stat		*/
Widget			popup;		/* Used by pop[up|down]scale	*/
static XtResource	application_resources[] = {
    {"columns", "Columns", XtRInt, sizeof (int),
	(Cardinal) &columns, XtRString, "0"},
    {"defStat", "DefStat", XtRString, sizeof (char *),
	(Cardinal) &defstat, XtRString, DEFSTATNAME},
    {"errorBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &back[ERROR], XtRString, XtDefaultBackground},
    {"errorBd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &bd[ERROR], XtRString, XtDefaultForeground},
    {"errorBitmap", XtCBitmap, XtRBitmap, sizeof (Pixmap),
	(Cardinal) &bitmap[ERROR].bm, XtRBitmap, (caddr_t) &bitmapdef},
    {"errorFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &fore[ERROR], XtRString, XtDefaultForeground},
    {"errorHl", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &hl[ERROR], XtRString, XtDefaultForeground},
    {"errorIbd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &ibd[ERROR], XtRString, XtDefaultForeground},
    {"errorLevel", "ErrorLevel", XtRInt, sizeof (int),
	(Cardinal) &errorLevel, XtRString, "6"},
    {"errorProg", "Program", XtRString, sizeof (char *),
	(Cardinal) &prog[ERROR], XtRString, NULL},
    {"fatalBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &back[FATAL], XtRString, XtDefaultBackground},
    {"fatalBd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &bd[FATAL], XtRString, XtDefaultForeground},
    {"fatalBitmap", XtCBitmap, XtRBitmap, sizeof (Pixmap),
	(Cardinal) &bitmap[FATAL].bm, XtRBitmap, (caddr_t) &bitmapdef},
    {"fatalFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &fore[FATAL], XtRString, XtDefaultForeground},
    {"fatalHl", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &hl[FATAL], XtRString, XtDefaultForeground},
    {"fatalIbd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &ibd[FATAL], XtRString, XtDefaultForeground},
    {"fatalProg", "Program", XtRString, sizeof (char *),
	(Cardinal) &prog[FATAL], XtRString, NULL},
    {"lErrorBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lback[ERROR], XtRString, XtDefaultBackground},
    {"lErrorFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lfore[ERROR], XtRString, XtDefaultForeground},
    {"lFatalBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lback[FATAL], XtRString, XtDefaultBackground},
    {"lFatalFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lfore[FATAL], XtRString, XtDefaultForeground},
    {"lOkBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lback[OK], XtRString, XtDefaultBackground},
    {"lOkFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lfore[OK], XtRString, XtDefaultForeground},
    {"lWarnBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lback[WARN], XtRString, XtDefaultBackground},
    {"lWarnFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &lfore[WARN], XtRString, XtDefaultForeground},
    {"okBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &back[OK], XtRString, XtDefaultBackground},
    {"okBd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &bd[OK], XtRString, XtDefaultForeground},
    {"okBitmap", XtCBitmap, XtRBitmap, sizeof (Pixmap),
	(Cardinal) &bitmap[OK].bm, XtRBitmap, (caddr_t) &bitmapdef},
    {"okFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &fore[OK], XtRString, XtDefaultForeground},
    {"okHl", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &hl[OK], XtRString, XtDefaultForeground},
    {"okIbd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &ibd[OK], XtRString, XtDefaultForeground},
    {"okProg", "Program", XtRString, sizeof (char *),
	(Cardinal) &prog[OK], XtRString, NULL},
    {"retries", "Retries", XtRInt, sizeof (int),
	(Cardinal) &retries, XtRString, "2"},
    {"rows", "Rows", XtRInt, sizeof (int),
	(Cardinal) &rows, XtRString, "0"},
    {"scaleColl", "ScaleColl", XtRInt, sizeof (int),
	(Cardinal) &sd[S_COLL].scale, XtRString, "1"},
    {"scaleCpu", "ScaleCpu", XtRInt, sizeof (int),
	(Cardinal) &sd[S_CPU].scale, XtRString, "20"},
    {"scaleDisks", "ScaleDisks", XtRInt, sizeof (int),
	(Cardinal) &sd[S_DISKS].scale, XtRString, "40"},
    {"scaleErr", "ScaleErr", XtRInt, sizeof (int),
	(Cardinal) &sd[S_ERR].scale, XtRString, "1"},
    {"scaleIerr", "ScaleIerr", XtRInt, sizeof (int),
	(Cardinal) &sd[S_IERR].scale, XtRString, "1"},
    {"scaleIntr", "ScaleIntr", XtRInt, sizeof (int),
	(Cardinal) &sd[S_INTR].scale, XtRString, "50"},
    {"scaleIpkt", "ScaleIpkt", XtRInt, sizeof (int),
	(Cardinal) &sd[S_IPKT].scale, XtRString, "20"},
    {"scaleLoad", "ScaleLoad", XtRInt, sizeof (int),
	(Cardinal) &sd[S_LOAD].scale, XtRInt, (caddr_t) &loadscaledef},
    {"scaleOerr", "ScaleOerr", XtRInt, sizeof (int),
	(Cardinal) &sd[S_OERR].scale, XtRString, "1"},
    {"scaleOpkt", "ScaleOpkt", XtRInt, sizeof (int),
	(Cardinal) &sd[S_OPKT].scale, XtRString, "20"},
    {"scalePage", "ScalePage", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PAGE].scale, XtRString, "10"},
    {"scalePgpgin", "ScalePgpgin", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PGPGIN].scale, XtRString, "10"},
    {"scalePgpgout", "ScalePgpgout", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PGPGOUT].scale, XtRString, "10"},
    {"scalePkts", "ScalePkts", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PKTS].scale, XtRString, "40"},
    {"scalePswpin", "ScalePswpin", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PSWPIN].scale, XtRString, "5"},
    {"scalePswpout", "ScalePswpout", XtRInt, sizeof (int),
	(Cardinal) &sd[S_PSWPOUT].scale, XtRString, "5"},
    {"scaleSwap", "ScaleSwap", XtRInt, sizeof (int),
	(Cardinal) &sd[S_SWAP].scale, XtRString, "5"},
    {"scaleSwt", "ScaleSwt", XtRInt, sizeof (int),
	(Cardinal) &sd[S_SWT].scale, XtRString, "30"},
    {"scaleSys", "ScaleSys", XtRInt, sizeof (int),
	(Cardinal) &sd[S_SYS].scale, XtRString, "20"},
    {"scaleUser", "ScaleUser", XtRInt, sizeof (int),
	(Cardinal) &sd[S_USER].scale, XtRString, "20"},
    {"shortName", "Shortname", XtRBoolean, sizeof (Boolean),
	(Cardinal) &shortname, XtRString, "False"},
    {"timeout", "Timeout", XtRInt, sizeof (int),
	(Cardinal) &timeout, XtRString, "5"},
    {"warnBack", XtCBackground, XtRPixel, sizeof (Pixel),
	(Cardinal) &back[WARN], XtRString, XtDefaultBackground},
    {"warnBd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &bd[WARN], XtRString, XtDefaultForeground},
    {"warnBitmap", XtCBitmap, XtRBitmap, sizeof (Pixmap),
	(Cardinal) &bitmap[WARN].bm, XtRBitmap, (caddr_t) &bitmapdef},
    {"warnFore", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &fore[WARN], XtRString, XtDefaultForeground},
    {"warnHl", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &hl[WARN], XtRString, XtDefaultForeground},
    {"warnIbd", XtCForeground, XtRPixel, sizeof (Pixel),
	(Cardinal) &ibd[WARN], XtRString, XtDefaultForeground},
    {"warnLevel", "WarnLevel", XtRInt, sizeof (int),
	(Cardinal) &warnLevel, XtRString, "3"},
    {"warnProg", "Program", XtRString, sizeof (char *),
	(Cardinal) &prog[WARN], XtRString, NULL},
};
static XrmOptionDescRec	options[] = {
    {"-cols",		"columns",		XrmoptionSepArg,	NULL},
    {"-ebd",		"errorBd",		XrmoptionSepArg,	NULL},
    {"-ebg",		"errorBack",		XrmoptionSepArg,	NULL},
    {"-efg",		"errorFore",		XrmoptionSepArg,	NULL},
    {"-ehl",		"errorHl",		XrmoptionSepArg,	NULL},
    {"-eibd",		"errorIbd",		XrmoptionSepArg,	NULL},
    {"-elevel",		"errorLevel",		XrmoptionSepArg,	NULL},
    {"-ep",		"errorProg",		XrmoptionSepArg,	NULL},
    {"-fbd",		"fatalBd",		XrmoptionSepArg,	NULL},
    {"-fbg",		"fatalBack",		XrmoptionSepArg,	NULL},
    {"-ffg",		"fatalFore",		XrmoptionSepArg,	NULL},
    {"-fhl",		"fatalHl",		XrmoptionSepArg,	NULL},
    {"-fibd",		"fatalIbd",		XrmoptionSepArg,	NULL},
    {"-fp",		"fatalProg",		XrmoptionSepArg,	NULL},
    {"-h",		"*Paned.height",	XrmoptionSepArg,	NULL},
    {"-lebg",		"lErrorBack",		XrmoptionSepArg,	NULL},
    {"-lefg",		"lErrorFore",		XrmoptionSepArg,	NULL},
    {"-lfbg",		"lFatalBack",		XrmoptionSepArg,	NULL},
    {"-lffg",		"lFatalFore",		XrmoptionSepArg,	NULL},
    {"-lobg",		"lOkBack",		XrmoptionSepArg,	NULL},
    {"-lofg",		"lOkFore",		XrmoptionSepArg,	NULL},
    {"-lwbg",		"lWarnBack",		XrmoptionSepArg,	NULL},
    {"-lwfg",		"lWarnFore",		XrmoptionSepArg,	NULL},
    {"-obd",		"okBd",			XrmoptionSepArg,	NULL},
    {"-obg",		"okBack",		XrmoptionSepArg,	NULL},
    {"-ofg",		"okFore",		XrmoptionSepArg,	NULL},
    {"-ohl",		"okHl",			XrmoptionSepArg,	NULL},
    {"-oibd",		"okIbd",		XrmoptionSepArg,	NULL},
    {"-op",		"okProg",		XrmoptionSepArg,	NULL},
    {"-rows",		"rows",			XrmoptionSepArg,	NULL},
    {"-rt",		"retries",		XrmoptionSepArg,	NULL},
    {"-scoll",		"scaleColl",		XrmoptionSepArg,	NULL},
    {"-scpu",		"scaleCpu",		XrmoptionSepArg,	NULL},
    {"-sdisks",		"scaleDisks",		XrmoptionSepArg,	NULL},
    {"-serr",		"scaleErr",		XrmoptionSepArg,	NULL},
    {"-sierr",		"scaleIerr",		XrmoptionSepArg,	NULL},
    {"-sintr",		"scaleIntr",		XrmoptionSepArg,	NULL},
    {"-sipkt",		"scaleIpkt",		XrmoptionSepArg,	NULL},
    {"-sload",		"scaleLoad",		XrmoptionSepArg,	NULL},
    {"-sn",		"shortName",		XrmoptionNoArg,		"on"},
    {"-soerr",		"scaleOerr",		XrmoptionSepArg,	NULL},
    {"-sopkt",		"scaleOpkt",		XrmoptionSepArg,	NULL},
    {"-spage",		"scalePage",		XrmoptionSepArg,	NULL},
    {"-spgpgin",	"scalePgpgin",		XrmoptionSepArg,	NULL},
    {"-spgpgout",	"scalePgpgout",		XrmoptionSepArg,	NULL},
    {"-spkts",		"scalePkts",		XrmoptionSepArg,	NULL},
    {"-spswpin",	"scalePswpin",		XrmoptionSepArg,	NULL},
    {"-spswpout",	"scalePswpout",		XrmoptionSepArg,	NULL},
    {"-sswap",		"scaleSwap",		XrmoptionSepArg,	NULL},
    {"-sswt",		"scaleSwt",		XrmoptionSepArg,	NULL},
    {"-ssys",		"scaleSys",		XrmoptionSepArg,	NULL},
    {"-suser",		"scaleUser",		XrmoptionSepArg,	NULL},
    {"-to",		"timeout",		XrmoptionSepArg,	NULL},
    {"-update",		"*StripChart.update",	XrmoptionSepArg,	NULL},
    {"-w",		"*Paned.width",		XrmoptionSepArg,	NULL},
    {"-wbd",		"warnBd",		XrmoptionSepArg,	NULL},
    {"-wbg",		"warnBack",		XrmoptionSepArg,	NULL},
    {"-wfg",		"warnFore",		XrmoptionSepArg,	NULL},
    {"-whl",		"warnHl",		XrmoptionSepArg,	NULL},
    {"-wibd",		"warnIbd",		XrmoptionSepArg,	NULL},
    {"-wlevel",		"warnLevel",		XrmoptionSepArg,	NULL},
    {"-wp",		"warnProg",		XrmoptionSepArg,	NULL},
};
static Arg		formargs[] = {
    {XtNdefaultDistance,	1},		/* Spacing		*/
    {XtNresizable,		TRUE },		/* Resizable		*/
};

XtActionsRec scact[] = {
    {"popupscale", popupscale},
    {"popdownscale", popdownscale}
};
#define NUMSCACT	(sizeof (scact) / sizeof (XtActionsRec))

main (argc, argv)

int	argc;
char	**argv;

{
  Widget		toplevel;
  Widget		form;
  Widget		pane = NULL;
  int			i;
  int			n;
  Arg			args[10];
  XtAppContext		appcon;
  String		mbtrans =
      "<EnterWindow>:	highlight()\n\
       <LeaveWindow>:	reset()\n\
       <BtnDown>:	set() notify() PopupMenu()";
  XtTranslations	mbtt;
  String		sctrans =
      "<BtnDown>:	popupscale()\n\
       <BtnUp>:		popdownscale()";
  XtTranslations	scacttt;
  Widget		*lw;	/* Last column or row of widgets	*/
  int			numwgt;

  progname = argv[0];
  toplevel = XtAppInitialize (&appcon, "XMeter", options,
		XtNumber (options), &argc, argv, NULL, NULL, ZERO);
  XtGetApplicationResources (toplevel, NULL, application_resources,
			     XtNumber (application_resources), NULL, 0);
  form = XtCreateManagedWidget ("form", formWidgetClass, toplevel,
			        formargs, XtNumber (formargs));
  if (argc < 2)
      usage ();
  if (strcmp ("-v", argv[1]) == 0)
       printversion ();
  init (toplevel, &lw);
  createmenus (form);
  mbtt = XtParseTranslationTable (mbtrans);
  scacttt = XtParseTranslationTable (sctrans);
  XtAppAddActions (appcon, scact, NUMSCACT);
  /*
   * Each meter consists of a paned widget, each paned widget has two
   * children, which are a menubutton widget and stripchart widget.
   * The following loop creates and initializes the appropriate widgets.
   */
  for (i = 1, numwgt = 0; i < argc; i++, numwgt++) {
      meterlist = initmeter (meterlist, &i, argc, argv);
	/*
	 * Create paned widget.
	 */
      n = 0;
      if (columns > 0) {		/* Locate pane in form ...	*/
	  XtSetArg (args[n], XtNfromVert, lw[numwgt % columns]); n++;
	  if (numwgt % columns != 0) {
	      XtSetArg (args[n], XtNfromHoriz, pane); n++;
	  }
      } else {
	  XtSetArg (args[n], XtNfromHoriz, lw[numwgt % rows]); n++;
	  if (numwgt % rows != 0) {
	      XtSetArg (args[n], XtNfromVert, pane); n++;
	  }
      }
      XtSetArg (args[n], XtNborder, bd[OK]); n++;
      XtSetArg (args[n], XtNinternalBorderColor, ibd[OK]); n++;
      pane = XtCreateManagedWidget (meterlist->sh->name, panedWidgetClass,
				    form, args, n);
      lw[numwgt % (columns > 0 ? columns : rows)] = pane;
      meterlist->pdwidget = pane;
	/*
	 * Create menubutton widget.
	 */
      n = 0;
      XtSetArg (args[n], XtNshowGrip, XtEno); n++;
      XtSetArg (args[n], XtNlabel, meterlist->label); n++;
      XtSetArg (args[n], XtNbackground, lback[OK]); n++;
      XtSetArg (args[n], XtNforeground, lfore[OK]); n++;
      XtSetArg (args[n], XtNmenuName, STATMENU); n++;
      XtSetArg (args[n], XtNtranslations, mbtt); n++;
      meterlist->mbwidget = XtCreateManagedWidget ("menu",
				menuButtonWidgetClass, pane, args, n);
      XtRemoveAllCallbacks (meterlist->mbwidget, XtNcallback);
      XtAddCallback (meterlist->mbwidget, XtNcallback, selecthost, meterlist);
	/*
	 * Create stripchart widget.
	 */
      n = 0;
      XtSetArg (args[n], XtNfromVert, meterlist->mbwidget); n++;
      XtSetArg (args[n], XtNresizable, TRUE); n++;
      XtSetArg (args[n], XtNbackground, back[OK]); n++;
      XtSetArg (args[n], XtNforeground, fore[OK]); n++;
      XtSetArg (args[n], XtNhighlight, hl[OK]); n++;
      XtSetArg (args[n], XtNtranslations, scacttt); n++;
      meterlist->scwidget = XtCreateManagedWidget ("load",
				stripChartWidgetClass, pane, args, n);
      XtRemoveAllCallbacks (meterlist->scwidget, XtNgetValue);
      XtAddCallback (meterlist->scwidget, XtNgetValue, getstatus, meterlist);
  }
  free (lw);
  signal (SIGCHLD, freechild);		/* Clean up after our children	*/
  XtRealizeWidget (toplevel);
  setokbackgrounds (XtDisplay (toplevel), XtScreen (toplevel));
  XtAppMainLoop (appcon);
}

usage ()

{
  int	i;

  fprintf (stderr, "Usage: %s [toolkit options] [options] [stat] host ...\n",
	   progname);
  fprintf (stderr, "  Options are:");
  for (i = 0; i < XtNumber (options); i++) {
      if (i % 8 == 0)
          fprintf (stderr, "\n    ");
      else
	  fprintf (stderr, ", ");
      fprintf (stderr, "%s", options[i].option);
  }
  fprintf (stderr, ", -v\n  Stats to watch are:");
  for (i = 0; i < MAXSTAT; i++) {
      if (i % 8 == 0)
          fprintf (stderr, "\n    ");
      else
	  fprintf (stderr, ", ");
      fprintf (stderr, "-%s", sd[i].name);
  }
  fprintf (stderr, "\n");
  exit (1);
}

/*
 * printversion - Print version number and exit.
 */
printversion ()

{
  printf ("XMeter version %d.%d\n", MAJORVERSION, PATCHLEVEL);
  exit (0);
}

/*
 * init - Initialze various globals.
 */
init (toplevel, lw)

Widget	toplevel;
Widget	**lw;

{
  int		i;
  Window	rw;
  int		x;
  int		y;
  int		bwidth;
  int		depth;

  ptto.tv_sec = timeout;
  tto.tv_sec = timeout * retries;
  if (columns <= 0 && rows <= 0)
      columns = 1;
  if (columns > 0) {
      if ((*lw = (Widget *) calloc (columns, sizeof (Widget))) == NULL)	
	  fatal ("lw");	
  } else
      if ((*lw = (Widget *) calloc (rows, sizeof (Widget))) == NULL)
	  fatal ("lw");
  for (i = 0; i < MAXBACKS; i++)
      if (bitmap[i].bm != bitmapdef)	/* Get bitmap dimensions	*/
	  XGetGeometry (XtDisplay (toplevel), bitmap[i].bm, &rw, &x, &y,
			&bitmap[i].w, &bitmap[i].h, &bwidth, &depth);
}

/*
 * createmenus - Create menus used by each meter.  Currently there's just
 *   one which consists of the stats that can be monitored.
 */
createmenus (parent)

Widget	parent;

{
  int		i;
  int		n;
  Widget	menu;
  Widget	me;
  Arg		args[10];

  menu = XtCreatePopupShell (STATMENU, simpleMenuWidgetClass, parent,
			     NULL, ZERO);
  for (i = 0; i < MAXSTAT; i++) {
      n = 0;
      XtSetArg (args[n], XtNlabel, sd[i].name); n++;
      me = XtCreateManagedWidget (sd[i].name, smeBSBObjectClass, menu,
				  args, n);
      XtRemoveAllCallbacks (me, XtNcallback);
      XtAddCallback (me, XtNcallback, changestat, i);
  }
}

void popupscale (w, event, params, num)

Widget		w;
XEvent		*event;
String		*params;
Cardinal	num;

{
  int		n;
  Arg		args[10];
  Position	x;
  Position	y;
  static char	buf[100];
  METER		*h;
  time_t	tdown;
  struct tm	t;

  for (h = meterlist; h; h = h->nxt)
      if (h->scwidget == w)
	  break;
  if (!h) {
      fprintf (stderr, "popupscale: Bad stripchart widget\n");
      exit (1);
  }
  XtTranslateCoords (w, (Position) 0, (Position) 0, &x, &y);
  n = 0;
  XtSetArg (args[n], XtNx, x); n++;
  XtSetArg (args[n], XtNy, y); n++;
  popup = XtCreatePopupShell ("popup", transientShellWidgetClass, w,
			      args, n);
  if (h->sh->pid != -1) {
      tdown = time (0) - h->sh->when;
      sprintf (buf, "version %d.%d\nscale %d\ndown %02d:%02d:%02d",
	       MAJORVERSION, PATCHLEVEL,
	       h->stat == S_LOAD ? sd[h->stat].scale / FSCALE :
				   sd[h->stat].scale,
	       tdown / (60*60), (tdown / 60) % 60, tdown % 60);
  } else
      sprintf (buf, "version %d.%d\nscale %d",
	       MAJORVERSION, PATCHLEVEL,
	       h->stat == S_LOAD ? sd[h->stat].scale / FSCALE :
				   sd[h->stat].scale);
  n = 0;
  XtSetArg (args[n], XtNlabel, buf); n++;
  XtCreateManagedWidget ("label", labelWidgetClass, popup, args, n);
  XtPopup (popup, XtGrabNone);
}

void popdownscale (w, event, params, num)

Widget		w;
XEvent		*event;
String		*params;
Cardinal	num;

{
  XtDestroyWidget (popup);
}

/*
 * setokbackgrounds - Set initial background pixmaps.  Can't do this
 *   when the stripchart widgets are created as they have to be
 *   realized apparently before the pixmaps can be created.
 */
setokbackgrounds (d, s)

Display	*d;
Screen	*s;

{
  METER		*h;
  int		n;
  Arg		args[10];

  if (bitmap[OK].bm != bitmapdef) {
      for (h = meterlist; h; h = h->nxt) {
	  h->pm = XmuCreatePixmapFromBitmap (d, XtWindow (h->scwidget),
					     bitmap[OK].bm,
					     bitmap[OK].w, bitmap[OK].h,
					     DefaultDepthOfScreen (s),
					     fore[OK],
					     back[OK]);
	  n = 0;
	  XtSetArg (args[n], XtNbackgroundPixmap, h->pm); n++;
	  XtSetValues (h->scwidget, args, n);
      }
  }
}

/*
 * freechild - Clean up child processes.
 */
freechild ()

{
  int	status;
  int	pid;
  METER	*h;

  pid = waitpid (-1, &status, WNOHANG);
  if (pid == 0 || WIFSTOPPED (status))
      return;
  for (h = meterlist; h; h = h->nxt)
      if (h->sh->pid == pid) {	/* Start updating host again	*/
	  h->sh->pid = -1;
	  break;
      }
}

/*
 * getstatus - Get status from remote host, update meter appropriately,
 *   including changing colors, background pixmap and label if necessary.
 */
getstatus (w, h, data)

Widget	w;
METER	*h;
char	*data;

{
  int			l;
  int			n;
  int			s;
  register SHMETER	*sh;
  Arg			args[10];

  sh = h->sh;
  if (h->oldstate == FATAL && sh->pid != -1) {	/* Ignore dead hosts	*/
      *(double *) data = 0.0;
      return;
  }
  if (h->oldjumpscroll) {		/* Restore old jumpscroll value	*/
      n = 0;
      XtSetArg (args[n], XtNjumpScroll, h->oldjumpscroll); n++;
      XtSetValues (h->scwidget, args, n);
      h->oldjumpscroll = 0;
  }
  s = state (l = getmeter (h), h);
  *(double *) data = s == FATAL ? 0.0 : (double) l / sd[h->stat].scale;
  if (s != h->oldstate) {
      sh->when = time (0);
      if (bitmap[h->oldstate].bm != bitmapdef)
	  XFreePixmap (XtDisplay (w), h->pm);
      n = 0;				/* Update stripchart widget	*/
      XtSetArg (args[n], XtNbackground, back[s]); n++;
      XtSetArg (args[n], XtNforeground, fore[s]); n++;
      XtSetArg (args[n], XtNhighlight, hl[s]); n++;
      if (bitmap[s].bm != bitmapdef) {
	  h->pm = XmuCreatePixmapFromBitmap (XtDisplay (w),
					     XtWindow (w),
					     bitmap[s].bm,
					     bitmap[s].w, bitmap[s].h,
					     DefaultDepthOfScreen(XtScreen(w)),
					     fore[s],
					     back[s]);
	  XtSetArg (args[n], XtNbackgroundPixmap, h->pm); n++;
      } else if (bitmap[h->oldstate].bm != bitmapdef) {
	  XtSetArg (args[n], XtNbackgroundPixmap, XtUnspecifiedPixmap); n++;
      }
      XtSetValues (w, args, n);
      n = 0;				/* Update menubutton widget	*/
      XtSetArg (args[n], XtNbackground, lback[s]); n++;
      XtSetArg (args[n], XtNforeground, lfore[s]); n++;
      if (s == FATAL || h->oldstate == FATAL) {
	  sprintf (h->label, "%s %s", sh->name,
		   s == FATAL ? DMSG : sd[h->stat].name);
	  XtSetArg (args[n], XtNlabel, h->label); n++;
      }
      XtSetValues (h->mbwidget, args, n);
      n = 0;				/* Update paned widget		*/
      XtSetArg (args[n], XtNborder, bd[s]); n++;
      XtSetArg (args[n], XtNinternalBorderColor, ibd[s]); n++;
      XtSetValues (h->pdwidget, args, n);
      if (prog[s])
	  runprog (h, s);
      if (s == FATAL)
	  sh->pid = waitforhost (h);
  }
  h->oldstate = s;
}

/*
 * waitforhost - Fork process which will wait for host to come back up.
 */
int waitforhost (h)

METER	*h;

{
  int	pid;

  if (pid = fork ())
      return (pid);
  while (1) {
      sleep (10);
      if (getport (h) > 0)
	  exit (0);
  }
}

/*
 * runprog - Run user specified alert program.
 */
runprog (h, s)

METER	*h;
int	s;

{
  char	oldstate[4];
  char	state[4];

  sprintf (oldstate, "%d", h->oldstate);
  sprintf (state, "%d", s);
  if (vfork ())
      return;			/* Parent just returns		*/
  execlp (prog[s], prog[s], oldstate, state, h->sh->name, NULL);
  fatal (prog[s]);
}

/*
 * selecthost - Select host for next changestat().
 */
selecthost (w, h, data)

Widget	w;
METER	*h;
char	*data;

{
  selected = h;
}

/*
 * changestat - Change statistic we're looking at.  To clear the stripchart
 *   it's necessary to poke the "interval" field in the StripChartWidget
 *   structure, and then set "jumpScroll" to the width of the stripchart.
 *   jumpScroll is saved here and restored in getstatus().
 */
#include <X11/IntrinsicP.h>
#include <X11/Xaw/StripCharP.h>
changestat (w, statidx, data)

Widget	w;
int	statidx;
char	*data;

{
  int	n;
  Arg	args[10];
  int	width;

  selected->stat = statidx;
  sprintf (selected->label, "%s %s", selected->sh->name, sd[statidx].name);
  n = 0;
  XtSetArg (args[n], XtNlabel, selected->label); n++;
  XtSetValues (selected->mbwidget, args, n);
  n = 0;
  XtSetArg (args[n], XtNwidth, &width); n++;
  XtSetArg (args[n], XtNjumpScroll, &selected->oldjumpscroll); n++;
  XtGetValues (selected->scwidget, args, n);
  n = 0;
  XtSetArg (args[n], XtNjumpScroll, width); n++;
  XtSetValues (selected->scwidget, args, n);
  ((StripChartWidget) selected->scwidget)->strip_chart.interval = width;
}

/*
 * state - Return state of current stat.
 */
int state (l, h)

int	l;
METER	*h;

{
  if (l < 0)
      return (FATAL);
  else if (l < warnLevel * sd[h->stat].scale)
      return (OK);
  else if (l < errorLevel * sd[h->stat].scale)
      return (WARN);
  else
      return (ERROR);
}

/*
 * The following functions return the value of the specified stat, which
 * is normally computed by taking the difference between the current
 * value and the previous value, and dividing by the update interval in
 * order to get the current rate.
 */
#define DIF(m,fld)	(m->sh->st[m->sh->idx].fld - \
			 m->sh->st[m->sh->idx ^ 1].fld)

int fcoll (h)

METER	*h;

{
  return (DIF (h, if_collisions) / DIF (h, curtime.tv_sec));
}

int fcpu (h)

METER	*h;

{
  int	i;
  int	t;
  int	d[CPUSTATES];

  for (t = 0, i= 0; i < CPUSTATES; i++)
      t += (d[i] = DIF (h, cp_time[i]));
  return (t ? (100 * (d[CP_USER]+d[CP_NICE]+d[CP_SYS])) / t : 0);
}

int fdisks (h)

METER	*h;

{
  int	i;
  int	t;

  for (t = 0, i= 0; i < DK_NDRIVE; i++)
      t += (DIF (h, dk_xfer[i]));
  return (t / DIF (h, curtime.tv_sec));
}

int ferr (h)

METER	*h;

{
  return (fierr (h) + foerr (h));
}

int fierr (h)

METER	*h;

{
  return (DIF (h, if_ierrors) / DIF (h, curtime.tv_sec));
}

int fintr (h)

METER	*h;

{
  return (DIF (h, v_intr) / DIF (h, curtime.tv_sec));
}

int fipkt (h)

METER	*h;

{
  return (DIF (h, if_ipackets) / DIF (h, curtime.tv_sec));
}

int fload (h)

METER	*h;

{
  return (h->sh->st[h->sh->idx].avenrun[0]);
}

int foerr (h)

METER	*h;

{
  return (DIF (h, if_oerrors) / DIF (h, curtime.tv_sec));
}

int fopkt (h)

METER	*h;

{
  return (DIF (h, if_opackets) / DIF (h, curtime.tv_sec));
}

int fpage (h)

METER	*h;

{
  return ((DIF (h, v_pgpgin) + DIF (h, v_pgpgout)) / DIF (h, curtime.tv_sec));
}

int fpgpgin (h)

METER	*h;

{
  return (DIF (h, v_pgpgin) / DIF (h, curtime.tv_sec));
}

int fpkts (h)

METER	*h;

{
  return (fipkt (h) + fopkt (h));
}

int fpgpgout (h)

METER	*h;

{
  return (DIF (h, v_pgpgout) / DIF (h, curtime.tv_sec));
}

int  fswap (h)

METER	*h;

{
  return (fpswpin (h) + fpswpout (h));
}

int  fpswpin (h)

METER	*h;

{
  return (DIF (h, v_pswpin) / DIF (h, curtime.tv_sec));
}

int fpswpout (h)

METER	*h;

{
  return (DIF (h, v_pswpout) / DIF (h, curtime.tv_sec));
}

int fswt (h)

METER	*h;

{
  return (DIF (h, v_swtch) / DIF (h, curtime.tv_sec));
}

int fsys (h)

METER	*h;

{
  int	i;
  int	t;
  int	d[CPUSTATES];

  for (t = 0, i= 0; i < CPUSTATES; i++)
      t += (d[i] = DIF (h, cp_time[i]));
  return (t ? (100 * d[CP_SYS]) / t : 0);
}

int fuser (h)

METER	*h;

{
  int	i;
  int	t;
  int	d[CPUSTATES];

  for (t = 0, i= 0; i < CPUSTATES; i++)
      t += (d[i] = DIF (h, cp_time[i]));
  return (t ? (100 * (d[CP_USER] + d[CP_NICE])) / t : 0);
}

/*
 * getmeter - Executes rstat(3) call to read statistics for specified host.
 *   I do all the rpc junk myself so that I have better control over timeouts
 *   than rstat(3) gives me.  If we're watching multiple stats
 *   on the same host I only do one rstat(3) call (refcnt and curcnt are
 *   used for this).
 */
int getmeter (h)

register METER	*h;

{
  enum clnt_stat	cs;
  register SHMETER	*sh;
  int			p;

  sh = h->sh;
  if (sh->curcnt >= sh->refcnt)
      sh->curcnt = 0;
  if (!sh->curcnt++) {
      if (sh->clnt == NULL) {
          if ((p = getport (h)) <= 0)
	      return (-1);
	  sh->addr.sin_port = htons (p);
          sh->s = RPC_ANYSOCK;
          if (!(sh->clnt = clntudp_create(&sh->addr, RSTATPROG, RSTATVERS_TIME,
					  ptto, &sh->s)))
	      return (-1);
	  sh->first = 1;
          sh->idx = 0;
      } else {
	  sh->first = 0;
	  sh->idx ^= 1;
      }
      cs = clnt_call (sh->clnt, RSTATPROC_STATS, xdr_void, 0, xdr_statstime,
		      &sh->st[sh->idx], tto);
      if (cs != RPC_SUCCESS) {
          clnt_destroy (sh->clnt);
	  close (sh->s);	/* Some clnt_destroy's don't do this	*/
          sh->clnt = NULL;
          return (-1);
      }
  }
  return (sh->first ? 0 :
		      sh->clnt == NULL ? -1 : (sd[h->stat].val) (h));
}

/*
 * getport - Get port rstatd is listening on.
 */
int getport (h)

METER	*h;

{
  CLIENT		*c;
  enum clnt_stat	cs;
  static struct pmap	pm = {RSTATPROG, RSTATVERS_TIME, IPPROTO_UDP, 0};
  short 		p;
  register SHMETER	*sh;

  sh = h->sh;
  sh->s = RPC_ANYSOCK;
  sh->addr.sin_port = htons (PMAPPORT);
  if (!(c = clntudp_create (&sh->addr, PMAPPROG, PMAPVERS, ptto, &sh->s)))
      return (-1);
  cs = clnt_call (c, PMAPPROC_GETPORT, xdr_pmap, &pm, xdr_u_short, &p, tto);
  clnt_destroy (c);
  close (sh->s);		/* Some clnt_destroys don't do this	*/
  return (cs == RPC_SUCCESS ? p : -1);
}

/*
 * initmeter - Fill in METER and SHMETER structures for this stat.
 */
METER *initmeter (meterlist, idx, argc, argv)

METER	*meterlist;
int	*idx;
int	argc;
char	**argv;

{
  register METER	*h;
  register SHMETER	*sh;
  struct hostent	*he;
  int			i;
  char			*cp;

  /*
   * Create and fill in METER struct.
   */
  if (!(h = (METER *) malloc (sizeof (METER))))
      fatal ("METER");
  h->nxt = meterlist;
  cp = (argv[*idx][0] == '-') ? &argv[*idx][1] : defstat;
  for (i = 0; i < MAXSTAT; i++)
      if (strcmp (cp, sd[i].name) == 0) {
	  h->stat = i;
	  break;
      }
  if ((cp != defstat) && (++(*idx) == argc || i >= MAXSTAT))
      usage ();
  if ((he = gethostbyname (argv[*idx])) == NULL)
       fatal (argv[*idx]);
  if (shortname)
      if (cp = index (he->h_name, '.'))
          *cp = '\0';
  if (!(h->label = malloc (strlen (he->h_name) + 2 + MAXSTATNAME + 6)))
      fatal ("label");
  sprintf (h->label, "%s %s", he->h_name, sd[h->stat].name);
  h->oldstate = OK;
  h->oldjumpscroll = 0;
  /*
   * If we're already looking at this host then just point h->sh at
   * existing structure and we're done.
   */
  for (sh = shmeters; sh; sh = sh->nxt)
      if (strcmp (sh->name, he->h_name) == 0) {
	  h->sh = sh;
	  sh->refcnt++;		/* Count how many references	*/
	  return (h);
      }
  /*
   * Not looking at this host yet, create and fill in SHMETER struct.
   */
  if (!(sh = (SHMETER *) malloc (sizeof (SHMETER))))
      fatal ("SHMETER");
  sh->nxt = shmeters;	/* Save this struct in linked list	*/
  shmeters = sh;
  h->sh = sh;
  if (!(sh->name = malloc (strlen (he->h_name) + 1)))
      fatal ("name");
  strcpy (sh->name, he->h_name);
  bcopy (he->h_addr, &sh->addr.sin_addr, he->h_length);
  sh->addr.sin_family = AF_INET;
  sh->clnt = NULL;
  sh->refcnt = 1;
  sh->curcnt = 0;
  sh->pid = -1;
  return (h);
}

fatal (m)

char	*m;

{
  perror (m);
  exit (1);
}
