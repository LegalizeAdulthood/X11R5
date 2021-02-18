/***********************************************************
Copyright 1987,1991 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/


/*
**++
**  File Name:
**
**	pl_utl.c
**
**  Subsystem:
**
**	PEXlib
**
**  Version:
**
**	V1.0
**
**  Abstract:
**
**	This file contains PEXlib utility routines.
**
**  Keywords:
**
**  Environment:
**
**	VMS and Ultrix
**
**
**  Creation Date:
**
**  Modification History:
**
**	19-Aug-88  
**
**--
*/

/*
 * Table of Contents (Routine names in order of appearance)
 */

/*
 * E   PEXRotationMatrix
 * E   PEXArbRotationMatrix
 * E   PEXScalingMatrix
 * E   PEXTranslationMatrix
 * E   PEXCopyMatrix
 * E   PEXMultiplyMatrices
 * E   PEXIdentityMatrix
 * E   PEXInvertMatrix
 * E   PEXTransform3dPoints
 * E   PEXTransform4dPoints
 * E   PEXLookatViewMatrix
 * E   PEXPolarViewMatrix
 * E   PEXOrthoProjMatrix
 * E   PEXPerspProjMatrix
 * E   PEXComputeNormals
 */

/*
 *   Include Files
 */

#include "pex_features.h"
#include "pex_macros.h"

#include <math.h>

#include "PEXlib.h"
#include "PEXlibint.h"

#include "pl_ref_data.h"

/*+
 * NAME:
 * 	PEXRotationMatrix
 *
 * FORMAT:
 * 	`PEXRotationMatrix`
 *
 * ARGUMENTS:
 *	axis		The axis about which the rotation is to occur.
 *
 *	angle		The rotation angle, in radians.
 *
 *	matrixReturn	Returns the resulting rotation matrix.
 *
 * RETURNS:
 *	Zero if the matrix is computed, non-zero if the `axis' argument is
 *	invalid.
 *
 * DESCRIPTION:
 * 	This routine formats a rotation matrix for a right-handed rotation of
 *	`angle' radians about `axis' and returns it in `matrixReturn'.  `Axis'
 *	can be one of the constants `pxlXAxis', `pxlYAxis', or `pxlZAxis',
 *	defined in the `PEXlib.h' include file.
 *	Angle is measured counter-clockwise when looking along the specified
 *	axis from the positive side of the axis.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXArbRotationMatrix", `PEXScalingMatrix", `PEXTranslationMatrix",
 *	`PEXMultiplyMatrices", `PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 *
 */

Status
PEXRotationMatrix (axis, angle, matrixReturn)

INPUT int		axis;
INPUT float		angle;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    double	sine;
    double	cosine;
    double	ang;

    ang = angle;
    sine = sin (ang);
    cosine = cos (ang);

    switch (axis)
    {
    case pxlXAxis:
        matrixReturn[0][0] = 1.;
        matrixReturn[0][1] = 0.;
        matrixReturn[0][2] = 0.;
        matrixReturn[0][3] = 0.;

        matrixReturn[1][0] = 0.;
        matrixReturn[1][1] = cosine;
        matrixReturn[1][2] = -sine;
        matrixReturn[1][3] = 0.;

        matrixReturn[2][0] = 0.;
        matrixReturn[2][1] = sine;
        matrixReturn[2][2] = cosine;
        matrixReturn[2][3] = 0.;

        matrixReturn[3][0] = 0.;
        matrixReturn[3][1] = 0.;
        matrixReturn[3][2] = 0.;
        matrixReturn[3][3] = 1.;
        break;

    case pxlYAxis:
        matrixReturn[0][0] = cosine;
        matrixReturn[0][1] = 0.;
        matrixReturn[0][2] = sine;
        matrixReturn[0][3] = 0.;

        matrixReturn[1][0] = 0.;
        matrixReturn[1][1] = 1.;
        matrixReturn[1][2] = 0.;
        matrixReturn[1][3] = 0.;

        matrixReturn[2][0] = -sine;
        matrixReturn[2][1] = 0.;
        matrixReturn[2][2] = cosine;
        matrixReturn[2][3] = 0.;

        matrixReturn[3][0] = 0.;
        matrixReturn[3][1] = 0.;
        matrixReturn[3][2] = 0.;
        matrixReturn[3][3] = 1.;
        break;

    case pxlZAxis:
        matrixReturn[0][0] = cosine;
        matrixReturn[0][1] = -sine;
        matrixReturn[0][2] = 0.;
        matrixReturn[0][3] = 0.;

        matrixReturn[1][0] = sine;
        matrixReturn[1][1] = cosine;
        matrixReturn[1][2] = 0.;
        matrixReturn[1][3] = 0.;

        matrixReturn[2][0] = 0.;
        matrixReturn[2][1] = 0.;
        matrixReturn[2][2] = 1.;
        matrixReturn[2][3] = 0.;

        matrixReturn[3][0] = 0.;
        matrixReturn[3][1] = 0.;
        matrixReturn[3][2] = 0.;
        matrixReturn[3][3] = 1.;
        break;

    default:
        return (1); /* error - invalid axis specifier */
    }

    return (Success);
}

