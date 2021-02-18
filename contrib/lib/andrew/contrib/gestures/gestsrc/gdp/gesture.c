/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include <gestures/args.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
/* #include "region.h" */
#include <gestures/util.h>
#include <gestures/bool.h>
#include <gestures/bitvector.h>
#include <gestures/matrix.h>
#include <gestures/sc.h>
#include <gestures/fv.h>
#include "gdp.h"
#include <gestures/gdev.h>
#include <gestures/zdebug.h>


char _zdebug_flag[128];

sClassifier fullclassifier;		/* full recognizer */
sClassifier doneclassifier;	/* ambigous/unambiguous classifier */

extern	int	gdevdebug;

int	argc;
char 	**argv;

double	rho = 1.0;

char *
fetcharg(c)
{
	int i;
	char *r;

	for(i = 1; i < argc; i++) {
		if(strchr(argv[i], c)) {
			r = argv[i];
			--argc;
			for( ; i < argc; i++)
				argv[i] = argv[i+1];
			return r;
		}
	}
	return NULL;
}

int timeout = 300;
int eagerness = 0;

GESTUREinit(ac, av)
int ac;
char **av;
{
	char *r;
	FILE *f;

	argc = ac;
	argv = av;

	if((r = fetcharg('=')) != NULL)
		GDEVsets("Xgeometry", r);

	if((r = fetcharg('#')) != NULL) {
		register char *p = index(r, '#');
		if(p != NULL) {
			*p++ = '\0';
			GDEVsets(r, p);
			printf("set %s=%s\n", r, p);
		}
	}


	ARGLOOP
		FLAGARG('G')	gdevdebug++;		ENDFLAGARG
		STRINGARG('p')	rho = atof(p);		ENDSTRINGARG
		STRINGARG('t')	timeout = atoi(p);	ENDSTRINGARG
		STRINGARG('e')	eagerness = atoi(p);	ENDSTRINGARG
		STRINGARG('Z')	while(*p) _zdebug_flag[*p++]++; ENDSTRINGARG
		STRINGARG('z')	while(*p) _zdebug_flag[*p++]--; ENDSTRINGARG
			
		BADARG
			error("Usage: gdp [ -Z flags ] [ -G ] [ -P ] [ -c ]  [ -p rho ] [ -t timeout-in-msec ] [ -e eagerness ] full-classifier-file [stop-classifier-file]");
		ENDBADARG
	ENDARGLOOP

	switch(argc) {
	case 2:
		if((f = fopen(argv[1], "r")) == NULL)
			error("Can't open %s for reading", argv[1]);
		doneclassifier = sRead(f);
		fclose(f);
		/* fall into ... */
	case 1:
		if((f = fopen(argv[0], "r")) == NULL)
			error("Can't open %s for reading", argv[0]);
		fullclassifier = sRead(f);
		fclose(f);
		break;
	default:
		error("Usage: testclass description-file [stop-desc-file]");
	}

	init3();
}

static FV fv;

init3()
{
	fv = FvAlloc(0);
}


#define	TIMEOUT_CHAR	'T'
char	timeout_string[] = { TIMEOUT_CHAR, '\0' };

#define	G_NORMAL_CHAR		0
#define	G_MOUSE_STILL_DOWN	1
#define	G_MOUSE_UP		2
#define	G_ERROR			3

#define ERROR(s) return ((printf s),G_ERROR)

static	int	Gx, Gy, Gt;	/* first point of gesture */
static	int	Lx, Ly, Lt = 0;	/* last point of gesture */

