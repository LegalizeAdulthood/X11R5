/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/oldrf.ch,v 2.6 1991/09/12 19:46:51 bobg Exp $ */
/* $ACIS:oldrf.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/oldrf.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidoldRF_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/oldrf.ch,v 2.6 1991/09/12 19:46:51 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*  oldrf.H

	oldRF package

	Routines for reading and writing rasters in the old ITC format
	as defined by rastfile.h

	The data stream begins with a 14 or 16 byte header,
	which is followed by the bits of the data.

 */

#include <pixelimg.ih>

#define  oldRF_PROGRAMMERVERSION    1

#define BUFBITS (1<<17)

package oldRF[oldrf]  {

classprocedures:

	WriteRow(FILE *file,  unsigned char *byteaddr,  long nbytes);
		/* write to 'file' the oldRF form representation of the 'nbytes'
		    bytes starting at location 'byteaddr' */
	
	ReadRow(FILE *file,  unsigned char *row,  long length) returns long;
		/* Reads from 'file' the encoding of bytes to fill in 'row'.  Row will be
		    truncated or padded (with WHITE) to exactly 'length' bytes.
		*/
	ReadImage(FILE *file, struct pixelimage *pix) returns long;
		/* Read a raster image from 'file' and put it in 'pix' 
			return error code */
	WriteImage(FILE *file, struct pixelimage *pix, struct rectangle *sub);
		/* Write a raster image to 'file' from 'pix'  */

};