/*+
 * NAME:
 * 	PEXArbRotationMatrix
 *
 * FORMAT:
 * 	`PEXArbRotationMatrix`
 *
 * ARGUMENTS:
 *	pt1		First endpoint of line segment defining the
 *			axis of rotation.
 *
 *	pt2		Second endpoint of line segment that defining the
 *			axis of rotation.
 *
 *	angle		The rotation angle, in radians.
 *
 *	matrixReturn	Returns the resulting rotation matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine formats a matrix for a right-handed rotation about an
 *	arbitrary axis.  The rotation axis is defined by the line segment
 *	joining `pt1' and `pt2'.  `Angle' is in radians and is measured
 *	counter-clockwise when looking from `pt1' to `pt2' along the
 *	specified axis.  The formatted matrix is returned in `matrixReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXRotationMatrix", `PEXScalingMatrix", `PEXTranslationMatrix",
 *	`PEXMultiplyMatrices", `PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

void
PEXArbRotationMatrix (pt1, pt2, angle, matrixReturn)

INPUT pxlCoord3D	*pt1;
INPUT pxlCoord3D	*pt2;
INPUT float		angle;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    pxlMatrix	preMatrix;
    pxlMatrix	calcMatrix;
    pxlMatrix	postMatrix;
    pxlMatrix	tempMatrix;
    pxlCoord3D	diff;
    pxlCoord3D	rot;
    float	dist;
    float	temp;
    float	s;
    double	ang;
    double	sine;
    double	cosine;
    int		i;
    int		j;

    /* The matrix is calculated as

       preMatrix * calcMatrix * postMatrix

    where postMatrix translates by pt1 and preMatrix translates back by pt1
    and calcMatrix does the real work. */


    /* Set up preMatrix */

    preMatrix[0][0] = 1.;
    preMatrix[0][1] = 0.;
    preMatrix[0][2] = 0.;
    preMatrix[0][3] = (pt1->x);

    preMatrix[1][0] = 0.;
    preMatrix[1][1] = 1.;
    preMatrix[1][2] = 0.;
    preMatrix[1][3] = (pt1->y);

    preMatrix[2][0] = 0.;
    preMatrix[2][1] = 0.;
    preMatrix[2][2] = 1.;
    preMatrix[2][3] = (pt1->z);

    preMatrix[3][0] = 0.;
    preMatrix[3][1] = 0.;
    preMatrix[3][2] = 0.;
    preMatrix[3][3] = 1.;


    /* Set up postMatrix */

    postMatrix[0][0] = 1.;
    postMatrix[0][1] = 0.;
    postMatrix[0][2] = 0.;
    postMatrix[0][3] = -(pt1->x);

    postMatrix[1][0] = 0.;
    postMatrix[1][1] = 1.;
    postMatrix[1][2] = 0.;
    postMatrix[1][3] = -(pt1->y);

    postMatrix[2][0] = 0.;
    postMatrix[2][1] = 0.;
    postMatrix[2][2] = 1.;
    postMatrix[2][3] = -(pt1->z);

    postMatrix[3][0] = 0.;
    postMatrix[3][1] = 0.;
    postMatrix[3][2] = 0.;
    postMatrix[3][3] = 1.;


    /* Compute calcMatrix */

    /* difference between the two points */
    diff.x = pt2->x - pt1->x;
    diff.y = pt2->y - pt1->y;
    diff.z = pt2->z - pt1->z;

    /* distance between two points */
    dist = sqrt (diff.x*diff.x + diff.y*diff.y + diff.z*diff.z);

    /* normalized rotation vector */
    rot.x = diff.x = diff.x/dist;
    rot.y = diff.y = diff.y/dist;
    rot.z = diff.z = diff.z/dist;

    /* square it */
    rot.x = rot.x*rot.x;
    rot.y = rot.y*rot.y;
    rot.z = rot.z*rot.z;

    ang = angle;
    cosine = cos(ang);
    sine = sin(ang);

    calcMatrix[0][0] = rot.x + cosine*(1.-rot.x);
    calcMatrix[1][1] = rot.y + cosine*(1.-rot.y);
    calcMatrix[2][2] = rot.z + cosine*(1.-rot.z);

    temp = diff.x*diff.y * (1. - cosine);
    s = sine * diff.z;

    calcMatrix[1][0] = temp - s;
    calcMatrix[0][1] = temp + s;

    temp = diff.x*diff.z * (1. - cosine);
    s = sine * diff.y;

    calcMatrix[2][0] = temp + s;
    calcMatrix[0][2] = temp - s;

    temp = diff.y*diff.z * (1. - cosine);
    s = sine * diff.x;

    calcMatrix[2][1] = temp - s;
    calcMatrix[1][2] = temp + s;

    calcMatrix[0][3] = 0.;
    calcMatrix[1][3] = 0.;
    calcMatrix[2][3] = 0.;
    calcMatrix[3][0] = 0.;
    calcMatrix[3][1] = 0.;
    calcMatrix[3][2] = 0.;
    calcMatrix[3][3] = 1.;


    /* Multiply preMatrix by calcMatrix */

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            tempMatrix[i][j] = preMatrix[i][0]*calcMatrix[0][j]
                             + preMatrix[i][1]*calcMatrix[1][j]
                             + preMatrix[i][2]*calcMatrix[2][j]
                             + preMatrix[i][3]*calcMatrix[3][j];
        }
    }


    /* Multiply by postMatrix and return the new matrix */

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            matrixReturn[i][j] = tempMatrix[i][0]*postMatrix[0][j]
                               + tempMatrix[i][1]*postMatrix[1][j]
                               + tempMatrix[i][2]*postMatrix[2][j]
                               + tempMatrix[i][3]*postMatrix[3][j];
        }
    }

    return;
}

/*+
 * NAME:
 * 	PEXScalingMatrix
 *
 * FORMAT:
 * 	`PEXScalingMatrix`
 *
 * ARGUMENTS:
 *	sx		Scale factor in the x direction.
 *
 *	sy		Scale factor in the y direction.
 *
 *	sz		Scale factor in the z direction.
 *
 *	matrixReturn	Returns the resulting scaling matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine formats a matrix for scaling x, y, and z values by
 *	`sx', `sy', and `sz', respectively.  The formatted matrix is returned
 *	in `matrixReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXRotationMatrix", `PEXArbRotationMatrix", `PEXTranslationMatrix",
 *	`PEXMultiplyMatrices", `PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

void
PEXScalingMatrix (sx, sy, sz, matrixReturn)

INPUT float		sx;
INPUT float		sy;
INPUT float		sz;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    matrixReturn[0][0] = sx;
    matrixReturn[0][1] = 0.;
    matrixReturn[0][2] = 0.;
    matrixReturn[0][3] = 0.;

    matrixReturn[1][0] = 0.;
    matrixReturn[1][1] = sy;
    matrixReturn[1][2] = 0.;
    matrixReturn[1][3] = 0.;

    matrixReturn[2][0] = 0.;
    matrixReturn[2][1] = 0.;
    matrixReturn[2][2] = sz;
    matrixReturn[2][3] = 0.;

    matrixReturn[3][0] = 0.;
    matrixReturn[3][1] = 0.;
    matrixReturn[3][2] = 0.;
    matrixReturn[3][3] = 1.;

    return;
}

/*+
 * NAME:
 * 	PEXTranslationMatrix
 *
 * FORMAT:
 * 	`PEXTranslationMatrix`
 *
 * ARGUMENTS:
 *	tx		Translation factor in the x direction.
 *
 *	ty		Translation factor in the y direction.
 *
 *	tz		Translation factor in the z direction.
 *
 *	matrixReturn	Returns the resulting translation matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *  	This routine formats a matrix for translating x, y, and z values by
 *	`tx', `ty', and `tz', respectively.  The formatted matrix is returned
 *	in `matrixReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXRotationMatrix", `PEXArbRotationMatrix", `PEXScalingMatrix",
 *	`PEXMultiplyMatrices", `PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

void
PEXTranslationMatrix (tx, ty, tz, matrixReturn)

INPUT float		tx;
INPUT float		ty;
INPUT float		tz;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    matrixReturn[0][0] = 1.;
    matrixReturn[0][1] = 0.;
    matrixReturn[0][2] = 0.;
    matrixReturn[0][3] = tx;

    matrixReturn[1][0] = 0.;
    matrixReturn[1][1] = 1.;
    matrixReturn[1][2] = 0.;
    matrixReturn[1][3] = ty;

    matrixReturn[2][0] = 0.;
    matrixReturn[2][1] = 0.;
    matrixReturn[2][2] = 1.;
    matrixReturn[2][3] = tz;

    matrixReturn[3][0] = 0.;
    matrixReturn[3][1] = 0.;
    matrixReturn[3][2] = 0.;
    matrixReturn[3][3] = 1.;

    return;
}

/*+
 * NAME:
 * 	PEXCopyMatrix
 *
 * FORMAT:
 * 	`PEXCopyMatrix`
 *
 * ARGUMENTS:
 *	matrix		Source matrix.
 *
 *	matrixReturn	Destination matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine copies `matrix' to `matrixReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *
 */

