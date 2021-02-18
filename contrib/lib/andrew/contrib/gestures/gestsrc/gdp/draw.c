/***********************************************************************

Copyright (C) 1991 Dean Rubine

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.

**********************************************************************/
#include <gestures/bool.h>
#include <gestures/util.h>
#include "set.h"
#include "object.h"
#include "iobject.h"
#include <gestures/matrix.h>
#include "transform.h"
#include <stdio.h>
#include <math.h>

int	DrawDebug = 0;
#define	D if(DrawDebug)

static void
dmpo(o)
Object o;
{
	printf("	-(");
	DumpObject(o);
}

DumpObject(o)
register Object o;
{
	register int i;
	register struct dope *d = &dope[(int) o->type];

	printf("%s <%x> ", d->name, o);
	for(i = 0; i < d->npoints; i++)
		printf(" (%d,%d)", 
			o->point[i].x,
		   	o->point[i].y);
	if(o->text != NULL)
		printf("'%s'", o->text);
	printf(")\n");
	if(o->type == SetOfObjects)
		Map(o->subobjects, dmpo, NULL);
}

static
Bool
Drawable(o)
register Object o;
{
	register int i;
	register struct dope *d = &dope[(int) o->type];
	for(i = 0; i < d->npoints; i++)
		if(o->point[i].x == NOT_A_POINT ||
		   o->point[i].y == NOT_A_POINT)
			return FALSE;
	if(o->type == Text && o->text == NULL)
		return FALSE;
	return TRUE;
}

static
void
ReallyDraw(o)
register Object o;
{
	D printf("ReallyDraw("), DumpObject(o);   /* debug */
	if(Drawable(o)) {
		GDEVsets("currentlinetype", o->highlight ? "dotted" : "solid");
		(*dope[(int)o->type].draw)(o);	
	}
}


/* ObjEqual does a straight bit compare */

Bool
ObjEqual(o1, o2)
register Object o1, o2;
{
	return ! bcmp( (char *) o1, (char *) o2, sizeof(struct object));
}

/* cache of things to draw */

#define	CACHESIZE	1024

static int
hash(o)
register Object o;
{
	return  (unsigned) (o->point[0].x + o->point[0].y +
		o->point[1].x + o->point[1].y +
		o->point[2].x + o->point[2].y +
		(int) o->type) % CACHESIZE;
}

static struct cache {
	struct	cache *next;
	struct	cache *prev;
	struct	object	o;
} cache[CACHESIZE];

struct	cache headspace = { &headspace, &headspace }, *head = &headspace;

static struct {
	int	flushes;
	int	news;
	int	hits;
	int	collisions;
} stat;

CacheStats()
{
	printf("%d new %d hits %d collisions %d flushes\n",
	stat.flushes, stat.news, stat.hits, stat.collisions);
}

CacheFlush(fcn)
void (*fcn)(/* Object */);
{
	register struct cache *c;

	stat.flushes++;
	for(c = head->next; c != head; c = c->next) {
		if(fcn)
			(*fcn)(&c->o);
		c->o.type = Nothing;
	}
	head->prev = head->next = head;
}

/* possible cache return values */

#define	C_NEW		0
#define	C_COLLISION	1
#define	C_HIT		2

int
CacheEnter(o)
Object o;
{
	register int h = hash(o);
	register struct cache *r = &cache[h];

	D printf("hash=%d ", h);
	if(ObjEqual(&r->o, o)) {
		/* hit - delete from cache */
		r->o.type = Nothing;
		r->prev->next = r->next;
		r->next->prev = r->prev;
		stat.hits++;
		return C_HIT;
	}
	if(r->o.type == Nothing) {
		r->o = *o;
		r->next = head->next;
		r->prev = head;
		head->next->prev = r;
		head->next = r;
		stat.news++;
		return C_NEW;
	}
	stat.collisions++;
	return C_COLLISION;
}

void
Erase(o)
register Object o;
{
	D printf("Erase ");
	Draw(o);
}

