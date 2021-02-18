/***********************************************************************

gtest.c - simple test program for GDEV

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
/*
 test out GDEV
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "gdev.h"
#include "args.h"

extern	int	gdevdebug;
static	int	NoWait = 0;
static	char  	*Tests = NULL;

int	argc;
char 	**argv;

int input_test();
int output_test();
int menu_test();
int draw_test();

main(ac, av)
int ac;
char **av;
{
	argc = ac;
	argv = av;

	init();
	DoTest('m', menu_test);
	DoTest('o', output_test);
	DoTest('i', input_test);
	DoTest('d', draw_test);
	GDEVstop();
}

DoTest(c, test)
int c;
int (*test)();
{
	char *index();

	if(Tests == NULL || index(Tests, c)) {
		(*test)();
		GDEVflush();
		if(! NoWait)
			Wait();
	}
}

#define	MOUSE_CHAR	001

#define    LEFT_DOWN			001
#define    LEFT_MOVE		  	002
#define    LEFT_UP		  	003

#define    RIGHT_DOWN			011
#define    RIGHT_MOVE		  	012
#define    RIGHT_UP		  	013

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

init()
{
	char *r;

	GDEVinit(NULL);
	GDEVsets("program", argv[0]);
	if((r = fetcharg('=')) != NULL)
		GDEVsets("Xgeometry", r);

	while((r = fetcharg('#')) != NULL) {
		register char *p = index(r, '#');
		if(p != NULL) {
			*p++ = '\0';
			if(isdigit(*p)) {
				GDEVseti(r, atoi(p));
				printf("seti %s=%d\n", r, atoi(p));
			}
			else {
				GDEVsets(r, p);
				printf("sets %s=%s\n", r, p);
			}
		}
	}


	ARGLOOP
		FLAGARG('G')	gdevdebug++;		ENDFLAGARG
		FLAGARG('w')	NoWait++;		ENDFLAGARG
		STRINGARG('t')	Tests = p;		ENDSTRINGARG
		STRINGARG('o')	GDEVrecord(p);		ENDSTRINGARG
		STRINGARG('i')	GDEVplay(p);		ENDSTRINGARG
			
		BADARG
			fprintf(stderr, "unknown option %c\n", *p);
			exit(1);
		ENDBADARG
	ENDARGLOOP

	init_mouse();
}

init_mouse()
{
	char rv[3];

	GDEVstart();

	rv[0] = MOUSE_CHAR; rv[1] = LEFT_DOWN; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(LEFT_BUTTON, DOWN_TRANSITION), rv);

	rv[0] = MOUSE_CHAR; rv[1] = LEFT_MOVE; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(LEFT_BUTTON, DOWN_MOVEMENT), rv);

	rv[0] = MOUSE_CHAR; rv[1] = LEFT_UP; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(LEFT_BUTTON, UP_TRANSITION), rv);

	rv[0] = MOUSE_CHAR; rv[1] = RIGHT_DOWN; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(RIGHT_BUTTON, DOWN_TRANSITION), rv);

	rv[0] = MOUSE_CHAR; rv[1] = RIGHT_MOVE; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(RIGHT_BUTTON, DOWN_MOVEMENT), rv);

	rv[0] = MOUSE_CHAR; rv[1] = RIGHT_UP; rv[2] = '\0';
	GDEVmouse(MOUSE_EVENT(RIGHT_BUTTON, UP_TRANSITION), rv);

	GDEVflush();

}

Wait()
{
	int c;
	int button;
	int x, y;

	printf("click mouse or hit return to continue\n");
	for(;;) switch(c = GDEVgetchar()) {
	case MOUSE_CHAR:
		button = GDEVgetchar();
		GDEVgetXYT(&x, &y, NULL);
		switch(button) {
		case RIGHT_UP:
		case LEFT_UP:
			return;
		}
		break;

	case 'q':
	case EOF:
		exit(0);

	case '\n':
		return;


	default:
		printf("Got %c\n", c);
	}
}

/* ------------------- tests ------------------------ */


output_test()
{
	int width, height;
	int xincr, yincr;
	int i;
	char buf[100];
	int xshim, yshim;

	GDEVstart();

	GDEVseti("thickness", 1);
	GDEVgetdim(&width, &height);
	printf("-------- Output Test ---------\n");
	printf("width = %d  height = %d\n", width, height);

	xincr = width / 10; width = xincr * 10; xshim = xincr/10;
	yincr = height / 10; height = yincr * 10; yshim = yincr/10;

	for(i = 0; i <= 10; i++) {
		GDEVline(i*xincr, 0, i*xincr, height);
		GDEVline(0, i*yincr, width, i*yincr);
	}

	GDEVtext(0,0, "0,0");
	sprintf(buf, "%d,%d", 5*xincr, 5*yincr);
	GDEVtext(5*xincr,5*yincr, buf);

	/* thickness test */
	for(i = 1; i < 10; i++) {
		sprintf(buf, "%d", i);
		GDEVtext(i*xincr - xshim, yincr - yshim, buf);
		GDEVseti("thickness", i);
		GDEVline(i*xincr, yincr,
			(int) ((i+.75)*xincr), (int) (yincr*1.75) );
	}
	GDEVseti("thickness", 1);
}

input_test()
{
	printf("-------- Input Test ---------\n");
}


menu_test()
{
	printf("-------- Menu Test ---------\n");

	GDEVmenuitem("Item 1", "1");
	GDEVmenuitem("Item 2", "2");
	GDEVmenuitem("Quit", "q");
}

draw_test()
{
	int x, y;
	int lx, ly;
	int button;
	int c;

	printf("-------- Draw Test ---------\n");

	GDEVstart(); GDEVflush();
	for(;;) {
		switch(c = GDEVgetchar()) {
		case MOUSE_CHAR:
			button = GDEVgetchar();
			GDEVgetXYT(&x, &y, NULL);
			switch(button) {
			case RIGHT_DOWN:
				return;
			case LEFT_MOVE:
				GDEVline(x, y, lx, ly);
				GDEVflush();
				/* fall into .. */
			case LEFT_DOWN:
				lx = x; ly = y;
				break;
			}
			break;

		case 'q':
		case EOF:
			GDEVstoprecording();
			return;
			
		default:
			printf("got %c\n", c);
			break;
		}

	}
}

/*-----------------------------------------------------------------*/