void
PEXCopyMatrix (matrix, matrixReturn)

INPUT pxlMatrix		matrix;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    int		i;
    int		j;

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            matrixReturn[i][j] = matrix[i][j];
        }
    }

    return;
}

/*+
 * NAME:
 * 	PEXMultiplyMatrices
 *
 * FORMAT:
 * 	`PEXMultiplyMatrices`
 *
 * ARGUMENTS:
 *	mat1		Matrix to be multiplied.
 *
 *	mat2		Matrix to multiply `mat1' by.
 *
 *	matrixReturn	Returns the result of the matrix multiplication.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 *	This routine multiplies `mat1' by `mat2' and returns the result in
 *	`matrixReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXRotationMatrix", `PEXArbRotationMatrix", `PEXScalingMatrix",
 *	`PEXTranslationMatrix"
 *
 */

void
PEXMultiplyMatrices (mat1, mat2, matrixReturn)

INPUT pxlMatrix		mat1;
INPUT pxlMatrix		mat2;
OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    pxlMatrix	tempMatrix; /* in case one of input matrices is output, too */
    int		i;
    int		j;

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            tempMatrix[i][j] = mat1[i][0]*mat2[0][j] + mat1[i][1]*mat2[1][j]
                             + mat1[i][2]*mat2[2][j] + mat1[i][3]*mat2[3][j];
        }
    }

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            matrixReturn[i][j] = tempMatrix[i][j];
        }
    }

    return;
}

/*+
 * NAME:
 * 	PEXIdentityMatrix
 *
 * FORMAT:
 * 	`PEXIdentityMatrix`
 *
 * ARGUMENTS:
 *	matrixReturn		Matrix that is set to the identity matrix.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine sets matrix `matrixReturn' to the identity matrix.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

void
PEXIdentityMatrix (matrixReturn)

OUTPUT pxlMatrix	matrixReturn;

{
/* AUTHOR:  Sally C. Barry */
    matrixReturn[0][0] = 1.;
    matrixReturn[0][1] = 0.;
    matrixReturn[0][2] = 0.;
    matrixReturn[0][3] = 0.;

    matrixReturn[1][0] = 0.;
    matrixReturn[1][1] = 1.;
    matrixReturn[1][2] = 0.;
    matrixReturn[1][3] = 0.;

    matrixReturn[2][0] = 0.;
    matrixReturn[2][1] = 0.;
    matrixReturn[2][2] = 1.;
    matrixReturn[2][3] = 0.;

    matrixReturn[3][0] = 0.;
    matrixReturn[3][1] = 0.;
    matrixReturn[3][2] = 0.;
    matrixReturn[3][3] = 1.;

    return;
}

/*+
 * NAME:
 * 	PEXInvertMatrix
 *
 * FORMAT:
 * 	`PEXInvertMatrix`
 *
 * ARGUMENTS:
 *	matrix			Matrix that is inverted.
 *
 *	inverseReturn		Returns the computed inverse of `matrix'.
 *
 * RETURNS:
 *	Zero if the matrix has a computable inverse, non-zero otherwise.
 *
 * DESCRIPTION:
 *	This routine computes the inverse of the matrix `matrix' and returns
 *	it in `inverseReturn'.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *
 */

Status
PEXInvertMatrix (matrix, inverseReturn)

INPUT pxlMatrix		matrix;
OUTPUT pxlMatrix	inverseReturn;

{
/* AUTHOR:  Sally C. Barry */
    int		h;
    int		i;
    int		j;
    int		k;
    int		ipivot;
    float	aug[4][5];
    float	pivot;
    float	temp;
    float	q;

    /* This routine calculates a 4x4 inverse matrix.  It uses Gaussian
    elimination on the system [matrix][inverse] = [identity].  The values of
    matrix then become the coefficients of the system of equations that evolves
    from this equation.  The system is then solved for the values of [inverse].
    The algorithm solves for each column of [inverse] in turn.  Partial
    pivoting is also done to reduce the numerical error involved in the
    computations.  If singularity is detected, the routine ends and returns
    an error.
    (See Numerical Analysis, L.W. Johnson and R.D.Riess, 1982) */

    for (h=0; h<4; h++)   /* solve column by column */
    {
        /* set up the augmented matrix for [matrix][inverse] = [identity] */

        for (i=0; i<4; i++)
        {
            aug[i][0] = matrix[i][0];
            aug[i][1] = matrix[i][1];
            aug[i][2] = matrix[i][2];
            aug[i][3] = matrix[i][3];
            aug[i][4] = (h == i) ? 1. : 0.;
        }

        /* search for the largest entry in column i, rows i through 3.
           ipivot is the row index of the largest entry. */

        for (i=0; i<3; i++)
        {
            pivot = 0.;

            for (j=i; j<4; j++)
            {
                temp = ABS(aug[j][i]);
                if (pivot < temp)
                {
                    pivot = temp;
                    ipivot = j;
                }
            }
            if (pivot == 0.) /* singularity check */
                {return (1);}

            /* interchange rows i and ipivot */

            if (ipivot != i)
            {
                for (k=i; k<5; k++)
                {
                    temp = aug[i][k];
                    aug[i][k] = aug[ipivot][k];
                    aug[ipivot][k] = temp;
                }
            }

            /* put augmented matrix in echelon form */

            for (k=i+1; k<4; k++)
            {
                q = -aug[k][i] / aug[i][i];
                aug[k][i] = 0.;
                for (j=i+1; j<5; j++)
                {
                    aug[k][j] = q * aug[i][j] + aug[k][j];
                }
            }
        }

        if (aug[3][3] == 0.)   /* singularity check */
            {return (1);}

        /* backsolve to obtain values for inverse matrix */

        inverseReturn[3][h] = aug[3][4] / aug[3][3];

        for (k=1; k<4; k++)
        {
            q = 0.;
            for (j=1; j<=k; j++)
            {
                q = q + aug[3-k][4-j] * inverseReturn[4-j][h];
            }
            inverseReturn[3-k][h] = (aug[3-k][4] - q) / aug[3-k][3-k];
        }
    }

    return (Success);
}