void
Draw(o)
register Object o;
{
	D printf("Draw("), DumpObject(o);   /* debug */
	if(o->type == SetOfObjects)
		ReallyDraw(o);
	else {
		switch(CacheEnter(o)) {
		case C_HIT:	/* already in cache - no need to draw */
			D printf("Cache hit - not drawing\n");
			break;
		case C_NEW:	/* cache slot empty - will be drawn later */
			D printf("Cache empty - maybe draw later\n");
			break;
		case C_COLLISION:    /* cache slot full so draw immediately */
			D printf("Cache collision - draw immediately\n");
			ReallyDraw(o);
			break;
		}
	}
}

void
DrawSync()
{
	register Object o;

	D printf("DrawSync..");   /* debug */
	CacheFlush(ReallyDraw);
	WmFlush();
	D printf("Done\n");   /* debug */
}


DrawClear()
{
	register Object o;
	CacheFlush(NULL);
	WmClear();
}

/*-----------------------------------------------------------------*/

void
LineDraw(o)
register Object o;
{
	/* printf("<%d %d %d %d> ", o->point[0].x, o->point[0].y, o->point[1].x, o->point[1].y); fflush(stdout); */
	GDEVline(o->point[0].x, o->point[0].y, o->point[1].x, o->point[1].y);
}

void
LineTransform(o, t)
register Object o;
{
	ApplyTran(o->point[0].x, o->point[0].y, t,
		&o->point[0].x, &o->point[0].y);
	ApplyTran(o->point[1].x, o->point[1].y, t,
		&o->point[1].x, &o->point[1].y);
}

void
RectDraw(o)
register Object o;
{
	register int x1 = o->point[0].x, y1 = o->point[0].y;
	register int x2 = o->point[1].x, y2 = o->point[1].y;
	register int x3 = o->point[2].x, y3 = o->point[2].y;
	register int x4 = x1+x3-x2, y4 = y1+y3-y2;

	GDEVline(x1, y1, x2, y2);
	GDEVline(x2, y2, x3, y3);
	GDEVline(x3, y3, x4, y4);
	GDEVline(x4, y4, x1, y1);
}

void
RectTransform(o, t)
register Object o;
Transformation t;
{
	ApplyTran(o->point[0].x, o->point[0].y, t,
		&o->point[0].x, &o->point[0].y);
	ApplyTran(o->point[1].x, o->point[1].y, t,
		&o->point[1].x, &o->point[1].y);
	ApplyTran(o->point[2].x, o->point[2].y, t,
		&o->point[2].x, &o->point[2].y);
}


void
TextDraw(o)
register Object o;
{
	GDEVtext(o->point[0].x, o->point[0].y, "text");
}

void
TextTransform(o, t)
register Object o;
Transformation t;
{
	ApplyTran(o->point[0].x, o->point[0].y, t,
		&o->point[0].x, &o->point[0].y);
}

void
CircleDraw(o)
register Object o;
{
#define NSEGS   16
#define	SHIFT	8
#define FACTOR	(1<<SHIFT)
	static called;
	static int _x[NSEGS+1], _y[NSEGS+1];
	register int i;
	int x = o->point[0].x, y = o->point[0].y, r = o->point[1].x;

	if(!called) {
		double pi = 4 * atan(1.0);
		for(i = 0; i <= NSEGS; i++) {
			_x[i] = FACTOR * cos( (2*pi*i)/NSEGS );
			_y[i] = FACTOR * sin( (2*pi*i)/NSEGS );
		}
		called = 1;
	}

	for(i = 0; i < NSEGS; i++)
	    GDEVline(x + ((_x[i]*r)>>SHIFT), y + ((_y[i]*r)>>SHIFT),
		     x + ((_x[i+1]*r)>>SHIFT), y + ((_y[i+1]*r)>>SHIFT) );
}

void
CircleTransform(o, t)
register Object o;
Transformation t;
{
	ApplyTran(o->point[0].x, o->point[0].y, t,
		&o->point[0].x, &o->point[0].y);
	o->point[1].x *= TransScale(t);
}

int
DistanceToPoint(x1, y1, x2, y2)
int x1, y1, x2, y2;
{
	int dx = x2 - x1, dy = y2 - y1;
	return dx * dx + dy * dy;
}

#define	inorder(a, b, c)	( ((a) <= (b) && (b) <= (c)) || \
				  ((c) <= (b) && (b) <= (a)) ) 
