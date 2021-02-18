/***********************************************************************

gdev.c - Window system independent graphics package

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

***********************************************************************/

#include "gdrv.h"

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>


/*
 * gdev.c - kernel for device independent graphics interface
 * 
 */

#ifndef TRUE
#	define TRUE 	1
#	define FALSE	0
#endif

#define	STDIN_FD	0	/* file desc of stdin */


int gdevdebug = 0;

static char *mouse_event();
static char *appendf();
static char *chr();
static char *chrs();

#define VERSION		1

static	FILE *recordfile = NULL;
static	FILE *playfile = NULL;
static	int fileversion = VERSION;


/*
 * Dispatch table to device specific routines
 * Devices currently supported:
 *	poof
 *	wm
 *	X
 */

static int GDEVnull() { }	/* Used for functions not supplied */

Pointer	POOFinit();
int	POOFstart(), POOFflush(), POOFstop(),
	POOFline(), POOFpoint(), POOFrect(), POOFtext(),
	POOFsetdim(), POOFgetdim();

Pointer	PSinit();
int	PSstart(), PSflush(), PSstop(),
	PSline(), PSpoint(), PSrect(), PStext(),
	PSsetdim(), PSgetdim();

Pointer	DPinit();
int	DPstart(), DPflush(), DPstop(),
	DPline(), DPpoint(), DPrect(), DPtext(),
	DPsetdim(), DPgetdim();

Pointer	WMinit();
int	WMstart(), WMflush(), WMstop(),
	WMline(), WMpoint(), WMrect(), WMtext(),
	WMsetdim(), WMgetdim(),
	WMmenuitem(), WMmouseinterest();

Pointer	XDEVinit();
int	XDEVstart(), XDEVflush(), XDEVstop(),
	XDEVline(), XDEVpoint(), XDEVrect(), XDEVtext(),
	XDEVsetdim(), XDEVgetdim(),
	XDEVmenuitem(), XDEVmouseinterest();

Pointer	X11init();
int	X11start(), X11flush(), X11stop(),
	X11line(), X11point(), X11rect(), X11text(),
	X11setdim(), X11getdim(),
	X11menuitem(), X11mouseinterest();

typedef struct alist *Alist, ListElem;
typedef	int (*Function)();

struct dev {
	char *name;
	Pointer (*init)();
	int (*start)(), (*flush)(), (*stop)();
	int (*line)(), (*point)(), (*rect)(), (*text)();
	int (*setdim)(), (*getdim)();
	int (*menuitem)(), (*mouseinterest)(); 
	/*--------------------*/
	/* per driver runtime information (not initialized) */
	Alist	variable_functions;  /* Assoc variable (string) numbers with
					functons to call */
	Alist 	variable_pointers;  /* Assoc variable with address */
	int	drv_thinks_y_at_top; /* true iff driver thinks 0,0 is
						top left of window */
} dev[] = {

#ifdef X11
	{ "X11",
		X11init, X11start, X11flush, X11stop,
		X11line, X11point, X11rect, X11text,
		X11setdim, X11getdim,
		X11menuitem, X11mouseinterest,
	},
#endif

#ifdef PS
	{ "ps",
		PSinit, PSstart, PSflush, PSstop,
		PSline, PSpoint, PSrect, PStext,
		PSsetdim, PSgetdim, 
		GDEVnull, GDEVnull,
	},
#endif

#ifdef POOF
	{ "poof",
		POOFinit, POOFstart, POOFflush, POOFstop,
		POOFline, POOFpoint, POOFrect, POOFtext,
		POOFsetdim, POOFgetdim, 
		GDEVnull, GDEVnull,
	},
#endif

#ifdef DP
	{ "dp",
		DPinit, DPstart, DPflush, DPstop,
		DPline, DPpoint, DPrect, DPtext,
		DPsetdim, DPgetdim, 
		GDEVnull, GDEVnull,
	},
#endif

#ifdef WM
	{ "wm",
		WMinit, WMstart, WMflush, WMstop, 
		WMline, WMpoint, WMrect, WMtext,
		WMsetdim, WMgetdim,
		WMmenuitem, WMmouseinterest,
	},
#endif

#ifdef XDEV
	{ "X",
		XDEVinit, XDEVstart, XDEVflush, XDEVstop,
		XDEVline, XDEVpoint, XDEVrect, XDEVtext,
		XDEVsetdim, XDEVgetdim,
		XDEVmenuitem, XDEVmouseinterest,
	},
#endif

	{ NULL }
};