/*+
 * NAME:
 * 	PEXTransform3dPoints
 *
 * FORMAT:
 * 	`PEXTransform3dPoints`
 *
 * ARGUMENTS:
 *	mat		Transformation matrix.
 *
 *	pts		List of points to be transformed.
 *
 *	numPts		Number of points in `pts' list.
 *
 *	ptsReturn	Returns transformed points.
 *
 * RETURNS:
 *	Zero if all points were transformed correctly, non-zero if
 *	any of the points could not correctly be transformed.
 *
 * DESCRIPTION:
 *	This routine transforms the non-homogeneous points in `pts' using the
 *	transformation matrix `mat'.  `NumPts' indicates the number of points
 *	in the list.
 *	The homogeneous coordinate value, called `w', for each input point
 *	is assumed to be 1.0.
 *	The points are considered to be column vectors.  The transformed
 *	point `ptsReturn'[`i'] is computed by multiplying the matrix `mat'
 *	by the input point (column vector) `pts'[`i'].  In other words,
 *	`ptsReturn'[`i'] = `mat' x `pts'[`i'].
 *
 *	If any point could not be transformed because multiplying it
 *	by `mat' would have resulted in a division by zero when it was divided
 *	by its new `w' component, an error is returned.  In that case, all
 *	points that can be correctly transformed will be, but the incorrectly
 *	transformed points are set to (0, 0, 0).
 *
 *	Due to differences in
 *	floating point representations, note that the
 *	values computed using this utility routine may not
 *	be exactly equal to the values that would be computed by
 *	a specific implementation of the PEX server extension.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXTransform4dPoints"
 *
 */

Status
PEXTransform3dPoints (mat, pts, numPts, ptsReturn)

INPUT pxlMatrix		mat;
INPUT pxlCoord3D	*pts;
INPUT int		numPts;
OUTPUT pxlCoord3D	*ptsReturn;

{
/* AUTHOR:  Sally C. Barry */
    int		i;
    float	w;
    int		status;
    pxlCoord3D	point;

    status = Success;

    for (i=0; i<numPts; i++)
    {
	w = mat[3][0]*pts[i].x + mat[3][1]*pts[i].y
	  + mat[3][2]*pts[i].z + mat[3][3];

	if (w == 0.)
	{
	    ptsReturn[i].x = 0.;
	    ptsReturn[i].y = 0.;
	    ptsReturn[i].z = 0.;

	    status = 1;   /* return an error */
	}
	else
	{
            point.x = (mat[0][0]*pts[i].x + mat[0][1]*pts[i].y
                     + mat[0][2]*pts[i].z + mat[0][3]) / w;

            point.y = (mat[1][0]*pts[i].x + mat[1][1]*pts[i].y
                     + mat[1][2]*pts[i].z + mat[1][3]) / w;

            point.z = (mat[2][0]*pts[i].x + mat[2][1]*pts[i].y
                     + mat[2][2]*pts[i].z + mat[2][3]) / w;

	    ptsReturn[i] = point;
	}
    }

    return (status);
}

/*+
 * NAME:
 * 	PEXTransform4dPoints
 *
 * FORMAT:
 * 	`PEXTransform4dPoints`
 *
 * ARGUMENTS:
 *	mat		Transformation matrix.
 *
 *	pts		List of points to be transformed.
 *
 *	numPts		Number of points in `pts' list.
 *
 *	ptsReturn	Returns transformed points.
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine transforms the homogeneous points in `pts' using the
 *	transformation matrix `mat'.  `NumPts' indicates the number of
 *	points in the list.
 *	The points are considered to be column vectors.  The transformed
 *	point `ptsReturn'[`i'] is computed by multiplying the matrix `mat'
 *	by the input point (column vector) `pts'[`i'].  In other words,
 *	`ptsReturn'[`i'] = `mat' x `pts'[`i'].
 *
 *	Due to differences in
 *	floating point representations, note that the
 *	values computed using this utility routine may not
 *	be exactly equal to the values that would be computed by
 *	a specific implementation of the PEX server extension.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXTransform3dPoints"
 *
 */

void
PEXTransform4dPoints (mat, pts, numPts, ptsReturn)

INPUT pxlMatrix		mat;
INPUT pxlCoord4D	*pts;
INPUT int		numPts;
OUTPUT pxlCoord4D	*ptsReturn;

{
/* AUTHOR:  Sally C. Barry */
    int		i;
    pxlCoord4D	point;

    for (i=0; i<numPts; i++)
    {
        point.x = mat[0][0]*pts[i].x + mat[0][1]*pts[i].y
                + mat[0][2]*pts[i].z + mat[0][3]*pts[i].w;

        point.y = mat[1][0]*pts[i].x + mat[1][1]*pts[i].y
                + mat[1][2]*pts[i].z + mat[1][3]*pts[i].w;

        point.z = mat[2][0]*pts[i].x + mat[2][1]*pts[i].y
                + mat[2][2]*pts[i].z + mat[2][3]*pts[i].w;

        point.w = mat[3][0]*pts[i].x + mat[3][1]*pts[i].y
                + mat[3][2]*pts[i].z + mat[3][3]*pts[i].w;

	ptsReturn[i] = point;
    }

    return;
}

