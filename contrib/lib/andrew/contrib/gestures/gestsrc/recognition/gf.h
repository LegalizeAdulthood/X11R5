/***********************************************************************

gf.h - read and write gesture classifier files

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

#define	MAXGESTURES	10000
#define	MAXPOINTS	1000

typedef struct gesture *Gesture;
typedef struct gpoint *Gpoint;
typedef struct gesturefile *GestureFile;

GestureFile ReadGestureFile();

struct gesturefile  {
	int ngestures;
	struct gesture {
		int	classindex;
		char	*examplename;
		int	npoints;
		struct gpoint { short path, x, y, t; } *point;
		Vector	y;
	} gesture[MAXGESTURES];
};

#define ClassName(g)	(classname[(g)->classindex])

extern char *classname[];