/*
   we allow simultaneous open windows of possibly DIFFERENT device types
   There is a current window, to which all other calls apply.
   There is also a current window for input.
   Whenever input comes from a window different than the current input window,
   a "input window changed" string is prepended, changing the current input
   window.
*/


/* Settings global to all windows */

static	Alist	fd_functions; /* Assoc fds with fd functions */
static	char 	*windowchanged_retval;

#define	 NWINDOWS	10	/* max simultaneous open windows */


static struct window {
	struct	dev	*dev;		/* device type */
	Pointer		handle;
	int		app_wants_y_at_top; /* true iff application wants 0,0 at
						top left of window */
	int maxy;

	Alist	menu_items;	/* Assoc items with GDRVmenu returns */
	Alist	menu_retvals;	/* Assoc GDRVmenu returns with retvals */
	int	menu_uid;	/* uid to gen GDRVmenu returns */
	Alist	mouse_retvals;	/* Assoc mouse events with retvals */
	char	*refresh_retval;/* retval for refresh */
	int	(*refresh_function)();	/* called when refresh required */

} window[NWINDOWS];

static int nwindows = 0;

static struct window *CurOut = NULL;	/* current window */


#define	INVERT_Y(y)		(CurOut->maxy - (y))
#define INVERTING_Y		(CurOut->app_wants_y_at_top != \
				 CurOut->dev->drv_thinks_y_at_top)
#define TRANSFORM_Y(y)		(INVERTING_Y ? INVERT_Y(y) : (y))

static int inhardcopy;
static struct window *hardcopyout;
static int hardcopychar = 'z'&037;

static void cbufputc();

/*
 * Create a new window, making it the current window for output
 */

int
GDEVinit(device)
char *device;
{
	register i;
	char *getenv(), *p;
	struct dev *d;


	/* if the device name begins with an asterisk, it is not overriden by
	   the GDEV variable, otherwise it is */

	printf("GDEV: %s\n", device ? device : "NULL");

	if(device && device[0] == '*')
		device++;
	else if(((p = getenv("GDEV")) != NULL) && *p != '\0') {
		device = p;
	}

	if(device == NULL)
		d = &dev[0];
	else {
		d = NULL;
		for(i = 0; dev[i].name; i++)
			if(strcmp(device, dev[i].name) == 0)
				d = &dev[i];
		if(d == NULL) {
			printf("Don't know about device %s\n", device);
			return -1;
		}
	}

	if(CurOut == NULL) {	/* first time called */
		cbufinit();
		{ int ReadStdin();  GDRVfdnotify(STDIN_FD, ReadStdin); }
		hardcopyinit();
	}

	if(nwindows >= NWINDOWS) {
		printf("Too many open windows, increase NWINDOWS\n");
		return -1;
	}
	CurOut = &window[nwindows++];

	CurOut->dev = d;
	CurOut->handle = (*d->init)(CurOut - window);
	CurOut->app_wants_y_at_top = FALSE;

	return CurOut - window;
}

static struct window *
Gwindow(w, s)
int w;
char *s;
{
	if(w < 0 ||  w >= nwindows) {
		printf("%s: no such window %d\n", s, w);
		exit(1);
	}
	return &window[w];
}

void
GDEVcurrentoutput(w)
int w;
{
	if(! inhardcopy)
		CurOut = Gwindow(w, "GDEVcurrentoutput");
}

void
GDEVstart()
{
	(*CurOut->dev->start)(CurOut->handle);
	GDEVsetmaxy();
}

void
GDEVflush()
{
	if(recordfile != NULL)
		fflush(recordfile);
	(*CurOut->dev->flush)(CurOut->handle);
	if(inhardcopy)
		hardcopydone();
}

void
GDEVstop()
{
	if(recordfile != NULL)
		fflush(recordfile);
	(*CurOut->dev->stop)(CurOut->handle);
}

void
GDEVline(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	if(INVERTING_Y)
		(*CurOut->dev->line)(CurOut->handle, x1, INVERT_Y(y1),
						     x2, INVERT_Y(y2));
	else
		(*CurOut->dev->line)(CurOut->handle, x1, y1, x2, y2);
}

