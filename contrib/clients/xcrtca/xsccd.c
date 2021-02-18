/*
 *
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * xyY colorspace to XCMS database conversion.
 */
#include <stdio.h>

#define R   3
#define C   3

typedef double matrix[R][C];
typedef double colorArray[256][3];
typedef double intensityArray[256];

matrix rgbtoxyz, xyztorgb, times;
colorArray redA, greenA, blueA;
intensityArray rediA, greeniA, blueiA;

#define OverRow(r) for (r = 0; r < R; r++)
#define OverCol(c) OverRow(c)
#define OverMatrix(r,c)	OverRow(r) OverCol (c)

diagonal (m)
    matrix  m;
{
    int	r, c;

    OverMatrix(r,c) {
	m[r][c] = 0;
    }
    OverRow(r) m[r][r] = 1;
}

copymatrix(s,d)
    matrix  s, d;
{
    int	r, c;

    OverMatrix(r,c) d[r][c] = s[r][c];
}

timesrow (m,r,v)
    matrix  m;
    int	    r;
    double  v;
{
    int	    c;

    OverCol(c)
	m[r][c] *= v;
}

timesaddrows(m,source,dest,v)
    matrix  m;
    int	    source,dest;
    double  v;
{
    int	    c;
    OverCol(c)
	m[dest][c] = m[dest][c] + m[source][c] * v;
}

timesmatrix(a,b,d)
    matrix  a,b,d;
{
    int	    dr, dc;
    int	    c;
    double  v;

    OverMatrix(dr,dc) {
	v = 0;
	OverCol(c)
	    v += a[dr][c] * b[c][dc];
	d[dr][dc] = v;
    }
}

printmatrix(m)
    matrix  m;
{
    int	r, c;
    OverRow (r) {
        printf("          ");
	OverCol(c)
	  if (c == 2)
	    printf ("%20.15lf", m[r][c]);
	  else
	    printf ("%20.15lf ", m[r][c]);
	printf ("\n");
    }
}

invert (m, i)
    matrix  m;
    matrix  i;
{
    matrix  t, q;
    int	    r, c;
    double  v;

    copymatrix(m,t);
    diagonal (i);
    timesmatrix(t,i,q);
    for (c = 0; c < C; c++) for (r = c+1; r < R; r++) {
	v = - t[r][c] / t[c][c];
	timesaddrows(t,c,r,v);
	timesaddrows(i,c,r,v);
	timesmatrix(t,i,q);
    }
    for (c = C-1; c >= 0; c--) for (r = c-1; r >= 0; r--) {
	v = -t[r][c] / t[c][c];
	timesaddrows(t,c,r,v);
	timesaddrows(i,c,r,v);
	timesmatrix(t,i,q);
    }
    for (r = 0; r < R; r++) {
	v = 1/t[r][r];
	timesrow(t,r,v);
	timesrow(i,r,v);
	timesmatrix(t,i,q);
    }
}

ifixup(intenA, i, desc)
  intensityArray intenA;
  int i;
  char *desc;
{
  if (intenA[i] > intenA[i+1]) {
    fprintf(stderr, "    lowering 0x%02x %s intensity from %f to %f\n",
	    i, desc, intenA[i], intenA[i+1]);
    intenA[i] = intenA[i+1];
  } else if (i && intenA[i] < intenA[i-1] && intenA[i-1] < intenA[i+1]) {
    fprintf(stderr, "    raising 0x%02x %s intensity from %f to %f\n",
	    i, desc, intenA[i], intenA[i-1]);
    intenA[i] = intenA[i-1];
  }
}

readNameAndModelAndFormat(name, model, format)
     char *name, *model, *format;
{
  scanf("NAME %[^\n]\n", name);
  scanf("MODEL %[^\n]\n", model);
  scanf("FORMAT %[^\n]", format);
}

readArray(a)
     colorArray a;
{
  int i, junk;
  for (i=0; i<256; i++)
    scanf("%d %lf;%lf;%lf", &junk, &a[i][0], &a[i][1], &a[i][2]);
}

