 /* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xwdio.ch,v 1.3 1991/09/12 19:47:16 bobg Exp $ */
/* $ACIS:xwdio.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xwdio.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxwdio_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xwdio.ch,v 1.3 1991/09/12 19:47:16 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*  xwdio.ch

        xwdio package

        Routines for reading and writing rasters in X Window Dump (V7)
        as defined by XWDFile.h

 */

#include <pixelimg.ih>

#define  xwdio_PROGRAMMERVERSION    1

#define BUFBITS (1<<17)

package xwdio[xwdio]  {

    classprocedures:
      WriteRow(FILE *file,  unsigned char *byteaddr,  long nbytes);
    /* Write 'nbytes' bytes starting at 'byteaddr'
      into 'file' in X Window Dump image format,
     after the header is written. */

    ReadRow(FILE *file,  unsigned char *row,  long nbytes) returns long;
    /* Read 'nbytes' bytes from 'file' into
      positions starting at 'row', after the
      header is read; there is usually some
	padding in the last byte of each row,
      but the image is resized after all rows
      are read to remove it. */
    ReadImage(FILE *file, struct pixelimage *pix) returns long;
    /* Read an X Window Dump  from 'file' and
      put it in 'pix,' return error code */
    WriteImage(FILE *file, struct pixelimage *pix, struct rectangle *sub);
    /* Write an X Window Dump  to 'file' from  subrectangle 'sub' of pixelimage 'pix'  */

};