void
GDEVpoint(x, y)
int x, y;
{
	(*CurOut->dev->point)(CurOut->handle, x, TRANSFORM_Y(y));
}

void
GDEVrect(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	if(INVERTING_Y)
		(*CurOut->dev->rect)(CurOut->handle,
				x1, INVERT_Y(y1), x2, INVERT_Y(y2));
	else
		(*CurOut->dev->rect)(CurOut->handle,
				x1, y1, x2, y2);
}

void
GDEVtext(x1, y1, str)
int x1, y1;
char *str;
{
	(*CurOut->dev->text)(CurOut->handle, x1, TRANSFORM_Y(y1), str);
}

void
GDEVsetdim(x, y)
int x, y;
{
	(*CurOut->dev->setdim)(CurOut->handle, x, y);
	GDEVsetmaxy();
}

void
GDEVgetdim(x, y)
int *x, *y;
{
	(*CurOut->dev->getdim)(CurOut->handle, x, y);
}

/*
 * The functions so far have basically just dealt with output and pretty
 * much just pass their arguments along to the appropriate driver functions.
 * When input is added things get a bit more hairy.
 */

/*-------------------------- input buffer package --------------------------*/

/*
  We use a circular buffer of annotated characters to keep track of input
  that is to be delivered to the application
 */

#define	CBUFSIZE	5120

/* classes of charaters */

typedef	unsigned char Cclass;

#define	C_RETURNED	0x01	/* character has already been returned to app*/
#define	C_EOF		0x02	/* character is EOF */
#define	C_DOUBLE	0x04	/* char is part of double precision # */
#define	C_INT		0x08	/* char is part of integer # */

#define	C_TYPED		0x10	/* User typed characters */
#define	C_MOUSE		0x20	/* Mouse event characters */
#define	C_MENU		0x40	/* Menu event characters */
#define	C_REFRESH	0x80	/* Refresh event characters */

#define	C_ANY		(C_TYPED | C_MOUSE | C_MENU | C_REFRESH | C_EOF)

typedef struct {
	char	c;
	Cclass	cclass;
} Cbuf;

static	Cbuf	cbuf[CBUFSIZE];		/* Circular buffer of characters */
static	Cbuf	*Chead;			/* Points to first character  */
static	Cbuf	*Ctail;			/* Points to where to add chars   */

/* The circular queue is empty if Chead == Ctail */

#define	C_NOCHARSPENDING	(257)	/*  return when no chars available */

static
cbufinit()
{
	Chead = Ctail = cbuf;
}

cbufwindow(w)
int w;
{
	static int lastwin = -2;

	if(w != lastwin) {
		lastwin = w;
		if(windowchanged_retval) {
			cbufputs(windowchanged_retval, C_TYPED); /* C_TYPED?? */
			cbufputc(w, C_TYPED);
		}
	}
}


#define	nextcp(cp)	(((cp) == &cbuf[CBUFSIZE-1]) ? ((cp) = cbuf) : ++(cp))

static int
cbufempty()
{
	for(; Chead->cclass == C_RETURNED && Chead != Ctail; nextcp(Chead)) 
		/* skip */ ;

	return Chead == Ctail;
}

static int
cbufgetc(classes)
Cclass classes;
{
	register Cbuf *cp;
	register int r;

	/* skip over prevously returned chars */
	for(; Chead->cclass == C_RETURNED && Chead != Ctail; nextcp(Chead)) 
		/* skip */ ;

	if(playfile != NULL) {
		static void cbufputc();
		register int c;
		Cclass class;
		do {
			if((c = getc(playfile)) == EOF ||
			   (class = getc(playfile)) == ((Cclass) EOF)) {
				fclose(playfile);
				playfile = NULL;
				break;
			}
			cbufputc(c, class);
		} while( ! ( class & classes ) );
	}

	/* search for chars satisying the given class set */
	r = C_NOCHARSPENDING;
	for(cp = Chead; cp != Ctail; nextcp(cp))
		if(cp->cclass & classes) {
			r = cp->c;
			if(cp->cclass == C_EOF)
				r = EOF;
			cp->cclass = C_RETURNED;
			break;
		}
	if(gdevdebug)
		printf("<%s ", chr(r)), fflush(stdout);

	return r;

}

