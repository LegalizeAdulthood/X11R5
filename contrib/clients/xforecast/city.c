/*
 * Copyright 1991 by Frank Adelstein.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the authors name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT 
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM 
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "types.h"

void
ReadInCities()
{

  char line[500];
  FILE *fp;

  if ((fp = fopen(CITYFILE, "r")) == NULL)
    {
      (void) fprintf (stderr, "Can't open city data file [%s]\n",CITYFILE);
      exit (3);
    }
  
  CityIndex = 0;

  while (fgets (line, 500, fp) != NULL)
    {
      if (line[0] != '\0' && line[0] != '#')
	/* skip blank lines and comments (lines beginning with #'s) */
	{
	  if (sscanf(line, "%d %d %s %s %[^\n]", 
		     &City[CityIndex].x, &City[CityIndex].y,
		     City[CityIndex].state, City[CityIndex].citycode, 
		     City[CityIndex].cityname)
	      != 5)
	    {
	      (void) fprintf (stderr, "Line %d malformed in city list\n",
			      CityIndex+1);
	    }
	  CityIndex++;
	}
    }
  (void) fclose (fp);
  return;
}

int
FindCity(x,y)
int x,y;

{
  int i;
  int best = -1;
  double biggest = 1000.0;
  double temp;

  for (i = 0; i < CityIndex; i++)
    {
      if (_ABS(x - City[i].x) < TOLERANCE &&
	  _ABS(y - City[i].y) < TOLERANCE)
	{
	  temp = _ABS(x - City[i].x)*_ABS(x - City[i].x) + 
	         _ABS(y - City[i].y)*_ABS(y - City[i].y);
	  if (temp < biggest)
	    {
	      biggest = temp;
	      best    = i;
	    }
	}
    }
  return (best);
}
