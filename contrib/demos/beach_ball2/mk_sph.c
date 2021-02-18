static char sccsid[] = "@(#)84	1.1  R2/PEX/clients/beach_ball/mk_sph.c, gos 11/14/90 19:24:52";
/* 
 * Phigs+ Demostration and Testing.
 *
 * Totally reorganised 11/9/89
 */

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <phigs/phigs.h>

#define BLACK		0
#define BLUE		1
#define GREEN		2
#define CYAN		3
#define RED  	  	4
#define MAGENTA		5
#define YELLOW		6
#define	WHITE		7

Ppoint3		lat[7][13];


read_points()
{
	FILE	*fp, *fopen();
	int	i, j;
	char	*directory, file[256];

	if( (directory = getenv("BBALLDATAPATH")) == NULL )
		strcpy(file, "bball.pts");
	else
		sprintf(file, "%s/%s", directory, "bball.pts");

	if( (fp = fopen(file, "r")) == NULL )
	{
		fprintf(stderr, "No file with sphere points\n");
		exit(-1);
	}

	for(i=0; i<7; i++)
		for(j=0; j<13; j++)
			fscanf(fp, "%f %f %f", &lat[i][j].x,
				&lat[i][j].y, &lat[i][j].z);
}



normal(pt, norm)
Ppoint3		*pt;
Pvec3		*norm;
{
	double	len;

	len = pt->x + pt->y + pt->z;
	norm->delta_x = pt->x / len;
	norm->delta_y = pt->y / len;
	norm->delta_z = pt->z / len;
}


my_build_sphere(colors_sw)
Pint	colors_sw;
{
	Pint		i, j, k;			/* for general	*/
	Pint		l1, l2;				/* for strips	*/
	Pfloat		r, x, y, z, pi_per_deg, ang;	/*  use		*/
	Pcolr_rep	color;				/* color rep.	*/
	Plight_src_bundle	lrep;			/* light source */
	Pint_list	act, deact;			/*  setting	*/
	char		buf[10];			/* terminal I/O */
	Prefl_props	aprop;				/* area property*/
	Pint		colr;

	Pmatrix3	mat;				/* modeling	*/
	Pvec3		shift, scale;			/*  transform.	*/
	Pfloat		xang, yang, zang;
	Ppoint3		fixed;

	Ptext_align	align;				/* text alignment */
	Ppoint		pos;

	Pview_rep3		vrep;				/* view reps. */
	Ppoint3		vrp;				/* view orientation */
	Pvec3		vpn, vup;

	Ppoint3		line[2];
	Ppoint3		tpt;
	Pvec3		tvec[2];

	Pint		err;

	/* TRIANGLE STRIP */

	Pint		fflag;
	Pint		vflag;
	Pint		num_pts;
	Pint		color_model;
	Pfacet_data_arr3	a_data[12];
	Pfacet_vdata_arr3	v_data[12];

	num_pts = 12;

	read_points();

	/* geometry of the triangle strip */

	for(i = 0 ; i < num_pts; i++) 
	{
		colr = (i % 7) + 1;

		v_data[i].ptconorms = (Pptconorm3 *) 
			malloc(num_pts * sizeof(Pptconorm3));

		v_data[i].ptconorms[0].point.x = lat[0][0].x;
		v_data[i].ptconorms[0].point.y = lat[0][0].y;
		v_data[i].ptconorms[0].point.z = lat[0][0].z;
		normal( &(v_data[i].ptconorms[0].point),
		        &(v_data[i].ptconorms[0].norm) );
		if(!colors_sw)
			v_data[i].ptconorms[0].colr.ind /* WARNING: may be index */= YELLOW;
		else
			v_data[i].ptconorms[0].colr.ind /* WARNING: may be index */= colr;

		for(j=1, k = 1; k<6; k++, j += 2)
		{
			v_data[i].ptconorms[j].point.x = lat[k][i+1].x;
			v_data[i].ptconorms[j].point.y = lat[k][i+1].y;
			v_data[i].ptconorms[j].point.z = lat[k][i+1].z;
			normal( &(v_data[i].ptconorms[j].point),
			        &(v_data[i].ptconorms[j].norm) );
			if(!colors_sw)
				v_data[i].ptconorms[j].colr.ind /* WARNING: may be index */= YELLOW;
			else
				v_data[i].ptconorms[j].colr.ind /* WARNING: may be index */= colr;

			v_data[i].ptconorms[j+1].point.x = lat[k][i].x;
			v_data[i].ptconorms[j+1].point.y = lat[k][i].y;
			v_data[i].ptconorms[j+1].point.z = lat[k][i].z;
			normal( &(v_data[i].ptconorms[j+1].point),
			        &(v_data[i].ptconorms[j+1].norm) );
			if(!colors_sw)
				v_data[i].ptconorms[j+1].colr.ind /* WARNING: may be index */= YELLOW;
			else
				v_data[i].ptconorms[j+1].colr.ind /* WARNING: may be index */= colr;

		}

		v_data[i].ptconorms[11].point.x = lat[6][0].x;
		v_data[i].ptconorms[11].point.y = lat[6][0].y;
		v_data[i].ptconorms[11].point.z = lat[6][0].z;
		normal( &(v_data[i].ptconorms[11].point),
		        &(v_data[i].ptconorms[11].norm) );
		if(!colors_sw)
			v_data[i].ptconorms[11].colr.ind /* WARNING: may be index */= YELLOW;
		else
			v_data[i].ptconorms[11].colr.ind /* WARNING: may be index */= colr;

	}

	vflag = PVERT_COORD_COLOUR_NORMAL;	/* per vertex flag */
	fflag = PFACET_COLOUR;			/* per facet flag */
	color_model = PINDIRECT;
						/* color type: for both
						 * vertex and facet */
	/* facet data for the strip */

	for(i=0; i<num_pts; i++)
	{
		colr = (i % 7) + 1;

		a_data[i].colrs = (Pcoval *) malloc((num_pts-2)*sizeof(Pcoval));
		for(j=0; j<num_pts-2; j++)
			if(!colors_sw)
				a_data[i].colrs[j].ind /* WARNING: may be index */= YELLOW;
			else
				a_data[i].colrs[j].ind /* WARNING: may be index */= colr;
	}

	/* END PRIMITIVE DEFINITION */


	shift.delta_x = 0.0; scale.delta_x = 1.0; xang = 0.0;
	shift.delta_y = 0.0; scale.delta_y = 1.0; yang = 0.0;
	shift.delta_z = 0.0; scale.delta_z = 1.0; zang = 0.0;
	fixed.x = 0.0;
	fixed.y = 0.0;
	fixed.z = 0.0;
	pbuild_tran_matrix3(&fixed, &shift, xang, yang, zang, &scale, &k, mat);

	pset_local_tran3(mat, PTYPE_REPLACE);	/* 2 - local trans */

        pset_hlhsr_id( PHIGS_HLHSR_MODE_ZBUFF );

	for(i=0; i<12; i++)
		ptri_strip3_data(fflag, vflag, color_model, num_pts, 
			&a_data[i], &v_data[i]);

} /* END MAIN */