static void
cbufputc(c, cclass)
int c;
Cclass cclass;
{
	if(gdevdebug)
		printf(">%s ", chr(c)), fflush(stdout);
	if(c == hardcopychar && cclass == C_TYPED && hardcopyout) {
		hardcopy();
		return;
	}
	if(recordfile != NULL)
		putc(c, recordfile), putc(cclass, recordfile);

	/* skip over prevously returned chars */
	for(; Chead->cclass == C_RETURNED && Chead != Ctail; nextcp(Chead)) 
		/* skip */ ;

	/* add character to end */
	Ctail->c = c;
	Ctail->cclass = cclass;
	nextcp(Ctail);
	if(Ctail == Chead)
		GDEVerror("GDEV input char overflow - increase CBUFSIZE");

}

static
cbufputi(i)
int i;
{
	register char *p = (char *) &i;
	register int n = sizeof(i);

	while(--n >= 0)
		cbufputc(*p++, C_INT);
}

static int
cbufgeti()
{
	int i = 0;
	register char *p = (char *) &i;
	register int n = sizeof(i);
	int c;

	while(--n >= 0) {
		if((c = cbufgetc(C_INT)) == C_NOCHARSPENDING) {
			printf("Error getting integer off cbuf!\n");
			break;
		}
		*p++ = c;
	}
	return i;
}


static
cbufputd(d)
double d;
{
	register char *p = (char *) &d;
	register int i = sizeof(d);

	while(--i >= 0)
		cbufputc(*p++, C_DOUBLE);
}

static double
cbufgetd()
{
	double d = 0;
	register char *p = (char *) &d;
	register int i = sizeof(d);
	int c;

	while(--i >= 0) {
		if((c = cbufgetc(C_DOUBLE)) == C_NOCHARSPENDING) {
			printf("Error getting double off cbuf!\n");
			break;
		}
		*p++ = c;
	}
	return d;
}

static int
cbufputs(s, cclass)
char *s;
Cclass cclass;
{
	if(s) while(*s)
		cbufputc(*s++, cclass);
}


/*--------- Some utility functions --------------------------------*/

char *malloc();

static char *
scopy(s)
char *s;
{
	register char *p = malloc((unsigned) strlen(s) + 1);
	if(p == NULL)
		GDEVerror("scopy");
	(void) strcpy(p, s);
	return p;
}

/*--------- Simple linked association list -------------------------*/

struct alist {
	int		tag;
	Alist		next;
	union {
		char 		*string;
		char 		**stringp;
		int		*intp;
		double		*doublep;
		Pointer		pointer;
		Function	function;
	} data1;
	Pointer		data2;
};

#if 1
Alist __alist;
#define	LookupInt(alist, tag, enterflag) \
	( __alist = _lookupint(&alist, tag, enterflag), \
	gdevdebug > 1 ? \
	  printf("_lookupint(%x, %d, %d)=%x\n", &alist, tag, enterflag, __alist) :0, \
	__alist )

#define	LookupString(alist, s, enterflag) \
	( __alist = _lookupstring(&alist, s, enterflag), \
	gdevdebug > 1 ? \
	  printf("_lookupstring(%x, %s, %d)=%x\n", &alist, chrs(s), enterflag, __alist) :0, \
	__alist )

#else
#define	LookupInt(alist, tag, enterflag) _lookupint(&alist, tag, enterflag)
#define	LookupString(alist, s, enterflag) _lookupstring(&alist, s, enterflag)
#endif

static Alist _makenew();

static Alist
_lookupint(alistp, tag, enterflag)
register Alist *alistp;
int tag;
int enterflag;	/* if TRUE, make new entry if tag not found */
{
	register Alist l;

	for(l = *alistp; l != NULL; l = l->next)
		if(l->tag == tag)
			return l;

	return _makenew(alistp, enterflag);
}

static Alist
_lookupstring(alistp, s, enterflag)
register Alist *alistp;
char *s;
int enterflag;	/* if TRUE, make new entry if tag not found */
{
	register Alist l;

	for(l = *alistp; l != NULL; l = l->next)
		if( ! strcmp(l->data1.string, s) )
			return l;

	return _makenew(alistp, enterflag);
}

