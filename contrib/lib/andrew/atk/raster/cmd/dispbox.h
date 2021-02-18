/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/dispbox.h,v 2.5 1991/09/12 19:46:07 bobg Exp $ */
/* $ACIS:rect.h 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/dispbox.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrectangle = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/raster/cmd/RCS/dispbox.h,v 2.5 1991/09/12 19:46:07 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

extern boolean RastersInitiallyShrunk;
extern char debug;

#define Debug(s) {printf s ; fflush(stdout);}
#define DEBUG(s) {if (debug) {printf s ; fflush(stdout);}}
#define ENTER(r) DEBUG(("Enter %s(0x%lx)\n", "r", self))
#define LEAVE(r) DEBUG(("Leave %s(0x%lx)\n", "r", self))

#define DisplayAndReturn(self, String) {message_DisplayString(self, 0, String); return;}

#define AskOrCancel(self, string, buf) \
	{if (message_AskForString(self, 0, string, "", buf, sizeof buf - 1) < 0) \
	{message_DisplayString(self, 0, "Cancelled."); \
	return;}}

#define ClipAndWritePixImage(clipw, cliph, G, x, y, pix, x1, y1, w, h) {	\
if ((x1) < (clipw) && (y1) < (cliph)) {						\
long width = ((x1) + (w) > (clipw)) ? (clipw) - (x1) : (w);			\
long height = ((y1) + (h) > (cliph)) ? (cliph) - (y1) : (h);			\
if (width > 0 && height > 0)							\
graphic_WritePixImage((G), (x), (y), (pix), (x1), (y1), width, height); } }

#define DrawHighlight(self, Graphic, Rect, OuterBorder, InnerBorder)		\
{										\
    rectangle_SetRectSize(&(Rect),						\
			   rectangle_Left(&(Rect)) - (self)->Xoff - BORDER,	\
			   rectangle_Top(&(Rect)) - (self)->Yoff - BORDER,	\
			   rectangle_Width(&(Rect)) + TWOBORDER - 1,		\
			   rectangle_Height(&(Rect)) + TWOBORDER - 1);		\
    rasterview_SetTransferMode((self), (OuterBorder));				\
    graphic_DrawRect((Graphic), &(Rect));					\
    if ((InnerBorder) >= 0) {							\
	rasterview_SetTransferMode((self), (InnerBorder));			\
	InsetRect(&(Rect), 1, 1);						\
	graphic_DrawRect((Graphic), &(Rect));					\
	rectangle_SetRectSize(&(Rect),						\
				rectangle_Left(&(Rect)) + (self)->Xoff + BORDER - 1,	\
				rectangle_Top(&(Rect)) + (self)->Yoff + BORDER - 1,	\
				rectangle_Width(&(Rect)) - TWOBORDER + 3,		\
				rectangle_Height(&(Rect)) - TWOBORDER + 3);		\
    }										\
    else {									\
	rectangle_SetRectSize(&(Rect),						\
				rectangle_Left(&(Rect)) + (self)->Xoff + BORDER,\
				rectangle_Top(&(Rect)) + (self)->Yoff + BORDER,	\
				rectangle_Width(&(Rect)) - TWOBORDER + 1,	\
				rectangle_Height(&(Rect)) - TWOBORDER + 1);	\
    }										\
}

#define DrawHighlightScreenCoordinates(self, Graphic, Rect, OuterBorder, InnerBorder)	\
{										\
    rectangle_SetRectSize(&(Rect),						\
			   rectangle_Left(&(Rect)) - BORDER,			\
			   rectangle_Top(&(Rect)) - BORDER,			\
			   rectangle_Width(&(Rect)) + TWOBORDER - 1,		\
			   rectangle_Height(&(Rect)) + TWOBORDER - 1);		\
    rasterview_SetTransferMode((self), (OuterBorder));				\
    graphic_DrawRect((Graphic), &(Rect));					\
    if ((InnerBorder) >= 0) {							\
	rasterview_SetTransferMode((self), (InnerBorder));			\
	InsetRect(&(Rect), 1, 1);						\
	graphic_DrawRect((Graphic), &(Rect));					\
	rectangle_SetRectSize(&(Rect),						\
				rectangle_Left(&(Rect)) + BORDER - 1,		\
				rectangle_Top(&(Rect)) + BORDER - 1,		\
				rectangle_Width(&(Rect)) - TWOBORDER + 3,	\
				rectangle_Height(&(Rect)) - TWOBORDER + 3);	\
    }										\
    else {									\
	rectangle_SetRectSize(&(Rect),						\
				rectangle_Left(&(Rect)) + BORDER,		\
				rectangle_Top(&(Rect)) + BORDER,		\
				rectangle_Width(&(Rect)) - TWOBORDER + 1,	\
				rectangle_Height(&(Rect)) - TWOBORDER + 1);	\
    }										\
}

#define DrawHighlightBlackAndWhite(self, G, R) \
	DrawHighlight((self), (G), (R), graphic_BLACK, graphic_WHITE);
#define DrawHighlightWhite(self, G, R) \
	DrawHighlight((self), (G), (R), graphic_WHITE, graphic_WHITE);
#define DrawHighlightBlack(self, G, R) \
	DrawHighlight((self), (G), (R), graphic_BLACK, graphic_BLACK);


void DisplayBoxDrawPanHighlight(/* self */);

void DisplayBoxWritePixImage(/* self, G */);

void DisplayBoxWritePixImageFull(/* self, G, pix */);

void DisplayBoxDrawHighlight(/* self, G */);

void DisplayBoxDrawHighlightGray(/* self, G */);

void DisplayBoxHideHighlight(/* self */);

void UpdateDisplayBox(/* self, pix */);


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - *\
  *
  *	Rectangle Macroizations
  *	
  \* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define IsEmptyRect(TestRect) (TestRect)->width <= 0 || (TestRect)->height <= 0

#define IsNotEmptyRect(TestRect) (TestRect)->width > 0 && (TestRect)->height > 0

#define InsetRect(r, deltax, deltay) rectangle_SetRectSize((r), rectangle_Left((r)) + (deltax), rectangle_Top((r)) + (deltay), rectangle_Width((r)) - 2*(deltax), rectangle_Height((r)) - 2*(deltay));

#define OffsetRect(r, deltax, deltay) rectangle_SetRectSize((r), rectangle_Left((r)) + (deltax), rectangle_Top((r)) + (deltay), rectangle_Width((r)), rectangle_Height((r)));

#define IsEnclosedBy(a, b)				\
(rectangle_Left((b)) <= rectangle_Left((a))		\
 && rectangle_Top((b)) <= rectangle_Top((a))		\
 && rectangle_Right((b)) >= rectangle_Right((a))	\
 && rectangle_Bottom((b)) >= rectangle_Bottom((a)))

#define IsExclusivelyEnclosedBy(a, b)			\
(rectangle_Left((b)) < rectangle_Left((a))		\
 && rectangle_Top((b)) < rectangle_Top((a))		\
 && rectangle_Right((b)) > rectangle_Right((a))		\
 && rectangle_Bottom((b)) > rectangle_Bottom((a)))

#define SetLeftRect(r, l) (r)->left = (l);
#define SetTopRect(r, t) (r)->top = (t);
#define SetWidthRect(r, w) (r)->width = (w);
#define SetHeightRect(r, h) (r)->height = (h);
