/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/

#define	MAXPOINTS	3
#define	NOT_A_POINT	(-9999)

typedef struct {
	int	x, y;
} Point;

struct object {
	ObjectType	type;
	int		state;
	Point		point[MAXPOINTS];
	char		*text;
	Set		subobjects;
	char		highlight;
};

void	Erase(), Draw();
void	EraseOn(), EraseOff();

void	LineDraw();
void	LineTransform();
int	LineDistance();

void	RectDraw();
void	RectTransform();
int	RectDistance();

void	TextDraw();
void	TextTransform();
int	TextDistance();

void	CircleDraw();
void	CircleTransform();
int	CircleDistance();

void	SetDraw();
void	SetTransform();
int	SetDistance();

Bool	AlwaysOK(), AlwaysBad(), VlinePoint();

extern struct dope {
	ObjectType	type;
	char		*name;
	int		npoints;
	Bool		(*point)();
	void		(*draw)();
	void		(*transform)();
	int		(*distance)();  /* actually distance squared */
} dope[];
