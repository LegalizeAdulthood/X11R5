/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/paint.ch,v 2.5 1991/09/12 19:46:55 bobg Exp $ */
/* $ACIS:paint.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/paint.ch,v $ */

#ifndef lint
static char *rcsidpaint_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/paint.ch,v 2.5 1991/09/12 19:46:55 bobg Exp $";
#endif /* lint */

/*  paint.H

	paint package

	Routines for reading and writing rasters in the Macpaint format

 */

#include <pixelimg.ih>

#define  paint_PROGRAMMERVERSION    1

#define BUFBITS (1<<17)

package paint  {

classprocedures:

	WriteRow(FILE *file, unsigned char *byteaddr, long nbits);
		/* write to 'file' the Macpaint representation of the
			 'nbits' bits starting at location 'byteaddr' */

	ReadRow(FILE *file,  unsigned char *row,  long length) returns long;
		/* Reads from 'file' the encoding of bytes to fill in 'row'.
		    Row will be truncated or padded (with WHITE)
		    to exactly 'length' bytes. */

	ReadImage(FILE *file, struct pixelimage *pix) returns long;
		/* Read a raster image from 'file' and put it in 'pix'
			return error code, as defined in dataobj.ih */

	WriteImage(FILE *file, struct pixelimage *pix, struct rectangle *sub);
		/* Write the subraster 'sub' image to 'file' from 'pix' */

};

