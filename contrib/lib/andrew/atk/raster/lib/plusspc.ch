/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/plusspc.ch,v 2.6 1991/09/12 19:46:59 bobg Exp $ */
/* $ACIS:plusspc.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/plusspc.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidplusspace_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/plusspc.ch,v 2.6 1991/09/12 19:46:59 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/*  plusspc.H

	plusspace package

	Routines for reading and writing rasters in 'plus-space' form
	(as used for the first BE2 rasters code)

	The data stream is packed with the same scheme as used for MacPaint
	Then bytes are split to nibbles and written by adding ' ' to each nibble.

 */

#define  plusspace_PROGRAMMERVERSION    1

package plusspace[plusspc] {

classprocedures:

	WriteRow(FILE *file,  unsigned char *byteaddr,  long nbytes);
		/* write to 'file' the plusspace form representation of the 'nbytes'
		    bytes starting at location 'byteaddr' */
	
	ReadRow(FILE *file,  unsigned char *row,  long length) returns long;
		/* Reads from 'file' the encoding of bytes to fill in 'row'.  Row will be
		    truncated or padded (with WHITE) to exactly 'length' bytes.
		*/

};