/*+
 * NAME:
 * 	PEXLookatViewMatrix
 *
 * FORMAT:
 * 	`PEXLookatViewMatrix`
 *
 * ARGUMENTS:
 *	from		Viewing position.
 *
 *	to		Look at position.
 *
 *	up		Vector representing the "up" direction.
 *
 *	matReturn	Returns computed "look at" view transformation matrix.
 *
 * RETURNS:
 *	Zero if the matrix was computed, non-zero if the `from' and `to'
 *	positions are equal.
 *
 * DESCRIPTION:
 * 	This routine formats a "look at" view orientation matrix.
 *
 *	The `from' position defines the viewpoint, and the `to' position
 *	specifies the point being viewed.  The orientation of the view is
 *	determined by the `up' vector, which specifies the up direction in
 *	view reference coordinates.
 *
 *	The matrix created by this routine translates the point specified by
 *	`to' to the origin, aligns the viewpoint with the +`z' axis, and aligns
 *	the up direction with the +`y' axis.  If the up vector is zero or is
 *	colinear with the `to' and `from' positions, the up direction is
 *	assumed to be in the direction of the +`y' axis.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXPolarViewMatrix",
 *	`PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

Status
PEXLookatViewMatrix (from, to, up, matReturn)

INPUT pxlCoord3D	*from;
INPUT pxlCoord3D	*to;
INPUT pxlVector3D	*up;
OUTPUT pxlMatrix	matReturn;

{
/* AUTHOR:  Sally C. Barry */
    pxlCoord3D		a;
    pxlCoord3D		b;
    pxlCoord3D		c;
    pxlCoord3D		d;
    pxlCoord3D		e;
    pxlCoord3D		f;
    pxlCoord3D		t;
    float		magnitude;
    float		distance;
    float		dot_product;
    int			i;
    pxlMatrix		calcMatrix;

    /* This matrix can be thought of as having 2 parts.  The first part (next
       to the coordinate point when it is being transformed) moves the to
       point to the origin.  The second part performs the rotation of the data.

       The three basis vectors of the rotation are obtained as follows.
       The Z basis vector is determined by subtracting from from to and
       dividing by its length (b).  The Y basis vector is determined by
       calculating the vector perpendicular to b and in the plane defined by
       the to and from points and the up vector and then normalizing it (e).
       The X basis vector (f) is calculated by e CROSS b.

       The resulting matrix looks like this:

	| fx fy fz 0 | | 1 0 0 -tox | | fx fy fz tz |
	| ex ey ez 0 |*| 0 1 0 -toy |=| ex ey ez ty |
	| bx by bz 0 | | 0 0 1 -toz | | bx by bz tz |
	|  0  0  0 1 | | 0 0 0   1  | |  0  0  0  1 |

       where tx = -to DOT f, ty = -to DOT e, and tz = -to DOT b. */

    /* Calculate the rotations */

    /* difference between to and from */
    a.x = from->x - to->x;
    a.y = from->y - to->y;
    a.z = from->z - to->z;

    magnitude = sqrt (a.x*a.x + a.y*a.y + a.z*a.z);

    if (magnitude == 0.)
        {return (1);}   /* from and to are the same */

    distance = magnitude;

    /* normalize the from-to vector */
    b.x = a.x / magnitude;
    b.y = a.y / magnitude;
    b.z = a.z / magnitude;

    /* up is second basis vector */
    c.x = up->x;
    c.y = up->y;
    c.z = up->z;

    /* compute the dot product of the previous two vectors */
    dot_product = (c.x*b.x) + (c.y*b.y) + (c.z*b.z);

    /* calculate the vector perpendicular to the up vector and in the
       plane defined by the to and from points and the up vector. */
    d.x = c.x - (dot_product * b.x);
    d.y = c.y - (dot_product * b.y);
    d.z = c.z - (dot_product * b.z);

    magnitude = sqrt (d.x*d.x + d.y*d.y + d.z*d.z);

    if (magnitude == 0.)   /* use the defaults */
    {
        c.x = 0.;
        c.y = 1.;
        c.z = 0.;

        dot_product = b.y;

        d.x = - (dot_product*b.x);
        d.y = 1. - (dot_product*b.y);
        d.z = - (dot_product*b.z);

        magnitude = sqrt (d.x*d.x + d.y*d.y + d.z*d.z);

        if (magnitude == 0.)
        {
            c.x = 0.;
            c.y = 0.;
            c.z = 1.;

            dot_product = b.z;

            d.x = - (dot_product*b.x);
            d.y = - (dot_product*b.y);
            d.z = 1. - (dot_product*b.z);

            magnitude = sqrt (d.x*d.x + d.y*d.y + d.z*d.z);
        }
    }

    /* calculate the unit vector in the from, to, and at plane and perpendicular
    to the up vector */
    e.x = d.x / magnitude;
    e.y = d.y / magnitude;
    e.z = d.z / magnitude;

    /* calculate the unit vector perpendicular to the other two by crossing
    them */
    f.x = (e.y*b.z) - (b.y*e.z);
    f.y = (e.z*b.x) - (b.z*e.x);
    f.z = (e.x*b.y) - (b.x*e.y);

    /* fill in the rotation values */
    matReturn[0][0] = f.x;
    matReturn[0][1] = f.y;
    matReturn[0][2] = f.z;

    matReturn[1][0] = e.x;
    matReturn[1][1] = e.y;
    matReturn[1][2] = e.z;

    matReturn[2][0] = b.x;
    matReturn[2][1] = b.y;
    matReturn[2][2] = b.z;

    /* calculate the translation part of the matrix */
    t.x = (-to->x*f.x) + (-to->y*f.y) + (-to->z*f.z);
    t.y = (-to->x*e.x) + (-to->y*e.y) + (-to->z*e.z);
    t.z = (-to->x*b.x) + (-to->y*b.y) + (-to->z*b.z);

    matReturn[0][3] = t.x;
    matReturn[1][3] = t.y;
    matReturn[2][3] = t.z;

    /* and fill in the rest of the matrix */
    matReturn[3][0] = 0.;
    matReturn[3][1] = 0.;
    matReturn[3][2] = 0.;
    matReturn[3][3] = 1.;

    return (Success);
}

