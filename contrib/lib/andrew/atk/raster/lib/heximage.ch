/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/heximage.ch,v 2.5 1991/09/12 19:46:46 bobg Exp $ */
/* $ACIS:heximage.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/heximage.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidheximage_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/heximage.ch,v 2.5 1991/09/12 19:46:46 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*  heximage.H

	heximage package

	Routines for reading and writing rasters in hexadecimal
	(as required for readhexstring in postscript)


 */

#include <pixelimg.ih>

#define  heximage_PROGRAMMERVERSION    1
#define BUFBITS (1<<17)


package heximage  {

classprocedures:

	WriteRow(FILE *file,  unsigned char *byteaddr,  long nbytes, boolean invert);
		/* write to 'file' the heximage form representation of the 'nbytes'
		    bytes starting at location 'byteaddr' 
		    invert the image if 'invert' is true */
	
	ReadRow(FILE *file,  unsigned char *row,  long length) returns long;
		/* Reads from 'file' the encoding of bytes to fill in 'row'.  Row will be
		    truncated or padded (with WHITE) to exactly 'length' bytes.
		*/
	ReadImage(FILE *file, struct pixelimage *pix, long width, long height) returns long;
		/* Read a raster image from 'file' and put it in 'pix' 
			The width and height must be supplied 
			and will be stored in the pix.
			return error code */
	WriteImage(FILE *file, struct pixelimage *pix, struct rectangle *sub);
		/* Write a raster image to 'file' from 'pix'  */

	WritePostscript(FILE *file, struct pixelimage *pix, struct rectangle *sub,
			double xfrac, double yfrac);
		/* write to 'file' the postscript code 
			to print the subraster 'sub' of 'pix' */

};