int
read_gesture(chr)
int *chr;
{

	int button;
	int x, y;
	int c;
	int t;

#define is_escape(c)	((c)=='q' || c=='n' || c==EOF)
	/* State machine for input */

newgesture:

	Sreset();
	GDEVtimeout(0, NULL);
	FvInit(fv);
	c = GDEVgetchar();
	if(c == MOUSE_CHAR) goto mouse_down;
	*chr = c;
	return G_NORMAL_CHAR;

mouse_down:
	button = GDEVgetchar();
	GDEVgetXYT(&x, &y, &t);
	Gx = x; Gy = y; Gt = t;
	if(button != LEFT_DOWN) ERROR(("mouse_down %o\n", button));
	FvAddPoint(fv, x, y, (long) t, 0);
	Gpoint(x, y);
	Lx = x, Ly = y;
	goto set_time_then_charwait;

charwait_then_set_time:
	GDEVtimeout(0, NULL);
	c = GDEVgetchar();
	GDEVtimeout(timeout, timeout_string);
	goto dispatch;
	
set_time_then_charwait:
	GDEVtimeout(timeout, timeout_string);
	goto charwait;

charwait:
	c = GDEVgetchar();
	goto dispatch;

dispatch:
	if(is_escape(c)) goto escape;
	if(c == MOUSE_CHAR) goto mouse;
	if(c == TIMEOUT_CHAR) goto timeout;
	printf("mouse_down: c=%o %c\n", c, c);
	/* shouldn't really get here */

mouse:
	button = GDEVgetchar();
	GDEVgetXYT(&x, &y, &t);
	if(button == LEFT_MOVE) goto newpoint;
	if(button == LEFT_UP) goto lastpoint;
	printf("mouse: button=%o\n", button);
	goto dispatch;

newpoint:
	FvAddPoint(fv, x, y, (long) t, 0);
	Gpoint(x, y);
	Lx = x; Ly = y;
	GDEVflush();
	if(Unambiguous(fv))
		goto classifynow;
	goto charwait;

lastpoint:
	FvAddPoint(fv, x, y, (long) t, 0);
	Gpoint(x, y);
	GDEVflush();
	Lx = x; Ly = y;
	GDEVtimeout(0, NULL);
	*chr = ClassifyFv();
	Serase();
	return G_MOUSE_UP;

timeout:
	Xat(Lx, Ly, 10);
	goto classifynow;

classifynow:
	GDEVflush();
	GDEVtimeout(0, NULL);
	*chr = ClassifyFv();
	Serase();
	return G_MOUSE_STILL_DOWN;
	/* goto charwait_then_set_time; */

escape:
	return;
}

int
ClassifyFv()
{
	Vector v;

	v = FvCalc(fv);
	return ClassifyVector(v);
}

int
ClassifyVector(y)
Vector y;
{
	register sClassDope cd;

	cd = sClassify(fullclassifier, y);
	if(cd == NULL) {
		printf(" rejected\n", cd->name);
		return 0;
	} else {
		printf(" %s\n", cd->name);
		return cd->name[0];
	}
}

Xat(x, y, eps)
int x, y, eps;
{
	Gline(x-eps, y-eps, x+eps, y+eps);
	Gline(x-eps, y+eps, x+eps, y-eps);
}


static	int	gesture_character = 0xff;

void
GESTUREcharacter(c)
int c;
{
	gesture_character = c;
}

#define NPEEK	20
static int peek[NPEEK];
static int npeek = 0;

Gpop()
{
	register int i, r;

	if(npeek > 0) {
		r = peek[0];
		npeek--;
		for(i = 0; i < npeek; i++)
			peek[i] = peek[i+1];
		return r;
	}
	else
		error("Gpop");
}

int
GESTUREgetchar()
{
	int r, chr;

	if(npeek > 0)
		return Gpop();

	switch( r = read_gesture(&chr) ) {
	case G_ERROR:
		return GESTUREgetchar();

	case G_NORMAL_CHAR:
		return chr;

	case G_MOUSE_STILL_DOWN:
	case G_MOUSE_UP:
		if(chr == 0) {
			printf("gesture rejected\n");
			return GESTUREgetchar();
		}

		peek[0] = chr;
		peek[1] = MOUSE_CHAR;
		peek[2] = r==G_MOUSE_UP ? LEFT_UP : LEFT_MOVE;
		npeek = 3;
		Gfake();
		return gesture_character;

	default:
		error("read_gesture!!!");
	}
}


GESTUREgetXYT(xp, yp, tp)
int *xp, *yp, *tp;
{
	if(xp) *xp = Gx;
	if(yp) *yp = Gy;
	if(tp) *tp = Gt;
}


/* return the next input character or mouse event */
/* Usually calls GDEVgetchar, but there is a one mouse-event buffer
   which allows gestures that end with mouse button up rather than timeouts
   to always be followed by a mouse up event, making it easier on the
   callers of these routines */

static int Gfaking;

Ggetchar()
{
	if(npeek > 0)
		return Gpop();
	return GDEVgetchar();
}

void
GgetXYT(xp, yp, tp)
int *xp, *yp, *tp;
{
	if(Gfaking) {
		Gfaking = 0;
		if(xp) *xp = Lx;
		if(yp) *yp = Ly;
		if(tp) *xp = Lt;
	}
	else
		GDEVgetXYT(xp, yp, tp);
}

Gfake()
{
	/* if(Gfaking) error("Gfaking"); */
	Gfaking = 1;
}

/*----------------------------------------------------------------------*/

Unambiguous(fv)
FV fv;
{
	Vector y;
	register sClassDope cd;
	register int retval = 0;
	static int count = 0;

	if(doneclassifier == NULL)
		return FALSE;

	y = FvCalc(fv);
	cd = sClassify(doneclassifier, y);
	if(cd == NULL) {
		printf("r"); fflush(stdout);
	}
	else {
		printf("%s ", cd->name); fflush(stdout);
		if(cd->name[0] == 'c') {
			if(++count > eagerness)
				retval = 1;
		}
		else {
			count = 0;
		}
	} 
	return retval;
}
