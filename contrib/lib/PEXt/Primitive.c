/*
			P r i m i t i v e . c

    $Header: Primitive.c,v 1.0 91/10/04 17:01:00 rthomson Exp $

    Primitive emulations to allow higher-level primitives.

    Author:	Rich Thomson
    Date:	April 24th, 1990

		Copyright (C) 1990, 1991, Evans & Sutherland

*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			   Salt Lake City, Utah

			    All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its 
    documentation for any purpose and without fee is hereby granted, 
    provided that the above copyright notice appear in all copies and that
    both that copyright notice and this permission notice appear in 
    supporting documentation, and that the names of Evans & Sutherland not be
    used in advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.  

    EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*
			    Include Files
*/
#include <stdio.h>
#include <math.h>

#include "PEXt.h"
#include "Primitive.h"
#include "Sphere.h"
#include "Structure.h"

/*
  PEXtCircle

  3D circle primitive, with circle in Z = CenterZ plane.
*/
void PEXtCircle(CenterX, CenterY, CenterZ, Radius, NumSegments)
     Pfloat CenterX, CenterY, CenterZ, Radius;
     Pint NumSegments;
{
  register float x, y, inc;
  register int i;
  Ppoint3 *points, *current;

  points = (Ppoint3 *) calloc(NumSegments+1, sizeof(Ppoint3));

  x = CenterX + Radius;
  y = CenterY;
  inc = 2.0*PEXtPI/NumSegments;

  for (i = 0, current = points; i < NumSegments; current++)
    {
      current->x = x, current->y = y, current->z = CenterZ;

      i++;
      x = CenterX + Radius*cos(i*inc), y = CenterY + Radius*sin(i*inc);
    }
  current->x = CenterX + Radius;	/* force agreement on last point */
  current->y = CenterY;
  current->z = CenterZ;
  ppolyline3(NumSegments+1, points);
    
  free(points);
}

/*
  PEXtEllipse2Points

  Compute the points needed for a 2D ellipse and return them as an
  array of 2D points.
*/
static Ppoint *_PEXtEllipse2Points(CenterX, CenterY, Radius, Eccentricity,
				NumSegments)
     Pfloat CenterX, CenterY, Radius, Eccentricity;
     Pint NumSegments;
{
  register float x, y, inc, minor, major;
  register int i;
  Ppoint *points, *current;

  points = (Ppoint *) calloc(NumSegments+1, sizeof(Ppoint));

  inc = 2.0*PEXtPI/NumSegments;
  if (Eccentricity > 1.0)		/* taller than wide */
    {
      major = Radius/Eccentricity;
      minor = Radius;
    }
  else					/* wider than tall */
    {
      major = Radius;
      minor = Radius*Eccentricity;
    }
  x = CenterX + major;
  y = CenterY;

  for (i = 0, current = points; i < NumSegments; current++)
    {
      current->x = x, current->y = y;

      i++;
      x = CenterX + major*cos(i*inc), y = CenterY + minor*sin(i*inc);
    }
  current->x = CenterX + major;		/* force agreement on last point */
  current->y = CenterY;

  return points;
}

/*
  PEXtEllipse2

  Ellipse created with polyline.
*/
void PEXtEllipse2(CenterX, CenterY, Radius, Eccentricity, NumSegments)
     Pfloat CenterX, CenterY,		/* intersection point of */
					/* major and minor axes */
       Radius,				/* radius of major axis */
       Eccentricity;			/* height/width */
     Pint NumSegments;
{
  Ppoint *points;

  points = _PEXtEllipse2Points(CenterX, CenterY, Radius, Eccentricity,
			       NumSegments);
  ppolyline(NumSegments+1, points);
    
  free(points);
}

/*
  PEXtCircle2

  Circle created as ellipse with Eccentricity = 1.0
*/
void PEXtCircle2(CenterX, CenterY, Radius, NumSegments)
     Pfloat CenterX, CenterY, Radius;
     Pint NumSegments;
{
  PEXtEllipse2(CenterX, CenterY, Radius, 1.0, NumSegments);
}

