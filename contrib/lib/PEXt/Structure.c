/*
			S t r u c t u r e . c

    $Header: Structure.c,v 1.0 91/10/04 17:01:05 rthomson Exp $

    Structure convenience functions.			

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
#include "Structure.h"

Pmatrix3 PEXtIdentityMatrix = {		/* Identity matrix */
  { 1., 0., 0., 0. },
  { 0., 1., 0., 0. },
  { 0., 0., 1., 0. },
  { 0., 0., 0., 1. }
};

/*
  PEXtRotate

  Compose a 2D rotation matrix as a local transform structure element.
*/
void PEXtRotate(Angle, Composition)
     Pfloat Angle;
     Pcompose_type Composition;
{
  Pmatrix matrix;

  protate(Angle, &PEXtError, matrix);
  SAFE_PEX("PEXtRotate.protate");
  pset_local_tran(matrix, Composition);
}

/*
  PEXtRotateX

  Compose a rotation matrix as a local transform structure.
*/
void PEXtRotateX(Angle, Composition)
     Pfloat Angle;
     Pcompose_type Composition;
{
  Pmatrix3 matrix;

  protate_x(Angle, &PEXtError, matrix);
  SAFE_PEX("PEXtRotateX.protate_x");
  pset_local_tran3(matrix, Composition);
}

/*
  PEXtRotateY

  Compose a rotation matrix as a local transform structure.
*/
void PEXtRotateY(Angle, Composition)
     Pfloat Angle;
     Pcompose_type Composition;
{
  Pmatrix3 matrix;
  
  protate_y(Angle, &PEXtError, matrix);
  SAFE_PEX("PEXtRotateY.protate_y");
  pset_local_tran3(matrix, Composition);
}

/*
  PEXtRotateZ

  Compose a rotation matrix as a local transform structure.
*/
void PEXtRotateZ(Angle, Composition)
     Pfloat Angle;
     Pcompose_type Composition;
{
  Pmatrix3 matrix;

  protate_z(Angle, &PEXtError, matrix);
  SAFE_PEX("PEXtRotateZ.protate_z");
  pset_local_tran3(matrix, Composition);
}

/*
  PEXtTranslate

  Compose a 2D translation matrix as a local transform structure element.
*/
void PEXtTranslate(X, Y, Composition)
     Pfloat X, Y;
     Pcompose_type Composition;
{
  Pvec vector;
  Pmatrix matrix;

  vector.delta_x = X, vector.delta_y = Y;
  ptranslate(&vector, &PEXtError, matrix);
  SAFE_PEX("PEXtTranslate.ptranslate");
  pset_local_tran(matrix, Composition);
}  

/*
  PEXtTranslate3

  Compose a 3D translation matrix as a local xform structure
*/
void PEXtTranslate3(x, y, z, comp)
     Pfloat x,y,z;
     Pcompose_type comp;
{
  Pvec3 vector;
  Pmatrix3 matrix;

  vector.delta_x = x, vector.delta_y = y, vector.delta_z = z;
  ptranslate3(&vector, &PEXtError, matrix);
  SAFE_PEX("PEXtTranslate.ptranslate3");
  pset_local_tran3(matrix, comp);
}

/*
  PEXtNonUniformScale

  Compose a non-uniform 2D scale matrix as a local xform structure
*/
void PEXtNonUniformScale(sfx, sfy, comp)
     Pfloat sfx, sfy;
     Pcompose_type comp;
{
  Pvec vector;
  Pmatrix matrix;

  vector.delta_x = sfx, vector.delta_y = sfy;
  pscale(&vector, &PEXtError, matrix);
  SAFE_PEX("PEXtNonUniformScale.pscale");
  pset_local_tran(matrix, comp);
}

/*
  PEXtNonUniformScale3

  Compose a non-uniform scale matrix as a local xform structure
*/
void PEXtNonUniformScale3(sfx, sfy, sfz, comp)
     Pfloat sfx, sfy, sfz;
     Pcompose_type comp;
{
  Pvec3 vector;
  Pmatrix3 matrix;

  vector.delta_x = sfx, vector.delta_y = sfy, vector.delta_z = sfz;
  pscale3(&vector, &PEXtError, matrix);
  SAFE_PEX("PEXtNonUniformScale.pscale3");
  pset_local_tran3(matrix, comp);
}

/*
  PEXtBuildTran

  Compose an arbitrary matrix as a local transform structure element.
*/
void PEXtBuildTran(fixed_x, fixed_y, shift_x, shift_y,
		   angle, scale_x, scale_y, comp)
     Pfloat fixed_x, fixed_y, shift_x, shift_y, angle, scale_x, scale_y;
     Pcompose_type comp;
{
  Ppoint fixed;
  Pvec shift;
  Pvec scale;
  Pmatrix matrix;

  fixed.x = fixed_x, fixed.y = fixed_y;
  shift.delta_x = shift_x, shift.delta_y = shift_y;
  scale.delta_x = scale_x, scale.delta_y = scale_y;
  pbuild_tran_matrix(&fixed, &shift, angle, &scale, &PEXtError, matrix);
  SAFE_PEX("PEXtBuildTran.pbuild_tran_matrix");
  pset_local_tran(matrix, comp);
}

/*
  PEXtBuildTran3

  Compose an arbitrary 3D matrix as a local transform structure element.
*/
void PEXtBuildTran3(fixed_x, fixed_y, fixed_z,
		    shift_x, shift_y, shift_z,
		    x_angle, y_angle, z_angle,
		    scale_x, scale_y, scale_z, comp)
     Pfloat fixed_x, fixed_y, fixed_z,
       shift_x, shift_y, shift_z,
       x_angle, y_angle, z_angle,
       scale_x, scale_y, scale_z;
     Pcompose_type comp;
{
  Ppoint3 fixed;
  Pvec3 shift;
  Pvec3 scale;
  Pmatrix3 matrix;
  register int i, j;

  for (i = 0; i < 4; i++)
    for (j = 0; j < 4; j++)
      matrix[i][j] = (i == j) ? 1.0 : 0.0;

  fixed.x = fixed_x, fixed.y = fixed_y, fixed.z = fixed_z;
  shift.delta_x = shift_x, shift.delta_y = shift_y, shift.delta_z = shift_z;
  scale.delta_x = scale_x, scale.delta_y = scale_y, scale.delta_z = scale_z;
  pbuild_tran_matrix3(&fixed, &shift, x_angle, y_angle, z_angle, &scale,
		      &PEXtError, matrix);
  SAFE_PEX("PEXtBuildTran3.pbuild_tran_matrix3");
  pset_local_tran3(matrix, comp);
}
