/***********************************************************************

region.c - kaka for mrecord to work

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
/*
 region.c - manipulates a GDEV window
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <gestures/gdev.h>
#include "region.h"

void
REGIONinit()
{

	GDEVinit(NULL);
	GDEVsets("program", "gsink");
}

/* char	*template = "out.%d.rec"; */
char	*template = NULL;
char	name[100];
int	current = 1;

void
setrecordfile(filename)
char *filename;
{
	register char *p, *endp;
	register int i;
	static char fname[100];

	strcpy(fname, filename);

	current = 1;
	for(p = &filename[strlen(filename) - 1]; p >= filename; p--)
		if(isdigit(*p)) {
			for(endp = p ; isdigit(*endp); endp--)
				/* */ ;
			endp++;
			current = atoi(endp);
			for(i = 0; &filename[i] < endp; i++)
				fname[i] = filename[i];
			fname[i++] = '%';
			fname[i++] = 'd';
			for(++p; *p != '\0'; p++, i++)
				fname[i] = *p;
			fname[i] = '\0';
			break;
		}

 	if(index(fname, '%') == NULL)
		printf("Warning: File name \"%s\" does not contain a number or %%d\n",
			filename);

	template = fname;
}

void
REGIONinit2()
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