#ifdef lint
	Alist	alistmalloc();
#else
#	define  alistmalloc()	((Alist) malloc(sizeof(struct alist)))
#endif

static Alist
_makenew(alistp, enterflag)
int enterflag;
Alist *alistp;
{
	register Alist newl;

	if( ! enterflag)
		return NULL;
	
	if((newl = alistmalloc()) == NULL)
		GDEVerror("Lookup malloc(%d)", sizeof(*newl));

	newl->next = *alistp;
	newl->tag = 0;
	newl->data1.string = NULL;
	*alistp = newl;
	return newl;
}

/*--------- String to Int and vice verse -----------------------------------*/

static	Alist string_numbers;	      /* Assoc strings with numbers */

static int
StringToInt(s)
char *s;
{
	static int uid = 0;

	register Alist a = LookupString(string_numbers, s, TRUE);
	if(a->data1.string == NULL)
		a->data1.string = scopy(s), a->tag = ++uid;
	return a->tag;
}

/* uncomment this out if anyone ever has to use this function
static char *
IntToString(i)
int i;
{
	register Alist a = LookupInt(string_numbers, i, FALSE);
	if(a == NULL)
		return NULL;
	return a->data1.string;
}
*/
	

/*--------- Input to the application  ---------------------------------------*/

static
ReadStdin()
{
	char buf[512];

	register int i, n;
	n = read(STDIN_FD, buf, 512);
	if(n < 0)
		perror("ReadStdin");
	else if(n == 0) {
		cbufputc(0, C_EOF);
		close(STDIN_FD);
		if(open("/dev/tty", 0) != STDIN_FD)
			perror("/dev/tty");
	}
	else for(i = 0; i < n; i++)
		cbufputc(buf[i], C_TYPED);
}


/* Call polling function, if any, to try to get input */

static int
GDEVpoll()
{
	register Alist a;
	for(a = fd_functions; a != NULL; a = a->next)
		if(a->tag == FD_POLL && a->data1.function) {
			(*a->data1.function)();
			return TRUE;
		}
	return FALSE;
}

/* call select, and dispatch appropriately to handle input */

#ifndef FD_SET

/* for systems with old version of select */

#ifndef	FD_SETSIZE
#define	FD_SETSIZE	256
#endif

#ifndef NBBY
#define NBBY	8
#endif

#define	fd_set	int_fd_set	/* seem to be conflict in some includes */

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */
#ifndef howmany
#define	howmany(x, y)	(((x)+((y)-1))/(y))
#endif

typedef	struct fd_set {
	fd_mask	fds_bits[howmany(FD_SETSIZE, NFDBITS)];
} fd_set;

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))

#endif


#define	FD_SET_NULL	((fd_set *) 0)			/* Ah, lint */
#define	TIMEVAL_NULL	((struct timeval *) 0)

static	struct timeval	*timeout_interval;
static	char 		*timeout_retval;

char *bits();

static
GDEVselect(waitflag)
int waitflag;
{
	register Alist a;
	int i, n;
	fd_set rfds;
	int maxfd;
	static struct timeval zero;

	FD_ZERO(&rfds);
	maxfd = 0;
	for(a = fd_functions; a != NULL; a = a->next) {
		if(a->tag != FD_POLL)
			FD_SET(a->tag, &rfds);
		if(a->tag > maxfd)
			maxfd = a->tag;
	}

	if(gdevdebug) printf("select(rfds=%s)", bits(rfds) ), fflush(stdout);

	n = select(maxfd + 1, &rfds, FD_SET_NULL, FD_SET_NULL,
			waitflag ? timeout_interval : &zero);

	if(gdevdebug) printf("=%d (rfds=%s)\n", n, bits(rfds));

	if(n == 0) {
		if(timeout_retval && waitflag)
			cbufputs(timeout_retval, C_REFRESH);
			/* change C_REFRESH to something better */
		return;
	}

	if(n < 0) {
		printf("select returned %d\n", n), perror("select");
		return;
	}

	for(i = 0; i <= maxfd; i++)
		if(FD_ISSET(i, &rfds)) {
			a = LookupInt(fd_functions, i, FALSE);
			if(a && a->data1.function)
				(* a->data1.function)();
				
		}
}