/*

   Distance from point to line segment.
   Based on Amon/Agin algorithm found in
 	/../h/usr/amon/NP2/np1.mss

  Let A=(xa,ya) B=(xb,yb) 	X=(x,y)

  Now, the line between A and B is given parametrically by
	V(k) = (1-k)A + kB
  and adding the constraint 0 <= k <= 1 makes V(K) the line sgement from A to B

  Now, the line through X perpendicular to V(k) intersects V(k) when k equals

	    (B - A) . (X - A)
	kp = ------------------
	       | B - A |^2

 So if kp =< 0, X is closest to A, kp >= 1, X is closest to B, and if 
	0 < k < kp, X is closest to V(kp).

*/


static
int
DistanceToLineSeg(x, y, xa, ya, xb, yb)
int x, y, xa, ya, xb, yb;
{
	int xba = xb - xa, yba = yb - ya;
	double kp;
	
	if(xba == 0 && yba == 0)	/* degenerate line */
		return DistanceToPoint(x, y, xa, ya);

	kp = ( (double) ( xba * (x - xa) + yba * (y - ya) ) ) /
	      			(xba * xba + yba * yba);
	if(kp <= 0)
		return DistanceToPoint(x, y, xa, ya);
	else if(kp >= 1)
		return DistanceToPoint(x, y, xb, yb);
	else
		return DistanceToPoint(x, y, (int) ((1 - kp) * xa + kp * xb),
					     (int) ((1 - kp) * ya + kp * yb));
}

int
LineDistance(o, x, y)
register Object o;
{
	return DistanceToLineSeg(x, y, o->point[0].x, o->point[0].y,
				    o->point[1].x, o->point[1].y);
}

static int
min(a, b)
int a, b;
{
	return a < b ? a : b;
}

int
RectDistance(o, x, y)
register Object o;
int x, y;
{
	register int x1 = o->point[0].x, y1 = o->point[0].y;
	register int x2 = o->point[1].x, y2 = o->point[1].y;
	register int x3 = o->point[2].x, y3 = o->point[2].y;
	register int x4 = x1+x3-x2, y4 = y1+y3-y2;

	return  min(DistanceToLineSeg(x, y, x1, y1, x2, y2),
		min(DistanceToLineSeg(x, y, x2, y2, x3, y3),
		min(DistanceToLineSeg(x, y, x3, y3, x4, y4),
		DistanceToLineSeg(x, y, x4, y4, x1, y1))));
}

int
TextDistance(o, x, y)
register Object o;
int x, y;
{
	return DistanceToPoint(x, y, o->point[0].x, o->point[0].y);
}

int
CircleDistance(o, x, y)
register Object o;
int x, y;
{
	int dx = o->point[0].x - x, dy = o->point[0].y - y;
	double c =  dx * dx + dy * dy;
	double sqrt();
	double d = sqrt(c) - o->point[1].x;
	return (int) (d * d);
}

int
inrec(x, y, x1, y1, x2, y2)
register int x, y, x1, y1, x2, y2;
{
	register int t;
	if(x1 > x2) t = x1, x1 = x2, x2 = t;
	if(y1 > y2) t = y1, y1 = y2, y2 = t;
	return x1 <= x && x <= x2 && y1 <= y && y <= y2;
}

void
SetDraw(o)
register Object o;
{
	Map(o->subobjects, Draw, NULL);
}

void
SetTransform(o, t)
register Object o;
Transformation t;
{
	void Transform();

	Map(o->subobjects, Transform, t);
}

struct size {
	int x, y, min_d;
	int count;
};

static void
sizem(o, s)
Object o;
struct size *s;
{
	int d = Distance(o, s->x, s->y);
	if(d < s->min_d)
		s->min_d = d;
	s->count++;
}

int
SetDistance(o, x, y)
register Object o;
int x, y;
{
	struct size s;

	s.x = x; s.y = y;
	s.count = 0;
	s.min_d = 2000000;
	Map(o->subobjects, sizem, (Pointer) &s);
	/* printf("(%d,%d) ", s.count, s.min_d); fflush(stdout); */
	return s.min_d;
}

