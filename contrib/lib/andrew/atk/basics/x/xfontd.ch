/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1991 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xfontd.ch,v 1.7 1991/09/12 19:24:40 bobg Exp $ */
/* $ACIS:fontdesc.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xfontd.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxfontdesc_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R5/ftp/src/atk/basics/x/RCS/xfontd.ch,v 1.7 1991/09/12 19:24:40 bobg Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


class xfontdesc[xfontd] : fontdesc {
overrides:

	CvtCharToGraphic(struct graphic *gr, char SpecialChar) returns struct graphic *;
	GetRealFontDesc(struct graphic *gr) returns struct font *;

	StringSize(struct graphic *gr, char * string, long * XWidth, long * YWidth) returns long;
	TextSize(struct graphic *gr, char * text, long TextLength, long *XWidth, long *YWidth)
		returns long;
	WidthTable(struct graphic *gr) returns  short*; /* actually an array of short's */
	HeightTable(struct graphic *gr) returns short *; /* actually an array of short's */
	CharSummary(struct graphic *gr, char LookUpChar, 
			struct fontdesc_charInfo * returnedInfo);

classprocedures:
	InitializeObject(struct fontdesc * FontDescObject) returns boolean;
	FinalizeObject(struct fontdesc * FontDescObject);
	InitializeClass() returns boolean;
        Allocate() returns struct xfontdesc *;
        Deallocate(struct xfontdesc *);

};