/*
  PEXtEllipsoid2

  2D ellipse created with fillarea
*/
void PEXtEllipsoid2(CenterX, CenterY, Radius, Eccentricity, NumSegments)
     Pfloat CenterX, CenterY,		/* intersection point of */
					/* major and minor axes */
       Radius,				/* radius of major axis */
       Eccentricity;			/* height/width */
     Pint NumSegments;
{
  Ppoint *points;

  points = _PEXtEllipse2Points(CenterX, CenterY, Radius, Eccentricity,
			       NumSegments);
  pfill_area(NumSegments+1, points);

  free(points);
}

/*
  PEXtCube

  This procedure creates a cube, with sides of the given length,
  centered on the origin using PFillArea3.  The cube is edited into
  the currently open structure.

  Required Edit Mode:	PEDIT_INSERT
*/
#define PT(j,k,l) (verts[i].x = j, verts[i].y = k, verts[i].z = l, i++)
void PEXtCube(side)
     double side;
{
  Ppoint_list3 face;
  Ppoint3 verts[4];
  register int i;
  register Pfloat pos = side*0.5, neg = -side*0.5;

  face.num_points = 4;
  face.points = verts;

  i = 0;
  PT(pos, pos, neg); PT(neg, pos, neg);
  PT(neg, pos, pos); PT(pos, pos, pos);
  pfill_area3(&face);
  
  i = 0;
  PT(pos, neg, neg); PT(pos, neg, pos);
  PT(neg, neg, pos); PT(neg, neg, neg);
  pfill_area3(&face);
  
  i = 0; 
  PT(neg, neg, pos); PT(neg, pos, pos);
  PT(neg, pos, neg); PT(neg, neg, neg);
  pfill_area3(&face);
  
  i = 0;
  PT(pos, pos, neg); PT(pos, pos, pos);
  PT(pos, neg, pos); PT(pos, neg, neg);
  pfill_area3(&face);
  
  i = 0;
  PT(pos, pos, pos); PT(neg, pos, pos);
  PT(neg, neg, pos); PT(pos, neg, pos);
  pfill_area3(&face);
  
  i = 0;
  PT(pos, pos, neg); PT(pos, neg, neg);
  PT(neg, neg, neg); PT(neg, pos, neg);
  pfill_area3(&face);
}

/*
  PEXtCylinder

  This routine creates a cylinder, of the given height and radius,
  centered on the origin, with its axis oriented along the z axis.
  The cylinder is created with num_strips planar strips with normals
  at the vertices.  If num_strips is specified as zero, then 20 strips
  will be used.  The cylinder is edited into the currently open
  structure.

  Required Edit Mode:	PEDIT_INSERT
*/
void PEXtCylinder(num_strips, height, radius)
     int num_strips;
     double height, radius;
{
  register int i, j, k;
  Pfacet_vdata_list3 fdata;
  Pptnorm3 vdata[4];
  double scale;

  if (num_strips <= 0)
    num_strips = 20;

  scale = M_PI*2.0/num_strips;
  fdata.num_vertices = 4;
  fdata.vertex_data.ptnorms = vdata;
					/* these values don't change over j */
  vdata[0].point.z = vdata[1].point.z = -height/2.0;
  vdata[2].point.z = vdata[3].point.z = height/2.0;
					/* setup for first patch */
  vdata[0].norm.delta_z = vdata[1].norm.delta_z =
    vdata[2].norm.delta_z = vdata[3].norm.delta_z = 0.0;

  vdata[0].norm.delta_x = vdata[3].norm.delta_x =
    vdata[0].point.x = vdata[3].point.x = radius;
  vdata[0].norm.delta_y = vdata[3].norm.delta_y =
    vdata[0].point.y = vdata[3].point.y = 0.0;
  
  for (j = 0; j < num_strips-1; j++)	/* for each patch */
    {
      vdata[1].norm.delta_x = vdata[2].norm.delta_x =
	vdata[1].point.x = vdata[2].point.x = radius*cos((j+1)*scale);
      vdata[1].norm.delta_y = vdata[2].norm.delta_y =
	vdata[1].point.y = vdata[2].point.y = radius*sin((j+1)*scale);
  
      pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
			   PMODEL_RGB, NULL, 1, NULL, &fdata);

      vdata[0].norm.delta_x = vdata[3].norm.delta_x =
	vdata[0].point.x = vdata[3].point.x = vdata[1].point.x;
      vdata[0].norm.delta_y = vdata[3].norm.delta_y =
	vdata[0].point.y = vdata[3].point.y = vdata[1].point.y;
    }
					/* setup for last patch */
  vdata[1].norm.delta_x = vdata[2].norm.delta_x =
    vdata[1].point.x = vdata[2].point.x = radius;
  vdata[1].norm.delta_y = vdata[2].norm.delta_y =
    vdata[1].point.y = vdata[2].point.y = 0.0;

  pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
		       PMODEL_RGB, NULL, 1, NULL, &fdata);
}