main()
{
  int i;
  double scaledMaxRedx, scaledMaxRedy, scaledMaxRedY, scaledMaxGreenx,
        scaledMaxGreeny, scaledMaxGreenY, scaledMaxBluex, scaledMaxBluey,
        scaledMaxBlueY, whiteY;
  double temp;
  char name[80], model[80], format[8];

  readNameAndModelAndFormat(name, model, format);
  readArray(redA);
  readArray(greenA);
  readArray(blueA);

  if (strcmp(format, "xyY") == 0) {
    /*  create the RGBtoXYZ matrix:  For the 3 maximum xyYvalues for Red, Green,
     *  and Blue
     */
    whiteY = redA[255][2] + greenA[255][2] + blueA[255][2];
    scaledMaxRedx = redA[255][0];
    scaledMaxRedy = redA[255][1];
    scaledMaxRedY = redA[255][2] / whiteY;
    scaledMaxGreenx = greenA[255][0];
    scaledMaxGreeny = greenA[255][1];
    scaledMaxGreenY = greenA[255][2] / whiteY;
    scaledMaxBluex = blueA[255][0];
    scaledMaxBluey = blueA[255][1];
    scaledMaxBlueY = blueA[255][2] / whiteY;
  				/* create RGB to XYZ matrix */
    rgbtoxyz[0][0] = 
      scaledMaxRedx / scaledMaxRedy * scaledMaxRedY; /* red X */
    rgbtoxyz[0][1] = 
      scaledMaxGreenx / scaledMaxGreeny * scaledMaxGreenY; /* green X*/
    rgbtoxyz[0][2] = 
      scaledMaxBluex / scaledMaxBluey * scaledMaxBlueY; /* blue X */
    rgbtoxyz[1][0] = scaledMaxRedY;
    rgbtoxyz[1][1] = scaledMaxGreenY;
    rgbtoxyz[1][2] = scaledMaxBlueY;
    rgbtoxyz[2][0] = 
      (1 - scaledMaxRedx - scaledMaxRedy) / scaledMaxRedy * scaledMaxRedY;
    rgbtoxyz[2][1] = 
      (1 - scaledMaxGreenx - scaledMaxGreeny) / scaledMaxGreeny *scaledMaxGreenY;
    rgbtoxyz[2][2] = 
      (1 - scaledMaxBluex - scaledMaxBluey) / scaledMaxBluey * scaledMaxBlueY;
    /* compute the intensity arrays */
    for (i=0; i<=0xff; i++) {
      rediA[i] = redA[i][2] / redA[255][2];
      greeniA[i] = greenA[i][2] / greenA[255][2];
      blueiA[i] = blueA[i][2] / blueA[255][2];
    }
  } else if (strcmp(format, "XYZ") == 0) {
    /*
     * create the RGBtoXYZ matrix
     */
    whiteY = redA[255][1] + greenA[255][1] + blueA[255][1];
    rgbtoxyz[0][0] = redA[255][0] / whiteY;	/* red X */
    rgbtoxyz[0][1] = greenA[255][0] / whiteY;	/* green X */
    rgbtoxyz[0][2] = blueA[255][0] / whiteY;	/* blue X */
    rgbtoxyz[1][0] = redA[255][1] / whiteY;	/* red Y */
    rgbtoxyz[1][1] = greenA[255][1] / whiteY;	/* green Y */
    rgbtoxyz[1][2] = blueA[255][1] / whiteY;	/* blue Y */
    rgbtoxyz[2][0] = redA[255][2] / whiteY;	/* red Z */
    rgbtoxyz[2][1] = greenA[255][2] / whiteY;	/* green Z */
    rgbtoxyz[2][2] = blueA[255][2] / whiteY;	/* blue Z */

    /* compute the intensity arrays */
    for (i=0; i<=0xff; i++) {
      rediA[i] = redA[i][1] / redA[255][1];
      greeniA[i] = greenA[i][1] / greenA[255][1];
      blueiA[i] = blueA[i][1] / blueA[255][1];
    }
  } else {
    fprintf (stderr, "Invalid format %s\n", format);
    exit (1);
  }

  /* force a non-decreasing ramp */
  for (i=0xfe; i>=0; i--) {
    ifixup(rediA, i, "red");
    ifixup(greeniA, i, "green");
    ifixup(blueiA, i, "blue");
  }

  printf ("SCREENDATA_BEGIN        0.3\n");
  printf ("\n");
  printf ("    NAME                %s", name);
  printf ("\n");
  printf ("    PART_NUMBER         3");
  printf ("\n");
  printf ("    MODEL               %s", model);
  printf ("\n");
  printf ("    SCREEN_CLASS        VIDEO_RGB");
  printf ("\n");
  printf ("    REVISION            2.0");
  printf ("\n");
  printf ("\n");
  printf ("    COLORIMETRIC_BEGIN\n");
  printf ("        XYZtoRGB_MATRIX_BEGIN\n");
  invert(rgbtoxyz, xyztorgb); 
  printmatrix(xyztorgb);
  printf ("        XYZtoRGB_MATRIX_END\n");
  printf ("        RGBtoXYZ_MATRIX_BEGIN\n");
  printmatrix(rgbtoxyz);
  printf ("        RGBtoXYZ_MATRIX_END\n");
  printf ("    COLORIMETRIC_END\n");
  printf ("    INTENSITY_PROFILE_BEGIN 0 3\n");
  printf ("    INTENSITY_TBL_BEGIN    RED    256\n");
  for (i=0; i<=0xff; i++) {
    printf ("            0x%04x %20.18f\n", (i<<8)+i, rediA[i]);
  }
  printf ("    INTENSITY_TBL_END\n");
  printf ("    INTENSITY_TBL_BEGIN    GREEN    256\n");
  for (i=0; i<=0xff; i++)
    printf ("            0x%04x %20.18f\n", (i<<8)+i, greeniA[i]);
  printf ("    INTENSITY_TBL_END\n");
  printf ("    INTENSITY_TBL_BEGIN    BLUE    256\n");
  for (i=0; i<=0xff; i++)
    printf ("            0x%04x %20.18f\n", (i<<8)+i, blueiA[i]);
  printf ("    INTENSITY_TBL_END\n");
  printf ("    INTENSITY_PROFILE_END\n");
  printf ("\n");
  printf ("SCREENDATA_END");
}
