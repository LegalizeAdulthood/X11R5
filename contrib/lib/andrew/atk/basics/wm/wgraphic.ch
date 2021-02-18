/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wgraphic.ch,v 1.8 1991/09/12 19:24:18 bobg Exp $ */
/* $ACIS:graphic.ch 1.4$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wgraphic.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidgraphic_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/wm/RCS/wgraphic.ch,v 1.8 1991/09/12 19:24:18 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#include <fontdesc.ih>
#include <point.h>
#include <rect.h>

#define wmgraphic_PROGRAMMERVERSION	1



#define graphic_ConvertX(to, oldx)  \
    ((oldx) - ((struct graphic *) (to))->enclosedOrigin.x)
#define graphic_ConvertY(to, oldy)  \
    ((oldy) - ((struct graphic *) (to))->enclosedOrigin.y)

#define graphic_OutputX(self, oldx)  \
    ((oldx) - ((struct graphic *)(self))->visualBounds.left + (self)->header.pixmap.physicalOrigin.x)
#define graphic_OutputY(self, oldy)  \
    ((oldy) - ((struct graphic *)(self))->visualBounds.top + (self)->header.pixmap.physicalOrigin.y)



class wmgraphic[wgraphic] : graphic {
overrides:
    MoveTo(long NewX, long NewY);
    Move(long DeltaX, long DeltaY);

    DrawLineTo(long XEnd, long YEnd);
    DrawLine(long DeltaX, long DeltaY);

    DrawString(char * Text, short Operation);
    DrawText(char * Text, long TextLength, short Operation);
    SetSpaceShim(short Amount);
    SetFont(struct fontdesc * ChosenFont);

    DrawRectSize(long x,long y,long width,long height);
    DrawPolygon(struct point * PointArray, short PointCount);
    DrawPath(struct point * PointArray, short PointCount);
    DrawOvalSize(long x,long y,long width,long height);
    DrawArcSize(long x,long y,long width,long height, short StartAngle, short OffsetAngle);
    DrawRRectSize(long x,long y,long width,long height, long cornerWidth,long cornerHeight);

    FillRect(struct rectangle * Rect, struct graphic * Tile);
    FillRectSize(long x,long y,long width,long height, struct graphic * Tile);
    FillPolygon(struct point * PointArray, short PointCount, struct graphic * Tile);
    FillOvalSize(long x,long y,long width,long height, struct graphic * Tile);
    FillArcSize(long x,long y,long width,long height, 
		short StartAngle, short OffsetAngle, struct graphic * Tile);
    FillRRectSize(long x, long y, long width, long height, 
		long cornerWidth, long cornerHeight, struct graphic * Tile);
    FillRgn(region * Rgn, struct graphic * Tile);
    FillTrapezoid(long topX, long topY, long topWidth, 
		long bottomX, long bottomY, long bottomWidth, struct graphic * Tile);

    BitBlt(struct rectangle * SrcRect, 
		struct graphic *DestGraphic, struct point * DstOrigin, 
		struct rectangle * ClipRectList);
    WritePixImage(/* struct wmgraphic *self, */ long DestX, long DestY, 
		struct pixelimage *SrcPixels, long SrcX, long SrcY, 
		long width, long height);
		/* transfers a pixelimage to the graphic */
    ReadPixImage(/* struct wmgraphic *self, */ long SrcX, long SrcY, 
		struct pixelimage *DestPixels, long DestX, long DestY, 
		long width, long height);
		/* transfers a piece of the graphic to a piece of the pixelimage */

    InsertGraphic(struct graphic * EnclosingGraphic, struct rectangle * EnclosedRectangle);

    SetClippingRect(struct rectangle * AdditionalRect);
    ClearClippingRect();

    SetTransferMode(short NewTransferMode);

    RestoreGraphicsState();
    FlushGraphics();

    WhitePattern() returns struct graphic *;
    BlackPattern() returns struct graphic *;
    GrayPattern(short IntensityNum, IntensityDenom) returns struct graphic *;

    SetForegroundColor(char *colorName, long red, long green, long blue);
    SetBackgroundColor(char *colorName, long red, long green, long blue);

    GetHorizontalResolution() returns long;
    GetVerticalResolution() returns long;
    GetWindowManagerType() returns char *;
    GetDevice() returns long;
classprocedures:
    ClearCache(struct wm_window * wPtr);
data:
    struct wm_window * window;
    /* Special kludge for performing fills with characters */
    boolean altPixMapUsed;
    struct fontdesc * fillFont;
    char fillChar;
    short forePixel;
    short backPixel;
};