/*
  PEXtCone

  This routine creates a cone of the given height and radius, centered on
  the origin, with its axis oriented along the positive Z axis.  The cone
  is created with num_triangles triangles in a radial fashion around the
  center axis of the cone.  No structure elements are created if the number
  if triangles is <= 0.

  TODO: correct points, normals
*/
void PEXtCone(num_triangles, height, radius)
     int num_triangles;
     double height, radius;
{
  register int i, j, k;
  Pfacet_vdata_list3 fdata;
  Pptnorm3 vdata[3];
  double scale;

  if (num_triangles <= 0)
    return;

  scale = M_PI*2.0/num_triangles;
  fdata.num_vertices = 4;
  fdata.vertex_data.ptnorms = vdata;
					/* these values don't change over j */
  vdata[0].point.x = vdata[0].point.y = vdata[0].point.z = 0.0;
  vdata[1].point.z = vdata[2].point.z = height;
					/* setup for first patch *b/
  vdata[0].norm.delta_z = vdata[1].norm.delta_z =
    vdata[2].norm.delta_z = vdata[3].norm.delta_z = 0.0;

  vdata[0].norm.delta_x = vdata[3].norm.delta_x =
    vdata[0].point.x = vdata[3].point.x = radius;
  vdata[0].norm.delta_y = vdata[3].norm.delta_y =
    vdata[0].point.y = vdata[3].point.y = 0.0;
  
  for (j = 0; j < num_triangles-1; j++)	/* for each patch */
    {
      vdata[1].norm.delta_x = vdata[2].norm.delta_x =
	vdata[1].point.x = vdata[2].point.x = radius*cos((j+1)*scale);
      vdata[1].norm.delta_y = vdata[2].norm.delta_y =
	vdata[1].point.y = vdata[2].point.y = radius*sin((j+1)*scale);
  
      pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
			   PMODEL_RGB, NULL, 1, NULL, &fdata);

      vdata[0].norm.delta_x = vdata[3].norm.delta_x =
	vdata[0].point.x = vdata[3].point.x = vdata[1].point.x;
      vdata[0].norm.delta_y = vdata[3].norm.delta_y =
	vdata[0].point.y = vdata[3].point.y = vdata[1].point.y;
    }
					/* setup for last patch */
  vdata[1].norm.delta_x = vdata[2].norm.delta_x =
    vdata[1].point.x = vdata[2].point.x = radius;
  vdata[1].norm.delta_y = vdata[2].norm.delta_y =
    vdata[1].point.y = vdata[2].point.y = 0.0;

  pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
		       PMODEL_RGB, NULL, 1, NULL, &fdata);
}

/*
  PEXtSphere

  This procedure creates a sphere of radius 1.0, centered at the
  origin.  The sphere is constructed of patches with vertex normals so
  that it is evenly shaded.  The data for the patches is contained in
  the file ``sphere.h''.

  Required Edit Mode:	PEDIT_INSERT
*/
void PEXtSphere(radius)
     double radius;
{
  register int i;
  Pfacet_vdata_list3 vertices;

  PEXtUniformScale(radius, PTYPE_POSTCONCAT);

  vertices.num_vertices = 21;
  vertices.vertex_data.ptnorms = PEXtSphereTopPoints;
  pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
		       PMODEL_RGB, NULL, 1, NULL, &vertices);

  vertices.vertex_data.ptnorms =  PEXtSphereBottomPoints;
  pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
		       PMODEL_RGB, NULL, 1, NULL, &vertices);

  vertices.num_vertices = 4;
  for (i = 0; i < 399; i++)
    {
      vertices.vertex_data.ptnorms = PEXtSphereMiddlePoints[i];
      pfill_area_set3_data(PFACET_NONE, PEDGE_NONE, PVERT_COORD_NORMAL,
			   PMODEL_RGB, NULL, 1, NULL, &vertices);
    }
}