static char *
bits(fds)
fd_set fds;
{
	static char buf[300];
	register int i;
	sprintf(buf, "{");
	for(i = 0; i < FD_SETSIZE; i++) {
		if(FD_ISSET(i, &fds))
			sprintf(buf+strlen(buf), "%d ", i);
	}
	i = strlen(buf) - 1;
	sprintf(buf + i + (buf[i] == '{'), "}");
	return buf;
}

void
GDRVfdnotify(fd, function)
int fd;
Function function;
{
	register Alist a = LookupInt(fd_functions, fd, TRUE);
	a->tag = fd;
	a->data1.function = function;
}

static int
GDEVgetc(cclass)
Cclass cclass;
{
	register int c;

	if((c = cbufgetc(cclass)) != C_NOCHARSPENDING)
		return c;

again:
	/* No chars available, try polling */
	if(GDEVpoll())
		if((c = cbufgetc(cclass)) != C_NOCHARSPENDING)
			return c;

	/* No chars available, we better do a select */
	GDEVselect(TRUE);

	if((c = cbufgetc(cclass)) != C_NOCHARSPENDING)
		return c;

	if(cclass == C_ANY)
		if(gdevdebug) printf("GDEVgetchar select ok but no chars\n");
	goto again;
}

int
GDEVgetchar()
{
	return GDEVgetc(C_ANY);
}

int
GDEVcharswaiting()
{
	if( ! cbufempty() )
		return TRUE;
	if(GDEVpoll() && ! cbufempty() )
		return TRUE;
	GDEVselect(FALSE);
	if( ! cbufempty() )
		return TRUE;
	return FALSE;
}

char *
GDEVgets(line)
char *line;
{
	register int c;
	register char *p = line;

	while((c = GDEVgetc(C_TYPED)) != '\n' && c != EOF)
		*p++ = c;
	*p = '\0';
	if(c == EOF && p == line)
		return NULL;
	return line;
}

#define	MAGIC	0xf0	/* magic header byte */

int
GDEVplay(file)
char *file;
{
	register int c;

	if(playfile != NULL)
		fclose(playfile);
	if((playfile = fopen(file, "r")) == NULL) {
		printf("Couldn't open %s\n", file);
		perror(file);
		return FALSE;
	}
	c = getc(playfile);
	if( (c&0xff) == MAGIC ) {
		fileversion = getc(playfile);
	}
	else  {
		fileversion = 0;
		ungetc(c, playfile);
	}
	return TRUE;
}

int
GDEVrecord(file)
char *file;
{
	if((recordfile = fopen(file, "w")) == NULL) {
		printf("Couldn't create %s", file);
		return FALSE;
	}
	fputc(MAGIC, recordfile);
	fputc(VERSION, recordfile);
	return TRUE;
}

void
GDEVstoprecording()
{
	if(recordfile != NULL)
		fclose(recordfile);
}

/* menus */

void
GDEVmenuitem(label, retval)
char *label, *retval;
{
	register Alist item, rv;
	int found = TRUE;

	item = LookupString(CurOut->menu_items, label, TRUE);
	if(item->data1.string == NULL) {	/* item not found */
		item->data1.string = scopy(label);
		item->tag = ++CurOut->menu_uid;
		found = FALSE;
	}

	rv = LookupInt(CurOut->menu_retvals, item->tag, TRUE);
	if(rv->data1.string != NULL)	/* retval for item exists already */
		free(rv->data1.string);
	else
		found = FALSE;

	rv->tag =  item->tag;

	if(retval) {
		rv->data1.string = scopy(retval);
		(*CurOut->dev->menuitem)(CurOut->handle,
			item->data1.string, item->tag, TRUE);
	} else {
		rv->data1.string = NULL;
		if(found)
			(*CurOut->dev->menuitem)(CurOut->handle,
				item->data1.string, item->tag, FALSE);
	}
}

void
GDRVmenu(w, retval)
int w;
int retval;
{
	register Alist rv;
	register struct window *gwin = Gwindow(w, "GDRVmenu");

	if(gdevdebug)
		printf("GDRVmenu([win %d] %d)\n", w, retval);
	rv = LookupInt(gwin->menu_retvals, retval, FALSE);
	if(rv == NULL)	/* retval not valid */
		GDEVerror("GDRVmenu: illegal retval (%d)", retval);
	cbufwindow(w);
	cbufputs(rv->data1.string, C_MENU);
}