/*+
 * NAME:
 * 	PEXPolarViewMatrix
 *
 * FORMAT:
 * 	`PEXPolarViewMatrix`
 *
 * ARGUMENTS:
 *	from		The viewing position.
 *
 *	distance	The distance between the `from' position and the
 *			position being viewed.
 *
 *	azimuth		The angle in the `x',`z' plane from the +`z' axis to the
 *			line of sight, in radians.  Positive values are
 *			counter-clockwise when viewed from the positive `y'
 *			axis.
 *
 *	altitude        The angular inclination of the line of sight from the
 *			`x',`z' plane.  The `altitude' argument is the angle
 *			in radians.  Positive values are towards the positive
 *			`y' axis.
 *
 *	twist		The up direction of the viewing, in radians.  Positive
 *			values of the twist angle twist in a counter-clockwise
 *			direction.
 *
 *	matReturn	Returns computed polar view transformation matrix.
 *
 * RETURNS:
 *	Zero if the matrix was computed, non-zero if `distance' is less than or
 *	equal to zero.
 *
 * DESCRIPTION:
 * 	This routine formats a polar view orientation matrix.  This routine is
 *	similar to `PEXLookatViewMatrix", except that the viewing parameters
 *	are specified in spherical coordinates.
 *
 *	The view is defined with respect to a point, `from', (the
 *	viewing position) and the distance between it and the position being
 *	viewed.  The `azimuth' angle specifies the direction of the line of
 *	sight going toward the position being viewed.
 *	Positive values of `azimuth' are counter-clockwise when viewed from the
 *	positive `y' axis.
 *
 *	The `azimuth' and `altitude' angles apply to the coordinate system
 *	with `from' at the origin and the line of sight emanating from it.  The
 *	`azimuth' argument specifies the angle between the line of sight and
 *	the +`z' axis, and the `altitude' argument defines the angle between
 *	it and the `x',`z' plane.
 *
 *	When applied, the transformation places the viewing position at the
 *	origin, aligns the viewpoint with the +`z' axis, applies any
 *	twist to the coordinates, and then places the viewed point at the
 *	origin.
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXLookatViewMatrix",  `PEXSetGlobalTransform", `PEXSetLocalTransform"
 *
 */

Status
PEXPolarViewMatrix (from, distance, azimuth, altitude, twist, matReturn)

INPUT pxlCoord3D	*from;
INPUT float		distance;
INPUT float		azimuth;
INPUT float		altitude;
INPUT float		twist;
OUTPUT pxlMatrix	matReturn;

{
/* AUTHOR:  Sally C. Barry */
    pxlCoord3D	trans;
    float	cost;
    float	sint;
    float	cosaz;
    float	sinaz;
    float	cosalt;
    float	sinalt;

    if (distance <= 0.)
        {return (1);}   /* from and to are the same */

    cost = cos (twist);
    sint = sin (twist);
    cosaz = cos (-azimuth);
    sinaz = sin (-azimuth);
    cosalt = cos (-altitude);
    sinalt = sin (-altitude);

    /* This matrix can be thought of as having five parts.  The first part
       (the part nearest the point to be transformed) translates the from point
       to the origin.  The last part translates the rotated data back by
       distance so that the to point is at the origin.  (Since the data is
       lined up along the Z axis, there are no X and Y parts to this
       translation.)

       The three parts in the middle rotate around the Y axis by azimuth,
       rotate around the X axis by altitude, and rotate around the Z axis by
       twist.

       The matrix calculated in this routine is the result of:

       (trans, -distance)*(twist, Z)*(altitude, X)*(azimuth, Y)*(trans, -from)

	   | cost -sint 0 0 | | 1    0       0   0 | | cosaz 0 sinaz 0 |
	Td*| sint  cost 0 0 |*| 0 cosalt -sinalt 0 |*|   0   1   0   0 |*Tfrom
	   |  0     0   1 0 | | 0 sinalt  cosalt 0 | |-sinaz 0 cosaz 0 |
	   |  0     0   0 1 | | 0    0       0   1 | |   0   0   0   1 |
       */

    matReturn[0][0] = cost * cosaz + (-sint) * (-sinalt) * (-sinaz);
    matReturn[0][1] = (-sint) * cosalt;
    matReturn[0][2] = cost * sinaz + (-sint) * (-sinalt) * cosaz;

    matReturn[1][0] = sint * cosaz + cost * (-sinalt) * (-sinaz);
    matReturn[1][1] = cost * cosalt;
    matReturn[1][2] = sint * sinaz + cost * (-sinalt) * cosaz;

    matReturn[2][0] = cosalt * (-sinaz);
    matReturn[2][1] = sinalt;
    matReturn[2][2] = cosalt * cosaz;

    /* calculate the translation part of the matrix */
    trans.x = -from->x * matReturn[0][0] + -from->y * matReturn[0][1] +
	      -from->z * matReturn[0][2];
    trans.y = -from->x * matReturn[1][0] + -from->y * matReturn[1][1] +
	      -from->z * matReturn[1][2];
    trans.z = -from->x * matReturn[2][0] + -from->y * matReturn[2][1] +
	      -from->z * matReturn[2][2];

    matReturn[0][3] = trans.x;
    matReturn[1][3] = trans.y;
    matReturn[2][3] = trans.z + distance;

    /* finish filling in the matrix */
    matReturn[3][0] = 0.;
    matReturn[3][1] = 0.;
    matReturn[3][2] = 0.;
    matReturn[3][3] = 1.;

    return (Success);
}

/*+
 * NAME:
 * 	PEXOrthoProjMatrix
 *
 * FORMAT:
 * 	`PEXOrthoProjMatrix`
 *
 * ARGUMENTS:
 *	height		The height of the orthographic viewing box.
 *
 *	aspect		The aspect ratio (width/height) of the orthographic
 *			viewing box.
 *
 *	near		The distance to the near clipping plane.
 *
 *	far		The distance to the far clipping plane.
 *
 *	matReturn	Returns the computed orthographic projection matrix.
 *
 * RETURNS:
 *	Zero if the matrix was computed, non-zero if the viewing box depth,
 *	width, or height is zero.
 *
 * DESCRIPTION:
 *	This routine formats a view mapping matrix.  The resulting matrix is
 *	returned in `matReturn'.
 *
 *	A projection matrix defines a visible region of the coordinate space.
 *	An orthographic projection defines the visible region as a box
 *	specified by its height (`height'), width (`height' x `aspect'), and
 *	its near and far boundaries (`near' and `far').
 *
 *	The reference point for the projection is the origin; the near and
 *	far clipping planes are defined with respect to it.  The height is
 *	defined in view reference coordinates.  Clipping at the
 *	planes is controlled by the clipping flags in the selected view table
 *	entry.
 *
 *	A projection transformation is typically used with a viewing
 *	transformation.  The viewing transformations defined by
 *	`PEXLookatViewMatrix" and `PEXPolarViewMatrix" place the point
 *	being viewed at the origin and align the line of sight along the
 *	`z' axis.
 *
 *	For example, if a unit cube is being viewed, a "look at"
 *	view matrix with the "to" point at the center
 *	of the cube, or a "polar" view
 *	matrix with the viewed point at the center of the
 *	cube, places the cube at the origin.  A matrix created by
 *	`PEXOrthoProjMatrix" with `height' = 1, `aspect' = 1, `near' = 0.5,
 *	and `far' = -0.5 makes the entire cube visible.
 *
 *	A projection transformation matrix with perspective foreshortening is
 *	created by `PEXPerspProjMatrix".
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXPerspProjMatrix", `PEXCreateLookupTable"
 *
 */

