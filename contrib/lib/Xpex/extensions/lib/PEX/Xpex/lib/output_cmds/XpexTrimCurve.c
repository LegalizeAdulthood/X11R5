/* $Header: XpexTrimCurve.c,v 2.3 91/09/11 16:07:11 sinyaw Exp $ */
/*
 *	XpexTrimCurve.c
 */

/*****************************************************************
Copyright 1991 by Sony Microsystems Company, San Jose, California

					All Rights Reserved

Permission to use, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of Sony not be used
in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

SONY DISCLAIMS ANY AND ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL EXPRESS WARRANTIES AND ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, FOR A PARTICULAR PURPOSE. IN NO EVENT
SHALL SONY BE LIABLE FOR ANY DAMAGES OF ANY KIND, INCLUDING BUT NOT
LIMITED TO SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES RESULTING FROM
LOSS OF USE, DATA OR LOSS OF ANY PAST, PRESENT, OR PROSPECTIVE PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS 
SOFTWARE.
*****************************************************************/

#include "Xpexlib.h"

int
Xpex_sizeof_trim_curve(p)
	XpexTrimCurve *p;
{
	int size = sizeof(pexTrimCurve) +
		p->knots.num_floats * sizeof(XpexFloat) +
		( p->ctrl_points.num_points * 
		(p->type == Xpex_Rational ? 
		sizeof(pexCoord3D) : sizeof(pexCoord2D)));

	return size;
}

/* 
 *
 *
 */

int
Xpex_sizeof_list_of_trim_curve( p)
	XpexListOfTrimCurve *p;
{
	int size = sizeof(CARD32);
	int count = p->count;
	XpexTrimCurve *tc = p->curves;

	while (count--) {
		size += Xpex_sizeof_trim_curve(tc);
		tc++;
	}

	return size;
}

/*
 *
 *
 */

int
Xpex_sizeof_array_of_list_of_trim_curve(p, count)
	XpexListOfTrimCurve *p;
	int count; 
{
	int size = 0;

	while (count--) {
		size += Xpex_sizeof_list_of_trim_curve( p);
		p++;
	}

	return size;
}

/* 
 * 
 * 
 */

void
Xpex_pack_trim_curve(s, stream, pLength)
	XpexTrimCurve *s;
	char *stream; /* RETURN */
	int *pLength; /* RETURN */
{
	int bytes_written;
	pexTrimCurve *d = (pexTrimCurve *) stream;

	d->visibility = (pexSwitch) s->visibility;
	d->order = (CARD16) s->curve_order;
	d->type = (pexCoordType) s->type;
	d->approxMethod = (INT16) s->approx_method;
	d->tolerance = s->tolerance;
	d->tMin = (CARD32) s->t_min;
	d->tMax = (CARD32) s->t_max;
	d->numKnots = (CARD32) s->knots.num_floats;
	d->numCoord = (CARD32) s->ctrl_points.num_points;

	stream += sizeof(pexTrimCurve);
	*pLength = sizeof(pexTrimCurve);
	
	bcopy((char *) s->knots.f_value, stream,
	s->knots.num_floats * sizeof(XpexFloat));

	stream += (s->knots.num_floats * sizeof(XpexFloat));
	*pLength +=  (s->knots.num_floats * sizeof(XpexFloat));
	

	if (s->type == Xpex_Rational) {
		bcopy((char *)s->ctrl_points.points.points_3d, stream,
		s->ctrl_points.num_points * sizeof(pexCoord3D));
		stream += (s->ctrl_points.num_points * sizeof(pexCoord3D));
		*pLength += (s->ctrl_points.num_points * sizeof(pexCoord3D));
	} else {
		bcopy((char *)s->ctrl_points.points.points_2d, stream,
		s->ctrl_points.num_points * sizeof(pexCoord2D));
		stream += (s->ctrl_points.num_points * sizeof(pexCoord2D));
		*pLength += (s->ctrl_points.num_points * sizeof(pexCoord2D));
	}
}

/*
 *
 *
 */

void
Xpex_pack_list_of_trim_curve( pSrc, stream, pLength)
	XpexListOfTrimCurve *pSrc;
	char *stream; /* RETURN */
	int *pLength; /* RETURN */
{
	XpexTrimCurve *p = pSrc->curves;
	CARD32 *count = (CARD32 *) stream;
	int n = pSrc->count;

	*count = n;
	stream += sizeof(CARD32);
	*pLength = sizeof(CARD32);

	while (n--) {
		int bytes_written;

		Xpex_pack_trim_curve( p, stream, &bytes_written);

		stream += bytes_written;
		*pLength += bytes_written;
	}
}

/*
 *
 *
 */

void
Xpex_pack_array_of_list_of_trim_curve( count, pSrc, stream, pLength)
	int count;
	XpexListOfTrimCurve *pSrc;
	char *stream;
	int *pLength;
{
	*pLength = 0; /* Just in case */

	while (count--) {	
		int bytes_written;

		Xpex_pack_list_of_trim_curve( pSrc, stream, &bytes_written);

		*pLength += bytes_written;
		pSrc++; 
	}
}