/* mouse */

void
GDEVmouse(event, retval)
int event;
char *retval;
{
	register Alist rv;

	rv = LookupInt(CurOut->mouse_retvals, event, TRUE);
	if(rv->data1.string != NULL)	/* retval for item exists already */
		free(rv->data1.string);

	rv->tag =  event;
	if(retval != NULL) {
		rv->data1.string = scopy(retval);
		(*CurOut->dev->mouseinterest)(CurOut->handle, event);
	} else {
		rv->data1.string = NULL;
	}
}

void
GDRVmouse(w, event, x, y, thetime)
int w;
int event;
int x, y;
int thetime;
{
	register Alist rv;
	register struct window *gwin = Gwindow(w, "GDRVmouse");

	if(gdevdebug)
		printf("GDRVmouse([win %d] %s, %d, %d[%d])\n", 
			w, mouse_event(event), x, y, TRANSFORM_Y(y));
	rv = LookupInt(gwin->mouse_retvals, event, FALSE);
	if(rv != NULL && rv->data1.string != NULL) {
		cbufwindow(w);
		cbufputs(rv->data1.string, C_MOUSE);
		cbufputi(x);
		cbufputi(TRANSFORM_Y(y));
		cbufputi(thetime);
	}
}

void
GDEVgetXYT(xp, yp, timep)
int *xp, *yp;
int *timep; 
{
	int x, y;
	int thetime = 0;

	x = cbufgeti();
	y = cbufgeti();
	if(fileversion != 0)
		thetime = cbufgeti();
	if(xp) *xp = x;
	if(yp) *yp = y;
	if(timep) *timep = thetime;
}

/* window changed */

void
GDEVwindowchanged(retval)
char *retval;
{
	if(windowchanged_retval)
		free(windowchanged_retval);
	windowchanged_retval = scopy(retval);
}

/* refresh */

void
GDEVrefresh(retval)
char *retval;
{
	if(CurOut->refresh_retval)
		free(CurOut->refresh_retval);
	CurOut->refresh_retval = scopy(retval);
}

void
GDEVrefresh_function(fcn)
int (*fcn)();
{
	CurOut->refresh_function = fcn;
}

void
GDRVrefresh(w)
int w;
{
	register struct window *gwin = Gwindow(w, "GDRVrefresh");

	/* If we wanted to we could eliminate duplicate refreshes */
	if(gdevdebug)
		printf("GDRVrefresh [win %d]\n", w);
	cbufwindow(w);
	cbufputs(gwin->refresh_retval, C_REFRESH);
	if(gwin->refresh_function)
		(*gwin->refresh_function)();
}

void
GDEVtimeout(msec, retval)
int msec;
char *retval; 
{
	static struct timeval timer;
	timeout_retval = retval;
	if(retval == NULL)
		timeout_interval = NULL;
	else {
		timeout_interval = &timer;
		timer.tv_sec = msec / 1000;
		timer.tv_usec = (msec % 1000L) * 1000;
	}
}

/*----------------- variables ------------------------------*/

#define GDEVset(elem) \
	register int v = StringToInt(variable);  			\
	register Alist a; 						\
	a = LookupInt(CurOut->dev->variable_functions, v, FALSE);  	\
	if(a && a->data1.function)					\
	  return (* a->data1.function)(CurOut->handle, variable,	\
						     value, a->data2);	\
	a = LookupInt(CurOut->dev->variable_pointers, v, FALSE);  	\
	if(a && a->data1.elem)						\
		(* a->data1.elem) = value; 				\
	return 0;

int
GDEVseti(variable, value)
char *variable; int value;
{
	GDEVset(intp);
}

int
GDEVsetd(variable, value)
char *variable; double value;
{
	GDEVset(doublep);
}

int
GDEVsets(variable, value)
char *variable, *value;
{
	GDEVset(stringp);
}

void
GDRVvar_fcn(w, varname, function, arg)
char *varname;
Function function;
Pointer arg;
{
	int i = StringToInt(varname);
	register struct window *gwin = Gwindow(w, "GDRVvar_fcn");

	register Alist a = LookupInt(gwin->dev->variable_functions, i, TRUE); 
	a->tag = i;
	a->data1.function = function;
	a->data2 = arg;
}