Status
PEXOrthoProjMatrix (height, aspect, near, far, matReturn)

INPUT float		height;
INPUT float		aspect;
INPUT float		near;
INPUT float		far;
OUTPUT pxlMatrix	matReturn;

{
/* AUTHOR:  Sally C. Barry */
    float 	width;
    float	depth;

    width = height * aspect;
    depth = near - far;

    if ((depth == 0.) || (width == 0.) || (height == 0.))
    {
	return (1);    /* return an error */
    }

    /* This matrix maps the width, height, and depth to the range of zero to
       one in all three directions.  It also maps the z values to be
       in front of the origin.  The near plane is mapped to z = 1 and the
       far plane is mapped to z = 0.  It also translates the x and y coordinates
       over by .5 so that x=0 and y=0 is in the middle of the NPC space. */

    /* fill the values into the matrix */
    matReturn[0][0] = 1./width;
    matReturn[0][1] = 0.;
    matReturn[0][2] = 0.;
    matReturn[0][3] = .5;

    matReturn[1][0] = 0.;
    matReturn[1][1] = 1./height;
    matReturn[1][2] = 0.;
    matReturn[1][3] = .5;

    matReturn[2][0] = 0.;
    matReturn[2][1] = 0.;
    matReturn[2][2] = 1./depth;
    matReturn[2][3] = 1. - (near/depth);

    matReturn[3][0] = 0.;
    matReturn[3][1] = 0.;
    matReturn[3][2] = 0.;
    matReturn[3][3] = 1.;

    return (Success);
}

/*+
 * NAME:
 * 	PEXPerspProjMatrix
 *
 * FORMAT:
 * 	`PEXPerspProjMatrix`
 *
 * ARGUMENTS:
 *	fovy		Field of view (in radians) in the horizontal direction.
 *
 *	distance	The distance to the eyepoint.
 *
 *	aspect		The aspect ratio (width/height) of the perspective
 *			viewing frustum.
 *
 *	near		The distance to the near clipping plane.
 *
 *	far		The distance to the far clipping plane.
 *
 *	matReturn	Returns the computed perspective transformation matrix.
 *
 * RETURNS:
 *	Zero if the matrix was computed, non-zero if `near' <= `far',
 *	`fovy' = 0., `aspect' = 0, or `distance' <= `near'.
 *
 * DESCRIPTION:
 *	This routine formats a view mapping matrix.  The resulting matrix is
 *	returned in `matReturn'.
 *
 *	A projection matrix defines the visible region of the coordinate
 *	space.
 *	A perspective projection defines the visible region as a truncated
 *	pyramid or frustum.  The amount of perspective in the projection is
 *	specified by argument `fovy'.
 *	The perspective increases as the angle increases to
 *	a value of pi radians.
 *
 *	The distance between the eyepoint and the origin
 *	is specified by `distance'.  If the application
 *	program calls `PEXLookatViewMatrix" to calculate the view
 *	orientation matrix, the `distance' is typically the distance between
 *	the `from' and `to' points specified to that routine.
 *	If the application program calls `PEXPolarViewMatrix" to calculate the
 *	view orientation matrix, the `distance' is typically the
 *	`distance' specified to that routine.
 *
 *	The height of the frustum at the near clipping
 *	plane is determined by `fovy' and the distance to the near plane.
 *	The width of the frustum is determined from the aspect ratio.
 *
 *	The reference point for the projection is the origin; the near and far
 *	clipping planes are defined with respect to it.  Clipping at the
 *	planes is controlled by the clip flags in the selected view table
 *	entry.
 *
 *	A projection transformation is typically used with a viewing
 *	transformation.  The viewing transformations defined by
 *	`PEXLookatViewMatrix" and `PEXPolarViewMatrix" place the point
 *	being viewed at the origin and align the line of sight along the
 *	`z' axis.  The near and far clipping planes are defined with
 *	respect to the result of the viewing transformation.
 *
 *	It is useful to think of `PEXPerspProjMatrix" as defining a
 *	camera.  The object being viewed is defined near the
 *	origin.  The lens is defined by `fovy'; a larger value of `fovy' defines
 *	a wide angle lens.  For those who wish to keep the height at the
 *	near plane constant and automatically back up the camera to frame the
 *	subject, the relationship between `fovy', `eye_distance', which is the
 *	distance between the eyepoint and the near plane, and `height',
 *	which is the height at the near plane, is:
 *
 *	tan (`fovy'/2) = ( (`height'/2) / `eye_distance')
 *
 *	For example, if a unit cube is being viewed, a "look at" view with
 *	the "to" point at the center of the cube or a "polar"
 *	view with the viewed point
 *	at the center of the cube, places the cube
 *	at the origin.  A matrix created by `PEXPerspProjMatrix" with
 *	`aspect' = 1, `near' = 0.5, and `far' = -0.5 makes the
 *	entire cube visible, with `fovy' and `distance' controlling the amount
 *	of perspective applied.
 *
 *	An orthographic projection defines a visible region as a box.  An
 *	orthographic projection matrix is created by `PEXOrthoProjMatrix".
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXOrthoProjMatrix", `PEXCreateLookupTable"
 *
 */

Status
PEXPerspProjMatrix (fovy, distance, aspect, near, far, matReturn)

INPUT float		fovy;
INPUT float		distance;
INPUT float		aspect;
INPUT float		near;
INPUT float		far;
OUTPUT pxlMatrix	matReturn;

