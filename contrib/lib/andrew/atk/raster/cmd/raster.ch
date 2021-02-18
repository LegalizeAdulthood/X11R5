/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/raster.ch,v 2.9 1991/09/12 19:46:11 bobg Exp $ */
/* $ACIS:raster.ch 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/raster.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidraster_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/raster.ch,v 2.9 1991/09/12 19:46:11 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/*

	A raster  refers to a rasterimage containing the raw bits.  It can specify
	scaling, cropping, reflection, rotation, and inversion of the image from the raw bits.

	For a description of the data stream, see itc/be2/raster/rasterspec.d
*/

#include <rect.h>

#include <rastimg.ih>	/* because some macros call routines from it */

#define  raster_PROGRAMMERVERSION    1

/* status values for notify observer */
#define  raster_BITSCHANGED    2  /* the parameters in the
					struct raster have changed */
#define  raster_BOUNDSCHANGED  3  /* the raster is viewing a different
					part of the rasterimage */

class raster : dataobject[dataobj]  {

overrides:

	Read(/* struct raster *self, */ FILE *file, long id)returns long;
	Write(/* struct raster *self, */ FILE *file, long id, long level)
			returns long;
	ObservedChanged(/* struct raster *self, */ struct rasterimage *pix, long status);
	SetAttributes( struct attributes *attributes );

methods:

	SetPix(/* struct raster *self, */ struct rasterimage *pix);
		/* changes to refer to a new rasterimage;  handles observing */
	Resize(/* struct raster *self, */ long width, long height);
		/* changes the size of the raster;  resets subraster to whole */
	ReadSubRaster(/* struct raster *self, */ FILE *file, struct rectangle *r)
				returns long;
		/* read from file into the subraster with upper left pixel at (x, y) */
	WriteSubRaster(/* struct raster *self, */ FILE *file, long objectid, 
			struct rectangle *subraster)  returns long;
		/* write the subraster at (x,y) with given width and height to file */
	WriteShare(/* struct raster *self, */ FILE *file, struct rectangle *subraster);
		/* write a "share" record for the indicated 'subraster' of 'self' */

macromethods:

	Clear()		(rasterimage_Clear(self->pix))
	GetWidth()	(rasterimage_GetWidth(self->pix))
	GetHeight()	(rasterimage_GetHeight(self->pix))
	GetPix()		(self->pix)

classprocedures:

	InitializeObject(/* struct classhdr *ClassID, */ struct raster *self) returns boolean;
	FinalizeObject(/* struct classhdr *ClassID, */ struct raster *self);
	Create(/* struct classhdr *ClassID, */ long width, long height)
			returns struct raster *;
		/* creates a raster object with a rasterimage having
		space for a raster of the given width and height */

data:

	struct rasterimage *pix;	/* the image storage */
	boolean	readOnly;
	struct rectangle subraster;	/* choose subraster of rasterimage */

	/* the raster may be a negative image or reflection of the bits in the
	    underlying raster: */
	char  options;
		/* bits of 'option' value */
#define  raster_INVERT	(1<<0)	/* exchange black and white */
#define  raster_FLIP	(1<<1)	/* exch top and bottom */
#define  raster_FLOP	(1<<2)	/* exch left and right */
#define  raster_ROTATE	(1<<3)	/* rotate 90 clockwise */

#define raster_UNITSCALE  136533	 /*0x20000 print at nominal screen size */
	/*  default print scaling is 0x10000 which means 1/2 screen size */

	long xScale, yScale;	/* scaling for printing */
};