void
GDRVvar_addr(w, varname, address)
int w;
char *varname;
Pointer address;
{
	int i = StringToInt(varname);
	register struct window *gwin = Gwindow(w, "GDRVvar_fcn");
	register Alist a = LookupInt(gwin->dev->variable_pointers, i, TRUE); 

	a->tag = i;
	a->data1.pointer = address;
}

/*VARARGS1*/
GDEVerror(a, b, c, d, e, f)
char *a;
{
	printf("GDEV: ");
	printf(a, b, c, d, e, f);
	printf("\n");
	exit(1);
}

static
char *
mouse_event(event)
{
	static char buf[200];
	buf[0] = '\0';
	if(event&LEFT_BUTTON) appendf(buf, "LEFT_BUTTON |");
	if(event&MIDDLE_BUTTON) appendf(buf, "MIDDLE_BUTTON |");
	if(event&RIGHT_BUTTON) appendf(buf, "RIGHT_BUTTON |");
	if(event&DOWN_TRANSITION) appendf(buf," DOWN_TRANSITION");
	if(event&UP_TRANSITION) appendf(buf," UP_TRANSITION");
	if(event&DOWN_MOVEMENT) appendf(buf," DOWN_MOVEMENT");
	appendf(buf, "(%x)", event);
	return buf;
}

/*VARARGS2*/
static char *
appendf(s, f, a, b, c, d, e)
char *s, *f;
{
	sprintf(strlen(s) + s, f, a, b, c, d, e);
	return 0;
}

static char *
chr(r)
int r;
{
	static char buf[10];
	if(!isascii(r) || r==0177)
		sprintf(buf, "\\%d", r);
	else if(isprint(r))
		sprintf(buf, "%c", r);
	else 
		sprintf(buf, "^%c", r + 'A' - 1);
	return buf;
}

static char *
chrs(s)
char *s;
{
	static char buf[100];
	buf[0] = '\0';
	while(*s) {
		appendf(buf, "%s", chr(*s++));
	}
	return buf;
}

/* handle y=0 top/bottom sillyness */

static
GDEVsetmaxy()
{
	int x;

	GDEVgetdim(&x, &CurOut->maxy);
}

void
GDEV00topleft()
{
	CurOut->app_wants_y_at_top = TRUE;
}

void
GDRV00topleft(w)
{
	Gwindow(w, "GDRV00topleft")->dev->drv_thinks_y_at_top = TRUE;
}


GDRVputc(c)
{
	cbufputc(c, C_TYPED);
}

GDRVwindow(w)
int w;
{
	cbufwindow(w);
}

#include <sys/time.h>
int
GDEVcurrenttime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

#include <signal.h>

static struct window *savedwindow;

hardcopy()
{
	if(!hardcopyout || !CurOut) {
		printf("hardcopy() ignored\n");
		return;
	}
	printf("hardcopy() begin\n");
	savedwindow = CurOut;
	CurOut = hardcopyout;
	CurOut->app_wants_y_at_top = savedwindow->app_wants_y_at_top;
	inhardcopy = TRUE;
	if(savedwindow->refresh_function) {
		(*savedwindow->refresh_function)();
		hardcopydone();
	}
	else {
		GDRVrefresh(savedwindow - window);
	}
}

hardcopydone()
{
	if(inhardcopy) {
		inhardcopy = FALSE;
		printf("hardcopy() done\n");
		GDEVstop();
		CurOut = savedwindow;
	}
}

hardcopyinit()
{
	static called = 0;
	register char *p;
	char d[100];

	if(called) return;	/* avoid infinite recursion */
	called++;

	if(((p = getenv("HARDCOPYGDEV")) != NULL) && *p != '\0') {
		printf("HARDCOPYGDEV: %s, send signal SIGUSR1=%d\n",
			p, SIGUSR1);
		sprintf(d, "*%s", p);
		savedwindow = CurOut;	/* always NULL, I think */
		GDEVinit(d);
		hardcopyout = CurOut;
		printf("hardcopyout->dev->name: %s\n", hardcopyout->dev->name);
		CurOut = savedwindow;
		signal(SIGUSR1, hardcopy);
		inhardcopy = FALSE;
	}
}

