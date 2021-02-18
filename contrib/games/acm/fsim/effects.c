#include "pm.h"
#include <math.h>

extern long random ();
extern char * malloc();

#define SPIKES		16
#define FLAME_SPIKES	8
#define SMOKE_INNER	0.2
#define SMOKE_RADIUS	1.0
#define SMOKE_MIN_RADIUS 0.5
#define SMOKE_VARIATION (SMOKE_RADIUS - SMOKE_MIN_RADIUS)
#define FLAME_RADIUS	0.5
#define FLAME_MIN_RADIUS 0.3
#define FLAME_VARIATION	(FLAME_RADIUS - FLAME_MIN_RADIUS)

double efrandom () {
	return ((random() % 32768) - 16384) / 16384.0;
}

static VObject *explosionTemplate;

int placeExplosion();
static craftType expcraft;

void newExplosion (loc, s, dur1, dur2)
VPoint *loc;
double s;
double dur1, dur2; {

	register int	i;
	register craft	*e;

	for (i=0; i<MAXPROJECTILES; ++i) {
		if (mtbl[i].type == CT_FREE) {
			e = &mtbl[i];
			e->type = CT_EXPLOSION;
			e->Sg = *loc;
			e->escale = s;
			e->duration = (int) (dur1 / deltaT + 0.5);
			e->flameDuration = (int) (dur2 / deltaT + 0.5);
			e->cinfo = &expcraft;
			e->cinfo->placeProc = placeExplosion;
			e->curHeading = e->curRoll = e->curPitch = 0.0;
			break;
		}
	}
}

static VColor *effectBlackColor;

void buildExplosion () {

	register int i, numSpikes, numFlame, numRed, poly;
	register VObject *obj;
	VColor	*redFlameColor, *orangeFlameColor, *color;
	VPoint	vp[3];
	double	x, s;

	numSpikes = SPIKES;
	numFlame = FLAME_SPIKES;
	numRed = numFlame / 2;

	effectBlackColor = VAllocColor ("black");
	redFlameColor = VAllocColor ("red");
#ifndef EIGHT_COLORS
	orangeFlameColor = VAllocColor ("#ffc900");
#endif

	obj = (VObject *) malloc (sizeof(VObject));
	obj->name = "explosion";
	obj->numPolys = numSpikes + numFlame;
	obj->polygon = (VPolygon **) malloc (obj->numPolys*sizeof(VPolygon *));

	poly = 0;

	for (i=0; i<numSpikes; ++i) {
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].x = (SMOKE_MIN_RADIUS + x * SMOKE_VARIATION) * s;
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].y = (SMOKE_MIN_RADIUS + x * SMOKE_VARIATION) * s;
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].z = (SMOKE_MIN_RADIUS + x * SMOKE_VARIATION) * s;
		vp[1].x = efrandom() * SMOKE_INNER;
		vp[1].y = efrandom() * SMOKE_INNER;
		vp[1].z = efrandom() * SMOKE_INNER;
		vp[2].x = efrandom() * SMOKE_INNER;
		vp[2].y = efrandom() * SMOKE_INNER;
		vp[2].z = efrandom() * SMOKE_INNER;
		obj->polygon[poly++] = VCreatePolygon (3, vp, effectBlackColor);
	}

	for (i=0; i<numFlame; ++i) {
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].x = (FLAME_MIN_RADIUS + x * FLAME_RADIUS) * s;
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].y = (FLAME_MIN_RADIUS + x * FLAME_RADIUS) * s;
		x = efrandom();
		s = copysign(1.0, x);
		x = fabs (x);
		vp[0].z = (FLAME_MIN_RADIUS + x * FLAME_RADIUS) * s;
		vp[1].x = efrandom() * SMOKE_INNER;
		vp[1].y = efrandom() * SMOKE_INNER;
		vp[1].z = efrandom() * SMOKE_INNER;
		vp[2].x = efrandom() * SMOKE_INNER;
		vp[2].y = efrandom() * SMOKE_INNER;
		vp[2].z = efrandom() * SMOKE_INNER;
#ifndef EIGHT_COLORS
		if (i < numRed)
			color = redFlameColor;
		else
			color = orangeFlameColor;
#else
		color = redFlameColor;
#endif
		obj->polygon[poly++] = VCreatePolygon (3, vp, color);
	}

	explosionTemplate = obj;

}

int placeExplosion (obj, mtx, poly, cnt)
craft		*obj;
VMatrix		*mtx;
VPolygon	**poly;
int		*cnt; {

	register int i, j, k, n;
	register VPolygon **p;
	VPoint	tmp, *q;

	j = *cnt;

	n = explosionTemplate->numPolys;
	p = explosionTemplate->polygon;

	for (i=0; i<n; ++i) {
		if (obj->flameDuration > 0 || p[i]->color == effectBlackColor) {
		poly[j] = VCopyPolygon(p[i]);
		for ((k=0, q=poly[j]->vertex); k<poly[j]->numVtces; (++k, ++q)) {
			tmp.x = q->x * obj->escale + obj->Sg.x;
			tmp.y = q->y * obj->escale + obj->Sg.y;
			tmp.z = q->z * obj->escale + obj->Sg.z;
			*q = tmp;
		}
		++j;
		}
	}

	*cnt = j;

	return 0;
}