{
/* AUTHOR:  Sally C. Barry */
    double	fovy2;
    double	c_hy;
    double	s_hy;
    float	height;
    float	depth;
    float	width;
    float	eye_distance;

    /* check for errors */
    if ((near <= far) || (fovy == 0.) || (aspect == 0.) || (distance <= near))
    {
	return (1);   /* return an error */
    }

    /* limit the field of view to be less than pi (minus a little) and ensure
       that it's positive and then halve it */

    if ((fovy > 3.140) || (fovy < -3.140))
    {
	fovy2 = 3.140/2.;
    }
    else if (fovy < 0.)
    {
	fovy2 = -fovy/2.;
    }
    else
    {
	fovy2 = fovy/2.;
    }

    /* calculate some dimensions we need */

    c_hy = cos (fovy2);
    s_hy = sin (fovy2);

    eye_distance = distance - near;
    height = 2. * eye_distance * (s_hy/c_hy);
    depth = near - far;
    width = height * aspect;

    /* Thanks to Tony Yamamoto for figuring out how Steve McAllister of E&S
       formulated the Lynx perspective matrix and showing me how to transform
       it for PEX, a couple of different times. */

    /* This matrix is made up of three parts.  The first part is a perspective
       transformation matrix.  The second part is a matrix to scale and
       translate the coordinates so that z is between 0 and 1, x is
       between height/2 and -height/2 and y is between width/2 and
       -width/2.  The third part is a matrix to translate the eyepoint to
       .5 in x and y.

       The viewing frustum looks like this.  We want to project the point
       (x, y, z) onto the near plane to get (x', y', z').

  +Z             |      o (x, y, z)
 <==             |    / :
                 |  /   :
      (x',y',z') |/     :
                /|      :
              /  |      :
            <----|----------------
                 |      z
                 |
           eye  near

       By similar triangles, x'/eye_dist = x/(eye_dist+near-z)
                             y'/eye_dist = y/(eye_dist+near-z)
                             z' = near

       So the projection matrix, Mproj =

             | 1   0      0        0                |
             | 0   1      0        0                |
             | 0   0      0       near              |
             | 0   0 -1/eye_dist  1+(near/eye_dist) |

       (Row vectors are shown below for simplicity.  They're really column
       vectors.)

       It can be shown that:
	Mproj * (0, 0, near, 1) = (0, 0, near, 1)
	Mproj * (0, 0, far, 1)  = (0, 0, near, 1+near/eye_dist-far/eye_dist)

       Next, we want to find a matrix, Mst, such that the near plane is
       transformed to z=1 and the far plane is transformed to z=0.
	Mst * Mproj * (0, 0, near, 1) = (0, 0, 1, 1)
	Mst * Mproj * (0, 0, far, 1)  = (0, 0, 0, t)
                                          where t = (eye_dist-far+near)/eye_dist

       Using the equations just above, this means that
	Mst * (0, 0, near, 1) = (0, 0, 1, 1)
	Mst * (0, 0, near, t) = (0, 0, 0, t)

       Concentrating on the lower right-hand corner of Mst, this means
	Mst * | near near | = | 1 0 |
              |  1    t   |   | 1 t |

       Solving for Mst, we get
	Mst = | t/(far(t-1))  -1/(t-1) |
              |     0            1     |

       Multiplying and simplifying, we get
	Mst * Mproj = | 1   0       0               0         |
                      | 0   1       0               0         |
                      | 0   0    1/depth       -far/depth     |
                      | 0   0  -1/eye_dist  1 + near/eye_dist |
                                                            where depth=near-far

       Now scale X and Y so that they have a range of "width"
       in X and "height" in Y.   The resulting matrix is
	M2 = | 1/width      0        0              0         |
             |    0     1/height     0              0         |
             |    0         0    1/depth       -far/depth     |
             |    0         0  -1/eye_dist  1 + near/eye_dist |

       Last, we need to translate the results by .5 in X and Y so that the eye
       point is in the middle of NPC space.
	matReturn = | 1 0 0 .5 | * M2
                    | 0 1 0 .5 |
                    | 0 0 1  0 |
                    | 0 0 0  1 |

	= | 1/width      0    -1/(2*eye_dist) (1+near/eye_dist)/2 |
          |    0     1/height -1/(2*eye_dist) (1+near/eye_dist)/2 |
          |    0         0        1/depth          -far/depth     |
          |    0         0      -1/eye_dist     1+near/eye_dist   |


    /* fill the values into the matrix */
    matReturn[0][0] = 1./width;
    matReturn[0][1] = 0.;
    matReturn[0][2] = -1./(2.*eye_distance);
    matReturn[0][3] = (1. + (near/eye_distance))/2.;

    matReturn[1][0] = 0.;
    matReturn[1][1] = 1./height;
    matReturn[1][2] = -1./(2.*eye_distance);
    matReturn[1][3] = (1. + (near/eye_distance))/2.;

    matReturn[2][0] = 0.;
    matReturn[2][1] = 0.;
    matReturn[2][2] = 1./depth;
    matReturn[2][3] = -far/depth;

    matReturn[3][0] = 0.;
    matReturn[3][1] = 0.;
    matReturn[3][2] = -1./eye_distance;
    matReturn[3][3] = 1. + (near/eye_distance);

    return (Success);
}

/*+
 * NAME:
 * 	PEXComputeNormals
 *
 * FORMAT:
 * 	`PEXComputeNormals`
 *
 * ARGUMENTS:
 *	method
 *
 *	primType
 *
 *	prim
 *
 *	primReturn
 *
 * RETURNS:
 *	None
 *
 * DESCRIPTION:
 * 	This routine computes geometric normals for the primitive specified by
 *	`prim'.  The resulting primitive, with facet normals, is returned in
 *	`primReturn'.
 *	`PrimType' specifies the type of primitive for which normals are to
 *	be computed.  `PrimType' can be of type `Polygon', `ComplexPolygon',
 *	`TriangleStrip', or `QuadrilateralMesh' (defined
 *  	in the `PEXlib.h' include file).  `Method' can be either `Clockwise' or
 *	`CounterClockwise' and indicates the order that should be used
 *	to create vectors and form a cross product for the purpose of
 *	generating a geometric normal.
 *
 *	*** Note:  this routine is still being designed ***
 *
 *	(Issue: who allocates memory for primOut?  who frees it?)
 *
 * ERRORS:
 *	None
 *
 * SEE ALSO:
 *	`PEXMultiFillAreaData", `PEXMultiFillAreaSetData", 
 *	`PEXTriangleStrip", `PEXQuadMesh"
 *
 */

void
PEXComputeNormals (method, primType, prim, primReturn)

INPUT int		method;
INPUT int		primType;
INPUT char		*prim;
OUTPUT char		*primReturn;

{
/* AUTHOR: */
/* primReturn could be **primReturn if data is allocated by PEXlib. */

/* TBD */
    return;
}
