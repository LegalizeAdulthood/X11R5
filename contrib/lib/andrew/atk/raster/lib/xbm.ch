 /*  ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xbm.ch,v 1.2 1991/09/12 19:47:12 bobg Exp $ */
/* $ACIS:xbm.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xbm.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxbm_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/lib/RCS/xbm.ch,v 1.2 1991/09/12 19:47:12 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */
/*  xbm.H

        xbm package

        Routines for reading and writing rasters in X Bitmap format.


 */



#include <pixelimg.ih>

#define  xbm_PROGRAMMERVERSION    1

#define BUFBITS (1<<17)

package xbm[xbm]  {

    classprocedures:

    ReadImage(FILE *file, struct pixelimage *pix) returns long;
    /* Read an X Window Dump  from 'file' and put it in 'pix,'
      return error code */
    WriteImage(FILE *file, struct pixelimage *pix, struct rectangle *sub);
    /* Write an X Window Dump  to 'file' from  subrectangle 'sub' of pixelimage 'pix'  */

};

